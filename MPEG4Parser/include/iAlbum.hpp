/*
*	Author: Jesse Tucker
*	Date: January 1st 2012
*	Name: iAlbum.hpp
*
*	Description: Itunes specific box. Contains the album name
*/

#ifndef IALBUM_HPP
#define IALBUM_HPP

#include "Box.hpp"

namespace Arcusical { namespace MPEG4 {

	class iAlbum : public Box
	{
		public:
			iAlbum() {}
			virtual ~iAlbum() {}

			virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

			std::string GetAlbumName();

		protected:
			virtual void ReadContents(Util::Stream& stream) override;

		private:

			std::string m_albumName;
	};

} /*namespace: MPEG4*/}/*namespace: Arcusical*/ 

#endif