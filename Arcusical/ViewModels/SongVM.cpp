#include "pch.h"

#include <future>
#include <string>
#include <unordered_map>

#include "IPlayer.hpp"
#include "Playlist.hpp"
#include "Song.hpp"
#include "SongVM.hpp"
#include "Utility/DispatcherHelper.hpp"

using namespace std;
using namespace Windows::UI::Xaml::Data;

using namespace Arcusical::Player;

namespace Arcusical{
namespace ViewModel {

	static unordered_map<Model::AudioFormat, ViewModel::AudioFormat> FORMAT_MAPPING =
	{
		{ Model::AudioFormat::AAC, ViewModel::AudioFormat::AAC },
		{ Model::AudioFormat::ALAC, ViewModel::AudioFormat::ALAC },
		{ Model::AudioFormat::FLAC, ViewModel::AudioFormat::FLAC },
		{ Model::AudioFormat::MP3, ViewModel::AudioFormat::MP3 }
	};

	SongVM::SongVM(const Model::Song& song, Playlist& playlist, IPlayer& player)
		: m_song(song)
		, m_playlist(playlist)
		, m_player(player)
	{
		m_Title = ref new Platform::String(m_song.GetTitle().c_str());
		m_Artist = ref new Platform::String(m_song.GetArtist().c_str());
		m_Length = m_song.GetLength();

		if (m_song.GetTrackNumber().first != 0)
		{
			m_Title = ref new Platform::String((to_wstring(m_song.GetTrackNumber().first) + L" - ").c_str()) + Title;
		}
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
		async([this]()
		{
			m_playlist.Clear();
			m_playlist.Enqueue(m_song);
		});
	}

	void SongVM::Pause()
	{
		async([this]()
		{
			m_player.Stop();
		});
	}
}
}