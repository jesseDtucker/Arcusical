#include "pch.h"

#include "iArtist.hpp"

#include "DataBox.hpp"
#include "MPEG4_Parser.hpp"

/*
*	Author: Jesse Tucker
*	Date: December 29th 2012
*	Name: iName.hpp
*
*	Description: Itunes specific box. Contains the artist of the song
*/

using namespace std;

namespace Arcusical {
namespace MPEG4 {

void iArtist::ReadContents(Util::Stream& stream) {
  m_children = MPEG4_Parser::ParseBoxes(stream, m_bodySize);

  ARC_ASSERT_MSG(m_children.size() > 0, "iAlbum box with no data!");
  if (m_children.size() > 0) {
    std::vector<unsigned char>& data = *dynamic_pointer_cast<DataBox>(m_children[0])->GetData();
    // ignore the first 8 bytes of data
    m_artist = std::string(data.begin() + 8, data.end());
  }
}

void iArtist::PrintBox(ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "iArtist Box: " << std::endl;
  outStream << tabs << "\tArtist: " << m_artist << std::endl;
}

std::string iArtist::GetArtist() { return m_artist; }

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/