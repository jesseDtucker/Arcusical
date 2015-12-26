/*
*	Author: Jesse Tucker
*	Date: December 16th 2012
*	Name: MPEG4_Tree.h
*
*	Description: Defines the tree of data contained in an MPEG4 container. Please see the
*	ISO 14496-12 standard for details on the boxes and the file format.
*/

#ifndef MPEG4_TREE_H
#define MPEG4_TREE_H

#include <vector>
#include <memory>

#include "Box.hpp"

namespace Arcusical {
namespace MPEG4 {

class MPEG4_Tree final {
 public:
  /*
  * Create an empty tree of boxes
  */
  MPEG4_Tree();

  /*
  * Add the provided box to the tree
  */
  void AddBox(std::shared_ptr<Box> box);

  /*
  * Get the boxes contained within this tree
  */
  std::vector<std::shared_ptr<Box>>& GetBoxes();

  /*
  *	Print this tree in a readable std::string representation to the provided stream
  */
  void PrintTree(std::ostream& outStream);

 private:
  std::vector<std::shared_ptr<Box>> m_boxes;
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/

#endif /* !MPEG4_TREE_H*/