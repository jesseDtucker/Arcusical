#pragma once

#include <functional>
#include <string>
#include <unordered_set>

namespace Arcusical {
namespace Web {
class Crawler final {
 public:
  Crawler(std::function<bool(const std::wstring&)> filter);
  Crawler(const Crawler&) = delete;

  std::unordered_set<std::wstring> CrawlFrom(std::wstring addr, int saveFrequency = 50);

 private:
  std::function<bool(const std::wstring&)> m_linkFilter;
  Windows::Web::Http::HttpClient ^ m_client;
};
}
}