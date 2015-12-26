/*
*	File: Cprt.hpp
*	Author: Jesse Tucker
*	Date: January 1st 2013
*
*	Description: This is an ISO standard box for containing copyright information. However it is currently
*	implemented as an unknown box to maintain compatibility with apple meta data. The Cprt box is supposed
*	to be contained within the udta box however apple has another box by the same name (cprt) contained within
*	their apple specific metadata, however it has a different format.
*
*	As a result of this discrepency, and the low priority of being able to edit the cprt box, this box is
*implemented
*	as if it were an unknown type. This may change in the future.
*/

/*
*	Author: Jesse Tucker
*	Date: January 1st 2013
*	Description: Itunes specific box. No publicly available documentation. Some suggest it is a 'compilation' box
*	however no hard data exists. This is treated as a specialized unknown box
*/

#ifndef CPRT_HPP
#define CPRT_HPP

#include "UnknownBox.hpp"

namespace Arcusical {
namespace MPEG4 {

class Cprt : public UnknownBox {
 public:
  Cprt() {}
  virtual ~Cprt() {}
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/

#endif