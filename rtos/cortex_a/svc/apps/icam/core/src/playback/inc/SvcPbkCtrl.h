/**
*  @file SvcPbkCtrl.h
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
*  @details svc playback control
*
*/

#ifndef SVC_PBK_CTRL_H
#define SVC_PBK_CTRL_H

#include "AmbaStreamWrap.h"
#include "AmbaPlayer.h"

typedef struct {
    UINT32      TaskPriority;
    UINT32      TaskCpuBits;
} SVC_PBK_CTRL_TASK_INFO_s;

typedef struct {
    UINT8               VoutIdx;             /* Vout index */
    UINT8               VoutRotateFlip;      /* Vout rotate and flip setting */
    AMBA_DSP_WINDOW_s   VoutWindow;          /* Vout video window */
    UINT32              VoutWindowAR;        /* Vout video window aspect ratio */
    UINT32              IsInterlace;
} SVC_PBK_CTRL_VOUT_INFO_s;

typedef struct {
    /* fill by user */
    char                           Drive;
#define SVC_PBK_CTRL_MODE_PBK      (0U)  /* playback mode */
#define SVC_PBK_CTRL_MODE_DPX      (1U)  /* duplex mode */
    UINT32                         DspMode;
    UINT32                         VoutNum;
    SVC_PBK_CTRL_VOUT_INFO_s       VoutInfo[AMBA_DSP_MAX_VOUT_NUM];
    SVC_PBK_CTRL_TASK_INFO_s       PbkTaskInfo;
    UINT32                         FeedBits; /* feed the decoder output yuv to other viewzones */

    /* inner control */
    UINT32                         FileIndex;
    UINT32                         Loop;
    AMBA_PLAYER_s                  *pPlayer;
} SVC_PBK_CTRL_INFO_s;

typedef struct {
    char                           *FileName;
    UINT32                         FileIndex;
#define SVC_PBK_CTRL_IDX_LATEST    (0U)

    UINT32                         Loop;
#define SVC_PBK_CTRL_LOOP_NONE     (0U)
#define SVC_PBK_CTRL_LOOP_ONE_FILE (1U)
#define SVC_PBK_CTRL_LOOP_ALL      (2U)

    UINT32                         Seamless;
    UINT32                         Speed;
    UINT32                         Direction;
    UINT32                         StartTime;
} SVC_PBK_CTRL_CREATE_s;

typedef struct {
    char                           *FileName;
    UINT32                         FileIndex;
} SVC_PBK_CTRL_DEC_FRAME_s;

void   SvcPbkCtrl_InfoGet(UINT32 PlayerId, SVC_PBK_CTRL_INFO_s **ppInfo);
UINT32 SvcPbkCtrl_TaskCreate(UINT32 PlayerId, const SVC_PBK_CTRL_CREATE_s *pCreate);
UINT32 SvcPbkCtrl_TaskDelete(UINT32 PlayerId);
UINT32 SvcPbkCtrl_PostCtrl(UINT32 PlayerId, AMBA_PLAYER_DISP_CTRL_s *pDisp);

UINT32 SvcPbkCtrl_Start(UINT32 PlayerBits);
UINT32 SvcPbkCtrl_Stop(UINT32 PlayerBits);
UINT32 SvcPbkCtrl_TrickPlay(UINT32 PlayerBits, UINT32 Operation);

UINT32 SvcPbkCtrl_DecOneFrame(UINT32 PlayerId, const SVC_PBK_CTRL_DEC_FRAME_s *pInput);

#define SVC_PBK_CTRL_TRACK_AUDIO     (1U)
#define SVC_PBK_CTRL_TRACK_TEXT      (2U)
void   SvcPbkCtrl_TrackCtrl(UINT32 TrackType, UINT32 Enable);

#endif  /* SVC_PBK_CTRL_H */
