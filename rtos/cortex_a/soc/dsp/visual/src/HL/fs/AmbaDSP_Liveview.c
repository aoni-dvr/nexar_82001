/**
 *  @file AmbaDSP_Liveview.c
 *
 * Copyright (c) 2020 Ambarella International LP
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
 *  @details Implementation of SSP Liveview control API
 *
 */

#include "AmbaDSP_Log.h"
#include "AmbaDSP_Cmds.h"
#include "AmbaDSP_IntChk.h"
#include "AmbaDSP_BaseCfg.h"
#include "AmbaDSP_ArmComm.h"
#include "AmbaDSP_ApiDump.h"
#include "AmbaDSP_ApiCheck.h"
#include "AmbaDSP_ApiTiming.h"
#include "AmbaDSP_EventCtrl.h"
#include "AmbaDSP_EncodeAPI.h"
#include "AmbaDSP_ApiResource.h"
#include "AmbaDSP_EncodeUtility.h"
#include "AmbaDSPSimilar_EncodeUtility.h"
#include "AmbaDSP_DecodeUtility.h"
#include "dsp_priv_api.h"
#include "ambadsp_ioctl.h"

#ifdef DEBUG_LIVEVIEW_FLOW
static inline void HL_LiveviewFillVinInfo(void)
{
    // DO NOTHING, migrate to dsp_liveview_vin_drv_cfg
}
#endif

static inline void HL_LiveviewVinCompressionSetup(void)
{
    UINT32 i;
    UINT16 VinIdx = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_VIN_INFO_s VinInfo = {0};
    UINT16 YuyvEnable = 0U;

    HL_GetResourcePtr(&Resource);

    for (i = 0; i < Resource->MaxViewZoneNum; i++) {
        if (0U == DSP_GetBit(Resource->ViewZoneActiveBit, i, 1U)) {
            continue;
        }

        HL_GetViewZoneInfoPtr((UINT16)i, &ViewZoneInfo);
        if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON) ||
            (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC)) {
            continue;
        }

        DSP_Bit2U16Idx((UINT32)ViewZoneInfo->SourceVin, &VinIdx);

        HL_GetVinInfo(HL_MTX_OPT_GET, VinIdx, &VinInfo);
        if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM) ||
            (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_422) ||
            (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_420)) {
            if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_422) ||
                (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_420)) {
                VinInfo.CfaCompressed = (UINT8)0U;
                VinInfo.SensorMode = VIN_INPUT_MODE_YUV;
            } else {
                HL_DisassembleVinCompression(ViewZoneInfo->IkCompression, &VinInfo.CfaCompressed, &VinInfo.CfaCompact);
                VinInfo.SensorMode = (UINT8)ViewZoneInfo->IkSensorMode;
            }
        } else {
            YuyvEnable = 0U;
            (void)HL_GetVinMipiYuyvInfo(VinIdx, &VinInfo, &YuyvEnable);
            YuyvEnable = (ViewZoneInfo->Pipe == DSP_DRAM_PIPE_MIPI_YUV)? (UINT16)1U: (UINT16)0U;

            if ((VinInfo.InputFormat == DSP_VIN_INPUT_YUV_422_INTLC) ||
                (VinInfo.InputFormat == DSP_VIN_INPUT_YUV_422_PROG)) {
                if (YuyvEnable == 0U) {
                    VinInfo.CfaCompressed = (UINT8)0U;
                    VinInfo.CfaCompact = (UINT8)0U;
                } else {
                    HL_DisassembleVinCompression(ViewZoneInfo->IkCompression, &VinInfo.CfaCompressed, &VinInfo.CfaCompact);
                }
                VinInfo.SensorMode = (UINT8)VIN_INPUT_MODE_YUV;
            } else {
                HL_DisassembleVinCompression(ViewZoneInfo->IkCompression, &VinInfo.CfaCompressed, &VinInfo.CfaCompact);
                VinInfo.SensorMode = (UINT8)ViewZoneInfo->IkSensorMode;
            }
        }
        HL_SetVinInfo(HL_MTX_OPT_SET, VinIdx, &VinInfo);
    }
}

#ifdef DEBUG_LIVEVIEW_FLOW
static inline void HL_LiveviewFlipModeSetup(void)
{
    // DO NOTHING, migrate to AmbaDSP_LiveviewCtrl
}
#endif

static inline UINT32 HL_LvSysPreprocOnVirtVinTD(UINT32 *pVirtVinBitMask,
                                                const CTX_DSP_INST_INFO_s *pDspInstInfo)
{
    UINT32 Rval = OK, VirtVinNum = 0U;
    UINT16 i, j, VinExist;
    UINT8 ExitJLoop;
    CTX_VIN_INFO_s VinInfo = {0};
    UINT16 VirtVinIdx;

    for (i = 0; i < AMBA_DSP_MAX_VIN_NUM; i++) {
        VinExist = 0U;
        HL_GetVinExistence(i, &VinExist);
        if (VinExist > 0U) {
            HL_GetVinInfo(HL_MTX_OPT_GET, i, &VinInfo);
            if (VinInfo.TimeDivisionNum[0U] > 1U) {
                /* First Grp must be PhysicalVin */
                VinInfo.TimeDivisionVinId[0U][0U] = i;

                ExitJLoop = 0U;
                for (j = 1U; j < VinInfo.TimeDivisionNum[0U]; j++) {
                    DSP_Bit2Cnt(*pVirtVinBitMask, &VirtVinNum);
                    if (VirtVinNum < (UINT32)pDspInstInfo->MaxVirtVinNum) {
                        VirtVinIdx = DSP_VIRT_VIN_IDX_INVALID;
                        DSP_FindEmptyBit(pVirtVinBitMask, (UINT16)pDspInstInfo->MaxVirtVinNum, &VirtVinIdx, 0U/*FindOnly*/, 1U);
                        VinInfo.TimeDivisionVinId[0U][j] = VirtVinIdx;
                        if (VirtVinIdx == DSP_VIRT_VIN_IDX_INVALID) {
                            ExitJLoop = 1U;
                            Rval = DSP_ERR_0006;
                            AmbaLL_LogUInt5("VirtVin shortage [0x%X][%d]", Rval, __LINE__, 0U, 0U, 0U);
                        }
                    } else {
                        //No Spared Virtual Vin
                        ExitJLoop = 1U;
                        Rval = DSP_ERR_0006;
                        AmbaLL_LogUInt5("VirtVin shortage [0x%X][%d]", Rval, __LINE__, 0U, 0U, 0U);
                    }
                    if (ExitJLoop == 1U) {
                        break;
                    }
                }
            } else {
                //Clear relative TimeDivision setting
                (void)dsp_osal_memset(&VinInfo.TimeDivisionVinId[0U][0U], 0xFFFF, sizeof(UINT16)*AMBA_DSP_MAX_VIN_TD_NUM);
            }
            HL_SetVinInfo(HL_MTX_OPT_SET, i, &VinInfo);
        }
    }

    return Rval;
}

static inline UINT32 HL_LvSysPreprocOnVirtVinPivRaw(UINT32 *pVirtVinBitMask,
                                                    const CTX_DSP_INST_INFO_s *pDspInstInfo)
{
    UINT32 Rval = OK, VirtVinNum = 0U;
    UINT16 VirtVinIdx;
    CTX_STILL_INFO_s StlResInfo = {0};
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);
    if ((Resource->MaxStlRawInputWidth > 0U) && (Resource->MaxStlRawInputHeight > 0U)) {
        DSP_Bit2Cnt(*pVirtVinBitMask, &VirtVinNum);
        if (VirtVinNum < (UINT32)pDspInstInfo->MaxVirtVinNum) {
            VirtVinIdx = DSP_VIRT_VIN_IDX_INVALID;
            DSP_FindEmptyBit(pVirtVinBitMask, (UINT16)pDspInstInfo->MaxVirtVinNum, &VirtVinIdx, 0U/*FindOnly*/, 1U);
            if (VirtVinIdx != DSP_VIRT_VIN_IDX_INVALID) {
                HL_GetStlInfo(HL_MTX_OPT_GET, &StlResInfo);
                StlResInfo.RawInVirtVinId = VirtVinIdx;
                HL_SetStlInfo(HL_MTX_OPT_SET, &StlResInfo);
            } else {
                HL_GetStlInfo(HL_MTX_OPT_GET, &StlResInfo);
                StlResInfo.RawInVirtVinId = DSP_VIRT_VIN_IDX_INVALID;
                HL_SetStlInfo(HL_MTX_OPT_SET, &StlResInfo);
                Rval = DSP_ERR_0006;
                AmbaLL_LogUInt5("VirtVin shortage [0x%X][%d]", Rval, __LINE__, 0U, 0U, 0U);
            }
        } else {
            //No Spared Virtual Vin
            Rval = DSP_ERR_0006;
            AmbaLL_LogUInt5("VirtVin shortage [0x%X][%d]", Rval, __LINE__, 0U, 0U, 0U);
        }
    }

    return Rval;
}

static inline UINT32 HL_LvSysPreprocOnVirtVinPivYuv(UINT32 *pVirtVinBitMask,
                                                    const CTX_DSP_INST_INFO_s *pDspInstInfo)
{
    UINT32 Rval = OK, VirtVinNum = 0U;
    UINT16 VirtVinIdx;
    CTX_STILL_INFO_s StlResInfo = {0};
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);
    if ((Resource->MaxStlYuvInputWidth > 0U) && (Resource->MaxStlYuvInputHeight > 0U)) {
        DSP_Bit2Cnt(*pVirtVinBitMask, &VirtVinNum);
        if (VirtVinNum < (UINT32)pDspInstInfo->MaxVirtVinNum) {
            VirtVinIdx = DSP_VIRT_VIN_IDX_INVALID;
            DSP_FindEmptyBit(pVirtVinBitMask, (UINT16)pDspInstInfo->MaxVirtVinNum, &VirtVinIdx, 0U/*FindOnly*/, 1U);
            if (VirtVinIdx != DSP_VIRT_VIN_IDX_INVALID) {
                HL_GetStlInfo(HL_MTX_OPT_GET, &StlResInfo);
                StlResInfo.YuvInVirtVinId = VirtVinIdx;
                HL_SetStlInfo(HL_MTX_OPT_SET, &StlResInfo);
            } else {
                HL_GetStlInfo(HL_MTX_OPT_GET, &StlResInfo);
                StlResInfo.YuvInVirtVinId = DSP_VIRT_VIN_IDX_INVALID;
                HL_SetStlInfo(HL_MTX_OPT_SET, &StlResInfo);
                Rval = DSP_ERR_0006;
                AmbaLL_LogUInt5("VirtVin shortage [0x%X][%d]", Rval, __LINE__, 0U, 0U, 0U);
            }
        } else {
            //No Spared Virtual Vin
            Rval = DSP_ERR_0006;
            AmbaLL_LogUInt5("VirtVin shortage [0x%X][%d]", Rval, __LINE__, 0U, 0U, 0U);
        }
    }

    return Rval;
}

static inline UINT32 HL_LvSysPreprocOnVirtVinYuvEnc(UINT32 *pVirtVinBitMask,
                                                    const CTX_DSP_INST_INFO_s *pDspInstInfo)
{
    UINT32 Rval = OK, VirtVinNum = 0U;
    UINT16 VirtVinIdx;
    CTX_STILL_INFO_s StlResInfo = {0};
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);
    if ((Resource->MaxStlYuvEncWidth > 0U) && (Resource->MaxStlYuvEncHeight > 0U)) {
        DSP_Bit2Cnt(*pVirtVinBitMask, &VirtVinNum);
        if (VirtVinNum < (UINT32)pDspInstInfo->MaxVirtVinNum) {
            VirtVinIdx = DSP_VIRT_VIN_IDX_INVALID;
            DSP_FindEmptyBit(pVirtVinBitMask, (UINT16)pDspInstInfo->MaxVirtVinNum, &VirtVinIdx, 0U/*FindOnly*/, 1U);
            if (VirtVinIdx != DSP_VIRT_VIN_IDX_INVALID) {
                HL_GetStlInfo(HL_MTX_OPT_GET, &StlResInfo);
                StlResInfo.YuvEncVirtVinId = VirtVinIdx;
                HL_SetStlInfo(HL_MTX_OPT_SET, &StlResInfo);
            } else {
                HL_GetStlInfo(HL_MTX_OPT_GET, &StlResInfo);
                StlResInfo.YuvEncVirtVinId = DSP_VIRT_VIN_IDX_INVALID;
                HL_SetStlInfo(HL_MTX_OPT_SET, &StlResInfo);
                Rval = DSP_ERR_0006;
                AmbaLL_LogUInt5("VirtVin shortage [0x%X][%d]", Rval, __LINE__, 0U, 0U, 0U);
            }
        } else {
            //No Spared Virtual Vin
            Rval = DSP_ERR_0006;
            AmbaLL_LogUInt5("VirtVin shortage [0x%X][%d]", Rval, __LINE__, 0U, 0U, 0U);
        }
    }

    return Rval;
}

static inline UINT32 HL_LvSysPreprocOnVirtVinTimeLapse(UINT32 *pVirtVinBitMask,
                                                       const CTX_DSP_INST_INFO_s *pDspInstInfo,
                                                       const CTX_RESOURCE_INFO_s *pResource)
{
    UINT32 Rval = OK, VirtVinNum = 0U;
    UINT16 i, VirtVinIdx;
    CTX_TIMELAPSE_INFO_s TimeLapseInfo = {0};

    if (pResource->MaxTimeLapseNum > 0U) {
        for (i=0; i < pResource->MaxTimeLapseNum; i++) {
            DSP_Bit2Cnt(*pVirtVinBitMask, &VirtVinNum);
            if (VirtVinNum < (UINT32)pDspInstInfo->MaxVirtVinNum) {
                VirtVinIdx = DSP_VIRT_VIN_IDX_INVALID;
                DSP_FindEmptyBit(pVirtVinBitMask, (UINT16)pDspInstInfo->MaxVirtVinNum, &VirtVinIdx, 0U/*FindOnly*/, 1U);
                if (VirtVinIdx != DSP_VIRT_VIN_IDX_INVALID) {
                    HL_GetTimeLapseInfo(HL_MTX_OPT_GET, i, &TimeLapseInfo);
                    TimeLapseInfo.VirtVinId = VirtVinIdx;
                    TimeLapseInfo.Status = ENC_TIMELAPSE_STATUS_IDLE;
                    HL_SetTimeLapseInfo(HL_MTX_OPT_SET, i, &TimeLapseInfo);
                    AmbaLL_LogUInt5("TimeLapse[%d] VirtVinId[%d]", i, VirtVinIdx, 0U, 0U, 0U);
                } else {
                    HL_GetTimeLapseInfo(HL_MTX_OPT_GET, i, &TimeLapseInfo);
                    TimeLapseInfo.VirtVinId = DSP_VIRT_VIN_IDX_INVALID;
                    TimeLapseInfo.Status = ENC_TIMELAPSE_STATUS_IDLE;
                    HL_SetTimeLapseInfo(HL_MTX_OPT_SET, i, &TimeLapseInfo);
                    Rval = DSP_ERR_0006;
                    AmbaLL_LogUInt5("VirtVin shortage [0x%X][%d]", Rval, __LINE__, 0U, 0U, 0U);
                }
            } else {
                //No Spared Virtual Vin
                Rval = DSP_ERR_0006;
                AmbaLL_LogUInt5("VirtVin shortage [0x%X][%d]", Rval, __LINE__, 0U, 0U, 0U);
            }
        }
    }

    return Rval;
}

#ifdef DUPLEX_DEC_SHARE_VIRT_VIN
static inline UINT32 HL_LvSysPreprocOnVirtVinDec(UINT32 *pVirtVinBitMask,
                                                 const CTX_DSP_INST_INFO_s *pDspInstInfo,
                                                 UINT16 *pDecSharedVirtVinId)
{
    UINT32 Rval = OK, VirtVinNum = 0U;
    UINT16 Idx, VirtVinIdx;
    CTX_VID_DEC_INFO_s VidDecInfo = {0};
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    HL_GetResourcePtr(&Resource);

    for (Idx = 0U; Idx < Resource->MaxViewZoneNum; Idx++) {
        UINT16 DecIdx = 0U;

        HL_GetViewZoneInfoPtr(Idx, &ViewZoneInfo);
        if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) {
            DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &DecIdx);
            if (*pDecSharedVirtVinId != DSP_VIRT_VIN_IDX_INVALID) {
                HL_GetVidDecInfo(HL_MTX_OPT_GET, DecIdx, &VidDecInfo);
                VidDecInfo.YuvInVirtVinId = *pDecSharedVirtVinId;
                HL_SetVidDecInfo(HL_MTX_OPT_SET, DecIdx, &VidDecInfo);
            } else {
                DSP_Bit2Cnt(*pVirtVinBitMask, &VirtVinNum);
                if (VirtVinNum < (UINT32)pDspInstInfo->MaxVirtVinNum) {
                    VirtVinIdx = DSP_VIRT_VIN_IDX_INVALID;
                    DSP_FindEmptyBit(pVirtVinBitMask, (UINT16)pDspInstInfo->MaxVirtVinNum, &VirtVinIdx, 0U/*FindOnly*/, 1U);

                    if (VirtVinIdx != DSP_VIRT_VIN_IDX_INVALID) {
                        HL_GetVidDecInfo(HL_MTX_OPT_GET, DecIdx, &VidDecInfo);
                        VidDecInfo.YuvInVirtVinId = VirtVinIdx;
                        HL_SetVidDecInfo(HL_MTX_OPT_SET, DecIdx, &VidDecInfo);
                        *pDecSharedVirtVinId = VirtVinIdx;
                    } else {
                        HL_GetVidDecInfo(HL_MTX_OPT_GET, DecIdx, &VidDecInfo);
                        VidDecInfo.YuvInVirtVinId = DSP_VIRT_VIN_IDX_INVALID;
                        HL_SetVidDecInfo(HL_MTX_OPT_SET, DecIdx, &VidDecInfo);
                        Rval = DSP_ERR_0006;
                        AmbaLL_LogUInt5("VirtVin shortage [0x%X][%d]", Rval, __LINE__, 0U, 0U, 0U);
                    }
                } else {
                    //No Spared Virtual Vin
                    Rval = DSP_ERR_0006;
                    AmbaLL_LogUInt5("VirtVin shortage [0x%X][%d]", Rval, __LINE__, 0U, 0U, 0U);
                }
            }
        }
    }
    return Rval;
}
#else
static inline UINT32 HL_LvSysPreprocOnVirtVinDec(UINT32 *pVirtVinBitMask,
                                                 const CTX_DSP_INST_INFO_s *pDspInstInfo)
{
    UINT32 Rval = OK, VirtVinNum = 0U;
    UINT16 Idx, VirtVinIdx;
    CTX_VID_DEC_INFO_s VidDecInfo = {0};
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    HL_GetResourcePtr(&Resource);

    for (Idx = 0U; Idx < Resource->MaxViewZoneNum; Idx++) {
        UINT16 DecIdx = 0U;

        HL_GetViewZoneInfoPtr(Idx, &ViewZoneInfo);
        if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) {
            DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &DecIdx);
            DSP_Bit2Cnt(*pVirtVinBitMask, &VirtVinNum);
            if (VirtVinNum < (UINT32)pDspInstInfo->MaxVirtVinNum) {
                VirtVinIdx = DSP_VIRT_VIN_IDX_INVALID;
                DSP_FindEmptyBit(pVirtVinBitMask, (UINT16)pDspInstInfo->MaxVirtVinNum, &VirtVinIdx, 0U/*FindOnly*/, 1U);

                if (VirtVinIdx != DSP_VIRT_VIN_IDX_INVALID) {
                    HL_GetVidDecInfo(HL_MTX_OPT_GET, DecIdx, &VidDecInfo);
                    VidDecInfo.YuvInVirtVinId = VirtVinIdx;
                    HL_SetVidDecInfo(HL_MTX_OPT_SET, DecIdx, &VidDecInfo);
                } else {
                    HL_GetVidDecInfo(HL_MTX_OPT_GET, DecIdx, &VidDecInfo);
                    VidDecInfo.YuvInVirtVinId = DSP_VIRT_VIN_IDX_INVALID;
                    HL_SetVidDecInfo(HL_MTX_OPT_SET, DecIdx, &VidDecInfo);
                    Rval = DSP_ERR_0006;
                    AmbaLL_LogUInt5("VirtVin shortage [0x%X][%d]", Rval, __LINE__, 0U, 0U, 0U);
                }
            } else {
                //No Spared Virtual Vin
                Rval = DSP_ERR_0006;
                AmbaLL_LogUInt5("VirtVin shortage [0x%X][%d]", Rval, __LINE__, 0U, 0U, 0U);
            }
        }
    }
    return Rval;
}
#endif

static inline UINT32 HL_LvSysPreprocOnVirtVinTestEnc(UINT32 *pVirtVinBitMask,
                                                     const CTX_DSP_INST_INFO_s *pDspInstInfo,
                                                     UINT16 *pTestEncVirtVinId)
{
    UINT32 Rval = OK, VirtVinNum = 0U;
    UINT16 VirtVinIdx;

    DSP_Bit2Cnt(*pVirtVinBitMask, &VirtVinNum);
    if (VirtVinNum < (UINT32)pDspInstInfo->MaxVirtVinNum) {
        VirtVinIdx = DSP_VIRT_VIN_IDX_INVALID;
        DSP_FindEmptyBit(pVirtVinBitMask, (UINT16)pDspInstInfo->MaxVirtVinNum, &VirtVinIdx, 0U/*FindOnly*/, 1U);
        if (VirtVinIdx != DSP_VIRT_VIN_IDX_INVALID) {
            *pTestEncVirtVinId = VirtVinIdx;
        } else {
            *pTestEncVirtVinId = DSP_VIRT_VIN_IDX_INVALID;
            Rval = DSP_ERR_0006;
            AmbaLL_LogUInt5("VirtVin shortage [0x%X][%d]", Rval, __LINE__, 0U, 0U, 0U);
        }
    } else {
        //No Spared Virtual Vin
        Rval = DSP_ERR_0006;
        AmbaLL_LogUInt5("VirtVin shortage [0x%X][%d]", Rval, __LINE__, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_LvSysPreprocOnVprocStl(UINT32 *pVprocBitMask,
                                               const CTX_DSP_INST_INFO_s *pDspInstInfo,
                                               const CTX_RESOURCE_INFO_s *pResource)
{
    UINT32 Rval = OK, VprocNum = 0U;
    UINT16 VprocIdx;
    CTX_STILL_INFO_s StlResInfo = {0};
    CTX_DSP_INST_INFO_s TmpDspInstInfo;

#ifdef SUPPORT_VPROC_RT_RESTART
    DSP_Bit2Cnt(*pVprocBitMask, &VprocNum);
    if (VprocNum < (UINT32)pDspInstInfo->MaxVpocNum) {
        VprocIdx = DSP_VPROC_IDX_INVALID;
        DSP_FindEmptyBit(pVprocBitMask, (UINT16)pDspInstInfo->MaxVpocNum, &VprocIdx, 0U/*FindOnly*/, 1U);
        if (VprocIdx != DSP_VPROC_IDX_INVALID) {
            HL_GetStlInfo(HL_MTX_OPT_GET, &StlResInfo);
            if ((pResource->MaxStlRawInputWidth > 0U) && (pResource->MaxStlRawInputHeight > 0U)) {
                StlResInfo.RawInVprocId = VprocIdx;
                StlResInfo.RawInVprocPin = DSP_STLPROC_R2Y_DEFAULT_PIN;
                HL_GetDspInstance(HL_MTX_OPT_GET, &TmpDspInstInfo);
                TmpDspInstInfo.VprocPurpose[VprocIdx] = VPROC_PURPOSE_STILL;
                HL_SetDspInstance(HL_MTX_OPT_SET, &TmpDspInstInfo);
            } else {
                StlResInfo.RawInVprocId = DSP_VPROC_IDX_INVALID;
                StlResInfo.RawInVprocPin = DSP_VPROC_PIN_NUM;
            }
            if ((pResource->MaxStlYuvInputWidth > 0U) && (pResource->MaxStlYuvInputHeight > 0U)) {
                StlResInfo.YuvInVprocId = VprocIdx;
                StlResInfo.YuvInVprocPin = DSP_STLPROC_Y2Y_DEFAULT_PIN;
                HL_GetDspInstance(HL_MTX_OPT_GET, &TmpDspInstInfo);
                TmpDspInstInfo.VprocPurpose[VprocIdx] = VPROC_PURPOSE_STILL;
                HL_SetDspInstance(HL_MTX_OPT_SET, &TmpDspInstInfo);
            } else {
                StlResInfo.YuvInVprocId = DSP_VPROC_IDX_INVALID;
                StlResInfo.YuvInVprocPin = DSP_VPROC_PIN_NUM;
            }
            HL_SetStlInfo(HL_MTX_OPT_SET, &StlResInfo);
        } else {
            Rval = DSP_ERR_0006;
            AmbaLL_LogUInt5("Vproc shortage [0x%X][%d]", Rval, __LINE__, 0U, 0U, 0U);
        }
    } else {
        //No Spared Virtual Vin
        Rval = DSP_ERR_0006;
        AmbaLL_LogUInt5("Vproc shortage [0x%X][%d]", Rval, __LINE__, 0U, 0U, 0U);
    }
#else //ndef SUPPORT_VPROC_RT_RESTART
    if ((Resource->MaxStlRawInputWidth > 0U) && (Resource->MaxStlRawInputHeight > 0U)) {
        DSP_Bit2Cnt(*pVprocBitMask, &VprocNum);
        if (VprocNum < (UINT32)pDspInstInfo->MaxVpocNum) {
            VprocIdx = DSP_VPROC_IDX_INVALID;
            DSP_FindEmptyBit(pVprocBitMask, (UINT16)pDspInstInfo->MaxVpocNum, &VprocIdx, 0U/*FindOnly*/, 1U);
            if (VprocIdx != DSP_VPROC_IDX_INVALID) {
                HL_GetStlInfo(HL_MTX_OPT_GET, &StlResInfo);
                StlResInfo.RawInVprocId = VprocIdx;
                StlResInfo.RawInVprocPin = DSP_STLPROC_R2Y_DEFAULT_PIN;
                HL_GetDspInstance(HL_MTX_OPT_GET, &TmpDspInstInfo);
                TmpDspInstInfo.VprocPurpose[VprocIdx] = VPROC_PURPOSE_STILL;
                HL_SetDspInstance(HL_MTX_OPT_SET, &TmpDspInstInfo);
                HL_SetStlInfo(HL_MTX_OPT_SET, &StlResInfo);
            } else {
                Rval = DSP_ERR_0006;
                AmbaLL_LogUInt5("Vproc shortage [0x%X][%d]", Rval, __LINE__, 0U, 0U, 0U);
            }
        } else {
            //No Spared Virtual Vin
            Rval = DSP_ERR_0006;
            AmbaLL_LogUInt5("Vproc shortage [0x%X][%d]", Rval, __LINE__, 0U, 0U, 0U);
        }
    }
    if ((Resource->MaxStlYuvInputWidth > 0U) && (Resource->MaxStlYuvInputHeight > 0U)) {
        DSP_Bit2Cnt(*pVprocBitMask, &VprocNum);
        if (VprocNum < (UINT32)pDspInstInfo->MaxVpocNum) {
            VprocIdx = DSP_VPROC_IDX_INVALID;
            DSP_FindEmptyBit(pVprocBitMask, (UINT16)pDspInstInfo->MaxVpocNum, &VprocIdx, 0U/*FindOnly*/, 1U);
            if (VprocIdx != DSP_VPROC_IDX_INVALID) {
                HL_GetStlInfo(HL_MTX_OPT_GET, &StlResInfo);
                StlResInfo.YuvInVprocId = VprocIdx;
                StlResInfo.YuvInVprocPin = DSP_STLPROC_Y2Y_DEFAULT_PIN;
                HL_GetDspInstance(HL_MTX_OPT_GET, &TmpDspInstInfo);
                TmpDspInstInfo.VprocPurpose[VprocIdx] = VPROC_PURPOSE_STILL;
                HL_SetDspInstance(HL_MTX_OPT_SET, &TmpDspInstInfo);
                HL_SetStlInfo(HL_MTX_OPT_SET, &StlResInfo);
            } else {
                Rval = DSP_ERR_0006;
                AmbaLL_LogUInt5("Vproc shortage [0x%X][%d]", Rval, __LINE__, 0U, 0U, 0U);
            }
        } else {
            //No Spared Virtual Vin
            Rval = DSP_ERR_0006;
            AmbaLL_LogUInt5("Vproc shortage [0x%X][%d]", Rval, __LINE__, 0U, 0U, 0U);
        }

        DSP_Bit2Cnt(*pVprocBitMask, &VprocNum);
        if (VprocNum < (UINT32)pDspInstInfo->MaxVpocNum) {
            VprocIdx = DSP_VPROC_IDX_INVALID;
            DSP_FindEmptyBit(pVprocBitMask, (UINT16)pDspInstInfo->MaxVpocNum, &VprocIdx, 0U/*FindOnly*/);
            if (VprocIdx != DSP_VPROC_IDX_INVALID) {
                HL_GetStlInfo(HL_MTX_OPT_GET, &StlResInfo);
                StlResInfo.Yuv422InVprocId = VprocIdx;
                HL_GetDspInstance(HL_MTX_OPT_GET, &TmpDspInstInfo);
                TmpDspInstInfo.VprocPurpose[VprocIdx] = VPROC_PURPOSE_STILL;
                HL_SetDspInstance(HL_MTX_OPT_SET, &TmpDspInstInfo);
                HL_SetStlInfo(HL_MTX_OPT_SET, &StlResInfo);
            } else {
                Rval = DSP_ERR_0006;
                AmbaLL_LogUInt5("Vproc shortage [0x%X][%d]", Rval, __LINE__, 0U, 0U, 0U);
            }
        } else {
            //No Spared Virtual Vin
            Rval = DSP_ERR_0006;
            AmbaLL_LogUInt5("Vproc shortage [0x%X][%d]", Rval, __LINE__, 0U, 0U, 0U);
        }
    }
#endif

    return Rval;
}

/* Collect all possible requirement Here and request resource in one shot */
static inline UINT32 HL_LiveivewSystemPreProcImpl(void)
{
    UINT32 Rval, VirtVinBitMask, VprocBitMask = 0U;
    UINT16 i;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_DSP_INST_INFO_s DspInstInfo = {0};
    UINT8 NeedAvcFmt, NeedHevcFmt, NeedJpgFmt, JpgEncEnable = 0U;
    UINT8 HasDecResource;
    UINT16 DecSharedVirtVinId = DSP_VIRT_VIN_IDX_INVALID;
    UINT16 TestEncVirtVinId = DSP_VIRT_VIN_IDX_INVALID;

    HL_GetDspInstance(HL_MTX_OPT_ALL, &DspInstInfo);
    HL_GetResourcePtr(&Resource);
    VirtVinBitMask = (UINT16)Resource->MaxVirtVinBit;

    /**************
     * VirtualVin *
     **************/
    // VirtVin for Vin TimeDivision, consider PhysicalVin at this stage
    Rval = HL_LvSysPreprocOnVirtVinTD(&VirtVinBitMask, &DspInstInfo);

    // VirtualVin for PIV RawInput
    if (Rval == OK) {
        Rval = HL_LvSysPreprocOnVirtVinPivRaw(&VirtVinBitMask, &DspInstInfo);
    }

    // VirtualVin for PIV YuvInput
    if (Rval == OK) {
        Rval = HL_LvSysPreprocOnVirtVinPivYuv(&VirtVinBitMask, &DspInstInfo);
    }

    // VirtualVin for YuvMemEnc
    for (i=0; i < AMBA_DSP_MAX_STREAM_NUM; i++) {
        NeedJpgFmt = (UINT8)DSP_GetU8Bit(Resource->MaxStrmFmt[i], 2U/*JPG*/, 1U);
        NeedAvcFmt = (UINT8)DSP_GetU8Bit(Resource->MaxStrmFmt[i], 0U/*AVC*/, 1U);
        NeedHevcFmt = (UINT8)DSP_GetU8Bit(Resource->MaxStrmFmt[i], 1U/*HEVC*/, 1U);
        if ((1U == NeedJpgFmt) &&
            (0U == NeedAvcFmt) &&
            (0U == NeedHevcFmt)) {
            JpgEncEnable = 1U;
        }
    }
    if ((Rval == OK) &&
        (JpgEncEnable > 0U)) {
        Rval = HL_LvSysPreprocOnVirtVinYuvEnc(&VirtVinBitMask, &DspInstInfo);
    }

    // TimeLapse VirtualVin for TimeLapase
    if (Rval == OK) {
        Rval = HL_LvSysPreprocOnVirtVinTimeLapse(&VirtVinBitMask, &DspInstInfo, Resource);
    }

    // YuvInput VirtualVin for decode
    HasDecResource = HL_HasDecResource();
    if ((Rval == OK) &&
        (HasDecResource == 1U)) {
        DecSharedVirtVinId = DspInstInfo.DecSharedVirtVinId;
#ifdef DUPLEX_DEC_SHARE_VIRT_VIN
        Rval = HL_LvSysPreprocOnVirtVinDec(&VirtVinBitMask, &DspInstInfo, &DecSharedVirtVinId);
#else
        Rval = HL_LvSysPreprocOnVirtVinDec(&VirtVinBitMask, &DspInstInfo);
#endif

        /* Update DecVirtVin */
        HL_GetDspInstance(HL_MTX_OPT_GET, &DspInstInfo);
        DspInstInfo.DecSharedVirtVinId = DecSharedVirtVinId;
        HL_SetDspInstance(HL_MTX_OPT_SET, &DspInstInfo);
    }

    // Enc TestFrm
    if ((Rval == OK) &&
        (Resource->TestFrmNumOnStage[DSP_TEST_STAGE_VDSP_0] > 0U)) {
        Rval = HL_LvSysPreprocOnVirtVinTestEnc(&VirtVinBitMask, &DspInstInfo, &TestEncVirtVinId);

        /* Update TestEncVirtVin */
        HL_GetDspInstance(HL_MTX_OPT_GET, &DspInstInfo);
        DspInstInfo.TestEncVirtVinId = TestEncVirtVinId;
        HL_SetDspInstance(HL_MTX_OPT_SET, &DspInstInfo);
    }

    /*********
     * Vproc *
     *********/
    // Update VprocBitMask base on Liveview request
    for (i=0; i < Resource->MaxViewZoneNum; i++) {
        DSP_SetBit(&VprocBitMask, i);
        HL_GetDspInstance(HL_MTX_OPT_GET, &DspInstInfo);
        DspInstInfo.VprocPurpose[i] = VPROC_PURPOSE_USER_DEFINED;
        HL_SetDspInstance(HL_MTX_OPT_SET, &DspInstInfo);
    }

    // Vproc for still
    if ((Resource->MaxStlMainWidth > 0U) && (Resource->MaxStlMainHeight > 0U)) {
        Rval = HL_LvSysPreprocOnVprocStl(&VprocBitMask, &DspInstInfo, Resource);
    }

    HL_GetDspInstance(HL_MTX_OPT_GET, &DspInstInfo);
    DspInstInfo.VirtVinBitMask = VirtVinBitMask;
    DspInstInfo.VprocBitMask = VprocBitMask;
    HL_SetDspInstance(HL_MTX_OPT_SET, &DspInstInfo);

    return Rval;
}

static inline UINT32 HL_LiveivewSystemSetupImpl(const UINT8 WriteMode)
{
    UINT32 Rval;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_dsp_config_t *SysSetup = HL_DefCtxCmdBufPtrSys;
    cmd_dsp_hal_inf_t *SysHalSetup = HL_DefCtxCmdBufPtrHal;

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&SysSetup, &CmdBufferAddr);
    Rval = HL_FillLiveviewSystemSetup(SysSetup);

    if (Rval == OK) {
        Rval = AmbaHL_CmdDspConfig(WriteMode, SysSetup);
    }

    HL_RelCmdBuffer(CmdBufferId);
    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&SysHalSetup, &CmdBufferAddr);
    if (Rval == OK) {
        Rval = HL_FillSystemHalSetup(SysHalSetup);
    }

    if (Rval == OK) {
        Rval = AmbaHL_CmdDspHalInf(WriteMode, SysHalSetup);
    }
    HL_RelCmdBuffer(CmdBufferId);
    return Rval;
}

static inline void HL_LiveviewDspDebugLevelImpl(const UINT8 WriteMode)
{
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_set_debug_level_t *DebugLevel = HL_DefCtxCmdBufPtrDbgLvl;
    cmd_print_th_disable_mask_t *DebugThread = HL_DefCtxCmdBufPtrDbgthd;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&DebugLevel, &CmdBufferAddr);
    if ((Resource->DspDbgLvl == 0U) && (Resource->DspDbgMod == 0U) && (Resource->DspDbgThrd == 0U)) {
        DebugLevel->level = 1U;
        DebugLevel->module = 0xFFFFFFFFU;
        (void)AmbaHL_CmdDspSetDebugLevel(WriteMode, DebugLevel);
    } else {
        DebugLevel->level = Resource->DspDbgLvl;
        DebugLevel->module = Resource->DspDbgMod;
        (void)AmbaHL_CmdDspSetDebugLevel(WriteMode, DebugLevel);
    }
    HL_RelCmdBuffer(CmdBufferId);

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&DebugThread, &CmdBufferAddr);
    if ((Resource->DspDbgThrd == 0U) && (Resource->DspDbgThrdValid == 0U)) {
        DebugThread->orccode_mask_valid = (UINT8)1U;
        DebugThread->orcme_mask_valid = (UINT8)1U;
        DebugThread->orcmdxf_mask_valid = (UINT8)1U;

        DebugThread->orccode_mask = (UINT16)0U;
        DebugThread->orcme_mask = (UINT8)0U;
        DebugThread->orcmdxf_mask = (UINT8)0U;
        (void)AmbaHL_CmdDspPrintThreadDisableMask(WriteMode, DebugThread);
    } else {
        DebugThread->orccode_mask_valid = (UINT8)DSP_GetBit(Resource->DspDbgThrdValid, 0U, 1U);
        DebugThread->orcme_mask_valid = (UINT8)DSP_GetBit(Resource->DspDbgThrdValid, 1U, 1U);
        DebugThread->orcmdxf_mask_valid = (UINT8)DSP_GetBit(Resource->DspDbgThrdValid, 2U, 1U);

        DebugThread->orccode_mask = (UINT16)DSP_GetBit(Resource->DspDbgThrd, 0U, 12U);
        DebugThread->orcme_mask = (UINT8)DSP_GetBit(Resource->DspDbgThrd, 12U, 2U);
        DebugThread->orcmdxf_mask = (UINT8)DSP_GetBit(Resource->DspDbgThrd, 14U, 2U);
        (void)AmbaHL_CmdDspPrintThreadDisableMask(WriteMode, DebugThread);
    }
    HL_RelCmdBuffer(CmdBufferId);
}

static inline void HL_FillDramCfgOnEnc(DSP_DRAM_CFG_s *pDramCfg,
                                       UINT32 *pWorkSize,
                                       const CTX_RESOURCE_INFO_s *pResource)
{
    UINT16 i;
    UINT32 Mask = 0U;
    enc_cfg_t *pDspStrmEncCfg = NULL;

    DSP_SetBit(&Mask, DSP_MEM_AREA_ENC_BIT);
    pDramCfg->EncNum = (UINT8)pResource->MaxEncodeStream;
    for (i=0U; i<pResource->MaxEncodeStream; i++) {
        HL_GetPointerDspEncStrmCfg(i, &pDspStrmEncCfg);
        if (pResource->MaxGopM[i] > 1U) {
            pDramCfg->ReconNum[i] = DEFAULT_RECON_NUM_IPB;
        } else {
            pDramCfg->ReconNum[i] = DEFAULT_RECON_NUM_IP;
        }
        if (pDspStrmEncCfg != NULL) {
            pDramCfg->ReconNum[i] += (UINT8)pDspStrmEncCfg->max_rec_fb_num;
            pDramCfg->MaxEncWidth[i] = pDspStrmEncCfg->max_enc_width;
            pDramCfg->MaxEncHieght[i] = pDspStrmEncCfg->max_enc_height;
        }
    }

    (void)HL_CalcWorkingBuffer(Mask, pDramCfg, pWorkSize);
}

static inline void HL_FillDramCfgOnVproc(DSP_DRAM_CFG_s *pDramCfg,
                                         UINT32 *pWorkSize,
                                         const cmd_dsp_vproc_flow_max_cfg_t *pResVprocSetup)
{
    UINT32 Mask = 0U;

    DSP_SetBit(&Mask, DSP_MEM_AREA_VPROC_COMM_BIT);

    pDramCfg->VprocComm0Fmt = (UINT8)pResVprocSetup->prev_com0_ch_fmt;
    pDramCfg->VprocComm1Fmt = (UINT8)pResVprocSetup->prev_com1_ch_fmt;

    pDramCfg->MaxVprocEfctComm0FrmBufNum = (UINT8)pResVprocSetup->prev_com0_fb_num;
    pDramCfg->MaxVprocEfctComm1FrmBufNum = (UINT8)pResVprocSetup->prev_com1_fb_num;

    pDramCfg->MaxVprocEfctMainFrmBufNum = (UINT8)pResVprocSetup->postp_main_fb_num;
    pDramCfg->MaxVprocEfctMainMeFrmBufNum = (UINT8)pResVprocSetup->postp_main_me01_fb_num;
    pDramCfg->MaxVprocEfctPipFrmBufNum = (UINT8)pResVprocSetup->postp_pip_fb_num;
    pDramCfg->MaxVprocEfctPipMeFrmBufNum = (UINT8)pResVprocSetup->postp_pip_me01_fb_num;

    pDramCfg->MaxComm0Width = pResVprocSetup->prev_com0_w_max;
    pDramCfg->MaxComm0Hieght = pResVprocSetup->prev_com0_h_max;
    pDramCfg->MaxComm1Width = pResVprocSetup->prev_com1_w_max;
    pDramCfg->MaxComm1Hieght = pResVprocSetup->prev_com1_h_max;
    pDramCfg->MaxEfctMainWidth = pResVprocSetup->postp_main_w_max;
    pDramCfg->MaxEfctMainHeight = pResVprocSetup->postp_main_h_max;
    pDramCfg->MaxEfctPipWidth = pResVprocSetup->postp_pip_w_max;
    pDramCfg->MaxEfctPipHieght = pResVprocSetup->postp_pip_h_max;
    (void)HL_CalcWorkingBuffer(Mask, pDramCfg, pWorkSize);
}

static inline void HL_FillDramCfgOnVin(DSP_DRAM_CFG_s *pDramCfg,
                                       UINT32 *pWorkSize,
                                       UINT16 VinId,
                                       const cmd_dsp_vin_flow_max_cfg_t *pResVinSetup)
{
    UINT32 Mask = 0U;

    DSP_SetBit(&Mask, DSP_MEM_AREA_VIN_BIT);

    DSP_SetBit(&pDramCfg->VinBitMask, VinId);
    if (pResVinSetup->is_sensor_raw_out_enabled == (UINT8)1U) {
        pDramCfg->VinCmpr[VinId] = (pResVinSetup->raw_width_is_byte_width == 1U)? 1U: 0U;
        pDramCfg->VinMaxCapOutWidth[VinId] = pResVinSetup->max_raw_cap_width;
        pDramCfg->VinMaxCapOutHeight[VinId] = pResVinSetup->max_raw_cap_height;
        pDramCfg->CapOutBufNum[VinId] = (UINT8)pResVinSetup->max_raw_cap_dbuf_num;
    } else if (pResVinSetup->is_yuv422_out_enabled == (UINT8)1U) {
        pDramCfg->VinMaxCapOutWidth[VinId] = pResVinSetup->max_yuv422_out_width;
        pDramCfg->VinMaxCapOutHeight[VinId] = pResVinSetup->max_yuv422_out_height;
        pDramCfg->CapOutBufNum[VinId] = (UINT8)pResVinSetup->max_yuv422_out_dbuf_num;
    } else {
        // DO NOTHING
    }

    if (pResVinSetup->is_contrast_enhance_out_enabled == (UINT8)1U) {
        DSP_SetBit(&pDramCfg->VinCeBitMask, VinId);
        pDramCfg->VinMaxCeOutWidth[VinId] = pResVinSetup->max_contrast_enhance_out_width;
        pDramCfg->VinMaxCeOutHeight[VinId] = pResVinSetup->max_contrast_enhance_out_height;
        pDramCfg->CeOutBufNum[VinId] = (UINT8)pResVinSetup->max_contrast_enhance_out_dbuf_num;
    }

    (void)HL_CalcWorkingBuffer(Mask, pDramCfg, pWorkSize);
    pDramCfg->VinBitMask = 0U;
    pDramCfg->VinCeBitMask = 0U;
}

static inline void HL_FillDramCfgOnDec(DSP_DRAM_CFG_s *pDramCfg,
                                       UINT32 *pWorkSize,
                                       const CTX_RESOURCE_INFO_s *pResource,
                                       const cmd_dsp_dec_flow_max_cfg_t *pDecFlwMaxCfg)
{
    UINT32 Mask = 0U;

    DSP_SetBit(&Mask, DSP_MEM_AREA_DEC_BIT);

    pDramCfg->DecNum = (UINT8)pResource->DecMaxStreamNum;
    pDramCfg->DecFmt = HL_GetDecFmtTotalBit();
    if (pDecFlwMaxCfg->max_frm_num_of_dpb > 0U) {
        pDramCfg->DecMaxDpb = (UINT8)pDecFlwMaxCfg->max_frm_num_of_dpb;
    } else {
        pDramCfg->DecMaxDpb = 4U;
    }
    pDramCfg->DecMaxWidth = pDecFlwMaxCfg->max_frm_width;
    pDramCfg->DecMaxHeight = pDecFlwMaxCfg->max_frm_height;
    (void)HL_CalcWorkingBuffer(Mask, pDramCfg, pWorkSize);
}

static inline void HL_LiveviewResourceSetupImpl(const UINT8 WriteMode)
{
    UINT32 Rval;
    UINT16 i, VinExist;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_dsp_enc_flow_max_cfg_t *ResEncSetup = HL_DefCtxCmdBufPtrEncMax;
    cmd_dsp_vproc_flow_max_cfg_t *ResVprocSetup = HL_DefCtxCmdBufPtrVpcMax;
    cmd_dsp_vin_flow_max_cfg_t *ResVinSetup = HL_DefCtxCmdBufPtrVinMax;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT32 WorkSize;
    DSP_DRAM_CFG_s DramCfg = {0};

    HL_GetResourcePtr(&Resource);
    WorkSize = Resource->ProfWorkSize;

    /* Follow H2, EncMaxCfg must issued before VprocMax for ShareRefEnc information syncing */
    if (Resource->MaxEncodeStream > 0U) {
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&ResEncSetup, &CmdBufferAddr);
        Rval = HL_FillSystemResourceEncodeSetup(ResEncSetup);
        if (Rval != OK) {
            AmbaLL_LogUInt5("ResEnc filling fail", 0U, 0U, 0U, 0U, 0U);
        } else {
            (void)AmbaHL_CmdDspEncFlowMaxCfg(WriteMode, ResEncSetup);
        }

        //DramCfg
        HL_FillDramCfgOnEnc(&DramCfg, &WorkSize, Resource);

        HL_RelCmdBuffer(CmdBufferId);
    }

    /* Vproc Max Cfg */
    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&ResVprocSetup, &CmdBufferAddr);
    Rval = HL_FillSystemResourceVprocSetup(ResVprocSetup);
    if (Rval != OK) {
        AmbaLL_LogUInt5("ResVproc filling fail", 0U, 0U, 0U, 0U, 0U);
    } else {
        (void)AmbaHL_CmdDspVprocFlowMaxCfg(WriteMode, ResVprocSetup);
    }

    //DramCfg
    HL_FillDramCfgOnVproc(&DramCfg, &WorkSize, ResVprocSetup);

    HL_RelCmdBuffer(CmdBufferId);

    /* Vin Max Cfg on desired */
    for (i = 0; i<DSP_VIN_MAX_NUM; i++) {
        VinExist = 0U;
        HL_GetVinExistence(i, &VinExist);
        if (VinExist > 0U) {
            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&ResVinSetup, &CmdBufferAddr);
            Rval = HL_FillSystemResourceVinSetup(ResVinSetup, i);
            if (Rval != OK) {
                AmbaLL_LogUInt5("ResVin[%d] filling fail", i, 0U, 0U, 0U, 0U);
            } else {
                (void)AmbaHL_CmdDspVinFlowMaxCfg(WriteMode, ResVinSetup);
            }

            //DramCfg
            HL_FillDramCfgOnVin(&DramCfg, &WorkSize, i, ResVinSetup);

            HL_RelCmdBuffer(CmdBufferId);
        }
    }

    /* Decode Flow Max Config */
    if (HL_HasDecResource() == 1U) {
        cmd_dsp_dec_flow_max_cfg_t *DecFlwMaxCfg = HL_DefCtxCmdBufPtrDecMax;

        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&DecFlwMaxCfg, &CmdBufferAddr);
        (void)HL_FillDecFlowMaxCfgSetup(DecFlwMaxCfg);
        (void)AmbaHL_CmdDspDecFlowMaxCfg(WriteMode, DecFlwMaxCfg);

        //DramCfg
        HL_FillDramCfgOnDec(&DramCfg, &WorkSize, Resource, DecFlwMaxCfg);

        HL_RelCmdBuffer(CmdBufferId);
    }

    /* Update Dram usage */
    HL_GetResourceLock(&Resource);
    Resource->ProfWorkSize = WorkSize;
    HL_GetResourceUnLock();
}

static inline void HL_LiveviewSystemActivateImpl(const UINT8 WriteMode)
{
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_dsp_activate_profile_t *ActProfile = HL_DefCtxCmdBufPtrActPrf;

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&ActProfile, &CmdBufferAddr);
    ActProfile->dsp_prof_id = DSP_PROF_STATUS_CAMERA;
    (void)AmbaHL_CmdDspActivateProfile(WriteMode, ActProfile);
    HL_RelCmdBuffer(CmdBufferId);
}

static inline void HL_LiveviewFpBindingImpl(const UINT8 WriteMode)
{
    UINT16 i;
    UINT32 Rval = OK;
    CTX_VID_DEC_INFO_s VidDecInfo = {0};
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    HL_GetResourcePtr(&Resource);

//FIXME, MultiChan and ViewZoneOI
    for (i = 0; (i<Resource->MaxViewZoneNum); i++) {
        if (0U == DSP_GetBit(Resource->ViewZoneActiveBit, i, 1U)) {
            continue;
        }

        HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
        if (ViewZoneInfo->Pipe == (UINT8)DSP_DRAM_PIPE_RAW_ONLY) {
            continue;
        }

        if (ViewZoneInfo->InputFromMemory != VIN_SRC_FROM_RECON) { //bind when encode
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_binding_cfg_t *FpBindCfg = HL_DefCtxCmdBufPtrBind;

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&FpBindCfg, &CmdBufferAddr);
            if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) {
                UINT32 DecIdx = 0U;
                UINT16 SrcPinId = 0U;

                DSP_Bit2Idx(ViewZoneInfo->SourceVin, &DecIdx);
                HL_GetVidDecInfo(HL_MTX_OPT_GET, (UINT16)DecIdx, &VidDecInfo);
                VidDecInfo.ViewZoneId = i;
                HL_SetVidDecInfo(HL_MTX_OPT_SET, (UINT16)DecIdx, &VidDecInfo);
                if (VidDecInfo.ExtYuvBuf.Pitch == 0U) {
                    Rval = HL_FillFpBindSetup(FpBindCfg, BIND_VIDEO_DECODE_TO_VPROC, 1U/*IsBind*/,
                                              (UINT16)DecIdx/*SrcId -> src_ch_id*/,
                                              0U/*SrcPinId -> xx*/,
                                              i/*StrmId -> dst_ch_id*/);
                    if (Rval == OK) {
                        HL_GetVidDecInfo(HL_MTX_OPT_GET, (UINT16)DecIdx, &VidDecInfo);
                        VidDecInfo.CurrentBind = BIND_VIDEO_DECODE_TO_VPROC;
                        HL_SetVidDecInfo(HL_MTX_OPT_SET, (UINT16)DecIdx, &VidDecInfo);
                    } else {
                        //
                    }
                } else {
                    /* Default Image is provided */
                    SrcPinId = (UINT16)HL_GetViewZoneLocalIndexOnVin(VidDecInfo.ViewZoneId);

                    Rval = HL_FillFpBindSetup(FpBindCfg, BIND_STILL_PROC, 1U/*IsBind*/,
                                              VidDecInfo.YuvInVirtVinId + AMBA_DSP_MAX_VIN_NUM/*SrcId -> src_fp_id*/,
                                              SrcPinId/*SrcPinId -> xx*/,
                                              VidDecInfo.ViewZoneId/*StrmId -> dst_ch_id*/);
                    if (Rval == OK) {
                        HL_GetVidDecInfo(HL_MTX_OPT_GET, (UINT16)DecIdx, &VidDecInfo);
                        VidDecInfo.CurrentBind = BIND_STILL_PROC;
                        HL_SetVidDecInfo(HL_MTX_OPT_SET, (UINT16)DecIdx, &VidDecInfo);
                    } else {
                        //
                    }
                }
            } else if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM) ||
                       (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_422) ||
                       (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_420)) {

                /* same as VIN_SRC_FROM_HW */
                if (ViewZoneInfo->ExtYuvBuf.Pitch == 0U) {
                    Rval = HL_FillFpBindSetup(FpBindCfg, BIND_VIDEO_YUVCONV,
                                              1U/*IsBind*/, i/*ViewZoneId*/, 0U/*SrcPinId*/, 0U/*StrmId*/);
                /* Feed with one yuv before boot case, for effect */
                } else {
                    UINT16 ViewZoneVinId;

                    (void)HL_GetViewZoneVinId(i, &ViewZoneVinId);
                    Rval = HL_FillFpBindSetup(FpBindCfg, BIND_STILL_PROC, 1U/*IsBind*/,
                                              ViewZoneVinId/*SrcId -> src_fp_id*/,
                                              0U/*SrcPinId -> xx*/,
                                              i/*StrmId -> dst_ch_id*/);
                    if (Rval == OK) {
                        HL_GetViewZoneInfoLock(i, &ViewZoneInfo);
                        ViewZoneInfo->CurrentBind = BIND_STILL_PROC;
                        HL_GetViewZoneInfoUnLock(i);
                    } else {
                        //
                    }
                }
            /* VIN_SRC_FROM_HW */
            } else {
                Rval = HL_FillFpBindSetup(FpBindCfg, BIND_VIDEO_YUVCONV,
                                          1U/*IsBind*/, i/*ViewZoneId*/, 0U/*SrcPinId*/, 0U/*StrmId*/);
            }
            if (Rval != OK) {
                AmbaLL_LogUInt5("FpBind filling fail", 0U, 0U, 0U, 0U, 0U);
            } else {
                (void)AmbaHL_CmdDspBindingCfg(WriteMode, FpBindCfg);
            }
            HL_RelCmdBuffer(CmdBufferId);
        }
    }
}

static inline void HL_InitExtPymdLndtBuf(UINT16 NumViewZone)
{
    UINT16 i;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    for (i = 0U; i < NumViewZone; i++) {
        HL_GetViewZoneInfoLock(i, &ViewZoneInfo);
        if ((ViewZoneInfo->PymdIsExtMem > 0U) &&
            (ViewZoneInfo->PymdAllocType == ALLOC_EXTERNAL_DISTINCT) &&
            (ViewZoneInfo->Pyramid.HierBit > 0U) &&
            (ViewZoneInfo->PymdYuvBufTbl[0U] != 0U)) {

            ViewZoneInfo->PymdBufferStatus = PYMD_BUF_STATUS_CONFIG;

        } else if ((ViewZoneInfo->PymdIsExtMem > 0U) &&
                   (ViewZoneInfo->PymdAllocType == ALLOC_EXTERNAL_CYCLIC) &&
                   (ViewZoneInfo->Pyramid.HierBit > 0U)) {
            ViewZoneInfo->PymdBufferStatus = PYMD_BUF_STATUS_CONFIG;
        } else {
            // DO NOTHING
        }
        if ((ViewZoneInfo->LndtIsExtMem > 0U) &&
            (ViewZoneInfo->LndtAllocType == ALLOC_EXTERNAL_DISTINCT) &&
            (ViewZoneInfo->LndtYuvBufTbl[0U] != 0U)) {

            ViewZoneInfo->LndtBufferStatus = LNDT_BUF_STATUS_CONFIG;
        } else if ((ViewZoneInfo->LndtIsExtMem > 0U) &&
                   (ViewZoneInfo->LndtAllocType == ALLOC_EXTERNAL_CYCLIC)) {

            ViewZoneInfo->LndtBufferStatus = LNDT_BUF_STATUS_CONFIG;
        } else {
            // DO NOTHING
        }
        if ((ViewZoneInfo->MainY12IsExtMem > 0U) &&
            (ViewZoneInfo->MainY12AllocType == ALLOC_EXTERNAL_DISTINCT) &&
            (ViewZoneInfo->MainY12BufTbl[0U] != 0U)) {
            ViewZoneInfo->MainY12BufferStatus = MAIN_Y12_BUF_STATUS_CONFIG;
        } else if ((ViewZoneInfo->MainY12IsExtMem > 0U) &&
                   (ViewZoneInfo->MainY12AllocType == ALLOC_EXTERNAL_CYCLIC)) {
            ViewZoneInfo->MainY12BufferStatus = MAIN_Y12_BUF_STATUS_CONFIG;
        } else {
            // DO NOTHING
        }
        HL_GetViewZoneInfoUnLock(i);
    }
}

static inline void HL_LiveviewVprocConfigImpl(const UINT8 WriteMode)
{
    UINT32 Rval = OK;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT16 i;
    UINT32 WorkSize, Mask;
    DSP_DRAM_CFG_s DramCfg = {0};

    HL_GetResourcePtr(&Resource);
    WorkSize = Resource->ProfWorkSize;

    for (i = 0; (i<Resource->MaxViewZoneNum); i++) {
        UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
        cmd_vproc_cfg_t *VprocCfg = HL_DefCtxCmdBufPtrVpc;
        CTX_VPROC_INFO_s VprocInfo = {0};
        CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

        if (0U == DSP_GetBit(Resource->ViewZoneActiveBit, i, 1U)) {
            continue;
        }

        HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
        if (ViewZoneInfo->Pipe == (UINT8)DSP_DRAM_PIPE_RAW_ONLY) {
            continue;
        }

        HL_GetVprocInfo(HL_MTX_OPT_ALL, i, &VprocInfo);
        if (VprocInfo.Status != DSP_VPROC_STATUS_IDLE2RUN) {
            continue;
        }

        /* Vproc Config */
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&VprocCfg, &CmdBufferAddr);
        Rval = HL_FillVideoProcCfg(VprocCfg, i);
        if (Rval != OK) {
            AmbaLL_LogUInt5("VprocCfg[%d] filling fail", i, 0U, 0U, 0U, 0U);
        } else {
            (void)AmbaHL_CmdVprocConfig(WriteMode, VprocCfg);
        }

        /* Re-issue Vproc-Config means external memory config reset */
        HL_InitExtPymdLndtBuf(Resource->MaxViewZoneNum);

        { //DramCfg
            UINT16 j, OctaveMode = 0U, HeirBufWidth = 0U, HeirBufHeight = 0U;
            UINT32 HierBufSize;

            DramCfg.MaxVprocNum = (UINT8)Resource->MaxViewZoneNum;
            DSP_SetBit(&DramCfg.VprocBitMask, i);

            DramCfg.MaxC2YWidth[i] = VprocCfg->W_pre_warp_luma_max;
            DramCfg.MaxC2YHeight[i] = VprocCfg->H_pre_warp_luma_max;
            DramCfg.VprocC2YFrmBufNum[i] = VprocCfg->c2y_int_fb_num;

            DramCfg.MaxW0[i] = VprocCfg->W0_max;
            DramCfg.MaxH0[i] = VprocCfg->H0_max;

            DramCfg.MaxMainWidth[i] = VprocCfg->W_main_max;
            DramCfg.MaxMainHeight[i] = VprocCfg->H_main_max;
            DramCfg.VprocMainFrmBufNum[i] = VprocCfg->main_fb_num;
            DramCfg.VprocMainMeFrmBufNum[i] = VprocCfg->main_me01_fb_num;

            DramCfg.MaxPrevAWidth[i] = VprocCfg->prev_a_w_max;
            DramCfg.MaxPrevAHeight[i] = VprocCfg->prev_a_h_max;
            DramCfg.VprocPrevAFrmBufNum[i] = VprocCfg->prev_a_fb_num;
            DramCfg.VprocPrevAMeFrmBufNum[i] = VprocCfg->prev_a_me_fb_num;

            DramCfg.MaxPrevBWidth[i] = VprocCfg->prev_b_w_max;
            DramCfg.MaxPrevBHeight[i] = VprocCfg->prev_b_h_max;
            DramCfg.VprocPrevBFrmBufNum[i] = VprocCfg->prev_b_fb_num;
            DramCfg.VprocPrevBMeFrmBufNum[i] = VprocCfg->prev_b_me_fb_num;

            DramCfg.MaxPrevCWidth[i] = VprocCfg->prev_c_w_max;
            DramCfg.MaxPrevCHeight[i] = VprocCfg->prev_c_h_max;
            DramCfg.VprocPrevCFrmBufNum[i] = VprocCfg->prev_c_fb_num;
            DramCfg.VprocPrevCMeFrmBufNum[i] = VprocCfg->prev_c_me_fb_num;

            DramCfg.MaxLndtWidth[i] = VprocCfg->prev_ln_w_max;
            DramCfg.MaxLndtHeight[i] = VprocCfg->prev_ln_h_max;
            DramCfg.VprocLndtFrmBufNum[i] = VprocCfg->ln_dec_fb_num;

            for (j=0U; j<AMBA_DSP_MAX_HIER_NUM; j++) {
                if (0U == DSP_GetU16Bit(ViewZoneInfo->Pyramid.HierBit, j, 1U)) {
                    DSP_SetU16Bit(&OctaveMode, (UINT32)j+3U); //B[3:15] is HierDisable bit
                }
            }
            (void)dsp_cal_hier_buffer_size(&VprocCfg->W_main_max, &VprocCfg->H_main_max,
                                           &VprocCfg->prev_hier_w_max, &VprocCfg->prev_hier_h_max,
                                           OctaveMode, &HierBufSize,
                                           &HeirBufWidth, &HeirBufHeight);
            DramCfg.MaxHierBufWidth[i] = HeirBufWidth;
            DramCfg.MaxHierBufHeight[i] = HeirBufHeight;
            DramCfg.VprocHierFrmBufNum[i] = VprocCfg->hier_fb_num;
            Mask = 0U;
            DSP_SetBit(&Mask, DSP_MEM_AREA_VPROC_BIT);
            (void)HL_CalcWorkingBuffer(Mask, &DramCfg, &WorkSize);
            DramCfg.VprocBitMask = 0U;
        }
        HL_RelCmdBuffer(CmdBufferId);
    }

    /* Update Dram usage */
    HL_GetResourceLock(&Resource);
    Resource->ProfWorkSize = WorkSize;
    HL_GetResourceUnLock();
}

static inline UINT32 HL_LvVprocSetupOnPpStrm(const UINT8 WriteMode,
                                             const CTX_RESOURCE_INFO_s *pResource)
{
    UINT32 Rval = OK, IsEfctStrmExist;
    UINT16 i;
    UINT8 IsEfctYuvStrm;

    IsEfctStrmExist = HL_GetEffectChannelEnable();
    if (IsEfctStrmExist > 0U) {
        UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
        cmd_vproc_multi_stream_pp_t *VprocStrmPp = HL_DefCtxCmdBufPtrVpcPp;

        for (i=0U; i<pResource->YuvStrmNum; i++) {
            IsEfctYuvStrm = (1U == HL_GetEffectEnableOnYuvStrm(i))? (UINT8)1U: (UINT8)0U;

            if (1U == IsEfctYuvStrm) {
                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&VprocStrmPp, &CmdBufferAddr);
                (void)HL_FillPpStrmCfg(i, VprocStrmPp, NULL);
                (void)AmbaHL_CmdVprocMultiStrmPpCmd(WriteMode, VprocStrmPp);
                HL_RelCmdBuffer(CmdBufferId);
            }
        }
    }

    return Rval;
}

#ifdef SUPPORT_DSP_EXT_PIN_BUF
static inline UINT32 HL_LvVprocSetupOnPinExtMemStatus(UINT16 ViewZoneId,
                                                      const CTX_VIEWZONE_INFO_s *pViewZoneInfo,
                                                      const CTX_VPROC_INFO_s *pVprocInfo)
{
    UINT32 Rval = OK;
    UINT16 PinIdx, YuvStrmIdx;
    UINT8 IsEfctYuvStrm;
    CTX_VIEWZONE_INFO_s *TmpViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    UINT8 BufCreator = 0U;

    for (PinIdx = 0U; PinIdx < DSP_VPROC_PIN_NUM; PinIdx++) {
        if (pVprocInfo->PinUsage[PinIdx] > 0U) {
            DSP_Bit2U16Idx(pVprocInfo->PinUsage[PinIdx], &YuvStrmIdx);
            HL_GetYuvStrmInfoPtr(YuvStrmIdx, &YuvStrmInfo);
            IsEfctYuvStrm = (1U == HL_GetEffectEnableOnYuvStrm(YuvStrmIdx))? (UINT8)1U: (UINT8)0U;

            if ((YuvStrmInfo->IsExtMem > 0U) &&
                (YuvStrmInfo->AllocType == ALLOC_EXTERNAL_DISTINCT) &&
                (YuvStrmInfo->YuvBufTbl[0U] != 0U)) {

                if (1U == IsEfctYuvStrm) {
                    /* FirstChan in Efct will allocate final output */
                    if (YuvStrmInfo->Layout.ChanCfg[0U].ViewZoneId == ViewZoneId) {
                        BufCreator = 1U;
                    } else {
                        BufCreator = 0U;
                    }
                } else {
                    BufCreator = 1U;
                }

                if ((BufCreator == 1U) &&
                    (YuvStrmInfo->BufferStatus == YUVSTRM_BUF_STATUS_CONFIG)) {

                    HL_GetViewZoneInfoLock(ViewZoneId, &TmpViewZoneInfo);
                    DSP_SetU8Bit(&TmpViewZoneInfo->PinIsExtMem, PinIdx);
                    HL_GetViewZoneInfoUnLock(ViewZoneId);
                }

                HL_GetViewZoneInfoLock(ViewZoneId, &TmpViewZoneInfo);
                DSP_SetU8Bit(&TmpViewZoneInfo->PinComposeExtMem, PinIdx);
                HL_GetViewZoneInfoUnLock(ViewZoneId);
            } else if ((YuvStrmInfo->IsExtMem > 0U) &&
                       (YuvStrmInfo->AllocType == ALLOC_EXTERNAL_CYCLIC) &&
                       (YuvStrmInfo->BufferStatus == YUVSTRM_BUF_STATUS_CONFIG)) {
                if (1U == IsEfctYuvStrm) {
                    /* FirstChan in Efct will allocate final output */
                    if (YuvStrmInfo->Layout.ChanCfg[0U].ViewZoneId == ViewZoneId) {
                        BufCreator = 1U;
                    } else {
                        BufCreator = 0U;
                    }
                } else {
                    BufCreator = 1U;
                }

                if ((BufCreator == 1U) &&
                    (YuvStrmInfo->BufferStatus == YUVSTRM_BUF_STATUS_CONFIG)) {
                    HL_GetViewZoneInfoLock(ViewZoneId, &TmpViewZoneInfo);
                    DSP_SetU8Bit(&TmpViewZoneInfo->PinIsExtMem, PinIdx);
                    HL_GetViewZoneInfoUnLock(ViewZoneId);
                }

                HL_GetViewZoneInfoLock(ViewZoneId, &TmpViewZoneInfo);
                DSP_SetU8Bit(&TmpViewZoneInfo->PinComposeExtMem, PinIdx);
                HL_GetViewZoneInfoUnLock(ViewZoneId);
            } else if ((pViewZoneInfo->PymdAllocType == ALLOC_INTERNAL) &&
                       (pViewZoneInfo->PymdBufferStatus == YUVSTRM_BUF_STATUS_CONFIG)) {
                //TBD
            } else {
                //TBD
            }
        }
    }

    return Rval;
}

static inline void HL_LvVprocSetupOnPinExtMemCmdBufCreator(UINT8 IsEfctYuvStrm, const CTX_YUV_STRM_INFO_s *pYuvStrmInfo,
                                                           UINT16 ViewZoneId, UINT8 *pBufCreator)
{
    if (1U == IsEfctYuvStrm) {
        /* FirstChan in Efct will allocate final output */
        if (pYuvStrmInfo->Layout.ChanCfg[0U].ViewZoneId == ViewZoneId) {
            *pBufCreator = 1U;
        } else {
            *pBufCreator = 0U;
        }
    } else {
        *pBufCreator = 1U;
    }
}

static inline UINT32 HL_LvVprocSetupOnPinExtMemCmd(const UINT8 WriteMode,
                                                   UINT16 ViewZoneId,
                                                   const CTX_VIEWZONE_INFO_s *pViewZoneInfo,
                                                   const CTX_VPROC_INFO_s *pVprocInfo)
{
    UINT32 Rval = OK;
    UINT16 PinIdx, YuvStrmIdx, i;
    UINT8 IsEfctYuvStrm;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    UINT8 BufCreator = 0U;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_vproc_set_ext_mem_t *VprocExtMem = HL_DefCtxCmdBufPtrVpcExtMem;
    ULONG ULAddr = 0U;
    UINT32 *pBufTblAddr = NULL;
    UINT32 PhysAddr;

    for (PinIdx = 0U; PinIdx < DSP_VPROC_PIN_NUM; PinIdx++) {
        if (pVprocInfo->PinUsage[PinIdx] > 0U) {
            DSP_Bit2U16Idx(pVprocInfo->PinUsage[PinIdx], &YuvStrmIdx);
            HL_GetYuvStrmInfoPtr(YuvStrmIdx, &YuvStrmInfo);
            IsEfctYuvStrm = (1U == HL_GetEffectEnableOnYuvStrm(YuvStrmIdx))? (UINT8)1U: (UINT8)0U;

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VprocExtMem, &CmdBufferAddr);
            if ((YuvStrmInfo->IsExtMem > 0U) &&
                (YuvStrmInfo->AllocType == ALLOC_EXTERNAL_DISTINCT) &&
                (YuvStrmInfo->YuvBufTbl[0U] != 0U)) {

                HL_LvVprocSetupOnPinExtMemCmdBufCreator(IsEfctYuvStrm, YuvStrmInfo, ViewZoneId, &BufCreator);

                if ((BufCreator == 1U) &&
                    (YuvStrmInfo->BufferStatus == YUVSTRM_BUF_STATUS_CONFIG)) {
                    VprocExtMem->vin_id_or_chan_id = (UINT8)ViewZoneId;
                    VprocExtMem->memory_type = (UINT8)HL_VprocPinVprocMemTypePoolMap[PinIdx];
                    VprocExtMem->allocation_type = VIN_EXT_MEM_ALLOC_TYPE_DISTINCT;
                    VprocExtMem->allocation_mode = VIN_EXT_MEM_MODE_NEW;
                    VprocExtMem->overflow_cntl = VIN_EXT_MEM_CTRL_CIRCULAR;
                    VprocExtMem->num_frm_buf = YuvStrmInfo->BufNum;
                    VprocExtMem->buf_pitch = ALIGN_NUM16(YuvStrmInfo->YuvBuf.Pitch, (UINT16)DSP_BUF_ALIGNMENT);
                    VprocExtMem->buf_width = YuvStrmInfo->YuvBuf.Window.Width;
                    VprocExtMem->buf_height = YuvStrmInfo->YuvBuf.Window.Height;

                    HL_GetPointerToDspExtYuvStrmBufArray(YuvStrmIdx, &pBufTblAddr);
                    for (i = 0; i<YuvStrmInfo->BufNum; i++) {
                        (void)dsp_osal_virt2cli(YuvStrmInfo->YuvBufTbl[i], &PhysAddr);
                        pBufTblAddr[i] = PhysAddr;
                    }
                    dsp_osal_typecast(&ULAddr, &pBufTblAddr);
                    (void)dsp_osal_virt2cli(ULAddr, &VprocExtMem->buf_addr);
                    VprocExtMem->img_width = YuvStrmInfo->YuvBuf.Window.Width;
                    VprocExtMem->img_height = YuvStrmInfo->YuvBuf.Window.Height;
                    VprocExtMem->chroma_format = (YuvStrmInfo->YuvBuf.DataFmt == AMBA_DSP_YUV420)? DSP_YUV_420: DSP_YUV_422;
                    VprocExtMem->luma_img_ofs_x = 0U;
                    VprocExtMem->luma_img_ofs_y = 0U;
                    VprocExtMem->chroma_img_ofs_x = 0U;
                    VprocExtMem->chroma_img_ofs_y = 0U;
                    (void)AmbaHL_CmdVprocSetExtMem(WriteMode, VprocExtMem);
                    HL_GetYuvStrmInfoLock(YuvStrmIdx, &YuvStrmInfo);
                    YuvStrmInfo->BufferStatus = YUVSTRM_BUF_STATUS_ALLOCATED;
                    HL_GetYuvStrmInfoUnLock(YuvStrmIdx);
                }
            } else if ((YuvStrmInfo->IsExtMem > 0U) &&
                       (YuvStrmInfo->AllocType == ALLOC_EXTERNAL_CYCLIC) &&
                       (YuvStrmInfo->BufferStatus == YUVSTRM_BUF_STATUS_CONFIG)) {

                HL_LvVprocSetupOnPinExtMemCmdBufCreator(IsEfctYuvStrm, YuvStrmInfo, ViewZoneId, &BufCreator);

                if ((BufCreator == 1U) &&
                    (YuvStrmInfo->BufferStatus == YUVSTRM_BUF_STATUS_CONFIG)) {
                    VprocExtMem->vin_id_or_chan_id = (UINT8)ViewZoneId;
                    VprocExtMem->memory_type = (UINT8)HL_VprocPinVprocMemTypePoolMap[PinIdx];
                    VprocExtMem->allocation_type = VIN_EXT_MEM_ALLOC_TYPE_CONTIGUOUS;
                    VprocExtMem->allocation_mode = VIN_EXT_MEM_MODE_NEW;
                    VprocExtMem->overflow_cntl = VIN_EXT_MEM_CTRL_CIRCULAR;
                    VprocExtMem->num_frm_buf = YuvStrmInfo->BufNum;
                    if (VprocExtMem->num_frm_buf > EXT_MEM_VPROC_MAX_NUM) {
                        AmbaLL_LogUInt5("[%d]Only %d ExtMem supported", __LINE__, EXT_MEM_VPROC_MAX_NUM, 0U, 0U, 0U);
                    }
                    VprocExtMem->buf_pitch = ALIGN_NUM16(YuvStrmInfo->YuvBuf.Pitch, (UINT16)DSP_BUF_ALIGNMENT);
                    VprocExtMem->buf_width = YuvStrmInfo->YuvBuf.Window.Width;
                    VprocExtMem->buf_height = YuvStrmInfo->YuvBuf.Window.Height;
                    (void)dsp_osal_virt2cli(YuvStrmInfo->YuvBuf.BaseAddrY, &VprocExtMem->buf_addr);
                    VprocExtMem->img_width = YuvStrmInfo->YuvBuf.Window.Width;
                    VprocExtMem->img_height = YuvStrmInfo->YuvBuf.Window.Height;
                    VprocExtMem->chroma_format = (YuvStrmInfo->YuvBuf.DataFmt == AMBA_DSP_YUV420)? DSP_YUV_420: DSP_YUV_422;
                    VprocExtMem->luma_img_ofs_x = 0U;
                    VprocExtMem->luma_img_ofs_y = 0U;
                    VprocExtMem->chroma_img_ofs_x = 0U;
                    VprocExtMem->chroma_img_ofs_y = 0U;
                    (void)AmbaHL_CmdVprocSetExtMem(WriteMode, VprocExtMem);

                    HL_GetYuvStrmInfoLock(YuvStrmIdx, &YuvStrmInfo);
                    YuvStrmInfo->BufferStatus = YUVSTRM_BUF_STATUS_ALLOCATED;
                    HL_GetYuvStrmInfoUnLock(YuvStrmIdx);
                }
            } else if ((pViewZoneInfo->PymdAllocType == ALLOC_INTERNAL) &&
                       (pViewZoneInfo->PymdBufferStatus == YUVSTRM_BUF_STATUS_CONFIG)) {
                HL_GetYuvStrmInfoLock(YuvStrmIdx, &YuvStrmInfo);
                YuvStrmInfo->BufferStatus = YUVSTRM_BUF_STATUS_ALLOCATED;
                HL_GetYuvStrmInfoUnLock(YuvStrmIdx);
            } else {
                //TBD
            }
            HL_RelCmdBuffer(CmdBufferId);
        }
    }

    return Rval;
}
#endif

static inline UINT32 HL_LvVprocSetupOnPymdExtMem(const UINT8 WriteMode,
                                                 const UINT16 ViewZoneId,
                                                 const CTX_VIEWZONE_INFO_s *pViewZoneInfo)
{
    UINT32 Rval = OK, PhysAddr;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    CTX_VIEWZONE_INFO_s *TmpViewZoneInfo = HL_CtxViewZoneInfoPtr;
    cmd_vproc_set_ext_mem_t *VprocExtMem = HL_DefCtxCmdBufPtrVpcExtMem;
    UINT16 i = 0U;
    //const void *pVoid = NULL;
    UINT16 ExtHierMemType;
    UINT16 ExtChromaFmt;
    ULONG ULAddr = 0U;
    UINT32 *pBufTblAddr = NULL;

    if (DSP_GetU16Bit(pViewZoneInfo->Pyramid.IsPolySqrt2, DSP_PYMD_Y12_IDX, DSP_PYMD_Y12_LEN) > 0U) {
        ExtHierMemType = VPROC_EXT_MEM_TYPE_HIER_Y12;
        ExtChromaFmt = DSP_YUV_MONO;
    } else {
        ExtHierMemType = VPROC_EXT_MEM_TYPE_HIER_0;
        if (pViewZoneInfo->PymdYuvBuf.DataFmt == AMBA_DSP_YUV420) {
            ExtChromaFmt = DSP_YUV_420;
        } else {
            ExtChromaFmt = DSP_YUV_422;
        }
    }

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&VprocExtMem, &CmdBufferAddr);
    if ((pViewZoneInfo->PymdIsExtMem > 0U) &&
        (pViewZoneInfo->PymdAllocType == ALLOC_EXTERNAL_DISTINCT) &&
        (pViewZoneInfo->Pyramid.HierBit > 0U) &&
        (pViewZoneInfo->PymdYuvBufTbl[0U] != 0U) &&
        (pViewZoneInfo->PymdBufferStatus == PYMD_BUF_STATUS_CONFIG)) {
        VprocExtMem->vin_id_or_chan_id = (UINT8)ViewZoneId;
        VprocExtMem->memory_type = (UINT8)ExtHierMemType;
        VprocExtMem->allocation_type = VIN_EXT_MEM_ALLOC_TYPE_DISTINCT;
        VprocExtMem->allocation_mode = VIN_EXT_MEM_MODE_NEW;
        VprocExtMem->overflow_cntl = VIN_EXT_MEM_CTRL_CIRCULAR;
        VprocExtMem->num_frm_buf = pViewZoneInfo->PymdBufNum;
        if (VprocExtMem->num_frm_buf > EXT_MEM_VPROC_MAX_NUM) {
            AmbaLL_LogUInt5("[%d]Only %d ExtMem supported", __LINE__, EXT_MEM_VPROC_MAX_NUM, 0U, 0U, 0U);
        }
        VprocExtMem->buf_pitch = ALIGN_NUM16(pViewZoneInfo->PymdYuvBuf.Pitch, (UINT16)DSP_BUF_ALIGNMENT);
        VprocExtMem->buf_width = pViewZoneInfo->PymdYuvBuf.Window.Width;
        VprocExtMem->buf_height = pViewZoneInfo->PymdYuvBuf.Window.Height;

        HL_GetPointerToDspExtPymdBufArray(ViewZoneId, &pBufTblAddr);
        for (i = 0; i<pViewZoneInfo->PymdBufNum; i++) {
            (void)dsp_osal_virt2cli(pViewZoneInfo->PymdYuvBufTbl[i], &PhysAddr);
            pBufTblAddr[i] = PhysAddr;
        }
        dsp_osal_typecast(&ULAddr, &pBufTblAddr);
        (void)dsp_osal_virt2cli(ULAddr, &VprocExtMem->buf_addr);
        //dsp_osal_typecast(&pVoid, &pBufTblAddr);
        //(void)dsp_osal_cache_clean(pVoid, sizeof(UINT32)*VprocExtMem->num_frm_buf);

        VprocExtMem->img_width = pViewZoneInfo->PymdYuvBuf.Window.Width;
        VprocExtMem->img_height = pViewZoneInfo->PymdYuvBuf.Window.Height;
        VprocExtMem->chroma_format = (UINT8)ExtChromaFmt;
        VprocExtMem->luma_img_ofs_x = 0U;
        VprocExtMem->luma_img_ofs_y = 0U;
        VprocExtMem->chroma_img_ofs_x = 0U;
        VprocExtMem->chroma_img_ofs_y = 0U;

        (void)AmbaHL_CmdVprocSetExtMem(WriteMode, VprocExtMem);

        HL_GetViewZoneInfoLock(ViewZoneId, &TmpViewZoneInfo);
        TmpViewZoneInfo->PymdBufferStatus = PYMD_BUF_STATUS_ALLOCATED;
        HL_GetViewZoneInfoUnLock(ViewZoneId);
    } else if ((pViewZoneInfo->PymdIsExtMem > 0U) &&
               (pViewZoneInfo->PymdAllocType == ALLOC_EXTERNAL_CYCLIC) &&
               (pViewZoneInfo->Pyramid.HierBit > 0U) &&
               (pViewZoneInfo->PymdBufferStatus == PYMD_BUF_STATUS_CONFIG)) {
        VprocExtMem->vin_id_or_chan_id = (UINT8)ViewZoneId;
        VprocExtMem->memory_type = (UINT8)ExtHierMemType;
        VprocExtMem->allocation_type = VIN_EXT_MEM_ALLOC_TYPE_CONTIGUOUS;
        VprocExtMem->allocation_mode = VIN_EXT_MEM_MODE_NEW;
        VprocExtMem->overflow_cntl = VIN_EXT_MEM_CTRL_CIRCULAR;
        VprocExtMem->num_frm_buf = pViewZoneInfo->PymdBufNum;
        if (VprocExtMem->num_frm_buf > EXT_MEM_VPROC_MAX_NUM) {
            AmbaLL_LogUInt5("[%d]Only %d ExtMem supported", __LINE__, EXT_MEM_VPROC_MAX_NUM, 0U, 0U, 0U);
        }
        VprocExtMem->buf_pitch = ALIGN_NUM16(pViewZoneInfo->PymdYuvBuf.Pitch, (UINT16)DSP_BUF_ALIGNMENT);
        VprocExtMem->buf_width = pViewZoneInfo->PymdYuvBuf.Window.Width;
        VprocExtMem->buf_height = pViewZoneInfo->PymdYuvBuf.Window.Height;
        (void)dsp_osal_virt2cli(pViewZoneInfo->PymdYuvBuf.BaseAddrY, &VprocExtMem->buf_addr);
        VprocExtMem->img_width = pViewZoneInfo->PymdYuvBuf.Window.Width;
        VprocExtMem->img_height = pViewZoneInfo->PymdYuvBuf.Window.Height;
        VprocExtMem->chroma_format = (UINT8)ExtChromaFmt;
        VprocExtMem->luma_img_ofs_x = 0U;
        VprocExtMem->luma_img_ofs_y = 0U;
        VprocExtMem->chroma_img_ofs_x = 0U;
        VprocExtMem->chroma_img_ofs_y = 0U;
        (void)AmbaHL_CmdVprocSetExtMem(WriteMode, VprocExtMem);

        HL_GetViewZoneInfoLock(ViewZoneId, &TmpViewZoneInfo);
        TmpViewZoneInfo->PymdBufferStatus = PYMD_BUF_STATUS_ALLOCATED;
        HL_GetViewZoneInfoUnLock(ViewZoneId);
    } else if ((pViewZoneInfo->PymdAllocType == ALLOC_INTERNAL) &&
               (pViewZoneInfo->PymdBufferStatus == PYMD_BUF_STATUS_CONFIG)) {
        HL_GetViewZoneInfoLock(ViewZoneId, &TmpViewZoneInfo);
        TmpViewZoneInfo->PymdBufferStatus = PYMD_BUF_STATUS_ALLOCATED;
        HL_GetViewZoneInfoUnLock(ViewZoneId);
    } else {
        //TBD
    }
    HL_RelCmdBuffer(CmdBufferId);

    return Rval;
}

static inline UINT32 HL_LvVprocSetupOnLndtExtMem(const UINT8 WriteMode,
                                                 const UINT16 ViewZoneId,
                                                 const CTX_VIEWZONE_INFO_s *pViewZoneInfo)
{
    UINT32 Rval = OK, PhysAddr;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    CTX_VIEWZONE_INFO_s *TmpViewZoneInfo = HL_CtxViewZoneInfoPtr;
    cmd_vproc_set_ext_mem_t *VprocExtMem = HL_DefCtxCmdBufPtrVpcExtMem;
    UINT16 i = 0U;
    const void *pVoid = NULL;
    ULONG ULAddr = 0U;
    UINT32 *pBufTblAddr = NULL;

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&VprocExtMem, &CmdBufferAddr);
    if ((pViewZoneInfo->LndtIsExtMem > 0U) &&
        (pViewZoneInfo->LndtAllocType == ALLOC_EXTERNAL_DISTINCT) &&
        (pViewZoneInfo->LndtYuvBufTbl[0U] != 0U) &&
        (pViewZoneInfo->LndtBufferStatus == LNDT_BUF_STATUS_CONFIG)) {
        VprocExtMem->vin_id_or_chan_id = (UINT8)ViewZoneId;
        VprocExtMem->memory_type = VPROC_EXT_MEM_TYPE_LN_DEC;
        VprocExtMem->allocation_type = VIN_EXT_MEM_ALLOC_TYPE_DISTINCT;
        VprocExtMem->allocation_mode = VIN_EXT_MEM_MODE_NEW;
        VprocExtMem->overflow_cntl = VIN_EXT_MEM_CTRL_CIRCULAR;
        VprocExtMem->num_frm_buf = pViewZoneInfo->LndtBufNum;
        if (VprocExtMem->num_frm_buf > EXT_MEM_VPROC_MAX_NUM) {
            AmbaLL_LogUInt5("[%d]Only %d ExtMem supported", __LINE__, EXT_MEM_VPROC_MAX_NUM, 0U, 0U, 0U);
        }
        VprocExtMem->buf_pitch = ALIGN_NUM16(pViewZoneInfo->LndtYuvBuf.Pitch, (UINT16)DSP_BUF_ALIGNMENT);
        VprocExtMem->buf_width = pViewZoneInfo->LndtYuvBuf.Window.Width;
        VprocExtMem->buf_height = pViewZoneInfo->LndtYuvBuf.Window.Height;

        HL_GetPointerToDspExtLndtBufArray(ViewZoneId, &pBufTblAddr);
        for (i = 0; i<pViewZoneInfo->LndtBufNum; i++) {
            (void)dsp_osal_virt2cli(pViewZoneInfo->LndtYuvBufTbl[i], &PhysAddr);
            pBufTblAddr[i] = PhysAddr;
        }
        dsp_osal_typecast(&ULAddr, &pBufTblAddr);
        dsp_osal_typecast(&pVoid, &pBufTblAddr);
        (void)dsp_osal_virt2cli(ULAddr, &VprocExtMem->buf_addr);
        (void)dsp_osal_cache_clean(pVoid, (UINT32)(sizeof(UINT32)*VprocExtMem->num_frm_buf));

        VprocExtMem->img_width = pViewZoneInfo->LndtYuvBuf.Window.Width;
        VprocExtMem->img_height = pViewZoneInfo->LndtYuvBuf.Window.Height;
        VprocExtMem->chroma_format = (pViewZoneInfo->LndtYuvBuf.DataFmt == AMBA_DSP_YUV420)? DSP_YUV_420: DSP_YUV_422;
        VprocExtMem->luma_img_ofs_x = 0U;
        VprocExtMem->luma_img_ofs_y = 0U;
        VprocExtMem->chroma_img_ofs_x = 0U;
        VprocExtMem->chroma_img_ofs_y = 0U;

        (void)AmbaHL_CmdVprocSetExtMem(WriteMode, VprocExtMem);

        HL_GetViewZoneInfoLock(ViewZoneId, &TmpViewZoneInfo);
        TmpViewZoneInfo->LndtBufferStatus = LNDT_BUF_STATUS_ALLOCATED;
        HL_GetViewZoneInfoUnLock(ViewZoneId);
    } else if ((pViewZoneInfo->LndtIsExtMem > 0U) &&
               (pViewZoneInfo->LndtAllocType == ALLOC_EXTERNAL_CYCLIC) &&
               (pViewZoneInfo->LndtBufferStatus == LNDT_BUF_STATUS_CONFIG)) {
        VprocExtMem->vin_id_or_chan_id = (UINT8)ViewZoneId;
        VprocExtMem->memory_type = VPROC_EXT_MEM_TYPE_LN_DEC;
        VprocExtMem->allocation_type = VIN_EXT_MEM_ALLOC_TYPE_CONTIGUOUS;
        VprocExtMem->allocation_mode = VIN_EXT_MEM_MODE_NEW;
        VprocExtMem->overflow_cntl = VIN_EXT_MEM_CTRL_CIRCULAR;
        VprocExtMem->num_frm_buf = pViewZoneInfo->LndtBufNum;
        if (VprocExtMem->num_frm_buf > EXT_MEM_VPROC_MAX_NUM) {
            AmbaLL_LogUInt5("[%d]Only %d ExtMem supported", __LINE__, EXT_MEM_VPROC_MAX_NUM, 0U, 0U, 0U);
        }
        VprocExtMem->buf_pitch = ALIGN_NUM16(pViewZoneInfo->LndtYuvBuf.Pitch, (UINT16)DSP_BUF_ALIGNMENT);
        VprocExtMem->buf_width = pViewZoneInfo->LndtYuvBuf.Window.Width;
        VprocExtMem->buf_height = pViewZoneInfo->LndtYuvBuf.Window.Height;
        (void)dsp_osal_virt2cli(pViewZoneInfo->LndtYuvBuf.BaseAddrY, &VprocExtMem->buf_addr);
        VprocExtMem->img_width = pViewZoneInfo->LndtYuvBuf.Window.Width;
        VprocExtMem->img_height = pViewZoneInfo->LndtYuvBuf.Window.Height;
        VprocExtMem->chroma_format = (pViewZoneInfo->LndtYuvBuf.DataFmt == AMBA_DSP_YUV420)? DSP_YUV_420: DSP_YUV_422;
        VprocExtMem->luma_img_ofs_x = 0U;
        VprocExtMem->luma_img_ofs_y = 0U;
        VprocExtMem->chroma_img_ofs_x = 0U;
        VprocExtMem->chroma_img_ofs_y = 0U;
        (void)AmbaHL_CmdVprocSetExtMem(WriteMode, VprocExtMem);

        HL_GetViewZoneInfoLock(ViewZoneId, &TmpViewZoneInfo);
        TmpViewZoneInfo->LndtBufferStatus = LNDT_BUF_STATUS_ALLOCATED;
        HL_GetViewZoneInfoUnLock(ViewZoneId);

    } else if ((pViewZoneInfo->LndtAllocType == ALLOC_INTERNAL) &&
               (pViewZoneInfo->LndtBufferStatus == LNDT_BUF_STATUS_CONFIG)) {
        HL_GetViewZoneInfoLock(ViewZoneId, &TmpViewZoneInfo);
        TmpViewZoneInfo->LndtBufferStatus = LNDT_BUF_STATUS_ALLOCATED;
        HL_GetViewZoneInfoUnLock(ViewZoneId);
    } else {
        //TBD
    }
    HL_RelCmdBuffer(CmdBufferId);

    return Rval;
}

static inline UINT32 HL_LvVprocSetupOnMainY12ExtMem(const UINT8 WriteMode,
                                                    const UINT16 ViewZoneId,
                                                    const CTX_VIEWZONE_INFO_s *pViewZoneInfo)
{
    UINT32 Rval = OK, PhysAddr;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    CTX_VIEWZONE_INFO_s *TmpViewZoneInfo = HL_CtxViewZoneInfoPtr;
    cmd_vproc_set_ext_mem_t *VprocExtMem = HL_DefCtxCmdBufPtrVpcExtMem;
    UINT16 i = 0U;
    const void *pVoid = NULL;
    ULONG ULAddr = 0U;
    UINT32 *pBufTblAddr = NULL;

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&VprocExtMem, &CmdBufferAddr);
    if ((pViewZoneInfo->MainY12IsExtMem > 0U) &&
        (pViewZoneInfo->MainY12AllocType == ALLOC_EXTERNAL_DISTINCT) &&
        (pViewZoneInfo->LndtYuvBufTbl[0U] != 0U) &&
        (pViewZoneInfo->MainY12BufferStatus == MAIN_Y12_BUF_STATUS_CONFIG)) {
        VprocExtMem->vin_id_or_chan_id = (UINT8)ViewZoneId;
        VprocExtMem->memory_type = VPROC_EXT_MEM_TYPE_C2Y_Y12;
        VprocExtMem->allocation_type = VIN_EXT_MEM_ALLOC_TYPE_DISTINCT;
        VprocExtMem->allocation_mode = VIN_EXT_MEM_MODE_NEW;
        VprocExtMem->overflow_cntl = VIN_EXT_MEM_CTRL_CIRCULAR;
        VprocExtMem->num_frm_buf = pViewZoneInfo->MainY12BufNum;
        if (VprocExtMem->num_frm_buf > EXT_MEM_VPROC_MAX_NUM) {
            AmbaLL_LogUInt5("[%d]Only %d ExtMem supported", __LINE__, EXT_MEM_VPROC_MAX_NUM, 0U, 0U, 0U);
        }
        VprocExtMem->buf_pitch = ALIGN_NUM16(pViewZoneInfo->MainY12Buf.Pitch, (UINT16)DSP_BUF_ALIGNMENT);
        VprocExtMem->buf_width = pViewZoneInfo->MainY12Buf.Window.Width;
        VprocExtMem->buf_height = pViewZoneInfo->MainY12Buf.Window.Height;

        HL_GetPointerToDspExtMainY12BufArray(ViewZoneId, &pBufTblAddr);
        for (i = 0; i<pViewZoneInfo->MainY12BufNum; i++) {
            (void)dsp_osal_virt2cli(pViewZoneInfo->MainY12BufTbl[i], &PhysAddr);
            pBufTblAddr[i] = PhysAddr;
        }
        dsp_osal_typecast(&ULAddr, &pBufTblAddr);
        dsp_osal_typecast(&pVoid, &pBufTblAddr);
        (void)dsp_osal_virt2cli(ULAddr, &VprocExtMem->buf_addr);
        (void)dsp_osal_cache_clean(pVoid, (UINT32)(sizeof(UINT32)*VprocExtMem->num_frm_buf));

        VprocExtMem->img_width = pViewZoneInfo->MainY12Buf.Window.Width;
        VprocExtMem->img_height = pViewZoneInfo->MainY12Buf.Window.Height;
        VprocExtMem->chroma_format = DSP_YUV_MONO;
        VprocExtMem->luma_img_ofs_x = 0U;
        VprocExtMem->luma_img_ofs_y = 0U;
        VprocExtMem->chroma_img_ofs_x = 0U;
        VprocExtMem->chroma_img_ofs_y = 0U;

        (void)AmbaHL_CmdVprocSetExtMem(WriteMode, VprocExtMem);

        HL_GetViewZoneInfoLock(ViewZoneId, &TmpViewZoneInfo);
        TmpViewZoneInfo->MainY12BufferStatus = MAIN_Y12_BUF_STATUS_ALLOCATED;
        HL_GetViewZoneInfoUnLock(ViewZoneId);
    } else if ((pViewZoneInfo->MainY12IsExtMem > 0U) &&
               (pViewZoneInfo->MainY12AllocType == ALLOC_EXTERNAL_CYCLIC) &&
               (pViewZoneInfo->MainY12BufferStatus == MAIN_Y12_BUF_STATUS_CONFIG)) {
        VprocExtMem->vin_id_or_chan_id = (UINT8)ViewZoneId;
        VprocExtMem->memory_type = VPROC_EXT_MEM_TYPE_C2Y_Y12;
        VprocExtMem->allocation_type = VIN_EXT_MEM_ALLOC_TYPE_CONTIGUOUS;
        VprocExtMem->allocation_mode = VIN_EXT_MEM_MODE_NEW;
        VprocExtMem->overflow_cntl = VIN_EXT_MEM_CTRL_CIRCULAR;
        VprocExtMem->num_frm_buf = pViewZoneInfo->MainY12BufNum;
        if (VprocExtMem->num_frm_buf > EXT_MEM_VPROC_MAX_NUM) {
            AmbaLL_LogUInt5("[%d]Only %d ExtMem supported", __LINE__, EXT_MEM_VPROC_MAX_NUM, 0U, 0U, 0U);
        }
        VprocExtMem->buf_pitch = ALIGN_NUM16(pViewZoneInfo->MainY12Buf.Pitch, (UINT16)DSP_BUF_ALIGNMENT);
        VprocExtMem->buf_width = pViewZoneInfo->MainY12Buf.Window.Width;
        VprocExtMem->buf_height = pViewZoneInfo->MainY12Buf.Window.Height;
        (void)dsp_osal_virt2cli(pViewZoneInfo->MainY12Buf.BaseAddrY, &VprocExtMem->buf_addr);
        VprocExtMem->img_width = pViewZoneInfo->MainY12Buf.Window.Width;
        VprocExtMem->img_height = pViewZoneInfo->MainY12Buf.Window.Height;
        VprocExtMem->chroma_format = DSP_YUV_MONO;
        VprocExtMem->luma_img_ofs_x = 0U;
        VprocExtMem->luma_img_ofs_y = 0U;
        VprocExtMem->chroma_img_ofs_x = 0U;
        VprocExtMem->chroma_img_ofs_y = 0U;
        (void)AmbaHL_CmdVprocSetExtMem(WriteMode, VprocExtMem);

        HL_GetViewZoneInfoLock(ViewZoneId, &TmpViewZoneInfo);
        TmpViewZoneInfo->MainY12BufferStatus = MAIN_Y12_BUF_STATUS_ALLOCATED;
        HL_GetViewZoneInfoUnLock(ViewZoneId);

    } else if ((pViewZoneInfo->MainY12AllocType == ALLOC_INTERNAL) &&
               (pViewZoneInfo->MainY12BufferStatus == MAIN_Y12_BUF_STATUS_CONFIG)) {
        HL_GetViewZoneInfoLock(ViewZoneId, &TmpViewZoneInfo);
        TmpViewZoneInfo->MainY12BufferStatus = MAIN_Y12_BUF_STATUS_ALLOCATED;
        HL_GetViewZoneInfoUnLock(ViewZoneId);
    } else {
        //TBD
    }
    HL_RelCmdBuffer(CmdBufferId);

    return Rval;
}

static inline UINT32 HL_LvVprocSetupOnPymd(const UINT8 WriteMode,
                                           UINT16 ViewZoneId)
{
    UINT32 Rval;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_vproc_img_pyramid_setup_t *VprocPrmdSetup = HL_DefCtxCmdBufPtrVpcPymd;

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&VprocPrmdSetup, &CmdBufferAddr);
    Rval = HL_FillVideoProcPyramidSetup(VprocPrmdSetup, ViewZoneId);
    if (Rval != OK) {
        AmbaLL_LogUInt5("PyramidSetup[%d] filling fail", (UINT32)ViewZoneId, 0U, 0U, 0U, 0U);
    } else {
        (void)AmbaHL_CmdVprocImgPrmdSetup(WriteMode, VprocPrmdSetup);
    }
    HL_RelCmdBuffer(CmdBufferId);

    return Rval;
}

static inline UINT32 HL_LvVprocSetupOnLndt(const UINT8 WriteMode,
                                           UINT16 ViewZoneId)
{
    UINT32 Rval;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_vproc_lane_det_setup_t *LndtSetup = HL_DefCtxCmdBufPtrVpcLndt;

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&LndtSetup, &CmdBufferAddr);
    Rval = HL_FillVideoProcLnDtSetup(LndtSetup, ViewZoneId);
    if (Rval != OK) {
        AmbaLL_LogUInt5("LndtSetup[%d] filling fail", ViewZoneId, 0U, 0U, 0U, 0U);
    } else {
        (void)AmbaHL_CmdVprocLaneDetSetup(WriteMode, LndtSetup);
    }
    HL_RelCmdBuffer(CmdBufferId);

    return Rval;
}

static inline UINT32 HL_LvVprocSetupOnPreview(const UINT8 WriteMode,
                                              UINT16 ViewZoneId)
{
    UINT8 CmdByPass = 0U;
    UINT16 j;
    UINT32 Rval;

    for (j = 0U; j < DSP_VPROC_PREV_D; j++) {
        UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
        cmd_vproc_prev_setup_t *PrevSetup = HL_DefCtxCmdBufPtrVpcPrev;

        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&PrevSetup, &CmdBufferAddr);
        Rval = HL_FillVideoPreviewSetup(PrevSetup, ViewZoneId, (UINT8)j/*PrevId*/, &CmdByPass, NULL);
        if (Rval != OK) {
            AmbaLL_LogUInt5("PrevSetup[%d][%d] filling fail", ViewZoneId, j, 0U, 0U, 0U);
            HL_RelCmdBuffer(CmdBufferId);
            break;
        } else {
            if (CmdByPass == 0U) {
                (void)AmbaHL_CmdVprocPrevSetup(WriteMode, PrevSetup);
            }
        }
        HL_RelCmdBuffer(CmdBufferId);
    }

    return Rval;
}

static inline UINT32 HL_LvVprocSetupOnDecimation(const UINT8 WriteMode,
                                                 const UINT16 ViewZoneId)
{
    UINT32 Rval = OK;
#ifdef SUPPORT_DSP_PINOUT_DECI
    UINT16 YuvStrmIdx;

    /* In CV2FS, PinOut decimation is implement in HL as following rule
     * MainStrm  : Decimate in down-stream, like encoder will recv decimated pic, dram traffic will be the same
     * PrevA/B/C : Decimate in vproc DramOut, so dram traffic is saved
     * When vproc-strm with effect, decimated will not take effect.
     * CV2SDK shall share same logic
     */
    for (YuvStrmIdx = 0U; YuvStrmIdx < AMBA_DSP_MAX_YUVSTRM_NUM; YuvStrmIdx++) {
        UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
        CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
        cmd_vproc_pin_out_deci_t *VprocDeci = HL_DefCtxCmdBufPtrVpcDeci;

        HL_GetYuvStrmInfoPtr(YuvStrmIdx, &YuvStrmInfo);
        if (YuvStrmInfo->DestDeciRate == 0U) {
            continue;
        }
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&VprocDeci, &CmdBufferAddr);
        Rval = HL_FillVideoProcDecimation(ViewZoneId, YuvStrmIdx, VprocDeci);
        if (Rval != OK) {
            AmbaLL_LogUInt5("VideoProcDecimation [%d][%d] filling fail [0x%X]", ViewZoneId, YuvStrmIdx, Rval, 0U, 0U);
        } else {
            Rval = AmbaHL_CmdVprocPinOutDecimation(WriteMode, VprocDeci);
            if (Rval != OK) {
                AmbaLL_LogUInt5("AmbaHL_CmdVprocPinOutDecimation [%d][%d] [0x%X]", ViewZoneId, YuvStrmIdx, Rval, 0U, 0U);
            }
        }
        HL_RelCmdBuffer(CmdBufferId);
    }
#else
(void)WriteMode;
(void)ViewZoneId;
#endif
    return Rval;
}

static inline UINT32 HL_LvVprocSetupOnGrouping(const UINT8 WriteMode)
{
    UINT32 Rval;
#ifdef SUPPORT_VPROC_GROUPING
    /* Vproc grouping */
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_vproc_set_vproc_grping *SetVprocGrping = HL_DefCtxCmdBufPtrVpcGrp;

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&SetVprocGrping, &CmdBufferAddr);
    Rval = HL_FillVideoProcGrouping(SetVprocGrping);
    if (Rval != OK) {
        AmbaLL_LogUInt5("HL_FillVideoProcGrouping filling fail", 0U, 0U, 0U, 0U, 0U);
    } else {
        (void)AmbaHL_CmdVprocSetVprocGrping(WriteMode, SetVprocGrping);
    }
    HL_RelCmdBuffer(CmdBufferId);
#else
    /* If SSP uses cmd_vproc_multi_chan_proc_order_t for grouping */
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_vproc_multi_chan_proc_order_t *ChanProcOrder = HL_DefCtxCmdBufPtrVpcChOrd;

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&ChanProcOrder, &CmdBufferAddr);
    Rval = HL_FillVideoProcChanProcOrder(ChanProcOrder);
    if (Rval != OK) {
        AmbaLL_LogUInt5("HL_FillVideoProcChanProcOrder filling fail", 0U, 0U, 0U, 0U, 0U);
    } else {
        (void)AmbaHL_CmdVprocMultiChanProcOrder(WriteMode, ChanProcOrder);
    }
    HL_RelCmdBuffer(CmdBufferId);
#endif

    return Rval;
}

static inline UINT32 HL_LvVprocSetupOnTestFrm(const UINT8 WriteMode,
                                              const CTX_RESOURCE_INFO_s *pResource)
{
    static const UINT16 TestStageIdxMax[DSP_TEST_STAGE_IDSP_1 + 1U] = {
        [DSP_TEST_STAGE_IDSP_0] = DSP_MAX_TEST_INP_STAGE0,
        [DSP_TEST_STAGE_IDSP_1] = DSP_MAX_TEST_INP_STAGE1,
    };
    UINT32 Rval = OK;
    UINT16 i, j;
    UINT32 PhysAddr;
    ULONG ULAddr = 0U;
    UINT32 *pBufTblAddr = NULL;

    for (i = 0U; i <= DSP_TEST_STAGE_IDSP_1 ;i++) {
        if (pResource->TestFrmNumOnStage[i] > 0U) {
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_vproc_set_testframe_t *VprocTestFrm = HL_DefCtxCmdBufPtrVpcTestFrm;

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VprocTestFrm, &CmdBufferAddr);
            if (i == DSP_TEST_STAGE_IDSP_0) {
                VprocTestFrm->type = DSP_VPROC_C2Y_TESTFRAME_TYPE;
            } else {
                VprocTestFrm->type = DSP_VPROC_Y2Y_TESTFRAME_TYPE;
            }

            VprocTestFrm->is_sending_jobs = (UINT8)0U;
            VprocTestFrm->num_of_cases = (UINT8)pResource->TestFrmNumOnStage[i];
            VprocTestFrm->issue_tf_interval = pResource->TestFrmInterval[i]/10U;
            for (j = 0U; j < pResource->TestFrmNumOnStage[i]; j++) {
                (void)dsp_osal_virt2cli(pResource->TestCfgOnStage[i][j], &VprocTestFrm->tf_cfg_daddr[j]);
            }

//            for (j = 0U; j < pResource->TestFrmNumOnStage[i]; j++) {
//                VprocTestFrm->tc_order[j] = pResource->TestFrmOrd[i][j];
//            }

            HL_GetPointerToDspTestFrmInpTbl(i, &pBufTblAddr);
//            AmbaLL_LogUInt5("HL_LvVprocSetupOnTestFrm i:%u pBufTblAddr:0x%x", i, (UINT32)pBufTblAddr, 0U, 0U, 0U);
            if (pBufTblAddr != NULL) {
                for (j = 0; j < TestStageIdxMax[i]; j++) {
                    (void)dsp_osal_virt2cli(pResource->TestFrmInpTable[i][j], &PhysAddr);
                    pBufTblAddr[j] = PhysAddr;
                }
            }
            dsp_osal_typecast(&ULAddr, &pBufTblAddr);
            (void)dsp_osal_virt2cli(ULAddr, &VprocTestFrm->tf_in_tbl_addr);
            Rval = AmbaHL_CmdVprocSetTestFrame(WriteMode, VprocTestFrm);
            HL_RelCmdBuffer(CmdBufferId);
        }
    }

    return Rval;
}

static inline UINT32 HL_LvVprocSetupOnTestFrmExec(const UINT8 WriteMode,
                                                  const CTX_RESOURCE_INFO_s *pResource)
{
    static const UINT16 TestStageIdxMax[DSP_TEST_STAGE_IDSP_1 + 1U] = {
        [DSP_TEST_STAGE_IDSP_0] = DSP_MAX_TEST_INP_STAGE0,
        [DSP_TEST_STAGE_IDSP_1] = DSP_MAX_TEST_INP_STAGE1,
    };
    UINT32 Rval = OK;
    UINT16 i, j;
    UINT32 PhysAddr;
    ULONG ULAddr = 0U;
    UINT32 *pBufTblAddr = NULL;

    for (i = 0U; i <= DSP_TEST_STAGE_IDSP_1 ;i++) {
        if ((pResource->TestFrmNumOnStage[i] > 0U) && (pResource->TestFrmEnableMask[i] > 0U)) {
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_vproc_set_testframe_t *VprocTestFrm = HL_DefCtxCmdBufPtrVpcTestFrm;

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VprocTestFrm, &CmdBufferAddr);
            if (i == DSP_TEST_STAGE_IDSP_0) {
                VprocTestFrm->type = DSP_VPROC_C2Y_TESTFRAME_TYPE;
            } else {
                VprocTestFrm->type = DSP_VPROC_Y2Y_TESTFRAME_TYPE;
            }

            VprocTestFrm->num_of_cases = (UINT8)pResource->TestFrmNumOnStage[i];
            VprocTestFrm->is_sending_jobs = (UINT8)1U;
            VprocTestFrm->tf_repeat_mode = (UINT8)pResource->TestFrmRepeat[i];
            VprocTestFrm->issue_tf_interval = pResource->TestFrmInterval[i]/10U;
//            for (j = 0U; j < pResource->TestFrmNumOnStage[i]; j++) {
//                (void)dsp_osal_virt2cli(pResource->TestCfgOnStage[i][j], &VprocTestFrm->tf_cfg_daddr[j]);
//            }

            for (j = 0U; j < pResource->TestFrmNumOnStage[i]; j++) {
                VprocTestFrm->tc_order[j] = (UINT8)pResource->TestFrmOrder[i][j];
            }

            HL_GetPointerToDspTestFrmInpTbl(i, &pBufTblAddr);
//            AmbaLL_LogUInt5("HL_LvVprocSetupOnTestFrmExec i:%u pBufTblAddr:0x%x TestFrmEnableMask:%x",
//                    i, (UINT32)pBufTblAddr, pResource->TestFrmEnableMask[i], 0U, 0U);
            if (pBufTblAddr != NULL) {
                for (j = 0; j < TestStageIdxMax[i]; j++) {
                    (void)dsp_osal_virt2cli(pResource->TestFrmInpTable[i][j], &PhysAddr);
                    pBufTblAddr[j] = PhysAddr;
                }
            }
            dsp_osal_typecast(&ULAddr, &pBufTblAddr);
            (void)dsp_osal_virt2cli(ULAddr, &VprocTestFrm->tf_in_tbl_addr);
            Rval = AmbaHL_CmdVprocSetTestFrame(WriteMode, VprocTestFrm);
            HL_RelCmdBuffer(CmdBufferId);
        }
    }

    return Rval;
}

static inline void HL_LiveviewVprocSetupImpl2ndHalf(const UINT8 WriteMode, UINT32 *pRval, UINT16 *pGrpNum, const CTX_RESOURCE_INFO_s *pResource)
{
    if (*pRval == OK) {
        *pRval = HL_GetVprocGroupNum(pGrpNum);
    }

    if (*pRval == OK) {
        *pRval = HL_LvVprocSetupOnGrouping(WriteMode);
    }

    if (*pRval == OK) {
        *pRval = HL_LvVprocSetupOnTestFrm(WriteMode, pResource);
    }

    if (*pRval == OK) {
        *pRval = HL_LvVprocSetupOnTestFrmExec(WriteMode, pResource);
    }
}

static inline UINT32 HL_LiveviewVprocSetupImpl(const UINT8 WriteMode)
{
    UINT32 Rval = OK;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 i, GrpNum = 0U;

    HL_GetResourcePtr(&Resource);

    /* MultiStreamPp */
    (void)HL_LvVprocSetupOnPpStrm(WriteMode, Resource);

    for (i = 0; (i<Resource->MaxViewZoneNum); i++) {
        CTX_VPROC_INFO_s VprocInfo = {0};

        if (0U == DSP_GetBit(Resource->ViewZoneActiveBit, i, 1U)) {
            continue;
        }

        HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
        if (ViewZoneInfo->Pipe == (UINT8)DSP_DRAM_PIPE_RAW_ONLY) {
            continue;
        }

        HL_GetVprocInfo(HL_MTX_OPT_ALL, i, &VprocInfo);
        if (VprocInfo.Status != DSP_VPROC_STATUS_IDLE2RUN) {
            continue;
        }

        /* When Enable ExtMemDesc from ARM, SetExtMemCmd MUST be issued after VprocSetupCmd */
#ifdef SUPPORT_DSP_EXT_PIN_BUF
        Rval = HL_LvVprocSetupOnPinExtMemStatus(i, ViewZoneInfo, &VprocInfo);
#endif

#ifdef SUPPORT_DSP_EXT_PIN_BUF
        /* Vproc Start */
        if (Rval == OK) {
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_vproc_setup_t *VprocSetup = HL_DefCtxCmdBufPtrVpcSetup;

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VprocSetup, &CmdBufferAddr);
            Rval = HL_FillVideoProcSetup(VprocSetup, i);
            if (Rval != OK) {
                AmbaLL_LogUInt5("VprocSetup[%d] filling fail", i, 0U, 0U, 0U, 0U);
            } else {
                (void)AmbaHL_CmdVprocSetup(WriteMode, VprocSetup);
            }
            HL_RelCmdBuffer(CmdBufferId);
        }
#endif

#ifdef SUPPORT_DSP_EXT_PIN_BUF
        Rval = HL_LvVprocSetupOnPinExtMemCmd(WriteMode, i, ViewZoneInfo, &VprocInfo);
#endif

        /* ExtMem for Hier */
        if (Rval == OK) {
            Rval = HL_LvVprocSetupOnPymdExtMem(WriteMode, i, ViewZoneInfo);
        }

        /* ExtMem for LnDt */
        if (Rval == OK) {
            Rval = HL_LvVprocSetupOnLndtExtMem(WriteMode, i, ViewZoneInfo);
        }

        /* ExtMem for MainY12 */
        if (Rval == OK) {
            Rval = HL_LvVprocSetupOnMainY12ExtMem(WriteMode, i, ViewZoneInfo);
        }
#ifndef SUPPORT_DSP_EXT_PIN_BUF
        /* Vproc Start */
        if (Rval == OK) {
            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VprocSetup, &CmdBufferAddr);
            Rval = HL_FillVideoProcSetup(VprocSetup, i);
            if (Rval != OK) {
                AmbaLL_LogUInt5("VprocSetup[%d] filling fail", i, 0U, 0U, 0U, 0U);
            } else {
                (void)AmbaHL_CmdVprocSetup(WriteMode, VprocSetup);
                ActVprocNum++;
            }
            HL_RelCmdBuffer(CmdBufferId);
        }
#endif

        /* Pymd Setup */
        if (Rval == OK) {
            Rval = HL_LvVprocSetupOnPymd(WriteMode, i);
        }

        /* Lane Detect offset */
        if (Rval == OK) {
            Rval = HL_LvVprocSetupOnLndt(WriteMode, i);
        }

        /* PreviewSetup */
        if (Rval == OK) {
            Rval = HL_LvVprocSetupOnPreview(WriteMode, i);
        }

        /* Decimation */
        if (Rval == OK) {
            Rval = HL_LvVprocSetupOnDecimation(WriteMode, i);
        } else {
            break;
        }
    }

    HL_LiveviewVprocSetupImpl2ndHalf(WriteMode, &Rval, &GrpNum, Resource);

    return Rval;
}

static inline UINT32 HL_LvFillVinCmdOnVinCfg(const UINT8 WriteMode,
                                             UINT16 VinId)
{
    UINT32 Rval = OK;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    set_vin_config_t *VinCfg = HL_DefCtxCmdBufPtrVinCfg;

        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&VinCfg, &CmdBufferAddr);
        Rval = HL_FillVinConfig(VinCfg, VinId);
        if (Rval != OK) {
            AmbaLL_LogUInt5("VinConfig[%d] filling fail", VinId, 0U, 0U, 0U, 0U);
        } else {
            (void)AmbaHL_CmdSetVinConfig(WriteMode, VinCfg);
        }
        HL_RelCmdBuffer(CmdBufferId);

    return Rval;
}

static inline UINT32 HL_LvFillVinCmdOnPreVinStart(UINT16 VinId,
                                                  UINT8 *pVinStartPurpose,
                                                  cmd_vin_start_t *pVinStartCfg)
{
    UINT8 IsVirtVin = 0U;
    UINT32 Rval;
    UINT16 TDPhyVinId = DSP_VIRT_VIN_IDX_INVALID;

    (void)HL_IsVirtualVinIdx(VinId, &IsVirtVin);
    (void)HL_GetTimeDivisionVirtVinInfo(VinId, &TDPhyVinId);

    if (IsVirtVin == 1U) {
        if (TDPhyVinId == DSP_VIRT_VIN_IDX_INVALID) {
            *pVinStartPurpose = VIN_START_VIRT_VIN;
        } else {
            *pVinStartPurpose = VIN_START_VIRT_VIN_LIVEVIEW;
        }
    } else {
        *pVinStartPurpose = VIN_START_LIVEVIEW_ONLY;
    }
    Rval = HL_FillVinStartCfg(pVinStartCfg, VinId, *pVinStartPurpose, VIN_START_CFG_ISOCFG, NULL/*DataCapCfg*/);
    if (Rval != OK) {
        AmbaLL_LogUInt5("VinStartCfg[%d] Opt[0x%X] filling fail", VinId, VIN_START_CFG_ISOCFG, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_FillVinExtMemInLv(cmd_vin_set_ext_mem_t *pVinExtMem,
                                          UINT16 VinId,
                                          UINT8 IsAuxData,
                                          const CTX_RESOURCE_INFO_s *pResource)
{
    UINT32 Rval = OK;
    ULONG ULAddr;
    UINT32 *pBufTblAddr = NULL;
    UINT32 PhysAddr;
    UINT16 i;

    pVinExtMem->vin_id_or_chan_id = (UINT8)VinId;
    pVinExtMem->allocation_mode = VIN_EXT_MEM_MODE_NEW;
    pVinExtMem->overflow_cntl = VIN_EXT_MEM_CTRL_CIRCULAR;

    pVinExtMem->num_frm_buf = pResource->RawBufNum[VinId];
    if (pVinExtMem->num_frm_buf > EXT_MEM_VIN_MAX_NUM) {
        AmbaLL_LogUInt5("Only %d ExtMem supported", EXT_MEM_VIN_MAX_NUM, 0U, 0U, 0U, 0U);
    }

//FIXME, YuvInput
    pVinExtMem->chroma_format = DSP_YUV_MONO;

    if (IsAuxData == (UINT8)0U) {
        pVinExtMem->buf_pitch = ALIGN_NUM16(pResource->RawBuf[VinId].Pitch, (UINT16)DSP_BUF_ALIGNMENT);
        pVinExtMem->buf_width = pVinExtMem->buf_pitch;
        pVinExtMem->buf_height = (UINT16)pResource->RawBuf[VinId].Window.Height;

        if (pResource->RawAllocType[VinId] == ALLOC_EXTERNAL_DISTINCT) {
            pVinExtMem->allocation_type = VIN_EXT_MEM_ALLOC_TYPE_DISTINCT;
            HL_GetPointerToDspExtVinRawBufArray(VinId, &pBufTblAddr);
            for (i = 0U; i<pResource->RawBufNum[VinId]; i++) {
                (void)dsp_osal_virt2cli(pResource->RawBufTbl[VinId][i], &PhysAddr);
                pBufTblAddr[i] = PhysAddr;
            }
            dsp_osal_typecast(&ULAddr, &pBufTblAddr);
            (void)dsp_osal_virt2cli(ULAddr, &pVinExtMem->buf_addr);
        } else {
            pVinExtMem->allocation_type = VIN_EXT_MEM_ALLOC_TYPE_CONTIGUOUS;
            (void)dsp_osal_virt2cli(pResource->RawBuf[VinId].BaseAddr, &pVinExtMem->buf_addr);
        }
        pVinExtMem->memory_type = DSP_VIN_EXT_MEM_TYPE_RAW;
    } else {
        pVinExtMem->buf_pitch = ALIGN_NUM16(pResource->CeBuf[VinId].Pitch, (UINT16)DSP_BUF_ALIGNMENT);
        pVinExtMem->buf_width = pVinExtMem->buf_pitch;
        pVinExtMem->buf_height = (UINT16)pResource->CeBuf[VinId].Window.Height;

        if (pResource->RawAllocType[VinId] == ALLOC_EXTERNAL_DISTINCT) {
            pVinExtMem->allocation_type = VIN_EXT_MEM_ALLOC_TYPE_DISTINCT;

            HL_GetPointerToDspExtVinAuxBufArray(VinId, &pBufTblAddr);
            for (i = 0U; i<pResource->RawBufNum[VinId]; i++) {
                (void)dsp_osal_virt2cli(pResource->CeBufTbl[VinId][i], &PhysAddr);
                pBufTblAddr[i] = PhysAddr;
            }
            dsp_osal_typecast(&ULAddr, &pBufTblAddr);
            (void)dsp_osal_virt2cli(ULAddr, &pVinExtMem->buf_addr);
        } else {
            pVinExtMem->allocation_type = VIN_EXT_MEM_ALLOC_TYPE_CONTIGUOUS;
            (void)dsp_osal_virt2cli(pResource->CeBuf[VinId].BaseAddr, &pVinExtMem->buf_addr);
        }
        pVinExtMem->memory_type = DSP_VIN_EXT_MEM_TYPE_HDS;
    }

    return Rval;
}

static inline UINT32 HL_FillVinInitCapInLv(cmd_vin_initiate_raw_cap_to_ext_buf_t *pVinInitRawCap,
                                           UINT16 VinId,
                                           const CTX_VIN_INFO_s *pVinInfo)
{
    UINT32 Rval = OK;

    pVinInitRawCap->vin_id = (UINT8)VinId;
    pVinInitRawCap->stop_condition = 0xFFFFFFFFU;
//FIXME, YuvInput
    pVinInitRawCap->input_data_type = (UINT8)DSP_VIN_SEND_IN_DATA_RAW;

    if (pVinInfo->CfaCompressed == 0U) {
        pVinInitRawCap->raw_compression_type = (UINT8)DSP_VIN_CMPR_NONE;
    } else {
        pVinInitRawCap->raw_compression_type = (UINT8)DSP_VIN_CMPR_6_75;
    }

    pVinInitRawCap->raw_cap_done_sw_ctrl = (UINT8)DSP_VIN_CAP_DONE_2_IDLE;
    pVinInitRawCap->raw_buffer_proc_ctrl = (UINT8)DSP_VIN_CAP_TO_YUV;

//FIXME
    pVinInitRawCap->raw_cap_sync_event = (UINT8)0U;

    return Rval;
}


static inline UINT32 HL_LvFillVinCmdOnVinExtMem(const UINT8 WriteMode,
                                                UINT16 VinId,
                                                UINT16 ViewZoneId,
                                                const CTX_RESOURCE_INFO_s *pResource)
{
    UINT8 IsVirtVin = 0U;
    UINT32 Rval = OK;
    CTX_VIN_INFO_s VinInfo = {0};
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    (void)HL_IsVirtualVinIdx(VinId, &IsVirtVin);

    if ((IsVirtVin == 0U) && (VinId < AMBA_DSP_MAX_VIN_NUM)) {
        HL_GetVinInfo(HL_MTX_OPT_ALL, VinId, &VinInfo);
        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);

        if ((pResource->RawAllocType[VinId] == ALLOC_EXTERNAL_DISTINCT) ||
            (pResource->RawAllocType[VinId] == ALLOC_EXTERNAL_CYCLIC)) {
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_vin_set_ext_mem_t *VinExtMem = HL_DefCtxCmdBufPtrVinExtMem;
            cmd_vin_initiate_raw_cap_to_ext_buf_t *VinInitRawCap = HL_DefCtxCmdBufPtrVinInitCap;

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VinExtMem, &CmdBufferAddr);
            Rval = HL_FillVinExtMemInLv(VinExtMem, VinId, (UINT8)0U/*IsAux*/, pResource);
            if (Rval != OK) {
                AmbaLL_LogUInt5("VinExtMem[%d] filling fail", VinId, 0U, 0U, 0U, 0U);
            } else {
                (void)AmbaHL_CmdVinSetExtMem(WriteMode, VinExtMem);
            }
            HL_RelCmdBuffer(CmdBufferId);

            if ((ViewZoneInfo->HdrBlendNumMinusOne > 0U) || (ViewZoneInfo->LinearCe > 0U)) {
                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&VinExtMem, &CmdBufferAddr);
                Rval = HL_FillVinExtMemInLv(VinExtMem, VinId, (UINT8)1U/*IsAux*/, pResource);
                if (Rval != OK) {
                    AmbaLL_LogUInt5("VinExtMem[%d] Aux filling fail", VinId, 0U, 0U, 0U, 0U);
                } else {
                    (void)AmbaHL_CmdVinSetExtMem(WriteMode, VinExtMem);
                }
                HL_RelCmdBuffer(CmdBufferId);
            }

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VinInitRawCap, &CmdBufferAddr);
            Rval = HL_FillVinInitCapInLv(VinInitRawCap, VinId, &VinInfo);
            if (Rval != OK) {
                AmbaLL_LogUInt5("VinInitCap[%d] filling fail", VinId, 0U, 0U, 0U, 0U);
            } else {
                (void)AmbaHL_CmdVinInitiateRawCapToExtBuf(WriteMode, VinInitRawCap);
            }
            HL_RelCmdBuffer(CmdBufferId);
        } else {
            //DO NOTHING
        }
    }

    return Rval;
}

static inline UINT32 HL_LvFillVinCmdOnVinStart(const UINT8 WriteMode,
                                               UINT16 VinId,
                                               const UINT8 *pVinStartPurpose,
                                               cmd_vin_start_t *pVinStartCfg)
{
    UINT32 Rval;
    UINT8 IsVirtVin = 0U;
    CTX_VIN_INFO_s VinInfo = {0};

    (void)HL_IsVirtualVinIdx(VinId, &IsVirtVin);

    Rval = HL_FillVinStartCfg(pVinStartCfg, VinId, *pVinStartPurpose, VIN_START_CFG_CMD, NULL/*DataCapCfg*/);
    if (Rval != OK) {
        AmbaLL_LogUInt5("VinStartCfg[%d] Opt[0x%X] filling fail", VinId, VIN_START_CFG_CMD, 0U, 0U, 0U);
    } else {
        (void)AmbaHL_CmdVinStart(WriteMode, pVinStartCfg);
    }
    /* For Virtual Vin, VinState is either ACTIVE or INVALID */
    if (IsVirtVin == 1U) {
        HL_GetVinInfo(HL_MTX_OPT_GET, VinId, &VinInfo);
        VinInfo.VinCtrl.VinState = DSP_VIN_STATUS_ACTIVE;
        HL_SetVinInfo(HL_MTX_OPT_SET, VinId, &VinInfo);
    }

    return Rval;
}

static inline UINT32 HL_LvFillVinCmdOnHdrCeTd(const UINT8 WriteMode,
                                              const UINT8 HdrBlendNumMinusOne,
                                              const CTX_VIN_INFO_s *pVinInfo,
                                              cmd_vin_ce_setup_t *pCeSetp,
                                              cmd_vin_hdr_setup_t *pHdrSetup,
                                              UINT8 *pSkipCmd,
                                              UINT32 *pVinBitMask)
{
    UINT8 SkipCmd = *pSkipCmd;
    UINT16 i;
    UINT32 VinBitMask = *pVinBitMask;
    UINT32 Rval = OK;

    if (pVinInfo->TimeDivisionNum[0U] > 1U) {
        for (i=1U; i<pVinInfo->TimeDivisionNum[0U]; i++) {
            UINT32 TdVinId = (UINT32)pVinInfo->TimeDivisionVinId[0U][i] + AMBA_DSP_MAX_VIN_NUM;

            if (0U == DSP_GetBit(VinBitMask, TdVinId, 1U)) {
                SkipCmd = (UINT8)0xFFU; //Force update
                Rval = HL_FillVinCeSetup(pVinInfo->TimeDivisionVinId[0U][0U], pCeSetp, &SkipCmd);
                if (Rval != OK) {
                    //TBD
                } else if (SkipCmd == 1U) {
                    //Skip cmd
                } else {
                    pCeSetp->vin_id = (UINT8)TdVinId;
                    (void)AmbaHL_CmdVinCeSetup(WriteMode, pCeSetp);
                }

                if (HdrBlendNumMinusOne > 0U) {
                    SkipCmd = (UINT8)0xFFU; //Force update
                    Rval = HL_FillVinHdrSetup(pVinInfo->TimeDivisionVinId[0U][0U], pHdrSetup, 0U, &SkipCmd);
                    if (Rval != OK) {
                        //TBD
                    } else if (SkipCmd == 1U) {
                        //Skip cmd
                    } else {
                        pHdrSetup->vin_id = (UINT8)TdVinId;
                        (void)AmbaHL_CmdVinHdrSetup(WriteMode, pHdrSetup);
                    }
                }
                DSP_SetBit(&VinBitMask, TdVinId);
            }
        }
    }
    *pSkipCmd = SkipCmd;
    *pVinBitMask = VinBitMask;

    return Rval;
}

static inline UINT32 HL_LvFillVinCmdOnHdrCe(const UINT8 WriteMode,
                                            UINT16 VinId,
                                            UINT16 ViewZoneId)
{
    UINT8 IsVirtVin = 0U;
    UINT32 Rval = OK;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_VIN_INFO_s VinInfo = {0};

    (void)HL_IsVirtualVinIdx(VinId, &IsVirtVin);

    if (IsVirtVin == 0U) {
        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);

        if ((ViewZoneInfo->HdrBlendNumMinusOne > 0U) ||
            (ViewZoneInfo->LinearCe > 0U) ||
            (ViewZoneInfo->InputMuxSelSrc >= 1U)) {

            UINT8 SkipCmd;
            UINT8 CmdBufferIdCe = 0U, CmdBufferIdHdr = 0U; void *CmdBufferAddr = NULL;
            UINT32 VinBitMask = 0x0U;
            cmd_vin_ce_setup_t *CeSetp = HL_DefCtxCmdBufPtrVinCe;
            cmd_vin_hdr_setup_t *HdrSetup = HL_DefCtxCmdBufPtrVinHdr;

            HL_GetVinInfo(HL_MTX_OPT_ALL, VinId, &VinInfo);
            HL_AcqCmdBuffer(&CmdBufferIdCe, &CmdBufferAddr);
            dsp_osal_typecast(&CeSetp, &CmdBufferAddr);
            /* all cmd need to be re-send for DefCmdQ(ie. ModeSwitch) */
            SkipCmd = (WriteMode == AMBA_DSP_CMD_DEFAULT_WRITE)? (UINT8)0xFFU: (UINT8)0U;
            Rval = HL_FillVinCeSetup(VinId, CeSetp, &SkipCmd);
            if (Rval != OK) {
                // DO NOTHING
            } else if (SkipCmd == 1U) {
                // Skip it
            } else {
                (void)AmbaHL_CmdVinCeSetup(WriteMode, CeSetp);
            }
            HL_AcqCmdBuffer(&CmdBufferIdHdr, &CmdBufferAddr);
            dsp_osal_typecast(&HdrSetup, &CmdBufferAddr);
            if (ViewZoneInfo->HdrBlendNumMinusOne > 0U) {
                /* all cmd need to be re-send for DefCmdQ(ie. ModeSwitch) */
                SkipCmd = (WriteMode == AMBA_DSP_CMD_DEFAULT_WRITE)? (UINT8)0xFFU: (UINT8)0U;
                Rval = HL_FillVinHdrSetup(VinId, HdrSetup, 0U, &SkipCmd);
                if (Rval != OK) {
                    // DO NOTHING
                } else if (SkipCmd == 1U) {
                    // Skip it
                } else {
                    (void)AmbaHL_CmdVinHdrSetup(WriteMode, HdrSetup);
                }
            }
            DSP_SetBit(&VinBitMask, VinId);

            // Consider HDR/CE case when TD
            Rval = HL_LvFillVinCmdOnHdrCeTd(WriteMode,
                                            ViewZoneInfo->HdrBlendNumMinusOne,
                                            &VinInfo,
                                            CeSetp,
                                            HdrSetup,
                                            &SkipCmd,
                                            &VinBitMask);
            HL_RelCmdBuffer(CmdBufferIdCe);
            HL_RelCmdBuffer(CmdBufferIdHdr);
            if (Rval != OK) {
                AmbaLL_LogUInt5("HL_LvFillVinCmdOnHdrCeTd Error:0x%x vin:%u vz:%u",
                        Rval, VinId, ViewZoneId, 0U, 0U);
            }
        }
    }

    return Rval;
}

static inline UINT32 HL_LvFillVinCmdOnTD(const UINT8 WriteMode,
                                         UINT16 VinId)
{
    UINT32 Rval = OK;
    CTX_VIN_INFO_s VinInfo = {0};
    UINT8 IsVirtVin = 0U;
    UINT16 i, j;

    (void)HL_IsVirtualVinIdx(VinId, &IsVirtVin);

    if (IsVirtVin == 0U) {
        HL_GetVinInfo(HL_MTX_OPT_ALL, VinId, &VinInfo);

        if (VinInfo.TimeDivisionNum[0U] > 1U) {
            UINT16 TotalNum = 0U, Idx = 0U;
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_vin_temporal_demux_setup_t *VinTDSetup = HL_DefCtxCmdBufPtrVinTd;

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VinTDSetup, &CmdBufferAddr);
            VinTDSetup->vin_id = (UINT8)VinId;
            for (i=0U; i<VinInfo.TimeDivisionNum[0U]; i++) {
                TotalNum += VinInfo.TimeDivisionFrmNum[0U][i];
            }
            VinTDSetup->temporal_demux_num = (UINT8)TotalNum;
            VinTDSetup->en_temporal_demux = (UINT8)1U;

            for (i=0U; i<VinInfo.TimeDivisionNum[0U]; i++) {
                for (j=0U; j<VinInfo.TimeDivisionFrmNum[0U][i]; j++) {
                    if (i == 0U) {
                        VinTDSetup->temporal_demux_tar[Idx] = (UINT8)VinId;
                    } else {
                        VinTDSetup->temporal_demux_tar[Idx] = (UINT8)(VinInfo.TimeDivisionVinId[0U][i] + AMBA_DSP_MAX_VIN_NUM);
                    }
                    Idx++;
                }
            }

            (void)AmbaHL_CmdVinTemporalDemuxSetup(WriteMode, VinTDSetup);
            HL_RelCmdBuffer(CmdBufferId);
        }
    }

    return Rval;
}

static inline UINT32 HL_LvFillVinCmdOnFlip(const UINT8 WriteMode,
                                           UINT16 VinId,
                                           UINT16 ViewZoneId,
                                           const CTX_RESOURCE_INFO_s *pResource)
{
    UINT8 IsVirtVin = 0U;
    UINT16 i, ViewZoneVinId = 0U;
    UINT32 Rval = OK;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    (void)HL_IsVirtualVinIdx(VinId, &IsVirtVin);

    if (IsVirtVin == 0U) {
        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);

        if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_HW) {
            for (i=0U; i<pResource->MaxViewZoneNum; i++) {
                if (0U == DSP_GetBit(pResource->ViewZoneActiveBit, i, 1U)) {
                    continue;
                }
                HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
                if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON) {
                    continue;
                }
                if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) {
                    continue;
                }
                (void)HL_GetViewZoneVinId(i, &ViewZoneVinId);
                if (ViewZoneVinId == VinId) {
                    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
                    cmd_vin_set_frm_lvl_flip_rot_control_t *FrmLvlRot = HL_DefCtxCmdBufPtrVinFlip;

                    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                    dsp_osal_typecast(&FrmLvlRot, &CmdBufferAddr);
                    FrmLvlRot->vin_id = (UINT8)VinId;
                    FrmLvlRot->fov_id = (UINT8)HL_GetViewZoneLocalIndexOnVin(i);
                    FrmLvlRot->vproc_hflip_control = HL_IS_HFLIP(ViewZoneInfo->MainRotate);
                    FrmLvlRot->vproc_vflip_control = HL_IS_VFLIP(ViewZoneInfo->MainRotate);
                    /* uCode doesn't support the terms below */
                    /* Per ChenHan 20191211, VFlip perform before Sec2 */
                    /*                       HFlip perform before Sec3 */
                    FrmLvlRot->vproc_rotation_control = 0U;
                    FrmLvlRot->vout_hflip_control = 0U;
                    FrmLvlRot->vout_vflip_control = 0U;
                    FrmLvlRot->vout_rotation_control = 0U;
                    (void)AmbaHL_CmdVinSetFrmLvlFlipRotControl(WriteMode, FrmLvlRot);
                    HL_RelCmdBuffer(CmdBufferId);
                }
            }
        }
    }

    return Rval;
}

#ifndef SUPPORT_VPROC_DLY
static inline UINT32 HL_LvFillVinCmdOnDly(const UINT8 WriteMode,
                                          UINT16 VinId,
                                          UINT16 ViewZoneId,
                                          const CTX_RESOURCE_INFO_s *pResource)
{
    UINT8 IsVirtVin = 0U;
    UINT16 i, ViewZoneVinId = 0U;
    UINT32 Rval = OK;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    (void)HL_IsVirtualVinIdx(VinId, &IsVirtVin);

    if (IsVirtVin == 0U) {
        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);

        if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_HW) {
            for (i=0U; i<pResource->MaxViewZoneNum; i++) {
                if (0U == DSP_GetBit(pResource->ViewZoneActiveBit, i, 1U)) {
                    continue;
                }
                HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
                if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON) {
                    continue;
                }
                if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) {
                    continue;
                }
                (void)HL_GetViewZoneVinId(i, &ViewZoneVinId);
                if (ViewZoneVinId == VinId) {
                    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
                    cmd_vin_set_frm_vproc_delay_t *VinFrmVprocDelay = HL_DefCtxCmdBufPtrVinDly;

                    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                    VinFrmVprocDelay->vin_id = (UINT8)VinId;
                    VinFrmVprocDelay->delay_time_msec = ViewZoneInfo->ProcPostPoneTime[AMBA_DSP_VZ_POSTPONE_STAGE_PRE_R2Y]/10U;
                    Rval = AmbaHL_CmdVinSetFrmVprocDelay(WriteMode, VinFrmVprocDelay);
                    HL_RelCmdBuffer(CmdBufferId);
                }
            }
        }
    }

    return Rval;
}
#endif

static inline UINT32 HL_LvFillVinCmdOnEvnMask(const UINT8 WriteMode,
                                              UINT16 VinId,
                                              const CTX_RESOURCE_INFO_s *pResource)
{
    UINT32 Rval = OK;
    UINT8 IsVirtVin = 0U;
    UINT16 i, j, TDPhyVinId = DSP_VIRT_VIN_IDX_INVALID, ViewZoneVinId = 0U;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr, *TmpViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT32 EventMask;

    (void)HL_IsVirtualVinIdx(VinId, &IsVirtVin);
    (void)HL_GetTimeDivisionVirtVinInfo(VinId, &TDPhyVinId);

    if ((IsVirtVin == 0U) ||
        ((IsVirtVin == 1U) && (TDPhyVinId != DSP_VIRT_VIN_IDX_INVALID))) {
        UINT32 ViewZoneBitMask, EventFovMask, BitMask;
#ifndef SUPPORT_DSP_EXT_PIN_BUF
        UINT16 PrevPinIdx = 0U;
#endif

        /* Generate actual ViewZoneBitMask */
        ViewZoneBitMask = pResource->ViewZoneActiveBit;
        for (i=0U; i< pResource->MaxViewZoneNum; i++) {
            if (0U == DSP_GetBit(ViewZoneBitMask, i, 1U)) {
                continue;
            }
            (void)HL_GetViewZoneVinId(i, &ViewZoneVinId);
            if (VinId != ViewZoneVinId) {
                DSP_ClearBit(&ViewZoneBitMask, i);
            }
        }

        for (i=0U; i<pResource->MaxViewZoneNum; i++) {
            if (0U == DSP_GetBit(ViewZoneBitMask, i, 1U)) {
                continue;
            }
            HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);

#ifdef SUPPORT_DSP_EXT_PIN_BUF
            /* Using VprocExtBufMask */
            if (ViewZoneInfo->VinDecimationRate > 1U) {
                EventFovMask = 0U;
                BitMask = 0U;

//FIXME, FovMask setting when different FOV has different Event
                DSP_SetBit(&EventFovMask, HL_GetViewZoneLocalIndexOnVin(i));
                DSP_SetBit(&BitMask, i);

                for (j=i+1U; j<pResource->MaxViewZoneNum; j++) {
                    if (0U == DSP_GetBit(ViewZoneBitMask, i, 1U)) {
                        continue;
                    }
                    HL_GetViewZoneInfoPtr(j, &TmpViewZoneInfo);
                    if (TmpViewZoneInfo->VinDecimationRate == ViewZoneInfo->VinDecimationRate) {
                        DSP_SetBit(&EventFovMask, HL_GetViewZoneLocalIndexOnVin(j));
                        DSP_SetBit(&BitMask, j);
                    }
                }

                if (EventFovMask > 0U) {
                    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
                    cmd_vin_attach_event_to_raw_t *VinAttachEvent = HL_DefCtxCmdBufPtrVinAttRaw;

                    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                    dsp_osal_typecast(&VinAttachEvent, &CmdBufferAddr);
                    VinAttachEvent->vin_id = (UINT8)VinId;
                    (void)dsp_osal_memcpy(&EventMask, &VinAttachEvent->event_mask, sizeof(UINT32));
                    DSP_SetBit(&EventMask, DSP_BIT_POS_VPROC_RPT);
                    (void)dsp_osal_memcpy(&VinAttachEvent->event_mask, &EventMask, sizeof(UINT32));
                    VinAttachEvent->repeat_cnt = ((UINT32)ViewZoneInfo->VinDecimationRate << 16U) | 0x1U; // B[0:15] Repeat, B[16:31] Decimation
                    VinAttachEvent->time_duration_bet_rpts = 0U; //TBD
                    VinAttachEvent->fov_mask_valid = EventFovMask;

                    (void)AmbaHL_CmdVinAttachEventToRaw(WriteMode, VinAttachEvent);
                    HL_RelCmdBuffer(CmdBufferId);
                    ViewZoneBitMask &= ~BitMask;
                }
            } else {
                DSP_ClearBit(&ViewZoneBitMask, i);
            }
#else
            if ((ViewZoneInfo->VinDecimationRate > 1U) ||
                (ViewZoneInfo->PinComposeExtMem > 0U)) {
                EventFovMask = 0U;
                BitMask = 0U;

//FIXME, FovMask setting when different FOV has differnt Event
                DSP_SetBit(&EventFovMask, HL_GetViewZoneLocalIndexOnVin(i));
                DSP_SetBit(&BitMask, i);

                for (j=i+1U; j<pResource->MaxViewZoneNum; j++) {
                    if (0U == DSP_GetBit(ViewZoneBitMask, i, 1U)) {
                        continue;
                    }
                    HL_GetViewZoneInfoPtr(j, &TmpViewZoneInfo);
                    if (TmpViewZoneInfo->VinDecimationRate == ViewZoneInfo->VinDecimationRate) {
                        DSP_SetBit(&EventFovMask, HL_GetViewZoneLocalIndexOnVin(j));
                        DSP_SetBit(&BitMask, j);
                    }
                }

                if (EventFovMask > 0U) {
                    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
                    cmd_vin_attach_event_to_raw_t *VinAttachEvent = NULL;

                    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                    dsp_osal_typecast(&VinAttachEvent, &CmdBufferAddr);
                    VinAttachEvent->vin_id = (UINT8)VinId;
                    (void)dsp_osal_memcpy(&EventMask, &VinAttachEvent->event_mask, sizeof(UINT32));
                    DSP_SetBit(&EventMask, DSP_BIT_POS_VPROC_RPT);
                    for (PrevPinIdx = 0U; PrevPinIdx < DSP_VPROC_PIN_NUM; PrevPinIdx++) {
                        if (1U == DSP_GetU8Bit(ViewZoneInfo->PinComposeExtMem, PrevPinIdx, 1U)) {
                            DSP_SetBit(&EventMask, HL_VprocPinVinAttachBitMap[PrevPinIdx]);
                        }
                    }
                    (void)dsp_osal_memcpy(&VinAttachEvent->event_mask, &EventMask, sizeof(UINT32));

                    VinAttachEvent->repeat_cnt = ((UINT32)ViewZoneInfo->VinDecimationRate << 16U) | 0x1U; // B[0:15] Repeat, B[16:31] Decimation
                    VinAttachEvent->time_duration_bet_rpts = 0U; //TBD
                    VinAttachEvent->fov_mask_valid = EventFovMask;

                    (void)AmbaHL_CmdVinAttachEventToRaw(WriteMode, VinAttachEvent);
                    HL_RelCmdBuffer(CmdBufferId);
                    ViewZoneBitMask &= ~BitMask;
                }
            } else {
                DSP_ClearBit(&ViewZoneBitMask, i);
            }
#endif
        }
    }

    return Rval;
}

/*
 * AmbaHL_CmdVinSetExtMem
 * AmbaHL_CmdVinSendInputData with/without isocfg
 */
static inline UINT32 HL_LvFillInvalidVinCmdOnDec(const UINT8 WriteMode, UINT16 VinId)
{
    UINT32 Rval = OK;
    UINT32 NewWp = 0U;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    AMBA_DSP_BUF_s ExtYuvBuf = {0};
    CTX_VID_DEC_INFO_s VidDecInfo = {0};
    cmd_vin_set_ext_mem_t *VinExtMem = HL_DefCtxCmdBufPtrVinExtMem;
    cmd_vin_send_input_data_t *FeedData = HL_DefCtxCmdBufPtrVinSndData;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 DecIdx = 0U;
#ifdef DUPLEX_DEC_SHARE_VIRT_VIN
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_DSP_INST_INFO_s DspInstInfo = {0};
    UINT16 i;
#endif

#ifdef DUPLEX_DEC_SHARE_VIRT_VIN
    HL_GetResourcePtr(&Resource);
    HL_GetDspInstance(HL_MTX_OPT_ALL, &DspInstInfo);
    if ((DspInstInfo.DecSharedVirtVinId != DSP_VIRT_VIN_IDX_INVALID) &&
        (DspInstInfo.DecSharedVirtVinId + AMBA_DSP_MAX_VIN_NUM == VinId)) {
        for (i = 0U; i < Resource->MaxViewZoneNum; i++) {
            HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
            if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) {
                DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &DecIdx);
                HL_GetVidDecInfo(HL_MTX_OPT_GET, DecIdx, &VidDecInfo);

                if (VidDecInfo.ExtYuvBuf.Pitch != 0U) {
                    ExtYuvBuf.BaseAddr = VidDecInfo.ExtYuvBuf.BaseAddrY;
                    ExtYuvBuf.Pitch = VidDecInfo.ExtYuvBuf.Pitch;
                    ExtYuvBuf.Window.OffsetX = VidDecInfo.ExtYuvBuf.Window.OffsetX;
                    ExtYuvBuf.Window.OffsetY = VidDecInfo.ExtYuvBuf.Window.OffsetY;
                    ExtYuvBuf.Window.Width   = VidDecInfo.ExtYuvBuf.Window.Width;
                    ExtYuvBuf.Window.Height  = VidDecInfo.ExtYuvBuf.Window.Height;
                    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                    dsp_osal_typecast(&VinExtMem, &CmdBufferAddr);
                    (void)HL_LiveviewFillVinExtMem((UINT32)VinId,
                                                   0U /* IsYuvVin2Enc */,
                                                   1U /* IsFirstMem */,
                                                   DSP_VIN_EXT_MEM_TYPE_YUV422 /* MemType */,
                                                   YuvFmtTable[VidDecInfo.ExtYuvBuf.DataFmt] /* ChromaFmt */,
                                                   VIN_EXT_MEM_CTRL_WAIT_APPEND /* OverFlowCtrl */,
                                                   &ExtYuvBuf,
                                                   NULL,
                                                   VinExtMem);
                    /*
                     * dec2vproc vin buffer need alignment for easy of use
                     * if we use 1080 height for first vin_ext_mem,
                     * ucode we use 1080 to create memory map,
                     * if pic_info_cache=1,
                     * we cannot re-create memory map by vin_ext_mem.allocation_mode=0,
                     * so we use 1088 for the virtual vin
                     *
                     */
                    VinExtMem->buf_height = ALIGN_NUM16(VinExtMem->buf_height, 16U);
                    (void)AmbaHL_CmdVinSetExtMem(WriteMode, VinExtMem);
                    HL_RelCmdBuffer(CmdBufferId);

                    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                    dsp_osal_typecast(&FeedData, &CmdBufferAddr);
                    FeedData->vin_id = VinId;
                    FeedData->chan_id = 0U; //obsoleted
                    FeedData->send_data_dest = DSP_VIN_SEND_IN_DATA_DEST_VPROC;
                    FeedData->is_compression_en = 0U;
                    FeedData->blk_sz = 0U;
                    FeedData->mantissa = 0U;
                    FeedData->raw_frm_cap_cnt = VidDecInfo.YuvFrameCnt;
                    FeedData->input_data_type = (VidDecInfo.ExtYuvBuf.DataFmt == AMBA_DSP_YUV420)? (UINT32)DSP_VIN_SEND_IN_DATA_YUV420: (UINT32)DSP_VIN_SEND_IN_DATA_YUV422;
                    FeedData->ext_fb_idx = DSP_FB_IDX_FOR_NEW2APPEND;

                    HL_GetViewZoneInfoLock(VidDecInfo.ViewZoneId, &ViewZoneInfo);
                    if ((ViewZoneInfo->StartIsoCfgAddr > 0U) &&
                        (VidDecInfo.FirstIsoCfgIssued == (UINT8)0U)) {
                        /* Request BatchCmdQ buffer */
                        Rval = DSP_ReqBuf(&ViewZoneInfo->BatchQPoolDesc, 1U, &NewWp, 1U/*FullnessCheck*/);
                        if (Rval != OK) {
                            HL_GetViewZoneInfoUnLock(VidDecInfo.ViewZoneId);
                            AmbaLL_LogWarnUInt5((UINT16)AMBALLLOG_WARN_BATCHQ_FULL, "[FeedYuvData] ViewZone[%d] batch pool is full", VidDecInfo.ViewZoneId, 0U, 0U, 0U, 0U);
                        } else {
                            UINT32 *pBatchQAddr, BatchCmdId;
                            cmd_vproc_ik_config_t *pIsoCfgCmd;
                            ULONG BatchQAddr = 0U;

                            ViewZoneInfo->BatchQPoolDesc.Wp = NewWp;
                            VidDecInfo.FirstIsoCfgIssued = (UINT8)1U;
                            HL_GetViewZoneInfoUnLock(VidDecInfo.ViewZoneId);
                            HL_GetPointerToDspBatchQ(VidDecInfo.ViewZoneId, (UINT16)ViewZoneInfo->BatchQPoolDesc.Wp, &pBatchQAddr, &BatchCmdId);

                            /* Reset New BatchQ after Wp advanced */
                            HL_ResetDspBatchQ(pBatchQAddr);

                            /* IsoCfgCmd occupied first CmdSlot */
                            dsp_osal_typecast(&pIsoCfgCmd, &pBatchQAddr);
                            (void)HL_FillIsoCfgUpdate(VidDecInfo.ViewZoneId, pIsoCfgCmd);
                            (void)dsp_osal_virt2cli(ViewZoneInfo->StartIsoCfgAddr, &pIsoCfgCmd->idsp_flow_addr);
                            pIsoCfgCmd->ik_cfg_id = ViewZoneInfo->StartIsoCfgIndex;

                            dsp_osal_typecast(&BatchQAddr, &pBatchQAddr);
                            (void)dsp_osal_virt2cli(BatchQAddr, &FeedData->batch_cmd_set_info.addr);
                            HL_SetDspBatchQInfo(BatchCmdId, 1/*AutoReset*/, BATCHQ_INFO_ISO_CFG, ViewZoneInfo->StartIsoCfgIndex);
                            FeedData->batch_cmd_set_info.id = BatchCmdId;
                            FeedData->batch_cmd_set_info.size = HL_GetBatchCmdNumber(BatchQAddr)*CMD_SIZE_IN_BYTE;

                            (void)AmbaHL_CmdVinSendInputData(WriteMode, FeedData);
                        }
                    } else {
                        HL_GetViewZoneInfoUnLock(VidDecInfo.ViewZoneId);
                        (void)AmbaHL_CmdVinSendInputData(WriteMode, FeedData);
                    }
                    HL_RelCmdBuffer(CmdBufferId);

                    VidDecInfo.YuvFrameCnt++;
                } else {
                    //DO NOTHING
                }
                HL_SetVidDecInfo(HL_MTX_OPT_SET, DecIdx, &VidDecInfo);
            }
        }
    } else {
        //TBD
    }
#else
    //Find Dec instance idx by pairing VirtVinId with input VinId
    (void)HL_GetDecIdxFromVinId(VinId, &DecIdx);
    HL_GetVidDecInfo(HL_MTX_OPT_GET, DecIdx, &VidDecInfo);
    if ((VinId == (VidDecInfo.YuvInVirtVinId + AMBA_DSP_MAX_VIN_NUM)) &&
        (VidDecInfo.ExtYuvBuf.Pitch != 0U)) {
        //AmbaLL_LogUInt5("HL_LiveviewFillVinCmds[%d] dec Pitch=%d", __LINE__, VidDecInfo.ExtYuvBuf.Pitch, 0, 0, 0);
        ExtYuvBuf.BaseAddr = VidDecInfo.ExtYuvBuf.BaseAddrY;
        ExtYuvBuf.Pitch = VidDecInfo.ExtYuvBuf.Pitch;
        ExtYuvBuf.Window.OffsetX = VidDecInfo.ExtYuvBuf.Window.OffsetX;
        ExtYuvBuf.Window.OffsetY = VidDecInfo.ExtYuvBuf.Window.OffsetY;
        ExtYuvBuf.Window.Width   = VidDecInfo.ExtYuvBuf.Window.Width;
        ExtYuvBuf.Window.Height  = VidDecInfo.ExtYuvBuf.Window.Height;
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&VinExtMem, &CmdBufferAddr);
        (void)HL_LiveviewFillVinExtMem((UINT32)VinId,
                                       0U /* IsYuvVin2Enc */,
                                       1U /* IsFirstMem */,
                                       DSP_VIN_EXT_MEM_TYPE_YUV422 /* MemType */,
                                       YuvFmtTable[VidDecInfo.ExtYuvBuf.DataFmt] /* ChromaFmt */,
                                       VIN_EXT_MEM_CTRL_WAIT_APPEND /* OverFlowCtrl */,
                                       &ExtYuvBuf,
                                       NULL,
                                       VinExtMem);
        /*
         * dec2vproc vin buffer need alignment for easy of use
         * if we use 1080 height for first vin_ext_mem,
         * ucode we use 1080 to create memory map,
         * if pic_info_cache=1,
         * we cannot re-create memory map by vin_ext_mem.allocation_mode=0,
         * so we use 1088 for the virtual vin
         *
         */
        VinExtMem->buf_height = ALIGN_NUM16(VinExtMem->buf_height, 16U);
        (void)AmbaHL_CmdVinSetExtMem(WriteMode, VinExtMem);
        HL_RelCmdBuffer(CmdBufferId);

        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&FeedData, &CmdBufferAddr);
        FeedData->vin_id = (UINT8)VinId;
        FeedData->chan_id = 0U; //obsoleted
        FeedData->send_data_dest = DSP_VIN_SEND_IN_DATA_DEST_VPROC;
        FeedData->is_compression_en = 0U;
        FeedData->blk_sz = 0U;
        FeedData->mantissa = 0U;
        FeedData->raw_frm_cap_cnt = VidDecInfo.YuvFrameCnt;
        FeedData->input_data_type = (VidDecInfo.ExtYuvBuf.DataFmt == AMBA_DSP_YUV420)? (UINT32)DSP_VIN_SEND_IN_DATA_YUV420: (UINT32)DSP_VIN_SEND_IN_DATA_YUV422;
        FeedData->ext_fb_idx = DSP_FB_IDX_FOR_NEW2APPEND;

        HL_GetViewZoneInfoLock(VidDecInfo.ViewZoneId, &ViewZoneInfo);
        if ((ViewZoneInfo->StartIsoCfgAddr > 0U) &&
            (VidDecInfo.FirstIsoCfgIssued == (UINT8)0U)) {
            /* Request BatchCmdQ buffer */
            Rval = DSP_ReqBuf(&ViewZoneInfo->BatchQPoolDesc, 1U, &NewWp, 1U/*FullnessCheck*/);
            if (Rval != OK) {
                HL_GetViewZoneInfoUnLock(VidDecInfo.ViewZoneId);
                AmbaLL_LogWarnUInt5((UINT16)AMBALLLOG_WARN_BATCHQ_FULL, "[FeedYuvData] ViewZone[%d] batch pool is full", VidDecInfo.ViewZoneId, 0U, 0U, 0U, 0U);
            } else {
                UINT32 *pBatchQAddr, BatchCmdId;
                cmd_vproc_ik_config_t *pIsoCfgCmd;
                ULONG BatchQAddr = 0U;

                ViewZoneInfo->BatchQPoolDesc.Wp = NewWp;
                VidDecInfo.FirstIsoCfgIssued = (UINT8)1U;
                HL_GetViewZoneInfoUnLock(VidDecInfo.ViewZoneId);
                HL_GetPointerToDspBatchQ(VidDecInfo.ViewZoneId, (UINT16)ViewZoneInfo->BatchQPoolDesc.Wp, &pBatchQAddr, &BatchCmdId);

                /* Reset New BatchQ after Wp advanced */
                HL_ResetDspBatchQ(pBatchQAddr);

                /* IsoCfgCmd occupied first CmdSlot */
                dsp_osal_typecast(&pIsoCfgCmd, &pBatchQAddr);
                (void)HL_FillIsoCfgUpdate(VidDecInfo.ViewZoneId, pIsoCfgCmd);
                (void)dsp_osal_virt2cli(ViewZoneInfo->StartIsoCfgAddr, &pIsoCfgCmd->idsp_flow_addr);
                pIsoCfgCmd->ik_cfg_id = ViewZoneInfo->StartIsoCfgIndex;

                dsp_osal_typecast(&BatchQAddr, &pBatchQAddr);
                (void)dsp_osal_virt2cli(BatchQAddr, &FeedData->batch_cmd_set_info.addr);
                HL_SetDspBatchQInfo(BatchCmdId, 1/*AutoReset*/, BATCHQ_INFO_ISO_CFG, ViewZoneInfo->StartIsoCfgIndex);
                FeedData->batch_cmd_set_info.id = BatchCmdId;
                FeedData->batch_cmd_set_info.size = HL_GetBatchCmdNumber(BatchQAddr)*CMD_SIZE_IN_BYTE;

                (void)AmbaHL_CmdVinSendInputData(WriteMode, FeedData);
            }
        } else {
            HL_GetViewZoneInfoUnLock(VidDecInfo.ViewZoneId);
            (void)AmbaHL_CmdVinSendInputData(WriteMode, FeedData);
        }
        HL_RelCmdBuffer(CmdBufferId);

        VidDecInfo.YuvFrameCnt++;
    } else {
        //DO NOTHING
    }
    HL_SetVidDecInfo(HL_MTX_OPT_SET, DecIdx, &VidDecInfo);
#endif

    return Rval;
}

/*
 * AmbaHL_CmdVinSetExtMem
 * AmbaHL_CmdVinSendInputData with/without isocfg
 */
static inline UINT32 HL_LvFillInvalidVinCmdOnMem(const UINT8 WriteMode, UINT16 VinId)
{
    UINT8 ExitLoop = 0U;
    UINT16 i, ViewZoneId = 0U, ViewZoneVinId = 0U;
    UINT32 Rval = OK, NewWp = 0U;
    AMBA_DSP_BUF_s ExtYuvBuf = {0};
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);
    for (i = 0U; i < Resource->MaxViewZoneNum; i++) {
        if (0U == DSP_GetBit(Resource->ViewZoneActiveBit, i, 1U)) {
            continue;
        }
        HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
        if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_HW) ||
            (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) ||
            (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON)) {
            continue;
        }
        (void)HL_GetViewZoneVinId(i, &ViewZoneVinId);
        if (ViewZoneVinId == VinId) {
            ViewZoneId = i;
            ExitLoop = 1U;
            break;
        }
    }

    if ((ExitLoop == 1U) &&
        (ViewZoneInfo->ExtYuvBuf.Pitch != 0U)) {
        UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
        cmd_vin_set_ext_mem_t *VinExtMem = HL_DefCtxCmdBufPtrVinExtMem;
        cmd_vin_send_input_data_t *FeedData = HL_DefCtxCmdBufPtrVinSndData;

        ExtYuvBuf.BaseAddr = ViewZoneInfo->ExtYuvBuf.BaseAddrY;
        ExtYuvBuf.Pitch = ViewZoneInfo->ExtYuvBuf.Pitch;
        ExtYuvBuf.Window.OffsetX = ViewZoneInfo->ExtYuvBuf.Window.OffsetX;
        ExtYuvBuf.Window.OffsetY = ViewZoneInfo->ExtYuvBuf.Window.OffsetY;
        ExtYuvBuf.Window.Width   = ViewZoneInfo->ExtYuvBuf.Window.Width;
        ExtYuvBuf.Window.Height  = ViewZoneInfo->ExtYuvBuf.Window.Height;
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&VinExtMem, &CmdBufferAddr);
        (void)HL_LiveviewFillVinExtMem((UINT32)VinId,
                                       0U /* IsYuvVin2Enc */,
                                       1U /* IsFirstMem */,
                                       DSP_VIN_EXT_MEM_TYPE_YUV422 /* MemType */,
                                       YuvFmtTable[ViewZoneInfo->ExtYuvBuf.DataFmt] /* ChromaFmt */,
                                       VIN_EXT_MEM_CTRL_WAIT_APPEND /* OverFlowCtrl */,
                                       &ExtYuvBuf,
                                       NULL,
                                       VinExtMem);
        /* dec2vproc vin buffer need alignment for easy of use
         * if we use 1080 height for first vin_ext_mem,
         * ucode we use 1080 to create memory map,
         * if pic_info_cache=1,
         * we cannot re-create memory map by vin_ext_mem.allocation_mode=0,
         * so we use 1088 for the virtual vin
         *  */
        VinExtMem->buf_height = ALIGN_NUM16(VinExtMem->buf_height, 16U);
        (void)AmbaHL_CmdVinSetExtMem(WriteMode, VinExtMem);
        HL_RelCmdBuffer(CmdBufferId);

        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&FeedData, &CmdBufferAddr);
        FeedData->vin_id = (UINT8)VinId;
        FeedData->chan_id = 0U; //obsoleted
        FeedData->send_data_dest = DSP_VIN_SEND_IN_DATA_DEST_VPROC;
        FeedData->is_compression_en = 0U;
        FeedData->blk_sz = 0U;
        FeedData->mantissa = 0U;
        FeedData->raw_frm_cap_cnt = ViewZoneInfo->YuvFrameCnt;
        FeedData->input_data_type = (ViewZoneInfo->ExtYuvBuf.DataFmt == AMBA_DSP_YUV420)? (UINT32)DSP_VIN_SEND_IN_DATA_YUV420: (UINT32)DSP_VIN_SEND_IN_DATA_YUV422;
        FeedData->ext_fb_idx = DSP_FB_IDX_FOR_NEW2APPEND;

        if (ViewZoneInfo->StartIsoCfgAddr > 0U) {
            /* Request BatchCmdQ buffer */
            Rval = DSP_ReqBuf(&ViewZoneInfo->BatchQPoolDesc, 1U, &NewWp, 1U/*FullnessCheck*/);
            if (Rval != OK) {
                AmbaLL_LogWarnUInt5((UINT16)AMBALLLOG_WARN_BATCHQ_FULL, "[FeedYuvData] ViewZone[%d] batch pool is full", ViewZoneId, 0U, 0U, 0U, 0U);
            } else {
                UINT32 *pBatchQAddr, BatchCmdId;
                cmd_vproc_ik_config_t *pIsoCfgCmd;
                ULONG BatchQAddr = 0U;

                HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
                ViewZoneInfo->BatchQPoolDesc.Wp = NewWp;
                HL_GetViewZoneInfoUnLock(ViewZoneId);
                HL_GetPointerToDspBatchQ(ViewZoneId, (UINT16)ViewZoneInfo->BatchQPoolDesc.Wp, &pBatchQAddr, &BatchCmdId);

                /* Reset New BatchQ after Wp advanced */
                HL_ResetDspBatchQ(pBatchQAddr);

                /* IsoCfgCmd occupied first CmdSlot */
                dsp_osal_typecast(&pIsoCfgCmd, &pBatchQAddr);
                (void)HL_FillIsoCfgUpdate(ViewZoneId, pIsoCfgCmd);
                (void)dsp_osal_virt2cli(ViewZoneInfo->StartIsoCfgAddr, &pIsoCfgCmd->idsp_flow_addr);
                pIsoCfgCmd->ik_cfg_id = ViewZoneInfo->StartIsoCfgIndex;

                dsp_osal_typecast(&BatchQAddr, &pBatchQAddr);
                (void)dsp_osal_virt2cli(BatchQAddr, &(FeedData->batch_cmd_set_info.addr));
                HL_SetDspBatchQInfo(BatchCmdId, 1/*AutoReset*/, BATCHQ_INFO_ISO_CFG, ViewZoneInfo->StartIsoCfgIndex);
                FeedData->batch_cmd_set_info.id = BatchCmdId;
                FeedData->batch_cmd_set_info.size = HL_GetBatchCmdNumber(BatchQAddr)*CMD_SIZE_IN_BYTE;
                (void)AmbaHL_CmdVinSendInputData(WriteMode, FeedData);
            }
        } else {
            HL_GetViewZoneInfoUnLock(ViewZoneId);
            (void)AmbaHL_CmdVinSendInputData(WriteMode, FeedData);
        }
        HL_RelCmdBuffer(CmdBufferId);

        HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
        ViewZoneInfo->YuvFrameCnt++;
        HL_GetViewZoneInfoUnLock(ViewZoneId);
    } else {
        //DO NOTHING
    }

    return Rval;
}

static inline UINT32 HL_LiveviewFillVinCmdsInvalid2ndHalf(const UINT8 WriteMode, UINT16 VinId, UINT16 ViewZoneId, const CTX_RESOURCE_INFO_s *pResource)
{
    UINT32 Rval;

    /* 20190909, Per uCoder, we need to issue VinInitRawCap after VinStartCmd */
    Rval = HL_LvFillVinCmdOnVinExtMem(WriteMode, VinId, ViewZoneId, pResource);


    /* Vertical or horizontal flip */
    if (Rval == OK) {
        Rval = HL_LvFillVinCmdOnFlip(WriteMode, VinId, ViewZoneId, pResource);
    }

    /* Some Vin Relative cmd must be put after VinStartCmd */
#ifndef SUPPORT_VPROC_DLY
    /* Vertical or horizontal flip */
    if (Rval == OK) {
        Rval = HL_LvFillVinCmdOnDly(WriteMode, VinId, ViewZoneId, pResource);
    }
#endif

    /* Only Handle VinFovDecimation here, ExtPymd/ExtLndt can be set in VprocSetupCmd */
    if (Rval == OK) {
        Rval = HL_LvFillVinCmdOnEvnMask(WriteMode, VinId, pResource);
    }

    /* For dec2vproc yuv boot case */
    if (Rval == OK) {
        Rval = HL_LvFillInvalidVinCmdOnDec(WriteMode, VinId);
    }

    /* For mem yuv boot case */
    //need to get viewzone inside HL_LvFillVinCmdOnMem
    if (Rval == OK) {
        Rval = HL_LvFillInvalidVinCmdOnMem(WriteMode, VinId);
    }

    return Rval;
}

static inline UINT32 HL_LiveviewFillVinCmdsInvalid(const UINT8 WriteMode, UINT16 VinId)
{
    UINT32 Rval = OK;
    UINT8 IsVirtVin = 0U;
    UINT8 VinStartPurpose = VIN_START_LIVEVIEW_ONLY;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT16 i, ViewZoneId = 0U, ViewZoneVinId = 0U;
    CTX_VIN_INFO_s VinInfo = {0};
    cmd_vin_start_t *VinStartCfg = HL_DefCtxCmdBufPtrVinStart;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    HL_GetResourcePtr(&Resource);

    (void)HL_IsVirtualVinIdx(VinId, &IsVirtVin);

    if (IsVirtVin == 0U) {
        HL_GetVinInfo(HL_MTX_OPT_ALL, VinId, &VinInfo);

        /** Fetch first viewzone information of this Vin */
        for (i = 0U; i < Resource->MaxViewZoneNum; i++) {
            if (0U == DSP_GetBit(Resource->ViewZoneActiveBit, i, 1U)) {
                continue;
            }
            HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
            if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) ||
                (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON)) {
                continue;
            }
            (void)HL_GetViewZoneVinId(i, &ViewZoneVinId);
            if (ViewZoneVinId == VinId) {
                ViewZoneId = i;
                break;
            }
        }
        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);

        Rval = HL_LvFillVinCmdOnVinCfg(WriteMode, VinId);
    }

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&VinStartCfg, &CmdBufferAddr);
    if (Rval == OK) {
        /* Pack IsoCfg first for fetching HDR relative information from it */
        Rval = HL_LvFillVinCmdOnPreVinStart(VinId, &VinStartPurpose, VinStartCfg);
    }

    /* Ce(HDR)Setup, inform uCode to allocate resource BEFORE VinStartCmd */
    if (Rval == OK) {
        Rval = HL_LvFillVinCmdOnHdrCe(WriteMode, VinId, ViewZoneId);
    }

//FIXME, VirtVin
    if (Rval == OK) {
        Rval = HL_LvFillVinCmdOnTD(WriteMode, VinId);
    }

    if (Rval == OK) {
        /* VinStartCmd is used for activate VinFp,
         * even in VideoRawEncode, we still need to make VinProc alive before sending Raw/Yuv file
         */
        Rval = HL_LvFillVinCmdOnVinStart(WriteMode, VinId, &VinStartPurpose, VinStartCfg);
    }

    if (Rval == OK) {
        Rval = HL_LiveviewFillVinCmdsInvalid2ndHalf(WriteMode, VinId, ViewZoneId, Resource);
    }
    HL_RelCmdBuffer(CmdBufferId);

    return Rval;
}

/*
 * AmbaHL_CmdVinSetExtMem as normal case(not first time)
 * AmbaHL_CmdVinSendInputData with/without isocfg
 */
static inline UINT32 HL_LvFillActiveVinCmdOnDecFeed(const UINT8 WriteMode, UINT16 VinId)
{
    UINT16 DecIdx = 0U;
    UINT32 Rval = OK;
    UINT32 NewWp = 0U;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    AMBA_DSP_BUF_s ExtYuvBuf = {0};
    CTX_VID_DEC_INFO_s VidDecInfo = {0};
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    cmd_vin_set_ext_mem_t *VinExtMem = HL_DefCtxCmdBufPtrVinExtMem;
    cmd_vin_send_input_data_t *FeedData = HL_DefCtxCmdBufPtrVinSndData;

    //Find Dec instance idx by pairing VirtVinId with input VinId
    (void)HL_GetDecIdxFromVinId(VinId, &DecIdx);
    HL_GetVidDecInfo(HL_MTX_OPT_GET, DecIdx, &VidDecInfo);
    if ((VinId == (VidDecInfo.YuvInVirtVinId + AMBA_DSP_MAX_VIN_NUM)) &&
        (VidDecInfo.ExtYuvBuf.Pitch != 0U)) {

        /* Must bind back to BIND_STILL_PROC first, at HL_LvBindDecVprocToVirtVin
         * Move ExtYuvBufIdx forward,
         * ucode memory map idx maximum is: EXT_MEM_VIN_MAX_NUM,
         * If you always use allocation_mode=VIN_EXT_MEM_MODE_NEW,
         * You don't need to update cmd_vin_send_input_data_t.ext_fb_idx
         */
        VidDecInfo.ExtYuvBufIdx = (UINT16)((VidDecInfo.ExtYuvBufIdx + 1U) % EXT_MEM_VIN_MAX_NUM);

        //AmbaLL_LogUInt5("HL_LiveviewFillVinCmds[%d] dec Pitch=%d", __LINE__, VidDecInfo.ExtYuvBuf.Pitch, 0, 0, 0);
        ExtYuvBuf.BaseAddr = VidDecInfo.ExtYuvBuf.BaseAddrY;
        ExtYuvBuf.Pitch = VidDecInfo.ExtYuvBuf.Pitch;
        ExtYuvBuf.Window.OffsetX = VidDecInfo.ExtYuvBuf.Window.OffsetX;
        ExtYuvBuf.Window.OffsetY = VidDecInfo.ExtYuvBuf.Window.OffsetY;
        ExtYuvBuf.Window.Width   = VidDecInfo.ExtYuvBuf.Window.Width;
        ExtYuvBuf.Window.Height  = VidDecInfo.ExtYuvBuf.Window.Height;
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&VinExtMem, &CmdBufferAddr);
        (void)HL_LiveviewFillVinExtMem((UINT32)VinId,
                                       0U /* IsYuvVin2Enc */,
                                       0U /* IsFirstMem */,
                                       DSP_VIN_EXT_MEM_TYPE_YUV422 /* MemType */,
                                       YuvFmtTable[VidDecInfo.ExtYuvBuf.DataFmt] /* ChromaFmt */,
                                       VIN_EXT_MEM_CTRL_WAIT_APPEND /* OverFlowCtrl */,
                                       &ExtYuvBuf,
                                       NULL,
                                       VinExtMem);
        /* dec2vproc vin buffer need alignment for easy of use
         * if we use 1080 height for first vin_ext_mem,
         * ucode we use 1080 to create memory map,
         * if pic_info_cache=1,
         * we cannot re-create memory map by vin_ext_mem.allocation_mode=0,
         * so we use 1088 for the virtual vin
         *  */
        VinExtMem->buf_height = ALIGN_NUM16(VinExtMem->buf_height, 16U);
        (void)AmbaHL_CmdVinSetExtMem(WriteMode, VinExtMem);
        HL_RelCmdBuffer(CmdBufferId);

        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&FeedData, &CmdBufferAddr);
        FeedData->vin_id = (UINT8)VinId;
        FeedData->chan_id = 0U; //obsoleted
        FeedData->send_data_dest = DSP_VIN_SEND_IN_DATA_DEST_VPROC;
        FeedData->is_compression_en = 0U;
        FeedData->blk_sz = 0U;
        FeedData->mantissa = 0U;
        FeedData->raw_frm_cap_cnt = VidDecInfo.YuvFrameCnt;
        FeedData->input_data_type = (VidDecInfo.ExtYuvBuf.DataFmt == AMBA_DSP_YUV420)? (UINT32)DSP_VIN_SEND_IN_DATA_YUV420: (UINT32)DSP_VIN_SEND_IN_DATA_YUV422;
        FeedData->ext_fb_idx = DSP_FB_IDX_FOR_NEW2APPEND;

        HL_GetViewZoneInfoLock(VidDecInfo.ViewZoneId, &ViewZoneInfo);

        if ((ViewZoneInfo->StartIsoCfgAddr > 0U) &&
            (VidDecInfo.FirstIsoCfgIssued == (UINT8)0U)) {
            /* Request BatchCmdQ buffer */
            Rval = DSP_ReqBuf(&ViewZoneInfo->BatchQPoolDesc, 1U, &NewWp, 1U/*FullnessCheck*/);
            if (Rval != OK) {
                HL_GetViewZoneInfoUnLock(VidDecInfo.ViewZoneId);
                AmbaLL_LogWarnUInt5((UINT16)AMBALLLOG_WARN_BATCHQ_FULL, "[FeedYuvData] ViewZone[%d] batch pool is full", VidDecInfo.ViewZoneId, 0U, 0U, 0U, 0U);
            } else {
                UINT32 *pBatchQAddr, BatchCmdId;
                cmd_vproc_ik_config_t *pIsoCfgCmd;
                ULONG BatchQAddr = 0U;

                ViewZoneInfo->BatchQPoolDesc.Wp = NewWp;
                VidDecInfo.FirstIsoCfgIssued = (UINT8)1U;
                HL_GetViewZoneInfoUnLock(VidDecInfo.ViewZoneId);
                HL_GetPointerToDspBatchQ(VidDecInfo.ViewZoneId, (UINT16)ViewZoneInfo->BatchQPoolDesc.Wp, &pBatchQAddr, &BatchCmdId);

                /* Reset New BatchQ after Wp advanced */
                HL_ResetDspBatchQ(pBatchQAddr);

                /* IsoCfgCmd occupied first CmdSlot */
                dsp_osal_typecast(&pIsoCfgCmd, &pBatchQAddr);
                (void)HL_FillIsoCfgUpdate(VidDecInfo.ViewZoneId, pIsoCfgCmd);
                (void)dsp_osal_virt2cli(ViewZoneInfo->StartIsoCfgAddr, &pIsoCfgCmd->idsp_flow_addr);
                pIsoCfgCmd->ik_cfg_id = ViewZoneInfo->StartIsoCfgIndex;

                dsp_osal_typecast(&BatchQAddr, &pBatchQAddr);
                (void)dsp_osal_virt2cli(BatchQAddr, &FeedData->batch_cmd_set_info.addr);
                HL_SetDspBatchQInfo(BatchCmdId, 1/*AutoReset*/, BATCHQ_INFO_ISO_CFG, ViewZoneInfo->StartIsoCfgIndex);
                FeedData->batch_cmd_set_info.id = BatchCmdId;
                FeedData->batch_cmd_set_info.size = HL_GetBatchCmdNumber(BatchQAddr)*CMD_SIZE_IN_BYTE;

                (void)AmbaHL_CmdVinSendInputData(WriteMode, FeedData);
            }
        } else {
            HL_GetViewZoneInfoUnLock(VidDecInfo.ViewZoneId);
            (void)AmbaHL_CmdVinSendInputData(WriteMode, FeedData);
        }
        HL_RelCmdBuffer(CmdBufferId);
        VidDecInfo.YuvFrameCnt++;
    } else {
        //DO NOTHING
    }
    HL_SetVidDecInfo(HL_MTX_OPT_SET, DecIdx, &VidDecInfo);

    return Rval;
}

/*
 * AmbaHL_CmdVinSetExtMem as normal case(not first time)
 * AmbaHL_CmdVinSendInputData with/without isocfg
 */
static inline UINT32 HL_LvFillActiveVinCmdOnMemFeed(const UINT8 WriteMode, UINT16 VinId)
{
    UINT8 ExitLoop;
    UINT16 i, ViewZoneId=0U, ViewZoneVinId=0U;
    UINT32 Rval = OK, NewWp = 0U;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    AMBA_DSP_BUF_s ExtYuvBuf = {0};
    cmd_vin_set_ext_mem_t *VinExtMem = HL_DefCtxCmdBufPtrVinExtMem;
    cmd_vin_send_input_data_t *FeedData = HL_DefCtxCmdBufPtrVinSndData;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    /** Fetch first virtual vin's viewzone information */
    HL_GetResourcePtr(&Resource);
    ExitLoop = 0U;
    for (i = 0U; i < Resource->MaxViewZoneNum; i++) {
        if (0U == DSP_GetBit(Resource->ViewZoneActiveBit, i, 1U)) {
            continue;
        }
        HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
        if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) ||
            (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON)) {
            continue;
        }
        (void)HL_GetViewZoneVinId(i, &ViewZoneVinId);
        if (ViewZoneVinId == VinId) {
            ViewZoneId = i;
            ExitLoop = 1U;
            break;
        }
    }

    HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
    if ((ExitLoop == 1U) &&
        (VinId == ViewZoneVinId) &&
        (ViewZoneInfo->ExtYuvBuf.Pitch != 0U)) {

        ExtYuvBuf.BaseAddr = ViewZoneInfo->ExtYuvBuf.BaseAddrY;
        ExtYuvBuf.Pitch = ViewZoneInfo->ExtYuvBuf.Pitch;
        ExtYuvBuf.Window.OffsetX = ViewZoneInfo->ExtYuvBuf.Window.OffsetX;
        ExtYuvBuf.Window.OffsetY = ViewZoneInfo->ExtYuvBuf.Window.OffsetY;
        ExtYuvBuf.Window.Width   = ViewZoneInfo->ExtYuvBuf.Window.Width;
        ExtYuvBuf.Window.Height  = ViewZoneInfo->ExtYuvBuf.Window.Height;
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&VinExtMem, &CmdBufferAddr);
        (void)HL_LiveviewFillVinExtMem((UINT32)VinId,
                                       0U /* IsYuvVin2Enc */,
                                       1U /* IsFirstMem */,
                                       DSP_VIN_EXT_MEM_TYPE_YUV422 /* MemType */,
                                       YuvFmtTable[ViewZoneInfo->ExtYuvBuf.DataFmt] /* ChromaFmt */,
                                       VIN_EXT_MEM_CTRL_WAIT_APPEND /* OverFlowCtrl */,
                                       &ExtYuvBuf,
                                       NULL,
                                       VinExtMem);
        /* dec2vproc vin buffer need alignment for easy of use
         * if we use 1080 height for first vin_ext_mem,
         * ucode we use 1080 to create memory map,
         * if pic_info_cache=1,
         * we cannot re-create memory map by vin_ext_mem.allocation_mode=0,
         * so we use 1088 for the virtual vin
         *  */
        VinExtMem->buf_height = ALIGN_NUM16(VinExtMem->buf_height, 16U);
        (void)AmbaHL_CmdVinSetExtMem(WriteMode, VinExtMem);
        HL_RelCmdBuffer(CmdBufferId);

        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&FeedData, &CmdBufferAddr);
        FeedData->vin_id = (UINT8)VinId;
        FeedData->chan_id = 0U; //obsoleted
        FeedData->send_data_dest = DSP_VIN_SEND_IN_DATA_DEST_VPROC;
        FeedData->is_compression_en = 0U;
        FeedData->blk_sz = 0U;
        FeedData->mantissa = 0U;
        FeedData->raw_frm_cap_cnt = ViewZoneInfo->YuvFrameCnt;
        FeedData->input_data_type = (ViewZoneInfo->ExtYuvBuf.DataFmt == AMBA_DSP_YUV420)? (UINT32)DSP_VIN_SEND_IN_DATA_YUV420: (UINT32)DSP_VIN_SEND_IN_DATA_YUV422;
        FeedData->ext_fb_idx = DSP_FB_IDX_FOR_NEW2APPEND;

        if (ViewZoneInfo->StartIsoCfgAddr > 0U) {
            /* Request BatchCmdQ buffer */
            Rval = DSP_ReqBuf(&ViewZoneInfo->BatchQPoolDesc, 1U, &NewWp, 1U/*FullnessCheck*/);
            if (Rval != OK) {
                AmbaLL_LogWarnUInt5((UINT16)AMBALLLOG_WARN_BATCHQ_FULL, "[FeedYuvData] ViewZone[%d] batch pool is full", ViewZoneId, 0U, 0U, 0U, 0U);
            } else {
                UINT32 *pBatchQAddr, BatchCmdId;
                cmd_vproc_ik_config_t *pIsoCfgCmd;
                ULONG BatchQAddr = 0U;

                HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
                ViewZoneInfo->BatchQPoolDesc.Wp = NewWp;
                HL_GetViewZoneInfoUnLock(ViewZoneId);
                HL_GetPointerToDspBatchQ(ViewZoneId, (UINT16)ViewZoneInfo->BatchQPoolDesc.Wp, &pBatchQAddr, &BatchCmdId);

                /* Reset New BatchQ after Wp advanced */
                HL_ResetDspBatchQ(pBatchQAddr);

                /* IsoCfgCmd occupied first CmdSlot */
                dsp_osal_typecast(&pIsoCfgCmd, &pBatchQAddr);
                (void)HL_FillIsoCfgUpdate(ViewZoneId, pIsoCfgCmd);
                (void)dsp_osal_virt2cli(ViewZoneInfo->StartIsoCfgAddr, &pIsoCfgCmd->idsp_flow_addr);
                pIsoCfgCmd->ik_cfg_id = ViewZoneInfo->StartIsoCfgIndex;

                dsp_osal_typecast(&BatchQAddr, &pBatchQAddr);
                (void)dsp_osal_virt2cli(BatchQAddr, &FeedData->batch_cmd_set_info.addr);
                HL_SetDspBatchQInfo(BatchCmdId, 1/*AutoReset*/, BATCHQ_INFO_ISO_CFG, ViewZoneInfo->StartIsoCfgIndex);
                FeedData->batch_cmd_set_info.id = BatchCmdId;
                FeedData->batch_cmd_set_info.size = HL_GetBatchCmdNumber(BatchQAddr)*CMD_SIZE_IN_BYTE;

                (void)AmbaHL_CmdVinSendInputData(WriteMode, FeedData);
            }
        } else {
            HL_GetViewZoneInfoUnLock(ViewZoneId);
            (void)AmbaHL_CmdVinSendInputData(WriteMode, FeedData);
        }
        HL_RelCmdBuffer(CmdBufferId);

        HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
        ViewZoneInfo->YuvFrameCnt++;
        /* Must bind back to BIND_STILL_PROC first, at HL_LvBindDecVprocToVirtVin
         * Move ExtYuvBufIdx forward,
         * ucode memory map idx maximum is: EXT_MEM_VIN_MAX_NUM,
         * If you always use allocation_mode=VIN_EXT_MEM_MODE_NEW,
         * You don't need to update cmd_vin_send_input_data_t.ext_fb_idx
         */
        ViewZoneInfo->ExtYuvBufIdx = (UINT16)((ViewZoneInfo->ExtYuvBufIdx + 1U) % EXT_MEM_VIN_MAX_NUM);
        HL_GetViewZoneInfoUnLock(ViewZoneId);
    } else {
        //DO NOTHING
    }

    return Rval;
}

/* In duplex pip case, when liveview ctrl enable without resource/lvcfg change,
 * virtual vin would not stopped at previous liveview ctrl disable,
 * duplex pip need send input data to trigger effect
 * */
static inline UINT32 HL_LiveviewFillVinCmdsActive(const UINT8 WriteMode, UINT16 VinId)
{
    UINT32 Rval;

    Rval = HL_LvFillActiveVinCmdOnDecFeed(WriteMode, VinId);
    if (Rval != OK) {
        AmbaLL_LogUInt5("HL_LiveviewFillVinCmdsActive[%d] VinId:%u HL_LvFillActiveVinCmdOnDecFeed fail:0x%x",
                __LINE__, VinId, Rval, 0, 0);
    }
    Rval = HL_LvFillActiveVinCmdOnMemFeed(WriteMode, VinId);
    if (Rval != OK) {
        AmbaLL_LogUInt5("HL_LiveviewFillVinCmdsActive[%d] VinId:%u HL_LvFillActiveVinCmdOnMemFeed fail:0x%x",
                __LINE__, VinId, Rval, 0, 0);
    }
    return Rval;
}

static inline UINT32 HL_LiveviewVinSetupImpl(const UINT8 WriteMode)
{
    UINT32 Rval = OK;
    UINT8 IsVirtVin = 0U;
    UINT16 i, VinExist;
    CTX_VIN_INFO_s VinInfo = {0};
#ifdef SUPPORT_DSP_SET_RAW_CAP_CNT
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_vin_set_raw_frm_cap_cnt_t *VinSetFrmCnt = HL_DefCtxCmdBufPtrVinCnt;
#endif

    /* Fetch Vin active status */
    for (i = 0; i < DSP_VIN_MAX_NUM; i++) {
        VinExist = 0U;
        HL_GetVinExistence(i, &VinExist);
        HL_GetVinInfo(HL_MTX_OPT_ALL, i, &VinInfo);

        (void)HL_IsVirtualVinIdx(i, &IsVirtVin);

        if (IsVirtVin == 1U) {
            if ((VinExist > 0U) &&
                ((VinInfo.VinCtrl.VinState == DSP_VIN_STATUS_INVALID) ||
                 (VinInfo.VinCtrl.VinState == DSP_VIN_STATUS_IDLE2RUN))) {
                Rval = HL_LiveviewFillVinCmdsInvalid(WriteMode, i);
            } else if (VinInfo.VinCtrl.VinState == DSP_VIN_STATUS_ACTIVE) {
                Rval = HL_LiveviewFillVinCmdsActive(WriteMode, i);
            } else if (VinInfo.VinCtrl.VinState == DSP_VIN_STATUS_CTX_RESET) {
#ifdef SUPPORT_DSP_SET_RAW_CAP_CNT
                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&VinSetFrmCnt, &CmdBufferAddr);
                VinSetFrmCnt->vin_id = (UINT8)i;
                VinSetFrmCnt->cap_seq_no = 0;
                (void)AmbaHL_CmdVinSetRawFrmCapCnt(WriteMode, VinSetFrmCnt);
                HL_RelCmdBuffer(CmdBufferId);
#else
                AmbaLL_LogUInt5("No SetRawFrmCapCntCmd in this chip", 0U, 0U, 0U, 0U, 0U);
#endif
                HL_GetVinInfo(HL_MTX_OPT_GET, i, &VinInfo);
                VinInfo.VinCtrl.VinState = DSP_VIN_STATUS_ACTIVE;
                HL_SetVinInfo(HL_MTX_OPT_SET, i, &VinInfo);
            } else {
                //
            }
        } else {
            if ((VinExist > 0U) && (VinInfo.VinCtrl.VinState == DSP_VIN_STATUS_IDLE2RUN)) {
                Rval = HL_LiveviewFillVinCmdsInvalid(WriteMode, i);
            }
        }

        if (Rval != OK) {
            break;
        }
    }

    return Rval;
}

static inline UINT32 HL_LvVoutSetupOnChkReset(const CTX_RESOURCE_INFO_s *pResource,
                                              const CTX_VOUT_INFO_s *pVoutInfo,
                                              const UINT8 *pEnablePrev,
                                              UINT8 *pResetVout)
{
    UINT32 Rval = OK;
    UINT8 DspOpModeInit, DspOpModeIdle;
    UINT16 i;

    DspOpModeInit = (DSP_GetProfState() == DSP_PROF_STATUS_INVALID)? 1U: 0U;
    DspOpModeIdle = (DSP_GetProfState() == DSP_PROF_STATUS_IDLE)? 1U: 0U;

    if ((DspOpModeInit == 1U) || (DspOpModeIdle == 1U)) { //booting
        for (i=0U; i<AMBA_DSP_MAX_VOUT_NUM; i++) {
            if ((pEnablePrev[i] == 1U) || \
                 (pResource->MaxVoutWidth[i] > 0U) || \
                 (pResource->MaxOsdBufWidth[i] > 0U)) {
                pResetVout[i] = 1U;
            }
        }
    } else { //DSP mode switch, or change resolution
        for (i=0U; i<AMBA_DSP_MAX_VOUT_NUM; i++) {
            if ((pEnablePrev[i] == 1U) && (pVoutInfo[i].VideoEnable == 0U)) {
                pResetVout[i] = 1U;
            } else if ((pEnablePrev[i] == 1U) &&
                       (pVoutInfo[i].VideoEnable == 1U) &&
                       (pVoutInfo[i].DisplayStatus == DSP_VOUT_DISPLAY_RESET)) {
                pResetVout[i] = 1U;
            } else {
                // DO NOTHING
            }
        }
    }

    return Rval;
}

static inline UINT32 HL_LvVoutSetupOnReset(const UINT8 WriteMode,
                                           const UINT8 *pResetVout)
{
    UINT32 Rval = OK;
    UINT16 i;

    for (i = 0U; i<AMBA_DSP_MAX_VOUT_NUM; i++) {
        if (pResetVout[i] > 0U) {
            (void)HL_VoutResetSetup((UINT8)i, WriteMode);
            (void)HL_VoutDisplaySetup((UINT8)i, WriteMode);
            if (i == VOUT_IDX_A) {
                (void)HL_VoutGammaSetup((UINT8)i, WriteMode);
            }
            if (i == VOUT_IDX_B) {
                (void)HL_VoutDveSetup((UINT8)i, WriteMode);
            }
            (void)HL_VoutMixerSetup((UINT8)i, WriteMode);
        }
    }

    return Rval;
}

static inline UINT32 HL_LvVoutSetupOnVoutSync(const UINT8 WriteMode,
                                              const UINT16 IsVinSync,
                                              const UINT8 Timing,
                                              UINT16 *pVoutSyncEnable,
                                              UINT8 *pVoutVinSync,
                                              const CTX_VOUT_INFO_s *pVoutInfo)
{
#define VOUT_SYNC_WITH_VIN  (0U)
#define VOUT_SYNC_WITH_VOUT (1U)
    UINT32 Rval = OK;
    UINT16 i;
    UINT8 VoutVinSyncCnt = (UINT8)0U;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_vin_vout_lock_setup_t *VinVoutLock = HL_DefCtxCmdBufPtrVinVoutLock;
    UINT16 VinTargetIndex = 0U;

    if (IsVinSync > 0U) {
        for (i = 0U; i<AMBA_DSP_MAX_VOUT_NUM; i++) {
            if ((pVoutInfo[i].DispCfg.VinVoutSyncDelay > 0U) ||
                (pVoutInfo[i].DispCfg.VoutSyncDelay > 0U)) {
                if (pVoutInfo[i].DispCfg.VinVoutSyncDelay > 0U) {
                    pVoutSyncEnable[i] |= 1U;
                    VinTargetIndex = pVoutInfo[i].DispCfg.VinTargetIndex;
                }
                if (pVoutInfo[i].DispCfg.VoutSyncDelay > 0U) {
                    pVoutSyncEnable[i] |= ((UINT16)1U << VOUT_SYNC_WITH_VOUT);
                }
            }
        }
    }

    /* VinVoutLockSetup */
    for (i = 0U; i<AMBA_DSP_MAX_VOUT_NUM; i++) {
        if (1U == DSP_GetU16Bit(pVoutSyncEnable[i], VOUT_SYNC_WITH_VIN, 1U)) {
            pVoutVinSync[i] = 1U;
            VoutVinSyncCnt++;
        }
    }

    if ((Timing == VOUT_CFG_TIME_POST_VIN) &&
        (VoutVinSyncCnt > (UINT8)0U)) {

        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&VinVoutLock, &CmdBufferAddr);
        VinVoutLock->vin_id = (UINT8)VinTargetIndex;
        VinVoutLock->en_lock = (UINT8)1U;
        (void)AmbaHL_CmdVinVoutLockSetup(WriteMode, VinVoutLock);
        HL_RelCmdBuffer(CmdBufferId);
        for (i = 0U; i<AMBA_DSP_MAX_VOUT_NUM; i++) {
            if (1U == DSP_GetU16Bit(pVoutSyncEnable[i], VOUT_SYNC_WITH_VIN, 1U)) {
                Rval = DSP_WaitVoutLock((UINT8)i, 1U, WAIT_FLAG_TIMEOUT);
            }
        }
    }

    return Rval;
}

static inline UINT32 HL_LvVoutSetupOnVideoOsdSetup(const UINT8 WriteMode,
                                                   const UINT8 Timing,
                                                   const UINT16 *pVoutSyncEnable,
                                                   const UINT8 *pEnablePrev,
                                                   const CTX_VOUT_INFO_s *pVoutInfo)
{
    UINT32 Rval = OK;
    UINT16 i;
    UINT8 VoutSrc;

    for (i = 0U; i<AMBA_DSP_MAX_VOUT_NUM; i++) {
        if (((Timing == VOUT_CFG_TIME_PRE_VIN) && (pVoutSyncEnable[i] == 0U)) ||
            ((Timing == VOUT_CFG_TIME_POST_VIN) && (pVoutSyncEnable[i] > 0U)) ||
            ((Timing == VOUT_CFG_TIME_PRE_VIN_PROF_SWITCH) && (pVoutSyncEnable[i] == 0U)) ||
            ((Timing == VOUT_CFG_TIME_POST_VIN_PROF_SWITCH) && (pVoutSyncEnable[i] > 0U))) {
            if ((pEnablePrev[i] == 0U) && (pVoutInfo[i].VideoEnable == 1U)) {
                if (pVoutInfo[i].VideoCfg.VideoSource == VOUT_SOURCE_INTERNAL) {
                    VoutSrc = DSP_VOUT_SRC_VCAP;
                } else {
                    VoutSrc = pVoutInfo[i].VideoCfg.VideoSource;
                }
                (void)HL_VoutVideoSetupSourceSelect((UINT8)i, VoutSrc, 0U/*DONT CARE*/);
                (void)HL_VoutVideoSetup((UINT8)i, WriteMode);
            } else if (pEnablePrev[i] == 1U) {
                (void)HL_VoutVideoSetupSourceSelect((UINT8)i, DSP_VOUT_SRC_VCAP, HL_VOUT_SOURCE_VIDEO_CAP);
                (void)HL_VoutVideoSetup((UINT8)i, WriteMode);
                if (pVoutInfo[i].OsdEnable == 1U) {
                    (void)HL_VoutOsdSetup((UINT8)i, WriteMode);
                }
            } else {
                //DO NOTHING
            }
        }
    }

    return Rval;
}

void HL_LiveviewVoutSetupImpl(const UINT8 WriteMode, const UINT8 Timing)
{
    UINT32 Rval, i, j;
    UINT8 EnablePrev[AMBA_DSP_MAX_VOUT_NUM];
    UINT8 ResetVout[AMBA_DSP_MAX_VOUT_NUM];
    UINT16 VoutSyncEnable[AMBA_DSP_MAX_VOUT_NUM], IsVinSync = 0U;
    UINT8 VoutVinSync[AMBA_DSP_MAX_VOUT_NUM];
    CTX_VOUT_INFO_s VoutInfo[AMBA_DSP_MAX_VOUT_NUM];
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;

    for (i = 0U; i < AMBA_DSP_MAX_VOUT_NUM; i++) {
        EnablePrev[i] = 0U;
        ResetVout[i] = 0U;
        VoutSyncEnable[i] = 0U;
        VoutVinSync[i] = 0U;
    }

    HL_GetResourcePtr(&Resource);
    for (i = 0U; i < Resource->YuvStrmNum; i++) {
        HL_GetYuvStrmInfoLock((UINT16)i, &YuvStrmInfo);
        for (j = 0U; j < AMBA_DSP_MAX_VOUT_NUM; j++) {
            if (1U == DSP_GetU16Bit(YuvStrmInfo->DestVout, j, 1U)) {
                EnablePrev[j] |= 1U;
            }
        }
        HL_GetYuvStrmInfoUnLock((UINT16)i);
    }

    for (i = 0U; i<AMBA_DSP_MAX_VOUT_NUM; i++) {
        HL_GetVoutInfo(HL_MTX_OPT_ALL, (UINT16)i, &VoutInfo[i]);

        if (VoutInfo[i].DispCfg.VinVoutSyncDelay > 0U) {
            IsVinSync = 1U;
        }
    }

    AmbaLL_LogUInt5("VoutCmd Timing[%d] ProfStat[%d] IsVinSync[%d]",
                          Timing, DSP_GetProfState(), IsVinSync, 0U, 0U);
    for (i = 0U; i<AMBA_DSP_MAX_VOUT_NUM; i++) {
        AmbaLL_LogUInt5("     Vout[%d] PrevEn[%d] VdoEn[%d] OsdEn[%d] Csc[%d]", i,
                              EnablePrev[i], VoutInfo[i].VideoEnable,
                              VoutInfo[i].OsdEnable, VoutInfo[i].CscUpdated);
        AmbaLL_LogUInt5("              MaxWidth[%d] MaxOsd[%d] StDisp[%d] StMix[%d]",
                              Resource->MaxVoutWidth[i], Resource->MaxOsdBufWidth[i],
                              VoutInfo[i].DisplayStatus, VoutInfo[i].MixerStatus, 0U);
        AmbaLL_LogUInt5("              StOsd[%d] StDve[%d]",
                              VoutInfo[i].OsdStatus, VoutInfo[i].DveStatus,
                              0U, 0U, 0U);
    }

    //Check reset vout?
    Rval = HL_LvVoutSetupOnChkReset(Resource,
                                    &VoutInfo[0U],
                                    &EnablePrev[0U],
                                    &ResetVout[0U]);

    // VoutReset
    if (Rval == OK) {
        Rval = HL_LvVoutSetupOnReset(WriteMode, &ResetVout[0U]);
    }

    if (Rval == OK) {
        Rval = HL_LvVoutSetupOnVoutSync(WriteMode,
                                        IsVinSync,
                                        Timing,
                                        &VoutSyncEnable[0U],
                                        &VoutVinSync[0U],
                                        &VoutInfo[0U]);
    }

    // VoutVideoSetup and OSDSetup
    if (Rval == OK) {
        (void)HL_LvVoutSetupOnVideoOsdSetup(WriteMode,
                                            Timing,
                                            &VoutSyncEnable[0U],
                                            &EnablePrev[0U],
                                            &VoutInfo[0U]);
    }
}

static inline UINT8 HL_PureStillProcessed(void)
{
    UINT8 Rval;
    UINT16 i = 0U, NoActiveVproc = 1U;
    CTX_STILL_INFO_s StlInfo = {0};
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);
    HL_GetStlInfo(HL_MTX_OPT_ALL, &StlInfo);
    if (((StlInfo.RawInVprocId != DSP_VPROC_IDX_INVALID) && (StlInfo.RawInVprocStatus != STL_VPROC_STATUS_IDLE)) ||
        ((StlInfo.YuvInVprocId != DSP_VPROC_IDX_INVALID) && (StlInfo.YuvInVprocStatus != STL_VPROC_STATUS_IDLE))) {
        for (i=0U; i<Resource->MaxViewZoneNum; i++) {
            if (DSP_GetVprocState((UINT8)i) == DSP_VPROC_STATUS_ACTIVE) {
                NoActiveVproc = 0U;
                break;
            }
        }
        if (NoActiveVproc == 1U) {
            Rval = 1U;
        } else {
            Rval = 0U;
        }
    } else {
        Rval = 0U;
    }

    return Rval;
}
static inline UINT32 HL_LiveviewFillPreProcGroupCmds(const UINT8 WriteMode,
                                                     const UINT8 IsPartialEnable)
{
    UINT32 Rval = OK;
    UINT8 DspOpModeInit, DspOpModeIdle, VOutTiming, PureStillProcessed;

    DspOpModeInit = (DSP_GetProfState() == DSP_PROF_STATUS_INVALID)? 1U: 0U;
    DspOpModeIdle = (DSP_GetProfState() == DSP_PROF_STATUS_IDLE)? 1U: 0U;
    PureStillProcessed = HL_PureStillProcessed();

    /* booting or from idle */
    if ((DspOpModeInit == 1U) || (DspOpModeIdle == 1U)) {
        /* SystemSetup */
        Rval = HL_LiveivewSystemSetupImpl(WriteMode);

        /* Debug Level */
        HL_LiveviewDspDebugLevelImpl(WriteMode);

        /* Max. ResourceSetup */
        HL_LiveviewResourceSetupImpl(WriteMode);

        /* Profile Activate */
        HL_LiveviewSystemActivateImpl(WriteMode);

        /* FlowProcessor Binding */
        HL_LiveviewFpBindingImpl(WriteMode);
    }
    if (((DspOpModeInit == 1U) || (DspOpModeIdle == 1U)) ||
        (PureStillProcessed == 1U)) {
        /* Vproc limit */
        HL_LiveviewVprocConfigImpl(WriteMode);
    }

    /* Vproc relative */
    if ((Rval == OK) &&
        (IsPartialEnable == 0U)) {
        Rval = HL_LiveviewVprocSetupImpl(WriteMode);
    }

    /* Vin Relative */
    if (Rval == OK) {
        Rval = HL_LiveviewVinSetupImpl(WriteMode);
    }

    /* Vout Relative */
    if (Rval == OK) {
        VOutTiming = (DspOpModeInit == 1U)? VOUT_CFG_TIME_PRE_VIN: VOUT_CFG_TIME_PRE_VIN_PROF_SWITCH;
        HL_LiveviewVoutSetupImpl(WriteMode, VOutTiming);
    }

    return Rval;
}

UINT32 HL_LiveviewCmdPrepare(const UINT8 WriteMode,
                             const UINT8 IsPartialEnable)
{
    UINT32 Rval;

#ifdef DEBUG_LIVEVIEW_FLOW
    HL_LiveviewFillVinInfo();
#endif
    HL_LiveviewVinCompressionSetup();
#ifdef DEBUG_LIVEVIEW_FLOW
    HL_LiveviewFlipModeSetup();
#endif

#ifdef SUPPORT_INTERNAL_CHECK
    /* PixelClock bonding Check */
    Rval = HL_LiveviewPixelClockCheckImpl();
    if (Rval == OK) {
        Rval = HL_LiveviewFillPreProcGroupCmds(WriteMode);
    }
#else
    Rval = HL_LiveviewFillPreProcGroupCmds(WriteMode, IsPartialEnable);
#endif

    return Rval;
}

static UINT32 HL_LiveviewDecIsoCfgImpl(const UINT16 DecIdx,
                                       const UINT32 CfgIndex,
                                       const ULONG CfgAddress)
{
    UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT16 ViewZoneId;
    UINT32 Rval;
    CTX_VID_DEC_INFO_s VidDecInfo = {0};

#ifdef SUPPORT_DSP_DEC_BATCH
    UINT32 *pBatchQAddr = NULL, BatchCmdId = 0U, NewWp = 0U;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    cmd_decoder_batch_t *DecBchInfo = HL_DefCtxCmdBufPtrDecBch;
    cmd_vproc_ik_config_t *pIsoCfgCmd = NULL;
#else
    cmd_vproc_ik_config_t *VprocIkCfg = HL_DefCtxCmdBufPtrVpcIkCfg;
#endif

    HL_GetVidDecInfo(HL_MTX_OPT_ALL, DecIdx, &VidDecInfo);
    ViewZoneId = VidDecInfo.ViewZoneId;

#ifdef SUPPORT_DSP_DEC_BATCH
    HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
    /* Request BatchCmdQ buffer */
    Rval = DSP_ReqBuf(&ViewZoneInfo->BatchQPoolDesc, 1U, &NewWp, 1U/*FullnessCheck*/);
    if (Rval != OK) {
        HL_GetViewZoneInfoUnLock(ViewZoneId);
    } else {
        ULONG BatchQAddr = 0U;

        ViewZoneInfo->BatchQPoolDesc.Wp = NewWp;
        HL_GetViewZoneInfoUnLock(ViewZoneId);
        HL_GetPointerToDspBatchQ(ViewZoneId,
                                 (UINT16)ViewZoneInfo->BatchQPoolDesc.Wp,
                                 &pBatchQAddr,
                                 &BatchCmdId);
        /* Reset New BatchQ after Wp advanced */
        HL_ResetDspBatchQ(pBatchQAddr);

        dsp_osal_typecast(&pIsoCfgCmd, &pBatchQAddr);
        (void)HL_FillIsoCfgUpdate(ViewZoneId, pIsoCfgCmd);
        (void)dsp_osal_virt2cli(CfgAddress, &pIsoCfgCmd->idsp_flow_addr);
        pIsoCfgCmd->ik_cfg_id = CfgIndex;

        HL_SetDspBatchQInfo(BatchCmdId, 1U/*AutoReset*/, BATCHQ_INFO_ISO_CFG, CfgIndex);

        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&DecBchInfo, &CmdBufferAddr);
        DecBchInfo->batch_cmd_set_info.id = BatchCmdId;

        dsp_osal_typecast(&BatchQAddr, &pBatchQAddr);
        (void)dsp_osal_virt2cli(BatchQAddr, &DecBchInfo->batch_cmd_set_info.addr);
        DecBchInfo->batch_cmd_set_info.size = HL_GetBatchCmdNumber(BatchQAddr)*CMD_SIZE_IN_BYTE;
        DecBchInfo->hdr.decoder_id = (UINT8)DecIdx;
        DecBchInfo->hdr.codec_type = HL_DecCodec2DspDecCodec[VidDecInfo.BitsFormat];
        Rval = AmbaHL_CmdDecoderBatch(WriteMode, DecBchInfo);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[LiveviewUpdateIsoCfg] AmbaHL_CmdDecoderBatch fail %d",
                    Rval, 0U, 0U, 0U, 0U);
        }
        HL_RelCmdBuffer(CmdBufferId);
    }
#else
    //Using CMD_VPROC_IK_CONFIG when bring-up
    AmbaLL_LogUInt5("[DEC] Force using CMD_VPROC_IK_CONFIG Vz:%u", ViewZoneId, 0U, 0U, 0U, 0U);

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&VprocIkCfg, &CmdBufferAddr);
    (void)HL_FillIsoCfgUpdate(ViewZoneId, VprocIkCfg);
    (void)dsp_osal_virt2cli(CfgAddress, &(VprocIkCfg->idsp_flow_addr));
    VprocIkCfg->ik_cfg_id = CfgIndex;
    Rval = AmbaHL_CmdVprocIkConfig(WriteMode, VprocIkCfg);
    HL_RelCmdBuffer(CmdBufferId);
    if (Rval != OK) {
        AmbaLL_LogUInt5("CMD_VPROC_IK_CONFIG fail[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }
#endif

    return Rval;
}

static UINT32 HL_LiveviewDec2VprocBind2VirtVin(const UINT16 DecIdx, CTX_VID_DEC_INFO_s *pVidDecInfo)
{
    UINT16 ViewZoneId = pVidDecInfo->ViewZoneId;
    UINT16 YuvInVirtVinId = pVidDecInfo->YuvInVirtVinId + AMBA_DSP_MAX_VIN_NUM;
    UINT32 Rval;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_binding_cfg_t *FpBindCfg = HL_DefCtxCmdBufPtrBind;

    Rval = DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP0_EVENT, 1, WAIT_INTERRUPT_TIMEOUT);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[HL_LiveviewDec2VprocIsoCfgPauseImpl] %d Wait VDSP0 fail %d", __LINE__, Rval, 0U, 0U, 0U);
    }
    /* 0.UnBind decoder first */
    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&FpBindCfg, &CmdBufferAddr);
    Rval = HL_FillFpBindSetup(FpBindCfg, BIND_VIDEO_DECODE_TO_VPROC, 0U/*IsBind*/,
                              DecIdx/*SrcId -> src_ch_id*/,
                              0U/*SrcPinId -> xx*/,
                              ViewZoneId/*StrmId -> dst_ch_id*/);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[HL_LiveviewDec2VprocIsoCfgPauseImpl] %d FpBind filling fail", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        (void)AmbaHL_CmdDspBindingCfg(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, FpBindCfg);
    }
    HL_RelCmdBuffer(CmdBufferId);

    Rval = DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP0_EVENT, 2, WAIT_INTERRUPT_TIMEOUT);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[HL_LiveviewDec2VprocIsoCfgPauseImpl] %d FpBind DSP_WaitVdspEvent fail", __LINE__, 0U, 0U, 0U, 0U);
    }

    /* 1.Bind VirtVin */
    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&FpBindCfg, &CmdBufferAddr);
    Rval = HL_FillFpBindSetup(FpBindCfg, BIND_STILL_PROC, 1U/*IsBind*/,
                              YuvInVirtVinId/*SrcId -> src_fp_id*/,
                              0U/*SrcPinId*/,
                              ViewZoneId/*StrmId -> dst_ch_id*/);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[HL_LiveviewDec2VprocIsoCfgPauseImpl] %d FpBind filling fail", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        (void)AmbaHL_CmdDspBindingCfg(AMBA_DSP_CMD_VCAP_NORMAL_WRITE, FpBindCfg);
        HL_GetVidDecInfo(HL_MTX_OPT_GET, DecIdx, pVidDecInfo);
        pVidDecInfo->CurrentBind = BIND_STILL_PROC;
        HL_SetVidDecInfo(HL_MTX_OPT_SET, DecIdx, pVidDecInfo);
    }
    HL_RelCmdBuffer(CmdBufferId);
    Rval = DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP0_EVENT, 2, WAIT_INTERRUPT_TIMEOUT);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[HL_LiveviewDec2VprocIsoCfgPauseImpl] %d FpBind DSP_WaitVdspEvent fail", __LINE__, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 HL_LiveviewVirtVinIsoCfgImpl(const UINT16 DecIdx,
                                           CTX_VID_DEC_INFO_s *pVidDecInfo,
                                           const UINT32 CfgIndex,
                                           const ULONG CfgAddress)
{
    UINT8 IsFirstMem;
    UINT16 ViewZoneId = pVidDecInfo->ViewZoneId;
    UINT16 YuvInVirtVinId = pVidDecInfo->YuvInVirtVinId + AMBA_DSP_MAX_VIN_NUM;
    UINT32 NewWp = 0U, Rval;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    AMBA_DSP_BUF_s LastYuvBuf = {0};
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    cmd_vin_set_ext_mem_t *VinExtMem = HL_DefCtxCmdBufPtrVinExtMem;
    cmd_vin_send_input_data_t *FeedData = HL_DefCtxCmdBufPtrVinSndData;

    /* 1.Feed Last yuv */
    HL_GetVidDecInfo(HL_MTX_OPT_GET, DecIdx, pVidDecInfo);

    /* First time use vin_ext_mem, use allocation_mode=VIN_EXT_MEM_MODE_NEW
     * Other feed use VIN_EXT_MEM_MODE_APPEND */
    if (pVidDecInfo->YuvFrameCnt > 0U) {
        IsFirstMem = 0U;
    } else {
        IsFirstMem = 1U;
    }

    if (pVidDecInfo->YuvFrameCnt > 0U) {
        /* Move ExtYuvBufIdx forward,
         * ucode memory map idx maximum is: EXT_MEM_VIN_MAX_NUM,
         * If you always use allocation_mode=VIN_EXT_MEM_MODE_NEW,
         * You don't need to update cmd_vin_send_input_data_t.ext_fb_idx
         */
        pVidDecInfo->ExtYuvBufIdx = (UINT16)((pVidDecInfo->ExtYuvBufIdx + 1U) % EXT_MEM_VIN_MAX_NUM);
    } else {
        /* use idx=0 for first time virtual vin */
    }
    /* This is Yuv to Vproc case, not to encoder */
    /* pVidDecInfo->LastYuvBuf is updated by AmbaDSP_Event.c: VideoDecodePicPreProcFunc */
    LastYuvBuf.BaseAddr = pVidDecInfo->LastYuvBuf.BaseAddrY;
    LastYuvBuf.Pitch = pVidDecInfo->LastYuvBuf.Pitch;
    LastYuvBuf.Window.OffsetX = pVidDecInfo->LastYuvBuf.Window.OffsetX;
    LastYuvBuf.Window.OffsetY = pVidDecInfo->LastYuvBuf.Window.OffsetY;
    LastYuvBuf.Window.Width   = pVidDecInfo->LastYuvBuf.Window.Width;
    LastYuvBuf.Window.Height  = pVidDecInfo->LastYuvBuf.Window.Height;
    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&VinExtMem, &CmdBufferAddr);
    (void)HL_LiveviewFillVinExtMem(YuvInVirtVinId,
                                   0U /* IsYuvVin2Enc */,
                                   IsFirstMem,
                                   DSP_VIN_EXT_MEM_TYPE_YUV422 /* MemType */,
                                   YuvFmtTable[pVidDecInfo->LastYuvBuf.DataFmt] /* ChromaFmt */,
                                   VIN_EXT_MEM_CTRL_WAIT_APPEND /* OverFlowCtrl */,
                                   &LastYuvBuf,
                                   NULL,
                                   VinExtMem);
    /* fill correct buffer w/h */
    VinExtMem->buf_width = (UINT16)pVidDecInfo->LastYuvBufWidth;
    VinExtMem->buf_height = (UINT16)pVidDecInfo->LastYuvBufHeight;
    /* we suggest user to have 16align on Luma buffer height */
    VinExtMem->buf_height = ALIGN_NUM16(VinExtMem->buf_height, 16U);
    (void)AmbaHL_CmdVinSetExtMem(AMBA_DSP_CMD_VCAP_NORMAL_WRITE, VinExtMem);
    HL_RelCmdBuffer(CmdBufferId);

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&FeedData, &CmdBufferAddr);
    FeedData->vin_id = (UINT8)YuvInVirtVinId;
    FeedData->chan_id = 0U; //obsoleted
    FeedData->send_data_dest = DSP_VIN_SEND_IN_DATA_DEST_VPROC;
    FeedData->is_compression_en = 0U;
    FeedData->blk_sz = 0U;
    FeedData->mantissa = 0U;
    FeedData->raw_frm_cap_cnt = pVidDecInfo->YuvFrameCnt;
    FeedData->input_data_type = (pVidDecInfo->LastYuvBuf.DataFmt == AMBA_DSP_YUV420)? (UINT32)DSP_VIN_SEND_IN_DATA_YUV420: (UINT32)DSP_VIN_SEND_IN_DATA_YUV422;
    FeedData->ext_fb_idx = DSP_FB_IDX_FOR_NEW2APPEND;

    /* 3.prepare iso cfg */
    HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
    /* Request BatchCmdQ buffer */
    Rval = DSP_ReqBuf(&ViewZoneInfo->BatchQPoolDesc, 1U, &NewWp, 1U/*FullnessCheck*/);
    if (Rval != OK) {
        HL_GetViewZoneInfoUnLock(ViewZoneId);
        AmbaLL_LogUInt5("[HL_LiveviewDec2VprocIsoCfgPauseImpl] %d, ViewZone(%d) batch pool is full",
                __LINE__, ViewZoneId, 0U, 0U, 0U);
    } else {
        UINT32 *pBatchQAddr, BatchCmdId;
        cmd_vproc_ik_config_t *pIsoCfgCmd;
        ULONG BatchQAddr = 0U;
        ViewZoneInfo->BatchQPoolDesc.Wp = NewWp;
        HL_GetViewZoneInfoUnLock(ViewZoneId);
        HL_GetPointerToDspBatchQ(ViewZoneId, (UINT16)ViewZoneInfo->BatchQPoolDesc.Wp, &pBatchQAddr, &BatchCmdId);

        /* Reset New BatchQ after Wp advanced */
        HL_ResetDspBatchQ(pBatchQAddr);

        /* IsoCfgCmd occupied first CmdSlot */
        dsp_osal_typecast(&pIsoCfgCmd, &pBatchQAddr);
        Rval = HL_FillIsoCfgUpdate(ViewZoneId, pIsoCfgCmd);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[HL_LiveviewDec2VprocIsoCfgPauseImpl] %d, HL_FillIsoCfgUpdate fail %d",
                    __LINE__, Rval, 0U, 0U, 0U);
        } else {
            (void)dsp_osal_virt2cli(CfgAddress, &pIsoCfgCmd->idsp_flow_addr);
            pIsoCfgCmd->ik_cfg_id = CfgIndex;
            HL_SetDspBatchQInfo(BatchCmdId, 1U/*AutoReset*/, BATCHQ_INFO_ISO_CFG, CfgIndex);
            FeedData->batch_cmd_set_info.id = BatchCmdId;
            dsp_osal_typecast(&BatchQAddr, &pBatchQAddr);
            (void)dsp_osal_virt2cli(BatchQAddr, &FeedData->batch_cmd_set_info.addr);
            FeedData->batch_cmd_set_info.size = HL_GetBatchCmdNumber(BatchQAddr)*CMD_SIZE_IN_BYTE;
        }
    }

    (void)AmbaHL_CmdVinSendInputData(AMBA_DSP_CMD_VCAP_NORMAL_WRITE, FeedData);
    HL_RelCmdBuffer(CmdBufferId);
    pVidDecInfo->YuvFrameCnt++;
    HL_SetVidDecInfo(HL_MTX_OPT_SET, DecIdx, pVidDecInfo);
    Rval = DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP0_EVENT, 5U, WAIT_INTERRUPT_TIMEOUT);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[HL_LiveviewDec2VprocIsoCfgPauseImpl] %d ExtMem/SendInp DSP_WaitVdspEvent fail",
                __LINE__, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

#ifdef SUPPORT_INTERNAL_CHECK
UINT32 HL_LiveviewPixelClockCheckImpl(void)
{
    UINT32 Rval;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_VIN_INFO_s VinInfo = {0};
    UINT16 i;
    UINT16 VinId, Width, Height;
    UINT32 FrameRate, Pixel;
    UINT64 PixelClk = 0U;

    HL_GetResourcePtr(&Resource);

    for (i = 0U; i < Resource->ViewZoneNum; i++) {
        HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
        Width = ViewZoneInfo->Main.Width;
        Height = ViewZoneInfo->Main.Height;

        (void)HL_GetViewZoneVinId(i, &VinId);
        if (VinId == VIN_IDX_INVALID) {
//FIXME, source is memory
            FrameRate = 30U;
        } else {
            HL_GetVinInfo(HL_MTX_OPT_ALL, VinId, &VinInfo);

            if (ViewZoneInfo->VinDecimationRate > 1U) {
                FrameRate = (VinInfo.FrameRate.TimeScale/(UINT32)ViewZoneInfo->VinDecimationRate)/1000U;
            } else {
                FrameRate = (VinInfo.FrameRate.TimeScale)/1000U;
            }
        }

        Pixel = (UINT32)Width*Height;
        PixelClk += (((UINT64)FrameRate*Pixel)/(UINT32)1000000U);
    }
    Rval = HL_Int00Check((UINT32)PixelClk);
    return Rval;
}
#endif

/* The flow of reset liveview */
static void HL_LiveviewReset(void)
{
    /* stl vproc here? */
    {
        CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

        HL_GetResourcePtr(&Resource);
        if ((Resource->MaxStlMainWidth > 0U) && (Resource->MaxStlMainHeight > 0U)) {
            UINT8 VprocId;
            CTX_STILL_INFO_s StlInfo = {0};

            HL_GetStlInfo(HL_MTX_OPT_ALL, &StlInfo);
            VprocId = (UINT8)StlInfo.YuvInVprocId;
            if (VprocId < AMBA_DSP_MAX_VIEWZONE_NUM) {
                if (DSP_VPROC_STATUS_ACTIVE == DSP_GetVprocState(VprocId)) {
                    UINT8 WriteMode;
                    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
                    cmd_vproc_stop_t *VprocStop = HL_DefCtxCmdBufPtrVpcStop;

                    WriteMode = HL_GetVin2CmdNormalWrite(StlInfo.YuvInVirtVinId + AMBA_DSP_MAX_VIN_NUM);
                    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                    dsp_osal_typecast(&VprocStop, &CmdBufferAddr);
                    VprocStop->channel_id = VprocId;
                    VprocStop->stop_cap_seq_no = 0U; //obsoleted
                    VprocStop->reset_option = 0U; //obsoleted
                    (void)AmbaHL_CmdVprocStop(WriteMode, VprocStop);
                    HL_RelCmdBuffer(CmdBufferId);
                    (void)DSP_WaitVprocState(VprocId, DSP_VPROC_STATUS_TIMER, MODE_SWITCH_TIMEOUT, 1U);
                } else {
                    // DO NOTHING
                }
            } else {
                // DO NOTHING
            }
        }
    }
}

static inline void HL_LvCfgOnCheckViewZoneChkSourceVin(const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s *pViewZoneCfg,
                                                       const CTX_VIEWZONE_INFO_s *pViewZoneInfo,
                                                       UINT32 *pVinTotalBit,
                                                       UINT8 *pLvcfgChg,
                                                       UINT32 *pChangedLine)
{
    UINT8 SourceTdIdx;
    UINT16 SourceVin, ActualVinId;
    UINT32 VinBit = 0U;

    if (1U == DSP_GetU16Bit(pViewZoneCfg->VinId, VIN_VIRT_IDX, 1U)) {
        ActualVinId = (UINT16)DSP_GetU16Bit(pViewZoneCfg->VinId, 0U, 15U);
        ActualVinId += AMBA_DSP_MAX_VIN_NUM;
    } else {
        ActualVinId = (UINT16)DSP_GetU16Bit(pViewZoneCfg->VinId, 0U, 15U);
    }
    DSP_Idx2Bit((UINT8)ActualVinId, &VinBit);
    if (pViewZoneCfg->VinSource != DSP_LV_SOURCE_MEM_DEC) {
        SourceVin = (UINT16)VinBit;
        if (pViewZoneCfg->VinSource != DSP_LV_SOURCE_MEM_RECON) {
            *pVinTotalBit |= VinBit;
        }
    } else {
        SourceVin = 0U;
    }
    if (SourceVin != pViewZoneInfo->SourceVin) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }

    if (1U == DSP_GetU8Bit(pViewZoneCfg->IsVirtChan, VIEWZONE_VIN_TD_BIT_IDX, 1U)) {
        SourceTdIdx = (UINT8)DSP_GetU16Bit(pViewZoneCfg->VinVirtChId, 8U, 8U);
    } else {
        SourceTdIdx = VIEWZONE_SRC_TD_IDX_NULL;
    }
    if (SourceTdIdx != pViewZoneInfo->SourceTdIdx) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }

    if ((UINT8)pViewZoneCfg->VinSource != pViewZoneInfo->InputFromMemory) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }
}

static inline void HL_LvCfgOnCheckViewZoneChkFlowOpt(const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s *pViewZoneCfg,
                                                     const CTX_VIEWZONE_INFO_s *pViewZoneInfo,
                                                     UINT8 *pLvcfgChg,
                                                     UINT32 *pChangedLine)
{
    UINT32 FlowBypassOption = 0U;
    UINT8 MctfDisable = (UINT8)DSP_GetU8Bit(pViewZoneCfg->MctfDisable, DSP_MCTF_OFF_IDX, DSP_MCTF_OFF_LEN);
    UINT8 MctfCmpr = (UINT8)DSP_GetU8Bit(pViewZoneCfg->MctfDisable, DSP_MCTF_CMPR_IDX, DSP_MCTF_CMPR_LEN);
    UINT8 MctsDisable = (UINT8)DSP_GetU8Bit(pViewZoneCfg->MctsDisable, DSP_MCTS_OFF_IDX, DSP_MCTS_OFF_LEN);
    UINT8 MctsDramOutDisable = (UINT8)DSP_GetU8Bit(pViewZoneCfg->MctsDisable, DSP_MCTS_DRAM_OUT_IDX, DSP_MCTS_DRAM_OUT_LEN);

    if (MctfDisable > 0U) {
        DSP_SetBit(&FlowBypassOption, FLOW_BYPASS_MCTF_IDX);
    } else {
        DSP_ClearBit(&FlowBypassOption, FLOW_BYPASS_MCTF_IDX);
    }
    if (MctsDisable > 0U) {
        DSP_SetBit(&FlowBypassOption, FLOW_BYPASS_MCTS_IDX);
    } else {
        DSP_ClearBit(&FlowBypassOption, FLOW_BYPASS_MCTS_IDX);
    }
    if (MctsDramOutDisable > 0U) {
        DSP_SetBit(&FlowBypassOption, FLOW_BYPASS_MCTS_DRAM_OUT_IDX);
    } else {
        DSP_ClearBit(&FlowBypassOption, FLOW_BYPASS_MCTS_DRAM_OUT_IDX);
    }

    if (FlowBypassOption != pViewZoneInfo->FlowBypassOption) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    } else if (MctfCmpr != pViewZoneInfo->MctfCmpr) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    } else {
        // DO NOTHING
    }
}

static inline void HL_LvCfgOnCheckViewZoneChkHdr(const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s *pViewZoneCfg,
                                                       const CTX_VIEWZONE_INFO_s *pViewZoneInfo,
                                                       UINT8 *pLvcfgChg,
                                                       UINT32 *pChangedLine)
{
    UINT8 HdrBlendNumMinusOne;

    if (pViewZoneCfg->LinearCE != pViewZoneInfo->LinearCe) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }

    HdrBlendNumMinusOne = (pViewZoneCfg->HdrExposureNum == 0U)? 0U: (pViewZoneCfg->HdrExposureNum-1U);
    if (HdrBlendNumMinusOne != pViewZoneInfo->HdrBlendNumMinusOne) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }

    if (pViewZoneCfg->HdrBlendHieght != pViewZoneInfo->HdrBlendHeight) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }
}

static inline void HL_LvCfgOnCheckViewZoneChkWindow(const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s *pViewZoneCfg,
                                                    const CTX_VIEWZONE_INFO_s *pViewZoneInfo,
                                                    UINT8 *pLvcfgChg,
                                                    UINT32 *pChangedLine)
{
    UINT32 CmpVal = 0U;

    (void)dsp_osal_memcmp(&pViewZoneCfg->VinRoi,
                          &pViewZoneInfo->CapWindow,
                          sizeof(AMBA_DSP_WINDOW_s),
                          &CmpVal);
    if (CmpVal != 0U) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }

    if (pViewZoneCfg->MainWidth != pViewZoneInfo->PinWindow[DSP_VPROC_PIN_MAIN].Width) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }
    if (pViewZoneCfg->MainHeight != pViewZoneInfo->PinWindow[DSP_VPROC_PIN_MAIN].Height) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }

    if (pViewZoneCfg->MainWidth != pViewZoneInfo->Main.Width) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }

    if (pViewZoneCfg->MainHeight != pViewZoneInfo->Main.Height) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }
}

static inline void HL_LvCfgOnCheckViewZoneChkVd(const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s *pViewZoneCfg,
                                                const CTX_VIEWZONE_INFO_s *pViewZoneInfo,
                                                UINT8 *pLvcfgChg,
                                                UINT32 *pChangedLine)
{
    UINT8 VinDecimationRate;
    UINT8 IsProcRawDramOut;
    UINT8 IsInputMuxSel;
    UINT8 InputMuxSelSrc = (UINT8)DSP_GetBit(pViewZoneCfg->InputMuxSelCfg, VZ_IN_MUXSEL_SRC_BIT_IDX, VZ_IN_MUXSEL_SRC_LEN);
    UINT16 InputMuxSelHorDeci = (UINT16)DSP_GetBit(pViewZoneCfg->InputMuxSelCfg, VZ_IN_MUXSEL_HOR_DEC_BIT_IDX, VZ_IN_MUXSEL_DEC_LEN);
    UINT16 InputMuxSelVerDeci = (UINT16)DSP_GetBit(pViewZoneCfg->InputMuxSelCfg, VZ_IN_MUXSEL_VER_DEC_BIT_IDX, VZ_IN_MUXSEL_DEC_LEN);

    if (1U == DSP_GetU8Bit(pViewZoneCfg->IsVirtChan, VIEWZONE_VIN_DECI_BIT_IDX, 1U)) {
        VinDecimationRate = pViewZoneCfg->VinDecimation;
    } else {
        VinDecimationRate = (UINT8)1U;
    }
    if (VinDecimationRate != pViewZoneInfo->VinDecimationRate) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }

    if (pViewZoneCfg->DramUtilityPipe != pViewZoneInfo->Pipe) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }

    IsInputMuxSel = (UINT8)DSP_GetU8Bit(pViewZoneCfg->IsVirtChan, VIEWZONE_INPUT_MUXSEL_BIT_IDX, 1U);

    if (IsInputMuxSel != pViewZoneInfo->InputMuxSelSrc) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    } else {
        if ((IsInputMuxSel == (UINT8)1U) &&
            ((InputMuxSelSrc != pViewZoneInfo->InputMuxSelSrc) ||
             (InputMuxSelHorDeci != pViewZoneInfo->InputMuxSelHorDeci) ||
             (InputMuxSelVerDeci != pViewZoneInfo->InputMuxSelVerDeci))) {
            *pLvcfgChg += 1U;
            *pChangedLine = __LINE__;
        }
    }

    if (pViewZoneCfg->RotateFlip != pViewZoneInfo->MainRotate) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }

    IsProcRawDramOut = (UINT8)DSP_GetU8Bit(pViewZoneCfg->IsVirtChan, VIEWZONE_PROC_RAW_BIT_IDX, 1U);
    if (IsProcRawDramOut != pViewZoneInfo->IsProcRawDramOut) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }
}

static inline void HL_LvCfgOnCheckViewZoneChkPymd(const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s *pViewZoneCfg,
                                                  const CTX_VIEWZONE_INFO_s *pViewZoneInfo,
                                                  UINT8 *pLvcfgChg,
                                                  UINT32 *pChangedLine)
{
    UINT32 CmpVal = 0U;

    (void)dsp_osal_memcmp(&pViewZoneCfg->Pyramid,
                          &pViewZoneInfo->Pyramid,
                          sizeof(AMBA_DSP_LIVEVIEW_PYRAMID_s),
                          &CmpVal);
    if (CmpVal != 0U) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }

    if (pViewZoneCfg->pPyramidBuf != NULL) {
        UINT8   PymdIsExtMem;
        AMBA_DSP_YUV_IMG_BUF_s PymdYuvBuf;

        PymdIsExtMem = (pViewZoneCfg->pPyramidBuf->AllocType == ALLOC_INTERNAL)? 0U: 1U;
        if (PymdIsExtMem != pViewZoneInfo->PymdIsExtMem) {
            *pLvcfgChg += 1U;
            *pChangedLine = __LINE__;
        }
        if (pViewZoneCfg->pPyramidBuf->AllocType != pViewZoneInfo->PymdAllocType) {
            *pLvcfgChg += 1U;
            *pChangedLine = __LINE__;
        }
        if (pViewZoneCfg->pPyramidBuf->BufNum != pViewZoneInfo->PymdBufNum) {
            *pLvcfgChg += 1U;
            *pChangedLine = __LINE__;
        }
        (void)dsp_osal_memcpy(&PymdYuvBuf, &pViewZoneCfg->pPyramidBuf->YuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
        (void)dsp_osal_memcmp(&PymdYuvBuf , &pViewZoneInfo->PymdYuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s), &CmpVal);
        if (CmpVal != 0U) {
            *pLvcfgChg += 1U;
            *pChangedLine = __LINE__;
        }

        if (pViewZoneCfg->pPyramidBuf->pYuvBufTbl != NULL) {
            if (pViewZoneCfg->pPyramidBuf->pYuvBufTbl[0U] != pViewZoneInfo->PymdYuvBufTbl[0U]) {
                *pLvcfgChg += 1U;
                *pChangedLine = __LINE__;
            }
        }
    }
}

static inline void HL_LvCfgOnCheckViewZoneChkLnd(const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s *pViewZoneCfg,
                                                 const CTX_VIEWZONE_INFO_s *pViewZoneInfo,
                                                 UINT8 *pLvcfgChg,
                                                 UINT32 *pChangedLine)
{
    UINT32 CmpVal = 0U;

    (void)dsp_osal_memcmp(&pViewZoneCfg->LaneDetect,
                          &pViewZoneInfo->LaneDetect,
                          sizeof(AMBA_DSP_LIVEVIEW_LANE_DETECT_s),
                          &CmpVal);
    if (CmpVal != 0U) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }

    if (pViewZoneCfg->pLaneDetectBuf != NULL) {
        UINT8   LndtIsExtMem;
        AMBA_DSP_YUV_IMG_BUF_s LndtYuvBuf;

        LndtIsExtMem = (pViewZoneCfg->pLaneDetectBuf->AllocType == ALLOC_INTERNAL)? 0U: 1U;
        if (LndtIsExtMem != pViewZoneInfo->LndtIsExtMem) {
            *pLvcfgChg += 1U;
            *pChangedLine = __LINE__;
        }
        if (pViewZoneCfg->pLaneDetectBuf->AllocType != pViewZoneInfo->LndtAllocType) {
            *pLvcfgChg += 1U;
            *pChangedLine = __LINE__;
        }
        if (pViewZoneCfg->pLaneDetectBuf->BufNum != pViewZoneInfo->LndtBufNum) {
            *pLvcfgChg += 1U;
            *pChangedLine = __LINE__;
        }
        (void)dsp_osal_memcpy(&LndtYuvBuf, &pViewZoneCfg->pLaneDetectBuf->YuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
        (void)dsp_osal_memcmp(&LndtYuvBuf , &pViewZoneInfo->LndtYuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s), &CmpVal);
        if (CmpVal != 0U) {
            *pLvcfgChg += 1U;
            *pChangedLine = __LINE__;
        }

        if (pViewZoneCfg->pLaneDetectBuf->pYuvBufTbl != NULL) {
            if (pViewZoneCfg->pLaneDetectBuf->pYuvBufTbl[0U] != pViewZoneInfo->LndtYuvBufTbl[0U]) {
                *pLvcfgChg += 1U;
                *pChangedLine = __LINE__;
            }
        }
    }
}

static inline void HL_LvCfgOnCheckViewZoneChkMainY12(const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s *pViewZoneCfg,
                                                     const CTX_VIEWZONE_INFO_s *pViewZoneInfo,
                                                     UINT8 *pLvcfgChg,
                                                     UINT32 *pChangedLine)
{
    UINT32 CmpVal = 0U;

    if (pViewZoneCfg->pMainY12Buf != NULL) {
        UINT8 MainY12IsExtMem;
        AMBA_DSP_YUV_IMG_BUF_s MainY12Buf;

        if (pViewZoneInfo->MainY12Enable == 0U) {
            *pLvcfgChg += 1U;
            *pChangedLine = __LINE__;
        }

        MainY12IsExtMem = (pViewZoneCfg->pMainY12Buf->AllocType == ALLOC_INTERNAL)? 0U: 1U;
        if (MainY12IsExtMem != pViewZoneInfo->MainY12IsExtMem) {
            *pLvcfgChg += 1U;
            *pChangedLine = __LINE__;
        }
        if (pViewZoneCfg->pMainY12Buf->AllocType != pViewZoneInfo->MainY12AllocType) {
            *pLvcfgChg += 1U;
            *pChangedLine = __LINE__;
        }
        if (pViewZoneCfg->pMainY12Buf->BufNum != pViewZoneInfo->MainY12BufNum) {
            *pLvcfgChg += 1U;
            *pChangedLine = __LINE__;
        }
        (void)dsp_osal_memcpy(&MainY12Buf, &pViewZoneCfg->pMainY12Buf->YuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
        (void)dsp_osal_memcmp(&MainY12Buf , &pViewZoneInfo->MainY12Buf, sizeof(AMBA_DSP_YUV_IMG_BUF_s), &CmpVal);
        if (CmpVal != 0U) {
            *pLvcfgChg += 1U;
            *pChangedLine = __LINE__;
        }
        if (pViewZoneCfg->pMainY12Buf->pYuvBufTbl != NULL) {
            if (pViewZoneCfg->pMainY12Buf->pYuvBufTbl[0U] != pViewZoneInfo->MainY12BufTbl[0U]) {
                *pLvcfgChg += 1U;
                *pChangedLine = __LINE__;
            }
        }
    } else {
        if (pViewZoneInfo->MainY12Enable == 1U) {
            *pLvcfgChg += 1U;
            *pChangedLine = __LINE__;
        }
    }
}

/* Translate and check ViewZone information
 *     CTX_RESOURCE_INFO_s
 *          SourceVin
 *          SourceTdIdx
 *          InputFromMemory
 *          FlowBypassOption
 *          LinearCe
 *          HdrBlendNumMinusOne
 *          HdrBlendHeight
 *          CapWindow
 *          PinWindow[DSP_VPROC_PIN_NUM]
 *          Main
 *          VinDecimationRate
 *          MainRotate
 *          Pyramid;
 *          PymdBufferStatus
 *          PymdIsExtMem
 *          PymdAllocType
 *          PymdBufNum
 *          PymdYuvBuf
 *          *pPymdYuvBufTbl
 *          LaneDetect;
 *          LndtBufferStatus;
 *          LndtIsExtMem;
 *          LndtAllocType;
 *          LndtBufNum;
 *          LndtYuvBuf;
 *          pLndtYuvBufTbl
 *          MainY12BufferStatus;
 *          MainY12IsExtMem;
 *          MainY12AllocType;
 *          MainY12BufNum;
 *          MainY12YuvBuf;
 *          pMainY12BufTbl
 *          Pipe
 *          IsEffectChanMember
 * */
static inline void HL_LvCfgOnCheckViewZoneChg(UINT16 NumViewZone,
                                              const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s *pViewZoneCfg,
                                              UINT32 *pVinTotalBit,
                                              UINT32 *ViewZoneTotalBit,
                                              UINT8 *pLvcfgChg,
                                              UINT32 *pChangedLine)
{
    UINT16 i, ViewZoneId;
    UINT32 ViewZoneBit = 0U;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    for (i=0U; i<NumViewZone; i++) {
        const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s *pViewZoneCfgEach = &pViewZoneCfg[i];

        ViewZoneId = pViewZoneCfgEach->ViewZoneId;
        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
        DSP_Idx2Bit((UINT8)ViewZoneId, &ViewZoneBit);
        *ViewZoneTotalBit |= ViewZoneBit;

        HL_LvCfgOnCheckViewZoneChkSourceVin(pViewZoneCfgEach, ViewZoneInfo,
                                            pVinTotalBit, pLvcfgChg, pChangedLine);

        HL_LvCfgOnCheckViewZoneChkFlowOpt(pViewZoneCfgEach, ViewZoneInfo,
                                          pLvcfgChg, pChangedLine);

        HL_LvCfgOnCheckViewZoneChkHdr(pViewZoneCfgEach, ViewZoneInfo,
                                      pLvcfgChg, pChangedLine);

        HL_LvCfgOnCheckViewZoneChkWindow(pViewZoneCfgEach, ViewZoneInfo,
                                         pLvcfgChg, pChangedLine);

        HL_LvCfgOnCheckViewZoneChkVd(pViewZoneCfgEach, ViewZoneInfo,
                                     pLvcfgChg, pChangedLine);

        HL_LvCfgOnCheckViewZoneChkPymd(pViewZoneCfgEach, ViewZoneInfo,
                                       pLvcfgChg, pChangedLine);

        HL_LvCfgOnCheckViewZoneChkLnd(pViewZoneCfgEach, ViewZoneInfo,
                                       pLvcfgChg, pChangedLine);

        HL_LvCfgOnCheckViewZoneChkMainY12(pViewZoneCfgEach, ViewZoneInfo,
                                          pLvcfgChg, pChangedLine);
    }
}

static inline void HL_LvCfgOnCheckYuvStrmChkPurpose(const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYUVStrmCfg,
                                                    const CTX_YUV_STRM_INFO_s *pYuvStrmInfo,
                                                    UINT8 *pLvcfgChg,
                                                    UINT32 *pChangedLine)
{
    if (pYUVStrmCfg->Purpose != pYuvStrmInfo->Purpose) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }

    if (pYUVStrmCfg->DestVout != pYuvStrmInfo->DestVout) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }

    if (pYUVStrmCfg->DestEnc != pYuvStrmInfo->DestEnc) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }

    if (pYUVStrmCfg->DestDeciRate != pYuvStrmInfo->DestDeciRate) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }
}

static inline void HL_LvCfgOnCheckYuvStrmChkSize(const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYUVStrmCfg,
                                                 const CTX_YUV_STRM_INFO_s *pYuvStrmInfo,
                                                 UINT8 *pLvcfgChg,
                                                 UINT32 *pChangedLine)
{
    if (pYUVStrmCfg->Width != pYuvStrmInfo->Width) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }

    if (pYUVStrmCfg->Height != pYuvStrmInfo->Height) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }

    if (pYUVStrmCfg->MaxWidth != pYuvStrmInfo->MaxWidth) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }

    if (pYUVStrmCfg->MaxHeight != pYuvStrmInfo->MaxHeight) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }
}

static inline void HL_LvCfgOnCheckYuvStrmChkStrmBuffer(const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYUVStrmCfg,
                                                       const CTX_YUV_STRM_INFO_s *pYuvStrmInfo,
                                                       UINT8 *pLvcfgChg,
                                                       UINT32 *pChangedLine)
{
    UINT8  IsExtMem;
    UINT32 CmpVal = 0U;

    IsExtMem = (pYUVStrmCfg->StreamBuf.AllocType == ALLOC_INTERNAL)? 0U: 1U;
    if (IsExtMem != pYuvStrmInfo->IsExtMem) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }
    if (pYUVStrmCfg->StreamBuf.AllocType != pYuvStrmInfo->AllocType) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }
    if (pYUVStrmCfg->StreamBuf.BufNum != pYuvStrmInfo->BufNum) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }
    (void)dsp_osal_memcmp(&pYUVStrmCfg->StreamBuf.YuvBuf,
                          &pYuvStrmInfo->YuvBuf,
                          sizeof(AMBA_DSP_YUV_IMG_BUF_s), &CmpVal);
    if (CmpVal != 0U) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }
    if (pYUVStrmCfg->StreamBuf.AllocType == ALLOC_EXTERNAL_DISTINCT) {
        if (pYUVStrmCfg->StreamBuf.pYuvBufTbl[0U] != pYuvStrmInfo->YuvBufTbl[0U]) {
            *pLvcfgChg += 1U;
            *pChangedLine = __LINE__;
        }
    }
}

static inline void HL_LvCfgOnCheckYuvStrmChkChNum(const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYUVStrmCfg,
                                                  const CTX_YUV_STRM_INFO_s *pYuvStrmInfo,
                                                  const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout,
                                                  UINT8 *pLvcfgChg,
                                                  UINT32 *pChangedLine)
{
    UINT16 MaxChanNum = MAX2_16(pYUVStrmCfg->MaxChanNum,
                                pYUVStrmCfg->NumChan);
    if (MaxChanNum != pYuvStrmInfo->MaxChanNum) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }

    if (pYUVStrmCfg->NumChan != pYuvStrmLayout->NumChan) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }

//        pYuvStrmLayout->ChanLinked = 0x0U;
//        if (pYuvStrmLayout->ChanLinked != pYuvStrmLayoutOrig->ChanLinked) {
//            *pLvcfgChg += 1U;
//            *pChangedLine = __LINE__;
//        }
}

static inline void HL_LvCfgOnCheckYuvStrmChkChCfg(const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYUVStrmCfg,
                                                  const CTX_YUV_STRM_INFO_s *pYuvStrmInfo,
                                                  const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout,
                                                  UINT8 *pIsEfctYuvStrm,
                                                  UINT32 *pYuvStrmViewZoneOI,
                                                  UINT8 *pLvcfgChg,
                                                  UINT32 *pChangedLine)
{
    UINT8 IsEfctYuvStrm;
    UINT16 i, j, ViewZoneId;
    UINT32 CmpVal = 0U;

    IsEfctYuvStrm = HL_GetYuvStrmIsEffectOut(pYUVStrmCfg->MaxChanNum,
                                             pYUVStrmCfg->pChanCfg[0U].Window.OffsetX,
                                             pYUVStrmCfg->pChanCfg[0U].Window.OffsetY,
                                             pYUVStrmCfg->pChanCfg[0U].RotateFlip);
    for (i=0; i<pYUVStrmCfg->NumChan; i++) {
        UINT8 BlendNum;
        AMBA_DSP_WINDOW_DIMENSION_s MaxChanWin;

        (void)dsp_osal_memset(&MaxChanWin, 0, sizeof(AMBA_DSP_WINDOW_DIMENSION_s));
        (void)dsp_osal_memcmp(&pYUVStrmCfg->pChanCfg[i],
                              &pYuvStrmLayout->ChanCfg[i],
                              sizeof(AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s), &CmpVal);
        if (CmpVal != 0U) {
            *pLvcfgChg += 1U;
            *pChangedLine = __LINE__;
        }
        ViewZoneId = pYUVStrmCfg->pChanCfg[i].ViewZoneId;
        BlendNum = pYUVStrmCfg->pChanCfg[i].BlendNum;
        if (BlendNum > 0U) {
            for (j=0; j<BlendNum; j++) {
                const AMBA_DSP_BUF_s *pBldBuf = NULL;

                dsp_osal_typecast(&pBldBuf, &pYUVStrmCfg->pChanCfg[i].LumaAlphaTable);
                (void)dsp_osal_memcmp(&pBldBuf[j],
                                      &pYuvStrmLayout->BlendCfg[i][j],
                                      sizeof(AMBA_DSP_BUF_s),
                                      &CmpVal);
                if (CmpVal != 0U) {
                    *pLvcfgChg += 1U;
                    *pChangedLine = __LINE__;
                }
            }
        }

        /* Fill MaxChan information according to Layout to handle empty input */
        DSP_SetBit(pYuvStrmViewZoneOI, ViewZoneId);
        if (HL_GET_ROTATE(pYUVStrmCfg->pChanCfg[i].RotateFlip) == DSP_ROTATE_90_DEGREE) {
            MaxChanWin.Width = pYUVStrmCfg->pChanCfg[i].Window.Height;
            MaxChanWin.Height = pYUVStrmCfg->pChanCfg[i].Window.Width;
        } else {
            MaxChanWin.Width = pYUVStrmCfg->pChanCfg[i].Window.Width;
            MaxChanWin.Height = pYUVStrmCfg->pChanCfg[i].Window.Height;
        }

        if (1U == IsEfctYuvStrm) {
            CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

            HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
            if (1U != ViewZoneInfo->IsEffectChanMember) {
                *pLvcfgChg += 1U;
                *pChangedLine = __LINE__;
            }
        }
        if (pYUVStrmCfg->pMaxChanWin != NULL) {
            UINT16 Width = pYUVStrmCfg->pMaxChanWin[ViewZoneId].Width;
            UINT16 Height = pYUVStrmCfg->pMaxChanWin[ViewZoneId].Height;

            if ((Width > 0U) && (Height > 0U)) {
                MaxChanWin.Width = Width;
                MaxChanWin.Height = Height;
            }
        }
        (void)dsp_osal_memcmp(&MaxChanWin,
                              &pYuvStrmInfo->MaxChanWin[ViewZoneId],
                              sizeof(AMBA_DSP_WINDOW_DIMENSION_s), &CmpVal);
        if (CmpVal != 0U) {
            *pLvcfgChg += 1U;
            *pChangedLine = __LINE__;
        }
    }

    *pIsEfctYuvStrm = IsEfctYuvStrm;
}

static inline void HL_LvCfgOnCheckYuvStrmChkVzOI(const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYUVStrmCfg,
                                                 const CTX_YUV_STRM_INFO_s *pYuvStrmInfo,
                                                 const UINT32 *pYuvStrmViewZoneOI,
                                                 UINT8 *pLvcfgChg,
                                                 UINT32 *pChangedLine)
{
    UINT32 MaxChanBitMask;

    if (*pYuvStrmViewZoneOI != pYuvStrmInfo->ChanBitMask) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }

    /* Maximal possible channel and window */
    if (pYUVStrmCfg->MaxChanBitMask != 0U) {
        MaxChanBitMask = pYUVStrmCfg->MaxChanBitMask;
    } else {
        MaxChanBitMask = *pYuvStrmViewZoneOI;
    }
    if (MaxChanBitMask != pYuvStrmInfo->MaxChanBitMask) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }
}

static inline void HL_LvCfgOnCheckYuvStrmChkChWin(const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYUVStrmCfg,
                                                  const UINT32 YuvStrmViewZoneOI,
                                                  const UINT8 IsEfctYuvStrm,
                                                  UINT8 *pLvcfgChg,
                                                  UINT32 *pChangedLine)
{
    UINT16 i;
    if (pYUVStrmCfg->pMaxChanWin != NULL) {
        for (i=0; i<AMBA_DSP_MAX_YUVSTRM_VIEW_NUM; i++) {
            if ((pYUVStrmCfg->pMaxChanWin[i].Width > 0U) && (pYUVStrmCfg->pMaxChanWin[i].Height > 0U)) {
                if ((0U == DSP_GetBit(YuvStrmViewZoneOI, i, 1U)) && (1U == IsEfctYuvStrm)) {
                    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

                    HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
                    if (1U != ViewZoneInfo->IsEffectChanMember) {
                        *pLvcfgChg += 1U;
                        *pChangedLine = __LINE__;
                    }
                }
            }
        }
    }
}

/* Translate and check YuvStrm information
 *     CTX_YUV_STRM_INFO_s
 *         Purpose
 *         DestVout
 *         DestEnc
 *         Width
 *         Height
 *         MaxWidth
 *         MaxHeight
 *         IsExtMem
 *         AllocType
 *         BufNum
 *         YuvBuf
 *         pYuvBufTbl
 *         BufferStatus
 *         MaxChanNum
 *         Layout
 *         MaxChanBitMask
 *         MaxChanWin
 *         ChanBitMask
 * */
static inline void HL_LvCfgOnCheckYuvStrmChg(UINT16 NumYUVStream,
                                             const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYUVStrmCfg,
                                             UINT8 *pLvcfgChg,
                                             UINT32 *pChangedLine)
{
    UINT8 IsEfctYuvStrm;
    UINT16 i, StreamId;
    UINT32 YuvStrmViewZoneOI;
    CTX_YUV_STRM_INFO_s *YuvStrmInfoOrig = HL_CtxYuvStrmInfoPtr;
    const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayoutOrig;

    for (i=0U; ((i<NumYUVStream) && (*pChangedLine == 0U)); i++) {
        const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pStrmCfgEach = &pYUVStrmCfg[i];

        StreamId = pStrmCfgEach->StreamId;
        HL_GetYuvStrmInfoLock(StreamId, &YuvStrmInfoOrig);
        pYuvStrmLayoutOrig = &YuvStrmInfoOrig->Layout;

        HL_LvCfgOnCheckYuvStrmChkPurpose(pStrmCfgEach, YuvStrmInfoOrig,
                                         pLvcfgChg, pChangedLine);

        HL_LvCfgOnCheckYuvStrmChkSize(pStrmCfgEach, YuvStrmInfoOrig,
                                      pLvcfgChg, pChangedLine);

        HL_LvCfgOnCheckYuvStrmChkStrmBuffer(pStrmCfgEach, YuvStrmInfoOrig,
                                            pLvcfgChg, pChangedLine);

        HL_LvCfgOnCheckYuvStrmChkChNum(pStrmCfgEach, YuvStrmInfoOrig, pYuvStrmLayoutOrig,
                                            pLvcfgChg, pChangedLine);

        YuvStrmViewZoneOI = 0U;
        HL_LvCfgOnCheckYuvStrmChkChCfg(pStrmCfgEach, YuvStrmInfoOrig, pYuvStrmLayoutOrig,
                                       &IsEfctYuvStrm, &YuvStrmViewZoneOI, pLvcfgChg, pChangedLine);

        HL_LvCfgOnCheckYuvStrmChkVzOI(pStrmCfgEach, YuvStrmInfoOrig,
                                      &YuvStrmViewZoneOI, pLvcfgChg, pChangedLine);

        HL_LvCfgOnCheckYuvStrmChkChWin(pStrmCfgEach, YuvStrmViewZoneOI, IsEfctYuvStrm,
                                       pLvcfgChg, pChangedLine);
        HL_GetYuvStrmInfoUnLock(StreamId);
    }
}

/* Check ViewZone about resource information
       CTX_RESOURCE_INFO_s
*          ViewZoneNum
*          YuvStrmNum
*          VinBit
*          ViewZoneActiveBit
*/
static inline void HL_LvCfgCheckRescChange(const UINT16 NumViewZone,
                                           const UINT16 NumYUVStream,
                                           const UINT32 VinTotalBit,
                                           const UINT32 ViewZoneTotalBit,
                                           UINT8 *pLvcfgChg,
                                           UINT32 *pChangedLine)
{
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);
    if (Resource->ViewZoneNum != NumViewZone) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }
    if (Resource->YuvStrmNum != NumYUVStream) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }
    if (Resource->VinBit != VinTotalBit) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }
    if (Resource->ViewZoneActiveBit != ViewZoneTotalBit) {
        *pLvcfgChg += 1U;
        *pChangedLine = __LINE__;
    }
}

static inline void HL_LvCfgOnVzExtmem(CTX_VIEWZONE_INFO_s *pViewZoneInfo,
                                      const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s *pViewZoneCfg)
{
    UINT16 j;

    (void)dsp_osal_memcpy(&pViewZoneInfo->Pyramid, &pViewZoneCfg->Pyramid, sizeof(AMBA_DSP_LIVEVIEW_PYRAMID_s));
    if (pViewZoneCfg->pPyramidBuf != NULL) {
        pViewZoneInfo->PymdIsExtMem = (pViewZoneCfg->pPyramidBuf->AllocType == ALLOC_INTERNAL)? 0U: 1U;
        pViewZoneInfo->PymdAllocType = pViewZoneCfg->pPyramidBuf->AllocType;
        pViewZoneInfo->PymdBufNum = pViewZoneCfg->pPyramidBuf->BufNum;
        (void)dsp_osal_memcpy(&pViewZoneInfo->PymdYuvBuf, &pViewZoneCfg->pPyramidBuf->YuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
        for (j = 0U; (j < pViewZoneInfo->PymdBufNum) && (pViewZoneCfg->pPyramidBuf->pYuvBufTbl != NULL); j++) {
            pViewZoneInfo->PymdYuvBufTbl[j] = pViewZoneCfg->pPyramidBuf->pYuvBufTbl[j];
        }

        pViewZoneInfo->PymdBufferStatus = PYMD_BUF_STATUS_CONFIG;
    }

    (void)dsp_osal_memcpy(&pViewZoneInfo->LaneDetect, &pViewZoneCfg->LaneDetect, sizeof(AMBA_DSP_LIVEVIEW_LANE_DETECT_s));
    if (pViewZoneCfg->pLaneDetectBuf != NULL) {
        pViewZoneInfo->LndtIsExtMem = (pViewZoneCfg->pLaneDetectBuf->AllocType == ALLOC_INTERNAL)? 0U: 1U;
        pViewZoneInfo->LndtAllocType = pViewZoneCfg->pLaneDetectBuf->AllocType;
        pViewZoneInfo->LndtBufNum = pViewZoneCfg->pLaneDetectBuf->BufNum;
        (void)dsp_osal_memcpy(&pViewZoneInfo->LndtYuvBuf, &pViewZoneCfg->pLaneDetectBuf->YuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
        for (j = 0U; (j < pViewZoneInfo->LndtBufNum) && (pViewZoneCfg->pLaneDetectBuf->pYuvBufTbl != NULL); j++) {
            pViewZoneInfo->LndtYuvBufTbl[j] = pViewZoneCfg->pLaneDetectBuf->pYuvBufTbl[j];
        }

        pViewZoneInfo->LndtBufferStatus = LNDT_BUF_STATUS_CONFIG;
    }

    if (pViewZoneCfg->pMainY12Buf != NULL) {
        pViewZoneInfo->MainY12Enable = 1U;
        pViewZoneInfo->MainY12IsExtMem = (pViewZoneCfg->pMainY12Buf->AllocType == ALLOC_INTERNAL)? 0U: 1U;
        pViewZoneInfo->MainY12AllocType = pViewZoneCfg->pMainY12Buf->AllocType;
        pViewZoneInfo->MainY12BufNum = pViewZoneCfg->pMainY12Buf->BufNum;
        (void)dsp_osal_memcpy(&pViewZoneInfo->MainY12Buf, &pViewZoneCfg->pMainY12Buf->YuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
        for (j = 0U; (j < pViewZoneInfo->LndtBufNum) && (pViewZoneCfg->pMainY12Buf->pYuvBufTbl != NULL); j++) {
            pViewZoneInfo->MainY12BufTbl[j] = pViewZoneCfg->pMainY12Buf->pYuvBufTbl[j];
        }

        pViewZoneInfo->MainY12BufferStatus = MAIN_Y12_BUF_STATUS_CONFIG;
    } else {
        pViewZoneInfo->MainY12Enable = 0U;
    }
}

/* Config ViewZone information
 *     CTX_RESOURCE_INFO_s
 *          SourceVin
 *          SourceTdIdx
 *          InputFromMemory
 *          FlowBypassOption
 *          MctfCmpr
 *          LinearCe
 *          HdrBlendNumMinusOne
 *          HdrBlendHeight
 *          CapWindow
 *          PinWindow[DSP_VPROC_PIN_NUM]
 *          Main
 *          VinDecimationRate
 *          MainRotate
 *          Pyramid;
 *          PymdBufferStatus
 *          PymdIsExtMem
 *          PymdAllocType
 *          PymdBufNum
 *          PymdYuvBuf
 *          *pPymdYuvBufTbl
 *          LaneDetect;
 *          LndtBufferStatus;
 *          LndtIsExtMem;
 *          LndtAllocType;
 *          LndtBufNum;
 *          LndtYuvBuf;
 *          pLndtYuvBufTbl
 *          MainY12BufferStatus;
 *          MainY12IsExtMem;
 *          MainY12AllocType;
 *          MainY12BufNum;
 *          MainY12YuvBuf;
 *          pMainY12YuvBufTbl
 *          Pipe
 *          IsEffectChanMember
 * */
static inline void HL_LvCfgOnCfgViewZone(UINT16 NumViewZone,
                                         const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s *pViewZoneCfg)
{
    UINT16 i, ActualVinId, ViewZoneId;
    UINT32 VinBit = 0U;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT8 MctfDisable;
    UINT8 MctsDisable;
    UINT8 MctsDramOutDisable;

    for (i=0U; i<NumViewZone; i++) {
        ViewZoneId = pViewZoneCfg[i].ViewZoneId;
        HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);

        if (1U == DSP_GetU16Bit(pViewZoneCfg[i].VinId, VIN_VIRT_IDX, 1U)) {
            ActualVinId = (UINT16)DSP_GetU16Bit(pViewZoneCfg[i].VinId, 0U, 15U);
            ActualVinId += AMBA_DSP_MAX_VIN_NUM;
        } else {
            ActualVinId = (UINT16)DSP_GetU16Bit(pViewZoneCfg[i].VinId, 0U, 15U);
        }

        DSP_Idx2Bit((UINT8)ActualVinId, &VinBit);
        ViewZoneInfo->SourceVin = (UINT16)VinBit;

        if (1U == DSP_GetU8Bit(pViewZoneCfg[i].IsVirtChan, VIEWZONE_VIN_TD_BIT_IDX, 1U)) {
            ViewZoneInfo->SourceTdIdx = (UINT8)DSP_GetU16Bit(pViewZoneCfg[i].VinVirtChId, 8U, 8U);
        } else {
            ViewZoneInfo->SourceTdIdx = VIEWZONE_SRC_TD_IDX_NULL;
        }

        ViewZoneInfo->InputFromMemory = (UINT8)pViewZoneCfg[i].VinSource;

        MctfDisable = (UINT8)DSP_GetU8Bit(pViewZoneCfg[i].MctfDisable, DSP_MCTF_OFF_IDX, DSP_MCTF_OFF_LEN);
        if (MctfDisable > 0U) {
            DSP_SetBit(&ViewZoneInfo->FlowBypassOption, FLOW_BYPASS_MCTF_IDX);
        } else {
            DSP_ClearBit(&ViewZoneInfo->FlowBypassOption, FLOW_BYPASS_MCTF_IDX);
        }

        MctsDisable = (UINT8)DSP_GetU8Bit(pViewZoneCfg[i].MctsDisable, DSP_MCTS_OFF_IDX, DSP_MCTS_OFF_LEN);
        if (MctsDisable > 0U) {
            DSP_SetBit(&ViewZoneInfo->FlowBypassOption, FLOW_BYPASS_MCTS_IDX);
        } else {
            DSP_ClearBit(&ViewZoneInfo->FlowBypassOption, FLOW_BYPASS_MCTS_IDX);
        }
        MctsDramOutDisable = (UINT8)DSP_GetU8Bit(pViewZoneCfg[i].MctsDisable, DSP_MCTS_DRAM_OUT_IDX, DSP_MCTS_DRAM_OUT_LEN);
        if (MctsDramOutDisable > 0U) {
            DSP_SetBit(&ViewZoneInfo->FlowBypassOption, FLOW_BYPASS_MCTS_DRAM_OUT_IDX);
        } else {
            DSP_ClearBit(&ViewZoneInfo->FlowBypassOption, FLOW_BYPASS_MCTS_DRAM_OUT_IDX);
        }

        ViewZoneInfo->MctfCmpr = (UINT8)DSP_GetU8Bit(pViewZoneCfg[i].MctfDisable, DSP_MCTF_CMPR_IDX, DSP_MCTF_CMPR_LEN);

        ViewZoneInfo->LinearCe = pViewZoneCfg[i].LinearCE;
        ViewZoneInfo->HdrBlendNumMinusOne = (pViewZoneCfg[i].HdrExposureNum == 0U)? 0U: (pViewZoneCfg[i].HdrExposureNum-1U);
        ViewZoneInfo->HdrBlendHeight = pViewZoneCfg[i].HdrBlendHieght;

        (void)dsp_osal_memcpy(&ViewZoneInfo->CapWindow, &pViewZoneCfg[i].VinRoi, sizeof(AMBA_DSP_WINDOW_s));

        ViewZoneInfo->PinWindow[DSP_VPROC_PIN_MAIN].Width = pViewZoneCfg[i].MainWidth;
        ViewZoneInfo->PinWindow[DSP_VPROC_PIN_MAIN].Height = pViewZoneCfg[i].MainHeight;

        ViewZoneInfo->Main.Width = pViewZoneCfg[i].MainWidth;
        ViewZoneInfo->Main.Height = pViewZoneCfg[i].MainHeight;

        if (1U == DSP_GetU8Bit(pViewZoneCfg[i].IsVirtChan, VIEWZONE_VIN_DECI_BIT_IDX, 1U)) {
            ViewZoneInfo->VinDecimationRate = pViewZoneCfg[i].VinDecimation;
        } else {
            ViewZoneInfo->VinDecimationRate = (UINT8)1U;
        }

        ViewZoneInfo->Pipe = pViewZoneCfg[i].DramUtilityPipe;
        ViewZoneInfo->MainRotate = pViewZoneCfg[i].RotateFlip;

        ViewZoneInfo->IsProcRawDramOut = (DSP_GetU8Bit(pViewZoneCfg[i].IsVirtChan, VIEWZONE_PROC_RAW_BIT_IDX, 1U) == 1U) ? 1U : 0U;

        if (1U == DSP_GetU8Bit(pViewZoneCfg[i].IsVirtChan, VIEWZONE_INPUT_MUXSEL_BIT_IDX, 1U)) {
            ViewZoneInfo->IsInputMuxSel = (UINT8)1U;
            ViewZoneInfo->InputMuxSelSrc = (UINT8)DSP_GetBit(pViewZoneCfg[i].InputMuxSelCfg, VZ_IN_MUXSEL_SRC_BIT_IDX, VZ_IN_MUXSEL_SRC_LEN);
            ViewZoneInfo->InputMuxSelHorDeci = (UINT16)DSP_GetBit(pViewZoneCfg[i].InputMuxSelCfg, VZ_IN_MUXSEL_HOR_DEC_BIT_IDX, VZ_IN_MUXSEL_DEC_LEN);
            ViewZoneInfo->InputMuxSelVerDeci = (UINT16)DSP_GetBit(pViewZoneCfg[i].InputMuxSelCfg, VZ_IN_MUXSEL_VER_DEC_BIT_IDX, VZ_IN_MUXSEL_DEC_LEN);
        } else {
            ViewZoneInfo->IsInputMuxSel = (UINT8)0U;
            ViewZoneInfo->InputMuxSelSrc = (UINT8)0U;
            ViewZoneInfo->InputMuxSelHorDeci = 1U;
            ViewZoneInfo->InputMuxSelVerDeci = 1U;
        }

        HL_LvCfgOnVzExtmem(ViewZoneInfo, &pViewZoneCfg[i]);
        HL_GetViewZoneInfoUnLock(ViewZoneId);
    }
}

static inline void HL_LvCfgOnCfgYuvStrmChan(const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYUVStrmCfg,
                                            CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout,
                                            const UINT8 IsEfctYuvStrm,
                                            UINT32 *pYuvStrmViewZoneOI,
                                            CTX_YUV_STRM_INFO_s *pYuvStrmInfo)
{
    UINT16 j, ViewZoneId;

    for (j = 0U; j<pYUVStrmCfg->NumChan; j++) {
        (void)dsp_osal_memcpy(&pYuvStrmLayout->ChanCfg[j],
                              &pYUVStrmCfg->pChanCfg[j],
                              sizeof(AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s));
        ViewZoneId = pYUVStrmCfg->pChanCfg[j].ViewZoneId;
        (void)dsp_osal_memset(&pYuvStrmLayout->BlendCfg[j][0U],
                              0,
                              sizeof(AMBA_DSP_BUF_s)*DSP_MAX_PP_STRM_BLEND_NUMBER);
        if (pYuvStrmLayout->ChanCfg[j].BlendNum > 0U) {
            const AMBA_DSP_BUF_s *pBldBuf;

            dsp_osal_typecast(&pBldBuf, &pYUVStrmCfg->pChanCfg[j].LumaAlphaTable);
            (void)dsp_osal_memcpy(&pYuvStrmLayout->BlendCfg[j][0U],
                                  &pBldBuf[0U],
                                  sizeof(AMBA_DSP_BUF_s)*(pYuvStrmLayout->ChanCfg[j].BlendNum));
        }
        /* Fill MaxChan information according to Layout to handle empty input */
        DSP_SetBit(pYuvStrmViewZoneOI, ViewZoneId);
        if (HL_GET_ROTATE(pYUVStrmCfg->pChanCfg[j].RotateFlip) == DSP_ROTATE_90_DEGREE) {
            pYuvStrmInfo->MaxChanWin[ViewZoneId].Width = pYUVStrmCfg->pChanCfg[j].Window.Height;
            pYuvStrmInfo->MaxChanWin[ViewZoneId].Height = pYUVStrmCfg->pChanCfg[j].Window.Width;
        } else {
            pYuvStrmInfo->MaxChanWin[ViewZoneId].Width = pYUVStrmCfg->pChanCfg[j].Window.Width;
            pYuvStrmInfo->MaxChanWin[ViewZoneId].Height = pYUVStrmCfg->pChanCfg[j].Window.Height;
        }

        if (1U == IsEfctYuvStrm) {
            CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

            HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
            ViewZoneInfo->IsEffectChanMember = 1U;
            HL_GetViewZoneInfoUnLock(ViewZoneId);
        }
    }
}

/* Config ViewZone information
 *     CTX_YUV_STRM_INFO_s
 *         Purpose
 *         DestVout
 *         DestEnc
 *         Width
 *         Height
 *         MaxWidth
 *         MaxHeight
 *         IsExtMem
 *         AllocType
 *         BufNum
 *         YuvBuf
 *         pYuvBufTbl
 *         BufferStatus
 *         MaxChanNum
 *         Layout
 *         MaxChanBitMask
 *         MaxChanWin
 *         ChanBitMask
 * */
static inline void HL_LvCfgOnCfgYuvStrm(UINT16 NumYUVStream,
                                        const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYUVStrmCfg)
{
    UINT8 IsEfctYuvStrm;
    UINT16 i, j, StreamId;
    UINT32 YuvStrmViewZoneOI;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout = NULL;

    for (i = 0U; i < NumYUVStream; i++) {
        StreamId = pYUVStrmCfg[i].StreamId;
        HL_GetYuvStrmInfoLock(StreamId, &YuvStrmInfo);
        pYuvStrmLayout = &YuvStrmInfo->Layout;
        YuvStrmInfo->Purpose = pYUVStrmCfg[i].Purpose;
        YuvStrmInfo->DestVout = pYUVStrmCfg[i].DestVout;
        YuvStrmInfo->DestEnc = pYUVStrmCfg[i].DestEnc;
        YuvStrmInfo->Width = pYUVStrmCfg[i].Width;
        YuvStrmInfo->Height = pYUVStrmCfg[i].Height;
        YuvStrmInfo->MaxWidth = pYUVStrmCfg[i].MaxWidth;
        YuvStrmInfo->MaxHeight = pYUVStrmCfg[i].MaxHeight;
        YuvStrmInfo->IsExtMem = (pYUVStrmCfg[i].StreamBuf.AllocType == ALLOC_INTERNAL)? 0U: 1U;
        YuvStrmInfo->AllocType = pYUVStrmCfg[i].StreamBuf.AllocType;
        YuvStrmInfo->BufNum = pYUVStrmCfg[i].StreamBuf.BufNum;
        (void)dsp_osal_memcpy(&YuvStrmInfo->YuvBuf, &pYUVStrmCfg[i].StreamBuf.YuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
        if (pYUVStrmCfg[i].StreamBuf.AllocType == ALLOC_EXTERNAL_DISTINCT) {
            for (j = 0U; j < YuvStrmInfo->BufNum; j++) {
                YuvStrmInfo->YuvBufTbl[j] = pYUVStrmCfg[i].StreamBuf.pYuvBufTbl[j];
            }
        }

        YuvStrmInfo->BufferStatus = YUVSTRM_BUF_STATUS_CONFIG;
        YuvStrmInfo->MaxChanNum = MAX2_16(pYUVStrmCfg[i].MaxChanNum, pYUVStrmCfg[i].NumChan);
        pYuvStrmLayout->NumChan = pYUVStrmCfg[i].NumChan;
        pYuvStrmLayout->ChanLinked = 0x0U;
//        if (pYuvStrmLayout->ChanLinked != pYuvStrmLayoutOrig->ChanLinked) {
//            *pLvcfgChg += 1U;
//            *pChangedLine = __LINE__;
//        }
        YuvStrmViewZoneOI = 0U;
        IsEfctYuvStrm = HL_GetYuvStrmIsEffectOut(pYUVStrmCfg[i].MaxChanNum,
                                                 pYUVStrmCfg[i].pChanCfg[0U].Window.OffsetX,
                                                 pYUVStrmCfg[i].pChanCfg[0U].Window.OffsetY,
                                                 pYUVStrmCfg[i].pChanCfg[0U].RotateFlip);

        HL_LvCfgOnCfgYuvStrmChan(&pYUVStrmCfg[i], pYuvStrmLayout, IsEfctYuvStrm, &YuvStrmViewZoneOI, YuvStrmInfo);
        YuvStrmInfo->ChanBitMask = YuvStrmViewZoneOI;

        /* setup all Layout.ChanCfg for HL_LvUptEfctCfgOnStateXXX to check pSyncJob->PrevSizeUpdate */
        for (j=0; j<MAX_GROUP_CMD_POOL_NUM; j++) {
            UINT8 k;
            CTX_YUV_STRM_LAYOUT_s *pLayout = HL_CtxYuvStrmEfctLayoutPtr;

            HL_GetYuvStrmEfctSyncJobSetLayoutLock(i, j, &pLayout);
            pLayout->NumChan = pYUVStrmCfg[i].NumChan;
            for (k=0; k<pYUVStrmCfg[i].NumChan; k++) {
                (void)dsp_osal_memcpy(&pLayout->ChanCfg[k],
                                      &pYuvStrmLayout->ChanCfg[k],
                                      sizeof(AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s));
            }
            HL_GetYuvStrmEfctSyncJobSetUnLock(i, j);
        }

        /* Maximal possible channel and window */
        if (pYUVStrmCfg[i].MaxChanBitMask != 0U) {
            YuvStrmInfo->MaxChanBitMask = pYUVStrmCfg[i].MaxChanBitMask;
        } else {
            YuvStrmInfo->MaxChanBitMask = YuvStrmViewZoneOI;
        }
        if (pYUVStrmCfg[i].pMaxChanWin != NULL) {
            for (j = 0U; j < AMBA_DSP_MAX_YUVSTRM_VIEW_NUM; j++) {
                UINT16 MaxChanWinWidth = pYUVStrmCfg[i].pMaxChanWin[j].Width;
                UINT16 MaxChanWinHeight = pYUVStrmCfg[i].pMaxChanWin[j].Height;

                if ((MaxChanWinWidth > 0U) && (MaxChanWinHeight > 0U)) {
                    YuvStrmInfo->MaxChanWin[j].Width = MaxChanWinWidth;
                    YuvStrmInfo->MaxChanWin[j].Height = MaxChanWinHeight;

                    if ((0U == DSP_GetBit(YuvStrmViewZoneOI, j, 1U)) && (1U == IsEfctYuvStrm)) {
                        CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

                        HL_GetViewZoneInfoLock((UINT16)j, &ViewZoneInfo);
                        ViewZoneInfo->IsEffectChanMember = 1U;
                        HL_GetViewZoneInfoUnLock((UINT16)j);
                    }
                }
            }
        }
        YuvStrmInfo->DestDeciRate = pYUVStrmCfg[i].DestDeciRate;
        HL_GetYuvStrmInfoUnLock(StreamId);
    }
}

/* Store resource change about ViewZone
       CTX_RESOURCE_INFO_s
*          ViewZoneNum
*          YuvStrmNum
*          VinBit
*          ViewZoneActiveBit
*/
static inline void HL_LvCfgOnCfgResc(UINT16 NumViewZone,
                                     UINT16 NumYUVStream,
                                     UINT32 VinTotalBit,
                                     UINT32 ViewZoneTotalBit)
{
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourceLock(&Resource);
    Resource->ViewZoneNum = NumViewZone;
    Resource->YuvStrmNum = NumYUVStream;
    Resource->VinBit = (UINT16)VinTotalBit;
    Resource->ViewZoneActiveBit = ViewZoneTotalBit;
    Resource->RescState = HL_RESC_CONFIGED;
    HL_GetResourceUnLock();
}

static inline void HL_LvCfgOnReInitYuvStrm(UINT16 NumYUVStream)
{
    UINT16 i;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;

    for (i=0U; i<NumYUVStream; i++) {
        HL_GetYuvStrmInfoLock(i, &YuvStrmInfo);
        YuvStrmInfo->BufferStatus = YUVSTRM_BUF_STATUS_CONFIG;
        HL_GetYuvStrmInfoUnLock(i);
    }
}

static inline UINT32 HL_LiveviewConfigChk(UINT16 NumViewZone,
                                          const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s *pViewZoneCfg,
                                          UINT16 NumYUVStream,
                                          const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYUVStrmCfg)
{
    UINT32 Rval;
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pLiveviewConfig(NumViewZone, pViewZoneCfg, NumYUVStream, pYUVStrmCfg);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pLiveviewConfig(NumViewZone, pViewZoneCfg, NumYUVStream, pYUVStrmCfg);
    }

    /* Logic sanity check */
    if (Rval == OK) {
        // check blending cfg position
#if 0 // SSP handle it by dummy blending
        UINT8 YuvStrmHasBlend;
        UINT16 i, j;

        for (i=0U; i<NumYUVStream; i++) {
            YuvStrmHasBlend = 0U;
            for (j=0U; j<pYUVStrmCfg[i].NumChan; j++) {
                if (pYUVStrmCfg[i].pChanCfg[j].BlendNum != 0U) {
                    YuvStrmHasBlend = 1U;
                }
                if ((YuvStrmHasBlend == 1U) &&
                    (j == (pYUVStrmCfg[i].NumChan - 1U)) && (pYUVStrmCfg[i].pChanCfg[j].BlendNum == 0U)) {
                    AmbaLL_LogUInt5("[ERROR] YuvStrm[%d]'s last channel must have blending job", i, 0U, 0U, 0U, 0U);
                    Rval = DSP_ERR_0006;
                    break;
                }
            }
        }
#endif
    }

    return Rval;
}

/**
* Liveview config function
* @param [in]  NumViewZone number of viewzone
* @param [in]  pViewZoneCfg viewzone config
* @param [in]  NumYUVStream number of yuv stream
* @param [in]  pYUVStrmCfg yuv stream config
* @return ErrorCode
*/
UINT32 dsp_liveview_cfg(UINT16 NumViewZone,
                        const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s *pViewZoneCfg,
                        UINT16 NumYUVStream,
                        const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYUVStrmCfg)
{
    UINT32 Rval;

    Rval = HL_LiveviewConfigChk(NumViewZone, pViewZoneCfg, NumYUVStream, pYUVStrmCfg);

    /* Body */
    if (Rval == OK) {
        UINT32 VinTotalBit = 0U, ViewZoneTotalBit = 0U;
        UINT32 ChgLine = 0U;
        UINT8 LvcfgChg = 0U;

        /* Translate and check ViewZone information */
        HL_LvCfgOnCheckViewZoneChg(NumViewZone, pViewZoneCfg,
                                   &VinTotalBit, &ViewZoneTotalBit,
                                   &LvcfgChg, &ChgLine);
        if (ChgLine != 0U) {
            AmbaLL_LogUInt5("LvCfg LvcfgChg: %u @%u", LvcfgChg, ChgLine, 0U, 0U, 0U);
        }

        /* Translate and check YuvStrm information */
        HL_LvCfgOnCheckYuvStrmChg(NumYUVStream, pYUVStrmCfg,
                                  &LvcfgChg, &ChgLine);
        if (ChgLine != 0U) {
            AmbaLL_LogUInt5("LvCfg LvcfgChg: %u @%u", LvcfgChg, ChgLine, 0U, 0U, 0U);
        }

        /* Check resource change */
        HL_LvCfgCheckRescChange(NumViewZone, NumYUVStream, VinTotalBit, ViewZoneTotalBit,
                                &LvcfgChg, &ChgLine);
        if (ChgLine != 0U) {
            AmbaLL_LogUInt5("LvCfg LvcfgChg: %u @%u", LvcfgChg, ChgLine, 0U, 0U, 0U);
        }

        if (LvcfgChg != 0U) {
            UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;
            /* Reset necessary status information */
            HL_ResetViewZoneInfo();
            if (pIsSimilarEnabled != NULL) {
                if (1U == IsSimilarEnabled()) {
                    SIM_ResetViewZoneInfo();
                }
            }
            HL_ResetYuvStreamInfo();
            HL_ResetYuvStrmEfctSyncCtrl();
            HL_ResetYuvStrmEfctSyncJobSet();
            HL_ResetVprocInfo();

            /* Store ViewZone information */
            HL_LvCfgOnCfgViewZone(NumViewZone, pViewZoneCfg);

            /* Store YuvStream information */
            HL_LvCfgOnCfgYuvStrm(NumYUVStream, pYUVStrmCfg);

            /* Store Lvcfg related CTX_RESOURCE_INFO_s */
            HL_LvCfgOnCfgResc(NumViewZone, NumYUVStream, VinTotalBit, ViewZoneTotalBit);
        } else {
            /* Re-init ViewZone information */
            HL_InitExtPymdLndtBuf(NumViewZone);

            /* Re-init YuvStream information */
            HL_LvCfgOnReInitYuvStrm(NumYUVStream);
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_LiveviewConfig Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

//#define DEBUG_LV_CTRL_DISABLE
static inline UINT32 HL_LvCtrlLogicCheck(UINT16 NumViewZone,
                                         const UINT16 *pViewZoneId,
                                         const UINT8 *pEnable,
                                         UINT8 *pNumEnable,
                                         UINT8 *pNumDisable,
                                         UINT32 *pVprocEnableBit,
                                         UINT32 *pVprocDisableBit)
{
    UINT32 Rval = OK;
    UINT16 i;

    for (i=0; i<NumViewZone; i++) {
        if (pEnable[i] == 0U) {
            (*pNumDisable)++;
            DSP_SetBit(pVprocDisableBit, pViewZoneId[i]);
        } else {
            (*pNumEnable)++;
            DSP_SetBit(pVprocEnableBit, pViewZoneId[i]);
        }
    }

    if ((*pNumEnable > 0U) && (*pNumDisable > 0U)) {
        Rval = DSP_ERR_0001;
        AmbaLL_LogUInt5("CANT support Enable/Disable at same time now", 0U, 0U, 0U, 0U, 0U);
    }

#ifdef DEBUG_LV_CTRL_DISABLE
       AmbaLL_LogUInt5("HL_LvCtrlLogicCheck NumViewZone:%u *pNumEnable:%u *pNumDisable:%u *pVprocEnableBit:0x%x *pVprocDisableBit:0x%x",
               NumViewZone, *pNumEnable, *pNumDisable,
               *pVprocEnableBit, *pVprocDisableBit);
#endif
    return Rval;
}

static inline UINT32 HL_LvCtrDisOnVoutSrcCtrl(UINT16 NumViewZone,
                                              const UINT16 *pViewZoneId,
                                              const UINT8 *pEnable,
                                              const CTX_RESOURCE_INFO_s *pResource)
{
    UINT32 Rval = OK;
    UINT16 i, j;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;

    for (i = 0U; i < pResource->YuvStrmNum; i++) {
        UINT16 VoutId = 0xFFU;
        UINT32 YuvStrmViewOI = 0U;

        HL_GetYuvStrmInfoPtr((UINT16)i, &YuvStrmInfo);

        if (0U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U)) {
            continue;
        }

        /* If all the active Views in YuvStrm are disabled, switch this Vout YuvStream */
        YuvStrmViewOI = YuvStrmInfo->MaxChanBitMask;
        for (j=0U; (j<NumViewZone)&&(pEnable[j]==0U); j++) {
            DSP_ClearBit(&YuvStrmViewOI, (UINT32)pViewZoneId[j]);
        }
        if (YuvStrmViewOI == 0U) {
            DSP_Bit2U16Idx((UINT32)YuvStrmInfo->DestVout, &VoutId);
        }
        if (VoutId != 0xFFU) {
            CTX_VOUT_INFO_s VoutInfo = {0};

            WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
            HL_GetVoutInfo(HL_MTX_OPT_ALL, VoutId, &VoutInfo);
            if ((VoutInfo.VideoEnable == 1U) && (VoutInfo.Source == DSP_VOUT_SRC_VCAP)) {
                (void)HL_VoutVideoSetupSourceSelect((UINT8)VoutId, DSP_VOUT_SRC_BG, HL_VOUT_SOURCE_NONE);
                Rval = HL_VoutVideoSetup((UINT8)VoutId, WriteMode);
                if (Rval == OK) {
                    Rval = DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP0_EVENT, 1, WAIT_INTERRUPT_TIMEOUT);
                    if (Rval != OK) {
                        AmbaLL_LogUInt5("[Err][0x%X] Can't wait for VDSP0 EVENT", Rval, 0U, 0U, 0U, 0U);
                        Rval = DSP_ERR_0003;
                    }
                }
            }
        }
    }

    return Rval;
}

static inline UINT32 HL_LvCtrChkVprocOI(UINT16 NumViewZone,
                                        const UINT16 *pViewZoneId,
                                        const CTX_RESOURCE_INFO_s *pResource,
                                        UINT32 *pTargetVprocBit)
{
    UINT32 Rval = OK;
    UINT16 i, j;

    for (i = 0U; i < pResource->YuvStrmNum; i++) {
       CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
       UINT32 YuvStrmViewOI = 0U, PartialDisableViewOI = 0U;
       UINT16 ViewNum, ViewId;

       /* Only need to check for effect stream,
        * Keep TargetVprocBit as original if no effect,
        * Original value is assigned at HL_LvCtrlLogicCheck */
       HL_GetYuvStrmInfoPtr((UINT16)i, &YuvStrmInfo);
       if (0U == HL_GetEffectEnableOnYuvStrm(i)) {
            continue;
       }

       YuvStrmViewOI = YuvStrmInfo->MaxChanBitMask;
       for (j=0U; j<NumViewZone; j++) {
           DSP_ClearBit(&YuvStrmViewOI, (UINT32)pViewZoneId[j]);
           DSP_SetBit(&PartialDisableViewOI, (UINT32)pViewZoneId[j]);
       }
       if (YuvStrmViewOI != 0U) {
           DSP_Bit2U16Cnt(PartialDisableViewOI, &ViewNum);
           for (j=0U; j<ViewNum; j++) {
               DSP_Bit2U16Idx(PartialDisableViewOI, &ViewId);
               DSP_ClearBit(pTargetVprocBit, ViewId);
               DSP_ClearBit(&PartialDisableViewOI, ViewId);
           }
       }
#ifdef DEBUG_LV_CTRL_DISABLE
       AmbaLL_LogUInt5("HL_LvCtrChkVprocOI Yuv:%u MaxChan:0x%x YuvStrmViewOI:0x%x pTargetVprocBit:0x%x",
               i, YuvStrmInfo.MaxChanBitMask, YuvStrmViewOI, *pTargetVprocBit, 0U);
#endif
    }
#ifdef DEBUG_LV_CTRL_DISABLE
       AmbaLL_LogUInt5("HL_LvCtrChkVprocOI pTargetVprocBit:0x%x", *pTargetVprocBit, 0U, 0U, 0U, 0U);
#endif
    return Rval;
}

static inline UINT32 HL_LvCtrDisUptVprocGrpFillPpStrmCfg(UINT16 YuvStrmIdx,
                                                         const UINT16 *pViewZoneOrder,
                                                         UINT8 WriteMode)
{
    UINT32 Rval;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_vproc_multi_stream_pp_t *VprocStrmPp = HL_DefCtxCmdBufPtrVpcPp;

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&VprocStrmPp, &CmdBufferAddr);
    Rval = HL_FillPpStrmCfgOrder(YuvStrmIdx, pViewZoneOrder, VprocStrmPp, NULL);
    if (Rval != OK) {
        AmbaLL_LogUInt5("HL_FillPpStrmCfgOrder fail", 0U, 0U, 0U, 0U, 0U);
    } else {
        Rval = AmbaHL_CmdVprocMultiStrmPpCmd(WriteMode, VprocStrmPp);
        if (Rval != OK) {
            AmbaLL_LogUInt5("AmbaHL_CmdVprocMultiStrmPpCmd fail", 0U, 0U, 0U, 0U, 0U);
        }
    }
    HL_RelCmdBuffer(CmdBufferId);

    return Rval;
}

static inline UINT32 HL_LvCtrDisUptVprocGrp(const UINT16 GrpId,
                                            const UINT16 MasterViewId,
                                            const UINT16 MasterVinId,
                                            const UINT16 GrpDisableVZNum,
                                            const UINT16 *pGrpDisableVZId)
{
    UINT8 WriteMode = 0U, HasMasterVproc = 0U, HasSlaveVprocVinAlive = 0U;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT16 i, ViewZoneId;
    UINT16 ViewZoneOrder[NUM_VPROC_MAX_CHAN] = {0};
    UINT32 Rval = OK, DisableVzMask = 0U, IsGrpFreeRun;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    cmd_vproc_multi_chan_proc_order_t *ChanProcOrder = HL_DefCtxCmdBufPtrVpcChOrd;
    cmd_vproc_set_vproc_grping *VprocGroup = HL_DefCtxCmdBufPtrVpcGrp;

    /* check target is master vproc in same group*/
    for (i=0U; i<GrpDisableVZNum; i++) {
        ViewZoneId = pGrpDisableVZId[i];
        if (ViewZoneId == MasterViewId) {
            HasMasterVproc = 1U;
            DSP_SetBit(&DisableVzMask, ViewZoneId);
        }
#ifdef DEBUG_LV_CTRL_DISABLE
        AmbaLL_LogUInt5("HL_LvCtrDisUptVprocGrp i:%u HasMsVproc:%u MsId:%u DisVzMask:0x%x",
                        i, HasMasterVproc, MasterViewId, DisableVzMask, 0U);
#endif
    }

    /* check target group has other slave vproc using alive vin */
    HL_GetResourcePtr(&Resource);
    for (i=0U; i<Resource->MaxViewZoneNum; i++) {
        UINT8 IsVzActive = (0U == DSP_GetBit(Resource->ViewZoneActiveBit, i, 1U)) ? (UINT8)0U: (UINT8)1U;
        UINT16 VzGrpId, VinId;

        ViewZoneId = i;
        Rval = HL_GetVprocGroupIdx(ViewZoneId, &VzGrpId, 0/*IsStlProc*/);
        if ((Rval != OK) || (VzGrpId != GrpId) || (IsVzActive == 0U)) {
            continue;
        }
        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
        DSP_Bit2U16Idx((UINT32)ViewZoneInfo->SourceVin, &VinId);
        if (ViewZoneInfo->IsMaster == 0U) {
            if (VinId != MasterVinId) {
                /* slave vproc using alive vin */
                HasSlaveVprocVinAlive = 1U;
            } else {
                /* Don't change to vz which vin is going to idle */
                DSP_SetBit(&DisableVzMask, ViewZoneId);
            }
        }
#ifdef DEBUG_LV_CTRL_DISABLE
        AmbaLL_LogUInt5("HL_LvCtrDisUptVprocGrp vz:%u HasSlaveVprocVinAlive:%u DisableVzMask:0x%x IsMaster:%u VinId:%u",
                        i, HasSlaveVprocVinAlive, DisableVzMask, ViewZoneInfo->IsMaster, VinId);
#endif
    }

    /*
     * different vin has no effect will use free_run_grp,
     * no need to update VprocChanProcOrder in that case
     * */
    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&VprocGroup, &CmdBufferAddr);
    (void)HL_FillVideoProcGrouping(VprocGroup);
    IsGrpFreeRun = DSP_GetBit(VprocGroup->free_run_grp, GrpId, 1U);
    HL_RelCmdBuffer(CmdBufferId);
#ifdef DEBUG_LV_CTRL_DISABLE
    AmbaLL_LogUInt5("HL_LvCtrDisUptVprocGrp free_run_grp:0x%x IsGrpFreeRun:%u HasMasterVproc:%x HasSlaveVprocVinAlive:%x",
            VprocGroup->free_run_grp, IsGrpFreeRun, HasMasterVproc, HasSlaveVprocVinAlive, 0U);
#endif

    if ((HasMasterVproc == 1U) && (HasSlaveVprocVinAlive == 1U) && (IsGrpFreeRun == 0U)) {
        (void)dsp_osal_memset(ViewZoneOrder, 0x0, (sizeof(UINT16)*NUM_VPROC_MAX_CHAN));
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&ChanProcOrder, &CmdBufferAddr);
        HL_UpdateVprocChanProcOrder((UINT8)GrpId,
                                    DisableVzMask,
                                    ChanProcOrder,
                                    0U/*DisableVproc*/,
                                    ViewZoneOrder);
#ifdef DEBUG_LV_CTRL_DISABLE
        AmbaLL_LogUInt5("HL_LvCtrDisUptVprocGrp num_of_vproc_channel:%u DisableVzMask:0x%x",
                ChanProcOrder->num_of_vproc_channel, DisableVzMask, 0U, 0U, 0U);
#endif
        if (ChanProcOrder->num_of_vproc_channel > 1U) {
            UINT16 YuvStrmIdx;

            /* set new vproc order */
            WriteMode = HL_GetVin2CmdNormalWrite(MasterVinId);
            Rval = AmbaHL_CmdVprocMultiChanProcOrder(WriteMode, ChanProcOrder);
            if (Rval != OK) {
                AmbaLL_LogUInt5("AmbaHL_CmdVprocMultiChanProcOrder fail", 0U, 0U, 0U, 0U, 0U);
            }
            /* update viewzone master info */
            HL_GetViewZoneInfoLock(MasterViewId, &ViewZoneInfo);
            ViewZoneInfo->IsMaster = 0U;
            HL_GetViewZoneInfoUnLock(MasterViewId);
            HL_GetViewZoneInfoLock(ViewZoneOrder[0], &ViewZoneInfo);
            ViewZoneInfo->IsMaster = 1U;
            HL_GetViewZoneInfoUnLock(ViewZoneOrder[0]);

            /* need new pp for all effect yuv stream*/
            for (YuvStrmIdx=0U; YuvStrmIdx<Resource->YuvStrmNum; YuvStrmIdx++) {
                UINT8 IsEfctYuvStrm, IsMasterVprocInYuvStrm;
                CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;

                HL_GetYuvStrmInfoLock(YuvStrmIdx, &YuvStrmInfo);
                /* Only need for effect yuv stream */
                IsEfctYuvStrm = (1U == HL_GetEffectEnableOnYuvStrm(YuvStrmIdx))? (UINT8)1U: (UINT8)0U;
                /* Only need for effect member include disable master vproc */
                IsMasterVprocInYuvStrm = (0U == DSP_GetBit(YuvStrmInfo->ChanBitMask, MasterViewId, 1U)) ? (UINT8)0U: (UINT8)1U;
                HL_GetYuvStrmInfoUnLock(YuvStrmIdx);
#ifdef DEBUG_LV_CTRL_DISABLE
                AmbaLL_LogUInt5("HL_LvCtrDisUptVprocGrp i:%u YuvStrmIdx:%u IsEfctYuvStrm:%u IsMasterVprocInYuvStrm:%u",
                                GrpId, YuvStrmIdx, IsEfctYuvStrm, IsMasterVprocInYuvStrm, 0U);
#endif
                if ((IsEfctYuvStrm == 0U) || (IsMasterVprocInYuvStrm == 0U)) {
                    continue;
                }

                Rval = HL_LvCtrDisUptVprocGrpFillPpStrmCfg(YuvStrmIdx, ViewZoneOrder, WriteMode);
            }
        }
        HL_RelCmdBuffer(CmdBufferId);
    }
    return Rval;
}

static inline UINT32 HL_LvCtrDisOnVprocStop(UINT16 NumViewZone,
                                            const UINT16 *pViewZoneId,
                                            const UINT8 *pEnable,
                                            const UINT32 *pVprocDisableBit)
{
    UINT32 Rval = OK;
    UINT16 i, j, GrpNum;
    UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;

    (void)HL_GetVprocGroupNum(&GrpNum);
#ifdef DEBUG_LV_CTRL_DISABLE
       AmbaLL_LogUInt5("HL_LvCtrDisOnVprocStop GrpNum:%u", GrpNum, 0, 0, 0, 0);
#endif
    for (i = 0; i<GrpNum; i++) {
        UINT8 VprocStopped = 0U;
        UINT16 ViewZoneId, GrpId = 0U, GrpDisableVZNum = 0U;
        UINT16 MasterVZId = AMBA_DSP_MAX_VIEWZONE_NUM, MasterVinId = 0U;
        UINT16 GrpDisableVZId[NUM_VPROC_MAX_CHAN] = {0};
        UINT32 GrpDisableVZMask = 0U;
        CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

        /* Get group info */
        HL_GetGroupMasterVprocId(i, &MasterVZId);
        if (MasterVZId < AMBA_DSP_MAX_VIEWZONE_NUM) {
            HL_GetViewZoneInfoPtr(MasterVZId, &ViewZoneInfo);
            if (ViewZoneInfo->InputFromMemory != VIN_SRC_FROM_DEC) {
                DSP_Bit2U16Idx((UINT32)ViewZoneInfo->SourceVin, &MasterVinId);
            } else {
                MasterVinId = HL_GetMinimalActiveVinId();
            }
//            HL_GetGroupVZMask(i, &GrpVZMask);
            (void)dsp_osal_memset(GrpDisableVZId, 0x0, (sizeof(UINT16)*NUM_VPROC_MAX_CHAN));
            for (j=0U; (j<NumViewZone)&&(pEnable[j]==0U); j++) {
                ViewZoneId = pViewZoneId[j];
                Rval = HL_GetVprocGroupIdx(ViewZoneId, &GrpId, 0/*IsStlProc*/);
                if ((Rval == OK) && (GrpId == i)) {
                    GrpDisableVZId[GrpDisableVZNum] = ViewZoneId;
                    GrpDisableVZNum++;
#ifdef DEBUG_LV_CTRL_DISABLE
                    AmbaLL_LogUInt5("HL_LvCtrDisOnVprocStop Grp:%u j:%u GrpDisableVZId:%u GrpDisableVZNum:%u",
                            i, j, ViewZoneId, GrpDisableVZNum, 0U);
#endif
                }
            }
//            DSP_Bit2U16Cnt(GrpVZMask, &GrpVzNum);
        }
#ifdef DEBUG_LV_CTRL_DISABLE
        AmbaLL_LogUInt5("HL_LvCtrDisOnVprocStop Grp:%u MasterVZId:%u MasterVinId:%u GrpDisableVZNum:%u",
                        i, MasterVZId, MasterVinId, GrpDisableVZNum, 0U);
#endif
        /* Stop Vproc */
        for (j=0U; j<GrpDisableVZNum; j++) {
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_vproc_stop_t *VprocStop = HL_DefCtxCmdBufPtrVpcStop;

            /* only stop if not partial zone,
             * pVprocDisableBit is settle in HL_LvCtrChkVprocOI */
            ViewZoneId = GrpDisableVZId[j];
            if (0U == DSP_GetBit(*pVprocDisableBit, ViewZoneId, 1U)) {
                continue;
            }
            if (DSP_VPROC_STATUS_INVALID == DSP_GetVprocState((UINT8)ViewZoneId)) {
                continue;
            }

            DSP_SetBit(&GrpDisableVZMask, ViewZoneId);
            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VprocStop, &CmdBufferAddr);
            VprocStop->channel_id = (UINT8)ViewZoneId;
            VprocStop->stop_cap_seq_no = 0U; //obsoleted
            VprocStop->reset_option = 0U; //obsoleted
            WriteMode = HL_GetVin2CmdGroupWrite(MasterVinId);
            Rval = AmbaHL_CmdVprocStop(WriteMode, VprocStop);
            HL_RelCmdBuffer(CmdBufferId);
            if (Rval != OK) {
                AmbaLL_LogUInt5("AmbaHL_CmdVprocStop fail", 0U, 0U, 0U, 0U, 0U);
            } else {
                VprocStopped = 1U;
            }
#ifdef DEBUG_LV_CTRL_DISABLE
            AmbaLL_LogUInt5("HL_LvCtrDisOnVprocStop Grp:%u MasterVZId:0x%x ViewZoneId:%u MasterVinId:%u",
                    i, MasterVZId, ViewZoneId, MasterVinId, 0U);
#endif
        }
//        if ((VprocStopped == 1U) && (MasterVZId < AMBA_DSP_MAX_VIEWZONE_NUM)) {
        if (VprocStopped == 1U) {
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            UINT16 ViewZoneOrder[NUM_VPROC_MAX_CHAN] = {0};
            cmd_vproc_multi_chan_proc_order_t *ChanProcOrder = HL_DefCtxCmdBufPtrVpcChOrd;

            /* Reorder the process order */
            (void)dsp_osal_memset(ViewZoneOrder, 0x0, (sizeof(UINT16)*NUM_VPROC_MAX_CHAN));
            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&ChanProcOrder, &CmdBufferAddr);
            HL_UpdateVprocChanProcOrder((UINT8)i,
                                        GrpDisableVZMask,
                                        ChanProcOrder,
                                        0U/*DisableVproc*/,
                                        ViewZoneOrder);

            /* update viewzone master info */
            HL_GetViewZoneInfoLock(MasterVZId, &ViewZoneInfo);
            ViewZoneInfo->IsMaster = 0U;
            HL_GetViewZoneInfoUnLock(MasterVZId);
            HL_GetViewZoneInfoLock(ViewZoneOrder[0], &ViewZoneInfo);
            ViewZoneInfo->IsMaster = 1U;
            HL_GetViewZoneInfoUnLock(ViewZoneOrder[0]);

            if (ChanProcOrder->num_of_vproc_channel > 1U) {
                Rval = AmbaHL_CmdVprocMultiChanProcOrder(WriteMode, ChanProcOrder);
                if (Rval != OK) {
                    AmbaLL_LogUInt5("AmbaHL_CmdVprocMultiChanProcOrder fail", 0U, 0U, 0U, 0U, 0U);
                }
            }
            HL_RelCmdBuffer(CmdBufferId);
            /* Process group write commands */
            Rval = DSP_ProcessBufCmds(WriteMode);
            if (Rval != OK) {
                AmbaLL_LogUInt5("[LvCtrl][%d] DSP_ProcessBufCmds fail", __LINE__, 0U, 0U, 0U, 0U);
            }
        } else {
            /*
             * If some vproc of effect group did not stop,
             * but user trying to stop master vin,
             * then we need to change group order
             *
             * If some vproc does not timeout,
             * It already changed master.
             */
            Rval = HL_LvCtrDisUptVprocGrp(i,
                                          MasterVZId,
                                          MasterVinId,
                                          GrpDisableVZNum,
                                          GrpDisableVZId);
            if (Rval != OK) {
                AmbaLL_LogUInt5("[LvCtrl][%d] HL_LvCtrDisUptVprocGrp fail", __LINE__, 0U, 0U, 0U, 0U);
            }
        }
    }

    return Rval;
}

static inline UINT32 HL_LvBindDecVprocToVirtVin(UINT16 DecIdx)
{
    UINT32 Rval = OK;
    CTX_VID_DEC_INFO_s VidDecInfo = {0};

    HL_GetVidDecInfo(HL_MTX_OPT_ALL, DecIdx, &VidDecInfo);
    if (VidDecInfo.CurrentBind == BIND_VIDEO_DECODE_TO_VPROC) {
        UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
        cmd_binding_cfg_t *FpBindCfg = HL_DefCtxCmdBufPtrBind;

        /* wait vdsp start, to make sure cmd won't be delayed */
        Rval = DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP0_EVENT, 2, WAIT_INTERRUPT_TIMEOUT);
        if (Rval != OK) {
            AmbaLL_LogUInt5("LvBind Wait VDSP0 fail %u %d", Rval, __LINE__, 0U, 0U, 0U);
        }

        /* UnBind first */
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&FpBindCfg, &CmdBufferAddr);
        Rval = HL_FillFpBindSetup(FpBindCfg, BIND_VIDEO_DECODE_TO_VPROC, 0U/*IsBind*/,
                                  (UINT16)DecIdx/*SrcId -> src_ch_id*/,
                                  0U/*SrcPinId -> xx*/,
                                  VidDecInfo.ViewZoneId/*StrmId -> dst_ch_id*/);
        if (Rval != OK) {
            AmbaLL_LogUInt5("LvBind [%d]FpBind filling fail", __LINE__, 0U, 0U, 0U, 0U);
        } else {
            (void)AmbaHL_CmdDspBindingCfg(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, FpBindCfg);
        }
        HL_RelCmdBuffer(CmdBufferId);

        /* wait bind command take effect */
        Rval = DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP0_EVENT, 2, WAIT_INTERRUPT_TIMEOUT);
        if (Rval != OK) {
            AmbaLL_LogUInt5("LvBind Wait VDSP0 fail %u %d", Rval, __LINE__, 0U, 0U, 0U);
        }

        /* Bind VirtVin and Vproc */
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&FpBindCfg, &CmdBufferAddr);
        Rval = HL_FillFpBindSetup(FpBindCfg, BIND_STILL_PROC, 1U/*IsBind*/,
                                  VidDecInfo.YuvInVirtVinId + AMBA_DSP_MAX_VIN_NUM/*SrcId -> src_fp_id*/,
                                  0U/*SrcPinId*/,
                                  VidDecInfo.ViewZoneId/*StrmId -> dst_ch_id*/);
        if (Rval != OK) {
            AmbaLL_LogUInt5("LvBind [%d]FpBind filling fail", __LINE__, 0U, 0U, 0U, 0U);
            HL_RelCmdBuffer(CmdBufferId);
        } else {
            (void)AmbaHL_CmdDspBindingCfg(AMBA_DSP_CMD_VCAP_NORMAL_WRITE, FpBindCfg);
            HL_RelCmdBuffer(CmdBufferId);
            HL_GetVidDecInfo(HL_MTX_OPT_GET, DecIdx, &VidDecInfo);
            VidDecInfo.CurrentBind = BIND_STILL_PROC;
            HL_SetVidDecInfo(HL_MTX_OPT_SET, DecIdx, &VidDecInfo);

            /* wait bind command take effect */
            Rval = DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP0_EVENT, 2, WAIT_INTERRUPT_TIMEOUT);
            if (Rval != OK) {
                AmbaLL_LogUInt5("LvBind Wait VDSP0 fail %u %d", Rval, __LINE__, 0U, 0U, 0U);
            }
        }
    }
    return Rval;
}

static inline UINT32 HL_LvCtrDisOnVinStop(UINT16 NumViewZone,
                                          const UINT16 *pViewZoneId,
                                          const UINT8 *pEnable)
{
    UINT32 Rval = OK;
    UINT16 i, ViewZoneId, VinId;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_VIN_INFO_s VinInfo;
    UINT8 DspVinState;
    UINT8 WriteMode;
    UINT16 DecIdx = 0U;
    UINT8 IsVirtVin;

    for (i=0U; (i<NumViewZone)&&(pEnable[i]==0U); i++) {
        ViewZoneId = pViewZoneId[i];
        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
        if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) {
            if (ViewZoneInfo->IsEffectChanMember == 1U) {
                DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &DecIdx);
                Rval = HL_LvBindDecVprocToVirtVin(DecIdx);
                if (Rval != OK) {
                    AmbaLL_LogUInt5("HL_LvCtrDisOnVinStop LvBind fail %d", Rval, 0U, 0U, 0U, 0U);
                }
            }
        } else if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON) {
            //skip
            continue;
        } else {
            //do nothing
        }
        (void)HL_GetViewZoneVinId(ViewZoneId, &VinId);
        (void)HL_IsVirtualVinIdx(VinId, &IsVirtVin);

        if (IsVirtVin == 0U) {
            DspVinState = DSP_GetVinState((UINT8)VinId);
            WriteMode = HL_GetVin2CmdNormalWrite(VinId);

            /* Process Vin stop */
            if (DSP_RAW_CAP_STATUS_VIDEO == DspVinState) {
                /* Stop Vin */
                if (OK != HL_PollingEncodeTimerMode((UINT8)VinId, WriteMode, 1U/*TimerScale*/, MODE_SWITCH_TIMEOUT)) {
                    AmbaLL_LogUInt5("HL_PollingEncodeTimerMode fail", 0U, 0U, 0U, 0U, 0U);
                    Rval |= DSP_ERR_0000;
                } else {
                    HL_GetVinInfo(HL_MTX_OPT_GET, VinId, &VinInfo);
                    VinInfo.VinCtrl.VinState = DSP_VIN_STATUS_TIMER;
                    HL_SetVinInfo(HL_MTX_OPT_SET, VinId, &VinInfo);
                }
            } else if (DSP_RAW_CAP_STATUS_TIMER == DspVinState) {
                AmbaLL_LogUInt5("This Vin has already been in timer mode, do nothing", 0U, 0U, 0U, 0U, 0U);
                /* This Vin has already been in timer mode, do nothing */
            } else {
                AmbaLL_LogUInt5("Unknown dsp mode[%d]", DspVinState, 0U, 0U, 0U, 0U);
                Rval |= DSP_ERR_0000;
            }
        } else {
            HL_GetVinInfo(HL_MTX_OPT_GET, VinId, &VinInfo);
            VinInfo.VinCtrl.VinState = DSP_VIN_STATUS_CTX_RESET;
            HL_SetVinInfo(HL_MTX_OPT_SET, VinId, &VinInfo);
        }
    }

    return Rval;
}


static inline UINT32 HL_LvCtrDisOnVprocStopPostproc(UINT16 NumViewZone,
                                                    const UINT16 *pViewZoneId,
                                                    const UINT8 *pEnable,
                                                    const UINT32 *pVprocDisableBit)
{
    UINT32 Rval = OK;
    UINT16 i, ViewZoneId;
    CTX_VPROC_INFO_s VprocInfo;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    for (i=0U; (i<NumViewZone)&&(pEnable[i]==0U); i++) {

        ViewZoneId = pViewZoneId[i];
        if (0U == DSP_GetBit(*pVprocDisableBit, ViewZoneId, 1U)) {
            continue;
        }

        if (DSP_VPROC_STATUS_INVALID == DSP_GetVprocState((UINT8)ViewZoneId)) {
            continue;
        }

        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
        if (ViewZoneInfo->Pipe == DSP_DRAM_PIPE_RAW_ONLY) {
            continue;
        }

        Rval = DSP_WaitVprocState((UINT8)ViewZoneId, DSP_VPROC_STATUS_TIMER, MODE_SWITCH_TIMEOUT, 1U);

        if (Rval == OK) {
            HL_GetVprocInfo(HL_MTX_OPT_GET, ViewZoneId, &VprocInfo);
            VprocInfo.Status = DSP_VPROC_STATUS_TIMER;
            HL_SetVprocInfo(HL_MTX_OPT_SET, ViewZoneId, &VprocInfo);

            /* 1. Must bind back to BIND_STILL_PROC first, at HL_LvBindDecVprocToVirtVin
             * 2. reset VidDecInfo.ExtYuvBuf if needed
             *    user need to set VidDecInfo.ExtYuvBuf by AmbaDSP_LiveviewFeedYuvData every time,
             *    to clarify he need yuv img at dec2vproc start */
            if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) {
                CTX_VID_DEC_INFO_s VidDecInfo = {0};
                UINT32 DecIdx = 0U;

                DSP_Bit2Idx(ViewZoneInfo->SourceVin, &DecIdx);
                HL_GetVidDecInfo(HL_MTX_OPT_GET, (UINT16)DecIdx, &VidDecInfo);
                (void)dsp_osal_memset(&VidDecInfo.ExtYuvBuf, 0, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
                (void)dsp_osal_memset(&VidDecInfo.LastYuvBuf, 0, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
                VidDecInfo.YuvFrameCnt = 0U;
                VidDecInfo.ExtYuvBufIdx = 0U;
                VidDecInfo.FirstIsoCfgIssued = 0U;
                HL_SetVidDecInfo(HL_MTX_OPT_SET, (UINT16)DecIdx, &VidDecInfo);
            } else if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_420) ||
                       (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_422)) {

                HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
                (void)dsp_osal_memset(&ViewZoneInfo->ExtYuvBuf, 0, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
                ViewZoneInfo->YuvFrameCnt = 0U;
                ViewZoneInfo->ExtYuvBufIdx = 0U;
                HL_GetViewZoneInfoUnLock(ViewZoneId);
            } else {
                //
            }
            /* This function only called when all group has stopped */
            HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
            ViewZoneInfo->IsMaster = 0U;
            HL_GetViewZoneInfoUnLock(ViewZoneId);
        } else {
            AmbaLL_LogUInt5("DSP_WaitVprocState fail Vproc[%d] TargetState[%d]", ViewZoneId, DSP_VPROC_STATUS_TIMER, 0U, 0U, 0U);
        }
    }

    return Rval;
}

static inline UINT32 HL_LvCtrOnDisable(UINT16 NumViewZone,
                                       const UINT16 *pViewZoneId,
                                       const UINT8 *pEnable,
                                       const CTX_RESOURCE_INFO_s *pResource,
                                       UINT32 *pVprocDisableBit)
{
    UINT32 Rval = OK;
    UINT8 ProfState;
#ifdef SUPPORT_DSP_SUSPEND
    CTX_RESOURCE_INFO_s *TmpResource = HL_CtxResInfoPtr;
#endif

    ProfState = DSP_GetProfState();
    if (DSP_PROF_STATUS_CAMERA == ProfState) {
        /* Switch Vout YuvStream to background if all its source View are disabled */
        (void)HL_LvCtrDisOnVoutSrcCtrl(NumViewZone,
                                       pViewZoneId,
                                       pEnable,
                                       pResource);

        /* Stop Vproc only when ViewZoneOI doesn't partially contain effect channel members */
        (void)HL_LvCtrChkVprocOI(NumViewZone,
                                 pViewZoneId,
                                 pResource,
                                 pVprocDisableBit);

        (void)HL_LvCtrDisOnVprocStop(NumViewZone,
                                     pViewZoneId,
                                     pEnable,
                                     pVprocDisableBit);

        (void)HL_LvCtrDisOnVprocStopPostproc(NumViewZone,
                                             pViewZoneId,
                                             pEnable,
                                             pVprocDisableBit);

        /* Stop Vin */
        Rval = HL_LvCtrDisOnVinStop(NumViewZone,
                                    pViewZoneId,
                                    pEnable);

#ifdef SUPPORT_DSP_SUSPEND
        if (Rval == OK) {
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_dsp_suspend_profile_t *SusOpMode = NULL;

            /* Lock still JPEG encode resource to prevent DSP assert for background JPEG encode */
//                      AmbaDSP_EncodeHAL_Take(HAL_IMAGE_ENCODE, SCAP_EVENT_TIMEOUT);
            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&SusOpMode, &CmdBufferAddr);
            (void)AmbaHL_CmdDspSuspendProfile(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, SusOpMode);
            HL_RelCmdBuffer(CmdBufferId);

            if (OK != DSP_WaitProfState(DSP_PROF_STATUS_IDLE, MODE_SWITCH_TIMEOUT)) {
                AmbaLL_LogUInt5("DSP_WaitProfState[%d] fail", DSP_PROF_STATUS_IDLE, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0003;
            } else {
                /* Reset certain information */
                HL_GetResourceLock(&TmpResource);
                TmpResource->ProfWorkSize = 0U;
                HL_GetResourceUnLock();
            }
            /* Release still JPEG encode resource */
//                        AmbaDSP_EncodeHAL_Give(HAL_IMAGE_ENCODE);
        } else {
            /* Polling timer mode failed or invalid DSP Vin status */
        }
#endif
    } else if (DSP_PROF_STATUS_INVALID == ProfState) {
#if 0
        UINT16 i;
        //FIXME unreasonable condition
        for (i=0; i<AMBA_DSP_MAX_VIN_NUM; i++) {
            if (1U == DSP_GetU16Bit(pResource->VinBit, i, 1U)) {
                Rval = HL_PollingEncodeTimerMode((UINT8)i,
                                                 AMBA_DSP_CMD_VDSP_NORMAL_WRITE,
                                                 1U/*TimerScale*/,
                                                 MODE_SWITCH_TIMEOUT,
                                                 AMBA_DSP_CMD_DEFAULT_WRITE/*WaitType*/);
                if (Rval != OK) {
                    AmbaLL_LogUInt5("AmbaHL_PollingEncodeTimerMode fail", 0U, 0U, 0U, 0U, 0U);
                    Rval = DSP_ERR_0000;
                } else {
                    /* DO NOTHING */
                }
            } else {
                /* Inactive Vin, DO NOTHING */
            }
        }
#endif
    } else {
        /* DO NOTHING */
    }

    return Rval;
}

static inline UINT32 HL_LvCtrEnableRescChg(UINT16 NumViewZone,
                                           const UINT16 *pViewZoneId,
                                           const UINT8 *pEnable,
                                           UINT8 *pRescChged)
{
    UINT32 Rval = OK;
    UINT16 i, VinId;
    UINT8 IsVirtVin, ExitILoop, PureStillProcessed, IsEffectOn;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_VIN_INFO_s VinInfo;
    UINT32 ChgLine = 0U;
    UINT8 VinCompact, VinCompressed;

    /* Check Vin RescChg */
    ExitILoop = 0U;
    for (i=0U; ((i<NumViewZone)&&(pEnable[i] == 1U)); i++) {
        HL_GetViewZoneInfoPtr(pViewZoneId[i], &ViewZoneInfo);
        if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) ||
            (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON)) {
            continue;
        }

        DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &VinId);

        (void)HL_IsVirtualVinIdx(VinId, &IsVirtVin);
        HL_GetVinInfo(HL_MTX_OPT_ALL, VinId, &VinInfo);
        if ((IsVirtVin == (UINT8)0U) &&
            (VinInfo.InputFormat == DSP_VIN_INPUT_RGB_RAW)) {
            HL_DisassembleVinCompression(ViewZoneInfo->IkCompression, &VinCompressed, &VinCompact);

            /* Compression changed */
            if (VinInfo.CfaCompressed != VinCompressed) {
                *pRescChged = (UINT8)1U;; ChgLine = __LINE__; ExitILoop = 1U;
            }

            /* CfaCompact changed */
            if (VinInfo.CfaCompact != VinCompact) {
                *pRescChged = (UINT8)1U;; ChgLine = __LINE__; ExitILoop = 1U;
            }

            /* SensorMode changed */
            if (VinInfo.SensorMode != ViewZoneInfo->IkSensorMode) {
                *pRescChged = (UINT8)1U;; ChgLine = __LINE__; ExitILoop = 1U;
            }

            /* FrameRate Changed */
            // TBD
        }
        if (ExitILoop == 1U) {
            break;
        }
    }

    /* Pure still processed means that changing back to liveview must re-issue
     * vproc-config which will assign duplicate resource for effect-chan */
    PureStillProcessed = HL_PureStillProcessed();
    IsEffectOn = (HL_GetEffectChannelEnable() > 0U)? 1U: 0U;
    if ((PureStillProcessed == 1U) && (IsEffectOn == 1U)) {
        *pRescChged = (UINT8)1U; ChgLine = __LINE__;
    }

    AmbaLL_LogUInt5("LvCtrEnableRescChg[%d] %d", *pRescChged, ChgLine, 0U, 0U, 0U);

    return Rval;
}

static inline UINT32 HL_LvCtrEnablePreprocStlStop(void)
{
    UINT16 ViewZoneId;
    UINT32 Rval = OK;
    CTX_STILL_INFO_s StlInfo = {0};

    HL_GetStlInfo(HL_MTX_OPT_ALL, &StlInfo);

    if (StlInfo.RawInVprocStatus != STL_VPROC_STATUS_IDLE) {
        UINT8 WriteMode;
        UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
        cmd_binding_cfg_t *FpBindCfg = HL_DefCtxCmdBufPtrBind;

        ViewZoneId = StlInfo.RawInVprocId;
        WriteMode = HL_GetVin2CmdNormalWrite(StlInfo.RawInVirtVinId + AMBA_DSP_MAX_VIN_NUM);

        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&FpBindCfg, &CmdBufferAddr);
        Rval = HL_FillFpBindSetup(FpBindCfg, BIND_STILL_PROC, 0U/*IsBind*/,
                StlInfo.RawInVirtVinId + AMBA_DSP_MAX_VIN_NUM, 0U/*SrcPinId*/, ViewZoneId);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[%d]FpBind filling fail", __LINE__, 0U, 0U, 0U, 0U);
        } else {
            (void)AmbaHL_CmdDspBindingCfg(WriteMode, FpBindCfg);
        }
        HL_RelCmdBuffer(CmdBufferId);
    }
    if (StlInfo.YuvInVprocStatus != STL_VPROC_STATUS_IDLE) {
        UINT8 WriteMode;
        UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
        cmd_binding_cfg_t *FpBindCfg = HL_DefCtxCmdBufPtrBind;

        ViewZoneId = StlInfo.YuvInVprocId;
        WriteMode = HL_GetVin2CmdNormalWrite(StlInfo.YuvInVirtVinId + AMBA_DSP_MAX_VIN_NUM);

        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&FpBindCfg, &CmdBufferAddr);
        Rval = HL_FillFpBindSetup(FpBindCfg, BIND_STILL_PROC, 0U/*IsBind*/,
                StlInfo.YuvInVirtVinId + AMBA_DSP_MAX_VIN_NUM, 0U/*SrcPinId*/, ViewZoneId);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[%d]FpBind filling fail", __LINE__, 0U, 0U, 0U, 0U);
        } else {
            (void)AmbaHL_CmdDspBindingCfg(WriteMode, FpBindCfg);
        }
        HL_RelCmdBuffer(CmdBufferId);
    }

    if (StlInfo.YuvInVprocId == StlInfo.RawInVprocId) {
        if ((StlInfo.RawInVprocStatus != STL_VPROC_STATUS_IDLE) ||
            (StlInfo.YuvInVprocStatus != STL_VPROC_STATUS_IDLE)) {
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_vproc_stop_t *VprocStop = HL_DefCtxCmdBufPtrVpcStop;

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VprocStop, &CmdBufferAddr);
            ViewZoneId = StlInfo.RawInVprocId;
            VprocStop->channel_id = (UINT8)ViewZoneId;
            VprocStop->stop_cap_seq_no = 0U; //obsoleted
            VprocStop->reset_option = 0U; //obsoleted

            Rval = AmbaHL_CmdVprocStop(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, VprocStop);
            HL_RelCmdBuffer(CmdBufferId);
            if (Rval != OK) {
                AmbaLL_LogUInt5("AmbaHL_CmdVprocStop fail", 0U, 0U, 0U, 0U, 0U);
            }
            (void)DSP_WaitVprocState((UINT8)ViewZoneId, DSP_VPROC_STATUS_TIMER, MODE_SWITCH_TIMEOUT, 1U);

            HL_GetStlInfo(HL_MTX_OPT_GET, &StlInfo);
            StlInfo.RawInVprocStatus = STL_VPROC_STATUS_STOPPED;
            StlInfo.YuvInVprocStatus = STL_VPROC_STATUS_STOPPED;
            HL_SetStlInfo(HL_MTX_OPT_SET, &StlInfo);
        } else {
            //vproc already at idle state
        }
    } else {
        if (StlInfo.RawInVprocStatus != STL_VPROC_STATUS_IDLE) {
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_vproc_stop_t *VprocStop = HL_DefCtxCmdBufPtrVpcStop;

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VprocStop, &CmdBufferAddr);
            ViewZoneId = StlInfo.RawInVprocId;
            VprocStop->channel_id = (UINT8)ViewZoneId;
            VprocStop->stop_cap_seq_no = 0U; //obsoleted
            VprocStop->reset_option = 0U; //obsoleted

            Rval = AmbaHL_CmdVprocStop(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, VprocStop);
            HL_RelCmdBuffer(CmdBufferId);
            if (Rval != OK) {
                AmbaLL_LogUInt5("AmbaHL_CmdVprocStop fail", 0U, 0U, 0U, 0U, 0U);
            }
            (void)DSP_WaitVprocState((UINT8)ViewZoneId, DSP_VPROC_STATUS_TIMER, MODE_SWITCH_TIMEOUT, 1U);

            HL_GetStlInfo(HL_MTX_OPT_GET, &StlInfo);
            StlInfo.RawInVprocStatus = STL_VPROC_STATUS_STOPPED;
            HL_SetStlInfo(HL_MTX_OPT_SET, &StlInfo);
        }
        if (StlInfo.YuvInVprocStatus != STL_VPROC_STATUS_IDLE) {
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_vproc_stop_t *VprocStop = HL_DefCtxCmdBufPtrVpcStop;

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VprocStop, &CmdBufferAddr);
            ViewZoneId = StlInfo.YuvInVprocId;
            VprocStop->channel_id = (UINT8)ViewZoneId;
            VprocStop->stop_cap_seq_no = 0U; //obsoleted
            VprocStop->reset_option = 0U; //obsoleted

            Rval = AmbaHL_CmdVprocStop(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, VprocStop);
            HL_RelCmdBuffer(CmdBufferId);
            if (Rval != OK) {
                AmbaLL_LogUInt5("AmbaHL_CmdVprocStop fail", 0U, 0U, 0U, 0U, 0U);
            }
            (void)DSP_WaitVprocState((UINT8)ViewZoneId, DSP_VPROC_STATUS_TIMER, MODE_SWITCH_TIMEOUT, 1U);

            HL_GetStlInfo(HL_MTX_OPT_GET, &StlInfo);
            StlInfo.YuvInVprocStatus = STL_VPROC_STATUS_STOPPED;
            HL_SetStlInfo(HL_MTX_OPT_SET, &StlInfo);
        }
    }

    return Rval;
}

static inline void HL_LvCtrEnableChkIsPartialVz(UINT16 NumViewZone,
                                                const UINT16 *pViewZoneId,
                                                const UINT32 *pViewZoneOI,
                                                UINT8 *pIsPartialCtrl)
{
    UINT32 ViewZoneOI = *pViewZoneOI;
    UINT32 TargetViewZoneOI = *pViewZoneOI;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);

    /* Setup Vproc only when ViewZoneOI doesn't partially contain effect channel members */
    (void)HL_LvCtrChkVprocOI(NumViewZone,
                             pViewZoneId,
                             Resource,
                             &TargetViewZoneOI);

    /* check is partial enable */
    if (ViewZoneOI != TargetViewZoneOI) {
        *pIsPartialCtrl = 1U;
    } else {
        *pIsPartialCtrl = 0U;
    }
//    AmbaLL_LogUInt5("HL_LvCtrEnableChkIsPartialVz ViewZoneOI:0x%x TargetViewZoneOI:0x%x pIsPartialCtrl:%u",
//            ViewZoneOI, TargetViewZoneOI, *pIsPartialCtrl, 0U, 0U);
}

static inline UINT32 HL_LvCtrEnPrepOnNoneCameraMode(const UINT32 ViewZoneOI,
                                                    UINT8 *pProfileSwitch,
                                                    UINT8 *pWriteMode)
{
    UINT32 Rval;

    *pProfileSwitch = 1U;
    *pWriteMode = AMBA_DSP_CMD_DEFAULT_WRITE;
    Rval = HL_SwitchProfile(DSP_PROF_STATUS_CAMERA, SWITCH_PROF_PREPROC);
    if (Rval == OK) {
        UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;
        /* Profile switch enables resource reconfiguration */

        /* The flow of reset liveview */
        HL_LiveviewReset();
        /* Reset all context for certain ViewZoneOI */
        HL_CtxLvDataPathReset(ViewZoneOI);
        /* Reset all context */
        HL_CtxLvReset();
        if (pIsSimilarEnabled != NULL) {
            if (1U == IsSimilarEnabled()) {
                SIM_CtxLvReset();
            }
        }
    }

    return Rval;
}

static inline UINT32 HL_LvCtrEnPrepOnCameraMode(UINT16 NumViewZone,
                                                const UINT16 *pViewZoneId,
                                                const UINT8 *pEnable,
                                                UINT8 *pNeedLvChg,
                                                UINT8 *pWriteMode)
{
    UINT32 Rval = OK;
    UINT16 i;
    UINT16 ViewZoneId;
    UINT16 VinId;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT8 DspVinState;

    for (i = 0U; (i < NumViewZone) && (pEnable[i] == 1U); i++) {
        ViewZoneId = pViewZoneId[i];
        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
        if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) ||
            (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON)) {
            continue;
        }

        if ((ViewZoneInfo->SourceTdIdx != VIEWZONE_SRC_TD_IDX_NULL) &&
            (ViewZoneInfo->SourceTdIdx != 0U)) {
            //Force Chg...
        } else {
            DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &VinId);
            DspVinState = DSP_GetVinState((UINT8)VinId);

            if (DSP_RAW_CAP_STATUS_VIDEO == DspVinState) {
                *pNeedLvChg = 0U;
                Rval = OK;
            } else if (DSP_RAW_CAP_STATUS_TIMER != DspVinState) {
                *pWriteMode = HL_GetVin2CmdNormalWrite(VinId);
                if (OK != HL_PollingEncodeTimerMode((UINT8)VinId, *pWriteMode, 1/*TimerScale*/, MODE_SWITCH_TIMEOUT)) {
                    AmbaLL_LogUInt5("PollingTimerMode fail", 0U, 0U, 0U, 0U, 0U);
                    *pNeedLvChg = 0U;
                    Rval = DSP_ERR_0000;
                }
            } else {
                Rval = OK;
            }
        }
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err][0x%X][%d] HL_LvCtrEnablePreproc[%d]",
                Rval, __LINE__, 0U, 0U, 0U);
    }
    /* If user issue LiveviewCtrl ENABLE when still-vproc is not IDLE,
     * it means there was a still-process and still-vproc is activated
     * Here we stop that still-vproc and reset the status of it */
    Rval = HL_LvCtrEnablePreprocStlStop();
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err][0x%X][%d] HL_LvCtrEnablePreprocStlStop[%d]",
                Rval, __LINE__, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT8 HL_LvCtrEnablePreprocChkEffReset(UINT16 NumViewZone,
                                                     const UINT16 *pViewZoneId,
                                                     const UINT8 IsPartialEnable)
{
    UINT8 Rval = 0U;
    UINT16 i, GrpNum = 0U, VporcGrpId;
    UINT32 EnaVprocGrpMask = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    (void)HL_GetVprocGroupNum(&GrpNum);
    HL_GetResourcePtr(&Resource);
    if ((HL_GetEffectChannelEnable() > 0U) && (IsPartialEnable == 0U)) {
        //single group need to reset
        if (GrpNum == 1U) {
            Rval = 1U;
        } else {    //multiple group need to check is anyone else alive
            Rval = 1U;
            for (i=0U; i<NumViewZone; i++) {
                (void)HL_GetVprocGroupIdx(pViewZoneId[i], &VporcGrpId, 0U/*IsStlProc*/);
#ifdef DEBUG_LV_CTRL_DISABLE
                AmbaLL_LogUInt5("HL_LvCtrEnablePreprocChkEffReset pViewZoneId[i]:%u VporcGrpId:%u",
                        pViewZoneId[i], VporcGrpId, 0U, 0U, 0U);
#endif
                DSP_SetBit(&EnaVprocGrpMask, VporcGrpId);
            }
#ifdef DEBUG_LV_CTRL_DISABLE
            AmbaLL_LogUInt5("HL_LvCtrEnablePreprocChkEffReset EnaVprocGrpMask:0x%x",
                    EnaVprocGrpMask, 0U, 0U, 0U, 0U);
#endif
            for (i=0U; i<Resource->MaxViewZoneNum; i++) {
                if (0U == DSP_GetBit(Resource->ViewZoneActiveBit, i, 1U)) {
                    continue;
                }
                HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
                if (VIEWZONE_VIN_TIMEOUT == ViewZoneInfo->TimeoutState) {
                    continue;
                }

                (void)HL_GetVprocGroupIdx(i, &VporcGrpId, 0U/*IsStlProc*/);
#ifdef DEBUG_LV_CTRL_DISABLE
                AmbaLL_LogUInt5("HL_LvCtrEnablePreprocChkEffReset i:%u VporcGrpId:%u",
                        i, VporcGrpId, 0U, 0U, 0U);
#endif
                //if there has another alive viewzone
                if (0U == DSP_GetBit(EnaVprocGrpMask, VporcGrpId, 1U)) {
                    Rval = 0U;
                    break;
                }
            }
        }
    } else {
        Rval = 0U;
    }
    return Rval;
}

static inline UINT32 HL_LvCtrEnablePreproc(UINT16 NumViewZone,
                                           const UINT16 *pViewZoneId,
                                           const UINT8 *pEnable,
                                           const UINT32 ViewZoneOI,
                                           const UINT8 IsPartialEnable,
                                           UINT8 *pProfileSwitch,
                                           UINT8 *pNeedLvChg,
                                           UINT8 *pWriteMode)
{
    UINT8 DspOpModeInit, DspOpModeIdle, DspOpModeCamera;
    UINT8 DspOpModePlayback, RescChged, IsEffectNeedReset;
    UINT32 Rval = OK;

    DspOpModeInit = (DSP_GetProfState() == DSP_PROF_STATUS_INVALID)? 1U: 0U;
    DspOpModeIdle = (DSP_GetProfState() == DSP_PROF_STATUS_IDLE)? 1U: 0U;
    DspOpModeCamera = (DSP_GetProfState() == DSP_PROF_STATUS_CAMERA)? 1U: 0U;
    DspOpModePlayback = (DSP_GetProfState() == DSP_PROF_STATUS_PLAYBACK)? 1U: 0U;
    RescChged = (HL_GetRescState() == HL_RESC_CONFIGED)? 1U: 0U;
    IsEffectNeedReset = HL_LvCtrEnablePreprocChkEffReset(NumViewZone, pViewZoneId, IsPartialEnable);

    AmbaLL_LogUInt5("ProfState:%u RescChged:%u ViewZoneOI:%x IsPartialEnable:%u IsEffectNeedReset:%u",
            DSP_GetProfState(), RescChged, ViewZoneOI, IsPartialEnable, IsEffectNeedReset);
    if (DspOpModeInit == 0U) {
        /* Final RescChg check */
        if (RescChged == (UINT8)0U) {
            (void)HL_LvCtrEnableRescChg(NumViewZone,
                                        pViewZoneId,
                                        pEnable,
                                        &RescChged);
        }
    }

    if (DspOpModeInit == 1U) {
        DSP_ClrDefCfgBuffer();
        *pWriteMode = AMBA_DSP_CMD_DEFAULT_WRITE;
    } else if ((DspOpModeIdle == 1U) ||
               (DspOpModePlayback == 1U) ||
               (RescChged == 1U) ||
               (IsEffectNeedReset == 1U)) {
        Rval = HL_LvCtrEnPrepOnNoneCameraMode(ViewZoneOI,
                                              pProfileSwitch,
                                              pWriteMode);
    } else if (DspOpModeCamera == 1U) {
        Rval = HL_LvCtrEnPrepOnCameraMode(NumViewZone,
                                          pViewZoneId,
                                          pEnable,
                                          pNeedLvChg,
                                          pWriteMode);
    } else {
        //DO NOTHING
    }

    return Rval;
}

static inline UINT32 HL_LvCtrEnableOnStatusUpdate(UINT16 NumViewZone,
                                                  const UINT16 *pViewZoneId,
                                                  const UINT8 *pEnable,
                                                  const UINT8 IsInitMode)
{
    UINT32 Rval = OK;
    UINT16 i, ViewZoneId, VinId;
    CTX_VIN_INFO_s VinInfo;
    CTX_VPROC_INFO_s VprocInfo;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT8 IsVirtVin = (UINT8)0U;

    for (i=0U; (i<NumViewZone)&&(pEnable[i]==1U); i++) {
        /* Update Vproc status as Idle2Run */
        ViewZoneId = pViewZoneId[i];
        HL_GetVprocInfo(HL_MTX_OPT_GET, ViewZoneId, &VprocInfo);
        if ((VprocInfo.Status == DSP_VPROC_STATUS_TIMER) ||
            (VprocInfo.Status == DSP_VPROC_STATUS_INVALID) ||
            (IsInitMode == 1U)) {
            VprocInfo.Status = DSP_VPROC_STATUS_IDLE2RUN;
        }
        HL_SetVprocInfo(HL_MTX_OPT_SET, ViewZoneId, &VprocInfo);

        /* Update Vin status as Idle2Run */
        HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
        if ((ViewZoneInfo->InputFromMemory != VIN_SRC_FROM_DEC) &&
            (ViewZoneInfo->InputFromMemory != VIN_SRC_FROM_RECON)) {
            (void)HL_GetViewZoneVinId(ViewZoneId, &VinId);

            HL_GetVinInfo(HL_MTX_OPT_GET, VinId, &VinInfo);
            (void)HL_IsVirtualVinIdx(VinId, &IsVirtVin);

            if ((IsVirtVin == (UINT8)0U) &&
                (ViewZoneInfo->SourceTdIdx != VIEWZONE_SRC_TD_IDX_NULL) &&
                (ViewZoneInfo->SourceTdIdx != 0U)) {
                /* VirtVin can't be restart, so we only set CapSeqNum = 0 */
                if (VinInfo.VinCtrl.VinState == DSP_VIN_STATUS_INVALID) {
                    VinInfo.VinCtrl.VinState = DSP_VIN_STATUS_IDLE2RUN;
                }

                VinInfo.VinCtrl.RawSeqWp = 0xFFU;
                ViewZoneInfo->YuvInput.RawSeqWp = 0xFFU;
                ViewZoneInfo->PymdInput.RawSeqWp = 0xFFU;
                ViewZoneInfo->TimeoutState = VIEWZONE_VIN_NORMAL;
            } else {
                if ((VinInfo.VinCtrl.VinState == DSP_VIN_STATUS_TIMER) ||
                    (VinInfo.VinCtrl.VinState == DSP_VIN_STATUS_INVALID) ||
                    (VinInfo.VinCtrl.VinState == DSP_VIN_STATUS_TIMEOUT) ||
                    (IsInitMode == 1U)) {
                    VinInfo.VinCtrl.VinState = DSP_VIN_STATUS_IDLE2RUN;
                    VinInfo.VinCtrl.RawSeqWp = 0xFFU;
                    ViewZoneInfo->YuvInput.RawSeqWp = 0xFFU;
                    ViewZoneInfo->PymdInput.RawSeqWp = 0xFFU;
                    ViewZoneInfo->TimeoutState = VIEWZONE_VIN_NORMAL;
                }
            }
            HL_SetVinInfo(HL_MTX_OPT_SET, VinId, &VinInfo);
        }
        HL_GetViewZoneInfoUnLock(ViewZoneId);
    }

    return Rval;
}

static inline UINT32 HL_LvCtrEnableOnVinPostCfg(const CTX_RESOURCE_INFO_s *pResource)
{
    UINT32 Rval;
    UINT16 VinId, VinPostCfgBitMask = 0U;
    CTX_VIN_INFO_s VinInfo;

    /* Find Vin needing PostCfg */
    for (VinId=0U; VinId<AMBA_DSP_MAX_VIN_NUM; VinId++) {
        if (0U == DSP_GetU16Bit(pResource->VinBit, VinId, 1U)) {
            continue;
        }

        HL_GetVinInfo(HL_MTX_OPT_ALL, VinId, &VinInfo);
        if (((UINT16)VinInfo.PostCfgType & AMBA_DSP_VIN_CONFIG_POST) > 0U) {
            DSP_SetU16Bit(&VinPostCfgBitMask, VinId);
        }
    }
    /* Handle VinPostCfg first, Wait until VinRegCfged */
    Rval = HL_VinPostCfgHandler(VinPostCfgBitMask);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err][0x%X][%d] HL_VinPostCfgHandler[%d]",
                Rval, __LINE__, VinPostCfgBitMask, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_LvCtrEnableOnPostStatusUpdate(const CTX_RESOURCE_INFO_s *pResource,
                                                      const UINT8 IsPartialEnable)
{
    UINT32 Rval = OK;
    UINT16 VinId, ViewZoneId;
    CTX_VIN_INFO_s VinInfo;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_VPROC_INFO_s VprocInfo;

    /* Wait and update Vin Status */
    for (VinId=0U; VinId<AMBA_DSP_MAX_VIN_NUM; VinId++) {
        if (0U == DSP_GetU16Bit(pResource->VinBit, VinId, 1U)) {
            continue;
        }

        if (OK != DSP_WaitVinState((UINT8)VinId, DSP_RAW_CAP_STATUS_VIDEO, MODE_SWITCH_TIMEOUT)) {
            AmbaLL_LogUInt5("%d DSP_WaitVinState %d fail",
                    __LINE__, DSP_RAW_CAP_STATUS_VIDEO, 0U, 0U, 0U);
        } else {
            HL_GetVinInfo(HL_MTX_OPT_GET, VinId, &VinInfo);
            VinInfo.VinCtrl.VinState = DSP_VIN_STATUS_ACTIVE;
            VinInfo.IsSetPinExtMem = 0U;
            HL_SetVinInfo(HL_MTX_OPT_SET, VinId, &VinInfo);
        }
    }

    /* Wait and update Vproc Status*/
    for (ViewZoneId = 0U; ViewZoneId < pResource->ViewZoneNum; ViewZoneId++) {
        UINT8 NeedWaitVprocStat = 1U;
        UINT16 GrpId = 0U, FirstVprocId = 0U;

        if (1U == DSP_GetBit(pResource->ViewZoneDisableBit, ViewZoneId, 1U)) {
            continue;
        }

        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
        if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) ||
            (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON) ||
            (ViewZoneInfo->Pipe == DSP_DRAM_PIPE_RAW_ONLY)) {
            NeedWaitVprocStat = 0U;
        }

        if (NeedWaitVprocStat == 1U) {
            if (OK != DSP_WaitVprocState((UINT8)ViewZoneId, DSP_VPROC_STATUS_ACTIVE, MODE_SWITCH_TIMEOUT, 1U)) {
                AmbaLL_LogUInt5("[Err][%d] DSP_WaitVprocState %d",
                        __LINE__, DSP_VPROC_STATUS_ACTIVE, 0U, 0U, 0U);
                HL_GetVprocInfo(HL_MTX_OPT_GET, ViewZoneId, &VprocInfo);
                VprocInfo.Status = DSP_VPROC_STATUS_INVALID;
                HL_SetVprocInfo(HL_MTX_OPT_SET, ViewZoneId, &VprocInfo);
            } else {
                UINT8 i;
                HL_GetVprocInfo(HL_MTX_OPT_GET, ViewZoneId, &VprocInfo);
                VprocInfo.Status = DSP_VPROC_STATUS_ACTIVE;
                /* reset ext_mem whenever vproc started */
                for (i=0U; i<DSP_VPROC_PIN_NUM; i++) {
                    VprocInfo.IsSetPinExtMem[i] = 0U;
                }
                HL_SetVprocInfo(HL_MTX_OPT_SET, ViewZoneId, &VprocInfo);

                HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
                ViewZoneInfo->TimeoutState = VIEWZONE_VIN_NORMAL;
                HL_GetViewZoneInfoUnLock(ViewZoneId);
            }
        }

        //setup new master if this is complete enable for each group
        (void)HL_GetVprocGroupIdx(ViewZoneId, &GrpId, 0/*IsStlProc*/);
        HL_GetGroupFirstVprocId(GrpId, &FirstVprocId);
        if ((IsPartialEnable == 0U) && (FirstVprocId != AMBA_DSP_MAX_VIEWZONE_NUM)) {
//            AmbaLL_LogUInt5("HL_LvCtrEnableOnPostStatusUpdate vz:%u FirstVprocId:%u IsMaster:%u -> 1",
//                    ViewZoneId, FirstVprocId, ViewZoneInfo->IsMaster, 0U, 0U);
            HL_GetViewZoneInfoLock(FirstVprocId, &ViewZoneInfo);
            ViewZoneInfo->IsMaster = 1U;
            HL_GetViewZoneInfoUnLock(FirstVprocId);
        }
    }

    return Rval;
}

static inline UINT32 HL_LvCtrOnEnableUpdateVprocAndVin(UINT16 NumViewZone,
                                                       const UINT16 *pViewZoneId,
                                                       const UINT8 *pEnable)
{
    UINT32 Rval = OK;
    UINT8 DspOpModeInit, DspOpModeIdle;

    DspOpModeInit = (DSP_GetProfState() == DSP_PROF_STATUS_INVALID)? 1U: 0U;
    DspOpModeIdle = (DSP_GetProfState() == DSP_PROF_STATUS_IDLE)? 1U: 0U;

    /* booting or from idle */
    if ((DspOpModeInit == 1U) || (DspOpModeIdle == 1U)) {
        /* SystemSetupPreproc */
        Rval = HL_LiveivewSystemPreProcImpl();
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][0x%X][%d] HL_LiveivewSystemPreProcImpl", Rval, __LINE__, 0U, 0U, 0U);
        }
    }
    if (Rval == OK) {
        Rval = HL_LvCtrEnableOnStatusUpdate(NumViewZone,
                                            pViewZoneId,
                                            pEnable,
                                            DspOpModeInit);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][0x%X][%d] HL_LvCtrEnableOnStatusUpdate",
                            Rval, __LINE__, 0U, 0U, 0U);
        }
    }
    return Rval;
}

static inline UINT32 HL_LvCtrOnEnable(UINT16 NumViewZone,
                                      const UINT16 *pViewZoneId,
                                      const UINT8 *pEnable,
                                      const CTX_RESOURCE_INFO_s *pResource,
                                      UINT32 ViewZoneOI)
{
    UINT32 Rval;
    UINT8 WriteMode;
    UINT8 ProfState = 0U, DspVinState, ProfSwitch = 0U, NeedLvChg = 1U, IsPartialEnable = 0U;
    UINT32 TargetViewZoneOI = ViewZoneOI;

    WriteMode = AMBA_DSP_CMD_VDSP_GROUP_WRITE;

    HL_LvCtrEnableChkIsPartialVz(NumViewZone,
                                 pViewZoneId,
                                 &TargetViewZoneOI,
                                 &IsPartialEnable);

    /* Switch dsp to IDLE or TIMER mode */
    Rval = HL_LvCtrEnablePreproc(NumViewZone,
                                 pViewZoneId,
                                 pEnable,
                                 ViewZoneOI,
                                 IsPartialEnable,
                                 &ProfSwitch,
                                 &NeedLvChg,
                                 &WriteMode);
//    AmbaLL_LogUInt5("HL_LvCtrOnEnable ProfSwitch:%u NeedLvChg:%u WriteMode:%u TargetViewZoneOI:%u IsPartialEnable:%u",
//            ProfSwitch, NeedLvChg, WriteMode, TargetViewZoneOI, IsPartialEnable);

    /* Update Vproc and Vin status as Idle2Run */
    if (Rval == OK) {
        Rval = HL_LvCtrOnEnableUpdateVprocAndVin(NumViewZone, pViewZoneId, pEnable);
    }

    /* Issue LiveviewEnable GrpCmd to DefCmdQ/VdspCmdQ */
    if (NeedLvChg == 1U) {
        ProfState = DSP_GetProfState();
        DspVinState = DSP_GetVinState(0U/*VinId*/);

        if ((AMBA_DSP_CMD_DEFAULT_WRITE == WriteMode) &&
            (0U == ProfSwitch)) {
            /*
             * Since we may encounter DspBinary not ready when this API be invoked
             * When Dsp in invalid stage, ignore following cmd filling and issued, and postpone until boot dsp
             */
        } else {
            Rval = HL_LiveviewCmdPrepare(WriteMode, IsPartialEnable);

            if ((Rval == OK) &&
                (1U == ProfSwitch)) {

                //Trigger dsp to read Cmd from DefCmdQ
                Rval = HL_SwitchProfile(DSP_PROF_STATUS_CAMERA, SWITCH_PROF_PROC);
                if (Rval == OK) {
                    HL_SwitchDspSysState(DSP_PROF_STATUS_CAMERA);
                    HL_SetRescState(HL_RESC_SETTLED);
                }

                /* Profile switch enables resource reconfiguration
                 * Move to the time before HL_LiveviewCmdPrepare */
//                        HL_CtxLvDataPathReset(ViewZoneOI);
            }

            /* Process GrpCmdBuf and Wait until Vin and Vproc be activated */
            if (Rval == OK) {
                Rval = DSP_ProcessBufCmds(WriteMode);
            }

            if (Rval == OK) {
                /* Vin Post */
                Rval = HL_LvCtrEnableOnVinPostCfg(pResource);
                if (Rval != OK) {
                    AmbaLL_LogUInt5("[Err][0x%X][%d] HL_LvCtrOnEnable", Rval, __LINE__, 0U, 0U, 0U);
                }
                /* Vproc/Vin status wait and update */
                Rval = HL_LvCtrEnableOnPostStatusUpdate(pResource, IsPartialEnable);
                if (Rval != OK) {
                    AmbaLL_LogUInt5("[Err][0x%X][%d] HL_LvCtrOnEnable", Rval, __LINE__, 0U, 0U, 0U);
                }

                HL_LiveviewVoutSetupImpl(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, VOUT_CFG_TIME_POST_VIN_PROF_SWITCH);
            } else {
                /* Process GrpCmd fail, DO NOTHING */
            }
            AmbaLL_LogUInt5("[SSP] WrMode %u, Prof %u, VinState %u, ProfSwitch %u OpStatus %u",
                WriteMode, ProfState, DspVinState, ProfSwitch, pResource->DspOpStatus);
        }
    } else {
        /* DO NOTHING */
    }

    return Rval;
}

/**
* Liveview control function
* @param [in]  NumViewZone number of viewzone
* @param [in]  pViewZoneId viewzone index
* @param [in]  pEnable enable information
* @return ErrorCode
*/
UINT32 dsp_liveview_ctrl(UINT16 NumViewZone, const UINT16 *pViewZoneId, const UINT8 *pEnable)
{
    UINT32 Rval;
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();
    UINT8 NumEnable = 0U, NumDisable = 0U;
    UINT32 VprocEnableBit = 0U, VprocDisableBit = 0U;

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    /* Input sanity check */
    if (DumpApi != NULL) {
        DumpApi->pLiveviewCtrl(NumViewZone, pViewZoneId, pEnable);
    }
    if (CheckApi != NULL) {
        Rval = CheckApi->pLiveviewCtrl(NumViewZone, pViewZoneId, pEnable);
    }
    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pLiveviewCtrl(NumViewZone, pViewZoneId, pEnable);
    }
    /* Logic sanity check */
    if (Rval == OK) {
        Rval = HL_LvCtrlLogicCheck(NumViewZone,
                                   pViewZoneId,
                                   pEnable,
                                   &NumEnable,
                                   &NumDisable,
                                   &VprocEnableBit,
                                   &VprocDisableBit);
    }

    /* Body */
    if (Rval == OK) {
        CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

        HL_GetResourcePtr(&Resource);

        if (NumDisable > 0U) {
            Rval = HL_LvCtrOnDisable(NumViewZone,
                                     pViewZoneId,
                                     pEnable,
                                     Resource,
                                     &VprocDisableBit);
        } else if (NumEnable > 0U) {
            Rval = HL_LvCtrOnEnable(NumViewZone,
                                    pViewZoneId,
                                    pEnable,
                                    Resource,
                                    VprocEnableBit);
        } else {
            /* DO NOTHING */
        }
    }

    return Rval;
}

static inline UINT32 HL_LvUptCfgOnSyncJobSearch(const CTX_EFCT_SYNC_JOB_SET_s *pEfctSyncJobSet,
                                                const UINT16 ViewZoneId,
                                                UINT8 *pJobExisted,
                                                UINT16 *pJobSeqId,
                                                UINT16 *pJobType)
{
    UINT32 Rval = OK;
    UINT16 j;

    for (j=0U; j<pEfctSyncJobSet->JobNum; j++) {
        *pJobType = (UINT16)pEfctSyncJobSet->Job[j].JobId.JobTypeBit;

        if (0U == DSP_GetU16Bit(*pJobType, EFCT_SYNC_JOB_BIT_DISPLAY, 1U)) {
            if (ViewZoneId == pEfctSyncJobSet->Job[j].ViewZoneIdx) {
                *pJobSeqId = j;
                *pJobExisted = 1U;
                break;
            }
        }
    }

    return Rval;
}

static inline UINT32 HL_LvUptEfctCfgOnStateNewJob(const UINT16 StreamId,
                                                  const UINT32 EfctSyncCtrlPoolDescWp,
                                                  const AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *pChanCfg,
                                                  CTX_EFCT_SYNC_JOB_SET_s *pEfctSyncJobSet,
                                                  DSP_EFCT_SYNC_JOB_s *pSyncJob,
                                                  const CTX_YUV_STRM_INFO_s *pYuvStrmInfo)
{
    UINT32 Rval;
    UINT16 JobType;

    pSyncJob->JobId = pEfctSyncJobSet->JobId;
    JobType = (UINT16)1U << EFCT_SYNC_JOB_BIT_LAYOUT;
    Rval = HL_ComposeEfctSyncJobId(&pSyncJob->JobId,
                                   EFCT_SYNC_JOB_ID_DONTCARE,
                                   EFCT_SYNC_JOB_ID_DONTCARE,
                                   JobType,
                                   (UINT16)pEfctSyncJobSet->JobNum+1U);
    if (Rval == OK) {
        pSyncJob->ViewZoneIdx = pChanCfg->ViewZoneId;
        if (1U == DSP_GetBit(pYuvStrmInfo->ChanBitMask, pChanCfg->ViewZoneId, 1U)) {
            UINT16 PrevWp = (UINT16)(((EfctSyncCtrlPoolDescWp + MAX_GROUP_CMD_POOL_NUM) - 1U) % MAX_GROUP_CMD_POOL_NUM);
            AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *PrevChanCfg;

            Rval = HL_GetYuvStrmEfctSyncJobSetChanCfg(StreamId, PrevWp, pChanCfg->ViewZoneId, &PrevChanCfg);
            if (Rval == OK) {
                if ((HL_GET_ROTATE(pChanCfg->RotateFlip) == DSP_NO_ROTATE) &&
                    (PrevChanCfg->Window.Width == pChanCfg->Window.Width) &&
                    (PrevChanCfg->Window.Height == pChanCfg->Window.Height) &&
                    (PrevChanCfg->ROI.Width == pChanCfg->ROI.Width) &&
                    (PrevChanCfg->ROI.Height == pChanCfg->ROI.Height)) {
                    pSyncJob->PrevSizeUpdate = 0U;
                } else if ((HL_GET_ROTATE(pChanCfg->RotateFlip) == DSP_ROTATE_90_DEGREE) &&
                           (PrevChanCfg->Window.Width == pChanCfg->Window.Height) &&
                           (PrevChanCfg->Window.Height == pChanCfg->Window.Width) &&
                           (PrevChanCfg->ROI.Width == pChanCfg->ROI.Height) &&
                           (PrevChanCfg->ROI.Height == pChanCfg->ROI.Width)) {
                    pSyncJob->PrevSizeUpdate = 0U;
                } else {
                    pSyncJob->PrevSizeUpdate = 1U;
                }
            } else {
                AmbaLL_LogUInt5("HL_LvUptEfctCfgOnStateNewJob pChanCfg vz:%u, force update",
                        pChanCfg->ViewZoneId, 0U, 0U, 0U, 0U);
                pSyncJob->PrevSizeUpdate = 1U;
            }
        } else {
            pSyncJob->PrevSizeUpdate = 1U;
        }

        /* Advanced JobNum */
        pEfctSyncJobSet->JobNum++;
    }
    return Rval;
}

static inline UINT32 HL_LvUptEfctCfgOnStateExistJob(const UINT16 StreamId,
                                                    const CTX_YUV_STRM_INFO_s *pYuvStrmInfo,
                                                    const AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *pChanCfg,
                                                    UINT16 *pJobType,
                                                    DSP_EFCT_SYNC_JOB_s *pSyncJob)
{
    UINT32 Rval = OK;

    *pJobType |= (UINT16)1U << EFCT_SYNC_JOB_BIT_LAYOUT;
    pSyncJob->JobId.JobTypeBit = (UINT8)*pJobType;

    if (1U == DSP_GetBit(pYuvStrmInfo->ChanBitMask, pChanCfg->ViewZoneId, 1U)) {
        UINT16 PrevWp;
        CTX_YUVSTRM_EFCT_SYNC_CTRL_s *EfctSyncCtrl = HL_CtxYuvStrmEfctSyncCtrlPtr;
        AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *PrevChanCfg = NULL;

        HL_GetYuvStrmEfctSyncCtrlLock(StreamId, &EfctSyncCtrl);
        PrevWp = (UINT16)(((EfctSyncCtrl->PoolDesc.Wp + MAX_GROUP_CMD_POOL_NUM) - 1U) % MAX_GROUP_CMD_POOL_NUM);
        Rval = HL_GetYuvStrmEfctSyncJobSetChanCfg(StreamId, PrevWp, pChanCfg->ViewZoneId, &PrevChanCfg);
        if (Rval == OK) {
            if ((HL_GET_ROTATE(pChanCfg->RotateFlip) == DSP_NO_ROTATE) &&
                (PrevChanCfg->Window.Width == pChanCfg->Window.Width) &&
                (PrevChanCfg->Window.Height == pChanCfg->Window.Height) &&
                (PrevChanCfg->ROI.Width == pChanCfg->ROI.Width) &&
                (PrevChanCfg->ROI.Height == pChanCfg->ROI.Height)) {
                pSyncJob->PrevSizeUpdate = 0U;
            } else if ((HL_GET_ROTATE(pChanCfg->RotateFlip) == DSP_ROTATE_90_DEGREE) &&
                       (PrevChanCfg->Window.Width == pChanCfg->Window.Height) &&
                       (PrevChanCfg->Window.Height == pChanCfg->Window.Width) &&
                       (PrevChanCfg->ROI.Width == pChanCfg->ROI.Height) &&
                       (PrevChanCfg->ROI.Height == pChanCfg->ROI.Width)) {
                pSyncJob->PrevSizeUpdate = 0U;
            } else {
                pSyncJob->PrevSizeUpdate = 1U;
            }
        } else {
            AmbaLL_LogUInt5("HL_LvUptEfctCfgOnStateExistJob pChanCfg vz:%u, force update",
                    pChanCfg->ViewZoneId, 0U, 0U, 0U, 0U);
            pSyncJob->PrevSizeUpdate = 1U;
        }
        HL_GetYuvStrmEfctSyncCtrlUnLock(StreamId);
    } else {
        pSyncJob->PrevSizeUpdate = 1U;
    }

    return Rval;
}

static inline UINT32 HL_LvUptEfctCfgPotNewJob(CTX_EFCT_SYNC_JOB_SET_s *pEfctSyncJobSet,
                                              DSP_EFCT_SYNC_JOB_s *pSyncJob,
                                              UINT16 ViewZoneId,
                                              UINT16 *pJobType,
                                              const CTX_YUV_STRM_INFO_s *pYuvStrmInfo)
{
    UINT32 Rval;

    pSyncJob->JobId = pEfctSyncJobSet->JobId;
    *pJobType = (UINT16)1U << EFCT_SYNC_JOB_BIT_LAYOUT;
    Rval = HL_ComposeEfctSyncJobId(&pSyncJob->JobId,
                                   EFCT_SYNC_JOB_ID_DONTCARE,
                                   EFCT_SYNC_JOB_ID_DONTCARE,
                                   *pJobType,
                                   (UINT16)pEfctSyncJobSet->JobNum+1U);
    if (Rval == OK) {
        pSyncJob->ViewZoneIdx = ViewZoneId;
        if (0U == DSP_GetBit(pYuvStrmInfo->ChanBitMask, ViewZoneId, 1U)) {
            pSyncJob->PrevSizeUpdate = 0U;
        } else {
            pSyncJob->PrevSizeUpdate = 1U;
        }

        /* Advanced JobNum */
        pEfctSyncJobSet->JobNum++;
    }
    return Rval;
}

static inline UINT32 HL_LvUptEfctCfgPotExistJob(UINT16 *pJobType,
                                                DSP_EFCT_SYNC_JOB_s *pSyncJob,
                                                UINT16 ViewZoneId,
                                                const CTX_YUV_STRM_INFO_s *pYuvStrmInfo)
{
    UINT32 Rval = OK;

    *pJobType |= (UINT16)1U << EFCT_SYNC_JOB_BIT_LAYOUT;
    pSyncJob->JobId.JobTypeBit = (UINT8)*pJobType;
    if (0U == DSP_GetBit(pYuvStrmInfo->ChanBitMask, ViewZoneId, 1U)) {
        pSyncJob->PrevSizeUpdate = 0U;
    } else {
        pSyncJob->PrevSizeUpdate = 1U;
    }

    return Rval;
}

static inline UINT32 HL_LvUptCfgOnEfctSyncOffState(const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYuvStrmCfg,
                                                   CTX_YUV_STRM_INFO_s *pYuvStrmInfo,
                                                   UINT32 *pViewZoneOI)
{
    UINT32 Rval = OK;
    UINT16 j;
    CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout = &pYuvStrmInfo->Layout;

    pYuvStrmInfo->Purpose = pYuvStrmCfg->Purpose;
    pYuvStrmInfo->DestVout = pYuvStrmCfg->DestVout;
    pYuvStrmInfo->DestEnc = pYuvStrmCfg->DestEnc;
    pYuvStrmInfo->Width = pYuvStrmCfg->Width;
    pYuvStrmInfo->Height = pYuvStrmCfg->Height;
    pYuvStrmInfo->MaxWidth = pYuvStrmCfg->MaxWidth;
    pYuvStrmInfo->MaxHeight = pYuvStrmCfg->MaxHeight;
    pYuvStrmInfo->IsExtMem = (pYuvStrmCfg->StreamBuf.AllocType == ALLOC_INTERNAL)? 0U: 1U;
    pYuvStrmInfo->AllocType = pYuvStrmCfg->StreamBuf.AllocType;
    pYuvStrmInfo->BufNum = pYuvStrmCfg->StreamBuf.BufNum;
    (void)dsp_osal_memcpy(&pYuvStrmInfo->YuvBuf, &pYuvStrmCfg->StreamBuf.YuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
    if (pYuvStrmCfg->StreamBuf.AllocType == ALLOC_EXTERNAL_DISTINCT) {
        for (j = 0U; j < pYuvStrmInfo->BufNum; j++) {
            pYuvStrmInfo->YuvBufTbl[j] = pYuvStrmCfg->StreamBuf.pYuvBufTbl[j];
        }
    }

    pYuvStrmLayout->NumChan = pYuvStrmCfg->NumChan;
    pYuvStrmLayout->ChanLinked = 0x0U;
    for (j=0; j<pYuvStrmCfg->NumChan; j++) {
        DSP_SetBit(pViewZoneOI, (UINT32)pYuvStrmCfg->pChanCfg[j].ViewZoneId);
        (void)dsp_osal_memcpy(&pYuvStrmLayout->ChanCfg[j], &pYuvStrmCfg->pChanCfg[j], sizeof(AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s));

        (void)dsp_osal_memset(&pYuvStrmLayout->BlendCfg[j][0U], 0, sizeof(AMBA_DSP_BUF_s)*DSP_MAX_PP_STRM_BLEND_NUMBER);
        if (pYuvStrmLayout->ChanCfg[j].BlendNum > 0U) {
            const AMBA_DSP_BUF_s *pBldBuf;

            dsp_osal_typecast(&pBldBuf, &pYuvStrmCfg->pChanCfg[j].LumaAlphaTable);
            (void)dsp_osal_memcpy(&pYuvStrmLayout->BlendCfg[j][0U], &pBldBuf[0U], sizeof(AMBA_DSP_BUF_s)*pYuvStrmLayout->ChanCfg[j].BlendNum);
        }
    }

    return Rval;
}

static inline UINT32 HL_LvUptCfgOnYuvStrm2ndHalf(const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYuvStrmCfg,
                                                 const UINT32 *pViewZoneOI,
                                                 CTX_YUV_STRM_INFO_s *pYuvStrmInfo,
                                                 CTX_EFCT_SYNC_JOB_SET_s *pEfctSyncJobSet,
                                                 CTX_YUV_STRM_LAYOUT_s *pEfctSyncJobSetLayout)
{
    UINT32 Rval = OK;
    UINT16 j, Idx;
    UINT8 JobExisted = 0U;
    UINT16 JobSeqId = 0U, JobType = 0U;
    DSP_EFCT_SYNC_JOB_s *pSyncJob = NULL;
    CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout;

    for (Idx=0U; Idx<AMBA_DSP_MAX_YUVSTRM_VIEW_NUM; Idx++) {
        UINT8 ViewExistInMaxChan = (UINT8)DSP_GetBit(pYuvStrmInfo->MaxChanBitMask, Idx, 1U);
        UINT8 ViewExistInViewOI = (UINT8)DSP_GetBit(*pViewZoneOI, Idx, 1U);
        CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

        HL_GetViewZoneInfoPtr(Idx, &ViewZoneInfo);
        if ((0U == ViewExistInMaxChan) ||
            (1U == ViewExistInViewOI) ||
            (ViewZoneInfo->EffectSyncState != EFCT_SYNC_ON)) {
            continue;
        }

        JobExisted = 0U;
        (void)HL_LvUptCfgOnSyncJobSearch(pEfctSyncJobSet,
                                         Idx,
                                         &JobExisted,
                                         &JobSeqId,
                                         &JobType);

        if (JobExisted == 0U) {
            /* Create new Job with prepared YuvStrmIdx/SeqIdx */
            pSyncJob = &pEfctSyncJobSet->Job[pEfctSyncJobSet->JobNum];
            Rval = HL_LvUptEfctCfgPotNewJob(pEfctSyncJobSet,
                                            pSyncJob,
                                            Idx,
                                            &JobType,
                                            pYuvStrmInfo);
#ifdef DEBUG_SYNC_CMD
            AmbaLL_LogUInt5("@@[%d] New Layout Type[0x%X] Sub[%d]", __LINE__, pSyncJob->JobId.JobTypeBit, pSyncJob->JobId.SubJobIdx, 0, 0);
#endif
        } else if (JobSeqId < MAX_GROUP_CMD_SUBJOB_NUM) {
            pSyncJob = &pEfctSyncJobSet->Job[JobSeqId];
            (void)HL_LvUptEfctCfgPotExistJob(&JobType,
                                             pSyncJob,
                                             Idx,
                                             pYuvStrmInfo);
#ifdef DEBUG_SYNC_CMD
            AmbaLL_LogUInt5("@@[%d] Exist Layout Type[0x%X] Sub[%d]", __LINE__, pSyncJob->JobId.JobTypeBit, pSyncJob->JobId.SubJobIdx, 0, 0);
#endif
        } else {
            AmbaLL_LogUInt5("[%d] No JobExist Neither JobDesc", __LINE__, 0, 0, 0, 0);
        }

        if ((Rval == OK) && (pSyncJob != NULL)) {
            //
        } else {
            break;
        }
    }

    if (Rval == OK) {
        pYuvStrmInfo->ChanBitMask = *pViewZoneOI;

        /* Fill Job content */
        pYuvStrmLayout = pEfctSyncJobSetLayout;
        pYuvStrmLayout->NumChan = pYuvStrmCfg->NumChan;
        pYuvStrmLayout->ChanLinked = 0x0U;

        for (j=0; j<pYuvStrmCfg->NumChan; j++) {
            (void)dsp_osal_memcpy(&pYuvStrmLayout->ChanCfg[j], &pYuvStrmCfg->pChanCfg[j], sizeof(AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s));

            (void)dsp_osal_memset(&pYuvStrmLayout->BlendCfg[j][0U], 0, sizeof(AMBA_DSP_BUF_s)*DSP_MAX_PP_STRM_BLEND_NUMBER);
            if (pYuvStrmLayout->ChanCfg[j].BlendNum > 0U) {
                const AMBA_DSP_BUF_s *pBldBuf;

                dsp_osal_typecast(&pBldBuf, &pYuvStrmCfg->pChanCfg[j].LumaAlphaTable);
                (void)dsp_osal_memcpy(&pYuvStrmLayout->BlendCfg[j][0U], &pBldBuf[0U], sizeof(AMBA_DSP_BUF_s)*pYuvStrmLayout->ChanCfg[j].BlendNum);
            }
        }
    }

    return Rval;
}

static inline UINT32 HL_LvUptCfgOnYuvStrmPreProc(const UINT32 EfctSyncCtrlPoolDescWp,
                                                 const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYuvStrmCfg,
                                                 const CTX_YUV_STRM_INFO_s *pYuvStrmInfo,
                                                 CTX_EFCT_SYNC_JOB_SET_s *pEfctSyncJobSet,
                                                 UINT32 *pViewZoneOI)
{
    UINT8 JobExisted;
    UINT16 Idx;
    UINT16 JobSeqId, JobType;
    UINT32 Rval = OK;
    DSP_EFCT_SYNC_JOB_s *pSyncJob = NULL;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    for (Idx=0U; Idx<pYuvStrmCfg->NumChan; Idx++) {
        HL_GetViewZoneInfoPtr(pYuvStrmCfg->pChanCfg[Idx].ViewZoneId, &ViewZoneInfo);
        if ((ViewZoneInfo->EffectSyncState == EFCT_SYNC_OFF) && (Idx != 0U)) {
            /* We need first ViewZone's batch set for layout change */
            continue;
        }
        DSP_SetBit(pViewZoneOI, pYuvStrmCfg->pChanCfg[Idx].ViewZoneId);

        JobExisted = 0U;
        JobSeqId = 0U;
        JobType = 0U;
        (void)HL_LvUptCfgOnSyncJobSearch(pEfctSyncJobSet,
                                         pYuvStrmCfg->pChanCfg[Idx].ViewZoneId,
                                         &JobExisted,
                                         &JobSeqId,
                                         &JobType);

        if (JobExisted == 0U) {
            /* Create new Job with prepared YuvStrmIdx/SeqIdx */
            pSyncJob = &pEfctSyncJobSet->Job[pEfctSyncJobSet->JobNum];
            Rval = HL_LvUptEfctCfgOnStateNewJob(pYuvStrmCfg->StreamId,
                                                EfctSyncCtrlPoolDescWp,
                                                &pYuvStrmCfg->pChanCfg[Idx],
                                                pEfctSyncJobSet,
                                                pSyncJob,
                                                pYuvStrmInfo);
#ifdef DEBUG_SYNC_CMD
            AmbaLL_LogUInt5("@@[%d] New Layout Type[0x%X] Sub[%d]",
                    __LINE__, pSyncJob->JobId.JobTypeBit, pSyncJob->JobId.SubJobIdx, 0, 0);
#endif
        } else if (JobSeqId < MAX_GROUP_CMD_SUBJOB_NUM) {
            pSyncJob = &pEfctSyncJobSet->Job[JobSeqId];
            (void)HL_LvUptEfctCfgOnStateExistJob(pYuvStrmCfg->StreamId,
                                                 pYuvStrmInfo,
                                                 &pYuvStrmCfg->pChanCfg[Idx],
                                                 &JobType,
                                                 pSyncJob);
#ifdef DEBUG_SYNC_CMD
            AmbaLL_LogUInt5("@@[%d] Exist Layout Type[0x%X] Sub[%d]",
                    __LINE__, pSyncJob->JobId.JobTypeBit, pSyncJob->JobId.SubJobIdx, 0, 0);
#endif
        } else {
            AmbaLL_LogUInt5("[%d] No JobExist Neither JobDesc", __LINE__, 0, 0, 0, 0);
        }

        if (Rval == OK) {
//            HL_LvUptCfgOnYuvStrmPrevSizeUpdate(pSyncJob, Idx, pYuvStrmInfo, pYuvStrmCfg);
        } else {
            break;
        }
    }
    return Rval;
}

static inline UINT32 HL_LvUptCfgOnYuvStrm(const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYuvStrmCfg,
                                          UINT8 *pEfctSyncOn,
                                          UINT32 *pViewZoneOI)
{
    UINT32 Rval = OK;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_EFCT_SYNC_JOB_SET_s *EfctSyncJobSet = HL_CtxYuvStrmEfctSyncJobSetPtr;
    CTX_YUVSTRM_EFCT_SYNC_CTRL_s *pEfctSyncCtrl = HL_CtxYuvStrmEfctSyncCtrlPtr;
    CTX_YUV_STRM_LAYOUT_s *EfctSyncJobSetLayout = HL_CtxYuvStrmEfctLayoutPtr;

    HL_GetYuvStrmInfoLock(pYuvStrmCfg->StreamId, &YuvStrmInfo);
    HL_GetYuvStrmEfctSyncCtrlLock(pYuvStrmCfg->StreamId, &pEfctSyncCtrl);
    HL_GetYuvStrmEfctSyncJobSetLock(pYuvStrmCfg->StreamId, (UINT16)pEfctSyncCtrl->PoolDesc.Wp, &EfctSyncJobSet);
    HL_GetYuvStrmEfctSyncJobSetLayoutLock(pYuvStrmCfg->StreamId, (UINT16)pEfctSyncCtrl->PoolDesc.Wp, &EfctSyncJobSetLayout);

    if (pEfctSyncCtrl->State != EFCT_SYNC_OFF) {
        /* Sweep Existed Job on each View in case PrevOutputSize changed */
        Rval = HL_LvUptCfgOnYuvStrmPreProc(pEfctSyncCtrl->PoolDesc.Wp,
                                           pYuvStrmCfg,
                                           YuvStrmInfo,
                                           EfctSyncJobSet,
                                           pViewZoneOI);
        if (Rval == OK) {
            /* Potential component */
            Rval = HL_LvUptCfgOnYuvStrm2ndHalf(pYuvStrmCfg, pViewZoneOI, YuvStrmInfo, EfctSyncJobSet, EfctSyncJobSetLayout);
        }
        *pEfctSyncOn = EFCT_SYNC_ON;
    } else {
        (void)HL_LvUptCfgOnEfctSyncOffState(pYuvStrmCfg, YuvStrmInfo, pViewZoneOI);
        *pEfctSyncOn = EFCT_SYNC_OFF;
    }
    HL_GetYuvStrmEfctSyncJobSetUnLock(pYuvStrmCfg->StreamId, (UINT16)pEfctSyncCtrl->PoolDesc.Wp);
    HL_GetYuvStrmEfctSyncCtrlUnLock(pYuvStrmCfg->StreamId);
    HL_GetYuvStrmInfoUnLock(pYuvStrmCfg->StreamId);
    return Rval;
}

static inline UINT32 HL_LvUptCfgOnPrevUpdate(const UINT8 WriteMode,
                                             const UINT32 ViewZoneOI)
{
    UINT32 Rval = OK;
    UINT16 i, j;

    for (i = 0U; i < AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
        if (0U == DSP_GetBit(ViewZoneOI, i, 1U)) {
            continue;
        }

        for (j = 0; j < DSP_VPROC_PREV_D; j++) {
            UINT8 CmdByPass = 0U;
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_vproc_prev_setup_t *PrevSetup = HL_DefCtxCmdBufPtrVpcPrev;

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&PrevSetup, &CmdBufferAddr);
            Rval = HL_FillVideoPreviewSetup(PrevSetup, i/*ViewZoneId*/, (UINT8)j/*PrevId*/, &CmdByPass, NULL);
            if (Rval != OK) {
                AmbaLL_LogUInt5("PrevSetup[%d][%d] filling fail", i, j, 0U, 0U, 0U);
            } else if (CmdByPass == 0U) {
                Rval = AmbaHL_CmdVprocPrevSetup(WriteMode, PrevSetup);
                if (Rval != OK) {
                    break;
                }
            } else {
                // no such preview out
            }
            HL_RelCmdBuffer(CmdBufferId);
        }
    }

    return Rval;
}

static inline UINT32 HL_LiveviewUpdateConfigChk(const UINT16 NumYuvStream,
                                                const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYuvStrmCfg,
                                                const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    //Misrac
    (void)pAttachedRawSeq;
    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pLiveviewUpdateConfig(NumYuvStream, pYuvStrmCfg, pAttachedRawSeq);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pLiveviewUpdateConfig(NumYuvStream, pYuvStrmCfg, pAttachedRawSeq);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pLiveviewUpdateConfig(NumYuvStream, pYuvStrmCfg, pAttachedRawSeq);
    }

    /* Timing sanity check */

    /* Logic sanity check */
    return Rval;
}

/**
* Liveview configuration update function
* @param [in]  NumYuvStream number of Yuv stream
* @param [in]  pYuvStrmCfg yuv stream configuration
* @param [out] pAttachedRawSeq attached raw sequence
* @return ErrorCode
*/
UINT32 dsp_liveview_update_cfg(UINT16 NumYuvStream,
                               const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYuvStrmCfg,
                               UINT64 *pAttachedRawSeq)
{
    UINT32 Rval;
    UINT16 i;
    UINT8 EfctSyncCtrlOn = EFCT_SYNC_OFF;
    UINT32 ViewZoneOI = 0U;
    UINT8 WriteMode = AMBA_DSP_CMD_VDSP_GROUP_WRITE;

    AmbaMisra_TouchUnused(pAttachedRawSeq);
    Rval = HL_LiveviewUpdateConfigChk(NumYuvStream, pYuvStrmCfg, pAttachedRawSeq);

    /* Body */
    if (Rval == OK) {
//FIXME, handle runtime Layout change
        /* Process YuvStrm with Sync currently */
        for (i=0U; i<NumYuvStream; i++) {
            UINT8 YuvEfctSyncCtrlOn = EFCT_SYNC_OFF;

            Rval = HL_LvUptCfgOnYuvStrm(&pYuvStrmCfg[i],
                                        &YuvEfctSyncCtrlOn,
                                        &ViewZoneOI);
            EfctSyncCtrlOn |= YuvEfctSyncCtrlOn;
            if (Rval != OK) {
                break;
            }
        }

        if (Rval == OK) {
            if ((EfctSyncCtrlOn == EFCT_SYNC_OFF) && (ViewZoneOI != 0U)) {
                if (pYuvStrmCfg[i].NumChan == 1U) {
                    /* Vproc-rebinding: mainly for PinWindow update */
                    HL_CtxLvDataPathReset(ViewZoneOI);
                    Rval = HL_VideoProcBinding();
                    if (Rval == OK) {
                        AmbaLL_LogUInt5("HL_VideoProcBinding fails", 0U, 0U, 0U, 0U, 0U);
                    }
                }

                Rval = HL_LvUptCfgOnPrevUpdate(WriteMode, ViewZoneOI);
                if (Rval == OK) {
                    Rval = DSP_ProcessBufCmds(WriteMode);
                }
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_LiveviewUpdateConfig Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }
    return Rval;
}

static inline void HL_LvUptIsoCfgOnBeforeDspBoot(UINT16 NumViewZone,
                                                 const AMBA_DSP_LIVEVIEW_ISOCFG_CTRL_s *pIsoCfgCtrl)
{
    UINT16 i;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    for (i=0U; i<NumViewZone; i++) {
        HL_GetViewZoneInfoLock(pIsoCfgCtrl[i].ViewZoneId, &ViewZoneInfo);
        ViewZoneInfo->StartIsoCtxIndex = (UINT8)pIsoCfgCtrl[i].CtxIndex;
        ViewZoneInfo->StartIsoCfgIndex = pIsoCfgCtrl[i].CfgIndex;
        ViewZoneInfo->StartIsoCfgAddr = pIsoCfgCtrl[i].CfgAddress;
        ViewZoneInfo->StartHdrCfgIndex = (UINT8)pIsoCfgCtrl[i].HdrCfgIndex;
        HL_GetViewZoneInfoUnLock(pIsoCfgCtrl[i].ViewZoneId);
    }
}

static inline UINT32 HL_LvUptIsoCfgOnDecVz(const UINT16 DecIdx,
                                           const AMBA_DSP_LIVEVIEW_ISOCFG_CTRL_s *pIsoCfgCtrl)
{
    UINT16 ViewZoneId = pIsoCfgCtrl->ViewZoneId;
    UINT32 Rval = OK;
    UINT32 CfgIndex = pIsoCfgCtrl->CfgIndex;
    CTX_VPROC_INFO_s VprocInfo = {0};
    CTX_VID_DEC_INFO_s VidDecInfo = {0};
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);

    HL_GetVprocInfo(HL_MTX_OPT_ALL, ViewZoneId, &VprocInfo);
    HL_GetVidDecInfo(HL_MTX_OPT_ALL, DecIdx, &VidDecInfo);

    /* Attach to decoder when running */
    if ((VidDecInfo.CurrentBind == BIND_VIDEO_DECODE_TO_VPROC) &&
        (VidDecInfo.State == VIDDEC_STATE_RUN)) {

        Rval = HL_LiveviewDecIsoCfgImpl(DecIdx, CfgIndex, pIsoCfgCtrl->CfgAddress);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[HL_LvUptIsoCfgOnDecVz] HL_LiveviewDec2VprocIsoCfgImpl fail %d",
                    Rval, 0U, 0U, 0U, 0U);
        }

    /* Attach to decoder when pause */
    } else if (VidDecInfo.State == VIDDEC_STATE_PAUSE) {
        if (VidDecInfo.CurrentBind == BIND_VIDEO_DECODE_TO_VPROC) {
            Rval = HL_LiveviewDec2VprocBind2VirtVin(DecIdx, &VidDecInfo);
            if (Rval != OK) {
                AmbaLL_LogUInt5("[HL_LvUptIsoCfgOnDecVz] HL_LiveviewDec2VprocIsoCfgBind fail %d",
                        Rval, 0U, 0U, 0U, 0U);
            }
        }
        Rval = HL_LiveviewVirtVinIsoCfgImpl(DecIdx, &VidDecInfo, CfgIndex, pIsoCfgCtrl->CfgAddress);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[HL_LvUptIsoCfgOnDecVz] HL_LiveviewDec2VprocIsoCfgPauseImpl fail %d",
                    Rval, 0U, 0U, 0U, 0U);
        }
    /* Suppose to attach to virtual vin,
     * Since vproc is stopped, just keep it */
//    } else if ((VidDecInfo.CurrentBind == BIND_STILL_PROC) &&
//               (VidDecInfo.State <= VIDDEC_STATE_OPENED) &&
//               (VprocInfo.Status != DSP_VPROC_STATUS_ACTIVE)) {
    } else if (VidDecInfo.State < VIDDEC_STATE_OPENED) {
        HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
        ViewZoneInfo->StartIsoCtxIndex = (UINT8)(pIsoCfgCtrl->CtxIndex);
        ViewZoneInfo->StartIsoCfgIndex = pIsoCfgCtrl->CfgIndex;
        ViewZoneInfo->StartIsoCfgAddr = pIsoCfgCtrl->CfgAddress;
        ViewZoneInfo->StartHdrCfgIndex = (UINT8)(pIsoCfgCtrl->HdrCfgIndex);
        HL_GetViewZoneInfoUnLock(ViewZoneId);
    /* Attach to virtual vin */
    } else if ((VidDecInfo.CurrentBind == BIND_STILL_PROC) &&
               (VidDecInfo.State <= VIDDEC_STATE_OPENED) &&
               (VprocInfo.Status == DSP_VPROC_STATUS_ACTIVE)) {
        Rval = HL_LiveviewVirtVinIsoCfgImpl(DecIdx, &VidDecInfo, CfgIndex, pIsoCfgCtrl->CfgAddress);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[HL_LvUptIsoCfgOnDecVz] HL_LiveviewDec2VprocVirtVinIsoCfgImpl fail %d",
                    Rval, 0U, 0U, 0U, 0U);
        }
    /* unknown binding or unexpected state */
    } else {
        AmbaLL_LogUInt5("[HL_LvUptIsoCfgOnDecVz] %u unexpected: DspState:%u, State:%u bind:%u",
                __LINE__, VidDecInfo.DspState, VidDecInfo.State, VidDecInfo.CurrentBind, 0U);
    }

    return Rval;
}

static inline UINT32 HL_LvUptIsoCfgOnIso(UINT8 *pWriteMode,
                                         UINT16 ViewZoneId,
                                         UINT16 VinId,
                                         UINT32 CfgIndex,
                                         ULONG  CfgAddress)
{
    UINT8 IsNewCmd = 0U;
    UINT16 LogicVinId = 0U;
    UINT32 Rval = OK;
    UINT32 NewWp = 0U, LocalViewZoneId;
    UINT32 *pBatchQAddr = NULL, BatchCmdId = 0U;
    ULONG CmdBufAddr = 0U, ULAddr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    vin_fov_batch_cmd_set_t *pBatchCmdSet;
    cmd_vproc_ik_config_t *pIsoCfgCmd;
    const cmd_vin_attach_proc_cfg_to_cap_frm_t *pVinAttachProcCfg;

    *pWriteMode = HL_GetVin2CmdNormalWrite(VinId);
    LocalViewZoneId = HL_GetViewZoneLocalIndexOnVin(ViewZoneId);
    HL_CtrlBatchQBufMtx(HL_MTX_OPT_GET, ViewZoneId);
    (void)AmbaLL_CmdQuery(*pWriteMode,
                          CMD_VIN_ATTACH_PROC_CFG_TO_CAP_FRM,
                          &CmdBufAddr,
                          (UINT32)VinId,
                          LocalViewZoneId);
    if (CmdBufAddr > 0U) {
        /* Cmd Exist */

        dsp_osal_typecast(&pVinAttachProcCfg, &CmdBufAddr);
        (void)dsp_osal_cli2virt(pVinAttachProcCfg->fov_batch_cmd_set_addr, &ULAddr);
        dsp_osal_typecast(&pBatchCmdSet, &ULAddr);
        (void)dsp_osal_cli2virt(pBatchCmdSet->batch_cmd_set_info.addr, &ULAddr);
        dsp_osal_typecast(&pBatchQAddr, &ULAddr);
        HL_GetDspBatchCmdId(ULAddr, ViewZoneId, &BatchCmdId);
#ifdef DEBUG_SYNC_ISO_CMD
        AmbaLL_LogUInt5("  [UpdateIsoCfg] Cmd Exist Id[0x%X] Addr[0x%X%X]", BatchCmdId, DSP_GetU64Msb((UINT64)CmdBufAddr), DSP_GetU64Lsb((UINT64)CmdBufAddr), 0U, 0U);
#endif
    } else {
        UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;

        /* NewCmd in CurrentCmdQ, Forward BatchCmdQ WP */
        IsNewCmd = 1U;
        HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);

        /* Request BatchCmdQ buffer */
        Rval = DSP_ReqBuf(&ViewZoneInfo->BatchQPoolDesc, 1U, &NewWp, 1U/*FullnessCheck*/);
        if (Rval != OK) {
            HL_GetViewZoneInfoUnLock(ViewZoneId);
            AmbaLL_LogWarnUInt5((UINT16)AMBALLLOG_WARN_BATCHQ_FULL, "[LiveviewUpdateIsoCfg] ViewZone(%d) batch pool is full", ViewZoneId, 0U, 0U, 0U, 0U);
        } else {
            ViewZoneInfo->BatchQPoolDesc.Wp = NewWp;
            HL_GetViewZoneInfoUnLock(ViewZoneId);
            HL_GetPointerToDspBatchQ(ViewZoneId, (UINT16)ViewZoneInfo->BatchQPoolDesc.Wp, &pBatchQAddr, &BatchCmdId);

            /* Reset New BatchQ after Wp advanced */
            HL_ResetDspBatchQ(pBatchQAddr);
        }
        HL_GetPointerToDspBatchCmdSet(ViewZoneId, 0U/*Idx*/, &pBatchCmdSet);

        if (pIsSimilarEnabled != NULL) {
            if (1U == IsSimilarEnabled()) {
                SIM_UpdateCtxViewZoneBatchQWPtr(ViewZoneId);
            }
        }
#ifdef DEBUG_SYNC_ISO_CMD
        AmbaLL_LogUInt5("  [UpdateIsoCfg] NewCmd 0x%X", BatchCmdId, 0U, 0U, 0U, 0U);
#endif
    }

    if (Rval == OK) {
        (void)HL_GetViewZoneVinId(ViewZoneId, &LogicVinId);
        pBatchCmdSet->vin_id = LogicVinId;
        pBatchCmdSet->chan_id = (UINT16)HL_GetViewZoneLocalIndexOnVin(ViewZoneId);

        /* IsoCfgCmd occupies the first CmdSlot in batch */
        dsp_osal_typecast(&pIsoCfgCmd, &pBatchQAddr);
        (void)HL_FillIsoCfgUpdate(ViewZoneId, pIsoCfgCmd);
        (void)dsp_osal_virt2cli(CfgAddress, &pIsoCfgCmd->idsp_flow_addr);
        pIsoCfgCmd->ik_cfg_id = CfgIndex;

        HL_SetDspBatchQInfo(BatchCmdId, IsNewCmd/*AutoReset*/, BATCHQ_INFO_ISO_CFG, CfgIndex);
        pBatchCmdSet->batch_cmd_set_info.id = BatchCmdId;
        dsp_osal_typecast(&ULAddr, &pBatchQAddr);
        (void)dsp_osal_virt2cli(ULAddr, &pBatchCmdSet->batch_cmd_set_info.addr);
        pBatchCmdSet->batch_cmd_set_info.size = HL_GetBatchCmdNumber(ULAddr)*CMD_SIZE_IN_BYTE;

#ifndef TEST_FOR_ISOCFG_VIA_IK_CFG_CMD
        if (IsNewCmd == 1U) {
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_vin_attach_proc_cfg_to_cap_frm_t *VinAttachProcCfg = HL_DefCtxCmdBufPtrVinAttfrm;

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VinAttachProcCfg, &CmdBufferAddr);
            dsp_osal_typecast(&ULAddr, &pBatchCmdSet);
            (void)dsp_osal_virt2cli(ULAddr, &VinAttachProcCfg->fov_batch_cmd_set_addr);
            VinAttachProcCfg->vin_id = (UINT8)LogicVinId;
            VinAttachProcCfg->fov_num = 1U;
            (void)AmbaHL_CmdVinAttachCfgToCapFrm(*pWriteMode, VinAttachProcCfg);
            HL_RelCmdBuffer(CmdBufferId);
        }
#else
        //Using CMD_VPROC_IK_CONFIG when bring-up
        AmbaLL_LogUInt5("[LV] Force using CMD_VPROC_IK_CONFIG", 0U, 0U, 0U, 0U, 0U);
        {
            cmd_vproc_ik_config_t *VprocIkCfg = HL_DefCtxCmdBufPtrVpcIkCfg;

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VprocIkCfg, &CmdBufferAddr);
            (void)dsp_osal_memcpy(VprocIkCfg, pIsoCfgCmd, sizeof(cmd_vproc_ik_config_t));
            Rval = AmbaHL_CmdVprocIkConfig(*pWriteMode, VprocIkCfg);
            HL_RelCmdBuffer(CmdBufferId);
            if (Rval != OK) {
                AmbaLL_LogUInt5("CMD_VPROC_IK_CONFIG fail[0x%X]", Rval, 0U, 0U, 0U, 0U);
            }
        }
#endif
    }
    HL_CtrlBatchQBufMtx(HL_MTX_OPT_SET, ViewZoneId);

    return Rval;
}

static inline UINT32 HL_LvUptIsoCfgOnHdrCe(const UINT8 *pWriteMode,
                                           UINT16 ViewZoneId,
                                           UINT16 VinId,
                                           ULONG CfgAddress)
{
    UINT32 Rval = OK;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
    if ((ViewZoneInfo->HdrBlendNumMinusOne > 0U) || (ViewZoneInfo->LinearCe > 0U)) {
        UINT8 SkipCmd;
        UINT8 CmdBufferIdCe = 0U; void *CmdBufferAddrCe = NULL;
        cmd_vin_ce_setup_t *CeSetp = HL_DefCtxCmdBufPtrVinCe;

        SkipCmd = 0U;
        HL_AcqCmdBuffer(&CmdBufferIdCe, &CmdBufferAddrCe);
        dsp_osal_typecast(&CeSetp, &CmdBufferAddrCe);
        Rval = HL_FillVinCeSetup(VinId, CeSetp, &SkipCmd);
        if (Rval != OK) {
            //TBD
        } else if (SkipCmd == 1U) {
            //Skip cmd
        } else {
            (void)AmbaHL_CmdVinCeSetup(*pWriteMode, CeSetp);
        }
        HL_RelCmdBuffer(CmdBufferIdCe);

        if (ViewZoneInfo->HdrBlendNumMinusOne > 0U) {
            UINT8 CmdBufferIdHdr = 0U; void *CmdBufferAddrHdr = NULL;
            cmd_vin_hdr_setup_t *HdrSetup = HL_DefCtxCmdBufPtrVinHdr;

            SkipCmd = 0U;
            HL_AcqCmdBuffer(&CmdBufferIdHdr, &CmdBufferAddrHdr);
            dsp_osal_typecast(&HdrSetup, &CmdBufferAddrHdr);
            Rval = HL_FillVinHdrSetup(VinId, HdrSetup, CfgAddress, &SkipCmd);
            if (Rval != OK) {
                //TBD
            } else if (SkipCmd == 1U) {
                //Skip cmd
            } else {
                (void)AmbaHL_CmdVinHdrSetup(*pWriteMode, HdrSetup);
            }
            HL_RelCmdBuffer(CmdBufferIdHdr);
        }
    }

    return Rval;
}

static inline UINT32 HL_LvUptIsoCfgOnFovLayout(const UINT8 *pWriteMode,
                                               UINT16 ViewZoneId,
                                               UINT16 VinId)
{
    UINT32 Rval = OK;
    CTX_VIN_INFO_s VinInfo = {0};
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    HL_GetVinInfo(HL_MTX_OPT_ALL, VinId, &VinInfo);

    /* Check duplicate cmd */
    HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
    if ((VinInfo.CmdUpdate.SetFovLayout > 0U) ||
        (ViewZoneInfo->CmdUpdate.SetFovLayout > 0U)) {
        UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
        cmd_vin_set_fov_layout_t *FovLayout = HL_DefCtxCmdBufPtrVinLayout;

        // Reset CmdUpdate flag
        if (VinInfo.CmdUpdate.SetFovLayout > 0U) {
            HL_GetVinInfo(HL_MTX_OPT_GET, VinId, &VinInfo);
            VinInfo.CmdUpdate.SetFovLayout--;
            HL_SetVinInfo(HL_MTX_OPT_SET, VinId, &VinInfo);
        }
        if (ViewZoneInfo->CmdUpdate.SetFovLayout > 0U) {
            ViewZoneInfo->CmdUpdate.SetFovLayout--;
        }
        HL_GetViewZoneInfoUnLock(ViewZoneId);

        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&FovLayout, &CmdBufferAddr);
        Rval = HL_FillVinSetFovLayout(FovLayout, ViewZoneId, NULL);
        if (Rval != OK) {
            AmbaLL_LogUInt5("SetFovLayout filling fail", 0U, 0U, 0U, 0U, 0U);
        }
        (void)AmbaHL_CmdVinSetFovLayout(*pWriteMode, FovLayout);
        HL_RelCmdBuffer(CmdBufferId);
    } else {
        HL_GetViewZoneInfoUnLock(ViewZoneId);
    }

    return Rval;
}

static inline UINT32 HL_LiveviewUpdateIsoCfgChk(const UINT16 NumViewZone,
                                                const AMBA_DSP_LIVEVIEW_ISOCFG_CTRL_s *pIsoCfgCtrl,
                                                const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();

    //FIXME, Misra
    (void)pAttachedRawSeq;

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pLiveviewUpdateIsoConfig(NumViewZone, pIsoCfgCtrl, pAttachedRawSeq);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pLiveviewIsoConfigUpdate(NumViewZone, pIsoCfgCtrl, pAttachedRawSeq);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pLiveviewUpdateIsoConfig(NumViewZone, pIsoCfgCtrl, pAttachedRawSeq);
    }
    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pLiveviewUpdateIsoCfg(NumViewZone, pIsoCfgCtrl, pAttachedRawSeq);
    }
    /* Logic sanity check */

    return Rval;
}

static UINT32 HL_LvUptIsoCfgOnAfterDspBoot(UINT16 NumViewZone,
                                           const AMBA_DSP_LIVEVIEW_ISOCFG_CTRL_s *pIsoCfgCtrl)
{
    UINT32 Rval = OK;
    UINT16 i = 0U, VinId = 0U;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    for (i=0U; i<NumViewZone; i++) {
        UINT8 VinState;
        UINT16 ViewZoneId = pIsoCfgCtrl[i].ViewZoneId;

        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
        DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &VinId);
        VinState = DSP_GetVinState((UINT8)VinId);

        if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) {

            Rval = HL_LvUptIsoCfgOnDecVz(VinId/*DecIdx*/, &pIsoCfgCtrl[i]);
            if (Rval != OK) {
                AmbaLL_LogUInt5("[Err][0x%X][%d] AmbaDSP_LiveviewUpdateIsoCfg", Rval, __LINE__, 0U, 0U, 0U);
            }
        } else if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM) ||
                   (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_422) ||
                   (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_420) ||
                   (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON) ||
                   (VinState == DSP_RAW_CAP_STATUS_TIMER) ||
                   (VinState == DSP_RAW_CAP_STATUS_INVALID)) {

            if (((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM) ||
                 (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_422) ||
                 (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_420)) &&
                 (ViewZoneInfo->UsedStartIsoCfg == 0U)) {
                /* Make sure the first iso-cfg was used */
                Rval = DSP_ERR_0004;
            } else {
                HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
                ViewZoneInfo->StartIsoCtxIndex = (UINT8)pIsoCfgCtrl[i].CtxIndex;
                ViewZoneInfo->StartIsoCfgIndex = pIsoCfgCtrl[i].CfgIndex;
                ViewZoneInfo->StartIsoCfgAddr = pIsoCfgCtrl[i].CfgAddress;
                ViewZoneInfo->StartHdrCfgIndex = (UINT8)pIsoCfgCtrl[i].HdrCfgIndex;
                HL_GetViewZoneInfoUnLock(ViewZoneId);
            }
        } else if (IsValidULAddr(pIsoCfgCtrl[i].CfgAddress) == 1U) {
            UINT8 WriteMode;

            HL_GetResourcePtr(&Resource);

            /* Iso */
            Rval = HL_LvUptIsoCfgOnIso(&WriteMode,
                                       ViewZoneId,
                                       VinId,
                                       pIsoCfgCtrl[i].CfgIndex,
                                       pIsoCfgCtrl[i].CfgAddress);
            if (Rval != OK) {
                AmbaLL_LogUInt5("[Err][0x%X][%d] AmbaDSP_LiveviewUpdateIsoCfg", Rval, __LINE__, 0U, 0U, 0U);
            } else {
                /* HDR/CE */
                Rval = HL_LvUptIsoCfgOnHdrCe(&WriteMode, ViewZoneId, VinId, pIsoCfgCtrl[i].CfgAddress);
                if (Rval != OK) {
                    AmbaLL_LogUInt5("[Err][0x%X][%d] AmbaDSP_LiveviewUpdateIsoCfg", Rval, __LINE__, 0U, 0U, 0U);
                } else {
                    /* Multi-ViewZone in OneVin */
                    Rval = HL_LvUptIsoCfgOnFovLayout(&WriteMode, ViewZoneId, VinId);
                    if (Rval != OK) {
                        AmbaLL_LogUInt5("[Err][0x%X][%d] AmbaDSP_LiveviewUpdateIsoCfg", Rval, __LINE__, 0U, 0U, 0U);
                    }
                }
            }
        } else {
            Rval = DSP_ERR_0000;
            AmbaLL_LogUInt5("AmbaDSP_LiveviewUpdateIsoCfg idx:%u vz:%u ctx:%u cfg:%u",
                            i, ViewZoneId, pIsoCfgCtrl[i].CtxIndex, pIsoCfgCtrl[i].CfgIndex, 0U);
            AmbaLL_LogUInt5("                             CfgAddress 0x%X%X",
                        DSP_GetU64Msb((UINT64)pIsoCfgCtrl[i].CfgAddress),
                        DSP_GetU64Lsb((UINT64)pIsoCfgCtrl[i].CfgAddress), 0U, 0U, 0U);
            break;
        }
    }
    return Rval;
}

/**
* Liveview IsoConfig update function
* @param [in]  NumViewZone number of viewzone
* @param [in]  pIsoCfgCtrl IsoConfig control information
* @param [out] pAttachedRawSeq attached raw sequence
* @return ErrorCode
*/
UINT32 dsp_liveview_update_isocfg(UINT16 NumViewZone,
                                  const AMBA_DSP_LIVEVIEW_ISOCFG_CTRL_s *pIsoCfgCtrl,
                                  UINT64 *pAttachedRawSeq)
{
    UINT32 Rval;

    Rval = HL_LiveviewUpdateIsoCfgChk(NumViewZone, pIsoCfgCtrl, pAttachedRawSeq);
    AmbaMisra_TouchUnused(pAttachedRawSeq);

    /* Body */
    if (Rval == OK) {
        if (DSP_PROF_STATUS_CAMERA != DSP_GetProfState()) {

            HL_LvUptIsoCfgOnBeforeDspBoot(NumViewZone, pIsoCfgCtrl);
        } else {
            Rval = HL_LvUptIsoCfgOnAfterDspBoot(NumViewZone, pIsoCfgCtrl);
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_LiveviewUpdateIsoCfg Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_LiveviewUpdateVinCfgChk(const UINT16 VinId,
                                                const UINT16 SubChNum,
                                                const AMBA_DSP_VIN_SUB_CHAN_s *pSubCh,
                                                const AMBA_DSP_LIVEVIEW_VINCFG_CTRL_s *pLvVinCfgCtrl,
                                                const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pLiveviewUpdateVinCfg(VinId, SubChNum, pSubCh, pLvVinCfgCtrl, pAttachedRawSeq);
    }
    /* Debug print */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pLiveviewUpdateVinCfg(VinId, SubChNum, pSubCh, pLvVinCfgCtrl, pAttachedRawSeq);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pLiveviewUpdateVinCfg(VinId, SubChNum, pSubCh, pLvVinCfgCtrl, pAttachedRawSeq);
    }
    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pLiveviewUpdateVinCfg(VinId, SubChNum, pSubCh, pLvVinCfgCtrl, pAttachedRawSeq);
    }

    return Rval;
}

/**
* Liveview VinConfig update function
* @param [in]  VinId Vin Index
* @param [in]  SubChNum sub-channel num
* @param [in]  pSubCh array of sub channel
* @param [in]  pLvVinCfgCtrl Liveview VinConfig control information
* @param [out] pAttachedRawSeq attached raw sequence
* @return ErrorCode
*/
UINT32 dsp_liveview_update_vincfg(UINT16 VinId,
                                  UINT16 SubChNum,
                                  const AMBA_DSP_VIN_SUB_CHAN_s *pSubCh,
                                  const AMBA_DSP_LIVEVIEW_VINCFG_CTRL_s *pLvVinCfgCtrl,
                                  UINT64 *pAttachedRawSeq)
{
    UINT32 Rval;
    UINT8 WriteMode = HL_GetVin2CmdNormalWrite(VinId);

    Rval = HL_LiveviewUpdateVinCfgChk(VinId, SubChNum, pSubCh, pLvVinCfgCtrl, pAttachedRawSeq);

    if (Rval == OK) {
        /* Input sanity check */
        if (VinId >= AMBA_DSP_MAX_VIN_NUM) {
            /* Not an valid VinId */
            Rval = DSP_ERR_0001;
        } else if ((pLvVinCfgCtrl == NULL) || (pAttachedRawSeq == NULL)) {
            Rval = DSP_ERR_0000;
        } else if ((pLvVinCfgCtrl->EnaVinCompand == 1U) && \
                   ((pLvVinCfgCtrl->VinCompandTableAddr == 0U) || \
                    (pLvVinCfgCtrl->VinDeCompandTableAddr == 0U))) {
            Rval = DSP_ERR_0001;
        } else {
            CTX_VIN_INFO_s VinInfo;
            UINT8 VinState = DSP_GetVinState((UINT8)VinId);
            UINT8 DspState = DSP_GetProfState();

            // Update Vin info
            HL_GetVinInfo(HL_MTX_OPT_GET, VinId, &VinInfo);
            if (pLvVinCfgCtrl->EnaVinCompand == 1U) {
                VinInfo.CompandEna = 1;
                VinInfo.DeCompandEna = 1;
                VinInfo.CompandTableAddr = pLvVinCfgCtrl->VinCompandTableAddr;
                VinInfo.DeCompandTableAddr = pLvVinCfgCtrl->VinDeCompandTableAddr;
            } else {
                VinInfo.CompandEna = 0;
                VinInfo.DeCompandEna = 0;
                VinInfo.CompandTableAddr = 0;
                VinInfo.DeCompandTableAddr = 0;
            }
            HL_SetVinInfo(HL_MTX_OPT_SET, VinId, &VinInfo);

            /* Timing sanity check */
            if ((DSP_PROF_STATUS_CAMERA == DspState) &&
                (DSP_RAW_CAP_STATUS_VIDEO == VinState)) {
                UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
                set_vin_config_t *VinCfg = HL_DefCtxCmdBufPtrVinCfg;

                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&VinCfg, &CmdBufferAddr);
                Rval = HL_FillVinConfig(VinCfg, VinId);
                if (Rval == OK) {
                    Rval = AmbaHL_CmdSetVinConfig(WriteMode, VinCfg);
                }
                HL_RelCmdBuffer(CmdBufferId);
            }
            *pAttachedRawSeq = 0;   /* FIXME */
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_LiveviewUpdateVinCfg Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_LiveviewConfigVinCaptureChk(UINT16 VinId, UINT16 SubChNum, const AMBA_DSP_VIN_SUB_CHAN_CFG_s *pSubChCfg)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pLiveviewConfigVinCapture(VinId, SubChNum, pSubChCfg);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pLiveviewConfigVinCapture(VinId, SubChNum, pSubChCfg);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pLiveviewConfigVinCapture(VinId, SubChNum, pSubChCfg);
    }

    /* Logic sanity check */
    return Rval;
}

static inline UINT32 HL_LvCfgVinCapImpl(UINT16 VinId, UINT16 SubChNum, const AMBA_DSP_VIN_SUB_CHAN_CFG_s *pSubChCfg)
{
    UINT32 Rval = OK;
    UINT16 i, j;
    CTX_VIN_INFO_s VinInfo = {0};
    AMBA_DSP_WINDOW_s* pDspWindow = NULL;

    HL_GetVinInfo(HL_MTX_OPT_GET, VinId, &VinInfo);
    VinInfo.SubChanBit = 0U; //Reset it
    for (i=0U; i<SubChNum; i++) {
        if (pSubChCfg[i].SubChan.IsVirtual == 1U) {
            pDspWindow = &VinInfo.CapWindow[pSubChCfg[i].SubChan.Index];
            (void)dsp_osal_memcpy(pDspWindow, &pSubChCfg[i].CaptureWindow, sizeof(AMBA_DSP_WINDOW_s));
            VinInfo.Option[pSubChCfg[i].SubChan.Index] = pSubChCfg[i].Option;
            VinInfo.ConCatNum[pSubChCfg[i].SubChan.Index] = pSubChCfg[i].ConCatNum;
            VinInfo.IntcNum[pSubChCfg[i].SubChan.Index] = pSubChCfg[i].IntcNum;
            DSP_SetBit(&VinInfo.SubChanBit, (UINT32)pSubChCfg[i].SubChan.Index);
        } else {
            pDspWindow = &VinInfo.CapWindow[0];
            (void)dsp_osal_memcpy(pDspWindow, &pSubChCfg[i].CaptureWindow, sizeof(AMBA_DSP_WINDOW_s));
            VinInfo.Option[0U] = pSubChCfg[i].Option;
            VinInfo.ConCatNum[0U] = pSubChCfg[i].ConCatNum;
            VinInfo.IntcNum[0U] = pSubChCfg[i].IntcNum;
            DSP_SetBit(&VinInfo.SubChanBit, (UINT32)0U);
        }

//FIXME VirtChan
        if (pSubChCfg[i].TDNum > 1U) {
            VinInfo.TimeDivisionNum[0U] = pSubChCfg[i].TDNum;

            for (j=0U; j<VinInfo.TimeDivisionNum[0U]; j++) {
                if (pSubChCfg[i].pTDFrmNum != NULL) {
                    VinInfo.TimeDivisionFrmNum[0U][j] = pSubChCfg[i].pTDFrmNum[j];
                }
            }
        }
    }

    HL_SetVinInfo(HL_MTX_OPT_SET, VinId, &VinInfo);

    return Rval;
}

static inline UINT32 HL_LvCfgVinCapEmbdImpl(UINT16 VinId, const AMBA_DSP_VIN_SUB_CHAN_CFG_s *pSubChCfg)
{
    UINT32 Rval;
    CTX_VIN_INFO_s VinInfo = {0};

    // Embd data shall only exit on physical vin
    HL_GetVinInfo(HL_MTX_OPT_GET, VinId, &VinInfo);
    Rval = dsp_osal_memcpy(&VinInfo.EmbdCapWin, &pSubChCfg[0U].CaptureWindow, sizeof(AMBA_DSP_WINDOW_s));
    HL_SetVinInfo(HL_MTX_OPT_SET, VinId, &VinInfo);

    return Rval;
}

/**
* Liveview vin capture config function
* @param [in]  VinId Vin Index, Bit[15] means VirtualVin enable
* @param [in]  SubChNum Sub-channel num
* @param [in]  pSubChCfg array of Sub-channel config
* @return ErrorCode
*/
UINT32 dsp_liveview_cfg_vin_cap(UINT16 VinId, UINT16 SubChNum, const AMBA_DSP_VIN_SUB_CHAN_CFG_s *pSubChCfg)
{
    UINT32 Rval;
    UINT16 ActualVinId;

    Rval = HL_LiveviewConfigVinCaptureChk(VinId, SubChNum, pSubChCfg);

    /* Body */
    if (Rval == OK) {
        if (pSubChCfg == NULL) {
            Rval = DSP_ERR_0000;
            AmbaLL_LogUInt5("Null Input", 0U, 0U, 0U, 0U, 0U);
        } else {
            ActualVinId = (UINT16)DSP_GetU16Bit(VinId, 0U, 15U);

            if (1U == (UINT16)DSP_GetU16Bit(VinId, VIN_VIRT_IDX, 1U)) {
                ActualVinId += AMBA_DSP_MAX_VIN_NUM;
            }

            if (pSubChCfg[0U].Option == AMBA_DSP_VIN_CAP_OPT_EMBD) {
                Rval = HL_LvCfgVinCapEmbdImpl(ActualVinId, pSubChCfg);
            } else {
                Rval = HL_LvCfgVinCapImpl(ActualVinId, SubChNum, pSubChCfg);
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_LiveviewConfigVinCapture Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_LiveviewConfigVinPostChk(const UINT8 Type, UINT16 VinId)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pLiveviewConfigVinPost(Type, VinId);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pLiveviewConfigVinPost(Type, VinId);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pLiveviewConfigVinPost(Type, VinId);
    }

    /* Logic sanity check */
    return Rval;
}

/**
* Liveview vin post operation config function
* @param [in]  Type operation index
* @param [in]  VinId Vin Index
* @return ErrorCode
*/
UINT32 dsp_liveview_cfg_vin_post(const UINT8 Type, UINT16 VinId)
{
    UINT32 Rval;
    CTX_VIN_INFO_s VinInfo = {0};

    Rval = HL_LiveviewConfigVinPostChk(Type, VinId);

    /* Body */
    if (Rval == OK) {
        HL_GetVinInfo(HL_MTX_OPT_GET, VinId, &VinInfo);
        VinInfo.PostCfgType = Type;
        HL_SetVinInfo(HL_MTX_OPT_SET, VinId, &VinInfo);
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_LiveviewConfigVinPost Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_LiveviewFeedRawDataChk(UINT16 NumViewZone, const UINT16 *pViewZoneId, const AMBA_DSP_LIVEVIEW_EXT_RAW_s *pExtBuf)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pLiveviewFeedRawData(NumViewZone, pViewZoneId, pExtBuf);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pLiveviewFeedRawData(NumViewZone, pViewZoneId, pExtBuf);
    }
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pLiveviewFeedRawData(NumViewZone, pViewZoneId, pExtBuf);
    }
    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pLiveviewFeedRawData(NumViewZone, pViewZoneId, pExtBuf);
    }
    /* Logic sanity check */
    return Rval;
}

static inline UINT32 HL_LvFeedRawDataVinCmdSend(UINT16 VinId, const CTX_VIEWZONE_INFO_s *pViewZoneInfo)
{
    UINT32 Rval = OK;
    UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;

    if ((pViewZoneInfo->HdrBlendNumMinusOne > 0U) || (pViewZoneInfo->LinearCe > 0U)) {
        UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
        cmd_vin_ce_setup_t *CeSetp = HL_DefCtxCmdBufPtrVinCe;
        cmd_vin_hdr_setup_t *HdrSetup = HL_DefCtxCmdBufPtrVinHdr;
        UINT8 SkipCmd;

        SkipCmd = 0U;
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&CeSetp, &CmdBufferAddr);
        Rval = HL_FillVinCeSetup(VinId, CeSetp, &SkipCmd);
        if (Rval != OK) {
            //TBD
        } else if (SkipCmd == 1U) {
            //Skip cmd
        } else {
            Rval = AmbaHL_CmdVinCeSetup(WriteMode, CeSetp);
        }
        HL_RelCmdBuffer(CmdBufferId);

        if (Rval == OK) {
            if (pViewZoneInfo->HdrBlendNumMinusOne > 0U) {
                SkipCmd = (UINT8)0U;
                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&HdrSetup, &CmdBufferAddr);
                Rval = HL_FillVinHdrSetup(VinId, HdrSetup, pViewZoneInfo->StartIsoCfgAddr, &SkipCmd);
                if (Rval != OK) {
                    //TBD
                } else if (SkipCmd == 1U) {
                    //Skip cmd
                } else {
                    Rval = AmbaHL_CmdVinHdrSetup(WriteMode, HdrSetup);
                }
                HL_RelCmdBuffer(CmdBufferId);
            }
        }
    }
    return Rval;
}

static inline UINT32 HL_LvFeedRawOnSndData(const UINT8 WriteMode,
                                           const UINT16 VinId,
                                           const UINT16 ViewZoneId,
                                           const AMBA_DSP_LIVEVIEW_EXT_RAW_s *pExtBuf,
                                           const CTX_VIEWZONE_INFO_s *pViewZoneInfo,
                                           cmd_vin_set_ext_mem_t *pVinExtMem,
                                           UINT32 *RawCapSequenceNumber)
{
    UINT32 Rval = OK;
    ULONG ULAddr = 0x0U;
    UINT32 *pBufTblAddr = NULL;
    UINT32 PhysAddr = 0U;

    if ((pViewZoneInfo->HdrBlendNumMinusOne > 0U) || (pViewZoneInfo->LinearCe == 1U)) {
        pVinExtMem->memory_type = DSP_VIN_EXT_MEM_TYPE_HDS;
        pVinExtMem->buf_pitch = pExtBuf->CeBuffer.Pitch;
        pVinExtMem->buf_width = pExtBuf->CeBuffer.Pitch;
        pVinExtMem->buf_height = pExtBuf->CeBuffer.Window.Height;

        HL_GetPointerToDspExtRawBufArray(VinId, 1U/*Aux*/, &pBufTblAddr);
        (void)dsp_osal_virt2cli(pExtBuf->CeBuffer.BaseAddr, &PhysAddr);
        pBufTblAddr[0U] = PhysAddr;
        dsp_osal_typecast(&ULAddr, &pBufTblAddr);
        (void)dsp_osal_virt2cli(ULAddr, &pVinExtMem->buf_addr);

        Rval = AmbaHL_CmdVinSetExtMem(WriteMode, pVinExtMem);
    }

    if (Rval == OK) {
        UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
        UINT32 NewWp = 0U;
        UINT32 *pBatchQAddr, BatchCmdId = 0U;
        CTX_VIEWZONE_INFO_s *pTmpViewZoneInfo = HL_CtxViewZoneInfoPtr;
        cmd_vin_send_input_data_t *FeedData = HL_DefCtxCmdBufPtrVinSndData;

        Rval = HL_LvFeedRawDataVinCmdSend(VinId, pViewZoneInfo);
        if (Rval == OK) {
            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&FeedData, &CmdBufferAddr);
            FeedData->vin_id = (UINT8)VinId;
            FeedData->chan_id = 0U; //obsoleted
            FeedData->send_data_dest = DSP_VIN_SEND_IN_DATA_DEST_VPROC;
            FeedData->is_compression_en = pExtBuf->RawBuffer.Compressed;
            FeedData->blk_sz = (pExtBuf->RawBuffer.Compressed > 0U)? (UINT8)4U: (UINT8)0U;
            FeedData->mantissa = (pExtBuf->RawBuffer.Compressed > 0U)? (UINT8)6U: (UINT8)0U;
            FeedData->raw_frm_cap_cnt = (pExtBuf->CapSequence > 0U)? (UINT32)pExtBuf->CapSequence: RawCapSequenceNumber[VinId];
            FeedData->input_data_type = DSP_VIN_SEND_IN_DATA_RAW;
            FeedData->ext_fb_idx = DSP_FB_IDX_FOR_NEW2APPEND;
            if ((pViewZoneInfo->HdrBlendNumMinusOne > 0U) || (pViewZoneInfo->LinearCe == 1U)) {
                FeedData->ext_ce_fb_idx = DSP_FB_IDX_FOR_NEW2APPEND;
            }
            FeedData->vproc_hflip_control = HL_IS_HFLIP(pViewZoneInfo->MainRotate);
            FeedData->vproc_vflip_control = HL_IS_VFLIP(pViewZoneInfo->MainRotate);

            HL_GetViewZoneInfoLock(ViewZoneId, &pTmpViewZoneInfo);
            /* Reguest BatchCmdQ buffer */
            Rval = DSP_ReqBuf(&pTmpViewZoneInfo->BatchQPoolDesc, 1U, &NewWp, 1U/*FullnessCheck*/);
            if (Rval != OK) {
                HL_GetViewZoneInfoUnLock(ViewZoneId);
                AmbaLL_LogWarnUInt5((UINT16)AMBALLLOG_WARN_BATCHQ_FULL, "[LiveviewFeedRawData] ViewZone(%d) batch pool is full",
                    ViewZoneId, 0U, 0U, 0U, 0U);
            } else {
                ULONG BatchQAddr = 0U;
                cmd_vproc_ik_config_t *pIsoCfgCmd;

                pTmpViewZoneInfo->BatchQPoolDesc.Wp = NewWp;
                HL_GetViewZoneInfoUnLock(ViewZoneId);
                HL_GetPointerToDspBatchQ(ViewZoneId,
                                         (UINT16)NewWp,
                                         &pBatchQAddr,
                                         &BatchCmdId);

                /* Reset New BatchQ after Wp advanced */
                HL_ResetDspBatchQ(pBatchQAddr);

                /* IsoCfgCmd occupied first CmdSlot */
                dsp_osal_typecast(&pIsoCfgCmd, &pBatchQAddr);
                (void)HL_FillIsoCfgUpdate(ViewZoneId, pIsoCfgCmd);
                (void)dsp_osal_virt2cli(pViewZoneInfo->StartIsoCfgAddr, &pIsoCfgCmd->idsp_flow_addr);
                pIsoCfgCmd->ik_cfg_id = pViewZoneInfo->StartIsoCfgIndex;

                //Fill BatchCmdEnd
//                    pBatchQAddr[CMD_SIZE_IN_WORD] = MAX_BATCH_CMD_END;

                dsp_osal_typecast(&BatchQAddr, &pBatchQAddr);
                (void)dsp_osal_virt2cli(BatchQAddr, &FeedData->batch_cmd_set_info.addr);
                HL_SetDspBatchQInfo(BatchCmdId,
                                    1/*AutoReset*/,
                                    BATCHQ_INFO_ISO_CFG,
                                    pViewZoneInfo->StartIsoCfgIndex);
                FeedData->batch_cmd_set_info.id = BatchCmdId;
                FeedData->batch_cmd_set_info.size = HL_GetBatchCmdNumber(BatchQAddr)*CMD_SIZE_IN_BYTE;

                Rval = AmbaHL_CmdVinSendInputData(WriteMode, FeedData);

                if (pViewZoneInfo->UsedStartIsoCfg == 0U) {
                    HL_GetViewZoneInfoLock(ViewZoneId, &pTmpViewZoneInfo);
                    pTmpViewZoneInfo->UsedStartIsoCfg = 1U;
                    HL_GetViewZoneInfoUnLock(ViewZoneId);
                }

                if (Rval == OK) {
                    if (pExtBuf->IsLast > 0U) {
                        RawCapSequenceNumber[VinId] = 0U;
                    } else {
                        if (pExtBuf->CapSequence == 0U) {
                            RawCapSequenceNumber[VinId]++;
                        }
                    }
                }
            }
            HL_RelCmdBuffer(CmdBufferId);
        }
    }

    return Rval;
}

/**
* Liveview raw feeding function
* @param [in]  NumViewZone number of viewzone
* @param [in]  pViewZoneId viewzone index
* @param [in]  pExtBuf raw buffer information
* @return ErrorCode
*/
UINT32 dsp_liveview_feed_raw_data(UINT16 NumViewZone,
                                  const UINT16 *pViewZoneId,
                                  const AMBA_DSP_LIVEVIEW_EXT_RAW_s *pExtBuf)
{
    static UINT32 RawCapSequenceNumber[DSP_VIN_MAX_NUM] = {0};
    UINT32 Rval, Idx;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_VIN_INFO_s VinInfo = {0};
    ULONG ULAddr = 0x0U;
    UINT32 *pBufTblAddr = NULL;
    UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
    UINT16 VinId;
    UINT32 PhysAddr = 0U;

    Rval = HL_LiveviewFeedRawDataChk(NumViewZone, pViewZoneId, pExtBuf);

    /* Body */
    if (Rval == OK) {
        for (Idx = 0U; Idx < NumViewZone; Idx++) {
            HL_GetViewZoneInfoPtr(pViewZoneId[Idx], &ViewZoneInfo);

            if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM) {
                UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
                cmd_vin_set_ext_mem_t *VinExtMem = HL_DefCtxCmdBufPtrVinExtMem;

                VinId = (UINT16)DSP_GetU16Bit(pExtBuf[Idx].VinId, 0U, 15U);
                if (1U == DSP_GetU16Bit(pExtBuf[Idx].VinId, VIN_VIRT_IDX, 1U)) {
                    VinId += AMBA_DSP_MAX_VIN_NUM;
                }

                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&VinExtMem, &CmdBufferAddr);
                VinExtMem->vin_id_or_chan_id = (UINT8)VinId;
                VinExtMem->memory_type = DSP_VIN_EXT_MEM_TYPE_RAW;
                VinExtMem->allocation_type = VIN_EXT_MEM_ALLOC_TYPE_DISTINCT;
                HL_GetVinInfo(HL_MTX_OPT_GET, VinId, &VinInfo);
                VinExtMem->allocation_mode = (VinInfo.IsSetPinExtMem == 0U)? VIN_EXT_MEM_MODE_NEW: VIN_EXT_MEM_MODE_APPEND;
                if (VinInfo.IsSetPinExtMem == 0U) {
                    VinInfo.IsSetPinExtMem = 1U;
                }
                HL_SetVinInfo(HL_MTX_OPT_SET, VinId, &VinInfo);
                VinExtMem->overflow_cntl = VIN_EXT_MEM_CTRL_WAIT_APPEND;
                VinExtMem->num_frm_buf = MAX_EXT_MEM_BUF_NUM;
#if 0
                if (VinExtMem->num_frm_buf > EXT_MEM_VIN_MAX_NUM) {
                    AmbaLL_LogUInt5("[%d]Only %d ExtMem supported", __LINE__, EXT_MEM_VIN_MAX_NUM, 0U, 0U, 0U);
                }
#endif
                VinExtMem->chroma_format = DSP_YUV_MONO;
                VinExtMem->luma_img_ofs_x = 0U;
                VinExtMem->luma_img_ofs_y = 0U;
                VinExtMem->chroma_img_ofs_x = 0U;
                VinExtMem->chroma_img_ofs_y = 0U;
                VinExtMem->img_width = 0U;
                VinExtMem->img_height = 0U;
                VinExtMem->aux_pitch = 0U;
                VinExtMem->aux_width = 0U;
                VinExtMem->aux_height = 0U;
                VinExtMem->aux_img_ofs_x = 0U;
                VinExtMem->aux_img_ofs_y = 0U;
                VinExtMem->aux_img_width = 0U;
                VinExtMem->aux_img_height = 0U;
                VinExtMem->buf_pitch = pExtBuf[Idx].RawBuffer.Pitch;
                VinExtMem->buf_width = pExtBuf[Idx].RawBuffer.Pitch;
                VinExtMem->buf_height = pExtBuf[Idx].RawBuffer.Window.Height;

                HL_GetPointerToDspExtRawBufArray(VinId, 0U/*Raw*/, &pBufTblAddr);
                (void)dsp_osal_virt2cli(pExtBuf[Idx].RawBuffer.BaseAddr, &PhysAddr);
                pBufTblAddr[0U] = PhysAddr;
                dsp_osal_typecast(&ULAddr, &pBufTblAddr);
                (void)dsp_osal_virt2cli(ULAddr, &VinExtMem->buf_addr);

                Rval = AmbaHL_CmdVinSetExtMem(WriteMode, VinExtMem);

                if (Rval == OK) {
                    Rval = HL_LvFeedRawOnSndData(WriteMode,
                                                 VinId,
                                                 pViewZoneId[Idx],
                                                 &pExtBuf[Idx],
                                                 ViewZoneInfo,
                                                 VinExtMem,
                                                 RawCapSequenceNumber);
                    if (Rval != OK) {
                        AmbaLL_LogUInt5("HL_LvFeedRawOnSndData Error[0x%X] vz:%u vin:%u",
                                Rval, pViewZoneId[Idx], VinId, 0U, 0U);
                    }
                }
                HL_RelCmdBuffer(CmdBufferId);
            } else {
                AmbaLL_LogUInt5("Unsupported input format[%d]", ViewZoneInfo->InputFromMemory, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0001;
            }
            if (Rval != OK) {
                break;
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_LiveviewFeedRawData Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_LvFeedYuvDataOnMem(UINT16 ViewZoneId,
                                           const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf)
{
    UINT8 IsFirstMem = 1U;
    UINT8 VprocState = DSP_GetVprocState((UINT8)ViewZoneId);
    UINT8 RescChanged = (HL_GetRescState() == HL_RESC_CONFIGED)? 1U: 0U;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT16 VinId;
    UINT32 Rval = OK;
    UINT32 NewWp = 0U;
    AMBA_DSP_BUF_s ExtYuvBuf = {0};
    AMBA_DSP_BUF_s ExtYuvAuxBuf = {0};
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_VPROC_INFO_s VprocInfo = {0};
    cmd_vin_set_ext_mem_t *VinExtMem = HL_DefCtxCmdBufPtrVinExtMem;
    cmd_vin_send_input_data_t *FeedData = HL_DefCtxCmdBufPtrVinSndData;
    UINT8 WriteMode = AMBA_DSP_CMD_VCAP_NORMAL_WRITE;

    HL_GetVprocInfo(HL_MTX_OPT_ALL, ViewZoneId, &VprocInfo);

    /* 1.Previous liveview used internal vproc then switched to another liveview,
     *   since internal vproc does not has stopped flow until next liveview start,
     *   user must called ResourceChanged in this case
     * 2.Not boot/init yet
     * 3.DSP_VPROC_STATUS_IDLE2RUN is to identify raw encode case*/
    if ((VprocInfo.Status != DSP_VPROC_STATUS_IDLE2RUN) &&
         ((RescChanged == 1U) ||
          (DSP_VPROC_STATUS_ACTIVE != VprocState))) {

        /* Store use input pExtYuvBuf into ViewZoneInfo->ExtYuvBuf */
        HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
        (void)dsp_osal_memcpy(&ViewZoneInfo->ExtYuvBuf, &pExtYuvBuf->ExtYuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
        HL_GetViewZoneInfoUnLock(ViewZoneId);
    } else {
        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
        DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &VinId);
        WriteMode = HL_GetVin2CmdNormalWrite(VinId);

        /* First time use vin_ext_mem, use allocation_mode=VIN_EXT_MEM_MODE_NEW
         * Other feed use VIN_EXT_MEM_MODE_APPEND */
        if (ViewZoneInfo->YuvFrameCnt >0U) {
            IsFirstMem = 0U;
            /* Move ExtYuvBufIdx forward,
             * ucode memory map idx maximum is: EXT_MEM_VIN_MAX_NUM,
             * If you always use allocation_mode=VIN_EXT_MEM_MODE_NEW,
             * You don't need to update cmd_vin_send_input_data_t.ext_fb_idx
             */
            ViewZoneInfo->ExtYuvBufIdx = (UINT16)((ViewZoneInfo->ExtYuvBufIdx + 1U) % EXT_MEM_VIN_MAX_NUM);
        } else {
            IsFirstMem = 1U;
        }
        ExtYuvBuf.BaseAddr = pExtYuvBuf->ExtYuvBuf.BaseAddrY;
        ExtYuvBuf.Pitch = pExtYuvBuf->ExtYuvBuf.Pitch;
        ExtYuvBuf.Window.OffsetX = pExtYuvBuf->ExtYuvBuf.Window.OffsetX;
        ExtYuvBuf.Window.OffsetY = pExtYuvBuf->ExtYuvBuf.Window.OffsetY;
        ExtYuvBuf.Window.Width   = pExtYuvBuf->ExtYuvBuf.Window.Width;
        ExtYuvBuf.Window.Height  = pExtYuvBuf->ExtYuvBuf.Window.Height;
        ExtYuvAuxBuf.BaseAddr = pExtYuvBuf->ExtYuvBuf.BaseAddrUV;

        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&VinExtMem, &CmdBufferAddr);
        (void)HL_LiveviewFillVinExtMem(VinId,
                                       0U /*IsYuvVin2Enc*/,
                                       IsFirstMem,
                                       DSP_VIN_EXT_MEM_TYPE_YUV422 /* MemType */,
                                       YuvFmtTable[pExtYuvBuf->ExtYuvBuf.DataFmt] /* ChromaFmt */,
                                       VIN_EXT_MEM_CTRL_WAIT_APPEND /* OverFlowCtrl */,
                                       &ExtYuvBuf,
                                       &ExtYuvAuxBuf,
                                       VinExtMem);
        (void)AmbaHL_CmdVinSetExtMem(WriteMode, VinExtMem);
        HL_RelCmdBuffer(CmdBufferId);

        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&FeedData, &CmdBufferAddr);
        FeedData->vin_id = (UINT8)VinId;
        FeedData->chan_id = 0U; //obsoleted
        FeedData->send_data_dest = DSP_VIN_SEND_IN_DATA_DEST_VPROC;
        FeedData->is_compression_en = 0U;
        FeedData->blk_sz = 0U;
        FeedData->mantissa = 0U;
        FeedData->raw_frm_cap_cnt = ViewZoneInfo->YuvFrameCnt;
        FeedData->input_data_type = (pExtYuvBuf->ExtYuvBuf.DataFmt == AMBA_DSP_YUV420) ? (UINT32)DSP_VIN_SEND_IN_DATA_YUV420: (UINT32)DSP_VIN_SEND_IN_DATA_YUV422;
        FeedData->ext_fb_idx = DSP_FB_IDX_FOR_NEW2APPEND;

        HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
        /* Request BatchCmdQ buffer */
        Rval = DSP_ReqBuf(&ViewZoneInfo->BatchQPoolDesc, 1U, &NewWp, 1U/*FullnessCheck*/);
        if (Rval != OK) {
            HL_GetViewZoneInfoUnLock(ViewZoneId);
            AmbaLL_LogWarnUInt5((UINT16)AMBALLLOG_WARN_BATCHQ_FULL, "[LiveviewFeedYuvData] ViewZone[%d] batch pool is full", ViewZoneId, 0U, 0U, 0U, 0U);
        } else {
            UINT32 *pBatchQAddr, BatchCmdId;
            cmd_vproc_ik_config_t *pIsoCfgCmd;
            ULONG BatchQAddr = 0U;
            ViewZoneInfo->BatchQPoolDesc.Wp = NewWp;
            HL_GetViewZoneInfoUnLock(ViewZoneId);
            HL_GetPointerToDspBatchQ(ViewZoneId, (UINT16)ViewZoneInfo->BatchQPoolDesc.Wp, &pBatchQAddr, &BatchCmdId);

            /* Reset New BatchQ after Wp advanced */
            HL_ResetDspBatchQ(pBatchQAddr);

            /* IsoCfgCmd occupied first CmdSlot */
            dsp_osal_typecast(&pIsoCfgCmd, &pBatchQAddr);
            (void)HL_FillIsoCfgUpdate(ViewZoneId, pIsoCfgCmd);
            (void)dsp_osal_virt2cli(ViewZoneInfo->StartIsoCfgAddr, &pIsoCfgCmd->idsp_flow_addr);
            pIsoCfgCmd->ik_cfg_id = ViewZoneInfo->StartIsoCfgIndex;

            dsp_osal_typecast(&BatchQAddr, &pBatchQAddr);
            (void)dsp_osal_virt2cli(BatchQAddr, &FeedData->batch_cmd_set_info.addr);
            HL_SetDspBatchQInfo(BatchCmdId, 1/*AutoReset*/, BATCHQ_INFO_ISO_CFG, ViewZoneInfo->StartIsoCfgIndex);
            FeedData->batch_cmd_set_info.id = BatchCmdId;
            FeedData->batch_cmd_set_info.size = HL_GetBatchCmdNumber(BatchQAddr)*CMD_SIZE_IN_BYTE;

            FeedData->encode_start_idc = IsFirstMem;

            (void)AmbaHL_CmdVinSendInputData(WriteMode, FeedData);

            HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
            ViewZoneInfo->YuvFrameCnt++;
            if (ViewZoneInfo->UsedStartIsoCfg == 0U) {
                ViewZoneInfo->UsedStartIsoCfg = 1U;
            }
            HL_GetViewZoneInfoUnLock(ViewZoneId);
        }
        HL_RelCmdBuffer(CmdBufferId);
    }

    return Rval;
}

static inline UINT32 HL_LvFeedYuvDataOnDec(UINT16 ViewZoneId,
                                           const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf)
{
    UINT8 IsFirstMem = 1U;
    UINT8 WriteMode = AMBA_DSP_CMD_VCAP_NORMAL_WRITE;
    UINT8 RescChanged = (HL_GetRescState() == HL_RESC_CONFIGED)? 1U: 0U;
    UINT8 VprocState = DSP_GetVprocState((UINT8)ViewZoneId);
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT16 VinId;
    UINT16 DecIdx = 0U;
    UINT32 Rval = OK;
    UINT32 NewWp = 0U;
    CTX_VID_DEC_INFO_s VidDecInfo = {0};
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    cmd_vin_set_ext_mem_t *VinExtMem = HL_DefCtxCmdBufPtrVinExtMem;
    cmd_vin_send_input_data_t *FeedData = HL_DefCtxCmdBufPtrVinSndData;

    HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
    DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &DecIdx);

    /* 1.Previous liveview used internal vproc then switched to another liveview,
     * since internal vproc does not has stopped flow until next liveview start,
     * user must called ResourceChanged in this case */
    /* 2.Not boot/init yet */
    if ((RescChanged == 1U) ||
        (DSP_VPROC_STATUS_ACTIVE != VprocState)) {
        /* Store use input pExtYuvBuf into VidDecInfo.ExtYuvBuf */
        HL_GetVidDecInfo(HL_MTX_OPT_GET, DecIdx, &VidDecInfo);
        (void)dsp_osal_memcpy(&VidDecInfo.ExtYuvBuf, &pExtYuvBuf->ExtYuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
        (void)dsp_osal_memcpy(&VidDecInfo.LastYuvBuf, &pExtYuvBuf->ExtYuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
        HL_SetVidDecInfo(HL_MTX_OPT_SET, DecIdx, &VidDecInfo);
    } else {
        Rval = HL_LvBindDecVprocToVirtVin(DecIdx);
        if (Rval != OK) {
            AmbaLL_LogUInt5("HL_LvFeedYuvDataOnDec LvBind fail %d", Rval, 0U, 0U, 0U, 0U);
        }

        if (Rval == OK) {
            AMBA_DSP_BUF_s ExtYuvBuf = {0};

            HL_GetVidDecInfo(HL_MTX_OPT_GET, DecIdx, &VidDecInfo);
            VinId = VidDecInfo.YuvInVirtVinId + AMBA_DSP_MAX_VIN_NUM;
            /* store ExtYuvBuf info */
            (void)dsp_osal_memcpy(&VidDecInfo.ExtYuvBuf, &pExtYuvBuf->ExtYuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
            (void)dsp_osal_memcpy(&VidDecInfo.LastYuvBuf, &pExtYuvBuf->ExtYuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));

            /* First time use vin_ext_mem, use allocation_mode=VIN_EXT_MEM_MODE_NEW
             * Other feed use VIN_EXT_MEM_MODE_APPEND */
            if (VidDecInfo.YuvFrameCnt > 0U) {
                IsFirstMem = 0U;
                /* Move ExtYuvBufIdx forward,
                 * ucode memory map idx maximum is: EXT_MEM_VIN_MAX_NUM,
                 * If you always use allocation_mode=VIN_EXT_MEM_MODE_NEW,
                 * You don't need to update cmd_vin_send_input_data_t.ext_fb_idx
                 */
                VidDecInfo.ExtYuvBufIdx = (UINT16)((VidDecInfo.ExtYuvBufIdx + 1U) % EXT_MEM_VIN_MAX_NUM);
            } else {
                IsFirstMem = 1U;
            }
            ExtYuvBuf.BaseAddr = pExtYuvBuf->ExtYuvBuf.BaseAddrY;
            ExtYuvBuf.Pitch = pExtYuvBuf->ExtYuvBuf.Pitch;
            ExtYuvBuf.Window.OffsetX = pExtYuvBuf->ExtYuvBuf.Window.OffsetX;
            ExtYuvBuf.Window.OffsetY = pExtYuvBuf->ExtYuvBuf.Window.OffsetY;
            ExtYuvBuf.Window.Width   = pExtYuvBuf->ExtYuvBuf.Window.Width;
            ExtYuvBuf.Window.Height  = pExtYuvBuf->ExtYuvBuf.Window.Height;
            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VinExtMem, &CmdBufferAddr);
            /* This is Yuv to Vproc case, not to encoder */
            (void)HL_LiveviewFillVinExtMem(VinId,
                                           0U/*IsYuvVin2Enc*/,
                                           IsFirstMem,
                                           DSP_VIN_EXT_MEM_TYPE_YUV422 /* MemType */,
                                           YuvFmtTable[pExtYuvBuf->ExtYuvBuf.DataFmt],
                                           VIN_EXT_MEM_CTRL_WAIT_APPEND /* OverFlowCtrl */,
                                           &ExtYuvBuf,
                                           NULL,
                                           VinExtMem);
            /* we suggest user to have 16align on Luma buffer height */
            VinExtMem->buf_height = ALIGN_NUM16(VinExtMem->buf_height, 16U);
            (void)AmbaHL_CmdVinSetExtMem(AMBA_DSP_CMD_VCAP_NORMAL_WRITE, VinExtMem);
            HL_RelCmdBuffer(CmdBufferId);

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&FeedData, &CmdBufferAddr);
            FeedData->vin_id = (UINT8)VinId;
            FeedData->chan_id = 0U; //obsoleted
            FeedData->send_data_dest = DSP_VIN_SEND_IN_DATA_DEST_VPROC;
            FeedData->is_compression_en = 0U;
            FeedData->blk_sz = 0U;
            FeedData->mantissa = 0U;
            FeedData->raw_frm_cap_cnt = VidDecInfo.YuvFrameCnt;
            FeedData->input_data_type = (pExtYuvBuf->ExtYuvBuf.DataFmt == AMBA_DSP_YUV420)? (UINT32)DSP_VIN_SEND_IN_DATA_YUV420: (UINT32)DSP_VIN_SEND_IN_DATA_YUV422;
            FeedData->ext_fb_idx = DSP_FB_IDX_FOR_NEW2APPEND;

            HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
            if (ViewZoneInfo->StartIsoCfgAddr > 0U) {
                /* Request BatchCmdQ buffer */
                Rval = DSP_ReqBuf(&ViewZoneInfo->BatchQPoolDesc, 1U, &NewWp, 1U/*FullnessCheck*/);
                if (Rval != OK) {
                    HL_GetViewZoneInfoUnLock(ViewZoneId);
                    AmbaLL_LogWarnUInt5((UINT16)AMBALLLOG_WARN_BATCHQ_FULL, "[LiveviewFeedYuvData] ViewZone[%d] batch pool is full", ViewZoneId, 0U, 0U, 0U, 0U);
                } else {
                    UINT32 *pBatchQAddr, BatchCmdId;
                    cmd_vproc_ik_config_t *pIsoCfgCmd;
                    ULONG BatchQAddr = 0U;
                    ViewZoneInfo->BatchQPoolDesc.Wp = NewWp;
                    HL_GetViewZoneInfoUnLock(ViewZoneId);
                    HL_GetPointerToDspBatchQ(ViewZoneId, (UINT16)ViewZoneInfo->BatchQPoolDesc.Wp, &pBatchQAddr, &BatchCmdId);

                    /* Reset New BatchQ after Wp advanced */
                    HL_ResetDspBatchQ(pBatchQAddr);

                    /* IsoCfgCmd occupied first CmdSlot */
                    dsp_osal_typecast(&pIsoCfgCmd, &pBatchQAddr);
                    (void)HL_FillIsoCfgUpdate(ViewZoneId, pIsoCfgCmd);
                    (void)dsp_osal_virt2cli(ViewZoneInfo->StartIsoCfgAddr, &pIsoCfgCmd->idsp_flow_addr);
                    pIsoCfgCmd->ik_cfg_id = ViewZoneInfo->StartIsoCfgIndex;

                    dsp_osal_typecast(&BatchQAddr, &pBatchQAddr);
                    (void)dsp_osal_virt2cli(BatchQAddr, &FeedData->batch_cmd_set_info.addr);
                    HL_SetDspBatchQInfo(BatchCmdId, 1/*AutoReset*/, BATCHQ_INFO_ISO_CFG, ViewZoneInfo->StartIsoCfgIndex);
                    FeedData->batch_cmd_set_info.id = BatchCmdId;
                    FeedData->batch_cmd_set_info.size = HL_GetBatchCmdNumber(BatchQAddr)*CMD_SIZE_IN_BYTE;
                    (void)AmbaHL_CmdVinSendInputData(WriteMode, FeedData);
                }
            } else {
                HL_GetViewZoneInfoUnLock(ViewZoneId);
                (void)AmbaHL_CmdVinSendInputData(WriteMode, FeedData);
            }
            HL_RelCmdBuffer(CmdBufferId);

            VidDecInfo.YuvFrameCnt++;
            HL_SetVidDecInfo(HL_MTX_OPT_SET, DecIdx, &VidDecInfo);
        } else {
            AmbaLL_LogUInt5("LvFeedYuv [%d]FpBind fail", __LINE__, 0U, 0U, 0U, 0U);
        }
    }

    return Rval;
}

static inline UINT32 HL_LiveviewFeedYuvDataChk(UINT16 NumViewZone, const UINT16 *pViewZoneId, const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();
    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pLiveviewFeedYuvData(NumViewZone, pViewZoneId, pExtYuvBuf);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pLiveviewFeedYuvData(NumViewZone, pViewZoneId, pExtYuvBuf);
    }
    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pLiveviewFeedYuvData(NumViewZone, pViewZoneId, pExtYuvBuf);
    }
    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pLiveviewFeedYuvData(NumViewZone, pViewZoneId, pExtYuvBuf);
    }

    /* Logic sanity check */
    return Rval;
}

/**
* Liveview yuv feeding function
* @param [in]  NumViewZone number of viewzone
* @param [in]  pViewZoneId viewzone index
* @param [in]  pExtYuvBuf yuv buffer information
* @return ErrorCode
*/
UINT32 dsp_liveview_feed_yuv_data(UINT16 NumViewZone, const UINT16 *pViewZoneId, const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf)
{
    UINT32 Rval;
    UINT16 i;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    Rval = HL_LiveviewFeedYuvDataChk(NumViewZone, pViewZoneId, pExtYuvBuf);

    /* Body */
    if (Rval == OK) {
        for (i=0U; i<NumViewZone; i++) {
            HL_GetViewZoneInfoPtr(pViewZoneId[i], &ViewZoneInfo);

            if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_420) ||
                (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_422)) {
                Rval = HL_LvFeedYuvDataOnMem(pViewZoneId[i], &pExtYuvBuf[i]);
            /* After boot && VIN_SRC_FROM_DEC */
            } else if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) {
                Rval = HL_LvFeedYuvDataOnDec(pViewZoneId[i], &pExtYuvBuf[i]);
            } else {
                AmbaLL_LogUInt5("LvFeedYuv Unsupported input format[%d]", ViewZoneInfo->InputFromMemory, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0001;
            }
            if (Rval != OK) {
                break;
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_LiveviewFeedYuvData Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_LiveviewGetIDspCfgChk(UINT16 ViewZoneId, const ULONG *CfgAddr)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pLiveviewGetIDspCfg(ViewZoneId, CfgAddr);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pLiveviewGetIDspCfg(ViewZoneId, CfgAddr);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pLiveviewGetIDspCfg(ViewZoneId, CfgAddr);
    }

    /* Logic sanity check */
    return Rval;
}

static inline void HL_IdspAddrConvertAAA(IDSP_INFO_s *pIdspInfo)
{
    ULONG PhysAddr;
    UINT16 i, j;

    /* CfaAaa */
    for (i = 0U; i<IDSP_MAX_AAA_SLICE_NUM; i++) {
        (void)dsp_osal_virt2phys(pIdspInfo->CfaAAA.Addr[i], &PhysAddr);
        pIdspInfo->CfaAAA.Addr[i] = PhysAddr;
    }
    (void)dsp_osal_virt2phys(pIdspInfo->CfaAAA.DstAddr, &PhysAddr);
    pIdspInfo->CfaAAA.DstAddr = PhysAddr;

    /* PgAAA */
    for (i = 0U; i<IDSP_MAX_AAA_SLICE_NUM; i++) {
        (void)dsp_osal_virt2phys(pIdspInfo->PgAAA.Addr[i], &PhysAddr);
        pIdspInfo->PgAAA.Addr[i] = PhysAddr;
    }
    (void)dsp_osal_virt2phys(pIdspInfo->PgAAA.DstAddr, &PhysAddr);
    pIdspInfo->PgAAA.DstAddr = PhysAddr;

    /* HistAAA */
    for (j = 0U; j<DSP_VIN_HIST_NUM; j++) {
        for (i = 0U; i<IDSP_MAX_AAA_SLICE_NUM; i++) {
            (void)dsp_osal_virt2phys(pIdspInfo->HistAAA[j].Addr[i], &PhysAddr);
            pIdspInfo->HistAAA[j].Addr[i] = PhysAddr;
        }
        (void)dsp_osal_virt2phys(pIdspInfo->HistAAA[j].DstAddr, &PhysAddr);
        pIdspInfo->HistAAA[j].DstAddr = PhysAddr;
    }
}

static inline void HL_IdspAddrConvertTile(IDSP_INFO_s *pIdspInfo)
{
    ULONG PhysAddr;
    UINT16 i, j;

    /* Sec2TileCfg */
    for (j = 0U; j<DSP_C2Y_TILE_Y_NUM; j++) {
        for (i = 0U; i<DSP_C2Y_TILE_X_NUM; i++) {
            (void)dsp_osal_virt2phys(pIdspInfo->Sec2TileCfg[i][j].CfgAddr, &PhysAddr);
            pIdspInfo->Sec2TileCfg[i][j].CfgAddr = PhysAddr;
        }
    }

    /* Sec3TileCfg */
    for (j = 0U; j < DSP_C2Y_TILE_Y_NUM; j++) {
        for (i = 0U; i < DSP_C2Y_TILE_X_NUM; i++) {
            (void)dsp_osal_virt2phys(pIdspInfo->Sec3TileCfg[i][j].CfgAddr, &PhysAddr);
            pIdspInfo->Sec3TileCfg[i][j].CfgAddr = PhysAddr;
        }
    }

    /* Sec4TileCfg */
    for (j = 0U; j < DSP_C2Y_TILE_Y_NUM; j++) {
        for (i = 0U; i < DSP_C2Y_TILE_X_NUM; i++) {
            (void)dsp_osal_virt2phys(pIdspInfo->Sec4TileCfg[i][j].CfgAddr, &PhysAddr);
            pIdspInfo->Sec4TileCfg[i][j].CfgAddr = PhysAddr;
        }
    }

    /* MctfTileCfg */
    for (j = 0U; j < DSP_C2Y_TILE_Y_NUM; j++) {
        for (i = 0U; i < DSP_C2Y_TILE_X_NUM; i++) {
            (void)dsp_osal_virt2phys(pIdspInfo->MctfTileCfg[i][j].CfgAddr, &PhysAddr);
            pIdspInfo->MctfTileCfg[i][j].CfgAddr = PhysAddr;
        }
    }
}

static inline void HL_IdspAddrConvertHiso(IDSP_INFO_s *pIdspInfo)
{
    ULONG PhysAddr;
    UINT16 i;

    /* HISO */
    for (i = 0U; i<DSP_HISO_STEP_NUM; i++) {
        (void)dsp_osal_virt2phys(pIdspInfo->HISO[i].Sec2Cfg.CfgAddr, &PhysAddr);
        pIdspInfo->HISO[i].Sec2Cfg.CfgAddr = PhysAddr;
        (void)dsp_osal_virt2phys(pIdspInfo->HISO[i].Sec4Cfg.CfgAddr, &PhysAddr);
        pIdspInfo->HISO[i].Sec4Cfg.CfgAddr = PhysAddr;
        (void)dsp_osal_virt2phys(pIdspInfo->HISO[i].Sec5Cfg.CfgAddr, &PhysAddr);
        pIdspInfo->HISO[i].Sec5Cfg.CfgAddr = PhysAddr;
        (void)dsp_osal_virt2phys(pIdspInfo->HISO[i].Sec6Cfg.CfgAddr, &PhysAddr);
        pIdspInfo->HISO[i].Sec6Cfg.CfgAddr = PhysAddr;
        (void)dsp_osal_virt2phys(pIdspInfo->HISO[i].Sec7Cfg.CfgAddr, &PhysAddr);
        pIdspInfo->HISO[i].Sec7Cfg.CfgAddr = PhysAddr;
        (void)dsp_osal_virt2phys(pIdspInfo->HISO[i].Sec9Cfg.CfgAddr, &PhysAddr);
        pIdspInfo->HISO[i].Sec9Cfg.CfgAddr = PhysAddr;
        (void)dsp_osal_virt2phys(pIdspInfo->HISO[i].MctfCfg.CfgAddr, &PhysAddr);
        pIdspInfo->HISO[i].MctfCfg.CfgAddr = PhysAddr;
        (void)dsp_osal_virt2phys(pIdspInfo->HISO[i].MctsCfg.CfgAddr, &PhysAddr);
        pIdspInfo->HISO[i].MctsCfg.CfgAddr = PhysAddr;

        (void)dsp_osal_virt2phys(pIdspInfo->HISO[i].Sec2.LumaAddr, &PhysAddr);
        pIdspInfo->HISO[i].Sec2.LumaAddr = PhysAddr;
        (void)dsp_osal_virt2phys(pIdspInfo->HISO[i].Sec2.ChromaAddr, &PhysAddr);
        pIdspInfo->HISO[i].Sec2.ChromaAddr = PhysAddr;
        (void)dsp_osal_virt2phys(pIdspInfo->HISO[i].Sec5.LumaAddr, &PhysAddr);
        pIdspInfo->HISO[i].Sec5.LumaAddr = PhysAddr;
        (void)dsp_osal_virt2phys(pIdspInfo->HISO[i].Sec5.ChromaAddr, &PhysAddr);
        pIdspInfo->HISO[i].Sec5.ChromaAddr = PhysAddr;
        (void)dsp_osal_virt2phys(pIdspInfo->HISO[i].Sec6.LumaAddr, &PhysAddr);
        pIdspInfo->HISO[i].Sec6.LumaAddr = PhysAddr;
        (void)dsp_osal_virt2phys(pIdspInfo->HISO[i].Sec6.ChromaAddr, &PhysAddr);
        pIdspInfo->HISO[i].Sec6.ChromaAddr = PhysAddr;
        (void)dsp_osal_virt2phys(pIdspInfo->HISO[i].Sec7.LumaAddr, &PhysAddr);
        pIdspInfo->HISO[i].Sec7.LumaAddr = PhysAddr;
        (void)dsp_osal_virt2phys(pIdspInfo->HISO[i].Sec7.ChromaAddr, &PhysAddr);
        pIdspInfo->HISO[i].Sec7.ChromaAddr = PhysAddr;
        (void)dsp_osal_virt2phys(pIdspInfo->HISO[i].Sec9.LumaAddr, &PhysAddr);
        pIdspInfo->HISO[i].Sec9.LumaAddr = PhysAddr;
        (void)dsp_osal_virt2phys(pIdspInfo->HISO[i].Sec9.ChromaAddr, &PhysAddr);
        pIdspInfo->HISO[i].Sec9.ChromaAddr = PhysAddr;
        (void)dsp_osal_virt2phys(pIdspInfo->HISO[i].Mctf.LumaAddr, &PhysAddr);
        pIdspInfo->HISO[i].Mctf.LumaAddr = PhysAddr;
        (void)dsp_osal_virt2phys(pIdspInfo->HISO[i].Mctf.ChromaAddr, &PhysAddr);
        pIdspInfo->HISO[i].Mctf.ChromaAddr = PhysAddr;
        (void)dsp_osal_virt2phys(pIdspInfo->HISO[i].Mcts.LumaAddr, &PhysAddr);
        pIdspInfo->HISO[i].Mcts.LumaAddr = PhysAddr;
        (void)dsp_osal_virt2phys(pIdspInfo->HISO[i].Mcts.ChromaAddr, &PhysAddr);
        pIdspInfo->HISO[i].Mcts.ChromaAddr = PhysAddr;
    }
}

static inline void HL_IdspCfgGetViewZoneId(const UINT16 ViewZoneId,
                                           UINT16 *pId)
{
    UINT16 Id = (UINT8)DSP_GetU16Bit(ViewZoneId, DSP_VIEWZONE_ID_BIT_IDX, DSP_VIEWZONE_ID_LEN);
    UINT16 Type = (UINT8)DSP_GetU16Bit(ViewZoneId, DSP_VIEWZONE_TYPE_IDX, DSP_VIEWZONE_TYPE_LEN);
    CTX_STILL_INFO_s StlInfo = {0};

    *pId = DSP_VPROC_IDX_INVALID;

    HL_GetStlInfo(HL_MTX_OPT_ALL, &StlInfo);
    if ((Type == DSP_VIEWZONE_TYPE_STL_R2Y) ||
        (Type == DSP_VIEWZONE_TYPE_STL_R2R)) {
        if (StlInfo.RawInVprocId != DSP_VPROC_IDX_INVALID) {
            *pId = StlInfo.RawInVprocId;
        }
    } else if (Type == DSP_VIEWZONE_TYPE_STL_Y2Y) {
        if (StlInfo.YuvInVprocId != DSP_VPROC_IDX_INVALID) {
            *pId = StlInfo.YuvInVprocId;
        }
    } else {
        /* TYPE_LV */
        *pId = Id;
    }
}

UINT32 dsp_liveview_get_idsp_cfg(UINT16 ViewZoneId, ULONG *CfgAddr)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    UINT8 ProfState = DSP_GetProfState();
    UINT8 IsCameraMode, IsValidViewZoneInfo = (UINT8)0U;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 VinId, MappedViewZoneId = 0U;
    CTX_VIN_INFO_s VinInfo = {0};
    ULONG PhysAddr;

    Rval = HL_LiveviewGetIDspCfgChk(ViewZoneId, CfgAddr);

    /* Body */
    if (Rval == OK) {
        HL_IdspCfgGetViewZoneId(ViewZoneId, &MappedViewZoneId);
        if (MappedViewZoneId != DSP_VPROC_IDX_INVALID) {
            HL_GetViewZoneInfoPtr(MappedViewZoneId, &ViewZoneInfo);
            IsValidViewZoneInfo = (UINT8)1U;
        }

        IsCameraMode = ((ProfState == DSP_PROF_STATUS_CAMERA) || (ProfState == DSP_PROF_STATUS_SAFETY))? 1U: 0U;

        if ((IsCameraMode == 1U) &&
            (IsValidViewZoneInfo == 1U)) {
            const IDSP_INFO_s *pSrcIdspInfo;
            IDSP_INFO_s *pIdspInfo;
            ULONG DstAddr;
#ifdef USE_STAT_IDSP_BUF
            DSP_MEM_BLK_t MemBlk = {0};
#endif

            if (ViewZoneInfo->LastIdspCfgAddr != 0x0U) {
                dsp_osal_typecast(&pSrcIdspInfo, &ViewZoneInfo->LastIdspCfgAddr);
#ifdef USE_STAT_IDSP_BUF
                (void)DSP_GetStatBuf(DSP_STAT_BUF_IDSP, AMBA_DSP_MAX_VIEWZONE_NUM/*for the query one*/, &MemBlk);
                dsp_osal_typecast(&pIdspInfo, &MemBlk.Base);
                (void)dsp_osal_memcpy(pIdspInfo, pSrcIdspInfo, sizeof(IDSP_INFO_s));
                DstAddr = MemBlk.Base;
#else
                dsp_osal_typecast(&pIdspInfo, &pSrcIdspInfo);
                DstAddr = ViewZoneInfo->LastIdspCfgAddr;
#endif

                /* C2Y */
                (void)dsp_osal_virt2phys(pIdspInfo->C2Y.Sec2Cfg.CfgAddr, &PhysAddr);
                pIdspInfo->C2Y.Sec2Cfg.CfgAddr = PhysAddr;
                (void)dsp_osal_virt2phys(pIdspInfo->C2Y.Sec4Cfg.CfgAddr, &PhysAddr);
                pIdspInfo->C2Y.Sec4Cfg.CfgAddr = PhysAddr;
                (void)dsp_osal_virt2phys(pIdspInfo->C2Y.Sec2R2y.LumaAddr, &PhysAddr);
                pIdspInfo->C2Y.Sec2R2y.LumaAddr = PhysAddr;
                (void)dsp_osal_virt2phys(pIdspInfo->C2Y.Sec2R2y.ChromaAddr, &PhysAddr);
                pIdspInfo->C2Y.Sec2R2y.ChromaAddr = PhysAddr;
                (void)dsp_osal_virt2phys(pIdspInfo->C2Y.Sec2Me1.Addr, &PhysAddr);
                pIdspInfo->C2Y.Sec2Me1.Addr = PhysAddr;
                /* WarpMctf */
                (void)dsp_osal_virt2phys(pIdspInfo->vWarpMctf.Sec3Cfg.CfgAddr, &PhysAddr);
                pIdspInfo->vWarpMctf.Sec3Cfg.CfgAddr = PhysAddr;
                (void)dsp_osal_virt2phys(pIdspInfo->vWarpMctf.MctfCfg.CfgAddr, &PhysAddr);
                pIdspInfo->vWarpMctf.MctfCfg.CfgAddr = PhysAddr;
                (void)dsp_osal_virt2phys(pIdspInfo->vWarpMctf.MctsCfg.CfgAddr, &PhysAddr);
                pIdspInfo->vWarpMctf.MctsCfg.CfgAddr = PhysAddr;
                (void)dsp_osal_virt2phys(pIdspInfo->vWarpMctf.Sec3Me1.Addr, &PhysAddr);
                pIdspInfo->vWarpMctf.Sec3Me1.Addr = PhysAddr;
                (void)dsp_osal_virt2phys(pIdspInfo->vWarpMctf.Mctf.LumaAddr, &PhysAddr);
                pIdspInfo->vWarpMctf.Mctf.LumaAddr = PhysAddr;
                (void)dsp_osal_virt2phys(pIdspInfo->vWarpMctf.Mctf.ChromaAddr, &PhysAddr);
                pIdspInfo->vWarpMctf.Mctf.ChromaAddr = PhysAddr;
                (void)dsp_osal_virt2phys(pIdspInfo->vWarpMctf.Mcts.LumaAddr, &PhysAddr);
                pIdspInfo->vWarpMctf.Mcts.LumaAddr = PhysAddr;
                (void)dsp_osal_virt2phys(pIdspInfo->vWarpMctf.Mcts.ChromaAddr, &PhysAddr);
                pIdspInfo->vWarpMctf.Mcts.ChromaAddr = PhysAddr;
                (void)dsp_osal_virt2phys(pIdspInfo->vWarpMctf.Mctf2bit.Addr, &PhysAddr);
                pIdspInfo->vWarpMctf.Mctf2bit.Addr = PhysAddr;
                /* Me1 */
                (void)dsp_osal_virt2phys(pIdspInfo->Me1Smooth.Sec2Cfg.CfgAddr, &PhysAddr);
                pIdspInfo->Me1Smooth.Sec2Cfg.CfgAddr = PhysAddr;
                (void)dsp_osal_virt2phys(pIdspInfo->Me1Smooth.Sec6Cfg.CfgAddr, &PhysAddr);
                pIdspInfo->Me1Smooth.Sec6Cfg.CfgAddr = PhysAddr;
                (void)dsp_osal_virt2phys(pIdspInfo->Me1Smooth.Sec5Cfg.CfgAddr, &PhysAddr);
                pIdspInfo->Me1Smooth.Sec5Cfg.CfgAddr = PhysAddr;
                (void)dsp_osal_virt2phys(pIdspInfo->Me1Smooth.vWarpMe1.Addr, &PhysAddr);
                pIdspInfo->Me1Smooth.vWarpMe1.Addr = PhysAddr;
                (void)dsp_osal_virt2phys(pIdspInfo->Me1Smooth.Me1Dn3.Addr, &PhysAddr);
                pIdspInfo->Me1Smooth.Me1Dn3.Addr = PhysAddr;
                /* Md1 */
                (void)dsp_osal_virt2phys(pIdspInfo->Md.Sec2Cfg[0U].CfgAddr, &PhysAddr);
                pIdspInfo->Md.Sec2Cfg[0U].CfgAddr = PhysAddr;
                (void)dsp_osal_virt2phys(pIdspInfo->Md.Sec2Cfg[1U].CfgAddr, &PhysAddr);
                pIdspInfo->Md.Sec2Cfg[1U].CfgAddr = PhysAddr;
                (void)dsp_osal_virt2phys(pIdspInfo->Md.Sec2Cfg[2U].CfgAddr, &PhysAddr);
                pIdspInfo->Md.Sec2Cfg[2U].CfgAddr = PhysAddr;
                (void)dsp_osal_virt2phys(pIdspInfo->Md.Sec6Cfg.CfgAddr, &PhysAddr);
                pIdspInfo->Md.Sec6Cfg.CfgAddr = PhysAddr;
                (void)dsp_osal_virt2phys(pIdspInfo->Md.Sec5Cfg.CfgAddr, &PhysAddr);
                pIdspInfo->Md.Sec5Cfg.CfgAddr = PhysAddr;
                (void)dsp_osal_virt2phys(pIdspInfo->Md.MctfCfg.CfgAddr, &PhysAddr);
                pIdspInfo->Md.MctfCfg.CfgAddr = PhysAddr;
                (void)dsp_osal_virt2phys(pIdspInfo->Md.MdMbB.Addr, &PhysAddr);
                pIdspInfo->Md.MdMbB.Addr = PhysAddr;
                (void)dsp_osal_virt2phys(pIdspInfo->Md.MdMbC.Addr, &PhysAddr);
                pIdspInfo->Md.MdMbC.Addr = PhysAddr;
                (void)dsp_osal_virt2phys(pIdspInfo->Md.MoAsf.Addr, &PhysAddr);
                pIdspInfo->Md.MoAsf.Addr = PhysAddr;

                HL_IdspAddrConvertAAA(pIdspInfo);

                /* VinCfg */
                (void)dsp_osal_virt2phys(pIdspInfo->VinCfg.VinSecCfg.CfgAddr, &PhysAddr);
                pIdspInfo->VinCfg.VinSecCfg.CfgAddr = PhysAddr;
                DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &VinId);
                HL_GetVinInfo(HL_MTX_OPT_ALL, VinId, &VinInfo);
                (void)dsp_osal_virt2phys(VinInfo.CompandTableAddr, &PhysAddr);
                pIdspInfo->VinCfg.CompandTblAddr = PhysAddr;
                (void)dsp_osal_virt2phys(VinInfo.DeCompandTableAddr, &PhysAddr);
                pIdspInfo->VinCfg.DeCompandTblAddr = PhysAddr;

                HL_IdspAddrConvertTile(pIdspInfo);

                HL_IdspAddrConvertHiso(pIdspInfo);

                /* MainY12Cfg */
                (void)dsp_osal_virt2phys(pIdspInfo->MainY12.Addr, &PhysAddr);
                pIdspInfo->MainY12.Addr = PhysAddr;

                /* V2P */
                (void)dsp_osal_virt2phys(DstAddr, &PhysAddr);
                *CfgAddr = PhysAddr;
            } else {
                Rval = DSP_ERR_0004;; ErrLine = __LINE__;
            }

#if 0 //debug purpose
            if (Rval == OK) {
                AmbaLL_LogUInt5("IdspDump[ViewZone 0x%X %d] Chan[%d]", ViewZoneId, MappedViewZoneId, pIdspInfo->ChannelId, 0U, 0U);
                AmbaLL_LogUInt5("  Sec1[%d @ 0x%X]", pIdspInfo->VinCfg.VinSecCfg.CfgSize, pIdspInfo->VinCfg.VinSecCfg.CfgAddr, 0U, 0U, 0U);
                AmbaLL_LogUInt5("  C2YTile[0x%X %d][0x%X %d]", pIdspInfo->Sec2TileCfg[0U][0U].CfgAddr, pIdspInfo->Sec2TileCfg[0U][0U].CfgSize, pIdspInfo->Sec2TileCfg[1U][0U].CfgAddr, pIdspInfo->Sec2TileCfg[1U][0U].CfgSize, 0U);
                AmbaLL_LogUInt5("         [0x%X %d][0x%X %d]", pIdspInfo->Sec2TileCfg[2U][0U].CfgAddr, pIdspInfo->Sec2TileCfg[2U][0U].CfgSize, pIdspInfo->Sec2TileCfg[3U][0U].CfgAddr, pIdspInfo->Sec2TileCfg[3U][0U].CfgSize, 0U);
                AmbaLL_LogUInt5("  Sec4Tile[0x%X %d][0x%X %d]", pIdspInfo->Sec4TileCfg[0U][0U].CfgAddr, pIdspInfo->Sec4TileCfg[0U][0U].CfgSize, pIdspInfo->Sec4TileCfg[1U][0U].CfgAddr, pIdspInfo->Sec4TileCfg[1U][0U].CfgSize, 0U);
                AmbaLL_LogUInt5("          [0x%X %d][0x%X %d]", pIdspInfo->Sec4TileCfg[2U][0U].CfgAddr, pIdspInfo->Sec4TileCfg[2U][0U].CfgSize, pIdspInfo->Sec4TileCfg[3U][0U].CfgAddr, pIdspInfo->Sec4TileCfg[3U][0U].CfgSize, 0U);
                AmbaLL_LogUInt5("  C2Y[%d] Sec2[%d @ 0x%X] Sec4[%d @ 0x%X]", pIdspInfo->C2Y.CapSeqNo, pIdspInfo->C2Y.Sec2Cfg.CfgSize, pIdspInfo->C2Y.Sec2Cfg.CfgAddr, pIdspInfo->C2Y.Sec4Cfg.CfgSize, pIdspInfo->C2Y.Sec4Cfg.CfgAddr);
                AmbaLL_LogUInt5("          Luma[0x%X] P[%d] Dim[%dx%d]", pIdspInfo->C2Y.Sec2R2y.LumaAddr, pIdspInfo->C2Y.Sec2R2y.LumaPitch, pIdspInfo->C2Y.Sec2R2y.LumaWidth, pIdspInfo->C2Y.Sec2R2y.LumaHeight, 0U);
                AmbaLL_LogUInt5("          Chroma[0x%X] P[%d] Dim[%dx%d]", pIdspInfo->C2Y.Sec2R2y.ChromaAddr, pIdspInfo->C2Y.Sec2R2y.ChromaPitch, pIdspInfo->C2Y.Sec2R2y.ChromaWidth, pIdspInfo->C2Y.Sec2R2y.ChromaHeight, 0U);
                AmbaLL_LogUInt5("  vWarp[%d] Sec3[%d @ 0x%X]", pIdspInfo->vWarpMctf.CapSeqNo, pIdspInfo->vWarpMctf.Sec3Cfg.CfgSize, pIdspInfo->vWarpMctf.Sec3Cfg.CfgAddr, 0U, 0U);
                AmbaLL_LogUInt5("            Mctf[%d @ 0x%X] Mcts[%d @ 0x%X]", pIdspInfo->vWarpMctf.MctfCfg.CfgSize, pIdspInfo->vWarpMctf.MctfCfg.CfgAddr, pIdspInfo->vWarpMctf.MctsCfg.CfgSize, pIdspInfo->vWarpMctf.MctsCfg.CfgAddr, 0U);
                AmbaLL_LogUInt5("            Mctf Luma[0x%X] P[%d] Dim[%dx%d]", pIdspInfo->vWarpMctf.Mctf.LumaAddr, pIdspInfo->vWarpMctf.Mctf.LumaPitch, pIdspInfo->vWarpMctf.Mctf.LumaWidth, pIdspInfo->vWarpMctf.Mctf.LumaHeight, 0U);
                AmbaLL_LogUInt5("                 Chroma[0x%X] P[%d] Dim[%dx%d]", pIdspInfo->vWarpMctf.Mctf.ChromaAddr, pIdspInfo->vWarpMctf.Mctf.ChromaPitch, pIdspInfo->vWarpMctf.Mctf.ChromaWidth, pIdspInfo->vWarpMctf.Mctf.ChromaHeight, 0U);
                AmbaLL_LogUInt5("            Mcts Luma[0x%X] P[%d] Dim[%dx%d]", pIdspInfo->vWarpMctf.Mcts.LumaAddr, pIdspInfo->vWarpMctf.Mcts.LumaPitch, pIdspInfo->vWarpMctf.Mcts.LumaWidth, pIdspInfo->vWarpMctf.Mcts.LumaHeight, 0U);
                AmbaLL_LogUInt5("                 Chroma[0x%X] P[%d] Dim[%dx%d]", pIdspInfo->vWarpMctf.Mcts.ChromaAddr, pIdspInfo->vWarpMctf.Mcts.ChromaPitch, pIdspInfo->vWarpMctf.Mcts.ChromaWidth, pIdspInfo->vWarpMctf.Mcts.ChromaHeight, 0U);
                AmbaLL_LogUInt5("  Sec3Tile[0x%X %d][0x%X %d]", pIdspInfo->Sec3TileCfg[0U][0U].CfgAddr, pIdspInfo->Sec3TileCfg[0U][0U].CfgSize, pIdspInfo->Sec3TileCfg[1U][0U].CfgAddr, pIdspInfo->Sec3TileCfg[1U][0U].CfgSize, 0U);
                AmbaLL_LogUInt5("          [0x%X %d][0x%X %d]", pIdspInfo->Sec3TileCfg[2U][0U].CfgAddr, pIdspInfo->Sec3TileCfg[2U][0U].CfgSize, pIdspInfo->Sec3TileCfg[3U][0U].CfgAddr, pIdspInfo->Sec3TileCfg[3U][0U].CfgSize, 0U);
                AmbaLL_LogUInt5("  MctfTile[0x%X %d][0x%X %d]", pIdspInfo->MctfTileCfg[0U][0U].CfgAddr, pIdspInfo->MctfTileCfg[0U][0U].CfgSize, pIdspInfo->MctfTileCfg[1U][0U].CfgAddr, pIdspInfo->MctfTileCfg[1U][0U].CfgSize, 0U);
                AmbaLL_LogUInt5("          [0x%X %d][0x%X %d]", pIdspInfo->MctfTileCfg[2U][0U].CfgAddr, pIdspInfo->MctfTileCfg[2U][0U].CfgSize, pIdspInfo->MctfTileCfg[3U][0U].CfgAddr, pIdspInfo->MctfTileCfg[3U][0U].CfgSize, 0U);

                AmbaLL_LogUInt5("  Cfa[%d] Slice[%dx%d] Addr[0x%X 0x%X]", pIdspInfo->CfaAAA.CapSeqNo,
                                        pIdspInfo->CfaAAA.SliceX, pIdspInfo->CfaAAA.SliceY,
                                        pIdspInfo->CfaAAA.Addr[0], pIdspInfo->CfaAAA.Addr[1]);
                AmbaLL_LogUInt5("  Pg[%d] Slice[%dx%d] Addr[0x%X 0x%X]", pIdspInfo->PgAAA.CapSeqNo,
                                        pIdspInfo->PgAAA.SliceX, pIdspInfo->PgAAA.SliceY,
                                        pIdspInfo->PgAAA.Addr[0], pIdspInfo->PgAAA.Addr[1]);
                AmbaLL_LogUInt5("  Hist_0[%d] Slice[%dx%d] Addr[0x%X 0x%X]", pIdspInfo->HistAAA[0].CapSeqNo,
                                        pIdspInfo->HistAAA[0].SliceX, pIdspInfo->HistAAA[0].SliceY,
                                        pIdspInfo->HistAAA[0].Addr[0], pIdspInfo->HistAAA[0].Addr[1]);
                AmbaLL_LogUInt5("  Hist_1[%d] Slice[%dx%d] Addr[0x%X 0x%X]", pIdspInfo->HistAAA[1].CapSeqNo,
                                        pIdspInfo->HistAAA[1].SliceX, pIdspInfo->HistAAA[1].SliceY,
                                        pIdspInfo->HistAAA[1].Addr[0], pIdspInfo->HistAAA[1].Addr[1]);
                AmbaLL_LogUInt5("  Hist_2[%d] Slice[%dx%d] Addr[0x%X 0x%X]", pIdspInfo->HistAAA[2].CapSeqNo,
                                        pIdspInfo->HistAAA[2].SliceX, pIdspInfo->HistAAA[2].SliceY,
                                        pIdspInfo->HistAAA[2].Addr[0], pIdspInfo->HistAAA[2].Addr[1]);
                AmbaLL_LogUInt5("  MainY12[0x%X] P[%d] Dim[%dx%d]", pIdspInfo->MainY12.Addr, pIdspInfo->MainY12.Pitch, pIdspInfo->MainY12.Width, pIdspInfo->MainY12.Height, 0U);
            }
#endif
        } else {
            Rval = DSP_ERR_0004; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_LiveviewGetIDspCfg Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_LvYuvStrmSyncStart(const CTX_RESOURCE_INFO_s *pResource,
                                           UINT16 YuvStrmIdx,
                                           const AMBA_DSP_LIVEVIEW_SYNC_CTRL_s *pYuvStrmSyncCtrl,
                                           UINT32 *pSyncJobId)
{
    UINT32 Rval;
    UINT16 i;
    UINT32 NewWp = 0U;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_EFCT_SYNC_JOB_SET_s *EfctSyncJobSet = HL_CtxYuvStrmEfctSyncJobSetPtr;
    CTX_YUVSTRM_EFCT_SYNC_CTRL_s *EfctSyncCtrl = HL_CtxYuvStrmEfctSyncCtrlPtr;

    for (i=0U; i<pResource->ViewZoneNum; i++) {
        if (1U == (UINT16)DSP_GetBit(pYuvStrmSyncCtrl->TargetViewZoneMask, (UINT32)i, 1U)) {
            HL_GetViewZoneInfoLock(i, &ViewZoneInfo);
            ViewZoneInfo->EffectSyncState = EFCT_SYNC_ON;
            HL_GetViewZoneInfoUnLock(i);
        }
    }

    HL_GetYuvStrmEfctSyncCtrlLock(YuvStrmIdx, &EfctSyncCtrl);
    Rval = DSP_ReqBuf(&EfctSyncCtrl->PoolDesc, 1U/*ReqSize*/, &NewWp, 1U/*FullnessCheck*/);
    HL_GetYuvStrmEfctSyncJobSetLock(YuvStrmIdx, (UINT16)NewWp, &EfctSyncJobSet);
    if (Rval == OK) {
        EfctSyncCtrl->PoolDesc.Wp = NewWp;

        /* Reset SyncJobSet and assign JobId */
        (void)dsp_osal_memset(EfctSyncJobSet, 0, sizeof(CTX_EFCT_SYNC_JOB_SET_s));
        (void)HL_ComposeEfctSyncJobId(&EfctSyncJobSet->JobId,
                                      YuvStrmIdx,
                                      (UINT16)(NewWp + 1U),
                                      EFCT_SYNC_JOB_ID_DONTCARE,
                                      EFCT_SYNC_JOB_ID_DONTCARE);
        (void)dsp_osal_memcpy(pSyncJobId, &EfctSyncJobSet->JobId, sizeof(UINT32));
        EfctSyncCtrl->State = EFCT_SYNC_ON;
        EfctSyncJobSet->Status = EFCT_SYNC_JOBSET_STATUS_CONFIG;
    }
    HL_GetYuvStrmEfctSyncJobSetUnLock(YuvStrmIdx, (UINT16)NewWp);
    HL_GetYuvStrmEfctSyncCtrlUnLock(YuvStrmIdx);

    return Rval;
}

static inline UINT32 HL_LvYuvStrmSyncExecOnLayoutChg(const CTX_RESOURCE_INFO_s *pResource,
                                                     const UINT16 YuvStrmIdx,
                                                     const UINT16 JobSetIdx,
                                                     CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout,
                                                     UINT16 MasterViewZoneIdx,
                                                     UINT16 LastViewZoneIdx,
                                                     UINT8 *pIsNewCmd)
{
    UINT32 Rval = OK;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 i, j, VinIdx;
    UINT8 WriteMode;
    ULONG CmdBufAddr = 0U, ULAddr;
    UINT16 BatchCmdIdx = 0U;
    const cmd_vin_attach_proc_cfg_to_cap_frm_t *pVinAttachProcCfg;
    vin_fov_batch_cmd_set_t *pBatchCmdSet;
    UINT32 *pBatchQAddr = NULL;
    UINT32 BatchCmdId = 0;
    UINT32 NewWp = 0U, U32Val, LocalViewZoneId;

    HL_GetViewZoneInfoLock(MasterViewZoneIdx, &ViewZoneInfo);
    DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &VinIdx);
    HL_GetViewZoneInfoUnLock(MasterViewZoneIdx);
    WriteMode = HL_GetVin2CmdNormalWrite(VinIdx);
    LocalViewZoneId = HL_GetViewZoneLocalIndexOnVin(MasterViewZoneIdx);
    HL_CtrlBatchQBufMtx(HL_MTX_OPT_GET, MasterViewZoneIdx);
    (void)AmbaLL_CmdQuery(WriteMode,
                          CMD_VIN_ATTACH_PROC_CFG_TO_CAP_FRM,
                          &CmdBufAddr,
                          (UINT32)VinIdx,
                          LocalViewZoneId);
    if (CmdBufAddr > 0U) {
        /* Cmd Exist */

        dsp_osal_typecast(&pVinAttachProcCfg, &CmdBufAddr);
        (void)dsp_osal_cli2virt(pVinAttachProcCfg->fov_batch_cmd_set_addr, &ULAddr);
        dsp_osal_typecast(&pBatchCmdSet, &ULAddr);
        (void)dsp_osal_cli2virt(pBatchCmdSet->batch_cmd_set_info.addr, &ULAddr);
        dsp_osal_typecast(&pBatchQAddr, &ULAddr);
        HL_GetDspBatchCmdId(ULAddr, MasterViewZoneIdx, &BatchCmdId);
#ifdef DEBUG_SYNC_ISO_CMD
        AmbaLL_LogUInt5("[StrmSync] Cmd Exist Id[0x%X] Addr[0x%X%X]", BatchCmdId, DSP_GetU64Msb((UINT64)CmdBufAddr), DSP_GetU64Lsb((UINT64)CmdBufAddr), 0U, 0U);
#endif
    } else {
        /* NewCmd in CurrentCmdQ, Forward BatchCmdQ WP */
        *pIsNewCmd = 1U;
        HL_GetViewZoneInfoLock(MasterViewZoneIdx, &ViewZoneInfo);

        /* Request BatchCmdQ buffer */
        Rval = DSP_ReqBuf(&ViewZoneInfo->BatchQPoolDesc, 1U, &NewWp, 1U/*FullnessCheck*/);
        if (Rval != OK) {
            HL_GetViewZoneInfoUnLock(MasterViewZoneIdx);
        } else {
            ViewZoneInfo->BatchQPoolDesc.Wp = NewWp;
            HL_GetPointerToDspBatchQ(MasterViewZoneIdx, (UINT16)ViewZoneInfo->BatchQPoolDesc.Wp, &pBatchQAddr, &BatchCmdId);
            HL_GetViewZoneInfoUnLock(MasterViewZoneIdx);

            /* Reset New BatchQ after Wp advanced */
            HL_ResetDspBatchQ(pBatchQAddr);
        }
        HL_GetPointerToDspBatchCmdSet(MasterViewZoneIdx, 0U/*Idx*/, &pBatchCmdSet);
#ifdef DEBUG_SYNC_ISO_CMD
        AmbaLL_LogUInt5("[StrmSync] NewCmd 0x%X Wp %d", BatchCmdId, NewWp, 0U, 0U, 0U);
#endif
    }

    if (Rval == OK) {
        UINT8 IsEfctYuvStrm;
        UINT16 BatchCmdNum;
        CTX_EFCT_SYNC_JOB_SET_s *EfctSyncJobSet = HL_CtxYuvStrmEfctSyncJobSetPtr;
        CTX_YUVSTRM_EFCT_SYNC_CTRL_s *EfctSyncCtrl = HL_CtxYuvStrmEfctSyncCtrlPtr;
        CTX_YUV_STRM_LAYOUT_s *EfctSyncJobSetLayout = HL_CtxYuvStrmEfctLayoutPtr;

        /* If user changes only VinROI in a non-effect channel, then there is no need to
         * re-issue the pp-stream-cfg of other effect channels
         * */
        IsEfctYuvStrm = (1U == HL_GetEffectEnableOnYuvStrm(YuvStrmIdx))? (UINT8)1U: (UINT8)0U;

        if (1U == IsEfctYuvStrm) {
            /*
             * Since uCode will reset all PpStrm setting after rcv any PpStrmCmd
             * we shall issue ther EfctYuvStrm PpStrmCmd as well even they dont change at all
             */
            for (j=0U; j<pResource->YuvStrmNum; j++) {
                cmd_vproc_multi_stream_pp_t* pPpStrmCfg;
                const UINT32* pU32Val = NULL;

                IsEfctYuvStrm = (1U == HL_GetEffectEnableOnYuvStrm(j))? (UINT8)1U: (UINT8)0U;

                if ((j != YuvStrmIdx) && (1U == IsEfctYuvStrm)) {
                    CTX_YUVSTRM_EFCT_SYNC_CTRL_s *pEfctSyncCtrl = HL_CtxYuvStrmEfctSyncCtrlPtr;
                    CTX_YUV_STRM_LAYOUT_s *pEfctSyncJobSetLayout = HL_CtxYuvStrmEfctLayoutPtr;
                    HL_GetYuvStrmEfctSyncCtrlLock(j, &pEfctSyncCtrl);
                    HL_GetYuvStrmEfctSyncJobSetLayoutLock(j, (UINT16)pEfctSyncCtrl->PoolDesc.Wp, &pEfctSyncJobSetLayout);
                    BatchCmdNum = BATCH_CMD_START_NUM + BatchCmdIdx;
                    pU32Val = &pBatchQAddr[CMD_SIZE_IN_WORD*BatchCmdNum];
                    dsp_osal_typecast(&pPpStrmCfg, &pU32Val);
                    (void)HL_FillPpStrmCfg(j, pPpStrmCfg, pEfctSyncJobSetLayout);
                    pPpStrmCfg->cmd_code = 0x2000064U; //CMD_VPROC_MULTI_STREAM_PP
                    /* PrintOut cmd for debug */
                    (void)AmbaHL_CmdVprocMultiStrmPpCmd(AMBA_DSP_CMD_NOP, pPpStrmCfg);
                    BatchCmdIdx++;
                    HL_GetYuvStrmEfctSyncCtrlUnLock(j);
                }
            }
        }
        HL_GetYuvStrmEfctSyncCtrlLock(YuvStrmIdx, &EfctSyncCtrl);
        HL_GetYuvStrmEfctSyncJobSetLock(YuvStrmIdx, JobSetIdx, &EfctSyncJobSet);
        HL_GetYuvStrmEfctSyncJobSetLayoutLock(YuvStrmIdx, JobSetIdx, &EfctSyncJobSetLayout);

        (void)HL_FillSyncJobIntoBatch(YuvStrmIdx, pYuvStrmLayout,
                                      MasterViewZoneIdx, LastViewZoneIdx,
                                      EfctSyncJobSet, EfctSyncJobSetLayout,
                                      pBatchQAddr, BatchCmdIdx);
        (void)dsp_osal_memcpy(&U32Val, &EfctSyncJobSet->JobId, sizeof(UINT32));

        HL_SetDspBatchQInfo(BatchCmdId, *pIsNewCmd/*AutoReset*/, BATCHQ_INFO_YUVSTRM_GRP, U32Val);
        pBatchCmdSet->vin_id = VinIdx;
        pBatchCmdSet->chan_id = (UINT16)HL_GetViewZoneLocalIndexOnVin(MasterViewZoneIdx);
        pBatchCmdSet->batch_cmd_set_info.id = BatchCmdId;
        dsp_osal_typecast(&ULAddr, &pBatchQAddr);
        (void)dsp_osal_virt2cli(ULAddr, &pBatchCmdSet->batch_cmd_set_info.addr);
        pBatchCmdSet->batch_cmd_set_info.size = HL_GetBatchCmdNumber(ULAddr)*CMD_SIZE_IN_BYTE;

        /* Update SyncState */
        EfctSyncCtrl->State = EFCT_SYNC_PROCESS;
        EfctSyncJobSet->Status = EFCT_SYNC_JOBSET_STATUS_PROC;

        /* Set_Fov_Layout for Vin ROI change */
        for (i=0; i<AMBA_DSP_MAX_YUVSTRM_VIEW_NUM; i++) {
            if ((EfctSyncJobSet->VinRoi[i].Width > 0U) &&
                (EfctSyncJobSet->VinRoi[i].Height > 0U)) {

                UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
                cmd_vin_set_fov_layout_t *FovLayout = HL_DefCtxCmdBufPtrVinLayout;

                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&FovLayout, &CmdBufferAddr);
                Rval = HL_FillVinSetFovLayout(FovLayout, i, &EfctSyncJobSet->VinRoi[i]);
                if (Rval != OK) {
                    HL_RelCmdBuffer(CmdBufferId);
                    AmbaLL_LogUInt5("SetFovLayout filling fail", 0U, 0U, 0U, 0U, 0U);
                    continue;
                }
                (void)AmbaHL_CmdVinSetFovLayout(WriteMode, FovLayout);
                HL_RelCmdBuffer(CmdBufferId);
            }
        }
        HL_GetYuvStrmEfctSyncJobSetUnLock(YuvStrmIdx, JobSetIdx);
        HL_GetYuvStrmEfctSyncCtrlUnLock(YuvStrmIdx);

        if ((*pIsNewCmd == 1U) && (pBatchCmdSet->batch_cmd_set_info.size > 0U)) {
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_vin_attach_proc_cfg_to_cap_frm_t *NewVinAttachCfg = HL_DefCtxCmdBufPtrVinAttfrm;

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&NewVinAttachCfg, &CmdBufferAddr);
            dsp_osal_typecast(&ULAddr, &pBatchCmdSet);
            (void)dsp_osal_virt2cli(ULAddr, &NewVinAttachCfg->fov_batch_cmd_set_addr);

            NewVinAttachCfg->vin_id = (UINT8)VinIdx;
            NewVinAttachCfg->fov_num = (UINT8)1U;
            (void)AmbaHL_CmdVinAttachCfgToCapFrm(WriteMode, NewVinAttachCfg);
            HL_RelCmdBuffer(CmdBufferId);
        }
    }
    HL_CtrlBatchQBufMtx(HL_MTX_OPT_SET, MasterViewZoneIdx);

    return Rval;
}

static inline UINT32 HL_LiveviewYuvStreamSyncChk(const UINT16 YuvStrmIdx,
                                                 const AMBA_DSP_LIVEVIEW_SYNC_CTRL_s *pYuvStrmSyncCtrl,
                                                 const UINT32 *pSyncJobId,
                                                 const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();
    CTX_YUVSTRM_EFCT_SYNC_CTRL_s EfctSyncCtrl = {0};

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pLiveviewYuvStreamSync(YuvStrmIdx, pYuvStrmSyncCtrl, pSyncJobId, pAttachedRawSeq);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pLiveviewYuvStreamSync(YuvStrmIdx, pYuvStrmSyncCtrl, pSyncJobId, pAttachedRawSeq);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pLiveviewYuvStreamSync(YuvStrmIdx, pYuvStrmSyncCtrl, pSyncJobId, pAttachedRawSeq);
    }
    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pLiveviewYuvStreamSync(YuvStrmIdx, pYuvStrmSyncCtrl, pSyncJobId, pAttachedRawSeq);
    }

    //TBD, MultiSync in one frame?
        /* Timing sanity check */
       if (Rval == OK) {
           HL_GetYuvStrmEfctSyncCtrl(HL_MTX_OPT_ALL, YuvStrmIdx, &EfctSyncCtrl);
           if ((pYuvStrmSyncCtrl->Opt == AMBA_DSP_YUVSTRM_SYNC_START) &&
               (EfctSyncCtrl.State == EFCT_SYNC_ON)) {
               Rval = DSP_ERR_0004;
               AmbaLL_LogUInt5("[%d]YuvStreamSync Locked [%d %d]", YuvStrmIdx, pYuvStrmSyncCtrl->Opt, EfctSyncCtrl.State, 0, 0);
           }
       }

    /* Logic sanity check */

    return Rval;
}

/**
* Liveview yuv stream sync function
* @param [in]  YuvStrmIdx Yuv stream index
* @param [in]  pYuvStrmSyncCtrl SyncCtrl setting
* @param [out] pSyncJobId SyncJobId for user to track
* @param [out] pAttachedRawSeq attached raw sequence
* @return ErrorCode
*/
UINT32 dsp_liveview_yuvstream_sync(UINT16 YuvStrmIdx, const AMBA_DSP_LIVEVIEW_SYNC_CTRL_s *pYuvStrmSyncCtrl, UINT32 *pSyncJobId, UINT64 *pAttachedRawSeq)
{
    UINT32 Rval;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout = NULL;
    CTX_YUVSTRM_EFCT_SYNC_CTRL_s *EfctSyncCtrl = HL_CtxYuvStrmEfctSyncCtrlPtr;
    CTX_EFCT_SYNC_JOB_SET_s *EfctSyncJobSet = HL_CtxYuvStrmEfctSyncJobSetPtr;
    CTX_YUV_STRM_LAYOUT_s *EfctSyncJobSetLayout = HL_CtxYuvStrmEfctLayoutPtr;
#ifdef SUPPORT_VPROC_DISABLE_BIT
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT32 *pBatchQAddr = NULL;
    UINT32 BatchCmdId = 0;
    UINT16 VinIdx,j;
    UINT32 NewWp = 0U;
    UINT8 WriteMode = AMBA_DSP_CMD_VCAP_NORMAL_WRITE;
    ULONG ULAddr;
#endif
    UINT8 IsNewCmd = 0U;
    UINT16 i;

    Rval = HL_LiveviewYuvStreamSyncChk(YuvStrmIdx, pYuvStrmSyncCtrl, pSyncJobId, pAttachedRawSeq);
    AmbaMisra_TouchUnused(pAttachedRawSeq);

    /* Body */
    if (Rval == OK) {
        HL_GetResourcePtr(&Resource);

        //Update GrpSync state
        if (pYuvStrmSyncCtrl->Opt == AMBA_DSP_YUVSTRM_SYNC_START) {
            Rval = HL_LvYuvStrmSyncStart(Resource,
                                         YuvStrmIdx,
                                         pYuvStrmSyncCtrl,
                                         pSyncJobId);
        } else if (pYuvStrmSyncCtrl->Opt == AMBA_DSP_YUVSTRM_SYNC_EXECUTE) {
            UINT16 MasterViewZoneIdx;   //General shall be first processed ViewZone in YuvStrm.ChanCfg
            UINT16 LastViewZoneIdx;     //General shall be last processed ViewZone in YuvStrm.ChanCfg
            UINT16 JobType, LayoutJobExisted = 0U;
            UINT16 JobSetIdx;
#ifdef SUPPORT_VPROC_DISABLE_BIT
            const cmd_vin_attach_proc_cfg_to_cap_frm_t *pVinAttachProcCfg;
            vin_fov_batch_cmd_set_t *pBatchCmdSet;
            ULONG ULAddr = 0U;
            ULONG CmdBufAddr = 0U;
            UINT32 DisabledStrmViewZoneOI = 0U;
#endif

            /* Sweep for MasterViewZoneIdx */
            HL_GetYuvStrmEfctSyncCtrlLock(YuvStrmIdx, &EfctSyncCtrl);
            JobSetIdx = (UINT16)EfctSyncCtrl->PoolDesc.Wp;
            HL_GetYuvStrmEfctSyncJobSetLock(YuvStrmIdx, JobSetIdx, &EfctSyncJobSet);
            HL_GetYuvStrmEfctSyncJobSetLayoutLock(YuvStrmIdx, JobSetIdx, &EfctSyncJobSetLayout);

            for (i=0U; i<EfctSyncJobSet->JobNum; i++) {
                JobType = (UINT16)EfctSyncJobSet->Job[i].JobId.JobTypeBit;

                if (1U == DSP_GetU16Bit(JobType, EFCT_SYNC_JOB_BIT_LAYOUT, 1U)) {
                    LayoutJobExisted = 1U;
                }
            }
            HL_GetYuvStrmInfoPtr(YuvStrmIdx, &YuvStrmInfo);

            if (LayoutJobExisted == 1U) {
                pYuvStrmLayout = EfctSyncJobSetLayout;
#ifdef SUPPORT_VPROC_DISABLE_BIT
                DisabledStrmViewZoneOI = YuvStrmInfo->MaxChanBitMask;
                for (i=0U; i < EfctSyncJobSetLayout->NumChan; i++) {
                    DSP_ClearBit(&DisabledStrmViewZoneOI, EfctSyncJobSetLayout->ChanCfg[i].ViewZoneId);
                }
#endif
            } else {
                pYuvStrmLayout = &YuvStrmInfo->Layout;
            }
#ifdef SUPPORT_VPROC_DISABLE_BIT
            MasterViewZoneIdx = YuvStrmInfo->Layout.CurrentMasterViewId;
            LastViewZoneIdx = pYuvStrmLayout->ChanCfg[pYuvStrmLayout->NumChan - 1U].ViewZoneId;
#else
            DSP_Bit2U16Idx(YuvStrmInfo->MaxChanBitMask, &MasterViewZoneIdx);
            DSP_ReverseBit2U16Idx(YuvStrmInfo->MaxChanBitMask, &LastViewZoneIdx);
#endif
            HL_GetYuvStrmEfctSyncCtrlUnLock(YuvStrmIdx);
            HL_GetYuvStrmEfctSyncJobSetUnLock(YuvStrmIdx, JobSetIdx);

#ifdef SUPPORT_VPROC_DISABLE_BIT
            HL_GetViewZoneInfoPtr(MasterViewZoneIdx, &ViewZoneInfo);
            DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &VinIdx);

            WriteMode = HL_GetVin2CmdNormalWrite(VinIdx);

            /* If there is a change of members in effect channel, there must also be a corresponding change in Vproc group,
             * i.e. the Vprocs removed from effect channel should also be disabled by cmd_vproc_multi_chan_proc_order,
             * and cmd_vproc_multi_chan_proc_order_t must be put in the batch right before the batch command for layout change.
             * Here, we wait a VDSP1 sync after updating vproc-order, so if there is a change of members, there will also be a
             * frame time delay between every layout change.
             * */
            if ((LayoutJobExisted == 1U) &&
                (Resource->ViewZoneDisableBit != DisabledStrmViewZoneOI)) {
                Rval = AmbaLL_CmdQuery(WriteMode, CMD_VIN_ATTACH_PROC_CFG_TO_CAP_FRM, &CmdBufAddr, (UINT32)VinIdx, (UINT32)MasterViewZoneIdx);
                if (Rval == OK) {
                    if (CmdBufAddr > 0U) {
                        /* Cmd Exist */
                        HL_CtrlBatchQBufMtx(HL_MTX_OPT_GET, MasterViewZoneIdx);

                        dsp_osal_typecast(&pVinAttachProcCfg, &CmdBufAddr);
                        (void)dsp_osal_cli2virt(pVinAttachProcCfg->fov_batch_cmd_set_addr, &ULAddr);
                        dsp_osal_typecast(&pBatchCmdSet, &ULAddr);
                        (void)dsp_osal_cli2virt(pBatchCmdSet->batch_cmd_set_info.addr, &ULAddr);
                        dsp_osal_typecast(&pBatchQAddr, &ULAddr);
                        HL_GetDspBatchCmdId(ULAddr, MasterViewZoneIdx, &BatchCmdId);
#ifdef DEBUG_SYNC_ISO_CMD
                        AmbaLL_LogUInt5("[StrmSync] Cmd Exist Id[0x%X] Addr[0x%X%X]", BatchCmdId, DSP_GetU64Msb((UINT64)CmdBufAddr), DSP_GetU64Lsb((UINT64)CmdBufAddr), 0U, 0U);
#endif
                    } else {
                        /* NewCmd in CurrentCmdQ, Forward BatchCmdQ WP */
                        IsNewCmd = 1U;
                        HL_GetViewZoneInfoLock(MasterViewZoneIdx, &ViewZoneInfo);

                        /* Request BatchCmdQ buffer */
                        Rval = DSP_ReqBuf(&ViewZoneInfo->BatchQPoolDesc, 1U, &NewWp, 1U/*FullnessCheck*/);
                        if (Rval != OK) {
                            HL_GetViewZoneInfoUnLock(MasterViewZoneIdx);
                        } else {
                            ViewZoneInfo->BatchQPoolDesc.Wp = NewWp;
                            HL_GetViewZoneInfoUnLock(MasterViewZoneIdx);
                            HL_GetPointerToDspBatchQ(MasterViewZoneIdx, (UINT16)ViewZoneInfo->BatchQPoolDesc.Wp, &pBatchQAddr, &BatchCmdId);

                            /* Reset New BatchQ after Wp advanced */
                            HL_ResetDspBatchQ(pBatchQAddr);
                        }
                        HL_GetPointerToDspBatchCmdSet(MasterViewZoneIdx, 0U/*Idx*/, &pBatchCmdSet);
#ifdef DEBUG_SYNC_ISO_CMD
                        AmbaLL_LogUInt5("[StrmSync] NewCmd 0x%X Wp %d", BatchCmdId, NewWp, 0U, 0U, 0U);
#endif
                    }
                }

                if (Rval == OK) {
                    /* VprocOrder */
                    UINT16 GrpId = 0U;
                    cmd_vproc_multi_chan_proc_order_t* ChanProcOrder = HL_DefCtxCmdBufPtrVpcChOrd;
                    UINT16 BatchCmdNum;
                    const UINT32* pU32Val;

                    BatchCmdNum = BATCH_CMD_START_NUM;
                    pU32Val = &pBatchQAddr[CMD_SIZE_IN_WORD*BatchCmdNum];

                    dsp_osal_typecast(&ChanProcOrder, &pU32Val);
                    Rval = HL_GetVprocGroupIdx(MasterViewZoneIdx, &GrpId, 0U/*IsStilProc*/);
                    if (Rval == OK) {
                        HL_UpdateVprocChanProcOrder((UINT8)GrpId, DisabledStrmViewZoneOI, ChanProcOrder, 1U/*DisableVproc*/);
                        /* PrintOut cmd for debug */
                        Rval = AmbaHL_CmdVprocMultiChanProcOrder(AMBA_DSP_CMD_NOP, ChanProcOrder);
                        if (Rval != OK) {
                            AmbaLL_LogUInt5("AmbaHL_CmdVprocMultiChanProcOrder fail", 0U, 0U, 0U, 0U, 0U);
                        } else {

                            HL_GetResourceLock(&Resource);
                            Resource->ViewZoneDisableBit = DisabledStrmViewZoneOI;
                            HL_GetResourceUnLock();

                            HL_SetDspBatchQInfo(BatchCmdId, IsNewCmd/*AutoReset*/, BATCHQ_INFO_YUVSTRM_GRP, 0U);
                            pBatchCmdSet->vin_id = VinIdx;
                            pBatchCmdSet->chan_id = (UINT16)HL_GetViewZoneLocalIndexOnVin(MasterViewZoneIdx);
                            pBatchCmdSet->batch_cmd_set_info.id = BatchCmdId;
                            dsp_osal_typecast(&ULAddr, &pBatchQAddr);
                            (void)dsp_osal_virt2cli(ULAddr, &pBatchCmdSet->batch_cmd_set_info.addr);
                            pBatchCmdSet->batch_cmd_set_info.size = HL_GetBatchCmdNumber(ULAddr)*CMD_SIZE_IN_BYTE;

                            if (IsNewCmd == 1U) {
                                UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
                                cmd_vin_attach_proc_cfg_to_cap_frm_t *VinAttachProcCfg = HL_DefCtxCmdBufPtrVinAttfrm;

                                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                                dsp_osal_typecast(&VinAttachProcCfg, &CmdBufferAddr);
                                dsp_osal_typecast(&ULAddr, &pBatchCmdSet);
                                (void)dsp_osal_virt2cli(ULAddr, &VinAttachProcCfg.fov_batch_cmd_set_addr);

                                VinAttachProcCfg->vin_id = (UINT8)VinIdx;
                                VinAttachProcCfg->fov_num = (UINT8)1U;
                                Rval = AmbaHL_CmdVinAttachCfgToCapFrm(WriteMode, VinAttachProcCfg);
                                HL_RelCmdBuffer(CmdBufferId);
                            } else {
                                HL_CtrlBatchQBufMtx(HL_MTX_OPT_SET, MasterViewZoneIdx);
                            }
                            if (Rval == OK) {
                                Rval = DSP_WaitVdspEvent(EVENT_FLAG_INT_VDSP1_EVENT, 1U, WAIT_FLAG_TIMEOUT);
                            }
                        }
                    }
                }
            }
#endif
#ifdef SUPPORT_VPROC_DISABLE_BIT
            if (Rval == OK) {
#endif
                // For effect layout change
                Rval = HL_LvYuvStrmSyncExecOnLayoutChg(Resource,
                                                       YuvStrmIdx,
                                                       JobSetIdx,
                                                       pYuvStrmLayout,
                                                       MasterViewZoneIdx,
                                                       LastViewZoneIdx,
                                                       &IsNewCmd);
#ifdef SUPPORT_VPROC_DISABLE_BIT
            }
#endif
        } else {
            //TBD
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_LiveviewYuvStreamSync Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }
    return Rval;
}

static inline UINT32 HL_LvUptGeoCfgCmdCpyToLocal(ULONG GeoCfgAddr, UINT8 *pIsGeoCfgUpdate)
{
    UINT32 Rval = OK;
    const idsp_grp_upt_info_t *pIdspGeoCfg = NULL;

    /* CV2FS use different WarpGrp config instead of CmdBased config */
    dsp_osal_typecast(&pIdspGeoCfg, &GeoCfgAddr);

    if ((pIdspGeoCfg->calib_update == (UINT8)1U) ||
        (pIdspGeoCfg->window_info_update == (UINT8)1U) ||
        (pIdspGeoCfg->phase_info_update == (UINT8)1U) ||
        (pIdspGeoCfg->aaa_stats_update == (UINT8)1U)) {
        *pIsGeoCfgUpdate = 1U;
    } else {
        *pIsGeoCfgUpdate = 0U;
    }

    return Rval;
}

static inline UINT32 HL_LvUptGeoCfgCmdCpyToDB(DSP_EFCT_SYNC_JOB_s *pSyncJob,
                                              ULONG GeoCfgAddr,
                                              UINT8 IsGeoCfgUpdate)
{
    UINT32 Rval = OK;
    cmd_vproc_warp_group_update_t *pDspWarpCmd = NULL;
    const DSP_CMD_s *pDspCmd;

    if (IsGeoCfgUpdate == (UINT32)1U) {
        pSyncJob->WarpUpdate = (UINT8)1U;

        pDspCmd = &pSyncJob->WarpGrpCmd;
        dsp_osal_typecast(&pDspWarpCmd, &pDspCmd);

        pDspWarpCmd->cmd_code = 0x200000DU/*CMD_VPROC_WARP_GROUP_UPDATE*/;
        pDspWarpCmd->channel_id = (UINT8)pSyncJob->ViewZoneIdx;
        (void)dsp_osal_virt2cli(GeoCfgAddr, &pDspWarpCmd->group_update_info_daddr);
    }

    return Rval;
}

static inline UINT32 HL_LiveviewUpdateGeoCfgChk(const UINT16 ViewZoneId,
                                                const AMBA_DSP_LIVEVIEW_GEOCFG_CTRL_s *pGeoCfgCtrl,
                                                const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pLiveviewUpdateGeoCfg(ViewZoneId, pGeoCfgCtrl, pAttachedRawSeq);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pLiveviewUpdateGeoCfg(ViewZoneId, pGeoCfgCtrl, pAttachedRawSeq);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pLiveviewUpdateGeoCfg(ViewZoneId, pGeoCfgCtrl, pAttachedRawSeq);
    }
    /* Timing sanity check */
    if (Rval == OK) {
        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
        if (ViewZoneInfo->EffectSyncState == EFCT_SYNC_OFF) {
            Rval = DSP_ERR_0004;
            AmbaLL_LogUInt5("[%d]UpdateGeoCfg invalid timing", ViewZoneId, 0, 0, 0, 0);
        }
    }
    /* Logic sanity check */
    return Rval;
}

/**
* Liveview GeoConfig update function
* @param [in]  ViewZoneId viewzone index
* @param [in]  GeoCfgId Geo config index
* @param [in]  GeoCfgAddr Geo config address
* @param [out] pAttachedRawSeq attached raw sequence
* @return ErrorCode
*/
UINT32 dsp_liveview_update_geocfg(UINT16 ViewZoneId, const AMBA_DSP_LIVEVIEW_GEOCFG_CTRL_s *pGeoCfgCtrl, UINT64 *pAttachedRawSeq)
{
    UINT32 Rval;
    UINT16 i;
    UINT8 ExitLoop;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_YUVSTRM_EFCT_SYNC_CTRL_s *EfctSyncCtrl = HL_CtxYuvStrmEfctSyncCtrlPtr;
    CTX_EFCT_SYNC_JOB_SET_s *EfctSyncJobSet = HL_CtxYuvStrmEfctSyncJobSetPtr;

    Rval = HL_LiveviewUpdateGeoCfgChk(ViewZoneId, pGeoCfgCtrl, pAttachedRawSeq);
    AmbaMisra_TouchUnused(pAttachedRawSeq);

    /* Body */
    if (Rval == OK) {
        UINT8 JobExisted = 0U, IsGeoCfgUpdate = 0U;
        UINT16 JobSeqId = 0U, JobType = EFCT_SYNC_JOB_BIT_DISPLAY, YuvStrmIdx = 0U;
        DSP_EFCT_SYNC_JOB_s *pSyncJob = NULL;

        //AmbaLL_LogUInt5("GeoCtrl[%d] for Sync", ViewZoneId, 0U, 0U, 0U, 0U);

        HL_GetResourcePtr(&Resource);

        /* Sweep YuvStrm locked */
        ExitLoop = 0U;
        for (i=0U; i<Resource->YuvStrmNum; i++) {
            HL_GetYuvStrmEfctSyncCtrlLock(i, &EfctSyncCtrl);
            if (EfctSyncCtrl->State == EFCT_SYNC_ON) {
                ExitLoop = 1U;
                YuvStrmIdx = i;
                HL_GetYuvStrmEfctSyncCtrlUnLock(i);
                break;
            } else {
                HL_GetYuvStrmEfctSyncCtrlUnLock(i);
            }
        }

        if (ExitLoop == 0U) {
            AmbaLL_LogUInt5("GeoCtrl[%d] with Null YuvStrm", ViewZoneId, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0004;
        }

        if (Rval == OK) {
            /* Assign Job and Copy necessary Data */
            HL_GetYuvStrmEfctSyncCtrlLock(YuvStrmIdx, &EfctSyncCtrl);
            HL_GetYuvStrmEfctSyncJobSetLock(YuvStrmIdx, (UINT16)EfctSyncCtrl->PoolDesc.Wp, &EfctSyncJobSet);

            (void)HL_LvUptGeoCfgCmdCpyToLocal(pGeoCfgCtrl->GeoCfgAddr, &IsGeoCfgUpdate);

            /* No warp cmd need to be updated */
            if (IsGeoCfgUpdate == 0U) {
                AmbaLL_LogUInt5("GeoCtrl[%d] with Null warp cmd", ViewZoneId, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0000;
            }

            if (Rval == OK) {
                /* Sweep Existed Job */
                JobExisted = 0U;
                (void)HL_LvUptCfgOnSyncJobSearch(EfctSyncJobSet,
                                                 ViewZoneId,
                                                 &JobExisted,
                                                 &JobSeqId,
                                                 &JobType);

                if (JobExisted == 0U) {
                    /* Create new Job with prepared YuvStrmIdx/SeqIdx */
                    pSyncJob = &EfctSyncJobSet->Job[EfctSyncJobSet->JobNum];
                    pSyncJob->JobId = EfctSyncJobSet->JobId;
                    JobType = (UINT16)1U << EFCT_SYNC_JOB_BIT_GEO;
                    Rval = HL_ComposeEfctSyncJobId(&pSyncJob->JobId,
                                                   EFCT_SYNC_JOB_ID_DONTCARE,
                                                   EFCT_SYNC_JOB_ID_DONTCARE,
                                                   JobType,
                                                   (UINT16)EfctSyncJobSet->JobNum+1U);
                    if (Rval == OK) {
                        pSyncJob->ViewZoneIdx = ViewZoneId;

                        /* Advanced JobNum */
                        (EfctSyncJobSet->JobNum)++;
#ifdef DEBUG_SYNC_CMD
                        AmbaLL_LogUInt5("@@[%d] New Geo Type[0x%X] Sub[%d]", __LINE__, pSyncJob->JobId.JobTypeBit, pSyncJob->JobId.SubJobIdx, 0, 0);
#endif
                    }
                } else if (JobSeqId < MAX_GROUP_CMD_SUBJOB_NUM) {
                    pSyncJob = &EfctSyncJobSet->Job[JobSeqId];
                    JobType |= (UINT16)1U << EFCT_SYNC_JOB_BIT_GEO;
                    pSyncJob->JobId.JobTypeBit = (UINT8)JobType;

#ifdef DEBUG_SYNC_CMD
                    AmbaLL_LogUInt5("@@[%d] Existed Geo Type[0x%X] Sub[%d]", __LINE__, pSyncJob->JobId.JobTypeBit, pSyncJob->JobId.SubJobIdx, 0, 0);
#endif
                } else {
                    AmbaLL_LogUInt5("[%d] No JobExist Neither JobDesc", __LINE__, 0, 0, 0, 0);
                }
                if (Rval == OK) {
                    if (pSyncJob != NULL) {
                        (void)HL_LvUptGeoCfgCmdCpyToDB(pSyncJob, pGeoCfgCtrl->GeoCfgAddr, IsGeoCfgUpdate);
                    }
                }
            }
            HL_GetYuvStrmEfctSyncJobSetUnLock(YuvStrmIdx, (UINT16)EfctSyncCtrl->PoolDesc.Wp);
            HL_GetYuvStrmEfctSyncCtrlUnLock(YuvStrmIdx);
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_LiveviewUpdateGeoCfg Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_LiveviewUpdateVZSourceChk(const UINT16 NumViewZone,
                                                  const AMBA_DSP_LIVEVIEW_VZ_SRC_CFG_s *pVzSrcCfg,
                                                  const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    UINT16 i = 0U, ViewZoneId;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;


    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pLiveviewUpdateVZSrcCfg(NumViewZone, pVzSrcCfg, pAttachedRawSeq);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pLiveviewUpdateVZSrcCfg(NumViewZone, pVzSrcCfg, pAttachedRawSeq);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pLiveviewUpdateVZSrcCfg(NumViewZone, pVzSrcCfg, pAttachedRawSeq);
    }

    /* Timing sanity check */
    if (Rval == OK) {
        for (i = 0U; i<NumViewZone; i++) {
            ViewZoneId = pVzSrcCfg[i].ViewZoneId;
            HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
            if (ViewZoneInfo->EffectSyncState == EFCT_SYNC_OFF) {
                Rval = DSP_ERR_0004;
                AmbaLL_LogUInt5("[%d]UpdateVZSource invalid timing", ViewZoneId, 0, 0, 0, 0);
            }
        }
    }

    /* Logic sanity check */
    return Rval;
}

/**
* Liveview ViewZone source update function
* @param [in]  NumViewZone viewzone number
* @param [in]  pVzSrcCfg viewzone source config
* @param [out] pAttachedRawSeq attached raw sequence
* @return ErrorCode
*/
UINT32 dsp_liveview_update_vz_src(UINT16 NumViewZone, const AMBA_DSP_LIVEVIEW_VZ_SRC_CFG_s *pVzSrcCfg, UINT64 *pAttachedRawSeq)
{
    UINT8 ExitLoop = 0U;
    UINT32 Rval;
    UINT16 i = 0U, ViewZoneId;

    Rval = HL_LiveviewUpdateVZSourceChk(NumViewZone, pVzSrcCfg, pAttachedRawSeq);
    AmbaMisra_TouchUnused(pAttachedRawSeq);

    /* Body */
    if (Rval == OK) {
        CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
        CTX_YUVSTRM_EFCT_SYNC_CTRL_s *EfctSyncCtrl = HL_CtxYuvStrmEfctSyncCtrlPtr;
        CTX_EFCT_SYNC_JOB_SET_s *EfctSyncJobSet = HL_CtxYuvStrmEfctSyncJobSetPtr;
        UINT8 JobExisted = 0U;
        UINT16 JobSeqId = 0U, JobType = EFCT_SYNC_JOB_BIT_DISPLAY, YuvStrmIdx = 0U;
        DSP_EFCT_SYNC_JOB_s *pSyncJob = NULL;

        HL_GetResourcePtr(&Resource);

        /* Sweep YuvStrm locked */
        for (i=0U; i<Resource->YuvStrmNum; i++) {
            HL_GetYuvStrmEfctSyncCtrlLock(i, &EfctSyncCtrl);
            if (EfctSyncCtrl->State == EFCT_SYNC_ON) {
                YuvStrmIdx = i;
                ExitLoop = 1U;
                HL_GetYuvStrmEfctSyncCtrlUnLock(i);
                break;
            } else {
                HL_GetYuvStrmEfctSyncCtrlUnLock(i);
            }
        }

        if (ExitLoop == 0U) {
            AmbaLL_LogUInt5("VZSource with Null YuvStrm", 0U, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0004;
        } else {
            for (i = 0U; i<NumViewZone; i++) {
                ViewZoneId = pVzSrcCfg[i].ViewZoneId;
                HL_GetYuvStrmEfctSyncCtrlLock(YuvStrmIdx, &EfctSyncCtrl);
                HL_GetYuvStrmEfctSyncJobSetLock(YuvStrmIdx, (UINT16)EfctSyncCtrl->PoolDesc.Wp, &EfctSyncJobSet);

                JobExisted = 0U;
                /* Sweep Existed Job */
                (void)HL_LvUptCfgOnSyncJobSearch(EfctSyncJobSet,
                                                 ViewZoneId,
                                                 &JobExisted,
                                                 &JobSeqId,
                                                 &JobType);

                if (JobExisted == 0U) {
                    /* Create new Job with prepared YuvStrmIdx/SeqIdx */
                    pSyncJob = &EfctSyncJobSet->Job[EfctSyncJobSet->JobNum];
                    pSyncJob->JobId = EfctSyncJobSet->JobId;
                    JobType = (UINT16)1U << EFCT_SYNC_JOB_BIT_VINROI;
                    Rval = HL_ComposeEfctSyncJobId(&pSyncJob->JobId,
                                                   EFCT_SYNC_JOB_ID_DONTCARE,
                                                   EFCT_SYNC_JOB_ID_DONTCARE,
                                                   JobType,
                                                   (UINT16)EfctSyncJobSet->JobNum+1U);
                    if (Rval == OK) {
                        pSyncJob->ViewZoneIdx = ViewZoneId;

                        /* Advanced JobNum */
                        (EfctSyncJobSet->JobNum)++;
#ifdef DEBUG_SYNC_CMD
                        AmbaLL_LogUInt5("@@[%d] New VZSrc Type[0x%X] Sub[%d]", __LINE__, pSyncJob->JobId.JobTypeBit, pSyncJob->JobId.SubJobIdx, 0, 0);
#endif
                    }
                } else if (JobSeqId < MAX_GROUP_CMD_SUBJOB_NUM) {
                    pSyncJob = &EfctSyncJobSet->Job[JobSeqId];
                    JobType |= (UINT16)1U << EFCT_SYNC_JOB_BIT_VINROI;
                    pSyncJob->JobId.JobTypeBit = (UINT8)JobType;

#ifdef DEBUG_SYNC_CMD
                    AmbaLL_LogUInt5("@@[%d] Existed VZSrc Type[0x%X] Sub[%d]", __LINE__, pSyncJob->JobId.JobTypeBit, pSyncJob->JobId.SubJobIdx, 0, 0);
#endif
                } else {
                    AmbaLL_LogUInt5("[%d] No JobExist Neither JobDesc", __LINE__, 0, 0, 0, 0);
                }

                if (Rval == OK) {
                    (void)dsp_osal_memcpy(&EfctSyncJobSet->VinRoi[pVzSrcCfg[i].ViewZoneId], &pVzSrcCfg[i].VinRoi, sizeof(AMBA_DSP_WINDOW_s));
                }
                HL_GetYuvStrmEfctSyncJobSetUnLock(YuvStrmIdx, (UINT16)EfctSyncCtrl->PoolDesc.Wp);
                HL_GetYuvStrmEfctSyncCtrlUnLock(YuvStrmIdx);
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_LiveviewUpdateVZSource Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_LiveviewSliceCfgChk(UINT16 ViewZoneId, const AMBA_DSP_LIVEVIEW_SLICE_CFG_s *pLvSliceCfg)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pLiveviewSliceCfg(ViewZoneId, pLvSliceCfg);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pLiveviewSliceCfg(ViewZoneId, pLvSliceCfg);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pLiveviewSliceCfg(ViewZoneId, pLvSliceCfg);
    }

    /* Timing sanity check */

    /* Logic sanity check */
    return Rval;
}

/**
* Liveview ViewZone slice configuration setup
* @param [in]  ViewZoneId viewzone index
* @param [in]  pLvSliceCfg slice config
* @return ErrorCode
*/
UINT32 dsp_liveview_slice_cfg(UINT16 ViewZoneId, const AMBA_DSP_LIVEVIEW_SLICE_CFG_s *pLvSliceCfg)
{
    UINT32 Rval;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    Rval = HL_LiveviewSliceCfgChk(ViewZoneId, pLvSliceCfg);

    /* Body */
    if (Rval == OK) {
        HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
        ViewZoneInfo->SliceNumCol = pLvSliceCfg->SliceNumCol;
        ViewZoneInfo->SliceNumRow = pLvSliceCfg->SliceNumRow;
        ViewZoneInfo->EncSyncOpt = pLvSliceCfg->EncSyncOpt;
        ViewZoneInfo->WarpLumaWaitLine = pLvSliceCfg->WarpLumaWaitLine;
        ViewZoneInfo->WarpChromaWaitLine = pLvSliceCfg->WarpChromaWaitLine;
        ViewZoneInfo->WarpOverLap = pLvSliceCfg->WarpOverLap;
        ViewZoneInfo->VinDragLine = pLvSliceCfg->VinDragLine;
        HL_GetViewZoneInfoUnLock(ViewZoneId);
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_LiveviewSliceCfg Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_LiveviewSliceCfgCalcChk(UINT16 ViewZoneId, const AMBA_DSP_LIVEVIEW_SLICE_CFG_s *pLvSliceCfg)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pLiveviewSliceCfg(ViewZoneId, pLvSliceCfg);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pLiveviewSliceCfg(ViewZoneId, pLvSliceCfg);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pLiveviewSliceCfg(ViewZoneId, pLvSliceCfg);
    }

    /* Timing sanity check */

    /* Logic sanity check */
    return Rval;
}

static inline void HL_LvSliceCfgCalc(const UINT16 ViewZoneId,
                                     const CTX_VIEWZONE_INFO_s *pViewZoneInfo,
                                     UINT32 *pIntSliceNumCol)
{
    UINT8 IsTileMode = 0U;
    UINT16 C2YTileX = 1U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);

    /* use tile mode if anyone need it */
    (void)HL_IsTileMode(&IsTileMode);

    /* currently only care TileX only */
    if (IsTileMode > 0U) {
        UINT8 FixedOverlap;
        UINT16 C2YInTileX = 1U;
        UINT16 TileOverlap;

        //get init tile overlap
        (void)HL_GetViewZoneWarpOverlapX(ViewZoneId, &TileOverlap);
        FixedOverlap = (Resource->MaxHorWarpComp[ViewZoneId] > 0U) ? 1U : 0U;

        //calculate init tile number, than check availability to decode correct overlap
        (void)HL_CalcVideoTileC2Y(0xFF/*ignore ViewId*/,
                                  pViewZoneInfo->CapWindow.Width,
                                  SEC2_MAX_IN_WIDTH,
                                  WARP_GRID_EXP,
                                  FixedOverlap,
                                  0U, /* ChkSmem */
                                  &C2YInTileX,
                                  &TileOverlap);
        (void)HL_CalcVideoTileC2Y(0xFF/*ignore ViewId*/,
                                  pViewZoneInfo->Main.Width,
                                  SEC2_MAX_OUT_WIDTH,
                                  WARP_GRID_EXP,
                                  FixedOverlap,
                                  1U, /* ChkSmem */
                                  &C2YTileX,
                                  &TileOverlap);
#ifdef DEBUG_TILE_CALC
        AmbaLL_LogUInt5("HL_LvSliceCfgCalc ViewZoneId:%u CapWindow.Width:%u Main.Width:%u TileOverlap:%u",
                ViewZoneId, pViewZoneInfo->CapWindow.Width, pViewZoneInfo->Main.Width, TileOverlap, 0U);
        AmbaLL_LogUInt5("VprocSetup ViewZoneId:%u C2YInTileX:%u C2YTileX:%u",
                ViewZoneId, C2YInTileX, C2YTileX, 0U, 0U);
#endif
        C2YTileX = MAX2_16(C2YInTileX, C2YTileX);
    }

    *pIntSliceNumCol = (UINT32)C2YTileX;
}

/**
* Liveview ViewZone calculate and get slice configuration
* @param [in]  ViewZoneId viewzone index
* @param [out]  pLvSliceCfg slice config
* @return ErrorCode
*/
UINT32 dsp_liveview_slice_cfg_calc(UINT16 ViewZoneId, AMBA_DSP_LIVEVIEW_SLICE_CFG_s *pLvSliceCfg)
{
    UINT32 Rval;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    Rval = HL_LiveviewSliceCfgCalcChk(ViewZoneId, pLvSliceCfg);

    /* Body */
    if (Rval == OK) {
        UINT32 IntSliceNumCol = 1U;

        (void)dsp_osal_memset(pLvSliceCfg, 0, sizeof(AMBA_DSP_LIVEVIEW_SLICE_CFG_s));
        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);

        /* Slice */
        HL_LvSliceCfgCalc(ViewZoneId, ViewZoneInfo, &IntSliceNumCol);

        pLvSliceCfg->SliceNumCol = (ViewZoneInfo->SliceNumCol > 0U) ? ViewZoneInfo->SliceNumCol : (UINT8)IntSliceNumCol;
        pLvSliceCfg->SliceNumRow = (ViewZoneInfo->SliceNumRow > 0U) ? ViewZoneInfo->SliceNumRow : 1U;

        /* WarpWaitLine */
        pLvSliceCfg->WarpLumaWaitLine = ViewZoneInfo->WarpLumaWaitLine;
        pLvSliceCfg->WarpChromaWaitLine = ViewZoneInfo->WarpChromaWaitLine;

        /* WarpOverLap */
        {
            UINT16 TileOverLapX = 0U;
            UINT16 TileOverLapY = 0U;

            (void)HL_GetViewZoneWarpOverlap(ViewZoneId, &TileOverLapX, &TileOverLapY);
            pLvSliceCfg->WarpOverLap = (ViewZoneInfo->WarpOverLap > 0U) ? ViewZoneInfo->WarpOverLap : TileOverLapX;
        }

        /* VinDragLine */
        pLvSliceCfg->VinDragLine = (ViewZoneInfo->VinDragLine > 0U)? ViewZoneInfo->VinDragLine: CV2X_EXTRA_SLICE_CAP_LINE;

    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_LiveviewSliceCfgCalc Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_LiveviewVZPostponeCfgChk(UINT16 ViewZoneId, const AMBA_DSP_LV_VZ_POSTPONE_CFG_s *pVzPostPoneCfg)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pLiveviewVZPostponeCfg(ViewZoneId, pVzPostPoneCfg);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pLiveviewVZPostponeCfg(ViewZoneId, pVzPostPoneCfg);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pLiveviewVZPostponeCfg(ViewZoneId, pVzPostPoneCfg);
    }

    /* Timing sanity check */

    /* Logic sanity check */
    return Rval;
}

/**
* Liveview ViewZone postpone config
* @param [in]  ViewZoneId viewzone index
* @param [in]  pPostPoneCfg postpone config
* @return ErrorCode
*/
UINT32 dsp_liveview_vz_postpone_cfg(UINT16 ViewZoneId, const AMBA_DSP_LV_VZ_POSTPONE_CFG_s *pVzPostPoneCfg)
{
    UINT32 Rval;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    Rval = HL_LiveviewVZPostponeCfgChk(ViewZoneId, pVzPostPoneCfg);

    /* Body */
    if (Rval == OK) {
        HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
        ViewZoneInfo->ProcPostPoneTime[pVzPostPoneCfg->StageId] = pVzPostPoneCfg->DelayTimeIn10xMs;
        HL_GetViewZoneInfoUnLock(ViewZoneId);
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_LiveviewVZPostponeCfg Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

#if 0
static inline UINT32 HL_LvVinLostChkEffFin(UINT16 VinId)
{
    UINT16 i;
    UINT32 Rval = DSP_ERR_0000, IsEfctStrmExist;

    IsEfctStrmExist = HL_GetEffectChannelEnable();
    if (IsEfctStrmExist > 0U) {
        UINT8 RawSeqWp, YuvRawSeqWp;
        UINT64 VinRawCapSeq;
        CTX_VIN_INFO_s VinInfo = {0};
        CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

        HL_GetVinInfo(HL_MTX_OPT_ALL, VinId, &VinInfo);
        RawSeqWp = VinInfo.VinCtrl.RawSeqWp % MAX_RAW_SEQ_Q;
        VinRawCapSeq = VinInfo.VinCtrl.RawCapSeq64[RawSeqWp];
        //default raw seq
        if (VinInfo.VinCtrl.VinState == DSP_VIN_STATUS_TIMEOUT) {
            if (VinRawCapSeq == 0U) {
                Rval = OK;  //boot timeout case
            } else {
                VinRawCapSeq --;
            }
        }
        AmbaLL_LogUInt5("HL_LvVinLostChkEffFin vin:%u RawSeqWp:%u VinRawCapSeq:%u VinState:%u Rval:0x%x",
                VinId, VinInfo.VinCtrl.RawSeqWp, (UINT32)VinRawCapSeq, VinInfo.VinCtrl.VinState, Rval);
        /* search all vz which source from that vin */
        if (Rval != OK) {
            HL_GetResourcePtr(&Resource);
            for (i=0U; i<Resource->MaxViewZoneNum; i++) {
                UINT8 VprocState;
                UINT16 VzVinIdx = 0U;
                UINT64 EffRawCapSeq;
                CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

                HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
                DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &VzVinIdx);
                if ((0U == DSP_GetBit(Resource->ViewZoneActiveBit, i, 1U)) ||
                    (ViewZoneInfo->InputFromMemory != VIN_SRC_FROM_HW) ||
                    (VzVinIdx != VinId)) {
                    continue;
                }
                VprocState = DSP_GetVprocState((UINT8)i);
                YuvRawSeqWp = ViewZoneInfo->YuvInput.RawSeqWp % MAX_RAW_SEQ_Q;
                EffRawCapSeq = ViewZoneInfo->YuvInput.RawCapSeq64[YuvRawSeqWp];

                AmbaLL_LogUInt5("HL_LvVinLostChkEffFin vz:%u st:%u YuvRawSeqWp:%u EffRawCapSeq:%u",
                        i, VprocState, YuvRawSeqWp, (UINT32)EffRawCapSeq, 0U);
                if ((VprocState != DSP_VPROC_STATUS_ACTIVE) ||
                    (VinRawCapSeq > EffRawCapSeq)) {
                    //has some raw which is not finished effect job
                    Rval = DSP_ERR_0004;
                    break;
                } else {
                    Rval = OK;
                }
            }
        } else {
            //direct return
//            Rval = OK;
        }
    } else {
        //direct return
        Rval = OK;
    }

    return Rval;
}
#endif

//#define DEBUG_LV_UPDATE_VIN_STATE
static inline UINT32 HL_LvUptVinStChk(UINT16 NumVin, const AMBA_DSP_LV_VIN_STATE_s *pVinState)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pLiveviewUpdateVinState(NumVin, pVinState);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pLiveviewUpdateVinState(NumVin, pVinState);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pLiveviewUpdateVinState(NumVin, pVinState);
    }
    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pLiveviewUpdateVinState(NumVin, pVinState);
    }

    /* Timing sanity check */

    /* Logic sanity check */

    return Rval;
}

static inline UINT32 HL_LvUptVinStUptGrp(UINT16 VinId,
                                         UINT16 *pGrpNum,
                                         UINT16 *GrpId,
                                         UINT32 *DisableViewZoneBitMask,
                                         UINT16 *MasterViewZoneId,
                                         UINT16 *GrpMasterViewZoneId,
                                         UINT16 *GrpVprocNum)
{
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT16 GrpIdx, GrpNum;
    UINT16 UpdGrpNum = 0U;
    UINT16 NumVproc = 0U;
    UINT16 NewMasterViewZoneId = AMBA_DSP_MAX_VIEWZONE_NUM;
    UINT32 Rval = OK;
    UINT32 DisableVzBit = 0U, CurrentMasterVzBit;
    UINT32 IsGrpFreeRun;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    cmd_vproc_set_vproc_grping *VprocGroup = HL_DefCtxCmdBufPtrVpcGrp;

    HL_GetResourcePtr(&Resource);
    (void)HL_GetVprocGroupNum(&GrpNum);

    /*
     * different vin has no effect will use free_run_grp,
     * no need to update VprocChanProcOrder in that case
     */
    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&VprocGroup, &CmdBufferAddr);
    (void)dsp_osal_memset(VprocGroup, 0x0, sizeof(cmd_vproc_set_vproc_grping));
    (void)HL_FillVideoProcGrouping(VprocGroup);
    IsGrpFreeRun = VprocGroup->free_run_grp;
    HL_RelCmdBuffer(CmdBufferId);

    for (GrpIdx = 0; GrpIdx<GrpNum; GrpIdx++) {
        UINT16 MasterVprocVinId = VIN_IDX_INVALID;
        UINT16 MasterVprocId = AMBA_DSP_MAX_VIEWZONE_NUM;
        CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

        if (DSP_GetBit(IsGrpFreeRun, GrpIdx, 1U) != 0U) {
            continue;
        }

        /* find current master */
        HL_GetGroupMasterVprocId(GrpIdx, &MasterVprocId);

        /* get MasterVprocVinId */
        if (MasterVprocId != AMBA_DSP_MAX_VIEWZONE_NUM) {
            HL_GetViewZoneInfoPtr(MasterVprocId, &ViewZoneInfo);
            if (ViewZoneInfo->SourceVin > 0U) {
                DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &MasterVprocVinId);
            }
        }

        //find new master vin, aka exclude itself, smallest alive vin
        CurrentMasterVzBit = 0U;
        DSP_SetBit(&CurrentMasterVzBit, MasterVprocId);
        HL_GetGroupAliveViewZoneId((UINT8)GrpIdx,
                                   VinId,
                                   CurrentMasterVzBit,
                                   &NewMasterViewZoneId,
                                   &NumVproc);
#ifdef DEBUG_LV_UPDATE_VIN_STATE
        AmbaLL_LogUInt5("HL_LvUptVinStUptGrp Grp:%u Master:%u MasterVin:%u NewMasterVz:%u",
                GrpIdx, MasterVprocId, MasterVprocVinId, NewMasterViewZoneId, 0U);
#endif

        /* update current master vproc as timeout
         * set DisableViewZoneBitMask, only do when it has new master
         * disable ViewZoneInfo->IsMaster
         */
        if (MasterVprocVinId == VinId) {
            UINT16 VzIdx;

            /* search all vz which source from that vin */
            for (VzIdx=0U; VzIdx<Resource->MaxViewZoneNum; VzIdx++) {
                UINT16 VzGrpId = 0U;
                UINT16 VzVinIdx = 0U;

                if (0U == DSP_GetBit(Resource->ViewZoneActiveBit, VzIdx, 1U)) {
                    continue;
                }
                HL_GetViewZoneInfoPtr(VzIdx, &ViewZoneInfo);
                Rval = HL_GetVprocGroupIdx(VzIdx, &VzGrpId, 0/*IsStlProc*/);
                DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &VzVinIdx);

                /*
                 * exclude different group vz
                 * exclude vz which source from different vin
                 * exclude none physical vin
                 * exclude timeout re-ordered vz
                 */
                if ((Rval != OK) ||
                    (VzGrpId != GrpIdx) ||
                    (VzVinIdx != MasterVprocVinId) ||
                    (ViewZoneInfo->InputFromMemory != VIN_SRC_FROM_HW) ||
                    (ViewZoneInfo->TimeoutState == VIEWZONE_VIN_TIMEOUT)) {
                    continue;
                }

                //update timeout state
                HL_GetViewZoneInfoLock(VzIdx, &ViewZoneInfo);
                ViewZoneInfo->TimeoutState = VIEWZONE_VIN_TIMEOUT;

                //set as DisableViewZoneBitMask, only do when it has new master
                if (NewMasterViewZoneId != AMBA_DSP_MAX_VIEWZONE_NUM) {
                    DSP_SetBit(&DisableVzBit, VzIdx);
                    if (ViewZoneInfo->IsMaster == 1U) {
                        ViewZoneInfo->IsMaster = 0U;
                        GrpId[UpdGrpNum] = GrpIdx;
                        DisableViewZoneBitMask[UpdGrpNum] = DisableVzBit;
                        MasterViewZoneId[UpdGrpNum] = NewMasterViewZoneId;
                        GrpMasterViewZoneId[GrpIdx] = NewMasterViewZoneId;
                        GrpVprocNum[UpdGrpNum] = NumVproc;
                        UpdGrpNum++;
                    }
                }
                HL_GetViewZoneInfoUnLock(VzIdx);
#ifdef DEBUG_LV_UPDATE_VIN_STATE
                AmbaLL_LogUInt5("HL_LvUptVinStUptGrp DisableVzBit:%x vz:%u IsMaster:%u Tout:%u",
                        DisableVzBit, VzIdx, ViewZoneInfo->IsMaster, ViewZoneInfo->TimeoutState, 0U);
#endif
            }
        } else {
            //
#ifdef DEBUG_LV_UPDATE_VIN_STATE
            AmbaLL_LogUInt5("HL_LvUptVinStUptGrp vin:%u != MasterVprocId:%u",
                    VinId, MasterVprocId, 0U, 0U, 0U);
#endif
        }
    }
    *pGrpNum = UpdGrpNum;
    return Rval;
}

static inline void HL_LvUptVinStSetVzOrd(CTX_YUV_STRM_INFO_s *pYuvStrmInfo,
                                         const UINT16 *NewMasterViewZoneIdArr)
{
    UINT16 GrpId, NewMasterViewZoneId;
    const AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *pChanCfg = pYuvStrmInfo->Layout.ChanCfg;
    UINT16 FirstVzId = pChanCfg[0].ViewZoneId;

#ifdef DEBUG_LV_UPDATE_VIN_STATE
    AmbaLL_LogUInt5("HL_LvUptVinStSetVzOrd NewMasterViewZoneIdArr:%u/%u/%u/%u/%u",
            NewMasterViewZoneIdArr[0], NewMasterViewZoneIdArr[1], NewMasterViewZoneIdArr[2],
            NewMasterViewZoneIdArr[3], NewMasterViewZoneIdArr[4]);
#endif
    //get GrpId for YuvStrm
    (void)HL_GetVprocGroupIdx(FirstVzId, &GrpId, 0/*IsStlProc*/);
    NewMasterViewZoneId = NewMasterViewZoneIdArr[GrpId];

#ifdef DEBUG_LV_UPDATE_VIN_STATE
    AmbaLL_LogUInt5("HL_LvUptVinStSetVzOrd NewMasterViewZoneId:%u GrpId:%u FirstVzId:%u/%u/%u",
            NewMasterViewZoneId, GrpId, FirstVzId, 0, 0);
#endif
    if (NewMasterViewZoneId != AMBA_DSP_MAX_VIEWZONE_NUM) {
        UINT8 *pGroupNum = NULL;
        UINT16 i, GroupNum, NewMasterChanIdx=0;
        UINT16 ChanIdxArray[AMBA_DSP_MAX_YUVSTRM_VIEW_NUM] = {0};

        //reset ChanIdxArray

        /* create channel idx as original */
        /* find new master chan idx */
        /* GroupNum is in the first in HL_GetPointerToDspVprocGrpNum */
        HL_GetPointerToDspVprocGrpNum(GrpId, &pGroupNum);
        if (pGroupNum != NULL) {
            GroupNum = *pGroupNum;
        } else {
            GroupNum = 1U;
        }
        for (i=0U; i<GroupNum; i++) {
            UINT8 *pViewzoneId = NULL;

            HL_GetPointerToDspVprocGrpOrd(GrpId, i, &pViewzoneId);
            ChanIdxArray[i] = *pViewzoneId;
            if (*pViewzoneId == NewMasterViewZoneId) {
                NewMasterChanIdx = i;
            }
        }
#ifdef DEBUG_LV_UPDATE_VIN_STATE
        AmbaLL_LogUInt5("HL_LvUptVinStSetVzOrd ChanIdxArray:%u/%u/%u/%u NewMasterChanIdx:%u",
                ChanIdxArray[0], ChanIdxArray[1], ChanIdxArray[2], ChanIdxArray[3], NewMasterChanIdx);
#endif
        /* swap new master chan to idx0 */
        for (i=NewMasterChanIdx; i>0U; i--) {
            UINT16 TmpIdx = ChanIdxArray[i];

            ChanIdxArray[i] = ChanIdxArray[i-1U];
            ChanIdxArray[i-1U] = TmpIdx;
        }

        for (i=0U; i<GroupNum; i++) {
            pYuvStrmInfo->ChanIdxArray[i] = ChanIdxArray[i];
        }
    }

#ifdef DEBUG_LV_UPDATE_VIN_STATE
    AmbaLL_LogUInt5("HL_LvUptVinStSetVzOrd YuvStrmInfo->ChanIdxArray:%u/%u/%u/%u/%u",
            pYuvStrmInfo->ChanIdxArray[0], pYuvStrmInfo->ChanIdxArray[1], pYuvStrmInfo->ChanIdxArray[2],
            pYuvStrmInfo->ChanIdxArray[3], pYuvStrmInfo->ChanIdxArray[4]);
#endif
}

static inline UINT32 HL_LvUptVinStSetPpStrm(const UINT16 YuvStrmNum,
                                            const UINT16 *NewMasterViewZoneId,
                                            cmd_vproc_multi_stream_pp_t *pVprocStrmPpArr)
{
    UINT8 IsEfctYuvStrm;
    UINT16 i;
    UINT32 Rval = OK, IsEfctStrmExist;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;

    IsEfctStrmExist = HL_GetEffectChannelEnable();
    if (IsEfctStrmExist > 0U) {
        for (i=0U; i<YuvStrmNum; i++) {
            cmd_vproc_multi_stream_pp_t *pVprocStrmPp = &pVprocStrmPpArr[i];

            HL_GetYuvStrmInfoPtr(i, &YuvStrmInfo);
            IsEfctYuvStrm = (1U == HL_GetEffectEnableOnYuvStrm(i))? (UINT8)1U: (UINT8)0U;

#ifdef DEBUG_LV_UPDATE_VIN_STATE
            AmbaLL_LogUInt5("HL_LvUptVinStSetPpStrm i:%u MaxChanNum:%u IsEfctYuvStrm:%u pVprocStrmPp:0x%x",
                    i, YuvStrmInfo->MaxChanNum, IsEfctYuvStrm, (UINT32)pVprocStrmPp, 0U);
            AmbaLL_LogUInt5("HL_LvUptVinStSetPpStrm str_id:%u/%u/%u/%u",
                    pVprocStrmPpArr[0].str_id,
                    pVprocStrmPpArr[1].str_id,
                    pVprocStrmPpArr[2].str_id,
                    pVprocStrmPpArr[3].str_id,
                    0);
#endif
            if (1U == IsEfctYuvStrm) {
                HL_LvUptVinStSetVzOrd(YuvStrmInfo, NewMasterViewZoneId);
                Rval = HL_FillPpStrmCfgOrder(i, YuvStrmInfo->ChanIdxArray, pVprocStrmPp, NULL);
#ifdef DEBUG_LV_UPDATE_VIN_STATE
                AmbaLL_LogUInt5("HL_LvUptVinStSetPpStrm i:%u Rval:%x HL_FillPpStrmCfgOrder str_id:%u",
                        i, Rval, pVprocStrmPp->str_id, 0U, 0U);
                AmbaLL_LogUInt5("HL_LvUptVinStSetPpStrm ChanIdxArray:%u/%u/%u/%u",
                        YuvStrmInfo->ChanIdxArray[0],
                        YuvStrmInfo->ChanIdxArray[1],
                        YuvStrmInfo->ChanIdxArray[2],
                        YuvStrmInfo->ChanIdxArray[3],
                        YuvStrmInfo->ChanIdxArray[4]);
                AmbaLL_LogUInt5("HL_LvUptVinStSetPpStrm str_id:%u/%u/%u/%u",
                        pVprocStrmPpArr[0].str_id,
                        pVprocStrmPpArr[1].str_id,
                        pVprocStrmPpArr[2].str_id,
                        pVprocStrmPpArr[3].str_id,
                        0);
#endif
                if (Rval != OK) {
                    AmbaLL_LogUInt5("HL_LvUptVinStSetPpStrm HL_FillPpStrmCfgOrder i:%u Rval:%x",
                            i, Rval, 0U, 0U, 0U);
                    break;
                }
            }
        }
    }
    return Rval;
}

static UINT32 HL_LvUptVinStGrpMasterOrder(const UINT16 GrpNum,
                                          const UINT16 *GrpId,
                                          const UINT16 *GrpVprocNum,
                                          const UINT16 *NewMasterViewZoneId,
                                          const UINT32 *DisableViewZoneBitMask,
                                          cmd_vproc_multi_chan_proc_order_t *ChanProcOrder)
{
    UINT16 GrpIdx;
    UINT32 Rval = OK;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    for (GrpIdx=0U; GrpIdx<GrpNum; GrpIdx++) {
#ifdef DEBUG_LV_UPDATE_VIN_STATE
        AmbaLL_LogUInt5("HL_LvUptVinStGrpMasterOrder GrpNum:%u GrpIdx:%u GrpId:%u GrpVprocNum:%u",
                GrpNum, GrpIdx, GrpId[GrpIdx], GrpVprocNum[GrpIdx], 0U);
        AmbaLL_LogUInt5("HL_LvUptVinStGrpMasterOrder GrpNum:%u GrpIdx:%u NewMasterVz:%u DisVzBit:0x%x",
                GrpNum, GrpIdx, NewMasterViewZoneId[GrpIdx], DisableViewZoneBitMask[GrpIdx], 0U);
#endif
        if ((DisableViewZoneBitMask[GrpIdx] != 0U) &&
            (GrpVprocNum[GrpIdx] > 0U) &&
            (NewMasterViewZoneId[GrpIdx] != AMBA_DSP_MAX_VIEWZONE_NUM)) {

            /* set new master */
            HL_GetViewZoneInfoLock(NewMasterViewZoneId[GrpIdx], &ViewZoneInfo);
            ViewZoneInfo->IsMaster = 1U;
            HL_GetViewZoneInfoUnLock(NewMasterViewZoneId[GrpIdx]);

            /* fill ChanProcOrder */
            HL_UpdateVprocChanProcOrderTout((UINT8)GrpId[GrpIdx],
                                            NewMasterViewZoneId[GrpIdx],
                                            GrpVprocNum[GrpIdx],
                                            &(ChanProcOrder[GrpIdx]));
//            if (Rval != OK) {
//                AmbaLL_LogUInt5("HL_UpdateVprocChanProcOrderTout:0x%x Idx:%u Grp:%u GrpVprocNum:%u NewMasterVz:%u",
//                        Rval, GrpIdx, GrpId[GrpIdx], GrpVprocNum[GrpIdx], NewMasterViewZoneId[GrpIdx]);
//            }
        }
    }

    return Rval;
}

static UINT32 HL_LvUptVinStGrpMaster(const UINT16 GrpNum,
                                     const UINT16 *GrpId,
                                     const UINT16 *GrpVprocNum,
                                     const UINT16 *NewMasterViewZoneId,
                                     const UINT16 *GrpNewMasterViewZoneId,
                                     const UINT32 *DisableViewZoneBitMask)
{
    UINT8 WriteMode;
    UINT16 YuvStrmIdx;
    UINT16 GrpIdx;
    UINT16 YuvStrmNum;
    UINT16 NewMasterVin = 0U;
    UINT16 WaitEventVinIdx[AMBA_DSP_MAX_VIN_NUM] = {0};
    UINT32 Rval;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    cmd_vproc_multi_stream_pp_t VprocStrmPpArr[AMBA_DSP_MAX_YUVSTRM_NUM] = {0};
    cmd_vproc_multi_chan_proc_order_t ChanProcOrder[AMBA_DSP_MAX_YUVSTRM_NUM] = {0};

    //prepare ChanProcOrder for every group
    Rval = HL_LvUptVinStGrpMasterOrder(GrpNum,
                                       GrpId,
                                       GrpVprocNum,
                                       NewMasterViewZoneId,
                                       DisableViewZoneBitMask,
                                       ChanProcOrder);
    if (Rval != OK) {
        AmbaLL_LogUInt5("HL_LvUptVinStGrpMasterOrder:0x%x", Rval, 0U, 0U, 0U, 0U);
    }

    //prepare pp
    HL_ResetYuvStreamInfoPP();
    HL_GetResourceLock(&Resource);
    (void)dsp_osal_memset(Resource->EffectLogicBufMask, 0, sizeof(UINT32)*EFFECT_BUF_MASK_DEPTH);
    YuvStrmNum = Resource->YuvStrmNum;
    HL_GetResourceUnLock();
#ifdef DEBUG_LV_UPDATE_VIN_STATE
            AmbaLL_LogUInt5("HL_LvUptVinStGrpMaster0 str_id:%u/%u/%u/%u",
                    VprocStrmPpArr[0].str_id,
                    VprocStrmPpArr[1].str_id,
                    VprocStrmPpArr[2].str_id,
                    VprocStrmPpArr[3].str_id,
                    0);
#endif
    Rval = HL_LvUptVinStSetPpStrm(YuvStrmNum,
                                  GrpNewMasterViewZoneId,
                                  VprocStrmPpArr);
#ifdef DEBUG_LV_UPDATE_VIN_STATE
            AmbaLL_LogUInt5("HL_LvUptVinStGrpMaster1 str_id:%u/%u/%u/%u",
                    VprocStrmPpArr[0].str_id,
                    VprocStrmPpArr[1].str_id,
                    VprocStrmPpArr[2].str_id,
                    VprocStrmPpArr[3].str_id,
                    0);
#endif
    if (Rval != OK) {
        AmbaLL_LogUInt5("HL_LvUptVinStSetPpStrm:0x%x", 0U, 0U, 0U, 0U, 0U);
    }

    //send all ChanProcOrder for every group
    (void)HL_GetViewZoneVinId(NewMasterViewZoneId[0U], &NewMasterVin);
    WriteMode = HL_GetVin2CmdNormalWrite(NewMasterVin);
    //wait one sync to make sure 0x2000064/0x2000066 in same sync
    WaitEventVinIdx[0U] = NewMasterVin;
#ifdef DEBUG_LV_UPDATE_VIN_STATE
        AmbaLL_LogUInt5("HL_LvUptVinStGrpMaster NewMasterViewZoneId[0U]:%u NewMasterVin:%u",
                NewMasterViewZoneId[0U], NewMasterVin, 0U, 0U, 0U);
#endif
    Rval = DSP_WaitVcapEvent(1U, WaitEventVinIdx, 1U, WAIT_FLAG_TIMEOUT);
    if (Rval != OK) {
        AmbaLL_LogUInt5("DSP_WaitVcapEvent %u fail", WaitEventVinIdx[0U], 0U, 0U, 0U, 0U);
    } else {
        /*
         * write to new vin's vcap,
         * Normally ChanProcOrder need to do after vproc effect, should use batch cmd,
         * since original master is timeout, no effect is running, so it is ok to send to new master's vcap
         *
         * Normally VprocStrmPp should take effect at same frame with ChanProcOrder, should use batch to control timing,
         * since original master is timeout, it is ok to send together with ChanProcOrder
         */
        for (GrpIdx=0U; GrpIdx<GrpNum; GrpIdx++) {
            Rval = AmbaHL_CmdVprocMultiChanProcOrder(WriteMode, &ChanProcOrder[GrpIdx]);
            if (Rval != OK) {
                AmbaLL_LogUInt5("AmbaHL_CmdVprocMultiChanProcOrder fail", 0U, 0U, 0U, 0U, 0U);
            }
        }

        //send pp
        for (YuvStrmIdx=0U; YuvStrmIdx<YuvStrmNum; YuvStrmIdx++) {
            cmd_vproc_multi_stream_pp_t *pVprocStrmPp = &VprocStrmPpArr[YuvStrmIdx];

            if (HL_GetEffectEnableOnYuvStrm(YuvStrmIdx) == 1U) {
#ifdef DEBUG_LV_UPDATE_VIN_STATE
                AmbaLL_LogUInt5("HL_LvUptVinStGrpMaster AmbaHL_CmdVprocMultiStrmPpCmd YuvStrmIdx:%u str_id:%u",
                        YuvStrmIdx, pVprocStrmPp->str_id, 0U, 0U, 0U);
#endif
                Rval = AmbaHL_CmdVprocMultiStrmPpCmd(WriteMode, pVprocStrmPp);

                if (Rval != OK) {
                    AmbaLL_LogUInt5("AmbaHL_CmdVprocMultiStrmPpCmd fail", 0U, 0U, 0U, 0U, 0U);
                    break;
                }
            }
        }
    }


    return Rval;
}

/**
* Liveview Vin Lost config
* @param [in]  NumVin vin number
* @param [in]  pVinState vin state information
* @return ErrorCode
*/
UINT32 dsp_liveview_update_vin_state(UINT16 NumVin, const AMBA_DSP_LV_VIN_STATE_s *pVinState)
{
    UINT32 Rval;

    Rval = HL_LvUptVinStChk(NumVin, pVinState);

    /* Body */
    if (Rval == OK) {
        /* only support update one vin at a time */
        UINT16 GrpIdx;
        UINT16 VinId = pVinState[0U].VinId;
        UINT16 GrpNum = 0U;
        UINT16 GrpId[AMBA_DSP_MAX_VIEWZONE_NUM];
        UINT16 GrpVprocNum[AMBA_DSP_MAX_VIEWZONE_NUM];
        UINT16 NewMasterViewZoneId[AMBA_DSP_MAX_VIEWZONE_NUM];
        UINT16 GrpNewMasterViewZoneId[AMBA_DSP_MAX_VIEWZONE_NUM];
        UINT32 DisableViewZoneBitMask[AMBA_DSP_MAX_VIEWZONE_NUM];
        CTX_VIN_INFO_s VinInfo = {0};

        // update vin state as timeout,
        HL_GetVinInfo(HL_MTX_OPT_GET, VinId, &VinInfo);
#ifdef DEBUG_LV_UPDATE_VIN_STATE
        if (VinInfo.VinCtrl.VinState != DSP_VIN_STATUS_TIMEOUT) {
            AmbaLL_LogUInt5("AmbaDSP_LiveviewUpdateVinState vin:%u st:%u force timeout",
                    VinId, VinInfo.VinCtrl.VinState, 0U, 0U, 0U);
        }
#endif
        VinInfo.VinCtrl.VinState = DSP_VIN_STATUS_TIMEOUT;
        HL_SetVinInfo(HL_MTX_OPT_SET, VinId, &VinInfo);

        /* check vz of all group */
        for (GrpIdx=0U; GrpIdx<NUM_VPROC_MAX_GROUP; GrpIdx++) {
            GrpId[GrpIdx] = NUM_VPROC_MAX_GROUP;
            GrpVprocNum[GrpIdx] = 0U;
            NewMasterViewZoneId[GrpIdx] = AMBA_DSP_MAX_VIEWZONE_NUM;
            GrpNewMasterViewZoneId[GrpIdx] = AMBA_DSP_MAX_VIEWZONE_NUM;
            DisableViewZoneBitMask[GrpIdx] = 0U;
        }
        Rval = HL_LvUptVinStUptGrp(VinId,
                                   &GrpNum,
                                   GrpId,
                                   DisableViewZoneBitMask,
                                   NewMasterViewZoneId,
                                   GrpNewMasterViewZoneId,
                                   GrpVprocNum);
        if (Rval != OK) {
            AmbaLL_LogUInt5("AmbaDSP_LiveviewUpdateVinState HL_LvUptVinStUptGrp:%x",
                    Rval, 0U, 0U, 0U, 0U);
        } else {
            if (GrpNum > 0U) {
                Rval = HL_LvUptVinStGrpMaster(GrpNum,
                                              GrpId,
                                              GrpVprocNum,
                                              NewMasterViewZoneId,
                                              GrpNewMasterViewZoneId,
                                              DisableViewZoneBitMask);
                if (Rval != OK) {
                    AmbaLL_LogUInt5("AmbaDSP_LiveviewUpdateVinState HL_LvUptVinStGrpMaster:%x GrpNum:%u",
                            Rval, GrpNum, 0U, 0U, 0U);
                }
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_LiveviewUpdateVinState Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

/**
* Liveview Slow-Shutter Control
* @param [in]  NumViewZone the number of viewzones
* @param [in]  pViewZoneId a pointer to an array containing viewzoneID
* @param [in]  pRatio a pointer to an array containing the repeated ratio each viewzone should follow
* @return ErrorCode
*/
UINT32 dsp_liveview_slow_shutter_ctrl(UINT16 NumViewZone, const UINT16* pViewZoneId, const UINT32* pRatio)
{
    (void)NumViewZone;
    (void)pViewZoneId;
    (void)pRatio;

    return DSP_ERR_0008;
}

#if 0
UINT32 AmbaDSP_LiveviewVinLostBch(UINT16 NumVin, const AMBA_DSP_LV_VIN_LOST_CFG_s *pVinLostCfg)
{
    UINT32 Rval;
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Input sanity check */
    if (DumpApi != NULL) {
        DumpApi->pLiveviewVinLost(NumVin, pVinLostCfg);
    }
    if (CheckApi != NULL) {
        Rval = CheckApi->pLiveviewVinLost(NumVin, pVinLostCfg);
    }

    /* Timing sanity check */

    /* Logic sanity check */

    /* Body */
    if (Rval == OK) {
        /* only support update one vin at a time */
        UINT8 WriteMode;
        UINT16 GrpId = 0U;
        UINT16 NumVproc = 0U;
        UINT16 ViewZoneId = 0U;
        UINT16 NewMasterVin = 0U;
        UINT16 VinId = pVinLostCfg[0U].VinId;
        UINT16 WaitEventVinIdx[AMBA_DSP_MAX_VIN_NUM] = {0};
        UINT32 DisableViewZoneBitMask = 0U;
        CTX_VIN_INFO_s VinInfo = {0};
        vin_fov_batch_cmd_set_t *pBatchCmdSet;

        /* update vin state as timeout */
        HL_GetVinInfo(HL_MTX_OPT_GET, VinId, &VinInfo);
        if (VinInfo.VinCtrl.VinState != DSP_VIN_STATUS_TIMEOUT) {
            AmbaLL_LogUInt5("AmbaDSP_LiveviewVinLost vin:%u st:%u force timeout",
                    VinId, VinInfo.VinCtrl.VinState, 0U, 0U, 0U);
        }
        VinInfo.VinCtrl.VinState = DSP_VIN_STATUS_TIMEOUT;
        HL_SetVinInfo(HL_MTX_OPT_SET, VinId, &VinInfo);

        /* update all group */
        Rval = HL_LvVinLostUptGrp(VinId, &GrpId, &DisableViewZoneBitMask);
        if (Rval != OK) {
            AmbaLL_LogUInt5("AmbaDSP_LiveviewVinLost HL_LvVinLostUptGrp:%x",
                    Rval, 0U, 0U, 0U, 0U);
        }
        //find new master vin
        HL_GetAliveViewZoneId((UINT8)GrpId, DisableViewZoneBitMask, &NewMasterVin, &NumVproc);
//        AmbaLL_LogUInt5("AmbaDSP_LiveviewVinLost %u GrpId:%u DisableViewZoneBitMask:%x NewMasterVin:%u NumVproc:%u",
//                __LINE__, GrpId, DisableViewZoneBitMask, NewMasterVin, NumVproc);

        if ((DisableViewZoneBitMask != 0U) && (NumVproc > 0U)) {
            UINT8 IsNewCmd = 0U;
            UINT32 U32Addr = 0U, U32VirtAddr, NewWp = 0U, BatchCmdId = 0U;
            ULONG ULAddr = 0U;
            UINT32 *pBatchQAddr = NULL, *pBatchQAddrBase = NULL;
            CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
            CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
            cmd_vproc_multi_stream_pp_t VprocStrmPp = {0};
            cmd_vproc_multi_chan_proc_order_t ChanProcOrder = {0};
            cmd_vin_attach_proc_cfg_to_cap_frm_t VinAttachProcCfg = {0};

            /* fill ChanProcOrder */
            HL_UpdateVprocChanProcOrderTout((UINT8)GrpId,
                                            NewMasterVin,
                                            NumVproc,
                                            &ChanProcOrder);
            ChanProcOrder.cmd_code = 0x2000066U; //CMD_VPROC_MULTI_CHAN_PROC_ORDER

            /* Prepare new PP cmd */
            HL_ResetYuvStreamInfoPP();
            HL_GetResourceLock(&Resource);
            (void)dsp_osal_memset(Resource->EffectLogicBufMask, 0, sizeof(UINT32)*EFFECT_BUF_MASK_DEPTH);
            HL_GetResourceUnLock();
            (void)HL_LvVinLostSetPpStrm(Resource, &VprocStrmPp);
            VprocStrmPp.cmd_code = 0x2000064U; //CMD_VPROC_MULTI_STREAM_PP

            //find timeout vin's ViewZoneId
            (void)HL_GetVinViewZoneId(VinId, &ViewZoneId);
            WriteMode = HL_GetVin2CmdNormalWrite(VinId);

            //wait one sync to make sure 0x2000064 and 0x2000066 could separate
            WaitEventVinIdx[0U] = VinId;
            Rval = DSP_WaitVcapEvent(1U, WaitEventVinIdx, 1U, WAIT_FLAG_TIMEOUT);
            if (Rval != OK) {
                AmbaLL_LogUInt5("DSP_WaitVcapEvent %u fail", WaitEventVinIdx[0U], 0U, 0U, 0U, 0U);
            }

            /*
             * write to timeout vin's batch to setup new ChanProcOrder
             */
            (void)AmbaLL_CmdQuery(WriteMode,
                                  CMD_VIN_ATTACH_PROC_CFG_TO_CAP_FRM,
                                  &ULAddr,
                                  (UINT32)VinId,
                                  HL_GetViewZoneLocalIndexOnVin(ViewZoneId));
            AmbaLL_LogUInt5("AmbaDSP_LiveviewVinLostB2 66 WriteMode:%u VinId:%u ViewZoneId:%u ULAddr:0x%x",
                    WriteMode, VinId, ViewZoneId, (UINT32)ULAddr, 0U);

            if (ULAddr > 0U) {
                UINT32 BatchNum;
                const cmd_vin_attach_proc_cfg_to_cap_frm_t *pVinAttachProcCfg;

                /* Cmd Exist */
                HL_CtrlBatchQBufMtx(HL_MTX_OPT_GET, ViewZoneId);

                dsp_osal_typecast(&pVinAttachProcCfg, &ULAddr);
                U32toU32Virtual(pVinAttachProcCfg->fov_batch_cmd_set_addr, &U32VirtAddr);
                DSP_TypeCastU32toUL(&pBatchCmdSet, &U32VirtAddr);
                U32toU32Virtual(pBatchCmdSet->batch_cmd_set_info.addr, &U32VirtAddr);
                DSP_TypeCastU32toUL(&pBatchQAddrBase, &U32VirtAddr);
                HL_GetDspBatchCmdId(U32VirtAddr, ViewZoneId, &BatchCmdId);
                DSP_TypeCastU32toUL(&ULAddr, &U32VirtAddr);
                BatchNum = HL_GetBatchCmdNumber(ULAddr);
                AmbaLL_LogUInt5("AmbaDSP_LiveviewVinLostB 66 BatchNum:%u", BatchNum, 0U, 0U, 0U, 0U);
                if (BatchNum == 0U) {
                    BatchNum = 1U;
                }
                U32VirtAddr += BatchNum*CMD_SIZE_IN_BYTE;
                DSP_TypeCastU32toUL(&pBatchQAddr, &U32VirtAddr);
            } else {
                /* NewCmd in CurrentCmdQ, Forward BatchCmdQ WP */
                IsNewCmd = 1U;
                (void)dsp_osal_memset(&VinAttachProcCfg, 0, sizeof(cmd_vin_attach_proc_cfg_to_cap_frm_t));
                HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);

                /* Request BatchCmdQ buffer */
                Rval = DSP_ReqBuf(&ViewZoneInfo->BatchQPoolDesc, 1U, &NewWp, 1U/*FullnessCheck*/);
                if (Rval != OK) {
                    HL_GetViewZoneInfoUnLock(ViewZoneId);
                } else {
                    ViewZoneInfo->BatchQPoolDesc.Wp = NewWp;
                    HL_GetViewZoneInfoUnLock(ViewZoneId);
                    HL_GetPointerToDspBatchQ(ViewZoneId,
                                             (UINT16)ViewZoneInfo->BatchQPoolDesc.Wp,
                                             &pBatchQAddrBase,
                                             &BatchCmdId);

                    /* Reset New BatchQ after Wp advanced */
                    HL_ResetDspBatchQ(pBatchQAddrBase);
                }
                HL_GetPointerToDspBatchCmdSet(ViewZoneId, 0U/*Idx*/, &pBatchCmdSet);
                dsp_osal_typecast(&U32VirtAddr, &pBatchQAddrBase);
                U32VirtAddr += CMD_SIZE_IN_BYTE;
                DSP_TypeCastU32toUL(&pBatchQAddr, &U32VirtAddr);
            }
            AmbaLL_LogUInt5("AmbaDSP_LiveviewVinLostB2 66 IsNewCmd:%u pBatchCmdSet:0x%x ViewZoneId:%u pBatchQAddrBase:0x%x pBatchQAddr:0x%x",
                    (UINT32)IsNewCmd, (UINT32)pBatchCmdSet, (UINT32)ViewZoneId,
                    (UINT32)pBatchQAddrBase, (UINT32)pBatchQAddr);

            if (Rval == OK) {
                (void)dsp_osal_memcpy(pBatchQAddr, &ChanProcOrder, sizeof(cmd_vproc_multi_chan_proc_order_t));
                ChanProcOrder.cmd_code = 0x2000066U; //CMD_VPROC_MULTI_CHAN_PROC_ORDER
                /* PrintOut cmd for debug */
                (void)AmbaHL_CmdVprocMultiChanProcOrder(AMBA_DSP_CMD_NOP, &ChanProcOrder);

                pBatchCmdSet->vin_id = VinId;
                pBatchCmdSet->chan_id = (UINT16)HL_GetViewZoneLocalIndexOnVin(ViewZoneId);
                pBatchCmdSet->batch_cmd_set_info.id = BatchCmdId;
                dsp_osal_typecast(&ULAddr, &pBatchQAddrBase);
                (void)dsp_osal_virt2cli(ULAddr, &pBatchCmdSet->batch_cmd_set_info.addr);
                pBatchCmdSet->batch_cmd_set_info.size = HL_GetBatchCmdNumber(ULAddr)*CMD_SIZE_IN_BYTE;
                AmbaLL_LogUInt5("AmbaDSP_LiveviewVinLostB2 vin:%u batch_cmd_set_info.addr:0x%x size:%u",
                        pBatchCmdSet->vin_id,
                        pBatchCmdSet->batch_cmd_set_info.addr, pBatchCmdSet->batch_cmd_set_info.size,
                        0U, 0U);
                if (IsNewCmd == 1U) {
                    dsp_osal_typecast(&ULAddr, &pBatchCmdSet);
                    (void)dsp_osal_virt2cli(ULAddr, &VinAttachProcCfg.fov_batch_cmd_set_addr);
                    VinAttachProcCfg.vin_id = (UINT8)VinId;
                    VinAttachProcCfg.fov_num = 1U;
                    (void)AmbaHL_CmdVinAttachCfgToCapFrm(WriteMode, &VinAttachProcCfg);
                } else {
                    HL_CtrlBatchQBufMtx(HL_MTX_OPT_SET, ViewZoneId);
                }
            } else {
                if (IsNewCmd == 0U) {
                    HL_CtrlBatchQBufMtx(HL_MTX_OPT_SET, ViewZoneId);
                }
            }

            ///////////////////////////////////////////////////////////////////////////////////
            //find NewMasterVin ViewZoneId
            (void)HL_GetVinViewZoneId(NewMasterVin, &ViewZoneId);
            WriteMode = HL_GetVin2CmdNormalWrite(NewMasterVin);

            //wait one sync to make sure 0x2000066 has been sent
            WaitEventVinIdx[0U] = VinId;
            Rval = DSP_WaitVcapEvent(1U, WaitEventVinIdx, 1U, WAIT_FLAG_TIMEOUT);

            /*
             * write to NewMasterVin batch
             */
            (void)AmbaLL_CmdQuery(WriteMode,
                                  CMD_VIN_ATTACH_PROC_CFG_TO_CAP_FRM,
                                  &CmdBufAddr,
                                  (UINT32)NewMasterVin,
                                  HL_GetViewZoneLocalIndexOnVin(ViewZoneId));

            AmbaLL_LogUInt5("AmbaDSP_LiveviewVinLostB2 PP WriteMode:%u NewMasterVin:%u ViewZoneId:%u CmdBufAddr:0x%x",
                    WriteMode, NewMasterVin, ViewZoneId, CmdBufAddr, 0U);

            if (CmdBufAddr > 0U) {
                UINT32 BatchNum;
                const cmd_vin_attach_proc_cfg_to_cap_frm_t *pVinAttachProcCfg;

                /* Cmd Exist */
                HL_CtrlBatchQBufMtx(HL_MTX_OPT_GET, ViewZoneId);

                dsp_osal_typecast(&pVinAttachProcCfg, &CmdBufAddr);
                U32toU32Virtual(pVinAttachProcCfg->fov_batch_cmd_set_addr, &U32VirtAddr);
                DSP_TypeCastU32toUL(&pBatchCmdSet, &U32VirtAddr);
                U32toU32Virtual(pBatchCmdSet->batch_cmd_set_info.addr, &U32VirtAddr);
                DSP_TypeCastU32toUL(&pBatchQAddrBase, &U32VirtAddr);
                HL_GetDspBatchCmdId(U32VirtAddr, ViewZoneId, &BatchCmdId);
                BatchNum = HL_GetBatchCmdNumber(U32VirtAddr);
                AmbaLL_LogUInt5("AmbaDSP_LiveviewVinLostB2 PP BatchNum:%u", BatchNum, 0U, 0U, 0U, 0U);
                if (BatchNum == 0U) {
                    BatchNum = 1U;
                }
                U32VirtAddr += BatchNum*CMD_SIZE_IN_BYTE;
                DSP_TypeCastU32toUL(&pBatchQAddr, &U32VirtAddr);
            } else {
                /* NewCmd in CurrentCmdQ, Forward BatchCmdQ WP */
                IsNewCmd = 1U;
                (void)dsp_osal_memset(&VinAttachProcCfg, 0, sizeof(cmd_vin_attach_proc_cfg_to_cap_frm_t));
                HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);

                /* Request BatchCmdQ buffer */
                Rval = DSP_ReqBuf(&ViewZoneInfo->BatchQPoolDesc, 1U, &NewWp, 1U/*FullnessCheck*/);
                if (Rval != OK) {
                    HL_GetViewZoneInfoUnLock(ViewZoneId);
                } else {
                    ViewZoneInfo->BatchQPoolDesc.Wp = NewWp;
                    HL_GetViewZoneInfoUnLock(ViewZoneId);
                    HL_GetPointerToDspBatchQ(ViewZoneId,
                                             (UINT16)ViewZoneInfo->BatchQPoolDesc.Wp,
                                             &pBatchQAddrBase,
                                             &BatchCmdId);

                    /* Reset New BatchQ after Wp advanced */
                    HL_ResetDspBatchQ(pBatchQAddrBase);
                }
                HL_GetPointerToDspBatchCmdSet(ViewZoneId, 0U/*Idx*/, &pBatchCmdSet);
                dsp_osal_typecast(&U32VirtAddr, &pBatchQAddrBase);
                U32VirtAddr += CMD_SIZE_IN_BYTE;
                DSP_TypeCastU32toUL(&pBatchQAddr, &U32VirtAddr);
            }
            AmbaLL_LogUInt5("AmbaDSP_LiveviewVinLostB2 66 IsNewCmd:%u pBatchCmdSet:0x%x ViewZoneId:%u pBatchQAddrBase:0x%x pBatchQAddr:0x%x",
                    (UINT32)IsNewCmd, (UINT32)pBatchCmdSet, (UINT32)ViewZoneId,
                    (UINT32)pBatchQAddrBase, (UINT32)pBatchQAddr);

            if (Rval == OK) {
                //write CMD_VPROC_MULTI_CHAN_PROC_ORDER into batch
                (void)dsp_osal_memcpy(pBatchQAddr, &ChanProcOrder, sizeof(cmd_vproc_multi_chan_proc_order_t));

                //write CMD_VPROC_MULTI_STREAM_PP into batch
                dsp_osal_typecast(&U32VirtAddr, &pBatchQAddr);
                U32VirtAddr += CMD_SIZE_IN_BYTE;
                DSP_TypeCastU32toUL(&pBatchQAddr, &U32VirtAddr);
                (void)dsp_osal_memcpy(pBatchQAddr, &VprocStrmPp, sizeof(cmd_vproc_multi_stream_pp_t));
                AmbaLL_LogUInt5("AmbaDSP_LiveviewVinLostB2 PP pBatchQAddr:0x%x",
                        (UINT32)pBatchQAddr, 0U, 0U, 0U, 0U);

                pBatchCmdSet->vin_id = NewMasterVin;
                pBatchCmdSet->chan_id = (UINT16)HL_GetViewZoneLocalIndexOnVin(ViewZoneId);
                pBatchCmdSet->batch_cmd_set_info.id = BatchCmdId;
                dsp_osal_typecast(&U32Addr, &pBatchQAddrBase);
                U32toU32Physical(U32Addr, &pBatchCmdSet->batch_cmd_set_info.addr);
                pBatchCmdSet->batch_cmd_set_info.size = HL_GetBatchCmdNumber(U32Addr)*CMD_SIZE_IN_BYTE;
                AmbaLL_LogUInt5("AmbaDSP_LiveviewVinLostB2 vin:%u batch_cmd_set_info.addr:0x%x size:%u",
                        pBatchCmdSet->vin_id,
                        pBatchCmdSet->batch_cmd_set_info.addr, pBatchCmdSet->batch_cmd_set_info.size,
                        0U, 0U);
                if (IsNewCmd == 1U) {
                    dsp_osal_typecast(&U32Addr, &pBatchCmdSet);
                    U32toU32Physical(U32Addr, &VinAttachProcCfg.fov_batch_cmd_set_addr);
                    VinAttachProcCfg.vin_id = (UINT8)NewMasterVin;
                    VinAttachProcCfg.fov_num = 1U;
                    (void)AmbaHL_CmdVinAttachCfgToCapFrm(WriteMode, &VinAttachProcCfg);
                } else {
                    HL_CtrlBatchQBufMtx(HL_MTX_OPT_SET, ViewZoneId);
                }
            } else {
                if (IsNewCmd == 0U) {
                    HL_CtrlBatchQBufMtx(HL_MTX_OPT_SET, ViewZoneId);
                }
            }
            /* PrintOut cmd for debug */
            (void)AmbaHL_CmdVprocMultiChanProcOrder(AMBA_DSP_CMD_NOP, &ChanProcOrder);
            (void)AmbaHL_CmdVprocMultiStrmPpCmd(AMBA_DSP_CMD_NOP, &VprocStrmPp);
        }
    }

    return Rval;
}
#endif

UINT32 dsp_liveview_vin_drv_cfg(UINT16 VinId, UINT16 Type, const void *pDrvData)
{
    UINT32 Rval;
    AMBA_DSP_VIN_MAIN_CONFIG_DATA_s *pVinMainCfg = NULL;
    const dsp_vin_drv_cfg_cfa_t *pDspVinDrvCfgCfa = NULL;
    CTX_SENSOR_INFO_s SensorInfo = {0};
    CTX_VIN_INFO_s VinInfo = {0};

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Input sanity check */
    if (VinId >= AMBA_DSP_MAX_VIN_NUM) {
        Rval = DSP_ERR_0001;
    } else if (Type >= NUM_DSP_VIN_DRV_CFG_TYPE) {
        Rval = DSP_ERR_0001;
    } else if (pDrvData == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        // DO NOTHING
    }

    /* HW/SW resources check */

    /* Timing sanity check */

    /* Logic sanity check */

    /* Body */
    if (Rval == OK) {
        if (Type == DSP_VIN_DRV_CFG_TYPE_DATA) {
            DSP_GetDspVinCfg(VinId, &pVinMainCfg);
            (void)dsp_osal_memcpy(pVinMainCfg, pDrvData, sizeof(AMBA_DSP_VIN_MAIN_CONFIG_DATA_s));
        } else {
            dsp_osal_typecast(&pDspVinDrvCfgCfa, &pDrvData);

            HL_GetSensorInfo(HL_MTX_OPT_GET, VinId, &SensorInfo);
            SensorInfo.BayerPattern = pDspVinDrvCfgCfa->BayerPattern;
            SensorInfo.BitResolution = (UINT8)pDspVinDrvCfgCfa->NumDataBits;
//FIXME, BinningMode, hard-wired as 1x binning, 0x10000U | ((pVinInfo->DspPhaseShift.Horizontal << 8U) + pVinInfo->DspPhaseShift.Vertical)
            SensorInfo.ReadoutMode = 0x10101U;
            SensorInfo.FieldFormat = (pDspVinDrvCfgCfa->FrameRate.Interlace == 1U)? 2U : 1U;
            HL_SetSensorInfo(HL_MTX_OPT_SET, VinId, &SensorInfo);

            HL_GetVinInfo(HL_MTX_OPT_GET, VinId, &VinInfo);
            VinInfo.SensorNum = 1U; //TBD
            VinInfo.SensorIndexTable[0U] = (UINT8)VinId; //sensor index
            if ((pDspVinDrvCfgCfa->ColorSpace == DSP_VIN_COLOR_SPACE_YUV) &&
                (pDspVinDrvCfgCfa->FrameRate.Interlace == 1U)) {
                VinInfo.InputFormat = DSP_VIN_INPUT_YUV_422_INTLC;
            } else if (pDspVinDrvCfgCfa->ColorSpace == DSP_VIN_COLOR_SPACE_YUV) {
                VinInfo.InputFormat = DSP_VIN_INPUT_YUV_422_PROG;
            } else {
                VinInfo.InputFormat = DSP_VIN_INPUT_RGB_RAW;
            }
            VinInfo.SkipFrame = pDspVinDrvCfgCfa->NumSkipFrame;
            (void)dsp_osal_memcpy(&VinInfo.FrameRate, &pDspVinDrvCfgCfa->FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));
            HL_SetVinInfo(HL_MTX_OPT_SET, VinId, &VinInfo);
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("dsp_liveview_vin_drv_cfg Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

UINT32 dsp_liveview_ik_drv_cfg(UINT16 ViewZoneId, UINT16 Type, const void *pDrvData)
{
    UINT32 Rval;
    const dsp_ik_drv_cfg_sensor_info_t *pDspIkDrvCfgSensorInfo = NULL;
    const dsp_ik_drv_cfg_ability_t *pDspIkDrvCfgAbility = NULL;
    const dsp_ik_drv_cfg_iso_t *pDspIkDrvCfgIso = NULL;
    const dsp_ik_drv_cfg_dol_offset_t *pDspIkDrvCfgDolOfst = NULL;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_STILL_INFO_s StlInfo = {0};

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Input sanity check */
    if (ViewZoneId >= AMBA_DSP_MAX_VIEWZONE_NUM) {
        Rval = DSP_ERR_0001;
    } else if (Type >= NUM_DSP_IK_DRV_CFG_TYPE) {
        Rval = DSP_ERR_0001;
    } else if (pDrvData == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        // DO NOTHING
    }

    /* HW/SW resources check */

    /* Timing sanity check */

    /* Logic sanity check */

    /* Body */
    if (Rval == OK) {
        if (Type == DSP_IK_DRV_CFG_TYPE_SNSR_INFO) {
            dsp_osal_typecast(&pDspIkDrvCfgSensorInfo, &pDrvData);

            if (pDspIkDrvCfgSensorInfo->Pipe == DSP_IK_PIPE_VDO) {
                HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
                ViewZoneInfo->IkSensorMode = pDspIkDrvCfgSensorInfo->SensorMode;
                ViewZoneInfo->IkCompression = pDspIkDrvCfgSensorInfo->Compression;
                HL_GetViewZoneInfoUnLock(ViewZoneId);
            } else {
                HL_GetStlInfo(HL_MTX_OPT_GET, &StlInfo);
                StlInfo.IkSensorMode = pDspIkDrvCfgSensorInfo->SensorMode;
                StlInfo.IkCompression = pDspIkDrvCfgSensorInfo->Compression;
                HL_SetStlInfo(HL_MTX_OPT_SET, &StlInfo);
            }
        } else if (Type == DSP_IK_DRV_CFG_TYPE_ABILITY) {
            dsp_osal_typecast(&pDspIkDrvCfgAbility, &pDrvData);

            HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
            ViewZoneInfo->IkPipe = pDspIkDrvCfgAbility->Pipe;
            ViewZoneInfo->IkStlPipe = pDspIkDrvCfgAbility->StlPipe;
            HL_GetViewZoneInfoUnLock(ViewZoneId);
        } else if (Type == DSP_IK_DRV_CFG_TYPE_ISO) {
            dsp_osal_typecast(&pDspIkDrvCfgIso, &pDrvData);

            HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
            ViewZoneInfo->IkId = pDspIkDrvCfgIso->Id;
            ViewZoneInfo->IkIsoAddr = pDspIkDrvCfgIso->Addr;
            HL_GetViewZoneInfoUnLock(ViewZoneId);
        } else if (Type == DSP_IK_DRV_CFG_TYPE_DOL_OFST) {
            dsp_osal_typecast(&pDspIkDrvCfgDolOfst, &pDrvData);
            if (pDspIkDrvCfgDolOfst->Pipe == DSP_IK_PIPE_VDO) {
                UINT8 i;
                HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
                for (i = 0U; i < VIN_HDR_MAX_EXP_NUM; i++) {
                    ViewZoneInfo->HdrRawYOffset[i] = (UINT16)pDspIkDrvCfgDolOfst->Y[i];
                }
                HL_GetViewZoneInfoUnLock(ViewZoneId);
            } else {
                HL_GetStlInfo(HL_MTX_OPT_GET, &StlInfo);
                (void)dsp_osal_memcpy(StlInfo.IkDolOfstY, pDspIkDrvCfgDolOfst->Y, sizeof(UINT32)*DSP_IK_DOL_OFST_NUM);
                HL_SetStlInfo(HL_MTX_OPT_SET, &StlInfo);
            }
        } else {
            // DO NOTHING
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("dsp_liveview_ik_drv_cfg Error[0x%X] View %d Type %d", Rval, ViewZoneId, Type, 0U, 0U);
    }

    return Rval;
}

void dsp_set_system_config(UINT8 ParIdx, UINT32 Val)
{
    HL_SetDspSystemCfg(ParIdx, Val);
}

