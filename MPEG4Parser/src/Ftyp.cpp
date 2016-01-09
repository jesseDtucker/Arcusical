#include "pch.h"

#include "Ftyp.hpp"
#include "IncompatibleBoxTypeException.hpp"
#include "MPEG4_Parser.hpp"

/*
*	Author: Jesse Tucker
*	Date: December 23rd 2012
*	Name: BoxFtyp.hpp
*
*	Description: Defines the ftyp box. Definition found in ISO 14496-12 standard, page 5.
*/

using namespace std;

namespace Arcusical {
namespace MPEG4 {

Ftyp::Ftyp() {
  m_majorBrand = "";
  m_minorVersion = "";
}

Ftyp::~Ftyp() {
  // no things to do!
}

// public
std::string Ftyp::GetMajorBrand() { return m_majorBrand; }

// public
std::string Ftyp::GetMinorVersion() { return m_minorVersion; }

// public
std::vector<std::string> Ftyp::GetCompatibleBrands() { return m_compatibleBrands; }

// public, inherited from Box
void Ftyp::ReadContents(Util::Stream& stream) {
  // first 4 bytes are major brand, next 4 are minor brand, ever sequence of 4 past that is
  // compatible brands
  m_majorBrand = stream.ReadString(4);
  m_minorVersion = stream.ReadString(4);

  auto bytesRead = 8;

  while (bytesRead < m_bodySize) {
    m_compatibleBrands.push_back(stream.ReadString(4));
    bytesRead += 4;
  }
}

void Ftyp::PrintBox(ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "FTYP Box: " << std::endl;
  outStream << tabs << "\tMajor Brand: " << m_majorBrand << std::endl;
  outStream << tabs << "\tMinor Version: " << m_minorVersion << std::endl;

  outStream << tabs << "\tCompatible Brands: " << std::endl;
  for (std::string val : m_compatibleBrands) {
    outStream << "\t\t" << val << std::endl;
  }
}

} /*Arcusical*/
} /*MPEG4*/