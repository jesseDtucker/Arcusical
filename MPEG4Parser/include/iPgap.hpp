#pragma once

#include "UnknownBox.hpp"
/*
*	Author: Jesse Tucker
*	Date: January 1st 2013
*	Description: Itunes specific box. No publicly available documentation. Appears to be a flag for
*	gapless playback however all sources state the flag is an 8 bit boolean. Oberservation however
*	shows that there are many more bytes in the data (approx. 9). Treating as unknown for now.
*/


namespace Arcusical {
namespace MPEG4 {

class iPgap : public UnknownBox {
 public:
  iPgap() {}
  virtual ~iPgap() {}
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/
