/*
*	Author: Jesse Tucker
*	Date: December 30th 2012
*	Name: Stts.hpp
*
*	Description: Defines the STTS box. Definition found in ISO 14496-12 standard, page 32.
*	This is the Decoding Time to Sample Box. It contains information on the time to sample
*	map.
*/

#include "pch.h"

#include "Stts.hpp"
#include "MPEG4_Parser.hpp"
#include "InvalidValueException.hpp"

namespace Arcusical {
namespace MPEG4 {

Stts::Stts() : m_version(0) {
  // nothing to do
}

Stts::~Stts() {}

void Stts::ReadContents(Util::Stream& stream) {
  m_version = stream.ReadInteger<uint32_t>();
  uint32_t numEntries = stream.ReadInteger<uint32_t>();

  // now parse to the end of the box
  auto bytesLeft = m_bodySize - 2 * sizeof(uint32_t);
  while (bytesLeft > 0) {
    SampleEntry entry;

    entry.m_sampleCount = stream.ReadInteger<uint32_t>();
    entry.m_sampleDelta = stream.ReadInteger<uint32_t>();

    m_entries.push_back(entry);
    bytesLeft -= 2 * sizeof(uint32_t);
  }

  ARC_ASSERT_MSG(bytesLeft == 0, "Read more entries than there were bytes!");

  // verify count
  if (numEntries != m_entries.size()) {
    throw InvalidValueException("The entries in the table do no match up with the reported amount!");
  }
}

void Stts::PrintBox(std::ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "STTS Box:" << std::endl;

  outStream << tabs << "\tVersion: " << m_version << std::endl;
  outStream << tabs << "\tEntry Count: " << m_entries.size() << std::endl;

  // I would rather use an iterator... but having the count is useful
  for (unsigned int index = 0; index < m_entries.size(); ++index) {
    outStream << tabs << "\tEntry " << index << ":" << std::endl;
    outStream << tabs << "\t\tSample Count: " << m_entries[index].m_sampleCount << std::endl;
    outStream << tabs << "\t\tSample Delta: " << m_entries[index].m_sampleDelta << std::endl;
  }
}

#pragma region Public Getters

uint32_t Stts::GetVersion() { return m_version; }

std::vector<SampleEntry> Stts::GetEntries() { return m_entries; }

#pragma endregion

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/