#include <algorithm>

#include "Arc_Assert.hpp"
#include "boost\type_traits\make_unsigned.hpp"
#include "CheckedCasts.hpp"
#include "FileReader.hpp"
#include "IFile.hpp"

namespace FileSystem
{
	FileReader::FileReader(std::shared_ptr<IFile> file, unsigned int bufferSize)
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

	long long FileReader::GetLength()
	{
		return m_totalBytes;
	}

	unsigned int FileReader::GetAvailableLength()
	{
		auto availableBytes = m_endPosition - m_currentPosition;

		ARC_ASSERT_MSG(availableBytes >= 0, "available bytes must be greater than zero!");
		typedef boost::make_unsigned<decltype(availableBytes)>::type unsignedType;
		return Util::SafeIntCast<unsigned int>(static_cast<unsignedType>(availableBytes));
	}

	long long FileReader::GetBytesRemainaingLength()
	{
		return GetLength() - m_bytesRead;
	}

	long long FileReader::GetPosition()
	{
		return m_bytesRead;
	}

	std::vector<unsigned char> FileReader::GetBytes(unsigned int count)
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

				bytes.insert(bytes.end(), bytesFromFile.begin(), bytesFromFile.begin() + remainingBytes);
				std::copy(bytesFromFile.begin() + remainingBytes, bytesFromFile.end(), m_buffer.begin());

				m_currentPosition = m_buffer.begin();
				m_endPosition = m_buffer.begin() + bytesToStoreInBuffer;

				m_bytesReadFromFile += bytesToReadFromFile;
			}
		}

		m_bytesRead += bytes.size();

		return bytes;
	}

	void FileReader::GetBytesFromBuffer(std::vector<unsigned char>& bytes, unsigned int count)
	{
		ARC_ASSERT_MSG(count <= GetAvailableLength(), "Attempted to read too many bytes from ring buffer!");

		auto endPoint = m_currentPosition + count;
		bytes.insert(bytes.begin(), m_currentPosition, endPoint);
		m_currentPosition += count;
	}

	void FileReader::Advance(unsigned int count)
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

	void FileReader::Rewind(unsigned int count)
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

	void FileReader::EnsureBytesAvailable(unsigned int count /* = 0 */)
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