/*
*	Author: Jesse Tucker
*	Date: January 1st 2012
*	Name: iDiskNumber.hpp
*
*	Description: Itunes specific box. Contiains information on the disk number. Includes both the number
*	for this disk and the disk number for the album. ie: track x of y.
*/

#include "pch.h"

#include "iDiskNumber.hpp"
#include "DataBox.hpp"
#include "MPEG4_Parser.hpp"

using namespace std;

namespace Arcusical { namespace MPEG4 {

	void iDiskNumber::ReadContents(Util::Stream& stream)
	{
		m_children = MPEG4_Parser::ParseBoxes(stream, m_bodySize);

		//extract the data
		ARC_ASSERT_MSG(m_children.size() > 0, "iAlbum box with no data!");
		if (m_children.size() > 0)
		{
			std::vector<unsigned char>& data = *dynamic_pointer_cast<DataBox>(m_children[0])->GetData();

			auto dataItr = data.begin() + 10;
			m_diskNumber = Util::ReadInteger<uint16_t>(dataItr);
			m_maxDisk = Util::ReadInteger<uint16_t>(dataItr + sizeof(uint16_t));
		}
	}

	void iDiskNumber::PrintBox(ostream& outStream, int depth)
	{
		std::string tabs = GetTabs(depth);

		outStream << tabs << "iDisk Number Box:" << std::endl;
		outStream << tabs << "\tDisk " << m_diskNumber << " of " << m_maxDisk << std::endl;
	}
		
	uint16_t iDiskNumber::GetDiskNumber()
	{
		return m_diskNumber;
	}
		
	uint16_t iDiskNumber::GetMaxDisk()
	{
		return m_maxDisk;
	}

}/*Arcusical*/}/*MPEG4*/