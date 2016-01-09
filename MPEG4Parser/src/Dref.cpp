/*
*	Author: Jesse Tucker
*	Date: December 29th 2012
*	Name: Dref.hpp
*
*	Description: Defines the Dref box. Please see ISO 14496-12 standard, page 38 for details.
*	The Dref box is the Data Reference box, it in turn contains 2 other types of boxes, the
*	Url box and the Urn box. Essentially this is just a container, just with somewhat different
*	parsing rules. See standard for specifics.
*/

#include "pch.h"

#include "Dref.hpp"
#include "InvalidValueException.hpp"
#include "MPEG4_Parser.hpp"

using namespace std;

namespace Arcusical {
namespace MPEG4 {

Dref::Dref() : m_version(0) {
  // just a container, nothing to do
}

Dref::~Dref() {
  // naughta
}

void Dref::ReadContents(Util::Stream& stream) {
  m_version = stream.ReadInteger<uint32_t>();
  m_entryCount = stream.ReadInteger<uint32_t>();

  auto childrenLength = m_bodySize - 2 * sizeof(uint32_t);
  m_children = MPEG4_Parser::ParseBoxes(stream, childrenLength);

  // quick verification of count as we just parsed to the end of the box
  // rather than explicitly handling the count
  if (m_entryCount != m_children.size()) {
    // Problem
    throw InvalidValueException("Count was not a correct value!");
  }
}

void Dref::PrintBox(ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "DREF Box:" << std::endl;

  outStream << tabs << "\tVersion: " << m_version << std::endl;
  outStream << tabs << "\tEntry Count: " << m_entryCount << std::endl;

  for (std::shared_ptr<Box> child : m_children) {
    child->PrintBox(outStream, depth + 1);
  }
}

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/