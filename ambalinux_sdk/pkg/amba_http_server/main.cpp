#include "public.h"
#include "config.h"

int win_main(int argc, char* argv[]);

int linux_main(int argc, char* argv[]);

int main(int argc, char** argv) {
#ifdef _WIN32
  int ret = win_main(argc, argv);
#else
  int ret = linux_main(argc, argv);
#endif
  //release_comm_rtos();
  if (ret == 0) {
    LOGI("process exit: " << getpid() << ", ret: " << ret);
  } else {
    LOGE("process exit: " << getpid() << ", ret: " << ret);
  }
}