/*
*	Author: Jesse Tucker
*	Date: December 30th 2012
*	Name: Stsz.hpp
*
*	Description: The is the Sample Size Box. There are 2 variations of this box.
*	This box is defined on page 41 of the ISO 14496-12 standard. The first variation
*	(which is the version implemented in this header file) is STCO and records the
*	values in 32 bits, the other version (CO64) uses 64 bit values.
*
*	This defines the offset table that gives the index of each chunk
*	in the containing file.
*/

#ifndef STCO_HPP
#define STCO_HPP

#include "Box.hpp"

namespace Arcusical {
namespace MPEG4 {

class Stco : public Box {
 public:
  Stco();
  virtual ~Stco() override;

  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

  uint32_t GetVersion();
  std::vector<uint32_t> GetEntries();

 protected:
  virtual void ReadContents(Util::Stream& stream) override;

 private:
  uint32_t m_version;
  std::vector<uint32_t> m_entries;
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/

#endif
