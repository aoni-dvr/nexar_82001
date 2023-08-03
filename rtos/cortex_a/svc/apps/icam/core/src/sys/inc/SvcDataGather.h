/**
*  @file SvcDataGather.h
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
*  @details svc data gather
*
*/

#ifndef SVC_DATA_GATHER_H
#define SVC_DATA_GATHER_H

#if defined(CONFIG_ICAM_DATA_GATHER_USED)

#include "AmbaRscData.h"
#if defined(CONFIG_BUILD_AMBA_ADAS)
#include "AmbaSurround.h"
#endif

#define SVC_DATA_GATHER_FILE_WRITABLE  (1U)

typedef void (*AMBA_DATG_READY_CB)(UINT16 CtrlId, AMBA_DATG_s *pDataG);

UINT32 SvcDataGather_Create(UINT32 Priority, UINT32 CpuBits);
UINT32 SvcDataGather_Destroy(void);
UINT32 SvcDataGather_Register(UINT16 *pCtrlId, AMBA_DATG_READY_CB pDataReadyCB);
UINT32 SvcDataGather_UnRegister(UINT16 CtrlId);
void   SvcDataGather_StartFetch(UINT16 CtrlId, UINT16 TickType, UINT32 RecStrmIdx, UINT64 SyncTime);
void   SvcDataGather_StopFetch(UINT16 CtrlId);
#if defined(CONFIG_BUILD_AMBA_ADAS)
UINT32 SvcDataGather_GetCanData(UINT16 TickType, UINT64 SyncTime, AMBA_SR_CANBUS_TRANSFER_DATA_s *pData);
#endif
void   SvcDataGather_Dump(AMBA_SHELL_PRINT_f PrintFunc);
void   SvcDataGather_FileFeedReset(void);
void   SvcDataGather_FileFeedData(const AMBA_DATG_s *pData);
void   SvcDataGather_FileFeedStaus(UINT32 *pStatus);

#endif

#endif  /* SVC_DATA_GATHER_H */