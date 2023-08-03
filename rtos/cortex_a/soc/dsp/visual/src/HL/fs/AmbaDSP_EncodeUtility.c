/**
 *  @file AmbaDSP_EncodeUtility.c
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
 *  @details Implementation of SSP Encode utility internal API
 *
 */

#include "AmbaDSP_Log.h"
#include "AmbaDSP_Cmds.h"
#include "AmbaDSP_BaseCfg.h"
#include "ambadsp_ioctl.h"
#include "AmbaDSP_EncodeAPI.h"
//#include "AmbaDSP_ImageUtility.h"
#include "AmbaDSP_EncodeUtility.h"
#include "AmbaDSPSimilar_EncodeUtility.h"
#include "AmbaDSP_DecodeUtility.h"
#include "dsp_priv_api.h"

static HL_DSP_SYSTEM_CONFIG_s TuneDspSystemCfg = {0};

UINT8 HL_IS_YUV420_INPUT(UINT32 VinDataFmt)
{
    UINT8 Enable = 0U;

    if ((VinDataFmt == DSP_VIN_INPUT_DRAM_INTLC) || (VinDataFmt == DSP_VIN_INPUT_DRAM_PROG)) {
        Enable = 1U;
    }

    return Enable;
}

UINT8 HL_IS_YUV422_INPUT(UINT32 VinDataFmt)
{
    UINT8 Enable = 0U;

    if ((VinDataFmt == DSP_VIN_INPUT_YUV_422_INTLC) || (VinDataFmt == DSP_VIN_INPUT_YUV_422_PROG)) {
        Enable = 1U;
    }

    return Enable;
}

static inline void HL_LvSysBaseCfgFbCache(DSP_BASE_CFG_s *pDspBaseCfg,
                                          cmd_dsp_config_t *pSysSetup,
                                          const CTX_RESOURCE_INFO_s *pResource,
                                          const UINT16 VprocNum)
{
    UINT8 U8Mask;

    if (TuneDspSystemCfg.FbCache > 0U) {
        pDspBaseCfg->FbCache = TuneDspSystemCfg.FbCache;
    } else {
        if (pResource->DecMaxStreamNum > 0U) {
            U8Mask = pDspBaseCfg->FbCache;
            DSP_SetU8Bit(&U8Mask, BASE_CFG_CACHE_SMEM_IDX);
            pDspBaseCfg->FbCache = U8Mask;
        } else if (VprocNum >= 7U) {
            U8Mask = pDspBaseCfg->FbCache;
            DSP_SetU8Bit(&U8Mask, BASE_CFG_CACHE_SMEM_IDX);
            pDspBaseCfg->FbCache = U8Mask;
        } else {
            //TBD
        }
    }

    if (1U == DSP_GetBit(pDspBaseCfg->FbCache, BASE_CFG_CACHE_DRAM_IDX, 1U)) {
        pSysSetup->use_fb_dram_cache = 1U;
    } else if (1U == DSP_GetBit(pDspBaseCfg->FbCache, BASE_CFG_CACHE_SMEM_IDX, 1U)) {
        pSysSetup->use_fb_smem_cache = 1U;
    } else {
        pSysSetup->use_fb_dram_cache = 0U;
        pSysSetup->use_fb_smem_cache = 0U;
    }
}


static inline void HL_LvSysBaseCfgFbpCache(DSP_BASE_CFG_s *pDspBaseCfg,
                                           cmd_dsp_config_t *pSysSetup,
                                           const CTX_RESOURCE_INFO_s *pResource,
                                           const UINT16 VprocNum)
{
    UINT8 U8Mask;

    if (TuneDspSystemCfg.FbpCache > 0U) {
        pDspBaseCfg->FbpCache = TuneDspSystemCfg.FbpCache;
    } else {
        if (pResource->DecMaxStreamNum > 0U) {
            U8Mask = pDspBaseCfg->FbpCache;
            /* fbp use smem as ChenHan suggested */
            DSP_SetU8Bit(&U8Mask, BASE_CFG_CACHE_SMEM_IDX);
            pDspBaseCfg->FbpCache = U8Mask;
        } else if (VprocNum <= 2U) {
            pDspBaseCfg->FbpCache = 0U;
        } else if (VprocNum <= 4U) {
            if (HL_GetEffectChannelEnable() > 0U) {
                if (VprocNum == 4U) {
                    U8Mask = pDspBaseCfg->FbpCache;
                    DSP_SetU8Bit(&U8Mask, BASE_CFG_CACHE_SMEM_IDX);
                    pDspBaseCfg->FbpCache = U8Mask;
                }
            }
        } else if (VprocNum <= 8U) {
            U8Mask = pDspBaseCfg->FbpCache;
            DSP_SetU8Bit(&U8Mask, BASE_CFG_CACHE_SMEM_IDX);
            pDspBaseCfg->FbpCache = U8Mask;
        } else {
            U8Mask = pDspBaseCfg->FbpCache;
            DSP_SetU8Bit(&U8Mask, BASE_CFG_CACHE_SMEM_IDX);
            pDspBaseCfg->FbpCache = U8Mask;
        }
    }

    if (1U == DSP_GetBit(pDspBaseCfg->FbpCache, BASE_CFG_CACHE_DRAM_IDX, 1U)) {
        pSysSetup->use_fbp_dram_cache = 1U;
    } else if (1U == DSP_GetBit(pDspBaseCfg->FbpCache, BASE_CFG_CACHE_SMEM_IDX, 1U)) {
        pSysSetup->use_fbp_smem_cache = 1U;
    } else {
        pSysSetup->use_fbp_dram_cache = 0U;
        pSysSetup->use_fbp_smem_cache = 0U;
    }
}

static inline void HL_LvSysBaseCfgMFbCache(DSP_BASE_CFG_s *pDspBaseCfg,
                                           cmd_dsp_config_t *pSysSetup,
                                           const CTX_RESOURCE_INFO_s *pResource,
                                           const UINT16 VprocNum)
{
    UINT8 U8Mask;

    if (TuneDspSystemCfg.MFbCache > 0U) {
        pDspBaseCfg->MFbCache = TuneDspSystemCfg.MFbCache;
    } else {
        if (pResource->DecMaxStreamNum > 0U) {
            U8Mask = pDspBaseCfg->MFbCache;
            DSP_SetU8Bit(&U8Mask, BASE_CFG_CACHE_SMEM_IDX);
            pDspBaseCfg->MFbCache = U8Mask;
        } else if (VprocNum <= 1U) {
            pDspBaseCfg->MFbCache = (UINT8)0U;
        } else if (VprocNum <= 4U) {
            U8Mask = pDspBaseCfg->MFbCache;
            DSP_SetU8Bit(&U8Mask, BASE_CFG_CACHE_SMEM_IDX);
            pDspBaseCfg->MFbCache = U8Mask;
        } else if (VprocNum <= 8U) {
            U8Mask = pDspBaseCfg->MFbCache;
            DSP_SetU8Bit(&U8Mask, BASE_CFG_CACHE_SMEM_IDX);
            pDspBaseCfg->MFbCache = U8Mask;
        } else {
            U8Mask = pDspBaseCfg->MFbCache;
            DSP_SetU8Bit(&U8Mask, BASE_CFG_CACHE_SMEM_IDX);
            pDspBaseCfg->MFbCache = U8Mask;
        }
    }

    if (1U == DSP_GetBit(pDspBaseCfg->MFbCache, BASE_CFG_CACHE_DRAM_IDX, 1U)) {
        pSysSetup->use_mfb_dram_cache = 1U;
    } else if (1U == DSP_GetBit(pDspBaseCfg->MFbCache, BASE_CFG_CACHE_SMEM_IDX, 1U)) {
        pSysSetup->use_mfb_smem_cache = 1U;
    } else {
        pSysSetup->use_mfb_dram_cache = 0U;
        pSysSetup->use_mfb_smem_cache = 0U;
    }
}


static inline void HL_LvSysBaseCfgMFbpCache(DSP_BASE_CFG_s *pDspBaseCfg,
                                            cmd_dsp_config_t *pSysSetup,
                                            const CTX_RESOURCE_INFO_s *pResource,
                                            const UINT16 VprocNum)
{
    UINT8 U8Mask;

    if (TuneDspSystemCfg.MFbpCache > 0U) {
        pDspBaseCfg->MFbpCache = TuneDspSystemCfg.MFbpCache;
    } else {
        if (pResource->DecMaxStreamNum > 0U) {
            U8Mask = pDspBaseCfg->MFbpCache;
            DSP_SetU8Bit(&U8Mask, BASE_CFG_CACHE_SMEM_IDX);
            pDspBaseCfg->MFbpCache = U8Mask;
        } else if (VprocNum <= 2U) {
            pDspBaseCfg->MFbpCache = (UINT8)0U;
        } else if (VprocNum <= 4U) {
            U8Mask = pDspBaseCfg->MFbpCache;
            DSP_SetU8Bit(&U8Mask, BASE_CFG_CACHE_SMEM_IDX);
            pDspBaseCfg->MFbpCache = U8Mask;
        } else if (VprocNum <= 8U) {
            U8Mask = pDspBaseCfg->MFbpCache;
            DSP_SetU8Bit(&U8Mask, BASE_CFG_CACHE_SMEM_IDX);
            pDspBaseCfg->MFbpCache = U8Mask;
        } else {
            U8Mask = pDspBaseCfg->MFbpCache;
            DSP_SetU8Bit(&U8Mask, BASE_CFG_CACHE_SMEM_IDX);
            pDspBaseCfg->MFbpCache = U8Mask;
        }
    }

    if (1U == DSP_GetBit(pDspBaseCfg->MFbpCache, BASE_CFG_CACHE_DRAM_IDX, 1U)) {
        pSysSetup->use_mfbp_dram_cache = 1U;
    } else if (1U == DSP_GetBit(pDspBaseCfg->MFbpCache, BASE_CFG_CACHE_SMEM_IDX, 1U)) {
        pSysSetup->use_mfbp_smem_cache = 1U;
    } else {
        pSysSetup->use_mfbp_dram_cache = 0U;
        pSysSetup->use_mfbp_smem_cache = 0U;
    }
}

static inline void HL_LvSysBaseCfgImgInfCache(DSP_BASE_CFG_s *pDspBaseCfg,
                                              cmd_dsp_config_t *pSysSetup,
                                              const CTX_RESOURCE_INFO_s *pResource,
                                              const UINT16 VprocNum)
{
    UINT8 U8Mask;

    if (TuneDspSystemCfg.ImgInfCache > 0U) {
        pDspBaseCfg->ImgInfCache = TuneDspSystemCfg.ImgInfCache;
    } else {
        if (pResource->DecMaxStreamNum > 0U) {
            U8Mask = pDspBaseCfg->ImgInfCache;
            DSP_SetU8Bit(&U8Mask, BASE_CFG_CACHE_SMEM_IDX);
            pDspBaseCfg->ImgInfCache = U8Mask;
        } else if (VprocNum <= 4U) {
            if (HL_GetEffectChannelEnable() > 0U) {
                U8Mask = pDspBaseCfg->ImgInfCache;
                DSP_SetU8Bit(&U8Mask, BASE_CFG_CACHE_SMEM_IDX);
                pDspBaseCfg->ImgInfCache = U8Mask;
            }
        } else if (VprocNum <= 8U) {
            U8Mask = pDspBaseCfg->ImgInfCache;
            DSP_SetU8Bit(&U8Mask, BASE_CFG_CACHE_SMEM_IDX);
            pDspBaseCfg->ImgInfCache = U8Mask;
        } else {
            U8Mask = pDspBaseCfg->ImgInfCache;
            DSP_SetU8Bit(&U8Mask, BASE_CFG_CACHE_SMEM_IDX);
            pDspBaseCfg->ImgInfCache = U8Mask;
        }
    }

    if (1U == DSP_GetBit(pDspBaseCfg->ImgInfCache, BASE_CFG_CACHE_DRAM_IDX, 1U)) {
        pSysSetup->use_imginf_dram_cache = 1U;
    } else if (1U == DSP_GetBit(pDspBaseCfg->ImgInfCache, BASE_CFG_CACHE_SMEM_IDX, 1U)) {
        pSysSetup->use_imginf_smem_cache = 1U;
    } else {
        pSysSetup->use_imginf_dram_cache = 0U;
        pSysSetup->use_imginf_smem_cache = 0U;
    }
}

static inline void HL_LvSysBaseCfgMImgInfCache(DSP_BASE_CFG_s *pDspBaseCfg,
                                               cmd_dsp_config_t *pSysSetup,
                                               const CTX_RESOURCE_INFO_s *pResource,
                                               const UINT16 VprocNum)
{
    UINT8 U8Mask;

    if (TuneDspSystemCfg.MImgInfCache > 0U) {
        pDspBaseCfg->MImgInfCache = TuneDspSystemCfg.MImgInfCache;
    } else {
        if (pResource->DecMaxStreamNum > 0U) {
            U8Mask = pDspBaseCfg->MImgInfCache;
            DSP_SetU8Bit(&U8Mask, BASE_CFG_CACHE_SMEM_IDX);
            pDspBaseCfg->MImgInfCache = U8Mask;
        } else if (VprocNum >= 7U) {
            U8Mask = pDspBaseCfg->MImgInfCache;
            DSP_SetU8Bit(&U8Mask, BASE_CFG_CACHE_SMEM_IDX);
            pDspBaseCfg->MImgInfCache = U8Mask;
        } else {
            //TBD
        }
    }

    if (1U == DSP_GetBit(pDspBaseCfg->MImgInfCache, BASE_CFG_CACHE_DRAM_IDX, 1U)) {
        pSysSetup->use_mimginf_dram_cache = 1U;
    } else if (1U == DSP_GetBit(pDspBaseCfg->MImgInfCache, BASE_CFG_CACHE_SMEM_IDX, 1U)) {
        pSysSetup->use_mimginf_smem_cache = 1U;
    } else {
        pSysSetup->use_mimginf_dram_cache = 0U;
        pSysSetup->use_mimginf_smem_cache = 0U;
    }
}

static inline void HL_LvSysBaseCfgIeng(DSP_BASE_CFG_s *pDspBaseCfg,
                                      cmd_dsp_config_t *pSysSetup)
{
    pDspBaseCfg->LdStrNum[0U] = pSysSetup->tot_idsp_ld_str_num[0U];
    pDspBaseCfg->LdStrNum[1U] = pSysSetup->tot_idsp_ld_str_num[1U];
    pDspBaseCfg->LdStrNum[2U] = pSysSetup->tot_idsp_ld_str_num[2U];

    pSysSetup->tot_idsp_ld_str_num[0U] = (TuneDspSystemCfg.IdspSblk0 > 0U)? TuneDspSystemCfg.IdspSblk0: (UINT8)13U;
    pSysSetup->tot_idsp_ld_str_num[1U] = (TuneDspSystemCfg.IdspSblk > 0U)? TuneDspSystemCfg.IdspSblk: (UINT8)6U;
    pSysSetup->tot_idsp_ld_str_num[2U] = (UINT8)6U;//0U;
    pDspBaseCfg->LdStrNum[0U] = pSysSetup->tot_idsp_ld_str_num[0U];
    pDspBaseCfg->LdStrNum[1U] = pSysSetup->tot_idsp_ld_str_num[1U];
    pDspBaseCfg->LdStrNum[2U] = pSysSetup->tot_idsp_ld_str_num[2U];
}

static inline void HL_LvSysBaseCfgVinVirtualDec(DSP_BASE_CFG_s *pDspBaseCfg,
                                             const CTX_RESOURCE_INFO_s *pResource,
                                             const UINT16 VinIdx)
{
    UINT16 ExitLoop = 0U, Idx;
    UINT16 ViewZoneVinId;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    // VirtualVin for TimeLapse
    for (Idx = 0U; Idx < pResource->MaxTimeLapseNum; Idx++) {
        CTX_TIMELAPSE_INFO_s TimeLapseInfo = {0};

        HL_GetTimeLapseInfo(HL_MTX_OPT_ALL, Idx, &TimeLapseInfo);
        if ((TimeLapseInfo.VirtVinId != DSP_VIRT_VIN_IDX_INVALID) &&
            (TimeLapseInfo.VirtVinId == (VinIdx - AMBA_DSP_MAX_VIN_NUM))) {
            pDspBaseCfg->FrmBufNum[VinIdx] = 1U;
            DSP_ClearBit(&pDspBaseCfg->VinCeBitMask, (UINT32)VinIdx);
            DSP_SetBit(&pDspBaseCfg->VinYuvBitMask, (UINT32)VinIdx);
            ExitLoop = 1U;
            break;
        }
    }
    if (ExitLoop == 0U) {
        for (Idx = 0U; Idx < pResource->MaxViewZoneNum; Idx++) {
            HL_GetViewZoneInfoPtr(Idx, &ViewZoneInfo);
            (void)HL_GetViewZoneVinId(Idx, &ViewZoneVinId);
            if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM) &&
                (ViewZoneVinId == VinIdx)) {
                pDspBaseCfg->FrmBufNum[VinIdx] = CV2X_MAX_RAW_FRM_NUM;
                DSP_SetBit(&pDspBaseCfg->VinCeBitMask, (UINT32)VinIdx);
                DSP_ClearBit(&pDspBaseCfg->VinYuvBitMask, (UINT32)VinIdx);
                break;
            } else if (((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_422) ||
                        (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_420)) &&
                        (ViewZoneVinId == VinIdx)) {
                pDspBaseCfg->FrmBufNum[VinIdx] = CV2X_MAX_RAW_FRM_NUM;
                DSP_ClearBit(&pDspBaseCfg->VinCeBitMask, (UINT32)VinIdx);
                DSP_SetBit(&pDspBaseCfg->VinYuvBitMask, (UINT32)VinIdx);
            } else {
                //DO NOTHING
            }
        }
    }
}

static inline void HL_LvSysBaseCfgVinVirtualTL(DSP_BASE_CFG_s *pDspBaseCfg,
                                             const CTX_RESOURCE_INFO_s *pResource,
                                             const UINT16 VinIdx)
{
    UINT16 Idx;
    CTX_VID_DEC_INFO_s VidDecInfo = {0};
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    // VirtualVin for Decode
    for (Idx = 0U; Idx < pResource->MaxViewZoneNum; Idx++) {
        UINT16 DecIdx = 0U;

        HL_GetViewZoneInfoPtr(Idx, &ViewZoneInfo);
        if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) {
            DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &DecIdx);
            HL_GetVidDecInfo(HL_MTX_OPT_ALL, DecIdx, &VidDecInfo);
            if ((VidDecInfo.YuvInVirtVinId != DSP_VIRT_VIN_IDX_INVALID) &&
                (VidDecInfo.YuvInVirtVinId == (VinIdx - AMBA_DSP_MAX_VIN_NUM))) {
                pDspBaseCfg->FrmBufNum[VinIdx] = 2U;
                DSP_ClearBit(&pDspBaseCfg->VinCeBitMask, (UINT32)VinIdx);
                DSP_SetBit(&pDspBaseCfg->VinYuvBitMask, (UINT32)VinIdx);
            }
        }
    }
}

static inline void HL_LvSysBaseCfgVinVirtual(DSP_BASE_CFG_s *pDspBaseCfg,
                                             const CTX_RESOURCE_INFO_s *pResource,
                                             const UINT16 VinIdx)
{
    CTX_STILL_INFO_s CtxStlInfo = {0};

    HL_GetStlInfo(HL_MTX_OPT_ALL, &CtxStlInfo);
    if ((CtxStlInfo.RawInVirtVinId != DSP_VIRT_VIN_IDX_INVALID) &&
        (CtxStlInfo.RawInVirtVinId == (VinIdx - AMBA_DSP_MAX_VIN_NUM))) {
        pDspBaseCfg->FrmBufNum[VinIdx] = CV2X_MAX_RAW_FRM_NUM;
        DSP_SetBit(&pDspBaseCfg->VinCeBitMask, (UINT32)VinIdx);
        DSP_ClearBit(&pDspBaseCfg->VinYuvBitMask, (UINT32)VinIdx);
    } else if ((CtxStlInfo.YuvInVirtVinId != DSP_VIRT_VIN_IDX_INVALID) &&
               (CtxStlInfo.YuvInVirtVinId == (VinIdx - AMBA_DSP_MAX_VIN_NUM))) {
        pDspBaseCfg->FrmBufNum[VinIdx] = 1U;
        DSP_ClearBit(&pDspBaseCfg->VinCeBitMask, (UINT32)VinIdx);
        DSP_SetBit(&pDspBaseCfg->VinYuvBitMask, (UINT32)VinIdx);
    } else if ((CtxStlInfo.YuvEncVirtVinId != DSP_VIRT_VIN_IDX_INVALID) &&
               (CtxStlInfo.YuvEncVirtVinId == (VinIdx - AMBA_DSP_MAX_VIN_NUM))) {
        pDspBaseCfg->FrmBufNum[VinIdx] = 1U;
        DSP_ClearBit(&pDspBaseCfg->VinCeBitMask, (UINT32)VinIdx);
        DSP_SetBit(&pDspBaseCfg->VinYuvBitMask, (UINT32)VinIdx);
    } else {
        // VirtualVin for TimeLapse
        HL_LvSysBaseCfgVinVirtualDec(pDspBaseCfg, pResource, VinIdx);
        // VirtualVin for Decode
        HL_LvSysBaseCfgVinVirtualTL(pDspBaseCfg, pResource, VinIdx);
    }
}

static inline UINT8 HL_FetchFirstViewZoneInfo(const UINT16 VinIdx,
                                              UINT16 *pViewZoneId,
                                              CTX_VIEWZONE_INFO_s **pViewZoneInfo)
{
    UINT8 Found = 0U;
    UINT16 Idx, ViewZoneVinId;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);
    for (Idx = 0U; Idx < Resource->MaxViewZoneNum; Idx++) {
        if (0U == DSP_GetBit(Resource->ViewZoneActiveBit, Idx, 1U)) {
            continue;
        }
        HL_GetViewZoneInfoPtr(Idx, pViewZoneInfo);
        (void)HL_GetViewZoneVinId(Idx, &ViewZoneVinId);

        if (ViewZoneVinId == VinIdx) {
            *pViewZoneId = Idx;
            Found = 1U;
            break;
        }
    }
    return Found;
}

static inline void HL_LvSysBaseCfgVinPhysical(DSP_BASE_CFG_s *pDspBaseCfg,
                                              const CTX_RESOURCE_INFO_s *pResource,
                                              const UINT16 VinId,
                                              const UINT16 PhyVinId)
{
    UINT8 MaxHdrExpNumMiunsOne = 0U, LinearCeEnable = 0U, IsStlCeEnabled;
    UINT16 MaxHdrBlendHeight = 0U;
    UINT16 ViewZoneId;
    CTX_VIN_INFO_s VinInfo = {0};
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    HL_GetVinInfo(HL_MTX_OPT_ALL, PhyVinId, &VinInfo);

    ViewZoneId = 0U;
    /** Fetch first viewzone information of this Vin */
    (void)HL_FetchFirstViewZoneInfo(PhyVinId, &ViewZoneId, &ViewZoneInfo);

    /* Vin output */
    if ((VinInfo.InputFormat == DSP_VIN_INPUT_YUV_422_INTLC) ||
        (VinInfo.InputFormat == DSP_VIN_INPUT_YUV_422_PROG) ||
        (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_422) ||
        (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_420)) {
        pDspBaseCfg->FrmBufNum[VinId] = (pResource->RawBufNum[PhyVinId] > 0U)? pResource->RawBufNum[PhyVinId]: CV2X_MAX_RAW_FRM_NUM;
        DSP_SetBit(&pDspBaseCfg->VinYuvBitMask, (UINT32)VinId);
    } else {
        pDspBaseCfg->FrmBufNum[VinId] = (pResource->RawBufNum[PhyVinId] > 0U)? pResource->RawBufNum[PhyVinId]: CV2X_MAX_RAW_FRM_NUM;
        DSP_ClearBit(&pDspBaseCfg->VinYuvBitMask, (UINT32)VinId);
    }
    (void)HL_GetSystemHdrSettingOnVin(PhyVinId, &MaxHdrExpNumMiunsOne, &LinearCeEnable, &MaxHdrBlendHeight);
    IsStlCeEnabled = (1U == DSP_GetU8Bit(pResource->MaxProcessFormat, 2U/*CE*/, 1U))? (UINT8)1U: (UINT8)0U;
    if ((MaxHdrExpNumMiunsOne > 0U) || (LinearCeEnable > 0U) || (IsStlCeEnabled == 1U)) {
        DSP_SetBit(&pDspBaseCfg->VinCeBitMask, (UINT32)VinId);
    } else {
        DSP_ClearBit(&pDspBaseCfg->VinCeBitMask, (UINT32)VinId);
    }

    if ((pResource->MaxVinEmbdDataWidth[PhyVinId] > 0U) &&
        (pResource->MaxVinEmbdDataHeight[PhyVinId] > 0U)) {
        DSP_SetBit(&pDspBaseCfg->VinAuxBitMask, (UINT32)VinId);
    } else {
        DSP_ClearBit(&pDspBaseCfg->VinAuxBitMask, (UINT32)VinId);
    }
}

static inline void HL_LvSysBaseCfgVin(DSP_BASE_CFG_s *pDspBaseCfg,
                                      const CTX_RESOURCE_INFO_s *pResource,
                                      const UINT16 MaxVinPinNum,
                                      const UINT16 VirtVinNum)
{
    UINT8 IsVirtVin;
    UINT16 Num, VinExist, VinId;
    UINT16 PhyVinId;

    DSP_Bit2U16Cnt((UINT32)pResource->MaxVinBit, &Num);
    pDspBaseCfg->VinNum = (UINT8)Num;

    pDspBaseCfg->VirtVinNum = (UINT8)VirtVinNum;
    pDspBaseCfg->MaxVinPinNum = (UINT8)MaxVinPinNum;

    for (VinId = 0U; VinId < DSP_VIN_MAX_NUM; VinId++) {
        PhyVinId = DSP_VIRT_VIN_IDX_INVALID;

        HL_GetVinExistence(VinId, &VinExist);
        if (VinExist > 0U) {
            DSP_SetBit(&pDspBaseCfg->VinBitMask, (UINT32)VinId);
            (void)HL_IsVirtualVinIdx(VinId, &IsVirtVin);
            if (IsVirtVin == 0U) {
                PhyVinId = VinId;
            } else {
                (void)HL_GetTimeDivisionVirtVinInfo(VinId, &PhyVinId);
            }

            if ((IsVirtVin > 0U) &&
                (PhyVinId == DSP_VIRT_VIN_IDX_INVALID)) {
                HL_LvSysBaseCfgVinVirtual(pDspBaseCfg, pResource, VinId);
            } else if (PhyVinId < AMBA_DSP_MAX_VIN_NUM) {
                HL_LvSysBaseCfgVinPhysical(pDspBaseCfg, pResource, VinId, PhyVinId);
            } else {
                //TBD
            }
        }
    }
}

static inline void HL_LvSysBaseCfgVout(DSP_BASE_CFG_s *pDspBaseCfg)
{
#ifdef SUPPORT_DUMMY_VOUT_THREAD
    /* FS need at least one vout instance */
    UINT8 DummyVoutBit, ActualVoutBit;

    DummyVoutBit = 1U;
    ActualVoutBit = (UINT8)HL_GetVoutTotalBit();
    if (ActualVoutBit == 0U) {
        pDspBaseCfg->VoutBitMask = DummyVoutBit;
    } else {
        pDspBaseCfg->VoutBitMask = ActualVoutBit;
    }
#else
    pDspBaseCfg->VoutBitMask = (UINT8)HL_GetVoutTotalBit();
#endif
}

static inline void HL_LvSysBaseCfgVprocTile(DSP_BASE_CFG_s *pDspBaseCfg,
                                            const CTX_RESOURCE_INFO_s *pResource)
{
    UINT8 IsSliceMode = 0U;
    UINT16 ViewZoneId;
    UINT16 C2YTileY = 1U, MctfTileY = 1U;
    UINT16 MaxVinWidth = 0U, MaxVinHeight = 0U;
    UINT16 MaxMainWidth = 0U, MaxMainHeight = 0U;
    UINT16 MaxChC2YTileNum = 0U, MaxChTileNum = 0U;
    UINT16 C2YInTileX = 0U, C2YTileX = 0U, MaxMctfTileX = 0U;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 TileOverlap;

    (void)HL_GetSystemVinMaxWindow(&MaxVinWidth, &MaxVinHeight);
    (void)HL_GetSystemVprocPinMaxWindow(DSP_VPROC_PIN_MAIN, &MaxMainWidth, &MaxMainHeight);
    (void)HL_IsSliceMode(&IsSliceMode);

    if ((MaxVinWidth > SEC2_MAX_IN_WIDTH) ||
        (MaxMainWidth > SEC2_MAX_OUT_WIDTH) ||
        (IsSliceMode > (UINT8)0U)) {

        pDspBaseCfg->VprocResTileMode = (UINT8)1U;
        /* Calculate Tile number */
        for (ViewZoneId=0; ViewZoneId < pResource->MaxViewZoneNum; ViewZoneId++) {
            UINT8 FixedOverlap;

            HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
            /* if check empty viewzone, will cause overlap always=128 */
            if (ViewZoneInfo->Main.Width == 0U) {
                continue;
            }
            //get init tile overlap
            (void)HL_GetViewZoneWarpOverlapX(ViewZoneId, &TileOverlap);
            FixedOverlap = (pResource->MaxHorWarpComp[ViewZoneId] > 0U) ? 1U : 0U;

            //calculate init tile number, than check availability to decode correct overlap
            (void)HL_CalcVideoTileC2Y(ViewZoneId,
                                      ViewZoneInfo->CapWindow.Width,
                                      SEC2_MAX_IN_WIDTH,
                                      WARP_GRID_EXP,
                                      FixedOverlap,
                                      0U, /* ChkSmem */
                                      &C2YInTileX,
                                      &TileOverlap);
            (void)HL_CalcVideoTileC2Y(ViewZoneId,
                                      ViewZoneInfo->Main.Width,
                                      SEC2_MAX_OUT_WIDTH,
                                      WARP_GRID_EXP,
                                      FixedOverlap,
                                      1U, /* ChkSmem */
                                      &C2YTileX,
                                      &TileOverlap);
            C2YTileX = MAX2_16(C2YInTileX, C2YTileX);

            if (ViewZoneInfo->Main.Width > SEC2_MAX_OUT_WIDTH) {
                (void)HL_CalcVideoTileC2Y(ViewZoneId,
                                          ViewZoneInfo->Main.Width,
                                          SEC2_MAX_OUT_WIDTH,
                                          WARP_GRID_EXP,
                                          FixedOverlap,
                                          1U, /* ChkSmem */
                                          &MaxMctfTileX,
                                          &TileOverlap);
            } else {
                MaxMctfTileX = 1U;
            }

            if (ViewZoneInfo->SliceNumRow > 1U) {
                C2YTileY = ViewZoneInfo->SliceNumRow;
                MctfTileY = ViewZoneInfo->SliceNumRow;
            } else {
                C2YTileY = 1U;
                MctfTileY = 1U;
            }
            MaxChC2YTileNum += (UINT16)(C2YTileX*C2YTileY);
            MaxChTileNum += (UINT16)(MaxMctfTileX*MctfTileY);
        }
    } else {
        pDspBaseCfg->VprocResTileMode = (UINT8)0U;
        MaxChC2YTileNum = (UINT16)(1U*pResource->MaxViewZoneNum);
        MaxChTileNum = (UINT16)(1U*pResource->MaxViewZoneNum);
    }

    // Consider Still in X only
    if (pResource->MaxProcessFormat > 0U) {
        UINT16 Overlap = TILE_OVERLAP_WIDTH;
//FIXME, StlProc overlap
//FIXME, StlProc TileWidthExp
        (void)HL_CalcVideoTileC2Y(0xFF/*ignore ViewId*/,
                                  pResource->MaxStlRawInputWidth,
                                  SEC2_MAX_IN_WIDTH,
                                  WARP_GRID_EXP,
                                  0U, /* FixedOverlap */
                                  0U, /* ChkSmem */
                                  &C2YInTileX,
                                  &Overlap);
        C2YTileX = MAX2_16(C2YInTileX, C2YTileX);
        (void)HL_CalcVideoTileC2Y(0xFF/*ignore ViewId*/,
                                  pResource->MaxStlYuvInputWidth,
                                  SEC2_MAX_IN_WIDTH,
                                  WARP_GRID_EXP,
                                  0U, /* FixedOverlap */
                                  0U, /* ChkSmem */
                                  &C2YInTileX,
                                  &Overlap);
        C2YTileX = MAX2_16(C2YInTileX, C2YTileX);
        (void)HL_CalcVideoTileC2Y(0xFF/*ignore ViewId*/,
                                  pResource->MaxStlMainWidth,
                                  SEC2_MAX_OUT_WIDTH,
                                  WARP_GRID_EXP,
                                  0U, /* FixedOverlap */
                                  1U, /* ChkSmem */
                                  &C2YInTileX,
                                  &Overlap);
        C2YTileX = MAX2_16(C2YInTileX, C2YTileX);

        if (pResource->MaxStlMainWidth > SEC2_MAX_OUT_WIDTH) {
            (void)HL_CalcVideoTileC2Y(0xFF/*ignore ViewId*/,
                                      pResource->MaxStlMainWidth,
                                      SEC2_MAX_OUT_WIDTH,
                                      WARP_GRID_EXP,
                                      0U, /* FixedOverlap */
                                      1U, /* ChkSmem */
                                      &MaxMctfTileX,
                                      &Overlap);
        } else {
            MaxMctfTileX = 1U;
        }
        MaxChC2YTileNum += C2YTileX;
        MaxChTileNum += MaxMctfTileX;
    }
    pDspBaseCfg->VprocResMaxC2YTileNum = (UINT8)MaxChC2YTileNum;
    pDspBaseCfg->VprocResMaxTileNum = (UINT8)MaxChTileNum;
}

static inline void HL_LvSysBaseCfgVprocStl(DSP_BASE_CFG_s *pDspBaseCfg,
                                           const CTX_RESOURCE_INFO_s *pResource,
                                           const UINT16 ViewZoneId)
{
    UINT8 IsStlCeEnable, IsStlHdrEnable, IsStlHisoEnable;

    DSP_ClearBit(&pDspBaseCfg->VprocCompOutBitMask, ViewZoneId);

    IsStlCeEnable = (1U == DSP_GetU8Bit(pResource->MaxProcessFormat, 2U/*CE*/, 1U))? (UINT8)1U: (UINT8)0U;
    IsStlHdrEnable = (1U == DSP_GetU8Bit(pResource->MaxProcessFormat, 3U/*HDR*/, 1U))? (UINT8)1U: (UINT8)0U;
    IsStlHisoEnable = (1U == DSP_GetU8Bit(pResource->MaxProcessFormat, 1U/*HISO*/, 1U))? (UINT8)1U: (UINT8)0U;

    if ((IsStlCeEnable > 0U) || (IsStlHdrEnable > 0U)) {
        DSP_SetBit(&pDspBaseCfg->VprocHdrBitMask, ViewZoneId);
    } else {
        DSP_ClearBit(&pDspBaseCfg->VprocHdrBitMask, ViewZoneId);
    }
    pDspBaseCfg->VprocResHiso = IsStlHisoEnable;

    pDspBaseCfg->VprocC2YFrmBufNum[ViewZoneId] = (UINT8)CV2X_MAX_STL_C2Y_FB_NUM;
    pDspBaseCfg->VprocC2YY12FrmBufNum[ViewZoneId] = (UINT8)CV2X_MAX_STL_C2Y_FB_NUM;
    pDspBaseCfg->VprocMainFrmBufNum[ViewZoneId] = (UINT8)CV2X_MAX_STL_MAIN_FB_NUM;
    pDspBaseCfg->VprocMainMeFrmBufNum[ViewZoneId] = (UINT8)CV2X_MAX_STL_MAIN_FB_NUM;
    pDspBaseCfg->VprocPrevAFrmBufNum[ViewZoneId] = (UINT8)CV2X_MAX_STL_PREVA_FB_NUM;
    pDspBaseCfg->VprocPrevAMeFrmBufNum[ViewZoneId] = (UINT8)DSP_VPROC_FB_NUM_DISABLE;
    pDspBaseCfg->VprocPrevBFrmBufNum[ViewZoneId] = (UINT8)DSP_VPROC_FB_NUM_DISABLE;
    pDspBaseCfg->VprocPrevBMeFrmBufNum[ViewZoneId] = (UINT8)DSP_VPROC_FB_NUM_DISABLE;
    pDspBaseCfg->VprocPrevCFrmBufNum[ViewZoneId] = (UINT8)DSP_VPROC_FB_NUM_DISABLE;
    pDspBaseCfg->VprocPrevCMeFrmBufNum[ViewZoneId] = (UINT8)DSP_VPROC_FB_NUM_DISABLE;
    pDspBaseCfg->VprocHierFrmBufNum[ViewZoneId] = (UINT8)DSP_VPROC_FB_NUM_DISABLE;
    pDspBaseCfg->VprocLndtFrmBufNum[ViewZoneId] = (UINT8)DSP_VPROC_FB_NUM_DISABLE;
#ifdef SUPPORT_DSP_EXT_PIN_BUF
    pDspBaseCfg->VprocExtMemBufNum[ViewZoneId][VPROC_EXT_MEM_TYPE_MAIN] = (UINT8)DSP_VPROC_EXT_FB_DEFAULT_NUM;
    pDspBaseCfg->VprocExtMemBufNum[ViewZoneId][VPROC_EXT_MEM_TYPE_PREV_A] = (UINT8)DSP_VPROC_EXT_FB_DEFAULT_NUM;
    pDspBaseCfg->VprocExtMemBufNum[ViewZoneId][VPROC_EXT_MEM_TYPE_PREV_B] = (UINT8)DSP_VPROC_EXT_FB_DEFAULT_NUM;
    pDspBaseCfg->VprocExtMemBufNum[ViewZoneId][VPROC_EXT_MEM_TYPE_PREV_C] = (UINT8)DSP_VPROC_EXT_FB_DEFAULT_NUM;
    pDspBaseCfg->VprocExtMemBufNum[ViewZoneId][VPROC_EXT_MEM_TYPE_LN_DEC] = (UINT8)0U;
    pDspBaseCfg->VprocExtMemBufNum[ViewZoneId][VPROC_EXT_MEM_TYPE_HIER_0] = (UINT8)0U;
    pDspBaseCfg->VprocExtMemBufNum[ViewZoneId][VPROC_EXT_MEM_TYPE_COMP_RAW] = (UINT8)0U;
    pDspBaseCfg->VprocExtMemBufNum[ViewZoneId][VPROC_EXT_MEM_TYPE_MAIN_ME] = (UINT8)DSP_VPROC_EXT_FB_DEFAULT_NUM;
    pDspBaseCfg->VprocExtMemBufNum[ViewZoneId][VPROC_EXT_MEM_TYPE_PIP_ME] = (UINT8)DSP_VPROC_EXT_FB_DEFAULT_NUM;
    pDspBaseCfg->VprocExtMemBufNum[ViewZoneId][VPROC_EXT_MEM_TYPE_HIER_Y12] = (UINT8)0U;
    pDspBaseCfg->VprocExtMemBufNum[ViewZoneId][VPROC_EXT_MEM_TYPE_C2Y_Y12] = (UINT8)0U;
#endif

    pDspBaseCfg->VprocHisoW0Max = pResource->MaxStlRawInputWidth;
    pDspBaseCfg->VprocHisoSec2Max = SEC2_MAX_IN_WIDTH;
    pDspBaseCfg->VprocHisoSec4Max = SEC4_MAX_IN_WIDTH;
    pDspBaseCfg->VprocHisoSec5Max = PREVA_MAX_WIDTH;
    pDspBaseCfg->VprocHisoSec6Max = PREVB_MAX_WIDTH;
    pDspBaseCfg->VprocHisoSec7Max = PREVC_MAX_WIDTH;
    pDspBaseCfg->VprocHisoSec9Max = PREVD_MAX_WIDTH;
    pDspBaseCfg->VprocHisoColRepMax = pDspBaseCfg->VprocHisoSec2Max;
}

static inline void HL_LvSysBaseCfgVprocVdoPin(DSP_BASE_CFG_s *pDspBaseCfg,
                                              const UINT16 ViewZoneId)
{
    UINT16 Num, AuxNum;

    (void)HL_GetViewZonePinFbNum(ViewZoneId, DSP_VPROC_PIN_MAIN, &Num, &AuxNum, 0U);
    pDspBaseCfg->VprocMainFrmBufNum[ViewZoneId] = (UINT8)Num;
#if defined (SUPPORT_DSP_MAIN_ME_DISABLE)
    pDspBaseCfg->VprocMainMeFrmBufNum[ViewZoneId] = (UINT8)AuxNum;
#else
    pDspBaseCfg->VprocMainMeFrmBufNum[ViewZoneId] = (UINT8)Num;
#endif

    (void)HL_GetViewZonePinFbNum(ViewZoneId, DSP_VPROC_PIN_PREVA, &Num, &AuxNum, 0U);
    if ((Num > 0U) && (Num != DSP_VPROC_FB_NUM_DISABLE)) {
        pDspBaseCfg->VprocPrevAFrmBufNum[ViewZoneId] = (UINT8)Num;
        pDspBaseCfg->VprocPrevAMeFrmBufNum[ViewZoneId] = (UINT8)Num;
    }

    (void)HL_GetViewZonePinFbNum(ViewZoneId, DSP_VPROC_PIN_PREVB, &Num, &AuxNum, 0U);
    if ((Num > 0U) && (Num != DSP_VPROC_FB_NUM_DISABLE)) {
        pDspBaseCfg->VprocPrevBFrmBufNum[ViewZoneId] = (UINT8)Num;
        pDspBaseCfg->VprocPrevBMeFrmBufNum[ViewZoneId] = (UINT8)Num;
    }

    (void)HL_GetViewZonePinFbNum(ViewZoneId, DSP_VPROC_PIN_PREVC, &Num, &AuxNum, 0U);
    if ((Num > 0U) && (Num != DSP_VPROC_FB_NUM_DISABLE)) {
        pDspBaseCfg->VprocPrevCFrmBufNum[ViewZoneId] = (UINT8)Num;
        pDspBaseCfg->VprocPrevCMeFrmBufNum[ViewZoneId] = (UINT8)Num;
    }
}

static inline void HL_LvSysBaseCfgVprocVdoExt(DSP_BASE_CFG_s *pDspBaseCfg,
                                              const UINT16 ViewZoneId)
{
#ifdef SUPPORT_DSP_EXT_PIN_BUF
    UINT16 Num, AuxNum;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);

    if (1U == DSP_GetU8Bit(ViewZoneInfo->PinIsExtMem, DSP_VPROC_PIN_MAIN, 1U)) {
        (void)HL_GetViewZonePinFbNum(ViewZoneId, DSP_VPROC_PIN_MAIN, &Num, &AuxNum, 1U);
        pDspBaseCfg->VprocExtMemBufNum[ViewZoneId][VPROC_EXT_MEM_TYPE_MAIN] = (UINT8)Num;
        pDspBaseCfg->VprocExtMemBufNum[ViewZoneId][VPROC_EXT_MEM_TYPE_MAIN_ME] = (UINT8)Num;
    } else {
        pDspBaseCfg->VprocExtMemBufNum[ViewZoneId][VPROC_EXT_MEM_TYPE_MAIN] = (UINT8)CV2X_MAX_MAIN_FB_NUM;
        pDspBaseCfg->VprocExtMemBufNum[ViewZoneId][VPROC_EXT_MEM_TYPE_MAIN_ME] = (UINT8)CV2X_MAX_MAIN_FB_NUM;
    }
    if (1U == DSP_GetU8Bit(ViewZoneInfo->PinIsExtMem, DSP_VPROC_PIN_PREVA, 1U)) {
        (void)HL_GetViewZonePinFbNum(ViewZoneId, DSP_VPROC_PIN_PREVA, &Num, &AuxNum, 1U);
        pDspBaseCfg->VprocExtMemBufNum[ViewZoneId][VPROC_EXT_MEM_TYPE_PREV_A] = (UINT8)Num;
    } else {
        pDspBaseCfg->VprocExtMemBufNum[ViewZoneId][VPROC_EXT_MEM_TYPE_PREV_A] = (UINT8)DSP_VPROC_EXT_FB_DEFAULT_NUM;
    }
    if (1U == DSP_GetU8Bit(ViewZoneInfo->PinIsExtMem, DSP_VPROC_PIN_PREVB, 1U)) {
        (void)HL_GetViewZonePinFbNum(ViewZoneId, DSP_VPROC_PIN_PREVB, &Num, &AuxNum, 1U);
        pDspBaseCfg->VprocExtMemBufNum[ViewZoneId][VPROC_EXT_MEM_TYPE_PREV_B] = (UINT8)Num;
    } else {
        pDspBaseCfg->VprocExtMemBufNum[ViewZoneId][VPROC_EXT_MEM_TYPE_PREV_B] = (UINT8)DSP_VPROC_EXT_FB_DEFAULT_NUM;
    }
    if (1U == DSP_GetU8Bit(ViewZoneInfo->PinIsExtMem, DSP_VPROC_PIN_PREVC, 1U)) {
        (void)HL_GetViewZonePinFbNum(ViewZoneId, DSP_VPROC_PIN_PREVC, &Num, &AuxNum, 1U);
        pDspBaseCfg->VprocExtMemBufNum[ViewZoneId][VPROC_EXT_MEM_TYPE_PREV_C] = (UINT8)Num;
    } else {
        pDspBaseCfg->VprocExtMemBufNum[ViewZoneId][VPROC_EXT_MEM_TYPE_PREV_C] = (UINT8)DSP_VPROC_EXT_FB_DEFAULT_NUM;
    }

    pDspBaseCfg->VprocExtMemBufNum[ViewZoneId][VPROC_EXT_MEM_TYPE_LN_DEC] = (UINT8)DSP_VPROC_EXT_FB_DEFAULT_NUM;
    pDspBaseCfg->VprocExtMemBufNum[ViewZoneId][VPROC_EXT_MEM_TYPE_HIER_0] = (UINT8)DSP_VPROC_EXT_FB_DEFAULT_NUM;
    pDspBaseCfg->VprocExtMemBufNum[ViewZoneId][VPROC_EXT_MEM_TYPE_COMP_RAW] = (UINT8)0U;
    pDspBaseCfg->VprocExtMemBufNum[ViewZoneId][VPROC_EXT_MEM_TYPE_PIP_ME] = (UINT8)DSP_VPROC_EXT_FB_DEFAULT_NUM;
    pDspBaseCfg->VprocExtMemBufNum[ViewZoneId][VPROC_EXT_MEM_TYPE_HIER_Y12] = (UINT8)DSP_VPROC_EXT_FB_DEFAULT_NUM;
    pDspBaseCfg->VprocExtMemBufNum[ViewZoneId][VPROC_EXT_MEM_TYPE_C2Y_Y12] = (UINT8)DSP_VPROC_EXT_FB_DEFAULT_NUM;
#endif
}

static inline void HL_LvSysBaseCfgVprocVdo(DSP_BASE_CFG_s *pDspBaseCfg,
                                           const UINT16 ViewZoneId)
{
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT32 ViewZoneBitMask = 0U;
    UINT16 YuyvNumber = 0U;
    UINT8 IsPymdY12Ena = 0U;

    HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);

    if (DSP_GetU16Bit(ViewZoneInfo->Pyramid.IsPolySqrt2, DSP_PYMD_Y12_IDX, DSP_PYMD_Y12_LEN) > 0U) {
        IsPymdY12Ena = 1U;
    }

    if (ViewZoneInfo->IsProcRawDramOut > (UINT8)0U) {
        DSP_SetBit(&pDspBaseCfg->VprocCompOutBitMask, ViewZoneId);
    } else {
        DSP_ClearBit(&pDspBaseCfg->VprocCompOutBitMask, ViewZoneId);
    }

    if ((ViewZoneInfo->HdrBlendNumMinusOne > 0U) || (ViewZoneInfo->LinearCe > 0U)) {
        DSP_SetBit(&pDspBaseCfg->VprocHdrBitMask, ViewZoneId);
    } else {
        DSP_ClearBit(&pDspBaseCfg->VprocHdrBitMask, ViewZoneId);
    }

    /* FIXME, use formal information from DspDriver CTX */
    // this is a temp solution to make IK still regression work
    if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM) {
        if ((ViewZoneInfo->IkPipe == (UINT8)DSP_IK_PIPE_STL) &&
            (ViewZoneInfo->IkStlPipe == (UINT8)DSP_IK_STL_PIPE_HI)) {
            pDspBaseCfg->VprocResHiso = (UINT8)1U;
        }
    }

    DSP_SetBit(&ViewZoneBitMask, ViewZoneId);
    (void)HL_GetVprocMipiYuyvNumber(ViewZoneBitMask, &YuyvNumber);
    if (YuyvNumber > 0U) {
        DSP_SetBit(&pDspBaseCfg->VprocMipiYuyvBitMask, ViewZoneId);
    } else {
        DSP_ClearBit(&pDspBaseCfg->VprocMipiYuyvBitMask, ViewZoneId);
    }

    pDspBaseCfg->VprocC2YFrmBufNum[ViewZoneId] = CV2X_MAX_C2Y_FB_NUM;
    pDspBaseCfg->VprocC2YY12FrmBufNum[ViewZoneId] = CV2X_MAX_C2Y_FB_NUM;

    if (IsPymdY12Ena == 1U) {
        DSP_SetBit(&pDspBaseCfg->VprocY12BitMask, ViewZoneId);
        DSP_SetBit(&pDspBaseCfg->VprocC2YY12BitMask, ViewZoneId);
    } else if (ViewZoneInfo->MainY12Enable > 0U) {
        DSP_ClearBit(&pDspBaseCfg->VprocY12BitMask, ViewZoneId);
        DSP_SetBit(&pDspBaseCfg->VprocC2YY12BitMask, ViewZoneId);
    } else {
        DSP_ClearBit(&pDspBaseCfg->VprocY12BitMask, ViewZoneId);
        DSP_ClearBit(&pDspBaseCfg->VprocC2YY12BitMask, ViewZoneId);
    }

    HL_LvSysBaseCfgVprocVdoPin(pDspBaseCfg, ViewZoneId);

    if (ViewZoneInfo->PymdAllocType == ALLOC_INTERNAL) {
        if (ViewZoneInfo->PymdBufNum == 0U) {
            pDspBaseCfg->VprocHierFrmBufNum[ViewZoneId] = (UINT8)CV2X_DEFAULT_HIER_FB_NUM;
            pDspBaseCfg->VprocHierY12FrmBufNum[ViewZoneId] = (IsPymdY12Ena == 1U)? (UINT8)CV2X_DEFAULT_HIER_FB_NUM: DSP_VPROC_HIER_NUM_DISABLE;
        } else {
            pDspBaseCfg->VprocHierFrmBufNum[ViewZoneId] = (UINT8)ViewZoneInfo->PymdBufNum;
            pDspBaseCfg->VprocHierY12FrmBufNum[ViewZoneId] = (IsPymdY12Ena == 1U)? (UINT8)CV2X_DEFAULT_HIER_FB_NUM: DSP_VPROC_HIER_NUM_DISABLE;
        }
    }
    if (ViewZoneInfo->LndtAllocType == ALLOC_INTERNAL) {
        if (ViewZoneInfo->LndtBufNum == 0U) {
            pDspBaseCfg->VprocLndtFrmBufNum[ViewZoneId] = (UINT8)CV2X_DEFAULT_LNDT_FB_NUM;
        } else {
            pDspBaseCfg->VprocLndtFrmBufNum[ViewZoneId] = (UINT8)ViewZoneInfo->LndtBufNum;
        }
    }

    HL_LvSysBaseCfgVprocVdoExt(pDspBaseCfg, ViewZoneId);
}

static inline void HL_LvSysBaseCfgVproc(DSP_BASE_CFG_s *pDspBaseCfg,
                                        const cmd_dsp_config_t *pSysSetup,
                                        const CTX_RESOURCE_INFO_s *pResource,
                                        const UINT16 VprocNum)
{
    UINT8 MaxHdrExpNumMiunsOne = 0U, LinearCeEnable = 0U;
    UINT16 MaxHdrBlendHeight = 0U, i;
    UINT16 LndtWidth = 0U, LndtHeight = 0U;
    UINT16 Num, AuxNum, ViewZoneId;
    CTX_DSP_INST_INFO_s DspInstInfo = {0};

    (void)HL_GetSystemHdrSetting(&MaxHdrExpNumMiunsOne, &LinearCeEnable, &MaxHdrBlendHeight);
    pDspBaseCfg->VprocResHdr = ((MaxHdrExpNumMiunsOne > 0U) || (LinearCeEnable > 0U))? 1U: 0U;

    (void)HL_GetSystemLndtMaxWindow(&LndtWidth, &LndtHeight);
    pDspBaseCfg->VprocResLndt = ((LndtWidth > 0U) && (LndtHeight > 0U))? 1U: 0U;

    pDspBaseCfg->MaxVprocNum = (UINT8)VprocNum;
    pDspBaseCfg->VprocResEfct = (HL_GetEffectChannelEnable() > 0U)? 1U: 0U;

    HL_LvSysBaseCfgVprocTile(pDspBaseCfg, pResource);

    (void)HL_GetPinOutDestBufNum(AMBA_DSP_PREV_DEST_VOUT0, 0U/*negligible*/, &Num, &AuxNum);
    pDspBaseCfg->MaxVprocEfctComm0FrmBufNum = (UINT8)Num;

    (void)HL_GetPinOutDestBufNum(AMBA_DSP_PREV_DEST_VOUT1, 0U/*negligible*/, &Num, &AuxNum);
    pDspBaseCfg->MaxVprocEfctComm1FrmBufNum = (UINT8)Num;

    (void)HL_GetPinOutDestBufNum(AMBA_DSP_PREV_DEST_PIN, DSP_VPROC_PIN_MAIN, &Num, &AuxNum);
    pDspBaseCfg->MaxVprocEfctMainFrmBufNum = (UINT8)Num;
    pDspBaseCfg->MaxVprocEfctMainMeFrmBufNum = (UINT8)Num;

    (void)HL_GetPinOutDestBufNum(AMBA_DSP_PREV_DEST_PIN, DSP_VPROC_PIN_PREVA, &Num, &AuxNum);
    pDspBaseCfg->MaxVprocEfctPipFrmBufNum = (UINT8)Num;
    pDspBaseCfg->MaxVprocEfctPipMeFrmBufNum = (UINT8)Num;

#ifdef SUPPORT_DSP_EXT_MEM_INIT_MODE
    pDspBaseCfg->VprocExtMemAllocMode = pSysSetup->ext_mem_init_mode;
#else
(void)pSysSetup;
    pDspBaseCfg->VprocExtMemAllocMode = (UINT8)0U;
#endif
    for (i=0U; i<pResource->YuvStrmNum; i++) {
        if (1U == HL_GetEffectEnableOnYuvStrm(i)) {
            UINT8 U8Val;
/* FIXME, use simple rule to limit max 2 copy/blend every YuvStrm */
            U8Val = (UINT8)pDspBaseCfg->VprocResMaxEfctCopyNum + DSP_MAX_PP_STRM_COPY_NUMBER;
            pDspBaseCfg->VprocResMaxEfctCopyNum = U8Val;
            pDspBaseCfg->VprocResMaxEfctBlendNum = (UINT8)pDspBaseCfg->VprocResMaxEfctBlendNum + DSP_MAX_PP_STRM_BLEND_NUMBER;
        }
    }

    HL_GetDspInstance(HL_MTX_OPT_ALL, &DspInstInfo);
    for (ViewZoneId=0; (ViewZoneId<VprocNum); ViewZoneId++) {
        if (DspInstInfo.VprocPurpose[ViewZoneId] == VPROC_PURPOSE_STILL) {
            HL_LvSysBaseCfgVprocStl(pDspBaseCfg, pResource, ViewZoneId);
        } else {
            HL_LvSysBaseCfgVprocVdo(pDspBaseCfg, ViewZoneId);
        }
    }
}

static inline void HL_LvSysBaseCfgEnc(DSP_BASE_CFG_s *pDspBaseCfg,
                                      const CTX_RESOURCE_INFO_s *pResource)
{
    UINT16 Idx;
    pDspBaseCfg->EncNum = (UINT8)pResource->MaxEncodeStream;
    for (Idx=0U; Idx<pResource->MaxEncodeStream; Idx++) {
        if (pResource->MaxGopM[Idx] > 1U) {
            pDspBaseCfg->ReconNum[Idx] = DEFAULT_RECON_NUM_IPB;
        } else {
            pDspBaseCfg->ReconNum[Idx] = DEFAULT_RECON_NUM_IP;
        }
        pDspBaseCfg->ReconNum[Idx] += pResource->MaxExtraReconNum[Idx];
    }
}

static inline void HL_LvSysBaseCfgDec(DSP_BASE_CFG_s *pDspBaseCfg,
                                      const CTX_RESOURCE_INFO_s *pResource)
{
    pDspBaseCfg->DecNum = (UINT8)pResource->DecMaxStreamNum;
    pDspBaseCfg->DecFmt = HL_GetDecFmtTotalBit();
}

static inline void HL_LvSysBaseCfgTestFrm(DSP_BASE_CFG_s *pDspBaseCfg,
                                          const CTX_RESOURCE_INFO_s *pResource)
{
    pDspBaseCfg->TestIdspC2YNum = pResource->TestFrmNumOnStage[DSP_TEST_STAGE_IDSP_0];
    pDspBaseCfg->TestIdspY2YNum = pResource->TestFrmNumOnStage[DSP_TEST_STAGE_IDSP_1];
}

static inline void HL_LvSysSetDspBaseCfg(DSP_BASE_CFG_s *pDspBaseCfg,
                                         cmd_dsp_config_t *pSysSetup,
                                         const UINT16 MaxVinPinNum,
                                         const UINT16 VirtVinNum,
                                         const UINT16 VprocNum)
{
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);
    (void)dsp_osal_memset(pDspBaseCfg, 0, sizeof(DSP_BASE_CFG_s));

    /* IN:Cache */
    HL_LvSysBaseCfgFbCache(pDspBaseCfg, pSysSetup, Resource, VprocNum);
    HL_LvSysBaseCfgFbpCache(pDspBaseCfg, pSysSetup, Resource, VprocNum);
    HL_LvSysBaseCfgMFbCache(pDspBaseCfg, pSysSetup, Resource, VprocNum);
    HL_LvSysBaseCfgMFbpCache(pDspBaseCfg, pSysSetup, Resource, VprocNum);
    HL_LvSysBaseCfgImgInfCache(pDspBaseCfg, pSysSetup, Resource, VprocNum);
    HL_LvSysBaseCfgMImgInfCache(pDspBaseCfg, pSysSetup, Resource, VprocNum);

    /* IN:IENG */
    HL_LvSysBaseCfgIeng(pDspBaseCfg, pSysSetup);

    /* IN:Vin */
    HL_LvSysBaseCfgVin(pDspBaseCfg, Resource, MaxVinPinNum, VirtVinNum);

    /* IN:Vout */
    HL_LvSysBaseCfgVout(pDspBaseCfg);

    /* IN:Vproc */
    HL_LvSysBaseCfgVproc(pDspBaseCfg, pSysSetup, Resource, VprocNum);

    /* IN:Enc */
    HL_LvSysBaseCfgEnc(pDspBaseCfg, Resource);

    /* IN:Dec */
    HL_LvSysBaseCfgDec(pDspBaseCfg, Resource);

    /* IN: Misc */
    //TBD

    /* IN:Dec */
    HL_LvSysBaseCfgTestFrm(pDspBaseCfg, Resource);
}

static inline void HL_LvSysHijackDspCfgPart01(const DSP_BASE_PARAM_s *pDspBaseParam,
                                              cmd_dsp_config_t *pSysSetup)
{
#define ORC_ALL_MSG_Q_NUM   (24U)

    pSysSetup->max_daik_par_num = (TuneDspSystemCfg.AikPar != 0U) ? TuneDspSystemCfg.AikPar : pDspBaseParam->AikParNum;
    pSysSetup->max_dram_par_num = (TuneDspSystemCfg.DramPar != 0U) ? TuneDspSystemCfg.DramPar : pDspBaseParam->DramParNum;
    pSysSetup->max_smem_par_num = (TuneDspSystemCfg.SmemPar != 0U) ? TuneDspSystemCfg.SmemPar : pDspBaseParam->SmemParNum;
    pSysSetup->max_sub_par_num = (TuneDspSystemCfg.SubPar != 0U) ? TuneDspSystemCfg.SubPar : pDspBaseParam->SubParNum;
    pSysSetup->max_sup_par_num = (TuneDspSystemCfg.SupPar != 0U) ? TuneDspSystemCfg.SupPar : pDspBaseParam->SupParNum;
    pSysSetup->mbuf_par_size = (TuneDspSystemCfg.MbufPar != 0U) ? TuneDspSystemCfg.MbufPar : pDspBaseParam->MbufParSize;
    pSysSetup->max_fb_num = (TuneDspSystemCfg.Fb != 0U) ? TuneDspSystemCfg.Fb : pDspBaseParam->FbNum;
    pSysSetup->max_fbp_num = (TuneDspSystemCfg.Fbp != 0U) ? TuneDspSystemCfg.Fbp : pDspBaseParam->FbpNum;
    pSysSetup->max_fbp_num = (pSysSetup->max_fbp_num > 255U) ? 255U : pSysSetup->max_fbp_num;
    pSysSetup->max_dbp_num = (TuneDspSystemCfg.Dbp != 0U) ? TuneDspSystemCfg.Dbp : pDspBaseParam->DbpNum;

    pSysSetup->max_orccode_msg_qs = (TuneDspSystemCfg.OrcCodeMsgQ != 0U) ? TuneDspSystemCfg.OrcCodeMsgQ : pDspBaseParam->OrccodeMsgNum;
    pSysSetup->max_orc_all_msg_qs = (TuneDspSystemCfg.OrcAllMsgQ != 0U) ? TuneDspSystemCfg.OrcAllMsgQ : ORC_ALL_MSG_Q_NUM;
}

static inline void HL_LvSysHijackDspCfgPart02(const DSP_BASE_PARAM_s *pDspBaseParam,
                                              cmd_dsp_config_t *pSysSetup)
{
    pSysSetup->max_cbuf_num = (TuneDspSystemCfg.CBuf != 0U) ? TuneDspSystemCfg.CBuf : pDspBaseParam->CBufNum;
    pSysSetup->max_mcbl_num = (TuneDspSystemCfg.Mcbl != 0U) ? TuneDspSystemCfg.Mcbl : pDspBaseParam->McblNum;
    pSysSetup->mbuf_size = (TuneDspSystemCfg.Mbuf != 0U) ? TuneDspSystemCfg.Mbuf : pDspBaseParam->MbufSize;
    pSysSetup->max_bdt_num = (TuneDspSystemCfg.Bdt != 0U) ? TuneDspSystemCfg.Bdt : pDspBaseParam->BdtNum;
    pSysSetup->max_bd_num = (TuneDspSystemCfg.Bd != 0U) ? TuneDspSystemCfg.Bd : pDspBaseParam->BdNum;
    pSysSetup->max_imginf_num = (TuneDspSystemCfg.ImgInf != 0U) ? TuneDspSystemCfg.ImgInf : pDspBaseParam->ImgInfNum;
    pSysSetup->max_ext_fb_num = (TuneDspSystemCfg.ExtFb != 0U) ? TuneDspSystemCfg.ExtFb : pDspBaseParam->ExtFbNum;
    pSysSetup->max_mcb_num = (TuneDspSystemCfg.Mcb != 0U) ? TuneDspSystemCfg.Mcb : pDspBaseParam->McbNum;
    pSysSetup->max_mfbp_num = (TuneDspSystemCfg.MFbp != 0U) ? TuneDspSystemCfg.MFbp : pDspBaseParam->MFbpNum;
    pSysSetup->max_mfb_num = (TuneDspSystemCfg.MFb != 0U) ? TuneDspSystemCfg.MFb : pDspBaseParam->MFbSize;
    pSysSetup->max_ext_mfb_num = (TuneDspSystemCfg.MExtFb != 0U) ? TuneDspSystemCfg.MExtFb : pDspBaseParam->ExtMFbNum;
    pSysSetup->max_mimginf_num = (TuneDspSystemCfg.MImgInf != 0U) ? TuneDspSystemCfg.MImgInf : pDspBaseParam->MImgInfNum;
}

static inline void HL_LvSysDspCfg(cmd_dsp_config_t *pSysSetup,
                                  const DSP_BASE_CFG_s *pDspBaseCfg)
{
    UINT32 Rval;
    DSP_BASE_PARAM_s DspBaseParam;

    (void)dsp_osal_memset(&DspBaseParam, 0, sizeof(DSP_BASE_PARAM_s));
    Rval = HL_PrepareBaseParam(pDspBaseCfg, &DspBaseParam);
    if (Rval != OK) {
        AmbaLL_LogUInt5("PrepareBaseParam Fail [0x%X]", Rval, 0U, 0U, 0U, 0U);
    } else {
        /* Hijack Param base on Backdoor cfg or calculated number */
        HL_LvSysHijackDspCfgPart01(&DspBaseParam, pSysSetup);
        HL_LvSysHijackDspCfgPart02(&DspBaseParam, pSysSetup);

        AmbaLL_LogUInt5("Page %d", DSP_GetPageSize(), 0U, 0U, 0U, 0U);
        //dsp default as 3 (Log2(Size) - 10, 0 means 1024, 1 means 2048, 3 means 8192)
        pSysSetup->page_size_k_log2 = (TuneDspSystemCfg.PgSzLog2 != 0U) ? (UINT8)TuneDspSystemCfg.PgSzLog2 : (UINT8)0U;
    }
}

static inline void HL_LvSysVin(cmd_dsp_config_t *pSysSetup,
                               const CTX_RESOURCE_INFO_s *pResource,
                               const UINT16 MaxVinPinNum,
                               const UINT16 VirtVinNum)
{
    UINT16 i;
    CTX_DSP_INST_INFO_s DspInstInfo = {0};
    UINT8 *pDspVinPinNum = NULL;
    UINT8 PinNum;
    ULONG ULAddr;

    HL_GetDspInstance(HL_MTX_OPT_ALL, &DspInstInfo);

    pSysSetup->vin_bit_mask = (UINT8)pResource->MaxVinBit;

    if (pResource->MaxVinBit > 0U) {
        for (i = 0U; i<AMBA_DSP_MAX_VIN_NUM ; i++) {
            HL_GetPointerDspVinPinCfg(i, &pDspVinPinNum);
            (void)HL_GetVinOutputPinNum(i, 0U/*VirtChanId*/, pResource, &PinNum);
            *pDspVinPinNum = PinNum;
        }
        HL_GetPointerDspVinPinCfg(0U, &pDspVinPinNum);
        dsp_osal_typecast(&ULAddr, &pDspVinPinNum);
        (void)dsp_osal_virt2cli(ULAddr, &pSysSetup->vin_out_pin_num_info_daddr);
    } else {
        pSysSetup->vin_out_pin_num_info_daddr = 0U;
    }

    pSysSetup->max_fov_num_per_vin = (UINT8)MaxVinPinNum;

    if (VirtVinNum > 0U) {
        pSysSetup->num_of_virtual_vins = (UINT8)VirtVinNum;
        for (i = 0U; i < VirtVinNum; i++) {
            UINT16 VirtVinId = i + AMBA_DSP_MAX_VIN_NUM, PhyVinId = DSP_VIRT_VIN_IDX_INVALID;

            (void)HL_GetTimeDivisionVirtVinInfo(VirtVinId, &PhyVinId);
            if (PhyVinId != DSP_VIRT_VIN_IDX_INVALID) {
                PinNum = (UINT8)HL_GetSystemVinMaxViewZoneNum(VirtVinId);
            } else if ((DspInstInfo.DecSharedVirtVinId != DSP_VIRT_VIN_IDX_INVALID) &&
                (DspInstInfo.DecSharedVirtVinId == i)) {
                PinNum = (UINT8)HL_GetSystemVinMaxViewZoneNum(VirtVinId);
            } else {
                PinNum = 1U;
            }

            HL_GetPointerDspVinPinCfg(i + AMBA_DSP_MAX_VIN_NUM, &pDspVinPinNum);
            *pDspVinPinNum = PinNum;
        }

        HL_GetPointerDspVinPinCfg(AMBA_DSP_MAX_VIN_NUM/*FirstVirtVinId*/, &pDspVinPinNum);
        dsp_osal_typecast(&ULAddr, &pDspVinPinNum);
        (void)dsp_osal_virt2cli(ULAddr, &pSysSetup->vir_vin_out_pin_num_info_daddr);
    } else {
        pSysSetup->vir_vin_out_pin_num_info_daddr = 0U;
    }
}

static inline void HL_LvSysEnc(cmd_dsp_config_t *pSysSetup,
                               const CTX_RESOURCE_INFO_s *pResource)
{
    UINT32 Rval;
    UINT32 i, EncIdx = 0U;
    UINT16 YuvStrmIdx, MaxEncWidth = 0U;    //ucode default 0=4096
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;

    pSysSetup->num_of_enc_channel = (UINT8)pResource->MaxEncodeStream;

    /* find max_eng0_width */
    for (i=0U; i<pResource->MaxEncodeStream; i++) {
        for (YuvStrmIdx=0U; YuvStrmIdx<pResource->YuvStrmNum; YuvStrmIdx++) {
            HL_GetYuvStrmInfoLock(YuvStrmIdx, &YuvStrmInfo);
            if (1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_ENCODE_IDX, 1U)) {
                DSP_Bit2Idx((UINT32)YuvStrmInfo->DestEnc, &EncIdx);
                if (EncIdx == i) {
                    if (YuvStrmInfo->MaxWidth > MaxEncWidth) {
                        MaxEncWidth = YuvStrmInfo->MaxWidth;
                    }
                }
            }
            HL_GetYuvStrmInfoUnLock(YuvStrmIdx);
        }

        if (i == pResource->TestEncStrmId) {
            test_binary_header_t EncHdr = {0};

            Rval = HL_GetTestEncHeader(&EncHdr, pResource);
            if (Rval == OK) {
                if (EncHdr.luma0_w > MaxEncWidth) {
                    MaxEncWidth = (UINT16)EncHdr.luma0_w;
                }
            }
        }
    }
    pSysSetup->max_eng0_width = (UINT16)MaxEncWidth;
}

static inline void HL_LvSysEff(cmd_dsp_config_t *pSysSetup)
{
    UINT8 EffectLogicBufNum;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    pSysSetup->is_effect_on = (HL_GetEffectChannelEnable() > 0U)? 1U: 0U;
    if (pSysSetup->is_effect_on > 0U) {
        UINT16 YuvStrmIdx;
        CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;

        EffectLogicBufNum = (UINT8)(NUM_DSP_VPROC_OUT_STRM*pSysSetup->num_of_vproc_channel);

        //if any vin support timeout, then use independent y2y buffer for every chan
        if (HL_GetDefaultRawEnable() == 1U) {
            HL_GetResourcePtr(&Resource);
            for (YuvStrmIdx=0U; YuvStrmIdx<Resource->YuvStrmNum; YuvStrmIdx++) {
                UINT16 TotalPostBldNum = 0;
                HL_GetYuvStrmInfoLock(YuvStrmIdx, &YuvStrmInfo);
                (void)HL_GetEffectChannelPostBlendNum(YuvStrmIdx, &YuvStrmInfo->Layout, &TotalPostBldNum);
                HL_GetYuvStrmInfoUnLock(YuvStrmIdx);
                EffectLogicBufNum += (UINT8)TotalPostBldNum;
            }
        }
    } else {
        EffectLogicBufNum = 0U;
    }
    /* Update Resource */
    HL_GetResourceLock(&Resource);
    Resource->EffectLogicBufNum = EffectLogicBufNum;
    (void)dsp_osal_memset(Resource->EffectLogicBufMask, 0, sizeof(UINT32)*EFFECT_BUF_MASK_DEPTH);
    HL_GetResourceUnLock();
}

static inline void HL_LvSysDec(cmd_dsp_config_t *pSysSetup,
                               const CTX_RESOURCE_INFO_s *pResource)
{
    if (HL_HasDecResource() == 0U) {
        pSysSetup->num_of_dec_channel = 0U;
        pSysSetup->dec_codec_support = 0U;
        pSysSetup->vdec_capture_ena = 0U;
    } else {
        pSysSetup->num_of_dec_channel = (UINT8)pResource->DecMaxStreamNum;
        pSysSetup->dec_codec_support = HL_GetDecFmtTotalBit();
        pSysSetup->vdec_capture_ena = 0U;
    }
}

#ifdef SUPPORT_DUMMY_VOUT_THREAD
static inline UINT32 HL_LvSysVoutDmyThrd(cmd_dsp_config_t *pSysSetup,
                                         const CTX_RESOURCE_INFO_s *pResource)
{
    UINT8 DummyVoutBit, ActualVoutBit;
    CTX_VOUT_INFO_s VoutInfo = {0};
    UINT16 i;
    UINT16 MaxMixerWidth = 0U;

    /* FS need at least one vout instance */
    DummyVoutBit = 1U;
    ActualVoutBit = (UINT8)HL_GetVoutTotalBit();
    if (ActualVoutBit == 0U) {
        pSysSetup->vout_bit_mask = DummyVoutBit;
    } else {
        pSysSetup->vout_bit_mask = ActualVoutBit;
    }

    if (1U == DSP_GetU8Bit(ActualVoutBit, VOUT_IDX_A, 1U)) {
        i = VOUT_IDX_A;
        HL_GetVoutInfo(HL_MTX_OPT_ALL, VOUT_IDX_A, &VoutInfo);
        pSysSetup->vouta_max_pic_ncols = pResource->MaxVoutWidth[i];
        pSysSetup->vouta_max_osd_ncols = (pResource->MaxOsdBufWidth[i] == 0U)? DUMMY_VOUT_MAX_COL: pResource->MaxOsdBufWidth[i];
        if (HL_GET_ROTATE(VoutInfo.VideoCfg.RotateFlip) == DSP_ROTATE_90_DEGREE) {
            (void)HL_GetVoutRotateRescRow(VoutInfo.VideoCfg.Window.Width, &pSysSetup->vouta_max_pic_nrows);
        } else {
            pSysSetup->vouta_max_pic_nrows = VOUT_DISP_DEF_MAX_ROW;
        }
        pSysSetup->vouta_max_osd_nrows = VOUT_OSD_DEF_MAX_ROW; //Use ucode default setting, 2rows
    } else {
        pSysSetup->vouta_max_pic_ncols = DUMMY_VOUT_MAX_COL;
        pSysSetup->vouta_max_osd_ncols = DUMMY_VOUT_MAX_COL;
        pSysSetup->vouta_max_pic_nrows = DUMMY_VOUT_MAX_ROW;
        pSysSetup->vouta_max_osd_nrows = DUMMY_VOUT_MAX_ROW;
    }

    if (1U == DSP_GetU8Bit(ActualVoutBit, VOUT_IDX_B, 1U)) {
        i = VOUT_IDX_B;
        HL_GetVoutInfo(HL_MTX_OPT_ALL, VOUT_IDX_B, &VoutInfo);
        pSysSetup->voutb_max_pic_ncols = pResource->MaxVoutWidth[i];
        pSysSetup->voutb_max_osd_ncols = (pResource->MaxOsdBufWidth[i] == 0U)? DUMMY_VOUT_MAX_COL: pResource->MaxOsdBufWidth[i];
        if (HL_GET_ROTATE(VoutInfo.VideoCfg.RotateFlip) == DSP_ROTATE_90_DEGREE) {
            (void)HL_GetVoutRotateRescRow(VoutInfo.VideoCfg.Window.Width, &pSysSetup->voutb_max_pic_nrows);
        } else {
            pSysSetup->voutb_max_pic_nrows = VOUT_DISP_DEF_MAX_ROW;
        }
        pSysSetup->voutb_max_osd_nrows = VOUT_OSD_DEF_MAX_ROW; //Use ucode default setting, 2rows
    } else {
        pSysSetup->voutb_max_pic_ncols = DUMMY_VOUT_MAX_COL;
        pSysSetup->voutb_max_osd_ncols = DUMMY_VOUT_MAX_COL;
        pSysSetup->voutb_max_pic_nrows = DUMMY_VOUT_MAX_ROW;
        pSysSetup->voutb_max_osd_nrows = DUMMY_VOUT_MAX_ROW;
    }

    if (ActualVoutBit > (UINT8)0U) {
        for (i = 0U; i < AMBA_DSP_MAX_VOUT_NUM; i++) {
            if (1U == DSP_GetU8Bit(ActualVoutBit, i, 1U)) {
                MaxMixerWidth = MAX2_16(MaxMixerWidth, pResource->MaxVoutWidth[i]);
            }
        }
        pSysSetup->vout_mixer_ncols = MaxMixerWidth;
        pSysSetup->vout_mixer_nrows = VOUT_MIXER_DEF_MAX_ROW; //Use ucode default setting, 3rows
    } else {
        pSysSetup->vout_mixer_ncols = DUMMY_VOUT_MAX_COL;
        pSysSetup->vout_mixer_nrows = DUMMY_VOUT_MAX_ROW;
    }
    return OK;
}
#else
static inline UINT32 HL_LvSysVoutNoDmyThrd(cmd_dsp_config_t *pSysSetup,
                                           const CTX_RESOURCE_INFO_s *pResource)
{
    UINT8 VoutBit = (UINT8)HL_GetVoutTotalBit();
    UINT32 Rval = OK;

    if (pResource->VoutBit == RESC_DEFT_VOUT_BIT) {
        pSysSetup->vout_bit_mask = VoutBit;
    } else if (pResource->VoutBit == VoutBit) {
        pSysSetup->vout_bit_mask = VoutBit;
    } else {
        Rval = DSP_ERR_0000;
        AmbaLL_LogUInt5("Vout capability changed 0x%X -> 0x%X", pResource->VoutBit, VoutBit, 0U, 0U, 0U);
    }
    pSysSetup->vouta_max_pic_ncols = pResource->MaxVoutWidth[VOUT_IDX_A];
    pSysSetup->vouta_max_osd_ncols = pResource->MaxOsdBufWidth[VOUT_IDX_A];
    pSysSetup->voutb_max_pic_ncols = pResource->MaxVoutWidth[VOUT_IDX_B];
    pSysSetup->voutb_max_osd_ncols = pResource->MaxOsdBufWidth[VOUT_IDX_B];
    pSysSetup->vouta_max_pic_nrows = VOUT_DISP_DEF_MAX_ROW;
    pSysSetup->vouta_max_osd_nrows = VOUT_OSD_DEF_MAX_ROW;
    pSysSetup->voutb_max_pic_nrows = VOUT_DISP_DEF_MAX_ROW;
    pSysSetup->voutb_max_osd_nrows = VOUT_OSD_DEF_MAX_ROW;
    pSysSetup->vout_mixer_ncols = MAX2_16(pResource->MaxVoutWidth[VOUT_IDX_A], pResource->MaxVoutWidth[VOUT_IDX_B]);
    pSysSetup->vout_mixer_nrows = VOUT_MIXER_DEF_MAX_ROW;

    return Rval;
}
#endif

static inline UINT32 HL_LvSysVout(cmd_dsp_config_t *pSysSetup,
                                  const CTX_RESOURCE_INFO_s *pResource)
{
#ifdef SUPPORT_DUMMY_VOUT_THREAD
    return HL_LvSysVoutDmyThrd(pSysSetup, pResource);
#else
    return HL_LvSysVoutNoDmyThrd(pSysSetup, pResource);
#endif
}

static void HL_LvSysMisc(cmd_dsp_config_t *pSysSetup,
                         const CTX_RESOURCE_INFO_s *pResource)
{
    UINT16 i;

    pSysSetup->is_partial_load_en = pResource->ParLoadEn;
    pSysSetup->vout_double_cmd_en = (UINT8)0U;

    pSysSetup->is_testframe_on = (UINT8)0U;
    for (i = 0U; i<=DSP_TEST_STAGE_IDSP_1; i++) {
        if (pResource->TestFrmNumOnStage[i] > 0U) {
            pSysSetup->is_testframe_on = (UINT8)1U;
            break;
        }
    }
}

static inline void HL_LvSysTestVdspPreproc(void)
{
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourceLock(&Resource);
    /* Test Encode */
    if (Resource->TestFrmNumOnStage[DSP_TEST_STAGE_VDSP_0] > 0U) {
        Resource->TestEncStrmId = Resource->MaxEncodeStream;
        Resource->MaxGopM[Resource->TestEncStrmId] = (UINT8)1U;
        Resource->MaxExtraReconNum[Resource->TestEncStrmId] = (UINT8)0U;
        Resource->MaxSearchRange[Resource->TestEncStrmId] = (UINT8)8U; //TBD
        Resource->MaxStrmFmt[Resource->TestEncStrmId] = (UINT8)0x1U; // AVC only
        Resource->MaxEncodeStream++;
    } else {
        Resource->TestEncStrmId = INVALID_TESTVDSP_STRM_ID;
    }

    /* Test Decode */
    if (Resource->TestFrmNumOnStage[DSP_TEST_STAGE_VDSP_1] > 0U) {
        Resource->TestDecStrmId = Resource->DecMaxStreamNum;
        Resource->DecMaxStrmFmt[Resource->TestDecStrmId] = 1U; //AVC only
        Resource->DecMaxStreamNum++;
    } else {
        Resource->TestDecStrmId = INVALID_TESTVDSP_STRM_ID;
    }
    HL_GetResourceUnLock();
}

static UINT32 FillLiveviewSystemSetup(cmd_dsp_config_t *pSysSetup)
{
#define AUDIO_TICK_4MS  (49155U)  // 4ms * 12288, increase 3 by ucode's suggestion
    UINT16 i, MaxVinPinNum = 0U, VirtVinNum = 0U, VprocNum = 0U;
    UINT32 Rval = OK;
    DSP_BASE_CFG_s DspBaseCfg;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    if (pSysSetup == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        // Vproc Pin connect base on YuvStream
        Rval = HL_VideoProcBinding();

        if (Rval == OK) {
            /* Update Resource setting based on TestEnc */
            HL_LvSysTestVdspPreproc();
        }

        if (Rval == OK) {
            HL_GetResourcePtr(&Resource);

            pSysSetup->dsp_prof_id = DSP_PROF_STATUS_CAMERA;
            pSysSetup->orc_poll_intv = AUDIO_TICK_4MS;
            //ExtMem
#ifdef SUPPORT_DSP_EXT_PIN_BUF
#ifdef SUPPORT_DSP_EXT_MEM_INIT_MODE
            pSysSetup->ext_mem_init_mode = 1U;
#endif
#endif

            (void)HL_GetVprocNum(&VprocNum);

            /* Find MaxVinPinNum, physical */
            for (i=0U; i<AMBA_DSP_MAX_VIN_NUM; i++) {
                MaxVinPinNum = MAX2_16(MaxVinPinNum, Resource->MaxVinVirtChanOutputNum[i][0U]);
            }
            /* Find MaxVinPinNum, virtual */
            (void)HL_GetVirtualVinNum(&VirtVinNum);
            for (i=DSP_VIRTUAL_VIN_START_IDX; i<DSP_VIN_MAX_NUM; i++) {
                MaxVinPinNum = MAX2_16(MaxVinPinNum, (UINT16)HL_GetSystemVinMaxViewZoneNum(i));
            }

            /* DspBase Part */
            HL_LvSysSetDspBaseCfg(&DspBaseCfg,
                                  pSysSetup,
                                  MaxVinPinNum,
                                  VirtVinNum,
                                  VprocNum);
            HL_LvSysDspCfg(pSysSetup, &DspBaseCfg);

            /* Vin Part */
            HL_LvSysVin(pSysSetup, Resource, MaxVinPinNum, VirtVinNum);

            /* Vproc Part */
            pSysSetup->num_of_vproc_channel = (UINT8)VprocNum;

            /* Still Part */

            /* Encode Part */
            HL_LvSysEnc(pSysSetup, Resource);

            /* Effect */
            HL_LvSysEff(pSysSetup);

            /* Decode Part */
            HL_LvSysDec(pSysSetup, Resource);

            /* Vout resource */
            Rval = HL_LvSysVout(pSysSetup, Resource);

            /* Misc resource */
            HL_LvSysMisc(pSysSetup, Resource);
        } else {
            AmbaLL_LogUInt5("HL_VideoProcBinding returned NG 0x%X", Rval, 0U, 0U, 0U, 0U);
        }
    }
    return Rval;
}

UINT32 HL_FillLiveviewSystemSetup(cmd_dsp_config_t *pSysSetup)
{
    UINT32 Rval;
    UINT32 CrcHL, CrcSimilar;
    const UINT32 *pU32 = NULL;
    UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;
#ifdef CONFIG_ENABLE_DSP_MONITOR
    AMBA_DSP_MONITOR_CRC_CMPR_s CrcCmpr;
#endif

    Rval = FillLiveviewSystemSetup(pSysSetup);
    if (pIsSimilarEnabled != NULL) {
        if (1U == IsSimilarEnabled()) {
            UINT32 SIM_Rval;
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_dsp_config_t *SimSysSetup = SIM_DefCtxCmdBufPtrSys;

            dsp_osal_typecast(&pU32, &pSysSetup);
            CrcHL = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_dsp_config_t), 0U);

            SIM_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&SimSysSetup, &CmdBufferAddr);
            SIM_Rval = SIM_FillLiveviewSystemSetup(SimSysSetup);
            if (SIM_Rval == OK) {
                dsp_osal_typecast(&pU32, &SimSysSetup);
                CrcSimilar = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_dsp_config_t), 0U);
#ifdef CONFIG_ENABLE_DSP_MONITOR
                CrcCmpr.ModuleId = AMBA_SSP_PRINT_MODULE_ID;
                CrcCmpr.Crc0 = CrcHL;
                CrcCmpr.Crc1 = CrcSimilar;
                CrcCmpr.UUID = __LINE__;
                SIM_Rval = dsp_mon_crc_cmpr(&CrcCmpr);
                if (SIM_Rval != OK) {
                    AmbaLL_LogUInt5("dsp_mon_crc_cmpr return 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
                }
#else
                if (CrcSimilar != CrcHL) {
                    AmbaLL_LogUInt5("[ERR] Similar CRC mismatch @ %d HL 0x%x Similar 0x%x", __LINE__, CrcHL, CrcSimilar, 0U, 0U);
                }
#endif
            } else {
                AmbaLL_LogUInt5("[ERR] Similar function returned NG 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
            }
            SIM_RelCmdBuffer(CmdBufferId);
        }
    }

    return Rval;
}

static UINT32 FillSystemHalSetup(cmd_dsp_hal_inf_t *SysHalSetup)
{
    UINT32 Rval = OK;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    if (SysHalSetup == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        HL_GetResourceLock(&Resource);
        SysHalSetup->audi_clk_freq = Resource->AudioClk;
        SysHalSetup->core_clk_freq = Resource->CoreClk;
        SysHalSetup->dram_clk_freq = Resource->DramClk;
        SysHalSetup->idsp_clk_freq = Resource->IdspClk;
        HL_GetResourceUnLock();
    }

    return Rval;
}
UINT32 HL_FillSystemHalSetup(cmd_dsp_hal_inf_t *SysHalSetup)
{
    UINT32 Rval;
    UINT32 CrcHL, CrcSimilar;
    const UINT32 *pU32 = NULL;
    UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;
#ifdef CONFIG_ENABLE_DSP_MONITOR
    AMBA_DSP_MONITOR_CRC_CMPR_s CrcCmpr;
#endif

    Rval = FillSystemHalSetup(SysHalSetup);
    if (pIsSimilarEnabled != NULL) {
        if (1U == IsSimilarEnabled()) {
            UINT32 SIM_Rval;
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_dsp_hal_inf_t *SimSysHalSetup = SIM_DefCtxCmdBufPtrHal;

            dsp_osal_typecast(&pU32, &SysHalSetup);
            CrcHL = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_dsp_hal_inf_t), 0U);

            SIM_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&SimSysHalSetup, &CmdBufferAddr);
            SIM_Rval = SIM_FillSystemHalSetup(SimSysHalSetup);
            if (SIM_Rval == OK) {
                dsp_osal_typecast(&pU32, &SimSysHalSetup);
                CrcSimilar = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_dsp_hal_inf_t), 0U);
#ifdef CONFIG_ENABLE_DSP_MONITOR
                CrcCmpr.ModuleId = AMBA_SSP_PRINT_MODULE_ID;
                CrcCmpr.Crc0 = CrcHL;
                CrcCmpr.Crc1 = CrcSimilar;
                CrcCmpr.UUID = __LINE__;
                SIM_Rval = dsp_mon_crc_cmpr(&CrcCmpr);
                if (SIM_Rval != OK) {
                    AmbaLL_LogUInt5("dsp_mon_crc_cmpr return 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
                }
#else
                if (CrcSimilar != CrcHL) {
                    AmbaLL_LogUInt5("[ERR] Similar CRC mismatch @ %d HL 0x%x Similar 0x%x", __LINE__, CrcHL, CrcSimilar, 0U, 0U);
                }
#endif
            } else {
                AmbaLL_LogUInt5("[ERR] Similar function returned NG 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
            }
            SIM_RelCmdBuffer(CmdBufferId);
        }
    }

    return Rval;
}

static inline void HL_FillSysResEncMaxRes(const CTX_RESOURCE_INFO_s *pResource,
                                          UINT32 *pEncStrmMask)
{
    UINT32 Rval;
    UINT16 i, YuvStrmIdx;
    UINT32 EncIdx = 0U;
    UINT32 EncStrmMask = *pEncStrmMask;
    enc_cfg_t *DspStrmEncCfg = NULL;
    UINT8 U8Val;
    UINT16 ReconPostStatus = 0U;

    for (i=0U; i<pResource->MaxEncodeStream; i++) {
        DSP_SetBit(&EncStrmMask, i);
    }

    for (i=0U; i<pResource->MaxEncodeStream; i++) {
        HL_GetPointerDspEncStrmCfg(i, &DspStrmEncCfg);
        /*
         * 20200512, HL SMV logic, reduce it can save smem usage
         *   32 when EncWidth > 1920
         *   48 when EncWidth > 1280
         *   56 when EncWidth <= 1024
         */
        DspStrmEncCfg->max_smvmax = pResource->MaxSearchRange[i];
        if (pResource->MaxGopM[i] > 1U) {
            DspStrmEncCfg->max_rec_fb_num = DEFAULT_RECON_NUM_IPB;
        } else {
            DspStrmEncCfg->max_rec_fb_num = DEFAULT_RECON_NUM_IP;
        }
        U8Val = (UINT8)((UINT8)DspStrmEncCfg->max_rec_fb_num + pResource->MaxExtraReconNum[i]);
        DspStrmEncCfg->max_rec_fb_num = U8Val;

        (void)HL_GetReconPostStatus(i, &ReconPostStatus);
        if (1U == DSP_GetU16Bit(ReconPostStatus, RECON_POST_2_VPROC_BIT, 1U)) {
            DspStrmEncCfg->data_q_out_num = 1U;
        } else {
            DspStrmEncCfg->data_q_out_num = 0U;
        }

        for (YuvStrmIdx=0U; YuvStrmIdx<pResource->YuvStrmNum; YuvStrmIdx++) {
            CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;

            HL_GetYuvStrmInfoLock(YuvStrmIdx, &YuvStrmInfo);
            if (1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_ENCODE_IDX, 1U)) {
                DSP_Bit2Idx((UINT32)YuvStrmInfo->DestEnc, &EncIdx);
                if (EncIdx == i) {
                    DspStrmEncCfg->max_enc_width = YuvStrmInfo->MaxWidth;
                    DspStrmEncCfg->max_enc_height = ALIGN_NUM16(YuvStrmInfo->MaxHeight, 16U);
                    DSP_ClearBit(&EncStrmMask, i);
                }
            }
            HL_GetYuvStrmInfoUnLock(YuvStrmIdx);
        }

        if (i == pResource->TestEncStrmId) {
            test_binary_header_t EncHdr = {0};

            Rval = HL_GetTestEncHeader(&EncHdr, pResource);
            if (Rval == OK) {
                DspStrmEncCfg->max_enc_width = (UINT16)EncHdr.luma0_w;
                DspStrmEncCfg->max_enc_height = (UINT16)EncHdr.luma0_h;
//                DspStrmEncCfg->max_smvmax = (UINT16)56U;
                DSP_ClearBit(&EncStrmMask, i);
            }
        }
    }
    *pEncStrmMask = EncStrmMask;
}

static UINT32 FillSystemResourceEncodeSetup(cmd_dsp_enc_flow_max_cfg_t *ResEncSetup)
{
    UINT8 IsEncRotate;
    UINT16 i, MaxEncWidth = 0U, MaxEncHeight = 0U;
    UINT32 Rval = OK, EncStrmMask = 0U;
    enc_cfg_t *DspStrmEncCfg = NULL;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    ULONG ULAddr;
    UINT16 MaxWin;

    if (ResEncSetup == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        HL_GetResourcePtr(&Resource);

        ResEncSetup->reconfig_enc_type = 1U; //default ON
        ResEncSetup->separate_ref_smem = 0U; //default OFF
        ResEncSetup->smem_encode = DSP_SMEM_ENC_OFF; // not support SGOP in CV2FS
        ResEncSetup->reset_info_fifo_offset = 0U; //default OFF
        ResEncSetup->slice_memd = 0U; //default OFF, turn ON when ULD

        /* Fill MaxResolution */
        HL_FillSysResEncMaxRes(Resource, &EncStrmMask);

        /* EncStrm who is not belong to YuvStrm and StlEnc */
        for (i=0U; i<Resource->MaxEncodeStream; i++) {
            if (1U == DSP_GetBit(EncStrmMask, i, 1U)) {
                HL_GetPointerDspEncStrmCfg(i, &DspStrmEncCfg);
                if (DspStrmEncCfg != NULL) {
                    DspStrmEncCfg->max_enc_width = Resource->MaxExtMemWidth[i];
                    DspStrmEncCfg->max_enc_height = ALIGN_NUM16(Resource->MaxExtMemHeight[i], 16U);
                }
                DSP_ClearBit(&EncStrmMask, i);
            }

            /* check and set rotate for each stream */
            IsEncRotate = (UINT8)DSP_GetU8Bit(Resource->MaxStrmFmt[i], ENC_STRM_FMT_ROTATE_BIT_IDX, ENC_STRM_FMT_ROTATE_LEN);
            if (IsEncRotate == 1U) {
                HL_GetPointerDspEncStrmCfg(i, &DspStrmEncCfg);
                if (DspStrmEncCfg != NULL) {
                    MaxWin = MAX2_16(DspStrmEncCfg->max_enc_width, DspStrmEncCfg->max_enc_height);
                    DspStrmEncCfg->max_enc_width = MaxWin;
                    DspStrmEncCfg->max_enc_height = MaxWin;
                }
            }
        }

        HL_GetPointerDspEncStrmCfg(0U, &DspStrmEncCfg);
        dsp_osal_typecast(&ULAddr, &DspStrmEncCfg);
        (void)dsp_osal_virt2cli(ULAddr, &ResEncSetup->enc_cfg_daddr);

        /**
         * Per Shihao(2015/05/25), to avoid msg queue full under some cases (ex:/low light), set larger msg queue size when necessary.
         * additional SMEM is required.
         */
        ResEncSetup->eng0_msg_queue_size[0] = 0; /* default is 32 */
        ResEncSetup->eng0_msg_queue_size[1] = 64;

        (void)HL_GetEncMaxWindow(&MaxEncWidth, &MaxEncHeight);

        /* Test interval */
        if (Resource->TestFrmInterval[DSP_TEST_STAGE_VDSP_0] > 0U) {
            ResEncSetup->test_encoder_interval = Resource->TestFrmInterval[DSP_TEST_STAGE_VDSP_0]/10U;
        } else {
            ResEncSetup->test_encoder_interval = 0U;
        }
    }

    return Rval;
}
UINT32 HL_FillSystemResourceEncodeSetup(cmd_dsp_enc_flow_max_cfg_t *ResEncSetup)
{
    UINT32 Rval;
    UINT32 CrcHL, CrcSimilar;
    const UINT32 *pU32 = NULL;
    UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;
#ifdef CONFIG_ENABLE_DSP_MONITOR
    AMBA_DSP_MONITOR_CRC_CMPR_s CrcCmpr;
#endif

    Rval = FillSystemResourceEncodeSetup(ResEncSetup);
    if (pIsSimilarEnabled != NULL) {
        if (1U == IsSimilarEnabled()) {
            UINT32 SIM_Rval;
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_dsp_enc_flow_max_cfg_t *SimResEncSetup = SIM_DefCtxCmdBufPtrEncMax;

            dsp_osal_typecast(&pU32, &ResEncSetup);
            CrcHL = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_dsp_enc_flow_max_cfg_t), 0U);

            SIM_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&SimResEncSetup, &CmdBufferAddr);
            SIM_Rval = SIM_FillSystemResourceEncodeSetup(SimResEncSetup);
            if (SIM_Rval == OK) {
                dsp_osal_typecast(&pU32, &SimResEncSetup);
                CrcSimilar = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_dsp_enc_flow_max_cfg_t), 0U);
#ifdef CONFIG_ENABLE_DSP_MONITOR
                CrcCmpr.ModuleId = AMBA_SSP_PRINT_MODULE_ID;
                CrcCmpr.Crc0 = CrcHL;
                CrcCmpr.Crc1 = CrcSimilar;
                CrcCmpr.UUID = __LINE__;
                SIM_Rval = dsp_mon_crc_cmpr(&CrcCmpr);
                if (SIM_Rval != OK) {
                    AmbaLL_LogUInt5("dsp_mon_crc_cmpr return 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
                }
#else
                if (CrcSimilar != CrcHL) {
                    AmbaLL_LogUInt5("[ERR] Similar CRC mismatch @ %d HL 0x%x Similar 0x%x", __LINE__, CrcHL, CrcSimilar, 0U, 0U);
                }
#endif
            } else {
                AmbaLL_LogUInt5("[ERR] Similar function returned NG 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
            }
            SIM_RelCmdBuffer(CmdBufferId);
        }
    }

    return Rval;
}

static inline void HL_ResVprocVinHalf(cmd_dsp_vproc_flow_max_cfg_t *ResVprocSetup,
                                      const UINT8 MaxHdrExpNumMiunsOne,
                                      const UINT16 YuyvNumber,
                                      const UINT16 MaxHdrBlendHeight,
                                      const UINT16 MaxVinWidth,
                                      const UINT16 MaxVinHeight,
                                      const CTX_VIEWZONE_INFO_s *ViewZoneInfo)
{
    UINT8 IsSliceMode = (UINT8)0U;
    UINT8 SysDram2WarpEnable = (UINT8)HL_GetDramToWarpEnable();
    UINT8 SysSmem2WarpEnable = (UINT8)HL_GetSmemToWarpEnable();

    (void)HL_IsSliceMode(&IsSliceMode);

    // Simple Rule of tile : Vin > 1920 or MainW > 1920
    if ((MaxVinWidth > SEC2_MAX_IN_WIDTH) ||
        (ResVprocSetup->W_main_max > SEC2_MAX_OUT_WIDTH) ||
        (IsSliceMode > (UINT8)0U)) {
        ResVprocSetup->is_tile_mode = 1U;
    } else {
        ResVprocSetup->is_tile_mode = 0U;
    }

    /* Saving Dram traffic in real case */
    if (ViewZoneInfo->InputFromMemory != VIN_SRC_FROM_HW) {
        ResVprocSetup->is_c2y_burst_tiling_out = 0U; //Default OFF
    } else {
        if ((1U == DSP_GetU8Bit(IsSliceMode, SLICE_MODE_VER_IDX, 1U)) ||
            (YuyvNumber > 0U)) {
            ResVprocSetup->is_c2y_burst_tiling_out = (UINT8)0U;
            ResVprocSetup->is_c2y_dram_sync_to_warp = (UINT8)1U;
        } else {
            /* Reserve c2y burst tile mode resource when tile mode */
            if (ResVprocSetup->is_tile_mode > 0U) {
                if ((0U == SysDram2WarpEnable) && (1U == SysSmem2WarpEnable)) {
                    ResVprocSetup->is_c2y_burst_tiling_out = (UINT8)0U;
                } else {
                    ResVprocSetup->is_c2y_burst_tiling_out = (UINT8)1U;
                }
            } else {
                ResVprocSetup->is_c2y_burst_tiling_out = (UINT8)0U;
            }
            ResVprocSetup->is_c2y_dram_sync_to_warp = (UINT8)0U;
        }
    }
    if (TuneDspSystemCfg.C2YBurstTile > 0U) {
        ResVprocSetup->is_c2y_burst_tiling_out = (UINT8)(TuneDspSystemCfg.C2YBurstTile - 1U);
    }

    ResVprocSetup->is_sbp_enabled = 1U;

//FIXME, update W0/H0 base on new comment
    /*
     * 2018/12/04, W0/H0 are Sec2 output
     * In IK's fixed flow
     * H rescale always be executed at Sec2
     * V rescale be executed in Sec2(when DownScale) or Sec3(when UpScale)
     */
    ResVprocSetup->W0_max = MaxVinWidth;
    if (MaxHdrExpNumMiunsOne > 0U) {
        ResVprocSetup->H0_max = MaxHdrBlendHeight;
    } else {
        ResVprocSetup->H0_max = MaxVinHeight;
    }

    /* Pipe */
    ResVprocSetup->is_c2y_smem_sync_to_warp = (1U == HL_GetSmemToWarpEnable())? 1U: 0U;

    /* smem saving when all pipe running smem2warp */
    if ((1U == SysSmem2WarpEnable) && (0U == SysDram2WarpEnable)) {
        ResVprocSetup->is_c2y_warp_smem_shared = 1U;
    } else {
        ResVprocSetup->is_c2y_warp_smem_shared = 0U;
    }
}

static inline void HL_ResVprocVin(cmd_dsp_vproc_flow_max_cfg_t *ResVprocSetup,
                                  const UINT32 ViewZoneActiveBit,
                                  const UINT16 MaxMainWidth,
                                  const UINT16 MaxMainHeight)
{
    UINT8 MaxHdrExpNumMiunsOne = 0U, LinearCeEnable = 0U;
    UINT16 YuyvNumber = 0U;
    UINT16 MaxHdrBlendHeight = 0U;
    UINT16 MaxVinWidth = 0U, MaxVinHeight = 0U;
    UINT32 VinId = 0U;
    CTX_VIN_INFO_s VinInfo = {0};
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT8 IsProcRawDramOut = (UINT8)0U;

    //FIXME, Using first ViewZone's Vin?
    HL_GetViewZoneInfoPtr(0U, &ViewZoneInfo);
    DSP_Bit2Idx(ViewZoneInfo->SourceVin, &VinId);
    HL_GetVinInfo(HL_MTX_OPT_ALL, (UINT16)VinId, &VinInfo);
    (void)HL_GetVprocMipiYuyvNumber(ViewZoneActiveBit, &YuyvNumber);
    (void)HL_GetSystemHdrSetting(&MaxHdrExpNumMiunsOne, &LinearCeEnable, &MaxHdrBlendHeight);

    /*
     * Resource Relative
     * Since Y2Y start to support PG Stat (Data Feed in the middle of Sec2)
     * we shall set VprocMaxFlow.InputFmt as RAW to let HL allocate sec2 relative resource
     */
    ResVprocSetup->input_format = DSP_VPROC_INPUT_RAW_RGB;

    (void)HL_GetSystemVinMaxWindow(&MaxVinWidth, &MaxVinHeight);
    if (MaxHdrExpNumMiunsOne > 0U) {
        ResVprocSetup->raw_height_max = VinInfo.CapWindow[0U].Height; //total raw capture height
    } else {
        ResVprocSetup->raw_height_max = MaxVinHeight; //total raw capture height
    }

    if (VinInfo.CfaCompressed > 0U) {
        UINT16 RawWidth = 0U, RawPitch = 0U, Mantissa = 0U, BlkSz = 0U;

        (void)HL_GetCmprRawBufInfo(MaxVinWidth,
                                   VinInfo.CfaCompressed,
                                   VinInfo.CfaCompact,
                                   &RawWidth, &RawPitch,
                                   &Mantissa, &BlkSz);
        ResVprocSetup->is_raw_compressed = 1U;
        ResVprocSetup->raw_comp_blk_sz_wst = (UINT8)BlkSz;
        ResVprocSetup->raw_comp_mantissa_wst = (UINT8)Mantissa;
    } else {
        ResVprocSetup->is_raw_compressed = 0U;
        ResVprocSetup->raw_comp_blk_sz_wst = (UINT8)0U;
        ResVprocSetup->raw_comp_mantissa_wst = (UINT8)0U;
    }

    // Uncompressed raw byte unit, DONT CARE when YUV input
    if (YuyvNumber > 0U) {
        ResVprocSetup->raw_width_max = (UINT16)(((UINT32)MaxVinWidth<<1U)<<1U);
    } else {
        ResVprocSetup->raw_width_max = (UINT16)((UINT32)MaxVinWidth<<1U);
    }

    /* HDR relative */
    ResVprocSetup->is_ce_enabled = ((MaxHdrExpNumMiunsOne > 0U) || (LinearCeEnable > 0U))? 1U: 0U;
    ResVprocSetup->num_exp_max = (UINT8)(MaxHdrExpNumMiunsOne + 1U);

    /* Compressed out will output raw_comp, which can be treat a blended raw */
    (void)HL_GetProcRawDramOutEnable(&IsProcRawDramOut);
    ResVprocSetup->is_compressed_out_enabled = IsProcRawDramOut;

    /* C2Y relative, Dsp treat YuvInput as RawInput */
    ResVprocSetup->is_li_enabled = 1U;
    if (TuneDspSystemCfg.LiEnable > 0U) {
        ResVprocSetup->is_li_enabled = (UINT8)(TuneDspSystemCfg.LiEnable - 1U);
    }
    ResVprocSetup->is_dzoom_enabled = 1U;

    /* Main window
     * W_main_max will change in HL_ResVprocTile and HL_ResVprocNoneTile
     *  */
    ResVprocSetup->W_main_max = MaxMainWidth;
    ResVprocSetup->H_main_max = ALIGN_NUM16(MaxMainHeight, 16U);

    HL_ResVprocVinHalf(ResVprocSetup,
                       MaxHdrExpNumMiunsOne,
                       YuyvNumber,
                       MaxHdrBlendHeight,
                       MaxVinWidth,
                       MaxVinHeight,
                       ViewZoneInfo);
}

static inline void HL_ResVprocTileGetInOutWidth(UINT16 *pCapWidth,
                                                UINT16 *pMainWidth,
                                                const CTX_VIEWZONE_INFO_s *pViewZoneInfo)
{
    *pCapWidth = (pViewZoneInfo->Pipe != DSP_DRAM_PIPE_RAW_ONLY)? pViewZoneInfo->CapWindow.Width: CV2X_VPROC_DUMMY_SIZE;
    *pMainWidth = (pViewZoneInfo->Pipe != DSP_DRAM_PIPE_RAW_ONLY)? pViewZoneInfo->Main.Width: CV2X_VPROC_DUMMY_SIZE;
}

static inline void HL_GetVprocTileFromUserSetting(UINT16 *pC2YTileY,
                                                  UINT16 *pMctfTileY,
                                                  const CTX_VIEWZONE_INFO_s *pViewZoneInfo)
{
    *pC2YTileY = (pViewZoneInfo->SliceNumRow > 1U)? pViewZoneInfo->SliceNumRow: 1U;
    *pMctfTileY = (pViewZoneInfo->SliceNumRow > 1U)? pViewZoneInfo->SliceNumRow: 1U;
}

static inline void HL_ResVprocTile(cmd_dsp_vproc_flow_max_cfg_t *ResVprocSetup)
{
    UINT8 AccumMaxC2YTile = 0U, AccumMaxWarpTile = 0U;
    UINT16 i, j;
    UINT16 TileOverlap;
    UINT16 SysWarpOverlap = 0U;
    UINT16 MaxMainWidth = 0U;
    UINT16 C2YTileY = 1U, MctfTileY = 1U;
    UINT16 MaxC2YTileY = 0U, MaxMctfTileY = 0U;
    UINT16 C2YTileW = 0U, MaxC2YTileW = 0U, C2YInTileX = 0U, C2YTileX = 0U, MaxC2YTileX = 0U;
    UINT16 TileNumOfC2yTile = 0U, TileNumOfMaxC2yTile = 0U;
    UINT16 TempTileW = 0U, PrevPinWidth = 0U, PrevPinHeight = 0U;
    UINT16 MctfTileX = 0U, MaxMctfTileX = 0U;
    UINT16 DontCareValue = 0U;
    UINT16 MaxHierTileW = 0U, MaxLndtTileW = 0U, MaxPrevTileW[DSP_VPROC_PREV_NUM] = {0};
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 CapWidth, MainWidth;

    /*
     * Currently only care TileX only
     * MinTileNum : worst TileNum(c2y) among all channel
     * MaxTileNum : worst TileNum(warp) among all channel
     * MaxChTileNum : Accumulated TileNum(warp) in all channel, each channel contribute its worst TileNum
     * MaxChC2yTileNum : Accumulated TileNum(c2y) in all channel, each channel contribute its worst TileNum
     */
    (void)dsp_osal_memset(&MaxPrevTileW[0U], 0, sizeof(UINT16)*DSP_VPROC_PREV_NUM);
    HL_GetResourcePtr(&Resource);
    for (i = 0U; i < Resource->MaxViewZoneNum; i++) {
        UINT8 FixedOverlap;

        HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
        /* if check empty viewzone, will cause overlap always=128 */
        if (ViewZoneInfo->Main.Width == 0U) {
            continue;
        }

        HL_ResVprocTileGetInOutWidth(&CapWidth, &MainWidth, ViewZoneInfo);

        //get init tile overlap
        (void)HL_GetViewZoneWarpOverlapX(i, &TileOverlap);
        FixedOverlap = (Resource->MaxHorWarpComp[i] > 0U) ? 1U : 0U;

        //calculate init tile number, than check availability to decode correct overlap
        (void)HL_CalcVideoTileC2Y(i, CapWidth,
                                  SEC2_MAX_IN_WIDTH,
                                  WARP_GRID_EXP,
                                  FixedOverlap,
                                  0U, /* ChkSmem */
                                  &C2YInTileX,
                                  &TileOverlap);
        (void)HL_CalcVideoTileC2Y(i, MainWidth,
                                  SEC2_MAX_OUT_WIDTH,
                                  WARP_GRID_EXP,
                                  FixedOverlap,
                                  1U, /* ChkSmem */
                                  &C2YTileX,
                                  &TileOverlap);
#ifdef DEBUG_TILE_CALC
        AmbaLL_LogUInt5("ResVprocSetup i:%u CapWindow.Width:%u C2YTileX:%u C2YInTileX:%u TileOverlap:%u",
                i, CapWidth, C2YTileX, C2YInTileX, TileOverlap);
#endif
        C2YTileX = MAX2_16(C2YInTileX, C2YTileX);

        if (C2YTileX > 0U) {
            (void)HL_CalcVideoTileWidth(MainWidth, C2YTileX, &C2YTileW);
            TileNumOfC2yTile = C2YTileX;
        } else {
//            C2YTileW = ViewZoneInfo->Main.Width;
            TileNumOfC2yTile = 1;
        }
#ifdef DEBUG_TILE_CALC
        AmbaLL_LogUInt5("ResVprocSetup i:%u Main.Width:%u C2YTileX:%u C2YTileW:%u",
                i, MainWidth, C2YTileX, C2YTileW, 0U);
#endif
//FIXME, for DE pipe use smaller max? if MctfTileX>C2YTileX
        if (MainWidth > SEC2_MAX_OUT_WIDTH) {
            (void)HL_CalcVideoTileC2Y(i, MainWidth,
                                      SEC2_MAX_OUT_WIDTH,
                                      WARP_GRID_EXP,
                                      FixedOverlap,
                                      1U, /* ChkSmem */
                                      &MctfTileX,
                                      &TileOverlap);
        } else {
            MctfTileX = 1U;
        }
#ifdef DEBUG_TILE_CALC
        AmbaLL_LogUInt5("ResVprocSetup i:%u Main.Width:%u MctfTileX:%u TileOverlap:%u",
                i, MainWidth, MctfTileX, TileOverlap, 0U);
#endif
        /* Hier/Lndt/Prev tile width */
        {
            (void)HL_CalcVideoTileWidth(Resource->MaxHierWidth[i], MctfTileX, &TempTileW);
            MaxHierTileW = MAX2_16(MaxHierTileW, TempTileW);

            (void)HL_CalcVideoTileWidth(Resource->MaxLndtWidth[i], MctfTileX, &TempTileW);
            MaxLndtTileW = MAX2_16(MaxLndtTileW, TempTileW);

            for (j=DSP_VPROC_PREV_A; j<DSP_VPROC_PREV_D; j++) {
                (void)HL_GetViewZoneVprocPinMaxWin(i, HL_DspPrevCtxVprocPinMap[j], &PrevPinWidth, &PrevPinHeight);
                (void)HL_CalcVideoTileWidth(PrevPinWidth, MctfTileX, &TempTileW);
                MaxPrevTileW[j] = MAX2_16(MaxPrevTileW[j], TempTileW);
            }
        }

        HL_GetVprocTileFromUserSetting(&C2YTileY, &MctfTileY, ViewZoneInfo);

        MaxC2YTileY = MAX2_16(MaxC2YTileY, C2YTileY);
        MaxMctfTileY = MAX2_16(MaxMctfTileY, MctfTileY);

        AccumMaxC2YTile += (UINT8)(C2YTileX*C2YTileY);
        AccumMaxWarpTile += (UINT8)(MctfTileX*MctfTileY);

        TileNumOfMaxC2yTile = MAX2_16(TileNumOfMaxC2yTile, TileNumOfC2yTile);
        MaxMctfTileX = MAX2_16(MaxMctfTileX, MctfTileX);
        MaxC2YTileW = MAX2_16(MaxC2YTileW, C2YTileW);
        MaxC2YTileX = MAX2_16(MaxC2YTileX, C2YTileX);
        MaxMainWidth = MAX2_16(MaxMainWidth, MainWidth);
        MaxMainWidth = MAX2_16(MaxMainWidth, MaxC2YTileW*MaxC2YTileX);
#ifdef DEBUG_TILE_CALC
        AmbaLL_LogUInt5("ResVprocSetup i:%u C2YTileX:%u AccumMaxC2YTile:%u MctfTileX:%u AccumMaxWarpTile:%u",
                i, C2YTileX, AccumMaxC2YTile, MctfTileX, AccumMaxWarpTile);
        AmbaLL_LogUInt5("ResVprocSetup i:%u C2YTileW:%u TileNumOfMaxC2yTile:%u TileNumOfC2yTile:%u",
                i, C2YTileW, TileNumOfMaxC2yTile, TileNumOfC2yTile, 0U);
        AmbaLL_LogUInt5("ResVprocSetup i:%u MaxMctfTileX:%u MctfTileX:%u MaxMainWidth:%u MaxC2YTileW %d",
                i, MaxMctfTileX, MctfTileX, MaxMainWidth, MaxC2YTileW);
#endif
    }

    (void)HL_GetSystemWarpWarpOverlap(&SysWarpOverlap);
#ifdef DEBUG_TILE_CALC
        AmbaLL_LogUInt5("ResVprocSetup AccumMaxWarpTile:%u AccumMaxC2YTile:%u TileNumOfMaxC2yTile:%u MaxMctfTileX:%u MaxMainWidth:%u",
                AccumMaxWarpTile, AccumMaxC2YTile, TileNumOfMaxC2yTile, MaxMctfTileX, MaxMainWidth);
#endif
    //Consider Still
    if (Resource->MaxProcessFormat > 0U) {
        UINT16 Overlap = TILE_OVERLAP_WIDTH;
//FIXME, StlProc overlap
//FIXME, StlProc TileWidthExp
        (void)HL_CalcVideoTileC2Y(0xFF/*ignore ViewId*/,
                                  Resource->MaxStlRawInputWidth,
                                  SEC2_MAX_IN_WIDTH,
                                  WARP_GRID_EXP,
                                  0U, /* FixedOverlap */
                                  0U, /* ChkSmem */
                                  &C2YInTileX,
                                  &Overlap);
        C2YTileX = MAX2_16(C2YInTileX, C2YTileX);
        (void)HL_CalcVideoTileC2Y(0xFF/*ignore ViewId*/,
                                  Resource->MaxStlYuvInputWidth,
                                  SEC2_MAX_IN_WIDTH,
                                  WARP_GRID_EXP,
                                  0U, /* FixedOverlap */
                                  0U, /* ChkSmem */
                                  &C2YInTileX,
                                  &Overlap);
        C2YTileX = MAX2_16(C2YInTileX, C2YTileX);
        (void)HL_CalcVideoTileC2Y(0xFF/*ignore ViewId*/,
                                  Resource->MaxStlMainWidth,
                                  SEC2_MAX_OUT_WIDTH,
                                  WARP_GRID_EXP,
                                  0U, /* FixedOverlap */
                                  1U, /* ChkSmem */
                                  &C2YInTileX,
                                  &Overlap);
        C2YTileX = MAX2_16(C2YInTileX, C2YTileX);
        MaxMainWidth = MAX2_16(MaxMainWidth, Resource->MaxStlMainWidth);

        if (C2YTileX > 0U) {
            (void)HL_CalcVideoTileWidth(Resource->MaxStlMainWidth, C2YTileX, &C2YTileW);
            TileNumOfC2yTile = C2YTileX;
        } else {
//            C2YTileW = Resource->MaxStlMainWidth;
            TileNumOfC2yTile = 1;
        }

        if (Resource->MaxStlMainWidth > SEC2_MAX_OUT_WIDTH) {
            (void)HL_CalcVideoTileC2Y(0xFF/*ignore ViewId*/,
                                      Resource->MaxStlMainWidth,
                                      SEC2_MAX_OUT_WIDTH,
                                      WARP_GRID_EXP,
                                      0U, /* FixedOverlap */
                                      1U, /* ChkSmem */
                                      &MctfTileX,
                                      &Overlap);
        } else {
            MctfTileX = 1U;
        }

        /* PrevA tile width */
        {
            (void)HL_CalcVideoTileWidth(Resource->MaxStlMainWidth, MctfTileX, &TempTileW);
            MaxPrevTileW[DSP_VPROC_PREV_A] = MAX2_16(MaxPrevTileW[DSP_VPROC_PREV_A], TempTileW);
        }

        /* PrevB tile width */
        {
            (void)HL_CalcVideoTileWidth(Resource->MaxStlMainWidth, MctfTileX, &TempTileW);
            MaxPrevTileW[DSP_VPROC_PREV_B] = MAX2_16(MaxPrevTileW[DSP_VPROC_PREV_B], TempTileW);
        }

        AccumMaxC2YTile += (UINT8)C2YTileX;
        AccumMaxWarpTile += (UINT8)MctfTileX;
        TileNumOfMaxC2yTile = MAX2_16(TileNumOfMaxC2yTile, TileNumOfC2yTile);
        MaxMctfTileX = MAX2_16(MaxMctfTileX, MctfTileX);
#ifdef DEBUG_TILE_CALC
        AmbaLL_LogUInt5("ResVprocSetup STL C2YTileX:%u AccumMaxC2YTile:%u MctfTileX:%u AccumMaxWarpTile:%u MaxMainWidth:%u",
                C2YTileX, AccumMaxC2YTile, MctfTileX, AccumMaxWarpTile, MaxMainWidth);
        AmbaLL_LogUInt5("ResVprocSetup STL C2YTileW:%u TileNumOfMaxC2yTile:%u TileNumOfC2yTile:%u MaxMctfTileX:%u MctfTileX:%u",
                C2YTileW, TileNumOfMaxC2yTile, TileNumOfC2yTile, MaxMctfTileX, MctfTileX);
#endif
        SysWarpOverlap = MAX2_16(SysWarpOverlap, TILE_OVERLAP_WIDTH);
    }
#ifdef DEBUG_TILE_CALC
    AmbaLL_LogUInt5("ResVprocSetup AccumMaxWarpTile:%u AccumMaxC2YTile:%u TileNumOfMaxC2yTile:%u MaxMctfTileX:%u MaxMainWidth:%u",
            AccumMaxWarpTile, AccumMaxC2YTile, TileNumOfMaxC2yTile, MaxMctfTileX, MaxMainWidth);
#endif
    ResVprocSetup->max_ch_warp_tile_num = (UINT8)MIN2_16(AccumMaxWarpTile, 16U);
    ResVprocSetup->max_ch_c2y_tile_num = (UINT8)MIN2_16(AccumMaxC2YTile, 16U);

    ResVprocSetup->max_c2y_tile_x_num = (UINT8)TileNumOfMaxC2yTile;
    ResVprocSetup->max_warp_tile_x_num = (UINT8)MaxMctfTileX;
    ResVprocSetup->max_c2y_tile_y_num = (UINT8)MaxC2YTileY;
    ResVprocSetup->max_warp_tile_y_num = (UINT8)MaxMctfTileY;

    /* Warp overlap */
    ResVprocSetup->warp_tile_overlap_x_max = SysWarpOverlap;

    /* Main window */
    ResVprocSetup->W_main_max = MaxMainWidth;

    /* Update MaxVprocTileWidth */
    HL_GetResourceLock(&Resource);
    (void)HL_CalcVideoTileWidth(MaxMainWidth, MaxMctfTileX, &Resource->MaxVprocTileWidth);
    Resource->MaxVprocTileNum = MaxMctfTileX;
    Resource->MinVprocTileNum = TileNumOfMaxC2yTile;
    Resource->MaxVprocTileOverlap = SysWarpOverlap;
    HL_GetResourceUnLock();
#ifdef DEBUG_TILE_CALC
    AmbaLL_LogUInt5("ResVprocSetup MaxVprocTileWidth:%u MaxVprocTileNum:%u MinVprocTileNum:%u MaxVprocTileOverlap:%u",
            Resource->MaxVprocTileWidth, Resource->MaxVprocTileNum,
            Resource->MinVprocTileNum, Resource->MaxVprocTileOverlap, 0U);
#endif
    /* Hier window */
    (void)HL_GetSystemPymdMaxWindow(&DontCareValue, &ResVprocSetup->prev_hier_h_max);
    ResVprocSetup->prev_hier_w_max = (UINT16)(MaxMctfTileX*MaxHierTileW);

    /* Lndt window */
    (void)HL_GetSystemLndtMaxWindow(&DontCareValue, &ResVprocSetup->prev_ln_h_max);
    ResVprocSetup->prev_ln_w_max = (UINT16)(MaxMctfTileX*MaxLndtTileW);

    /* PrevA window */
    (void)HL_GetSystemVprocPinMaxWindow(DSP_VPROC_PIN_PREVA, &DontCareValue, &ResVprocSetup->prev_a_h_max);
    ResVprocSetup->prev_a_w_max = (UINT16)(MaxMctfTileX*MaxPrevTileW[HL_CtxVprocPinDspPrevMap[DSP_VPROC_PIN_PREVA]]);

    /* PrevB window */
    (void)HL_GetSystemVprocPinMaxWindow(DSP_VPROC_PIN_PREVB, &DontCareValue, &ResVprocSetup->prev_b_h_max);
    ResVprocSetup->prev_b_w_max = (UINT16)(MaxMctfTileX*MaxPrevTileW[HL_CtxVprocPinDspPrevMap[DSP_VPROC_PIN_PREVB]]);

    /* PrevC window */
    (void)HL_GetSystemVprocPinMaxWindow(DSP_VPROC_PIN_PREVC, &DontCareValue, &ResVprocSetup->prev_c_h_max);
    ResVprocSetup->prev_c_w_max = (UINT16)(MaxMctfTileX*MaxPrevTileW[HL_CtxVprocPinDspPrevMap[DSP_VPROC_PIN_PREVC]]);

#ifdef SUPPORT_DSP_PREVB_MASTER_MODE
    if ((ResVprocSetup->prev_b_w_max == 0U) &&
        ((ResVprocSetup->prev_a_w_max != 0U) ||
         (ResVprocSetup->prev_c_w_max != 0U) ||
         (ResVprocSetup->prev_hier_w_max != 0U))) {
        ResVprocSetup->prev_b_w_max = 640U;
    }
    if ((ResVprocSetup->prev_b_h_max == 0U) &&
        ((ResVprocSetup->prev_a_h_max != 0U) ||
         (ResVprocSetup->prev_c_h_max != 0U) ||
         (ResVprocSetup->prev_hier_h_max != 0U))) {
        ResVprocSetup->prev_b_h_max = 480U;
    }
#endif
}

static inline void HL_ResVprocNoneTile(cmd_dsp_vproc_flow_max_cfg_t *ResVprocSetup,
                                       const UINT16 MaxMainWidth)
{
    UINT8 AccumMaxC2YTileX, AccumMaxWarpTileX;
    UINT16 C2YTileW = 0U, C2YInTileX = 0U, C2YTileX = 0U;
    UINT16 MaxC2YTileX = 1U, MctfTileX = 0U, MaxMctfTileX = 1U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);
    AccumMaxC2YTileX = (UINT8)(1U*Resource->MaxViewZoneNum);
    AccumMaxWarpTileX = (UINT8)(1U*Resource->MaxViewZoneNum);

    //Consider Still
    if (Resource->MaxProcessFormat > 0U) {
        UINT16 Overlap = TILE_OVERLAP_WIDTH;
        //FIXME, StlProc overlap
        //FIXME, StlProc TileWidthExp
        (void)HL_CalcVideoTileC2Y(0xFF/*ignore ViewId*/,
                                  Resource->MaxStlRawInputWidth,
                                  SEC2_MAX_IN_WIDTH,
                                  WARP_GRID_EXP,
                                  0U, /* FixedOverlap */
                                  0U, /* ChkSmem */
                                  &C2YInTileX,
                                  &Overlap);
        C2YTileX = MAX2_16(C2YInTileX, C2YTileX);
        (void)HL_CalcVideoTileC2Y(0xFF/*ignore ViewId*/,
                                  Resource->MaxStlYuvInputWidth,
                                  SEC2_MAX_IN_WIDTH,
                                  WARP_GRID_EXP,
                                  0U, /* FixedOverlap */
                                  0U, /* ChkSmem */
                                  &C2YInTileX,
                                  &Overlap);
        C2YTileX = MAX2_16(C2YInTileX, C2YTileX);
        (void)HL_CalcVideoTileC2Y(0xFF/*ignore ViewId*/,
                                  Resource->MaxStlMainWidth,
                                  SEC2_MAX_OUT_WIDTH,
                                  WARP_GRID_EXP,
                                  0U, /* FixedOverlap */
                                  1U, /* ChkSmem */
                                  &C2YInTileX,
                                  &Overlap);
        C2YTileX = MAX2_16(C2YInTileX, C2YTileX);

        if (C2YTileX > 0U) {
            (void)HL_CalcVideoTileWidth(Resource->MaxStlMainWidth, C2YTileX, &C2YTileW);
        }

        if (Resource->MaxStlMainWidth > SEC2_MAX_OUT_WIDTH) {
            (void)HL_CalcVideoTileC2Y(0xFF/*ignore ViewId*/,
                                      Resource->MaxStlMainWidth,
                                      SEC2_MAX_OUT_WIDTH,
                                      WARP_GRID_EXP,
                                      0U, /* FixedOverlap */
                                      1U, /* ChkSmem */
                                      &MctfTileX,
                                      &Overlap);
        } else {
            MctfTileX = 1U;
        }

        AccumMaxC2YTileX += (UINT8)C2YTileX;
        AccumMaxWarpTileX += (UINT8)MctfTileX;

        MaxC2YTileX = MAX2_16(MaxC2YTileX, C2YTileX);
        MaxMctfTileX = MAX2_16(MaxMctfTileX, MctfTileX);
    }

    /* Main window */
    ResVprocSetup->W_main_max = MaxMainWidth;

    /* Update MaxVprocTileWidth */
    HL_GetResourceLock(&Resource);
    Resource->MaxVprocTileWidth = MaxMainWidth;
    HL_GetResourceUnLock();

    ResVprocSetup->max_ch_warp_tile_num = (UINT8)AccumMaxWarpTileX;
    ResVprocSetup->max_ch_c2y_tile_num = (UINT8)AccumMaxC2YTileX;
    ResVprocSetup->max_c2y_tile_x_num = (UINT8)MaxC2YTileX;
    ResVprocSetup->max_warp_tile_x_num = (UINT8)MaxMctfTileX;
    ResVprocSetup->max_c2y_tile_y_num = (UINT8)1U;
    ResVprocSetup->max_warp_tile_y_num = (UINT8)1U;

    /* Warp relative
     * move to HL_ResVprocMisc */
//    (void)HL_GetSystemWarpWarpOverlap(&SysWarpOverlap);
//    ResVprocSetup->warp_tile_overlap_x_max = (SysWarpOverlap == 0U) ? TILE_OVERLAP_WIDTH : SysWarpOverlap;

    /* Hier window */
    (void)HL_GetSystemPymdMaxWindow(&ResVprocSetup->prev_hier_w_max, &ResVprocSetup->prev_hier_h_max);

    /* Lndt window */
    (void)HL_GetSystemLndtMaxWindow(&ResVprocSetup->prev_ln_w_max, &ResVprocSetup->prev_ln_h_max);

    /* PrevA window */
    (void)HL_GetSystemVprocPinMaxWindow(DSP_VPROC_PIN_PREVA, &ResVprocSetup->prev_a_w_max, &ResVprocSetup->prev_a_h_max);

    /* PrevB window */
    (void)HL_GetSystemVprocPinMaxWindow(DSP_VPROC_PIN_PREVB, &ResVprocSetup->prev_b_w_max, &ResVprocSetup->prev_b_h_max);

    /* PrevC window */
    (void)HL_GetSystemVprocPinMaxWindow(DSP_VPROC_PIN_PREVC, &ResVprocSetup->prev_c_w_max, &ResVprocSetup->prev_c_h_max);

#ifdef SUPPORT_DSP_PREVB_MASTER_MODE
    if ((ResVprocSetup->prev_b_w_max == 0U) &&
        ((ResVprocSetup->prev_a_w_max != 0U) ||
         (ResVprocSetup->prev_c_w_max != 0U) ||
         (ResVprocSetup->prev_hier_w_max != 0U))) {
        ResVprocSetup->prev_b_w_max = 640U;
    }
    if ((ResVprocSetup->prev_b_h_max == 0U) &&
        ((ResVprocSetup->prev_a_h_max != 0U) ||
         (ResVprocSetup->prev_c_h_max != 0U) ||
         (ResVprocSetup->prev_hier_h_max != 0U))) {
        ResVprocSetup->prev_b_h_max = 480U;
    }
#endif

}

static inline void HL_ResVprocMiscSecondHalf(cmd_dsp_vproc_flow_max_cfg_t *ResVprocSetup, UINT8 MainY12Enable, UINT8 HierY12Enable)
{
    /* Pymd relative */
    if ((ResVprocSetup->prev_hier_w_max > 0U) && (ResVprocSetup->prev_hier_h_max > 0U)) {
        ResVprocSetup->is_prev_hier_enabled = 1U;
        ResVprocSetup->is_y12_out_enabled = (HierY12Enable > 0U)? 1U: 0U;
    } else {
        ResVprocSetup->is_prev_hier_enabled = 0U;
        ResVprocSetup->is_y12_out_enabled = 0U;
    }

    ResVprocSetup->is_hier_burst_tiling_out = 0U; //Default OFF
    ResVprocSetup->hier_poly_tile_overlap_x_max = 0U; //Default ?

    /* Lndt relative */
    if ((ResVprocSetup->prev_ln_w_max > 0U) && (ResVprocSetup->prev_ln_h_max > 0U)) {
        ResVprocSetup->is_prev_ln_enabled = 1U;
    } else {
        ResVprocSetup->is_prev_ln_enabled = 0U;
    }

    /* Main Y12 relative */
    if ((MainY12Enable > 0U) && (HierY12Enable == 0U)) {
        ResVprocSetup->is_c2y_y12_out_enabled = 1U;
    } else {
        ResVprocSetup->is_c2y_y12_out_enabled = 0U;
    }
}

static inline UINT32 HL_ResVprocMisc(cmd_dsp_vproc_flow_max_cfg_t *ResVprocSetup,
                                     const CTX_RESOURCE_INFO_s *pResource)
{
    UINT32 Rval;
    UINT16 LumaDmaSize = 0U, ChromaDmaSize = 0U;
    UINT8 MctfEnable = 0U, MctsEnable = 0U, MctfCmpr = 0U;
    UINT16 BufNum, AuxBufNum, NumGroup = 0U;
    ULONG ULAddr;
    UINT8 MainY12Enable = 0U, HierY12Enable = 0U, ComChFmt = DSP_YUV_420;
    UINT16 LumaWaitLine = 0U, ChromaWaitLine = 0U;
    UINT16 i;

    /* Vproc group */
    /* Calculate the total VprocGroup before call HL_GetVprocGroupNum */
    Rval = HL_CalcVprocGroupNum();
    if (Rval == DSP_ERR_NONE) {
        UINT8 SysDram2WarpEnable = (UINT8)HL_GetDramToWarpEnable();
        UINT8 SysSmem2WarpEnable = (UINT8)HL_GetSmemToWarpEnable();

        (void)HL_GetVprocGroupNum(&NumGroup);
        ResVprocSetup->num_of_vproc_groups = (UINT8)NumGroup;

        /* Warp relative */
        ResVprocSetup->is_warp_enabled = 1U; //Default ON
        (void)HL_GetSystemWarpDmaSetting(&LumaDmaSize, &ChromaDmaSize);
        (void)HL_GetSystemWarpWaitLineSetting(&LumaWaitLine, &ChromaWaitLine);
        if ((0U == SysDram2WarpEnable) && (1U == SysSmem2WarpEnable)) {
            ResVprocSetup->warp_wait_lines_max = CV2X_WARP_SMEM2WARP_WAITLINE; //HL suggest 8lines increment
        } else {
            ResVprocSetup->warp_wait_lines_max = CV2X_WARP_WAITLINE; //HL suggest 8lines increment
        }

        ResVprocSetup->warp_wait_lines_max = (LumaWaitLine > 0U)? LumaWaitLine: ResVprocSetup->warp_wait_lines_max;

        ResVprocSetup->warp_a_y_in_blk_h_max = (LumaDmaSize > 0U)? LumaDmaSize: CV2X_WARP_DMA_SIZE; //only be used when Dram2Warp
        ResVprocSetup->warp_a_uv_in_blk_h_max = (ChromaDmaSize > 0U)? ChromaDmaSize: CV2X_WARP_CHROMA_DMA_SIZE; //only be used when Dram2Warp

        if ((0U == SysDram2WarpEnable) && (1U == SysSmem2WarpEnable)) {
            ResVprocSetup->warp_tile_overlap_x_max = EFCY_TILE_OVERLAP_WIDTH;
        } else {
            UINT16 SysWarpOverlap = 0U;

            (void)HL_GetSystemWarpWarpOverlap(&SysWarpOverlap);
            ResVprocSetup->warp_tile_overlap_x_max = (SysWarpOverlap == 0U) ? TILE_OVERLAP_WIDTH : SysWarpOverlap;
        }

        // MaxPreWarp Shall follow Main, FIXME when introduce DZOOM
        ResVprocSetup->W_pre_warp_luma_max = ResVprocSetup->W_main_max;
        ResVprocSetup->H_pre_warp_luma_max = ResVprocSetup->H_main_max;

        /* Mctf/Mcts relative */
        (void)HL_GetSystemMctfSetting(&MctfEnable, &MctsEnable, &MctfCmpr);
        ResVprocSetup->is_mctf_enabled = MctfEnable;
        ResVprocSetup->is_mctf_cmpr_en = MctfCmpr;
        ResVprocSetup->is_mcts_disabled = (MctsEnable > 0U)? (UINT8)0U: (UINT8)1U;

        /* Main and Pip pp stream buffer */
        /*
         * 20190619, In CVx current design
         * PostP_XX_ and Prev_comX_ have different meaning
         * NonEffect Pipeline
         *   - means the YuvBuffer to Destination
         * Effect Pipeline
         *   - means the YuvBuffer from which VprocPin
         *     postp_main_ : From Main
         *     postp_Pip_ : From PrevA
         *     prev_com0 : From PrevC
         *     prev_com1 : From PrevB
         */

        (void)HL_GetPinOutDestBufNum(AMBA_DSP_PREV_DEST_PIN, DSP_VPROC_PIN_MAIN, &BufNum, &AuxBufNum);
        ResVprocSetup->postp_main_fb_num = (UINT8)BufNum;
        ResVprocSetup->postp_main_me01_fb_num = (UINT8)AuxBufNum;
        (void)HL_GetPinOutDestBufNum(AMBA_DSP_PREV_DEST_PIN, DSP_VPROC_PIN_PREVA, &BufNum, &AuxBufNum);
        ResVprocSetup->postp_pip_fb_num = (UINT8)BufNum;
        ResVprocSetup->postp_pip_me01_fb_num = (UINT8)AuxBufNum;

        /* Prev filter relative */
        (void)HL_GetPinOutDestBufNum(AMBA_DSP_PREV_DEST_VOUT0, 0U/*negligible*/, &BufNum, &AuxBufNum);
        ResVprocSetup->prev_com0_fb_num = (UINT8)BufNum;
        (void)HL_GetPinOutDestBufNum(AMBA_DSP_PREV_DEST_VOUT1, 0U/*negligible*/, &BufNum, &AuxBufNum);
        ResVprocSetup->prev_com1_fb_num = (UINT8)BufNum;
        ResVprocSetup->is_prev_a_enabled = ((ResVprocSetup->prev_a_w_max>0U) && (ResVprocSetup->prev_a_h_max>0U))? 1U: 0U;
        ResVprocSetup->is_prev_b_enabled = ((ResVprocSetup->prev_b_w_max>0U) && (ResVprocSetup->prev_b_h_max>0U))? 1U: 0U;
        ResVprocSetup->is_prev_c_enabled = ((ResVprocSetup->prev_c_w_max>0U) && (ResVprocSetup->prev_c_h_max>0U))? 1U: 0U;

        /* chroma format */
        (void)HL_GetPinOutDestChFmt(AMBA_DSP_PREV_DEST_VOUT0, 0U/*negligible*/, &ComChFmt);
        ResVprocSetup->prev_com0_ch_fmt = ComChFmt;
        (void)HL_GetPinOutDestChFmt(AMBA_DSP_PREV_DEST_VOUT1, 0U/*negligible*/, &ComChFmt);
        ResVprocSetup->prev_com1_ch_fmt = ComChFmt;

        (void)HL_GetCommBufMaxWindow(AMBA_DSP_PREV_DEST_PIN, DSP_VPROC_PIN_MAIN, &ResVprocSetup->postp_main_w_max, &ResVprocSetup->postp_main_h_max);
        /* Mctf ouput buffer need to have extra padding for following AVC/HEVC encode */
        /* since AVC need 16 align, so here we set padding number as 16 */
        if (ResVprocSetup->postp_main_h_max > 0U) {
            ResVprocSetup->postp_main_h_max += DSP_POSTP_MAIN_PADDING_FOR_ENC;
        }
        (void)HL_GetCommBufMaxWindow(AMBA_DSP_PREV_DEST_PIN, DSP_VPROC_PIN_PREVA, &ResVprocSetup->postp_pip_w_max, &ResVprocSetup->postp_pip_h_max);
        (void)HL_GetCommBufMaxWindow(AMBA_DSP_PREV_DEST_VOUT0, 0U/*negligible*/, &ResVprocSetup->prev_com0_w_max, &ResVprocSetup->prev_com0_h_max);
        (void)HL_GetCommBufMaxWindow(AMBA_DSP_PREV_DEST_VOUT1, 0U/*negligible*/, &ResVprocSetup->prev_com1_w_max, &ResVprocSetup->prev_com1_h_max);

        (void)HL_GetLuma12Info(&MainY12Enable, &HierY12Enable);
        HL_ResVprocMiscSecondHalf(ResVprocSetup, MainY12Enable, HierY12Enable);

        /* ExtStatusMsgQ */
        DSP_GetVprocExtMsgBufferInfo(&ULAddr);
        ResVprocSetup->msg_ext_data_max_num = AMBA_DSP_VPROC_EXT_MSG_NUM;
        (void)dsp_osal_virt2cli(ULAddr, &ResVprocSetup->msg_ext_data_base_addr);
    } else {
        ResVprocSetup->num_of_vproc_groups = 0U;
    }

    /* Test Frm */
    ResVprocSetup->max_c2y_testframes = pResource->TestFrmNumOnStage[DSP_TEST_STAGE_IDSP_0];
    ResVprocSetup->max_y2y_testframes = pResource->TestFrmNumOnStage[DSP_TEST_STAGE_IDSP_1];

    /* Effect */
    ResVprocSetup->max_ch_effect_copy_num = 0U;
    ResVprocSetup->max_ch_effect_blend_num = 0U;
    for (i=0U; i<pResource->YuvStrmNum; i++) {
        if (1U == HL_GetEffectEnableOnYuvStrm(i)) {
            UINT8 U8Val;
/* FIXME, use simple rule to limit max 2 copy/blend every YuvStrm */
            U8Val = (UINT8)(ResVprocSetup->max_ch_effect_copy_num + DSP_MAX_PP_STRM_COPY_NUMBER);
            ResVprocSetup->max_ch_effect_copy_num = U8Val;
            U8Val = (UINT8)(ResVprocSetup->max_ch_effect_blend_num + DSP_MAX_PP_STRM_BLEND_NUMBER);
            ResVprocSetup->max_ch_effect_blend_num = U8Val;
        }
    }

    return Rval;
}

static UINT32 FillSystemResourceVprocSetup(cmd_dsp_vproc_flow_max_cfg_t *ResVprocSetup)
{
    UINT32 Rval = OK;
    UINT16 MaxMainWidth = 0U, MaxMainHeight = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    if (ResVprocSetup == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        HL_GetResourcePtr(&Resource);

        if (Resource->MaxViewZoneNum == 0U) {
            Rval = DSP_ERR_0006;
        }

        if (Rval == OK) {
            (void)HL_GetSystemVprocPinMaxWindow(DSP_VPROC_PIN_MAIN, &MaxMainWidth, &MaxMainHeight);
            HL_ResVprocVin(ResVprocSetup,
                           Resource->ViewZoneActiveBit,
                           MaxMainWidth,
                           MaxMainHeight);

            if (ResVprocSetup->is_tile_mode > 0U) {
                HL_ResVprocTile(ResVprocSetup);
            } else {
                HL_ResVprocNoneTile(ResVprocSetup, MaxMainWidth);
            }

            Rval = HL_ResVprocMisc(ResVprocSetup, Resource);
            if (Rval != OK) {
                AmbaLL_LogUInt5("HL_FillSystemResourceVprocSetup HL_ResVprocMisc:%x",
                                Rval, 0U, 0U, 0U, 0U);
            }

            ResVprocSetup->idsp_hw_timeout = (UINT8)TuneDspSystemCfg.IdspTimeOut;
        }
    }
    return Rval;
}
UINT32 HL_FillSystemResourceVprocSetup(cmd_dsp_vproc_flow_max_cfg_t *ResVprocSetup)
{
    UINT32 Rval;
    UINT32 CrcHL, CrcSimilar;
    const UINT32 *pU32 = NULL;
    UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;
#ifdef CONFIG_ENABLE_DSP_MONITOR
    AMBA_DSP_MONITOR_CRC_CMPR_s CrcCmpr;
#endif

    Rval = FillSystemResourceVprocSetup(ResVprocSetup);
    if (pIsSimilarEnabled != NULL) {
        if (1U == IsSimilarEnabled()) {
            UINT32 SIM_Rval;
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_dsp_vproc_flow_max_cfg_t *SimResVprocSetup = SIM_DefCtxCmdBufPtrVpcMax;

            dsp_osal_typecast(&pU32, &ResVprocSetup);
            CrcHL = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_dsp_vproc_flow_max_cfg_t), 0U);

            SIM_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&SimResVprocSetup, &CmdBufferAddr);
            SIM_Rval = SIM_FillSystemResourceVprocSetup(SimResVprocSetup);
            if (SIM_Rval == OK) {
                dsp_osal_typecast(&pU32, &SimResVprocSetup);
                CrcSimilar = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_dsp_vproc_flow_max_cfg_t), 0U);
#ifdef CONFIG_ENABLE_DSP_MONITOR
                CrcCmpr.ModuleId = AMBA_SSP_PRINT_MODULE_ID;
                CrcCmpr.Crc0 = CrcHL;
                CrcCmpr.Crc1 = CrcSimilar;
                CrcCmpr.UUID = __LINE__;
                SIM_Rval = dsp_mon_crc_cmpr(&CrcCmpr);
                if (SIM_Rval != OK) {
                    AmbaLL_LogUInt5("dsp_mon_crc_cmpr return 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
                }
#else
                if (CrcSimilar != CrcHL) {
                    AmbaLL_LogUInt5("[ERR] Similar CRC mismatch @ %d HL 0x%x Similar 0x%x", __LINE__, CrcHL, CrcSimilar, 0U, 0U);
                }
#endif
            } else {
                AmbaLL_LogUInt5("[ERR] Similar function returned NG 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
            }
            SIM_RelCmdBuffer(CmdBufferId);
        }
    }

    return Rval;
}

static inline void HL_FillResVinVirtualInternal(cmd_dsp_vin_flow_max_cfg_t *ResVinSetup,
                                                UINT8 *pMemIdx,
                                                const UINT16 VinId,
                                                const UINT16 ViewZoneId,
                                                const UINT16 MaxVirtVinWidth,
                                                const UINT16 MaxVirtVinHeight)
{
    UINT8 MemIdx = *pMemIdx;
    UINT8 MaxHdrExpNumMiunsOne = 0U, LinearCeEnable = 0U;
    UINT16 MaxHdrBlendHeight = 0U;
    UINT16 RawWidth = 0U, Mantissa = 0U, BlkSz = 0U;
    CTX_VIN_INFO_s VinInfo = {0};
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT8 i;

    HL_GetVinInfo(HL_MTX_OPT_ALL, VinId, &VinInfo);
    HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);

    if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM) {
        (void)HL_GetSystemHdrSetting(&MaxHdrExpNumMiunsOne, &LinearCeEnable, &MaxHdrBlendHeight);
        ResVinSetup->is_contrast_enhance_out_enabled = ((MaxHdrExpNumMiunsOne > 0U) || (LinearCeEnable > 0U))? 1U: 0U;

        ResVinSetup->is_sensor_raw_out_enabled = 1U;
        if (VinInfo.CfaCompressed > 0U) {
            ResVinSetup->raw_width_is_byte_width = 1U;
            (void)HL_GetCmprRawBufInfo(MaxVirtVinWidth,
                                       VinInfo.CfaCompressed,
                                       VinInfo.CfaCompact,
                                       &RawWidth,
                                       &ResVinSetup->max_raw_cap_width,
                                       &Mantissa, &BlkSz);
        } else {
            ResVinSetup->raw_width_is_byte_width = 0U;
            ResVinSetup->max_raw_cap_width = MaxVirtVinWidth;
        }

        if (MaxHdrExpNumMiunsOne > 0U) {
            ResVinSetup->max_raw_cap_height = VinInfo.CapWindow[0U].Height;
        } else {
            ResVinSetup->max_raw_cap_height = MaxVirtVinHeight;
        }
        ResVinSetup->max_raw_cap_dbuf_num = CV2X_MAX_RAW_FRM_NUM;

#ifdef SUPPORT_DSP_VIN_YUV_ENC_FBP_CTRL
        ResVinSetup->vin_yuv_enc_fbp_init = 0U;
#endif
        if (ResVinSetup->is_contrast_enhance_out_enabled > 0U) {
            ResVinSetup->max_contrast_enhance_out_dbuf_num = CV2X_MAX_RAW_FRM_NUM;

            /* New ucode prepare CE pitch to be same logic as Raw */
            if (VinInfo.CfaCompressed > 0U) {
                (void)HL_GetCmprRawBufInfo((UINT16)(MaxVirtVinWidth/4U),
                                           VinInfo.CfaCompressed,
                                           VinInfo.CfaCompact,
                                           &RawWidth,
                                           &ResVinSetup->max_contrast_enhance_out_width,
                                           &Mantissa, &BlkSz);
            } else {
                ResVinSetup->max_contrast_enhance_out_width = (UINT16)(MaxVirtVinWidth/4U);
            }
            if (MaxHdrExpNumMiunsOne > 0U) {
                ResVinSetup->max_contrast_enhance_out_height = VinInfo.CapWindow[0U].Height;
            } else {
                ResVinSetup->max_contrast_enhance_out_height = MaxVirtVinHeight;
            }
        }
    } else if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_422) ||
               (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_420)) {
        ResVinSetup->is_sensor_raw_out_enabled = 0U;
        ResVinSetup->is_contrast_enhance_out_enabled = 0U;
        ResVinSetup->is_sensor_raw_out_enabled = 0U;
        ResVinSetup->is_yuv422_out_enabled = 1U;

        ResVinSetup->max_yuv422_out_dbuf_num = CV2X_MAX_RAW_FRM_NUM;
        ResVinSetup->max_yuv422_out_width = MaxVirtVinWidth;
        ResVinSetup->max_yuv422_out_height = MaxVirtVinHeight;

#ifdef SUPPORT_DSP_VIN_YUV_ENC_FBP_CTRL
        ResVinSetup->vin_yuv_enc_fbp_init = 0U;
#endif
    } else {
        //DO NOTHING
    }

    for (i=0U; i<DSP_VIN_EXT_MEM_TYPE_NUM; i++) {
        ResVinSetup->ext_mem_cfg[MemIdx].memory_type = i;
        ResVinSetup->ext_mem_cfg[MemIdx].max_daddr_slot = DEFAULT_EXT_MEM_DADDR_NUM; //uCode default 64, means number of address
        ResVinSetup->ext_mem_cfg[MemIdx].pool_buf_num = DEFAULT_EXT_MEM_POOL_VIN_BUF_NUM; //uCode default 7, vin logic frame buffer
        MemIdx++;
    }
    ResVinSetup->ext_mem_cfg_num = MemIdx;

    *pMemIdx = MemIdx;
}

static inline void HL_FillResVinVirtualTimeLapse(cmd_dsp_vin_flow_max_cfg_t *ResVinSetup, UINT8 *pMemIdx,
                                                 const CTX_RESOURCE_INFO_s *pResource, UINT8 *pEnableExtMemMask,
                                                 const UINT16 VinId, UINT16 *pViewZoneId)
{
    UINT16 i, j, ExitLoop;
    UINT16 ViewZoneVinId = AMBA_DSP_MAX_VIN_NUM;

    ExitLoop = 0U;
    for (i = 0U; i < pResource->MaxTimeLapseNum; i++) {
        CTX_TIMELAPSE_INFO_s TimeLapseInfo = {0};

        HL_GetTimeLapseInfo(HL_MTX_OPT_ALL, i, &TimeLapseInfo);

        if ((TimeLapseInfo.VirtVinId != DSP_VIRT_VIN_IDX_INVALID) &&
            (TimeLapseInfo.VirtVinId == ((UINT16)ResVinSetup->vin_id - AMBA_DSP_MAX_VIN_NUM))) {
            ResVinSetup->is_yuv422_out_enabled = 1U;
            ResVinSetup->max_yuv422_out_dbuf_num = 1U;
            (void)HL_GetEncMaxTimeLapseWindow(&ResVinSetup->max_yuv422_out_width, &ResVinSetup->max_yuv422_out_height);

//FIXME, Need all memory type?
            for (j=0U; j<DSP_VIN_EXT_MEM_TYPE_NUM; j++) {
                DSP_SetU8Bit(pEnableExtMemMask, j);
            }

#ifdef SUPPORT_DSP_VIN_YUV_ENC_FBP_CTRL
            /*
             * vin_yuv_enc_fbp_init will allocate resource for virtual vin yuv input from ext_mem at vin_flow_max stage
             * Need to allocate resource (vin_yuv_enc_fbp_disable=0) when send ext_mem after vproc
             * */
            ResVinSetup->vin_yuv_enc_fbp_init = 1U;
#endif
            ExitLoop = 1U;
            break;
        }
    }

    // VirtualVin requested From User
    if (ExitLoop == 0U) {
        for (i = 0U; i < pResource->MaxViewZoneNum; i++) {
            CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

            HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
            (void)HL_GetViewZoneVinId((UINT8)i, &ViewZoneVinId);
            if (((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM) ||
                 (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_422) ||
                 (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_420)) &&
                 (ViewZoneVinId == VinId)) {
                *pViewZoneId = i;
                ExitLoop = 1U;
                break;
            }
        }

        if (ExitLoop == 1U) {
            UINT16 MaxVirtVinWidth = pResource->MaxVirtVinWidth[ViewZoneVinId - AMBA_DSP_MAX_VIN_NUM];
            UINT16 MaxVirtVinHeight = pResource->MaxVirtVinHeight[ViewZoneVinId - AMBA_DSP_MAX_VIN_NUM];

            HL_FillResVinVirtualInternal(ResVinSetup,
                                         pMemIdx,
                                         VinId,
                                         *pViewZoneId,
                                         MaxVirtVinWidth,
                                         MaxVirtVinHeight);
        }
    }
}

static inline void HL_FillResVinVirtualEnd(cmd_dsp_vin_flow_max_cfg_t *ResVinSetup, const CTX_DSP_INST_INFO_s *pDspInstInfo, const UINT16 VinId)
{
    if ((pDspInstInfo->DecSharedVirtVinId != DSP_VIRT_VIN_IDX_INVALID) &&
        ((pDspInstInfo->DecSharedVirtVinId + AMBA_DSP_MAX_VIN_NUM) == VinId)) {
        ResVinSetup->max_fov_num = (UINT16)HL_GetSystemVinMaxViewZoneNum(VinId);
    } else {
        ResVinSetup->max_fov_num = 1U;
    }
}

static inline void HL_FillResVinVirtual(cmd_dsp_vin_flow_max_cfg_t *ResVinSetup,
                                        UINT8 *pMemIdx,
                                        const UINT16 VinId)
{
    UINT32 Rval;
    UINT8 MemIdx = *pMemIdx;
    UINT8 i = 0;
    UINT16 ViewZoneId = 0U;
    CTX_STILL_INFO_s CtxStlInfo = {0};
    CTX_VID_DEC_INFO_s VidDecInfo = {0};
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT16 DecIdx = 0U;
    CTX_DSP_INST_INFO_s DspInstInfo = {0};
    UINT8 EnableExtMemMask = 0U;
    UINT8 MaxBufNum = 0U;

    HL_GetResourcePtr(&Resource);
    HL_GetStlInfo(HL_MTX_OPT_ALL, &CtxStlInfo);
    HL_GetDspInstance(HL_MTX_OPT_ALL, &DspInstInfo);

    //Find Dec instance idx by pairing VirtVinId with input VinId
    (void)HL_GetDecIdxFromVinId((UINT16)ResVinSetup->vin_id, &DecIdx);
    HL_GetVidDecInfo(HL_MTX_OPT_ALL, DecIdx, &VidDecInfo);
    if ((CtxStlInfo.RawInVirtVinId != DSP_VIRT_VIN_IDX_INVALID) &&
        (CtxStlInfo.RawInVirtVinId == ((UINT16)ResVinSetup->vin_id - AMBA_DSP_MAX_VIN_NUM))) {
        ResVinSetup->is_sensor_raw_out_enabled = 1U;
        ResVinSetup->max_raw_cap_dbuf_num = CV2X_MAX_RAW_FRM_NUM;
        ResVinSetup->max_raw_cap_width = Resource->MaxStlRawInputWidth;
        ResVinSetup->max_raw_cap_height = Resource->MaxStlRawInputHeight;
        ResVinSetup->raw_width_is_byte_width = 0U;

        ResVinSetup->is_contrast_enhance_out_enabled = 1U;
        ResVinSetup->max_contrast_enhance_out_dbuf_num = CV2X_MAX_RAW_FRM_NUM;
        ResVinSetup->max_contrast_enhance_out_width = Resource->MaxStlRawInputWidth / 4U;
        ResVinSetup->max_contrast_enhance_out_height = Resource->MaxStlRawInputHeight;
        /*
         * vin_yuv_enc_fbp_init will allocate resource for virtual vin yuv input from ext_mem at vin_flow_max stage
         * No need to reset resource again(vin_yuv_enc_fbp_disable=0) when send ext_mem after vproc
         * */
#ifdef SUPPORT_DSP_VIN_YUV_ENC_FBP_CTRL
        ResVinSetup->vin_yuv_enc_fbp_init = 0U;
#endif

//FIXME, Need all memory type?
        for (i = 0U; i<DSP_VIN_EXT_MEM_TYPE_NUM; i++) {
            DSP_SetU8Bit(&EnableExtMemMask, i);
        }
    } else if ((CtxStlInfo.YuvInVirtVinId != DSP_VIRT_VIN_IDX_INVALID) &&
               (CtxStlInfo.YuvInVirtVinId == ((UINT16)ResVinSetup->vin_id - AMBA_DSP_MAX_VIN_NUM))) {
        ResVinSetup->is_yuv422_out_enabled = 1U;
        ResVinSetup->max_yuv422_out_dbuf_num = 1U;
        ResVinSetup->max_yuv422_out_width = Resource->MaxStlYuvInputWidth;
        ResVinSetup->max_yuv422_out_height = Resource->MaxStlYuvInputHeight;
        /*
         * vin_yuv_enc_fbp_init will allocate resource for virtual vin yuv input from ext_mem at vin_flow_max stage
         * No need to reset resource again(vin_yuv_enc_fbp_disable=0) when send ext_mem after vproc
         * */
#ifdef SUPPORT_DSP_VIN_YUV_ENC_FBP_CTRL
        ResVinSetup->vin_yuv_enc_fbp_init = 1U;
#endif

//FIXME, Need all memory type?
        for (i = 0U; i<DSP_VIN_EXT_MEM_TYPE_NUM; i++) {
            DSP_SetU8Bit(&EnableExtMemMask, i);
        }
    } else if ((CtxStlInfo.YuvEncVirtVinId != DSP_VIRT_VIN_IDX_INVALID) &&
               (CtxStlInfo.YuvEncVirtVinId == ((UINT16)ResVinSetup->vin_id - AMBA_DSP_MAX_VIN_NUM))) {
        ResVinSetup->is_yuv422_out_enabled = 1U;
        ResVinSetup->max_yuv422_out_dbuf_num = 1U;
        ResVinSetup->max_yuv422_out_width = Resource->MaxStlYuvEncWidth;
        ResVinSetup->max_yuv422_out_height = Resource->MaxStlYuvEncHeight;
        /*
         * vin_yuv_enc_fbp_init will allocate resource for virtual vin yuv input from ext_mem at vin_flow_max stage
         * Need to allocate resource (vin_yuv_enc_fbp_disable=0) when send ext_mem after vproc
         * */
#ifdef SUPPORT_DSP_VIN_YUV_ENC_FBP_CTRL
        ResVinSetup->vin_yuv_enc_fbp_init = 1U;
#endif

//FIXME, Need all memory type?
        for (i = 0U; i<DSP_VIN_EXT_MEM_TYPE_NUM; i++) {
            DSP_SetU8Bit(&EnableExtMemMask, i);
        }
    } else if ((VidDecInfo.YuvInVirtVinId != DSP_VIRT_VIN_IDX_INVALID) &&
               (VidDecInfo.YuvInVirtVinId == ((UINT16)ResVinSetup->vin_id - AMBA_DSP_MAX_VIN_NUM))) {
        ResVinSetup->is_yuv422_out_enabled = 1U;
        /* one for pic_info_cache, another for normal use */
        ResVinSetup->max_yuv422_out_dbuf_num = 2U;
//FIXME, to get real formula
#define DEC_PADDED_SIZE (16U)
        ResVinSetup->max_yuv422_out_width = Resource->DecMaxFrameWidth + DEC_PADDED_SIZE;
        /* dec2vproc vin buffer need alignment for easy of use
         * if we use 1080 height for first vin_ext_mem,
         * ucode we use 1080 to create memory map,
         * if pic_info_cache=1,
         * we cannot re-create memory map by vin_ext_mem.allocation_mode=0,
         * so we use 1088 for the virtaul vin
         *  */
        ResVinSetup->max_yuv422_out_height = ALIGN_NUM16(Resource->DecMaxFrameHeight, (UINT16)DSP_BUF_ALIGNMENT) + DEC_PADDED_SIZE;
        /*
         * This is virtual vin feed into vproc case
         * vin_yuv_enc_fbp_init no need to allocate resource at vin_flow_max stage
         * No need to allocate resource (vin_yuv_enc_fbp_disable=1) when send ext_mem after vproc
         * */
#ifdef SUPPORT_DSP_VIN_YUV_ENC_FBP_CTRL
        ResVinSetup->vin_yuv_enc_fbp_init = 0U;
#endif

//FIXME, Need all memory type?
        for (i = 0U; i<DSP_VIN_EXT_MEM_TYPE_NUM; i++) {
            DSP_SetU8Bit(&EnableExtMemMask, i);
        }
    } else if ((DspInstInfo.TestEncVirtVinId != DSP_VIRT_VIN_IDX_INVALID) &&
               (DspInstInfo.TestEncVirtVinId == ((UINT16)ResVinSetup->vin_id - AMBA_DSP_MAX_VIN_NUM))) {
        test_binary_header_t EncHdr = {0};

        ResVinSetup->is_yuv422_out_enabled = 1U;
        ResVinSetup->max_yuv422_out_dbuf_num = 1U;
        Rval = HL_GetTestEncHeader(&EncHdr, Resource);
        if (Rval == OK) {
            ResVinSetup->max_yuv422_out_width = (UINT16)EncHdr.luma0_w;
            ResVinSetup->max_yuv422_out_height = (UINT16)EncHdr.luma0_h;
        }

        /*
         * vin_yuv_enc_fbp_init will allocate resource for virtual vin yuv input from ext_mem at vin_flow_max stage
         * Need to allocate resource (vin_yuv_enc_fbp_disable=0) when send ext_mem after vproc
         * */
#ifdef SUPPORT_DSP_VIN_YUV_ENC_FBP_CTRL
        ResVinSetup->vin_yuv_enc_fbp_init = 1U;
#endif
        DSP_SetU8Bit(&EnableExtMemMask, DSP_VIN_EXT_MEM_TYPE_YUV422);
        DSP_SetU8Bit(&EnableExtMemMask, DSP_VIN_EXT_MEM_TYPE_ME);
        MaxBufNum = 3U; //need at least 3 frm to complete Encode
    } else {
        // VirtualVin for TimeLapse
        HL_FillResVinVirtualTimeLapse(ResVinSetup, pMemIdx, Resource, &EnableExtMemMask, VinId, &ViewZoneId);
    }

    if (EnableExtMemMask > 0U) {
        for (i = 0U; i<DSP_VIN_EXT_MEM_TYPE_NUM; i++) {
            if (1U == DSP_GetU8Bit(EnableExtMemMask, i, 1U)) {
                ResVinSetup->ext_mem_cfg[MemIdx].memory_type = i;
                ResVinSetup->ext_mem_cfg[MemIdx].max_daddr_slot = DEFAULT_EXT_MEM_DADDR_NUM;
                MaxBufNum = ((MaxBufNum > 0U) ? MaxBufNum: DEFAULT_EXT_MEM_POOL_VIN_BUF_NUM);
                ResVinSetup->ext_mem_cfg[MemIdx].pool_buf_num = MaxBufNum;
                MemIdx++;
            }
        }
        ResVinSetup->ext_mem_cfg_num = MemIdx;
    }

    HL_FillResVinVirtualEnd(ResVinSetup, &DspInstInfo, VinId);

    *pMemIdx = MemIdx;
}

static inline void HL_FillResVinPhysicalRaw(cmd_dsp_vin_flow_max_cfg_t *ResVinSetup,
                                            const CTX_VIN_INFO_s *pVinInfo,
                                            const CTX_RESOURCE_INFO_s *pResource,
                                            const UINT16 PhyVinId,
                                            const UINT16 YuyvEnable,
                                            const UINT8 MaxHdrExpNumMiunsOne)
{
    UINT16 YuyvFactor = 1U;
    UINT16 RawWidth = 0U, RawPitch = 0U, Mantissa = 0U, BlkSz = 0U;
    UINT8 IsStlRawUncmpr = 0U, IsLiveviewUncmpr;
    UINT16 VinWidth, StlVinPitch;

    if (pResource->MaxStlVinWidth[PhyVinId][0U] > 0U) {
        IsStlRawUncmpr = (1U == DSP_GetU8Bit(pResource->MaxProcessFormat, 4U/*UnCmprRaw*/, 1U))? (UINT8)1U: (UINT8)0U;
    }
    IsLiveviewUncmpr = ((pVinInfo->CfaCompact == 0U) && (pVinInfo->CfaCompressed == 0U))? 1U: 0U;

    if (YuyvEnable > 0U) {
        YuyvFactor = 2U;//YUYV occupied 2 times(U/V) of original pixel per line under uncompressed mode
    }

    if ((IsStlRawUncmpr == 1U) && (IsLiveviewUncmpr == 1U)) {
        ResVinSetup->raw_width_is_byte_width = 0U;
        VinWidth = (UINT16)(pResource->MaxVinVirtChanWidth[PhyVinId][0U]*YuyvFactor);
        VinWidth = MAX2_16(VinWidth, pResource->MaxStlVinWidth[PhyVinId][0U]);
        ResVinSetup->max_raw_cap_width = VinWidth;
    } else {
        ResVinSetup->raw_width_is_byte_width = 1U;
        VinWidth = (UINT16)(pResource->MaxVinVirtChanWidth[PhyVinId][0U]*YuyvFactor);
        (void)HL_GetCmprRawBufInfo(VinWidth,
                                   pVinInfo->CfaCompressed,
                                   pVinInfo->CfaCompact,
                                   &RawWidth,
                                   &RawPitch,
                                   &Mantissa, &BlkSz);
        StlVinPitch = (IsStlRawUncmpr==1U)? (pResource->MaxStlVinWidth[PhyVinId][0U]<<1U): pResource->MaxStlVinWidth[PhyVinId][0U];
        ResVinSetup->max_raw_cap_width = MAX2_16(RawPitch, StlVinPitch);
    }

    if (MaxHdrExpNumMiunsOne > 0U) {
        ResVinSetup->max_raw_cap_height = pVinInfo->CapWindow[0U].Height;
    } else {
        ResVinSetup->max_raw_cap_height = pResource->MaxVinVirtChanHeight[PhyVinId][0U];
    }
    /* Consider still raw */
    ResVinSetup->max_raw_cap_height = MAX2_16(ResVinSetup->max_raw_cap_height, pResource->MaxStlVinHeight[PhyVinId][0U]);

    /* Per Max, 4:safe, 3:reasonable, 2:dangerous */
    if (pResource->RawBufNum[PhyVinId] > 0U) {
        ResVinSetup->max_raw_cap_dbuf_num = pResource->RawBufNum[PhyVinId];
    } else {
        ResVinSetup->max_raw_cap_dbuf_num = CV2X_MAX_RAW_FRM_NUM;
    }

    /* Embedded data */
    if ((pResource->MaxVinEmbdDataWidth[PhyVinId] > 0U) &&
        (pResource->MaxVinEmbdDataHeight[PhyVinId] > 0U)) {
        ResVinSetup->is_aux_out_enabled = (UINT8)1U;
        ResVinSetup->max_aux_out_dbuf_num = (UINT8)ResVinSetup->max_raw_cap_dbuf_num;
        //should <<1 under raw compressed
        ResVinSetup->max_aux_out_width = pResource->MaxVinEmbdDataWidth[PhyVinId] << 1U;
        ResVinSetup->max_aux_out_height = pResource->MaxVinEmbdDataHeight[PhyVinId];
    }
}

static inline void HL_FillResVinPhysicalCe(cmd_dsp_vin_flow_max_cfg_t *ResVinSetup,
                                           const CTX_VIN_INFO_s *pVinInfo,
                                           const CTX_RESOURCE_INFO_s *pResource,
                                           const UINT16 PhyVinId,
                                           const UINT8 MaxHdrExpNumMiunsOne)
{
    UINT16 VinWidth = 0U, Mantissa = 0U, BlkSz = 0U;
    UINT16 CeWidth = 0U, CePitch = 0U, CeFactor = 4U, StlCePitch;
    UINT8 IsStlRawUncmpr = 0U, IsLiveviewUncmpr;

    if (pResource->MaxStlVinWidth[PhyVinId][0U] > 0U) {
        IsStlRawUncmpr = (1U == DSP_GetU8Bit(pResource->MaxProcessFormat, 4U/*UnCmprRaw*/, 1U))? (UINT8)1U: (UINT8)0U;
    }

    IsLiveviewUncmpr = ((pVinInfo->CfaCompact == 0U) && (pVinInfo->CfaCompressed == 0U))? 1U: 0U;

    /** !!! CV2X use Fixed 4X downSample !!! */
    /* New ucode prepare CE pitch to be same logic as Raw */
    if ((IsStlRawUncmpr == 1U)&& (IsLiveviewUncmpr == 1U)) {
        VinWidth = MAX2_16(pResource->MaxVinVirtChanWidth[PhyVinId][0U], pResource->MaxStlVinWidth[PhyVinId][0U]);
        ResVinSetup->max_contrast_enhance_out_width = VinWidth/CeFactor;
    } else {
        // In byte unit
        VinWidth = pResource->MaxVinVirtChanWidth[PhyVinId][0U];
        (void)HL_GetCmprRawBufInfo(VinWidth/CeFactor,
                                   pVinInfo->CfaCompressed,
                                   pVinInfo->CfaCompact,
                                   &CeWidth,
                                   &CePitch,
                                   &Mantissa, &BlkSz);
        StlCePitch = (IsStlRawUncmpr==1U)? (pResource->MaxStlVinWidth[PhyVinId][0U]<<1U): pResource->MaxStlVinWidth[PhyVinId][0U];
        StlCePitch = StlCePitch/CeFactor;
        ResVinSetup->max_contrast_enhance_out_width = MAX2_16(CePitch, StlCePitch);
    }

    if (MaxHdrExpNumMiunsOne > 0U) {
        ResVinSetup->max_contrast_enhance_out_height = pVinInfo->CapWindow[0U].Height;
    } else {
        ResVinSetup->max_contrast_enhance_out_height = pResource->MaxVinVirtChanHeight[PhyVinId][0U];
    }
    /* Consider still raw */
    ResVinSetup->max_contrast_enhance_out_height = MAX2_16(ResVinSetup->max_contrast_enhance_out_height, pResource->MaxStlVinHeight[PhyVinId][0U]);


    if (pResource->RawBufNum[PhyVinId] > 0U) {
        ResVinSetup->max_contrast_enhance_out_dbuf_num = pResource->RawBufNum[PhyVinId];
    } else {
        ResVinSetup->max_contrast_enhance_out_dbuf_num = CV2X_MAX_RAW_FRM_NUM;
    }

}

static inline UINT32 HL_FillResVinPhysical(cmd_dsp_vin_flow_max_cfg_t *ResVinSetup,
                                           UINT8 *pMemIdx,
                                           const UINT16 VinId,
                                           const UINT16 PhyVinId)
{
    UINT8 MemIdx = *pMemIdx;
    UINT8 MaxHdrExpNumMiunsOne = 0U, LinearCeEnable = 0U, IsStlCeEnabled;
    UINT16 MaxHdrBlendHeight = 0U, Idx;
    UINT16 ViewZoneId = 0U, YuyvEnable;
    UINT32 Rval = OK;
    CTX_VIN_INFO_s VinInfo = {0};
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT8 i;

    HL_GetResourcePtr(&Resource);
    HL_GetVinInfo(HL_MTX_OPT_ALL, PhyVinId, &VinInfo);
    (void)HL_GetVinMipiYuyvInfo(VinId, &VinInfo, &YuyvEnable);
    for (Idx = 0U; Idx < Resource->MaxViewZoneNum; Idx++) {
        if (0U == DSP_GetBit(Resource->ViewZoneActiveBit, Idx, 1U)) {
            continue;
        }
        HL_GetViewZoneInfoPtr(Idx, &ViewZoneInfo);

        if ((ViewZoneInfo->SourceVin == VinId) &&
            (ViewZoneInfo->Pipe == DSP_DRAM_PIPE_MIPI_YUV)) {
            YuyvEnable = (UINT16)1U;
            break;
        }
    }

    /** Fetch first viewzone information of this Vin */
    if (HL_FetchFirstViewZoneInfo(VinId, &ViewZoneId, &ViewZoneInfo) != 1U) {
        AmbaLL_LogUInt5("HL_FillResVinPhysical [%d]cannot find ViewZone", VinId, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0001;
    }

    /* Vin output */
    if (YuyvEnable > 0U) {
        ResVinSetup->is_sensor_raw_out_enabled = 1U;
        ResVinSetup->is_yuv422_out_enabled = 0U;
    } else {
        if ((VinInfo.InputFormat == DSP_VIN_INPUT_YUV_422_INTLC) || \
            (VinInfo.InputFormat == DSP_VIN_INPUT_YUV_422_PROG) || \
            (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_422) ||
            (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_420)) {
            ResVinSetup->is_sensor_raw_out_enabled = 0U;
            ResVinSetup->is_yuv422_out_enabled = 1U;
        } else {
            ResVinSetup->is_sensor_raw_out_enabled = 1U;
            ResVinSetup->is_yuv422_out_enabled = 0U;
        }
    }

    (void)HL_GetSystemHdrSettingOnVin(PhyVinId, &MaxHdrExpNumMiunsOne, &LinearCeEnable, &MaxHdrBlendHeight);
    IsStlCeEnabled = (1U == DSP_GetU8Bit(Resource->MaxProcessFormat, 2U/*CE*/, 1U))? (UINT8)1U: (UINT8)0U;
    ResVinSetup->is_contrast_enhance_out_enabled = ((MaxHdrExpNumMiunsOne > 0U) || (LinearCeEnable > 0U) || (IsStlCeEnabled == 1U))? 1U: 0U;

    if (ResVinSetup->is_sensor_raw_out_enabled > 0U) {
        HL_FillResVinPhysicalRaw(ResVinSetup,
                                 &VinInfo,
                                 Resource,
                                 PhyVinId,
                                 YuyvEnable,
                                 MaxHdrExpNumMiunsOne);
    }

    if (ResVinSetup->is_yuv422_out_enabled > 0U) {
        if (Resource->RawBufNum[PhyVinId] > 0U) {
            ResVinSetup->max_yuv422_out_dbuf_num = Resource->RawBufNum[PhyVinId];
        } else {
            ResVinSetup->max_yuv422_out_dbuf_num = CV2X_MAX_RAW_FRM_NUM;
        }
        ResVinSetup->max_yuv422_out_width = Resource->MaxVinVirtChanWidth[PhyVinId][0U];
        ResVinSetup->max_yuv422_out_height = Resource->MaxVinVirtChanHeight[PhyVinId][0U];
    }

    if (ResVinSetup->is_contrast_enhance_out_enabled > 0U) {
        HL_FillResVinPhysicalCe(ResVinSetup,
                                &VinInfo,
                                Resource,
                                PhyVinId,
                                MaxHdrExpNumMiunsOne);
    }

    /*
     * Per ChenHan, using DMA window mode to have better performance
     * the cost is smem usage, but shall be slight
     */
    ResVinSetup->vin_raw_smem_win_out = 1U;
    ResVinSetup->max_vin_outstanding_dram_xfers = 0U; //ucode internal value

    ResVinSetup->max_fov_num = (UINT16)HL_GetSystemVinMaxViewZoneNum(VinId);

    /* Set min number */
    for (i = 0U; i < DSP_VIN_EXT_MEM_TYPE_NUM; i++) {
        ResVinSetup->ext_mem_cfg[MemIdx].memory_type = i;
        ResVinSetup->ext_mem_cfg[MemIdx].max_daddr_slot = DEFAULT_EXT_MEM_DADDR_NUM;
        ResVinSetup->ext_mem_cfg[MemIdx].pool_buf_num = DEFAULT_EXT_MEM_POOL_VIN_BUF_NUM;
        MemIdx++;
    }
    ResVinSetup->ext_mem_cfg_num = MemIdx;

    *pMemIdx = MemIdx;

    return Rval;
}

static UINT32 FillSystemResourceVinSetup(cmd_dsp_vin_flow_max_cfg_t *ResVinSetup, UINT16 VinId)
{
    UINT8 MemIdx = 0U, IsVirtVin = 0U;
    UINT16 PhyVinId = DSP_VIRT_VIN_IDX_INVALID;
    UINT32 Rval = OK;

    if (ResVinSetup == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (VinId >= DSP_VIN_MAX_NUM) {
            Rval = DSP_ERR_0001;
        } else {
            ResVinSetup->vin_id = (UINT8)VinId;

            (void)HL_IsVirtualVinIdx(VinId, &IsVirtVin);
            if (IsVirtVin == 0U) {
                PhyVinId = VinId;
            } else {
                (void)HL_GetTimeDivisionVirtVinInfo(VinId, &PhyVinId);
            }

            if ((IsVirtVin > 0U) &&
                (PhyVinId == DSP_VIRT_VIN_IDX_INVALID)) {
                HL_FillResVinVirtual(ResVinSetup,
                                     &MemIdx,
                                     VinId);
            } else if (PhyVinId < AMBA_DSP_MAX_VIN_NUM) {
                Rval = HL_FillResVinPhysical(ResVinSetup,
                                             &MemIdx,
                                             VinId,
                                             PhyVinId);
            } else {
                //TBD
            }
        }
    }
    return Rval;
}
UINT32 HL_FillSystemResourceVinSetup(cmd_dsp_vin_flow_max_cfg_t *ResVinSetup, UINT16 VinId)
{
    UINT32 Rval;
    UINT32 CrcHL, CrcSimilar;
    const UINT32 *pU32 = NULL;
    UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;
#ifdef CONFIG_ENABLE_DSP_MONITOR
    AMBA_DSP_MONITOR_CRC_CMPR_s CrcCmpr;
#endif

    Rval = FillSystemResourceVinSetup(ResVinSetup, VinId);
    if (pIsSimilarEnabled != NULL) {
        if (1U == IsSimilarEnabled()) {
            UINT32 SIM_Rval;
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_dsp_vin_flow_max_cfg_t *SimResVinSetup = SIM_DefCtxCmdBufPtrVinMax;

            dsp_osal_typecast(&pU32, &ResVinSetup);
            CrcHL = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_dsp_vin_flow_max_cfg_t), 0U);

            SIM_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&SimResVinSetup, &CmdBufferAddr);
            SIM_Rval = SIM_FillSystemResourceVinSetup(SimResVinSetup, VinId);
            if (SIM_Rval == OK) {
                dsp_osal_typecast(&pU32, &SimResVinSetup);
                CrcSimilar = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_dsp_vin_flow_max_cfg_t), 0U);
#ifdef CONFIG_ENABLE_DSP_MONITOR
                CrcCmpr.ModuleId = AMBA_SSP_PRINT_MODULE_ID;
                CrcCmpr.Crc0 = CrcHL;
                CrcCmpr.Crc1 = CrcSimilar;
                CrcCmpr.UUID = __LINE__;
                SIM_Rval = dsp_mon_crc_cmpr(&CrcCmpr);
                if (SIM_Rval != OK) {
                    AmbaLL_LogUInt5("dsp_mon_crc_cmpr return 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
                }
#else
                if (CrcSimilar != CrcHL) {
                    AmbaLL_LogUInt5("[ERR] Similar CRC mismatch @ %d HL 0x%x Similar 0x%x", __LINE__, CrcHL, CrcSimilar, 0U, 0U);
                }
#endif
            } else {
                AmbaLL_LogUInt5("[ERR] Similar function returned NG 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
            }
            SIM_RelCmdBuffer(CmdBufferId);
        }
    }

    return Rval;
}

static inline void HL_FillFpBindSetupYuvConv(cmd_binding_cfg_t *FpBindCfg,
                                             const UINT8 BindType,
                                             const UINT16 SrcId)
{
    UINT8 NumBind = 0U;
    UINT16 ViewZoneId = SrcId, VinId;
    UINT32 LocalViewZoneIdx;

    (void)HL_GetViewZoneVinId(ViewZoneId, &VinId);
    LocalViewZoneIdx = HL_GetViewZoneLocalIndexOnVin(ViewZoneId);

    FpBindCfg->bindings[NumBind].bind_type = BindType;
    FpBindCfg->bindings[NumBind].bind_msg = (UINT8)FP_BIND_DATA;
    FpBindCfg->bindings[NumBind].src_fp_type = (UINT8)FP_TYPE_VIN;
    FpBindCfg->bindings[NumBind].src_fp_id = (VinId == VIN_IDX_INVALID)? (UINT8)0U: (UINT8)VinId;
    FpBindCfg->bindings[NumBind].src_ch_id = (UINT8)LocalViewZoneIdx;
    FpBindCfg->bindings[NumBind].src_str_id = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_fp_type = (UINT8)FP_TYPE_VPROC_DV;
    FpBindCfg->bindings[NumBind].dst_fp_id = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_ch_id = (UINT8)ViewZoneId;
    FpBindCfg->bindings[NumBind].dst_str_id = (UINT8)0U;
    NumBind++;

    FpBindCfg->num_of_bindings = NumBind;
}

static inline void HL_FillFpBindSetupVidEnc(cmd_binding_cfg_t *FpBindCfg,
                                             const UINT8 BindPurpose,
                                             const UINT8 BindType,
                                             const UINT16 SrcId,
                                             const UINT16 SrcPinId,
                                             const UINT16 StrmId)
{
    UINT8 NumBind = 0U;
    UINT16 ViewZoneId = SrcId, VinId = SrcId; // SrcId = VinId when BIND_VIDEO_INTERVAL_ENCODE

    FpBindCfg->bindings[NumBind].bind_type = BindType;
    FpBindCfg->bindings[NumBind].bind_msg = (UINT8)FP_BIND_DATA;

    FpBindCfg->bindings[NumBind].src_fp_type = (BIND_VIDEO_ENCODE == BindPurpose)? (UINT8)FP_TYPE_VPROC_DV: (UINT8)FP_TYPE_VIN;
    FpBindCfg->bindings[NumBind].src_fp_id = (BIND_VIDEO_INTERVAL_ENCODE == BindPurpose)? (UINT8)VinId: (UINT8)0U; //VprocFp Index

    if (BIND_VIDEO_ENCODE == BindPurpose) {
        FpBindCfg->bindings[NumBind].src_ch_id = (UINT8)ViewZoneId;
    } else if (BIND_VIDEO_INTERVAL_ENCODE == BindPurpose) {
        FpBindCfg->bindings[NumBind].src_ch_id = (UINT8)0U;
    } else {
        UINT8 SrcChId = (UINT8)StrmId + 1U;
        FpBindCfg->bindings[NumBind].src_ch_id = SrcChId;
    }

    if (BIND_VIDEO_ENCODE == BindPurpose) {
        FpBindCfg->bindings[NumBind].src_str_id = (UINT8)SrcPinId;
    } else {
        FpBindCfg->bindings[NumBind].src_str_id = 0U;
    }

    FpBindCfg->bindings[NumBind].dst_fp_type = (UINT8)FP_TYPE_VENC;
    FpBindCfg->bindings[NumBind].dst_fp_id = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_ch_id = (UINT8)StrmId; //TBD LowDelay
    FpBindCfg->bindings[NumBind].dst_str_id = (UINT8)0U;
    NumBind++;

    FpBindCfg->bindings[NumBind].bind_type = BindType;
    FpBindCfg->bindings[NumBind].bind_msg = (UINT8)FP_BIND_MSG;
    FpBindCfg->bindings[NumBind].src_fp_type = (UINT8)FP_TYPE_VENC;
    FpBindCfg->bindings[NumBind].src_fp_id = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_ch_id = (UINT8)StrmId;
    FpBindCfg->bindings[NumBind].src_str_id = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_fp_type = (UINT8)FP_TYPE_ENG0;
    FpBindCfg->bindings[NumBind].dst_fp_id = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_ch_id = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_str_id = (UINT8)0U; //TBD LowDelay
    NumBind++;

    FpBindCfg->num_of_bindings = NumBind;
}

static inline void HL_FillFpBindSetupVEnc(cmd_binding_cfg_t *FpBindCfg,
                                          const UINT8 BindType,
                                          const UINT16 SrcId,
                                          const UINT16 SrcPinId,
                                          const UINT16 StrmId)
{
    UINT8 NumBind = 0U;
    UINT16 ViewZoneId = SrcId;

    FpBindCfg->bindings[NumBind].bind_type = BindType;
    FpBindCfg->bindings[NumBind].bind_msg = (UINT8)FP_BIND_DATA;

    FpBindCfg->bindings[NumBind].src_fp_type = (UINT8)FP_TYPE_VPROC_DV;
    FpBindCfg->bindings[NumBind].src_fp_id = (UINT8)0U;

    FpBindCfg->bindings[NumBind].src_ch_id = (UINT8)ViewZoneId;

    FpBindCfg->bindings[NumBind].src_str_id = (UINT8)SrcPinId;

    FpBindCfg->bindings[NumBind].dst_fp_type = (UINT8)FP_TYPE_VENC;
    FpBindCfg->bindings[NumBind].dst_fp_id = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_ch_id = (UINT8)StrmId; //TBD LowDelay
    FpBindCfg->bindings[NumBind].dst_str_id = (UINT8)0U;
    NumBind++;

    FpBindCfg->num_of_bindings = NumBind;
}

static inline void HL_FillFpBindSetupDec(cmd_binding_cfg_t *FpBindCfg,
                                         const UINT8 BindType)
{
    UINT8 NumBind = 0U;

    //bindings[0]
    FpBindCfg->bindings[NumBind].bind_type      = BindType;
    FpBindCfg->bindings[NumBind].bind_msg       = (UINT8)FP_BIND_DATA;
    FpBindCfg->bindings[NumBind].src_fp_type    = (UINT8)FP_TYPE_DEC;
    FpBindCfg->bindings[NumBind].src_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_str_id     = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_fp_type    = (UINT8)FP_TYPE_DPROC;
    FpBindCfg->bindings[NumBind].dst_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_str_id     = (UINT8)0U;
    NumBind++;

    //bindings[1]
    FpBindCfg->bindings[NumBind].bind_type      = BindType;
    FpBindCfg->bindings[NumBind].bind_msg       = (UINT8)FP_BIND_MSG;
    FpBindCfg->bindings[NumBind].src_fp_type    = (UINT8)FP_TYPE_DEC;
    FpBindCfg->bindings[NumBind].src_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_str_id     = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_fp_type    = (UINT8)FP_TYPE_POSTP;
    FpBindCfg->bindings[NumBind].dst_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_str_id     = (UINT8)1U;
    NumBind++;

    //bindings[2]
    FpBindCfg->bindings[NumBind].bind_type      = BindType;
    FpBindCfg->bindings[NumBind].bind_msg       = (UINT8)FP_BIND_MSG;
    FpBindCfg->bindings[NumBind].src_fp_type    = (UINT8)FP_TYPE_POSTP;
    FpBindCfg->bindings[NumBind].src_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_str_id     = (UINT8)1U;    // second str is to postp for vout
    FpBindCfg->bindings[NumBind].dst_fp_type    = (UINT8)FP_TYPE_DEC;
    FpBindCfg->bindings[NumBind].dst_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_str_id     = (UINT8)0U;
    NumBind++;

    //bindings[3]
    FpBindCfg->bindings[NumBind].bind_type      = BindType;
    FpBindCfg->bindings[NumBind].bind_msg       = (UINT8)FP_BIND_MSG;
    FpBindCfg->bindings[NumBind].src_fp_type    = (UINT8)FP_TYPE_POSTP;
    FpBindCfg->bindings[NumBind].src_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_str_id     = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_fp_type    = (UINT8)FP_TYPE_DEC;
    FpBindCfg->bindings[NumBind].dst_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_str_id     = (UINT8)0U;
    NumBind++;

    //bindings[4]
    FpBindCfg->bindings[NumBind].bind_type      = BindType;
    FpBindCfg->bindings[NumBind].bind_msg       = (UINT8)FP_BIND_DATA;
    FpBindCfg->bindings[NumBind].src_fp_type    = (UINT8)FP_TYPE_DPROC;
    FpBindCfg->bindings[NumBind].src_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_str_id     = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_fp_type    = (UINT8)FP_TYPE_POSTP;
    FpBindCfg->bindings[NumBind].dst_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_str_id     = (UINT8)1U;
    NumBind++;

    //bindings[5]
    FpBindCfg->bindings[NumBind].bind_type      = BindType;
    FpBindCfg->bindings[NumBind].bind_msg       = (UINT8)FP_BIND_DATA;
    FpBindCfg->bindings[NumBind].src_fp_type    = (UINT8)FP_TYPE_DPROC;
    FpBindCfg->bindings[NumBind].src_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_str_id     = (UINT8)1U;
    FpBindCfg->bindings[NumBind].dst_fp_type    = (UINT8)FP_TYPE_POSTP;
    FpBindCfg->bindings[NumBind].dst_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_str_id     = (UINT8)0U;
    NumBind++;

    FpBindCfg->num_of_bindings = NumBind;

    /* Video decode use PostP no dproc*/
#if 0
    //bindings[0]
    FpBindCfg->bindings[NumBind].bind_type      = BindType;
    FpBindCfg->bindings[NumBind].bind_msg       = (UINT8)FP_BIND_DATA;
    FpBindCfg->bindings[NumBind].src_fp_type    = (UINT8)FP_TYPE_DEC;
    FpBindCfg->bindings[NumBind].src_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_str_id     = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_fp_type    = (UINT8)FP_TYPE_POSTP;
    FpBindCfg->bindings[NumBind].dst_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_str_id     = (UINT8)1U;    //Vout1
    NumBind++;

    //bindings[1]
    FpBindCfg->bindings[NumBind].bind_type      = BindType;
    FpBindCfg->bindings[NumBind].bind_msg       = (UINT8)FP_BIND_MSG;
    FpBindCfg->bindings[NumBind].src_fp_type    = (UINT8)FP_TYPE_DEC;
    FpBindCfg->bindings[NumBind].src_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_str_id     = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_fp_type    = (UINT8)FP_TYPE_POSTP;
    FpBindCfg->bindings[NumBind].dst_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_str_id     = (UINT8)1U;    //Vout1
    NumBind++;

    //bindings[2]
    FpBindCfg->bindings[NumBind].bind_type      = BindType;
    FpBindCfg->bindings[NumBind].bind_msg       = (UINT8)FP_BIND_MSG;
    FpBindCfg->bindings[NumBind].src_fp_type    = (UINT8)FP_TYPE_POSTP;
    FpBindCfg->bindings[NumBind].src_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_str_id     = (UINT8)1U;    //Vout1
    FpBindCfg->bindings[NumBind].dst_fp_type    = (UINT8)FP_TYPE_DEC;
    FpBindCfg->bindings[NumBind].dst_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_str_id     = (UINT8)0U;
    NumBind++;

    //bindings[3]
    FpBindCfg->bindings[NumBind].bind_type      = BindType;
    FpBindCfg->bindings[NumBind].bind_msg       = (UINT8)FP_BIND_MSG;
    FpBindCfg->bindings[NumBind].src_fp_type    = (UINT8)FP_TYPE_POSTP;
    FpBindCfg->bindings[NumBind].src_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_str_id     = (UINT8)0U;    //Vout0
    FpBindCfg->bindings[NumBind].dst_fp_type    = (UINT8)FP_TYPE_DEC;
    FpBindCfg->bindings[NumBind].dst_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_str_id     = (UINT8)0U;
    NumBind++;

    FpBindCfg->num_of_bindings = NumBind;
#endif
}

static inline void HL_FillFpBindSetupStlEnc(cmd_binding_cfg_t *FpBindCfg,
                                            const UINT8 BindType,
                                            const UINT16 SrcId,
                                            const UINT16 StrmId)
{
    UINT8 NumBind = 0U;

    FpBindCfg->bindings[NumBind].bind_type = BindType;
    FpBindCfg->bindings[NumBind].bind_msg = (UINT8)FP_BIND_DATA;
    FpBindCfg->bindings[NumBind].src_fp_type = (UINT8)FP_TYPE_VIN;
    FpBindCfg->bindings[NumBind].src_fp_id = (UINT8)SrcId;
    FpBindCfg->bindings[NumBind].src_ch_id = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_str_id = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_fp_type = (UINT8)FP_TYPE_VENC;
    FpBindCfg->bindings[NumBind].dst_fp_id = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_ch_id = (UINT8)StrmId;
    FpBindCfg->bindings[NumBind].dst_str_id = (UINT8)0U;
    NumBind++;

    FpBindCfg->bindings[NumBind].bind_type = BindType;
    FpBindCfg->bindings[NumBind].bind_msg = (UINT8)FP_BIND_MSG;
    FpBindCfg->bindings[NumBind].src_fp_type = (UINT8)FP_TYPE_VENC;
    FpBindCfg->bindings[NumBind].src_fp_id = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_ch_id = (UINT8)StrmId;
    FpBindCfg->bindings[NumBind].src_str_id = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_fp_type = (UINT8)FP_TYPE_ENG0;
    FpBindCfg->bindings[NumBind].dst_fp_id = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_ch_id = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_str_id = (UINT8)0U;
    NumBind++;

    FpBindCfg->num_of_bindings = NumBind;
}

static inline void HL_FillFpBindSProc(cmd_binding_cfg_t *FpBindCfg,
                                      const UINT8 BindType,
                                      const UINT16 SrcId,
                                      const UINT16 SrcPinId,
                                      const UINT16 StrmId)
{
    UINT8 NumBind = 0U;

    FpBindCfg->bindings[NumBind].bind_type = BindType;
    FpBindCfg->bindings[NumBind].bind_msg = (UINT8)FP_BIND_DATA;
    FpBindCfg->bindings[NumBind].src_fp_type = (UINT8)FP_TYPE_VIN;
    FpBindCfg->bindings[NumBind].src_fp_id = (UINT8)SrcId;
    FpBindCfg->bindings[NumBind].src_ch_id = (UINT8)SrcPinId;
    FpBindCfg->bindings[NumBind].src_str_id = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_fp_type = (UINT8)FP_TYPE_VPROC_DV;
    FpBindCfg->bindings[NumBind].dst_fp_id = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_ch_id = (UINT8)StrmId;
    FpBindCfg->bindings[NumBind].dst_str_id = (UINT8)0U;
    NumBind++;

    FpBindCfg->num_of_bindings = NumBind;
}

static inline void HL_FillFpBindDec2VProc(cmd_binding_cfg_t *FpBindCfg,
                                          const UINT8 BindType,
                                          const UINT16 SrcId,
                                          const UINT16 StrmId)
{
    UINT8 NumBind = 0U;

    FpBindCfg->bindings[NumBind].bind_type      = BindType;
    FpBindCfg->bindings[NumBind].bind_msg       = (UINT8)FP_BIND_DATA;
    FpBindCfg->bindings[NumBind].src_fp_type    = (UINT8)FP_TYPE_DEC;
    FpBindCfg->bindings[NumBind].src_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_ch_id      = (UINT8)SrcId;
    FpBindCfg->bindings[NumBind].src_str_id     = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_fp_type    = (UINT8)FP_TYPE_VPROC_DV;
    FpBindCfg->bindings[NumBind].dst_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_ch_id      = (UINT8)StrmId;
    FpBindCfg->bindings[NumBind].dst_str_id     = (UINT8)0U;
    NumBind++;

    FpBindCfg->num_of_bindings = NumBind;
}

static inline void HL_FillFpBindRecon2VProc(cmd_binding_cfg_t *FpBindCfg,
                                            const UINT16 SrcId,
                                            const UINT16 StrmId)
{
    UINT8 NumBind = 0U;

    FpBindCfg->bindings[NumBind].bind_type      = (UINT8)BIND_TYPE_BIND;
    FpBindCfg->bindings[NumBind].bind_msg       = (UINT8)FP_BIND_DATA;
    FpBindCfg->bindings[NumBind].src_fp_type    = (UINT8)FP_TYPE_VENC;
    FpBindCfg->bindings[NumBind].src_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_ch_id      = (UINT8)SrcId;
    FpBindCfg->bindings[NumBind].src_str_id     = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_fp_type    = (UINT8)FP_TYPE_VPROC_DV;
    FpBindCfg->bindings[NumBind].dst_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_ch_id      = (UINT8)StrmId;
    FpBindCfg->bindings[NumBind].dst_str_id     = (UINT8)0U;
    NumBind++;

    FpBindCfg->num_of_bindings = NumBind;
}

#if 0
static inline void HL_FillFpBindStlDec(void)
{
    /* Still decode use Dproc2PostP, no Dec2PostP needed */
    FpBindCfg->bindings[NumBind].bind_type      = (UINT8)BIND_TYPE_BIND;
    FpBindCfg->bindings[NumBind].bind_msg       = (UINT8)FP_BIND_DATA;
    FpBindCfg->bindings[NumBind].src_fp_type    = (UINT8)FP_TYPE_DEC;
    FpBindCfg->bindings[NumBind].src_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_str_id     = (UINT8)SrcId;
    FpBindCfg->bindings[NumBind].dst_fp_type    = (UINT8)FP_TYPE_POSTP;
    FpBindCfg->bindings[NumBind].dst_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_str_id     = (UINT8)1U; //VOUT1
    NumBind++;

    FpBindCfg->bindings[NumBind].bind_type      = (UINT8)BIND_TYPE_BIND;
    FpBindCfg->bindings[NumBind].bind_msg       = (UINT8)FP_BIND_MSG;
    FpBindCfg->bindings[NumBind].src_fp_type    = (UINT8)FP_TYPE_DEC;
    FpBindCfg->bindings[NumBind].src_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_str_id     = (UINT8)SrcId;
    FpBindCfg->bindings[NumBind].dst_fp_type    = (UINT8)FP_TYPE_POSTP;
    FpBindCfg->bindings[NumBind].dst_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_str_id     = (UINT8)1U; //VOUT1
    NumBind++;

    FpBindCfg->bindings[NumBind].bind_type      = (UINT8)BIND_TYPE_BIND;
    FpBindCfg->bindings[NumBind].bind_msg       = (UINT8)FP_BIND_MSG;
    FpBindCfg->bindings[NumBind].src_fp_type    = (UINT8)FP_TYPE_POSTP;
    FpBindCfg->bindings[NumBind].src_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_str_id     = (UINT8)1U; //VOUT1
    FpBindCfg->bindings[NumBind].dst_fp_type    = (UINT8)FP_TYPE_DEC;
    FpBindCfg->bindings[NumBind].dst_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_str_id     = (UINT8)SrcId;
    NumBind++;

    FpBindCfg->bindings[NumBind].bind_type      = (UINT8)BIND_TYPE_BIND;
    FpBindCfg->bindings[NumBind].bind_msg       = (UINT8)FP_BIND_MSG;
    FpBindCfg->bindings[NumBind].src_fp_type    = (UINT8)FP_TYPE_POSTP;
    FpBindCfg->bindings[NumBind].src_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_str_id     = (UINT8)0U; //VOUT0
    FpBindCfg->bindings[NumBind].dst_fp_type    = (UINT8)FP_TYPE_DEC;
    FpBindCfg->bindings[NumBind].dst_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_str_id     = (UINT8)SrcId;
    NumBind++;

    FpBindCfg->num_of_bindings = NumBind;
}
#endif

static inline void HL_FillFpBindRStlDecY2Disp(cmd_binding_cfg_t *FpBindCfg)
{
    UINT8 NumBind = 0U;

    FpBindCfg->bindings[NumBind].bind_type      = (UINT8)BIND_TYPE_BIND;
    FpBindCfg->bindings[NumBind].bind_msg       = (UINT8)FP_BIND_DATA;
    FpBindCfg->bindings[NumBind].src_fp_type    = (UINT8)FP_TYPE_DPROC;
    FpBindCfg->bindings[NumBind].src_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_str_id     = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_fp_type    = (UINT8)FP_TYPE_POSTP;
    FpBindCfg->bindings[NumBind].dst_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_str_id     = (UINT8)VOUT_IDX_B;
    NumBind++;

    FpBindCfg->bindings[NumBind].bind_type      = (UINT8)BIND_TYPE_BIND;
    FpBindCfg->bindings[NumBind].bind_msg       = (UINT8)FP_BIND_MSG;
    FpBindCfg->bindings[NumBind].src_fp_type    = (UINT8)FP_TYPE_DPROC;
    FpBindCfg->bindings[NumBind].src_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_str_id     = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_fp_type    = (UINT8)FP_TYPE_POSTP;
    FpBindCfg->bindings[NumBind].dst_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_str_id     = (UINT8)VOUT_IDX_B;
    NumBind++;

    FpBindCfg->bindings[NumBind].bind_type      = (UINT8)BIND_TYPE_BIND;
    FpBindCfg->bindings[NumBind].bind_msg       = (UINT8)FP_BIND_DATA;
    FpBindCfg->bindings[NumBind].src_fp_type    = (UINT8)FP_TYPE_DPROC;
    FpBindCfg->bindings[NumBind].src_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_str_id     = (UINT8)VOUT_IDX_B;
    FpBindCfg->bindings[NumBind].dst_fp_type    = (UINT8)FP_TYPE_POSTP;
    FpBindCfg->bindings[NumBind].dst_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_str_id     = (UINT8)VOUT_IDX_A;
    NumBind++;

    FpBindCfg->bindings[NumBind].bind_type      = (UINT8)BIND_TYPE_BIND;
    FpBindCfg->bindings[NumBind].bind_msg       = (UINT8)FP_BIND_MSG;
    FpBindCfg->bindings[NumBind].src_fp_type    = (UINT8)FP_TYPE_DPROC;
    FpBindCfg->bindings[NumBind].src_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].src_str_id     = (UINT8)VOUT_IDX_B;
    FpBindCfg->bindings[NumBind].dst_fp_type    = (UINT8)FP_TYPE_POSTP;
    FpBindCfg->bindings[NumBind].dst_fp_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_ch_id      = (UINT8)0U;
    FpBindCfg->bindings[NumBind].dst_str_id     = (UINT8)VOUT_IDX_A;
    NumBind++;

    FpBindCfg->num_of_bindings = NumBind;
}

static UINT32 FillFpBindSetup(cmd_binding_cfg_t *FpBindCfg,
                          UINT8 BindPurpose,
                          UINT8 IsBind,
                          UINT16 SrcId,
                          UINT16 SrcPinId,
                          UINT16 StrmId)
{
    UINT8 BindType;
    UINT32 Rval = OK;

    if (IsBind == 1U) {
        BindType = BIND_TYPE_BIND;
    } else {
        BindType = FP_BIND_TYPE_UNBIND;
    }

    if (FpBindCfg == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (BindPurpose == BIND_VIDEO_YUVCONV) {
            HL_FillFpBindSetupYuvConv(FpBindCfg, BindType, SrcId);
        } else if ((BIND_VIDEO_ENCODE == BindPurpose) || \
                   (BIND_VIDEO_INTERVAL_ENCODE == BindPurpose) || \
                   (BIND_VIDEO_ENCODE_YUV == BindPurpose)) {
            HL_FillFpBindSetupVidEnc(FpBindCfg, BindPurpose, BindType, SrcId, SrcPinId, StrmId);
        } else if (BIND_VIDEO_ENCODE_VENC == BindPurpose) {
            HL_FillFpBindSetupVEnc(FpBindCfg, BindType, SrcId, SrcPinId, StrmId);
        } else if (BIND_VIDEO_DECODE == BindPurpose) {
            HL_FillFpBindSetupDec(FpBindCfg, BindType);
        } else if (BIND_STILL_ENCODE == BindPurpose) {
            HL_FillFpBindSetupStlEnc(FpBindCfg, BindType, SrcId, StrmId);
        } else if (BindPurpose == BIND_STILL_PROC) {
            HL_FillFpBindSProc(FpBindCfg, BindType, SrcId, SrcPinId, StrmId);
        } else if (BIND_VIDEO_DECODE_TO_VPROC == BindPurpose) {
            HL_FillFpBindDec2VProc(FpBindCfg, BindType, SrcId, StrmId);
        } else if (BIND_VIDEO_RECON_TO_VPROC == BindPurpose) {
            HL_FillFpBindRecon2VProc(FpBindCfg, SrcId, StrmId);
        } else if (BIND_STILL_DECODE == BindPurpose) {
            /* Still decode use Dproc2PostP, no Dec2PostP needed */
            //HL_FillFpBindStlDec();
        } else if (BIND_STILL_DECODE_Y2DISP == BindPurpose) {
            HL_FillFpBindRStlDecY2Disp(FpBindCfg);
        } else {
            Rval = DSP_ERR_0001;
        }
    }

    return Rval;
}
UINT32 HL_FillFpBindSetup(cmd_binding_cfg_t *FpBindCfg,
                          UINT8 BindPurpose,
                          UINT8 IsBind,
                          UINT16 SrcId,
                          UINT16 SrcPinId,
                          UINT16 StrmId)
{
    UINT32 Rval;
    UINT32 CrcHL, CrcSimilar;
    const UINT32 *pU32 = NULL;
    UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;
#ifdef CONFIG_ENABLE_DSP_MONITOR
    AMBA_DSP_MONITOR_CRC_CMPR_s CrcCmpr;
#endif

    Rval = FillFpBindSetup(FpBindCfg, BindPurpose, IsBind, SrcId, SrcPinId, StrmId);
    if (pIsSimilarEnabled != NULL) {
        if (1U == IsSimilarEnabled()) {
            UINT32 SIM_Rval;
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_binding_cfg_t *SimFpBindCfg = SIM_DefCtxCmdBufPtrBind;

            dsp_osal_typecast(&pU32, &FpBindCfg);
            CrcHL = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_binding_cfg_t), 0U);

            SIM_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&SimFpBindCfg, &CmdBufferAddr);
            SIM_Rval = SIM_FillFpBindSetup(SimFpBindCfg, BindPurpose, IsBind, SrcId, SrcPinId, StrmId);
            if (SIM_Rval == OK) {
                dsp_osal_typecast(&pU32, &SimFpBindCfg);
                CrcSimilar = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_binding_cfg_t), 0U);
#ifdef CONFIG_ENABLE_DSP_MONITOR
                CrcCmpr.ModuleId = AMBA_SSP_PRINT_MODULE_ID;
                CrcCmpr.Crc0 = CrcHL;
                CrcCmpr.Crc1 = CrcSimilar;
                CrcCmpr.UUID = __LINE__;
                SIM_Rval = dsp_mon_crc_cmpr(&CrcCmpr);
                if (SIM_Rval != OK) {
                    AmbaLL_LogUInt5("dsp_mon_crc_cmpr return 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
                }
#else
                if (CrcSimilar != CrcHL) {
                    AmbaLL_LogUInt5("[ERR] Similar CRC mismatch @ %d HL 0x%x Similar 0x%x", __LINE__, CrcHL, CrcSimilar, 0U, 0U);
                }
#endif
            } else {
                AmbaLL_LogUInt5("[ERR] Similar function returned NG 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
            }
            SIM_RelCmdBuffer(CmdBufferId);
        }
    }

    return Rval;
}

static inline void HL_FillVideoProcCfgVin(cmd_vproc_cfg_t *VidPrep,
                                          const UINT16 ViewZoneId,
                                          const UINT16 YuyvNumber,
                                          const CTX_VIN_INFO_s *pVinInfo)
{
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    /* Vin Relative */
    HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
    if (ViewZoneInfo->InputFromMemory != VIN_SRC_FROM_RECON) {
        if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM) ||
            (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_422) ||
            (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_420)) {
            if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_420) {
                VidPrep->input_format = DSP_VPROC_INPUT_YUV_420;
            } else if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_422) {
                VidPrep->input_format = DSP_VPROC_INPUT_YUV_422;
            } else {
                VidPrep->input_format = DSP_VPROC_INPUT_RAW_RGB;
            }
        } else if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) {
            VidPrep->input_format = DSP_VPROC_INPUT_YUV_420;
        } else {
            if (YuyvNumber > 0U) {
                VidPrep->input_format = DSP_VPROC_INPUT_MIPI_YUYV;
            } else {
                if (HL_IS_YUV420_INPUT(pVinInfo->InputFormat) == 1U) {
                    VidPrep->input_format = DSP_VPROC_INPUT_YUV_420;
                } else if (HL_IS_YUV422_INPUT(pVinInfo->InputFormat) == 1U) {
                    VidPrep->input_format = DSP_VPROC_INPUT_YUV_422;
                } else {
                    VidPrep->input_format = DSP_VPROC_INPUT_RAW_RGB;
                }
            }
        }
    } else {
        VidPrep->input_format = DSP_VPROC_INPUT_YUV_420;
    }
}


static inline void HL_FillVideoProcCfgVinRaw(cmd_vproc_cfg_t *VidPrep,
                                             const UINT16 ViewZoneId,
                                             const UINT16 YuyvNumber,
                                             const UINT8 VinExist,
                                             const CTX_VIN_INFO_s *pVinInfo,
                                             const UINT16 RescMaxVinWidth,
                                             const UINT16 RescMaxVinHeight)
{
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
    if (VinExist > 0U) {
        if (pVinInfo->CfaCompressed > 0U) {
            UINT16 RawWidth = 0U, RawPitch = 0U, Mantissa = 0U, BlkSz = 0U;

            (void)HL_GetCmprRawBufInfo(RescMaxVinWidth,
                                       pVinInfo->CfaCompressed,
                                       pVinInfo->CfaCompact,
                                       &RawWidth, &RawPitch,
                                       &Mantissa, &BlkSz);
            VidPrep->raw_comp_blk_sz_wst = (UINT8)BlkSz;
            VidPrep->raw_comp_mantissa_wst = (UINT8)Mantissa;
        } else {
            VidPrep->raw_comp_blk_sz_wst = 0U;
            VidPrep->raw_comp_mantissa_wst = 0U;
        }

//FIXME, use ResourceMaxVin or currently CapWin
        // Uncompressed raw byte unit, DONT CARE when YUV input
        if (YuyvNumber > 0U) {
            VidPrep->raw_width_max = (UINT16)((UINT16)(RescMaxVinWidth << 1U) << 1U);
        } else {
            VidPrep->raw_width_max = (UINT16)(RescMaxVinWidth << 1U);
        }

        if (ViewZoneInfo->HdrBlendNumMinusOne > 0U) {
            VidPrep->raw_height_max = pVinInfo->CapWindow[0U].Height; //total raw capture height
        } else {
            VidPrep->raw_height_max = RescMaxVinHeight; //total raw capture height
        }
    } else {
        VidPrep->raw_comp_blk_sz_wst = 0U;
        VidPrep->raw_comp_mantissa_wst = 0U;

//FIXME, use ResourceMaxVin or currently CapWin
        VidPrep->raw_width_max = (UINT16)(ViewZoneInfo->CapWindow.Width << 1U); // uncompressed raw byte unit, DONT CARE when YUV input
        VidPrep->raw_height_max = ViewZoneInfo->CapWindow.Height;
    }
}

static inline void HL_FillVideoProcCfgPymdLndt(cmd_vproc_cfg_t *VidPrep,
                                               const UINT16 ViewZoneId)
{
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
    /* Pymd relative */
    VidPrep->hier_y12_fb_num = (UINT8)DSP_VPROC_HIER_NUM_DISABLE; //default turn off
    if ((ViewZoneInfo->PymdAllocType == ALLOC_INTERNAL) &&
        (VidPrep->prev_hier_w_max > 0U) &&
        (VidPrep->prev_hier_h_max > 0U)) {
        if (ViewZoneInfo->PymdBufNum == 0U) {
            VidPrep->hier_fb_num = (UINT8)CV2X_DEFAULT_HIER_FB_NUM;
            if (DSP_GetU16Bit(ViewZoneInfo->Pyramid.IsPolySqrt2, DSP_PYMD_Y12_IDX, DSP_PYMD_Y12_LEN) > 0U) {
                VidPrep->hier_y12_fb_num = (UINT8)CV2X_DEFAULT_HIER_FB_NUM;
            }
        } else {
            VidPrep->hier_fb_num = (UINT8)ViewZoneInfo->PymdBufNum;
            if (DSP_GetU16Bit(ViewZoneInfo->Pyramid.IsPolySqrt2, DSP_PYMD_Y12_IDX, DSP_PYMD_Y12_LEN) > 0U) {
                VidPrep->hier_y12_fb_num = (UINT8)ViewZoneInfo->PymdBufNum;
            }
        }
    } else {
        VidPrep->hier_fb_num = (UINT8)DSP_VPROC_HIER_NUM_DISABLE;
    }

    if (DSP_GetU16Bit(ViewZoneInfo->Pyramid.IsPolySqrt2, DSP_PYMD_Y12_IDX, DSP_PYMD_Y12_LEN) > 0U) {
        VidPrep->is_y12_out_enabled = 1U;
    }

    /* Lndt relative */
    (void)HL_GetSystemLndtMaxWindow(&VidPrep->prev_ln_w_max, &VidPrep->prev_ln_h_max);
    if ((ViewZoneInfo->LndtAllocType == ALLOC_INTERNAL) &&
        (VidPrep->prev_ln_w_max > 0U) &&
        (VidPrep->prev_ln_h_max > 0U)) {
        if (ViewZoneInfo->LndtBufNum == 0U) {
            VidPrep->ln_dec_fb_num = (UINT8)CV2X_DEFAULT_LNDT_FB_NUM;
        } else {
            VidPrep->ln_dec_fb_num = (UINT8)ViewZoneInfo->LndtBufNum;
        }
    } else {
        VidPrep->ln_dec_fb_num = (UINT8)DSP_VPROC_LNDEC_NUM_DISABLE;
    }

    /* Main Y12 relative */
    if ((ViewZoneInfo->MainY12Enable > 0U) &&
        (VidPrep->is_y12_out_enabled == 0U)) {
        VidPrep->is_c2y_y12_out_enabled = 1U;
    }
}

static inline void HL_GetVideoProcTileX(const UINT8 IsTileMode,
                                        const UINT16 ViewZoneId,
                                        const CTX_VIEWZONE_INFO_s *pViewZoneInfo,
                                        const CTX_RESOURCE_INFO_s *pResource,
                                        UINT8 *pC2YTileX,
                                        UINT8 *pWarpTileX)
{
    UINT8 FixedOverlap;
    UINT16 TileOverlap, C2YInTileX, C2YTileX, MctfTileX;

    if (IsTileMode > 0U) {
        //get init tile overlap
        (void)HL_GetViewZoneWarpOverlapX(ViewZoneId, &TileOverlap);
        FixedOverlap = (pResource->MaxHorWarpComp[ViewZoneId] > 0U) ? 1U : 0U;

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
        (void)HL_CalcVideoTileC2Y(0xFF/*ignore ViewId*/,
                                  pViewZoneInfo->Main.Width,
                                  SEC2_MAX_OUT_WIDTH,
                                  WARP_GRID_EXP,
                                  FixedOverlap,
                                  1U, /* ChkSmem */
                                  &MctfTileX,
                                  &TileOverlap);
#ifdef DEBUG_TILE_CALC
        AmbaLL_LogUInt5("VprocSetup ViewZoneId:%u CapWindow.Width:%u Main.Width:%u TileOverlap:%u",
                ViewZoneId, pViewZoneInfo->CapWindow.Width, pViewZoneInfo->Main.Width, TileOverlap, 0U);
        AmbaLL_LogUInt5("VprocSetup ViewZoneId:%u C2YInTileX:%u C2YTileX:%u MctfTileX:%u",
                ViewZoneId, C2YInTileX, C2YTileX, MctfTileX, 0U);
#endif
        *pC2YTileX = (UINT8)(MAX2_16(C2YInTileX, C2YTileX));
        *pWarpTileX = (UINT8)((pViewZoneInfo->Pipe == DSP_DRAM_PIPE_EFCY) ? MAX2_16(C2YInTileX, C2YTileX): MctfTileX);
    } else {
        *pC2YTileX = 1U;
        *pWarpTileX = 1U;
    }
}

static void HL_FillVideoProcCfgRoi(cmd_vproc_cfg_t *VidPrep, UINT16 ViewZoneId,
                                   const CTX_RESOURCE_INFO_s *pResource, const CTX_VIEWZONE_INFO_s *pViewZoneInfo)
{
    /* Slice */
    VidPrep->c2y_tile_num_y_max = (pViewZoneInfo->SliceNumRow > 1U)? pViewZoneInfo->SliceNumRow: 1U;
    VidPrep->warp_tile_num_y_max = (pViewZoneInfo->SliceNumRow > 1U)? pViewZoneInfo->SliceNumRow: 1U;
    {
        UINT8 IsTileMode, C2YTileX = 0U, WarpTileX = 0U;

        (void)HL_IsTileMode(&IsTileMode);
        HL_GetVideoProcTileX(IsTileMode, ViewZoneId, pViewZoneInfo, pResource, &C2YTileX, &WarpTileX);

        C2YTileX = (UINT8)(MAX2_16((UINT16)C2YTileX, (UINT16)pViewZoneInfo->SliceNumCol));
        WarpTileX = (UINT8)(MAX2_16((UINT16)WarpTileX, (UINT16)pViewZoneInfo->SliceNumCol));

        VidPrep->c2y_tile_num_x_max = C2YTileX;
        VidPrep->warp_tile_num_x_max = WarpTileX;
    }

//FIXME
    VidPrep->is_li_enabled = 0U;
    if (TuneDspSystemCfg.LiEnable > 0U) {
        VidPrep->is_li_enabled = (UINT8)(TuneDspSystemCfg.LiEnable - 1U);
    }

    VidPrep->is_warp_enabled = 1U;

}

static void HL_FillVideoProcCfgEffect(cmd_vproc_cfg_t *VidPrep, UINT16 ViewZoneId, const CTX_RESOURCE_INFO_s *pResource)
{
    UINT8 EffOnChan = 0U;
    UINT16 i;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;

    for (i=0; (i<pResource->YuvStrmNum); i++) {
        HL_GetYuvStrmInfoLock(i, &YuvStrmInfo);
        if (1U == DSP_GetBit(YuvStrmInfo->ChanBitMask, ViewZoneId, 1U)) {
            if (YuvStrmInfo->MaxChanNum > 1U) {
                EffOnChan = 1U;
            } else if (YuvStrmInfo->MaxChanNum == 1U) {
                if ((YuvStrmInfo->Layout.ChanCfg[0U].Window.OffsetX > 0U) ||
                    (YuvStrmInfo->Layout.ChanCfg[0U].Window.OffsetY > 0U) ||
                    (YuvStrmInfo->Layout.ChanCfg[0U].RotateFlip != AMBA_DSP_ROTATE_0)) {
                    EffOnChan = 1U;
                }
            } else {
                //TBD
            }
        }
        HL_GetYuvStrmInfoUnLock(i);
    }

    if (EffOnChan == 1U) {
//            if (HL_GetDefaultRawEnable() == 1U) {
            VidPrep->effect_copy_job_num_max = DSP_MAX_PP_STRM_BLEND_NUMBER;
            VidPrep->effect_blend_job_num_max = DSP_MAX_PP_STRM_BLEND_NUMBER;
//            }
    } else {
//                AmbaLL_LogUInt5("effect_y2y_job_num_max %u off", ViewZoneId, 0U, 0U, 0U, 0U);
    }
}

static inline void FillVideoProcCfgHalf(cmd_vproc_cfg_t *VidPrep,
                                        const UINT8 VinExist,
                                        const UINT16 ViewZoneId,
                                        const UINT16 RescMaxVinWidth,
                                        const UINT16 RescMaxVinHeight,
                                        const CTX_VIEWZONE_INFO_s *ViewZoneInfo,
                                        const CTX_RESOURCE_INFO_s *Resource)
{
    UINT16 Num = 0U, AuxNum = 0U;
    UINT16 ChromaDmaSize = 0U;

    /* C2Y relative */
    if (VinExist > 0U) {
        (void)HL_GetViewZoneVprocPinMaxWin(ViewZoneId,
                                           DSP_VPROC_PIN_MAIN,
                                           &VidPrep->W_main_max,
                                           &VidPrep->H_main_max);
        VidPrep->H_main_max = ALIGN_NUM16(VidPrep->H_main_max, 16U);
        VidPrep->W0_max = RescMaxVinWidth;

        if (ViewZoneInfo->HdrBlendNumMinusOne > 0U) {
            VidPrep->H0_max = ViewZoneInfo->HdrBlendHeight;
        } else {
            VidPrep->H0_max = RescMaxVinHeight;
        }
    } else {
        VidPrep->W_main_max = ViewZoneInfo->Main.Width;
        VidPrep->H_main_max = ALIGN_NUM16(ViewZoneInfo->Main.Height, 16U);
        VidPrep->W0_max = ViewZoneInfo->CapWindow.Width;
        VidPrep->H0_max = ViewZoneInfo->CapWindow.Height;
    }

    /* Per ucoder, c2y int fb shall be 2 for ping-pong in normal case */
    VidPrep->c2y_int_fb_num = CV2X_MAX_C2Y_FB_NUM;
    VidPrep->c2y_int_y12_fb_num = CV2X_MAX_C2Y_FB_NUM;

    /* Main frame buffer */
    (void)HL_GetViewZonePinFbNum(ViewZoneId, DSP_VPROC_PIN_MAIN, &Num, &AuxNum, 0U);
    VidPrep->main_fb_num = (UINT8)Num;
#if defined (SUPPORT_DSP_MAIN_ME_DISABLE)
    VidPrep->main_me01_fb_num = (UINT8)AuxNum;
#else
    VidPrep->main_me01_fb_num = (UINT8)Num;
#endif

    /* Warp relative */
    VidPrep->warp_a_y_in_blk_h_max = (Resource->MaxWarpDma[ViewZoneId] > 0U)? Resource->MaxWarpDma[ViewZoneId]: CV2X_WARP_DMA_SIZE;
    HL_GetWarpChromaDma(VidPrep->warp_a_y_in_blk_h_max, &ChromaDmaSize);
    VidPrep->warp_a_uv_in_blk_h_max = ChromaDmaSize;

    // MaxPreWarp Shall follow Main, FIXME when introduce DZOOM
    VidPrep->W_pre_warp_luma_max = VidPrep->W_main_max;
    VidPrep->H_pre_warp_luma_max = VidPrep->H_main_max;

    /* Prev filter relative */
    (void)HL_GetViewZoneVprocPinMaxWin(ViewZoneId,
                                       DSP_VPROC_PIN_PREVA,
                                       &VidPrep->prev_a_w_max,
                                       &VidPrep->prev_a_h_max);
    (void)HL_GetViewZoneVprocPinMaxWin(ViewZoneId,
                                       DSP_VPROC_PIN_PREVB,
                                       &VidPrep->prev_b_w_max,
                                       &VidPrep->prev_b_h_max);
    (void)HL_GetViewZoneVprocPinMaxWin(ViewZoneId,
                                       DSP_VPROC_PIN_PREVC,
                                       &VidPrep->prev_c_w_max,
                                       &VidPrep->prev_c_h_max);
    /* Hier filter relative */
    (void)HL_GetViewZonePymdMaxWindow(&VidPrep->prev_hier_w_max,
                                      &VidPrep->prev_hier_h_max,
                                      ViewZoneId);
#ifdef SUPPORT_DSP_PREVB_MASTER_MODE
    if ((VidPrep->prev_b_w_max == 0U) &&
        ((VidPrep->prev_a_w_max != 0U) ||
         (VidPrep->prev_c_w_max != 0U) ||
         (VidPrep->prev_hier_w_max != 0U))) {
        VidPrep->prev_b_w_max = 640U;
    }
    if ((VidPrep->prev_b_h_max == 0U) &&
        ((VidPrep->prev_a_h_max != 0U) ||
         (VidPrep->prev_c_h_max != 0U) ||
         (VidPrep->prev_hier_h_max != 0U))) {
        VidPrep->prev_b_h_max = 480U;
    }
#endif

    (void)HL_GetViewZonePinFbNum(ViewZoneId, DSP_VPROC_PIN_PREVA, &Num, &AuxNum, 0U);
    VidPrep->prev_a_fb_num = (UINT8)Num;
    VidPrep->prev_a_me_fb_num = VidPrep->prev_a_fb_num;
    (void)HL_GetViewZonePinFbNum(ViewZoneId, DSP_VPROC_PIN_PREVB, &Num, &AuxNum, 0U);
    VidPrep->prev_b_fb_num = (UINT8)Num;
    VidPrep->prev_b_me_fb_num = VidPrep->prev_b_fb_num;
    (void)HL_GetViewZonePinFbNum(ViewZoneId, DSP_VPROC_PIN_PREVC, &Num, &AuxNum, 0U);
    VidPrep->prev_c_fb_num = (UINT8)Num;
    VidPrep->prev_c_me_fb_num = VidPrep->prev_c_fb_num;

    VidPrep->prev_mode = 0U; //DramPreview

    HL_FillVideoProcCfgPymdLndt(VidPrep, ViewZoneId);

    /* ROI */
    //CV2FS default support ROI in frame/tile mode
    HL_FillVideoProcCfgRoi(VidPrep, ViewZoneId, Resource, ViewZoneInfo);

    /* Effect */
    HL_FillVideoProcCfgEffect(VidPrep, ViewZoneId, Resource);
}

static UINT32 FillVideoProcCfg(cmd_vproc_cfg_t *VidPrep, UINT16 ViewZoneId)
{
    UINT32 Rval = OK;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIN_INFO_s VinInfo;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 VinId = 0U, GrpIdx = 0U;
    UINT8 VinExist = 0U, IsVirtVin = 0U;
    UINT16 YuyvNumber = 0U;
    UINT32 ViewZoneBitMask = 0U;
    UINT16 RescMaxVinWidth = 0U;
    UINT16 RescMaxVinHeight = 0U;

    if (VidPrep == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        HL_GetResourcePtr(&Resource);
        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);

        (void)dsp_osal_memset(&VinInfo, 0, sizeof(CTX_VIN_INFO_s));
        if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON) ||
            (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) ||
            (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_420) ||
            (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_422)) {
            VinExist = (UINT8)0U;
        } else {
            VinExist = (UINT8)1U;
            DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &VinId);
            (void)HL_IsVirtualVinIdx(VinId, &IsVirtVin);
            HL_GetVinInfo(HL_MTX_OPT_ALL, VinId, &VinInfo);

            if ((IsVirtVin == 1U) && (VinId >= AMBA_DSP_MAX_VIN_NUM)) {
                RescMaxVinWidth = Resource->MaxVirtVinWidth[VinId - AMBA_DSP_MAX_VIN_NUM];
                RescMaxVinHeight = Resource->MaxVirtVinHeight[VinId - AMBA_DSP_MAX_VIN_NUM];
            } else {
                RescMaxVinWidth = Resource->MaxVinVirtChanWidth[VinId][0U];
                RescMaxVinHeight = Resource->MaxVinVirtChanHeight[VinId][0U];
                if ((VinInfo.Option[0U] == AMBA_DSP_VIN_CAP_OPT_INTC) &&
                    (VinInfo.IntcNum[0U] > 1U)) {
                    RescMaxVinHeight /= VinInfo.IntcNum[0U];
                }
            }
        }

        VidPrep->channel_id = (UINT8)ViewZoneId;

        DSP_SetBit(&ViewZoneBitMask, ViewZoneId);
        (void)HL_GetVprocMipiYuyvNumber(ViewZoneBitMask, &YuyvNumber);

        /* Vin Relative */
        HL_FillVideoProcCfgVin(VidPrep,
                               ViewZoneId,
                               YuyvNumber,
                               &VinInfo);
        HL_FillVideoProcCfgVinRaw(VidPrep,
                                  ViewZoneId,
                                  YuyvNumber,
                                  VinExist,
                                  &VinInfo,
                                  RescMaxVinWidth,
                                  RescMaxVinHeight);

        /* HDR relative */

        /* Group ID */
        Rval = HL_GetVprocGroupIdx(ViewZoneId, &GrpIdx, 0/*IsStlProc*/);
        if (Rval == OK) {
            VidPrep->grp_id = (UINT8)GrpIdx;
        }

        FillVideoProcCfgHalf(VidPrep,
                             VinExist,
                             ViewZoneId,
                             RescMaxVinWidth,
                             RescMaxVinHeight,
                             ViewZoneInfo,
                             Resource);
    }

    return Rval;
}
UINT32 HL_FillVideoProcCfg(cmd_vproc_cfg_t *VidPrep, UINT16 ViewZoneId)
{
    UINT32 Rval;
    UINT32 CrcHL, CrcSimilar;
    const UINT32 *pU32 = NULL;
    UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;
#ifdef CONFIG_ENABLE_DSP_MONITOR
    AMBA_DSP_MONITOR_CRC_CMPR_s CrcCmpr;
#endif

    Rval = FillVideoProcCfg(VidPrep, ViewZoneId);
    if (pIsSimilarEnabled != NULL) {
        if (1U == IsSimilarEnabled()) {
            UINT32 SIM_Rval;
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_vproc_cfg_t *SimVidPrep = SIM_DefCtxCmdBufPtrVpc;

            dsp_osal_typecast(&pU32, &VidPrep);
            CrcHL = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_vproc_cfg_t), 0U);

            SIM_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&SimVidPrep, &CmdBufferAddr);
            SIM_Rval = SIM_FillVideoProcCfg(SimVidPrep, ViewZoneId);
            if (SIM_Rval == OK) {
                dsp_osal_typecast(&pU32, &SimVidPrep);
                CrcSimilar = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_vproc_cfg_t), 0U);
#ifdef CONFIG_ENABLE_DSP_MONITOR
                CrcCmpr.ModuleId = AMBA_SSP_PRINT_MODULE_ID;
                CrcCmpr.Crc0 = CrcHL;
                CrcCmpr.Crc1 = CrcSimilar;
                CrcCmpr.UUID = __LINE__;
                SIM_Rval = dsp_mon_crc_cmpr(&CrcCmpr);
                if (SIM_Rval != OK) {
                    AmbaLL_LogUInt5("dsp_mon_crc_cmpr return 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
                }
#else
                if (CrcSimilar != CrcHL) {
                    AmbaLL_LogUInt5("[ERR] Similar CRC mismatch @ %d HL 0x%x Similar 0x%x", __LINE__, CrcHL, CrcSimilar, 0U, 0U);
                }
#endif
            } else {
                AmbaLL_LogUInt5("[ERR] Similar function returned NG 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
            }
            SIM_RelCmdBuffer(CmdBufferId);
        }
    }

    return Rval;
}

static inline void HL_FillVideoProcSetupVinInputFormat(const UINT8 InputFromMemory,
                                                       const CTX_VIN_INFO_s *pVinInfo,
                                                       cmd_vproc_setup_t *VprocSetup,
                                                       const UINT16 YuyvNumber)
{
    if (InputFromMemory != VIN_SRC_FROM_RECON) {
        if ((InputFromMemory == VIN_SRC_FROM_DRAM) ||
            (InputFromMemory == VIN_SRC_FROM_DRAM_422) ||
            (InputFromMemory == VIN_SRC_FROM_DRAM_420)) {
            if (InputFromMemory == VIN_SRC_FROM_DRAM_420) {
                VprocSetup->input_format = DSP_VPROC_INPUT_YUV_420;
            } else if (InputFromMemory == VIN_SRC_FROM_DRAM_422) {
                VprocSetup->input_format = DSP_VPROC_INPUT_YUV_422;
            } else {
                VprocSetup->input_format = DSP_VPROC_INPUT_RAW_RGB;
            }
        } else if (InputFromMemory == VIN_SRC_FROM_DEC) {
            VprocSetup->input_format = DSP_VPROC_INPUT_YUV_420;
        } else {
            if (YuyvNumber > 0U) {
                VprocSetup->input_format = DSP_VPROC_INPUT_MIPI_YUYV;
            } else {
                if (HL_IS_YUV420_INPUT(pVinInfo->InputFormat) == 1U) {
                    VprocSetup->input_format = DSP_VPROC_INPUT_YUV_420;
                } else if (HL_IS_YUV422_INPUT(pVinInfo->InputFormat) == 1U) {
                    VprocSetup->input_format = DSP_VPROC_INPUT_YUV_422;
                } else {
                    VprocSetup->input_format = DSP_VPROC_INPUT_RAW_RGB;
                }
            }
        }
    } else {
        VprocSetup->input_format = DSP_VPROC_INPUT_YUV_420;
    }

    if ((InputFromMemory == VIN_SRC_FROM_DEC) ||
        (InputFromMemory == VIN_SRC_FROM_RECON) ||
        (InputFromMemory == VIN_SRC_FROM_DRAM_422) ||
        (InputFromMemory == VIN_SRC_FROM_DRAM_420)) {
        VprocSetup->is_raw_compressed = 0U;
    } else {
        VprocSetup->is_raw_compressed = (pVinInfo->CfaCompressed > 0U)? (UINT8)1U: (UINT8)0U;
    }
}

static inline void HL_FillVideoProcSetupVin(cmd_vproc_setup_t *VprocSetup,
                                            const UINT16 ViewZoneId,
                                            const CTX_VIEWZONE_INFO_s *pViewZoneInfo)
{
    UINT16 VinId = 0U, YuyvNumber = 0U;
    UINT32 ViewZoneBitMask = 0U;
    CTX_VIN_INFO_s VinInfo;
    const UINT8 InputFromMemory = pViewZoneInfo->InputFromMemory;

    (void)dsp_osal_memset(&VinInfo, 0, sizeof(CTX_VIN_INFO_s));
    if ((InputFromMemory != VIN_SRC_FROM_RECON) &&
        (InputFromMemory != VIN_SRC_FROM_DEC)) {
        DSP_Bit2U16Idx(pViewZoneInfo->SourceVin, &VinId);
        HL_GetVinInfo(HL_MTX_OPT_ALL, VinId, &VinInfo);
    }

    VprocSetup->channel_id = (UINT8)ViewZoneId;

    DSP_SetBit(&ViewZoneBitMask, ViewZoneId);
    (void)HL_GetVprocMipiYuyvNumber(ViewZoneBitMask, &YuyvNumber);

    /* Vin Relative */
    HL_FillVideoProcSetupVinInputFormat(InputFromMemory, &VinInfo, VprocSetup, YuyvNumber);

    /* Input MuxSel */
    if (pViewZoneInfo->IsInputMuxSel > 0U) {
        VprocSetup->proc_hds_as_main = (pViewZoneInfo->InputMuxSelSrc > 0U)? (UINT8)1U: (UINT8)0U;
    } else {
        VprocSetup->proc_hds_as_main = (UINT8)0U;
    }

    /* HDR relative */
    if ((InputFromMemory != VIN_SRC_FROM_RECON) && (YuyvNumber == 0U)) {
        VprocSetup->is_ce_enabled = ((pViewZoneInfo->HdrBlendNumMinusOne > 0U) ||
                                     (pViewZoneInfo->LinearCe > 0U))?
                                     1U: 0U;
        VprocSetup->num_of_exp = (pViewZoneInfo->HdrBlendNumMinusOne > 0U) ? (UINT8)(pViewZoneInfo->HdrBlendNumMinusOne + 1U): 1U;
/* CV2FS uCode not impl yet */
        VprocSetup->is_hdr_blend_dram_out_enabled = 0U;
    } else {
        VprocSetup->is_ce_enabled = 0U;
/* CV2FS uCode not impl yet */
        VprocSetup->is_hdr_blend_dram_out_enabled = 0U;
        VprocSetup->num_of_exp = 1U;
    }

    /* Compressed out will output raw_comp, which can be treat a blended raw */
    VprocSetup->is_compressed_out_enabled = pViewZoneInfo->IsProcRawDramOut;

    /* C2Y relative, YuvInput start to support PG stat */
    if (InputFromMemory == VIN_SRC_FROM_HW) {
        VprocSetup->is_dzoom_enabled = (YuyvNumber > 0U)? 0U: 1U;
    } else {
        if (InputFromMemory == VIN_SRC_FROM_RECON) {
            VprocSetup->is_dzoom_enabled = 1U;
        } else {
            /* InputFromMemory == VIN_SRC_FROM_DEC could use dzoom */
            VprocSetup->is_dzoom_enabled = 1U;
        }
    }
}

static inline void HL_FillVideoProcSetupInputTail(cmd_vproc_setup_t *VprocSetup,
                                                  const UINT8 InputFromMemory,
                                                  const CTX_VIEWZONE_INFO_s *pViewZoneInfo)
{
    if (TuneDspSystemCfg.C2YBurstTile > 0U) {
        VprocSetup->is_c2y_burst_tiling_out = (UINT8)(TuneDspSystemCfg.C2YBurstTile - 1U);
    }

    if ((InputFromMemory == VIN_SRC_FROM_DEC) ||
        (InputFromMemory == VIN_SRC_FROM_RECON) ||
        (InputFromMemory == VIN_SRC_FROM_DRAM_422) ||
        (InputFromMemory == VIN_SRC_FROM_DRAM_420)) {
        //CV2FS dont support Warp2Prev
        VprocSetup->is_warp_dram_out_enabled = 0U;
    } else {
        if (0U == DSP_GetBit(pViewZoneInfo->FlowBypassOption, FLOW_BYPASS_MCTF_IDX, 1U)) {
            VprocSetup->is_warp_dram_out_enabled = 0U;
        } else {
            VprocSetup->is_warp_dram_out_enabled = 1U;
        }
    }
    if (TuneDspSystemCfg.WarpDramOut > 0U) {
        VprocSetup->is_warp_dram_out_enabled = (UINT8)(TuneDspSystemCfg.WarpDramOut - 1U);
    }

    VprocSetup->is_fast_y2y_en = 0U; //TBD when MultiFovYuvInput
    if (pViewZoneInfo->IsEffectChanMember == 1U) {
        VprocSetup->is_pic_info_cache_enabled = 1U;
    } else {
        VprocSetup->is_pic_info_cache_enabled = 0U;
    }
}

static inline void HL_FillVideoProcSetupInputHalf(cmd_vproc_setup_t *VprocSetup,
                                                  const UINT8 InputFromMemory,
                                                  const UINT16 ViewZoneId,
                                                  const UINT16 YuyvNumber,
                                                  const CTX_VIEWZONE_INFO_s *pViewZoneInfo)
{
    UINT8 Smem2WarpEnable = (UINT8)HL_GetViewZoneSmemToWarpEnable(ViewZoneId);
    UINT8 Dram2WarpEnable = (UINT8)HL_GetViewZoneDramToWarpEnable(ViewZoneId);

    if (InputFromMemory != VIN_SRC_FROM_HW) {
        VprocSetup->is_c2y_burst_tiling_out = 0U; //Default OFF
    } else {
        if ((pViewZoneInfo->SliceNumRow > 1U) ||
            (YuyvNumber > 0U)) {
            VprocSetup->is_c2y_burst_tiling_out = 0U;
        } else {

            if (VprocSetup->is_tile_mode > (UINT8)0U) {
                /* When Smem2Warp, c2y burst tile is useless since there is no C2Y DramOut */
                if ((0U == Dram2WarpEnable) && (1U == Smem2WarpEnable)) {
                    VprocSetup->is_c2y_burst_tiling_out = 0U;
                } else {
                    VprocSetup->is_c2y_burst_tiling_out = 1U;
                }
            } else {
                /*
                 * 2020/4/13 When FrmMode, enable c2y burst tile need,
                 *  - extra 8 line in LumaMainResamplerOut
                 *  - extra 8 line in ChromaMainResamplerOut
                 *  - extra 8 line in ChromaUpsamplerOut
                 * so we disable c2y burst tile in frm mode, due to IK didn't have the information about burt tile now
                 * But when tile mode, dsp will modify above itself
                 */
                VprocSetup->is_c2y_burst_tiling_out = 0U;
            }
        }
    }
}

static inline void HL_FillVideoProcSetupInput(cmd_vproc_setup_t *VprocSetup,
                                              const UINT16 ViewZoneId,
                                              const CTX_VIEWZONE_INFO_s *pViewZoneInfo)
{
    UINT16 YuyvNumber = 0U;
    UINT32 ViewZoneBitMask = 0U;
    const UINT8 InputFromMemory = pViewZoneInfo->InputFromMemory;

    DSP_SetBit(&ViewZoneBitMask, ViewZoneId);
    (void)HL_GetVprocMipiYuyvNumber(ViewZoneBitMask, &YuyvNumber);

    if ((InputFromMemory == VIN_SRC_FROM_DEC) ||
        (YuyvNumber > 0U)) {
        VprocSetup->is_aaa_enabled = 0U;
    } else {
        VprocSetup->is_aaa_enabled = 1U;
    }
    if (TuneDspSystemCfg.AAAEnable > 0U) {
        VprocSetup->is_aaa_enabled = (UINT8)(TuneDspSystemCfg.AAAEnable - 1U);
    }

    if ((InputFromMemory == VIN_SRC_FROM_RECON) ||
        (YuyvNumber > 0U)) {
        VprocSetup->is_li_enabled = 0U;
        VprocSetup->is_sbp_enabled = 0U;
    } else if (InputFromMemory == VIN_SRC_FROM_DEC) {
        VprocSetup->is_li_enabled = 1U;
        VprocSetup->is_sbp_enabled = 0U;
    } else {    //(InputFromMemory == VIN_SRC_FROM_DEC)
        VprocSetup->is_li_enabled = 1U;
        VprocSetup->is_sbp_enabled = 1U;
    }

    if (TuneDspSystemCfg.LiEnable > 0U) {
        VprocSetup->is_li_enabled = (UINT8)(TuneDspSystemCfg.LiEnable - 1U);
    }

    HL_FillVideoProcSetupInputHalf(VprocSetup,
                                   InputFromMemory,
                                   ViewZoneId,
                                   YuyvNumber,
                                   pViewZoneInfo);
    HL_FillVideoProcSetupInputTail(VprocSetup,
                                   InputFromMemory,
                                   pViewZoneInfo);
}

static inline void HL_FillVideoProcSetupTile(cmd_vproc_setup_t *VprocSetup,
                                             const UINT16 ViewZoneId,
                                             const CTX_VIEWZONE_INFO_s *pViewZoneInfo,
                                             UINT8 *pIntSliceNumCol)
{
    UINT8 IsTileMode = 0U;
    UINT16 YuyvNumber = 0U;
    UINT32 ViewZoneBitMask = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VPROC_INFO_s VprocInfo = {0};
    UINT8 C2YTileX = 0U, WarpTileX = 0U;

    DSP_SetBit(&ViewZoneBitMask, ViewZoneId);
    (void)HL_GetVprocMipiYuyvNumber(ViewZoneBitMask, &YuyvNumber);

    HL_GetResourcePtr(&Resource);
    HL_GetVprocInfo(HL_MTX_OPT_ALL, ViewZoneId, &VprocInfo);

    VprocSetup->W_main = pViewZoneInfo->Main.Width;
    VprocSetup->H_main = ALIGN_NUM16(pViewZoneInfo->Main.Height, 16U);

    /* use tile mode if anyone need it */
    (void)HL_IsTileMode(&IsTileMode);
    VprocSetup->is_tile_mode = IsTileMode;

    HL_GetVideoProcTileX(IsTileMode, ViewZoneId, pViewZoneInfo, Resource, &C2YTileX, &WarpTileX);

    /* currently only care TileX only */
    if (VprocSetup->is_tile_mode > 0U) {
        VprocSetup->c2y_tile_num_x = C2YTileX;
        *pIntSliceNumCol = C2YTileX;
        VprocSetup->c2y_tile_num_y = 1U;
        VprocSetup->warp_tile_num_x = WarpTileX;
        VprocSetup->warp_tile_num_y = 1U;

        if (pViewZoneInfo->SliceNumRow > 1U) {
            VprocSetup->c2y_tile_num_y = pViewZoneInfo->SliceNumRow;
            VprocSetup->warp_tile_num_y = pViewZoneInfo->SliceNumRow;
            VprocSetup->is_ldly_enabled = (UINT8)1U;
            VprocSetup->is_c2y_dram_sync_to_warp = (UINT8)1U;
        }

        if (pViewZoneInfo->WarpLumaWaitLine == 0U) {
            VprocSetup->warp_wait_lines = (0U == HL_GetViewZoneSmemToWarpEnable(ViewZoneId))? (UINT16)CV2X_WARP_WAITLINE: (UINT16)CV2X_WARP_SMEM2WARP_WAITLINE;
        } else {
            VprocSetup->warp_wait_lines = pViewZoneInfo->WarpLumaWaitLine;
        }

        /* Slice Config */
#ifdef SUPPORT_DSP_LDY_SLICE
        if (pViewZoneInfo->SliceNumRow > 1U) {
            DSP_SLICE_LAYOUT_s *pSliceLayout = NULL;

            DSP_GetDspVprocSliceLayoutBuf(ViewZoneId, &pSliceLayout);
            dsp_osal_typecast(&ULAddr, &pSliceLayout);

            (void)HL_FillSliceLayoutCfg((UINT16)VprocSetup->c2y_tile_num_x,
                                        (UINT16)VprocSetup->c2y_tile_num_y,
                                        VprocSetup->W_main,
                                        VprocSetup->H_main,
                                        pSliceLayout);
            (void)dsp_osal_virt2cli(ULAddr, &VprocSetup->slice_layout_ptr);
            VprocSetup->slice_layout_usize = ALIGN_NUM(sizeof(DSP_SLICE_LAYOUT_s), 64U);
        } else {
            VprocSetup->slice_layout_ptr = 0U;
            VprocSetup->slice_layout_usize = 0U;
        }
#endif

//        if ((VprocSetup->is_tile_mode > 0U) &&
//            (VprocSetup->warp_tile_num_y == 1U)) {
            VprocSetup->aaa_cfa_mux_sel = 1U; //default from BadPixel
//        } else {
//            VprocSetup->aaa_cfa_mux_sel = 1U;
//        }
    } else {
        VprocSetup->c2y_tile_num_x = C2YTileX;
        *pIntSliceNumCol = VprocSetup->c2y_tile_num_x;
        VprocSetup->c2y_tile_num_y = 1U;
        VprocSetup->warp_tile_num_x = WarpTileX;
        VprocSetup->warp_tile_num_y = 1U;
        VprocSetup->aaa_cfa_mux_sel = 1U; //default from BadPixel

        if (pViewZoneInfo->WarpLumaWaitLine == 0U) {
            VprocSetup->warp_wait_lines = (0U == HL_GetViewZoneSmemToWarpEnable(ViewZoneId))? (UINT16)CV2X_WARP_WAITLINE: (UINT16)CV2X_WARP_SMEM2WARP_WAITLINE;
        } else {
            VprocSetup->warp_wait_lines = pViewZoneInfo->WarpLumaWaitLine;
        }
    }

    /* AAA Frm Alt */
    VprocSetup->is_frame_alternate = (pViewZoneInfo->IkSensorMode == DSP_IK_SNSR_MODE_RGB_IR)? (UINT8)1U: (UINT8)0U;
    VprocSetup->aaa_cfa_mux_sel = (pViewZoneInfo->IkSensorMode == DSP_IK_SNSR_MODE_RGB_IR)? (UINT8)2U: (UINT8)1U; // RgbIR case : EvenFrm from Vig, OddFrm from BadPixel
}

static inline void HL_FillVideoProcSetupGetChFmt(const UINT16 ViewZoneId,
                                                 const UINT16 VprocPin,
                                                 UINT8 *ChFmt)
{
    UINT8 DestChFmt = DSP_YUV_420;
    UINT16 YuvStrmIdx = 0U;
    CTX_VPROC_INFO_s VprocInfo = {0};
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;

    HL_GetVprocInfo(HL_MTX_OPT_ALL, ViewZoneId, &VprocInfo);
    if (VprocInfo.PinUsage[VprocPin] > 0U) {
        DSP_Bit2U16Idx(VprocInfo.PinUsage[VprocPin], &YuvStrmIdx);
        HL_GetYuvStrmInfoLock(YuvStrmIdx, &YuvStrmInfo);
        if (1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U)) {
            DestChFmt = YuvFmtTable[YuvStrmInfo->YuvBuf.DataFmt];
        }
        HL_GetYuvStrmInfoUnLock(YuvStrmIdx);
    }
    *ChFmt = DestChFmt;
}

static inline UINT16 HL_FillVideoProcPrevW(const CTX_VIEWZONE_INFO_s *pViewZoneInfo, const UINT8 Prev)
{
    return ((pViewZoneInfo->PinWindow[Prev].Width == 0U) &&
            (pViewZoneInfo->PinWindow[Prev].Height == 0U) &&
            (pViewZoneInfo->PinMaxWindow[Prev].Width != 0U) &&
            (pViewZoneInfo->PinMaxWindow[Prev].Height != 0U)) ?
                    pViewZoneInfo->PinMaxWindow[Prev].Width :
                    pViewZoneInfo->PinWindow[Prev].Width;
}

static inline UINT16 HL_FillVideoProcPrevH(const CTX_VIEWZONE_INFO_s *pViewZoneInfo, const UINT8 Prev)
{
    return ((pViewZoneInfo->PinWindow[Prev].Width == 0U) &&
            (pViewZoneInfo->PinWindow[Prev].Height == 0U) &&
            (pViewZoneInfo->PinMaxWindow[Prev].Width != 0U) &&
            (pViewZoneInfo->PinMaxWindow[Prev].Height != 0U)) ?
                    pViewZoneInfo->PinMaxWindow[Prev].Height :
                    pViewZoneInfo->PinWindow[Prev].Height;
}

static inline void HL_FillVideoProcPrevEna(cmd_vproc_setup_t *VprocSetup)
{
    VprocSetup->is_prev_a_enabled = ((VprocSetup->prev_a_w > 0U) && (VprocSetup->prev_a_h > 0U))? 1U: 0U;
    VprocSetup->is_prev_b_enabled = ((VprocSetup->prev_b_w > 0U) && (VprocSetup->prev_b_h > 0U))? 1U: 0U;
    VprocSetup->is_prev_c_enabled = ((VprocSetup->prev_c_w > 0U) && (VprocSetup->prev_c_h > 0U))? 1U: 0U;
}

#ifdef SUPPORT_DSP_PREVB_MASTER_MODE
static inline void HL_FillVideoProcPrevBChk(cmd_vproc_setup_t *VprocSetup)
{
    if ((VprocSetup->prev_b_w == 0U) && ((VprocSetup->prev_a_w != 0U) ||
                                         (VprocSetup->prev_c_w != 0U) ||
                                         (VprocSetup->hier_poly_w != 0U))) {
        VprocSetup->prev_b_w = 640U;
    } else {
        //
    }
    if ((VprocSetup->prev_b_h == 0U) && ((VprocSetup->prev_a_h != 0U) ||
                                         (VprocSetup->prev_c_h != 0U) ||
                                         (VprocSetup->hier_poly_h != 0U))) {
        VprocSetup->prev_b_h = 480U;
    } else {
        //
    }
}
#endif
static inline void HL_FillVideoProcSetupPip(cmd_vproc_setup_t *VprocSetup,
                                            const UINT16 ViewZoneId,
                                            const CTX_VIEWZONE_INFO_s *pViewZoneInfo)
{
    UINT16 PrevDest = AMBA_DSP_PREV_DEST_DUMMY;
    UINT8 DestChFmt = DSP_YUV_420;
    UINT8 MctfEnable = 0U, MctsEnable = 0U, MctsOutEnable = 0U, MctfCmpr = 0U;
    UINT16 TileOverLapX = 0U;
    UINT16 TileOverLapY = 0U;

    /* Pipe */
    if (1U == HL_GetViewZoneSmemToWarpEnable(ViewZoneId)) {
        VprocSetup->is_c2y_smem_sync_to_warp = (UINT8)1U;
        VprocSetup->is_c2y_dram_out_enabled = 0U;
    } else {
        VprocSetup->is_c2y_smem_sync_to_warp = (UINT8)0U;
        VprocSetup->is_c2y_dram_out_enabled = 1U;
    }
    if (TuneDspSystemCfg.C2YDramOut > 0U) {
        VprocSetup->is_c2y_dram_out_enabled = (UINT8)(TuneDspSystemCfg.C2YDramOut - 1U);
    }

    /* Warp relative */
    VprocSetup->is_warp_enabled = 1U; //Default ON
    (void)HL_GetViewZoneWarpOverlap(ViewZoneId, &TileOverLapX, &TileOverLapY);
    VprocSetup->warp_tile_overlap_x = TileOverLapX;
    VprocSetup->warp_tile_overlap_y = TileOverLapY;

    /* Mctf relative */
    (void)HL_GetViewZoneMctfEnable(ViewZoneId, &MctfEnable, &MctsEnable, &MctsOutEnable, &MctfCmpr);
    VprocSetup->is_mctf_enabled = MctfEnable;
    VprocSetup->is_mcts_disabled = (MctsEnable > 0U)? (UINT8)0U: (UINT8)1U;
    VprocSetup->is_mcts_dram_out_enabled = MctsOutEnable;
    VprocSetup->is_mctf_cmpr_en = MctfCmpr;
    VprocSetup->mctf_mode = VPROC_MCTF_MODE_FILTER;

    /* Prev filter relative */
    // Per Max(2015/08/10), keep PrevA/B/C alive even App dont want it
    // just set it as small window (like WQVGA),
    VprocSetup->prev_mode = 0U; //DramPreview

    HL_FillVideoProcSetupGetChFmt(ViewZoneId, DSP_VPROC_PIN_PREVA, &DestChFmt);
    VprocSetup->prev_a_ch_fmt = DestChFmt;
    HL_FillVideoProcSetupGetChFmt(ViewZoneId, DSP_VPROC_PIN_PREVB, &DestChFmt);
    VprocSetup->prev_b_ch_fmt = DestChFmt;
    HL_FillVideoProcSetupGetChFmt(ViewZoneId, DSP_VPROC_PIN_PREVC, &DestChFmt);
    VprocSetup->prev_c_ch_fmt = DestChFmt;

    VprocSetup->prev_a_format = VPROC_PREV_FMT_PROG; //TBD
    VprocSetup->prev_c_format = VPROC_PREV_FMT_PROG; //TBD

    VprocSetup->prev_a_src = VPROC_PREV_SRC_DRAM;
    VprocSetup->prev_c_src = VPROC_PREV_SRC_DRAM;

    (void)HL_GetViewZonePrevDest(ViewZoneId, DSP_VPROC_PIN_PREVA, &PrevDest);
    VprocSetup->prev_a_dst = (UINT8)PrevDest;
    (void)HL_GetViewZonePrevDest(ViewZoneId, DSP_VPROC_PIN_PREVB, &PrevDest);
    VprocSetup->prev_b_dst = (UINT8)PrevDest;
    (void)HL_GetViewZonePrevDest(ViewZoneId, DSP_VPROC_PIN_PREVC, &PrevDest);
    VprocSetup->prev_c_dst = (UINT8)PrevDest;

    VprocSetup->prev_a_frame_rate = 0U; //TBD
    VprocSetup->prev_b_frame_rate = 0U; //TBD
    VprocSetup->prev_c_frame_rate = 0U; //TBD

    VprocSetup->prev_a_w = HL_FillVideoProcPrevW(pViewZoneInfo, DSP_VPROC_PIN_PREVA);
    VprocSetup->prev_a_h = HL_FillVideoProcPrevH(pViewZoneInfo, DSP_VPROC_PIN_PREVA);
    VprocSetup->prev_b_w = HL_FillVideoProcPrevW(pViewZoneInfo, DSP_VPROC_PIN_PREVB);
    VprocSetup->prev_b_h = HL_FillVideoProcPrevH(pViewZoneInfo, DSP_VPROC_PIN_PREVB);
    VprocSetup->prev_c_w = HL_FillVideoProcPrevW(pViewZoneInfo, DSP_VPROC_PIN_PREVC);
    VprocSetup->prev_c_h = HL_FillVideoProcPrevH(pViewZoneInfo, DSP_VPROC_PIN_PREVC);

#ifdef SUPPORT_DSP_PREVB_MASTER_MODE
    HL_FillVideoProcPrevBChk(VprocSetup);
#endif
    HL_FillVideoProcPrevEna(VprocSetup);

    /* C2Y relative */
    TileOverLapX = 0U;
    (void)HL_GetViewZoneRawOverlap(ViewZoneId, &TileOverLapX);
    VprocSetup->raw_tile_overlap_x = (UINT16)DSP_GetU16Bit(TileOverLapX, 0U, 8U);
}

static inline void HL_FillVideoProcSetupPymdLndt(cmd_vproc_setup_t *VprocSetup,
                                                 UINT16 *pExtBufMask,
                                                 const UINT16 ViewZoneId,
                                                 const CTX_VIEWZONE_INFO_s *pViewZoneInfo)
{
    UINT16 ExtBufMask = *pExtBufMask;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT16 ExtHierMemBitIdx;

    /* Pymd relative */
    /*
     * Dsp can't support run-time allocate Pymd resource,
     * if user want output Pymd in the middle of Liveview.
     *  - Set MaxHierWin
     *  - Set HierEnable
     *  - Set HierBitMask = 0 (turn on it later when user want)
     */
    HL_GetResourcePtr(&Resource);
    VprocSetup->is_hier_poly_sqrt2 = (UINT8)DSP_GetU16Bit(pViewZoneInfo->Pyramid.IsPolySqrt2, DSP_PYMD_PLOY_IDX, DSP_PYMD_PLOY_LEN);
    if (DSP_GetU16Bit(pViewZoneInfo->Pyramid.IsPolySqrt2, DSP_PYMD_Y12_IDX, DSP_PYMD_Y12_LEN) > 0U) {
        VprocSetup->is_y12_out_enabled = 1U;
        ExtHierMemBitIdx = DSP_BIT_POS_EXT_HIER_Y12;
    } else {
        ExtHierMemBitIdx = DSP_BIT_POS_EXT_HIER_0;
    }

    VprocSetup->hier_poly_w = pViewZoneInfo->Pyramid.HierPolyWidth;
    VprocSetup->hier_poly_h = pViewZoneInfo->Pyramid.HierPolyHeight;

    if ((Resource->MaxHierWidth[ViewZoneId] > 0U) && (Resource->MaxHierHeight[ViewZoneId] > 0U)) {
        VprocSetup->is_prev_hier_enabled = 1U;
        if ((pViewZoneInfo->PymdAllocType == ALLOC_EXTERNAL_DISTINCT) ||
            (pViewZoneInfo->PymdAllocType == ALLOC_EXTERNAL_CYCLIC)) {
            DSP_SetU16Bit(&ExtBufMask, ExtHierMemBitIdx);
        } else {
            DSP_ClearU16Bit(&ExtBufMask, ExtHierMemBitIdx);
        }
    } else {
        VprocSetup->is_prev_hier_enabled = 0U;
        DSP_ClearU16Bit(&ExtBufMask, ExtHierMemBitIdx);
    }

    VprocSetup->is_hier_burst_tiling_out = 0U; //Default OFF
    VprocSetup->hier_poly_tile_overlap_x = 0U; //Default ?

    /* Lndt relative */
    VprocSetup->prev_ln_w = pViewZoneInfo->LaneDetect.Width;
    VprocSetup->prev_ln_h = pViewZoneInfo->LaneDetect.Height;
    VprocSetup->ln_det_src = (UINT8)pViewZoneInfo->LaneDetect.HierSource;
    if ((VprocSetup->prev_ln_w > 0U) && (VprocSetup->prev_ln_h > 0U)) {
        VprocSetup->is_prev_ln_enabled = 1U;
        if ((pViewZoneInfo->LndtAllocType == ALLOC_EXTERNAL_DISTINCT) ||
            (pViewZoneInfo->LndtAllocType == ALLOC_EXTERNAL_CYCLIC)) {
            DSP_SetU16Bit(&ExtBufMask, DSP_BIT_POS_EXT_LN_DEC);
        } else {
            DSP_ClearU16Bit(&ExtBufMask, DSP_BIT_POS_EXT_LN_DEC);
        }
    } else {
        VprocSetup->is_prev_ln_enabled = 0U;
        DSP_ClearU16Bit(&ExtBufMask, DSP_BIT_POS_EXT_LN_DEC);
    }

    /* MainY12 relative */
    if (pViewZoneInfo->MainY12Enable > 0U) {
        VprocSetup->is_c2y_y12_out_enabled = (VprocSetup->is_y12_out_enabled == 1U)? 0U: 1U;
        if ((pViewZoneInfo->MainY12AllocType == ALLOC_EXTERNAL_DISTINCT) ||
            (pViewZoneInfo->MainY12AllocType == ALLOC_EXTERNAL_CYCLIC)) {
            DSP_SetU16Bit(&ExtBufMask, DSP_BIT_POS_EXT_C2Y_Y12);
        } else {
            DSP_ClearU16Bit(&ExtBufMask, DSP_BIT_POS_EXT_C2Y_Y12);
        }
    } else {
        VprocSetup->is_c2y_y12_out_enabled = 0U;
        DSP_ClearU16Bit(&ExtBufMask, DSP_BIT_POS_EXT_C2Y_Y12);
    }


    *pExtBufMask = ExtBufMask;
}

#ifdef SUPPORT_DSP_EXT_PIN_BUF
static inline void HL_FillVideoProcSetupContentExtMemDesc(ext_mem_desc_t *pExtMemDesc,
                                                          const UINT16 ViewZoneId,
                                                          const CTX_VIEWZONE_INFO_s *pViewZoneInfo,
                                                          UINT32 DummyExtMemPool)
{
    UINT16 i;
    UINT16 Num = 0U, AuxNum = 0U;

    for (i = 0U; i < MAX_VPROC_EXT_MEM_TYPE_NUM; i++) {
        if (1U == DSP_GetBit(DummyExtMemPool, pExtMemDesc[i].memory_type, 1U)) {
            pExtMemDesc[i].max_daddr_slot = (UINT8)CV2X_MAX_C2Y_FB_NUM;
        } else {
            if ((pExtMemDesc[i].memory_type == VPROC_EXT_MEM_TYPE_MAIN) ||
                (pExtMemDesc[i].memory_type == VPROC_EXT_MEM_TYPE_MAIN_ME)) {
                (void)HL_GetViewZonePinFbNum(ViewZoneId, DSP_VPROC_PIN_MAIN, &Num, &AuxNum, 1U);
                pExtMemDesc[i].max_daddr_slot = (UINT8)Num;
            } else if ((pExtMemDesc[i].memory_type == VPROC_EXT_MEM_TYPE_PREV_A) ||
                       (pExtMemDesc[i].memory_type == VPROC_EXT_MEM_TYPE_PIP_ME)) {
                (void)HL_GetViewZonePinFbNum(ViewZoneId, DSP_VPROC_PIN_PREVA, &Num, &AuxNum, 1U);
                pExtMemDesc[i].max_daddr_slot = (UINT8)Num;
            } else if (pExtMemDesc[i].memory_type == VPROC_EXT_MEM_TYPE_PREV_B) {
                (void)HL_GetViewZonePinFbNum(ViewZoneId, DSP_VPROC_PIN_PREVB, &Num, &AuxNum, 1U);
                pExtMemDesc[i].max_daddr_slot = (UINT8)Num;
            } else if (pExtMemDesc[i].memory_type == VPROC_EXT_MEM_TYPE_PREV_C) {
                (void)HL_GetViewZonePinFbNum(ViewZoneId, DSP_VPROC_PIN_PREVC, &Num, &AuxNum, 1U);
                pExtMemDesc[i].max_daddr_slot = (UINT8)Num;
            } else if (pExtMemDesc[i].memory_type == VPROC_EXT_MEM_TYPE_LN_DEC) {
                if (pViewZoneInfo->LndtBufNum == 0U) {
                    pExtMemDesc[i].max_daddr_slot = (UINT8)CV2X_DEFAULT_LNDT_FB_NUM;
                } else {
                    pExtMemDesc[i].max_daddr_slot = (UINT8)pViewZoneInfo->LndtBufNum;
                }
            } else if ((pExtMemDesc[i].memory_type == VPROC_EXT_MEM_TYPE_HIER_0) ||
                       (pExtMemDesc[i].memory_type == VPROC_EXT_MEM_TYPE_HIER_Y12)) {
                if (pViewZoneInfo->PymdBufNum == 0U) {
                    pExtMemDesc[i].max_daddr_slot = (UINT8)CV2X_DEFAULT_HIER_FB_NUM;
                } else {
                    pExtMemDesc[i].max_daddr_slot = (UINT8)pViewZoneInfo->PymdBufNum;
                }
            } else if (pExtMemDesc[i].memory_type == VPROC_EXT_MEM_TYPE_COMP_RAW) {
    //FIXME, current only support DebugUsage and allow ONE buffer
                pExtMemDesc[VPROC_EXT_MEM_TYPE_COMP_RAW].max_daddr_slot = (UINT8)DEFAULT_EXT_MEM_DADDR_NUM;
            } else if (pExtMemDesc[i].memory_type == VPROC_EXT_MEM_TYPE_C2Y_Y12) {
                if (pViewZoneInfo->MainY12BufNum == 0U) {
                    pExtMemDesc[i].max_daddr_slot = (UINT8)CV2X_MAX_C2Y_FB_NUM;
                } else {
                    pExtMemDesc[i].max_daddr_slot = (UINT8)pViewZoneInfo->MainY12BufNum;
                }
            } else {
                break;
            }
        }
    }
}

static inline void HL_FillVideoProcSetupExtMemDesc(ULONG *pULAddr,
                                                   const UINT16 *pExtBufMask,
                                                   const UINT16 ViewZoneId,
                                                   const CTX_VIEWZONE_INFO_s *pViewZoneInfo,
                                                   UINT16 *pExtMemNum)
{
    UINT8 IsExtMemMain, IsExtMemMctf, IsExtMemMcts;
    UINT16 i;
    UINT16 ExtBufMask = *pExtBufMask;
    ext_mem_desc_t *pExtMemDesc = NULL;
    UINT16 Idx = 0U;
    UINT32 DummyExtMemPool = 0U; //bit-wise, indicate we reserve min pool number for CapToExtMem feature

    DSP_GetDspVprocExtMemDescBuf(ViewZoneId, &pExtMemDesc);

    for (i = 0U; i<DSP_BIT_POS_EXT_NUM; i++) {
//FIXME, HL not support yet
        if ((i == DSP_BIT_POS_EXT_PREV_B_ME) ||
            (i == DSP_BIT_POS_EXT_PREV_C_ME) ||
            (i == DSP_BIT_POS_EXT_MCTS) ||
            (i == DSP_BIT_POS_EXT_MCTF)) {
            continue;
        }

        if (i == DSP_BIT_POS_EXT_MAIN) {
            /* Main/Mcts/Mctf share same physical buffer */
            IsExtMemMain = (1U == DSP_GetU16Bit(ExtBufMask, DSP_BIT_POS_EXT_MAIN, 1U))? (UINT8)1U: (UINT8)0U;
            IsExtMemMctf = (1U == DSP_GetU16Bit(ExtBufMask, DSP_BIT_POS_EXT_MCTF, 1U))? (UINT8)1U: (UINT8)0U;
            IsExtMemMcts = (1U == DSP_GetU16Bit(ExtBufMask, DSP_BIT_POS_EXT_MCTS, 1U))? (UINT8)1U: (UINT8)0U;

            if ((IsExtMemMain > (UINT8)0U) || (IsExtMemMctf > (UINT8)0U) || (IsExtMemMcts > (UINT8)0U)) {
                DSP_ClearBit(&DummyExtMemPool, HL_VprocMemBitMemTypeMap[i]);
            } else {
                DSP_SetBit(&DummyExtMemPool, HL_VprocMemBitMemTypeMap[i]);
            }
            pExtMemDesc[Idx].memory_type = (UINT8)HL_VprocMemBitMemTypeMap[i];
            Idx++;
        } else if (i == DSP_BIT_POS_EXT_COMP_RAW) {
            if (pViewZoneInfo->IsProcRawDramOut > (UINT8)0U) {
                pExtMemDesc[Idx].memory_type = (UINT8)HL_VprocMemBitMemTypeMap[i];
                Idx++;
            }
        } else {
            if (1U == DSP_GetU16Bit(ExtBufMask, i, 1U)) {
                DSP_ClearBit(&DummyExtMemPool, HL_VprocMemBitMemTypeMap[i]);
            } else {
                DSP_SetBit(&DummyExtMemPool, HL_VprocMemBitMemTypeMap[i]);
            }
            pExtMemDesc[Idx].memory_type = (UINT8)HL_VprocMemBitMemTypeMap[i];
            Idx++;
        }
    }

    HL_FillVideoProcSetupContentExtMemDesc(pExtMemDesc, ViewZoneId, pViewZoneInfo, DummyExtMemPool);

    dsp_osal_typecast(pULAddr, &pExtMemDesc);
    *pExtMemNum = Idx;
}
#endif

static inline void HL_FillVideoProcSetupExtMem(cmd_vproc_setup_t *VprocSetup,
                                               UINT16 *pExtBufMask,
                                               const UINT16 ViewZoneId,
                                               const CTX_VIEWZONE_INFO_s *pViewZoneInfo)
{
    UINT16 ExtBufMask = *pExtBufMask;
#ifdef SUPPORT_VPROC_DLY
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT32 AudClk = 0U;
#endif
#ifdef SUPPORT_DSP_EXT_PIN_BUF
    UINT16 i;
    ULONG ULAddr;
    UINT16 ExtMemNum = 0U;

    /* ExtMem */
    for (i = 0U; i<DSP_VPROC_PIN_NUM; i++) {
        if (1U == DSP_GetU8Bit(pViewZoneInfo->PinIsExtMem, i, 1U)) {
            DSP_SetU16Bit(&ExtBufMask, DSP_VprocPinVprocMemTypeMap[i]);
            // Let uCode to handle ME buffer
        } else {
            DSP_ClearU16Bit(&ExtBufMask, DSP_VprocPinVprocMemTypeMap[i]);
        }
    }

    HL_FillVideoProcSetupExtMemDesc(&ULAddr,
                                    &ExtBufMask,
                                    ViewZoneId,
                                    pViewZoneInfo,
                                    &ExtMemNum);
    (void)dsp_osal_virt2cli(ULAddr, &VprocSetup->ext_mem_cfg_addr);
    VprocSetup->ext_mem_cfg_num = (UINT8)ExtMemNum;
#endif
    VprocSetup->ext_buf_mask = ExtBufMask;

    /* Misc. */
#ifdef SUPPORT_VPROC_DLY
    HL_GetResourcePtr(&Resource);
    AudClk = Resource->AudioClk/10000U;
    VprocSetup->frm_delay_time_ticks = pViewZoneInfo->ProcPostPoneTime[AMBA_DSP_VZ_POSTPONE_STAGE_PRE_R2Y]*AudClk;
#endif
    VprocSetup->pts_delta = 0U;

    *pExtBufMask = ExtBufMask;
}

static UINT32 FillVideoProcSetup(cmd_vproc_setup_t *VprocSetup, UINT16 ViewZoneId)
{
    UINT32 Rval = OK;
    UINT16 ExtBufMask = 0U;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT8 IntSliceNumCol = 0U;
    UINT16 AaaMuxSel = 0U;

    if (VprocSetup == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);

        HL_FillVideoProcSetupVin(VprocSetup,
                                 ViewZoneId,
                                 ViewZoneInfo);
        HL_FillVideoProcSetupTile(VprocSetup,
                                  ViewZoneId,
                                  ViewZoneInfo,
                                  &IntSliceNumCol);
        HL_FillVideoProcSetupInput(VprocSetup,
                                   ViewZoneId,
                                   ViewZoneInfo);
        /* Update IntSliceNumCol */
        HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
        ViewZoneInfo->IntSliceNumCol = IntSliceNumCol;
        HL_GetViewZoneInfoUnLock(ViewZoneId);

        /* Pymd will affect warp overlap y, so invoke this one first */
        HL_FillVideoProcSetupPymdLndt(VprocSetup,
                                      &ExtBufMask,
                                      ViewZoneId,
                                      ViewZoneInfo);
        HL_FillVideoProcSetupPip(VprocSetup,
                                 ViewZoneId,
                                 ViewZoneInfo);
        HL_FillVideoProcSetupExtMem(VprocSetup,
                                    &ExtBufMask,
                                    ViewZoneId,
                                    ViewZoneInfo);

        /* Update ViewZoneInfo */
        HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
        if (VprocSetup->is_frame_alternate > 0U) {
            DSP_SetU16Bit(&AaaMuxSel, VZ_AAA_MUXSEL_ENB_BIT_IDX);
            AaaMuxSel |= VprocSetup->aaa_cfa_mux_sel;
        }
        ViewZoneInfo->AaaMuxSel = AaaMuxSel;
        HL_GetViewZoneInfoUnLock(ViewZoneId);
    }

    return Rval;
}
UINT32 HL_FillVideoProcSetup(cmd_vproc_setup_t *VprocSetup, UINT16 ViewZoneId)
{
    UINT32 Rval;
    UINT32 CrcHL, CrcSimilar;
    const UINT32 *pU32 = NULL;
    UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;
#ifdef CONFIG_ENABLE_DSP_MONITOR
    AMBA_DSP_MONITOR_CRC_CMPR_s CrcCmpr;
#endif

    Rval = FillVideoProcSetup(VprocSetup, ViewZoneId);
    if (pIsSimilarEnabled != NULL) {
        if (1U == IsSimilarEnabled()) {
            UINT32 SIM_Rval;
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_vproc_setup_t *SimVprocSetup = SIM_DefCtxCmdBufPtrVpcSetup;

            dsp_osal_typecast(&pU32, &VprocSetup);
            CrcHL = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_vproc_setup_t), 0U);

            SIM_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&SimVprocSetup, &CmdBufferAddr);
            SIM_Rval = SIM_FillVideoProcSetup(SimVprocSetup, ViewZoneId);
            if (SIM_Rval == OK) {
                dsp_osal_typecast(&pU32, &SimVprocSetup);
                CrcSimilar = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_vproc_setup_t), 0U);
#ifdef CONFIG_ENABLE_DSP_MONITOR
                CrcCmpr.ModuleId = AMBA_SSP_PRINT_MODULE_ID;
                CrcCmpr.Crc0 = CrcHL;
                CrcCmpr.Crc1 = CrcSimilar;
                CrcCmpr.UUID = __LINE__;
                SIM_Rval = dsp_mon_crc_cmpr(&CrcCmpr);
                if (SIM_Rval != OK) {
                    AmbaLL_LogUInt5("dsp_mon_crc_cmpr return 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
                }
#else
                if (CrcSimilar != CrcHL) {
                    AmbaLL_LogUInt5("[ERR] Similar CRC mismatch @ %d HL 0x%x Similar 0x%x", __LINE__, CrcHL, CrcSimilar, 0U, 0U);
                }
#endif
            } else {
                AmbaLL_LogUInt5("[ERR] Similar function returned NG 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
            }
            SIM_RelCmdBuffer(CmdBufferId);
        }
    }

    return Rval;
}

static inline UINT32 HL_FillVideoProcPymdROI(cmd_vproc_img_pyramid_setup_t *pVprocPymdSetup,
                                             UINT8 IsPolySqrt2,
                                             const CTX_VIEWZONE_INFO_s *pViewZoneInfo)
{
    UINT32 Rval = OK;
    UINT16 i, HierIdx;
    UINT16 BitVal;
    UINT8 NeedManuallyCrop = (UINT8)0U;
    UINT8 OctaveMode;
    UINT16 InitialWidth = 0U, InitialHeight = 0U;
    UINT16 Height;

    OctaveMode = (IsPolySqrt2 == (UINT8)1U)? (UINT8)0U: (UINT8)1U;

    for (i = 0U; i < AMBA_DSP_MAX_HIER_NUM; i++) {
        /* Hier0/2/4/6 comes from Main,
         *   if ROI setting is zero, uCode will fetch VprocSetup.H_Main.
         *   To prevent align_16 information be send out to VpMsg.
         *   we shall specify ROI window of Hier0/2/4 if user want whole FOV
         * Hier1 comes from Main with Fixed DownScale factor(OctaveMode)
         *   If we set PolyW/H larger than needed, DSP will report image W/H as PolyW/H setting
         *   Hence, user will see un-touched area data
         *   To prevent it, when user didn't have crop setting, we set crop to make DSP report meaningful message
         */
        HierIdx = (OctaveMode == 0U)? i: HL_HierUserIdx2VprocIdx[i];
        if (((i%2U) == 0U) || (OctaveMode == 1U)) {
            InitialWidth = pViewZoneInfo->Main.Width;
            InitialHeight = pViewZoneInfo->Main.Height;
            InitialHeight = ALIGN_NUM16(InitialHeight, 2U);
        } else {
            (void)HL_GetOctaveSize(OctaveMode, pViewZoneInfo->Main.Width, &InitialWidth);
            InitialWidth = ALIGN_NUM16(InitialWidth, 16U);
            (void)HL_GetOctaveSize(OctaveMode, pViewZoneInfo->Main.Height, &InitialHeight);
            InitialHeight = ALIGN_NUM16(InitialHeight, 2U);
        }

        NeedManuallyCrop = (UINT8)0U;
        BitVal = (UINT16)DSP_GetU16Bit(pViewZoneInfo->Pyramid.HierBit, HierIdx, 1U);
        if ((pViewZoneInfo->Pyramid.HierCropWindow[i].Width == 0U) &&
            (pViewZoneInfo->Pyramid.HierCropWindow[i].Height == 0U)) {
            if (BitVal == 1U) {
                NeedManuallyCrop = (UINT8)1U;
            }
        }

        if (NeedManuallyCrop == (UINT8)1U) {
            if (OctaveMode == 0U) {
                pVprocPymdSetup->scale_info[HierIdx].roi_width = InitialWidth >> HL_Hier2ShiftNumSqrt[i];
                Height = InitialHeight >> HL_Hier2ShiftNumSqrt[i];
            } else {
                InitialHeight = ALIGN_NUM16(InitialHeight, (UINT16)2U << i);
                pVprocPymdSetup->scale_info[HierIdx].roi_width = InitialWidth >> i;
                Height = InitialHeight >> i;
            }
            pVprocPymdSetup->scale_info[HierIdx].roi_height = ALIGN_NUM16(Height, 2U);
        } else {
            pVprocPymdSetup->scale_info[HierIdx].roi_width = pViewZoneInfo->Pyramid.HierCropWindow[i].Width;
            pVprocPymdSetup->scale_info[HierIdx].roi_height = pViewZoneInfo->Pyramid.HierCropWindow[i].Height;
        }
        pVprocPymdSetup->scale_info[HierIdx].roi_start_col = (INT16)pViewZoneInfo->Pyramid.HierCropWindow[i].OffsetX;
        pVprocPymdSetup->scale_info[HierIdx].roi_start_row = (INT16)pViewZoneInfo->Pyramid.HierCropWindow[i].OffsetY;
    }

    return Rval;
}

static UINT32 FillVideoProcPyramidSetup(cmd_vproc_img_pyramid_setup_t *VprocPymdSetup, UINT16 ViewZoneId)
{
    UINT32 Rval = OK;
    UINT16 i;
    UINT16 PymdDeci;
    UINT8 IsPolySqrt2;
    UINT8 HierBitMask = (UINT8)0U;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    if (VprocPymdSetup == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);

        VprocPymdSetup->channel_id = (UINT8)ViewZoneId;
        /* uCode has different mapping when IsPolySqrt2 = 0 */
        IsPolySqrt2 = (UINT8)DSP_GetU16Bit(ViewZoneInfo->Pyramid.IsPolySqrt2, DSP_PYMD_PLOY_IDX, DSP_PYMD_PLOY_LEN);
        VprocPymdSetup->is_hier_poly_sqrt2 = (UINT8)ViewZoneInfo->Pyramid.IsPolySqrt2;
        VprocPymdSetup->is_hier_y12 = (UINT8)DSP_GetU16Bit(ViewZoneInfo->Pyramid.IsPolySqrt2, DSP_PYMD_Y12_IDX, DSP_PYMD_Y12_LEN);
        if (IsPolySqrt2 == 0U) {
            for (i = 0U; i < AMBA_DSP_MAX_HIER_NUM; i++) {
                if (1U == DSP_GetU16Bit(ViewZoneInfo->Pyramid.HierBit, i, 1U)) {
                    DSP_SetU8Bit(&HierBitMask, HL_HierUserIdx2VprocIdx[i]);
                }
            }
            VprocPymdSetup->enable_bit_mask = (UINT8)HierBitMask;
        } else {
            VprocPymdSetup->enable_bit_mask = (UINT8)ViewZoneInfo->Pyramid.HierBit;
        }

        Rval = HL_FillVideoProcPymdROI(VprocPymdSetup,
                                       IsPolySqrt2,
                                       ViewZoneInfo);

        VprocPymdSetup->is_grp_cmd = 0U; //TBD
        VprocPymdSetup->grp_fov_cmd_id = 0U;
        VprocPymdSetup->is_hier_burst_tiling_out = 0U;
        VprocPymdSetup->roi_tag = ViewZoneInfo->Pyramid.HierTag;
        PymdDeci = (UINT16)DSP_GetU16Bit(ViewZoneInfo->Pyramid.IsPolySqrt2, DSP_PYMD_DECI_IDX, DSP_PYMD_DECI_LEN);
        PymdDeci = (PymdDeci == 0U)? 1U: PymdDeci;
        VprocPymdSetup->deci_rate = ((UINT32)PymdDeci << 16U) | 0x1U;
    }

    return Rval;
}
UINT32 HL_FillVideoProcPyramidSetup(cmd_vproc_img_pyramid_setup_t *VprocPymdSetup, UINT16 ViewZoneId)
{
    UINT32 Rval;
    UINT32 CrcHL, CrcSimilar;
    const UINT32 *pU32 = NULL;
    UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;
#ifdef CONFIG_ENABLE_DSP_MONITOR
    AMBA_DSP_MONITOR_CRC_CMPR_s CrcCmpr;
#endif

    Rval = FillVideoProcPyramidSetup(VprocPymdSetup, ViewZoneId);
    if (pIsSimilarEnabled != NULL) {
        if (1U == IsSimilarEnabled()) {
            UINT32 SIM_Rval;
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_vproc_img_pyramid_setup_t *SimVprocPymdSetup = SIM_DefCtxCmdBufPtrVpcPymd;

            dsp_osal_typecast(&pU32, &VprocPymdSetup);
            CrcHL = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_vproc_img_pyramid_setup_t), 0U);

            SIM_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&SimVprocPymdSetup, &CmdBufferAddr);
            SIM_Rval = SIM_FillVideoProcPyramidSetup(SimVprocPymdSetup, ViewZoneId);
            if (SIM_Rval == OK) {
                dsp_osal_typecast(&pU32, &SimVprocPymdSetup);
                CrcSimilar = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_vproc_img_pyramid_setup_t), 0U);
#ifdef CONFIG_ENABLE_DSP_MONITOR
                CrcCmpr.ModuleId = AMBA_SSP_PRINT_MODULE_ID;
                CrcCmpr.Crc0 = CrcHL;
                CrcCmpr.Crc1 = CrcSimilar;
                CrcCmpr.UUID = __LINE__;
                SIM_Rval = dsp_mon_crc_cmpr(&CrcCmpr);
                if (SIM_Rval != OK) {
                    AmbaLL_LogUInt5("dsp_mon_crc_cmpr return 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
                }
#else
                if (CrcSimilar != CrcHL) {
                    AmbaLL_LogUInt5("[ERR] Similar CRC mismatch @ %d HL 0x%x Similar 0x%x", __LINE__, CrcHL, CrcSimilar, 0U, 0U);
                }
#endif
            } else {
                AmbaLL_LogUInt5("[ERR] Similar function returned NG 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
            }
            SIM_RelCmdBuffer(CmdBufferId);
        }
    }

    return Rval;
}

static UINT32 FillVideoProcLnDtSetup(cmd_vproc_lane_det_setup_t *LnDtSetup, UINT16 ViewZoneId)
{
    UINT32 Rval = OK;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    if (LnDtSetup == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);

        LnDtSetup->channel_id = (UINT8)ViewZoneId;
        LnDtSetup->pyr_filter_idx = (UINT8)ViewZoneInfo->LaneDetect.HierSource;
        LnDtSetup->ld_roi_start_col = ViewZoneInfo->LaneDetect.CropWindow.OffsetX;
        LnDtSetup->ld_roi_start_row = ViewZoneInfo->LaneDetect.CropWindow.OffsetY;
        LnDtSetup->ld_roi_width = ViewZoneInfo->LaneDetect.CropWindow.Width;
        LnDtSetup->ld_roi_height = ViewZoneInfo->LaneDetect.CropWindow.Height;
        LnDtSetup->is_grp_cmd = (UINT8)0U;
        LnDtSetup->grp_fov_cmd_id = (UINT8)0U;
    }

    return Rval;
}
UINT32 HL_FillVideoProcLnDtSetup(cmd_vproc_lane_det_setup_t *LnDtSetup, UINT16 ViewZoneId)
{
    UINT32 Rval;
    UINT32 CrcHL, CrcSimilar;
    const UINT32 *pU32 = NULL;
    UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;
#ifdef CONFIG_ENABLE_DSP_MONITOR
    AMBA_DSP_MONITOR_CRC_CMPR_s CrcCmpr;
#endif

    Rval = FillVideoProcLnDtSetup(LnDtSetup, ViewZoneId);
    if (pIsSimilarEnabled != NULL) {
        if (1U == IsSimilarEnabled()) {
            UINT32 SIM_Rval;
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_vproc_lane_det_setup_t *SimLnDtSetup = SIM_DefCtxCmdBufPtrVpcLndt;

            dsp_osal_typecast(&pU32, &LnDtSetup);
            CrcHL = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_vproc_lane_det_setup_t), 0U);

            SIM_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&SimLnDtSetup, &CmdBufferAddr);
            SIM_Rval = SIM_FillVideoProcLnDtSetup(SimLnDtSetup, ViewZoneId);
            if (SIM_Rval == OK) {
                dsp_osal_typecast(&pU32, &SimLnDtSetup);
                CrcSimilar = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_vproc_lane_det_setup_t), 0U);
#ifdef CONFIG_ENABLE_DSP_MONITOR
                CrcCmpr.ModuleId = AMBA_SSP_PRINT_MODULE_ID;
                CrcCmpr.Crc0 = CrcHL;
                CrcCmpr.Crc1 = CrcSimilar;
                CrcCmpr.UUID = __LINE__;
                SIM_Rval = dsp_mon_crc_cmpr(&CrcCmpr);
                if (SIM_Rval != OK) {
                    AmbaLL_LogUInt5("dsp_mon_crc_cmpr return 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
                }
#else
                if (CrcSimilar != CrcHL) {
                    AmbaLL_LogUInt5("[ERR] Similar CRC mismatch @ %d HL 0x%x Similar 0x%x", __LINE__, CrcHL, CrcSimilar, 0U, 0U);
                }
#endif
            } else {
                AmbaLL_LogUInt5("[ERR] Similar function returned NG 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
            }
            SIM_RelCmdBuffer(CmdBufferId);
        }
    }

    return Rval;
}

static UINT32 FillVideoPreviewSetup(cmd_vproc_prev_setup_t *PrevSetup,
                                UINT16 ViewZoneId,
                                const UINT8 PrevId,
                                UINT8 *CmdByPass,
                                const CTX_YUV_STRM_LAYOUT_s *pExtStrmLayout)
{
    UINT32 Rval = OK;
    UINT16 i = 0U, YuvStrmIdx = 0U, Found = 0U, PinId = DSP_VPROC_PIN_NUM;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout;
    CTX_VPROC_INFO_s VprocInfo = {0};
    const AMBA_DSP_WINDOW_s *Win = NULL;
    const AMBA_DSP_WINDOW_s *MaxWin = NULL;
    UINT16 PrevDest = AMBA_DSP_PREV_DEST_DUMMY, TileNum = 1U;
#ifdef SUPPORT_VOUT_16XDS_TRUNCATE_TRICK
    UINT16 RoiWidth = 0U;
#endif

    if ((ViewZoneId >= AMBA_DSP_MAX_VIEWZONE_NUM) ||
        (PrevId >= DSP_VPROC_PREV_D)) {
        AmbaLL_LogUInt5("[HL_FillVideoPreviewSetup]Input out of range %d %d", ViewZoneId, PrevId, 0U, 0U, 0U);
        Rval = DSP_ERR_0001;
    } else if (PrevSetup == NULL) {
        AmbaLL_LogUInt5("[HL_FillVideoPreviewSetup]Null Input", 0U, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0000;
    } else {
        HL_GetResourcePtr(&Resource);
        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
        PinId = HL_DspPrevCtxVprocPinMap[PrevId];

        Win = &ViewZoneInfo->PinWindow[PinId];
        MaxWin = &ViewZoneInfo->PinMaxWindow[PinId];
        (void)HL_CalcVideoTile(ViewZoneInfo->Main.Width, SEC2_MAX_OUT_WIDTH, &TileNum, TILE_OVERLAP_WIDTH);
        if (((Win->Width == 0U) || (Win->Height == 0U)) &&
            ((MaxWin->Width == 0U) || (MaxWin->Height == 0U))) {
            *CmdByPass = 1U;
        } else {
            PrevSetup->channel_id = (UINT8)ViewZoneId;
            PrevSetup->prev_id = PrevId;
            PrevSetup->prev_src = VPROC_PREV_SRC_DRAM;
            PrevSetup->prev_format = VPROC_PREV_FMT_PROG;
            PrevSetup->prev_frame_rate = 0U;
            PrevSetup->prev_out_width = 0U;
            PrevSetup->prev_out_height = 0U;
            PrevSetup->prev_freeze_ena = 0U; //TBD

            HL_GetVprocInfo(HL_MTX_OPT_ALL, ViewZoneId, &VprocInfo);
            if (VprocInfo.PinUsage[PinId] > 0U) {
                DSP_Bit2U16Idx(VprocInfo.PinUsage[PinId], &YuvStrmIdx);
                HL_GetYuvStrmInfoPtr(YuvStrmIdx, &YuvStrmInfo);

                if (pExtStrmLayout == NULL) {
                    pYuvStrmLayout = &YuvStrmInfo->Layout;
                } else {
                    pYuvStrmLayout = pExtStrmLayout;
                }

                // Check if this Preview-out is currently in its output YuvStrm
                for (i=0U; i<pYuvStrmLayout->NumChan; i++) {
                    if (pYuvStrmLayout->ChanCfg[i].ViewZoneId == ViewZoneId) {
                        Rval = HL_CheckPrevROI(pYuvStrmLayout->ChanCfg[i].ROI, ViewZoneInfo->Main);
                        if (Rval != OK) {
                            AmbaLL_LogUInt5("[ERROR] ViewZone[%d] Preview[%d] cropping is out of main range",
                                            ViewZoneId, PrevId, 0U, 0U, 0U);
                            break;
                        }

                        RoiWidth = pYuvStrmLayout->ChanCfg[i].ROI.Width;
#ifdef SUPPORT_VOUT_16XDS_TRUNCATE_TRICK
                        /* Only support 2 pixel truncate */
                        if ((HL_MaxVprocPinOutputRatioMap[PinId] == PREV_MAX_RATIO_16X) &&
                            (pYuvStrmLayout->ChanCfg[i].ROI.Width >= pYuvStrmLayout->ChanCfg[i].Window.Width)) {
                            if (pYuvStrmLayout->ChanCfg[i].ROI.Width == (pYuvStrmLayout->ChanCfg[i].Window.Width*PREV_MAX_RATIO_16X)) {
                                RoiWidth -= VOUT_16XDS_TRUNCATE_LINE_NUM;
                            }
                        }
#endif
                        PrevSetup->prev_src_w = RoiWidth;
                        PrevSetup->prev_src_h = pYuvStrmLayout->ChanCfg[i].ROI.Height;
                        PrevSetup->prev_src_x_offset = pYuvStrmLayout->ChanCfg[i].ROI.OffsetX;
                        PrevSetup->prev_src_y_offset = pYuvStrmLayout->ChanCfg[i].ROI.OffsetY;

                        if (HL_GET_ROTATE(pYuvStrmLayout->ChanCfg[i].RotateFlip) == DSP_ROTATE_90_DEGREE) {
                            PrevSetup->prev_w = pYuvStrmLayout->ChanCfg[i].Window.Height;
                            PrevSetup->prev_h = pYuvStrmLayout->ChanCfg[i].Window.Width;
                        } else {
                            PrevSetup->prev_w = pYuvStrmLayout->ChanCfg[i].Window.Width;
                            PrevSetup->prev_h = pYuvStrmLayout->ChanCfg[i].Window.Height;
                        }
                        (void)HL_GetViewZonePrevDest(ViewZoneId, PinId, &PrevDest);
                        PrevSetup->prev_dst = (UINT8)PrevDest;
                        Found = 1U;
                    }
                }

                // Potential Pin-out, temporarily set its destination to DUMMY
                if ((Rval == OK) && (Found == 0U)) {
                    PrevSetup->prev_src_w = ViewZoneInfo->Main.Width;
                    PrevSetup->prev_src_h = ViewZoneInfo->Main.Height;
                    PrevSetup->prev_src_x_offset = 0U;
                    PrevSetup->prev_src_y_offset = 0U;
                    PrevSetup->prev_w = YuvStrmInfo->MaxChanWin[ViewZoneId].Width;
                    PrevSetup->prev_h = YuvStrmInfo->MaxChanWin[ViewZoneId].Height;
                    PrevSetup->prev_dst = AMBA_DSP_PREV_DEST_DUMMY;
                }
            }

            *CmdByPass = 0U;
        }
    }

    return Rval;
}
UINT32 HL_FillVideoPreviewSetup(cmd_vproc_prev_setup_t *PrevSetup,
                                UINT16 ViewZoneId,
                                const UINT8 PrevId,
                                UINT8 *CmdByPass,
                                const CTX_YUV_STRM_LAYOUT_s *pExtStrmLayout)
{
    UINT32 Rval;
    UINT32 CrcHL, CrcSimilar;
    const UINT32 *pU32 = NULL;
    UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;
#ifdef CONFIG_ENABLE_DSP_MONITOR
    AMBA_DSP_MONITOR_CRC_CMPR_s CrcCmpr;
#endif

    Rval = FillVideoPreviewSetup(PrevSetup, ViewZoneId, PrevId, CmdByPass, pExtStrmLayout);
    if (pIsSimilarEnabled != NULL) {
        if (1U == IsSimilarEnabled()) {
            UINT32 SIM_Rval;
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_vproc_prev_setup_t *SimPrevSetup = SIM_DefCtxCmdBufPtrVpcPrev;

            dsp_osal_typecast(&pU32, &PrevSetup);
            CrcHL = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_vproc_prev_setup_t), 0U);

            SIM_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&SimPrevSetup, &CmdBufferAddr);
            SIM_Rval = SIM_FillVideoPreviewSetup(SimPrevSetup, ViewZoneId, PrevId, CmdByPass, pExtStrmLayout);
            if (SIM_Rval == OK) {
                dsp_osal_typecast(&pU32, &SimPrevSetup);
                CrcSimilar = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_vproc_prev_setup_t), 0U);
#ifdef CONFIG_ENABLE_DSP_MONITOR
                CrcCmpr.ModuleId = AMBA_SSP_PRINT_MODULE_ID;
                CrcCmpr.Crc0 = CrcHL;
                CrcCmpr.Crc1 = CrcSimilar;
                CrcCmpr.UUID = __LINE__;
                SIM_Rval = dsp_mon_crc_cmpr(&CrcCmpr);
                if (SIM_Rval != OK) {
                    AmbaLL_LogUInt5("dsp_mon_crc_cmpr return 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
                }
#else
                if (CrcSimilar != CrcHL) {
                    AmbaLL_LogUInt5("[ERR] Similar CRC mismatch @ %d HL 0x%x Similar 0x%x", __LINE__, CrcHL, CrcSimilar, 0U, 0U);
                }
#endif
            } else {
                AmbaLL_LogUInt5("[ERR] Similar function returned NG 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
            }
            SIM_RelCmdBuffer(CmdBufferId);
        }
    }

    return Rval;
}

static UINT32 FillVideoProcDecimation(const UINT16 ViewZoneId,
                                  const UINT16 YuvStrmIdx,
                                  cmd_vproc_pin_out_deci_t *VprocDeci)
{
    UINT32 Rval = OK;
    UINT16 Found = 0U;
    UINT16 PinIdx, PinId = DSP_VPROC_PIN_PREVC;
    CTX_VPROC_INFO_s VprocInfo = {0};

    HL_GetVprocInfo(HL_MTX_OPT_ALL, ViewZoneId, &VprocInfo);
    for (PinIdx=0U; PinIdx<DSP_VPROC_PIN_NUM; PinIdx++) {
        if (VprocInfo.PinUsageOnStrm[PinIdx][YuvStrmIdx] != 0U) {
            PinId = PinIdx;
            Found = 1U;
            break;
        }
    }
    if (Found == 1U) {
        CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;

        HL_GetYuvStrmInfoLock(YuvStrmIdx, &YuvStrmInfo);
//        if ((PinId < DSP_VPROC_PIN_NUM) && (YuvStrmInfo->DestDeciRate != 0U)) {
        if (YuvStrmInfo->DestDeciRate != 0U) {
            (void)dsp_osal_memset(VprocDeci, 0, sizeof(cmd_vproc_pin_out_deci_t));
            VprocDeci->channel_id = (UINT8)ViewZoneId;
            VprocDeci->stream_id = (UINT8)HL_CtxVprocPinDspPinMap[PinId];
            VprocDeci->output_repeat_ratio = YuvStrmInfo->DestDeciRate;  // 0x20001 : 2x decimation
        }
        HL_GetYuvStrmInfoUnLock(YuvStrmIdx);
    }
    return Rval;
}

UINT32 HL_FillVideoProcDecimation(const UINT16 ViewZoneId,
                                  const UINT16 YuvStrmIdx,
                                  cmd_vproc_pin_out_deci_t *VprocDeci)
{
    UINT32 Rval;
    UINT32 CrcHL, CrcSimilar;
    const UINT32 *pU32 = NULL;
    UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;
#ifdef CONFIG_ENABLE_DSP_MONITOR
    AMBA_DSP_MONITOR_CRC_CMPR_s CrcCmpr;
#endif

    Rval = FillVideoProcDecimation(ViewZoneId, YuvStrmIdx, VprocDeci);
    if (pIsSimilarEnabled != NULL) {
        if (1U == IsSimilarEnabled()) {
            UINT32 SIM_Rval;
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_vproc_pin_out_deci_t *SimVprocDeci = SIM_DefCtxCmdBufPtrVpcDeci;

            dsp_osal_typecast(&pU32, &VprocDeci);
            CrcHL = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_vproc_pin_out_deci_t), 0U);

            SIM_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&SimVprocDeci, &CmdBufferAddr);
            SIM_Rval = SIM_FillVideoProcDecimation(ViewZoneId, YuvStrmIdx, SimVprocDeci);
            if (SIM_Rval == OK) {
                dsp_osal_typecast(&pU32, &SimVprocDeci);
                CrcSimilar = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_vproc_pin_out_deci_t), 0U);
#ifdef CONFIG_ENABLE_DSP_MONITOR
                CrcCmpr.ModuleId = AMBA_SSP_PRINT_MODULE_ID;
                CrcCmpr.Crc0 = CrcHL;
                CrcCmpr.Crc1 = CrcSimilar;
                CrcCmpr.UUID = __LINE__;
                SIM_Rval = dsp_mon_crc_cmpr(&CrcCmpr);
                if (SIM_Rval != OK) {
                    AmbaLL_LogUInt5("dsp_mon_crc_cmpr return 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
                }
#else
                if (CrcSimilar != CrcHL) {
                    AmbaLL_LogUInt5("[ERR] Similar CRC mismatch @ %d HL 0x%x Similar 0x%x", __LINE__, CrcHL, CrcSimilar, 0U, 0U);
                }
#endif
            } else {
                AmbaLL_LogUInt5("[ERR] Similar function returned NG 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
            }
            SIM_RelCmdBuffer(CmdBufferId);
        }
    }

    return Rval;
}

static UINT32 FillIsoCfgUpdate(UINT16 ViewZoneId, cmd_vproc_ik_config_t *IsoCfgUpdate)
{
    UINT32 Rval = OK;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
    IsoCfgUpdate->cmd_code = CMD_VPROC_IK_CONFIG;
    IsoCfgUpdate->channel_id = (UINT8)ViewZoneId;

    if (ViewZoneInfo->EffectSyncState != EFCT_SYNC_OFF) {
        //Enable WarpCmdUpdate
        IsoCfgUpdate->use_flow_warp_cr = (UINT8)0U;
    } else {
        //Disable WarpCmdUpdate
        IsoCfgUpdate->use_flow_warp_cr = (UINT8)1U; //use to
    }

    return Rval;
}
UINT32 HL_FillIsoCfgUpdate(UINT16 ViewZoneId, cmd_vproc_ik_config_t *IsoCfgUpdate)
{
    UINT32 Rval;
    UINT32 CrcHL, CrcSimilar;
    const UINT32 *pU32 = NULL;
    UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;
#ifdef CONFIG_ENABLE_DSP_MONITOR
    AMBA_DSP_MONITOR_CRC_CMPR_s CrcCmpr;
#endif

    Rval = FillIsoCfgUpdate(ViewZoneId, IsoCfgUpdate);
    if (pIsSimilarEnabled != NULL) {
        if (1U == IsSimilarEnabled()) {
            UINT32 SIM_Rval;
            dsp_osal_typecast(&pU32, &IsoCfgUpdate);
            CrcHL = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_vproc_ik_config_t), 0U);

            SIM_Rval = SIM_FillIsoCfgUpdate(ViewZoneId, IsoCfgUpdate);
            if (SIM_Rval == OK) {
                dsp_osal_typecast(&pU32, &IsoCfgUpdate);
                CrcSimilar = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_vproc_ik_config_t), 0U);
#ifdef CONFIG_ENABLE_DSP_MONITOR
                CrcCmpr.ModuleId = AMBA_SSP_PRINT_MODULE_ID;
                CrcCmpr.Crc0 = CrcHL;
                CrcCmpr.Crc1 = CrcSimilar;
                CrcCmpr.UUID = __LINE__;
                SIM_Rval = dsp_mon_crc_cmpr(&CrcCmpr);
                if (SIM_Rval != OK) {
                    AmbaLL_LogUInt5("dsp_mon_crc_cmpr return 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
                }
#else
                if (CrcSimilar != CrcHL) {
                    AmbaLL_LogUInt5("[ERR] Similar CRC mismatch @ %d HL 0x%x Similar 0x%x", __LINE__, CrcHL, CrcSimilar, 0U, 0U);
                }
#endif
            } else {
                AmbaLL_LogUInt5("[ERR] Similar function returned NG 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
            }
        }
    }

    return Rval;
}

static UINT32 FillVinConfig(set_vin_config_t *VinCfg, UINT16 VinId)
{
    UINT32 Rval = OK;
    CTX_VIN_INFO_s VinInfo = {0};
    ULONG ULAddr;
    AMBA_DSP_VIN_MAIN_CONFIG_DATA_s *pVinMainCfg = NULL;
    UINT16 ViewZoneId = 0U;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    if (VinCfg == NULL) {
        Rval = DSP_ERR_0000;
    } else if (VinId >= AMBA_DSP_MAX_VIN_NUM) {
        Rval = DSP_ERR_0001;
    } else {
        HL_GetVinInfo(HL_MTX_OPT_ALL, VinId, &VinInfo);

        if (VinInfo.SensorNum > 0U) {
            UINT8 i;
            CTX_SENSOR_INFO_s SensorInfo = {0};

            VinCfg->vin_id = (UINT8)VinId;
            VinCfg->vin_width = VinInfo.CapWindow[0U].Width;
            VinCfg->vin_height = VinInfo.CapWindow[0U].Height;

            DSP_GetDspVinCfg(VinId, &pVinMainCfg);
            dsp_osal_typecast(&ULAddr, &pVinMainCfg);
            (void)dsp_osal_virt2cli(ULAddr, &VinCfg->vin_config_dram_addr);
            VinCfg->vin_config_data_size = (UINT16)sizeof(AMBA_DSP_VIN_MAIN_CONFIG_DATA_s);

            for (i=0; i<VinInfo.SensorNum; i++) {
                HL_GetSensorInfo(HL_MTX_OPT_ALL, VinInfo.SensorIndexTable[i], &SensorInfo);
                VinCfg->sensor_resolution = SensorInfo.BitResolution;
                VinCfg->bayer_pattern = SensorInfo.BayerPattern;
            }

            if (IsAligned4(VinInfo.CapWindow[0U].Width) == 1U) {
                if (VinInfo.CompandEna == 1U) {
                    VinCfg->decomp_en = 1;
                    VinCfg->comp_en = 1;
                    (void)dsp_osal_virt2cli(VinInfo.CompandTableAddr, &VinCfg->comp_lookup_table_daddr);
                    (void)dsp_osal_virt2cli(VinInfo.DeCompandTableAddr, &VinCfg->decomp_lookup_table_daddr);
                } else {
                    VinCfg->decomp_en = 0U;
                    VinCfg->comp_en = 0U;
                    VinCfg->decomp_lookup_table_daddr = 0x0U;
                    VinCfg->comp_lookup_table_daddr = 0x0U;
                }
            } else {
                AmbaLL_LogUInt5("HL_FillVinConfig [%d]RawCapW[%d] must be 4 align", VinId, VinInfo.CapWindow[0U].Width, 0U, 0U, 0U);
                Rval = DSP_ERR_0001;
            }

            /** Fetch first viewzone information of this Vin */
            if (HL_FetchFirstViewZoneInfo(VinId, &ViewZoneId, &ViewZoneInfo) == 1U) {
                VinCfg->no_of_hdr_exposures = (UINT8)(ViewZoneInfo->HdrBlendNumMinusOne + 1U);
            } else {
                AmbaLL_LogUInt5("HL_FillVinConfig [%d]cannot find ViewZone", VinId, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0001;
            }
//FIXME
            VinCfg->ir_mode = (UINT8)0U;
        } else {
            AmbaLL_LogUInt5("Null Sensor in Vin[%d]", VinId, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
        }
    }

    return Rval;
}

UINT32 HL_FillVinConfig(set_vin_config_t *VinCfg, UINT16 VinId)
{
    UINT32 Rval;
    UINT32 CrcHL, CrcSimilar;
    const UINT32 *pU32 = NULL;
    UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;
#ifdef CONFIG_ENABLE_DSP_MONITOR
    AMBA_DSP_MONITOR_CRC_CMPR_s CrcCmpr;
#endif

    Rval = FillVinConfig(VinCfg, VinId);
    if (pIsSimilarEnabled != NULL) {
        if (1U == IsSimilarEnabled()) {
            UINT32 SIM_Rval;
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            set_vin_config_t *SimVinCfg = SIM_DefCtxCmdBufPtrVinCfg;

            dsp_osal_typecast(&pU32, &VinCfg);
            CrcHL = CalcCheckSum32Add(pU32, (UINT32)sizeof(set_vin_config_t), 0U);

            SIM_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&SimVinCfg, &CmdBufferAddr);
            SIM_Rval = SIM_FillVinConfig(SimVinCfg, VinId);
            if (SIM_Rval == OK) {
                dsp_osal_typecast(&pU32, &SimVinCfg);
                CrcSimilar = CalcCheckSum32Add(pU32, (UINT32)sizeof(set_vin_config_t), 0U);
#ifdef CONFIG_ENABLE_DSP_MONITOR
                CrcCmpr.ModuleId = AMBA_SSP_PRINT_MODULE_ID;
                CrcCmpr.Crc0 = CrcHL;
                CrcCmpr.Crc1 = CrcSimilar;
                CrcCmpr.UUID = __LINE__;
                SIM_Rval = dsp_mon_crc_cmpr(&CrcCmpr);
                if (SIM_Rval != OK) {
                    AmbaLL_LogUInt5("dsp_mon_crc_cmpr return 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
                }
#else
                if (CrcSimilar != CrcHL) {
                    AmbaLL_LogUInt5("[ERR] Similar CRC mismatch @ %d HL 0x%x Similar 0x%x", __LINE__, CrcHL, CrcSimilar, 0U, 0U);
                }
#endif
            } else {
                AmbaLL_LogUInt5("[ERR] Similar function returned NG 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
            }
            SIM_RelCmdBuffer(CmdBufferId);
        }
    }

    return Rval;
}

static inline void HL_UpdateViewzoneStartIsoCfg(const UINT16 CfgPtn, const UINT16 ViewZoneId)
{
    if ((CfgPtn & VIN_START_CFG_ISOCFG) > 0U) {
        CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
//FIXME, when RawEnc, we dont need IsoCfgAddr at this moment, just update when feeding raw
//       here we use this code segment to generate first IsoCfg for 1st raw

        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);

        if ((ViewZoneInfo->Pipe == DSP_DRAM_PIPE_NORMAL) ||
            (ViewZoneInfo->Pipe == DSP_DRAM_PIPE_EFCY) ||
            (ViewZoneInfo->Pipe == DSP_DRAM_PIPE_MIPI_YUV)) {
            /* Only update FirstViewZone */
            HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
            ViewZoneInfo->StartIsoCfgIndex = ViewZoneInfo->IkId;
            ViewZoneInfo->StartIsoCfgAddr = ViewZoneInfo->IkIsoAddr;
            HL_GetViewZoneInfoUnLock(ViewZoneId);
        }
    }
}

static inline UINT32 HL_UpdateViewzoneStartIsoCfgSetup(cmd_vin_start_t *pVinStartCfg, const UINT16 ViewZoneId)
{
    UINT32 IsoCfgId, Rval = OK;
    ULONG IsoCfgAddr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    /* IsoCfg */
    HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
    if (ViewZoneInfo->StartIsoCfgAddr == 0U) {
        ULONG NullAddr = 0U;

        (void)dsp_osal_virt2cli(NullAddr, &pVinStartCfg->batch_cmd_set_info.addr);
        pVinStartCfg->batch_cmd_set_info.id = 0U;
        pVinStartCfg->batch_cmd_set_info.size = 0U;
        AmbaLL_LogUInt5("!!! IK Exec[%d] Fail, Fill IsoCfg as Zero", ViewZoneId, 0U, 0U, 0U, 0U);
    } else {
        UINT32 *pBatchQAddr, BatchCmdId;
        cmd_vproc_ik_config_t *pIsoCfgCmd;
        UINT32 NewWp = 0U;
        ULONG BatchQAddr = 0U;

        IsoCfgId = ViewZoneInfo->StartIsoCfgIndex;
        IsoCfgAddr = ViewZoneInfo->StartIsoCfgAddr;

        HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
        /* Request BatchCmdQ buffer */
        Rval = DSP_ReqBuf(&ViewZoneInfo->BatchQPoolDesc, 1U, &NewWp, 1U/*FullnessCheck*/);
        if (Rval != OK) {
            ULONG NullAddr = 0U;

            HL_GetViewZoneInfoUnLock(ViewZoneId);
            (void)dsp_osal_virt2cli(NullAddr, &pVinStartCfg->batch_cmd_set_info.addr);
            pVinStartCfg->batch_cmd_set_info.id = 0U;
            pVinStartCfg->batch_cmd_set_info.size = 0U;

            AmbaLL_LogWarnUInt5((UINT16)AMBALLLOG_WARN_BATCHQ_FULL, "[FillVinStartCfg] ViewZone(%d) batch pool is full", ViewZoneId, 0U, 0U, 0U, 0U);
        } else {
            ViewZoneInfo->BatchQPoolDesc.Wp = NewWp;
            HL_GetViewZoneInfoUnLock(ViewZoneId);
            HL_GetPointerToDspBatchQ(ViewZoneId, (UINT16)ViewZoneInfo->BatchQPoolDesc.Wp, &pBatchQAddr, &BatchCmdId);

            /* Reset New BatchQ after Wp advanced */
            HL_ResetDspBatchQ(pBatchQAddr);

            /* IsoCfgCmd occupied first CmdSlot */
            dsp_osal_typecast(&pIsoCfgCmd, &pBatchQAddr);
            (void)dsp_osal_memset(pIsoCfgCmd, 0, sizeof(DSP_CMD_s));
            (void)HL_FillIsoCfgUpdate(ViewZoneId, pIsoCfgCmd);
            (void)dsp_osal_virt2cli(IsoCfgAddr, &pIsoCfgCmd->idsp_flow_addr);
            pIsoCfgCmd->ik_cfg_id = IsoCfgId;

            dsp_osal_typecast(&BatchQAddr, &pBatchQAddr);
            (void)dsp_osal_virt2cli(BatchQAddr, &pVinStartCfg->batch_cmd_set_info.addr);
            HL_SetDspBatchQInfo(BatchCmdId, 1/*AutoReset*/, BATCHQ_INFO_ISO_CFG, (IsoCfgId > 0U)? IsoCfgId: 0x1U);
            pVinStartCfg->batch_cmd_set_info.id = BatchCmdId; // uCode need non-null
            pVinStartCfg->batch_cmd_set_info.size = HL_GetBatchCmdNumber(BatchQAddr)*CMD_SIZE_IN_BYTE;
        }
    }
    return Rval;
}

/* Dram raw in, VIN_SRC_FROM_DRAM, rawenc case */
static inline UINT32 HL_FillVinStartCfgVirtRawEnc(cmd_vin_start_t *VinStartCfg,
                                                  const UINT16 VinId,
                                                  const UINT16 CfgPtn,
                                                  const UINT16 ViewZoneId)
{
    UINT32 Rval = OK;
    CTX_VIN_INFO_s VinInfo = {0};
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    HL_UpdateViewzoneStartIsoCfg(CfgPtn, ViewZoneId);

    if ((CfgPtn & VIN_START_CFG_CMD) > 0U) {

        HL_GetVinInfo(HL_MTX_OPT_ALL, VinId, &VinInfo);
        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);

//FIXME, should consider effect
        VinStartCfg->no_reset_fp_shared_res = 1U;

        VinStartCfg->output_dest = VIN_RAW_DEST_INTERNAL;
        VinStartCfg->vin_cap_width = VinInfo.CapWindow[0U].Width;
        VinStartCfg->vin_cap_height = VinInfo.CapWindow[0U].Height;

        if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM) {
            VinStartCfg->input_source = VIN_RAW_SRC_TYPE_RAW;
            VinStartCfg->send_input_data_type = DSP_VIN_SEND_IN_DATA_RAW;
        } else if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_422) {
            VinStartCfg->input_source = VIN_RAW_SRC_TYPE_RAW;
            VinStartCfg->send_input_data_type = DSP_VIN_SEND_IN_DATA_YUV422;
        } else if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_420) {
            VinStartCfg->input_source = VIN_RAW_SRC_TYPE_RAW;
            VinStartCfg->send_input_data_type = DSP_VIN_SEND_IN_DATA_YUV420;
        } else {
            //DO NOTHING
        }

        if (VinInfo.CfaCompressed > 0U) {
            UINT16 RawWidth = 0U, RawPitch = 0U, Mantissa = 0U, BlkSz = 0U;

            (void)HL_GetCmprRawBufInfo(VinInfo.CapWindow[0U].Width,
                                       VinInfo.CfaCompressed,
                                       VinInfo.CfaCompact,
                                       &RawWidth, &RawPitch,
                                       &Mantissa, &BlkSz);
            VinStartCfg->is_compression_en = (UINT8)1U;
            VinStartCfg->blk_sz = (UINT8)BlkSz;
            VinStartCfg->mantissa = (UINT8)Mantissa;
        } else {
            VinStartCfg->is_compression_en = (UINT8)0U;
            if (VinInfo.CfaCompact > 0U) {
                VinStartCfg->blk_sz = (UINT8)RawCmptRateTable[VinInfo.CfaCompact].Block;
                VinStartCfg->mantissa = (UINT8)RawCmptRateTable[VinInfo.CfaCompact].Mantissa;
            } else {
                VinStartCfg->blk_sz = 0U;
                VinStartCfg->mantissa = 0U;
            }
        }
        Rval = HL_UpdateViewzoneStartIsoCfgSetup(VinStartCfg, ViewZoneId);
    }
    return Rval;
}
/* Dram yuv in, VIN_SRC_FROM_DRAM_420/422, yuv rawenc, or yuv feeding case */
static inline UINT32 HL_FillVinStartCfgVirtImpl(cmd_vin_start_t *VinStartCfg,
                                                const UINT16 VinId,
                                                const UINT16 CfgPtn,
                                                const CTX_RESOURCE_INFO_s *pResource)
{
    UINT8 ExitLoop = 0U;
    UINT16 i ,ViewZoneVinId = 0U, ViewZoneId = 0U;
    UINT32 Rval = OK;
    CTX_VIN_INFO_s VinInfo = {0};
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    for (i = 0U; i < pResource->MaxViewZoneNum; i++) {
        HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
        (void)HL_GetViewZoneVinId(i, &ViewZoneVinId);
        if (((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_422) ||
             (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_420)) &&
             (ViewZoneVinId == VinId)) {
            ViewZoneId = i;
            ExitLoop = 1U;
            break;
        }
    }
    if (ExitLoop == (UINT8)1U) {
        HL_GetVinInfo(HL_MTX_OPT_ALL, VinId, &VinInfo);
        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);

        /* For virtual vin yuv in case, need to setup iso cfg,
         * Yuv feed case should has StartIsoCfgAddr */
        if (ViewZoneInfo->StartIsoCfgAddr == 0U) {
            HL_UpdateViewzoneStartIsoCfg(CfgPtn, ViewZoneId);
        }
        if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_422) {
            VinStartCfg->input_source = VIN_RAW_SRC_TYPE_RAW;
            VinStartCfg->send_input_data_type = DSP_VIN_SEND_IN_DATA_YUV422;
        } else if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_420) {
            VinStartCfg->input_source = VIN_RAW_SRC_TYPE_RAW;
            VinStartCfg->send_input_data_type = DSP_VIN_SEND_IN_DATA_YUV420;
        } else {
            //DO NOTHING
        }
        VinStartCfg->output_dest = VIN_RAW_DEST_INTERNAL;
        VinStartCfg->vin_cap_width = VinInfo.CapWindow[0U].Width;
        VinStartCfg->vin_cap_height = VinInfo.CapWindow[0U].Height;

        if (ViewZoneInfo->IsEffectChanMember == 1U) {
            VinStartCfg->no_reset_fp_shared_res = 1U;
        } else {
            VinStartCfg->no_reset_fp_shared_res = 0U;
        }
    } else {
        VinStartCfg->send_input_data_type = DSP_VIN_SEND_IN_DATA_RAW;
        VinStartCfg->output_dest = VIN_RAW_DEST_INTERNAL;
        VinStartCfg->vin_cap_width = pResource->MaxVirtVinWidth[VinId - AMBA_DSP_MAX_VIN_NUM];
        VinStartCfg->vin_cap_height = pResource->MaxVirtVinHeight[VinId - AMBA_DSP_MAX_VIN_NUM];
    }
    return Rval;
}

static inline UINT8 HL_GetFovLayoutHdrIntlacMode(const UINT8 HdrBlendNumMinusOne, const UINT16 ViewZoneInterlace)
{
    UINT8 HdrIntacMode = 0U;

    if ((HdrBlendNumMinusOne > 0U) && (ViewZoneInterlace > 0U)) {
        HdrIntacMode = 1U;
    }

    return HdrIntacMode;
}

static inline UINT8 HL_IsLayoutLastFov(const UINT8 FovTotalNum, const UINT8 FovCnt)
{
    UINT8 IsLayoutLastFov = 0U;

    if (FovCnt == (FovTotalNum - 1U)) {
        IsLayoutLastFov = 1U;
    }

    return IsLayoutLastFov;
}

#ifdef DUPLEX_DEC_SHARE_VIRT_VIN
static inline void HL_FillVirtVinStartCfgImplSetFovLayout(cmd_vin_start_t *pVinStartCfg,
                                                          const UINT16 VinId,
                                                          const UINT16 MaxViewZoneNum,
                                                          const UINT32 ViewZoneActiveBit)
{
    UINT8 ExitILoop = (UINT8)0U, Cnt = 0U;
    UINT16 i, ViewZoneInterlace = 0;
    vin_fov_cfg_t *pDspVinFovLayout = NULL;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_DSP_INST_INFO_s DspInstInfo = {0};
    ULONG ULAddr, NullAddr = 0U;

    HL_GetDspInstance(HL_MTX_OPT_ALL, &DspInstInfo);
    if ((DspInstInfo.DecSharedVirtVinId != DSP_VIRT_VIN_IDX_INVALID) &&
        ((DspInstInfo.DecSharedVirtVinId + AMBA_DSP_MAX_VIN_NUM) == VinId)) {
        for (i = 0U; (i < MaxViewZoneNum) && (ExitILoop == (UINT8)0U); i++) {
            if (0U == DSP_GetBit(ViewZoneActiveBit, i, 1U)) {
                continue;
            }
            HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
            if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) {
                HL_GetPointerToDspVinStartFovCfg(VinId, (UINT8)Cnt, &pDspVinFovLayout);
                pDspVinFovLayout->chan_id = Cnt;
                pDspVinFovLayout->xpitch = (UINT8)((ViewZoneInterlace + 1U)*ViewZoneInfo->InputMuxSelVerDeci);
                // Decode source will not have HDR
                pDspVinFovLayout->hdr_intlac_mode = 0U;
                pDspVinFovLayout->is_fov_active = 1U;
                pDspVinFovLayout->x_offset = ViewZoneInfo->CapWindow.OffsetX;
                pDspVinFovLayout->y_offset = ViewZoneInfo->CapWindow.OffsetY;
                pDspVinFovLayout->width = ViewZoneInfo->CapWindow.Width;
                pDspVinFovLayout->height = ViewZoneInfo->CapWindow.Height;
                pDspVinFovLayout->is_last_fov = HL_IsLayoutLastFov(pVinStartCfg->fov_num, Cnt);

                (void)dsp_osal_virt2cli(NullAddr, &pVinStartCfg->batch_cmd_set_info.addr);
                pDspVinFovLayout->batch_cmd_set_info.id = 0U;
                pDspVinFovLayout->batch_cmd_set_info.size = 0U;

                /* SliceCfg */
                (void)dsp_osal_memset(&pDspVinFovLayout->slice_term_cap_line[0U],
                                      0,
                                      sizeof(UINT16)*DSP_MAX_VIN_CAP_SLICE_NUM);

                Cnt++;
                if (Cnt == pVinStartCfg->fov_num) {
                    ExitILoop = (UINT8)1U;
                }
            }
        }
        HL_GetPointerToDspVinStartFovCfg(VinId, (UINT8)0U, &pDspVinFovLayout);
        dsp_osal_typecast(&ULAddr, &pDspVinFovLayout);
        (void)dsp_osal_virt2cli(ULAddr, &pVinStartCfg->fov_cfg_tbl_daddr);
    } else {
        //TBD
    }
}
#endif

static inline UINT32 HL_FillVinStartCfgVirtTimLapse(cmd_vin_start_t *VinStartCfg,
                                                    const UINT16 VinId,
                                                    const UINT16 CfgPtn,
                                                    const CTX_RESOURCE_INFO_s *Resource)
{
    UINT32 Rval = OK;
    UINT8 ExitLoop;
    UINT16 i ,ViewZoneVinId = 0U, ViewZoneId = 0U;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_TIMELAPSE_INFO_s TimeLapseInfo = {0};

    // TimeLapse
    ExitLoop = 0U;
    for (i = 0U; i < Resource->MaxTimeLapseNum; i++) {
        HL_GetTimeLapseInfo(HL_MTX_OPT_ALL, i, &TimeLapseInfo);
        if ((TimeLapseInfo.VirtVinId != DSP_VIRT_VIN_IDX_INVALID) &&
            (TimeLapseInfo.VirtVinId == (VinId - AMBA_DSP_MAX_VIN_NUM))) {
            VinStartCfg->send_input_data_type = DSP_VIN_SEND_IN_DATA_YUV420;
            VinStartCfg->output_dest = VIN_RAW_DEST_INTERNAL;
            (void)HL_GetEncMaxTimeLapseWindow(&VinStartCfg->vin_cap_width, &VinStartCfg->vin_cap_height);
            ExitLoop = 1U;
            break;
        }
    }

    if (ExitLoop == 0U) {
        for (i = 0U; i < Resource->MaxViewZoneNum; i++) {
            HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
            (void)HL_GetViewZoneVinId(i, &ViewZoneVinId);
            if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM) &&
                 (ViewZoneVinId == VinId)) {
                ViewZoneId = i;
                ExitLoop = 1U;
                break;
            }
        }

//FIXME, MultiViewZone
        if (ExitLoop == (UINT8)1U) {
            /* Dram raw in, VIN_SRC_FROM_DRAM, rawenc case */
            Rval = HL_FillVinStartCfgVirtRawEnc(VinStartCfg, VinId, CfgPtn, ViewZoneId);
        } else {
            /* Dram yuv in, VIN_SRC_FROM_DRAM_420/422, yuv rawenc, or yuv feeding case */
            Rval = HL_FillVinStartCfgVirtImpl(VinStartCfg, VinId, CfgPtn, Resource);
        }
    }
    return Rval;
}

static inline UINT32 HL_FillVinStartCfgVirt(cmd_vin_start_t *VinStartCfg,
                                            const UINT16 VinId,
                                            const UINT16 CfgPtn)
{
    UINT16 i;
    UINT32 Rval = OK;
    CTX_STILL_INFO_s CtxStlInfo = {0};
    CTX_VID_DEC_INFO_s VidDecInfo = {0};
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_DSP_INST_INFO_s DspInstInfo = {0};
    ULONG ULAddr = 0U;

    HL_GetResourcePtr(&Resource);
    HL_GetStlInfo(HL_MTX_OPT_ALL, &CtxStlInfo);
    HL_GetDspInstance(HL_MTX_OPT_ALL, &DspInstInfo);

    //Find Dec instance idx by pairing VirtVinId with input VinId
    (void)HL_GetDecIdxFromVinId(VinId, &i);
    HL_GetVidDecInfo(HL_MTX_OPT_ALL, i, &VidDecInfo);

    VinStartCfg->vin_id = (UINT8)VinId;

    // default setting
    VinStartCfg->vin_poll_intvl = 0U;
    VinStartCfg->skip_frm_cnt = 0U;

    // default setting
    VinStartCfg->cmd_msg_decimation_rate = 1U; //TBD for HFR

    // default setting
    VinStartCfg->is_reset_frm_sync = 0U;
    VinStartCfg->is_auto_frm_drop_en = 0U;
    VinStartCfg->is_check_timestamp = 0U;

    // default setting
    VinStartCfg->capture_time_out_msec = 0U;
#ifdef SUPPORT_DSP_VIN_DEFT_RAW
    VinStartCfg->default_raw_image_address = 0U;
    VinStartCfg->default_raw_image_pitch = 0U;
    VinStartCfg->default_ce_image_address = 0U;
    VinStartCfg->default_ce_image_pitch = 0U;
#endif
#ifdef SUPPORT_DSP_VIN_FRM_CTRL
    VinStartCfg->capture_compl_cntl_msg_addr = 0U;
    VinStartCfg->compl_cntl_msg_update_time_msec = 0U;
#endif

    // default setting
    VinStartCfg->cap_slice_num = 0U;

    // default setting
    VinStartCfg->fov_num = 0U;
    VinStartCfg->is_compression_en = 0U;
    VinStartCfg->blk_sz = 0U;
    VinStartCfg->mantissa = 0U;

    // default setting
    (void)dsp_osal_virt2cli(ULAddr, &VinStartCfg->fov_cfg_tbl_daddr);
    (void)dsp_osal_virt2cli(ULAddr, &VinStartCfg->batch_cmd_set_info.addr);
    VinStartCfg->batch_cmd_set_info.id = 0U;
    VinStartCfg->batch_cmd_set_info.size = 0U;

    // default setting
    VinStartCfg->input_source = VIN_RAW_SRC_TYPE_RAW;
    if ((CtxStlInfo.RawInVirtVinId != DSP_VIRT_VIN_IDX_INVALID) &&
        (CtxStlInfo.RawInVirtVinId == (VinId - AMBA_DSP_MAX_VIN_NUM))) {
        VinStartCfg->send_input_data_type = DSP_VIN_SEND_IN_DATA_RAW;
        VinStartCfg->output_dest = VIN_RAW_DEST_INTERNAL;
        VinStartCfg->vin_cap_width = Resource->MaxStlRawInputWidth;
        VinStartCfg->vin_cap_height = Resource->MaxStlRawInputHeight;
    } else if ((CtxStlInfo.YuvInVirtVinId != DSP_VIRT_VIN_IDX_INVALID) &&
               (CtxStlInfo.YuvInVirtVinId == (VinId - AMBA_DSP_MAX_VIN_NUM))) {
        VinStartCfg->send_input_data_type = DSP_VIN_SEND_IN_DATA_YUV420;
        VinStartCfg->output_dest = VIN_RAW_DEST_INTERNAL;
        VinStartCfg->vin_cap_width = Resource->MaxStlYuvInputWidth;
        VinStartCfg->vin_cap_height = Resource->MaxStlYuvInputHeight;
    } else if ((CtxStlInfo.YuvEncVirtVinId != DSP_VIRT_VIN_IDX_INVALID) &&
               (CtxStlInfo.YuvEncVirtVinId == (VinId - AMBA_DSP_MAX_VIN_NUM))) {
        VinStartCfg->send_input_data_type = DSP_VIN_SEND_IN_DATA_YUV420;
        VinStartCfg->output_dest = VIN_RAW_DEST_INTERNAL;
        VinStartCfg->vin_cap_width = Resource->MaxStlYuvEncWidth;
        VinStartCfg->vin_cap_height = Resource->MaxStlYuvEncHeight;
    } else if ((VidDecInfo.YuvInVirtVinId != DSP_VIRT_VIN_IDX_INVALID) &&
               (VidDecInfo.YuvInVirtVinId == (VinId - AMBA_DSP_MAX_VIN_NUM))) {
        VinStartCfg->send_input_data_type = DSP_VIN_SEND_IN_DATA_YUV420;
        VinStartCfg->output_dest = VIN_RAW_DEST_INTERNAL;
        if ((VidDecInfo.MaxFrameWidth == 0U) || (VidDecInfo.MaxFrameHeight == 0U)) {
            // Dummy setting for preventing dsp assert
            HL_GetVidDecInfo(HL_MTX_OPT_GET, i, &VidDecInfo);
            VidDecInfo.MaxFrameWidth = 256U;
            VidDecInfo.MaxFrameHeight = 144U;
            HL_SetVidDecInfo(HL_MTX_OPT_SET, i, &VidDecInfo);
        }
        VinStartCfg->vin_cap_width = VidDecInfo.MaxFrameWidth;
        VinStartCfg->vin_cap_height = VidDecInfo.MaxFrameHeight;

        HL_GetViewZoneInfoPtr(VidDecInfo.ViewZoneId, &ViewZoneInfo);
        VinStartCfg->no_reset_fp_shared_res = (ViewZoneInfo->IsEffectChanMember == 1U) ? 1U: 0U;
#ifdef DUPLEX_DEC_SHARE_VIRT_VIN
        VinStartCfg->fov_num = (UINT8)HL_GetViewZoneNumOnVin(VinId);
        HL_FillVirtVinStartCfgImplSetFovLayout(VinStartCfg,
                                               VinId,
                                               Resource->MaxViewZoneNum,
                                               Resource->ViewZoneActiveBit);
#endif
    } else if ((DspInstInfo.TestEncVirtVinId != DSP_VIRT_VIN_IDX_INVALID) &&
               (DspInstInfo.TestEncVirtVinId == (VinId - AMBA_DSP_MAX_VIN_NUM))) {
        test_binary_header_t EncHdr = {0};

        VinStartCfg->send_input_data_type = DSP_VIN_SEND_IN_DATA_YUV420;
        VinStartCfg->output_dest = VIN_RAW_DEST_INTERNAL;

        Rval = HL_GetTestEncHeader(&EncHdr, Resource);
        if (Rval == OK) {
            VinStartCfg->vin_cap_width = (UINT16)EncHdr.luma0_w;
            VinStartCfg->vin_cap_height = (UINT16)EncHdr.luma0_h;
        } else {
            // Dummy setting for preventing dsp assert
            VinStartCfg->vin_cap_width = 256U;
            VinStartCfg->vin_cap_height = 144U;
        }
    } else {
        Rval = HL_FillVinStartCfgVirtTimLapse(VinStartCfg,
                                              VinId,
                                              CfgPtn,
                                              Resource);
    }
    return Rval;
}

static inline void HL_FillVinStartCfgImplSetSource(cmd_vin_start_t *pVinStartCfg,
                                                   const UINT8 Purpose,
                                                   const UINT16 YuyvEnable,
                                                   const CTX_VIN_INFO_s *pVinInfo,
                                                   const CTX_VIEWZONE_INFO_s *pViewZoneInfo,
                                                   const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg)
{
    const UINT16 YuyvFactor = 2U; //YUYV occupied 2 times(U/V) of original pixel per line under uncompressed mode
    UINT8 CfaCompressed = (UINT8)0U;
    UINT8 CfaCompact = (UINT8)0U;
    UINT16 RawWidth = 0U, RawPitch = 0U, Mantissa = 0U, BlkSz = 0U;

    if (YuyvEnable > 0U) {
        pVinStartCfg->vin_cap_width = (UINT16)(pVinInfo->CapWindow[0U].Width*YuyvFactor);
    } else {
        pVinStartCfg->vin_cap_width = pVinInfo->CapWindow[0U].Width;
    }
    pVinStartCfg->vin_cap_height = pVinInfo->CapWindow[0U].Height;

    if (Purpose == VIN_START_STILL_RAW_ONLY) {
        if (pVinInfo->VinCtrl.VinState == DSP_VIN_STATUS_TIMER) {
            CfaCompressed = pDataCapCfg->CmprRate;
            CfaCompact = pDataCapCfg->CmptRate;
        } else {
            CfaCompressed = pVinInfo->CfaCompressed;
            CfaCompact = pVinInfo->CfaCompact;
        }
    } else {
        CfaCompressed = pVinInfo->CfaCompressed;
        CfaCompact = pVinInfo->CfaCompact;
    }

    if (CfaCompressed > 0U) {
        (void)HL_GetCmprRawBufInfo(pVinInfo->CapWindow[0U].Width,
                                   CfaCompressed, CfaCompact,
                                   &RawWidth, &RawPitch,
                                   &Mantissa, &BlkSz);
        pVinStartCfg->is_compression_en = (UINT8)1U;
        pVinStartCfg->blk_sz = (UINT8)BlkSz;
        pVinStartCfg->mantissa = (UINT8)Mantissa;
    } else {
        pVinStartCfg->is_compression_en = (UINT8)0U;
        if (CfaCompact > 0U) {
            pVinStartCfg->blk_sz = (UINT8)RawCmptRateTable[CfaCompact].Block;
            pVinStartCfg->mantissa = (UINT8)RawCmptRateTable[CfaCompact].Mantissa;
        } else {
            pVinStartCfg->blk_sz = 0U;
            pVinStartCfg->mantissa = 0U;
        }
    }

    pVinStartCfg->skip_frm_cnt = (YuyvEnable > 0U)? 0U: pVinInfo->SkipFrame;
//FIXME, OtherPurpose
    if (Purpose == VIN_START_STILL_RAW_ONLY) {
        pVinStartCfg->output_dest = VIN_RAW_DEST_EXTERNAL;
    } else {
        pVinStartCfg->output_dest = VIN_RAW_DEST_INTERNAL;
    }

    if (pViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM) {
        pVinStartCfg->input_source = VIN_RAW_SRC_TYPE_RAW;
        pVinStartCfg->send_input_data_type = DSP_VIN_SEND_IN_DATA_RAW;
    } else if (pViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_422) {
        pVinStartCfg->input_source = VIN_RAW_SRC_TYPE_RAW;
        pVinStartCfg->send_input_data_type = DSP_VIN_SEND_IN_DATA_YUV422;
    } else if (pViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_420) {
        pVinStartCfg->input_source = VIN_RAW_SRC_TYPE_RAW;
        pVinStartCfg->send_input_data_type = DSP_VIN_SEND_IN_DATA_YUV420;
    } else if ((pVinInfo->InputFormat == DSP_VIN_INPUT_YUV_422_INTLC) || \
               (pVinInfo->InputFormat == DSP_VIN_INPUT_YUV_422_PROG)) {
        pVinStartCfg->input_source = (YuyvEnable > 0U)? VIN_RAW_SRC_TYPE_SENSOR: VIN_RAW_SRC_TYPE_YUV422;
    } else {
        if (Purpose == VIN_START_VIRT_VIN_LIVEVIEW) {
            pVinStartCfg->input_source = VIN_RAW_SRC_TYPE_RAW; //TBD
        } else {
            pVinStartCfg->input_source = VIN_RAW_SRC_TYPE_SENSOR; //TBD
        }
        pVinStartCfg->send_input_data_type = DSP_VIN_SEND_IN_DATA_RAW; //TBD
    }

    // Embeded Data
    if ((pVinInfo->EmbdCapWin.Width > 0U) &&
        (pVinInfo->EmbdCapWin.Height > 0U)) {
        pVinStartCfg->vin_aux_out_en = (UINT8)1U;
        pVinStartCfg->vin_aux_cap_width = pVinInfo->EmbdCapWin.Width;
        pVinStartCfg->vin_aux_cap_height = pVinInfo->EmbdCapWin.Height;
    }
}

static inline void HL_GetFovLayoutActualHeight(UINT16 *pActualHeight,
                                               const CTX_VIEWZONE_INFO_s *pViewZoneInfo)
{
    UINT16 ActualHeight = 0U;

    if (pViewZoneInfo->HdrBlendNumMinusOne > 0U) {
        UINT16 BldNumMiOne = (UINT16)pViewZoneInfo->HdrBlendNumMinusOne;
        UINT16 BldHeight = (UINT16)pViewZoneInfo->HdrBlendHeight;

        ActualHeight = (UINT16)(BldHeight*(BldNumMiOne+1U)) + (UINT16)pViewZoneInfo->HdrRawYOffset[BldNumMiOne] - 1U;
    } else {
        ActualHeight = pViewZoneInfo->CapWindow.Height;
    }

    *pActualHeight = ActualHeight;
}

static inline void HL_GetViewzoneInterlaceNum(UINT16 *pViewZoneInterlace,
                                              const CTX_VIN_INFO_s *pVinInfo)
{
    UINT16 ViewZoneInterlace;
    if (pVinInfo->Option[0U] == AMBA_DSP_VIN_CAP_OPT_INTC) {
        if (pVinInfo->IntcNum[0U] == 0U) {
            ViewZoneInterlace = 0U;
        } else {
            ViewZoneInterlace = pVinInfo->IntcNum[0U] - 1U;
        }
    } else {
        ViewZoneInterlace = 0U;
    }

    *pViewZoneInterlace = ViewZoneInterlace;
}

static inline void HL_FillVinStartCfgImplSliceCfg(UINT16 i,
                                                  const CTX_VIEWZONE_INFO_s *pViewZoneInfo,
                                                  vin_fov_cfg_t *pDspVinFovLayout,
                                                  UINT16 IntcNumMinusOne)
{
    if (pViewZoneInfo->SliceNumRow > (UINT8)1U) {
        (void)HL_FillSliceCapLine(i, &pDspVinFovLayout->slice_term_cap_line[0U], IntcNumMinusOne);
    } else {
        (void)dsp_osal_memset(&pDspVinFovLayout->slice_term_cap_line[0U],
                              0,
                              sizeof(UINT16)*DSP_MAX_VIN_CAP_SLICE_NUM);
    }
}

static inline void HL_FillVinStartCfgImplIsoCfg(cmd_vin_start_t *pVinStartCfg,
                                                const UINT16 ViewZoneId,
                                                vin_fov_cfg_t *pDspVinFovLayout)
{
    UINT32 Rval;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
    if (ViewZoneInfo->IkIsoAddr != 0U) {
        UINT32 *pBatchQAddr, BatchCmdId;
        UINT32 NewWp = 0U;
        ULONG BatchQAddr = 0U;
        cmd_vproc_ik_config_t *pIsoCfgCmd;

        /* Update following ViewZone IsoCfgStartAddr */
        ViewZoneInfo->StartIsoCfgAddr = ViewZoneInfo->IkIsoAddr;
        ViewZoneInfo->StartIsoCfgIndex = ViewZoneInfo->IkId;

        /* Request BatchCmdQ buffer */
        Rval = DSP_ReqBuf(&ViewZoneInfo->BatchQPoolDesc, 1U, &NewWp, 1U/*FullnessCheck*/);
        if (Rval != OK) {
            ULONG NullAddr = 0U;

            HL_GetViewZoneInfoUnLock(ViewZoneId);
            (void)dsp_osal_virt2cli(NullAddr, &pVinStartCfg->batch_cmd_set_info.addr);
            pVinStartCfg->batch_cmd_set_info.id = 0U;
            pVinStartCfg->batch_cmd_set_info.size = 0U;

            AmbaLL_LogWarnUInt5((UINT16)AMBALLLOG_WARN_BATCHQ_FULL, "[FillVinStartCfg] ViewZone(%d) batch pool is full", ViewZoneId, 0U, 0U, 0U, 0U);
        } else {
            ViewZoneInfo->BatchQPoolDesc.Wp = NewWp;
            HL_GetViewZoneInfoUnLock(ViewZoneId);
            HL_GetPointerToDspBatchQ(ViewZoneId, (UINT16)ViewZoneInfo->BatchQPoolDesc.Wp, &pBatchQAddr, &BatchCmdId);

            /* Reset New BatchQ after Wp advanced */
            HL_ResetDspBatchQ(pBatchQAddr);

            /* IsoCfgCmd occupied first CmdSlot */
            dsp_osal_typecast(&pIsoCfgCmd, &pBatchQAddr);
            (void)dsp_osal_memset(pIsoCfgCmd, 0, sizeof(DSP_CMD_s));
            (void)HL_FillIsoCfgUpdate(ViewZoneId, pIsoCfgCmd);
            (void)dsp_osal_virt2cli(ViewZoneInfo->IkIsoAddr, &pIsoCfgCmd->idsp_flow_addr);
            pIsoCfgCmd->ik_cfg_id = ViewZoneInfo->IkId;

            dsp_osal_typecast(&BatchQAddr, &pBatchQAddr);
            (void)dsp_osal_virt2cli(BatchQAddr, &pDspVinFovLayout->batch_cmd_set_info.addr);
            HL_SetDspBatchQInfo(BatchCmdId, 1/*AutoReset*/, BATCHQ_INFO_ISO_CFG, (ViewZoneInfo->IkId > 0U)? ViewZoneInfo->IkId: 0x1U);
            pDspVinFovLayout->batch_cmd_set_info.id = BatchCmdId; // uCode need non-null
            pDspVinFovLayout->batch_cmd_set_info.size = HL_GetBatchCmdNumber(BatchQAddr)*CMD_SIZE_IN_BYTE;

            dsp_osal_typecast(&BatchQAddr, &pBatchQAddr);
        }
    } else {
        ULONG NullAddr = 0U;

        HL_GetViewZoneInfoUnLock(ViewZoneId);
        (void)dsp_osal_virt2cli(NullAddr, &pVinStartCfg->batch_cmd_set_info.addr);
        pDspVinFovLayout->batch_cmd_set_info.id = 0U;
        pDspVinFovLayout->batch_cmd_set_info.size = 0U;
        AmbaLL_LogUInt5("!!! Null StartIsoCfg, Fill IsoCfg as Zero", 0U, 0U, 0U, 0U, 0U);
    }
}

static inline void HL_FillVinStartCfgImplSetFovLayout(cmd_vin_start_t *pVinStartCfg,
                                                      const UINT16 VinId,
                                                      const UINT16 YuyvEnable,
                                                      const UINT16 MaxViewZoneNum,
                                                      const UINT32 ViewZoneActiveBit,
                                                      const CTX_VIN_INFO_s *pVinInfo)
{
    UINT8 FovCnt = 0U;
    UINT16 i, ViewZoneVinId, ViewZoneInterlace = 0;
    vin_fov_cfg_t *pDspVinFovLayout = NULL;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    ULONG ULAddr;

    HL_GetViewzoneInterlaceNum(&ViewZoneInterlace, pVinInfo);

    for (i = 0U; i < MaxViewZoneNum; i++) {
        if (0U == DSP_GetBit(ViewZoneActiveBit, i, 1U)) {
            continue;
        }
        HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
        if (ViewZoneInfo->Pipe == DSP_DRAM_PIPE_RAW_ONLY) {
            continue;
        }

        (void)HL_GetViewZoneVinId(i, &ViewZoneVinId);
        if (ViewZoneVinId == (UINT32)VinId) {
            UINT16 ActualHeight = 0U;

            HL_GetPointerToDspVinStartFovCfg(VinId, (UINT8)FovCnt, &pDspVinFovLayout);
            pDspVinFovLayout->chan_id = FovCnt;
            pDspVinFovLayout->xpitch = (UINT8)(ViewZoneInterlace + 1U);
            pDspVinFovLayout->hdr_intlac_mode = HL_GetFovLayoutHdrIntlacMode(ViewZoneInfo->HdrBlendNumMinusOne, ViewZoneInterlace);
            pDspVinFovLayout->is_fov_active = 1U;
            pDspVinFovLayout->x_offset = ViewZoneInfo->CapWindow.OffsetX;
            pDspVinFovLayout->y_offset = ViewZoneInfo->CapWindow.OffsetY;
            pDspVinFovLayout->width = (YuyvEnable > 0U) ? (UINT16)(ViewZoneInfo->CapWindow.Width*2U) : \
                                                          ViewZoneInfo->CapWindow.Width;
            pDspVinFovLayout->is_last_fov = HL_IsLayoutLastFov(pVinStartCfg->fov_num, FovCnt);
            if (FovCnt == 0U) {
                pDspVinFovLayout->batch_cmd_set_info.addr = pVinStartCfg->batch_cmd_set_info.addr;
                pDspVinFovLayout->batch_cmd_set_info.id = pVinStartCfg->batch_cmd_set_info.id;
                pDspVinFovLayout->batch_cmd_set_info.size = pVinStartCfg->batch_cmd_set_info.size;
            } else {
                HL_FillVinStartCfgImplIsoCfg(pVinStartCfg, i, pDspVinFovLayout);
            }

            HL_GetFovLayoutActualHeight(&ActualHeight, ViewZoneInfo);
            pDspVinFovLayout->height = ActualHeight;

            /* SliceCfg */
            /* uCode will use largest TermCapLine in whole FOV(at same Vin) for every slice entry timing  */
            HL_FillVinStartCfgImplSliceCfg(i, ViewZoneInfo, pDspVinFovLayout, ViewZoneInterlace);

            FovCnt++;
            if (FovCnt == pVinStartCfg->fov_num) {
                break;
            }
        }
    }

    if (FovCnt > 0U) {
        HL_GetPointerToDspVinStartFovCfg(VinId, (UINT8)0U, &pDspVinFovLayout);
        dsp_osal_typecast(&ULAddr, &pDspVinFovLayout);
    } else {
        ULAddr = 0U;
    }
    (void)dsp_osal_virt2cli(ULAddr, &pVinStartCfg->fov_cfg_tbl_daddr);
}

static inline UINT32 HL_FillVinStartCfgImpl(cmd_vin_start_t *VinStartCfg,
                                            UINT16 VinId,
                                            UINT8 Purpose,
                                            UINT16 CfgPtn,
                                            const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg)
{
    UINT16 Idx;
    UINT16 ViewZoneId = 0U;
    UINT16 PhyVinId = DSP_VIRT_VIN_IDX_INVALID;
    UINT16 YuyvEnable = 0U;
    UINT32 Rval = OK;
    CTX_VIN_INFO_s VinInfo = {0};
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    ULONG NullAddr = 0U;

    if (Purpose == VIN_START_VIRT_VIN_LIVEVIEW) { //TimeDivision case
        (void)HL_GetTimeDivisionVirtVinInfo(VinId, &PhyVinId);
    } else {
        PhyVinId = VinId;
    }

    HL_GetResourcePtr(&Resource);
    HL_GetVinInfo(HL_MTX_OPT_ALL, PhyVinId, &VinInfo);
    (void)HL_GetVinMipiYuyvInfo(VinId, &VinInfo, &YuyvEnable);
    for (Idx = 0U; Idx < Resource->MaxViewZoneNum; Idx++) {
        if (0U == DSP_GetBit(Resource->ViewZoneActiveBit, Idx, 1U)) {
            continue;
        }
        HL_GetViewZoneInfoPtr(Idx, &ViewZoneInfo);

        if ((ViewZoneInfo->SourceVin == VinId) &&
            (ViewZoneInfo->Pipe == DSP_DRAM_PIPE_MIPI_YUV)) {
                YuyvEnable = (UINT16)1U;
                break;
        }
    }

    /** Fetch first viewzone information of this Vin */
    if (HL_FetchFirstViewZoneInfo(VinId, &ViewZoneId, &ViewZoneInfo) == 1U) {
        HL_UpdateViewzoneStartIsoCfg(CfgPtn, ViewZoneId);
        if ((CfgPtn & VIN_START_CFG_CMD) > 0U) {
            VinStartCfg->vin_id = (UINT8)VinId;

            //FIXME, New
            VinStartCfg->vin_poll_intvl = 0U;

            VinStartCfg->cmd_msg_decimation_rate = 1U; //TBD for HFR

            HL_FillVinStartCfgImplSetSource(VinStartCfg,
                                            Purpose,
                                            YuyvEnable,
                                            &VinInfo,
                                            ViewZoneInfo,
                                            pDataCapCfg);

    //FIXME, CVX Sync for Vin
            VinStartCfg->is_reset_frm_sync = 0U;
            VinStartCfg->is_auto_frm_drop_en = 0U;
            VinStartCfg->is_check_timestamp = 0U;

            if (Purpose != VIN_START_STILL_RAW_ONLY) {
                /* IsoCfg */
                if (ViewZoneInfo->Pipe != DSP_DRAM_PIPE_RAW_ONLY) {
                    Rval = HL_UpdateViewzoneStartIsoCfgSetup(VinStartCfg, ViewZoneId);
                    if (Rval != OK) {
                        AmbaLL_LogUInt5("[FillVinStartCfg] Vin[%d] ViewZone[%u] fail", VinId, ViewZoneId, 0U, 0U, 0U);
                    }
                }

                /* DefaultRaw/CE */
                VinStartCfg->capture_time_out_msec = Resource->MaxVinTimeout[PhyVinId];
#ifdef SUPPORT_DSP_VIN_DEFT_RAW
                (void)dsp_osal_virt2cli(Resource->DefaultRawBuf[PhyVinId].BaseAddr, &VinStartCfg->default_raw_image_address);
                VinStartCfg->default_raw_image_pitch = Resource->DefaultRawBuf[PhyVinId].Pitch;
                (void)dsp_osal_virt2cli(Resource->DefaultAuxRawBuf[PhyVinId].BaseAddr, &VinStartCfg->default_ce_image_address);
                VinStartCfg->default_ce_image_pitch = Resource->DefaultAuxRawBuf[PhyVinId].Pitch;
#endif
                /* ViewZone at Vin */
                VinStartCfg->fov_num = (UINT8)HL_GetViewZoneNumOnVin(VinId);

            //FIXME, SubChan
                HL_FillVinStartCfgImplSetFovLayout(VinStartCfg,
                                                   VinId,
                                                   YuyvEnable,
                                                   Resource->MaxViewZoneNum,
                                                   Resource->ViewZoneActiveBit,
                                                   &VinInfo);
            } else {
                /* IsoCfg */
                (void)dsp_osal_virt2cli(NullAddr, &VinStartCfg->batch_cmd_set_info.addr);
                VinStartCfg->batch_cmd_set_info.id = 0U;
                VinStartCfg->batch_cmd_set_info.size = 0U;

                /* DefaultRaw/CE */
                VinStartCfg->capture_time_out_msec = 0U;
#ifdef SUPPORT_DSP_VIN_DEFT_RAW
                VinStartCfg->default_raw_image_address = 0U;
                VinStartCfg->default_raw_image_pitch = 0U;
                VinStartCfg->default_ce_image_address = 0U;
                VinStartCfg->default_ce_image_pitch = 0U;
#endif
                /* ViewZone at Vin */
                VinStartCfg->fov_num = (UINT8)0U;

                /* FovLayout */
                VinStartCfg->fov_cfg_tbl_daddr = 0U;

                /* IsoCfg */
                (void)dsp_osal_memset(&VinStartCfg->batch_cmd_set_info, 0, sizeof(batch_cmd_set_info_t));
            }

    //FIXME, Repeat/Drop MailBox
#ifdef SUPPORT_DSP_VIN_FRM_CTRL
            VinStartCfg->capture_compl_cntl_msg_addr = 0U;
            VinStartCfg->compl_cntl_msg_update_time_msec = 0U;
#endif

            /* LowDelay */
            HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
            if (ViewZoneInfo->SliceNumRow > (UINT8)1U) {
                VinStartCfg->is_vin_cap_presend = 1U;
                VinStartCfg->cap_slice_num = ViewZoneInfo->SliceNumRow;
            } else {
                VinStartCfg->is_vin_cap_presend = 0U;
                VinStartCfg->cap_slice_num = 1U;
            }

    //FIXME, should consider effect
            VinStartCfg->no_reset_fp_shared_res = 1U;

            if (Purpose == VIN_START_STILL_RAW_ONLY) {
                VinStartCfg->vin_ce_out_reset = 1U;
            }
        }
    } else {
        AmbaLL_LogUInt5("HL_FillVinStartCfgImpl [%d]cannot find ViewZone", VinId, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0001;
    }
    return Rval;
}

static UINT32 FillVinStartCfg(cmd_vin_start_t *VinStartCfg,
                          UINT16 VinId,
                          UINT8 Purpose,
                          UINT16 CfgPtn,
                          const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg)
{
    UINT32 Rval = OK;

    if (VinStartCfg == NULL) {
        Rval = DSP_ERR_0000;
    } else if (Purpose == VIN_START_VIRT_VIN) {
        Rval = HL_FillVinStartCfgVirt(VinStartCfg, VinId, CfgPtn);
    } else if ((Purpose == VIN_START_VIRT_VIN_LIVEVIEW) ||
               (Purpose == VIN_START_LIVEVIEW_ONLY) ||
               (Purpose == VIN_START_STILL_RAW_ONLY)) {
        Rval = HL_FillVinStartCfgImpl(VinStartCfg, VinId, Purpose, CfgPtn, pDataCapCfg);
    } else {
        //TBD
    }
    return Rval;
}
UINT32 HL_FillVinStartCfg(cmd_vin_start_t *VinStartCfg,
                          UINT16 VinId,
                          UINT8 Purpose,
                          UINT16 CfgPtn,
                          const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg)
{
    UINT32 Rval;
    UINT32 CrcHL, CrcSimilar;
    const UINT32 *pU32 = NULL;
    UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;
#ifdef CONFIG_ENABLE_DSP_MONITOR
    AMBA_DSP_MONITOR_CRC_CMPR_s CrcCmpr;
#endif

    Rval = FillVinStartCfg(VinStartCfg, VinId, Purpose, CfgPtn, pDataCapCfg);
    if (pIsSimilarEnabled != NULL) {
        if (1U == IsSimilarEnabled()) {
            UINT32 SIM_Rval;
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_vin_start_t *SimVinStartCfg = SIM_DefCtxCmdBufPtrVinStart;

            dsp_osal_typecast(&pU32, &VinStartCfg);
            CrcHL = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_vin_start_t), 0U);

            SIM_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&SimVinStartCfg, &CmdBufferAddr);
            SIM_Rval = SIM_FillVinStartCfg(SimVinStartCfg, VinId, Purpose, CfgPtn, pDataCapCfg);
            if (SIM_Rval == OK) {
                dsp_osal_typecast(&pU32, &SimVinStartCfg);
                CrcSimilar = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_vin_start_t), 0U);
#ifdef CONFIG_ENABLE_DSP_MONITOR
                CrcCmpr.ModuleId = AMBA_SSP_PRINT_MODULE_ID;
                CrcCmpr.Crc0 = CrcHL;
                CrcCmpr.Crc1 = CrcSimilar;
                CrcCmpr.UUID = __LINE__;
                SIM_Rval = dsp_mon_crc_cmpr(&CrcCmpr);
                if (SIM_Rval != OK) {
                    AmbaLL_LogUInt5("dsp_mon_crc_cmpr return 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
                }
#else
                if (CrcSimilar != CrcHL) {
                    AmbaLL_LogUInt5("[ERR] Similar CRC mismatch @ %d HL 0x%x Similar 0x%x", __LINE__, CrcHL, CrcSimilar, 0U, 0U);
                }
#endif
            } else {
                AmbaLL_LogUInt5("[ERR] Similar function returned NG 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
            }
            SIM_RelCmdBuffer(CmdBufferId);
        }
    }

    return Rval;
}

static UINT32 FillVinHdrSetup(UINT16 VinId, cmd_vin_hdr_setup_t *HdrSetup, ULONG CfgAddr, UINT8 *SkipCmd)
{
    UINT32 Rval = OK;
    CTX_VIN_INFO_s VinInfo = {0};
    CTX_SENSOR_INFO_s SensorInfo = {0};
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 ViewZoneId = 0;
(void)CfgAddr;
    if (HdrSetup == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        /** Fetch first viewzone information of this Vin */
        if (HL_FetchFirstViewZoneInfo(VinId, &ViewZoneId, &ViewZoneInfo) == 1U) {

            /* Update HDR RawInfo YOffset */
            HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
            if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_HW) ||
                (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM)) {

                /* Sanity check once Yoffset[1/2] = 0 */
                if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM) {
                    if ((ViewZoneInfo->HdrBlendNumMinusOne == 1U) &&
                        (ViewZoneInfo->HdrRawYOffset[1U] == 0U)) {
                        ViewZoneInfo->HdrRawYOffset[0U] = 0U; //LE
                        ViewZoneInfo->HdrRawYOffset[1U] = 1U; //SE
                    } else if ((ViewZoneInfo->HdrBlendNumMinusOne == 2U) &&
                               ((ViewZoneInfo->HdrRawYOffset[1U] == 0U) ||
                                (ViewZoneInfo->HdrRawYOffset[2U] == 0U))) {
                        ViewZoneInfo->HdrRawYOffset[0U] = 0U; //LE
                        ViewZoneInfo->HdrRawYOffset[1U] = 1U; //SE
                        ViewZoneInfo->HdrRawYOffset[2U] = 2U; //VSE
                    } else {
                        // DO NOTHING
                    }
                }
            } else {
                // DO NOTHING
            }
            HL_GetViewZoneInfoUnLock(ViewZoneId);

            HL_GetVinInfo(HL_MTX_OPT_ALL, VinId, &VinInfo);

            HL_GetSensorInfo(HL_MTX_OPT_ALL, 0U/*FirstSensor*/, &SensorInfo);
            HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);

            /* Check duplicate cmd */
            if ((VinInfo.CmdUpdate.HdrSetup > 0U) ||
                (ViewZoneInfo->CmdUpdate.HdrSetup > 0U) ||
                (*SkipCmd == (UINT8)0xFFU)) {
                // Reset CmdUpdate flag
                if ((VinInfo.CmdUpdate.HdrSetup > 0U) &&
                    (*SkipCmd != (UINT8)0xFFU)) {
                    HL_GetVinInfo(HL_MTX_OPT_GET, VinId, &VinInfo);
                    VinInfo.CmdUpdate.HdrSetup--;
                    HL_SetVinInfo(HL_MTX_OPT_SET, VinId, &VinInfo);
                }
                if ((ViewZoneInfo->CmdUpdate.HdrSetup > 0U) &&
                    (*SkipCmd != (UINT8)0xFFU)) {
                    HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
                    ViewZoneInfo->CmdUpdate.HdrSetup--;
                    HL_GetViewZoneInfoUnLock(ViewZoneId);
                }

                HdrSetup->vin_id = (UINT8)VinId;
                HdrSetup->num_exp_minus_1 = ViewZoneInfo->HdrBlendNumMinusOne;

                if (ViewZoneInfo->HdrBlendNumMinusOne == 1U) {
                    HdrSetup->exp_vert_offset_table[0U] = ViewZoneInfo->HdrRawYOffset[0U]; //LE
                    HdrSetup->exp_vert_offset_table[1U] = ViewZoneInfo->HdrRawYOffset[1U]; //SE
                } else if (ViewZoneInfo->HdrBlendNumMinusOne == 2U) {
                    HdrSetup->exp_vert_offset_table[0U] = ViewZoneInfo->HdrRawYOffset[0U]; //LE
                    HdrSetup->exp_vert_offset_table[1U] = ViewZoneInfo->HdrRawYOffset[1U]; //SE
                    HdrSetup->exp_vert_offset_table[2U] = ViewZoneInfo->HdrRawYOffset[2U]; //VSE
                } else {
                    // DO NOTHING
                }
            } else {
                *SkipCmd = 1U;
            }
        } else {
            AmbaLL_LogUInt5("HL_FillVinHdrSetup [%d]cannot find ViewZone", VinId, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
        }
    }

    return Rval;
}
UINT32 HL_FillVinHdrSetup(UINT16 VinId, cmd_vin_hdr_setup_t *HdrSetup, ULONG CfgAddr, UINT8 *SkipCmd)
{
    UINT32 Rval;
    UINT32 CrcHL, CrcSimilar;
    const UINT32 *pU32 = NULL;
    UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;
#ifdef CONFIG_ENABLE_DSP_MONITOR
    AMBA_DSP_MONITOR_CRC_CMPR_s CrcCmpr;
#endif

    Rval = FillVinHdrSetup(VinId, HdrSetup, CfgAddr, SkipCmd);
    if (pIsSimilarEnabled != NULL) {
        if ((1U == IsSimilarEnabled()) &&
            (*SkipCmd != (UINT8)0x1U)) {
            UINT32 SIM_Rval;
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_vin_hdr_setup_t *SimHdrSetup = SIM_DefCtxCmdBufPtrVinHdr;

            UINT8 SimSkipCmd = (UINT8)0xFFU;
            dsp_osal_typecast(&pU32, &HdrSetup);
            CrcHL = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_vin_hdr_setup_t), 0U);

            SIM_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&SimHdrSetup, &CmdBufferAddr);
            SIM_Rval = SIM_FillVinHdrSetup(VinId, SimHdrSetup, CfgAddr, &SimSkipCmd);
            if (SIM_Rval == OK) {
                dsp_osal_typecast(&pU32, &SimHdrSetup);
                CrcSimilar = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_vin_hdr_setup_t), 0U);
#ifdef CONFIG_ENABLE_DSP_MONITOR
                CrcCmpr.ModuleId = AMBA_SSP_PRINT_MODULE_ID;
                CrcCmpr.Crc0 = CrcHL;
                CrcCmpr.Crc1 = CrcSimilar;
                CrcCmpr.UUID = __LINE__;
                SIM_Rval = dsp_mon_crc_cmpr(&CrcCmpr);
                if (SIM_Rval != OK) {
                    AmbaLL_LogUInt5("dsp_mon_crc_cmpr return 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
                }
#else
                if (CrcSimilar != CrcHL) {
                    AmbaLL_LogUInt5("[ERR] Similar CRC mismatch @ %d HL 0x%x Similar 0x%x", __LINE__, CrcHL, CrcSimilar, 0U, 0U);
                }
#endif
            } else {
                AmbaLL_LogUInt5("[ERR] Similar function returned NG 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
            }
            SIM_RelCmdBuffer(CmdBufferId);
        }
    }

    return Rval;
}

static UINT32 FillVinCeSetup(UINT16 VinId, cmd_vin_ce_setup_t *CeSetup, UINT8 *SkipCmd)
{
    UINT32 Rval = OK;
    CTX_VIN_INFO_s VinInfo = {0};
    CTX_SENSOR_INFO_s SensorInfo = {0};
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 ViewZoneId = 0;

    if (CeSetup == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        /** Fetch first viewzone information of this Vin */
        if (HL_FetchFirstViewZoneInfo(VinId, &ViewZoneId, &ViewZoneInfo) == 1U) {
            HL_GetVinInfo(HL_MTX_OPT_ALL, VinId, &VinInfo);
            HL_GetSensorInfo(HL_MTX_OPT_ALL, 0U/*FirstSensor*/, &SensorInfo);

            /* Check duplicate cmd */
            if ((VinInfo.CmdUpdate.CeSetup > 0U) ||
                (ViewZoneInfo->CmdUpdate.CeSetup > 0U) ||
                (*SkipCmd == (UINT8)0xFFU)) {

                if (IsAligned4(VinInfo.CapWindow[0U].Width) == 1U) {
                    // Reset CmdUpdate flag
                    if ((VinInfo.CmdUpdate.CeSetup > 0U) &&
                        (*SkipCmd != (UINT8)0xFFU)) {
                        HL_GetVinInfo(HL_MTX_OPT_GET, VinId, &VinInfo);
                        VinInfo.CmdUpdate.CeSetup--;
                        HL_SetVinInfo(HL_MTX_OPT_SET, VinId, &VinInfo);
                    }
                    if ((ViewZoneInfo->CmdUpdate.CeSetup > 0U) &&
                        (*SkipCmd != (UINT8)0xFFU)) {
                        HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
                        ViewZoneInfo->CmdUpdate.CeSetup--;
                        HL_GetViewZoneInfoUnLock(ViewZoneId);
                    }
                    CeSetup->vin_id = (UINT8)VinId;
                    CeSetup->ce_width = VinInfo.CapWindow[0U].Width/4U; //Fixed 4x DownScale
                    CeSetup->ce_height = VinInfo.CapWindow[0U].Height;
                } else {
                    AmbaLL_LogUInt5("HL_FillVinCeSetup [%d]RawCapW[%d] must be 4 align", VinId, VinInfo.CapWindow[0U].Width, 0U, 0U, 0U);
                    Rval = DSP_ERR_0001;
                }
            } else {
                *SkipCmd = 1U;
            }
        } else {
            AmbaLL_LogUInt5("HL_FillVinCeSetup [%d]cannot find ViewZone", VinId, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
        }
    }

    return Rval;
}

UINT32 HL_FillVinCeSetup(UINT16 VinId, cmd_vin_ce_setup_t *CeSetup, UINT8 *SkipCmd)
{
    UINT32 Rval;
    UINT32 CrcHL, CrcSimilar;
    const UINT32 *pU32 = NULL;
    UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;
#ifdef CONFIG_ENABLE_DSP_MONITOR
    AMBA_DSP_MONITOR_CRC_CMPR_s CrcCmpr;
#endif

    Rval = FillVinCeSetup(VinId, CeSetup, SkipCmd);
    if (pIsSimilarEnabled != NULL) {
        if ((1U == IsSimilarEnabled()) &&
            (*SkipCmd != (UINT8)0x1U)) {
            UINT8 SimSkipCmd = (UINT8)0xFFU;
            UINT32 SIM_Rval;
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_vin_ce_setup_t *SimCeSetup = SIM_DefCtxCmdBufPtrVinCe;

            dsp_osal_typecast(&pU32, &CeSetup);
            CrcHL = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_vin_ce_setup_t), 0U);

            SIM_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&SimCeSetup, &CmdBufferAddr);
            SIM_Rval = SIM_FillVinCeSetup(VinId, SimCeSetup, &SimSkipCmd);
            if (SIM_Rval == OK) {
                dsp_osal_typecast(&pU32, &SimCeSetup);
                CrcSimilar = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_vin_ce_setup_t), 0U);
#ifdef CONFIG_ENABLE_DSP_MONITOR
                CrcCmpr.ModuleId = AMBA_SSP_PRINT_MODULE_ID;
                CrcCmpr.Crc0 = CrcHL;
                CrcCmpr.Crc1 = CrcSimilar;
                CrcCmpr.UUID = __LINE__;
                SIM_Rval = dsp_mon_crc_cmpr(&CrcCmpr);
                if (SIM_Rval != OK) {
                    AmbaLL_LogUInt5("dsp_mon_crc_cmpr return 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
                }
#else
                if (CrcSimilar != CrcHL) {
                    AmbaLL_LogUInt5("[ERR] Similar CRC mismatch @ %d HL 0x%x Similar 0x%x", __LINE__, CrcHL, CrcSimilar, 0U, 0U);
                }
#endif
            } else {
                AmbaLL_LogUInt5("[ERR] Similar function returned NG 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
            }
            SIM_RelCmdBuffer(CmdBufferId);
        }
    }

    return Rval;
}

static UINT32 FillVinSetFovLayout(cmd_vin_set_fov_layout_t *pSetFovLayout, UINT16 ViewZoneId, const AMBA_DSP_WINDOW_s* VinROI)
{
    UINT32 Rval = OK;
    UINT16 ViewZoneInterlace = 0, LocalViewZoneIdx;
    UINT16 VinId, ViewZoneNumOnVin;
    UINT16 ActualHeight;
    CTX_VIN_INFO_s VinInfo = {0};
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    fov_layout_t *pFovLayout = NULL;
    ULONG ULAddr;

    HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
    DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &VinId);
    HL_GetVinInfo(HL_MTX_OPT_ALL, VinId, &VinInfo);

    LocalViewZoneIdx = (UINT16)HL_GetViewZoneLocalIndexOnVin(ViewZoneId);
    ViewZoneNumOnVin = (UINT16)HL_GetViewZoneNumOnVin(VinId);

    HL_GetPointerToDspVinFovLayoutCfg(VinId, LocalViewZoneIdx, &pFovLayout);

    pSetFovLayout->vin_id = (UINT8)VinId;
    pSetFovLayout->num_of_fovs = 1U;

    //FIXME, SubChan
    if (VinInfo.Option[0U] == AMBA_DSP_VIN_CAP_OPT_INTC) {
        if (VinInfo.IntcNum[0U] == 0U) {
            ViewZoneInterlace = 0U;
        } else {
            ViewZoneInterlace = VinInfo.IntcNum[0U] - 1U;
        }
    } else {
        ViewZoneInterlace = 0U;
    }

    pFovLayout->chan_id = LocalViewZoneIdx;

    if ((VinROI == NULL) || (VinROI->Width == 0U) || (VinROI->Height == 0U)) {
        pFovLayout->img_x_ofset = ViewZoneInfo->CapWindow.OffsetX;
        pFovLayout->img_y_ofset = ViewZoneInfo->CapWindow.OffsetY;
        pFovLayout->img_width = ViewZoneInfo->CapWindow.Width;
        if (ViewZoneInfo->HdrBlendNumMinusOne > 0U) {
            ActualHeight = (UINT16)(ViewZoneInfo->HdrBlendHeight*((UINT16)ViewZoneInfo->HdrBlendNumMinusOne+1U)) + ViewZoneInfo->HdrRawYOffset[ViewZoneInfo->HdrBlendNumMinusOne] - 1U;
        } else {
            ActualHeight = ViewZoneInfo->CapWindow.Height;
        }
        pFovLayout->img_height = ActualHeight;
    } else {
        pFovLayout->img_x_ofset = VinROI->OffsetX;
        pFovLayout->img_y_ofset = VinROI->OffsetY;
        pFovLayout->img_width   = VinROI->Width;
        pFovLayout->img_height  = VinROI->Height;
    }

    pFovLayout->xpitch = (UINT8)(ViewZoneInterlace + 1U);
    pFovLayout->hdr_intlac_mode = HL_GetFovLayoutHdrIntlacMode(ViewZoneInfo->HdrBlendNumMinusOne, ViewZoneInterlace);
    pFovLayout->is_fov_active = 1U;
    pFovLayout->is_last_fov = HL_IsLayoutLastFov((UINT8)ViewZoneNumOnVin, (UINT8)LocalViewZoneIdx);
    dsp_osal_typecast(&ULAddr, &pFovLayout);
    (void)dsp_osal_virt2cli(ULAddr, &pSetFovLayout->fov_lay_out_ptr);

    return Rval;
}
UINT32 HL_FillVinSetFovLayout(cmd_vin_set_fov_layout_t *pSetFovLayout, UINT16 ViewZoneId, const AMBA_DSP_WINDOW_s* VinROI)
{
    UINT32 Rval;
    UINT32 CrcHL, CrcSimilar;
    const UINT32 *pU32 = NULL;
    UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;
#ifdef CONFIG_ENABLE_DSP_MONITOR
    AMBA_DSP_MONITOR_CRC_CMPR_s CrcCmpr;
#endif

    Rval = FillVinSetFovLayout(pSetFovLayout, ViewZoneId, VinROI);
    if (pIsSimilarEnabled != NULL) {
        if (1U == IsSimilarEnabled()) {
            UINT32 SIM_Rval;
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_vin_set_fov_layout_t *SimSetFovLayout = SIM_DefCtxCmdBufPtrVinLayout;

            dsp_osal_typecast(&pU32, &pSetFovLayout);
            CrcHL = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_vin_set_fov_layout_t), 0U);

            SIM_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&SimSetFovLayout, &CmdBufferAddr);
            SIM_Rval = SIM_FillVinSetFovLayout(SimSetFovLayout, ViewZoneId, VinROI);
            if (SIM_Rval == OK) {
                dsp_osal_typecast(&pU32, &SimSetFovLayout);
                CrcSimilar = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_vin_set_fov_layout_t), 0U);
#ifdef CONFIG_ENABLE_DSP_MONITOR
                CrcCmpr.ModuleId = AMBA_SSP_PRINT_MODULE_ID;
                CrcCmpr.Crc0 = CrcHL;
                CrcCmpr.Crc1 = CrcSimilar;
                CrcCmpr.UUID = __LINE__;
                SIM_Rval = dsp_mon_crc_cmpr(&CrcCmpr);
                if (SIM_Rval != OK) {
                    AmbaLL_LogUInt5("dsp_mon_crc_cmpr return 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
                }
#else
                if (CrcSimilar != CrcHL) {
                    AmbaLL_LogUInt5("[ERR] Similar CRC mismatch @ %d HL 0x%x Similar 0x%x", __LINE__, CrcHL, CrcSimilar, 0U, 0U);
                }
#endif
            } else {
                AmbaLL_LogUInt5("[ERR] Similar function returned NG 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
            }
            SIM_RelCmdBuffer(CmdBufferId);
        }
    }

    return Rval;
}

static inline void HL_PpNoneExist(const UINT16 YuvStrmIdx,
                                  const UINT8 YuvStrmCfgStrId,
                                  const UINT16 ViewZoneId,
                                  const UINT16 MinViewZoneIdx,
                                  const UINT16 BufDescOutputBufId,
                                  const UINT16 PpStrmBufIdx,
                                  const UINT16 PinId,
                                  UINT32 *pTmpBufferUsage,
                                  CTX_YUV_STRM_INFO_s *pYuvStrmInfo,
                                  UINT16 *pNumView,
                                  UINT8 *pNeedDummyBlend)
{
    UINT8 NeedDummyBlend = *pNeedDummyBlend;
    UINT16 NumView = *pNumView;
    vproc_pp_stream_cntl_t *pPpStrmCtrl = NULL;
    ULONG NullAddr = 0U;

#ifndef PPSTRM_SWITCH_CHANGE_INPUTS_NUM
    if (DSP_GetBit(pYuvStrmInfo->MaxChanBitMask, (UINT32)ViewZoneId, 1U) == 1U) {
        // Potential streams
        CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
        HL_GetPointerToDspPpStrmCtrlCfg(YuvStrmIdx, PpStrmBufIdx, NumView, &pPpStrmCtrl);
        (void)dsp_osal_memset(pPpStrmCtrl, 0, sizeof(vproc_pp_stream_cntl_t));

        pPpStrmCtrl->input_channel_num = (UINT8)ViewZoneId;
        pYuvStrmInfo->ChanIdxArray[NumView] = ViewZoneId;
        pPpStrmCtrl->input_stream_num = YuvStrmCfgStrId;

        pPpStrmCtrl->output_x_ofs = 0U;
        pPpStrmCtrl->output_y_ofs = 0U;
        if (PinId < DSP_VPROC_PIN_NUM) {
            pPpStrmCtrl->output_width = ViewZoneInfo->PinMaxWindow[PinId].Width;
            pPpStrmCtrl->output_height = ViewZoneInfo->PinMaxWindow[PinId].Height;
        }

        pPpStrmCtrl->output_buf_id = BufDescOutputBufId;

        // try to allocate buf in dummy channel
        if (ViewZoneId == MinViewZoneIdx) {
            pPpStrmCtrl->stream_cfg.is_alloc_out_buf = (UINT8)1U;
            DSP_SetBit(pTmpBufferUsage, pPpStrmCtrl->output_buf_id);
        } else {
            pPpStrmCtrl->stream_cfg.is_alloc_out_buf = (UINT8)0U;
        }

        pPpStrmCtrl->stream_cfg.is_last_blending_channel = (UINT8)0U;
        pPpStrmCtrl->stream_cfg.is_last_copy_channel = (UINT8)0U;
        pPpStrmCtrl->stream_cfg.num_of_post_r2y_copy_ops = (UINT8)0U;
        pPpStrmCtrl->stream_cfg.num_of_y2y_blending_ops = (UINT8)0U;
        (void)dsp_osal_virt2cli(NullAddr, &pPpStrmCtrl->post_r2y_copy_cfg_address);
        (void)dsp_osal_virt2cli(NullAddr, &pPpStrmCtrl->y2y_blending_cfg_address);

        if ((pYuvStrmInfo->MaxChanNum - 1U) == NumView) {
            pPpStrmCtrl->stream_cfg.is_last_blending_channel = (UINT8)1U;
            pPpStrmCtrl->stream_cfg.num_of_y2y_blending_ops = (UINT8)1U;
            NeedDummyBlend = 1U;
        }
        NumView++;
    }
#endif
    *pNumView = NumView;
    *pNeedDummyBlend = NeedDummyBlend;
}

static inline void HL_BldJobBufSetting(vproc_y2y_blending_cfg_t *pPpStrmBld,
                                        const DSP_EFFECT_BLEND_JOB_s *pBldJob,
                                        vproc_pp_stream_cntl_t *pPpStrmCtrl,
                                        UINT32 JobIdx, UINT16 ViewZoneIdx, const UINT16 MinViewZoneIdx,
                                        UINT32 *pTmpBufferUsage, UINT16 *pY2YBufId,
                                        UINT8 *pNeedInternalBldBuf, UINT8 *pUseInternalBldBuf)
{
    if ((pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW) ||
        (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_INT) ||
        (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_INT_DUMMY)) {
        *pNeedInternalBldBuf += (UINT8)1U;
        *pY2YBufId = pBldJob->Src0BufIdx;
    }
    if ((pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_REUSE) ||
        (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_REUSE_INT) ||
        (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_REUSE_INT_DUMMY)) {
        *pUseInternalBldBuf += (UINT8)1U;
    }
    if ((pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_INT) ||
        (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_REUSE_INT) ||
        (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_INT_DUMMY) ||
        (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_REUSE_INT_DUMMY)) {
        *pY2YBufId = pBldJob->Src0BufIdx;

        /* Update TempYuv offset */
        pPpStrmCtrl->output_x_ofs = pBldJob->Src0Win.OffsetX;
        pPpStrmCtrl->output_y_ofs = pBldJob->Src0Win.OffsetY;
    }
    pPpStrmBld[JobIdx].is_rotate_2nd_inp = (UINT8)DSP_EFCT_ROT_NONE;
    if ((pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_INT_DUMMY) ||
        (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_REUSE_INT_DUMMY)) {
        /* Update Input rotate */
        pPpStrmBld[JobIdx].is_rotate_2nd_inp = pPpStrmBld[JobIdx].is_rotate_1st_inp;
    }

    if ((*pNeedInternalBldBuf > 0U) && (ViewZoneIdx == MinViewZoneIdx)) {
        pPpStrmBld[JobIdx].is_alloc_out_buf = (UINT8)1U;
        DSP_SetBit(pTmpBufferUsage, pBldJob->DestBufIdx);
    }

}

static inline void HL_PpExistBldJob(const UINT16 YuvStrmIdx,
                                    const UINT16 ChanIdx,
                                    const UINT16 MinViewZoneIdx,
                                    const UINT16 BufDescOutputBufId,
                                    const UINT16 PpStrmBldBufIdx,
                                    const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout,
                                    vproc_pp_stream_cntl_t *pPpStrmCtrl,
                                    UINT32 *pTmpBufferUsage)
{
    UINT8 NeedInternalBldBuf = 0U, UseInternalBldBuf = 0U;
    UINT32 JobIdx;
    UINT16 Y2YBufId = DSP_EFCT_INVALID_IDX;
    UINT32 TmpBufferUsage = *pTmpBufferUsage;
    ULONG NullAddr = 0U;
    ULONG ULAddr;
    vproc_y2y_blending_cfg_t *pPpStrmBld = NULL;
    const DSP_EFFECT_BLEND_JOB_s *pBldJob;
    //const void *pVoid = NULL;

    /* Blending jobs */
    if (pYuvStrmLayout->ChanCfg[ChanIdx].BlendNum > 0U) {
        pPpStrmCtrl->stream_cfg.num_of_y2y_blending_ops = pYuvStrmLayout->ChanCfg[ChanIdx].BlendNum;
    } else {
        if (pYuvStrmLayout->ChanCfg[ChanIdx].RotateFlip != AMBA_DSP_ROTATE_0) {
            pPpStrmCtrl->stream_cfg.num_of_y2y_blending_ops = 1U;
        }
    }
    if (pPpStrmCtrl->stream_cfg.num_of_y2y_blending_ops > 0U) {

        /* Request PpStrmBld buffer */
        HL_GetPointerToDspPpStrmBldCfg(YuvStrmIdx, PpStrmBldBufIdx, ChanIdx, &pPpStrmBld);
        (void)dsp_osal_memset(pPpStrmBld, 0, sizeof(vproc_y2y_blending_cfg_t)*DSP_MAX_PP_STRM_BLEND_NUMBER_CTX);

        for (JobIdx = 0U; JobIdx<pPpStrmCtrl->stream_cfg.num_of_y2y_blending_ops; JobIdx++) {
            pBldJob = &pYuvStrmLayout->BlendJobDesc[ChanIdx][JobIdx];

            pPpStrmBld[JobIdx].is_rotate_1st_inp = HL_RotateFlipMap[pYuvStrmLayout->ChanCfg[ChanIdx].RotateFlip];

            HL_BldJobBufSetting(pPpStrmBld, pBldJob, pPpStrmCtrl, JobIdx,
                                pYuvStrmLayout->ChanCfg[ChanIdx].ViewZoneId, MinViewZoneIdx, &TmpBufferUsage,
                                &Y2YBufId, &NeedInternalBldBuf, &UseInternalBldBuf);

            pPpStrmBld[JobIdx].first_input_buf_id = pBldJob->Src0BufIdx;
            pPpStrmBld[JobIdx].first_inp_x_ofs = pBldJob->Src0Win.OffsetX;
            pPpStrmBld[JobIdx].first_inp_y_ofs = pBldJob->Src0Win.OffsetY;

            //Must from Dram
            pPpStrmBld[JobIdx].second_input_buf_id = pBldJob->Src1BufIdx;
            pPpStrmBld[JobIdx].second_inp_x_ofs = pBldJob->Src1Win.OffsetX;
            pPpStrmBld[JobIdx].second_inp_y_ofs = pBldJob->Src1Win.OffsetY;

            pPpStrmBld[JobIdx].blending_width = pBldJob->DestWin.Width;
            pPpStrmBld[JobIdx].blending_height = pBldJob->DestWin.Height;
            pPpStrmBld[JobIdx].output_x_ofs = pBldJob->DestWin.OffsetX;
            pPpStrmBld[JobIdx].output_y_ofs = pBldJob->DestWin.OffsetY;
            pPpStrmBld[JobIdx].output_buf_id = pBldJob->DestBufIdx;

            /* Using AlphaTable instead of AlphaValue */
            pPpStrmBld[JobIdx].is_alpha_stream_value = (UINT8)0U;
            pPpStrmBld[JobIdx].alpha_luma_value = (UINT8)0U;
            pPpStrmBld[JobIdx].alpha_u_value = (UINT8)0U;
            pPpStrmBld[JobIdx].alpha_v_value = (UINT8)0U;

            pPpStrmBld[JobIdx].alpha_luma_dram_pitch = pBldJob->AlphaPitch;
            pPpStrmBld[JobIdx].alpha_chroma_dram_pitch = (UINT16)(pBldJob->AlphaPitch*2U);
            (void)dsp_osal_virt2cli(pBldJob->AlphaAddr, &pPpStrmBld[JobIdx].alpha_luma_dram_addr);
            (void)dsp_osal_virt2cli(pBldJob->AlphaAddr, &pPpStrmBld[JobIdx].alpha_chroma_dram_addr);
        }

        dsp_osal_typecast(&ULAddr, &pPpStrmBld);
        //dsp_osal_typecast(&pVoid, &pPpStrmBld);
        (void)dsp_osal_virt2cli(ULAddr, &pPpStrmCtrl->y2y_blending_cfg_address);
        //(void)dsp_osal_cache_clean(pVoid, sizeof(vproc_y2y_blending_cfg_t));
    } else {
        (void)dsp_osal_virt2cli(NullAddr, &pPpStrmCtrl->y2y_blending_cfg_address);
    }

    /*
     * Assign output buffer at final stage:
     *   output_buf_id indicates the final output of each channel,
     *   so it may be an intermediate buffer or the final buffer of this effect-channel.
     *   here we take care blend job more to make sure intermediate buffer is created correctly
     */
    if (NeedInternalBldBuf > 0U) {
        pPpStrmCtrl->stream_cfg.is_alloc_out_buf = (UINT8)1U;
        pPpStrmCtrl->output_buf_id = Y2YBufId;
        DSP_SetBit(&TmpBufferUsage, Y2YBufId);
    } else if (UseInternalBldBuf > 0U) {
        pPpStrmCtrl->output_buf_id = Y2YBufId;
    } else {
#ifdef PPSTRM_SWITCH_CHANGE_INPUTS_NUM
        if (pYuvStrmLayout->ChanCfg[ChanIdx].ViewZoneId == MinActViewZoneIdx) {
#else
        if (pYuvStrmLayout->ChanCfg[ChanIdx].ViewZoneId == MinViewZoneIdx) {
#endif
            pPpStrmCtrl->stream_cfg.is_alloc_out_buf = (UINT8)1U;
            DSP_SetBit(&TmpBufferUsage, BufDescOutputBufId);
        } else {
            pPpStrmCtrl->stream_cfg.is_alloc_out_buf = (UINT8)0U;
        }
        pPpStrmCtrl->output_buf_id = BufDescOutputBufId;
    }
    *pTmpBufferUsage = TmpBufferUsage;
}

static inline void HL_PpExist(const UINT16 YuvStrmIdx,
                              const UINT16 PpStrmBufIdx,
                              const UINT16 PpStrmCpyBufIdx,
                              const UINT8 YuvStrmCfgStrId,
                              const UINT16 YuvStrmMaxChanNum,
                              const UINT16 ChanIdx,
                              const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout,
                              const UINT16 MinViewZoneIdx,
                              const UINT16 BufDescOutputBufId,
                              UINT32 *pTmpBufferUsage,
                              UINT16 *pDummyBlendOutputOfstX,
                              UINT16 *pDummyBlendOutputOfstY,
                              UINT16 *pNumView,
                              UINT8 *pNeedDummyBlend)
{
    UINT8 NeedDummyBlend = *pNeedDummyBlend;
    UINT16 JobIdx;
    UINT16 NumView = *pNumView;
    UINT32 TmpBufferUsage = *pTmpBufferUsage;
    vproc_pp_stream_cntl_t *pPpStrmCtrl = NULL;
    vproc_post_r2y_copy_cfg_t *pPpStrmCpy = NULL;
    const DSP_EFFECT_COPY_JOB_s *pCpyJob;
    CTX_YUV_STRM_INFO_s *pYuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    //const void *pVoid = NULL;
    ULONG ULAddr;

    HL_GetPointerToDspPpStrmCtrlCfg(YuvStrmIdx, PpStrmBufIdx, NumView, &pPpStrmCtrl);
    (void)dsp_osal_memset(pPpStrmCtrl, 0, sizeof(vproc_pp_stream_cntl_t));
    pPpStrmCtrl->input_channel_num = (UINT8)pYuvStrmLayout->ChanCfg[ChanIdx].ViewZoneId;
    HL_GetYuvStrmInfoPtr(YuvStrmIdx, &pYuvStrmInfo);
    pYuvStrmInfo->ChanIdxArray[NumView] = pYuvStrmLayout->ChanCfg[ChanIdx].ViewZoneId;
    pPpStrmCtrl->input_stream_num = YuvStrmCfgStrId;

    pPpStrmCtrl->output_x_ofs = pYuvStrmLayout->ChanCfg[ChanIdx].Window.OffsetX;
    pPpStrmCtrl->output_y_ofs = pYuvStrmLayout->ChanCfg[ChanIdx].Window.OffsetY;
    if (HL_GET_ROTATE(pYuvStrmLayout->ChanCfg[ChanIdx].RotateFlip) == DSP_ROTATE_90_DEGREE) {
        pPpStrmCtrl->output_width = pYuvStrmLayout->ChanCfg[ChanIdx].Window.Height;
        pPpStrmCtrl->output_height = pYuvStrmLayout->ChanCfg[ChanIdx].Window.Width;
    } else {
        pPpStrmCtrl->output_width = pYuvStrmLayout->ChanCfg[ChanIdx].Window.Width;
        pPpStrmCtrl->output_height = pYuvStrmLayout->ChanCfg[ChanIdx].Window.Height;
    }

#ifdef SUPPORT_VPROC_DISABLE_BIT
    if (j == MaxActViewZoneIdx) {
#else
    if ((YuvStrmMaxChanNum - 1U) == NumView) {
#endif
#ifdef PPSTRM_LAST_CHAN_MUST_BLEND
        pPpStrmCtrl->stream_cfg.is_last_blending_channel = (UINT8)1U;
        pPpStrmCtrl->stream_cfg.is_last_copy_channel = (UINT8)0U;
        pPpStrmCtrl->stream_cfg.is_last_passthrough_channel = (UINT8)0U;
#else
        if (ChanIdx[j] == pYuvStrmLayout->EffectBufDesc.LastBldIdx) {
            pPpStrmCtrl->stream_cfg.is_last_blending_channel = (UINT8)1U;
        } else if (ChanIdx[j] == pYuvStrmLayout->EffectBufDesc.LastCpyIdx) {
            pPpStrmCtrl->stream_cfg.is_last_copy_channel = (UINT8)1U;
        } else if (ChanIdx[j] == pYuvStrmLayout->EffectBufDesc.LastPsThIdx) {
            pPpStrmCtrl->stream_cfg.is_last_passthrough_channel = (UINT8)1U;
        } else {
            // TBD
        }
#endif

        if ((pYuvStrmLayout->ChanCfg[ChanIdx].BlendNum == 0U) &&
            (pYuvStrmLayout->ChanCfg[ChanIdx].RotateFlip == AMBA_DSP_ROTATE_0)) {
            NeedDummyBlend = 1U;
            *pDummyBlendOutputOfstX = pPpStrmCtrl->output_x_ofs;
            *pDummyBlendOutputOfstY = pPpStrmCtrl->output_y_ofs;
        }
    }

    /* Copy jobs */
    pPpStrmCtrl->stream_cfg.num_of_post_r2y_copy_ops = (UINT8)pYuvStrmLayout->CopyJobNum[ChanIdx];
    if (pPpStrmCtrl->stream_cfg.num_of_post_r2y_copy_ops > 0U) {

        /* Request PpStrmCpy buffer */
        HL_GetPointerToDspPpStrmCopyCfg(YuvStrmIdx, PpStrmCpyBufIdx, ChanIdx, &pPpStrmCpy);
        (void)dsp_osal_memset(pPpStrmCpy, 0, sizeof(vproc_post_r2y_copy_cfg_t)*DSP_MAX_PP_STRM_COPY_NUMBER);

        for (JobIdx = 0U; JobIdx<pYuvStrmLayout->CopyJobNum[ChanIdx]; JobIdx++) {
            pCpyJob = &pYuvStrmLayout->CopyJobDesc[ChanIdx][JobIdx];

            if (JobIdx == 0U) {
                pPpStrmCpy[JobIdx].is_alloc_out_buf = (UINT8)1U;
                DSP_SetBit(&TmpBufferUsage, pCpyJob->DestBufIdx);
            } else {
                pPpStrmCpy[JobIdx].is_alloc_out_buf = (UINT8)0U;
            }
            pPpStrmCpy[JobIdx].output_buf_id = pCpyJob->DestBufIdx;
            pPpStrmCpy[JobIdx].is_rotate = (UINT8)0U;

            pPpStrmCpy[JobIdx].input_start_x = (UINT32)pCpyJob->SrcWin.OffsetX;
            pPpStrmCpy[JobIdx].input_start_y = (UINT32)pCpyJob->SrcWin.OffsetY;

            pPpStrmCpy[JobIdx].output_start_x = (UINT32)pCpyJob->DstWin.OffsetX;
            pPpStrmCpy[JobIdx].output_start_y = (UINT32)pCpyJob->DstWin.OffsetY;

            pPpStrmCpy[JobIdx].copy_width = (UINT32)pCpyJob->SrcWin.Width;
            pPpStrmCpy[JobIdx].copy_height = (UINT32)pCpyJob->SrcWin.Height;
        }

        dsp_osal_typecast(&ULAddr, &pPpStrmCpy);
        //dsp_osal_typecast(&pVoid, &pPpStrmCpy);
        (void)dsp_osal_virt2cli(ULAddr, &pPpStrmCtrl->post_r2y_copy_cfg_address);
        //(void)dsp_osal_cache_clean(pVoid, sizeof(vproc_post_r2y_copy_cfg_t)*pYuvStrmLayout->CopyJobNum[ChanIdx]);
    } else {
        (void)dsp_osal_virt2cli(0U, &pPpStrmCtrl->post_r2y_copy_cfg_address);
    }

    /* Blending jobs */
    HL_PpExistBldJob(YuvStrmIdx,
                     ChanIdx,
                     MinViewZoneIdx,
                     BufDescOutputBufId,
                     PpStrmBufIdx,
                     pYuvStrmLayout,
                     pPpStrmCtrl,
                     &TmpBufferUsage);
    NumView++;
    *pNumView = NumView;
    *pNeedDummyBlend = NeedDummyBlend;
    *pTmpBufferUsage = TmpBufferUsage;
}

static inline UINT32 HL_PpDumBld(const UINT16 YuvStrmIdx,
                                 const UINT16 PpStrmBufIdx,
                                 const UINT16 PpStrmBldBufIdx,
                                 const UINT16 DummyBlendOutputOfstX,
                                 const UINT16 DummyBlendOutputOfstY,
                                 const CTX_RESOURCE_INFO_s *pResource,
                                 const UINT32 TmpBufferUsage,
                                 const UINT16 NumView,
                                 DSP_EFFECT_BUF_DESC_s *pBufDesc,
                                 UINT32 *EffectBufMask)
{
    UINT8 *pU8 = NULL;
    UINT8 IsY2yBufInUse;
    UINT16 JobIdx;
    UINT32 Rval = OK;
    vproc_pp_stream_cntl_t *pPpStrmCtrl = NULL;
    vproc_y2y_blending_cfg_t *pPpStrmBld = NULL;
    //const void *pVoid = NULL;
    ULONG ULAddr;

    /* Reuse last PpStrmCtrl buffer */
    HL_GetPointerToDspPpStrmCtrlCfg(YuvStrmIdx, PpStrmBufIdx, (UINT16)(NumView-1U), &pPpStrmCtrl);

    /* Request PpStrmBld buffer */
    HL_GetPointerToDspPpStrmBldCfg(YuvStrmIdx, PpStrmBldBufIdx, (UINT16)(NumView-1U), &pPpStrmBld);
    (void)dsp_osal_memset(pPpStrmBld, 0, sizeof(vproc_y2y_blending_cfg_t)*DSP_MAX_PP_STRM_BLEND_NUMBER_CTX);

    JobIdx = 0U;
    // check the amount of unused temp buffers
    if (pBufDesc->CpyBufId != DSP_EFCT_INVALID_IDX) {
        if (DSP_GetBit(TmpBufferUsage, (UINT32)(pBufDesc->CpyBufId), 1U) == 0U) {
            // copy temp buffer is active but unused
            pPpStrmBld[JobIdx].is_alloc_out_buf = (UINT8)1U;
            pPpStrmBld[JobIdx].first_input_buf_id = pBufDesc->OutputBufId;
            pPpStrmBld[JobIdx].first_inp_x_ofs = DummyBlendOutputOfstX;
            pPpStrmBld[JobIdx].first_inp_y_ofs = DummyBlendOutputOfstY;
            pPpStrmBld[JobIdx].second_input_buf_id = pBufDesc->OutputBufId;
            pPpStrmBld[JobIdx].second_inp_x_ofs = DummyBlendOutputOfstX;
            pPpStrmBld[JobIdx].second_inp_y_ofs = DummyBlendOutputOfstY;

            pPpStrmBld[JobIdx].blending_width = 128U;
            pPpStrmBld[JobIdx].blending_height = 32U;
            pPpStrmBld[JobIdx].output_buf_id = pBufDesc->CpyBufId;

            // Change BlendOutput offset to valid area
            pPpStrmBld[JobIdx].output_x_ofs = DummyBlendOutputOfstX;
            pPpStrmBld[JobIdx].output_y_ofs = DummyBlendOutputOfstY;

            pPpStrmBld[JobIdx].alpha_luma_dram_pitch = 128U;
            pPpStrmBld[JobIdx].alpha_chroma_dram_pitch = 256U;
            DSP_GetDspPpDummyBldTbl(&pU8);
            dsp_osal_typecast(&ULAddr, &pU8);
            (void)dsp_osal_virt2cli(ULAddr, &pPpStrmBld[JobIdx].alpha_luma_dram_addr);
            (void)dsp_osal_virt2cli(ULAddr, &pPpStrmBld[JobIdx].alpha_chroma_dram_addr);

            JobIdx++;
        }
    }
    if (pBufDesc->Y2YBufId == DSP_EFCT_INVALID_IDX) {
        IsY2yBufInUse = 0U;
    } else if (DSP_GetBit(TmpBufferUsage, pBufDesc->Y2YBufId, 1U) == 0U) {
        IsY2yBufInUse = 0U;
    } else {
        IsY2yBufInUse = 1U;
    }
    if ((IsY2yBufInUse == 0U) || (JobIdx == 0U)) {
        if (pBufDesc->Y2YBufId == DSP_EFCT_INVALID_IDX) {
            // no y2y temp buffer, generates one
            DSP_FindEmptyBit(EffectBufMask,
                             pResource->EffectLogicBufNum,
                             &pBufDesc->Y2YBufId,
                             0U/*FindOnly*/,
                             EFFECT_BUF_MASK_DEPTH);
            if (pBufDesc->Y2YBufId == DSP_EFCT_INVALID_IDX) {
                AmbaLL_LogUInt5("  No Buf for Y2YOut 0x%X%X%X%X",
                                EffectBufMask[3], EffectBufMask[2], EffectBufMask[1], EffectBufMask[0], 0U);
                Rval = DSP_ERR_0006;
            }
            pPpStrmBld[JobIdx].is_alloc_out_buf = (UINT8)1U;
        } else if (DSP_GetBit(TmpBufferUsage, pBufDesc->Y2YBufId, 1U) == 0U) {
            // y2y temp buffer is active but unused
            pPpStrmBld[JobIdx].is_alloc_out_buf = (UINT8)1U;
        } else {
            pPpStrmBld[JobIdx].is_alloc_out_buf = (UINT8)0U;
        }

        pPpStrmBld[JobIdx].first_input_buf_id = pBufDesc->OutputBufId;
        pPpStrmBld[JobIdx].first_inp_x_ofs = DummyBlendOutputOfstX;
        pPpStrmBld[JobIdx].first_inp_y_ofs = DummyBlendOutputOfstY;
        pPpStrmBld[JobIdx].second_input_buf_id = pBufDesc->OutputBufId;
        pPpStrmBld[JobIdx].second_inp_x_ofs = DummyBlendOutputOfstX;
        pPpStrmBld[JobIdx].second_inp_y_ofs = DummyBlendOutputOfstY;

        pPpStrmBld[JobIdx].blending_width = 128U;
        pPpStrmBld[JobIdx].blending_height = 32U;
        pPpStrmBld[JobIdx].output_buf_id = pBufDesc->Y2YBufId;

        // Change BlendOutput offset to valid area
        pPpStrmBld[JobIdx].output_x_ofs = DummyBlendOutputOfstX;
        pPpStrmBld[JobIdx].output_y_ofs = DummyBlendOutputOfstY;

        pPpStrmBld[JobIdx].alpha_luma_dram_pitch = 128U;
        pPpStrmBld[JobIdx].alpha_chroma_dram_pitch = 256U;
        DSP_GetDspPpDummyBldTbl(&pU8);
        dsp_osal_typecast(&ULAddr, &pU8);
        (void)dsp_osal_virt2cli(ULAddr, &pPpStrmBld[JobIdx].alpha_luma_dram_addr);
        (void)dsp_osal_virt2cli(ULAddr, &pPpStrmBld[JobIdx].alpha_chroma_dram_addr);

        JobIdx++;
    }
    pPpStrmCtrl->stream_cfg.num_of_y2y_blending_ops = (UINT8)JobIdx;
    dsp_osal_typecast(&ULAddr, &pPpStrmBld);
    //dsp_osal_typecast(&pVoid, &pPpStrmBld);
    (void)dsp_osal_virt2cli(ULAddr, &pPpStrmCtrl->y2y_blending_cfg_address);
    //(void)dsp_osal_cache_clean(pVoid, sizeof(vproc_y2y_blending_cfg_t));

    return Rval;
}

//#define DEBUG_PP_REBASE
#ifdef PPSTRM_INDPT_INT_BLD_BUFFER
static inline void HL_PpExistBldJobAvl(const UINT16 YuvStrmIdx,
                                       const UINT16 CfgIdx,
                                       const UINT16 MinViewZoneIdx,
                                       const UINT16 PpStrmBldBufIdx,
                                       const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout,
                                       vproc_pp_stream_cntl_t *pPpStrmCtrl,
                                       UINT32 *pTmpBufferUsage)
{
    UINT8 i;
    UINT8 NeedInternalBldBuf = 0U;
    UINT8 OrigBldNum = pPpStrmCtrl->stream_cfg.num_of_y2y_blending_ops;
    UINT8 TotAvlBldNum = 0U, TotArrgBldNum = 0U;
    UINT8 ExtraBldNum = 0U;
    UINT16 JobIdx;
    UINT16 Y2YBufId = DSP_EFCT_INVALID_IDX;
    UINT32 TmpBufferUsage = *pTmpBufferUsage;
    vproc_pp_stream_cntl_t *pPpStrmCtrlAvl;
    vproc_y2y_blending_cfg_t *pPpStrmBld = NULL;
    const DSP_EFFECT_BLEND_JOB_s *pBldJob;
    void *pVoid = NULL;
    ULONG ULAddr;

#ifdef DEBUG_PP_REBASE
    AmbaLL_LogUInt5("HL_PpExistBldJobAvl YuvStrmIdx:%u CfgIdx:%u ChanIdx:%u MinViewZoneIdx:%u BufDescOutputBufId:%u",
            YuvStrmIdx, CfgIdx, ChanIdx, MinViewZoneIdx, BufDescOutputBufId);
#endif
    for(i=0U; i<pYuvStrmLayout->NumChan; i++) {
        HL_GetPointerToDspPpStrmCtrlCfg(YuvStrmIdx, PpStrmBldBufIdx, i, &pPpStrmCtrlAvl);
        TotAvlBldNum += pYuvStrmLayout->ChanCfg[i].BlendNum;
        TotArrgBldNum += pPpStrmCtrlAvl->stream_cfg.num_of_y2y_blending_ops;
        if ((pYuvStrmLayout->ChanCfg[i].BlendNum == 0U) &&
            (pYuvStrmLayout->ChanCfg[i].RotateFlip != AMBA_DSP_ROTATE_0)) {
            TotAvlBldNum += 1U;
        }
#ifdef DEBUG_PP_REBASE
        AmbaLL_LogUInt5("HL_PpExistBldJobAvl i:%u [i]BlendNum:%u [i]blending_ops:%x TotAvlBldNum:%u TotArrgBldNum:%u",
                i, pYuvStrmLayout->ChanCfg[i].BlendNum,
                pPpStrmCtrlAvl->stream_cfg.num_of_y2y_blending_ops, TotAvlBldNum, TotArrgBldNum);
#endif
        if (TotAvlBldNum > TotArrgBldNum) {
            /* check all overlap chan has it's output to do bld job */
            UINT32 OverlapChan;

            pBldJob = &pYuvStrmLayout->BlendJobDesc[i][0U];
            OverlapChan = pBldJob->OverlapChan;
            //overlap chan not include itself
            DSP_SetBit(&OverlapChan, i);
            if ((~(pYuvStrmLayout->EffectBufDesc.ChannelOutputMask) & OverlapChan) == 0U) {
                ExtraBldNum = TotAvlBldNum;
                pPpStrmCtrl->stream_cfg.num_of_y2y_blending_ops += TotAvlBldNum;
#ifdef DEBUG_PP_REBASE
                AmbaLL_LogUInt5("HL_PpExistBldJobAvl 0 i:%u CfgIdx:%u [i]BlendNum:%u ChannelOutputMask:%x OverlapChan:%x",
                        i, CfgIdx, pYuvStrmLayout->ChanCfg[i].BlendNum,
                        ~(pYuvStrmLayout->EffectBufDesc.ChannelOutputMask),
                        OverlapChan);
#endif
                break;
            } else {
                //chan content not ready
#ifdef DEBUG_PP_REBASE
                AmbaLL_LogUInt5("HL_PpExistBldJobAvl 1 i:%u ChanIdx:%u [i]BlendNum:%u ChannelOutputMask:%x OverlapChan:%x",
                        i, CfgIdx, pYuvStrmLayout->ChanCfg[i].BlendNum,
                        ~(pYuvStrmLayout->EffectBufDesc.ChannelOutputMask),
                        OverlapChan);
#endif
            }
        }
    }
#ifdef DEBUG_PP_REBASE
    AmbaLL_LogUInt5("HL_PpExistBldJobAvl i:%u ExtraBldNum:%u OrigBldNum:%u pPpStrmCtrl->stream_cfg.num_of_y2y_blending_ops:%u pYuvStrmLayout->ChanCfg[ChanIdx].ViewZoneId:%u",
            i, ExtraBldNum, OrigBldNum, pPpStrmCtrl->stream_cfg.num_of_y2y_blending_ops, pYuvStrmLayout->ChanCfg[ChanIdx].ViewZoneId);
#endif
    /* Original Blending jobs */
    if (ExtraBldNum > 0U) {

        /* Request PpStrmBld buffer, Chan i using CfgIdx's vproc_y2y_blending_cfg_t  */
        HL_GetPointerToDspPpStrmBldCfg(YuvStrmIdx, PpStrmBldBufIdx, CfgIdx, &pPpStrmBld);
#ifdef DEBUG_PP_REBASE
        AmbaLL_LogUInt5("HL_GetPointerToDspPpStrmBldCfg YuvStrmIdx:%u PpStrmBldBufIdx:%u ChanIdx:%u pPpStrmBld:%x",
                YuvStrmIdx, PpStrmBldBufIdx, CfgIdx, (UINT32)pPpStrmBld, 0U);
#endif
        (void)dsp_osal_memset(pPpStrmBld, 0, sizeof(vproc_y2y_blending_cfg_t)*DSP_MAX_PP_STRM_BLEND_NUMBER);

        for (JobIdx = OrigBldNum; JobIdx<pPpStrmCtrl->stream_cfg.num_of_y2y_blending_ops; JobIdx++) {
            pBldJob = &pYuvStrmLayout->BlendJobDesc[i][JobIdx];

            pPpStrmBld[JobIdx].is_rotate_1st_inp = HL_RotateFlipMap[pYuvStrmLayout->ChanCfg[i].RotateFlip];

            if ((pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW) ||
                (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_INT) ||
                (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_INT_DUMMY)) {
                NeedInternalBldBuf += (UINT8)1U;
                Y2YBufId = pBldJob->Src0BufIdx;
            }

            if ((pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_INT) ||
                (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_REUSE_INT) ||
                (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_INT_DUMMY) ||
                (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_REUSE_INT_DUMMY)) {
                Y2YBufId = pBldJob->Src0BufIdx;

                /*
                 * Update TempYuv offset,
                 * This is other's bld job, no need to adjust pPpStrmCtrl->output_x/y_ofs
                 */
//                pPpStrmCtrl->output_x_ofs = pBldJob->Src0Win.OffsetX;
//                pPpStrmCtrl->output_y_ofs = pBldJob->Src0Win.OffsetY;
#ifdef DEBUG_PP_REBASE
                AmbaLL_LogUInt5("HL_PpExistBldJobAvl Update TempYuv offset i:%u JobIdx:%u NeedY2YBuf:%u Src0Win.OffsetX/Y: %u/%u",
                        i, JobIdx, pBldJob->NeedY2YBuf,
                        pBldJob->Src0Win.OffsetX,
                        pBldJob->Src0Win.OffsetY);
#endif
            }
            pPpStrmBld[JobIdx].is_rotate_2nd_inp = (UINT8)DSP_EFCT_ROT_NONE;
            if ((pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_INT_DUMMY) ||
                (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_REUSE_INT_DUMMY)) {
                /* Update Input rotate */
                pPpStrmBld[JobIdx].is_rotate_2nd_inp = pPpStrmBld[JobIdx].is_rotate_1st_inp;
            }

            if ((NeedInternalBldBuf > 0U) &&
                (pYuvStrmLayout->ChanCfg[i].ViewZoneId == MinViewZoneIdx)) {
                pPpStrmBld[JobIdx].is_alloc_out_buf = (UINT8)1U;
                DSP_SetBit(&TmpBufferUsage, pBldJob->DestBufIdx);
            }
#ifdef DEBUG_PP_REBASE
            AmbaLL_LogUInt5("HL_PpExistBldJobAvl JobIdx:%u NeedY2YBuf:%u pBldJob->Src0BufIdx:%u pBldJob->DestBufIdx:%u TmpBufferUsage:%x",
                    JobIdx, pBldJob->NeedY2YBuf, pBldJob->Src0BufIdx, pBldJob->DestBufIdx, TmpBufferUsage);
            AmbaLL_LogUInt5("HL_PpExistBldJobAvl JobIdx:%u NeedInternalBldBuf:%u UseInternalBldBuf:%u Y2YBufId:%u",
                    JobIdx, NeedInternalBldBuf, UseInternalBldBuf, Y2YBufId, 0);
#endif
            pPpStrmBld[JobIdx].first_input_buf_id = pBldJob->Src0BufIdx;
            pPpStrmBld[JobIdx].first_inp_x_ofs = pBldJob->Src0Win.OffsetX;
            pPpStrmBld[JobIdx].first_inp_y_ofs = pBldJob->Src0Win.OffsetY;

            //Must from Dram
            pPpStrmBld[JobIdx].second_input_buf_id = pBldJob->Src1BufIdx;
            pPpStrmBld[JobIdx].second_inp_x_ofs = pBldJob->Src1Win.OffsetX;
            pPpStrmBld[JobIdx].second_inp_y_ofs = pBldJob->Src1Win.OffsetY;

            pPpStrmBld[JobIdx].blending_width = pBldJob->DestWin.Width;
            pPpStrmBld[JobIdx].blending_height = pBldJob->DestWin.Height;
            pPpStrmBld[JobIdx].output_x_ofs = pBldJob->DestWin.OffsetX;
            pPpStrmBld[JobIdx].output_y_ofs = pBldJob->DestWin.OffsetY;
            pPpStrmBld[JobIdx].output_buf_id = pBldJob->DestBufIdx;

            /* Using AlphaTable instead of AlphaValue */
            pPpStrmBld[JobIdx].is_alpha_stream_value = (UINT8)0U;
            pPpStrmBld[JobIdx].alpha_luma_value = (UINT8)0U;
            pPpStrmBld[JobIdx].alpha_u_value = (UINT8)0U;
            pPpStrmBld[JobIdx].alpha_v_value = (UINT8)0U;

            pPpStrmBld[JobIdx].alpha_luma_dram_pitch = pBldJob->AlphaPitch;
            pPpStrmBld[JobIdx].alpha_chroma_dram_pitch = (UINT16)(pBldJob->AlphaPitch*2U);
            (void)dsp_osal_virt2cli(pBldJob->AlphaAddr, &pPpStrmBld[JobIdx].alpha_luma_dram_addr);
            (void)dsp_osal_virt2cli(pBldJob->AlphaAddr, &pPpStrmBld[JobIdx].alpha_chroma_dram_addr);
        }

        dsp_osal_typecast(&ULAddr, &pPpStrmBld);
        dsp_osal_typecast(&pVoid, &pPpStrmBld);
        (void)dsp_osal_virt2cli(ULAddr, &pPpStrmCtrl->y2y_blending_cfg_address);
        (void)dsp_osal_cache_clean(pVoid, sizeof(vproc_y2y_blending_cfg_t));
    }

    /*
     * This is for extra bld,
     * should not change pPpStrmCtrl is_alloc_out_buf, or output_buf_id
     */
    if (NeedInternalBldBuf > 0U) {
        DSP_SetBit(&TmpBufferUsage, Y2YBufId);
    }
    *pTmpBufferUsage = TmpBufferUsage;
}

static inline void HL_PpExistBldJobToutCalcNum(const UINT16 ChanId,
                                               const UINT16 JobIdx,
                                               const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout,
                                               vproc_pp_stream_cntl_t *pPpStrmCtrl,
                                               UINT8 *pNeedInternalBldBuf,
                                               UINT8 *pUseInternalBldBuf,
                                               UINT16 *pY2YBufId)
{
    UINT8 BlendNum, RotateFlip;
    UINT8 NeedInternalBldBuf = *pNeedInternalBldBuf;
    UINT8 UseInternalBldBuf = *pUseInternalBldBuf;
    UINT16 Y2YBufId = *pY2YBufId;
    UINT32 OverlapChan;
    const DSP_EFFECT_BLEND_JOB_s *pBldJob = &pYuvStrmLayout->BlendJobDesc[ChanId][JobIdx];

    BlendNum = pYuvStrmLayout->ChanCfg[ChanId].BlendNum;
    RotateFlip = pYuvStrmLayout->ChanCfg[ChanId].RotateFlip;
    OverlapChan = pBldJob->OverlapChan;
    //OverlapChan not include itself
    DSP_SetBit(&OverlapChan, ChanId);

    /* check all overlap chan has it's output to do bld job */
    if ((~(pYuvStrmLayout->EffectBufDesc.ChannelOutputMask) & OverlapChan) == 0U) {

#ifdef DEBUG_PP_REBASE
        AmbaLL_LogUInt5("HL_PpExistBldJobToutBldNum 0 [i]BlendNum:%u ChannelOutputMask:%x OverlapChan:%x",
                BlendNum,
                ~(pYuvStrmLayout->EffectBufDesc.ChannelOutputMask),
                OverlapChan, 0U, 0U);
#endif
        if (BlendNum > 0U) {
            pPpStrmCtrl->stream_cfg.num_of_y2y_blending_ops = BlendNum;
        } else if (RotateFlip != AMBA_DSP_ROTATE_0) {
            pPpStrmCtrl->stream_cfg.num_of_y2y_blending_ops = 1U;
        } else {
            //do nothing
        }
    } else {
        if ((BlendNum > 0U) || (RotateFlip != AMBA_DSP_ROTATE_0)) {
            /*
             * chan content not ready, still need to output to y2ybuffer,
             * but not to do bld job, leave to other chan
             */
            if ((pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW) ||
                (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_INT) ||
                (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_INT_DUMMY)) {
                NeedInternalBldBuf += (UINT8)1U;
                Y2YBufId = pBldJob->Src0BufIdx;
            }
            if ((pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_REUSE) ||
                (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_REUSE_INT) ||
                (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_REUSE_INT_DUMMY)) {
                UseInternalBldBuf += (UINT8)1U;
            }
            if ((pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_INT) ||
                (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_REUSE_INT) ||
                (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_INT_DUMMY) ||
                (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_REUSE_INT_DUMMY)) {
                Y2YBufId = pBldJob->Src0BufIdx;

                /* Update TempYuv offset */
                pPpStrmCtrl->output_x_ofs = pBldJob->Src0Win.OffsetX;
                pPpStrmCtrl->output_y_ofs = pBldJob->Src0Win.OffsetY;
#ifdef DEBUG_PP_REBASE
                AmbaLL_LogUInt5("HL_PpExistBldJobToutBldNum not ready, Update TempYuv offset ChanId:%u JobIdx:%u NeedY2YBuf:%u Src0Win.OffsetX/Y: %u/%u",
                        ChanId, JobIdx, pBldJob->NeedY2YBuf,
                        pBldJob->Src0Win.OffsetX,
                        pBldJob->Src0Win.OffsetY);
#endif
            }
        } else {
            //do nothing
        }
#ifdef DEBUG_PP_REBASE
        AmbaLL_LogUInt5("HL_PpExistBldJobToutBldNum not ready, JobIdx:%u NeedY2YBuf:%u pBldJob->Src0BufIdx:%u pBldJob->DestBufIdx:%u",
                JobIdx, pBldJob->NeedY2YBuf, pBldJob->Src0BufIdx, pBldJob->DestBufIdx, 0U);
        AmbaLL_LogUInt5("HL_PpExistBldJobToutBldNum not ready, JobIdx:%u NeedInternalBldBuf:%u UseInternalBldBuf:%u Y2YBufId:%u",
                JobIdx, NeedInternalBldBuf, UseInternalBldBuf, Y2YBufId, 0);
        AmbaLL_LogUInt5("HL_PpExistBldJobToutBldNum not ready, [i]BlendNum:%u ChannelOutputMask:%x OverlapChan:%x",
                BlendNum,
                ~(pYuvStrmLayout->EffectBufDesc.ChannelOutputMask),
                OverlapChan, 0U, 0U);
#endif
    }

    *pNeedInternalBldBuf = NeedInternalBldBuf;
    *pUseInternalBldBuf = UseInternalBldBuf;
    *pY2YBufId = Y2YBufId;
}

static inline void HL_PpExistBldJobTout(const UINT16 YuvStrmIdx,
                                        const UINT16 CfgIdx,
                                        const UINT16 ChanId,
                                        const UINT16 BufDescOutputBufId,
                                        const UINT16 PpStrmBldBufIdx,
                                        const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout,
                                        vproc_pp_stream_cntl_t *pPpStrmCtrl,
                                        UINT32 *pTmpBufferUsage)
{
    UINT8 NeedInternalBldBuf = 0U, UseInternalBldBuf = 0U;
    UINT16 JobIdx = 0U;
    UINT16 Y2YBufId = DSP_EFCT_INVALID_IDX;
    UINT32 TmpBufferUsage = *pTmpBufferUsage;
    ULONG NullAddr = 0U;
    vproc_y2y_blending_cfg_t *pPpStrmBld = NULL;
    const DSP_EFFECT_BLEND_JOB_s *pBldJob;
    void *pVoid;
    ULONG ULAddr;

#ifdef DEBUG_PP_REBASE
    AmbaLL_LogUInt5("HL_PpExistBldJobTout YuvStrmIdx:%u CfgIdx:%u ChanId:%u BufDescOutputBufId:%u",
            YuvStrmIdx, CfgIdx, ChanId, BufDescOutputBufId, 0U);
#endif
    /* check all overlap chan has it's output to do bld job */
    HL_PpExistBldJobToutCalcNum(ChanId,
                                JobIdx,
                                pYuvStrmLayout,
                                pPpStrmCtrl,
                                &NeedInternalBldBuf,
                                &UseInternalBldBuf,
                                &Y2YBufId);

    /* Original Blending jobs */
    if (pPpStrmCtrl->stream_cfg.num_of_y2y_blending_ops > 0U) {

        /* Request PpStrmBld buffer */
        HL_GetPointerToDspPpStrmBldCfg(YuvStrmIdx, PpStrmBldBufIdx, CfgIdx, &pPpStrmBld);
#ifdef DEBUG_PP_REBASE
        AmbaLL_LogUInt5("HL_GetPointerToDspPpStrmBldCfg YuvStrmIdx:%u PpStrmBldBufIdx:%u CfgIdx:%u pPpStrmBld:%x",
                YuvStrmIdx, PpStrmBldBufIdx, CfgIdx, (UINT32)pPpStrmBld, 0U);
#endif
        (void)dsp_osal_memset(pPpStrmBld, 0, sizeof(vproc_y2y_blending_cfg_t)*DSP_MAX_PP_STRM_BLEND_NUMBER);

        for (JobIdx = 0U; JobIdx<pPpStrmCtrl->stream_cfg.num_of_y2y_blending_ops; JobIdx++) {
            pBldJob = &pYuvStrmLayout->BlendJobDesc[ChanId][JobIdx];

            pPpStrmBld[JobIdx].is_rotate_1st_inp = HL_RotateFlipMap[pYuvStrmLayout->ChanCfg[ChanId].RotateFlip];

            if ((pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW) ||
                (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_INT) ||
                (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_INT_DUMMY)) {
                NeedInternalBldBuf += (UINT8)1U;
                Y2YBufId = pBldJob->Src0BufIdx;
            }
            if ((pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_REUSE) ||
                (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_REUSE_INT) ||
                (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_REUSE_INT_DUMMY)) {
                UseInternalBldBuf += (UINT8)1U;
            }
            if ((pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_INT) ||
                (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_REUSE_INT) ||
                (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_INT_DUMMY) ||
                (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_REUSE_INT_DUMMY)) {
                Y2YBufId = pBldJob->Src0BufIdx;

                /* Update TempYuv offset */
                pPpStrmCtrl->output_x_ofs = pBldJob->Src0Win.OffsetX;
                pPpStrmCtrl->output_y_ofs = pBldJob->Src0Win.OffsetY;
#ifdef DEBUG_PP_REBASE
                AmbaLL_LogUInt5("HL_PpExistBldJobTout Update TempYuv offset ChanId:%u JobIdx:%u NeedY2YBuf:%u Src0Win.OffsetX/Y: %u/%u",
                        ChanId, JobIdx, pBldJob->NeedY2YBuf,
                        pBldJob->Src0Win.OffsetX,
                        pBldJob->Src0Win.OffsetY);
#endif
            }
            pPpStrmBld[JobIdx].is_rotate_2nd_inp = (UINT8)DSP_EFCT_ROT_NONE;
            if ((pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_INT_DUMMY) ||
                (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_REUSE_INT_DUMMY)) {
                /* Update Input rotate */
                pPpStrmBld[JobIdx].is_rotate_2nd_inp = pPpStrmBld[JobIdx].is_rotate_1st_inp;
            }

            if ((NeedInternalBldBuf > 0U) && (CfgIdx == 0U)) {
                pPpStrmBld[JobIdx].is_alloc_out_buf = (UINT8)1U;
                DSP_SetBit(&TmpBufferUsage, pBldJob->DestBufIdx);
            }
#ifdef DEBUG_PP_REBASE
            AmbaLL_LogUInt5("HL_PpExistBldJobTout JobIdx:%u NeedY2YBuf:%u pBldJob->Src0BufIdx:%u pBldJob->DestBufIdx:%u TmpBufferUsage:%x",
                    JobIdx, pBldJob->NeedY2YBuf, pBldJob->Src0BufIdx, pBldJob->DestBufIdx, TmpBufferUsage);
            AmbaLL_LogUInt5("HL_PpExistBldJobTout JobIdx:%u NeedInternalBldBuf:%u UseInternalBldBuf:%u Y2YBufId:%u",
                    JobIdx, NeedInternalBldBuf, UseInternalBldBuf, Y2YBufId, 0);
#endif
            pPpStrmBld[JobIdx].first_input_buf_id = pBldJob->Src0BufIdx;
            pPpStrmBld[JobIdx].first_inp_x_ofs = pBldJob->Src0Win.OffsetX;
            pPpStrmBld[JobIdx].first_inp_y_ofs = pBldJob->Src0Win.OffsetY;

            //Must from Dram
            pPpStrmBld[JobIdx].second_input_buf_id = pBldJob->Src1BufIdx;
            pPpStrmBld[JobIdx].second_inp_x_ofs = pBldJob->Src1Win.OffsetX;
            pPpStrmBld[JobIdx].second_inp_y_ofs = pBldJob->Src1Win.OffsetY;

            pPpStrmBld[JobIdx].blending_width = pBldJob->DestWin.Width;
            pPpStrmBld[JobIdx].blending_height = pBldJob->DestWin.Height;
            pPpStrmBld[JobIdx].output_x_ofs = pBldJob->DestWin.OffsetX;
            pPpStrmBld[JobIdx].output_y_ofs = pBldJob->DestWin.OffsetY;
            pPpStrmBld[JobIdx].output_buf_id = pBldJob->DestBufIdx;

            /* Using AlphaTable instead of AlphaValue */
            pPpStrmBld[JobIdx].is_alpha_stream_value = (UINT8)0U;
            pPpStrmBld[JobIdx].alpha_luma_value = (UINT8)0U;
            pPpStrmBld[JobIdx].alpha_u_value = (UINT8)0U;
            pPpStrmBld[JobIdx].alpha_v_value = (UINT8)0U;

            pPpStrmBld[JobIdx].alpha_luma_dram_pitch = pBldJob->AlphaPitch;
            pPpStrmBld[JobIdx].alpha_chroma_dram_pitch = (UINT16)(pBldJob->AlphaPitch*2U);
            (void)dsp_osal_virt2cli(pBldJob->AlphaAddr, &pPpStrmBld[JobIdx].alpha_luma_dram_addr);
            (void)dsp_osal_virt2cli(pBldJob->AlphaAddr, &pPpStrmBld[JobIdx].alpha_chroma_dram_addr);
        }

        dsp_osal_typecast(&ULAddr, &pPpStrmBld);
        dsp_osal_typecast(&pVoid, &pPpStrmBld);
        (void)dsp_osal_virt2cli(ULAddr, &pPpStrmCtrl->y2y_blending_cfg_address);
        (void)dsp_osal_cache_clean(pVoid, sizeof(vproc_y2y_blending_cfg_t));
    } else {
        (void)dsp_osal_virt2cli(NullAddr, &pPpStrmCtrl->y2y_blending_cfg_address);
    }

    /*
     * Assign output buffer at final stage:
     *   output_buf_id indicates the final output of each channel,
     *   so it may be an intermediate buffer or the final buffer of this effect-channel.
     *   here we take care blend job more to make sure intermediate buffer is created correctly
     */
    if (NeedInternalBldBuf > 0U) {
        pPpStrmCtrl->stream_cfg.is_alloc_out_buf = (UINT8)1U;
        pPpStrmCtrl->output_buf_id = Y2YBufId;
        DSP_SetBit(&TmpBufferUsage, Y2YBufId);
    } else if (UseInternalBldBuf > 0U) {
        pPpStrmCtrl->output_buf_id = Y2YBufId;
    } else {
#ifdef PPSTRM_SWITCH_CHANGE_INPUTS_NUM
        if (pYuvStrmLayout->ChanCfg[ChanId].ViewZoneId == MinActViewZoneIdx) {
#else
        /* if this is first chan,
         * or this is original first chan
         *  */
        if (CfgIdx == 0U) {
#endif
            pPpStrmCtrl->stream_cfg.is_alloc_out_buf = (UINT8)1U;
            DSP_SetBit(&TmpBufferUsage, BufDescOutputBufId);
        } else if (/*(CfgIdx != 0U) &&*/ (ChanId == 0U)) {
            UINT16 CfgIdxAvl = 0U;
            vproc_pp_stream_cntl_t *pPpStrmCtrlAvl = NULL;
            vproc_y2y_blending_cfg_t *pPpStrmBldAvl = NULL;

            /*
             * need to check is BufDescOutputBufId has been output to decide is_alloc_out_buf,
             * if you are ChanId=0, CfgIdx=0 may has been output something to BufDescOutputBufId,
             * And is_alloc_out_buf has been set
             */
            HL_GetPointerToDspPpStrmCtrlCfg(YuvStrmIdx, PpStrmBldBufIdx, CfgIdxAvl, &pPpStrmCtrlAvl);
            HL_GetPointerToDspPpStrmBldCfg(YuvStrmIdx, PpStrmBldBufIdx, CfgIdxAvl, &pPpStrmBldAvl);

#ifdef DEBUG_PP_REBASE
            AmbaLL_LogUInt5("HL_PpExistBldJobTout CfgIdxAvl:%u blending_ops:%u output_buf_id:%u is_alloc_out_buf:%u",
                    CfgIdxAvl,
                    pPpStrmCtrlAvl->stream_cfg.num_of_y2y_blending_ops,
                    pPpStrmBldAvl->output_buf_id,
                    pPpStrmBldAvl->is_alloc_out_buf, 0);
#endif
            if ((pPpStrmCtrlAvl->stream_cfg.num_of_y2y_blending_ops > 0U) &&
                (pPpStrmBldAvl->output_buf_id == BufDescOutputBufId) &&
                (pPpStrmBldAvl->is_alloc_out_buf == 1U)) {

                pPpStrmCtrl->stream_cfg.is_alloc_out_buf = (UINT8)0U;

            } else {
                pPpStrmCtrl->stream_cfg.is_alloc_out_buf = (UINT8)1U;
                DSP_SetBit(&TmpBufferUsage, BufDescOutputBufId);
            }
        } else {
            pPpStrmCtrl->stream_cfg.is_alloc_out_buf = (UINT8)0U;
        }
        pPpStrmCtrl->output_buf_id = BufDescOutputBufId;
#ifdef DEBUG_PP_REBASE
        AmbaLL_LogUInt5("HL_PpExistBldJobTout is_alloc_out_buf:%u output_buf_id:%u",
                pPpStrmCtrl->stream_cfg.is_alloc_out_buf,
                pPpStrmCtrl->output_buf_id,
                0, 0, 0);
#endif
    }
    *pTmpBufferUsage = TmpBufferUsage;
}

static inline void HL_PpExistTout(const UINT16 YuvStrmIdx,
                                  const UINT16 PpStrmBufIdx,
                                  const UINT16 PpStrmCpyBufIdx,
                                  const UINT8 YuvStrmCfgStrId,
                                  const UINT16 YuvStrmMaxChanNum,
                                  const UINT16 CfgIdx,
                                  const UINT16 ChanId,
                                  const UINT16 MinViewZoneIdx,
                                  const UINT16 BufDescOutputBufId,
                                  const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout,
                                  UINT32 *pTmpBufferUsage,
                                  UINT16 *pDummyBlendOutputOfstX,
                                  UINT16 *pDummyBlendOutputOfstY,
                                  UINT16 *pNumView,
                                  UINT8 *pNeedDummyBlend)
{
    UINT8 NeedDummyBlend = *pNeedDummyBlend;
    UINT16 JobIdx;
    UINT16 NumView = *pNumView;
    UINT32 TmpBufferUsage = *pTmpBufferUsage;
    ULONG NullAddr = 0U;
    vproc_pp_stream_cntl_t *pPpStrmCtrl = NULL;
    vproc_post_r2y_copy_cfg_t *pPpStrmCpy = NULL;
    const DSP_EFFECT_COPY_JOB_s *pCpyJob;
    void *pVoid;
    ULONG ULAddr;

    HL_GetPointerToDspPpStrmCtrlCfg(YuvStrmIdx, PpStrmBufIdx, NumView, &pPpStrmCtrl);
    (void)dsp_osal_memset(pPpStrmCtrl, 0, sizeof(vproc_pp_stream_cntl_t));
    pPpStrmCtrl->input_channel_num = (UINT8)pYuvStrmLayout->ChanCfg[ChanId].ViewZoneId;
    pPpStrmCtrl->input_stream_num = YuvStrmCfgStrId;
#ifdef DEBUG_PP_REBASE
    AmbaLL_LogUInt5("HL_PpExistTout CfgIdx:%u ChanId:%u OffsetX:%u OffsetY:%u",
            CfgIdx, ChanId,
            pYuvStrmLayout->ChanCfg[ChanId].Window.OffsetX,
            pYuvStrmLayout->ChanCfg[ChanId].Window.OffsetY, 0);
#endif
    pPpStrmCtrl->output_x_ofs = pYuvStrmLayout->ChanCfg[ChanId].Window.OffsetX;
    pPpStrmCtrl->output_y_ofs = pYuvStrmLayout->ChanCfg[ChanId].Window.OffsetY;
    if (HL_GET_ROTATE(pYuvStrmLayout->ChanCfg[ChanId].RotateFlip) == DSP_ROTATE_90_DEGREE) {
        pPpStrmCtrl->output_width = pYuvStrmLayout->ChanCfg[ChanId].Window.Height;
        pPpStrmCtrl->output_height = pYuvStrmLayout->ChanCfg[ChanId].Window.Width;
    } else {
        pPpStrmCtrl->output_width = pYuvStrmLayout->ChanCfg[ChanId].Window.Width;
        pPpStrmCtrl->output_height = pYuvStrmLayout->ChanCfg[ChanId].Window.Height;
    }

    /* Copy jobs, don't do it if someone timeout */
    pPpStrmCtrl->stream_cfg.num_of_post_r2y_copy_ops = (UINT8)pYuvStrmLayout->CopyJobNum[ChanId];
    if (pPpStrmCtrl->stream_cfg.num_of_post_r2y_copy_ops > 0U) {

        /* Request PpStrmCpy buffer */
        HL_GetPointerToDspPpStrmCopyCfg(YuvStrmIdx, PpStrmCpyBufIdx, NumView, &pPpStrmCpy);
        (void)dsp_osal_memset(pPpStrmCpy, 0, sizeof(vproc_post_r2y_copy_cfg_t)*DSP_MAX_PP_STRM_COPY_NUMBER);

        for (JobIdx = 0U; JobIdx<pYuvStrmLayout->CopyJobNum[ChanId]; JobIdx++) {
            pCpyJob = &pYuvStrmLayout->CopyJobDesc[ChanId][JobIdx];

            if (JobIdx == 0U) {
                pPpStrmCpy[JobIdx].is_alloc_out_buf = (UINT8)1U;
                DSP_SetBit(&TmpBufferUsage, pCpyJob->DestBufIdx);
            } else {
                pPpStrmCpy[JobIdx].is_alloc_out_buf = (UINT8)0U;
            }
            pPpStrmCpy[JobIdx].output_buf_id = pCpyJob->DestBufIdx;
            pPpStrmCpy[JobIdx].is_rotate = (UINT8)0U;

            pPpStrmCpy[JobIdx].input_start_x = (UINT32)pCpyJob->SrcWin.OffsetX;
            pPpStrmCpy[JobIdx].input_start_y = (UINT32)pCpyJob->SrcWin.OffsetY;

            pPpStrmCpy[JobIdx].output_start_x = (UINT32)pCpyJob->DstWin.OffsetX;
            pPpStrmCpy[JobIdx].output_start_y = (UINT32)pCpyJob->DstWin.OffsetY;

            pPpStrmCpy[JobIdx].copy_width = (UINT32)pCpyJob->SrcWin.Width;
            pPpStrmCpy[JobIdx].copy_height = (UINT32)pCpyJob->SrcWin.Height;
        }

        dsp_osal_typecast(&ULAddr, &pPpStrmCpy);
        dsp_osal_typecast(&pVoid, &pPpStrmCpy);
        (void)dsp_osal_virt2cli(ULAddr, &pPpStrmCtrl->post_r2y_copy_cfg_address);
        (void)dsp_osal_cache_clean(pVoid,
                                   sizeof(vproc_post_r2y_copy_cfg_t)*pYuvStrmLayout->CopyJobNum[ChanId]);
    } else {
        (void)dsp_osal_virt2cli(NullAddr, &pPpStrmCtrl->post_r2y_copy_cfg_address);
    }

    /* Blending jobs */
    HL_PpExistBldJobTout(YuvStrmIdx,
                         CfgIdx,
                         ChanId,
                         BufDescOutputBufId,
                         PpStrmBufIdx,
                         pYuvStrmLayout,
                         pPpStrmCtrl,
                         &TmpBufferUsage);

    /* try to do other available bld job */
    HL_PpExistBldJobAvl(YuvStrmIdx,
                        NumView,
                        MinViewZoneIdx,
                        PpStrmBufIdx,
                        pYuvStrmLayout,
                        pPpStrmCtrl,
                        &TmpBufferUsage);

#ifdef SUPPORT_VPROC_DISABLE_BIT
    if (j == MaxActViewZoneIdx) {
#else
    if ((YuvStrmMaxChanNum - 1U) == NumView) {
#endif
#ifdef PPSTRM_LAST_CHAN_MUST_BLEND
        pPpStrmCtrl->stream_cfg.is_last_blending_channel = (UINT8)1U;
        pPpStrmCtrl->stream_cfg.is_last_copy_channel = (UINT8)0U;
        pPpStrmCtrl->stream_cfg.is_last_passthrough_channel = (UINT8)0U;
#else
        if (VZtoChanIdx[j] == pYuvStrmLayout->EffectBufDesc.LastBldIdx) {
            pPpStrmCtrl->stream_cfg.is_last_blending_channel = (UINT8)1U;
        } else if (VZtoChanIdx[j] == pYuvStrmLayout->EffectBufDesc.LastCpyIdx) {
            pPpStrmCtrl->stream_cfg.is_last_copy_channel = (UINT8)1U;
        } else if (VZtoChanIdx[j] == pYuvStrmLayout->EffectBufDesc.LastPsThIdx) {
            pPpStrmCtrl->stream_cfg.is_last_passthrough_channel = (UINT8)1U;
        } else {
            // TBD
        }
#endif

        if ((pPpStrmCtrl->stream_cfg.num_of_y2y_blending_ops == 0U) &&
            (pYuvStrmLayout->ChanCfg[ChanId].RotateFlip == AMBA_DSP_ROTATE_0)) {
            NeedDummyBlend = 1U;
            *pDummyBlendOutputOfstX = pPpStrmCtrl->output_x_ofs;
            *pDummyBlendOutputOfstY = pPpStrmCtrl->output_y_ofs;
#ifdef DEBUG_PP_REBASE
            AmbaLL_LogUInt5("HL_PpExistTout ChanId:%u BlendNum:%u NeedDummyBlend = 1",
                    ChanId, pYuvStrmLayout->ChanCfg[ChanId].BlendNum, 0, 0, 0U);
#endif
        }
    }

    NumView++;
    *pNumView = NumView;
    *pNeedDummyBlend = NeedDummyBlend;
    *pTmpBufferUsage = TmpBufferUsage;
}

UINT32 HL_FillPpStrmCfgOrder(UINT16 YuvStrmIdx,
                             const UINT16 *pYuvIdxOrder,
                             cmd_vproc_multi_stream_pp_t *PpStrmCfg,
                             CTX_YUV_STRM_LAYOUT_s *pExtStrmLayout)
{
#ifdef PPSTRM_SWITCH_BETWEEN_PASSTHROUGH_AND_BLENDING
    UINT16 MinActViewZoneIdx, MaxActViewZoneIdx;
    UINT16 TotalPostBldNum, OriginalTotalBldNum;
#endif
#if defined(UCODE_SUPPORT_EFFECT_PREV_BC_REARRANGE) || defined(UCODE_SUPPORT_EFFECT_PREV_ABC_REARRANGE)
    UINT8 ExitILoop = 0U;
    UINT16 PinId = 0U;
    UINT16 PrevDest;
    CTX_VPROC_INFO_s VprocInfo = {0};
#else
    UINT8 IsDramMipiYuv;
#endif
    UINT8 NeedDummyBlend = 0U;
    UINT16 i, MinViewZoneIdx, ChanIdx, ChanId;
    UINT16 DummyBlendOutputOfstX = 0U, DummyBlendOutputOfstY = 0U;
    UINT16 VZtoChanIdx[AMBA_DSP_MAX_VIEWZONE_NUM], NumView = 0U;
    UINT32 Rval = OK;
    UINT32 EffectBufMask[EFFECT_BUF_MASK_DEPTH];
    UINT32 YuvStrmViewOI = 0U;
    UINT32 PpStrmBufIdx;
    UINT32 TmpBufferUsage = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    DSP_EFFECT_BUF_DESC_s *pBufDesc;
    CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout;
    vproc_pp_stream_cntl_t *pPpStrmCtrl = NULL;
    void *pVoid;
    ULONG ULAddr;

    HL_GetYuvStrmInfoLock(YuvStrmIdx, &YuvStrmInfo);
    if (pExtStrmLayout == NULL) {
        pYuvStrmLayout = &YuvStrmInfo->Layout;
    } else {
        pYuvStrmLayout = pExtStrmLayout;
    }

#ifdef PPSTRM_SWITCH_BETWEEN_PASSTHROUGH_AND_BLENDING
    (void)HL_GetEffectChannelPostBlendNum(YuvStrmIdx, pYuvStrmLayout, &TotalPostBldNum);
    (void)HL_GetEffectChannelPostBlendNum(YuvStrmIdx, &YuvStrmInfo->Layout, &OriginalTotalBldNum);
#endif

    /* Find the end-points among real channels */
    (void)dsp_osal_memset(VZtoChanIdx, 0, sizeof(UINT16)*AMBA_DSP_MAX_VIEWZONE_NUM);
    for (ChanIdx=0U; ChanIdx<pYuvStrmLayout->NumChan; ChanIdx++) {
        DSP_SetBit(&YuvStrmViewOI, (UINT32)pYuvStrmLayout->ChanCfg[ChanIdx].ViewZoneId);
        VZtoChanIdx[pYuvStrmLayout->ChanCfg[ChanIdx].ViewZoneId] = ChanIdx;
    }
#ifdef PPSTRM_SWITCH_BETWEEN_PASSTHROUGH_AND_BLENDING
    DSP_Bit2U16Idx(YuvStrmViewOI, &MinActViewZoneIdx);
    DSP_ReverseBit2U16Idx(YuvStrmViewOI, &MaxActViewZoneIdx);
#endif
    /* Including potential channels */
    DSP_Bit2U16Idx(YuvStrmInfo->MaxChanBitMask, &MinViewZoneIdx);

    PpStrmCfg->chan_id = (UINT8)MinViewZoneIdx;
    PpStrmCfg->effect_grp_id = (UINT8)0U;

#if defined(UCODE_SUPPORT_EFFECT_PREV_BC_REARRANGE) || defined(UCODE_SUPPORT_EFFECT_PREV_ABC_REARRANGE)
    HL_GetVprocInfo(HL_MTX_OPT_ALL, MinViewZoneIdx, &VprocInfo);
    for (i=0U; (i<DSP_VPROC_PIN_NUM) &&(ExitILoop == 0U); i++) {
        if (VprocInfo.PinUsage[i] > 0U) {
            if (1U == DSP_GetBit(VprocInfo.PinUsage[i], YuvStrmIdx, 1U)) {
                ExitILoop = 1U;
                PinId = i;
            }
        }
    }

    if (PinId == DSP_VPROC_PIN_NUM) {
        AmbaLL_LogUInt5("StrmPp[%d] route fail", YuvStrmIdx, 0U, 0U, 0U, 0U);
        PpStrmCfg->output_dst = AMBA_DSP_PREV_DEST_PIN;
        PpStrmCfg->str_id = 0U;
    } else {
        (void)HL_GetViewZonePrevDest(MinViewZoneIdx, PinId, &PrevDest);
        PpStrmCfg->output_dst = PrevDest;
        PpStrmCfg->str_id = HL_CtxVprocPinDspPinMap[PinId];
    }
#else
    IsDramMipiYuv = (UINT8)HL_GetDramMipiYuvEnable();
    (void)HL_GetEffectChannelVprocPin(YuvStrmInfo->Purpose,
                                      YuvStrmInfo->DestVout,
                                      YuvStrmInfo->DestEnc,
                                      &PinId,
                                      IsDramMipiYuv);
    if (PinId == DSP_VPROC_PIN_NUM) {
        AmbaLL_LogUInt5("StrmPp[%d] route fail", YuvStrmIdx, 0U, 0U, 0U, 0U);
        PpStrmCfg->output_dst = AMBA_DSP_PREV_DEST_PIN;
        PpStrmCfg->str_id = 0U;
    } else {
        if ((PinId == DSP_VPROC_PIN_MAIN) ||
            (PinId == DSP_VPROC_PIN_PREVA)) {
            PpStrmCfg->output_dst = AMBA_DSP_PREV_DEST_PIN;
        } else if (PinId == DSP_VPROC_PIN_PREVC) {
            PpStrmCfg->output_dst = AMBA_DSP_PREV_DEST_VOUT0;
        } else { // DSP_VPROC_PIN_PREVB
            PpStrmCfg->output_dst = AMBA_DSP_PREV_DEST_VOUT1;
        }
        PpStrmCfg->str_id = HL_CtxVprocPinDspPinMap[PinId];
    }
#endif

#ifdef PPSTRM_SWITCH_CHANGE_INPUTS_NUM
    PpStrmCfg->num_of_inputs = (UINT8)pYuvStrmLayout->NumChan;
    YuvStrmInfo->Layout.CurrentMasterViewId = pYuvStrmLayout->ChanCfg[0].ViewZoneId;
#else
    PpStrmCfg->num_of_inputs = (UINT8)YuvStrmInfo->MaxChanNum;
    YuvStrmInfo->Layout.CurrentMasterViewId = YuvStrmInfo->Layout.ChanCfg[0].ViewZoneId;
#endif

    /* Sweep all overlapped area for Copy/Blend Jobs on each Chan */
    (void)HL_CalcEffectPostPJob(YuvStrmIdx, pYuvStrmLayout);
    pBufDesc = &pYuvStrmLayout->EffectBufDesc;
    PpStrmCfg->final_output_buf_id = pBufDesc->OutputBufId;

    HL_GetResourcePtr(&Resource);
    (void)dsp_osal_memcpy(&EffectBufMask[0U],
                          &Resource->EffectLogicBufMask[0U],
                          sizeof(UINT32)*EFFECT_BUF_MASK_DEPTH);

    /* Request PpStrmCtrl buffer */
    (void)DSP_ReqBuf(&YuvStrmInfo->PpStrmPoolDesc, 1U, &PpStrmBufIdx, 0U/*FullnessCheck*/);
    YuvStrmInfo->PpStrmPoolDesc.Wp = (UINT16)PpStrmBufIdx;
    HL_GetPointerToDspPpStrmCtrlCfg(YuvStrmIdx, (UINT16)PpStrmBufIdx, 0U, &pPpStrmCtrl);
    dsp_osal_typecast(&ULAddr, &pPpStrmCtrl);
    (void)dsp_osal_virt2cli(ULAddr, &PpStrmCfg->input_pp_cfg_addr_array);

    for (ChanIdx=0U; (ChanIdx<AMBA_DSP_MAX_YUVSTRM_VIEW_NUM) && (NumView < YuvStrmInfo->MaxChanNum); ChanIdx++) {
        //use re-ordered ChanId
        ChanId = pYuvIdxOrder[ChanIdx];

        //set output
        DSP_SetBit(&pBufDesc->ChannelOutputMask, ChanId);

        //fill pPpStrmCtrl depend on existence
        if (DSP_GetBit(YuvStrmViewOI, (UINT32)ChanId, 1U) == 0U) {
            HL_PpNoneExist(YuvStrmIdx,
                           (UINT8)PpStrmCfg->str_id,
                           ChanId,
                           MinViewZoneIdx,
                           pBufDesc->OutputBufId,
                           (UINT16)PpStrmBufIdx,
                           PinId,
                           &YuvStrmInfo,
                           &NumView,
                           &NeedDummyBlend);
            AmbaLL_LogUInt5("HL_FillPpStrmCfgOrder Idx:%u HL_PpExist 1 YuvStrmViewOI:%x ChanId:%u ChannelOutputMask:%x",
                    ChanIdx,
                    YuvStrmViewOI,
                    ChanId,
                    pBufDesc->ChannelOutputMask, 0U);
        } else if (DSP_GetBit(YuvStrmViewOI, (UINT32)ChanId, 1U) == 1U) {
            HL_PpExistTout(YuvStrmIdx,
                           (UINT16)PpStrmBufIdx,
                           (UINT16)PpStrmBufIdx,
                           (UINT8)PpStrmCfg->str_id,
                           YuvStrmInfo->MaxChanNum,
                           ChanIdx,
                           ChanId,
                           MinViewZoneIdx,
                           pBufDesc->OutputBufId,
                           pYuvStrmLayout,
                           &TmpBufferUsage,
                           &DummyBlendOutputOfstX,
                           &DummyBlendOutputOfstY,
                           &NumView,
                           &NeedDummyBlend);
        } else {
            // inactive channels
        }
    }

    // dummy blending
    if (NeedDummyBlend == 1U) {
        Rval = HL_PpDumBld(YuvStrmIdx,
                           (UINT16)PpStrmBufIdx,
                           (UINT16)PpStrmBufIdx,
                           DummyBlendOutputOfstX,
                           DummyBlendOutputOfstY,
                           Resource,
                           TmpBufferUsage,
                           NumView,
                           pBufDesc,
                           EffectBufMask);
    }
    //CleanCache
    HL_GetPointerToDspPpStrmCtrlCfg(YuvStrmIdx, (UINT16)PpStrmBufIdx, 0U, &pPpStrmCtrl);
    dsp_osal_typecast(&pVoid, &pPpStrmCtrl);
    (void)dsp_osal_cache_clean(pVoid, sizeof(vproc_pp_stream_cntl_t)*pYuvStrmLayout->NumChan);
    HL_GetYuvStrmInfoUnLock(YuvStrmIdx);

    HL_GetResourceLock(&Resource);
    (void)dsp_osal_memcpy(&Resource->EffectLogicBufMask[0U],
                          &EffectBufMask[0U],
                          sizeof(UINT32)*EFFECT_BUF_MASK_DEPTH);
    HL_GetResourceUnLock();

    return Rval;
}
#else

/*
 * if (idx0 NeedY2YBuf = BLD_JOB_Y2Y_BUF_NEW_OUT)
 *      output to new y2y buffer
 *      not to do bldnum
 *      do bld at CfgIdx = ChanId
 *      do rot/flip at CfgIdx = ChanId
 *
 * if CfgIdx = ChanId,
 *      check need to do cfgidx0 bld
 */
static inline void HL_CalcBldJobsOnReBaseBld(const UINT16 CfgIdx,
                                             const UINT16 ChanId,
                                             const DSP_EFFECT_BLEND_JOB_s *pBaseBldJob,
                                             const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout,
                                             UINT32 *pBldOps)
{
    if ((CfgIdx == 0U) &&
        ((pBaseBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_OUT) ||
         (pBaseBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_OUT_INT) ||
         (pBaseBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_OUT_INT_DUMMY))) {
        *pBldOps = 0U;
    } else {
        if (pYuvStrmLayout->ChanCfg[ChanId].BlendNum > 0U) {
            *pBldOps = pYuvStrmLayout->ChanCfg[ChanId].BlendNum;
        } else {
            if (pYuvStrmLayout->ChanCfg[ChanId].RotateFlip != AMBA_DSP_ROTATE_0) {
                *pBldOps = 1U;
            }
        }
    }
}

static inline void HL_FillOrgBldJobsOnReBaseBld(const UINT16 YuvStrmIdx,
                                                const UINT16 PpStrmBldBufIdx,
                                                const UINT16 CfgIdx,
                                                const UINT32 BldOps,
                                                const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout,
                                                const UINT16 ChanId,
                                                UINT8 *pNeedInternalBldBuf,
                                                UINT8 *pUseInternalBldBuf,
                                                UINT16 *pY2YBufId,
                                                vproc_pp_stream_cntl_t *pPpStrmCtrl,
                                                UINT32 *pTmpBufferUsage)
{
    vproc_y2y_blending_cfg_t *pPpStrmBld = NULL;
    UINT32 JobIdx;
    const DSP_EFFECT_BLEND_JOB_s *pBldJob;
    UINT32 TmpBufferUsage = *pTmpBufferUsage;
    //const void *pVoid;
    ULONG ULAddr;

    /* Request PpStrmBld buffer */
    HL_GetPointerToDspPpStrmBldCfg(YuvStrmIdx, PpStrmBldBufIdx, CfgIdx, &pPpStrmBld);
#ifdef DEBUG_PP_REBASE
    AmbaLL_LogUInt5("HL_GetPointerToDspPpStrmBldCfg YuvStrmIdx:%u PpStrmBldBufIdx:%u CfgIdx:%u pPpStrmBld:%x",
            YuvStrmIdx, PpStrmBldBufIdx, CfgIdx, (UINT32)pPpStrmBld, 0U);
#endif

    (void)dsp_osal_memset(pPpStrmBld, 0, sizeof(vproc_y2y_blending_cfg_t)*DSP_MAX_PP_STRM_BLEND_NUMBER_CTX);

    for (JobIdx = 0U; JobIdx<BldOps; JobIdx++) {
        pBldJob = &pYuvStrmLayout->BlendJobDesc[ChanId][JobIdx];

        pPpStrmBld[JobIdx].is_rotate_1st_inp = HL_RotateFlipMap[pYuvStrmLayout->ChanCfg[ChanId].RotateFlip];

        if ((pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW) ||
            (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_INT) ||
            (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_INT_DUMMY)) {
            *pNeedInternalBldBuf += (UINT8)1U;
            *pY2YBufId = pBldJob->Src0BufIdx;
        }
        if ((pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_REUSE) ||
            (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_REUSE_INT) ||
            (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_REUSE_INT_DUMMY)) {
            *pUseInternalBldBuf += (UINT8)1U;
        }
        if ((pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_INT) ||
            (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_REUSE_INT) ||
            (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_INT_DUMMY) ||
            (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_REUSE_INT_DUMMY)) {
            *pY2YBufId = pBldJob->Src0BufIdx;

            /* Update TempYuv offset */
            pPpStrmCtrl->output_x_ofs = pBldJob->Src0Win.OffsetX;
            pPpStrmCtrl->output_y_ofs = pBldJob->Src0Win.OffsetY;
#ifdef DEBUG_PP_REBASE
            AmbaLL_LogUInt5("HL_PpExistCfgReBaseBld Update TempYuv offset ChanId:%u JobIdx:%u NeedY2YBuf:%u Src0Win.OffsetX/Y: %u/%u",
                    ChanId, JobIdx, pBldJob->NeedY2YBuf,
                    pBldJob->Src0Win.OffsetX,
                    pBldJob->Src0Win.OffsetY);
#endif
        }
        pPpStrmBld[JobIdx].is_rotate_2nd_inp = (UINT8)DSP_EFCT_ROT_NONE;
        if ((pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_INT_DUMMY) ||
            (pBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_REUSE_INT_DUMMY)) {
            /* Update Input rotate */
            pPpStrmBld[JobIdx].is_rotate_2nd_inp = pPpStrmBld[JobIdx].is_rotate_1st_inp;
        }

        if ((*pNeedInternalBldBuf > 0U) && (CfgIdx == 0U)) {
            pPpStrmBld[JobIdx].is_alloc_out_buf = (UINT8)1U;
            DSP_SetBit(&TmpBufferUsage, pBldJob->DestBufIdx);
        }
#ifdef DEBUG_PP_REBASE
        AmbaLL_LogUInt5("HL_PpExistBldJobTout JobIdx:%u NeedY2YBuf:%u pBldJob->Src0BufIdx:%u pBldJob->DestBufIdx:%u TmpBufferUsage:%x",
                JobIdx, pBldJob->NeedY2YBuf, pBldJob->Src0BufIdx, pBldJob->DestBufIdx, TmpBufferUsage);
        AmbaLL_LogUInt5("HL_PpExistBldJobTout JobIdx:%u NeedInternalBldBuf:%u UseInternalBldBuf:%u Y2YBufId:%u",
                JobIdx, *pNeedInternalBldBuf, *pUseInternalBldBuf, *pY2YBufId, 0);
#endif
        pPpStrmBld[JobIdx].first_input_buf_id = pBldJob->Src0BufIdx;
        pPpStrmBld[JobIdx].first_inp_x_ofs = pBldJob->Src0Win.OffsetX;
        pPpStrmBld[JobIdx].first_inp_y_ofs = pBldJob->Src0Win.OffsetY;

        //Must from Dram
        pPpStrmBld[JobIdx].second_input_buf_id = pBldJob->Src1BufIdx;
        pPpStrmBld[JobIdx].second_inp_x_ofs = pBldJob->Src1Win.OffsetX;
        pPpStrmBld[JobIdx].second_inp_y_ofs = pBldJob->Src1Win.OffsetY;

        pPpStrmBld[JobIdx].blending_width = pBldJob->DestWin.Width;
        pPpStrmBld[JobIdx].blending_height = pBldJob->DestWin.Height;
        pPpStrmBld[JobIdx].output_x_ofs = pBldJob->DestWin.OffsetX;
        pPpStrmBld[JobIdx].output_y_ofs = pBldJob->DestWin.OffsetY;
        pPpStrmBld[JobIdx].output_buf_id = pBldJob->DestBufIdx;

        /* Using AlphaTable instead of AlphaValue */
        pPpStrmBld[JobIdx].is_alpha_stream_value = (UINT8)0U;
        pPpStrmBld[JobIdx].alpha_luma_value = (UINT8)0U;
        pPpStrmBld[JobIdx].alpha_u_value = (UINT8)0U;
        pPpStrmBld[JobIdx].alpha_v_value = (UINT8)0U;

        pPpStrmBld[JobIdx].alpha_luma_dram_pitch = pBldJob->AlphaPitch;
        pPpStrmBld[JobIdx].alpha_chroma_dram_pitch = (UINT16)(pBldJob->AlphaPitch*2U);
        (void)dsp_osal_virt2cli(pBldJob->AlphaAddr, &pPpStrmBld[JobIdx].alpha_luma_dram_addr);
        (void)dsp_osal_virt2cli(pBldJob->AlphaAddr, &pPpStrmBld[JobIdx].alpha_chroma_dram_addr);
    }

    dsp_osal_typecast(&ULAddr, &pPpStrmBld);
    //dsp_osal_typecast(&pVoid, &pPpStrmBld);
    (void)dsp_osal_virt2cli(ULAddr, &pPpStrmCtrl->y2y_blending_cfg_address);
    //(void)dsp_osal_cache_clean(pVoid, sizeof(vproc_y2y_blending_cfg_t));

    *pTmpBufferUsage = TmpBufferUsage;
}

static inline void HL_FillNewBldJobsOnReBaseBld(const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout,
                                                const UINT16 BaseChanId,
                                                const DSP_EFFECT_BLEND_JOB_s *pBaseBldJob,
                                                const UINT16 YuvStrmIdx,
                                                const UINT16 PpStrmBldBufIdx,
                                                const UINT16 CfgIdx,
                                                UINT32 *pBldOps,
                                                vproc_pp_stream_cntl_t *pPpStrmCtrl,
                                                UINT32 *pTmpBufferUsage)
{
    UINT32 BaseChanIdBldNum = pYuvStrmLayout->ChanCfg[BaseChanId].BlendNum;
    vproc_y2y_blending_cfg_t *pPpStrmBld = NULL;
    UINT32 JobIdx;
    UINT32 TmpBufferUsage = *pTmpBufferUsage;
    //const void *pVoid;
    ULONG ULAddr;

    if (pYuvStrmLayout->ChanCfg[BaseChanId].RotateFlip != AMBA_DSP_ROTATE_0) {
        BaseChanIdBldNum += 1U;
    }
#ifdef DEBUG_PP_REBASE
    AmbaLL_LogUInt5("HL_PpExistCfgReBaseBld CfgIdx == BaseChan BaseChanIdBldNum:%u NeedY2YBuf:%u pBaseBldJob->Src0BufIdx:%u pBaseBldJob->DestBufIdx:%u TmpBufferUsage:%x",
            BaseChanIdBldNum, pBaseBldJob->NeedY2YBuf, pBaseBldJob->Src0BufIdx, pBaseBldJob->DestBufIdx, TmpBufferUsage);
#endif

    /* Request PpStrmBld buffer */
    HL_GetPointerToDspPpStrmBldCfg(YuvStrmIdx, PpStrmBldBufIdx, CfgIdx, &pPpStrmBld);
    if (*pBldOps == 0U) {
        (void)dsp_osal_memset(pPpStrmBld, 0, sizeof(vproc_y2y_blending_cfg_t)*DSP_MAX_PP_STRM_BLEND_NUMBER_CTX);
    }

    for (JobIdx = *pBldOps; JobIdx<BaseChanIdBldNum; JobIdx++) {
        pPpStrmBld[JobIdx].is_rotate_1st_inp = HL_RotateFlipMap[pYuvStrmLayout->ChanCfg[BaseChanId].RotateFlip];
        pPpStrmBld[JobIdx].is_rotate_2nd_inp = (UINT8)DSP_EFCT_ROT_NONE;
        if ((pBaseBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_INT_DUMMY) ||
            (pBaseBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_REUSE_INT_DUMMY) ||
            (pBaseBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_OUT_INT_DUMMY)) {
            /* Update Input rotate */
            pPpStrmBld[JobIdx].is_rotate_2nd_inp = pPpStrmBld[JobIdx].is_rotate_1st_inp;
        }

#ifdef DEBUG_PP_REBASE
        AmbaLL_LogUInt5("HL_PpExistCfgReBaseBld pBaseBldJob JobIdx:%u NeedY2YBuf:%u pBaseBldJob->Src0BufIdx:%u pBaseBldJob->DestBufIdx:%u TmpBufferUsage:%x",
                JobIdx, pBaseBldJob->NeedY2YBuf, pBaseBldJob->Src0BufIdx, pBaseBldJob->DestBufIdx, TmpBufferUsage);
        AmbaLL_LogUInt5("HL_PpExistCfgReBaseBld JobIdx:%u NeedInternalBldBuf:%u ",
                JobIdx, 0U, 0U, 0U, 0U);
#endif
        pPpStrmBld[JobIdx].first_input_buf_id = pBaseBldJob->Src0BufIdx;
        pPpStrmBld[JobIdx].first_inp_x_ofs = pBaseBldJob->Src0Win.OffsetX;
        pPpStrmBld[JobIdx].first_inp_y_ofs = pBaseBldJob->Src0Win.OffsetY;

        //Must from Dram
        pPpStrmBld[JobIdx].second_input_buf_id = pBaseBldJob->Src1BufIdx;
        pPpStrmBld[JobIdx].second_inp_x_ofs = pBaseBldJob->Src1Win.OffsetX;
        pPpStrmBld[JobIdx].second_inp_y_ofs = pBaseBldJob->Src1Win.OffsetY;

        pPpStrmBld[JobIdx].blending_width = pBaseBldJob->DestWin.Width;
        pPpStrmBld[JobIdx].blending_height = pBaseBldJob->DestWin.Height;
        pPpStrmBld[JobIdx].output_x_ofs = pBaseBldJob->DestWin.OffsetX;
        pPpStrmBld[JobIdx].output_y_ofs = pBaseBldJob->DestWin.OffsetY;
        pPpStrmBld[JobIdx].output_buf_id = pBaseBldJob->DestBufIdx;

        /* Using AlphaTable instead of AlphaValue */
        pPpStrmBld[JobIdx].is_alpha_stream_value = (UINT8)0U;
        pPpStrmBld[JobIdx].alpha_luma_value = (UINT8)0U;
        pPpStrmBld[JobIdx].alpha_u_value = (UINT8)0U;
        pPpStrmBld[JobIdx].alpha_v_value = (UINT8)0U;

        pPpStrmBld[JobIdx].alpha_luma_dram_pitch = pBaseBldJob->AlphaPitch;
        pPpStrmBld[JobIdx].alpha_chroma_dram_pitch = (UINT16)(pBaseBldJob->AlphaPitch*2U);
        (void)dsp_osal_virt2cli(pBaseBldJob->AlphaAddr, &pPpStrmBld[JobIdx].alpha_luma_dram_addr);
        (void)dsp_osal_virt2cli(pBaseBldJob->AlphaAddr, &pPpStrmBld[JobIdx].alpha_chroma_dram_addr);
    }
    *pBldOps += BaseChanIdBldNum;
    dsp_osal_typecast(&ULAddr, &pPpStrmBld);
    //dsp_osal_typecast(&pVoid, &pPpStrmBld);
    (void)dsp_osal_virt2cli(ULAddr, &pPpStrmCtrl->y2y_blending_cfg_address);
    //(void)dsp_osal_cache_clean(pVoid, sizeof(vproc_y2y_blending_cfg_t));

    *pTmpBufferUsage = TmpBufferUsage;
}

static inline void HL_PpExistCfgReBaseBld(const UINT16 YuvStrmIdx,
                                          const UINT16 BaseChanId,
                                          const UINT16 CfgIdx,
                                          const UINT16 ChanId,
                                          const UINT16 BufDescOutputBufId,
                                          const UINT16 PpStrmBldBufIdx,
                                          const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout,
                                          vproc_pp_stream_cntl_t *pPpStrmCtrl,
                                          UINT32 *pTmpBufferUsage)
{
    UINT8 NeedInternalBldBuf = 0U, UseInternalBldBuf = 0U;
    UINT16 Y2YBufId = DSP_EFCT_INVALID_IDX;
    UINT32 BldOps = 0U;
    UINT32 TmpBufferUsage = *pTmpBufferUsage;
    const DSP_EFFECT_BLEND_JOB_s *pBaseBldJob = &pYuvStrmLayout->BlendJobDesc[BaseChanId][0U];
    ULONG NullAddr = 0U;

#ifdef DEBUG_PP_REBASE
    AmbaLL_LogUInt5("HL_PpExistCfgReBaseBld YuvStrmIdx:%u CfgIdx:%u ChanId:%u BaseChanId:%u BufDescOutputBufId:%u",
            YuvStrmIdx, CfgIdx, ChanId, BaseChanId, BufDescOutputBufId);
    AmbaLL_LogUInt5("HL_PpExistCfgReBaseBld pBaseBldJob Src0BufIdx:%u Src1BufIdx:%u DestBufIdx:%u ReBaseOutBufIdx:%u NeedY2YBuf:%u",
            pBaseBldJob->Src0BufIdx, pBaseBldJob->Src1BufIdx,
            pBaseBldJob->DestBufIdx, pBaseBldJob->ReBaseOutBufIdx,
            pBaseBldJob->NeedY2YBuf);
#endif

    /* Blending jobs */
    HL_CalcBldJobsOnReBaseBld(CfgIdx,
                              ChanId,
                              pBaseBldJob,
                              pYuvStrmLayout,
                              &BldOps);

#ifdef DEBUG_PP_REBASE
    AmbaLL_LogUInt5("HL_PpExistCfgReBaseBld 0 BldOps:%u BaseChanId.BlendNum:%u",
            BldOps, pYuvStrmLayout->ChanCfg[BaseChanId].BlendNum, 0U, 0U, 0U);
#endif
    /* Original Blending jobs */
    if (BldOps > 0U) {
        HL_FillOrgBldJobsOnReBaseBld(YuvStrmIdx,
                                     PpStrmBldBufIdx,
                                     CfgIdx,
                                     BldOps,
                                     pYuvStrmLayout,
                                     ChanId,
                                     &NeedInternalBldBuf,
                                     &UseInternalBldBuf,
                                     &Y2YBufId,
                                     pPpStrmCtrl,
                                     &TmpBufferUsage);
    } else {
        (void)dsp_osal_virt2cli(NullAddr, &pPpStrmCtrl->y2y_blending_cfg_address);
    }

#ifdef DEBUG_PP_REBASE
    AmbaLL_LogUInt5("HL_PpExistCfgReBaseBld 0 BldOps:%u ", BldOps, 0U, 0U, 0U, 0U);
#endif
    if ((CfgIdx == BaseChanId) &&
        ((pBaseBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_OUT) ||
         (pBaseBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_OUT_INT) ||
         (pBaseBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_OUT_INT_DUMMY))) {
        HL_FillNewBldJobsOnReBaseBld(pYuvStrmLayout,
                                     BaseChanId,
                                     pBaseBldJob,
                                     YuvStrmIdx,
                                     PpStrmBldBufIdx,
                                     CfgIdx,
                                     &BldOps,
                                     pPpStrmCtrl,
                                     &TmpBufferUsage);
    }

    pPpStrmCtrl->stream_cfg.num_of_y2y_blending_ops = (UINT8)BldOps;
#ifdef DEBUG_PP_REBASE
    AmbaLL_LogUInt5("HL_PpExistCfgReBaseBld 1 blending_ops:%u NeedInternalBldBuf:%u UseInternalBldBuf:%u",
            pPpStrmCtrl->stream_cfg.num_of_y2y_blending_ops, NeedInternalBldBuf, UseInternalBldBuf, 0U, 0U);
#endif

    /*
     * Assign output buffer at final stage:
     *   output_buf_id indicates the final output of each channel,
     *   so it may be an intermediate buffer or the final buffer of this effect-channel.
     *   here we take care blend job more to make sure intermediate buffer is created correctly
     */
    if (NeedInternalBldBuf > 0U) {
        pPpStrmCtrl->stream_cfg.is_alloc_out_buf = (UINT8)1U;
        pPpStrmCtrl->output_buf_id = Y2YBufId;
        DSP_SetBit(&TmpBufferUsage, Y2YBufId);
    } else if (UseInternalBldBuf > 0U) {
        pPpStrmCtrl->output_buf_id = Y2YBufId;
    } else {
        /* if this is first chan,
         * or this is original first chan
         *  */
        if (CfgIdx == 0U) {
            if ((pBaseBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_OUT) ||
                (pBaseBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_OUT_INT) ||
                (pBaseBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_OUT_INT_DUMMY)) {
                pPpStrmCtrl->output_buf_id = pBaseBldJob->ReBaseOutBufIdx;
                DSP_SetBit(&TmpBufferUsage, pBaseBldJob->ReBaseOutBufIdx);

                /* Update TempYuv offset */
                pPpStrmCtrl->output_x_ofs = pBaseBldJob->Src0Win.OffsetX;
                pPpStrmCtrl->output_y_ofs = pBaseBldJob->Src0Win.OffsetY;
#ifdef DEBUG_PP_REBASE
                AmbaLL_LogUInt5("HL_PpExistCfgReBaseBld Update (CfgIdx == 0U) TempYuv offset ChanId:%u NeedY2YBuf:%u Src0Win.OffsetX/Y: %u/%u",
                        ChanId, pBaseBldJob->NeedY2YBuf,
                        pBaseBldJob->Src0Win.OffsetX,
                        pBaseBldJob->Src0Win.OffsetY, 0U);
#endif
            } else {
                pPpStrmCtrl->output_buf_id = BufDescOutputBufId;
                DSP_SetBit(&TmpBufferUsage, BufDescOutputBufId);
            }
            pPpStrmCtrl->stream_cfg.is_alloc_out_buf = (UINT8)1U;
        } else if (ChanId == 0U) {
            if ((pBaseBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_OUT) ||
                (pBaseBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_OUT_INT) ||
                (pBaseBldJob->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_OUT_INT_DUMMY)) {
                pPpStrmCtrl->stream_cfg.is_alloc_out_buf = (UINT8)1U;
            } else {
                pPpStrmCtrl->stream_cfg.is_alloc_out_buf = (UINT8)0U;
            }
            pPpStrmCtrl->output_buf_id = BufDescOutputBufId;
        } else {
            pPpStrmCtrl->stream_cfg.is_alloc_out_buf = (UINT8)0U;
            pPpStrmCtrl->output_buf_id = BufDescOutputBufId;
        }
#ifdef DEBUG_PP_REBASE
        AmbaLL_LogUInt5("HL_PpExistCfgReBaseBld is_alloc:%u output_buf_id:%u",
                pPpStrmCtrl->stream_cfg.is_alloc_out_buf,
                pPpStrmCtrl->output_buf_id,
                0U, 0U, 0U);
#endif
    }
    *pTmpBufferUsage = TmpBufferUsage;
}

static inline void HL_PpExistCfgReBase(const UINT16 YuvStrmIdx,
                                       const UINT16 PpStrmBufIdx,
                                       const UINT8 YuvStrmCfgStrId,
                                       const UINT16 YuvStrmMaxChanNum,
                                       const UINT16 BaseChanId,
                                       const UINT16 CfgIdx,
                                       const UINT16 ChanId,
                                       const UINT16 BufDescOutputBufId,
                                       const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout,
                                       UINT32 *pTmpBufferUsage,
                                       UINT16 *pDummyBlendOutputOfstX,
                                       UINT16 *pDummyBlendOutputOfstY,
                                       UINT16 *pNumView,
                                       UINT8 *pNeedDummyBlend)
{
    UINT8 NeedDummyBlend = *pNeedDummyBlend;
    UINT16 NumView = *pNumView;
    UINT32 TmpBufferUsage = *pTmpBufferUsage;
    vproc_pp_stream_cntl_t *pPpStrmCtrl = NULL;
    ULONG NullAddr = 0U;

    HL_GetPointerToDspPpStrmCtrlCfg(YuvStrmIdx, PpStrmBufIdx, NumView, &pPpStrmCtrl);
    (void)dsp_osal_memset(pPpStrmCtrl, 0, sizeof(vproc_pp_stream_cntl_t));
    pPpStrmCtrl->input_channel_num = (UINT8)pYuvStrmLayout->ChanCfg[ChanId].ViewZoneId;
    pPpStrmCtrl->input_stream_num = YuvStrmCfgStrId;
#ifdef DEBUG_PP_REBASE
    AmbaLL_LogUInt5("HL_PpExistReBase 0 YuvStrmIdx:%u PpStrmBufIdx:%u NumView:%u ViewZoneId:%u",
            YuvStrmIdx, PpStrmBufIdx,
            NumView,
            pYuvStrmLayout->ChanCfg[ChanId].ViewZoneId, 0U);
    AmbaLL_LogUInt5("HL_PpExistReBase 0 CfgIdx:%u ChanId:%u OffsetX:%u OffsetY:%u",
            CfgIdx, ChanId,
            pYuvStrmLayout->ChanCfg[ChanId].Window.OffsetX,
            pYuvStrmLayout->ChanCfg[ChanId].Window.OffsetY, 0U);
#endif
    pPpStrmCtrl->output_x_ofs = pYuvStrmLayout->ChanCfg[ChanId].Window.OffsetX;
    pPpStrmCtrl->output_y_ofs = pYuvStrmLayout->ChanCfg[ChanId].Window.OffsetY;
    if (HL_GET_ROTATE(pYuvStrmLayout->ChanCfg[ChanId].RotateFlip) == DSP_ROTATE_90_DEGREE) {
        pPpStrmCtrl->output_width = pYuvStrmLayout->ChanCfg[ChanId].Window.Height;
        pPpStrmCtrl->output_height = pYuvStrmLayout->ChanCfg[ChanId].Window.Width;
    } else {
        pPpStrmCtrl->output_width = pYuvStrmLayout->ChanCfg[ChanId].Window.Width;
        pPpStrmCtrl->output_height = pYuvStrmLayout->ChanCfg[ChanId].Window.Height;
    }

    //FIXME Original Copy jobs, don't do it if someone timeout
    (void)dsp_osal_virt2cli(NullAddr, &pPpStrmCtrl->post_r2y_copy_cfg_address);

    /* Blending jobs */
    /*
     * if (idx0 NeedY2YBuf = BLD_JOB_Y2Y_BUF_NEW_OUT)
     *      output to new y2y buffer
     *      not to do bldnum
     *      do bld at CfgIdx = ChanId
     *      do rot/flip at CfgIdx = ChanId
     *
     * if CfgIdx = ChanId,
     *      check need to do cfgidx0 bld
     */
    HL_PpExistCfgReBaseBld(YuvStrmIdx,
                           BaseChanId,
                           CfgIdx,
                           ChanId,
                           BufDescOutputBufId,
                           PpStrmBufIdx,
                           pYuvStrmLayout,
                           pPpStrmCtrl,
                           &TmpBufferUsage);
#ifdef DEBUG_PP_REBASE
    AmbaLL_LogUInt5("HL_PpExistReBase 1 CfgIdx:%u ChanId:%u OffsetX:%u OffsetY:%u NeedDummyBlend:%u",
            CfgIdx, ChanId,
            pYuvStrmLayout->ChanCfg[ChanId].Window.OffsetX,
            pYuvStrmLayout->ChanCfg[ChanId].Window.OffsetY, NeedDummyBlend);
#endif

#ifdef SUPPORT_VPROC_DISABLE_BIT
    if (j == MaxActViewZoneIdx) {
#else
    if ((YuvStrmMaxChanNum - 1U) == NumView) {
#endif
#ifdef PPSTRM_LAST_CHAN_MUST_BLEND
        pPpStrmCtrl->stream_cfg.is_last_blending_channel = (UINT8)1U;
        pPpStrmCtrl->stream_cfg.is_last_copy_channel = (UINT8)0U;
        pPpStrmCtrl->stream_cfg.is_last_passthrough_channel = (UINT8)0U;
#else
        if (VZtoChanIdx[j] == pYuvStrmLayout->EffectBufDesc.LastBldIdx) {
            pPpStrmCtrl->stream_cfg.is_last_blending_channel = (UINT8)1U;
        } else if (VZtoChanIdx[j] == pYuvStrmLayout->EffectBufDesc.LastCpyIdx) {
            pPpStrmCtrl->stream_cfg.is_last_copy_channel = (UINT8)1U;
        } else if (VZtoChanIdx[j] == pYuvStrmLayout->EffectBufDesc.LastPsThIdx) {
            pPpStrmCtrl->stream_cfg.is_last_passthrough_channel = (UINT8)1U;
        } else {
            // TBD
        }
#endif

        if ((pPpStrmCtrl->stream_cfg.num_of_y2y_blending_ops == 0U) &&
            (pYuvStrmLayout->ChanCfg[ChanId].RotateFlip == AMBA_DSP_ROTATE_0)) {
            NeedDummyBlend = 1U;
            *pDummyBlendOutputOfstX = pPpStrmCtrl->output_x_ofs;
            *pDummyBlendOutputOfstY = pPpStrmCtrl->output_y_ofs;
#ifdef DEBUG_PP_REBASE
            AmbaLL_LogUInt5("HL_PpExistTout ChanId:%u BlendNum:%u NeedDummyBlend = 1",
                    ChanId, pYuvStrmLayout->ChanCfg[ChanId].BlendNum, 0, 0, 0U);
#endif
        }
    }

    NumView++;
    *pNumView = NumView;
    *pNeedDummyBlend = NeedDummyBlend;
    *pTmpBufferUsage = TmpBufferUsage;
}
#endif

UINT32 HL_FillPpStrmCfg(UINT16 YuvStrmIdx,
                        cmd_vproc_multi_stream_pp_t *PpStrmCfg,
                        CTX_YUV_STRM_LAYOUT_s *pExtStrmLayout)
{
#ifdef PPSTRM_SWITCH_BETWEEN_PASSTHROUGH_AND_BLENDING
    UINT16 MinActViewZoneIdx, MaxActViewZoneIdx;
    UINT16 TotalPostBldNum, OriginalTotalBldNum;
#endif
#if defined(UCODE_SUPPORT_EFFECT_PREV_BC_REARRANGE) || defined(UCODE_SUPPORT_EFFECT_PREV_ABC_REARRANGE)
    UINT16 PinId = 0U;
    UINT16 PrevDest;
    CTX_VPROC_INFO_s VprocInfo = {0};
#else
    UINT8 IsDramMipiYuv;
#endif

    UINT8 NeedDummyBlend = 0U;
    UINT16 i, MinViewZoneIdx, ChanIdx, VZIdx;
    UINT16 DummyBlendOutputOfstX = 0U, DummyBlendOutputOfstY = 0U;
    UINT16 VZtoChanIdx[AMBA_DSP_MAX_VIEWZONE_NUM], NumView = 0U;
    UINT32 Rval;
    UINT32 EffectBufMask[EFFECT_BUF_MASK_DEPTH];
    UINT32 YuvStrmViewOI = 0U;
    UINT32 PpStrmBufIdx;
    UINT32 TmpBufferUsage = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    DSP_EFFECT_BUF_DESC_s *pBufDesc;
    CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout;
    vproc_pp_stream_cntl_t *pPpStrmCtrl = NULL;
    //const void *pVoid;
    ULONG ULAddr;

    HL_GetYuvStrmInfoLock(YuvStrmIdx, &YuvStrmInfo);
    if (pExtStrmLayout == NULL) {
        pYuvStrmLayout = &YuvStrmInfo->Layout;
    } else {
        pYuvStrmLayout = pExtStrmLayout;
    }

#ifdef PPSTRM_SWITCH_BETWEEN_PASSTHROUGH_AND_BLENDING
    (void)HL_GetEffectChannelPostBlendNum(YuvStrmIdx, pYuvStrmLayout, &TotalPostBldNum);
    (void)HL_GetEffectChannelPostBlendNum(YuvStrmIdx, &YuvStrmInfo->Layout, &OriginalTotalBldNum);
#endif

    /* Find the end-points among real channels */
    (void)dsp_osal_memset(VZtoChanIdx, 0, sizeof(UINT16)*AMBA_DSP_MAX_VIEWZONE_NUM);
    for (ChanIdx=0U; ChanIdx<pYuvStrmLayout->NumChan; ChanIdx++) {
        DSP_SetBit(&YuvStrmViewOI, (UINT32)pYuvStrmLayout->ChanCfg[ChanIdx].ViewZoneId);
        VZtoChanIdx[pYuvStrmLayout->ChanCfg[ChanIdx].ViewZoneId] = ChanIdx;
    }
#ifdef PPSTRM_SWITCH_BETWEEN_PASSTHROUGH_AND_BLENDING
    DSP_Bit2U16Idx(YuvStrmViewOI, &MinActViewZoneIdx);
    DSP_ReverseBit2U16Idx(YuvStrmViewOI, &MaxActViewZoneIdx);
#endif
    /* Including potential channels */
    DSP_Bit2U16Idx(YuvStrmInfo->MaxChanBitMask, &MinViewZoneIdx);

    PpStrmCfg->chan_id = (UINT8)MinViewZoneIdx;

#if defined(UCODE_SUPPORT_EFFECT_PREV_BC_REARRANGE) || defined(UCODE_SUPPORT_EFFECT_PREV_ABC_REARRANGE)
    HL_GetVprocInfo(HL_MTX_OPT_ALL, MinViewZoneIdx, &VprocInfo);
    for (i=0U; i<DSP_VPROC_PIN_NUM; i++) {
        if (VprocInfo.PinUsage[i] > 0U) {
            if (1U == DSP_GetBit(VprocInfo.PinUsage[i], YuvStrmIdx, 1U)) {
                PinId = i;
                break;
            }
        }
    }

    if (PinId == DSP_VPROC_PIN_NUM) {
        AmbaLL_LogUInt5("StrmPp[%d] route fail", YuvStrmIdx, 0U, 0U, 0U, 0U);
        PpStrmCfg->output_dst = AMBA_DSP_PREV_DEST_PIN;
        PpStrmCfg->str_id = 0U;
    } else {
        (void)HL_GetViewZonePrevDest(MinViewZoneIdx, PinId, &PrevDest);
        PpStrmCfg->output_dst = (UINT8)PrevDest;
        PpStrmCfg->str_id = (UINT8)HL_CtxVprocPinDspPinMap[PinId];
    }

    { /* fill effect_grp_id same as vproc group */
        UINT16 VprocGrpId = 0U;

        Rval = HL_GetVprocGroupIdx(MinViewZoneIdx, &VprocGrpId, 0U);
        if (Rval != OK) {
            AmbaLL_LogUInt5("StrmPp YuvStrmIdx:%u VZ:%u HL_GetVprocGroupIdx:%x",
                    YuvStrmIdx, MinViewZoneIdx, Rval, 0U, 0U);
        }
        PpStrmCfg->effect_grp_id = (UINT8)VprocGrpId;
    }

#else
    IsDramMipiYuv = (UINT8)HL_GetDramMipiYuvEnable();
    (void)HL_GetEffectChannelVprocPin(YuvStrmInfo->Purpose,
                                      YuvStrmInfo->DestVout,
                                      YuvStrmInfo->DestEnc,
                                      &PinId,
                                      IsDramMipiYuv);
    if (PinId == DSP_VPROC_PIN_NUM) {
        AmbaLL_LogUInt5("StrmPp[%d] route fail", YuvStrmIdx, 0U, 0U, 0U, 0U);
        PpStrmCfg->output_dst = AMBA_DSP_PREV_DEST_PIN;
        PpStrmCfg->str_id = 0U;
    } else {
        if ((PinId == DSP_VPROC_PIN_MAIN) ||
            (PinId == DSP_VPROC_PIN_PREVA)) {
            PpStrmCfg->output_dst = AMBA_DSP_PREV_DEST_PIN;
        } else if (PinId == DSP_VPROC_PIN_PREVC) {
            PpStrmCfg->output_dst = AMBA_DSP_PREV_DEST_VOUT0;
        } else { // DSP_VPROC_PIN_PREVB
            PpStrmCfg->output_dst = AMBA_DSP_PREV_DEST_VOUT1;
        }
        PpStrmCfg->str_id = HL_CtxVprocPinDspPinMap[PinId];
    }
#endif

#ifdef PPSTRM_SWITCH_BETWEEN_PASSTHROUGH_AND_BLENDING
    if ((TotalPostBldNum > 0U) || (OriginalTotalBldNum > 0U)) {
#endif

#ifdef PPSTRM_SWITCH_CHANGE_INPUTS_NUM
    PpStrmCfg->num_of_inputs = (UINT8)pYuvStrmLayout->NumChan;
    YuvStrmInfo->Layout.CurrentMasterViewId = pYuvStrmLayout->ChanCfg[0].ViewZoneId;
#else
    PpStrmCfg->num_of_inputs = (UINT8)YuvStrmInfo->MaxChanNum;
    YuvStrmInfo->Layout.CurrentMasterViewId = YuvStrmInfo->Layout.ChanCfg[0].ViewZoneId;
#endif

    /* Sweep all overlapped area for Copy/Blend Jobs on each Chan */
    (void)HL_CalcEffectPostPJob(YuvStrmIdx, pYuvStrmLayout);
    pBufDesc = &pYuvStrmLayout->EffectBufDesc;
    PpStrmCfg->final_output_buf_id = pBufDesc->OutputBufId;

    HL_GetResourcePtr(&Resource);
    (void)dsp_osal_memcpy(&EffectBufMask[0U],
                          &Resource->EffectLogicBufMask[0U],
                          sizeof(UINT32)*EFFECT_BUF_MASK_DEPTH);

    /* Request PpStrmCtrl buffer */
    (void)DSP_ReqBuf(&YuvStrmInfo->PpStrmPoolDesc, 1U, &PpStrmBufIdx, 0U/*FullnessCheck*/);
    YuvStrmInfo->PpStrmPoolDesc.Wp = (UINT16)PpStrmBufIdx;
    HL_GetPointerToDspPpStrmCtrlCfg(YuvStrmIdx, (UINT16)PpStrmBufIdx, 0U, &pPpStrmCtrl);
    dsp_osal_typecast(&ULAddr, &pPpStrmCtrl);
    (void)dsp_osal_virt2cli(ULAddr, &PpStrmCfg->input_pp_cfg_addr_array);

    for (VZIdx=0U; VZIdx<AMBA_DSP_MAX_VIEWZONE_NUM; VZIdx++) {
        if (DSP_GetBit(YuvStrmViewOI, (UINT32)VZIdx, 1U) == 0U) {
            HL_PpNoneExist(YuvStrmIdx,
                           (UINT8)PpStrmCfg->str_id,
                           VZIdx,   //VZIdx as channel
                           MinViewZoneIdx,
                           pBufDesc->OutputBufId,
                           (UINT16)PpStrmBufIdx,
                           PinId,
                           &TmpBufferUsage,
                           YuvStrmInfo,
                           &NumView,
                           &NeedDummyBlend);
#ifdef DEBUG_PP_REBASE
            AmbaLL_LogUInt5("HL_FillPpStrmCfg HL_PpNoneExist 3 VZIdx:%u VZtoChanIdx:%u NeedDummyBlend:%u",
                            VZIdx, VZtoChanIdx[VZIdx], NeedDummyBlend, 0U, 0U);
#endif
        } else if (DSP_GetBit(YuvStrmViewOI, (UINT32)VZIdx, 1U) == 1U) {
            HL_PpExist(YuvStrmIdx,
                       (UINT16)PpStrmBufIdx,
                       (UINT16)PpStrmBufIdx,
                       (UINT8)PpStrmCfg->str_id,
                       YuvStrmInfo->MaxChanNum,
                       VZtoChanIdx[VZIdx],
                       pYuvStrmLayout,
                       MinViewZoneIdx,
                       pBufDesc->OutputBufId,
                       &TmpBufferUsage,
                       &DummyBlendOutputOfstX,
                       &DummyBlendOutputOfstY,
                       &NumView,
                       &NeedDummyBlend);
#ifdef DEBUG_PP_REBASE
            AmbaLL_LogUInt5("HL_FillPpStrmCfg HL_PpExist 3 VZIdx:%u VZtoChanIdx:%u NeedDummyBlend:%u",
                            VZIdx, VZtoChanIdx[VZIdx], NeedDummyBlend, 0U, 0U);
#endif
        } else {
            // inactive channels
        }
        if (NumView >= YuvStrmInfo->MaxChanNum) {
            break;
        }
    }//for (VZIdx=0U; (VZIdx<AMBA_DSP_MAX_VIEWZONE_NUM) && (NumView < YuvStrmInfo->MaxChanNum); VZIdx++) {

    // dummy blending
    if (NeedDummyBlend == 1U) {
        Rval = HL_PpDumBld(YuvStrmIdx,
                           (UINT16)PpStrmBufIdx,
                           (UINT16)PpStrmBufIdx,
                           DummyBlendOutputOfstX,
                           DummyBlendOutputOfstY,
                           Resource,
                           TmpBufferUsage,
                           NumView,
                           pBufDesc,
                           EffectBufMask);
    }

    //CleanCache
    //HL_GetPointerToDspPpStrmCtrlCfg(YuvStrmIdx, (UINT16)PpStrmBufIdx, 0U, &pPpStrmCtrl);
    //dsp_osal_typecast(&pVoid, &pPpStrmCtrl);
    //(void)dsp_osal_cache_clean(pVoid, sizeof(vproc_pp_stream_cntl_t)*pYuvStrmLayout->NumChan);

    HL_GetResourceLock(&Resource);
    (void)dsp_osal_memcpy(&Resource->EffectLogicBufMask[0U],
                          &EffectBufMask[0U],
                          sizeof(UINT32)*EFFECT_BUF_MASK_DEPTH);
    HL_GetResourceUnLock();

    HL_GetYuvStrmInfoUnLock(YuvStrmIdx);
#ifdef PPSTRM_SWITCH_BETWEEN_PASSTHROUGH_AND_BLENDING
    } else { //Passthrough
        HL_GetYuvStrmInfoLock(YuvStrmIdx, &YuvStrmInfo);

        PpStrmCfg->num_of_inputs = (UINT8)pYuvStrmLayout->NumChan;

        pBufDesc = &pYuvStrmLayout->EffectBufDesc;

        /* Request PpStrmCtrl buffer */
        (void)DSP_ReqBuf(&YuvStrmInfo->PpStrmPoolDesc, 1U, &NewWp, 0U/*FullnessCheck*/);
        PpStrmBufIdx = (UINT16)NewWp;
        YuvStrmInfo->PpStrmPoolDesc.Wp = (UINT16)NewWp;
        HL_GetPointerToDspPpStrmCtrlCfg(YuvStrmIdx, PpStrmBufIdx, 0U, &pPpStrmCtrl);
        dsp_osal_typecast(&ULAddr, &pPpStrmCtrl);
        (void)dsp_osal_virt2cli(ULAddr, &PpStrmCfg->input_pp_cfg_addr_array);

        HL_GetResourcePtr(&Resource);
        (void)dsp_osal_memcpy(&EffectBufMask[0U], &Resource->EffectLogicBufMask[0U], sizeof(UINT32)*EFFECT_BUF_MASK_DEPTH);

        /* Release BufferId linked to this YuvStream */
        if (pBufDesc->OutputBufId != DSP_EFCT_INVALID_IDX) {
            DSP_ClearU32ArrayBit(&EffectBufMask[0U], pBufDesc->OutputBufId, EFFECT_BUF_MASK_DEPTH);
        }
        if (pBufDesc->CpyBufId != DSP_EFCT_INVALID_IDX) {
            DSP_ClearU32ArrayBit(&EffectBufMask[0U], pBufDesc->CpyBufId, EFFECT_BUF_MASK_DEPTH);
        }
        if (pBufDesc->Y2YBufId != DSP_EFCT_INVALID_IDX) {
            DSP_ClearU32ArrayBit(&EffectBufMask[0U], pBufDesc->Y2YBufId, EFFECT_BUF_MASK_DEPTH);
        }
        /* Reset Buffer Id */
        pBufDesc->OutputBufId   = DSP_EFCT_INVALID_IDX;
        pBufDesc->CpyBufId      = DSP_EFCT_INVALID_IDX;
        pBufDesc->Y2YBufId      = DSP_EFCT_INVALID_IDX;

        DSP_FindEmptyBit(&EffectBufMask[0U], Resource->EffectLogicBufNum, &pBufDesc->OutputBufId, 0U/*FindOnly*/, EFFECT_BUF_MASK_DEPTH);
        if (pBufDesc->OutputBufId == DSP_EFCT_INVALID_IDX) {
            AmbaLL_LogUInt5("  No Buf for PassThruOut 0x%X%X%X%X",
                            EffectBufMask[3U], EffectBufMask[2U], EffectBufMask[1U], EffectBufMask[0U], 0U);
            Rval = DSP_ERR_0006;
        } else {
            PpStrmCfg->final_output_buf_id = pBufDesc->OutputBufId;
        }

        for (j=0U; j<pYuvStrmLayout->NumChan; j++) {
            HL_GetPointerToDspPpStrmCtrlCfg(YuvStrmIdx, PpStrmBufIdx, j, &pPpStrmCtrl);
            (void)dsp_osal_memset(pPpStrmCtrl, 0, sizeof(vproc_pp_stream_cntl_t));

            pPpStrmCtrl->input_channel_num = (UINT8)pYuvStrmLayout->ChanCfg[j].ViewZoneId;
            pPpStrmCtrl->input_stream_num = (UINT8)PpStrmCfg->str_id;

            pPpStrmCtrl->output_x_ofs = pYuvStrmLayout->ChanCfg[j].Window.OffsetX;
            pPpStrmCtrl->output_y_ofs = pYuvStrmLayout->ChanCfg[j].Window.OffsetY;
            pPpStrmCtrl->output_width = pYuvStrmLayout->ChanCfg[j].Window.Width;
            pPpStrmCtrl->output_height = pYuvStrmLayout->ChanCfg[j].Window.Height;

            pPpStrmCtrl->output_buf_id = pBufDesc->OutputBufId;
            if (pYuvStrmLayout->ChanCfg[j].ViewZoneId == MinActViewZoneIdx) {
                pPpStrmCtrl->stream_cfg.is_alloc_out_buf = (UINT8)1U;
            } else {
                pPpStrmCtrl->stream_cfg.is_alloc_out_buf = (UINT8)0U;
            }
            pPpStrmCtrl->stream_cfg.is_last_blending_channel = (UINT8)0U;
            if (pYuvStrmLayout->ChanCfg[j].ViewZoneId == MaxActViewZoneIdx) {
                pPpStrmCtrl->stream_cfg.is_last_passthrough_channel = (UINT8)1U;
                pBufDesc->LastPsThIdx = MaxActViewZoneIdx;
            } else {
                pPpStrmCtrl->stream_cfg.is_last_passthrough_channel = (UINT8)0U;
            }
            pPpStrmCtrl->stream_cfg.is_last_copy_channel = (UINT8)0U;
            pPpStrmCtrl->stream_cfg.num_of_post_r2y_copy_ops = (UINT8)0U;
            pPpStrmCtrl->stream_cfg.num_of_y2y_blending_ops = (UINT8)0U;
            (void)dsp_osal_virt2cli(NullAddr, &pPpStrmCtrl->post_r2y_copy_cfg_address);
            (void)dsp_osal_virt2cli(NullAddr, &pPpStrmCtrl->y2y_blending_cfg_address);
        }

        //CleanCache
        HL_GetPointerToDspPpStrmCtrlCfg(YuvStrmIdx, PpStrmBufIdx, 0U, &pPpStrmCtrl);
        dsp_osal_typecast(&pVoid, &pPpStrmCtrl);
        (void)dsp_osal_cache_clean(pVoid, sizeof(vproc_pp_stream_cntl_t)*YuvStrmInfo->MaxChanNum);
        HL_GetYuvStrmInfoUnLock(YuvStrmIdx);

        HL_GetResourceLock(&Resource);
        (void)dsp_osal_memcpy(&Resource->EffectLogicBufMask[0U], &EffectBufMask[0U], sizeof(UINT32)*EFFECT_BUF_MASK_DEPTH);
        HL_GetResourceUnLock();
    }
#endif
    return Rval;
}

static void HL_FillPpStrmCfgOrderInitTables(const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout, UINT16 *VZtoChanIdx, UINT32 *pYuvStrmViewOI)
{
    UINT16 ChanIdx;

    (void)dsp_osal_memset(VZtoChanIdx, 0, sizeof(UINT16)*AMBA_DSP_MAX_VIEWZONE_NUM);
    for (ChanIdx = 0U; ChanIdx < pYuvStrmLayout->NumChan; ChanIdx++) {
        DSP_SetBit(pYuvStrmViewOI, (UINT32)pYuvStrmLayout->ChanCfg[ChanIdx].ViewZoneId);
        VZtoChanIdx[pYuvStrmLayout->ChanCfg[ChanIdx].ViewZoneId] = ChanIdx;
    }
}

#if defined(UCODE_SUPPORT_EFFECT_PREV_BC_REARRANGE) || defined(UCODE_SUPPORT_EFFECT_PREV_ABC_REARRANGE)
static inline void HL_PpStrmGetVprocPinId(const CTX_VPROC_INFO_s *pVprocInfo,
                                          UINT16 YuvStrmIdx,
                                          UINT16 *pPinId)
{
    UINT16 i;

    for (i = 0U; i < DSP_VPROC_PIN_NUM; i++) {
        if (pVprocInfo->PinUsage[i] > 0U) {
            if (1U == DSP_GetBit(pVprocInfo->PinUsage[i], YuvStrmIdx, 1U)) {
                *pPinId = i;
                break;
            }
        }
    }
}
#endif

UINT32 HL_FillPpStrmCfgOrder(UINT16 YuvStrmIdx,
                             const UINT16 *pYuvIdxOrder,
                             cmd_vproc_multi_stream_pp_t *PpStrmCfg,
                             CTX_YUV_STRM_LAYOUT_s *pExtStrmLayout)
{
#ifdef PPSTRM_SWITCH_BETWEEN_PASSTHROUGH_AND_BLENDING
    UINT16 MinActViewZoneIdx, MaxActViewZoneIdx;
    UINT16 TotalPostBldNum, OriginalTotalBldNum;
#endif
#if defined(UCODE_SUPPORT_EFFECT_PREV_BC_REARRANGE) || defined(UCODE_SUPPORT_EFFECT_PREV_ABC_REARRANGE)
    UINT16 PinId = 0U;
    UINT16 PrevDest;
    CTX_VPROC_INFO_s VprocInfo = {0};
#else
    UINT8 IsDramMipiYuv;
#endif
    UINT8 NeedDummyBlend = 0U;
    UINT16 MinViewZoneIdx, ChanId, VZIdx;
    UINT16 DummyBlendOutputOfstX = 0U, DummyBlendOutputOfstY = 0U;
    UINT16 VZtoChanIdx[AMBA_DSP_MAX_VIEWZONE_NUM], NumView = 0U;
    UINT32 Rval;
    UINT32 EffectBufMask[EFFECT_BUF_MASK_DEPTH];
    UINT32 YuvStrmViewOI = 0U;
    UINT32 PpStrmBufIdx;
    UINT32 TmpBufferUsage = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    DSP_EFFECT_BUF_DESC_s *pBufDesc;
    CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout;
    vproc_pp_stream_cntl_t *pPpStrmCtrl = NULL;
    //const void *pVoid;
    ULONG ULAddr;

    HL_GetYuvStrmInfoPtr(YuvStrmIdx, &YuvStrmInfo);
    if (pExtStrmLayout == NULL) {
        pYuvStrmLayout = &YuvStrmInfo->Layout;
    } else {
        pYuvStrmLayout = pExtStrmLayout;
    }
#ifdef DEBUG_PP_REBASE
    AmbaLL_LogUInt5("HL_FillPpStrmCfgOrder 7741 YuvStrmIdx:%u pYuvStrmLayout->ChanCfg[].ViewZoneId:%u/%u/%u",
            YuvStrmIdx,
            pYuvStrmLayout->ChanCfg[0].ViewZoneId,
            pYuvStrmLayout->ChanCfg[1].ViewZoneId,
            pYuvStrmLayout->ChanCfg[2].ViewZoneId,
            0);
    AmbaLL_LogUInt5("HL_FillPpStrmCfgOrder 7741 pYuvIdxOrder:%u/%u/%u/%u MaxChanBitMask:0x%x",
            pYuvIdxOrder[0],
            pYuvIdxOrder[1],
            pYuvIdxOrder[2],
            pYuvIdxOrder[3],
            YuvStrmInfo->MaxChanBitMask);
#endif

#ifdef PPSTRM_SWITCH_BETWEEN_PASSTHROUGH_AND_BLENDING
    (void)HL_GetEffectChannelPostBlendNum(YuvStrmIdx, pYuvStrmLayout, &TotalPostBldNum);
    (void)HL_GetEffectChannelPostBlendNum(YuvStrmIdx, &YuvStrmInfo->Layout, &OriginalTotalBldNum);
#endif

    /* Find the end-points among real channels */
    HL_FillPpStrmCfgOrderInitTables(pYuvStrmLayout, VZtoChanIdx, &YuvStrmViewOI);

#ifdef DEBUG_PP_REBASE
    AmbaLL_LogUInt5("HL_FillPpStrmCfgOrder 9248 YuvStrmViewOI:0x%x VZtoChanIdx:%u/%u/%u/%u",
            YuvStrmViewOI,
            VZtoChanIdx[0],
            VZtoChanIdx[1],
            VZtoChanIdx[2],
            VZtoChanIdx[3]);
#endif

#ifdef PPSTRM_SWITCH_BETWEEN_PASSTHROUGH_AND_BLENDING
    DSP_Bit2U16Idx(YuvStrmViewOI, &MinActViewZoneIdx);
    DSP_ReverseBit2U16Idx(YuvStrmViewOI, &MaxActViewZoneIdx);
#endif
    /* Including potential channels */
    DSP_Bit2U16Idx(YuvStrmInfo->MaxChanBitMask, &MinViewZoneIdx);

    PpStrmCfg->chan_id = (UINT8)MinViewZoneIdx;

#if defined(UCODE_SUPPORT_EFFECT_PREV_BC_REARRANGE) || defined(UCODE_SUPPORT_EFFECT_PREV_ABC_REARRANGE)
    HL_GetVprocInfo(HL_MTX_OPT_ALL, MinViewZoneIdx, &VprocInfo);
    HL_PpStrmGetVprocPinId(&VprocInfo, YuvStrmIdx, &PinId);

    if (PinId == DSP_VPROC_PIN_NUM) {
        AmbaLL_LogUInt5("StrmPp[%d] route fail", YuvStrmIdx, 0U, 0U, 0U, 0U);
        PpStrmCfg->output_dst = AMBA_DSP_PREV_DEST_PIN;
        PpStrmCfg->str_id = 0U;
    } else {
        (void)HL_GetViewZonePrevDest(MinViewZoneIdx, PinId, &PrevDest);
        PpStrmCfg->output_dst = (UINT8)PrevDest;
        PpStrmCfg->str_id = (UINT8)HL_CtxVprocPinDspPinMap[PinId];
    }
#ifdef DEBUG_PP_REBASE
    AmbaLL_LogUInt5("HL_FillPpStrmCfgOrder 9279 MinViewZoneIdx:%u PinId:%d str_id:%u",
            MinViewZoneIdx,
            PinId,
            HL_CtxVprocPinDspPinMap[PinId],
            0,
            0);
    AmbaLL_LogUInt5("HL_FillPpStrmCfgOrder 9279 VprocInfo:%u/%u/%u/%u",
            VprocInfo.PinUsage[0],
            VprocInfo.PinUsage[1],
            VprocInfo.PinUsage[2],
            VprocInfo.PinUsage[3],
            0);
#endif
    { /* fill effect_grp_id same as vproc group */
        UINT16 VprocGrpId = 0U;

        Rval = HL_GetVprocGroupIdx(MinViewZoneIdx, &VprocGrpId, 0U);
        if (Rval != OK) {
            AmbaLL_LogUInt5("StrmPp YuvStrmIdx:%u VZ:%u HL_GetVprocGroupIdx:%x",
                    YuvStrmIdx, MinViewZoneIdx, Rval, 0U, 0U);
        }
        PpStrmCfg->effect_grp_id = (UINT8)VprocGrpId;
    }
#else
    IsDramMipiYuv = (UINT8)HL_GetDramMipiYuvEnable();
    (void)HL_GetEffectChannelVprocPin(YuvStrmInfo->Purpose,
                                      YuvStrmInfo->DestVout,
                                      YuvStrmInfo->DestEnc,
                                      &PinId,
                                      IsDramMipiYuv);
    if (PinId == DSP_VPROC_PIN_NUM) {
        AmbaLL_LogUInt5("StrmPp[%d] route fail", YuvStrmIdx, 0U, 0U, 0U, 0U);
        PpStrmCfg->output_dst = AMBA_DSP_PREV_DEST_PIN;
        PpStrmCfg->str_id = 0U;
    } else {
        if ((PinId == DSP_VPROC_PIN_MAIN) ||
            (PinId == DSP_VPROC_PIN_PREVA)) {
            PpStrmCfg->output_dst = AMBA_DSP_PREV_DEST_PIN;
        } else if (PinId == DSP_VPROC_PIN_PREVC) {
            PpStrmCfg->output_dst = AMBA_DSP_PREV_DEST_VOUT0;
        } else { // DSP_VPROC_PIN_PREVB
            PpStrmCfg->output_dst = AMBA_DSP_PREV_DEST_VOUT1;
        }
        PpStrmCfg->str_id = HL_CtxVprocPinDspPinMap[PinId];
    }
#endif

#ifdef PPSTRM_SWITCH_BETWEEN_PASSTHROUGH_AND_BLENDING
    if ((TotalPostBldNum > 0U) || (OriginalTotalBldNum > 0U)) {
#endif

#ifdef PPSTRM_SWITCH_CHANGE_INPUTS_NUM
    PpStrmCfg->num_of_inputs = (UINT8)pYuvStrmLayout->NumChan;
    YuvStrmInfo->Layout.CurrentMasterViewId = pYuvStrmLayout->ChanCfg[0].ViewZoneId;
#else
    PpStrmCfg->num_of_inputs = (UINT8)YuvStrmInfo->MaxChanNum;
    YuvStrmInfo->Layout.CurrentMasterViewId = YuvStrmInfo->Layout.ChanCfg[0].ViewZoneId;
#endif

    /* Sweep all overlapped area for Copy/Blend Jobs on each Chan */
    Rval = HL_CalcEffectPostPJob(YuvStrmIdx, pYuvStrmLayout);
    /* Adjust new master chan to the first chan in Pp, depend on new pYuvIdxOrder array */
    if (Rval == OK) {
        Rval = HL_EffPostPReBase(YuvStrmIdx, pYuvIdxOrder, pYuvStrmLayout);
        if (Rval == OK) {
            pBufDesc = &pYuvStrmLayout->EffectBufDesc;
            PpStrmCfg->final_output_buf_id = pBufDesc->OutputBufId;

            HL_GetResourcePtr(&Resource);
            (void)dsp_osal_memcpy(&EffectBufMask[0U],
                                  &Resource->EffectLogicBufMask[0U],
                                  sizeof(UINT32)*EFFECT_BUF_MASK_DEPTH);

            /* Request PpStrmCtrl buffer */
            Rval = DSP_ReqBuf(&YuvStrmInfo->PpStrmPoolDesc, 1U, &PpStrmBufIdx, 0U/*FullnessCheck*/);
            if (Rval == OK) {
                YuvStrmInfo->PpStrmPoolDesc.Wp = (UINT16)PpStrmBufIdx;
                HL_GetPointerToDspPpStrmCtrlCfg(YuvStrmIdx, (UINT16)PpStrmBufIdx, 0U, &pPpStrmCtrl);
                dsp_osal_typecast(&ULAddr, &pPpStrmCtrl);
                (void)dsp_osal_virt2cli(ULAddr, &PpStrmCfg->input_pp_cfg_addr_array);

                for (VZIdx=0U; VZIdx<AMBA_DSP_MAX_VIEWZONE_NUM; VZIdx++) {
                    //use re-ordered ChanId
                    ChanId = pYuvIdxOrder[VZIdx];

                    //fill pPpStrmCtrl depend on existence
                    //getting none exist vz
                    if (DSP_GetBit(YuvStrmViewOI, ChanId, 1U) == 0U) {
                        if (DSP_GetBit(YuvStrmInfo->MaxChanBitMask, ChanId, 1U) == 1U) {
#ifdef DEBUG_PP_REBASE
                            AmbaLL_LogUInt5("HL_FillPpStrmCfgOrder VZIdx:%u HL_PpNoneExist 0 YuvStrmViewOI:%x ChanId:%u ChannelOutputMask:%x",
                                    VZIdx, YuvStrmViewOI, ChanId, 0U, 0U);
#endif
                            HL_PpNoneExist(YuvStrmIdx,
                                           (UINT8)PpStrmCfg->str_id,
                                           ChanId,
                                           pYuvIdxOrder[0],     //none exist should take first vz as base
                                           pBufDesc->OutputBufId,
                                           (UINT16)PpStrmBufIdx,
                                           PinId,
                                           &TmpBufferUsage,
                                           YuvStrmInfo,
                                           &NumView,
                                           &NeedDummyBlend);
#ifdef DEBUG_PP_REBASE
                            AmbaLL_LogUInt5("HL_FillPpStrmCfgOrder VZIdx:%u HL_PpNoneExist 1 YuvStrmViewOI:%x ChanId:%u",
                                    VZIdx, YuvStrmViewOI, ChanId, 0U, 0U);
#endif
                        }
                    } else if (DSP_GetBit(YuvStrmViewOI, ChanId, 1U) == 1U) {
#ifdef DEBUG_PP_REBASE
                        AmbaLL_LogUInt5("HL_FillPpStrmCfgOrder VZIdx:%u HL_PpExist 2 YuvStrmOI:%x ChanId:%u VZtoChanIdx:%u NeedDummyBlend:%u",
                                VZIdx, YuvStrmViewOI, ChanId, VZtoChanIdx[ChanId], NeedDummyBlend);
#endif
                        HL_PpExistCfgReBase(YuvStrmIdx,
                                            (UINT16)PpStrmBufIdx,
                                            (UINT8)PpStrmCfg->str_id,
                                            YuvStrmInfo->MaxChanNum,
                                            pYuvIdxOrder[0U],
                                            VZIdx,
                                            VZtoChanIdx[ChanId],    //use original chan id in layout
                                            pBufDesc->OutputBufId,
                                            pYuvStrmLayout,
                                            &TmpBufferUsage,
                                            &DummyBlendOutputOfstX,
                                            &DummyBlendOutputOfstY,
                                            &NumView,
                                            &NeedDummyBlend);
#ifdef DEBUG_PP_REBASE
                        AmbaLL_LogUInt5("HL_FillPpStrmCfgOrder Idx:%u HL_PpExist 3 NeedDummyBlend:%u",
                                VZIdx, NeedDummyBlend, 0, 0, 0U);
#endif
                    } else {
                        // inactive channels
                    }
                    if (NumView >= YuvStrmInfo->MaxChanNum) {
                        break;
                    }
#ifdef DEBUG_PP_REBASE
        AmbaLL_LogUInt5("HL_FillPpStrmCfgOrder %u VZIdx:%u ChanId:%u NumView:%u NeedDummyBlend:%u",
                __LINE__, VZIdx, ChanId, NumView, NeedDummyBlend);
#endif
                }//for (VZIdx=0U; (VZIdx<AMBA_DSP_MAX_VIEWZONE_NUM) && (NumView < YuvStrmInfo->MaxChanNum); VZIdx++) {
                // dummy blending
                if (NeedDummyBlend == 1U) {
                    Rval = HL_PpDumBld(YuvStrmIdx,
                                       (UINT16)PpStrmBufIdx,
                                       (UINT16)PpStrmBufIdx,
                                       DummyBlendOutputOfstX,
                                       DummyBlendOutputOfstY,
                                       Resource,
                                       TmpBufferUsage,
                                       NumView,
                                       pBufDesc,
                                       EffectBufMask);
                }

                //CleanCache
                //HL_GetPointerToDspPpStrmCtrlCfg(YuvStrmIdx, (UINT16)PpStrmBufIdx, 0U, &pPpStrmCtrl);
                //dsp_osal_typecast(&pVoid, &pPpStrmCtrl);
                //(void)dsp_osal_cache_clean(pVoid, sizeof(vproc_pp_stream_cntl_t)*pYuvStrmLayout->NumChan);
                HL_GetResourceLock(&Resource);
                (void)dsp_osal_memcpy(&Resource->EffectLogicBufMask[0U],
                                      &EffectBufMask[0U],
                                      sizeof(UINT32)*EFFECT_BUF_MASK_DEPTH);
                HL_GetResourceUnLock();
            }
        }
    }
    return Rval;
}

static inline void HL_FillSyncJobIntoBatchDisplay(const DSP_EFCT_SYNC_JOB_s *pSyncJob,
                                                  const UINT16 JobIdx,
                                                  const UINT16 LastViewZoneId,
                                                  const CTX_EFCT_SYNC_JOB_SET_s *pSyncJobSet,
                                                  const UINT32 *pBatchQAddr,
                                                  UINT16 *pJobStatus,
                                                  UINT16 *pGrpQCmdNum,
                                                  UINT16 *pBatchCmdNum,
                                                  UINT16 *pPreBatchCmdNumInGrpCmd,
                                                  UINT16 *pBatchCmdNumAccumedInGrpCmd)
{
    UINT16 JobStatus = *pJobStatus;
    UINT16 GrpQCmdNum = *pGrpQCmdNum;
    UINT16 BatchCmdNum = *pBatchCmdNum;
    UINT16 PreBatchCmdNumInGrpCmd;
    UINT16 BatchCmdNumAccumedInGrpCmd = *pBatchCmdNumAccumedInGrpCmd; //FirstGrpCmd update BatchCmdNum
    const UINT32 *pU32Val;
    UINT32 U32Val;
    UINT32 *pGrpQAddr = NULL;
    cmd_vout_osd_setup_t *pOsdSetup = NULL;
    cmd_vproc_fov_grp_cmd_t *pVprocGrpCmd = NULL;
    cmd_vout_osd_clut_setup_t *pOsdClutCfg = NULL;
    ULONG ULAddr;

    /* Update JobStatus of Non-Master Job */
    DSP_SetU16Bit(&JobStatus, JobIdx);

    /* Copy into GrpQ one by one */
    HL_GetPointerToDspGroupCmdQ((UINT16)pSyncJob->JobId.YuvStrmId,
                                ((UINT16)pSyncJob->JobId.SeqIdx - 1U),
                                ((UINT16)pSyncJob->JobId.SubJobIdx - 1U),
                                &pGrpQAddr);

    /* OSD Setup */
    pU32Val = &pGrpQAddr[CMD_SIZE_IN_WORD*GrpQCmdNum];
    dsp_osal_typecast(&pOsdSetup, &pU32Val);
    (void)dsp_osal_memset(pOsdSetup, 0, sizeof(DSP_CMD_s));
    (void)HL_FillOsdSetup(pSyncJob->VoutIdx, &pSyncJobSet->OsdCfg[pSyncJob->VoutIdx], pOsdSetup);
    pOsdSetup->cmd_code = CMD_VOUT_OSD_SETUP;
    pOsdSetup->en = pSyncJobSet->OsdEnable[pSyncJob->VoutIdx];
    /* PrintOut cmd for debug */
    (void)AmbaHL_CmdVoutOsdSetup(AMBA_DSP_CMD_NOP, pOsdSetup);
    GrpQCmdNum++;

    /* OSD CLUT */
    if (pSyncJobSet->OsdCfg[pSyncJob->VoutIdx].DataFormat == OSD_8BIT_CLUT_MODE) {
        pU32Val = &pGrpQAddr[CMD_SIZE_IN_WORD*GrpQCmdNum];
        dsp_osal_typecast(&pOsdClutCfg, &pU32Val);
        (void)dsp_osal_memset(pOsdClutCfg, 0, sizeof(DSP_CMD_s));
        pOsdClutCfg->cmd_code = CMD_VOUT_OSD_CLUT_SETUP;
        pOsdClutCfg->vout_id = pSyncJob->VoutIdx;
        (void)dsp_osal_virt2cli(pSyncJobSet->OsdCfg[pSyncJob->VoutIdx].CLUTAddr, &pOsdClutCfg->clut_dram_addr);
        /* PrintOut cmd for debug */
        (void)AmbaHL_CmdVoutOsdClutSetup(AMBA_DSP_CMD_NOP, pOsdClutCfg);
        GrpQCmdNum++;
    }

    /* VideoSetup */
    //TBD

    /* VprocGrpCmd */
    /* First Processed JobType in this condition, always write to new BatchSlot */
    pU32Val = &pBatchQAddr[CMD_SIZE_IN_WORD*BatchCmdNum];
    PreBatchCmdNumInGrpCmd = BatchCmdNum;

    dsp_osal_typecast(&pVprocGrpCmd, &pU32Val);
    (void)dsp_osal_memset(pVprocGrpCmd, 0, sizeof(DSP_CMD_s));
    pVprocGrpCmd->cmd_code = CMD_VPROC_GRP_CMD;
    pVprocGrpCmd->channel_id = (UINT8)LastViewZoneId; //LastChannel
    pVprocGrpCmd->is_sent_from_sys = (UINT8)1U; //be TRUE all the time
    pVprocGrpCmd->is_vout_cmds = (UINT8)1U;
    pVprocGrpCmd->vout_id = pSyncJob->VoutIdx;
    (void)dsp_osal_memcpy(&U32Val, &pSyncJob->JobId, sizeof(UINT32));
    pVprocGrpCmd->grp_cmd_buf_id = U32Val;
    dsp_osal_typecast(&ULAddr, &pGrpQAddr);
    (void)dsp_osal_virt2cli(ULAddr, &pVprocGrpCmd->grp_cmd_buf_addr);
    pVprocGrpCmd->num_cmds_in_buf = (UINT32)GrpQCmdNum;
    BatchCmdNum++;
    /* PrintOut cmd for debug */
    (void)AmbaHL_CmdVprocGrpCmd(AMBA_DSP_CMD_NOP, pVprocGrpCmd);
#ifdef DEBUG_SYNC_CMD
    AmbaLL_LogUInt5("DISPLAY [%d] << Grp[0x%X] BchNum[%d]", __LINE__, pVprocGrpCmd->grp_cmd_buf_id, BatchCmdNum, 0, 0);
#endif

    *pJobStatus = JobStatus;
    *pBatchCmdNum = BatchCmdNum;
    *pGrpQCmdNum = GrpQCmdNum;
    *pPreBatchCmdNumInGrpCmd = PreBatchCmdNumInGrpCmd;
    *pBatchCmdNumAccumedInGrpCmd = BatchCmdNumAccumedInGrpCmd;
}

static inline void HL_FillSyncJobIntoBatchGeo(const DSP_EFCT_SYNC_JOB_s *pSyncJob,
                                              const UINT16 JobIdx,
                                              const UINT16 ViewZoneId,
                                              UINT32 *pBatchQAddr,
                                              UINT16 *pJobStatus,
                                              UINT16 *pGrpQCmdNum,
                                              UINT16 *pBatchCmdNum,
                                              UINT16 *pPreBatchCmdNumInGrpCmd,
                                              UINT16 *pBatchCmdNumAccumedInGrpCmd)
{
    UINT32 *pU32Val;
    UINT32 *pGrpQAddr = NULL;
    UINT32 U32Val;
    UINT16 JobStatus = *pJobStatus;
    UINT16 GrpQCmdNum = *pGrpQCmdNum;
    UINT16 BatchCmdNum = *pBatchCmdNum;
    UINT16 PreBatchCmdNumInGrpCmd = *pPreBatchCmdNumInGrpCmd;
    UINT16 BatchCmdNumAccumedInGrpCmd = *pBatchCmdNumAccumedInGrpCmd; //FirstGrpCmd update BatchCmdNum
    cmd_vproc_fov_grp_cmd_t *pVprocGrpCmd = NULL;
    ULONG ULAddr;

    if (ViewZoneId == pSyncJob->ViewZoneIdx) { /* Put into BatchQ */
        /* WarpGrp */
        if (pSyncJob->WarpUpdate == 1U) {
            pU32Val = &pBatchQAddr[CMD_SIZE_IN_WORD*BatchCmdNum];
            (void)dsp_osal_memcpy(pU32Val, &pSyncJob->WarpGrpCmd, sizeof(DSP_CMD_s));
            DSP_SetU16Bit(&JobStatus, EFCT_SYNC_JOB_STATUS_BATCH_BIT);
            BatchCmdNum++;
        }

#ifdef DEBUG_SYNC_CMD
        AmbaLL_LogUInt5("GEO [%d] << BchNum[%d]", __LINE__, BatchCmdNum, 0, 0, 0);
#endif
    } else { /* Put into GrpQ */
        /* Update JobStatus of Non-Master Job */
        DSP_SetU16Bit(&JobStatus, JobIdx);

        /* Copy into GrpQ one by one */
        HL_GetPointerToDspGroupCmdQ((UINT16)pSyncJob->JobId.YuvStrmId,
                                    ((UINT16)pSyncJob->JobId.SeqIdx - 1U),
                                    ((UINT16)pSyncJob->JobId.SubJobIdx - 1U),
                                    &pGrpQAddr);

        /* WarpGrp */
        if (pSyncJob->WarpUpdate == 1U) {
            pU32Val = &pGrpQAddr[CMD_SIZE_IN_WORD*GrpQCmdNum];
            (void)dsp_osal_memcpy(pU32Val, &pSyncJob->WarpGrpCmd, sizeof(DSP_CMD_s));
            GrpQCmdNum++;
        }

        /* VprocGrpCmd */
        /* First Processed JobType in this condition, always write to new BatchSlot */
        pU32Val = &pBatchQAddr[CMD_SIZE_IN_WORD*BatchCmdNum];
        PreBatchCmdNumInGrpCmd = BatchCmdNum;

        dsp_osal_typecast(&pVprocGrpCmd, &pU32Val);
        (void)dsp_osal_memset(pVprocGrpCmd, 0, sizeof(DSP_CMD_s));
        pVprocGrpCmd->cmd_code = CMD_VPROC_GRP_CMD;
        pVprocGrpCmd->channel_id = (UINT8)pSyncJob->ViewZoneIdx;
        pVprocGrpCmd->is_sent_from_sys = (UINT8)1U; //be TRUE all the time
        pVprocGrpCmd->is_vout_cmds = 0U;
        pVprocGrpCmd->vout_id = 0U; //DONT CARE in this case
        (void)dsp_osal_memcpy(&U32Val, &pSyncJob->JobId, sizeof(UINT32));
        pVprocGrpCmd->grp_cmd_buf_id = U32Val;
        dsp_osal_typecast(&ULAddr, &pGrpQAddr);
        (void)dsp_osal_virt2cli(ULAddr, &pVprocGrpCmd->grp_cmd_buf_addr);
        pVprocGrpCmd->num_cmds_in_buf = (UINT32)GrpQCmdNum;
        BatchCmdNumAccumedInGrpCmd = 1U;
        BatchCmdNum++;
        /* PrintOut cmd for debug */
        (void)AmbaHL_CmdVprocGrpCmd(AMBA_DSP_CMD_NOP, pVprocGrpCmd);
#ifdef DEBUG_SYNC_CMD
        AmbaLL_LogUInt5("GEO [%d] << Grp[0x%X] BchNum[%d]", __LINE__, pVprocGrpCmd->grp_cmd_buf_id, BatchCmdNum, 0, 0);
#endif
    }

    *pJobStatus = JobStatus;
    *pBatchCmdNum = BatchCmdNum;
    *pGrpQCmdNum = GrpQCmdNum;
    *pPreBatchCmdNumInGrpCmd = PreBatchCmdNumInGrpCmd;
    *pBatchCmdNumAccumedInGrpCmd = BatchCmdNumAccumedInGrpCmd;
}

static inline UINT32 HL_FillSyncJobIntoBatchLayoutNoneMaster(const DSP_EFCT_SYNC_JOB_s *pSyncJob,
                                                             const UINT16 JobIdx,
                                                             const UINT16 YuvStrmIdx,
                                                             const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout,
                                                             const CTX_YUV_STRM_LAYOUT_s *pSyncJobSetLayout,
                                                             const UINT32 *pBatchQAddr,
                                                             UINT16 *pJobStatus,
                                                             UINT16 *pGrpQCmdNum,
                                                             UINT16 *pBatchCmdNum,
                                                             UINT16 *pPreBatchCmdNumInGrpCmd,
                                                             UINT16 *pBatchCmdNumAccumedInGrpCmd)
{
    UINT8 PrevId = DSP_VPROC_PREV_B, CmdBypass = 0U;
    UINT16 PinId = DSP_VPROC_PIN_PREVC;
    UINT16 JobStatus = *pJobStatus;
    UINT16 GrpQCmdNum = *pGrpQCmdNum;
    UINT16 BatchCmdNum = *pBatchCmdNum;
    UINT16 PreBatchCmdNumInGrpCmd = *pPreBatchCmdNumInGrpCmd;
    UINT16 BatchCmdNumAccumedInGrpCmd = *pBatchCmdNumAccumedInGrpCmd; //FirstGrpCmd update BatchCmdNum
    UINT32 Rval = OK;
    const UINT32 *pU32Val;
    UINT32 U32Val;
    UINT32 *pGrpQAddr = NULL;
    cmd_vproc_prev_setup_t *pPrevSetup;
#if defined(UCODE_SUPPORT_EFFECT_PREV_BC_REARRANGE) || defined(UCODE_SUPPORT_EFFECT_PREV_ABC_REARRANGE)
    UINT16 PinIdx;
    CTX_VPROC_INFO_s VprocInfo = {0};
#else
    UINT8 IsDramMipiYuv;
#endif
    ULONG ULAddr;

    if (pSyncJob->PrevSizeUpdate == 1U) {
        /* Update JobStatus of Non-Master Job */
        DSP_SetU16Bit(&JobStatus, JobIdx);

        /* Copy into GrpQ one by one */
        HL_GetPointerToDspGroupCmdQ((UINT16)pSyncJob->JobId.YuvStrmId,
                                    ((UINT16)pSyncJob->JobId.SeqIdx - 1U),
                                    ((UINT16)pSyncJob->JobId.SubJobIdx - 1U),
                                    &pGrpQAddr);

        pU32Val = &pGrpQAddr[CMD_SIZE_IN_WORD*GrpQCmdNum];
        dsp_osal_typecast(&pPrevSetup, &pU32Val);

        /* Get output VprocPin of this effect channel */
#if defined(UCODE_SUPPORT_EFFECT_PREV_BC_REARRANGE) || defined(UCODE_SUPPORT_EFFECT_PREV_ABC_REARRANGE)
//FIXME, RACING?
        HL_GetVprocInfo(HL_MTX_OPT_ALL, pYuvStrmLayout->ChanCfg[0U].ViewZoneId, &VprocInfo);
        for (PinIdx=0U; PinIdx<DSP_VPROC_PIN_NUM; PinIdx++) {
            if (VprocInfo.PinUsage[PinIdx] > 0U) {
                if (1U == DSP_GetBit(VprocInfo.PinUsage[PinIdx], YuvStrmIdx, 1U)) {
                    PinId = PinIdx;
                    break;
                }
            }
        }

#else
        IsDramMipiYuv = (UINT8)HL_GetDramMipiYuvEnable();
        (void)HL_GetEffectChannelVprocPin(pYuvStrmInfo->Purpose,
                                          pYuvStrmInfo->DestVout,
                                          pYuvStrmInfo->DestEnc,
                                          &PinId,
                                          IsDramMipiYuv);
#endif
        if (PinId == DSP_VPROC_PIN_MAIN) {
            AmbaLL_LogUInt5("[%d]Layout change for Main?", __LINE__, 0U, 0, 0, 0);
        } else if (PinId == DSP_VPROC_PIN_NUM) {
            AmbaLL_LogUInt5("StrmPp route fail", 0U, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0006;
        } else {
            PrevId = (UINT8)HL_CtxVprocPinDspPrevMap[PinId];
        }
        if (Rval != OK) {
            AmbaLL_LogUInt5("HL_FillSyncJobIntoBatchLayoutNoneMaster strm:%u job:%u",
                    YuvStrmIdx, JobIdx, 0U, 0U, 0U);
        }

        if (PinId != DSP_VPROC_PIN_MAIN) {
            cmd_vproc_fov_grp_cmd_t *pVprocGrpCmd = NULL;

            (void)dsp_osal_memset(pPrevSetup, 0, sizeof(cmd_vproc_prev_setup_t));
            (void)HL_FillVideoPreviewSetup(pPrevSetup,
                                           pSyncJob->ViewZoneIdx,
                                           PrevId,
                                           &CmdBypass,
                                           pSyncJobSetLayout);
            /* PrintOut cmd for debug */
            (void)AmbaHL_CmdVprocPrevSetup(AMBA_DSP_CMD_NOP, pPrevSetup);
            GrpQCmdNum++;

            /* VprocGrpCmd */
            /* 2nd Processed JobType in this condition, Check BatchSlot */
            if (BatchCmdNumAccumedInGrpCmd == 0U) {
                pU32Val = &pBatchQAddr[CMD_SIZE_IN_WORD*BatchCmdNum];
                PreBatchCmdNumInGrpCmd = BatchCmdNum;
            } else {
                pU32Val = &pBatchQAddr[CMD_SIZE_IN_WORD*PreBatchCmdNumInGrpCmd];
            }
            dsp_osal_typecast(&pVprocGrpCmd, &pU32Val);
            (void)dsp_osal_memset(pVprocGrpCmd, 0, sizeof(DSP_CMD_s));
            pVprocGrpCmd->cmd_code = CMD_VPROC_GRP_CMD;
            pVprocGrpCmd->channel_id = (UINT8)pSyncJob->ViewZoneIdx;
            pVprocGrpCmd->is_sent_from_sys = (UINT8)1U; //be TRUE all the time
            pVprocGrpCmd->is_vout_cmds = 0U;
            pVprocGrpCmd->vout_id = 0U; //DONT CARE in this case
            (void)dsp_osal_memcpy(&U32Val, &pSyncJob->JobId, sizeof(UINT32));
            pVprocGrpCmd->grp_cmd_buf_id = U32Val;
            dsp_osal_typecast(&ULAddr, &pGrpQAddr);
            (void)dsp_osal_virt2cli(ULAddr, &pVprocGrpCmd->grp_cmd_buf_addr);
            pVprocGrpCmd->num_cmds_in_buf = (UINT32)GrpQCmdNum;
            if (BatchCmdNumAccumedInGrpCmd == 0U) {
                BatchCmdNumAccumedInGrpCmd = 1U;
                BatchCmdNum++;
            }
            /* PrintOut cmd for debug */
            (void)AmbaHL_CmdVprocGrpCmd(AMBA_DSP_CMD_NOP, pVprocGrpCmd);
#ifdef DEBUG_SYNC_CMD
            AmbaLL_LogUInt5("LAYOUT [%d] << Grp[0x%X] BchNum[%d]", __LINE__, pVprocGrpCmd->grp_cmd_buf_id, BatchCmdNum, 0, 0);
#endif
        }
    }
    *pJobStatus = JobStatus;
    *pBatchCmdNum = BatchCmdNum;
    *pGrpQCmdNum = GrpQCmdNum;
    *pPreBatchCmdNumInGrpCmd = PreBatchCmdNumInGrpCmd;
    *pBatchCmdNumAccumedInGrpCmd = BatchCmdNumAccumedInGrpCmd;
    return Rval;
}

static inline UINT32 HL_FillSyncJobIntoBatchLayout(const DSP_EFCT_SYNC_JOB_s *pSyncJob,
                                                   const UINT16 JobIdx,
                                                   const UINT16 ViewZoneId,
                                                   const UINT16 YuvStrmIdx,
                                                   CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout,
                                                   const CTX_YUV_STRM_LAYOUT_s *pSyncJobSetLayout,
                                                   const UINT32 *pBatchQAddr,
                                                   UINT16 *pJobStatus,
                                                   UINT16 *pGrpQCmdNum,
                                                   UINT16 *pBatchCmdNum,
                                                   UINT16 *pPreBatchCmdNumInGrpCmd,
                                                   UINT16 *pBatchCmdNumAccumedInGrpCmd)
{
    UINT8 PrevId = DSP_VPROC_PREV_B, CmdBypass = 0U;
    UINT16 PinId = DSP_VPROC_PIN_PREVC;
    UINT16 JobStatus = *pJobStatus;
    UINT16 BatchCmdNum = *pBatchCmdNum;
    UINT16 PreBatchCmdNumInGrpCmd = *pPreBatchCmdNumInGrpCmd;
    UINT16 BatchCmdNumAccumedInGrpCmd = *pBatchCmdNumAccumedInGrpCmd; //FirstGrpCmd update BatchCmdNum
    UINT32 Rval = OK;
    const UINT32 *pU32Val;
    cmd_vproc_prev_setup_t *pPrevSetup;
#if defined(UCODE_SUPPORT_EFFECT_PREV_BC_REARRANGE) || defined(UCODE_SUPPORT_EFFECT_PREV_ABC_REARRANGE)
    UINT16 PinIdx;
    CTX_VPROC_INFO_s VprocInfo = {0};
#else
    UINT8 IsDramMipiYuv;
#endif

    if (ViewZoneId == pSyncJob->ViewZoneIdx) { /* Put into BatchQ */
        cmd_vproc_multi_stream_pp_t *pPpStrmCfg;

        /* PpStrm */
        pU32Val = &pBatchQAddr[CMD_SIZE_IN_WORD*BatchCmdNum];
        dsp_osal_typecast(&pPpStrmCfg, &pU32Val);
        (void)dsp_osal_memset(pPpStrmCfg, 0, sizeof(DSP_CMD_s));
        (void)HL_FillPpStrmCfg(YuvStrmIdx, pPpStrmCfg, pYuvStrmLayout);
        /* PrintOut cmd for debug */
        (void)AmbaHL_CmdVprocMultiStrmPpCmd(AMBA_DSP_CMD_NOP, pPpStrmCfg);
        DSP_SetU16Bit(&JobStatus, EFCT_SYNC_JOB_STATUS_BATCH_BIT);
        BatchCmdNum++;

//FIXME, sanity check for Main
        /* PrevSetup only for preview resampler output */
        if (pSyncJob->PrevSizeUpdate == 1U) {
            pU32Val = &pBatchQAddr[CMD_SIZE_IN_WORD*BatchCmdNum];
            dsp_osal_typecast(&pPrevSetup, &pU32Val);
            (void)dsp_osal_memset(pPrevSetup, 0, sizeof(DSP_CMD_s));

            /* Get output VprocPin of this effect channel */
#if defined(UCODE_SUPPORT_EFFECT_PREV_BC_REARRANGE) || defined(UCODE_SUPPORT_EFFECT_PREV_ABC_REARRANGE)
//FIXME, RACING?
            HL_GetVprocInfo(HL_MTX_OPT_ALL, pYuvStrmLayout->ChanCfg[0U].ViewZoneId, &VprocInfo);
            for (PinIdx=0U; PinIdx<DSP_VPROC_PIN_NUM; PinIdx++) {
                if (VprocInfo.PinUsage[PinIdx] > 0U) {
                    if (1U == DSP_GetBit(VprocInfo.PinUsage[PinIdx], YuvStrmIdx, 1U)) {
                        PinId = PinIdx;
                        break;
                    }
                }
            }
#else
            {
                CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;

                HL_GetYuvStrmInfoLock(YuvStrmIdx, &YuvStrmInfo);
                IsDramMipiYuv = (UINT8)HL_GetDramMipiYuvEnable();
                (void)HL_GetEffectChannelVprocPin(pYuvStrmInfo->Purpose, pYuvStrmInfo->DestVout,
                                                  pYuvStrmInfo->DestEnc,
                                                  &PinId,
                                                  IsDramMipiYuv);
                HL_GetYuvStrmInfoUnLock(YuvStrmInfo);
            }
#endif
            if (PinId == DSP_VPROC_PIN_MAIN) {
                AmbaLL_LogUInt5("[%d]Layout change for Main?", __LINE__, 0U, 0, 0, 0);
            } else if (PinId == DSP_VPROC_PIN_NUM) {
                AmbaLL_LogUInt5("StrmPp route fail", 0U, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0006;
            } else {
                PrevId = (UINT8)HL_CtxVprocPinDspPrevMap[PinId];
            }

            if (Rval != OK) {
                AmbaLL_LogUInt5("HL_FillSyncJobIntoBatchLayout vz:%u strm:%u job:%u",
                        ViewZoneId, YuvStrmIdx, JobIdx, 0U, 0U);
            }

            if (PinId != DSP_VPROC_PIN_MAIN) {
                (void)dsp_osal_memset(pPrevSetup, 0, sizeof(cmd_vproc_prev_setup_t));
                (void)HL_FillVideoPreviewSetup(pPrevSetup,
                                               pSyncJob->ViewZoneIdx,
                                               PrevId,
                                               &CmdBypass,
                                               pSyncJobSetLayout);
                /* PrintOut cmd for debug */
                (void)AmbaHL_CmdVprocPrevSetup(AMBA_DSP_CMD_NOP, pPrevSetup);
                DSP_SetU16Bit(&JobStatus, EFCT_SYNC_JOB_STATUS_BATCH_BIT);
                BatchCmdNum++;
            }
        }

#ifdef DEBUG_SYNC_CMD
        AmbaLL_LogUInt5("LAYOUT [%d] << BchNum[%d] PrevSizeUpdate[%d]", __LINE__, BatchCmdNum, pSyncJob->PrevSizeUpdate, 0, 0);
#endif
    } else {
        Rval = HL_FillSyncJobIntoBatchLayoutNoneMaster(pSyncJob,
                                                       JobIdx,
                                                       YuvStrmIdx,
                                                       pYuvStrmLayout,
                                                       pSyncJobSetLayout,
                                                       pBatchQAddr,
                                                       &JobStatus,
                                                       pGrpQCmdNum,
                                                       &BatchCmdNum,
                                                       &PreBatchCmdNumInGrpCmd,
                                                       &BatchCmdNumAccumedInGrpCmd);
    }

    *pJobStatus = JobStatus;
    *pBatchCmdNum = BatchCmdNum;
    *pPreBatchCmdNumInGrpCmd = PreBatchCmdNumInGrpCmd;
    *pBatchCmdNumAccumedInGrpCmd = BatchCmdNumAccumedInGrpCmd;

    return Rval;
}

UINT32 HL_FillSyncJobIntoBatch(UINT16 YuvStrmIdx,
                               CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout,
                               UINT16 ViewZoneId,
                               UINT16 LastViewZoneId,
                               CTX_EFCT_SYNC_JOB_SET_s *pSyncJobSet,
                               const CTX_YUV_STRM_LAYOUT_s *pSyncJobSetLayout,
                               UINT32 *pBatchQAddr,
                               UINT16 BatchCmdIdx)
{
    UINT32 Rval = OK;
    UINT16 JobIdx;
    UINT16 JobStatus, JobType;
    UINT16 BatchCmdNum, GrpQCmdNum;
    UINT16 PreBatchCmdNumInGrpCmd = 0U;
    UINT16 BatchCmdNumAccumedInGrpCmd; //FirstGrpCmd update BatchCmdNum
    const DSP_EFCT_SYNC_JOB_s *pSyncJob;

    BatchCmdNum = BATCH_CMD_START_NUM + BatchCmdIdx; //FirstSlot is reserved for IsoCfgUpdate

    /* General rules:
     *  We use MasterViewZone's (the one with minimal Id) batch command set for synchronization, this command set contains
     *  (1) Warp/preview commands relating to MasterViewZone
     *  (2) Multi-stream-pp command
     *  (3) Several GroupCmds for other ViewZones, each GroupCmdSet contains warp/preview DSP commands relating to each ViewZone
     *  (4) Vout commands are placed in standalone GroupCmdSet of the last ViewZone with is_vout_cmds flag enabled
     *  */
    for (JobIdx=0U; (JobIdx<pSyncJobSet->JobNum) && (pBatchQAddr != NULL); JobIdx++) {
        pSyncJob = &pSyncJobSet->Job[JobIdx];
        JobStatus = pSyncJobSet->JobStatus;
        JobType = (UINT16)pSyncJob->JobId.JobTypeBit;

        GrpQCmdNum = 0U;
        BatchCmdNumAccumedInGrpCmd = 0U;

        if (1U == DSP_GetU16Bit(JobType, EFCT_SYNC_JOB_BIT_DISPLAY, 1U)) {
            HL_FillSyncJobIntoBatchDisplay(pSyncJob,
                                           JobIdx,
                                           LastViewZoneId,
                                           pSyncJobSet,
                                           pBatchQAddr,
                                           &JobStatus,
                                           &GrpQCmdNum,
                                           &BatchCmdNum,
                                           &PreBatchCmdNumInGrpCmd,
                                           &BatchCmdNumAccumedInGrpCmd);
        } else {
            if (1U == DSP_GetU16Bit(JobType, EFCT_SYNC_JOB_BIT_GEO, 1U)) {
                HL_FillSyncJobIntoBatchGeo(pSyncJob,
                                           JobIdx,
                                           ViewZoneId,
                                           pBatchQAddr,
                                           &JobStatus,
                                           &GrpQCmdNum,
                                           &BatchCmdNum,
                                           &PreBatchCmdNumInGrpCmd,
                                           &BatchCmdNumAccumedInGrpCmd);
            }

            if (1U == DSP_GetU16Bit(JobType, EFCT_SYNC_JOB_BIT_LAYOUT, 1U)) {
                Rval = HL_FillSyncJobIntoBatchLayout(pSyncJob,
                                                     JobIdx,
                                                     ViewZoneId,
                                                     YuvStrmIdx,
                                                     pYuvStrmLayout,
                                                     pSyncJobSetLayout,
                                                     pBatchQAddr,
                                                     &JobStatus,
                                                     &GrpQCmdNum,
                                                     &BatchCmdNum,
                                                     &PreBatchCmdNumInGrpCmd,
                                                     &BatchCmdNumAccumedInGrpCmd);
            }
        }
        pSyncJobSet->JobStatus = JobStatus;
    }

    return Rval;
}

static UINT32 FillOsdSetup(UINT8 VoutIdx, const AMBA_DSP_VOUT_OSD_BUF_CONFIG_s *pOsdBufCfg, cmd_vout_osd_setup_t *pOsdSetup)
{
    static const UINT8 DspOsdDataFmtMap[OSD_FORMAT_NUM] =
    {
        [OSD_8BIT_CLUT_MODE]    = DSP_OSD_MODE_VYU565, // DONTCARE
        [OSD_16BIT_VYU_RGB_565] = DSP_OSD_MODE_VYU565, // 0 for yuv
        [OSD_16BIT_UYV_BGR_565] = DSP_OSD_MODE_UYV565,
        [OSD_16BIT_AYUV_4444]   = DSP_OSD_MODE_AYUV4444,
        [OSD_16BIT_RGBA_4444]   = DSP_OSD_MODE_RGBA4444,
        [OSD_16BIT_BGRA_4444]   = DSP_OSD_MODE_BGRA4444,
        [OSD_16BIT_ABGR_4444]   = DSP_OSD_MODE_ABGR4444,
        [OSD_16BIT_ARGB_4444]   = DSP_OSD_MODE_ARGB4444,
        [OSD_16BIT_AYUV_1555]   = DSP_OSD_MODE_AYUV1555,
        [OSD_16BIT_YUV_1555]    = DSP_OSD_MODE_YUV555,
        [OSD_16BIT_RGBA_5551]   = DSP_OSD_MODE_RGBA5551,
        [OSD_16BIT_BGRA_5551]   = DSP_OSD_MODE_BGRA5551,
        [OSD_16BIT_ABGR_1555]   = DSP_OSD_MODE_ABGR1555,
        [OSD_16BIT_ARGB_1555]   = DSP_OSD_MODE_ARGB1555,
        [OSD_32BIT_AYUV_8888]   = DSP_OSD_MODE_AYUV8888,
        [OSD_32BIT_RGBA_8888]   = DSP_OSD_MODE_RGBA8888,
        [OSD_32BIT_BGRA_8888]   = DSP_OSD_MODE_BGRA8888,
        [OSD_32BIT_ABGR_8888]   = DSP_OSD_MODE_ABGR8888,
        [OSD_32BIT_ARGB_8888]   = DSP_OSD_MODE_ARGB8888
    };

    UINT32 Rval = OK;

    pOsdSetup->vout_id = VoutIdx;
    pOsdSetup->flip = 0U; //default No flip

    (void)dsp_osal_virt2cli(pOsdBufCfg->BaseAddr, &pOsdSetup->osd_buf_dram_addr);
    pOsdSetup->osd_buf_pitch = pOsdBufCfg->Pitch;
    pOsdSetup->osd_buf_repeat_field = pOsdBufCfg->FieldRepeat;

    /* Window */
    pOsdSetup->rescaler_input_width = pOsdBufCfg->InputWidth;
    pOsdSetup->rescaler_input_height = pOsdBufCfg->InputHeight;
    pOsdSetup->win_width = pOsdBufCfg->Window.Width;
    pOsdSetup->win_height = pOsdBufCfg->Window.Height;
    pOsdSetup->win_offset_x = pOsdBufCfg->Window.OffsetX;
    pOsdSetup->win_offset_y = pOsdBufCfg->Window.OffsetY;
    if ((pOsdBufCfg->InputWidth == pOsdBufCfg->Window.Width) &&
        (pOsdBufCfg->InputHeight == pOsdBufCfg->Window.Height)) {
        pOsdSetup->rescaler_en = 0U;
    } else {
        pOsdSetup->rescaler_en = 1U;
    }

    pOsdSetup->premultiplied = pOsdBufCfg->PremultipliedEnable;
    pOsdSetup->global_blend = (pOsdBufCfg->GlobalBlendEnable == 0U)? 255U: pOsdBufCfg->GlobalBlendEnable;
    pOsdSetup->osd_swap_bytes = pOsdBufCfg->SwapByteEnable;

    /* transparent */
    pOsdSetup->osd_transparent_color_en = pOsdBufCfg->TransparentColorEnable;
    pOsdSetup->osd_transparent_color = pOsdBufCfg->TransparentColor;

    /* Data format */
    if (pOsdBufCfg->DataFormat == OSD_8BIT_CLUT_MODE) {
        pOsdSetup->src = DSP_OSD_SRC_MAPPED_IN;
    } else if (pOsdBufCfg->DataFormat >= OSD_32BIT_AYUV_8888) {
        pOsdSetup->src = DSP_OSD_SRC_DIRECT_IN_32;
        pOsdSetup->osd_direct_mode = DspOsdDataFmtMap[pOsdBufCfg->DataFormat];
    } else {
        pOsdSetup->src = DSP_OSD_SRC_DIRECT_IN_16;
        pOsdSetup->osd_direct_mode = DspOsdDataFmtMap[pOsdBufCfg->DataFormat];
    }

    /* ucode checks the content of osd_buf_info_dram_addr every Vout sync,
     * In previous effect-sync implementation, we update the content of this osd info buffer then put osd_setup into group-command,
     * the delay between osd buffer changed and group-command processed leads to OSD offsync.
     * Thus we disable this term and only use osd_buf_dram_addr for OSD buffer update.
     */
    pOsdSetup->osd_buf_info_dram_addr = 0U;

    return Rval;
}
UINT32 HL_FillOsdSetup(UINT8 VoutIdx, const AMBA_DSP_VOUT_OSD_BUF_CONFIG_s *pOsdBufCfg, cmd_vout_osd_setup_t *pOsdSetup)
{
    UINT32 Rval;
    UINT32 CrcHL, CrcSimilar;
    const UINT32 *pU32 = NULL;
    UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;
#ifdef CONFIG_ENABLE_DSP_MONITOR
    AMBA_DSP_MONITOR_CRC_CMPR_s CrcCmpr;
#endif

    Rval = FillOsdSetup(VoutIdx, pOsdBufCfg, pOsdSetup);
    if (pIsSimilarEnabled != NULL) {
        if (1U == IsSimilarEnabled()) {
            UINT32 SIM_Rval;
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_vout_osd_setup_t *SimOsdSetup = SIM_DefCtxCmdBufPtrVoutOsd;

            dsp_osal_typecast(&pU32, &pOsdSetup);
            CrcHL = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_vout_osd_setup_t), 0U);

            SIM_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&SimOsdSetup, &CmdBufferAddr);
            SIM_Rval = SIM_FillOsdSetup(VoutIdx, pOsdBufCfg, SimOsdSetup);
            if (SIM_Rval == OK) {
                dsp_osal_typecast(&pU32, &SimOsdSetup);
                CrcSimilar = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_vout_osd_setup_t), 0U);
#ifdef CONFIG_ENABLE_DSP_MONITOR
                CrcCmpr.ModuleId = AMBA_SSP_PRINT_MODULE_ID;
                CrcCmpr.Crc0 = CrcHL;
                CrcCmpr.Crc1 = CrcSimilar;
                CrcCmpr.UUID = __LINE__;
                SIM_Rval = dsp_mon_crc_cmpr(&CrcCmpr);
                if (SIM_Rval != OK) {
                    AmbaLL_LogUInt5("dsp_mon_crc_cmpr return 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
                }
#else
                if (CrcSimilar != CrcHL) {
                    AmbaLL_LogUInt5("[ERR] Similar CRC mismatch @ %d HL 0x%x Similar 0x%x", __LINE__, CrcHL, CrcSimilar, 0U, 0U);
                }
#endif
            } else {
                AmbaLL_LogUInt5("[ERR] Similar function returned NG 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
            }
            SIM_RelCmdBuffer(CmdBufferId);
        }
    }

    return Rval;
}

//#define DEBUG_BINDING_EFFECT
//#define DEBUG_BINDING_MAIN
//#define DEBUG_BINDING_VOUT
//#define DEBUG_BINDING_ENC

#if defined(UCODE_SUPPORT_EFFECT_PREV_BC_REARRANGE) || defined(UCODE_SUPPORT_EFFECT_PREV_ABC_REARRANGE)

#ifdef UCODE_SUPPORT_EFFECT_PREV_ABC_REARRANGE
static inline void HL_ArngBindEffPrevAPreBindUpScaleChk(const AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *ChanInfo,
                                                        AMBA_DSP_WINDOW_s *pUpscaleChkWinOut)
{
    if (HL_GET_ROTATE(ChanInfo->RotateFlip) == DSP_ROTATE_90_DEGREE) {
        pUpscaleChkWinOut->Width = ChanInfo->Window.Height;
        pUpscaleChkWinOut->Height = ChanInfo->Window.Width;
    } else {
        pUpscaleChkWinOut->Width = ChanInfo->Window.Width;
        pUpscaleChkWinOut->Height = ChanInfo->Window.Height;
    }
}

static inline void HL_CalPinOutWindow(const AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *pChanWin,
                                      AMBA_DSP_WINDOW_s *pPinOutWin)
{
    pPinOutWin->Width = (HL_IS_ROTATE(pChanWin->RotateFlip) == 1U)? pChanWin->Window.Height: pChanWin->Window.Width;
    pPinOutWin->Height = (HL_IS_ROTATE(pChanWin->RotateFlip) == 1U)? pChanWin->Window.Width: pChanWin->Window.Height;
}

static inline UINT8 HL_GetBindPrevWindowCheck(const UINT16 MaxChanWinWidth, const UINT16 WinWindth,
                                              const UINT16 MaxPinOutWidth, const UINT16 TileNum)
{
    UINT8 WindowCheck;

    if (MaxChanWinWidth > 0U) {
        WindowCheck = ((MaxPinOutWidth*TileNum) >= MaxChanWinWidth)? (UINT8)1U: (UINT8)0U;
    } else {
        WindowCheck = ((MaxPinOutWidth*TileNum) >= WinWindth)? (UINT8)1U: (UINT8)0U;
    }

    return WindowCheck;
}

static inline void HL_ArngBindEffPrevAPreBindChk(UINT8 *pPrevFilterConflict,
                                                 const UINT16 YuvStrmId,
                                                 const UINT16 YuvStrmNum)
{
    UINT16 i;
    UINT8 IsNonVoutPurpose;
    UINT8 IsEffectStrm;
    UINT16 ViewZoneId;
    UINT16 YuvChIdx;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_VPROC_INFO_s VprocInfo = {0};
    UINT32 PinUsage[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT32 ChanLinked;
    const AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *ChanInfo = NULL;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    const UINT8 PinId = DSP_VPROC_PIN_PREVA;
    UINT16 MaxPinOutWidth = 0U;
    UINT16 TileNum = 0U;
    UINT8 WindowCheck;
    UINT8 PinCheck;
    UINT8 UpscaleChkPass;
    UINT16 FoundPin;
    AMBA_DSP_WINDOW_s UpscaleChkWinOut = {0};
#ifdef SUPPORT_RESAMPLING_CHECK
    AMBA_DSP_WINDOW_s PinOutWin = {0};
#endif

    for (i = 0U; i < YuvStrmNum; i++) {
        HL_GetYuvStrmInfoPtr(i, &YuvStrmInfo);
        IsNonVoutPurpose = (0U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U))? (UINT8)1U: (UINT8)0U;
        IsEffectStrm = (1U == HL_GetEffectEnableOnYuvStrm(i))? (UINT8)1U: (UINT8)0U;

        if ((i == YuvStrmId) ||
            (IsNonVoutPurpose == 0U) ||
            (IsEffectStrm == 0U)) {
            continue;
        }

        /* A TestRun to make sure all Chan pass PinCheck/WinCheck */
        FoundPin = 0U;
        (void)dsp_osal_memset(&PinUsage[0U], 0, sizeof(UINT32)*AMBA_DSP_MAX_VIEWZONE_NUM);
        for (ViewZoneId = 0U; ViewZoneId < AMBA_DSP_MAX_VIEWZONE_NUM; ViewZoneId++) {
            HL_GetVprocInfo(HL_MTX_OPT_ALL, ViewZoneId, &VprocInfo);
            PinUsage[ViewZoneId] = VprocInfo.PinUsage[PinId];
        }

        ChanLinked = YuvStrmInfo->Layout.ChanLinked;
        for (YuvChIdx = 0U; YuvChIdx < YuvStrmInfo->Layout.NumChan; YuvChIdx++) {
            if (1U == DSP_GetBit(ChanLinked, YuvChIdx, 1U)) {
                continue;
            }

            ChanInfo = (const AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *)&YuvStrmInfo->Layout.ChanCfg[YuvChIdx];
            HL_GetViewZoneInfoPtr(ChanInfo->ViewZoneId, &ViewZoneInfo);
#ifdef SUPPORT_RESAMPLING_CHECK
            HL_CalPinOutWindow(ChanInfo, &PinOutWin);
            // Check the MAX(preview's width) >= yuv's width
            (void)HL_CalcVideoTile(PinOutWin.Width, SEC2_MAX_OUT_WIDTH, &TileNum, TILE_OVERLAP_WIDTH);
            (void)HL_GetVprocPinMaxWidth(PinId, &ViewZoneInfo->Main, &PinOutWin, &MaxPinOutWidth);
#else
            MaxPinOutWidth = (UINT16)(HL_MaxVprocPinOutputWidthMap[PinId]);
#endif
            WindowCheck = HL_GetBindPrevWindowCheck(YuvStrmInfo->MaxChanWin[ChanInfo->ViewZoneId].Width,
                                                    PinOutWin.Width,
                                                    MaxPinOutWidth,
                                                    TileNum);

            /* Upscale capability check */
            HL_ArngBindEffPrevAPreBindUpScaleChk(ChanInfo, &UpscaleChkWinOut);

            (void)HL_VprocUpscaleCheck(PinId,
                                       &ChanInfo->ROI,
                                       &UpscaleChkWinOut,
                                       &UpscaleChkPass);
            /* reset WinChk if Upscalechk failed */
            WindowCheck = (UpscaleChkPass == 0U)? (UINT8)0U: WindowCheck;

            PinCheck = (0U == PinUsage[ChanInfo->ViewZoneId])? (UINT8)1U: (UINT8)0U;

            //all Chan shall pass the criteria
            if ((WindowCheck == 1U) && (PinCheck == 1U)) {
                //found and occupy it
                DSP_SetBit(&ChanLinked, YuvChIdx);
                DSP_SetBit(&PinUsage[ChanInfo->ViewZoneId], (UINT16)i);
                FoundPin++;
            } else {
                break;
            }
        }

        if (FoundPin < YuvStrmInfo->Layout.NumChan) {
            continue;
        } else {
            /* PREVIEW FILTER CONFLICT! */
            *pPrevFilterConflict = (UINT8)1U;
            break;
        }
    }
}
#endif

static inline void HL_ArngBindEffPrevBCPreBindChk(UINT8 *pNoOtherVout,
                                                  UINT8 *pPrevFilterConflict,
                                                  const UINT16 DestVout,
                                                  const UINT16 YuvStrmId,
                                                  const UINT16 YuvStrmNum,
                                                  const UINT16 PinIdx)
{
    UINT8 IsTempVoutPurpose;
    UINT8 PrevFilterConflict = 1U, NoOtherVout = 1U;
    UINT8 PrevAFilterConflict = 0U;
    UINT16 TmpYuvIdx;
    CTX_YUV_STRM_INFO_s *TmpYuvStrmInfo = NULL;

    for (TmpYuvIdx=0U; TmpYuvIdx<YuvStrmNum; TmpYuvIdx++) {
        HL_GetYuvStrmInfoPtr(TmpYuvIdx, &TmpYuvStrmInfo);
        IsTempVoutPurpose = (1U == DSP_GetU16Bit(TmpYuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U))? 1U: 0U;
        if ((TmpYuvIdx == YuvStrmId) || (IsTempVoutPurpose == 0U)) {
            continue;
        }

        NoOtherVout = 0U;

        if (0U == HL_GetEffectEnableOnYuvStrm(TmpYuvIdx)) {
            if (1U == DSP_GetU16Bit(DestVout, 0U/*LCD*/, 1U)) {
                /* If CandIdx == DSP_VPROC_PIN_PREVB, NoEfct can't have VOUT1 */
                if (PinIdx == DSP_VPROC_PIN_PREVB) {
                    if (0U == DSP_GetU16Bit(TmpYuvStrmInfo->DestVout, 1U/*TV*/, 1U)) {
                        PrevFilterConflict = 0U;
                    }
                } else {
                    PrevFilterConflict = 0U;
                }
            } else if (1U == DSP_GetU16Bit(DestVout, 1U/*TV*/, 1U)) {
                /* If CandIdx == DSP_VPROC_PIN_PREVC, NoEfct can't have VOUT0 */
                if (PinIdx == DSP_VPROC_PIN_PREVC) {
                    if (0U == DSP_GetU16Bit(TmpYuvStrmInfo->DestVout, 0U/*LCD*/, 1U)) {
                        PrevFilterConflict = 0U;
                    }
                } else {
                    PrevFilterConflict = 0U;
                }
            } else {
                AmbaLL_LogUInt5("Invalid vout[%d %d] dest", DestVout, TmpYuvStrmInfo->DestVout, 0U, 0U, 0U);
            }
        } else {
            PrevFilterConflict = 0U;
        }
        if (PrevFilterConflict == 0U) {
            break;
        }
    }

#ifdef UCODE_SUPPORT_EFFECT_PREV_ABC_REARRANGE
    /*
     * PrevA will be used for 2ndStrm encode as well
     * here we shall check any potential Efct2ndStrm exist and met conflict or not
     */
    if (PinIdx == DSP_VPROC_PIN_PREVA) {
        HL_ArngBindEffPrevAPreBindChk(&PrevAFilterConflict,
                                      YuvStrmId,
                                      YuvStrmNum);
    }
#endif

#ifdef UCODE_SUPPORT_EFFECT_PREV_ABC_REARRANGE
    *pPrevFilterConflict = (UINT8)((UINT16)PrevFilterConflict | (UINT16)PrevAFilterConflict);
#else
    *pPrevFilterConflict = PrevFilterConflict;
#endif

    *pNoOtherVout = NoOtherVout;
}

static inline UINT32 HL_ArngBindEffPrevBCPreBind(const UINT16 YuvIdx,
                                                 const UINT16 IsVoutPurpose,
                                                 const CTX_YUV_STRM_INFO_s *pYuvStrmInfo,
                                                 const UINT16 PinIdx,
                                                 const UINT16 YuvStrmNum,
                                                 UINT16 *pFoundPin)
{
    UINT8 PrevFilterConflict, NoOtherVout;
    UINT8 WindowCheck, PinCheck, UpscaleChkPass;
    UINT16 VzIdx, YuvChIdx, FoundPin = *pFoundPin;
    UINT16 MaxPinOutWidth = 0U, TileNum = 0U;
    UINT32 Rval = OK;
    UINT32 ChanLinked;
    UINT32 PinUsage[AMBA_DSP_MAX_VIEWZONE_NUM][DSP_VPROC_PIN_NUM];
    CTX_VPROC_INFO_s VprocInfo = {0};
    AMBA_DSP_WINDOW_s UpscaleChkWinOut = {0};
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    const AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *ChanInfo = NULL;
#ifdef SUPPORT_RESAMPLING_CHECK
    AMBA_DSP_WINDOW_s PinOutWin = {0};
#endif

    /* A TestRun to make sure all Chan pass PinCheck/WinCheck */
    for (VzIdx=0U; VzIdx < AMBA_DSP_MAX_VIEWZONE_NUM; VzIdx++) {
        HL_GetVprocInfo(HL_MTX_OPT_ALL, VzIdx, &VprocInfo);
        (void)dsp_osal_memcpy(&PinUsage[VzIdx][0U],
                              &VprocInfo.PinUsage[0U],
                              sizeof(UINT32)*DSP_VPROC_PIN_NUM);
    }

    ChanLinked = pYuvStrmInfo->Layout.ChanLinked;
    for (YuvChIdx = 0U; YuvChIdx < pYuvStrmInfo->Layout.NumChan; YuvChIdx++) {
        if (1U == DSP_GetBit(ChanLinked, YuvChIdx, 1U)) {
            AmbaLL_LogUInt5("[%d]No matching in Effect: YuvStrm[%d]Ch[%d] has been linked[%d]",
                            __LINE__, YuvIdx, YuvChIdx, ChanLinked, 0U);
            Rval = DSP_ERR_0006;
            continue;
        }

        ChanInfo = (const AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *)&pYuvStrmInfo->Layout.ChanCfg[YuvChIdx];
        HL_GetViewZoneInfoPtr(ChanInfo->ViewZoneId, &ViewZoneInfo);
        if (PinIdx == DSP_VPROC_PIN_MAIN) {
            // Check main w/h is the same with yuv's w/h
            UINT32 Cmp;

            (void)dsp_osal_memcmp(&ViewZoneInfo->PinWindow[PinIdx], &ChanInfo->ROI, sizeof(AMBA_DSP_WINDOW_s), &Cmp);
            WindowCheck = (0U == Cmp)? 1U: 0U;
        } else {
            // Check the MAX(preview's width) >= yuv's width
#ifdef SUPPORT_RESAMPLING_CHECK
            HL_CalPinOutWindow(ChanInfo, &PinOutWin);
            (void)HL_CalcVideoTile(PinOutWin.Width, SEC2_MAX_OUT_WIDTH, &TileNum, TILE_OVERLAP_WIDTH);
            (void)HL_GetVprocPinMaxWidth(PinIdx, &ViewZoneInfo->Main, &PinOutWin, &MaxPinOutWidth);
#else
            MaxPinOutWidth = (UINT16)(HL_MaxVprocPinOutputWidthMap[CandPinIdx[CandIdx]]);
#endif
            WindowCheck = HL_GetBindPrevWindowCheck(pYuvStrmInfo->MaxChanWin[ChanInfo->ViewZoneId].Width,
                                                    ChanInfo->Window.Width,
                                                    MaxPinOutWidth,
                                                    TileNum);

            /* Upscale capability check */
            if (HL_GET_ROTATE(ChanInfo->RotateFlip) == DSP_ROTATE_90_DEGREE) {
                UpscaleChkWinOut.Width = ChanInfo->Window.Height;
                UpscaleChkWinOut.Height = ChanInfo->Window.Width;
            } else {
                UpscaleChkWinOut.Width = ChanInfo->Window.Width;
                UpscaleChkWinOut.Height = ChanInfo->Window.Height;
            }
            (void)HL_VprocUpscaleCheck(PinIdx,
                                       &ChanInfo->ROI,
                                       &UpscaleChkWinOut,
                                       &UpscaleChkPass);
            /* reset WinChk if Upscalechk failed */
            WindowCheck = (UpscaleChkPass == 0U)? 0U: WindowCheck;
        }

        PinCheck = (0U == PinUsage[ChanInfo->ViewZoneId][PinIdx])? 1U: 0U;

        //all Chan shall pass the criteria
        if ((WindowCheck == 1U) && (PinCheck == 1U)) {
            if (IsVoutPurpose > 0U) {
                PrevFilterConflict = 1U;
                NoOtherVout = 1U;

                /*
                 * Extra criteria for VOUT purpose
                 * Check NonEfctVout existence and it's potential PrevFilter
                 */
                HL_ArngBindEffPrevBCPreBindChk(&NoOtherVout,
                                               &PrevFilterConflict,
                                               pYuvStrmInfo->DestVout,
                                               YuvIdx,
                                               YuvStrmNum,
                                               PinIdx);
                //found and occupy it if there's no Vout/Prev conflict
                if ((PrevFilterConflict == 0U) || (NoOtherVout == 1U)) {
                    DSP_SetBit(&ChanLinked, YuvChIdx);
                    DSP_SetBit(&PinUsage[ChanInfo->ViewZoneId][PinIdx], (UINT16)YuvIdx);
                    FoundPin++;
                }
            } else {
                //found and occupy it
                DSP_SetBit(&ChanLinked, YuvChIdx);
                DSP_SetBit(&PinUsage[ChanInfo->ViewZoneId][PinIdx], (UINT16)YuvIdx);
                FoundPin++;
            }
        } else {
            break;
        }
    }

    *pFoundPin = FoundPin;

    return Rval;
}

static inline UINT32 HL_ArngBindEffPrevBCBind(const UINT16 YuvIdx,
                                              const UINT16 CandIdx,
                                              const UINT16 *CandPinIdx,
                                              CTX_YUV_STRM_INFO_s *pYuvStrmInfo,
                                              UINT32 *pStrmViewZoneOI)
{
    UINT8 WindowCheck, PinCheck, UpscaleChkPass;
    UINT16 Idx;
    UINT16 MaxPinOutWidth = 0U, TileNum = 0U;
    UINT32 Rval = OK;
    UINT32 StrmViewZoneOI = *pStrmViewZoneOI;
    CTX_VPROC_INFO_s VprocInfo = {0};
    AMBA_DSP_WINDOW_s UpscaleChkWinOut = {0};
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_YUV_STRM_INFO_s *TmpYuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    const AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *ChanInfo = NULL;
#ifdef SUPPORT_RESAMPLING_CHECK
    AMBA_DSP_WINDOW_s PinOutWin = {0};
#endif

    HL_GetResourcePtr(&Resource);

    for (Idx=0U; Idx<pYuvStrmInfo->Layout.NumChan; Idx++) {
        if (1U == DSP_GetBit(pYuvStrmInfo->Layout.ChanLinked, Idx, 1U)) {
            AmbaLL_LogUInt5("[%d]No matching in Effect: YuvStrm[%d]Ch[%d] has been linked[%d]",
                            __LINE__, YuvIdx, Idx, pYuvStrmInfo->Layout.ChanLinked, 0U);
            Rval = DSP_ERR_0006;
            continue;
        }

        ChanInfo = (const AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *)&pYuvStrmInfo->Layout.ChanCfg[Idx];
        HL_GetViewZoneInfoLock(ChanInfo->ViewZoneId, &ViewZoneInfo);
        if (CandPinIdx[CandIdx] == DSP_VPROC_PIN_MAIN) {
            // Check main w/h is identical to yuv's w/h
            UINT32 Cmp;

            (void)dsp_osal_memcmp(&ViewZoneInfo->PinWindow[CandPinIdx[CandIdx]],
                                  &ChanInfo->ROI, sizeof(AMBA_DSP_WINDOW_s),
                                  &Cmp);
            WindowCheck = (0U == Cmp)? 1U: 0U;
        } else {
            // Check the MAX(preview's width) >= yuv's width
#ifdef SUPPORT_RESAMPLING_CHECK
            HL_CalPinOutWindow(ChanInfo, &PinOutWin);
            (void)HL_CalcVideoTile(PinOutWin.Width, SEC2_MAX_OUT_WIDTH, &TileNum, TILE_OVERLAP_WIDTH);
            (void)HL_GetVprocPinMaxWidth(CandPinIdx[CandIdx], &ViewZoneInfo->Main, &PinOutWin, &MaxPinOutWidth);
#else
            MaxPinOutWidth = (UINT16)(HL_MaxVprocPinOutputWidthMap[CandPinIdx[CandIdx]]);
#endif
            WindowCheck = HL_GetBindPrevWindowCheck(pYuvStrmInfo->MaxChanWin[ChanInfo->ViewZoneId].Width,
                                                    PinOutWin.Width,
                                                    MaxPinOutWidth,
                                                    TileNum);

            /* Upscale capability check */
            if (HL_GET_ROTATE(ChanInfo->RotateFlip) == DSP_ROTATE_90_DEGREE) {
                UpscaleChkWinOut.Width = ChanInfo->Window.Height;
                UpscaleChkWinOut.Height = ChanInfo->Window.Width;
            } else {
                UpscaleChkWinOut.Width = ChanInfo->Window.Width;
                UpscaleChkWinOut.Height = ChanInfo->Window.Height;
            }
            (void)HL_VprocUpscaleCheck(CandPinIdx[CandIdx],
                                       &ChanInfo->ROI,
                                       &UpscaleChkWinOut,
                                       &UpscaleChkPass);
            /* reset WinChk if Upscalechk failed */
            WindowCheck = (UpscaleChkPass == 0U)? 0U: WindowCheck;
        }

        HL_GetVprocInfo(HL_MTX_OPT_GET, ChanInfo->ViewZoneId, &VprocInfo);
        PinCheck = (0U == VprocInfo.PinUsage[CandPinIdx[CandIdx]])? 1U: 0U;

        if ((WindowCheck == 1U) && (PinCheck == 1U)) {
            //found and occupy it
            DSP_SetBit(&pYuvStrmInfo->Layout.ChanLinked, Idx);
            HL_GetYuvStrmInfoLock(YuvIdx, &TmpYuvStrmInfo);
            TmpYuvStrmInfo->Layout.ChanLinked = pYuvStrmInfo->Layout.ChanLinked;
            HL_GetYuvStrmInfoUnLock(YuvIdx);
            DSP_SetBit(&VprocInfo.PinUsageOnStrm[CandPinIdx[CandIdx]][YuvIdx], (UINT16)ChanInfo->ViewZoneId);
            DSP_SetBit(&VprocInfo.PinUsage[CandPinIdx[CandIdx]], (UINT16)YuvIdx);
            VprocInfo.PinUsageNumOnStrm[CandPinIdx[CandIdx]][YuvIdx]++;
            DSP_SetBit(&StrmViewZoneOI, ChanInfo->ViewZoneId);

            if (CandPinIdx[CandIdx] < DSP_VPROC_PIN_MAIN) {
                if (HL_GET_ROTATE(ChanInfo->RotateFlip) == DSP_ROTATE_90_DEGREE) {
                    ViewZoneInfo->PinWindow[CandPinIdx[CandIdx]].Width = ChanInfo->Window.Height;
                    ViewZoneInfo->PinWindow[CandPinIdx[CandIdx]].Height = ChanInfo->Window.Width;
                } else {
                    ViewZoneInfo->PinWindow[CandPinIdx[CandIdx]].Width = ChanInfo->Window.Width;
                    ViewZoneInfo->PinWindow[CandPinIdx[CandIdx]].Height = ChanInfo->Window.Height;
                }
                /* Rotation is changeable after liveview start,
                 * so we expect user provide maximal size before rotation */
                ViewZoneInfo->PinMaxWindow[CandPinIdx[CandIdx]].Width = pYuvStrmInfo->MaxChanWin[ChanInfo->ViewZoneId].Width;
                ViewZoneInfo->PinMaxWindow[CandPinIdx[CandIdx]].Height = pYuvStrmInfo->MaxChanWin[ChanInfo->ViewZoneId].Height;
            }
#ifdef DEBUG_BINDING_EFFECT
            AmbaLL_LogUInt5("    ChanNum[%d] ChanId[%d] PinUpdate[0x%X] ChanLink[0x%X]",
                    Idx, ChanInfo->ViewZoneId,
                    VprocInfo.PinUsage[CandPinIdx[CandIdx]],
                    pYuvStrmInfo->Layout.ChanLinked, 0U);
            AmbaLL_LogUInt5("    WindowCheck[%u] PinCheck[%u] CandIdx[%d] CandPinIdx[CandIdx][%d]",
                    WindowCheck, PinCheck, CandIdx, CandPinIdx[CandIdx], 0);
#endif
        }
        HL_SetVprocInfo(HL_MTX_OPT_SET, ChanInfo->ViewZoneId, &VprocInfo);
        HL_GetViewZoneInfoUnLock(ChanInfo->ViewZoneId);
    }
    HL_GetResourceLock(&Resource);
    DSP_SetBit(&Resource->PostPinLinkMask[CandPinIdx[CandIdx]], YuvIdx);
    HL_GetResourceUnLock();

    *pStrmViewZoneOI = StrmViewZoneOI;

    return Rval;
}

static inline void HL_ArngBindEffPrevBCBindPotential(const UINT16 YuvIdx,
                                                     const UINT16 CandIdx,
                                                     const UINT16 *CandPinIdx,
                                                     const CTX_YUV_STRM_INFO_s *pYuvStrmInfo,
                                                     const UINT32 StrmViewZoneOI)
{
    UINT8 WindowCheck, PinCheck;
    UINT16 Idx, MaxPinOutWidth = 0U, TileNum = 0U;
    CTX_VPROC_INFO_s VprocInfo = {0};
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    for (Idx=0U; Idx < AMBA_DSP_MAX_YUVSTRM_VIEW_NUM; Idx++) {
        if (0U == DSP_GetBit(pYuvStrmInfo->MaxChanBitMask, Idx, 1U)) {
            continue;
        }
        if (1U == DSP_GetBit(StrmViewZoneOI, Idx, 1U)) {
            continue;
        }

        HL_GetVprocInfo(HL_MTX_OPT_GET, Idx, &VprocInfo);
        HL_GetViewZoneInfoLock(Idx, &ViewZoneInfo);
        if (CandPinIdx[CandIdx] == DSP_VPROC_PIN_MAIN) {
            // Check main w/h is the same with yuv's w/h
            if ((pYuvStrmInfo->MaxChanWin[Idx].Width == ViewZoneInfo->Main.Width) &&
                (pYuvStrmInfo->MaxChanWin[Idx].Height == ViewZoneInfo->Main.Height)) {
                WindowCheck = 1U;
            } else {
                WindowCheck = 0U;
            }
        } else {
            // Check the MAX(preview's width) >= yuv's width
#ifdef SUPPORT_RESAMPLING_CHECK
            AMBA_DSP_WINDOW_s PrevWindow = {0};
            PrevWindow.Width = pYuvStrmInfo->MaxChanWin[Idx].Width;
            PrevWindow.Height = pYuvStrmInfo->MaxChanWin[Idx].Height;
            (void)HL_CalcVideoTile(pYuvStrmInfo->MaxChanWin[Idx].Width, SEC2_MAX_OUT_WIDTH, &TileNum, TILE_OVERLAP_WIDTH);
            (void)HL_GetVprocPinMaxWidth(CandPinIdx[CandIdx], &ViewZoneInfo->Main, &PrevWindow, &MaxPinOutWidth);
#else
            MaxPinOutWidth = (UINT16)(HL_MaxVprocPinOutputWidthMap[CandPinIdx[CandIdx]]);
#endif
            WindowCheck = ((MaxPinOutWidth*TileNum) >= pYuvStrmInfo->MaxChanWin[Idx].Width)? 1U: 0U;
        }
        PinCheck = (0U == VprocInfo.PinUsage[CandPinIdx[CandIdx]])? 1U: 0U;

        if ((WindowCheck == 1U) && (PinCheck == 1U)) {
            DSP_SetBit(&VprocInfo.PinUsageOnStrm[CandPinIdx[CandIdx]][YuvIdx], (UINT16)Idx);
            DSP_SetBit(&VprocInfo.PinUsage[CandPinIdx[CandIdx]], (UINT16)YuvIdx);
            ViewZoneInfo->PinMaxWindow[CandPinIdx[CandIdx]].Width = pYuvStrmInfo->MaxChanWin[Idx].Width;
            ViewZoneInfo->PinMaxWindow[CandPinIdx[CandIdx]].Height = pYuvStrmInfo->MaxChanWin[Idx].Height;
        }
        HL_GetViewZoneInfoUnLock(Idx);
        HL_SetVprocInfo(HL_MTX_OPT_SET, Idx, &VprocInfo);
#ifdef DEBUG_BINDING_EFFECT
        AmbaLL_LogUInt5("    MaxWin ChanId[%d] PinUpdate[0x%X] ChanLink[0x%X]",
                Idx, VprocInfo.PinUsage[CandPinIdx[CandIdx]], pYuvStrmInfo->Layout.ChanLinked, 0U, 0U);
#endif
    }
}

static inline UINT32 HL_ArngBindEffPrevBC(const UINT16 YuvStrmNum)
{
    UINT8 IsVoutPurpose, IsEfctYuvStrm, IsDramMipiYuv;
    UINT16 YuvIdx, FoundPin;
    UINT16 CandPinNum, CandPinIdx[DSP_VPROC_PIN_NUM], CandIdx;
    UINT32 Rval = OK, StrmViewZoneOI = 0U;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    IsDramMipiYuv = (UINT8)HL_GetDramMipiYuvEnable();

    /*
     * EfctYuvStrms must come from SAME preview buffer
     *   EncStream: Main or PreviewA
     *   Vout0: PreviewC or B or A(when ABC_REARRANGE)
     *   Vout1: PreviewB or C or A(when ABC_REARRANGE)
     *
     * Due to uCode PrevComBuf definition (NonEffect/Effect are different)
     * we need to take care of common buffer conflict when Effect/NonEfct YuvBuffer co-exist
     *   Said :
     *     PrevC Effect to VOUT1, BufNum will set to PrevCom0
     *     PrevA NonEfct to VOUT0, BufNum will overwrite PrevCom0....
     * A simple rule is when bind EfctVout, also check NonEfctVout to see it will conflict PrevComBuf setting or not
     */
    for (YuvIdx=0U; YuvIdx<YuvStrmNum; YuvIdx++) {
        HL_GetYuvStrmInfoPtr(YuvIdx, &YuvStrmInfo);
        IsEfctYuvStrm = (1U == HL_GetEffectEnableOnYuvStrm(YuvIdx))? (UINT8)1U: (UINT8)0U;

        if (IsEfctYuvStrm == 1U) {
            IsVoutPurpose = (1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U))? 1U: 0U;
            (void)HL_GetEffectChannelVprocPin(YuvStrmInfo->Purpose,
                                              YuvStrmInfo->DestVout,
                                              YuvStrmInfo->DestEnc,
                                              &CandPinNum,
                                              &CandPinIdx[0U],
                                              IsDramMipiYuv,
                                              YuvStrmInfo->YuvBuf.DataFmt);

            FoundPin = 0U;
            for (CandIdx = 0U; CandIdx < CandPinNum; CandIdx++) {
                HL_GetResourcePtr(&Resource);
#ifdef DEBUG_BINDING_EFFECT
                AmbaLL_LogUInt5("Effect Strm[%d] CandIdx[%d] PinIdx[%d] PostPinLinkMask[0x%x]",
                        YuvIdx, CandIdx, CandPinIdx[CandIdx], Resource->PostPinLinkMask[CandPinIdx[CandIdx]], 0U);
#endif
                if (Resource->PostPinLinkMask[CandPinIdx[CandIdx]] > 0U) {
                    continue;
                }
                if (CandPinIdx[CandIdx] == DSP_VPROC_PIN_NUM) {
                    AmbaLL_LogUInt5("No matching in Effect: VprocBind[%d] NumChan[%d] Purpose[0x%X] DestEnc[%d] DestVout[%d]",
                        YuvIdx, YuvStrmInfo->Layout.NumChan, YuvStrmInfo->Purpose,
                        YuvStrmInfo->DestEnc, YuvStrmInfo->DestVout);
                    Rval = DSP_ERR_0006;
                    continue;
                }

#ifdef DEBUG_BINDING_EFFECT
                AmbaLL_LogUInt5("Effect Strm[%d] NumChan[%d] Purpose[0x%X] CandIdx[%d] PinIdx[%d]",
                        YuvIdx, YuvStrmInfo->Layout.NumChan, YuvStrmInfo->Purpose,
                        CandIdx, CandPinIdx[CandIdx]);
#endif

                /* A TestRun to make sure all Chan pass PinCheck/WinCheck */
                Rval = HL_ArngBindEffPrevBCPreBind(YuvIdx,
                                                   IsVoutPurpose,
                                                   YuvStrmInfo,
                                                   CandPinIdx[CandIdx],
                                                   YuvStrmNum,
                                                   &FoundPin);
                if (Rval != OK) {
                    AmbaLL_LogUInt5("PrevBC PreBind[0x%X]", YuvIdx, Rval, 0U, 0U, 0U);
                }


                // Check if every channel have found a corresponding pin-out
                if (FoundPin < YuvStrmInfo->Layout.NumChan) {
                    if (CandIdx == (CandPinNum - 1U)) {
                        AmbaLL_LogUInt5("[%d]No matching in Effect: YuvStrm[%d] missed some channels[0x%X] [%d %d]",
                                         __LINE__, YuvIdx, StrmViewZoneOI, FoundPin, YuvStrmInfo->Layout.NumChan);
                        Rval = DSP_ERR_0006;
                    }
                } else {
                    //Actual Run
                    Rval = HL_ArngBindEffPrevBCBind(YuvIdx,
                                                    CandIdx,
                                                    CandPinIdx,
                                                    YuvStrmInfo,
                                                    &StrmViewZoneOI);
                    if (Rval != OK) {
                        AmbaLL_LogUInt5("PrevBC Bind[0x%X]", YuvIdx, Rval, 0U, 0U, 0U);
                    }

                    // Check if there is any potential pin-out
                    if (StrmViewZoneOI != YuvStrmInfo->MaxChanBitMask) {
                        // Bind potential pins
                        HL_ArngBindEffPrevBCBindPotential(YuvIdx,
                                                          CandIdx,
                                                          CandPinIdx,
                                                          YuvStrmInfo,
                                                          StrmViewZoneOI);
                    }

                    break;
                }
            }
        }
#ifdef DEBUG_BINDING_EFFECT
        HL_GetYuvStrmInfoPtr(YuvIdx, &YuvStrmInfo);
        AmbaLL_LogUInt5("    ~Efct YuvStrmId[%u] ChanLinked[%d]",
                YuvIdx, YuvStrmInfo->Layout.ChanLinked, 0U, 0U, 0U);
#endif
    }

    return Rval;
}
#endif

#if defined(UCODE_SUPPORT_EFFECT_PREV_BC_REARRANGE) || defined(UCODE_SUPPORT_EFFECT_PREV_ABC_REARRANGE)

#else
static inline UINT32 HL_ArngBindEffFixed(void)
{
    UINT32 Rval = OK;
    UINT16 ViewIdx, YuvIdx, EncStrmIdx, Idx, PinIdx, FoundPin = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout;
    CTX_VPROC_INFO_s VprocInfo = {0};
    CTX_STREAM_INFO_s StrmInfo;
    const AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *ChanInfo = NULL;
    UINT8 IsEncPurpose, IsVertSliceEnable = 0U, IsEncSyncEnable, IsDramMipiYuv;
    UINT8 IsMemPurpose, IsVoutPurpose, IsEfctYuvStrm;
    UINT16 LowDlyPurpose = 0U;
    UINT8 ExitYuvIdxLoop, WindowCheck, PinCheck;
    UINT16 MaxPinOutWidth, TileNum = 0U;
    UINT32 StrmViewZoneOI = 0U;

    HL_GetResourcePtr(&Resource);
    IsDramMipiYuv = (UINT8)HL_GetDramMipiYuvEnable();

    for (YuvIdx=0U; YuvIdx<Resource->YuvStrmNum; YuvIdx++) {
        HL_GetYuvStrmInfoLock(YuvIdx, &YuvStrmInfo);
        pYuvStrmLayout = &YuvStrmInfo->Layout;
        IsEfctYuvStrm = (1U == HL_GetEffectEnableOnYuvStrm(YuvIdx))? (UINT8)1U: (UINT8)0U;

        /* Skip binding process once binding failure happens */
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Bind][Effect] Skip it due to previous binding failure", 0U, 0U, 0U, 0U, 0U);
            break;
        }

        if (IsEfctYuvStrm == 1U) {
            (void)HL_GetEffectChannelVprocPin(YuvStrmInfo->Purpose,
                                              YuvStrmInfo->DestVout,
                                              YuvStrmInfo->DestEnc,
                                              &PinIdx,
                                              IsDramMipiYuv);

            if (PinIdx == DSP_VPROC_PIN_NUM) {
                AmbaLL_LogUInt5("[Bind][Effect] No pin matched to YuvStrm[%d], Purpose[0x%X] DestEnc[%d] DestVout[%d]",
                                YuvIdx, YuvStrmInfo->Purpose, YuvStrmInfo->DestEnc, YuvStrmInfo->DestVout, 0U);
                Rval = DSP_ERR_0006;
                continue;
            }

        #ifdef DEBUG_BINDING_EFFECT
            AmbaLL_LogUInt5("Effect Strm[%d] NumChan[%d] Purpose[0x%X] PinIdx[%d]", YuvIdx, pYuvStrmLayout->NumChan, YuvStrmInfo->Purpose, PinIdx, 0U);
        #endif
            FoundPin = 0U; StrmViewZoneOI = 0U;
            for (Idx=0U; Idx<pYuvStrmLayout->NumChan; Idx++) {
                if (1U == DSP_GetBit(pYuvStrmLayout->ChanLinked, Idx, 1U)) {
                    AmbaLL_LogUInt5("[Bind][Effect] No pin matched to YuvStrm[%d], its Channel[%d] had been linked[%d]",
                                    YuvIdx, Idx, pYuvStrmLayout->ChanLinked, 0U, 0U);
                    Rval = DSP_ERR_0006;
                    continue;
                }

                ChanInfo = (AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *)&pYuvStrmLayout->ChanCfg[Idx];
                HL_GetViewZoneInfoLock(ChanInfo->ViewZoneId, &ViewZoneInfo);
                if (PinIdx == DSP_VPROC_PIN_MAIN) {
                    // Check main w/h is identical to yuv's w/h
                    UINT32 Cmp;

                    (void)dsp_osal_memcmp(&ViewZoneInfo->PinWindow[PinIdx], &ChanInfo->ROI, sizeof(AMBA_DSP_WINDOW_s), &Cmp);
                    WindowCheck = (0U == Cmp)? 1U: 0U;
                } else {
                    // Check the MAX(preview's width) >= yuv's width
#ifdef SUPPORT_RESAMPLING_CHECK
                    (void)HL_CalcVideoTile(ChanInfo->Window.Width, SEC2_MAX_OUT_WIDTH, &TileNum, TILE_OVERLAP_WIDTH);
                    (void)HL_GetVprocPinMaxWidth(PinIdx, &ViewZoneInfo->Main, &ChanInfo->Window, &MaxPinOutWidth);
//SUPPORT_RESAMPLING_CHECK
#else
                    MaxPinOutWidth = (UINT16)(HL_MaxVprocPinOutputWidthMap[PinIdx]);
//SUPPORT_RESAMPLING_CHECK
#endif
                    WindowCheck = ((MaxPinOutWidth*TileNum) >= ChanInfo->Window.Width)? 1U: 0U;
                }

                HL_GetVprocInfo(HL_MTX_OPT_GET, ChanInfo->ViewZoneId, &VprocInfo);
                PinCheck = (0U == VprocInfo.PinUsage[PinIdx])? 1U: 0U;

                if ((WindowCheck == 1U) && (PinCheck == 1U)) {
                    //found and occupy it
                    DSP_SetBit(&pYuvStrmLayout->ChanLinked, Idx);

                    DSP_SetBit(&VprocInfo.PinUsageOnStrm[PinIdx][YuvIdx], (UINT16)ChanInfo->ViewZoneId);
                    DSP_SetBit(&VprocInfo.PinUsage[PinIdx], (UINT16)YuvIdx);
                    VprocInfo.PinUsageNumOnStrm[PinIdx][YuvIdx]++;
                    DSP_SetBit(&StrmViewZoneOI, ChanInfo->ViewZoneId);
                    FoundPin++;

                    if (PinIdx < DSP_VPROC_PIN_MAIN) {
                        if (HL_GET_ROTATE(ChanInfo->RotateFlip) == DSP_ROTATE_90_DEGREE) {
                            ViewZoneInfo->PinWindow[PinIdx].Width = ChanInfo->Window.Height;
                            ViewZoneInfo->PinWindow[PinIdx].Height = ChanInfo->Window.Width;
                        } else {
                            ViewZoneInfo->PinWindow[PinIdx].Width = ChanInfo->Window.Width;
                            ViewZoneInfo->PinWindow[PinIdx].Height = ChanInfo->Window.Height;
                        }
                        /* Rotation is changeable after liveview start,
                         * so we expect user provide maximal size before rotation */
                        ViewZoneInfo->PinMaxWindow[PinIdx].Width = YuvStrmInfo->MaxChanWin[ChanInfo->ViewZoneId].Width;
                        ViewZoneInfo->PinMaxWindow[PinIdx].Height = YuvStrmInfo->MaxChanWin[ChanInfo->ViewZoneId].Height;
                    }
                #ifdef DEBUG_BINDING_EFFECT
                    AmbaLL_LogUInt5("    ChanNum[%d] ChanId[%d] PinUpdate[0x%X] ChanLink[0x%X]", Idx, ChanInfo->ViewZoneId, VprocInfo.PinUsage[PinIdx], pYuvStrmLayout->ChanLinked, 0U);
                #endif
                }
                HL_SetVprocInfo(HL_MTX_OPT_SET, ChanInfo->ViewZoneId, &VprocInfo);
                HL_GetViewZoneInfoUnLock(ChanInfo->ViewZoneId);
            }

            // Check if every channel has found a corresponding pin-out
            if (FoundPin < pYuvStrmLayout->NumChan) {
                AmbaLL_LogUInt5("[Bind][Effect] No pin matched to YuvStrm[%d], it missed some channels[0x%X]",
                                YuvIdx, StrmViewZoneOI, 0U, 0U, 0U);
                Rval = DSP_ERR_0006;
                continue;
            }

            // Check if there is any potential pin-out
            if (StrmViewZoneOI == YuvStrmInfo->MaxChanBitMask) {
                continue;
            }

            // Bind potential pins
            for (Idx=0U; Idx < AMBA_DSP_MAX_YUVSTRM_VIEW_NUM; Idx++) {
                if (0U == DSP_GetBit(YuvStrmInfo->MaxChanBitMask, Idx, 1U)) {
                    continue;
                }
                if (1U == DSP_GetBit(StrmViewZoneOI, Idx, 1U)) {
                    continue;
                }

                HL_GetVprocInfo(HL_MTX_OPT_GET, Idx, &VprocInfo);
                HL_GetViewZoneInfoLock(Idx, &ViewZoneInfo);
                if (PinIdx == DSP_VPROC_PIN_MAIN) {
                    // Check main w/h is the same with yuv's w/h
                    if ((YuvStrmInfo->MaxChanWin[Idx].Width == ViewZoneInfo->Main.Width) &&
                        (YuvStrmInfo->MaxChanWin[Idx].Height == ViewZoneInfo->Main.Height)) {
                        WindowCheck = 1U;
                    } else {
                        WindowCheck = 0U;
                    }
                } else {
                    // Check the MAX(preview's width) >= yuv's width
#ifdef SUPPORT_RESAMPLING_CHECK
                    AMBA_DSP_WINDOW_s PrevWindow = {0};
                    PrevWindow.Width = YuvStrmInfo->MaxChanWin[Idx].Width;
                    PrevWindow.Height = YuvStrmInfo->MaxChanWin[Idx].Height;
                    (void)HL_CalcVideoTile(YuvStrmInfo->MaxChanWin[Idx].Width, SEC2_MAX_OUT_WIDTH, &TileNum, TILE_OVERLAP_WIDTH);
                    (void)HL_GetVprocPinMaxWidth(PinIdx, &ViewZoneInfo->Main, &PrevWindow, &MaxPinOutWidth);
//SUPPORT_RESAMPLING_CHECK
#else
                    MaxPinOutWidth = (UINT16)(HL_MaxVprocPinOutputWidthMap[PinIdx]);
//SUPPORT_RESAMPLING_CHECK
#endif
                    WindowCheck = ((MaxPinOutWidth*TileNum) >= YuvStrmInfo->MaxChanWin[Idx].Width)? 1U: 0U;
                }
                PinCheck = (0U == VprocInfo.PinUsage[PinIdx])? 1U: 0U;

                if ((WindowCheck == 1U) && (PinCheck == 1U)) {
                    DSP_SetBit(&VprocInfo.PinUsage[PinIdx], (UINT16)YuvIdx);
                    ViewZoneInfo->PinMaxWindow[PinIdx].Width = YuvStrmInfo->MaxChanWin[Idx].Width;
                    ViewZoneInfo->PinMaxWindow[PinIdx].Height = YuvStrmInfo->MaxChanWin[Idx].Height;
                }
                HL_GetViewZoneInfoUnLock(Idx);
                HL_SetVprocInfo(HL_MTX_OPT_SET, Idx, &VprocInfo);
            #ifdef DEBUG_BINDING_EFFECT
                AmbaLL_LogUInt5("    MaxWin ChanId[%d] PinUpdate[0x%X] ChanLink[0x%X]", Idx, VprocInfo.PinUsage[PinIdx], pYuvStrmLayout->ChanLinked, 0U, 0U);
            #endif
            }
        }
        HL_GetYuvStrmInfoUnLock(YuvIdx);
    }

    return Rval;
}
#endif

static inline UINT32 HL_ArngBindEff(const UINT16 YuvStrmNum)
{
#if defined(UCODE_SUPPORT_EFFECT_PREV_BC_REARRANGE) || defined(UCODE_SUPPORT_EFFECT_PREV_ABC_REARRANGE)
    return HL_ArngBindEffPrevBC(YuvStrmNum);
#else
    return HL_ArngBindEffFixed();
#endif

}

static inline void HL_FindYuvStrmLoop(const UINT16 ViewIdx,
                                      const UINT16 Idx,
                                      const UINT8 IsMemPurpose,
                                      const UINT8 IsVoutPurpose,
                                      const UINT8 IsEncPurpose,
                                      const UINT8 IsVertSliceEnable,
                                      const AMBA_DSP_WINDOW_s *pVzPinWindow,
                                      const UINT32 YuvStrmInfoDestEnc,
                                      const AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *pYuvChanInfo,
                                      UINT8 *pExitYuvIdxLoop,
                                      UINT16 *pFoundPin,
                                      UINT16 *pYuvIdx)
{
    UINT8 ExitYuvIdxLoop = *pExitYuvIdxLoop;
    UINT8 WindowCheck;
    UINT16 EncStrmIdx, YuvIdx = *pYuvIdx, FoundPin = *pFoundPin;
    CTX_STREAM_INFO_s StrmInfo;

    if (IsVertSliceEnable > 0U) {
        if ((IsMemPurpose == 1U) && (IsVoutPurpose == 0U)) {
            UINT32 CmpROI, CmpWin;
            (void)dsp_osal_memcmp(pVzPinWindow, &pYuvChanInfo->ROI, sizeof(AMBA_DSP_WINDOW_s), &CmpROI);
            (void)dsp_osal_memcmp(pVzPinWindow, &pYuvChanInfo->Window, sizeof(AMBA_DSP_WINDOW_s), &CmpWin);
            WindowCheck = ((0U == CmpROI) && (0U == CmpWin))? 1U: 0U;

            if ((ViewIdx == pYuvChanInfo->ViewZoneId) && (WindowCheck == 1U)) {
                FoundPin = 1U;
                YuvIdx = Idx;
                ExitYuvIdxLoop = 1U;
            } else {
                //
#ifdef DEBUG_BINDING_MAIN
                AmbaLL_LogUInt5("    Main ViewIdx[%u] CmpROI[%u] CmpWin[%u] WindowCheck[%u] ViewZoneId[%u]",
                        ViewIdx, CmpROI, CmpWin, WindowCheck, ChanInfo->ViewZoneId);
#endif
            }
        }
    } else {
        if (((IsMemPurpose == 1U) || (IsEncPurpose == 1U)) && (IsVoutPurpose == 0U)) {
            UINT32 CmpROI, CmpWin;
            (void)dsp_osal_memcmp(pVzPinWindow, &pYuvChanInfo->ROI, sizeof(AMBA_DSP_WINDOW_s), &CmpROI);
            (void)dsp_osal_memcmp(pVzPinWindow, &pYuvChanInfo->Window, sizeof(AMBA_DSP_WINDOW_s), &CmpWin);
            WindowCheck = ((0U == CmpROI) && (0U == CmpWin))? 1U: 0U;

            if ((ViewIdx == pYuvChanInfo->ViewZoneId) && (WindowCheck == 1U)) {
                FoundPin = 1U;
                YuvIdx = Idx;
                ExitYuvIdxLoop = 1U;
            } else {
                //
#ifdef DEBUG_BINDING_MAIN
                AmbaLL_LogUInt5("    Main ViewIdx[%u] CmpROI[%u] CmpWin[%u] WindowCheck[%u] ViewZoneId[%u]",
                        ViewIdx, CmpROI, CmpWin, WindowCheck, ChanInfo->ViewZoneId);
#endif
            }
            /* Fill-in EncodeStream source YuvStrmID */
            if ((IsEncPurpose == 1U) && (YuvStrmInfoDestEnc != 0U)) {
                DSP_Bit2U16Idx(YuvStrmInfoDestEnc, &EncStrmIdx);
                HL_GetStrmInfo(HL_MTX_OPT_GET, EncStrmIdx, &StrmInfo);
                StrmInfo.SourceYuvStrmId = Idx;
                HL_SetStrmInfo(HL_MTX_OPT_SET, EncStrmIdx, &StrmInfo);
            }
        }
    }
    *pExitYuvIdxLoop = ExitYuvIdxLoop;
    *pYuvIdx = YuvIdx;
    *pFoundPin = FoundPin;
}

static inline void HL_ArngBindNoneVoutFindYuvStrm(const UINT16 ViewIdx,
                                                  const UINT16 YuvStrmNum,
                                                  UINT16 *pFoundPin,
                                                  UINT16 *pYuvIdx)
{
    UINT8 IsMemPurpose, IsVoutPurpose, IsEncPurpose, IsVertSliceEnable = 0U;
    UINT8 ExitYuvIdxLoop = 0U;
    UINT16 Idx;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout;

    HL_GetViewZoneInfoPtr(ViewIdx, &ViewZoneInfo);
    if (ViewZoneInfo->SliceNumRow > (UINT8)1U) {
        IsVertSliceEnable |= 1U;
    } else {
        IsVertSliceEnable = 0U;
    }

    for (Idx=0U; Idx<YuvStrmNum; Idx++) {
        HL_GetYuvStrmInfoPtr(Idx, &YuvStrmInfo);
        pYuvStrmLayout = &YuvStrmInfo->Layout;
#ifdef DEBUG_BINDING_MAIN
        AmbaLL_LogUInt5("    Main YuvStrm[%d] Purpose[0x%X] ChanLinked[%u]",
                Idx, YuvStrmInfo->Purpose, pYuvStrmLayout->ChanLinked, 0U, 0U);
#endif
        if (pYuvStrmLayout->ChanLinked != 0U) {
            continue;
        }
        IsMemPurpose  = (1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_MEMORY_IDX, 1U))? 1U: 0U;
        IsEncPurpose  = (1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_ENCODE_IDX, 1U))? 1U: 0U;
        IsVoutPurpose = (1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX,   1U))? 1U: 0U;

        HL_FindYuvStrmLoop(ViewIdx,
                           Idx,
                           IsMemPurpose,
                           IsVoutPurpose,
                           IsEncPurpose,
                           IsVertSliceEnable,
                           &ViewZoneInfo->PinWindow[DSP_VPROC_PIN_MAIN],
                           YuvStrmInfo->DestEnc,
                           &pYuvStrmLayout->ChanCfg[0U],
                           &ExitYuvIdxLoop,
                           pFoundPin,
                           pYuvIdx);

        if (ExitYuvIdxLoop == 1U) {
            break;
        }
    }
//moved to HL_FindYuvStrmLoop
//    *pYuvIdx = YuvIdx;
//    *pFoundPin = FoundPin;
}

static inline UINT32 HL_ArngBindNoneVout(const UINT16 YuvStrmNum,
                                         const UINT16 MaxViewZoneNum,
                                         const UINT32 ViewZoneActiveBit)
{
    UINT8 PinCheck;
    UINT16 ViewIdx, YuvIdx, FoundPin;
    UINT32 Rval = OK;
    CTX_VPROC_INFO_s VprocInfo = {0};
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout;
    UINT8 MctfEnable = 0U, MctsEnable = 0U, MctsOutEnable = 0U, MctfCmpr = 0U;

    for (ViewIdx=0U; ViewIdx<MaxViewZoneNum; ViewIdx++) {
        if (0U == DSP_GetBit(ViewZoneActiveBit, ViewIdx, 1U)) {
            continue;
        }

        YuvIdx = 0U;
        FoundPin = 0U;
        HL_ArngBindNoneVoutFindYuvStrm(ViewIdx, YuvStrmNum, &FoundPin, &YuvIdx);
        (void)HL_GetViewZoneMctfEnable(ViewIdx, &MctfEnable, &MctsEnable, &MctsOutEnable, &MctfCmpr);

        /* If we have found a matching YuvStream, bind this YuvStream to Vproc's main Pin */
        HL_GetVprocInfo(HL_MTX_OPT_GET, ViewIdx, &VprocInfo);
        PinCheck = (0U == VprocInfo.PinUsage[DSP_VPROC_PIN_MAIN])? 1U: 0U;
        if ((PinCheck == 1U) && (FoundPin == 1U) && (MctsOutEnable == 1U)) {
            //found and occupy it
            HL_GetYuvStrmInfoLock(YuvIdx, &YuvStrmInfo);
            pYuvStrmLayout = &YuvStrmInfo->Layout;
            DSP_SetBit(&pYuvStrmLayout->ChanLinked, 0U);
#ifdef DEBUG_BINDING_MAIN
            {
                CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
                HL_GetViewZoneInfoPtr(ViewIdx, &ViewZoneInfo);
                AmbaLL_LogUInt5("    Main Update ViewZone[%d] MainW[%d] YuvStrm[%d] YuvW[%d] ChanLink[0x%X]",
                        ViewIdx, ViewZoneInfo->PinWindow[DSP_VPROC_PIN_MAIN].Width,
                        YuvIdx, pYuvStrmLayout->ChanCfg[0].Window.Width,
                        pYuvStrmLayout->ChanLinked);
            }
#endif
            HL_GetYuvStrmInfoUnLock(YuvIdx);

            DSP_SetBit(&VprocInfo.PinUsageOnStrm[DSP_VPROC_PIN_MAIN][YuvIdx], (UINT16)ViewIdx);
            DSP_SetBit(&VprocInfo.PinUsage[DSP_VPROC_PIN_MAIN], YuvIdx);
            VprocInfo.PinUsageNumOnStrm[DSP_VPROC_PIN_MAIN][YuvIdx]++;
        } else {
            //
#ifdef DEBUG_BINDING_MAIN
            AmbaLL_LogUInt5("    ~Main Update ViewZone[%d] YuvStrm[%d] ChanLink[0x%X] PinCheck[0x%X] FoundPin[0x%X]",
                    ViewIdx, YuvIdx, pYuvStrmLayout->ChanLinked, PinCheck, FoundPin);
#endif
        }
        HL_SetVprocInfo(HL_MTX_OPT_SET, ViewIdx, &VprocInfo);
    }

    return Rval;
}

static inline UINT32 HL_ArngBindDummyPrev(const UINT16 YuvIdx,
                                          const CTX_YUV_STRM_INFO_s *pYuvStrmInfo,
                                          const UINT32 StrmViewZoneOI,
                                          UINT8 IsDramMipiYuv)
{
    UINT8 Found;
    UINT8 IsEncSyncEnable;
    UINT16 TileNum = 0U, LowDlyPurpose = 0U;
    UINT16 Idx, PinIdx = 0U;
    UINT32 Rval = OK;
    CTX_VPROC_INFO_s VprocInfo = {0};
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    HL_GetResourcePtr(&Resource);

    if (StrmViewZoneOI != pYuvStrmInfo->MaxChanBitMask) {
        for (Idx = 0U; Idx < AMBA_DSP_MAX_YUVSTRM_VIEW_NUM; Idx++) {
            AMBA_DSP_WINDOW_s MaxWin = {0}, ROI = {0};
            if (0U == DSP_GetBit(pYuvStrmInfo->MaxChanBitMask, Idx, 1U)) {
                continue;
            }
            if (1U == DSP_GetBit(StrmViewZoneOI, Idx, 1U)) {
                continue;
            }
            HL_GetViewZoneInfoPtr(Idx, &ViewZoneInfo);

            if (ViewZoneInfo->SliceNumRow > (UINT8)1U) {
                IsEncSyncEnable = (1U == DSP_GetU16Bit(ViewZoneInfo->EncSyncOpt, ENC_SYNC_ENABLE_IDX, 1U))? 1U: 0U;
            } else {
                IsEncSyncEnable = 0U;
            }

            (void)HL_CalcVideoTile(ViewZoneInfo->Main.Width, SEC2_MAX_OUT_WIDTH, &TileNum, TILE_OVERLAP_WIDTH);
            MaxWin.Width = pYuvStrmInfo->MaxChanWin[Idx].Width;
            MaxWin.Height = pYuvStrmInfo->MaxChanWin[Idx].Height;
            ROI.Width = ViewZoneInfo->Main.Width;
            ROI.Height = ViewZoneInfo->Main.Height;
            Found = (UINT8)0U;
            (void)HL_FindMinimalVprocPin(&MaxWin, &ROI,
                                         TileNum, Idx, &PinIdx, &Found,
                                         LowDlyPurpose, IsEncSyncEnable, IsDramMipiYuv);
            if (Found == (UINT8)1U) {
                /* Update Vproc PinUsage */
                HL_GetVprocInfo(HL_MTX_OPT_GET, Idx, &VprocInfo);
                DSP_SetBit(&VprocInfo.PinUsageOnStrm[PinIdx][YuvIdx], (UINT32)Idx);
                DSP_SetBit(&VprocInfo.PinUsage[PinIdx], (UINT32)YuvIdx);
                VprocInfo.PinUsageNumOnStrm[PinIdx][YuvIdx]++;
                HL_SetVprocInfo(HL_MTX_OPT_SET, Idx, &VprocInfo);

                /* Update ViewZone Pin Window */
                HL_GetViewZoneInfoLock(Idx, &ViewZoneInfo);
                ViewZoneInfo->PinMaxWindow[PinIdx].Width = pYuvStrmInfo->MaxChanWin[Idx].Width;
                ViewZoneInfo->PinMaxWindow[PinIdx].Height = pYuvStrmInfo->MaxChanWin[Idx].Height;
                HL_GetViewZoneInfoUnLock(Idx);
            } else {
                AmbaLL_LogUInt5("[Bind] No pin matched to YuvStrm[%d]'s potential member[%d]",
                        YuvIdx, Idx, 0U, 0U, 0U);
                Rval = DSP_ERR_0006;
                continue;
            }
        }
    }

    return Rval;
}

static inline UINT32 HL_ArngBindVout(const UINT16 YuvStrmNum)
{
    UINT8 ExitYuvLoop = 0U;
    UINT8 IsVoutPurpose, IsVertSliceEnable = 0U, IsEncSyncEnable, IsDramMipiYuv;
    UINT16 TileNum = 0U, LowDlyPurpose = 0U;
    UINT16 YuvIdx, PinIdx;
    UINT8 Found;
    UINT32 Rval = OK;
    UINT32 StrmViewZoneOI = 0U;
    CTX_VPROC_INFO_s VprocInfo = {0};
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout;
    const AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *ChanInfo = NULL;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);

    IsDramMipiYuv = (UINT8)HL_GetDramMipiYuvEnable();

    for (YuvIdx = 0U; YuvIdx < YuvStrmNum; YuvIdx++) {
        HL_GetYuvStrmInfoPtr(YuvIdx, &YuvStrmInfo);
        pYuvStrmLayout = &YuvStrmInfo->Layout;
        IsVoutPurpose = (1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U))? 1U: 0U;
        LowDlyPurpose = (IsVertSliceEnable > 0U)? YuvStrmInfo->Purpose: 0U;
        StrmViewZoneOI = 0U;

        if ((pYuvStrmLayout->ChanLinked == 0U) && (IsVoutPurpose == 1U)) {
#ifdef DEBUG_BINDING_VOUT
            AmbaLL_LogUInt5("Vout YuvIdx[%d] Purpose[%d] NumChan[%d] ChanLink[0x%X]",
                    YuvIdx, YuvStrmInfo->Purpose, pYuvStrmLayout->NumChan, pYuvStrmLayout->ChanLinked, 0U);
#endif
            ChanInfo = (AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *)&pYuvStrmLayout->ChanCfg[0U];
            HL_GetViewZoneInfoPtr(ChanInfo->ViewZoneId, &ViewZoneInfo);

            if (ViewZoneInfo->SliceNumRow > (UINT8)1U) {
                IsEncSyncEnable = (1U == DSP_GetU16Bit(ViewZoneInfo->EncSyncOpt, ENC_SYNC_ENABLE_IDX, 1U))? 1U: 0U;
            } else {
                IsEncSyncEnable = 0U;
            }

            (void)HL_CalcVideoTile(ViewZoneInfo->Main.Width, SEC2_MAX_OUT_WIDTH, &TileNum, TILE_OVERLAP_WIDTH);
            Found = (UINT8)0U;
            (void)HL_FindMinimalVprocPin(&ChanInfo->Window, &ChanInfo->ROI,
                                         TileNum, ChanInfo->ViewZoneId, &PinIdx, &Found,
                                         LowDlyPurpose, IsEncSyncEnable, IsDramMipiYuv);
#ifdef DEBUG_BINDING_VOUT
            AmbaLL_LogUInt5("Vout TileNum[%d] Main[%d] FoundPin[%d] ChanLink[0x%X]",
                    TileNum, ViewZoneInfo->Main.Width, Found, pYuvStrmLayout->ChanLinked, 0U);
#endif
            if (Found == (UINT8)1U) {
                /* Update Vproc PinUsage */
                HL_GetVprocInfo(HL_MTX_OPT_GET, ChanInfo->ViewZoneId, &VprocInfo);
                DSP_SetBit(&VprocInfo.PinUsageOnStrm[PinIdx][YuvIdx], (UINT32)ChanInfo->ViewZoneId);
                DSP_SetBit(&VprocInfo.PinUsage[PinIdx], (UINT32)YuvIdx);
                VprocInfo.PinUsageNumOnStrm[PinIdx][YuvIdx]++;
                HL_SetVprocInfo(HL_MTX_OPT_SET, ChanInfo->ViewZoneId, &VprocInfo);

                /* Update ViewZone Pin Window */
                HL_GetViewZoneInfoLock(ChanInfo->ViewZoneId, &ViewZoneInfo);
                if (HL_GET_ROTATE(ChanInfo->RotateFlip) == DSP_ROTATE_90_DEGREE) {
                    ViewZoneInfo->PinWindow[PinIdx].Width = ChanInfo->Window.Height;
                    ViewZoneInfo->PinWindow[PinIdx].Height = ChanInfo->Window.Width;
                    ViewZoneInfo->PinMaxWindow[PinIdx].Width = YuvStrmInfo->MaxChanWin[ChanInfo->ViewZoneId].Height;
                    ViewZoneInfo->PinMaxWindow[PinIdx].Height = YuvStrmInfo->MaxChanWin[ChanInfo->ViewZoneId].Width;
                } else {
                    ViewZoneInfo->PinWindow[PinIdx].Width = ChanInfo->Window.Width;
                    ViewZoneInfo->PinWindow[PinIdx].Height = ChanInfo->Window.Height;
                    ViewZoneInfo->PinMaxWindow[PinIdx].Width = YuvStrmInfo->MaxChanWin[ChanInfo->ViewZoneId].Width;
                    ViewZoneInfo->PinMaxWindow[PinIdx].Height = YuvStrmInfo->MaxChanWin[ChanInfo->ViewZoneId].Height;
                }
                HL_GetViewZoneInfoUnLock(ChanInfo->ViewZoneId);

                /* Update YuvStream link status*/
                HL_GetYuvStrmInfoLock(YuvIdx, &YuvStrmInfo);
                DSP_SetBit(&pYuvStrmLayout->ChanLinked, 0U);
                HL_GetYuvStrmInfoUnLock(YuvIdx);
                DSP_SetBit(&StrmViewZoneOI, (UINT32)ChanInfo->ViewZoneId);
#ifdef DEBUG_BINDING_VOUT
                AmbaLL_LogUInt5("  Vout ChanIdx[%d] ViewId[%d] PinIdx[%d]",
                        0U, ChanInfo->ViewZoneId, PinIdx, 0U, 0U);
                AmbaLL_LogUInt5("    Vout PinUpdate[0x%X] ChanLink[0x%X]",
                        VprocInfo.PinUsage[PinIdx], pYuvStrmLayout->ChanLinked, 0U, 0U, 0U);
#endif
            } else {
                AmbaLL_LogUInt5("[Bind][Vout] No pin matched to YuvStrm[%d]", YuvIdx, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0006;
                ExitYuvLoop = 1U;
            }

            /* Bind potential windows as dummy preview out */
            if (Rval == OK) {
                Rval = HL_ArngBindDummyPrev(YuvIdx,
                                            YuvStrmInfo,
                                            StrmViewZoneOI,
                                            IsDramMipiYuv);
                if (Rval != OK) {
                    Rval = DSP_ERR_0006;
                    ExitYuvLoop = 1U;
                }
            } else {
                Rval = DSP_ERR_0006;
                ExitYuvLoop = 1U;
            }
        }
        if (ExitYuvLoop == 1U) {
            break;
        }
    }

    return Rval;
}

static inline UINT32 HL_ArngBindRemStrm(const UINT16 YuvStrmNum)
{
    UINT8 ExitYuvLoop = 0U;
    UINT8 IsVertSliceEnable = 0U, IsEncSyncEnable, IsDramMipiYuv;
    UINT16 TileNum = 0U;
    UINT16 LowDlyPurpose = 0U;
    UINT16 YuvIdx, EncStrmIdx, PinIdx;
    UINT8 Found;
    UINT32 Rval = OK;
    UINT32 StrmViewZoneOI = 0U;
    CTX_VPROC_INFO_s VprocInfo = {0};
    CTX_STREAM_INFO_s StrmInfo;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout;
    const AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *ChanInfo = NULL;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);

    IsDramMipiYuv = (UINT8)HL_GetDramMipiYuvEnable();

    for (YuvIdx=0U; YuvIdx<YuvStrmNum; YuvIdx++) {
        HL_GetYuvStrmInfoPtr(YuvIdx, &YuvStrmInfo);
        pYuvStrmLayout = &YuvStrmInfo->Layout;
        StrmViewZoneOI = 0U;

        LowDlyPurpose = (IsVertSliceEnable > 0U)? YuvStrmInfo->Purpose: 0U;

        if (0U == DSP_GetBit(pYuvStrmLayout->ChanLinked, 0U, 1U)) {
#ifdef DEBUG_BINDING_ENC
            AmbaLL_LogUInt5("Enc/Mem YuvIdx[%d] Purpose[0x%X] NumChan[%d]", YuvIdx, YuvStrmInfo->Purpose, pYuvStrmLayout->NumChan, 0U, 0U);
#endif
            ChanInfo = (AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *)&pYuvStrmLayout->ChanCfg[0U];
            HL_GetViewZoneInfoPtr(ChanInfo->ViewZoneId, &ViewZoneInfo);

            if (ViewZoneInfo->SliceNumRow > (UINT8)1U) {
                IsEncSyncEnable = (1U == DSP_GetU16Bit(ViewZoneInfo->EncSyncOpt, ENC_SYNC_ENABLE_IDX, 1U))? 1U: 0U;
            } else {
                IsEncSyncEnable = 0U;
            }

            (void)HL_CalcVideoTile(ViewZoneInfo->Main.Width, SEC2_MAX_OUT_WIDTH, &TileNum, TILE_OVERLAP_WIDTH);
            Found = (UINT8)0U;
            (void)HL_FindMinimalVprocPin(&ChanInfo->Window, &ChanInfo->ROI,
                                         TileNum, ChanInfo->ViewZoneId, &PinIdx, &Found,
                                         LowDlyPurpose, IsEncSyncEnable, IsDramMipiYuv);

            if (Found == (UINT8)1U) {
                /* Update Vproc PinUsage */
                HL_GetVprocInfo(HL_MTX_OPT_GET, ChanInfo->ViewZoneId, &VprocInfo);
                DSP_SetBit(&VprocInfo.PinUsageOnStrm[PinIdx][YuvIdx], (UINT32)ChanInfo->ViewZoneId);
                DSP_SetBit(&VprocInfo.PinUsage[PinIdx], (UINT32)YuvIdx);
                VprocInfo.PinUsageNumOnStrm[PinIdx][YuvIdx]++;
                HL_SetVprocInfo(HL_MTX_OPT_SET, ChanInfo->ViewZoneId, &VprocInfo);

                /* Update ViewZone Pin Window */
                HL_GetViewZoneInfoLock(ChanInfo->ViewZoneId, &ViewZoneInfo);
                if (HL_GET_ROTATE(ChanInfo->RotateFlip) == DSP_ROTATE_90_DEGREE) {
                    ViewZoneInfo->PinWindow[PinIdx].Width = ChanInfo->Window.Height;
                    ViewZoneInfo->PinWindow[PinIdx].Height = ChanInfo->Window.Width;
                    ViewZoneInfo->PinMaxWindow[PinIdx].Width = YuvStrmInfo->MaxChanWin[ChanInfo->ViewZoneId].Height;
                    ViewZoneInfo->PinMaxWindow[PinIdx].Height = YuvStrmInfo->MaxChanWin[ChanInfo->ViewZoneId].Width;
                } else {
                    ViewZoneInfo->PinWindow[PinIdx].Width = ChanInfo->Window.Width;
                    ViewZoneInfo->PinWindow[PinIdx].Height = ChanInfo->Window.Height;
                    ViewZoneInfo->PinMaxWindow[PinIdx].Width = YuvStrmInfo->MaxChanWin[ChanInfo->ViewZoneId].Width;
                    ViewZoneInfo->PinMaxWindow[PinIdx].Height = YuvStrmInfo->MaxChanWin[ChanInfo->ViewZoneId].Height;
                }
                HL_GetViewZoneInfoUnLock(ChanInfo->ViewZoneId);

                /* Update YuvStream link status*/
                HL_GetYuvStrmInfoLock(YuvIdx, &YuvStrmInfo);
                DSP_SetBit(&pYuvStrmLayout->ChanLinked, 0U);
                HL_GetYuvStrmInfoUnLock(YuvIdx);
                DSP_SetBit(&StrmViewZoneOI, ChanInfo->ViewZoneId);
#ifdef DEBUG_BINDING_ENC
                AmbaLL_LogUInt5("  Enc/Mem ChanIdx[%d] ViewId[%d] PinIdx[%d]", 0U, ChanInfo->ViewZoneId, PinIdx, 0U, 0U);
                AmbaLL_LogUInt5("    Enc/Mem PinUpdate[0x%X] ChanLink[0x%X]", VprocInfo.PinUsage[PinIdx], pYuvStrmLayout->ChanLinked, 0U, 0U, 0U);
#endif
            } else {
                AmbaLL_LogUInt5("[Bind][Enc/Mem] No pin matched to YuvStrm[%d] SourceVZ[%d]", YuvIdx, ChanInfo->ViewZoneId, 0U, 0U, 0U);
                ExitYuvLoop = 1U;
            }

            /* Bind potential windows as dummy preview out */
            Rval = HL_ArngBindDummyPrev(YuvIdx,
                                        YuvStrmInfo,
                                        StrmViewZoneOI,
                                        IsDramMipiYuv);
            if ((Rval != OK) || (Found == (UINT8)0U)) {
                Rval = DSP_ERR_0006;
                ExitYuvLoop = 1U;
            }
        }
        /* Fill-in EncodeStream source YuvStrmID */
        if ((1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_ENCODE_IDX, 1U)) &&
            (YuvStrmInfo->DestEnc != 0U)) {
            DSP_Bit2U16Idx(YuvStrmInfo->DestEnc, &EncStrmIdx);
            HL_GetStrmInfo(HL_MTX_OPT_GET, EncStrmIdx, &StrmInfo);
            StrmInfo.SourceYuvStrmId = YuvIdx;
            HL_SetStrmInfo(HL_MTX_OPT_SET, EncStrmIdx, &StrmInfo);
        }
        if (ExitYuvLoop == 1U) {
            break;
        }
    }
    return Rval;
}

#ifdef UCODE_SUPPORT_PREVFILTER_REARRANGE
#ifdef CONFIG_ENABLE_DSP_MONITOR
static inline UINT32 HL_ArngBindSetDspMonYuv(void)
{
    UINT16 BitIdx, VprocIdx, VprocNum=0U;
    UINT32 Rval = OK;
    UINT32 PinIdx;

    (void)HL_GetVprocNum(&VprocNum);
    for (PinIdx=0; PinIdx<DSP_VPROC_PIN_NUM; PinIdx++) {
        UINT16 YuvIdx = 0U;
        AMBA_DSP_MONITOR_YUV_SRC_INFO_s YuvOutStrmSrc;

        (void)dsp_osal_memset(&YuvOutStrmSrc, 0x0, sizeof(AMBA_DSP_MONITOR_YUV_SRC_INFO_s));
        for (VprocIdx=0; VprocIdx<VprocNum; VprocIdx++) {
            CTX_VPROC_INFO_s VprocInfo = {0};

            HL_GetVprocInfo(HL_MTX_OPT_ALL, VprocIdx, &VprocInfo);

            for (BitIdx=0; BitIdx<31U; BitIdx++) {
                if (((VprocInfo.PinUsage[PinIdx] >> BitIdx) & 0x1U) == 1U) {
                    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;

                    HL_GetYuvStrmInfoPtr(BitIdx, &YuvStrmInfo);
                    YuvOutStrmSrc.YuvSrc[YuvIdx].YuvStreamId = BitIdx;
                    YuvOutStrmSrc.YuvSrc[YuvIdx].Purpose = YuvStrmInfo->Purpose;
                    YuvIdx++;
                }
            }
        }
        YuvOutStrmSrc.YuvSrcNum = YuvIdx;
        Rval = DspMon_SetYuvOutStrmSrc(PinIdx, &YuvOutStrmSrc);
        if (Rval != OK) {
            AmbaLL_LogUInt5("HL_ArngBindSetDspMonYuv failure", 0U, 0U, 0U, 0U, 0U);
        }
    }
    return Rval;
}
#endif
static inline UINT32 HL_ArngBind(void)
{
    UINT32 Rval;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);
    /*
     * 1. Bind EfctYuvStrms
     */
    Rval = HL_ArngBindEff(Resource->YuvStrmNum);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Bind][Eff] Skip it due to previous binding failure", 0U, 0U, 0U, 0U, 0U);
    }

    /*
     * 2. For every ViewZone, search all the non-Vout YuvStrms to find the one matching Main W/H
     *    when LowDelay, only PrevB could be fed into encoder
     */
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Bind][Main] Skip it due to previous binding failure", 0U, 0U, 0U, 0U, 0U);
    }
    Rval = HL_ArngBindNoneVout(Resource->YuvStrmNum,
                               Resource->MaxViewZoneNum,
                               Resource->ViewZoneActiveBit);

    /*
     * 3. Bind all the Vout purpose YuvStrms
     */
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Bind][Vout] Skip it due to previous binding failure", 0U, 0U, 0U, 0U, 0U);
    }
    Rval = HL_ArngBindVout(Resource->YuvStrmNum);

    /*
     * 4. bind remaining YuvStrms
     */
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Bind][Enc/Mem] Skip it due to previous binding failure", 0U, 0U, 0U, 0U, 0U);
    }
    Rval = HL_ArngBindRemStrm(Resource->YuvStrmNum);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Bind][Enc/Mem] failure", 0U, 0U, 0U, 0U, 0U);
    }
#ifdef CONFIG_ENABLE_DSP_MONITOR
    Rval = HL_ArngBindSetDspMonYuv();
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Bind]HL_ArngBindSetDspMonYuv fail", 0U, 0U, 0U, 0U, 0U);
    }
#endif
    return Rval;
}
//UCODE_SUPPORT_PREVFILTER_REARRANGE
#endif


#ifndef UCODE_SUPPORT_PREVFILTER_REARRANGE
static inline UINT32 HL_FixedBind(void)
{
    UINT32 Rval = OK;
    UINT16 ViewIdx, YuvIdx, EncStrmIdx, Idx, PinIdx, FoundPin = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout;
    CTX_VPROC_INFO_s VprocInfo = {0};
    CTX_STREAM_INFO_s StrmInfo;
    const AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *ChanInfo = NULL;
    UINT8 IsEncPurpose, IsVertSliceEnable = 0U, IsEncSyncEnable;
    UINT8 IsMemPurpose, IsVoutPurpose, IsEfctYuvStrm;
    UINT16 LowDlyPurpose = 0U;
    UINT8 ExitYuvIdxLoop, WindowCheck, PinCheck;
    UINT16 MaxPinOutWidth, TileNum = 0U;
    UINT32 StrmViewZoneOI = 0U;

    HL_GetResourcePtr(&Resource);

    /***************************************************
     * uCode only support fixed preview filter routing *
     ***************************************************/

    //1. Always need MainOuput, Use Largest YuvStream without vout purpose
    for (ViewIdx=0U; ViewIdx<Resource->ViewZoneNum; ViewIdx++) {
        HL_GetViewZoneInfoPtr(ViewIdx, &ViewZoneInfo);

        for (YuvIdx=0U; YuvIdx<Resource->YuvStrmNum; YuvIdx++) {
            HL_GetYuvStrmInfoLock(YuvIdx, &YuvStrmInfo);
            pYuvStrmLayout = &YuvStrmInfo->Layout;

#ifdef DEBUG_BINDING_MAIN
            AmbaLL_LogUInt5("    Main Strm[%d] Purpose[0x%X]", YuvIdx, YuvStrmInfo->Purpose, 0U, 0U, 0U);
#endif
            IsMemPurpose = (1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_MEMORY_IDX, 1U))? 1U: 0U;
            IsEncPurpose =(1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_ENCODE_IDX, 1U))? 1U: 0U;
            IsVoutPurpose = (1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U))? 1U: 0U;

            if (((IsMemPurpose == 1U) || (IsEncPurpose == 1U)) && (IsVoutPurpose == 0U)) {
                for (Idx=0U; Idx<pYuvStrmLayout->NumChan; Idx++) {
                    UINT32 Cmp;
                    ChanInfo = (AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *)&pYuvStrmLayout->ChanCfg[Idx];

                    IsChanLinked = (1U == DSP_GetBit(pYuvStrmLayout->ChanLinked, Idx, 1U))? 1U: 0U;
                    (void)dsp_osal_memcmp(&ViewZoneInfo->PinWindow[DSP_VPROC_PIN_MAIN], &ChanInfo->ROI, sizeof(AMBA_DSP_WINDOW_s), &Cmp);
                    IsSameWin = (0U == Cmp)? 1U: 0U;

                    if ((ViewIdx == ChanInfo->ViewZoneId) && (IsChanLinked == 0U) && (IsSameWin == 1U) &&
                        (MaxViewZoneWidth[ViewIdx] < ChanInfo->Window.Width)) {
                        MaxViewZoneWidthStrmIdx[ViewIdx] = YuvIdx;
                        MaxViewZoneWidth[ViewIdx] = ChanInfo->Window.Width;
                        DSP_SetBit(&pYuvStrmLayout->ChanLinked, Idx);
                    }
                }

                if (IsEncPurpose == 1U) {
                    for (Idx=0U; Idx<AMBA_DSP_MAX_STREAM_NUM; Idx++) {
                        if (1U == DSP_GetBit(YuvStrmInfo->DestEnc, Idx, 1U)) {
                            HL_GetStrmInfo(HL_MTX_OPT_GET, Idx, &StrmInfo);
                            StrmInfo.SourceYuvStrmId = YuvIdx;
                            HL_SetStrmInfo(HL_MTX_OPT_SET, Idx, &StrmInfo);
                        }
                    }
                }
            }
            HL_GetYuvStrmInfoUnLock(YuvIdx);
#ifdef DEBUG_BINDING_MAIN
            AmbaLL_LogUInt5("    Main Update ChanLink[0x%X]", pYuvStrmLayout->ChanLinked, 0U, 0U, 0U, 0U);
#endif
        }

        /* Fill it */
        HL_GetVprocInfo(HL_MTX_OPT_GET, ViewIdx, &VprocInfo);
        if (0U == VprocInfo.PinUsage[DSP_VPROC_PIN_MAIN]) {
            //found and occupy it
            DSP_SetBit(&VprocInfo.PinUsageOnStrm[DSP_VPROC_PIN_MAIN][MaxViewZoneWidthStrmIdx[ViewIdx]], (UINT32)ViewIdx);
            DSP_SetBit(&VprocInfo.PinUsage[DSP_VPROC_PIN_MAIN], (UINT32)MaxViewZoneWidthStrmIdx[ViewIdx]);
            VprocInfo.PinUsageNumOnStrm[DSP_VPROC_PIN_MAIN][MaxViewZoneWidthStrmIdx[ViewIdx]]++;
        }

#ifdef DEBUG_BINDING_MAIN
        AmbaLL_LogUInt5("Main[%d] MaxW[%d][%d]", ViewIdx, MaxViewZoneWidth[ViewIdx], MaxViewZoneWidthStrmIdx[ViewIdx], 0U, 0U);
#endif
        HL_SetVprocInfo(HL_MTX_OPT_SET, ViewIdx, &VprocInfo);
    }

    //2. Search Vout purpose first, since only PrecB/C support vout and PrevB connect to TV, PrevC connect to LCD
    ExitYuvIdxLoop = 0U;
    for (YuvIdx=0U; (YuvIdx<Resource->YuvStrmNum) && (ExitYuvIdxLoop == 0U); YuvIdx++) {
        HL_GetYuvStrmInfoLock(YuvIdx, &YuvStrmInfo);
        pYuvStrmLayout = &YuvStrmInfo->Layout;

#ifdef DEBUG_BINDING_VOUT
        AmbaLL_LogUInt5("Vout YuvIdx[%d] Purpose[%d] NumChan[%d] ChanLink[0x%X]", YuvIdx, YuvStrmInfo->Purpose, pYuvStrmLayout->NumChan, pYuvStrmLayout->ChanLinked, 0U);
#endif

        if (1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U)) {
            for (Idx=0U; Idx<pYuvStrmLayout->NumChan; Idx++) {
                if (0U == DSP_GetBit(pYuvStrmLayout->ChanLinked, Idx, 1U)) {
                    ChanInfo = (AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *)&pYuvStrmLayout->ChanCfg[Idx];

                    FoundPin = 0U;
                    HL_GetViewZoneInfoLock(ChanInfo->ViewZoneId, &ViewZoneInfo);
                    HL_GetVprocInfo(HL_MTX_OPT_GET, ChanInfo->ViewZoneId, &VprocInfo);

                    if (1U == DSP_GetU16Bit(YuvStrmInfo->DestVout, 0U/*LCD*/, 1U)) {
                        PinIdx = DSP_VPROC_PIN_PREVC;
                    } else if (1U == DSP_GetU16Bit(YuvStrmInfo->DestVout, 1U/*TV*/, 1U)) {
                        PinIdx = DSP_VPROC_PIN_PREVB;
                    } else {
                        AmbaLL_LogUInt5("Invalid vout[%d] dest", YuvStrmInfo->DestVout, 0U, 0U, 0U, 0U);
                    }

#ifdef SUPPORT_RESAMPLING_CHECK
                    (void)HL_GetVprocPinMaxWidth(PinIdx, &ViewZoneInfo->Main, &ChanInfo->Window, MaxTileNum, &MaxPinOutWidth);
//SUPPORT_RESAMPLING_CHECK
#else
                    if (PinIdx != DSP_VPROC_PIN_MAIN) {
                        MaxPinOutWidth = HL_MaxVprocPinOutputWidthMap[PinIdx]*MaxTileNum;
                    } else {
                        MaxPinOutWidth = HL_MaxVprocPinOutputWidthMap[PinIdx];
                    }
//SUPPORT_RESAMPLING_CHECK
#endif

#ifdef DEBUG_BINDING_VOUT
                    AmbaLL_LogUInt5("    Vout PinIdx[%d] PinUsage[0x%X] Win[%d %d]", PinIdx, VprocInfo.PinUsage[PinIdx], MaxPinOutWidth, ChanInfo->Window.Width, 0U);
#endif
                    if (0U == VprocInfo.PinUsage[PinIdx]) {
                        if (MaxPinOutWidth >= ChanInfo->Window.Width) {
                            //found and occupy it
                            DSP_SetBit(&VprocInfo.PinUsageOnStrm[PinIdx][YuvIdx], (UINT32)ChanInfo->ViewZoneId);
                            DSP_SetBit(&VprocInfo.PinUsage[PinIdx], (UINT32)YuvIdx);
                            VprocInfo.PinUsageNumOnStrm[PinIdx][YuvIdx]++;

                            if (HL_GET_ROTATE(ChanInfo->RotateFlip) > 0U) {
                                ViewZoneInfo->PinWindow[PinIdx].Width = ChanInfo->Window.Height;
                                ViewZoneInfo->PinWindow[PinIdx].Height = ChanInfo->Window.Width;
                            } else {
                                ViewZoneInfo->PinWindow[PinIdx].Width = ChanInfo->Window.Width;
                                ViewZoneInfo->PinWindow[PinIdx].Height = ChanInfo->Window.Height;
                            }
                            DSP_SetBit(&pYuvStrmLayout->ChanLinked, Idx);
#ifdef DEBUG_BINDING_VOUT
                            AmbaLL_LogUInt5("    Vout PinUpdate[0x%X] ChanLink[0x%X]", VprocInfo.PinUsage[PinIdx], pYuvStrmLayout->ChanLinked, 0U, 0U, 0U);
#endif
                            FoundPin = 1U;
                        }
                    }

                    HL_SetVprocInfo(HL_MTX_OPT_SET, ChanInfo->ViewZoneId, &VprocInfo);
                    HL_GetViewZoneInfoUnLock(ChanInfo->ViewZoneId);
                }

                if (FoundPin == 0U) {
                    AmbaLL_LogUInt5("No matching on Vout-VprocBind[%d]", YuvIdx, 0U, 0U, 0U, 0U);
                    ExitYuvIdxLoop = 1U;
                    Rval = DSP_ERR_0006;
                }
            }
        }
        HL_GetYuvStrmInfoUnLock(YuvIdx);
    }

    //3. Bind remaining YuvStream for mem or encode
    ExitYuvIdxLoop = 0U;
    for (YuvIdx=0U; (YuvIdx<Resource->YuvStrmNum) && (ExitYuvIdxLoop == 0U); YuvIdx++) {
        HL_GetYuvStrmInfoLock(YuvIdx, &YuvStrmInfo);
        pYuvStrmLayout = &YuvStrmInfo->Layout;

        IsMemPurpose = (1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_MEMORY_IDX, 1U))? 1U: 0U;
        IsEncPurpose =(1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_ENCODE_IDX, 1U))? 1U: 0U;
        IsVoutPurpose = (1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U))? 1U: 0U;

#ifdef DEBUG_BINDING_ENC
        AmbaLL_LogUInt5("Enc YuvIdx[%d] Purpose[%d] NumChan[%d] ChanLink[0x%X]", YuvIdx, YuvStrmInfo->Purpose, pYuvStrmLayout->NumChan, pYuvStrmLayout->ChanLinked, 0U);
#endif
        if (((IsMemPurpose == 1U) || (IsEncPurpose == 1U)) && (IsVoutPurpose == 0U)) {
            for (Idx=0U; Idx<pYuvStrmLayout->NumChan; Idx++) {
                if (0U == DSP_GetBit(pYuvStrmLayout->ChanLinked, Idx, 1U)) {
                    ChanInfo = (AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *)&pYuvStrmLayout->ChanCfg[Idx];

                    FoundPin = 0U;
                    HL_GetViewZoneInfoLock(ChanInfo->ViewZoneId, &ViewZoneInfo);
                    HL_GetVprocInfo(HL_MTX_OPT_GET, ChanInfo->ViewZoneId, &VprocInfo);
                    /* Find Smallest Pin, TBD as API*/
                    {
                        //only PrevA left
                        PinIdx = DSP_VPROC_PIN_PREVA;

#ifdef SUPPORT_RESAMPLING_CHECK
                        (void)HL_GetVprocPinMaxWidth(PinIdx, &ViewZoneInfo->Main, &ChanInfo->Window, MaxTileNum, &MaxPinOutWidth);
//SUPPORT_RESAMPLING_CHECK
#else
                        if (PinIdx != DSP_VPROC_PIN_MAIN) {
                            MaxPinOutWidth = HL_MaxVprocPinOutputWidthMap[PinIdx]*MaxTileNum;
                        } else {
                            MaxPinOutWidth = HL_MaxVprocPinOutputWidthMap[PinIdx];
                        }
//SUPPORT_RESAMPLING_CHECK
#endif

#ifdef DEBUG_BINDING_ENC
                        AmbaLL_LogUInt5("  Enc ChanIdx[%d]", Idx, 0U, 0U, 0U, 0U);
#endif
                        if (0U == VprocInfo.PinUsage[PinIdx]) {
                            if (MaxPinOutWidth >= ChanInfo->Window.Width) {
                                //found and occupy it
                                DSP_SetBit(&VprocInfo.PinUsageOnStrm[PinIdx][YuvIdx], (UINT32)ChanInfo->ViewZoneId);
                                DSP_SetBit(&VprocInfo.PinUsage[PinIdx], (UINT32)YuvIdx);
                                VprocInfo.PinUsageNumOnStrm[PinIdx][YuvIdx]++;

                                if (HL_GET_ROTATE(ChanInfo->RotateFlip) > 0U) {
                                    ViewZoneInfo->PinWindow[PinIdx].Width = ChanInfo->Window.Height;
                                    ViewZoneInfo->PinWindow[PinIdx].Height = ChanInfo->Window.Width;
                                } else {
                                    ViewZoneInfo->PinWindow[PinIdx].Width = ChanInfo->Window.Width;
                                    ViewZoneInfo->PinWindow[PinIdx].Height = ChanInfo->Window.Height;
                                }
                                DSP_SetBit(&pYuvStrmLayout->ChanLinked, Idx);

#ifdef DEBUG_BINDING_ENC
                                AmbaLL_LogUInt5("    Enc PinUpdate[0x%X] ChanLink[0x%X]", VprocInfo.PinUsage[PinIdx], pYuvStrmLayout->ChanLinked, 0U, 0U, 0U);
#endif
                                FoundPin = 1U;
                            }
                        }
                    }
                    HL_SetVprocInfo(HL_MTX_OPT_SET, ChanInfo->ViewZoneId, &VprocInfo);
                    HL_GetViewZoneInfoUnLock(ChanInfo->ViewZoneId);

                    if (FoundPin == 0U) {
                        AmbaLL_LogUInt5("No matching in Enc/Mem-VprocBind[%d]", YuvIdx, 0U, 0U, 0U, 0U);
                        ExitYuvIdxLoop = 1U;
                        Rval = DSP_ERR_0006;
                    }
                }
            }

            if (IsEncPurpose == 1U) {
                for (Idx=0U; Idx<AMBA_DSP_MAX_STREAM_NUM; Idx++) {
                    if (1U == DSP_GetBit(YuvStrmInfo->DestEnc, Idx, 1U)) {
                        HL_GetStrmInfo(HL_MTX_OPT_GET, Idx, &StrmInfo);
                        StrmInfo.SourceYuvStrmId = YuvIdx;
                        HL_SetStrmInfo(HL_MTX_OPT_SET, Idx, &StrmInfo);
                    }
                }
            }
        }
        HL_GetYuvStrmInfoUnLock(YuvIdx);
    }
    return Rval;
}
#endif

UINT32 HL_VideoProcBinding(void)
{
#ifdef UCODE_SUPPORT_PREVFILTER_REARRANGE
    return HL_ArngBind();
#else
    return HL_FixedBind();
#endif
}

static inline void HL_SetDspSystemCfgMbf2ndHalf(UINT8 ParIdx, UINT32 Val)
{
    if (ParIdx == DSP_CONFIG_PARAM_CBUF) {
        TuneDspSystemCfg.CBuf = (UINT16)Val;
        AmbaLL_LogUInt5("Syscfg CBuf[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_PARAM_BDT) {
        TuneDspSystemCfg.Bdt = (UINT16)Val;
        AmbaLL_LogUInt5("Syscfg Bdt[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_PARAM_BD) {
        TuneDspSystemCfg.Bd = (UINT16)Val;
        AmbaLL_LogUInt5("Syscfg Bd[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_PARAM_IMGINF) {
        TuneDspSystemCfg.ImgInf = (UINT16)Val;
        AmbaLL_LogUInt5("Syscfg ImgInf[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_PARAM_EXT_FB) {
        TuneDspSystemCfg.ExtFb = (UINT16)Val;
        AmbaLL_LogUInt5("Syscfg ExtFb[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_PARAM_MCBL) {
        TuneDspSystemCfg.Mcbl = (UINT16)Val;
        AmbaLL_LogUInt5("Syscfg Mcbl[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_PARAM_MCB) {
        TuneDspSystemCfg.Mcb = (UINT16)Val;
        AmbaLL_LogUInt5("Syscfg Mcb[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_PARAM_MBUF_PAR) {
        TuneDspSystemCfg.MbufPar = (UINT16)Val;
        AmbaLL_LogUInt5("Syscfg MbufPar[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_PARAM_MBUF) {
        TuneDspSystemCfg.Mbuf = Val;
        AmbaLL_LogUInt5("Syscfg Mbuf[%d]", Val, 0U, 0U, 0U, 0U);
    } else {
//        AmbaLL_LogUInt5("Syscfg ParamterId out of index [%d/%d]",ParIdx, DSP_CONFIG_PARAM_NUM, 0U, 0U, 0U);
    }
}

static inline void HL_SetDspSystemCfgMbf(UINT8 ParIdx, UINT32 Val)
{
    if (ParIdx == DSP_CONFIG_PARAM_DRAM_PAR) {
        TuneDspSystemCfg.DramPar = (UINT16)Val;
        AmbaLL_LogUInt5("Syscfg DramPar[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_PARAM_SMEM_PAR) {
        TuneDspSystemCfg.SmemPar = (UINT16)Val;
        AmbaLL_LogUInt5("Syscfg SmemPar[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_PARAM_SUB_PAR) {
        TuneDspSystemCfg.SubPar = (UINT16)Val;
        AmbaLL_LogUInt5("Syscfg SubPar[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_PARAM_SUP_PAR) {
        TuneDspSystemCfg.SupPar = (UINT16)Val;
        AmbaLL_LogUInt5("Syscfg SupPar[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_PARAM_FBP) {
        TuneDspSystemCfg.Fbp = (UINT16)Val;
        AmbaLL_LogUInt5("Syscfg Fbp[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_PARAM_FB) {
        TuneDspSystemCfg.Fb = (UINT16)Val;
        AmbaLL_LogUInt5("Syscfg Fbto[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_PARAM_DBP) {
        TuneDspSystemCfg.Dbp = (UINT16)Val;
        AmbaLL_LogUInt5("Syscfg Dbp[%d]", Val, 0U, 0U, 0U, 0U);
    } else {
        HL_SetDspSystemCfgMbf2ndHalf(ParIdx, Val);
    }
}

static inline void HL_SetDspSystemCfgOrcallMsg2ndHalf(UINT8 ParIdx, UINT32 Val)
{
    if (ParIdx == DSP_CONFIG_PARAM_MFBP_CACHE) {
        TuneDspSystemCfg.MFbpCache = (UINT8)Val;
        AmbaLL_LogUInt5("Syscfg MFbpCache[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_PARAM_MFB_CACHE) {
        TuneDspSystemCfg.MFbCache = (UINT8)Val;
        AmbaLL_LogUInt5("Syscfg MFbCache[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_PARAM_MIMGINF_CACHE) {
        TuneDspSystemCfg.MImgInfCache = (UINT8)Val;
        AmbaLL_LogUInt5("Syscfg ImgInfCache[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_PARAM_ORCCODE_MSG_Q) {
        TuneDspSystemCfg.OrcCodeMsgQ = (UINT8)Val;
        AmbaLL_LogUInt5("Syscfg OrcCodeMsgQ[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_PARAM_ORCALL_MSG_Q) {
        TuneDspSystemCfg.OrcAllMsgQ = (UINT8)Val;
        AmbaLL_LogUInt5("Syscfg OrcAllMsgQ[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_IDSP_TIMEOUT) {
        TuneDspSystemCfg.IdspTimeOut = (UINT16)Val;
        AmbaLL_LogUInt5("Syscfg IdspTimeout[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_WARP_DRAM_OUT) {
        TuneDspSystemCfg.WarpDramOut = (UINT8)Val;
        AmbaLL_LogUInt5("Syscfg WarpDramOut[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_C2Y_DRAM_OUT) {
        TuneDspSystemCfg.C2YDramOut = (UINT8)Val;
        AmbaLL_LogUInt5("Syscfg C2YDramOut[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_C2Y_BURST_TILE) {
        TuneDspSystemCfg.C2YBurstTile = (UINT8)Val;
        AmbaLL_LogUInt5("Syscfg C2YBurstTile[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_AAA_ENABLE) {
        TuneDspSystemCfg.AAAEnable = (UINT8)Val;
        AmbaLL_LogUInt5("Syscfg AAAEnable[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_LI_ENABLE) {
        TuneDspSystemCfg.LiEnable = (UINT8)Val;
        AmbaLL_LogUInt5("Syscfg LiEnable[%d]", Val, 0U, 0U, 0U, 0U);
    } else {
//        AmbaLL_LogUInt5("Syscfg ParamterId out of index [%d/%d]",ParIdx, DSP_CONFIG_PARAM_NUM, 0U, 0U, 0U);
    }
}

static inline void HL_SetDspSystemCfgOrcallMsg(UINT8 ParIdx, UINT32 Val)
{
    if (ParIdx == DSP_CONFIG_PARAM_AIK_PAR) {
        TuneDspSystemCfg.AikPar = (UINT16)Val;
        AmbaLL_LogUInt5("Syscfg AikPar[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_PARAM_MFBP) {
        TuneDspSystemCfg.MFbp = (UINT16)Val;
        AmbaLL_LogUInt5("Syscfg MFbp[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_PARAM_MFB) {
        TuneDspSystemCfg.MFb = (UINT16)Val;
        AmbaLL_LogUInt5("Syscfg MFb[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_PARAM_MEXT_FB) {
        TuneDspSystemCfg.MExtFb = (UINT16)Val;
        AmbaLL_LogUInt5("Syscfg MExtFb[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_PARAM_MIMGINF) {
        TuneDspSystemCfg.MImgInf = (UINT16)Val;
        AmbaLL_LogUInt5("Syscfg MImgInf[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_PARAM_PG_SZ_LOG2) {
        TuneDspSystemCfg.PgSzLog2 = (UINT16)Val;
        AmbaLL_LogUInt5("Syscfg PgSzLog2[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_PARAM_IDSP_SBLK0) {
        TuneDspSystemCfg.IdspSblk0 = (UINT8)Val;
        AmbaLL_LogUInt5("Syscfg IdspSblk0[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_PARAM_IDSP_SBLK) {
        TuneDspSystemCfg.IdspSblk = (UINT8)Val;
        AmbaLL_LogUInt5("Syscfg IdspSblk[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_PARAM_FBP_CACHE) {
        TuneDspSystemCfg.FbpCache = (UINT8)Val;
        AmbaLL_LogUInt5("Syscfg FbpCache[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_PARAM_FB_CACHE) {
        TuneDspSystemCfg.FbCache = (UINT8)Val;
        AmbaLL_LogUInt5("Syscfg FbCache[%d]", Val, 0U, 0U, 0U, 0U);
    } else if (ParIdx == DSP_CONFIG_PARAM_IMGINF_CACHE) {
        TuneDspSystemCfg.ImgInfCache = (UINT8)Val;
        AmbaLL_LogUInt5("Syscfg ImgInfCache[%d]", Val, 0U, 0U, 0U, 0U);
    } else {
        HL_SetDspSystemCfgOrcallMsg2ndHalf(ParIdx, Val);
    }
}

static void SetDspSystemCfg(UINT8 ParIdx, UINT32 Val)
{
    if (ParIdx >= DSP_CONFIG_PARAM_NUM) {
        AmbaLL_LogUInt5("Syscfg ParamterId out of index [%d/%d]",
                ParIdx, DSP_CONFIG_PARAM_NUM, 0U, 0U, 0U);
    } else {
        HL_SetDspSystemCfgMbf(ParIdx, Val);
        HL_SetDspSystemCfgOrcallMsg(ParIdx, Val);
    }
}

void HL_SetDspSystemCfg(UINT8 ParIdx, UINT32 Val)
{
    UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;
    SetDspSystemCfg(ParIdx, Val);
    if (pIsSimilarEnabled != NULL) {
        SIM_SetDspSystemCfg(ParIdx, Val);
    }
}

static UINT32 FillVideoProcSideBandConfig(cmd_vin_attach_sideband_info_to_cap_frm_t *VinAttachSideband, UINT16 ViewZoneId)
{
    UINT32 Rval = OK;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT32 ViewZoneVinId = 0U, i;
    sideband_info_t *pSidebandInfo = NULL;
    ULONG ULAddr;

    if (VinAttachSideband == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
        DSP_Bit2Idx((UINT32)ViewZoneInfo->SourceVin, &ViewZoneVinId);

        VinAttachSideband->vin_id = (UINT8)ViewZoneVinId;
        VinAttachSideband->sideband_info_num = ViewZoneInfo->SidebandNum;
        HL_GetPointerToDspFovSideBandCfg(ViewZoneId, ViewZoneInfo->SidebandAddrIdx, &pSidebandInfo);
        for (i = 0U; i < (VinAttachSideband->sideband_info_num); i++) {
            dsp_osal_typecast(&ULAddr, &pSidebandInfo);
            (void)dsp_osal_virt2cli(ULAddr, &(VinAttachSideband->per_fov_sideband_info_addr[i]));
            pSidebandInfo++;
        }
    }
    return Rval;
}
UINT32 HL_FillVideoProcSideBandConfig(cmd_vin_attach_sideband_info_to_cap_frm_t *VinAttachSideband, UINT16 ViewZoneId)
{
    UINT32 Rval;
    UINT32 CrcHL, CrcSimilar;
    const UINT32 *pU32 = NULL;
    UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;
#ifdef CONFIG_ENABLE_DSP_MONITOR
    AMBA_DSP_MONITOR_CRC_CMPR_s CrcCmpr;
#endif

    Rval = FillVideoProcSideBandConfig(VinAttachSideband, ViewZoneId);
    if (pIsSimilarEnabled != NULL) {
        if (1U == IsSimilarEnabled()) {
            UINT32 SIM_Rval;
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_vin_attach_sideband_info_to_cap_frm_t *SimVinAttachSideband = SIM_DefCtxCmdBufPtrVinSidBend;

            dsp_osal_typecast(&pU32, &VinAttachSideband);
            CrcHL = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_vin_attach_sideband_info_to_cap_frm_t), 0U);

            SIM_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&SimVinAttachSideband, &CmdBufferAddr);
            SIM_Rval = SIM_FillVideoProcSideBandConfig(SimVinAttachSideband, ViewZoneId);
            if (SIM_Rval == OK) {
                dsp_osal_typecast(&pU32, &SimVinAttachSideband);
                CrcSimilar = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_vin_attach_sideband_info_to_cap_frm_t), 0U);
#ifdef CONFIG_ENABLE_DSP_MONITOR
                CrcCmpr.ModuleId = AMBA_SSP_PRINT_MODULE_ID;
                CrcCmpr.Crc0 = CrcHL;
                CrcCmpr.Crc1 = CrcSimilar;
                CrcCmpr.UUID = __LINE__;
                SIM_Rval = dsp_mon_crc_cmpr(&CrcCmpr);
                if (SIM_Rval != OK) {
                    AmbaLL_LogUInt5("dsp_mon_crc_cmpr return 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
                }
#else
                if (CrcSimilar != CrcHL) {
                    AmbaLL_LogUInt5("[ERR] Similar CRC mismatch @ %d HL 0x%x Similar 0x%x", __LINE__, CrcHL, CrcSimilar, 0U, 0U);
                }
#endif
            } else {
                AmbaLL_LogUInt5("[ERR] Similar function returned NG 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
            }
            SIM_RelCmdBuffer(CmdBufferId);
        }
    }

    return Rval;
}

static UINT32 FillVideoProcGrouping(cmd_vproc_set_vproc_grping *VprocGrping)
{
    UINT8 *pGroupNumAddr = NULL;
    UINT8 *pGroupOrderAddr = NULL;
    UINT8 NumVprocInGrp;
    UINT16 NumGrp = 0U, FreeRunGrpBit = 0U, VprocGrpId = 0U;
    UINT16 i, j;
    UINT32 Rval = OK;
    CTX_RESOURCE_INFO_s *pResource = HL_CtxResInfoPtr;
    CTX_DSP_INST_INFO_s DspInstInfo = {0};
    ULONG ULAddr;

    HL_GetResourcePtr(&pResource);
    (void)HL_GetVprocGroupNum(&NumGrp);
    (void)HL_GetVprocGroupFreeRun(&FreeRunGrpBit);
    for (i = 0U; i < NumGrp; i++) {
        NumVprocInGrp = 0U;
        for (j = 0U; j < pResource->MaxViewZoneNum; j++) {
            (void)HL_GetVprocGroupIdx(j, &VprocGrpId, 0/*STL_PROC*/);
            if (VprocGrpId == i) {
                HL_GetPointerToDspVprocGrpOrd(i, NumVprocInGrp, &pGroupOrderAddr);
                *pGroupOrderAddr = (UINT8)j;
                NumVprocInGrp++;
            }
        }

        /* PIV */
        if (NumVprocInGrp == 0U) {
            HL_GetDspInstance(HL_MTX_OPT_ALL, &DspInstInfo);
            for (j = 0U; j < DspInstInfo.MaxVpocNum; j++) {
                if (DspInstInfo.VprocPurpose[j] == VPROC_PURPOSE_STILL) {
                    (void)HL_GetVprocGroupIdx(j, &VprocGrpId, 1/*STL_PROC*/);
                    if (VprocGrpId == i) {
                        HL_GetPointerToDspVprocGrpOrd(i, NumVprocInGrp, &pGroupOrderAddr);
                        *pGroupOrderAddr = (UINT8)j;
                        NumVprocInGrp++;
                    }
                }
            }
        }
        HL_GetPointerToDspVprocGrpNum(i, &pGroupNumAddr);
        *pGroupNumAddr = NumVprocInGrp;
    }

    VprocGrping->num_of_vproc_groups = (UINT8)NumGrp;
    VprocGrping->max_vprocs_per_group = NUM_VPROC_MAX_CHAN;
#ifdef SUPPORT_VPROC_INDEPENDENT_WITHIN_GROUPING
    VprocGrping->free_run_grp = FreeRunGrpBit;
#else
(void)FreeRunGrpBit;
#endif

    HL_GetPointerToDspVprocGrpNum(0U, &pGroupNumAddr);
    dsp_osal_typecast(&ULAddr, &pGroupNumAddr);
    (void)dsp_osal_virt2cli(ULAddr, &VprocGrping->num_vprocs_per_group_addr);

    HL_GetPointerToDspVprocGrpOrd(0U, 0U, &pGroupOrderAddr);
    dsp_osal_typecast(&ULAddr, &pGroupOrderAddr);
    (void)dsp_osal_virt2cli(ULAddr, &VprocGrping->vprocs_in_a_group_addr);

    return Rval;
}
UINT32 HL_FillVideoProcGrouping(cmd_vproc_set_vproc_grping *VprocGrping)
{
    UINT32 Rval;
    UINT32 CrcHL, CrcSimilar;
    const UINT32 *pU32 = NULL;
    UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;
#ifdef CONFIG_ENABLE_DSP_MONITOR
    AMBA_DSP_MONITOR_CRC_CMPR_s CrcCmpr;
#endif

    Rval = FillVideoProcGrouping(VprocGrping);
    if (pIsSimilarEnabled != NULL) {
        if (1U == IsSimilarEnabled()) {
            UINT32 SIM_Rval;
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_vproc_set_vproc_grping *SimVprocGrping = SIM_DefCtxCmdBufPtrVpcGrp;

            dsp_osal_typecast(&pU32, &VprocGrping);
            CrcHL = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_vproc_set_vproc_grping), 0U);

            SIM_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&SimVprocGrping, &CmdBufferAddr);
            SIM_Rval = SIM_FillVideoProcGrouping(SimVprocGrping);
            if (SIM_Rval == OK) {
                dsp_osal_typecast(&pU32, &SimVprocGrping);
                CrcSimilar = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_vproc_set_vproc_grping), 0U);
#ifdef CONFIG_ENABLE_DSP_MONITOR
                CrcCmpr.ModuleId = AMBA_SSP_PRINT_MODULE_ID;
                CrcCmpr.Crc0 = CrcHL;
                CrcCmpr.Crc1 = CrcSimilar;
                CrcCmpr.UUID = __LINE__;
                SIM_Rval = dsp_mon_crc_cmpr(&CrcCmpr);
                if (SIM_Rval != OK) {
                    AmbaLL_LogUInt5("dsp_mon_crc_cmpr return 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
                }
#else
                if (CrcSimilar != CrcHL) {
                    AmbaLL_LogUInt5("[ERR] Similar CRC mismatch @ %d HL 0x%x Similar 0x%x", __LINE__, CrcHL, CrcSimilar, 0U, 0U);
                }
#endif
            } else {
                AmbaLL_LogUInt5("[ERR] Similar function returned NG 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
            }
            SIM_RelCmdBuffer(CmdBufferId);
        }
    }

    return Rval;
}

#ifndef SUPPORT_VPROC_GROUPING
static UINT32 FillVideoProcChanProcOrder(cmd_vproc_multi_chan_proc_order_t *ChanProcOrder)
{
    UINT32 Rval = OK;
    CTX_VIN_INFO_s VinInfo = {0};
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 ViewZoneOrder[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};
    UINT32 ViewZoneFrameRateX1000[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};
    UINT32 FrameRate = 0U;
    UINT16 Tmp = 0U;
    UINT16 VinIdx = 0;
    UINT16 i, j;
    UINT8 *pVprocOrderAddr = NULL;
    ULONG ULAddr;

    (void)dsp_osal_memset(ViewZoneOrder, 0x0, (sizeof(UINT16)*AMBA_DSP_MAX_VIEWZONE_NUM));
    (void)dsp_osal_memset(ViewZoneFrameRateX1000, 0x0, (sizeof(UINT32)*AMBA_DSP_MAX_VIEWZONE_NUM));
    HL_GetResourcePtr(&Resource);

    /*
     * According to ChenHan, if all the Vprocs are in one group, the Vproc process Priority must follows:
     * (1) Process order shall be arranged base on InputRate
     *     i.e. ch0 30fps, ch1 60fps : order shall be ch1 -> ch0
     * (2) Stopped Vproc must behind active Vprocs
     */

    /* Initial and fill */
    for (i = 0; ((i<Resource->MaxViewZoneNum) && (i<AMBA_DSP_MAX_VIEWZONE_NUM)); i++) {
        ViewZoneOrder[i] = i;
        if (0U == DSP_GetBit(Resource->ViewZoneActiveBit, i, 1U)) {
            /* Set inactive Vproc's FrameRate to 0 to make it behind the active one */
            ViewZoneFrameRateX1000[i] = 0U;
        } else {
            HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
            DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &VinIdx);
            if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON) ||
                (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) ||
                (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_422) ||
                (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_420)) {
                continue;
            }
            HL_GetVinInfo(HL_MTX_OPT_ALL, VinIdx, &VinInfo);
            if ((VinInfo.FrameRate.NumUnitsInTick * (1U + (UINT32)VinInfo.FrameRate.Interlace)) != 0U) {
                FrameRate = (VinInfo.FrameRate.TimeScale*1000U)/(VinInfo.FrameRate.NumUnitsInTick * (1U + (UINT32)VinInfo.FrameRate.Interlace));
            } else {
                FrameRate = 0U;
            }
            ViewZoneFrameRateX1000[i] = FrameRate;
        }
    }

    /* sort it */
    for (i = (Resource->MaxViewZoneNum-1U); ((i>0U) && (i<AMBA_DSP_MAX_VIEWZONE_NUM)); --i) {
        for (j = 0; (j<i); ++j) {
            if (ViewZoneFrameRateX1000[j] < ViewZoneFrameRateX1000[j+1U]) {
                //((j+1U) < AMBA_DSP_MAX_VIEWZONE_NUM)) {
                //Swap
                Tmp = ViewZoneOrder[j];
                ViewZoneOrder[j] = ViewZoneOrder[j+1U];
                ViewZoneOrder[j+1U] = Tmp;

                FrameRate = ViewZoneFrameRateX1000[j];
                ViewZoneFrameRateX1000[j] = ViewZoneFrameRateX1000[j+1U];
                ViewZoneFrameRateX1000[j+1U] = FrameRate;
            }
        }
    }

    ChanProcOrder->num_of_vproc_channel = Resource->MaxViewZoneNum;
    ChanProcOrder->proc_order_check_en = 0U;
    ChanProcOrder->grp_id = 0U;
    for (i = 0; i < ChanProcOrder->num_of_vproc_channel; i++) {
        if (ChanProcOrder->num_of_vproc_channel < 16U) {
            ChanProcOrder->proc_order[i] = (UINT8)ViewZoneOrder[i];
        }
        if (ChanProcOrder->num_of_vproc_channel < NUM_VPROC_MAX_CHAN) {
            HL_GetPointerToDspVprocOrder(0U, i, &pVprocOrderAddr);  //fixme GrpId
            *pVprocOrderAddr = (UINT8)ViewZoneOrder[i];
        }
    }
    HL_GetPointerToDspVprocOrder(0U, 0U, &pVprocOrderAddr); //fixme GrpId
    dsp_osal_typecast(&ULAddr, &pVprocOrderAddr);
    (void)dsp_osal_virt2cli(ULAddr, &ChanProcOrder->proc_order_addr);

    return Rval;
}

UINT32 HL_FillVideoProcChanProcOrder(cmd_vproc_multi_chan_proc_order_t *ChanProcOrder)
{
    UINT32 Rval;
    UINT32 CrcHL, CrcSimilar;
    const UINT32 *pU32 = NULL;
    UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;
#ifdef CONFIG_ENABLE_DSP_MONITOR
    AMBA_DSP_MONITOR_CRC_CMPR_s CrcCmpr;
#endif

    Rval = FillVideoProcChanProcOrder(ChanProcOrder);
    if (pIsSimilarEnabled != NULL) {
        if (1U == IsSimilarEnabled()) {
            UINT32 SIM_Rval;
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_vproc_multi_chan_proc_order_t *SimChanProcOrder = SIM_DefCtxCmdBufPtrVpcChOrd;

            dsp_osal_typecast(&pU32, &ChanProcOrder);
            CrcHL = CalcCheckSum32Add(pU32, sizeof(cmd_vproc_multi_chan_proc_order_t), 0U);

            SIM_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&SimChanProcOrder, &CmdBufferAddr);
            SIM_Rval = SIM_FillVideoProcChanProcOrder(SimChanProcOrder);
            if (SIM_Rval == OK) {
                dsp_osal_typecast(&pU32, &SimChanProcOrder);
                CrcSimilar = CalcCheckSum32Add(pU32, sizeof(cmd_vproc_multi_chan_proc_order_t), 0U);
#ifdef CONFIG_ENABLE_DSP_MONITOR
                CrcCmpr.ModuleId = AMBA_SSP_PRINT_MODULE_ID;
                CrcCmpr.Crc0 = CrcHL;
                CrcCmpr.Crc1 = CrcSimilar;
                CrcCmpr.UUID = __LINE__;
                SIM_Rval = dsp_mon_crc_cmpr(&CrcCmpr);
                if (SIM_Rval != OK) {
                    AmbaLL_LogUInt5("dsp_mon_crc_cmpr return 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
                }
#else
                if (CrcSimilar != CrcHL) {
                    AmbaLL_LogUInt5("[ERR] Similar CRC mismatch @ %d HL 0x%x Similar 0x%x", __LINE__, CrcHL, CrcSimilar, 0U, 0U);
                }
#endif
            } else {
                AmbaLL_LogUInt5("[ERR] Similar function returned NG 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
            }
            SIM_RelCmdBuffer(CmdBufferId);
        }
    }

    return Rval;
}
#endif

void HL_CheckVinInfoUpdatePush(CTX_VIN_INFO_s *pInfo)
{
    /* In order to check VinInfo change at HL_SetVinInfo,
     * Need to preserve correspond field */
    pInfo->CmdUpdate.CapWindow.Width = pInfo->CapWindow[0U].Width;
    pInfo->CmdUpdate.CapWindow.Height = pInfo->CapWindow[0U].Height;
    pInfo->CmdUpdate.Option[0U] = pInfo->Option[0U];
    pInfo->CmdUpdate.IntcNum[0U] = pInfo->IntcNum[0U];
}

void HL_CheckVinInfoUpdate(UINT16 VinId, CTX_VIN_INFO_s *pNewInfo, const CTX_VIN_INFO_s *pOldInfo)
{
    /* When VinInfo change, check if the following CMD shall be resend
    ** 1. CMD_VIN_CE_SETUP
    ** 2. CMD_VIN_HDR_SETUP
    ** 3. CMD_SET_FOV_LAYOUT */
    if ((pNewInfo->CapWindow[0U].Height != pOldInfo->CapWindow[0U].Height) ||
        (pNewInfo->CapWindow[0U].Width != pOldInfo->CapWindow[0U].Width)) {
        pNewInfo->CmdUpdate.CeSetup = (UINT8)1U;
        pNewInfo->CmdUpdate.HdrSetup = (UINT8)1U;
    }
    if ((pNewInfo->Option[0U] != pOldInfo->Option[0U]) ||
        (pNewInfo->IntcNum[0U] != pOldInfo->IntcNum[0U])) {
        // All Fov need to update
        pNewInfo->CmdUpdate.SetFovLayout = (UINT8)HL_GetViewZoneNumOnVin(VinId);
    }
}

void HL_CheckViewZoneInfoUpdatePush(CTX_VIEWZONE_INFO_s *pInfo)
{
    /* In order to check VinInfo change at HL_SetViewZoneInfo,
     * Need to preserve correspond field */

    pInfo->CmdUpdate.HdrBlendNumMinusOne = pInfo->HdrBlendNumMinusOne;
    pInfo->CmdUpdate.SourceVin = pInfo->SourceVin;
    pInfo->CmdUpdate.HdrBlendHeight = pInfo->HdrBlendHeight;
    pInfo->CmdUpdate.HdrRawYOffset[0U] = pInfo->HdrRawYOffset[0U];
    pInfo->CmdUpdate.HdrRawYOffset[1U] = pInfo->HdrRawYOffset[1U];
    pInfo->CmdUpdate.HdrRawYOffset[2U] = pInfo->HdrRawYOffset[2U];
    pInfo->CmdUpdate.LinearCe = pInfo->LinearCe;
    pInfo->CmdUpdate.InputFromMemory = pInfo->InputFromMemory;
    pInfo->CmdUpdate.CapWindow.Width = pInfo->CapWindow.Width;
    pInfo->CmdUpdate.CapWindow.Height = pInfo->CapWindow.Height;
    pInfo->CmdUpdate.CapWindow.OffsetX = pInfo->CapWindow.OffsetX;
    pInfo->CmdUpdate.CapWindow.OffsetY = pInfo->CapWindow.OffsetY;
}

void HL_CheckViewZoneInfoUpdate(CTX_VIEWZONE_INFO_s *pInfo)
{
    /* When ViewZoneInfo change, check if the following CMD shall be resend
    ** 1. CMD_VIN_CE_SETUP
    ** 2. CMD_VIN_HDR_SETUP
    ** 3. CMD_SET_FOV_LAYOUT */
    if ((pInfo->HdrBlendNumMinusOne != pInfo->CmdUpdate.HdrBlendNumMinusOne) ||
        (pInfo->SourceVin != pInfo->CmdUpdate.SourceVin)) {
        pInfo->CmdUpdate.CeSetup = (UINT8)1U;
        pInfo->CmdUpdate.HdrSetup = (UINT8)1U;
        pInfo->CmdUpdate.SetFovLayout= (UINT8)1U;
    }
    if ((pInfo->HdrBlendHeight != pInfo->CmdUpdate.HdrBlendHeight) ||
        (pInfo->HdrRawYOffset[0U] != pInfo->CmdUpdate.HdrRawYOffset[0U]) ||
        (pInfo->HdrRawYOffset[1U] != pInfo->CmdUpdate.HdrRawYOffset[1U]) ||
        (pInfo->HdrRawYOffset[2U] != pInfo->CmdUpdate.HdrRawYOffset[2U])) {
        pInfo->CmdUpdate.HdrSetup = (UINT8)1U;
        pInfo->CmdUpdate.SetFovLayout= (UINT8)1U;
    }
    if (pInfo->LinearCe != pInfo->CmdUpdate.LinearCe) {
        pInfo->CmdUpdate.CeSetup = (UINT8)1U;
    }
    if (pInfo->InputFromMemory != pInfo->CmdUpdate.InputFromMemory) {
        pInfo->CmdUpdate.HdrSetup = (UINT8)1U;
    }
    if ((pInfo->CapWindow.OffsetX != pInfo->CmdUpdate.CapWindow.OffsetX) ||
        (pInfo->CapWindow.OffsetY != pInfo->CmdUpdate.CapWindow.OffsetY) ||
        (pInfo->CapWindow.Width != pInfo->CmdUpdate.CapWindow.Width) ||
        (pInfo->CapWindow.Height != pInfo->CmdUpdate.CapWindow.Height)) {
        pInfo->CmdUpdate.SetFovLayout= (UINT8)1U;
    }
}

static inline void HL_UptVprocOrder(const UINT16 MaxViewZoneNum,
                                    const UINT32 DisableViewZoneBitMask,
                                    const UINT32 GrpViewZoneBitMask,
                                    UINT8 DisableVproc,
                                    UINT16 *pNumVproc,
                                    UINT16 *ViewZoneOrder,
                                    UINT16 *ChanIdxArray)
{
    UINT16 i, NumVproc = *pNumVproc;

    /* Put active VZ first */
    for (i=0; i<MaxViewZoneNum; i++) {
        if (0U == DSP_GetBit(GrpViewZoneBitMask, i, 1U)) {
            continue;
        }
        if (1U == DSP_GetBit(DisableViewZoneBitMask, i, 1U)) {
            continue;
        }
//        AmbaLL_LogUInt5("HL_UptVprocOrder NumVproc:%u ChanId:%u",
//                NumVproc, i, 0U, 0U, 0U);
        ViewZoneOrder[NumVproc] = i;
        ChanIdxArray[NumVproc] = i;
        NumVproc++;
    }

    /* then disabled VZ */
    for (i=0; i<MaxViewZoneNum; i++) {
        if (1U == DSP_GetBit(DisableViewZoneBitMask, i, 1U)) {
//            AmbaLL_LogUInt5("HL_UptVprocOrder NumVproc:%u ChanId:%u",
//                    NumVproc, i, 0U, 0U, 0U);
            ViewZoneOrder[NumVproc] = i;
            ChanIdxArray[NumVproc] = i;
#ifdef SUPPORT_VPROC_DISABLE_BIT
            if (DisableVproc == 1U) {
                /* Disable vproc bit */
                DSP_SetU16Bit(&ViewZoneOrder[NumVproc], 7U);
            }
#else
            (void)DisableVproc;
#endif
            NumVproc++;
        }
    }
    *pNumVproc = NumVproc;

}

void HL_UpdateVprocChanProcOrder(UINT8 GrpID,
                                 UINT32 DisableViewZoneBitMask,
                                 cmd_vproc_multi_chan_proc_order_t* pChanProcOrder,
                                 UINT8 DisableVproc,
                                 UINT16 *ChanIdxArray)
{
    UINT8 *pVprocOrderAddr = NULL;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT16 i, ViewGrpIdx, NumVproc = 0U;
    UINT16 ViewZoneOrder[NUM_VPROC_MAX_CHAN] = {0};
    UINT32 IsGrpFreeRun;
    UINT32 GrpViewZoneBitMask = 0U;
    ULONG ULAddr;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    cmd_vproc_set_vproc_grping *VprocGroup = HL_DefCtxCmdBufPtrVpcGrp;

    (void)dsp_osal_memset(ViewZoneOrder, 0x0, (sizeof(UINT16)*NUM_VPROC_MAX_CHAN));
    HL_GetResourcePtr(&Resource);

    /* Find all the ViewZones in GrpID */
    for (i=0; i<Resource->MaxViewZoneNum; i++) {
        if (0U == DSP_GetBit(Resource->ViewZoneActiveBit, i, 1U)) {
            continue;
        }

        (void)HL_GetVprocGroupIdx(i, &ViewGrpIdx, 0/*IsStlProc*/);
        if (GrpID == ViewGrpIdx) {
            DSP_SetBit(&GrpViewZoneBitMask, i);
        }
    }

    /*
     * different vin has no effect will use free_run_grp,
     * no need to update VprocChanProcOrder in that case
     */
    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&VprocGroup, &CmdBufferAddr);
    (void)HL_FillVideoProcGrouping(VprocGroup);
    IsGrpFreeRun = DSP_GetBit(VprocGroup->free_run_grp, GrpID, 1U);
    HL_RelCmdBuffer(CmdBufferId);

//    AmbaLL_LogUInt5("HL_UpdateVprocChanProcOrder free_run_grp:0x%x IsGrpFreeRun:%u GrpViewZoneBitMask:%x DisableViewZoneBitMask:%x",
//            VprocGroup.free_run_grp, IsGrpFreeRun, GrpViewZoneBitMask, DisableViewZoneBitMask, 0U);

    if ((GrpViewZoneBitMask != DisableViewZoneBitMask) && (IsGrpFreeRun == 0U)) {
        HL_UptVprocOrder(Resource->MaxViewZoneNum,
                         DisableViewZoneBitMask,
                         GrpViewZoneBitMask,
                         DisableVproc,
                         &NumVproc,
                         ViewZoneOrder,
                         ChanIdxArray);

        pChanProcOrder->num_of_vproc_channel = NumVproc;
        pChanProcOrder->proc_order_check_en = 0U;
        pChanProcOrder->is_runtime_change = 1U;
        pChanProcOrder->grp_id = GrpID;
        for (i = 0; i < pChanProcOrder->num_of_vproc_channel; i++) {
            if (pChanProcOrder->num_of_vproc_channel < AMBA_DSP_MAX_VIEWZONE_NUM) {
                pChanProcOrder->proc_order[i] = (UINT8)ViewZoneOrder[i];
            }
            if (pChanProcOrder->num_of_vproc_channel < NUM_VPROC_MAX_CHAN) {
                HL_GetPointerToDspVprocOrder(GrpID, i, &pVprocOrderAddr);
                *pVprocOrderAddr = (UINT8)ViewZoneOrder[i];
            }
        }
        HL_GetPointerToDspVprocOrder(GrpID, 0U, &pVprocOrderAddr);
        dsp_osal_typecast(&ULAddr, &pVprocOrderAddr);
        (void)dsp_osal_virt2cli(ULAddr, &pChanProcOrder->proc_order_addr);

    } else if ((GrpViewZoneBitMask != DisableViewZoneBitMask) && (IsGrpFreeRun == 1U)) {
        //some viewzone alive, and it is free run, need to update master, no need to change order

        HL_UptVprocOrder(Resource->MaxViewZoneNum,
                         DisableViewZoneBitMask,
                         GrpViewZoneBitMask,
                         DisableVproc,
                         &NumVproc,
                         ViewZoneOrder,
                         ChanIdxArray);
    } else {
        //disabling all viewzone, no need to change order
    }

    return;
}

void HL_UpdateVprocChanProcOrderTout(const UINT8 GrpID,
                                     const UINT16 NewMasterViewZoneId,
                                     const UINT16 NumVproc,
                                     cmd_vproc_multi_chan_proc_order_t* pChanProcOrder)
{
    UINT8 GroupNum;
    UINT8 *pGroupNum = NULL;
    UINT8 *pVprocOrderAddr = NULL;
    UINT16 i, NewMasterChanIdx = 0U;
    UINT16 ViewZoneOrder[NUM_VPROC_MAX_CHAN] = {0};
    ULONG ULAddr;

    (void)dsp_osal_memset(ViewZoneOrder, 0x0, (sizeof(UINT16)*NUM_VPROC_MAX_CHAN));
    /* create channel idx as original */
    /* find new master chan idx */
    HL_GetPointerToDspVprocGrpNum(GrpID, &pGroupNum);
    if (pGroupNum != NULL) {
        GroupNum = *pGroupNum;
    } else {
        GroupNum = 1U;
    }
    for (i=0U; i<GroupNum; i++) {
        UINT8 *pViewzoneId = NULL;

        HL_GetPointerToDspVprocGrpOrd(GrpID, i, &pViewzoneId);
        ViewZoneOrder[i] = *pViewzoneId;
        if (*pViewzoneId == NewMasterViewZoneId) {
            NewMasterChanIdx = i;
        }
    }

    /* swap new master chan to idx0 */
    for (i=NewMasterChanIdx; i>0U; i--) {
        UINT16 TmpIdx = ViewZoneOrder[i];

        ViewZoneOrder[i] = ViewZoneOrder[i-1U];
        ViewZoneOrder[i-1U] = TmpIdx;
    }

    pChanProcOrder->num_of_vproc_channel = NumVproc;
    pChanProcOrder->proc_order_check_en = 0U;
    pChanProcOrder->is_runtime_change = 1U;
    pChanProcOrder->grp_id = GrpID;
    for (i = 0; i < NumVproc; i++) {
        if (NumVproc < AMBA_DSP_MAX_VIEWZONE_NUM) {
            pChanProcOrder->proc_order[i] = (UINT8)ViewZoneOrder[i];
        }
        if (NumVproc < NUM_VPROC_MAX_CHAN) {
            HL_GetPointerToDspVprocOrder(GrpID, i, &pVprocOrderAddr);
            *pVprocOrderAddr = (UINT8)ViewZoneOrder[i];
        }
    }
    HL_GetPointerToDspVprocOrder(GrpID, 0U, &pVprocOrderAddr);
    dsp_osal_typecast(&ULAddr, &pVprocOrderAddr);
    (void)dsp_osal_virt2cli(ULAddr, &pChanProcOrder->proc_order_addr);

    return;
}

UINT32 HL_ReqTimeLapseId(UINT16 StrmId, UINT16 *TimeLapseId) {
    UINT32 Rval = OK;
    UINT16 i, ExitLoop = 0U;
    CTX_TIMELAPSE_INFO_s TimeLapseInfo = {0};

    *TimeLapseId = DSP_TIMELAPSE_IDX_INVALID;
    for (i=0U; i<AMBA_DSP_MAX_STREAM_NUM; i++) {
        HL_GetTimeLapseInfo(HL_MTX_OPT_GET, i, &TimeLapseInfo);
        if (TimeLapseInfo.VirtVinId == DSP_VIRT_VIN_IDX_INVALID) {
            ExitLoop = 1U;
        } else if (TimeLapseInfo.Status == ENC_TIMELAPSE_STATUS_IDLE) {
            *TimeLapseId = i;
            ExitLoop = 1U;
            TimeLapseInfo.Status = ENC_TIMELAPSE_STATUS_CONFIG;
            TimeLapseInfo.StrmId = StrmId;
        } else {
            // TBD
        }
        HL_SetTimeLapseInfo(HL_MTX_OPT_SET, i, &TimeLapseInfo);
        if (ExitLoop == 1U) {
            break;
        }
    }
    if (*TimeLapseId == DSP_TIMELAPSE_IDX_INVALID) {
       AmbaLL_LogUInt5("Request TimeLapseId failed", 0U, 0U, 0U, 0U, 0U);
       Rval = DSP_ERR_0005;
    }
    return Rval;
}

UINT32 HL_FreeTimeLapseId(UINT16 TimeLapseId) {
    UINT32 Rval = OK;
    CTX_TIMELAPSE_INFO_s TimeLapseInfo = {0};

    if (TimeLapseId >= AMBA_DSP_MAX_STREAM_NUM) {
        Rval = DSP_ERR_0000;
    } else {
        HL_GetTimeLapseInfo(HL_MTX_OPT_GET, TimeLapseId, &TimeLapseInfo);
        if (TimeLapseInfo.Status == ENC_TIMELAPSE_STATUS_RUN) {
            AmbaLL_LogUInt5("Free TimeLapseId[%d] failed: status is running", TimeLapseId, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0004;
        } else {
            TimeLapseInfo.Status = ENC_TIMELAPSE_STATUS_IDLE;
            TimeLapseInfo.TotalIssuedMemEncodeNum = 0U;
        }
        HL_SetTimeLapseInfo(HL_MTX_OPT_SET, TimeLapseId, &TimeLapseInfo);
    }
    return Rval;
}

UINT32 HL_LiveviewFillVinExtMem(const UINT32 VinId,
                                const UINT32 IsYuvVin2Enc,
                                const UINT8 IsFirstMem,
                                const UINT8 MemType,
                                const UINT32 ChromaFmt,
                                const UINT32 OverFlowCtrl,
                                const AMBA_DSP_BUF_s *pBuf,
                                const AMBA_DSP_BUF_s *pAuxBuf,
                                cmd_vin_set_ext_mem_t *pVinExtMem)
{
    UINT16 RawBufferType;
    UINT32 *pBufTblAddr = NULL;
    UINT32 Rval = OK;
    ULONG ULAddr = 0U;

    pVinExtMem->vin_id_or_chan_id = (UINT8)VinId;
    pVinExtMem->memory_type = MemType;
    pVinExtMem->allocation_type = VIN_EXT_MEM_ALLOC_TYPE_DISTINCT;
    /* First time use vin_ext_mem, use allocation_mode=VIN_EXT_MEM_MODE_NEW
     * Other feed use VIN_EXT_MEM_MODE_APPEND */
    pVinExtMem->allocation_mode = (IsFirstMem == 1U) ? VIN_EXT_MEM_MODE_NEW : VIN_EXT_MEM_MODE_APPEND;
    pVinExtMem->overflow_cntl = (UINT8)OverFlowCtrl;
    pVinExtMem->num_frm_buf = MAX_EXT_MEM_BUF_NUM;
    if (pVinExtMem->num_frm_buf > EXT_MEM_VIN_MAX_NUM) {
        Rval = DSP_ERR_0000;
        AmbaLL_LogUInt5("HL_LiveviewFillVinExtMem Only %d ExtMem supported", __LINE__, EXT_MEM_VIN_MAX_NUM, 0U, 0U, 0U);
    }
    pVinExtMem->chroma_format = (UINT8)ChromaFmt;
    pVinExtMem->luma_img_ofs_x = 0U;
    pVinExtMem->luma_img_ofs_y = 0U;
    pVinExtMem->chroma_img_ofs_x = 0U;
    pVinExtMem->chroma_img_ofs_y = 0U;

    pVinExtMem->aux_pitch = 0U;
    pVinExtMem->aux_width = 0U;
    pVinExtMem->aux_height = 0U;
    pVinExtMem->aux_img_ofs_x = 0U;
    pVinExtMem->aux_img_ofs_y = 0U;
    pVinExtMem->aux_img_width = 0U;
    pVinExtMem->aux_img_height = 0U;

    pVinExtMem->buf_pitch = pBuf->Pitch;
    pVinExtMem->buf_width = pBuf->Window.Width;
    /* To make life easier, uCode only use Yuv422 buffer size */
    /* Per YK 20180719, BufH == ImgH when YuvFormat */
    if ((ChromaFmt != DSP_YUV_MONO) &&
        (pAuxBuf != NULL)) {
        pVinExtMem->buf_height = (UINT16)((pAuxBuf->BaseAddr - pBuf->BaseAddr)/pBuf->Pitch);
    } else {
        pVinExtMem->buf_height = pBuf->Window.Height;
    }
    pVinExtMem->img_width = pBuf->Window.Width;
    pVinExtMem->img_height = pBuf->Window.Height;

    if (pAuxBuf != NULL) {
        pVinExtMem->aux_pitch  = pAuxBuf->Pitch;
        pVinExtMem->aux_width  = pAuxBuf->Window.Width;
        pVinExtMem->aux_height = pAuxBuf->Window.Height;
    }
    if (IsYuvVin2Enc == 1U) {
        /*
         * vin_yuv_enc_fbp_init will allocate resource for virtual vin yuv input from ext_mem at vin_flow_max stage
         * Need to allocate resource (vin_yuv_enc_fbp_disable=0) when send ext_mem after vproc
         * */
#ifdef SUPPORT_DSP_VIN_YUV_ENC_FBP_CTRL
        pVinExtMem->vin_yuv_enc_fbp_disable = 0U;
#endif
    } else {
        /*
         * This is virtual vin feed into vproc case
         * vin_yuv_enc_fbp_init no need to allocate resource at vin_flow_max stage
         * No need to allocate resource (vin_yuv_enc_fbp_disable=1) when send ext_mem after vproc
         * */
#ifdef SUPPORT_DSP_VIN_YUV_ENC_FBP_CTRL
        pVinExtMem->vin_yuv_enc_fbp_disable = 1U;
#endif
    }

    if (MemType == DSP_VIN_EXT_MEM_TYPE_ME) {
        /*ME*/
        RawBufferType = 1U;
    } else {
        /*DSP_VIN_EXT_MEM_TYPE_YUV422, Raw*/
        RawBufferType = 0U;
    }

    if (VinId < DSP_VIN_MAX_NUM) {
        HL_GetPointerToDspExtRawBufArray((UINT16)VinId, RawBufferType, &pBufTblAddr);
        (void)dsp_osal_virt2cli(pBuf->BaseAddr, &pBufTblAddr[0U]);
        dsp_osal_typecast(&ULAddr, &pBufTblAddr);
    } else {
        Rval = DSP_ERR_0000;
    }
    (void)dsp_osal_virt2cli(ULAddr, &pVinExtMem->buf_addr);

    return Rval;
}
