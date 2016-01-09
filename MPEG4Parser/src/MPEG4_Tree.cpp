#include "pch.h"

#include "MPEG4_Tree.hpp"

/*
*	Author: Jesse Tucker
*	Date: December 22nd 2012
*	Name: MPEG4_Tree.cpp
*
*	Description: Please see header for details
*/

using namespace std;

namespace Arcusical {
namespace MPEG4 {

MPEG4_Tree::MPEG4_Tree() {}

// public
void MPEG4_Tree::AddBox(std::shared_ptr<Box> box) { m_boxes.push_back(box); }

void MPEG4_Tree::PrintTree(ostream& outStream) {
  for (std::shared_ptr<Box>& box : m_boxes) {
    box->PrintBox(outStream);
  }
}

std::vector<std::shared_ptr<Box>>& MPEG4_Tree::GetBoxes() { return m_boxes; }

} /*MPEG4*/
} /*Arcusical*/