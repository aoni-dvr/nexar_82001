#pragma once

#include <string>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#include "log_stream.h"

extern int g_log_level;

void start_log(int log_level, int max_log_size, std::string const& log_dir);
void log_internal(int level, std::string const& text);
std::string get_log_pathname();

#ifdef ENABLE_LOG
#define LOG(level, f) { \
  if (level >= g_log_level) { \
    log_stream s; \
    s << "[" << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "]\t" << \
    f << log_stream::endl; \
    log_internal(level, s.str()); \
    } \
}
#else
#define LOG(level, f)
#endif

enum LogPriority {
  LOG_UNKNOWN = 0,
  LOG_DEFAULT,    /* only for SetMinPriority() */
  LOG_VERBOSE,
  LOG_DEBUG,
  LOG_INFO,
  LOG_WARN,
  LOG_ERROR,
  LOG_FATAL,
  LOG_SILENT,     /* only for SetMinPriority(); must be last */
};

#ifdef ENABLE_LOG
#define LOGV(f)  LOG(LOG_VERBOSE, f)
#define LOGD(f)  LOG(LOG_DEBUG, f)
#define LOGI(f)  LOG(LOG_INFO, f)
#define LOGW(f)  LOG(LOG_WARN, f)
#define LOGE(f)  LOG(LOG_ERROR, f)
#define LOGF(f)  LOG(LOG_FATAL, f)
#else
#define LOGV(f)
#define LOGD(f)
#define LOGI(f)
#define LOGW(f)
#define LOGE(f)
#define LOGF(f)
#endif

struct LOG_TICK {
  LOG_TICK(char const* desc) : desc_(desc) {
    begin_ = std::chrono::steady_clock::now();
  }
  ~LOG_TICK() {
    auto end = std::chrono::steady_clock::now();
    auto period = std::chrono::duration_cast<std::chrono::milliseconds>(
      end - begin_);
    LOGD(desc_ << " use " << (size_t)period.count() << " ms");
  }
private:
  std::chrono::steady_clock::time_point begin_;
  char const* desc_;
};

