/*
*	Author: Jesse Tucker
*	Date: December 29th 2012
*	Name: Url.hpp
*
*	Description: Defines the Url box. Please see ISO 14496-12 standard, page 38 for details.
*	This box contains the information on the track location. This value is a URL, however
*	if the track is contained within the same file then no url std::string will be present
*	in this box
*/

#ifndef URL_HPP
#define URL_HPP

#include "Box.hpp"

namespace Arcusical {
namespace MPEG4 {

class Url : public Box {
 public:
  Url();
  virtual ~Url() override;

  virtual void ReadContents(Util::Stream& stream) override;
  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

  uint8_t GetVersion();
  uint32_t GetFlags();
  std::string GetUrl();

 private:
  uint8_t m_version;
  uint32_t m_flags;
  std::string m_url;
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/

#endif