#include "pch.h"

#include "ItunesDashBox.hpp"
#include "MPEG4_Parser.hpp"
/*
*	Author: Jesse Tucker
*	Date: December 31st 2012
*	Name: ItunesDashAtom.hpp
*
*	Description: Itunes specific meta data. The dash atom has a format code of '----' and contains
*	3 other boxes. A name box, a mean box and a data box.
*/

using namespace std;

namespace Arcusical {
namespace MPEG4 {

void ItunesDashBox::PrintBox(ostream& outStream, int depth) {
  outStream << GetTabs(depth) << "ItunesDashBox:" << std::endl;

  for (std::shared_ptr<Box> child : m_children) {
    child->PrintBox(outStream, depth + 1);
  }
}

} /*MPEG4*/
} /*Arcusical*/