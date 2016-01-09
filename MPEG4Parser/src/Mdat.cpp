#include "pch.h"

#include "Mdat.hpp"
/*
*	Author: Jesse Tucker
*	Date: January 19th 2013
*	File: mdat.hpp
*
*	Description: This file contains the raw media data. The contents of this data will be specific to the
*	media type.
*/

namespace Arcusical {
namespace MPEG4 {

void Mdat::ReadContents(Util::Stream& stream) {
  // TODO::JT figure out how to handle this case!
  // Note: current code is not at all safe!
  stream.Advance(Util::SafeIntCast<unsigned int>(m_bodySize));
}

void Mdat::PrintBox(std::ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "Data Box:" << std::endl;
  outStream << tabs << "\tSize: " << m_bodySize << std::endl;
  outStream << tabs << "\tName: " << *GetBoxName() << std::endl;
}

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/