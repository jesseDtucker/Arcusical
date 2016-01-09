#include "pch.h"

#include "DataBox.hpp"
#include "MPEG4_Parser.hpp"
#include "iXid.hpp"

/*
*	File: iXid.hpp
*	Author: Jesse Tucker
*	Date: January 1st 2013
*
*	Description: This box is an itunes non-standard box that contains a std::string that uniquely identifies
*	any piece of media purchased through the itunes store. The idea is that this code can be used to
*	identify media that has had its filename/metadata modified.
*/

using namespace std;

namespace Arcusical {
namespace MPEG4 {

void iXID::ReadContents(Util::Stream& stream) {
  m_children = MPEG4_Parser::ParseBoxes(stream, m_bodySize);

  ARC_ASSERT_MSG(m_children.size() > 0, "iAlbum box with no data!");
  if (m_children.size() > 0) {
    std::vector<unsigned char>& data = *dynamic_pointer_cast<DataBox>(m_children[0])->GetData();
    // offset by 8 to ignore junk data
    m_XID = std::string(data.begin() + 8, data.end());
  }
}

void iXID::PrintBox(std::ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "iXID Box:" << std::endl;

  outStream << tabs << "\tXID: " << m_XID << std::endl;
}

std::string iXID::GetXID() { return m_XID; }

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/