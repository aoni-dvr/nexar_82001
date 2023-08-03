#pragma once

#include "config.h"
#include "server.h"


class Worker {
public:
  Worker(std::string const& config_pathname);
  bool run();
  void async_close();
private:
  std::unique_ptr<Config> config_;
  std::unique_ptr<Server> server_;
  std::atomic<bool> want_exit_{ false };
};
