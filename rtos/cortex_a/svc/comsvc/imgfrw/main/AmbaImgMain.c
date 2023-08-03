/**
 *  @file AmbaImgMain.c
 *
 *  Copyright (c) [2020] Ambarella International LP
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
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
 *  @details Amba Image Main
 *
 */

#include "AmbaTypes.h"
#include "AmbaVIN_Def.h"
#include "AmbaKAL.h"

#include "AmbaDSP_ImageDefine.h"
#include "AmbaDSP_ImageFilter.h"

#include "AmbaSensor.h"

#include "AmbaImg_External_CtrlFunc.h"
#include "AmbaIQParamHandlerSample.h"
#include "Amba_Image.h"
#include "Amba_AeAwbAdj_Control.h"

#include "AmbaImgFramework.h"
#include "AmbaImgMain.h"
#include "AmbaImgMain_Internal.h"

#include "AmbaImgMainStatistics.h"
#include "AmbaImgMainSync.h"
#include "AmbaImgMainAe.h"
#include "AmbaImgMainAe_Platform.h"
#include "AmbaImgMainAwb.h"
#include "AmbaImgMainAdj.h"
#include "AmbaImgMainAf.h"
#include "AmbaImgMainAvm.h"
#include "AmbaImgMainFov.h"
#include "AmbaImgMainEis.h"
//#include "AmbaImgSensorDrv_Chan.h"
#ifdef CONFIG_BUILD_IMGFRW_EFOV
#include "AmbaImgMainEFov.h"
#endif
#ifndef AMBA_SHELL_H
#include "AmbaShell.h"
#endif
#include "AmbaImgFrwCmdApp_Def.h"

#include "AmbaImgFrwCmdApp.h"
#include "AmbaImgFrwCmdFlow.h"

#define OK_UL   ((UINT32) 0U)
#define NG_UL   ((UINT32) 1U)

typedef union /*_AMBA_IMG_MEM_INFO_u_*/ {
    AMBA_IMG_MAIN_MEM_ADDR               Data;
    void                                 *pVoid;
    AMBA_IMG_MAIN_ALGO_MSG_s             *pAlgoMsg;
    AMBA_IMG_MAIN_AE_USER_DATA_s         *pUser;
    AMBA_AE_INFO_s                       *pAeInfo;
#ifdef CONFIG_BUILD_IMGFRW_RAW_ENC
    AMBA_IMGPROC_OFFLINE_AAA_INFO_s      *pAaaInfo;
    AMBA_IMG_SYSTEM_RING_PIPE_s          *pRingPipe;
    AMBA_IMG_MAIN_ALGO_FOV_PIPE_s        *pFovPipe;
#endif
    UINT32                               *pUint32;
    AMBA_IK_MODE_CFG_s                   *pImgMode;
    AMBA_IK_CFA_3A_DATA_s                *pCfa;
    void                                 **pPvoid;
    UINT8                                *pUint8;
    AMBA_IMG_MAIN_CMD_MSG_RET_s          *pRetMsg;
#ifdef CONFIG_BUILD_IMGFRW_ISO_DIS
    AMBA_IMG_MAIN_ISO_CB_f               pUserIsoCb;
    const void                           *pCvoid;
    const AMBA_IMG_CHANNEL_ISO_INFO_s    *pCisoInfo;
#endif
} AMBA_IMG_MEM_INFO_u;

typedef struct /*_AMBA_IMG_MEM_INFO_s_*/ {
    AMBA_IMG_MEM_INFO_u    Ctx;
} AMBA_IMG_MEM_INFO_s;

AMBA_IMG_CHANNEL_s **pAmbaImgMainChannel = NULL;

static UINT32 AmbaImgMain_QueryActuralGain(UINT32 MainViewID, UINT32 ExposureFrame, const AMBA_IMG_SENSOR_GAIN_INFO_s *pDesiredGain, AMBA_IMG_SENSOR_GAIN_INFO_s *pActualGain);
static UINT32 AmbaImgMain_QueryActuralShutter(UINT32 MainViewID, UINT32 ExposureFrame, const FLOAT *DesiredShutter, AMBA_IMG_SENSOR_SHUTTER_INFO_s *pActualShutter);
static UINT32 AmbaImgMain_QueryActuralStrobe(UINT32 MainViewID, const UINT32 *pDesiredWidth, UINT32 *pActualWidth, UINT32 *pStrobeCtrl);

static   void AmbaImgMain_AlgoImgMode(UINT16 ViewId, UINT8 Mode, AMBA_IK_MODE_CFG_s* pMode);
static   void AmbaImgMain_AlgoLiveviewInfo(UINT16 ViewId, LIVEVIEW_INFO_s *pLiveViewInfo);

AMBA_IMG_CHANNEL_ID_s GNU_SECTION_NOZEROINIT AmbaImgMain_Algo2ChanTable[AMBA_IMG_NUM_VIN_CHANNEL*AMBA_IMG_NUM_VIN_ALGO*2U];

static UINT32 AmbaImgMain_ImgVinIqSensor[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO] = {0};
static UINT32 AmbaImgMain_ImgVinHdrSensor[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO] = {0};
static UINT32 AmbaImgMain_ImgSlowShutterTable[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_SENSOR] = {0};

UINT32 AmbaImgMain_ImgAaaCore[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO] = {0};
UINT32 AmbaImgMain_SensorIdTable[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_SENSOR] = {0};
UINT32 AmbaImgMain_LastMasterSyncTable[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_SENSOR] = {0};
#ifdef CONFIG_BUILD_IMGFRW_SMC
UINT32 AmbaImgMain_ImgSmcI2sId[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO] = {0};
#endif
UINT32 GNU_SECTION_NOZEROINIT AmbaImgMain_VinToneCurveUpd[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
#ifdef CONFIG_BUILD_IMGFRW_ISO_DIS
static AMBA_IMG_MAIN_ISO_CB_f AmbaImgMain_UserIsoCb = NULL;
#endif
UINT32 AmbaImgMain_DebugCount = AMBA_IMG_MAIN_DBG_COUNT;
UINT32 AmbaImgMain_DebugStillTiming = AMBA_IMG_MAIN_DBG_STILL_TIMING;

/**
 *  Amba image main dev get
 *  @param[in] VinId vin id
 *  @note this function is intended for internal use only
 */
static void AmbaImgMain_DevGet(UINT32 VinId)
{
    UINT32 FuncRetCode;
    UINT32 i, j;

    AMBA_SENSOR_CHANNEL_s SensorChanId;
    AMBA_SENSOR_DEVICE_INFO_s SensorDevInfo;

    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;

    SensorChanId.VinID = VinId;
    SensorChanId.SensorID = 0U;

    if ((pAmbaImgMainChannel != NULL) &&
        (pAmbaImgMainChannel[VinId] != NULL)) {
        for (i = 0U; pAmbaImgMainChannel[VinId][i].Magic == 0xCafeU; i++) {
            /* ctx check */
            if (pAmbaImgMainChannel[VinId][i].pCtx != NULL) {
                /* ctx get */
                pCtx = pAmbaImgMainChannel[VinId][i].pCtx;
                /* sensor id get */
                for (j = 0U; (j < AMBA_IMG_NUM_VIN_ALGO) && ((((UINT32) pCtx->Id.Ctx.Bits.SensorId) >> j) > 0U); j++) {
                    if ((((UINT32) pCtx->Id.Ctx.Bits.SensorId) & (((UINT32) 1U) << j)) > 0U) {
                        SensorChanId.SensorID = AmbaImgMain_SensorIdTable[VinId][j];
                        break;
                    }
                }
#ifdef CONFIG_BUILD_IMGFRW_TD_SM_BCNT
                /* td? */
                if (pCtx->Inter.Ctx.Bits.Num > 1U) {
                    /* alt dev info */
                    SensorChanId.Reserved[0] = 1U;
                    SensorChanId.Reserved[1] = 0U;
                }
#endif
                /* dev (chan) info get */
                AmbaImgPrint2(PRINT_FLAG_DBG, "sensor dev chan", VinId, j);
                FuncRetCode = AmbaSensor_GetDeviceInfo(&SensorChanId, &SensorDevInfo);
                if (FuncRetCode == SENSOR_ERR_NONE) {
                    /* agc delay */
                    AmbaImgSensorHAL_Info[VinId][i].Op.User.AgcDelay = (UINT32) SensorDevInfo.AnalogGainCtrlInfo.FirstReflectedFrame;
                    AmbaImgPrint(PRINT_FLAG_DBG, "sensor agc delay", AmbaImgSensorHAL_Info[VinId][i].Op.User.AgcDelay);
#ifdef AMBA_IMG_MAIN_DGC_SENSOR
                    /* dgc type & delay */
                    AmbaImgSensorHAL_Info[VinId][i].Op.User.DgcType = (UINT32) IMAGE_DGC_SENSOR;
                    AmbaImgSensorHAL_Info[VinId][i].Op.User.DgcDelay = (UINT32) SensorDevInfo.DigitalGainCtrlInfo.FirstReflectedFrame;
                    AmbaImgPrint(PRINT_FLAG_DBG, "sensor dgc delay", AmbaImgSensorHAL_Info[VinId][i].Op.User.DgcDelay);
#endif
#ifdef AMBA_IMG_MAIN_DGC_DSP
                    /* dgc type & delay */
                    AmbaImgSensorHAL_Info[VinId][i].Op.User.DgcType = (UINT32) IMAGE_DGC_DSP;
                    AmbaImgSensorHAL_Info[VinId][i].Op.User.DgcDelay = 0U;
                    AmbaImgPrint(PRINT_FLAG_DBG, "dgc (dsp) delay", AmbaImgSensorHAL_Info[VinId][i].Op.User.DgcDelay);
#endif
#ifdef AMBA_IMG_MAIN_DGC_BOTH
                    /* dgc type & delay */
                    AmbaImgSensorHAL_Info[VinId][i].Op.User.DgcType = (UINT32) IMAGE_DGC_BOTH;
                    AmbaImgSensorHAL_Info[VinId][i].Op.User.DgcDelay = (UINT32) SensorDevInfo.DigitalGainCtrlInfo.FirstReflectedFrame;
                    AmbaImgPrint(PRINT_FLAG_DBG, "sensor dgc (both) delay", AmbaImgSensorHAL_Info[VinId][i].Op.User.DgcDelay);
#endif
                    /* wgc delay */
                    AmbaImgSensorHAL_Info[VinId][i].Op.User.WgcDelay = (UINT32) SensorDevInfo.WbGainCtrlInfo.FirstReflectedFrame;
                    AmbaImgPrint(PRINT_FLAG_DBG, "sensor wgc delay", AmbaImgSensorHAL_Info[VinId][i].Op.User.WgcDelay);

                    /* svr delay */
                    AmbaImgSensorHAL_Info[VinId][i].Op.User.ShrDelay = (UINT32) SensorDevInfo.ShutterSpeedCtrlInfo.FirstReflectedFrame;
                    AmbaImgPrint(PRINT_FLAG_DBG, "sensor shr delay", AmbaImgSensorHAL_Info[VinId][i].Op.User.ShrDelay);

                    /* svr delay */
                    AmbaImgSensorHAL_Info[VinId][i].Op.User.SvrDelay = (UINT32) SensorDevInfo.FrameRateCtrlInfo.FirstReflectedFrame;
                    AmbaImgPrint(PRINT_FLAG_DBG, "sensor svr delay", AmbaImgSensorHAL_Info[VinId][i].Op.User.SvrDelay);

                    /* force post sof */
                    AmbaImgSensorHAL_Info[VinId][i].Op.User.ForcePostSof = 2U;
                    AmbaImgPrint(PRINT_FLAG_DBG, "force post sof", AmbaImgSensorHAL_Info[VinId][i].Op.User.ForcePostSof);

                    /* force adv eof */
                    AmbaImgSensorHAL_Info[VinId][i].Op.User.ForceAdvEof = 1U;
                    AmbaImgPrint(PRINT_FLAG_DBG, "force adv eof", AmbaImgSensorHAL_Info[VinId][i].Op.User.ForceAdvEof);

                    /* ss type */
                    AmbaImgSensorHAL_Info[VinId][i].Op.User.SsType = (UINT32) IMAGE_SS_TYPE_0;
                    AmbaImgPrint(PRINT_FLAG_DBG, "ss type", AmbaImgSensorHAL_Info[VinId][i].Op.User.SsType);

                    /* aux delay */
                    if (AmbaImgSensorHAL_Info[VinId][i].Op.User.ShrDelay <= 2U) {
                        AmbaImgSensorHAL_Info[VinId][i].Op.User.AuxDelay = 0U;
                    } else {
                        AmbaImgSensorHAL_Info[VinId][i].Op.User.AuxDelay = 1U;
                    }
                    AmbaImgPrint(PRINT_FLAG_DBG, "aux delay", AmbaImgSensorHAL_Info[VinId][i].Op.User.AuxDelay);

                    /* stat delay */
                    if (AmbaImgSensorHAL_Info[VinId][i].pAux->Ctx.Bits.R2yBackground == 0U) {
                        AmbaImgSensorHAL_Info[VinId][i].Op.User.StatDelay = 0U;
                    } else {
                        AmbaImgSensorHAL_Info[VinId][i].Op.User.StatDelay = 1U;
                    }
                    AmbaImgPrint(PRINT_FLAG_DBG, "stat delay", AmbaImgSensorHAL_Info[VinId][i].Op.User.StatDelay);

                    /* force ss in video */
#ifndef CONFIG_BUILD_IMGFRW_SMC
                    AmbaImgSensorHAL_Info[VinId][i].Op.User.ForceSsInVideo = 0U;
#else
                    if (AmbaImgSensorHAL_AuxInfo[VinId].Ctx.Bits.ForceSlowShutter > 0U) {
                        if (AmbaImgSensorHAL_AuxInfo[VinId].Ctx.Bits.MaxSlowShutterIndex > 0U) {
                            AmbaImgSensorHAL_Info[VinId][i].Op.User.ForceSsInVideo = 0x80000000U | ((UINT32) AmbaImgSensorHAL_AuxInfo[VinId].Ctx.Bits.MaxSlowShutterIndex);
                        } else {
                            AmbaImgSensorHAL_Info[VinId][i].Op.User.ForceSsInVideo = 0U;
                        }
                    } else {
                        AmbaImgSensorHAL_Info[VinId][i].Op.User.ForceSsInVideo = 0U;
                    }
#endif
                    AmbaImgPrintEx(PRINT_FLAG_DBG, "force ss in video", AmbaImgSensorHAL_Info[VinId][i].Op.User.ForceSsInVideo, 16U);

                    /* eof move max time */
                    AmbaImgSensorHAL_Info[VinId][i].Op.User.EofMoveMaxTime = 0U;
                    AmbaImgPrint(PRINT_FLAG_DBG, "eof move max time", AmbaImgSensorHAL_Info[VinId][i].Op.User.EofMoveMaxTime);

                    /* write latency */
                    AmbaImgSensorHAL_Info[VinId][i].Op.User.WriteLatency = 0U;
                    AmbaImgPrint(PRINT_FLAG_DBG, "write latency", AmbaImgSensorHAL_Info[VinId][i].Op.User.WriteLatency);

                } else {
                    AmbaImgPrint(PRINT_FLAG_ERR, "error: sensor dev info", FuncRetCode);
                }
            }
        }
    }
}

/**
 *  Amba image main info get
 *  @param[in] VinId vin id
 *  @note this function is intended for internal use only
 */
static void AmbaImgMain_InfoGet(UINT32 VinId)
{
    UINT32 FuncRetCode;
    UINT32 i, j, k;

    UINT32 TimeScale;
    UINT32 NumUnitsInTick;

    FLOAT ValueF;
    UINT64 ValueULL;

    AMBA_SENSOR_CHANNEL_s SensorChanId;
    AMBA_SENSOR_STATUS_INFO_s SensorStatus;

    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;

    SensorChanId.VinID = VinId;
    SensorChanId.SensorID = 0U;

    if ((pAmbaImgMainChannel != NULL) &&
        (pAmbaImgMainChannel[VinId] != NULL)) {
        for (i = 0U; pAmbaImgMainChannel[VinId][i].Magic == 0xCafeU; i++) {
            /* ctx check */
            if (pAmbaImgMainChannel[VinId][i].pCtx != NULL) {
                /* ctx get */
                pCtx = pAmbaImgMainChannel[VinId][i].pCtx;
                /* sensor id get */
                for (j = 0U; (j < AMBA_IMG_NUM_VIN_ALGO) && ((((UINT32) pCtx->Id.Ctx.Bits.SensorId) >> j) > 0U); j++) {
                    if ((((UINT32) pCtx->Id.Ctx.Bits.SensorId) & (((UINT32) 1U) << j)) > 0U) {
                        SensorChanId.SensorID = AmbaImgMain_SensorIdTable[VinId][j];
                        break;
                    }
                }
                /* status (chan) info get */
                AmbaImgPrint2(PRINT_FLAG_DBG, "sensor info chan", VinId, j);
                FuncRetCode = AmbaSensor_GetStatus(&SensorChanId, &SensorStatus);
                if (FuncRetCode == SENSOR_ERR_NONE) {
                    FuncRetCode = AmbaSensor_GetModeInfo(&SensorChanId, &(SensorStatus.ModeInfo.Config), &(SensorStatus.ModeInfo));
                    if (FuncRetCode == SENSOR_ERR_NONE) {
                        TimeScale = SensorStatus.ModeInfo.FrameRate.TimeScale;
                        NumUnitsInTick = SensorStatus.ModeInfo.FrameRate.NumUnitsInTick;

                        /* cfg info */
                        AmbaImgSensorHAL_Info[VinId][i].Cfg.Mode = SensorStatus.ModeInfo.Config.ModeID;
                        AmbaImgSensorHAL_Info[VinId][i].Cfg.Bit = SensorStatus.ModeInfo.OutputInfo.NumDataBits;
                        AmbaImgSensorHAL_Info[VinId][i].Cfg.FirstPixelColor = SensorStatus.ModeInfo.OutputInfo.BayerPattern;
                        AmbaImgSensorHAL_Info[VinId][i].Cfg.Width = SensorStatus.ModeInfo.OutputInfo.OutputWidth;
                        AmbaImgSensorHAL_Info[VinId][i].Cfg.Height = SensorStatus.ModeInfo.OutputInfo.OutputHeight;
                        AmbaImgSensorHAL_Info[VinId][i].Cfg.Binning = SensorStatus.ModeInfo.InputInfo.SummingFactor;

                        /* ssr/ssg */
                        if (SensorStatus.ElecShutterMode == AMBA_SENSOR_ESHUTTER_TYPE_ROLLING) {
                            AmbaImgSensorHAL_Info[VinId][i].Cfg.ShutterMode = (UINT32) IMAGE_SHR_SSR;
                        } else {
                            AmbaImgSensorHAL_Info[VinId][i].Cfg.ShutterMode = (UINT32) IMAGE_SHR_SSG;
                        }

                        /* frame info */
                        AmbaImgSensorHAL_Info[VinId][i].Frame.TotalLine = SensorStatus.ModeInfo.FrameLengthLines;
                        AmbaImgSensorHAL_Info[VinId][i].Frame.TotalShutterLine = SensorStatus.ModeInfo.NumExposureStepPerFrame;
                        ValueF = ((SensorStatus.ModeInfo.RowTime*1000000000.0f) + 0.5f);
                        AmbaImgSensorHAL_Info[VinId][i].Frame.TotalNsInLine = (ValueF > 0.0f) ? (UINT32) ValueF : 0U;
                        ValueULL = (((UINT64) AmbaImgSensorHAL_Info[VinId][i].Frame.TotalNsInLine)*((UINT64) AmbaImgSensorHAL_Info[VinId][i].Frame.TotalLine))/((UINT64) AmbaImgSensorHAL_Info[VinId][i].Frame.TotalShutterLine);
                        AmbaImgSensorHAL_Info[VinId][i].Frame.TotalNsInShutterLine = (UINT32) (ValueULL & 0xFFFFFFFFULL);
                        AmbaImgSensorHAL_Info[VinId][i].Frame.Rate = TimeScale*100U/NumUnitsInTick;
                        AmbaImgSensorHAL_Info[VinId][i].Frame.Timetick = 100000U/AmbaImgSensorHAL_Info[VinId][i].Frame.Rate;
#ifdef CONFIG_BUILD_IMGFRW_HDR_SS
                        /* frame hdr info */
                        AmbaImgSensorHAL_HdrInfo[VinId][i].Frame.ChannelSsType = AMBA_IMG_SENSOR_HAL_HDR_SS_DEF; //SensorStatus.ModeInfo.HdrInfo.HdrSsType;
                        AmbaImgSensorHAL_HdrInfo[VinId][i].Frame.ChannelNum = SensorStatus.ModeInfo.HdrInfo.ActiveChannels;
                        for (k = 0U; k < SensorStatus.ModeInfo.HdrInfo.ActiveChannels; k++) {
                            AmbaImgSensorHAL_HdrInfo[VinId][i].Frame.TotalShutterLine[k] = SensorStatus.ModeInfo.HdrInfo.ChannelInfo[k].NumExposureStepPerFrame;
                            AmbaImgSensorHAL_HdrInfo[VinId][i].Frame.ExposureLine[k].Max = SensorStatus.ModeInfo.HdrInfo.ChannelInfo[k].MaxExposureLine;
                            AmbaImgSensorHAL_HdrInfo[VinId][i].Frame.ExposureLine[k].Min = SensorStatus.ModeInfo.HdrInfo.ChannelInfo[k].MinExposureLine;
                        }
#else
                        /* frame hdr info (preset) */
                        AmbaImgSensorHAL_HdrInfo[VinId][i].Frame.ChannelSsType = 0U;
                        AmbaImgSensorHAL_HdrInfo[VinId][i].Frame.ChannelNum = SensorStatus.ModeInfo.HdrInfo.ActiveChannels;
                        for (k = 0U; k < AMBA_IMG_SENSOR_HAL_HDR_SIZE; k++) {
                            AmbaImgSensorHAL_HdrInfo[VinId][i].Frame.TotalShutterLine[k] = 0U;
                            AmbaImgSensorHAL_HdrInfo[VinId][i].Frame.ExposureLine[k].Max = SensorStatus.ModeInfo.HdrInfo.ChannelInfo[k].MaxExposureLine;
                            AmbaImgSensorHAL_HdrInfo[VinId][i].Frame.ExposureLine[k].Min = SensorStatus.ModeInfo.HdrInfo.ChannelInfo[k].MinExposureLine;
                        }
#endif
                        /* image info */
                        AmbaImgSensorHAL_Info[VinId][i].Image.StartX = SensorStatus.ModeInfo.InputInfo.PhotodiodeArray.StartX;
                        AmbaImgSensorHAL_Info[VinId][i].Image.StartY = SensorStatus.ModeInfo.InputInfo.PhotodiodeArray.StartY;
                        AmbaImgSensorHAL_Info[VinId][i].Image.Width = SensorStatus.ModeInfo.InputInfo.PhotodiodeArray.Width;
                        AmbaImgSensorHAL_Info[VinId][i].Image.Height = SensorStatus.ModeInfo.InputInfo.PhotodiodeArray.Height;
                        AmbaImgSensorHAL_Info[VinId][i].Image.HFactorNum = SensorStatus.ModeInfo.InputInfo.HSubsample.FactorNum;
                        AmbaImgSensorHAL_Info[VinId][i].Image.HFactorDen = SensorStatus.ModeInfo.InputInfo.HSubsample.FactorDen;
                        AmbaImgSensorHAL_Info[VinId][i].Image.VFactorNum = SensorStatus.ModeInfo.InputInfo.VSubsample.FactorNum;
                        AmbaImgSensorHAL_Info[VinId][i].Image.VFactorDen = SensorStatus.ModeInfo.InputInfo.VSubsample.FactorDen;

                        AmbaImgPrint(PRINT_FLAG_DBG, "sensor mode", AmbaImgSensorHAL_Info[VinId][i].Cfg.Mode);
                        AmbaImgPrint(PRINT_FLAG_DBG, "sensor bits", AmbaImgSensorHAL_Info[VinId][i].Cfg.Bit);
                        AmbaImgPrint(PRINT_FLAG_DBG, "sensor bayer", AmbaImgSensorHAL_Info[VinId][i].Cfg.FirstPixelColor);
                        AmbaImgPrint(PRINT_FLAG_DBG, "sensor width", AmbaImgSensorHAL_Info[VinId][i].Cfg.Width);
                        AmbaImgPrint(PRINT_FLAG_DBG, "sensor height", AmbaImgSensorHAL_Info[VinId][i].Cfg.Height);
                        AmbaImgPrint(PRINT_FLAG_DBG, "sensor binning", AmbaImgSensorHAL_Info[VinId][i].Cfg.Binning);

                        AmbaImgPrint(PRINT_FLAG_DBG, "sensor frame total line", AmbaImgSensorHAL_Info[VinId][i].Frame.TotalLine);
                        AmbaImgPrint(PRINT_FLAG_DBG, "sensor frame total shutter line", AmbaImgSensorHAL_Info[VinId][i].Frame.TotalShutterLine);
                        AmbaImgPrint(PRINT_FLAG_DBG, "sensor frame total ns in line", AmbaImgSensorHAL_Info[VinId][i].Frame.TotalNsInLine);
                        AmbaImgPrint(PRINT_FLAG_DBG, "sensor frame total ns in shutter line", AmbaImgSensorHAL_Info[VinId][i].Frame.TotalNsInShutterLine);

                        AmbaImgPrint(PRINT_FLAG_DBG, "sensor frame rate", AmbaImgSensorHAL_Info[VinId][i].Frame.Rate);
                        AmbaImgPrint(PRINT_FLAG_DBG, "sensor frame timetick", AmbaImgSensorHAL_Info[VinId][i].Frame.Timetick);
#ifdef CONFIG_BUILD_IMGFRW_HDR_SS
                        AmbaImgPrint(PRINT_FLAG_DBG, "sensor hdr channel ss type", AmbaImgSensorHAL_HdrInfo[VinId][i].Frame.ChannelSsType);
                        AmbaImgPrint(PRINT_FLAG_DBG, "sensor hdr channel num", AmbaImgSensorHAL_HdrInfo[VinId][i].Frame.ChannelNum);
                        for (k = 0U; k < AmbaImgSensorHAL_HdrInfo[VinId][i].Frame.ChannelNum; k++) {
                            AmbaImgPrint2(PRINT_FLAG_DBG, "sensor hdr total shutter line", k, AmbaImgSensorHAL_HdrInfo[VinId][i].Frame.TotalShutterLine[k]);
                        }
#endif
                        AmbaImgPrint(PRINT_FLAG_DBG, "sensor image start x", AmbaImgSensorHAL_Info[VinId][i].Image.StartX);
                        AmbaImgPrint(PRINT_FLAG_DBG, "sensor image start y", AmbaImgSensorHAL_Info[VinId][i].Image.StartY);
                        AmbaImgPrint(PRINT_FLAG_DBG, "sensor image width", AmbaImgSensorHAL_Info[VinId][i].Image.Width);
                        AmbaImgPrint(PRINT_FLAG_DBG, "sensor image height", AmbaImgSensorHAL_Info[VinId][i].Image.Height);
                        AmbaImgPrint(PRINT_FLAG_DBG, "sensor image hfactor num", AmbaImgSensorHAL_Info[VinId][i].Image.HFactorNum);
                        AmbaImgPrint(PRINT_FLAG_DBG, "sensor image hfactor den", AmbaImgSensorHAL_Info[VinId][i].Image.HFactorDen);
                        AmbaImgPrint(PRINT_FLAG_DBG, "sensor image vfactor num", AmbaImgSensorHAL_Info[VinId][i].Image.VFactorNum);
                        AmbaImgPrint(PRINT_FLAG_DBG, "sensor image vfactor den", AmbaImgSensorHAL_Info[VinId][i].Image.VFactorDen);

                        /* readout time (DBG) */
                        {
                            UINT32 CaptureLine = AmbaImgSystem_Info[VinId][i].Vin.Cap.Bits.Height;
                            UINT32 LineInNs = AmbaImgSensorHAL_Info[VinId][i].Frame.TotalNsInLine;

                            ValueULL = ((UINT64) LineInNs) * ((UINT64) CaptureLine)/1000ULL;
                            AmbaImgPrint(PRINT_FLAG_DBG, "readout time", (UINT32) (ValueULL & 0xFFFFFFFFULL));
                        }
                    } else {
                        AmbaImgPrint(PRINT_FLAG_ERR, "error: sensor mode info", FuncRetCode);
                    }
                } else {
                    AmbaImgPrint(PRINT_FLAG_ERR, "error: sensor status", FuncRetCode);
                }
            }
        }
    }
}

#ifdef CONFIG_BUILD_IMGFRW_ISO_DIS
/**
 *  Amba image main user iso callback hook
 *  @param[in] pFunc iso callback function
 *  @note this function is intended for internal use only
 */
static void AmbaImgMain_UserIsoCbHook(AMBA_IMG_MAIN_ISO_CB_f pFunc)
{
    AmbaImgMain_UserIsoCb = pFunc;
}

/**
 *  Amba image main image framework iso callback
 *  @param[in] pIsoInfo pointer to iso information
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMain_ImgChanIsoCb(const AMBA_IMG_CHANNEL_ISO_INFO_s *pIsoInfo)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    AMBA_IMG_MEM_INFO_s MemInfo;

    /* iso info get */
    MemInfo.Ctx.pCisoInfo = pIsoInfo;
    /* iso cb check */
    if (AmbaImgMain_UserIsoCb != NULL) {
        /* iso cb */
        FuncRetCode = AmbaImgMain_UserIsoCb(MemInfo.Ctx.pCvoid);
        if (FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }
    }

    return RetCode;
}
#endif

/**
 *  Amba image main image framework dispatch
 *  @param[in] VinId vin id
 *  @param[in] Param command and message
 *  @note this function is intended for internal use only
 */
static void AmbaImgMain_ImgFrwDispatch(UINT32 VinId, UINT64 Param)
{
    static UINT32 AmbaImgMain_ImgStatisticsInter[AMBA_IMG_NUM_VIN_CHANNEL] = {0};
    UINT32 FuncRetCode;
    UINT32 i;

    AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

    CmdMsg.Ctx.Data = Param;

    if (VinId < AMBA_IMG_NUM_VIN_CHANNEL) {
        switch (CmdMsg.Ctx.Bits.Cmd) {
            case (UINT8) LIVEVIEW_CMD_START:
                AmbaImgPrint2(PRINT_FLAG_DBG, "liveview-cmd-start", VinId, (UINT32) CmdMsg.Ctx.Bits.Cmd);
                AmbaImgSensorHAL_AuxInfo[VinId].Ctx.Data = CmdMsg.Ctx.Data;

                AmbaImgPrint(PRINT_FLAG_DBG, "op mode", AmbaImgSensorHAL_AuxInfo[VinId].Ctx.Bits.OpMode);
                AmbaImgPrint(PRINT_FLAG_DBG, "r2y background", AmbaImgSensorHAL_AuxInfo[VinId].Ctx.Bits.R2yBackground);
                AmbaImgPrint(PRINT_FLAG_DBG, "stitch", AmbaImgSensorHAL_AuxInfo[VinId].Ctx.Bits.Stitch);
                AmbaImgPrint(PRINT_FLAG_DBG, "deferred black leve", AmbaImgSensorHAL_AuxInfo[VinId].Ctx.Bits.DeferredBlackLevel);
                AmbaImgPrint(PRINT_FLAG_DBG, "low delay", AmbaImgSensorHAL_AuxInfo[VinId].Ctx.Bits.LowDelay);
                AmbaImgPrint(PRINT_FLAG_DBG, "max slow shutter index", AmbaImgSensorHAL_AuxInfo[VinId].Ctx.Bits.MaxSlowShutterIndex);
                AmbaImgPrint(PRINT_FLAG_DBG, "force slow shutter", AmbaImgSensorHAL_AuxInfo[VinId].Ctx.Bits.ForceSlowShutter);
                AmbaImgPrint(PRINT_FLAG_DBG, "iq section id", AmbaImgSensorHAL_AuxInfo[VinId].Ctx.Bits.IqSecId);

                AmbaImgPrint(PRINT_FLAG_DBG, "vin skip frame en", AmbaImgSensorHAL_AuxInfo[VinId].Ctx.Bits.VinSkipFrameEn);
                AmbaImgPrint(PRINT_FLAG_DBG, "vin skip frame", AmbaImgSensorHAL_AuxInfo[VinId].Ctx.Bits.VinSkipFrame);

                /* dev get */
                AmbaImgMain_DevGet(VinId);
                /* info get */
                AmbaImgMain_InfoGet(VinId);

                /* vr map */
                FuncRetCode = AmbaImgChannel_VrMap(VinId, pAmbaImgMainChannel);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrint2(PRINT_FLAG_ERR, "error: image channel vr map", VinId, FuncRetCode);
                }

                /* image statistics reset */
                FuncRetCode = AmbaImgStatistics_Reset(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrint2(PRINT_FLAG_ERR, "error: image statistics reset", VinId, FuncRetCode);
                }
                /* image statistics inter */
                FuncRetCode = AmbaImgStatistics_Inter(VinId, AmbaImgMain_ImgStatisticsInter[VinId]);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrint2(PRINT_FLAG_ERR, "error: image statistics inter", VinId, FuncRetCode);
                }

                /* image sensor HAL reset */
                FuncRetCode = AmbaImgSensorHAL_Reset(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrint2(PRINT_FLAG_ERR, "error: image sensor HAL reset", VinId, FuncRetCode);
                }

                /* image sensor sync reset */
                FuncRetCode = AmbaImgSensorSync_Reset(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrint2(PRINT_FLAG_ERR, "error: image sensor Sync reset", VinId, FuncRetCode);
                }

                /* image system reset */
                FuncRetCode = AmbaImgSystem_Reset(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrint2(PRINT_FLAG_ERR, "error: image system reset", VinId, FuncRetCode);
                }

                /* master sync table reset */
                if ((pAmbaImgMainChannel != NULL) && (pAmbaImgMainChannel[VinId] != NULL)) {
                    for (i = 0U; pAmbaImgMainChannel[VinId][i].Magic == 0xCafeU; i++) {
                        /* ctx check */
                        if (pAmbaImgMainChannel[VinId][i].pCtx != NULL) {
                            UINT32 SensorId = pAmbaImgMainChannel[VinId][i].pCtx->Id.Ctx.Bits.SensorId;
                            for (UINT32 j = 0U; j < AMBA_IMG_NUM_VIN_SENSOR; j++) {
                                if ((SensorId & (((UINT32) 1U) << j)) > 0U) {
                                    AmbaImgMain_LastMasterSyncTable[VinId][j] = 0U;
                                }
                            }
                        }
                    }
                }

                /* debug msg print */
                if ((pAmbaImgMainChannel != NULL) &&
                    (pAmbaImgMainChannel[VinId] != NULL)) {
                    for (i = 0U; pAmbaImgMainChannel[VinId][i].Magic == 0xCafeU; i++) {
                        /* ctx check */
                        if (pAmbaImgMainChannel[VinId][i].pCtx != NULL) {
                            /* chan print */
                            AmbaImgPrint2(PRINT_FLAG_DBG, "hal info chan", VinId, i);
                            AmbaImgPrint(PRINT_FLAG_DBG, "adv ssi cmd", (UINT32) AmbaImgSensorHAL_Info[VinId][i].Op.Ctrl.AdvSsiCmd);
                            AmbaImgPrint(PRINT_FLAG_DBG, "adv svr upd", (UINT32) AmbaImgSensorHAL_Info[VinId][i].Op.Ctrl.AdvSvrUpd);
                            AmbaImgPrint(PRINT_FLAG_DBG, "adv rgb cmd", (UINT32) AmbaImgSensorHAL_Info[VinId][i].Op.Ctrl.AdvRgbCmd);
                            AmbaImgPrint(PRINT_FLAG_DBG, "adv aik cmd", (UINT32) AmbaImgSensorHAL_Info[VinId][i].Op.Ctrl.AdvAikCmd);
                            AmbaImgPrint(PRINT_FLAG_DBG, "post sof",    (UINT32) AmbaImgSensorHAL_Info[VinId][i].Op.Timing.PostSof.Bits.Timetick);
                            AmbaImgPrint(PRINT_FLAG_DBG, "adv dsp",     (UINT32) AmbaImgSensorHAL_Info[VinId][i].Op.Timing.AdvDsp.Bits.Timetick);
                            AmbaImgPrint(PRINT_FLAG_DBG, "adv eof",     (UINT32) AmbaImgSensorHAL_Info[VinId][i].Op.Timing.AdvEof.Bits.Timetick);
                            AmbaImgPrint(PRINT_FLAG_DBG, "aik exec",    (UINT32) AmbaImgSensorHAL_Info[VinId][i].Op.Timing.AikExecute.Bits.Timetick);
                            AmbaImgPrint(PRINT_FLAG_DBG, "mof",         (UINT32) AmbaImgSensorHAL_Info[VinId][i].Op.Timing.Mof.Bits.Timetick);
                        }
                    }
                }
                break;
            case (UINT8) VIN_CMD_CAP:
                AmbaImgPrint2(PRINT_FLAG_DBG, "vin-cmd-cap", VinId, (UINT32) CmdMsg.Ctx.Bits.Cmd);
                AmbaImgSystem_Info[VinId][CmdMsg.Ctx.VinCap.ChainId].Vin.Cap.Data = CmdMsg.Ctx.Data;
                AmbaImgPrint(PRINT_FLAG_DBG, "chain id", (UINT32) CmdMsg.Ctx.VinCap.ChainId);
                AmbaImgPrint(PRINT_FLAG_DBG, "cap width", (UINT32) AmbaImgSystem_Info[VinId][CmdMsg.Ctx.VinCap.ChainId].Vin.Cap.Bits.Width);
                AmbaImgPrint(PRINT_FLAG_DBG, "cap height", (UINT32) AmbaImgSystem_Info[VinId][CmdMsg.Ctx.VinCap.ChainId].Vin.Cap.Bits.Height);
                break;
            case (UINT8) VIN_CMD_SENSOR:
                AmbaImgPrint2(PRINT_FLAG_DBG, "vin-cmd-sensor", VinId, (UINT32) CmdMsg.Ctx.Bits.Cmd);
                if (CmdMsg.Ctx.VinSensor.Idx < AMBA_IMG_NUM_VIN_SENSOR) {
                    AmbaImgMain_SensorIdTable[VinId][CmdMsg.Ctx.VinSensor.Idx] = (UINT32) CmdMsg.Ctx.VinSensor.Id;
                    AmbaImgMain_LastMasterSyncTable[VinId][CmdMsg.Ctx.VinSensor.Idx] = 0U;
                }
                AmbaImgPrint(PRINT_FLAG_DBG, "sensor idx", (UINT32) CmdMsg.Ctx.VinSensor.Idx);
                AmbaImgPrintEx(PRINT_FLAG_DBG, "sensor id", (UINT32) CmdMsg.Ctx.VinSensor.Id, 16U);
                break;
            case (UINT8) PIPE_CMD_MODE:
                AmbaImgPrint2(PRINT_FLAG_DBG, "pipe-cmd-mode", VinId, (UINT32) CmdMsg.Ctx.Bits.Cmd);
                AmbaImgPrint(PRINT_FLAG_DBG, "chain id", (UINT32) CmdMsg.Ctx.PipeMode.ChainId);
                AmbaImgPrint(PRINT_FLAG_DBG, "zone idx", (UINT32) CmdMsg.Ctx.PipeMode.ZoneIdx);
                AmbaImgPrint(PRINT_FLAG_DBG, "pipe mode", (UINT32) CmdMsg.Ctx.PipeMode.Id);
                FuncRetCode = AmbaImgChannel_PipeModePut(VinId, CmdMsg.Ctx.PipeMode.ChainId, CmdMsg.Ctx.PipeMode.ZoneIdx, CmdMsg.Ctx.PipeMode.Id);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrint(PRINT_FLAG_ERR, "error: chan pipe mode", FuncRetCode);
                }
                break;
            case (UINT8) PIPE_CMD_OUT:
                AmbaImgPrint2(PRINT_FLAG_DBG, "pipe-cmd-out", VinId, (UINT32) CmdMsg.Ctx.Bits.Cmd);
                AmbaImgPrint(PRINT_FLAG_DBG, "chain id", (UINT32) CmdMsg.Ctx.PipeOut.ChainId);
                AmbaImgPrint(PRINT_FLAG_DBG, "zone idx", (UINT32) CmdMsg.Ctx.PipeOut.ZoneIdx);
                AmbaImgPrint(PRINT_FLAG_DBG, "main width", (UINT32) CmdMsg.Ctx.PipeOut.Width);
                AmbaImgPrint(PRINT_FLAG_DBG, "main height", (UINT32) CmdMsg.Ctx.PipeOut.Height);
                FuncRetCode = AmbaImgChannel_PipeOutPut(VinId, CmdMsg.Ctx.PipeOut.ChainId, CmdMsg.Ctx.PipeOut.ZoneIdx, CmdMsg.Ctx.PipeOut.Width, CmdMsg.Ctx.PipeOut.Height);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrint(PRINT_FLAG_ERR, "error: chan pipe out", FuncRetCode);
                }
                break;
            case (UINT8) PIPE_CMD_HDR:
                AmbaImgPrint2(PRINT_FLAG_DBG, "pipe-cmd-hdr", VinId, (UINT32) CmdMsg.Ctx.Bits.Cmd);
                AmbaImgSystem_Info[VinId][CmdMsg.Ctx.PipeHdr.ChainId].Pipe.Hdr.Data = CmdMsg.Ctx.Data;
                AmbaImgPrint(PRINT_FLAG_DBG, "chain id", (UINT32) CmdMsg.Ctx.PipeHdr.ChainId);
                AmbaImgPrint(PRINT_FLAG_DBG, "hdr enable", (UINT32) AmbaImgSystem_Info[VinId][CmdMsg.Ctx.PipeHdr.ChainId].Pipe.Hdr.Bits.Enable);
                AmbaImgPrint(PRINT_FLAG_DBG, "hdr exposure num", (UINT32) AmbaImgSystem_Info[VinId][CmdMsg.Ctx.PipeHdr.ChainId].Pipe.Hdr.Bits.ExposureNum);
                AmbaImgPrint(PRINT_FLAG_DBG, "hdr linear ce", (UINT32) AmbaImgSystem_Info[VinId][CmdMsg.Ctx.PipeHdr.ChainId].Pipe.Hdr.Bits.LCeEnable);
                AmbaImgPrint(PRINT_FLAG_DBG, "hdr back wgc", (UINT32) AmbaImgSystem_Info[VinId][CmdMsg.Ctx.PipeHdr.ChainId].Pipe.Hdr.Bits.BackWgc);
                AmbaImgPrint(PRINT_FLAG_DBG, "hdr back alt wgc", (UINT32) AmbaImgSystem_Info[VinId][CmdMsg.Ctx.PipeHdr.ChainId].Pipe.Hdr.Bits.BackAltWgc);
                AmbaImgPrint(PRINT_FLAG_DBG, "hdr back dgc", (UINT32) AmbaImgSystem_Info[VinId][CmdMsg.Ctx.PipeHdr.ChainId].Pipe.Hdr.Bits.BackDgc);
                break;
            case (UINT8) IMAGE_CMD_CHAN:
                AmbaImgPrint2(PRINT_FLAG_DBG, "image-cmd-chan", VinId, (UINT32) CmdMsg.Ctx.Bits.Cmd);
                switch (CmdMsg.Ctx.Bits.Msg) {
                    case (UINT8) CHAN_MSG_SELECT:
                        AmbaImgPrint(PRINT_FLAG_DBG, "chan-msg-select", (UINT32) CmdMsg.Ctx.ImageChanSelect.Msg);
                        AmbaImgPrint(PRINT_FLAG_DBG, "chain index", CmdMsg.Ctx.ImageChanSelect.ChainIndex);
                        pAmbaImgMainChannel = AmbaImgChannel_Select(VinId, (UINT32) CmdMsg.Ctx.ImageChanSelect.ChainIndex);
                        AmbaImgChannel_Register(pAmbaImgMainChannel);
                        break;
                    case (UINT8) CHAN_MSG_SENSOR:
                        AmbaImgPrint(PRINT_FLAG_DBG, "chan-msg-sensor", (UINT32) CmdMsg.Ctx.ImageChanSensor.Msg);
                        AmbaImgPrint(PRINT_FLAG_DBG, "chain id", CmdMsg.Ctx.ImageChanSensor.ChainId);
                        AmbaImgPrintEx(PRINT_FLAG_DBG, "sensor id", CmdMsg.Ctx.ImageChanSensor.Id, 16U);
                        FuncRetCode = AmbaImgChannel_SensorIdPut(VinId, CmdMsg.Ctx.ImageChanSensor.ChainId, CmdMsg.Ctx.ImageChanSensor.Id);
                        if (FuncRetCode != OK_UL) {
                            AmbaImgPrint(PRINT_FLAG_ERR, "error: chan sensor id", FuncRetCode);
                        }
                        break;
                    case (UINT8) CHAN_MSG_ALGO:
                        AmbaImgPrint(PRINT_FLAG_DBG, "chan-msg-algo", (UINT32) CmdMsg.Ctx.ImageChanAlgo.Msg);
                        AmbaImgPrint(PRINT_FLAG_DBG, "chain id", CmdMsg.Ctx.ImageChanAlgo.ChainId);
                        AmbaImgPrint(PRINT_FLAG_DBG, "algo id", CmdMsg.Ctx.ImageChanAlgo.Id);
                        FuncRetCode = AmbaImgChannel_AlgoIdPut(VinId, CmdMsg.Ctx.ImageChanAlgo.ChainId, CmdMsg.Ctx.ImageChanAlgo.Id);
                        if (FuncRetCode != OK_UL) {
                            AmbaImgPrint(PRINT_FLAG_ERR, "error: chan algo id", FuncRetCode);
                        }
                        break;
                    case (UINT8) CHAN_MSG_ZONE:
                        AmbaImgPrint(PRINT_FLAG_DBG, "chan-msg-zone", (UINT32) CmdMsg.Ctx.ImageChanZone.Msg);
                        AmbaImgPrint(PRINT_FLAG_DBG, "chain id", CmdMsg.Ctx.ImageChanZone.ChainId);
                        AmbaImgPrintEx(PRINT_FLAG_DBG, "zone id", CmdMsg.Ctx.ImageChanZone.Id, 16U);
                        FuncRetCode = AmbaImgChannel_ZoneIdPut(VinId, CmdMsg.Ctx.ImageChanZone.ChainId, CmdMsg.Ctx.ImageChanZone.Id);
                        if (FuncRetCode != OK_UL) {
                            AmbaImgPrint(PRINT_FLAG_ERR, "error: chan zone id", FuncRetCode);
                        }
                        break;
                    case (UINT8) CHAN_MSG_ZONE_MSB:
                        AmbaImgPrint(PRINT_FLAG_DBG, "chan-msg-zone-msb", (UINT32) CmdMsg.Ctx.ImageChanZoneMsb.Msg);
                        AmbaImgPrint(PRINT_FLAG_DBG, "chain id", CmdMsg.Ctx.ImageChanZoneMsb.ChainId);
                        AmbaImgPrintEx(PRINT_FLAG_DBG, "zone msb id", CmdMsg.Ctx.ImageChanZoneMsb.Id, 16U);
                        FuncRetCode = AmbaImgChannel_ZoneMsbIdPut(VinId, CmdMsg.Ctx.ImageChanZoneMsb.ChainId, CmdMsg.Ctx.ImageChanZoneMsb.Id);
                        if (FuncRetCode != OK_UL) {
                            AmbaImgPrint(PRINT_FLAG_ERR, "error: chan zone msb id", FuncRetCode);
                        }
                        break;
                    case (UINT8) CHAN_MSG_VR:
                        AmbaImgPrint(PRINT_FLAG_DBG, "chan-msg-vr", (UINT32) CmdMsg.Ctx.ImageChanVr.Msg);
                        AmbaImgPrint(PRINT_FLAG_DBG, "chain id", CmdMsg.Ctx.ImageChanVr.ChainId);
                        AmbaImgPrintEx(PRINT_FLAG_DBG, "vr id", CmdMsg.Ctx.ImageChanVr.Id, 16U);
                        AmbaImgPrintEx(PRINT_FLAG_DBG, "vr alt id", CmdMsg.Ctx.ImageChanVr.AltId, 16U);
                        FuncRetCode = AmbaImgChannel_VrIdPut(VinId, CmdMsg.Ctx.ImageChanVr.ChainId, CmdMsg.Ctx.ImageChanVr.Id, CmdMsg.Ctx.ImageChanVr.AltId);
                        if (FuncRetCode != OK_UL) {
                            AmbaImgPrint(PRINT_FLAG_ERR, "error: chan vr id", FuncRetCode);
                        }
                        break;
                    case (UINT8) CHAN_MSG_IQ:
                        AmbaImgPrint(PRINT_FLAG_DBG, "chan-msg-iq", (UINT32) CmdMsg.Ctx.ImageChanIq.Msg);
                        AmbaImgPrint(PRINT_FLAG_DBG, "chain id", CmdMsg.Ctx.ImageChanIq.ChainId);
                        AmbaImgPrint(PRINT_FLAG_DBG, "hdr mode", CmdMsg.Ctx.ImageChanIq.HdrId);
                        AmbaImgPrint(PRINT_FLAG_DBG, "iq table", CmdMsg.Ctx.ImageChanIq.Id);
                        AmbaImgMain_ImgVinHdrSensor[VinId][CmdMsg.Ctx.ImageChanIq.ChainId] = (UINT32) CmdMsg.Ctx.ImageChanIq.HdrId;
                        AmbaImgMain_ImgVinIqSensor[VinId][CmdMsg.Ctx.ImageChanIq.ChainId] = (UINT32) CmdMsg.Ctx.ImageChanIq.Id;
                        break;
                    case (UINT8) CHAN_MSG_INTER:
                        AmbaImgPrint(PRINT_FLAG_DBG, "chan-msg-inter", (UINT32) CmdMsg.Ctx.ImageChanInter.Msg);
                        AmbaImgPrint(PRINT_FLAG_DBG, "chain id", CmdMsg.Ctx.ImageChanInter.ChainId);
                        AmbaImgPrint(PRINT_FLAG_DBG, "inter id", CmdMsg.Ctx.ImageChanInter.Id);
                        AmbaImgPrint(PRINT_FLAG_DBG, "inter num", CmdMsg.Ctx.ImageChanInter.Num);
                        AmbaImgPrint(PRINT_FLAG_DBG, "inter skip", CmdMsg.Ctx.ImageChanInter.Skip);
                        AmbaImgPrint(PRINT_FLAG_DBG, "inter mode", CmdMsg.Ctx.ImageChanInter.Mode);
                        FuncRetCode = AmbaImgChannel_InterIdPut(VinId, CmdMsg.Ctx.ImageChanInter.ChainId, CmdMsg.Ctx.ImageChanInter.Id, CmdMsg.Ctx.ImageChanInter.Num, CmdMsg.Ctx.ImageChanInter.Skip, CmdMsg.Ctx.ImageChanInter.Mode);
                        if (FuncRetCode != OK_UL) {
                            AmbaImgPrint(PRINT_FLAG_ERR, "error: chan inter id", FuncRetCode);
                        }
                        break;
                    case (UINT8) CHAN_MSG_AF:
                        AmbaImgPrint(PRINT_FLAG_DBG, "chan-msg-af", (UINT32) CmdMsg.Ctx.ImageChanAf.Msg);
                        AmbaImgPrint(PRINT_FLAG_DBG, "chain id", CmdMsg.Ctx.ImageChanAf.ChainId);
                        AmbaImgPrint(PRINT_FLAG_DBG, "enable", CmdMsg.Ctx.ImageChanAf.Enable);
                        AmbaImgMainAf_Enable[VinId][CmdMsg.Ctx.ImageChanAf.ChainId] = (UINT32) CmdMsg.Ctx.ImageChanAf.Enable;
                        break;
                    case (UINT8) CHAN_MSG_POST_ZONE:
                        AmbaImgPrint(PRINT_FLAG_DBG, "chan-msg-post-zone", (UINT32) CmdMsg.Ctx.ImageChanPostZone.Msg);
                        AmbaImgPrint(PRINT_FLAG_DBG, "chain id", CmdMsg.Ctx.ImageChanPostZone.ChainId);
                        AmbaImgPrint(PRINT_FLAG_DBG, "post zone id", CmdMsg.Ctx.ImageChanPostZone.Id);
                        FuncRetCode = AmbaImgChannel_PostZoneIdPut(VinId, CmdMsg.Ctx.ImageChanPostZone.ChainId, CmdMsg.Ctx.ImageChanPostZone.Id);
                        if (FuncRetCode != OK_UL) {
                            AmbaImgPrint(PRINT_FLAG_ERR, "error: chan post zone id", FuncRetCode);
                        }
                        break;
                    case (UINT8) CHAN_MSG_POST_ZONE_ADD:
                        AmbaImgPrint(PRINT_FLAG_DBG, "chan-msg-post-zone-add", (UINT32) CmdMsg.Ctx.ImageChanPostZone.Msg);
                        AmbaImgPrint(PRINT_FLAG_DBG, "chain id", CmdMsg.Ctx.ImageChanPostZone.ChainId);
                        AmbaImgPrint(PRINT_FLAG_DBG, "post zone id", CmdMsg.Ctx.ImageChanPostZone.Id);
                        FuncRetCode = AmbaImgChannel_PostZoneIdAdd(VinId, CmdMsg.Ctx.ImageChanPostZone.ChainId, CmdMsg.Ctx.ImageChanPostZone.Id);
                        if (FuncRetCode != OK_UL) {
                            AmbaImgPrint(PRINT_FLAG_ERR, "error: chan post zone id add", FuncRetCode);
                        }
                        break;
#ifdef CONFIG_BUILD_IMGFRW_ISO_DIS
                    case (UINT8) CHAN_MSG_ISO_DIS:
                        AmbaImgPrint(PRINT_FLAG_DBG, "chan-msg-iso-dis", (UINT32) CmdMsg.Ctx.ImageChanIso.Msg);
                        AmbaImgPrint(PRINT_FLAG_DBG, "chain id", CmdMsg.Ctx.ImageChanIso.ChainId);
                        AmbaImgPrint(PRINT_FLAG_DBG, "iso disable id", CmdMsg.Ctx.ImageChanIso.DisId);
                        FuncRetCode = AmbaImgChannel_ZoneIsoDisIdPut(VinId, CmdMsg.Ctx.ImageChanIso.ChainId, CmdMsg.Ctx.ImageChanIso.DisId);
                        if (FuncRetCode == OK_UL) {
                            if (CmdMsg.Ctx.ImageChanIso.DisId > 0U) {
                                /* iso cb hook */
                                FuncRetCode = AmbaImgChannel_ZoneIsoCb(VinId, CmdMsg.Ctx.ImageChanIso.ChainId, AmbaImgMain_ImgChanIsoCb);
                                if (FuncRetCode != OK_UL) {
                                    AmbaImgPrint(PRINT_FLAG_ERR, "error: chan zone iso cb (hook)", FuncRetCode);
                                }
                            } else {
                                /* iso cb unhook */
                                FuncRetCode = AmbaImgChannel_ZoneIsoCb(VinId, CmdMsg.Ctx.ImageChanIso.ChainId, NULL);
                                if (FuncRetCode != OK_UL) {
                                    AmbaImgPrint(PRINT_FLAG_ERR, "error: chan zone iso cb (unhook)", FuncRetCode);
                                }
                            }
                        } else {
                            AmbaImgPrint(PRINT_FLAG_ERR, "error: chan zone iso disable id", FuncRetCode);
                        }
                        break;
                    case (UINT8) CHAN_MSG_ISO_DIS_ADD:
                        AmbaImgPrint(PRINT_FLAG_DBG, "chan-msg-iso-dis-add", (UINT32) CmdMsg.Ctx.ImageChanIso.Msg);
                        AmbaImgPrint(PRINT_FLAG_DBG, "chain id", CmdMsg.Ctx.ImageChanIso.ChainId);
                        AmbaImgPrint(PRINT_FLAG_DBG, "iso disable id", CmdMsg.Ctx.ImageChanIso.DisId);
                        FuncRetCode = AmbaImgChannel_ZoneIsoDisIdAdd(VinId, CmdMsg.Ctx.ImageChanIso.ChainId, CmdMsg.Ctx.ImageChanIso.DisId);
                        if (FuncRetCode == OK_UL) {
                            if (CmdMsg.Ctx.ImageChanIso.DisId > 0U) {
                                /* iso cb hook */
                                FuncRetCode = AmbaImgChannel_ZoneIsoCb(VinId, CmdMsg.Ctx.ImageChanIso.ChainId, AmbaImgMain_ImgChanIsoCb);
                                if (FuncRetCode != OK_UL) {
                                    AmbaImgPrint(PRINT_FLAG_ERR, "error: chan zone iso cb (hook)", FuncRetCode);
                                }
                            }
                        } else {
                            AmbaImgPrint(PRINT_FLAG_ERR, "error: chan zone iso disable id add", FuncRetCode);
                        }
                        break;
#endif
                    case (UINT8) CHAN_MSG_AAA_CORE:
                        AmbaImgPrint(PRINT_FLAG_DBG, "chan-msg-aaa-core", (UINT32) CmdMsg.Ctx.ImageChanCore.Msg);
                        AmbaImgPrint(PRINT_FLAG_DBG, "chain id", CmdMsg.Ctx.ImageChanCore.ChainId);
                        AmbaImgPrint(PRINT_FLAG_DBG, "core id", CmdMsg.Ctx.ImageChanCore.Id);
                        AmbaImgMain_ImgAaaCore[VinId][CmdMsg.Ctx.ImageChanCore.ChainId] = CmdMsg.Ctx.ImageChanCore.Id;
                        break;
                    case (UINT8) CHAN_MSG_SLOW_SHUTTER:
                        AmbaImgPrint(PRINT_FLAG_DBG, "chan-msg-slowshutter", (UINT32) CmdMsg.Ctx.ImageChanSlowShutter.Msg);
                        AmbaImgPrint(PRINT_FLAG_DBG, "chain id", CmdMsg.Ctx.ImageChanSlowShutter.ChainId);
                        AmbaImgPrint(PRINT_FLAG_DBG, "enable", CmdMsg.Ctx.ImageChanSlowShutter.Enable);
                        AmbaImgMain_ImgSlowShutterTable[VinId][CmdMsg.Ctx.ImageChanSlowShutter.ChainId] = CmdMsg.Ctx.ImageChanSlowShutter.Enable;
                        break;
#ifdef CONFIG_BUILD_IMGFRW_SMC
                    case (UINT8) CHAN_MSG_SMC:
                        AmbaImgPrint(PRINT_FLAG_DBG, "chan-msg-smc", (UINT32) CmdMsg.Ctx.ImageChanSmc.Msg);
                        AmbaImgPrint(PRINT_FLAG_DBG, "chain id", CmdMsg.Ctx.ImageChanSmc.ChainId);
                        AmbaImgPrintEx(PRINT_FLAG_DBG, "i2s id", CmdMsg.Ctx.ImageChanSmc.I2sId, 16U);
                        AmbaImgMain_ImgSmcI2sId[VinId][CmdMsg.Ctx.ImageChanSmc.ChainId] = CmdMsg.Ctx.ImageChanSmc.I2sId;
                    break;
#endif
                    case (UINT8) CHAN_MSG_VIN_TONE_CURVE:
                        AmbaImgPrint(PRINT_FLAG_DBG, "chan-msg-vin-tone-curve", (UINT32) CmdMsg.Ctx.ImageChanVinToneCurve.Msg);
                        AmbaImgPrint(PRINT_FLAG_DBG, "chain id", CmdMsg.Ctx.ImageChanVinToneCurve.ChainId);
                        AmbaImgPrintEx(PRINT_FLAG_DBG, "enable", CmdMsg.Ctx.ImageChanVinToneCurve.Enable, 16U);
                        AmbaImgMain_VinToneCurveUpd[VinId][CmdMsg.Ctx.ImageChanVinToneCurve.ChainId] = CmdMsg.Ctx.ImageChanVinToneCurve.Enable;
                    break;
                    case (UINT8) CHAN_MSG_SNAP_AEB:
                        AmbaImgPrint(PRINT_FLAG_DBG, "chan-msg-snap-aeb", (UINT32) CmdMsg.Ctx.ImageChanSnapAeb.Msg);
                        AmbaImgPrint(PRINT_FLAG_DBG, "chain id", CmdMsg.Ctx.ImageChanSnapAeb.ChainId);
                        AmbaImgPrint(PRINT_FLAG_DBG, "aeb cnt", CmdMsg.Ctx.ImageChanSnapAeb.Cnt);
                        AmbaImgPrint(PRINT_FLAG_DBG, "aeb num", CmdMsg.Ctx.ImageChanSnapAeb.Num);
                        AmbaImgPrint(PRINT_FLAG_DBG, "aeb den", CmdMsg.Ctx.ImageChanSnapAeb.Den);
                        FuncRetCode = AmbaImgChannel_SnapAebIdPut(VinId, CmdMsg.Ctx.ImageChanSnapAeb.ChainId, CmdMsg.Ctx.Bits.Var);
                        if (FuncRetCode != OK_UL) {
                            AmbaImgPrint(PRINT_FLAG_ERR, "error: chan snap aeb id", FuncRetCode);
                        }
                        break;
                    default:
                        /* */
                        break;
                }
                break;
            case (UINT8) LIVEVIEW_CMD_STOP:
                FuncRetCode = AmbaImgSensorHAL_Stop(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrint2(PRINT_FLAG_ERR, "error: imae sensor hal stop", VinId, FuncRetCode);
                }
                FuncRetCode = AmbaImgSensorSync_Stop(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrint2(PRINT_FLAG_ERR, "error: imae sensor sync stop", VinId, FuncRetCode);
                }
                break;
            case (UINT8) LIVEVIEW_CMD_LOCK:
                AmbaImgPrint2(PRINT_FLAG_DBG, "liveview-cmd-lock", VinId, (UINT32) CmdMsg.Ctx.Bits.Cmd);
                break;
            case (UINT8) LIVEVIEW_CMD_UNLOCK:
                AmbaImgPrint2(PRINT_FLAG_DBG, "liveview-cmd-unlock", VinId, (UINT32) CmdMsg.Ctx.Bits.Cmd);
                break;
            case (UINT8) STATISTICS_CMD_MASK:
                AmbaImgPrint2(PRINT_FLAG_DBG, "statistics-cmd-mask", VinId, (UINT32) CmdMsg.Ctx.Bits.Cmd);
                AmbaImgPrintEx(PRINT_FLAG_DBG, "mask", CmdMsg.Ctx.Stat.Mask, 16U);
                AmbaImgMainStatistics_MaskSet(CmdMsg.Ctx.Stat.Mask);
                break;
            case (UINT8) SYNC_CMD_ENABLE:
                AmbaImgPrint2(PRINT_FLAG_DBG, "sync-cmd-enable", VinId, (UINT32) CmdMsg.Ctx.Bits.Cmd);
                AmbaImgPrintEx(PRINT_FLAG_DBG, "enable", CmdMsg.Ctx.Sync.Enable, 16U);
                AmbaImgMainSync_Enable(VinId, (UINT32) CmdMsg.Ctx.Sync.Enable);
                break;
            case (UINT8) CAPTURE_CMD_START:
                AmbaImgPrint2(PRINT_FLAG_DBG, "capture-cmd-start", VinId, (UINT32) CmdMsg.Ctx.Bits.Cmd);
                AmbaImgSensorHAL_AuxInfo[VinId].Ctx.Data = CmdMsg.Ctx.Data;

                AmbaImgPrint(PRINT_FLAG_DBG, "op mode", AmbaImgSensorHAL_AuxInfo[VinId].Ctx.Bits.OpMode);
                AmbaImgPrint(PRINT_FLAG_DBG, "r2y background", AmbaImgSensorHAL_AuxInfo[VinId].Ctx.Bits.R2yBackground);

                /* dev get */
                AmbaImgMain_DevGet(VinId);
                /* info get */
                AmbaImgMain_InfoGet(VinId);

                /* image sensor HAL reset */
                FuncRetCode = AmbaImgSensorHAL_Reset(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrint2(PRINT_FLAG_ERR, "error: image sensor hal reset", VinId, FuncRetCode);
                }

                /* image sensor sync reset */
                FuncRetCode = AmbaImgSensorSync_Reset(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrint2(PRINT_FLAG_ERR, "error: image sensor sync reset", VinId, FuncRetCode);
                }

                /* image system reset */
                FuncRetCode = AmbaImgSystem_Reset(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrint2(PRINT_FLAG_ERR, "error: image system reset", VinId, FuncRetCode);
                }

                /* debug msg print */
                if ((pAmbaImgMainChannel != NULL) &&
                    (pAmbaImgMainChannel[VinId] != NULL)) {
                    for (i = 0U; pAmbaImgMainChannel[VinId][i].Magic == 0xCafeU; i++) {
                        /* ctx check */
                        if (pAmbaImgMainChannel[VinId][i].pCtx != NULL) {
                            /* chan print */
                            AmbaImgPrint2(PRINT_FLAG_DBG, "hal info chan", VinId, i);
                            AmbaImgPrint(PRINT_FLAG_DBG, "adv ssi cmd", (UINT32) AmbaImgSensorHAL_Info[VinId][i].Op.Ctrl.AdvSsiCmd);
                            AmbaImgPrint(PRINT_FLAG_DBG, "adv svr upd", (UINT32) AmbaImgSensorHAL_Info[VinId][i].Op.Ctrl.AdvSvrUpd);
                            AmbaImgPrint(PRINT_FLAG_DBG, "adv rgb cmd", (UINT32) AmbaImgSensorHAL_Info[VinId][i].Op.Ctrl.AdvRgbCmd);
                            AmbaImgPrint(PRINT_FLAG_DBG, "adv aik cmd", (UINT32) AmbaImgSensorHAL_Info[VinId][i].Op.Ctrl.AdvAikCmd);
                            AmbaImgPrint(PRINT_FLAG_DBG, "post sof",    (UINT32) AmbaImgSensorHAL_Info[VinId][i].Op.Timing.PostSof.Bits.Timetick);
                            AmbaImgPrint(PRINT_FLAG_DBG, "adv dsp",     (UINT32) AmbaImgSensorHAL_Info[VinId][i].Op.Timing.AdvDsp.Bits.Timetick);
                            AmbaImgPrint(PRINT_FLAG_DBG, "adv eof",     (UINT32) AmbaImgSensorHAL_Info[VinId][i].Op.Timing.AdvEof.Bits.Timetick);
                            AmbaImgPrint(PRINT_FLAG_DBG, "aik exec",    (UINT32) AmbaImgSensorHAL_Info[VinId][i].Op.Timing.AikExecute.Bits.Timetick);
                            AmbaImgPrint(PRINT_FLAG_DBG, "mof",         (UINT32) AmbaImgSensorHAL_Info[VinId][i].Op.Timing.Mof.Bits.Timetick);
                        }
                    }
                }

                /* timing debug (TBD) */
                if (AmbaImgMain_DebugStillTiming > 0U) {
                    FuncRetCode = AmbaImgSensorHAL_TimingMarkEn(VinId, 1U);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    FuncRetCode = AmbaImgSensorHAL_TimingMarkClr(VinId);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                }
                break;
            case (UINT8) CAPTURE_CMD_STOP:
                FuncRetCode = AmbaImgSensorHAL_Stop(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrint2(PRINT_FLAG_ERR, "error: imae sensor hal stop", VinId, FuncRetCode);
                }
                FuncRetCode = AmbaImgSensorSync_Stop(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrint2(PRINT_FLAG_ERR, "error: imae sensor sync stop", VinId, FuncRetCode);
                }
                break;
            case (UINT8) STATISTICS_CMD_INTER:
                AmbaImgPrint2(PRINT_FLAG_DBG, "statistics-cmd-inter", VinId, (UINT32) CmdMsg.Ctx.Bits.Cmd);
                AmbaImgMain_ImgStatisticsInter[VinId] = CmdMsg.Ctx.Bits.Var;
                AmbaImgPrint(PRINT_FLAG_DBG, "enable", (UINT32) CmdMsg.Ctx.Bits.Var);
                break;
            default:
                /* */
                break;
        }
    }
}

/**
 *  Amba image main image framework dispatch ex
 *  @param[in] VinId vin id
 *  @param[in] Param command and message
 *  @param[in] Param2 aux data
 *  @note this function is intended for internal use only
 */
static void AmbaImgMain_ImgFrwDispatchEx(UINT32 VinId, UINT64 Param, UINT64 Param2)
{
    UINT32 FuncRetCode;

    AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;
    AMBA_IMG_MAIN_AUX_DATA_s AuxData;

    CmdMsg.Ctx.Data = Param;
    AuxData.Ctx.Data = Param2;

    if (VinId < AMBA_IMG_NUM_VIN_CHANNEL) {
        switch (CmdMsg.Ctx.Bits.Cmd) {
            case (UINT8) IMAGE_CMD_CHAN:
                AmbaImgPrint2(PRINT_FLAG_DBG, "image-cmd-chan", VinId, (UINT32) CmdMsg.Ctx.Bits.Cmd);
                switch (CmdMsg.Ctx.Bits.Msg) {
                    case (UINT8) CHAN_MSG_IMG_MODE:
                        AmbaImgPrint(PRINT_FLAG_DBG, "chan-msg-img-mode", (UINT32) CmdMsg.Ctx.ImageChanImgMode.Msg);
                        AmbaImgPrint(PRINT_FLAG_DBG, "chain id", CmdMsg.Ctx.ImageChanImgMode.ChainId);
                        AmbaImgPrint(PRINT_FLAG_DBG, "type", CmdMsg.Ctx.ImageChanImgMode.Type);
                        AmbaImgPrint64Ex(PRINT_FLAG_DBG, "img mode", AuxData.Ctx.ImageChanImgMode.Data, 16U);
                        if ((pAmbaImgMainChannel != NULL) &&
                            (pAmbaImgMainChannel[VinId] != NULL)) {
                            if (pAmbaImgMainChannel[VinId][CmdMsg.Ctx.ImageChanImgMode.ChainId].Magic == 0xCafeU) {
                                /* ctx check */
                                if (pAmbaImgMainChannel[VinId][CmdMsg.Ctx.ImageChanImgMode.ChainId].pCtx != NULL) {
                                    AMBA_IMG_MEM_INFO_s MemInfo;
                                    UINT32 Type;
                                    AMBA_IMG_CHANNEL_ID_s ImageChanId;
                                    /* image channel get */
                                    ImageChanId.Ctx.Data = pAmbaImgMainChannel[VinId][CmdMsg.Ctx.ImageChanImgMode.ChainId].pCtx->Id.Ctx.Data;
                                    /* img mode get */
                                    MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) AuxData.Ctx.ImageChanImgMode.Data;
                                    /* type get */
                                    Type = CmdMsg.Ctx.ImageChanImgMode.Type;
                                    /* still? */
                                    if (Type == AMBA_IK_PIPE_STILL) {
                                        /* still */
                                        AmbaImgPrint(PRINT_FLAG_DBG, "still context id", MemInfo.Ctx.pImgMode->ContextId);
                                        /* still zone id get */
                                        ImageChanId.Ctx.Bits.ZoneId = AmbaImgStatistics_ZoneIdGet(ImageChanId);
                                        /* img mode put */
                                        FuncRetCode = AmbaImgSystem_ImageModeSet(ImageChanId, AMBA_IK_PIPE_STILL, MemInfo.Ctx.pImgMode);
                                        if (FuncRetCode != OK_UL) {
                                            AmbaImgPrint(PRINT_FLAG_ERR, "error: chan still img mode", FuncRetCode);
                                        }
                                    } else {
                                        /* video: AUTO */
                                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: chan video img mode (auto)");
                                    }
                                }
                            }
                        }
                        break;
                    default:
                        /* */
                        break;
                }
                break;
#ifdef CONFIG_BUILD_IMGFRW_ISO_DIS
            case (UINT8) IMAGE_CMD_ISO_CB:
                AmbaImgPrint2(PRINT_FLAG_DBG, "img-cmd-iso-cb", VinId, (UINT32) CmdMsg.Ctx.Bits.Cmd);
                {
                    AMBA_IMG_MEM_INFO_s MemInfo;
                    MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) AuxData.Ctx.Bits.Var;
                    AmbaImgPrint64Ex(PRINT_FLAG_DBG, "iso cb", (UINT64) MemInfo.Ctx.Data, 16U);
                    AmbaImgMain_UserIsoCbHook(MemInfo.Ctx.pUserIsoCb);
                }
                break;
#endif
            default:
                /* */
                break;
        }
    }
}

/**
 *  Amba image main algorithm dispatch
 *  @param[in] VinId vin id
 *  @param[in] Param command and message
 *  @note this function is intended for internal use only
 */
static void AmbaImgMain_AlgoDispatch(UINT32 VinId, UINT64 Param)
{
    UINT32 FuncRetCode;
    UINT32 AlgoId;
    UINT32 PipeMode = (UINT32) IMAGE_PIPE_NORMAL;
    UINT32 i, j;

    UINT16 ImgIq;
    UINT16 ImgSs;

    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;

    AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

    CmdMsg.Ctx.Data = Param;

    if (VinId < AMBA_IMG_NUM_VIN_CHANNEL) {
        switch (CmdMsg.Ctx.Bits.Cmd) {
            case (UINT8) LIVEVIEW_CMD_START:
                /* */
                if ((pAmbaImgMainChannel != NULL) &&
                    (pAmbaImgMainChannel[VinId] != NULL)) {
                    for (i = 0U; pAmbaImgMainChannel[VinId][i].Magic == 0xCafeU; i++) {
                        if (pAmbaImgMainChannel[VinId][i].pCtx != NULL) {
                            /* ctx get */
                            pCtx = pAmbaImgMainChannel[VinId][i].pCtx;
                            /* algo id get */
                            AlgoId = pCtx->Id.Ctx.Bits.AlgoId;
                            /* algo to image channel */
                            AmbaImgMain_Algo2ChanTable[AlgoId].Ctx.Data = pCtx->Id.Ctx.Data;
                            /* pipe mode */
                            for (j = 0U; j < AMBA_IMG_NUM_FOV_CHANNEL; j++) {
                                if ((pCtx->Id.Ctx.Bits.ZoneId & (((UINT32) 1U) << j)) > 0U) {
                                    /* pipe mode get */
                                    FuncRetCode = AmbaImgChannel_PipeModeGet(VinId, i, j, &PipeMode);
                                    if (FuncRetCode != OK_UL) {
                                        /* pipe mode fail */
                                        PipeMode = (UINT32) IMAGE_PIPE_NORMAL;
                                    }
                                    break;
                                }
                            }
                            AmbaImgPrint2(PRINT_FLAG_MSG, "algo pipe mode", AlgoId, PipeMode);
                            /* pipe mode set */
                            if (PipeMode == (UINT32) IMAGE_PIPE_DRAM_EFCY) {
                                FuncRetCode = AmbaImgProc_SetPipelineMode(AlgoId, 4U/*dram efficiency*/);
                                if (FuncRetCode != 0U) {
                                    /* */
                                }
                            } else {
                                FuncRetCode = AmbaImgProc_SetPipelineMode(AlgoId, 1U/*normal*/);
                                if (FuncRetCode != 0U) {
                                    /* */
                                }
                            }
                            /* hdr video mode set */
                            AmbaImgPrint2(PRINT_FLAG_MSG, "algo hdr mode", AlgoId, AmbaImgMain_ImgVinHdrSensor[VinId][i]);
                            FuncRetCode = AmbaImgProc_HDRSetVideoMode(AlgoId, AmbaImgMain_ImgVinHdrSensor[VinId][i]);
                            if (FuncRetCode != OK) {
                                AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image main hdr modr init");
                                AmbaImgPrint_Flush();
                            }
                            /* ce enable? */
                            AmbaImgPrint2(PRINT_FLAG_MSG, "algo lce", AlgoId, AmbaImgSystem_Info[VinId][pCtx->Id.Ctx.Bits.ChainId].Pipe.Hdr.Bits.LCeEnable);
                            if (AmbaImgSystem_Info[VinId][pCtx->Id.Ctx.Bits.ChainId].Pipe.Hdr.Bits.LCeEnable > 0U) {
                                FuncRetCode = AmbaImgProc_HDRSetLinearCEEnable(AlgoId, 1U);
                                if (FuncRetCode != 0U) {
                                    /* */
                                }
                            } else {
                                FuncRetCode = AmbaImgProc_HDRSetLinearCEEnable(AlgoId, 0U);
                                if (FuncRetCode != 0U) {
                                    /* */
                                }
                            }
                            /* iq table */
                            AmbaImgPrint2(PRINT_FLAG_MSG, "load iq table", AlgoId, AmbaImgMain_ImgVinIqSensor[VinId][i]);
                            ImgIq =  (UINT16) (AmbaImgMain_ImgVinIqSensor[VinId][i] & 0xFFFFU);
                            FuncRetCode = App_Image_Init_Iq_Params_Section(AlgoId, (INT32) ImgIq, (UINT8) AmbaImgSensorHAL_AuxInfo[VinId].Ctx.Bits.IqSecId);
                            if (FuncRetCode != OK) {
                                AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image main iq param init");
                                AmbaImgPrint_Flush();
                            }
                            AmbaImgPrint2(PRINT_FLAG_MSG, "load iq table end", AlgoId, AmbaImgMain_ImgVinIqSensor[VinId][i]);
                            /* slow shutter setting */
                            {
                                AE_CONTROL_s AeControlCap;
                                AmbaImgPrint2(PRINT_FLAG_MSG, "ae slow shutter enable", AlgoId, AmbaImgMain_ImgSlowShutterTable[VinId][i]);
                                FuncRetCode = AmbaImgProc_AEGetAEControlCap(AlgoId, &AeControlCap);
                                if (FuncRetCode == OK_UL) {
                                    ImgSs = (UINT16) (AmbaImgMain_ImgSlowShutterTable[VinId][i] & 0xFFFFU);
                                    AeControlCap.SlowShutter = (INT32) ImgSs;
                                    AeControlCap.PhotoSlowShutter = (INT32) ImgSs;
                                    FuncRetCode = AmbaImgProc_AESetAEControlCap(AlgoId, &AeControlCap);
                                    if (FuncRetCode != OK_UL) {
                                        /* */
                                    }
                                }
                            }
                        }
                    }
                }
                break;
            case (UINT8) IMAGE_CMD_ALGO:
                switch (CmdMsg.Ctx.Bits.Msg) {
                    case (UINT8) ALGO_MSG_IQ_PRELOAD:
                        AmbaImgPrint2(PRINT_FLAG_DBG, "image-cmd-algo", VinId, (UINT32) CmdMsg.Ctx.Bits.Cmd);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "algo-msg-iq-preload", VinId, (UINT32) CmdMsg.Ctx.ImageAlgoIqPreload.Msg);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "hdr mode", VinId, CmdMsg.Ctx.ImageAlgoIqPreload.HdrId);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "iq table", VinId, CmdMsg.Ctx.ImageAlgoIqPreload.Id);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "sec id", VinId, CmdMsg.Ctx.ImageAlgoIqPreload.SecId);
                        AlgoId = CmdMsg.Ctx.ImageAlgoIqPreload.AlgoId;
                        FuncRetCode = AmbaImgProc_HDRSetVideoMode(AlgoId, (UINT32) CmdMsg.Ctx.ImageAlgoIqPreload.HdrId);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        FuncRetCode = App_Image_Iq_Params_load_Section(AlgoId, (INT32) CmdMsg.Ctx.ImageAlgoIqPreload.Id, (UINT8) CmdMsg.Ctx.ImageAlgoIqPreload.SecId);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        break;
                    case (UINT8) ALGO_MSG_IQ_INIT:
                        AmbaImgPrint2(PRINT_FLAG_DBG, "image-cmd-algo", VinId, (UINT32) CmdMsg.Ctx.Bits.Cmd);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "algo-msg-iq-init", VinId, (UINT32) CmdMsg.Ctx.ImageAlgoIqInit.Msg);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "hdr mode", VinId, CmdMsg.Ctx.ImageAlgoIqInit.HdrId);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "iq table", VinId, CmdMsg.Ctx.ImageAlgoIqInit.Id);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "sec id", VinId, CmdMsg.Ctx.ImageAlgoIqInit.SecId);
                        AlgoId = CmdMsg.Ctx.ImageAlgoIqInit.AlgoId;
                        FuncRetCode = AmbaImgProc_HDRSetVideoMode(AlgoId, (UINT32) CmdMsg.Ctx.ImageAlgoIqInit.HdrId);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        FuncRetCode = App_Image_Init_Iq_Params_Section(AlgoId, (INT32) CmdMsg.Ctx.ImageAlgoIqInit.Id, (UINT8) CmdMsg.Ctx.ImageAlgoIqInit.SecId);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        break;
                    case (UINT8) ALGO_MSG_DEFOG:
                        AmbaImgPrint2(PRINT_FLAG_DBG, "image-cmd-algo", VinId, (UINT32) CmdMsg.Ctx.Bits.Cmd);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "algo-msg-defog", VinId, (UINT32) CmdMsg.Ctx.ImageAlgoDefog.Msg);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "enable", VinId, CmdMsg.Ctx.ImageAlgoDefog.Enable);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "scene mode", VinId, CmdMsg.Ctx.ImageAlgoDefog.SceneMode);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "algo id", VinId, CmdMsg.Ctx.ImageAlgoDefog.AlgoId);
                        AlgoId = CmdMsg.Ctx.ImageAlgoDefog.AlgoId;
                        Amba_AdjSetDefog(AlgoId, (UINT32) CmdMsg.Ctx.ImageAlgoDefog.Enable, (INT32) CmdMsg.Ctx.ImageAlgoDefog.SceneMode);
                        break;
                    default:
                        /* */
                        break;
                }
                break;
            case (UINT8) LIVEVIEW_CMD_STOP:
                /* */
                break;
            case (UINT8) LIVEVIEW_CMD_LOCK:
                /* aeb: TBD */
                if ((pAmbaImgMainChannel != NULL) &&
                    (pAmbaImgMainChannel[VinId] != NULL)) {
                    for (i = 0U; pAmbaImgMainChannel[VinId][i].Magic == 0xCafeU; i++) {
                        if (pAmbaImgMainChannel[VinId][i].pCtx != NULL) {
                            /* ctx get */
                            pCtx = pAmbaImgMainChannel[VinId][i].pCtx;
                            /* algo id get */
                            AlgoId = pCtx->Id.Ctx.Bits.AlgoId;
                            /* aeb? */
                            if (pCtx->Snap.Ctx.Bits.AebId > 0U) {
                                AEB_INFO_s AmbaImgMain_AebInfo = {0};
                                UINT32 AebCnt;
                                UINT32 AebNum;
                                UINT32 AebDen;
                                UINT32 AebStep;
                                UINT32 AebEv;
                                /* aeb cnt/num/den get */
                                AebCnt = pCtx->Snap.Ctx.Bits2.AebCnt;
                                AebNum = pCtx->Snap.Ctx.Bits2.AebNum;
                                AebDen = pCtx->Snap.Ctx.Bits2.AebDen;
                                /* aeb step */
                                AebStep = AebCnt/2U;
                                /* aeb table put */
                                AmbaImgPrint(PRINT_FLAG_DBG, "chaid id", pCtx->Id.Ctx.Bits.ChainId);
                                AmbaImgPrint(PRINT_FLAG_DBG, "algo id", pCtx->Id.Ctx.Bits.AlgoId);
                                for (j = 1U; j <= AebStep; j++) {
                                    AebEv = (AebNum*32U*j)/(AebDen*AebStep);
                                    AmbaImgMain_AebInfo.EvBias[j] = (AebEv < 128U) ? (INT8) AebEv : (INT8) 127L;
                                    AmbaImgMain_AebInfo.EvBias[j+1U] = (INT8) (-1L * ((AebEv < 128U) ? (INT8) AebEv : 127L));
                                    AmbaImgPrint2(PRINT_FLAG_DBG, " aeb", j, (AebEv*1000U)/32U);
                                }
                                /* aeb cnt put */
                                AmbaImgMain_AebInfo.Num = (UINT8) AebCnt;
                                /* algo aeb setting */
                                FuncRetCode = AmbaImgProc_AESetAEBInfo(AlgoId, &AmbaImgMain_AebInfo);
                                if (FuncRetCode != OK_UL) {
                                    /* */
                                }
                            }
                        }
                    }
                }
                break;
            case (UINT8) CAPTURE_CMD_START:
                /* */
                break;
            default:
                /* */
                break;
        }
    }
}

/**
 *  Amba image main algorithm dispatch ex
 *  @param[in] VinId vin id
 *  @param[in] Param command and message
 *  @param[in] Param2 aux data
 *  @note this function is intended for internal use only
 */
static void AmbaImgMain_AlgoDispatchEx(UINT32 VinId, UINT64 Param, UINT64 Param2)
{
    UINT32 FuncRetCode;
    UINT32 i;

    AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;
    AMBA_IMG_MAIN_AUX_DATA_s AuxData;

    CmdMsg.Ctx.Data = Param;
    AuxData.Ctx.Data = Param2;

    if (VinId < AMBA_IMG_NUM_VIN_CHANNEL) {
        switch (CmdMsg.Ctx.Bits.Cmd) {
            case (UINT8) IMAGE_CMD_ALGO:
                switch (CmdMsg.Ctx.Bits.Msg) {
                    case (UINT8) ALGO_MSG_INFO_GET:
                        AmbaImgPrint2(PRINT_FLAG_CYCLE, "image-cmd-algo", VinId, (UINT32) CmdMsg.Ctx.Bits.Cmd);
                        AmbaImgPrint2(PRINT_FLAG_CYCLE, "algo-msg-info-get", VinId, (UINT32) CmdMsg.Ctx.ImageAlgoInfo.Msg);
                        AmbaImgPrint2(PRINT_FLAG_CYCLE, "id", VinId, CmdMsg.Ctx.ImageAlgoInfo.Id);
                        AmbaImgPrint64Ex2(PRINT_FLAG_CYCLE, "addr", (UINT64) VinId, 10U, (UINT64) AuxData.Ctx.ImageAlgoInfo.Var, 16U);
                        {
                            AMBA_IMG_MEM_INFO_s MemInfo;
                            AMBA_IMG_MEM_INFO_s MemInfo2;
                            AMBA_IMG_MEM_INFO_s MemInfo3;

                            AMBA_IMG_SENSOR_HAL_DATA_REG_s Reg;
                            AMBA_IMG_CHANNEL_ID_s ImageChanId;

                            FLOAT VarF;
                            UINT32 ExposureNum;

                            /* image channel id get */
                            ImageChanId.Ctx.Data = AmbaImgMain_Algo2ChanTable[CmdMsg.Ctx.ImageAlgoInfo.Id].Ctx.Data;
                            /* exposure num */
                            ExposureNum = (UINT32) AmbaImgSystem_Info[VinId][ImageChanId.Ctx.Bits.ChainId].Pipe.Hdr.Bits.ExposureNum;
                            /* algo msg get */
                            MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) AuxData.Ctx.ImageAlgoInfo.Var;
                            /* algo info get by raw cap seqence */
                            FuncRetCode = AmbaImgSensorHAL_RingGet(ImageChanId, &Reg, MemInfo.Ctx.pAlgoMsg->RawCapSeq);
                            if (FuncRetCode == OK_UL) {
                                /* user get */
                                MemInfo2.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) Reg.User;
                                /* fov */
                                MemInfo.Ctx.pAlgoMsg->Info.Fov.SelectBits = ImageChanId.Ctx.Bits.ZoneId;
                                AmbaImgPrintEx(PRINT_FLAG_CYCLE, "fov", MemInfo.Ctx.pAlgoMsg->Info.Fov.SelectBits, 16U);
                                /* ae */
                                MemInfo.Ctx.pAlgoMsg->Info.ExposureNum = ExposureNum;
                                for (i = 0U; i < ExposureNum; i++) {
                                    MemInfo.Ctx.pAlgoMsg->Info.Ae[i].ExposureTime = MemInfo2.Ctx.pUser->AeInfo.Ctx[i].ShutterTime;
                                    MemInfo.Ctx.pAlgoMsg->Info.Ae[i].Gain = MemInfo2.Ctx.pUser->AeInfo.Ctx[i].AgcGain;
                                    VarF = MemInfo.Ctx.pAlgoMsg->Info.Ae[i].ExposureTime*1000000.0f;
                                    AmbaImgPrint2(PRINT_FLAG_CYCLE, "exposure", i, (VarF > 0.0f) ? (UINT32) VarF : 0U);
                                    VarF = MemInfo.Ctx.pAlgoMsg->Info.Ae[i].Gain*1000.0f;
                                    AmbaImgPrint2(PRINT_FLAG_CYCLE, "gain", i, (VarF > 0.0f) ? (UINT32) VarF : 0U);
                                }
                                /* awb */
                                for (i = 0U; i < 3U; i++) {
                                    MemInfo.Ctx.pAlgoMsg->Info.Awb.Wgc[i] = MemInfo2.Ctx.pUser->WbInfo.Dsp.Wgc[0][i];
                                    AmbaImgPrint2(PRINT_FLAG_CYCLE, "wgc", i, MemInfo.Ctx.pAlgoMsg->Info.Awb.Wgc[i]);
                                }
                                /* ae ctx put */
                                MemInfo3.Ctx.pAeInfo = MemInfo2.Ctx.pUser->AeInfo.Ctx;
                                MemInfo.Ctx.pAlgoMsg->Info.Ctx.pAeInfo = MemInfo3.Ctx.pVoid;
                                /* debug msg */
                                AmbaImgPrint64Ex(PRINT_FLAG_CYCLE, "ae info", (UINT64) MemInfo3.Ctx.Data, 16U);
#ifdef CONFIG_BUILD_IMGFRW_RAW_ENC
                                /* aaa ctx put */
                                MemInfo3.Ctx.pAaaInfo = &(MemInfo2.Ctx.pUser->AaaInfo);
                                MemInfo.Ctx.pAlgoMsg->Info.Ctx.pAaaInfo = MemInfo3.Ctx.pVoid;
                                /* debug msg */
                                AmbaImgPrint64Ex(PRINT_FLAG_CYCLE, "aaa info", (UINT64) MemInfo3.Ctx.Data, 16U);
                                /* pipe ctx */
                                {
                                    UINT32 RawCapSeq = MemInfo.Ctx.pAlgoMsg->RawCapSeq;
                                    UINT32 FovSelectBits = MemInfo.Ctx.pAlgoMsg->Info.Fov.SelectBits;

                                    AMBA_IMG_CHANNEL_ID_s ImageChanId2;

                                    ImageChanId2.Ctx.Data = ImageChanId.Ctx.Data;

                                    for (i = 0U; i < AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
                                        if ((FovSelectBits >> i) == 0U) {
                                            break;
                                        }
                                        if ((FovSelectBits & (((UINT32) 1U) << i)) > 0U) {
                                            ImageChanId2.Ctx.Bits.ZoneId = ((UINT32) 1U) << i;
                                            /* ring pipe get */
                                            FuncRetCode = AmbaImgSystem_RingGet(ImageChanId2, &(MemInfo3.Ctx.pRingPipe), RawCapSeq);
                                            if (FuncRetCode == OK_UL) {
                                                /* fov pipe put */
                                                MemInfo.Ctx.pAlgoMsg->Info.Fov.pPipe[i] = MemInfo3.Ctx.pFovPipe;
                                                /* debug msg */
                                                AmbaImgPrint64Ex2(PRINT_FLAG_CYCLE, "pip info", (UINT64) i, 16U, (UINT64) MemInfo3.Ctx.Data, 16U);
#if 1
                                                /* debug msg */
                                                {
                                                    UINT32 FeChanNum;
                                                    UINT32 j;
                                                    if (AmbaImgSystem_Info[VinId][ImageChanId2.Ctx.Bits.ChainId].Pipe.Hdr.Bits.Enable == 1U) {
                                                        FeChanNum = ExposureNum;
                                                    } else {
                                                        FeChanNum = 1U;
                                                    }
                                                    /* fe */
                                                    for (j = 0U; j < FeChanNum; j++) {
                                                        AmbaImgPrint2(PRINT_FLAG_CYCLE, "fe r gain", j, MemInfo.Ctx.pAlgoMsg->Info.Fov.pPipe[i]->Wb.Fe[j].Rgain);
                                                        AmbaImgPrint2(PRINT_FLAG_CYCLE, "fe g gain", j, MemInfo.Ctx.pAlgoMsg->Info.Fov.pPipe[i]->Wb.Fe[j].Ggain);
                                                        AmbaImgPrint2(PRINT_FLAG_CYCLE, "fe b gain", j, MemInfo.Ctx.pAlgoMsg->Info.Fov.pPipe[i]->Wb.Fe[j].Bgain);
                                                        AmbaImgPrint2(PRINT_FLAG_CYCLE, "fe shr ratio", j, MemInfo.Ctx.pAlgoMsg->Info.Fov.pPipe[i]->Wb.Fe[j].ShutterRatio);
                                                    }
                                                    /* be */
                                                    AmbaImgPrint2(PRINT_FLAG_CYCLE, "be r gain", j, MemInfo.Ctx.pAlgoMsg->Info.Fov.pPipe[i]->Wb.Be.GainR);
                                                    AmbaImgPrint2(PRINT_FLAG_CYCLE, "be g gain", j, MemInfo.Ctx.pAlgoMsg->Info.Fov.pPipe[i]->Wb.Be.GainG);
                                                    AmbaImgPrint2(PRINT_FLAG_CYCLE, "be b gain", j, MemInfo.Ctx.pAlgoMsg->Info.Fov.pPipe[i]->Wb.Be.GainB);
                                                    /* be alt */
                                                    AmbaImgPrint2(PRINT_FLAG_CYCLE, "be alt r gain", j, MemInfo.Ctx.pAlgoMsg->Info.Fov.pPipe[i]->Wb.BeAlt.GainR);
                                                    AmbaImgPrint2(PRINT_FLAG_CYCLE, "be alt g gain", j, MemInfo.Ctx.pAlgoMsg->Info.Fov.pPipe[i]->Wb.BeAlt.GainG);
                                                    AmbaImgPrint2(PRINT_FLAG_CYCLE, "be alt b gain", j, MemInfo.Ctx.pAlgoMsg->Info.Fov.pPipe[i]->Wb.BeAlt.GainB);
                                                }
#endif
                                            } else {
                                                MemInfo.Ctx.pAlgoMsg->Info.Fov.pPipe[i] = NULL;
                                            }
                                        }
                                    }
                                }
#else
                                /* aaa ctx */
                                MemInfo.Ctx.pAlgoMsg->Info.Ctx.pAaaInfo = NULL;
                                /* pipe ctx */
                                {
                                    UINT32 FovSelectBits = MemInfo.Ctx.pAlgoMsg->Info.Fov.SelectBits;

                                    for (i = 0U; i < AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
                                        if ((FovSelectBits >> i) == 0U) {
                                            break;
                                        }
                                        if ((FovSelectBits & (((UINT32) 1U) << i)) > 0U) {
                                            MemInfo.Ctx.pAlgoMsg->Info.Fov.pPipe[i] = NULL;
                                        }
                                    }
                                }
#endif
                                /* sucess  */
                                MemInfo.Ctx.pAlgoMsg->RetCode = OK_UL;
                            } else {
                                /* fail */
                                MemInfo.Ctx.pAlgoMsg->RetCode = NG_UL;
                            }
                        }
                        break;
                    case (UINT8) ALGO_MSG_AE_STABLE_WAIT:
                        AmbaImgPrint2(PRINT_FLAG_STATUS, "image-cmd-algo", VinId, (UINT32) CmdMsg.Ctx.Bits.Cmd);
                        AmbaImgPrint2(PRINT_FLAG_STATUS, "algo-msg-ae-stable-wait", VinId, (UINT32) CmdMsg.Ctx.Bits2.Msg);
                        AmbaImgPrint2(PRINT_FLAG_STATUS, "chain id", VinId, CmdMsg.Ctx.Bits2.ChainId);
                        AmbaImgPrintEx2(PRINT_FLAG_STATUS, "timeout", VinId, 10U, CmdMsg.Ctx.Bits2.Var, 16U);
                        AmbaImgPrint64Ex2(PRINT_FLAG_STATUS, "addr", (UINT64) VinId, 10U, (UINT64) AuxData.Ctx.Bits.Var, 16U);
                        {
                            AMBA_IMG_MEM_INFO_s MemInfo;
                            /* algo msg get */
                            MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) AuxData.Ctx.Bits.Var;
                            /* ae stable wait */
                            FuncRetCode = AmbaImgMainAe_StableWait(VinId, CmdMsg.Ctx.Bits2.ChainId, CmdMsg.Ctx.Bits.Var);
                            /* ret code put */
                            MemInfo.Ctx.pRetMsg->RetCode = FuncRetCode;
                        }
                        break;
                    default:
                        /* */
                        break;
                }
                break;
            default:
                /* */
                break;
        }
    }
}

/**
 *  Amba image main task dispatch
 *  @param[in] VinId vin id
 *  @param[in] Param command and message
 *  @note this function is intended for internal use only
 */
static void AmbaImgMain_TaskDispatch(UINT32 VinId, UINT64 Param)
{
    UINT32 FuncRetCode;

    AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

    CmdMsg.Ctx.Data = Param;

    if (VinId < AMBA_IMG_NUM_VIN_CHANNEL) {
        switch (CmdMsg.Ctx.Bits.Cmd) {
            case (UINT8) LIVEVIEW_CMD_START:
                /* ae task create */
                FuncRetCode = AmbaImgMainAe_Create(VinId, 14U, 0x01U);
                if (FuncRetCode == OK_UL) {
                    /* ae task idle wait */
                    FuncRetCode = AmbaImgMainAe_IdleWait(VinId);
                    if (FuncRetCode == OK_UL) {
                        /* ae task active */
                        FuncRetCode = AmbaImgMainAe_Active(VinId);
                        if (FuncRetCode != OK_UL) {
                            AmbaImgPrintStr(PRINT_FLAG_ERR, "error: ae task active");
                        }
                    } else {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: ae task ready");
                    }
                } else {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: ae task create");
                }
                /* awb task create */
                FuncRetCode = AmbaImgMainAwb_Create(VinId, 18U, 0x01U);
                if (FuncRetCode == OK_UL) {
                    /* awb task idle wait */
                    FuncRetCode = AmbaImgMainAwb_IdleWait(VinId);
                    if (FuncRetCode == OK_UL) {
                        /* awb task active */
                        FuncRetCode = AmbaImgMainAwb_Active(VinId);
                        if (FuncRetCode != OK_UL) {
                            AmbaImgPrintStr(PRINT_FLAG_ERR, "error: awb task active");
                        }
                    } else {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: awb task ready");
                    }
                } else {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: awb task create");
                }
                /* adj task create */
                FuncRetCode = AmbaImgMainAdj_Create(VinId, 22U, 0x01U);
                if (FuncRetCode == OK_UL) {
                    /* adj task idle wait */
                    FuncRetCode = AmbaImgMainAdj_IdleWait(VinId);
                    if (FuncRetCode == OK_UL) {
                        /* adj task active */
                        FuncRetCode = AmbaImgMainAdj_Active(VinId);
                        if (FuncRetCode != OK_UL) {
                            AmbaImgPrintStr(PRINT_FLAG_ERR, "error: adj task active");
                        }
                    } else {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: adj task ready");
                    }
                } else {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: adj task create");
                }
                /* af task create */
                FuncRetCode = AmbaImgMainAf_Create(VinId, 12U, 0x01U);
                if (FuncRetCode == OK_UL) {
                    /* af task idle wait */
                    FuncRetCode = AmbaImgMainAf_IdleWait(VinId);
                    if (FuncRetCode == OK_UL) {
                        /* af task active */
                        FuncRetCode = AmbaImgMainAf_Active(VinId);
                        if (FuncRetCode != OK_UL) {
                            AmbaImgPrintStr(PRINT_FLAG_ERR, "error: af task active");
                        }
                    } else {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: af task ready");
                    }
                } else {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: af task create");
                }
                break;
            case (UINT8) LIVEVIEW_CMD_STOP:
                /* ae task inactive */
                FuncRetCode = AmbaImgMainAe_Inactive(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: ae task inactive");
                }
                /* awb task inactive */
                FuncRetCode = AmbaImgMainAwb_Inactive(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: awb task inactive");
                }
                /* adj task inactive */
                FuncRetCode = AmbaImgMainAdj_Inactive(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: adj task inactive");
                }
                /* af task inactive */
                FuncRetCode = AmbaImgMainAf_Inactive(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: af task inactive");
                }
                /* statistics push */
                FuncRetCode = AmbaImgStatistics_Push(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: statistics push");
                }
                /* ae task idle wait */
                FuncRetCode = AmbaImgMainAe_IdleWait(VinId);
                if (FuncRetCode == OK_UL) {
                    /* ae task delete */
                    FuncRetCode = AmbaImgMainAe_Delete(VinId);
                    if (FuncRetCode != OK_UL) {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: ae task delete");
                    }
                } else {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: ae task idle");
                }
                /* awb task idle wait */
                FuncRetCode = AmbaImgMainAwb_IdleWait(VinId);
                if (FuncRetCode == OK_UL) {
                    /* awb task delete */
                    FuncRetCode = AmbaImgMainAwb_Delete(VinId);
                    if (FuncRetCode != OK_UL) {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: awb task delete");
                    }
                } else {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: awb task idle");
                }
                /* adj task idle wait */
                FuncRetCode = AmbaImgMainAdj_IdleWait(VinId);
                if (FuncRetCode == OK_UL) {
                    /* adj task delete */
                    FuncRetCode = AmbaImgMainAdj_Delete(VinId);
                    if (FuncRetCode != OK_UL) {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: adj task delete");
                    }
                } else {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: adj task dile");
                }
                /* af task idle wait */
                FuncRetCode = AmbaImgMainAf_IdleWait(VinId);
                if (FuncRetCode == OK_UL) {
                    /* af task delete */
                    FuncRetCode = AmbaImgMainAf_Delete(VinId);
                    if (FuncRetCode != OK_UL) {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: af task delete");
                    }
                } else {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: af task dile");
                }
                break;
            case (UINT8) LIVEVIEW_CMD_LOCK:
                /* lock */
                FuncRetCode = AmbaImgMainAe_Lock(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: ae task lock");
                }
                FuncRetCode = AmbaImgMainAwb_Lock(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: awb task lock");
                }
                FuncRetCode = AmbaImgMainAdj_Lock(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: adj task lock");
                }
                FuncRetCode = AmbaImgMainAf_Lock(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: af task lock");
                }
                /* lock wait */
                FuncRetCode = AmbaImgMainAe_LockWait(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: ae task lock wait");
                }
                FuncRetCode = AmbaImgMainAwb_LockWait(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: awb task lock wait");
                }
                FuncRetCode = AmbaImgMainAdj_LockWait(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: adj task lock wait");
                }
                FuncRetCode = AmbaImgMainAf_LockWait(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: af task lock wait");
                }
                break;
            case (UINT8) LIVEVIEW_CMD_UNLOCK:
                /* unlock */
                FuncRetCode = AmbaImgMainAe_Unlock(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: ae task unlock");
                }
                FuncRetCode = AmbaImgMainAwb_Unlock(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: awb task unlock");
                }
                FuncRetCode = AmbaImgMainAdj_Unlock(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: adj task unlock");
                }
                FuncRetCode = AmbaImgMainAf_Unlock(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: af task unlock");
                }
                break;
            case (UINT8) SYNC_CMD_START:
                /* sync task create */
                FuncRetCode = AmbaImgMainSync_Create(VinId, 5U, 0x01U);
                if (FuncRetCode == OK_UL) {
                    /* sync task idle wait */
                    FuncRetCode = AmbaImgMainSync_IdleWait(VinId);
                    if (FuncRetCode == OK_UL) {
                        /* sync task active */
                        FuncRetCode = AmbaImgMainSync_Active(VinId);
                        if (FuncRetCode != OK_UL) {
                            AmbaImgPrintStr(PRINT_FLAG_ERR, "error: sync task active");
                        }
                    } else {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: sync task ready");
                    }
                } else {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: sync task create");
                }
                break;
            case (UINT8) SYNC_CMD_STOP:
                /* sync task inactive */
                FuncRetCode = AmbaImgMainSync_Inactive(VinId);
                if (FuncRetCode == OK_UL) {
                    /* sync task push */
                    FuncRetCode = AmbaImgMainSync_Push(VinId);
                    if (FuncRetCode != OK_UL) {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: sync task push");
                    }
                    /* sync task idle wait */
                    FuncRetCode = AmbaImgMainSync_IdleWait(VinId);
                    if (FuncRetCode != OK_UL) {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: sync task idle");
                    }
                } else {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: sync task inactive");
                }
                /* sync task delete */
                FuncRetCode = AmbaImgMainSync_Delete(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: sync task delete");
                }
                break;
            case (UINT8) STATISTICS_CMD_START:
                /* statistics task create */
                FuncRetCode = AmbaImgMainStatistics_Create(11U, 0x01U);
                if (FuncRetCode == OK_UL) {
                    /* statistics task idle wait */
                    FuncRetCode = AmbaImgMainStatistics_IdleWait();
                    if (FuncRetCode == OK_UL) {
                        /* statistics task active */
                        FuncRetCode = AmbaImgMainStatistics_Active();
                        if (FuncRetCode != OK_UL) {
                            AmbaImgPrintStr(PRINT_FLAG_ERR, "error: statistics task active");
                        }
                    } else {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: statistics task ready");
                    }
                } else {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: statistics task create");
                }
                break;
            case (UINT8) STATISTICS_CMD_STOP:
                /* statistics task inactive */
                FuncRetCode = AmbaImgMainStatistics_Inactive();
                if (FuncRetCode == OK_UL) {
                    /* statistics task push */
                    FuncRetCode = AmbaImgMainStatistics_Push();
                    if (FuncRetCode != OK_UL) {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: statistics task push");
                    }
                    /* statistics task idle wait */
                    FuncRetCode = AmbaImgMainStatistics_IdleWait();
                    if (FuncRetCode != OK_UL) {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: statistics task idle");
                    }
                } else {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: statistics task inactive");
                }
                /* statistics task delete */
                FuncRetCode = AmbaImgMainStatistics_Delete();
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: statistics task delete");
                }
                break;
#ifdef CONFIG_BUILD_IMGFRW_SMC
            case (UINT8) SMC_CMD_ON:
                /* smc on */
                FuncRetCode = AmbaImgMainAe_SmcOn(VinId);
                if (FuncRetCode != OK_UL) {
                    /* */
                }
                break;
            case (UINT8) SMC_CMD_OFF:
                /* smc off */
                FuncRetCode = AmbaImgMainAe_SmcOff(VinId);
                if (FuncRetCode != OK_UL) {
                    /* */
                }
                break;
#endif
            default:
                /* */
                break;
        }
    }
}

/**
 *  Amba image main aaa algorithm id get
 *  @param[in] ImageChanId image channel id
 *  @return algorithm id
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMain_AaaAlgoIdGet(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 i;

    UINT32 VrId;
    UINT32 VrAltId;

    UINT32 mVinId = 0U;
    UINT32 mChainId = 0U;

    UINT32 mAlgoId;

    const AMBA_IMG_CHANNEL_VR_MAP_s *pVrMap;
    AMBA_IMG_CHANNEL_ID_s ImageChanId2;

    /* image channel id */
    ImageChanId2.Ctx.Data = pAmbaImgMainChannel[ImageChanId.Ctx.Bits.VinId][ImageChanId.Ctx.Bits.ChainId].pCtx->Id.Ctx.Data;
    /* vr map get */
    pVrMap = &(pAmbaImgMainChannel[ImageChanId.Ctx.Bits.VinId][ImageChanId.Ctx.Bits.ChainId].pCtx->VrMap);
    /* vr? */
    if (pVrMap->Id.Ctx.Bits.VrId > 0U) {
        /* vr id */
        VrId = pVrMap->Id.Ctx.Bits.VrId;
        /* vr alt id */
        VrAltId = pVrMap->Id.Ctx.Bits.VrAltId;
        /* vr master? */
        if (((VrId & (((UINT32) 1U) << ImageChanId2.Ctx.Bits.VinId)) > 0U) &&
            ((VrAltId & (((UINT32) 1U) << ImageChanId2.Ctx.Bits.ChainId)) > 0U)) {
            /* algo id (vr) */
            mAlgoId = ImageChanId2.Ctx.Bits.AlgoId;
        } else {
            /* slave */
            for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
                if ((VrId & (((UINT32) 1U) << i)) > 0U) {
                    /* master vin id */
                    mVinId = i;
                    break;
                }
            }
            for (i = 0U; i < AMBA_IMG_NUM_VIN_ALGO; i++) {
                if ((VrAltId & (((UINT32) 1U) << i)) > 0U) {
                    /* master chain id */
                    mChainId = i;
                    break;
                }
            }
            /* algo id (vr) */
            mAlgoId = pAmbaImgMainChannel[mVinId][mChainId].pCtx->Id.Ctx.Bits.AlgoId;
        }
    } else {
        /* algo id (non-vr) */
        mAlgoId = ImageChanId2.Ctx.Bits.AlgoId;
    }

    return mAlgoId;
}

/**
 *  Amba image main snap dispatch
 *  @param[in] VinId vin id
 *  @param[in] Param command and message
 *  @note this function is intended for internal use only
 */
static void AmbaImgMain_SnapDispatch(UINT32 VinId, UINT64 Param)
{
    UINT32 FuncRetCode;
    UINT32 AlgoId;
    UINT32 i;

    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;

    AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

    CmdMsg.Ctx.Data = Param;

    if (VinId < AMBA_IMG_NUM_VIN_CHANNEL) {
        switch (CmdMsg.Ctx.Bits.Cmd) {
            case (UINT8) CAPTURE_CMD_START:
                /* still adj flag reset */
                if ((pAmbaImgMainChannel != NULL) &&
                    (pAmbaImgMainChannel[VinId] != NULL)) {
                    for (i = 0U; pAmbaImgMainChannel[VinId][i].Magic == 0xCafeU; i++) {
                        if (pAmbaImgMainChannel[VinId][i].pCtx != NULL) {
                            /* ctx get */
                            pCtx = pAmbaImgMainChannel[VinId][i].pCtx;
                            /* algo id get */
                            AlgoId = AmbaImgMain_AaaAlgoIdGet(pCtx->Id);
                            /* algo still reset */
#ifndef CONFIG_BUILD_IMGFRW_AAA_ADJ_EXT
                            FuncRetCode = AmbaImgProc_ADJResetStillFlags(AlgoId, 0U/*iso*/);
                            if (FuncRetCode != OK_UL) {
                                /* */
                            }
                            FuncRetCode = AmbaImgProc_ADJResetStillFlags(AlgoId, 1U/*hdr*/);
                            if (FuncRetCode != OK_UL) {
                                /* */
                            }
                            FuncRetCode = AmbaImgProc_ADJResetStillFlags(AlgoId, 2U/*ce*/);
                            if (FuncRetCode != OK_UL) {
                                /* */
                            }
#else
                            FuncRetCode = Amba_Adj_ResetStillFlags(AlgoId, 0U/*iso*/);
                            if (FuncRetCode != OK_UL) {
                                /* */
                            }
                            FuncRetCode = Amba_Adj_ResetStillFlags(AlgoId, 1U/*hdr*/);
                            if (FuncRetCode != OK_UL) {
                                /* */
                            }
                            FuncRetCode = Amba_Adj_ResetStillFlags(AlgoId, 2U/*ce*/);
                            if (FuncRetCode != OK_UL) {
                                /* */
                            }
#endif
                        }
                    }
                }
                break;
            default:
                /* */
                break;
        }
    }
}

/**
 *  Amba image main snap dispatch ex
 *  @param[in] VinId vin id
 *  @param[in] Param command and message
 *  @param[in] Param2 aux data
 *  @note this function is intended for internal use only
 */
static void AmbaImgMain_SnapDispatchEx(UINT32 VinId, UINT64 Param, UINT64 Param2)
{
    UINT32 FuncRetCode;

    AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;
    AMBA_IMG_MAIN_AUX_DATA_s AuxData;

    CmdMsg.Ctx.Data = Param;
    AuxData.Ctx.Data = Param2;

    if (VinId < AMBA_IMG_NUM_VIN_CHANNEL) {
        switch (CmdMsg.Ctx.Bits.Cmd) {
            case (UINT8) CAPTURE_CMD_AAA:
                /* aaa */
                AmbaImgPrint2(PRINT_FLAG_DBG, "capture-cmd-aaa", VinId, (UINT32) CmdMsg.Ctx.Bits.Cmd);
                AmbaImgPrint(PRINT_FLAG_DBG, "chain id", (UINT32) CmdMsg.Ctx.StillAaa.ChainId);
                AmbaImgPrint(PRINT_FLAG_DBG, "snap count", (UINT32) CmdMsg.Ctx.StillAaa.SnapCount);
                AmbaImgPrint64Ex(PRINT_FLAG_DBG, "is hiso", AuxData.Ctx.StillAaa.pIsHiso, 16U);
                {
                    UINT32 ChainId;
                    UINT32 SnapCount;
                    UINT32 *pIsHiso;

                    AMBA_IMG_MEM_INFO_s MemInfo;

                    ChainId = CmdMsg.Ctx.StillAaa.ChainId;
                    SnapCount = CmdMsg.Ctx.StillAaa.SnapCount;

                    /* hiso get */
                    MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) AuxData.Ctx.StillAaa.pIsHiso;
                    pIsHiso = MemInfo.Ctx.pUint32;

                    /* still ae */
                    FuncRetCode = AmbaImgMainAe_Snap(VinId, ChainId, SnapCount, pIsHiso);
                    if (FuncRetCode != OK_UL) {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: snap ae");
                    }
                    /* still awb */
                    FuncRetCode = AmbaImgMainAwb_Snap(VinId, ChainId, SnapCount, pIsHiso);
                    if (FuncRetCode != OK_UL) {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: snap awb");
                    }
                    /* still adj */
                    FuncRetCode = AmbaImgMainAdj_Snap(VinId, ChainId, SnapCount, pIsHiso);
                    if (FuncRetCode != OK_UL) {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: snap adj");
                    }
                }
                break;
            case (UINT8) CAPTURE_CMD_ISO:
                /* iso */
                AmbaImgPrint2(PRINT_FLAG_DBG, "capture-cmd-iso", VinId, (UINT32) CmdMsg.Ctx.Bits.Cmd);
                AmbaImgPrint(PRINT_FLAG_DBG, "chain id", (UINT32) CmdMsg.Ctx.StillIso.ChainId);
                AmbaImgPrint(PRINT_FLAG_DBG, "is hiso", (UINT32) CmdMsg.Ctx.StillIso.IsHiso);
                AmbaImgPrint(PRINT_FLAG_DBG, "is piv", (UINT32) CmdMsg.Ctx.StillIso.IsPiv);
                AmbaImgPrint64Ex(PRINT_FLAG_DBG, "cfa", AuxData.Ctx.StillIso.pCfa, 16U);
                {
                    UINT32 ChainId;
                    UINT32 IsHiso;
                    UINT32 IsPiv;

                    AMBA_IMG_MEM_INFO_s MemInfo;

                    ChainId = CmdMsg.Ctx.StillIso.ChainId;
                    IsHiso = CmdMsg.Ctx.StillIso.IsHiso;
                    IsPiv = CmdMsg.Ctx.StillIso.IsPiv;

                    /* cfa get */
                    MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) AuxData.Ctx.StillIso.pCfa;

                    /* still ae dsp dgc */
                    FuncRetCode = AmbaImgMainAe_SnapDspDgc(VinId, ChainId, MemInfo.Ctx.pCfa, IsPiv, IsHiso);
                    if (FuncRetCode != OK_UL) {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: snap dsp dgc");
                    }
                    /* still awb dsp wgc */
                    FuncRetCode = AmbaImgMainAwb_SnapDspWgc(VinId, ChainId, MemInfo.Ctx.pCfa, IsPiv, IsHiso);
                    if (FuncRetCode != OK_UL) {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: snap dsp wgc");
                    }
                    /* still adj dsp filter */
                    FuncRetCode = AmbaImgMainAdj_SnapDspFilter(VinId, ChainId, MemInfo.Ctx.pCfa, IsPiv, IsHiso);
                    if (FuncRetCode != OK_UL) {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: snap dsp iso");
                    }
                }
                break;
            case (UINT8) CAPTURE_CMD_EXIF:
                /* exif */
                AmbaImgPrint2(PRINT_FLAG_DBG, "capture-cmd-exif", VinId, (UINT32) CmdMsg.Ctx.Bits.Cmd);
                AmbaImgPrint(PRINT_FLAG_DBG, "chain id", (UINT32) CmdMsg.Ctx.StillExif.ChainId);
                AmbaImgPrint64Ex(PRINT_FLAG_DBG, "exif", AuxData.Ctx.StillExif.Var, 16U);
                {
                    AMBA_IMG_MEM_INFO_s MemInfo;

                    UINT32 ChainId = (UINT32) CmdMsg.Ctx.StillExif.ChainId;

                    MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) AuxData.Ctx.StillExif.Var;

                    /* sanp exif get */
                    FuncRetCode = AmbaImgMainAe_SnapExifGet(VinId, ChainId, MemInfo.Ctx.pPvoid);
                    if (FuncRetCode != OK_UL) {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: snap exif");
                    }
                }
                break;
#ifdef CONFIG_BUILD_IMGFRW_RAW_PIV
            case (UINT8) CAPTURE_CMD_PIV:
                /* video (r2y) piv start */
                AmbaImgPrint2(PRINT_FLAG_DBG, "capture-cmd-piv", VinId, (UINT32) CmdMsg.Ctx.Bits.Cmd);
                AmbaImgPrint(PRINT_FLAG_DBG, "chain id", (UINT32) CmdMsg.Ctx.StillPiv.ChainId);
                AmbaImgPrintEx(PRINT_FLAG_DBG, "raw cap seq", CmdMsg.Ctx.StillPiv.RawCapSeq, 16U);
                AmbaImgPrint64Ex(PRINT_FLAG_DBG, "is hiso", AuxData.Ctx.StillPiv.pIsHiso, 16U);
                {
                    UINT32 ChainId;
                    UINT32 RawCapSeq;
                    UINT32 *pIsHiso;

                    AMBA_IMG_MEM_INFO_s MemInfo;

                    ChainId = CmdMsg.Ctx.StillPiv.ChainId;
                    RawCapSeq = CmdMsg.Ctx.StillPiv.RawCapSeq;

                    /* hiso get */
                    MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) AuxData.Ctx.StillPiv.pIsHiso;
                    pIsHiso = MemInfo.Ctx.pUint32;

                    FuncRetCode = AmbaImgMainAe_Piv(VinId, ChainId, RawCapSeq, pIsHiso);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                }
                break;
#endif
            default:
                /* */
                break;
        }
    }
}

/**
 *  Amba image main dispatch
 *  @param[in] VinId vin id
 *  @param[in] Param command and message
 *  @note this function is intended for internal use only
 */
static void AmbaImgMain_Dispatch(UINT32 VinId, UINT64 Param)
{
    if (VinId < AMBA_IMG_NUM_VIN_CHANNEL) {
        /* image framework */
        AmbaImgMain_ImgFrwDispatch(VinId, Param);
        /* algorithm */
        AmbaImgMain_AlgoDispatch(VinId, Param);
        /* task */
        AmbaImgMain_TaskDispatch(VinId, Param);
    }

    /* avm */
    AmbaImgMainAvm_Dispatch(VinId, Param);
    /* fov adj */
    AmbaImgMainFov_Dispatch(VinId, Param);
    /* eis */
    AmbaImgMainEis_Dispatch(VinId, Param);

    /* snap cb */
    AmbaImgMain_SnapDispatch(VinId, Param);
#ifdef CONFIG_BUILD_IMGFRW_EFOV
    /* efov txrx */
    AmbaImgMainEFov_Dispatch(VinId, Param);
#endif
}

/**
 *  Amba image main dispatch ex
 *  @param[in] VinId vin id
 *  @param[in] Param command and message
 *  @param[in] Param2 aux data
 *  @note this function is intended for internal use only
 */
static void AmbaImgMain_DispatchEx(UINT32 VinId, UINT64 Param, UINT64 Param2)
{
    if (VinId < AMBA_IMG_NUM_VIN_CHANNEL) {
        /* image framework ex */
        AmbaImgMain_ImgFrwDispatchEx(VinId, Param, Param2);
        /* algorithm ex */
        AmbaImgMain_AlgoDispatchEx(VinId, Param, Param2);
    }

    /* avm */
    AmbaImgMainAvm_DispatchEx(VinId, Param, Param2);
#ifdef CONFIG_BUILD_IMGFRW_EIS_WARP_AC
    /* eis */
    AmbaImgMainEis_DispatchEx(VinId, Param, Param2);
#endif
    /* snap cb ex */
    AmbaImgMain_SnapDispatchEx(VinId, Param, Param2);
#ifdef CONFIG_BUILD_IMGFRW_EFOV
    /* efov txrx */
    AmbaImgMainEFov_DispatchEx(VinId, Param, Param2);
#endif
}

/**
 *  Amba image main system init
 */
void AmbaImgMain_SysInit(void)
{
    UINT32 FuncRetCode;

    AmbaImgPrint_Enable(AMBA_IMG_PRINT_ENABLE_FLAG);
    AmbaImgPrintStr(PRINT_FLAG_MSG, "image main sys init...");

    /* image channel user init */
    FuncRetCode = AmbaImgMain_ChanMemInit();
    if (FuncRetCode != OK_UL) {
        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image main mem init");
    }

    /* image event init */
    FuncRetCode = AmbaImgEvent_Init();
    if (FuncRetCode != OK_UL) {
        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image event init");
    }

    /* image message init */
    FuncRetCode = AmbaImgMessage_Init();
    if (FuncRetCode != OK_UL) {
        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image message init");
    }

    /* image system init */
    FuncRetCode = AmbaImgSystem_Init();
    if (FuncRetCode != OK_UL) {
        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image system init");
    }

    /* image system init */
    FuncRetCode = AmbaImgStatistics_Init();
    if (FuncRetCode != OK_UL) {
        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image statistics init");
    }

    /* image sensor drv init */
    FuncRetCode = AmbaImgSensorDrv_Init();
    if (FuncRetCode != OK_UL) {
        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image sensor drv init");
    }
    /* image sensor hal init */
    FuncRetCode = AmbaImgSensorHAL_Init();
    if (FuncRetCode != OK_UL) {
        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image sensor hal init");
    }
    /* image sensor sync init */
    FuncRetCode = AmbaImgSensorSync_Init();
    if (FuncRetCode != OK_UL) {
        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image sensor sync init");
    }

    /* image main statistics init */
    FuncRetCode = AmbaImgMainStatistics_Init();
    if (FuncRetCode != OK_UL) {
        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image main statistics init");
    }

    /* image main sync init */
    FuncRetCode = AmbaImgMainSync_Init();
    if (FuncRetCode != OK_UL) {
        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image main sync init");
    }

    /* image main ae init */
    FuncRetCode = AmbaImgMainAe_Init();
    if (FuncRetCode != OK_UL) {
        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image main ae init");
    }

    /* image main awb init */
    FuncRetCode = AmbaImgMainAwb_Init();
    if (FuncRetCode != OK_UL) {
        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image main awb init");
    }

    /* image main adj init */
    FuncRetCode = AmbaImgMainAdj_Init();
    if (FuncRetCode != OK_UL) {
        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image main adj init");
    }

    /* image main af init */
    FuncRetCode = AmbaImgMainAf_Init();
    if (FuncRetCode != OK_UL) {
        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image main af init");
    }

    /* image main avm init */
    FuncRetCode = AmbaImgMainAvm_Init();
    if (FuncRetCode != OK_UL) {
        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image main avm init");
    }

    /* image main fov init */
    FuncRetCode = AmbaImgMainFov_Init();
    if (FuncRetCode != OK_UL) {
        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image main fov init");
    }

    /* image main eis init */
    FuncRetCode = AmbaImgMainEis_Init();
    if (FuncRetCode != OK_UL) {
        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image main eis init");
    }
#ifdef CONFIG_BUILD_IMGFRW_EFOV
    /* image main efov init */
    FuncRetCode = AmbaImgMainEFov_Init();
    if (FuncRetCode != OK_UL) {
        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image main efov init");
    }
#endif
    /* image framework command attach */
    {
        FuncRetCode = AmbaImgFrwCmd_Attach();
        if (FuncRetCode != OK_UL) {
            AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image framework command attach");
        }
    }
    {
        FuncRetCode = AmbaImgFrwCmdApp_Attach();
        if (FuncRetCode != OK_UL) {
            AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image framework command app attach");
        }
    }
    {
        FuncRetCode = AmbaImgFrwCmdFlow_Attach();
        if (FuncRetCode != OK_UL) {
            AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image framework command flow attach");
        }
    }
}
#ifdef AMBA_IMG_MAIN_API_COMPATIBLE
/**
 *  Image main system init (compatible, will be removed)
 */
void ImageMain_SysInit(void)
{
    AmbaImgMain_SysInit();
}
#endif
/**
 *  Amba image main memory size query
 *  @return memory size
 */
UINT32 AmbaImgMain_MemSizeQuery(void)
{
    UINT32 MemSize;
    MemSize = AmbaImgMainAe_MemSizeQuery();
    return MemSize;
}
#ifdef AMBA_IMG_MAIN_API_COMPATIBLE
/**
 *  Image main memory size query (compatible, will be removed)
 *  @return memory size
 */
UINT32 ImageMain_MemSizeQuery(void)
{
    return AmbaImgMain_MemSizeQuery();
}
#endif
/**
 *  Amba image main memory init
 *  @param[in] pMem pointer to the memory address
 *  @param[in] MemSizeInByte memory size
 */
void AmbaImgMain_MemInit(void *pMem, UINT32 MemSizeInByte)
{
    UINT32 FuncRetCode;

    FuncRetCode = AmbaImgMainAe_MemInit(pMem, MemSizeInByte);
    if (FuncRetCode != OK_UL) {
        /* */
    }
}
#ifdef AMBA_IMG_MAIN_API_COMPATIBLE
/**
 *  Image main memory init (compatible, will be removed)
 *  @param[in] pMem pointer to the memory address
 *  @param[in] MemSzeInByte memory size
 */
void ImageMain_MemInit(void *pMem, UINT32 MemSizeInByte)
{
    AmbaImgMain_MemInit(pMem, MemSizeInByte);
}
#endif
/**
 *  Amba image main iq memory size query
 *  @param[in] MaxAlgoNum max algorithm channel number
 *  @return memory size
 */
UINT32 AmbaImgMain_IqMemSizeQuery(UINT32 MaxAlgoNum)
{
    UINT32 FuncRetCode;
    UINT32 IqMemSize = 0U;
    UINT32 IqMemSizeAlign = 0U;

    /* img iq memory size query */
    FuncRetCode = Amba_IQParam_QueryIqBufSize(&IqMemSize);
    if (FuncRetCode != 0U) {
        AmbaImgPrint2(PRINT_FLAG_ERR, "error: IQ memory size query", MaxAlgoNum, IqMemSize);
        AmbaImgPrint_Flush();
    } else {
        /* alignment */
        IqMemSizeAlign = IqMemSize + ((((UINT32) AMBA_CACHE_LINE_SIZE) - (IqMemSize % ((UINT32) AMBA_CACHE_LINE_SIZE))) % ((UINT32) AMBA_CACHE_LINE_SIZE));
        //AmbaImgPrint2(PRINT_FLAG_MSG, "iq mem size query", IqMemSize, IqMemSizeAlign);
    }
    return (UINT32) (IqMemSizeAlign*MaxAlgoNum);
}
#ifdef AMBA_IMG_MAIN_API_COMPATIBLE
/**
 *  Image main iq memory size query (compatible, will be removed)
 *  @param[in] MaxAlgoNum max algorithm channel number
 *  @return memory size
 */
UINT32 ImageMain_IqMemSizeQuery(UINT32 MaxAlgoNum)
{
    return AmbaImgMain_IqMemSizeQuery(MaxAlgoNum);
}
#endif
/**
 *  Amba image main iq memory init
 *  @param[in] MaxAlgoNum max algorithm channel number
 *  @param[in] pMem pointer to the memory address
 *  @param[in] MemSizeInByte memory size
 */
void AmbaImgMain_IqMemInit(UINT32 MaxAlgoNum, void *pMem, UINT32 MemSizeInByte)
{
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 IqMemSize;

    AMBA_IMG_MEM_INFO_s MemInfo;
    AMBA_IMG_MEM_INFO_s MemInfo2;

    UINT32 Index;

    /* mem get */
    MemInfo.Ctx.pVoid = pMem;

    /* iq size get */
    FuncRetCode = Amba_IQParam_QueryIqBufSize(&IqMemSize);
    if (FuncRetCode == OK_UL) {
        /* alignment */
        IqMemSize = IqMemSize + ((((UINT32) AMBA_CACHE_LINE_SIZE) - (IqMemSize % ((UINT32) AMBA_CACHE_LINE_SIZE))) % ((UINT32) AMBA_CACHE_LINE_SIZE));
        /* iq mem init */
        for (i = 0U; i < MaxAlgoNum; i++) {
            /* index get */
            Index = IqMemSize*i;
            if (Index < MemSizeInByte) {
                /* base get */
                MemInfo2.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[Index]);
                /* mem settled */
                FuncRetCode = Amba_IQParam_SetIqBuf(i, MemInfo2.Ctx.pVoid);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrint64Ex2(PRINT_FLAG_ERR, "error: iq memory setup", (UINT64) MemInfo2.Ctx.Data, 16U, (UINT64) IqMemSize, 16U);
                    AmbaImgPrint_Flush();
                }
                //AmbaImgPrintEx(PRINT_FLAG_MSG, "iq mem", (UINT32) MemInfo2.Ctx.Data, 16U);
            } else {
                /* buffer exceeded */
                AmbaImgPrintEx2(PRINT_FLAG_ERR, "error: iq memory is not enough", i, 10U, Index, 16U);
            }
        }
    }
}
#ifdef AMBA_IMG_MAIN_API_COMPATIBLE
/**
 *  Image main iq memory init (compatible, will be removed)
 *  @param[in] MaxAlgoNum max algorithm channel number
 *  @param[in] pMem pointer to the memory address
 *  @param[in] MemSzeInByte memory size
 */
void ImageMain_IqMemInit(UINT32 MaxAlgoNum, void *pMem, UINT32 MemSizeInByte)
{
    AmbaImgMain_IqMemInit(MaxAlgoNum, pMem, MemSizeInByte);
}
#endif
/**
 *  Amba image main algo memory size query
 *  @param[in] MaxAlgoNum max algorithm channel number
 *  @param[in] MaxExposureNum max exposure number of algorithm channel
 *  @return memory size
 */
UINT32 AmbaImgMain_AlgoMemSizeQuery(UINT32 MaxAlgoNum, UINT32 MaxExposureNum)
{
    UINT32 FuncRetCode;
    SIZE_t AAAMemSize = 0U;

    /* img proc exp grp set */
    FuncRetCode = AmbaImgProc_SetMaxExpGroupNo(MaxExposureNum);
    if (FuncRetCode != OK_UL) {
        /* */
    }
    /* img proc memory size query */
    FuncRetCode = AmbaImgProc_QueryMemorySize(MaxAlgoNum, &AAAMemSize);
    if (FuncRetCode != 0U) {
        AmbaImgPrint2(PRINT_FLAG_ERR, "error: AAA memory size query", MaxAlgoNum, (UINT32) AAAMemSize);
        AmbaImgPrint_Flush();
    }

    return (UINT32) AAAMemSize;
}
#ifdef AMBA_IMG_MAIN_API_COMPATIBLE
/**
 *  Image main algorithm memory size query (compatible, will be removed)
 *  @param[in] MaxAlgoNum max algorithm channel number
 *  @param[in] MaxExposureNum max exposure number of algorithm channel
 *  @return memory size
 */
UINT32 ImageMain_AlgoMemSizeQuery(UINT32 MaxAlgoNum, UINT32 MaxExposureNum)
{
    return AmbaImgMain_AlgoMemSizeQuery(MaxAlgoNum, MaxExposureNum);
}
#endif
/**
 *  Amba image main algorithm memory init
 *  @param[in] pMem pointer to the memory address
 *  @param[in] MemSizeInByte memory size
 */
void AmbaImgMain_AlgoMemInit(void *pMem, UINT32 MemSizeInByte)
{
    UINT32 FuncRetCode;

    FuncRetCode = AmbaImgProc_SetMemInfo(pMem, MemSizeInByte);
    if (FuncRetCode != 0U) {
        AMBA_IMG_MEM_INFO_s MemInfo;
        MemInfo.Ctx.pVoid = pMem;
        AmbaImgPrint64Ex2(PRINT_FLAG_ERR, "error: AAA memory setup", (UINT64) MemInfo.Ctx.Data, 16U, (UINT64) MemSizeInByte, 10U);
        AmbaImgPrint_Flush();
    }
}
#ifdef AMBA_IMG_MAIN_API_COMPATIBLE
/**
 *  Image main algorithm memory init (compatible, will be removed)
 *  @param[in] pMem pointer to the memory address
 *  @param[in] MemSzeInByte memory size
 */
void ImageMain_AlgoMemInit(void *pMem, UINT32 MemSizeInByte)
{
    AmbaImgMain_AlgoMemInit(pMem, MemSizeInByte);
}
#endif
/**
 *  Amba image main algorithm init
 *  @param[in] MaxAlgoNum max algorithm channel number
 *  @param[in] MaxExposureNum max exposure number of algorithm channel
 */
void AmbaImgMain_AlgoInit(UINT32 MaxAlgoNum, UINT32 MaxExposureNum)
{
    UINT32 FuncRetCode;
    UINT32 i;

    AmbaImgPrint2(PRINT_FLAG_MSG, "algo init...", MaxAlgoNum, MaxExposureNum);
    /* img proc exp grp set */
    FuncRetCode = AmbaImgProc_SetMaxExpGroupNo(MaxExposureNum);
    if (FuncRetCode != OK_UL) {
        /* */
    }
    /* img proc init */
    FuncRetCode = App_Image_Init(MaxAlgoNum);
    if (FuncRetCode == OK) {
        /* func hook */
        IMG_PROC_FUNC_s IpFunc = {NULL, NULL, NULL, NULL, NULL, NULL};
        APP_INFO_FUNC_s InfoFunc = {
            AmbaImgMain_AlgoImgMode,
            AmbaImgMain_AlgoLiveviewInfo
        };

        for(i = 0U; i < MaxAlgoNum; i++) {
            /* query */
            FuncRetCode = AmbaImgProc_GetRegFunc(i, &IpFunc);
            if (FuncRetCode == OK_UL) {
                IpFunc.QueryActualShutterTime = AmbaImgMain_QueryActuralShutter;
                IpFunc.QueryActualGainFactor = AmbaImgMain_QueryActuralGain;
                IpFunc.QueryActualStrobeWidth = AmbaImgMain_QueryActuralStrobe;
                FuncRetCode = AmbaImgProc_SetRegFunc(i, &IpFunc);
                if (FuncRetCode != OK) {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image main aaa query func register");
                }
            } else {
                AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image main aaa query func get");
            }
            /* mode cfg & liv info */
            FuncRetCode = AmbaImgProc_SetAppInfoFunc(i, &InfoFunc);
            if (FuncRetCode != OK_UL) {
                /* */
            }
        }
    }
}
#ifdef AMBA_IMG_MAIN_API_COMPATIBLE
/**
 *  Amba image main algorithm init (compatible, will be removed)
 *  @param[in] MaxAlgoNum max algorithm channel number
 *  @param[in] MaxExposureNum max exposure number of algorithm channel
 */
void ImageMain_AlgoInit(UINT32 MaxAlgoNum, UINT32 MaxExposureNum)
{
    AmbaImgMain_AlgoInit(MaxAlgoNum, MaxExposureNum);
}
#endif
/**
 *  Amba image main config
 *  @param[in] VinId vin id
 *  @param[in] Param config command and message
 */
void AmbaImgMain_Config(UINT32 VinId, UINT64 Param)
{
    AmbaImgMain_Dispatch(VinId, Param);
}

/**
 *  Amba image main config ex
 *  @param[in] VinId vin id
 *  @param[in] Param config command and message
 *  @param[in] Param2 config aux data
 */
void AmbaImgMain_ConfigEx(UINT32 VinId, UINT64 Param, UINT64 Param2)
{
    AmbaImgMain_DispatchEx(VinId, Param, Param2);
}
#ifdef AMBA_IMG_MAIN_API_COMPATIBLE
/**
 *  Image main config (compatible, will be removed)
 *  @param[in] VinId vin id
 *  @param[in] Param config command and message
 */
void ImageMain_Config(UINT32 VinId, UINT64 Param)
{
    AmbaImgMain_Config(VinId, Param);
}

/**
 *  Image main config ex (compatible, will be removed)
 *  @param[in] VinId vin id
 *  @param[in] Param config command and message
 *  @param[in] Param2 config aux data
 */
void ImageMain_ConfigEx(UINT32 VinId, UINT64 Param, UINT64 Param2)
{
    AmbaImgMain_ConfigEx(VinId, Param, Param2);
}

/**
 *  Image print enable (compatible, will be removed)
 *  @param[in] Flag print flag
 */
void ImagePrint_Enable(UINT32 Flag)
{
    AmbaImgPrint_Enable(Flag);
}
#endif

/**
 *  Amba shell image framework command entry
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 */
void AmbaImgMain_ImgfrwCommand(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    AmbaShell_CommandAmbaImgFrw(Argc, Argv, PrintFunc);
}

/**
 *  Amba image main query actual shutter
 *  @param[in] MainViewID algorithm view channel id
 *  @param[in] ExposureFrame exposure number
 *  @param[in] DesiredShutter pointer to the desired shutter data
 *  @param[out] pActualShutter pointer to the actual shutter information
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMain_QueryActuralShutter(UINT32 MainViewID, UINT32 ExposureFrame, const FLOAT *DesiredShutter, AMBA_IMG_SENSOR_SHUTTER_INFO_s *pActualShutter)
{
    UINT32 RetCode = OK;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 SensorId;

    AMBA_SENSOR_CHANNEL_s SensorChanId;

    UINT32 DesiredShutterU;
    FLOAT DesiredShutterF;
    FLOAT DesiredShutterTime[4] = {0.0f};

    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    ImageChanId.Ctx.Data = AmbaImgMain_Algo2ChanTable[MainViewID].Ctx.Data;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    SensorId = ImageChanId.Ctx.Bits.SensorId;

    SensorChanId.VinID = VinId;
    for (i = 0U; (i < AMBA_IMG_NUM_VIN_ALGO) && ((SensorId >> i) > 0U); i++) {
        if ((SensorId & (((UINT32) 1U) << i)) > 0U) {
            SensorChanId.SensorID = AmbaImgMain_SensorIdTable[VinId][i];
            break;
        }
    }
#ifdef CONFIG_BUILD_IMGFRW_TD_SM_BCNT
    {
        if ((pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU) &&
            (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL)) {
            const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;
            /* ctx get */
            pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;
            /* sub sensor mode */
            SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
            SensorChanId.Reserved[1] = 0U;
#if 1
            /* debug message */
            if (AmbaImgSensorHAL_Vin[VinId].Timing.Op.Bits.Enable > 0U) {
                char str[11];
                str[0] = 's';
                var_utoa(SensorChanId.Reserved[0], &(str[1]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                str[5] = ' ';
                var_utoa(SensorChanId.Reserved[1] & 0x7FFFFFFFU, &(str[6]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                AmbaImgSensorHAL_TimingMarkPut(VinId, str);
            }
#endif
        }
    }
#endif
    for (i = 0U; i < ExposureFrame; i++) {
        DesiredShutterF = DesiredShutter[i]*1024.0f*1024.0f;
        DesiredShutterU = (DesiredShutterF > 0.0f) ? (UINT32) DesiredShutterF : 0U;
        DesiredShutterTime[i] = ((FLOAT) DesiredShutterU)/(1024.0f*1024.0f);
    }

    FuncRetCode = AmbaSensor_ConvertShutterSpeed(&SensorChanId, DesiredShutterTime, pActualShutter->ShutterTime, pActualShutter->ShutterLine);
    if (FuncRetCode != SENSOR_ERR_NONE) {
        /* */
    }
    /* debug only */
    if (AmbaImgMainAe_QueryCount[VinId][ChainId] < AmbaImgMain_DebugCount) {
        FLOAT VarF;
        AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
        AmbaImgPrint2(PRINT_FLAG_DBG, "query vin", MainViewID, SensorChanId.VinID);
        VarF = DesiredShutter[0]*1000000.0f;
        AmbaImgPrint2(PRINT_FLAG_DBG, "query d shutter", MainViewID, (VarF > 0.0f) ? (UINT32) VarF : 0U);
        VarF = DesiredShutterTime[0]*1000000.0f;
        AmbaImgPrint2(PRINT_FLAG_DBG, "query q shutter", MainViewID, (VarF > 0.0f) ? (UINT32) VarF : 0U);
        VarF = pActualShutter->ShutterTime[0]*1000000.0f;
        AmbaImgPrint2(PRINT_FLAG_DBG, "query a shutter", MainViewID,  (VarF > 0.0f) ? (UINT32) VarF : 0U);
        AmbaImgPrint2(PRINT_FLAG_DBG, "query shr", MainViewID, pActualShutter->ShutterLine[0]);
    }

    return RetCode;
}

/**
 *  Amba image main query actual gain
 *  @param[in] MainViewID algorithm view channel id
 *  @param[in] ExposureFrame exposure number
 *  @param[in] pDesiredGain pointer to the desired gain data
 *  @param[out] pActualGain pointer to the actual gain information
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMain_QueryActuralGain(UINT32 MainViewID, UINT32 ExposureFrame, const AMBA_IMG_SENSOR_GAIN_INFO_s *pDesiredGain, AMBA_IMG_SENSOR_GAIN_INFO_s *pActualGain)
{
    UINT32 RetCode = OK;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 SensorId;

    AMBA_SENSOR_CHANNEL_s SensorChanId;

    UINT32 DesiredAgcU;
    FLOAT DesiredAgcF;
    AMBA_SENSOR_GAIN_FACTOR_s DesiredFactor;
    AMBA_SENSOR_GAIN_FACTOR_s ActualFactor;
    AMBA_SENSOR_GAIN_CTRL_s GainCtrl;

    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    ImageChanId.Ctx.Data = AmbaImgMain_Algo2ChanTable[MainViewID].Ctx.Data;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    SensorId = ImageChanId.Ctx.Bits.SensorId;

    SensorChanId.VinID = VinId;
    for (i = 0U; (i < AMBA_IMG_NUM_VIN_ALGO) && ((SensorId >> i) > 0U); i++) {
        if ((SensorId & (((UINT32) 1U) << i)) > 0U) {
            SensorChanId.SensorID = AmbaImgMain_SensorIdTable[VinId][i];
            break;
        }
    }
#ifdef CONFIG_BUILD_IMGFRW_TD_SM_BCNT
    {
        if ((pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU) &&
            (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL)) {
            const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;
            /* ctx get */
            pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;
            /* sub sensor mode */
            SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
            SensorChanId.Reserved[1] = 0U;
#if 1
            /* debug message */
            if (AmbaImgSensorHAL_Vin[VinId].Timing.Op.Bits.Enable > 0U) {
                char str[11];
                str[0] = 'g';
                var_utoa(SensorChanId.Reserved[0], &(str[1]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                str[5] = ' ';
                var_utoa(SensorChanId.Reserved[1] & 0x7FFFFFFFU, &(str[6]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                AmbaImgSensorHAL_TimingMarkPut(VinId, str);
            }
#endif
        }
    }
#endif
    DesiredAgcF = pDesiredGain->AGC[0]*4096.0f;
    DesiredAgcU = (DesiredAgcF > 0.0f) ? (UINT32) DesiredAgcF : 0U;
    DesiredFactor.Gain[0] = ((FLOAT) DesiredAgcU)/4096.0f;
    DesiredFactor.WbGain[0].R = pDesiredGain->GainR[0];
    DesiredFactor.WbGain[0].Gr = pDesiredGain->GainGr[0];
    DesiredFactor.WbGain[0].Gb = pDesiredGain->GainGb[0];
    DesiredFactor.WbGain[0].B = pDesiredGain->GainB[0];

    for (i = 1U; i < ExposureFrame; i++) {
        DesiredAgcF = pDesiredGain->AGC[i]*4096.0f;
        DesiredAgcU = (DesiredAgcF > 0.0f) ? (UINT32) DesiredAgcF : 0U;
        DesiredFactor.Gain[i] = ((FLOAT) DesiredAgcU)/4096.0f;
        DesiredFactor.WbGain[i].R = pDesiredGain->GainR[i];
        DesiredFactor.WbGain[i].Gr = pDesiredGain->GainGr[i];
        DesiredFactor.WbGain[i].Gb = pDesiredGain->GainGb[i];
        DesiredFactor.WbGain[i].B = pDesiredGain->GainB[i];
    }

    FuncRetCode = AmbaSensor_ConvertGainFactor(&SensorChanId, &DesiredFactor, &ActualFactor, &GainCtrl);

    if (FuncRetCode == SENSOR_ERR_NONE) {
        for (i = 0U; i < ExposureFrame; i++) {
            pActualGain->AGC[i] = ActualFactor.Gain[i];
            pActualGain->GainR[i] = ActualFactor.WbGain[i].R;
            pActualGain->GainGr[i] = ActualFactor.WbGain[i].Gr;
            pActualGain->GainGb[i] = ActualFactor.WbGain[i].Gb;
            pActualGain->GainB[i] = ActualFactor.WbGain[i].B;
        }
    } else {
        /* */
    }

    /* debug only */
    if (AmbaImgMainAe_QueryCount[VinId][ChainId] < AmbaImgMain_DebugCount) {
        FLOAT VarF;
        AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
        AmbaImgPrint2(PRINT_FLAG_DBG, "query vin", MainViewID, SensorChanId.VinID);
        VarF = pDesiredGain->AGC[0]*1000.0f;
        AmbaImgPrint2(PRINT_FLAG_DBG, "query d gain", MainViewID, (VarF > 0.0f) ? (UINT32) VarF : 0U);
        VarF = DesiredFactor.Gain[0]*1000.0f;
        AmbaImgPrint2(PRINT_FLAG_DBG, "query q gain", MainViewID, (VarF > 0.0f) ? (UINT32) VarF : 0U);
        VarF = ActualFactor.Gain[0]*1000.0f;
        AmbaImgPrint2(PRINT_FLAG_DBG, "query a gain", MainViewID, (VarF > 0.0f) ? (UINT32) VarF : 0U);
        AmbaImgPrintEx2(PRINT_FLAG_DBG, "query agc", MainViewID, 10U, GainCtrl.AnalogGain[0], 16U);
        AmbaImgPrintEx2(PRINT_FLAG_DBG, "query dgc", MainViewID, 10U, GainCtrl.DigitalGain[0], 16U);
        /* count accumulate */
        AmbaImgMainAe_QueryCount[VinId][ChainId]++;
        if (AmbaImgMainAe_QueryCount[VinId][ChainId] == AmbaImgMain_DebugCount) {
            /* count reset */
            AmbaImgMainAe_QueryCount[VinId][ChainId] = 0xFFFFFFFFU;
        }
    }

    return RetCode;
}

/**
 *  Amba image main query actual strobe
 *  @param[in] MainViewID algorithm view channel id
 *  @param[in] pDesiredWidth pointer to the desired strobe data
 *  @param[out] pActualWidth pointer to the actual strobe information
 *  @param[out] pStrobeCtrl pointer to the strobe control data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMain_QueryActuralStrobe(UINT32 MainViewID, const UINT32 *pDesiredWidth, UINT32 *pActualWidth, UINT32 *pStrobeCtrl)
{
    UINT32 RetCode = OK;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 SensorId;

    AMBA_SENSOR_CHANNEL_s SensorChanId;

    FLOAT DesiredFactorF;
    FLOAT ActualFactorF;

    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    ImageChanId.Ctx.Data = AmbaImgMain_Algo2ChanTable[MainViewID].Ctx.Data;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    SensorId = ImageChanId.Ctx.Bits.SensorId;

    SensorChanId.VinID = VinId;
    for (i = 0U; (i < AMBA_IMG_NUM_VIN_ALGO) && ((SensorId >> i) > 0U); i++) {
        if ((SensorId & (((UINT32) 1U) << i)) > 0U) {
            SensorChanId.SensorID = AmbaImgMain_SensorIdTable[VinId][i];
            break;
        }
    }

    DesiredFactorF = ((FLOAT) *pDesiredWidth) / 1000000.0f;

    FuncRetCode = AmbaSensor_ConvertStrobeWidth(&SensorChanId, &DesiredFactorF, &ActualFactorF, pStrobeCtrl);
    if (FuncRetCode != SENSOR_ERR_NONE) {
        /* */
    }

    ActualFactorF = ActualFactorF * 1000000.0f;
    *pActualWidth = (ActualFactorF > 0.0f) ? (UINT32) ActualFactorF : 0U;

    /* debug only */
    if (AmbaImgMainAe_QueryCount[VinId][ChainId] < AmbaImgMain_DebugCount) {
        AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
        AmbaImgPrint2(PRINT_FLAG_DBG, "query vin", MainViewID, SensorChanId.VinID);
        AmbaImgPrint2(PRINT_FLAG_DBG, "query d strobe", MainViewID, *pDesiredWidth);
        AmbaImgPrint2(PRINT_FLAG_DBG, "query a strobe", MainViewID, *pActualWidth);
        AmbaImgPrintEx2(PRINT_FLAG_DBG, "query stb", MainViewID, 10U, *pStrobeCtrl, 16U);
    }

    return RetCode;
}

/**
 *  Amba image main algorithm image mode
 *  @param[in] ViewId view channel id
 *  @param[in] Mode pipe mode
 *  @param[out] pMode pointer to the image mode configuration data
 *  @note this function is intended for internal use only
 */
static void AmbaImgMain_AlgoImgMode(UINT16 ViewId, UINT8 Mode, AMBA_IK_MODE_CFG_s* pMode)
{
    UINT32 i;

    UINT32 ZoneId;
    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    if (Mode == IP_MODE_VIDEO) {
        ImageChanId.Ctx.Data = AmbaImgMain_Algo2ChanTable[ViewId].Ctx.Data;

        ZoneId = ImageChanId.Ctx.Bits.ZoneId;
        for(i = 0U; (i < AMBA_IMG_NUM_FOV_CHANNEL) && ((ZoneId >> i) > 0U); i++) {
           if ((ZoneId & (((UINT32) 1U) << i)) > 0U) {
               pMode->ContextId = AmbaImgSystem_ContextIdGet(i);
               break;
           }
        }
    } else {
        pMode->ContextId = 0U;
    }
}

/**
 *  Amba image main algorithm live view inforamtion
 *  @param[in] ViewId algorithm view channel id
 *  @param[out] pLiveViewInfo pointer to the live view information
 *  @note this function is intended for internal use only
 */
static void AmbaImgMain_AlgoLiveviewInfo(UINT16 ViewId, LIVEVIEW_INFO_s *pLiveViewInfo)
{
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 ZoneId;
#ifdef CONFIG_BUILD_IMGFRW_AAA_SHR_BDY
    UINT32 ChannelNum;

    UINT32 TotalShutterLine;
    UINT32 TotalNsInShutterLine;

    UINT32 MaxExposureLine;
    UINT32 MinExposureLine;

    UINT32 MaxExposureTimeInNs[AMBA_IMG_SENSOR_HAL_HDR_SIZE] = {0};
    UINT32 MinExposureTimeInNs[AMBA_IMG_SENSOR_HAL_HDR_SIZE] = {0};
#endif
    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    UINT32 Width = 1920U;
    UINT32 Height = 1080U;

    ImageChanId.Ctx.Data = AmbaImgMain_Algo2ChanTable[ViewId].Ctx.Data;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    ZoneId = ImageChanId.Ctx.Bits.ZoneId;

    for (i = 0U; (i < AMBA_IMG_NUM_FOV_CHANNEL) && ((ZoneId >> i) > 0U); i++) {
        if ((ZoneId & (((UINT32) 1U) << i)) > 0U) {
            /* main size get */
            FuncRetCode = AmbaImgChannel_PipeOutGet(VinId, ChainId, i, &Width, &Height);
            if (FuncRetCode != OK_UL) {
                AmbaImgPrintStr(PRINT_FLAG_ERR, "error: algo pipe out size");
                AmbaImgPrint_Flush();
            }
            break;
        }
    }

    pLiveViewInfo->MainW = (UINT16) (Width & 0xFFFFU);
    pLiveViewInfo->MainH = (UINT16) (Height & 0xFFFFU);
    pLiveViewInfo->FrameRateInt = (AmbaImgSensorHAL_Info[VinId][ChainId].Frame.Rate + 50U)/100U;
    pLiveViewInfo->OverSamplingEnable = 0U;
    pLiveViewInfo->BinningHNum = (UINT8) (AmbaImgSensorHAL_Info[VinId][ChainId].Image.HFactorNum & 0xFFU);
    pLiveViewInfo->BinningHDen = (UINT8) (AmbaImgSensorHAL_Info[VinId][ChainId].Image.HFactorDen & 0xFFU);
    pLiveViewInfo->BinningVNum = (UINT8) (AmbaImgSensorHAL_Info[VinId][ChainId].Image.VFactorNum & 0xFFU);
    pLiveViewInfo->BinningVDen = (UINT8) (AmbaImgSensorHAL_Info[VinId][ChainId].Image.VFactorDen & 0xFFU);
#ifdef CONFIG_BUILD_IMGFRW_AAA_SHR_BDY
    /* exposure info (frame) (TBD) */
    ChannelNum = AmbaImgSensorHAL_HdrInfo[VinId][ChainId].Frame.ChannelNum;
    if (ChannelNum == 0U) {
        /* non-hdr */
        TotalShutterLine = AmbaImgSensorHAL_Info[VinId][ChainId].Frame.TotalShutterLine;
        if (TotalShutterLine <= 2U) {
            pLiveViewInfo->MaxMainShtTime[0] = 0.0f;
            pLiveViewInfo->MinMainShtTime[0] = 0.0f;
        } else {
            /* max exposure time (frame) */
            TotalNsInShutterLine = AmbaImgSensorHAL_Info[VinId][ChainId].Frame.TotalNsInShutterLine;
            MaxExposureLine = TotalShutterLine - 2U;
            MaxExposureTimeInNs[0] = MaxExposureLine*TotalNsInShutterLine;
            pLiveViewInfo->MaxMainShtTime[0] = ((FLOAT) MaxExposureTimeInNs[0])/1000000000.0f;
            /* min exposure time (frame) */
            MinExposureLine = 1U;
            MinExposureTimeInNs[0] = MinExposureLine*TotalNsInShutterLine;
            pLiveViewInfo->MinMainShtTime[0] = ((FLOAT) MinExposureTimeInNs[0])/1000000000.0f;
        }
    } else {
        /* hdr */
        TotalNsInShutterLine = AmbaImgSensorHAL_Info[VinId][ChainId].Frame.TotalNsInShutterLine;
        for (i = 0U; i < ChannelNum; i++) {
            /* max exposure time (frame) */
            MaxExposureLine = AmbaImgSensorHAL_HdrInfo[VinId][ChainId].Frame.ExposureLine[i].Max;
            MaxExposureTimeInNs[i] = MaxExposureLine*TotalNsInShutterLine;
            pLiveViewInfo->MaxMainShtTime[i] = ((FLOAT) MaxExposureTimeInNs[i])/1000000000.0f;
            /* min exposure time (frame) */
            MinExposureLine = AmbaImgSensorHAL_HdrInfo[VinId][ChainId].Frame.ExposureLine[i].Min;
            MinExposureTimeInNs[i] = MinExposureLine*TotalNsInShutterLine;
            pLiveViewInfo->MinMainShtTime[i] = ((FLOAT) MinExposureTimeInNs[i])/1000000000.0f;
        }
    }
#endif
#if 1
    /* debug info */
    {
        static UINT32 AlgoLivCounter[16] = {0};
        if ((AlgoLivCounter[ViewId] % 100U) == 0U) {
            /* liv info */
            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
            AmbaImgPrint(PRINT_FLAG_DBG, "vin", VinId);
            AmbaImgPrint(PRINT_FLAG_DBG, "chain", ChainId);
            AmbaImgPrintEx(PRINT_FLAG_DBG, "zone", ZoneId, 16U);
            AmbaImgPrint(PRINT_FLAG_DBG, "algo", ViewId);
            AmbaImgPrint(PRINT_FLAG_DBG, "width", Width);
            AmbaImgPrint(PRINT_FLAG_DBG, "height", Height);
#ifdef CONFIG_BUILD_IMGFRW_AAA_SHR_BDY
            {
                UINT32 AlgoLivPrintFlag = 0U;
                /* max/min exposure time (frame) */
                AmbaImgPrint_EnableGet(&AlgoLivPrintFlag);
                if ((AlgoLivPrintFlag & PRINT_FLAG_DBG) > 0U) {
                    if (ChannelNum == 0U) {
                        /* non-hdr */
                        UINT32 Argc = 2U;
                        char Str[2][11];
                        const char *Argv[2];
                        FLOAT VarF;
                        Argv[0] = Str[0];
                        Argv[1] = Str[1];
                        var_utoa(MaxExposureTimeInNs[0], &(Str[0][0]), 10U, 10U, (UINT32) VAR_LEADING_NONE);
                        VarF = pLiveViewInfo->MaxMainShtTime[0]*1000000000.0f;
                        var_utoa((VarF > 0.0f) ? (UINT32) VarF : 0U, &(Str[1][0]), 10U, 10U, (UINT32) VAR_LEADING_NONE);
                        var_print("max exposure time = (%s, %s)", &Argc, Argv);
                        var_utoa(MinExposureTimeInNs[0], &(Str[0][0]), 10U, 10U, (UINT32) VAR_LEADING_NONE);
                        VarF = pLiveViewInfo->MinMainShtTime[0]*1000000000.0f;
                        var_utoa((VarF > 0.0f) ? (UINT32) VarF : 0U, &(Str[1][0]), 10U, 10U, (UINT32) VAR_LEADING_NONE);
                        var_print("min exposure time = (%s, %s)", &Argc, Argv);
                    } else {
                        /* hdr */
                        UINT32 Argc = 3U;
                        char Str[3][11];
                        const char *Argv[3];
                        FLOAT VarF;
                        Argv[0] = Str[0];
                        Argv[1] = Str[1];
                        Argv[2] = Str[2];
                        for (i = 0U; i < ChannelNum; i++) {
                            var_utoa(i, &(Str[0][0]), 10U, 10U, (UINT32) VAR_LEADING_NONE);
                            var_utoa(MaxExposureTimeInNs[i], &(Str[1][0]), 10U, 10U, (UINT32) VAR_LEADING_NONE);
                            VarF = pLiveViewInfo->MaxMainShtTime[i]*1000000000.0f;
                            var_utoa((VarF > 0.0f) ? (UINT32) VarF : 0U, &(Str[2][0]), 10U, 10U, (UINT32) VAR_LEADING_NONE);
                            var_print("hdr max exposure time (%s) = (%s, %s)", &Argc, Argv);
                            var_utoa(MinExposureTimeInNs[i], &(Str[1][0]), 10U, 10U, (UINT32) VAR_LEADING_NONE);
                            VarF = pLiveViewInfo->MinMainShtTime[i]*1000000000.0f;
                            var_utoa((VarF > 0.0f) ? (UINT32) VarF : 0U, &(Str[2][0]), 10U, 10U, (UINT32) VAR_LEADING_NONE);
                            var_print("hdr min exposure time (%s) = (%s, %s)", &Argc, Argv);
                        }
                    }
                }
            }
#endif
            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
            AlgoLivCounter[ViewId]++;
        }
    }
#endif
}
#if 0
/**
 *  Amba image main convert value to pointer
 *  @param[out] pParam1 pointer to dest parameter
 *  @param[in] Param2 src parameter
 */
void AmbaImgMain_MemV2P(void *pParam1, const AMBA_IMG_MAIN_MEM_ADDR Param2)
{
    UINT32 FuncRetCode;

    FuncRetCode = AmbaWrap_memcpy(pParam1, &Param2, sizeof(AMBA_IMG_MAIN_MEM_ADDR));
    if (FuncRetCode != 0U) {
        /* */
    }
}

/**
 *  Amba image main convert pointer to pointer
 *  @param[out] pParam1 pointer to dest parameter
 *  @param[in] pParam2 pointer to src parameter
 */
void AmbaImgMain_MemP2P(void *pParam1, const void *pParam2)
{
    UINT32 FuncRetCode;

    FuncRetCode = AmbaWrap_memcpy(pParam1, &pParam2, sizeof(void *));
    if (FuncRetCode != 0U) {
        /* */
    }
}

/**
 *  Amba image main convert pointer to value
 *  @param[out] pParam1 pointer to dest parameter
 *  @param[in] pParam2 pointer to src parameter
 */
void AmbaImgMain_MemP2V(void *pParam1, const void *pParam2)
{
    UINT32 FuncRetCode;

    FuncRetCode = AmbaWrap_memcpy(pParam1, &pParam2, sizeof(void *));
    if (FuncRetCode != 0U) {
        /* */
    }
}

/**
 *  Amba image main convert alt pointer to value
 *  @param[out] pParam1 pointer to dest parameter
 *  @param[in] pParam2 pointer to src parameter
 */
void AmbaImgMain_MemF2V(void *pParam1, const void *pParam2)
{
    UINT32 FuncRetCode;

    FuncRetCode = AmbaWrap_memcpy(pParam1, pParam2, sizeof(void *));
    if (FuncRetCode != 0U) {
        /* */
    }
}
#endif
/**
 *  @private
 *  Amba image main timing mark put by vin
 *  @param[in] VinId vin id
 *  @param[in] pName pointer to the string
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMain_TimingMarkPut(UINT32 VinId, const char *pName)
{
    UINT32 FuncRetCode = NG_UL;
    UINT32 i, j;

    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;

    if (VinId < AMBA_IMG_NUM_VIN_CHANNEL) {
        /* vin channel */
        if ((pAmbaImgMainChannel != NULL) && (pAmbaImgMainChannel[VinId] != NULL)) {
            for (j = 0U; pAmbaImgMainChannel[VinId][j].Magic == 0xCafeU; j++) {
                /* ctx check */
                if (pAmbaImgMainChannel[VinId][j].pCtx != NULL) {
                    /* ctx get */
                    pCtx = pAmbaImgMainChannel[VinId][j].pCtx;
                    /* timing mark */
                    FuncRetCode = AmbaImgSensorHAL_TimingMarkPutE(pCtx->Id.Ctx.Bits.VinId, pName);
                    break;
                }
            }
        }
    } else if (VinId == AMBA_IMG_NUM_VIN_CHANNEL) {
        /* search for first or vr master */
        UINT32 ChainId;
        UINT32 VrId;
        UINT32 VrAltId;
        UINT32 BreakFlag = 0U;
        for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
            if (BreakFlag != 0U) {
                break;
            }
            if ((pAmbaImgMainChannel != NULL) &&
                (pAmbaImgMainChannel[i] != NULL)) {
                for (j = 0U; pAmbaImgMainChannel[i][j].Magic == 0xCafeU; j++) {
                    /* ctx check */
                    if (pAmbaImgMainChannel[i][j].pCtx != NULL) {
                        /* ctx get */
                        pCtx = pAmbaImgMainChannel[i][j].pCtx;

                        /* chain id get */
                        ChainId = pCtx->Id.Ctx.Bits.ChainId;
                        /* vr id get */
                        VrId = pCtx->VrMap.Id.Ctx.Bits.VrId;
                        /* vr alt id get */
                        VrAltId = pCtx->VrMap.Id.Ctx.Bits.VrAltId;

                        /* non-vr/vr-master */
                        if ((VrId == 0U/*non-vr*/) ||
                            (((VrId & (((UINT32) 1U) << pCtx->Id.Ctx.Bits.VinId)) > 0U) && ((VrAltId & (((UINT32) 1U) << ChainId)) > 0U)/*vr master*/)) {
                            /* timing mark */
                            FuncRetCode = AmbaImgSensorHAL_TimingMarkPutE(pCtx->Id.Ctx.Bits.VinId, pName);
                            BreakFlag = 1U;
                            break;
                        }
                    }
                }
            }
        }
    } else {
        /* broadcast */
        for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
            if ((pAmbaImgMainChannel != NULL) &&
                (pAmbaImgMainChannel[i] != NULL)) {
                for (j = 0U; pAmbaImgMainChannel[i][j].Magic == 0xCafeU; j++) {
                    /* ctx check */
                    if (pAmbaImgMainChannel[i][j].pCtx != NULL) {
                        /* ctx get */
                        pCtx = pAmbaImgMainChannel[i][j].pCtx;
                        /* timing mark */
                        FuncRetCode = AmbaImgSensorHAL_TimingMarkPutE(pCtx->Id.Ctx.Bits.VinId, pName);
                        break;
                    }
                }
            }
        }
    }

    return FuncRetCode;
}

/**
 *  @private
 *  Amba image main timing mark put by fov
 *  @param[in] FovId fov id
 *  @param[in] pName pointer to the string
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMain_TimingMarkPutByFov(UINT32 FovId, const char *pName)
{
    UINT32 FuncRetCode = NG_UL;
    UINT32 i, j;

    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;

    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        if ((pAmbaImgMainChannel != NULL) &&
            (pAmbaImgMainChannel[i] != NULL)) {
            for (j = 0U; pAmbaImgMainChannel[i][j].Magic == 0xCafeU; j++) {
                /* ctx check */
                if (pAmbaImgMainChannel[i][j].pCtx != NULL) {
                    /* ctx get */
                    pCtx = pAmbaImgMainChannel[i][j].pCtx;
                    /* fov belong? */
                    if ((pCtx->Id.Ctx.Bits.ZoneId & (((UINT32) 1U) << (FovId & 0x1FU))) > 0U) {
                        /* timing mark */
                        FuncRetCode = AmbaImgSensorHAL_TimingMarkPutE(pCtx->Id.Ctx.Bits.VinId, pName);
                        break;
                    }
                }
            }
        }
    }

    return FuncRetCode;
}

/**
 *  @private
 *  Amba image main timing mark put by vout
 *  @param[in] VoutId vout id
 *  @param[in] pName pointer to the string
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMain_TimingMarkPutByVout(UINT32 VoutId, const char *pName)
{
    UINT32 FuncRetCode;

    (void) VoutId;

    /* broadcast */
    FuncRetCode = AmbaImgMain_TimingMarkPut(0xFU, pName);

    return FuncRetCode;
}

