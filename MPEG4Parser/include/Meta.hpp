#pragma once

#include "Box.hpp"

/*
*	Author: Jesse Tucker
*	Date: December 30th 2012
*	Name: Meta.hpp
*
*	Description: The Meta box is within the user data box. It is defined on page 56
*	of the ISO 14496-12 standard. This box is a container for various types of metadata.
*/

namespace Arcusical {
namespace MPEG4 {

class Meta : public Box {
 public:
  Meta();
  virtual ~Meta() override;

  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

  uint32_t GetVersion();

 protected:
  virtual void ReadContents(Util::Stream& stream) override;

 private:
  uint32_t m_version;
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/
