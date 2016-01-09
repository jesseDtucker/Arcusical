#pragma once

#include "ContainerBox.hpp"

/*
*	Author: Jesse Tucker
*	Date: December 31th 2012
*	Name: Meta.hpp
*
*	Description: Non standard box used to contain Apple's metadata. No official public documentation
*	available. Implementation derived from various online sources and observation of file structure.
*	This box is a container for a variety of other atoms that define the apple atoms
*/

namespace Arcusical {
namespace MPEG4 {

class Ilst : public ContainerBox {
 public:
  Ilst();
  virtual ~Ilst() override;

  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/
