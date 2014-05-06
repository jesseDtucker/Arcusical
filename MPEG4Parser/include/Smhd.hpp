/*
*	Author: Jesse Tucker
*	Date: December 29th 2012
*	Name: Smhd.hpp
*
*	Description: Definition found in ISO 14496-12 standard, page 23.
*	Sound Media Header box, contains information pertaining to the general
*	presentation of the sound data.
*/

#ifndef SMHD_HPP
#define SMHD_HPP

#include "Box.hpp"

namespace Arcusical { namespace MPEG4 {

	class Smhd : public Box
	{
		public:

			Smhd();
			virtual ~Smhd() override;

			virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

			uint32_t GetVersion();
			float GetBalance();

		protected:
			virtual void ReadContents(Util::Stream& stream) override;

		private:

			uint32_t m_version;
			float m_balance;	//8.8 fixed point number, -1.0 is full left, 0.0 is center, 1.0 is full right, used in mono tracks
	};

} /*namespace: MPEG4*/}/*namespace: Arcusical*/ 

#endif