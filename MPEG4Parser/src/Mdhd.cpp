/*
*	Author: Jesse Tucker
*	Date: December 29th 2012
*	Name: Mdia.hpp
*
*	Description: Media header box. Declares information specific to the media
*	in a single track. Defined on page 21 of the ISO 14496-12 standard.
*/

#include "pch.h"

#include "Mdhd.hpp"
#include "MPEG4_Parser.hpp"
#include "InvalidValueException.hpp"

namespace Arcusical { namespace MPEG4 {

	Mdhd::Mdhd() :	m_creationTime(0), m_modificationTime(0), m_timeScale(0),
					m_duration(0), m_version(0)
	{

	}

	Mdhd::~Mdhd()
	{
		//Nothing needed here!
	}

	//public
	void Mdhd::ReadContents(Util::Stream& stream)
	{
		m_version = stream.ReadInteger<uint32_t>();

		if(m_version == 1)
		{
			//then 64 bit values are used, timescale is exception
			m_creationTime = stream.ReadInteger<uint64_t>();
			m_modificationTime = stream.ReadInteger<uint64_t>();
			m_timeScale = stream.ReadInteger<uint32_t>();
			m_duration = stream.ReadInteger<uint64_t>();
		}
		else if(m_version == 0)
		{
			//then 32 bit values are used
			m_creationTime = stream.ReadInteger<uint32_t>();
			m_modificationTime = stream.ReadInteger<uint32_t>();
			m_timeScale = stream.ReadInteger<uint32_t>();
			m_duration = stream.ReadInteger<uint32_t>();
		}
		else
		{
			throw InvalidValueException("Version can only be zero or one!");
		}

		//language is stored as three 5 bit integers, read them all in at once then fill
		//out the std::vector.
		//Note: did not check ISO 639-2/T for specifics oh how this works. This value
		//may not be decoded correctly! The value is supposed to be a language code
		uint32_t language = stream.ReadInteger<uint16_t>();
		for(int index = 0 ; index < 3 ; ++index)
		{
			m_language.push_back(language & 0x1F);
			language = language >> 5;
		}

		//next 2 bytes are zero
		stream.Advance(2);
	}

	void Mdhd::PrintBox(std::ostream& outStream, int depth)
	{
		std::string tabs = GetTabs(depth);

		outStream << tabs << "MDHD Box:" << std::endl;

		outStream << tabs << "\tVersion: " << m_version << std::endl;
		outStream << tabs << "\tCreation Time: " << m_creationTime << std::endl;
		outStream << tabs << "\tModification Time: " << m_modificationTime << std::endl;
		outStream << tabs << "\tTime Scale: " << m_timeScale << std::endl;
		outStream << tabs << "\tDuration: " << m_duration << std::endl;
	}

	#pragma region public Getters

	uint64_t Mdhd::GetCreationTime()
	{
		return m_creationTime;
	}

	uint64_t Mdhd::GetModificationTime()
	{
		return m_modificationTime;
	}

	uint32_t Mdhd::GetTimeScale()
	{
		return m_timeScale;
	}

	uint64_t Mdhd::GetDuration()
	{
		return m_duration;
	}

	std::vector<uint8_t> Mdhd::GetLanguage()
	{
		return m_language;
	}

	#pragma endregion

} /*namespace: MPEG4*/}/*namespace: Arcusical*/ 