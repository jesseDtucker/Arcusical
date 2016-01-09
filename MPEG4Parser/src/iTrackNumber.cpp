#include "pch.h"

#include "DataBox.hpp"
#include "MPEG4_Parser.hpp"
#include "iTrackNumber.hpp"

/*
*	Author: Jesse Tucker
*	Date: January 1st 2012
*	Name: iTrackNumber.hpp
*
*	Description: Itunes specific box. Contiains information on the track number. Includes both the number
*	for this track and the track number for the album. ie: track x of y.
*/

using namespace std;

namespace Arcusical {
namespace MPEG4 {

void iTrackNumber::ReadContents(Util::Stream& stream) {
  m_children = MPEG4_Parser::ParseBoxes(stream, m_bodySize);

  // extract the data
  ARC_ASSERT_MSG(m_children.size() > 0, "iAlbum box with no data!");
  if (m_children.size() > 0) {
    std::vector<unsigned char>& data = *dynamic_pointer_cast<DataBox>(m_children[0])->GetData();

    auto dataItr = data.begin() + 10;
    m_trackNumber = Util::ReadInteger<uint16_t>(dataItr);
    m_maxTrack = Util::ReadInteger<uint16_t>(dataItr + 2);
  }
}

void iTrackNumber::PrintBox(std::ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "iTrackNumber Box:" << std::endl;
  outStream << tabs << "\tTrack Number: " << m_trackNumber << " of " << m_maxTrack << std::endl;
}

uint16_t iTrackNumber::GetTrackNumber() { return m_trackNumber; }

uint16_t iTrackNumber::GetMaxTrack() { return m_maxTrack; }

} /*Arcusical*/
} /*MPEG4*/