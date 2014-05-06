/*
*	Author: Jesse Tucker
*	Date: December 28th 2012
*	Name: Trak.hpp
*
*	Description: The Trak box contains information for a single media track. For
*	details please see ISO 14496-12 page 17-18. The Trak box is simply a container
*	for other boxes.
*/

#ifndef TRAK_HPP
#define TRAK_HPP

#include "ContainerBox.hpp"

namespace Arcusical { namespace MPEG4 {

	class Trak : public ContainerBox
	{
		public:
			Trak();
			virtual ~Trak() override;

			virtual void PrintBox(std::ostream& outStream, int depth = 0) override;
	};

} /*namespace: MPEG4*/}/*namespace: Arcusical*/ 

#endif