#include "pch.h"

#include "Mp4a.hpp"

#include "MPEG4_Parser.hpp"
#include "Stream.hpp"

/*
*	Author: Jesse Tucker
*	Date: December 30th 2012
*	Name: Mp4a.hpp
*
*	Description: Audio Sample Entry box. Contains information on the encoding
*	of the audio as it is specific to the Mp4a atom. This atom usually contains
*	information for tracks encoded in either AAC or MP3.
*/

namespace Arcusical {
namespace MPEG4 {

Mp4a::Mp4a() : m_dataReferenceIndex(0), m_channelCount(0), m_sampleSize(0), m_sampleRate(0) {
  // done
}

Mp4a::~Mp4a() {}

void Mp4a::ReadContents(Util::Stream& stream) {
  auto startPosition = stream.GetPosition();

  // first 6 bytes are reserved
  stream.Advance(6);
  m_dataReferenceIndex = stream.ReadInteger<uint16_t>();

  // next two 32 bit integers reserved
  stream.Advance(8);
  m_channelCount = stream.ReadInteger<uint16_t>();
  m_sampleSize = stream.ReadInteger<uint16_t>();
  // next 4 bytes are zero
  stream.Advance(4);
  m_sampleRate = stream.ReadFixedPoint<uint32_t>(16);

  auto childBytes = m_bodySize - (stream.GetPosition() - startPosition);
  m_children = MPEG4_Parser::ParseBoxes(stream, childBytes);
}

void Mp4a::PrintBox(std::ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "MP4A Box:" << std::endl;

  outStream << tabs << "\tData Reference Index: " << m_dataReferenceIndex << std::endl;
  outStream << tabs << "\tChannel Count: " << m_channelCount << std::endl;
  outStream << tabs << "\tSample Size: " << m_sampleSize << std::endl;
  outStream << tabs << "\tSample Rate: " << m_sampleRate << std::endl;

  for (std::shared_ptr<Box> child : m_children) {
    child->PrintBox(outStream, depth + 1);
  }
}

#pragma region Public Getters

uint16_t Mp4a::GetDataReferenceIndex() { return m_dataReferenceIndex; }

uint16_t Mp4a::GetChannelCount() { return m_channelCount; }

uint16_t Mp4a::GetSampleSize() { return m_sampleSize; }

float Mp4a::GetSampleRate() { return m_sampleRate; }

#pragma endregion

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/