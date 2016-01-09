#pragma once

#include "Box.hpp"
/*
*	Author: Jesse Tucker
*	Date: December 27th 2012
*	Name: BoxFactory.hpp
*
*	Description: Defines a free box. This is a box that acts as a spacer and simply takes up space
*/


namespace Arcusical {
namespace MPEG4 {

class FreeBox : public Box {
 public:
  FreeBox();
  virtual ~FreeBox() override;

  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

 protected:
  virtual void ReadContents(Util::Stream& stream) override;
};

} /*MPEG4*/
} /*Arcusical*/
