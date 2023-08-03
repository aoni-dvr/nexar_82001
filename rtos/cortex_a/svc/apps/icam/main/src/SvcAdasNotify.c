/**
*  @file SvcAdasNotify.c
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
*  @Svc Svc ADAS Notify
*
*/


#include "AmbaKAL.h"
#include "AmbaTypes.h"
#include "AmbaSYS.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaWS_FCWS.h"
#include "AmbaUtility.h"
#include "AmbaSurround.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaAP_LKA.h"
#include "SvcErrCode.h"
#include "SvcStgMonitor.h"
#include "SvcPref.h"
#include "AmbaShell.h"
#include "SvcTask.h"
#include "SvcCan.h"
#include "SvcOsd.h"
#include "SvcGui.h"
#include "SvcResCfg.h"
#include "AmbaCalib_AVMIF.h"
#include "SvcCalibAdas.h"
#include "SvcLog.h"
#include "SvcCvCamCtrl.h"
#include "SvcLdwsTask.h"
#include "SvcLdwsTaskV2.h"
#include "RefFlow_FC.h"
#include "SvcCvFlow_Comm.h"
#include "SvcFcwsFcmdTaskV2.h"
#include "SvcAdasNotify.h"
#include "SvcSegDrawTask.h"
#include "SvcWrap.h"
#include "SvcOsd.h"
#include "SvcUserPref.h"
#include "RefFlow_AutoCal.h"
#include "AmbaWrap.h"
#include "SvcCvAppDef.h"

#define HACK_FAKE_ENABLE 0
#define CALIB_Horizontal_Thickness      (3U)
#define SVC_FC_WARNINGZONE_OUTPUT_DEPTH (3U)

static AMBA_SR_CANBUS_TRANSFER_DATA_s SvcAdasDrawSpd_CanTraData GNU_SECTION_NOZEROINIT; // Canbus transfer data
static UINT32 IsDecodeMode = 0U; //0U: Not decode mode (Liveview mode); 1U: Decode mode
static DOUBLE DecodeModeSpeed = 0.0;
static UINT32 g_FcwsModeEnable = 0U;
static UINT32 g_QuickCalibZoneAdj = 0U;
static UINT32 g_FcwsMode GNU_SECTION_NOZEROINIT;
static UINT32 g_CalibHorizontalX GNU_SECTION_NOZEROINIT;
static UINT32 g_CalibHorizontalY GNU_SECTION_NOZEROINIT;
static UINT32 g_CalibEnable = 0U;// Not draw HorizontalY by default
static UINT32 g_WarningZoneEnable GNU_SECTION_NOZEROINIT;

static DOUBLE g_DataCollectRatio = 0.0;
static UINT32 g_AutoCalResult = (UINT32)AUTOCAL_HDLR_DATA_COLLECT;

static DOUBLE g_SvcAdasDrawSafetyDist   GNU_SECTION_NOZEROINIT; // Safety distance (UNIT: m)

static DOUBLE g_SvcAdasDrawTtcTime   GNU_SECTION_NOZEROINIT; // TTC time (UNIT: msec)

static DOUBLE g_SvcAdasDrawLdwsR   GNU_SECTION_NOZEROINIT;
static DOUBLE g_SvcAdasDrawLdwsL   GNU_SECTION_NOZEROINIT;

static DOUBLE g_RequiredSpeed = -1.0;
static DOUBLE g_Acceleration = 9999.0;

/* Init to a negative number */
static INT32 g_Zone1X[4] = {-1, -1, -1, -1}, g_Zone1Y[4] = {-1, -1, -1, -1};
static UINT32 AdasNotifyCalibFromNAND = 0U;
static AMBA_SR_LANE_MEMBER_s g_SubjectLaneInfo[SVC_FC_WARNINGZONE_OUTPUT_DEPTH] GNU_SECTION_NOZEROINIT;
static UINT32 g_SubjectLaneInfoIdx        GNU_SECTION_NOZEROINIT;
static AMBA_WS_FCWS_DATA_s g_WsFcwsCfg    GNU_SECTION_NOZEROINIT;
static DOUBLE g_FcTransferSpeed           GNU_SECTION_NOZEROINIT;
static SVC_CALIB_ADAS_INFO_GET_s *g_AdasNotifyCalibData = NULL;
static AMBA_SEG_CROP_INFO_s  g_SegCropInfo GNU_SECTION_NOZEROINIT;


static SvcADAS_NOTIFY_DRAW_s g_SpeedLayOut    GNU_SECTION_NOZEROINIT;
static SvcADAS_NOTIFY_DRAW_s g_FcInfoLayOut   GNU_SECTION_NOZEROINIT;
static SvcADAS_NOTIFY_DRAW_s g_DistRLayOut    GNU_SECTION_NOZEROINIT;
static SvcADAS_NOTIFY_DRAW_s g_DistLLayOut    GNU_SECTION_NOZEROINIT;
static SVC_CV_DISP_OSD_CMD_s g_HorizontalLine GNU_SECTION_NOZEROINIT;

#define ADAS_GUI_SPEED_UPDATE       (0x01U)
#define ADAS_GUI_DIST_UPDATE        (0x02U)
#define ADAS_GUI_FCWS_DIST_UPDATE   (0x04U)
#define ADAS_GUI_LDWS_DIST_R_UPDATE (0x10U)
#define ADAS_GUI_LDWS_DIST_L_UPDATE (0x20U)
#define ADAS_GUI_FCWS_TIME_UPDATE   (0x40U)
#define ADAS_GUI_QUICK_CALIB_ADJ    (0x80U)

#if defined(CONFIG_ICAM_32BITS_OSD_USED)
static UINT32 ADAS_StringColor = 0xffffffffU;
#else
static UINT32 ADAS_StringColor = 255U;
#endif

static UINT32 SvcAdasGuiUpdate = 0U;
static UINT32  AdasNotifyGuiLevel = SVC_NOTIFY_GUI_DRAW_MSG;

#if defined(CONFIG_ICAM_REBEL_USAGE)
static UINT32  g_AdasNotifyTargetVout = VOUT_IDX_A;
#else
static UINT32  g_AdasNotifyTargetVout = VOUT_IDX_B;
#endif

static UINT32 SvcAdasNotify_GetCalibrationData(void)
{
    UINT32 Rval;
    static UINT32 AdasNotifyCalibInitFlag = 0U;
    static SVC_CALIB_ADAS_INFO_GET_s Cfg;
    static AMBA_CAL_EM_CAM_CALIB_DATA_s CalibDataRaw2World;
    static AMBA_CAL_EM_CURVED_SURFACE_CFG_s CurvedSurface;

    if (AdasNotifyCalibInitFlag == 0U) {
        Cfg.AdasCfgV1.TransCfg.pCalibDataRaw2World = &CalibDataRaw2World;
        Cfg.AdasCfgV1.TransCfg.pCurvedSurface = &CurvedSurface;
        Rval = SvcCalib_AdasCfgGetV1(SVC_CALIB_ADAS_TYPE_FRONT, &Cfg);
        if(Rval == SVC_OK) {
            SvcLog_OK("SvcFc_GetCalibrationData", "[AdasNotify] Get ADAS CAM CALIB from NAND !Ver: ", SvcCalib_AdasCfgGetCalVer(SVC_CALIB_ADAS_TYPE_FRONT), 0U);
            g_AdasNotifyCalibData = &Cfg;
            if (SVC_CALIB_ADAS_VERSION_V1 == SvcCalib_AdasCfgGetCalVer(SVC_CALIB_ADAS_TYPE_FRONT)) {
                AdasNotifyCalibInitFlag = 1U;
            }
        }
    }

    return AdasNotifyCalibInitFlag;
}


/**
 *  Show ADAS Icon at the upper middle
 *  @param[in] Check input event need to update or not
 *  @return which ADAS icon need to show
 */
SvcADAS_NOTIFY_ADAS_PRIORITY_e SvcAdasNotify_ADAS_IconShow(SvcADAS_NOTIFY_ADAS_PRIORITY_e EventCheck)
{
    static SvcADAS_NOTIFY_ADAS_PRIORITY_e CurrentEvent = ADAS_PRIORITY_NONE;

    if (EventCheck == ADAS_PRIORITY_NONE) {
        CurrentEvent = ADAS_PRIORITY_NONE;
    } else if (EventCheck <= CurrentEvent) {
        CurrentEvent = EventCheck;
    } else {
        //Do nothing
    }

    return CurrentEvent;
}

static void* DrawSpeed_TaskEntry(void* EntryArg)
{
    UINT32  RetVal = SVC_OK;

    AmbaMisra_TouchUnused(EntryArg);

    while (RetVal == SVC_OK) {
        AMBA_SR_CANBUS_RAW_DATA_s CanbusRawDataCar;

        /* init CanbusParaData (only for Tiguan) */
        SvcCan_GetRawData(&CanbusRawDataCar);

        if (CanbusRawDataCar.FlagValidRawData == 1U) {
            SvcAdasGuiUpdate |= ADAS_GUI_SPEED_UPDATE;
        }

        RetVal = AmbaKAL_TaskSleep(30U); //30 FPS VOUT
        if (SVC_OK != RetVal) {
            SvcLog_NG(__func__, "line %u AmbaKAL_TaskSleep return %u", __LINE__, RetVal);
        }
    }

    return NULL;
}

/**
 *  Svc Draw Speedometer for ADAS
 *  @return error code
 */
#define SVC_DRAW_SPEED_TASK_STACK_SIZE       (0x6000U)
UINT32 SvcAdasNotify_DrawSpeed(void)
{
    UINT32 RetVal = SVC_OK;
    static UINT32 OsdBufWidth, OsdBufHeight;
    static SVC_TASK_CTRL_s  TaskCtrlSpeed GNU_SECTION_NOZEROINIT;
    static UINT8            DrawSpeedStack[SVC_DRAW_SPEED_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;

    RetVal = SvcOsd_GetOsdBufSize(g_AdasNotifyTargetVout, &OsdBufWidth, &OsdBufHeight);
    if (SVC_OK != RetVal) {
        SvcLog_NG(__func__, "line %u SvcOsd_GetOsdBufSize return %u", __LINE__, RetVal);
    } else {
        //Do nothing
    }
    g_SpeedLayOut.StartW = OsdBufWidth - (SVC_NOTIFY_GUI_CHAR_W * 8U);
    g_SpeedLayOut.StartH = OsdBufHeight - SVC_NOTIFY_GUI_CHAR_H;

    AmbaSvcWrap_MisraMemset(&SvcAdasDrawSpd_CanTraData, 0, sizeof(AMBA_SR_CANBUS_TRANSFER_DATA_s));

    //Create Draw speed task to polling that CAN bus invalid ot not
    TaskCtrlSpeed.Priority    = (150U);
    TaskCtrlSpeed.EntryFunc   = DrawSpeed_TaskEntry;
    TaskCtrlSpeed.EntryArg    = 0U;
    TaskCtrlSpeed.pStackBase  = DrawSpeedStack;
    TaskCtrlSpeed.StackSize   = SVC_DRAW_SPEED_TASK_STACK_SIZE;
    TaskCtrlSpeed.CpuBits     = (0x01U);

    RetVal =  SvcTask_Create("DrawSpeed_TaskEntry", &TaskCtrlSpeed);
    if (RetVal != SVC_OK) {
        SvcLog_NG(__func__, "Fail to create DrawSpeed_TaskEntry!", 0U, 0U);
    }

    return RetVal;
}

/**
 *  Update FCWS Safety distance
 *  @param[in] Dist FCWS Safety distance (Uint: M)
 */
void SvcAdasNotify_FcwsSafeDistUpdate(DOUBLE Dist)
{
    g_SvcAdasDrawSafetyDist = Dist;
    SvcAdasGuiUpdate |= ADAS_GUI_FCWS_DIST_UPDATE;
}

/**
 *  Update FCWS time to collision time (msec)
 *  @param[in] FCWS Safety time to collision time  (Uint: msec)
 */
void SvcAdasNotify_FcwsTtcUpdate(DOUBLE Time)
{
    g_SvcAdasDrawTtcTime = Time;
    SvcAdasGuiUpdate |= ADAS_GUI_FCWS_TIME_UPDATE;
}

/**
 *  Svc Draw Safe Distance for ADAS(FCWS)
 *  @return error code
 */
UINT32 SvcAdasNotify_DrawFcInfo(void)
{
    UINT32           RetVal = SVC_OK;
    static UINT32 OsdBufWidth, OsdBufHeight;

    RetVal = SvcOsd_GetOsdBufSize(g_AdasNotifyTargetVout, &OsdBufWidth, &OsdBufHeight);
    if (SVC_OK != RetVal) {
        SvcLog_NG(__func__, "line %u SvcOsd_GetOsdBufSize return %u", __LINE__, RetVal);
    } else {
        //Do nothing
    }
    g_FcInfoLayOut.StartW = OsdBufWidth - (SVC_NOTIFY_GUI_CHAR_W * 5U);
    g_FcInfoLayOut.StartH = OsdBufHeight - SVC_NOTIFY_GUI_CHAR_H;

    g_SvcAdasDrawSafetyDist   = 0.0;
    g_SvcAdasDrawTtcTime      = 0.0;

    return RetVal;
}

/**
 *  The distance of right wheel to lane boundary (in mm)
 *  @return error code
 */
UINT32 SvcAdasNotify_LdwsDistR(void)
{
    UINT32           RetVal = SVC_OK;
    static UINT32 OsdBufWidth, OsdBufHeight;

    RetVal = SvcOsd_GetOsdBufSize(g_AdasNotifyTargetVout, &OsdBufWidth, &OsdBufHeight);
    if (SVC_OK != RetVal) {
     SvcLog_NG(__func__, "line %u SvcOsd_GetOsdBufSize return %u", __LINE__, RetVal);
    } else {
     //Do nothing
    }
    g_DistRLayOut.StartW = OsdBufWidth - (SVC_NOTIFY_GUI_CHAR_W * 5U);
    g_DistRLayOut.StartH = OsdBufHeight - (SVC_NOTIFY_GUI_CHAR_H* 2U);

    g_SvcAdasDrawLdwsR = 0.0;

    return RetVal;
}

/**
 *  The distance of left wheel to lane boundary (in mm)
 *  @return error code
 */
UINT32 SvcAdasNotify_LdwsDistL(void)
{
    UINT32           RetVal = SVC_OK;
    static UINT32 OsdBufWidth, OsdBufHeight;

    RetVal = SvcOsd_GetOsdBufSize(g_AdasNotifyTargetVout, &OsdBufWidth, &OsdBufHeight);
    if (SVC_OK != RetVal) {
     SvcLog_NG(__func__, "line %u SvcOsd_GetOsdBufSize return %u", __LINE__, RetVal);
    } else {
     //Do nothing
    }
    g_DistLLayOut.StartW = OsdBufWidth - (SVC_NOTIFY_GUI_CHAR_W * 5U);
    g_DistLLayOut.StartH = OsdBufHeight - (SVC_NOTIFY_GUI_CHAR_H * 3U);

    g_SvcAdasDrawLdwsL = 0.0;

    return RetVal;
}

/**
 *  Update the distance of LDWS between front wheel to lane boundary (in mm)
 *  @param[in] DistL Update the distance of left wheel to lane boundary (in mm)
 *  @param[in] DistR Update the distance of right wheel to lane boundary (in mm)
 */
void SvcAdasNotify_LdwsDistRLUpdate(DOUBLE DistL, DOUBLE DistR)
{
    g_SvcAdasDrawLdwsL = DistL;
    g_SvcAdasDrawLdwsR = DistR;
}

static void FcModeUpdate(void)
{
    UINT32 Rval;

    if (1U == g_FcwsModeEnable) {
        if (g_FcwsMode == AMBA_WS_FCWS_MODE_TTC) { //TTC mode
            Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, 0U,
                                                 10U, 2U, ADAS_StringColor, "FCmode:TTC");
            if(Rval != 0U) {
                SvcLog_NG(__func__, "[1]SvcOsd_DrawString failed!!", 0U, 0U);
            }
        } else {//Position mode
            Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, 0U,
                                                 10U, 2U, ADAS_StringColor, "FCmode:POS");
            if(Rval != 0U) {
                SvcLog_NG(__func__, "[2]SvcOsd_DrawString failed!!", 0U, 0U);
            }
        }
    }
}

static void CarSpeedUpdate(void)
{
    AMBA_SR_CANBUS_RAW_DATA_s CanbusRawDataCar;
    UINT32 Rval, StrLength = 0;
    char   DoubleBuf[32U];

    AmbaMisra_TouchUnused(&StrLength);

    /* init SvcAdasDrawSpd_CanTraData */
    SvcAdasDrawSpd_CanTraData.CANBusGearStatus = 0U;
    SvcAdasDrawSpd_CanTraData.CANBusTurnLightStatus = 0U;
    SvcAdasDrawSpd_CanTraData.FlagValidTransferData = 0U;
    SvcAdasDrawSpd_CanTraData.TransferSpeed = 0.0;
    SvcAdasDrawSpd_CanTraData.TransferWheelAngle = 0.0;
    SvcAdasDrawSpd_CanTraData.TransferWheelAngleSpeed = 0.0;
    SvcAdasDrawSpd_CanTraData.WheelDir = 0U;
    SvcAdasDrawSpd_CanTraData.CapTS = 0U;

    /* init CanbusParaData (only for Tiguan) */
    SvcCan_GetRawData(&CanbusRawDataCar);

    /* canbus transfer */
    Rval = SvcCan_Transfer(&CanbusRawDataCar, &SvcAdasDrawSpd_CanTraData);
    if (Rval != SVC_OK) {
        SvcLog_NG(__func__, "%d", __LINE__, 0U);
    }

    SvcAdasGuiUpdate |= ADAS_GUI_SPEED_UPDATE;


#if (1 == HACK_FAKE_ENABLE)
    {
//        UINT32 RandomVal;
//        (void)AmbaWrap_rand(&RandomVal);
//        SvcAdasDrawSpd_CanTraData.TransferSpeed = (DOUBLE)(RandomVal % 140U);
        static DOUBLE SpeedHack = 0.0;
        SvcAdasDrawSpd_CanTraData.TransferSpeed = SpeedHack;
        SpeedHack++;
        if (SpeedHack >= 1000.0)
            SpeedHack = 0.0;
    }
#endif

#if (0 == HACK_FAKE_ENABLE)
    if ((SvcAdasDrawSpd_CanTraData.FlagValidTransferData == 1U)||(1U == IsDecodeMode)) {
#endif
        if (1U == IsDecodeMode) {
            SvcAdasDrawSpd_CanTraData.TransferSpeed = DecodeModeSpeed;
        }
        StrLength = AmbaUtility_DoubleToStr(DoubleBuf, 32U, SvcAdasDrawSpd_CanTraData.TransferSpeed, 0U);
        AmbaMisra_TouchUnused(&StrLength);

        if(SvcAdasDrawSpd_CanTraData.TransferSpeed >= 100.0) {
            Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, g_SpeedLayOut.StartW + (GUI_CHAR_OFFSET * 3U),
                                                 g_SpeedLayOut.StartH, 3U, ADAS_StringColor, DoubleBuf);
            AmbaMisra_TouchUnused(&Rval);
        } else if(SvcAdasDrawSpd_CanTraData.TransferSpeed >= 10.0) {
            Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, g_SpeedLayOut.StartW + SVC_NOTIFY_GUI_CHAR_W + (GUI_CHAR_OFFSET * 2U),
                                                 g_SpeedLayOut.StartH, 3U, ADAS_StringColor, DoubleBuf);
            AmbaMisra_TouchUnused(&Rval);
        } else {
            Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, g_SpeedLayOut.StartW + (SVC_NOTIFY_GUI_CHAR_W * 2U) + GUI_CHAR_OFFSET,
                                                 g_SpeedLayOut.StartH, 3U, ADAS_StringColor, DoubleBuf);
            AmbaMisra_TouchUnused(&Rval);
        }
#if (0 == HACK_FAKE_ENABLE)
    } else {
        Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, g_SpeedLayOut.StartW + SVC_NOTIFY_GUI_CHAR_W + GUI_CHAR_OFFSET,
                                             g_SpeedLayOut.StartH, 3U, ADAS_StringColor, "NA");
        AmbaMisra_TouchUnused(&Rval);
    }
#endif
    Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, g_SpeedLayOut.StartW + (SVC_NOTIFY_GUI_CHAR_W * 3U),
                                         g_SpeedLayOut.StartH, 3U, ADAS_StringColor, "KM");

    /* Draw ACC RequiredV (RequiredSpeed)*/
    if (g_RequiredSpeed >= 0.0){
        UINT32  OsdBufWidth, OsdBufHeight, RetVal;

        RetVal = SvcOsd_GetOsdBufSize(g_AdasNotifyTargetVout, &OsdBufWidth, &OsdBufHeight);
        if (SVC_OK != RetVal) {
            SvcLog_NG(__func__, "line %u SvcOsd_GetOsdBufSize return %u", __LINE__, RetVal);
        } else {
            //Do nothing
        }

        StrLength = AmbaUtility_DoubleToStr(DoubleBuf, 32U, g_RequiredSpeed, 0U);
        AmbaMisra_TouchUnused(&StrLength);
        if (g_RequiredSpeed > 100.0) {
            DoubleBuf[3] = '\0';
        } else if(g_RequiredSpeed >= 10.0) {
            DoubleBuf[2] = '\0';
        } else {
            DoubleBuf[1] = '\0';
        }

        Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, OsdBufWidth - ((SVC_NOTIFY_GUI_CHAR_W/2U) * 17U),
                (OsdBufHeight - (SVC_NOTIFY_GUI_CHAR_H * 4U)), 3U, ADAS_StringColor, "ACC ReqSpd");
        AmbaMisra_TouchUnused(&Rval);

        Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, OsdBufWidth - ((SVC_NOTIFY_GUI_CHAR_W/2U) * 7U),
                (OsdBufHeight - (SVC_NOTIFY_GUI_CHAR_H * 4U)), 3U, ADAS_StringColor, DoubleBuf);
        AmbaMisra_TouchUnused(&Rval);

        Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, (OsdBufWidth - (SVC_NOTIFY_GUI_CHAR_W * 2U)),
                (OsdBufHeight - (SVC_NOTIFY_GUI_CHAR_H * 3U)) - SVC_NOTIFY_GUI_CHAR_H, 3U, ADAS_StringColor, "KM");
    }

    if (g_Acceleration != 9999.0){
        UINT32 OsdBufWidth, OsdBufHeight, RetVal;
        UINT32 RecStartX, RecStartY, RecEndX, RecEndY;
        UINT32 AccelerationBarW, AccelerationBarH, TargetAccelerationBar, Rlen = 0U;

        RetVal = SvcOsd_GetOsdBufSize(g_AdasNotifyTargetVout, &OsdBufWidth, &OsdBufHeight);
        if (SVC_OK != RetVal) {
            SvcLog_NG(__func__, "line %u SvcOsd_GetOsdBufSize return %u", __LINE__, RetVal);
        } else {
            //Do nothing
        }
        AccelerationBarW = OsdBufWidth/30U;
        if ((OsdBufHeight/9U)>(SVC_LW_LKA_RADIUS*2U)) {
            AccelerationBarH = OsdBufHeight/9U;
        } else {
            AccelerationBarH = SVC_LW_LKA_RADIUS*2U;
        }
        RecStartX = (OsdBufWidth/2U) + (SVC_LW_LKA_RADIUS*2U);//+ OsdBufWidth/18U ;
        RecStartY = OsdBufHeight - (LKA_MINUS_ICON_H + 10U + SVC_LW_LKA_RADIUS) - SVC_LW_LKA_RADIUS;
        RecEndX   = RecStartX + AccelerationBarW;
        RecEndY   = RecStartY + AccelerationBarH;
        RetVal = SvcOsd_DrawRect(g_AdasNotifyTargetVout, RecStartX, RecStartY, RecEndX, RecEndY, 0xffff0000U, 5U);
        AmbaMisra_TouchUnused(&RetVal);

        SVC_WRAP_SNPRINT "%3.1f"
                         SVC_SNPRN_ARG_S DoubleBuf
                         SVC_SNPRN_ARG_DOUBLE FC_MAX_ACCELERATION
                         SVC_SNPRN_ARG_BSIZE sizeof(DoubleBuf)
                         SVC_SNPRN_ARG_RLEN &Rlen
                         SVC_SNPRN_ARG_E

        RetVal = SvcOsd_DrawString(g_AdasNotifyTargetVout, RecStartX, RecStartY - 15U, 2U, 0xff00ff00U, DoubleBuf);
        AmbaMisra_TouchUnused(&RetVal);
        RetVal = SvcOsd_DrawString(g_AdasNotifyTargetVout, RecStartX + 25U, RecStartY - 17U, 1U, 0xff00ff00U, "m/s^2");
        AmbaMisra_TouchUnused(&RetVal);


        SVC_WRAP_SNPRINT "%3.1f"
                         SVC_SNPRN_ARG_S DoubleBuf
                         SVC_SNPRN_ARG_DOUBLE FC_MIN_ACCELERATION
                         SVC_SNPRN_ARG_BSIZE sizeof(DoubleBuf)
                         SVC_SNPRN_ARG_RLEN &Rlen
                         SVC_SNPRN_ARG_E

        RetVal = SvcOsd_DrawString(g_AdasNotifyTargetVout, RecStartX, RecEndY + 2U, 2U, 0xffff0000U, DoubleBuf);
        AmbaMisra_TouchUnused(&RetVal);
        RetVal = SvcOsd_DrawString(g_AdasNotifyTargetVout, RecStartX + 5U, RecEndY + 15U, 1U, 0xffff0000U, "m/s^2");
        AmbaMisra_TouchUnused(&RetVal);


        /*
         *  ___ FC_MAX_ACCELERATION
         * |   |                                    g_Acceleration - FC_MIN_ACCELERATION               x
         * |   | <--- g_Acceleration      =>     -----------------------------------------  =  ----------------
         * |   |    |                             FC_MAX_ACCELERATION - FC_MIN_ACCELERATION    AccelerationBarH
         * |   |    |x
         * |___|    |
         *      FC_MIN_ACCELERATION                    AccelerationBarH(g_Acceleration - FC_MIN_ACCELERATION)
         *                                 =>     x = --------------------------------------------------------
         *                                                  (FC_MAX_ACCELERATION - FC_MIN_ACCELERATION)
         *
         **/
        /* Draw ACCELERATION BAR if TransferSpeed must >= FC_ACC_ACTIVESPEED */
        if (SvcAdasDrawSpd_CanTraData.TransferSpeed >= FC_ACC_ACTIVESPEED) {
            UINT32 AccelerationColor;
            DOUBLE TargetAccelerationBarTmp;
            INT64  DoubleToINT64;

            /* Use different color represent over FC_MIN_ACCELERATION or not */
            if (g_Acceleration < (DOUBLE)FC_MIN_ACCELERATION) {
                AccelerationColor = 0xffffff00U;
            } else {
                AccelerationColor = 0xffffffffU;
            }
            TargetAccelerationBarTmp = (g_Acceleration-(DOUBLE)FC_MIN_ACCELERATION)/((DOUBLE)FC_MAX_ACCELERATION - (DOUBLE)FC_MIN_ACCELERATION);
            TargetAccelerationBarTmp *= (DOUBLE)AccelerationBarH;
            DoubleToINT64 = (INT64)(TargetAccelerationBarTmp);
            AmbaMisra_TypeCast32(&TargetAccelerationBar, &DoubleToINT64);
            /* Check TargetAccelerationBar not over RecEndY boundary */
            if (TargetAccelerationBar > RecEndY) {
                TargetAccelerationBar = 0U;
            }
            TargetAccelerationBar = RecEndY - TargetAccelerationBar;
            RetVal = SvcOsd_DrawLine(g_AdasNotifyTargetVout, RecStartX -5U, TargetAccelerationBar,
                                    RecEndX + 10U, TargetAccelerationBar,
                                    2U, AccelerationColor);
            AmbaMisra_TouchUnused(&RetVal);
            SVC_WRAP_SNPRINT "%3.1f"
                             SVC_SNPRN_ARG_S DoubleBuf
                             SVC_SNPRN_ARG_DOUBLE g_Acceleration
                             SVC_SNPRN_ARG_BSIZE sizeof(DoubleBuf)
                             SVC_SNPRN_ARG_RLEN &Rlen
                             SVC_SNPRN_ARG_E
            RetVal = SvcOsd_DrawString(g_AdasNotifyTargetVout, RecEndX + 15U, TargetAccelerationBar - 8U, 2U, AccelerationColor, DoubleBuf);
            AmbaMisra_TouchUnused(&RetVal);
        }
    }

    //Draw format_id
//    {
//        SVC_USER_PREF_s *pSvcUserPref;
//        UINT32 RetVal = SVC_OK;
//
//        RetVal = SvcUserPref_Get(&pSvcUserPref);
//        if (SVC_OK != RetVal) {
//            SvcLog_NG(__func__, "line %u SvcUserPref_Get() failed with %d", __LINE__, RetVal);
//        } else {
//
//            UINT32  OsdBufWidth, OsdBufHeight;
//
//            RetVal = SvcOsd_GetOsdBufSize(g_AdasNotifyTargetVout, &OsdBufWidth, &OsdBufHeight);
//            if (SVC_OK != RetVal) {
//                SvcLog_NG(__func__, "line %u SvcOsd_GetOsdBufSize return %u", __LINE__, RetVal);
//            } else {
//                //Do nothing
//            }
//            RetVal = AmbaUtility_UInt32ToStr(DoubleBuf, 32U, pSvcUserPref->FormatId, 10U);
//            if (0U >= RetVal) {
//                SvcLog_NG(__func__, "line %u AmbaUtility_UInt32ToStr() failed with %d", __LINE__, RetVal);
//            }
//
//            RetVal = SvcOsd_DrawString(g_AdasNotifyTargetVout, ((OsdBufWidth/2U) - 70U), 5U, 3U, ADAS_StringColor, "Format_id:");
//            if (SVC_OK != RetVal) {
//                SvcLog_NG(__func__, "line %u SvcOsd_DrawString() failed with %d", __LINE__, RetVal);
//            }
//
//            RetVal = SvcOsd_DrawString(g_AdasNotifyTargetVout, ((OsdBufWidth/2U) + 110U), 5U, 3U, ADAS_StringColor, DoubleBuf);
//            if (SVC_OK != RetVal) {
//                SvcLog_NG(__func__, "line %u SvcOsd_DrawString() failed with %d", __LINE__, RetVal);
//            }
//        }
//    }


    AmbaMisra_TouchUnused(&Rval);
}

static void LdInfoUpdate(void)
{
    char   DoubleBuf[32U];
    UINT32 Rlen = 0U, Rval = 0U;

    if (g_SvcAdasDrawLdwsL != 0.0) {
#if (1 == HACK_FAKE_ENABLE)
        static DOUBLE LdwsLHack = 0.0;
        g_SvcAdasDrawLdwsL = LdwsLHack / 10;
        LdwsLHack++;
#endif

        /* Double value to String */
        SVC_WRAP_SNPRINT "%4.1f"
                         SVC_SNPRN_ARG_S DoubleBuf
                         SVC_SNPRN_ARG_DOUBLE g_SvcAdasDrawLdwsL
                         SVC_SNPRN_ARG_BSIZE sizeof(DoubleBuf)
                         SVC_SNPRN_ARG_RLEN &Rlen
                         SVC_SNPRN_ARG_E

//        Rval = SvcOsd_DrawSolidRect(g_AdasNotifyTargetVout, g_DistLLayOut.StartW,
//                                                g_DistLLayOut.StartH,
//                                                g_DistLLayOut.StartW + (SVC_NOTIFY_GUI_CHAR_W * 5U),
//                                                g_DistLLayOut.StartH +  + SVC_NOTIFY_GUI_CHAR_H,
//                                                73U);
        AmbaMisra_TouchUnused(&Rval);
        if(g_SvcAdasDrawLdwsL >= 1000.0) {
            DoubleBuf[4] = '\0';
            Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, g_FcInfoLayOut.StartW + SVC_NOTIFY_GUI_CHAR_W + (GUI_CHAR_OFFSET * 2U),
                                                 g_DistLLayOut.StartH, 3U, ADAS_StringColor, DoubleBuf);
            AmbaMisra_TouchUnused(&Rval);
        } else if(g_SvcAdasDrawLdwsL >= 100.0) {
            DoubleBuf[3] = '\0';
            Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, g_FcInfoLayOut.StartW + (SVC_NOTIFY_GUI_CHAR_W * 2U) + GUI_CHAR_OFFSET,
                                                 g_DistLLayOut.StartH, 3U, ADAS_StringColor, DoubleBuf);
            AmbaMisra_TouchUnused(&Rval);
        } else if(g_SvcAdasDrawLdwsL >= 10.0) {
            DoubleBuf[2] = '\0';
            Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, g_FcInfoLayOut.StartW + (SVC_NOTIFY_GUI_CHAR_W * 3U),
                                                 g_DistLLayOut.StartH, 3U, ADAS_StringColor, DoubleBuf);
            AmbaMisra_TouchUnused(&Rval);
        } else {
            DoubleBuf[1] = '\0';
            Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, g_FcInfoLayOut.StartW + (SVC_NOTIFY_GUI_CHAR_W * 3U),
                                                 g_DistLLayOut.StartH, 3U, ADAS_StringColor, DoubleBuf);
            AmbaMisra_TouchUnused(&Rval);
        }
    } else {
        //Do nothing
    }
    Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, g_DistLLayOut.StartW + (SVC_NOTIFY_GUI_CHAR_W * 4U),
                                         g_DistLLayOut.StartH, 3U, ADAS_StringColor, "L");
    AmbaMisra_TouchUnused(&Rval);

    if (g_SvcAdasDrawLdwsR != 0.0) {
#if (1 == HACK_FAKE_ENABLE)
        static DOUBLE LdwsRHack = 0.0;
        g_SvcAdasDrawLdwsL = LdwsRHack / 10;
        LdwsRHack++;
#endif
        /* Double value to String */
        SVC_WRAP_SNPRINT "%4.1f"
                         SVC_SNPRN_ARG_S DoubleBuf
                         SVC_SNPRN_ARG_DOUBLE g_SvcAdasDrawLdwsR
                         SVC_SNPRN_ARG_BSIZE sizeof(DoubleBuf)
                         SVC_SNPRN_ARG_RLEN &Rlen
                         SVC_SNPRN_ARG_E

//        Rval = SvcOsd_DrawSolidRect(g_AdasNotifyTargetVout, g_DistRLayOut.StartW,
//                                                g_DistRLayOut.StartH,
//                                                g_DistRLayOut.StartW + (SVC_NOTIFY_GUI_CHAR_W * 5U),
//                                                g_DistRLayOut.StartH + SVC_NOTIFY_GUI_CHAR_H,
//                                                73U);
        AmbaMisra_TouchUnused(&Rval);
        if(g_SvcAdasDrawLdwsR >= 1000.0) {
            DoubleBuf[4] = '\0';
            Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, g_FcInfoLayOut.StartW + SVC_NOTIFY_GUI_CHAR_W + (GUI_CHAR_OFFSET * 2U),
                                                 g_DistRLayOut.StartH, 3U, ADAS_StringColor, DoubleBuf);
            AmbaMisra_TouchUnused(&Rval);
        } else if(g_SvcAdasDrawLdwsR >= 100.0) {
            DoubleBuf[3] = '\0';
            Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, g_FcInfoLayOut.StartW + (SVC_NOTIFY_GUI_CHAR_W * 2U) + GUI_CHAR_OFFSET,
                                                 g_DistRLayOut.StartH, 3U, ADAS_StringColor, DoubleBuf);
            AmbaMisra_TouchUnused(&Rval);
        } else if(g_SvcAdasDrawLdwsR >= 10.0) {
            DoubleBuf[2] = '\0';
            Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, g_FcInfoLayOut.StartW + (SVC_NOTIFY_GUI_CHAR_W * 3U),
                                                 g_DistRLayOut.StartH, 3U, ADAS_StringColor, DoubleBuf);
            AmbaMisra_TouchUnused(&Rval);
        } else {
            DoubleBuf[1] = '\0';
            Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, g_FcInfoLayOut.StartW + (SVC_NOTIFY_GUI_CHAR_W * 3U),
                                                 g_DistRLayOut.StartH, 3U, ADAS_StringColor, DoubleBuf);
            AmbaMisra_TouchUnused(&Rval);
        }
    } else {
        //Do nothing
    }
    Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, g_DistRLayOut.StartW + (SVC_NOTIFY_GUI_CHAR_W * 4U),
                                         g_DistRLayOut.StartH, 3U, ADAS_StringColor, "R");
    AmbaMisra_TouchUnused(&Rval);
}

static void FcInfoUpdate(void)
{
    char   DoubleBuf[32U];
    UINT32 Rlen = 0U, Rval = 0U;

    if (g_FcwsMode == AMBA_WS_FCWS_MODE_TTC) { //TTC mode
        if (g_SvcAdasDrawTtcTime != 0.0) {
#if (1 == HACK_FAKE_ENABLE)
            static DOUBLE TtcTimeHack = 0.0;
#endif
            DOUBLE UpdateFcInfo = 0.0;
            UpdateFcInfo = g_SvcAdasDrawTtcTime;
#if (1 == HACK_FAKE_ENABLE)
            UpdateFcInfo = TtcTimeHack;
            TtcTimeHack++;
            UpdateFcInfo = TtcTimeHack;
#endif
            UpdateFcInfo = UpdateFcInfo/ 1000.0; //msec to second
            if (UpdateFcInfo < 10.0) {
                if (UpdateFcInfo < 0.0) {
                    /* Double value to String */
                    SVC_WRAP_SNPRINT "%4.1f"
                                     SVC_SNPRN_ARG_S DoubleBuf
                                     SVC_SNPRN_ARG_DOUBLE UpdateFcInfo
                                     SVC_SNPRN_ARG_BSIZE sizeof(DoubleBuf)
                                     SVC_SNPRN_ARG_RLEN &Rlen
                                     SVC_SNPRN_ARG_E
                     DoubleBuf[1] = '\0';
                     Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, g_FcInfoLayOut.StartW + (SVC_NOTIFY_GUI_CHAR_W * 3U),
                                                          g_FcInfoLayOut.StartH, 3U, ADAS_StringColor, DoubleBuf);
                     AmbaMisra_TouchUnused(&Rval);
                } else {
                    /* Double value to String */
                    SVC_WRAP_SNPRINT "%4.2f"
                                     SVC_SNPRN_ARG_S DoubleBuf
                                     SVC_SNPRN_ARG_DOUBLE UpdateFcInfo
                                     SVC_SNPRN_ARG_BSIZE sizeof(DoubleBuf)
                                     SVC_SNPRN_ARG_RLEN &Rlen
                                     SVC_SNPRN_ARG_E
                     Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, g_FcInfoLayOut.StartW + SVC_NOTIFY_GUI_CHAR_W + (GUI_CHAR_OFFSET * 2U),
                                                          g_FcInfoLayOut.StartH, 3U, ADAS_StringColor, DoubleBuf);
                    AmbaMisra_TouchUnused(&Rval);
                }
            } else { //>=10
                /* Double value to String */
                SVC_WRAP_SNPRINT "%4.1f"
                                 SVC_SNPRN_ARG_S DoubleBuf
                                 SVC_SNPRN_ARG_DOUBLE UpdateFcInfo
                                 SVC_SNPRN_ARG_BSIZE sizeof(DoubleBuf)
                                 SVC_SNPRN_ARG_RLEN &Rlen
                                 SVC_SNPRN_ARG_E
                if (UpdateFcInfo >= 10000.0) {
                    Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, g_FcInfoLayOut.StartW + SVC_NOTIFY_GUI_CHAR_W + (GUI_CHAR_OFFSET * 2U),
                                                         g_FcInfoLayOut.StartH, 3U, ADAS_StringColor, ">10k");
                    AmbaMisra_TouchUnused(&Rval);
                } else if (UpdateFcInfo >= 1000.0) {
                    DoubleBuf[4] = '\0';
                    Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, g_FcInfoLayOut.StartW + SVC_NOTIFY_GUI_CHAR_W + (GUI_CHAR_OFFSET * 2U),
                                                         g_FcInfoLayOut.StartH, 3U, ADAS_StringColor, DoubleBuf);
                    AmbaMisra_TouchUnused(&Rval);
                } else if(UpdateFcInfo >= 100.0) {
                    DoubleBuf[3] = '\0';
                    Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, g_FcInfoLayOut.StartW + (SVC_NOTIFY_GUI_CHAR_W * 2U) + GUI_CHAR_OFFSET,
                                                         g_FcInfoLayOut.StartH, 3U, ADAS_StringColor, DoubleBuf);
                    AmbaMisra_TouchUnused(&Rval);
                } else {
                    DoubleBuf[2] = '\0';
                    Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, g_FcInfoLayOut.StartW + (SVC_NOTIFY_GUI_CHAR_W * 3U),
                                                         g_FcInfoLayOut.StartH, 3U, ADAS_StringColor, DoubleBuf);
                    AmbaMisra_TouchUnused(&Rval);
                }
            }
        }
        Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, g_FcInfoLayOut.StartW + (SVC_NOTIFY_GUI_CHAR_W * 4U),
                                             g_FcInfoLayOut.StartH, 3U, ADAS_StringColor, "S");
        AmbaMisra_TouchUnused(&Rval);
    } else {//Position mode
        if (g_SvcAdasDrawSafetyDist != 0.0) {
#if (1 == HACK_FAKE_ENABLE)
            static DOUBLE DistHack = 0.0;
            g_SvcAdasDrawSafetyDist = DistHack;
            DistHack++;
#endif
            /* Double value to String */
            SVC_WRAP_SNPRINT "%4.1f"
                             SVC_SNPRN_ARG_S DoubleBuf
                             SVC_SNPRN_ARG_DOUBLE g_SvcAdasDrawSafetyDist
                             SVC_SNPRN_ARG_BSIZE sizeof(DoubleBuf)
                             SVC_SNPRN_ARG_RLEN &Rlen
                             SVC_SNPRN_ARG_E

             if (g_SvcAdasDrawSafetyDist >= 10000.0) {
                 Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, g_FcInfoLayOut.StartW + SVC_NOTIFY_GUI_CHAR_W + (GUI_CHAR_OFFSET * 2U),
                                                      g_FcInfoLayOut.StartH, 3U, ADAS_StringColor, ">10k");
                 AmbaMisra_TouchUnused(&Rval);
             } else if (g_SvcAdasDrawSafetyDist >= 1000.0) {
                 DoubleBuf[4] = '\0';
                 Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, g_FcInfoLayOut.StartW + SVC_NOTIFY_GUI_CHAR_W + (GUI_CHAR_OFFSET * 2U),
                                                      g_FcInfoLayOut.StartH, 3U, ADAS_StringColor, DoubleBuf);
                 AmbaMisra_TouchUnused(&Rval);
             } else if(g_SvcAdasDrawSafetyDist >= 100.0) {
                 DoubleBuf[3] = '\0';
                 Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, g_FcInfoLayOut.StartW + (SVC_NOTIFY_GUI_CHAR_W * 2U) + GUI_CHAR_OFFSET,
                                                      g_FcInfoLayOut.StartH, 3U, ADAS_StringColor, DoubleBuf);
                 AmbaMisra_TouchUnused(&Rval);
             } else if(g_SvcAdasDrawSafetyDist < 0.0) {
                 DoubleBuf[1] = '\0';
                 Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, g_FcInfoLayOut.StartW + (SVC_NOTIFY_GUI_CHAR_W * 3U),
                                                      g_FcInfoLayOut.StartH, 3U, ADAS_StringColor, DoubleBuf);
                 AmbaMisra_TouchUnused(&Rval);
             } else {
                 DoubleBuf[2] = '\0';
                 Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, g_FcInfoLayOut.StartW + (SVC_NOTIFY_GUI_CHAR_W * 3U),
                                                      g_FcInfoLayOut.StartH, 3U, ADAS_StringColor, DoubleBuf);
                 AmbaMisra_TouchUnused(&Rval);
             }
        }
        Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, g_FcInfoLayOut.StartW + (SVC_NOTIFY_GUI_CHAR_W * 4U),
                                             g_FcInfoLayOut.StartH, 3U, ADAS_StringColor, "M");
        AmbaMisra_TouchUnused(&Rval);
    }
}

static void HorizontalLineUpdate(void)
{
    if (1U == g_FcwsModeEnable) {
//        UINT32 RetVal;
//
//        RetVal = SvcOsd_DrawLine(g_HorizontalLine.Channel,
//                                 g_HorizontalLine.X1, g_HorizontalLine.Y1,
//                                 g_HorizontalLine.X2, g_HorizontalLine.Y2,
//                                 g_HorizontalLine.Thickness, ADAS_StringColor);
//        if (SVC_NG == RetVal) {
//            SvcLog_NG(__func__, "HorizontalLineUpdate() error !", 0U, 0U);
//        }
        AmbaMisra_TouchUnused(&g_HorizontalLine);
    }
}

static void CalibHorizontalYUpdate(void)
{
    if (1U == g_CalibEnable){
        UINT32 RetVal;
        RetVal = SvcOsd_DrawLine(g_AdasNotifyTargetVout,
                                 0,                  g_CalibHorizontalY,
                                 g_CalibHorizontalX, g_CalibHorizontalY,
                                 CALIB_Horizontal_Thickness, ADAS_StringColor);
        if (SVC_NG == RetVal) {
            SvcLog_NG(__func__, "CalibHorizontalYUpdate() error !", 0U, 0U);
            SvcLog_NG(__func__, "g_CalibHorizontalX = %d, g_CalibHorizontalY = %d", g_CalibHorizontalX, g_CalibHorizontalY);
        }
    }
}

static void WarningZoneQuickCalibAdj(void)
{
    if (1U == g_QuickCalibZoneAdj){
//        UINT32 RetVal;
//        RetVal = SvcOsd_DrawLine(g_AdasNotifyTargetVout,
//                 (UINT32)g_Zone1X[0], (UINT32)g_Zone1Y[0],
//                 (UINT32)g_Zone1X[1], (UINT32)g_Zone1Y[1],
//                 CALIB_Horizontal_Thickness, ADAS_StringColor);
//
//        RetVal = SvcOsd_DrawLine(g_AdasNotifyTargetVout,
//                 (UINT32)g_Zone1X[2], (UINT32)g_Zone1Y[2],
//                 (UINT32)g_Zone1X[3], (UINT32)g_Zone1Y[3],
//                 CALIB_Horizontal_Thickness, ADAS_StringColor);
//
//        if (SVC_NG == RetVal) {
//            SvcLog_NG(__func__, "WarningZoneUpdate() error !", 0U, 0U);
//        }

        AmbaMisra_TouchUnused(&g_QuickCalibZoneAdj);
    }
}

static UINT32 TransferWorld2SrcV1(AMBA_SR_LINE_PNT_3D_s LineWorld,
        const AMBA_CAL_EM_CALC_COORD_CFG_V1_s *pCalCfg,
        AMBA_SR_LINE_PNT_2D_s *LineSrc)
{
    /**
    *  Width, Height: Source image's width and height
    */
    UINT32 Rval;
    AMBA_CAL_POINT_DB_3D_s pnt3D;
    AMBA_CAL_POINT_DB_2D_s pnt2D;
	
	AmbaSvcWrap_MisraMemset(&pnt3D, 0, sizeof(AMBA_CAL_POINT_DB_3D_s));
	AmbaSvcWrap_MisraMemset(&pnt2D, 0, sizeof(AMBA_CAL_POINT_DB_2D_s));
//    AMBA_CAL_EM_CALC_COORD_CFG_s CalCfg;

//    AmbaSvcWrap_MisraMemcpy(&(CalCfg.Type), &(pCalCfg->Type), sizeof(AMBA_CAL_EM_VIEW_TYPE_e));
//    AmbaSvcWrap_MisraMemcpy(&(CalCfg.CamPos), &(pCalCfg->CamPos), sizeof(AMBA_CAL_POINT_DB_3D_s));
//    AmbaSvcWrap_MisraMemcpy(&(CalCfg.Roi), &(pCalCfg->Roi), sizeof(AMBA_CAL_ROI_s));
//    AmbaSvcWrap_MisraMemcpy(&(CalCfg.VoutArea), &(pCalCfg->VoutArea), sizeof(AMBA_CAL_ROI_s));
//    CalCfg.TransCfg.pCalibDataRaw2World = pCalCfg->TransCfg.pCalibDataRaw2World;
//    CalCfg.TransCfg.pCurvedSurface = pCalCfg->TransCfg.pCurvedSurface;

    pnt3D.X = LineWorld.X;
    pnt3D.Y = LineWorld.Y;
    pnt3D.Z = LineWorld.Z;

//    Rval = AmbaCal_EmConvPtWorldToImgPlane(&CalCfg, &pnt3D, &pnt2D);
    Rval = AmbaCal_EmConvPtWorldToImgV1(pCalCfg, &pnt3D, &pnt2D);
    if (Rval != 0U) {
        AmbaPrint_PrintInt5("TransferWorld2SrcV1, AmbaCal_EmConvPtWorldToImgPlane failed. 3D(%d, %d) -> 2D(%d, %d)", (INT32)pnt3D.X, (INT32)pnt3D.Y, (INT32)pnt2D.X, (INT32)pnt2D.Y, 0);
    }

    LineSrc->X = (INT32)pnt2D.X;
    LineSrc->Y = (INT32)pnt2D.Y;

    return Rval;
}

#define UTIL_VSL_FCWS_WZ_FURTHEST_DIS           (70000.0)
#define UTIL_VSL_FCWS_WZ_CLOSEST_DIS            (20000.0)
#define UTIL_VSL_FCWS_LANE_WIDTH                (3750.0)
#define KPH2SI                                  (0.27777777778f)     /**< km/hr to m/s */
static void WarningZoneFromAdasFC(void)
{
    UINT32 SubjectLaneInfoIdx = g_SubjectLaneInfoIdx;
//    AMBA_SR_LANE_MEMBER_s *pLaneInfo;
    AMBA_SR_LANE_MEMBER_s LaneInfo;
    AMBA_SR_LANE_MEMBER_s EgoData;
    DOUBLE yLimit;
    UINT32 RetV = 0U;
//    INT32 BufWidth  = (INT32)g_AdasNotifyCalibData->AdasCfgV1.VoutArea.Width;
//    INT32 BufHeight = (INT32)g_AdasNotifyCalibData->AdasCfgV1.VoutArea.Height;

//    SvcLog_NG(__func__, "BufWidth = %d, BufHeight = %d", BufWidth, BufHeight);
    if (SubjectLaneInfoIdx == 0U) {
        SubjectLaneInfoIdx = (SVC_FC_WARNINGZONE_OUTPUT_DEPTH - 1U);
    } else {
        SubjectLaneInfoIdx--;
    }
//    AmbaMisra_TypeCast32(&pLaneInfo, &g_SubjectLaneInfo[SubjectLaneInfoIdx]);

    AmbaSvcWrap_MisraMemcpy(&LaneInfo, &g_SubjectLaneInfo[SubjectLaneInfoIdx], sizeof(AMBA_SR_LANE_MEMBER_s));

//    if ((pLaneInfo == NULL) || (pLaneInfo->LineStat[AMBA_SR_LANE_LINE_CENTER] == 0U)) {
    if (LaneInfo.LineStat[AMBA_SR_LANE_LINE_CENTER] == 0U) {
        AmbaSvcWrap_MisraMemset(&EgoData, 0, sizeof(AMBA_SR_LANE_MEMBER_s));
        EgoData.LineFuncPara[AMBA_SR_LANE_LINE_LEFT0].Coef[0] = -(UTIL_VSL_FCWS_LANE_WIDTH / 3.0);
        EgoData.LineFuncPara[AMBA_SR_LANE_LINE_RIGHT0].Coef[0] = UTIL_VSL_FCWS_LANE_WIDTH / 3.0;
        yLimit = UTIL_VSL_FCWS_WZ_FURTHEST_DIS;
    } else {
//        AmbaSvcWrap_MisraMemcpy(&EgoData, pLaneInfo, sizeof(AMBA_SR_LANE_MEMBER_s));
        AmbaSvcWrap_MisraMemcpy(&EgoData, &LaneInfo, sizeof(AMBA_SR_LANE_MEMBER_s));
        yLimit = UTIL_VSL_FCWS_WZ_CLOSEST_DIS;
    }

    if (g_FcwsMode == AMBA_WS_FCWS_MODE_TTC) { //TTC mode
        for (UINT32 LineId = 0U; LineId < AMBA_SR_LANE_LINE_MAX_NUM; LineId++) {
             UINT32 pNum;
             AMBA_SR_LINE_PNT_3D_s pnt3d;
             AMBA_SR_LINE_PNT_2D_s pnt2d/*, pnt2dConvert*/;
             INT32 xlist[AMBA_SR_MAX_PNT_NUM], ylist[AMBA_SR_MAX_PNT_NUM];
             DOUBLE yStart, yEnd, yFarthest, yFinal;

             AmbaSvcWrap_MisraMemset(&xlist, 0, sizeof(INT32)*(AMBA_SR_MAX_PNT_NUM));
             AmbaSvcWrap_MisraMemset(&ylist, 0, sizeof(INT32)*(AMBA_SR_MAX_PNT_NUM));
             if (LineId == AMBA_SR_LANE_LINE_CENTER) {
                 continue;
             }
             yStart = (g_WsFcwsCfg.CarSize[1]/2.0) + 2000.0;
             if (EgoData.LineStat[AMBA_SR_LANE_LINE_CENTER] == AMBA_SR_LANE_LINE_STAT_VALID) {
                 yEnd = EgoData.Line3D[AMBA_SR_LANE_LINE_CENTER].PntData[EgoData.Line3D[AMBA_SR_LANE_LINE_CENTER].PntNum-1U].Y;
             } else {
                 yEnd = 0.0;
             }
             yFarthest = g_FcTransferSpeed* KPH2SI* g_WsFcwsCfg.FCWSSpec.TTC_th[0]* g_WsFcwsCfg.FCWSSpec.PrewarnToColwarnRatio;
             yFinal = (yEnd < yFarthest) ? yEnd : yFarthest;
             yFinal = (yFinal > yLimit) ? yFinal : yLimit; // When yEnd = 0 and TransSpeed too low, not enough points to draw
             pNum = 0U;
             for (UINT32 pId = 0U; pId < AMBA_SR_MAX_PNT_NUM; pId++) {
                 DOUBLE xval, yval;
                 const DOUBLE *pCoef;

                 yval = yStart + ((DOUBLE)pId * 3000.0);
                 pCoef = EgoData.LineFuncPara[LineId].Coef;
                 xval = pCoef[0] + (pCoef[1] * yval) + (pCoef[2] * yval * yval);

                 pnt3d.X = xval;
                 pnt3d.Y = yval;
                 pnt3d.Z = 0.0;
                 RetV = TransferWorld2SrcV1(pnt3d, &g_AdasNotifyCalibData->AdasCfgV1, &pnt2d);
                 if ((RetV != 0U) || (yval > yFinal)) {
                     if (RetV != 0U) {
                         AmbaPrint_PrintUInt5("UtilVSL_DrawFcwsWzV2, TransferWorld2SrcV1 failed", 0U, 0U, 0U, 0U, 0U);
                     }
                     break;
                 }
                 xlist[pId] = pnt2d.X;
                 ylist[pId] = pnt2d.Y;
                 pNum ++;
             }

             if (pNum > 1U) {
                 {
                     UINT32 RetVal;
                     const SVC_GUI_CANVAS_s *pCanvas;
                     const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
                     UINT32 CvFlowNum = pResCfg->CvFlowNum;
                     UINT32 CvFlowBits = pResCfg->CvFlowBits;
                     const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;
                     UINT32 RatioX, RatioY, i, FovIdx = 0U;
                     SvcADAS_NOTIFY_POINT_s WarningZone[AMBA_SR_MAX_PNT_NUM];

                     pCanvas = SvcSegDrawTask_GetCanvasROIinfo();
                     for (i = 0U; i < CvFlowNum; i++) {
                         if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                             if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_SEG_FDAG) ||
                                 (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_SEG)) {
                                 FovIdx = pCvFlow[i].InputCfg.Input[0].StrmId;
                             }
                         }
                     }
                     RatioX = ((UINT32)pResCfg->FovCfg[FovIdx].MainWin.Width) /(pCanvas->Width);
                     RatioY = ((UINT32)pResCfg->FovCfg[FovIdx].MainWin.Height)/(pCanvas->Height);
                     for(UINT32 ZoneIdx = 0U;ZoneIdx < pNum; ZoneIdx++) {
                         WarningZone[ZoneIdx].x = (INT16)(((xlist[ZoneIdx]) / (INT32)RatioX) + (INT32)pCanvas->StartX);
                         WarningZone[ZoneIdx].y = (INT16)(((ylist[ZoneIdx]) /(INT32)RatioY) + (INT32)pCanvas->StartY);
                     }
                     RetVal = SvcOsd_DrawMultiplePointsLine(g_AdasNotifyTargetVout, WarningZone, pNum, CALIB_Horizontal_Thickness, 0xff00ff00U);
                     if (SVC_NG == RetVal) {
                         SvcLog_NG(__func__, "SvcOsd_DrawMultiplePointsLine() error !", 0U, 0U);
                     }
                 }
             }
        }
    } else {//Position mode
//        const DOUBLE *pCoef;
//        DOUBLE xval, yval;
//        INT32 xlist[AMBA_SR_MAX_PNT_NUM], ylist[AMBA_SR_MAX_PNT_NUM];
//        AMBA_SR_LINE_PNT_3D_s pnt3d;
//        AMBA_SR_LINE_PNT_2D_s pnt2d;
//        DOUBLE TotalDelay = g_WsFcwsCfg.Delay.DetectionDelay + g_WsFcwsCfg.Delay.OutputDelay + g_WsFcwsCfg.Delay.ReconstructionDelay;
//        DOUBLE SafeDistance = g_WsFcwsCfg.FCWSSpec.Position_th[(INT32)g_FcTransferSpeed] * 1000.0;
//        DOUBLE SafeDistanceTotal = 0.0;       // safe distance, units:mm
//        DOUBLE PreSafeDistanceTotal = 0.0;    // preliminary safe distance, units:mm
//
//        SafeDistanceTotal = SafeDistance + (((TotalDelay * g_FcTransferSpeed) * 5.0) / 18.0);
//        PreSafeDistanceTotal = SafeDistanceTotal * g_WsFcwsCfg.FCWSSpec.PrewarnToColwarnRatio;
//
//        for (UINT32 i = 0U; i < (AMBA_SR_LANE_LINE_MAX_NUM-1U); i++) {
//            pCoef = EgoData.LineFuncPara[i].Coef;
//            xval = pCoef[0] + (pCoef[1] * SafeDistanceTotal) + (pCoef[2] * SafeDistanceTotal * SafeDistanceTotal);
//            pnt3d.X = xval;
//            pnt3d.Y = SafeDistanceTotal;
//            pnt3d.Z = 0.0;
//            RetV = TransferWorld2SrcV1(pnt3d, &g_AdasNotifyCalibData->AdasCfgV1, &pnt2d);
//            if (RetV != 0U) {
//                AmbaPrint_PrintUInt5("UtilVSL_DrawFcwsWzV2, TransferWorld2SrcV1 failed", 0U, 0U, 0U, 0U, 0U);
//                break;
//            }
//            xlist[i] = pnt2d.X;
//            ylist[i] = pnt2d.Y;
//        }
//        RetV = UtilDRAW_PolyLine(&BufInfo, xlist, ylist, 2, 0, 3U, LineColor2);
//
//        for (UINT32 i = 0U; i < (AMBA_SR_LANE_LINE_MAX_NUM-1U); i++) {
//            pCoef = EgoData.LineFuncPara[i].Coef;
//            xval = pCoef[0] + (pCoef[1] * PreSafeDistanceTotal) + (pCoef[2] * PreSafeDistanceTotal * PreSafeDistanceTotal);
//            pnt3d.X = xval;
//            pnt3d.Y = PreSafeDistanceTotal;
//            pnt3d.Z = 0.0;
//            RetV = TransferWorld2SrcV1(pnt3d, &g_AdasNotifyCalibData->AdasCfgV1, &pnt2d);
//            if (RetV != 0U) {
//                AmbaPrint_PrintUInt5("UtilVSL_DrawFcwsWzV2, TransferWorld2SrcV1 failed", 0U, 0U, 0U, 0U, 0U);
//                break;
//            }
//            xlist[i] = pnt2d.X;
//            ylist[i] = pnt2d.Y;
//        }
//        RetV = UtilDRAW_PolyLine(&BufInfo, xlist, ylist, 2, 0, 3U, LineColor3);
//
//
//        for (UINT32 LineId = 0U; LineId < (AMBA_SR_LANE_LINE_MAX_NUM-1U); LineId++) {
//            UINT32 pNum;
//            DOUBLE yStart, yEnd, yFarthest, yFinal;
//
//            yStart = (g_WsFcwsCfg.CarSize[1]/2.0) + 2000.0;
//            if (EgoData.LineStat[AMBA_SR_LANE_LINE_CENTER] == AMBA_SR_LANE_LINE_STAT_VALID) {
//                yEnd = EgoData.Line3D[AMBA_SR_LANE_LINE_CENTER].PntData[EgoData.Line3D[AMBA_SR_LANE_LINE_CENTER].PntNum-1U].Y;
//            } else {
//                yEnd = 0.0;
//            }
//            yFarthest = g_FcTransferSpeed* KPH2SI* g_WsFcwsCfg.FCWSSpec.TTC_th[0]*g_WsFcwsCfg.FCWSSpec.PrewarnToColwarnRatio;
//            yFinal = (yEnd < yFarthest) ? yEnd : yFarthest;
//            yFinal = (yFinal > yLimit) ? yFinal : yLimit; // When yEnd = 0 and TransSpeed too low, not enough points to draw
//            pNum = 0U;
//            for (UINT32 pId = 0U; pId < AMBA_SR_MAX_PNT_NUM; pId++) {
//                yval = yStart + ((DOUBLE)pId * 3000.0);
//                pCoef = EgoData.LineFuncPara[LineId].Coef;
//                xval = pCoef[0] + (pCoef[1] * yval) + (pCoef[2] * yval * yval);
//
//                pnt3d.X = xval;
//                pnt3d.Y = yval;
//                pnt3d.Z = 0.0;
//                RetV = TransferWorld2SrcV1(pnt3d, &g_AdasNotifyCalibData->AdasCfgV1, &pnt2d);
//                if ((RetV != 0U) || (yval > yFinal)) {
//                    if (RetV != 0U) {
//                        AmbaPrint_PrintUInt5("UtilVSL_DrawFcwsWzV2, TransferWorld2SrcV1 failed", 0U, 0U, 0U, 0U, 0U);
//                    }
//                    break;
//                }
//                xlist[pId] = pnt2d.X;
//                ylist[pId] = pnt2d.Y;
//                pNum ++;
//            }
//
//            if (pNum > 1U) {
//                RetV = UtilDRAW_PolyLine(&BufInfo, xlist, ylist, (INT32)pNum, 0, 3U, LineColor2);
//            }
//        }
    }
}

static void WarningZoneUpdate(void)
{
    if (/*(1U == g_FcwsModeEnable) &&*/
        (1U == g_WarningZoneEnable)){
//        UINT32 RetVal;
//        RetVal = SvcOsd_DrawLine(g_AdasNotifyTargetVout,
//                 (UINT32)g_Zone1X[0], (UINT32)g_Zone1Y[0],
//                 (UINT32)g_Zone1X[1], (UINT32)g_Zone1Y[1],
//                 CALIB_Horizontal_Thickness, ADAS_StringColor);
//
//
//        RetVal = SvcOsd_DrawLine(g_AdasNotifyTargetVout,
//                 (UINT32)g_Zone1X[2], (UINT32)g_Zone1Y[2],
//                 (UINT32)g_Zone1X[3], (UINT32)g_Zone1Y[3],
//                 CALIB_Horizontal_Thickness, ADAS_StringColor);
//
//
//        if (SVC_NG == RetVal) {
//            SvcLog_NG(__func__, "WarningZoneUpdate() error !", 0U, 0U);
//        }

        /* Warning zone from ADAS team FC module report */
        if (1U == AdasNotifyCalibFromNAND) {
            WarningZoneFromAdasFC();
        }

    }


}

#define BarX   (875U)
#define BarY   (90U)
#define BarW   (50U)
#define BarH   (30U)
#define BarGap (2U)
static void AutoCalibProgress(void)
{
    if ((1U == SvcLdwsTask_GetAutoCalStatus()) || (1U == SvcLdwsTaskV2_GetAutoCalStatus())) {
        UINT32 RetVal;
        UINT32 Rlen = 0, Rval = 0U;
        char   DoubleBuf[32U];
        DOUBLE CollectRate = ((g_DataCollectRatio) / 100.0)*((DOUBLE)BarW);

        RetVal = SvcOsd_DrawString(g_AdasNotifyTargetVout, 810U, 100U,
                                 2U, 0xFF000000U, "AutoCal:");
        RetVal |= SvcOsd_DrawLine(g_AdasNotifyTargetVout, BarX,      BarY,      BarX+BarW, BarY,      3U, 0xFF000000U);
        RetVal |= SvcOsd_DrawLine(g_AdasNotifyTargetVout, BarX,      BarY+BarH, BarX+BarW, BarY+BarH, 3U, 0xFF000000U);
        RetVal |= SvcOsd_DrawLine(g_AdasNotifyTargetVout, BarX,      BarY,      BarX,      BarY+BarH, 3U, 0xFF000000U);
        RetVal |= SvcOsd_DrawLine(g_AdasNotifyTargetVout, BarX+BarW, BarY,      BarX+BarW, BarY+BarH, 3U, 0xFF000000U);

        SVC_WRAP_SNPRINT "%4f%"
                         SVC_SNPRN_ARG_S DoubleBuf
                         SVC_SNPRN_ARG_DOUBLE g_DataCollectRatio
                         SVC_SNPRN_ARG_BSIZE sizeof(DoubleBuf)
                         SVC_SNPRN_ARG_RLEN &Rlen
                         SVC_SNPRN_ARG_E
         DoubleBuf[3] = '%';
         DoubleBuf[4] = '\0';
         Rval = SvcOsd_DrawString(g_AdasNotifyTargetVout, BarX, (BarY + BarH + BarGap),
                                  2U, 0xffffffffU, DoubleBuf);
         AmbaMisra_TouchUnused(&Rval);

         if (g_AutoCalResult == AUTOCAL_HDLR_CALIB_SUCCEED) {
             RetVal |= SvcOsd_DrawString(g_AdasNotifyTargetVout, BarX, (BarY+BarH+15U),
                                        1U, 0x8000ff00U, "Success");
             RetVal |= SvcOsd_DrawSolidRect(g_AdasNotifyTargetVout, (BarX+BarGap), (BarY+BarGap),
                                           ((BarX+BarW)), ((BarY+BarH)-BarGap), 0x8000ff00U);

         } else if (g_AutoCalResult == AUTOCAL_HDLR_CALIB_FAIL) {
             RetVal |= SvcOsd_DrawString(g_AdasNotifyTargetVout, BarX, (BarY+BarH+15U),
                                        1U, 0x80ff0000U, "Fail");
             RetVal |= SvcOsd_DrawSolidRect(g_AdasNotifyTargetVout, (BarX+BarGap), (BarY+BarGap),
                                           ((BarX+BarW)), ((BarY+BarH)-BarGap), 0x80ff0000U);

         } else {
             if ((UINT32)CollectRate > BarGap) {
                 RetVal |= SvcOsd_DrawSolidRect(g_AdasNotifyTargetVout, (BarX+BarGap), (BarY+BarGap),
                            (BarX+(UINT32)CollectRate), (BarY+BarH-BarGap), 0x8000ff00U);
             }
         }

        if (SVC_NG == RetVal) {
            SvcLog_NG(__func__, "AutoCalibProgress() error !", 0U, 0U);
        }
    }
}

static void SvcAdasNotify_MsgDrawCB(UINT32 VoutIdx, UINT32 Level)
{
    AmbaMisra_TouchUnused(&Level);//pass vcast
    AmbaMisra_TouchUnused(&VoutIdx);//pass vcast

    FcModeUpdate();
    CarSpeedUpdate();
    LdInfoUpdate();
    FcInfoUpdate();
    HorizontalLineUpdate();
    CalibHorizontalYUpdate();
    WarningZoneUpdate();
    AutoCalibProgress();
    WarningZoneQuickCalibAdj();
}

static void SvcAdasNotify_MsgDrawUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    if ((SvcAdasGuiUpdate & ADAS_GUI_SPEED_UPDATE) > 0U) {
        SvcAdasGuiUpdate &= ~(ADAS_GUI_SPEED_UPDATE);
        *pUpdate = 1U;
    }
    if ((SvcAdasGuiUpdate & ADAS_GUI_DIST_UPDATE) > 0U) {
        SvcAdasGuiUpdate &= ~(ADAS_GUI_DIST_UPDATE);
        *pUpdate = 1U;
    }
    if ((SvcAdasGuiUpdate & ADAS_GUI_FCWS_DIST_UPDATE) > 0U) {
        SvcAdasGuiUpdate &= ~(ADAS_GUI_FCWS_DIST_UPDATE);
        *pUpdate = 1U;
    }
    if ((SvcAdasGuiUpdate & ADAS_GUI_LDWS_DIST_R_UPDATE) > 0U) {
        SvcAdasGuiUpdate &= ~(ADAS_GUI_LDWS_DIST_R_UPDATE);
        *pUpdate = 1U;
    }
    if ((SvcAdasGuiUpdate & ADAS_GUI_LDWS_DIST_L_UPDATE) > 0U) {
        SvcAdasGuiUpdate &= ~(ADAS_GUI_LDWS_DIST_L_UPDATE);
        *pUpdate = 1U;
    }

    if ((SvcAdasGuiUpdate & ADAS_GUI_FCWS_TIME_UPDATE) > 0U) {
        SvcAdasGuiUpdate &= ~(ADAS_GUI_FCWS_TIME_UPDATE);
        *pUpdate = 1U;
    }

    if ((SvcAdasGuiUpdate & ADAS_GUI_QUICK_CALIB_ADJ) > 0U) {
        SvcAdasGuiUpdate &= ~(ADAS_GUI_QUICK_CALIB_ADJ);
        *pUpdate = 1U;
    }
}

/**
 *  Svc notify draw init function
 */
void SvcAdasNotify_DrawInit(void)
{
    static SVC_USER_PREF_s  *pSvcUserPref;

    if (SVC_OK == SvcUserPref_Get(&pSvcUserPref)) {//TTC mode or position mode
        g_FcwsMode = pSvcUserPref->FcwsMode;
    } else {
        g_FcwsMode = AMBA_WS_FCWS_MODE_POSITION;
        SvcLog_NG(__func__, "SvcAdasNotify_DrawInit() Somthing wrong of the FcwsMode", 0U, 0U);
    }

    g_SubjectLaneInfoIdx = 0U;
    g_FcTransferSpeed    = 0.0;
    AmbaSvcWrap_MisraMemset(&g_WsFcwsCfg, 0, sizeof(AMBA_WS_FCWS_DATA_s));
    AmbaSvcWrap_MisraMemset(&g_SubjectLaneInfo, 0, sizeof(AMBA_SR_LANE_MEMBER_s)*(SVC_FC_WARNINGZONE_OUTPUT_DEPTH));
    AmbaSvcWrap_MisraMemset(&g_SegCropInfo, 0, sizeof(AMBA_SEG_CROP_INFO_s));

    AdasNotifyCalibFromNAND = SvcAdasNotify_GetCalibrationData();
    if (1U == AdasNotifyCalibFromNAND) {
        SvcLog_OK(__func__, "SvcFc_GetCalibrationData have calibration data!", 0U, 0U);
    } else {
        SvcLog_OK(__func__, "SvcFc_GetCalibrationData No calibration data!", 0U, 0U);
    }

    if (g_AdasNotifyTargetVout == VOUT_IDX_A) {
        AdasNotifyGuiLevel = (UINT32)SVC_NOTIFY_GUI_DRAW_MSG +1U;
    } else { //VOUT B and other
        AdasNotifyGuiLevel = (UINT32)SVC_NOTIFY_GUI_DRAW_MSG;
    }
    SvcGui_Register(g_AdasNotifyTargetVout, AdasNotifyGuiLevel, "Adas", SvcAdasNotify_MsgDrawCB, SvcAdasNotify_MsgDrawUpdate);
}

/**
 *  Svc notify draw deinit function
 */
void SvcAdasNotify_DrawDeInit(void)
{
    SvcGui_Unregister(g_AdasNotifyTargetVout, AdasNotifyGuiLevel);
}

/**
 *  Drawing current when system running decode mode
 *  @param[in] Speed Car speed
 */
void SvcAdasNotify_DecModeDrawSpeed(DOUBLE Speed)
{
    IsDecodeMode = 1U;
    DecodeModeSpeed = Speed;
}

/**
 *  Enable/Disable ADAS FC mode draw (TTC or position)
 *  @param[in] Enable/Disable
 */
void SvcAdasNotify_DrawFcMode(UINT32 Enable)
{
    g_FcwsModeEnable = Enable;
}

/**
 *  Svc notify Chck system have calibration data or not
 *  @return error code
 */
UINT32 SvcAdasNotify_CalibCheck(void)
{
    UINT32 Rval;
    static AMBA_CAL_EM_CALC_COORD_CFG_s Cfg;
    static AMBA_CAL_EM_CAM_CALIB_DATA_s CalibDataRaw2World;

    Cfg.TransCfg.pCalibDataRaw2World = &CalibDataRaw2World;
    Rval = SvcCalib_AdasCfgGet(SVC_CALIB_ADAS_TYPE_FRONT, &Cfg);

    return Rval;
}

/**
 *  Update WarningZone
 *  @param[in] X 4 point
 *  @param[in] Y 4 point
 *  @param[in] Size of Zone1X
 *  @param[in] Size of Zone1Y
 */
void SvcAdasNotify_WarningZoneUpdate(const INT32 Zone1X[4], const INT32 Zone1Y[4], UINT32 SizeZone1X, UINT32 SizeZone1Y)
{
    AmbaSvcWrap_MisraMemcpy(g_Zone1X, Zone1X, SizeZone1X);
    AmbaSvcWrap_MisraMemcpy(g_Zone1Y, Zone1Y, SizeZone1Y);
}

/**
 *  Update HorizontalLine
 *  @param[in] HorizontalLine Info
 *  @return error code
 */
UINT32 SvcAdasNotify_HorizontalLineUpdate(const SVC_CV_DISP_OSD_CMD_s *pInfo)
{
    UINT32   RetVal;

    RetVal = AmbaWrap_memcpy(&g_HorizontalLine, pInfo, sizeof(SVC_CV_DISP_OSD_CMD_s));
    if (SVC_OK != RetVal) {
        SvcLog_NG(__func__, "line %u [Error] HorizontalLineUpdate return %u", __LINE__, RetVal);
    }

    return RetVal;
}

/**
 *  Set calibration HorizontalLine
 *  @param[in] Enable
 *  @param[in] Horizontal (Y) value
 *  @return HorizonY
 */
UINT32 SvcAdasNotify_SetCalibHorizontal(UINT32 Enable, UINT32 Y)
{
    UINT32  HorizonY = 0U, Rval = 0U;
    UINT32  OsdBufWidth, OsdBufHeight;
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
    UINT32 RawHeight;

    g_CalibEnable = Enable;
    Rval = SvcOsd_GetOsdBufSize(g_AdasNotifyTargetVout, &OsdBufWidth, &OsdBufHeight);
    AmbaMisra_TouchUnused(&Rval);
    if (Y == 0U) {
        g_CalibHorizontalX = 0U;
        g_CalibHorizontalY = 0U;
    } else {
        if (Y > (OsdBufHeight - CALIB_Horizontal_Thickness)) {
            g_CalibHorizontalX = (OsdBufWidth - CALIB_Horizontal_Thickness);
            g_CalibHorizontalY = (OsdBufHeight - CALIB_Horizontal_Thickness);
            SvcLog_NG(__func__, "Y > OsdBufHeight - Horizontal_Thickness (%d) > (%d)", Y, (OsdBufHeight - CALIB_Horizontal_Thickness));
        } else {
            g_CalibHorizontalX = OsdBufWidth - CALIB_Horizontal_Thickness;
            g_CalibHorizontalY = Y;
        }
        RawHeight     = pCfg->FovCfg[0U].RawWin.Height;
        HorizonY      = (Y*RawHeight)/OsdBufHeight;
    }

    return HorizonY;
}

/**
 *  Get calibration HorizontalLine
 *  @param[out] HorizonY
 */
UINT32 SvcAdasNotify_GetCalibHorizontal(void)
{
    return g_CalibHorizontalY;
}

/**
 *  Get WarningZoneX InPixel
 *  @param[in] X value
 *  @param[in] SenserWidth
 *  @return WarningZoneXinPixel
 */
UINT32 SvcAdasNotify_GetWarningZoneX_InPixel(INT32 WarningZoneX, UINT32 SenserWidth)
{

    UINT32  WarningZoneXinPixel = 0U, Rval = 0U;
    UINT32  OsdBufWidth, OsdBufHeight;

    Rval = SvcOsd_GetOsdBufSize(g_AdasNotifyTargetVout, &OsdBufWidth, &OsdBufHeight);
    AmbaMisra_TouchUnused(&Rval);
    if ((UINT32)WarningZoneX < (OsdBufWidth - CALIB_Horizontal_Thickness)) {
        WarningZoneXinPixel = (SenserWidth*((UINT32)WarningZoneX))/OsdBufWidth;
        AmbaPrint_PrintUInt5("GetWarningZoneX_InPixel(), WarningZoneXinPixel = %d, WarningZoneX = %d, SenserWidth = %d",
                                                         WarningZoneXinPixel, (UINT32)WarningZoneX, SenserWidth, 0U, 0U);
    } else {
        SvcLog_NG(__func__, "Error!! X too big, X = %d", (UINT32)WarningZoneX, 0U);
    }

    return WarningZoneXinPixel;
}

/**
 *  Enable/Disable WarningZone
 *  @param[in] Enable(1)/Disable(0)
 */
void SvcAdasNotify_WarningZoneEnable(UINT32 Enable)
{
    g_WarningZoneEnable = Enable;
}

/**
 *  Get WarningZone
 *  @param[in] WarningZone X
 *  @param[in] WarningZone Y
 */
void SvcAdasNotify_WarningZoneGet(INT32 *ZoneX, INT32 *ZoneY)
{
    AmbaSvcWrap_MisraMemcpy(ZoneX, g_Zone1X, sizeof(g_Zone1X));
    AmbaSvcWrap_MisraMemcpy(ZoneY, g_Zone1Y, sizeof(g_Zone1Y));
}

/**
 *  Set WarningZone
 *  @param[in] WarningZone X
 *  @param[in] WarningZone Y
 */
void SvcAdasNotify_WarningZoneSet(INT32 *ZoneX, INT32 *ZoneY, UINT32 SetEnable)
{
    AmbaSvcWrap_MisraMemcpy(g_Zone1X, ZoneX, sizeof(g_Zone1X));
    AmbaSvcWrap_MisraMemcpy(g_Zone1Y, ZoneY, sizeof(g_Zone1Y));
    g_QuickCalibZoneAdj = SetEnable;
    if (1U == SetEnable) {
        SvcAdasGuiUpdate |= ADAS_GUI_QUICK_CALIB_ADJ;
    }
    AmbaMisra_TouchUnused(ZoneX);//pass vcast
    AmbaMisra_TouchUnused(ZoneY);//pass vcast
}

static void* WarningZoneAutoTune_TaskEntry(void* EntryArg)
{
    UINT32 InitStatus, LoopU = 1U;
    static UINT8 StgChkCnt = 0U;
    UINT32 IsBreak = 0U;

    AmbaMisra_TouchUnused(&LoopU);
    AmbaMisra_TouchUnused(EntryArg);
    while (LoopU == 1U) {
        static SVC_USER_PREF_s     *pSvcUserPref;

        if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
            SvcLog_NG(__func__, "SvcUserPref_Get() failed!!", 0U, 0U);
        }

        if ((0.0 != pSvcUserPref->CalibFocalLength) &&
            (1U  == pSvcUserPref->WarningZoon_AutoRun)) {
            if (KAL_ERR_NONE != AmbaKAL_TaskSleep(1000)) {
                SvcLog_NG(__func__, "AmbaKAL_TaskSleep failed!", 0U, 0U);
            }
            InitStatus = SvcStgMonitor_StgInitStatus();
            StgChkCnt++;
            SvcLog_OK(__func__, "[WarningZoneAutoTune] StgChkCnt = %d ", StgChkCnt, 0U);
            if (1U == InitStatus) {
                ULONG  PrefBufAddr;
                UINT32 PrefBufSize;

                DOUBLE FocalLengthPrint = pSvcUserPref->CalibFocalLength;
                INT64  DoubleToINT64;
                UINT32 CalibFocalLengthIn_nm, Rval = 0U;
                FocalLengthPrint *= 1000000.0;/*mm to nm */
                DoubleToINT64 = (INT64)(FocalLengthPrint);
                AmbaMisra_TypeCast32(&CalibFocalLengthIn_nm, &DoubleToINT64);

                SvcLog_OK(__func__, "CalibFocalLength %d (nm), WarningZoon_AutoRun %d",
                                        CalibFocalLengthIn_nm,
                                        pSvcUserPref->WarningZoon_AutoRun);

                if (SVC_CALIB_ADAS_VERSION == SvcCalib_AdasCfgGetCalVer(SVC_CALIB_ADAS_TYPE_FRONT)) {
                    static char QuickCalibCmd[] = "svc_app adas warning_zone auto_tune";

                    /* Reset WarningZoon_AutoRun to 0*/
                    pSvcUserPref->WarningZoon_AutoRun = 0U;
                    SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
                    if (SVC_OK != SvcPref_Save(PrefBufAddr, PrefBufSize)) {
                        SvcLog_NG(__func__, "SvcPref_Save failed!!", 0U, 0U);
                    }
                    Rval = AmbaShell_ExecCommand(QuickCalibCmd);
                    AmbaMisra_TouchUnused(&Rval);
                    AmbaPrint_PrintStr5("[WarningZoneAutoTune_TaskEntry] Auto Tune start, cmd: %s ", QuickCalibCmd, NULL, NULL, NULL, NULL);
                } else if (SVC_CALIB_ADAS_VERSION_V1 == SvcCalib_AdasCfgGetCalVer(SVC_CALIB_ADAS_TYPE_FRONT)) {
                    static char QuickCalibCmd_V1[] = "svc_app adas warning_zone auto_tune_V1";

                    /* Reset WarningZoon_AutoRun to 0*/
                    pSvcUserPref->WarningZoon_AutoRun = 0U;
                    SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
                    if (SVC_OK != SvcPref_Save(PrefBufAddr, PrefBufSize)) {
                        SvcLog_NG(__func__, "SvcPref_Save failed!!", 0U, 0U);
                    }
                    Rval = AmbaShell_ExecCommand(QuickCalibCmd_V1);
                    AmbaMisra_TouchUnused(&Rval);
                    AmbaPrint_PrintStr5("[WarningZoneAutoTune_TaskEntry] Auto Tune start, cmd: %s", QuickCalibCmd_V1, NULL, NULL, NULL, NULL);
                } else {
                    /* Reset WarningZoon_AutoRun to 0*/
                    pSvcUserPref->WarningZoon_AutoRun = 0U;
                    SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
                    if (SVC_OK != SvcPref_Save(PrefBufAddr, PrefBufSize)) {
                        SvcLog_NG(__func__, "SvcPref_Save failed!!", 0U, 0U);
                    }
                    SvcLog_NG(__func__, "Error! No ADAS calib version,  version = 0x%X", SvcCalib_AdasCfgGetCalVer(SVC_CALIB_ADAS_TYPE_FRONT), 0U);
                }

                //Finish WarningZoon_AutoRun task
                IsBreak = 1U;
            }
        } else {
            SvcLog_OK(__func__, "No need to do auto tune, WarningZoon_AutoRun = %d",
                            pSvcUserPref->WarningZoon_AutoRun, 0U);

            SVC_WRAP_PRINT "No need to do auto tune, CalibFocalLength = %5f"
            SVC_PRN_ARG_S __func__
            SVC_PRN_ARG_PROC SvcLog_OK
            SVC_PRN_ARG_DOUBLE  pSvcUserPref->CalibFocalLength
            SVC_PRN_ARG_E
            IsBreak = 1U;
        }

        if (StgChkCnt > 5U) {
            ULONG  PrefBufAddr;
            UINT32 PrefBufSize;
            SvcLog_OK(__func__, "Exit while loop if StgChkCnt > 5U (only check StgChkCnt 5 times)", 0U, 0U);
            /* Reset WarningZoon_AutoRun to 0*/
            pSvcUserPref->WarningZoon_AutoRun = 0U;
            SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
            if (SVC_OK != SvcPref_Save(PrefBufAddr, PrefBufSize)) {
                SvcLog_NG(__func__, "SvcPref_Save failed!!", 0U, 0U);
            }
            IsBreak = 1U;
        }

        if (1U == IsBreak) {break;}
    }

    return NULL;
}

/**
 *  Warning Zone Auto Tune Init
 */
#define SVC_WARNING_ZONE_AUTO_TUNE_TASK_STACK_SIZE       (0x6000U)
void SvcAdasNotify_WarningZoneAutoTuneInit(void)
{
    UINT32 Rval;
    static SVC_TASK_CTRL_s  TaskCtrl GNU_SECTION_NOZEROINIT;
    static UINT8            WarningZoneAutoTuneStack[SVC_WARNING_ZONE_AUTO_TUNE_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;

    TaskCtrl.Priority    = (150U);
    TaskCtrl.EntryFunc   = WarningZoneAutoTune_TaskEntry;
    TaskCtrl.EntryArg    = 0U;
    TaskCtrl.pStackBase  = WarningZoneAutoTuneStack;
    TaskCtrl.StackSize   = SVC_WARNING_ZONE_AUTO_TUNE_TASK_STACK_SIZE;
    TaskCtrl.CpuBits     = (0x01U);

    Rval =  SvcTask_Create("WarningZoneAutoTuneTask", &TaskCtrl);

    if (Rval == SVC_OK) {
        SvcLog_OK(__func__, "WarningZoneAutoTune_TaskEntry created!", 0U, 0U);
    } else {
        SvcLog_NG(__func__, "fail to create WarningZoneAutoTune_TaskEntry!", 0U, 0U);
    }
}

/**
 *  UpdateAutoCalDataCollectRatio
 *  @param[in] Auto calibration DataCollect Ratio
 *  @param[in] Auto calibration final result
 */
void SvcAdasNotify_UpdateAutoCalDataCollectRatio(DOUBLE DataCollectRatio, UINT32 AutoCalResult)
{
    g_DataCollectRatio = DataCollectRatio;
    g_AutoCalResult    = AutoCalResult;
}

/**
 *  SvcAdasNotify_SetACCRequiredSpeed
 *  @param[in] Setup ACC RequiredSpeed
 */
void SvcAdasNotify_SetACCRequiredSpeed(DOUBLE Speed)
{
    g_RequiredSpeed = Speed;
}

/**
 *  SvcAdasNotify_SetACCAcceleration
 *  @param[in] Setup ACC Acceleration
 */
void SvcAdasNotify_SetACCAcceleration(DOUBLE Acceleration)
{
    g_Acceleration = Acceleration;
}

/**
 *  SvcAdasNotify_UpdateFCWarningZone
 *  @param[in] Update WarningZone from FC module
 */
void SvcAdasNotify_UpdateFCWarningZone(const AMBA_SR_LANE_MEMBER_s *pSubjectLaneInfo)
{
    AmbaSvcWrap_MisraMemcpy(&g_SubjectLaneInfo[g_SubjectLaneInfoIdx], pSubjectLaneInfo, sizeof(AMBA_SR_LANE_MEMBER_s));
    g_SubjectLaneInfoIdx++;
    /* Reset g_LaneInfoIdx to 0U */
    if (g_SubjectLaneInfoIdx >= SVC_FC_WARNINGZONE_OUTPUT_DEPTH) {
        g_SubjectLaneInfoIdx = 0U;
    }
}

/**
 *  SvcAdasNotify_UpdateFCInitInfo
 *  @param[in] Update FC Init Info
 */
void SvcAdasNotify_UpdateFCInitInfo(const AMBA_WS_FCWS_DATA_s *pWsFcwsCfg)
{
    AmbaSvcWrap_MisraMemcpy(&g_WsFcwsCfg, pWsFcwsCfg, sizeof(AMBA_WS_FCWS_DATA_s));
}

/**
 *  SvcAdasNotify_UpdateFCCurrentTransferSpeed
 *  @param[in] Update Fc Current Transfer Speed
 */
void SvcAdasNotify_UpdateFCCurrentTransferSpeed(DOUBLE  FcTransferSpeed)
{
    g_FcTransferSpeed = FcTransferSpeed;
}

/**
 *  SvcAdasNotify_UpdateLDSegCropInfo
 *  @param[in] Update LD Seg Crop Info
 */
void SvcAdasNotify_UpdateLDSegCropInfo(const AMBA_SEG_CROP_INFO_s  *pSegCropInfo)
{
    AmbaSvcWrap_MisraMemcpy(&g_SegCropInfo, pSegCropInfo, sizeof(AMBA_SEG_CROP_INFO_s));
}
