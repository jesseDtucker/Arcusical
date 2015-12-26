/*
*	Author: Jesse Tucker
*	Date: December 24th 2012
*	Name: IncompatibleBoxTypeException.hpp
*
*	Description: Exception that is to be thrown when the type of
*	a box does not match the box that is being parsed.
*/

#include "BoxParsingException.hpp"

#ifndef INCOMPATIBLE_BOX_TYPE_EXCEPTION_HPP
#define INCOMPATIBLE_BOX_TYPE_EXCEPTION_HPP

namespace Arcusical {
namespace MPEG4 {

class IncompatibleBoxTypeException : public BoxParsingException {
 public:
  IncompatibleBoxTypeException(const char* errMessage) : BoxParsingException(errMessage) {}
};

} /*Arcusical*/
} /*MPEG4*/

#endif