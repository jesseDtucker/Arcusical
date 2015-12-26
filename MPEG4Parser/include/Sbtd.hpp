/*
*	Author: Jesse Tucker
*	Date: December 30th 2012
*	Name: Stbd.hpp
*
*	Description: Non-Standard box. Likely introduced by apple but I'm not sure.
*	No idea what data is contained in here...
*/

#ifndef SBTD_HPP
#define SBTD_HPP

#include "UnknownBox.hpp"

namespace Arcusical {
namespace MPEG4 {

// this class is minimal, exists soley to distinguish itself from a truly unknown box.
class Sbtd : public UnknownBox {
 public:
  Sbtd() {}
  virtual ~Sbtd() {};
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/

#endif