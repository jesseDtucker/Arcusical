// FileSystem.cpp : Defines the exported functions for the DLL application.
// 

#include <algorithm>
#include <future>
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
	IFolder* Storage::s_musicFolder = nullptr;
	IFolder* Storage::s_applicationFolder = nullptr;

	static std::mutex s_loadingLock;

#ifdef __cplusplus_winrt
	
	IFolder& Storage::MusicFolder()
	{
		if (s_musicFolder == nullptr)
		{
			std::unique_lock<std::mutex> lockguard(s_loadingLock);
			if (s_musicFolder == nullptr)
			{
				s_musicFolder = new Folder(Windows::Storage::KnownFolders::MusicLibrary);
			}
		}

		auto& folder = *s_musicFolder;

#ifdef _DEBUG
		static std::shared_ptr<IFolder> debugFolder = nullptr;
		auto folders = folder.GetSubfolders();
		for (auto& f : folders)
		{
			if (f->GetName() == L"Test")
			{
				debugFolder = f;
				break;
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
		if (s_applicationFolder == nullptr)
		{
			std::unique_lock<std::mutex> lockguard(s_loadingLock);
			if (s_applicationFolder == nullptr)
			{
				s_applicationFolder = new Folder(Windows::Storage::ApplicationData::Current->LocalFolder);
			}
		}

		return *s_applicationFolder;
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