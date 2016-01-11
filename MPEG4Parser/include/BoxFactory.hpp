#pragma once

#include <memory>

#include "Box.hpp"

/*
*	Author: Jesse Tucker
*	Date: December 27th 2012
*	Name: BoxFactory.hpp
*
*	Description: Creates Boxes based upon type.
*/

namespace Arcusical {
namespace MPEG4 {

class BoxFactory final {
 public:
  static std::shared_ptr<Box> GetBox(BoxType type);
};

} /*MPEG4*/
} /*Arcusical*/
