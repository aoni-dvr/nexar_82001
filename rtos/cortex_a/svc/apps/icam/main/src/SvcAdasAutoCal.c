/**
 *  @file SvcAdasAutoCal.c
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
 *  @details Svc auto calibration flow
 *
 */

#include "AmbaTypes.h"
#include "AmbaUtility.h"

/* ssp */
#include "AmbaDSP_Capability.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaCalib_EmirrorIF.h"
#include "AmbaCalib_AVMIF.h"
#include "AmbaMisraFix.h"
#include "AmbaRTSL_ORC.h"
#include "AmbaFS.h"
#include "AmbaGDMA_Def.h"
#include "AmbaGDMA.h"
#include "AmbaSYS_Def.h"
#include "AmbaSYS.h"
#include "AmbaDef.h"
#include "AmbaAP_LKA.h"
#include "AmbaSvcWrap.h"
#include "AmbaCalib_Def.h"

#if defined(CONFIG_BUILD_COMMON_TUNE_CTUNER)
#include "AmbaCT_EmirTunerIF.h"
#endif

/* svc-framework */
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcCvCamCtrl.h"
#include "SvcTask.h"
/* svc-shared */
#include "SvcResCfg.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcBufMap.h"
#include "AmbaShell.h"

/* svc-icam */
#include "SvcUserPref.h"
#include "SvcCvAppDef.h"
#include "SvcCalibAdas.h"
#include "AmbaSR_Lane.h"
#include "RefFlow_FC.h"
#include "RefFlow_AutoCal.h"
#include "SvcAdasAutoCal.h"
#include "SvcAdasNotify.h"

#define AUTO_CAL_DBG 0

#define SVC_LOG_AUTOCAL_TASK     "AUTOCAL_TASK"
#define SVC_AUTO_CAL_TASK_STACK_SIZE (0x1000000)

static AMBA_KAL_MSG_QUEUE_t    SvcAutoCalFreeQueId GNU_SECTION_NOZEROINIT;
static AMBA_KAL_MSG_QUEUE_t    SvcAutoCalDataQueId GNU_SECTION_NOZEROINIT;
static UINT8 *g_WrkBufAddr GNU_SECTION_NOZEROINIT;

static AMBA_AUTOCAL_HDLR_CFG_s g_AdasAutoCalHdlr[SVC_ADAS_AUTOCAL_MSG_DEPTH] GNU_SECTION_NOZEROINIT;

/**
* PrintDouble message (Use %s tp print)
* @param [in] string format
* @param [in] argument1
* @param [in] argument2
* @param [in] argument3
* @param [in] argument4
* @param [in] argument5
* @param [in] how many number print after decimal point
*/
void SvcAdasAutoCal_PrintDouble5(const char* pStrFmt, DOUBLE Arg1, DOUBLE Arg2, DOUBLE Arg3, DOUBLE Arg4, DOUBLE Arg5, UINT32 AfterPoint)
{
    char Str[5U][128U];
    DOUBLE Val[5U];

    Val[0] = Arg1;
    Val[1] = Arg2;
    Val[2] = Arg3;
    Val[3] = Arg4;
    Val[4] = Arg5;

    for (UINT32 Idx = 0U; Idx < 5U; Idx++) {
        if (0U == AmbaUtility_DoubleToStr(Str[Idx], 128U, Val[Idx], AfterPoint)){//Return string length
            SvcLog_NG(SVC_LOG_AUTOCAL_TASK, "AmbaUtility_DoubleToStr failed", 0U, 0U);
        }
    }

    AmbaPrint_PrintStr5(pStrFmt, Str[0U], Str[1U], Str[2U], Str[3U], Str[4U]);
}

static void* SvcAdasAutoCalTaskEntry(void* EntryArg)
{
    UINT32 Rval = SVC_OK;
    SVC_ADAS_AUTOCAL_MSG_s AutoCalMsg;
    SVC_ADAS_AUTOCAL_LD_INFO_s *pAutoCalData;
    static AMBA_AUTOCAL_HDLR_OUT_s AdasAutoCalOutput;
    static UINT32 FirstAutoCalDone = 0U;

    AmbaMisra_TouchUnused(EntryArg);
	AmbaSvcWrap_MisraMemset(&AutoCalMsg, 0, sizeof(SVC_ADAS_AUTOCAL_MSG_s));
    while (Rval == SVC_OK) {
        Rval = AmbaKAL_MsgQueueReceive(&SvcAutoCalDataQueId, &AutoCalMsg, AMBA_KAL_WAIT_FOREVER);
        if (SVC_NG == Rval) {
            SvcLog_NG(SVC_LOG_AUTOCAL_TASK, "SvcAutoCalDataQueId MsgQue Receive error !", 0U, 0U);
        } else {
            AmbaMisra_TypeCast(&pAutoCalData, &(AutoCalMsg.DataBufferAddr));
            g_AdasAutoCalHdlr[AutoCalMsg.BufferIdx].pLaneDetectionData = &pAutoCalData->LaneDetectionData;
            g_AdasAutoCalHdlr[AutoCalMsg.BufferIdx].pCanbusData = &pAutoCalData->CanbusData;
            if (SVC_OK != AmbaAutoCal_RefHandler(&g_AdasAutoCalHdlr[AutoCalMsg.BufferIdx], g_WrkBufAddr, &AdasAutoCalOutput)){
                SvcLog_NG(SVC_LOG_AUTOCAL_TASK, "AmbaAutoCal_RefHandler failed", 0U, 0U);
            }
#if (1 == AUTO_CAL_DBG)
            SvcAdasAutoCal_PrintDouble5("@@ DataCollectRatio = %s, Status = %s",
                                         AdasAutoCalOutput.DataCollectRatio,
                                         (DOUBLE)AdasAutoCalOutput.Status, 0.0, 0.0, 0.0, 6U);
#endif


            if (FirstAutoCalDone == 0U) {
                SvcAdasNotify_UpdateAutoCalDataCollectRatio(AdasAutoCalOutput.DataCollectRatio, AdasAutoCalOutput.Status);
            }


            //Setup Auto calibration info if AmbaAutoCal_RefHandler() get success message.
            if (AUTOCAL_HDLR_CALIB_SUCCEED == AdasAutoCalOutput.Status) {
                static AMBA_CAL_EM_CALC_COORD_CFG_s Cfg;
                static AMBA_CAL_EM_CAM_CALIB_DATA_s CalibDataRaw2World;
                static UINT32 PrintAutoCalibSuccessMsg = 0U;

                if (0U == PrintAutoCalibSuccessMsg) {
                    SvcLog_OK(SVC_LOG_AUTOCAL_TASK, "Update Auto Calibration start", 0U, 0U);
                }

                Cfg.TransCfg.pCalibDataRaw2World = &CalibDataRaw2World;
                if(SVC_OK != SvcCalib_AdasCfgGet(SVC_CALIB_ADAS_TYPE_FRONT, &Cfg)) {
                    SvcLog_NG(SVC_LOG_AUTOCAL_TASK, "Auto calib SvcCalib_AdasCfgGet fail !", 0U, 0U);
                }
                AmbaSvcWrap_MisraMemcpy(Cfg.TransCfg.pCalibDataRaw2World, &AdasAutoCalOutput.AutoCalibInfo.CalibData, sizeof(AMBA_CAL_EM_CAM_CALIB_DATA_s));

                SvcAdasAutoCal_SetupCurrCalInfo(&AdasAutoCalOutput.AutoCalibInfo);
                if(SVC_OK != RefFlowFC_UpdateCalibData(Cfg.TransCfg.pCalibDataRaw2World)) {
                    SvcLog_NG(SVC_LOG_AUTOCAL_TASK, "RefFlowFC_UpdateCalibData fail !", 0U, 0U);
                }
                if(SVC_OK != AmbaSR_Lane_SetConfig(AMBA_SR_LANE_CFG_CALIBRATION, &Cfg)) {
                    SvcLog_NG(SVC_LOG_AUTOCAL_TASK, "AmbaSR_Lane_SetConfig fail !", 0U, 0U);
                }

                if (0U == PrintAutoCalibSuccessMsg) {
                    SvcLog_OK(SVC_LOG_AUTOCAL_TASK, "Update Auto Calibration done", 0U, 0U);
                    PrintAutoCalibSuccessMsg = 1U;
                    SvcAdasNotify_UpdateAutoCalDataCollectRatio(100.0, AdasAutoCalOutput.Status);
                    FirstAutoCalDone = 2U;
                }
            } else if ((FirstAutoCalDone == 1U) && (AUTOCAL_HDLR_CALIB_FAIL == AdasAutoCalOutput.Status)) {
                FirstAutoCalDone = 2U;
                SvcLog_OK(SVC_LOG_AUTOCAL_TASK, "Update Auto Calibration fail!", 0U, 0U);
                SvcAdasNotify_UpdateAutoCalDataCollectRatio(100.0, AdasAutoCalOutput.Status);
            } else {
                //Do nothing
            }

            //If DataCollectRatio become 100%, auto calibration will be launched in next round
            if ((FirstAutoCalDone == 0U) && (AdasAutoCalOutput.DataCollectRatio == 100.0)) {
                FirstAutoCalDone = 1U;
            }

            //Free data buffer size
            Rval = AmbaKAL_MsgQueueSend(&SvcAutoCalFreeQueId, &AutoCalMsg, AMBA_KAL_WAIT_FOREVER);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_AUTOCAL_TASK, "MsgQueueSend() SvcAutoCalFreeQueId failed", 0U, 0U);
            }

        }
    }

    return NULL;
}

/**
 * Initial Auto calibration
 * @return ErrorCode
 */
UINT32 SvcAdasAutoCalTask_Init(void)
{
    UINT32 RetVal = SVC_OK;
    SVC_USER_PREF_s  *pSvcUserPref;
    static SVC_TASK_CTRL_s     SvcAutoCalTaskCtrl GNU_SECTION_NOZEROINIT;
    static UINT8               SvcAutoCalTaskStack[SVC_AUTO_CAL_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    ULONG  CacheBase;
    UINT32 CacheSize = 0U;
    static char                QueueNameFree[] = "SvcAutoCalFreeQue";
    static char                QueueNameData[] = "SvcAutoCalDataQue";
    static SVC_ADAS_AUTOCAL_MSG_s SvcAutoCalFreeQue[SVC_ADAS_AUTOCAL_MSG_DEPTH] GNU_SECTION_NOZEROINIT;
    static SVC_ADAS_AUTOCAL_MSG_s SvcAutoCalDataQue[SVC_ADAS_AUTOCAL_MSG_DEPTH] GNU_SECTION_NOZEROINIT;
    SVC_ADAS_AUTOCAL_MSG_s SvcAutoCalInitInfo;

    if (SVC_OK != AmbaWrap_memset(&g_AdasAutoCalHdlr, 0, sizeof(AMBA_AUTOCAL_HDLR_CFG_s) * SVC_ADAS_AUTOCAL_MSG_DEPTH)) {
        SvcLog_NG(SVC_LOG_AUTOCAL_TASK, "SvcAdasAutoCalTask_Init(), AmbaWrap_memset() failed!!", 0U, 0U);
    }

    if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
        SvcLog_NG(SVC_LOG_AUTOCAL_TASK, "SvcUserPref_Get() failed!!", 0U, 0U);
    }
    SvcAdasAutoCal_SetupFocalLength(pSvcUserPref->CalibFocalLength);

    RetVal = SvcBuffer_Request(SVC_BUFFER_SHARED,
                               SMEM_PF0_ID_AUTO_CALIB,
                               &CacheBase,
                               &CacheSize);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_AUTOCAL_TASK, "SvcBuffer_Request fail (AUTO_CALIB)", 0U, 0U);
    }

    AmbaMisra_TypeCast(&g_WrkBufAddr, &(CacheBase));

    /* create SvcAutoCalFreeQueId msg queue */
    if (SVC_OK != AmbaKAL_MsgQueueCreate(&SvcAutoCalFreeQueId,
                                         QueueNameFree,
                                         sizeof(SVC_ADAS_AUTOCAL_MSG_s),
                                         SvcAutoCalFreeQue,
                                         SVC_ADAS_AUTOCAL_MSG_DEPTH * sizeof(SVC_ADAS_AUTOCAL_MSG_s))) {
        SvcLog_NG(SVC_LOG_AUTOCAL_TASK, "SvcAdasAutoCalTask_Init: Fail to create SvcAutoCalFreeQueId msg queue", 0U, 0U);
    }

    /* create SvcAutoCalDataQueId msg queue */
    if (SVC_OK != AmbaKAL_MsgQueueCreate(&SvcAutoCalDataQueId,
                                         QueueNameData,
                                         sizeof(SVC_ADAS_AUTOCAL_MSG_s),
                                         SvcAutoCalDataQue,
                                         SVC_ADAS_AUTOCAL_MSG_DEPTH * sizeof(SVC_ADAS_AUTOCAL_MSG_s))) {
        SvcLog_NG(SVC_LOG_AUTOCAL_TASK, "SvcAdasAutoCalTask_Init: Fail to create SvcAutoCalDataQueId msg queue", 0U, 0U);
    }

    /* Create ADAS Cal task */
    SvcAutoCalTaskCtrl.Priority   = 53U;
    SvcAutoCalTaskCtrl.EntryFunc  = SvcAdasAutoCalTaskEntry;
    SvcAutoCalTaskCtrl.pStackBase = SvcAutoCalTaskStack;
    SvcAutoCalTaskCtrl.StackSize  = SVC_AUTO_CAL_TASK_STACK_SIZE;
    SvcAutoCalTaskCtrl.CpuBits    = 0x1U;
    SvcAutoCalTaskCtrl.EntryArg   = 0x0U;


    RetVal = SvcTask_Create("SvcAdasAutoCalTask", &SvcAutoCalTaskCtrl);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_AUTOCAL_TASK, "SvcAdasAutoCalTask create fail", 0U, 0U);
    }


    /* Allocate/Assign Free queue  address */
    RetVal = SvcBuffer_Request(SVC_BUFFER_SHARED,
                               SMEM_PF0_ID_AUTO_CALIB_MSG,
                               &CacheBase,
                               &CacheSize);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_AUTOCAL_TASK, "SvcBuffer_Request fail (AUTO_CALIB_MSG)", 0U, 0U);
    }

    for (UINT32 i = 0U;i< SVC_ADAS_AUTOCAL_MSG_DEPTH;i++) {
        SvcAutoCalInitInfo.BufferIdx = i;
        SvcAutoCalInitInfo.DataBufferAddr = CacheBase;
        RetVal = AmbaKAL_MsgQueueSend(&SvcAutoCalFreeQueId, &SvcAutoCalInitInfo, AMBA_KAL_WAIT_FOREVER);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_AUTOCAL_TASK, "MsgQueueSend() SvcAutoCalFreeQueId failed", 0U, 0U);
        }
        CacheBase += sizeof(SVC_ADAS_AUTOCAL_LD_INFO_s);
    }

    return RetVal;
}

/**
 * Update AUTOCAL_LD_INFO
 * @param [in] AUTOCAL_LD_INFO info
 * @return ErrorCode
 */
UINT32 SvcAdasAutoCal_MsgSend(const SVC_ADAS_AUTOCAL_LD_INFO_s *pAdasAutoCalInput)
{
    UINT32 RetVal = SVC_OK;
    SVC_ADAS_AUTOCAL_MSG_s AutoCalMsg;
    SVC_ADAS_AUTOCAL_LD_INFO_s *pAutoCalData;

    RetVal = AmbaKAL_MsgQueueReceive(&SvcAutoCalFreeQueId, &AutoCalMsg, AMBA_KAL_WAIT_FOREVER);
    if (SVC_NG == RetVal) {
        SvcLog_NG(SVC_LOG_AUTOCAL_TASK, "SvcAutoCalFreeQueId MsgQue Receive error !", 0U, 0U);
    } else {
        //Assign Data to buffer
        AmbaMisra_TypeCast(&pAutoCalData, &(AutoCalMsg.DataBufferAddr));
        if (SVC_OK != AmbaWrap_memcpy(&pAutoCalData->LaneDetectionData, &pAdasAutoCalInput->LaneDetectionData, sizeof(AMBA_SR_LANE_RECONSTRUCT_INFO_s))){
            SvcLog_NG(SVC_LOG_AUTOCAL_TASK, "SvcAdasAutoCal_MsgSend() LaneDetectionData failed", 0U, 0U);
        }
        if (SVC_OK != AmbaWrap_memcpy(&pAutoCalData->CanbusData, &pAdasAutoCalInput->CanbusData, sizeof(AMBA_SR_CANBUS_TRANSFER_DATA_s))){
            SvcLog_NG(SVC_LOG_AUTOCAL_TASK, "SvcAdasAutoCal_MsgSend() CanbusData failed", 0U, 0U);
        }


        //Transfer data
        RetVal = AmbaKAL_MsgQueueSend(&SvcAutoCalDataQueId, &AutoCalMsg, AMBA_KAL_WAIT_FOREVER);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_AUTOCAL_TASK, "MsgQueueSend() SvcAutoCalDataQueId failed", 0U, 0U);
        }

    }

    return RetVal;
}

/**
 * Setup Cam Info
 * @param [in] Cam info
 */
void SvcAdasAutoCal_SetupCam(const AMBA_CAL_CAM_s *pCam)
{
    UINT32 RetVal = SVC_OK;

    SvcLog_OK(SVC_LOG_AUTOCAL_TASK, "SvcAdasAutoCal_SetupCam(), TableLen = %d", pCam->Lens.LensDistoSpec.pRealExpect->Length, 0U);
        for (UINT32 i = 0U; i< SVC_ADAS_AUTOCAL_MSG_DEPTH; i++) {
            RetVal = AmbaWrap_memcpy(&g_AdasAutoCalHdlr[i].Cam, pCam, sizeof(AMBA_CAL_CAM_s));
            if (SVC_OK != RetVal){
                SvcLog_NG(SVC_LOG_AUTOCAL_TASK, "SvcAdasAutoCal_SetupCam[%d] failed", i, 0U);
            } else {
                g_AdasAutoCalHdlr[i].Cam.Lens.LensDistoUnit = AMBA_CAL_LD_PIXEL;

#if (1 == AUTO_CAL_DBG)
                AmbaPrint_PrintUInt5("[%d] Cam Lens LensDistoType = %d, LensDistoUnit = %d",
                        i, (UINT32)g_AdasAutoCalHdlr[i].Cam.Lens.LensDistoType, (UINT32)g_AdasAutoCalHdlr[i].Cam.Lens.LensDistoUnit,
                        0U, 0U);

                for (UINT32 k = 0U; k< pCam->Lens.LensDistoSpec.pRealExpect->Length; k++) {
                    SvcAdasAutoCal_PrintDouble5("@@ Length = %s,  %s,  %s",
                            (DOUBLE)pCam->Lens.LensDistoSpec.pRealExpect->Length,
                            g_AdasAutoCalHdlr[i].Cam.Lens.LensDistoSpec.pRealExpect->pRealTbl[k],
                            g_AdasAutoCalHdlr[i].Cam.Lens.LensDistoSpec.pRealExpect->pExpectTbl[k],
                            0.0, 0.0, 4U);
                }

                SvcAdasAutoCal_PrintDouble5("Cam Sensor CellSize = %s, StartX = %s, StartY = %s, Width = %s, Height = %s",
                        g_AdasAutoCalHdlr[i].Cam.Sensor.CellSize,
                        (DOUBLE)g_AdasAutoCalHdlr[i].Cam.Sensor.StartX,
                        (DOUBLE)g_AdasAutoCalHdlr[i].Cam.Sensor.StartY,
                        (DOUBLE)g_AdasAutoCalHdlr[i].Cam.Sensor.Width,
                        (DOUBLE)g_AdasAutoCalHdlr[i].Cam.Sensor.Height, 5U);

                SvcAdasAutoCal_PrintDouble5("Cam OpticalCenter X = %s, Y = %s",
                        g_AdasAutoCalHdlr[i].Cam.OpticalCenter.X, g_AdasAutoCalHdlr[i].Cam.OpticalCenter.Y,
                        0.0, 0.0, 0.0, 5U);

                SvcAdasAutoCal_PrintDouble5("Cam Pos X = %s, Y = %s, Z = %s",
                        g_AdasAutoCalHdlr[i].Cam.Pos.X, g_AdasAutoCalHdlr[i].Cam.Pos.Y,
                        g_AdasAutoCalHdlr[i].Cam.Pos.Z,
                        0.0, 0.0, 5U);

                SvcAdasAutoCal_PrintDouble5("Cam Rotation = %s",
                        (DOUBLE)g_AdasAutoCalHdlr[i].Cam.Rotation,
                        0.0, 0.0, 0.0, 0.0, 5U);

                SvcAdasAutoCal_PrintDouble5("Cam FocalLength = %s",
                        g_AdasAutoCalHdlr[i].FocalLength,
                        0.0, 0.0, 0.0, 0.0, 5U);

                SvcAdasAutoCal_PrintDouble5("Cam RawRoi StartX = %s, StartY = %s, Width = %s, Height = %s",
                        (DOUBLE)g_AdasAutoCalHdlr[i].RawRoi.StartX,
                        (DOUBLE)g_AdasAutoCalHdlr[i].RawRoi.StartY,
                        (DOUBLE)g_AdasAutoCalHdlr[i].RawRoi.Width,
                        (DOUBLE)g_AdasAutoCalHdlr[i].RawRoi.Height, 0.0, 5U);

                SvcAdasAutoCal_PrintDouble5("Cam VoutView StartX = %s, StartY = %s, Width = %s, Height = %s",
                        (DOUBLE)g_AdasAutoCalHdlr[i].VoutView.StartX,
                        (DOUBLE)g_AdasAutoCalHdlr[i].VoutView.StartY,
                        (DOUBLE)g_AdasAutoCalHdlr[i].VoutView.Width,
                        (DOUBLE)g_AdasAutoCalHdlr[i].VoutView.Height, 0.0, 5U);

                SvcAdasAutoCal_PrintDouble5("Cam SegRoi StartX = %s, StartY = %s, Width = %s, Height = %s",
                        (DOUBLE)g_AdasAutoCalHdlr[i].SegRoi.StartX,
                        (DOUBLE)g_AdasAutoCalHdlr[i].SegRoi.StartY,
                        (DOUBLE)g_AdasAutoCalHdlr[i].SegRoi.Width,
                        (DOUBLE)g_AdasAutoCalHdlr[i].SegRoi.Height, 0.0, 5U);

                for (UINT32 l = 0U; l< EMIR_CALIB_POINT_NUM; l++) {
                    SvcAdasAutoCal_PrintDouble5("DefCalibPoints[%s].WorldPos X = %s, Y = %s, Z = %s",
                            (DOUBLE)l,
                            g_AdasAutoCalHdlr[i].DefCalibPoints[l].WorldPos.X,
                            g_AdasAutoCalHdlr[i].DefCalibPoints[l].WorldPos.Y,
                            g_AdasAutoCalHdlr[i].DefCalibPoints[l].WorldPos.Z,
                            0.0, 5U);

                    SvcAdasAutoCal_PrintDouble5("DefCalibPoints[%s].RawPos X = %s, Y = %s",
                            (DOUBLE)l,
                            g_AdasAutoCalHdlr[i].DefCalibPoints[l].RawPos.X,
                            g_AdasAutoCalHdlr[i].DefCalibPoints[l].RawPos.Y,
                            0.0,
                            0.0, 5U);
                }
#endif
            }
        }
}

/**
 * Setup FocalLength Info
 * @param [in] FocalLength info
 */
void SvcAdasAutoCal_SetupFocalLength(const DOUBLE FocalLength)
{
    SvcLog_OK(SVC_LOG_AUTOCAL_TASK, "SvcAdasAutoCal_SetupFocalLength()", 0U, 0U);
    for (UINT32 i = 0U;i< SVC_ADAS_AUTOCAL_MSG_DEPTH;i++) {
        g_AdasAutoCalHdlr[i].FocalLength = FocalLength;
    }
}

/**
 * Setup Roi Info
 * @param [in] Roi info
 */
void SvcAdasAutoCal_SetupRawRoi(const AMBA_CAL_ROI_s *pRawRoi)
{
    SvcLog_OK(SVC_LOG_AUTOCAL_TASK, "SvcAdasAutoCal_SetupRawRoi()", 0U, 0U);
    for (UINT32 i = 0U;i< SVC_ADAS_AUTOCAL_MSG_DEPTH;i++) {
        if (SVC_OK != AmbaWrap_memcpy(&g_AdasAutoCalHdlr[i].RawRoi, pRawRoi, sizeof(AMBA_CAL_ROI_s))){
            SvcLog_NG(SVC_LOG_AUTOCAL_TASK, "SvcAdasAutoCal_SetupRawRoi[%d] failed", i, 0U);
        }
    }
}

/**
 * Setup VoutView Info
 * @param [in] VoutView info
 */
void SvcAdasAutoCal_SetupVoutView(const AMBA_CAL_ROI_s *pVoutView)
{
    SvcLog_OK(SVC_LOG_AUTOCAL_TASK, "SvcAdasAutoCal_SetupVoutView()", 0U, 0U);
    for (UINT32 i = 0U;i< SVC_ADAS_AUTOCAL_MSG_DEPTH;i++) {
        if (SVC_OK != AmbaWrap_memcpy(&g_AdasAutoCalHdlr[i].VoutView, pVoutView, sizeof(AMBA_CAL_ROI_s))){
            SvcLog_NG(SVC_LOG_AUTOCAL_TASK, "SvcAdasAutoCal_SetupVoutView[%d] failed", i, 0U);
        }
    }
}

/**
 * Setup SegRoi Info
 * @param [in] SegRoi info
 */
void SvcAdasAutoCal_SetupSegRoi(const AMBA_CAL_ROI_s *pSegRoi)
{
    SvcLog_OK(SVC_LOG_AUTOCAL_TASK, "SvcAdasAutoCal_SetupSegRoi()", 0U, 0U);
    for (UINT32 i = 0U;i< SVC_ADAS_AUTOCAL_MSG_DEPTH;i++) {
        if (SVC_OK != AmbaWrap_memcpy(&g_AdasAutoCalHdlr[i].SegRoi, pSegRoi, sizeof(AMBA_CAL_ROI_s))){
            SvcLog_NG(SVC_LOG_AUTOCAL_TASK, "SvcAdasAutoCal_SetupSegRoi[%d] failed", i, 0U);
        }
    }
}

/**
 * Setup DefCalibPoints Info
 * @param [in] DefCalibPoints info
 */
void SvcAdasAutoCal_SetupDefCalibPoints(const AMBA_CT_EM_CALIB_POINTS_s *InCalibPoints)
{
    SvcLog_OK(SVC_LOG_AUTOCAL_TASK, "SvcAdasAutoCal_SetupDefCalibPoints()", 0U, 0U);
    for (UINT32 i = 0U;i< SVC_ADAS_AUTOCAL_MSG_DEPTH;i++) {
        for (UINT32 Idx = 0U; Idx < EMIR_CALIB_POINT_NUM; Idx++) {
            g_AdasAutoCalHdlr[i].DefCalibPoints[Idx].WorldPos.X = InCalibPoints->WorldPositionX[Idx];
            g_AdasAutoCalHdlr[i].DefCalibPoints[Idx].WorldPos.Y = InCalibPoints->WorldPositionY[Idx];
            g_AdasAutoCalHdlr[i].DefCalibPoints[Idx].WorldPos.Z = InCalibPoints->WorldPositionZ[Idx];
            g_AdasAutoCalHdlr[i].DefCalibPoints[Idx].RawPos.X = InCalibPoints->RawPositionX[Idx];
            g_AdasAutoCalHdlr[i].DefCalibPoints[Idx].RawPos.Y = InCalibPoints->RawPositionY[Idx];
        }
    }
}

/**
 * Setup CurrCalInfo Info
 * @param [in] CurrCalInfo info
 */
void SvcAdasAutoCal_SetupCurrCalInfo(const AMBA_AUTOCAL_CALIB_INFO_s *pCurrCalInfo)
{
    SvcLog_OK(SVC_LOG_AUTOCAL_TASK, "SvcAdasAutoCal_SetupCurrCalInfo()", 0U, 0U);
    for (UINT32 i = 0U;i< SVC_ADAS_AUTOCAL_MSG_DEPTH;i++) {
        if (SVC_OK != AmbaWrap_memcpy(&g_AdasAutoCalHdlr[i].CurrCalInfo, pCurrCalInfo, sizeof(AMBA_AUTOCAL_CALIB_INFO_s))){
            SvcLog_NG(SVC_LOG_AUTOCAL_TASK, "SvcAdasAutoCal_SetupCurrCalInfo[%d] failed", i, 0U);
        }
    }
}
