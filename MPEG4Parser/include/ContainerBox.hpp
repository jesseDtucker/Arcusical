/*
*	Author: Jesse Tucker
*	Date: January 19th 2013
*	Name: Box.hpp
*
*	Description: A special type of box that contains other boxes, and ONLY other boxes.
*	It contains no data that is unique to itself.
*/

#ifndef CONTAINER_BOX_HPP
#define CONTAINER_BOX_HPP

#include "Box.hpp"

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

#endif  // !CONTAINER_BOX_HPP
