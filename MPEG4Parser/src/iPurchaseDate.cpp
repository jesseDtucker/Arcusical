#include "pch.h"

#include "DataBox.hpp"
#include "MPEG4_Parser.hpp"
#include "iPurchaseDate.hpp"

/*
*	File: iPurchaseDate.hpp
*	Author: Jesse Tucker
*	Date: January 1st 2013
*
*	Description: This box is an itunes non-standard box that contains a std::string with the purchase date.
*/

using namespace std;

namespace Arcusical {
namespace MPEG4 {

void iPurchaseDate::ReadContents(Util::Stream& stream) {
  m_children = MPEG4_Parser::ParseBoxes(stream, m_bodySize);

  ARC_ASSERT_MSG(m_children.size() > 0, "iAlbum box with no data!");
  if (m_children.size() > 0) {
    std::vector<unsigned char>& data = *dynamic_pointer_cast<DataBox>(m_children[0])->GetData();

    // offset by 8 to skip junk data
    m_purchaseDate = std::string(data.begin() + 8, data.end());
  }
}

void iPurchaseDate::PrintBox(ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "iPurchase Date Box:" << std::endl;
  outStream << tabs << "\tPurchase Date: " << m_purchaseDate << std::endl;
}

std::string iPurchaseDate::GetPurchaseDate() { return m_purchaseDate; }

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/