#pragma once

class LogFile {
public:
  LogFile(std::string const& path, int max_log_size);
  ~LogFile();
  void save_log(char const* level, std::string const& text);
  std::string const& pathname();
private:
  bool try_clean_old();
  void stop();
  bool update_current_size();
  int get_current_size();
private:
  std::string pathname_;
  FileUniquePtr file_log_;
  int max_log_size_ = 0;
  int current_size_ = 0;
};

extern const std::string kStartFlagLog;