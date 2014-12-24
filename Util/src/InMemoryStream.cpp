#include "windows.h"

#include "Arc_Assert.hpp"
#include "InMemoryStream.hpp"
#include "CheckedCasts.hpp"

namespace Util
{
	InMemoryStream::InMemoryStream(unsigned char* data, size_t length, bool createLocalCopy /* = false */)
		: m_data(data)
		, m_length(length)
		, m_ownsData(createLocalCopy)
		, m_bytesRead(0)
	{
		if (createLocalCopy)
		{
			m_data = new unsigned char[length];
			memcpy(m_data, data, m_length);
		}
	}

	InMemoryStream::~InMemoryStream()
	{
		if (m_ownsData)
		{
			delete m_data;
		}
	}

	long long InMemoryStream::GetLength()
	{
		return m_length;
	}

	long long InMemoryStream::GetBytesRemainaingLength()
	{
		ARC_ASSERT(m_length - m_bytesRead > 0);
		return m_length - m_bytesRead;
	}

	unsigned int InMemoryStream::GetAvailableLength()
	{
		// as the entire stream is in memory the available length is the full length
		return Util::SafeIntCast<unsigned int, decltype(GetBytesRemainaingLength())>(GetBytesRemainaingLength());
	}

	long long InMemoryStream::GetPosition()
	{
		return m_bytesRead;
	}

	void InMemoryStream::Advance(unsigned int count)
	{
		ARC_ASSERT(count <= GetAvailableLength());
		m_bytesRead += count;
	}

	void InMemoryStream::Rewind(unsigned int count)
	{
		ARC_ASSERT(m_bytesRead >= count);
		m_bytesRead -= count;
	}

	void InMemoryStream::EnsureBytesAvailable(unsigned int count /* = 0 */)
	{
		ARC_ASSERT(count <= GetAvailableLength());
		UNREFERENCED_PARAMETER(count); // needed for release builds
		// nothing to do here other than assert, There is no data source to pull extra bytes from
	}

	std::vector<unsigned char> InMemoryStream::GetBytes(unsigned int count)
	{
		ARC_ASSERT(count <= GetAvailableLength());
		std::vector<unsigned char> result;
		result.resize(count);

		auto currentPos = m_data + m_bytesRead;
		std::copy(currentPos, currentPos + count, result.begin());
		m_bytesRead += count;

		return result;
	}
}