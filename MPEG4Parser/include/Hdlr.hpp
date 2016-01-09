#pragma once

#include "Box.hpp"
#include "HandlerTypes.hpp"
/*
*	Author: Jesse Tucker
*	Date: December 29th 2012
*	Name: Hdlr.hpp
*
*	Description: Defines the HDLR box. Definition found in ISO 14496-12 standard, page 22.
*	The HDLR is a handler reference box.
*/


namespace Arcusical {
namespace MPEG4 {

class Hdlr : public Box {
 public:
  Hdlr();
  virtual ~Hdlr() override;

  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

  uint32_t GetVersion();
  HandlerType GetHandleType();
  std::string GetName();  // Note: this name exists for debugging and inspection purpose.
                          // please see page 22 of standard for details.

 protected:
  virtual void ReadContents(Util::Stream& stream) override;

 private:
  uint32_t m_version;  // Note: standard does not appear to use this value for this box... but it is still encoded...
  HandlerType m_handleType;
  std::string m_name;
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/
