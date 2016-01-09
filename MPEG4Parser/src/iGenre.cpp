#include "pch.h"

#include <codecvt>

#include "DataBox.hpp"
#include "MPEG4_Parser.hpp"
#include "iGenre.hpp"
/*
*	Author: Jesse Tucker
*	Date: January 1st 2012
*	Name: Hdlr.hpp
*
*	Description: Itunes specific box. Contains a code that determines the genre. Code is 16 bits
*	and genre is determined by its mapping to the ID3 genres. However the actual coded value is one larger.
*/

using namespace std;

namespace Arcusical {
namespace MPEG4 {

void iGenre::ReadContents(Util::Stream& stream) {
  // this box may contain a field or it may contain child elements
  // itunes music will usually have the data in children
  // some other boxes will have a string member.
  auto nextBoxSize = stream.ReadInteger<uint32_t>();
  stream.Rewind(sizeof(uint32_t));
  if (nextBoxSize > 0) {
    // then the genre is stored in a child!
    m_children = MPEG4_Parser::ParseBoxes(stream, m_bodySize);

    ARC_ASSERT_MSG(m_children.size() > 0, "iAlbum box with no data!");
    if (m_children.size() > 0) {
      std::vector<unsigned char>& data = *dynamic_pointer_cast<DataBox>(m_children[0])->GetData();
      auto dataItr = data.begin();
      // skip first 8 bytes
      dataItr += 8;

      uint16_t encodedGenre = Util::ReadInteger<uint16_t>(dataItr);

      auto genreItr = NumToGenre.find(encodedGenre);

      ARC_ASSERT_MSG(genreItr != NumToGenre.end(), "Found Unknown Genre!");
      if (genreItr != NumToGenre.end()) {
        m_genre = genreItr->second;
      } else {
        m_genre = GenreType::UNKNOWN;
      }
    }
  } else {
    // ignore the first 6 bytes then read the string in
    // The first 4 are the empty size field that marks this odd box,
    // I have no idea what the next 2 are supposed to be so just ignoring them
    stream.Advance(6);
    // presumably this is a unicode string
    auto stringData = stream.ReadRawBytes(Util::SafeIntCast<unsigned int>(m_bodySize - 6));

    std::wstring_convert<std::codecvt_utf16<wchar_t, 0x10ffff, std::consume_header>> converter;
    m_nonStandardGenre = converter.from_bytes(reinterpret_cast<char*>(stringData.data()),
                                              reinterpret_cast<char*>(stringData.data() + stringData.size()));
  }
}

void iGenre::PrintBox(ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "iGenre Box:" << std::endl;
  outStream << tabs << "\tGenre: " << m_genre << std::endl;
}

GenreType iGenre::GetGenre() { return m_genre; }

std::wstring iGenre::GetNonStandardGenre() {
  ARC_ASSERT_MSG(m_genre == GenreType::UNKNOWN,
                 "Can only fetch non standard genre if we do not know what type the genre is!");
  return m_nonStandardGenre;
}

} /*Arcusical*/
} /*MPEG4*/