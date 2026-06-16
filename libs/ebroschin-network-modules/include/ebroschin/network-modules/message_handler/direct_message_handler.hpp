#pragma once

#include <ebroschin/network/commons.hpp>
#include <ebroschin/utility/variadic.hpp>

#include <functional>
#include <tuple>

namespace ebroschin::network::modules {

template<typename... TMessages>
class DirectMessageHandler {
public:
  template<typename TMessage>
  void HandleMessage(ConnectionId id, const TMessage& message) {
    constexpr int index = utility::IndexOf<TMessage, TMessages...>();
    auto& handler = std::get<index>(handlers_);
    if (!handler) return;

    handler(id, message);
  }

  template<typename TMessage>
  void Register(std::function<void(ConnectionId, const TMessage&)> function) {
    constexpr int index = utility::IndexOf<TMessage, TMessages...>();
    std::get<index>(handlers_) = std::move(function);
  }

private:
  std::tuple<std::function<void(ConnectionId, const TMessages&)>...> handlers_{};
};

}