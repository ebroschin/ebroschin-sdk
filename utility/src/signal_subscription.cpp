#include "ebroschin/utility/signal_subscription.hpp"

#include "ebroschin/utility/signal.hpp"

namespace ebroschin::utility {

SignalSubscription::SignalSubscription(int id, std::weak_ptr<SignalStateBase> signal_state):
  id_{id},
  signal_state_{std::move(signal_state)}
{}

SignalSubscription::SignalSubscription(SignalSubscription&& other) noexcept:
  id_{other.id_},
  signal_state_{std::move(other.signal_state_)}
{}

SignalSubscription& SignalSubscription::operator=(SignalSubscription&& other) noexcept {
  if (this == &other) return *this;
  Unsubscribe();

  id_ = other.id_;
  signal_state_ = std::move(other.signal_state_);
  return *this;
}

SignalSubscription::~SignalSubscription() {
  Unsubscribe();
}

void SignalSubscription::Unsubscribe() {
  const auto state = signal_state_.lock();
  signal_state_.reset();
  if (!state) return;

  state->Unsubscribe(id_);
}

}