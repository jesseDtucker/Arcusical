#pragma once

#include <string>

#include "Box.hpp"
/*
*	Author: Jesse Tucker
*	Date: December 31st 2012
*	Name: ItunesDashAtom.hpp
*
*	Description: Itunes specific meta data. Contained within a '----' box (Itunes specific box).
*	This is the name used to identify the data contained within this atom. No idea why custom atoms
*	weren't used here...
*/


namespace Arcusical {
namespace MPEG4 {

class Name : public Box {
 public:
  Name() {}
  virtual ~Name() {}

  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

  std::string GetName();

 protected:
  virtual void ReadContents(Util::Stream& stream) override;

 private:
  std::string m_name;
};

} /*MPEG4*/
} /*Arcusical*/
