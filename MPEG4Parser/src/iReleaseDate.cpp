#include "pch.h"

#include "DataBox.hpp"
#include "MPEG4_Parser.hpp"
#include "iReleaseDate.hpp"
/*
*	Author: Jesse Tucker
*	Date: December 30th 2012
*	Name: Pinf.hpp
*
*	Description: Non-Standard box introduced by apple. Contains a std::string representing the release date.
*	Note: online sources indicate this field may not always be identical format, sometimes it may only
*	be the year.
*/

using namespace std;

namespace Arcusical {
namespace MPEG4 {

void iReleaseDate::ReadContents(Util::Stream& stream) {
  m_children = MPEG4_Parser::ParseBoxes(stream, m_bodySize);

  ARC_ASSERT_MSG(m_children.size() > 0, "iAlbum box with no data!");
  if (m_children.size() > 0) {
    std::vector<unsigned char>& data = *dynamic_pointer_cast<DataBox>(m_children[0])->GetData();

    // skip the first 8
    m_releaseDate = std::string(data.begin() + 8, data.end());
  }
}

void iReleaseDate::PrintBox(std::ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "iRelease Date Box: " << std::endl;
  outStream << tabs << "\tRelease Date: " << m_releaseDate << std::endl;
}

std::string iReleaseDate::GetReleaseDate() { return m_releaseDate; }

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/