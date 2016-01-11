#include "pch.h"

#include "Smhd.hpp"

#include "MPEG4_Parser.hpp"

/*
*	Author: Jesse Tucker
*	Date: December 29th 2012
*	Name: Smhd.hpp
*
*	Description: Definition found in ISO 14496-12 standard, page 23.
*	Sound Media Header box, contains information pertaining to the general
*	presentation of the sound data.
*/

namespace Arcusical {
namespace MPEG4 {

Smhd::Smhd() : m_version(0), m_balance(0) {
  // nothing to do
}

Smhd::~Smhd() {
  // nothing to do
}

void Smhd::ReadContents(Util::Stream& stream) {
  m_version = stream.ReadInteger<uint32_t>();
  m_balance = stream.ReadFixedPoint<uint16_t>(8);
  // next 2 bytes are reserved
  stream.Advance(2);
}

void Smhd::PrintBox(std::ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "SMHD Box:" << std::endl;

  outStream << tabs << "\tVersion: " << m_version << std::endl;
  outStream << tabs << "\tBalance: " << m_balance << std::endl;
}

#pragma region Public Getters

uint32_t Smhd::GetVersion() { return m_version; }

float Smhd::GetBalance() { return m_balance; }

#pragma endregion

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/