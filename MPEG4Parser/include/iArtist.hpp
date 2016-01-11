#pragma once

#include "Box.hpp"

/*
*	Author: Jesse Tucker
*	Date: December 29th 2012
*	Name: iName.hpp
*
*	Description: Itunes specific box. Contains the artist of the song
*/

namespace Arcusical {
namespace MPEG4 {

class iArtist : public Box {
 public:
  iArtist() {}
  virtual ~iArtist() {}

  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

  std::string GetArtist();

 protected:
  virtual void ReadContents(Util::Stream& stream) override;

 private:
  std::string m_artist;
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/
