#include "pch.h"

#include "MPEG4_Parser.hpp"
#include "MeanBox.hpp"

/*
*	Author: Jesse Tucker
*	Date: December 31st 2012
*	Name: ItunesDashAtom.hpp
*
*	Description: Itunes specific meta data. Contained within a '----' box (Itunes specific box).
*	No official specification available. Appears to be just a std::string in reverse DNS order. Unsure
*	of specific use...
*/

namespace Arcusical {
namespace MPEG4 {

void MeanBox::ReadContents(Util::Stream& stream) {
  // first 4 bytes appear to always be zero, either its a version number or reserved. No way of knowing...
  stream.Advance(4);

  // the remainder is a std::string
  m_data = stream.ReadString(Util::SafeIntCast<unsigned int>(m_bodySize) - 4);
}

void MeanBox::PrintBox(std::ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "Mean Box: " << std::endl;
  outStream << tabs << "\tData: " << m_data << std::endl;
}

std::string MeanBox::GetDataString() { return m_data; }

} /*MPEG4*/
} /*Arcusical*/
