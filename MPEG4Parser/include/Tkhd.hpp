/*
*	Author: Jesse Tucker
*	Date: December 28th 2012
*	Name: Tkhd.hpp
*
*	Description: The TKHD box contains is a track header box and contains
*	the details of a single track. Please see ISO 14496-12 page 18-19 for details.
*/

#ifndef TKHD_HPP
#define TKHD_HPP

#include "Box.hpp"

namespace Arcusical { namespace MPEG4 {

	class Tkhd : public Box
	{
		public:
			Tkhd();
			virtual ~Tkhd() override;

			virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

			uint8_t GetVersion();
			uint32_t GetFlags();	//encoded as a 24 bit integer
			uint64_t GetCreationTime();
			uint64_t GetModificationTime();
			uint32_t GetTrackID();
			uint64_t GetDuration();
			uint16_t GetLayer();
			uint64_t GetAlternateGroup();
			float GetVolume();	//8.8 fixed point number
			std::vector<uint32_t> GetMatrix();
			float GetWidth();	//16.16 fixed point number
			float GetHeight();	//16.16 fixed point number

		protected:
			virtual void ReadContents(Util::Stream& stream) override;

		private:
			uint8_t m_version;
			uint32_t m_flags;	//encoded as a 24 bit integer
			uint64_t m_creationTime;
			uint64_t m_modificationTime;
			uint32_t m_trackID;
			uint64_t m_duration;
			uint16_t m_layer;
			uint64_t m_alternateGroup;
			float m_volume;	//8.8 fixed point number
			std::vector<uint32_t> m_matrix;
			float m_width;	//16.16 fixed point number
			float m_height;	//16.16 fixed point number
	};

} /*namespace: MPEG4*/}/*namespace: Arcusical*/ 

#endif