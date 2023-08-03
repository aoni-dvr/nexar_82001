#pragma once

#define DEFAULT_MAX_SPEED (1024*1024*4)

class Config {
public:
  Config(std::string const& filename); // throw std::exception
  std::string const& root_dir() const {
    return root_dir_;
  }
  uint32_t max_connections() const {
    return max_connections_;
  }
  uint32_t max_speed() const {
    return max_speed_;
  }
  uint16_t bind_port() const {
    return bind_port_;
  }
  uint32_t max_packet_len() const {
    return max_packet_len_;
  }
  uint32_t max_packet_count() const {
    return max_packet_count_;
  }
  std::string const& log_dir() const {
    return log_dir_;
  }
  int log_level() const {
    return log_level_;
  }
  int max_log_size() const {
    return max_log_size_;
  }
private:
  bool valid() const;
  std::string root_dir_;
  uint16_t bind_port_;
  uint32_t max_connections_;
  uint32_t max_speed_;
  uint32_t max_packet_len_;
  uint32_t max_packet_count_;
  std::string log_dir_;
  int log_level_;
  int max_log_size_;
};
