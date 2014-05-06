/*
*	Author: Jesse Tucker
*	Date: December 27th 2012
*	Name: UnknownBox.hpp
*
*	Description: Defines a box of an unknown type
*/

#include "pch.h"

#include "UnknownBox.hpp"

namespace Arcusical { namespace MPEG4 {

	void UnknownBox::PrintBox(std::ostream& outStream, int depth)
	{
		std::string tabs = GetTabs(depth);

		outStream << tabs << "Unknown Box:" << std::endl;
		outStream << tabs << "\tSize: " << m_bodySize << std::endl;
		outStream << tabs << "\tName: " << *GetBoxName() << std::endl;
	}

}/*MPEG4*/}/*Arcusical*/