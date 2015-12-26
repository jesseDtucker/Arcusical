/*
*	Author: Jesse Tucker
*	Date: December 29th 2012
*	Name: iName.hpp
*
*	Description: Itunes specific box. Contains the title of the song
*/

#ifndef INAME_HPP
#define INAME_HPP

#include "Box.hpp"

namespace Arcusical {
namespace MPEG4 {

class iName : public Box {
 public:
  iName() {}
  virtual ~iName() {}

  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

  std::string GetTitle();

 protected:
  virtual void ReadContents(Util::Stream& stream) override;

 private:
  std::string m_title;
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/

#endif