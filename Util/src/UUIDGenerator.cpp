#include "UUIDGenerator.hpp"

#include <algorithm>

using namespace std;
using namespace boost::uuids;

namespace Util {
UUIDGenerator::UUIDGenerator() : m_generator(mt19937(random_device()())) {}

uuid UUIDGenerator::operator()() {
  uuid id;
  generate(begin(id), end(id), ref(m_generator));
  return id;
}
}