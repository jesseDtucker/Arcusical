#pragma once

#include "BoxParsingException.hpp"

/*
*	Author: Jesse Tucker
*	Date: December 24th 2012
*	Name: InvalidValueException.hpp
*
*	Description: Exception that is to be thrown when a value read
*	from a box is not allowed
*/

namespace Arcusical {
namespace MPEG4 {

class InvalidValueException : public BoxParsingException {
 public:
  InvalidValueException(const char* errMessage) : BoxParsingException(errMessage) {}
};

} /*Arcusical*/
} /*MPEG4*/
