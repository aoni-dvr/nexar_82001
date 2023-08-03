/**
 *  @file SvcODDrawTask.c
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
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"

/* ssp */
#include "AmbaDSP_Capability.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_Liveview.h"
#if defined(CONFIG_ICAM_PROJECT_EMIRROR) && defined(CONFIG_BUILD_AMBA_ADAS)
#include "AmbaWS_BSD.h"
#endif
#include "AmbaSvcWrap.h"

/* svc-framework */
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"

/* svc-shared */
#include "SvcGui.h"
#include "SvcOsd.h"
#include "SvcResCfg.h"
#include "SvcResCfgTask.h"
#include "SvcCvFlow.h"
#include "SvcCvFlow_Comm.h"
#include "SvcCvCamCtrl.h"
#include "SvcSysStat.h"
#include "SvcWinCalc.h"
#include "SvcTiming.h"
#include "SvcCvAlgo.h"
#include "cvapi_amba_od_interface.h"
#include "cvapi_svccvalgo_ambaperception.h"
//#include "SvcCvFlow_Comm.h"
#include "SvcCvFlow_Rebel_RefOD.h"

/* svc-icam */
#include "SvcAppStat.h"
#if defined(CONFIG_ICAM_PROJECT_EMIRROR) && defined(CONFIG_BUILD_AMBA_ADAS)
#include "AmbaCalib_AVMIF.h"
#include "AmbaOD_2DBbx.h"
#include "AmbaOD_Tracker.h"
#include "AmbaSurround.h"

#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcBufMap.h"
#include "SvcBsdTask.h"
#include "SvcRmgTask.h"
#include "SvcCalibAdas.h"
#endif
#include "SvcCvAppDef.h"
#include "SvcODDrawTask.h"
#include "SvcODDrawTask_Rebel.h"
#include "SvcVoutFrmCtrlTask.h"

#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
#include "RefFlow_FC.h"
#include "SvcFcwsFcmdTask.h"
#include "SvcFcwsFcmdTaskV2.h"
#endif

#if defined(CONFIG_ICAM_IMGCAL_STITCH_USED)
#include "SvcEmrAdaptiveTask.h"
#endif

#if defined(CONFIG_ICAM_CV_LOGGER)
#include "AmbaVfs.h"
#include "SvcCvLogger.h"
#include "SvcCvLoggerTask.h"
#include "RefFlow_Common.h"
#include "RefFlow_BSD.h"
#include "RefFlow_FC.h"
#include "AmbaOD_2DBbx.h"
#endif

#define SVC_LOG_OD_TASK     "OD_TASK"

#define SVC_APP_CV_FLOW_NUM_MAX         (6U)
#define SVC_APP_CV_OD_BOX_MAX           (150U)
#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
#define SVC_APP_CV_3DOD_BOX_MAX         (FC_IN_DATA_V2_3D_MAX)
#else
#define SVC_APP_CV_3DOD_BOX_MAX         (150U)
#endif
#define SVC_APP_CV_OD_LINE_MAX          (650U)
#define SVC_APP_CV_OD_DRAW_CAT_MAX      (32U)

#define MAX_VOUT_CHAN_NUM               (2U)

#define OD_DRAW_OSD_BUF_RING_NUM        (4U)

#define THICKNESS_RATIO (30U)

typedef struct {
    INT16 x;
    INT16 y;
} FC_3D_POINT_s;

typedef struct {
    UINT16 RectMaxIdx;
    UINT16 StrMaxIdx;
    UINT16 LineMaxIdx;
    UINT16 BitmapMaxIdx;
    UINT16 MaxIdx3D;
    UINT16 RectWrIdx[OD_DRAW_OSD_BUF_RING_NUM]; /* Ring buffer */
    UINT16 RectRdIdx[OD_DRAW_OSD_BUF_RING_NUM]; /* Ring buffer */
    UINT16 StrWrIdx[OD_DRAW_OSD_BUF_RING_NUM];
    UINT16 StrRdIdx[OD_DRAW_OSD_BUF_RING_NUM];
    UINT16 LineWrIdx[OD_DRAW_OSD_BUF_RING_NUM];
    UINT16 LineRdIdx[OD_DRAW_OSD_BUF_RING_NUM];
    UINT16 BitmapWrIdx[OD_DRAW_OSD_BUF_RING_NUM];
    UINT16 BitmapRdIdx[OD_DRAW_OSD_BUF_RING_NUM];
    UINT16 WrIdx3D[OD_DRAW_OSD_BUF_RING_NUM]; /* Ring buffer */
    UINT16 RdIdx3D[OD_DRAW_OSD_BUF_RING_NUM]; /* Ring buffer */
    UINT8  WrBufIdx;
    UINT8  RdBufIdx;
} SVC_CV_DISP_OSD_BUF_CTRL_s;

typedef struct {
    UINT32 X1;
    UINT32 Y1;
    UINT32 X2;
    UINT32 Y2;
} RECT_POS_s;

/* Definition for DetRes */
typedef struct {
    UINT8                  Used;
    UINT32                 MainWidth;                   /* Retrieve from ResCfg */
    UINT32                 MainHeight;                  /* Retrieve from ResCfg */
    UINT32                 HierWidth;                   /* Retrieve from ResCfg */
    UINT32                 HierHeight;                  /* Retrieve from ResCfg */
    REGION_s               PyramidDim[AMBA_DSP_MAX_HIER_NUM]; /* Calc by Main and Hier */
    REGION_s               PrevSrc;                     /* Retrieve from ResCfg */
    SVC_CV_ROI_INFO_s      RoiInfo;                     /* Original ROI info. */
    SVC_CV_ROI_INFO_s      Roi_Revt;                    /* ROI with respect to main dimension. */
    REGION_s               OsdReg;                      /* OSD region with respect to OSD buffer. Updated in ODDrawTask_DetResFovInit. */
    UINT32                 OsdRegFlip;                  /* Horizontal flip of the region.  Updated in ODDrawTask_DetResFovInit. */
    UINT32                 GuiLevel;

    const SVC_CHAN_CFG_s   *pPrevChan;                  /* Pointer to ChanCfg of ResCfg */
    UINT8                  PrevCrop;

    UINT8                  BbxDrawFlag[SVC_APP_CV_OD_BOX_MAX];
    #define VALID_DRAW_OBJ      (0x00000001U)
    #define DRAWN_2D_BBX        (0x00000002U)
#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
    #define DRAWN_3D_BBX        (0x00000004U)
#endif
    #define DRAWN_2D_CATEGORY   (0x00000008U)
    #define DRAWN_CLF_CATEGORY  (0x00000010U)
    RECT_POS_s             BbxOsdPos[SVC_APP_CV_OD_BOX_MAX];
} OD_DRAW_INFO_s;

typedef struct {
    UINT8                  Init;
    OD_DRAW_INFO_s         *pDrawInfo;
} OD_DRAW_CTRL_s;

static OD_DRAW_CTRL_s g_ODDrawCtrl[SVC_CV_FLOW_CHAN_MAX] GNU_SECTION_NOZEROINIT;
static OD_DRAW_INFO_s g_ODDrawInfo[SVC_APP_CV_FLOW_NUM_MAX] GNU_SECTION_NOZEROINIT;
static UINT32 g_LastNumBbx[SVC_CV_FLOW_CHAN_MAX] GNU_SECTION_NOZEROINIT;

/* Definition of control OD feature */
static UINT32 ODInit = 0U;

#if defined(CONFIG_ICAM_PROJECT_EMIRROR) && defined(CONFIG_BUILD_AMBA_ADAS)
#define SVC_ADAS_COLOR_MAP          (0U)
#define SVC_EMR_COLOR_MAP           (1U)
#endif
#ifdef CONFIG_CV_FLEXIDAG_AMBAPERCEPTION
#define SVC_PCPT_COLOR_MAP          (2U)
#endif
#define SVC_MAX_COLOR_MAP_NUM       (3U)
static UINT32 ODColorMap = 0U;

/* Definition for OSD */
static SVC_CV_DISP_OSD_BUF_CTRL_s ODOsdBufCtrl[SVC_CV_FLOW_CHAN_MAX] = {0};
static SVC_CV_DISP_OSD_CMD_s    *pODOsdRectMsgInfo[SVC_CV_FLOW_CHAN_MAX][OD_DRAW_OSD_BUF_RING_NUM];    /* Ring buffer */
static SVC_CV_DISP_OSD_STR_s    *pODOsdStrMsgInfo[SVC_CV_FLOW_CHAN_MAX][OD_DRAW_OSD_BUF_RING_NUM];     /* Ring buffer */
static SVC_CV_DISP_OSD_LINE_s   *pODOsdLineMsgInfo[SVC_CV_FLOW_CHAN_MAX][OD_DRAW_OSD_BUF_RING_NUM];    /* Ring buffer */
static SVC_CV_DISP_OSD_BITMAP_s *pODOsdBitmapMsgInfo[SVC_CV_FLOW_CHAN_MAX][OD_DRAW_OSD_BUF_RING_NUM];  /* Ring buffer */
static SVC_CV_DISP_3D_CMD_s     *pODOsd3DMsgInfo[SVC_CV_FLOW_CHAN_MAX][OD_DRAW_OSD_BUF_RING_NUM];      /* Ring buffer */
static UINT32 ODOnOff = 0U;  /* Enable to draw detection on OSD by default */
static UINT8 ODStarted = 0;
static UINT8 ODResultUpdate = 0U;

/* Rebel OD Info flag */
static UINT8 g_RebelOD_ID = 1U;
static UINT8 g_D_xy = 1U;
static UINT8 g_V_xy = 0U;
static UINT8 g_All_OD_Info = 0U;

/* Definition for OSD FileOut */
static UINT32 ODPrintDetRes = 0U;
static UINT32 DetResCount = 0;

/* The Cv Flow registered ID */
static UINT32 ODDrawCvFlowRegisterID[SVC_CV_FLOW_CHAN_MAX] = {0};

/* Mutex for protection */
static AMBA_KAL_MUTEX_t ODMutex[SVC_CV_FLOW_CHAN_MAX] GNU_SECTION_NOZEROINIT;

/* callback function for CV task */
static void   ODDrawTask_DetectionResultHandler(UINT32 CvFlowChan, UINT32 CvType, void *pDetResMsg);
static void   ODDrawTask_RefODResult(UINT32 CvFlowChan, UINT32 CvType, void *pOutput);

/* Internal function for handling the msg from CV */
static void   ODDrawTask_DetResFovInit(UINT32 CvFlowChan);
static void   ODDrawTask_DetResFovDeInit(UINT32 CvFlowChan);
static void   ODDrawTask_DetResUpdateDrawInfo(UINT32 CvFlowChan);
static void   ODDrawTask_DetResUpdateRoiInfo(UINT32 CvFlowChan);
static void   ODDrawTask_DetResBbxOsdProc(UINT32 CvFlowChan, const SVC_CV_DETRES_BBX_LIST_s *pList, UINT32 *OsdUpdate);
static void   ODDrawTask_DetResBbxOsdProcRebel(UINT32 CvFlowChan, const SVC_CV_DETRES_BBX_LIST_REBEL_s *pListRebel, UINT32 *OsdUpdate);
static void   ODDrawTask_DetResDrawRoi(UINT32 CvFlowChan);
static void   ODDrawTask_DetResBbxOsdPosConv(UINT32 CvFlowChan, const SVC_CV_DETRES_BBX_LIST_s *pList);
static void   ODDrawTask_DetResBbxOsdPosConvRebel(UINT32 CvFlowChan, const SVC_CV_DETRES_BBX_LIST_REBEL_s *pListRebel);
static void   ODDrawTask_DetResDrawBbx(UINT32 CvFlowChan, const SVC_CV_DETRES_BBX_LIST_s *pList);
static void   ODDrawTask_DetResDrawBbxRebel(UINT32 CvFlowChan, const SVC_CV_DETRES_BBX_LIST_REBEL_s *pListRebel);
static UINT32 ODDrawTask_DetResRebaseObj(RECT_POS_s *pObjPos, const REGION_s *pNewBase);
static void   ODDrawTask_PrintDetInfo(UINT32 CvFlowChan, const SVC_CV_DETRES_BBX_LIST_s *pList);

static void   ODDrawTask_DetResPcptOsdProc(UINT32 CvFlowChan, const SVC_CV_PERCEPTION_OUTPUT_s *pPcptOut, UINT32 *OsdUpdate);
static void   ODDrawTask_DetResDrawKp(UINT32 CvFlowChan, const SVC_CV_PERCEPTION_OUTPUT_s *pPcptOut, const SVC_CV_PERCEPTION_DATA_s *pPcptData);
static void   ODDrawTask_DetResDrawMask(UINT32 CvFlowChan, const SVC_CV_PERCEPTION_OUTPUT_s *pPcptOut, UINT32 NumData, SVC_CV_PERCEPTION_DATA_s const ** pPcptData);
static void   ODDrawTask_DetResShowClfStr(UINT32 CvFlowChan, const SVC_CV_PERCEPTION_OUTPUT_s *pPcptOut, const SVC_CV_PERCEPTION_DATA_s *pPcptData);
static void   ODDrawTask_DetResShowClfStr_TS(UINT32 CvFlowChan, const SVC_CV_PERCEPTION_OUTPUT_s *pPcptOut, UINT32 NumData, SVC_CV_PERCEPTION_DATA_s const ** pPcptData);

#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
static void   ODDrawTask_FcDetResDrawBbx(UINT32 CvFlowChan, const SVC_FC_DRAW_INFO_s *pList);
static void   ODDrawTask_FcDetResDraw3DBbx(UINT32 CvFlowChan, const SVC_FC_DRAW_INFO_V2_s *pList);
static void   ODDrawTask_FcDetResBbxOsdProc(UINT32 CvFlowChan, const SVC_FC_DRAW_INFO_s *pList);
static void   ODDrawTask_FcDetRes3DBbxOsdProc(UINT32 CvFlowChan, const SVC_FC_DRAW_INFO_V2_s *pList);
#endif

#if defined(CONFIG_ICAM_PROJECT_EMIRROR) && defined(CONFIG_BUILD_AMBA_ADAS)
static void   ODDrawTask_BsdDetResDrawBbx(UINT32 CvFlowChan, const SVC_CV_DETRES_BBX_LIST_s *pList, AMBA_SR_SROBJECT_DATA_s *pSRData);
static void   ODDrawTask_BsdDraw(SVC_BSD_DRAW_INFO_s *pBsdDrawInfo);
static void   ODDrawTask_RmgDetResDrawBbx(UINT32 CvFlowChan, SVC_CV_DETRES_BBX_LIST_s *pList, AMBA_SR_SROBJECT_DATA_s *pSRData);
static void   ODDrawTask_RmgDraw(SVC_RMG_DRAW_INFO_s *pRmgDrawInfo);
#endif

#if defined(CONFIG_ICAM_IMGCAL_STITCH_USED)
static void   ODDrawTask_EmaDetResDrawBbx(UINT32 CvFlowChan, const SVC_CV_DETRES_BBX_LIST_s *pList, AMBA_SR_SROBJECT_DATA_s *pSRData);
static void   ODDrawTask_EmaDraw(SVC_EMA_DRAW_INFO_s *pEmaDrawInfo);
#endif

static void Draw3DbbxResult(const SVC_CV_DISP_3D_CMD_s  *p3DMsgInfo);

/* Internal function to handle both CV detection result */
static void   ODDrawTask_DrawDetectionResult(UINT32 CvFlowChan,const SVC_CV_CAMCTRL_MSG_s *pCamMsg);

/* Internal function to draw  */
static void   ODDrawTask_OsdDraw(UINT32 VoutIdx, UINT32 Level);
static void   ODDrawTask_OsdRectMsgPut(UINT32 CvFlowChan, const SVC_CV_DISP_OSD_CMD_s *pInputMsgInfo);
static void   ODDrawTask_OsdStrMsgPut(UINT32 CvFlowChan, const SVC_CV_DISP_OSD_STR_s *pInputMsgInfo);
static void   ODDrawTask_OsdLineMsgPut(UINT32 CvFlowChan, const SVC_CV_DISP_OSD_LINE_s *pInputMsgInfo);
static void   ODDrawTask_OsdBitmapMsgPut(UINT32 CvFlowChan, const SVC_CV_DISP_OSD_BITMAP_s *pInputMsgInfo);
static void   ODDrawTask_Osd3DMsgPut(UINT32 CvFlowChan, const SVC_CV_DISP_3D_CMD_s *pInputMsgInfo);
static void   ODDrawTask_OsdFlush(UINT32 VoutIdx, UINT32 CvFlowChan);
static UINT32 ODDrawTask_OsdColorMap(UINT32 ColorMap, UINT32 Category);
static UINT32 ODDrawTask_GuiLevel(UINT32 Idx);
static void   ODDrawTask_OsdUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate);

/* callback function for menu */
static void   ODDrawTask_MenuStatusCallback(UINT32 StatIdx, void *pInfo);

/* Bounding Box Label */
static UINT32 FontSize = 1U;
static UINT32 ODLabelCat = 1U;
static UINT32 ODLabelScore = 0U;
static UINT32 ODDrawRoi = 1U;
static UINT32 ODRectThickness = 2U;
static UINT32 ODMaxStrLen = 0U;
static UINT32 ODLabelClfCat = 1U;
static UINT32 ODDrawKp = 1U;
static UINT32 ODDrawMask = 1U;
static UINT32 ODCatAllowBits = 0xFFFFFFFFU;
#if defined(CONFIG_ICAM_PROJECT_EMIRROR) && defined(CONFIG_BUILD_AMBA_ADAS)
static UINT32 ODLabelPosition = 0U;
static AMBA_CAL_EM_CALC_COORD_CFG_s Cfg;
#endif

/* Duplicate from the index 27 of g_SvcOsdClutBuf */
/* Set alpha as semi transparent for mask drawing */
#define SVC_OD_DRAW_BITMAP_CLUT_ENTRY_SIZE     (21U)
extern UINT32 ODDrawBitmapClutBuf[SVC_OD_DRAW_BITMAP_CLUT_ENTRY_SIZE];
UINT32 ODDrawBitmapClutBuf[SVC_OD_DRAW_BITMAP_CLUT_ENTRY_SIZE] GNU_ALIGNED_CACHESAFE = {
    //0xAARRGGBB ,    Idx ( RED, GRN, BLU )   //000
    0x00000000U, // 027 ( 000, 000, 000 )
    // For OD
    0x80FFBF00U, // 033 ( 255, 191, 000 )   //001
    0x80FFBF00U, // 033 ( 255, 191, 000 )   //002
    0x80FF6600U, // 034 ( 255, 102, 000 )   //003
    0x80FF6600U, // 034 ( 255, 102, 000 )   //004
    0x80BFFF00U, // 028 ( 191, 255, 000 )   //005
    0x8000CC00U, // 029 ( 000, 204, 000 )   //006
    0x80FF0000U, // 037 ( 255, 255, 000 )   //007
    0x8000CC00U, // 035 ( 000, 204, 000 )   //008
    0x80FF00FFU, // 038 ( 255, 000, 255 )   //009
    0x808000FFU, // 039 ( 128, 000, 255 )   //010
    0x80FF0000U, // 037 ( 255, 255, 000 )   //011
    0x80A0A0A0U, // 047 ( 224, 160, 000 )   //012
    0x80FF0000U, // 037 ( 255, 255, 000 )   //013
    0x80A0A0A0U, // 047 ( 224, 160, 000 )   //014
    0x80A0A0A0U, // 047 ( 224, 160, 000 )   //015
    0x80A0A0A0U, // 047 ( 224, 160, 000 )   //016
    0x80A0A0A0U, // 047 ( 224, 160, 000 )   //017
    0x80A0A0A0U, // 047 ( 224, 160, 000 )   //018
    0x80A0A0A0U, // 047 ( 224, 160, 000 )   //019
    0x80A0A0A0U, // 047 ( 224, 160, 000 )   //020
};

static void OD_DRAW_TASK_DBG(const char *pFormat, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    static UINT32 ODDrawTaskDebug = 0U;
    if (ODDrawTaskDebug > 0U) {
        AmbaPrint_PrintUInt5(pFormat, Arg1, Arg2, Arg3, Arg4, Arg5);
    }
}

/**
 *  Init the object detection module
 *  @return error code
 */
UINT32 SvcODDrawTask_Init(void)
{
    UINT32 i, j;
    UINT32 RetVal = SVC_OK;
    UINT32 Length, Index;

    static SVC_CV_DISP_OSD_CMD_s  ODOsdRectMsgBuffer[SVC_CV_FLOW_CHAN_MAX * SVC_APP_CV_OD_BOX_MAX * OD_DRAW_OSD_BUF_RING_NUM] GNU_SECTION_NOZEROINIT;
    static SVC_CV_DISP_OSD_STR_s  ODOsdStrMsgBuffer [SVC_CV_FLOW_CHAN_MAX * SVC_APP_CV_OD_BOX_MAX * OD_DRAW_OSD_BUF_RING_NUM] GNU_SECTION_NOZEROINIT;
    static SVC_CV_DISP_OSD_LINE_s ODOsdLINEMsgBuffer[SVC_CV_FLOW_CHAN_MAX * SVC_APP_CV_OD_LINE_MAX * OD_DRAW_OSD_BUF_RING_NUM] GNU_SECTION_NOZEROINIT;
    static SVC_CV_DISP_OSD_BITMAP_s ODOsdBitmapMsgBuffer[SVC_CV_FLOW_CHAN_MAX * SVC_APP_CV_OD_BOX_MAX * OD_DRAW_OSD_BUF_RING_NUM] GNU_SECTION_NOZEROINIT;
    static SVC_CV_DISP_3D_CMD_s   ODOsd3DMsgBuffer[SVC_CV_FLOW_CHAN_MAX * SVC_APP_CV_3DOD_BOX_MAX * OD_DRAW_OSD_BUF_RING_NUM] GNU_SECTION_NOZEROINIT;
    static char ODMutexName[SVC_CV_FLOW_CHAN_MAX][16] GNU_SECTION_NOZEROINIT;

    AmbaMisra_TouchUnused(ODDrawBitmapClutBuf);
    AmbaMisra_TouchUnused(&ODMaxStrLen);
    /* Prepare the ring buffer for Object Detection OSD message */
    for (i = 0; i < SVC_CV_FLOW_CHAN_MAX; i++) {
        ODOsdBufCtrl[i].RectMaxIdx = SVC_APP_CV_OD_BOX_MAX;
        ODOsdBufCtrl[i].StrMaxIdx = SVC_APP_CV_OD_BOX_MAX;
        ODOsdBufCtrl[i].LineMaxIdx = SVC_APP_CV_OD_LINE_MAX;
        ODOsdBufCtrl[i].BitmapMaxIdx = SVC_APP_CV_OD_BOX_MAX;
        ODOsdBufCtrl[i].MaxIdx3D = SVC_APP_CV_3DOD_BOX_MAX;

        for (j = 0; j < OD_DRAW_OSD_BUF_RING_NUM; j++) {
            Index = (i * OD_DRAW_OSD_BUF_RING_NUM) + j;
            pODOsdRectMsgInfo[i][j] = &ODOsdRectMsgBuffer[Index * SVC_APP_CV_OD_BOX_MAX];
            pODOsdStrMsgInfo [i][j] = &ODOsdStrMsgBuffer [Index * SVC_APP_CV_OD_BOX_MAX];
            pODOsdLineMsgInfo[i][j] = &ODOsdLINEMsgBuffer[Index * SVC_APP_CV_OD_LINE_MAX];
            pODOsdBitmapMsgInfo[i][j] = &ODOsdBitmapMsgBuffer[Index * SVC_APP_CV_OD_BOX_MAX];
            pODOsd3DMsgInfo[i][j] = &ODOsd3DMsgBuffer[Index * SVC_APP_CV_3DOD_BOX_MAX];
        }
    }

    for (i = 0; i < SVC_CV_FLOW_CHAN_MAX; i++) {
        Length = SvcWrap_sprintfU32(ODMutexName[i], 16, "ODMutex_%d", 1, &i);
        if (Length <= 0U) {
            SvcLog_NG(SVC_LOG_OD_TASK, "SvcODDrawTask_Init err. SvcWrap queue name length", Length, 0U);
            RetVal = SVC_NG;
        }

        if (SVC_OK == RetVal) {
            RetVal = AmbaKAL_MutexCreate(&ODMutex[i], ODMutexName[i]);
        } else {
            SvcLog_NG(SVC_LOG_OD_TASK, "AmbaKAL_MutexCreate ODMutex[%u] failed, return %u", i, RetVal);
        }
    }

    if (SVC_OK == RetVal) {
        SVC_SYS_STAT_ID StatusID;
        RetVal = SvcSysStat_Register(SVC_APP_STAT_MENU, ODDrawTask_MenuStatusCallback, &StatusID);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_OD_TASK, "SvcODDrawTask_Init err. Menu status register failed", 0U, 0U);
        }
        RetVal = SvcSysStat_Register(SVC_APP_STAT_DISP, ODDrawTask_MenuStatusCallback, &StatusID);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_OD_TASK, "SvcODDrawTask_Init err. Register DISP status failed", 0U, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        ODInit = 1U;
    }

    return RetVal;
}

/**
 *  Config the object detection module
 *  @param[in] ColorMap: OD color map. 0 - adas. 1 - emirror.
 *  @return error code
 */
#define MAX_SRT_CNT (17U)
UINT32 SvcODDrawTask_Config(UINT32 ColorMap)
{
    UINT32 RetVal = SVC_OK;
#ifdef CONFIG_SVC_LVGL_USED
    UINT32 Width, Height;
#endif

    if (SVC_MAX_COLOR_MAP_NUM > ColorMap) {
#ifdef CONFIG_CV_FLEXIDAG_AMBAPERCEPTION
        ODColorMap = SVC_PCPT_COLOR_MAP;
#else
        ODColorMap = ColorMap;
#endif
        RetVal = SVC_OK;
    } else {
        RetVal = SVC_NG;
    }

#ifdef CONFIG_ICAM_VOUTB_OSD_BUF_FHD
    ODRectThickness = 3U;
#else
    ODRectThickness = 2U;
#endif

#ifdef CONFIG_SVC_LVGL_USED
    if (RetVal == SVC_OK) {
        if (FontSize >= 1U) {
            RetVal = SvcOsd_GetFontRes(FontSize, &Width, &Height);
            if (RetVal == SVC_NG) {
                SvcLog_NG(SVC_LOG_OD_TASK, "SvcOsd_GetFontRes() error ", 0U, 0U);
            }

            ODMaxStrLen = MAX_SRT_CNT * Width;
            SvcLog_OK(SVC_LOG_OD_TASK, "ODMaxStrLen = %d", ODMaxStrLen, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_OD_TASK, "2U <= ColorMap", 0U, 0U);
    }
#else
    ODMaxStrLen = 50U;
#endif

    return RetVal;
}

/**
 *  Start the object detection
 *  @return error code
 */
UINT32 SvcODDrawTask_Start(void)
{
    UINT32 RetVal;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    SvcLog_DBG(SVC_LOG_OD_TASK, "SvcODDrawTask_Start", 0U, 0U);

    if (ODInit == 1U) {
        ODOnOff = 1U;
        RetVal = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_OD_TASK, "OD module not init yet. Cannot start", 0U, 0U);
        RetVal = SVC_NG;
    }

    /* Register and start AmbaOD */
    if ((SVC_OK == RetVal) && (0U == (pResCfg->UserFlag & SVC_USER_FLAG_OD_DRAW_OFF))) {
        UINT32 i;
        UINT32 CvFlowNum = pResCfg->CvFlowNum;
        UINT32 CvFlowBits = pResCfg->CvFlowBits;
        const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;

        RetVal |= AmbaWrap_memset(&g_ODDrawCtrl[0], 0, sizeof(g_ODDrawCtrl));
        RetVal |= AmbaWrap_memset(&g_ODDrawInfo[0], 0, sizeof(g_ODDrawInfo));
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_OD_TASK, "AmbaWrap_memset fail", 0U, 0U);
        }
        DetResCount = 0U;

        for (i = 0U; i < CvFlowNum; i++) {
            if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_OD) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_OD) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_OD_FDAG) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_OD_FDAG)) {
                    RetVal |= SvcCvFlow_Register(i, ODDrawTask_RefODResult, &ODDrawCvFlowRegisterID[i]);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_OD_TASK, "SvcODDrawTask_Start err. SvcCvFlow_Start failed %d", RetVal, 0U);
                    }
                }
            }
        }
    }

    if (SVC_OK == RetVal) {
        SVC_APP_STAT_OD_s ODStat = { .Status = SVC_APP_STAT_OD_ENABLE };
        RetVal = SvcSysStat_Issue(SVC_APP_STAT_OD, &ODStat);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_OD_TASK, "App Status issue failed", 0U, 0U);
        }
    }

#if defined(CONFIG_ICAM_PROJECT_EMIRROR) && defined(CONFIG_BUILD_AMBA_ADAS)
    {
        static AMBA_CAL_EM_CAM_CALIB_DATA_s CalibDataRaw2World;
        Cfg.TransCfg.pCalibDataRaw2World = &CalibDataRaw2World;

        if ((pResCfg->UserFlag & SVC_EMR_POSITION) > 0U) {
            if(SvcCalib_AdasCfgGet(SVC_CALIB_ADAS_TYPE_BACK, &Cfg) == 0U) {
                ODLabelCat      = 0U;
                ODDrawRoi       = 0U;
                ODLabelPosition = 1U;
                FontSize = 2U;
            } else {
                SvcLog_NG(__func__, "Line %d", __LINE__, 0U);
            }
        }
    }
#else
    FontSize = 1U;
#endif

    return RetVal;
}

/**
 *  Stop the object detection
 *  @return error code
 */
UINT32 SvcODDrawTask_Stop(void)
{
    UINT32 RetVal;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    SvcLog_DBG(SVC_LOG_OD_TASK, "SvcODDrawTask_Stop", 0U, 0U);

    if (ODInit == 1U) {
        ODOnOff = 0U;
        RetVal = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_OD_TASK, "OD module not init yet. Cannot stop", 0U, 0U);
        RetVal = SVC_NG;
    }

    /* Unregister */
    if ((SVC_OK == RetVal) && (0U == (pResCfg->UserFlag & SVC_USER_FLAG_OD_DRAW_OFF))) {
        UINT32 i;
        UINT32 CvFlowNum = pResCfg->CvFlowNum;
        UINT32 CvFlowBits = pResCfg->CvFlowBits;
        const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;

        for (i = 0U; i < CvFlowNum; i++) {
            if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_OD) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_OD) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_OD_FDAG) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_OD_FDAG)) {
                    RetVal = SvcCvFlow_Unregister(i, ODDrawCvFlowRegisterID[i]);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_OD_TASK, "SvcODDrawTask_Stop err. SvcCvFlow_Stop failed %d", RetVal, 0U);
                    }
                }
            }
        }

        for (i = 0U; i < SVC_CV_FLOW_CHAN_MAX; i++) {
            ODDrawTask_DetResFovDeInit(i);
        }
    }

    if (SVC_OK == RetVal) {
        SVC_APP_STAT_OD_s ODStat = { .Status = SVC_APP_STAT_OD_DISABLE };
        RetVal = SvcSysStat_Issue(SVC_APP_STAT_OD, &ODStat);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_OD_TASK, "App Status issue failed", 0U, 0U);
        }
    }

    return RetVal;
}

/**
 *  Control for the OD Draw task
 *  @param[in] pCmd: Command string
 *  @param[in] Param: Parameter
 *  @return error code
 */
UINT32 SvcODDrawTask_Ctrl(const char *pCmd, UINT32 Param)
{
    if (0 == SvcWrap_strcmp("detres", pCmd)) {
        ODPrintDetRes = Param;
    } else if (0 == SvcWrap_strcmp("category_label", pCmd)) {
        ODLabelCat = Param;
    } else if (0 == SvcWrap_strcmp("score_label", pCmd)) {
        ODLabelScore = Param;
    } else if (0 == SvcWrap_strcmp("roi", pCmd)) {
        ODDrawRoi = Param;
    } else if (0 == SvcWrap_strcmp("kp", pCmd)) {
        ODDrawKp = Param;
    } else if (0 == SvcWrap_strcmp("mask", pCmd)) {
        ODDrawMask = Param;
    } else if (0 == SvcWrap_strcmp("category_clf", pCmd)) {
        ODLabelClfCat = Param;
    } else if (0 == SvcWrap_strcmp("bbx_allow_bits", pCmd)) {
        ODCatAllowBits = Param;
    } else {
        /* Do nothing */
    }

    return SVC_OK;
}

/**
 *  Enable for the OD Draw task
 *  @param[in] Enable: Enable
 *  @return error code
 */
UINT32 SvcODDrawTask_Enable(UINT32 Enable)
{
    UINT32 i, RetVal = SVC_NG;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;
    UINT32 CvFlowNum  = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;

    if ((0U == Enable) /*&& (1U == ODOnOff)*/) { //Unregister CV call-back
        for (i = 0U; i < CvFlowNum; i++) {
            if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_OD) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_OD) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_OD_FDAG) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_OD_FDAG)) {
                    RetVal = SvcCvFlow_Unregister(i, ODDrawCvFlowRegisterID[i]);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_OD_TASK, "SvcCvFlow_Unregister failed %d", RetVal, 0U);
                    } else {/*
                        if (ODInit == 1U) {
                            ODOnOff = 0U;
                        }*/
                        SvcLog_OK(SVC_LOG_OD_TASK, "SvcCvFlow_Unregister success %d", 0U, 0U);
                    }
                }
            }
        }
    } else if ((1U == Enable)/* && (0U == ODOnOff)*/) { //Register CV call-back
        for (i = 0U; i < CvFlowNum; i++) {
            if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_OD) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_OD) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_OD_FDAG) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_OD_FDAG)) {
                    RetVal = SvcCvFlow_Register(i, ODDrawTask_RefODResult, &ODDrawCvFlowRegisterID[i]);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_OD_TASK, "SvcCvFlow_Register failed %d", RetVal, 0U);
                    } else {/*
                        if (ODInit == 1U) {
                            ODOnOff = 1U;
                        }*/
                        SvcLog_OK(SVC_LOG_OD_TASK, "SvcCvFlow_Register success %d", 0U, 0U);
                    }
                }
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_OD_TASK, "Something wrong of SvcODDrawTask_Enable() API, Enable = %d, ODOnOff = %d",
                                    Enable, ODOnOff);
    }

    return RetVal;
}

/**
 *  Get current status of Object detection task
 *  @param[in] pEnable: Get current OD module status (enable or disable)
 *  @return error code
 */
UINT32 SvcODDrawTask_GetStatus(UINT32 *pEnable)
{
    UINT32 RetVal;

    if (ODInit == 1U) {
        *pEnable = ODOnOff;
        RetVal = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_OD_TASK, "OD module not init yet. Cannot get status", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
 *  Reset FOV info
 */
void SvcODDrawTask_ResetFoV(void){
    UINT32 RetVal = SVC_NG;
    UINT32 i;

    for(i = 0U; i < SVC_APP_CV_FLOW_NUM_MAX; i++){
        ODDrawTask_DetResFovDeInit(i);
    }

    RetVal = AmbaWrap_memset(&g_ODDrawInfo[0], 0, sizeof(g_ODDrawInfo));
    RetVal |= AmbaWrap_memset(&g_ODDrawCtrl[0], 0, sizeof(g_ODDrawCtrl));
    if (RetVal != SVC_OK) {
        SvcLog_NG(SVC_LOG_OD_TASK, "AmbaWrap_memset error", 0U, 0U);
    }

}

/**
 *  check if OD started
 */
void SvcODDrawTask_CheckStart(UINT32 *pStarted)
{
    *pStarted = ODStarted;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ODDrawTask_DetectionResultHandler
 *
 *  @Description:: Callback function for Detection Result CV
 *
 *  @Input      ::
 *                 CvFlowChan: CvFlow Channel
 *                 CvType: CvFlow Type
 *                 *pDetResMsg: The pointer to the Object detection result message for drawing bonding box
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 None
\*-----------------------------------------------------------------------------------------------*/
static void ODDrawTask_DetectionResultHandler(UINT32 CvFlowChan, UINT32 CvType, void *pDetResMsg)
{
    const UINT32 *pMsg;
    UINT32 OsdUpdate = 0, Update = 0;

    AmbaMisra_TypeCast(&pMsg, &pDetResMsg);

    if (*pMsg == SVC_CV_DETRES_BBX) {
        const SVC_CV_DETRES_BBX_LIST_s *pList;

        (void) pDetResMsg;
        AmbaMisra_TypeCast(&pList, &pDetResMsg);
        if (CvFlowChan < SVC_CV_FLOW_CHAN_MAX) {
            #if defined(CONFIG_ICAM_TIMING_LOG)
            { /* for SVC timing measurement */
                static UINT8 Done = 0U;
                if (0U == Done) {
                    SvcTime(SVC_TIME_CV_FLEXIDAG_1ST_RESULT, "CV FlexiDag 1st result");
                    SvcTime_PrintBootTimeCv();
                    Done = 1U;
                }
            }
            #endif

            ODDrawTask_DetResFovInit(CvFlowChan);

            ODDrawTask_DetResUpdateDrawInfo(CvFlowChan);

            if (ODDrawRoi == 1U) {
                ODDrawTask_DetResUpdateRoiInfo(CvFlowChan);
            }

            ODDrawTask_DetResBbxOsdPosConv(CvFlowChan, pList);

            ODDrawTask_DetResBbxOsdProc(CvFlowChan, pList, &OsdUpdate);
            if(OsdUpdate == 1U) {
                ODDrawTask_OsdFlush(g_ODDrawCtrl[CvFlowChan].pDrawInfo->OsdReg.Index, CvFlowChan);
            }

            ODDrawTask_PrintDetInfo(CvFlowChan, pList);

            ODStarted = 1;
        } else {
            SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_DetResBbxProc invalid Fov_id", 0U, 0U);
        }
    } else if (*pMsg == SVC_CV_DETRES_BBX_REBEL) {
        const SVC_CV_DETRES_BBX_LIST_REBEL_s *pListRebel;

        (void) pDetResMsg;
        AmbaMisra_TypeCast(&pListRebel, &pDetResMsg);
        if (CvFlowChan < SVC_CV_FLOW_CHAN_MAX) {
            #if defined(CONFIG_ICAM_TIMING_LOG)
            { /* for SVC timing measurement */
                static UINT8 Done = 0U;
                if (0U == Done) {
                    SvcTime(SVC_TIME_CV_FLEXIDAG_1ST_RESULT, "CV FlexiDag 1st result");
                    SvcTime_PrintBootTimeCv();
                    Done = 1U;
                }
            }
            #endif

            ODDrawTask_DetResFovInit(CvFlowChan);

            ODDrawTask_DetResUpdateDrawInfo(CvFlowChan);

            if (ODDrawRoi == 1U) {
                ODDrawTask_DetResUpdateRoiInfo(CvFlowChan);
            }

            ODDrawTask_DetResBbxOsdPosConvRebel(CvFlowChan, pListRebel);

            ODDrawTask_DetResBbxOsdProcRebel(CvFlowChan, pListRebel, &OsdUpdate);
            if(OsdUpdate == 1U) {
                ODDrawTask_OsdFlush(g_ODDrawCtrl[CvFlowChan].pDrawInfo->OsdReg.Index, CvFlowChan);
            }

//            ODDrawTask_PrintDetInfo(CvFlowChan, pListRebel);

            ODStarted = 1;
        } else {
            SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_DetResBbxProc invalid Fov_id", 0U, 0U);
        }
        {

            UINT64 CapSequence;

            CapSequence = (UINT64)pListRebel->FrameNum;

            if (SVC_OK != SvcVoutFrmCtrlTask_Ctrl(SVC_VFC_TASK_CMD_CAP_SEQ, &CapSequence)) {
                SvcLog_NG(SVC_LOG_OD_TASK, "Send vout sync info failed", 0U, 0U);
            }

        }
    } else if (*pMsg == SVC_CV_PERCEPTION_OUTPUT) {
        const SVC_CV_PERCEPTION_OUTPUT_s *pPcptOut;

        (void) pDetResMsg;
        AmbaMisra_TypeCast(&pPcptOut, &pDetResMsg);
        if (CvFlowChan < SVC_CV_FLOW_CHAN_MAX) {
            #if defined(CONFIG_ICAM_TIMING_LOG)
            { /* for SVC timing measurement */
                static UINT8 Done = 0U;
                if (0U == Done) {
                    SvcTime(SVC_TIME_CV_FLEXIDAG_1ST_RESULT, "CV FlexiDag 1st result");
                    SvcTime_PrintBootTimeCv();
                    Done = 1U;
                }
            }
            #endif

            ODDrawTask_DetResFovInit(CvFlowChan);

            ODDrawTask_DetResUpdateDrawInfo(CvFlowChan);

            if (ODDrawRoi == 1U) {
                ODDrawTask_DetResUpdateRoiInfo(CvFlowChan);
            }

            ODDrawTask_DetResBbxOsdPosConv(CvFlowChan, pPcptOut->pBbxList);

            ODDrawTask_DetResPcptOsdProc(CvFlowChan, pPcptOut, &Update);
            OsdUpdate |= Update;

            ODDrawTask_DetResBbxOsdProc(CvFlowChan, pPcptOut->pBbxList, &Update);
            OsdUpdate |= Update;

            if(OsdUpdate == 1U) {
                ODDrawTask_OsdFlush(g_ODDrawCtrl[CvFlowChan].pDrawInfo->OsdReg.Index, CvFlowChan);
            }

            ODDrawTask_PrintDetInfo(CvFlowChan, pPcptOut->pBbxList);

            ODStarted = 1;
        } else {
            SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_DetResBbxProc invalid Fov_id", 0U, 0U);
        }
    } else {
        /* Do nothing */
    }

    AmbaMisra_TouchUnused(&CvType);
}

#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
#define FC_DISP_MODE_PCPT       (0U)
#define FC_DISP_MODE_3D         (1U)
static void ODDrawTask_FcPcptDraw(void *pDetResMsg, UINT32 DispMode)
{
    const UINT32 *pMsg;
    UINT32 Update = 0;

    AmbaMisra_TypeCast(&pMsg, &pDetResMsg);

    if (*pMsg == SVC_CV_PERCEPTION_OUTPUT) {
        const SVC_FC_DRAW_INFO_V2_s *pList;
        UINT32 CvFlowChan, CvType;

        (void) pDetResMsg;
        AmbaMisra_TypeCast(&pList, &pDetResMsg);

        CvFlowChan = pList->Chan;
        if (CvFlowChan < SVC_CV_FLOW_CHAN_MAX) {
            CvType = pList->CvType;

            ODDrawTask_DetResFovInit(CvFlowChan);

            ODDrawTask_DetResUpdateDrawInfo(CvFlowChan);

            if (ODDrawRoi == 1U) {
                ODDrawTask_DetResUpdateRoiInfo(CvFlowChan);
            }

            ODDrawTask_DetResBbxOsdPosConv(CvFlowChan, &pList->BbxOut);

            if (DispMode == FC_DISP_MODE_PCPT) {
                ODDrawTask_DetResPcptOsdProc(CvFlowChan, &pList->PcptOut, &Update);
            }

            ODDrawTask_FcDetRes3DBbxOsdProc(CvFlowChan, pList);

            ODStarted = 1;
            AmbaMisra_TouchUnused(&CvType);
        } else {
            SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_DetResBbxProc invalid Fov_id", 0U, 0U);
        }
    } else if (*pMsg == SVC_CV_OSD_BUFFER) {

    } else {
        SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_DetResBbxProc invalid msg", 0U, 0U);
    }
}

static void ODDrawTask_FcDraw(void *pDetResMsg)
{
    const UINT32 *pMsg;

    AmbaMisra_TypeCast(&pMsg, &pDetResMsg);

    if (*pMsg == SVC_CV_DETRES_BBX) {
        const SVC_FC_DRAW_INFO_s *pList;
        UINT32 CvFlowChan, CvType;

        (void) pDetResMsg;
        AmbaMisra_TypeCast(&pList, &pDetResMsg);

        CvFlowChan = pList->Chan;
        if (CvFlowChan < SVC_CV_FLOW_CHAN_MAX) {
            CvType = pList->CvType;

            ODDrawTask_DetResFovInit(CvFlowChan);

            ODDrawTask_DetResUpdateDrawInfo(CvFlowChan);

            if (ODDrawRoi == 1U) {
                ODDrawTask_DetResUpdateRoiInfo(CvFlowChan);
            }

            ODDrawTask_DetResBbxOsdPosConv(CvFlowChan, &pList->BbxOut);

            ODDrawTask_FcDetResBbxOsdProc(CvFlowChan, pList);

            ODStarted = 1;
            AmbaMisra_TouchUnused(&CvType);
        } else {
            SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_DetResBbxProc invalid Fov_id", 0U, 0U);
        }
    } else if (*pMsg == SVC_CV_OSD_BUFFER) {

    } else {
        SvcLog_NG(SVC_LOG_OD_TASK, "?ODDrawTask_DetResBbxProc invalid msg", 0U, 0U);
    }
}
#endif

#if defined(CONFIG_ICAM_CV_LOGGER)
static void ODDrawTask_DataLoggerHandler(UINT32 CvFlowChan, void *pOutput)
{
    UINT32 RetVal;
    UINT32 i;
    const UINT32 *pMsg;
    UINT32 RegSize, PayloadSize;
    SVC_CV_LOGGER_WRITE_BUFFER_s WrBuf = {.Size = 0};
    RF_LOG_HEADER_V0_s *pHeader;
    typedef struct { void **pDPtr; } VOID_PTR_s;
    VOID_PTR_s VoidPtr;

    if (1U == SvcCvLoggerTask_IsEnable(CvFlowChan)) {
        AmbaMisra_TypeCast(&pMsg, &pOutput);

        if (*pMsg == SVC_CV_DETRES_BBX) {
            #define OD_2DBBX_HEADER_SIZE    (4U * sizeof(UINT32))
            const SVC_CV_DETRES_BBX_LIST_s *pResult;
            AMBA_OD_2DBBX_LIST_s *pLogOut;

            AmbaMisra_TypeCast(&pResult, &pOutput);

            PayloadSize = OD_2DBBX_HEADER_SIZE + (pResult->BbxAmount * sizeof(AMBA_OD_2DBBX_s));
            RegSize = SVC_CV_LOGGER_HEADER_SIZE + PayloadSize;
            WrBuf.Size = 0U;
            RetVal = SvcCvLoggerTask_TrunkBufferGet(CvFlowChan, RegSize, SVC_CV_LOGGER_FLAG_LOG_TRUNK, &WrBuf);
            if ((RetVal == SVC_OK) && (WrBuf.Size >= RegSize)) {
                AmbaMisra_TypeCast(&pHeader, &WrBuf.Addr);
                /* pLogOut = &pHeader->pDataAddr */
                VoidPtr.pDPtr = &pHeader->pDataAddr;
                AmbaMisra_TypeCast(&pLogOut, &VoidPtr);

                pHeader->HdrVer   = RF_DATA_FMT_HDR_VER;
                pHeader->DataFmt  = RF_FC_DFMT_2DBBX_OUT;
                pHeader->Count    = 0U;
                pHeader->Channel  = 0U;
                pHeader->CapPTS   = pResult->CaptureTime;
                pHeader->DataSize = PayloadSize;

                pLogOut->MsgCode     = pResult->MsgCode;
                pLogOut->CaptureTime = pResult->CaptureTime;
                pLogOut->FrameNum    = pResult->FrameNum;
                pLogOut->NumBbx      = pResult->BbxAmount;
                for (i = 0; i < pResult->BbxAmount; i++){
                    pLogOut->Bbx[i].Cat      = pResult->Bbx[i].Cat;
                    pLogOut->Bbx[i].Fid      = pResult->Bbx[i].Fid;
                    pLogOut->Bbx[i].ObjectId = pResult->Bbx[i].ObjectId;
                    pLogOut->Bbx[i].Score    = pResult->Bbx[i].Score;
                    pLogOut->Bbx[i].X        = pResult->Bbx[i].X;
                    pLogOut->Bbx[i].Y        = pResult->Bbx[i].Y;
                    pLogOut->Bbx[i].W        = pResult->Bbx[i].W;
                    pLogOut->Bbx[i].H        = pResult->Bbx[i].H;
                }

                RetVal = SvcCvLoggerTask_TrunkBufferUpdate(CvFlowChan, &WrBuf, RegSize);
                if (SVC_OK != RetVal) {
                    SvcLog_DBG(SVC_LOG_OD_TASK, "TrunkBufferUpdate(%d) failed(0x%x)", CvFlowChan, RetVal);
                }
            } else {
                SvcLog_DBG(SVC_LOG_OD_TASK, "CvFlowChan(%d) WriteBufferGet failed(0x%x)", CvFlowChan, RetVal);
            }
        }
    }

    AmbaMisra_TouchUnused(pOutput);
}
#endif

#if defined(CONFIG_ICAM_ENABLE_VOUT_FRAME_CTRL)
static void ODDrawTask_VoutFrmCtrl(UINT32 CvFlowChan, void *pOutput)
{
    const UINT32 *pMsg;
    const SVC_CV_PERCEPTION_OUTPUT_s *pPcptOut;
    UINT64 CapSequence;

    AmbaMisra_TypeCast(&pMsg, &pOutput);
    if (*pMsg == SVC_CV_PERCEPTION_OUTPUT) {
        if (CvFlowChan == 0U) {
            AmbaMisra_TypeCast(&pPcptOut, &pOutput);
            CapSequence = (UINT64)pPcptOut->pBbxList->FrameNum;

            if (SVC_OK != SvcVoutFrmCtrlTask_Ctrl(SVC_VFC_TASK_CMD_CAP_SEQ, &CapSequence)) {
                SvcLog_NG(SVC_LOG_OD_TASK, "Send vout sync info failed", 0U, 0U);
            }
        }
    }

    AmbaMisra_TouchUnused(pOutput);
}
#endif

/**
 *  OD Draw Task message handler
 *  @param[in] MsgCode message code to point out which FC msg draw
 *  @param[in] pInfo draw info
 *  @return error code
 */
UINT32 SvcODDrawTask_Msg(UINT32 MsgCode, void *pInfo)
{
    UINT32 RetVal = SVC_OK;

    switch (MsgCode) {
    case SVC_FC_PCPT_DRAW:
#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
        ODDrawTask_FcPcptDraw(pInfo, FC_DISP_MODE_PCPT);
#if defined(CONFIG_ICAM_ENABLE_VOUT_FRAME_CTRL)
        {
            SVC_FC_DRAW_INFO_V2_s *pList;
            AmbaMisra_TypeCast(&pList, &pInfo);
            ODDrawTask_VoutFrmCtrl(pList->Chan, &pList->PcptOut);
        }
#endif
#endif
        break;
    case SVC_FC_3D_DRAW:
#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
        ODDrawTask_FcPcptDraw(pInfo, FC_DISP_MODE_3D);
#endif
        break;
    case SVC_FC_DRAW:
#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
        ODDrawTask_FcDraw(pInfo);
#endif
        break;
    case SVC_BSD_DRAW:
#if defined(CONFIG_ICAM_PROJECT_EMIRROR) && defined(CONFIG_BUILD_AMBA_ADAS)
        {
            SVC_BSD_DRAW_INFO_s *pBsdDrawInfo;
            AmbaMisra_TypeCast(&pBsdDrawInfo, &pInfo);
            ODDrawTask_BsdDraw(pBsdDrawInfo);
        }
#endif
        break;
    case SVC_RMG_DRAW:
#if defined(CONFIG_ICAM_PROJECT_EMIRROR) && defined(CONFIG_BUILD_AMBA_ADAS)
        {
            SVC_RMG_DRAW_INFO_s *pRmgDrawInfo;
            AmbaMisra_TypeCast(&pRmgDrawInfo, &pInfo);
            ODDrawTask_RmgDraw(pRmgDrawInfo);
        }
#endif
        break;
    case SVC_EMA_DRAW:
#if defined(CONFIG_ICAM_IMGCAL_STITCH_USED)
        {
            SVC_EMA_DRAW_INFO_s *pEmaDrawInfo;
            AmbaMisra_TypeCast(&pEmaDrawInfo, &pInfo);
            ODDrawTask_EmaDraw(pEmaDrawInfo);
        }
#endif
        break;
    default:
        RetVal = SVC_NG;
        break;
    }

    AmbaMisra_TouchUnused(pInfo);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ODDrawTask_RefODResult
 *
 *  @Description:: The callback function for differnt OD result
 *
 *  @Input      ::
 *                 CvFlowChan: CvFlow channel
 *                 CvType: CvFlow Type
 *                 pOutput: The OD result
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 None
\*-----------------------------------------------------------------------------------------------*/
static void ODDrawTask_RefODResult(UINT32 CvFlowChan, UINT32 CvType, void *pOutput)
{
    AmbaMisra_TouchUnused(&CvFlowChan);

    ODDrawTask_DetectionResultHandler(CvFlowChan, CvType, pOutput);
#if defined(CONFIG_ICAM_ENABLE_VOUT_FRAME_CTRL)
    ODDrawTask_VoutFrmCtrl(CvFlowChan, pOutput);
#endif
#if defined(CONFIG_ICAM_CV_LOGGER)
    ODDrawTask_DataLoggerHandler(CvFlowChan, pOutput);
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ODDrawTask_DetResFovInit
 *
 *  @Description:: Initialize FOV related configuration
 *
 *  @Input      ::
 *                 CvFlowChan: CvFlow channel
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 None
\*-----------------------------------------------------------------------------------------------*/
static void ODDrawTask_DetResFovInit(UINT32 CvFlowChan)
{
    const SVC_RES_CFG_s  *pCfg = SvcResCfg_Get();
    const SVC_CV_FLOW_s *pCvFlow = pCfg->CvFlow;
    UINT32 i, j, k, Found = 0;
    OD_DRAW_INFO_s *pDrawInfo = NULL;
    const SVC_CHAN_CFG_s  *pChanCfg = NULL;
    const SVC_DISP_STRM_s *pDispStrm = NULL;
    UINT32 VoutPri[MAX_VOUT_CHAN_NUM];
    UINT32 Value, MixerWidth, MixerHeight, OsdWidth, OsdHeight;
    SVC_GUI_CANVAS_s GuiCanvas;
    AMBA_DSP_WINDOW_DIMENSION_s MainWin, HierWin;
    UINT32 ScaleFactor;
    UINT32 FovIdx = 0;
    UINT32 DispNum = 0U;
    const SVC_DISP_STRM_s *pDisp = NULL;

    if (g_ODDrawCtrl[CvFlowChan].Init == 0U) {
        g_ODDrawCtrl[CvFlowChan].Init = 1U;

        for (i = 0; i < SVC_APP_CV_FLOW_NUM_MAX; i++) {
            if (g_ODDrawInfo[i].Used == 0U) {
                g_ODDrawInfo[i].Used = 1;
                pDrawInfo = &g_ODDrawInfo[i];
                pDrawInfo->GuiLevel = ODDrawTask_GuiLevel(i);
                break;
            }
        }

        if ((pDrawInfo != NULL) && (CvFlowChan < SVC_RES_CV_FLOW_NUM)) {
            /* Get Display config */
            VoutPri[0] = VOUT_IDX_A;
            VoutPri[1] = VOUT_IDX_A;

            if(pCfg->DispAltNum > 0U){
                pDisp = pCfg->DispAlt[pCfg->DispAltIdx].DispStrm;
                DispNum = pCfg->DispAlt[pCfg->DispAltIdx].DispNum;
            } else{
                pDisp = pCfg->DispStrm;
                DispNum = pCfg->DispNum;
            }

            for (k = 0; k < MAX_VOUT_CHAN_NUM; k++) {
                /* Search corresponding VOUT */
                for (i = 0; i < DispNum; i++) {
                    if (pDisp[i].VoutID == VoutPri[k]) {
                        /* Search corresponding FOV */
                        for (j = 0; j < pDisp[i].StrmCfg.NumChan; j++) {
                            FovIdx = pCvFlow[CvFlowChan].InputCfg.Input[0].StrmId;
                            if (FovIdx == pDisp[i].StrmCfg.ChanCfg[j].FovId) {
                                pDispStrm = &pDisp[i];
                                pChanCfg  = &pDisp[i].StrmCfg.ChanCfg[j];
                                Found = 1U;
                                AmbaPrint_PrintUInt5("ODDrawTask: Found FOV%d DispStrm[%u].StrmCfg.ChanCfg[%u]", FovIdx, i, j, 0U, 0U);
                                break;
                            }
                        }
                    }
                    if (1U == Found) {
                        break;
                    }
                }
                if (1U == Found) {
                    break;
                }
            }

            if ((NULL != pDispStrm) && (NULL != pChanCfg)) {
                /* Get FOV config */
                MainWin.Width         = pCfg->FovCfg[FovIdx].MainWin.Width;
                MainWin.Height        = pCfg->FovCfg[FovIdx].MainWin.Height;
                SvcWinCalc_CalcPyramidHierWin((pCfg->FovCfg[FovIdx].PyramidFlag & SVC_RES_PYRAMID_DOWNSCALE_2X),&MainWin, &HierWin);

                pDrawInfo->MainWidth  = MainWin.Width;
                pDrawInfo->MainHeight = MainWin.Height;
                pDrawInfo->HierWidth  = HierWin.Width;
                pDrawInfo->HierHeight = HierWin.Height;

                /*AmbaPrint_PrintUInt5("ODDrawTask: Main %u %u Hier %u %u",
                    pDrawInfo->MainWidth,
                    pDrawInfo->MainHeight,
                    pDrawInfo->HierWidth,
                    pDrawInfo->HierHeight,
                    0U);*/

                /* Calculate pyramid dimension */
                for (i = 0; i < AMBA_DSP_MAX_HIER_NUM; i += 2U) {
                    ScaleFactor = (i >> 1); /* 0, 2, 4 to 0, 1, 2 */
                    pDrawInfo->PyramidDim[i].Index = i;
                    pDrawInfo->PyramidDim[i].W = (pDrawInfo->MainWidth >> ScaleFactor);
                    pDrawInfo->PyramidDim[i].H = (pDrawInfo->MainHeight >> ScaleFactor);

                    AmbaPrint_PrintUInt5("PyramidDim: N %u W %u H %u",
                        pDrawInfo->PyramidDim[i].Index, pDrawInfo->PyramidDim[i].W, pDrawInfo->PyramidDim[i].H, 0U, 0U);
                }

                for (i = 1; i < AMBA_DSP_MAX_HIER_NUM; i+= 2U) {
                    ScaleFactor = (i >> 1); /* 1, 3, 5 to 0, 1, 2 */
                    pDrawInfo->PyramidDim[i].Index = i;
                    pDrawInfo->PyramidDim[i].W = (pDrawInfo->HierWidth >> ScaleFactor);
                    pDrawInfo->PyramidDim[i].H = (pDrawInfo->HierHeight >> ScaleFactor);

                    AmbaPrint_PrintUInt5("PyramidDim: N %u W %u H %u",
                        pDrawInfo->PyramidDim[i].Index, pDrawInfo->PyramidDim[i].W, pDrawInfo->PyramidDim[i].H, 0U, 0U);
                }

                pDrawInfo->PrevSrc.X = pChanCfg->SrcWin.OffsetX;
                pDrawInfo->PrevSrc.Y = pChanCfg->SrcWin.OffsetY;
                pDrawInfo->PrevSrc.W = pChanCfg->SrcWin.Width;
                pDrawInfo->PrevSrc.H = pChanCfg->SrcWin.Height;
                pDrawInfo->pPrevChan = pChanCfg;

                AmbaPrint_PrintUInt5("ODDrawTask: PrevSrc %u %u %u %u",
                    pDrawInfo->PrevSrc.X,
                    pDrawInfo->PrevSrc.Y,
                    pDrawInfo->PrevSrc.W,
                    pDrawInfo->PrevSrc.H,
                    0U);

                /* Calculate OSD region with resepect to OSD buffer */
                /* Suppose max OSD window is the same as Mixer window */
                if (SVC_OK != SvcOsd_GetOsdBufSize(pDispStrm->VoutID, &OsdWidth, &OsdHeight)) {
                    SvcLog_NG(SVC_LOG_OD_TASK, "SvcOsd_GetOsdBufSize failed", 0U, 0U);
                }
                MixerWidth  = pDispStrm->StrmCfg.MaxWin.Width;
                MixerHeight = pDispStrm->StrmCfg.MaxWin.Height;

                if (pDispStrm->VideoRotateFlip == (UINT8) AMBA_DSP_ROTATE_0_HORZ_FLIP) {
                    Value = (UINT32) pDispStrm->StrmCfg.Win.Width - ((UINT32) pChanCfg->DstWin.OffsetX + (UINT32) pChanCfg->DstWin.Width);
                } else {
                    Value = pChanCfg->DstWin.OffsetX;
                }
                pDrawInfo->OsdReg.Index = pDispStrm->VoutID;
                pDrawInfo->OsdReg.X     = ((Value + ((MixerWidth - pDispStrm->StrmCfg.Win.Width) >> 1)) * OsdWidth) / MixerWidth;
                pDrawInfo->OsdReg.Y     = ((pChanCfg->DstWin.OffsetY + ((MixerHeight - pDispStrm->StrmCfg.Win.Height) >> 1)) * OsdHeight) / MixerHeight;
                pDrawInfo->OsdReg.W     = (pChanCfg->DstWin.Width * OsdWidth) / MixerWidth;
                pDrawInfo->OsdReg.H     = (pChanCfg->DstWin.Height * OsdHeight) / MixerHeight;
                pDrawInfo->OsdRegFlip   = pDispStrm->VideoRotateFlip;
                GuiCanvas.StartX        = pDrawInfo->OsdReg.X;
                GuiCanvas.StartY        = pDrawInfo->OsdReg.Y;
                GuiCanvas.Width         = pDrawInfo->OsdReg.W;
                GuiCanvas.Height        = pDrawInfo->OsdReg.H;

                /* Hook up Draw info before register GUI */
                g_ODDrawCtrl[CvFlowChan].pDrawInfo = pDrawInfo;

                /* Reset g_LastNumBbx */
                g_LastNumBbx[CvFlowChan] = 0xffff;

#if defined(CONFIG_ICAM_ENABLE_VOUT_DEF_IMG)
                (void)ODDrawTask_OsdDraw;
                (void)ODDrawTask_OsdUpdate;
                SvcLog_DBG(SVC_LOG_OD_TASK, "ODDrawTask:Skip GUI register, due to eanble vout default image", 0U, 0U);
#else
                SvcGui_Register(pDispStrm->VoutID, pDrawInfo->GuiLevel, "ODDraw", ODDrawTask_OsdDraw, ODDrawTask_OsdUpdate);
                AmbaPrint_PrintUInt5("ODDrawTask: Register GUI Canvas: Level %u X %u Y %u W %u H %u",
                    pDrawInfo->GuiLevel,
                    GuiCanvas.StartX,
                    GuiCanvas.StartY,
                    GuiCanvas.Width,
                    GuiCanvas.Height);
#endif
                SvcLog_DBG(SVC_LOG_OD_TASK, "ODDrawTask_DetResFovInit Done", 0U, 0U);
            } else {
                AmbaPrint_PrintUInt5("ODDrawTask_DetResFovInit: Not found DispStrm", 0U, 0U, 0U, 0U, 0U);
            }
        } else {
            SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_DetResFovInit fail", 0U, 0U);
        }
    }
}

static void ODDrawTask_DetResFovDeInit(UINT32 CvFlowChan)
{
    const OD_DRAW_CTRL_s *pCtrl = &g_ODDrawCtrl[CvFlowChan];

    if ((pCtrl->Init == 1U) &&
        (pCtrl->pDrawInfo != NULL) &&
        (pCtrl->pDrawInfo->Used == 1U)) {
#if defined(CONFIG_ICAM_ENABLE_VOUT_DEF_IMG)
        SvcLog_DBG(SVC_LOG_OD_TASK, "ODDrawTask:Skip GUI unregister, due to eanble vout default image", 0U, 0U);
#else
        SvcGui_Unregister(pCtrl->pDrawInfo->OsdReg.Index, pCtrl->pDrawInfo->GuiLevel);
#endif
        SvcLog_DBG(SVC_LOG_OD_TASK, "ODDrawTask_DetResFovDeInit Done", 0U, 0U);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ODDrawTask_DetResUpdateDrawInfo
 *
 *  @Description:: Update Draw info
 *
 *  @Input      ::
 *                 CvFlowChan: CvFlow channel
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 None
\*-----------------------------------------------------------------------------------------------*/
static void ODDrawTask_DetResUpdateDrawInfo(UINT32 CvFlowChan)
{
    UINT32 RetVal;
    OD_DRAW_INFO_s *pDrawInfo = g_ODDrawCtrl[CvFlowChan].pDrawInfo;

    if (pDrawInfo != NULL) {
        /* Reset draw flag */
        RetVal = AmbaWrap_memset(pDrawInfo->BbxDrawFlag, 0, sizeof(pDrawInfo->BbxDrawFlag));
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_DetResUpdateDrawInfo: memset failed %d", RetVal, 0U);
        }

        /* Update Preview crop window */
        if (pDrawInfo->pPrevChan != NULL) {
            pDrawInfo->PrevSrc.X = pDrawInfo->pPrevChan->SrcWin.OffsetX;
            pDrawInfo->PrevSrc.Y = pDrawInfo->pPrevChan->SrcWin.OffsetY;
            pDrawInfo->PrevSrc.W = pDrawInfo->pPrevChan->SrcWin.Width;
            pDrawInfo->PrevSrc.H = pDrawInfo->pPrevChan->SrcWin.Height;
        }

        if ((pDrawInfo->MainWidth != pDrawInfo->PrevSrc.W) || (pDrawInfo->MainHeight!= pDrawInfo->PrevSrc.H)) {
            pDrawInfo->PrevCrop = 1U;
        }
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ODDrawTask_DetResUpdateRoiInfo
 *
 *  @Description:: Update ROI info
 *
 *  @Input      ::
 *                 CvFlowChan: CvFlow channel
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 None
\*-----------------------------------------------------------------------------------------------*/
static void ODDrawTask_DetResUpdateRoiInfo(UINT32 CvFlowChan)
{
    UINT32 RetVal;
    OD_DRAW_INFO_s *pDrawInfo = g_ODDrawCtrl[CvFlowChan].pDrawInfo;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    SVC_CV_ROI_INFO_s RoiInfo = {0};
    UINT32 i, PyramidIndex, PyramidWidth, PyramidHeight;

    if (pDrawInfo != NULL) {
        if (CvFlowChan < SVC_RES_CV_FLOW_NUM) {
            if (pResCfg->CvFlow[CvFlowChan].CvFlowType == SVC_CV_FLOW_AMBA_OD) {
                /* Retrieve ROI window from CV if exists */
                RetVal = SvcCvFlow_Control(CvFlowChan, SVC_CV_CTRL_GET_ROI_INFO, &RoiInfo);
                if (RetVal == SVC_OK) {
                    for (i = 0; i < RoiInfo.NumRoi; i++) {
                        if ((pDrawInfo->RoiInfo.Roi[i].StartX != RoiInfo.Roi[i].StartX) ||
                            (pDrawInfo->RoiInfo.Roi[i].StartY != RoiInfo.Roi[i].StartY) ||
                            (pDrawInfo->RoiInfo.Roi[i].Width  != RoiInfo.Roi[i].Width)  ||
                            (pDrawInfo->RoiInfo.Roi[i].Height != RoiInfo.Roi[i].Height)) {
                            if (SVC_OK != AmbaWrap_memcpy(&pDrawInfo->RoiInfo.Roi[i], &RoiInfo.Roi[i], sizeof(SVC_CV_ROI_s))) {
                                SvcLog_NG(SVC_LOG_OD_TASK, "AmbaWrap_memcpy pDrawInfo->RoiInfo.Roi[%d] failed", i, 0U);
                            }

                            PyramidIndex  = pDrawInfo->RoiInfo.Roi[i].Index;
                            PyramidWidth  = pDrawInfo->PyramidDim[PyramidIndex].W;
                            PyramidHeight = pDrawInfo->PyramidDim[PyramidIndex].H;
                            pDrawInfo->Roi_Revt.Roi[i].Index    = 0U;
                            pDrawInfo->Roi_Revt.Roi[i].StartX = (pDrawInfo->RoiInfo.Roi[i].StartX * pDrawInfo->MainWidth) / PyramidWidth;
                            pDrawInfo->Roi_Revt.Roi[i].StartY = (pDrawInfo->RoiInfo.Roi[i].StartY * pDrawInfo->MainHeight) / PyramidHeight;
                            pDrawInfo->Roi_Revt.Roi[i].Width    = (pDrawInfo->RoiInfo.Roi[i].Width * pDrawInfo->MainWidth) / PyramidWidth;
                            pDrawInfo->Roi_Revt.Roi[i].Height   = (pDrawInfo->RoiInfo.Roi[i].Height * pDrawInfo->MainHeight) / PyramidHeight;

                            AmbaPrint_PrintUInt5("ODDrawTask: Got ROI info (%d %d %d %d %d)",
                                pDrawInfo->RoiInfo.Roi[i].Index,
                                pDrawInfo->RoiInfo.Roi[i].StartX,
                                pDrawInfo->RoiInfo.Roi[i].StartY,
                                pDrawInfo->RoiInfo.Roi[i].Width,
                                pDrawInfo->RoiInfo.Roi[i].Height);
                            AmbaPrint_PrintUInt5("Roi_Revt: N %u X %u Y %u W %u H %u",
                                pDrawInfo->Roi_Revt.Roi[i].Index,
                                pDrawInfo->Roi_Revt.Roi[i].StartX,
                                pDrawInfo->Roi_Revt.Roi[i].StartY,
                                pDrawInfo->Roi_Revt.Roi[i].Width,
                                pDrawInfo->Roi_Revt.Roi[i].Height);
                        }
                    }

                    pDrawInfo->RoiInfo.NumRoi = RoiInfo.NumRoi;
                    pDrawInfo->Roi_Revt.NumRoi = RoiInfo.NumRoi;
                } else {
                    AmbaPrint_PrintUInt5("ODDrawTask: No ROI info", 0U, 0U, 0U, 0U, 0U);
                }
            }
        } else {
            SvcLog_NG(SVC_LOG_OD_TASK, "Invalid CvFlowChan(%d)", CvFlowChan, 0U);
        }
    }
}

static void ODDrawTask_PrintDetInfo(UINT32 CvFlowChan, const SVC_CV_DETRES_BBX_LIST_s *pList)
{
    UINT32 i;

    if ((ODPrintDetRes > 0U) || ((DetResCount % 2000U) < 6U)) {
        AmbaPrint_PrintUInt5("DetRes[%d]: Num = %u (%d)", CvFlowChan, pList->BbxAmount, pList->FrameNum, 0U, 0U);
        if (0U != pList->BbxAmount) {
            for (i = 0; i < pList->BbxAmount; i++) {
                AmbaPrint_PrintUInt5("DetRes: C %u X %u Y %u W %u H %u",
                    pList->Bbx[i].Cat,
                    pList->Bbx[i].X,
                    pList->Bbx[i].Y,
                    pList->Bbx[i].W,
                    pList->Bbx[i].H);
            }
        }
    }
    DetResCount++;
    ODStarted = 1;
}

static void ODDrawTask_DetResBbxOsdProcRebel(UINT32 CvFlowChan, const SVC_CV_DETRES_BBX_LIST_REBEL_s *pListRebel, UINT32 *OsdUpdate)
{
    const OD_DRAW_INFO_s *pDrawInfo = g_ODDrawCtrl[CvFlowChan].pDrawInfo;

    if (pDrawInfo != NULL) {
        if (ODDrawRoi == 1U) {
            ODDrawTask_DetResDrawRoi(CvFlowChan);
            *OsdUpdate = 1;
        }

        if ((pListRebel->BbxAmount > 0U) || (pListRebel->BbxAmount != g_LastNumBbx[CvFlowChan])) {
            ODDrawTask_DetResDrawBbxRebel(CvFlowChan, pListRebel);
            *OsdUpdate = 1;
        }
        g_LastNumBbx[CvFlowChan] = pListRebel->BbxAmount;
    }
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ODDrawTask_DetResBbxOsdProc
 *
 *  @Description:: Pass BBX to OSD
 *
 *  @Input      ::
 *                 CvFlowChan: CvFlow channel
 *                 pList: The list contains the bounding box information
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 None
\*-----------------------------------------------------------------------------------------------*/
static void ODDrawTask_DetResBbxOsdProc(UINT32 CvFlowChan, const SVC_CV_DETRES_BBX_LIST_s *pList, UINT32 *OsdUpdate)
{
    const OD_DRAW_INFO_s *pDrawInfo = g_ODDrawCtrl[CvFlowChan].pDrawInfo;

    if (pDrawInfo != NULL) {
        if (ODDrawRoi == 1U) {
            ODDrawTask_DetResDrawRoi(CvFlowChan);
            *OsdUpdate = 1;
        }

        if ((pList->BbxAmount > 0U) || (pList->BbxAmount != g_LastNumBbx[CvFlowChan])) {
            ODDrawTask_DetResDrawBbx(CvFlowChan, pList);
            *OsdUpdate = 1;
        }
        g_LastNumBbx[CvFlowChan] = pList->BbxAmount;
    }
}

static void ODDrawTask_DetResPcptOsdProc(UINT32 CvFlowChan, const SVC_CV_PERCEPTION_OUTPUT_s *pPcptOut, UINT32 *OsdUpdate)
{
    const OD_DRAW_INFO_s *pDrawInfo = g_ODDrawCtrl[CvFlowChan].pDrawInfo;
    UINT32 i;
    const SVC_CV_PERCEPTION_DATA_s *pPcptData;
    const SVC_CV_PERCEPTION_DATA_s *pPcptData2[2U];

    if (pDrawInfo != NULL) {
        for (i = 0; i < SVC_CV_MAX_PCPT_PROC; i++) {
            pPcptData = &pPcptOut->PcptData[i];
            if ((pPcptData->Enable == 1U) && (pPcptData->NumObj > 0U)) {
                if (pPcptData->PcptType == AMBANET_OD37_KP) {
                    if (ODDrawKp != 0U) {
                        ODDrawTask_DetResDrawKp(CvFlowChan, pPcptOut, pPcptData);
                    }
                } else if (pPcptData->PcptType == AMBANET_OD37_MKL) {
                    if (ODDrawMask != 0U) {
                        pPcptData2[0U] = &pPcptOut->PcptData[AMBANET_OD37_MKL];
                        pPcptData2[1U] = &pPcptOut->PcptData[AMBANET_OD37_MKS];

                        ODDrawTask_DetResDrawMask(CvFlowChan, pPcptOut, 2U, pPcptData2);
                    }
                } else if (pPcptData->PcptType == AMBANET_OD37_TS) {
                    if (ODLabelClfCat != 0U) {
                        pPcptData2[0U] = &pPcptOut->PcptData[AMBANET_OD37_TS];
                        pPcptData2[1U] = &pPcptOut->PcptData[AMBANET_OD37_SL];

                        ODDrawTask_DetResShowClfStr_TS(CvFlowChan, pPcptOut, 2U, pPcptData2);
                    }
                } else if ((pPcptData->PcptType == AMBANET_OD37_TLS) ||
                           (pPcptData->PcptType == AMBANET_OD37_AR)) {
                    if (ODLabelClfCat != 0U) {
                        ODDrawTask_DetResShowClfStr(CvFlowChan, pPcptOut, pPcptData);
                    }
                } else {
                    /* Do nothing */
                }
            }
        }

        *OsdUpdate = 1;
    }
}

static void ODDrawTask_KpLine(UINT32 CvFlowChan, UINT32 X1, UINT32 Y1, UINT32 X2, UINT32 Y2, UINT32 Color)
{
    UINT32  OsdWidth, OsdHeight;
    SVC_CV_DISP_OSD_LINE_s   LineCmd;
    SVC_CV_CAMCTRL_MSG_s    CamMsg;
    const OD_DRAW_INFO_s    *pDrawInfo = g_ODDrawCtrl[CvFlowChan].pDrawInfo;

    if (SVC_OK != SvcOsd_GetOsdBufSize(pDrawInfo->OsdReg.Index, &OsdWidth, &OsdHeight)) {
        SvcLog_NG(SVC_LOG_OD_TASK, "SvcOsd_GetOsdBufSize failed", 0U, 0U);
    }
    LineCmd.Channel = pDrawInfo->OsdReg.Index;
    LineCmd.X1 = (X1 * OsdWidth)  / pDrawInfo->MainWidth;
    LineCmd.X2 = (X2 * OsdWidth)  / pDrawInfo->MainWidth;
    LineCmd.Y1 = (Y1 * OsdHeight) / pDrawInfo->MainHeight;
    LineCmd.Y2 = (Y2 * OsdHeight) / pDrawInfo->MainHeight;
    LineCmd.LineWidth = 2U;
    LineCmd.Color = Color;

    CamMsg.Msg = SVC_CV_OSD_DRAW_LINE;
    CamMsg.pCtx = &LineCmd;
    ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);
}

static void ODDrawTask_DetResDrawKp(UINT32 CvFlowChan, const SVC_CV_PERCEPTION_OUTPUT_s *pPcptOut, const SVC_CV_PERCEPTION_DATA_s *pPcptData)
{
    //#define AMBA_KP_NOSE             0
    //#define AMBA_KP_LEFT_EYE         1
    //#define AMBA_KP_RIGHT_EYE        2
    #define AMBA_KP_LEFT_EAR         3
    #define AMBA_KP_RIGHT_EAR        4
    #define AMBA_KP_LEFT_SHOUDLER    5
    #define AMBA_KP_RIGHT_SHOUDLER   6
    #define AMBA_KP_LEFT_ELBOW       7
    #define AMBA_KP_RIGHT_ELBOW      8
    #define AMBA_KP_LEFT_WRIST       9
    #define AMBA_KP_RIGHT_WRIST      10
    #define AMBA_KP_LEFT_HIP         11
    #define AMBA_KP_RIGHT_HIP        12
    #define AMBA_KP_LEFT_KNEE        13
    #define AMBA_KP_RIGHT_KNEE       14
    #define AMBA_KP_LEFT_ANKLE       15
    #define AMBA_KP_RIGHT_ANKLE      16

    const OD_DRAW_INFO_s *pDrawInfo = g_ODDrawCtrl[CvFlowChan].pDrawInfo;
    const amba_kp_out_t *pKPOut;
    const amba_kp_candidate_t *pKPObj;
    ULONG ObjAddr;
    const UINT16 *pX, *pY;
    #define MAX_KP_OBJ_NUM           12U
    UINT32 MaxObjNum = 0U;
    UINT8 ObjIdx;

    if ((pPcptOut != NULL) && (pPcptData->pCvOut != NULL)) {
        AmbaMisra_TypeCast(&pKPOut, &pPcptData->pCvOut);
        AmbaMisra_TypeCast(&ObjAddr, &pKPOut);
        ObjAddr = ObjAddr + (ULONG)pKPOut->objects_offset;
        AmbaMisra_TypeCast(&pKPObj, &ObjAddr);

        /* Limit number of drawing objects */
        MaxObjNum = pKPOut->num_objects;
        if (pKPOut->num_objects > MAX_KP_OBJ_NUM) {
            MaxObjNum = MAX_KP_OBJ_NUM;
        }

        for (UINT32 i = 0; i < MaxObjNum; i++) {
            /* Get corresponding 2D object index */
            ObjIdx = pPcptData->pObjIdxList[i];
            if ((pDrawInfo->BbxDrawFlag[ObjIdx] & VALID_DRAW_OBJ) == 0U) {
                continue;
            }

            pX = &(pKPObj[i].coord[0]);
            pY = &(pKPObj[i].coord[32]);

            ODDrawTask_KpLine(CvFlowChan, pX[AMBA_KP_LEFT_ANKLE],       pY[AMBA_KP_LEFT_ANKLE],
                                          pX[AMBA_KP_LEFT_KNEE],        pY[AMBA_KP_LEFT_KNEE],
                                          0xFFBFFF00U);
            ODDrawTask_KpLine(CvFlowChan, pX[AMBA_KP_LEFT_KNEE],        pY[AMBA_KP_LEFT_KNEE],
                                          pX[AMBA_KP_LEFT_HIP],         pY[AMBA_KP_LEFT_HIP],
                                          0xFF00CC00U);
            ODDrawTask_KpLine(CvFlowChan, pX[AMBA_KP_RIGHT_ANKLE],      pY[AMBA_KP_RIGHT_ANKLE],
                                          pX[AMBA_KP_RIGHT_KNEE],       pY[AMBA_KP_RIGHT_KNEE],
                                          0xFF0040FFU);
            ODDrawTask_KpLine(CvFlowChan, pX[AMBA_KP_RIGHT_KNEE],       pY[AMBA_KP_RIGHT_KNEE],
                                          pX[AMBA_KP_RIGHT_HIP],        pY[AMBA_KP_RIGHT_HIP],
                                          0xFF00BFFFU);
            ODDrawTask_KpLine(CvFlowChan, pX[AMBA_KP_LEFT_HIP],         pY[AMBA_KP_LEFT_HIP],
                                          pX[AMBA_KP_RIGHT_HIP],        pY[AMBA_KP_RIGHT_HIP],
                                          0xFFFF80FFU);
            ODDrawTask_KpLine(CvFlowChan, pX[AMBA_KP_LEFT_SHOUDLER],    pY[AMBA_KP_LEFT_SHOUDLER],
                                          pX[AMBA_KP_LEFT_HIP],         pY[AMBA_KP_LEFT_HIP],
                                          0xFFFFBF00U);
            ODDrawTask_KpLine(CvFlowChan, pX[AMBA_KP_RIGHT_SHOUDLER],   pY[AMBA_KP_RIGHT_SHOUDLER],
                                          pX[AMBA_KP_RIGHT_HIP],        pY[AMBA_KP_RIGHT_HIP],
                                          0xFFFF6600U);
            ODDrawTask_KpLine(CvFlowChan, pX[AMBA_KP_LEFT_SHOUDLER],    pY[AMBA_KP_LEFT_SHOUDLER],
                                          pX[AMBA_KP_RIGHT_SHOUDLER],   pY[AMBA_KP_RIGHT_SHOUDLER],
                                          0xFF00CC00U);
            ODDrawTask_KpLine(CvFlowChan, pX[AMBA_KP_LEFT_SHOUDLER],    pY[AMBA_KP_LEFT_SHOUDLER],
                                          pX[AMBA_KP_LEFT_ELBOW],       pY[AMBA_KP_LEFT_ELBOW],
                                          0xFFFFFF00U);
            ODDrawTask_KpLine(CvFlowChan, pX[AMBA_KP_RIGHT_SHOUDLER],   pY[AMBA_KP_RIGHT_SHOUDLER],
                                          pX[AMBA_KP_RIGHT_ELBOW],      pY[AMBA_KP_RIGHT_ELBOW],
                                          0xFFFF0000U);
            ODDrawTask_KpLine(CvFlowChan, pX[AMBA_KP_LEFT_ELBOW],       pY[AMBA_KP_LEFT_ELBOW],
                                          pX[AMBA_KP_LEFT_WRIST],       pY[AMBA_KP_LEFT_WRIST],
                                          0xFFFF00FFU);
            ODDrawTask_KpLine(CvFlowChan, pX[AMBA_KP_RIGHT_ELBOW],      pY[AMBA_KP_RIGHT_ELBOW],
                                          pX[AMBA_KP_RIGHT_WRIST],      pY[AMBA_KP_RIGHT_WRIST],
                                          0xFF8000FFU);
#if 0
            //ODDrawTask_KpLine(CvFlowChan, pX[AMBA_KP_LEFT_EYE],         pY[AMBA_KP_LEFT_EYE],
            //                              pX[AMBA_KP_RIGHT_EYE],        pY[AMBA_KP_RIGHT_EYE],
            //                              0xFFBFFF00U);
            //ODDrawTask_KpLine(CvFlowChan, pX[AMBA_KP_NOSE],             pY[AMBA_KP_NOSE],
            //                              pX[AMBA_KP_LEFT_EYE],         pY[AMBA_KP_LEFT_EYE],
            //                              0xFF00CC00U);
            //ODDrawTask_KpLine(CvFlowChan, pX[AMBA_KP_NOSE],             pY[AMBA_KP_NOSE],
            //                              pX[AMBA_KP_RIGHT_EYE],        pY[AMBA_KP_RIGHT_EYE],
            //                              0xFF0040FFU);
            //ODDrawTask_KpLine(CvFlowChan, pX[AMBA_KP_LEFT_EYE],         pY[AMBA_KP_LEFT_EYE],
            //                              pX[AMBA_KP_LEFT_EAR],         pY[AMBA_KP_LEFT_EAR],
            //                              0xFF00BFFFU);
            //ODDrawTask_KpLine(CvFlowChan, pX[AMBA_KP_RIGHT_EYE],        pY[AMBA_KP_RIGHT_EYE],
            //                              pX[AMBA_KP_RIGHT_EAR],        pY[AMBA_KP_RIGHT_EAR],
            //                              0xFFFF80FFU);
#else    /* Simplify lines */
            ODDrawTask_KpLine(CvFlowChan, pX[AMBA_KP_LEFT_EAR],         pY[AMBA_KP_LEFT_EAR],
                                          pX[AMBA_KP_RIGHT_EAR],        pY[AMBA_KP_RIGHT_EAR],
                                          0xFFBF80BFU);
#endif
            ODDrawTask_KpLine(CvFlowChan, pX[AMBA_KP_LEFT_EAR],         pY[AMBA_KP_LEFT_EAR],
                                          pX[AMBA_KP_LEFT_SHOUDLER],    pY[AMBA_KP_LEFT_SHOUDLER],
                                          0xFFBF80BFU);
            ODDrawTask_KpLine(CvFlowChan, pX[AMBA_KP_RIGHT_EAR],        pY[AMBA_KP_RIGHT_EAR],
                                          pX[AMBA_KP_RIGHT_SHOUDLER],   pY[AMBA_KP_RIGHT_SHOUDLER],
                                          0xFF80BF80U);
        }
    } else {
        SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_DetResDrawKp: Invalid data", 0U, 0U);
    }

    AmbaMisra_TouchUnused(&pPcptOut);
}

static void ODDrawTask_DetResDrawMask(UINT32 CvFlowChan, const SVC_CV_PERCEPTION_OUTPUT_s *pPcptOut, UINT32 NumData, SVC_CV_PERCEPTION_DATA_s const ** pPcptData)
{
    const OD_DRAW_INFO_s *pDrawInfo = g_ODDrawCtrl[CvFlowChan].pDrawInfo;
    const amba_is_out_t *pMKOut;
    const ULONG *pAddr;
    const UINT16 *pW, *pH;
    ULONG TmpAddr, Addr;
    UINT8 ObjIdx, ListIdx = 0;
    SVC_CV_DISP_OSD_BITMAP_s BitmapCmd;
    SVC_CV_CAMCTRL_MSG_s CamMsg;
    const RECT_POS_s *pBbxOsdPos;

    BitmapCmd.Channel = pDrawInfo->OsdReg.Index;
    CamMsg.Msg = SVC_CV_OSD_BITMAP;
    CamMsg.pCtx = &BitmapCmd;

    /* Large mask and small mask */
    for (UINT32 DataIdx  = 0; DataIdx < NumData; DataIdx++) {
        if ((pPcptOut != NULL) && (pPcptData[DataIdx]->pCvOut != NULL)) {
            AmbaMisra_TypeCast(&pMKOut, &pPcptData[DataIdx]->pCvOut);
            AmbaMisra_TypeCast(&Addr, &pMKOut);

            TmpAddr = Addr + (ULONG)pMKOut->list_offset_width;
            AmbaMisra_TypeCast(&pW, &TmpAddr);
            TmpAddr = Addr + (ULONG)pMKOut->list_offset_height;
            AmbaMisra_TypeCast(&pH, &TmpAddr);
            TmpAddr = Addr + (ULONG)pMKOut->list_offset_address;
            AmbaMisra_TypeCast(&pAddr, &TmpAddr);

            for (UINT32 i = 0; i < pMKOut->num_objects; i++) {
                /* Get corresponding 2D object index */
                ObjIdx = pPcptData[DataIdx]->pObjIdxList[ListIdx];
                ListIdx++;
                if ((pDrawInfo->BbxDrawFlag[ObjIdx] & VALID_DRAW_OBJ) == 0U) {
                    continue;
                }
                pBbxOsdPos = &pDrawInfo->BbxOsdPos[ObjIdx];

                BitmapCmd.X = pBbxOsdPos->X1;
                AmbaMisra_TouchUnused(&BitmapCmd.X);

                BitmapCmd.Y = pBbxOsdPos->Y1;
                AmbaMisra_TouchUnused(&BitmapCmd.Y);

                BitmapCmd.W = pW[i];
                AmbaMisra_TouchUnused(&BitmapCmd.W);

                BitmapCmd.H = pH[i];
                AmbaMisra_TouchUnused(&BitmapCmd.H);

                BitmapCmd.SrcW = pW[i];
                AmbaMisra_TouchUnused(&BitmapCmd.SrcW);

                BitmapCmd.SrcH = pH[i];
                AmbaMisra_TouchUnused(&BitmapCmd.SrcH);

                BitmapCmd.SrcPitch = GetAlignedValU32(pW[i], 32U);
                AmbaMisra_TypeCast(&BitmapCmd.pAddr, &pAddr[i]);

                ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);
#if 0
                if (ODPrintDetRes == 1U) {
                    AmbaPrint_PrintUInt5("DrawMask[%u] ObjIdx %u Cat %u ListIdx %u",
                            i, ObjIdx, pPcptOut->pBbxList->Bbx[ObjIdx].Cat, ListIdx, 0U);
                    AmbaPrint_PrintUInt5("X %u Y %u W %u H %u Addr 0x%x",
                            BitmapCmd.X, BitmapCmd.Y, BitmapCmd.W, BitmapCmd.H, pAddr[i]);
                }
#endif
                if (ListIdx >= SVC_CV_MAX_PCPT_OBJ_INDEX) {
                    break;
                }
            }
        } else {
            SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_DetResDrawMask: Invalid data[%u]", DataIdx, 0U);
        }
    }

    AmbaMisra_TouchUnused(pPcptData);
}

static void ODDrawTask_DetResShowClfStr(UINT32 CvFlowChan, const SVC_CV_PERCEPTION_OUTPUT_s *pPcptOut, const SVC_CV_PERCEPTION_DATA_s *pPcptData)
{
    OD_DRAW_INFO_s *pDrawInfo = g_ODDrawCtrl[CvFlowChan].pDrawInfo;
    UINT8 ObjIdx;
    SVC_CV_DISP_OSD_STR_s   StrOsdCmd;
    SVC_CV_CAMCTRL_MSG_s    CamMsg;
    UINT32                  OsdWidth, OsdHeight;
    const UINT8             *pClfCat;
    const RECT_POS_s        *pBbxOsdPos;

    AmbaMisra_TypeCast(&pClfCat, &pPcptData->pCvOut);

    if (SVC_OK != SvcOsd_GetOsdBufSize(pDrawInfo->OsdReg.Index, &OsdWidth, &OsdHeight)) {
        SvcLog_NG(SVC_LOG_OD_TASK, "SvcOsd_GetOsdBufSize failed", 0U, 0U);
    }

    for (UINT32 ListIdx = 0; ListIdx < pPcptData->NumObj; ListIdx++) {
        /* Get corresponding 2D object index */
        ObjIdx = pPcptData->pObjIdxList[ListIdx];
        if ((pDrawInfo->BbxDrawFlag[ObjIdx] & VALID_DRAW_OBJ) == 0U) {
            continue;
        }
        pBbxOsdPos = &pDrawInfo->BbxOsdPos[ObjIdx];

        StrOsdCmd.Channel = pDrawInfo->OsdReg.Index;

        /* swap the position of x1 and x2 for flip case */
        if (pDrawInfo->OsdRegFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP){
            StrOsdCmd.X1 = pBbxOsdPos->X2;
        } else {
            StrOsdCmd.X1 = pBbxOsdPos->X1;
        }

        /* avoid string over OSD boundary */
        if ((OsdWidth - StrOsdCmd.X1) > ODMaxStrLen) {
            StrOsdCmd.X1 = ((OsdWidth - StrOsdCmd.X1) > ODMaxStrLen) ? StrOsdCmd.X1 : (OsdWidth - ODMaxStrLen);
            StrOsdCmd.Y1 = (pBbxOsdPos->Y1 < 20U) ? (pBbxOsdPos->Y1 + 10U) : (pBbxOsdPos->Y1 - 10U);
            StrOsdCmd.Color = ODDrawTask_OsdColorMap(ODColorMap, (UINT32) pPcptOut->pBbxList->Bbx[ObjIdx].Cat);

            if (pPcptData->PcptType == AMBANET_OD37_TLS) {
                /* Show OD Category-TLS Category */
                SvcWrap_strcpy(StrOsdCmd.String,
                               sizeof(StrOsdCmd.String),
                               pPcptOut->pBbxList->class_name[pPcptOut->pBbxList->Bbx[ObjIdx].Cat]);
                AmbaUtility_StringAppend(StrOsdCmd.String, sizeof(StrOsdCmd.String), "-");
                AmbaUtility_StringAppend(StrOsdCmd.String, sizeof(StrOsdCmd.String), pPcptData->class_name[pClfCat[ListIdx]]);
            } else {
                /* Classifier Cat */
                SvcWrap_strcpy(StrOsdCmd.String, sizeof(StrOsdCmd.String), pPcptData->class_name[pClfCat[ListIdx]]);
            }

            CamMsg.Msg = SVC_CV_OSD_DRAWSTRING;
            CamMsg.pCtx = &StrOsdCmd;
            ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);
        }

        pDrawInfo->BbxDrawFlag[ObjIdx] |= DRAWN_CLF_CATEGORY;
    }
}

/* Show classifier string: traffic sign and speed limit */
static void ODDrawTask_DetResShowClfStr_TS(UINT32 CvFlowChan, const SVC_CV_PERCEPTION_OUTPUT_s *pPcptOut, UINT32 NumData, SVC_CV_PERCEPTION_DATA_s const ** pPcptData)
{
    #define TS_DATA     (0U)    /* Traffic sign */
    #define SL_DATA     (1U)    /* Speed limit */
    OD_DRAW_INFO_s          *pDrawInfo = g_ODDrawCtrl[CvFlowChan].pDrawInfo;
    UINT8                   ObjIdx;
    SVC_CV_DISP_OSD_STR_s   StrOsdCmd;
    SVC_CV_CAMCTRL_MSG_s    CamMsg;
    UINT32                  OsdWidth, OsdHeight;
    const UINT8             *pTsCat;
    const UINT8             *pSlCat;
    const RECT_POS_s        *pBbxOsdPos;

    AmbaMisra_TypeCast(&pTsCat, &pPcptData[TS_DATA]->pCvOut);
    AmbaMisra_TypeCast(&pSlCat, &pPcptData[SL_DATA]->pCvOut);

    if (SVC_OK != SvcOsd_GetOsdBufSize(pDrawInfo->OsdReg.Index, &OsdWidth, &OsdHeight)) {
        SvcLog_NG(SVC_LOG_OD_TASK, "SvcOsd_GetOsdBufSize failed", 0U, 0U);
    }

    for (UINT32 ListIdx = 0; ListIdx < pPcptData[TS_DATA]->NumObj; ListIdx++) {
        /* Get corresponding 2D object index */
        ObjIdx = pPcptData[TS_DATA]->pObjIdxList[ListIdx];
        if ((pDrawInfo->BbxDrawFlag[ObjIdx] & VALID_DRAW_OBJ) == 0U) {
            continue;
        }
        pBbxOsdPos = &pDrawInfo->BbxOsdPos[ObjIdx];

        StrOsdCmd.Channel = pDrawInfo->OsdReg.Index;

        /* swap the position of x1 and x2 for flip case */
        if (pDrawInfo->OsdRegFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP){
            StrOsdCmd.X1 = pBbxOsdPos->X2;
        } else {
            StrOsdCmd.X1 = pBbxOsdPos->X1;
        }

        /* avoid string over OSD boundary */
        if ((OsdWidth - StrOsdCmd.X1) > ODMaxStrLen) {
            StrOsdCmd.X1 = ((OsdWidth - StrOsdCmd.X1) > ODMaxStrLen) ? StrOsdCmd.X1 : (OsdWidth - ODMaxStrLen);
            StrOsdCmd.Y1 = (pBbxOsdPos->Y1 < 20U) ? (pBbxOsdPos->Y1 + 10U) : (pBbxOsdPos->Y1 - 10U);
            StrOsdCmd.Color = ODDrawTask_OsdColorMap(ODColorMap, (UINT32) pPcptOut->pBbxList->Bbx[ObjIdx].Cat);

            if (pTsCat[ListIdx] == 0U /* AMBANET_TSC3_CAT0_SPEEDLIMIT*/) {
                /* Show speed limit */
                SvcWrap_strcpy(StrOsdCmd.String, sizeof(StrOsdCmd.String), pPcptData[SL_DATA]->class_name[pSlCat[ListIdx]]);
            } else {
                SvcWrap_strcpy(StrOsdCmd.String, sizeof(StrOsdCmd.String), pPcptData[TS_DATA]->class_name[pTsCat[ListIdx]]);
            }

            CamMsg.Msg = SVC_CV_OSD_DRAWSTRING;
            CamMsg.pCtx = &StrOsdCmd;
            ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);
        }

        pDrawInfo->BbxDrawFlag[ObjIdx] |= DRAWN_CLF_CATEGORY;
    }

    AmbaMisra_TouchUnused(&NumData);
    AmbaMisra_TouchUnused(pPcptData);
}

#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
static void ODDrawTask_FcDetRes3DBbxOsdProc(UINT32 CvFlowChan, const SVC_FC_DRAW_INFO_V2_s *pList)
{
    SVC_CV_DISP_OSD_CTRL_s OsdCmd;
    UINT32 OsdUpdate = 0;
    const OD_DRAW_INFO_s *pDrawInfo = g_ODDrawCtrl[CvFlowChan].pDrawInfo;

    if (pDrawInfo != NULL) {
        OsdCmd.Channel = pDrawInfo->OsdReg.Index;
        if (ODDrawRoi == 1U) {
            ODDrawTask_DetResDrawRoi(CvFlowChan);
            OsdUpdate = 1;
        }
        if ((pList->BbxOut.BbxAmount > 0U) ||
            (pList->Info3Dbbx.TotalNum > 0U) ||
            (pList->BbxOut.BbxAmount != g_LastNumBbx[CvFlowChan])) {
            ODDrawTask_FcDetResDraw3DBbx(CvFlowChan, pList);
            OsdUpdate = 1;
        }
        g_LastNumBbx[CvFlowChan] = pList->BbxOut.BbxAmount + pList->Info3Dbbx.TotalNum;

        if (OsdUpdate > 0U) {
            ODDrawTask_OsdFlush(OsdCmd.Channel, CvFlowChan);
        }
    }
}
static void ODDrawTask_FcDetResBbxOsdProc(UINT32 CvFlowChan, const SVC_FC_DRAW_INFO_s *pList)
{
    SVC_CV_DISP_OSD_CTRL_s OsdCmd;
    UINT32 OsdUpdate = 0;
    const OD_DRAW_INFO_s *pDrawInfo = g_ODDrawCtrl[CvFlowChan].pDrawInfo;

    if (pDrawInfo != NULL) {
        OsdCmd.Channel = pDrawInfo->OsdReg.Index;
        if (ODDrawRoi == 1U) {
            ODDrawTask_DetResDrawRoi(CvFlowChan);
            OsdUpdate = 1;
        }
        if ((pList->BbxOut.BbxAmount > 0U) || (pList->BbxOut.BbxAmount != g_LastNumBbx[CvFlowChan])) {
            ODDrawTask_FcDetResDrawBbx(CvFlowChan, pList);
            OsdUpdate = 1;
        }
        g_LastNumBbx[CvFlowChan] = pList->BbxOut.BbxAmount;

        if (OsdUpdate > 0U) {
            ODDrawTask_OsdFlush(OsdCmd.Channel, CvFlowChan);
        }
    }
}
#endif

#if defined(CONFIG_ICAM_PROJECT_EMIRROR) && defined(CONFIG_BUILD_AMBA_ADAS)
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ODDrawTask_DetResBbxOsdProc
 *
 *  @Description:: Pass BBX to OSD
 *
 *  @Input      ::
 *                 CvFlowChan: CvFlow channel
 *                 pList: The list contains the bounding box information
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 None
\*-----------------------------------------------------------------------------------------------*/
static void ODDrawTask_BsdDetResBbxOsdProc(UINT32 CvFlowChan, const SVC_CV_DETRES_BBX_LIST_s *pList, AMBA_SR_SROBJECT_DATA_s *pSRData)
{
    SVC_CV_DISP_OSD_CTRL_s OsdCmd;
    UINT32 OsdUpdate = 0;
    const OD_DRAW_INFO_s *pDrawInfo = g_ODDrawCtrl[CvFlowChan].pDrawInfo;

    if (pDrawInfo != NULL) {
        OsdCmd.Channel = pDrawInfo->OsdReg.Index;
        if (g_LastNumBbx[CvFlowChan] > 0U) {
            #if 0//SVC_APP_CV_DISP_ROI
            ODDrawTask_DetResDrawRoi(CvFlowChan);
            #endif
            OsdUpdate = 1;
        }

        if (pList->BbxAmount > 0U) {
            ODDrawTask_BsdDetResDrawBbx(CvFlowChan, pList, pSRData);
            OsdUpdate = 1;
        }

        g_LastNumBbx[CvFlowChan] = pList->BbxAmount;

        if (OsdUpdate > 0U) {
            ODDrawTask_OsdFlush(OsdCmd.Channel, CvFlowChan);
        }
    }
}

static void ODDrawTask_RmgDetResBbxOsdProc(UINT32 CvFlowChan, SVC_CV_DETRES_BBX_LIST_s *pList, AMBA_SR_SROBJECT_DATA_s *pSRData)
{
    SVC_CV_DISP_OSD_CTRL_s OsdCmd;
    UINT32 OsdUpdate = 0;
    const OD_DRAW_INFO_s *pDrawInfo = g_ODDrawCtrl[CvFlowChan].pDrawInfo;

    if (pDrawInfo != NULL) {
        OsdCmd.Channel = pDrawInfo->OsdReg.Index;
        if (ODDrawRoi == 1U) {
            ODDrawTask_DetResDrawRoi(CvFlowChan);
            OsdUpdate = 1;
        }

        if ((pList->BbxAmount > 0U) || (pList->BbxAmount != g_LastNumBbx[CvFlowChan])) {
            ODDrawTask_RmgDetResDrawBbx(CvFlowChan, pList, pSRData);
            OsdUpdate = 1;
        }

        g_LastNumBbx[CvFlowChan] = pList->BbxAmount;

        if (OsdUpdate > 0U) {
            ODDrawTask_OsdFlush(OsdCmd.Channel, CvFlowChan);
        }
    }
}
#endif

#if defined(CONFIG_ICAM_IMGCAL_STITCH_USED)
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ODDrawTask_DetResBbxOsdProc
 *
 *  @Description:: Pass BBX to OSD
 *
 *  @Input      ::
 *                 CvFlowChan: CvFlow channel
 *                 pList: The list contains the bounding box information
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 None
\*-----------------------------------------------------------------------------------------------*/
static void ODDrawTask_EmaDetResBbxOsdProc(UINT32 CvFlowChan, const SVC_CV_DETRES_BBX_LIST_s *pList, AMBA_SR_SROBJECT_DATA_s *pSRData)
{
    SVC_CV_DISP_OSD_CTRL_s OsdCmd;
    UINT32 OsdUpdate = 0;
    const OD_DRAW_INFO_s *pDrawInfo = g_ODDrawCtrl[CvFlowChan].pDrawInfo;

    if (pDrawInfo != NULL) {
        OsdCmd.Channel = pDrawInfo->OsdReg.Index;
        if (g_LastNumBbx[CvFlowChan] > 0U) {
            #if 0//SVC_APP_CV_DISP_ROI
            ODDrawTask_DetResDrawRoi(CvFlowChan);
            #endif
            OsdUpdate = 1;
        }

        if (pList->BbxAmount > 0U) {
            ODDrawTask_EmaDetResDrawBbx(CvFlowChan, pList, pSRData);
            OsdUpdate = 1;
        }

        g_LastNumBbx[CvFlowChan] = pList->BbxAmount;

        if (OsdUpdate > 0U) {
            ODDrawTask_OsdFlush(OsdCmd.Channel, CvFlowChan);
        }
    }
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ODDrawTask_DetResDrawRoi
 *
 *  @Description:: Draw ROI
 *
 *  @Input      ::
 *                 CvFlowChan: CvFlow channel
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 None
\*-----------------------------------------------------------------------------------------------*/
static void ODDrawTask_DetResDrawRoi(UINT32 CvFlowChan)
{
    UINT32 i;
    SVC_CV_DISP_OSD_CMD_s   OsdCmd;
    SVC_CV_CAMCTRL_MSG_s    CamMsg;
    RECT_POS_s              RoiPos;
    const OD_DRAW_INFO_s    *pDrawInfo = g_ODDrawCtrl[CvFlowChan].pDrawInfo;

    if (pDrawInfo != NULL) {
        for (i = 0; i < pDrawInfo->Roi_Revt.NumRoi; i++) {
            /* Rebase ROI from main window to VOUT src window */
            RoiPos.X1 = pDrawInfo->Roi_Revt.Roi[i].StartX;
            RoiPos.Y1 = pDrawInfo->Roi_Revt.Roi[i].StartY;
            RoiPos.X2 = pDrawInfo->Roi_Revt.Roi[i].StartX + pDrawInfo->Roi_Revt.Roi[i].Width - 1U;
            RoiPos.Y2 = pDrawInfo->Roi_Revt.Roi[i].StartY + pDrawInfo->Roi_Revt.Roi[i].Height - 1U;
            if (pDrawInfo->PrevCrop != 0U) {
                if (0U == ODDrawTask_DetResRebaseObj(&RoiPos, &pDrawInfo->PrevSrc)) {
                    continue;
                }
            }

            OsdCmd.Channel = pDrawInfo->OsdReg.Index;
            OsdCmd.X1 = (RoiPos.X1 * pDrawInfo->OsdReg.W) / pDrawInfo->PrevSrc.W;
            OsdCmd.Y1 = (RoiPos.Y1 * pDrawInfo->OsdReg.H) / pDrawInfo->PrevSrc.H;
            OsdCmd.X2 = (RoiPos.X2 * pDrawInfo->OsdReg.W) / pDrawInfo->PrevSrc.W;
            OsdCmd.Y2 = (RoiPos.Y2 * pDrawInfo->OsdReg.H) / pDrawInfo->PrevSrc.H;

            if (pDrawInfo->OsdRegFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP) {
                OsdCmd.X1 = pDrawInfo->OsdReg.W - OsdCmd.X1;
                OsdCmd.X2 = pDrawInfo->OsdReg.W - OsdCmd.X2;
            }

            /* add offset for the region */
            OsdCmd.X1 += pDrawInfo->OsdReg.X;
            OsdCmd.Y1 += pDrawInfo->OsdReg.Y;
            OsdCmd.X2 += pDrawInfo->OsdReg.X;
            OsdCmd.Y2 += pDrawInfo->OsdReg.Y;
            OsdCmd.Thickness = 2;
            OsdCmd.Color = 0xff;
            OsdCmd.WarningLevel = 0U;
            CamMsg.Msg = SVC_CV_OSD_DRAW_RECT;
            CamMsg.pCtx = &OsdCmd;
            ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);
        }
    }
}

static void ODDrawTask_DetResBbxOsdPosConvRebel(UINT32 CvFlowChan, const SVC_CV_DETRES_BBX_LIST_REBEL_s *pListRebel)
{
    OD_DRAW_INFO_s          *pDrawInfo = g_ODDrawCtrl[CvFlowChan].pDrawInfo;
    UINT32                  i;
    RECT_POS_s              BbxPos;
    UINT32                  X1, X2, Y1, Y2;
    UINT32                  OsdRegW, OsdRegH;
    RECT_POS_s              *pBbxOsdPos;

    if (pDrawInfo != NULL) {
        OsdRegW = pDrawInfo->OsdReg.W;
        OsdRegH = pDrawInfo->OsdReg.H;

        for (i = 0; i < pListRebel->BbxAmount; i++) {
            /* Rebase BBX from main window to VOUT src window */
            BbxPos.X1 = pListRebel->Bbx[i].X;
            BbxPos.Y1 = pListRebel->Bbx[i].Y;
            BbxPos.X2 = (UINT32)pListRebel->Bbx[i].X + (UINT32)pListRebel->Bbx[i].W - 1U;
            BbxPos.Y2 = (UINT32)pListRebel->Bbx[i].Y + (UINT32)pListRebel->Bbx[i].H - 1U;
            if (pDrawInfo->PrevCrop != 0U) {
                if (0U == ODDrawTask_DetResRebaseObj(&BbxPos, &pDrawInfo->PrevSrc)) {
                    pDrawInfo->BbxDrawFlag[i] = 0U;
                    continue;
                }
            }

            X1 = (BbxPos.X1 * OsdRegW) / pDrawInfo->PrevSrc.W;
            Y1 = (BbxPos.Y1 * OsdRegH) / pDrawInfo->PrevSrc.H;
            X2 = (BbxPos.X2 * OsdRegW) / pDrawInfo->PrevSrc.W;
            Y2 = (BbxPos.Y2 * OsdRegH) / pDrawInfo->PrevSrc.H;

            if (pDrawInfo->OsdRegFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP) {
                X1 = OsdRegW - X1;
                X2 = OsdRegW - X2;
            }

            /* avoid draw over canvas boundary */
            if ((X1 + ODRectThickness) >= OsdRegW) {
                X1 = OsdRegW - ODRectThickness;
            }
            if ((X2 + ODRectThickness) >= OsdRegW) {
                X2 = OsdRegW - ODRectThickness;
            }

            if ((Y1 + ODRectThickness) >= OsdRegH) {
                Y1 = OsdRegH - ODRectThickness;
            }
            if ((Y2 + ODRectThickness) >= OsdRegH) {
                Y2 = OsdRegH - ODRectThickness;
            }

            /* add offset for the region */
            pBbxOsdPos = &pDrawInfo->BbxOsdPos[i];
            pBbxOsdPos->X1 = X1 + pDrawInfo->OsdReg.X;
            pBbxOsdPos->Y1 = Y1 + pDrawInfo->OsdReg.Y;
            pBbxOsdPos->X2 = X2 + pDrawInfo->OsdReg.X;
            pBbxOsdPos->Y2 = Y2 + pDrawInfo->OsdReg.Y;

            pDrawInfo->BbxDrawFlag[i] |= VALID_DRAW_OBJ;
        }
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ODDrawTask_DetResBbxOsdPosConv
 *
 *  @Description:: Convert BBX to OSD coordinate
 *
 *  @Input      ::
 *                 CvFlowChan: CvFlow channel
 *                 pList: The list contains the bounding box information
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 None
\*-----------------------------------------------------------------------------------------------*/
static void ODDrawTask_DetResBbxOsdPosConv(UINT32 CvFlowChan, const SVC_CV_DETRES_BBX_LIST_s *pList)
{
    OD_DRAW_INFO_s          *pDrawInfo = g_ODDrawCtrl[CvFlowChan].pDrawInfo;
    UINT32                  i;
    RECT_POS_s              BbxPos;
    UINT32                  X1, X2, Y1, Y2;
    UINT32                  OsdRegW, OsdRegH;
    RECT_POS_s              *pBbxOsdPos;
    UINT32                  Cat;

    if (pDrawInfo != NULL) {
        OsdRegW = pDrawInfo->OsdReg.W;
        OsdRegH = pDrawInfo->OsdReg.H;

        for (i = 0; i < pList->BbxAmount; i++) {
            /* If score = 0 */
            if (pList->Bbx[i].Score == 0U) {
                pDrawInfo->BbxDrawFlag[i] = 0U;
                continue;
            }

            /* If the category is not allowed to be drawn */
            Cat = pList->Bbx[i].Cat;
            if (Cat < SVC_APP_CV_OD_DRAW_CAT_MAX) {
                if ((ODCatAllowBits & ((UINT32)1U << Cat)) == 0U) {
                    continue;
                }
            }

            /* Rebase BBX from main window to VOUT src window */
            BbxPos.X1 = pList->Bbx[i].X;
            BbxPos.Y1 = pList->Bbx[i].Y;
            BbxPos.X2 = (UINT32)pList->Bbx[i].X + (UINT32)pList->Bbx[i].W - 1U;
            BbxPos.Y2 = (UINT32)pList->Bbx[i].Y + (UINT32)pList->Bbx[i].H - 1U;
            if (pDrawInfo->PrevCrop != 0U) {
                if (0U == ODDrawTask_DetResRebaseObj(&BbxPos, &pDrawInfo->PrevSrc)) {
                    pDrawInfo->BbxDrawFlag[i] = 0U;
                    continue;
                }
            }

            X1 = (BbxPos.X1 * OsdRegW) / pDrawInfo->PrevSrc.W;
            Y1 = (BbxPos.Y1 * OsdRegH) / pDrawInfo->PrevSrc.H;
            X2 = (BbxPos.X2 * OsdRegW) / pDrawInfo->PrevSrc.W;
            Y2 = (BbxPos.Y2 * OsdRegH) / pDrawInfo->PrevSrc.H;

            if (pDrawInfo->OsdRegFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP) {
                X1 = OsdRegW - X1;
                X2 = OsdRegW - X2;
            }

            /* avoid draw over canvas boundary */
            if ((X1 + ODRectThickness) >= OsdRegW) {
                X1 = OsdRegW - ODRectThickness;
            }
            if ((X2 + ODRectThickness) >= OsdRegW) {
                X2 = OsdRegW - ODRectThickness;
            }

            if ((Y1 + ODRectThickness) >= OsdRegH) {
                Y1 = OsdRegH - ODRectThickness;
            }
            if ((Y2 + ODRectThickness) >= OsdRegH) {
                Y2 = OsdRegH - ODRectThickness;
            }

            /* add offset for the region */
            pBbxOsdPos = &pDrawInfo->BbxOsdPos[i];
            pBbxOsdPos->X1 = X1 + pDrawInfo->OsdReg.X;
            pBbxOsdPos->Y1 = Y1 + pDrawInfo->OsdReg.Y;
            pBbxOsdPos->X2 = X2 + pDrawInfo->OsdReg.X;
            pBbxOsdPos->Y2 = Y2 + pDrawInfo->OsdReg.Y;

            pDrawInfo->BbxDrawFlag[i] |= VALID_DRAW_OBJ;
        }
    }
}

#define DEPTH_INFO_COLOR (0xFFFFFFFFU)
#define DEPTH_INFO_DRAW_CAT_LIMIT (3U)

#define Rebel_BBX_MSG_W (10U)   //Show OD message if Width  > 10
#define Rebel_BBX_MSG_H (40U)   //Show OD message if Height > 40
static void ODDrawTask_DetResDrawBbxRebel(UINT32 CvFlowChan, const SVC_CV_DETRES_BBX_LIST_REBEL_s *pListRebel)
{
    UINT32 i;
    SVC_CV_DISP_OSD_CMD_s   RectOsdCmd;
    SVC_CV_DISP_OSD_STR_s   StrOsdCmd, RebelBbxInfo;
    SVC_CV_CAMCTRL_MSG_s    CamMsg;
    SVC_CV_DISP_OSD_LINE_s  LineCmd;
    OD_DRAW_INFO_s          *pDrawInfo = g_ODDrawCtrl[CvFlowChan].pDrawInfo;
    const RECT_POS_s        *pBbxOsdPos;
    UINT32                  OsdWidth, OsdHeight;

    if (pDrawInfo != NULL) {
        if (SVC_OK != SvcOsd_GetOsdBufSize(pDrawInfo->OsdReg.Index, &OsdWidth, &OsdHeight)) {
            SvcLog_NG(SVC_LOG_OD_TASK, "SvcOsd_GetOsdBufSize failed", 0U, 0U);
        }

        for (i = 0; i < pListRebel->BbxAmount; i++) {
            if ((pDrawInfo->BbxDrawFlag[i] & VALID_DRAW_OBJ) == 0U) {
                continue;
            }
            pBbxOsdPos = &pDrawInfo->BbxOsdPos[i];

            /* put bbx msg */
            RectOsdCmd.Channel = pDrawInfo->OsdReg.Index;
            RectOsdCmd.X1 = pBbxOsdPos->X1;
            RectOsdCmd.Y1 = pBbxOsdPos->Y1;
            RectOsdCmd.X2 = pBbxOsdPos->X2;
            RectOsdCmd.Y2 = pBbxOsdPos->Y2;
            RectOsdCmd.Color = ODDrawTask_OsdColorMap(ODColorMap, (UINT32) pListRebel->Bbx[i].Cat);
            RectOsdCmd.Thickness = ODRectThickness;
            RectOsdCmd.WarningLevel = 0U;

            CamMsg.Msg = SVC_CV_OSD_DRAW_RECT;
            CamMsg.pCtx = &RectOsdCmd;
            ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);
            pDrawInfo->BbxDrawFlag[i] |= DRAWN_2D_BBX;


            //Draw car direction
            {
                UINT32 ST_LineWidth = 0U;
                UINT32 X_GAP, Y_GAP;

                SVC_POINT_2D_s NearestPoint;
                SVC_POINT_2D_s DepthPointX;
                SVC_POINT_2D_s DepthPointY;

                LineCmd.Channel = pDrawInfo->OsdReg.Index;
                AmbaSvcWrap_MisraMemcpy(&NearestPoint, &pListRebel->Bbx[i].NearestPoint, sizeof(SVC_POINT_2D_s));
                AmbaSvcWrap_MisraMemcpy(&DepthPointX,  &pListRebel->Bbx[i].DepthPointX,  sizeof(SVC_POINT_2D_s));
                AmbaSvcWrap_MisraMemcpy(&DepthPointY,  &pListRebel->Bbx[i].DepthPointY,  sizeof(SVC_POINT_2D_s));

                NearestPoint.x = (NearestPoint.x * pDrawInfo->OsdReg.W) / pDrawInfo->PrevSrc.W;
                NearestPoint.y = (NearestPoint.y * pDrawInfo->OsdReg.H) / pDrawInfo->PrevSrc.H;
                DepthPointX.x = (DepthPointX.x * pDrawInfo->OsdReg.W) / pDrawInfo->PrevSrc.W;
                DepthPointX.y = (DepthPointX.y * pDrawInfo->OsdReg.H) / pDrawInfo->PrevSrc.H;
                DepthPointY.x = (DepthPointY.x * pDrawInfo->OsdReg.W) / pDrawInfo->PrevSrc.W;
                DepthPointY.y = (DepthPointY.y * pDrawInfo->OsdReg.H) / pDrawInfo->PrevSrc.H;

                ST_LineWidth = (pListRebel->Bbx[i].W * pDrawInfo->OsdReg.W) / pDrawInfo->PrevSrc.W;
                LineCmd.LineWidth = (ST_LineWidth >= THICKNESS_RATIO) ? (ST_LineWidth/THICKNESS_RATIO):1U;
                /* avoid draw over canvas boundary */
                if ((NearestPoint.x + LineCmd.LineWidth) >= pDrawInfo->OsdReg.W) {
                    NearestPoint.x = pDrawInfo->OsdReg.W - LineCmd.LineWidth;
                }
                if ((NearestPoint.y + LineCmd.LineWidth) >= pDrawInfo->OsdReg.H) {
                    NearestPoint.y = pDrawInfo->OsdReg.H - LineCmd.LineWidth;
                }

                if ((DepthPointX.x + LineCmd.LineWidth) >= pDrawInfo->OsdReg.W) {
                    DepthPointX.x = pDrawInfo->OsdReg.W - LineCmd.LineWidth;
                }
                if ((DepthPointX.y + LineCmd.LineWidth) >= pDrawInfo->OsdReg.H) {
                    DepthPointX.y = pDrawInfo->OsdReg.H - LineCmd.LineWidth;
                }

                if ((DepthPointY.x + LineCmd.LineWidth) >= pDrawInfo->OsdReg.W) {
                    DepthPointY.x = pDrawInfo->OsdReg.W - LineCmd.LineWidth;
                }
                if ((DepthPointY.y + LineCmd.LineWidth) >= pDrawInfo->OsdReg.H) {
                    DepthPointY.y = pDrawInfo->OsdReg.H - LineCmd.LineWidth;
                }

                /* add offset for the region */
                NearestPoint.x += pDrawInfo->OsdReg.X;
                NearestPoint.y += pDrawInfo->OsdReg.Y;
                DepthPointX.x  += pDrawInfo->OsdReg.X;
                DepthPointX.y  += pDrawInfo->OsdReg.Y;
                DepthPointY.x  += pDrawInfo->OsdReg.X;
                DepthPointY.y  += pDrawInfo->OsdReg.Y;

                //NearestPoint x connect to NearestPoint x,
                //NearestPoint y connect to DepthPointX y
                LineCmd.X1 = NearestPoint.x;
                LineCmd.X2 = NearestPoint.x;
                LineCmd.Y1 = NearestPoint.y;
                LineCmd.Y2 = DepthPointX.y;
                LineCmd.Color = 0x8039EBEBU;

                if (LineCmd.Y1 > LineCmd.Y2) {
                    Y_GAP = (LineCmd.Y1 - LineCmd.Y2);
                } else {
                    Y_GAP = (LineCmd.Y2 - LineCmd.Y1);
                }

                if (Y_GAP > LineCmd.LineWidth) {
                    CamMsg.Msg = SVC_CV_OSD_DRAW_LINE;
                    CamMsg.pCtx = &LineCmd;
                    ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);
                }

                //DepthPointX connect DepthPointY
                LineCmd.X1 = DepthPointX.x;
                LineCmd.X2 = DepthPointY.x;
                LineCmd.Y1 = DepthPointX.y;
                LineCmd.Y2 = DepthPointY.y;
                LineCmd.Color = 0x8039EBEBU;

                if (LineCmd.X1 > LineCmd.X2) {
                    X_GAP = (LineCmd.X1 - LineCmd.X2);
                } else {
                    X_GAP = (LineCmd.X2 - LineCmd.X1);
                }
                if (LineCmd.Y1 > LineCmd.Y2) {
                    Y_GAP = (LineCmd.Y1 - LineCmd.Y2);
                } else {
                    Y_GAP = (LineCmd.Y2 - LineCmd.Y1);
                }

                if ((X_GAP > LineCmd.LineWidth)||(Y_GAP > LineCmd.LineWidth)) {
                    CamMsg.Msg = SVC_CV_OSD_DRAW_LINE;
                    CamMsg.pCtx = &LineCmd;
                    ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);
                }

            }

            if ((pDrawInfo->BbxDrawFlag[i] & DRAWN_CLF_CATEGORY) == 0U) {
                /* put category msg */
                if (ODLabelCat == 1U) {
                    if (pListRebel->Bbx[i].Cat < 255U) {
                        StrOsdCmd.Channel = pDrawInfo->OsdReg.Index;

                        /* swap the position of x1 and x2 for flip case */
                        if (pDrawInfo->OsdRegFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP){
                            StrOsdCmd.X1 = RectOsdCmd.X2;
                        } else {
                            StrOsdCmd.X1 = RectOsdCmd.X1;
                        }

                        /* avoid string over OSD boundary */
                        if (((OsdWidth - RectOsdCmd.X1) > 105U)&&
                            ((OsdHeight - RectOsdCmd.Y1) > 50U)) {
                            StrOsdCmd.Y1 = (RectOsdCmd.Y1 < 20U) ? (RectOsdCmd.Y1 + 10U) : (RectOsdCmd.Y1 - 10U);
                            StrOsdCmd.Color = RectOsdCmd.Color;

                            SvcWrap_strcpy(StrOsdCmd.String, sizeof(StrOsdCmd.String), pListRebel->class_name[pListRebel->Bbx[i].Cat]);
                            CamMsg.Msg = SVC_CV_OSD_DRAWSTRING;
                            CamMsg.pCtx = &StrOsdCmd;
                            ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);


                            if ((pListRebel->Bbx[i].Cat > 0U)&&
                                (pListRebel->Bbx[i].Cat <= DEPTH_INFO_DRAW_CAT_LIMIT)) {
                                if ((g_All_OD_Info == 1U) ||
                                    (((RectOsdCmd.X2 - RectOsdCmd.X1) >= Rebel_BBX_MSG_W)&&
                                    ((RectOsdCmd.Y2 - RectOsdCmd.Y1) >= Rebel_BBX_MSG_H))) {
                                    FLOAT PositionX = pListRebel->Bbx[i].PositionX;
                                    FLOAT PositionY = pListRebel->Bbx[i].PositionY;
                                    FLOAT VelocityX = pListRebel->Bbx[i].VelocityX;
                                    FLOAT VelocityY = pListRebel->Bbx[i].VelocityY;

                                    RebelBbxInfo.Channel = pDrawInfo->OsdReg.Index;
                                    RebelBbxInfo.X1 = (RectOsdCmd.X1+5U);
                                    RebelBbxInfo.Y1 = RectOsdCmd.Y1;

                                    if (1U == g_RebelOD_ID) {
                                        AmbaSvcWrap_MisraMemset(&RebelBbxInfo.String, 0, sizeof(24U));
                                        RebelBbxInfo.Y1 += 10U;
                                        RebelBbxInfo.Color = DEPTH_INFO_COLOR;
                                        RebelBbxInfo.String[0] = 'I';
                                        RebelBbxInfo.String[1] = 'D';
                                        RebelBbxInfo.String[2] = ':';
                                        RebelBbxInfo.String[3] = '\0';
                                        AmbaUtility_StringAppendUInt32(RebelBbxInfo.String, sizeof(RebelBbxInfo.String), (UINT32)pListRebel->Bbx[i].ObjectID, 10U);
                                        CamMsg.Msg = SVC_CV_OSD_DRAWSTRING;
                                        CamMsg.pCtx = &RebelBbxInfo;
                                        ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);
                                    }

                                    /*
                                     *  Range of each PositionX, PositionY, VelocityX and VelocityY
                                        d_x: Position X [0,1000]
                                        d_y: Position Y [-100,100]
                                        v_x: Velocity X [-1000,1000]
                                        v_y: Velocity Y [-1000,1000]

                                     * */
                                    if (1U == g_D_xy) {
                                        AmbaSvcWrap_MisraMemset(&RebelBbxInfo.String, 0, sizeof(24U));
                                        RebelBbxInfo.Y1 += 15U;
                                        RebelBbxInfo.Color = DEPTH_INFO_COLOR;
                                        RebelBbxInfo.String[0] = 'd';
                                        RebelBbxInfo.String[1] = '_';
                                        RebelBbxInfo.String[2] = 'x';
                                        RebelBbxInfo.String[3] = ':';
                                        if ((PositionX <= 1000.0) && (PositionX > 0.0)) {
                                            RebelBbxInfo.String[4] = '\0';
                                            AmbaUtility_StringAppendFloat(RebelBbxInfo.String, sizeof(RebelBbxInfo.String), PositionX, 2U);
                                        } else {
                                            RebelBbxInfo.String[4] = 'N';
                                            RebelBbxInfo.String[5] = '/';
                                            RebelBbxInfo.String[6] = 'A';
                                            RebelBbxInfo.String[7] = '\0';
                                        }
                                        CamMsg.Msg = SVC_CV_OSD_DRAWSTRING;
                                        CamMsg.pCtx = &RebelBbxInfo;
                                        ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);


                                        AmbaSvcWrap_MisraMemset(&RebelBbxInfo.String, 0, sizeof(24U));
                                        RebelBbxInfo.Y1 += 15U;
                                        RebelBbxInfo.Color = DEPTH_INFO_COLOR;
                                        RebelBbxInfo.String[0] = 'd';
                                        RebelBbxInfo.String[1] = '_';
                                        RebelBbxInfo.String[2] = 'y';
                                        RebelBbxInfo.String[3] = ':';
                                        if ((PositionY >= -100.0) && (PositionY <= 100.0)) {
                                            RebelBbxInfo.String[4] = '\0';
                                            AmbaUtility_StringAppendFloat(RebelBbxInfo.String, sizeof(RebelBbxInfo.String), PositionY, 2U);
                                        } else {
                                            RebelBbxInfo.String[4] = 'N';
                                            RebelBbxInfo.String[5] = '/';
                                            RebelBbxInfo.String[6] = 'A';
                                            RebelBbxInfo.String[7] = '\0';
                                        }
                                        CamMsg.Msg = SVC_CV_OSD_DRAWSTRING;
                                        CamMsg.pCtx = &RebelBbxInfo;
                                        ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);
                                    }

                                    if (1U == g_V_xy) {
                                        AmbaSvcWrap_MisraMemset(&RebelBbxInfo.String, 0, sizeof(24U));
                                        RebelBbxInfo.Y1 += 15U;
                                        RebelBbxInfo.Color = DEPTH_INFO_COLOR;
                                        RebelBbxInfo.String[0] = 'v';
                                        RebelBbxInfo.String[1] = '_';
                                        RebelBbxInfo.String[2] = 'x';
                                        RebelBbxInfo.String[3] = ':';
                                        if ((VelocityX >= -1000.0) && (VelocityX <= 1000.0)) {
                                            RebelBbxInfo.String[4] = '\0';
                                            AmbaUtility_StringAppendFloat(RebelBbxInfo.String, sizeof(RebelBbxInfo.String), VelocityX, 2U);
                                        } else {
                                            RebelBbxInfo.String[4] = 'N';
                                            RebelBbxInfo.String[5] = '/';
                                            RebelBbxInfo.String[6] = 'A';
                                            RebelBbxInfo.String[7] = '\0';
                                        }

                                        CamMsg.Msg = SVC_CV_OSD_DRAWSTRING;
                                        CamMsg.pCtx = &RebelBbxInfo;
                                        ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);


                                        AmbaSvcWrap_MisraMemset(&RebelBbxInfo.String, 0, sizeof(24U));
                                        RebelBbxInfo.Y1 += 15U;
                                        RebelBbxInfo.Color = DEPTH_INFO_COLOR;
                                        RebelBbxInfo.String[0] = 'v';
                                        RebelBbxInfo.String[1] = '_';
                                        RebelBbxInfo.String[2] = 'y';
                                        RebelBbxInfo.String[3] = ':';
                                        if ((VelocityY >= -1000.0) && (VelocityY <= 1000.0)) {
                                            RebelBbxInfo.String[4] = '\0';
                                            AmbaUtility_StringAppendFloat(RebelBbxInfo.String, sizeof(RebelBbxInfo.String), VelocityY, 2U);
                                        } else {
                                            RebelBbxInfo.String[4] = 'N';
                                            RebelBbxInfo.String[5] = '/';
                                            RebelBbxInfo.String[6] = 'A';
                                            RebelBbxInfo.String[7] = '\0';
                                        }
                                        CamMsg.Msg = SVC_CV_OSD_DRAWSTRING;
                                        CamMsg.pCtx = &RebelBbxInfo;
                                        ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);
                                    }

//                                    SVC_WRAP_PRINT "[After] ObjectID %5f PositionX %5f PositionY %5f VelocityX %5f VelocityY %5f"
//                                    SVC_PRN_ARG_S ""
//                                    SVC_PRN_ARG_PROC SvcLog_OK
//                                    SVC_PRN_ARG_DOUBLE  pListRebel->Bbx[i].ObjectID
//                                    SVC_PRN_ARG_DOUBLE  PositionX
//                                    SVC_PRN_ARG_DOUBLE  PositionY
//                                    SVC_PRN_ARG_DOUBLE  VelocityX
//                                    SVC_PRN_ARG_DOUBLE  VelocityY
//                                    SVC_PRN_ARG_E
                                }
                            }
                        }
                    }

                    pDrawInfo->BbxDrawFlag[i] |= DRAWN_2D_CATEGORY;
                }
            }
        }
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ODDrawTask_DetResDrawBbx
 *
 *  @Description:: Draw BBX
 *
 *  @Input      ::
 *                 CvFlowChan: CvFlow channel
 *                 pList: The list contains the bounding box information
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 None
\*-----------------------------------------------------------------------------------------------*/
static void ODDrawTask_DetResDrawBbx(UINT32 CvFlowChan, const SVC_CV_DETRES_BBX_LIST_s *pList)
{
    UINT32 i;
    SVC_CV_DISP_OSD_CMD_s   RectOsdCmd;
    SVC_CV_DISP_OSD_STR_s   StrOsdCmd;
    SVC_CV_CAMCTRL_MSG_s    CamMsg;
    OD_DRAW_INFO_s          *pDrawInfo = g_ODDrawCtrl[CvFlowChan].pDrawInfo;
    const RECT_POS_s        *pBbxOsdPos;
    UINT32                  OsdWidth, OsdHeight;
    char                    Osd_String[24];
    UINT32                  Arg[1];

    if (pDrawInfo != NULL) {
        if (SVC_OK != SvcOsd_GetOsdBufSize(pDrawInfo->OsdReg.Index, &OsdWidth, &OsdHeight)) {
            SvcLog_NG(SVC_LOG_OD_TASK, "SvcOsd_GetOsdBufSize failed", 0U, 0U);
        }

        for (i = 0; i < pList->BbxAmount; i++) {
            if ((pDrawInfo->BbxDrawFlag[i] & VALID_DRAW_OBJ) == 0U) {
                continue;
            }
            pBbxOsdPos = &pDrawInfo->BbxOsdPos[i];

            /* put bbx msg */
            RectOsdCmd.Channel = pDrawInfo->OsdReg.Index;
            RectOsdCmd.X1 = pBbxOsdPos->X1;
            RectOsdCmd.Y1 = pBbxOsdPos->Y1;
            RectOsdCmd.X2 = pBbxOsdPos->X2;
            RectOsdCmd.Y2 = pBbxOsdPos->Y2;
            RectOsdCmd.Color = ODDrawTask_OsdColorMap(ODColorMap, (UINT32) pList->Bbx[i].Cat);
            RectOsdCmd.Thickness = ODRectThickness;
            RectOsdCmd.WarningLevel = 0U;

            CamMsg.Msg = SVC_CV_OSD_DRAW_RECT;
            CamMsg.pCtx = &RectOsdCmd;
            ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);
            pDrawInfo->BbxDrawFlag[i] |= DRAWN_2D_BBX;

            /* put score msg */
            if (ODLabelScore > 0U) {
                StrOsdCmd.Channel = pDrawInfo->OsdReg.Index;

                /* swap the position of x1 and x2 for flip case */
                if (pDrawInfo->OsdRegFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP){
                    StrOsdCmd.X1 = RectOsdCmd.X2;
                } else {
                    StrOsdCmd.X1 = RectOsdCmd.X1;
                }

                /* avoid string over OSD boundary */
                StrOsdCmd.X1 = ((OsdWidth - StrOsdCmd.X1) > 50U) ? StrOsdCmd.X1 : (StrOsdCmd.X1 - 50U);
                StrOsdCmd.Y1 = (RectOsdCmd.Y1 < 20U) ? (RectOsdCmd.Y1 + 20U) : (RectOsdCmd.Y1 - 20U);
                StrOsdCmd.Color = RectOsdCmd.Color;

                if (ODLabelScore == 1U) {
                    Arg[0] = pList->Bbx[i].Score;
                } else {
                    Arg[0] = (pList->Bbx[i].Score * 100U) / ODLabelScore; /* percentage */
                }
                if (SVC_OK != AmbaUtility_StringPrintUInt32(Osd_String, sizeof(StrOsdCmd.String), "%d", 1U, Arg)) {
                    // SvcLog_NG(SVC_LOG_OD_TASK, "AmbaUtility_StringPrintUInt32 Osd_String failed", 0U, 0U);
                }
                SvcWrap_strcpy(StrOsdCmd.String, sizeof(StrOsdCmd.String), Osd_String);
                CamMsg.Msg = SVC_CV_OSD_DRAWSTRING;
                CamMsg.pCtx = &StrOsdCmd;
                ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);
            }

            if ((pDrawInfo->BbxDrawFlag[i] & DRAWN_CLF_CATEGORY) == 0U) {
                /* put category msg */
                if (ODLabelCat == 1U) {
                    if (pList->Bbx[i].Cat < 255U) {
                        StrOsdCmd.Channel = pDrawInfo->OsdReg.Index;

                        /* swap the position of x1 and x2 for flip case */
                        if (pDrawInfo->OsdRegFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP){
                            StrOsdCmd.X1 = RectOsdCmd.X2;
                        } else {
                            StrOsdCmd.X1 = RectOsdCmd.X1;
                        }

                        /* avoid string over OSD boundary */
                        if ((OsdWidth - RectOsdCmd.X1) > ODMaxStrLen) {
                            StrOsdCmd.X1 = ((OsdWidth - StrOsdCmd.X1) > ODMaxStrLen) ? StrOsdCmd.X1 : (OsdWidth - ODMaxStrLen);
                            StrOsdCmd.Y1 = (RectOsdCmd.Y1 < 20U) ? (RectOsdCmd.Y1 + 10U) : (RectOsdCmd.Y1 - 10U);
                            StrOsdCmd.Color = RectOsdCmd.Color;

                            SvcWrap_strcpy(StrOsdCmd.String, sizeof(StrOsdCmd.String), pList->class_name[pList->Bbx[i].Cat]);
                            CamMsg.Msg = SVC_CV_OSD_DRAWSTRING;
                            CamMsg.pCtx = &StrOsdCmd;
                            ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);
                        }
                    }

                    pDrawInfo->BbxDrawFlag[i] |= DRAWN_2D_CATEGORY;
                }
            }

            #if defined(CONFIG_ICAM_PROJECT_EMIRROR) && defined(CONFIG_BUILD_AMBA_ADAS)
            if (ODLabelPosition == 1U) {
                StrOsdCmd.Channel = pDrawInfo->OsdReg.Index;

                /* swap the position of x1 and x2 for flip case */
                if (pDrawInfo->OsdRegFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP){
                    StrOsdCmd.X1 = RectOsdCmd.X2;
                } else {
                    StrOsdCmd.X1 = RectOsdCmd.X1;
                }

                /* avoid string over OSD boundary */
                StrOsdCmd.Y1 = (RectOsdCmd.Y1 < 20U) ? (RectOsdCmd.Y1 + 10U) : (RectOsdCmd.Y1 - 20U);
                StrOsdCmd.Color = RectOsdCmd.Color;
                {
                    UINT32 Rval;
                    static AMBA_CAL_POINT_DB_2D_s BbxPoint;
                    const AMBA_CAL_EM_PLANE_EQUATION_s CrossPlane = {0.0, 0.0, 1.0, 0.0};
                    static AMBA_CAL_POINT_DB_3D_s WorldPoint;

                    BbxPoint.X = (DOUBLE)(pList->Bbx[i].X) + ((DOUBLE)(pList->Bbx[i].W)/2.0f);
                    BbxPoint.Y = (DOUBLE)(pList->Bbx[i].Y) + (DOUBLE)(pList->Bbx[i].H);

                    Rval = AmbaCal_EmConvPtImgToWorldPlane(&Cfg, &BbxPoint, &CrossPlane, &WorldPoint);
                    if(Rval != 0U) {
                        #if 0
                        extern void RefCalib_ShowCalibCoordOutput(const AMBA_CAL_EM_CALC_COORD_CFG_s *CalCoordOutput);

                        RefCalib_ShowCalibCoordOutput(&Cfg);

                        SVC_WRAP_PRINT "BbxPoint.X %3f BbxPoint.Y %3f CrossPlane %1.1f %1.1f %1.1f %1.1f"
                        SVC_PRN_ARG_S __func__
                        SVC_PRN_ARG_PROC SvcLog_NG
                        SVC_PRN_ARG_DOUBLE BbxPoint.X
                        SVC_PRN_ARG_DOUBLE BbxPoint.Y
                        SVC_PRN_ARG_DOUBLE CrossPlane.CoefA
                        SVC_PRN_ARG_DOUBLE CrossPlane.CoefB
                        SVC_PRN_ARG_DOUBLE CrossPlane.CoefC
                        SVC_PRN_ARG_DOUBLE CrossPlane.CoefD
                        SVC_PRN_ARG_E
                        #else
                        SvcLog_NG(__func__, "Line %d", __LINE__, 0U);
                        #endif
                    } else {
                        if (SVC_OK != AmbaUtility_DoubleToStr(Osd_String, sizeof(Osd_String), -(WorldPoint.Y/1000.0), 1U)) {
                            // SvcLog_NG(SVC_LOG_OD_TASK, "AmbaUtility_DoubleToStr Osd_String failed", 0U, 0U);
                        }
                        SvcWrap_strcpy(StrOsdCmd.String, sizeof(StrOsdCmd.String), Osd_String);
                        AmbaUtility_StringAppend(StrOsdCmd.String, sizeof(StrOsdCmd.String), "m");
                        CamMsg.Msg = SVC_CV_OSD_DRAWSTRING;
                        CamMsg.pCtx = &StrOsdCmd;
                        ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);
                        #if 0
                        SVC_WRAP_PRINT "x%u y%u w%u h%u .X%3f .Y%3f WorldPoint.Y%1.1f "
                        SVC_PRN_ARG_S __func__
                        SVC_PRN_ARG_PROC SvcLog_NG
                        SVC_PRN_ARG_INT16 pList->Bbx[i].X SVC_PRN_ARG_POST
                        SVC_PRN_ARG_INT16 pList->Bbx[i].Y SVC_PRN_ARG_POST
                        SVC_PRN_ARG_INT16 pList->Bbx[i].W SVC_PRN_ARG_POST
                        SVC_PRN_ARG_INT16 pList->Bbx[i].H SVC_PRN_ARG_POST
                        SVC_PRN_ARG_DOUBLE BbxPoint.X
                        SVC_PRN_ARG_DOUBLE BbxPoint.Y
                        SVC_PRN_ARG_DOUBLE WorldPoint.Y
                        SVC_PRN_ARG_E
                        #endif
                    }
                }
            }
            #endif
        }
    }
}

#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
static void ODDrawTask_FcDetResDraw3DBbx(UINT32 CvFlowChan, const SVC_FC_DRAW_INFO_V2_s *pList)
{
    UINT32 i;
    SVC_CV_DISP_3D_CMD_s    Osd3DCmd;
    SVC_CV_DISP_OSD_CMD_s   RectOsdCmd;
    SVC_CV_DISP_OSD_STR_s   StrOsdCmd;
    SVC_CV_CAMCTRL_MSG_s    CamMsg;
    OD_DRAW_INFO_s    *pDrawInfo = g_ODDrawCtrl[CvFlowChan].pDrawInfo;
    const RECT_POS_s        *pBbxOsdPos;
    UINT32 OsdWidth, OsdHeight;
    char Osd_String[12];
    UINT32 Arg[1];
    UINT32 Draw3bbx = 0U;
    UINT32 Map3Dto2DIdx = 0U;
    UINT32 ColorOf3Dbbx;

    if (pDrawInfo != NULL) {
        Map3Dto2DIdx = 0U;
        for (i = 0; i < pList->Info3Dbbx.TotalNum; i++) {
            //No need to draw 3D bbx if VerticeStatus != 255U
            if (pList->Info3Dbbx.SrImg3dData[i].VerticeStatus != 255U) {
                continue;
            }
            //Collect and set flag(DRAWN_3D_BBX) to represent 2D idx no deed to draw because of it already draw 3D
            if ((AMBA_3D_MODEL_CUBOID == pList->Info3Dbbx.SrImg3dData[i].ModelType )||
                (AMBA_3D_MODEL_DIR_CUBOID == pList->Info3Dbbx.SrImg3dData[i].ModelType)) {
                for (Map3Dto2DIdx = 0; Map3Dto2DIdx < pList->BbxOut.BbxAmount; Map3Dto2DIdx++) {
                    if (pList->Info3Dbbx.ObjectId[i] == pList->BbxOut.Bbx[Map3Dto2DIdx].ObjectId) {
                       pDrawInfo->BbxDrawFlag[Map3Dto2DIdx] |= DRAWN_3D_BBX;
                        break;
                    }
                }
            }
            Draw3bbx = 0U;
            Osd3DCmd.Channel = pDrawInfo->OsdReg.Index;
            Osd3DCmd.Thickness = 2U;
            Osd3DCmd.Color = ODDrawTask_OsdColorMap(ODColorMap, (UINT32) pList->BbxOut.Bbx[Map3Dto2DIdx].Cat);
            SvcOsd_ColorIdxToColorSetting(Osd3DCmd.Channel, Osd3DCmd.Color, &ColorOf3Dbbx);
            Osd3DCmd.Color = ColorOf3Dbbx;
            if (AMBA_3D_MODEL_DIR_CUBOID == pList->Info3Dbbx.SrImg3dData[i].ModelType) {
                //Have direction
                //Rear->Rear; Front->Front
                Draw3bbx = 1U;
                Osd3DCmd.HaveDir = 1U;
                Osd3DCmd.RBL_x = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_DIRCUBE_VERTICE_RBL].X) * pDrawInfo->OsdReg.W) / pDrawInfo->PrevSrc.W);
                Osd3DCmd.RBL_y = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_DIRCUBE_VERTICE_RBL].Y) * pDrawInfo->OsdReg.H) / pDrawInfo->PrevSrc.H);
                Osd3DCmd.RBR_x = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_DIRCUBE_VERTICE_RBR].X) * pDrawInfo->OsdReg.W) / pDrawInfo->PrevSrc.W);
                Osd3DCmd.RBR_y = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_DIRCUBE_VERTICE_RBR].Y) * pDrawInfo->OsdReg.H) / pDrawInfo->PrevSrc.H);
                Osd3DCmd.FBL_x = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_DIRCUBE_VERTICE_FBL].X) * pDrawInfo->OsdReg.W) / pDrawInfo->PrevSrc.W);
                Osd3DCmd.FBL_y = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_DIRCUBE_VERTICE_FBL].Y) * pDrawInfo->OsdReg.H) / pDrawInfo->PrevSrc.H);
                Osd3DCmd.FBR_x = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_DIRCUBE_VERTICE_FBR].X) * pDrawInfo->OsdReg.W) / pDrawInfo->PrevSrc.W);
                Osd3DCmd.FBR_y = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_DIRCUBE_VERTICE_FBR].Y) * pDrawInfo->OsdReg.H) / pDrawInfo->PrevSrc.H);
                Osd3DCmd.RTL_x = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_DIRCUBE_VERTICE_RTL].X) * pDrawInfo->OsdReg.W) / pDrawInfo->PrevSrc.W);
                Osd3DCmd.RTL_y = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_DIRCUBE_VERTICE_RTL].Y) * pDrawInfo->OsdReg.H) / pDrawInfo->PrevSrc.H);
                Osd3DCmd.RTR_x = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_DIRCUBE_VERTICE_RTR].X) * pDrawInfo->OsdReg.W) / pDrawInfo->PrevSrc.W);
                Osd3DCmd.RTR_y = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_DIRCUBE_VERTICE_RTR].Y) * pDrawInfo->OsdReg.H) / pDrawInfo->PrevSrc.H);
                Osd3DCmd.FTL_x = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_DIRCUBE_VERTICE_FTL].X) * pDrawInfo->OsdReg.W) / pDrawInfo->PrevSrc.W);
                Osd3DCmd.FTL_y = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_DIRCUBE_VERTICE_FTL].Y) * pDrawInfo->OsdReg.H) / pDrawInfo->PrevSrc.H);
                Osd3DCmd.FTR_x = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_DIRCUBE_VERTICE_FTR].X) * pDrawInfo->OsdReg.W) / pDrawInfo->PrevSrc.W);
                Osd3DCmd.FTR_y = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_DIRCUBE_VERTICE_FTR].Y) * pDrawInfo->OsdReg.H) / pDrawInfo->PrevSrc.H);
            } else if (AMBA_3D_MODEL_CUBOID == pList->Info3Dbbx.SrImg3dData[i].ModelType) {
                //Have direction
                //Rear->Close; Front->Far
                Draw3bbx = 1U;
                Osd3DCmd.HaveDir = 0U;
                Osd3DCmd.RBL_x = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_CUBOID_VERTICE_CBL].X) * pDrawInfo->OsdReg.W) / pDrawInfo->PrevSrc.W);
                Osd3DCmd.RBL_y = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_CUBOID_VERTICE_CBL].Y) * pDrawInfo->OsdReg.H) / pDrawInfo->PrevSrc.H);
                Osd3DCmd.RBR_x = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_CUBOID_VERTICE_CBR].X) * pDrawInfo->OsdReg.W) / pDrawInfo->PrevSrc.W);
                Osd3DCmd.RBR_y = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_CUBOID_VERTICE_CBR].Y) * pDrawInfo->OsdReg.H) / pDrawInfo->PrevSrc.H);
                Osd3DCmd.FBL_x = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_CUBOID_VERTICE_FBL].X) * pDrawInfo->OsdReg.W) / pDrawInfo->PrevSrc.W);
                Osd3DCmd.FBL_y = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_CUBOID_VERTICE_FBL].Y) * pDrawInfo->OsdReg.H) / pDrawInfo->PrevSrc.H);
                Osd3DCmd.FBR_x = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_CUBOID_VERTICE_FBR].X) * pDrawInfo->OsdReg.W) / pDrawInfo->PrevSrc.W);
                Osd3DCmd.FBR_y = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_CUBOID_VERTICE_FBR].Y) * pDrawInfo->OsdReg.H) / pDrawInfo->PrevSrc.H);
                Osd3DCmd.RTL_x = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_CUBOID_VERTICE_CTL].X) * pDrawInfo->OsdReg.W) / pDrawInfo->PrevSrc.W);
                Osd3DCmd.RTL_y = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_CUBOID_VERTICE_CTL].Y) * pDrawInfo->OsdReg.H) / pDrawInfo->PrevSrc.H);
                Osd3DCmd.RTR_x = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_CUBOID_VERTICE_CTR].X) * pDrawInfo->OsdReg.W) / pDrawInfo->PrevSrc.W);
                Osd3DCmd.RTR_y = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_CUBOID_VERTICE_CTR].Y) * pDrawInfo->OsdReg.H) / pDrawInfo->PrevSrc.H);
                Osd3DCmd.FTL_x = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_CUBOID_VERTICE_FTL].X) * pDrawInfo->OsdReg.W) / pDrawInfo->PrevSrc.W);
                Osd3DCmd.FTL_y = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_CUBOID_VERTICE_FTL].Y) * pDrawInfo->OsdReg.H) / pDrawInfo->PrevSrc.H);
                Osd3DCmd.FTR_x = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_CUBOID_VERTICE_FTR].X) * pDrawInfo->OsdReg.W) / pDrawInfo->PrevSrc.W);
                Osd3DCmd.FTR_y = ((((UINT32)pList->Info3Dbbx.SrImg3dData[i].Vertices[AMBA_3D_MD_CUBOID_VERTICE_FTR].Y) * pDrawInfo->OsdReg.H) / pDrawInfo->PrevSrc.H);
            } else {
                Draw3bbx = 0U;
            }

            if (1U == Draw3bbx) {
                /* avoid draw over canvas boundary */
                {
                    if ((Osd3DCmd.RBL_x + Osd3DCmd.Thickness) >= pDrawInfo->OsdReg.W) {Osd3DCmd.RBL_x = pDrawInfo->OsdReg.W - Osd3DCmd.Thickness;}
                    if ((Osd3DCmd.RBL_y + Osd3DCmd.Thickness) >= pDrawInfo->OsdReg.H) {Osd3DCmd.RBL_y = pDrawInfo->OsdReg.H - Osd3DCmd.Thickness;}

                    if ((Osd3DCmd.RBR_x + Osd3DCmd.Thickness) >= pDrawInfo->OsdReg.W) {Osd3DCmd.RBR_x = pDrawInfo->OsdReg.W - Osd3DCmd.Thickness;}
                    if ((Osd3DCmd.RBR_y + Osd3DCmd.Thickness) >= pDrawInfo->OsdReg.H) {Osd3DCmd.RBR_y = pDrawInfo->OsdReg.H - Osd3DCmd.Thickness;}

                    if ((Osd3DCmd.FBL_x + Osd3DCmd.Thickness) >= pDrawInfo->OsdReg.W) {Osd3DCmd.FBL_x = pDrawInfo->OsdReg.W - Osd3DCmd.Thickness;}
                    if ((Osd3DCmd.FBL_y + Osd3DCmd.Thickness) >= pDrawInfo->OsdReg.H) {Osd3DCmd.FBL_y = pDrawInfo->OsdReg.H - Osd3DCmd.Thickness;}

                    if ((Osd3DCmd.FBR_x + Osd3DCmd.Thickness) >= pDrawInfo->OsdReg.W) {Osd3DCmd.FBR_x = pDrawInfo->OsdReg.W - Osd3DCmd.Thickness;}
                    if ((Osd3DCmd.FBR_y + Osd3DCmd.Thickness) >= pDrawInfo->OsdReg.H) {Osd3DCmd.FBR_y = pDrawInfo->OsdReg.H - Osd3DCmd.Thickness;}

                    if ((Osd3DCmd.RTL_x + Osd3DCmd.Thickness) >= pDrawInfo->OsdReg.W) {Osd3DCmd.RTL_x = pDrawInfo->OsdReg.W - Osd3DCmd.Thickness;}
                    if ((Osd3DCmd.RTL_y + Osd3DCmd.Thickness) >= pDrawInfo->OsdReg.H) {Osd3DCmd.RTL_y = pDrawInfo->OsdReg.H - Osd3DCmd.Thickness;}

                    if ((Osd3DCmd.RTR_x + Osd3DCmd.Thickness) >= pDrawInfo->OsdReg.W) {Osd3DCmd.RTR_x = pDrawInfo->OsdReg.W - Osd3DCmd.Thickness;}
                    if ((Osd3DCmd.RTR_y + Osd3DCmd.Thickness) >= pDrawInfo->OsdReg.H) {Osd3DCmd.RTR_y = pDrawInfo->OsdReg.H - Osd3DCmd.Thickness;}

                    if ((Osd3DCmd.FTL_x + Osd3DCmd.Thickness) >= pDrawInfo->OsdReg.W) {Osd3DCmd.FTL_x = pDrawInfo->OsdReg.W - Osd3DCmd.Thickness;}
                    if ((Osd3DCmd.FTL_y + Osd3DCmd.Thickness) >= pDrawInfo->OsdReg.H) {Osd3DCmd.FTL_y = pDrawInfo->OsdReg.H - Osd3DCmd.Thickness;}

                    if ((Osd3DCmd.FTR_x + Osd3DCmd.Thickness) >= pDrawInfo->OsdReg.W) {Osd3DCmd.FTR_x = pDrawInfo->OsdReg.W - Osd3DCmd.Thickness;}
                    if ((Osd3DCmd.FTR_y + Osd3DCmd.Thickness) >= pDrawInfo->OsdReg.H) {Osd3DCmd.FTR_y = pDrawInfo->OsdReg.H - Osd3DCmd.Thickness;}
                }

                /* add offset for the region */
                {
                    Osd3DCmd.RBL_x += pDrawInfo->OsdReg.X;
                    Osd3DCmd.RBL_y += pDrawInfo->OsdReg.Y;
                    Osd3DCmd.RBR_x += pDrawInfo->OsdReg.X;
                    Osd3DCmd.RBR_y += pDrawInfo->OsdReg.Y;
                    Osd3DCmd.FBL_x += pDrawInfo->OsdReg.X;
                    Osd3DCmd.FBL_y += pDrawInfo->OsdReg.Y;
                    Osd3DCmd.FBR_x += pDrawInfo->OsdReg.X;
                    Osd3DCmd.FBR_y += pDrawInfo->OsdReg.Y;
                    Osd3DCmd.RTL_x += pDrawInfo->OsdReg.X;
                    Osd3DCmd.RTL_y += pDrawInfo->OsdReg.Y;
                    Osd3DCmd.RTR_x += pDrawInfo->OsdReg.X;
                    Osd3DCmd.RTR_y += pDrawInfo->OsdReg.Y;
                    Osd3DCmd.FTL_x += pDrawInfo->OsdReg.X;
                    Osd3DCmd.FTL_y += pDrawInfo->OsdReg.Y;
                    Osd3DCmd.FTR_x += pDrawInfo->OsdReg.X;
                    Osd3DCmd.FTR_y += pDrawInfo->OsdReg.Y;
                }

                CamMsg.Msg = SVC_CV_OSD_DRAW_3D;
                CamMsg.pCtx = &Osd3DCmd;
                ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);
            }

        }

        for (i = 0; i < pList->BbxOut.BbxAmount; i++) {
            UINT32 SkipDraw2Dbbx = 0U;
            if ((pDrawInfo->BbxDrawFlag[i] & VALID_DRAW_OBJ) == 0U) {
                continue;
            }
            pBbxOsdPos = &pDrawInfo->BbxOsdPos[i];

            //Check flag, "DRAWN_3D_BBX", if it enable means no need to draw 2D bbx because if already draw 3D bbx
            if (((pDrawInfo->BbxDrawFlag[i] & DRAWN_3D_BBX)) == DRAWN_3D_BBX) {
                //Set SkipDraw2Dbbx = 1U to skip draw 2D bbx
                SkipDraw2Dbbx = 1U;
            }

            /* put bbx msg */
            RectOsdCmd.Channel = pDrawInfo->OsdReg.Index;
            RectOsdCmd.X1 = pBbxOsdPos->X1;
            RectOsdCmd.Y1 = pBbxOsdPos->Y1;
            RectOsdCmd.X2 = pBbxOsdPos->X2;
            RectOsdCmd.Y2 = pBbxOsdPos->Y2;
            RectOsdCmd.Color = ODDrawTask_OsdColorMap(ODColorMap, (UINT32) pList->BbxOut.Bbx[i].Cat);
            RectOsdCmd.Thickness = ODRectThickness;
            RectOsdCmd.WarningLevel = 0U;

            CamMsg.Msg = SVC_CV_OSD_DRAW_RECT;
            CamMsg.pCtx = &RectOsdCmd;
            //No need to draw 2D bbx if SkipDraw2Dbbx == 1
            if (SkipDraw2Dbbx == 0U) {
                ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);
            }

            if (SVC_OK != SvcOsd_GetOsdBufSize(pDrawInfo->OsdReg.Index, &OsdWidth, &OsdHeight)) {
                SvcLog_NG(SVC_LOG_OD_TASK, "SvcOsd_GetOsdBufSize failed", 0U, 0U);
            }
            /* put score msg */
            if (ODLabelScore > 0U){
                StrOsdCmd.Channel = pDrawInfo->OsdReg.Index;

                /* swap the position of x1 and x2 for flip case */
                if (pDrawInfo->OsdRegFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP){
                    StrOsdCmd.X1 = RectOsdCmd.X2;
                } else {
                    StrOsdCmd.X1 = RectOsdCmd.X1;
                }

                /* avoid string over OSD boundary */
                StrOsdCmd.X1 = ((OsdWidth - RectOsdCmd.X1) > 50U) ? RectOsdCmd.X1 : (RectOsdCmd.X1 - 50U);
                StrOsdCmd.Y1 = (RectOsdCmd.Y1 < 20U) ? (RectOsdCmd.Y1 + 20U) : (RectOsdCmd.Y1 - 20U);
                StrOsdCmd.Color = RectOsdCmd.Color;

                if (ODLabelScore == 1U) {
                    Arg[0] = pList->BbxOut.Bbx[i].Score;
                } else {
                    Arg[0] = (pList->BbxOut.Bbx[i].Score * 100U) / ODLabelScore; /* percentage */
                }
                if (SVC_OK != AmbaUtility_StringPrintUInt32(Osd_String, sizeof(StrOsdCmd.String), "%d", 1U, Arg)) {
                    // SvcLog_NG(SVC_LOG_OD_TASK, "AmbaUtility_StringPrintUInt32 Osd_String failed", 0U, 0U);
                }
                SvcWrap_strcpy(StrOsdCmd.String, sizeof(StrOsdCmd.String), Osd_String);
                CamMsg.Msg = SVC_CV_OSD_DRAWSTRING;
                CamMsg.pCtx = &StrOsdCmd;
                ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);
            }

            /* put category msg */
            if ((ODLabelCat == 1U) &&
                ((pDrawInfo->BbxDrawFlag[i] & DRAWN_CLF_CATEGORY) == 0U)) {
                if (pList->BbxOut.Bbx[i].Cat < 255U){
                    DOUBLE CarDist = 0.0;
                    UINT32 Id;
                    UINT32 Flag = 1U;
                    UINT32 OsdBufWidth, OsdBufHeight;
                    StrOsdCmd.Channel = pDrawInfo->OsdReg.Index;

                    //swap the position of x1 and x2 for flip case
                    if (pDrawInfo->OsdRegFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP){
                        StrOsdCmd.X1 = RectOsdCmd.X2;
                    } else {
                        StrOsdCmd.X1 = RectOsdCmd.X1;
                    }

                    // avoid string over OSD boundary
                    StrOsdCmd.X1 = ((OsdWidth - RectOsdCmd.X1) > ODMaxStrLen) ? RectOsdCmd.X1 : (OsdWidth - ODMaxStrLen);
                    StrOsdCmd.Y1 = (RectOsdCmd.Y1 < 20U) ? (RectOsdCmd.Y1 + 10U) : (RectOsdCmd.Y1 - 10U);
                    StrOsdCmd.Color = RectOsdCmd.Color;

                    // put ObjectId
                    if ((0U != pList->BbxOut.Bbx[i].ObjectId) &&
                        ((OsdWidth - RectOsdCmd.X1) > 130U)) {//Category id of string drawing boundary check

                        // typedef struct {
                        //    UINT32 Camera:3;
                        //    UINT32 ObjType:5;
                        //    UINT32 ObjectId:SR_OBJ_ID_BITS;
                        //} SR_UUID_DETAIL_s;

                        UINT32 ObjectIdMSB32 = ((pList->BbxOut.Bbx[i].ObjectId )>>8);
                        StrOsdCmd.String[0] = '[';
                        StrOsdCmd.String[1] = '\0';
                        AmbaUtility_StringAppendUInt32(StrOsdCmd.String, sizeof(StrOsdCmd.String), ObjectIdMSB32, 10U);
                        AmbaUtility_StringAppend(StrOsdCmd.String, sizeof(StrOsdCmd.String), "]");
                        AmbaUtility_StringAppend(StrOsdCmd.String, sizeof(StrOsdCmd.String), pList->BbxOut.class_name[pList->BbxOut.Bbx[i].Cat]);
                    } else {
                        SvcWrap_strcpy(StrOsdCmd.String, sizeof(StrOsdCmd.String), pList->BbxOut.class_name[pList->BbxOut.Bbx[i].Cat]);
                    }
                    CamMsg.Msg = SVC_CV_OSD_DRAWSTRING;
                    CamMsg.pCtx = &StrOsdCmd;
                    ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);

                    // swap the position of x1 and x2 for flip case
                    if (pDrawInfo->OsdRegFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP){
                        StrOsdCmd.X1 = RectOsdCmd.X2;
                    } else {
                        StrOsdCmd.X1 = RectOsdCmd.X1;
                    }

                    // avoid string over OSD boundary
                    if (SVC_OK != SvcOsd_GetOsdBufSize(1U, &OsdBufWidth, &OsdBufHeight)) {
                        SvcLog_NG(SVC_LOG_OD_TASK, "SvcOsd_GetOsdBufSize failed", 0U, 0U);
                    }
                    StrOsdCmd.X1 = ((OsdWidth - RectOsdCmd.X1) > 50U) ? RectOsdCmd.X1 : (RectOsdCmd.X1 - 50U);
                    StrOsdCmd.Y1 = (RectOsdCmd.Y2 > OsdBufHeight) ? (RectOsdCmd.Y2 - 20U) : (RectOsdCmd.Y2);
                    StrOsdCmd.Color = RectOsdCmd.Color;

#if 1
                    Id = pList->BbxOut.Bbx[i].ObjectId;
                    for (UINT32 j = 0U; j < pList->SROut.TotalNum; j++) {
                        if (Id == pList->SROut.SRObject[j].Object.ObjId) {
                            UINT32 VerticeStatus = pList->SROut.SRObject[j].Object.Model.VerticeStatus;
                            UINT32 ModelType = pList->SROut.SRObject[j].Object.Model.ModelType;
                            DOUBLE y1, y2;//x1, x2,
                            if (ModelType == AMBA_3D_MODEL_CUBOID) {
                                if (((VerticeStatus & (1UL << AMBA_3D_MD_CUBOID_VERTICE_CBL)) > 0U) &&
                                    ((VerticeStatus & (1UL << AMBA_3D_MD_CUBOID_VERTICE_CBR)) > 0U)) {
                                    //x1 = pList->SROut.SRObject[j].Object.Model.Vertices[AMBA_3D_MD_CUBOID_VERTICE_CBL].X;
                                    y1 = pList->SROut.SRObject[j].Object.Model.Vertices[AMBA_3D_MD_CUBOID_VERTICE_CBL].Y;
                                    //x2 = pList->SROut.SRObject[j].Object.Model.Vertices[AMBA_3D_MD_CUBOID_VERTICE_CBR].X;
                                    y2 = pList->SROut.SRObject[j].Object.Model.Vertices[AMBA_3D_MD_CUBOID_VERTICE_CBR].Y;
                                    CarDist = (y1>y2)?y2:y1;
                                }else {
                                    Flag = 0U;
                                }
                            } else if (ModelType == AMBA_3D_MODEL_WALL) {
                                if ((VerticeStatus & (1UL << AMBA_3D_MD_WALL_VERTICE_CB)) > 0U) {
                                    //x1 = pList->SROut.SRObject[j].Object.Model.Vertices[AMBA_3D_MD_WALL_VERTICE_CB].X;
                                    y1 = pList->SROut.SRObject[j].Object.Model.Vertices[AMBA_3D_MD_WALL_VERTICE_CB].Y;
                                    //x2 = x1;
                                    y2 = y1;
                                    CarDist = (y1>y2)?y2:y1;
                                }else {
                                    Flag = 0U;
                                }
                            } else if (ModelType == AMBA_3D_MODEL_POLE) {
                                if ((VerticeStatus & (1UL << AMBA_3D_MD_POLE_VERTICE_B)) > 0U) {
                                    //x1 = pList->SROut.SRObject[j].Object.Model.Vertices[AMBA_3D_MD_POLE_VERTICE_B].X;
                                    y1 = pList->SROut.SRObject[j].Object.Model.Vertices[AMBA_3D_MD_POLE_VERTICE_B].Y;
                                    //x2 = x1;
                                    y2 = y1;
                                    CarDist = (y1>y2)?y2:y1;
                                }else {
                                    Flag = 0U;
                                }
                            } else {
                                Flag = 0U;
                            }
                            break;
                        }
                    }
#else
                    CarDist = (pList->SROut.SRObject[i].Object.Model.Vertices[AMBA_3D_MD_CUBOID_VERTICE_CBL].Y + pList->SROut.SRObject[i].Object.Model.Vertices[AMBA_3D_MD_CUBOID_VERTICE_CBL].Y)/2.0;
#endif

                    if ((CarDist > 0.0) && (Flag == 1U)) {
                        Arg[0] = ((UINT32)(CarDist))/1000U;
                        if (SVC_OK != AmbaUtility_StringPrintUInt32(Osd_String, sizeof(StrOsdCmd.String), "%d", 1U, Arg)) {
                            // SvcLog_NG(SVC_LOG_OD_TASK, "AmbaUtility_StringPrintUInt32 Osd_String failed", 0U, 0U);
                        }
                        SvcWrap_strcpy(StrOsdCmd.String, sizeof(StrOsdCmd.String), Osd_String);
                        AmbaUtility_StringAppend(StrOsdCmd.String, sizeof(StrOsdCmd.String), "m");
                        CamMsg.Msg = SVC_CV_OSD_DRAWSTRING;
                        CamMsg.pCtx = &StrOsdCmd;
                        ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);
                    }
                }
            }

            /* Highlight FC Object car */
            if ((pList->FCOut.WsFcwsStat.ObjectStatus.StatusUpdate == 1U) &&
                (pList->FCOut.WsFcwsStat.ObjectStatus.ObjectID == pList->BbxOut.Bbx[i].ObjectId)) {
                RectOsdCmd.Color = ODDrawTask_OsdColorMap(ODColorMap, (UINT32) pList->BbxOut.Bbx[i].Cat);
                RectOsdCmd.WarningLevel = 1U;
                RectOsdCmd.Color = 67U;

                CamMsg.Msg = SVC_CV_OSD_DRAW_RECT;
                CamMsg.pCtx = &RectOsdCmd;
                ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);
            }
        }
    }
}
static void ODDrawTask_FcDetResDrawBbx(UINT32 CvFlowChan, const SVC_FC_DRAW_INFO_s *pList)
{
    UINT32 i;
    SVC_CV_DISP_OSD_CMD_s   RectOsdCmd;
    SVC_CV_DISP_OSD_STR_s   StrOsdCmd;
    SVC_CV_CAMCTRL_MSG_s    CamMsg;
    const OD_DRAW_INFO_s    *pDrawInfo = g_ODDrawCtrl[CvFlowChan].pDrawInfo;
    const RECT_POS_s        *pBbxOsdPos;
    UINT32 OsdWidth, OsdHeight;
    char Osd_String[12];
    UINT32 Arg[1];

    if (pDrawInfo != NULL) {
        for (i = 0; i < pList->BbxOut.BbxAmount; i++) {
            if ((pDrawInfo->BbxDrawFlag[i] & VALID_DRAW_OBJ) == 0U) {
                continue;
            }
            pBbxOsdPos = &pDrawInfo->BbxOsdPos[i];

            /* put bbx msg */
            RectOsdCmd.Channel = pDrawInfo->OsdReg.Index;
            RectOsdCmd.X1 = pBbxOsdPos->X1;
            RectOsdCmd.Y1 = pBbxOsdPos->Y1;
            RectOsdCmd.X2 = pBbxOsdPos->X2;
            RectOsdCmd.Y2 = pBbxOsdPos->Y2;
            RectOsdCmd.Color = ODDrawTask_OsdColorMap(ODColorMap, (UINT32) pList->BbxOut.Bbx[i].Cat);
            RectOsdCmd.Thickness = ODRectThickness;
            RectOsdCmd.WarningLevel = 0U;


            CamMsg.Msg = SVC_CV_OSD_DRAW_RECT;
            CamMsg.pCtx = &RectOsdCmd;
            ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);

            if (SVC_OK != SvcOsd_GetOsdBufSize(pDrawInfo->OsdReg.Index, &OsdWidth, &OsdHeight)) {
                SvcLog_NG(SVC_LOG_OD_TASK, "SvcOsd_GetOsdBufSize failed", 0U, 0U);
            }
            /* put score msg */
            if (ODLabelScore > 0U){
                StrOsdCmd.Channel = pDrawInfo->OsdReg.Index;

                /* swap the position of x1 and x2 for flip case */
                if (pDrawInfo->OsdRegFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP){
                    StrOsdCmd.X1 = RectOsdCmd.X2;
                } else {
                    StrOsdCmd.X1 = RectOsdCmd.X1;
                }

                /* avoid string over OSD boundary */
                StrOsdCmd.X1 = ((OsdWidth - RectOsdCmd.X1) > 50U) ? RectOsdCmd.X1 : (RectOsdCmd.X1 - 50U);
                StrOsdCmd.Y1 = (RectOsdCmd.Y1 < 20U) ? (RectOsdCmd.Y1 + 20U) : (RectOsdCmd.Y1 - 20U);
                StrOsdCmd.Color = RectOsdCmd.Color;

                if (ODLabelScore == 1U) {
                    Arg[0] = pList->BbxOut.Bbx[i].Score;
                } else {
                    Arg[0] = (pList->BbxOut.Bbx[i].Score * 100U) / ODLabelScore; /* percentage */
                }
                if (SVC_OK != AmbaUtility_StringPrintUInt32(Osd_String, sizeof(StrOsdCmd.String), "%d", 1U, Arg)) {
                    // SvcLog_NG(SVC_LOG_OD_TASK, "AmbaUtility_StringPrintUInt32 Osd_String failed", 0U, 0U);
                }
                SvcWrap_strcpy(StrOsdCmd.String, sizeof(StrOsdCmd.String), Osd_String);
                CamMsg.Msg = SVC_CV_OSD_DRAWSTRING;
                CamMsg.pCtx = &StrOsdCmd;
                ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);
            }

            /* put category msg */
            if (ODLabelCat == 1U){
                if (pList->BbxOut.Bbx[i].Cat < 255U){
                    DOUBLE CarDist = 0.0;
                    UINT32 Id;
                    UINT32 Flag = 1U;
                    UINT32 OsdBufWidth, OsdBufHeight;
                    StrOsdCmd.Channel = pDrawInfo->OsdReg.Index;

                    /* swap the position of x1 and x2 for flip case */
                    if (pDrawInfo->OsdRegFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP){
                        StrOsdCmd.X1 = RectOsdCmd.X2;
                    } else {
                        StrOsdCmd.X1 = RectOsdCmd.X1;
                    }

                    /* avoid string over OSD boundary */
                    StrOsdCmd.X1 = ((OsdWidth - StrOsdCmd.X1) > ODMaxStrLen) ? StrOsdCmd.X1 : (OsdWidth - ODMaxStrLen);
                    StrOsdCmd.Y1 = (RectOsdCmd.Y1 < 20U) ? (RectOsdCmd.Y1 + 10U) : (RectOsdCmd.Y1 - 10U);
                    StrOsdCmd.Color = RectOsdCmd.Color;

                    /* put ObjectId */
                    if ((0U != pList->BbxOut.Bbx[i].ObjectId) &&
                        ((OsdWidth - RectOsdCmd.X1) > 130U)) {//Category id of string drawing boundary check
                        /*
                         typedef struct {
                            UINT32 Camera:3;
                            UINT32 ObjType:5;
                            UINT32 ObjectId:SR_OBJ_ID_BITS;
                        } SR_UUID_DETAIL_s;
                  */
                        UINT32 ObjectIdMSB32 = ((pList->BbxOut.Bbx[i].ObjectId )>>8);
                        StrOsdCmd.String[0] = '[';
                        StrOsdCmd.String[1] = '\0';
                        AmbaUtility_StringAppendUInt32(StrOsdCmd.String, sizeof(StrOsdCmd.String), ObjectIdMSB32, 10U);
                        AmbaUtility_StringAppend(StrOsdCmd.String, sizeof(StrOsdCmd.String), "]");
                        AmbaUtility_StringAppend(StrOsdCmd.String, sizeof(StrOsdCmd.String), pList->BbxOut.class_name[pList->BbxOut.Bbx[i].Cat]);
                    } else {
                        SvcWrap_strcpy(StrOsdCmd.String, sizeof(StrOsdCmd.String), pList->BbxOut.class_name[pList->BbxOut.Bbx[i].Cat]);
                    }
                    CamMsg.Msg = SVC_CV_OSD_DRAWSTRING;
                    CamMsg.pCtx = &StrOsdCmd;
                    ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);


                    /* swap the position of x1 and x2 for flip case */
                    if (pDrawInfo->OsdRegFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP){
                        StrOsdCmd.X1 = RectOsdCmd.X2;
                    } else {
                        StrOsdCmd.X1 = RectOsdCmd.X1;
                    }

                    /* avoid string over OSD boundary */
                    if (SVC_OK != SvcOsd_GetOsdBufSize(1U, &OsdBufWidth, &OsdBufHeight)) {
                        SvcLog_NG(SVC_LOG_OD_TASK, "SvcOsd_GetOsdBufSize failed", 0U, 0U);
                    }
                    StrOsdCmd.X1 = ((OsdWidth - RectOsdCmd.X1) > 50U) ? RectOsdCmd.X1 : (RectOsdCmd.X1 - 50U);
                    StrOsdCmd.Y1 = (RectOsdCmd.Y2 > OsdBufHeight) ? (RectOsdCmd.Y2 - 20U) : (RectOsdCmd.Y2);
                    StrOsdCmd.Color = RectOsdCmd.Color;

#if 1
                    Id = pList->BbxOut.Bbx[i].ObjectId;
                    for (UINT32 j = 0U; j < pList->SROut.TotalNum; j++) {
                        if (Id == pList->SROut.SRObject[j].Object.ObjId) {
                            UINT32 VerticeStatus = pList->SROut.SRObject[j].Object.Model.VerticeStatus;
                            UINT32 ModelType = pList->SROut.SRObject[j].Object.Model.ModelType;
                            DOUBLE /*x1, x2,*/ y1, y2;
                            if (ModelType == AMBA_3D_MODEL_CUBOID) {
                                if (((VerticeStatus & (1UL << AMBA_3D_MD_CUBOID_VERTICE_CBL)) > 0U) &&
                                    ((VerticeStatus & (1UL << AMBA_3D_MD_CUBOID_VERTICE_CBR)) > 0U)) {
                                    //x1 = pList->SROut.SRObject[j].Object.Model.Vertices[AMBA_3D_MD_CUBOID_VERTICE_CBL].X;
                                    y1 = pList->SROut.SRObject[j].Object.Model.Vertices[AMBA_3D_MD_CUBOID_VERTICE_CBL].Y;
                                    //x2 = pList->SROut.SRObject[j].Object.Model.Vertices[AMBA_3D_MD_CUBOID_VERTICE_CBR].X;
                                    y2 = pList->SROut.SRObject[j].Object.Model.Vertices[AMBA_3D_MD_CUBOID_VERTICE_CBR].Y;
                                    CarDist = (y1>y2)?y2:y1;
                                }else {
                                    Flag = 0U;
                                }
                            } else if (ModelType == AMBA_3D_MODEL_WALL) {
                                if ((VerticeStatus & (1UL << AMBA_3D_MD_WALL_VERTICE_CB)) > 0U) {
                                    //x1 = pList->SROut.SRObject[j].Object.Model.Vertices[AMBA_3D_MD_WALL_VERTICE_CB].X;
                                    y1 = pList->SROut.SRObject[j].Object.Model.Vertices[AMBA_3D_MD_WALL_VERTICE_CB].Y;
                                    //x2 = x1;
                                    y2 = y1;
                                    CarDist = (y1>y2)?y2:y1;
                                }else {
                                    Flag = 0U;
                                }
                            } else if (ModelType == AMBA_3D_MODEL_POLE) {
                                if ((VerticeStatus & (1UL << AMBA_3D_MD_POLE_VERTICE_B)) > 0U) {
                                    //x1 = pList->SROut.SRObject[j].Object.Model.Vertices[AMBA_3D_MD_POLE_VERTICE_B].X;
                                    y1 = pList->SROut.SRObject[j].Object.Model.Vertices[AMBA_3D_MD_POLE_VERTICE_B].Y;
                                    //x2 = x1;
                                    y2 = y1;
                                    CarDist = (y1>y2)?y2:y1;
                                }else {
                                    Flag = 0U;
                                }
                            } else {
                                Flag = 0U;
                            }
                            break;
                        }
                    }
#else
                    CarDist = (pList->SROut.SRObject[i].Object.Model.Vertices[AMBA_3D_MD_CUBOID_VERTICE_CBL].Y + pList->SROut.SRObject[i].Object.Model.Vertices[AMBA_3D_MD_CUBOID_VERTICE_CBL].Y)/2.0;
#endif

                    if ((CarDist > 0.0) && (Flag == 1U)) {
                        Arg[0] = ((UINT32)(CarDist))/1000U;
                        if (SVC_OK != AmbaUtility_StringPrintUInt32(Osd_String, sizeof(StrOsdCmd.String), "%d", 1U, Arg)) {
                            // SvcLog_NG(SVC_LOG_OD_TASK, "AmbaUtility_StringPrintUInt32 Osd_String failed", 0U, 0U);
                        }
                        SvcWrap_strcpy(StrOsdCmd.String, sizeof(StrOsdCmd.String), Osd_String);
                        AmbaUtility_StringAppend(StrOsdCmd.String, sizeof(StrOsdCmd.String), "m");
                        CamMsg.Msg = SVC_CV_OSD_DRAWSTRING;
                        CamMsg.pCtx = &StrOsdCmd;
                        ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);
                    }
                }
            }

            /* Highlight FC Object car */
            if ((pList->FCOut.WsFcwsStat.ObjectStatus.StatusUpdate == 1U) &&
                (pList->FCOut.WsFcwsStat.ObjectStatus.ObjectID == pList->BbxOut.Bbx[i].ObjectId)) {
                RectOsdCmd.Color = ODDrawTask_OsdColorMap(ODColorMap, (UINT32) pList->BbxOut.Bbx[i].Cat);
                RectOsdCmd.WarningLevel = 1U;
                RectOsdCmd.Color = 67U;

                CamMsg.Msg = SVC_CV_OSD_DRAW_RECT;
                CamMsg.pCtx = &RectOsdCmd;
                ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);
            }
        }
    }
}
#endif

#if defined(CONFIG_ICAM_PROJECT_EMIRROR) && defined(CONFIG_BUILD_AMBA_ADAS)
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ODDrawTask_BsdDetResDrawBbx
 *
 *  @Description:: Draw BBX
 *
 *  @Input      ::
 *                 CvFlowChan: CvFlow channel
 *                 pList: The list contains the bounding box information
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 None
\*-----------------------------------------------------------------------------------------------*/
static void ODDrawTask_BsdDetResDrawBbx(UINT32 CvFlowChan, const SVC_CV_DETRES_BBX_LIST_s *pList, AMBA_SR_SROBJECT_DATA_s *pSRData)
{
    static UINT32 ODLabelPos = 1U;

    UINT32 i;
    SVC_CV_DISP_OSD_CMD_s   RectOsdCmd;
    SVC_CV_DISP_OSD_STR_s   StrOsdCmd;
    SVC_CV_CAMCTRL_MSG_s    CamMsg;
    RECT_POS_s              BbxPos;
    const OD_DRAW_INFO_s    *pDrawInfo = g_ODDrawCtrl[CvFlowChan].pDrawInfo;
    UINT32 OsdWidth, OsdHeight;
    char Osd_String[12];
    UINT32 Arg[2];

    AmbaMisra_TouchUnused(pSRData);

    if (pDrawInfo != NULL) {
        for (i = 0; i < pList->BbxAmount; i++) {
            /* Rebase BBX from main window to VOUT src window */
            BbxPos.X1 = pList->Bbx[i].X;
            BbxPos.Y1 = pList->Bbx[i].Y;
            BbxPos.X2 = (UINT32)pList->Bbx[i].X + (UINT32)pList->Bbx[i].W - 1U;
            BbxPos.Y2 = (UINT32)pList->Bbx[i].Y + (UINT32)pList->Bbx[i].H - 1U;
            if (pDrawInfo->PrevCrop != 0U) {
                if (0U == ODDrawTask_DetResRebaseObj(&BbxPos, &pDrawInfo->PrevSrc)) {
                    continue;
                }
            }

            /* put rectangle msg */
            RectOsdCmd.Channel = pDrawInfo->OsdReg.Index;
            RectOsdCmd.X1 = (BbxPos.X1 * pDrawInfo->OsdReg.W) / pDrawInfo->PrevSrc.W;
            RectOsdCmd.Y1 = (BbxPos.Y1 * pDrawInfo->OsdReg.H) / pDrawInfo->PrevSrc.H;
            RectOsdCmd.X2 = (BbxPos.X2 * pDrawInfo->OsdReg.W) / pDrawInfo->PrevSrc.W;
            RectOsdCmd.Y2 = (BbxPos.Y2 * pDrawInfo->OsdReg.H) / pDrawInfo->PrevSrc.H;

            if (pDrawInfo->OsdRegFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP) {
                RectOsdCmd.X1 = pDrawInfo->OsdReg.W - RectOsdCmd.X1;
                RectOsdCmd.X2 = pDrawInfo->OsdReg.W - RectOsdCmd.X2;
            }

            /* avoid draw over canvas boundary */
            RectOsdCmd.Thickness = ODRectThickness;
            if ((RectOsdCmd.X1 + RectOsdCmd.Thickness) >= pDrawInfo->OsdReg.W) {
                RectOsdCmd.X1 = pDrawInfo->OsdReg.W - RectOsdCmd.Thickness;
            }
            if ((RectOsdCmd.X2 + RectOsdCmd.Thickness) >= pDrawInfo->OsdReg.W) {
                RectOsdCmd.X2 = pDrawInfo->OsdReg.W - RectOsdCmd.Thickness;
            }

            if ((RectOsdCmd.Y1 + RectOsdCmd.Thickness) >= pDrawInfo->OsdReg.H) {
                RectOsdCmd.Y1 = pDrawInfo->OsdReg.H - RectOsdCmd.Thickness;
            }
            if ((RectOsdCmd.Y2 + RectOsdCmd.Thickness) >= pDrawInfo->OsdReg.H) {
                RectOsdCmd.Y2 = pDrawInfo->OsdReg.H - RectOsdCmd.Thickness;
            }

            /* add offset for the region */
            RectOsdCmd.X1 += pDrawInfo->OsdReg.X;
            RectOsdCmd.Y1 += pDrawInfo->OsdReg.Y;
            RectOsdCmd.X2 += pDrawInfo->OsdReg.X;
            RectOsdCmd.Y2 += pDrawInfo->OsdReg.Y;
            RectOsdCmd.Color = ODDrawTask_OsdColorMap(ODColorMap, (UINT32) pList->Bbx[i].Cat);
            if(pList->Bbx[i].Cat == LV1_CAT) {
                RectOsdCmd.WarningLevel = 1U;
            } else if(pList->Bbx[i].Cat == LV2_CAT) {
                RectOsdCmd.WarningLevel = 1U;
            } else if(pList->Bbx[i].Cat == LV1_CLS_CAT) {
                RectOsdCmd.WarningLevel = 1U;
            } else{
                RectOsdCmd.WarningLevel = 0U;
            }

            CamMsg.Msg = SVC_CV_OSD_DRAW_RECT;
            CamMsg.pCtx = &RectOsdCmd;
            ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);

            if (SVC_OK != SvcOsd_GetOsdBufSize(pDrawInfo->OsdReg.Index, &OsdWidth, &OsdHeight)) {
                SvcLog_NG(SVC_LOG_OD_TASK, "SvcOsd_GetOsdBufSize failed", 0U, 0U);
            }
            /* put score msg */
            if (ODLabelScore > 0U) {
                StrOsdCmd.Channel = pDrawInfo->OsdReg.Index;

                /* swap the position of x1 and x2 for flip case */
                if (pDrawInfo->OsdRegFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP){
                    StrOsdCmd.X1 = RectOsdCmd.X2;
                } else {
                    StrOsdCmd.X1 = RectOsdCmd.X1;
                }

                /* avoid string over OSD boundary */
                StrOsdCmd.X1 = ((OsdWidth - RectOsdCmd.X1) > 50U) ? RectOsdCmd.X1 : (RectOsdCmd.X1 - 50U);
                StrOsdCmd.Y1 = (RectOsdCmd.Y1 < 20U) ? (RectOsdCmd.Y1 + 20U) : (RectOsdCmd.Y1 - 20U);
                StrOsdCmd.Color = RectOsdCmd.Color;

                if (ODLabelScore == 1U) {
                    Arg[0] = pList->Bbx[i].Score;
                } else {
                    Arg[0] = (pList->Bbx[i].Score * 100U) / ODLabelScore; /* percentage */
                }
                if (SVC_OK != AmbaUtility_StringPrintUInt32(Osd_String, sizeof(StrOsdCmd.String), "%d", 1U, Arg)) {
                    // SvcLog_NG(SVC_LOG_OD_TASK, "AmbaUtility_StringPrintUInt32 Osd_String failed", 0U, 0U);
                }
                SvcWrap_strcpy(StrOsdCmd.String, sizeof(StrOsdCmd.String), Osd_String);
                CamMsg.Msg = SVC_CV_OSD_DRAWSTRING;
                CamMsg.pCtx = &StrOsdCmd;
                ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);
            }

            /* put category msg */
            #if 0
            if (ODLabelCat == 1U) {
                if (pList->Bbx[i].Cat < 255U){
                    StrOsdCmd.Channel = pDrawInfo->OsdReg.Index;

                    /* swap the position of x1 and x2 for flip case */
                    if (pDrawInfo->OsdRegFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP){
                        StrOsdCmd.X1 = RectOsdCmd.X2;
                    } else {
                        StrOsdCmd.X1 = RectOsdCmd.X1;
                    }

                    /* avoid string over OSD boundary */
                    StrOsdCmd.X1 = ((OsdWidth - RectOsdCmd.X1) > 50U) ? RectOsdCmd.X1 : (RectOsdCmd.X1 - 50U);
                    StrOsdCmd.Y1 = (RectOsdCmd.Y1 < 20U) ? (RectOsdCmd.Y1 + 10U) : (RectOsdCmd.Y1 - 10U);
                    StrOsdCmd.Color = RectOsdCmd.Color;


                    SvcWrap_strcpy(StrOsdCmd.String, sizeof(StrOsdCmd.String), pList->class_name[pList->Bbx[i].Cat]);
                    CamMsg.Msg = SVC_CV_OSD_DRAWSTRING;
                    CamMsg.pCtx = &StrOsdCmd;
                    ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);
                }
            }
            #endif

            /* put category msg */
            if (ODLabelPos == 1U) {
                if (pList->Bbx[i].Cat < 255U) {
                    DOUBLE CarDist;

                    StrOsdCmd.Channel = pDrawInfo->OsdReg.Index;

                    /* swap the position of x1 and x2 for flip case */
                    if (pDrawInfo->OsdRegFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP){
                        StrOsdCmd.X1 = RectOsdCmd.X2;
                    } else {
                        StrOsdCmd.X1 = RectOsdCmd.X1;
                    }

                    /* avoid string over OSD boundary */
                    StrOsdCmd.X1 = ((OsdWidth - RectOsdCmd.X1) > 50U) ? RectOsdCmd.X1 : (RectOsdCmd.X1 - 50U);
                    StrOsdCmd.Y1 = (RectOsdCmd.Y1 < 20U) ? (RectOsdCmd.Y1 + 10U) : (RectOsdCmd.Y1 - 10U);
                    StrOsdCmd.Color = 28U;

                    #if 0
                    if (SVC_OK != AmbaUtility_DoubleToStr(Osd_String, 32U, pSRData->SRObject[i].RealSpeed, 0U)) {
                        // SvcLog_NG(SVC_LOG_OD_TASK, "AmbaUtility_DoubleToStr Osd_String failed", 0U, 0U);
                    }
                    SvcWrap_strcpy(StrOsdCmd.String, sizeof(StrOsdCmd.String), Osd_String);
                    AmbaUtility_StringAppend(StrOsdCmd.String, sizeof(StrOsdCmd.String), "km/");
                    #endif

                    CarDist = -((pSRData->SRObject[i].Object.Model.Vertices[AMBA_3D_MD_CUBOID_VERTICE_CBL].Y + pSRData->SRObject[i].Object.Model.Vertices[AMBA_3D_MD_CUBOID_VERTICE_CBR].Y)/2.0);
                    CarDist = CarDist/1000.0;

                    if(CarDist <= 80.0){
                        if(CarDist >= 0.0){
                            if (SVC_OK != AmbaUtility_DoubleToStr(Osd_String, sizeof(Osd_String), CarDist, 1U)) {
                                // SvcLog_NG(SVC_LOG_OD_TASK, "AmbaUtility_DoubleToStr Osd_String failed", 0U, 0U);
                            }
                            SvcWrap_strcpy(StrOsdCmd.String, sizeof(StrOsdCmd.String), Osd_String);
                            AmbaUtility_StringAppend(StrOsdCmd.String, sizeof(StrOsdCmd.String), "m");
                            CamMsg.Msg = SVC_CV_OSD_DRAWSTRING;
                            CamMsg.pCtx = &StrOsdCmd;
                            ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);
                        }
                    }
                }
            }
        }
    }
}

static void ODDrawTask_RmgDetResDrawBbx(UINT32 CvFlowChan, SVC_CV_DETRES_BBX_LIST_s *pList, AMBA_SR_SROBJECT_DATA_s *pSRData)
{
    static UINT32 ODLabelPos = 1U;

    UINT32 i;
    SVC_CV_DISP_OSD_CMD_s   RectOsdCmd;
    SVC_CV_DISP_OSD_STR_s   StrOsdCmd;
    SVC_CV_CAMCTRL_MSG_s    CamMsg;
    RECT_POS_s              BbxPos;
    const OD_DRAW_INFO_s    *pDrawInfo = g_ODDrawCtrl[CvFlowChan].pDrawInfo;
    UINT32 OsdWidth, OsdHeight;
    char Osd_String[12];

    AmbaMisra_TouchUnused(pSRData);

    if (pDrawInfo != NULL) {
        for (i = 0; i < pList->BbxAmount; i++) {

            /* abandon bbx which cat is IGNORE */
            if (pList->Bbx[i].Cat == RmgCatNone) {
                continue;
            }
            if ((pList->Bbx[i].Cat > RmgCatEgo) && (pList->Bbx[i].Cat < RmgCatNum)) {
                ODColorMap = SVC_ADAS_COLOR_MAP;
                pList->Bbx[i].Cat -= RmgCatNone;
            } else {
                ODColorMap = SVC_EMR_COLOR_MAP;
            }

            /* Rebase BBX from main window to VOUT src window */
            BbxPos.X1 = pList->Bbx[i].X;
            BbxPos.Y1 = pList->Bbx[i].Y;
            BbxPos.X2 = (UINT32)pList->Bbx[i].X + (UINT32)pList->Bbx[i].W - 1U;
            BbxPos.Y2 = (UINT32)pList->Bbx[i].Y + (UINT32)pList->Bbx[i].H - 1U;
            if (pDrawInfo->PrevCrop != 0U) {
                if (0U == ODDrawTask_DetResRebaseObj(&BbxPos, &pDrawInfo->PrevSrc)) {
                    continue;
                }
            }

            /* put rectangle msg */
            RectOsdCmd.Channel = pDrawInfo->OsdReg.Index;
            RectOsdCmd.X1 = (BbxPos.X1 * pDrawInfo->OsdReg.W) / pDrawInfo->PrevSrc.W;
            RectOsdCmd.Y1 = (BbxPos.Y1 * pDrawInfo->OsdReg.H) / pDrawInfo->PrevSrc.H;
            RectOsdCmd.X2 = (BbxPos.X2 * pDrawInfo->OsdReg.W) / pDrawInfo->PrevSrc.W;
            RectOsdCmd.Y2 = (BbxPos.Y2 * pDrawInfo->OsdReg.H) / pDrawInfo->PrevSrc.H;

            if (pDrawInfo->OsdRegFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP) {
                RectOsdCmd.X1 = pDrawInfo->OsdReg.W - RectOsdCmd.X1;
                RectOsdCmd.X2 = pDrawInfo->OsdReg.W - RectOsdCmd.X2;
            }

            /* avoid draw over canvas boundary */
            RectOsdCmd.Thickness = ODRectThickness;
            if ((RectOsdCmd.X1 + RectOsdCmd.Thickness) >= pDrawInfo->OsdReg.W) {
                RectOsdCmd.X1 = pDrawInfo->OsdReg.W - RectOsdCmd.Thickness;
            }
            if ((RectOsdCmd.X2 + RectOsdCmd.Thickness) >= pDrawInfo->OsdReg.W) {
                RectOsdCmd.X2 = pDrawInfo->OsdReg.W - RectOsdCmd.Thickness;
            }

            if ((RectOsdCmd.Y1 + RectOsdCmd.Thickness) >= pDrawInfo->OsdReg.H) {
                RectOsdCmd.Y1 = pDrawInfo->OsdReg.H - RectOsdCmd.Thickness;
            }
            if ((RectOsdCmd.Y2 + RectOsdCmd.Thickness) >= pDrawInfo->OsdReg.H) {
                RectOsdCmd.Y2 = pDrawInfo->OsdReg.H - RectOsdCmd.Thickness;
            }

            /* add offset for the region */
            RectOsdCmd.X1 += pDrawInfo->OsdReg.X;
            RectOsdCmd.Y1 += pDrawInfo->OsdReg.Y;
            RectOsdCmd.X2 += pDrawInfo->OsdReg.X;
            RectOsdCmd.Y2 += pDrawInfo->OsdReg.Y;
            RectOsdCmd.Color = ODDrawTask_OsdColorMap(ODColorMap, (UINT32) pList->Bbx[i].Cat);
            RectOsdCmd.WarningLevel = 1U;

            CamMsg.Msg = SVC_CV_OSD_DRAW_RECT;
            CamMsg.pCtx = &RectOsdCmd;
            ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);

            if (SVC_OK != SvcOsd_GetOsdBufSize(pDrawInfo->OsdReg.Index, &OsdWidth, &OsdHeight)) {
                SvcLog_NG(SVC_LOG_OD_TASK, "SvcOsd_GetOsdBufSize failed", 0U, 0U);
            }

            /* put category msg */
            if (ODLabelPos == 1U) {
                if (pList->Bbx[i].Cat < 255U) {
                    StrOsdCmd.Channel = pDrawInfo->OsdReg.Index;

                    /* swap the position of x1 and x2 for flip case */
                    if (pDrawInfo->OsdRegFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP){
                        StrOsdCmd.X1 = RectOsdCmd.X2;
                    } else {
                        StrOsdCmd.X1 = RectOsdCmd.X1;
                    }

                    /* avoid string over OSD boundary */
                    //StrOsdCmd.X1 = ((OsdWidth - RectOsdCmd.X1) > 50U) ? RectOsdCmd.X1 : (RectOsdCmd.X1 - 50U);
                    StrOsdCmd.Y1 = (RectOsdCmd.Y1 < 20U) ? (RectOsdCmd.Y1 + 10U) : (RectOsdCmd.Y1 - 10U);
                    //StrOsdCmd.Color = RectOsdCmd.Color;
                    StrOsdCmd.Color = 28U;

                    #if 0
                    if (SVC_OK != AmbaUtility_DoubleToStr(Osd_String, 32U, pSRData->SRObject[i].RealSpeed, 0U)) {
                        // SvcLog_NG(SVC_LOG_OD_TASK, "AmbaUtility_DoubleToStr Osd_String failed", 0U, 0U);
                    }
                    SvcWrap_strcpy(StrOsdCmd.String, sizeof(StrOsdCmd.String), Osd_String);
                    AmbaUtility_StringAppend(StrOsdCmd.String, sizeof(StrOsdCmd.String), "km/");
                    #endif

                    if(-(pSRData->SRObject[i].Object.Model.Vertices[AMBA_3D_MD_CUBOID_VERTICE_CBL].Y/1000.0) <= 80.0){
                        if(-(pSRData->SRObject[i].Object.Model.Vertices[AMBA_3D_MD_CUBOID_VERTICE_CBL].Y/1000.0) >= 0.0){
                            if (SVC_OK != AmbaUtility_DoubleToStr(Osd_String, sizeof(Osd_String), -(pSRData->SRObject[i].Object.Model.Vertices[AMBA_3D_MD_CUBOID_VERTICE_CBL].Y/1000.0), 1U)) {
                                // SvcLog_NG(SVC_LOG_OD_TASK, "AmbaUtility_DoubleToStr Osd_String failed", 0U, 0U);
                            }
                            SvcWrap_strcpy(StrOsdCmd.String, sizeof(StrOsdCmd.String), Osd_String);
                            AmbaUtility_StringAppend(StrOsdCmd.String, sizeof(StrOsdCmd.String), "m");
                            CamMsg.Msg = SVC_CV_OSD_DRAWSTRING;
                            CamMsg.pCtx = &StrOsdCmd;
                            ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);
                        }
                    }
                }
            }
        }
    }
}
#endif

#if defined(CONFIG_ICAM_IMGCAL_STITCH_USED)
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ODDrawTask_EmaDetResDrawBbx
 *
 *  @Description:: Draw BBX
 *
 *  @Input      ::
 *                 CvFlowChan: CvFlow channel
 *                 pList: The list contains the bounding box information
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 None
\*-----------------------------------------------------------------------------------------------*/
static void ODDrawTask_EmaDetResDrawBbx(UINT32 CvFlowChan, const SVC_CV_DETRES_BBX_LIST_s *pList, AMBA_SR_SROBJECT_DATA_s *pSRData)
{
    #define SVC_EMA_FILTER_WIDTH  (36U)
    static UINT32 ODLabelPos = 1U;

    const SVC_RES_CFG_s  *pCfg = SvcResCfg_Get();
    UINT32 i;
    SVC_CV_DISP_OSD_CMD_s   RectOsdCmd;
    SVC_CV_DISP_OSD_STR_s   StrOsdCmd;
    SVC_CV_CAMCTRL_MSG_s    CamMsg;
    RECT_POS_s              BbxPos;
    const OD_DRAW_INFO_s   *pDrawInfo = g_ODDrawCtrl[CvFlowChan].pDrawInfo;
    UINT32 OsdWidth, OsdHeight;
    char Osd_String[12];
    UINT32 Arg[1];

    AmbaMisra_TouchUnused(pSRData);

    if (pDrawInfo != NULL) {
        for (i = 0; i < pList->BbxAmount; i++) {
            /* Rebase BBX from main window to VOUT src window */
            BbxPos.X1 = pList->Bbx[i].X;
            BbxPos.Y1 = pList->Bbx[i].Y;
            BbxPos.X2 = (UINT32)pList->Bbx[i].X + (UINT32)pList->Bbx[i].W - 1U;
            BbxPos.Y2 = (UINT32)pList->Bbx[i].Y + (UINT32)pList->Bbx[i].H - 1U;
            if (pDrawInfo->PrevCrop != 0U) {
                if (0U == ODDrawTask_DetResRebaseObj(&BbxPos, &pDrawInfo->PrevSrc)) {
                    continue;
                }
            }

            /* put rectangle msg */
            RectOsdCmd.Channel = pDrawInfo->OsdReg.Index;
            RectOsdCmd.X1 = (BbxPos.X1 * pDrawInfo->OsdReg.W) / pDrawInfo->PrevSrc.W;
            RectOsdCmd.Y1 = (BbxPos.Y1 * pDrawInfo->OsdReg.H) / pDrawInfo->PrevSrc.H;
            RectOsdCmd.X2 = (BbxPos.X2 * pDrawInfo->OsdReg.W) / pDrawInfo->PrevSrc.W;
            RectOsdCmd.Y2 = (BbxPos.Y2 * pDrawInfo->OsdReg.H) / pDrawInfo->PrevSrc.H;

            if (pDrawInfo->OsdRegFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP) {
                RectOsdCmd.X1 = pDrawInfo->OsdReg.W - RectOsdCmd.X1;
                RectOsdCmd.X2 = pDrawInfo->OsdReg.W - RectOsdCmd.X2;
            }

            /* avoid draw over canvas boundary */
            RectOsdCmd.Thickness = ODRectThickness;
            if ((RectOsdCmd.X1 + RectOsdCmd.Thickness) >= pDrawInfo->OsdReg.W) {
                RectOsdCmd.X1 = pDrawInfo->OsdReg.W - RectOsdCmd.Thickness;
            }
            if ((RectOsdCmd.X2 + RectOsdCmd.Thickness) >= pDrawInfo->OsdReg.W) {
                RectOsdCmd.X2 = pDrawInfo->OsdReg.W - RectOsdCmd.Thickness;
            }

            if ((RectOsdCmd.Y1 + RectOsdCmd.Thickness) >= pDrawInfo->OsdReg.H) {
                RectOsdCmd.Y1 = pDrawInfo->OsdReg.H - RectOsdCmd.Thickness;
            }
            if ((RectOsdCmd.Y2 + RectOsdCmd.Thickness) >= pDrawInfo->OsdReg.H) {
                RectOsdCmd.Y2 = pDrawInfo->OsdReg.H - RectOsdCmd.Thickness;
            }

            /* add offset for the region */
            RectOsdCmd.X1 += pDrawInfo->OsdReg.X;
            RectOsdCmd.Y1 += pDrawInfo->OsdReg.Y;
            RectOsdCmd.X2 += pDrawInfo->OsdReg.X;
            RectOsdCmd.Y2 += pDrawInfo->OsdReg.Y;
            RectOsdCmd.Color = ODDrawTask_OsdColorMap(ODColorMap, (UINT32) pList->Bbx[i].Cat);
            RectOsdCmd.WarningLevel = 0U;

            /* abandon bbx which is vehicle with length smaller than certain pixels */
            if ((pCfg->UserFlag & SVC_EMR_STITCH) > 0U) {
                if (0 == SvcWrap_strcmp("Vehicle", pList->class_name[pList->Bbx[i].Cat])) {
                    if (RectOsdCmd.X2 > RectOsdCmd.X1) {
                        if ((RectOsdCmd.X2 - RectOsdCmd.X1) < SVC_EMA_FILTER_WIDTH) {
                            continue;
                        }
                    } else {
                        if ((RectOsdCmd.X1 - RectOsdCmd.X2) < SVC_EMA_FILTER_WIDTH) {
                            continue;
                        }
                    }
                }
            }

            CamMsg.Msg = SVC_CV_OSD_DRAW_RECT;
            CamMsg.pCtx = &RectOsdCmd;
            ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);

            if (SVC_OK != SvcOsd_GetOsdBufSize(pDrawInfo->OsdReg.Index, &OsdWidth, &OsdHeight)) {
                SvcLog_NG(SVC_LOG_OD_TASK, "SvcOsd_GetOsdBufSize failed", 0U, 0U);
            }

            /* put category msg */
            if (ODLabelPos == 1U) {
                if (pList->Bbx[i].Cat < 255U) {
                    DOUBLE CarDist;
                    StrOsdCmd.Channel = pDrawInfo->OsdReg.Index;

                    /* swap the position of x1 and x2 for flip case */
                    if (pDrawInfo->OsdRegFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP){
                        StrOsdCmd.X1 = RectOsdCmd.X2;
                    } else {
                        StrOsdCmd.X1 = RectOsdCmd.X1;
                    }

                    /* avoid string over OSD boundary */
                    //StrOsdCmd.X1 = ((OsdWidth - RectOsdCmd.X1) > 50U) ? RectOsdCmd.X1 : (RectOsdCmd.X1 - 50U);
                    StrOsdCmd.Y1 = (RectOsdCmd.Y1 < 20U) ? (RectOsdCmd.Y1 + 10U) : (RectOsdCmd.Y1 - 10U);
                    //StrOsdCmd.Color = RectOsdCmd.Color;
                    StrOsdCmd.Color = 28U;

                    CarDist = -((pSRData->SRObject[i].Object.Model.Vertices[AMBA_3D_MD_CUBOID_VERTICE_CBL].Y + pSRData->SRObject[i].Object.Model.Vertices[AMBA_3D_MD_CUBOID_VERTICE_CBR].Y)/2.0);

                    if (CarDist > 0.0) {
                        Arg[0] = ((UINT32) (CarDist)) / 1000U;
                        if (SVC_OK != AmbaUtility_StringPrintUInt32(Osd_String, sizeof(StrOsdCmd.String), "%d", 1U, Arg)) {
                            // SvcLog_NG(SVC_LOG_OD_TASK, "AmbaUtility_StringPrintUInt32 Osd_String failed", 0U, 0U);
                        }
                        SvcWrap_strcpy(StrOsdCmd.String, sizeof(StrOsdCmd.String), Osd_String);
                        AmbaUtility_StringAppend(StrOsdCmd.String, sizeof(StrOsdCmd.String), "m");
                        CamMsg.Msg = SVC_CV_OSD_DRAWSTRING;
                        CamMsg.pCtx = &StrOsdCmd;
                        ODDrawTask_DrawDetectionResult(CvFlowChan, &CamMsg);
                    }
                }
            }
        }
    }
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ODDrawTask_DetResRebaseObj
 *
 *  @Description:: Rebase object offset and window to new coordinate and filter out objest which is out of boundary.
 *
 *  @Input      ::
 *                 pObjPos: Object geometry
 *                 pNewBase: New base
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 UINT32: 0 - Invalid (No intersection). 1 - Valid (With intersection).
\*-----------------------------------------------------------------------------------------------*/
static UINT32 ODDrawTask_DetResRebaseObj(RECT_POS_s *pObjPos, const REGION_s *pNewBase)
{
    INT32 X1, Y1, X2, Y2;
    UINT32 Intersection;

    X1 = (INT32)pObjPos->X1 - (INT32)pNewBase->X;
    Y1 = (INT32)pObjPos->Y1 - (INT32)pNewBase->Y;
    X2 = (INT32)pObjPos->X2 - (INT32)pNewBase->X;
    Y2 = (INT32)pObjPos->Y2 - (INT32)pNewBase->Y;

    if (((X1 < 0) && (X2 < 0)) || ((X1 >= (INT32)pNewBase->W) && (X2 >= (INT32)pNewBase->W))) {
        Intersection = 0;
    } else {
        Intersection = 1;
        if (X1 < 0) {
            X1 = 0;
        }
        if (X2 >= (INT32)pNewBase->W) {
            X2 = (INT32)pNewBase->W - 1;
        }
    }

    if (Intersection != 0U) {
        if (((Y1 < 0) && (Y2 < 0)) || ((Y1 >= (INT32)pNewBase->H) && (Y2 >= (INT32)pNewBase->H))) {
            Intersection = 0;
        } else {
            Intersection = 1;
            if (Y1 < 0) {
                Y1 = 0;
            }
            if (Y2 >= (INT32)pNewBase->H) {
                Y2 = (INT32)pNewBase->H - 1;
            }
        }

        if ((X1 == X2) || (Y1 == Y2)) {
            Intersection = 0;
        } else {
            pObjPos->X1 = (UINT32)X1;
            pObjPos->Y1 = (UINT32)Y1;
            pObjPos->X2 = (UINT32)X2;
            pObjPos->Y2 = (UINT32)Y2;
        }
    }

    return Intersection;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ODDrawTask_DrawDetectionResult
 *
 *  @Description:: Draw BBX
 *
 *  @Input      ::
 *                 CvFlowChan: CvFlow channel
 *                 pCamMsg: The pointer to the message containing the bounding box information
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 None
\*-----------------------------------------------------------------------------------------------*/
static void ODDrawTask_DrawDetectionResult(UINT32 CvFlowChan, const SVC_CV_CAMCTRL_MSG_s *pCamMsg)
{
    switch(pCamMsg->Msg) {
    case SVC_CV_OSD_DRAW_3D:
        {
            const SVC_CV_DISP_3D_CMD_s *pMsgInfo;
            AmbaMisra_TypeCast(&pMsgInfo, &pCamMsg->pCtx);
            /* Put 3D bbx info to be drawed into buffer */
            ODDrawTask_Osd3DMsgPut(CvFlowChan, pMsgInfo);
        }
        break;
    case SVC_CV_OSD_DRAW_RECT:
        {
            const SVC_CV_DISP_OSD_CMD_s *pMsgInfo;
            AmbaMisra_TypeCast(&pMsgInfo, &pCamMsg->pCtx);
            /* Put rect to be drawed into buffer */
            ODDrawTask_OsdRectMsgPut(CvFlowChan, pMsgInfo);
        }
        break;
    case SVC_CV_OSD_FLUSH:
    case SVC_CV_OSD_FLUSH_CAPSEQ:
        {
            const SVC_CV_DISP_OSD_CTRL_s *pMsgInfo;
            AmbaMisra_TypeCast(&pMsgInfo, &pCamMsg->pCtx);
            ODDrawTask_OsdFlush(pMsgInfo->Channel, 0);
        }
        break;
    case SVC_CV_OSD_CLEAR:
        {
            const SVC_CV_DISP_OSD_CTRL_s *pMsgInfo;
            AmbaMisra_TypeCast(&pMsgInfo, &pCamMsg->pCtx);
        }
        break;
    case SVC_CV_OSD_DRAWSTRING:
        {
            const SVC_CV_DISP_OSD_STR_s *pMsgInfo;
            AmbaMisra_TypeCast(&pMsgInfo, &pCamMsg->pCtx);
            ODDrawTask_OsdStrMsgPut(CvFlowChan, pMsgInfo);
            //AmbaPrint_PrintStr5("SVC_CV_OSD_DRAWSTRING:%s", pMsgInfo->String, NULL, NULL, NULL, NULL);
        }
        break;
    case SVC_CV_OSD_SET_EXTOSDCLUT:
        {
            const SVC_CV_DISP_EXTOSD_CTRL_s *pMsgInfo;
            UINT32 Channel;
            ULONG  CLUTAddr;
            const UINT32 *pU32;

            AmbaMisra_TypeCast(&pMsgInfo, &pCamMsg->pCtx);
            Channel = pMsgInfo->Channel;
            CLUTAddr = pMsgInfo->Param[0];
            AmbaMisra_TypeCast(&pU32, &CLUTAddr);
            if (SVC_OK != SvcOsd_SetExtCLUT(Channel, pU32)) {
                SvcLog_NG(SVC_LOG_OD_TASK, "SvcOsd_SetExtCLUT failed", 0U, 0U);
            }
        }
        break;
    case SVC_CV_OSD_SET_EXTOSDBUFINFO:
        {
            const SVC_CV_DISP_EXTOSD_CTRL_s *pMsgInfo;
            UINT32 Channel;
            SvcOSD_ExtBuffer_CFG_s BufConfig;

            AmbaMisra_TypeCast(&pMsgInfo, &pCamMsg->pCtx);
            Channel = pMsgInfo->Channel;
            BufConfig.PixelFormat = (UINT8)pMsgInfo->Param[0];
            BufConfig.BufferPitch = pMsgInfo->Param[1];
            BufConfig.BufferWidth = pMsgInfo->Param[2];
            BufConfig.BufferHeight = pMsgInfo->Param[3];
            BufConfig.BufferSize = pMsgInfo->Param[4];
            BufConfig.WindowOffsetX = (UINT16)pMsgInfo->Param[5];
            BufConfig.WindowOffsetY = (UINT16)pMsgInfo->Param[6];
            BufConfig.WindowWidth = (UINT16)pMsgInfo->Param[7];
            BufConfig.WindowHeight = (UINT16)pMsgInfo->Param[8];
            BufConfig.Interlace = (UINT8)pMsgInfo->Param[9];

            if (SVC_OK != SvcOsd_SetExtBufInfo(Channel, &BufConfig)) {
                SvcLog_NG(SVC_LOG_OD_TASK, "SvcOsd_SetExtBufInfo failed", 0U, 0U);
            }
        }
        break;
    case SVC_CV_OSD_UPDATE_EXTBUF:
    case SVC_CV_OSD_UPDATE_EXTBUF_SYNC:
        {
            const SVC_CV_DISP_EXTOSD_CTRL_s *pMsgInfo;
            UINT32 Channel;
            ULONG  OSDBufAddr;
            const UINT8 *pU8 = NULL;

            AmbaMisra_TypeCast(&pMsgInfo, &pCamMsg->pCtx);
            Channel = pMsgInfo->Channel;
            OSDBufAddr = pMsgInfo->Param[0];
            AmbaMisra_TypeCast(&pU8, &OSDBufAddr);
            if (SVC_OK != SvcOsd_UpdateExtBuf(Channel,pU8)) {
                SvcLog_NG(SVC_LOG_OD_TASK, "SvcOsd_UpdateExtBuf failed", 0U, 0U);
            }
        }
        break;
    case SVC_CV_OSD_RELEASE_EXTOSD:
        {
            const SVC_CV_DISP_EXTOSD_CTRL_s *pMsgInfo;
            UINT32 Channel;

            AmbaMisra_TypeCast(&pMsgInfo, &pCamMsg->pCtx);
            Channel = pMsgInfo->Channel;
            if (SVC_OK != SvcOsd_ReleaseExtOSD(Channel)) {
                SvcLog_NG(SVC_LOG_OD_TASK, "SvcOsd_ReleaseExtOSD failed", 0U, 0U);
            }
        }
        break;
    case SVC_CV_OSD_DRAW_LINE:
        {
            const SVC_CV_DISP_OSD_LINE_s *pMsgInfo;
            AmbaMisra_TypeCast(&pMsgInfo, &pCamMsg->pCtx);
            ODDrawTask_OsdLineMsgPut(CvFlowChan, pMsgInfo);
        }
        break;
    case SVC_CV_OSD_BITMAP:
        {
            const SVC_CV_DISP_OSD_BITMAP_s *pMsgInfo;
            AmbaMisra_TypeCast(&pMsgInfo, &pCamMsg->pCtx);
            ODDrawTask_OsdBitmapMsgPut(CvFlowChan, pMsgInfo);
        }
        break;
    default:
        AmbaMisra_TouchUnused(pCamMsg->pCtx);
        break;
    }
}

#if defined(CONFIG_ICAM_PROJECT_EMIRROR) && defined(CONFIG_BUILD_AMBA_ADAS)
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ODDrawTask_BsdDraw
 *
 *  @Description:: Draw BSD result
 *
 *  @Input      ::
 *                 *pBsdDrawInfo: The pointer to the BSD result message for drawing bonding box
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 None
\*-----------------------------------------------------------------------------------------------*/
static void ODDrawTask_BsdDraw(SVC_BSD_DRAW_INFO_s *pBsdDrawInfo)
{
    SVC_CV_DETRES_BBX_LIST_s SvcBbxList;
    const SVC_CV_DETRES_BBX_LIST_s *pList = &SvcBbxList;
    UINT32 CvFlowChan;

    AmbaMisra_TouchUnused(pBsdDrawInfo);

    {  /* Convert BSD structure to SVC structure */
        SvcBbxList.MsgCode     = pBsdDrawInfo->pBbx->MsgCode;
        SvcBbxList.CaptureTime = pBsdDrawInfo->pBbx->CaptureTime;
        SvcBbxList.FrameNum    = pBsdDrawInfo->pBbx->FrameNum;
        SvcBbxList.BbxAmount   = pBsdDrawInfo->pBbx->NumBbx;
        SvcBbxList.class_name  = pBsdDrawInfo->Class_name;
        if (SVC_OK != AmbaWrap_memcpy(SvcBbxList.Bbx, pBsdDrawInfo->pBbx->Bbx, SvcBbxList.BbxAmount * sizeof(SVC_CV_DETRES_BBX_WIN_s))) {
            //SvcLog_NG(SVC_LOG_OD_TASK, "AmbaWrap_memcpy SvcBbxList.Bbx failed", 0U, 0U);
        }
    }

    CvFlowChan = pBsdDrawInfo->Chan;
    if (CvFlowChan < SVC_CV_FLOW_CHAN_MAX) {
        ODDrawTask_DetResFovInit(CvFlowChan);

        ODDrawTask_DetResUpdateDrawInfo(CvFlowChan);

        ODDrawTask_BsdDetResBbxOsdProc(CvFlowChan, pList, pBsdDrawInfo->pSRData);

        ODDrawTask_PrintDetInfo(CvFlowChan, pList);

        ODStarted = 1;
    } else {
        SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_BsdDraw invalid CvFlowChan", 0U, 0U);
    }

}

static void ODDrawTask_RmgDraw(SVC_RMG_DRAW_INFO_s *pRmgDrawInfo)
{
    SVC_CV_DETRES_BBX_LIST_s SvcBbxList;
    SVC_CV_DETRES_BBX_LIST_s *pList = &SvcBbxList;
    UINT32 CvFlowChan;

    AmbaMisra_TouchUnused(pRmgDrawInfo);

    {  /* Convert RMG structure to SVC structure */
        SvcBbxList.MsgCode     = pRmgDrawInfo->pBbx->MsgCode;
        SvcBbxList.CaptureTime = pRmgDrawInfo->pBbx->CaptureTime;
        SvcBbxList.FrameNum    = pRmgDrawInfo->pBbx->FrameNum;
        SvcBbxList.BbxAmount   = pRmgDrawInfo->pBbx->NumBbx;
        SvcBbxList.class_name  = pRmgDrawInfo->Class_name;
        if (SVC_OK != AmbaWrap_memcpy(SvcBbxList.Bbx, pRmgDrawInfo->pBbx->Bbx, SvcBbxList.BbxAmount * sizeof(SVC_CV_DETRES_BBX_WIN_s))) {
            // SvcLog_NG(SVC_LOG_OD_TASK, "AmbaWrap_memcpy SvcBbxList.Bbx failed", 0U, 0U);
        }
    }

    CvFlowChan = pRmgDrawInfo->Chan;
    if (CvFlowChan < SVC_CV_FLOW_CHAN_MAX) {
        ODDrawTask_DetResFovInit(CvFlowChan);

        ODDrawTask_DetResUpdateDrawInfo(CvFlowChan);

        if (ODDrawRoi == 1U) {
            ODDrawTask_DetResUpdateRoiInfo(CvFlowChan);
        }

        ODDrawTask_RmgDetResBbxOsdProc(CvFlowChan, pList, pRmgDrawInfo->pSRData);

        ODDrawTask_PrintDetInfo(CvFlowChan, pList);

        ODStarted = 1;
    } else {
        SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_RmgDraw invalid CvFlowChan", 0U, 0U);
    }

}
#endif

#if defined(CONFIG_ICAM_IMGCAL_STITCH_USED)
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ODDrawTask_EmaDraw
 *
 *  @Description:: Draw EMA result
 *
 *  @Input      ::
 *                 *pEmaDrawInfo: The pointer to the EMA result message for drawing bonding box
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 None
\*-----------------------------------------------------------------------------------------------*/
static void ODDrawTask_EmaDraw(SVC_EMA_DRAW_INFO_s *pEmaDrawInfo)
{
    SVC_CV_DETRES_BBX_LIST_s SvcBbxList;
    const SVC_CV_DETRES_BBX_LIST_s *pList = &SvcBbxList;
    UINT32 CvFlowChan;

    AmbaMisra_TouchUnused(pEmaDrawInfo);

    {  /* Convert EMA structure to SVC structure */
        SvcBbxList.MsgCode     = pEmaDrawInfo->pBbx->MsgCode;
        SvcBbxList.CaptureTime = pEmaDrawInfo->pBbx->CaptureTime;
        SvcBbxList.FrameNum    = pEmaDrawInfo->pBbx->FrameNum;
        SvcBbxList.BbxAmount   = pEmaDrawInfo->pBbx->NumBbx;
        SvcBbxList.class_name  = pEmaDrawInfo->Class_name;
        if (SVC_OK != AmbaWrap_memcpy(SvcBbxList.Bbx, pEmaDrawInfo->pBbx->Bbx, SvcBbxList.BbxAmount * sizeof(SVC_CV_DETRES_BBX_WIN_s))) {
            // SvcLog_NG(SVC_LOG_OD_TASK, "AmbaWrap_memcpy SvcBbxList.Bbx failed", 0U, 0U);
        }
    }

    CvFlowChan = pEmaDrawInfo->Chan;
    if (CvFlowChan < SVC_CV_FLOW_CHAN_MAX) {
        ODDrawTask_DetResFovInit(CvFlowChan);

        ODDrawTask_DetResUpdateDrawInfo(CvFlowChan);

        ODDrawTask_EmaDetResBbxOsdProc(CvFlowChan, pList, pEmaDrawInfo->pSRData);

        ODDrawTask_PrintDetInfo(CvFlowChan, pList);

        ODStarted = 1;
    } else {
        SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_EmaDraw invalid CvFlowChan", 0U, 0U);
    }

}
#endif

static void Draw3DbbxResult(const SVC_CV_DISP_3D_CMD_s  *p3DMsgInfo)
{
    UINT32 RetVal = SVC_NG;

#ifdef CONFIG_SVC_LVGL_USED
    UINT32 NuberOfPoint = 0U;
    FC_3D_POINT_s Point[12U]; /* Each 3dbbx max = 16 lines*/
    /*
      Draw 3D bbx front step_1:
         ____
        |    |
        |____|
    */
      NuberOfPoint = 0U;
      Point[NuberOfPoint].x = (INT16)p3DMsgInfo->FTL_x;
      Point[NuberOfPoint].y = (INT16)p3DMsgInfo->FTL_y;
      NuberOfPoint++;

      Point[NuberOfPoint].x = (INT16)p3DMsgInfo->FBL_x;
      Point[NuberOfPoint].y = (INT16)p3DMsgInfo->FBL_y;
      NuberOfPoint++;

      Point[NuberOfPoint].x = (INT16)p3DMsgInfo->FBR_x;
      Point[NuberOfPoint].y = (INT16)p3DMsgInfo->FBR_y;
      NuberOfPoint++;

      Point[NuberOfPoint].x = (INT16)p3DMsgInfo->FTR_x;
      Point[NuberOfPoint].y = (INT16)p3DMsgInfo->FTR_y;
      NuberOfPoint++;

      Point[NuberOfPoint].x = (INT16)p3DMsgInfo->FTL_x;
      Point[NuberOfPoint].y = (INT16)p3DMsgInfo->FTL_y;
      NuberOfPoint++;

      RetVal = SvcOsd_DrawMultiplePointsLine(p3DMsgInfo->Channel, Point, NuberOfPoint, p3DMsgInfo->Thickness, p3DMsgInfo->Color);
      if (SVC_OK != RetVal) {
          SvcLog_NG(SVC_LOG_OD_TASK, "[LVGL]Draw 3D bbx step_1 failed %d", RetVal, 0U);
      }

    /*
      Draw 3D bbx step_2:
           ___
          |   |
         _|___|
        |   |
        |___|
    */
      NuberOfPoint = 0U;
      Point[NuberOfPoint].x = (INT16)p3DMsgInfo->RTL_x;
      Point[NuberOfPoint].y = (INT16)p3DMsgInfo->RTL_y;
      NuberOfPoint++;

      Point[NuberOfPoint].x = (INT16)p3DMsgInfo->RBL_x;
      Point[NuberOfPoint].y = (INT16)p3DMsgInfo->RBL_y;
      NuberOfPoint++;

      Point[NuberOfPoint].x = (INT16)p3DMsgInfo->RBR_x;
      Point[NuberOfPoint].y = (INT16)p3DMsgInfo->RBR_y;
      NuberOfPoint++;

      Point[NuberOfPoint].x = (INT16)p3DMsgInfo->RTR_x;
      Point[NuberOfPoint].y = (INT16)p3DMsgInfo->RTR_y;
      NuberOfPoint++;

      Point[NuberOfPoint].x = (INT16)p3DMsgInfo->RTL_x;
      Point[NuberOfPoint].y = (INT16)p3DMsgInfo->RTL_y;
      NuberOfPoint++;

      RetVal = SvcOsd_DrawMultiplePointsLine(p3DMsgInfo->Channel, Point, NuberOfPoint, p3DMsgInfo->Thickness, p3DMsgInfo->Color);
      if (SVC_OK != RetVal) {
          SvcLog_NG(SVC_LOG_OD_TASK, "[LVGL]Draw 3D bbx step_2 failed %d", RetVal, 0U);
      }
#else
      /*
        Draw 3D bbx front step_1:
           ____
          |    |
          |____|
      */
      RetVal = SvcOsd_DrawLine(p3DMsgInfo->Channel,
                                  p3DMsgInfo->FTL_x,
                                  p3DMsgInfo->FTL_y,
                                  p3DMsgInfo->FBL_x,
                                  p3DMsgInfo->FBL_y,
                                  p3DMsgInfo->Thickness,
                                  p3DMsgInfo->Color);

      RetVal |= SvcOsd_DrawLine(p3DMsgInfo->Channel,
                                  p3DMsgInfo->FBL_x,
                                  p3DMsgInfo->FBL_y,
                                  p3DMsgInfo->FBR_x,
                                  p3DMsgInfo->FBR_y,
                                  p3DMsgInfo->Thickness,
                                  p3DMsgInfo->Color);

      RetVal |= SvcOsd_DrawLine(p3DMsgInfo->Channel,
                                  p3DMsgInfo->FBR_x,
                                  p3DMsgInfo->FBR_y,
                                  p3DMsgInfo->FTR_x,
                                  p3DMsgInfo->FTR_y,
                                  p3DMsgInfo->Thickness,
                                  p3DMsgInfo->Color);

      RetVal |= SvcOsd_DrawLine(p3DMsgInfo->Channel,
                                  p3DMsgInfo->FTR_x,
                                  p3DMsgInfo->FTR_y,
                                  p3DMsgInfo->FTL_x,
                                  p3DMsgInfo->FTL_y,
                                  p3DMsgInfo->Thickness,
                                  p3DMsgInfo->Color);

      if (SVC_OK != RetVal) {
          SvcLog_NG(SVC_LOG_OD_TASK, "[Non-LVGL]Draw 3D bbx step_1 failed %d", RetVal, 0U);
      }


      /*
        Draw 3D bbx step_2:
             ___
            |   |
           _|___|
          |   |
          |___|
      */
      RetVal = SvcOsd_DrawLine(p3DMsgInfo->Channel,
                                  p3DMsgInfo->RTL_x,
                                  p3DMsgInfo->RTL_y,
                                  p3DMsgInfo->RBL_x,
                                  p3DMsgInfo->RBL_y,
                                  p3DMsgInfo->Thickness,
                                  p3DMsgInfo->Color);

      RetVal |= SvcOsd_DrawLine(p3DMsgInfo->Channel,
                                  p3DMsgInfo->RBL_x,
                                  p3DMsgInfo->RBL_y,
                                  p3DMsgInfo->RBR_x,
                                  p3DMsgInfo->RBR_y,
                                  p3DMsgInfo->Thickness,
                                  p3DMsgInfo->Color);

      RetVal |= SvcOsd_DrawLine(p3DMsgInfo->Channel,
                                  p3DMsgInfo->RBR_x,
                                  p3DMsgInfo->RBR_y,
                                  p3DMsgInfo->RTR_x,
                                  p3DMsgInfo->RTR_y,
                                  p3DMsgInfo->Thickness,
                                  p3DMsgInfo->Color);

      RetVal |= SvcOsd_DrawLine(p3DMsgInfo->Channel,
                                  p3DMsgInfo->RTR_x,
                                  p3DMsgInfo->RTR_y,
                                  p3DMsgInfo->RTL_x,
                                  p3DMsgInfo->RTL_y,
                                  p3DMsgInfo->Thickness,
                                  p3DMsgInfo->Color);

      if (SVC_OK != RetVal) {
          SvcLog_NG(SVC_LOG_OD_TASK, "[Non-LVGL]Draw 3D bbx step_2 failed %d", RetVal, 0U);
      }
#endif

      /*
      Draw 3D bbx step_3 ~ Step_6
        _____         _____          _____          _____
       /|    |       /|   /|        /|   /|        /|   /|
      /_|__ _|      /_|__/_|       /_|__/_|       /_|__/_|
      |    |        |    |         | /  |         | /  | /
      |____|        |____|         |/___|         |/___|/


    */
      RetVal = SvcOsd_DrawLine(p3DMsgInfo->Channel,
                                  p3DMsgInfo->FTL_x,
                                  p3DMsgInfo->FTL_y,
                                  p3DMsgInfo->RTL_x,
                                  p3DMsgInfo->RTL_y,
                                  p3DMsgInfo->Thickness,
                                  p3DMsgInfo->Color);
      if (SVC_OK != RetVal) {
          SvcLog_NG(SVC_LOG_OD_TASK, "Draw 3D bbx step_3 failed %d", RetVal, 0U);
      }

      RetVal = SvcOsd_DrawLine(p3DMsgInfo->Channel,
                                  p3DMsgInfo->FTR_x,
                                  p3DMsgInfo->FTR_y,
                                  p3DMsgInfo->RTR_x,
                                  p3DMsgInfo->RTR_y,
                                  p3DMsgInfo->Thickness,
                                  p3DMsgInfo->Color);
      if (SVC_OK != RetVal) {
          SvcLog_NG(SVC_LOG_OD_TASK, "Draw 3D bbx step_4 failed %d", RetVal, 0U);
      }

      RetVal = SvcOsd_DrawLine(p3DMsgInfo->Channel,
                                  p3DMsgInfo->FBL_x,
                                  p3DMsgInfo->FBL_y,
                                  p3DMsgInfo->RBL_x,
                                  p3DMsgInfo->RBL_y,
                                  p3DMsgInfo->Thickness,
                                  p3DMsgInfo->Color);
      if (SVC_OK != RetVal) {
          SvcLog_NG(SVC_LOG_OD_TASK, "Draw 3D bbx step_5 failed %d", RetVal, 0U);
      }

      RetVal = SvcOsd_DrawLine(p3DMsgInfo->Channel,
                                  p3DMsgInfo->FBR_x,
                                  p3DMsgInfo->FBR_y,
                                  p3DMsgInfo->RBR_x,
                                  p3DMsgInfo->RBR_y,
                                  p3DMsgInfo->Thickness,
                                  p3DMsgInfo->Color);
      if (SVC_OK != RetVal) {
          SvcLog_NG(SVC_LOG_OD_TASK, "Draw 3D bbx step_6 failed %d", RetVal, 0U);
      }

      //Draw car direction if HaveDir == 1U
      if (1U == p3DMsgInfo->HaveDir) {
          RetVal = SVC_OK;
          if(p3DMsgInfo->FTL_x < p3DMsgInfo->FTR_x) {//Same direction car
              if ((p3DMsgInfo->RTR_x > p3DMsgInfo->FTR_x) &&
                  (p3DMsgInfo->FTL_x > p3DMsgInfo->RTL_x)){//In front of you
                  RetVal |= SvcOsd_DrawLine(p3DMsgInfo->Channel,
                                           p3DMsgInfo->RTR_x,
                                           p3DMsgInfo->RTR_y,
                                           p3DMsgInfo->FBR_x,
                                           p3DMsgInfo->FBR_y,
                                           p3DMsgInfo->Thickness,
                                           p3DMsgInfo->Color);

                  RetVal |= SvcOsd_DrawLine(p3DMsgInfo->Channel,
                                            p3DMsgInfo->RTL_x,
                                            p3DMsgInfo->RTL_y,
                                            p3DMsgInfo->FBL_x,
                                            p3DMsgInfo->FBL_y,
                                            p3DMsgInfo->Thickness,
                                            p3DMsgInfo->Color);

              } else if(p3DMsgInfo->RTR_x < p3DMsgInfo->FTR_x) {//Your left side
                  RetVal = SvcOsd_DrawLine(p3DMsgInfo->Channel,
                                           p3DMsgInfo->RTR_x,
                                           p3DMsgInfo->RTR_y,
                                           p3DMsgInfo->FBR_x,
                                           p3DMsgInfo->FBR_y,
                                           p3DMsgInfo->Thickness,
                                           p3DMsgInfo->Color);

              } else if (p3DMsgInfo->RTR_x > p3DMsgInfo->FTR_x) {//Your right side
                  RetVal = SvcOsd_DrawLine(p3DMsgInfo->Channel,
                                              p3DMsgInfo->RTL_x,
                                              p3DMsgInfo->RTL_y,
                                              p3DMsgInfo->FBL_x,
                                              p3DMsgInfo->FBL_y,
                                              p3DMsgInfo->Thickness,
                                              p3DMsgInfo->Color);
              } else {
                  //do nothing
              }
          } else {//Reverse direction
              if(p3DMsgInfo->RTR_x > p3DMsgInfo->FTR_x) {//Your left side
                  RetVal = SvcOsd_DrawLine(p3DMsgInfo->Channel,
                                           p3DMsgInfo->RTL_x,
                                           p3DMsgInfo->RTL_y,
                                           p3DMsgInfo->FBL_x,
                                           p3DMsgInfo->FBL_y,
                                           p3DMsgInfo->Thickness,
                                           p3DMsgInfo->Color);

              } else if (p3DMsgInfo->RTR_x < p3DMsgInfo->FTR_x) {//Your right side
                  RetVal = SvcOsd_DrawLine(p3DMsgInfo->Channel,
                                              p3DMsgInfo->RTR_x,
                                              p3DMsgInfo->RTR_y,
                                              p3DMsgInfo->FBR_x,
                                              p3DMsgInfo->FBR_y,
                                              p3DMsgInfo->Thickness,
                                              p3DMsgInfo->Color);
              } else {
                  //do nothing
              }
          }

          if (SVC_OK != RetVal) {
              SvcLog_NG(SVC_LOG_OD_TASK, "Draw car direction error %d", RetVal, 0U);
          }
      }
}

static void ST_DrawRec(const SVC_CV_DISP_OSD_CMD_s *pRectMsgInfo, UINT32 Color)
{
    UINT32 RetVal;
    UINT32 Width = (pRectMsgInfo->X2 - pRectMsgInfo->X1);
    UINT32 Thickness = (Width >= THICKNESS_RATIO) ? (Width/THICKNESS_RATIO):1U;

    RetVal = SvcOsd_DrawRect(pRectMsgInfo->Channel,
                             pRectMsgInfo->X1,
                             pRectMsgInfo->Y1,
                             pRectMsgInfo->X2,
                             pRectMsgInfo->Y2,
                             Color,
                             Thickness);
    AmbaMisra_TouchUnused(&RetVal);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ODDrawTask_OsdDraw
 *
 *  @Description:: Draw OSD
 *
 *  @Input      ::
 *                 Level: Which level to be shown. Differnt Fov use different level to show the result
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 None
\*-----------------------------------------------------------------------------------------------*/
static void ODDrawTask_OsdDraw(UINT32 VoutIdx, UINT32 Level)
{
    UINT32 RetVal = SVC_NG;
    UINT8 BufIdx;
    UINT16 RectRdIdx, StrRdIdx, LineRdIdx, BitmapRdIdx, RdIdx3D;
    UINT16 RectWrIdx, StrWrIdx, LineWrIdx, BitmapWrIdx, WrIdx3D;
    UINT16 i;
    SVC_CV_DISP_OSD_CMD_s *pRectMsgInfo;
    SVC_CV_DISP_OSD_STR_s *pStrMsgInfo;
    SVC_CV_DISP_OSD_LINE_s *pLineMsgInfo;
    SVC_CV_DISP_OSD_BITMAP_s *pBitmapMsgInfo;
    SVC_OSD_SW_RESCALE_CFG_s OsdRsCfg;
    SVC_CV_DISP_3D_CMD_s  *p3DMsgInfo;
    UINT32 CvFlowChan = 0U, Color;
    UINT8 RescaleMap[1] = {1};
    UINT32 ColorTmp1, ColorTmp2, SolidRectColor;

    AmbaMisra_TouchUnused(&VoutIdx);

    for (i = 0U; i < SVC_CV_FLOW_CHAN_MAX; i++) {
        if (g_ODDrawCtrl[i].pDrawInfo != NULL) {
            if (Level == g_ODDrawCtrl[i].pDrawInfo->GuiLevel) {
                CvFlowChan = i;
                RetVal = SVC_OK;
                break;
            }
        }
    }

    OD_DRAW_TASK_DBG("OsdDraw[%u] start. WrBufIdx(%u) RdBufIdx(%u)",
        Level, ODOsdBufCtrl[CvFlowChan].WrBufIdx, ODOsdBufCtrl[CvFlowChan].RdBufIdx, 0U, 0U);

    if (SVC_OK == RetVal) {
        RetVal = AmbaKAL_MutexTake(&ODMutex[CvFlowChan], AMBA_KAL_WAIT_FOREVER);
    } else {
        SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_OsdDraw failed %d, Unknown Level %d", RetVal, Level);
    }

    if (SVC_OK == RetVal) {
        if (ODOnOff == 1U) {
            BufIdx = ODOsdBufCtrl[CvFlowChan].RdBufIdx;

            /* Draw Bitmap queue */
            BitmapRdIdx  = ODOsdBufCtrl[CvFlowChan].BitmapRdIdx[BufIdx];
            BitmapWrIdx  = ODOsdBufCtrl[CvFlowChan].BitmapWrIdx[BufIdx];

            if (BitmapRdIdx != BitmapWrIdx) {
                OsdRsCfg.pHorizantalMap = RescaleMap;
                OsdRsCfg.HorizantalMapLen = 1U;
                OsdRsCfg.pVerticalMap = RescaleMap;
                OsdRsCfg.VerticalMapLen = 1U;
#ifdef CONFIG_ICAM_32BITS_OSD_USED
                OsdRsCfg.pClut = ODDrawBitmapClutBuf;
                OsdRsCfg.NumColor = SVC_OD_DRAW_BITMAP_CLUT_ENTRY_SIZE;
#else
                OsdRsCfg.pClut = NULL;
                OsdRsCfg.NumColor = 0U;
#endif

                for (i = BitmapRdIdx; i < BitmapWrIdx; i++) {
                    pBitmapMsgInfo = pODOsdBitmapMsgInfo[CvFlowChan][BufIdx];
                    pBitmapMsgInfo = &pBitmapMsgInfo[i];

                    AmbaMisra_TypeCast(&OsdRsCfg.pSrc, &pBitmapMsgInfo->pAddr);
                    OsdRsCfg.SrcWidth = pBitmapMsgInfo->SrcW;
                    OsdRsCfg.SrcHeight = pBitmapMsgInfo->SrcH;
                    OsdRsCfg.SrcPitch = pBitmapMsgInfo->SrcPitch;
                    OsdRsCfg.SrcStartY = 0U;
                    OsdRsCfg.pDst = NULL;
                    RetVal = SvcOsd_DrawBin_Rescale(pBitmapMsgInfo->Channel,
                                                    pBitmapMsgInfo->X,
                                                    pBitmapMsgInfo->Y,
                                                    pBitmapMsgInfo->X + pBitmapMsgInfo->W,
                                                    pBitmapMsgInfo->Y + pBitmapMsgInfo->H,
                                                    &OsdRsCfg);

                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_OsdDraw bitmap failed %d", RetVal, 0U);
                    }
                }
            }

            /* Draw 3D bbx queue */
            BufIdx = ODOsdBufCtrl[CvFlowChan].RdBufIdx;
            RdIdx3D  = ODOsdBufCtrl[CvFlowChan].RdIdx3D[BufIdx];
            WrIdx3D  = ODOsdBufCtrl[CvFlowChan].WrIdx3D[BufIdx];

            for (i = RdIdx3D; i < WrIdx3D; i++) {
                p3DMsgInfo = pODOsd3DMsgInfo[CvFlowChan][BufIdx];
                p3DMsgInfo = &p3DMsgInfo[i];
                Draw3DbbxResult(p3DMsgInfo);

            }

            /* Draw Rectangle queue */
            RectRdIdx  = ODOsdBufCtrl[CvFlowChan].RectRdIdx[BufIdx];
            RectWrIdx  = ODOsdBufCtrl[CvFlowChan].RectWrIdx[BufIdx];

            for (i = RectRdIdx; i < RectWrIdx; i++) {
                pRectMsgInfo = pODOsdRectMsgInfo[CvFlowChan][BufIdx];
                pRectMsgInfo = &pRectMsgInfo[i];
                SvcOsd_ColorIdxToColorSetting(pRectMsgInfo->Channel, pRectMsgInfo->Color, &Color);
                ColorTmp1 = Color &0x40000000U;
                ColorTmp2 = Color &0x00FFFFFFU;
                SolidRectColor = ColorTmp1 | ColorTmp2;

                if (pRectMsgInfo->Color <= 30U) { //No SolidRect of Traffic light/Traffic light/Road marker and Obstacle
                    RetVal = SvcOsd_DrawSolidRect(pRectMsgInfo->Channel,
                                                  pRectMsgInfo->X1,
                                                  pRectMsgInfo->Y1,
                                                  pRectMsgInfo->X2,
                                                  pRectMsgInfo->Y2,
                                                  SolidRectColor);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_OD_TASK, "SvcOsd_DrawSolidRect failed %d", RetVal, 0U);
                    }
                }

                ST_DrawRec(pRectMsgInfo, Color);

            }

            /* Draw String queue */
            StrRdIdx  = ODOsdBufCtrl[CvFlowChan].StrRdIdx[BufIdx];
            StrWrIdx  = ODOsdBufCtrl[CvFlowChan].StrWrIdx[BufIdx];

            for (i = StrRdIdx; i < StrWrIdx; i++) {
                pStrMsgInfo = pODOsdStrMsgInfo[CvFlowChan][BufIdx];
                pStrMsgInfo = &pStrMsgInfo[i];

                if (pStrMsgInfo->Color < 256U) { //256 color
                    SvcOsd_ColorIdxToColorSetting(pStrMsgInfo->Channel, pStrMsgInfo->Color, &Color);
                } else {
                    // Draw Depth is assigned in 32bit already instead of PSD map color lookup table,
                    //=> No need to go through SvcOsd_ColorIdxToColorSetting()
                    Color = pStrMsgInfo->Color;
                }

                RetVal = SvcOsd_DrawString(pStrMsgInfo->Channel,
                                            pStrMsgInfo->X1,
                                            pStrMsgInfo->Y1 - 5U,
                                            FontSize,
                                            Color,
                                            pStrMsgInfo->String);

                if (SVC_OK != RetVal) {
                    SvcLog_DBG(SVC_LOG_OD_TASK, "ODDrawTask_OsdDraw string failed %d", RetVal, 0U);
                }
            }

            /* Draw Line queue */
            LineRdIdx  = ODOsdBufCtrl[CvFlowChan].LineRdIdx[BufIdx];
            LineWrIdx  = ODOsdBufCtrl[CvFlowChan].LineWrIdx[BufIdx];

            for (i = LineRdIdx; i < LineWrIdx; i++) {
                pLineMsgInfo = pODOsdLineMsgInfo[CvFlowChan][BufIdx];
                pLineMsgInfo = &pLineMsgInfo[i];

                RetVal = SvcOsd_DrawLine(pLineMsgInfo->Channel,
                                            pLineMsgInfo->X1,
                                            pLineMsgInfo->Y1,
                                            pLineMsgInfo->X2,
                                            pLineMsgInfo->Y2,
                                            pLineMsgInfo->LineWidth,
                                            pLineMsgInfo->Color);

                if (SVC_OK != RetVal) {
                    SvcLog_DBG(SVC_LOG_OD_TASK, "ODDrawTask_OsdDraw line failed %d", RetVal, 0U);
                }
            }
        } else {
            /* Do nothing if ODOnOff == 0U */
        }
        RetVal = AmbaKAL_MutexGive(&ODMutex[CvFlowChan]);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_OsdDraw failed, Mutex Give failed %d", RetVal, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_OsdDraw failed, Mutex Take failed %d", RetVal, 0U);
    }

    OD_DRAW_TASK_DBG("OsdDraw[%u] end", Level, 0U, 0U, 0U, 0U);

}

static void ODDrawTask_OsdUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    OD_DRAW_TASK_DBG("OsdUpdate[%u] start. ODResultUpdate(%u)",
        Level, ODResultUpdate, 0U, 0U, 0U);

    if (ODResultUpdate != 0U) {
        *pUpdate = 1U;
        ODResultUpdate = 0U;
    } else {
        *pUpdate = 0U;
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ODDrawTask_Osd3DMsgPut
 *
 *  @Description:: Put 3D bbx messages
 *
 *  @Input      ::
 *                 CvFlowChan: CvFlow channel
 *                 pInputMsgInfo: The bounding box information to be saved to a buffer first then draw
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 None
\*-----------------------------------------------------------------------------------------------*/
static void ODDrawTask_Osd3DMsgPut(UINT32 CvFlowChan, const SVC_CV_DISP_3D_CMD_s *pInputMsgInfo)
{
    UINT32 RetVal;
    UINT8  BufIdx = ODOsdBufCtrl[CvFlowChan].WrBufIdx;  /* Ring buffer index*/
    UINT16 WrIdx3D  = ODOsdBufCtrl[CvFlowChan].WrIdx3D[BufIdx];

    SVC_CV_DISP_3D_CMD_s *pMsgInfo;

    if (WrIdx3D < ODOsdBufCtrl[CvFlowChan].MaxIdx3D) {
        pMsgInfo = pODOsd3DMsgInfo[CvFlowChan][BufIdx];
        pMsgInfo = &pMsgInfo[WrIdx3D];
        RetVal = AmbaWrap_memcpy(pMsgInfo, pInputMsgInfo, sizeof(SVC_CV_DISP_3D_CMD_s));
        ODOsdBufCtrl[CvFlowChan].WrIdx3D[BufIdx] += 1U;

        if (SVC_NG == RetVal) {
            SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_Osd3DMsgPut err. Memcpy failed %d", RetVal, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_Osd3DMsgPut err. WrIdx >= %d", ODOsdBufCtrl[CvFlowChan].MaxIdx3D, 0U);
        RetVal = SVC_NG;
    }

    AmbaMisra_TouchUnused(&RetVal);
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ODDrawTask_OsdMsgPut
 *
 *  @Description:: Put OSD messages
 *
 *  @Input      ::
 *                 CvFlowChan: CvFlow channel
 *                 pInputMsgInfo: The bounding box information to be saved to a buffer first then draw
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 None
\*-----------------------------------------------------------------------------------------------*/
static void ODDrawTask_OsdRectMsgPut(UINT32 CvFlowChan, const SVC_CV_DISP_OSD_CMD_s *pInputMsgInfo)
{
    UINT32 RetVal;
    UINT8  BufIdx = ODOsdBufCtrl[CvFlowChan].WrBufIdx;
    UINT16 RectWrIdx  = ODOsdBufCtrl[CvFlowChan].RectWrIdx[BufIdx];

    SVC_CV_DISP_OSD_CMD_s *pMsgInfo;

    if (RectWrIdx < ODOsdBufCtrl[CvFlowChan].RectMaxIdx) {
        pMsgInfo = pODOsdRectMsgInfo[CvFlowChan][BufIdx];
        pMsgInfo = &pMsgInfo[RectWrIdx];
        RetVal = AmbaWrap_memcpy(pMsgInfo, pInputMsgInfo, sizeof(SVC_CV_DISP_OSD_CMD_s));
        ODOsdBufCtrl[CvFlowChan].RectWrIdx[BufIdx] += 1U;

        if (SVC_NG == RetVal) {
            SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_OsdMsgPut err. Memcpy failed %d", RetVal, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_OsdMsgPut err. WrIdx >= %d", ODOsdBufCtrl[CvFlowChan].RectMaxIdx, 0U);
        RetVal = SVC_NG;
    }

    AmbaMisra_TouchUnused(&RetVal);
}

static void ODDrawTask_OsdLineMsgPut(UINT32 CvFlowChan, const SVC_CV_DISP_OSD_LINE_s *pInputMsgInfo)
{
    UINT32 RetVal;
    UINT8  BufIdx = ODOsdBufCtrl[CvFlowChan].WrBufIdx;
    UINT16 LineWrIdx  = ODOsdBufCtrl[CvFlowChan].LineWrIdx[BufIdx];

    SVC_CV_DISP_OSD_LINE_s *pMsgInfo;

    if (LineWrIdx < ODOsdBufCtrl[CvFlowChan].LineMaxIdx) {
        pMsgInfo = pODOsdLineMsgInfo[CvFlowChan][BufIdx];
        pMsgInfo = &pMsgInfo[LineWrIdx];
        RetVal = AmbaWrap_memcpy(pMsgInfo, pInputMsgInfo, sizeof(SVC_CV_DISP_OSD_LINE_s));
        ODOsdBufCtrl[CvFlowChan].LineWrIdx[BufIdx] += 1U;

        if (SVC_NG == RetVal) {
            SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_OsdLineMsgPut err. Memcpy failed %d", RetVal, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_OsdLineMsgPut err. WrIdx >= %d", ODOsdBufCtrl[CvFlowChan].LineMaxIdx, 0U);
        RetVal = SVC_NG;
    }

    AmbaMisra_TouchUnused(&RetVal);
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ODDrawTask_OsdStringMsgPut
 *
 *  @Description:: Put OSD messages
 *
 *  @Input      ::
 *                 CvFlowChan: CvFlow channel
 *                 pInputMsgInfo: The bounding box information to be saved to a buffer first then draw
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 None
\*-----------------------------------------------------------------------------------------------*/
static void ODDrawTask_OsdStrMsgPut(UINT32 CvFlowChan, const SVC_CV_DISP_OSD_STR_s *pInputMsgInfo)
{
    UINT32 RetVal;
    UINT8  BufIdx = ODOsdBufCtrl[CvFlowChan].WrBufIdx;  /* Ring buffer index*/
    UINT16 StrWrIdx  = ODOsdBufCtrl[CvFlowChan].StrWrIdx[BufIdx];

    SVC_CV_DISP_OSD_STR_s *pMsgInfo;

    if (StrWrIdx < ODOsdBufCtrl[CvFlowChan].StrMaxIdx) {
        pMsgInfo = pODOsdStrMsgInfo[CvFlowChan][BufIdx];
        pMsgInfo = &pMsgInfo[StrWrIdx];
        RetVal = AmbaWrap_memcpy(pMsgInfo, pInputMsgInfo, sizeof(SVC_CV_DISP_OSD_STR_s));
        ODOsdBufCtrl[CvFlowChan].StrWrIdx[BufIdx] += 1U;

        if (SVC_NG == RetVal) {
            SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_OsdStringMsgPut err. Memcpy failed %d", RetVal, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_OsdStringMsgPut err. WrIdx >= %d", ODOsdBufCtrl[CvFlowChan].StrMaxIdx, 0U);
        RetVal = SVC_NG;
    }

    AmbaMisra_TouchUnused(&RetVal);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ODDrawTask_OsdBitmapMsgPut
 *
 *  @Description:: Put OSD messages
 *
 *  @Input      ::
 *                 CvFlowChan: CvFlow channel
 *                 pInputMsgInfo: The bounding box information to be saved to a buffer first then draw
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 None
\*-----------------------------------------------------------------------------------------------*/
static void ODDrawTask_OsdBitmapMsgPut(UINT32 CvFlowChan, const SVC_CV_DISP_OSD_BITMAP_s *pInputMsgInfo)
{
    UINT32 RetVal;
    UINT8  BufIdx = ODOsdBufCtrl[CvFlowChan].WrBufIdx;  /* Ring buffer index*/
    UINT16 BitmapWrIdx  = ODOsdBufCtrl[CvFlowChan].BitmapWrIdx[BufIdx];

    SVC_CV_DISP_OSD_BITMAP_s *pMsgInfo;

    if (BitmapWrIdx < ODOsdBufCtrl[CvFlowChan].BitmapMaxIdx) {
        pMsgInfo = pODOsdBitmapMsgInfo[CvFlowChan][BufIdx];
        pMsgInfo = &pMsgInfo[BitmapWrIdx];
        RetVal = AmbaWrap_memcpy(pMsgInfo, pInputMsgInfo, sizeof(SVC_CV_DISP_OSD_BITMAP_s));
        ODOsdBufCtrl[CvFlowChan].BitmapWrIdx[BufIdx] += 1U;

        if (SVC_NG == RetVal) {
            SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_OsdBitmapMsgPut err. Memcpy failed %d", RetVal, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_OsdBitmapMsgPut err. WrIdx >= %d", ODOsdBufCtrl[CvFlowChan].BitmapMaxIdx, 0U);
        RetVal = SVC_NG;
    }

    AmbaMisra_TouchUnused(&RetVal);
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ODDrawTask_OsdFlush
 *
 *  @Description:: Flush OSD
 *
 *  @Input      ::
 *                 VoutIdx: The vout to be shown
 *                 CvFlowChan: CvFlow channel
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 None
\*-----------------------------------------------------------------------------------------------*/
static void ODDrawTask_OsdFlush(UINT32 VoutIdx, UINT32 CvFlowChan)
{
    UINT32 RetVal;

    SVC_CV_DISP_OSD_CMD_s *pRectMsgInfo;
    SVC_CV_DISP_OSD_STR_s *pStrMsgInfo;
    SVC_CV_DISP_OSD_LINE_s *pLineMsgInfo;
    SVC_CV_DISP_OSD_BITMAP_s *pBitmapMsgInfo;
    SVC_CV_DISP_3D_CMD_s  *pMsgInfo3D;
    UINT32 i;
    UINT8 WrBufIdx;

    AmbaMisra_TouchUnused(&VoutIdx);

    OD_DRAW_TASK_DBG("OsdFlush[%u] start. WrBufIdx(%u) RdBufIdx(%u)",
        CvFlowChan, ODOsdBufCtrl[CvFlowChan].WrBufIdx, ODOsdBufCtrl[CvFlowChan].RdBufIdx, 0U, 0U);

    /* Find the next write buffer */
    WrBufIdx = ODOsdBufCtrl[CvFlowChan].WrBufIdx;
    for (i = 0; i < OD_DRAW_OSD_BUF_RING_NUM; i++) {
        /* Increase writing buffer */
        if (WrBufIdx < (OD_DRAW_OSD_BUF_RING_NUM - 1U)) {
            WrBufIdx++;
        } else {
            WrBufIdx = 0U;
        }

        if ((WrBufIdx != ODOsdBufCtrl[CvFlowChan].WrBufIdx) && (WrBufIdx != ODOsdBufCtrl[CvFlowChan].RdBufIdx)) {
            break;
        }
    }

    /* Switch the RdBufIdx to the WrBufIdx to make sure the update is the last time writing */
    ODOsdBufCtrl[CvFlowChan].RdBufIdx = ODOsdBufCtrl[CvFlowChan].WrBufIdx;

    /* Switch writing buffer */
    ODOsdBufCtrl[CvFlowChan].WrBufIdx = WrBufIdx;

    OD_DRAW_TASK_DBG("OsdFlush[%u] Switch. WrBufIdx(%u) RdBufIdx(%u)",
        CvFlowChan, ODOsdBufCtrl[CvFlowChan].WrBufIdx, ODOsdBufCtrl[CvFlowChan].RdBufIdx, 0U, 0U);

    pRectMsgInfo = pODOsdRectMsgInfo[CvFlowChan][ODOsdBufCtrl[CvFlowChan].WrBufIdx]; /* Rectangle queue */
    pStrMsgInfo  = pODOsdStrMsgInfo [CvFlowChan][ODOsdBufCtrl[CvFlowChan].WrBufIdx]; /* String queue */
    pLineMsgInfo = pODOsdLineMsgInfo[CvFlowChan][ODOsdBufCtrl[CvFlowChan].WrBufIdx]; /* Line queue */
    pBitmapMsgInfo = pODOsdBitmapMsgInfo[CvFlowChan][ODOsdBufCtrl[CvFlowChan].WrBufIdx]; /* Bitmap queue */
    pMsgInfo3D   = pODOsd3DMsgInfo[CvFlowChan][ODOsdBufCtrl[CvFlowChan].WrBufIdx]; /* 3D bbx queue */

    /* Clean the RectMsgBuf before write */
    RetVal = AmbaWrap_memset(pRectMsgInfo, 0, sizeof(SVC_CV_DISP_OSD_CMD_s) * ODOsdBufCtrl[CvFlowChan].RectMaxIdx);
    if (SVC_NG == RetVal) {
        SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_OsdFlush err. RectMsgBuf Memset failed %d", RetVal, 0U);
    }

    /* Clean the StrMsgBuf before write */
    RetVal = AmbaWrap_memset(pStrMsgInfo, 0, sizeof(SVC_CV_DISP_OSD_STR_s) * ODOsdBufCtrl[CvFlowChan].StrMaxIdx);
    if (SVC_NG == RetVal) {
        SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_OsdFlush err. StrMsgBuf Memset failed %d", RetVal, 0U);
    }

    /* Clean the LineMsgInfo before write */
    RetVal = AmbaWrap_memset(pLineMsgInfo, 0, sizeof(SVC_CV_DISP_OSD_LINE_s) * ODOsdBufCtrl[CvFlowChan].LineMaxIdx);
    if (SVC_NG == RetVal) {
        SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_OsdFlush err. LineMsgInfo Memset failed %d", RetVal, 0U);
    }

    /* Clean the BitmapMsgInfo before write */
    RetVal = AmbaWrap_memset(pBitmapMsgInfo, 0, sizeof(SVC_CV_DISP_OSD_BITMAP_s) * ODOsdBufCtrl[CvFlowChan].BitmapMaxIdx);
    if (SVC_NG == RetVal) {
        SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_OsdFlush err. BitmapMsgInfo Memset failed %d", RetVal, 0U);
    }

    /* Clean the MsgBuf3D before write */
    RetVal = AmbaWrap_memset(pMsgInfo3D, 0, sizeof(SVC_CV_DISP_3D_CMD_s) * ODOsdBufCtrl[CvFlowChan].MaxIdx3D);
    if (SVC_NG == RetVal) {
        SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_OsdFlush err. MsgBuf3D Memset failed %d", RetVal, 0U);
    }

    /* Reset the Rect writing index */
    ODOsdBufCtrl[CvFlowChan].RectWrIdx[ODOsdBufCtrl[CvFlowChan].WrBufIdx] = 0U;

    /* Reset the Str writing index */
    ODOsdBufCtrl[CvFlowChan].StrWrIdx[ODOsdBufCtrl[CvFlowChan].WrBufIdx] = 0U;

    /* Reset the Line writing index */
    ODOsdBufCtrl[CvFlowChan].LineWrIdx[ODOsdBufCtrl[CvFlowChan].WrBufIdx] = 0U;

    /* Reset the Bitmap writing index */
    ODOsdBufCtrl[CvFlowChan].BitmapWrIdx[ODOsdBufCtrl[CvFlowChan].WrBufIdx] = 0U;

    /* Reset the 3D bbx writing index */
    ODOsdBufCtrl[CvFlowChan].WrIdx3D[ODOsdBufCtrl[CvFlowChan].WrBufIdx] = 0U;

    ODResultUpdate = 1U;

    OD_DRAW_TASK_DBG("OsdFlush[%u] end", CvFlowChan, 0U, 0U, 0U, 0U);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ODDrawTask_OsdColorMap
 *
 *  @Description:: Map OD category to OSD color index
 *
 *  @Input      ::
 *                 ColorMap: Color Map. 0 - adas. 1 - emirror.
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 UINT32: OSD color Index
\*-----------------------------------------------------------------------------------------------*/
static UINT32 ODDrawTask_OsdColorMap(UINT32 ColorMap, UINT32 Category)
{
    #define OD_OSD_CLUT_BASE_INDEX  (27U)   /* Index of g_SvcOsdClutBuf. */
    UINT32 ColorIdx;
    UINT32 ColorIdx_ST2Stage = Category;

    if (ColorMap == 1U) {
        /* cat 0 to  OD_OSD_CLUT_BASE_INDEX. Background.
           Cat 1 to (OD_OSD_CLUT_BASE_INDEX+1).
           Cat 2 to (OD_OSD_CLUT_BASE_INDEX+6).
           Cat 3 to (OD_OSD_CLUT_BASE_INDEX+7) */
        switch (Category) {
            case 1:
                ColorIdx = OD_OSD_CLUT_BASE_INDEX + 1U;
                break;
#if defined(CONFIG_ICAM_PROJECT_EMIRROR) && defined(CONFIG_BUILD_AMBA_ADAS)
            case LV1_CAT:
                ColorIdx = LV1_COLOR;
                break;
            case LV2_CAT:
                ColorIdx = LV2_COLOR;
                break;
            case RmgCatDebug:
                ColorIdx = RmgColorDebug;
                break;
#endif
            default:
                ColorIdx = Category + OD_OSD_CLUT_BASE_INDEX + 4U;
                break;
        }
    } else {
        /*
         * ST 2 stage purpose:
         * Category  8-43 same color with Category 4
         * Category 44-56 same color with Category 5
         * Category 57-61 same color with Category 6
         *
         * */
        if ((Category >= 8U) && (Category <= 43U)) {
            ColorIdx_ST2Stage = 4U;
        } else if ((Category >= 44U) && (Category <= 56U)) {
            ColorIdx_ST2Stage = 5U;
        } else if ((Category >= 57U) && (Category <= 61U)) {
            ColorIdx_ST2Stage = 6U;
        } else {
            ColorIdx_ST2Stage = Category;
        }

        ColorIdx = ColorIdx_ST2Stage + OD_OSD_CLUT_BASE_INDEX;
    }

    if (ColorIdx > 255U) { /* Max index = SVC_OSD_CLUT_ENTRY_SIZE */
        ColorIdx = 255;
    }

    return ColorIdx;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ODDrawTask_GuiLevel
 *
 *  @Description:: Decide GUI level
 *
 *  @Input      ::
 *                 Idx: Index
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 None
\*-----------------------------------------------------------------------------------------------*/
static UINT32 ODDrawTask_GuiLevel(UINT32 Idx)
{
    /* 28 ~ 33 */
    return (28U + Idx);
}

static void ODDrawTask_MenuStatusCallback(UINT32 StatIdx, void *pInfo)
{
    UINT32 RetVal = SVC_OK;
    SVC_APP_STAT_MENU_s *pMenuStatus = NULL;
    SVC_APP_STAT_DISP_s *pDispStatus = NULL;
    UINT32 ODDrawTaskStatus = 0U;

    AmbaMisra_TouchUnused(pInfo);   /* Misra-c fixed */
    AmbaMisra_TouchUnused(pMenuStatus); /* Misra-c fixed */
    AmbaMisra_TouchUnused(pDispStatus);

    if (StatIdx == SVC_APP_STAT_MENU) {
        AmbaMisra_TypeCast(&pMenuStatus, &pInfo);
        if (((pMenuStatus->Type == SVC_APP_STAT_MENU_TYPE_EMR) && (pMenuStatus->FuncIdx == SVC_APP_STAT_MENU_EMR_OD)) ||
            ((pMenuStatus->Type == SVC_APP_STAT_MENU_TYPE_ADAS_DVR) && (pMenuStatus->FuncIdx == SVC_APP_STAT_MENU_ADAS_OD))) {
            if (pMenuStatus->Operation == SVC_APP_STAT_MENU_OP_ENTER) {
                RetVal |= SvcODDrawTask_GetStatus(&ODDrawTaskStatus);
                if (ODDrawTaskStatus == 0U) {
                    RetVal |= SvcODDrawTask_Enable(1U);
                    ODOnOff = 1U;
                } else {
                    RetVal |= SvcODDrawTask_Enable(0U);
                    ODOnOff = 0U;
                }
            }
        }
    } else if (StatIdx == SVC_APP_STAT_DISP) {
        AmbaMisra_TypeCast(&pDispStatus, &pInfo);
        if(pDispStatus->Status == SVC_APP_STAT_DISP_CHG){
            SvcODDrawTask_ResetFoV();
            SvcLog_DBG(SVC_LOG_OD_TASK, "Display status changed. ODDraw setting should be reset", 0U, 0U);
        }else{
            /*Do nothing*/
        }
    } else {
        /* Do nothing */
    }

    if (SVC_NG == RetVal) {
        SvcLog_NG(SVC_LOG_OD_TASK, "ODDrawTask_MenuStatusCallback err. RetVal %d", RetVal, 0U);
    } else {
        /*Do nothing*/
    }
}

/**
 *  Setup Rebel ID info
 *  @param[in] Enable (1 or 0)
 */
void SvcODDrawTask_Rebel_EnableDrawID(UINT8 Enable)
{
    g_RebelOD_ID = Enable;
}

/**
 *  Setup Rebel g_D_xy info
 *  @param[in] Enable (1 or 0)
 */
void SvcODDrawTask_Rebel_EnableDrawDxy(UINT8 Enable)
{
    g_D_xy = Enable;
}

/**
 *  Setup Rebel g_V_xy info
 *  @param[in] Enable (1 or 0)
 */
void SvcODDrawTask_Rebel_EnableDrawVxy(UINT8 Enable)
{
    g_V_xy = Enable;
}

void SvcODDrawTask_Rebel_ShawAllODInfo(UINT8 Enable)
{
    g_All_OD_Info = Enable;
    AmbaPrint_PrintUInt5("ShawAllODInfo, Enable: %u", Enable, 0U, 0U, 0U, 0U);
}
