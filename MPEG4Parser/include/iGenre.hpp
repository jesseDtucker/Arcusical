/*
*	Author: Jesse Tucker
*	Date: January 1st 2012
*	Name: iGenre.hpp
*
*	Description: Itunes specific box. Contains a code that determines the genre. Code is 16 bits
*	and genre is determined by its mapping to the ID3 genres. However the actual coded value is one larger.
*/

#ifndef IGENRE_HPP
#define IGENRE_HPP

#include "Box.hpp"
#include "GenreTypes.hpp"

namespace Arcusical { namespace MPEG4 {

	class iGenre : public Box
	{
		public:

			iGenre() : m_genre(GenreType::UNKNOWN) {}
			virtual ~iGenre() {}

			virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

			GenreType GetGenre();
			std::wstring GetNonStandardGenre();

		protected:
			virtual void ReadContents(Util::Stream& stream) override;

		private:

			GenreType m_genre;
			std::wstring m_nonStandardGenre;
	};

}/*Arcusical*/}/*MPEG4*/

#endif