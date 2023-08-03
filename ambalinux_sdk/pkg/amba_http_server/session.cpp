#include <unistd.h>
#include "public.h"
#include "session.h"
#include "server.h"
#include "buffer.h"
#include <iomanip>
#include <iostream>
using namespace std;

namespace http_mime_type {
  const char any[] = "*/*";
  const char text[] = "text/html";
  const char jpg[] = "image/*";
  const char mp4[] = "video/mp4";
}

namespace misc_strings {
  const char crlf[] = "\r\n";
  const char content_length[] = "Content-Length: ";
  const char content_type[] = "Content-Type: ";
  const char connection_close[] = "Connection: close\r\n";
  const char boundary[] = "360_1234567890";
  const char range_content_type[] =
    "multipart/byteranges;boundary=360_1234567890";
  const char content_range[] = "Content-Range: ";
} // namespace misc_strings

namespace http_status {
  char const* ok =
    "HTTP/1.1 200 OK\r\n";
  char const* created =
    "HTTP/1.1 201 Created\r\n";
  char const* accepted =
    "HTTP/1.1 202 Accepted\r\n";
  char const* no_content =
    "HTTP/1.1 204 No Content\r\n";
  char const* partial_content =
    "HTTP/1.1 206 Partial content\r\n";
  char const* multiple_choices =
    "HTTP/1.1 300 Multiple Choices\r\n";
  char const* moved_permanently =
    "HTTP/1.1 301 Moved Permanently\r\n";
  char const* moved_temporarily =
    "HTTP/1.1 302 Moved Temporarily\r\n";
  char const* not_modified =
    "HTTP/1.1 304 Not Modified\r\n";
  char const* bad_request =
    "HTTP/1.1 400 Bad Request\r\n";
  char const* unauthorized =
    "HTTP/1.1 401 Unauthorized\r\n";
  char const* forbidden =
    "HTTP/1.1 403 Forbidden\r\n";
  char const* not_found =
    "HTTP/1.1 404 Not Found\r\n";
  char const* range_not_satisfiable =
    "HTTP/1.1 416 Requested Range Not Satisfiable\r\n";
  char const* internal_server_error =
    "HTTP/1.1 500 Internal Server Error\r\n";
  char const* not_implemented =
    "HTTP/1.1 501 Not Implemented\r\n";
  char const* bad_gateway =
    "HTTP/1.1 502 Bad Gateway\r\n";
  char const* service_unavailable =
    "HTTP/1.1 503 Service Unavailable\r\n";
}

namespace http_content {
  const char ok[] = "";
  const char created[] =
    "<html>"
    "<head><title>Created</title></head>"
    "<body><h1>201 Created</h1></body>"
    "</html>";
  const char accepted[] =
    "<html>"
    "<head><title>Accepted</title></head>"
    "<body><h1>202 Accepted</h1></body>"
    "</html>";
  const char no_content[] =
    "<html>"
    "<head><title>No Content</title></head>"
    "<body><h1>204 Content</h1></body>"
    "</html>";
  const char multiple_choices[] =
    "<html>"
    "<head><title>Multiple Choices</title></head>"
    "<body><h1>300 Multiple Choices</h1></body>"
    "</html>";
  const char moved_permanently[] =
    "<html>"
    "<head><title>Moved Permanently</title></head>"
    "<body><h1>301 Moved Permanently</h1></body>"
    "</html>";
  const char moved_temporarily[] =
    "<html>"
    "<head><title>Moved Temporarily</title></head>"
    "<body><h1>302 Moved Temporarily</h1></body>"
    "</html>";
  const char not_modified[] =
    "<html>"
    "<head><title>Not Modified</title></head>"
    "<body><h1>304 Not Modified</h1></body>"
    "</html>";
  const char bad_request[] =
    "<html>"
    "<head><title>Bad Request</title></head>"
    "<body><h1>400 Bad Request</h1></body>"
    "</html>";
  const char unauthorized[] =
    "<html>"
    "<head><title>Unauthorized</title></head>"
    "<body><h1>401 Unauthorized</h1></body>"
    "</html>";
  const char forbidden[] =
    "<html>"
    "<head><title>Forbidden</title></head>"
    "<body><h1>403 Forbidden</h1></body>"
    "</html>";
  const char not_found[] =
    "<html>"
    "<head><title>Not Found</title></head>"
    "<body><h1>404 Not Found</h1></body>"
    "</html>";
  const char range_not_satisfiable[] =
    "<html>"
    "<head><title>Requested Range Not Satisfiable</title></head>"
    "<body><h1>416 Requested Range Not Satisfiable</h1></body>"
    "</html>";
  const char internal_server_error[] =
    "<html>"
    "<head><title>Internal Server Error</title></head>"
    "<body><h1>500 Internal Server Error</h1></body>"
    "</html>";
  const char not_implemented[] =
    "<html>"
    "<head><title>Not Implemented</title></head>"
    "<body><h1>501 Not Implemented</h1></body>"
    "</html>";
  const char bad_gateway[] =
    "<html>"
    "<head><title>Bad Gateway</title></head>"
    "<body><h1>502 Bad Gateway</h1></body>"
    "</html>";
  const char service_unavailable[] =
    "<html>"
    "<head><title>Service Unavailable</title></head>"
    "<body><h1>503 Service Unavailable</h1></body>"
    "</html>";
}

Session::Session(Server* server)
  : server_(server)
  , io_service_(server->io_service())
  , socket_(io_service_)
  , expired_timer_(io_service_)
  , tc_timer_(io_service_)
  , closed_(true)
  , from_local_(false)
  , file_type_(http_mime_type::any)
  , file_length_(0)
  , max_speed_(config()->max_speed()) {
}

Session::~Session() {
  assert(closed_);
  LOGD("");
}

bool Session::need_traffic_control() {
  return max_speed_ < DEFAULT_MAX_SPEED;
}

void Session::start() {
  closed_ = false;

  auto remote = socket_.remote_endpoint(dummy_ec_);
  assert(!dummy_ec_);
  auto local = socket_.local_endpoint(dummy_ec_);
  assert(!dummy_ec_);

  from_local_ = remote.address().is_loopback() &&
    local.address().is_loopback();

  LOGD("new session: " << remote.address().to_string() << ":" << remote.port());

  read_request();
}

// only call by Server
void Session::close() {
  if (!closed_) {
    closed_ = true;
    expired_timer_.cancel();
    tc_timer_.cancel();
    socket_.close();
  }
}

tcp_socket& Session::socket() {
  return socket_;
}

void Session::async_read(BufferUPtr buffer) {
  if (closed_)
    return;

  auto self = shared_from_this();
  auto asio_buffer = buffer->asio_read_buffer();
  auto wrapped_buffer = wrap_move(std::move(buffer));

  socket_.async_read_some(asio_buffer,
    [self, this, wrapped_buffer](const asio::error_code& error,
      size_t bytes_transferred) {
    if (closed_)
      return;
    auto buffer = std::move(*wrapped_buffer);
    if (!bytes_transferred && error.value() == asio::error::eof) {
      handle_request(std::move(buffer), true);
    } else if (error || !bytes_transferred) {
      LOGW("async_read: " << error.message());
      on_error();
    } else {
      buffer->length_ += bytes_transferred;
      handle_request(std::move(buffer), false);
    }
  });
}

void Session::read_request() {
  auto self = shared_from_this();
  expired_timer_.expires_from_now(std::chrono::seconds(30));
  expired_timer_.async_wait([self, this](const asio::error_code& error) {
    if (!error) {
      LOGW("too long time to recv a full request data");
      on_error();
    }
  });

  auto buffer = server_->buffer_pool()->allocate();
  if (buffer) {
    async_read(std::move(buffer));
  } else {
    // out of memory
    LOGW("out of memory");
    on_error();
  }
}

bool Session::parse_request(Buffer const* request, bool eof,
  http::Parser& parser, ParseRequestResult* result) {
  http::ParserSettings settings;

  settings.on_headers_complete = [result]() {
    result->header_complete = true;
    return 0;
  };

  settings.on_header_field = [result](
    const char *at, size_t length) {
    if (result->last_was_value) {
      if (result->header_count >= 100) {
        LOGW("too many headers");
        return 1;
      }
      Header* header = result->headers + result->header_count;
      header->field.at = at;
      header->field.len = length;
      ++result->header_count;
    } else {
      if (!result->header_count) {
        ++result->header_count;
      }
      Header* header = result->headers + result->header_count - 1;
      header->field.at = at;
      header->field.len += length;
    }
    result->last_was_value = false;
    return 0;
  };

  settings.on_header_value = [result](
    const char *at, size_t length) {
    if (!result->last_was_value) {
      if (!result->header_count) {
        ++result->header_count;
      }
      Header* header = result->headers + result->header_count - 1;
      header->value.at = at;
      header->value.len = length;
    } else {
      assert(!result->header_count);
      Header* header = result->headers + result->header_count - 1;
      header->value.at = at;
      header->value.len += length;
    }
    result->last_was_value = true;
    return 0;
  };

  settings.on_url = [result](const char*at, size_t length) {
    if (!result->url.at) {
      result->url.at = at;
    }
    result->url.len += length;
    return 0;
  };

  settings.on_message_begin = [result]() {
    result->msg_begin = true;
    return 0;
  };

  settings.on_message_complete = [result]() {
    result->msg_complete = true;
    return 0;
  };

  settings.on_body = [result](const char *at, size_t length) {
    if (!result->body.at) {
      result->body.at = at;
    }
    result->body.len += length;
    return 0;
  };

  result->parsed = parser.execute(&settings, (char const*)request->data_,
    request->length_);

  if (parser.http_errno) {
    LOGW("invalid http request: " << parser.http_errno);
    return false;
  }

  if (parser.upgrade) {
    // not support upgrade to websocket or http2.0
    LOGW("not support http upgrade");
    return false;
  }

  if (result->parsed != request->length_) {
    LOGW("invalid http request");
    return false;
  }

  if (result->header_complete) {
    // only support http 1.0, 1.1
    if (parser.http_major != 1 || (parser.http_minor != 0 &&
      parser.http_minor != 1)) {
      LOGW("invalid http version: " << parser.http_major << "." <<
        parser.http_minor);
      return false;
    }

    // parse url
    http::http_parser_url u;
    if (http::Parser::parse_url(result->url.at, result->url.len, 0, &u)) {
      LOGW("url invalid: " << result);
      on_error();
      return false;
    }
    char const* at = result->url.at;
    if (u.field_set & (1 << http::UF_PATH)) {
      result->path.at = at + u.field_data[http::UF_PATH].off;
      result->path.len = u.field_data[http::UF_PATH].len;
    }
    if (u.field_set & (1 << http::UF_QUERY)) {
      result->query.at = at + u.field_data[http::UF_QUERY].off;
      result->query.len = u.field_data[http::UF_QUERY].len;
    }

    if (parser.method != http::HTTP_GET && parser.method != http::HTTP_POST &&
      parser.method != http::HTTP_HEAD) {
      LOGW("not support method: " << parser.method);
      return false;
    }

    // filter invalid path
    if (has_str(result->path, "../", sizeof("../") - 1)) {
      LOGW("http path can not have ../");
      return false;
    }

    if (!from_local_) {
      // TODO: check auth header
    }
  }

  if (eof && !result->msg_complete) {
    LOGW("request not complete");
    return false;
  }

  if (!result->msg_complete && request->is_full()) {
    LOGW("request too large");
    return false;
  }

  return true;
}

void Session::handle_request(BufferUPtr request, bool eof) {
  if (!request->length_) {
    LOGW("parse request failed: request->length_ is 0");
    on_error();
    return;
  }

  http::Parser parser(http::HTTP_REQUEST);
  ParseRequestResult result;
  if (!parse_request(request.get(), eof, parser, &result)) {
    LOGW("parse request failed");
    on_error();
    return;
  }

  if (!result.msg_complete) {
    // keep recv
    LOGI("request not complete, keep recv");
    return async_read(std::move(request));
  }

  LOGI("http request info:");
  LOGI("url: " << result.url);
  for (size_t i = 0; i < result.header_count; ++i) {
    auto const& header = result.headers[i];
    LOGI("header: " << header.field << ":" << header.value);
  }

  handle_full_request(std::move(request), parser, result);
}

void Session::handle_full_request(BufferUPtr request,
  http::Parser const& parser, ParseRequestResult const& result) {
  // now we get a full request
  LOGD("");
  expired_timer_.cancel();

  if (parser.method == http::HTTP_GET) {
    handle_get_request(std::move(request), parser, result);
  } else if (parser.method == http::HTTP_POST) {
    handle_post_request(std::move(request), parser, result);
  } else if (parser.method == http::HTTP_HEAD) {
    handle_head_request(std::move(request), parser, result);
  } else {
    // since we have filter the parser.method in
    // Session::parse_request()
    LOGF("never happen");
    throw std::logic_error("never happen");
  }
}

void Session::handle_post_request(BufferUPtr /*request*/,
  http::Parser const& /*parser*/, ParseRequestResult const& /*result*/) {
  // TODO: should be set password post request from localhost.
  return response(build_response(http_status::not_implemented,
    http_mime_type::text, http_content::not_implemented));
}

std::string Session::get_get_request_full_path(
  ParseRequestResult const& result) {
  char const kCommand[] = "/__command__";
  size_t const kCommandLen = sizeof(kCommand) - 1;
  char const kCommandLog[] = "log";
  size_t const kCommandLogLen = sizeof(kCommandLog) - 1;

  if (same_str(result.path, kCommand, kCommandLen)) {
    // http://192.168.1.1/__command__
    if (same_str(result.query, kCommandLog, kCommandLogLen)) {
      // http://192.168.1.1/__command__?log
      return get_log_pathname();
    }
    return "";
  }

  std::string full_path = config()->root_dir();
  full_path.append(result.path.at, result.path.len);
  return full_path;
}


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
void Session::handle_get_request(BufferUPtr /*request*/,
  http::Parser const& /*parser*/, ParseRequestResult const& result) {
  struct stat st;
  std::string full_path = get_get_request_full_path(result);

  if (full_path.empty()) {
    LOGW("not support the request command" << result.url);
    return response(build_response(http_status::not_implemented,
      http_mime_type::text, http_content::not_implemented));
  }
  //PrintLog("Step 2[%04d]%s\n", __LINE__, full_path.c_str());
  //AmbaPrint("Step 2[%04d]%s\n", __LINE__, full_path.c_str());
  //cout << "full_path: " << full_path << endl;
  if (stat(full_path.c_str(), &st)) {
    LOGW("can not find: " << full_path << ", error" << strerror(errno));
    return response(build_response(http_status::not_found,
      http_mime_type::text, http_content::not_found));
  }

  if (st.st_mode & S_IFDIR) {
    LOGW("do not support dir list");
    return response(build_response(http_status::not_implemented,
      http_mime_type::text, http_content::not_implemented));
  } else {
    file_length_ = st.st_size;
    if (!get_range_header(result)) {
      LOGW("range header invalid");
      return response(build_response(http_status::bad_request,
        http_mime_type::text, http_content::bad_request));
    }
    if (!uniform_ranges(file_ranges_, file_length_)) {
      char extra_header[64];
      sprintf(extra_header, "%s bytes */%u", misc_strings::content_range,
        (uint32_t)file_length_);
      LOGW("range not satisfiable: " << extra_header);
      return response(build_response(http_status::range_not_satisfiable,
        http_mime_type::text, http_content::range_not_satisfiable,
        extra_header));
    }
    return handle_get_file_request(full_path);
  }
}

bool Session::get_range_header(ParseRequestResult const& result) {
  static char const key[] = "range";
  static size_t const keylen = sizeof(key) - 1;
  auto end = result.headers + result.header_count;
  auto it = std::find_if(result.headers, end, [](Header const& header) {
    return same_str_nocase(header.field, key, keylen);
  });

  if (it != end) {
    return parse_ranges_header(it->value, file_ranges_);
  } else {
    return true;
  }
}

// 14.16 Content-Range
void Session::handle_head_request(BufferUPtr /*request*/,
  http::Parser const& /*parser*/, ParseRequestResult const& result) {
  std::string full_path = config()->root_dir();
  full_path.append(result.path.at, result.path.len);
  struct stat st;


  //PrintLog("Step 2[%04d]%s\n", __LINE__, full_path.c_str());
  //AmbaPrint("Step 2[%04d]%s\n", __LINE__, full_path.c_str());
  //cout << "full_path: " << full_path << "fmt: " << result.query.at << endl;
  if (stat(full_path.c_str(), &st)) {
    LOGW("can not find: " << full_path << ", error" << strerror(errno));
    return response(build_response(http_status::not_found,
      http_mime_type::text, http_content::not_found));
  }

  if (st.st_mode & S_IFDIR) {
    LOGW("do not support dir list");
    return response(build_response(http_status::not_implemented,
      http_mime_type::text, http_content::not_implemented));
  } else {
    file_length_ = st.st_size;
    if (!get_range_header(result)) {
      LOGW("range header invalid");
      return response(build_response(http_status::bad_request,
        http_mime_type::text, http_content::bad_request));
    }
    if (!uniform_ranges(file_ranges_, file_length_)) {
      char extra_header[64];
      sprintf(extra_header, "%s bytes */%u", misc_strings::content_range,
        (uint32_t)file_length_);
      LOGW("range not satisfiable: " << extra_header);
      return response(build_response(http_status::range_not_satisfiable,
        http_mime_type::text, http_content::range_not_satisfiable,
        extra_header));
    }
    return response(build_get_file_response_header(full_path));
  }
}



char const* Session::get_file_type(std::string const& name) {
  // TODO: parse the suffix of the file name
  if (name.size() < 4)
    return http_mime_type::any;
  char_view name_view;
  name_view.at = name.c_str() + name.size() - 4;
  name_view.len = 4;
  if (same_str_nocase(name_view, ".mp4", sizeof(".mp4") - 1))
    return http_mime_type::mp4;
  if (same_str_nocase(name_view, ".jpg", sizeof(".jpg") - 1))
    return http_mime_type::jpg;
  if (same_str_nocase(name_view, ".log", sizeof(".log") - 1))
    return http_mime_type::text;
  return http_mime_type::any;
}

BufferUPtr Session::build_get_file_response_header(std::string const& name) {
  BufferUPtr buffer = server_->buffer_pool()->allocate();
  BufferUPtr empty;
  if (!buffer) {
    LOGW("out of memory");
    return buffer;
  }

  const char* status = file_ranges_.empty() ? http_status::ok :
    http_status::partial_content;
  if (!buffer->append(status)) {
    LOGW("buffer full, impossible");
    return empty;
  }

  if (!append_header_connection_close(buffer.get())) {
    LOGW("buffer full, impossible");
    return empty;
  }

  file_type_ = get_file_type(name);

  if (file_ranges_.size() > 1) {
    // rfc2626 19.2: if has multiple range, must use multipart/byteranges
    if (!append_header_content_type(buffer.get(),
      misc_strings::range_content_type)) {
      LOGW("buffer full, impossible");
      return empty;
    }
    // NOTE: use file_type_ here
    size_t content_length = get_ranges_length();
    if (!append_header_content_length(buffer.get(), content_length)) {
      LOGW("buffer full, impossible");
      return empty;
    }
  } else {

    if (!append_header_content_type(buffer.get(), file_type_)) {
      LOGW("buffer full, impossible");
      return empty;
    }

    size_t content_length = file_ranges_.empty() ? file_length_ :
      get_range_length(file_ranges_[0]);
    if (!append_header_content_length(buffer.get(), content_length)) {
      LOGW("buffer full, impossible");
      return empty;
    }

    if (file_ranges_.size() == 1) {
      char header[64];
      auto const& range = file_ranges_[0];
      sprintf(header, "%s bytes %u-%u/%u\r\n", misc_strings::content_range,
        range.first, range.second, (uint32_t)file_length_);
      if (!buffer->append(header)) {
        LOGW("buffer full, impossible");
        return empty;
      }
    }
  }

  if (!buffer->append(misc_strings::crlf)) {
    LOGW("buffer full, impossible");
    return empty;
  }

  return buffer;
}

void Session::handle_get_file_request(std::string const& full_path) {
  auto buffer = build_get_file_response_header(full_path);
  if (!buffer) {
    return on_error();
  }

  if (!file_length_) {
    return response(std::move(buffer));
  }

  FileUniquePtr file(fopen(full_path.c_str(), "rb"), fclose);
  if (!file) {
    LOGW("file not found: " << full_path);
    return response(build_response(http_status::not_found,
      http_mime_type::text, http_content::not_found));
  }

  if (file_ranges_.empty() || file_ranges_.size() == 1) {
    size_t read_length = file_length_;
    if (file_ranges_.size() == 1) {
      size_t read_offset = file_ranges_[0].first;
      if (read_offset) {
        if (0 != fseek(file.get(), read_offset, SEEK_SET)) {
          LOGW("fseek failed");
          return on_error();
        }
      }
      read_length = get_range_length(file_ranges_[0]);
    }
    async_send_part_file(std::move(file), std::move(buffer), 0,
      read_length);
  } else {
    std::reverse(file_ranges_.begin(), file_ranges_.end());
    async_send_multipart_head(std::move(file), std::move(buffer));
  }
}

void Session::async_send_multipart_head(FileUniquePtr file,
  BufferUPtr buffer) {
  if (file_ranges_.empty()) {
    // the last boundary: \r\n + "--" + boundary + "--" + \r\n
    buffer->append("\r\n--");
    buffer->append(misc_strings::boundary);
    buffer->append("--\r\n");
    return response(std::move(buffer)); // last buffer
  }

  http_range_t const& range = file_ranges_.back();
  // \r\n + "--" + boundary + \r\n
  buffer->append("\r\n--");
  buffer->append(misc_strings::boundary);
  buffer->append("\r\n");
  // Content-Type: application/octet-stream\r\n
  buffer->append(misc_strings::content_type);
  buffer->append(file_type_);
  buffer->append("\r\n");
  // Content-Range: bytes 1-1/1430256\r\n
  buffer->append(misc_strings::content_range);
  buffer->append("bytes ");
  char range_header[64];
  sprintf(range_header, "%u-%u/%u", range.first, range.second,
    (uint32_t)file_length_);
  buffer->append(range_header);
  buffer->append("\r\n");
  // last \r\n
  buffer->append("\r\n");

  auto self = shared_from_this();
  auto wrapped_file = wrap_move(std::move(file));
  response(std::move(buffer), [self, this, wrapped_file](BufferUPtr buffer) {
    auto file = std::move(*wrapped_file);
    buffer->reset(); // reuse the buffer
    auto const& range = file_ranges_.back();
    size_t read_length = get_range_length(range);
    size_t read_offset = range.first;
    if (0 != fseek(file.get(), read_offset, SEEK_SET)) {
      LOGW("fseek failed");
      return on_error();
    }
    async_send_multipart_body(std::move(file), std::move(buffer), 0,
      read_length);
  });
}

void Session::async_send_multipart_body(FileUniquePtr file, BufferUPtr buffer,
  size_t have_read_length, size_t left_read_length) {
  if (!left_read_length) {
    assert(false);
    LOGW("oops, left_read_length: " << left_read_length);
    return on_error();
  }

  assert(!file_ranges_.empty());
  http_range_t const& range = file_ranges_.back();

  auto f = file.get();
  uint8_t* at = buffer->data_ + buffer->length_;
  size_t left_capacity = buffer->left_capacity();

  size_t read_length = std::min<size_t>(left_capacity, left_read_length);
  size_t n = fread_log(at, 1, read_length, f);
  if (ferror(f)) {
    LOGW("fread failed");
    return on_error();
  }

  buffer->length_ += n;
  left_read_length -= n;
  have_read_length += n;

  auto self = shared_from_this();
  if (feof(f) || n < read_length || !left_read_length) {
    assert(have_read_length == get_range_length(range));
    auto wrapped_file = wrap_move(std::move(file));
    response(std::move(buffer), [self, this, wrapped_file](BufferUPtr buffer) {
      file_ranges_.pop_back();
      auto file = std::move(*wrapped_file);
      buffer->reset();
      async_send_multipart_head(std::move(file), std::move(buffer));
    });
  } else {
    auto wrapped_file = wrap_move(std::move(file));
    response(std::move(buffer), [self, this, wrapped_file, have_read_length,
      left_read_length](BufferUPtr buffer) {
      auto file = std::move(*wrapped_file);
      buffer->reset(); // reuse the buffer
      async_send_multipart_body(std::move(file), std::move(buffer),
        have_read_length, left_read_length);
    });
  }
}

void Session::async_send_part_file(FileUniquePtr file, BufferUPtr buffer,
  size_t have_read_length, size_t left_read_length) {
  if (!left_read_length) {
    assert(false);
    LOGW("oops, left_read_length: " << left_read_length);
    return on_error();
  }

  auto f = file.get();
  uint8_t* at = buffer->data_ + buffer->length_;
  size_t left_capacity = buffer->left_capacity();

  size_t read_length = std::min<size_t>(left_capacity, left_read_length);
  size_t n = fread_log(at, 1, read_length, f);
  if (ferror(f)) {
    LOGW("fread failed");
    return on_error();
  }

  buffer->length_ += n;
  left_read_length -= n;
  have_read_length += n;

  if (feof(f) || n < read_length || !left_read_length) {
    return response(std::move(buffer));
  } else {
    auto self = shared_from_this();
    auto wrapped_file = wrap_move(std::move(file));
    response(std::move(buffer), [self, this, wrapped_file, have_read_length,
      left_read_length](BufferUPtr buffer) {
      auto file = std::move(*wrapped_file);
      buffer->reset(); // reuse the buffer
      async_send_part_file(std::move(file), std::move(buffer),
        have_read_length, left_read_length);
    });
  }
}

bool Session::append_header_content_type(Buffer* buffer, char const* type) {
  return buffer->append(misc_strings::content_type) &&
    buffer->append(type) &&
    buffer->append(misc_strings::crlf);
}

bool Session::append_header_content_length(Buffer* buffer, size_t length) {
  char content_length_buffer[16];
  sprintf(content_length_buffer, "%u", (uint32_t)length);

  return buffer->append(misc_strings::content_length) &&
    buffer->append(content_length_buffer) &&
    buffer->append(misc_strings::crlf);
}

bool Session::append_header_connection_close(Buffer* buffer) {
  return buffer->append(misc_strings::connection_close);
}

// if extra_headers is not null, must includes misc_strings::crlf
BufferUPtr Session::build_response(char const* status,
  char const* content_type, char const* content,
  char const* extra_headers) {
  BufferUPtr buffer = server_->buffer_pool()->allocate();
  BufferUPtr empty;

  if (buffer) {
    if (!buffer->append(status)) {
      LOGW("buffer full, impossible");
      return empty;
    }

    if (!append_header_connection_close(buffer.get())) {
      LOGW("buffer full, impossible");
      return empty;
    }

    if (!append_header_content_type(buffer.get(), content_type)) {
      LOGW("buffer full, impossible");
      return empty;
    }
    if (!append_header_content_length(buffer.get(), strlen(content))) {
      LOGW("buffer full, impossible");
      return empty;
    }
    if (extra_headers) {
      buffer->append(extra_headers);
    }

    if (!buffer->append(misc_strings::crlf)) {
      LOGW("buffer full, impossible");
      return empty;
    }
    if (!buffer->append(content)) {
      LOGW("buffer full, impossible");
      return empty;
    }
  } else {
    LOGW("out of memory");
  }

  return buffer;
}

void Session::response(BufferUPtr buffer) {
  auto self = shared_from_this();
  if (!buffer->length_) {
    LOGW("close connection directly");
    on_error();
  } else {
    response(std::move(buffer), [self, this](BufferUPtr /*buffer*/) {
      on_finished(); // after write complete, close the connection
    });
  }
}

void Session::response(BufferUPtr buffer,
  std::function<void(BufferUPtr)> const& on_send) {
  if (closed_)
    return;

  if (!buffer) {
    on_error();
    return;
  }

  if (!traffic_control()) {
    // delay send
    tc_timer_.expires_from_now(std::chrono::milliseconds(10));
    auto self = shared_from_this();
    auto wrapped_buffer = wrap_move(std::move(buffer));
    LOGI("delay send");
    tc_timer_.async_wait([self, this, wrapped_buffer, on_send](
      const asio::error_code& error) {
      if (!error) {
        auto buffer = std::move(*wrapped_buffer);
        response(std::move(buffer), on_send);
      }
    });
  } else {
    response_internal(std::move(buffer), on_send);
  }
}

void Session::response_internal(BufferUPtr buffer,
  std::function<void(BufferUPtr)> const& on_send) {
  assert(buffer);
  auto asio_buffer = buffer->asio_write_buffer();
  auto wrapped_buffer = wrap_move(std::move(buffer));
  auto self = shared_from_this();
  asio::async_write(socket_, asio_buffer,
    [self, this, wrapped_buffer, on_send](
      const asio::error_code& error, size_t byte_transferred) {
    if (error) {
      on_error();
    } else {
      add_send_tick(byte_transferred);
      auto buffer = std::move(*wrapped_buffer);
      on_send(std::move(buffer));
    }
  });
}

void Session::on_error() {
  if (!closed_) {
    LOGI("close connection");
    closed_ = true;
    socket_.close();
    expired_timer_.cancel();
    tc_timer_.cancel();
    server_->on_session_error(shared_from_this());
  }
}

void Session::on_finished() {
  LOGI("response finished");
  on_error();
}

Config const* Session::config() const {
  return server_->config();
}

size_t Session::get_ranges_length() {
  assert(file_ranges_.size() > 1);
  size_t ret = 0;
  const size_t crlf_len = sizeof(misc_strings::crlf) - 1;
  const size_t prefix_len = 2; // sizeof("--") - 1
  const size_t suffix_len = 2; // sizeof("--") - 1
  const size_t boundary_len = sizeof(misc_strings::boundary) - 1;
  const size_t type_key_len = sizeof(misc_strings::content_type) - 1;
  const size_t type_len = strlen(file_type_);
  const size_t range_key_len = sizeof(misc_strings::content_range) - 1 +
    sizeof("bytes ") - 1;

  char temp[64];
  for (auto const& range : file_ranges_) {
    // \r\n + "--" + boundary + \r\n
    ret += crlf_len + prefix_len + boundary_len + crlf_len;
    // Content-Type: application/octet-stream\r\n
    ret += type_key_len + type_len + crlf_len;
    // Content-Range: bytes 1-1/1430256\r\n
    ret += range_key_len;
    ret += sprintf(temp, "%u-%u/%u", range.first, range.second,
      (uint32_t)file_length_);
    ret += crlf_len;
    // last \r\n
    ret += crlf_len;
    ret += get_range_length(range);
  }
  // the last boundary: \r\n + "--" + boundary + "--" + \r\n
  ret += crlf_len + prefix_len + boundary_len + suffix_len + crlf_len;
  return ret;
}

size_t Session::fread_log(void* ptr, size_t size, size_t nmemb, FILE* stream) {
  char desc[256];
  auto begin = std::chrono::steady_clock::now();
  size_t ret = fread(ptr, size, nmemb, stream);
  auto end = std::chrono::steady_clock::now();
  auto period = std::chrono::duration_cast<std::chrono::milliseconds>(
    end - begin);
  sprintf(desc, "fread(%zu,%zu), return %zu, time(ms): %zu", size, nmemb, ret,
    (size_t)period.count());
  if (ret != nmemb) {
    LOGW(desc);
  } else {
    LOGD(desc);
  }
  return ret;
}

void Session::add_send_tick(size_t len) {
  LOGI("send " << len << " bytes");
  if (need_traffic_control()) {
    SendTick send_tick;
    send_tick.time = std::chrono::steady_clock::now();
    send_tick.length = len;
    send_ticks_.push_back(send_tick);
    LOGD("send_ticks_.size(): " << send_ticks_.size());
  }
}

// return true: send directly
// return false: delay sometime
bool Session::traffic_control() {
  if (!need_traffic_control())
    return true;

  auto now = std::chrono::steady_clock::now();
  while (!send_ticks_.empty()) {
    auto const& front = send_ticks_.front();
    auto period = std::chrono::duration_cast<std::chrono::milliseconds>(
      now - front.time);
    if (period.count() > 1000) {
      send_ticks_.pop_front();
    } else {
      break;
    }
  }
  size_t speed = 0;
  for (auto const& i : send_ticks_) {
    speed += i.length;
  }

  LOGD("current speed: " << speed);

  return speed < max_speed_;
}
