#include <gtest/gtest.h>

#include "api.hpp"
#include "mock_tcp_system.hpp"

#include <ebroschin/core/system_context.hpp>
#include <ebroschin/core/synchronization/queued_executor.hpp>

#include <chrono>
#include <thread>

namespace ebroschin::network::tests {

std::shared_ptr<MockTcpConnection> MockTcpConnector::CurrentConnection{};

class TcpSystemTests : public testing::Test {
protected:
  void SetUp() override {
    executor_ = std::make_unique<core::QueuedExecutor>();
    context_.Register<MockTcpSystem>(*executor_);
    context_.Initialize();
  }

  void TearDown() override {
    executor_->Stop();
    thread_ = {};
    MockTcpConnector::CurrentConnection = {};
    context_.Deinitialize();
  }

  MockTcpSystem& GetTcpSystem() const {
    return context_.Require<MockTcpSystem>();
  }

  void StartExecutorThread() {
    thread_ = std::jthread{[this]
    (const std::stop_token& st)
    {
      while (!st.stop_requested()) {
        executor_->ProcessBlocking();
      }
    }};
  }

private:
  core::SystemContext context_{};
  std::unique_ptr<core::QueuedExecutor> executor_{};
  std::jthread thread_{};
};

TEST_F(TcpSystemTests, SendMessage) {
  auto& tcp_system = GetTcpSystem();

  std::atomic<bool> connected{false};
  std::optional<ConnectionId> connection_id;
  tcp_system.Connect({}, [&](auto result) {
    if (!result.Ok()) return;

    connection_id = result.connection_id;
    connected.store(true);
    connected.notify_one();
  });
  StartExecutorThread();
  connected.wait(false);

  ASSERT_TRUE(connection_id.has_value());

  constexpr std::uint64_t payload_integer = 1337;
  constexpr double payload_fractional = 133.7;
  constexpr TestMessage test_message{payload_integer, payload_fractional};
  tcp_system.Send<TestMessage>(*connection_id, test_message);

  const auto sent_bytes = MockTcpConnector::CurrentConnection->GetSentBytes();
  ASSERT_EQ(sent_bytes.size(), sizeof(std::uint64_t) + sizeof(TestMessage));

  const auto decoded_envelope = MockNetworkCodec::DecodeEnvelope(sent_bytes);
  ASSERT_TRUE(decoded_envelope.has_value());
  ASSERT_EQ(decoded_envelope->first, TestMessage::TypeId);

  const auto decoded_payload = MockNetworkCodec::Decode<TestMessage>(decoded_envelope->second);
  ASSERT_TRUE(decoded_payload.has_value());
  EXPECT_EQ(decoded_payload->integer, payload_integer);
  EXPECT_EQ(decoded_payload->fractional, payload_fractional);
}

TEST_F(TcpSystemTests, ReceiveMessage) {
  auto& tcp_system = GetTcpSystem();

  std::atomic<bool> connected{false};
  std::optional<ConnectionId> connection_id;
  tcp_system.Connect({}, [&](auto result) {
    if (!result.Ok()) return;

    connection_id = result.connection_id;
    connected.store(true);
    connected.notify_one();
  });
  StartExecutorThread();
  connected.wait(false);

  ASSERT_TRUE(connection_id.has_value());

  constexpr std::uint64_t payload_integer = 1337;
  constexpr double payload_fractional = 133.7;
  constexpr TestMessage test_message{payload_integer, payload_fractional};

  std::condition_variable cv{};
  std::mutex mutex{};
  std::atomic message_processed{false};
  std::atomic test_result{false};

  auto message_event_subscription = tcp_system.Subscribe<TestMessage>([&]
  (const NetworkEvent<TestMessage>& event)
  {
    bool nested_test_result = event.connection_id.value_or(0) == *connection_id;
    nested_test_result &= event.data.integer == payload_integer;
    nested_test_result &= event.data.fractional == payload_fractional;

    test_result.store(nested_test_result);
    message_processed.store(true);
    cv.notify_one();
  });

  MockTcpConnector::CurrentConnection->ReceiveBytes(MockNetworkCodec::Encode(test_message));

  //wait until the message handler is called asynchronously
  if (!message_processed.load()) {
    using namespace std::chrono_literals;
    std::unique_lock lock{mutex};
    ASSERT_TRUE(cv.wait_for(lock, 5s, [&] { return message_processed.load(); }));
  }

  ASSERT_TRUE(test_result.load());
}

}