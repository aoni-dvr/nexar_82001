/**
 *  @file AmbaImgMainAdj.h
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
 *  @details Constants and Definitions for Amba Image Main Adj
 *
 */

#ifndef AMBA_IMG_MAIN_ADJ_H
#define AMBA_IMG_MAIN_ADJ_H

#ifdef CONFIG_BUILD_IMGFRW_EFOV
#define AMBA_IMG_ADJ_MAX_EXPOSURE_NUM    AMBA_IMG_SENSOR_HAL_HDR_SIZE

typedef struct /*_AMBA_IMG_MAIN_ADJ_EFOV_HISTO_s_*/ {
    UINT32 BinY[AMBA_IK_CFA_HISTO_COUNT];
} AMBA_IMG_MAIN_ADJ_EFOV_HISTO_s;

typedef struct /*_AMBA_IMG_MAIN_ADJ_EFOV_STAT_s_*/ {
    UINT32                            RawCapSeq;
    AMBA_IMG_MAIN_ADJ_EFOV_HISTO_s    Histo;
} AMBA_IMG_MAIN_ADJ_EFOV_STAT_s;

typedef struct /*_AMBA_IMG_MAIN_ADJ_EFOV_WB_s_*/ {
    AMBA_IK_WB_GAIN_s    Ctx[AMBA_IMG_ADJ_MAX_EXPOSURE_NUM];
} AMBA_IMG_MAIN_ADJ_EFOV_WB_s;

typedef struct /*_AMBA_IMG_MAIN_ADJ_EFOV_AE_s_*/ {
    AMBA_AE_INFO_s    Ctx[AMBA_IMG_ADJ_MAX_EXPOSURE_NUM];
} AMBA_IMG_MAIN_ADJ_EFOV_AE_s;

typedef struct /*_AMBA_IMG_MAIN_ADJ_EFOV_USER_s_*/ {
    AMBA_IMG_MAIN_ADJ_EFOV_AE_s      AeInfo;
    AMBA_IMG_MAIN_ADJ_EFOV_WB_s      WbInfo;
    AMBA_IMG_MAIN_ADJ_EFOV_STAT_s    AaaStat;
} AMBA_IMG_MAIN_ADJ_EFOV_USER_s;

typedef struct /*_AMBA_IMG_MAIN_ADJ_EFOV_TXRX_s_*/ {
    UINT64                           Msg;
    AMBA_IMG_MAIN_ADJ_EFOV_USER_s    User;
} AMBA_IMG_MAIN_ADJ_EFOV_TXRX_s;

typedef struct /*_AMBA_IMG_MAIN_ADJ_EFOV_RAW_s_*/ {
    UINT32 mCapSeq;
    UINT32 sCapSeq;
} AMBA_IMG_MAIN_ADJ_EFOV_RAW_s;

#define AMBA_IMG_MAIN_ADJ_EFOV_STAT_NUM  4U

typedef struct /*_AMBA_IMG_MAIN_ADJ_EFOV_RING_s_*/ {
    const void *pCfa[AMBA_IMG_MAIN_ADJ_EFOV_STAT_NUM];
    const void *pRgb[AMBA_IMG_MAIN_ADJ_EFOV_STAT_NUM];
} AMBA_IMG_MAIN_ADJ_EFOV_RING_s;

typedef struct /*_AMBA_IMG_MAIN_ADJ_EFOV_INFO_s_*/ {
    AMBA_KAL_MUTEX_t                 Mutex;
    AMBA_IMG_MAIN_ADJ_EFOV_RAW_s     Raw;
    AMBA_IMG_MAIN_ADJ_EFOV_RING_s    Ring;
} AMBA_IMG_MAIN_ADJ_EFOV_INFO_s;
#endif

typedef enum /*_AMBA_IMG_MAIN_ADJ_CMD_e_*/ {
    ADJ_TASK_CMD_STOP = 0,
    ADJ_TASK_CMD_START,
    ADJ_TASK_CMD_LOCK,
    ADJ_TASK_CMD_LOCK_PROC,
    ADJ_TASK_CMD_LOCK_DOWN,
    ADJ_TASK_CMD_UNLOCK
} AMBA_IMG_MAIN_ADJ_CMD_e;

extern UINT32 AmbaImgMainAdj_DebugCount[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];

UINT32 AmbaImgMainAdj_Init(void);
UINT32 AmbaImgMainAdj_Create(UINT32 VinId, UINT32 Priority, UINT32 CoreInclusion);
UINT32 AmbaImgMainAdj_Delete(UINT32 VinId);
UINT32 AmbaImgMainAdj_Active(UINT32 VinId);
UINT32 AmbaImgMainAdj_Inactive(UINT32 VinId);
UINT32 AmbaImgMainAdj_IdleWait(UINT32 VinId);
UINT32 AmbaImgMainAdj_Lock(UINT32 VinId);
UINT32 AmbaImgMainAdj_LockWait(UINT32 VinId);
UINT32 AmbaImgMainAdj_Unlock(UINT32 VinId);

UINT32 AmbaImgMainAdj_Snap(UINT32 VinId, UINT32 ChainId, UINT32 SnapCount, const UINT32 *pIsHiso);
UINT32 AmbaImgMainAdj_SnapDspFilter(UINT32 VinId, UINT32 ChainId, const AMBA_IK_CFA_3A_DATA_s *pCfa, UINT32 IsPiv, UINT32 IsHiso);

#endif  /* AMBA_IMG_MAIN_ADJ_H */
