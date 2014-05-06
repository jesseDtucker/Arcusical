/*
*	Author: Jesse Tucker
*	Date: January 13th 2013
*	Description: Itunes specific box. No publicly available documentation. Contains
*	the name of the Composer.
*/

#ifndef ICOMPOSER_HPP
#define ICOMPOSER_HPP

#include "Box.hpp"

namespace Arcusical { namespace MPEG4 {

	class iComposer : public Box
	{
		public:
			iComposer() {}
			virtual ~iComposer() {}

			virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

			std::string GetComposer();

		protected:
			virtual void ReadContents(Util::Stream& stream) override;

		private:

			std::string m_composer;
	};

} /*namespace: MPEG4*/}/*namespace: Arcusical*/ 

#endif