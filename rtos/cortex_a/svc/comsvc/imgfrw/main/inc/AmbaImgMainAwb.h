/**
 *  @file AmbaImgMainAwb.h
 *
 *  Copyright (c) [2020] Ambarella International LP
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
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
 *  @details Constants and Definitions for Amba Image Main Awb
 *
 */

#ifndef AMBA_IMG_MAIN_AWB_H
#define AMBA_IMG_MAIN_AWB_H

#ifdef CONFIG_BUILD_IMGFRW_EFOV
typedef struct /*_AMBA_IMG_MAIN_AWB_EFOV_WB_s_*/ {
    AMBA_IK_WB_GAIN_s    Ctx;
} AMBA_IMG_MAIN_AWB_EFOV_WB_s;

typedef struct /*_AMBA_IMG_MAIN_AWB_EFOV_USER_s_*/ {
    AMBA_IMG_MAIN_AWB_EFOV_WB_s       WbInfo;
} AMBA_IMG_MAIN_AWB_EFOV_USER_s;

typedef struct /*_AMBA_IMG_MAIN_AWB_EFOV_TXRX_s_*/ {
    UINT64                           Msg;
    AMBA_IMG_MAIN_AWB_EFOV_USER_s    User;
} AMBA_IMG_MAIN_AWB_EFOV_TXRX_s;
#endif

typedef enum /*_AMBA_IMG_MAIN_AWB_CMD_e_*/ {
    AWB_TASK_CMD_STOP = 0,
    AWB_TASK_CMD_START,
    AWB_TASK_CMD_LOCK,
    AWB_TASK_CMD_LOCK_PROC,
    AWB_TASK_CMD_LOCK_DOWN,
    AWB_TASK_CMD_UNLOCK
} AMBA_IMG_MAIN_AWB_CMD_e;

extern UINT32 AmbaImgMainAwb_DebugCount[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];

UINT32 AmbaImgMainAwb_Init(void);
UINT32 AmbaImgMainAwb_Create(UINT32 VinId, UINT32 Priority, UINT32 CoreInclusion);
UINT32 AmbaImgMainAwb_Delete(UINT32 VinId);
UINT32 AmbaImgMainAwb_Active(UINT32 VinId);
UINT32 AmbaImgMainAwb_Inactive(UINT32 VinId);
UINT32 AmbaImgMainAwb_IdleWait(UINT32 VinId);
UINT32 AmbaImgMainAwb_Lock(UINT32 VinId);
UINT32 AmbaImgMainAwb_LockWait(UINT32 VinId);
UINT32 AmbaImgMainAwb_Unlock(UINT32 VinId);

UINT32 AmbaImgMainAwb_Snap(UINT32 VinId, UINT32 ChainId, UINT32 SnapCount, const UINT32 *pIsHiso);
UINT32 AmbaImgMainAwb_SnapDspWgc(UINT32 VinId, UINT32 ChainId, const AMBA_IK_CFA_3A_DATA_s *pCfa, UINT32 IsPiv, UINT32 IsHiso);

#endif  /* AMBA_IMG_MAIN_AWB_H */
