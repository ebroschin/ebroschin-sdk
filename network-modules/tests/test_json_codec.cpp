#include <gtest/gtest.h>

#include "ebroschin/network-modules/codecs/json_network_codec.hpp"

#include <ebroschin/network/tcp/tcp_system_concepts.hpp>

namespace ebroschin::network::modules::tests {

struct TestMessage final {
  static constexpr std::uint64_t TypeId = 93001;

  int value;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    TestMessage,
    value);

static_assert(ebroschin::network::tcp::NetworkCodec<JsonNetworkCodec, TestMessage>);

TEST(JsonCodecTests, EncodeDecode) {
  constexpr TestMessage test_message{1234};
  const auto bytes = JsonNetworkCodec::Encode(test_message);

  const auto decoded_envelope = JsonNetworkCodec::DecodeEnvelope(bytes);
  ASSERT_TRUE(decoded_envelope.has_value());
  ASSERT_EQ(decoded_envelope->first, TestMessage::TypeId);

  const auto decoded_payload = JsonNetworkCodec::Decode<TestMessage>(decoded_envelope->second);
  ASSERT_TRUE(decoded_payload.has_value());
  EXPECT_EQ(decoded_payload->value, test_message.value);
}

}