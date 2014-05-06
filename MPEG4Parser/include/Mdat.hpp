/*
*	Author: Jesse Tucker
*	Date: January 3rd 2013
*	File: mdat.hpp
*
*	Description: This file contains the raw media data. The contents of this data will be specific to the
*	media type.
*/

#ifndef MDAT_HPP
#define MDAT_HPP

#include "DataBox.hpp"

namespace Arcusical { namespace MPEG4 {

	class Mdat : public Box
	{
		public:
			Mdat() {}
			virtual ~Mdat() {}

			virtual void ReadContents(Util::Stream& stream) override;
			virtual void PrintBox(std::ostream& outStream, int depth = 0) override;
	};

} /*namespace: MPEG4*/}/*namespace: Arcusical*/ 

#endif