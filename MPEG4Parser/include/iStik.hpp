#pragma once

#include "UnknownBox.hpp"

/*
*	Author: Jesse Tucker
*	Date: January 1st 2013
*	Description: Itunes specific box. No publicly available documentation. Reported as
*	being the media type. May have a use for this field in the future but for now it
*	is left as unknown.
*/

namespace Arcusical {
namespace MPEG4 {

class iStik : public UnknownBox {
 public:
  iStik() {}
  virtual ~iStik() {}
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/
