/*
*	Author: Jesse Tucker
*	Date: December 29th 2012
*	Name: Dref.hpp
*
*	Description: Defines the Dref box. Please see ISO 14496-12 standard, page 38 for details.
*	The Dref box is the Data Reference box, it in turn contains 2 other types of boxes, the
*	Url box and the Urn box. Essentially this is just a container, just with somewhat different
*	parsing rules. See standard for specifics.
*/

#ifndef DREF_HPP
#define DREF_HPP

#include "Box.hpp"

namespace Arcusical { namespace MPEG4 {

	class Dref : public Box
	{
		public:
			Dref();
			virtual ~Dref() override;

			virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

			uint32_t GetVersion();
			uint32_t GetEntryCount();

		protected:
			virtual void ReadContents(Util::Stream& stream) override;

		private:
			uint32_t m_version;
			uint32_t m_entryCount;
	};

} /*namespace: MPEG4*/}/*namespace: Arcusical*/ 

#endif