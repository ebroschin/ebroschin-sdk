#include "ebroschin/core/system_context.hpp"

#include <algorithm>
#include <ranges>

namespace ebroschin::core {

void SystemContext::Initialize() {
  std::ranges::for_each(ordered_systems_, &System::Initialize);
}

void SystemContext::Deinitialize() const {
  std::ranges::for_each(ordered_systems_ | std::views::reverse, &System::Deinitialize);
}

}