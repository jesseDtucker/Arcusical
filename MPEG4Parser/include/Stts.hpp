/*
*	Author: Jesse Tucker
*	Date: December 30th 2012
*	Name: Stts.hpp
*
*	Description: Defines the STTS box. Definition found in ISO 14496-12 standard, page 32.
*	This is the Decoding Time to Sample Box. It contains information on the time to sample
*	map. 
*/

#ifndef STTS_HPP
#define STTS_HPP

#include "Box.hpp"

namespace Arcusical { namespace MPEG4 {

	struct SampleEntry
	{
		uint32_t m_sampleCount;
		uint32_t m_sampleDelta;

		SampleEntry() : m_sampleCount(0), m_sampleDelta(0)
		{}
	};

	class Stts : public Box
	{
		public:

			Stts();
			virtual ~Stts() override;

			virtual void PrintBox(std::ostream& outStream, int depth = 0) override;
	
			uint32_t GetVersion();
			std::vector<SampleEntry> GetEntries();

		protected:
			virtual void ReadContents(Util::Stream& stream) override;

		private:
			uint32_t m_version;
			std::vector<SampleEntry> m_entries;
	};

} /*namespace: MPEG4*/}/*namespace: Arcusical*/ 

#endif