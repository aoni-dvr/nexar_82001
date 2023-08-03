/**
 *  @file SvcAnim.h
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
 *  @details Header file of svc animation
 *
 */

#ifndef SVC_ANIMATION_H
#define SVC_ANIMATION_H

#include "AmbaAnimLib.h"
#include "AmbaCalib_AVMIF.h"
#include "AmbaCT_AvmTunerIF.h"
#include "SvcAnimCalib.h"

#define SVC_ANIM_EVENT_LAYOUT_DONE    (0x01U) /*< animation has updated certain layout */
#define SVC_ANIM_EVENT_ANIM_DONE      (0x02U) /*< animation done */
#define SVC_ANIM_EVENT_OSD_CHANGED    (0x03U) /*< change to the new main OSD */
#define SVC_ANIM_EVENT_OSD_DRAW_DONE  (0x04U) /*< animation has finished the drawing */

typedef struct {
    UINT8 DispCount;  // indicates that how many display windows consist of current layout
    UINT8 OsdCount[SVC_ANIM_LIB_MAX_DISP];
    UINT8 *Buffer;
    UINT16 BufPitch;
    UINT16 BufWidth;
    UINT16 BufHeight;
    AMBA_DSP_WINDOW_s Window[SVC_ANIM_LIB_MAX_DISP][SVC_ANIM_MAX_OSD_PER_VIEW]; // position of all osd data in anim
} SVC_ANIM_EVENT_OSD_INFO_s;

typedef struct {
    UINT8 VoutCount;
    UINT8 VoutId[SVC_ANIM_LIB_MAX_VOUT];
    SVC_ANIM_EVENT_OSD_INFO_s OsdInfo[SVC_ANIM_LIB_MAX_VOUT];
} SVC_ANIM_EVENT_INFO_s;

typedef UINT32 (*SVC_ANIM_EVENT_CALLBACK_f)(UINT8 EventId, UINT32 LayoutId);
typedef UINT32 (*SVC_ANIM_OSD_BUF_CHANGE_CALLBACK_f)(SVC_ANIM_EVENT_INFO_s *EventInfo);
typedef UINT32 (*SVC_ANIM_OSD_DONE_CALLBACK_f)(SVC_ANIM_EVENT_INFO_s *EventInfo);

typedef struct {
    UINT8 VoutCount;
    UINT16 OsdBufPitch[SVC_ANIM_LIB_MAX_VOUT];
    UINT16 OsdBufHeight[SVC_ANIM_LIB_MAX_VOUT];
} SVC_ANIM_GET_BUF_CFG_s;

typedef struct {
    UINT8 DispCount;
    UINT8 DispId[SVC_ANIM_LIB_MAX_DISP];
    UINT16 ViewId[SVC_ANIM_LIB_MAX_DISP];
} SVC_ANIM_DEFAULT_DATA_CFG_s;

typedef struct {
    UINT8 VoutCount;
    UINT8 *CacheWorkBuf;
    UINT8 *NonCacheWorkBuf;
    UINT32 CacheWorkBufSize;
    UINT32 NonCacheWorkBufSize;
    UINT8 ChannelId[SVC_ANIM_LIB_MAX_DISP][SVC_ANIM_LIB_MAX_CHAN];
    UINT16 ViewZoneId[SVC_ANIM_LIB_MAX_DISP][SVC_ANIM_LIB_MAX_CHAN];
    SVC_ANIM_LIB_OFFSET_s DispOffset[SVC_ANIM_LIB_MAX_DISP];
    SVC_ANIM_LIB_VOUT_CFG_s VoutCfg[SVC_ANIM_LIB_MAX_VOUT];
    SVC_ANIM_LIB_OSD_CFG_s OsdCfg[SVC_ANIM_LIB_MAX_VOUT];
    SVC_ANIM_LIB_YUV_STRM_CFG_s YuvStrmCfg[SVC_ANIM_LIB_MAX_VOUT];
    SVC_ANIM_DEFAULT_DATA_CFG_s DefaultDataCfg;
    SVC_ANIM_EVENT_CALLBACK_f EventCb;
    SVC_ANIM_OSD_BUF_CHANGE_CALLBACK_f OsdChangeCb;
    SVC_ANIM_OSD_DONE_CALLBACK_f OsdDoneCb;
} SVC_ANIM_INIT_CFG_s;

typedef struct {
    UINT8 VoutCount;
    UINT8 ViewZoneCount;
    UINT8 VoutId[SVC_ANIM_LIB_MAX_VOUT];
    UINT8 *OsdBuf[SVC_ANIM_LIB_MAX_VOUT];
    UINT8 WarpEnable[SVC_ANIM_LIB_MAX_VIEWZONE];
    AMBA_IK_WARP_INFO_s IkWarpInfo[SVC_ANIM_LIB_MAX_VIEWZONE];
    AMBA_DSP_LIVEVIEW_STREAM_CFG_s LvStrmCfg[SVC_ANIM_LIB_MAX_VOUT];
    AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s ChanInfo[SVC_ANIM_LIB_MAX_VOUT][SVC_ANIM_LIB_MAX_CHAN];
    AMBA_DSP_WINDOW_DIMENSION_s MaxChanWin[SVC_ANIM_LIB_MAX_VOUT][SVC_ANIM_LIB_MAX_CHAN];
} SVC_ANIM_DEFAULT_DATA_s;

typedef struct {
    UINT16 PlayList[SVC_ANIM_LIB_MAX_DISP][SVC_ANIM_LIB_MAX_VIEW];
    UINT16 PlayCount[SVC_ANIM_LIB_MAX_DISP];
    UINT32 FrameRateDivisor;
} SVC_ANIM_PLAY_CFG_s;

typedef struct {
    UINT8 ChannelId[SVC_ANIM_LIB_MAX_DISP][SVC_ANIM_LIB_MAX_CHAN];
    UINT16 ViewZoneId[SVC_ANIM_LIB_MAX_DISP][SVC_ANIM_LIB_MAX_CHAN];
    SVC_ANIM_LIB_OFFSET_s Offset[SVC_ANIM_LIB_MAX_DISP];
} SVC_ANIM_DISPLAY_CFG_s;


UINT32 SvcAnim_GetWorkBufSize(const SVC_ANIM_GET_BUF_CFG_s *Cfg, UINT32 *CacheWorkBufSize, UINT32 *NonCacheWorkBufSize);
UINT32 SvcAnim_Init(const SVC_ANIM_INIT_CFG_s *Cfg);
UINT32 SvcAnim_GetDefaultData(SVC_ANIM_DEFAULT_DATA_s *DefaultData);
UINT32 SvcAnim_Play(const SVC_ANIM_PLAY_CFG_s *Cfg);
UINT32 SvcAnim_ConfigDisplay(const SVC_ANIM_DISPLAY_CFG_s *Cfg);
UINT32 SvcAnim_SyncJobRdyHandler(const void *EventInfo);

#endif /* _SVC_ANIMATION_H_ */

