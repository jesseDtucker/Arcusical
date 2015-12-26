/*
*	Author: Jesse Tucker
*	Date: December 24th 2012
*	Name: InvalidValueException.hpp
*
*	Description: Exception that is to be thrown when a value read
*	from a box is not allowed
*/

#include "BoxParsingException.hpp"

#ifndef INVALID_VALUE_EXCEPTION_HPP
#define INVALID_VALUE_EXCEPTION_HPP

namespace Arcusical {
namespace MPEG4 {

class InvalidValueException : public BoxParsingException {
 public:
  InvalidValueException(const char* errMessage) : BoxParsingException(errMessage) {}
};

} /*Arcusical*/
} /*MPEG4*/

#endif