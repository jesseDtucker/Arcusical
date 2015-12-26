/*
*	Author: Jesse Tucker
*	Date: December 30th 2012
*	Name: Udta.hpp
*
*	Description: The Udta box is the user data box. It is defined on page 53
*	of the ISO 14496-12 standard. The user data box contains user information
*	specific to the presentation of the media. This includes values such as title,
*	artist and copyright notices.
*/

#include "pch.h"

#include "Udta.hpp"
#include "MPEG4_Parser.hpp"

using namespace std;

namespace Arcusical {
namespace MPEG4 {

Udta::Udta() {};
Udta::~Udta() {};

void Udta::PrintBox(ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);
  outStream << tabs << "UDTA Box:" << std::endl;

  for (std::shared_ptr<Box> child : m_children) {
    child->PrintBox(outStream, depth + 1);
  }
}

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/