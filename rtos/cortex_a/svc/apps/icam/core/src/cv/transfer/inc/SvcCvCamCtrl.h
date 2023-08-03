/**
*  @file SvcCvCamCtrl.h
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
*  @details Interface for SVC and CamCtrl channel
*
*/

#ifndef SVC_CV_CAMCTRL_H
#define SVC_CV_CAMCTRL_H

/*-----------------------------------------------------------------------------------------------*\
 *  Definition for Message
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32 Msg;
    void   *pCtx;
} SVC_CV_CAMCTRL_MSG_s;

/* Definition for Msg of SVC_CV_CAMCTRL_MSG_s */
#define SVC_CV_OSD_ENABLE                (0U)
#define SVC_CV_OSD_DRAW_RECT             (1U)
#define SVC_CV_OSD_FLUSH                 (2U)
#define SVC_CV_OSD_CLEAR                 (3U)
#define SVC_CV_OSD_FLUSH_CAPSEQ          (4U)
#define SVC_CV_OSD_SETCHARCONFIG         (5U)
#define SVC_CV_OSD_DRAWSTRING            (6U)
#define SVC_CV_OSD_SET_EXTOSDCLUT        (7U)
#define SVC_CV_OSD_SET_EXTOSDBUFINFO     (8U)
#define SVC_CV_OSD_UPDATE_EXTBUF         (9U)
#define SVC_CV_OSD_UPDATE_EXTBUF_SYNC    (10U)
#define SVC_CV_OSD_RELEASE_EXTOSD        (11U)
#define SVC_CV_OSD_DRAW_LINE             (12U)
#define SVC_CV_OSD_BITMAP                (13U)
#define SVC_CV_OSD_DRAW_3D               (14U)

/* Definition for pCtx of SVC_CV_CAMCTRL_MSG_s */
/* SVC_CV_OSD_ENABLE, SVC_CV_OSD_FLUSH, SVC_CV_OSD_CLEAR, SVC_CV_OSD_FLUSH_CAPSEQ */
typedef struct {
    UINT32 Channel;
    UINT32 Param[6];
} SVC_CV_DISP_OSD_CTRL_s;

typedef struct {
    UINT32 Channel;
    UINT32 HaveDir; //0: No direction; 1: have direction
    UINT32 RBL_x;
    UINT32 RBL_y;
    UINT32 RBR_x;
    UINT32 RBR_y;
    UINT32 FBL_x;
    UINT32 FBL_y;
    UINT32 FBR_x;
    UINT32 FBR_y;
    UINT32 RTL_x;
    UINT32 RTL_y;
    UINT32 RTR_x;
    UINT32 RTR_y;
    UINT32 FTL_x;
    UINT32 FTL_y;
    UINT32 FTR_x;
    UINT32 FTR_y;
    UINT32 Color;
    UINT32 Thickness;
} SVC_CV_DISP_3D_CMD_s;

/* SVC_CV_OSD_DRAW_RECT */
typedef struct {
    UINT32 Channel;
    UINT32 X1;
    UINT32 Y1;
    UINT32 X2;
    UINT32 Y2;
    UINT32 Color;
    UINT32 Thickness;
    UINT32 WarningLevel;
} SVC_CV_DISP_OSD_CMD_s;

/* SVC_CV_OSD_DRAWSTRING */
typedef struct {
    UINT32 Channel;
    UINT32 X1;
    UINT32 Y1;
    UINT8  FontSize;
    UINT32 Color;
    char   String[24];
} SVC_CV_DISP_OSD_STR_s;

/* SVC_CV_OSD_DRAW_LINE */
typedef struct {
    UINT32 Channel;
    UINT32 X1;
    UINT32 Y1;
    UINT32 X2;
    UINT32 Y2;
    UINT32 LineWidth;
    UINT32 Color;
} SVC_CV_DISP_OSD_LINE_s;

/* SVC_CV_OSD_SET_EXTOSDCLUT, SVC_CV_OSD_SET_EXTOSDBUFINFO, SVC_CV_OSD_UPDATE_EXTBUF */
/* SVC_CV_OSD_UPDATE_EXTBUF_SYNC SVC_CV_OSD_RELEASE_EXTOSD */
typedef struct {
    UINT32 Channel;
    UINT32 Param[12];
} SVC_CV_DISP_EXTOSD_CTRL_s;

typedef struct {
    UINT32 Channel;
    UINT32 X;
    UINT32 Y;
    UINT32 W;
    UINT32 H;
    UINT32 SrcW;
    UINT32 SrcH;
    UINT32 SrcPitch;
    void   *pAddr;
} SVC_CV_DISP_OSD_BITMAP_s;

typedef struct {
    UINT32 Index;
    UINT32 X;
    UINT32 Y;
    UINT32 W;
    UINT32 H;
    UINT32 RotateFlip;
} REGION_s;
#endif  /* SVC_CV_CAMCTRL_H */
