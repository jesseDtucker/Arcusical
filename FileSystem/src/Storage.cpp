// FileSystem.cpp : Defines the exported functions for the DLL application.
// 

#include <algorithm>
#include <ppltasks.h>
#include <string>

#include "Arc_Assert.hpp"
#include "File.hpp"
#include "FileReader.hpp"
#include "Folder.hpp"
#include "Storage.hpp"

using namespace concurrency;

namespace FileSystem
{
#ifdef __cplusplus_winrt

	IFolder& Storage::MusicFolder()
	{
		static Folder folder(Windows::Storage::KnownFolders::MusicLibrary);

#ifdef _DEBUG
		static std::shared_ptr<IFolder> debugFolder = nullptr;
		auto folders = folder.GetSubfolders();
		for (auto& f : folders)
		{
			if (f->GetName() == L"Test")
			{
				debugFolder = f;
			}
		}

		if (debugFolder != nullptr)
		{
			return *debugFolder;
		}
#endif

		return folder;
	}

	IFolder& Storage::ApplicationFolder()
	{
		static Folder folder(Windows::Storage::ApplicationData::Current->LocalFolder);

		return folder;
	}

	std::shared_ptr<IFileReader> Storage::GetReader(std::shared_ptr<IFile> file)
	{
		return std::make_shared<FileReader>(file);
	}

	std::shared_ptr<IFile> Storage::LoadFileFromPath(std::wstring filePath)
	{
		std::replace(filePath.begin(), filePath.end(), L'/', L'\\');

		std::shared_ptr<IFile> file = nullptr;
		
		try
		{
			auto winRTfile = create_task(Windows::Storage::StorageFile::GetFileFromPathAsync(ref new Platform::String(filePath.c_str()))).get();
			file = std::make_shared<File>(winRTfile);
		}
		catch (Platform::Exception^ ex)
		{
			ARC_FAIL(std::wstring(ex->Message->Data()).c_str());
		}

		return file;
	}

#else
#error UNSUPPORTED PLATFORM!
#endif
}