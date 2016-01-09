#pragma once

#include "ContainerBox.hpp"

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

class Mdia : public ContainerBox {
 public:
  Mdia();
  virtual ~Mdia() override;

  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/
