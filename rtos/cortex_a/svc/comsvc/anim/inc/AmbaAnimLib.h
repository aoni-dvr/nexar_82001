/**
 *  @file AmbaAnimLib.h
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Header file of svc animation library
 *
 */

#ifndef SVC_ANIMATION_LIB_H
#define SVC_ANIMATION_LIB_H

#include <AmbaDSP_Capability.h>
#include <AmbaDSP_ImageFilter.h>
#include "AmbaCalibData.h"

#define SVC_ANIM_LIB_MAX_DISP         (3U)
#define SVC_ANIM_LIB_MAX_VIEW         (200U)
#define SVC_ANIM_LIB_MAX_CHAN         AMBA_DSP_MAX_YUVSTRM_VIEW_NUM   // max: AMBA_DSP_MAX_YUVSTRM_VIEW_NUM
#define SVC_ANIM_LIB_MAX_VIEWZONE     AMBA_DSP_MAX_VIEWZONE_NUM       // max: AMBA_DSP_MAX_VIEWZONE_NUM
#define SVC_ANIM_LIB_MAX_VOUT         AMBA_DSP_MAX_VOUT_NUM           // max: AMBA_DSP_MAX_VOUT_NUM

typedef struct {
    UINT16 OffsetX;
    UINT16 OffsetY;
} SVC_ANIM_LIB_OFFSET_s;

typedef struct {
    UINT8 VoutId;
    UINT16 YuvWidth;  // active vout width
    UINT16 YuvHeight; // active vout height
    AMBA_DSP_WINDOW_s VoutWin;
} SVC_ANIM_LIB_VOUT_CFG_s;

typedef struct {
    UINT8 DataFormat;
    UINT16 BufPitch;
    UINT16 BufWidth;
    UINT16 BufHeight;
    UINT32 ClutAddr;
    UINT32 ClutSize;
} SVC_ANIM_LIB_OSD_CFG_s;

typedef struct {
    UINT16 YuvStrmId;
    UINT16 Purpose;
    UINT16 EncDest;
    AMBA_DSP_WINDOW_s Roi[SVC_ANIM_LIB_MAX_VIEWZONE];
} SVC_ANIM_LIB_YUV_STRM_CFG_s;

typedef struct {
    UINT8 VoutCount;
    SVC_ANIM_LIB_VOUT_CFG_s VoutCfg[SVC_ANIM_LIB_MAX_VOUT];
    SVC_ANIM_LIB_OSD_CFG_s OsdCfg[SVC_ANIM_LIB_MAX_VOUT];
    SVC_ANIM_LIB_YUV_STRM_CFG_s YuvStrmCfg[SVC_ANIM_LIB_MAX_VOUT];
} SVC_ANIM_LIB_SYS_CFG_s;

UINT32 SvcAnimLib_Init(const SVC_ANIM_LIB_SYS_CFG_s *Cfg);
UINT32 SvcAnimLib_Load(UINT32 HdlrId, UINT32 GroupId, UINT32 ItemId, UINT8 ItemCount, UINT8 *LoadBuf, UINT32 LoadBufSize);
UINT32 SvcAnimLib_Decompress(UINT8 Algo, const AMBA_CALIB_DATA_DECMPR_INFO_s *Info, const UINT8 *SrcBuf, UINT32 SrcBufSize, UINT8 *DstBuf, UINT32 DstBufSize);
UINT32 SvcAnimLib_IkCalWarp(UINT16 ViewZoneId, const AMBA_IK_WARP_INFO_s *Info, const UINT8 *WorkBuf, UINT32 WorkSize, const UINT8 *OutBuf, UINT32 OutSize);
UINT32 SvcAnimLib_UpdateView(UINT8 ViewZoneCount, UINT8 DispCount, SVC_ANIM_LIB_OFFSET_s DispOffset[SVC_ANIM_LIB_MAX_DISP],
        UINT8 *OsdBuf[SVC_ANIM_LIB_MAX_VOUT], const UINT16 ViewZoneId[SVC_ANIM_LIB_MAX_VIEWZONE],
        const UINT8 IkGeoEnable[SVC_ANIM_LIB_MAX_VIEWZONE], UINT8 *IkGeoResult[SVC_ANIM_LIB_MAX_VIEWZONE]);

#endif /* _SVC_ANIMATION_LIB_H_ */

