#pragma once

#include "direct_message_handler.hpp"

#include <ebroschin/network/commons.hpp>
#include <ebroschin/utility/signal.hpp>
#include <ebroschin/utility/variadic.hpp>

#include <tuple>

namespace ebroschin::network::modules {

template <typename... TMessages>
class ObservableMessageHandler {
public:
  using SubscriptionHandle = utility::SignalSubscription;

  explicit ObservableMessageHandler()
  {
    (Register<TMessages>(), ...);
  }

  ObservableMessageHandler(ObservableMessageHandler& other) = delete;
  ObservableMessageHandler& operator=(ObservableMessageHandler& other) = delete;

  template<typename TMessage>
  [[nodiscard]] utility::SignalSubscription Subscribe(utility::Signal<ConnectionId, const TMessage&>::Slot slot) {
    constexpr int index = utility::IndexOf<TMessage, TMessages...>();
    auto& signal = std::get<index>(signals_);

    return signal.Subscribe(std::move(slot));
  }

  template<typename TMessage>
  void HandleMessage(ConnectionId id, const TMessage& message) {
    constexpr int index = utility::IndexOf<TMessage, TMessages...>();

    auto& signal = std::get<index>(signals_);
    signal.Emit(id, message);
  }

private:
  template<typename TMessage>
  void Register() {
    registry_.template Register<TMessage>([this](ConnectionId id, const TMessage& message) {
      HandleMessage(id, message);
    });
  }

  DirectMessageHandler<TMessages...> registry_{};
  std::tuple<utility::Signal<ConnectionId, const TMessages&> ...> signals_{};
};

}