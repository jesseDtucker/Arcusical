#pragma once

#include "Box.hpp"
/*
*	Author: Jesse Tucker
*	Date: January 19th 2013
*	Name: Box.hpp
*
*	Description: A special type of box that contains other boxes, and ONLY other boxes.
*	It contains no data that is unique to itself.
*/


namespace Arcusical {
namespace MPEG4 {

class ContainerBox : public Box {
 public:
  ContainerBox() {}

  virtual ~ContainerBox() {}

  virtual void PrintBox(std::ostream& outStream, int depth = 0);

 protected:
  virtual void ReadContents(Util::Stream& stream) override;
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/
