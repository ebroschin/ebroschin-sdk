#include <gtest/gtest.h>

#include "ebroschin/utility/signal.hpp"

namespace ebroschin::utility::tests {

TEST(SignalSubscriptionTests, Emit) {
  Signal<int> signal{};
  int received = -1;

  const auto signal_subscription = signal.Subscribe([&](int value) {
    received = value;
  });

  signal.Emit(1337);
  EXPECT_EQ(received, 1337);
}

TEST(SignalSubscriptionTests, RAIIUnsubscribe) {
  Signal signal{};
  int call_count = 0;

  {
    const auto signal_subscription = signal.Subscribe([&] {
      ++call_count;
    });

    signal.Emit();
    EXPECT_EQ(call_count, 1);
  }

  signal.Emit();
  EXPECT_EQ(call_count, 1);
}

TEST(SignalSubscriptionTests, SubscriptionMove) {
  Signal signal{};
  int call_count = 0;

  //create a subscription handle optional for late binding
  std::optional<SignalSubscription> outer_signal_subscription{};
  {
    auto signal_subscription = signal.Subscribe([&] {
      ++call_count;
    });

    signal.Emit();
    EXPECT_EQ(call_count, 1);

    //move the scoped subscription handle to the outer scope
    outer_signal_subscription.emplace(std::move(signal_subscription));

    //signal_subscription is destroyed but does not unsubscribe the signal,
    //since ownership was moved to the outer scope
  }

  signal.Emit();
  EXPECT_EQ(call_count, 2);

  //resetting the optional will destroy the owned subscription
  //and unsubscribe the signal via RAII
  outer_signal_subscription.reset();

  signal.Emit();
  EXPECT_EQ(call_count, 2);
}

TEST(SignalSubscriptionTests, ExplicitUnsubscribe) {
  Signal<> signal;
  int call_count = 0;

  auto signal_subscription = signal.Subscribe([&] { ++call_count; });

  signal.Emit();
  EXPECT_EQ(call_count, 1);

  signal_subscription.Unsubscribe();
  signal.Emit();
  EXPECT_EQ(call_count, 1);
}

}