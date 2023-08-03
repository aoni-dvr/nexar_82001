/**
*  @file avf_daemon.c
*
* Copyright (c) [2020] Ambarella International LP
*
* This file and its contents ("Software") are protected by intellectual
* property rights including, without limitation, U.S. and/or foreign
* copyrights. This Software is also the confidential and proprietary
* information of Ambarella International LP and its licensors. You may not use, reproduce,
* disclose, distribute, modify, or otherwise prepare derivative works of this
* Software or any portion thereof except pursuant to a signed license agreement
* or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
* In the absence of such an agreement, you agree to promptly notify and return
* this Software to Ambarella International LP.
*
* This file includes sample code and is only for internal testing and evaluation.  If you
* distribute this sample code (whether in source, object, or binary code form), it will be
* without any warranty or indemnity protection from Ambarella International LP or its affiliates.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
* MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*  @detail xxxx
*
*/
#include <stdio.h>
#include <stdint.h>         /* for fixed width integer types */
#include <stdlib.h>
#include <unistd.h>         /* for getcwd() */
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <mqueue.h>
#include <string.h>         /* for strcpy() */
#include <sys/prctl.h>      /* for process operation, prctl() */
#include <errno.h>          /* for number of last error */

#include "ArmErrCode.h"
#include "avf_daemon.h"
#include "avf.h"

#define MAX_Q_NUM               (2)
#define MAX_UNS_Q_NUM          (20)

#define AVFD_INIT_STATE_MEM     (1)
#define AVFD_INIT_STATE_REQ     (2)
#define AVFD_INIT_STATE_RES     (3)
#define AVFD_INIT_STATE_UNS     (4)

static const char avf_daemon_name[] = "AVF daemon";
static mqd_t      g_uns_dpt;    /* TBD: client side debug message */
static uint32_t   g_init_state = 0;

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: svc_nonblock_thd
 *
 *  @Description:: The thread to run avf_run_multi()
 *
 *  @Input      ::
 *      arg:       The thread arg
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void* svc_nonblock_thd(void *arg)
{
  uint32_t slot = *((uint32_t *)arg);

  (void) avf_run_multi(slot);
  (void) avf_reset_run_info(slot);

  return NULL;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: skeleton_daemon
 *
 *  @Description:: Skeleton of daemon
 *
 *  @Input      ::
 *    debug_f:     Debug flag
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void skeleton_daemon(uint32_t debug_f)
{
  pid_t pid;

  /* Fork off the parent process */
  pid = fork();

  /* An error occurred */
  if (pid < 0)
    exit(EXIT_FAILURE);

  /* Success: Let the parent terminate */
  if (pid > 0)
    exit(EXIT_SUCCESS);

  /* On success: The child process becomes session leader */
  if (setsid() < 0)
    exit(EXIT_FAILURE);

  /* Catch, ignore and handle signals */
  //TODO: Implement a working signal handler */
  signal(SIGCHLD, SIG_IGN);
  signal(SIGHUP, SIG_IGN);

  /* Fork off for the second time*/
  pid = fork();

  /* An error occurred */
  if (pid < 0)
    exit(EXIT_FAILURE);

  /* Success: Let the parent terminate */
  if (pid > 0)
    exit(EXIT_SUCCESS);

  /* Set new file permissions */
  umask(0);

  /* Change the working directory to the root directory */
  /* or another appropriated directory */
  chdir("/");

  /* Close all open file descriptors */
  #if 0
  int x;
  for (x = sysconf(_SC_OPEN_MAX); x>=0; x--)
  {
      close (x);
  }
  #endif
  if (debug_f == 0) {
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
  }

  /* Open the log file */
  openlog("AVF daemon", LOG_PID, LOG_DAEMON);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: avf_daemon_start
 *
 *  @Description:: AVF daemon
 *
 *  @Input      ::
 *    debug_f:     Debug flag
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
void avf_daemon_start(uint32_t debug_f)
{
  mqd_t req_dpt, res_dpt;

  do {
    /* Get all CMA */
    {
      uint32_t Rval;
      Rval = DaemonIO_InitMem();
      if (Rval == ARM_NG) {
        syslog(LOG_ERR, "## Init mem fail!");
        break;
      }
      (void) avf_init(); /* TBD: integrate mem */
      g_init_state = AVFD_INIT_STATE_MEM;
    }
    /* Create queue for receiving client request */
    {
      struct mq_attr attr = {0};
      char queue_name[64] = {0};
      strcpy(queue_name, AVF_DAEMON_REQUEST_QUE_NAME);
      syslog(LOG_NOTICE, "Create message queue %s", queue_name);
      attr.mq_maxmsg  = MAX_Q_NUM;
      attr.mq_msgsize = sizeof(AVF_DAEMON_REQ_MSG_s);
      req_dpt = mq_open(queue_name, O_CREAT | O_RDONLY, 0644, &attr);
      if (req_dpt == -1) {
        syslog(LOG_ERR, "## Create queue fail!");
        break;
      }
      g_init_state = AVFD_INIT_STATE_REQ;
    }
    /* Create queue for transmitting response to client */
    {
      struct mq_attr attr = {0};
      char queue_name[64] = {0};
      strcpy(queue_name, AVF_DAEMON_RESPONSE_QUE_NAME);
      syslog(LOG_NOTICE, "Create message queue %s", queue_name);
      attr.mq_maxmsg  = MAX_Q_NUM;
      attr.mq_msgsize = sizeof(AVF_DAEMON_RES_MSG_s);
      res_dpt = mq_open(queue_name, O_CREAT | O_WRONLY, 0644, &attr);
      if (res_dpt == -1) {
        syslog(LOG_ERR, "## Create queue fail!");
        break;
      }
      g_init_state = AVFD_INIT_STATE_RES;
    }
    /* Create queue for unsolicited message to client */
    {
      struct mq_attr attr = {0};
      char queue_name[64] = {0};
      strcpy(queue_name, AVF_DAEMON_UNSOLICITED_QUE_NAME);
      syslog(LOG_NOTICE, "Create message queue %s", queue_name);
      attr.mq_maxmsg  = MAX_UNS_Q_NUM;
      attr.mq_msgsize = sizeof(AVF_DAEMON_UNS_MSG_s);
      g_uns_dpt = mq_open(queue_name, O_CREAT | O_WRONLY | O_NONBLOCK, 0644, &attr);/* TBD: Need to be verified */
      if (g_uns_dpt == -1) {
        syslog(LOG_ERR, "## Create queue fail!");
        break;
      }
      g_init_state = AVFD_INIT_STATE_UNS;
    }

    /* Process is detached from TTY */
    skeleton_daemon(debug_f);

    prctl(PR_SET_NAME, avf_daemon_name, NULL, NULL, NULL);  /* Set daemon name */
    syslog(LOG_NOTICE, "Daemon started.(%s)", VERSION_OF_AVF);
    while (1)
    {
      /* Daemon code */
      int32_t ret = 0;
      AVF_DAEMON_REQ_MSG_s req_msg = {0};
      AVF_DAEMON_RES_MSG_s res_msg = {0};

      /* Receive request */
      ret = mq_receive(req_dpt, (char *)&req_msg, sizeof(req_msg), NULL);
      if (ret != sizeof(req_msg)) {
        syslog(LOG_ERR, "## mq_receive() fail, %d", ret);
        break;
      }

      /* Reply request */
      switch (req_msg.code) {
      case AVFD_CODE_SVC_CHK:
        {
          uint32_t used_size = 0, free_size = 0;
          res_msg.code = AVFD_CODE_SVC_CHK;

          ret = avf_check_slot_usage();
          if (0 == ret) {
            /* Slot free; do mem usage check */
            DaemonIO_GetCmaInfo(&used_size, &free_size);
            if (used_size == 0) {
              res_msg.result = AVFD_RESULT_OK;
            } else {
              res_msg.result = AVFD_RESULT_NG;
              syslog(LOG_ERR, "## Mem leakage!( %u, %u)", used_size, free_size);
            }
          } else {
            /* Slot# used; bypass mem check */
            res_msg.result = AVFD_RESULT_OK;
            syslog(LOG_NOTICE, "Bypass mem check");
          }
          ret = mq_send(res_dpt, (char *)&res_msg, sizeof(res_msg), 0);
          if (ret == -1) {
            syslog(LOG_ERR, "## mq_send() fail!, %d", ret);
            goto stop;
          }
        }
        break;
       case AVFD_CODE_SVC_RUN:
        {
          uint32_t slot = 0xFF;
          /* Fill all run info to AVF */
          ret = avf_unpack_run_info(&req_msg.run_info);

          /* Send result code to client */
          res_msg.code = AVFD_CODE_SVC_RUN;
          if (ret == -1) {
            res_msg.result = AVFD_RESULT_NG;
          } else {
            res_msg.result = AVFD_RESULT_OK;
            slot = (uint32_t) ret;
            /* Show run info */
            (void) avf_dump_run_info(slot);
          }
          res_msg.slot = slot;
          ret = mq_send(res_dpt, (char *)&res_msg, sizeof(res_msg), 0);
          if (ret == -1) {
            syslog(LOG_ERR, "## mq_send() fail!, %d", ret);
            goto stop;
          }

          if (slot != 0xFF) {
            /* change working dir */
            char cwd[256];
            chdir(req_msg.run_info.working_dir);
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
              syslog(LOG_NOTICE, "Current working dir: %s", cwd);
            } else {
              syslog(LOG_ERR, "## Current working dir: ERR");
            }
            /* run flexidag */
            (void) avf_run_multi(slot);
            (void) avf_reset_run_info(slot);
          } else {
            syslog(LOG_ERR, "## No avaiable AVF slot!");
          }
        }
        break;
      case AVFD_CODE_SVC_NONBLOCK_RUN:
        {
          /* This is for QA to verify yield feature */
          /* Test only; doesn't include error handling */
          static pthread_t svc_nonblock_thread;
          static uint32_t slot = 0xFF;
          /* Fill all run info to AVF */
          ret = avf_unpack_run_info(&req_msg.run_info);

          /* Send result code to client */
          res_msg.code = AVFD_CODE_SVC_NONBLOCK_RUN;
          if (ret == -1) {
            res_msg.result = AVFD_RESULT_NG;
          } else {
            res_msg.result = AVFD_RESULT_OK;
            slot = (uint32_t) ret;
            /* Show run info */
            (void) avf_dump_run_info(slot);
          }
          res_msg.slot = slot;
          ret = mq_send(res_dpt, (char *)&res_msg, sizeof(res_msg), 0);
          if (ret == -1) {
            syslog(LOG_ERR, "## mq_send() fail!, %d", ret);
            goto stop;
          }

          if (slot != 0xFF) {
            /* change working dir */
            char cwd[256];
            chdir(req_msg.run_info.working_dir);
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
              syslog(LOG_NOTICE, "Current working dir: %s", cwd);
            } else {
              syslog(LOG_ERR, "## Current working dir: ERR");
            }
            /* run flexidag */
            syslog(LOG_NOTICE, "Daemon is on Non-Block mode!");
            pthread_create(&svc_nonblock_thread, NULL, svc_nonblock_thd, &slot);
          } else {
            syslog(LOG_ERR, "## No avaiable AVF slot!");
          }
        }
        break;
      case AVFD_CODE_SVC_YIELD_CHK:
        {
          ret = avf_compare_yield_out(req_msg.run_info.working_dir, req_msg.run_info.out_num);
          /* Send result code to client */
          res_msg.code = AVFD_CODE_SVC_YIELD_CHK;
          if (ret == -1) {
            res_msg.result = AVFD_RESULT_NG;
          } else {
            res_msg.result = AVFD_RESULT_OK;
          }
          ret = mq_send(res_dpt, (char *)&res_msg, sizeof(res_msg), 0);
          if (ret == -1) {
            syslog(LOG_ERR, "## mq_send() fail!, %d", ret);
            goto stop;
          }
        }
        break;
      case AVFD_CODE_SVC_CLOSE:
        syslog(LOG_NOTICE, "Daemon shutdown...");
        goto stop;
        break;
      default:
        syslog(LOG_ERR, "## Unknow msg code, %u", req_msg.code);
        goto stop;
      }
#if 0
      ret = mq_send(res_dpt, (char *)&res_msg, sizeof(res_msg), 0);
      if (ret == -1) {
        syslog(LOG_ERR, "## mq_send() fail!, %d", ret);
        break;
      }
#endif

    } /* Daemon */

  } while(0);

stop:

  syslog(LOG_NOTICE, "Daemon terminated.");

  if (g_init_state >= AVFD_INIT_STATE_MEM) {
    (void) DaemonIO_DeInitMem();
  }

  if (g_init_state >= AVFD_INIT_STATE_REQ) {
    (void) mq_close(req_dpt);
  }

  if (g_init_state >= AVFD_INIT_STATE_RES) {
    (void) mq_close(res_dpt);
  }

  if (g_init_state >= AVFD_INIT_STATE_UNS) {
    (void) mq_close(g_uns_dpt);
  }

  closelog();
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: avf_daemon_unsolicited_msg
 *
 *  @Description:: Send unsolicited message
 *
 *  @Input      ::
 *    code:        Message code
 *    slot:        Slot index
 *    result:      Result code
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
void avf_daemon_unsolicited_msg(uint32_t code, uint32_t slot, int32_t result)
{
  do {
    int32_t ret = 0;
    AVF_DAEMON_UNS_MSG_s uns_msg = {0};

    if (g_init_state < AVFD_INIT_STATE_UNS) {
      break;
    }
    /* Show message on daemon */
    if (AVFD_UNS_CODE_WATCHDOG == code) {
      syslog(LOG_NOTICE, "[SLOT%u]Flexidag Running!", slot);
    } else if (AVFD_UNS_CODE_DONE == code) {
      syslog(LOG_NOTICE, "[SLOT%u]Flexidag iterations are done!", slot);
    }

    /* Show message on client */
    uns_msg.code = code;
    uns_msg.slot = slot;
    uns_msg.result = result;
    ret = mq_send(g_uns_dpt, (char *)&uns_msg, sizeof(uns_msg), 0);
    if (ret == -1) {
      if (errno != EAGAIN) {
        syslog(LOG_ERR, "## mq_send() fail!", ret);
        break;
      } else {
        /* Bypass message queue full; just lost some message */
      }
    }

  } while(0);
}

