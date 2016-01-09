#pragma once

#include "Box.hpp"

/*
*	Author: Jesse Tucker
*	Date: December 29th 2012
*	Name: Mdia.hpp
*
*	Description: Media header box. Declares information specific to the media
*	in a single track. Defined on page 21 of the ISO 14496-12 standard.
*/

namespace Arcusical {
namespace MPEG4 {

class Mdhd : public Box {
 public:
  Mdhd();
  virtual ~Mdhd() override;

  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

  uint64_t GetCreationTime();
  uint64_t GetModificationTime();
  uint32_t GetTimeScale();
  uint64_t GetDuration();
  std::vector<uint8_t> GetLanguage();

 protected:
  virtual void ReadContents(Util::Stream& stream) override;

 private:
  uint32_t m_version;
  uint64_t m_creationTime;
  uint64_t m_modificationTime;
  uint32_t m_timeScale;
  uint64_t m_duration;
  std::vector<uint8_t> m_language;  // 3 values each 5 bits in length, defines an ISO-639-2/T language code
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/
