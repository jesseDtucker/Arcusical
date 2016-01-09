#include "pch.h"

#include "MPEG4_Parser.hpp"
#include "Moov.hpp"
/*
*	Author: Jesse Tucker
*	Date: December 27th 2012
*	Name: Moov.hpp
*
*	Description: The MOOV box contains all the metadata for the media. Please see
*	ISO 14496-12 page 16 for details.
*/

namespace Arcusical {
namespace MPEG4 {

Moov::Moov() {
  // nothing to do
}

Moov::~Moov() {
  // nothing to do...
}

void Moov::PrintBox(std::ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "MOOV Box:" << std::endl;

  for (std::shared_ptr<Box> child : m_children) {
    child->PrintBox(outStream, depth + 1);
  }
}

} /*Arcusical*/
} /*MPEG4*/