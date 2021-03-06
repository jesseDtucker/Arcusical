#pragma once

#include "UnknownBox.hpp"

/*
*	Author: Jesse Tucker
*	Date: January 1st 2013
*	Description: Itunes specific box. No publicly available documentation. Reported as being
*	a catalog number itunes uses internally. Unsure of specific use so it is marked as being
*	an unknown type.
*/

namespace Arcusical {
namespace MPEG4 {

class iCnid : public UnknownBox {
 public:
  iCnid() {}
  virtual ~iCnid() {}
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/
