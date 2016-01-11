#pragma once

#include "ContainerBox.hpp"

/*
*	Author: Jesse Tucker
*	Date: Dec. 29th 2012
*	Description: The STBL box is defined in the ISO 14496-12 standard
*	on page 24. It is the Sample Table Box and is a container for other
*	boxes that contain information on the sample description, sample size,
*	sample location, etc. Please see standard for details.
*/

namespace Arcusical {
namespace MPEG4 {

class Stbl : public ContainerBox {
 public:
  Stbl();
  virtual ~Stbl() override;

  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/
