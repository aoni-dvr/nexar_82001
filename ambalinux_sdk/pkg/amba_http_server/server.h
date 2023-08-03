#pragma once

class Config;
class Session;
typedef std::shared_ptr<Session> SessionPtr;
class BufferPool;
typedef std::unique_ptr<BufferPool> BufferPoolUPtr;

class Server {
public:
  Server(Config const* config);
  ~Server();
  void start();
  void close();
  void run();
  asio::io_service& io_service();
  BufferPool* buffer_pool();
  void on_session_error(SessionPtr session);
  Config const* config() const;
private:
  void async_accept();
  SessionPtr create_session();
  void handle_accept(SessionPtr new_session, const asio::error_code& error);
  bool check_accept(SessionPtr new_session);
  void close_internal();
private:
  Config const* config_;
  asio::io_service io_service_;
  asio::ip::tcp::acceptor acceptor_;
  std::vector<SessionPtr> sessions_;
  std::atomic<bool> closed_;
  asio::error_code dummy_ec_;
  BufferPoolUPtr buffer_pool_;
private:
  Server(Server const&) = delete;
  Server& operator=(Server const&) = delete;
  Server(Server&&) = delete;
  Server& operator=(Server&&) = delete;
};
