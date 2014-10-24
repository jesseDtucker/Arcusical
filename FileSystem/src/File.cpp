#include "File.hpp"

#include <limits>
#include <ppltasks.h>

#include "Arc_Assert.hpp"
#include "NativeBufferWrapper.hpp"
#include "StorageExceptions.hpp"

using namespace std;
using namespace concurrency;

// this macro is interfering with the limits header numeric limits template
#ifdef max
#undef max
#endif

namespace FileSystem
{
	File::File(Windows::Storage::StorageFile^ file)
		: m_file(file)
	{
		
	}

	wstring File::GetExtension()
	{
		wstring name(m_file->Name->Data());
		auto found = name.find_last_of(L".");

		return name.substr(found + 1);
	}

	std::wstring File::GetName()
	{
		std::wstring name(m_file->Name->Data());
		auto found = name.find_last_of(L".");

		return name.substr(0, found);
	}

	std::wstring File::GetFullName()
	{
		return std::wstring(m_file->Name->Data());
	}

	std::wstring File::GetFullPath()
	{
		return std::wstring(m_file->Path->Data());
	}

	void File::WriteToFile(std::vector<unsigned char>& data)
	{
		auto buffer = NativeBufferWrapper::WrapBuffer(&data);
		auto task = create_task(Windows::Storage::FileIO::WriteBufferAsync(m_file, buffer));
		task.wait();
	}

	void File::ReadFromFile(std::vector<unsigned char>& buffer, unsigned int length /* = 0*/, unsigned long long startPosition /* = 0*/)
	{
		auto nativeBuffer = NativeBufferWrapper::WrapBuffer(&buffer);
		auto randomAccessStream = create_task(m_file->OpenReadAsync()).get();

		if (length == 0)
		{
			// this check is needed as the size returned by randomAccessStream is always a 64 bit int
			// however the buffer used to read in the data may only take a 32 bit int to its resize
			// if we are compiling for a 32 bit processor.
			auto isToBig = (randomAccessStream->Size) >= (std::numeric_limits<decltype(length)>::max());
			if (isToBig)
			{
				throw ReadTooLongException("Attempted to open a file whos length cannot be stored in a size_t");
			}

			// cast is safe at this point as we have already ensure there can be no data loss due to size
			length = static_cast<decltype(length)>(randomAccessStream->Size);
		}

		// do not try to read in zero bytes, the file may exist but it may be empty
		if (length > 0)
		{
			ARC_ASSERT_MSG(startPosition < randomAccessStream->Size, "Attempting to start reading from a point beyond the end of the file!");
			ARC_ASSERT_MSG(startPosition + length <= randomAccessStream->Size, "Attempted to read beyond the end of a file!");

			auto inputStream = randomAccessStream->GetInputStreamAt(startPosition);
			buffer.resize(length);
			auto task = create_task(inputStream->ReadAsync(nativeBuffer, length, Windows::Storage::Streams::InputStreamOptions::None));
			task.wait();
		}
	}

	unsigned long long File::GetFileSize()
	{
		auto propertiesTask = create_task(m_file->GetBasicPropertiesAsync());
		return propertiesTask.get()->Size;
	}

}