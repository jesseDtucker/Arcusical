#pragma once

#include <memory>
#include <vector>

#include "Export.hpp"
#include "IFile.hpp"
#include "WorkBuffer.hpp"

namespace FileSystem {
class EXPORT IFolder {
 public:
  virtual std::wstring GetName() const = 0;
  virtual std::wstring GetFullPath() const = 0;
  virtual std::shared_ptr<IFolder> GetParent() const = 0;
  virtual std::vector<std::shared_ptr<IFolder>> GetSubfolders() const = 0;
  virtual std::vector<std::shared_ptr<IFile>> GetFiles() const = 0;
  virtual bool ContainsFile(const std::wstring& fileName) const = 0;
  virtual std::shared_ptr<IFile> GetFile(const std::wstring& fileName) const = 0;

  virtual std::shared_ptr<IFile> CreateNewFile(const std::wstring& fileName) = 0;
  virtual std::shared_ptr<Util::WorkBuffer<std::shared_ptr<IFile>>> FindFilesWithExtensions(
      std::vector<std::wstring> extensions) = 0;

#ifdef __cplusplus_winrt
  virtual Windows::Storage::StorageFolder ^ GetRawHandle() = 0;
#endif

  virtual bool operator==(const IFolder& rhs) const = 0;
  virtual bool operator!=(const IFolder& rhs) const = 0;
};
}
