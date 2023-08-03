#include "public.h"
#include "config.h"
#include "inicxx.h"
#include "misc.h"

namespace {
  const int kMaxLogSize = 100 * 1024;
}

Config::Config(std::string const& filename) {
  INIReader ini(filename);
  root_dir_ = ini.Get("basic", "root", "");
  bind_port_ = (uint16_t)ini.GetInteger("basic", "bind_port", 80);
  max_connections_ = (uint32_t)ini.GetInteger("basic", "max_connections", 20);
  max_speed_ = (uint32_t)ini.GetInteger("basic", "max_speed", DEFAULT_MAX_SPEED);
  max_packet_len_ = (uint32_t)ini.GetInteger("basic", "max_packet_len",
    16 * 1024);
  max_packet_count_ = (uint32_t)ini.GetInteger("basic", "max_packet_count",
    max_connections_ * 4); // *4 should enough
  log_dir_ = ini.Get("basic", "logpath", "");
  log_level_ = ini.GetInteger("basic", "loglevel", LOG_SILENT);
  max_log_size_ = ini.GetInteger("basic", "max_log_size", kMaxLogSize);
#if 1
  if (!root_dir_.empty()) {
    if (root_dir_.back() == '\\' || root_dir_.back() == '/') {
      root_dir_.pop_back();
    }
  }

  if (!log_dir_.empty()) {
    if (log_dir_.back() == '\\' || log_dir_.back() == '/') {
      log_dir_.pop_back();
    }
  }
#endif
  if (!valid()) {
    throw std::runtime_error("invalid config");
  }

}

bool Config::valid() const {
  if (root_dir_.empty()) {
    printf("root_dir is empty\n");
    return false;
  }
  if (!bind_port_) {
    printf("bind_port is empty\n");
    return false;
  }
  if (!max_connections_) {
    printf("max_connections_ is 0\n");
    return false;
  }
  if (!max_speed_) {
    printf("max_speed_ is 0\n");
    return false;
  }
  if (!max_packet_len_) {
    printf("max_packet_len_ is 0\n");
    return false;
  }
  if (!max_packet_count_) {
    printf("max_packet_count_ is 0\n");
    return false;
  }

  if (!is_dir(root_dir_.c_str())) {
    printf("%s is not path\n", root_dir_.c_str());
    return false;
  }

  return true;
}