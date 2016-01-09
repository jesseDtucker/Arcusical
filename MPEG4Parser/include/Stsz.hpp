#pragma once

#include "Box.hpp"
/*
*	Author: Jesse Tucker
*	Date: December 30th 2012
*	Name: Stsz.hpp
*
*	Description: The is the Sample Size Box. There are 2 variations of this box.
*	This box is defined on page 39 of the ISO 14496-12 standard. This contains
*	either a value or a table of values that define the sample sizes.
*/


namespace Arcusical {
namespace MPEG4 {

class Stsz : public Box {
 public:
  Stsz();
  virtual ~Stsz() override;

  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

  uint32_t GetVersion();
  uint32_t GetSampleSize();
  uint32_t GetSampleCount();
  std::vector<uint32_t> GetEntries();

 protected:
  virtual void ReadContents(Util::Stream& stream) override;

 private:
  uint32_t m_version;
  uint32_t m_sampleSize;
  uint32_t m_sampleCount;
  std::vector<uint32_t> m_entries;
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/
