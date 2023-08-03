#pragma once

namespace gsl {
  template <class T>
  using owner = T;

  struct char_view {
    char const* at = nullptr;
    size_t len = 0;
  };
}
