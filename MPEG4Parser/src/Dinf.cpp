/*
*	Author: Jesse Tucker
*	Date: December 29th 2012
*	Name: Dinf.hpp
*
*	Description: Defines the DINF box. Definition found in ISO 14496-12 standard, page 38.
*	The DINF box is the Data Information Box and is a container for other boxes. The boxes
*	it contains hold information on the location of media information in a track.
*/

#include "pch.h"

#include "Dinf.hpp"
#include "MPEG4_Parser.hpp"

using namespace std;

namespace Arcusical {
namespace MPEG4 {

Dinf::Dinf() {
  // nothing to do
}

Dinf::~Dinf() {
  // still nothing to do (isn't this great...?)
}

void Dinf::PrintBox(ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "Dinf Box:" << std::endl;

  for (std::shared_ptr<Box> child : m_children) {
    child->PrintBox(outStream, depth + 1);
  }
}

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/