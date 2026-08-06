#include <gtest/gtest.h>

#include "api.hpp"
#include "mock_rpc_system.hpp"
#include "mock_rpc_timeout_handler.hpp"
#include "mock_tcp_system.hpp"

#include <ebroschin/core/synchronization/queued_executor.hpp>
#include <ebroschin/network/rpc/rpc_system.hpp>

#include <thread>

namespace ebroschin::network::tests {

class RpcMockSystemTests : public testing::Test {
protected:
  void SetUp() override {
    executor_ = std::make_unique<core::QueuedExecutor>();
    context_.Register<MockTcpSystem>(*executor_);
    context_.Register<MockRpcSystem>();
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

  MockRpcSystem& GetRpcSystem() const {
    return context_.Require<MockRpcSystem>();
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

TEST_F(RpcMockSystemTests, SuccessfulRpcCall) {
  auto& tcp_system = GetTcpSystem();
  auto& rpc_system = GetRpcSystem();

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

  std::condition_variable cv{};
  std::mutex mutex{};
  std::atomic success_response_received{false};
  std::atomic test_result{false};

  auto rpc_call = rpc_system.Prepare<RpcArithmeticRequestMessage>(*connection_id, 33);
  rpc_call.OnSuccess([&]
  (const RpcArithmeticResponseMessage& response)
  {
    bool nested_test_result = response.result == 66;
    nested_test_result &= response.request_id > 0;
    test_result.store(nested_test_result);
    success_response_received.store(true);
    cv.notify_one();
  });

  rpc_call.Call();

  ASSERT_FALSE(MockTcpConnector::CurrentConnection->GetSentBytes().empty());
  const auto request_envelope = MockNetworkCodec::DecodeEnvelope(MockTcpConnector::CurrentConnection->GetSentBytes());
  ASSERT_TRUE(request_envelope.has_value());
  const auto sent_request = MockNetworkCodec::Decode<RpcArithmeticRequestMessage>(request_envelope->second);
  ASSERT_TRUE(sent_request.has_value());

  const RpcArithmeticResponseMessage mock_response{sent_request->request_id, sent_request->operand * 2};
  MockTcpConnector::CurrentConnection->ReceiveBytes(MockNetworkCodec::Encode(mock_response));

  if (!success_response_received.load()) {
    using namespace std::chrono_literals;
    std::unique_lock lock{mutex};
    ASSERT_TRUE(cv.wait_for(lock, 5s, [&] { return success_response_received.load(); }));
  }

  ASSERT_TRUE(test_result.load());
}

}