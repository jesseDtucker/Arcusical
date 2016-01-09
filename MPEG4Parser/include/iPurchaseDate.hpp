#pragma once

#include "Box.hpp"

/*
*	File: iPurchaseDate.hpp
*	Author: Jesse Tucker
*	Date: January 1st 2013
*
*	Description: This box is an itunes non-standard box that contains a std::string with the purchase date.
*/

namespace Arcusical {
namespace MPEG4 {

class iPurchaseDate : public Box {
 public:
  iPurchaseDate() {}
  virtual ~iPurchaseDate() {}

  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

  std::string GetPurchaseDate();

 protected:
  virtual void ReadContents(Util::Stream& stream) override;

 private:
  std::string m_purchaseDate;
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/
