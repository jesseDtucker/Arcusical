/*
*	Author: Jesse Tucker
*	Date: December 31st 2012
*	Name: ItunesDashAtom.hpp
*
*	Description: Itunes specific meta data. The dash atom has a format code of '----' and contains
*	3 other boxes. A name box, a mean box and a data box.
*/

#ifndef ITUNES_DASH_ATOM_HPP
#define ITUNES_DASH_ATOM_HPP

#include "ContainerBox.hpp"

namespace Arcusical { namespace MPEG4 {

	class ItunesDashBox : public ContainerBox
	{
		public:

			ItunesDashBox() {}
			virtual ~ItunesDashBox() {}

			virtual void PrintBox(std::ostream& outStream, int depth = 0) override;
	};

}/*MPEG4*/}/*Arcusical*/

#endif