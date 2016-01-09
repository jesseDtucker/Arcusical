#include "pch.h"

#include "InvalidValueException.hpp"
#include "MPEG4_Parser.hpp"
#include "Stsz.hpp"
/*
*	Author: Jesse Tucker
*	Date: December 30th 2012
*	Name: Stsz.hpp
*
*	Description: The is the Sample Size Box. There are 2 variations of this box.
*	This box is defined on page 39 of the ISO 14496-12 standard. This contains
*	either a value or a table of values that define the sample sizes.
*/

namespace Arcusical {
namespace MPEG4 {

Stsz::Stsz() : m_version(0), m_sampleSize(0), m_sampleCount(0) {}

Stsz::~Stsz() {}

void Stsz::ReadContents(Util::Stream& stream) {
  auto startPosition = stream.GetPosition();

  m_version = stream.ReadInteger<uint32_t>();
  m_sampleSize = stream.ReadInteger<uint32_t>();
  m_sampleCount = stream.ReadInteger<uint32_t>();

  if (m_sampleSize == 0) {
    m_entries.reserve(m_sampleCount);

    auto bytesLeft = m_bodySize - (stream.GetPosition() - startPosition);

    while (bytesLeft > 0) {
      // parse entries and add them
      m_entries.push_back(stream.ReadInteger<uint32_t>());
      bytesLeft -= sizeof(uint32_t);
    }

    ARC_ASSERT_MSG(bytesLeft == 0, "Read a different number of entries as the bytes would indicate!");
  }

  // now check that the counts are equal
  if (m_sampleCount != m_entries.size()) {
    throw InvalidValueException("The count in the table and the count reported are not equal!");
  }
}

void Stsz::PrintBox(std::ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "STSZ Box:" << std::endl;

  outStream << tabs << "\tVersion: " << m_version << std::endl;
  outStream << tabs << "\tSampleSize: " << m_version << std::endl;
  outStream << tabs << "\tSampleCount: " << m_version << std::endl;
}

#pragma region Public Getters

uint32_t Stsz::GetVersion() { return m_version; }

uint32_t Stsz::GetSampleSize() { return m_sampleSize; }

uint32_t Stsz::GetSampleCount() { return m_sampleCount; }

std::vector<uint32_t> Stsz::GetEntries() { return m_entries; }

#pragma endregion

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/