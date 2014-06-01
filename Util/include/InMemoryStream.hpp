#pragma once
#ifndef IN_MEMORY_STREAM_HPP
#define IN_MEMORY_STREAM_HPP

#include "Stream.hpp"

namespace Util
{
	class InMemoryStream final : public Stream
	{
	public:
		InMemoryStream(unsigned char* data, size_t length, bool createLocalCopy = false);
		virtual ~InMemoryStream();

		virtual long long GetLength();
		virtual long long GetBytesRemainaingLength();
		virtual unsigned int GetAvailableLength();
		virtual long long GetPosition();
		virtual void Advance(unsigned int count);
		virtual void Rewind(unsigned int count);
		virtual void EnsureBytesAvailable(unsigned int count = 0);

	protected:
		virtual std::vector<unsigned char> GetBytes(unsigned int count);

	private:
		unsigned char* m_data;
		size_t m_length;
		size_t m_bytesRead;
		bool m_ownsData;
	};
}

#endif