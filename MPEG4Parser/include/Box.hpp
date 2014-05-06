/*
*	Author: Jesse Tucker
*	Date: December 16th 2012
*	Name: Box.hpp
*
*	Description: Abstract class that defines the properties of a box. In the MPEG4 format
*	all information is contained within boxes, sometimes called atoms. All boxes have a size,
*	a type, and zero or more children. For more details please see the ISO 14496-12 standard.
*	The box is defined on page 4.
*/

#ifndef BOX_HPP
#define BOX_HPP

#include <vector>
#include <memory>

#include "BoxTypes.hpp"

namespace Util
{
	class Stream;
}

namespace Arcusical { namespace MPEG4 {

	class Box
	{
		public:
			/*
			*	Create a box and initialize its default values
			*/
			Box();

			/*
			*	Destroy the box and cleanup memory
			*/
			virtual ~Box();

			/*
			*	Returns the size of this box
			*/
			uint64_t GetSize();

			/*
			*	Returns references to the children of this box
			*/
			std::vector<std::shared_ptr<Box>>& GetChildren();

			/*
			*	Will parse the contents of the box from the buffer
			*/
			virtual void ParseBox(uint64_t size, Util::Stream& stream, uint32_t headerSize);

			/*
			*	Print out the contents of this box in a human readable form to the provided stream.
			*	depth = depth of indentation, largely used internally however can be adjusted extenerally
			*	to increase number of tabs at beggining of lines.
			*/
			virtual void PrintBox(std::ostream& outStream, int depth = 0) = 0;

			virtual std::shared_ptr<std::string> GetBoxName();

		protected:

			int64_t m_bodySize;
			int64_t m_fullSize;
			std::vector<std::shared_ptr<Box>> m_children;

			virtual void ReadContents(Util::Stream& stream) = 0;

			/*
			*	Simple helper function used for indentation
			*/
			static const std::string& GetTabs(int count);

		private:
			std::shared_ptr<std::string> m_boxName;
	};

} /*namespace: MPEG4*/}/*namespace: Arcusical*/ 


#endif // !BOX_H
