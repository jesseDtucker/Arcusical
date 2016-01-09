#include "pch.h"

#include "MPEG4_Parser.hpp"

#include "Arc_Assert.hpp"
#include "BoxFactory.hpp"
#include "Ftyp.hpp"
#include "Stream.hpp"

/*
*	Author: Jesse Tucker
*	Date: December 16th 2012
*	Name: MPEG4_Parser.cpp
*
*	Description: Please see header for details
*/

using namespace std;

namespace Arcusical {
namespace MPEG4 {

#pragma region Ctor / Dtor

// public
MPEG4_Parser::MPEG4_Parser() { m_tree = nullptr; }

// public
MPEG4_Parser::~MPEG4_Parser() {
  // smart pointers will handle it all
}

#pragma endregion

#pragma region Public Methods

// public
std::shared_ptr<MPEG4_SongFile> MPEG4_Parser::ReadAndParseFromStream(Util::Stream& inStream) {
  m_tree = std::make_shared<MPEG4_Tree>();
  return make_shared<MPEG4_SongFile>(MPEG4_SongFile(ParseTree(inStream)));
}

#pragma endregion

#pragma region Public Static Methods

// public static
std::vector<std::shared_ptr<Box>> MPEG4_Parser::ParseBoxes(Util::Stream& stream, long long bytesToParse) {
  std::vector<std::shared_ptr<Box>> boxes;
  bool doneParsing = false;

  auto startPosition = stream.GetPosition();

  while (!doneParsing && stream.GetPosition() < startPosition + bytesToParse && stream.GetBytesRemainaingLength() > 0) {
    BoxHeader header = ReadHeader(stream);
    auto headerSize = header.m_is64BitSize ? 16 : 8;

    // get the box
    std::shared_ptr<Box> box = BoxFactory::GetBox(header.m_type);
    // parse box
    box->ParseBox(header.m_size, stream, headerSize);
    // add box to tree
    boxes.push_back(box);

    if (header.m_size == 0) {
      doneParsing = true;
    }
  }

  ARC_ASSERT_MSG(stream.GetPosition() - startPosition == bytesToParse,
                 "Did not parse the number of bytes that we were supposed to");

  return boxes;
}

// public static
uint64_t MPEG4_Parser::GetSize(Util::Stream& stream, bool& is64bit) {
  uint64_t size = 0;
  uint32_t shorterSize = 0;
  is64bit = false;

  shorterSize = stream.ReadInteger<uint32_t>();
  stream.Rewind(sizeof(uint32_t));

  if (shorterSize == 1) {
    // then it is a 64 bit value
    stream.Advance(4);  // skip the type bytes
    size = stream.ReadInteger<uint64_t>();
    is64bit = true;
    stream.Rewind(4);
  } else {
    size = shorterSize;
  }

  return size;
}

// public static
BoxType MPEG4_Parser::GetType(Util::Stream& stream) {
  stream.Advance(4);  // skip the size field
  std::string typeStr = stream.ReadString(4);
  stream.Rewind(8);  // go back to the start of the header

  auto type = StringToBoxType.find(typeStr);

  // ARC_ASSERT_MSG(type != StringToBoxType.end(), "Found unknown box type!")
  if (type != StringToBoxType.end()) {
    return type->second;
  } else {
    return BoxType::UNKNOWN;
  }
}

#pragma endregion

#pragma region Protected Methods

// protected
std::shared_ptr<MPEG4_Tree> MPEG4_Parser::ParseTree(Util::Stream& stream) {
  auto boxes = ParseBoxes(stream, stream.GetLength());
  std::for_each(boxes.begin(), boxes.end(), [this](std::shared_ptr<Box> box) { m_tree->AddBox(box); });

  return m_tree;
}

BoxHeader MPEG4_Parser::ReadHeader(Util::Stream& stream) {
  BoxHeader header;

  // get the type
  header.m_type = GetType(stream);  // plus 4 because we need to skip the size field
  header.m_size = GetSize(stream, header.m_is64BitSize);

  // advance the iterator to the start of the data
  if (header.m_is64BitSize) {
    // skip the first size field, the type field and the second size field
    stream.Advance(16);
  } else {
    // skip just the first size field and the type field
    stream.Advance(8);
  }

  return header;
}

#pragma endregion

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/