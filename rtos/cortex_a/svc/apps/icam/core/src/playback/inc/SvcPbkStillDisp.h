/**
*  @file SvcPbkStillDisp.h
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
*  @details svc still display related APIs
*
*/

#ifndef SVC_PBK_STILL_DISP_H
#define SVC_PBK_STILL_DISP_H

#include "SvcPbkPictDisp.h"

#define STILL_DISP_VOUT_FRAME_NUM       2U
#define STILL_DISP_EVENT_DISP_TO_VOUT   (0x00000010U)

#define STILL_DISP_NEXT_FILE    0U
#define STILL_DISP_PREV_FILE    1U

#define STILL_DISP_MSG_ENTRY_NUM        1U
#define STILL_DISP_SEM_INIT_COUNT       1U

#define STILL_DISP_MSG_SINGLE_FRAME     0x1U
#define STILL_DISP_MSG_SLIDING          0x2U

#define STILL_DISP_FLAG_IDLE            (1U)
#define STILL_DISP_FLAG_ENABLE          (2U)
#define STILL_DISP_FLAG_DONE            (4U)

#define STILL_DISP_MAX_IMG_PER_FRAME     12U

#define SVC_STILL_DISP_STACK_SIZE               (0x8000U)
#define SVC_PBK_STILL_DISP_TASK_PRI             (62U)
#define SVC_PBK_STILL_DISP_TASK_CPU_BITS        (0x01U)

typedef struct {
    ULONG  SrcBufYBase;
    ULONG  SrcBufUVBase;
    UINT32 SrcBufPitch;
    UINT32 SrcBufHeight;
    UINT32 SrcOffsetX;
    UINT32 SrcOffsetY;
    UINT32 SrcWidth;
    UINT32 SrcHeight;
    UINT32 SrcChromaFmt;

    UINT32 RotateFlip;
    UINT32 DstOffsetX;
    UINT32 DstOffsetY;
    UINT32 DstWidth;
    UINT32 DstHeight;
} PBK_STILL_DISP_PARAM_s;

typedef struct {
    UINT32                 VoutIdx;
    UINT32                 FlushVoutBuf;
    UINT32                 NumImg;
    PBK_STILL_DISP_PARAM_s *pParam;
} PBK_STILL_DISP_CONFIG_s;

typedef struct {
    UINT32                      MsgQueue[STILL_DISP_MSG_ENTRY_NUM];
    AMBA_KAL_MSG_QUEUE_t        MsgQueueId;
    AMBA_KAL_EVENT_FLAG_t       EventId;
    AMBA_KAL_SEMAPHORE_t        VoutBufSemId[AMBA_DSP_MAX_VOUT_NUM];
    UINT32                      NumVout;
    UINT32                      CurFrmBufIndex[AMBA_DSP_MAX_VOUT_NUM];
    ULONG                       CurVoutYAddr[AMBA_DSP_MAX_VOUT_NUM];
    PBK_STILL_DISP_CONFIG_s     DispConfig[AMBA_DSP_MAX_VOUT_NUM];
    PBK_STILL_DISP_PARAM_s      DispParam[AMBA_DSP_MAX_VOUT_NUM][STILL_DISP_MAX_IMG_PER_FRAME];
    UINT32                      SlideDir;
    UINT32                      PendingDispNum;
} PBK_STILL_DISP_MGR_s;

typedef struct {
    ULONG  YBase;
    ULONG  UVBase;
    UINT32 Width;
    UINT32 Height;
    UINT32 Pitch;
    UINT32 BufAR;
} PBK_STILL_DISP_VOUT_FRM_BUF_s;

typedef struct {
    UINT32  VoutId;
    UINT32  Width;
    UINT32  Pitch;
    UINT32  Height;
    UINT32  VoutAR;
    UINT32  RotateFlip;
    UINT32  FrmBufNum;
    PBK_STILL_DISP_VOUT_FRM_BUF_s VoutBuf[STILL_DISP_VOUT_FRAME_NUM];
} PBK_STILL_DISP_VOUT_INFO_s;

typedef struct {
    PBK_STILL_DISP_VOUT_INFO_s      VoutInfo[AMBA_DSP_MAX_VOUT_NUM];
    AMBA_KAL_EVENT_FLAG_t           *pFbEventId;
} PBK_STILL_DISP_CTRL_s;

typedef struct {
    UINT32 VoutIdx;
    ULONG  YBufAddr;
    ULONG  UVBufAddr;
    UINT32 Pitch;
    UINT32 Height;
    UINT32 StartX;
    UINT32 StartY;
    UINT32 RectWidth;
    UINT32 RectHeight;
    UINT32 LineWidth;
    UINT8 Y;
    UINT8 U;
    UINT8 V;
} PBK_STILL_DISP_DRAW_RECT_s;

UINT32 SvcPbkStillDisp_TaskCreate(void);
UINT32 SvcPbkStillDisp_TaskDelete(void);
UINT32 SvcPbkStillDisp_Setup(AMBA_KAL_EVENT_FLAG_t *pFeedbackEventID, UINT32 DispNum, const PBK_STILL_DISP_VOUT_INFO_s *pDispInfo);
UINT32 SvcPbkStillDisp_SingleFrame(UINT32 NumVout, const PBK_STILL_DISP_CONFIG_s *pDispConfig);
void SvcPbkStillDisp_GetCurrentVoutBuf(UINT32 VoutIdx, PBK_STILL_DISP_VOUT_FRM_BUF_s *pVoutBuf);
void SvcPbkStillDisp_WaitDspDispIdle(void);
void SvcPbkStillDisp_RescaleFrmToVoutBuf(UINT32 NumVout, const PBK_STILL_DISP_CONFIG_s *pDispConfig);
void SvcPbkStillDisp_FlushVoutBuf(UINT32 NumVout, const PBK_STILL_DISP_CONFIG_s *pDispConfig);
void SvcPbkStillDisp_DrawRectangle(const PBK_STILL_DISP_DRAW_RECT_s *pRectInfo);

#endif  /* SVC_PBK_STILL_DISP_H */
