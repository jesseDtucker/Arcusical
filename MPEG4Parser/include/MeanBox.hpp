/*
*	Author: Jesse Tucker
*	Date: December 31st 2012
*	Name: ItunesDashAtom.hpp
*
*	Description: Itunes specific meta data. Contained within a '----' box (Itunes specific box).
*	No official specification available. Appears to be just a std::string in reverse DNS order. Unsure
*	of specific use...
*/

#ifndef MEAN_HPP
#define MEAN_HPP

#include "Box.hpp"
#include <string>

namespace Arcusical {
namespace MPEG4 {

class MeanBox : public Box {
 public:
  MeanBox() {}
  virtual ~MeanBox() {}

  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

  std::string GetDataString();

 protected:
  virtual void ReadContents(Util::Stream& stream) override;

 private:
  std::string m_data;
};

} /*MPEG4*/
} /*Arcusical*/

#endif