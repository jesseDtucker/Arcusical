#include <algorithm>
#include <future>

#include "MusicFinder.hpp"
#include "Storage.hpp"
#include "IFile.hpp"
#include "IFolder.hpp"

using namespace FileSystem;

namespace Arcusical
{
namespace LocalMusicStore
{
	const std::vector<std::wstring> MusicFinder::s_supportedFileEndings = { L"m4a", L"mp3", L"wav" };

	std::future<std::vector<std::shared_ptr<IFile>>> MusicFinder::FindSongs()
	{
		return std::async([this]()
		{
			// recursively searches the folder to find music
			SearchFolder(Storage::MusicFolder());

			std::vector<std::shared_ptr<IFile>> results;

			for (auto file : m_filesFound)
			{
				// if the extension matches any of the supported extensions then include it in our search results
				auto currentFileExtension = file->GetExtension();
				if (std::any_of(s_supportedFileEndings.begin(), s_supportedFileEndings.end(), [&currentFileExtension](std::wstring fileEnding)
				{
					return currentFileExtension == fileEnding;
				}))
				{
					results.push_back(file);
				}
			}

			return results;
		});
	}

	void MusicFinder::SearchFolder(IFolder& folder)
	{
		auto subFolders = folder.GetSubfolders();
		auto files = folder.GetFiles();

		m_filesFound.reserve(m_filesFound.size() + files.size());
		m_filesFound.insert(m_filesFound.end(), files.begin(), files.end());

		for (auto subFolder : subFolders)
		{
			SearchFolder(*subFolder);
		}
	}
}
}