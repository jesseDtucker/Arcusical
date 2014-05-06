/*
*	Author: Jesse Tucker
*	Date: January 5th 2013
*	Description: Covers enumeration and std::string conversion/printing utilities for image type enums
*/

#pragma once
#ifndef IMAGE_TYPES_HPP
#define IMAGE_TYPES_HPP

#include <string>
#include <ostream>
#include <unordered_map>

#include "boost\assign.hpp"

namespace Arcusical { namespace MPEG4 {

	//TODO: need to add some support for a wider array of image types.
	enum class ImageType
	{
		JPEG,
		PNG,
		EXIF,
		UNKNOWN
	};

	const std::unordered_map<ImageType, std::string> ImageTypeToString = boost::assign::map_list_of<ImageType, std::string>
	(ImageType::JPEG, "JPEG")
	(ImageType::PNG, "PNG")
	(ImageType::EXIF, "EXIF")
	(ImageType::UNKNOWN, "Unknown");

	std::ostream& operator << (std::ostream& outStream, ImageType type);

} /*namespace: MPEG4*/}/*namespace: Arcusical*/ 

#endif