#include "Storage.hpp"

#include "boost/algorithm/string/predicate.hpp"
#include <algorithm>
#include <future>
#include <ppltasks.h>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "Arc_Assert.hpp"
#include "File.hpp"
#include "FileReader.hpp"
#include "Folder.hpp"

using namespace concurrency;
using namespace FileSystem;
using namespace Platform;
using namespace std;
using namespace Windows::Foundation;
using namespace Windows::Storage;

IFolder* Storage::s_musicFolder = nullptr;
IFolder* Storage::s_applicationFolder = nullptr;

static mutex s_loadingLock;
static mutex s_lookupLock;
static unordered_map<wstring, StorageFile ^> s_registeredFiles;
static const unordered_set<wchar_t> ILLEGAL_CHARACTERS = {'?', '<', '>', ':', '*', '|', '^', '/'};

#ifdef __cplusplus_winrt

IFolder& Storage::MusicFolder() {
  if (s_musicFolder == nullptr) {
    unique_lock<mutex> lockguard(s_loadingLock);
    if (s_musicFolder == nullptr) {
      s_musicFolder = new Folder(::KnownFolders::MusicLibrary);
    }
  }

  auto& folder = *s_musicFolder;

#ifdef _DEBUG
  static shared_ptr<IFolder> debugFolder = nullptr;
  auto folders = folder.GetSubfolders();
  for (auto& f : folders) {
    if (f->GetName() == L"Test" || f->GetName() == L"test") {
      debugFolder = f;
      break;
    }
  }

  if (debugFolder != nullptr) {
    return *debugFolder;
  }
#endif

  return folder;
}

shared_ptr<IFile> LookupRegisteredFile(const wstring& filePath) {
  unique_lock<mutex> lockguard(s_lookupLock);
  auto registeredFile = s_registeredFiles.find(filePath);
  if (registeredFile != end(s_registeredFiles)) {
    return make_shared<File>(registeredFile->second);
  } else {
    return nullptr;
  }
}

IFolder& Storage::ApplicationFolder() {
  if (s_applicationFolder == nullptr) {
    unique_lock<mutex> lockguard(s_loadingLock);
    if (s_applicationFolder == nullptr) {
      s_applicationFolder = new Folder(::ApplicationData::Current->LocalFolder);
    }
  }

  return *s_applicationFolder;
}

shared_ptr<IFileReader> Storage::GetReader(const IFile* file) { return make_shared<FileReader<const IFile*>>(file); }

shared_ptr<IFileReader> Storage::GetReader(const shared_ptr<IFile> file) {
  return make_shared<FileReader<const shared_ptr<IFile>>>(file);
}

shared_ptr<IFile> Storage::LoadFileFromPath(wstring filePath) {
  ARC_ASSERT(filePath.size() > 0);
  ARC_ASSERT_MSG(CheckForIllegalCharacters(filePath), "Path to load contained illegal characters!");
  shared_ptr<IFile> file = nullptr;

  auto registeredFile = LookupRegisteredFile(filePath);
  if (registeredFile != nullptr) {
    return registeredFile;
  }

  try {
    if (filePath.find(L"ms-appx") != wstring::npos) {
      auto uri = ref new Uri(ref new String(filePath.c_str()));
      auto winRTfile = create_task(StorageFile::GetFileFromApplicationUriAsync(uri)).get();
      file = make_shared<File>(winRTfile);
    } else {
      replace(filePath.begin(), filePath.end(), L'/', L'\\');
      auto winRTfile = create_task(StorageFile::GetFileFromPathAsync(ref new String(filePath.c_str()))).get();
      file = make_shared<File>(winRTfile);
    }
  } catch (Exception ^ ex) {
    ARC_FAIL(wstring(ex->Message->Data()).c_str());
  }

  return file;
}

bool Storage::CheckForIllegalCharacters(const wstring& filePath) {
  for (auto& badChar : ILLEGAL_CHARACTERS) {
    if (filePath.find(badChar) != basic_string<wchar_t>::npos) {
      return true;
    }
  }

  return false;
}

void Storage::RemoveIllegalCharacters(wstring& filePath, wchar_t replacementCharacter /* = '_' */) {
  replace(filePath.begin(), filePath.end(), L'/', L'\\');

  for (auto& badChar : ILLEGAL_CHARACTERS) {
    replace(begin(filePath), end(filePath), badChar, replacementCharacter);
  }
}

void Storage::RegisterStorageItem(Windows::Storage::IStorageItem ^ storageItem) {
  unique_lock<mutex> lockguard(s_lookupLock);
  auto path = wstring(storageItem->Path->Data());
  auto storageFile = dynamic_cast<StorageFile ^>(storageItem);
  ARC_ASSERT_MSG(storageFile != nullptr, "Only supporting file registration for now. Folders not yet supported");
  s_registeredFiles[path] = storageFile;
}

bool Storage::IsFile(wstring filePath) {
  auto result = (LookupRegisteredFile(filePath) != nullptr);
  if (!result && filePath.size() > 0) {
    try {
      if (boost::istarts_with(filePath, L"ms-appx:///")) {
        auto path = ref new String(filePath.c_str());
        auto uri = ref new Uri(path);
        auto res = create_task(StorageFile::GetFileFromApplicationUriAsync(uri)).get();
        result = true;
      } else {
        auto res = create_task(StorageFile::GetFileFromPathAsync(ref new String(filePath.c_str()))).get();
        result = true;
      }
    } catch (COMException ^ ex) {
      // files that do not exist are not files
      ARC_ASSERT_MSG(ex->HResult == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND), "unexpected HRESULT!");
    }
  }

  return result;
}

bool Storage::IsFolder(wstring filePath) {
  auto result = false;
  try {
    auto res = create_task(::StorageFolder::GetFolderFromPathAsync(ref new String(filePath.c_str()))).get();
    result = true;
  } catch (COMException ^ ex) {
    ARC_ASSERT_MSG(ex->HResult == E_INVALIDARG, "unexpected HRESULT!");
  }

  return result;
}

bool Storage::FileExists(wstring filePath) { return IsFile(filePath); }

bool Storage::FolderExists(wstring filePath) { return IsFolder(filePath); }

bool Storage::Exists(wstring filePath) { return FileExists(filePath) || FolderExists(filePath); }

#else
#error UNSUPPORTED PLATFORM!
#endif