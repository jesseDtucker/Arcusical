#include "pch.h"

#include "Tkhd.hpp"

#include "InvalidValueException.hpp"
#include "MPEG4_Parser.hpp"

/*
*	Author: Jesse Tucker
*	Date: December 28th 2012
*	Name: Tkhd.cpp
*
*	Description: The TKHD box contains is a track header box and contains
*	the details of a single track. Please see ISO 14496-12 page 18-19 for details.
*/

using namespace std;

namespace Arcusical {
namespace MPEG4 {

Tkhd::Tkhd()
    : m_version(0),
      m_flags(0),
      m_creationTime(0),
      m_modificationTime(0),
      m_trackID(0),
      m_duration(0),
      m_layer(0),
      m_alternateGroup(0),
      m_volume(0),
      m_width(0),
      m_height(0) {
  // Thats it!
}

Tkhd::~Tkhd() {
  // Nothing to do!
}

void Tkhd::ReadContents(Util::Stream& stream) {
  m_version = stream.ReadInteger<uint8_t>();
  // flags is actually a 24 bit integer
  m_flags = stream.ReadInteger<uint32_t, 3>();

  if (m_version == 1) {
    // then 64 bit values are used, track id is exception
    m_creationTime = stream.ReadInteger<uint64_t>();
    m_modificationTime = stream.ReadInteger<uint64_t>();
    m_trackID = stream.ReadInteger<uint32_t>();
    // next 32 bits are reserved
    stream.Advance(4);
    m_duration = stream.ReadInteger<uint64_t>();
  } else if (m_version == 0) {
    // then 32 bit values are used
    m_creationTime = stream.ReadInteger<uint32_t>();
    m_modificationTime = stream.ReadInteger<uint32_t>();
    m_trackID = stream.ReadInteger<uint32_t>();
    // next 32 bits are reserved
    stream.Advance(4);
    m_duration = stream.ReadInteger<uint32_t>();
  } else {
    throw InvalidValueException("Version number must be either 0 or 1!");
  }

  // next 2 32 bit integers are reserved
  stream.Advance(8);
  m_layer = stream.ReadInteger<uint16_t>();
  m_alternateGroup = stream.ReadInteger<uint16_t>();
  m_volume = stream.ReadFixedPoint<uint16_t>(8);
  // next 16 bits are reserved
  stream.Advance(2);
  for (int index = 0; index < 9; ++index) {
    m_matrix.push_back(stream.ReadInteger<uint32_t>());
  }

  m_width = stream.ReadFixedPoint<uint32_t>(16);
  m_height = stream.ReadFixedPoint<uint32_t>(16);
}

void Tkhd::PrintBox(ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "TKHD Box:" << std::endl;

  outStream << tabs << "\tVersion: " << static_cast<int>(m_version) << std::endl;  // cast is to ensure correct display
  outStream << tabs << "\tFlags: " << m_flags << std::endl;
  outStream << tabs << "\tCreation Time: " << m_creationTime << std::endl;
  outStream << tabs << "\tModification Time: " << m_modificationTime << std::endl;
  outStream << tabs << "\tTrack ID: " << m_trackID << std::endl;
  outStream << tabs << "\tDuration: " << m_duration << std::endl;
  outStream << tabs << "\tLayer: " << m_layer << std::endl;
  outStream << tabs << "\tAlternate Group: " << m_alternateGroup << std::endl;
  outStream << tabs << "\tVolume: " << m_volume << std::endl;
  outStream << tabs << "\tWidth: " << m_width << std::endl;
  outStream << tabs << "\tHeight: " << m_height << std::endl;
}

#pragma region public Getters

uint8_t Tkhd::GetVersion() { return m_version; }

// TODO: break this up into the flags values!
uint32_t Tkhd::GetFlags() { return m_flags; }

uint64_t Tkhd::GetCreationTime() { return m_creationTime; }

uint64_t Tkhd::GetModificationTime() { return m_modificationTime; }

uint32_t Tkhd::GetTrackID() { return m_trackID; }

uint64_t Tkhd::GetDuration() { return m_duration; }

uint16_t Tkhd::GetLayer() { return m_layer; }

uint64_t Tkhd::GetAlternateGroup() { return m_alternateGroup; }

float Tkhd::GetVolume() { return m_volume; }

std::vector<uint32_t> Tkhd::GetMatrix() { return m_matrix; }

float Tkhd::GetWidth() { return m_width; }

float Tkhd::GetHeight() { return m_height; }

#pragma endregion

} /*MPEG4*/
} /*Arcusical*/