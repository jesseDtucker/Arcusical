#pragma once

#include <stdexcept>

namespace FileSystem {
class ReadTooLongException : public std::runtime_error {
 public:
  ReadTooLongException(std::string msg) : std::runtime_error(msg) {}
};
}
