/**
*  @file avf_daemon.h
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
#ifndef AVF_DAEMON_H
#define AVF_DAEMON_H

#include "cvapi_ambacv_flexidag.h"
#include "avf.h"                    /* for struct AVF_RUN_INFO_s */

#define AVF_DAEMON_REQUEST_QUE_NAME       "/AVF_DAEMON_REQ_Q"  /* Client to AVFD */
#define AVF_DAEMON_RESPONSE_QUE_NAME      "/AVF_DAEMON_RES_Q"  /* AVFD to Client */
#define AVF_DAEMON_UNSOLICITED_QUE_NAME   "/AVF_DAEMON_UNS_Q"  /* AVFD to Client */

#define AVFD_CODE_SVC_CHK           (1)   /* Check if daemon is ready or not */
#define AVFD_CODE_SVC_CLOSE         (5)   /* Terminate daemon */
#define AVFD_CODE_SVC_RUN           (6)   /* RUN flexidag  */
#define AVFD_CODE_SVC_NONBLOCK_RUN  (7)   /* Non-block run on Daemon Side */
#define AVFD_CODE_SVC_YIELD_CHK     (8)   /* Check YIELD output data */

#define AVFD_UNS_CODE_WATCHDOG          (1)
#define AVFD_UNS_CODE_DONE              (2)
#define AVFD_UNS_CODE_FLEXI_CREATE      (3)
#define AVFD_UNS_CODE_FLEXI_START       (4)
#define AVFD_UNS_CODE_SLOT_NUM          (5)
#define AVFD_UNS_CODE_WATCHDOG_TIMEOUT  (6)
#define AVFD_UNS_CODE_GENERAL_ERR       (7)
#define AVFD_UNS_CODE_PROC_TIME         (8)

#define AVFD_RESULT_OK                   (1)
#define AVFD_RESULT_NG                  (-1)
#define AVFD_RESULT_READ_NG             (-2)
#define AVFD_RESULT_FEED_NG             (-3)
#define AVFD_RESULT_FLEXI_NG            (-4)
#define AVFD_RESULT_GOLDEN_COMPARE_NG   (-5)

typedef struct _AVF_DAEMON_REQ_MSG {
  uint32_t        code;
  AVF_RUN_INFO_s  run_info;
} AVF_DAEMON_REQ_MSG_s;

typedef struct _AVF_DAEMON_RES_MSG {
  uint32_t          code;
  uint32_t          slot;
  int32_t           result;
} AVF_DAEMON_RES_MSG_s;

typedef struct _AVF_DAEMON_UNS_MSG {
  uint32_t          code;
  uint32_t          slot;
  int32_t           result;
} AVF_DAEMON_UNS_MSG_s;


void avf_daemon_start(uint32_t debug_f);
void avf_daemon_unsolicited_msg(uint32_t code, uint32_t slot, int32_t result);

#endif
