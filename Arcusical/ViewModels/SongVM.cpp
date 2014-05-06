#include "pch.h"

#include <future>
#include <unordered_map>

#include "IPlayer.hpp"
#include "Commands/CommandBase.hpp"
#include "Commands/CommandWrapper.hpp"
#include "Song.hpp"
#include "SongVM.hpp"

using namespace Windows::UI::Xaml::Data;

namespace Arcusical{
namespace ViewModel {

	static std::unordered_map<Model::AudioFormat, ViewModel::AudioFormat> FORMAT_MAPPING =
	{
		{ Model::AudioFormat::AAC, ViewModel::AudioFormat::AAC },
		{ Model::AudioFormat::ALAC, ViewModel::AudioFormat::ALAC },
		{ Model::AudioFormat::FLAC, ViewModel::AudioFormat::FLAC },
		{ Model::AudioFormat::MP3, ViewModel::AudioFormat::MP3 }
	};

	SongVM::SongVM()
		: m_playSongCommand([](SongVM^){})
		, m_song(nullptr)
	{
		Title = "";
		Artist = "";
		Length = 0;
		Format = AudioFormat::UNKNOWN;
		PlayPauseSong = ref new Commands::CommandWrapper(m_playSongCommand);
	}

	SongVM::SongVM(std::shared_ptr<Model::Song> song)
		: m_song(song)
		, m_playSongCommand([this](SongVM^ selectedSong){ ARC_ASSERT( selectedSong == nullptr || selectedSong == this); this->Play(); })
	{
		Title = ref new Platform::String(m_song->GetTitle().c_str());
		Artist = ref new Platform::String(m_song->GetArtist().c_str());
		Length = m_song->GetLength();
		PlayPauseSong = ref new Commands::CommandWrapper(m_playSongCommand);
	}

	std::shared_ptr<Model::Song> SongVM::GetModel()
	{
		return m_song;
	}

	Commands::PlaySongCommand& SongVM::PlaySong()
	{
		return m_playSongCommand;
	}

	SongStreamVM^ SongVM::GetMediaStream()
	{
		auto songStream = m_song->GetStream();

		SongStreamVM^ streamVM = ref new SongStreamVM();
		streamVM->BitRate = songStream.songData.bitRate;
		streamVM->ChannelCount = songStream.songData.channelCount;
		streamVM->SampleSize = songStream.songData.sampleSize;
		streamVM->Stream = songStream.stream;

		return streamVM;
	}

	void SongVM::Play()
	{
		std::async([this]()
		{
			auto player = Player::PlayerLocator::ResolveService().lock();
			ARC_ASSERT(player != nullptr)
			if (player != nullptr)
			{
			if (player->GetCurrentSong() == m_song)
			{
				player->Stop();
			}
			else
			{
				player->SetSong(m_song);
				player->Play();
			}
			}
		});
	}

	Platform::Boolean SongVM::IsPlaying::get()
	{
		bool isPlaying = false;
		if (m_song != nullptr)
		{
			auto player = Player::PlayerLocator::ResolveService().lock();
			ARC_ASSERT(player != nullptr);
			if (player != nullptr)
			{
				isPlaying = player->GetCurrentSong() == this->GetModel();
			}
		}

		return isPlaying;
	}

	SongVM^ SongVM::GetEmptySong()
	{
		static SongVM^ emptySong = ref new SongVM();
		return emptySong;
	}
}
}