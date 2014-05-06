/*
*	Author: Jesse Tucker
*	Date: January 13th 2013
*	Description: Itunes specific box. No publicly available documentation. Reported as
*	being the box that stores the tempo however unsure as to the layout of this box.
*/

#ifndef ITMPO_HPP
#define ITMPO_HPP

#include "UnknownBox.hpp"

namespace Arcusical { namespace MPEG4 {

	class iTmpo : public UnknownBox
	{
		public:
			iTmpo() {}
			virtual ~iTmpo() {}
	};

} /*namespace: MPEG4*/}/*namespace: Arcusical*/ 

#endif