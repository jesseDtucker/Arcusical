#pragma once

#include <memory>
#include <vector>

#include "Box.hpp"
/*
*	Author: Jesse Tucker
*	Date: December 31st 2012
*	Name: DataBox.hpp
*
*	Description: Defines a general purpose box that contains nothing but raw data.
*/


namespace Arcusical {
namespace MPEG4 {

class DataBox : public Box {
 public:
  DataBox();
  virtual ~DataBox() override;

  virtual std::shared_ptr<std::vector<unsigned char>> GetData();
  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

 protected:
  virtual void ReadContents(Util::Stream& stream) override;

  std::shared_ptr<std::vector<unsigned char>> m_data;
};

} /*MPEG4*/
} /*Arcusical*/
