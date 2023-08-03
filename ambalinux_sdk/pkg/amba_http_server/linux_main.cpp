#ifndef _WIN32
#include "public.h"
#include "worker.h"

std::unique_ptr<Worker> g_worker;
bool g_debug_mode = false;

bool is_master_process() {
  return getsid(0) == getpid();
}

void async_close() {
  if (g_worker) {
    g_worker->async_close();
  }
}

void on_fatal(std::string const& reason) {
  LOGI("on_fatal: " << reason);
  kill(-getsid(0), 9);
  _exit(-1);
}

void sigexit_handler(int /*sig*/) {
  if (g_debug_mode) {
    async_close();
    return;
  }
  if (is_master_process()) {
    kill(-getsid(0), SIGUSR1);
  }
}

void siguser_handler(int sig) {
  if (g_debug_mode) {
    async_close();
    return;
  }
  if (!is_master_process() && sig == SIGUSR1) {
    LOGI("siguser_handler, ask worker to exit");
    async_close();
  }
}

void init_signal() {
  /*
  * ignore SIGPIPE signals; we can use errno==EPIPE if we
  * need that information
  */
  struct sigaction sa;
  sa.sa_handler = SIG_IGN;
  sa.sa_flags = 0;
  if (sigemptyset(&sa.sa_mask) == -1
    || sigaction(SIGPIPE, &sa, 0) == -1
    || sigaction(SIGHUP, &sa, 0) == -1
    ) {
    perror("failed to ignore SIGPIPE; sigaction");
    exit(EXIT_FAILURE);
  }

  sa.sa_handler = sigexit_handler;
  sa.sa_flags = 0;
  sigaction(SIGTERM, &sa, NULL);
  sigaction(SIGQUIT, &sa, NULL);
  sigaction(SIGINT, &sa, NULL);

  sa.sa_handler = siguser_handler;
  sa.sa_flags = 0;
  sigaction(SIGUSR1, &sa, NULL);
  sigaction(SIGUSR2, &sa, NULL);
}

void do_daemon() {
  if (getppid() == 1)
    return;

  /* Fork off for the second time*/
  pid_t pid = fork();

  /* An error occurred */
  if (pid < 0)
    exit(0);

  /* Success: Let the parent terminate */
  if (pid > 0)
    exit(0);

  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  /* handle standart I/O */
  int i = open("/dev/null", O_RDWR);
  LOGD("should be 0: " << i);
  int temp = dup(i);
  LOGD("should be 1: " << temp);
  temp = dup(i);
  LOGD("should be 2: " << temp);

  /* set newly created file permissions */
  umask(022);
}

bool processor(std::string const& config_pathname,
  std::string const& init_message) {  
  g_worker.reset(new Worker(config_pathname));
  LOGI("processor: " << getpid() << "," << init_message);
  bool ret = g_worker->run(); // never return
  g_worker.reset();
  LOGI("processor return: " << (ret? "true":"false"));
  return ret;
}

bool my_wait(int* status) {
  for (;;) {
    pid_t pid = wait(status);
    if (pid > 0) {
      LOGI("wait return: " << pid);
      return true;
    }
    if (errno != EINTR)
      return false;

    LOGI("wait EINTR");
  }
}

useconds_t get_wait_us(int failed_times) {
  failed_times = (failed_times % 10) + 1;
  return failed_times * 3 * 1000 * 1000;
}

int linux_main(int argc, char** argv) {
  
  if (argc <= 1) {
    LOGE("error: need parameter config_path_name");
    return -1;
  }

  std::string config_pathname;

  if (strcmp(argv[1], "-debug") == 0) {
    LOGI("debug mode");
    g_debug_mode = true;
    if (argc >= 3) {
      config_pathname = argv[2];
    }
  } else if (argc > 1) {
    config_pathname = argv[1];
  }

  if (config_pathname.empty()) {
    LOGE("error: need parameter config_path_name");
    return -1;
  }

  init_signal();

  if (g_debug_mode) {
    return (processor(config_pathname, "debug mode") ? 0 : -1);
  }

  do_daemon();
  // need root
  int sid = setsid();
  if (sid < 0) {
    LOGI("setsid failed: " << strerror(errno));
    return -3;
  } else if (sid != getpid()) {
    LOGI("oops, sid " << sid << " != " << "pid " << getpid());
    return -4;
  }

  int failed_times = 0;
  for (;;) {
    LOGI("create child");
    pid_t pid = fork();
    if (pid < 0) {
      on_fatal("fork");
    }

    if (pid == 0) {
      // child process, close all fd
      for (int i = 0; i < 1024; ++i) {
        //close(i);
      }
      std::string init_message;
      if (failed_times) {
        init_message = "restart after failed ";
        init_message += std::to_string(failed_times);
        init_message += " times";
      } else {
        init_message = "first start";
      }
      //init_comm_rtos();
      
      return (processor(config_pathname, init_message) ? 0 : -1);
    }

    int instat = 0;
    if (my_wait(&instat)) {
      LOGI("instat = " << instat << ", WIFEXITED(instat) = " << WIFEXITED(instat)
        << ", WEXITSTATUS(instat) = " << WEXITSTATUS(instat));
      if (WIFEXITED(instat) && WEXITSTATUS(instat) == 0) {
        // if child process exit with 0
        break;
      }

      LOGE("child process exit unexcepted");

      usleep(get_wait_us(++failed_times));
    } else {
      // oops
      on_fatal("wait");
    }
  }
  return 0;
}
#endif // #ifndef _WIN32