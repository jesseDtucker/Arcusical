/*
*	File: iXid.hpp
*	Author: Jesse Tucker
*	Date: January 1st 2013
*
*	Description: This box is an itunes non-standard box that contains a std::string that uniquely identifies
*	any piece of media purchased through the itunes store. The idea is that this code can be used to
*	identify media that has had its filename/metadata modified.
*/

#ifndef IXID_HPP
#define IXID_HPP

#include "Box.hpp"

namespace Arcusical {
namespace MPEG4 {

class iXID : public Box {
 public:
  iXID() {}
  virtual ~iXID() {}

  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

  std::string GetXID();

 protected:
  virtual void ReadContents(Util::Stream& stream) override;

 private:
  std::string m_XID;
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/

#endif