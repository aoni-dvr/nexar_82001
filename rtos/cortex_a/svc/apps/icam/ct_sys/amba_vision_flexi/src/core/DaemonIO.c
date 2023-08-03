/**
*  @file DaemonIO.c
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
#include <stdlib.h>         /* exit(), EXIT_SUCCESS */
#include <stdint.h>         /* for fixed width integer types */
#include <mqueue.h>
#include <string.h>         /* for strcpy() */
#include <syslog.h>         /* for syslog() */
#include <errno.h>          /* for error value */

#include "ArmLog.h"
#include "ArmMem.h"
#include "ArmMemPool.h"
#include "ArmErrCode.h"
#include "ArmMutex.h"
#include "avf_daemon.h"     /* For daemon IO message */
#include "DaemonIO.h"

typedef struct _DAEMONIO_HANDLE {
  uint32_t state;
  uint32_t slot;      /* Client Side Only */
  mqd_t    req_dpt;   /* Client Side Only */
  mqd_t    res_dpt;   /* Client Side Only */
  mqd_t    uns_dpt;   /* Client Side Only */
} DAEMONIO_HANDLE_s;

typedef struct _AVFD_MEM_HANDLE {
  uint32_t            total_size;
  uint32_t            avail_size;
  ArmMutex_t          mutex;      /* Protect avail_size */
  uint32_t            pool_id;
  flexidag_memblk_t   memblk;
} AVFD_MEM_HANDLE_s;

#define ARM_LOG_DAEMONIO     "DaemonIO"
#define ARM_LOG_AVFD         "AVFD"
#define MAX_SKIP_PROC_MSG    (2)
#define UNS_MSG_TIMEOUT      (6 * 60)
#define YIELD_FD1_INDEX      (0)
#define YIELD_FD2_INDEX      (1)
#define YIELD_PASS_UP_BOUND  (110)
#define YIELD_PASS_LOW_BOUND (95)
#define ALIGN4K(x)           (((x) + 4095U) & 0xFFFFF000U)
#define RESERVED_CMA_SIZE    (10 * 1024 * 1024)    /* Kernel need extra mem to do page frame management */

static DAEMONIO_HANDLE_s g_handle = {0};     /* Daemon and Client have their own handle */
static AVFD_MEM_HANDLE_s g_mem_handle = {0}; /* Daemon Only */


/*---------------------------------------------------------------------------*\
 * Client side APIs
\*---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: DaemonIO_Init
 *
 *  @Description:: Init daemon IO handle
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    INT32:       OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int32_t DaemonIO_Init()
{
  int32_t ret = DAEMON_OK;
  DAEMONIO_HANDLE_s *p_handle = &g_handle;

  /* Init daemon state */
  p_handle->state = DAEMON_STATE_OFF;

  /* Create queue for receiving client request */
  {
    char queue_name[64] = {0};
    strcpy(queue_name, AVF_DAEMON_REQUEST_QUE_NAME);
    ArmLog_DBG(ARM_LOG_DAEMONIO, "Open message queue %s", queue_name);
    p_handle->req_dpt = mq_open(queue_name, O_WRONLY);
    if (p_handle->req_dpt < -1) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## Open queue fail!");
      exit(EXIT_FAILURE);
    }
  }
  /* Create queue for transmitting response to client */
  {
    char queue_name[64] = {0};
    strcpy(queue_name, AVF_DAEMON_RESPONSE_QUE_NAME);
    ArmLog_DBG(ARM_LOG_DAEMONIO, "Open message queue %s", queue_name);
    p_handle->res_dpt = mq_open(queue_name, O_RDONLY);
    if (p_handle->res_dpt < -1) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## Open queue fail!");
      exit(EXIT_FAILURE);
    }
  }
  /* Create queue for unsolicited message to client */
  {
    char queue_name[64] = {0};
    strcpy(queue_name, AVF_DAEMON_UNSOLICITED_QUE_NAME);
    ArmLog_DBG(ARM_LOG_DAEMONIO, "Open message queue %s", queue_name);
    p_handle->uns_dpt = mq_open(queue_name, O_RDONLY);
    if (p_handle->uns_dpt < -1) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## Open queue fail!");
      exit(EXIT_FAILURE);
    }
  }

  return ret;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: DaemonIO_DeInit
 *
 *  @Description:: Shutdown AVFD
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    INT32:       OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int32_t DaemonIO_DeInit()
{
  int32_t ret = DAEMON_OK;
  DAEMONIO_HANDLE_s *p_handle = &g_handle;

  p_handle->state = DAEMON_STATE_OFF;

  ret = mq_close(p_handle->req_dpt);
  if (ret == -1) {
    ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_close() fail!");
    ret = DAEMON_NG;
  }

  ret = mq_close(p_handle->res_dpt);
  if (ret == -1) {
    ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_close() fail!");
    ret = DAEMON_NG;
  }

  ret = mq_close(p_handle->uns_dpt);
  if (ret == -1) {
    ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_close() fail!");
    ret = DAEMON_NG;
  }

  return ret;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: DaemonIO_CheckSvc
 *
 *  @Description:: Check if AVF daemon is inited
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    INT32:       OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int32_t DaemonIO_CheckSvc()
{
  int32_t ret = DAEMON_OK;
  AVF_DAEMON_REQ_MSG_s req_msg = {0};
  AVF_DAEMON_RES_MSG_s res_msg = {0};
  DAEMONIO_HANDLE_s *p_handle = &g_handle;

  do {
    req_msg.code = AVFD_CODE_SVC_CHK;
    ret = mq_send(p_handle->req_dpt, (char *)&req_msg, sizeof(req_msg), 0);
    if (ret == -1) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_send() fail!");
      p_handle->state = DAEMON_STATE_OFF;
      ret = DAEMON_NG;
      break;
    }

    ret = mq_receive(p_handle->res_dpt, (char *)&res_msg, sizeof(res_msg), NULL);
    if (ret != sizeof(res_msg)) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_receive() fail, %d", ret);
      ret = DAEMON_NG;
      break;
    }

    if (AVFD_CODE_SVC_CHK == res_msg.code) {
      if (AVFD_RESULT_OK == res_msg.result) {
        ArmLog_OK(ARM_LOG_DAEMONIO, "AVF Daemon is ready.");
        /* Update daemon state */
        p_handle->state = DAEMON_STATE_ON;
        ret = DAEMON_OK;
      } else {
        ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_receive() result is NG!!");
        ret = DAEMON_NG;
      }
    } else {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_receive() code mismatch, %u", res_msg.code);
      ret = DAEMON_NG;
    }
  } while(0);

  return ret;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: DaemonIO_RunFlexidag
 *
 *  @Description:: Request AVF daemon to run flexidag
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    INT32:       OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int32_t DaemonIO_RunFlexidag()
{
  int32_t ret = DAEMON_OK;
  AVF_DAEMON_REQ_MSG_s req_msg = {0};
  AVF_DAEMON_RES_MSG_s res_msg = {0};
  DAEMONIO_HANDLE_s *p_handle = &g_handle;

  do {
    ret = avf_pack_run_info(&req_msg.run_info);
    if (ret == -1) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## avf_pack_run_info() fail!");
      ret = DAEMON_NG;
      break;
    }
    req_msg.code = AVFD_CODE_SVC_RUN;
    ret = mq_send(p_handle->req_dpt, (char *)&req_msg, sizeof(req_msg), 0);
    if (ret == -1) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_send() fail!");
      p_handle->state = DAEMON_STATE_OFF;
      ret = DAEMON_NG;
      break;
    }

    ret = mq_receive(p_handle->res_dpt, (char *)&res_msg, sizeof(res_msg), NULL);
    if (ret != sizeof(res_msg)) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_receive() fail, %d", ret);
      ret = DAEMON_NG;
      break;
    }

    if (AVFD_CODE_SVC_RUN == res_msg.code) {
      p_handle->slot = res_msg.slot;
      if (AVFD_RESULT_OK == res_msg.result) {
        ArmLog_OK(ARM_LOG_DAEMONIO, "Flexidag%u running~", p_handle->slot);
        /* Update daemon state */
        ret = DAEMON_OK;
      } else {
        ArmLog_ERR(ARM_LOG_DAEMONIO, "## AVFD_CODE_SVC_RUN result is NG!!");
        ret = DAEMON_NG;
      }
    } else {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_receive() code mismatch, %u", res_msg.code);
      ret = DAEMON_NG;
    }
  } while(0);

  return ret;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: DaemonIO_RunNonBlock
 *
 *  @Description:: Request AVF daemon to run on Non-Block mode
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    INT32:       OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int32_t DaemonIO_RunNonBlock()
{
  int32_t ret = DAEMON_OK;
  AVF_DAEMON_REQ_MSG_s req_msg = {0};
  AVF_DAEMON_RES_MSG_s res_msg = {0};
  DAEMONIO_HANDLE_s *p_handle = &g_handle;

  /* NonBlock means daemon is non-block; i.e. it can receive other request */
  do {
    ret = avf_pack_run_info(&req_msg.run_info);
    if (ret == -1) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## avf_pack_run_info() fail!");
      ret = DAEMON_NG;
      break;
    }
    req_msg.code = AVFD_CODE_SVC_NONBLOCK_RUN;
    ret = mq_send(p_handle->req_dpt, (char *)&req_msg, sizeof(req_msg), 0);
    if (ret == -1) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_send() fail!");
      p_handle->state = DAEMON_STATE_OFF;
      ret = DAEMON_NG;
      break;
    }

    ret = mq_receive(p_handle->res_dpt, (char *)&res_msg, sizeof(res_msg), NULL);
    if (ret != sizeof(res_msg)) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_receive() fail, %d", ret);
      ret = DAEMON_NG;
      break;
    }

    if (AVFD_CODE_SVC_NONBLOCK_RUN == res_msg.code) {
      if (AVFD_RESULT_OK == res_msg.result) {
        ArmLog_OK(ARM_LOG_DAEMONIO, "Flexidag running~");
        /* Update daemon state */
        ret = DAEMON_OK;
      } else {
        ArmLog_ERR(ARM_LOG_DAEMONIO, "## AVFD_CODE_SVC_NONBLOCK_RUN result is NG!!");
        ret = DAEMON_NG;
      }
    } else {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_receive() code mismatch, %u", res_msg.code);
      ret = DAEMON_NG;
    }
  } while(0);

  return ret;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GetYieldUMsg
 *
 *  @Description:: Get yield unsolicited message
 *
 *  @Input      ::
 *    master_slot: The slot to control break state
 *    wait_flag:   The flag to wait all slot finish
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    INT32:       Average Proc Time
\*-----------------------------------------------------------------------------------------------*/
static int32_t GetYieldUMsg(uint32_t master_slot, uint32_t wait_flag)
{
  int32_t ret = DAEMON_OK;
  AVF_DAEMON_UNS_MSG_s uns_msg = {0};
  DAEMONIO_HANDLE_s *p_handle = &g_handle;
  uint32_t break_state = 0;
  uint32_t proc_msg_count = 0;
  int32_t total_proc_time = 0;
  uint32_t slot_done_flag = 0;

  do {
    struct timespec tm;

    clock_gettime(CLOCK_MONOTONIC, &tm);
    tm.tv_sec += UNS_MSG_TIMEOUT;
    ret = mq_timedreceive_monotonic(p_handle->uns_dpt, (char *)&uns_msg, sizeof(uns_msg), NULL, &tm);

    if (ret == -1) {
      int32_t errorvalue = errno;
      if (errorvalue == ETIMEDOUT) {
        ArmLog_ERR(ARM_LOG_DAEMONIO, "## Hit Daemon TimeOut!!! Please enable daemon log to check.");
      } else {
        ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_receive() fail, errno %d", errorvalue);
      }
      ret = DAEMON_NG;
      break;
    }

    switch (uns_msg.code) {
      case AVFD_UNS_CODE_SLOT_NUM:
        if (uns_msg.result == AVFD_RESULT_NG) {
          break_state = 1;
          ret = DAEMON_NG;
          ArmLog_ERR(ARM_LOG_DAEMONIO, "## Daemon slot are full.");
        } else {
          ArmLog_OK(ARM_LOG_DAEMONIO, "Prepare to run flexidag on Slot-%u!", uns_msg.slot);
        }
        break;
      case AVFD_UNS_CODE_FLEXI_CREATE:
        ArmLog_OK(ARM_LOG_DAEMONIO, "[SLOT%u]Flexidag Creating!", uns_msg.slot);
        break;
      case AVFD_UNS_CODE_FLEXI_START:
        ArmLog_OK(ARM_LOG_DAEMONIO, "[SLOT%u]Flexidag Start!", uns_msg.slot);
        break;
      case AVFD_UNS_CODE_WATCHDOG:
        ArmLog_OK(ARM_LOG_DAEMONIO, "[SLOT%u]Flexidag Running!", uns_msg.slot);
        break;
      case AVFD_UNS_CODE_DONE:
        ArmLog_OK(ARM_LOG_DAEMONIO, "[SLOT%u]Flexidag iterations are done!", uns_msg.slot);
        if (wait_flag == 1) {
          slot_done_flag++;
          if (slot_done_flag == 2) {
            break_state = 1;
          }
        } else {
          if (master_slot == uns_msg.slot) {
            break_state = 1;
          }
        }
        break;
      case AVFD_UNS_CODE_WATCHDOG_TIMEOUT:
        /* Script check the key word '|ERR' */
        ArmLog_ERR(ARM_LOG_DAEMONIO, "## Hit Watchdog TimeOut on Slot-%u!", uns_msg.slot);
        break_state = 1;
        break;
      case AVFD_UNS_CODE_GENERAL_ERR:
        if (uns_msg.result == AVFD_RESULT_READ_NG) {
          ArmLog_ERR(ARM_LOG_DAEMONIO, "## Check READ function on Slot-%u!", uns_msg.slot);
        } else if (uns_msg.result == AVFD_RESULT_FEED_NG) {
          ArmLog_ERR(ARM_LOG_DAEMONIO, "## Check FEED function on Slot-%u!", uns_msg.slot);
        } else if (uns_msg.result == AVFD_RESULT_FLEXI_NG) {
          ArmLog_ERR(ARM_LOG_DAEMONIO, "## Check FLEXI CREATE function on Slot-%u!", uns_msg.slot);
        } else {
          ArmLog_ERR(ARM_LOG_DAEMONIO, "## Enable Daemon log to check!!");
        }
        break_state = 1;
        break;
       case AVFD_UNS_CODE_PROC_TIME:
        if (master_slot == uns_msg.slot) {
          proc_msg_count++;
          if (proc_msg_count > MAX_SKIP_PROC_MSG) {
            total_proc_time += uns_msg.result;
          }
        }
        //ArmLog_OK(ARM_LOG_DAEMONIO, "[SLOT%u] Avg Proc Time: %u", uns_msg.slot, uns_msg.result);
        break;
      default:
        ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_receive() code mismatch, %u, on Slot-%u", uns_msg.code, uns_msg.slot);
        break_state = 1;
        ret = DAEMON_NG;
    }
  } while(break_state == 0);

  /* Avg proc time */
  if (proc_msg_count > MAX_SKIP_PROC_MSG) {
    proc_msg_count -= MAX_SKIP_PROC_MSG;
    ret = total_proc_time/proc_msg_count;
    ret/=100; // us -> 10ms
  }

  return ret;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: DaemonIO_TestYield
 *
 *  @Description:: Request AVF daemon to test yield
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    INT32:       OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int32_t DaemonIO_TestYield()
{
  int32_t ret = DAEMON_OK;
  AVF_DAEMON_REQ_MSG_s req_msg = {0};
  AVF_DAEMON_RES_MSG_s res_msg = {0};
  DAEMONIO_HANDLE_s *p_handle = &g_handle;
  int32_t  avg_proc_time[3] = {0};
  int32_t  fd2_dag_num = 0;

  do {
    /* 1. Run FD1:mnet-ssd */
    ret = avf_pack_yield_run_info(&req_msg.run_info, YIELD_FD1_INDEX, 200, 1);
    if (ret == -1) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## avf_pack_run_info() fail!");
      ret = DAEMON_NG;
      break;
    }
    req_msg.code = AVFD_CODE_SVC_RUN;
    ret = mq_send(p_handle->req_dpt, (char *)&req_msg, sizeof(req_msg), 0);
    if (ret == -1) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_send() fail!");
      p_handle->state = DAEMON_STATE_OFF;
      ret = DAEMON_NG;
      break;
    }

    ret = mq_receive(p_handle->res_dpt, (char *)&res_msg, sizeof(res_msg), NULL);
    if (ret != sizeof(res_msg)) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_receive() fail, %d", ret);
      ret = DAEMON_NG;
      break;
    }

    if (AVFD_CODE_SVC_RUN == res_msg.code) {
      p_handle->slot = res_msg.slot;
      if (AVFD_RESULT_OK == res_msg.result) {
        ArmLog_OK(ARM_LOG_DAEMONIO, "Flexidag%u running~", p_handle->slot);
        ret = DAEMON_OK;
      } else {
        ArmLog_ERR(ARM_LOG_DAEMONIO, "## AVFD_CODE_SVC_RUN result is NG!!");
        ret = DAEMON_NG;
      }
    } else {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_receive() code mismatch, %u", res_msg.code);
      ret = DAEMON_NG;
    }

    /* Show average proc time */
    {
      avg_proc_time[0] = GetYieldUMsg(p_handle->slot, 0);
      ArmLog_OK(ARM_LOG_DAEMONIO, "[YIELD]FD1 avg proc time: %d(ms)", avg_proc_time[0]/10);
    }
    /* Backup output */
    avf_backup_yield_output(YIELD_FD1_INDEX);
  } while(0);

  do {
    /* 2. Run FD2: segnet */
    ret = avf_pack_yield_run_info(&req_msg.run_info, YIELD_FD2_INDEX, 20, 1);
    if (ret == -1) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## avf_pack_run_info() fail!");
      ret = DAEMON_NG;
      break;
    }
    req_msg.code = AVFD_CODE_SVC_RUN;
    ret = mq_send(p_handle->req_dpt, (char *)&req_msg, sizeof(req_msg), 0);
    if (ret == -1) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_send() fail!");
      p_handle->state = DAEMON_STATE_OFF;
      ret = DAEMON_NG;
      break;
    }

    ret = mq_receive(p_handle->res_dpt, (char *)&res_msg, sizeof(res_msg), NULL);
    if (ret != sizeof(res_msg)) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_receive() fail, %d", ret);
      ret = DAEMON_NG;
      break;
    }

    if (AVFD_CODE_SVC_RUN == res_msg.code) {
      p_handle->slot = res_msg.slot;
      if (AVFD_RESULT_OK == res_msg.result) {
        ArmLog_OK(ARM_LOG_DAEMONIO, "Flexidag%u running~", p_handle->slot);
        ret = DAEMON_OK;
      } else {
        ArmLog_ERR(ARM_LOG_DAEMONIO, "## AVFD_CODE_SVC_RUN result is NG!!");
        ret = DAEMON_NG;
      }
    } else {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_receive() code mismatch, %u", res_msg.code);
      ret = DAEMON_NG;
    }

    /* Show average proc time */
    {
      avg_proc_time[1] = GetYieldUMsg(p_handle->slot, 0);
      ArmLog_OK(ARM_LOG_DAEMONIO, "[YIELD]FD2 avg proc time: %d(ms)", avg_proc_time[1]/10);
    }

    /* Backup output */
    avf_backup_yield_output(YIELD_FD2_INDEX);
  } while(0);

  do {
    /* 3. Run FD1&2 */
    /* 3.1. Run FD2 on nonblock */
    uint32_t master_slot;
    int32_t  fd1_run_iteration;

    fd2_dag_num = avf_get_dag_split_count(YIELD_FD2_INDEX);
    if (fd2_dag_num <= 0) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## avf_get_dag_split_count() fail!");
      ret = DAEMON_NG;
      break;
    }
    ret = avf_pack_yield_run_info(&req_msg.run_info, YIELD_FD2_INDEX, 20, 1);
    if (ret == -1) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## avf_pack_run_info() fail!");
      ret = DAEMON_NG;
      break;
    }
    req_msg.code = AVFD_CODE_SVC_NONBLOCK_RUN;
    ret = mq_send(p_handle->req_dpt, (char *)&req_msg, sizeof(req_msg), 0);
    if (ret == -1) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_send() fail!");
      p_handle->state = DAEMON_STATE_OFF;
      ret = DAEMON_NG;
      break;
    }

    ret = mq_receive(p_handle->res_dpt, (char *)&res_msg, sizeof(res_msg), NULL);
    if (ret != sizeof(res_msg)) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_receive() fail, %d", ret);
      ret = DAEMON_NG;
      break;
    }

    if (AVFD_CODE_SVC_NONBLOCK_RUN == res_msg.code) {
      p_handle->slot = res_msg.slot;
      master_slot    = res_msg.slot;
      if (AVFD_RESULT_OK == res_msg.result) {
        ArmLog_OK(ARM_LOG_DAEMONIO, "Flexidag%u running~", p_handle->slot);
        ret = DAEMON_OK;
      } else {
        ArmLog_ERR(ARM_LOG_DAEMONIO, "## AVFD_CODE_SVC_NONBLOCK_RUN result is NG!!");
        ret = DAEMON_NG;
      }
    } else {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_receive() code mismatch, %u", res_msg.code);
      ret = DAEMON_NG;
    }

    /* 3.2 Run FD1 */
    fd1_run_iteration = (20 * fd2_dag_num + 500);
    ret = avf_pack_yield_run_info(&req_msg.run_info, YIELD_FD1_INDEX, fd1_run_iteration, 1);
    if (ret == -1) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## avf_pack_run_info() fail!");
      ret = DAEMON_NG;
      break;
    }
    req_msg.code = AVFD_CODE_SVC_RUN;
    ret = mq_send(p_handle->req_dpt, (char *)&req_msg, sizeof(req_msg), 0);
    if (ret == -1) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_send() fail!");
      p_handle->state = DAEMON_STATE_OFF;
      ret = DAEMON_NG;
      break;
    }

    ret = mq_receive(p_handle->res_dpt, (char *)&res_msg, sizeof(res_msg), NULL);
    if (ret != sizeof(res_msg)) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_receive() fail, %d", ret);
      ret = DAEMON_NG;
      break;
    }

    if (AVFD_CODE_SVC_RUN == res_msg.code) {
      p_handle->slot = res_msg.slot;
      if (AVFD_RESULT_OK == res_msg.result) {
        ArmLog_OK(ARM_LOG_DAEMONIO, "Flexidag%u running~", p_handle->slot);
        ret = DAEMON_OK;
      } else {
        ArmLog_ERR(ARM_LOG_DAEMONIO, "## AVFD_CODE_SVC_RUN result is NG!!");
        ret = DAEMON_NG;
      }
    } else {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_receive() code mismatch, %u", res_msg.code);
      ret = DAEMON_NG;
    }

    /* Show average proc time */
    {
      avg_proc_time[2] = GetYieldUMsg(master_slot, 1);
      ArmLog_OK(ARM_LOG_DAEMONIO, "[YIELD]FD2 avg proc time with yield: %d(ms)", avg_proc_time[2]/10);
    }
  } while(0);

  /* 4.1 Check data */
  do {
    ret = avf_pack_yield_run_info(&req_msg.run_info, YIELD_FD1_INDEX, 0, 0);
    if (ret == -1) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## avf_pack_run_info() fail!");
      ret = DAEMON_NG;
      break;
    }

    req_msg.code = AVFD_CODE_SVC_YIELD_CHK;
    ret = mq_send(p_handle->req_dpt, (char *)&req_msg, sizeof(req_msg), 0);
    if (ret == -1) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_send() fail!");
      p_handle->state = DAEMON_STATE_OFF;
      ret = DAEMON_NG;
      break;
    }

    ret = mq_receive(p_handle->res_dpt, (char *)&res_msg, sizeof(res_msg), NULL);
    if (ret != sizeof(res_msg)) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_receive() fail, %d", ret);
      ret = DAEMON_NG;
      break;
    }

    if (AVFD_CODE_SVC_YIELD_CHK == res_msg.code) {
      if (AVFD_RESULT_OK == res_msg.result) {
        ArmLog_OK(ARM_LOG_DAEMONIO, "[YIELD]FD%u Data check: PASS!!!", YIELD_FD1_INDEX + 1);
        ret = DAEMON_OK;
      } else {
        ArmLog_ERR(ARM_LOG_DAEMONIO, "[YIELD]FD%u Data check: NG!!!", YIELD_FD1_INDEX + 1);
        ret = DAEMON_NG;
      }
    } else {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_receive() code mismatch, %u", res_msg.code);
      ret = DAEMON_NG;
    }
  } while (0);

  /* 4.2 Check data */
  do {
    ret = avf_pack_yield_run_info(&req_msg.run_info, YIELD_FD2_INDEX, 0, 0);
    if (ret == -1) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## avf_pack_run_info() fail!");
      ret = DAEMON_NG;
      break;
    }

    req_msg.code = AVFD_CODE_SVC_YIELD_CHK;
    ret = mq_send(p_handle->req_dpt, (char *)&req_msg, sizeof(req_msg), 0);
    if (ret == -1) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_send() fail!");
      p_handle->state = DAEMON_STATE_OFF;
      ret = DAEMON_NG;
      break;
    }

    ret = mq_receive(p_handle->res_dpt, (char *)&res_msg, sizeof(res_msg), NULL);
    if (ret != sizeof(res_msg)) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_receive() fail, %d", ret);
      ret = DAEMON_NG;
      break;
    }

    if (AVFD_CODE_SVC_YIELD_CHK == res_msg.code) {
      if (AVFD_RESULT_OK == res_msg.result) {
        ArmLog_OK(ARM_LOG_DAEMONIO, "[YIELD]FD%u Data check: PASS!!!", YIELD_FD2_INDEX + 1);
        ret = DAEMON_OK;
      } else {
        ArmLog_ERR(ARM_LOG_DAEMONIO, "[YIELD]FD%u Data check: NG!!!", YIELD_FD2_INDEX + 1);
        ret = DAEMON_NG;
      }
    } else {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_receive() code mismatch, %u", res_msg.code);
      ret = DAEMON_NG;
    }
  } while (0);

  /* 5. Check if yield is available */
  if (ret != DAEMON_NG){
    int32_t pass_criteria, pass_upper_bound, pass_lower_bound;
    pass_criteria = avg_proc_time[0] * fd2_dag_num + avg_proc_time[1]; // FD1 * NumOfDag(FD2) + FD2
    pass_lower_bound = (pass_criteria) * YIELD_PASS_LOW_BOUND;
    pass_upper_bound = (pass_criteria) * YIELD_PASS_UP_BOUND;
    pass_lower_bound /= 100;
    pass_upper_bound /= 100;
    ArmLog_OK(ARM_LOG_DAEMONIO, "[YIELD]FD1            : %dms", avg_proc_time[0]/10);
    ArmLog_OK(ARM_LOG_DAEMONIO, "[YIELD]FD2            : %dms", avg_proc_time[1]/10);
    ArmLog_OK(ARM_LOG_DAEMONIO, "[YIELD]FD2 with yield : %dms", avg_proc_time[2]/10);
    ArmLog_OK(ARM_LOG_DAEMONIO, "[YIELD]Pass criteria  : %dms(%d ~ %d)", pass_criteria/10, pass_lower_bound/10, pass_upper_bound/10);
    if ((avg_proc_time[0] > 0) && (avg_proc_time[1] > 0) && (pass_upper_bound > avg_proc_time[2]) && (avg_proc_time[2] > pass_lower_bound)) {
      ArmLog_OK(ARM_LOG_DAEMONIO, "[YIELD]Criteria check: PASS!!!");
    } else {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "[YIELD]Criteria check: NG!!!");
    }
  }

  return ret;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: DaemonIO_GetUnsolicitedMsg
 *
 *  @Description:: Get unsolicited message
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    INT32:       OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int32_t DaemonIO_GetUnsolicitedMsg()
{
  int32_t ret = DAEMON_OK;
  AVF_DAEMON_UNS_MSG_s uns_msg = {0};
  DAEMONIO_HANDLE_s *p_handle = &g_handle;
  uint32_t break_state = 0;

  do {
    struct timespec tm;

    clock_gettime(CLOCK_MONOTONIC, &tm);
    tm.tv_sec += UNS_MSG_TIMEOUT;
    ret = mq_timedreceive_monotonic(p_handle->uns_dpt, (char *)&uns_msg, sizeof(uns_msg), NULL, &tm);

    if (ret == -1) {
      int32_t errorvalue = errno;
      if (errorvalue == ETIMEDOUT) {
        ArmLog_ERR(ARM_LOG_DAEMONIO, "## Hit Daemon TimeOut!!! Please enable daemon log to check.");
      } else {
        ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_receive() fail, errno %d", errorvalue);
      }
      ret = DAEMON_NG;
      break;
    }

    switch (uns_msg.code) {
      case AVFD_UNS_CODE_SLOT_NUM:
        if (uns_msg.result == AVFD_RESULT_NG) {
          break_state = 1;
          ret = DAEMON_NG;
          ArmLog_ERR(ARM_LOG_DAEMONIO, "## Daemon slot are full.");
        } else {
          ArmLog_OK(ARM_LOG_DAEMONIO, "Prepare to run flexidag on Slot-%u!", uns_msg.slot);
        }
        break;
      case AVFD_UNS_CODE_FLEXI_CREATE:
        ArmLog_OK(ARM_LOG_DAEMONIO, "[SLOT%u]Flexidag Creating!", uns_msg.slot);
        break;
      case AVFD_UNS_CODE_FLEXI_START:
        ArmLog_OK(ARM_LOG_DAEMONIO, "[SLOT%u]Flexidag Start!", uns_msg.slot);
        break;
      case AVFD_UNS_CODE_WATCHDOG:
        ArmLog_OK(ARM_LOG_DAEMONIO, "[SLOT%u]Flexidag Running!", uns_msg.slot);
        break;
      case AVFD_UNS_CODE_DONE:
        ArmLog_OK(ARM_LOG_DAEMONIO, "[SLOT%u]Flexidag iterations are done!", uns_msg.slot);
        if (p_handle->slot == uns_msg.slot) {
          break_state = 1;
        }
        break;
      case AVFD_UNS_CODE_WATCHDOG_TIMEOUT:
        /* Script check the key word '|ERR' */
        ArmLog_ERR(ARM_LOG_DAEMONIO, "## Hit Watchdog TimeOut on Slot-%u!", uns_msg.slot);
        if (p_handle->slot == uns_msg.slot) {
          break_state = 1;
        }
        break;
      case AVFD_UNS_CODE_GENERAL_ERR:
        if (uns_msg.result == AVFD_RESULT_READ_NG) {
          ArmLog_ERR(ARM_LOG_DAEMONIO, "## Check READ function on Slot-%u!", uns_msg.slot);
        } else if (uns_msg.result == AVFD_RESULT_FEED_NG) {
          ArmLog_ERR(ARM_LOG_DAEMONIO, "## Check FEED function on Slot-%u!", uns_msg.slot);
        } else if (uns_msg.result == AVFD_RESULT_FLEXI_NG) {
          ArmLog_ERR(ARM_LOG_DAEMONIO, "## Check FLEXI CREATE function on Slot-%u!", uns_msg.slot);
        } else if (uns_msg.result == AVFD_RESULT_GOLDEN_COMPARE_NG) {
          ArmLog_ERR(ARM_LOG_DAEMONIO, "## Check GOLDEN COMPARE function on Slot-%u!", uns_msg.slot);
        } else {
          ArmLog_ERR(ARM_LOG_DAEMONIO, "## Enable Daemon log to check!!");
        }
        if (p_handle->slot == uns_msg.slot) {
          break_state = 1;
        }
        break;
       case AVFD_UNS_CODE_PROC_TIME:
        //ArmLog_OK(ARM_LOG_DAEMONIO, "[SLOT%u] Avg Proc Time: %u", uns_msg.slot, uns_msg.result);
        break;
      default:
        ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_receive() code mismatch, %u, on Slot-%u", uns_msg.code, uns_msg.slot);
        if (p_handle->slot == uns_msg.slot) {
          break_state = 1;
          ret = DAEMON_NG;
        }
    }
  } while(break_state == 0);

  return ret;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: DaemonIO_ShutdownSvc
 *
 *  @Description:: Shutdown AVFD
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    INT32:       OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int32_t DaemonIO_ShutdownSvc()
{
  int32_t ret = DAEMON_OK;
  AVF_DAEMON_REQ_MSG_s req_msg = {0};
  DAEMONIO_HANDLE_s *p_handle = &g_handle;

  do {
    req_msg.code = AVFD_CODE_SVC_CLOSE;
    ret = mq_send(p_handle->req_dpt, (char *)&req_msg, sizeof(req_msg), 0);
    if (ret == -1) {
      ArmLog_ERR(ARM_LOG_DAEMONIO, "## mq_send() fail!");
      p_handle->state = DAEMON_STATE_OFF;
      ret = DAEMON_NG;
      break;
    }
  } while(0);

  return ret;
}


/*---------------------------------------------------------------------------*\
 * Daemon Side APIs
\*---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: DaemonIO_GetCmaInfo
 *
 *  @Description:: Get memory usage information of CMA
 *
 *  @Input      ::
 *    p_usedSize:  Pointer to CMA info
 *    p_freeSize:  Pointer to CMA info
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
uint32_t DaemonIO_GetCmaInfo(uint32_t *p_usedSize, uint32_t *p_freeSize)
{
  uint32_t Rval = ARM_OK;
  DAEMONIO_HANDLE_s *p_handle = &g_handle;
  AVFD_MEM_HANDLE_s *p_mem_handle = &g_mem_handle;

  do {
    if (DAEMON_STATE_ON == p_handle->state) {

      /* Sanity check */
      if ((NULL == p_usedSize) || (NULL == p_freeSize)) {
        syslog(LOG_ERR, "## Null pointer!");
        Rval = ARM_NG;
        break;
      }

      ArmMutex_Take(&(p_mem_handle->mutex));
      *p_usedSize = p_mem_handle->total_size - p_mem_handle->avail_size;/* Used */
      *p_freeSize = p_mem_handle->avail_size; /* Free */
      ArmMutex_Give(&(p_mem_handle->mutex));
      syslog(LOG_NOTICE, "[CMA]Used: %luB, Free: %luB", *p_usedSize, *p_freeSize);
      syslog(LOG_NOTICE, "[CMA]Used: %luMB Free: %luMB", (*p_usedSize)/(1024*1024), (*p_freeSize)/(1024*1024));

    } else { /* DAEMON_STATE_OFF */
      Rval = ArmMem_GetMemUsage(p_usedSize, p_freeSize);
    }

  } while(0);

  return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: DaemonIO_AllocateCma
 *
 *  @Description:: Allocate buffer
 *
 *  @Input      ::
 *    req_size:    The requested buffer size
 *
 *  @Output     ::
 *    p_buf:     Pointer to the requested buffer
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
uint32_t DaemonIO_AllocateCma(uint32_t req_size, flexidag_memblk_t *p_buf)
{
  uint32_t Rval = ARM_OK;
  DAEMONIO_HANDLE_s *p_handle = &g_handle;
  AVFD_MEM_HANDLE_s *p_mem_handle = &g_mem_handle;

  do {
    if (DAEMON_STATE_ON == p_handle->state) {

      /* Sanity check */
      if (NULL == p_buf) {
        syslog(LOG_ERR, "## Null pointer!");
        Rval = ARM_NG;
        break;
      }

      ArmMutex_Take(&(p_mem_handle->mutex));
      if ((p_mem_handle->avail_size >= ALIGN4K(req_size)) && (req_size != 0)) {
        Rval = ArmMemPool_Allocate(p_mem_handle->pool_id, req_size, p_buf);
        if (Rval != ARM_OK) {
          syslog(LOG_ERR, "## CMA_REQ: ArmMemPool_Allocate() fail!");
          Rval = ARM_NG;
        } else {
          p_mem_handle->avail_size -= p_buf->buffer_size;
        }
      } else {
        syslog(LOG_ERR, "## CMA_REQ: Avail, Req ( %u, %u)", p_mem_handle->avail_size,  ALIGN4K(req_size));
        Rval = ARM_NG;
      }
      ArmMutex_Give(&(p_mem_handle->mutex));
    } else { /* DAEMON_STATE_OFF */
      Rval = ArmMem_Allocate(req_size, 1, p_buf);
    }

  } while(0);

  return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: DaemonIO_FreeCma
 *
 *  @Description:: Free buffer
 *
 *  @Input      ::
 *    pReqBuf:     Pointer to the buffer
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
uint32_t DaemonIO_FreeCma(flexidag_memblk_t *p_buf)
{
  uint32_t Rval = ARM_OK;
  DAEMONIO_HANDLE_s *p_handle = &g_handle;
  AVFD_MEM_HANDLE_s *p_mem_handle = &g_mem_handle;

  do {
    if (DAEMON_STATE_ON == p_handle->state) {
      uint32_t free_size;

      /* Sanity check */
      if (NULL == p_buf) {
        syslog(LOG_ERR, "## Null pointer!");
        Rval = ARM_NG;
        break;
      }

      free_size = p_buf->buffer_size;

      ArmMutex_Take(&(p_mem_handle->mutex));
      Rval = ArmMemPool_Free(p_mem_handle->pool_id, p_buf);
      if (Rval != ARM_OK) {
        syslog(LOG_ERR, "## CMA_FREE: ArmMemPool_Free() fail!");
        Rval = ARM_NG;
      } else {
        p_mem_handle->avail_size += free_size;
        if (p_mem_handle->avail_size > p_mem_handle->total_size) {
          syslog(LOG_ERR, "## CMA_FREE: Avail > Total (%u, %u)", p_mem_handle->avail_size , p_mem_handle->total_size);
          Rval = ARM_NG;
        }
      }
      ArmMutex_Give(&(p_mem_handle->mutex));

    } else { /* DAEMON_STATE_OFF */
      Rval = ArmMem_Free(p_buf);
    }

  } while(0);

  return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: DaemonIO_InitMem
 *
 *  @Description:: Init Mem for Deamon
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    INT32:       OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
uint32_t DaemonIO_InitMem()
{
  uint32_t Rval = ARM_OK;
  AVFD_MEM_HANDLE_s *p_mem_handle = &g_mem_handle;
  DAEMONIO_HANDLE_s *p_handle = &g_handle;

  do {
    {
      uint32_t UsedSize, FreeSize;

      (void) ArmMem_GetMemUsage(&UsedSize, &FreeSize);
      /* Reserved for CMA page header */
      if ((FreeSize == 0) || (FreeSize <= RESERVED_CMA_SIZE)) {
        ArmLog_ERR(ARM_LOG_AVFD, "  ## no available CMA");
        Rval = ARM_NG;
        break;
      }
      /* Check if mem is used for some module */
      if (UsedSize != 0) {
        ArmLog_WARN(ARM_LOG_AVFD, "Some modules have allocated CMA!");
      }

      /* Init mem handle mutex */
      {
        char MutexName[] = "DaemioIO_Mem_Mut";
        (void) ArmMutex_Create(&(p_mem_handle->mutex), MutexName);
      }

      /* Init mem handle */
      p_mem_handle->total_size = ALIGN4K(FreeSize - RESERVED_CMA_SIZE);
      p_mem_handle->avail_size = p_mem_handle->total_size;

      Rval = ArmMem_Allocate(p_mem_handle->total_size, 1, &p_mem_handle->memblk);
      if (Rval != ARM_OK) {
        ArmLog_ERR(ARM_LOG_AVFD, "  ## fail to ArmMem_Allocate()");
        Rval = ARM_NG;
        break;
      }

      /* Create mem pool for management */
      Rval = ArmMemPool_Create(&p_mem_handle->memblk, &p_mem_handle->pool_id);
      if (Rval != ARM_OK) {
        ArmLog_ERR(ARM_LOG_AVFD, "  ## fail to ArmMemPool_Create()");
        Rval = ARM_NG;
        break;
      }
    }
  } while(0);

  if (Rval == ARM_OK) {
    p_handle->state = DAEMON_STATE_ON;
  }

  return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: DaemonIO_DeInitMem
 *
 *  @Description:: DeInit Mem for Deamon
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    INT32:       OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
uint32_t DaemonIO_DeInitMem()
{
  uint32_t Rval = ARM_OK;
  AVFD_MEM_HANDLE_s *p_mem_handle = &g_mem_handle;

  (void) ArmMemPool_Delete(p_mem_handle->pool_id);
  (void) ArmMem_Free(&p_mem_handle->memblk);
  (void) ArmMutex_Delete(&(p_mem_handle->mutex));

  return Rval;
}

