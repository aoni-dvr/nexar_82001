/**
*  @file main.c
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

#include "avf.h"
#include "avf_daemon.h"
#include "ArmLog.h"

#define ARM_LOG_MAIN       "MAIN"

int32_t main(int32_t argc, char **argv)
{
  int32_t ret = DAEMON_OK;

  /* Parse I/O name */
  avf_parse_opt(argc, argv);
  if (g_avf_config.start_daemon != 0) {
    ArmLog_DBG(ARM_LOG_MAIN, "Start AVFD...");
    avf_daemon_start(g_avf_config.debug_daemon);
  } else if (g_avf_config.run_client != 0) {
    ArmLog_DBG(ARM_LOG_MAIN, "Run flexidag with AVFD...");
    do {
      ret = DaemonIO_Init();
      if (ret == DAEMON_NG) {
        ArmLog_ERR(ARM_LOG_MAIN, "## DaemonIO_Init() fail");
        break;
      }
      ret = DaemonIO_CheckSvc();
      if (ret == DAEMON_NG) {
        ArmLog_ERR(ARM_LOG_MAIN, "## DaemonIO_CheckSvc() fail");
        break;
      }
      ret = DaemonIO_RunFlexidag();
      if (ret == DAEMON_NG) {
        ArmLog_ERR(ARM_LOG_MAIN, "## DaemonIO_RunFlexidag() fail");
        break;
      }
      ret = DaemonIO_GetUnsolicitedMsg();
      if (ret == DAEMON_NG) {
        ArmLog_ERR(ARM_LOG_MAIN, "## DaemonIO_GetUnsolicitedMsg() fail");
        break;
      }
      ret = DaemonIO_DeInit();
      if (ret == DAEMON_NG) {
        ArmLog_ERR(ARM_LOG_MAIN, "## DaemonIO_DeInit() fail");
        break;
      }
    } while(0);
  } else if (g_avf_config.run_client_nonblock != 0) {
    ArmLog_DBG(ARM_LOG_MAIN, "Run flexidag with AVFD...");
    do {
      ret = DaemonIO_Init();
      if (ret == DAEMON_NG) {
        ArmLog_ERR(ARM_LOG_MAIN, "## DaemonIO_Init() fail");
        break;
      }
      ret = DaemonIO_CheckSvc();
      if (ret == DAEMON_NG) {
        ArmLog_ERR(ARM_LOG_MAIN, "## DaemonIO_CheckSvc() fail");
        break;
      }
      ret = DaemonIO_RunNonBlock();
      if (ret == DAEMON_NG) {
        ArmLog_ERR(ARM_LOG_MAIN, "## DaemonIO_RunNonBlock() fail");
        break;
      }
      ret = DaemonIO_DeInit();
      if (ret == DAEMON_NG) {
        ArmLog_ERR(ARM_LOG_MAIN, "## DaemonIO_DeInit() fail");
        break;
      }
    } while(0);
  } else if (g_avf_config.test_yield != 0) {
    ArmLog_DBG(ARM_LOG_MAIN, "Test Yield...");
    do {
      ret = DaemonIO_Init();
      if (ret == DAEMON_NG) {
        ArmLog_ERR(ARM_LOG_MAIN, "## DaemonIO_Init() fail");
        break;
      }
      ret = DaemonIO_CheckSvc();
      if (ret == DAEMON_NG) {
        ArmLog_ERR(ARM_LOG_MAIN, "## DaemonIO_CheckSvc() fail");
        break;
      }
      ret = DaemonIO_TestYield();
      if (ret == DAEMON_NG) {
        ArmLog_ERR(ARM_LOG_MAIN, "## DaemonIO_TestYield() fail");
        break;
      }
      ret = DaemonIO_DeInit();
      if (ret == DAEMON_NG) {
        ArmLog_ERR(ARM_LOG_MAIN, "## DaemonIO_DeInit() fail");
        break;
      }
    } while(0);
  } else if (g_avf_config.stop_daemon != 0) {
    ArmLog_DBG(ARM_LOG_MAIN, "Stop AVFD...");
    ret = DaemonIO_Init();
    DaemonIO_ShutdownSvc();
    DaemonIO_DeInit();
  } else {
    ArmLog_ERR(ARM_LOG_MAIN, "## Run flexidag(with non-AVFD)...doesn't support it");
  }

  return EXIT_SUCCESS;
}

