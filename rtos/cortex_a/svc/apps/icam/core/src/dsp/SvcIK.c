/**
*  @file SvcIK.c
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
*  @details svc image kernel functions
*
*/

#include "AmbaMisraFix.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaDSP.h"
#include "AmbaSensor.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaShell.h"
#include "AmbaUtility.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcDSP.h"
#include "SvcIK.h"
#include "SvcLiveview.h"
#include "SvcVinSrc.h"
#include "SvcResCfg.h"

#define SVC_LOG_IK          "S-IK"

static AMBA_IK_CONTEXT_SETTING_s *pSvcIKSetting;

/**
* initialization of image kernel
* @param [in] pCtxSetting context setting of image kernel
* @return 0-OK, 1-NG
*/
UINT32 SvcIK_Init(AMBA_IK_CONTEXT_SETTING_s *pCtxSetting)
{
    pSvcIKSetting = pCtxSetting;

    return SVC_OK;
}

/**
* initialization of image kernel working buffer
* @param [in] WorkBase base of working buffer
* @param [in] MaxSize size of working buffer
* @return 0-OK, 1-NG
*/
UINT32 SvcIK_InitArch(ULONG WorkBase, UINT32 MaxSize)
{
    void    *pAddr;
    UINT32  Rval;
    SIZE_t  NeededSize = MaxSize;

    Rval = AmbaIK_QueryArchMemorySize(pSvcIKSetting, &NeededSize);
    if (OK == Rval) {
        if (((UINT32)NeededSize) <= MaxSize) {
            AmbaMisra_TypeCast(&pAddr, &WorkBase);

            if (0U < SvcDSP_IsCleanBoot()) {
                Rval |= AmbaIK_InitArch(pSvcIKSetting, pAddr, NeededSize);
            } else {
                extern UINT32 AmbaIK_InitArchSoft(const AMBA_IK_CONTEXT_SETTING_s *pSettingOfEachContext,
                                                  void *pMemAddr,
                                                  SIZE_t MemSize) GNU_WEAK_SYMBOL;
                typedef UINT32 (*PFN_IK_INITARCH_SOFT)(const AMBA_IK_CONTEXT_SETTING_s *pSettingOfEachContext,
                                                       void *pMemAddr,
                                                       SIZE_t MemSize);

                PFN_IK_INITARCH_SOFT pfnInitIKArchSoft = AmbaIK_InitArchSoft;

                if (pfnInitIKArchSoft != NULL) {
                    Rval |= pfnInitIKArchSoft(pSvcIKSetting, pAddr, NeededSize);
                }
            }
            Rval |= AmbaIK_InitPrintFunc();
            Rval |= AmbaIK_InitCheckParamFunc();
            if (OK != Rval) {
                SvcLog_NG(SVC_LOG_IK, "## fail to init IK", 0U, 0U);
            } else {
                SvcLog_DBG(SVC_LOG_IK, "IK is initialized", (UINT32)NeededSize, MaxSize);
                SvcLog_DBG(SVC_LOG_IK, "IK work size, needed/max(%u/%u)", (UINT32)NeededSize, MaxSize);
            }
        } else {
            Rval = SVC_NG;
            SvcLog_NG(SVC_LOG_IK, "## size isn't enough, need/max(%u/%u)", (UINT32)NeededSize, MaxSize);
        }
    } else {
        SvcLog_NG(SVC_LOG_IK, "## fail to query IK work size, return 0x%x", Rval, 0U);
    }

    return Rval;

}

/**
* initialization of image kernel context
* @return 0-OK, 1-NG
*/
UINT32 SvcIK_CtxInit(void)
{
    UINT32              i, Rval = SVC_NG;
    UINT32              VinID = 0U;
    AMBA_IK_MODE_CFG_s  ImgMode = {0};

    SVC_LIV_INFO_s LivInfo;
    AMBA_SENSOR_STATUS_INFO_s SensorStatus;
    AMBA_SENSOR_CHANNEL_s SensorChan;

    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();

    SvcLiveview_InfoGet(&LivInfo);

    for (i = 0; i < pSvcIKSetting->ContextNumber; i++) {
        if ((pSvcIKSetting->ConfigSetting[i].pAbility->VideoPipe != AMBA_IK_VIDEO_Y2Y) &&
#if defined(AMBA_IK_VIDEO_Y2Y_MIPI)
            (pSvcIKSetting->ConfigSetting[i].pAbility->VideoPipe != AMBA_IK_VIDEO_Y2Y_MIPI) &&
#endif
            (i < *(LivInfo.pNumFov))) {
            /* Get corresponding VinID from FovIdx */
            if (SVC_OK != SvcResCfg_GetVinIDOfFovIdx(LivInfo.pFovCfg[i].ViewZoneId, &VinID)) {
                SvcLog_NG(SVC_LOG_IK, "SvcIK_CtxInit err. SvcResCfg_GetVinIDOfFovIdx failed", 0U, 0U);
            }
            /* sensor chan */
            SensorChan.VinID = VinID;
            SensorChan.SensorID = 1U;
            /* sensor status */
            pSvcIKSetting->ConfigSetting[i].pAbility->VideoPipe = AMBA_IK_VIDEO_LINEAR;

            Rval = AmbaSensor_GetStatus(&SensorChan, &SensorStatus);
            if (Rval == SVC_OK) {
                /* hdr? */
                if (SensorStatus.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
                    /* dol hdr */
                    if (SensorStatus.ModeInfo.HdrInfo.ActiveChannels == 2U) {
                        pSvcIKSetting->ConfigSetting[i].pAbility->VideoPipe = AMBA_IK_VIDEO_HDR_EXPO_2;
                    }
#if defined(AMBA_IK_VIDEO_HDR_EXPO_3)
                    else {
                        pSvcIKSetting->ConfigSetting[i].pAbility->VideoPipe = AMBA_IK_VIDEO_HDR_EXPO_3;
                    }
#endif
                } else if (SensorStatus.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
                    if ((pCfg->FovCfg[i].PipeCfg.LinearCE & LINEAR_CE_FORCE_DISABLE) > 0U) {
                        /* Nothing to do. Keep original setting */
                    } else {
                        pSvcIKSetting->ConfigSetting[i].pAbility->VideoPipe = AMBA_IK_VIDEO_LINEAR_CE;
                    }
                } else {
                    if ((pCfg->FovCfg[i].PipeCfg.LinearCE & LINEAR_CE_FORCE_DISABLE) > 0U) {
                        /* Nothing to do. Keep original setting */
                    } else if ((pCfg->FovCfg[i].PipeCfg.LinearCE & LINEAR_CE_ENABLE) > 0U) {
                        /* Enable CE based on FovCfg */
                        pSvcIKSetting->ConfigSetting[i].pAbility->VideoPipe = AMBA_IK_VIDEO_LINEAR_CE;
                    } else {
                        /* nothing to do */
                    }
                }
            }
        }
        ImgMode.ContextId = i;

        if (pSvcIKSetting->ConfigSetting[i].pAbility->Pipe == AMBA_IK_PIPE_STILL) {
            AmbaPrint_PrintUInt5("SvcIK_CtxInit [%u] pipe %u stillpipe %u", i, pSvcIKSetting->ConfigSetting[i].pAbility->Pipe , pSvcIKSetting->ConfigSetting[i].pAbility->StillPipe, 0U, 0U);
        } else {
            AmbaPrint_PrintUInt5("SvcIK_CtxInit [%u] pipe %u videopipe %u", i, pSvcIKSetting->ConfigSetting[i].pAbility->Pipe , pSvcIKSetting->ConfigSetting[i].pAbility->VideoPipe, 0U, 0U);
        }

        Rval = AmbaIK_InitContext(&ImgMode, pSvcIKSetting->ConfigSetting[i].pAbility);
        if (OK != Rval) {
            break;
        }
    }

    if (OK != Rval) {
        SvcLog_NG(SVC_LOG_IK, "## fail to init context, rval(%u)", Rval, 0U);
    }

    return Rval;
}

/**
* check and configure raw compress. now, the system will check the raw compact type same sensor data bits or not
* @param [in] VinID vin id
* @param [in] RawCompression raw compress type
* @return configuration raw compress value
*/
UINT32 SvcIK_RawCompressConfig(UINT32 VinID, UINT32 RawCompression)
{
    UINT32 RetRawCmpr = RawCompression;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        SvcLog_NG(SVC_LOG_IK, "SvcIK_RawCompressConfig err. invalid VinID(%d)", VinID, 0U);
    } else if (pResCfg == NULL) {
        SvcLog_NG(SVC_LOG_IK, "SvcIK_RawCompressConfig err. invalid ResCfg", 0U, 0U);
    } else if (pResCfg->VinCfg[VinID].pDriver == NULL) {
        SvcLog_NG(SVC_LOG_IK, "SvcIK_RawCompressConfig err. VinID(%d) sensor driver should not null", VinID, 0U);
    } else {

        // If enable raw compact mechanism, check RawCompression match sensor's data bits or not.
        if (RetRawCmpr > 255U) {
            AMBA_SENSOR_CHANNEL_s     SensorChan;
            AMBA_SENSOR_CONFIG_s      SensorMode;
            AMBA_SENSOR_MODE_INFO_s   SensorModeInfo;
            AMBA_SENSOR_OBJ_s        *pSensorObj;
            UINT32                    SensorNum = 0U, SensorIdxs[AMBA_DSP_MAX_VIEWZONE_NUM];

            AmbaMisra_TypeCast(&(pSensorObj), &(pResCfg->VinCfg[VinID].pDriver));

            if (pSensorObj == NULL) {
                SvcLog_NG(SVC_LOG_IK, "SvcIK_RawCompressConfig err. invalid VinID(%d) sensor object", VinID, 0U);
            } else {

                AmbaSvcWrap_MisraMemset(&SensorChan, 0, sizeof(SensorChan));
                SensorChan.VinID = VinID;
                SensorChan.SensorID = 0x10U;

                AmbaSvcWrap_MisraMemset(&SensorIdxs, 0, sizeof(SensorIdxs));
                if (0U != SvcResCfg_GetSensorIdxsInVinID(VinID, SensorIdxs, &SensorNum)) {
                    SvcLog_NG(SVC_LOG_IK, "SvcIK_RawCompressConfig err. get VinID(%d) sensor index fail", VinID, 0U);
                } else if (SensorNum == 0U) {
                    SvcLog_NG(SVC_LOG_IK, "SvcIK_RawCompressConfig err. VinID(%d) sensor index number should not zero", VinID, 0U);
                } else if (pSensorObj->GetModeInfo == NULL) {
                    SvcLog_NG(SVC_LOG_IK, "SvcIK_RawCompressConfig err. invalid sensor obj", 0U, 0U);
                } else {

                    AmbaSvcWrap_MisraMemset(&SensorMode, 0, sizeof(SensorMode));
                    SensorMode.ModeID = pResCfg->SensorCfg[VinID][SensorIdxs[0]].SensorMode;

                    AmbaSvcWrap_MisraMemset(&SensorModeInfo, 0, sizeof(SensorModeInfo));
                    if (0U != (pSensorObj->GetModeInfo)(&SensorChan, &SensorMode, &SensorModeInfo)) {
                        SvcLog_NG(SVC_LOG_IK, "SvcIK_RawCompressConfig err. Get sensor mode info fail", VinID, 0U);
                    } else {
                        switch(SensorModeInfo.OutputInfo.NumDataBits) {
#ifdef IK_RAW_COMPACT_8B
                        case 8U :
                            if (RawCompression != (UINT32)IK_RAW_COMPACT_8B) {
                                SvcLog_DBG(SVC_LOG_IK, "Correct raw compact type %d -> %d", RawCompression, (UINT32)IK_RAW_COMPACT_8B);
                                RetRawCmpr = (UINT32)IK_RAW_COMPACT_8B;
                            }
                            break;
#endif
                        case 10U :
                            if (RawCompression != (UINT32)IK_RAW_COMPACT_10B) {
                                SvcLog_DBG(SVC_LOG_IK, "Correct raw compact type %d -> %d", RawCompression, (UINT32)IK_RAW_COMPACT_10B);
                                RetRawCmpr = (UINT32)IK_RAW_COMPACT_10B;
                            }
                            break;

                        case 12U :
                            if (RawCompression != (UINT32)IK_RAW_COMPACT_12B) {
                                SvcLog_DBG(SVC_LOG_IK, "Correct raw compact type %d -> %d", RawCompression, (UINT32)IK_RAW_COMPACT_12B);
                                RetRawCmpr = (UINT32)IK_RAW_COMPACT_12B;
                            }
                            break;

                        case 14U :
                            if (RawCompression != (UINT32)IK_RAW_COMPACT_14B) {
                                SvcLog_DBG(SVC_LOG_IK, "Correct raw compact type %d -> %d", RawCompression, (UINT32)IK_RAW_COMPACT_14B);
                                RetRawCmpr = (UINT32)IK_RAW_COMPACT_14B;
                            }
                            break;

                        default :
                            SvcLog_NG(SVC_LOG_IK, "SvcIK_RawCompressConfig err. not catch supported raw compact type. use original setting", 0U, 0U);
                            break;
                        }
                    }
                }
            }

            AmbaMisra_TouchUnused(pSensorObj);
        }
    }

    return RetRawCmpr;
}

/**
* sensor configuration of image kernel
* @param [in] pImgMode block of image kernel context
* @return 0-OK, 1-NG
*/
UINT32 SvcIK_SensorConfig(const AMBA_IK_MODE_CFG_s *pImgMode)
{
    UINT32                     RetVal = SVC_OK;
    UINT32                     VinID = 0U, VinSrc, FovIdx, SensorID;
    AMBA_SENSOR_STATUS_INFO_s  SensorStat;
    AMBA_SENSOR_CHANNEL_s      SensorChan = {0};
    AMBA_IK_ABILITY_s          Ability = {0};
    AMBA_IK_VIN_SENSOR_INFO_s  SensorInfo = {0};
    const SVC_RES_CFG_s        *pCfg = SvcResCfg_Get();

    FovIdx = pImgMode->ContextId;
    if (0U != SvcResCfg_GetVinIDOfFovIdx(FovIdx, &VinID)) {
        SvcLog_NG(SVC_LOG_IK, "## fail to get VinID by FovIdx(%d)", FovIdx, 0U);
        RetVal = SVC_NG;
    } else if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        SvcLog_NG(SVC_LOG_IK, "## invalid VinID(%d)", VinID, 0U);
        RetVal = SVC_NG;
    } else if (0U != SvcResCfg_GetVinSrc(VinID, &VinSrc)) {
        SvcLog_NG(SVC_LOG_IK, "## fail to get VinID(%d) source", VinID, 0U);
        RetVal = SVC_NG;
    } else if (0U != SvcResCfg_GetSensorIDInVinID(VinID, &SensorID)) {
        SvcLog_NG(SVC_LOG_IK, "## fail to get SensorID by VinID(%d)", VinID, 0U);
        RetVal = SVC_NG;
    } else if (0U != AmbaIK_GetContextAbility(pImgMode, &Ability)) {
        SvcLog_NG(SVC_LOG_IK, "## fail to get IK ability", 0U, 0U);
        RetVal = SVC_NG;
    } else {

#if defined(AMBA_IK_VIDEO_Y2Y_MIPI)
        if ((Ability.VideoPipe != AMBA_IK_VIDEO_Y2Y) && (Ability.VideoPipe != AMBA_IK_VIDEO_Y2Y_MIPI)) {
#else
            if (Ability.VideoPipe != AMBA_IK_VIDEO_Y2Y) {
#endif
            UINT32 BayerPattern = 0U;
            UINT32 SensorType = 0U;
            UINT32 SensorPattern = 0U;

            SensorChan.VinID = VinID;
            SensorChan.SensorID = SensorID;

            RetVal = AmbaSensor_GetStatus(&SensorChan, &SensorStat);
            if (RetVal != 0U) {
                SvcLog_NG(SVC_LOG_IK, "## fail to get sensor status", 0U, 0U);
            } else {
                BayerPattern = SensorStat.ModeInfo.OutputInfo.BayerPattern;
                /* Determine SensorType */
                if (pCfg->FovCfg[FovIdx].PipeCfg.ForceSensorType != 0U) {
                    SensorType = pCfg->FovCfg[FovIdx].PipeCfg.ForceSensorType & 0xFU;
                } else {
                    switch (BayerPattern & 0xF0U) {
                        case 0x00:
                            SensorType = SVC_IK_SENSOR_RGB;
                        break;
                        case 0x10:
                            SensorType = SVC_IK_SENSOR_RGB_IR;
                        break;
                        case 0x20:
                            SensorType = SVC_IK_SENSOR_RCCC;
                        break;
                        default:
                            SensorType = SVC_IK_SENSOR_RGB;
                            SvcLog_NG(SVC_LOG_IK, "Unknown Bayer Pattern (0x%x). Please check sensor driver", BayerPattern, 0U);
                        break;
                    }
                }

                /* Determine BayerPattern based on different sensor type refering to DGG */
                switch (BayerPattern & 0x0FU) {
                    case 0x00:
                    case 0x04:
                        SensorPattern = 0U;
                    break;
                    case 0x01:
                    case 0x05:
                        SensorPattern = 1U;
                    break;
                    case 0x02:
                    case 0x06:
                        SensorPattern = 2U;
                    break;
                    case 0x03:
                    case 0x07:
                        SensorPattern = 3U;
                    break;
                    default:
                        SensorPattern = 0U;
                        SvcLog_NG(SVC_LOG_IK, "Unknown Sensor Pattern (0x%x). Please check sensor driver", BayerPattern, 0U);
                    break;
                }

                /* set sensor info */
                SensorInfo.VinId             = VinID;
                SensorInfo.SensorResolution  = SensorStat.ModeInfo.OutputInfo.NumDataBits;
                SensorInfo.SensorPattern     = SensorPattern;
                SensorInfo.SensorMode        = SensorType;          /*0: normal, 1: RGB-IR, 2: RCCC */
                SensorInfo.Compression       = SvcIK_RawCompressConfig(VinID, pCfg->FovCfg[FovIdx].PipeCfg.RawCompression);
                if (VinSrc == SVC_VIN_SRC_MEM) {
                    // Video raw encode only support un-compression raw type
                    SensorInfo.Compression = 0U;
                }

                SensorInfo.CompressionOffset = 0;
                SensorInfo.SensorReadoutMode = 0x0U;

                if (0U < SvcDSP_IsCleanBoot()) {
                    RetVal = AmbaIK_SetVinSensorInfo(pImgMode, &SensorInfo);
                    if (RetVal != OK) {
                        SvcLog_NG(SVC_LOG_IK, "## fail to set sensor info", 0U, 0U);
                    }
                }

                if (SensorType == SVC_IK_SENSOR_RGB_IR) {
#if defined(AMBA_SENSOR_BAYER_PATTERN_RGGI)
                    UINT32            RgbIrMode = 0U;
                    AMBA_IK_RGB_IR_s  IkRgbIr = {0};

                    switch (BayerPattern) {
                        case AMBA_SENSOR_BAYER_PATTERN_RGGI:
                        case AMBA_SENSOR_BAYER_PATTERN_IGGR:
                        case AMBA_SENSOR_BAYER_PATTERN_GRIG:
                        case AMBA_SENSOR_BAYER_PATTERN_GIRG:
                            RgbIrMode = 1U;     /* Look at 2x2 quad square of source CFA pixel. And it has red pixel and IR */
                        break;
                        case AMBA_SENSOR_BAYER_PATTERN_BGGI:
                        case AMBA_SENSOR_BAYER_PATTERN_IGGB:
                        case AMBA_SENSOR_BAYER_PATTERN_GBIG:
                        case AMBA_SENSOR_BAYER_PATTERN_GIBG:
                            RgbIrMode = 2U;     /* Look at 2x2 quad square of source CFA pixel. And it has blue pixel and IR */
                        break;
                        default:
                            SvcLog_NG(SVC_LOG_IK, "Mismatch BayerPattern (0x%x) and RGB-IR sensor", BayerPattern, 0U);
                            RgbIrMode = 0U;
                        break;
                    }
                    IkRgbIr.Mode = RgbIrMode;

                    if (0U < SvcDSP_IsCleanBoot()) {
                        RetVal = AmbaIK_SetRgbIr(pImgMode, &IkRgbIr);
                        if (RetVal != OK) {
                            SvcLog_NG(SVC_LOG_IK, "## fail to set RgbIr info", 0U, 0U);
                        }
                    }
#else
                    SvcLog_NG(SVC_LOG_IK, "## don't support SENSOR_RGB_IR", 0U, 0U);
#endif
                }
            }
        }
#if defined(CONFIG_ICAM_PIPE_PIPVIN_YUV_SUPPORTED)
        else {
            if (AMBA_IK_VIDEO_Y2Y_MIPI == Ability.VideoPipe) {
                SensorInfo.VinId       = VinID;
                SensorInfo.Compression = pCfg->FovCfg[FovIdx].PipeCfg.RawCompression;

                if (0U < SvcDSP_IsCleanBoot()) {
                    RetVal = AmbaIK_SetVinSensorInfo(pImgMode, &SensorInfo);
                    if (RetVal != OK) {
                        SvcLog_NG(SVC_LOG_IK, "## fail to set sensor info", 0U, 0U);
                    }
                }
            }
        }
#endif
    }
    return RetVal;
}

/**
* fov configuration of image kernel
* @param [in] pImgMode block of image kernel context
* @param [in] pWin window of fov
* @return 0-OK, 1-NG
*/
UINT32 SvcIK_FovWinConfig(const AMBA_IK_MODE_CFG_s *pImgMode, const SVC_IK_FOV_WIN_s *pWin)
{
    UINT32                          Rval = SVC_OK;
    UINT32                          VinSrc = 0U;
    AMBA_SENSOR_STATUS_INFO_s       SensorStat;
    AMBA_SENSOR_CONFIG_s            SensorConfig;
    AMBA_SENSOR_MODE_INFO_s        *pSensorModeInfo = &(SensorStat.ModeInfo);
    AMBA_IK_WINDOW_SIZE_INFO_s      SizeInfo = {0};
    AMBA_IK_VIN_ACTIVE_WINDOW_s     ActWin = {0};
    AMBA_IK_ABILITY_s               Ability = {0};
    const AMBA_SENSOR_INPUT_INFO_s  *pSsInput;
    const SVC_RES_CFG_s             *pResCfg = SvcResCfg_Get();

    AmbaSvcWrap_MisraMemset(&SensorStat, 0, sizeof(SensorStat));
    AmbaSvcWrap_MisraMemset(&SensorConfig, 0, sizeof(SensorConfig));

    if (pWin == NULL) {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_IK, "## pWin == NULL", 0U, 0U);
    } else {

        Rval = AmbaIK_GetContextAbility(pImgMode, &Ability);
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_IK, "## fail to AmbaIK_GetContextAbility", 0U, 0U);
        }

        Rval = SvcResCfg_GetVinSrc(pWin->Chan.VinID, &VinSrc);
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_IK, "## fail to SvcResCfg_GetVinSrc", 0U, 0U);
        }

#if defined(AMBA_IK_VIDEO_Y2Y_MIPI)
        if ((Ability.VideoPipe != AMBA_IK_VIDEO_Y2Y) && (Ability.VideoPipe != AMBA_IK_VIDEO_Y2Y_MIPI)) {
#else
        if (Ability.VideoPipe != AMBA_IK_VIDEO_Y2Y) {
#endif
            Rval = AmbaSensor_GetStatus(&(pWin->Chan), &SensorStat);
            if (Rval == SVC_OK) {
                SensorConfig.ModeID = SensorStat.ModeInfo.Config.ModeID;
                AmbaSvcWrap_MisraMemset(pSensorModeInfo, 0, sizeof(AMBA_SENSOR_MODE_INFO_s));
                Rval = AmbaSensor_GetModeInfo(&(pWin->Chan), &SensorConfig, pSensorModeInfo);
            }
        }

        if (Rval == SVC_OK) {
            /* set window size */
            SizeInfo.VinSensor.StartX               = pWin->Raw.OffsetX;
            SizeInfo.VinSensor.StartY               = pWin->Raw.OffsetY;
            SizeInfo.VinSensor.Width                = pWin->Raw.Width;
            SizeInfo.VinSensor.Height               = pWin->Raw.Height;
            SizeInfo.Main.Width                     = pWin->Main.Width;
            SizeInfo.Main.Height                    = pWin->Main.Height;
#if defined(AMBA_IK_VIDEO_Y2Y_MIPI)
            if ((Ability.VideoPipe != AMBA_IK_VIDEO_Y2Y) && (Ability.VideoPipe != AMBA_IK_VIDEO_Y2Y_MIPI)) {
#else
            if (Ability.VideoPipe != AMBA_IK_VIDEO_Y2Y) {
#endif
                UINT16 TempVal;
                UINT32 FactorDen, FactorNum;

                if ((pWin->ConCatNum > 1U) && (pWin->Option == AMBA_DSP_VIN_CAP_OPT_PROG)) {
                    TempVal = ( pWin->Raw.OffsetX / pSensorModeInfo->OutputInfo.RecordingPixels.Width ) * pSensorModeInfo->OutputInfo.RecordingPixels.Width;
                    TempVal = pWin->Raw.OffsetX - TempVal;
                } else {
                    TempVal = ( ( pWin->Raw.OffsetX + pWin->Cap.OffsetX ) - pSensorModeInfo->OutputInfo.RecordingPixels.StartX );
                }

                FactorDen = pSensorModeInfo->InputInfo.HSubsample.FactorDen; FactorDen &= 0xFU;
                FactorNum = pSensorModeInfo->InputInfo.HSubsample.FactorNum; FactorNum &= 0xFU;
                SizeInfo.VinSensor.StartX = TempVal;
                SizeInfo.VinSensor.StartX *= FactorDen;
                SizeInfo.VinSensor.StartX /= FactorNum;
                SizeInfo.VinSensor.StartX += pSensorModeInfo->InputInfo.PhotodiodeArray.StartX;

                if ((pWin->ConCatNum > 1U) && (pWin->Option == AMBA_DSP_VIN_CAP_OPT_INTC)) {
                    TempVal = ( ( ( pWin->Raw.OffsetY + pWin->Cap.OffsetY ) / (UINT16)(pWin->ConCatNum) ) - pSensorModeInfo->OutputInfo.RecordingPixels.StartY );
                } else {
                    TempVal = ( ( pWin->Raw.OffsetY + pWin->Cap.OffsetY ) - pSensorModeInfo->OutputInfo.RecordingPixels.StartY );
                }

                FactorDen = pSensorModeInfo->InputInfo.VSubsample.FactorDen; FactorDen &= 0xFU;
                FactorNum = pSensorModeInfo->InputInfo.VSubsample.FactorNum; FactorNum &= 0xFU;
                SizeInfo.VinSensor.StartY = TempVal;
                SizeInfo.VinSensor.StartY *= FactorDen;
                SizeInfo.VinSensor.StartY /= FactorNum;
                SizeInfo.VinSensor.StartY += pSensorModeInfo->InputInfo.PhotodiodeArray.StartY;
            }

#if defined(CONFIG_ICAM_CFA_WIN_SUPPORTED)
            if (pWin->IsCfaCustom == 1U) {
                SizeInfo.MaxCfaWindow.Width  = pWin->Cfa.Width;
                SizeInfo.MaxCfaWindow.Height = pWin->Cfa.Height;
            } else {
                SizeInfo.MaxCfaWindow.Width  = pWin->Raw.Width;
                SizeInfo.MaxCfaWindow.Height = pWin->Raw.Height;
            }
#endif

#if defined(AMBA_IK_VIDEO_Y2Y_MIPI)
            if ((Ability.VideoPipe != AMBA_IK_VIDEO_Y2Y) && (Ability.VideoPipe != AMBA_IK_VIDEO_Y2Y_MIPI)) {
#else
            if (Ability.VideoPipe != AMBA_IK_VIDEO_Y2Y) {
#endif
                pSsInput = &(SensorStat.ModeInfo.InputInfo);
                SizeInfo.VinSensor.HSubSample.FactorDen = pSsInput->HSubsample.FactorDen;
                SizeInfo.VinSensor.HSubSample.FactorNum = pSsInput->HSubsample.FactorNum;
                SizeInfo.VinSensor.VSubSample.FactorDen = pSsInput->VSubsample.FactorDen;
                SizeInfo.VinSensor.VSubSample.FactorNum = pSsInput->VSubsample.FactorNum;

                if (pResCfg != NULL) {
                    if (pImgMode->ContextId < AMBA_DSP_MAX_VIEWZONE_NUM) {
                        if ((pResCfg->FovCfg[pImgMode->ContextId].VirtChan.Enable & SVC_RES_PIPE_VIRTUAL_CHAN_INPUT_MUX_SEL) > 0U) {
                            SizeInfo.VinSensor.HSubSample.FactorDen *= (pResCfg->FovCfg[pImgMode->ContextId].VirtChan.InputSubsample & 0xFFFF0000U) >> 16U;
                            SizeInfo.VinSensor.VSubSample.FactorDen *= (pResCfg->FovCfg[pImgMode->ContextId].VirtChan.InputSubsample & 0xFFFFU);
                        }
                    }
                }
            } else {
                SizeInfo.VinSensor.HSubSample.FactorDen = 1U;
                SizeInfo.VinSensor.HSubSample.FactorNum = 1U;
                SizeInfo.VinSensor.VSubSample.FactorDen = 1U;
                SizeInfo.VinSensor.VSubSample.FactorNum = 1U;
            }

            if (0U < SvcDSP_IsCleanBoot()) {
                Rval = AmbaIK_SetWindowSizeInfo(pImgMode, &SizeInfo);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_IK, "## fail to set window size info", 0U, 0U);
                }
            }

            /* set active window */
#if defined(AMBA_IK_VIDEO_Y2Y_MIPI)
            if ((Ability.VideoPipe != AMBA_IK_VIDEO_Y2Y) && (Ability.VideoPipe != AMBA_IK_VIDEO_Y2Y_MIPI)) {
#else
            if (Ability.VideoPipe != AMBA_IK_VIDEO_Y2Y) {
#endif
                ActWin.Enable           = 0U;
                ActWin.ActiveGeo.StartX = pWin->Raw.OffsetX;
                ActWin.ActiveGeo.StartY = pWin->Raw.OffsetY;
                ActWin.ActiveGeo.Width  = pWin->Raw.Width;
                ActWin.ActiveGeo.Height = pWin->Raw.Height;
                Rval = AmbaIK_SetVinActiveWin(pImgMode, &ActWin);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_IK, "## fail to set active window", 0U, 0U);
                }
            }
        }
    }

    return Rval;
}

/**
* warp control of image kernel
* @param [in] pImgMode block of image kernel context
* @param [in] Enable enable or disable
* @return 0-OK, 1-NG
*/
UINT32 SvcIK_ImgWarpCtrl(const AMBA_IK_MODE_CFG_s *pImgMode, UINT32 Enable)
{
    UINT32  Rval = OK;

#if defined(AMBA_IK_VIDEO_Y2Y_MIPI)
    AMBA_IK_ABILITY_s               Ability = {0};
    Rval = AmbaIK_GetContextAbility(pImgMode, &Ability);

    if (Ability.VideoPipe != AMBA_IK_VIDEO_Y2Y_MIPI)
#endif
    {
        Rval = AmbaIK_SetWarpEnb(pImgMode, Enable);
        if (Rval == OK) {
            Rval = AmbaIK_SetCawarpEnb(pImgMode, Enable);
        }
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_IK, "## fail to control(%u) warp, rval(%u)", Enable, Rval);
        }
    }

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    /* set warp buffer info */
    {
        AMBA_IK_WARP_BUFFER_INFO_s  WarpBufInfo;
        const SVC_RES_CFG_s         *pCfg = SvcResCfg_Get();

        /* DramEfficiency:                                      */
        /*  0 = not dram efficiency mode                        */
        /*  1 = low delay and wait-line through dram            */
        /*  2 = low delay and wait-line through smem            */
        /* LumaDmaSize:                                         */
        /*  [8:64] consuming smem                               */
        /* LumaWaitLines:                                       */
        /*  [8:8192] consuming smem or dram, only meaningful    */
        /*           in dram efficiency mode = low delay        */

        WarpBufInfo.LumaDmaSize = 24U;
        if (pCfg->FovCfg[pImgMode->ContextId].PipeCfg.PipeMode == SVC_VDOPIPE_DRAMEFCY) {
            WarpBufInfo.DramEfficiency = 2U;
            WarpBufInfo.LumaWaitLines  = pCfg->FovCfg[pImgMode->ContextId].PipeCfg.WarpLumaWaitLine;
        } else {
            WarpBufInfo.DramEfficiency = 0U;
            WarpBufInfo.LumaWaitLines  = 8U;
        }

        if (0U < SvcDSP_IsCleanBoot()) {
            if (AmbaIK_SetWarpBufferInfo(pImgMode, &WarpBufInfo) != OK) {
                SvcLog_NG(SVC_LOG_IK, "## fail to set warp_buf_info", 0U, 0U);
            }
        }
    }
#endif

    return Rval;
}

/**
* dzoom control of image kernel
* @param [in] pImgMode block of image kernel context
* @param [in] pDzoom block of dzoom
* @return 0-OK, 1-NG
*/
UINT32 SvcIK_ImgDzoomCtrl(const AMBA_IK_MODE_CFG_s *pImgMode, const AMBA_IK_DZOOM_INFO_s *pDzoom)
{
    UINT32  Rval = OK;
#if defined(AMBA_IK_VIDEO_Y2Y_MIPI)
    AMBA_IK_ABILITY_s               Ability = {0};
    Rval = AmbaIK_GetContextAbility(pImgMode, &Ability);

    if (Ability.VideoPipe != AMBA_IK_VIDEO_Y2Y_MIPI)
#endif
    {
        Rval = AmbaIK_SetDzoomInfo(pImgMode, pDzoom);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_IK, "## fail to control dzoom, rval(%u)", Rval, 0U);
        }
    }

    return Rval;
}

/**
* size query of image kernel working buffer
* @param [out] pNeededSize buffer size
* @return 0-OK, 1-NG
*/
UINT32 SvcIK_QueryArchMemSize(UINT32 *pNeededSize)
{
    SIZE_t QuerySize = 0U;
    UINT32 Rval;

    if (pSvcIKSetting != NULL) {
        Rval = AmbaIK_QueryArchMemorySize(pSvcIKSetting, &QuerySize);
        if (SVC_OK == Rval) {
            *pNeededSize = (UINT32) QuerySize;
        } else{
            *pNeededSize = 0;
            Rval = SVC_NG;
        }
    } else {
        AMBA_IK_CONTEXT_SETTING_s SvcIkDefSetting;
        AMBA_IK_ABILITY_s         SvcIkDefAbility[MAX_CONTEXT_NUM];
        UINT32 Idx, MaxIkCtxNum, MaxIkCfgNum;

#ifdef CONFIG_ICAM_IK_MAX_CTX_NUM
        MaxIkCtxNum = CONFIG_ICAM_IK_MAX_CTX_NUM;
        AmbaMisra_TouchUnused(&MaxIkCtxNum);
        if (MaxIkCtxNum > MAX_CONTEXT_NUM) {
            MaxIkCtxNum = MAX_CONTEXT_NUM;
        }
#else
        MaxIkCtxNum = MAX_CONTEXT_NUM;
#endif

#ifdef CONFIG_ICAM_IK_MAX_CFG_NUM
        MaxIkCfgNum = CONFIG_ICAM_IK_MAX_CFG_NUM;
        AmbaMisra_TouchUnused(&MaxIkCfgNum);
#if defined(MAX_CONFIG_NUM)
        if (MaxIkCfgNum > MAX_CONFIG_NUM) {
            MaxIkCfgNum = MAX_CONFIG_NUM;
        }
#elif defined(MAX_CR_RING_NUM)
        if (MaxIkCfgNum > MAX_CR_RING_NUM) {
            MaxIkCfgNum = MAX_CR_RING_NUM;
        }
#endif
#else
#if defined(MAX_CONFIG_NUM)
        MaxIkCfgNum = MAX_CONFIG_NUM;
#elif defined(MAX_CR_RING_NUM)
        MaxIkCfgNum = MAX_CR_RING_NUM;
#endif
#endif

        if (SVC_OK != AmbaWrap_memset(&SvcIkDefSetting, 0, sizeof(AMBA_IK_CONTEXT_SETTING_s))) {
            SvcLog_NG(SVC_LOG_IK, "SvcIK_QueryArchMemSize() err, AmbaWrap_memset failed, AMBA_IK_CONTEXT_SETTING_s", 0U, 0U);
        }
        if (SVC_OK != AmbaWrap_memset(SvcIkDefAbility, 0, sizeof(SvcIkDefAbility))) {
            SvcLog_NG(SVC_LOG_IK, "SvcIK_QueryArchMemSize() err, AmbaWrap_memset failed, SvcIkDefAbility", 0U, 0U);
        }

        for (Idx = 0U; Idx < MaxIkCtxNum; Idx ++) {
            SvcIkDefAbility[Idx].Pipe      = AMBA_IK_PIPE_VIDEO;
            SvcIkDefAbility[Idx].VideoPipe = AMBA_IK_VIDEO_LINEAR;
            SvcIkDefAbility[Idx].StillPipe = AMBA_IK_STILL_MAX;
            SvcIkDefSetting.ConfigSetting[Idx].ConfigNumber = MaxIkCfgNum;
            SvcIkDefSetting.ConfigSetting[Idx].pAbility     = &(SvcIkDefAbility[Idx]);
            SvcIkDefSetting.ContextNumber += 1U;
        }

#ifndef CONFIG_SOC_CV2FS
        if (MaxIkCtxNum >= 1U) {
            /* for still HISO */
            Idx = MaxIkCtxNum - 1U;
            SvcIkDefAbility[Idx].Pipe      = AMBA_IK_PIPE_STILL;
            SvcIkDefAbility[Idx].StillPipe = AMBA_IK_STILL_HISO;
        } else {
            SvcLog_NG(SVC_LOG_IK, "MaxIkCtxNum %u not enought for still hiso", MaxIkCtxNum, 0U);
        }
#endif

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        if (MaxIkCtxNum >= 2U) {
            /* for still LISO */
            Idx = MaxIkCtxNum - 2U;
            SvcIkDefAbility[Idx].Pipe      = AMBA_IK_PIPE_STILL;
            SvcIkDefAbility[Idx].StillPipe = AMBA_IK_STILL_LISO;
        } else {
            SvcLog_NG(SVC_LOG_IK, "MaxIkCtxNum %u not enought for still liso with CE", MaxIkCtxNum, 0U);
        }
        if (MaxIkCtxNum >= 3U) {
            /* for still LISO */
            Idx = MaxIkCtxNum - 3U;
            SvcIkDefAbility[Idx].Pipe      = AMBA_IK_PIPE_VIDEO;
            SvcIkDefAbility[Idx].StillPipe = AMBA_IK_VIDEO_LINEAR;
        } else {
            SvcLog_NG(SVC_LOG_IK, "MaxIkCtxNum %u not enought for still liso without CE", MaxIkCtxNum, 0U);
        }
#endif

        for (Idx = 0U; Idx < MaxIkCtxNum; Idx ++) {
            if (SvcIkDefAbility[Idx].Pipe  == AMBA_IK_PIPE_STILL) {
                AmbaPrint_PrintUInt5("SvcIK_QueryArchMemSize [%u] pipe %u stillpipe %u", Idx, SvcIkDefAbility[Idx].Pipe, SvcIkDefAbility[Idx].StillPipe, 0U, 0U);
            } else {
                AmbaPrint_PrintUInt5("SvcIK_QueryArchMemSize [%u] pipe %u videopipe %u", Idx, SvcIkDefAbility[Idx].Pipe, SvcIkDefAbility[Idx].VideoPipe, 0U, 0U);
            }
        }

        Rval = AmbaIK_QueryArchMemorySize(&SvcIkDefSetting, &QuerySize);
        if (SVC_OK == Rval) {
            *pNeededSize = (UINT32) QuerySize;
        } else{
            *pNeededSize = 0;
            Rval = SVC_NG;
        }
    }

    if (Rval == SVC_NG) {
        SvcLog_NG(SVC_LOG_IK, "SvcIK_QueryArchMemSize failed", Rval, 0U);
    } else {
        SvcLog_DBG(SVC_LOG_IK, "SvcIK_QueryArchMemSize %d", (UINT32) QuerySize, 0U);
    }

    return Rval;
}
