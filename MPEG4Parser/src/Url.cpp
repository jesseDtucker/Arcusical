/*
*	Author: Jesse Tucker
*	Date: December 29th 2012
*	Name: Url.hpp
*
*	Description: Defines the Url box. Please see ISO 14496-12 standard, page 38 for details.
*	This box contains the information on the track location. This value is a URL, however
*	if the track is contained within the same file then no url std::string will be present
*	in this box
*/

#include "pch.h"

#include "Url.hpp"
#include "MPEG4_Parser.hpp"

namespace Arcusical { namespace MPEG4 {

	Url::Url() : m_version(0), m_flags(0), m_url("")
	{
		//work is done
	}

	Url::~Url()
	{
		//nothing to do
	}

	void Url::ReadContents(Util::Stream& stream)
	{
		m_version = stream.ReadInteger<uint8_t>();
		m_flags = stream.ReadInteger<uint32_t, 3>();

		if(m_flags != 1)
		{
			m_url = stream.ReadString(Util::SafeIntCast<unsigned int>(m_bodySize - 4));
		}
		//else no std::string exists, done parsing.
	}

	void Url::PrintBox(std::ostream& outStream, int depth)
	{
		std::string tabs = GetTabs(depth);

		outStream << tabs << "URL Box:" << std::endl;
		outStream << tabs << "\tVersion: " << static_cast<int>(m_version) << std::endl;	//cast is to ensure correct display
		outStream << tabs << "\tFlags: " << m_flags << std::endl;
		outStream << tabs << "\tURL: " << m_url << std::endl;
	}

	#pragma region Public Getters

	uint8_t Url::GetVersion()
	{
		return m_version;
	}

	uint32_t Url::GetFlags()
	{
		return m_flags;
	}
	std::string Url::GetUrl()
	{
		return m_url;
	}

	#pragma endregion

} /*namespace: MPEG4*/}/*namespace: Arcusical*/ 