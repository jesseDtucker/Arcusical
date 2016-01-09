/*
*	Author: Jesse Tucker
*	Date: December 30th 2012
*	Name: Meta.hpp
*
*	Description: The Meta box is within the user data box. It is defined on page 56
*	of the ISO 14496-12 standard. This box is a container for various types of metadata.
*/

#include "pch.h"

#include "MPEG4_Parser.hpp"
#include "Meta.hpp"

namespace Arcusical {
namespace MPEG4 {

Meta::Meta() : m_version(0) {}
Meta::~Meta() {}

void Meta::ReadContents(Util::Stream& stream) {
  m_version = stream.ReadInteger<uint32_t>();

  m_children = MPEG4_Parser::ParseBoxes(stream, m_bodySize - sizeof(uint32_t));
}

void Meta::PrintBox(std::ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "META Box: " << std::endl;
  outStream << tabs << "\tVersion: " << m_version << std::endl;

  for (std::shared_ptr<Box> child : m_children) {
    child->PrintBox(outStream, depth + 1);
  }
}

uint32_t Meta::GetVersion() { return m_version; }

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/