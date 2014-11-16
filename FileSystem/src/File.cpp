#include "File.hpp"

#include <limits>
#include <ppltasks.h>

#include "Arc_Assert.hpp"
#include "Folder.hpp"
#include "NativeBufferWrapper.hpp"
#include "CheckedCasts.hpp"
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
		ARC_ASSERT(file != nullptr);
	}

	wstring File::GetExtension() const
	{
		wstring name(m_file->Name->Data());
		auto found = name.find_last_of(L".");

		return name.substr(found + 1);
	}

	std::wstring File::GetName() const
	{
		std::wstring name(m_file->Name->Data());
		auto found = name.find_last_of(L".");

		return name.substr(0, found);
	}

	std::wstring File::GetFullName() const
	{
		return std::wstring(m_file->Name->Data());
	}

	std::wstring File::GetFullPath() const
	{
		return std::wstring(m_file->Path->Data());
	}

	void File::WriteToFile(std::vector<unsigned char>& data)
	{
		try
		{
			auto buffer = NativeBufferWrapper::WrapBuffer(&data);
			auto task = create_task(Windows::Storage::FileIO::WriteBufferAsync(m_file, buffer));
			task.wait();
		}
		catch (Platform::COMException^ ex)
		{
			ARC_FAIL("Unhandled Exception!");
		}
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

	std::shared_ptr<IFolder> File::GetParent()
	{
		auto parent = create_task(m_file->GetParentAsync()).get();
		return std::make_shared<Folder>(parent);
	}

	std::vector<unsigned char> File::GetThumbnail(bool allowIcon)
	{
		std::vector<unsigned char> buffer;

		try
		{
			auto thumbnailStream = create_task(m_file->GetThumbnailAsync(Windows::Storage::FileProperties::ThumbnailMode::SingleItem)).get();

			if (allowIcon || thumbnailStream->Type == Windows::Storage::FileProperties::ThumbnailType::Image)
			{
				auto nativeBuffer = NativeBufferWrapper::WrapBuffer(&buffer);
				auto length = Util::SafeIntCast<size_t, decltype(thumbnailStream->Size)>(thumbnailStream->Size);
				buffer.resize(length);

				auto inStream = thumbnailStream->GetInputStreamAt(0);
				create_task(inStream->ReadAsync(nativeBuffer, Util::SafeIntCast<unsigned int, decltype(length)>(length), Windows::Storage::Streams::InputStreamOptions::None)).wait();
			}
		}
		catch (Platform::COMException^ ex)
		{
			ARC_FAIL("Unhandled exception!");
		}

		return buffer;
	}

#ifdef __cplusplus_winrt
	Windows::Storage::StorageFile^ File::GetRawHandle() const
	{
		return m_file;
	}
#endif

}