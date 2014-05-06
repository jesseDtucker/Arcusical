#include "pch.h"

#include <codecvt>

#include "Arc_Assert.hpp"
#include "IFileReader.hpp"
#include "IFile.hpp"
#include "Song.hpp"
#include "SongLoader.hpp"
#include "Storage.hpp"

#include <sstream>
#include <string>

namespace Arcusical
{
namespace MusicProvider
{
	static std::unordered_map<MPEG4::Encoding, Model::AudioFormat> MPEG4_TO_MODEL_MAPPING =
	{
		{MPEG4::Encoding::AAC, Model::AudioFormat::AAC},
		{ MPEG4::Encoding::ALAC, Model::AudioFormat::ALAC },
		{ MPEG4::Encoding::UNKNOWN, Model::AudioFormat::UNKNOWN }
	};

	std::shared_ptr<Model::Song> SongLoader::LoadSong(std::shared_ptr<FileSystem::IFile> file)
	{
		std::shared_ptr<Model::Song> result;
		auto fileExtension = file->GetExtension();

		auto fileName = file->GetFullName();

		if (fileExtension == L"m4a")
		{
			auto fileReader = FileSystem::Storage::GetReader(file);
			auto mpegSong = m_mpegParser.ReadAndParseFromStream(*fileReader);

			result = std::make_shared<Model::Song>();

			//setup converter
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

			result->SetArtist(converter.from_bytes(mpegSong->GetArtist()));
			result->SetId(m_idGenerator()); // generate a random id, the odds of collision are slim to none
			result->SetLength(mpegSong->GetLength());
			result->SetTitle(converter.from_bytes(mpegSong->GetTitle()));

			Model::AudioFormat songFormat = Model::AudioFormat::UNKNOWN;
			auto encodingItr = MPEG4_TO_MODEL_MAPPING.find(mpegSong->GetEncoding());
			ARC_ASSERT(encodingItr != std::end(MPEG4_TO_MODEL_MAPPING));
			if (encodingItr != std::end(MPEG4_TO_MODEL_MAPPING))
			{
				songFormat = encodingItr->second;
			}

			Model::SongFile songFile;
			songFile.filePath = file->GetFullPath();
			songFile.bitRate = mpegSong->GetSampleSize() * mpegSong->GetSampleRate();
			songFile.channelCount = mpegSong->GetNumberOfChannels();
			songFile.sampleSize = mpegSong->GetNumberOfChannels();
			songFile.format = songFormat;
			result->AddFile(songFormat, songFile);

			ARC_ASSERT(songFile.bitRate != 0);
			ARC_ASSERT(songFile.channelCount != 0);
			ARC_ASSERT(songFile.sampleSize != 0);
		}
		else if (fileExtension == L"mp3")
		{
			result = std::make_shared<Model::Song>();

			result->SetArtist(L"Some Artist");
			result->SetId(m_idGenerator()); // generate a random id, the odds of collision are slim to none
			result->SetLength(100);
			result->SetTitle(L"Some MP3");

			Model::AudioFormat songFormat = Model::AudioFormat::MP3;

			Model::SongFile songFile;
			songFile.filePath = file->GetFullPath();
			songFile.bitRate = 0;
			songFile.channelCount = 0;
			songFile.sampleSize = 0;
			songFile.format = songFormat;
			result->AddFile(songFormat, songFile);
		}
		else if (fileExtension == L"wav")
		{
			result = std::make_shared<Model::Song>();

			result->SetArtist(L"Some Artist");
			result->SetId(m_idGenerator()); // generate a random id, the odds of collision are slim to none
			result->SetLength(100);
			result->SetTitle(L"Some WAV");

			Model::AudioFormat songFormat = Model::AudioFormat::MP3;

			Model::SongFile songFile;
			songFile.filePath = file->GetFullPath();
			songFile.bitRate = 0;
			songFile.channelCount = 0;
			songFile.sampleSize = 0;
			songFile.format = songFormat;
			result->AddFile(songFormat, songFile);
		}

		ARC_ASSERT_MSG(result != nullptr, "Attempted to load an unsupported format!");

		return result;
		
	}

}
}