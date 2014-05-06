/*
*	Author: Jesse Tucker
*	Date: December 31th 2012
*	Name: Meta.hpp
*
*	Description: Non standard box used to contain Apple's metadata. No official public documentation
*	available. Implementation derived from various online sources and observation of file structure.
*	This box is a container for a variety of other atoms that define the apple atoms
*/

#include "pch.h"

#include "Ilst.hpp"
#include "MPEG4_Parser.hpp"

using namespace std;

namespace Arcusical { namespace MPEG4 {

	Ilst::Ilst() {}

	Ilst::~Ilst() {}

	void Ilst::PrintBox(ostream& outStream, int depth)
	{
		std::string tabs = GetTabs(depth);

		outStream << tabs << "Ilst Box:" << std::endl;

		for(std::shared_ptr<Box> child : m_children)
		{
			child->PrintBox(outStream, depth + 1);
		}
	}

} /*namespace: MPEG4*/}/*namespace: Arcusical*/
