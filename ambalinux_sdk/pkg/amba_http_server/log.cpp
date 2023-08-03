#include "public.h"
#include "misc.h"
#include "log_file.h"

namespace
{
  inline char const* LogLevelString(int level) {
    char const * text[] = { "LOG_UNKNOWN", "LOG_DEFAULT", "LOG_VERBOSE",
      "LOG_DEBUG", "LOG_INFO", "LOG_WARN", "LOG_ERROR", "LOG_FATAL",
      "LOG_SILENT" };
    return text[level];
  }  
  std::unique_ptr<LogFile> g_log_file;
} // namespace anonymous


int g_log_level = LOG_VERBOSE;

void start_log(int log_level, int max_log_size, std::string const& log_dir) {
  g_log_level = log_level;
  g_log_file.reset(new LogFile(log_dir, max_log_size));
  LOGI(kStartFlagLog);
}

std::string get_log_pathname() {
  std::string pathname;
  if (g_log_file) {
    pathname = g_log_file->pathname();
  }
  return pathname;
}

void log_internal(int level, std::string const& text) {
  if (level >= g_log_level) {
    char const* level_string = LogLevelString(level);
    printf("%s\t%s\n", level_string, text.c_str());
    if (g_log_file) {
      g_log_file->save_log(level_string, text);
    }
  }
}

