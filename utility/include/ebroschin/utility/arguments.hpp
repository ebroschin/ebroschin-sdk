#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <span>
#include <optional>

namespace ebroschin::utility {

class Arguments {
public:
  using ArgumentsMap = std::unordered_map<std::string, std::vector<std::string>>;

  explicit Arguments(int argc, char** argv);
  ~Arguments() = default;

  Arguments(const Arguments&) = delete;
  Arguments& operator=(const Arguments&) = delete;
  Arguments(Arguments&&) = delete;
  Arguments& operator=(Arguments&&) = delete;

  std::optional<std::span<const std::string>> GetValues(const std::string& flag) const noexcept;

private:
  bool IsFlag(std::string_view argument) const;

  ArgumentsMap arguments_;
};

}