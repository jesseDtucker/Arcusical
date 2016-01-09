#include "pch.h"

#include "GenreTypes.hpp"
/*
*	Author: Jesse Tucker
*	File: GenreTypes.hpp
*	Description: This header contains an enermuration of the predefined Genre types. The types are
*	defined as ID3 genres, however the value itunes encodes appear to be exactly 1 more than the
*	value referenced by ID3.
*/

namespace Arcusical {
namespace MPEG4 {

std::ostream& operator<<(std::ostream& outStream, GenreType type) {
  outStream << GenreToString.at(type);
  return outStream;
}

} /*MPEG4*/
} /*Arcusical*/