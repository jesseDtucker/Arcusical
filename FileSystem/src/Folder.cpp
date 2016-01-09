#include "Folder.hpp"
#include <algorithm>
#include <collection.h>
#include <ppltasks.h>

#include "Arc_Assert.hpp"
#include "File.hpp"
#include "Storage.hpp"

using namespace concurrency;
using namespace std;

#define FILE_NOT_FOUND_HRESULT 0x80070002

namespace FileSystem {
#ifdef __cplusplus_winrt

Folder::Folder(Windows::Storage::StorageFolder ^ folder) : m_folder(folder) { ARC_ASSERT(folder != nullptr); }

wstring Folder::GetName() const { return wstring(m_folder->Name->Data()); }

wstring Folder::GetFullPath() const { return wstring(m_folder->Path->Data()); }

vector<shared_ptr<IFolder>> Folder::GetSubfolders() const {
  auto subFolders = create_task(m_folder->GetFoldersAsync()).get();

  vector<shared_ptr<IFolder>> results;
  results.reserve(subFolders->Size);

  auto itr = subFolders->First();

  while (itr->HasCurrent) {
    auto folder = make_shared<Folder>(itr->Current);
    results.push_back(folder);
    itr->MoveNext();
  }

  return results;
}

vector<shared_ptr<IFile>> Folder::GetFiles() const {
  auto files = create_task(m_folder->GetFilesAsync()).get();

  vector<shared_ptr<IFile>> results;
  results.reserve(files->Size);

  auto itr = files->First();

  while (itr->HasCurrent) {
    auto file = make_shared<File>(itr->Current);
    results.push_back(file);
    itr->MoveNext();
  }

  return results;
}

bool Folder::ContainsFile(const wstring& fileName) const {
  // this is ugly... but the existing interface doesn't allow for another option short of getting all files and then
  // checking them individually
  bool result = false;
  Platform::String ^ winrt_fileName = ref new Platform::String(fileName.c_str());

  try {
    auto file = create_task(m_folder->GetFileAsync(winrt_fileName)).get();
    result = true;
  } catch (Platform::COMException ^ ex)  // hack, force an exception to throw to find the actual one!
  {
    ARC_ASSERT_MSG(FILE_NOT_FOUND_HRESULT == ex->HResult, "Received comException with invalid HRESULT! "
                                                              << ex->HResult);
    result = false;
  }

  return result;
}

shared_ptr<IFile> Folder::GetFile(const wstring& fileName) const {
  ARC_ASSERT_MSG(this->ContainsFile(fileName), "Requested a file that doesn't exist! " << fileName.c_str());

  Platform::String ^ winrt_fileName = ref new Platform::String(fileName.c_str());
  return make_shared<File>(create_task(m_folder->GetFileAsync(winrt_fileName)).get());
}

shared_ptr<IFolder> Folder::GetParent() const {
  shared_ptr<IFolder> result = nullptr;
  try {
    auto parent = create_task(m_folder->GetParentAsync()).get();
    if (parent != nullptr) {
      result = make_shared<Folder>(parent);
    }
  } catch (Platform::COMException ^ ex) {
    ARC_FAIL("Unhandled exception!");
  }
  return result;
}

shared_ptr<IFile> Folder::CreateNewFile(const wstring& fileName) {
  const wstring* fn = &fileName;

  if (Storage::CheckForIllegalCharacters(fileName)) {
    ARC_FAIL("Attempted to create a file with illegal characters! Automatically replacing for the sake of sanity!");
    wstring newFileName = fileName;
    Storage::RemoveIllegalCharacters(newFileName);
    fn = &newFileName;
  }

  Platform::String ^ winrt_fileName = ref new Platform::String(fn->c_str());

  try {
    auto result = make_shared<File>(
        create_task(m_folder->CreateFileAsync(winrt_fileName, Windows::Storage::CreationCollisionOption::OpenIfExists))
            .get());
    return result;
  } catch (Platform::COMException ^ ex) {
    ARC_FAIL("Unhandled exception!");
    return nullptr;
  }
}

bool Folder::operator==(const IFolder& rhs) const {
  auto t2 = this->GetFullPath();
  auto t3 = rhs.GetFullPath();
  return t2 == t3;
}

bool Folder::operator!=(const IFolder& rhs) const { return !(*this == rhs); }

shared_ptr<Util::WorkBuffer<shared_ptr<IFile>>> Folder::FindFilesWithExtensions(vector<wstring> extensions) {
  using namespace Platform;
  using namespace Windows::Foundation::Collections;
  using namespace Windows::Storage;
  using namespace Windows::Storage::Search;

  auto buffer = make_shared<Util::WorkBuffer<shared_ptr<IFile>>>();

  auto ext = ref new Collections::Vector<String ^>();
  for (const auto& extension : extensions) {
    ext->Append(ref new String(extension.c_str()));
  }

  auto options = ref new QueryOptions();
  options->FolderDepth = FolderDepth::Deep;

  create_task(m_folder->CreateFolderQueryWithOptions(options)->GetFoldersAsync())
      .then([buffer, ext](IVectorView<StorageFolder ^> ^ allFolders) {
        vector<Concurrency::task<void>> tasks;

        for (const auto& folder : allFolders) {
          auto options = ref new QueryOptions(CommonFileQuery::DefaultQuery, ext);
          options->FolderDepth = FolderDepth::Shallow;
          auto task = create_task(folder->CreateFileQueryWithOptions(options)->GetFilesAsync())
                          .then([buffer](IVectorView<StorageFile ^> ^ files) {
                            vector<shared_ptr<IFile>> results;
                            transform(begin(files), end(files), back_inserter(results),
                                      [](StorageFile ^ file) { return make_shared<File>(file); });

                            buffer->Append(std::move(results));
                          });

          tasks.push_back(task);
        }

        for (const auto& task : tasks) {
          task.wait();
        }

        buffer->Complete();
      });

  return buffer;
}

#else
#error UNKNOWN_PLATFORM!
#endif
}