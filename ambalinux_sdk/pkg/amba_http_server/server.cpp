#include "public.h"
#include "server.h"
#include "session.h"
#include "buffer.h"

Server::Server(Config const* config)
  : config_(config)
  , acceptor_(io_service_)
  , closed_(true) {
  buffer_pool_ = std::unique_ptr<BufferPool>(new BufferPool(
    config->max_packet_len(), config->max_packet_count())); 
  LOGD("Server::Server");
}

Server::~Server() {
  LOGD("Server::~Server");
  assert(closed_);
}

BufferPool* Server::buffer_pool() {
  return buffer_pool_.get();
}

asio::io_service& Server::io_service() {
  return io_service_;
}

void Server::start() {
  closed_ = false;
  tcp_endpoint endpoint(asio::ip::address_v4(0), config_->bind_port());
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(asio::socket_base::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();
  async_accept();
}

void Server::close() {
  if (!closed_) {
    io_service_.dispatch([this]() {
      close_internal();
    });
  }
}

void Server::close_internal() {
  if (!closed_) {
    closed_ = true;
    acceptor_.close(dummy_ec_);
    for (auto& session : sessions_) {
      session->close();
    }
    sessions_.clear();
  }
}

void Server::run() {
  io_service_.run(); // never return
  close_internal();
  io_service_.stop();
}

void Server::async_accept() {
  if (closed_)
    return;
  SessionPtr new_session = create_session();
  acceptor_.async_accept(new_session->socket(),
    [this, new_session](const asio::error_code& error) {
    handle_accept(new_session, error);
  });
}

SessionPtr Server::create_session() {
  return std::make_shared<Session>(this);
}

void Server::handle_accept(SessionPtr new_session,
  const asio::error_code& error) {
  if (closed_)
    return;
  if (error) {
    // let io.run exit
    LOGE("handle_accept failed: " << error.message());
    throw std::runtime_error(error.message());
  }

  if (!check_accept(new_session)) {
    LOGW("check_accept failed, current session count: " << sessions_.size());
    new_session->close();
  } else {
    sessions_.push_back(new_session);
    new_session->start();
  }
  async_accept();
}

bool Server::check_accept(SessionPtr /*new_session*/) {
  return (sessions_.size() < config_->max_connections());
}

void Server::on_session_error(SessionPtr session) {
  auto it = std::find(sessions_.begin(), sessions_.end(), session);
  if (it != sessions_.end()) {
    sessions_.erase(it);
  } else {
    assert(false);
  }
}

Config const* Server::config() const {
  return config_;
}