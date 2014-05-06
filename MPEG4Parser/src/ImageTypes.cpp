/*
*	Author: Jesse Tucker
*	Date: January 5th 2013
*	Description: Covers enumeration and std::string conversion/printing utilities for image type enums
*/

#include "pch.h"

#include "Imagetypes.hpp"

namespace Arcusical { namespace MPEG4 {

	std::ostream& operator << (std::ostream& outStream, ImageType type)
	{
		outStream << ImageTypeToString.at(type);
		return outStream;
	}


} /*namespace: MPEG4*/}/*namespace: Arcusical*/ 