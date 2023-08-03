/**
 *  @file SvcViewCtrl.h
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
 *  @details svc view Control header
 *
 */

#ifndef SVC_VIEW_CTRL_H
#define SVC_VIEW_CTRL_H

#define SVC_VIEW_CTRL_QUEUE_NUM         (5U)
#define SVC_VIEW_CTRL_TASK_STACK_SIZE   (0x8000)

#define SVC_VIEW_CTRL_WARP_CMD_PAN          (1U)
#define SVC_VIEW_CTRL_WARP_CMD_TILT         (2U)
#define SVC_VIEW_CTRL_WARP_CMD_ZOOM         (3U)
#define SVC_VIEW_CTRL_WARP_CMD_ROTATE       (4U)
#define SVC_VIEW_CTRL_WARP_CMD_SET_WIN      (5U)
#define SVC_VIEW_CTRL_WARP_CMD_SET_WARP     (6U)
#define SVC_VIEW_CTRL_PREV_CMD_PAN          (7U)
#define SVC_VIEW_CTRL_PREV_CMD_TILT         (8U)
#define SVC_VIEW_CTRL_PREV_CMD_ZOOM         (9U)
#define SVC_VIEW_CTRL_PREV_CMD_SET_SRCWIN   (10U)
#define SVC_VIEW_CTRL_PREV_CMD_SET_DSTWIN   (11U)
#define SVC_VIEW_CTRL_CMD_VALIDATE          (12U)
#define SVC_VIEW_CTRL_CMD_RESTORE           (0U)

#define SVC_VIEW_CTRL_CMD_NUM               (13U)

typedef struct /* SVC_VIEW_CTRL_CMD_MSG_s */ {
    INT32 Value;
    INT32 Reserved[4];
    UINT8 VinID;
    UINT8 VoutID;
    UINT8 FovIdx;
    UINT8 Cmd;
} SVC_VIEW_CTRL_CMD_MSG_s;

typedef struct /* SVC_VIEW_CTRL_WINDOW_s */ {
    UINT16  OffsetX;
    UINT16  OffsetY;
    UINT16  Width;
    UINT16  Height;
} SVC_VIEW_CTRL_WINDOW_s;

typedef struct /*_SVC_VIEW_CTRL_WARP_POSITION_s_*/ {
    INT32  ShiftX;
    INT32  ShiftY;
    DOUBLE ZoomX;
    DOUBLE ZoomY;
} SVC_VIEW_CTRL_WARP_POSITION_s;

typedef struct /* _SVC_VIEW_CTRL_WARP_ROTATION_s_ */ {
    DOUBLE Theta;
} SVC_VIEW_CTRL_WARP_ROTATION_s;

typedef struct /*_SVC_VIEW_CTRL_WARP_INFO_s_*/ {
    SVC_VIEW_CTRL_WARP_POSITION_s Position;
    SVC_VIEW_CTRL_WARP_ROTATION_s Rotation;
    AMBA_IK_WARP_INFO_s           Warp;
} SVC_VIEW_CTRL_WARP_INFO_s;

typedef struct /**/ {
    UINT32 RawWidth;
    UINT32 RawHeight;
    UINT32 FovWidth;
    UINT32 FovHeight;
    INT32  ShiftX;
    INT32  ShiftY;
    DOUBLE ZoomX;
    DOUBLE ZoomY;
    AMBA_IK_WARP_INFO_s Warp;
} SVC_VIEW_CTRL_WARP_DEFAULT_INFO_s;

typedef struct /* _SVC_VIEW_CTRL_PREV_POSITION_s_ */ {
    UINT16 OffsetX;
    UINT16 OffsetY;
    UINT16 Width;
    UINT16 Height;
    DOUBLE ZoomX;
    DOUBLE ZoomY;
} SVC_VIEW_CTRL_PREV_POSITION_s;

typedef struct /* _SVC_VIEW_CTRL_PREV_INFO_s_ */ {
    SVC_VIEW_CTRL_PREV_POSITION_s SrcPosition;
    SVC_VIEW_CTRL_PREV_POSITION_s DstPosition;
} SVC_VIEW_CTRL_PREV_INFO_s;

typedef struct /*_SVC_VIEW_CTRL_INFO_s_*/ {
    SVC_VIEW_CTRL_CMD_MSG_s   Queue[SVC_VIEW_CTRL_QUEUE_NUM];
    AMBA_KAL_MSG_QUEUE_t      QueueId;
    char                      QueueName[32];
} SVC_VIEW_CTRL_INFO_s;

typedef struct /*_SVC_VIEW_CTRL_TASK_s_*/ {
    char            TaskName[32];
    AMBA_KAL_TASK_t TaskCtrl;
    UINT32          Priority;
    UINT32          EntryArg;
    UINT32          StackSize;
    UINT8           Stack[SVC_VIEW_CTRL_TASK_STACK_SIZE];
    UINT8           TaskCreated;
} SVC_VIEW_CTRL_TASK_s;

UINT32 SvcViewCtrl_Init(void);
UINT32 SvcViewCtrl_DeInit(void);
UINT32 SvcViewCtrl_Config(const SVC_VIEW_CTRL_WARP_INFO_s *pMirrorInfo);
void   SvcViewCtrl_Exec(SVC_VIEW_CTRL_CMD_MSG_s MirrorCmdMsg);
void   SvcViewCtrl_DebugEnable(UINT32 DebugEnable);
UINT32 SvcViewCtrl_GetViewCtrlInfo(UINT32 FovIdx, SVC_VIEW_CTRL_WARP_INFO_s *pMirrorInfo);
UINT32 SvcViewCtrl_GetViewWinInfo(UINT32 FovIdx, SVC_VIEW_CTRL_WINDOW_s *pWindowInfo);
UINT32 SvcViewCtrl_GetPrevWinInfo(UINT32 VoutID, UINT32 FovIdx, SVC_VIEW_CTRL_WINDOW_s *pWindowInfo);

#endif  /* SVC_VIEW_CTRL_H */
