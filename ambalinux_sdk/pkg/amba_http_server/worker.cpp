#include "public.h"
#include "worker.h"
#include "server.h"

Worker::Worker(std::string const& config_pathname) {
  config_.reset(new Config(config_pathname));
  start_log(config_->log_level(), config_->max_log_size(), config_->log_dir());
  server_.reset(new Server(config_.get()));
}

bool Worker::run() {
  change_limit();

  try {
    server_->start();
    server_->run(); // never return
                  // server.close();
    LOGI("worker exit\n");
  } catch (std::exception& e) {
    LOGF("exception: " << e.what());
  }
  return want_exit_.load();
}

void Worker::async_close() {
  LOGI("ask for exit");
  want_exit_.store(true);
  server_->close();
}