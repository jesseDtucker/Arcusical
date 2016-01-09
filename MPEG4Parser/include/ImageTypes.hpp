/*
*	Author: Jesse Tucker
*	Date: January 5th 2013
*	Description: Covers enumeration and std::string conversion/printing utilities for image type enums
*/

#pragma once
#ifndef IMAGE_TYPES_HPP
#define IMAGE_TYPES_HPP

#include <ostream>
#include <string>
#include <unordered_map>

namespace Arcusical {
namespace MPEG4 {

// TODO: need to add some support for a wider array of image types.
enum class ImageType { JPEG, PNG, EXIF, BMP, UNKNOWN };

const std::unordered_map<ImageType, std::string> ImageTypeToString = {{ImageType::JPEG, "jpg"},
                                                                      {ImageType::PNG, "png"},
                                                                      {ImageType::EXIF, "jpg"},  // stored in a jpg
                                                                      {ImageType::BMP, "bmp"},
                                                                      {ImageType::UNKNOWN, "Unknown"}};

const std::unordered_map<ImageType, std::wstring> ImageTypeToWString = {{ImageType::JPEG, L"jpg"},
                                                                        {ImageType::PNG, L"png"},
                                                                        {ImageType::EXIF, L"jpg"},  // stored in a jpg
                                                                        {ImageType::BMP, L"bmp"},
                                                                        {ImageType::UNKNOWN, L"Unknown"}};

const std::unordered_map<std::wstring, ImageType> WStringToImageType = {
    {L"jpg", ImageType::JPEG}, {L"jpeg", ImageType::JPEG}, {L"png", ImageType::PNG}, {L"bmp", ImageType::BMP}};

std::ostream& operator<<(std::ostream& outStream, ImageType type);

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/

#endif