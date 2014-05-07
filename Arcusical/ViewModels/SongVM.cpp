#include "pch.h"

#include <future>
#include <unordered_map>

#include "IPlayer.hpp"
#include "Commands/CommandBase.hpp"
#include "Commands/CommandWrapper.hpp"
#include "Song.hpp"
#include "SongVM.hpp"
#include "Utility/DispatcherHelper.hpp"

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
		, m_pauseSongCommand([](SongVM^){})
		, m_song(nullptr)
	{
		Title = "";
		Artist = "";
		Length = 0;
		Format = AudioFormat::UNKNOWN;
		PlaySongCmd = ref new Commands::CommandWrapper(m_playSongCommand);
		PauseSongCmd = ref new Commands::CommandWrapper(m_pauseSongCommand);
	}

	SongVM::SongVM(std::shared_ptr<Model::Song> song)
		: m_song(song)
		, m_playSongCommand([this](SongVM^ selectedSong){ ARC_ASSERT( selectedSong == nullptr || selectedSong == this); this->Play(); })
		, m_pauseSongCommand([this](SongVM^ selectedSong){ ARC_ASSERT(selectedSong == nullptr || selectedSong == this); this->Pause(); })
	{
		Title = ref new Platform::String(m_song->GetTitle().c_str());
		Artist = ref new Platform::String(m_song->GetArtist().c_str());
		Length = m_song->GetLength();
		PlaySongCmd = ref new Commands::CommandWrapper(m_playSongCommand);
		PauseSongCmd = ref new Commands::CommandWrapper(m_pauseSongCommand);
	}

	std::shared_ptr<Model::Song> SongVM::GetModel()
	{
		return m_song;
	}

	Commands::PlaySongCommand& SongVM::PlaySong()
	{
		return m_playSongCommand;
	}

	Commands::PlaySongCommand& SongVM::PauseSong()
	{
		return m_pauseSongCommand;
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
				player->SetSong(m_song);
				player->Play();
			}

			DispatchToUI([this]()
			{
				this->OnPropertyChanged("IsPlaying");
			});
		});
	}

	void SongVM::Pause()
	{
		std::async([this]()
		{
			auto player = Player::PlayerLocator::ResolveService().lock();
			ARC_ASSERT(player != nullptr)
			if (player != nullptr)
			{
				player->Stop();
			}

			DispatchToUI([this]()
			{
				this->OnPropertyChanged("IsPlaying");
			});
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
				isPlaying = player->GetIsPlaying() && player->GetCurrentSong() == this->GetModel();
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