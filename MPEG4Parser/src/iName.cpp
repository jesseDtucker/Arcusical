/*
*	Author: Jesse Tucker
*	Date: December 29th 2012
*	Name: iName.hpp
*
*	Description: Itunes specific box. Contains the title of the song
*/

#include "pch.h"

#include "DataBox.hpp"
#include "MPEG4_Parser.hpp"
#include "iName.hpp"

using namespace std;

namespace Arcusical {
namespace MPEG4 {

void iName::ReadContents(Util::Stream& stream) {
  // the title is contained within a data child
  m_children = MPEG4_Parser::ParseBoxes(stream, m_bodySize);

  ARC_ASSERT_MSG(m_children.size() > 0, "iAlbum box with no data!");
  if (m_children.size() > 0) {
    // extract the title
    std::shared_ptr<DataBox> dataBox = dynamic_pointer_cast<DataBox>(m_children[0]);
    std::vector<unsigned char>& data = *dataBox->GetData();
    // Note: the first 8 bytes of data are of unknown origin, the begin iterator below is offset to skip these values.
    m_title = std::string(data.begin() + 8, data.end());
  }
}

void iName::PrintBox(ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "iName Box:" << std::endl;
  outStream << tabs << "\tTitle: " << m_title << std::endl;
}

std::string iName::GetTitle() { return m_title; }

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/