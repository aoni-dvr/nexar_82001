/**
 *  @file SvcVoutFrmCtrlTask.h
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
 *  @details svc vout frame control task header file
 *
 */

#ifndef SVC_VOUT_FRM_CTRL_TASK_H
#define SVC_VOUT_FRM_CTRL_TASK_H

#include "AmbaTypes.h"

/* SyncMode */
#define SVC_VFC_TASK_CONT_DELAY                     (0x0U)
#define SVC_VFC_TASK_SYNC_CAP_SEQ_AND_OSD           (0x1U)   /* Sync VOUT with OSD flush event*/
#define SVC_VFC_TASK_SYNC_CAP_SEQ_AND_GUI_TASK      (0x2U)   /* Sync VOUT with OSD flush event, and use GUI draw start to drive image feeder */

typedef struct {
    UINT32 YuvStrmIdx;      /* Index of DispStrm[] in resolution config */
    UINT8  DlyCount;
    UINT8  SyncMode;
} SVC_VOUT_FRM_CTRL_CFG_s;

typedef struct {
    UINT32                  SyncMode;
    UINT64                  CapPts;
    UINT64                  CapSequence;
    ULONG                   BaseAddrY;
    ULONG                   BaseAddrUV;
} SVC_LIV_FRM_INFO_s;

UINT32 SvcVoutFrmCtrlTask_Init(void);
UINT32 SvcVoutFrmCtrlTask_Config(const SVC_VOUT_FRM_CTRL_CFG_s *pCfg);
UINT32 SvcVoutFrmCtrlTask_Start(void);

#define SVC_VFC_TASK_CMD_CAP_SEQ          ("cap_seg_info")
#define SVC_VFC_TASK_CMD_VOUT_UPDATE      ("vout_update")
UINT32 SvcVoutFrmCtrlTask_Ctrl(const char *pCmd, void* pInfo);

void SvcVoutFrmCtrlTask_DebugEnable(UINT32 DebugEnable);

#endif /* SVC_VOUT_FRM_CTRL_TASK_H */