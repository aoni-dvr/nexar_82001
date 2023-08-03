/**
 *  @file SvcAnimCalib.h
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
 *  @details Header file of calib mode
 *
 */

#ifndef SVC_ANIMATION_CALIB_H
#define SVC_ANIMATION_CALIB_H

#include <AmbaDSP_Liveview.h>
#include "AmbaShell.h"

#include "AmbaAnimLib.h"
#include "AmbaCalibData.h"

#define SVC_ANIM_MAX_WARP_PER_VIEW   (4U)                                   /*< max warp data count per view */
#define SVC_ANIM_MAX_BLEND_PER_VIEW  (4U)                                   /*< max blend data count per view */
#define SVC_ANIM_MAX_OSD_PER_VIEW    (1U)                                   /*< max osd data count per view */
#define SVC_ANIM_MAX_GROUP           (10U)                                  /*< max group count */
#define SVC_ANIM_MAX_ITEM            (AMBA_CALIB_DATA_MAX_ITEM_PER_GRP)     /*< max item count */
#define SVC_ANIM_MAX_ELEMENT         (AMBA_CALIB_DATA_MAX_ELEMENT_PER_ITEM) /*< max element count */
#define SVC_ANIM_AVM_MAINVIEW        (0U)
#define SVC_ANIM_AVM_FRONT           (1U)
#define SVC_ANIM_AVM_BACK            (2U)
#define SVC_ANIM_AVM_LEFT            (3U)
#define SVC_ANIM_AVM_RIGHT           (4U)

#define SVC_ANIM_MAX_NAME_LENGTH     (64U)

#define SVC_OSD_WIDTH                 (384U)
#define SVC_OSD_HEIGH                 (384U)

typedef struct {
   UINT8 WarpCount;
   UINT32 HorGridNum[SVC_ANIM_MAX_WARP_PER_VIEW];
   UINT32 VerGridNum[SVC_ANIM_MAX_WARP_PER_VIEW];
} SVC_ANIM_WARP_HEADER_s;

typedef struct {
   UINT8 BlendCount;
   UINT16 Width[SVC_ANIM_MAX_BLEND_PER_VIEW];
   UINT16 Height[SVC_ANIM_MAX_BLEND_PER_VIEW];
} SVC_ANIM_BLEND_HEADER_s;

typedef struct {
   UINT8 DispCount;   // Total disp count
   UINT32 GroupCount; // Total group count
   UINT16 ViewCount[SVC_ANIM_LIB_MAX_DISP]; // Total view count for each disp
   UINT32 CompSize[SVC_ANIM_LIB_MAX_DISP][SVC_ANIM_LIB_MAX_VIEW];
   SVC_ANIM_WARP_HEADER_s WarpHeader[SVC_ANIM_LIB_MAX_DISP][SVC_ANIM_LIB_MAX_VIEW];
   SVC_ANIM_BLEND_HEADER_s BlendHeader[SVC_ANIM_LIB_MAX_DISP][SVC_ANIM_LIB_MAX_VIEW];
} SVC_ANIM_HEADER_s;

typedef struct {
    UINT8 DataId; // for warp and blend, fill in order of 0, 1, 2, ... (should be consistent with WarpEnable and BlendType in SVC_ANIM_INDEX_s)
    UINT8 VoutId; // for osd and blend
} SVC_ANIM_ELEMENT_INFO_s;

typedef struct {
    UINT8 ElementCount;
    SVC_ANIM_ELEMENT_INFO_s ElementInfo[SVC_ANIM_MAX_ELEMENT];
} SVC_ANIM_ITEM_INFO_s;

typedef struct {
    UINT32 Id;
    UINT8 ItemCount;
    SVC_ANIM_ITEM_INFO_s ItemInfo[SVC_ANIM_MAX_ITEM];
} SVC_ANIM_GROUP_DESC_s;

typedef struct {
    UINT32 GroupId;
    UINT8 ItemId;
    UINT8 ElementId;
} SVC_ANIM_DATA_DESC_s;

typedef struct {
    UINT8 OsdCount;
    UINT8 GroupCount;
    SVC_ANIM_GROUP_DESC_s GroupDesc[SVC_ANIM_MAX_GROUP];
    SVC_ANIM_DATA_DESC_s OsdDataDesc[SVC_ANIM_MAX_OSD_PER_VIEW];
} SVC_ANIM_DATA_INFO_s;

typedef struct {
   AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s ChanInfo;
   AMBA_DSP_BUF_s LumaAlphaBuf;
} SVC_ANIM_RENDER_CFG_s;

typedef struct {
    UINT32 Version;
    UINT8 VoutCount;
    UINT8 ChanCount; // Total chan count in this view
    SVC_ANIM_DATA_INFO_s DataInfo;
    UINT8 WarpEnable[SVC_ANIM_LIB_MAX_CHAN];
    UINT8 BlendType[SVC_ANIM_LIB_MAX_CHAN]; // 0: w/o blend, 1: calculated blend, 2: transparent blend
    SVC_ANIM_RENDER_CFG_s RenderCfg[SVC_ANIM_LIB_MAX_VOUT][SVC_ANIM_LIB_MAX_CHAN];
} SVC_ANIM_INDEX_s;

extern UINT32 SvcAnimCalib_QueryBufSize(SIZE_t *Size);
extern void SvcAnimCalib_CmdEntry(UINT32 ArgCount, char * const * ArgVector, AMBA_SHELL_PRINT_f PrintFunc);

extern UINT32 SvcAnimCalib_AvmFeedPointMap(const AMBA_CT_AVM_CALIB_POINTS_s *pInCalibPoints,
    const AMBA_CT_AVM_ASSISTANCE_POINTS_s *pInAssistancePoints,
    AMBA_CAL_AVM_POINT_MAP_s *pOut);

extern UINT32 SvcAnimCalib_AvmFeedLensSpec(const AMBA_CT_AVM_LENS_s *pIn,
    AMBA_CAL_LENS_SPEC_s *pOut,
    AMBA_CAL_LENS_DST_REAL_EXPECT_s *pRealExpectBuf,
    AMBA_CAL_LENS_DST_ANGLE_s *pAngleBuf,
    AMBA_CAL_LENS_DST_FORMULA_s *pFormulaBuf);

#endif /* _SVC_ANIMATION_CALIB_H_ */

