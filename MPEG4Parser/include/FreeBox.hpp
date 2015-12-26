/*
*	Author: Jesse Tucker
*	Date: December 27th 2012
*	Name: BoxFactory.hpp
*
*	Description: Defines a free box. This is a box that acts as a spacer and simply takes up space
*/

#ifndef BOX_FREE_HPP
#define BOX_FREE_HPP

#include "Box.hpp"

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
#endif