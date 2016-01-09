#include "pch.h"

#include "HandlerTypes.hpp"

/*
*	Author: Jesse Tucker
*	Date: December 29th 2012
*	Name: HandlerTypes.hpp
*
*	Description: Defines the types of handles that can be encountered
*	in an MPEG4 container.
*/

namespace Arcusical {
namespace MPEG4 {

std::ostream& operator<<(std::ostream& outStream, HandlerType type) {
  outStream << HandlerTypeToString.at(type);
  return outStream;
}

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/