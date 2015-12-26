/*
*	Author: Jesse Tucker
*	Date: January 1st 2013
*	Description: Itunes specific box. No publicly available documentation. Some suggest it is a 'compilation' box
*	however no hard data exists. This is treated as a specialized unknown box
*/

#ifndef ICPIL_HPP
#define ICPIL_HPP

#include "UnknownBox.hpp"

namespace Arcusical {
namespace MPEG4 {

class iCpil : public UnknownBox {
 public:
  iCpil() {}
  virtual ~iCpil() {}
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/

#endif