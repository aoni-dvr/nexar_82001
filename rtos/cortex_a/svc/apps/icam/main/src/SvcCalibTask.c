/**
 *  @file SvcCalibTask.c
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
 *  @details svc calibration task
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"
#include "AmbaNVM_Partition.h"
#include "AmbaNAND.h"
#include "AmbaNAND_FTL.h"
#include "AmbaDSP.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaImg_Proc.h"
#include "AmbaImg_External_CtrlFunc.h"
#include "Amba_AwbCalib.h"
#include "AmbaShell.h"
#include "AmbaFPD.h"
#include "AmbaSensor.h"
#include "AmbaSvcWrap.h"

#include "SvcErrCode.h"
#include "SvcMem.h"
#include "SvcBufMap.h"
#include "SvcBuffer.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcCmd.h"
#include "SvcRawCap.h"
#include "SvcIK.h"
#ifdef CONFIG_BUILD_IMGFRW_AAA
#include "SvcImg.h"
#endif
#ifdef CONFIG_BUILD_SVC_FRAMEWORK_SERDESQUERY
#include "SvcSerDesQry.h"
#endif
#include "SvcVinSrc.h"
#include "SvcVoutSrc.h"
#include "SvcResCfg.h"
#include "SvcSysStat.h"
#include "SvcLiveview.h"
#include "SvcDisplay.h"
#include "SvcInfoPack.h"
#include "SvcCmd.h"
#include "SvcAppStat.h"
#include "SvcUserPref.h"
#include "SvcCalibMgr.h"
#include "SvcCalibCfg.h"
#include "SvcCalibTask.h"
#ifdef CONFIG_ICAM_EEPROM_USED
#include "SvcCalibModuleTask.h"
#endif
#include "SvcResCfgTask.h"


static void   SvcCalibTask_ErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode);
static void   SvcCalibTask_ImgAlgoIDGet(SVC_CALIB_CHANNEL_s *pCalibChan, UINT32 *pImgAlgoID, UINT32 *pImgAlgoNum);
static void   SvcCalibTask_ImgAlgoIDPreGet(SVC_CALIB_CHANNEL_s *pCalibChan, UINT32 *pImgAlgoID, UINT32 *pImgAlgoNum);
static void   SvcCalibTask_CalibChTblCfg(void);
static void   SvcCalibTask_ExtendDataGet(UINT32 VinID, UINT32 FovIdx, UINT32 *pExtData);
static UINT32 SvcCalibTask_WbUpdate(SVC_CALIB_CHANNEL_s *pCalibChan);
static void   SvcCalibTask_CmdAppEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
static void   SvcCalitTask_CmdInstall(void);
static UINT32 SvcCalibTask_GetCurFmtID(void);
static UINT32 SvcCalibTask_CfaDataRdyHdlr(const void *pEventData);

typedef struct {
    UINT32              CmdID;
    SVC_CALIB_CALC_CB_f CmdHdlr;
} SVC_CALIB_CALC_CMD_HDLR_s;

static void   SvcCalibTask_CalcCmdInit(void);
static UINT32 SvcCalibTask_CalcCmdStart(UINT32 CalID, UINT32 CalcCmd, void *pInfo);
static UINT32 SvcCalibTask_CalcCmdDone(UINT32 CalID, UINT32 CalcCmd, void *pInfo);
static UINT32 SvcCalibTask_CalcCmdWbInfoGet(UINT32 CalID, UINT32 CalcCmd, void *pInfo);
static UINT32 SvcCalibTask_CalcCmdMemLock(UINT32 CalID, UINT32 CalcCmd, void *pInfo);
static UINT32 SvcCalibTask_CalcCmdStgDriver(UINT32 CalID, UINT32 CalcCmd, void *pInfo);
static UINT32 SvcCalibTask_CalcCmdFunc(UINT32 CalID, UINT32 CalcCmd, void *pInfo);

#ifdef SVC_CAP_MAX_SENSOR_PER_VIN
#define SVC_CAL_MAX_SENSOR_PRE_VIN  SVC_CAP_MAX_SENSOR_PER_VIN
#else
#define SVC_CAL_MAX_SENSOR_PRE_VIN  4U
#endif

static UINT32                    CalibVZToImgAlgo[AMBA_DSP_MAX_VIEWZONE_NUM] GNU_SECTION_NOZEROINIT;
static UINT32                    CalibChToSenIdx[AMBA_DSP_MAX_VIN_NUM][SVC_CAL_MAX_SENSOR_PRE_VIN] GNU_SECTION_NOZEROINIT;
static SVC_CALIB_CALC_CMD_HDLR_s CalibCalcCmdHdlr[SVC_CALIB_CALC_CMD_NUM] GNU_SECTION_NOZEROINIT;
static UINT32 SVC_CalibTskFlg = SVC_LOG_CAL_DEF_FLG;

#define SVC_LOG_CAL_TSK "CALTSK"
#define PRN_CAL_TSK_LOG        { SVC_WRAP_PRINT_s CalibTskPrint; AmbaSvcWrap_MisraMemset(&(CalibTskPrint), 0, sizeof(CalibTskPrint)); CalibTskPrint.Argc --; CalibTskPrint.pStrFmt =
#define PRN_CAL_TSK_ARG_UINT32 ; CalibTskPrint.Argc ++; CalibTskPrint.Argv[CalibTskPrint.Argc].Uint64   = (UINT64)((
#define PRN_CAL_TSK_ARG_CSTR   ; CalibTskPrint.Argc ++; CalibTskPrint.Argv[CalibTskPrint.Argc].pCStr    = ((
#define PRN_CAL_TSK_ARG_CPOINT ; CalibTskPrint.Argc ++; CalibTskPrint.Argv[CalibTskPrint.Argc].pPointer = ((
#define PRN_CAL_TSK_ARG_POST   ))
#define PRN_CAL_TSK_OK         ; CalibTskPrint.Argc ++; SvcCalib_TskPrintLog(SVC_LOG_CAL_OK , &(CalibTskPrint)); }
#define PRN_CAL_TSK_NG         ; CalibTskPrint.Argc ++; SvcCalib_TskPrintLog(SVC_LOG_CAL_NG , &(CalibTskPrint)); }
#define PRN_CAL_TSK_API        ; CalibTskPrint.Argc ++; SvcCalib_TskPrintLog(SVC_LOG_CAL_API, &(CalibTskPrint)); }
#define PRN_CAL_TSK_DBG        ; CalibTskPrint.Argc ++; SvcCalib_TskPrintLog(SVC_LOG_CAL_DBG, &(CalibTskPrint)); }
#define PRN_CAL_TSK_ERR_HDLR   SvcCalibTask_ErrHdlr(__func__, __LINE__, PRetVal);

static void SvcCalib_TskPrintLog(UINT32 LogLevel, SVC_WRAP_PRINT_s *pPrint)
{
    if (pPrint != NULL) {
        if ((SVC_CalibTskFlg & LogLevel) > 0U) {
            switch (LogLevel) {
            case SVC_LOG_CAL_OK :
                pPrint->pProc = SvcLog_OK;
                break;
            case SVC_LOG_CAL_NG :
                pPrint->pProc = SvcLog_NG;
                break;
            default :
                pPrint->pProc = SvcLog_DBG;
                break;
            }

            SvcWrap_Print(SVC_LOG_CAL_TSK, pPrint);
        }
    }
}

static void SvcCalibTask_ErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode)
{
    if (pCaller != NULL) {
        if (ErrCode != 0U) {
            PRN_CAL_TSK_LOG "Catch ErrCode(0x%08x) @ %s, %d"
                PRN_CAL_TSK_ARG_UINT32 ErrCode  PRN_CAL_TSK_ARG_POST
                PRN_CAL_TSK_ARG_CSTR   pCaller  PRN_CAL_TSK_ARG_POST
                PRN_CAL_TSK_ARG_UINT32 CodeLine PRN_CAL_TSK_ARG_POST
            PRN_CAL_TSK_NG
        }
    }
}

static void SvcCalibTask_ImgAlgoIDGet(SVC_CALIB_CHANNEL_s *pCalibChan, UINT32 *pImgAlgoID, UINT32 *pImgAlgoNum)
{
    if (pCalibChan == NULL) {
        PRN_CAL_TSK_LOG "Fail to get image algo id - input calib channel should not null!" PRN_CAL_TSK_NG
    } else if (pImgAlgoID == NULL) {
        PRN_CAL_TSK_LOG "Fail to get image algo id - output img algo id should not null!" PRN_CAL_TSK_NG
    } else if (pImgAlgoNum == NULL) {
        PRN_CAL_TSK_LOG "Fail to get image algo id - output img algo num should not null!" PRN_CAL_TSK_NG
    } else {
#ifdef CONFIG_BUILD_IMGFRW_AAA
        UINT32 SerDesIdx;

        for (SerDesIdx = 0U; SerDesIdx < SVC_CAL_MAX_SENSOR_PRE_VIN; SerDesIdx ++) {
            if ((pCalibChan->SensorID & SvcCalib_BitGet(SerDesIdx)) > 0U) {
                break;
            }
        }

        if (SerDesIdx >= SVC_CAL_MAX_SENSOR_PRE_VIN) {
            PRN_CAL_TSK_LOG "Fail to get image algo id - invalid serdes index(%d)!"
                PRN_CAL_TSK_ARG_UINT32 SVC_CAL_MAX_SENSOR_PRE_VIN PRN_CAL_TSK_ARG_POST
            PRN_CAL_TSK_NG
        } else {
            if (0U != SvcImg_AlgoIdGetEx(pCalibChan->VinID, CalibChToSenIdx[pCalibChan->VinID][SerDesIdx], pImgAlgoID, pImgAlgoNum)) {
                PRN_CAL_TSK_LOG "Fail to get image algo id - get the ImgProcAlgoID fail!" PRN_CAL_TSK_NG
            } else {
                PRN_CAL_TSK_LOG "Get pImgAlgoID[0]=%d by CalibVinID(%d), CalibSensorID(0x%X), SensorIdx(%d)"
                PRN_CAL_TSK_ARG_UINT32 pImgAlgoID[0] PRN_CAL_TSK_ARG_POST
                PRN_CAL_TSK_ARG_UINT32 pCalibChan->VinID PRN_CAL_TSK_ARG_POST
                PRN_CAL_TSK_ARG_UINT32 pCalibChan->SensorID PRN_CAL_TSK_ARG_POST
                PRN_CAL_TSK_ARG_UINT32 CalibChToSenIdx[pCalibChan->VinID][SerDesIdx] PRN_CAL_TSK_ARG_POST
                PRN_CAL_TSK_DBG
            }
        }
#else
        PRN_CAL_TSK_LOG "The system does not enable Image Framework." PRN_CAL_TSK_API
#endif
        AmbaMisra_TouchUnused(pCalibChan);
        AmbaMisra_TouchUnused(pImgAlgoID);
        AmbaMisra_TouchUnused(pImgAlgoNum);
    }
}


static void SvcCalibTask_ImgAlgoIDPreGet(SVC_CALIB_CHANNEL_s *pCalibChan, UINT32 *pImgAlgoID, UINT32 *pImgAlgoNum)
{
    if (pCalibChan == NULL) {
        PRN_CAL_TSK_LOG "Fail to pre-get image algo id - input calib channel should not null!" PRN_CAL_TSK_NG
    } else if (pImgAlgoID == NULL) {
        PRN_CAL_TSK_LOG "Fail to pre-get image algo id - output img algo id should not null!" PRN_CAL_TSK_NG
    } else if (pImgAlgoNum == NULL) {
        PRN_CAL_TSK_LOG "Fail to pre-get image algo id - output img algo num should not null!" PRN_CAL_TSK_NG
    } else {
#ifdef CONFIG_BUILD_IMGFRW_AAA
        UINT32 SerDesIdx;

        for (SerDesIdx = 0U; SerDesIdx < SVC_CAL_MAX_SENSOR_PRE_VIN; SerDesIdx ++) {
            if ((pCalibChan->SensorID & SvcCalib_BitGet(SerDesIdx)) > 0U) {
                break;
            }
        }

        if (SerDesIdx >= SVC_CAL_MAX_SENSOR_PRE_VIN) {
            PRN_CAL_TSK_LOG "Fail to pre-get image algo id - invalid serdes index(%d)!"
                PRN_CAL_TSK_ARG_UINT32 SVC_CAL_MAX_SENSOR_PRE_VIN PRN_CAL_TSK_ARG_POST
            PRN_CAL_TSK_NG
        } else {
            if (0U != SvcImg_AlgoIdPreGetEx(pCalibChan->VinID, CalibChToSenIdx[pCalibChan->VinID][SerDesIdx], pImgAlgoID, pImgAlgoNum)) {
                PRN_CAL_TSK_LOG "Fail to pre-get image algo id - get the ImgProcAlgoID fail!" PRN_CAL_TSK_NG
            } else {
                PRN_CAL_TSK_LOG "Pre-get pImgAlgoID[0]=%d by CalibVinID(%d), CalibSensorID(0x%X), SensorIdx(%d)"
                PRN_CAL_TSK_ARG_UINT32 pImgAlgoID[0] PRN_CAL_TSK_ARG_POST
                PRN_CAL_TSK_ARG_UINT32 pCalibChan->VinID PRN_CAL_TSK_ARG_POST
                PRN_CAL_TSK_ARG_UINT32 pCalibChan->SensorID PRN_CAL_TSK_ARG_POST
                PRN_CAL_TSK_ARG_UINT32 CalibChToSenIdx[pCalibChan->VinID][SerDesIdx] PRN_CAL_TSK_ARG_POST
                PRN_CAL_TSK_DBG
            }
        }
#else
        PRN_CAL_TSK_LOG "The system does not enable Image Framework." PRN_CAL_TSK_API
        AmbaMisra_TouchUnused(&(CalibChToSenIdx[0U][0U]));
#endif
        AmbaMisra_TouchUnused(pCalibChan);
        AmbaMisra_TouchUnused(pImgAlgoID);
        AmbaMisra_TouchUnused(pImgAlgoNum);
    }
}

static void SvcCalibTask_CalibChTblCfg(void)
{
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    AmbaSvcWrap_MisraMemset(CalibVZToImgAlgo, 255, sizeof(CalibVZToImgAlgo));
    AmbaSvcWrap_MisraMemset(CalibChToSenIdx, 255, sizeof(CalibChToSenIdx));

    if (pResCfg != NULL) {
        UINT32 PRetVal;
        UINT32 ViewZoneID;
        UINT32 VinID;
        UINT32 SensorIdx, SerDesIdx;
        UINT32 ImgProcAlgoID[AMBA_DSP_MAX_VIN_TD_NUM], ImgProcAlgoNum = 0U, Idx = 0U;

        for (ViewZoneID = 0U; ViewZoneID < pResCfg->FovNum; ViewZoneID ++ ) {
            if (ViewZoneID < AMBA_DSP_MAX_VIEWZONE_NUM) {
                PRetVal = SVC_OK;
                VinID = 255U;
                SensorIdx = 255U;
                SerDesIdx = 255U;
                if (0U == SvcResCfg_GetSensorIdxOfFovIdx(ViewZoneID, &VinID, &SensorIdx)) {
                    if ((VinID < AMBA_DSP_MAX_VIN_NUM) && (SensorIdx < SVC_CAL_MAX_SENSOR_PRE_VIN)) {

                        if (pResCfg->VinCfg[VinID].SerdesType == SVC_RES_SERDES_TYPE_NONE) {
                            SerDesIdx = 0U;
                        } else if (0U != SvcResCfg_GetSerdesIdxOfFovIdx(ViewZoneID, &SerDesIdx)) {
                            PRetVal = SVC_NG;
                            PRN_CAL_TSK_LOG "Fail to get SerDesIdx by ViewZoneID(%d)"
                                PRN_CAL_TSK_ARG_UINT32 ViewZoneID PRN_CAL_TSK_ARG_POST
                            PRN_CAL_TSK_NG
                        } else if (SerDesIdx >= SVC_CAL_MAX_SENSOR_PRE_VIN) {
                            PRetVal = SVC_NG;
                            PRN_CAL_TSK_LOG "Fail to get invalid SerDesIdx(%d / %d)."
                                PRN_CAL_TSK_ARG_UINT32 SerDesIdx PRN_CAL_TSK_ARG_POST
                            PRN_CAL_TSK_NG
                        } else {
                            // misra-c
                        }

                        if (PRetVal == SVC_OK) {

                            CalibChToSenIdx[VinID][SerDesIdx] = SensorIdx;

                            ImgProcAlgoNum = 0U;
                            AmbaSvcWrap_MisraMemset(ImgProcAlgoID, 255, sizeof(ImgProcAlgoID));

#ifdef CONFIG_BUILD_IMGFRW_AAA
                            if (0U != SvcImg_AlgoIdPreGetEx(VinID, SensorIdx, ImgProcAlgoID, &ImgProcAlgoNum)) {
                                PRN_CAL_TSK_LOG "Fail to get image algo id - get the ImgProcAlgoID fail!" PRN_CAL_TSK_NG
                            } else {
                                for (Idx = 0U; Idx < ImgProcAlgoNum; Idx ++) {
                                    if (ImgProcAlgoID[Idx] < AMBA_DSP_MAX_VIEWZONE_NUM) {
                                        CalibVZToImgAlgo[ViewZoneID] = ImgProcAlgoID[Idx];
                                    }
                                }
                            }
#else
                            PRN_CAL_TSK_LOG "The system does not enable Image Framework." PRN_CAL_TSK_API
                            AmbaMisra_TouchUnused(&ImgProcAlgoNum);
                            AmbaMisra_TouchUnused(ImgProcAlgoID);
#endif
                        }
                    }
                }
            }
        }

        if ((SVC_CalibTskFlg & SVC_LOG_CAL_DBG) > 0U) {
            PRN_CAL_TSK_LOG " " PRN_CAL_TSK_API
            PRN_CAL_TSK_LOG "---- Calib Chan To SensorIdx ----" PRN_CAL_TSK_API
            PRN_CAL_TSK_LOG "  VinID | SerDesIdx | SensorIdx" PRN_CAL_TSK_API

            for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
                for (SerDesIdx = 0U; SerDesIdx < SVC_CAL_MAX_SENSOR_PRE_VIN; SerDesIdx ++) {
                    if (CalibChToSenIdx[VinID][SerDesIdx] < SVC_MAX_NUM_SENSOR_PER_VIN) {
                        PRN_CAL_TSK_LOG "  %5d |     %2d    |   %2d"
                        PRN_CAL_TSK_ARG_UINT32 VinID                             PRN_CAL_TSK_ARG_POST
                        PRN_CAL_TSK_ARG_UINT32 SerDesIdx                         PRN_CAL_TSK_ARG_POST
                        PRN_CAL_TSK_ARG_UINT32 CalibChToSenIdx[VinID][SerDesIdx] PRN_CAL_TSK_ARG_POST
                        PRN_CAL_TSK_API
                    }
                }
            }
            PRN_CAL_TSK_LOG "---- Calib ViewZone To ImageAlgoID ----" PRN_CAL_TSK_API
            PRN_CAL_TSK_LOG "  ViewZoneID | ImageAlgoID" PRN_CAL_TSK_API

            for (ViewZoneID = 0U; ViewZoneID < AMBA_DSP_MAX_VIEWZONE_NUM; ViewZoneID ++) {
                if (CalibVZToImgAlgo[ViewZoneID] < AMBA_DSP_MAX_VIEWZONE_NUM) {
                    PRN_CAL_TSK_LOG "          %2d | %2d"
                    PRN_CAL_TSK_ARG_UINT32 ViewZoneID                   PRN_CAL_TSK_ARG_POST
                    PRN_CAL_TSK_ARG_UINT32 CalibVZToImgAlgo[ViewZoneID] PRN_CAL_TSK_ARG_POST
                    PRN_CAL_TSK_API
                }
            }
        }

        AmbaMisra_TouchUnused(&Idx);
    }
}

static void SvcCalibTask_ExtendDataGet(UINT32 VinID, UINT32 FovIdx, UINT32 *pExtData)
{
    UINT32 PRetVal;

    if (pExtData != NULL) {
        UINT32 FovSrc = 0xFFU;

        *pExtData = 0U;

        PRetVal = SvcResCfg_GetFovSrc(FovIdx, &FovSrc); PRN_CAL_TSK_ERR_HDLR

        if ((FovSrc == SVC_VIN_SRC_SENSOR) || (FovSrc == SVC_VIN_SRC_MEM)) {
            AMBA_SENSOR_CHANNEL_s SsChan;
            AMBA_SENSOR_STATUS_INFO_s SsStatus;

            AmbaSvcWrap_MisraMemset(&SsChan, 0, sizeof(SsChan));
            SsChan.VinID = VinID;
            if (0U != SvcResCfg_GetSensorIDInVinID(VinID, &(SsChan.SensorID))) {
                SsChan.SensorID = 0x10U;
            }

            AmbaSvcWrap_MisraMemset(&SsStatus, 0, sizeof(SsStatus));

            if (0U == AmbaSensor_GetStatus(&SsChan, &SsStatus)) {
                *pExtData = SsStatus.ModeInfo.Config.ModeID;

                PRN_CAL_TSK_LOG "  SsStatus.ModeInfo.Config.ModeID(%d)"
                    PRN_CAL_TSK_ARG_UINT32 SsStatus.ModeInfo.Config.ModeID PRN_CAL_TSK_ARG_POST
                PRN_CAL_TSK_API
            }
        }
    }
}


static UINT32 SvcCalibTask_WbUpdate(SVC_CALIB_CHANNEL_s *pCalibChan)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (pCalibChan == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_TSK_LOG "Fail to update wb gain - calib channel should not null!" PRN_CAL_TSK_NG
    } else {
#ifdef CONFIG_BUILD_IMGFRW_AAA
        SVC_CALIB_TBL_INFO_s    TblInfo;
        SVC_CALIB_WB_TBL_DATA_s WbTblData;
        UINT32 Idx, TmpGainR, TmpGainB;
        UINT32 ImgProcAlgoID[AMBA_DSP_MAX_VIN_TD_NUM], ImgProcAlgoNum = 0U;

        AmbaSvcWrap_MisraMemset(ImgProcAlgoID, 255, sizeof(ImgProcAlgoID));

        SvcCalibTask_ImgAlgoIDPreGet(pCalibChan, ImgProcAlgoID, &ImgProcAlgoNum);
        for (Idx = 0U; Idx < ImgProcAlgoNum; Idx ++) {
            if (ImgProcAlgoID[Idx] == 0xFFFFFFFFU) {
                RetVal |= SVC_NG;
                PRN_CAL_TSK_LOG "Fail to update wb gain - invalid ImgProcAlgoID(0x%x)"
                    PRN_CAL_TSK_ARG_UINT32 ImgProcAlgoID[Idx] PRN_CAL_TSK_ARG_POST
                PRN_CAL_TSK_NG
            } else {
                AmbaSvcWrap_MisraMemset(&TblInfo, 0, sizeof(TblInfo));
                TblInfo.CalChan.VinID    = pCalibChan->VinID;
                TblInfo.CalChan.SensorID = pCalibChan->SensorID;

                AmbaSvcWrap_MisraMemset(&WbTblData, 0, sizeof(WbTblData));
                TblInfo.BufSize = (UINT32) sizeof(WbTblData);
                TblInfo.pBuf    = &(WbTblData);

                if (SVC_OK == SvcCalib_ItemTableGet(SVC_CALIB_WB_ID, 255U, &TblInfo)) {
                    UINT32 FlickerMode = 60U;
                    WB_Detect_Info_s LctWbInfo, HctWbInfo;

                    // Low color temperature
                    AmbaSvcWrap_MisraMemset(&LctWbInfo, 0, sizeof(LctWbInfo));
                    LctWbInfo.CurR = WbTblData.OriGain[SVC_CALIB_WB_LCT].GainR;
                    LctWbInfo.CurG = WbTblData.OriGain[SVC_CALIB_WB_LCT].GainG;
                    LctWbInfo.CurB = WbTblData.OriGain[SVC_CALIB_WB_LCT].GainB;
                    LctWbInfo.RefR = WbTblData.RefGain[SVC_CALIB_WB_LCT].GainR;
                    LctWbInfo.RefG = WbTblData.RefGain[SVC_CALIB_WB_LCT].GainG;
                    LctWbInfo.RefB = WbTblData.RefGain[SVC_CALIB_WB_LCT].GainB;
                    PRetVal = ImgProc_WBCal(ImgProcAlgoID[Idx], WB_CAL_STORE_LCT, FlickerMode, &LctWbInfo);
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;
                        PRN_CAL_TSK_LOG "Fail to update wb gain - configure low temperature gain fail!" PRN_CAL_TSK_API
                    }

                    // High color temperature
                    AmbaSvcWrap_MisraMemset(&HctWbInfo, 0, sizeof(HctWbInfo));
                    HctWbInfo.CurR = WbTblData.OriGain[SVC_CALIB_WB_HCT].GainR;
                    HctWbInfo.CurG = WbTblData.OriGain[SVC_CALIB_WB_HCT].GainG;
                    HctWbInfo.CurB = WbTblData.OriGain[SVC_CALIB_WB_HCT].GainB;
                    HctWbInfo.RefR = WbTblData.RefGain[SVC_CALIB_WB_HCT].GainR;
                    HctWbInfo.RefG = WbTblData.RefGain[SVC_CALIB_WB_HCT].GainG;
                    HctWbInfo.RefB = WbTblData.RefGain[SVC_CALIB_WB_HCT].GainB;
                    PRetVal = ImgProc_WBCal(ImgProcAlgoID[Idx], WB_CAL_STORE_HCT, FlickerMode, &HctWbInfo);
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;
                        PRN_CAL_TSK_LOG "Fail to update wb gain - configure high temperature gain fail!" PRN_CAL_TSK_API
                    }

                    PRN_CAL_TSK_LOG "CalibWb update to ImgProcAlgo(%d)"
                        PRN_CAL_TSK_ARG_UINT32 ImgProcAlgoID[Idx] PRN_CAL_TSK_ARG_POST
                    PRN_CAL_TSK_API

                    TmpGainR = ( ( LctWbInfo.CurG * 4096U ) / LctWbInfo.CurR );
                    TmpGainB = ( ( LctWbInfo.CurG * 4096U ) / LctWbInfo.CurB );
                    PRN_CAL_TSK_LOG "CalibWB - update to awb low  color temperature set_gain[0].    R(%d) G(%d) B(%d)"
                        PRN_CAL_TSK_ARG_UINT32 TmpGainR PRN_CAL_TSK_ARG_POST
                        PRN_CAL_TSK_ARG_UINT32 4096U    PRN_CAL_TSK_ARG_POST
                        PRN_CAL_TSK_ARG_UINT32 TmpGainB PRN_CAL_TSK_ARG_POST
                    PRN_CAL_TSK_API

                    TmpGainR = ( ( HctWbInfo.CurG * 4096U ) / HctWbInfo.CurR );
                    TmpGainB = ( ( HctWbInfo.CurG * 4096U ) / HctWbInfo.CurB );
                    PRN_CAL_TSK_LOG "CalibWB - update to awb high color temperature set_gain[1].    R(%d) G(%d) B(%d)"
                        PRN_CAL_TSK_ARG_UINT32 TmpGainR PRN_CAL_TSK_ARG_POST
                        PRN_CAL_TSK_ARG_UINT32 4096U    PRN_CAL_TSK_ARG_POST
                        PRN_CAL_TSK_ARG_UINT32 TmpGainB PRN_CAL_TSK_ARG_POST
                    PRN_CAL_TSK_API

                    TmpGainR = ( ( LctWbInfo.RefG * 4096U ) / LctWbInfo.RefR );
                    TmpGainB = ( ( LctWbInfo.RefG * 4096U ) / LctWbInfo.RefB );
                    PRN_CAL_TSK_LOG "CalibWB - update to awb low  color temperature target_gain[0]. R(%d) G(%d) B(%d)"
                        PRN_CAL_TSK_ARG_UINT32 TmpGainR PRN_CAL_TSK_ARG_POST
                        PRN_CAL_TSK_ARG_UINT32 4096U    PRN_CAL_TSK_ARG_POST
                        PRN_CAL_TSK_ARG_UINT32 TmpGainB PRN_CAL_TSK_ARG_POST
                    PRN_CAL_TSK_API

                    TmpGainR = ( ( HctWbInfo.RefG * 4096U ) / HctWbInfo.RefR );
                    TmpGainB = ( ( HctWbInfo.RefG * 4096U ) / HctWbInfo.RefB );
                    PRN_CAL_TSK_LOG "CalibWB - update to awb high color temperature target_gain[1]. R(%d) G(%d) B(%d)"
                        PRN_CAL_TSK_ARG_UINT32 TmpGainR PRN_CAL_TSK_ARG_POST
                        PRN_CAL_TSK_ARG_UINT32 4096U    PRN_CAL_TSK_ARG_POST
                        PRN_CAL_TSK_ARG_UINT32 TmpGainB PRN_CAL_TSK_ARG_POST
                    PRN_CAL_TSK_API
                }
            }
        }
#else
        SvcCalibTask_ImgAlgoIDPreGet(NULL, NULL, NULL);
        (void) PRetVal;
#endif
    }

    return RetVal;
}


static void SvcCalibTask_CmdAppEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#ifdef CONFIG_ICAM_EEPROM_USED
    UINT32 CmdProc = 0U;
    UINT32 PRetVal;

    if (ArgCount >= 3U) {
        if (0 == SvcWrap_strcmp(pArgVector[1U], "module_upd")) {
            UINT32 On = 0U;

            PRetVal = SvcWrap_strtoul(pArgVector[2U], &On); PRN_CAL_TSK_ERR_HDLR

            SvcCalibTask_ModuleUpdSwitch(On);

            CmdProc = 1U;
        }
    } else {
        PRN_CAL_TSK_LOG " " PRN_CAL_TSK_API
        PRN_CAL_TSK_LOG "====== %sCalib Task Command Usage%s ======"
            PRN_CAL_TSK_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_0 PRN_CAL_TSK_ARG_POST
            PRN_CAL_TSK_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_TSK_ARG_POST
        PRN_CAL_TSK_API
        PRN_CAL_TSK_LOG "  %smodule_upd%s : Enable/Disable calib module auto-update"
            PRN_CAL_TSK_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_TSK_ARG_POST
            PRN_CAL_TSK_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_TSK_ARG_POST
        PRN_CAL_TSK_API
        PRN_CAL_TSK_LOG "          On : 1: Enable, 0: Disable" PRN_CAL_TSK_API
        PRN_CAL_TSK_LOG " " PRN_CAL_TSK_API
    }

    if (CmdProc == 0U)
#endif
    {
        SvcCalib_CmdAppEntry(ArgCount, pArgVector);
    }

    AmbaMisra_TouchUnused(&PrintFunc);
}


static void SvcCalitTask_CmdInstall(void)
{
    AMBA_SHELL_COMMAND_s SvcCalibCmdApp;

    SvcCalibCmdApp.pName    = "svc_cal";
    SvcCalibCmdApp.MainFunc = SvcCalibTask_CmdAppEntry;
    SvcCalibCmdApp.pNext    = NULL;

    if (SHELL_ERR_SUCCESS != SvcCmd_CommandRegister(&SvcCalibCmdApp)) {
        PRN_CAL_TSK_LOG "Fail to install svc calib command!" PRN_CAL_TSK_NG
    }
}

static UINT32 SvcCalibTask_GetCurFmtID(void)
{
    UINT32 CurFmtID = 0U;
    SVC_USER_PREF_s *pSvcUserPref = NULL;

    if (0U != SvcUserPref_Get(&pSvcUserPref)) {
        PRN_CAL_TSK_LOG "Fail to get current format_id - get user pref fail!" PRN_CAL_TSK_NG
    } else if (pSvcUserPref == NULL) {
        PRN_CAL_TSK_LOG "Fail to get current format_id - invalid user pref!" PRN_CAL_TSK_NG
    } else {
        CurFmtID = pSvcUserPref->FormatId;
    }

    return CurFmtID;
}

static UINT32 SvcCalibTask_CfaDataRdyHdlr(const void *pEventData)
{
    AMBA_IK_CFA_3A_DATA_s *pCfaData;
    AmbaMisra_TypeCast(&(pCfaData), &(pEventData));

    if (pCfaData != NULL) {
        if (pCfaData->Header.ChanIndex < AMBA_DSP_MAX_VIEWZONE_NUM) {
#ifdef CONFIG_BUILD_IMGFRW_AAA
            UINT32 RetVal;
            UINT32 ImgAlgoID = CalibVZToImgAlgo[pCfaData->Header.ChanIndex];
            if (ImgAlgoID < AMBA_DSP_MAX_VIEWZONE_NUM) {
                UINT32 Enable = 0U;
                RetVal = ImgProc_WbCalibGetEnable(ImgAlgoID, &Enable);
                if (RetVal == 0U) {
                    if (Enable > 0U) {
                        extern void AmbaImg_AwbCalibTaskDispatch(UINT32 viewId);
                        AmbaImg_AwbCalibTaskDispatch(ImgAlgoID);
                    }
                }
            }
#endif
        }
        AmbaMisra_TouchUnused(pCfaData);
    }

    return 0U;
}

static void SvcCalibTask_CalcCmdInit(void)
{
    static UINT32 InitialCalcCmdHdlr = 0U;

    if (InitialCalcCmdHdlr == 0U) {

        AmbaSvcWrap_MisraMemset(CalibCalcCmdHdlr, 0, sizeof(CalibCalcCmdHdlr));
        CalibCalcCmdHdlr[SVC_CALIB_CALC_START     ] = (SVC_CALIB_CALC_CMD_HDLR_s) { SVC_CALIB_CALC_START,       SvcCalibTask_CalcCmdStart      };
        CalibCalcCmdHdlr[SVC_CALIB_CALC_DONE      ] = (SVC_CALIB_CALC_CMD_HDLR_s) { SVC_CALIB_CALC_DONE,        SvcCalibTask_CalcCmdDone       };
        CalibCalcCmdHdlr[SVC_CALIB_CALC_WB_INFO   ] = (SVC_CALIB_CALC_CMD_HDLR_s) { SVC_CALIB_CALC_WB_INFO,     SvcCalibTask_CalcCmdWbInfoGet  };
        CalibCalcCmdHdlr[SVC_CALIB_CALC_MEM_LOCK  ] = (SVC_CALIB_CALC_CMD_HDLR_s) { SVC_CALIB_CALC_MEM_LOCK,    SvcCalibTask_CalcCmdMemLock    };
        CalibCalcCmdHdlr[SVC_CALIB_CALC_MEM_UNLOCK] = (SVC_CALIB_CALC_CMD_HDLR_s) { SVC_CALIB_CALC_MEM_UNLOCK,  SvcCalibTask_CalcCmdMemLock    };
        CalibCalcCmdHdlr[SVC_CALIB_CALC_STG_DRIVER] = (SVC_CALIB_CALC_CMD_HDLR_s) { SVC_CALIB_CALC_STG_DRIVER,  SvcCalibTask_CalcCmdStgDriver  };

        InitialCalcCmdHdlr = 1U;
    }
}


static UINT32 SvcCalibTask_CalcCmdStart(UINT32 CalID, UINT32 CalcCmd, void *pInfo)
{
    UINT32 RetVal = SVC_OK;

    PRN_CAL_TSK_LOG "Calib item %d calculation start"
        PRN_CAL_TSK_ARG_UINT32 CalID PRN_CAL_TSK_ARG_POST
    PRN_CAL_TSK_API

    if (CalID == SVC_CALIB_WB_ID) {
        SVC_CALIB_CHANNEL_s *pCalibChan = NULL;

        AmbaMisra_TypeCast(&(pCalibChan), &(pInfo));

        if (pCalibChan == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_TSK_LOG "Invalid calib channel!" PRN_CAL_TSK_NG
        } else {
#ifdef CONFIG_BUILD_IMGFRW_AAA
            UINT32 Idx;
            UINT32 ImgProcAlgoID[AMBA_DSP_MAX_VIN_TD_NUM], ImgProcAlgoNum = 0U;

            AmbaSvcWrap_MisraMemset(ImgProcAlgoID, 255, sizeof(ImgProcAlgoID));

            SvcCalibTask_ImgAlgoIDGet(pCalibChan, ImgProcAlgoID, &ImgProcAlgoNum);
            for (Idx = 0U; Idx < ImgProcAlgoNum; Idx ++) {
                if (ImgProcAlgoID[Idx] == 0xFFFFFFFFU) {
                    RetVal |= SVC_NG;
                    PRN_CAL_TSK_LOG "Configure imgproc WB calibration control with invalid ViewID %d"
                        PRN_CAL_TSK_ARG_UINT32 ImgProcAlgoID[Idx] PRN_CAL_TSK_ARG_POST
                    PRN_CAL_TSK_NG
                } else {
                    if (0U != ImgProc_WbCalibCtrl(ImgProcAlgoID[Idx])) {
                        RetVal |= SVC_NG;
                        PRN_CAL_TSK_LOG "Configure imgproc WB calibration control fail! ViewID %d"
                            PRN_CAL_TSK_ARG_UINT32 ImgProcAlgoID[Idx] PRN_CAL_TSK_ARG_POST
                        PRN_CAL_TSK_NG
                    }
                }
            }
#endif
        }
    } else {
        AmbaMisra_TouchUnused(&CalcCmd);
        AmbaMisra_TouchUnused(pInfo);
    }
    return RetVal;
}


static UINT32 SvcCalibTask_CalcCmdDone(UINT32 CalID, UINT32 CalcCmd, void *pInfo)
{
    PRN_CAL_TSK_LOG "Calib item %d calculation done"
        PRN_CAL_TSK_ARG_UINT32 CalID PRN_CAL_TSK_ARG_POST
    PRN_CAL_TSK_API
    AmbaMisra_TouchUnused(pInfo);
    AmbaMisra_TouchUnused(&CalcCmd);
    AmbaMisra_TouchUnused(&pInfo);
    return SVC_OK;
}


static UINT32 SvcCalibTask_CalcCmdWbInfoGet(UINT32 CalID, UINT32 CalcCmd, void *pInfo)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    SVC_CALIB_CALC_WB_INFO_s *pWbInfo;

    AmbaMisra_TypeCast(&(pWbInfo), &(pInfo));
    if (pWbInfo == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_TSK_LOG "Fail to get wb info - wb info should not null!" PRN_CAL_TSK_NG

        AmbaMisra_TouchUnused(pInfo);
        AmbaMisra_TouchUnused(&CalID);
        AmbaMisra_TouchUnused(&CalcCmd);
    } else {
        RetVal = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_CFA_AAA_RDY, SvcCalibTask_CfaDataRdyHdlr);
        if (RetVal != 0U) {
            PRN_CAL_TSK_LOG "Fail to get wb info - register CFA data rdy! ErrCode(0x%08X)"
            PRN_CAL_TSK_ARG_UINT32 RetVal PRN_CAL_TSK_ARG_POST
            PRN_CAL_TSK_NG
        } else {
#ifdef CONFIG_BUILD_IMGFRW_AAA
            WB_Detect_Info_s CurWbInfo;
            UINT32 Idx;
            UINT32 ImgProcAlgoID[AMBA_DSP_MAX_VIN_TD_NUM], ImgProcAlgoNum = 0U;

            AmbaSvcWrap_MisraMemset(ImgProcAlgoID, 255, sizeof(ImgProcAlgoID));

            SvcCalibTask_ImgAlgoIDGet(&(pWbInfo->CalibChan), ImgProcAlgoID, &ImgProcAlgoNum);

            for (Idx = 0U; Idx < ImgProcAlgoNum; Idx ++) {
                if (ImgProcAlgoID[Idx] == 0xFFFFFFFFU) {
                    RetVal |= SVC_NG;
                    PRN_CAL_TSK_LOG "Fail to get wb info - invalid ImgProcAlgoID(0x%x)"
                        PRN_CAL_TSK_ARG_UINT32 ImgProcAlgoID[Idx] PRN_CAL_TSK_ARG_POST
                    PRN_CAL_TSK_NG
                } else {

                    PRetVal = ImgProc_WbCalibSetEnable(ImgProcAlgoID[Idx], 1U); PRN_CAL_TSK_ERR_HDLR

                    AmbaSvcWrap_MisraMemset(&CurWbInfo, 0, sizeof(CurWbInfo));
                    if ( 0U != ImgProc_WBCal(ImgProcAlgoID[Idx], pWbInfo->WbIndex, pWbInfo->FlickerMode, &CurWbInfo) ) {
                        RetVal |= SVC_NG;
                        PRN_CAL_TSK_LOG "Fail to get ImgProcAlgoID(%d) wb info - get current gain fail! Index: %d"
                            PRN_CAL_TSK_ARG_UINT32 ImgProcAlgoID[Idx] PRN_CAL_TSK_ARG_POST
                            PRN_CAL_TSK_ARG_UINT32 pWbInfo->WbIndex   PRN_CAL_TSK_ARG_POST
                        PRN_CAL_TSK_NG
                    } else {
                        pWbInfo->CurGain.GainR = CurWbInfo.CurR;
                        pWbInfo->CurGain.GainG = CurWbInfo.CurG;
                        pWbInfo->CurGain.GainB = CurWbInfo.CurB;
                    }

                    PRetVal = ImgProc_WbCalibSetEnable(ImgProcAlgoID[Idx], 0U); PRN_CAL_TSK_ERR_HDLR
                }
            }
#else
            SvcCalibTask_ImgAlgoIDGet(NULL, NULL, NULL);
#endif
            PRetVal = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_LV_CFA_AAA_RDY, SvcCalibTask_CfaDataRdyHdlr); PRN_CAL_TSK_ERR_HDLR
        }
    }

    return RetVal;
}


static UINT32 SvcCalibTask_CalcCmdMemLock(UINT32 CalID, UINT32 CalcCmd, void *pInfo)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (CalcCmd == SVC_CALIB_CALC_MEM_LOCK) {
        SVC_CALIB_CALC_MEM_INFO_s *pMemInfo = NULL;
        AmbaMisra_TypeCast(&(pMemInfo), &(pInfo));

        if (pMemInfo == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_TSK_LOG "Fail to lock memory - memory info should not null!" PRN_CAL_TSK_NG
        } else if (pMemInfo->ReqMemNum == 0U) {
            RetVal = SVC_NG;
            PRN_CAL_TSK_LOG "Fail to lock memory - request memory number should not zero!" PRN_CAL_TSK_NG
        } else {
            ULONG  FreeBase[SVC_MEM_TYPE_MAX];
            UINT32 FreeSize[SVC_MEM_TYPE_MAX];
            UINT32 MemIdx, CurReqNum = 0U, ChkSize, MaxMemId;
            UINT8 *pMemBuf;

            AmbaSvcWrap_MisraMemset(FreeBase, 0, sizeof(FreeBase));
            AmbaSvcWrap_MisraMemset(FreeSize, 0, sizeof(FreeSize));
            for (MemIdx = 0U; MemIdx < SVC_MEM_TYPE_MAX; MemIdx ++) {
                if ((MemIdx == SVC_MEM_TYPE_CV)
#ifdef SVC_MEM_TYPE_IO
                  ||(MemIdx == SVC_MEM_TYPE_IO)
#endif
                ) {
                    continue;
                }
                if (0U != SvcBuffer_LockFreeSpace(MemIdx, &(FreeBase[MemIdx]), &(FreeSize[MemIdx]))) {
                    FreeBase[MemIdx] = 0U;
                    FreeSize[MemIdx] = 0U;
                }
                PRetVal = SvcBuffer_UnLockFreeSpace(MemIdx); PRN_CAL_TSK_ERR_HDLR
            }
#ifdef CONFIG_SOC_CV2FS
            {
                SVC_APP_STAT_DSP_BOOT_s BootStatus;

                AmbaSvcWrap_MisraMemset(&BootStatus, 0, sizeof(BootStatus));
                RetVal = SvcSysStat_Get(SVC_APP_STAT_DSP_BOOT, &BootStatus);
                if (SVC_OK == RetVal) {
                    if (BootStatus.Status != SVC_APP_STAT_DSP_BOOT_NOT_READY) {
                        FreeBase[SVC_MEM_TYPE_NC] = 0U;
                        FreeSize[SVC_MEM_TYPE_NC] = 0U;
                    }
                }
            }
#endif
            while ((CurReqNum < pMemInfo->ReqMemNum) && (CurReqNum < SVC_CALIB_CALC_MEM_MAX_NUM)) {

                ChkSize = 0U;
                MaxMemId = 255U;

                for (MemIdx = 0U; MemIdx < SVC_MEM_TYPE_MAX; MemIdx ++) {
                    if (FreeSize[MemIdx] > ChkSize) {
                        MaxMemId = MemIdx;
                    }
                }

                if (MaxMemId >= SVC_MEM_TYPE_MAX) {
                    break;
                } else {
                    AmbaMisra_TypeCast(&(pMemBuf), &(FreeBase[MaxMemId]));
                    pMemInfo->pReqMem[CurReqNum]    = pMemBuf;
                    pMemInfo->ReqMemSize[CurReqNum] = FreeSize[MaxMemId];
                    pMemInfo->ReqMemType[CurReqNum] = MaxMemId;

                    FreeSize[MaxMemId] = 0U;
                }

                CurReqNum ++;
            }
        }

    } else if (CalcCmd == SVC_CALIB_CALC_MEM_UNLOCK) {
        const SVC_CALIB_CALC_MEM_INFO_s *pMemInfo = NULL;
        AmbaMisra_TypeCast(&(pMemInfo), &(pInfo));

        if (pMemInfo != NULL) {
            UINT32 Idx;

            for (Idx = 0U; Idx < SVC_CALIB_CALC_MEM_MAX_NUM; Idx ++) {
                if (pMemInfo->ReqMemSize[Idx] > 0U) {
                    PRetVal = SvcBuffer_UnLockFreeSpace(pMemInfo->ReqMemType[Idx]); PRN_CAL_TSK_ERR_HDLR
                }
            }
        }

        PRN_CAL_TSK_LOG "Success to un-lock memory" PRN_CAL_TSK_API
    } else {
        RetVal = SVC_NG;
        PRN_CAL_TSK_LOG "Fail to lock memory - wrong calc command!" PRN_CAL_TSK_NG
        AmbaMisra_TouchUnused(&CalID);
        AmbaMisra_TouchUnused(pInfo);
    }

    return RetVal;
}

static UINT32 SvcCalibTask_CalcCmdStgDriver(UINT32 CalID, UINT32 CalcCmd, void *pInfo)
{
    UINT32 RetVal = SVC_OK;

    if (pInfo == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_TSK_LOG "Fail to get main storage driver - output info should not null!" PRN_CAL_TSK_NG
    } else if (CalcCmd != SVC_CALIB_CALC_STG_DRIVER) {
        RetVal = SVC_NG;
        PRN_CAL_TSK_LOG "Fail to get main storage driver - only support CalcCmd(%d)"
        PRN_CAL_TSK_ARG_UINT32 CalcCmd PRN_CAL_TSK_ARG_POST
        PRN_CAL_TSK_NG
    } else {
        SVC_USER_PREF_s *pSvcUserPref = NULL;
        SVC_CALIB_CALC_STORAGE_DRIVER_s *pCurInfo = NULL;

        AmbaMisra_TypeCast(&pCurInfo, &pInfo);
        if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
            RetVal = SVC_NG;
            PRN_CAL_TSK_LOG "Fail to get main storage driver - user pref should not null" PRN_CAL_TSK_NG
        } else {
            pCurInfo->MainStgDriver = pSvcUserPref->MainStgDrive[0];
        }

        AmbaMisra_TouchUnused(&CalID);
        AmbaMisra_TouchUnused(pInfo);
    }

    return RetVal;
}

static UINT32 SvcCalibTask_CalcCmdFunc(UINT32 CalID, UINT32 CalcCmd, void *pInfo)
{
    UINT32 RetVal = SVC_NG;
    UINT32 CmdIdx, CmdCnt = (UINT32)(sizeof(CalibCalcCmdHdlr)) / (UINT32)(sizeof(CalibCalcCmdHdlr[0]));

    for (CmdIdx = 0U; CmdIdx < CmdCnt; CmdIdx ++) {
        if (CalibCalcCmdHdlr[CmdIdx].CmdHdlr != NULL) {
            if (CalibCalcCmdHdlr[CmdIdx].CmdID == CalcCmd) {
                RetVal = (CalibCalcCmdHdlr[CmdIdx].CmdHdlr)(CalID, CalcCmd, pInfo);
            }
        }
    }

    return RetVal;
}

/**
 * Query calib task memory
 *
 * @param [out] pMemSize Requested memory size
 * @param [out] pCalcMemSize Requested calculation memory size
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcCalibTask_MemQry(UINT32 *pShadowMemSize, UINT32 *pWorkMemSize, UINT32 *pCalcMemSize)
{
    UINT32 RetVal = SVC_OK;
    UINT32 ReqShadowMemSize = 0U;
    UINT32 ReqWorkMemSize   = 0U;
    UINT32 ReqCalcMemSize   = 0U;
    const SVC_CALIB_CFG_s *pCalibCfg = SvcCalibCfg_Get();

    if (pCalibCfg != NULL) {
        UINT32 ObjIdx;
        SVC_CALIB_CFG_s CalibMemQryCfg;
        SVC_CALIB_OBJ_s *pCalObj;
        UINT32 OriShadowSize;
        UINT32 OriWorkingSize;

        AmbaSvcWrap_MisraMemset(&CalibMemQryCfg, 0, sizeof(CalibMemQryCfg));
        CalibMemQryCfg.NumOfCalObj = 1U;

        for (ObjIdx = 0U; ObjIdx < pCalibCfg->NumOfCalObj; ObjIdx ++) {

            pCalObj = &(pCalibCfg->pCalObj[ObjIdx]);

            if (pCalObj->Enable > 0U) {
                if (pCalObj->CmdFunc != NULL) {
                    OriShadowSize  = pCalObj->ShadowBufSize;
                    OriWorkingSize = pCalObj->WorkingBufSize;

                    CalibMemQryCfg.pCalObj = pCalObj;

                    if (0U != SvcCalib_MemQry(&CalibMemQryCfg)) {
                        pCalObj->ShadowBufSize = 0U;
                        pCalObj->WorkingBufSize = 0U;
                    }

                    if (OriShadowSize > 0U) {
                        pCalObj->ShadowBufSize = OriShadowSize;
                    }

                    if (OriWorkingSize > 0U) {
                        pCalObj->WorkingBufSize = OriWorkingSize;
                    }
                }

                ReqShadowMemSize += pCalObj->ShadowBufSize;
                ReqWorkMemSize   += pCalObj->WorkingBufSize;
                ReqCalcMemSize   += pCalObj->CalcBufSize;
            }
        }
    }

    if (pShadowMemSize != NULL) {
        *pShadowMemSize = ReqShadowMemSize;
    }

    if (pWorkMemSize != NULL) {
        *pWorkMemSize = ReqWorkMemSize;
    }

    if (pCalcMemSize != NULL) {
        *pCalcMemSize = ReqCalcMemSize;
    }

    return RetVal;
}

/**
 * Initial calib task
 *
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcCalibTask_Init(void)
{
    UINT32 Rval;
    ULONG  ShadowBufBase = 0U;
    UINT32 ShadowBufSize = 0U;
    UINT8 *pShadowBuf = NULL;
    ULONG  WorkBufBase = 0U;
    UINT32 WorkBufSize = 0U;
    UINT8 *pWorkBuf = NULL;
    SVC_CALIB_CFG_s *pCalibCfg = SvcCalibCfg_Get();

    PRN_CAL_TSK_LOG "Calib init" PRN_CAL_TSK_API

    SvcCalibTask_CalcCmdInit();

    Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_CALIB, &ShadowBufBase, &ShadowBufSize);
    if (Rval != SVC_OK) {
        PRN_CAL_TSK_LOG "Get calib shadow buffer fail!" PRN_CAL_TSK_NG
    } else {
        AmbaMisra_TypeCast(&(pShadowBuf), &(ShadowBufBase));
    }

    Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_CALIB_IK, &WorkBufBase, &WorkBufSize);
    if (Rval != SVC_OK) {
        PRN_CAL_TSK_LOG "Get calib working buffer fail!" PRN_CAL_TSK_NG
    } else {
        AmbaMisra_TypeCast(&(pWorkBuf), &(WorkBufBase));
    }

    if ((pShadowBuf == NULL) || (ShadowBufSize == 0U)) {
        PRN_CAL_TSK_LOG "Invalid shadow buffer setting Base %p Size %d"
            PRN_CAL_TSK_ARG_CPOINT pShadowBuf    PRN_CAL_TSK_ARG_POST
            PRN_CAL_TSK_ARG_UINT32 ShadowBufSize PRN_CAL_TSK_ARG_POST
        PRN_CAL_TSK_NG
    } else if ((pWorkBuf == NULL) || (WorkBufSize == 0U)) {
        PRN_CAL_TSK_LOG "Invalid wokring buffer setting Base %p Size %d"
            PRN_CAL_TSK_ARG_CPOINT pWorkBuf    PRN_CAL_TSK_ARG_POST
            PRN_CAL_TSK_ARG_UINT32 WorkBufSize PRN_CAL_TSK_ARG_POST
        PRN_CAL_TSK_NG
    } else if (pCalibCfg == NULL) {
        PRN_CAL_TSK_LOG "Get calib config fail!" PRN_CAL_TSK_NG
    } else {
        UINT32 Idx, ReqShadowMemSize = 0U, ReqWorkMemSize = 0U;

        for (Idx = 0U; Idx < pCalibCfg->NumOfCalObj; Idx ++) {
            ReqShadowMemSize += pCalibCfg->pCalObj[Idx].ShadowBufSize;
            ReqWorkMemSize   += pCalibCfg->pCalObj[Idx].WorkingBufSize;
        }

        if ((ReqShadowMemSize > 0U) && (ReqShadowMemSize <= ShadowBufSize) &&
            (ReqWorkMemSize > 0U)   && (ReqWorkMemSize <= WorkBufSize)) {
            UINT32 ShadowBufOfs = 0U;
            UINT32 WorkBufOfs   = 0U;

            for (Idx = 0U; Idx < pCalibCfg->NumOfCalObj; Idx ++) {
                pCalibCfg->pCalObj[Idx].pShadowBuf = &(pShadowBuf[ShadowBufOfs]);
                ShadowBufOfs += pCalibCfg->pCalObj[Idx].ShadowBufSize;
                pCalibCfg->pCalObj[Idx].pWorkingBuf = &(pWorkBuf[WorkBufOfs]);
                WorkBufOfs += pCalibCfg->pCalObj[Idx].WorkingBufSize;
            }

            pCalibCfg->pCalcBuf    = NULL;
            pCalibCfg->CalcBufSize = 0U;
            pCalibCfg->CalcCbFunc  = SvcCalibTask_CalcCmdFunc;

            Rval = SvcCalib_Create(pCalibCfg);
            if (Rval != SVC_OK) {
                PRN_CAL_TSK_LOG "Fail to create calibration manager!" PRN_CAL_TSK_NG
            } else {
                PRN_CAL_TSK_LOG "Success to crate calibration manager!" PRN_CAL_TSK_OK
            }
        } else {
            PRN_CAL_TSK_LOG "Invalid buffer setting" PRN_CAL_TSK_NG
        }
    }

    SvcCalitTask_CmdInstall();
    PRN_CAL_TSK_LOG "Calib init done" PRN_CAL_TSK_API


    return Rval;
}

/**
 * Update calibration table
 *
 * @param [in] CalibUpdBits Calibration update bits
 * @param [in] pChan Calibration channel
 * @param [in] pImgMode Image Kernel config
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcCalibTask_UpdateTable(UINT32 CalibUpdBits, SVC_CALIB_CHANNEL_s *pChan, const AMBA_IK_MODE_CFG_s *pImgMode)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    const SVC_CALIB_CFG_s *pCfg = SvcCalibCfg_Get();

    if (pChan == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_TSK_LOG "Fail to update table - invalid calib channel" PRN_CAL_TSK_NG
    } else if (pImgMode == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_TSK_LOG "Fail to update table - invalid image kernel cfg" PRN_CAL_TSK_NG
    } else if (pCfg == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_TSK_LOG "Fail to update table - invalid calib config" PRN_CAL_TSK_NG
    } else {
        UINT32 Idx, CalID;

        for (Idx = 0U; Idx < pCfg->NumOfCalObj; Idx ++) {

            if ( pCfg->pCalObj[Idx].Enable > 0U ) {

                CalID = pCfg->pCalObj[Idx].ID;

                if (CalID < SVC_CALIB_MAX_OBJ_NUM) {
                    if ((CalibUpdBits & SvcCalib_BitGet(CalID)) > 0U) {

                        if ( CalID == SVC_CALIB_WB_ID ) {
                            PRetVal = SvcCalibTask_WbUpdate(pChan); PRN_CAL_TSK_ERR_HDLR
                        } else {

                            if (CalID == SVC_CALIB_LDC_ID) {
                                if ((CalibUpdBits & SVC_CALIB_LDC_UPD_FMT_FOV) > 0U) {
                                    PRetVal = SvcCalib_ItemTableCfg(CalID, SVC_CALIB_LDC_UPD_BY_FMT_FOV, NULL); PRN_CAL_TSK_ERR_HDLR
                                } else {
                                    PRetVal = SvcCalib_ItemTableCfg(CalID, SVC_CALIB_LDC_UPD_BY_VIN_SS, NULL); PRN_CAL_TSK_ERR_HDLR
                                }
                            }

                            PRetVal = SvcCalib_ItemTableUpdate( CalID, pChan, pImgMode ); PRN_CAL_TSK_ERR_HDLR
                        }

                    }
                }
            }

        }
    }

    return RetVal;
}

/**
 * Start calibration task
 *
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcCalibTask_Start(void)
{
    UINT32 Rval, PRetVal;

    PRN_CAL_TSK_LOG "Calib update start" PRN_CAL_TSK_API

#ifdef CONFIG_ICAM_EEPROM_USED
    if (0U != SvcCalibTask_ModuleInit()) {
        PRN_CAL_TSK_LOG "Fail to init calib module" PRN_CAL_TSK_NG
    }
#endif

    /* Calibration item initial */
    Rval = SvcCalib_ItemDataInitAll();

    /* Configure calibration lookup table */
    SvcCalibTask_CalibChTblCfg();

    if (Rval == SVC_OK) {
        const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();

        if (pCfg != NULL) {
            AMBA_IK_MODE_CFG_s  ImgMode;
            SVC_CALIB_CHANNEL_s CalibChan;
            UINT32 FovIdx, VinID, SerDesIdx, FovSrc;
            UINT32 CalUpdBits;

            for (FovIdx = 0U; FovIdx < pCfg->FovNum; FovIdx ++) {

                FovSrc = 0xFFU;
                PRetVal = SvcResCfg_GetFovSrc(FovIdx, &FovSrc); PRN_CAL_TSK_ERR_HDLR

                /* Calibration works only when source is sensor(raw or yuv) */
                if ((FovSrc != SVC_VIN_SRC_SENSOR) && (FovSrc != SVC_VIN_SRC_YUV)) {
                    continue;
                }

                VinID = 0xdeadbeefU;
                PRetVal = SvcResCfg_GetVinIDOfFovIdx(FovIdx, &VinID); PRN_CAL_TSK_ERR_HDLR
                if (VinID >= AMBA_NUM_VIN_CHANNEL) {
                    Rval = SVC_NG;
                    PRN_CAL_TSK_LOG "Fail to update calib table - Get invalid VinID(%d) by FovIdx(%d)"
                        PRN_CAL_TSK_ARG_UINT32 VinID  PRN_CAL_TSK_ARG_POST
                        PRN_CAL_TSK_ARG_UINT32 FovIdx PRN_CAL_TSK_ARG_POST
                    PRN_CAL_TSK_NG
                } else {
                    if (pCfg->VinCfg[VinID].SerdesType == SVC_RES_SERDES_TYPE_NONE) {
                        SerDesIdx = 0U;
                    } else {
                        SerDesIdx = 0xdeadbeefU;
                        PRetVal = SvcResCfg_GetSerdesIdxOfFovIdx(FovIdx, &SerDesIdx); PRN_CAL_TSK_ERR_HDLR
                        if (SerDesIdx >= 4U) {
                            Rval = SVC_NG;
                            PRN_CAL_TSK_LOG "Fail to update calib table - Get invalid SerDesIdx(%d) by FovIdx(%d)"
                                PRN_CAL_TSK_ARG_UINT32 SerDesIdx PRN_CAL_TSK_ARG_POST
                                PRN_CAL_TSK_ARG_UINT32 FovIdx    PRN_CAL_TSK_ARG_POST
                            PRN_CAL_TSK_NG
                        }
                    }

                    if (Rval == 0U) {
                        CalUpdBits = pCfg->FovCfg[FovIdx].CalUpdBits;
                        if (CalUpdBits == 0U) {
                            CalUpdBits = (UINT32)SVC_CALIB_DEF_UPDATE_FLAG;
                        }

                        AmbaSvcWrap_MisraMemset(&ImgMode, 0, sizeof(ImgMode));
                        ImgMode.ContextId = FovIdx;

                        AmbaSvcWrap_MisraMemset(&CalibChan, 0, sizeof(CalibChan));
                        CalibChan.VinID            = VinID;
                        CalibChan.VinSelectBits    = SvcCalib_BitGet(VinID);
                        CalibChan.SensorID         = SvcCalib_BitGet(SerDesIdx);
                        CalibChan.SensorSelectBits = SvcCalib_BitGet(SerDesIdx);
                        CalibChan.FmtID            = SvcCalibTask_GetCurFmtID();
                        CalibChan.FovID            = FovIdx;

                        SvcCalibTask_ExtendDataGet(VinID, FovIdx, &(CalibChan.ExtendData));

#ifdef CONFIG_ICAM_EEPROM_USED
                        if (0U == SvcCalibTask_ModuleCreate(FovIdx)) {
                            if (0U != SvcCalibTask_ModuleUpdate(FovIdx, &CalibChan)) {
                                PRN_CAL_TSK_LOG "Fail to update calib module, VinID(%d) FovIdx(%d)"
                                    PRN_CAL_TSK_ARG_UINT32 VinID  PRN_CAL_TSK_ARG_POST
                                    PRN_CAL_TSK_ARG_UINT32 FovIdx PRN_CAL_TSK_ARG_POST
                                PRN_CAL_TSK_NG
                            }
                        }
#endif

                        PRetVal = SvcCalibTask_UpdateTable(CalUpdBits, &CalibChan, &ImgMode); PRN_CAL_TSK_ERR_HDLR
                    }
                }
            }
        }
    }

    PRN_CAL_TSK_LOG "Calib update done" PRN_CAL_TSK_API

    return Rval;
}

/**
 * Stop calibration task
 *
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcCalibTask_Stop(void)
{
    UINT32 Rval = SVC_OK;

    return Rval;
}
