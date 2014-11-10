#include "Folder.hpp"

#include <ppltasks.h>

#include "Arc_Assert.hpp"
#include "File.hpp"
#include "Storage.hpp"

using namespace concurrency;

#define FILE_NOT_FOUND_HRESULT 0x80070002

namespace FileSystem
{
#ifdef __cplusplus_winrt

	Folder::Folder(Windows::Storage::StorageFolder^ folder)
		: m_folder(folder)
	{

	}

	std::wstring Folder::GetName()
	{
		return std::wstring(m_folder->Name->Data());
	}

	std::vector<std::shared_ptr<IFolder>> Folder::GetSubfolders()
	{
		auto subFolders = create_task(m_folder->GetFoldersAsync()).get();

		std::vector<std::shared_ptr<IFolder>> results;
		results.reserve(subFolders->Size);

		auto itr = subFolders->First();

		while (itr->HasCurrent)
		{
			auto folder = std::make_shared<Folder>(itr->Current);
			results.push_back(folder);
			itr->MoveNext();
		}

		return results;
	}

	std::vector<std::shared_ptr<IFile>> Folder::GetFiles()
	{
		auto files = create_task(m_folder->GetFilesAsync()).get();

		std::vector<std::shared_ptr<IFile>> results;
		results.reserve(files->Size);

		auto itr = files->First();

		while (itr->HasCurrent)
		{
			auto file = std::make_shared<File>(itr->Current);
			results.push_back(file);
			itr->MoveNext();
		}

		return results;
	}

	bool Folder::ContainsFile(const std::wstring& fileName)
	{
		// this is ugly... but the existing interface doesn't allow for another option short of getting all files and then checking them individually
		bool result = false;
		Platform::String^ winrt_fileName = ref new Platform::String(fileName.c_str());
		
		try
		{
			auto file = create_task(m_folder->GetFileAsync(winrt_fileName)).get();
			result = true;
		}
		catch (Platform::COMException^ ex) // hack, force an exception to throw to find the actual one!
		{
			ARC_ASSERT_MSG(FILE_NOT_FOUND_HRESULT == ex->HResult, "Received comException with invalid HRESULT! " << ex->HResult);
			result = false;
		}

		return result;
	}

	std::shared_ptr<IFile> Folder::GetFile(const std::wstring& fileName)
	{
		ARC_ASSERT_MSG(this->ContainsFile(fileName), "Requested a file that doesn't exist! " << fileName.c_str());

		Platform::String^ winrt_fileName = ref new Platform::String(fileName.c_str());
		return std::make_shared<File>(create_task(m_folder->GetFileAsync(winrt_fileName)).get());
	}

	std::shared_ptr<IFile> Folder::CreateNewFile(const std::wstring& fileName)
	{
		const std::wstring* fn = &fileName;

		if (Storage::CheckForIllegalCharacters(fileName))
		{
			ARC_FAIL("Attempted to create a file with illegal characters! Automatically replacing for the sake of sanity!");
			std::wstring newFileName = fileName;
			Storage::RemoveIllegalCharacters(newFileName);
			fn = &newFileName;
		}

		Platform::String^ winrt_fileName = ref new Platform::String(fn->c_str());

		try
		{
			auto result = std::make_shared<File>(create_task(m_folder->CreateFileAsync(winrt_fileName, Windows::Storage::CreationCollisionOption::OpenIfExists)).get());
			return result;
		}
		catch (Platform::COMException^ ex)
		{
			ARC_FAIL("Unhandled exception!");
			return nullptr;
		}
		
	}
#else
#error UNKNOWN_PLATFORM!
#endif
}