/*
*	Author: Jesse Tucker
*	Date: December 24th 2012
*	Name: BoxFtyp.hpp
*
*	Description: Exception that is to be thrown when the type of
*	a box does not match the box that is being parsed.
*/

#include "BoxParsingException.hpp"

#ifndef UNKNOWN_BOX_TYPE_HPP
#define UNKNOWN_BOX_TYPE_HPP

namespace Arcusical { namespace MPEG4 {

	class UnknownBoxTypeException : public BoxParsingException
	{
		public:
			UnknownBoxTypeException(const char* errMessage):BoxParsingException(errMessage){}
	};

}/*Arcusical*/}/*MPEG4*/

#endif