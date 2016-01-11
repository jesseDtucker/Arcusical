#pragma once

#include "Box.hpp"

/*
*	Author: Jesse Tucker
*	Date: December 30th 2012
*	Name: Stsd.hpp
*
*	Description: Sample Description Box. stsd box, defined in ISO 14496-12 page 28.
*	Contains information describing the mediums sample information.
*/

namespace Arcusical {
namespace MPEG4 {

class Stsd : public Box {
 public:
  Stsd();
  virtual ~Stsd() override;

  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

 protected:
  virtual void ReadContents(Util::Stream& stream) override;
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/
