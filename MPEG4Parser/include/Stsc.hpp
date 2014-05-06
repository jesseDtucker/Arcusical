/*
*	Author: Jesse Tucker
*	Date: December 30th 2012
*	Name: Stsc.hpp
*
*	Description: The STSC box is defined on page 40 of the
*	ISO 14496-12 standard. It is the Sample to chunk box.
*	It defines the number of samples in each chunk.
*/

#ifndef STSC_HPP
#define STSC_HPP

#include "Box.hpp"

namespace Arcusical { namespace MPEG4 {

	struct SampleToChunkEntry
	{
		uint32_t m_firstChunk;
		uint32_t m_samplePerChunk;
		uint32_t m_sampleDescriptionIndex;

		SampleToChunkEntry() : m_firstChunk(0), m_samplePerChunk(0), m_sampleDescriptionIndex(0)
		{}
	};

	class Stsc : public Box
	{
		public:

			Stsc();
			virtual ~Stsc() override;

			virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

			uint32_t GetVersion();
			std::vector<SampleToChunkEntry> GetEntries();

		protected:
			virtual void ReadContents(Util::Stream& stream) override;

		private:

			uint32_t m_version;
			std::vector<SampleToChunkEntry> m_entries;
	};

} /*namespace: MPEG4*/}/*namespace: Arcusical*/

#endif