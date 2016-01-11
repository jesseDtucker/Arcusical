#pragma once

#include "ContainerBox.hpp"

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

namespace Arcusical {
namespace MPEG4 {

class Udta : public ContainerBox {
 public:
  Udta();
  virtual ~Udta() override;

  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/
