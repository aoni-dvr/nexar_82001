#include "public.h"
#include "log_file.h"
#include "log.h"

const std::string kStartFlagLog = "_+_+_+_+_+_+_httpd start_+_+_+_+_+_+_";

LogFile::LogFile(std::string const& path, int max_log_size)
  : file_log_(nullptr, fclose) {
  if (path.empty())
    return;

  max_log_size_ = max_log_size;
  pathname_ = path + "/" + "httpd.log";
  file_log_.reset(fopen(pathname_.c_str(), "a+b"));
  if (!file_log_) {
    printf("open log failed: %s\n", pathname_.c_str());
    return;
  }

  printf("LogFile::start: %s\n", path.c_str());

  if (!update_current_size()) {
    file_log_.reset();
    return;
  }

  if (!try_clean_old()) {
    printf("LogFile::try_clean_old failed\n");
    file_log_.reset();
  }
}

LogFile::~LogFile() { stop(); }

std::string const& LogFile::pathname() {
  return pathname_;
}

void LogFile::stop() {
  if (file_log_) {
    LOGI("");
    file_log_.reset();
  }
}

void LogFile::save_log(char const* level, std::string const& text) {
  if (!file_log_)
    return;

  time_t now = time(NULL);
  std::string now_str(ctime(&now));
  if (!now_str.empty()) {
    now_str.pop_back();
  }

  auto file = file_log_.get();

  fwrite(now_str.c_str(), 1, now_str.size(), file);
  current_size_ += now_str.size();

  fwrite("\t", 1, 1, file);
  ++current_size_;

  size_t level_len = strlen(level);
  fwrite(level, 1, level_len, file);
  current_size_ += level_len;

  fwrite("\n", 1, 1, file);
  ++current_size_;

  fwrite(text.c_str(), 1, text.size(), file);
  current_size_ += text.size();

  fwrite("\n", 1, 1, file);
  ++current_size_;

  fflush(file);

  if (!try_clean_old()) {
    printf("LogFile::try_clean_old failed2\n");
    file_log_.reset();
    return;
  }

#ifdef _DEBUG
  int current_size = get_current_size();
  if (current_size_ != current_size) {
    LOGE("oops, current_size mismatch: " << current_size_ << " != " <<
      current_size);
  }
#endif
}

bool LogFile::try_clean_old() {
  if (current_size_ < max_log_size_)
    return true;

  auto file = file_log_.get();
  size_t pos = (size_t)current_size_ - (max_log_size_ / 2);
  size_t read_len = max_log_size_ / 2;
  if (0 != fseek(file, pos, SEEK_SET))
    return false;
  std::unique_ptr<char[]> data(new char[read_len]);
  //auto data = std::make_unique<char[]>(read_len);
  if (read_len != fread(data.get(), 1, read_len, file))
    return false;
  char const* start = strstr(data.get(), kStartFlagLog.c_str());
  if (!start) {
    start = strstr(data.get(), "\n");
    if (!start) {
      start = data.get();
    } else {
      start += 1;
    }
  } else {
    start += kStartFlagLog.size() + 1;
  }

  size_t write_len = read_len - (start - data.get());
  file_log_.reset(fopen(pathname_.c_str(), "wb+"));
  if (!file_log_)
    return false;
  if (write_len != fwrite(start, 1, write_len, file_log_.get()))
    return false;

  current_size_ = write_len;
  return true;
}

bool LogFile::update_current_size() {
  current_size_ = get_current_size();
  return current_size_ != -1;
}

int LogFile::get_current_size() {
  auto file = file_log_.get();
  int fd = fileno(file);
  struct stat s;
  if (0 != fstat(fd, &s)) {
    return -1;
  }
  return s.st_size;
}