/*
*	Author: Jesse Tucker
*	Date: December 27th 2012
*	Name: Moov.hpp
*
*	Description: The MOOV box contains all the metadata for the media. Please see
*	ISO 14496-12 page 16 for details.
*/

#ifndef MOOV_HPP
#define MOOV_HPP

#include "ContainerBox.hpp"

namespace Arcusical {
namespace MPEG4 {

class Moov : public ContainerBox {
 public:
  Moov();
  virtual ~Moov() override;

  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/

#endif