#include "pch.h"

#include "MPEG4_Parser.hpp"
#include "Mdia.hpp"

/*
*	Author: Jesse Tucker
*	Date: December 29th 2012
*	Name: Mdia.hpp
*
*	Description: Media box. A container for other boxes. Defined on page 20 of
*	the ISO 14496-12 standard.
*/

namespace Arcusical {
namespace MPEG4 {

Mdia::Mdia() {
  // Nothing to do
}

Mdia::~Mdia() {
  // nothing to do
}

void Mdia::PrintBox(std::ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "MDIA Box:" << std::endl;

  for (std::shared_ptr<Box> child : m_children) {
    child->PrintBox(outStream, depth + 1);
  }
}

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/