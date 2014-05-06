#pragma  once
#ifndef FILE_READER_HPP

#include <condition_variable>
#include <memory>
#include <mutex>
#include <vector>

#include "Stream.hpp"
#include "Util.hpp"

namespace FileSystem
{
	class IFile;

	class EXPORT IFileReader : public Util::Stream
	{
	public:
		IFileReader() = default;
		IFileReader(const IFileReader&) = delete;
		IFileReader& operator=(const IFileReader&) = delete;
		
		virtual long long GetLength() = 0;
		virtual unsigned int GetAvailableLength() = 0;
		virtual long long GetBytesRemainaingLength() = 0;
		virtual long long GetPosition() = 0;
		virtual void EnsureBytesAvailable(unsigned int count = 0) = 0;

	private:
		virtual std::vector<unsigned char> GetBytes(unsigned int count) = 0;
		virtual void Advance(unsigned int count) = 0;
	};

}

#endif