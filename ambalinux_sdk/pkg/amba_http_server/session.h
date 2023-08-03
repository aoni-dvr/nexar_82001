#pragma once

#include "buffer.h"
#include "misc.h"

class Session;
typedef std::shared_ptr<Session> SessionPtr;

class Server;

struct SendTick {
  std::chrono::steady_clock::time_point time;
  size_t length = 0;
};

struct Header {
  char_view field;
  char_view value;
};

struct ParseRequestResult {
  size_t parsed = 0;
  Header headers[100]; // 100 should enough
  size_t header_count = 0;
  bool header_complete = false;
  char_view url;
  char_view path;
  char_view query;
  bool last_was_value = false;
  bool msg_begin = false;
  bool msg_complete = false;
  char_view body;
};

class Session : public std::enable_shared_from_this<Session> {
public:
  Session(Server* server);
  ~Session();
  void start();
  void close();
public:
  tcp_socket& socket();
private:
  Config const* config() const;
  void async_read(BufferUPtr buffer);
  void read_request();
  bool parse_request(Buffer const* request, bool eof, http::Parser& parser,
    ParseRequestResult* result);
  void handle_request(BufferUPtr request, bool eof);
  void handle_full_request(BufferUPtr request, http::Parser const& parser,
    ParseRequestResult const& result);
  void handle_post_request(BufferUPtr request, http::Parser const& parser,
    ParseRequestResult const& result);
  void handle_get_request(BufferUPtr request, http::Parser const& parser,
    ParseRequestResult const& result);
  void handle_head_request(BufferUPtr request, http::Parser const& parser,
    ParseRequestResult const& result);
  bool get_range_header(ParseRequestResult const& result);
  void handle_get_file_request(std::string const& full_path);
  BufferUPtr build_response(char const* status, char const* content_type,
    char const* content, char const* extra_headers = nullptr);
  BufferUPtr build_get_file_response_header(std::string const& name);
  char const* get_file_type(std::string const& name);
  bool append_header_content_type(Buffer* buffer, char const* type);
  bool append_header_content_length(Buffer* buffer, size_t length);
  bool append_header_connection_close(Buffer* buffer);
  void response(BufferUPtr buffer);
  void response(BufferUPtr buffer,
    std::function<void(BufferUPtr)> const& on_send);
  void response_internal(BufferUPtr buffer,
    std::function<void(BufferUPtr)> const& on_send);
  void on_error();
  void on_finished();
  void async_send_part_file(FileUniquePtr file, BufferUPtr buffer,
    size_t have_read_length, size_t left_read_length);
  void async_send_multipart_head(FileUniquePtr file, BufferUPtr buffer);
  void async_send_multipart_body(FileUniquePtr file, BufferUPtr buffer,
    size_t have_read_length, size_t left_read_length);
  size_t get_ranges_length();
  size_t fread_log(void* ptr, size_t size, size_t nmemb, FILE* stream);
  void add_send_tick(size_t len);
  bool traffic_control();
  bool need_traffic_control();
  std::string get_get_request_full_path(ParseRequestResult const& result);
private:
  Server* server_;
  asio::io_service& io_service_;
  tcp_socket socket_;
  asio::steady_timer expired_timer_;
  asio::steady_timer tc_timer_;
  asio::error_code dummy_ec_;
  std::atomic<bool> closed_;
  bool from_local_;
  char const* file_type_;
  size_t file_length_;
  http_ranges_t file_ranges_;
  std::deque<SendTick> send_ticks_;
  const size_t max_speed_;
};
