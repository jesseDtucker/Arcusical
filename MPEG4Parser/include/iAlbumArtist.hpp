#pragma once

#include "Box.hpp"

/*
*	Author: Jesse Tucker
*	Date: January 1st 2012
*	Name: iAlbumArtist.hpp
*
*	Description: Itunes specific box. Contains the album artist
*/

namespace Arcusical {
namespace MPEG4 {

class iAlbumArtist : public Box {
 public:
  iAlbumArtist() {}
  virtual ~iAlbumArtist() {}

  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

  std::string GetAlbumArtist();

 protected:
  virtual void ReadContents(Util::Stream& stream) override;

 private:
  std::string m_albumArtist;
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/
