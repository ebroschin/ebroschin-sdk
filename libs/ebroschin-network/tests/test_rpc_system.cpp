#include <gtest/gtest.h>

#include "ebroschin/network/rpc/rpc_system.hpp"
#include "mock_tcp_system.hpp"
#include "mock_rpc_system.hpp"
#include "mock_rpc_timeout_handler.hpp"
#include "mock_connection_event_handler.hpp"
#include "api.hpp"

#include <thread>

namespace ebroschin::network::tests {

class RpcMockSystemTests : public testing::Test {
protected:
  void SetUp() override {
    context_.Register<MockTcpSystem>();
    context_.Register<MockRpcSystem>();
    context_.Initialize();
  }

  void TearDown() override {
    MockTcpConnector::CurrentConnection = {};
    context_.Deinitialize();
  }

  MockTcpSystem& GetTcpSystem() const {
    return context_.Require<MockTcpSystem>();
  }

  MockRpcSystem& GetRpcSystem() const {
    return context_.Require<MockRpcSystem>();
  }

private:
  core::SystemContext context_{};
};

TEST_F(RpcMockSystemTests, SuccessfulRpcCall) {
  MockConnectionEventHandler connection_event_handler{};
  auto& tcp_system = GetTcpSystem();
  auto& rpc_system = GetRpcSystem();

  tcp_system.Connect({}, &connection_event_handler);

  const auto connection_id = connection_event_handler.current_connection_id;
  ASSERT_TRUE(connection_id.has_value());

  std::condition_variable cv{};
  std::mutex mutex{};
  std::atomic success_response_received{false};
  std::atomic test_result{false};

  //run the message processor on a dedicated thread
  auto& message_processor = tcp_system.GetMessageProcessor();
  auto thread = std::jthread{[&message_processor]
  (const std::stop_token& st)
  {
    while (!st.stop_requested()) {
      message_processor.ProcessBlocking();
    }
  }};

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
  message_processor.Stop();
  thread.request_stop();
}

}