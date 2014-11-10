﻿#ifndef FILE_SYSTEM_HPP
#define FILE_SYSTEM_HPP

#include <memory>

#include "Export.hpp"

namespace FileSystem
{
	class IFolder;
	class IFileReader;
	class IFile;

	class EXPORT Storage final
	{
	public:
		static IFolder& MusicFolder();
		static IFolder& ApplicationFolder();

		static std::shared_ptr<IFile> LoadFileFromPath(std::wstring filePath);

		static std::shared_ptr<IFileReader> GetReader(IFile* file);
		static std::shared_ptr<IFileReader> GetReader(std::shared_ptr<IFile> file);

		static bool CheckForIllegalCharacters(const std::wstring& filePath);
		static void RemoveIllegalCharacters(std::wstring& filePath, wchar_t replacementCharacter = '_');
	private:

		static IFolder* s_musicFolder;
		static IFolder* s_applicationFolder;
	};
}

#endif