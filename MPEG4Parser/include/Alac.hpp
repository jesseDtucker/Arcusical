/*
*	Author: Jesse Tucker
*	Date: January 10th 2013
*	Name: Alac.hpp
*
*	Description: Defines the box used to contain information needed for decoding ALAC
*	data.
*/

#ifndef ALAC_HPP
#define ALAC_HPP

#include "Box.hpp"

namespace Arcusical { namespace MPEG4 {

	#pragma region ALAC Channel Layout Enum

	/*
	*	The following is pulled from the ALAC Magic Cookie Description.
	*	Source: http://alac.macosforge.org/trac/browser/trunk/ALACMagicCookieDescription.txt
	*/
	enum class AlacChannelLayout
	{
		ALACChannelLayoutTag_Mono           = (100<<16) | 1,        // C
		ALACChannelLayoutTag_Stereo         = (101<<16) | 2,        // L R
		ALACChannelLayoutTag_MPEG_3_0_B     = (113<<16) | 3,        // C L R
		ALACChannelLayoutTag_MPEG_4_0_B     = (116<<16) | 4,        // C L R Cs
		ALACChannelLayoutTag_MPEG_5_0_D     = (120<<16) | 5,        // C L R Ls Rs
		ALACChannelLayoutTag_MPEG_5_1_D     = (124<<16) | 6,        // C L R Ls Rs LFE
		ALACChannelLayoutTag_AAC_6_1        = (142<<16) | 7,        // C L R Ls Rs Cs LFE
		ALACChannelLayoutTag_MPEG_7_1_B     = (127<<16) | 8,        // C Lc Rc L R Ls Rs LFE    (doc: IS-13818-7 MPEG2-AAC)
		UNKNOWN
	};

	const std::unordered_map<uint32_t, AlacChannelLayout> NumToAlacChannelLayout = boost::assign::map_list_of<uint32_t, AlacChannelLayout>
		((100<<16) | 1, AlacChannelLayout::ALACChannelLayoutTag_Mono)
		((101<<16) | 2, AlacChannelLayout::ALACChannelLayoutTag_Stereo)
		((113<<16) | 3, AlacChannelLayout::ALACChannelLayoutTag_MPEG_3_0_B)
		((116<<16) | 4, AlacChannelLayout::ALACChannelLayoutTag_MPEG_4_0_B)
		((120<<16) | 5, AlacChannelLayout::ALACChannelLayoutTag_MPEG_5_0_D)
		((124<<16) | 6, AlacChannelLayout::ALACChannelLayoutTag_MPEG_5_1_D)
		((142<<16) | 7, AlacChannelLayout::ALACChannelLayoutTag_AAC_6_1)
		((127<<16) | 8, AlacChannelLayout::ALACChannelLayoutTag_MPEG_7_1_B);

	const std::unordered_map<AlacChannelLayout, std::string> AlacChannelLayoutToString = boost::assign::map_list_of<AlacChannelLayout, std::string>
		(AlacChannelLayout::ALACChannelLayoutTag_Mono, "Mono")
		(AlacChannelLayout::ALACChannelLayoutTag_Stereo, "Stereo")
		(AlacChannelLayout::ALACChannelLayoutTag_MPEG_3_0_B, "Center Left Right")
		(AlacChannelLayout::ALACChannelLayoutTag_MPEG_4_0_B, "C L R Cs")
		(AlacChannelLayout::ALACChannelLayoutTag_MPEG_5_0_D, "C L R Ls Rs")
		(AlacChannelLayout::ALACChannelLayoutTag_MPEG_5_1_D, "C L R Ls Rs LFE")
		(AlacChannelLayout::ALACChannelLayoutTag_AAC_6_1, "C L R Ls Rs LFE")
		(AlacChannelLayout::ALACChannelLayoutTag_MPEG_7_1_B, "C Lc Rc L R Ls Rs LFE")
		(AlacChannelLayout::UNKNOWN, "Unknown");

	std::ostream& operator << (std::ostream& outStream, AlacChannelLayout layout);

	#pragma endregion



	class Alac : public Box
	{
		public:

			Alac() :
				 m_numChannels(0),
				 m_sampleSize(0),
				 m_sampleRate(0),
				 m_samplePerFrame(0),
				 m_compatibleVersion(0),
				 m_pb(0),
				 m_mb(0),
				 m_kb(0),
				 m_maxRun(0),
				 m_maxFrameSize(0),
				 m_avgBitRate(0),
				 m_version(0),
				 m_channelLayout(AlacChannelLayout::UNKNOWN) {}

			virtual ~Alac() {}

			virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

			uint16_t GetNumChannels();
			uint16_t GetSampleSize();
			uint32_t GetSampleRate();
			uint32_t GetSamplePerFrame();
			uint8_t GetCompatibleVersion();
			uint32_t GetMaxFrameSize();
			uint32_t GetAvgBitRate();
			uint32_t GetVersion();
			uint16_t GetIndex();
			AlacChannelLayout GetChannelLayout();

		protected:
			virtual void ReadContents(Util::Stream& stream) override;

		private:

			uint32_t m_sampleRate;
			uint32_t m_samplePerFrame;
			uint32_t m_maxFrameSize;
			uint32_t m_avgBitRate;
			uint32_t m_version;
			uint16_t m_numChannels;
			uint16_t m_sampleSize;
			uint16_t m_maxRun;	//unused, default = 0xFF
			uint8_t m_compatibleVersion;
			uint8_t m_pb;	//unused tuning variable
			uint8_t m_mb;	//unused tuning variable
			uint8_t m_kb;	//unused tuning variable
			
			
			uint16_t m_index;
			AlacChannelLayout m_channelLayout;
	};

}/*MPEG4*/}/*Arcusical*/

#endif