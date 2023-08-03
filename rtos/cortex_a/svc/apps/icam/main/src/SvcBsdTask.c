/**
 *  @file SvcBsdTask.c
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
 *  @details svc Object Detection task
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
#include "AmbaWS_BSD.h"
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaSYS.h"
#include "AmbaImg_Proc.h"
#include "AmbaFS.h"
#include "AmbaShell.h"

#include "RefFlow_BSD.h"

/* svc-framework */
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"

/* svc-shared */
#include "SvcGui.h"
#include "SvcResCfg.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcBufMap.h"
#include "SvcOsd.h"

#include "SvcCvAlgo.h"
#include "SvcCvAppDef.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlow_Comm.h"
#include "SvcCvFlow_AmbaOD.h"
#include "SvcCvFlow_AmbaSeg.h"
#include "SvcCvFlow_RefOD.h"
#include "SvcCvFlow_RefSeg.h"

/* svc-icam */
#include "SvcFlowControl.h"
#include "SvcSysStat.h"
#include "SvcAppStat.h"
#include "SvcBsdTask.h"
#include "SvcCan.h"
#include "SvcUserPref.h"
#include "SvcPref.h"
#include "SvcODDrawTask.h"
#include "SvcCalibAdas.h"
#include "SvcResCfgTask.h"
#include "SvcDataGather.h"
#include "SvcVinSrc.h"
#include "SvcTask.h"
#include "SvcLogoDraw.h"

#define SAVE_OD_RESULT (0U)
#if SAVE_OD_RESULT == 1U
#define SAVE_FPS (12U)
#define SAVE_FRAMES (65U)
#define SAVE_OD_SIZE (3016U) /* 4*4 + 150*<2*6 + 4*2> */
#define SAVE_OD_BUFFER_SIZE (SAVE_OD_SIZE*65U*12U)
#define SAVE_CAN_SIZE (48U) /* 1*3 + 4*2 + 8*3 */
#define SAVE_CAN_BUFFER_SIZE (SAVE_CAN_SIZE*65U*12U)
#endif

#define BSD_CAN_GUI_LEVEL (6U)//SVC_GUI_LEVEL_BSD_CAN

#define BSD_CHAN_NUM      (2U)

#define SVC_LOG_BSD_TASK     "BSD_TASK"

/* Definition for BSD */
static AMBA_WS_BSD_DATA_s *pWsBsdCfg;

/* Definition for OSD */
static AMBA_CAL_EM_CALC_COORD_CFG_s *CalibData[BSD_CHAN_NUM] GNU_SECTION_NOZEROINIT;
static UINT32 CALIB_INIT_FLAG[BSD_CHAN_NUM] =  {0U};

static UINT8   Previous_CANBusTurnLightStatus[BSD_CHAN_NUM] = {0};
static UINT32  Previous_BsdEvent[BSD_CHAN_NUM] = {0};
static UINT32  HorizonPos[BSD_CHAN_NUM][2];
static UINT32  TotalOsdWidth;
static UINT32  OsdWidth, OsdHeight;
static UINT32  OsdOffset;
static UINT32  BbxStarted = 0U;
static UINT32  BsdVoutChan = VOUT_IDX_B;
static AMBA_DSP_WINDOW_s DispSrcWin[BSD_CHAN_NUM];
static UINT32  BsdOsdUpdate = 0U;
static UINT32  BsdWithClipCanBus = TRUE;
static UINT32  BsdRefDistLine = FALSE;

/* BSD output */
void  BsdTask_DetectionResultCallback(UINT32 Chan, UINT32 CvType, void *pDetResMsg);   /* The callback for DetRes (Detection Result) */
static void SvcBsd_GetLeftCalData(void);
static void SvcBsd_GetRightCalData(void);
void SvcBsd_BbxTransSvcToWs (AMBA_OD_2DBBX_LIST_s *pBbxdataWS, const SVC_CV_DETRES_BBX_LIST_s *pList);

//BSD OSD
static void SvcBsd_OsdDraw(UINT32 VoutIdx, UINT32 Level);
static void SvcBsd_OsdUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate);
void SvcBsdTask_HorizonPosition(UINT32 CvFovIdx);
static void BsdTask_StatusCallback(UINT32 StatIdx, void *pInfo);
UINT32 SvcBSDTask_MainToOsdDomain(INT32 Value, UINT32 Main, UINT32 OsdInput, UINT32 Flip, UINT32 Offset);
static UINT32 GetWarnZonePnts(
    const AMBA_CAL_EM_CALC_COORD_CFG_s *pCalCfg,
    const AMBA_WS_BSD_DATA_s *pBsdCfg,
    const AMBA_CAL_ROI_s *pDrawRoi,
    INT32 Zone1X[4], INT32 Zone1Y[4], INT32 Zone2X[4], INT32 Zone2Y[4]);

static void Svc_PrintNg(const char *pFormat, UINT32 Line, UINT32 RVAL)
{
    if(RVAL != SVC_OK) {
        SvcLog_NG(pFormat, "Line %d RVAL %d", Line, RVAL);
    }
}


#if SAVE_OD_RESULT
static void Save2DBbxResult(const AMBA_OD_2DBBX_LIST_s *OdResult)
{
    UINT32 RValue ;

    /** Log buf variable */
    static UINT8 odResLogBuf[SAVE_OD_BUFFER_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT32 addrOfs = 0;

    if (addrOfs < SAVE_OD_BUFFER_SIZE){
        UINT8 *addr;
        UINT32 NumBbx;

        addr = odResLogBuf;
        addr += addrOfs;

        NumBbx = (OdResult->NumBbx > AMBA_OD_2DBBX_MAX_BBX_NUM)? (AMBA_OD_2DBBX_MAX_BBX_NUM):(OdResult->NumBbx);

        AmbaSvcWrap_MisraMemcpy(addr, &(OdResult->MsgCode), sizeof(UINT32));        addr += sizeof(UINT32);
        AmbaSvcWrap_MisraMemcpy(addr, &(OdResult->CaptureTime), sizeof(UINT32));    addr += sizeof(UINT32);
        AmbaSvcWrap_MisraMemcpy(addr, &(OdResult->FrameNum), sizeof(UINT32));       addr += sizeof(UINT32);
        AmbaSvcWrap_MisraMemcpy(addr, &NumBbx, sizeof(UINT32));                     addr += sizeof(UINT32);

        for (UINT32 i = 0; i < NumBbx; i++){
            AmbaSvcWrap_MisraMemcpy(addr, &(OdResult->Bbx[i].Cat), sizeof(UINT16));         addr += sizeof(UINT16);
            AmbaSvcWrap_MisraMemcpy(addr, &(OdResult->Bbx[i].Fid), sizeof(UINT16));         addr += sizeof(UINT16);
            AmbaSvcWrap_MisraMemcpy(addr, &(OdResult->Bbx[i].ObjectId), sizeof(UINT32));    addr += sizeof(UINT32);
            AmbaSvcWrap_MisraMemcpy(addr, &(OdResult->Bbx[i].Score), sizeof(UINT32));       addr += sizeof(UINT32);
            AmbaSvcWrap_MisraMemcpy(addr, &(OdResult->Bbx[i].X), sizeof(UINT16));           addr += sizeof(UINT16);
            AmbaSvcWrap_MisraMemcpy(addr, &(OdResult->Bbx[i].Y), sizeof(UINT16));           addr += sizeof(UINT16);
            AmbaSvcWrap_MisraMemcpy(addr, &(OdResult->Bbx[i].W), sizeof(UINT16));           addr += sizeof(UINT16);
            AmbaSvcWrap_MisraMemcpy(addr, &(OdResult->Bbx[i].H), sizeof(UINT16));           addr += sizeof(UINT16);
        }

        /** print result for debug */
#if 0
        {
            AmbaPrint_PrintUInt5("MsgCode=%d, CaptureTime=%d, FrameNum=%d, NumBbx=%d", OdResult->MsgCode, OdResult->CaptureTime, OdResult->FrameNum, OdResult->NumBbx, 0U);
            for (UINT32 i = 0; i < NumBbx; i++){
                AmbaPrint_PrintUInt5("[Cat, Fid, ObjId, Score] = [%d, %d, %d, %d]", OdResult->Bbx[i].Cat, OdResult->Bbx[i].Fid, OdResult->Bbx[i].ObjectId, OdResult->Bbx[i].Score, 0U);
                AmbaPrint_PrintUInt5("[X,Y,W,H]=[%d, %d, %d, %d]", OdResult->Bbx[i].X, OdResult->Bbx[i].Y, OdResult->Bbx[i].W, OdResult->Bbx[i].H, 0U);
            }
        }
#endif

        addrOfs += SAVE_OD_SIZE;
        AmbaPrint_PrintUInt5("[Log OD] addrOfs = %d (%d)", addrOfs, SAVE_OD_BUFFER_SIZE, 0U, 0U, 0U);
    } else {
        char Filename[64];
        UINT32 Arg[1];
        static UINT8 fileCnt = 0;
        AMBA_FS_FILE *Out = NULL;
        char FMode[] = "w+";

        Arg[0] = fileCnt;
        (void)AmbaUtility_StringPrintUInt32(Filename,64,"c:\\BbxBuf%02u.bin",1,Arg);
        AmbaPrint_PrintStr5("File open file name %s", Filename, NULL, NULL, NULL, NULL);

        RValue = AmbaFS_FileOpen(Filename, FMode, &Out);
        if (RValue != AMBA_FS_ERR_NONE){
            AmbaPrint_PrintUInt5("Open file error %d", RValue, 0U, 0U, 0U, 0U);
        }else{
            (void)AmbaFS_FileWrite(odResLogBuf, 1, SAVE_OD_BUFFER_SIZE, Out, &RValue);
            (void)AmbaFS_FileClose(Out);
            AmbaPrint_PrintStr5("Done to save file", NULL, NULL, NULL, NULL, NULL);
        }

        addrOfs = 0;
        fileCnt++;
    }

}

static void SaveCanBus(const AMBA_SR_CANBUS_TRANSFER_DATA_s *pCan){
    UINT32 RValue ;

    /** Log buf variable */

    static UINT8 LogBuf[SAVE_CAN_BUFFER_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT32 addrOfs = 0;

    if (sizeof(AMBA_SR_CANBUS_TRANSFER_DATA_s) != SAVE_CAN_SIZE ){
        AmbaPrint_PrintUInt5("[Log CAN] Data size define error!! (real = %d, define = %d)", sizeof(AMBA_SR_CANBUS_TRANSFER_DATA_s), SAVE_CAN_SIZE, 0U, 0U, 0U);
    }

    if (addrOfs < SAVE_CAN_BUFFER_SIZE){
        AmbaSvcWrap_MisraMemcpy(LogBuf + addrOfs, pCan, sizeof(AMBA_SR_CANBUS_TRANSFER_DATA_s));
        addrOfs += SAVE_CAN_SIZE;
    } else {
        char Filename[64];
        UINT32 Arg[1];
        static UINT8 fileCnt = 0;
        AMBA_FS_FILE *Out = NULL;
        char FMode[] = "w+";

        Arg[0] = fileCnt;
        (void)AmbaUtility_StringPrintUInt32(Filename,64,"c:\\CanBusBuf%02u.bin",1,Arg);
        AmbaPrint_PrintStr5("File open file name %s", Filename, NULL, NULL, NULL, NULL);

        RValue = AmbaFS_FileOpen(Filename, FMode, &Out);
        if (RValue != AMBA_FS_ERR_NONE){
            AmbaPrint_PrintUInt5("Open file error %d", RValue, 0U, 0U, 0U, 0U);
        }else{
            (void)AmbaFS_FileWrite(LogBuf, 1, SAVE_CAN_BUFFER_SIZE, Out, &RValue);
            (void)AmbaFS_FileClose(Out);
            AmbaPrint_PrintStr5("Done to save file", NULL, NULL, NULL, NULL, NULL);
        }

        addrOfs = 0;
        fileCnt++;
    }
}
#endif

static AMBA_OD_2DBBX_LIST_s BbxOut GNU_SECTION_NOZEROINIT;
static AMBA_SR_SROBJECT_DATA_s SROuput GNU_SECTION_NOZEROINIT;

static UINT32 GetResult(RF_LOG_HEADER_s *pHeader) {
    UINT32 DataFmt = pHeader->DataFmt;
    UINT32 DataSize = pHeader->DataSize;
    const void* pDataAddr = pHeader->pDataAddr;
    UINT32 RVAL;

    AmbaMisra_TouchUnused(pHeader);

    if (DataFmt == RF_BSD_DFMT_2DBBX_OUT) {
        const AMBA_OD_2DBBX_LIST_s *pAddr; ;

        AmbaMisra_TypeCast(&pAddr, &pDataAddr);
        RVAL = AmbaWrap_memcpy(&BbxOut, pAddr, DataSize);
        Svc_PrintNg(__func__, __LINE__, RVAL);
    }

    if (DataFmt == RF_BSD_DFMT_SR) {
        const AMBA_SR_SROBJECT_DATA_s *pAddr ;

        AmbaMisra_TypeCast(&pAddr, &pDataAddr);
        RVAL = AmbaWrap_memcpy(&SROuput, pAddr, DataSize);
        Svc_PrintNg(__func__, __LINE__, RVAL);
    }

    return ADAS_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcBsdTask_Init
 *
 *  @Description:: Init the object detection module
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: void
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcBsdTask_Init(void)
{
    UINT32 Rval;
    UINT32 Size = 0U;
    static UINT8 *WrkBufAddr;
    static REF_FLOW_BSD_CFG_s RfBsdCfg GNU_SECTION_NOZEROINIT;
    const SVC_RES_CFG_s    *pCfg = SvcResCfg_Get();
    UINT32 TotalOsdHeight;
    ULONG  CacheBase;
    UINT32 CacheSize = 0U;

    SVC_CV_ROI_INFO_s RoiInfo[RF_BSD_CHANNEL_NUM];

    if(SVC_OK == SvcBSDTask_IsLiveView()) {
        //LV mode
        if (SvcBsdTask_GetBsdChan() == BSD_BOTH) {
            //2CV
            Rval = SvcCvFlow_Control(RF_BSD_CHANNEL_LEFT, SVC_CV_CTRL_GET_ROI_INFO, &RoiInfo[RF_BSD_CHANNEL_LEFT]);
            Svc_PrintNg(__func__, __LINE__, Rval);
            Rval = SvcCvFlow_Control(RF_BSD_CHANNEL_RIGHT, SVC_CV_CTRL_GET_ROI_INFO, &RoiInfo[RF_BSD_CHANNEL_RIGHT]);
            Svc_PrintNg(__func__, __LINE__, Rval);
        } else {
            //1CV
            Rval = SvcCvFlow_Control(0U, SVC_CV_CTRL_GET_ROI_INFO, &RoiInfo[RF_BSD_CHANNEL_LEFT]);
            Svc_PrintNg(__func__, __LINE__, Rval);
            Rval = SvcCvFlow_Control(0U, SVC_CV_CTRL_GET_ROI_INFO, &RoiInfo[RF_BSD_CHANNEL_RIGHT]);
            Svc_PrintNg(__func__, __LINE__, Rval);
        }
    } else {
        //decode mode, 1CV
        Rval = SvcCvFlow_Control(0U, SVC_CV_CTRL_GET_ROI_INFO, &RoiInfo[RF_BSD_CHANNEL_LEFT]);
        Svc_PrintNg(__func__, __LINE__, Rval);
        Rval = SvcCvFlow_Control(0U, SVC_CV_CTRL_GET_ROI_INFO, &RoiInfo[RF_BSD_CHANNEL_RIGHT]);
        Svc_PrintNg(__func__, __LINE__, Rval);
    }

    Rval = AmbaWrap_memset(&BbxOut, 0, sizeof(BbxOut));
    Svc_PrintNg(__func__, __LINE__, Rval);
    Rval = AmbaWrap_memset(&SROuput, 0, sizeof(SROuput));
    Svc_PrintNg(__func__, __LINE__, Rval);
    Rval = AmbaWrap_memset(&RfBsdCfg, 0, sizeof(RfBsdCfg));
    Svc_PrintNg(__func__, __LINE__, Rval);

    Rval = SvcBuffer_Request(SVC_BUFFER_SHARED,
               SMEM_PF0_ID_ADAS_BSD,
               &CacheBase,
               &CacheSize);
    Svc_PrintNg(__func__, __LINE__, Rval);

    Rval = RefFlowBSD_GetDefaultCfg(&Size, &RfBsdCfg);
    if (Rval != ADAS_ERR_NONE) {
        AmbaPrint_PrintStr5("%s, RefFlowBSD_GetDefaultCfg failed", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaPrint_PrintUInt5("working buffer size = %d", Size, 0U, 0U, 0U, 0U);


    /** ----------------------------------------------------------------- */
    /** Cfg for SR                                                        */
    /** ----------------------------------------------------------------- */
    if (Rval == ADAS_ERR_NONE){
        UINT32 cnt;

        RfBsdCfg.ODRoi[RF_BSD_CHANNEL_LEFT].StartX  = RoiInfo[RF_BSD_CHANNEL_LEFT].Roi[0].StartX;   /** refer to ambaOd_emr roi setting (BSD_LEFT). 52*1920/1360 */
        RfBsdCfg.ODRoi[RF_BSD_CHANNEL_LEFT].StartY  = RoiInfo[RF_BSD_CHANNEL_LEFT].Roi[0].StartY;   /** refer to ambaOd_emr roi setting (BSD_LEFT). 70*1080/768 */
        RfBsdCfg.ODRoi[RF_BSD_CHANNEL_LEFT].Width   = RoiInfo[RF_BSD_CHANNEL_LEFT].Roi[0].Width;     /** refer to ambaOd_emr roi setting (BSD_LEFT). 1280*1920/1360 */
        RfBsdCfg.ODRoi[RF_BSD_CHANNEL_LEFT].Height  = RoiInfo[RF_BSD_CHANNEL_LEFT].Roi[0].Height;    /** refer to ambaOd_emr roi setting (BSD_LEFT). 640*1080/768 */
        RfBsdCfg.ODRoi[RF_BSD_CHANNEL_RIGHT].StartX = RoiInfo[RF_BSD_CHANNEL_RIGHT].Roi[0].StartX;  /** refer to ambaOd_emr roi setting (BSD_RIGHT). 52*1920/1360 */
        RfBsdCfg.ODRoi[RF_BSD_CHANNEL_RIGHT].StartY = RoiInfo[RF_BSD_CHANNEL_RIGHT].Roi[0].StartY;  /** refer to ambaOd_emr roi setting (BSD_RIGHT). 70*1080/768 */
        RfBsdCfg.ODRoi[RF_BSD_CHANNEL_RIGHT].Width  = RoiInfo[RF_BSD_CHANNEL_RIGHT].Roi[0].Width;    /** refer to ambaOd_emr roi setting (BSD_RIGHT). 1280*1920/1360 */
        RfBsdCfg.ODRoi[RF_BSD_CHANNEL_RIGHT].Height = RoiInfo[RF_BSD_CHANNEL_RIGHT].Roi[0].Height;   /** refer to ambaOd_emr roi setting (BSD_RIGHT). 640*1080/768 */

        cnt = 0U;
        // car
        //AMBANET_OD37_EM_CAT4_CAR                (4U)
        RfBsdCfg.NNCat2ObjType[cnt].NNCat = 4U;                         /** NN category number for car */
        RfBsdCfg.NNCat2ObjType[cnt].ObjType = SR_OBJ_TYPE_VEHICLE_0;    /** set SR object type corresponding NN categroy */
        cnt++;

        //AMBANET_OD37_EM_CAT5_TRUCK              (5U)
        RfBsdCfg.NNCat2ObjType[cnt].NNCat = 5U;                         /** NN category number for car */
        RfBsdCfg.NNCat2ObjType[cnt].ObjType = SR_OBJ_TYPE_VEHICLE_0;    /** set SR object type corresponding NN categroy */
        cnt++;

        // cyclist
        //AMBANET_OD37_EM_CAT2_BICYCLE            (2U)
        RfBsdCfg.NNCat2ObjType[cnt].NNCat = 2U;                         /** NN category number for cyclist */
        RfBsdCfg.NNCat2ObjType[cnt].ObjType = SR_OBJ_TYPE_SCOOTER_0;    /** set SR object type corresponding NN categroy */
        cnt++;

        //AMBANET_OD37_EM_CAT3_MOTORCYCLE         (3U)
        RfBsdCfg.NNCat2ObjType[cnt].NNCat = 3U;                         /** NN category number for cyclist */
        RfBsdCfg.NNCat2ObjType[cnt].ObjType = SR_OBJ_TYPE_SCOOTER_0;    /** set SR object type corresponding NN categroy */
        cnt++;

        // person
        //AMBANET_OD37_EM_CAT0_PERSON             (0U)
        RfBsdCfg.NNCat2ObjType[cnt].NNCat = 0U;                         /** NN category number for person */
        RfBsdCfg.NNCat2ObjType[cnt].ObjType = SR_OBJ_TYPE_PERSON_0;     /** set SR object type corresponding NN categroy */
        cnt++;

        //AMBANET_OD37_EM_CAT1_RIDER              (1U)                  <== Ignore Rider on BSD now
        //RfBsdCfg.NNCat2ObjType[cnt].NNCat = 1U;                       /** NN category number for person */
        //RfBsdCfg.NNCat2ObjType[cnt].ObjType = SR_OBJ_TYPE_PERSON_0;   /** set SR object type corresponding NN categroy */
        //cnt++;

        RfBsdCfg.NNCatNum = cnt;

        RfBsdCfg.TSClock = BSD_TIME_CLOCK;
        RfBsdCfg.SelfCarSpec.Size.Length = CAR_LENGTH_SR;
        RfBsdCfg.SelfCarSpec.Size.Width = CAR_WIDTH_SR;
        RfBsdCfg.SelfCarSpec.Size.Height = 1632U;
        RfBsdCfg.SelfCarSpec.LeftRightHandDrive = UNKNOW_HAND_DRIVE;
        RfBsdCfg.SelfCarSpec.GearboxType = GEARBOX_TYPE_UNKNOWN;
        RfBsdCfg.SelfCarSpec.DrivingMode = DRIVING_MODE_UNKNOWN;
        RfBsdCfg.SelfCarSpec.WheelBase = 2603U;
        RfBsdCfg.SelfCarSpec.FrontOverhang = 933U;
        RfBsdCfg.SelfCarSpec.BackOverhang = 896U;
        RfBsdCfg.SelfCarSpec.FrontTrack = 1575U;
        RfBsdCfg.SelfCarSpec.RearTrack = 1588U;
        RfBsdCfg.SelfCarSpec.FrontWheel.SectionWidth = 0U; //*unknown
        RfBsdCfg.SelfCarSpec.FrontWheel.SectionHeight = 0U; //*unknown
        RfBsdCfg.SelfCarSpec.FrontWheel.OverallDiameter = 0U; //*unknown
        RfBsdCfg.SelfCarSpec.RearWheel.SectionWidth = 0U; //*unknown
        RfBsdCfg.SelfCarSpec.RearWheel.SectionHeight = 0U; //*unknown
        RfBsdCfg.SelfCarSpec.RearWheel.OverallDiameter = 0U; //*unknown
    }

    /** ----------------------------------------------------------------- */
    /** Cfg for WS                                                        */
    /** ----------------------------------------------------------------- */
    if (Rval == ADAS_ERR_NONE) {
        Rval = AmbaWrap_memset(RfBsdCfg.WsBsdCfg, 0, sizeof(AMBA_WS_BSD_DATA_s) * RF_BSD_MAX_WS_CFG_NUM);
        Svc_PrintNg(__func__, __LINE__, Rval);
        Rval = AmbaWrap_memset(RfBsdCfg.WsWarnObjType, 0, sizeof(REF_FLOW_BSD_WARN_OBJ_TYPE_s) * RF_BSD_MAX_WS_CFG_NUM);
        Svc_PrintNg(__func__, __LINE__, Rval);

        RfBsdCfg.WsBsdCfgNum = 1U;
        RfBsdCfg.WsWarnObjType[0U].TypeFlag[SR_OBJ_TYPE_UNKNOWN] = RF_BSD_WARN_TYPE_FLAG_DISABLE;
        RfBsdCfg.WsWarnObjType[0U].TypeFlag[SR_OBJ_TYPE_VEHICLE_0] = RF_BSD_WARN_TYPE_FLAG_ENABLE;
        RfBsdCfg.WsWarnObjType[0U].TypeFlag[SR_OBJ_TYPE_SCOOTER_0] = RF_BSD_WARN_TYPE_FLAG_ENABLE;
        RfBsdCfg.WsWarnObjType[0U].TypeFlag[SR_OBJ_TYPE_PERSON_0] = RF_BSD_WARN_TYPE_FLAG_ENABLE;

        pWsBsdCfg = &RfBsdCfg.WsBsdCfg[0U];
        Rval = AmbaWS_BSD_GetDefaultConfig(pWsBsdCfg);
        if (Rval == ADAS_ERR_NONE) {
            DOUBLE CarSize[2];
            AMBA_WS_BSD_SYSTEM_DELAY_s BSDDelay;
            AMBA_WS_BSD_SPEC_s BSDSpec;

            Rval = AmbaWrap_memcpy(&BSDSpec, &(pWsBsdCfg->BSDSpec), sizeof(AMBA_WS_BSD_SPEC_s));
            Svc_PrintNg(__func__, __LINE__, Rval);

            /* set BSD other config */
            CarSize[0] = (DOUBLE)CAR_WIDTH_SR;
            CarSize[1] = (DOUBLE)CAR_LENGTH_SR;
            BSDDelay.DetectionDelay = BSD_OD_DELAY;
            BSDDelay.FPS = BSD_FPS;
            BSDDelay.OutputDelay = BSD_OUTPUT_DELAY;
            BSDDelay.ReconstructionDelay = BSD_SR_DELAY;

            BSDSpec.Level1_th[0U] = BSD_LV1_THRESH_X;
            BSDSpec.Level1_th[1U] = BSD_LV1_THRESH_Y;

            BSDSpec.Level2_th[0U] = BSD_LV2_THRESH_X;
            BSDSpec.Level2_th[1U] = BSD_LV2_THRESH_Y;

            BSDSpec.Off_th[0U] = BSD_OFF_THRESH_X;
            BSDSpec.Off_th[1U] = BSD_OFF_THRESH_Y;

            BSDSpec.ActiveSpeed = BSD_MIN_WORKING_SPEED;
            BSDSpec.Suppress_th = BSD_SUP_THRESH_X;

            BSDSpec.TSClock = BSD_TIME_CLOCK;
            BSDSpec.SuppressTargetSpeed = 20U;


            Rval = AmbaWS_BSD_SetConfig(&BSDDelay, &BSDSpec, &CarSize[0], pWsBsdCfg);
            if (Rval != ADAS_ERR_NONE) {
                AmbaPrint_PrintStr5("%s, AmbaWS_BSD_SetConfig failed", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintStr5("%s, AmbaWS_BSD_GetDefaultConfig failed", __func__, NULL, NULL, NULL, NULL);
        }
    }

    {
        RfBsdCfg.WsBsdCfg[0U].BSDSpec.RelativeSpeedStride = 5.0;
        for (UINT32 i = 0U; i < TTC_LEVEL; i++) {
            RfBsdCfg.WsBsdCfg[0U].BSDSpec.TTC_th[i] = 5000.0;
        }
    }

    if (Rval == ADAS_ERR_NONE) {
#if 1
        SvcBsd_GetLeftCalData();
        SvcBsd_GetRightCalData();

        Rval = AmbaWrap_memcpy(&(RfBsdCfg.CalibCfg[RF_BSD_CHANNEL_LEFT]), CalibData[BSD_LEFT], sizeof(AMBA_CAL_EM_CALC_COORD_CFG_s));
        Svc_PrintNg(__func__, __LINE__, Rval);
        Rval = AmbaWrap_memcpy(&(RfBsdCfg.CalibCfg[RF_BSD_CHANNEL_RIGHT]), CalibData[BSD_RIGHT], sizeof(AMBA_CAL_EM_CALC_COORD_CFG_s));
        Svc_PrintNg(__func__, __LINE__, Rval);
        #if 0
        {
            extern void RefCalib_ShowCalibCoordOutput(const AMBA_CAL_EM_CALC_COORD_CFG_s *CalCoordOutput);

            RefCalib_ShowCalibCoordOutput(CalibData[BSD_LEFT]);
            RefCalib_ShowCalibCoordOutput(CalibData[BSD_RIGHT]);
        }
        #endif
#else
        {
            /** coord_emr_20190409_L.txt */
            static AMBA_CAL_EM_CAM_CALIB_DATA_s EmCamCalibData;
            UINT8 buf1[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 232, 140, 192, 0, 0, 0, 0, 0, 168, 129, 64, 0, 0, 0, 0, 0, 0, 143, 64, 0, 0, 0, 0, 0, 0, 0, 0, 128, 7, 0, 0, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 7, 0, 0, 56, 4, 0, 0, 0, 0, 0, 0, 32, 104, 84, 0};
            UINT8 buf2[] = {200, 80, 19, 6, 50, 12, 13, 192, 139, 150, 239, 103, 92, 253, 209, 63, 190, 62, 129, 220, 190, 143, 118, 64, 55, 250, 193, 101, 180, 136, 213, 191, 41, 5, 102, 67, 48, 166, 16, 192, 144, 230, 8, 174, 21, 174, 165, 64, 206, 19, 136, 65, 126, 87, 46, 191, 126, 111, 91, 194, 240, 191, 10, 191, 40, 156, 190, 22, 28, 23, 241, 63, 0, 0, 0, 0, 0, 28, 185, 192};
            AmbaPrint_PrintUInt5("buf size = %d, AMBA_CAL_EM_CALC_COORD_CFG_s size = %d", sizeof(buf1), sizeof(AMBA_CAL_EM_CALC_COORD_CFG_s), 0U, 0U, 0U);
            AmbaSvcWrap_MisraMemcpy(&(RfBsdCfg.CalibCfg[RF_BSD_CHANNEL_LEFT]), buf1, sizeof(AMBA_CAL_EM_CALC_COORD_CFG_s));
            AmbaSvcWrap_MisraMemcpy(&(RfBsdCfg.CalibCfg[RF_BSD_CHANNEL_RIGHT]), buf1, sizeof(AMBA_CAL_EM_CALC_COORD_CFG_s));

            AmbaPrint_PrintUInt5("buf size = %d, AMBA_CAL_EM_CALC_COORD_CFG_s size = %d", sizeof(buf2), sizeof(AMBA_CAL_EM_CAM_CALIB_DATA_s), 0U, 0U, 0U);
            AmbaSvcWrap_MisraMemcpy(&EmCamCalibData, buf2, sizeof(AMBA_CAL_EM_CAM_CALIB_DATA_s));
            AmbaSvcWrap_MisraMemcpy(&EmCamCalibData, buf2, sizeof(AMBA_CAL_EM_CAM_CALIB_DATA_s));
            RfBsdCfg.CalibCfg[RF_BSD_CHANNEL_LEFT].TransCfg.pCalibDataRaw2World = &EmCamCalibData;
            RfBsdCfg.CalibCfg[RF_BSD_CHANNEL_RIGHT].TransCfg.pCalibDataRaw2World = &EmCamCalibData;

            SvcBsd_GetLeftCalData();
            SvcBsd_GetRightCalData();
        }
#endif
        RfBsdCfg.LogFp = GetResult;
        AmbaMisra_TypeCast(&WrkBufAddr, &(CacheBase));
        Rval = RefFlowBSD_Init(&RfBsdCfg, WrkBufAddr);
        if (Rval != ADAS_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, RefFlowBSD_Init failed", __func__, NULL, NULL, NULL, NULL);
        }
    }

    if (ADAS_ERR_NONE == Rval) {
        SVC_SYS_STAT_ID StatusID;
        Rval = SvcSysStat_Register(SVC_APP_STAT_MENU, BsdTask_StatusCallback, &StatusID);
    }

    if(pCfg->DispNum == 1U){
        if(pCfg->DispStrm[0U].VoutID == VOUT_IDX_A) {
            BsdVoutChan = VOUT_IDX_A;
        }
    }

    #if defined(CONFIG_SOC_CV28)
    //1vin-1vout(HDMI)
    DispSrcWin[BSD_LEFT].Width    = pCfg->DispStrm[0].StrmCfg.ChanCfg[0].SrcWin.Width;
    DispSrcWin[BSD_LEFT].Height   = pCfg->DispStrm[0].StrmCfg.ChanCfg[0].SrcWin.Height;
    DispSrcWin[BSD_LEFT].OffsetX  = pCfg->DispStrm[0].StrmCfg.ChanCfg[0].SrcWin.OffsetX;
    DispSrcWin[BSD_LEFT].OffsetY  = pCfg->DispStrm[0].StrmCfg.ChanCfg[0].SrcWin.OffsetY;

    DispSrcWin[BSD_RIGHT].Width    = pCfg->DispStrm[0].StrmCfg.ChanCfg[0].SrcWin.Width;
    DispSrcWin[BSD_RIGHT].Height   = pCfg->DispStrm[0].StrmCfg.ChanCfg[0].SrcWin.Height;
    DispSrcWin[BSD_RIGHT].OffsetX  = pCfg->DispStrm[0].StrmCfg.ChanCfg[0].SrcWin.OffsetX;
    DispSrcWin[BSD_RIGHT].OffsetY  = pCfg->DispStrm[0].StrmCfg.ChanCfg[0].SrcWin.OffsetY;
    #else
    if(BsdVoutChan == VOUT_IDX_B) {
        if(pCfg->FovNum == 2U) {
            if(pCfg->DispNum == 1U){
                //2vin-2vout(side panel)
                DispSrcWin[BSD_LEFT].Width    = pCfg->DispStrm[0].StrmCfg.ChanCfg[0].SrcWin.Width;
                DispSrcWin[BSD_LEFT].Height   = pCfg->DispStrm[0].StrmCfg.ChanCfg[0].SrcWin.Height;
                DispSrcWin[BSD_LEFT].OffsetX  = pCfg->DispStrm[0].StrmCfg.ChanCfg[0].SrcWin.OffsetX;
                DispSrcWin[BSD_LEFT].OffsetY  = pCfg->DispStrm[0].StrmCfg.ChanCfg[0].SrcWin.OffsetY;

                DispSrcWin[BSD_RIGHT].Width   = pCfg->DispStrm[0].StrmCfg.ChanCfg[1].SrcWin.Width;
                DispSrcWin[BSD_RIGHT].Height  = pCfg->DispStrm[0].StrmCfg.ChanCfg[1].SrcWin.Height;
                DispSrcWin[BSD_RIGHT].OffsetX = pCfg->DispStrm[0].StrmCfg.ChanCfg[1].SrcWin.OffsetX;
                DispSrcWin[BSD_RIGHT].OffsetY = pCfg->DispStrm[0].StrmCfg.ChanCfg[1].SrcWin.OffsetY;
            }
        } else {
            //3vin-3vout(zs/side panel)
            DispSrcWin[BSD_LEFT].Width    = pCfg->DispStrm[BsdVoutChan].StrmCfg.ChanCfg[0].SrcWin.Width;
            DispSrcWin[BSD_LEFT].Height   = pCfg->DispStrm[BsdVoutChan].StrmCfg.ChanCfg[0].SrcWin.Height;
            DispSrcWin[BSD_LEFT].OffsetX  = pCfg->DispStrm[BsdVoutChan].StrmCfg.ChanCfg[0].SrcWin.OffsetX;
            DispSrcWin[BSD_LEFT].OffsetY  = pCfg->DispStrm[BsdVoutChan].StrmCfg.ChanCfg[0].SrcWin.OffsetY;

            DispSrcWin[BSD_RIGHT].Width   = pCfg->DispStrm[BsdVoutChan].StrmCfg.ChanCfg[1].SrcWin.Width;
            DispSrcWin[BSD_RIGHT].Height  = pCfg->DispStrm[BsdVoutChan].StrmCfg.ChanCfg[1].SrcWin.Height;
            DispSrcWin[BSD_RIGHT].OffsetX = pCfg->DispStrm[BsdVoutChan].StrmCfg.ChanCfg[1].SrcWin.OffsetX;
            DispSrcWin[BSD_RIGHT].OffsetY = pCfg->DispStrm[BsdVoutChan].StrmCfg.ChanCfg[1].SrcWin.OffsetY;
        }
    } else {
        //3vin-3vout(HDMI)
        DispSrcWin[BSD_LEFT].Width    = pCfg->DispStrm[BsdVoutChan].StrmCfg.ChanCfg[1].SrcWin.Width;
        DispSrcWin[BSD_LEFT].Height   = pCfg->DispStrm[BsdVoutChan].StrmCfg.ChanCfg[1].SrcWin.Height;
        DispSrcWin[BSD_LEFT].OffsetX  = pCfg->DispStrm[BsdVoutChan].StrmCfg.ChanCfg[1].SrcWin.OffsetX;
        DispSrcWin[BSD_LEFT].OffsetY  = pCfg->DispStrm[BsdVoutChan].StrmCfg.ChanCfg[1].SrcWin.OffsetY;

        DispSrcWin[BSD_RIGHT].Width   = pCfg->DispStrm[BsdVoutChan].StrmCfg.ChanCfg[2].SrcWin.Width;
        DispSrcWin[BSD_RIGHT].Height  = pCfg->DispStrm[BsdVoutChan].StrmCfg.ChanCfg[2].SrcWin.Height;
        DispSrcWin[BSD_RIGHT].OffsetX = pCfg->DispStrm[BsdVoutChan].StrmCfg.ChanCfg[2].SrcWin.OffsetX;
        DispSrcWin[BSD_RIGHT].OffsetY = pCfg->DispStrm[BsdVoutChan].StrmCfg.ChanCfg[2].SrcWin.OffsetY;
    }
    #endif

    Rval = SvcOsd_GetOsdBufSize(BsdVoutChan, &TotalOsdWidth, &TotalOsdHeight);
    Svc_PrintNg(__func__, __LINE__, Rval);

    #if defined(CONFIG_SOC_CV28)
    //CV28 LV with HDMI output
    OsdWidth  = TotalOsdWidth;
    OsdHeight = TotalOsdHeight;
    #else
    if(BsdVoutChan == VOUT_IDX_B) {
        if(SVC_OK == SvcBSDTask_IsLiveView()) {
            //LV with 3VIN 3VOUT
            OsdWidth  = TotalOsdWidth >> 1;
            OsdHeight = TotalOsdHeight;
        } else {
            if (pCfg->DispNum == 1U) {
                //Decode with HDMI output
                OsdWidth  = TotalOsdWidth;
                OsdHeight = TotalOsdHeight;
            } else {
                //Decode with side panel output
                OsdWidth  = TotalOsdWidth >> 1;
                OsdHeight = TotalOsdHeight;
            }
        }
    } else {
        if(SVC_OK == SvcBSDTask_IsLiveView()) {
            //LV with 3VIN 1VOUT
            OsdWidth  = pCfg->DispStrm[BsdVoutChan].StrmCfg.ChanCfg[1].DstWin.Width;
            OsdHeight = pCfg->DispStrm[BsdVoutChan].StrmCfg.ChanCfg[1].DstWin.Height;
        } else {
            //Decode with LCD/DSI output
            OsdWidth  = TotalOsdWidth;
            OsdHeight = TotalOsdHeight;
        }
    }
    #endif

    OsdOffset = TotalOsdWidth - OsdWidth;

    if(pCfg->DispNum == 2U){
        //3Vin 3Vout
        /* Draw Amba Logo */
        SvcLogoDraw_Init(VOUT_IDX_A);
        SvcLogoDraw_Update();
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcBsdTask_Start
 *
 *  @Description:: Start the object detection
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: void
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcBsdTask_Start(void)
{
    UINT32 RetVal;
    SVC_USER_PREF_s  *pSvcUserPref;
    static UINT8 SvcBsd_CvInit = 0U;

    if (SvcBsd_CvInit == 0U) {
        UINT32 i;
        const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
        const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;
        UINT32 CvFlowNum = pResCfg->CvFlowNum;
        UINT32 CvFlowBits = pResCfg->CvFlowBits;

        for (i = 0U; i < CvFlowNum; i++) {
            if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_OD) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_OD) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_OD_FDAG)) {
                    UINT32 CvFlowRegisterID = 0U;
                    RetVal = SvcCvFlow_Register(i, BsdTask_DetectionResultCallback, &CvFlowRegisterID);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(__func__, "SvcBsdTask_Start err. SvcCvFlow_Register failed %d", RetVal, 0U);
                    }
                }
            }
        }

        SvcBsd_CvInit = 1U;
    }

    RetVal = SvcUserPref_Get(&pSvcUserPref);
    if (SVC_OK == RetVal) {
        UINT32 PrefBufSize;
        ULONG  PrefBufAddr;

        SvcLog_OK("BSD", "BSD START", 0U, 0U);
        pSvcUserPref->EnableBSD = 1;
        SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
        RetVal = SvcPref_Save(PrefBufAddr, PrefBufSize);
        if (SVC_OK != RetVal) {
            SvcLog_NG(__func__, "SvcPref_Save return %u", RetVal, 0U);
        }
    } else {
        SvcLog_NG(__func__, "line %u SvcUserPref_Get return %u", __LINE__, RetVal);
    }

    if (SVC_OK == RetVal) {
        SvcBsdTask_OsdEnable(TRUE);
    }

    if (SVC_OK == RetVal) {
        SVC_APP_STAT_BSD_s BsdStat = { .Status = SVC_APP_STAT_BSD_ENABLE };
        RetVal = SvcSysStat_Issue(SVC_APP_STAT_BSD, &BsdStat);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_BSD_TASK, "App Status issue failed", 0U, 0U);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcBsdTask_Stop
 *
 *  @Description:: Stop the object detection
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: void
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcBsdTask_Stop(void)
{
    UINT32 RetVal;
    SVC_USER_PREF_s  *pSvcUserPref;

    RetVal = SvcUserPref_Get(&pSvcUserPref);
    if (SVC_OK == RetVal) {
        UINT32 PrefBufSize;
        ULONG  PrefBufAddr;

        SvcLog_OK("BSD", "BSD STOP", 0U, 0U);
        pSvcUserPref->EnableBSD = 0;
        SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
        RetVal = SvcPref_Save(PrefBufAddr, PrefBufSize);
        if (SVC_OK != RetVal) {
            SvcLog_NG(__func__, "SvcPref_Save return %u", RetVal, 0U);
        }
    } else {
        SvcLog_NG(__func__, "line %u SvcUserPref_Get return %u", __LINE__, RetVal);
    }

    if (SVC_OK == RetVal) {
        SVC_APP_STAT_BSD_s BsdStat = { .Status = SVC_APP_STAT_BSD_DISABLE };
        RetVal = SvcSysStat_Issue(SVC_APP_STAT_BSD, &BsdStat);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_BSD_TASK, "App Status issue failed", 0U, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        SvcBsdTask_OsdEnable(FALSE);
    }

    //reset value
    Previous_BsdEvent[BSD_LEFT] = 0U;
    Previous_BsdEvent[BSD_RIGHT] = 0U;
    Previous_CANBusTurnLightStatus[BSD_LEFT] = 0U;
    Previous_CANBusTurnLightStatus[BSD_RIGHT] = 0U;

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcBsdTask_GetStatus
 *
 *  @Description:: Get current status of Object detection task
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: void
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcBsdTask_GetStatus(UINT8 *pEnable)
{
    UINT32 RetVal;
    SVC_USER_PREF_s  *pSvcUserPref;
    const SVC_RES_CFG_s    *pCfg = SvcResCfg_Get();

    if((pCfg->UserFlag & SVC_EMR_BSD) == 0U) {
        *pEnable = 0;

        RetVal = SVC_OK;
    } else {
        RetVal = SvcBsdTask_GetCalDataReady();

        if (SVC_OK == RetVal) {
            RetVal = SvcUserPref_Get(&pSvcUserPref);
            if (SVC_OK == RetVal) {
                *pEnable = pSvcUserPref->EnableBSD;
            } else {
                *pEnable = 0;
                SvcLog_NG(__func__, "line %u SvcUserPref_Get return %u", __LINE__, RetVal);
            }
        } else {
            *pEnable = 0;
        }
    }

    return RetVal;
}

#if 0
void SvcBsd_BbxTransWsToSvc (SVC_CV_DETRES_BBX_LIST_s *pList, const AMBA_OD_2DBBX_LIST_s *pBbxdataWS, UINT32 Source) {
    pList->MsgCode = pBbxdataWS->MsgCode;
    pList->CaptureTime = pBbxdataWS->CaptureTime;
    pList->FrameNum = pBbxdataWS->FrameNum;
    pList->BbxAmount = pBbxdataWS->NumBbx;
    pList->Source = Source;

    AmbaSvcWrap_MisraMemcpy(pList->Bbx, pBbxdataWS->Bbx, SVC_CV_DETRES_MAX_BBX_NUM * sizeof(SVC_CV_DETRES_BBX_WIN_s));
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: BsdTask_DetectionResultCallback
 *
 *  @Description:: Callback function for Detection Result CV
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: void
\*-----------------------------------------------------------------------------------------------*/
void  BsdTask_DetectionResultCallback(UINT32 Chan, UINT32 CvType, void *pDetResMsg)
{
    static AMBA_WS_BSD_STATUS_s BSDOutput[BSD_CHAN_NUM] GNU_SECTION_NOZEROINIT;
    static AMBA_SR_CANBUS_RAW_DATA_s CanbusRawData[BSD_CHAN_NUM] GNU_SECTION_NOZEROINIT;// for adas
    static AMBA_OD_2DBBX_LIST_s BbxdataWS[BSD_CHAN_NUM] GNU_SECTION_NOZEROINIT;
    static REF_FLOW_BSD_IN_DATA_s rfBsdInData GNU_SECTION_NOZEROINIT;
    static REF_FLOW_BSD_OUT_DATA_s rfBsdOutData GNU_SECTION_NOZEROINIT;
    static SVC_BSD_DRAW_INFO_s BsdDrawInfo GNU_SECTION_NOZEROINIT;
    const SVC_RES_CFG_s    *pCfg = SvcResCfg_Get();

    UINT32 Rval;
    SVC_CV_DETRES_BBX_LIST_s *pList;
    UINT32 BsdChan;
    (void) pDetResMsg;

    AmbaMisra_TouchUnused(pDetResMsg);
    AmbaMisra_TouchUnused(&Chan);
    AmbaMisra_TouchUnused(&CvType);
    AmbaMisra_TouchUnused(&Rval);

    AmbaMisra_TypeCast(&pList, &pDetResMsg);
    BbxStarted = 1U;
    #if 0
    {
        UINT32 OrcTimer;

        AmbaPrint_PrintUInt5("bbx CaptureTime %d Source %d",pList->CaptureTime, pList->Source, 0U, 0U, 0U);
        OrcTimer = 0U;
        if (AmbaSYS_GetOrcTimer(&OrcTimer) == SYS_ERR_NONE) {
            AmbaPrint_PrintUInt5("AmbaSYS_GetOrcTimer %d",OrcTimer, 0U, 0U, 0U, 0U);
        }
        AmbaPrint_PrintUInt5("pList->FrameNum %d",pList->FrameNum, 0U, 0U, 0U, 0U);
        SvcLog_NG(__func__, "Line %d", __LINE__, 0U);
    }
    #endif

    if(SVC_OK == SvcBSDTask_IsLiveView()) {
        //LV mode
        if (pCfg->CvFlowNum == 1U){
            //1CV
            if(SvcBsdTask_GetBsdChan() == BSD_LEFT){
                BsdChan = RF_BSD_CHANNEL_LEFT;
            }
            if(SvcBsdTask_GetBsdChan() == BSD_RIGHT){
                BsdChan = RF_BSD_CHANNEL_RIGHT;
            }
        } else {
            //2CV
            if(pList->Source == pCfg->CvFlow[0U].InputCfg.Input[0U].StrmId){
                BsdChan = RF_BSD_CHANNEL_LEFT;
            } else {
                BsdChan = RF_BSD_CHANNEL_RIGHT;
            }
        }
    } else {
        //decode mode
        if(SvcBsdTask_GetBsdChan() == BSD_LEFT){
            BsdChan = RF_BSD_CHANNEL_LEFT;
        } else {
            BsdChan = RF_BSD_CHANNEL_RIGHT;
        }
    }

    SvcCan_GetRawData(&CanbusRawData[BsdChan]);
    if(CanbusRawData[BsdChan].FlagValidRawData == 0U) {
        if(SVC_OK != SvcBSDTask_IsLiveView()) {
            //Decode mode
            if(BsdWithClipCanBus == TRUE) {
                //Decode w CAN
                Rval = SvcDataGather_GetCanData(TICK_TYPE_AUDIO, (UINT64)pList->FrameNum, &rfBsdInData.CanbusTrData);
                Svc_PrintNg(__func__, __LINE__, Rval);
                pList->CaptureTime = rfBsdInData.CanbusTrData.CapTS;
            } else {
                //Decode w/o CAN
                rfBsdInData.CanbusTrData.FlagValidTransferData = 0U;
                rfBsdInData.CanbusTrData.CapTS = pList->CaptureTime;
            }
        } else {
            //LV w/o CAN
            rfBsdInData.CanbusTrData.FlagValidTransferData = 0U;
            rfBsdInData.CanbusTrData.CapTS = pList->CaptureTime;
        }
    } else {
        //LV w CAN
        Rval = SvcDataGather_GetCanData(TICK_TYPE_AUDIO, (UINT64)pList->CaptureTime, &rfBsdInData.CanbusTrData);
        Svc_PrintNg(__func__, __LINE__, Rval);
    }

    SvcBsd_BbxTransSvcToWs(&BbxdataWS[BsdChan], pList);
    Rval = AmbaWrap_memcpy(&rfBsdInData.BbxList, &BbxdataWS[BsdChan], sizeof(AMBA_OD_2DBBX_LIST_s));
    Svc_PrintNg(__func__, __LINE__, Rval);

    #if 0
    SVC_WRAP_PRINT "Vaild %u CAN.Speed %2.1f CAN.CapTS %u OD.CapTS: %u"
    SVC_PRN_ARG_S "BSD Task"
    SVC_PRN_ARG_PROC SvcLog_OK
    SVC_PRN_ARG_UINT32 rfBsdInData.CanbusTrData.FlagValidTransferData SVC_PRN_ARG_POST
    SVC_PRN_ARG_DOUBLE rfBsdInData.CanbusTrData.TransferSpeed
    SVC_PRN_ARG_UINT32 rfBsdInData.CanbusTrData.CapTS                 SVC_PRN_ARG_POST
    SVC_PRN_ARG_UINT32 BbxdataWS[BsdChan].CaptureTime                 SVC_PRN_ARG_POST
    SVC_PRN_ARG_E
    #endif

    #if SAVE_OD_RESULT
    Save2DBbxResult(&rfBsdInData.BbxList);
    SaveCanBus(&rfBsdInData.CanbusTrData);
    #endif

    Rval = RefFlowBSD_Process(BsdChan, &rfBsdInData, &rfBsdOutData);
    if (Rval != ADAS_ERR_NONE) {
        Svc_PrintNg(__func__, __LINE__, Rval);
    }

    Rval = AmbaWrap_memcpy(&BSDOutput[BsdChan], &rfBsdOutData.WsBsdStat[0], sizeof(AMBA_WS_BSD_STATUS_s));
    Svc_PrintNg(__func__, __LINE__, Rval);

    switch(BSDOutput[BsdChan].Event) {
        case AMBA_WS_BSD_EVENT_LV1_LEFT:
        case AMBA_WS_BSD_EVENT_LV1_RIGHT:
            for (UINT32 i = 0; i < pList->BbxAmount; i++) {
                if(BSDOutput[BsdChan].ObjectStatus.ObjectID == BbxOut.Bbx[i].ObjectId) {
                    BbxOut.Bbx[i].Cat = LV1_CAT;
                }
            }
            break;
        case AMBA_WS_BSD_EVENT_CLS_LV1_LEFT:
        case AMBA_WS_BSD_EVENT_CLS_LV1_RIGHT:
            for (UINT32 i = 0; i < pList->BbxAmount; i++) {
                if(BSDOutput[BsdChan].ObjectStatus.ObjectID == BbxOut.Bbx[i].ObjectId) {
                    BbxOut.Bbx[i].Cat = LV1_CLS_CAT;
                }
            }
            break;
        case AMBA_WS_BSD_EVENT_LV2_LEFT:
        case AMBA_WS_BSD_EVENT_LV2_RIGHT:
        case AMBA_WS_BSD_EVENT_CLS_LV2_LEFT:
        case AMBA_WS_BSD_EVENT_CLS_LV2_RIGHT:
            for (UINT32 i = 0; i < pList->BbxAmount; i++) {
                if(BSDOutput[BsdChan].ObjectStatus.ObjectID == BbxOut.Bbx[i].ObjectId) {
                    BbxOut.Bbx[i].Cat = LV2_CAT;
                }
            }
            break;
        case AMBA_WS_BSD_EVENT_NO_WARNING:
        case AMBA_WS_BSD_EVENT_UNIMPLEMENTED:
        case AMBA_WS_BSD_EVENT_UNACTIVE:
        default:
            //
            break;
    }
    Previous_BsdEvent[BsdChan] = BSDOutput[BsdChan].Event;
    Previous_CANBusTurnLightStatus[BsdChan] = CanbusRawData[BsdChan].CANBusTurnLightStatus;

    BsdDrawInfo.Chan = Chan;
    BsdDrawInfo.Class_name = pList->class_name;
    BsdDrawInfo.FovIdx = pList->Source;
    BsdDrawInfo.pBbx = &BbxOut;
    BsdDrawInfo.pSRData = &SROuput;

    Rval = SvcODDrawTask_Msg(SVC_BSD_DRAW, &BsdDrawInfo);
    Svc_PrintNg(__func__, __LINE__, Rval);
    BsdOsdUpdate = 1U;
}

void SvcBsdTask_OsdEnable(UINT32 Flag)
{
    static UINT8 BsdOsdInit = FALSE;

    if(Flag == TRUE) {
        if(BsdOsdInit == FALSE) {
            SvcGui_Register(BsdVoutChan, BSD_CAN_GUI_LEVEL, "BsdTask", SvcBsd_OsdDraw, SvcBsd_OsdUpdate);
            BsdOsdInit = TRUE;
        }
    } else {
        if(BsdOsdInit == TRUE) {
            SvcGui_Unregister(BsdVoutChan, BSD_CAN_GUI_LEVEL);
            BsdOsdInit = FALSE;
        }
    }
}

static void SvcBsd_OsdDraw(UINT32 VoutIdx, UINT32 Level)
{
    extern void RefCalib_ShowCalibCoordOutput(const AMBA_CAL_EM_CALC_COORD_CFG_s *CalCoordOutput);

    static INT32   Z1X[BSD_CHAN_NUM][4], Z1Y[BSD_CHAN_NUM][4], Z2X[BSD_CHAN_NUM][4], Z2Y[BSD_CHAN_NUM][4];
    const SVC_RES_CFG_s    *pCfg = SvcResCfg_Get();
    UINT32 RVAL;

    AMBA_CAL_POINT_DB_3D_s pnt3D[BSD_CHAN_NUM][2];
    AMBA_CAL_POINT_DB_2D_s pnt2D[BSD_CHAN_NUM][2];

    AmbaMisra_TouchUnused(&VoutIdx);


    if(BbxStarted == 1U) {
        #if 0//defined(CONFIG_SOC_CV28)
        for (UINT32 i=0U; i<BSD_CHAN_NUM; i++) {
            RVAL = GetWarnZonePnts(CalibData[i], pWsBsdCfg, NULL, Z1X[i], Z1Y[i], Z2X[i], Z2Y[i]);
            Svc_PrintNg(__func__, __LINE__, RVAL);
        }
        #else
        for (UINT32 i=0U; i<BSD_CHAN_NUM; i++) {
            if(SVC_OK == SvcBSDTask_IsLiveView()) {
                //LV Mode
                AMBA_CAL_ROI_s DrawRoi;

                DrawRoi.Width  = DispSrcWin[i].Width;
                DrawRoi.Height = DispSrcWin[i].Height;
                DrawRoi.StartX = DispSrcWin[i].OffsetX;
                DrawRoi.StartY = DispSrcWin[i].OffsetY;
                RVAL = GetWarnZonePnts(CalibData[i], pWsBsdCfg, &DrawRoi, Z1X[i], Z1Y[i], Z2X[i], Z2Y[i]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
            } else {
                //decode mode
                RVAL = GetWarnZonePnts(CalibData[i], pWsBsdCfg, NULL, Z1X[i], Z1Y[i], Z2X[i], Z2Y[i]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
            }
        }
        #endif

        for (UINT32 i=0U; i<pCfg->CvFlowNum; i++) {
            SvcBsdTask_HorizonPosition(i);
        }

        if(SvcBsdTask_GetBsdChan() == BSD_LEFT) {
            RVAL = SvcOsd_DrawLine(BsdVoutChan, 0U, HorizonPos[BSD_LEFT][1U], OsdWidth - 1U, HorizonPos[BSD_LEFT][0U], 1U, Z2_COLOR);
            Svc_PrintNg(__func__, __LINE__, RVAL);

            RVAL = SvcOsd_DrawLine(BsdVoutChan,
                SvcBSDTask_MainToOsdDomain(Z1X[BSD_LEFT][0], pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z1Y[BSD_LEFT][0], pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                SvcBSDTask_MainToOsdDomain(Z1X[BSD_LEFT][1], pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z1Y[BSD_LEFT][1], pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                1U, Z1_COLOR);
            Svc_PrintNg(__func__, __LINE__, RVAL);

            RVAL = SvcOsd_DrawLine(BsdVoutChan,
                SvcBSDTask_MainToOsdDomain(Z1X[BSD_LEFT][1], pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z1Y[BSD_LEFT][1], pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                SvcBSDTask_MainToOsdDomain(Z1X[BSD_LEFT][2], pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z1Y[BSD_LEFT][2], pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                1U, Z1_COLOR);
            Svc_PrintNg(__func__, __LINE__, RVAL);

            RVAL = SvcOsd_DrawLine(BsdVoutChan,
                SvcBSDTask_MainToOsdDomain(Z1X[BSD_LEFT][2], pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z1Y[BSD_LEFT][2], pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                SvcBSDTask_MainToOsdDomain(Z1X[BSD_LEFT][3], pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z1Y[BSD_LEFT][3], pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                1U, Z1_COLOR);
            Svc_PrintNg(__func__, __LINE__, RVAL);

            RVAL = SvcOsd_DrawLine(BsdVoutChan,
                SvcBSDTask_MainToOsdDomain(Z2X[BSD_LEFT][0], pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z2Y[BSD_LEFT][0], pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                SvcBSDTask_MainToOsdDomain(Z2X[BSD_LEFT][1], pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z2Y[BSD_LEFT][1], pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                1U, Z2_COLOR);
            Svc_PrintNg(__func__, __LINE__, RVAL);

            RVAL = SvcOsd_DrawLine(BsdVoutChan,
                SvcBSDTask_MainToOsdDomain(Z2X[BSD_LEFT][1], pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z2Y[BSD_LEFT][1], pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                SvcBSDTask_MainToOsdDomain(Z2X[BSD_LEFT][2], pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z2Y[BSD_LEFT][2], pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                1U, Z2_COLOR);
            Svc_PrintNg(__func__, __LINE__, RVAL);

            RVAL = SvcOsd_DrawLine(BsdVoutChan,
                SvcBSDTask_MainToOsdDomain(Z2X[BSD_LEFT][2], pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z2Y[BSD_LEFT][2], pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                SvcBSDTask_MainToOsdDomain(Z2X[BSD_LEFT][3], pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z2Y[BSD_LEFT][3], pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                1U, Z2_COLOR);
            Svc_PrintNg(__func__, __LINE__, RVAL);

            if(BsdRefDistLine == TRUE) {
                pnt3D[BSD_LEFT][0].X =   2000.0;
                pnt3D[BSD_LEFT][0].Y = -30000.0;
                pnt3D[BSD_LEFT][0].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_LEFT], &pnt3D[BSD_LEFT][0], &pnt2D[BSD_LEFT][0]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_LEFT][0].X = pnt2D[BSD_LEFT][0].X - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetX;
                pnt2D[BSD_LEFT][0].Y = pnt2D[BSD_LEFT][0].Y - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetY;

                pnt3D[BSD_LEFT][1].X = -20000.0;
                pnt3D[BSD_LEFT][1].Y = -30000.0;
                pnt3D[BSD_LEFT][1].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_LEFT], &pnt3D[BSD_LEFT][1], &pnt2D[BSD_LEFT][1]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_LEFT][1].X = pnt2D[BSD_LEFT][1].X - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetX;
                pnt2D[BSD_LEFT][1].Y = pnt2D[BSD_LEFT][1].Y - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetY;

                RVAL = SvcOsd_DrawLine(BsdVoutChan,
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][0].X, pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][0].Y, pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][1].X, pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][1].Y, pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                    1U, Z1_COLOR);
                Svc_PrintNg(__func__, __LINE__, RVAL);

                pnt3D[BSD_LEFT][0].X =   2000.0;
                pnt3D[BSD_LEFT][0].Y = -40000.0;
                pnt3D[BSD_LEFT][0].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_LEFT], &pnt3D[BSD_LEFT][0], &pnt2D[BSD_LEFT][0]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_LEFT][0].X = pnt2D[BSD_LEFT][0].X - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetX;
                pnt2D[BSD_LEFT][0].Y = pnt2D[BSD_LEFT][0].Y - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetY;

                pnt3D[BSD_LEFT][1].X = -20000.0;
                pnt3D[BSD_LEFT][1].Y = -40000.0;
                pnt3D[BSD_LEFT][1].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_LEFT], &pnt3D[BSD_LEFT][1], &pnt2D[BSD_LEFT][1]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_LEFT][1].X = pnt2D[BSD_LEFT][1].X - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetX;
                pnt2D[BSD_LEFT][1].Y = pnt2D[BSD_LEFT][1].Y - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetY;

                RVAL = SvcOsd_DrawLine(BsdVoutChan,
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][0].X, pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][0].Y, pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][1].X, pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][1].Y, pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                    1U, Z1_COLOR);
                Svc_PrintNg(__func__, __LINE__, RVAL);

                pnt3D[BSD_LEFT][0].X =   2000.0;
                pnt3D[BSD_LEFT][0].Y = -50000.0;
                pnt3D[BSD_LEFT][0].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_LEFT], &pnt3D[BSD_LEFT][0], &pnt2D[BSD_LEFT][0]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_LEFT][0].X = pnt2D[BSD_LEFT][0].X - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetX;
                pnt2D[BSD_LEFT][0].Y = pnt2D[BSD_LEFT][0].Y - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetY;

                pnt3D[BSD_LEFT][1].X = -20000.0;
                pnt3D[BSD_LEFT][1].Y = -50000.0;
                pnt3D[BSD_LEFT][1].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_LEFT], &pnt3D[BSD_LEFT][1], &pnt2D[BSD_LEFT][1]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_LEFT][1].X = pnt2D[BSD_LEFT][1].X - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetX;
                pnt2D[BSD_LEFT][1].Y = pnt2D[BSD_LEFT][1].Y - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetY;

                RVAL = SvcOsd_DrawLine(BsdVoutChan,
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][0].X, pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][0].Y, pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][1].X, pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][1].Y, pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                    1U, Z1_COLOR);
                Svc_PrintNg(__func__, __LINE__, RVAL);

                pnt3D[BSD_LEFT][0].X =   2000.0;
                pnt3D[BSD_LEFT][0].Y = -60000.0;
                pnt3D[BSD_LEFT][0].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_LEFT], &pnt3D[BSD_LEFT][0], &pnt2D[BSD_LEFT][0]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_LEFT][0].X = pnt2D[BSD_LEFT][0].X - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetX;
                pnt2D[BSD_LEFT][0].Y = pnt2D[BSD_LEFT][0].Y - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetY;

                pnt3D[BSD_LEFT][1].X = -20000.0;
                pnt3D[BSD_LEFT][1].Y = -60000.0;
                pnt3D[BSD_LEFT][1].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_LEFT], &pnt3D[BSD_LEFT][1], &pnt2D[BSD_LEFT][1]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_LEFT][1].X = pnt2D[BSD_LEFT][1].X - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetX;
                pnt2D[BSD_LEFT][1].Y = pnt2D[BSD_LEFT][1].Y - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetY;


                RVAL = SvcOsd_DrawLine(BsdVoutChan,
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][0].X, pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][0].Y, pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][1].X, pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][1].Y, pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                    1U, Z1_COLOR);
                Svc_PrintNg(__func__, __LINE__, RVAL);
            }
        } else if (SvcBsdTask_GetBsdChan() == BSD_RIGHT) {
            RVAL = SvcOsd_DrawLine(BsdVoutChan, 0U, HorizonPos[BSD_RIGHT][1U], OsdWidth - 1U, HorizonPos[BSD_RIGHT][0U], 1U, Z2_COLOR);
            Svc_PrintNg(__func__, __LINE__, RVAL);

            RVAL = SvcOsd_DrawLine(BsdVoutChan,
                SvcBSDTask_MainToOsdDomain(Z1X[BSD_RIGHT][0], pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z1Y[BSD_RIGHT][0], pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                SvcBSDTask_MainToOsdDomain(Z1X[BSD_RIGHT][1], pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z1Y[BSD_RIGHT][1], pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                1U, Z1_COLOR);
            Svc_PrintNg(__func__, __LINE__, RVAL);

            RVAL = SvcOsd_DrawLine(BsdVoutChan,
                SvcBSDTask_MainToOsdDomain(Z1X[BSD_RIGHT][1], pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z1Y[BSD_RIGHT][1], pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                SvcBSDTask_MainToOsdDomain(Z1X[BSD_RIGHT][2], pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z1Y[BSD_RIGHT][2], pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                1U, Z1_COLOR);
            Svc_PrintNg(__func__, __LINE__, RVAL);

            RVAL = SvcOsd_DrawLine(BsdVoutChan,
                SvcBSDTask_MainToOsdDomain(Z1X[BSD_RIGHT][2], pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z1Y[BSD_RIGHT][2], pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                SvcBSDTask_MainToOsdDomain(Z1X[BSD_RIGHT][3], pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z1Y[BSD_RIGHT][3], pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                1U, Z1_COLOR);
            Svc_PrintNg(__func__, __LINE__, RVAL);

            RVAL = SvcOsd_DrawLine(BsdVoutChan,
                SvcBSDTask_MainToOsdDomain(Z2X[BSD_RIGHT][0], pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z2Y[BSD_RIGHT][0], pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                SvcBSDTask_MainToOsdDomain(Z2X[BSD_RIGHT][1], pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z2Y[BSD_RIGHT][1], pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                1U, Z2_COLOR);
            Svc_PrintNg(__func__, __LINE__, RVAL);

            RVAL = SvcOsd_DrawLine(BsdVoutChan,
                SvcBSDTask_MainToOsdDomain(Z2X[BSD_RIGHT][1], pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z2Y[BSD_RIGHT][1], pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                SvcBSDTask_MainToOsdDomain(Z2X[BSD_RIGHT][2], pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z2Y[BSD_RIGHT][2], pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                1U, Z2_COLOR);
            Svc_PrintNg(__func__, __LINE__, RVAL);

            RVAL = SvcOsd_DrawLine(BsdVoutChan,
                SvcBSDTask_MainToOsdDomain(Z2X[BSD_RIGHT][2], pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z2Y[BSD_RIGHT][2], pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                SvcBSDTask_MainToOsdDomain(Z2X[BSD_RIGHT][3], pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z2Y[BSD_RIGHT][3], pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                1U, Z2_COLOR);
            Svc_PrintNg(__func__, __LINE__, RVAL);

            if(BsdRefDistLine == TRUE) {
                pnt3D[BSD_RIGHT][0].X =  -2000.0;
                pnt3D[BSD_RIGHT][0].Y = -30000.0;
                pnt3D[BSD_RIGHT][0].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_RIGHT], &pnt3D[BSD_RIGHT][0], &pnt2D[BSD_RIGHT][0]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_RIGHT][0].X = pnt2D[BSD_RIGHT][0].X - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetX;
                pnt2D[BSD_RIGHT][0].Y = pnt2D[BSD_RIGHT][0].Y - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetY;

                pnt3D[BSD_RIGHT][1].X =  20000.0;
                pnt3D[BSD_RIGHT][1].Y = -30000.0;
                pnt3D[BSD_RIGHT][1].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_RIGHT], &pnt3D[BSD_RIGHT][1], &pnt2D[BSD_RIGHT][1]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_RIGHT][1].X = pnt2D[BSD_RIGHT][1].X - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetX;
                pnt2D[BSD_RIGHT][1].Y = pnt2D[BSD_RIGHT][1].Y - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetY;

                RVAL = SvcOsd_DrawLine(BsdVoutChan,
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][0].X, pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, OsdOffset),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][0].Y, pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][1].X, pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, OsdOffset),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][1].Y, pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                    1U, Z1_COLOR);
                Svc_PrintNg(__func__, __LINE__, RVAL);

                pnt3D[BSD_RIGHT][0].X =  -2000.0;
                pnt3D[BSD_RIGHT][0].Y = -40000.0;
                pnt3D[BSD_RIGHT][0].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_RIGHT], &pnt3D[BSD_RIGHT][0], &pnt2D[BSD_RIGHT][0]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_RIGHT][0].X = pnt2D[BSD_RIGHT][0].X - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetX;
                pnt2D[BSD_RIGHT][0].Y = pnt2D[BSD_RIGHT][0].Y - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetY;

                pnt3D[BSD_RIGHT][1].X =  20000.0;
                pnt3D[BSD_RIGHT][1].Y = -40000.0;
                pnt3D[BSD_RIGHT][1].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_RIGHT], &pnt3D[BSD_RIGHT][1], &pnt2D[BSD_RIGHT][1]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_RIGHT][1].X = pnt2D[BSD_RIGHT][1].X - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetX;
                pnt2D[BSD_RIGHT][1].Y = pnt2D[BSD_RIGHT][1].Y - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetY;

                RVAL = SvcOsd_DrawLine(BsdVoutChan,
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][0].X, pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, OsdOffset),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][0].Y, pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][1].X, pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, OsdOffset),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][1].Y, pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                    1U, Z1_COLOR);
                Svc_PrintNg(__func__, __LINE__, RVAL);

                pnt3D[BSD_RIGHT][0].X =  -2000.0;
                pnt3D[BSD_RIGHT][0].Y = -50000.0;
                pnt3D[BSD_RIGHT][0].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_RIGHT], &pnt3D[BSD_RIGHT][0], &pnt2D[BSD_RIGHT][0]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_RIGHT][0].X = pnt2D[BSD_RIGHT][0].X - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetX;
                pnt2D[BSD_RIGHT][0].Y = pnt2D[BSD_RIGHT][0].Y - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetY;

                pnt3D[BSD_RIGHT][1].X =  20000.0;
                pnt3D[BSD_RIGHT][1].Y = -50000.0;
                pnt3D[BSD_RIGHT][1].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_RIGHT], &pnt3D[BSD_RIGHT][1], &pnt2D[BSD_RIGHT][1]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_RIGHT][1].X = pnt2D[BSD_RIGHT][1].X - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetX;
                pnt2D[BSD_RIGHT][1].Y = pnt2D[BSD_RIGHT][1].Y - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetY;

                RVAL = SvcOsd_DrawLine(BsdVoutChan,
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][0].X, pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, OsdOffset),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][0].Y, pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][1].X, pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, OsdOffset),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][1].Y, pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                    1U, Z1_COLOR);
                Svc_PrintNg(__func__, __LINE__, RVAL);

                pnt3D[BSD_RIGHT][0].X =  -2000.0;
                pnt3D[BSD_RIGHT][0].Y = -60000.0;
                pnt3D[BSD_RIGHT][0].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_RIGHT], &pnt3D[BSD_RIGHT][0], &pnt2D[BSD_RIGHT][0]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_RIGHT][0].X = pnt2D[BSD_RIGHT][0].X - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetX;
                pnt2D[BSD_RIGHT][0].Y = pnt2D[BSD_RIGHT][0].Y - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetY;

                pnt3D[BSD_RIGHT][1].X =  20000.0;
                pnt3D[BSD_RIGHT][1].Y = -60000.0;
                pnt3D[BSD_RIGHT][1].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_RIGHT], &pnt3D[BSD_RIGHT][1], &pnt2D[BSD_RIGHT][1]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_RIGHT][1].X = pnt2D[BSD_RIGHT][1].X - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetX;
                pnt2D[BSD_RIGHT][1].Y = pnt2D[BSD_RIGHT][1].Y - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetY;


                RVAL = SvcOsd_DrawLine(BsdVoutChan,
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][0].X, pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, OsdOffset),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][0].Y, pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][1].X, pCfg->FovCfg[0U].MainWin.Width,  OsdWidth,  1U, OsdOffset),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][1].Y, pCfg->FovCfg[0U].MainWin.Height, OsdHeight, 0U, 0U),
                    1U, Z1_COLOR);
                Svc_PrintNg(__func__, __LINE__, RVAL);
            }
        } else {
            RVAL = SvcOsd_DrawLine(BsdVoutChan, 0U,        HorizonPos[BSD_LEFT][1U],  OsdWidth - 1U,      HorizonPos[BSD_LEFT][0U], 1U, Z2_COLOR);
            Svc_PrintNg(__func__, __LINE__, RVAL);
            RVAL = SvcOsd_DrawLine(BsdVoutChan, OsdOffset, HorizonPos[BSD_RIGHT][1U], TotalOsdWidth - 1U, HorizonPos[BSD_RIGHT][0U] , 1U, Z2_COLOR);
            Svc_PrintNg(__func__, __LINE__, RVAL);

            RVAL = SvcOsd_DrawLine(BsdVoutChan,
                SvcBSDTask_MainToOsdDomain(Z1X[BSD_LEFT][0], DispSrcWin[BSD_LEFT].Width,  OsdWidth , 1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z1Y[BSD_LEFT][0], DispSrcWin[BSD_LEFT].Height, OsdHeight, 0U, 0U),
                SvcBSDTask_MainToOsdDomain(Z1X[BSD_LEFT][1], DispSrcWin[BSD_LEFT].Width,  OsdWidth , 1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z1Y[BSD_LEFT][1], DispSrcWin[BSD_LEFT].Height, OsdHeight, 0U, 0U),
                1U, Z1_COLOR);
            Svc_PrintNg(__func__, __LINE__, RVAL);

            RVAL = SvcOsd_DrawLine(BsdVoutChan,
                SvcBSDTask_MainToOsdDomain(Z1X[BSD_LEFT][1], DispSrcWin[BSD_LEFT].Width,  OsdWidth , 1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z1Y[BSD_LEFT][1], DispSrcWin[BSD_LEFT].Height, OsdHeight, 0U, 0U),
                SvcBSDTask_MainToOsdDomain(Z1X[BSD_LEFT][2], DispSrcWin[BSD_LEFT].Width,  OsdWidth , 1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z1Y[BSD_LEFT][2], DispSrcWin[BSD_LEFT].Height, OsdHeight, 0U, 0U),
                1U, Z1_COLOR);
            Svc_PrintNg(__func__, __LINE__, RVAL);

            RVAL = SvcOsd_DrawLine(BsdVoutChan,
                SvcBSDTask_MainToOsdDomain(Z1X[BSD_LEFT][2], DispSrcWin[BSD_LEFT].Width,  OsdWidth ,  1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z1Y[BSD_LEFT][2], DispSrcWin[BSD_LEFT].Height, OsdHeight,  0U, 0U),
                SvcBSDTask_MainToOsdDomain(Z1X[BSD_LEFT][3], DispSrcWin[BSD_LEFT].Width,  OsdWidth ,  1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z1Y[BSD_LEFT][3], DispSrcWin[BSD_LEFT].Height, OsdHeight,  0U, 0U),
                1U, Z1_COLOR);
            Svc_PrintNg(__func__, __LINE__, RVAL);

            if(BsdVoutChan == VOUT_IDX_B) {
                RVAL = SvcOsd_DrawLine(BsdVoutChan,
                    SvcBSDTask_MainToOsdDomain(Z2X[BSD_LEFT][0], DispSrcWin[BSD_LEFT].Width,  OsdWidth ,  1U, 0U),
                    SvcBSDTask_MainToOsdDomain(Z2Y[BSD_LEFT][0], DispSrcWin[BSD_LEFT].Height, OsdHeight,  0U, 0U),
                    SvcBSDTask_MainToOsdDomain(Z2X[BSD_LEFT][1], DispSrcWin[BSD_LEFT].Width,  OsdWidth ,  1U, 0U),
                    SvcBSDTask_MainToOsdDomain(Z2Y[BSD_LEFT][1], DispSrcWin[BSD_LEFT].Height, OsdHeight,  0U, 0U),
                    1U, Z2_COLOR);
                Svc_PrintNg(__func__, __LINE__, RVAL);
            }

            RVAL = SvcOsd_DrawLine(BsdVoutChan,
                SvcBSDTask_MainToOsdDomain(Z2X[BSD_LEFT][1], DispSrcWin[BSD_LEFT].Width,  OsdWidth ,  1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z2Y[BSD_LEFT][1], DispSrcWin[BSD_LEFT].Height, OsdHeight,  0U, 0U),
                SvcBSDTask_MainToOsdDomain(Z2X[BSD_LEFT][2], DispSrcWin[BSD_LEFT].Width,  OsdWidth ,  1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z2Y[BSD_LEFT][2], DispSrcWin[BSD_LEFT].Height, OsdHeight,  0U, 0U),
                1U, Z2_COLOR);
            Svc_PrintNg(__func__, __LINE__, RVAL);

            RVAL = SvcOsd_DrawLine(BsdVoutChan,
                SvcBSDTask_MainToOsdDomain(Z2X[BSD_LEFT][2], DispSrcWin[BSD_LEFT].Width,  OsdWidth ,  1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z2Y[BSD_LEFT][2], DispSrcWin[BSD_LEFT].Height, OsdHeight,  0U, 0U),
                SvcBSDTask_MainToOsdDomain(Z2X[BSD_LEFT][3], DispSrcWin[BSD_LEFT].Width,  OsdWidth ,  1U, 0U),
                SvcBSDTask_MainToOsdDomain(Z2Y[BSD_LEFT][3], DispSrcWin[BSD_LEFT].Height, OsdHeight,  0U, 0U),
                1U, Z2_COLOR);
            Svc_PrintNg(__func__, __LINE__, RVAL);

            RVAL = SvcOsd_DrawLine(BsdVoutChan,
                SvcBSDTask_MainToOsdDomain(Z1X[BSD_RIGHT][0], DispSrcWin[BSD_RIGHT].Width,  OsdWidth ,  1U, OsdOffset),
                SvcBSDTask_MainToOsdDomain(Z1Y[BSD_RIGHT][0], DispSrcWin[BSD_RIGHT].Height, OsdHeight,  0U, 0U),
                SvcBSDTask_MainToOsdDomain(Z1X[BSD_RIGHT][1], DispSrcWin[BSD_RIGHT].Width,  OsdWidth ,  1U, OsdOffset),
                SvcBSDTask_MainToOsdDomain(Z1Y[BSD_RIGHT][1], DispSrcWin[BSD_RIGHT].Height, OsdHeight,  0U, 0U),
                1U, Z1_COLOR);
            Svc_PrintNg(__func__, __LINE__, RVAL);

            RVAL = SvcOsd_DrawLine(BsdVoutChan,
                SvcBSDTask_MainToOsdDomain(Z1X[BSD_RIGHT][1], DispSrcWin[BSD_RIGHT].Width,  OsdWidth ,  1U, OsdOffset),
                SvcBSDTask_MainToOsdDomain(Z1Y[BSD_RIGHT][1], DispSrcWin[BSD_RIGHT].Height, OsdHeight,  0U, 0U),
                SvcBSDTask_MainToOsdDomain(Z1X[BSD_RIGHT][2], DispSrcWin[BSD_RIGHT].Width,  OsdWidth ,  1U, OsdOffset),
                SvcBSDTask_MainToOsdDomain(Z1Y[BSD_RIGHT][2], DispSrcWin[BSD_RIGHT].Height, OsdHeight,  0U, 0U),
                1U, Z1_COLOR);
            Svc_PrintNg(__func__, __LINE__, RVAL);

            RVAL = SvcOsd_DrawLine(BsdVoutChan,
                SvcBSDTask_MainToOsdDomain(Z1X[BSD_RIGHT][2], DispSrcWin[BSD_RIGHT].Width,  OsdWidth ,  1U, OsdOffset),
                SvcBSDTask_MainToOsdDomain(Z1Y[BSD_RIGHT][2], DispSrcWin[BSD_RIGHT].Height, OsdHeight,  0U, 0U),
                SvcBSDTask_MainToOsdDomain(Z1X[BSD_RIGHT][3], DispSrcWin[BSD_RIGHT].Width,  OsdWidth ,  1U, OsdOffset),
                SvcBSDTask_MainToOsdDomain(Z1Y[BSD_RIGHT][3], DispSrcWin[BSD_RIGHT].Height, OsdHeight,  0U, 0U),
                1U, Z1_COLOR);
            Svc_PrintNg(__func__, __LINE__, RVAL);

            if(BsdVoutChan == VOUT_IDX_B) {
                RVAL = SvcOsd_DrawLine(BsdVoutChan,
                    SvcBSDTask_MainToOsdDomain(Z2X[BSD_RIGHT][0], DispSrcWin[BSD_RIGHT].Width,  OsdWidth ,  1U, OsdOffset),
                    SvcBSDTask_MainToOsdDomain(Z2Y[BSD_RIGHT][0], DispSrcWin[BSD_RIGHT].Height, OsdHeight,  0U, 0U),
                    SvcBSDTask_MainToOsdDomain(Z2X[BSD_RIGHT][1], DispSrcWin[BSD_RIGHT].Width,  OsdWidth ,  1U, OsdOffset),
                    SvcBSDTask_MainToOsdDomain(Z2Y[BSD_RIGHT][1], DispSrcWin[BSD_RIGHT].Height, OsdHeight,  0U, 0U),
                    1U, Z2_COLOR);
                Svc_PrintNg(__func__, __LINE__, RVAL);
            }

            RVAL = SvcOsd_DrawLine(BsdVoutChan,
                SvcBSDTask_MainToOsdDomain(Z2X[BSD_RIGHT][1], DispSrcWin[BSD_RIGHT].Width,  OsdWidth ,  1U, OsdOffset),
                SvcBSDTask_MainToOsdDomain(Z2Y[BSD_RIGHT][1], DispSrcWin[BSD_RIGHT].Height, OsdHeight,  0U, 0U),
                SvcBSDTask_MainToOsdDomain(Z2X[BSD_RIGHT][2], DispSrcWin[BSD_RIGHT].Width,  OsdWidth ,  1U, OsdOffset),
                SvcBSDTask_MainToOsdDomain(Z2Y[BSD_RIGHT][2], DispSrcWin[BSD_RIGHT].Height, OsdHeight,  0U, 0U),
                1U, Z2_COLOR);
            Svc_PrintNg(__func__, __LINE__, RVAL);

            RVAL = SvcOsd_DrawLine(BsdVoutChan,
                SvcBSDTask_MainToOsdDomain(Z2X[BSD_RIGHT][2], DispSrcWin[BSD_RIGHT].Width,  OsdWidth ,  1U, OsdOffset),
                SvcBSDTask_MainToOsdDomain(Z2Y[BSD_RIGHT][2], DispSrcWin[BSD_RIGHT].Height, OsdHeight,  0U, 0U),
                SvcBSDTask_MainToOsdDomain(Z2X[BSD_RIGHT][3], DispSrcWin[BSD_RIGHT].Width,  OsdWidth ,  1U, OsdOffset),
                SvcBSDTask_MainToOsdDomain(Z2Y[BSD_RIGHT][3], DispSrcWin[BSD_RIGHT].Height, OsdHeight,  0U, 0U),
                1U, Z2_COLOR);
            Svc_PrintNg(__func__, __LINE__, RVAL);

            if(BsdRefDistLine == TRUE) {
                pnt3D[BSD_LEFT][0].X =   2000.0;
                pnt3D[BSD_LEFT][0].Y = -30000.0;
                pnt3D[BSD_LEFT][0].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_LEFT], &pnt3D[BSD_LEFT][0], &pnt2D[BSD_LEFT][0]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_LEFT][0].X = pnt2D[BSD_LEFT][0].X - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetX;
                pnt2D[BSD_LEFT][0].Y = pnt2D[BSD_LEFT][0].Y - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetY;

                pnt3D[BSD_LEFT][1].X = -20000.0;
                pnt3D[BSD_LEFT][1].Y = -30000.0;
                pnt3D[BSD_LEFT][1].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_LEFT], &pnt3D[BSD_LEFT][1], &pnt2D[BSD_LEFT][1]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_LEFT][1].X = pnt2D[BSD_LEFT][1].X - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetX;
                pnt2D[BSD_LEFT][1].Y = pnt2D[BSD_LEFT][1].Y - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetY;

                RVAL = SvcOsd_DrawLine(BsdVoutChan,
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][0].X, DispSrcWin[BSD_LEFT].Width,  OsdWidth,  1U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][0].Y, DispSrcWin[BSD_LEFT].Height, OsdHeight, 0U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][1].X, DispSrcWin[BSD_LEFT].Width,  OsdWidth,  1U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][1].Y, DispSrcWin[BSD_LEFT].Height, OsdHeight, 0U, 0U),
                    1U, Z1_COLOR);
                Svc_PrintNg(__func__, __LINE__, RVAL);

                pnt3D[BSD_LEFT][0].X =   2000.0;
                pnt3D[BSD_LEFT][0].Y = -40000.0;
                pnt3D[BSD_LEFT][0].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_LEFT], &pnt3D[BSD_LEFT][0], &pnt2D[BSD_LEFT][0]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_LEFT][0].X = pnt2D[BSD_LEFT][0].X - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetX;
                pnt2D[BSD_LEFT][0].Y = pnt2D[BSD_LEFT][0].Y - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetY;

                pnt3D[BSD_LEFT][1].X = -20000.0;
                pnt3D[BSD_LEFT][1].Y = -40000.0;
                pnt3D[BSD_LEFT][1].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_LEFT], &pnt3D[BSD_LEFT][1], &pnt2D[BSD_LEFT][1]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_LEFT][1].X = pnt2D[BSD_LEFT][1].X - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetX;
                pnt2D[BSD_LEFT][1].Y = pnt2D[BSD_LEFT][1].Y - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetY;

                RVAL = SvcOsd_DrawLine(BsdVoutChan,
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][0].X, DispSrcWin[BSD_LEFT].Width,  OsdWidth,  1U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][0].Y, DispSrcWin[BSD_LEFT].Height, OsdHeight, 0U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][1].X, DispSrcWin[BSD_LEFT].Width,  OsdWidth,  1U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][1].Y, DispSrcWin[BSD_LEFT].Height, OsdHeight, 0U, 0U),
                    1U, Z1_COLOR);
                Svc_PrintNg(__func__, __LINE__, RVAL);

                pnt3D[BSD_LEFT][0].X =   2000.0;
                pnt3D[BSD_LEFT][0].Y = -50000.0;
                pnt3D[BSD_LEFT][0].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_LEFT], &pnt3D[BSD_LEFT][0], &pnt2D[BSD_LEFT][0]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_LEFT][0].X = pnt2D[BSD_LEFT][0].X - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetX;
                pnt2D[BSD_LEFT][0].Y = pnt2D[BSD_LEFT][0].Y - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetY;

                pnt3D[BSD_LEFT][1].X = -20000.0;
                pnt3D[BSD_LEFT][1].Y = -50000.0;
                pnt3D[BSD_LEFT][1].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_LEFT], &pnt3D[BSD_LEFT][1], &pnt2D[BSD_LEFT][1]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_LEFT][1].X = pnt2D[BSD_LEFT][1].X - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetX;
                pnt2D[BSD_LEFT][1].Y = pnt2D[BSD_LEFT][1].Y - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetY;

                RVAL = SvcOsd_DrawLine(BsdVoutChan,
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][0].X, DispSrcWin[BSD_LEFT].Width,  OsdWidth,  1U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][0].Y, DispSrcWin[BSD_LEFT].Height, OsdHeight, 0U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][1].X, DispSrcWin[BSD_LEFT].Width,  OsdWidth,  1U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][1].Y, DispSrcWin[BSD_LEFT].Height, OsdHeight, 0U, 0U),
                    1U, Z1_COLOR);
                Svc_PrintNg(__func__, __LINE__, RVAL);

                pnt3D[BSD_LEFT][0].X =   2000.0;
                pnt3D[BSD_LEFT][0].Y = -60000.0;
                pnt3D[BSD_LEFT][0].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_LEFT], &pnt3D[BSD_LEFT][0], &pnt2D[BSD_LEFT][0]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_LEFT][0].X = pnt2D[BSD_LEFT][0].X - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetX;
                pnt2D[BSD_LEFT][0].Y = pnt2D[BSD_LEFT][0].Y - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetY;

                pnt3D[BSD_LEFT][1].X = -20000.0;
                pnt3D[BSD_LEFT][1].Y = -60000.0;
                pnt3D[BSD_LEFT][1].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_LEFT], &pnt3D[BSD_LEFT][1], &pnt2D[BSD_LEFT][1]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_LEFT][1].X = pnt2D[BSD_LEFT][1].X - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetX;
                pnt2D[BSD_LEFT][1].Y = pnt2D[BSD_LEFT][1].Y - (DOUBLE)DispSrcWin[BSD_LEFT].OffsetY;


                RVAL = SvcOsd_DrawLine(BsdVoutChan,
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][0].X, DispSrcWin[BSD_LEFT].Width,  OsdWidth,  1U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][0].Y, DispSrcWin[BSD_LEFT].Height, OsdHeight, 0U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][1].X, DispSrcWin[BSD_LEFT].Width,  OsdWidth,  1U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_LEFT][1].Y, DispSrcWin[BSD_LEFT].Height, OsdHeight, 0U, 0U),
                    1U, Z1_COLOR);
                Svc_PrintNg(__func__, __LINE__, RVAL);

                pnt3D[BSD_RIGHT][0].X =  -2000.0;
                pnt3D[BSD_RIGHT][0].Y = -30000.0;
                pnt3D[BSD_RIGHT][0].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_RIGHT], &pnt3D[BSD_RIGHT][0], &pnt2D[BSD_RIGHT][0]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_RIGHT][0].X = pnt2D[BSD_RIGHT][0].X - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetX;
                pnt2D[BSD_RIGHT][0].Y = pnt2D[BSD_RIGHT][0].Y - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetY;

                pnt3D[BSD_RIGHT][1].X =  20000.0;
                pnt3D[BSD_RIGHT][1].Y = -30000.0;
                pnt3D[BSD_RIGHT][1].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_RIGHT], &pnt3D[BSD_RIGHT][1], &pnt2D[BSD_RIGHT][1]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_RIGHT][1].X = pnt2D[BSD_RIGHT][1].X - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetX;
                pnt2D[BSD_RIGHT][1].Y = pnt2D[BSD_RIGHT][1].Y - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetY;

                RVAL = SvcOsd_DrawLine(BsdVoutChan,
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][0].X, DispSrcWin[BSD_RIGHT].Width,  OsdWidth,  1U, OsdOffset),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][0].Y, DispSrcWin[BSD_RIGHT].Height, OsdHeight, 0U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][1].X, DispSrcWin[BSD_RIGHT].Width,  OsdWidth,  1U, OsdOffset),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][1].Y, DispSrcWin[BSD_RIGHT].Height, OsdHeight, 0U, 0U),
                    1U, Z1_COLOR);
                Svc_PrintNg(__func__, __LINE__, RVAL);

                pnt3D[BSD_RIGHT][0].X =  -2000.0;
                pnt3D[BSD_RIGHT][0].Y = -40000.0;
                pnt3D[BSD_RIGHT][0].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_RIGHT], &pnt3D[BSD_RIGHT][0], &pnt2D[BSD_RIGHT][0]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_RIGHT][0].X = pnt2D[BSD_RIGHT][0].X - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetX;
                pnt2D[BSD_RIGHT][0].Y = pnt2D[BSD_RIGHT][0].Y - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetY;

                pnt3D[BSD_RIGHT][1].X =  20000.0;
                pnt3D[BSD_RIGHT][1].Y = -40000.0;
                pnt3D[BSD_RIGHT][1].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_RIGHT], &pnt3D[BSD_RIGHT][1], &pnt2D[BSD_RIGHT][1]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_RIGHT][1].X = pnt2D[BSD_RIGHT][1].X - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetX;
                pnt2D[BSD_RIGHT][1].Y = pnt2D[BSD_RIGHT][1].Y - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetY;

                RVAL = SvcOsd_DrawLine(BsdVoutChan,
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][0].X, DispSrcWin[BSD_RIGHT].Width,  OsdWidth,  1U, OsdOffset),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][0].Y, DispSrcWin[BSD_RIGHT].Height, OsdHeight, 0U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][1].X, DispSrcWin[BSD_RIGHT].Width,  OsdWidth,  1U, OsdOffset),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][1].Y, DispSrcWin[BSD_RIGHT].Height, OsdHeight, 0U, 0U),
                    1U, Z1_COLOR);
                Svc_PrintNg(__func__, __LINE__, RVAL);

                pnt3D[BSD_RIGHT][0].X =  -2000.0;
                pnt3D[BSD_RIGHT][0].Y = -50000.0;
                pnt3D[BSD_RIGHT][0].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_RIGHT], &pnt3D[BSD_RIGHT][0], &pnt2D[BSD_RIGHT][0]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_RIGHT][0].X = pnt2D[BSD_RIGHT][0].X - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetX;
                pnt2D[BSD_RIGHT][0].Y = pnt2D[BSD_RIGHT][0].Y - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetY;

                pnt3D[BSD_RIGHT][1].X =  20000.0;
                pnt3D[BSD_RIGHT][1].Y = -50000.0;
                pnt3D[BSD_RIGHT][1].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_RIGHT], &pnt3D[BSD_RIGHT][1], &pnt2D[BSD_RIGHT][1]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_RIGHT][1].X = pnt2D[BSD_RIGHT][1].X - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetX;
                pnt2D[BSD_RIGHT][1].Y = pnt2D[BSD_RIGHT][1].Y - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetY;

                RVAL = SvcOsd_DrawLine(BsdVoutChan,
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][0].X, DispSrcWin[BSD_RIGHT].Width,  OsdWidth,  1U, OsdOffset),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][0].Y, DispSrcWin[BSD_RIGHT].Height, OsdHeight, 0U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][1].X, DispSrcWin[BSD_RIGHT].Width,  OsdWidth,  1U, OsdOffset),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][1].Y, DispSrcWin[BSD_RIGHT].Height, OsdHeight, 0U, 0U),
                    1U, Z1_COLOR);
                Svc_PrintNg(__func__, __LINE__, RVAL);

                pnt3D[BSD_RIGHT][0].X =  -2000.0;
                pnt3D[BSD_RIGHT][0].Y = -60000.0;
                pnt3D[BSD_RIGHT][0].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_RIGHT], &pnt3D[BSD_RIGHT][0], &pnt2D[BSD_RIGHT][0]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_RIGHT][0].X = pnt2D[BSD_RIGHT][0].X - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetX;
                pnt2D[BSD_RIGHT][0].Y = pnt2D[BSD_RIGHT][0].Y - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetY;

                pnt3D[BSD_RIGHT][1].X =  20000.0;
                pnt3D[BSD_RIGHT][1].Y = -60000.0;
                pnt3D[BSD_RIGHT][1].Z =      0.0;

                RVAL = AmbaCal_EmConvPtWorldToImgPlane(CalibData[BSD_RIGHT], &pnt3D[BSD_RIGHT][1], &pnt2D[BSD_RIGHT][1]);
                Svc_PrintNg(__func__, __LINE__, RVAL);
                pnt2D[BSD_RIGHT][1].X = pnt2D[BSD_RIGHT][1].X - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetX;
                pnt2D[BSD_RIGHT][1].Y = pnt2D[BSD_RIGHT][1].Y - (DOUBLE)DispSrcWin[BSD_RIGHT].OffsetY;


                RVAL = SvcOsd_DrawLine(BsdVoutChan,
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][0].X, DispSrcWin[BSD_RIGHT].Width,  OsdWidth,  1U, OsdOffset),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][0].Y, DispSrcWin[BSD_RIGHT].Height, OsdHeight, 0U, 0U),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][1].X, DispSrcWin[BSD_RIGHT].Width,  OsdWidth,  1U, OsdOffset),
                    SvcBSDTask_MainToOsdDomain((INT32)pnt2D[BSD_RIGHT][1].Y, DispSrcWin[BSD_RIGHT].Height, OsdHeight, 0U, 0U),
                    1U, Z1_COLOR);
                Svc_PrintNg(__func__, __LINE__, RVAL);
            }
        }
    }

    AmbaMisra_TouchUnused(&Level);//pass vcast
    AmbaMisra_TouchUnused(&pCfg);//pass vcast
}

static void SvcBsd_OsdUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    if (BsdOsdUpdate == 1U) {
        BsdOsdUpdate = 0U;
        *pUpdate = 1U;
    } else {
        *pUpdate = 0U;
    }
}

UINT32 SvcBsdTask_GetCalDataReady(void) {
    UINT32 Rval;
    static AMBA_CAL_EM_CALC_COORD_CFG_s Cfg;
    static AMBA_CAL_EM_CAM_CALIB_DATA_s CalibDataRaw2World;
    const SVC_RES_CFG_s    *pCfg = SvcResCfg_Get();

    Cfg.TransCfg.pCalibDataRaw2World = &CalibDataRaw2World;
    Rval = SvcCalib_AdasCfgGet(SVC_CALIB_ADAS_TYPE_LEFT, &Cfg);
    if(Rval != SVC_OK) {
        AmbaPrint_PrintUInt5("No BSD_LEFT channel CAL data", 0U, 0U, 0U, 0U, 0U);
    } else {
        Rval = SvcCalib_AdasCfgGet(SVC_CALIB_ADAS_TYPE_RIGHT, &Cfg);
        if(Rval != SVC_OK) {
            AmbaPrint_PrintUInt5("No BSD_RIGHT channel CAL data", 0U, 0U, 0U, 0U, 0U);
        }
    }

    if (SVC_OK == Rval) {
        if (pCfg->CvFlowNum == 1U){
            SVC_USER_PREF_s  *pSvcUserPref;

            Rval = SvcUserPref_Get(&pSvcUserPref);
            if (SVC_OK == Rval) {
                SvcBsdTask_SetBsdChan(pSvcUserPref->PrefBsdChan, 0U);
                if (SvcBsdTask_GetBsdChan() == BSD_BOTH) {
                    //Need to set BSD process channel, if CV == 1
                    AmbaPrint_PrintUInt5("Need set BSD channel BSD_LEFT/BSD_RIGHT", 0U, 0U, 0U, 0U, 0U);
                    Rval = SVC_NG;
                }
            }
        }
    }

    return Rval;
}

//for VW Tiguan
static void SvcBsd_GetLeftCalData(void) {
    UINT32 Rval;
    static AMBA_CAL_EM_CALC_COORD_CFG_s Cfg;
    static AMBA_CAL_EM_CAM_CALIB_DATA_s CalibDataRaw2World;

    if (CALIB_INIT_FLAG[BSD_LEFT] == 0U) {
        Cfg.TransCfg.pCalibDataRaw2World = &CalibDataRaw2World;
        Rval = SvcCalib_AdasCfgGet(SVC_CALIB_ADAS_TYPE_LEFT, &Cfg);
        if(Rval == SVC_OK) {
            AmbaPrint_PrintUInt5("SvcBsd_GetLeftCalData from NAND ", 0U, 0U, 0U, 0U, 0U);
            CalibData[BSD_LEFT] = &Cfg;
            CALIB_INIT_FLAG[BSD_LEFT] = 1U;
        }
    }
}

//for VW Tiguan
static void SvcBsd_GetRightCalData(void) {
    UINT32 Rval;
    static AMBA_CAL_EM_CALC_COORD_CFG_s Cfg;
    static AMBA_CAL_EM_CAM_CALIB_DATA_s CalibDataRaw2World;

    if (CALIB_INIT_FLAG[BSD_RIGHT] == 0U) {
        Cfg.TransCfg.pCalibDataRaw2World = &CalibDataRaw2World;
        Rval = SvcCalib_AdasCfgGet(SVC_CALIB_ADAS_TYPE_RIGHT, &Cfg);
        if(Rval == SVC_OK) {
            AmbaPrint_PrintUInt5("SvcBsd_GetRightCalData from NAND ", 0U, 0U, 0U, 0U, 0U);
            CalibData[BSD_RIGHT] = &Cfg;
            CALIB_INIT_FLAG[BSD_RIGHT] = 1U;
        }
    }
}

void SvcBsd_BbxTransSvcToWs (AMBA_OD_2DBBX_LIST_s *pBbxdataWS, const SVC_CV_DETRES_BBX_LIST_s *pList) {
    UINT32 RVAL;

    pBbxdataWS->MsgCode = pList->MsgCode;
    pBbxdataWS->CaptureTime = pList->CaptureTime;
    pBbxdataWS->FrameNum = pList->FrameNum;
    pBbxdataWS->NumBbx = pList->BbxAmount;

    RVAL = AmbaWrap_memcpy(pBbxdataWS->Bbx, pList->Bbx, SVC_CV_DETRES_MAX_BBX_NUM * sizeof(SVC_CV_DETRES_BBX_WIN_s));
    Svc_PrintNg(__func__, __LINE__, RVAL);
}

static void BsdTask_StatusCallback(UINT32 StatIdx, void *pInfo) {
    UINT32 RetVal = SVC_OK;
    UINT8  BsdOnOff = 0U;
    UINT32 ODDrawTaskStatus = 0U;

    SVC_APP_STAT_MENU_s *pStatus = NULL;

    AmbaMisra_TouchUnused(&StatIdx);    /* Misra-c fixed */
    AmbaMisra_TouchUnused(pInfo);       /* Misra-c fixed */
    AmbaMisra_TouchUnused(pStatus);     /* Misra-c fixed */

    AmbaMisra_TypeCast(&pStatus, &pInfo);

    if (pStatus->Type == SVC_APP_STAT_MENU_TYPE_EMR) {
        if (pStatus->FuncIdx == SVC_APP_STAT_MENU_EMR_BSD) {
            if (pStatus->Operation == SVC_APP_STAT_MENU_OP_ENTER) {
                RetVal |= SvcODDrawTask_GetStatus(&ODDrawTaskStatus);
                if(ODDrawTaskStatus == TRUE){
                    RetVal |= SvcBsdTask_GetStatus(&BsdOnOff);
                    if (BsdOnOff == FALSE) {
                        RetVal |= SvcODDrawTask_Enable(FALSE);
                        RetVal |= SvcBsdTask_Start();
                    } else {
                        RetVal |= SvcBsdTask_Stop();
                        RetVal |= SvcODDrawTask_Enable(TRUE);
                    }
                }
            }
        }
    }

    if (pStatus->Type == SVC_APP_STAT_MENU_TYPE_EMR) {
        if(pStatus->FuncIdx == SVC_APP_STAT_MENU_EMR_OD) {
            if (pStatus->Operation == SVC_APP_STAT_MENU_OP_ENTER) {
                RetVal |= SvcODDrawTask_GetStatus(&ODDrawTaskStatus);
                if(ODDrawTaskStatus == 1U){
                    RetVal |= SvcBsdTask_GetStatus(&BsdOnOff);
                    if (BsdOnOff == TRUE) {
                        RetVal |= SvcODDrawTask_Enable(FALSE);
                        RetVal |= SvcBsdTask_Start();
                    } else {
                        RetVal |= SvcBsdTask_Stop();
                        RetVal |= SvcODDrawTask_Enable(TRUE);
                    }
                } else {
                    SvcBsdTask_OsdEnable(FALSE);
                }
            }
        }
    }

    if (pStatus->Type == SVC_APP_STAT_MENU_TYPE_EMR) {
        if(pStatus->FuncIdx == SVC_APP_STAT_MENU_EMR_MODE) {
            if (pStatus->Operation == SVC_APP_STAT_MENU_OP_PLUS) {
                if(BsdRefDistLine == FALSE){
                    BsdRefDistLine = TRUE;
                } else {
                    BsdRefDistLine = FALSE;
                }
            }
        }
    }

    if (RetVal != SVC_OK) {
        SvcLog_NG(__func__, "Line %d", __LINE__, 0U);
    }
}

static UINT32 BsdChan = BSD_BOTH;
void SvcBsdTask_SetBsdChan(UINT32 FovId, UINT32 Divisor) {
    UINT32 RVAL;

    if(Divisor != 0U){
        RVAL = SvcCvFlow_Control(0U, SVC_CV_CTRL_SET_SRC_RATE, &Divisor);
        Svc_PrintNg(__func__, __LINE__, RVAL);
    }

    BsdChan = FovId;

    {
        UINT32  PrefBufSize;
        ULONG   PrefBufAddr;
        SVC_USER_PREF_s  *pSvcUserPref;

        if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
            SvcLog_NG(__func__, "SvcUserPref_Get() failed!!", 0U, 0U);
        }
        pSvcUserPref->PrefBsdChan = BsdChan;
        SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
        if (SVC_OK != SvcPref_Save(PrefBufAddr, PrefBufSize)) {
            SvcLog_NG(__func__, "SvcPref_Save failed!!", 0U, 0U);
        }
    }
}

UINT32 SvcBsdTask_GetBsdChan(void) {
    return BsdChan;
}

void SvcBsdTask_DisplayLine(void) {
    BsdRefDistLine = TRUE;
}

void SvcBsdTask_SetBsdWithClipCanBus(UINT32 Value) {
    BsdWithClipCanBus = Value;
}

void SvcBsdTask_HorizonPosition(UINT32 CvFovIdx) {
    //#define FovInxDiff_FromBsdToSys    1U
    UINT32 Rval;
    const SVC_RES_CFG_s    *pCfg = SvcResCfg_Get();
    UINT32 FovInxDiff_FromBsdToSys = pCfg->CvFlow[0U].InputCfg.Input[0U].StrmId;

    if(SVC_OK == SvcBSDTask_IsLiveView()) {
        if (pCfg->CvFlowNum == 1U){
            if(SvcBsdTask_GetBsdChan() == BSD_LEFT){
                Rval = AmbaCal_EmFindHorizonPosition(CalibData[BSD_LEFT], 0U,
                    (UINT32)(pCfg->FovCfg[CvFovIdx].MainWin.Height)/2U, (UINT32)(pCfg->FovCfg[CvFovIdx].MainWin.Height)/2U, &HorizonPos[BSD_LEFT][0U]);
                if (Rval != CAL_OK) {
                    SvcLog_NG(__func__, "Line %d CvFovIdx %d", __LINE__, CvFovIdx);
                }
                HorizonPos[BSD_LEFT][0U] = HorizonPos[BSD_LEFT][0U] - DispSrcWin[BSD_LEFT].OffsetY;
                HorizonPos[BSD_LEFT][0U] = (HorizonPos[BSD_LEFT][0U] * OsdHeight) / DispSrcWin[BSD_LEFT].Height;

                Rval = AmbaCal_EmFindHorizonPosition(CalibData[BSD_LEFT], pCfg->FovCfg[CvFovIdx].MainWin.Width,
                    (UINT32)(pCfg->FovCfg[CvFovIdx].MainWin.Height)/2U, (UINT32)(pCfg->FovCfg[CvFovIdx].MainWin.Height)/2U, &HorizonPos[BSD_LEFT][1U]);
                if (Rval != CAL_OK) {
                    SvcLog_NG(__func__, "Line %d CvFovIdx %d", __LINE__, CvFovIdx);
                }
                HorizonPos[BSD_LEFT][1U] = HorizonPos[BSD_LEFT][1U] - DispSrcWin[BSD_LEFT].OffsetY;
                HorizonPos[BSD_LEFT][1U] = (HorizonPos[BSD_LEFT][1U] * OsdHeight) / DispSrcWin[BSD_LEFT].Height;
            } else {
                Rval = AmbaCal_EmFindHorizonPosition(CalibData[BSD_RIGHT], 0U,
                    (UINT32)(pCfg->FovCfg[CvFovIdx].MainWin.Height)/2U, (UINT32)(pCfg->FovCfg[CvFovIdx].MainWin.Height)/2U, &HorizonPos[BSD_RIGHT][0U]);
                if (Rval != CAL_OK) {
                    SvcLog_NG(__func__, "Line %d CvFovIdx %d", __LINE__, CvFovIdx);
                }
                HorizonPos[BSD_RIGHT][0U] = HorizonPos[BSD_RIGHT][0U] - DispSrcWin[BSD_RIGHT].OffsetY;
                HorizonPos[BSD_RIGHT][0U] = (HorizonPos[BSD_RIGHT][0U] * OsdHeight) / DispSrcWin[BSD_RIGHT].Height;

                Rval = AmbaCal_EmFindHorizonPosition(CalibData[BSD_RIGHT], pCfg->FovCfg[CvFovIdx].MainWin.Width,
                    (UINT32)(pCfg->FovCfg[CvFovIdx].MainWin.Height)/2U, (UINT32)(pCfg->FovCfg[CvFovIdx].MainWin.Height)/2U, &HorizonPos[BSD_RIGHT][1U]);
                if (Rval != CAL_OK) {
                    SvcLog_NG(__func__, "Line %d CvFovIdx %d", __LINE__, CvFovIdx);
                }
                HorizonPos[BSD_RIGHT][1U] = HorizonPos[BSD_RIGHT][1U] - DispSrcWin[BSD_RIGHT].OffsetY;
                HorizonPos[BSD_RIGHT][1U] = (HorizonPos[BSD_RIGHT][1U] * OsdHeight) / DispSrcWin[BSD_RIGHT].Height;
            }
        } else {
            UINT32 FovCfgId;
            FovCfgId = CvFovIdx + FovInxDiff_FromBsdToSys;

            //SvcLog_NG(__func__, "FovId %d FovInxDiff_FromBsdToSys %d", FovId, FovInxDiff_FromBsdToSys);

            Rval = AmbaCal_EmFindHorizonPosition(CalibData[CvFovIdx], 0U,
                (UINT32)(pCfg->FovCfg[FovCfgId].MainWin.Height)/2U, (UINT32)(pCfg->FovCfg[FovCfgId].MainWin.Height)/2U, &HorizonPos[CvFovIdx][0U]);
            if (Rval != CAL_OK) {
                SvcLog_NG(__func__, "Line %d CvFovIdx %d", __LINE__, CvFovIdx);
            }
            HorizonPos[CvFovIdx][0U] = HorizonPos[CvFovIdx][0U] - DispSrcWin[CvFovIdx].OffsetY;
            HorizonPos[CvFovIdx][0U] = (HorizonPos[CvFovIdx][0U] * OsdHeight) / DispSrcWin[CvFovIdx].Height;

            Rval = AmbaCal_EmFindHorizonPosition(CalibData[CvFovIdx], pCfg->FovCfg[FovCfgId].MainWin.Width,
                (UINT32)(pCfg->FovCfg[FovCfgId].MainWin.Height)/2U, (UINT32)(pCfg->FovCfg[FovCfgId].MainWin.Height)/2U, &HorizonPos[CvFovIdx][1U]);
            if (Rval != CAL_OK) {
                SvcLog_NG(__func__, "Line %d CvFovIdx %d", __LINE__, CvFovIdx);
            }
            HorizonPos[CvFovIdx][1U] = HorizonPos[CvFovIdx][1U] - DispSrcWin[CvFovIdx].OffsetY;
            HorizonPos[CvFovIdx][1U] = (HorizonPos[CvFovIdx][1U] * OsdHeight) / DispSrcWin[CvFovIdx].Height;
        }
    } else {
        if (pCfg->DispNum == 1U) {
            //Decode with HDMI output
            Rval = AmbaCal_EmFindHorizonPosition(CalibData[CvFovIdx], 0U,
                (UINT32)(pCfg->FovCfg[0U].MainWin.Height)/2U, (UINT32)(pCfg->FovCfg[0U].MainWin.Height)/2U, &HorizonPos[CvFovIdx][0U]);
            if (Rval != CAL_OK) {
                SvcLog_NG(__func__, "Line %d CvFovIdx %d", __LINE__, CvFovIdx);
            }
            HorizonPos[CvFovIdx][0U] = (HorizonPos[CvFovIdx][0U] * OsdHeight) / pCfg->FovCfg[0U].MainWin.Height;

            Rval = AmbaCal_EmFindHorizonPosition(CalibData[CvFovIdx], pCfg->FovCfg[0U].MainWin.Width,
                (UINT32)(pCfg->FovCfg[0U].MainWin.Height)/2U, (UINT32)(pCfg->FovCfg[0U].MainWin.Height)/2U, &HorizonPos[CvFovIdx][1U]);
            if (Rval != CAL_OK) {
                SvcLog_NG(__func__, "Line %d CvFovIdx %d", __LINE__, CvFovIdx);
            }
            HorizonPos[CvFovIdx][1U] = (HorizonPos[CvFovIdx][1U] * OsdHeight) / pCfg->FovCfg[0U].MainWin.Height;
        } else {
            //Decode with zs+side panel
            UINT32 FovCfgId;
            FovCfgId = CvFovIdx + FovInxDiff_FromBsdToSys;

            Rval = AmbaCal_EmFindHorizonPosition(CalibData[CvFovIdx], 0U,
                (UINT32)(pCfg->FovCfg[FovCfgId].MainWin.Height)/2U, (UINT32)(pCfg->FovCfg[FovCfgId].MainWin.Height)/2U, &HorizonPos[CvFovIdx][0U]);
            if (Rval != CAL_OK) {
                SvcLog_NG(__func__, "Line %d CvFovIdx %d", __LINE__, CvFovIdx);
            }
            HorizonPos[CvFovIdx][0U] = HorizonPos[CvFovIdx][0U] - DispSrcWin[CvFovIdx].OffsetY;
            HorizonPos[CvFovIdx][0U] = (HorizonPos[CvFovIdx][0U] * OsdHeight) / DispSrcWin[CvFovIdx].Height;

            Rval = AmbaCal_EmFindHorizonPosition(CalibData[CvFovIdx], pCfg->FovCfg[FovCfgId].MainWin.Width,
                (UINT32)(pCfg->FovCfg[FovCfgId].MainWin.Height)/2U, (UINT32)(pCfg->FovCfg[FovCfgId].MainWin.Height)/2U, &HorizonPos[CvFovIdx][1U]);
            if (Rval != CAL_OK) {
                SvcLog_NG(__func__, "Line %d CvFovIdx %d", __LINE__, CvFovIdx);
            }
            HorizonPos[CvFovIdx][1U] = HorizonPos[CvFovIdx][1U] - DispSrcWin[CvFovIdx].OffsetY;
            HorizonPos[CvFovIdx][1U] = (HorizonPos[CvFovIdx][1U] * OsdHeight) / DispSrcWin[CvFovIdx].Height;
        }
    }
}

UINT32 SvcBSDTask_IsLiveView(void) {
    UINT32                 RetVal = SVC_NG, i, Src, Err;
    SVC_USER_PREF_s        *pSvcUserPref;
    const SVC_RES_CFG_s    *pCfg = SvcResCfg_Get();

    AmbaMisra_TouchUnused(&RetVal);

    /* if it's in duplex mode */
    if (SVC_OK == SvcUserPref_Get(&pSvcUserPref)) {
        if (pSvcUserPref->OperationMode == 1U) {
            for (i = 0U; i < pCfg->FovNum; i++) {
                Err = SvcResCfg_GetFovSrc(i, &Src);
                if ((Err == SVC_OK) && (Src != SVC_VIN_SRC_MEM_DEC)) {
                    RetVal = SVC_OK;
                }
            }
        }
    }

    return RetVal;
}

UINT32 SvcBSDTask_MainToOsdDomain(INT32 Value, UINT32 Main, UINT32 OsdInput, UINT32 Flip, UINT32 Offset) {
    UINT32 RetVal = (UINT32)Value;
    UINT32 Osd = OsdInput - 1U;

    if(Flip == 1U) {
        if(RetVal > Main){
            RetVal = Main;
        }
        RetVal = (Main - RetVal) * Osd / Main;
    } else {
        RetVal =         RetVal  * Osd / Main;
    }

    RetVal = RetVal + Offset;

    #if 0
    AmbaPrint_PrintInt5("Value %d Main %d Osd %d Flip %d Offset %d",Value,Main,Osd,Flip,Offset);
    AmbaPrint_PrintInt5("(Main - Value) * Osd / Main = %d",(Main - Value) * Osd / Main,0,0,0,0);
    AmbaPrint_PrintInt5("RV %d \n",RetVal,0,0,0,0);
    #endif

    return RetVal;
}

static UINT32 GetWarnZonePnts(const AMBA_CAL_EM_CALC_COORD_CFG_s *pCalCfg, const AMBA_WS_BSD_DATA_s *pBsdCfg, const AMBA_CAL_ROI_s *pDrawRoi,
        INT32 Zone1X[4], INT32 Zone1Y[4], INT32 Zone2X[4], INT32 Zone2Y[4])
{
    /**
    * Get BSD warning zone1 and zone2 points
    * @param [in] pCalCfg    Calibration config for corresponding channel
    * @param [in] pBsdCfg    BSD config
    * @param [in] pDrawRoi   Change coordinate to roi which is crop from Vout, NULL for roi is the same as Vout
    * @param [out] Zone1X    4 Zone1 x points (0 for inner close point, 1 for inner far point, 2 for outer far point, 3 for outer close point)
    * @param [out] Zone1Y    4 Zone1 y points (0 for inner close point, 1 for inner far point, 2 for outer far point, 3 for outer close point)
    * @param [out] Zone2X    4 Zone2 x points (0 for inner close point, 1 for inner far point, 2 for outer far point, 3 for outer close point)
    * @param [out] Zone2Y    4 Zone2 y points (0 for inner close point, 1 for inner far point, 2 for outer far point, 3 for outer close point)
    * @return 0 for success, 1 for failed
    */

    UINT32 retVal = 0U;
    DOUBLE sign = 0.0;
    AMBA_CAL_POINT_DB_3D_s pnt3D[4];
    AMBA_CAL_POINT_DB_2D_s pnt2D[4];

    INT32 srcW = (INT32)pCalCfg->VoutArea.Width;  /**< not working for VoutArea.StartX and VoutArea.StartY */
    INT32 srcH = (INT32)pCalCfg->VoutArea.Height;
    DOUBLE srcWDbl = (DOUBLE)srcW;
    DOUBLE srcHDbl = (DOUBLE)srcH;

    INT32 roiW;
    INT32 roiH;
    INT32 roiX;
    INT32 roiY;

    if (pCalCfg->CamPos.X > 0.0) {
        sign = 1.0;
    } else{
        sign = -1.0;
    }

    if (pDrawRoi == NULL) {
        roiW = srcW;
        roiH = srcH;
        roiX = 0;
        roiY = 0;
    } else {
        roiW = (INT32)pDrawRoi->Width;
        roiH = (INT32)pDrawRoi->Height;
        roiX = (INT32)pDrawRoi->StartX;
        roiY = (INT32)pDrawRoi->StartY;
    }

    #if 0
    AmbaPrint_PrintInt5("srcW %d srcH %d",srcW,srcH,0,0,0);
    AmbaPrint_PrintInt5("roi W%d h%d x%d y%d",roiW,roiH,roiX,roiY,0);
    #endif

    if (((roiX + roiW) > srcW) || ((roiY + roiH) > srcH)) {
        AmbaPrint_PrintStr5("%s, Error!! Forbidden pDrawRoi setting.", __func__, NULL, NULL, NULL, NULL);
        retVal = 1U;
    }

    /** For Zone 1 */
    if (retVal == 0U) {
        pnt3D[0].X = pBsdCfg->BSDSpec.Suppress_th * sign;     /**< inner, close */
        pnt3D[0].Y = -pBsdCfg->BSDSpec.Level2_th[1] - (pBsdCfg->CarSize[1] / 2.0);
        pnt3D[0].Z = 0.0;
        pnt3D[1].X = pBsdCfg->BSDSpec.Suppress_th * sign;     /**< inner, far */
        pnt3D[1].Y = -pBsdCfg->BSDSpec.Level1_th[1] - (pBsdCfg->CarSize[1] / 2.0);
        pnt3D[1].Z = 0.0;
        pnt3D[2].X = pBsdCfg->BSDSpec.Level1_th[0] * sign;    /**< outer, far */
        pnt3D[2].Y = -pBsdCfg->BSDSpec.Level1_th[1] - (pBsdCfg->CarSize[1] / 2.0);
        pnt3D[2].Z = 0.0;
        pnt3D[3].X = pBsdCfg->BSDSpec.Level2_th[0] * sign;    /**< outer, close */
        pnt3D[3].Y = -pBsdCfg->BSDSpec.Level2_th[1] - (pBsdCfg->CarSize[1] / 2.0);
        pnt3D[3].Z = 0.0;

        for (UINT32 i = 0; i < 4U; i++) {
            if (retVal == 0U) {
                retVal = AmbaCal_EmConvPtWorldToImgPlane(pCalCfg, &pnt3D[i], &pnt2D[i]);
                if (retVal == CAL_OK) {
                    if ((pnt2D[i].X < 0.0) || (pnt2D[i].Y < 0.0) || (pnt2D[i].X > (srcWDbl-1.0)) || (pnt2D[i].Y > (srcHDbl-1.0))) {
                        retVal = 1U;
                        AmbaPrint_PrintStr5("%s, get zone1 pnt failed", __func__, NULL, NULL, NULL, NULL);
                    }

                    if (retVal == 0U) {
                        Zone1X[i] = (INT32)pnt2D[i].X - roiX;
                        Zone1Y[i] = (INT32)pnt2D[i].Y - roiY;
                        if ((Zone1X[i] < 0) || (Zone1Y[i] < 0) || (Zone1X[i] > (roiW - 1)) || (Zone1Y[i] > (roiH - 1))) {
                            AmbaPrint_PrintInt5("GetWarnZonePnts, Warning!! (Zone1X[%d] = %d) or (Zone1Y[%d] = %d) is out of drawing roi.", (INT32)i, Zone1X[i], (INT32)i, Zone1Y[i], 0);
                        }
                    }
                }else {
                    AmbaPrint_PrintStr5("%s, get zone2 pnt failed", __func__, NULL, NULL, NULL, NULL);
                    retVal = 1U;
                }
            }
        }
    }


    /** For Zone 2 */
    if (retVal == 0U) {
        pnt3D[0].X = pBsdCfg->BSDSpec.Suppress_th * sign;     /**< inner, close */
        pnt3D[0].Y = (- pBsdCfg->CarSize[1] / 2.0) - 1000.0;
        pnt3D[0].Z = 0.0;
        pnt3D[1].X = pBsdCfg->BSDSpec.Suppress_th * sign;     /**< inner, far */
        pnt3D[1].Y = -pBsdCfg->BSDSpec.Level2_th[1] - (pBsdCfg->CarSize[1] / 2.0);
        pnt3D[1].Z = 0.0;
        pnt3D[2].X = pBsdCfg->BSDSpec.Level2_th[0] * sign;    /**< outer, far */
        pnt3D[2].Y = -pBsdCfg->BSDSpec.Level2_th[1] - (pBsdCfg->CarSize[1] / 2.0);
        pnt3D[2].Z = 0.0;
        pnt3D[3].X = pBsdCfg->BSDSpec.Level2_th[0] * sign;    /**< outer, close */
        pnt3D[3].Y = (- pBsdCfg->CarSize[1] / 2.0) - 1000.0;
        pnt3D[3].Z = 0.0;

        for (UINT32 i = 0; i < 4U; i++) {
            if (retVal == 0U) {
                retVal = AmbaCal_EmConvPtWorldToImgPlane(pCalCfg, &pnt3D[i], &pnt2D[i]);
                if (retVal == CAL_OK) {
                    Zone2X[i] = (INT32)pnt2D[i].X - roiX;
                    Zone2Y[i] = (INT32)pnt2D[i].Y - roiY;
                } else {
                    AmbaPrint_PrintStr5("%s, get zone2 pnt failed", __func__, NULL, NULL, NULL, NULL);
                    retVal = 1U;
                }
            }
        }

        /** extrapolate near point*/
        if (retVal == 0U) {
            INT32 x, y;
            /** for inner case */
            y = roiH - 1;
            x = (((Zone2X[0] - Zone2X[1]) * (y - Zone2Y[1])) / (Zone2Y[0] - Zone2Y[1])) + Zone2X[1];
            if (x < 0) {
                x = 0;
                y = (((Zone2Y[0] - Zone2Y[1]) * (x - Zone2X[1])) / (Zone2X[0] - Zone2X[1])) + Zone2Y[1];
            } else if (x > (roiW - 1)) {
                x = roiW - 1;
                y = (((Zone2Y[0] - Zone2Y[1]) * (x - Zone2X[1])) / (Zone2X[0] - Zone2X[1])) + Zone2Y[1];
            } else {
                //
            }
            Zone2X[0] = x;
            Zone2Y[0] = y;

            /** for outer case */
            y = roiH - 1;
            x = (((Zone2X[3] - Zone2X[2]) * (y - Zone2Y[2])) / (Zone2Y[3] - Zone2Y[2])) + Zone2X[2];
            if (x < 0) {
                x = 0;
                y = (((Zone2Y[3] - Zone2Y[2]) * (x - Zone2X[2])) / (Zone2X[3] - Zone2X[2])) + Zone2Y[2];
            } else if (x > (roiW - 1)) {
                x = roiW - 1;
                y = (((Zone2Y[3] - Zone2Y[2]) * (x - Zone2X[2])) / (Zone2X[3] - Zone2X[2])) + Zone2Y[2];
            } else {
                //
            }
            Zone2X[3] = x;
            Zone2Y[3] = y;

        }
    }

    #if 0
    AmbaPrint_PrintInt5("Zone1 (%d,%d) (%d,%d)",Zone1X[0],Zone1Y[0],Zone1X[1],Zone1Y[1],0);
    AmbaPrint_PrintInt5("Zone1 (%d,%d) (%d,%d)",Zone1X[2],Zone1Y[2],Zone1X[3],Zone1Y[3],0);
    AmbaPrint_PrintInt5("Zone2 (%d,%d) (%d,%d)",Zone2X[0],Zone2Y[0],Zone2X[1],Zone2Y[1],0);
    AmbaPrint_PrintInt5("Zone2 (%d,%d) (%d,%d)",Zone2X[2],Zone2Y[2],Zone2X[3],Zone2Y[3],0);
    #endif

    return retVal;
}



