#pragma once

#include <ebroschin/network/rpc/commons.hpp>

#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

namespace ebroschin::chatcore::api {

using PersistenceId = std::uint32_t;

struct ChatMessage {
  PersistenceId id;
  PersistenceId channel_id;
  PersistenceId user_id;
  std::string content;
};

struct ChatChannel {
  PersistenceId id;
  std::string name;
};

struct User {
  PersistenceId id;
  std::string name;
};

struct PrintMessage {
  static constexpr std::uint64_t TypeId = 99;

  std::string value;
};

struct ErrorResponseMessage {
  static constexpr std::uint64_t TypeId = 100;

  network::RequestId request_id;
  std::string value;
};

struct ErrorMessage {
  static constexpr std::uint64_t TypeId = 101;

  std::string value;
};

struct ShutdownMessage {
  static constexpr std::uint64_t TypeId = 102;
};

struct ReceiveChatMessage {
  static constexpr std::uint64_t TypeId = 103;

  PersistenceId user_id;
  PersistenceId channel_id;
  std::string content;
};

struct WriteChatMessage {
  static constexpr std::uint64_t TypeId = 104;

  std::string content;
};

struct GetChatsRequestMessage {
  static constexpr std::uint64_t TypeId = 105;

  network::RequestId request_id;
  PersistenceId channel_id;
  std::uint32_t limit;
};

struct GetChatsResponseMessage {
  static constexpr std::uint64_t TypeId = 106;

  network::RequestId request_id;
  PersistenceId channel_id;
  std::vector<ChatMessage> messages;
};

struct CreateChannelRequestMessage {
  static constexpr std::uint64_t TypeId = 107;

  network::RequestId request_id;
  std::string name;
};

struct CreateChannelResponseMessage {
  static constexpr std::uint64_t TypeId = 108;

  network::RequestId request_id;
  ChatChannel channel;
};

struct CreateUserRequestMessage {
  static constexpr std::uint64_t TypeId = 109;

  network::RequestId request_id;
  std::string name;
  std::string password;
};

struct CreateUserResponseMessage {
  static constexpr std::uint64_t TypeId = 110;

  network::RequestId request_id;
  User user;
};

struct GetUsersRequestMessage {
  static constexpr std::uint64_t TypeId = 111;

  network::RequestId request_id;
  std::vector<PersistenceId> user_ids;
};

struct GetUsersResponseMessage {
  static constexpr std::uint64_t TypeId = 112;

  network::RequestId request_id;
  std::vector<User> users;
};

struct GetUserRequestMessage {
  static constexpr std::uint64_t TypeId = 113;

  network::RequestId request_id;
  std::string name;
};

struct GetUserResponseMessage {
  static constexpr std::uint64_t TypeId = 114;

  network::RequestId request_id;
  User user;
};

struct AuthenticateUserRequestMessage {
  static constexpr std::uint64_t TypeId = 115;

  network::RequestId request_id;
  std::string name;
  std::string password;
};

struct AuthenticateUserResponseMessage {
  static constexpr std::uint64_t TypeId = 116;

  network::RequestId request_id;
  User user;
};

struct JoinChatChannelRequestMessage {
  static constexpr std::uint64_t TypeId = 117;

  network::RequestId request_id;
  PersistenceId channel_id;
};

struct JoinChatChannelResponseMessage {
  static constexpr std::uint64_t TypeId = 118;

  network::RequestId request_id;
  PersistenceId channel_id;
};

struct GetChatChannelsRequestMessage {
  static constexpr std::uint64_t TypeId = 119;

  network::RequestId request_id;
};

struct GetChatChannelsResponseMessage {
  static constexpr std::uint64_t TypeId = 120;

  network::RequestId request_id;
  std::vector<ChatChannel> channels;
};

struct GetChatChannelRequestMessage {
  static constexpr std::uint64_t TypeId = 121;

  network::RequestId request_id;
  PersistenceId channel_id;
};

struct GetChatChannelResponseMessage {
  static constexpr std::uint64_t TypeId = 122;

  network::RequestId request_id;
  ChatChannel channel;
};

struct LogoutRequestMessage {
  static constexpr std::uint64_t TypeId = 123;

  network::RequestId request_id;
};

struct LogoutResponseMessage {
  static constexpr std::uint64_t TypeId = 124;

  network::RequestId request_id;
};

struct UserLogoutEventMessage {
  static constexpr std::uint64_t TypeId = 125;

  PersistenceId user_id;
};

struct UserLoginEventMessage {
  static constexpr std::uint64_t TypeId = 126;

  PersistenceId user_id;
};

struct ChannelLeaveEventMessage {
  static constexpr std::uint64_t TypeId = 127;

  PersistenceId channel_id;
  PersistenceId user_id;
};

struct ChannelJoinEventMessage {
  static constexpr std::uint64_t TypeId = 128;

  PersistenceId channel_id;
  PersistenceId user_id;
};

struct ChannelCreateEventMessage {
  static constexpr std::uint64_t TypeId = 129;

  ChatChannel channel;
  PersistenceId user_id;
};

using MessageTypes = std::tuple<
  PrintMessage,
  ErrorMessage,
  ErrorResponseMessage,
  ShutdownMessage,
  ReceiveChatMessage,
  WriteChatMessage,
  GetChatsRequestMessage,
  GetChatsResponseMessage,
  CreateChannelRequestMessage,
  CreateChannelResponseMessage,
  CreateUserRequestMessage,
  CreateUserResponseMessage,
  GetUsersRequestMessage,
  GetUsersResponseMessage,
  GetUserRequestMessage,
  GetUserResponseMessage,
  AuthenticateUserRequestMessage,
  AuthenticateUserResponseMessage,
  JoinChatChannelRequestMessage,
  JoinChatChannelResponseMessage,
  GetChatChannelsRequestMessage,
  GetChatChannelsResponseMessage,
  GetChatChannelRequestMessage,
  GetChatChannelResponseMessage,
  LogoutRequestMessage,
  LogoutResponseMessage,
  UserLogoutEventMessage,
  UserLoginEventMessage,
  ChannelLeaveEventMessage,
  ChannelJoinEventMessage,
  ChannelCreateEventMessage
>;

}

template <>
struct ebroschin::network::rpc::RpcCall<ebroschin::chatcore::api::AuthenticateUserRequestMessage> {
  using Response = chatcore::api::AuthenticateUserResponseMessage;
  using Error = chatcore::api::ErrorResponseMessage;
};

template <>
struct ebroschin::network::rpc::RpcCall<ebroschin::chatcore::api::JoinChatChannelRequestMessage> {
  using Response = chatcore::api::JoinChatChannelResponseMessage;
  using Error = chatcore::api::ErrorResponseMessage;
};

template <>
struct ebroschin::network::rpc::RpcCall<ebroschin::chatcore::api::GetChatsRequestMessage> {
  using Response = chatcore::api::GetChatsResponseMessage;
  using Error = chatcore::api::ErrorResponseMessage;
};

template <>
struct ebroschin::network::rpc::RpcCall<ebroschin::chatcore::api::GetChatChannelsRequestMessage> {
  using Response = chatcore::api::GetChatChannelsResponseMessage;
  using Error = chatcore::api::ErrorResponseMessage;
};

template <>
struct ebroschin::network::rpc::RpcCall<ebroschin::chatcore::api::GetChatChannelRequestMessage> {
  using Response = chatcore::api::GetChatChannelResponseMessage;
  using Error = chatcore::api::ErrorResponseMessage;
};

template <>
struct ebroschin::network::rpc::RpcCall<ebroschin::chatcore::api::CreateUserRequestMessage> {
  using Response = chatcore::api::CreateUserResponseMessage;
  using Error = chatcore::api::ErrorResponseMessage;
};

template <>
struct ebroschin::network::rpc::RpcCall<ebroschin::chatcore::api::CreateChannelRequestMessage> {
  using Response = chatcore::api::CreateChannelResponseMessage;
  using Error = chatcore::api::ErrorResponseMessage;
};

template <>
struct ebroschin::network::rpc::RpcCall<ebroschin::chatcore::api::GetUsersRequestMessage> {
  using Response = chatcore::api::GetUsersResponseMessage;
  using Error = chatcore::api::ErrorResponseMessage;
};

template <>
struct ebroschin::network::rpc::RpcCall<ebroschin::chatcore::api::GetUserRequestMessage> {
  using Response = chatcore::api::GetUserResponseMessage;
  using Error = chatcore::api::ErrorResponseMessage;
};

template <>
struct ebroschin::network::rpc::RpcCall<ebroschin::chatcore::api::LogoutRequestMessage> {
  using Response = chatcore::api::LogoutResponseMessage;
  using Error = chatcore::api::ErrorResponseMessage;
};