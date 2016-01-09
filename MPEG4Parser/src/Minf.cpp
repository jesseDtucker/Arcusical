#include "pch.h"

#include "Minf.hpp"

#include "MPEG4_Parser.hpp"

/*
*	Author: Jesse Tucker
*	Date: December 16th 2012
*	Name: Minf.hpp
*
*	Description: Defines the Minf box. Definition found in ISO 14496-12 standard, page 22.
*	An minf box is a Media information box. It is simply a container for other boxes.
*/

namespace Arcusical {
namespace MPEG4 {

Minf::Minf() {
  // Nothing to do
}

Minf::~Minf() {
  // Nothing to do
}

void Minf::PrintBox(std::ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "MINF Box: " << std::endl;

  for (std::shared_ptr<Box> child : m_children) {
    child->PrintBox(outStream, depth + 1);
  }
}

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/