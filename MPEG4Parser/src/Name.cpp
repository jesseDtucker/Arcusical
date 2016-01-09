#include "pch.h"

#include "MPEG4_Parser.hpp"
#include "Name.hpp"

/*
*	Author: Jesse Tucker
*	Date: December 31st 2012
*	Name: ItunesDashAtom.hpp
*
*	Description: Itunes specific meta data. Contained within a '----' box (Itunes specific box).
*	This is the name used to identify the data contained within this atom. No idea why custom atoms
*	weren't used here...
*/

namespace Arcusical {
namespace MPEG4 {

void Name::ReadContents(Util::Stream& stream) {
  // first 4 bytes appear to be either reserved or some sort of version number... always zero in my observations
  stream.Advance(4);

  m_name = stream.ReadString(Util::SafeIntCast<unsigned int>(m_bodySize - 4));
}

std::string Name::GetName() { return m_name; }

void Name::PrintBox(std::ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "iName Box:" << std::endl;
  outStream << tabs << "\tName: " << m_name << std::endl;
}

} /*MPEG4*/
} /*Arcusical*/