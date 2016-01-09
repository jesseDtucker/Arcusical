/*
*	Author: Jesse Tucker
*	Date: December 29th 2012
*	Name: iName.hpp
*
*	Description: Itunes specific box. Contains the album artist
*/

#include "pch.h"

#include "DataBox.hpp"
#include "MPEG4_Parser.hpp"
#include "iAlbumArtist.hpp"

using namespace std;

namespace Arcusical {
namespace MPEG4 {

void iAlbumArtist::ReadContents(Util::Stream& stream) {
  m_children = MPEG4_Parser::ParseBoxes(stream, m_bodySize);

  ARC_ASSERT_MSG(m_children.size() > 0, "iAlbum box with no data!");
  if (m_children.size() > 0) {
    std::vector<unsigned char>& data = *dynamic_pointer_cast<DataBox>(m_children[0])->GetData();
    // skip 8 bytes
    m_albumArtist = std::string(data.begin() + 8, data.end());
  }
}

void iAlbumArtist::PrintBox(ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "iAlbum Artist Box: " << std::endl;
  outStream << tabs << "\tAlbum Artist: " << m_albumArtist << std::endl;
}

std::string iAlbumArtist::GetAlbumArtist() { return m_albumArtist; }

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/