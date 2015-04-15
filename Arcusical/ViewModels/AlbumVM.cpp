#include "pch.h"

#include <future>

#include "Album.hpp"
#include "AlbumVM.hpp"
#include "Playlist.hpp"

using namespace Windows::UI::Xaml::Data;

using namespace std;
using namespace::Arcusical::Player;

namespace Arcusical{
namespace ViewModel{
	
	AlbumVM::AlbumVM(const Model::Album& album, Playlist& playlist, IPlayer& player)
		: m_album(album)
		, m_playlist(playlist)
		, m_songs(nullptr)
		, m_player(player)
	{
		this->Artist = ref new Platform::String(album.GetArtist().c_str());
		this->Title = ref new Platform::String(album.GetTitle().c_str());
		this->ImagePath = ref new Platform::String(album.GetImageFilePath().c_str());
	}

	void AlbumVM::Play()
	{
		std::async([this]()
		{
			m_playlist.Clear();
			m_playlist.Enqueue(*m_album.GetSongs());
		});
	}


	SongListVM^ AlbumVM::Songs::get()
	{
		if (m_songs == nullptr)
		{
			auto songs = m_album.GetSongs();
			m_songs = ref new SongListVM(*songs, m_playlist, m_player);

			DispatchToUI([this]()
			{
				OnPropertyChanged("Songs");
			});
		}

		return m_songs;
	}

} /*ViewModel*/
} /*Arcusical*/