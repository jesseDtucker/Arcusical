#pragma once

#include <string>
#include <vector>

#include "IFile.hpp"
#include "Util.hpp"

namespace FileSystem {
class File final : public IFile {
 public:
  virtual std::wstring GetName() const override;
  virtual std::wstring GetFullName() const override;
  virtual std::wstring GetExtension() const override;
  virtual std::wstring GetFullPath() const override;

  virtual std::shared_ptr<IFolder> GetParent() const override;
  virtual unsigned long long GetFileSize() const override;
  virtual std::vector<unsigned char> GetThumbnail(bool allowIcon = true) const override;

  virtual void WriteToFile(std::vector<unsigned char>& data) override;
  virtual void ReadFromFile(std::vector<unsigned char>& buffer, unsigned int length = 0,
                            unsigned long long startPosition = 0) const override;

  // This should not be instantiated by anything outside of the FileSystem project!
  // unfortunately it cannot be private due to the need to use make_shared. I'll find
  // a better workaround later.
  File(Windows::Storage::StorageFile ^ file);

  virtual Windows::Storage::StorageFile ^ GetRawHandle() const override;

 private:
  Windows::Storage::StorageFile ^ m_file;
};
}
