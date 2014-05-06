/*
*	Author: Jesse Tucker
*	Date: January 13th 2013
*	Description: Itunes specific box. No publicly available documentation. Contains
*	the name of the encoder used to create this file. However there is some unknown
*	information also contained within this box, likely version numbers...
*/

#include "pch.h"

#include "iEncodername.hpp"
#include "MPEG4_Parser.hpp"
#include "DataBox.hpp"

using namespace std;

namespace Arcusical { namespace MPEG4 {

	void iEncoderName::ReadContents(Util::Stream& stream)
	{
		m_children = MPEG4_Parser::ParseBoxes(stream, m_bodySize);

		ARC_ASSERT_MSG(m_children.size() > 0, "iAlbum box with no data!");
		if (m_children.size() > 0)
		{
			auto dataItr = dynamic_pointer_cast<DataBox>(m_children[0])->GetData()->begin();
			auto endData = dynamic_pointer_cast<DataBox>(m_children[0])->GetData()->end();

			//the first 8 bytes are ignored, unsure of what their values are, possibly a version number?
			dataItr += 8;

			m_encoder = std::string(dataItr, endData);
		}
	}

	void iEncoderName::PrintBox(std::ostream& outStream, int depth)
	{
		std::string tabs = GetTabs(depth);

		outStream << tabs << "iEncoderName Box:" << std::endl;
		outStream << tabs << "\tName: " << m_encoder << std::endl;
	}

	std::string iEncoderName::GetEncoderName()
	{
		return m_encoder;
	}

} /*namespace: MPEG4*/}/*namespace: Arcusical*/