/*
*	Author: Jesse Tucker
*	Date: January 1st 2013
*	Description: Itunes specific box. No publicly available documentation. No references
*	found to it online. Appears to be a std::string that describes the files encoding. Encountered
*	values are: 2:256 (file had 2 streams, 256 Kb/s), 7:720p (movie, presumed 7.1 stereo and 720P video)
*	I believe this box is called a flavour box...
*/

#ifndef IFLVR_HPP
#define IFLVR_HPP

#include "UnknownBox.hpp"

namespace Arcusical {
namespace MPEG4 {

class iFlvr : public UnknownBox {
 public:
  iFlvr() {}
  virtual ~iFlvr() {}
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/

#endif