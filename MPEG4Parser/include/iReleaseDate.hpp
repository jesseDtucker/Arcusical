/*
*	Author: Jesse Tucker
*	Date: December 30th 2012
*	Name: Pinf.hpp
*
*	Description: Non-Standard box introduced by apple. Contains a std::string representing the release date.
*	Note: online sources indicate this field may not always be identical format, sometimes it may only
*	be the year.
*/

#ifndef IRELEASE_DATE_HPP
#define IRELEASE_DATE_HPP

#include "Box.hpp"

namespace Arcusical {
namespace MPEG4 {

class iReleaseDate : public Box {
 public:
  iReleaseDate() {}
  virtual ~iReleaseDate(){};

  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

  std::string GetReleaseDate();

 protected:
  virtual void ReadContents(Util::Stream& stream) override;

 private:
  std::string m_releaseDate;
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/

#endif