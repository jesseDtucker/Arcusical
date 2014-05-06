#ifndef FILE_HPP
#define FILE_HPP

#include <string>
#include <vector>

#include "IFile.hpp"
#include "Util.hpp"

namespace FileSystem
{
	class File final : public IFile
	{
	public:

		virtual std::wstring GetName() override;
		virtual std::wstring GetFullName() override;
		virtual std::wstring GetExtension() override;
		virtual std::wstring GetFullPath() override;

		virtual unsigned long long GetFileSize() override;

		virtual void WriteToFile(std::vector<unsigned char>& data) override;
		virtual void ReadFromFile(std::vector<unsigned char>& buffer, unsigned int length = 0, unsigned long long startPosition = 0) override;

#ifdef __cplusplus_winrt
		// This should not be instantiated by anything outside of the FileSystem project!
		// unfortunately it cannot be private due to the need to use make_shared. I'll find
		// a better workaround later.
		File(Windows::Storage::StorageFile^ file);
	private:
		Windows::Storage::StorageFile^ m_file;
#else
#error UNSUPPORTED PLATFORM
#endif
	};
}

#endif
