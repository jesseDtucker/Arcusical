/*
*	Author: Jesse Tucker
*	Date: December 16th 2012
*	Name: MPEG4_Parser.h
*
*	Description: Parses an input stream to create an MPEG4_Tree that
*	represents the MPEG4 file.
*/

#ifndef MPEG4_PARSER_H
#define MPEG4_PARSER_H

#include "boost/detail/endian.hpp"
#include "MPEG4_SongFile.hpp"
#include "MPEG4_Tree.hpp"

namespace Util {
class Stream;
}

namespace Arcusical {
namespace MPEG4 {

struct BoxHeader {
 public:
  BoxType m_type;
  uint64_t m_size;
  bool m_is64BitSize;
};

class MPEG4_Parser final {
 public:
  MPEG4_Parser();
  MPEG4_Parser(MPEG4_Parser& parser) = delete;
  MPEG4_Parser& operator=(MPEG4_Parser& rhs) = delete;
  virtual ~MPEG4_Parser();

  /*
  *	Parse the tree and return it
  */
  std::shared_ptr<MPEG4_SongFile> ReadAndParseFromStream(Util::Stream& stream);

  /*
  *	Takes 2 iterators and parses the contents between them. The boxes parsed in this region will be returned.
  *	Postcondition: startItr will equal end.
  */
  static std::vector<std::shared_ptr<Box>> ParseBoxes(Util::Stream& stream, long long bytesToParse);

  /*
  *	Returns the size of the next box
  *	Precondition: The iterator must point to the start of the next box
  *	[out] is64bit: is the value 32 bit or 64 bit (affects iterator positioning)
  */
  static uint64_t GetSize(Util::Stream& stream, bool& is64bit);

  /*
  *	Returns the type of the box.
  *	Precondition: The iterator must point to the beginning of the box
  *	ie it must point to the size field.
  */
  static BoxType GetType(Util::Stream& stream);

 private:
  /*
  *	Parses the data buffers into the raw mpeg4 tree.
  */
  std::shared_ptr<MPEG4_Tree> ParseTree(Util::Stream& stream);

  /*
  *	Read in the box header from the buffer
  *	Pre: the iterator must be pointing to the start of the box (ie the size field)
  */
  static BoxHeader ReadHeader(Util::Stream& stream);

  /*
  *	The tree that this parser will generate.
  */
  std::shared_ptr<MPEG4_Tree> m_tree;
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/

#endif /*!MPEG4_PARSER_H*/