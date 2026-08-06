#pragma once

#include "commons.hpp"

#include <ebroschin/utility/signal.hpp>
#include <ebroschin/utility/variadic.hpp>

#include <tuple>

namespace ebroschin::network {

template <typename... TEvents>
class NetworkEventHandler {
public:

  template <typename TEvent>
  using Signal = utility::Signal<const NetworkEvent<TEvent>&>;

  template <typename TEvent>
  using Slot = Signal<TEvent>::Slot;

  using SubscriptionHandle = utility::SignalSubscription;

  NetworkEventHandler() = default;
  NetworkEventHandler(NetworkEventHandler& other) = delete;
  NetworkEventHandler& operator=(const NetworkEventHandler& other) = delete;
  NetworkEventHandler(NetworkEventHandler&& other) = delete;
  NetworkEventHandler& operator=(NetworkEventHandler&& other) = delete;

  template <typename TEvent>
  [[nodiscard]] utility::SignalSubscription Subscribe(Slot<TEvent> slot) {
    constexpr auto index = utility::IndexOf<TEvent, TEvents...>();
    auto& signal = std::get<index>(signals_);

    return signal.Subscribe(std::move(slot));
  }

  template<typename TEvent>
  void Emit(const NetworkEvent<TEvent>& event) {
    constexpr auto index = utility::IndexOf<TEvent, TEvents...>();
    auto& signal = std::get<index>(signals_);

    signal.Emit(event);
  }

private:
  std::tuple<utility::Signal<const NetworkEvent<TEvents>&>...> signals_{};
};

}