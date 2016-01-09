#pragma once

#include "Box.hpp"

/*
*	File: iAppleID.hpp
*	Author: Jesse Tucker
*	Date: January 1st 2013
*
*	Description: The is a non-standard box added by apple. This box contains the id of the person
*	who purchased the music. Note: appears to be the apple id of the person at time of purchase, apple does
*	not appear to update if person changes their ID unless the person edits the metadata after the change.
*/

namespace Arcusical {
namespace MPEG4 {

class iAppleID : public Box {
 public:
  iAppleID() {}
  virtual ~iAppleID() {}

  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

  std::string GetID();

 protected:
  virtual void ReadContents(Util::Stream& stream) override;

 private:
  std::string m_ID;
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/
