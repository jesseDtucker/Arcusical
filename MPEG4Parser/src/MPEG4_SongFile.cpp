/*
*	Author: Jesse Tucker
*	Date: Feb 24th 2013
*
*	Description:
*	This class wraps the MPEG4_Tree and provides a simple way of accessing and writing its information.
*	TODO::JT: implement means of writing information, currently only works as a reader
*/

#include "pch.h"

#include "boost\type_traits\make_unsigned.hpp"
#include "MPEG4_SongFile.hpp"

namespace Arcusical { namespace MPEG4 {

	MPEG4_SongFile::MPEG4_SongFile(std::shared_ptr<MPEG4_Tree> sourceTree)
	{
		m_tree = sourceTree;
		LoadTree(sourceTree->GetBoxes());
	}

	#pragma region Song Info

	std::string MPEG4_SongFile::GetAlbum()
	{
		if(m_iAlbum != nullptr)
		{
			return m_iAlbum->GetAlbumName();
		}
		else
		{
			return "";
		}
	}

	std::string MPEG4_SongFile::GetArtist()
	{
		if(m_iArtist != nullptr)
		{
			return m_iArtist->GetArtist();
		}
		else
		{
			return "";
		}
	}

	std::string MPEG4_SongFile::GetAlbumArtist()
	{
		if(m_iAlbumArtist != nullptr)
		{
			return m_iAlbumArtist->GetAlbumArtist();
		}
		else
		{
			return "";
		}
	}

	std::string MPEG4_SongFile::GetTitle()
	{
		if(m_iName != nullptr)
		{
			return m_iName->GetTitle();
		}
		else
		{
			return "";
		}
	}

	std::string MPEG4_SongFile::GetReleaseDate()
	{
		if(m_iReleaseDate != nullptr)
		{
			return m_iReleaseDate->GetReleaseDate();
		}
		else
		{
			return "";
		}
	}

	GenreType MPEG4_SongFile::GetGenre()
	{
		if(m_iGenre != nullptr)
		{
			return m_iGenre->GetGenre();
		}
		else
		{
			return GenreType::UNKNOWN;
		}
	}

	std::pair<uint16_t,uint16_t> MPEG4_SongFile::GetTrackNumber()
	{
		if(m_iTrackNumber != nullptr)
		{
			return std::make_pair<uint16_t, uint16_t>(m_iTrackNumber->GetTrackNumber(), m_iTrackNumber->GetMaxTrack());
		}
		else
		{
			return std::make_pair<uint16_t, uint16_t>(0,0);
		}
	}

	std::pair<uint16_t, uint16_t> MPEG4_SongFile::GetDiskNumber()
	{
		if(m_iDiskNumber != nullptr)
		{
			return std::make_pair<uint16_t, uint16_t>(m_iDiskNumber->GetDiskNumber(), m_iDiskNumber->GetMaxDisk());
		}
		else
		{
			return std::make_pair<uint16_t, uint16_t>(0,0);
		}
	}

	/*
	*	Returns the raw image data.
	*/
	std::shared_ptr<std::vector<char>> MPEG4_SongFile::GetImageData()
	{
		if(m_iCover != nullptr)
		{
			if(m_imageData == nullptr)
			{
				// then this is the first call, load the value in
				typedef boost::make_unsigned<std::vector<char>::difference_type>::type UnsignedSize;
				m_imageData = std::make_shared<std::vector<char>>(
					m_iCover->GetImageData(),
					m_iCover->GetImageData() + Util::SafeIntCast<UnsignedSize>(m_iCover->GetImageDataSize()));
			}

			return m_imageData;
		}
		else
		{
			return nullptr;
		}
	}

	#pragma endregion

	#pragma region Playback Information

	Encoding MPEG4_SongFile::GetEncoding()
	{
		// I expect only one of these to exist in the tree. If both exist then
		// a different mechanism will be needed to determine the encoding
		ARC_ASSERT(m_alac != nullptr || m_mp4a != nullptr);
		if (nullptr != m_alac)
		{
			return Encoding::ALAC;
		}
		else if (nullptr != m_mp4a)
		{
			return Encoding::AAC;
		}
		else
		{
			return Encoding::UNKNOWN;
		}
	}

	std::shared_ptr<std::vector<char>> MPEG4_SongFile::GetMediaData()
	{
		if(m_mdat != nullptr)
		{
			//return m_mdat->GetData();
			return nullptr;
		}
		else
		{
			return nullptr;
		}
	}

	uint64_t MPEG4_SongFile::GetLength()
	{
		if(m_mvhd != nullptr)
		{
			return GetDuration() / GetSampleRate();
		}
		else
		{
			return 0;
		}
	}

	uint64_t MPEG4_SongFile::GetDuration()
	{
		if(m_mvhd != nullptr)
		{
			return m_mvhd->GetDuration();
		}
		else
		{
			return 0;
		}
	}

	uint32_t MPEG4_SongFile::GetSampleRate()
	{
		if(m_mvhd != nullptr)
		{
			return m_mvhd->GetTimeScale();
		}
		else
		{
			//typical value
			return 44100;
		}
	}

	float MPEG4_SongFile::GetPlaybackRate()
	{
		if(m_mvhd != nullptr)
		{
			return m_mvhd->GetRate();
		}
		else
		{
			//typical value
			return 1.0;
		}
	}

	float MPEG4_SongFile::GetVolume()
	{
		if(m_mvhd != nullptr)
		{
			return m_mvhd->GetVolume();
		}
		else
		{
			//typical value
			return 1.0;
		}
	}

	uint16_t MPEG4_SongFile::GetNumberOfChannels()
	{
		//may be contained in either the MP4A or the ALAC box
		if(m_mp4a != nullptr)
		{
			return m_mp4a->GetChannelCount();
		}
		else if(m_alac != nullptr)
		{
			return m_alac->GetNumChannels();
		}
		else
		{
			//typical value
			return 2;
		}
	}

	uint16_t MPEG4_SongFile::GetSampleSize()
	{
		if(m_mp4a != nullptr)
		{
			return m_mp4a->GetSampleSize();
		}
		else if(m_alac != nullptr)
		{
			return m_alac->GetSampleSize();
		}
		else
		{
			return 0;
		}
	}

	#pragma endregion

	std::shared_ptr<MPEG4_Tree> MPEG4_SongFile::GetTree()
	{
		return m_tree;
	}

	/*
	*	Loads the tree and indexing all of the information for easy access.
	*/
	void MPEG4_SongFile::LoadTree(std::vector<std::shared_ptr<Box>>& boxes)
	{
		for(std::shared_ptr<Box>& box : boxes)
		{
			LoadBox(box);
			LoadTree(box->GetChildren());
		}
	}

	void MPEG4_SongFile::LoadBox(const std::shared_ptr<Box>& box)
	{
		// bleh... this is an ugly way to do this...
		// consider either refactoring this code or redesigning completely...
		// maybe the visitor pattern?
		// hell... what was wrong with a switch... why did I do this?

		if(LoadBox(std::dynamic_pointer_cast<Alac>(box)))
		{
			return;
		}
		else if(LoadBox(std::dynamic_pointer_cast<Mp4a>(box)))
		{
			return;
		}
		else if(LoadBox(std::dynamic_pointer_cast<Mvhd>(box)))
		{
			return;
		}
		else if(LoadBox(std::dynamic_pointer_cast<Mdat>(box)))
		{
			return;
		}
		else if(LoadBox(std::dynamic_pointer_cast<iName>(box)))
		{
			return;
		}
		else if(LoadBox(std::dynamic_pointer_cast<iCover>(box)))
		{
			return;
		}
		else if(LoadBox(std::dynamic_pointer_cast<iReleaseDate>(box)))
		{
			return;
		}
		else if(LoadBox(std::dynamic_pointer_cast<iTrackNumber>(box)))
		{
			return;
		}
		else if(LoadBox(std::dynamic_pointer_cast<iAlbum>(box)))
		{
			return;
		}
		else if(LoadBox(std::dynamic_pointer_cast<iArtist>(box)))
		{
			return;
		}
		else if(LoadBox(std::dynamic_pointer_cast<iDiskNumber>(box)))
		{
			return;
		}
	}

	bool MPEG4_SongFile::LoadBox(const std::shared_ptr<Alac>& box)
	{
		if(box != nullptr)
		{
			m_alac = box;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool MPEG4_SongFile::LoadBox(const std::shared_ptr<Mp4a>& box)
	{
		if(box != nullptr)
		{
			m_mp4a = box;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool MPEG4_SongFile::LoadBox(const std::shared_ptr<Mvhd>& box)
	{
		if(box != nullptr)
		{
			m_mvhd = box;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool MPEG4_SongFile::LoadBox(const std::shared_ptr<Mdat>& box)
	{
		if(box != nullptr)
		{
			m_mdat = box;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool MPEG4_SongFile::LoadBox(const std::shared_ptr<iName>& box)
	{
		if(box != nullptr)
		{
			m_iName = box;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool MPEG4_SongFile::LoadBox(const std::shared_ptr<iCover>& box)
	{
		if(box != nullptr)
		{
			m_iCover = box;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool MPEG4_SongFile::LoadBox(const std::shared_ptr<iReleaseDate>& box)
	{
		if(box != nullptr)
		{
			m_iReleaseDate = box;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool MPEG4_SongFile::LoadBox(const std::shared_ptr<iArtist>& box)
	{
		if(box != nullptr)
		{
			m_iArtist = box;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool MPEG4_SongFile::LoadBox(const std::shared_ptr<iAlbum>& box)
	{
		if(box != nullptr)
		{
			m_iAlbum = box;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool MPEG4_SongFile::LoadBox(const std::shared_ptr<iTrackNumber>& box)
	{
		if(box != nullptr)
		{
			m_iTrackNumber = box;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool MPEG4_SongFile::LoadBox(const std::shared_ptr<iDiskNumber>& box)
	{
		if(box != nullptr)
		{
			m_iDiskNumber = box;
			return true;
		}
		else
		{
			return false;
		}
	}


} /*namespace: MPEG4*/}/*namespace: Arcusical*/ 