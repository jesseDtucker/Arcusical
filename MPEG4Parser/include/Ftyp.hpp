#pragma once

#include "Box.hpp"
/*
*	Author: Jesse Tucker
*	Date: December 16th 2012
*	Name: BoxFtyp.hpp
*
*	Description: Defines the ftyp box. Definition found in ISO 14496-12 standard, page 5.
*/


namespace Arcusical {
namespace MPEG4 {

class Ftyp : public Box {
 public:
  Ftyp();
  virtual ~Ftyp() override;

  std::string GetMajorBrand();
  std::string GetMinorVersion();
  std::vector<std::string> GetCompatibleBrands();

  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

 protected:
  virtual void ReadContents(Util::Stream& stream) override;

 private:
  std::string m_majorBrand;
  std::string m_minorVersion;
  std::vector<std::string> m_compatibleBrands;
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/
