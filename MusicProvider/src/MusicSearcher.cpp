#include "MusicSearcher.hpp"

#include <algorithm>
#include "boost/algorithm/string/predicate.hpp"

#include "Arc_Assert.hpp"
#include "LocalMusicCache.hpp"
#include "XAlgorithm.hpp"


using namespace std;
using namespace Arcusical::LocalMusicStore;
using namespace Arcusical::Model;
using namespace Arcusical::MusicProvider;

// just a little helper to reduce code bloat
#define CHECK_CANCEL                                                   \
if (cancellationToken->IsCanceled())                                   \
{                                                                      \
	return{ cancellationToken };                                       \
}

Arcusical::MusicProvider::SearchResult::SearchResult(Util::CancellationTokenRef token)
	: CancellationToken(token)
{}

MusicSearcher::MusicSearcher(LocalMusicCache* musicCache)
{
	ARC_ASSERT(musicCache != nullptr);
	m_musicCache = musicCache;
}

future<SearchResult> MusicSearcher::Find(wstring searchTerm, Util::CancellationTokenRef cancellationToken)
{
	return std::async([this, searchTerm, cancellationToken]() -> SearchResult
	{
		// just keep pointers initially, we'll compile in order later before returning
		// anything that starts with gets priority in ordering. Songs are top, followed by albums
		// then albums that contain the artist
		std::vector<Song*> songsThatStartWith;
		std::vector<Song*> songsThatContain;
		std::vector<Album*> albumsThatStartWith;
		std::vector<Album*> albumsThatContain;
		std::vector<Album*> startsWithAlbumArtist;
		std::vector<Album*> containsAlbumArtist;

		SearchResult result { cancellationToken };

		// extra scopes because songs and albums are both locked_ptrs, only hold as long as needed and no more
		{
			// search song titles
			auto songs = m_musicCache->GetLocalSongs();
			for (auto& songPair : *songs)
			{
				CHECK_CANCEL;

				auto& song = songPair.second;
				auto& title = song.GetTitle();

				if (boost::istarts_with(title, searchTerm))
				{
					songsThatStartWith.push_back(&song);
				}
				else if (boost::icontains(title, searchTerm))
				{
					songsThatContain.push_back(&song);
				}
			}

			// now that we have the pointers insert the data into the results object
			// we copy the song data over
			result.Songs.resize(songsThatStartWith.size() + songsThatContain.size());
			auto next = begin(result.Songs);
			for (auto& songList : { &songsThatStartWith, &songsThatContain })
			{
				CHECK_CANCEL;

				next = transform(begin(*songList), end(*songList), begin(result.Songs), [](Song* song)
				{
					return *song;
				});
			}
			ARC_ASSERT(next == end(result.Songs));
		}
		{
			auto albums = m_musicCache->GetLocalAlbums();
			for (auto& albumPair : *albums)
			{
				CHECK_CANCEL;

				auto& album = albumPair.second;
				auto& title = album.GetTitle();
				auto& artist = album.GetArtist();

				if (boost::istarts_with(title, searchTerm))
				{
					albumsThatStartWith.push_back(&album);
				}
				else if (boost::icontains(title, searchTerm))
				{
					albumsThatContain.push_back(&album);
				}
				else if (boost::istarts_with(artist, searchTerm))
				{
					startsWithAlbumArtist.push_back(&album);
				}
				else if (boost::icontains(artist, searchTerm))
				{
					containsAlbumArtist.push_back(&album);
				}
			}

			auto foundAlbums = { albumsThatContain, albumsThatStartWith, startsWithAlbumArtist, containsAlbumArtist };
			auto albumCount = Util::Reduce(begin(foundAlbums), end(foundAlbums), 0, [](size_t acc, decltype(albumsThatContain) albums)
			{
				return acc + albums.size();
			});

			// no default constructor available nor do I plan to add one, create a bunch of invalid albums
			result.Albums.resize(albumCount, { nullptr });
			auto next = begin(result.Albums);
			for (auto& albumList : foundAlbums)
			{
				CHECK_CANCEL;

				next = transform(begin(albumList), end(albumList), next, [](Album* album)
				{
					return *album;
				});
			}
			ARC_ASSERT(next == end(result.Albums));
		}

		return result;
	});
}