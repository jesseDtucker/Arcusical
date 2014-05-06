/*
*	Author: Jesse Tucker
*	Date: January 1st 2013
*	Description: Itunes specific box. No publicly available documentation. Appears to contain
*	a value indicating the rating of the song. However as observed data and online references
*	disagree on exact values this box is being treated as an unknown.
*/

#ifndef IRTNG_HPP
#define IRTNG_HPP

#include "UnknownBox.hpp"

namespace Arcusical { namespace MPEG4 {

	class iRtng : public UnknownBox
	{
		public:
			iRtng() {}
			virtual ~iRtng() {}
	};

} /*namespace: MPEG4*/}/*namespace: Arcusical*/ 

#endif