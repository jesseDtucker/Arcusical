#pragma once

#include <functional>
#include <future>

namespace Arcusical {
std::future<void> DispatchToUI(std::function<void()> func);
bool HasThreadAccess();
}
