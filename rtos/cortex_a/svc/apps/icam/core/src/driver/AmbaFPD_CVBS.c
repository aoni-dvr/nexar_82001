/**
 *  @file AmbaFPD_CVBS.c
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
 *  @details Control APIs of CVBS
 *
 */

#include "AmbaTypes.h"
#include "AmbaCVBS.h"
#include "AmbaFPD.h"
#include "AmbaPrint.h"
#include "AmbaFPD_CVBS.h"

#include "AmbaMisraFix.h"

static UINT32 FPD_CVBSEnable(void)
{
    return ERR_NONE;
}

static UINT32 FPD_CVBSDisable(void)
{
#if defined(AMBA_CVBS_POWER_DOWN)
    return AmbaCVBS_SetMode(AMBA_CVBS_POWER_DOWN);
#else
    return ERR_NONE;
#endif
}

static UINT32 FPD_CVBSGetInfo(AMBA_FPD_INFO_s *pInfo)
{
    UINT32                RetVal;
    AMBA_CVBS_INFO_s      CvbsInfo;
    AMBA_CVBS_IRE_CTRL_s  IreCtrl;

    RetVal = AmbaCVBS_GetStatus(&CvbsInfo, &IreCtrl);
    if (RetVal == ERR_NONE) {
        pInfo->Width                    = (UINT16)CvbsInfo.VideoWidth;
        pInfo->Height                   = (UINT16)CvbsInfo.VideoHeight;
        pInfo->FrameRate.Interlace      = 1U;
        pInfo->FrameRate.TimeScale      = CvbsInfo.TimeScale;
        pInfo->FrameRate.NumUnitsInTick = CvbsInfo.NumUnitsInTick;
        pInfo->AspectRatio.X            = 3U;
        pInfo->AspectRatio.Y            = 2U;
    }

    return RetVal;
}

static UINT32 FPD_CVBSGetModeInfo(UINT8 Mode, AMBA_FPD_INFO_s *pInfo)
{
    AmbaMisra_TouchUnused(&Mode);

    return FPD_CVBSGetInfo(pInfo);
}

static UINT32 FPD_CVBSConfig(UINT8 Mode)
{
    UINT32  RetVal = ERR_NONE;

    switch (Mode) {
    case AMBA_CVBS_SYSTEM_NTSC:
    case AMBA_CVBS_SYSTEM_PAL:
    case AMBA_CVBS_NTSC_COLORBAR:
    case AMBA_CVBS_PAL_COLORBAR:
        RetVal = AmbaCVBS_SetMode(Mode);
        break;
    default:
        RetVal = ERR_ARG;
        AmbaPrint_PrintUInt5("[FPD_CVBS] invalid mode(%d)", Mode, 0U, 0U, 0U, 0U);
        break;
    }

    return RetVal;
}

static UINT32 FPD_CVBSSetBacklight(UINT32 EnableFlag)
{
    AmbaMisra_TouchUnused(&EnableFlag);

    return ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 * FPD driver object
\*-----------------------------------------------------------------------------------------------*/
AMBA_FPD_OBJECT_s AmbaFPD_CVBS_Obj = {
    .FpdEnable          = FPD_CVBSEnable,
    .FpdDisable         = FPD_CVBSDisable,
    .FpdGetInfo         = FPD_CVBSGetInfo,
    .FpdGetModeInfo     = FPD_CVBSGetModeInfo,
    .FpdConfig          = FPD_CVBSConfig,
    .FpdSetBacklight    = FPD_CVBSSetBacklight,

    .pName = "CVBS"
};
