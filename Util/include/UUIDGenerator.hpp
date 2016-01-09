#pragma once

#include "boost/uuid/uuid.hpp"
#include <random>

namespace Util {
// Simple UUID generator that uses the standard library. Needed because the boost UUID
// random generator does not compile with windows universal apps.
class UUIDGenerator {
 public:
  UUIDGenerator();
  boost::uuids::uuid operator()();

 private:
  std::independent_bits_engine<std::mt19937, CHAR_BIT, unsigned short> m_generator;
};
}