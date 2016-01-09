/*
*	Author: Jesse Tucker
*	Date: December 28th 2012
*	Name: Trak.cpp
*
*	Description: The Trak box contains information for a single media track. For
*	details please see ISO 14496-12 page 17-18. The Trak box is simply a container
*	for other boxes.
*/

#include "pch.h"

#include "MPEG4_Parser.hpp"
#include "Trak.hpp"

namespace Arcusical {
namespace MPEG4 {

Trak::Trak() {
  // nothing to do
}

Trak::~Trak() {
  // nothing to do...
}

void Trak::PrintBox(std::ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "Trak Box:" << std::endl;

  for (std::shared_ptr<Box> child : m_children) {
    child->PrintBox(outStream, depth + 1);
  }
}

} /*Arcusical*/
} /*MPEG4*/