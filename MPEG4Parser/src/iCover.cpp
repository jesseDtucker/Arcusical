#include "pch.h"

#include "iCover.hpp"

#include "DataBox.hpp"
#include "MPEG4_Parser.hpp"

/*
*	Author: Jesse Tucker
*	Date: January 1st 2013
*	File: iCovr.hpp
*
*	Description: Itunes non-standard box, contains the album artwork. This is the raw image file
*	that has been directly encoded with the song.
*/

using namespace std;

namespace Arcusical {
namespace MPEG4 {

// value found at the start of every JPEG file
const std::vector<unsigned char> iCover::JPEG_HEADER_START = boost::assign::list_of(0xFF)(0xD8)(0xFF)(0xE0);
const std::vector<unsigned char> iCover::PNG_HEADER_START =
    boost::assign::list_of(0x89)(0x50)(0x4E)(0x47)(0x0D)(0x0A)(0x1A)(0x0A);
const std::vector<unsigned char> iCover::EXIF_HEADER_START = boost::assign::list_of(0xFF)(0xD8)(0xFF)(0xE1);

void iCover::ReadContents(Util::Stream& stream) {
  m_children = MPEG4_Parser::ParseBoxes(stream, m_bodySize);

  ARC_ASSERT_MSG(m_children.size() > 0, "iAlbum box with no data!");
  if (m_children.size() > 0) {
    std::vector<unsigned char>& data = *dynamic_pointer_cast<DataBox>(m_children[0])->GetData();

    // first 8 bytes of the data are skipped.
    m_imageData = data.begin() + 8;
    m_imageDataSize = m_children[0]->GetSize() - 8;

    // now determine the type
    m_type = DetermineType(m_imageData);
  }
}

void iCover::PrintBox(ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "iCover Box:" << std::endl;
  outStream << tabs << "\tImage Type: " << m_type << std::endl;
}

ImageType iCover::DetermineType(std::vector<unsigned char>::const_iterator itr) {
  ImageType type = ImageType::UNKNOWN;

  // the type is assumed by the contents of the first few bytes.
  std::vector<unsigned char> data;

  data.insert(data.begin(), itr, itr + 4);

  if (data == JPEG_HEADER_START) {
    type = ImageType::JPEG;
  } else if (data == EXIF_HEADER_START) {
    type = ImageType::EXIF;
  } else {
    // then it might be png, need 4 more bytes to know, png header signature is 8 bytes
    data.insert(data.end(), itr + 4, itr + 8);

    if (data == PNG_HEADER_START) {
      type = ImageType::PNG;
    }
  }

  ARC_ASSERT_MSG(type != ImageType::UNKNOWN, "Warning: unknown image type detected!");
  return type;
}

std::vector<unsigned char>::const_iterator iCover::GetImageData() { return m_imageData; }

uint64_t iCover::GetImageDataSize() { return m_imageDataSize; }

ImageType iCover::GetType() { return m_type; }

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/