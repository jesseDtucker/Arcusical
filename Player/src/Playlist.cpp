#include "Playlist.hpp"

#include <algorithm>
#include <string>

#include "Arc_Assert.hpp"
#include "Exceptions.hpp"
#include "IPlayer.hpp"
#include "MusicProvider.hpp"
#include "Song.hpp"

using namespace std;
using namespace Arcusical::MusicProvider;
using namespace Arcusical::Model;

namespace Arcusical {
namespace Player {

const string Arcusical::Player::Playlist::ServiceName("Playlist");

static const int MAX_HISTORY_SIZE = 400;
static const int RANDOM_SIZE = 25;

Playlist::Playlist(IPlayer* player, MusicProvider::MusicProvider* musicProvider)
    : m_player(player), m_musicProvider(musicProvider) {
  m_songEndedSub = player->GetEnded() += [this]() { PlayNext(); };
}

void Playlist::PlayNext() {
  std::lock_guard<std::recursive_mutex> lock(m_syncLock);
  if (m_SongQueue.empty()) {
    SelectMoreSongs();
  }

  if (!m_SongQueue.empty()) {
    auto nextSong = m_SongQueue.back();
    m_player->SetSong(nextSong);
    m_SongQueue.pop_back();
    try {
      m_player->Play();
      m_RecentlyPlayed.push_back(nextSong);

      if (m_RecentlyPlayed.size() > MAX_HISTORY_SIZE) {
        auto newEnd = copy_n(begin(m_RecentlyPlayed), MAX_HISTORY_SIZE / 2, begin(m_RecentlyPlayed));
        m_RecentlyPlayed.resize(distance(begin(m_RecentlyPlayed), newEnd));
      }
    } catch (Util::NoSongFileAvailable&) {
      // in this case we are just going to try again if there is more in our current queue
      // we won't bother using the default 'find more songs' behavior in the case of an error
      if (!m_SongQueue.empty()) {
        PlayNext();
      } else {
        Clear();
      }
    }
  }
}

void Playlist::PlayPrevious(double goToStartThreshold /* = 5.0 */) {
  std::lock_guard<std::recursive_mutex> lock(m_syncLock);
  if (m_player->GetCurrentTime() > goToStartThreshold || m_RecentlyPlayed.size() <= 1) {
    // then just go to the start of the song
    m_player->SetCurrentTime(0.0);
  } else {
    // start playing the previous song
    if (m_RecentlyPlayed.size() > 1) {
      // put the current song on the queue and then the song before that
      auto curSong = m_RecentlyPlayed.back();
      m_RecentlyPlayed.pop_back();
      auto prevSong = m_RecentlyPlayed.back();
      m_RecentlyPlayed.pop_back();

      m_SongQueue.insert(end(m_SongQueue), {curSong, prevSong});
      PlayNext();
      PlaylistChanged();
    }
  }
}

void Playlist::SkipTo(const Song& song) {
  std::lock_guard<std::recursive_mutex> lock(m_syncLock);
  // we need to find the song. Skipping can go forward or backwards so first create a complete list and then find the
  // song in that.
  vector<const Song*> allSongs;
  allSongs.reserve(m_RecentlyPlayed.size() + m_SongQueue.size());
  auto toPointerFunc = [](const Song& song) { return &song; };
  transform(begin(m_RecentlyPlayed), end(m_RecentlyPlayed), back_inserter(allSongs), toPointerFunc);
  transform(rbegin(m_SongQueue), rend(m_SongQueue), back_inserter(allSongs), toPointerFunc);

  auto songToSkipTo =
      find_if(begin(allSongs), end(allSongs), [&song](const Song* songPtr) { return song == *songPtr; });
  if (songToSkipTo != end(allSongs)) {
    // excellent, we have something to skip to.
    vector<Song> newSongQueue = {};
    newSongQueue.reserve(distance(songToSkipTo, end(allSongs)));
    vector<Song> newRecentlyPlayed = {};
    newRecentlyPlayed.reserve(distance(begin(allSongs), songToSkipTo));

    auto songPtrToSongFunc = [](const Song* songPtr) { return *songPtr; };

    transform(begin(allSongs), songToSkipTo, back_inserter(newRecentlyPlayed), songPtrToSongFunc);
    transform(songToSkipTo, end(allSongs), back_inserter(newSongQueue), songPtrToSongFunc);
    reverse(begin(newSongQueue), end(newSongQueue));

    m_RecentlyPlayed = move(newRecentlyPlayed);
    m_SongQueue = move(newSongQueue);

    PlayNext();

    PlaylistChanged();
  }
}

bool Playlist::TryStartPlayback() {
  // start playback only if no song is playing and it is not currently paused
  bool startNext = !m_player->GetIsPlaying() && !m_player->GetIsPaused();
  if (m_wasRecentlyCleared || startNext) {
    PlayNext();
  }
  m_wasRecentlyCleared = false;

  return startNext;
}

void Playlist::Enqueue(const Song& song, bool startPlayback) {
  std::lock_guard<std::recursive_mutex> lock(m_syncLock);
  m_SongQueue.push_back(song);
  if (startPlayback) {
    m_Shuffle = false;
    TryStartPlayback();
  }
  PlaylistChanged();
}

void Playlist::Clear() {
  std::lock_guard<std::recursive_mutex> lock(m_syncLock);
  m_SongQueue = {};
  m_RecentlyPlayed = {};
  m_Shuffle = false;
  m_wasRecentlyCleared = true;
  m_player->ClearSong();
  PlaylistChanged();
}

void Playlist::SelectMoreSongs() {
  ARC_ASSERT(m_musicProvider != nullptr);

  auto songFilter = [this](const Song& song) {
    return find_if(begin(m_RecentlyPlayed), end(m_RecentlyPlayed),
                   [&song](const Song& playedSong) { return playedSong == song; }) == end(m_RecentlyPlayed);
  };

  auto songSelector = m_musicProvider->GetSongSelector();

  if (m_RecentlyPlayed.size() == 0) {
    m_Shuffle = true;
  }

  if (!m_Shuffle) {
    auto prevSong = m_RecentlyPlayed.back();
    auto albumSongs = songSelector->GetFromSameAlbum(prevSong, songFilter);
    if (albumSongs.size() > 0) {
      Enqueue(albumSongs, false);
    } else {
      auto artistSongs = songSelector->GetFromSameArtist(prevSong, songFilter);
      if (artistSongs.size() > 0) {
        Enqueue(artistSongs, false);
      }
    }
  }

  if (m_SongQueue.size() == 0) {
    m_Shuffle = true;
    auto randomSongs = songSelector->GetRandomSongs(RANDOM_SIZE, songFilter);
    if (randomSongs.size() > 0) {
      Enqueue(randomSongs, false);
    }
  }
}
}
}