#include "pch.h"

#include <future>
#include <unordered_map>

#include "IPlayer.hpp"
#include "Playlist.hpp"
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

	SongVM::SongVM(const Model::Song& song)
		: m_song(song)
	{
		Title = ref new Platform::String(m_song.GetTitle().c_str());
		Artist = ref new Platform::String(m_song.GetArtist().c_str());
		Length = m_song.GetLength();
	}

	Model::Song* SongVM::GetModel()
	{
		return &m_song;
	}

	SongStreamVM^ SongVM::GetMediaStream()
	{
		auto songStream = m_song.GetStream();

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
			auto playList = Player::PlaylistLocator::ResolveService().lock();
			ARC_ASSERT(playList != nullptr);
			if (playList != nullptr)
			{
				playList->Enqueue(m_song);
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
			ARC_ASSERT(player != nullptr);
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
		auto player = Player::PlayerLocator::ResolveService().lock();
		ARC_ASSERT(player != nullptr);
		if (player != nullptr)
		{
			isPlaying = player->GetIsPlaying() && *player->GetCurrentSong() == *this->GetModel();
		}

		return isPlaying;
	}
}
}