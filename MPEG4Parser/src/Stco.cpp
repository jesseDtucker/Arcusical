/*
*	Author: Jesse Tucker
*	Date: December 30th 2012
*	Name: Stsz.hpp
*
*	Description: The is the Sample Size Box. There are 2 variations of this box.
*	This box is defined on page 41 of the ISO 14496-12 standard. The first variation
*	(which is the version implemented in this header file) is STCO and records the
*	values in 32 bits, the other version (CO64) uses 64 bit values.
*
*	This defines the offset table that gives the index of each chunk
*	in the containing file.
*/

#include "pch.h"

#include "InvalidValueException.hpp"
#include "MPEG4_Parser.hpp"
#include "Stco.hpp"

namespace Arcusical {
namespace MPEG4 {

Stco::Stco() : m_version(0) {}

Stco::~Stco() {}

void Stco::ReadContents(Util::Stream& stream) {
  m_version = stream.ReadInteger<uint32_t>();

  uint32_t numEntries = stream.ReadInteger<uint32_t>();

  m_entries.reserve(numEntries);

  // times 2 because we have just read in 2 32 bit numbers
  auto bytesLeft = m_bodySize - 2 * sizeof(uint32_t);
  while (bytesLeft > 0) {
    m_entries.push_back(stream.ReadInteger<uint32_t>());
    bytesLeft -= sizeof(uint32_t);
  }

  ARC_ASSERT_MSG(bytesLeft == 0, "Read more entries than the number of bytes available!");

  if (numEntries != m_entries.size()) {
    throw InvalidValueException("The number of table entries does not match the size of the table!");
  }
}

void Stco::PrintBox(std::ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "STCO Box:" << std::endl;

  outStream << tabs << "\tVersion: " << m_version << std::endl;
  outStream << tabs << "\tNumber of Entries: " << m_entries.size() << std::endl;

  int entryNumber = 0;
  for (auto& entry : m_entries) {
    outStream << tabs << "\t\tEntry " << entryNumber << " : " << entry << std::endl;
    if (entryNumber > 0) {
      outStream << tabs << "\t\t\tDelta : " << entry - m_entries[entryNumber - 1] << std::endl;
    }
    entryNumber++;
  }
}

#pragma region Public Getters

uint32_t Stco::GetVersion() { return m_version; }

std::vector<uint32_t> Stco::GetEntries() { return m_entries; }

#pragma endregion

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/