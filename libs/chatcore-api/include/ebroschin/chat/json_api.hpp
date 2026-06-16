#pragma once

#include "api.hpp"

#define NLOHMANN_DEFINE_EMPTY_TYPE_NON_INTRUSIVE(Type) \
  inline void from_json(const nlohmann::json&, Type&) {} \
  inline void to_json(nlohmann::json& j, const Type&) { \
    j = nlohmann::json::object(); \
  }

namespace ebroschin::chatcore::api {

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PrintMessage, value);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ErrorResponseMessage, request_id, value);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ErrorMessage, value);
NLOHMANN_DEFINE_EMPTY_TYPE_NON_INTRUSIVE(ShutdownMessage);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ChatMessage, id, user_id, content);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ChatChannel, id, name);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(User, id, name);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ReceiveChatMessage, channel_id, user_id, content);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WriteChatMessage, content);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GetChatsRequestMessage, request_id, channel_id, limit);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GetChatsResponseMessage, request_id, channel_id, messages);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CreateChannelRequestMessage, request_id, name);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CreateChannelResponseMessage, request_id, channel);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CreateUserRequestMessage, request_id, name, password);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CreateUserResponseMessage, request_id, user);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GetUsersRequestMessage, request_id, user_ids);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GetUsersResponseMessage, request_id, users);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GetUserRequestMessage, request_id, name);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GetUserResponseMessage, request_id, user);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AuthenticateUserRequestMessage, request_id, name, password);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AuthenticateUserResponseMessage, request_id, user);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JoinChatChannelRequestMessage, request_id, channel_id);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JoinChatChannelResponseMessage, request_id, channel_id);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GetChatChannelsRequestMessage, request_id);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GetChatChannelsResponseMessage, request_id, channels);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GetChatChannelRequestMessage, request_id, channel_id);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GetChatChannelResponseMessage, request_id, channel);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LogoutRequestMessage, request_id);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LogoutResponseMessage, request_id);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UserLogoutEventMessage, user_id);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UserLoginEventMessage, user_id);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ChannelLeaveEventMessage, channel_id, user_id);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ChannelJoinEventMessage, channel_id, user_id);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ChannelCreateEventMessage, channel, user_id);

}