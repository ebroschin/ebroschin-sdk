#pragma once

#include "../commons.hpp"
#include "tcp_connection.hpp"
#include "tcp_message_processor.hpp"
#include "tcp_system_concepts.hpp"
#include "tcp_system_connector_facade.hpp"

#include <ebroschin/core/system.hpp>
#include <ebroschin/core/synchronization/executor.hpp>

#include <ranges>
#include <shared_mutex>
#include <tuple>
#include <unordered_map>

namespace ebroschin::network::tcp {

template<NetworkConnector TConnector,
  typename TCodec,
  typename TMessageHandler,
  NetworkMessages... TMessages>
requires NetworkCodec<TCodec, TMessages...>
  && NetworkMessageHandler<TMessageHandler, TMessages...>
class TcpSystem final : public core::System {
public:
  using MessageProcessor = TcpMessageProcessor<TCodec, TMessageHandler, TMessages...>;
  using MessageHandler = TMessageHandler;
  using MessageTypes = std::tuple<TMessages...>;
  using Connector = TConnector;
  using ConnectionEventHandler = TcpConnectionEventHandler<typename Connector::Parameters>;

  explicit TcpSystem(const core::SystemContext& ctx, core::Executor& executor):
    System{ctx},
    processor_{executor}
  {}

  void Initialize() override {
    using BaseConnector = TcpConnector<typename Connector::Parameters, typename Connector::Connection>;
    auto facade = TcpSystemConnectorFacadeBase<BaseConnector>::Create(this);
    connector_.Initialize(std::move(facade));
  }

  [[nodiscard]] MessageHandler& GetMessageHandler() noexcept
  { return processor_.GetMessageHandler(); }

  void Connect(Connector::Parameters parameters, ConnectionEventHandler* connection_event_handler = nullptr) {
    connector_.Connect(std::move(parameters), connection_event_handler);
  }

  void Disconnect(ConnectionId connection_id) {
    std::shared_ptr<TcpConnection> connection{};
    {
      std::scoped_lock lock{connection_mutex_};
      const auto it = connections_.find(connection_id);
      if (it == connections_.end()) return;

      connection = it->second;
    }

    connection->Disconnect();
  }

  template<typename TMessage>
  requires IsValidMessage<TMessage, TMessages...>
  void Send(ConnectionId id, const TMessage& message) {
    const auto bytes = TCodec::template Encode<TMessage>(message);
    std::shared_ptr<TcpConnection> connection{};
    {
      std::shared_lock lock{connection_mutex_};
      const auto it = connections_.find(id);
      if (it == connections_.end()) return;

      connection = it->second;
    }

    connection->SendBytes(bytes);
  }

  template<typename TMessage>
  requires IsValidMessage<TMessage, TMessages...>
  void Broadcast(const TMessage& message) {
    const auto bytes = TCodec::template Encode<TMessage>(message);
    for (const auto& connection : GetConnections(nullptr)) {
      connection->SendBytes(bytes);
    }
  }

  template<typename TMessage, typename TRange>
  requires IsValidMessage<TMessage, TMessages...>
    && std::ranges::range<TRange>
  void Broadcast(TRange&& range, const TMessage& message) {
    const auto bytes = TCodec::template Encode<TMessage>(message);
    for (const auto& connection : GetConnections(std::forward<TRange>(range))) {
      connection->SendBytes(bytes);
    }
  }

private:
  template<typename TRange>
  std::vector<std::shared_ptr<TcpConnection>> GetConnections(TRange&& range) {
    std::vector<std::shared_ptr<TcpConnection>> buffer{};
    {
      std::shared_lock lock{connection_mutex_};

      //statically avoid filtering connections if no filter is applied
      if constexpr (std::is_same_v<std::remove_cvref_t<TRange>, std::nullptr_t>) {
        buffer.reserve(connections_.size());

        for (const auto &connection : connections_ | std::views::values) {
          buffer.emplace_back(connection);
        }
      } else {
        if constexpr (std::ranges::sized_range<TRange>) {
          buffer.reserve(std::ranges::size(range));
        }

        for (const auto& connection_id : range) {
          const auto it = connections_.find(connection_id);
          if (it == connections_.end()) continue;

          buffer.emplace_back(it->second);
        }
      }
    }

    return buffer;
  }

  void CreateConnection(std::shared_ptr<typename Connector::Connection> connection, ConnectionEventHandler* connection_event_handler) {
    const auto connection_id = next_connection_id_.fetch_add(1, std::memory_order_relaxed);
    {
      std::scoped_lock lock{connection_mutex_};
      connections_.emplace(connection_id, connection);
    }

    auto facade = TcpSystemConnectionFacadeBase::Create(this, connection_id, connection_event_handler);
    connection->Initialize(std::move(facade));

    if (!connection_event_handler) return;
    connection_event_handler->OnConnected(connection_id);
  }

  void RemoveConnection(ConnectionId id) {
    std::scoped_lock lock{connection_mutex_};
    connections_.erase(id);
  }

  void ReceiveMessage(ConnectionId id, std::vector<std::byte> bytes) {
    processor_.Process(id, std::move(bytes));
  }

  MessageProcessor processor_;
  Connector connector_{};

  std::shared_mutex connection_mutex_{};
  std::unordered_map<ConnectionId, std::shared_ptr<TcpConnection>> connections_{};
  std::atomic<ConnectionId> next_connection_id_{1};

  template<class>
  friend class TcpSystemConnectionFacade;

  template<class, class>
  friend class TcpSystemConnectorFacade;
};

}