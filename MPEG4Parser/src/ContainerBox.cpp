/*
*	Author: Jesse Tucker
*	Date: January 19th 2013
*	Name: Box.hpp
*
*	Description: A special type of box that contains other boxes, and ONLY other boxes.
*	It contains no data that is unique to itself.
*/

#include "pch.h"

#include "ContainerBox.hpp"
#include "MPEG4_Parser.hpp"

namespace Arcusical {
namespace MPEG4 {

void ContainerBox::ReadContents(Util::Stream& stream) { m_children = MPEG4_Parser::ParseBoxes(stream, m_bodySize); }

void ContainerBox::PrintBox(std::ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "Container Box:" << std::endl;

  for (std::shared_ptr<Box> child : m_children) {
    child->PrintBox(outStream, depth + 1);
  }
}

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/