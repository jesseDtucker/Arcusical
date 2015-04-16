#include "pch.h"

#include <algorithm>
#include "boost/algorithm/string/predicate.hpp"
#include "boost/exception/all.hpp"
#include <limits>

#include "Exceptions.hpp"
#include "IFileReader.hpp"
#include "IFile.hpp"
#include "Song.hpp"
#include "Storage.hpp"

#undef max
#undef min

using namespace std;
using namespace Util;

namespace Arcusical{
namespace Model
{
	Song::Song()
		: m_Length(0)
		, m_TrackNumber({ 0, 0 })
		, m_DiskNumber({ 0, 0 })
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
		
		return	boost::iequals(this->GetTitle(), otherSong.GetTitle()) &&
				boost::iequals(this->GetArtist(), otherSong.GetArtist());
	}

	bool Song::operator<(const Song& other) const
	{
		// ordering is defined by a series of precedents
		// disk number takes priority
		// followed by track number
		// followed by lexicographical order of the title
		// 
		// zeros are considered 'no info' and so are considered to be after all other elements

		const auto MAX_INT = numeric_limits<int>::max();

		auto thisDiskNum = m_DiskNumber.first;
		auto thisTrackNum = m_TrackNumber.first;

		auto otherDiskNum = other.m_DiskNumber.first;
		auto otherTrackNum = other.m_TrackNumber.first;

		// treat zero as Int.max, we want those that have info to come first, others come second
		thisDiskNum = thisDiskNum != 0 ? thisDiskNum : MAX_INT;
		thisTrackNum = thisTrackNum != 0 ? thisTrackNum : MAX_INT;

		otherDiskNum = otherDiskNum != 0 ? otherDiskNum : MAX_INT;
		otherTrackNum = otherTrackNum != 0 ? otherTrackNum : MAX_INT;

		if (thisDiskNum != MAX_INT || otherDiskNum != MAX_INT)
		{
			// they one has a disk number, if they don't match compare those instead of the track number
if (thisDiskNum != otherDiskNum)
{
	return thisDiskNum < otherDiskNum;
}
		}

		if (thisTrackNum != MAX_INT || otherTrackNum != MAX_INT)
		{
			// one of them has a track number, compare that
			if (thisTrackNum != otherTrackNum)
			{
				return thisTrackNum < otherTrackNum;
			}
		}

		return m_Title < other.m_Title;
	}

	bool Song::operator>(const Song& other) const
	{
		return !(*this <= other);
	}

	bool Song::operator>=(const Song& other) const
	{
		return (*this == other) || (*this > other);
	}

	bool Song::operator<=(const Song& other) const
	{
		return (*this == other) || (*this < other);
	}

	void Song::AddFile(const SongFile& songFile)
	{
		auto doesNotHaveFormat = (m_Files.find(songFile.format) == m_Files.end());
		ARC_ASSERT_MSG(doesNotHaveFormat, "A song can have only one file per encoding!");

		if (doesNotHaveFormat)
		{
			m_Files[songFile.format] = songFile;
			m_AvailableFormats.insert(songFile.format);
		}
	}

	bool Song::HasStream()
	{
		return this->GetFiles().size() > 0;
	}

	SongStream Song::GetStream()
	{
		ARC_ASSERT_MSG(HasStream(), "Cannot get a stream! There are no files associated with this song!");
		return GetStream(DetermineBestFormat());
	}

	SongStream Song::GetStream(AudioFormat specificFormat)
	{
		ARC_ASSERT_MSG(HasStream(), "Cannot get a stream! There are no files associated with this song!");

		auto isFormatAvailable = find(begin(m_AvailableFormats), end(m_AvailableFormats), specificFormat) != end(m_AvailableFormats);
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
		result.stream = static_pointer_cast<Util::Stream>(fileReader);

		return result;
	}

	AudioFormat Song::DetermineBestFormat()
	{
		// try and select what I think the best format is likely to be
		// assuming lossless is best, followed by AAC followed by mp3
		static vector<AudioFormat> FORMATS =
		{
			AudioFormat::ALAC,
			AudioFormat::FLAC,
			AudioFormat::WAV,
			AudioFormat::AAC,
			AudioFormat::MP3
		};

		for (const auto audioFormat : FORMATS)
		{
			auto itr = find(begin(m_AvailableFormats), end(m_AvailableFormats), audioFormat);
			if (itr != end(m_AvailableFormats))
			{
				auto& songFile = m_Files[audioFormat];
				if (FileSystem::Storage::FileExists(songFile.filePath))
				{
					return audioFormat;
				}
			}
		}

		throw NoSongFileAvailable() << errinfo_msg("There are no files available in any format for this song!");
	}
}
}