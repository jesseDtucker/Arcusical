#pragma once

#include "boost/optional.hpp"
#include <memory>

#include "MulticastDelegate.hpp"
#include "PropertyHelper.hpp"
#include "Song.hpp"

namespace Arcusical {
namespace Player {
class IPlayer {
 public:
  IPlayer() : m_IsPlaying(false), m_IsPaused(false) {}
  IPlayer(const IPlayer&) = delete;
  IPlayer& operator=(const IPlayer&) = delete;
  virtual ~IPlayer() = default;

  static const std::string ServiceName;

  virtual void SetSong(const Model::Song& song) = 0;
  virtual Model::Song* GetCurrentSong() = 0;
  PROP_GET(bool, IsPlaying);  // true, playback is occurring, false, may be paused or stopped
  PROP_GET(bool, IsPaused);

  virtual void Play() = 0;
  virtual void Stop() = 0;
  virtual void ClearSong() = 0;

  virtual double GetDuration() const = 0;        // in seconds
  virtual double GetCurrentTime() const = 0;     // in seconds
  virtual void SetCurrentTime(double time) = 0;  // in seconds

  virtual void SetVolume(double volume) = 0;
  virtual double GetVolume() const = 0;

  PROP_GET_EX(Util::MulticastDelegate<void(double)>, DurationChanged, m_durationChanged, NOT_CONST);
  PROP_GET_EX(Util::MulticastDelegate<void(bool)>, Playing, m_playing, NOT_CONST);
  PROP_GET_EX(Util::MulticastDelegate<void()>, Ended, m_ended, NOT_CONST);
  PROP_GET_EX(Util::MulticastDelegate<void(double)>, TimeUpdate, m_timeUpdate, NOT_CONST);
  PROP_GET_EX(Util::MulticastDelegate<void(const boost::optional<Model::Song>&)>, SongChanged, m_songChanged,
              NOT_CONST);
};
}
}
