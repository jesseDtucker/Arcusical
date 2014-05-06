/*
*	Author: Jesse Tucker
*	Date: January 1st 2013
*	Description: Itunes specific box. No publicly available documentation. Reported as
*	being a 32 bit number that uniquely identifies the album artist or artist. Assumed
*	to be of internal use to the itunes store.
*/

#ifndef IATID_HPP
#define IATID_HPP

#include "UnknownBox.hpp"

namespace Arcusical { namespace MPEG4 {

	class iAtID : public UnknownBox
	{
		public:
			iAtID() {}
			virtual ~iAtID() {}
	};

} /*namespace: MPEG4*/}/*namespace: Arcusical*/ 

#endif