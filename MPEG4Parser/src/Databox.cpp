#include "pch.h"

#include <memory>

#include "DataBox.hpp"
#include "IncompatibleBoxTypeException.hpp"
#include "MPEG4_Parser.hpp"
/*
*	Author: Jesse Tucker
*	Date: December 31st 2012
*	Name: BoxFactory.hpp
*
*	Description: Defines a general purpose box that contains nothing but raw data.
*/

using namespace std;

namespace Arcusical {
namespace MPEG4 {

DataBox::DataBox() { m_data = make_shared<vector<unsigned char>>(); }

DataBox::~DataBox() {
  // nothing to do
}

std::shared_ptr<std::vector<unsigned char>> DataBox::GetData() { return m_data; }

void DataBox::ReadContents(Util::Stream& stream) {
  // read in the raw bytes
  stream.ReadRawBytes(Util::SafeIntCast<unsigned int>(m_bodySize), *m_data);
}

void DataBox::PrintBox(std::ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "Data Box:" << std::endl;
  outStream << tabs << "\tSize: " << m_bodySize << std::endl;
  outStream << tabs << "\tName: " << *GetBoxName() << std::endl;
}

} /*MPEG4*/
} /*Arcusical*/