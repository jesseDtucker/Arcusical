#include "pch.h"

#include "DataBox.hpp"
#include "MPEG4_Parser.hpp"
#include "iAppleID.hpp"

/*
*	File: iAppleID.hpp
*	Author: Jesse Tucker
*	Date: January 1st 2013
*
*	Description: The is a non-standard box added by apple. This box contains the id of the person
*	who purchased the music. Note: appears to be the apple id of the person at time of purchase, apple does
*	not appear to update if person changes their ID unless the meta data of the file is edited after the change.
*/

using namespace std;

namespace Arcusical {
namespace MPEG4 {

void iAppleID::ReadContents(Util::Stream& stream) {
  m_children = MPEG4_Parser::ParseBoxes(stream, m_bodySize);

  ARC_ASSERT_MSG(m_children.size() > 0, "iAlbum box with no data!");
  if (m_children.size() > 0) {
    std::vector<unsigned char>& data = *dynamic_pointer_cast<DataBox>(m_children[0])->GetData();
    m_ID = std::string(data.begin() + 8, data.end());
  }
}

void iAppleID::PrintBox(ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "iApple ID Box: " << std::endl;
  outStream << tabs << "\tID: " << m_ID << std::endl;
}

std::string iAppleID::GetID() { return m_ID; }

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/