/*
*	Author: Jesse Tucker
*	Date: December 30th 2012
*	Name: Stsc.hpp
*
*	Description: The STSC box is defined on page 40 of the
*	ISO 14496-12 standard. It is the Sample to chunk box.
*	It defines the number of samples in each chunk.
*/

#include "pch.h"

#include "Stsc.hpp"
#include "MPEG4_Parser.hpp"
#include "InvalidValueException.hpp"

namespace Arcusical {
namespace MPEG4 {

Stsc::Stsc() {}

Stsc::~Stsc() {}

void Stsc::ReadContents(Util::Stream& stream) {
  m_version = stream.ReadInteger<uint32_t>();
  uint32_t numEntries = stream.ReadInteger<uint32_t>();

  // read the entries
  //
  auto bytesLeft = m_bodySize - 2 * sizeof(uint32_t);
  while (bytesLeft > 0) {
    SampleToChunkEntry entry;
    entry.m_firstChunk = stream.ReadInteger<uint32_t>();
    entry.m_samplePerChunk = stream.ReadInteger<uint32_t>();
    entry.m_sampleDescriptionIndex = stream.ReadInteger<uint32_t>();

    m_entries.push_back(entry);

    bytesLeft -= 3 * sizeof(uint32_t);
  }

  ARC_ASSERT_MSG(bytesLeft == 0, "Read more entries than the number of bytes indicates!");

  // verify the count
  if (m_entries.size() != numEntries) {
    throw InvalidValueException("The number of entries does not match the recorded amount!");
  }
}

void Stsc::PrintBox(std::ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "STSC Box:" << std::endl;

  outStream << tabs << "\tVersion: " << m_version << std::endl;
  outStream << tabs << "\tEntry count: " << m_entries.size() << std::endl;

  for (unsigned int index = 0; index < m_entries.size(); ++index) {
    outStream << tabs << "\tEntry " << index << ":" << std::endl;
    outStream << tabs << "\t\tFirst Chunk: " << m_entries[index].m_firstChunk << std::endl;
    outStream << tabs << "\t\tSamples per Chunk: " << m_entries[index].m_samplePerChunk << std::endl;
    outStream << tabs << "\t\tSample Description Index: " << m_entries[index].m_sampleDescriptionIndex << std::endl;
  }
}

#pragma region Public Getters

uint32_t Stsc::GetVersion() { return m_version; }

std::vector<SampleToChunkEntry> Stsc::GetEntries() { return m_entries; }

#pragma endregion

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/