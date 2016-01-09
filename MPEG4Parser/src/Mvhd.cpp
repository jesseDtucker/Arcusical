#include "pch.h"

#include "InvalidValueException.hpp"
#include "MPEG4_Parser.hpp"
#include "Mvhd.hpp"

/*
*	Author: Jesse Tucker
*	Date: December 27th 2012
*	Name: Mvhd.hpp
*
*	Description: Movie Header Box definition as defined on page 16 of
*	ISO 14496-12. Contains media independant meta data.
*/

namespace Arcusical {
namespace MPEG4 {

Mvhd::Mvhd()
    : m_version(0),
      m_creationTime(0),
      m_modificationTime(0),
      m_timeScale(0),
      m_duration(0),
      m_rate(0),
      m_volume(0),
      m_nextTrackId(0) {
  // everything is now initialized
}

Mvhd::~Mvhd() {
  // nothing to do
}

void Mvhd::ReadContents(Util::Stream& stream) {
  // format can be found on page 16 of iso standard
  m_version = stream.ReadInteger<uint32_t>();

  if (m_version == 1) {
    // then 64 bit values are used (timescale is exception)
    m_creationTime = stream.ReadInteger<uint64_t>();
    m_modificationTime = stream.ReadInteger<uint64_t>();
    m_timeScale = stream.ReadInteger<uint32_t>();
    m_duration = stream.ReadInteger<uint64_t>();
  } else if (m_version == 0) {
    // then 32 bit values are used
    m_creationTime = stream.ReadInteger<uint32_t>();
    m_modificationTime = stream.ReadInteger<uint32_t>();
    m_timeScale = stream.ReadInteger<uint32_t>();
    m_duration = stream.ReadInteger<uint32_t>();
  } else {
    // invalid value!
    throw InvalidValueException("Version can only be 0 or 1!");
  }

  m_rate = stream.ReadFixedPoint<uint32_t>(16);
  m_volume = stream.ReadFixedPoint<uint16_t>(8);
  // the next 16 bits are reserved, ie: don't care
  // they are then followed by 2 32 bit integers that are reserved
  stream.Advance(10);
  // the next 9 sets of 32 bit integers define a movie playback matrix
  for (int index = 0; index < 9; ++index) {
    m_matrix.push_back(stream.ReadInteger<uint32_t>());
  }

  // the next 6 32 bit integers are defined to be zero, ie: don't care
  stream.Advance(24);

  m_nextTrackId = stream.ReadInteger<uint32_t>();
}

void Mvhd::PrintBox(std::ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "MVHD Box: " << std::endl;

  outStream << tabs << "\tVersion: " << m_version << std::endl;
  outStream << tabs << "\tCreation Time: " << m_creationTime << std::endl;
  outStream << tabs << "\tModification Time: " << m_modificationTime << std::endl;
  outStream << tabs << "\tTime Scale: " << m_timeScale << std::endl;
  outStream << tabs << "\tDuration: " << m_duration << std::endl;
  outStream << tabs << "\tRate: " << m_rate << std::endl;
  outStream << tabs << "\tVolume: " << m_volume << std::endl;
  outStream << tabs << "\tNext Track ID: " << m_nextTrackId << std::endl;
}

#pragma region public Getters

uint32_t Mvhd::GetVersion() { return m_version; }

uint64_t Mvhd::GetCreationTime() { return m_creationTime; }

uint64_t Mvhd::GetModificationTime() { return m_modificationTime; }

uint32_t Mvhd::GetTimeScale() { return m_timeScale; }

uint64_t Mvhd::GetDuration() { return m_duration; }

float Mvhd::GetRate() { return m_rate; }

float Mvhd::GetVolume() { return m_volume; }

std::vector<uint32_t> Mvhd::GetMatrix() { return m_matrix; }

uint32_t Mvhd::GetNextTrackId() { return m_nextTrackId; }

#pragma endregion

} /*Arcusical*/
} /*MPEG4*/