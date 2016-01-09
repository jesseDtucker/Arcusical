/*
*	Author: Jesse Tucker
*	Date: December 27th 2012
*	Name: UnknownBox.hpp
*
*	Description: Defines a box of an unknown type
*/

#ifndef BOX_UNKNOWN_HPP
#define BOX_UNKNOWN_HPP

#include <vector>

#include "DataBox.hpp"

namespace Arcusical {
namespace MPEG4 {

class UnknownBox : public DataBox {
 public:
  UnknownBox(){};
  virtual ~UnknownBox(){};

  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;
};

} /*MPEG4*/
} /*Arcusical*/

#endif