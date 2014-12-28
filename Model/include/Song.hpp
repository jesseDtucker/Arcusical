#pragma once

#ifndef SONG_HPP
#define SONG_HPP

#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "boost\uuid\uuid.hpp"

#include "Stream.hpp"
#include "Util.hpp"

namespace FileSystem
{
	class IFile;
}

namespace Arcusical {
namespace Model
{
	enum class AudioFormat
	{
		AAC,
		ALAC,
		FLAC,
		MP3,
		WAV,
		UNKNOWN
	};

	enum class ContainerType
	{
		MP4,
		MP3,
		FLAC,
		WAV,
		UNKNOWN
	};

	struct SongFile
	{
		unsigned int bitRate = 0;
		unsigned int sampleSize = 0;
		unsigned int channelCount = 0;
		AudioFormat format = AudioFormat::UNKNOWN;
		ContainerType container = ContainerType::UNKNOWN;
		std::wstring filePath;
	};

	struct SongStream
	{
		SongFile songData;
		std::shared_ptr<Util::Stream> stream;
	};

	class Song final
	{
	public:
		
		Song();
		Song(const Song&) = default;
		Song& operator=(const Song&) = default;

		PROP_SET_AND_GET(std::wstring, Title);
		PROP_SET_AND_GET(boost::uuids::uuid, Id);
		PROP_SET_AND_GET(std::wstring, Artist);
		PROP_SET_AND_GET(unsigned long long, Length);
		PROP_SET_AND_GET(std::wstring, AlbumName);
		PROP_SET_AND_GET(std::pair<int COMMA int>, TrackNumber);
		PROP_SET_AND_GET(std::pair<int COMMA int>, DiskNumber);

		PROP_GET(std::vector<AudioFormat>, AvailableFormats);
		PROP_GET(std::unordered_map<AudioFormat COMMA SongFile>, Files);
		void AddFile(const SongFile& songFile);

		SongStream GetStream();
		SongStream GetStream(AudioFormat specificFormat);

		bool operator==(const Song& rhs) const;
		bool operator!=(const Song& rhs) const;
		bool IsSameSong(const Song& otherSong) const; // unlike the comparison operators this checks if the songs references the same content, but does not have identical meta data
	
		bool operator<(const Song& other) const;
		bool operator>(const Song& other) const;
		bool operator>=(const Song& other) const;
		bool operator<=(const Song& other) const;
	private:
		AudioFormat DetermineBestFormat();
	};
}
}

#endif