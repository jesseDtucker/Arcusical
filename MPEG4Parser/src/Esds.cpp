#include "pch.h"

#include <limits>

#include "Arc_Assert.hpp"
#include "Esds.hpp"
#include "MPEG4_Parser.hpp"

/*
*	Author: Jesse Tucker
*	Date: December 30th 2012
*	Name: Esds.hpp
*
*	Description: box contained within the Mp4a box. Contains information
*	regarding the encoding.
*/

using namespace std;

namespace Arcusical {
namespace MPEG4 {

Esds::Esds() {}
Esds::~Esds() {}

void Esds::ReadContents(Util::Stream& stream) {
  m_version = stream.ReadInteger<uint8_t>();
  m_flags = stream.ReadInteger<uint32_t, 3>();

  // now each section is a tag segment
  ParseTags(stream, Util::SafeIntCast<unsigned int>(m_bodySize) - sizeof(uint8_t) - 3);
}

void Esds::PrintBox(ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "ESDS Box: " << std::endl;
  outStream << tabs << "\tVersion: " << static_cast<int>(m_version) << std::endl;  // cast is to ensure correct display
  outStream << tabs << "\tFlags: " << m_flags << std::endl;

  // now print each tag
  if (m_esDescription != nullptr) {
    outStream << tabs << *m_esDescription << std::endl;
  }
  if (m_decoderConfigDescription != nullptr) {
    outStream << tabs << *m_decoderConfigDescription << std::endl;
  }

  // I have no idea what these tags actually are so do not try to print them.
  // outStream << tabs << *m_decoderSpecificInfo << std::endl;
  // outStream << tabs << *m_slConfigDescription << std::endl;
}

void Esds::ParseTags(Util::Stream& stream, int bytesToRead) {
  stream.Advance(bytesToRead);
  /*
  while (bytesToRead > 0)
  {
          //get the tag
          ES_Tag tag = ReadTag(stream);
          uint32_t size = ReadSize(stream);

          switch (tag)
          {
          case Arcusical::MPEG4::ES_Tag::UNKNOWN:
                  ReadUnknownTagSection(stream, size);
                  break;
          case Arcusical::MPEG4::ES_Tag::ES_DESC:
                  ReadEsDescription(stream);
                  break;
          case Arcusical::MPEG4::ES_Tag::DECODER_CONFIG:
                  ReadDecoderConfigDescription(stream);
                  break;
          case Arcusical::MPEG4::ES_Tag::DECODER_INFO:
                  ReadDecoderSpecificInfo(stream, size);
                  break;
          case Arcusical::MPEG4::ES_Tag::SL_CONFIG_DESC:
                  ReadSlConfigDescription(stream, size);
                  break;
          }

          bytesToRead -= size;
  }
  */
  // hax
  // ARC_ASSERT_MSG(bytesToRead == 0, "Esds box read too far! It should read precisly the number of bytes it
  // requires!");
}

#pragma region Protected Readers

ES_Tag Esds::ReadTag(Util::Stream& stream) {
  ES_Tag value;
  auto tag = stream.ReadInteger<uint8_t>();
  auto valueItr = CharToESTag.find(tag);

  if (valueItr != CharToESTag.end()) {
    value = valueItr->second;
  } else {
    value = ES_Tag::UNKNOWN;
  }

  return value;
}

uint32_t Esds::ReadSize(Util::Stream& stream) {
  // size is stored in 7 bit segments ranging from 1-4 bytes
  int byteCount = 0;
  char val = 0;
  uint32_t result = 0;

  do {
    val = stream.ReadInteger<char>();
    ++byteCount;
    result = (result << 7) | (val & 0x7F);  // append the lower 7 bits
  } while ((byteCount < 4) && ((val & 0x80) != 0));

  return result;
}

void Esds::ReadEsDescription(Util::Stream& stream) {
  m_esDescription = make_shared<ES_Description>();

  m_esDescription->m_esID = stream.ReadInteger<uint16_t>();
  m_esDescription->m_streamPriority = stream.ReadInteger<uint8_t>();
}

void Esds::ReadDecoderConfigDescription(Util::Stream& stream) {
  m_decoderConfigDescription = make_shared<DecoderConfigDescription>();

  m_decoderConfigDescription->m_objectTypeID = stream.ReadInteger<uint8_t>();
  m_decoderConfigDescription->m_streamType = stream.ReadInteger<uint8_t>();
  m_decoderConfigDescription->m_bufferSize = stream.ReadInteger<uint32_t, 3>();
  m_decoderConfigDescription->m_maxBitRate = stream.ReadInteger<uint32_t>();
  m_decoderConfigDescription->m_averageBitRate = stream.ReadInteger<uint32_t>();
}

void Esds::ReadDecoderSpecificInfo(Util::Stream& stream, uint32_t size) {
  m_decoderSpecificInfo = make_shared<DecoderSpecificInfo>();

  stream.ReadRawBytes(size);
}

void Esds::ReadSlConfigDescription(Util::Stream& stream, uint32_t size) {
  m_slConfigDescription = make_shared<SL_ConfigDescription>();

  stream.ReadRawBytes(size);
}

void Esds::ReadUnknownTagSection(Util::Stream& stream, uint32_t size) {
  UnknownTagSection section;

  stream.ReadRawBytes(size);

  m_unknown.push_back(section);
}

#pragma region Tag Printing

std::ostream& operator<<(std::ostream& outStream, ES_Description& descriptor) {
  outStream << " esID: " << descriptor.m_esID;
  outStream << " Stream Priority: " << static_cast<int>(descriptor.m_streamPriority);  // cast ensures correct display

  return outStream;
}

std::ostream& operator<<(std::ostream& outStream, DecoderConfigDescription& configDescriptor) {
  outStream << " Object ID: " << static_cast<int>(configDescriptor.m_objectTypeID);  // cast ensures correct display
  outStream << " Stream Type: " << static_cast<int>(configDescriptor.m_streamType);  // cast is to ensure correct
                                                                                     // display
  outStream << " Buffer Size: " << configDescriptor.m_bufferSize;
  outStream << " Max Bit Rate: " << configDescriptor.m_maxBitRate;
  outStream << " Average Bit Rate: " << configDescriptor.m_averageBitRate;

  return outStream;
}

#pragma endregion

#pragma endregion

#pragma region Public Getters

uint8_t Esds::GetVersion() { return m_version; }

uint32_t Esds::GetFlags() { return m_flags; }

std::shared_ptr<ES_Description> Esds::GetES_Description() { return m_esDescription; }

std::shared_ptr<DecoderConfigDescription> Esds::GetDecoderConfigDescription() { return m_decoderConfigDescription; }

std::shared_ptr<DecoderSpecificInfo> Esds::GetDecoderSpecificInfo() { return m_decoderSpecificInfo; }

std::shared_ptr<SL_ConfigDescription> Esds::GetSlConfigDescription() { return m_slConfigDescription; }

#pragma endregion

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/