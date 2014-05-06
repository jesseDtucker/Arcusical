/*
*	Author: Jesse Tucker
*	Date: December 30th 2012
*	Name: Mp4a.hpp
*
*	Description: Audio Sample Entry box. Contains information on the encoding
*	of the audio as it is specific to the Mp4a atom. This atom usually contains
*	information for tracks encoded in either AAC or MP3.
*/

#ifndef MP4A_HPP
#define MP4A_HPP

#include "Box.hpp"

namespace Arcusical { namespace MPEG4 {

	class Mp4a : public Box
	{
		public:
			Mp4a();
			virtual ~Mp4a() override;

			virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

			uint16_t GetDataReferenceIndex();
			uint16_t GetChannelCount();
			uint16_t GetSampleSize();
			float GetSampleRate();

		protected:
			virtual void ReadContents(Util::Stream& stream) override;

		private:
			uint16_t m_dataReferenceIndex;
			uint16_t m_channelCount;
			uint16_t m_sampleSize;
			float m_sampleRate;	//16.16 fixed point number
	};

} /*namespace: MPEG4*/}/*namespace: Arcusical*/ 

#endif