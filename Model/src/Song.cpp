#include "pch.h"

#include <algorithm>

#include "Storage.hpp"
#include "IFileReader.hpp"
#include "Song.hpp"
#include "IFile.hpp"

namespace Arcusical{
namespace Model
{
	Song::Song()
		: m_Length(0)
	{
		
	}

	bool Song::operator==(const Song& rhs) const
	{
		return this->GetId() == rhs.GetId();
	}

	bool Song::operator!=(const Song& rhs) const
	{
		return !(*this == rhs);
	}

	bool Song::IsSameSong(const Song& otherSong) const
	{
		// they are considered equal if and only if they have the same title and artist
		return	this->GetTitle() == otherSong.GetTitle() &&
			this->GetArtist() == otherSong.GetArtist();
	}

	void Song::AddFile(const SongFile& songFile)
	{
		auto doesNotHaveFormat = (m_Files.find(songFile.format) == m_Files.end());
		ARC_ASSERT_MSG(doesNotHaveFormat, "A song can have only one file per encoding!");

		if (doesNotHaveFormat)
		{
			m_Files[songFile.format] = songFile;
			m_AvailableFormats.push_back(songFile.format);
		}
	}

	SongStream Song::GetStream()
	{
		ARC_ASSERT_MSG(this->GetFiles().size() > 0, "Cannot get a stream! There are no files associated with this song!");
		return GetStream(DetermineBestFormat());
	}

	SongStream Song::GetStream(AudioFormat specificFormat)
	{
		ARC_ASSERT_MSG(this->GetFiles().size() > 0, "Cannot get a stream! There are no files associated with this song!");

		auto isFormatAvailable = std::find(std::begin(m_AvailableFormats), std::end(m_AvailableFormats), specificFormat) != std::end(m_AvailableFormats);
		ARC_ASSERT_MSG(isFormatAvailable, "Attempted to get a format that is not available!");
		// correct the mistake and pick one for the user
		if (!isFormatAvailable)
		{
			specificFormat = DetermineBestFormat();
		}

		auto& songFile = m_Files[specificFormat];
		auto file = FileSystem::Storage::LoadFileFromPath(songFile.filePath);
		auto fileReader = FileSystem::Storage::GetReader(file);

		SongStream result;

		result.songData = songFile;
		result.stream = std::static_pointer_cast<Util::Stream>(fileReader);

		return result;
	}

	AudioFormat Song::DetermineBestFormat()
	{
		// try and select what I think the best format is likely to be
		// assuming lossless is best, followed by AAC followed by mp3
		static std::vector<AudioFormat> FORMATS =
		{
			AudioFormat::ALAC,
			AudioFormat::FLAC,
			AudioFormat::AAC,
			AudioFormat::MP3
		};

		for (const auto audioFormat : FORMATS)
		{
			auto itr = std::find(std::begin(m_AvailableFormats), std::end(m_AvailableFormats), audioFormat);
			if (itr != std::end(m_AvailableFormats))
			{
				return audioFormat;
			}
		}

		ARC_FAIL("Failed to determine an audio format!");
		ARC_ASSERT(this->GetAvailableFormats().size() > 0);
		// default to whatever is available
		return *this->GetAvailableFormats().begin();
	}
}
}