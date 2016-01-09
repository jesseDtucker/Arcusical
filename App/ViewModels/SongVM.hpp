#ifndef SONG_VM_HPP
#define SONG_VM_HPP

#include <memory>

#include "AsyncProcessor.hpp"
#include "PropertyHelper.hpp"
#include "Song.hpp"
#include "Stream.hpp"
#include "Utility/XamlMacros.hpp"

namespace Arcusical {
namespace Player {
class IPlayer;
class Playlist;
}
}

namespace Arcusical {
namespace ViewModel {

// TODO::JT is this needed?
// This enum is expected to match the enum in the song model
public
enum class AudioFormat { UNKNOWN, AAC, ALAC, FLAC, MP3 };

public
ref struct SongStreamVM sealed {
 public:
  property AudioFormat Encoding;
  property unsigned int BitRate;
  property unsigned int SampleSize;
  property unsigned int ChannelCount;
  internal : std::shared_ptr<Util::Stream> Stream;
};

[Windows::UI::Xaml::Data::Bindable] public ref class SongVM sealed : INotifyPropertyChanged_t {
 public:
  NOTIFY_PROPERTY_CHANGED_IMPL;

  PROP_SET_AND_GET_WINRT(Platform::String ^, Title);
  PROP_SET_AND_GET_WINRT(Platform::String ^, Artist);
  PROP_SET_AND_GET_WINRT(Platform::String ^, AlbumPlusArtist);
  PROP_SET_AND_GET_WINRT(int, TrackNumber);
  PROP_SET_AND_GET_WINRT(Platform::String ^, LengthStr);
  PROP_SET_AND_GET_WINRT(unsigned long long, Length);
  PROP_SET_AND_GET_WINRT(AudioFormat, Format);

  // used in lists only
  PROP_SET_AND_GET_WINRT(Platform::Boolean, IsAlternate);

  SongStreamVM ^ GetMediaStream();

  void Play();
  void Pause();

  internal : SongVM(const Model::Song& song, Player::Playlist& playlist, Player::IPlayer& player,
                    Util::BackgroundWorker& worker);
  Model::Song* GetModel();

 private:
  Model::Song m_song;
  Player::Playlist& m_playlist;
  Player::IPlayer& m_player;
  Util::BackgroundWorker& m_worker;
};

} /* ViewModel */
} /* Arcusical */

#endif