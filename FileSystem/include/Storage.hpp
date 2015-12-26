#ifndef FILE_SYSTEM_HPP
#define FILE_SYSTEM_HPP

#include <memory>

#include "Export.hpp"

namespace FileSystem {
class IFolder;
class IFileReader;
class IFile;

typedef std::shared_ptr<IFile> FilePtr;

class EXPORT Storage final {
 public:
  static IFolder& MusicFolder();
  static IFolder& ApplicationFolder();

  static bool IsFolder(std::wstring filePath);
  static bool IsFile(std::wstring filePath);
  static bool FolderExists(std::wstring filePath);
  static bool FileExists(std::wstring filePath);
  static bool Exists(std::wstring filePath);

  static std::shared_ptr<IFile> LoadFileFromPath(std::wstring filePath);

  static std::shared_ptr<IFileReader> GetReader(const IFile* file);
  static std::shared_ptr<IFileReader> GetReader(const std::shared_ptr<IFile> file);

  static bool CheckForIllegalCharacters(const std::wstring& filePath);
  static void RemoveIllegalCharacters(std::wstring& filePath, wchar_t replacementCharacter = '_');

 private:
  static IFolder* s_musicFolder;
  static IFolder* s_applicationFolder;
};
}

#endif