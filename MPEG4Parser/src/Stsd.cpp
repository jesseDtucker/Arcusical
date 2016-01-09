/*
*	Author: Jesse Tucker
*	Date: December 30th 2012
*	Name: Stsd.hpp
*
*	Description: Sample Description Box. stsd box, defined in ISO 14496-12 page 28.
*	Contains information describing the mediums sample information.
*/

#include "pch.h"

#include "MPEG4_Parser.hpp"
#include "Stsd.hpp"

namespace Arcusical {
namespace MPEG4 {

Stsd::Stsd(){};
Stsd::~Stsd(){};

void Stsd::ReadContents(Util::Stream& stream) {
  // first 4 bytes appear to be reserved. Standard and test file
  // are not consistent...
  stream.Advance(4);

// in a debug we'll use the value as verification,
// in release the variable is unreferenced and triggers a warning
// as it isn't used we just don't read it in
#ifdef _DEBUG
  uint32_t entryCount = stream.ReadInteger<uint32_t>();
#else
  stream.Advance(sizeof(uint32_t));
#endif

  m_children = MPEG4_Parser::ParseBoxes(stream, m_bodySize - 8);

  ARC_ASSERT_MSG(entryCount == m_children.size(), "Entry count and actual children do not match!");
}

void Stsd::PrintBox(std::ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "STSD Box:" << std::endl;

  for (std::shared_ptr<Box> child : m_children) {
    child->PrintBox(outStream, depth + 1);
  }
}

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/