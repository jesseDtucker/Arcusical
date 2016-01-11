#include "pch.h"

#include "Hdlr.hpp"

#include "MPEG4_Parser.hpp"

/*
*	Author: Jesse Tucker
*	Date: December 29th 2012
*	Name: Hdlr.hpp
*
*	Description: Defines the HDLR box. Definition found in ISO 14496-12 standard, page 22.
*	The HDLR is a handler reference box. It
*/

using namespace std;

namespace Arcusical {
namespace MPEG4 {

Hdlr::Hdlr() : m_version(0), m_handleType(HandlerType::Unknown), m_name("") {
  // nothing to do
}

Hdlr::~Hdlr() {
  // still good, nothing to do
}

void Hdlr::ReadContents(Util::Stream& stream) {
  auto startPosition = stream.GetPosition();

  // Note: this box doesn't use version... see page 22 of standard
  m_version = stream.ReadInteger<uint32_t>();

  // 4 bytes of reserved.
  stream.Advance(4);
  std::string handleValue = stream.ReadString(4);
  auto type = StringToHandlerType.find(handleValue);
  if (type != StringToHandlerType.end()) {
    m_handleType = type->second;
  } else {
    m_handleType = HandlerType::Unknown;
  }

  // next 12 bytes are reserved
  stream.Advance(12);

  auto bytesRead = stream.GetPosition() - startPosition;

  // null terminated std::string
  m_name = stream.ReadString(Util::SafeIntCast<unsigned int>(m_bodySize - bytesRead));
}

void Hdlr::PrintBox(ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "HDLR Box: " << std::endl;
  outStream << tabs << "\tVersion: " << m_version << std::endl;
  outStream << tabs << "\tHandle Type: " << m_handleType << std::endl;
  outStream << tabs << "\tName: " << m_name << std::endl;
}

#pragma region Public Getters

uint32_t Hdlr::GetVersion() { return m_version; }

HandlerType Hdlr::GetHandleType() { return m_handleType; }

std::string Hdlr::GetName() { return m_name; }

#pragma endregion

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/