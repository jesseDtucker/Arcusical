#pragma once

#include "ContainerBox.hpp"

/*
*	Author: Jesse Tucker
*	Date: December 29th 2012
*	Name: Dinf.hpp
*
*	Description: Defines the DINF box. Definition found in ISO 14496-12 standard, page 38.
*	The DINF box is the Data Information Box and is a container for other boxes. The boxes
*	it contains hold information on the location of media information in a track.
*/

namespace Arcusical {
namespace MPEG4 {

class Dinf : public ContainerBox {
 public:
  Dinf();
  virtual ~Dinf() override;

  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/
