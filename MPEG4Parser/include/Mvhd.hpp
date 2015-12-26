/*
*	Author: Jesse Tucker
*	Date: December 27th 2012
*	Name: Mvhd.hpp
*
*	Description: Movie Header Box definition as defined on page 16 of
*	ISO 14496-12. Contains media independant meta data.
*/

#ifndef MVHD_H
#define MVHD_H

#include "Box.hpp"

namespace Arcusical {
namespace MPEG4 {

class Mvhd : public Box {
 public:
  Mvhd();
  virtual ~Mvhd() override;

  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

  uint32_t GetVersion();
  uint64_t GetCreationTime();
  uint64_t GetModificationTime();
  uint32_t GetTimeScale();
  uint64_t GetDuration();
  float GetRate();
  float GetVolume();
  std::vector<uint32_t> GetMatrix();
  uint32_t GetNextTrackId();

 protected:
  virtual void ReadContents(Util::Stream& stream) override;

 private:
  uint32_t m_version;
  uint64_t m_creationTime;
  uint64_t m_modificationTime;
  uint32_t m_timeScale;
  uint64_t m_duration;
  float m_rate;  // 16.16 fixed point number
  float m_volume;  // 8.8 fixed point number
  std::vector<uint32_t> m_matrix;
  uint32_t m_nextTrackId;
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/

#endif