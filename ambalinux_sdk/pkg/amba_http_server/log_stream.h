#pragma once

#include "gsl.h"

// avoid to use std::stream
class log_stream {
public:
  enum showmode {
    dec = 1,//out put as dec
    hex,
    endl,
  };
  log_stream() : mode_(dec) {}
  void insert(const char* val);
  void insert(const char* val, size_t size);
  std::string const& str() { return str_; }
  showmode get_mode() { return mode_; }
  void set_mode(showmode mode) { mode_ = mode; }
private:
  showmode mode_;
  std::string str_;
};

inline void log_stream::insert(const char* val) {
  if (val) {
    size_t size = strlen(val);
    str_.insert(str_.end(), val, val + size);
  }
}

inline void log_stream::insert(const char* val, size_t size) {
  if (val && size) {
    str_.insert(str_.end(), val, val + size);
  }
}

inline log_stream& operator << (log_stream& obj, const char* val) {
  if (val) {
    obj.insert(val);
  }
  return obj;
}

inline log_stream& operator << (log_stream& obj, const void* val) {
  char str_val[64] = {};
  sprintf(str_val, "%p", val);
  obj.insert(str_val);
  return obj;
}

inline log_stream& operator << (log_stream& obj, int val) {
  char str_val[64] = {};
  if (obj.get_mode() == log_stream::dec) {
    sprintf(str_val, "%d", val);
  } else if (obj.get_mode() == log_stream::hex) {
    sprintf(str_val, "%x", val);
  }
  obj.insert(str_val);
  return obj;
}

inline log_stream& operator << (log_stream& obj, long long val) {
  char str_val[128] = {};
  if (obj.get_mode() == log_stream::dec) {
    sprintf(str_val, "%lld", val);
  } else if (obj.get_mode() == log_stream::hex) {
    sprintf(str_val, "%llx", val);
  }
  obj.insert(str_val);
  return obj;
}

inline log_stream& operator << (log_stream& obj, float val) {
  char str_val[64] = {};
  sprintf(str_val, "%f", val);
  obj.insert(str_val);
  return obj;
}

inline log_stream& operator << (log_stream& obj, double val) {
  char str_val[64] = {};
  sprintf(str_val, "%f", (float)val);
  obj.insert(str_val);
  return obj;
}

inline log_stream& operator << (log_stream& obj, unsigned int val) {
  char str_val[64] = {};
  if (obj.get_mode() == log_stream::dec) {
    sprintf(str_val, "%d", val);
  } else if (obj.get_mode() == log_stream::hex) {
    sprintf(str_val, "%x", val);
  }
  obj.insert(str_val);
  return obj;
}

inline log_stream& operator << (log_stream& obj, unsigned long val) {
  char str_val[64] = {};
  if (obj.get_mode() == log_stream::dec) {
    sprintf(str_val, "%ld", val);
  } else if (obj.get_mode() == log_stream::hex) {
    sprintf(str_val, "%lx", val);
  }
  obj.insert(str_val);
  return obj;
}

inline log_stream& operator << (log_stream& obj, unsigned long long val) {
  char str_val[128] = {};
  if (obj.get_mode() == log_stream::dec) {
    sprintf(str_val, "%lld", val);
  } else if (obj.get_mode() == log_stream::hex) {
    sprintf(str_val, "%llx", val);
  }
  obj.insert(str_val);
  return obj;
}

inline log_stream& operator << (log_stream& obj, const std::string& val) {
  obj.insert(val.c_str(), val.size());
  return obj;
}

inline log_stream& operator << (log_stream& obj,
  std::pair<char const*, size_t> const& val) {
  obj.insert(val.first, val.second);
  return obj;
}

inline log_stream& operator << (log_stream& obj,
  gsl::char_view const& val) {
  obj.insert(val.at, val.len);
  return obj;
}

inline log_stream& operator << (log_stream& obj, log_stream::showmode mode) {
  obj.set_mode(mode);
  if (mode == log_stream::endl) {
    obj.insert("\n");
  }
  return obj;
}
