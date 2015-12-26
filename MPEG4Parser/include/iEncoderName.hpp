/*
*	Author: Jesse Tucker
*	Date: January 13th 2013
*	Description: Itunes specific box. No publicly available documentation. Contains
*	the name of the encoder used to create this file. However there is some unknown
*	information also contained within this box, likely version numbers...
*/

#ifndef IENCODERNAME_HPP
#define IENCODERNAME_HPP

#include "Box.hpp"

namespace Arcusical {
namespace MPEG4 {

class iEncoderName : public Box {
 public:
  iEncoderName() {}
  virtual ~iEncoderName() {}

  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

  std::string GetEncoderName();

 protected:
  virtual void ReadContents(Util::Stream& stream) override;

 private:
  std::string m_encoder;
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/

#endif