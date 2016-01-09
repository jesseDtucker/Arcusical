#include "pch.h"

#include "DataBox.hpp"
#include "MPEG4_Parser.hpp"
#include "iAlbum.hpp"
/*
*	Author: Jesse Tucker
*	Date: January 1st 2012
*	Name: iAlbum.hpp
*
*	Description: Itunes specific box. Contains the album name
*/

using namespace std;

namespace Arcusical {
namespace MPEG4 {

void iAlbum::ReadContents(Util::Stream& stream) {
  m_children = MPEG4_Parser::ParseBoxes(stream, m_bodySize);

  ARC_ASSERT_MSG(m_children.size() > 0, "iAlbum box with no data!");
  if (m_children.size() > 0) {
    std::vector<unsigned char>& data = *dynamic_pointer_cast<DataBox>(m_children[0])->GetData();
    // skip the first 8 bytes.
    m_albumName = std::string(data.begin() + 8, data.end());
  }
}

void iAlbum::PrintBox(ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "iAlbum Box: " << std::endl;
  outStream << tabs << "\tAlbum Name; " << m_albumName << std::endl;
}

std::string iAlbum::GetAlbumName() { return m_albumName; }

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/