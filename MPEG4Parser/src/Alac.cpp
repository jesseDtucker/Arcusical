#include "pch.h"

#include "Alac.hpp"

#include "InvalidValueException.hpp"
#include "MPEG4_Parser.hpp"
#include "Stream.hpp"

/*
*	Author: Jesse Tucker
*	Date: January 11th 2013
*	Name: Alac.hpp
*
*	Description: Defines the box used to contain information needed for decoding ALAC
*	data.
*/

using namespace std;

namespace Arcusical {
namespace MPEG4 {

void Alac::ReadContents(Util::Stream& stream) {
  auto initialPosition = stream.GetPosition();
  // first 6 bytes are reserved
  stream.Advance(6);

  m_index = stream.ReadInteger<uint16_t>();

  // 8 bytes reserved
  stream.Advance(8);

  m_numChannels = stream.ReadInteger<uint16_t>();
  m_sampleSize = stream.ReadInteger<uint16_t>();

  // 4 bytes reserved
  stream.Advance(4);

  // Note: documentation claims it is a 32 bit unsigned integer, encoding
  // suggests that is is either a 16 bit integer or a 16.16 floating point
  // number, assuming 16 bit integer.
  // TODO::JT resolve this ambiguity!
  m_sampleRate = stream.ReadInteger<uint16_t>();

  // 2 bytes of unknown or reserved, see previous note about sample rate
  stream.Advance(2);

  // next begins the inner ALAC box, note: not using normal parsing means due to identical tags!
  // note: assuming 32 bit numbers only! hypothetically this could be 64 bit extended
  // but I'm assuming that this never occurs given the box is not supposed to be more that 48
  // bytes long.
  uint32_t innerSize = stream.ReadInteger<uint32_t>();

  std::string tag = stream.ReadString(4);

  if (innerSize > 0 && tag.compare("alac") == 0) {
    // this box can be either 28 bytes or 52 bytes depending upon whether the channel info is also provided
    // however the first 24 bytes are required and will always be there.
    m_version = stream.ReadInteger<uint32_t>();
    m_samplePerFrame = stream.ReadInteger<uint32_t>();
    m_compatibleVersion = stream.ReadInteger<uint8_t>();

    // sample size is repeated here
    auto otherSampleSize = stream.ReadInteger<uint8_t>();
    m_sampleSize = otherSampleSize;  // not asserting and just assuming mismatch should result in overwrite of value

    // these next 3 are unspecified tuning values, parsing just in case they are ever needed
    m_pb = stream.ReadInteger<uint8_t>();
    m_mb = stream.ReadInteger<uint8_t>();
    m_kb = stream.ReadInteger<uint8_t>();

    // channel count is repeated here
    auto otherChannelCount = stream.ReadInteger<uint8_t>();
    m_numChannels = otherChannelCount;  // assigning anyways as we assume that the inner box is the most truthful

    // maxRun, presently unused but required to be 0x00FF, parse and ignore value
    m_maxRun = stream.ReadInteger<uint16_t>();

    m_maxFrameSize = stream.ReadInteger<uint32_t>();
    m_avgBitRate = stream.ReadInteger<uint32_t>();

    // sample rate is now repeated, ignoring value
    auto otherSampleRate = stream.ReadInteger<uint32_t>();
    m_sampleRate = otherSampleRate;  // same as earlier, assuming inner box is most truthful value

    // I do not have any samples of the channel layout info section that may
    // occupy the remaining bits, simply assuming that it must run to the end
    // of the tag.
    auto bytesRead = stream.GetPosition() - initialPosition;
    if (bytesRead != m_bodySize) {
      // then assuming that there is more data, otherwise we would be at the end
      // of the tag.
      uint32_t channelLayoutSize = stream.ReadInteger<uint32_t>();
      std::string channelTag = stream.ReadString(4);

      // verify tag
      ARC_ASSERT_MSG(channelTag.compare("chan"), "Warning: Encountered unknown tag while parsing ALAC!");
      if (channelLayoutSize > 0 && channelTag.compare("chan") == 0) {
        // then carry on
        // first 4 bytes are version, just ignore
        stream.Advance(4);
        uint32_t channelLayout = stream.ReadInteger<uint32_t>();

        // convert int to enum
        auto enumItr = NumToAlacChannelLayout.find(channelLayout);

        if (enumItr != NumToAlacChannelLayout.end()) {
          m_channelLayout = enumItr->second;
        } else {
          m_channelLayout = AlacChannelLayout::UNKNOWN;
        }

        // remainder is reserved
        stream.Advance(8);
      } else {
        // else ignore, this isn't a very important piece of data
      }
    }
  } else {
    throw InvalidValueException("Encountered unexpected tag while parsing ALAC box!");
  }
}

void Alac::PrintBox(std::ostream& outStream, int depth) {
  std::string tabs = GetTabs(depth);

  outStream << tabs << "ALAC Box:" << std::endl;
  outStream << tabs << "\tNumber of Channels: " << m_numChannels << std::endl;
  outStream << tabs << "\tSample Size: " << m_sampleSize << std::endl;
  outStream << tabs << "\tSample Rate: " << m_sampleRate << std::endl;
  outStream << tabs << "\tSamples per Frame: " << m_samplePerFrame << std::endl;
  outStream << tabs << "\tCompatible Version: " << static_cast<int32_t>(m_compatibleVersion)
            << std::endl;  // cast is needed to assume correct display
  outStream << tabs << "\tMax Run: " << m_maxRun << std::endl;
  outStream << tabs << "\tMax Frame Size: " << m_maxFrameSize << std::endl;
  outStream << tabs << "\tAverage Bit Rate: " << m_avgBitRate << std::endl;
  outStream << tabs << "\tVersion: " << m_version << std::endl;
  outStream << tabs << "\tIndex: " << m_index << std::endl;
  outStream << tabs << "\tChannelLayout: " << m_channelLayout << std::endl;
}

#pragma region Public Getters

uint16_t Alac::GetNumChannels() { return m_numChannels; }

uint16_t Alac::GetSampleSize() { return m_sampleSize; }

uint32_t Alac::GetSampleRate() { return m_sampleRate; }

uint32_t Alac::GetSamplePerFrame() { return m_samplePerFrame; }

uint8_t Alac::GetCompatibleVersion() { return m_compatibleVersion; }

uint32_t Alac::GetMaxFrameSize() { return m_maxFrameSize; }

uint32_t Alac::GetAvgBitRate() { return m_avgBitRate; }

uint32_t Alac::GetVersion() { return m_version; }

uint16_t Alac::GetIndex() { return m_index; }

AlacChannelLayout Alac::GetChannelLayout() { return m_channelLayout; }

#pragma endregion

std::ostream& operator<<(std::ostream& outStream, AlacChannelLayout layout) {
  outStream << AlacChannelLayoutToString.at(layout);
  return outStream;
}

} /*MPEG4*/
} /*Arcusical*/