#pragma once
#ifndef MUSIC_FINDER_HPP
#define MUSIC_FINDER_HPP

#include <vector>
#include <future>

#include "Util.hpp"

namespace FileSystem
{
	class IFile;
	class IFolder;
}

namespace Arcusical
{
namespace LocalMusicStore
{
	class MusicFinder final
	{
	public:

		std::future<std::vector<std::shared_ptr<FileSystem::IFile>>> FindSongs();

	private:
		void SearchFolder(FileSystem::IFolder& folder);

		std::vector<std::shared_ptr<FileSystem::IFile>> m_filesFound;

		static const std::vector<std::wstring> s_supportedFileEndings;
	};
}
}

#endif