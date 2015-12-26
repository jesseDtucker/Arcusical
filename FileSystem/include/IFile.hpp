#ifndef IFILE_HPP
#define IFILE_HPP

#include <string>
#include <memory>
#include <vector>

#include "Export.hpp"

namespace FileSystem {
class IFolder;

class EXPORT IFile {
 public:
  virtual std::wstring GetName() const = 0;
  virtual std::wstring GetFullName() const = 0;
  virtual std::wstring GetExtension() const = 0;
  virtual std::wstring GetFullPath() const = 0;

  virtual std::shared_ptr<IFolder> GetParent() const = 0;
  virtual std::vector<unsigned char> GetThumbnail(bool allowIcon = true) const = 0;
  virtual unsigned long long GetFileSize() const = 0;

  virtual void WriteToFile(std::vector<unsigned char>& data) = 0;
  virtual void ReadFromFile(std::vector<unsigned char>& buffer, unsigned int length = 0,
                            unsigned long long startPosition = 0) const = 0;

#ifdef __cplusplus_winrt
  virtual Windows::Storage::StorageFile ^ GetRawHandle() const = 0;
#endif
};
}

#endif