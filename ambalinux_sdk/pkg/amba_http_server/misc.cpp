#include "public.h"
#include "misc.h"

bool is_dir(char const* path) {
  struct stat st;
  if (stat(path, &st)) {
    return false;
  }
  if (!(st.st_mode & S_IFDIR)) {
    return false;
  }
  return true;
}

char const* has_str(char_view const& view, char const* key, size_t keylen) {
  if (view.len < keylen)
    return nullptr;
  for (size_t i = 0; i <= view.len - keylen; ++i) {
    char const* v = view.at + i;
    if (!memcmp(v, key, keylen))
      return v;
  }
  return nullptr;
}

bool same_str(char_view const& view, char const* key, size_t keylen) {
  if (view.len != keylen)
    return false;
  return memcmp(view.at, key, keylen) == 0;
}

bool same_str_nocase(char_view const& view, char const* key, size_t keylen) {
  if (view.len != keylen)
    return false;
  for (size_t i = 0; i < keylen; ++i) {
    if (tolower(view.at[i]) != key[i])
      return false;
  }
  return true;
}

bool start_with_nocase(char const* p, char const* q) {
  for (;;) {
    char a = *p;
    char b = *q;
    if (!b)
      return true;
    if (!a)
      return false;
    if (tolower(a) != tolower(b))
      return false;
    ++p;
    ++q;
  }
}

bool end_with_nocase(char const* p, char const* q) {
  size_t plen = strlen(p);
  size_t qlen = strlen(q);
  if (plen < qlen)
    return false;

  p = p + plen - qlen;
  for (;;) {
    char a = *p;
    char b = *q;
    if (!b)
      return true;
    if (!a)
      return false;
    if (tolower(a) != tolower(b))
      return false;
    ++p;
    ++q;
  }
}

bool parse_ranges_header(char_view const& header, http_ranges_t& ranges) {
  char value[128];
  size_t value_len = 0;
  if (header.len > (sizeof(value) - 2))
    return false;
  for (size_t i = 0; i < header.len; ++i) {
    if (header.at[i] != ' ') {
      value[value_len++] = header.at[i];
    }
  }
  if (!value_len)
    return false;

  if (value[value_len - 1] != ',') {
    value[value_len++] = ',';
  }
  value[value_len] = 0;

  char const* key = "bytes=";
  if (!start_with_nocase(value, key))
    return false;
  
  char* p = value + strlen(key); 
  for (char* q = p; *q; ++q) {
    if (*q == ',') {
      *q = 0;
      uint32_t a = std::numeric_limits<uint32_t>::max();
      uint32_t b = std::numeric_limits<uint32_t>::max();
      if (*p != '-') {
        // byte-range-spec = first-byte-pos "-" [last-byte-pos]
        // a or a- or a-b
        if (sscanf(p, "%u-%u", &a, &b) < 1) {
          return false;
        }
      } else {
        // suffix-byte-range-spec = "-" suffix-length
        // -b
        sscanf(p, "-%u", &b);
      }
      if (a == std::numeric_limits<uint32_t>::max() &&
        b == std::numeric_limits<uint32_t>::max()) {
        return false;
      }
      ranges.push_back(std::make_pair(a, b));
      p = q + 1;
    }
  }

  return true;
}

// 0-499: [0,499]
// 1 or 1-: [1, end]
// -2: [end-2, end]
bool uniform_range(http_range_t& range, size_t length) {
  if (range.first != std::numeric_limits<uint32_t>::max()) {
    // byte-range-spec
    if (range.first >= length)
      return false;
    if (range.second <= range.first)
      return false;
    if (range.second >= length) {
      range.second = length - 1;
    }
  } else {
    // suffix-byte-range-spec
    if (!range.second)
      return false;
    if (range.second >= length) {
      range.first = 0;
      range.second = length - 1;
    } else {
      range.first = length - range.second;
      range.second = length - 1;
    }    
  }
  return true;
}

bool uniform_ranges(http_ranges_t& ranges, size_t length) {
  if (!ranges.empty() && !length)
    return false;

  for (auto& r : ranges) {
    if (!uniform_range(r, length))
      return false;
  }
  return true;
}

size_t get_range_length(http_range_t const& range) {
  return range.second - range.first + 1;
}

void uniform_filename(std::string& name) {
  size_t start_pos = 0;
  while ((start_pos = name.find_first_of(",;/\\ :\r\n", start_pos)) !=
    std::string::npos) {
    name.replace(start_pos, 1, "_");
  }
}

#ifdef _WIN32
void change_limit() {
}
#else
void change_limit() {
  rlimit rlim;
  if (getrlimit(RLIMIT_NOFILE, &rlim) < 0) {
    LOGE("getrlimit failed, error: " << strerror(errno));
    return;
  }
  if (rlim.rlim_cur < rlim.rlim_max) {
    auto old_cur = rlim.rlim_cur;
    rlim.rlim_cur = rlim.rlim_max;
    if (setrlimit(RLIMIT_NOFILE, &rlim) < 0) {
      LOGE("setrlimit failed, error: " << strerror(errno) << " " <<
        std::to_string(old_cur) + " to " <<
        std::to_string(rlim.rlim_max));
    } else {
      LOGI("setrlimit success: " << std::to_string(old_cur) + " to " <<
        std::to_string(rlim.rlim_max));
    }
  } else {
    LOGI("nofile limit: " << rlim.rlim_cur);
  }
}
#endif

