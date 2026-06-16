#include "ebroschin/utility/arguments.hpp"

namespace ebroschin::utility {

Arguments::Arguments(int argc, char** argv) {
  const std::vector<std::string_view> arguments{argv, argv + argc};

  auto current = arguments_.end();
  for (const auto argument : arguments) {
    if (IsFlag(argument)) {
      std::string flag{argument.substr(2)};
      current = arguments_.try_emplace(flag).first;
      continue;
    }

    if (current == arguments_.end()) continue;
    current->second.emplace_back(argument);
  }
}

bool Arguments::IsFlag(std::string_view argument) const {
  if (argument.size() < 2) return false;
  if (!argument.starts_with("--")) return false;

  return true;
}

std::optional<std::span<const std::string>> Arguments::GetValues(const std::string& flag) const noexcept {
  const auto it = arguments_.find(flag);
  if (it == arguments_.end()) return std::nullopt;

  return it->second;
}

}