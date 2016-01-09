#include <algorithm>
#include <codecvt>
#include <iostream>
#include <ppltasks.h>
#include <regex>

#include "Arc_Assert.hpp"
#include "CachedCrawl.pb.h"
#include "IFile.hpp"
#include "IFolder.hpp"
#include "Storage.hpp"
#include "WebCrawler.hpp"

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Filters;

using namespace std;
using namespace FileSystem;

namespace Arcusical {
namespace Web {

static wregex linkRegex(L"a href=\"([^\"]*)\"");

Crawler::Crawler(std::function<bool(const wstring&)> filter) : m_linkFilter(filter) { m_client = ref new HttpClient(); }

std::wstring GetCachedPath(const wstring& addr) {
  auto cachePath = addr + L"_web_cache.cache";
  Storage::RemoveIllegalCharacters(cachePath);
  std::replace(begin(cachePath), end(cachePath), L'\\', L'_');

  cachePath = L"webCache\\" + cachePath;

  return cachePath;
}

void LoadFromCached(const wstring& addr, unordered_set<wstring>& checkedUrls, unordered_set<wstring>& uncheckedUrls,
                    unordered_set<wstring>& allUrls) {
  auto cachePath = GetCachedPath(addr);
  if (Storage::ApplicationFolder().ContainsFile(cachePath)) {
    // load the file
    auto cacheFile = Storage::ApplicationFolder().GetFile(cachePath);
    vector<unsigned char> fileRawContents;
    cacheFile->ReadFromFile(fileRawContents);

    CachedCrawl cachedCrawl;
    cachedCrawl.ParseFromArray(fileRawContents.data(), fileRawContents.size());

    wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;

    for (int i = 0; i < cachedCrawl.checkedurls_size(); ++i) {
      checkedUrls.insert(converter.from_bytes(cachedCrawl.checkedurls(i)));
    }
    for (int i = 0; i < cachedCrawl.uncheckedurls_size(); ++i) {
      uncheckedUrls.insert(converter.from_bytes(cachedCrawl.uncheckedurls(i)));
    }
    for (int i = 0; i < cachedCrawl.allfoundurls_size(); ++i) {
      allUrls.insert(converter.from_bytes(cachedCrawl.allfoundurls(i)));
    }
  } else {
    uncheckedUrls.insert(addr);
  }
}

vector<wstring> GetLinks(const wstring& page, const wstring& baseAddr) {
  vector<wstring> urls;
  wsmatch matches;
  const ctype<wchar_t>& f = use_facet<ctype<wchar_t>>(locale());

  wsregex_token_iterator itr(begin(page), end(page), linkRegex, vector<int>({1}));
  decltype(itr) end;

  for (; itr != end; ++itr) {
    auto url = itr->str();
    if (url[0] == '\\' || url[0] == '/') {
      url = baseAddr + url;
    }

    f.tolower(&url[0], &url[0] + url.size());
    urls.push_back(url);
  }

  return urls;
}

bool checkIsLink(const wstring& link) {
  auto res = link.find(L"http") != std::wstring::npos;
  return res;
}

void FilterLinks(vector<wstring>& links, unordered_set<wstring>& allUrls, std::function<bool(const wstring&)>& filter) {
  auto rend = remove_if(begin(links), end(links), [&allUrls, &filter](const wstring& url) {
    auto result = true;
    if (allUrls.find(url) == end(allUrls)) {
      // haven't seen this yet, check with filters
      result = !(checkIsLink(url) && filter(url));
    } else {
      result = true;
    }
    allUrls.insert(url);

    return result;
  });
  links.resize(rend - begin(links));
}

void SaveToCache(const wstring& addr, const unordered_set<wstring>& checkedUrls,
                 const unordered_set<wstring>& uncheckedUrls, const unordered_set<wstring>& allUrls) {
  auto cachePath = GetCachedPath(addr);
  auto file = Storage::ApplicationFolder().CreateNewFile(cachePath);
  CachedCrawl cache;

  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

  for (auto& url : checkedUrls) {
    cache.add_checkedurls(converter.to_bytes(url));
  }
  for (auto& url : uncheckedUrls) {
    cache.add_uncheckedurls(converter.to_bytes(url));
  }
  for (auto& url : allUrls) {
    cache.add_allfoundurls(converter.to_bytes(url));
  }

  std::vector<unsigned char> buffer(cache.ByteSize());
  auto result = cache.SerializeToArray(buffer.data(), buffer.size());
  ARC_ASSERT_MSG(result, "Failed to serialize web crawler cache!");

  file->WriteToFile(buffer);
}

unordered_set<wstring> Crawler::CrawlFrom(wstring addr, int saveFrequency /* = 50 */) {
  try {
    unordered_set<wstring> checkedUrls;
    unordered_set<wstring> uncheckedUrls;
    unordered_set<wstring> allUrls;

    // attempt load from file
    LoadFromCached(addr, checkedUrls, uncheckedUrls, allUrls);

    int counter = checkedUrls.size();

    // where there are urls to check
    while (uncheckedUrls.size() > 0) {
      auto nextItr = std::begin(uncheckedUrls);
      auto nextUrl = *nextItr;

      OutputDebugStringA(to_string(counter).c_str());
      OutputDebugStringW(L"\tNext url to check : ");
      OutputDebugStringW(nextUrl.c_str());
      OutputDebugStringW(L"\n");

      Uri ^ uri = ref new Uri(ref new String(nextUrl.c_str()));

      // get a page
      try {
        auto page = create_task(m_client->GetAsync(uri)).get();

        // get all links from the page and then filter out the ones that we already have or that the user rejects
        auto links = GetLinks(page->Content->ToString()->Data(), addr);
        FilterLinks(links, allUrls, m_linkFilter);

        OutputDebugStringA((string("Added ") + to_string(links.size()) + string(" more links to search. There are ") +
                            to_string(uncheckedUrls.size()) + string(" links remaining\n"))
                               .c_str());

        for (auto& link : links) {
          uncheckedUrls.insert(link);
        }

        // remove url and continue
        uncheckedUrls.erase(nextItr);
        checkedUrls.insert(nextUrl);

        ++counter;
        if (counter % saveFrequency == 0) {
          SaveToCache(addr, checkedUrls, uncheckedUrls, allUrls);
        }
      } catch (Platform::COMException ^ ex) {
        OutputDebugStringW((L"Failed to connect to : " + nextUrl).c_str());
        OutputDebugStringW(ex->Message->Data());
      }
    }

    return allUrls;
  } catch (Platform::COMException ^ ex) {
    ARC_FAIL(ex->Message->Data());
  } catch (...) {
    ARC_FAIL("oops...");
  }

  return {};
}
}
}