#pragma once

#include <string>
#include <vector>

#include "IFolder.hpp"
#include "Util.hpp"
#include "WorkBuffer.hpp"

namespace FileSystem {
class Storage;
class IFile;

class Folder final : public IFolder {
 public:
  virtual std::wstring GetName() const override;
  virtual std::wstring GetFullPath() const override;
  virtual std::shared_ptr<IFolder> GetParent() const override;
  virtual std::vector<std::shared_ptr<IFolder>> GetSubfolders() const override;
  virtual std::vector<std::shared_ptr<IFile>> GetFiles() const override;
  virtual bool ContainsFile(const std::wstring& fileName) const override;
  virtual std::shared_ptr<IFile> GetFile(const std::wstring& fileName) const override;

  virtual std::shared_ptr<IFile> CreateNewFile(const std::wstring& filename) override;
  virtual std::shared_ptr<Util::WorkBuffer<std::shared_ptr<IFile>>> FindFilesWithExtensions(
      std::vector<std::wstring> extensions) override;

  virtual bool operator==(const IFolder& rhs) const override;
  virtual bool operator!=(const IFolder& rhs) const override;

#ifdef __cplusplus_winrt
  Folder(Windows::Storage::StorageFolder ^ folder);
  virtual Windows::Storage::StorageFolder ^ GetRawHandle() override;

 private:
  Windows::Storage::StorageFolder ^ m_folder;
#endif

  friend Storage;  // so the static folders ( ie. music library) can be instantiated
};
}
