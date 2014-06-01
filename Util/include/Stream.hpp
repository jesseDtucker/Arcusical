#pragma once
#ifndef STREAM_HPP
#define STREAM_HPP

#include <stdint.h>
#include <type_traits>
#include <vector>

#include "RawParser.hpp"
#include "PropertyHelper.hpp"
#include "Util.hpp"

namespace Util
{
	class EXPORT Stream
	{
	public:

		Stream() = default;
		Stream(const Stream&) = delete;
		Stream& operator=(const Stream&) = delete;
		virtual ~Stream() = default;

		/************************************************************************/
		/* Returns the number of bytes in this stream
		/************************************************************************/
		virtual long long GetLength() = 0;

		/************************************************************************/
		/* Returns the number of bytes that are not yet read from this stream
		/************************************************************************/
		virtual long long GetBytesRemainaingLength() = 0;

		/************************************************************************/
		/* Returns the number of bytes available without having to read more
		/************************************************************************/
		virtual unsigned int GetAvailableLength() = 0;

		/************************************************************************/
		/* Returns a value that indicates the current position of the stream
		/************************************************************************/
		virtual long long GetPosition() = 0;

		/************************************************************************/
		/* Advances the stream by the specified number of bytes without 
		/* reading them
		/************************************************************************/
		virtual void Advance(unsigned int count) = 0;

		/************************************************************************/
		/* Rewinds the stream by the provided number of bytes
		/************************************************************************/
		virtual void Rewind(unsigned int count) = 0;

		/************************************************************************/
		/* Ensure that at least this many bytes can be read without waiting
		 * Note: count must be less than the buffer size! A value of zero indicates
		 * The buffer should fill itself to capacity!
		/************************************************************************/
		virtual void EnsureBytesAvailable(unsigned int count = 0) = 0;

		/************************************************************************/
		/* Reads in a number of type T_Number, will handle machine endianess
		/************************************************************************/
		template <typename T_Number>
		T_Number ReadInteger();

		/************************************************************************/
		/* Reads in a number of type T_Number, will handle machine endianess.
		/* Will only read the number of bytes specified. This method is intended
		/* for handling numbers of non-standard sizes (eg. 24 bit integers)
		/************************************************************************/
		template <typename T_Number, uint32_t countBytes>
		T_Number ReadInteger();

		/************************************************************************/
		/* Reads in a fixed point number and stores it as a floating point number.
		 * T_SizeType is a data value that is capable of representing the stored
		 * number as an integer (eg. a 16.16 number could be represented by any
		 * 32 bit data type such as a uint32_t)
		/************************************************************************/
		template <typename T_SizeType>
		float ReadFixedPoint(uint32_t bitsAfterDecimal);

		/************************************************************************/
		/* Will read in the characters up until count is reached then advance the iterator just past that point
		/************************************************************************/
		std::string ReadString(unsigned int count);

		/************************************************************************/
		/* Appends the bytes from the provided iterator to the destination
		/************************************************************************/
		template<typename T>
		void ReadRawBytes(unsigned int count, T& destination);
		std::vector<unsigned char> ReadRawBytes(unsigned int count);

	protected:
		virtual std::vector<unsigned char> GetBytes(unsigned int count) = 0;
	};

#pragma region Template Implemenation

	template <typename T_Number>
	T_Number Stream::ReadInteger()
	{
		auto rawData = GetBytes(sizeof(T_Number));
		return Util::ReadInteger<T_Number>(rawData.data());
	}

// the warning can be safely disabled as there are static asserts to ensure that there cannot
// be a bad copy or buffer overrun
#pragma warning(push)
#pragma warning(disable : 4996)

	template <typename T_Number, uint32_t countBytes>
	T_Number Stream::ReadInteger()
	{
		auto rawData = GetBytes(countBytes);
		return Util::ReadInteger<T_Number, decltype(rawData.data()), countBytes>(rawData.data());
	}

#pragma warning(pop)

	template <typename T_SizeType>
	float Stream::ReadFixedPoint(uint32_t bitsAfterDecimal)
	{
		T_SizeType value = ReadInteger<T_SizeType>();
		return (static_cast<float>(value) / static_cast<float>(1 << bitsAfterDecimal));
	}

	/*
	*	Appends the bytes from the provided iterator to the destination
	*/
	template<typename T>
	void Stream::ReadRawBytes(unsigned int count, T& destination)
	{
		auto rawBytes = GetBytes(count);
		destination.insert(destination.begin(), rawBytes.begin(), rawBytes.end());
	}

#pragma endregion


}

#endif