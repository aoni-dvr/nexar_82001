/**
 *  @file AmbaDSP_EncodeAPI.c
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Implementation of SSP Encode internal API
 *
 */

#include "AmbaDSP_Log.h"
#include "AmbaDSPSimilar_EncodeAPI.h"
#include "AmbaDSP_MsgDispatcher.h"

UINT32 SIM_GetVprocMipiYuyvNumber(UINT32 ViewZoneOI, UINT16 *YuyvNumber)
{
    UINT32 Rval = OK;

(void)ViewZoneOI;
    *YuyvNumber = 0U;

    return Rval;
}

UINT32 SIM_GetVinMipiYuyvInfo(UINT16 VinId, UINT16 *YuyvEnable)
{
    UINT32 Rval = OK;

(void)VinId;
    *YuyvEnable = 0U;

    return Rval;
}

UINT32 SIM_GetViewZoneWarpOverlap(const UINT16 ViewZoneId, UINT16 *pOverLapX, UINT16 *pOverLapY)
{
    UINT32 Rval = OK;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    (void)SIM_GetViewZoneWarpOverlapX(ViewZoneId, pOverLapX);

    HL_GetResourcePtr(&Resource);
    HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
    if (ViewZoneInfo->WarpOverLap == 0U) {
        if ((Resource->MaxHierWidth[ViewZoneId] > 0U) &&
            (Resource->MaxHierHeight[ViewZoneId] > 0U)) {
            *pOverLapY = CV2X_WARP_OVERLAP_Y_HIER_ON;
        } else {
            *pOverLapY = CV2X_WARP_OVERLAP_Y_HIER_OFF;
        }
    } else {
        *pOverLapY = ViewZoneInfo->WarpOverLap;
    }

    return Rval;
}

UINT32 SIM_GetViewZoneWarpOverlapX(const UINT16 ViewZoneId, UINT16 *pOverLapX)
{
    UINT8 IsSmem2Warp;
    UINT32 Rval = OK;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);
    HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);

    IsSmem2Warp = (ViewZoneInfo->Pipe == DSP_DRAM_PIPE_EFCY) ? 1U : 0U;
#ifdef DEBUG_TILE_CALC
    AmbaLL_LogUInt5("SIM_GetViewZoneWarpOverlapX ViewZoneId:%u IsSmem2Warp:%u SliceNumRow:%u Main:%u/%u",
            ViewZoneId, IsSmem2Warp, ViewZoneInfo->SliceNumRow, ViewZoneInfo->Main.Width, ViewZoneInfo->Main.Height);
#endif
    if (ViewZoneInfo->Main.Width == 0U) {
        *pOverLapX = 0U;
    } else if (Resource->MaxHorWarpComp[ViewZoneId] > 0U) {
        *pOverLapX = Resource->MaxHorWarpComp[ViewZoneId];
    } else if ((ViewZoneInfo->SliceNumRow > 1U) ||
               (IsSmem2Warp == (UINT8)1U)) {

        UINT16 TileOverlap = EFCY_TILE_OVERLAP_WIDTH;
        UINT16 C2YInTileX = 0U;

//FIXME separate raw overlap from warp setting
//            if (IsSmem2Warp == (UINT8)1U) {
//                (void)HL_GetViewZoneRawOverlap(ViewZoneId, &ViewZoneInfo, pOverLapX);
//                *pOverLapX = EFCY_TILE_OVERLAP_WIDTH;
//            } else {
//                *pOverLapX = EFCY_TILE_OVERLAP_WIDTH;
//            }

        //go to check c2y tile align
        (void)SIM_CalcVideoTileC2Y(ViewZoneId,
                                   ViewZoneInfo->Main.Width,
                                   SEC2_MAX_IN_WIDTH,
                                   WARP_GRID_EXP,
                                   1U, /* FixedOverlap */
                                   1U, /* ChkSmem */
                                   &C2YInTileX,
                                   &TileOverlap);
#ifdef DEBUG_TILE_CALC
        AmbaLL_LogUInt5("SIM_GetViewZoneWarpOverlapX slice ViewZoneId:%u C2YInTileX:%u TileOverlap:%u",
                ViewZoneId, C2YInTileX, TileOverlap, 0U, 0U);
#endif
        *pOverLapX = TileOverlap;
    } else {
        UINT8 IsTileMode = 0U;
        UINT16 C2YInTileX = 0U;
        //Default value is 128, number > 128 is useless for sec3
        UINT16 TileOverlap = TILE_OVERLAP_WIDTH;

        (void)SIM_IsTileMode(&IsTileMode);
        if (IsTileMode == 1U) {
            UINT8 FixedOverlap;

            FixedOverlap = (Resource->MaxHorWarpComp[ViewZoneId] > 0U) ? 1U : 0U;
            //go to check c2y tile align
            (void)SIM_CalcVideoTileC2Y(ViewZoneId,
                                      ViewZoneInfo->Main.Width,
                                      SEC2_MAX_IN_WIDTH,
                                      WARP_GRID_EXP,
                                      FixedOverlap,
                                      1U, /* ChkSmem */
                                      &C2YInTileX,
                                      &TileOverlap);
        }
#ifdef DEBUG_TILE_CALC
        AmbaLL_LogUInt5("SIM_GetViewZoneWarpOverlapX ViewZoneId:%u IsTileMode:%u C2YInTileX:%u TileOverlap:%u",
                ViewZoneId, IsTileMode, C2YInTileX, TileOverlap, 0U);
#endif
        *pOverLapX = TileOverlap;
    }
#ifdef DEBUG_TILE_CALC
    AmbaLL_LogUInt5("SIM_GetViewZoneWarpOverlapX ViewZoneId:%u TileOverlap:%u",
            ViewZoneId, *pOverLapX, 0U, 0U, 0U);
#endif
    return Rval;
}

#if 0
UINT32 SIM_GetViewZoneWarpOverlapXMax(const UINT16 ViewZoneId, UINT16 *pOverLapX)
{
    UINT16 OverLapX;
    UINT32 Rval = OK;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);
    (void)SIM_GetViewZoneWarpOverlapX(ViewZoneId, &OverLapX);
//    if (Resource->MaxHorWarpComp[ViewZoneId] > 0U) {
//        *pOverLapX = MAX2_16(OverLapX, Resource->MaxHorWarpComp[ViewZoneId]);
//    } else {
//        *pOverLapX = MAX2_16(OverLapX, TILE_OVERLAP_WIDTH);
//    }
#ifdef DEBUG_TILE_CALC
    AmbaLL_LogUInt5("SIM_GetViewZoneWarpOverlapX ViewZoneId:%u MaxHorWarpComp:%u TILE_OVERLAP_WIDTH:%u",
            ViewZoneId, Resource->MaxHorWarpComp[ViewZoneId], TILE_OVERLAP_WIDTH, 0U, 0U);
#endif
    return Rval;
}
#endif

UINT32 SIM_GetSystemWarpWarpOverlap(UINT16 *WarpOverLap)
{
    UINT16 ViewZoneIdx;
    UINT32 Rval = OK;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);
    for (ViewZoneIdx=0U; ViewZoneIdx<AMBA_DSP_MAX_VIEWZONE_NUM; ViewZoneIdx++) {
        UINT16 ViewZoneWarpOverlap;
        (void)SIM_GetViewZoneWarpOverlapX(ViewZoneIdx, &ViewZoneWarpOverlap);
#ifdef DEBUG_TILE_CALC
        AmbaLL_LogUInt5("SIM_GetViewZoneWarpOverlapX ViewZoneIdx:%u ViewZoneWarpOverlap:%u MaxHorWarpComp:%u *WarpOverLap:%u",
                ViewZoneIdx, ViewZoneWarpOverlap, Resource->MaxHorWarpComp[ViewZoneIdx], *WarpOverLap, 0U);
#endif
        ViewZoneWarpOverlap = MAX2_16(ViewZoneWarpOverlap, Resource->MaxHorWarpComp[ViewZoneIdx]);
        *WarpOverLap = MAX2_16(*WarpOverLap, ViewZoneWarpOverlap);
    }

    return Rval;
}

UINT32 SIM_GetViewZoneRawOverlap(const UINT16 ViewZoneId, UINT16 *pOverLapX)
{
    UINT32 Rval = OK;
//FIXME separate raw overlap from warp setting
    (void)SIM_GetViewZoneWarpOverlapX(ViewZoneId, pOverLapX);

    return Rval;
}

UINT32 SIM_GetLuma12Info(UINT8 *pMainY12Enable, UINT8 *pHierY12Enable)
{
    UINT32 Rval = OK;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 i;

    *pMainY12Enable = 0U;
    *pHierY12Enable = 0U;

    HL_GetResourcePtr(&Resource);

    for (i = 0U; i<Resource->MaxViewZoneNum; i++) {
        HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);

        if (DSP_GetU16Bit(ViewZoneInfo->Pyramid.IsPolySqrt2, DSP_PYMD_Y12_IDX, DSP_PYMD_Y12_LEN) > 0U) {
            *pMainY12Enable = 1U;
            *pHierY12Enable = 1U;
        }
        if (ViewZoneInfo->MainY12Enable > 0U) {
            *pMainY12Enable = 1U;
        }
    }

    return Rval;
}

UINT32 SIM_GetVinWaitFlagTotalMask(UINT32 *pMask)
{
    UINT32 Rval = OK;

    *pMask = (AMBA_DSP_VIN0_SOF | AMBA_DSP_VIN0_EOF |
              AMBA_DSP_VIN1_SOF | AMBA_DSP_VIN1_EOF |
              AMBA_DSP_VIN2_SOF | AMBA_DSP_VIN2_EOF |
              AMBA_DSP_VIN3_SOF | AMBA_DSP_VIN3_EOF |
              AMBA_DSP_VIN4_SOF | AMBA_DSP_VIN4_EOF |
              AMBA_DSP_VIN5_SOF | AMBA_DSP_VIN5_EOF);

    return Rval;
}

UINT32 SIM_GetVoutWaitFlagTotalMask(UINT32 *pMask)
{
    UINT32 Rval = OK;

    *pMask = AMBA_DSP_VOUT0_INT | AMBA_DSP_VOUT1_INT;

    return Rval;
}

UINT32 SIM_GetVinOutputPinNum(UINT16 VinId, UINT16 VirtChanId, const CTX_RESOURCE_INFO_s *pResource, UINT8 *pPinNum)
{
    UINT32 Rval = OK;

    if ((VinId >= AMBA_DSP_MAX_VIN_NUM) ||
        (VirtChanId >= AMBA_DSP_MAX_VIRT_CHAN_NUM)) {
        Rval = DSP_ERR_0001;
    } else if ((pResource == NULL) ||
               (pPinNum == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        *pPinNum = (UINT8)pResource->MaxVinVirtChanOutputNum[VinId][VirtChanId];
    }

    return Rval;
}

UINT8 SIM_GetVin2CmdNormalWrite(UINT16 VinId) {
    UINT8 NormalWrite = 0U;
    UINT16 Idx = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    static const UINT8 HLVin2CmdNormalWrite[AMBA_DSP_MAX_VIN_NUM] = {
        [0U] = AMBA_DSP_CMD_VCAP_NORMAL_WRITE,
        [1U] = AMBA_DSP_CMD_VCAP2_NORMAL_WRITE,
        [2U] = AMBA_DSP_CMD_VCAP2_3RD_VIN_NORMAL_WRITE,
        [3U] = AMBA_DSP_CMD_VCAP2_4TH_VIN_NORMAL_WRITE,
        [4U] = AMBA_DSP_CMD_VCAP2_5TH_VIN_NORMAL_WRITE,
        [5U] = AMBA_DSP_CMD_VCAP2_6TH_VIN_NORMAL_WRITE,
    };

    HL_GetResourcePtr(&Resource);

    if (VinId < AMBA_DSP_MAX_VIN_NUM) {
        NormalWrite = HLVin2CmdNormalWrite[VinId];
    } else if (VinId < DSP_VIN_MAX_NUM) {
//FIXME, Temp use First alived Vin for all virtual-vin
        if (Resource->VinBit > 0U) {
            DSP_Bit2U16Idx((UINT32)Resource->VinBit, &Idx);
            NormalWrite = HLVin2CmdNormalWrite[Idx];
        } else {
            AmbaLL_LogUInt5("Vin2CmdNormalWrite No active vin", 0U, 0U, 0U, 0U, 0U);
            NormalWrite = HLVin2CmdNormalWrite[0U];
        }

    } else {
        NormalWrite = HLVin2CmdNormalWrite[0U];
    }
    return NormalWrite;
}

UINT8 SIM_GetVin2CmdGroupWrite(UINT16 VinId) {
    UINT8 NormalWrite = 0U;
    UINT16 Idx = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    static const UINT8 HLVin2CmdGroupWrite[AMBA_DSP_MAX_VIN_NUM] = {
        [0U] = AMBA_DSP_CMD_VCAP_GROUP_WRITE,
        [1U] = AMBA_DSP_CMD_VCAP2_GROUP_WRITE,
        [2U] = AMBA_DSP_CMD_VCAP2_3RD_VIN_GROUP_WRITE,
        [3U] = AMBA_DSP_CMD_VCAP2_4TH_VIN_GROUP_WRITE,
        [4U] = AMBA_DSP_CMD_VCAP2_5TH_VIN_GROUP_WRITE,
        [5U] = AMBA_DSP_CMD_VCAP2_6TH_VIN_GROUP_WRITE,
    };

    HL_GetResourcePtr(&Resource);

    if (VinId < AMBA_DSP_MAX_VIN_NUM) {
        NormalWrite = HLVin2CmdGroupWrite[VinId];
    } else if (VinId < DSP_VIN_MAX_NUM) {
//FIXME, Temp use First alived Vin for all virtual-vin
        if (Resource->VinBit > 0U) {
            DSP_Bit2U16Idx((UINT32)Resource->VinBit, &Idx);
            NormalWrite = HLVin2CmdGroupWrite[Idx];
        } else {
            AmbaLL_LogUInt5("Vin2CmdGroupWrite No active vin", 0U, 0U, 0U, 0U, 0U);
            NormalWrite = HLVin2CmdGroupWrite[0U];
        }
    } else {
        //TBD
    }
    return NormalWrite;
}

UINT32 SIM_GetVinSecCfgSize(UINT16 VinId) {
    static const UINT32 HLVinSecCfgSize[AMBA_DSP_MAX_VIN_NUM] = {
        [0U] = DSP_SEC1_CFG_SIZE,
        [1U] = DSP_SEC10_CFG_SIZE,
        [2U] = DSP_SEC11_CFG_SIZE,
    };
    UINT32 SecCfgSize = HLVinSecCfgSize[0];

    if (VinId < AMBA_DSP_MAX_VIN_NUM) {
        SecCfgSize = HLVinSecCfgSize[VinId];
    }
    return SecCfgSize;
}

