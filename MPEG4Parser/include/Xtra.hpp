/*
*	Author: Jesse Tucker
*	Date: January 13th 2013
*
*	Description: Box of unknown origins. Some references found online link it to microsoft
*	however little data exists. Examination of binary shows a large amount of plain test.
*	Unsure of the value of this box so for now it is being marked as unknown
*/

#ifndef XTRA_HPP
#define XTRA_HPP

#include "UnknownBox.hpp"

namespace Arcusical {
namespace MPEG4 {

class Xtra : public UnknownBox {
 public:
  Xtra() {}
  virtual ~Xtra() {}
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/

#endif