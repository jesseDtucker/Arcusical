#pragma once

#include "UnknownBox.hpp"

/*
*	Author: Jesse Tucker
*	Date: December 30th 2012
*	Name: Pinf.hpp
*
*	Description: Non-Standard box introduced by apple. Unable to find detailed
*	information on its contents. Appears to be related to the account of the person
*	who purchased the media. Pinf = Purchase Info???
*/

namespace Arcusical {
namespace MPEG4 {

// this class is minimal, exists soley to distinguish itself from a truly unknown box.
class Pinf : public UnknownBox {
 public:
  Pinf() {}
  virtual ~Pinf(){};
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/
