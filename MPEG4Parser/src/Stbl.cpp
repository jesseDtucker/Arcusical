/*
*	Author: Jesse Tucker
*	Date: Dec. 29th 2012
*	Description: The STBL box is defined in the ISO 14496-12 standard
*	on page 24. It is the Sample Table Box and is a container for other
*	boxes that contain information on the sample description, sample size,
*	sample location, etc. Please see standard for details.
*/

#include "pch.h"

#include "Stbl.hpp"
#include "MPEG4_Parser.hpp"

namespace Arcusical {
namespace MPEG4 {

Stbl::Stbl() {
  // Nothing to do
}

Stbl::~Stbl() {
  // Nothing to do
}

void Stbl::PrintBox(std::ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  for (std::shared_ptr<Box> child : m_children) {
    child->PrintBox(outStream, depth + 1);
  }
}

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/