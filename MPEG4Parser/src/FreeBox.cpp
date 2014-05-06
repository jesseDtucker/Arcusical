/*
*	Author: Jesse Tucker
*	Date: December 27th 2012
*	Name: BoxFactory.hpp
*
*	Description: Defines a free box. This is a box that acts as a spacer and simply takes up space
*/

#include "pch.h"

#include "FreeBox.hpp"
#include "Stream.hpp"

using namespace std;

namespace Arcusical { namespace MPEG4 {

	FreeBox::FreeBox()
	{
		//Nothing to do
	}

	FreeBox::~FreeBox()
	{
		//Naughta
	}

	void FreeBox::ReadContents(Util::Stream& stream)
	{
		//the contents of the box are ignored, just advance the iterator to the
		//correct location
		stream.Advance(Util::SafeIntCast<unsigned int>(m_bodySize));
	}

	void FreeBox::PrintBox(ostream& outStream, int depth)
	{
		std::string tabs = GetTabs(depth);

		outStream << tabs << "Free Box: " << std::endl;
		outStream << tabs << "\tSize: " << m_bodySize << std::endl;
	}

}/*MPEG4*/}/*Arcusical*/