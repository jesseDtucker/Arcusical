#pragma once

#include "UnknownBox.hpp"

/*
*	Author: Jesse Tucker
*	Date: January 1st 2013
*	Description: Itunes specific box. No publicly available documentation. Reported as
*	being the country code of the country the media was purchased in. Some specifics
*	on the values however source cannot be verified and this item has no value and so
*	will be treated as unknown.
*/

namespace Arcusical {
namespace MPEG4 {

class iSfID : public UnknownBox {
 public:
  iSfID() {}
  virtual ~iSfID() {}
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/
