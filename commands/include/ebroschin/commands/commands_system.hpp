#pragma once

#include <ebroschin/core/system.hpp>
#include <ebroschin/utility/variadic.hpp>

#include <functional>
#include <span>
#include <unordered_map>

namespace ebroschin::commands {

template <typename TCommand>
concept Command = requires(TCommand command, std::span<std::string_view> arguments) {
  { TCommand::Token } -> std::convertible_to<std::string_view>;
  { TCommand::Description } -> std::convertible_to<std::string_view>;
  { command.Execute(arguments) } -> std::same_as<void>;
};

template <Command... TCommands>
class CommandsSystem final : public core::System {
  using CommandArguments = std::span<std::string_view>;
  using CommandHandler = std::function<void(CommandArguments)>;

public:
  using Commands = std::tuple<TCommands...>;

  explicit CommandsSystem(const core::SystemContext& ctx) noexcept:
    System(ctx),
    commands_(std::make_tuple(TCommands(ctx)...))
  {
    (Register<TCommands>(), ...);
  }

  bool Execute(const std::string& token, CommandArguments arguments) {
    const auto it = handlers_.find(token);
    if (it == handlers_.end()) return false;

    it->second(arguments);
    return true;
  }

private:
  template <typename TCommand>
  void Register() {
    std::string key{TCommand::Token};
    handlers_.try_emplace(key, [this](CommandArguments arguments) { Execute<TCommand>(arguments); });
  }

  template <typename TCommand>
  void Execute(CommandArguments arguments) {
    constexpr auto index = utility::IndexOf<TCommand, TCommands...>();
    auto& command = std::get<index>(commands_);
    command.Execute(arguments);
  }

  std::tuple<TCommands...> commands_;
  std::unordered_map<std::string, CommandHandler> handlers_{};
};

}