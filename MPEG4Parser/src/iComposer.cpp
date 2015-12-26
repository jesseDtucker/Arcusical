/*
*	Author: Jesse Tucker
*	Date: January 13th 2013
*	Description: Itunes specific box. No publicly available documentation. Contains
*	the name of the Composer.
*/

#include "pch.h"

#include "iComposer.hpp"
#include "MPEG4_Parser.hpp"
#include "DataBox.hpp"

using namespace std;

namespace Arcusical {
namespace MPEG4 {

void iComposer::ReadContents(Util::Stream& stream) {
  m_children = MPEG4_Parser::ParseBoxes(stream, m_bodySize);

  ARC_ASSERT_MSG(m_children.size() > 0, "iAlbum box with no data!");
  if (m_children.size() > 0) {
    auto dataItr = dynamic_pointer_cast<DataBox>(m_children[0])->GetData()->begin();
    auto dataEnd = dynamic_pointer_cast<DataBox>(m_children[0])->GetData()->end();

    // first 8 bytes are unknown values
    dataItr += 8;

    m_composer = std::string(dataItr, dataEnd);
  }
}

void iComposer::PrintBox(std::ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "iComposer Box:" << std::endl;
  outStream << tabs << "\tName: " << m_composer << std::endl;
}

std::string iComposer::GetComposer() { return m_composer; }

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/