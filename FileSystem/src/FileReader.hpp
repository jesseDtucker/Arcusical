#pragma  once
#ifndef FILE_READER_HPP

#undef min

#include <algorithm>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <vector>

#include "Arc_Assert.hpp"
#include "boost\type_traits\make_unsigned.hpp"
#include "CheckedCasts.hpp"
#include "IFile.hpp"
#include "IFileReader.hpp"
#include "Stream.hpp"
#include "Util.hpp"

#undef min
#undef max

namespace FileSystem
{
	template<typename IFilePtr>
	class FileReader : public IFileReader
	{
	public:
		FileReader(IFilePtr file, unsigned int bufferSize = 65536);
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

		IFilePtr m_file;
	};

	//////////////////////////////////////////////////////////////////////////
	//	Implementation
	//////////////////////////////////////////////////////////////////////////

	template<typename IFilePtr>
	FileReader<IFilePtr>::FileReader(IFilePtr file, unsigned int bufferSize)
		: IFileReader()
		, m_file(file)
		, m_bufferSize(bufferSize)
		, m_bytesRead(0)
		, m_bytesReadFromFile(0)
	{
		m_buffer.resize(m_bufferSize);

		m_currentPosition = m_buffer.begin();
		m_endPosition = m_buffer.begin();
		m_totalBytes = file->GetFileSize();
	}

	template<typename IFilePtr>
	long long FileReader<IFilePtr>::GetLength()
	{
		return m_totalBytes;
	}

	template<typename IFilePtr>
	unsigned int FileReader<IFilePtr>::GetAvailableLength()
	{
		auto availableBytes = m_endPosition - m_currentPosition;

		ARC_ASSERT_MSG(availableBytes >= 0, "available bytes must be greater than zero!");
		typedef boost::make_unsigned<decltype(availableBytes)>::type unsignedType;
		return Util::SafeIntCast<unsigned int>(static_cast<unsignedType>(availableBytes));
	}

	template<typename IFilePtr>
	long long FileReader<IFilePtr>::GetBytesRemainaingLength()
	{
		return GetLength() - m_bytesRead;
	}

	template<typename IFilePtr>
	long long FileReader<IFilePtr>::GetPosition()
	{
		return m_bytesRead;
	}

	template<typename IFilePtr>
	std::vector<unsigned char> FileReader<IFilePtr>::GetBytes(unsigned int count)
	{
		std::vector<unsigned char> bytes;
		auto availableBytes = GetAvailableLength();

		if (count <= availableBytes)
		{
			// then we have enough buffer available to support the operation
			GetBytesFromBuffer(bytes, count);
		}
		else
		{
			// not enough data in buffer, we'll need to hit disk
			// first lets fill the bytes with what we have
			GetBytesFromBuffer(bytes, availableBytes);
			auto remainingBytes = count - availableBytes;

			std::vector<unsigned char> bytesFromFile;
			auto bytesToReadFromFile = m_bufferSize + remainingBytes;
			auto bytesRemainingInFile = m_totalBytes - m_bytesReadFromFile;
			ARC_ASSERT_MSG(remainingBytes <= bytesRemainingInFile, "Warning: attempting to read more bytes than are in this file! Returned buffer will be smaller than requested!");
			if (bytesToReadFromFile > bytesRemainingInFile)
			{
				bytesToReadFromFile = Util::SafeIntCast<decltype(bytesToReadFromFile)>(bytesRemainingInFile);
			}

			if (bytesToReadFromFile > 0)
			{
				auto bytesToStoreInBuffer = bytesToReadFromFile - remainingBytes;

				m_file->ReadFromFile(bytesFromFile, bytesToReadFromFile, m_bytesRead + availableBytes);

				ARC_ASSERT(bytesFromFile.size() >= remainingBytes);
				auto bytesToInsert = std::min(remainingBytes, Util::SafeIntCast<decltype(remainingBytes)>(bytesFromFile.size()));
				bytes.insert(bytes.end(), bytesFromFile.begin(), bytesFromFile.begin() + bytesToInsert);
				std::copy(bytesFromFile.begin() + bytesToInsert, bytesFromFile.end(), m_buffer.begin());

				m_currentPosition = m_buffer.begin();
				ARC_ASSERT(m_buffer.size() >= bytesToStoreInBuffer);
				m_endPosition = m_buffer.begin() + std::min(bytesToStoreInBuffer, Util::SafeIntCast<decltype(bytesToStoreInBuffer)>(m_buffer.size()));

				m_bytesReadFromFile += bytesToReadFromFile;
			}
		}

		m_bytesRead += bytes.size();

		return bytes;
	}

	template<typename IFilePtr>
	void FileReader<IFilePtr>::GetBytesFromBuffer(std::vector<unsigned char>& bytes, unsigned int count)
	{
		ARC_ASSERT_MSG(count <= GetAvailableLength(), "Attempted to read too many bytes from ring buffer!");

		auto endPoint = m_currentPosition + count;
		bytes.insert(bytes.begin(), m_currentPosition, endPoint);
		m_currentPosition += count;
	}

	template<typename IFilePtr>
	void FileReader<IFilePtr>::Advance(unsigned int count)
	{
		ARC_ASSERT_MSG(count <= GetBytesRemainaingLength(), "Attempted to advance beyond the edge of the stream");
		auto availableBytes = GetAvailableLength();
		if (count <= availableBytes)
		{
			m_currentPosition += count;
		}
		else
		{
			// we need to invalidate the buffer
			m_currentPosition = m_buffer.begin();
			m_endPosition = m_buffer.begin();
			auto extraBytesIgnored = count - availableBytes;

			m_bytesReadFromFile += extraBytesIgnored;
		}

		m_bytesRead += count;
	}

	template<typename IFilePtr>
	void FileReader<IFilePtr>::Rewind(unsigned int count)
	{
		ARC_ASSERT_MSG(count <= m_bytesRead, "Attempted to rewind beyound the start of the file!");
		typedef boost::make_unsigned<decltype(m_currentPosition - m_buffer.begin())>::type RemainingType;
		auto bytesReadFromBuffer = m_currentPosition - m_buffer.begin();
		ARC_ASSERT_MSG(bytesReadFromBuffer >= 0, "Negative bytes read from buffer?!?");
		if (static_cast<RemainingType>(bytesReadFromBuffer) >= count)
		{
			// then we can simply move the start point there
			m_currentPosition = m_currentPosition - count;
		}
		else
		{
			auto bytesInBuffer = m_endPosition - m_buffer.begin();

			m_bytesReadFromFile -= bytesInBuffer + count;

			// we need to invalidate the buffer
			m_currentPosition = m_buffer.begin();
			m_endPosition = m_buffer.begin();
		}

		m_bytesRead -= count;
	}

	template<typename IFilePtr>
	void FileReader<IFilePtr>::EnsureBytesAvailable(unsigned int count /* = 0 */)
	{
		count = count == 0 ? m_bufferSize : count;
		ARC_ASSERT(count <= m_bufferSize);
		count = std::min(m_bufferSize, count);
		auto availableBytes = GetAvailableLength();
		if (availableBytes < count)
		{
			// then we need to fill the buffer up some more
			auto missingBytes = count - availableBytes;
			std::vector<unsigned char> bytesReadIn;
			m_file->ReadFromFile(bytesReadIn, missingBytes, GetPosition());
			ARC_ASSERT(bytesReadIn.size() <= availableBytes && bytesReadIn.size() + availableBytes < m_bufferSize);
			std::copy(std::begin(bytesReadIn), std::end(bytesReadIn), std::end(m_buffer));
			m_bytesReadFromFile += bytesReadIn.size();
		}
	}

}

#endif