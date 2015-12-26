/*
*	Author: Jesse Tucker
*	Date: January 1st 2012
*	Name: iTrackNumber.hpp
*
*	Description: Itunes specific box. Contiains information on the track number. Includes both the number
*	for this track and the track number for the album. ie: track x of y.
*/

#ifndef ITRACK_NUMBER_HPP
#define ITRACK_NUMBER_HPP

#include "Box.hpp"

namespace Arcusical {
namespace MPEG4 {

class iTrackNumber : public Box {
 public:
  iTrackNumber() {}
  virtual ~iTrackNumber() {}

  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

  uint16_t GetTrackNumber();
  uint16_t GetMaxTrack();

 protected:
  virtual void ReadContents(Util::Stream& stream) override;

 private:
  uint16_t m_trackNumber;
  uint16_t m_maxTrack;
};

} /*Arcusical*/
} /*MPEG4*/

#endif