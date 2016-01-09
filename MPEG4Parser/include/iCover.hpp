#pragma once

#include "boost\assign.hpp"

#include "Box.hpp"
#include "ImageTypes.hpp"
/*
*	Author: Jesse Tucker
*	Date: January 1st 2013
*	File: iCovr.hpp
*
*	Description: Itunes non-standard box, contains the album artwork. This is the raw image file
*	that has been directly encoded with the song.
*/


namespace Arcusical {
namespace MPEG4 {

class iCover : public Box {
 public:
  iCover() : m_type(ImageType::UNKNOWN){};
  virtual ~iCover(){};

  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

  std::vector<unsigned char>::const_iterator GetImageData();
  uint64_t GetImageDataSize();
  ImageType GetType();

 protected:
  virtual void ReadContents(Util::Stream& stream) override;

  virtual ImageType DetermineType(std::vector<unsigned char>::const_iterator itr);

  std::vector<unsigned char>::const_iterator m_imageData;
  uint64_t m_imageDataSize;
  ImageType m_type;

  // constant sets of bytes found at the head of image files.
  static const std::vector<unsigned char> JPEG_HEADER_START;
  static const std::vector<unsigned char> PNG_HEADER_START;
  static const std::vector<unsigned char> EXIF_HEADER_START;
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/
