#pragma once

#include "ContainerBox.hpp"
/*
*	Author: Jesse Tucker
*	Date: December 16th 2012
*	Name: Minf.hpp
*
*	Description: Defines the Minf box. Definition found in ISO 14496-12 standard, page 22.
*	An minf box is a Media information box. It is simply a container for other boxes.
*/


namespace Arcusical {
namespace MPEG4 {

class Minf : public ContainerBox {
 public:
  Minf();
  virtual ~Minf() override;

  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/
