/*
*	Author: Jesse Tucker
*	Date: January 1st 2012
*	Name: iDiskNumber.hpp
*
*	Description: Itunes specific box. Contiains information on the disk number. Includes both the number
*	for this disk and the disk number for the album. ie: track x of y.
*/

#ifndef IDISK_NUMBER_HPP
#define IDISK_NUMBER_HPP

#include "Box.hpp"

namespace Arcusical { namespace MPEG4 {

	class iDiskNumber : public Box
	{
		public:

			iDiskNumber() {}
			virtual ~iDiskNumber() {}

			virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

			uint16_t GetDiskNumber();
			uint16_t GetMaxDisk();

		protected:
			virtual void ReadContents(Util::Stream& stream) override;

		private:

			uint16_t m_diskNumber;
			uint16_t m_maxDisk;
	};

}/*Arcusical*/}/*MPEG4*/

#endif