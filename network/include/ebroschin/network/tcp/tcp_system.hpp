#pragma once

#include "../commons.hpp"
#include "../network_event_handler.hpp"
#include "tcp_connection.hpp"
#include "tcp_message_processor.hpp"
#include "tcp_system_callbacks.hpp"
#include "tcp_system_concepts.hpp"
#include "tcp_system_network_events.hpp"

#include <ebroschin/core/synchronization/executor.hpp>
#include <ebroschin/core/system.hpp>

#include <ranges>
#include <shared_mutex>
#include <tuple>
#include <unordered_map>

namespace ebroschin::network::tcp {

template<NetworkConnector TConnector,
  typename TCodec,
  NetworkMessages... TMessages>
requires NetworkCodec<TCodec, TMessages...>
class TcpSystem final : public core::System {
public:
  using MessageTypes = std::tuple<TMessages...>;
  using Connector = TConnector;
  using EventHandler = TcpNetworkEvents<TcpSystem>::EventHandlerType;
  using MessageProcessor = TcpMessageProcessor<TCodec, EventHandler, TMessages...>;

  explicit TcpSystem(const core::SystemContext& ctx, core::Executor& executor):
    System{ctx},
    executor_{executor}
  {}

  void Initialize() override {
    connector_callbacks_ = {
      .on_connection_created = [this]
        (std::shared_ptr<TcpConnection> connection, ConnectCallback callback)
      {
        CreateConnection(std::move(connection), std::move(callback));
      },
      .on_connection_failed = [this]
        (std::string error, ConnectCallback callback)
      {
        Emit(NetworkEvent<ConnectionFailed>{std::nullopt, {error}});
        CompleteConnect(std::move(callback), ConnectionResult{.error = std::move(error)});
      }
    };

    connection_callbacks_ = {
      .on_receive = [this]
        (ConnectionId connection_id, std::vector<std::byte> bytes)
      {
        ReceiveMessage(connection_id, std::move(bytes));
      },
      .on_disconnect = [this]
        (ConnectionId connection_id)
      {
        RemoveConnection(connection_id);
      }
    };

    connector_.Initialize(&connector_callbacks_);
  }

  void Connect(Connector::Parameters parameters, ConnectCallback callback = {}) {
    connector_.Connect(std::move(parameters), std::move(callback));
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

  template <typename TEvent>
  [[nodiscard]] utility::SignalSubscription Subscribe(EventHandler::template Slot<TEvent> slot) {
    return event_handler_.template Subscribe<TEvent>(std::move(slot));
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

  void CreateConnection(std::shared_ptr<TcpConnection> connection, ConnectCallback callback) {
    const auto connection_id = next_connection_id_.fetch_add(1, std::memory_order_relaxed);
    {
      std::scoped_lock lock{connection_mutex_};
      connections_.emplace(connection_id, connection);
    }

    connection->Initialize(connection_id, &connection_callbacks_);
    Emit(NetworkEvent<ConnectionCreated>{connection_id, {}});
    CompleteConnect(std::move(callback), ConnectionResult{.connection_id = connection_id});
  }

  void RemoveConnection(ConnectionId connection_id) {
    {
      std::scoped_lock lock{connection_mutex_};
      if (connections_.erase(connection_id) == 0) return;
    }

    Emit(NetworkEvent<ConnectionRemoved>{connection_id, {}});
  }

  void ReceiveMessage(ConnectionId id, std::vector<std::byte> bytes) {
    processor_.Process(id, std::move(bytes));
  }

  void CompleteConnect(ConnectCallback callback, ConnectionResult result) {
    if (!callback) return;

    executor_.Post([callback = std::move(callback), result = std::move(result)] {
      callback(result);
    });
  }

  template <typename TEvent>
  void Emit(NetworkEvent<TEvent> event) {
    executor_.Post([this, event = std::move(event)] {
      event_handler_.Emit(event);
    });
  }

  core::Executor& executor_;
  EventHandler event_handler_{};
  MessageProcessor processor_{event_handler_, executor_};
  Connector connector_{};

  std::shared_mutex connection_mutex_{};
  std::unordered_map<ConnectionId, std::shared_ptr<TcpConnection>> connections_{};
  std::atomic<ConnectionId> next_connection_id_{1};

  ConnectorCallbacks connector_callbacks_{};
  ConnectionCallbacks connection_callbacks_{};
};

}