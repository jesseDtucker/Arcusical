/*
*	Author: Jesse Tucker
*	Date: December 23rd 2012
*	Name: Box.cpp
*
*	Description: Abstract class that defines the properties of a box. In the MPEG4 format
*	all information is contained within boxes, sometimes called atoms. All boxes have a size,
*	a type, and zero or more children. For more details please see the ISO 14496-12 standard.
*	The box is defined on page 4.
*/

#include "pch.h"

#include <typeinfo>

#include "Arc_Assert.hpp"
#include "Box.hpp"
#include "IncompatibleBoxTypeException.hpp"
#include "MPEG4_Parser.hpp"
#include "Stream.hpp"

using namespace std;

namespace Arcusical { namespace MPEG4 {

	//public
	Box::Box()
		: m_bodySize(0)
		, m_boxName(nullptr)
		, m_fullSize(0)
	{ }

	Box::~Box()
	{
		//nothing to do, love smart pointers!!! :)
	}

	//public
	uint64_t Box::GetSize()
	{
		return m_bodySize;
	}

	//public
	std::vector<std::shared_ptr<Box>>& Box::GetChildren()
	{
		return m_children;
	}

	void Box::ParseBox(uint64_t size, Util::Stream& stream, uint32_t headerSize)
	{
		m_fullSize = size;
		m_bodySize = size - headerSize;

		auto positionBefore = stream.GetPosition();

		ReadContents(stream);

		auto bytesReadByBox = stream.GetPosition() - positionBefore;
		ARC_ASSERT_MSG(bytesReadByBox == m_bodySize, "Box did not read all of the bytes in its box! Bytes read : " << bytesReadByBox << " / " << m_bodySize);
		if (bytesReadByBox != m_bodySize)
		{
			// fix the stream so that errors do not propagate
			auto offset = bytesReadByBox - m_bodySize;
			if (offset > 0)
			{
				stream.Rewind(Util::SafeIntCast<unsigned int>(offset));
			}
			else
			{
				stream.Advance(Util::SafeIntCast<unsigned int>(-1 * offset));
			}
		}
	}

	shared_ptr<string> Box::GetBoxName()
	{
		if(m_boxName == nullptr)
		{
			m_boxName = make_shared<string>(typeid(*this).name());
		}

		return m_boxName;
	}

	//protected static helper function
	const std::string& Box::GetTabs(int count)
	{
		//Note: the following optimization through caching was not motivated by a need for performance improvement
		//while printing but rather a desire to test a particular method of implementing caching, in this
		//case it is doubtful the improvement would be more than negligible... if at all.

		static unordered_map<int, std::string> tabMap;

		if(tabMap.find(count) != tabMap.end())
		{
			return tabMap.at(count);
		}
		else
		{
			std::string newValue = "";
			for(int counter = 0; count > counter ; ++counter)
			{
				newValue.append("\t");
			}

			tabMap.insert(std::make_pair(count, newValue));
			return tabMap.at(count);
		}
	}

} /*Arcusical*/ }/*MPEG4*/