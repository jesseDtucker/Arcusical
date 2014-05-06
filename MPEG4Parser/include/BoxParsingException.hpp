/*
*	Author: Jesse Tucker
*	Date: December 24th 2012
*	Name: BoxFtyp.hpp
*
*	Description: Exception that is to be thrown when a box
*	cannot be succesfully parsed
*/

#include <exception>

#ifndef BOX_PARSING_EXCEPTION_HPP
#define BOX_PARSING_EXCEPTION_HPP

namespace Arcusical { namespace MPEG4 {

	class BoxParsingException : public std::exception
	{
		public:
			BoxParsingException(const char* errMessage):m_message(errMessage){}
			// overriden what() method from exception class
			const char* what() const throw() { return m_message; }
 
		 protected:
			const char* m_message;
	};

}/*Arcusical*/}/*MPEG4*/

#endif