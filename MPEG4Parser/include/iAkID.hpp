#pragma once

#include "UnknownBox.hpp"
/*
*	Author: Jesse Tucker
*	Date: January 1st 2013
*	Description: Itunes specific box. No publicly available documentation. Reported as
*	being the account code for the account that purchased the media. No interest in this
*	value so it is treated as unknown.
*/


namespace Arcusical {
namespace MPEG4 {

class iAkID : public UnknownBox {
 public:
  iAkID() {}
  virtual ~iAkID() {}
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/
