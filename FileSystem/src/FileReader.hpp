#pragma  once
#ifndef FILE_READER_HPP

#include <condition_variable>
#include <memory>
#include <mutex>
#include <vector>

#include "IFileReader.hpp"
#include "Stream.hpp"
#include "Util.hpp"

namespace FileSystem
{
	class IFile;

	class FileReader : public IFileReader
	{
	public:
		FileReader(std::shared_ptr<IFile> file, unsigned int bufferSize = 65536);
		FileReader(const FileReader&) = delete;
		FileReader& operator=(const FileReader&) = delete;

		virtual void Rewind(unsigned int count) override;
		virtual void Advance(unsigned int count) override;
		virtual void EnsureBytesAvailable(unsigned int count = 0) override;

		virtual long long GetLength() override;
		virtual unsigned int GetAvailableLength() override;
		virtual long long GetBytesRemainaingLength() override;
		virtual long long GetPosition() override;

	private:
		virtual std::vector<unsigned char> GetBytes(unsigned int count) override;
		void GetBytesFromBuffer(std::vector<unsigned char>& buffer, unsigned int count);

		std::vector<unsigned char> m_buffer;
		std::vector<unsigned char>::iterator m_currentPosition;
		std::vector<unsigned char>::iterator m_endPosition;

		unsigned int m_bufferSize;
		long long m_bytesRead;
		long long m_bytesReadFromFile;
		long long m_totalBytes;

		std::shared_ptr<IFile> m_file;
	};

}

#endif