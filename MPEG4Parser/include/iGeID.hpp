#pragma once

#include "UnknownBox.hpp"

/*
*	Author: Jesse Tucker
*	Date: January 1st 2013
*	Description: Itunes specific box. No publicly available documentation. Reported as
*	being a genre ID. No specifics on the mapping of this number to a genre, assumed
*	to be of internal use to itunes.
*/

namespace Arcusical {
namespace MPEG4 {

class iGeID : public UnknownBox {
 public:
  iGeID() {}
  virtual ~iGeID() {}
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/
