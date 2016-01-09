#pragma once

#include <vector>

#include "DataBox.hpp"
/*
*	Author: Jesse Tucker
*	Date: December 27th 2012
*	Name: UnknownBox.hpp
*
*	Description: Defines a box of an unknown type
*/


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
