/**
 *  @file AmbaIP_Monitor.h
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Definitions & Constants for IP Monitor APIs
 *
 */
#ifndef AMBA_IPMONITOR_H
#define AMBA_IPMONITOR_H

#include "Amba_IPMonitor_Def.h"

UINT32 AmbaIP_MonitorInit(void);

UINT32 AmbaIP_GetMonitorCfg(UINT32 ModuleId, AMBA_IP_MONITOR_CONFIG_s *pMonCfg);

UINT32 AmbaIP_MonitorAdjCmpr(const AMBA_IP_MONITOR_ADJ_CMPR_s *pAdjCmpr);

UINT32 AmbaIP_MonitorAeCmpr(const AMBA_IP_MONITOR_AE_CMPR_s *pAeCmpr);

UINT32 AmbaIP_MonitorAwbCmpr(const AMBA_IP_MONITOR_AWB_CMPR_s *pAwbCmpr);

UINT32 AmbaIP_MonitorHeartBeatCfg(const AMBA_IP_MONITOR_HEARTBEAT_s *pHeartBeatCfg);

UINT32 AmbaIP_MonitorErrorNotify(const AMBA_IP_MONITOR_ERR_NOTIFY_s *pErrNotify);

UINT32 Ipc_Test_Command(INT32 argc, char *const*argv);

#endif  /* AMBA_IP_MONITOR_H */
