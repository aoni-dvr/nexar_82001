#pragma once
#include <functional>
#include <vector>
#include "http/http_parser.h"

namespace http
{
  // c++ wrapper
  struct ParserSettings {
    typedef std::function<int(const char *at, size_t length)> f_data_cb;
    typedef std::function<int()> f_cb;
    f_cb      on_message_begin;
    f_data_cb on_url;
    f_data_cb on_status;
    f_data_cb on_header_field;
    f_data_cb on_header_value;
    f_cb      on_headers_complete;
    f_data_cb on_body;
    f_cb      on_message_complete;
    f_cb      on_chunk_header;
    f_cb      on_chunk_complete;
  };

  struct Parser : public http_parser {
    Parser(enum http_parser_type type) {
      http_parser_init(this, type);
    }

    size_t execute(ParserSettings const* cpp_settings, const char *buf,
      size_t len) {
      this->data = (void*)cpp_settings;
      http_parser_settings raw_settings;
      build_raw_settings(cpp_settings, raw_settings);
      return http_parser_execute(this, &raw_settings, buf, len);
    }

    void parser_pause(int paused) {
      return http_parser_pause(this, paused);
    }

    int body_is_final() {
      return http_body_is_final(this);
    }

    int should_keep_alive() {
      return http_should_keep_alive(this);
    }

    static const char* method_str(enum http_method m) {
      return http_method_str(m);
    }

    static const char *errno_name(enum http_errno err) {
      return http_errno_name(err);
    }

    static const char *errno_description(enum http_errno err) {
      return http_errno_description(err);
    }

    static int parse_url(const char *buf, size_t buflen,
      int is_connect, struct http_parser_url *u) {
      return http_parser_parse_url(buf, buflen, is_connect, u);
    }
  private:
    void build_raw_settings(ParserSettings const* cpp_settings,
      http_parser_settings& raw_settings) {
      memset(&raw_settings, 0, sizeof(raw_settings));
      raw_settings.on_body = (cpp_settings->on_body) ? on_body : nullptr;
      raw_settings.on_headers_complete = (cpp_settings->on_headers_complete) ? on_headers_complete : nullptr;
      raw_settings.on_header_field = (cpp_settings->on_header_field) ? on_header_field : nullptr;
      raw_settings.on_header_value = (cpp_settings->on_header_value) ? on_header_value : nullptr;
      raw_settings.on_message_begin = (cpp_settings->on_message_begin) ? on_message_begin : nullptr;
      raw_settings.on_message_complete = (cpp_settings->on_message_complete) ? on_message_complete : nullptr;
      raw_settings.on_status = (cpp_settings->on_status) ? on_status : nullptr;
      raw_settings.on_url = (cpp_settings->on_url) ? on_url : nullptr;
      raw_settings.on_chunk_header = (cpp_settings->on_chunk_header) ? on_chunk_header : nullptr;
      raw_settings.on_chunk_complete = (cpp_settings->on_chunk_complete) ? on_chunk_complete : nullptr;
    }
    static inline ParserSettings const* cpp_settings(http_parser* parser) {
      return (ParserSettings const*)(parser->data);
    }

    static int on_message_begin(http_parser* parser) {
      return cpp_settings(parser)->on_message_begin();
    }

    static int on_url(http_parser* parser, const char *at, size_t length) {
      return cpp_settings(parser)->on_url(at, length);
    }

    static int on_status(http_parser* parser, const char *at, size_t length) {
      return cpp_settings(parser)->on_status(at, length);
    }

    static int on_header_field(http_parser* parser, const char *at, size_t length) {
      return cpp_settings(parser)->on_header_field(at, length);
    }

    static int on_header_value(http_parser* parser, const char *at, size_t length) {
      return cpp_settings(parser)->on_header_value(at, length);
    }

    static int on_headers_complete(http_parser* parser) {
      return cpp_settings(parser)->on_headers_complete();
    }

    static int on_body(http_parser* parser, const char *at, size_t length) {
      return cpp_settings(parser)->on_body(at, length);
    }

    static int on_message_complete(http_parser* parser) {
      return cpp_settings(parser)->on_message_complete();
    }

    static int on_chunk_header(http_parser* parser) {
      return cpp_settings(parser)->on_chunk_header();
    }

    static int on_chunk_complete(http_parser* parser) {
      return cpp_settings(parser)->on_chunk_complete();
    }
  };
}