/**
 *  @file SvcVinErrTask.c
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
 *  @details svc vin error handle task
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaShell.h"
#include "AmbaSensor.h"
#include "AmbaYuv.h"
#include "AmbaFPD.h"
#include "AmbaDSP.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaDSP_Image3aStatistics.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaPrint.h"
#include "AmbaSYS.h"
#include "AmbaCache.h"
#include "AmbaSvcWrap.h"

#include "SvcErrCode.h"
#include "SvcWrap.h"
#include "SvcTask.h"
#ifdef CONFIG_BUILD_MONFRW_GRAPH
#include "AmbaMonDef.h"
#include "AmbaMonFramework.h"
#endif
#include "SvcCalibMgr.h"
#include "SvcCmd.h"
#include "SvcPref.h"

#include "SvcVinSrc.h"
#include "SvcVoutSrc.h"
#include "SvcDisplay.h"
#include "SvcIK.h"
#include "SvcImg.h"
#include "SvcOsd.h"
#include "SvcGui.h"
#include "SvcResCfg.h"
#include "SvcLiveview.h"
#include "SvcLog.h"
#include "SvcPlat.h"
#include "SvcFlowControl.h"

#include "SvcTaskList.h"
#include "SvcInfoPack.h"
#include "SvcUserPref.h"
#include "SvcCalibCfg.h"
#include "SvcCalibTask.h"
#include "SvcResCfgTask.h"
#include "SvcVinBufMonitorTask.h"
#ifdef CONFIG_SVC_VOUT_ERR_CTRL_USED
#include "SvcVoutErrTask.h"
#endif
#include "SvcVinErrTask.h"

#define SVC_VIN_ERR_FLG_SEN_FRZE_RAW    (1U)
#ifdef AMBA_MON_LISTEN_AAA_CHANGE_DETECTION
// #define SVC_VIN_ERR_FLG_SEN_FRZE_AAA    (1U)
#endif

#define SVC_VIN_ERR_FLG_INIT             (0x00000001U)
#define SVC_VIN_ERR_FLG_OSD_INIT         (0x00000002U)
#define SVC_VIN_ERR_FLG_CMD_INSTALL      (0x00000004U)
#define SVC_VIN_ERR_FLG_SHELL_INIT       (0x00000008U)
#define SVC_VIN_ERR_FLG_RAW_FRZ_INIT     (0x00000010U)
#define SVC_VIN_ERR_FLG_RAW_FRZ_OFF      (0x00000020U)
#define SVC_VIN_ERR_FLG_RCV_INIT         (0x00000100U)
#define SVC_VIN_ERR_FLG_RCV_CHK_OFF      (0x00000200U)
#define SVC_VIN_ERR_FLG_MON_MSG_INIT     (0x00001000U)

#define SVC_VIN_ERR_FLG_LOG_OK           (0x01000000U)
#define SVC_VIN_ERR_FLG_LOG_NG           (0x02000000U)
#define SVC_VIN_ERR_FLG_LOG_API          (0x04000000U)
#define SVC_VIN_ERR_FLG_LOG_MEM          (0x08000000U)
#define SVC_VIN_ERR_FLG_LOG_DBG1         (0x10000000U)  // Normal debug message
#define SVC_VIN_ERR_FLG_LOG_DBG2         (0x20000000U)  // Raw freeze debug message
#define SVC_VIN_ERR_FLG_LOG_DBG3         (0x40000000U)  // Vin-Vout latency debug message
#define SVC_VIN_ERR_FLG_LOG_DBG4         (0x80000000U)  // Fov latency debug message
#define SVC_VIN_ERR_FLG_LOG_DBG5         (0x00100000U)  // SerDes query message

#define SVC_VIN_ERR_STACK_SZ            (0xFF00U)
#define SVC_VIN_ERR_MAX_SER_NUM         (4U)
#define SVC_VIN_ERR_VIN_RECOVER_TOUT    (5000U)
#define SVC_VIN_ERR_CTRL_TASK_PRI       (SVC_VIN_ERR_TASK_PRI + 1U)
#define SVC_VIN_ERR_MON_TASK_PRI        (30U)
#define SVC_VIN_ERR_RECOVER_TASK_PRI    (SVC_VIN_ERR_TASK_PRI + 2U)
#define SVC_VIN_ERR_TIMER_PERIOD        (1U)
#define SVC_VIN_ERR_LNK_STA_CHG_THD     (1U)
#define SVC_VIN_ERR_DEP_BASE_DLY_FACT   (15U)   // If all link timeout, there is exist basic delay from Monitor framework ( about 1.5 frame sync )
#define SVC_VIN_ERR_DEP_BASE_DLY_DEN    (10U)
#define SVC_VIN_ERR_DEP_LIV_DLY_FACT    (15U)   // Configure the vin dependency VinLost update delay time
#define SVC_VIN_ERR_DEP_LIV_DLY_DEN     (10U)
#define SVC_VIN_ERR_MAX_SELECT_BITS     (32U)

/* =========================================================== *\
 *    For VIN-VOUT Latency
\* =========================================================== */
#define SVC_VIN_ERR_VOUT_LATENCY_THD     (100U)

typedef struct {
    UINT32 VoutID;
    UINT64 DispSeqCnt;
    UINT32 DispLatency;
    UINT32 FovSelectBits;
    UINT16 FovVoutLatency[AMBA_DSP_MAX_VIEWZONE_NUM];
} SVC_VIN_ERR_VOUT_LTN_UPD_s;

static UINT32 SvcVinErrTask_VoutLatencyComp(const SVC_VIN_ERR_VOUT_LTN_UPD_s *pLatency);

/* =========================================================== *\
 *    For FOV Latency
\* =========================================================== */
#define SVC_VIN_ERR_FOV_LATENCY_THD     (33U)

typedef struct {
    UINT32 FovID;
    UINT64 YuvSeqNum;
    UINT32 Latency;
} SVC_VIN_ERR_FOV_LTN_UPD_s;

static UINT32 SvcVinErrTask_FovLatencyComp(const SVC_VIN_ERR_FOV_LTN_UPD_s *pLatency);

/* =========================================================== *\
 *    For Raw Image Freeze
\* =========================================================== */
#ifdef SVC_VIN_ERR_FLG_SEN_FRZE_AAA
#define MON_VIN_STAT_MSG_CFA_INTVAL    (15U)
#define MON_VIN_STAT_MSG_CFA_SHIFT     (0U)
#endif

#define SVC_VIN_ERR_RAW_FRZ_TILE_COL    (20U)
#define SVC_VIN_ERR_RAW_FRZ_TILE_ROW    (30U)

typedef struct {
    UINT32 VinID;
    UINT8 *pRawBuf;
    UINT32 Pitch;
    UINT32 Width;
    UINT32 Height;
} SVC_VIN_ERR_RAW_FRZ_UPD_s;

typedef struct {
    UINT32 State;
#define SVC_VIN_ERR_RAW_FRZ_STATE_ALIVE     (0x1U)
#define SVC_VIN_ERR_RAW_FRZ_STATE_FREEZE    (0x2U)
#define SVC_VIN_ERR_RAW_FRZ_STATE_DISABLE   (0x3U)
    UINT32 OffsetRow[SVC_VIN_ERR_RAW_FRZ_TILE_ROW];
    UINT32 OffsetCol[SVC_VIN_ERR_RAW_FRZ_TILE_COL];
    UINT8 *pPreBuf;
    UINT32 BufPitch;
} SVC_VIN_ERR_RAW_FRZ_INFO_s;

typedef struct {
    UINT32 DataOfsRow[SVC_VIN_ERR_RAW_FRZ_TILE_ROW];
    UINT32 AccumLine;
} SVC_VIN_ERR_RAW_FRZ_DATA_REGION_s;

typedef struct {
    char                              Name[32];
    AMBA_KAL_MUTEX_t                  Mutex;
    UINT32                            SelectBits;
    SVC_VIN_ERR_RAW_FRZ_INFO_s        FrzeInfo[AMBA_DSP_MAX_VIN_NUM][SVC_VIN_ERR_MAX_SER_NUM];
    SVC_VIN_ERR_RAW_FRZ_DATA_REGION_s RawDataInfo[AMBA_DSP_MAX_VIN_NUM];
} SVC_VIN_ERR_RAW_FRZ_CTRL_s;

static UINT32 SvcVinErrTask_RawFrzCreate(void);
static UINT32 SvcVinErrTask_RawFrzDelete(void);
static void   SvcVinErrTask_RawFrzInfo(void);
static UINT32 SvcVinErrTask_RawFrzProc(SVC_VIN_ERR_RAW_FRZ_UPD_s *pRawUpd);
static void   SvcVinErrTask_RawFrzInvalidate(const SVC_VIN_ERR_RAW_FRZ_UPD_s *pRawUpd);
static void   SvcVinErrTask_RawFrzEnable(UINT32 FovID, UINT32 Enable);
static UINT32 SvcVinErrTask_RawFrzReset(UINT32 FovID);
static void   SvcVinErrTask_RawFrzVinEnable(UINT32 VinID, UINT32 Enable);
static UINT32 SvcVinErrTask_RawFrzVinReset(UINT32 VinID);

static SVC_VIN_ERR_RAW_FRZ_CTRL_s SvcVinErrRawFrzCtrl GNU_SECTION_NOZEROINIT;

/* =========================================================== *\
 *    For OSD
\* =========================================================== */
#define SVC_VIN_ERR_GUI_CANVAS_LVL      (10U)
#ifndef CONFIG_ICAM_32BITS_OSD_USED
#define SVC_VIN_ERR_OSD_STR_COLOR       (249U)
#else
#define SVC_VIN_ERR_OSD_STR_COLOR       (0x80FF0000U)
#endif
#define SVC_VIN_ERR_OSD_STR_PTN_LEN     (20U)

#define SVC_VIN_ERR_OSD_PTN_SIG_LOST    "SIGNAL LOST"
#define SVC_VIN_ERR_OSD_PTN_RAW_FREEZE  "RAW FREEZE"
#define SVC_VIN_ERR_OSD_PTN_RCV_PROC    "SIGNAL RECOVER"
#define SVC_VIN_ERR_OSD_PTN_VOUT_LTN    "IMAGE DELAY"
#define SVC_VIN_ERR_OSD_PTN_FOV_LTN     "FOV DELAY"

#define SVC_VIN_ERR_OSD_PTN_ID_SIG_LST  (0U)
#define SVC_VIN_ERR_OSD_PTN_ID_RAW_FRZ  (1U)
#define SVC_VIN_ERR_OSD_PTN_ID_RCV_PROC (2U)
#define SVC_VIN_ERR_OSD_PTN_ID_VOUT_LTN (3U)
#define SVC_VIN_ERR_OSD_PTN_ID_FOV_LTN  (4U)
#define SVC_VIN_ERR_OSD_PTN_NUM         (5U)

typedef struct {
#define SVC_VIN_ERR_OSD_UNIT_ENABLE     (0x1UL)
#define SVC_VIN_ERR_OSD_UNIT_DISPLAY    (0x2UL)
#define SVC_VIN_ERR_OSD_UPD_LOCK        (0x4UL)
#define SVC_VIN_ERR_OSD_UNIT_DBG        (0x8UL)
    UINT32            State;
    UINT32            VoutSelectBits;
    UINT32            OsdPtnSelectBits;
    SVC_GUI_CANVAS_s  DrawWin[AMBA_DSP_MAX_VOUT_NUM];
    UINT32            DrawFontSize[AMBA_DSP_MAX_VOUT_NUM][SVC_VIN_ERR_OSD_PTN_NUM];
    SVC_GUI_CANVAS_s  DrawStrWin[AMBA_DSP_MAX_VOUT_NUM][SVC_VIN_ERR_OSD_PTN_NUM];
} SVC_VIN_ERR_OSD_UNIT_s;

typedef struct {
    UINT32              VoutWidth;
    UINT32              VoutHeight;
    UINT32              OsdWidth;
    UINT32              OsdHeight;
    UINT32              DrawLevel;
    UINT32              DrawUpd;
    SVC_GUI_DRAW_FUNC   pDrawFunc;
    SVC_GUI_UPDATE_FUNC pUpdateFunc;
} SVC_VIN_ERR_OSD_INFO_s;

typedef struct {
    UINT32     PtnID;
    char       StrPtn[SVC_VIN_ERR_OSD_STR_PTN_LEN];
    UINT32     StrPtnLen;
#define SVC_VIN_ERR_OSD_PTN_ALIGN_TL    (4U)
#define SVC_VIN_ERR_OSD_PTN_ALIGN_TC    (1U)
#define SVC_VIN_ERR_OSD_PTN_ALIGN_TR    (2U)
#define SVC_VIN_ERR_OSD_PTN_ALIGN_CL    (3U)
#define SVC_VIN_ERR_OSD_PTN_ALIGN_CC    (0U)
#define SVC_VIN_ERR_OSD_PTN_ALIGN_CR    (5U)
#define SVC_VIN_ERR_OSD_PTN_ALIGN_BL    (6U)
#define SVC_VIN_ERR_OSD_PTN_ALIGN_BC    (7U)
#define SVC_VIN_ERR_OSD_PTN_ALIGN_BR    (8U)
    UINT32     AlignType;
} SVC_VIN_ERR_OSD_PTN_INFO_s;

typedef struct {
    char                        Name[32];
    AMBA_KAL_MUTEX_t            Mutex;
    UINT32                      SelectBits;
    SVC_VIN_ERR_OSD_INFO_s      OsdInfo[AMBA_DSP_MAX_VOUT_NUM];      // Depend on GUI OSD
    UINT32                      NumPtnInfos;
    SVC_VIN_ERR_OSD_PTN_INFO_s *pOsdPtnInfo;                         // Describe each OSD pattern
    UINT32                      NumUnits;
    SVC_VIN_ERR_OSD_UNIT_s      FovUnit[AMBA_DSP_MAX_VIEWZONE_NUM];  // Depend on each FOV
} SVC_VIN_ERR_OSD_CTRL_s;

static UINT32 SvcVinErrTask_OsdCreate(void);
static UINT32 SvcVinErrTask_OsdDelete(void);
static void   SvcVinErrTask_OsdUpdateLock(UINT32 VinID, UINT32 UpdateLock);
static void   SvcVinErrTask_OsdUpdate(UINT32 VinID);
static void   SvcVinErrTask_OsdInfo(void);
static void   SvcvinErrTask_OsdDrawStrCfg(UINT32 VoutID, const SVC_GUI_CANVAS_s *pDrawWin, const SVC_VIN_ERR_OSD_PTN_INFO_s *pPtnInfo,
                                          SVC_GUI_CANVAS_s *pStrWin, UINT32 *pFontSize);
static void   SvcVinErrTask_OsdDrawFunc(UINT32 VoutID, UINT32 Level);
static void   SvcVinErrTask_OsdUpdateFunc(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate);

static SVC_VIN_ERR_OSD_CTRL_s SvcVinErrOsdCtrl GNU_SECTION_NOZEROINIT;

/* =========================================================== *\
 *    For VIN Recover
\* =========================================================== */

typedef struct {
#define SVC_VIN_ERR_RCV_CMD_VIN     (0U)
#define SVC_VIN_ERR_RCV_CMD_SERDES  (1U)
#define SVC_VIN_ERR_RCV_CMD_LIV     (2U)
#define SVC_VIN_ERR_RCV_CMD_NUM     (3U)
    UINT32 CmdID;
    UINT32 VinSelectBits;
    UINT32 SerDesSelectBits;
} SVC_VIN_ERR_RECOVER_CMD_t;

#define SVC_VIN_ERR_RCV_PROC_STOP_VIN      (0U)    // STOP VIN
#define SVC_VIN_ERR_RCV_PROC_DIS_LNK_UPD   (1U)    // Disable SerDes Query
#define SVC_VIN_ERR_RCV_PROC_DIS_SENSOR    (2U)    // Disable Sensor Driver
#define SVC_VIN_ERR_RCV_PROC_RST_POWER     (3U)    // Reset Power
#define SVC_VIN_ERR_RCV_PROC_INI_SENSOR    (4U)    // Init Sensor Driver
#define SVC_VIN_ERR_RCV_PROC_ENA_SENSOR    (5U)    // Enable Sensor driver
#define SVC_VIN_ERR_RCV_PROC_CFG_SENSOR    (6U)    // Config Sensor driver
#define SVC_VIN_ERR_RCV_PROC_ENA_LNK_UPD   (7U)    // Enable SerDes Query
#define SVC_VIN_ERR_RCV_PROC_CFG_IDSP      (8U)    // Config IDSP
#define SVC_VIN_ERR_RCV_PROC_CFG_VIN       (9U)    // Config VIN
#define SVC_VIN_ERR_RCV_PROC_STR_VIN      (10U)    // Start VIN
#define SVC_VIN_ERR_RCV_PROC_RE_CFG_FPD   (11U)    // Re-config VOUT driver
#define SVC_VIN_ERR_RCV_PROC_NUM          (12U)

typedef UINT32 (*SVC_VIN_ERR_RCV_PROC_f)(UINT32 PreChkVal, const void *pCmd, const void *pProc);

typedef struct {
    UINT32                  Enable;
    UINT32                  ProcID;
    char                    ProcName[16];
    UINT32                  ProcFlg;
    SVC_VIN_ERR_RCV_PROC_f  pFunc;
} SVC_VIN_ERR_RCV_PROC_s;

typedef struct {
    AMBA_KAL_EVENT_FLAG_t    ActFlg;
    char                     Name[32];
    UINT32                   ProcSelectBits[SVC_VIN_ERR_RCV_CMD_NUM];
    SVC_VIN_ERR_RCV_PROC_s  *pSubProc;
} SVC_VIN_ERR_RCV_CTRL_s;

typedef struct {
    char                       Name[32];
    AMBA_KAL_MSG_QUEUE_t       Que;
    SVC_VIN_ERR_RECOVER_CMD_t  QueBuf[AMBA_DSP_MAX_VIN_NUM * SVC_VIN_ERR_MAX_SER_NUM];
    SVC_TASK_CTRL_s            Ctrl;
    UINT8                      Stack[SVC_VIN_ERR_STACK_SZ];
    SVC_VIN_ERR_RCV_CTRL_s     RcvCtrl;
    UINT32                     CurFmtID;
    SVC_VIN_SRC_INIT_s         VinSrcCfg;
} SVC_VIN_ERR_RECOVER_TASK_s;

static void               SvcVinErrTask_RcvSubProcInit(void);
static UINT32             SvcVinErrTask_RcvCtrlCfg(void);
static void               SvcVinErrTask_RcvCtrlInfo(void);
static void               SvcVinErrTask_RcvFlagClear(UINT32 Flag);
static void               SvcVinErrTask_RcvFlagSet(UINT32 Flag);
static void               SvcVinErrTask_RcvFlagGet(UINT32 Flag, UINT32 Timeout);
static UINT32             SvcVinErrTask_RcvFlagQry(UINT32 *pFlag);
static void               SvcVinErrTask_RcvIKConfig(UINT32 VinID);
static void               SvcVinErrTask_RcvCalibConfig(UINT32 VinID);
static void               SvcVinErrTask_RcvProcVin(SVC_VIN_ERR_RECOVER_CMD_t *pCmd);
static void               SvcVinErrTask_RcvProcSerDesLink(SVC_VIN_ERR_RECOVER_CMD_t *pCmd);
static void               SvcVinErrTask_RcvProcLiveview(SVC_VIN_ERR_RECOVER_CMD_t *pCmd);
static UINT32             SvcVinErrTask_RcvProcStartVin(UINT32 PreChkVal, const void *pCmd, const void *pProc);
static UINT32             SvcVinErrTask_RcvProcStopVin(UINT32 PreChkVal, const void *pCmd, const void *pProc);
static UINT32             SvcVinErrTask_RcvProcCfgVin(UINT32 PreChkVal, const void *pCmd, const void *pProc);
static UINT32             SvcVinErrTask_RcvProcResetPWR(UINT32 PreChkVal, const void *pCmd, const void *pProc);
static UINT32             SvcVinErrTask_RcvProcIniSensor(UINT32 PreChkVal, const void *pCmd, const void *pProc);
static UINT32             SvcVinErrTask_RcvProcEnaSensor(UINT32 PreChkVal, const void *pCmd, const void *pProc);
static UINT32             SvcVinErrTask_RcvProcDisSensor(UINT32 PreChkVal, const void *pCmd, const void *pProc);
static UINT32             SvcVinErrTask_RcvProcCfgSensor(UINT32 PreChkVal, const void *pCmd, const void *pProc);
static UINT32             SvcVinErrTask_RcvProcCfgIdsp(UINT32 PreChkVal, const void *pCmd, const void *pProc);
static UINT32             SvcVinErrTask_RcvProcEnaLnkUpd(UINT32 PreChkVal, const void *pCmd, const void *pProc);
static UINT32             SvcVinErrTask_RcvProcDisLnkUpd(UINT32 PreChkVal, const void *pCmd, const void *pProc);
static UINT32             SvcVinErrTask_RcvProcReCfgVout(UINT32 PreChkVal, const void *pCmd, const void *pProc);
static void*              SvcVinErrTask_RcvTaskEntry(void* EntryArg);

static SVC_VIN_ERR_RCV_PROC_s SvcVinErrRcvSubProc[SVC_VIN_ERR_RCV_PROC_NUM] GNU_SECTION_NOZEROINIT;

/* =========================================================== *\
 *    For AmbaMonitor Framework
\* =========================================================== */
#ifdef CONFIG_BUILD_MONFRW_GRAPH
#define VIN_ERR_MON_MSG_TIMEOUT        (0x10U)

#define VIN_ERR_MON_PORT_VIN_STATE     (0U)
#define VIN_ERR_MON_PORT_TIMELINE      (1U)
#define VIN_ERR_MON_PORT_NUM           (2U)

#define MON_VIN_STAT_MSG_RAW_START     (0U)
#define MON_VIN_STAT_MSG_RAW_TOUT      (1U)
#define MON_VIN_STAT_MSG_DEFRAW_START  (2U)
#define MON_VIN_STAT_MSG_SERLNK_STATE  (3U)
#define MON_VIN_STAT_MSG_CFA_STATE     (4U)
#define MON_VIN_STAT_MSG_VIN_TOUT      (5U)
#define MON_VIN_STAT_MSG_NUM           (6U)

#define MON_TIMELINE_MSG_RAW_DATA      (0U)
#define MON_TIMELINE_MSG_FOV_LTY       (1U)    // fov latency
#define MON_TIMELINE_MSG_VOUT_LTY      (2U)
#define MON_TIMELINE_MSG_NUM           (3U)

typedef void (*SVC_VIN_ERR_MON_MSG_HDLR_f)(const void *pMsg, UINT64 Flag, UINT32 MsgType);
typedef struct {
    UINT32                        Enable;
    UINT64                        MsgFlag;
    char                          MsgName[32];
    SVC_VIN_ERR_MON_MSG_HDLR_f    pMsgProc;
} SVC_VIN_ERR_MON_MSG_s;

typedef struct {
    UINT32                    Enable;
    UINT32                    PortID;
    char                      MsgPortName[32];
    UINT32                    NumOfMsg;
    SVC_VIN_ERR_MON_MSG_s    *pMsgList;
} SVC_VIN_ERR_MON_PORT_s;

typedef struct {
    UINT32                     NumOfMonMsgPort;
    AMBA_MON_MESSAGE_PORT_s    MonMsgPort[VIN_ERR_MON_PORT_NUM];
    SVC_VIN_ERR_MON_PORT_s    *pMonMsgPortCtrl;
} SVC_VIN_ERR_MON_CTRL_s;

typedef struct {
    UINT32                     NumOfTask;
    char                       Name[VIN_ERR_MON_PORT_NUM][32];
    SVC_TASK_CTRL_s            Ctrl[VIN_ERR_MON_PORT_NUM];
    UINT8                      Stack[VIN_ERR_MON_PORT_NUM][SVC_VIN_ERR_STACK_SZ];
} SVC_VIN_ERR_MON_TASK_s;

static void   SvcVinErrTask_MonMsgInit(void);
static UINT32 SvcVinErrTask_MonFrwkCfg(void);
static void   SvcVinErrTask_MonFrwkDeCfg(void);
static void*  SvcVinErrTask_MonFrwkTaskEntry(void* EntryArg);
static void   SvcVinErrTask_MonMsgProcRawRdy(const void *pMsg, UINT64 Flag, UINT32 MsgType);
static void   SvcVinErrTask_MonMsgProcRawStart(const void *pMsg, UINT64 Flag, UINT32 MsgType);
static void   SvcVinErrTask_MonMsgProcRawTout(const void *pMsg, UINT64 Flag, UINT32 MsgType);
static void   SvcVinErrTask_MonMsgProcDefRStart(const void *pMsg, UINT64 Flag, UINT32 MsgType);
static void   SvcVinErrTask_MonMsgProcLinkState(const void *pMsg, UINT64 Flag, UINT32 MsgType);
static void   SvcVinErrTask_MonMsgProcCfaState(const void *pMsg, UINT64 Flag, UINT32 MsgType);
static void   SvcVinErrTask_MonMsgProcVinTout(const void *pMsg, UINT64 Flag, UINT32 MsgType);
static void   SvcVinErrTask_MonMsgProcFovLatency(const void *pMsg, UINT64 Flag, UINT32 MsgType);
static void   SvcVinErrTask_MonMsgProcVinVoutLatency(const void *pMsg, UINT64 Flag, UINT32 MsgType);

static SVC_VIN_ERR_MON_MSG_s  MonVinStateMsg[MON_VIN_STAT_MSG_NUM] GNU_SECTION_NOZEROINIT;
static SVC_VIN_ERR_MON_MSG_s  MonDspListenMsg[MON_TIMELINE_MSG_NUM] GNU_SECTION_NOZEROINIT;
static SVC_VIN_ERR_MON_PORT_s MonMsgPortList[VIN_ERR_MON_PORT_NUM] GNU_SECTION_NOZEROINIT;

#else

typedef struct {
#define SVC_VIN_ERR_MSG_QRY_FLG_LINK_QRY       (0x1U)
#define SVC_VIN_ERR_MSG_QRY_FLG_TSK_EXIT_REQ   (0x10000U)
#define SVC_VIN_ERR_MSG_QRY_FLG_TSK_EXIT_DONE  (0x20000U)
    AMBA_KAL_EVENT_FLAG_t EvtFlg;
    char                  EvtFlgName[32];

    char                  TaskName[32];
    SVC_TASK_CTRL_s       TaskCtrl;
    UINT8                 TaskStack[SVC_VIN_ERR_STACK_SZ];

    UINT32                SerDesQryTimeout;
    UINT32                SerDesQryTimeCnt;
    UINT32                SerDesQryReset;    // bit0: Vin0, bit1: Vin1, ...
    UINT8                 SerDesQryProc;
    UINT32                SerDesQryTimeStampInTimer;
    UINT32                SerDesQrySensorID;
    UINT32                SerDesQryPreLinkLock;
    UINT32                SerDesQryPreVidLock;
} SVC_VIN_ERR_MSG_QRY_CTRL_s;

static UINT32 SvcVinErrTask_MsgQryCfg(void);
static void   SvcVinErrTask_MsgQryDeCfg(void);
static void   SvcVinErrTask_MsgQryInfo(void);
static void   SvcVinErrTask_MsgQryTimerHdlr(UINT32 EntryArg);
static void*  SvcVinErrTask_MsgQryTaskEntry(void* pEntryArg);
static UINT32 SvcVinErrTask_RawRdyHdlr(const void *pEventData);
static UINT32 SvcVinErrTask_VinToutHdlr(const void *pEventData);

#endif

/* =========================================================== *\
 *    For VIN Error Control
\* =========================================================== */

typedef struct {
    UINT32 VinID;
    UINT32 LinkLockState;
    UINT32 VideoLockState;
} SVC_VIN_ERR_LINK_UPD_s;

typedef struct {
    UINT32 FovID;
    UINT32 CdZero;
} SVC_VIN_ERR_SENS_UPD_s;

typedef struct {
#define SVC_VIN_ERR_CTRL_CMD_LNK_UPD    (0x1U)
#define SVC_VIN_ERR_CTRL_CMD_SENS_UPD   (0x2U)
#define SVC_VIN_ERR_CTRL_CMD_RAW_UPD    (0x3U)
#define SVC_VIN_ERR_CTRL_CMD_VLTY_UPD   (0x4U)
#define SVC_VIN_ERR_CTRL_CMD_FLTY_UPD   (0x5U)
#define SVC_VIN_ERR_CTRL_CMD_DEP_UPD    (0x6U)
#define SVC_VIN_ERR_CTRL_CMD_VIN_TOUT   (0x7U)
    UINT32 CmdID;
    UINT32 CmdData[15U];
} SVC_VIN_ERR_CTRL_CMD_t;

typedef struct {
    char                       Name[32];
    SVC_TASK_CTRL_s            Ctrl;
    AMBA_KAL_MSG_QUEUE_t       Que;
    SVC_VIN_ERR_CTRL_CMD_t     QueBuf[AMBA_DSP_MAX_VIN_NUM * 32U];
    UINT8                      Stack[SVC_VIN_ERR_STACK_SZ];
} SVC_VIN_ERR_CTRL_TASK_s;

#define SVC_VIN_ERR_SER_STAT_LINK_LOCK  ( 0x1U )
#define SVC_VIN_ERR_SER_STAT_VID_LOCK   ( 0x2U )

typedef struct {
    AMBA_KAL_EVENT_FLAG_t VinStateFlag;
#define SVC_VIN_ERR_VIN_STATE_LIV       (0x1U)
#define SVC_VIN_ERR_VIN_STATE_TOUT      (0x2U)
#define SVC_VIN_ERR_VIN_STATE_RECOVER   (0x4U)
#define SVC_VIN_ERR_VIN_STATE_DEP_TOUT  (0x8U)
#define SVC_VIN_ERR_VIN_STATE_RAW_TOUT  (0x10U)
#define SVC_VIN_ERR_VIN_STATE_RAW_RDY   (0x20U)
#define SVC_VIN_ERR_VIN_STATE_RAW_DEF   (0x40U)
#define SVC_VIN_ERR_VIN_STATE_IMGFRW_SYNC (0x100U)
#define SVC_VIN_ERR_VIN_STATE_IMGFRW_AAA  (0x200U)

    UINT32 LinkUpdEnable;
#define SVC_VIN_ERR_LNK_UPD_OFF         (0U)
#define SVC_VIN_ERR_LNK_UPD_ON          (1U)
    UINT32 LinkBkpState;
    UINT32 VideoBkpState;

    UINT32 LinkLockState;
    UINT32 VideoLockState;
    UINT32 LinkLockPreState;
    UINT32 VideoLockPreState;
#define SVC_VIN_ERR_SS_SERDES_LOCK_INVALID  (0U)
#define SVC_VIN_ERR_SS_SERDES_LOCK_OK       (1U)
#define SVC_VIN_ERR_SS_SERDES_LOCK_NG       (2U)

    UINT32 FovFreezeState;
#define SVC_VIN_ERR_FOV_STAT_FREZE      ( 0x1U )

    UINT32 FovFreezeSrc;
#define SVC_VIN_ERR_FOV_FREZE_SRC_RAW   (0x1U)
#define SVC_VIN_ERR_FOV_FREZE_SRC_AAA   (0x2U)
} SVC_VIN_ERR_STATE_s;

typedef struct {
    UINT32 UpdCtrl[AMBA_DSP_MAX_VIN_NUM];
#define SVC_VIN_ERR_DEP_UPD_IMMEDIATELY (0x1U)
#define SVC_VIN_ERR_DEP_UPD_BY_TIMER    (0x2U)
#define SVC_VIN_ERR_DEP_UPD_BY_TOUT     (0x4U)
#define SVC_VIN_ERR_DEP_UPD_BY_RESTART  (0x8U)
#ifdef CONFIG_LINUX
#define SVC_VIN_ERR_DEP_UPD_DEF         SVC_VIN_ERR_DEP_UPD_BY_TOUT
#else
#define SVC_VIN_ERR_DEP_UPD_DEF         (0U)
#endif
    UINT32 DepVinNum[AMBA_DSP_MAX_VIN_NUM];
    UINT32 DepVinBits[AMBA_DSP_MAX_VIN_NUM];    // Dependency vin bits
    UINT32 NumOfStrm;
    UINT32 StrmSelBits[AMBA_DSP_MAX_VIN_NUM];   // Streaming source bits

    struct {
        UINT32 DepVinNum;
        UINT32 DepVinBits;
        UINT32 FromVinID;
    } Recover[AMBA_DSP_MAX_VIN_NUM];
} SVC_VIN_ERR_DEPENDENCY_CTRL_s;

typedef struct {
    // Software Timer
    UINT32               State;
#define SVC_VIN_ERR_TIMER_READY         (0x1U)
#define SVC_VIN_ERR_TIMER_LINK_UPD      (0x2U)
#define SVC_VIN_ERR_TIMER_DEPENDENCY    (0x4U)

    char                 Name[32];
    UINT32               LinkLockUpd[AMBA_DSP_MAX_VIN_NUM];
    UINT32               LinkLock[AMBA_DSP_MAX_VIN_NUM];
    UINT32               VideoLockUpd[AMBA_DSP_MAX_VIN_NUM];
    UINT32               VideoLock[AMBA_DSP_MAX_VIN_NUM];
    UINT32               SelectBits;
    AMBA_KAL_TIMER_t     Timer;
    UINT32               TimerPeriod;
    UINT32               LinkChgTimeStamp[AMBA_DSP_MAX_VIN_NUM];
    UINT32               TimerSeqCnt;
    UINT32               UpdStartCnt[AMBA_DSP_MAX_VIN_NUM];
    UINT32               UpdDoneCnt[AMBA_DSP_MAX_VIN_NUM];
    UINT32               SendCmdRetVal[AMBA_DSP_MAX_VIN_NUM];

    // Dependency vin Update
    UINT32               DepVinBits;
    UINT32               DepTimeStamp[AMBA_DSP_MAX_VIN_NUM];
} SVC_VIN_ERR_SW_TIMER_CTRL_s;

typedef struct {
    UINT32                      State;
#define SVC_VIN_ERR_FOV_INIT        (0x1U)
#define SVC_VIN_ERR_FOV_VISIBLE     (0x2U)
#define SVC_VIN_ERR_FOV_VOUT_DELAY  (0x4U)
#define SVC_VIN_ERR_FOV_DELAY       (0x8U)

    UINT32                      SrcVinID;
    UINT32                      SrcSerDesIdx;
    UINT32                      DstVoutBits;
    UINT64                      VoutLatencySeqCnt;
    UINT32                      VoutLatencyThreshold;
    UINT64                      FovLatencySeqCnt;
    UINT32                      FovLatencyThreshold;
} SVC_VIN_ERR_FOV_INFO_s;

typedef struct {
    AMBA_KAL_MUTEX_t              Mutex;
    char                          Name[32];

    /*  Global state */
    UINT32                        VinSelectBits;
    UINT32                        SerDesSelectBits[AMBA_DSP_MAX_VIN_NUM];
    UINT32                        VinTimeOut[AMBA_DSP_MAX_VIN_NUM];
    UINT32                        FovSelectBits;
    SVC_VIN_ERR_FOV_INFO_s        FovInfo[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT32                        ImgStatisticMask;
    SVC_RES_CFG_s                 ResCfg;
    AMBA_KAL_EVENT_FLAG_t         VinLivFlag;
    SVC_VIN_ERR_STATE_s           ErrState[AMBA_DSP_MAX_VIN_NUM];

    SVC_VIN_ERR_SW_TIMER_CTRL_s   SwTimerCtrl;
    SVC_VIN_ERR_DEPENDENCY_CTRL_s DependencyCtrl;
#ifdef CONFIG_BUILD_MONFRW_GRAPH
    SVC_VIN_ERR_MON_CTRL_s        MonFrwkCtrl;
    SVC_VIN_ERR_MON_TASK_s        MonFrwkTask;
#else
    SVC_VIN_ERR_MSG_QRY_CTRL_s    MsgQryCtrl[AMBA_DSP_MAX_VIN_NUM];
#endif
    SVC_VIN_ERR_CTRL_TASK_s       ErrCtrlTask;
    SVC_VIN_ERR_RECOVER_TASK_s    RecoverTask;
} SVC_VIN_ERR_CTRL_s;

static inline UINT32      SvcVinErrTask_BitGet(UINT32 Val);
static void               SvcVinErrTask_ErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode);
static UINT32             SvcVinErrTask_MutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut);
static void               SvcVinErrTask_MutexGive(AMBA_KAL_MUTEX_t *pMutex);
static UINT32             SvcVinErrTask_DataInvalidate(const void *pData, UINT32 Size);
static UINT32             SvcVinErrTask_PopCount(UINT32 Val);
static UINT32             SvcVinErrTask_GetSerDesState(UINT32 VinID, UINT32 SerDesID);
static UINT32             SvcVinErrTask_IsVinLinkRdy(UINT32 VinID);
static void               SvcVinErrTask_RecoverChk(UINT32 VinID);
static void               SvcVinErrTask_SerDesPreBootChk(UINT32 VinID);
static void               SvcVinErrTask_TimerHandler(UINT32 EntryArg);
static void*              SvcVinErrTask_ErrCtrlTaskEntry(void* EntryArg);
static void               SvcVinErrTask_LinkStateChange(UINT32 VinID, UINT32 LinkLock, UINT32 VideoLock);
static void               SvcVinErrTask_LinkStateUpd(UINT32 VinID, UINT32 CurLinkLock, UINT32 CurVideoLock);
static void               SvcVinErrTask_LinkStateUpdEna(UINT32 VinID, UINT32 Enable);
static void               SvcVinErrTask_FovFreezeUpd(UINT32 FovID, UINT32 IsFreeze);
static void               SvcVinErrTask_FovStateUpd(UINT32 VinID);
static void               SvcVinErrTask_StatisticUpd(UINT32 VinID, UINT32 Enable);
static void               SvcVinErrTask_StatisticVZUpd(UINT32 ViewZoneBits, UINT32 Enable);
static void               SvcVinErrTask_ImgFrwkSyncUpd(UINT32 VinID, UINT32 Enable);
static void               SvcVinErrTask_ImgFrwkAaaUpd(UINT32 VinID, UINT32 Enable);
static void               SvcVinErrTask_DependencyChk(UINT32 VinID, UINT32 CurLinkState);
static void               SvcVinErrTask_DependencyCmd(UINT32 VinID);
static void               SvcVinErrTask_DependencyUpd(UINT32 VinID);
static void               SvcVinErrTask_VinTimeoutUpd(UINT32 VinID);
static void               SvcVinErrTask_VoutFrzReset(UINT32 VinID);
static UINT32             SvcVinErrTask_SendMsg(UINT32 CmdID, const void *pMsg, UINT32 MsgSize, UINT32 TimeOut);
static void               SvcVinErrTask_Info(void);
static UINT32             SvcVinErrTask_Config(void);
static UINT32             SvcVinErrTask_SerDesQryConfig(void);

static SVC_VIN_ERR_CTRL_s SvcVinErrCtrl GNU_SECTION_NOZEROINIT;
static UINT32             SvcVinErrCtrlFlg = ( SVC_VIN_ERR_FLG_LOG_OK |
                                               SVC_VIN_ERR_FLG_LOG_NG |
                                               SVC_VIN_ERR_FLG_LOG_API ); // SVC_VIN_ERR_FLG_DBG_MSG;

/* =========================================================== *\
 *    For Shell Command
\* =========================================================== */
typedef UINT32 (*SVC_VIN_ERR_SHELL_FUNC_f)(UINT32 ArgCount, char * const *pArgVector);
typedef void   (*SVC_VIN_ERR_SHELL_USAGE_f)(void);
typedef struct {
    UINT32                    Enable;
    char                      ShellCmdName[32];
    SVC_VIN_ERR_SHELL_FUNC_f  pFunc;
    SVC_VIN_ERR_SHELL_USAGE_f pUsage;
} SVC_VIN_ERR_SHELL_FUNC_s;

static void   SvcVinErrTask_ShellStrToU32(const char *pString, UINT32 *pValue);
static UINT32 SvcVinErrTask_ShellDbgMsg(UINT32 ArgCount, char * const *pArgVector);
static void   SvcVinErrTask_ShellDbgMsgU(void);
static UINT32 SvcVinErrTask_ShellVinRecover(UINT32 ArgCount, char * const *pArgVector);
static void   SvcVinErrTask_ShellVinRecoverU(void);
static UINT32 SvcVinErrTask_ShellLivReStart(UINT32 ArgCount, char * const *pArgVector);
static void   SvcVinErrTask_ShellLivReStartU(void);
static UINT32 SvcVinErrTask_ShellInfo(UINT32 ArgCount, char * const *pArgVector);
static void   SvcVinErrTask_ShellInfoU(void);
static UINT32 SvcVinErrTask_ShellMCtrl(UINT32 ArgCount, char * const *pArgVector);
static void   SvcVinErrTask_ShellMCtrlU(void);
static UINT32 SvcVinErrTask_ShellVoutLatency(UINT32 ArgCount, char * const *pArgVector);
static void   SvcVinErrTask_ShellVoutLatencyU(void);
static UINT32 SvcVinErrTask_ShellFovLatency(UINT32 ArgCount, char * const *pArgVector);
static void   SvcVinErrTask_ShellFovLatencyU(void);
static UINT32 SvcVinErrTask_ShellRawFrz(UINT32 ArgCount, char * const *pArgVector);
static void   SvcVinErrTask_ShellRawFrzU(void);
static UINT32 SvcVinErrTask_ShellRawFrzEna(UINT32 ArgCount, char * const *pArgVector);
static void   SvcVinErrTask_ShellRawFrzEnaU(void);
static UINT32 SvcVinErrTask_ShellRawFrzInfo(UINT32 ArgCount, char * const *pArgVector);
static void   SvcVinErrTask_ShellRawFrzInfoU(void);
static UINT32 SvcVinErrTask_ShellRawFrzRst(UINT32 ArgCount, char * const *pArgVector);
static void   SvcVinErrTask_ShellRawFrzRstU(void);
static void   SvcVinErrTask_ShellUsage(void);
static void   SvcVinErrTask_ShellEntryInit(void);
static void   SvcVinErrTask_ShellEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
static void   SvcVinErrTask_CommandInstall(void);

#define SVC_VIN_ERR_SHELL_RAW_FRZ_NUM   (3U)
static SVC_VIN_ERR_SHELL_FUNC_s SvcVinErrShellRawFrz[SVC_VIN_ERR_SHELL_RAW_FRZ_NUM] GNU_SECTION_NOZEROINIT;

#define SVC_VIN_ERR_SHELL_NUM   (8U)
static SVC_VIN_ERR_SHELL_FUNC_s SvcVinErrShellFunc[SVC_VIN_ERR_SHELL_NUM] GNU_SECTION_NOZEROINIT;

#define SVC_LOG_VIN_ERR   "VIN_ERR_TSK"
#define SVC_VIN_ERR_LOG_HL_TITLE_0  ("\033""[38;2;255;125;38m""\033""[48;2;0;0;0m" )
#define SVC_VIN_ERR_LOG_HL_TITLE_1  ("\033""[38;2;255;255;128m""\033""[48;2;0;0;0m")
#define SVC_VIN_ERR_LOG_HL_TITLE_2  ("\033""[38;2;181;230;29m""\033""[48;2;0;0;0m" )
#define SVC_VIN_ERR_LOG_HL_TITLE_3  ("\033""[38;2;239;228;176m""\033""[48;2;0;0;0m")
#define SVC_VIN_ERR_LOG_HL_TITLE_4  ("\033""[38;2;255;201;14m""\033""[48;2;0;0;0m" )
#define SVC_VIN_ERR_LOG_HL_TITLE_5  ("\033""[38;2;0;0;0m""\033""[48;2;255;201;14m" )
#define SVC_VIN_ERR_LOG_HL_TITLE_6  ("\033""[38;2;255;174;201m""\033""[48;2;0;0;0m")
#define SVC_VIN_ERR_LOG_HL_TITLE_7  ("\033""[38;2;96;255;96m""\033""[48;2;0;0;0m"  )
#define SVC_VIN_ERR_LOG_HL_NUM      ("\033""[38;2;255;128;128m""\033""[48;2;0;0;0m")
#define SVC_VIN_ERR_LOG_HL_STR      ("\033""[38;2;0;255;128m""\033""[48;2;0;0;0m"  )
#define SVC_VIN_ERR_LOG_HL_END      ("\033""[0m"                                   )

#define PRN_VIN_ERR_LOG        { SVC_WRAP_PRINT_s SvcVinErrLog; AmbaSvcWrap_MisraMemset(&(SvcVinErrLog), 0, sizeof(SvcVinErrLog)); SvcVinErrLog.Argc --; SvcVinErrLog.pStrFmt =
#define PRN_VIN_ERR_ARG_DOUBLE ; SvcVinErrLog.Argc ++; SvcVinErrLog.Argv[SvcVinErrLog.Argc].Doubld   =
#define PRN_VIN_ERR_ARG_UINT64 ; SvcVinErrLog.Argc ++; SvcVinErrLog.Argv[SvcVinErrLog.Argc].Uint64   =
#define PRN_VIN_ERR_ARG_UINT32 ; SvcVinErrLog.Argc ++; SvcVinErrLog.Argv[SvcVinErrLog.Argc].Uint64   = (UINT64)((
#define PRN_VIN_ERR_ARG_CSTR   ; SvcVinErrLog.Argc ++; SvcVinErrLog.Argv[SvcVinErrLog.Argc].pCStr    = ((
#define PRN_VIN_ERR_ARG_CPOINT ; SvcVinErrLog.Argc ++; SvcVinErrLog.Argv[SvcVinErrLog.Argc].pPointer = ((
#define PRN_VIN_ERR_ARG_POST   ))
#define PRN_VIN_ERR_OK         ; SvcVinErrLog.Argc ++; SvcVinErrTask_LogPrint(SVC_VIN_ERR_FLG_LOG_OK  , &(SvcVinErrLog)); }
#define PRN_VIN_ERR_NG         ; SvcVinErrLog.Argc ++; SvcVinErrTask_LogPrint(SVC_VIN_ERR_FLG_LOG_NG  , &(SvcVinErrLog)); }
#define PRN_VIN_ERR_API        ; SvcVinErrLog.Argc ++; SvcVinErrTask_LogPrint(SVC_VIN_ERR_FLG_LOG_API , &(SvcVinErrLog)); }
#define PRN_VIN_ERR_DBG1       ; SvcVinErrLog.Argc ++; SvcVinErrTask_LogPrint(SVC_VIN_ERR_FLG_LOG_DBG1, &(SvcVinErrLog)); }
#define PRN_VIN_ERR_DBG2       ; SvcVinErrLog.Argc ++; SvcVinErrTask_LogPrint(SVC_VIN_ERR_FLG_LOG_DBG2, &(SvcVinErrLog)); }
#define PRN_VIN_ERR_DBG3       ; SvcVinErrLog.Argc ++; SvcVinErrTask_LogPrint(SVC_VIN_ERR_FLG_LOG_DBG3, &(SvcVinErrLog)); }
#define PRN_VIN_ERR_DBG4       ; SvcVinErrLog.Argc ++; SvcVinErrTask_LogPrint(SVC_VIN_ERR_FLG_LOG_DBG4, &(SvcVinErrLog)); }
#define PRN_VIN_ERR_DBG5       ; SvcVinErrLog.Argc ++; SvcVinErrTask_LogPrint(SVC_VIN_ERR_FLG_LOG_DBG5, &(SvcVinErrLog)); }
#define PRN_VIN_ERR_DBG1_5     ; SvcVinErrLog.Argc ++; SvcVinErrTask_LogPrint(SVC_VIN_ERR_FLG_LOG_DBG1|SVC_VIN_ERR_FLG_LOG_DBG5, &(SvcVinErrLog)); }
#define PRN_VIN_ERR_HDLR       SvcVinErrTask_ErrHdlr(__func__, __LINE__, PRetVal);

static void SvcVinErrTask_NGLog(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if ((pModule != NULL) && (pFormat != NULL)) {
        AmbaPrint_PrintStr5("\033""[0;31m[%s|NG]: %s", pModule, pFormat, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(&Arg1);
    AmbaMisra_TouchUnused(&Arg2);
}

static void SvcVinErrTask_MemLog(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if (pFormat != NULL) {
        typedef void (*SVC_VIN_ERR_MEM_LOG_f)(const char *pStrBuf, UINT32 StrLen);
        extern void SvcVinBufMonTask_MemLogWrite(const char *pStrBuf, UINT32 StrLen) GNU_WEAK_SYMBOL;
        SVC_VIN_ERR_MEM_LOG_f pSvcVinErrMemLog = SvcVinBufMonTask_MemLogWrite;

        if (pSvcVinErrMemLog != NULL) {
            (pSvcVinErrMemLog)(pFormat, SvcWrap_strlen(pFormat));
            (pSvcVinErrMemLog)("\n", 1U);
        }
    }
    (void)pModule;
    AmbaMisra_TouchUnused(&Arg1);
    AmbaMisra_TouchUnused(&Arg2);
}

static void SvcVinErrTask_LogPrint(UINT32 LogLevel, SVC_WRAP_PRINT_s *pPrint)
{
    if (pPrint != NULL) {
        if ((SvcVinErrCtrlFlg & LogLevel) > 0U) {
            const char STR_END = '\0';

            switch (LogLevel) {
            case SVC_VIN_ERR_FLG_LOG_OK :
                pPrint->pProc = SvcLog_OK;
                break;
            case SVC_VIN_ERR_FLG_LOG_NG :
                pPrint->pProc = SvcVinErrTask_NGLog;
                break;
            default :
                if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_LOG_MEM) > 0U) {
                    UINT32 Idx;
                    for (Idx = 0U; Idx < pPrint->Argc; Idx ++) {
                        if (pPrint->Argv[Idx].pCStr != NULL) {
                            if ((pPrint->Argv[Idx].pCStr[0] == '\033') &&
                                (pPrint->Argv[Idx].pCStr[1] == '[')) {
                                pPrint->Argv[Idx].pCStr = &STR_END;
                            }
                        }
                    }
                    pPrint->pProc = SvcVinErrTask_MemLog;
                } else {
                    pPrint->pProc = SvcLog_DBG;
                }
                break;
            }

            SvcWrap_Print(SVC_LOG_VIN_ERR, pPrint);
        }
    }
}

static inline UINT32 SvcVinErrTask_BitGet(UINT32 Val)
{
    return ((Val < 32U) ? (UINT32)(0x1UL << Val) : 0U);
}

static UINT32 SvcVinErrTask_MutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut)
{
    return AmbaKAL_MutexTake(pMutex, TimeOut);
}

static void SvcVinErrTask_MutexGive(AMBA_KAL_MUTEX_t *pMutex)
{
    UINT32 ErrCode = AmbaKAL_MutexGive(pMutex);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }
}

static UINT32 SvcVinErrTask_DataInvalidate(const void *pData, UINT32 Size)
{
    UINT32 RetVal = SVC_OK;

    if (pData == NULL) {
        RetVal = SVC_NG;
    } else if (Size == 0U) {
        RetVal = SVC_NG;
    } else {
        ULONG DataSize, DataAddr, AlignAddr;

        DataSize = (ULONG)Size; DataSize &= 0xFFFFFFFFU;

        AmbaMisra_TypeCast(&DataAddr, &pData);

        AlignAddr = DataAddr & ~(CACHE_LINE_SIZE - 1U);

        if (AlignAddr < DataAddr) {
            DataSize += DataAddr - AlignAddr;
        }
        RetVal = SvcPlat_CacheInvalidate(AlignAddr, DataSize);
    }

    return RetVal;
}

static void SvcVinErrTask_ErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode)
{
    if (pCaller != NULL) {
        if (ErrCode != 0U) {
            if ((SvcVinErrCtrlFlg & (SVC_VIN_ERR_FLG_LOG_DBG1 | SVC_VIN_ERR_FLG_LOG_DBG2 | SVC_VIN_ERR_FLG_LOG_DBG3)) > 0U) {
                PRN_VIN_ERR_LOG "Catch ErrCode(0x%08x) @ %s, %d"
                    PRN_VIN_ERR_ARG_UINT32 ErrCode  PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   pCaller  PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 CodeLine PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_NG
            }
        }
    }
}

static UINT32 SvcVinErrTask_PopCount(UINT32 Val)
{
    UINT32 RetVal = 0U;
    UINT32 Idx;

    for (Idx = 0U; Idx < SVC_VIN_ERR_MAX_SELECT_BITS; Idx ++) {
        if ((Val & SvcVinErrTask_BitGet(Idx)) > 0U) {
            RetVal ++;
        }
    }


    return RetVal;
}

static UINT32 SvcVinErrTask_GetSerDesState(UINT32 VinID, UINT32 SerDesID)
{
    UINT32 State = 0U;

    if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        PRN_VIN_ERR_LOG "Fail to get serdes state - invalid VinID(%d)"
            PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_NG
    } else if (SerDesID >= SVC_VIN_ERR_MAX_SER_NUM) {
        PRN_VIN_ERR_LOG "Fail to get serdes state - invalid SerDesID(%d)"
            PRN_VIN_ERR_ARG_UINT32 SerDesID PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_NG
    } else {
        if ( ( SvcVinErrCtrl.SerDesSelectBits[VinID] & SvcVinErrTask_BitGet(SerDesID) ) == 0U) {
            PRN_VIN_ERR_LOG "Fail to get serdes state - not supported SerDesID(%d) SerDesSelectBits(0x%x)"
                PRN_VIN_ERR_ARG_UINT32 SerDesID                              PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.SerDesSelectBits[VinID] PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        } else {
            UINT32 CurLinkLock  = SvcVinErrCtrl.ErrState[VinID].LinkLockState;
            UINT32 CurVideoLock = SvcVinErrCtrl.ErrState[VinID].VideoLockState;

            if ( ( CurLinkLock & ( (UINT32)SVC_VIN_ERR_SS_SERDES_LOCK_OK << (0x8UL * SerDesID) ) ) > 0U ) {
                State |= SVC_VIN_ERR_SER_STAT_LINK_LOCK;
            } else if ( ( CurLinkLock & ( (UINT32)SVC_VIN_ERR_SS_SERDES_LOCK_NG << (0x8UL * SerDesID) ) ) > 0U ) {
                PRN_VIN_ERR_LOG "There is something wrong to get VinID(%d) SerDesID(%d) LinkLock!"
                    PRN_VIN_ERR_ARG_UINT32 VinID    PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SerDesID PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_DBG2
            } else {
                PRN_VIN_ERR_LOG "The system does not support VinID(%d) SerDesID(%d) LinkLock! %d"
                    PRN_VIN_ERR_ARG_UINT32 VinID                              PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SerDesID                           PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SVC_VIN_ERR_SS_SERDES_LOCK_INVALID PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_DBG1
            }

            if ((State & SVC_VIN_ERR_SER_STAT_LINK_LOCK) > 0U) {
                if ( ( CurVideoLock & ( (UINT32)SVC_VIN_ERR_SS_SERDES_LOCK_OK << (0x8UL * SerDesID) ) ) > 0U ) {
                    State |= SVC_VIN_ERR_SER_STAT_VID_LOCK;
                } else if ( ( CurVideoLock & ( (UINT32)SVC_VIN_ERR_SS_SERDES_LOCK_NG << (0x8UL * SerDesID) ) ) > 0U ) {
                    PRN_VIN_ERR_LOG "There is something wrong to get VinID(%d) SerDesID(%d) VideoLock!"
                        PRN_VIN_ERR_ARG_UINT32 VinID    PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 SerDesID PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_DBG2
                } else {
                    PRN_VIN_ERR_LOG "The system does not support VinID(%d) SerDesID(%d) VideoLock! %d"
                        PRN_VIN_ERR_ARG_UINT32 VinID                              PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 SerDesID                           PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 SVC_VIN_ERR_SS_SERDES_LOCK_INVALID PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_DBG1
                }
            }
        }
    }

    return State;
}

static UINT32 SvcVinErrTask_IsVinLinkRdy(UINT32 VinID)
{
    UINT32 RetVal = SVC_NG;

    if (VinID < AMBA_DSP_MAX_VIN_NUM) {
        UINT32 SerDesIdx;
        UINT32 LinkLock = 0U;

        for (SerDesIdx = 0U; SerDesIdx < SVC_VIN_ERR_MAX_SER_NUM; SerDesIdx ++) {
            if ((SvcVinErrCtrl.SerDesSelectBits[VinID] & SvcVinErrTask_BitGet(SerDesIdx)) > 0U) {
                if ((SvcVinErrTask_GetSerDesState(VinID, SerDesIdx) & SVC_VIN_ERR_SER_STAT_LINK_LOCK) > 0U) {
                    LinkLock |= SvcVinErrTask_BitGet(SerDesIdx);
                }
            }
        }

        if (LinkLock == SvcVinErrCtrl.SerDesSelectBits[VinID]) {
            RetVal = SVC_OK;
        }
    }

    return RetVal;
}

static void SvcVinErrTask_StatisticUpd(UINT32 VinID, UINT32 Enable)
{
    if (VinID < AMBA_DSP_MAX_VIN_NUM) {
        UINT32 Idx;
        UINT32 FovIDs[AMBA_DSP_MAX_VIEWZONE_NUM], FovNum = 0U;

        AmbaSvcWrap_MisraMemset(FovIDs, 0, sizeof(FovIDs));
        if (0U != SvcResCfg_GetFovIdxsInVinID(VinID, FovIDs, &FovNum)) {
            PRN_VIN_ERR_LOG "  %s VinID(%d) Image statistic fail"
                PRN_VIN_ERR_ARG_UINT32 VinID                                              PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   ( ( Enable > 0U ) ? ( "enable" ) : ( "disable" ) ) PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        } else {
            for (Idx = 0U; Idx < FovNum; Idx ++) {
                if (Enable > 0U) {
                    SvcVinErrCtrl.ImgStatisticMask |= SvcVinErrTask_BitGet(FovIDs[Idx]);
                } else {
                    SvcVinErrCtrl.ImgStatisticMask &= ~SvcVinErrTask_BitGet(FovIDs[Idx]);
                }
            }

            SvcImg_StatisticsMask(SvcVinErrCtrl.ImgStatisticMask);
            PRN_VIN_ERR_LOG "  process VinID(%d) image statistic task %s done"
                PRN_VIN_ERR_ARG_UINT32 VinID                                              PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   ( ( Enable > 0U ) ? ( "enable" ) : ( "disable" ) ) PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API
        }
    }
}

static void SvcVinErrTask_StatisticVZUpd(UINT32 ViewZoneBits, UINT32 Enable)
{
    if (ViewZoneBits > 0U) {
        UINT32 CurBits = (SvcVinErrTask_BitGet(AMBA_DSP_MAX_VIEWZONE_NUM) - 1U) & ViewZoneBits;

        if (Enable > 0U) {
            SvcVinErrCtrl.ImgStatisticMask |= CurBits;
        } else {
            SvcVinErrCtrl.ImgStatisticMask &= ~CurBits;
        }

        SvcImg_StatisticsMask(SvcVinErrCtrl.ImgStatisticMask);
        PRN_VIN_ERR_LOG "Update image statistic mask with 0x%X"
        PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.ImgStatisticMask PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_API
    }
}

static void SvcVinErrTask_ImgFrwkAaaUpd(UINT32 VinID, UINT32 Enable)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to update image task state - invalid VinID(%d)"
            PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_NG
    } else {
        extern void ImageMain_Config(UINT32 VinId, UINT64 Param);
        AMBA_KAL_EVENT_FLAG_INFO_s CurEventInfo;

        AmbaSvcWrap_MisraMemset(&CurEventInfo, 0, sizeof(CurEventInfo));
        PRetVal = AmbaKAL_EventFlagQuery(&(SvcVinErrCtrl.ErrState[VinID].VinStateFlag), &CurEventInfo); PRN_VIN_ERR_HDLR

        if (((CurEventInfo.CurrentFlags & SVC_VIN_ERR_VIN_STATE_IMGFRW_AAA) > 0U) &&
            (Enable == 0U)) {
            PRN_VIN_ERR_LOG "Disable VinID(%d) image aaa task."
                PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API
            SvcImg_AaaStop(VinID);
            PRN_VIN_ERR_LOG "Disable VinID(%d) image aaa task done."
                PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API

            PRetVal = AmbaKAL_EventFlagClear(&(SvcVinErrCtrl.ErrState[VinID].VinStateFlag), SVC_VIN_ERR_VIN_STATE_IMGFRW_AAA); PRN_VIN_ERR_HDLR

        } else if (((CurEventInfo.CurrentFlags & SVC_VIN_ERR_VIN_STATE_IMGFRW_AAA) == 0U) &&
                   (Enable > 0U)) {
            PRN_VIN_ERR_LOG "Enable VinID(%d) image aaa task."
                PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API
            SvcImg_AaaStart(VinID);
            PRN_VIN_ERR_LOG "Enable VinID(%d) image aaa task done."
                PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API

            PRetVal = AmbaKAL_EventFlagSet(&(SvcVinErrCtrl.ErrState[VinID].VinStateFlag), SVC_VIN_ERR_VIN_STATE_IMGFRW_AAA); PRN_VIN_ERR_HDLR
        } else {
            PRN_VIN_ERR_LOG "VinID(%d) image aaa task is %s. Not need to %s it"
                PRN_VIN_ERR_ARG_UINT32 VinID                                                                                      PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   (((CurEventInfo.CurrentFlags & SVC_VIN_ERR_VIN_STATE_IMGFRW_AAA) == 0U) ? "disable" : "enable") PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   ((Enable == 0U) ? "disable" : "enable")                                                    PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API
        }
    }

    AmbaMisra_TouchUnused(&RetVal);
}

static void SvcVinErrTask_ImgFrwkSyncUpd(UINT32 VinID, UINT32 Enable)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to update image task state - invalid VinID(%d)"
            PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_NG
    } else {
        extern void ImageMain_Config(UINT32 VinId, UINT64 Param);
        AMBA_KAL_EVENT_FLAG_INFO_s CurEventInfo;

        AmbaSvcWrap_MisraMemset(&CurEventInfo, 0, sizeof(CurEventInfo));
        PRetVal = AmbaKAL_EventFlagQuery(&(SvcVinErrCtrl.ErrState[VinID].VinStateFlag), &CurEventInfo); PRN_VIN_ERR_HDLR

        if (((CurEventInfo.CurrentFlags & SVC_VIN_ERR_VIN_STATE_IMGFRW_SYNC) > 0U) &&
            (Enable == 0U)) {
            /*
            PRN_VIN_ERR_LOG "Disable VinID(%d) image task."
                PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API
            SvcImg_AaaStop(VinID);
            PRN_VIN_ERR_LOG "Disable VinID(%d) image task done."
                PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API
            */

            PRN_VIN_ERR_LOG "Disable VinID(%d) sync task."
                PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API
            SvcImg_SyncEnable(VinID, 0U);
            PRN_VIN_ERR_LOG "Disable VinID(%d) sync task done."
                PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API

            PRetVal = AmbaKAL_EventFlagClear(&(SvcVinErrCtrl.ErrState[VinID].VinStateFlag), SVC_VIN_ERR_VIN_STATE_IMGFRW_SYNC); PRN_VIN_ERR_HDLR

        } else if (((CurEventInfo.CurrentFlags & SVC_VIN_ERR_VIN_STATE_IMGFRW_SYNC) == 0U) &&
                   (Enable > 0U)) {
            /*
            PRN_VIN_ERR_LOG "Enable VinID(%d) image task."
                PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API
            SvcImg_AaaStart(VinID);
            PRN_VIN_ERR_LOG "Enable VinID(%d) image task done."
                PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API
            */

            PRN_VIN_ERR_LOG "Enable VinID(%d) sync task."
                PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API
            SvcImg_SyncEnable(VinID, 1U);
            PRN_VIN_ERR_LOG "Enable VinID(%d) sync task done."
                PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API

            PRetVal = AmbaKAL_EventFlagSet(&(SvcVinErrCtrl.ErrState[VinID].VinStateFlag), SVC_VIN_ERR_VIN_STATE_IMGFRW_SYNC); PRN_VIN_ERR_HDLR
        } else {
            PRN_VIN_ERR_LOG "VinID(%d) image task is %s. Not need to %s it"
                PRN_VIN_ERR_ARG_UINT32 VinID                                                                                      PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   (((CurEventInfo.CurrentFlags & SVC_VIN_ERR_VIN_STATE_IMGFRW_SYNC) == 0U) ? "disable" : "enable") PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   ((Enable == 0U) ? "disable" : "enable")                                                    PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API
        }
    }

    AmbaMisra_TouchUnused(&RetVal);
}

static void SvcVinErrTask_DependencyChk(UINT32 VinID, UINT32 CurLinkState)
{
    if (VinID < AMBA_DSP_MAX_VIN_NUM) {
        UINT32 SerDesIdx, IsAllLinkLost = 1U;

        for (SerDesIdx = 0U; SerDesIdx < SVC_VIN_ERR_MAX_SER_NUM; SerDesIdx ++) {
            if ((SvcVinErrCtrl.SerDesSelectBits[VinID] & SvcVinErrTask_BitGet(SerDesIdx)) > 0U) {
                if (0U < SvcVinErrTask_GetSerDesState(VinID, SerDesIdx)) {
                    IsAllLinkLost = 0U;
                    break;
                }
            }
        }

        if (IsAllLinkLost == 1U) {
            const SVC_VIN_ERR_DEPENDENCY_CTRL_s *pCtrl = &(SvcVinErrCtrl.DependencyCtrl);

            if ((pCtrl->DepVinBits[VinID] & SvcVinErrTask_BitGet(VinID)) == 0U) {
                PRN_VIN_ERR_LOG "Fail to check dependency vin - invalid VinID(%d) dependency bits(0x%08x)"
                    PRN_VIN_ERR_ARG_UINT32 VinID                    PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 pCtrl->DepVinBits[VinID] PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_DBG1
            } else if (pCtrl->DepVinNum[VinID] <= 1U) {
                PRN_VIN_ERR_LOG "The VinID(%d) does not depend on other vin"
                    PRN_VIN_ERR_ARG_UINT32 VinID                    PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 pCtrl->DepVinBits[VinID] PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_DBG1
            } else {
                SvcVinErrTask_DependencyCmd(VinID);
            }
        }

        AmbaMisra_TouchUnused(&CurLinkState);
    }
}

static void SvcVinErrTask_DependencyCmd(UINT32 VinID)
{
    UINT32 PRetVal;

    if (VinID < AMBA_DSP_MAX_VIN_NUM) {
        UINT32 PreCmdTiming = 0xFFU;
        SVC_VIN_ERR_DEPENDENCY_CTRL_s *pCtrl = &(SvcVinErrCtrl.DependencyCtrl);

#if defined(SVC_VIN_ERR_DEP_UPD_IMMEDIATELY)
        PreCmdTiming = SVC_VIN_ERR_DEP_UPD_IMMEDIATELY;
#elif defined(SVC_VIN_ERR_DEP_UPD_BY_TIMER)
        PreCmdTiming = SVC_VIN_ERR_DEP_UPD_BY_TIMER;
#endif
#ifdef CONFIG_LINUX
        PreCmdTiming = SVC_VIN_ERR_DEP_UPD_BY_TOUT;
#endif
        AmbaMisra_TouchUnused(&PreCmdTiming);

        if (PreCmdTiming == SVC_VIN_ERR_DEP_UPD_IMMEDIATELY) {
            SVC_VIN_ERR_LINK_UPD_s LinkUpd;

            pCtrl->UpdCtrl[VinID] = SVC_VIN_ERR_DEP_UPD_IMMEDIATELY;

            AmbaSvcWrap_MisraMemset(&LinkUpd, 0, sizeof(LinkUpd));
            LinkUpd.VinID = VinID;
            PRetVal = SvcVinErrTask_SendMsg(SVC_VIN_ERR_CTRL_CMD_DEP_UPD, &LinkUpd, (UINT32)sizeof(LinkUpd), AMBA_KAL_NO_WAIT);
            if (PRetVal != 0U) {
                PRN_VIN_ERR_LOG "Fail to prepare VinID(%d) dependency update request - err control task queue full! ErrCode(0x%08x)"
                    PRN_VIN_ERR_ARG_UINT32 VinID   PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 PRetVal PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_NG
            } else {
                PRN_VIN_ERR_LOG "Send VinID(%s%d%s) dependency update!"
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_DBG1
            }
        } else if (PreCmdTiming == SVC_VIN_ERR_DEP_UPD_BY_TIMER) {
            const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
            SVC_VIN_ERR_SW_TIMER_CTRL_s *pTimerCtrl = &(SvcVinErrCtrl.SwTimerCtrl);
            UINT32 TimeTicks = 0U;

            PRetVal = AmbaKAL_GetSysTickCount(&TimeTicks); PRN_VIN_ERR_HDLR

            if (pCfg == NULL) {
                PRN_VIN_ERR_LOG "Fail to check dependency vin - res_cfg should not null"
                    PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_DBG1

                pCtrl->UpdCtrl[VinID] = SVC_VIN_ERR_DEP_UPD_BY_TOUT;
            } else if ((pTimerCtrl->DepVinBits & SvcVinErrTask_BitGet(VinID)) > 0U) {
                PRN_VIN_ERR_LOG "The VinID(%d) dependency timer update has been enable"
                    PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_DBG1
            } else {
                UINT32 BaseDlyTime, AdjDlyTime;

                BaseDlyTime  = ((pCfg->VinCfg[VinID].FrameRate.NumUnitsInTick * 1000U) * SVC_VIN_ERR_DEP_BASE_DLY_FACT);
                BaseDlyTime /= (pCfg->VinCfg[VinID].FrameRate.TimeScale * SVC_VIN_ERR_DEP_BASE_DLY_DEN);

                AdjDlyTime  = ((pCfg->VinCfg[VinID].FrameRate.NumUnitsInTick * 1000U) * SVC_VIN_ERR_DEP_LIV_DLY_FACT);
                AdjDlyTime /= (pCfg->VinCfg[VinID].FrameRate.TimeScale * SVC_VIN_ERR_DEP_LIV_DLY_DEN);

                if ((BaseDlyTime + AdjDlyTime) >= SvcVinErrCtrl.VinTimeOut[VinID]) {
                    pCtrl->UpdCtrl[VinID] = SVC_VIN_ERR_DEP_UPD_BY_TOUT;

                    PRetVal = BaseDlyTime + AdjDlyTime;

                    PRN_VIN_ERR_LOG "Total VinID(%d) dependency time(%u) > timeout(%u). Prepare VinLost after TimeOut event!"
                        PRN_VIN_ERR_ARG_UINT32 VinID                           PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 PRetVal                         PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.VinTimeOut[VinID] PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_DBG1
                } else {

                    pCtrl->UpdCtrl[VinID] = SVC_VIN_ERR_DEP_UPD_BY_TIMER;

                    pTimerCtrl->DepTimeStamp[VinID] = TimeTicks + AdjDlyTime;

                    PRN_VIN_ERR_LOG "Enable VinID(%d) dependency timer update after %d ms"
                        PRN_VIN_ERR_ARG_UINT32 VinID                           PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 pTimerCtrl->DepTimeStamp[VinID] PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_DBG1

                    pTimerCtrl->State |= SVC_VIN_ERR_TIMER_DEPENDENCY;
                    pTimerCtrl->DepVinBits |= SvcVinErrTask_BitGet(VinID);
                }
            }
        } else {
            PRN_VIN_ERR_LOG "Prepare VinID(%d) VinLost by timeout event"
                PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_DBG1

            pCtrl->UpdCtrl[VinID] = SVC_VIN_ERR_DEP_UPD_BY_TOUT;
        }
    }
}

static void SvcVinErrTask_DependencyUpd(UINT32 VinID)
{
    UINT32 PRetVal;

    if (VinID < AMBA_DSP_MAX_VIN_NUM) {
        SVC_VIN_ERR_DEPENDENCY_CTRL_s *pCtrl = &(SvcVinErrCtrl.DependencyCtrl);

        if (pCtrl->UpdCtrl[VinID] == 0U) {
            PRN_VIN_ERR_LOG "The VinID(%s%d%s) dependency update ctrl is 0x%x. Ignore current VinLost update!"
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 pCtrl->UpdCtrl[VinID]      PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_DBG1
        } else if ((pCtrl->UpdCtrl[VinID] != SVC_VIN_ERR_DEP_UPD_IMMEDIATELY) &&
                   (pCtrl->UpdCtrl[VinID] != SVC_VIN_ERR_DEP_UPD_BY_TIMER) &&
                   (pCtrl->UpdCtrl[VinID] != SVC_VIN_ERR_DEP_UPD_BY_TOUT) &&
                   (pCtrl->UpdCtrl[VinID] != SVC_VIN_ERR_DEP_UPD_BY_RESTART)) {
            PRN_VIN_ERR_LOG "The VinID(%s%d%s) dependency update ctrl is 0x%x. Un-support it!"
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 pCtrl->UpdCtrl[VinID]      PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_DBG1
        } else {
            UINT32 RetVal, Idx;
            AMBA_DSP_LV_VIN_STATE_s VinState = { .VinId = (UINT16)VinID, .State = AMBA_DSP_VIN_STATE_DISCNNT };

            PRN_VIN_ERR_LOG "Update VinID(%s%d%s) VinLost event"
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_DBG1

            RetVal = AmbaDSP_LiveviewUpdateVinState(1U, &VinState);

            pCtrl->UpdCtrl[VinID] = SVC_VIN_ERR_DEP_UPD_DEF;

            if (RetVal == 0U) {

                PRN_VIN_ERR_LOG "Update VinID(%s%d%s) VinLost done! 0x%08x"
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 RetVal                     PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_API

                PRetVal = AmbaKAL_EventFlagClear(&(SvcVinErrCtrl.ErrState[VinID].VinStateFlag), SVC_VIN_ERR_VIN_STATE_LIV); PRN_VIN_ERR_HDLR
                for (Idx = 0U; Idx < AMBA_DSP_MAX_VIN_NUM; Idx ++) {
                    if ((pCtrl->DepVinBits[VinID] & SvcVinErrTask_BitGet(Idx)) > 0U) {
                        PRetVal = AmbaKAL_EventFlagSet(&(SvcVinErrCtrl.ErrState[Idx].VinStateFlag), SVC_VIN_ERR_VIN_STATE_DEP_TOUT); PRN_VIN_ERR_HDLR
                    }
                }
            } else {
                PRN_VIN_ERR_LOG "Fail to Update VinID(%d) VinLost! ErrCode(0x%08x)"
                    PRN_VIN_ERR_ARG_UINT32 VinID  PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 RetVal PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_API
            }
        }
    }
}

static void SvcVinErrTask_VinTimeoutUpd(UINT32 VinID)
{
    if (VinID < AMBA_DSP_MAX_VIN_NUM) {
        const SVC_VIN_ERR_DEPENDENCY_CTRL_s *pDepCtrl = &(SvcVinErrCtrl.DependencyCtrl);

        // If current vin depend on other vin and enable update vin state change at vin timeout timing.
        if ((pDepCtrl->UpdCtrl[VinID] & SVC_VIN_ERR_DEP_UPD_BY_TOUT) > 0U) {
            SvcVinErrTask_DependencyUpd(VinID);
        }

        // Stop image framework with specify VIN
        SvcVinErrTask_ImgFrwkSyncUpd(VinID, 0U);

        // Update osd state
        SvcVinErrTask_OsdUpdate(VinID);
    }
}

static void SvcVinErrTask_VoutFrzReset(UINT32 VinID)
{
    if (VinID < AMBA_DSP_MAX_VIN_NUM) {
#ifdef CONFIG_SVC_VOUT_ERR_CTRL_USED
        if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_INIT) > 0U) {
            UINT32 Idx;
            UINT32 FovIDs[AMBA_DSP_MAX_VIEWZONE_NUM], FovNum = 0U;
            UINT32 ReqVoutBits = 0U;

            AmbaSvcWrap_MisraMemset(FovIDs, 0, sizeof(FovIDs));

            if (0U == SvcResCfg_GetFovIdxsInVinID(VinID, FovIDs, &FovNum)) {
                for (Idx = 0U; Idx < FovNum; Idx ++) {
                    if (FovIDs[Idx] < AMBA_DSP_MAX_VIEWZONE_NUM) {
                        ReqVoutBits |= SvcVinErrCtrl.FovInfo[FovIDs[Idx]].DstVoutBits;
                    }
                }
            }

            for (Idx = 0U; Idx < AMBA_DSP_MAX_VOUT_NUM; Idx ++) {
                if ((ReqVoutBits & SvcVinErrTask_BitGet(Idx)) > 0U) {
                    SvcVoutErrTask_FreezeReset(Idx);
                }
            }
        }
#endif
        AmbaMisra_TouchUnused(&VinID);
    }
}

static UINT32 SvcVinErrTask_SendMsg(UINT32 CmdID, const void *pMsg, UINT32 MsgSize, UINT32 TimeOut)
{
    UINT32 RetVal = SVC_OK;

    if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_INIT) == 0U) {
        RetVal = 1U;
    } else if (pMsg == NULL) {
        RetVal = 2U;
    } else if (MsgSize == 0U) {
        RetVal = 3U;
    } else {
        SVC_VIN_ERR_CTRL_CMD_t ErrCtrlCmd;

        AmbaSvcWrap_MisraMemset(&ErrCtrlCmd, 0, sizeof(ErrCtrlCmd));

        if (MsgSize > (UINT32)sizeof(ErrCtrlCmd.CmdData)) {
            RetVal = 4U;
        } else {
            ErrCtrlCmd.CmdID = CmdID;

            if (0U == AmbaWrap_memcpy(ErrCtrlCmd.CmdData, pMsg, MsgSize)) {
                RetVal = AmbaKAL_MsgQueueSend(&(SvcVinErrCtrl.ErrCtrlTask.Que), &ErrCtrlCmd, TimeOut);
            }
        }
    }

    return RetVal;
}

static void SvcVinErrTask_RcvIKConfig(UINT32 VinID)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to config image kernel - invalid VinID(%d)"
            PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_NG
    } else {
        UINT32 ViewZoneNum;
        UINT32 ViewZoneIDs[AMBA_DSP_MAX_VIEWZONE_NUM];

        PRN_VIN_ERR_LOG "---- %sConfigure VinID(%d) Image Kernel Setting%s ----"
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_2 PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_DBG1

        AmbaSvcWrap_MisraMemset(ViewZoneIDs, 0, sizeof(ViewZoneIDs));

        /* Get the view zone info */
        ViewZoneNum = 0U;
        PRetVal = SvcResCfg_GetFovIdxsInVinID(VinID, ViewZoneIDs, &ViewZoneNum); PRN_VIN_ERR_HDLR

        {
            AMBA_IK_MODE_CFG_s          ImgMode;
            AMBA_IK_ABILITY_s           ImgAbility;
            AMBA_IK_VIN_SENSOR_INFO_s   ImgSensorInfo;
            AMBA_IK_RGB_IR_s            ImgRgbIr;
            AMBA_IK_WINDOW_SIZE_INFO_s  ImgWinSizeInfo;
            AMBA_IK_VIN_ACTIVE_WINDOW_s ImgActWin;
            AMBA_IK_DZOOM_INFO_s        ImgDzoom;
#if defined(CONFIG_SOC_CV2FS)
            AMBA_IK_WARP_BUFFER_INFO_s  ImgWarpBufInfo;
#endif
            UINT32 Idx;

            for (Idx = 0U; Idx < ViewZoneNum; Idx ++) {

                AmbaSvcWrap_MisraMemset(&ImgAbility,     0, sizeof(ImgAbility));
                AmbaSvcWrap_MisraMemset(&ImgSensorInfo,  0, sizeof(ImgSensorInfo));
                AmbaSvcWrap_MisraMemset(&ImgRgbIr,       0, sizeof(ImgRgbIr));
                AmbaSvcWrap_MisraMemset(&ImgWinSizeInfo, 0, sizeof(ImgWinSizeInfo));
                AmbaSvcWrap_MisraMemset(&ImgActWin,      0, sizeof(ImgActWin));
                AmbaSvcWrap_MisraMemset(&ImgDzoom,       0, sizeof(ImgDzoom));
#if defined(CONFIG_SOC_CV2FS)
                AmbaSvcWrap_MisraMemset(&ImgWarpBufInfo, 0, sizeof(ImgWarpBufInfo));
#endif

                AmbaSvcWrap_MisraMemset(&ImgMode, 0, sizeof(ImgMode));
                ImgMode.ContextId = ViewZoneIDs[Idx];

                PRN_VIN_ERR_LOG "------ %sConfig Image Context(%d)%s ----"
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_3 PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 ImgMode.ContextId          PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_DBG1

                // Get current ik ability
                PRetVal = AmbaIK_GetContextAbility(&ImgMode, &ImgAbility); PRN_VIN_ERR_HDLR
                PRN_VIN_ERR_LOG "------ %sConfig Image Context Ability%s ----"
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_3 PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_DBG1
                PRN_VIN_ERR_LOG "         Pipe(%d), StillPipe(%d), VideoPipe(%d)"
                    PRN_VIN_ERR_ARG_UINT32 ImgAbility.Pipe      PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 ImgAbility.StillPipe PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 ImgAbility.VideoPipe PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_DBG1

                // get current ik sensor cfg
                {
                    PRetVal = AmbaIK_GetVinSensorInfo(&ImgMode, &ImgSensorInfo);
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;
                        PRN_VIN_ERR_LOG "Fail to config image kernel - get ik ctx(%d) sensor info fail!"
                            PRN_VIN_ERR_ARG_UINT32 ImgMode.ContextId PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_NG
                    } else {
                        if (ImgSensorInfo.SensorMode == SVC_IK_SENSOR_RGB_IR) {
                            PRetVal = AmbaIK_GetRgbIr(&ImgMode, &ImgRgbIr);
                            if (PRetVal != 0U) {
                                RetVal = SVC_NG;
                                PRN_VIN_ERR_LOG "Fail to config image kernel - get ik ctx(%d) rgb ir fail!"
                                    PRN_VIN_ERR_ARG_UINT32 ImgMode.ContextId PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_NG
                            }
                        }
                    }
                }

                // get current ik window size info
                {
                    PRetVal = AmbaIK_GetWindowSizeInfo(&ImgMode, &ImgWinSizeInfo);
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;
                        PRN_VIN_ERR_LOG "Fail to config image kernel - get ik ctx(%d) win size info fail!"
                            PRN_VIN_ERR_ARG_UINT32 ImgMode.ContextId PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_NG
                    }
                }

                // get current ik active window
                {
                    PRetVal = AmbaIK_GetVinActiveWin(&ImgMode, &ImgActWin);
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;
                        PRN_VIN_ERR_LOG "Fail to config image kernel - get ik ctx(%d) act win fail!"
                            PRN_VIN_ERR_ARG_UINT32 ImgMode.ContextId PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_NG
                    }
                }

                // get current ik dzoom info
                {
                    PRetVal = AmbaIK_GetDzoomInfo(&ImgMode, &ImgDzoom);
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;
                        PRN_VIN_ERR_LOG "Fail to config image kernel - get ik ctx(%d) dzoom fail!"
                            PRN_VIN_ERR_ARG_UINT32 ImgMode.ContextId PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_NG
                    }
                }

#if defined(CONFIG_SOC_CV2FS)
                // get current ik warp buffer info
                {
                    PRetVal = AmbaIK_GetWarpBufferInfo(&ImgMode, &ImgWarpBufInfo);
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;
                        PRN_VIN_ERR_LOG "Fail to config image kernel - get ik ctx(%d) warp buf info fail!"
                            PRN_VIN_ERR_ARG_UINT32 ImgMode.ContextId PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_NG
                    }
                }
#endif

                // Re-configure ik
                if (RetVal == SVC_OK) {
                    PRN_VIN_ERR_LOG "------ %sRe-init Image Kernel Context%s ----"
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_3 PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_DBG1

                    PRetVal = AmbaIK_InitContext(&ImgMode, &ImgAbility);
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;
                        PRN_VIN_ERR_LOG "Fail to config image kernel - initial ik ctx(%d) fail!"
                            PRN_VIN_ERR_ARG_UINT32 ImgMode.ContextId PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_NG
                    } else {

                        // set ik sensor cfg
                        PRN_VIN_ERR_LOG "------ %sSet IK Sensor Info%s ----"
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_3 PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_DBG1
                        PRetVal = AmbaIK_SetVinSensorInfo(&ImgMode, &ImgSensorInfo);
                        if (PRetVal != 0U) {
                            RetVal = SVC_NG;
                            PRN_VIN_ERR_LOG "Fail to config image kernel - set ik ctx(%d) sensor info fail!"
                                PRN_VIN_ERR_ARG_UINT32 ImgMode.ContextId PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_NG
                        }

                        // set ik RGB IR
                        if (ImgSensorInfo.SensorMode == SVC_IK_SENSOR_RGB_IR) {
                            PRN_VIN_ERR_LOG "------ %sSet IK RGB IR%s ----"
                                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_3 PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_DBG1
                            PRetVal = AmbaIK_SetRgbIr(&ImgMode, &ImgRgbIr);
                            if (PRetVal != 0U) {
                                RetVal = SVC_NG;
                                PRN_VIN_ERR_LOG "Fail to config image kernel - set ik ctx(%d) rgb ir fail!"
                                    PRN_VIN_ERR_ARG_UINT32 ImgMode.ContextId PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_NG
                            }
                        }

                        // set ik window size info
                        PRN_VIN_ERR_LOG "------ %sSet IK Window Size Info%s ----"
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_3 PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_DBG1
                        PRetVal = AmbaIK_SetWindowSizeInfo(&ImgMode, &ImgWinSizeInfo);
                        if (PRetVal != 0U) {
                            RetVal = SVC_NG;
                            PRN_VIN_ERR_LOG "Fail to config image kernel - set ik ctx(%d) win size info fail!"
                                PRN_VIN_ERR_ARG_UINT32 ImgMode.ContextId PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_NG
                        }

                        // set ik active win
                        PRN_VIN_ERR_LOG "------ %sSet IK Vin Active Window%s ----"
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_3 PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_DBG1
                        PRetVal = AmbaIK_SetVinActiveWin(&ImgMode, &ImgActWin);
                        if (PRetVal != 0U) {
                            RetVal = SVC_NG;
                            PRN_VIN_ERR_LOG "Fail to config image kernel - set ik ctx(%d) act win fail!"
                                PRN_VIN_ERR_ARG_UINT32 ImgMode.ContextId PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_NG
                        }

                        // set ik dzoom
                        PRN_VIN_ERR_LOG "------ %sSet IK DZoon Info%s ----"
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_3 PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_DBG1
                        PRetVal = AmbaIK_SetDzoomInfo(&ImgMode, &ImgDzoom);
                        if (PRetVal != 0U) {
                            RetVal = SVC_NG;
                            PRN_VIN_ERR_LOG "Fail to config image kernel - set ik ctx(%d) dzoom fail!"
                                PRN_VIN_ERR_ARG_UINT32 ImgMode.ContextId PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_NG
                        }

#ifdef CONFIG_SOC_CV2FS
                        // set ik warp buffer info
                        PRN_VIN_ERR_LOG "------ %sSet IK Warp Buffer Info%s ----"
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_3 PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_DBG1
                        PRetVal = AmbaIK_SetWarpBufferInfo(&ImgMode, &ImgWarpBufInfo);
                        if (PRetVal != 0U) {
                            RetVal = SVC_NG;
                            PRN_VIN_ERR_LOG "Fail to config image kernel - set ik ctx(%d) warp buf info fail!"
                                PRN_VIN_ERR_ARG_UINT32 ImgMode.ContextId PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_NG
                        }
#endif
                    }
                }

                if (RetVal != SVC_OK) {
                    break;
                } else {
                    PRN_VIN_ERR_LOG "Success to reset image kernel context(%d)"
                        PRN_VIN_ERR_ARG_UINT32 ImgMode.ContextId PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_OK
                }
            }
        }
    }

    AmbaMisra_TouchUnused(&RetVal);
}

static void SvcVinErrTask_RcvCalibConfig(UINT32 VinID)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();

    if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to config calib - invalid VinID(%d)"
            PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_NG
    } else if (pCfg == NULL) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to config calib - invalid res_cfg" PRN_VIN_ERR_NG
    } else {
        UINT32 ViewZoneIdx;
        UINT32 ViewZoneNum = 0U;
        UINT32 ViewZoneIDs[AMBA_DSP_MAX_VIEWZONE_NUM];
        UINT32 CalUpdBits;
        SVC_CALIB_CHANNEL_s CalibChan;
        AMBA_IK_MODE_CFG_s  ImgMode;

        PRN_VIN_ERR_LOG "---- %sConfigure VinID(%d) Calibration%s ----"
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_2 PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_DBG1

        AmbaSvcWrap_MisraMemset(ViewZoneIDs, 0, sizeof(ViewZoneIDs));
        PRetVal = SvcResCfg_GetFovIdxsInVinID(VinID, ViewZoneIDs, &ViewZoneNum);
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to config calib - get VinID(%d) view zone info fail!"
                PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        } else if (ViewZoneNum >= AMBA_DSP_MAX_VIEWZONE_NUM) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to config calib - invalid VinID(%d) ViewZoneNum(%d)"
                PRN_VIN_ERR_ARG_UINT32 VinID       PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 ViewZoneNum PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        } else {
            UINT32 SerDesIdx;

            for (ViewZoneIdx = 0U; ViewZoneIdx < ViewZoneNum; ViewZoneIdx ++) {
                if (ViewZoneIDs[ViewZoneIdx] >= AMBA_DSP_MAX_VIEWZONE_NUM) {
                    RetVal = SVC_NG;
                    PRN_VIN_ERR_LOG "Fail to config calib - invalid ViewZoneID(%d)"
                        PRN_VIN_ERR_ARG_UINT32 VinID                    PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 ViewZoneIDs[ViewZoneIdx] PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_NG
                } else {
                    if (pCfg->VinCfg[VinID].SerdesType == SVC_RES_SERDES_TYPE_NONE) {
                        SerDesIdx = 0U;
                    } else {
                        SerDesIdx = 0xdeadbeefU;
                        PRetVal = SvcResCfg_GetSerdesIdxOfFovIdx(ViewZoneIDs[ViewZoneIdx], &SerDesIdx); PRN_VIN_ERR_HDLR
                        if (SerDesIdx >= 4U) {
                            RetVal = SVC_NG;
                            PRN_VIN_ERR_LOG "Fail to config calib - Get invalid SerDesIdx(%d) by ViewZoneID(%d)"
                                PRN_VIN_ERR_ARG_UINT32 SerDesIdx                PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_UINT32 ViewZoneIDs[ViewZoneIdx] PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_NG
                        }
                    }

                    AmbaSvcWrap_MisraMemset(&CalibChan, 0, sizeof(CalibChan));
                    CalibChan.VinID    = VinID;
                    CalibChan.SensorID = SvcVinErrTask_BitGet(SerDesIdx);
                    CalibChan.FmtID    = SvcVinErrCtrl.RecoverTask.CurFmtID;

                    PRN_VIN_ERR_LOG "------ %sConfig Calib Channel%s ----"
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_3 PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_DBG1
                    PRN_VIN_ERR_LOG "         CalibChan .VinID(%d) .SensorID(0x%x) .FmtID(%d), ViewZoneID(%d)"
                        PRN_VIN_ERR_ARG_UINT32 CalibChan.VinID          PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 CalibChan.SensorID       PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 CalibChan.FmtID          PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 ViewZoneIDs[ViewZoneIdx] PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_DBG1

                    CalUpdBits = pCfg->FovCfg[ViewZoneIDs[ViewZoneIdx]].CalUpdBits;
                    if (CalUpdBits == 0U) {
                        CalUpdBits = (UINT32)SVC_CALIB_DEF_UPDATE_FLAG;
                    }
                    CalibChan.FovID = ViewZoneIDs[ViewZoneIdx];

                    AmbaSvcWrap_MisraMemset(&ImgMode, 0, sizeof(ImgMode));
                    ImgMode.ContextId = ViewZoneIDs[ViewZoneIdx];

                    PRN_VIN_ERR_LOG "------ %sConfig Calib IK Mode%s ----"
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_3 PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_DBG1
                    PRN_VIN_ERR_LOG "         ImgMode .ContextId(%d)"
                        PRN_VIN_ERR_ARG_UINT32 ImgMode.ContextId PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_DBG1

                    PRetVal = SvcCalibTask_UpdateTable(CalUpdBits, &CalibChan, &ImgMode); PRN_VIN_ERR_HDLR
                }

                if (RetVal != 0U) {
                    break;
                }
            }
        }
    }

    AmbaMisra_TouchUnused(&RetVal);
}

static void SvcVinErrTask_SerDesPreBootChk(UINT32 VinID)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (VinID < AMBA_DSP_MAX_VIN_NUM) {
#ifdef CONFIG_BUILD_MONFRW_SERDES
        AMBA_SENSOR_CHANNEL_s       SsChan;
        AMBA_SENSOR_SERDES_STATUS_s SsSerDesStatus[SVC_VIN_ERR_MAX_SER_NUM];

        AmbaSvcWrap_MisraMemset(&SsChan, 0, sizeof(SsChan));
        SsChan.VinID = VinID;
        PRetVal = SvcResCfg_GetSensorIDInVinID(SsChan.VinID, &(SsChan.SensorID));
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to check serdes link before booting - get VinID(%d) SensorID fail"
                PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        }

        AmbaSvcWrap_MisraMemset(SsSerDesStatus, 0, sizeof(SsSerDesStatus));
        if (RetVal == SVC_OK) {
            PRetVal = AmbaSensor_GetSerdesStatus(&SsChan, SsSerDesStatus);
        }

        if (PRetVal != SENSOR_ERR_NONE) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to check serdes link before booting - get VinID(%d) serdes status fail! ErrCode(0x%08x)"
                PRN_VIN_ERR_ARG_UINT32 VinID   PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 PRetVal PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        } else {
            UINT32 Idx, CurSerDesBits = 0U;

            for (Idx = 0U; Idx < SVC_VIN_ERR_MAX_SER_NUM; Idx ++) {
                if ( SsSerDesStatus[Idx].LinkLock == SVC_VIN_ERR_SS_SERDES_LOCK_OK) {
                    CurSerDesBits |= SvcVinErrTask_BitGet(Idx);
                } else if ( SsSerDesStatus[Idx].LinkLock == SVC_VIN_ERR_SS_SERDES_LOCK_NG ) {
                    PRN_VIN_ERR_LOG "Check SerDes link VinID(%d) Chan(%d) Link UnLock!"
                        PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 Idx   PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_API
                } else {
                    PRN_VIN_ERR_LOG "Check SerDes link VinID(%d) Chan(%d) Not Support!"
                        PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 Idx   PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_API
                }
            }

            PRN_VIN_ERR_LOG "SvcVinErrCtrl.SerDesSelectBits[%d](0x%x) / CurSerDesBits(0x%x)"
                PRN_VIN_ERR_ARG_UINT32 VinID                                 PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.SerDesSelectBits[VinID] PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 CurSerDesBits                         PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API
        }
#else
        AmbaMisra_TouchUnused(&PRetVal);
#endif
    }

    AmbaMisra_TouchUnused(&RetVal);
}

static void SvcVinErrTask_RecoverChk(UINT32 VinID)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to process vin recover - invalid VinID(%d)"
            PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_NG
    } else if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_RCV_CHK_OFF) != 0U) {
        PRN_VIN_ERR_LOG "VinID(%d) recover checking has been disable"
            PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_DBG1
    } else {
        UINT32 CurVinState = 0U;
        AMBA_KAL_EVENT_FLAG_INFO_s CurEventInfo;

        AmbaSvcWrap_MisraMemset(&CurEventInfo, 0, sizeof(CurEventInfo));
        PRetVal = AmbaKAL_EventFlagQuery(&(SvcVinErrCtrl.ErrState[VinID].VinStateFlag), &CurEventInfo);
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to check vin recover - query VinID(%d) state fail! ErrCode(0x%08x)"
                PRN_VIN_ERR_ARG_UINT32 VinID   PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 PRetVal PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        } else {
            CurVinState = CurEventInfo.CurrentFlags;
        }

        /* *
         * The vin recover is only support VIN base.
         * If specify viewzone is part channel of same VIN (ex: MAX96712 or MAX9296), these viewzone are same vin.
         *
         * Recover Case 1: Simple vin timeout.
         *     Condition : a. The specify vin is not virtual channel or physical part of MAX96712
         *                    The 'physical part' means VIN0, 4, 8, 11 of MAX96412 in CV5x
         *                 b. The specify vin is not depend on others vins.
         *                    ex: Output data to same display/enc stream with others vin.
         *                 b. The specify vin has timout.
         *                 c. All cable of specify vin are ready.
         *   Recover flow: Trigger specify vin recover only. It should not impact others alive vin.
         *
         * Recover Case 2: Multiple vin from same MAX96712 without effect on
         *     Condition : a. These vin's cable connect to same MAX96712.
         *                    ex: VIN0, 1, 2 and 3 are from same MAX96712 in CV5x
         *                        VIN4, 5, 6 and 7 are from same MAX96712 in CV5x
         *                        VIN8, 9 and 10 are from same MAX96712 in CV5x
         *                 b. These vins do not output to same display/enc stream.
         *                 c. All cable of specify MAX96712 are ready.
         *   Recover flow: Trigger vin recover with these vin at same process.
         *                 Because the MAX96712 need to re-configure at the same time.
         *                 Evan these vin is virtual vin case. And the timing should not dependency.
         *                 The SerDes - MAX96712 does not ready support re-link specify channel.
         *                 It means system need to re-configure fully MAX96712 at the same time.
         *
         * Recover Case 3: Exist difference vin output to same display/enc stream.
         *     Condition : a. All pipeline's vin cable are ready. No mater it is for above case_1 or case_2.
         *                    The system only support re-program liveview to itself.
         *   Recover flow: switch mode to same format_id
         *
         *
         * */

        /* vin recover */
        if ((CurVinState & SVC_VIN_ERR_VIN_STATE_RECOVER) == 0U) {
            UINT32 VinIdx, SerDesIdx;
            SVC_VIN_ERR_RECOVER_CMD_t RecoverCmd;

            AmbaSvcWrap_MisraMemset(&RecoverCmd, 0, sizeof(RecoverCmd));

            if ((CurVinState & SVC_VIN_ERR_VIN_STATE_TOUT) > 0U) {

                // Recover case 3
                if ((CurVinState & SVC_VIN_ERR_VIN_STATE_DEP_TOUT) > 0U) {
                    UINT32 UnRdyVinBits = 0U;

                    for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
                        if ((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(VinIdx)) > 0U) {
                            if (SVC_OK != SvcVinErrTask_IsVinLinkRdy(VinIdx)) {
                                UnRdyVinBits |= SvcVinErrTask_BitGet(VinIdx);
                                break;
                            }
                        }
                    }

                    if (UnRdyVinBits == 0U) {
                        RecoverCmd.CmdID = SVC_VIN_ERR_RCV_CMD_LIV;
                        RecoverCmd.VinSelectBits = SvcVinErrCtrl.VinSelectBits;
                        RecoverCmd.SerDesSelectBits = SvcVinErrCtrl.SerDesSelectBits[VinID];

                        PRN_VIN_ERR_LOG "Trigger recover - re-program liveview. " PRN_VIN_ERR_DBG1
                    }
                }
#ifndef CONFIG_SOC_CV2FS
                else if ((SvcVinErrCtrl.DependencyCtrl.Recover[VinID].DepVinBits & SvcVinErrTask_BitGet(VinID)) > 0U) {
                    UINT32 UnRdyVinBits = 0U;

                    for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
                        if ((SvcVinErrCtrl.DependencyCtrl.Recover[VinID].DepVinBits & SvcVinErrTask_BitGet(VinIdx)) > 0U) {
                            if (SVC_OK != SvcVinErrTask_IsVinLinkRdy(VinIdx)) {
                                UnRdyVinBits |= SvcVinErrTask_BitGet(VinIdx);
                                break;
                            }
                        }
                    }

                    if (UnRdyVinBits == 0U) {
                        RecoverCmd.CmdID = SVC_VIN_ERR_RCV_CMD_VIN;
                        RecoverCmd.VinSelectBits = SvcVinErrCtrl.DependencyCtrl.Recover[VinID].DepVinBits;
                        RecoverCmd.SerDesSelectBits = SvcVinErrCtrl.SerDesSelectBits[VinID];

                        PRN_VIN_ERR_LOG "Trigger recover - multiple vin recover. RcvVinBits(0x%08X)"
                        PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.DependencyCtrl.Recover[VinID].DepVinBits PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_DBG1
                    }

                } else {
                    RecoverCmd.CmdID = SVC_VIN_ERR_RCV_CMD_VIN;
                    RecoverCmd.VinSelectBits = SvcVinErrTask_BitGet(VinID);
                    RecoverCmd.SerDesSelectBits = SvcVinErrCtrl.SerDesSelectBits[VinID];

                    PRN_VIN_ERR_LOG "Trigger recover - multiple vin recover. RcvVinBits(0x%08X)"
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.DependencyCtrl.Recover[VinID].DepVinBits PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_DBG1
                }
#else
                else {
                    UINT32 VinNum = 0U;
                    UINT32 DepVinLinkLockBits;
                    UINT32 DepUnRdyVinBits = 0U;
                    UINT32 UnRdyVinBits = 0U;

                    for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
                        if ((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(VinIdx)) > 0U) {
                            VinNum ++;

                            if (VinID != VinIdx) {
                                DepVinLinkLockBits = 0U;
                                for (SerDesIdx = 0U; SerDesIdx < SVC_VIN_ERR_MAX_SER_NUM; SerDesIdx ++) {
                                    if ((SvcVinErrCtrl.SerDesSelectBits[VinIdx] & SvcVinErrTask_BitGet(SerDesIdx)) > 0U) {
                                        if ((SvcVinErrTask_GetSerDesState(VinIdx, SerDesIdx) & SVC_VIN_ERR_SER_STAT_LINK_LOCK) > 0U) {
                                            DepVinLinkLockBits |= SvcVinErrTask_BitGet(SerDesIdx);
                                        }
                                    }
                                }

                                if (DepVinLinkLockBits != SvcVinErrCtrl.SerDesSelectBits[VinIdx]) {
                                    DepUnRdyVinBits |= SvcVinErrTask_BitGet(VinIdx);
                                }
                            } else {
                                for (SerDesIdx = 0U; SerDesIdx < SVC_VIN_ERR_MAX_SER_NUM; SerDesIdx ++) {
                                    if ((SvcVinErrCtrl.SerDesSelectBits[VinIdx] & SvcVinErrTask_BitGet(SerDesIdx)) > 0U) {
                                        if ((SvcVinErrTask_GetSerDesState(VinIdx, SerDesIdx) & SVC_VIN_ERR_SER_STAT_LINK_LOCK) == 0U) {
                                            UnRdyVinBits |= SvcVinErrTask_BitGet(SerDesIdx);
                                        }
                                    }
                                }
                            }
                        }
                    }

                    if ((VinNum > 1U) && (DepUnRdyVinBits == 0U)) {
                        RecoverCmd.CmdID = SVC_VIN_ERR_RCV_CMD_LIV;
                        RecoverCmd.VinSelectBits = SvcVinErrCtrl.VinSelectBits;
                        RecoverCmd.SerDesSelectBits = SvcVinErrCtrl.SerDesSelectBits[VinID];
                    } else if (UnRdyVinBits == 0U) {
                        RecoverCmd.CmdID = SVC_VIN_ERR_RCV_CMD_VIN;
                        RecoverCmd.VinSelectBits = SvcVinErrTask_BitGet(VinID);
                        RecoverCmd.SerDesSelectBits = SvcVinErrCtrl.SerDesSelectBits[VinID];
                    } else {
                        // misra-c
                    }
                }
#endif
            }

            if (RecoverCmd.SerDesSelectBits > 0U) {
                for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
                    if ((RecoverCmd.VinSelectBits & SvcVinErrTask_BitGet(VinIdx)) > 0U) {
                        /* Update vin state flag */
                        PRetVal = AmbaKAL_EventFlagSet(&(SvcVinErrCtrl.ErrState[VinID].VinStateFlag), SVC_VIN_ERR_VIN_STATE_RECOVER); PRN_VIN_ERR_HDLR
                    }
                }

                /* Send cmd */
                PRetVal = AmbaKAL_MsgQueueSend(&(SvcVinErrCtrl.RecoverTask.Que), &RecoverCmd, 5U);
                if (PRetVal != 0U) {
                    PRN_VIN_ERR_LOG "Fail to prepare VinID(%d) recover command"
                        PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_NG
                }
            }

            /* Update pre link/video state */
            SvcVinErrCtrl.ErrState[VinID].LinkLockPreState = 0U;
            SvcVinErrCtrl.ErrState[VinID].VideoLockPreState = 0U;
            for (SerDesIdx = 0U; SerDesIdx < SVC_VIN_ERR_MAX_SER_NUM; SerDesIdx ++) {
                if ((SvcVinErrCtrl.SerDesSelectBits[VinID] & SvcVinErrTask_BitGet(SerDesIdx)) > 0U) {
                    if ((SvcVinErrTask_GetSerDesState(VinID, SerDesIdx) & SVC_VIN_ERR_SER_STAT_LINK_LOCK) > 0U) {
                        SvcVinErrCtrl.ErrState[VinID].LinkLockPreState |= SvcVinErrTask_BitGet(SerDesIdx);
                    }
                    if ((SvcVinErrTask_GetSerDesState(VinID, SerDesIdx) & SVC_VIN_ERR_SER_STAT_VID_LOCK) > 0U) {
                        SvcVinErrCtrl.ErrState[VinID].VideoLockPreState |= SvcVinErrTask_BitGet(SerDesIdx);
                    }
                }
            }

        }
    }

    AmbaMisra_TouchUnused(&RetVal);
}

static void* SvcVinErrTask_ErrCtrlTaskEntry(void* EntryArg)
{
    UINT32 PRetVal, Idx;
    SVC_VIN_ERR_CTRL_CMD_t ErrCtrlCmd;
    UINT32 VoutLatency;
    SVC_VIN_ERR_LINK_UPD_s *pLinkUpd;
    SVC_VIN_ERR_SENS_UPD_s *pSensorFreezeUpd;
    SVC_VIN_ERR_RAW_FRZ_UPD_s *pRawDataUpd;
    SVC_VIN_ERR_VOUT_LTN_UPD_s *pVoutLatencyUpd;
    SVC_VIN_ERR_FOV_LTN_UPD_s *pFovLatencyUpd;
    const void *pCmdData = ErrCtrlCmd.CmdData;
    ULONG ArgVal = 0U;

    AmbaMisra_TypeCast(&(pLinkUpd),         &(pCmdData));
    AmbaMisra_TypeCast(&(pSensorFreezeUpd), &(pCmdData));
    AmbaMisra_TypeCast(&(pRawDataUpd),      &(pCmdData));
    AmbaMisra_TypeCast(&(pVoutLatencyUpd),  &(pCmdData));
    AmbaMisra_TypeCast(&(pFovLatencyUpd),   &(pCmdData));

    AmbaMisra_TouchUnused(EntryArg);
    AmbaSvcWrap_MisraMemcpy(&ArgVal, EntryArg, sizeof(ULONG));

    while (ArgVal != 0xCafeU) {
        AmbaSvcWrap_MisraMemset(&ErrCtrlCmd, 0, sizeof(ErrCtrlCmd));

        PRetVal = AmbaKAL_MsgQueueReceive(&(SvcVinErrCtrl.ErrCtrlTask.Que), &ErrCtrlCmd, AMBA_KAL_WAIT_FOREVER);
        if (PRetVal == 0U) {
            if (ErrCtrlCmd.CmdID == SVC_VIN_ERR_CTRL_CMD_LNK_UPD) {
                SvcVinErrTask_LinkStateUpd(pLinkUpd->VinID,
                                           pLinkUpd->LinkLockState,
                                           pLinkUpd->VideoLockState);
            } else if (ErrCtrlCmd.CmdID == SVC_VIN_ERR_CTRL_CMD_DEP_UPD) {
                SvcVinErrTask_DependencyUpd(pLinkUpd->VinID);
            } else if (ErrCtrlCmd.CmdID == SVC_VIN_ERR_CTRL_CMD_VIN_TOUT) {
                SvcVinErrTask_VinTimeoutUpd(pLinkUpd->VinID);
            } else if (ErrCtrlCmd.CmdID == SVC_VIN_ERR_CTRL_CMD_SENS_UPD) {

                PRN_VIN_ERR_LOG "Update FovID(%d) Raw Freeze State(%d)"
                    PRN_VIN_ERR_ARG_UINT32 pSensorFreezeUpd->FovID  PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 pSensorFreezeUpd->CdZero PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_API

                SvcVinErrTask_FovFreezeUpd(pSensorFreezeUpd->FovID, pSensorFreezeUpd->CdZero);
            } else if (ErrCtrlCmd.CmdID == SVC_VIN_ERR_CTRL_CMD_RAW_UPD) {

                PRN_VIN_ERR_LOG "VinID(%s%d%s) err control - raw data ready %s%04u%s / %s%04u%s / %s%04u%s, %s%p%s !"
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1  PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 pRawDataUpd->VinID          PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END      PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0  PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 pRawDataUpd->Pitch          PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END      PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0  PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 pRawDataUpd->Width          PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END      PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0  PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 pRawDataUpd->Height         PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END      PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0  PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CPOINT pRawDataUpd->pRawBuf        PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END      PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_DBG2

                PRetVal = SvcVinErrTask_RawFrzProc(pRawDataUpd); PRN_VIN_ERR_HDLR

            } else if (ErrCtrlCmd.CmdID == SVC_VIN_ERR_CTRL_CMD_VLTY_UPD) {

                if ( ( SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_LOG_DBG3 ) > 0U ) {
                    for (Idx = 0U; Idx < AMBA_DSP_MAX_VIEWZONE_NUM; Idx ++) {
                        if ((pVoutLatencyUpd->FovSelectBits & SvcVinErrTask_BitGet(Idx)) > 0U) {

                            VoutLatency = (UINT32)(pVoutLatencyUpd->FovVoutLatency[Idx]); VoutLatency &= 0xFFFFU;
                            PRN_VIN_ERR_LOG "Update VoutID(%s%d%s), FovID(%s%d%s / 0x%x), SeqCnt(%s0x%llx%s), latency(%s%03d%s)"
                                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0     PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_UINT32 pVoutLatencyUpd->VoutID        PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END         PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1     PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_UINT32 Idx                            PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END         PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_UINT32 pVoutLatencyUpd->FovSelectBits PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0     PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_UINT64 pVoutLatencyUpd->DispSeqCnt
                                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END         PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0     PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_UINT32 VoutLatency                    PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END         PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_DBG3
                        }
                    }

                    PRN_VIN_ERR_LOG "Update VoutID(%s%d%s), DispYUV-to-VOUT(%s%d%s)"
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0     PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 pVoutLatencyUpd->VoutID        PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END         PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0     PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 pVoutLatencyUpd->DispLatency   PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END         PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_DBG3
                }

                PRetVal = SvcVinErrTask_VoutLatencyComp(pVoutLatencyUpd); PRN_VIN_ERR_HDLR

            } else if (ErrCtrlCmd.CmdID == SVC_VIN_ERR_CTRL_CMD_FLTY_UPD) {

                if ( ( SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_LOG_DBG4 ) > 0U ) {
                    if (pFovLatencyUpd->FovID < AMBA_DSP_MAX_VIEWZONE_NUM) {
                        PRN_VIN_ERR_LOG "Update FovID(%s%d%s), SeqCnt(%s0x%llx%s), latency(%s%03d%s)"
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1     PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 pFovLatencyUpd->FovID          PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END         PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0     PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT64 pFovLatencyUpd->YuvSeqNum
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END         PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0     PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 pFovLatencyUpd->Latency        PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END         PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_DBG4
                    }
                }

                PRetVal = SvcVinErrTask_FovLatencyComp(pFovLatencyUpd); PRN_VIN_ERR_HDLR

            } else {
                /* Do nothing */
            }
        }
        
        AmbaMisra_TouchUnused(&ArgVal);
    }

    AmbaMisra_TouchUnused(pLinkUpd);
    AmbaMisra_TouchUnused(pSensorFreezeUpd);
    AmbaMisra_TouchUnused(pRawDataUpd);
    AmbaMisra_TouchUnused(pVoutLatencyUpd);
    AmbaMisra_TouchUnused(pFovLatencyUpd);

    return NULL;
}

static void SvcVinErrTask_RcvProcVin(SVC_VIN_ERR_RECOVER_CMD_t *pCmd)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (pCmd == NULL) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to process vin recover - invalid recover cmd" PRN_VIN_ERR_NG
        AmbaMisra_TouchUnused(pCmd);
    } else {
        UINT32 VinID;
        UINT32 WaitLivFlag = 0U;
        UINT32 ProcIdx;
        SVC_VIN_ERR_RCV_CTRL_s *pRcvCtrl = (SVC_VIN_ERR_RCV_CTRL_s *)&(SvcVinErrCtrl.RecoverTask.RcvCtrl);

        AmbaMisra_TouchUnused(pRcvCtrl);

        AmbaSvcWrap_MisraMemset(&(SvcVinErrCtrl.RecoverTask.VinSrcCfg), 0, sizeof(SVC_VIN_SRC_INIT_s));
        SvcInfoPack_VinSrcInit(&(SvcVinErrCtrl.RecoverTask.VinSrcCfg));

        for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
            if ((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(VinID)) > 0U) {
                if ((pCmd->VinSelectBits & SvcVinErrTask_BitGet(VinID)) > 0U) {

                    WaitLivFlag |= SvcVinErrTask_BitGet(VinID);

                    // Update the OSD
                    SvcVinErrTask_OsdUpdate(VinID);

                    // Pause raw freeze
                    SvcVinErrTask_RawFrzVinEnable(VinID, 0U);

                    // Lock OSD update mechanism
                    SvcVinErrTask_OsdUpdateLock(VinID, 1U);

                    PRN_VIN_ERR_LOG " " PRN_VIN_ERR_API
                    PRN_VIN_ERR_LOG "%sRecover VinID(%d) Start%s"
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_4 PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_API
                }
            }
        }

        for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
            if ((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(VinID)) > 0U) {
                if ((pCmd->VinSelectBits & SvcVinErrTask_BitGet(VinID)) > 0U) {

                    PRN_VIN_ERR_LOG "---- %sEnable VinID(%d) Statistic Task%s ----"
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_2 PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_DBG1
                    SvcVinErrTask_StatisticUpd(VinID, 1U);

                    PRN_VIN_ERR_LOG "---- %sEnable VinID(%d) Image Task%s ----"
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_2 PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_DBG1
                    SvcVinErrTask_ImgFrwkSyncUpd(VinID, 1U);
                }
            }
        }

        // Start recover flow
        PRetVal = SVC_OK;
        for (ProcIdx = 0U; ProcIdx < SVC_VIN_ERR_RCV_PROC_NUM; ProcIdx ++) {
            if ((pRcvCtrl->pSubProc[ProcIdx].Enable > 0U) &&
                (pRcvCtrl->pSubProc[ProcIdx].pFunc != NULL)) {
                if ((pRcvCtrl->ProcSelectBits[pCmd->CmdID] & SvcVinErrTask_BitGet(ProcIdx)) > 0U) {
                    PRetVal = (pRcvCtrl->pSubProc[ProcIdx].pFunc)(PRetVal, pCmd, &(pRcvCtrl->pSubProc[ProcIdx]));
                }
            }
        }

        // Un-Lock OSD update mechanism
        for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
            if ((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(VinID)) > 0U) {
                if ((pCmd->VinSelectBits & SvcVinErrTask_BitGet(VinID)) > 0U) {
                    SvcVinErrTask_OsdUpdateLock(VinID, 0U);
                }
            }
        }

        // wait vin status
        if (PRetVal == 0U) {
            PRN_VIN_ERR_LOG "---- %sWait Vin liveview status%s, 0x%08X ----"
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_2 PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 WaitLivFlag                PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_DBG1

            PRetVal = AmbaKAL_EventFlagGet(&(SvcVinErrCtrl.VinLivFlag)
                                          , WaitLivFlag
                                          , AMBA_KAL_FLAGS_ALL
                                          , AMBA_KAL_FLAGS_CLEAR_NONE
                                          , &RetVal
                                          , SVC_VIN_ERR_VIN_RECOVER_TOUT);
            if (PRetVal != 0U) {
                PRN_VIN_ERR_LOG "Wait vin liveview status timeout(%u). ActFlag(0x%x)"
                    PRN_VIN_ERR_ARG_UINT32 SVC_VIN_ERR_VIN_RECOVER_TOUT PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 RetVal                       PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_NG
            }
        }

        if (PRetVal == 0U) {
            for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
                if ((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(VinID)) > 0U) {
                    if ((pCmd->VinSelectBits & SvcVinErrTask_BitGet(VinID)) > 0U) {

                        // Reset the link lock pre-state
                        SvcVinErrCtrl.ErrState[VinID].LinkLockPreState = 0xFFFFFFFFU;

                        // Clear timeout flag
                        PRetVal = AmbaKAL_EventFlagClear(&(SvcVinErrCtrl.ErrState[VinID].VinStateFlag), SVC_VIN_ERR_VIN_STATE_TOUT); PRN_VIN_ERR_HDLR

                        PRN_VIN_ERR_LOG "%sRecover VoutID(%d) success%s"
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_4 PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_API
                    }
                }
            }
        }

        for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
            if ((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(VinID)) > 0U) {
                if ((pCmd->VinSelectBits & SvcVinErrTask_BitGet(VinID)) > 0U) {

                    // Clear recover flag
                    PRetVal = AmbaKAL_EventFlagClear(&(SvcVinErrCtrl.ErrState[VinID].VinStateFlag), SVC_VIN_ERR_VIN_STATE_RECOVER); PRN_VIN_ERR_HDLR

                    // Reset raw freeze
                    PRetVal = SvcVinErrTask_RawFrzVinReset(VinID); PRN_VIN_ERR_HDLR

                    // Pause raw freeze
                    SvcVinErrTask_RawFrzVinEnable(VinID, 1U);

                    // Update the OSD
                    SvcVinErrTask_OsdUpdate(VinID);

                    // Update Fov state
                    SvcVinErrTask_FovStateUpd(VinID);

                    // Reset vout yuv freeze
                    SvcVinErrTask_VoutFrzReset(VinID);

                }
            }
        }
    }

    AmbaMisra_TouchUnused(&RetVal);
}

static void SvcVinErrTask_RcvProcSerDesLink(SVC_VIN_ERR_RECOVER_CMD_t *pCmd)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();

    if (pCmd == NULL) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to process vin recover - input recover command should not null!" PRN_VIN_ERR_NG
        AmbaMisra_TouchUnused(pCmd);
    } else if (pCfg == NULL) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to process vin recover - invalid cfg" PRN_VIN_ERR_NG
    } else {
        UINT32 SerIdx, VinSrcIdx, VinID;
        AMBA_SENSOR_CHANNEL_s SsChan;
        AMBA_SENSOR_CONFIG_s  SsCfg;
        const SVC_VIN_SRC_CFG_s *pCfgArr = NULL;
        SVC_VIN_ERR_STATE_s *pErrState;

        /* Configure vin source */
        AmbaSvcWrap_MisraMemset(&(SvcVinErrCtrl.RecoverTask.VinSrcCfg), 0, sizeof(SVC_VIN_SRC_INIT_s));
        PRN_VIN_ERR_LOG "---- %sGet Vin Source Info%s ----"
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_2 PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_DBG1
        SvcInfoPack_VinSrcInit(&(SvcVinErrCtrl.RecoverTask.VinSrcCfg));

        for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
            if ((pCmd->VinSelectBits & SvcVinErrTask_BitGet(VinID)) > 0U) {

                pErrState = &(SvcVinErrCtrl.ErrState[VinID]);

                // Update the OSD
                SvcVinErrTask_OsdUpdate(VinID);

                // Pause raw freeze
                SvcVinErrTask_RawFrzVinEnable(VinID, 0U);

                PRN_VIN_ERR_LOG "%sRe-link VinID(%d) SerDes(0x%x) start%s"
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_4 PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 pCmd->SerDesSelectBits     PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_API

                SvcVinErrTask_OsdUpdateLock(VinID, 1U);

                PRN_VIN_ERR_LOG "---- %sConfig VinID(%d) Sensor Driver%s ----"
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_2 PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_DBG1

                for ( VinSrcIdx = 0U; VinSrcIdx < SvcVinErrCtrl.RecoverTask.VinSrcCfg.CfgNum; VinSrcIdx ++ ) {

                    pCfgArr = &(SvcVinErrCtrl.RecoverTask.VinSrcCfg.InitCfgArr[VinSrcIdx].Config);

                    if ( (pCfgArr->VinID == VinID) &&
                         (pCfgArr->SrcType == SVC_VIN_SRC_SENSOR)) {

                        /* Disable serdes link update */
                        SvcVinErrTask_LinkStateUpdEna(VinID, SVC_VIN_ERR_LNK_UPD_OFF);

                        /* Re-configure sensor */
                        AmbaSvcWrap_MisraMemset(&SsChan, 0, sizeof(SsChan));
                        SsChan.VinID    = pCfgArr->VinID;
                        SsChan.SensorID = pCfgArr->SrcBits & 0xFU;

                        for (SerIdx = 0U; SerIdx < SVC_VIN_ERR_MAX_SER_NUM; SerIdx ++) {
                            if ((pCmd->SerDesSelectBits & SvcVinErrTask_BitGet(SerIdx)) > 0U) {
                                SsChan.SensorID |= SvcVinErrTask_BitGet((SerIdx + 1U) * 4U);
                            }
                        }

                        AmbaSvcWrap_MisraMemset(&SsCfg, 0, sizeof(SsCfg));
                        SsCfg.ModeID = pCfgArr->SrcMode | 0x10000000U;

                        PRN_VIN_ERR_LOG "       SsChan.VinID(0x%x) SensorID(0x%x), SsCfg.ModeID(0x%x)"
                            PRN_VIN_ERR_ARG_UINT32 SsChan.VinID    PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 SsChan.SensorID PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 SsCfg.ModeID    PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_DBG1

                        PRetVal = AmbaSensor_Config(&SsChan, &SsCfg);
                        if (PRetVal != 0U) {
                            PRN_VIN_ERR_LOG "Fail to process vin recover - configure VinID(%d) sensor fail! ErrCode(0x%08x)"
                                PRN_VIN_ERR_ARG_UINT32 VinID   PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_UINT32 PRetVal PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_NG
                        }

                        /* Enable serdes link update */
                        SvcVinErrTask_LinkStateUpdEna(VinID, SVC_VIN_ERR_LNK_UPD_ON);
                    }
                }

                SvcVinErrTask_OsdUpdateLock(VinID, 0U);

                /* Wait vin state */
                PRetVal = AmbaKAL_EventFlagGet(&(pErrState->VinStateFlag),
                                               SVC_VIN_ERR_VIN_STATE_LIV,
                                               AMBA_KAL_FLAGS_ALL,
                                               AMBA_KAL_FLAGS_CLEAR_NONE,
                                               &RetVal,
                                               SVC_VIN_ERR_VIN_RECOVER_TOUT);
                if (PRetVal != 0U) {
                    PRN_VIN_ERR_LOG "Wait VinID(%d) liveview timeout(%u). ActFlag(0x%x)"
                        PRN_VIN_ERR_ARG_UINT32 VinID                        PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 SVC_VIN_ERR_VIN_RECOVER_TOUT PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 RetVal                       PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_NG
                } else {

                    PRetVal = AmbaKAL_EventFlagClear(&(pErrState->VinStateFlag), SVC_VIN_ERR_VIN_STATE_TOUT); PRN_VIN_ERR_HDLR

                    PRN_VIN_ERR_LOG "%sRe-link VinID(%d) SerDes(0x%x) done%s"
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_4 PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 pCmd->SerDesSelectBits     PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_API
                }

                PRetVal = AmbaKAL_EventFlagClear(&(pErrState->VinStateFlag), SVC_VIN_ERR_VIN_STATE_RECOVER); PRN_VIN_ERR_HDLR

                // Reset raw freeze
                PRetVal = SvcVinErrTask_RawFrzVinReset(VinID); PRN_VIN_ERR_HDLR

                // Pause raw freeze
                SvcVinErrTask_RawFrzVinEnable(VinID, 1U);

                SvcVinErrTask_OsdUpdate(VinID);
            }
        }

    }

    AmbaMisra_TouchUnused(&RetVal);
}

static void SvcVinErrTask_RcvProcLiveview(SVC_VIN_ERR_RECOVER_CMD_t *pCmd)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();

    if (pCmd == NULL) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to process liveview re-config - invalid recover cmd" PRN_VIN_ERR_NG
        AmbaMisra_TouchUnused(pCmd);
    } else if (pCfg == NULL) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to process vin recover - invalid cfg" PRN_VIN_ERR_NG
    } else {
        UINT32 VinIdx;
        SVC_VIN_ERR_RCV_CTRL_s *pRcvCtrl = (SVC_VIN_ERR_RCV_CTRL_s *)&(SvcVinErrCtrl.RecoverTask.RcvCtrl);
        AMBA_KAL_EVENT_FLAG_INFO_s CurStateFlag;

        for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
            if ((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(VinIdx)) > 0U) {

                // Update the OSD
                SvcVinErrTask_OsdUpdate(VinIdx);

                // Pause raw freeze
                SvcVinErrTask_RawFrzVinEnable(VinIdx, 0U);
            }
        }

        PRN_VIN_ERR_LOG " " PRN_VIN_ERR_API
        PRN_VIN_ERR_LOG "%sRe-Program Liveview%s"
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_4 PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_API

        // Lock OSD update mechanism
        for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
            if ((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(VinIdx)) > 0U) {
                SvcVinErrTask_OsdUpdateLock(VinIdx, 1U);
            }
        }

        // Resume statistic mask
        SvcVinErrTask_StatisticVZUpd(0xFFFFFFFFU, 1U);

        // Resume imgfrw
        for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
            if ((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(VinIdx)) > 0U) {
                SvcVinErrTask_ImgFrwkSyncUpd(VinIdx, 1U);
            }
        }


        // Start proc 'switch mode'
        PRetVal = SvcFlowControl_Exec("liveview_switch");
        if (PRetVal != 0U) {
            PRN_VIN_ERR_LOG "SvcFlowControl_Exec('liveview_switch') fail! ErrCode(0x%08x)"
                PRN_VIN_ERR_ARG_UINT32 VinIdx  PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 PRetVal PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        }

        // Un-Lock OSD update mechanism
        for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
            if ((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(VinIdx)) > 0U) {
                SvcVinErrTask_OsdUpdateLock(VinIdx, 0U);
            }
        }

        // Wait vin state
        if (PRetVal == 0U) {
            UINT32 WaitTime = SVC_VIN_ERR_VIN_RECOVER_TOUT, WaitTimeStep = 5U;
            UINT32 IsExitLoop;
            UINT32 CurLivBits = 0U;

            // Check vin liveview ready
            do {
                PRetVal = 0U;

                IsExitLoop = 0U;

                for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
                    if (((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(VinIdx)) > 0U) &&
                        ((CurLivBits & SvcVinErrTask_BitGet(VinIdx)) == 0U)) {

                        AmbaSvcWrap_MisraMemset(&CurStateFlag, 0, sizeof(CurStateFlag));
                        PRetVal = AmbaKAL_EventFlagQuery(&(SvcVinErrCtrl.ErrState[VinIdx].VinStateFlag), &CurStateFlag);
                        if (PRetVal != 0U) {
                            RetVal = SVC_NG;
                            PRN_VIN_ERR_LOG "Query VinID(%d) state flag fail! ErrCode(0x%08x)"
                                PRN_VIN_ERR_ARG_UINT32 VinIdx  PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_UINT32 PRetVal PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_NG
                            break;
                        } else {
                            if ((CurStateFlag.CurrentFlags & SVC_VIN_ERR_VIN_STATE_LIV) > 0U) {
                                CurLivBits |= SvcVinErrTask_BitGet(VinIdx);
                                // Get one of vin liveview ready, change the wait time to 2x WaitTimeStep
                                WaitTime = WaitTimeStep * 2U;
                            }
                        }
                    }
                }

                WaitTime -= WaitTimeStep;
                if (CurLivBits == SvcVinErrCtrl.VinSelectBits) {
                    IsExitLoop = 1U;
                } else if (RetVal != 0U) {
                    IsExitLoop = 1U;
                } else if (WaitTime == 0U) {
                    IsExitLoop = 1U;
                } else {
                    PRetVal = AmbaKAL_TaskSleep(WaitTimeStep); PRN_VIN_ERR_HDLR
                }

            } while ((WaitTime > 0U) && (IsExitLoop == 0U));

            if (CurLivBits > 0U) {

                // Prepare the VinLost event for not ready vin
                if (CurLivBits != SvcVinErrCtrl.VinSelectBits) {
                    UINT32 UnRdyVinBits = SvcVinErrCtrl.VinSelectBits & (~CurLivBits);

                    for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
                        if ((UnRdyVinBits & SvcVinErrTask_BitGet(VinIdx)) > 0U) {
                            SvcVinErrCtrl.DependencyCtrl.UpdCtrl[VinIdx] = SVC_VIN_ERR_DEP_UPD_BY_RESTART;
                            SvcVinErrTask_DependencyUpd(VinIdx);

                            PRN_VIN_ERR_LOG "Wait VinID(%d) liveview timeout(%u)."
                                PRN_VIN_ERR_ARG_UINT32 VinIdx                       PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_UINT32 SVC_VIN_ERR_VIN_RECOVER_TOUT PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_NG
                        }
                    }
                }

                // Clear and update the ready vin flag/state
                for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
                    if ((CurLivBits & SvcVinErrTask_BitGet(VinIdx)) > 0U) {
                        // Reset the link lock pre-state
                        SvcVinErrCtrl.ErrState[VinIdx].LinkLockPreState = 0xFFFFFFFFU;

                        // Clear timeout flag
                        PRetVal = AmbaKAL_EventFlagClear(&(SvcVinErrCtrl.ErrState[VinIdx].VinStateFlag), SVC_VIN_ERR_VIN_STATE_TOUT); PRN_VIN_ERR_HDLR
                        PRetVal = AmbaKAL_EventFlagClear(&(SvcVinErrCtrl.ErrState[VinIdx].VinStateFlag), SVC_VIN_ERR_VIN_STATE_DEP_TOUT); PRN_VIN_ERR_HDLR
                    }
                }
            }
        }

        // Clear recover flag
        for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
            if ((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(VinIdx)) > 0U) {
                PRetVal = AmbaKAL_EventFlagClear(&(SvcVinErrCtrl.ErrState[VinIdx].VinStateFlag), SVC_VIN_ERR_VIN_STATE_RECOVER); PRN_VIN_ERR_HDLR
            }
        }

        AmbaMisra_TouchUnused(pRcvCtrl);

        for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
            if ((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(VinIdx)) > 0U) {

                // Reset raw freeze
                PRetVal = SvcVinErrTask_RawFrzVinReset(VinIdx); PRN_VIN_ERR_HDLR

                // Resume raw freeze
                SvcVinErrTask_RawFrzVinEnable(VinIdx, 1U);

                // Update the OSD
                SvcVinErrTask_OsdUpdate(VinIdx);
            }
        }

        PRN_VIN_ERR_LOG " " PRN_VIN_ERR_API
        PRN_VIN_ERR_LOG "%sRe-Program Liveview Done%s"
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_4 PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_API
    }

    AmbaMisra_TouchUnused(&RetVal);
}

static void* SvcVinErrTask_RcvTaskEntry(void* EntryArg)
{
    UINT32 PRetVal;
    SVC_VIN_ERR_RECOVER_CMD_t RecoverCmd;
    ULONG ArgVal = 0U;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaSvcWrap_MisraMemcpy(&ArgVal, EntryArg, sizeof(ULONG));

    while (ArgVal != 0xCafeU) {
        AmbaSvcWrap_MisraMemset(&RecoverCmd, 0, sizeof(RecoverCmd));
        PRetVal = AmbaKAL_MsgQueueReceive(&(SvcVinErrCtrl.RecoverTask.Que), &RecoverCmd, AMBA_KAL_WAIT_FOREVER);
        if (PRetVal == 0U) {
            if (RecoverCmd.CmdID == SVC_VIN_ERR_RCV_CMD_VIN) {
                SvcVinErrTask_RcvProcVin(&RecoverCmd);
            } else if (RecoverCmd.CmdID == SVC_VIN_ERR_RCV_CMD_SERDES) {
                SvcVinErrTask_RcvProcSerDesLink(&RecoverCmd);
            } else if (RecoverCmd.CmdID == SVC_VIN_ERR_RCV_CMD_LIV) {
                SvcVinErrTask_RcvProcLiveview(&RecoverCmd);
            } else {
                /* Do nothing */
            }
        }
        
        AmbaMisra_TouchUnused(&ArgVal);
    }

    return NULL;
}

static void SvcVinErrTask_FovFreezeUpd(UINT32 FovID, UINT32 IsFreeze)
{
    UINT32 PRetVal;

    if (FovID < AMBA_DSP_MAX_VIEWZONE_NUM) {
        UINT32 VinID = 0xFFFFFFFFU;
        UINT32 OriState = 0U, IsUpd = 0U;

        PRetVal = SvcResCfg_GetVinIDOfFovIdx(FovID, &VinID); PRN_VIN_ERR_HDLR

        if (VinID < AMBA_DSP_MAX_VIN_NUM) {
            OriState = SvcVinErrCtrl.ErrState[VinID].FovFreezeState;

            if (IsFreeze > 0U) {
                if ((SvcVinErrCtrl.ErrState[VinID].FovFreezeState & ((UINT32)SVC_VIN_ERR_FOV_STAT_FREZE << FovID)) == 0U) {
                    SvcVinErrCtrl.ErrState[VinID].FovFreezeState |= ((UINT32)SVC_VIN_ERR_FOV_STAT_FREZE << FovID);
                    IsUpd = 1U;
                }
            } else {
                if ((SvcVinErrCtrl.ErrState[VinID].FovFreezeState & ((UINT32)SVC_VIN_ERR_FOV_STAT_FREZE << FovID)) > 0U) {
                    SvcVinErrCtrl.ErrState[VinID].FovFreezeState &= ~((UINT32)SVC_VIN_ERR_FOV_STAT_FREZE << FovID);
                    IsUpd = 1U;
                }
            }

            if (IsUpd > 0U) {
                PRN_VIN_ERR_LOG "FovID(%s%d%s)/VinID(%s%d%s) freeze update %s0x%08x%s -> %s0x%08x%s!"
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1                   PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 FovID                                        PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END                       PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1                   PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 VinID                                        PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END                       PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0                   PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 OriState                                     PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END                       PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0                   PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.ErrState[VinID].FovFreezeState PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END                       PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_API
            }
        }

        if (IsUpd > 0U) {
            SvcVinErrTask_OsdUpdate(VinID);
        }
    }
}

static void SvcVinErrTask_LinkStateChange(UINT32 VinID, UINT32 LinkLock, UINT32 VideoLock)
{
    UINT32 PRetVal;

    if (VinID < AMBA_DSP_MAX_VIN_NUM) {
        if ((SvcVinErrCtrl.ErrState[VinID].LinkUpdEnable & SVC_VIN_ERR_LNK_UPD_ON) > 0U) {
            SVC_VIN_ERR_SW_TIMER_CTRL_s *pCtrl = &(SvcVinErrCtrl.SwTimerCtrl);

            if ((pCtrl->SelectBits & SvcVinErrTask_BitGet(VinID)) > 0U) {
                pCtrl->LinkLock[VinID]  = LinkLock;
                pCtrl->VideoLock[VinID] = VideoLock;

                PRN_VIN_ERR_LOG "Change VinID(%s%d%s) Link/Video value. 0x%08x/0x%08x"
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 LinkLock                   PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 VideoLock                  PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_DBG1
            } else {
                UINT32 TimeTicks = 0U;
                PRetVal = AmbaKAL_GetSysTickCount(&TimeTicks); PRN_VIN_ERR_HDLR

                pCtrl->LinkChgTimeStamp[VinID] = TimeTicks + SVC_VIN_ERR_LNK_STA_CHG_THD;
                pCtrl->LinkLock[VinID]         = LinkLock;
                pCtrl->LinkLockUpd[VinID]      = LinkLock;
                pCtrl->VideoLock[VinID]        = VideoLock;
                pCtrl->VideoLockUpd[VinID]     = VideoLock;

                pCtrl->UpdDoneCnt[VinID] = 0U;
                pCtrl->UpdStartCnt[VinID] = 0U;
                pCtrl->SendCmdRetVal[VinID] = 0U;

                PRN_VIN_ERR_LOG "Update VinID(%s%d%s) Link/Video State. 0x%08x/0x%08x, TimeStamp: %d"
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1     PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 VinID                          PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END         PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 LinkLock                       PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 VideoLock                      PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 pCtrl->LinkChgTimeStamp[VinID] PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_DBG1

                pCtrl->SelectBits |= SvcVinErrTask_BitGet(VinID);

                pCtrl->State |= SVC_VIN_ERR_TIMER_LINK_UPD;
            }
        } else {

            PRN_VIN_ERR_LOG "VinID(%s%d%s) SerDes state bkp update. LinkLock %s0x%08x%s -> %s0x%08x%s, VideoLock %s0x%08x%s -> %s0x%08x%s!"
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1                  PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 VinID                                       PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END                      PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0                  PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.ErrState[VinID].LinkBkpState  PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END                      PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0                  PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 LinkLock                                    PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END                      PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0                  PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.ErrState[VinID].VideoBkpState PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END                      PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0                  PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 VideoLock                                   PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END                      PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_DBG1

            SvcVinErrCtrl.ErrState[VinID].LinkBkpState = LinkLock;
            SvcVinErrCtrl.ErrState[VinID].VideoBkpState = VideoLock;
        }
    }
}

static void SvcVinErrTask_LinkStateUpd(UINT32 VinID, UINT32 CurLinkLock, UINT32 CurVideoLock)
{
    if ((VinID < AMBA_DSP_MAX_VIN_NUM) &&
        ((SvcVinErrCtrl.ErrState[VinID].LinkUpdEnable & SVC_VIN_ERR_LNK_UPD_ON) > 0U)) {

        PRN_VIN_ERR_LOG "VinID(%s%d%s) SerDes state update. LinkLock %s0x%08x%s -> %s0x%08x%s, VideoLock %s0x%08x%s -> %s0x%08x%s!"
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1                   PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 VinID                                        PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END                       PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0                   PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.ErrState[VinID].LinkLockState  PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END                       PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0                   PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 CurLinkLock                                  PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END                       PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0                   PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.ErrState[VinID].VideoLockState PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END                       PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0                   PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 CurVideoLock                                 PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END                       PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_API

        SvcVinErrCtrl.ErrState[VinID].LinkLockState = CurLinkLock;
        SvcVinErrCtrl.ErrState[VinID].VideoLockState = CurVideoLock;

        SvcVinErrTask_DependencyChk(VinID, CurLinkLock);

        SvcVinErrTask_FovStateUpd(VinID);

        SvcVinErrTask_OsdUpdate(VinID);

        SvcVinErrTask_RecoverChk(VinID);
    }
}

static void SvcVinErrTask_LinkStateUpdEna(UINT32 VinID, UINT32 Enable)
{
#ifdef CONFIG_BUILD_MONFRW_SERDES
    if (VinID < AMBA_DSP_MAX_VIN_NUM) {
        UINT32 OriLinkUpdEna = SvcVinErrCtrl.ErrState[VinID].LinkUpdEnable;

        SvcVinErrCtrl.ErrState[VinID].LinkUpdEnable = Enable;

        if (Enable != OriLinkUpdEna) {
            if (Enable == 0U) {
                SvcVinErrCtrl.ErrState[VinID].LinkBkpState = SvcVinErrCtrl.ErrState[VinID].LinkLockState;
                SvcVinErrCtrl.ErrState[VinID].VideoBkpState = SvcVinErrCtrl.ErrState[VinID].VideoLockState;
            } else {
                SvcVinErrTask_LinkStateChange(VinID, SvcVinErrCtrl.ErrState[VinID].LinkBkpState, SvcVinErrCtrl.ErrState[VinID].VideoBkpState);
                SvcVinErrCtrl.ErrState[VinID].LinkBkpState = 0U;
                SvcVinErrCtrl.ErrState[VinID].VideoBkpState = 0U;
            }
        }

        PRN_VIN_ERR_LOG "VinID(%s%d%s) SerDes state update %s%s%s!"
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1                       PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 VinID                                            PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END                           PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_STR                           PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   ( (Enable > 0U) ? ( "enable" ) : ( "disable" ) ) PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END                           PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_API
    }
#else
    PRN_VIN_ERR_LOG "Link Update mechanism does not active"
        PRN_VIN_ERR_ARG_UINT32 VinID  PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_ARG_UINT32 Enable PRN_VIN_ERR_ARG_POST
    PRN_VIN_ERR_DBG1
#endif
}

static void SvcVinErrTask_FovStateUpd(UINT32 VinID)
{
    UINT32 PRetVal;

    if (VinID < AMBA_DSP_MAX_VIN_NUM) {
        UINT32 SerDesIdx, VidLockState = 0U;
        UINT32 FovIdx, SrcSerDesIdx;
        UINT32 UpdFovBits = 0U;                     // using for disable 3a statistic
        AMBA_KAL_EVENT_FLAG_INFO_s CurStateFlag;

        AmbaSvcWrap_MisraMemset(&CurStateFlag, 0, sizeof(CurStateFlag));
        PRetVal = AmbaKAL_EventFlagQuery(&(SvcVinErrCtrl.ErrState[VinID].VinStateFlag), &CurStateFlag); PRN_VIN_ERR_HDLR

        for (SerDesIdx = 0U; SerDesIdx < SVC_VIN_ERR_MAX_SER_NUM; SerDesIdx ++) {
            if ((SvcVinErrCtrl.SerDesSelectBits[VinID] & SvcVinErrTask_BitGet(SerDesIdx)) > 0U) {
                if ((SvcVinErrTask_GetSerDesState(VinID, SerDesIdx) & SVC_VIN_ERR_SER_STAT_VID_LOCK) > 0U) {
                    VidLockState |= SvcVinErrTask_BitGet(SerDesIdx);
                }
            }
        }

        for (FovIdx = 0U; FovIdx < AMBA_DSP_MAX_VIEWZONE_NUM; FovIdx ++) {
            if ((SvcVinErrCtrl.FovSelectBits & SvcVinErrTask_BitGet(FovIdx)) > 0U) {
                SrcSerDesIdx = SvcVinErrCtrl.FovInfo[FovIdx].SrcSerDesIdx;
                if ((SvcVinErrCtrl.FovInfo[FovIdx].SrcVinID == VinID) && (SrcSerDesIdx < SVC_VIN_ERR_MAX_SELECT_BITS)) {
                    if ((VidLockState & SvcVinErrTask_BitGet(SrcSerDesIdx)) == 0U) {
                        SvcVinErrCtrl.FovInfo[FovIdx].State &= ~SVC_VIN_ERR_FOV_VISIBLE;
                        UpdFovBits |= SvcVinErrTask_BitGet(FovIdx);
                    } else {
                        if ((CurStateFlag.CurrentFlags & SVC_VIN_ERR_VIN_STATE_LIV) > 0U) {
                            SvcVinErrCtrl.FovInfo[FovIdx].State |= SVC_VIN_ERR_FOV_VISIBLE;
                        }
                    }
                }
            }
        }

        if (UpdFovBits > 0U) {
            SvcVinErrTask_StatisticVZUpd(UpdFovBits, 0U);
        }
    }

}

static void SvcVinErrTask_TimerHandler(UINT32 EntryArg)
{
    UINT32 PRetVal;
    SVC_VIN_ERR_SW_TIMER_CTRL_s *pCtrl = &(SvcVinErrCtrl.SwTimerCtrl);

    AmbaMisra_TouchUnused(&EntryArg);

#ifndef CONFIG_BUILD_MONFRW_GRAPH
    SvcVinErrTask_MsgQryTimerHdlr(EntryArg);
#endif
    if (pCtrl->TimerSeqCnt == 0xFFFFFFFFU) {
        pCtrl->TimerSeqCnt = 0U;
    }
    pCtrl->TimerSeqCnt += 1U;

    if ((pCtrl->State & SVC_VIN_ERR_TIMER_READY) > 0U) {
        UINT32 VinIdx;
        UINT32 CurSysTicks = 0U;
        SVC_VIN_ERR_LINK_UPD_s LinkUpd;

        PRetVal = AmbaKAL_GetSysTickCount(&CurSysTicks); PRN_VIN_ERR_HDLR

        // SerDes Link Update
        if (pCtrl->SelectBits > 0U) {

            for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
                if ((pCtrl->SelectBits & SvcVinErrTask_BitGet(VinIdx)) > 0U) {

                    if (pCtrl->UpdStartCnt[VinIdx] == 0U) {
                        pCtrl->UpdStartCnt[VinIdx] = pCtrl->TimerSeqCnt;
                    }

                    if ((pCtrl->LinkLock[VinIdx] == pCtrl->LinkLockUpd[VinIdx]) &&
                        (pCtrl->VideoLock[VinIdx] == pCtrl->VideoLockUpd[VinIdx])) {
                        if (CurSysTicks >= pCtrl->LinkChgTimeStamp[VinIdx]) {
                            pCtrl->SelectBits &= ~SvcVinErrTask_BitGet(VinIdx);

                            pCtrl->LinkChgTimeStamp[VinIdx] = 0U;
                            pCtrl->UpdDoneCnt[VinIdx] = pCtrl->TimerSeqCnt;

                            AmbaSvcWrap_MisraMemset(&LinkUpd, 0, sizeof(LinkUpd));
                            LinkUpd.VinID          = VinIdx;
                            LinkUpd.LinkLockState  = pCtrl->LinkLockUpd[VinIdx];
                            LinkUpd.VideoLockState = pCtrl->VideoLockUpd[VinIdx];
                            pCtrl->SendCmdRetVal[VinIdx] = SvcVinErrTask_SendMsg(SVC_VIN_ERR_CTRL_CMD_LNK_UPD, &LinkUpd, (UINT32)sizeof(LinkUpd), AMBA_KAL_NO_WAIT);

                            pCtrl->LinkLockUpd[VinIdx]  = 0xFFFFFFFFU;
                            pCtrl->VideoLockUpd[VinIdx] = 0xFFFFFFFFU;
                        }
                    } else {
                        pCtrl->LinkChgTimeStamp[VinIdx] = CurSysTicks + SVC_VIN_ERR_LNK_STA_CHG_THD;
                        pCtrl->LinkLockUpd[VinIdx]      = pCtrl->LinkLock[VinIdx];
                        pCtrl->VideoLockUpd[VinIdx]     = pCtrl->VideoLock[VinIdx];
                    }
                }
            }

            if (pCtrl->SelectBits == 0U) {
                pCtrl->State &= ~SVC_VIN_ERR_TIMER_LINK_UPD;
            }
        }

        // Dependency vin update
        if (pCtrl->DepVinBits > 0U) {
            for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
                if ((pCtrl->DepVinBits & SvcVinErrTask_BitGet(VinIdx)) > 0U) {
                    if (CurSysTicks >= pCtrl->DepTimeStamp[VinIdx]) {
                        pCtrl->DepVinBits &= ~SvcVinErrTask_BitGet(VinIdx);

                        pCtrl->DepTimeStamp[VinIdx] = 0U;

                        AmbaSvcWrap_MisraMemset(&LinkUpd, 0, sizeof(LinkUpd));
                        LinkUpd.VinID          = VinIdx;
                        PRetVal = SvcVinErrTask_SendMsg(SVC_VIN_ERR_CTRL_CMD_DEP_UPD, &LinkUpd, (UINT32)sizeof(LinkUpd), AMBA_KAL_NO_WAIT); PRN_VIN_ERR_HDLR
                    }
                }
            }

            if (pCtrl->DepVinBits == 0U) {
                pCtrl->State &= ~SVC_VIN_ERR_TIMER_DEPENDENCY;
            }
        }
    }
}

static void SvcVinErrTask_Info(void)
{
    UINT32 PRetVal;

    if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_INIT) > 0U) {
        UINT32 VinIdx, FovIdx;
        AMBA_KAL_EVENT_FLAG_INFO_s CurEvtInfo;

        PRN_VIN_ERR_LOG " " PRN_VIN_ERR_API
        PRN_VIN_ERR_LOG "====== %sVin Err Control%s ======"
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0 PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_API
        PRN_VIN_ERR_LOG "  Name : %s"
            PRN_VIN_ERR_ARG_CSTR   SvcVinErrCtrl.Name PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_API
        PRN_VIN_ERR_LOG "  VinSelectBits : 0x%x"
            PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.VinSelectBits PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_API
        for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
            if ((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(VinIdx)) > 0U) {
                PRN_VIN_ERR_LOG "  SerDesSelectBits[%d] : 0x%05x, VinID(%d)"
                    PRN_VIN_ERR_ARG_UINT32 VinIdx                                 PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.SerDesSelectBits[VinIdx] PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 VinIdx                                 PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_API
            }
        }

        PRN_VIN_ERR_LOG "  Dependency Ctrl : " PRN_VIN_ERR_API
        PRN_VIN_ERR_LOG "    NumOfStrm : %d"
            PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.DependencyCtrl.NumOfStrm PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_API
        PRN_VIN_ERR_LOG "    VinID | Number | Dependency Bits | stream bits" PRN_VIN_ERR_API
        for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
            if ((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(VinIdx)) > 0U) {
                PRN_VIN_ERR_LOG "       %02d |   %02d   |   0x%08x    |  0x%08x | %d"
                    PRN_VIN_ERR_ARG_UINT32 VinIdx                                           PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.DependencyCtrl.DepVinNum[VinIdx]   PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.DependencyCtrl.DepVinBits[VinIdx]  PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.DependencyCtrl.StrmSelBits[VinIdx] PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.DependencyCtrl.UpdCtrl[VinIdx]     PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_API
            }
        }

        PRN_VIN_ERR_LOG "  Rcv Dependency Ctrl : " PRN_VIN_ERR_API
        PRN_VIN_ERR_LOG "    VinID | Number | Dependency Bits | From VinID" PRN_VIN_ERR_API
        for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
            if ((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(VinIdx)) > 0U) {
                PRN_VIN_ERR_LOG "       %02d |   %02d   |   0x%08x    | 0%08x"
                    PRN_VIN_ERR_ARG_UINT32 VinIdx                                           PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.DependencyCtrl.Recover[VinIdx].DepVinNum  PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.DependencyCtrl.Recover[VinIdx].DepVinBits PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.DependencyCtrl.Recover[VinIdx].FromVinID  PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_API
            }
        }

        PRN_VIN_ERR_LOG "  FovSelectBits : 0x%x"
            PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.FovSelectBits PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_API

        PRN_VIN_ERR_LOG "  FovInfo ID |    State   | SrcVinID | SrcSerDesIdx | DstVoutBits | Latency threshold" PRN_VIN_ERR_API
        for (FovIdx = 0U; FovIdx < AMBA_DSP_MAX_VIEWZONE_NUM; FovIdx ++) {
            if ((SvcVinErrCtrl.FovSelectBits & SvcVinErrTask_BitGet(FovIdx)) > 0U) {
                PRN_VIN_ERR_LOG "          %02d | 0x%08x |    %02d    | %02d           |  0x%08X | %d"
                    PRN_VIN_ERR_ARG_UINT32 FovIdx                                         PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.FovInfo[FovIdx].State            PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.FovInfo[FovIdx].SrcVinID         PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.FovInfo[FovIdx].SrcSerDesIdx     PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.FovInfo[FovIdx].DstVoutBits      PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.FovInfo[FovIdx].VoutLatencyThreshold PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_API
            }
        }

        PRN_VIN_ERR_LOG "  ResCfg.VinTree : %s "
            PRN_VIN_ERR_ARG_CSTR   SvcVinErrCtrl.ResCfg.VinTree PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_API


        PRN_VIN_ERR_LOG "  ErrState VinID |  VinState  |  LinkLock  | PreLinkLock |  VideoLock | PreVideoLock | FovFreezeState | FovFreezeSrc" PRN_VIN_ERR_API
        for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
            if ((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(VinIdx)) > 0U) {
                AmbaSvcWrap_MisraMemset(&CurEvtInfo, 0, sizeof(CurEvtInfo));
                PRetVal = AmbaKAL_EventFlagQuery(&(SvcVinErrCtrl.ErrState[VinIdx].VinStateFlag), &CurEvtInfo); PRN_VIN_ERR_HDLR

                PRN_VIN_ERR_LOG "              %02d | 0x%08x | 0x%08x | 0x%08x  | 0x%08x |  0x%08x  |   0x%08x   | %02d"
                    PRN_VIN_ERR_ARG_UINT32 VinIdx                                           PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 CurEvtInfo.CurrentFlags                          PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.ErrState[VinIdx].LinkLockState     PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.ErrState[VinIdx].LinkLockPreState  PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.ErrState[VinIdx].VideoLockState    PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.ErrState[VinIdx].VideoLockPreState PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.ErrState[VinIdx].FovFreezeState    PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.ErrState[VinIdx].FovFreezeSrc      PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_API

            }
        }

        PRN_VIN_ERR_LOG "  LinkChgCtrl :" PRN_VIN_ERR_API
        PRN_VIN_ERR_LOG "    TimerSeqCnt   : %d "
            PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.SwTimerCtrl.TimerSeqCnt PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_API
        PRN_VIN_ERR_LOG "    TimerPeriod   : %d "
            PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.SwTimerCtrl.TimerPeriod PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_API
        PRN_VIN_ERR_LOG "    SelectBits    : 0x%x"
            PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.SwTimerCtrl.SelectBits PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_API

        PRN_VIN_ERR_LOG "    VinID |  LinkLock  | LinkLockUpd |  VideoLock | VideoLockUpd | Threshold" PRN_VIN_ERR_API
        for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
            PRN_VIN_ERR_LOG "       %02d | 0x%08x | 0x%08x  | 0x%08x |  0x%08x  | %d"
                PRN_VIN_ERR_ARG_UINT32 VinIdx                                             PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.SwTimerCtrl.LinkLock[VinIdx]         PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.SwTimerCtrl.LinkLockUpd[VinIdx]      PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.SwTimerCtrl.VideoLock[VinIdx]        PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.SwTimerCtrl.VideoLockUpd[VinIdx]     PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.SwTimerCtrl.LinkChgTimeStamp[VinIdx] PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API
        }

        PRN_VIN_ERR_LOG "    VinID |  UpdStart  |  UpdDone   | SendQueRet" PRN_VIN_ERR_API
        for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
            PRN_VIN_ERR_LOG "       %02d | 0x%08x | 0x%08x | 0x%08x"
                PRN_VIN_ERR_ARG_UINT32 VinIdx                                          PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.SwTimerCtrl.UpdStartCnt[VinIdx]   PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.SwTimerCtrl.UpdDoneCnt[VinIdx]    PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.SwTimerCtrl.SendCmdRetVal[VinIdx] PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API
        }
    }
}

static void SvcVinErrTask_RcvSubProcInit(void)
{
    if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_RCV_INIT) == 0U) {
        UINT32 RcvSeq = 0U;

        AmbaSvcWrap_MisraMemset(SvcVinErrRcvSubProc, 0, sizeof(SvcVinErrRcvSubProc));
        SvcVinErrRcvSubProc[RcvSeq] = (SVC_VIN_ERR_RCV_PROC_s) { 1U, SVC_VIN_ERR_RCV_PROC_STOP_VIN   , "STOP VIN     ", 0x00000001UL, SvcVinErrTask_RcvProcStopVin   }; RcvSeq ++;
        SvcVinErrRcvSubProc[RcvSeq] = (SVC_VIN_ERR_RCV_PROC_s) { 1U, SVC_VIN_ERR_RCV_PROC_DIS_LNK_UPD, "DIS LNK UPD  ", 0x00000002UL, SvcVinErrTask_RcvProcDisLnkUpd }; RcvSeq ++;
        SvcVinErrRcvSubProc[RcvSeq] = (SVC_VIN_ERR_RCV_PROC_s) { 1U, SVC_VIN_ERR_RCV_PROC_DIS_SENSOR , "DIS SENSOR   ", 0x00000004UL, SvcVinErrTask_RcvProcDisSensor }; RcvSeq ++;
        SvcVinErrRcvSubProc[RcvSeq] = (SVC_VIN_ERR_RCV_PROC_s) { 1U, SVC_VIN_ERR_RCV_PROC_RST_POWER  , "RESET POWER  ", 0x00000008UL, SvcVinErrTask_RcvProcResetPWR  }; RcvSeq ++;
        SvcVinErrRcvSubProc[RcvSeq] = (SVC_VIN_ERR_RCV_PROC_s) { 1U, SVC_VIN_ERR_RCV_PROC_INI_SENSOR , "INIT SENSOR  ", 0x00000010UL, SvcVinErrTask_RcvProcIniSensor }; RcvSeq ++;
        SvcVinErrRcvSubProc[RcvSeq] = (SVC_VIN_ERR_RCV_PROC_s) { 1U, SVC_VIN_ERR_RCV_PROC_ENA_SENSOR , "ENA SENSOR   ", 0x00000020UL, SvcVinErrTask_RcvProcEnaSensor }; RcvSeq ++;
        SvcVinErrRcvSubProc[RcvSeq] = (SVC_VIN_ERR_RCV_PROC_s) { 1U, SVC_VIN_ERR_RCV_PROC_CFG_SENSOR , "CONFIG SENSOR", 0x00000040UL, SvcVinErrTask_RcvProcCfgSensor }; RcvSeq ++;
        SvcVinErrRcvSubProc[RcvSeq] = (SVC_VIN_ERR_RCV_PROC_s) { 1U, SVC_VIN_ERR_RCV_PROC_ENA_LNK_UPD, "ENA LNK UPD  ", 0x00000080UL, SvcVinErrTask_RcvProcEnaLnkUpd }; RcvSeq ++;
        SvcVinErrRcvSubProc[RcvSeq] = (SVC_VIN_ERR_RCV_PROC_s) { 1U, SVC_VIN_ERR_RCV_PROC_CFG_IDSP   , "CONFIG IDSP  ", 0x00000100UL, SvcVinErrTask_RcvProcCfgIdsp   }; RcvSeq ++;
        SvcVinErrRcvSubProc[RcvSeq] = (SVC_VIN_ERR_RCV_PROC_s) { 1U, SVC_VIN_ERR_RCV_PROC_CFG_VIN    , "CONFIG VIN   ", 0x00000200UL, SvcVinErrTask_RcvProcCfgVin    }; RcvSeq ++;
        SvcVinErrRcvSubProc[RcvSeq] = (SVC_VIN_ERR_RCV_PROC_s) { 1U, SVC_VIN_ERR_RCV_PROC_STR_VIN    , "START VIN    ", 0x00000400UL, SvcVinErrTask_RcvProcStartVin  }; RcvSeq ++;
#ifdef CONFIG_SOC_CV2FS
        SvcVinErrRcvSubProc[RcvSeq] = (SVC_VIN_ERR_RCV_PROC_s) { 0U, SVC_VIN_ERR_RCV_PROC_RE_CFG_FPD , "RE CFG FPD   ", 0x00002000UL, SvcVinErrTask_RcvProcReCfgVout };
#else
        SvcVinErrRcvSubProc[RcvSeq] = (SVC_VIN_ERR_RCV_PROC_s) { 1U, SVC_VIN_ERR_RCV_PROC_RE_CFG_FPD , "RE CFG FPD   ", 0x00002000UL, SvcVinErrTask_RcvProcReCfgVout };
#endif


        SvcVinErrCtrlFlg |= SVC_VIN_ERR_FLG_RCV_INIT;
    }
}

static UINT32 SvcVinErrTask_RcvCtrlCfg(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    SVC_VIN_ERR_RECOVER_TASK_s *pRecover = &(SvcVinErrCtrl.RecoverTask);

    AmbaSvcWrap_MisraMemset(pRecover, 0, sizeof(SVC_VIN_ERR_RECOVER_TASK_s));

    /* %s_%s */
      AmbaUtility_StringCopy(pRecover->Name, sizeof(pRecover->Name), SvcVinErrCtrl.Name);
    AmbaUtility_StringAppend(pRecover->Name, (UINT32)sizeof(pRecover->Name), "_");
    AmbaUtility_StringAppend(pRecover->Name, (UINT32)sizeof(pRecover->Name), "Recover");

    /* Create queue */
    PRetVal = AmbaKAL_MsgQueueCreate( &(pRecover->Que),
                                      pRecover->Name,
                                      (UINT32)sizeof(SVC_VIN_ERR_RECOVER_CMD_t),
                                      pRecover->QueBuf,
                                      (UINT32)sizeof(pRecover->QueBuf) );
    if (PRetVal != 0U) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to create vin err control - create recover queue fail!" PRN_VIN_ERR_NG
    } else {

        /* Configure recover ctrl */
        SvcVinErrTask_RcvSubProcInit();
        pRecover->RcvCtrl.pSubProc = SvcVinErrRcvSubProc;

        PRetVal = AmbaKAL_EventFlagCreate(&(pRecover->RcvCtrl.ActFlg), pRecover->Name);
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to create vin err control - create recover control flag fail!" PRN_VIN_ERR_NG
        } else {
            PRetVal = AmbaKAL_EventFlagClear(&(pRecover->RcvCtrl.ActFlg), 0xFFFFFFFFU);  PRN_VIN_ERR_HDLR
        }

        if (RetVal == SVC_OK) {

            pRecover->RcvCtrl.ProcSelectBits[SVC_VIN_ERR_RCV_CMD_VIN] = SvcVinErrTask_BitGet(SVC_VIN_ERR_RCV_PROC_STOP_VIN)    |
                                                                        SvcVinErrTask_BitGet(SVC_VIN_ERR_RCV_PROC_DIS_LNK_UPD) |
                                                                        SvcVinErrTask_BitGet(SVC_VIN_ERR_RCV_PROC_DIS_SENSOR)  |
                                                                        SvcVinErrTask_BitGet(SVC_VIN_ERR_RCV_PROC_RST_POWER)   |
                                                                        SvcVinErrTask_BitGet(SVC_VIN_ERR_RCV_PROC_INI_SENSOR)  |
                                                                        SvcVinErrTask_BitGet(SVC_VIN_ERR_RCV_PROC_ENA_SENSOR)  |
                                                                        SvcVinErrTask_BitGet(SVC_VIN_ERR_RCV_PROC_CFG_SENSOR)  |
                                                                        SvcVinErrTask_BitGet(SVC_VIN_ERR_RCV_PROC_ENA_LNK_UPD) |
                                                                        SvcVinErrTask_BitGet(SVC_VIN_ERR_RCV_PROC_CFG_IDSP)    |
                                                                        SvcVinErrTask_BitGet(SVC_VIN_ERR_RCV_PROC_CFG_VIN)     |
                                                                        SvcVinErrTask_BitGet(SVC_VIN_ERR_RCV_PROC_STR_VIN);

            pRecover->RcvCtrl.ProcSelectBits[SVC_VIN_ERR_RCV_CMD_SERDES] = SvcVinErrTask_BitGet(SVC_VIN_ERR_RCV_PROC_CFG_SENSOR);

            pRecover->RcvCtrl.ProcSelectBits[SVC_VIN_ERR_RCV_CMD_LIV] = 0U;

            SvcVinErrTask_RcvFlagSet(0xFFFFFFFFU);

            if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_LOG_DBG1) > 0U) {
                SvcVinErrTask_RcvCtrlInfo();
            }
        }

        /* Create recover task */
        pRecover->Ctrl.CpuBits    = SVC_VIN_ERR_TASK_CPU_BITS;
        pRecover->Ctrl.Priority   = SVC_VIN_ERR_RECOVER_TASK_PRI;
        pRecover->Ctrl.EntryFunc  = SvcVinErrTask_RcvTaskEntry;
        pRecover->Ctrl.StackSize  = SVC_VIN_ERR_STACK_SZ;
        pRecover->Ctrl.pStackBase = &(pRecover->Stack[0]);

        PRetVal = SvcTask_Create(pRecover->Name, &(pRecover->Ctrl));
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to create vin err control - create recover task fail! ErrCode 0x%x"
                PRN_VIN_ERR_ARG_UINT32 PRetVal PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        } else {
            PRN_VIN_ERR_LOG "Successful to create vin err recover task - %s"
                PRN_VIN_ERR_ARG_CSTR   pRecover->Name PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_DBG1
        }

        if (RetVal == SVC_OK) {
            SVC_USER_PREF_s *pSvcUserPref = NULL;

            pRecover->CurFmtID = 0U;

            if (0U != SvcUserPref_Get(&pSvcUserPref)) {
                PRN_VIN_ERR_LOG "Fail to get current format_id - get user pref fail!" PRN_VIN_ERR_NG
            } else if (pSvcUserPref == NULL) {
                PRN_VIN_ERR_LOG "Fail to get current format_id - invalid user pref!" PRN_VIN_ERR_NG
            } else {
                pRecover->CurFmtID = pSvcUserPref->FormatId;
            }
        }
    }

    return RetVal;
}

static void SvcVinErrTask_RcvCtrlInfo(void)
{
    if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_RCV_INIT) > 0U) {
        SVC_VIN_ERR_RCV_CTRL_s *pCtrl = &(SvcVinErrCtrl.RecoverTask.RcvCtrl);
        UINT32 PRetVal;
        UINT32 Idx;
        AMBA_KAL_EVENT_FLAG_INFO_s CurFlgInfo;

        PRN_VIN_ERR_LOG " " PRN_VIN_ERR_API

        PRN_VIN_ERR_LOG "---- %sRecover Control Info%s ----"
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0 PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_API

        AmbaSvcWrap_MisraMemset(&CurFlgInfo, 0, sizeof(CurFlgInfo));
        PRetVal = AmbaKAL_EventFlagQuery(&(pCtrl->ActFlg), &(CurFlgInfo)); PRN_VIN_ERR_HDLR
        PRN_VIN_ERR_LOG "      ActFlg : 0x%08x ( wait task: %d )"
            PRN_VIN_ERR_ARG_UINT32 CurFlgInfo.CurrentFlags PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 CurFlgInfo.NumWaitTask  PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_API

        PRN_VIN_ERR_LOG "           Name : %s"
            PRN_VIN_ERR_ARG_CSTR   pCtrl->Name PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_API
        PRN_VIN_ERR_LOG " ProcSelectBits : Recover cmd index | process select bits" PRN_VIN_ERR_API
        for (Idx = 0U; Idx < SVC_VIN_ERR_RCV_CMD_NUM; Idx ++) {
            PRN_VIN_ERR_LOG "                                 %02d | 0x%08x"
                PRN_VIN_ERR_ARG_UINT32 Idx                        PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 pCtrl->ProcSelectBits[Idx] PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API
        }
        PRN_VIN_ERR_LOG "       pSubProc : %p"
            PRN_VIN_ERR_ARG_CPOINT pCtrl->pSubProc PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_API
        if (pCtrl->pSubProc != NULL) {
            const void *pTmpPointer = NULL;

#ifdef CONFIG_THREADX64
            PRN_VIN_ERR_LOG "                  Enable | ProcID |   ProcFlg  |      Func Addr     | ProcName" PRN_VIN_ERR_API
#else
            PRN_VIN_ERR_LOG "                  Enable | ProcID |   ProcFlg  |  Func Addr | ProcName" PRN_VIN_ERR_API
#endif

            for (Idx = 0U; Idx < SVC_VIN_ERR_RCV_PROC_NUM; Idx ++) {
                if (pCtrl->pSubProc[Idx].Enable > 0U) {
                    AmbaMisra_TypeCast(&(pTmpPointer), &(pCtrl->pSubProc[Idx].pFunc));

                    PRN_VIN_ERR_LOG "                    %02d   |   %02d   | 0x%08x | %p | %s"
                        PRN_VIN_ERR_ARG_UINT32 pCtrl->pSubProc[Idx].Enable   PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 pCtrl->pSubProc[Idx].ProcID   PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 pCtrl->pSubProc[Idx].ProcFlg  PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_CPOINT pTmpPointer                   PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_CSTR   pCtrl->pSubProc[Idx].ProcName PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_API
                }
            }
        }
    }
}

static void SvcVinErrTask_RcvFlagClear(UINT32 Flag)
{
    UINT32 PRetVal;
    PRetVal = AmbaKAL_EventFlagClear(&(SvcVinErrCtrl.RecoverTask.RcvCtrl.ActFlg), Flag); PRN_VIN_ERR_HDLR
}

static void SvcVinErrTask_RcvFlagSet(UINT32 Flag)
{
    UINT32 PRetVal;
    PRetVal = AmbaKAL_EventFlagSet(&(SvcVinErrCtrl.RecoverTask.RcvCtrl.ActFlg), Flag); PRN_VIN_ERR_HDLR
}

static void SvcVinErrTask_RcvFlagGet(UINT32 Flag, UINT32 Timeout)
{
    UINT32 PRetVal;
    UINT32 ActualFlags = 0U;

    PRetVal = AmbaKAL_EventFlagGet(&(SvcVinErrCtrl.RecoverTask.RcvCtrl.ActFlg)
                                  , Flag
                                  , AMBA_KAL_FLAGS_ALL
                                  , AMBA_KAL_FLAGS_CLEAR_NONE
                                  , &ActualFlags, Timeout); PRN_VIN_ERR_HDLR
}

static UINT32 SvcVinErrTask_RcvFlagQry(UINT32 *pFlag)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (pFlag != NULL) {
        AMBA_KAL_EVENT_FLAG_t *pActFlg = &(SvcVinErrCtrl.RecoverTask.RcvCtrl.ActFlg);
        AMBA_KAL_EVENT_FLAG_INFO_s CurFlgInfo;

        AmbaSvcWrap_MisraMemset(&CurFlgInfo, 0, sizeof(CurFlgInfo));
        PRetVal = AmbaKAL_EventFlagQuery(pActFlg, &CurFlgInfo);
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Query recover flag fail" PRN_VIN_ERR_NG
        } else {
            *pFlag = CurFlgInfo.CurrentFlags;
        }
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}

static UINT32 SvcVinErrTask_RcvProcStartVin(UINT32 PreChkVal, const void *pCmd, const void *pProc)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (PreChkVal == SVC_OK) {
        SVC_VIN_ERR_RECOVER_CMD_t *pRcvCmd;
        SVC_VIN_ERR_RCV_PROC_s    *pProcInfo;

        AmbaMisra_TypeCast(&(pRcvCmd), &(pCmd));
        AmbaMisra_TypeCast(&(pProcInfo), &(pProc));

        if (pRcvCmd == NULL) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to proc recover - invalid recover command!" PRN_VIN_ERR_NG
        } else if (pProcInfo == NULL) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to proc recover - invalid recover process info!" PRN_VIN_ERR_NG
        } else {
            UINT32 VinID;
            UINT16 NumArr = 0U;
            UINT16 ViewZoneIDArr[AMBA_DSP_MAX_VIEWZONE_NUM];
            UINT8  EnableArr[AMBA_DSP_MAX_VIEWZONE_NUM];
            UINT32 Idx;
            UINT32 CurViewZoneNum = 0U, CurViewZoneID[AMBA_DSP_MAX_VIEWZONE_NUM];

            SvcVinErrTask_RcvFlagGet(pProcInfo->ProcFlg, AMBA_KAL_WAIT_FOREVER);

            AmbaSvcWrap_MisraMemset(ViewZoneIDArr,   0, sizeof(ViewZoneIDArr));
            AmbaSvcWrap_MisraMemset(EnableArr,       0, sizeof(EnableArr));

            PRN_VIN_ERR_LOG "---- %sStart VinBits(0x%x)%s ----"
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_2 PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 pRcvCmd->VinSelectBits     PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_DBG1

            for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
                if ((pRcvCmd->VinSelectBits & SvcVinErrTask_BitGet(VinID)) > 0U) {

                    CurViewZoneNum = 0U;
                    AmbaSvcWrap_MisraMemset(CurViewZoneID, 0, sizeof(CurViewZoneID));

                    if (0U != SvcResCfg_GetFovIdxsInVinID(VinID, CurViewZoneID, &CurViewZoneNum)) {
                        RetVal = SVC_NG;
                        PRN_VIN_ERR_LOG "Fail to proc %s - get VinID(%d) ViewZone Info fail!"
                            PRN_VIN_ERR_ARG_CSTR   pProcInfo->ProcName PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 VinID               PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_NG
                    } else {

                        PRN_VIN_ERR_LOG "  Configure ViewZone Array: " PRN_VIN_ERR_DBG1
                        /* Configure liveview fov control */
                        for ( Idx = 0U; Idx < CurViewZoneNum; Idx++ ) {
                            ViewZoneIDArr[NumArr]  = (UINT16) CurViewZoneID[Idx];
                            EnableArr[NumArr]      = 1U;

                            PRN_VIN_ERR_LOG "    ViewZoneIDArr[%2d] = CurViewZoneID[%2d], %d"
                                PRN_VIN_ERR_ARG_UINT32 NumArr                PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_UINT32 Idx                   PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_UINT32 ViewZoneIDArr[NumArr] PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_DBG1

                            NumArr ++;
                        }
                    }

                }
            }

            if (NumArr > 0U) {
                PRetVal = AmbaDSP_LiveviewCtrl(NumArr, ViewZoneIDArr, EnableArr);
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    PRN_VIN_ERR_LOG "Fail to proc %s - Re-start VinBits(0x%x) fail! ErrCode(0x%08x)"
                        PRN_VIN_ERR_ARG_CSTR   pProcInfo->ProcName    PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 pRcvCmd->VinSelectBits PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 PRetVal                PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_NG
                }
            }

            PRN_VIN_ERR_LOG "Re-start VinBits(0x%x) done!"
                PRN_VIN_ERR_ARG_UINT32 pRcvCmd->VinSelectBits PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API
        }

        AmbaMisra_TouchUnused(&PRetVal);
        AmbaMisra_TouchUnused(pRcvCmd);
        AmbaMisra_TouchUnused(pProcInfo);
    }

    return RetVal;
}

static UINT32 SvcVinErrTask_RcvProcStopVin(UINT32 PreChkVal, const void *pCmd, const void *pProc)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (PreChkVal == SVC_OK) {
        SVC_VIN_ERR_RECOVER_CMD_t *pRcvCmd;
        SVC_VIN_ERR_RCV_PROC_s    *pProcInfo;

        AmbaMisra_TypeCast(&(pRcvCmd), &(pCmd));
        AmbaMisra_TypeCast(&(pProcInfo), &(pProc));

        if (pRcvCmd == NULL) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to proc recover - invalid recover command!" PRN_VIN_ERR_NG
        } else if (pProcInfo == NULL) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to proc recover - invalid recover process info!" PRN_VIN_ERR_NG
        } else {
            UINT32 VinID;
            UINT16 NumArr = 0U;
            UINT16 ViewZoneIDArr[AMBA_DSP_MAX_VIEWZONE_NUM];
            UINT8  EnableArr[AMBA_DSP_MAX_VIEWZONE_NUM];
            UINT32 Idx;
            UINT32 CurViewZoneNum = 0U, CurViewZoneID[AMBA_DSP_MAX_VIEWZONE_NUM];

            SvcVinErrTask_RcvFlagGet(pProcInfo->ProcFlg, AMBA_KAL_WAIT_FOREVER);

            AmbaSvcWrap_MisraMemset(ViewZoneIDArr,   0, sizeof(ViewZoneIDArr));
            AmbaSvcWrap_MisraMemset(EnableArr,       0, sizeof(EnableArr));

            PRN_VIN_ERR_LOG "---- %sStop VinBits(0x%x)%s ----"
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_2 PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 pRcvCmd->VinSelectBits     PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_DBG1

            for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
                if ((pRcvCmd->VinSelectBits & SvcVinErrTask_BitGet(VinID)) > 0U) {

                    CurViewZoneNum = 0U;
                    AmbaSvcWrap_MisraMemset(CurViewZoneID, 0, sizeof(CurViewZoneID));

                    if (0U != SvcResCfg_GetFovIdxsInVinID(VinID, CurViewZoneID, &CurViewZoneNum)) {
                        RetVal = SVC_NG;
                        PRN_VIN_ERR_LOG "Fail to proc %s - get VinID(%d) ViewZone Info fail!"
                            PRN_VIN_ERR_ARG_CSTR   pProcInfo->ProcName PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 VinID               PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_NG
                    } else {

                        PRN_VIN_ERR_LOG "  Configure ViewZone Array: " PRN_VIN_ERR_DBG1
                        /* Configure liveview fov control */
                        for ( Idx = 0U; Idx < CurViewZoneNum; Idx++ ) {
                            ViewZoneIDArr[NumArr]  = (UINT16) CurViewZoneID[Idx];
                            EnableArr[NumArr]      = 0U;

                            PRN_VIN_ERR_LOG "    ViewZoneIDArr[%2d] = CurViewZoneID[%2d], %d"
                                PRN_VIN_ERR_ARG_UINT32 NumArr                PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_UINT32 Idx                   PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_UINT32 ViewZoneIDArr[NumArr] PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_DBG1

                            NumArr ++;
                        }
                    }

                }
            }

            if (NumArr > 0U) {
                // stop vin
                PRetVal = AmbaDSP_LiveviewCtrl(NumArr, ViewZoneIDArr, EnableArr);
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    PRN_VIN_ERR_LOG "Fail to proc %s - stop VinBits(0x%x) fail! ErrCode(0x%08x)"
                        PRN_VIN_ERR_ARG_CSTR   pProcInfo->ProcName    PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 pRcvCmd->VinSelectBits PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 PRetVal                PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_NG
                } else {
                    // clear vin flag
                    PRetVal = AmbaKAL_EventFlagClear(&(SvcVinErrCtrl.VinLivFlag), pRcvCmd->VinSelectBits); PRN_VIN_ERR_HDLR

                    for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
                        if ((pRcvCmd->VinSelectBits & SvcVinErrTask_BitGet(VinID)) > 0U) {
                            PRetVal = AmbaKAL_EventFlagSet(&(SvcVinErrCtrl.ErrState[VinID].VinStateFlag), SVC_VIN_ERR_VIN_STATE_RAW_TOUT); PRN_VIN_ERR_HDLR
                        }
                    }
                }
            }

            PRN_VIN_ERR_LOG "Stop VinBits(0x%x) done!"
                PRN_VIN_ERR_ARG_UINT32 pRcvCmd->VinSelectBits PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API
        }

        AmbaMisra_TouchUnused(&PRetVal);
        AmbaMisra_TouchUnused(pRcvCmd);
        AmbaMisra_TouchUnused(pProcInfo);
    }

    return RetVal;
}

static UINT32 SvcVinErrTask_RcvProcResetPWR(UINT32 PreChkVal, const void *pCmd, const void *pProc)
{
    UINT32 RetVal = SVC_OK;
    SVC_VIN_ERR_RECOVER_CMD_t *pRcvCmd;
    SVC_VIN_ERR_RCV_PROC_s    *pProcInfo;

    AmbaMisra_TypeCast(&(pRcvCmd), &(pCmd));
    AmbaMisra_TypeCast(&(pProcInfo), &(pProc));

    AmbaMisra_TouchUnused(&PreChkVal);
    AmbaMisra_TouchUnused(pRcvCmd);
    AmbaMisra_TouchUnused(pProcInfo);

    return RetVal;
}

static UINT32 SvcVinErrTask_RcvProcIniSensor(UINT32 PreChkVal, const void *pCmd, const void *pProc)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (PreChkVal == SVC_OK) {
        SVC_VIN_ERR_RECOVER_CMD_t *pRcvCmd;
        SVC_VIN_ERR_RCV_PROC_s    *pProcInfo;
        const SVC_VIN_SRC_INIT_s *pVinSrc = &(SvcVinErrCtrl.RecoverTask.VinSrcCfg);

        AmbaMisra_TypeCast(&(pRcvCmd), &(pCmd));
        AmbaMisra_TypeCast(&(pProcInfo), &(pProc));

        if (pRcvCmd == NULL) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to proc recover - invalid recover command!" PRN_VIN_ERR_NG
        } else if (pVinSrc->CfgNum > AMBA_DSP_MAX_VIN_NUM) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to proc recover - VinSrc cfg number should not larger than vin number!" PRN_VIN_ERR_NG
        } else if (pProcInfo == NULL) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to proc recover - invalid recover process info!" PRN_VIN_ERR_NG
        } else {
            UINT32 VinID;
            UINT32 CfgIdx;
            AMBA_SENSOR_CHANNEL_s SsChan;
            AMBA_YUV_CHANNEL_s    YuvChan;

            SvcVinErrTask_RcvFlagGet(pProcInfo->ProcFlg, AMBA_KAL_WAIT_FOREVER);

            for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
                if ((pRcvCmd->VinSelectBits & SvcVinErrTask_BitGet(VinID)) > 0U) {
                    for (CfgIdx = 0U; CfgIdx < pVinSrc->CfgNum; CfgIdx ++) {
                        if (pVinSrc->InitCfgArr[CfgIdx].Config.VinID == VinID) {

                            if (pVinSrc->InitCfgArr[CfgIdx].Config.SrcType == SVC_VIN_SRC_SENSOR) {

                                AmbaSvcWrap_MisraMemset(&SsChan, 0, sizeof(SsChan));
                                SsChan.VinID    = pVinSrc->InitCfgArr[CfgIdx].Config.VinID;
                                SsChan.SensorID = pVinSrc->InitCfgArr[CfgIdx].Config.SrcBits;

                                PRN_VIN_ERR_LOG "---- %sInit VinID(%d), SensorID(0x%x) Sensor Driver%s ----"
                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_2 PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT32 SsChan.VinID               PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT32 SsChan.SensorID            PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_DBG1

                                PRetVal = AmbaSensor_Init(&SsChan);
                                if (PRetVal != 0U) {
                                    PRN_VIN_ERR_LOG "Fail to proc %s - init VinID(%d), SensorID(0x%x) sensor driver!"
                                        PRN_VIN_ERR_ARG_CSTR   pProcInfo->ProcName PRN_VIN_ERR_ARG_POST
                                        PRN_VIN_ERR_ARG_UINT32 SsChan.VinID        PRN_VIN_ERR_ARG_POST
                                        PRN_VIN_ERR_ARG_UINT32 SsChan.SensorID     PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_DBG1
                                }

                            } else if (pVinSrc->InitCfgArr[CfgIdx].Config.SrcType == SVC_VIN_SRC_YUV) {

                                AmbaSvcWrap_MisraMemset(&YuvChan, 0, sizeof(YuvChan));
                                YuvChan.VinID = pVinSrc->InitCfgArr[CfgIdx].Config.VinID;

                                PRN_VIN_ERR_LOG "---- %sInit VinID(%d) YUV Driver%s ----"
                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_2 PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT32 YuvChan.VinID              PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_DBG1

                                PRetVal = AmbaYuv_Init(&YuvChan);
                                if (PRetVal != 0U) {
                                    PRN_VIN_ERR_LOG "Fail to proc %s - init VinID(%d) yuv driver!"
                                        PRN_VIN_ERR_ARG_CSTR   pProcInfo->ProcName PRN_VIN_ERR_ARG_POST
                                        PRN_VIN_ERR_ARG_UINT32 YuvChan.VinID       PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_DBG1
                                }

                            } else {
                                //
                            }

                            break;
                        }
                    }
                }
            }

            PRN_VIN_ERR_LOG "Init VinBits(0x%x) driver done!"
                PRN_VIN_ERR_ARG_UINT32 pRcvCmd->VinSelectBits PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API
        }

        AmbaMisra_TouchUnused(&PRetVal);
        AmbaMisra_TouchUnused(pRcvCmd);
        AmbaMisra_TouchUnused(pProcInfo);
    }

    AmbaMisra_TouchUnused(&RetVal);
    return SVC_OK;
}

static UINT32 SvcVinErrTask_RcvProcEnaSensor(UINT32 PreChkVal, const void *pCmd, const void *pProc)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (PreChkVal == SVC_OK) {
        SVC_VIN_ERR_RECOVER_CMD_t *pRcvCmd;
        SVC_VIN_ERR_RCV_PROC_s    *pProcInfo;
        const SVC_VIN_SRC_INIT_s *pVinSrc = &(SvcVinErrCtrl.RecoverTask.VinSrcCfg);

        AmbaMisra_TypeCast(&(pRcvCmd), &(pCmd));
        AmbaMisra_TypeCast(&(pProcInfo), &(pProc));

        if (pRcvCmd == NULL) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to proc recover - invalid recover command!" PRN_VIN_ERR_NG
        } else if (pVinSrc->CfgNum > AMBA_DSP_MAX_VIN_NUM) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to proc recover - VinSrc cfg number should not larger than vin number!" PRN_VIN_ERR_NG
        } else if (pProcInfo == NULL) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to proc recover - invalid recover process info!" PRN_VIN_ERR_NG
        } else {
            UINT32 VinID;
            UINT32 CfgIdx;
            AMBA_SENSOR_CHANNEL_s SsChan;
            AMBA_YUV_CHANNEL_s    YuvChan;

            SvcVinErrTask_RcvFlagGet(pProcInfo->ProcFlg, AMBA_KAL_WAIT_FOREVER);

            for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
                if ((pRcvCmd->VinSelectBits & SvcVinErrTask_BitGet(VinID)) > 0U) {
                    for (CfgIdx = 0U; CfgIdx < pVinSrc->CfgNum; CfgIdx ++) {
                        if (pVinSrc->InitCfgArr[CfgIdx].Config.VinID == VinID) {

                            if (pVinSrc->InitCfgArr[CfgIdx].Config.SrcType == SVC_VIN_SRC_SENSOR) {

                                AmbaSvcWrap_MisraMemset(&SsChan, 0, sizeof(SsChan));
                                SsChan.VinID    = pVinSrc->InitCfgArr[CfgIdx].Config.VinID;
                                SsChan.SensorID = pVinSrc->InitCfgArr[CfgIdx].Config.SrcBits;

                                PRN_VIN_ERR_LOG "---- %sEnable VinID(%d), SensorID(0x%x) Sensor Driver%s ----"
                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_2 PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT32 SsChan.VinID               PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT32 SsChan.SensorID            PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_DBG1

                                PRetVal = AmbaSensor_Enable(&SsChan);
                                if (PRetVal != 0U) {
                                    PRN_VIN_ERR_LOG "Fail to proc %s - enable VinID(%d), SensorID(0x%x) sensor driver!"
                                        PRN_VIN_ERR_ARG_CSTR   pProcInfo->ProcName PRN_VIN_ERR_ARG_POST
                                        PRN_VIN_ERR_ARG_UINT32 SsChan.VinID        PRN_VIN_ERR_ARG_POST
                                        PRN_VIN_ERR_ARG_UINT32 SsChan.SensorID     PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_DBG1
                                }

                            } else if (pVinSrc->InitCfgArr[CfgIdx].Config.SrcType == SVC_VIN_SRC_YUV) {

                                AmbaSvcWrap_MisraMemset(&YuvChan, 0, sizeof(YuvChan));
                                YuvChan.VinID = pVinSrc->InitCfgArr[CfgIdx].Config.VinID;

                                PRN_VIN_ERR_LOG "---- %sEnable VinID(%d) YUV Driver%s ----"
                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_2 PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT32 YuvChan.VinID              PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_DBG1

                                PRetVal = AmbaYuv_Enable(&YuvChan);
                                if (PRetVal != 0U) {
                                    PRN_VIN_ERR_LOG "Fail to proc %s - enable VinID(%d) yuv driver!"
                                        PRN_VIN_ERR_ARG_CSTR   pProcInfo->ProcName PRN_VIN_ERR_ARG_POST
                                        PRN_VIN_ERR_ARG_UINT32 YuvChan.VinID       PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_DBG1
                                }

                            } else {
                                //
                            }

                            break;
                        }
                    }
                }
            }

            PRN_VIN_ERR_LOG "Enable VinBits(0x%x) driver done!"
                PRN_VIN_ERR_ARG_UINT32 pRcvCmd->VinSelectBits PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API
        }

        AmbaMisra_TouchUnused(&PRetVal);
        AmbaMisra_TouchUnused(pRcvCmd);
        AmbaMisra_TouchUnused(pProcInfo);
    }

    AmbaMisra_TouchUnused(&RetVal);
    return SVC_OK;
}

static UINT32 SvcVinErrTask_RcvProcDisSensor(UINT32 PreChkVal, const void *pCmd, const void *pProc)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (PreChkVal == SVC_OK) {
        SVC_VIN_ERR_RECOVER_CMD_t *pRcvCmd;
        SVC_VIN_ERR_RCV_PROC_s    *pProcInfo;
        const SVC_VIN_SRC_INIT_s *pVinSrc = &(SvcVinErrCtrl.RecoverTask.VinSrcCfg);

        AmbaMisra_TypeCast(&(pRcvCmd), &(pCmd));
        AmbaMisra_TypeCast(&(pProcInfo), &(pProc));

        if (pRcvCmd == NULL) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to proc recover - invalid recover command!" PRN_VIN_ERR_NG
        } else if (pVinSrc->CfgNum > AMBA_DSP_MAX_VIN_NUM) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to proc recover - VinSrc cfg number should not larger than vin number!" PRN_VIN_ERR_NG
        } else if (pProcInfo == NULL) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to proc recover - invalid recover process info!" PRN_VIN_ERR_NG
        } else {
            UINT32 VinID;
            UINT32 CfgIdx;
            AMBA_SENSOR_CHANNEL_s SsChan;
            AMBA_YUV_CHANNEL_s    YuvChan;

            SvcVinErrTask_RcvFlagGet(pProcInfo->ProcFlg, AMBA_KAL_WAIT_FOREVER);

            for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
                if ((pRcvCmd->VinSelectBits & SvcVinErrTask_BitGet(VinID)) > 0U) {
                    for (CfgIdx = 0U; CfgIdx < pVinSrc->CfgNum; CfgIdx ++) {
                        if (pVinSrc->InitCfgArr[CfgIdx].Config.VinID == VinID) {

                            if (pVinSrc->InitCfgArr[CfgIdx].Config.SrcType == SVC_VIN_SRC_SENSOR) {

                                AmbaSvcWrap_MisraMemset(&SsChan, 0, sizeof(SsChan));
                                SsChan.VinID    = pVinSrc->InitCfgArr[CfgIdx].Config.VinID;
                                SsChan.SensorID = pVinSrc->InitCfgArr[CfgIdx].Config.SrcBits;

                                PRN_VIN_ERR_LOG "---- %sDisable VinID(%d), SensorID(0x%x) Sensor Driver%s ----"
                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_2 PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT32 SsChan.VinID               PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT32 SsChan.SensorID            PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_DBG1

                                PRetVal = AmbaSensor_Disable(&SsChan);
                                if (PRetVal != 0U) {
                                    PRN_VIN_ERR_LOG "Fail to proc %s - disable VinID(%d), SensorID(0x%x) sensor driver!"
                                        PRN_VIN_ERR_ARG_CSTR   pProcInfo->ProcName PRN_VIN_ERR_ARG_POST
                                        PRN_VIN_ERR_ARG_UINT32 SsChan.VinID        PRN_VIN_ERR_ARG_POST
                                        PRN_VIN_ERR_ARG_UINT32 SsChan.SensorID     PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_DBG1
                                }

                            } else if (pVinSrc->InitCfgArr[CfgIdx].Config.SrcType == SVC_VIN_SRC_YUV) {

                                AmbaSvcWrap_MisraMemset(&YuvChan, 0, sizeof(YuvChan));
                                YuvChan.VinID = pVinSrc->InitCfgArr[CfgIdx].Config.VinID;

                                PRN_VIN_ERR_LOG "---- %sDisable VinID(%d) YUV Driver%s ----"
                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_2 PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT32 YuvChan.VinID              PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_DBG1

                                PRetVal = AmbaYuv_Disable(&YuvChan);
                                if (PRetVal != 0U) {
                                    PRN_VIN_ERR_LOG "Fail to proc %s - disable VinID(%d) yuv driver!"
                                        PRN_VIN_ERR_ARG_CSTR   pProcInfo->ProcName PRN_VIN_ERR_ARG_POST
                                        PRN_VIN_ERR_ARG_UINT32 YuvChan.VinID       PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_DBG1
                                }

                            } else {
                                //
                            }

                            break;
                        }
                    }
                }
            }

            PRN_VIN_ERR_LOG "Disable VinBits(0x%x) driver done!"
                PRN_VIN_ERR_ARG_UINT32 pRcvCmd->VinSelectBits PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API
        }

        AmbaMisra_TouchUnused(&PRetVal);
        AmbaMisra_TouchUnused(pRcvCmd);
        AmbaMisra_TouchUnused(pProcInfo);
    }

    AmbaMisra_TouchUnused(&RetVal);
    return SVC_OK;
}

static UINT32 SvcVinErrTask_RcvProcCfgSensor(UINT32 PreChkVal, const void *pCmd, const void *pProc)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (PreChkVal == SVC_OK) {
        SVC_VIN_ERR_RECOVER_CMD_t *pRcvCmd;
        SVC_VIN_ERR_RCV_PROC_s    *pProcInfo;
        const SVC_VIN_SRC_INIT_s *pVinSrc = &(SvcVinErrCtrl.RecoverTask.VinSrcCfg);

        AmbaMisra_TypeCast(&(pRcvCmd), &(pCmd));
        AmbaMisra_TypeCast(&(pProcInfo), &(pProc));

        if (pRcvCmd == NULL) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to proc recover - invalid recover command!" PRN_VIN_ERR_NG
        } else if (pVinSrc->CfgNum > AMBA_DSP_MAX_VIN_NUM) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to proc recover - VinSrc cfg number should not larger than vin number!" PRN_VIN_ERR_NG
        } else if (pProcInfo == NULL) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to proc recover - invalid recover process info!" PRN_VIN_ERR_NG
        } else {
            UINT32 VinID;
            UINT32 CfgIdx;
            AMBA_SENSOR_CHANNEL_s SsChan;
            AMBA_SENSOR_CONFIG_s  SsCfg;
            AMBA_YUV_CHANNEL_s    YuvChan;
            AMBA_YUV_CONFIG_s     YuvCfg;

            SvcVinErrTask_RcvFlagGet(pProcInfo->ProcFlg, AMBA_KAL_WAIT_FOREVER);

            for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
                if ((pRcvCmd->VinSelectBits & SvcVinErrTask_BitGet(VinID)) > 0U) {
                    for (CfgIdx = 0U; CfgIdx < pVinSrc->CfgNum; CfgIdx ++) {
                        if (pVinSrc->InitCfgArr[CfgIdx].Config.VinID == VinID) {

                            if (pVinSrc->InitCfgArr[CfgIdx].Config.SrcType == SVC_VIN_SRC_SENSOR) {

                                AmbaSvcWrap_MisraMemset(&SsChan, 0, sizeof(SsChan));
                                SsChan.VinID    = pVinSrc->InitCfgArr[CfgIdx].Config.VinID;
                                SsChan.SensorID = pVinSrc->InitCfgArr[CfgIdx].Config.SrcBits;

                                AmbaSvcWrap_MisraMemset(&SsCfg, 0, sizeof(AMBA_SENSOR_CONFIG_s));
                                SsCfg.ModeID        = pVinSrc->InitCfgArr[CfgIdx].Config.SrcMode;
                                SsCfg.ModeID_1      = pVinSrc->InitCfgArr[CfgIdx].Config.SrcMode_1;
                                SsCfg.ModeID_2      = pVinSrc->InitCfgArr[CfgIdx].Config.SrcMode_2;
                                SsCfg.ModeID_3      = pVinSrc->InitCfgArr[CfgIdx].Config.SrcMode_3;
                                SsCfg.Reserved0[0U] = (UINT8)pVinSrc->InitCfgArr[CfgIdx].Config.DisableMasterSync;
                                SsCfg.Reserved0[1U] = (UINT8)pVinSrc->InitCfgArr[CfgIdx].Config.VsyncDelay;
#ifdef CONFIG_MX01_IMX390_ASIL_ENABLED
                                SsCfg.EnableEmbDataCap = 1U;
#endif

                                PRN_VIN_ERR_LOG "---- %sDisable VinID(%d), SensorID(0x%x) Sensor Driver%s ----"
                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_2 PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT32 SsChan.VinID               PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT32 SsChan.SensorID            PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_DBG1
                                PRN_VIN_ERR_LOG "    ModeID       : %d" PRN_VIN_ERR_ARG_UINT32 SsCfg.ModeID        PRN_VIN_ERR_ARG_POST PRN_VIN_ERR_DBG1
                                PRN_VIN_ERR_LOG "    ModeID_1     : %d" PRN_VIN_ERR_ARG_UINT32 SsCfg.ModeID_1      PRN_VIN_ERR_ARG_POST PRN_VIN_ERR_DBG1
                                PRN_VIN_ERR_LOG "    ModeID_2     : %d" PRN_VIN_ERR_ARG_UINT32 SsCfg.ModeID_2      PRN_VIN_ERR_ARG_POST PRN_VIN_ERR_DBG1
                                PRN_VIN_ERR_LOG "    ModeID_3     : %d" PRN_VIN_ERR_ARG_UINT32 SsCfg.ModeID_3      PRN_VIN_ERR_ARG_POST PRN_VIN_ERR_DBG1
                                PRN_VIN_ERR_LOG "    Reserved0[0U]: %d" PRN_VIN_ERR_ARG_UINT32 SsCfg.Reserved0[0U] PRN_VIN_ERR_ARG_POST PRN_VIN_ERR_DBG1
                                PRN_VIN_ERR_LOG "    Reserved0[1U]: %d" PRN_VIN_ERR_ARG_UINT32 SsCfg.Reserved0[1U] PRN_VIN_ERR_ARG_POST PRN_VIN_ERR_DBG1

                                PRetVal = AmbaSensor_Config(&SsChan, &SsCfg);
                                if (PRetVal != 0U) {
                                    PRN_VIN_ERR_LOG "Fail to proc %s - config VinID(%d), SensorID(0x%x) sensor driver!"
                                        PRN_VIN_ERR_ARG_CSTR   pProcInfo->ProcName PRN_VIN_ERR_ARG_POST
                                        PRN_VIN_ERR_ARG_UINT32 SsChan.VinID        PRN_VIN_ERR_ARG_POST
                                        PRN_VIN_ERR_ARG_UINT32 SsChan.SensorID     PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_DBG1
                                }

                            } else if (pVinSrc->InitCfgArr[CfgIdx].Config.SrcType == SVC_VIN_SRC_YUV) {

                                AmbaSvcWrap_MisraMemset(&YuvChan, 0, sizeof(YuvChan));
                                YuvChan.VinID = pVinSrc->InitCfgArr[CfgIdx].Config.VinID;

                                AmbaSvcWrap_MisraMemset(&YuvCfg, 0, sizeof(YuvCfg));
                                YuvCfg.ModeID = pVinSrc->InitCfgArr[CfgIdx].Config.SrcMode;

                                PRN_VIN_ERR_LOG "---- %sConfig VinID(%d) YUV Driver%s SrcMode(%d) ----"
                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_2 PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT32 YuvChan.VinID              PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT32 YuvCfg.ModeID              PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_DBG1

                                PRetVal = AmbaYuv_Config(&YuvChan, &YuvCfg);
                                if (PRetVal != 0U) {
                                    PRN_VIN_ERR_LOG "Fail to proc %s - config VinID(%d) yuv driver!"
                                        PRN_VIN_ERR_ARG_CSTR   pProcInfo->ProcName PRN_VIN_ERR_ARG_POST
                                        PRN_VIN_ERR_ARG_UINT32 YuvChan.VinID       PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_DBG1
                                }

                            } else {
                                //
                            }

                            break;
                        }
                    }
                }
            }

            PRN_VIN_ERR_LOG "Config VinBits(0x%x) driver done!"
                PRN_VIN_ERR_ARG_UINT32 pRcvCmd->VinSelectBits PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API
        }

        AmbaMisra_TouchUnused(&PRetVal);
        AmbaMisra_TouchUnused(pRcvCmd);
        AmbaMisra_TouchUnused(pProcInfo);
    }


    AmbaMisra_TouchUnused(&RetVal);
    return SVC_OK;
}

static UINT32 SvcVinErrTask_RcvProcCfgVin(UINT32 PreChkVal, const void *pCmd, const void *pProc)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (PreChkVal == SVC_OK) {
        SVC_VIN_ERR_RECOVER_CMD_t *pRcvCmd;
        SVC_VIN_ERR_RCV_PROC_s    *pProcInfo;

        AmbaMisra_TypeCast(&(pRcvCmd), &(pCmd));
        AmbaMisra_TypeCast(&(pProcInfo), &(pProc));

        if (pRcvCmd == NULL) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to proc recover - invalid recover command!" PRN_VIN_ERR_NG
        } else if (pProcInfo == NULL) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to proc recover - invalid recover process info!" PRN_VIN_ERR_NG
        } else {
            UINT32           VinID;
            UINT32           VinSrcIdx, VinNum = 0U, VinBits;
            UINT32           SubChnIdx;
            SVC_LIV_VINCAP_s VinCapCfg[AMBA_DSP_MAX_VIN_NUM];

            SvcVinErrTask_RcvFlagGet(pProcInfo->ProcFlg, AMBA_KAL_WAIT_FOREVER);

            AmbaSvcWrap_MisraMemset(VinCapCfg, 0, sizeof(VinCapCfg));
            SvcInfoPack_LivVinCap(&VinNum, &VinBits, VinCapCfg);

            for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
                if ((pRcvCmd->VinSelectBits & SvcVinErrTask_BitGet(VinID)) > 0U) {
                    for ( VinSrcIdx = 0U; VinSrcIdx < VinNum; VinSrcIdx ++ ) {
                        if (VinCapCfg[VinSrcIdx].VinID == VinID) {

                            PRN_VIN_ERR_LOG "---- %sConfig VinID(%d) Vin Capture%s ----"
                                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_2 PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_UINT32 VinCapCfg[VinSrcIdx].VinID PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_DBG1

                            if ( ( SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_LOG_DBG1 ) > 0U ) {

                                PRN_VIN_ERR_LOG "       SubChNum(%d)"
                                    PRN_VIN_ERR_ARG_UINT32 VinCapCfg[VinSrcIdx].SubChNum PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_DBG1
                                PRN_VIN_ERR_LOG "       CaptureWindow Index | OffsetX | OffsetY |  Width | Height" PRN_VIN_ERR_DBG1

                                for (SubChnIdx = 0U; SubChnIdx < VinCapCfg[VinSrcIdx].SubChNum; SubChnIdx ++) {
                                    if (SubChnIdx < AMBA_DSP_MAX_VIRT_CHAN_NUM) {
                                        PRN_VIN_ERR_LOG "                         %d |    %4d |    %4d |   %4d |   %4d"
                                            PRN_VIN_ERR_ARG_UINT32 SubChnIdx                                                      PRN_VIN_ERR_ARG_POST
                                            PRN_VIN_ERR_ARG_UINT32 VinCapCfg[VinSrcIdx].SubChCfg[SubChnIdx].CaptureWindow.OffsetX PRN_VIN_ERR_ARG_POST
                                            PRN_VIN_ERR_ARG_UINT32 VinCapCfg[VinSrcIdx].SubChCfg[SubChnIdx].CaptureWindow.OffsetY PRN_VIN_ERR_ARG_POST
                                            PRN_VIN_ERR_ARG_UINT32 VinCapCfg[VinSrcIdx].SubChCfg[SubChnIdx].CaptureWindow.Width   PRN_VIN_ERR_ARG_POST
                                            PRN_VIN_ERR_ARG_UINT32 VinCapCfg[VinSrcIdx].SubChCfg[SubChnIdx].CaptureWindow.Height  PRN_VIN_ERR_ARG_POST
                                        PRN_VIN_ERR_DBG1
                                    }
                                }
                            }

                            PRetVal = AmbaDSP_LiveviewConfigVinCapture((UINT16)VinCapCfg[VinSrcIdx].VinID,
                                                                       (UINT16)VinCapCfg[VinSrcIdx].SubChNum,
                                                                       (VinCapCfg[VinSrcIdx].SubChCfg));
                            if (PRetVal != 0U) {
                                RetVal = SVC_NG;
                                PRN_VIN_ERR_LOG "Fail to process vin recover - configure VinID(%d) capture window fail!"
                                    PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_NG
                            } else {
                                PRetVal = AmbaDSP_LiveviewConfigVinPost(AMBA_DSP_VIN_CONFIG_POST, (UINT16)VinCapCfg[VinSrcIdx].VinID);
                                if (PRetVal != 0U) {
                                    PRN_VIN_ERR_LOG "Fail to process vin recover - configure VinID(%d) post fail!"
                                        PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_NG
                                }
                            }
                        }

                        if (RetVal != 0U) {
                            break;
                        }

                    }
                }

                if (RetVal != 0U) {
                    break;
                }
            }

            if (RetVal == SVC_OK) {
                PRN_VIN_ERR_LOG "Configure VinBits(0x%x) capture window done!"
                    PRN_VIN_ERR_ARG_UINT32 pRcvCmd->VinSelectBits PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_API
            }
        }

        AmbaMisra_TouchUnused(&PRetVal);
        AmbaMisra_TouchUnused(pRcvCmd);
        AmbaMisra_TouchUnused(pProcInfo);
    }

    return RetVal;

}

static UINT32 SvcVinErrTask_RcvProcCfgIdsp(UINT32 PreChkVal, const void *pCmd, const void *pProc)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (PreChkVal == SVC_OK) {
        SVC_VIN_ERR_RECOVER_CMD_t *pRcvCmd;
        SVC_VIN_ERR_RCV_PROC_s    *pProcInfo;

        AmbaMisra_TypeCast(&(pRcvCmd), &(pCmd));
        AmbaMisra_TypeCast(&(pProcInfo), &(pProc));

        if (pRcvCmd == NULL) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to proc recover - invalid recover command!" PRN_VIN_ERR_NG
        } else if (pProcInfo == NULL) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to proc recover - invalid recover process info!" PRN_VIN_ERR_NG
        } else {
            UINT32 VinID;

            SvcVinErrTask_RcvFlagGet(pProcInfo->ProcFlg, AMBA_KAL_WAIT_FOREVER);

            for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
                if ((pRcvCmd->VinSelectBits & SvcVinErrTask_BitGet(VinID)) > 0U) {
                    PRN_VIN_ERR_LOG "---- %sConfig VinID(%d) Idsp Setting%s ----"
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_2 PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_DBG1

                    /* stop image framework aaa */
                    SvcVinErrTask_ImgFrwkAaaUpd(VinID, 0U);

                    /* Re-configure ik */
                    SvcVinErrTask_RcvIKConfig(VinID);
                    PRN_VIN_ERR_LOG "Re-configure VinID(%d) related ik context done!"
                        PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_API

                    /* Re-configure calib */
                    SvcVinErrTask_RcvCalibConfig(VinID);
                    PRN_VIN_ERR_LOG "Re-configure VinID(%d) related calib data done!"
                        PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_API

                    /* Enable Statistic task */
                    PRN_VIN_ERR_LOG "---- %sEnable VinID(%d) Statistic Task%s ----"
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_2 PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_DBG1
                    SvcVinErrTask_StatisticUpd(VinID, 1U);

                    /* Enable image task */
                    PRN_VIN_ERR_LOG "---- %sEnable VinID(%d) Image Sync Task%s ----"
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_2 PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_DBG1
                    SvcVinErrTask_ImgFrwkSyncUpd(VinID, 1U);

                    /* re-start image framework aaa */
                    PRN_VIN_ERR_LOG "---- %sEnable VinID(%d) Image AAA Task%s ----"
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_2 PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_DBG1
                    SvcVinErrTask_ImgFrwkAaaUpd(VinID, 1U);

                    PRN_VIN_ERR_LOG "Configure VinID(%d) Idsp done!"
                        PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_API
                }
            }
        }

        AmbaMisra_TouchUnused(&PRetVal);
        AmbaMisra_TouchUnused(pRcvCmd);
        AmbaMisra_TouchUnused(pProcInfo);
    }

    return RetVal;


}

static UINT32 SvcVinErrTask_RcvProcEnaLnkUpd(UINT32 PreChkVal, const void *pCmd, const void *pProc)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    SVC_VIN_ERR_RECOVER_CMD_t *pRcvCmd;
    SVC_VIN_ERR_RCV_PROC_s    *pProcInfo;

    AmbaMisra_TypeCast(&(pRcvCmd), &(pCmd));
    AmbaMisra_TypeCast(&(pProcInfo), &(pProc));

    if (pRcvCmd == NULL) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to proc recover - invalid recover command!" PRN_VIN_ERR_NG
    } else if (pProcInfo == NULL) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to proc recover - invalid recover process info!" PRN_VIN_ERR_NG
    } else {
        UINT32 VinID;

        SvcVinErrTask_RcvFlagGet(pProcInfo->ProcFlg, AMBA_KAL_WAIT_FOREVER);

        for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
            if ((pRcvCmd->VinSelectBits & SvcVinErrTask_BitGet(VinID)) > 0U) {

                /* Enable serdes link update */
                PRN_VIN_ERR_LOG "---- %sEnable VinID(%d) SerDes Link Update%s ----"
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_2 PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_DBG1

                SvcVinErrTask_LinkStateUpdEna(VinID, SVC_VIN_ERR_LNK_UPD_ON);

                PRN_VIN_ERR_LOG "Enable VinID(%d) SerDes Query done"
                    PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_API
            }
        }
    }

    AmbaMisra_TouchUnused(&PRetVal);
    AmbaMisra_TouchUnused(pRcvCmd);
    AmbaMisra_TouchUnused(pProcInfo);
    AmbaMisra_TouchUnused(&PreChkVal);

    return RetVal;
}

static UINT32 SvcVinErrTask_RcvProcDisLnkUpd(UINT32 PreChkVal, const void *pCmd, const void *pProc)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    SVC_VIN_ERR_RECOVER_CMD_t *pRcvCmd;
    SVC_VIN_ERR_RCV_PROC_s    *pProcInfo;

    AmbaMisra_TypeCast(&(pRcvCmd), &(pCmd));
    AmbaMisra_TypeCast(&(pProcInfo), &(pProc));

    if (pRcvCmd == NULL) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to proc recover - invalid recover command!" PRN_VIN_ERR_NG
    } else if (pProcInfo == NULL) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to proc recover - invalid recover process info!" PRN_VIN_ERR_NG
    } else {
        UINT32 VinID;

        SvcVinErrTask_RcvFlagGet(pProcInfo->ProcFlg, AMBA_KAL_WAIT_FOREVER);

        for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
            if ((pRcvCmd->VinSelectBits & SvcVinErrTask_BitGet(VinID)) > 0U) {
                /* Disable serdes link update */
                PRN_VIN_ERR_LOG "---- %sDisable VinID(%d) SerDes Link Update%s ----"
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_2 PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_DBG1

                SvcVinErrTask_LinkStateUpdEna(VinID, SVC_VIN_ERR_LNK_UPD_OFF);

                PRN_VIN_ERR_LOG "Disable VinID(%d) SerDes Query done"
                    PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_API
            }
        }
    }

    AmbaMisra_TouchUnused(&PRetVal);
    AmbaMisra_TouchUnused(pRcvCmd);
    AmbaMisra_TouchUnused(pProcInfo);
    AmbaMisra_TouchUnused(&PreChkVal);

    return RetVal;
}

static UINT32 SvcVinErrTask_RcvProcReCfgVout(UINT32 PreChkVal, const void *pCmd, const void *pProc)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (PreChkVal == SVC_OK) {
        SVC_VIN_ERR_RECOVER_CMD_t *pRcvCmd;
        SVC_VIN_ERR_RCV_PROC_s    *pProcInfo;

        AmbaMisra_TypeCast(&(pRcvCmd), &(pCmd));
        AmbaMisra_TypeCast(&(pProcInfo), &(pProc));

        if (pRcvCmd == NULL) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to proc recover - invalid recover command!" PRN_VIN_ERR_NG
        } else if (pProcInfo == NULL) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to proc recover - invalid recover process info!" PRN_VIN_ERR_NG
        } else {
            SvcVinErrTask_RcvFlagGet(pProcInfo->ProcFlg, AMBA_KAL_WAIT_FOREVER);

#ifdef CONFIG_SVC_VOUT_ERR_CTRL_USED
            {
                UINT32 Idx;
                UINT32 ReqVoutBits = 0U;
                const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();

                if (pCfg != NULL) {
                    ReqVoutBits = pCfg->DispBits;
                }

                for (Idx = 0U; Idx < AMBA_DSP_MAX_VOUT_NUM; Idx ++) {
                    if ((ReqVoutBits & SvcVinErrTask_BitGet(Idx)) > 0U) {
                        PRN_VIN_ERR_LOG "---- %sRe-Config AmbaFPD driver%s ----"
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_2 PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_DBG1

                        if (0U != SvcVoutErrTask_RcvFpd(Idx)) {
                            RetVal = SVC_NG;
                            PRN_VIN_ERR_LOG "Fail to proc %s - re-config AmbaFPD driver fail!"
                                PRN_VIN_ERR_ARG_CSTR   pProcInfo->ProcName PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_NG
                        }

                        PRN_VIN_ERR_LOG "Re-Config AmbaFPD driver done!" PRN_VIN_ERR_API
                    }
                }
            }
#else
            PRN_VIN_ERR_LOG "Enable vout err ctrl first!" PRN_VIN_ERR_API
#endif
        }

        AmbaMisra_TouchUnused(&PRetVal);
        AmbaMisra_TouchUnused(pRcvCmd);
        AmbaMisra_TouchUnused(pProcInfo);
    }

    return RetVal;
}

#ifdef CONFIG_BUILD_MONFRW_GRAPH
static void SvcVinErrTask_MonMsgProcRawRdy(const void *pMsg, UINT64 Flag, UINT32 MsgType)
{
    UINT32 PRetVal;
    UINT32 VinID = 0xFFFFU;
    const AMBA_DSP_RAW_DATA_RDY_s *pRawRdy = NULL;
    const SVC_VIN_ERR_MON_MSG_s  *pMsgCtrl   = NULL;

    if (SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl != NULL) {
        if (SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl[VIN_ERR_MON_PORT_TIMELINE].pMsgList != NULL) {
            pMsgCtrl = &(SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl[VIN_ERR_MON_PORT_TIMELINE].pMsgList[MON_TIMELINE_MSG_RAW_DATA]);
        }
    }

    AmbaMisra_TypeCast(&(pRawRdy), &(pMsg));

    if (pMsgCtrl == NULL) {
        PRN_VIN_ERR_LOG "Fail to proc raw ready msg - invalid msg control!" PRN_VIN_ERR_NG
    } else if ((pMsgCtrl->MsgFlag & Flag) == 0ULL) {
        PRN_VIN_ERR_LOG "Fail to proc raw ready msg - wrong flag!" PRN_VIN_ERR_NG
    } else if (pRawRdy == NULL) {
        PRN_VIN_ERR_LOG "Fail to proc raw ready msg - invalid msg!" PRN_VIN_ERR_NG
    } else {
        VinID = (UINT32)(pRawRdy->VinId); VinID &= 0xFFU;

        if ((MsgType & VIN_ERR_MON_MSG_TIMEOUT) > 0U) {
            PRN_VIN_ERR_LOG "Monitor VinID(%s%d%s) raw data ready msg timeout!"
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_DBG2
        } else if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
            PRN_VIN_ERR_LOG "Fail to proc raw ready msg - invalid VinID(%d)!"
                PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        } else if ((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(VinID)) == 0U) {
            PRN_VIN_ERR_LOG "Fail to proc raw ready msg - Not supported VinID(%d)! CurSelectBits 0x%x"
                PRN_VIN_ERR_ARG_UINT32 VinID                       PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.VinSelectBits PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        } else {
            UINT32 IsDefRaw;
            UINT8 *pRawBuf = NULL;

            IsDefRaw = (UINT32)(pRawRdy->IsVirtChan); IsDefRaw &= 0xFFU;

            if ((pRawRdy->RawBuffer.BaseAddr > 0U) &&   // raw data address should not zero
                ((IsDefRaw & 0x4UL) == 0U)) {              // do not monitor default raw content

                AmbaMisra_TypeCast(&(pRawBuf), &(pRawRdy->RawBuffer.BaseAddr));

                PRN_VIN_ERR_LOG "Monitor VinID(%s%d%s) raw data ready %s%04u%s / %s%04u%s / %s%04u%s, %s%p%s"
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1       PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 VinID                            PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END           PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0       PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 pRawRdy->RawBuffer.Pitch         PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END           PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0       PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 pRawRdy->RawBuffer.Window.Width  PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END           PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0       PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 pRawRdy->RawBuffer.Window.Height PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END           PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0       PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CPOINT pRawBuf                          PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END           PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_DBG2

                if ((SvcVinErrCtrl.ErrState[VinID].FovFreezeSrc == SVC_VIN_ERR_FOV_FREZE_SRC_RAW) &&
                    ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_RAW_FRZ_OFF) == 0U)) {
                    SVC_VIN_ERR_RAW_FRZ_UPD_s RawDataUpd;

                    AmbaSvcWrap_MisraMemset(&RawDataUpd, 0, sizeof(RawDataUpd));
                    RawDataUpd.VinID   = VinID;
                    RawDataUpd.pRawBuf = pRawBuf;
                    RawDataUpd.Pitch   = (UINT32)(pRawRdy->RawBuffer.Pitch);         RawDataUpd.Pitch  &= 0xFFFFU;
                    RawDataUpd.Width   = (UINT32)(pRawRdy->RawBuffer.Window.Width);  RawDataUpd.Width  &= 0xFFFFU;
                    RawDataUpd.Height  = (UINT32)(pRawRdy->RawBuffer.Window.Height); RawDataUpd.Height &= 0xFFFFU;
                    PRetVal = SvcVinErrTask_SendMsg(SVC_VIN_ERR_CTRL_CMD_RAW_UPD, &RawDataUpd, (UINT32)sizeof(RawDataUpd), AMBA_KAL_NO_WAIT);
                    if (PRetVal != 0U) {
                        PRN_VIN_ERR_LOG "Fail to proc raw ready msg - push raw data msg to queue fail! ErrCode(0x%08x)"
                            PRN_VIN_ERR_ARG_UINT32 PRetVal PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_NG
                    }
                }
            }

            PRetVal = AmbaKAL_EventFlagSet(&(SvcVinErrCtrl.ErrState[VinID].VinStateFlag), SVC_VIN_ERR_VIN_STATE_RAW_RDY); PRN_VIN_ERR_HDLR
        }
    }
}

static void SvcVinErrTask_MonMsgProcRawStart(const void *pMsg, UINT64 Flag, UINT32 MsgType)
{
    UINT32 PRetVal;
    const AMBA_MON_DSP_RAW_STATE_s *pRawStartMsg = NULL;
    const SVC_VIN_ERR_MON_MSG_s    *pMsgCtrl     = NULL;

    if (SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl != NULL) {
        if (SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl[VIN_ERR_MON_PORT_VIN_STATE].pMsgList != NULL) {
            pMsgCtrl = &(SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl[VIN_ERR_MON_PORT_VIN_STATE].pMsgList[MON_VIN_STAT_MSG_RAW_START]);
        }
    }

    AmbaMisra_TypeCast(&(pRawStartMsg), &(pMsg));

    if (pMsgCtrl == NULL) {
        PRN_VIN_ERR_LOG "Fail to proc raw start msg - invalid msg control!" PRN_VIN_ERR_NG
    } else if ((pMsgCtrl->MsgFlag & Flag) == 0ULL) {
        PRN_VIN_ERR_LOG "Fail to proc raw start msg - wrong flag!" PRN_VIN_ERR_NG
    } else if (pRawStartMsg == NULL) {
        PRN_VIN_ERR_LOG "Fail to proc raw start msg - invalid msg!" PRN_VIN_ERR_NG
    } else if (pRawStartMsg->VinId >= AMBA_DSP_MAX_VIN_NUM) {
        PRN_VIN_ERR_LOG "Fail to proc raw start msg - invalid VinID(%d)!"
            PRN_VIN_ERR_ARG_UINT32 pRawStartMsg->VinId PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_NG
    } else if ((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(pRawStartMsg->VinId)) == 0U) {
        PRN_VIN_ERR_LOG "Fail to proc raw start msg - Not supported VinID(%d)! CurSelectBits 0x%x"
            PRN_VIN_ERR_ARG_UINT32 pRawStartMsg->VinId         PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.VinSelectBits PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_NG
    } else if ((MsgType & VIN_ERR_MON_MSG_TIMEOUT) > 0U) {
        PRN_VIN_ERR_LOG "Monitor VinID(%s%d%s) raw start msg timeout!"
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 pRawStartMsg->VinId        PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_DBG2
    } else {
        PRN_VIN_ERR_LOG "Start VinID(%s%d%s) raw!"
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 pRawStartMsg->VinId        PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_API

        PRetVal = AmbaKAL_EventFlagClear(&(SvcVinErrCtrl.ErrState[pRawStartMsg->VinId].VinStateFlag), SVC_VIN_ERR_VIN_STATE_RAW_TOUT);
        if (PRetVal != 0U) {
            PRN_VIN_ERR_LOG "Fail to proc raw start msg - clear event flag fail! ErrCode(0x%08x)"
                PRN_VIN_ERR_ARG_UINT32 PRetVal PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        }

        PRetVal = AmbaKAL_EventFlagSet(&(SvcVinErrCtrl.VinLivFlag), SvcVinErrTask_BitGet(pRawStartMsg->VinId));
        if (PRetVal != 0U) {
            UINT32 LivFlag = SvcVinErrTask_BitGet(pRawStartMsg->VinId);
            PRN_VIN_ERR_LOG "Fail to proc raw start msg - set vin liv event flag 0x%08x fail! ErrCode(0x%08x)"
                PRN_VIN_ERR_ARG_UINT32 LivFlag PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 PRetVal PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        }

        PRetVal = AmbaKAL_EventFlagSet(&(SvcVinErrCtrl.ErrState[pRawStartMsg->VinId].VinStateFlag), SVC_VIN_ERR_VIN_STATE_LIV);
        if (PRetVal != 0U) {
            PRN_VIN_ERR_LOG "Fail to proc raw start msg - set event flag fail! ErrCode(0x%08x)"
                PRN_VIN_ERR_ARG_UINT32 PRetVal PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        } else {
            SvcVinErrTask_FovStateUpd(pRawStartMsg->VinId);
        }
    }
}

static void SvcVinErrTask_MonMsgProcRawTout(const void *pMsg, UINT64 Flag, UINT32 MsgType)
{
    UINT32 PRetVal;
    const AMBA_MON_DSP_RAW_STATE_s *pRawToutMsg  = NULL;
    const SVC_VIN_ERR_MON_MSG_s    *pMsgCtrl     = NULL;

    if (SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl != NULL) {
        if (SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl[VIN_ERR_MON_PORT_VIN_STATE].pMsgList != NULL) {
            pMsgCtrl = &(SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl[VIN_ERR_MON_PORT_VIN_STATE].pMsgList[MON_VIN_STAT_MSG_RAW_TOUT]);
        }
    }

    AmbaMisra_TypeCast(&(pRawToutMsg), &(pMsg));
    AmbaMisra_TouchUnused(&MsgType);

    if (pMsgCtrl == NULL) {
        PRN_VIN_ERR_LOG "Fail to proc raw timeout msg - invalid msg control!" PRN_VIN_ERR_NG
    } else if ((pMsgCtrl->MsgFlag & Flag) == 0ULL) {
        PRN_VIN_ERR_LOG "Fail to proc raw timeout msg - wrong flag!" PRN_VIN_ERR_NG
    } else if (pRawToutMsg == NULL) {
        PRN_VIN_ERR_LOG "Fail to proc raw timeout msg - invalid msg!" PRN_VIN_ERR_NG
    } else if (pRawToutMsg->VinId >= AMBA_DSP_MAX_VIN_NUM) {
        PRN_VIN_ERR_LOG "Fail to proc raw timeout msg - invalid VinID(%d)!"
            PRN_VIN_ERR_ARG_UINT32 pRawToutMsg->VinId PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_NG
    } else if ((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(pRawToutMsg->VinId)) == 0U) {
        PRN_VIN_ERR_LOG "Fail to proc raw timeout msg - Not supported VinID(%d)! CurSelectBits 0x%x"
            PRN_VIN_ERR_ARG_UINT32 pRawToutMsg->VinId          PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.VinSelectBits PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_NG
    } else {
        PRN_VIN_ERR_LOG "VinID(%s%d%s) raw timeout!"
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 pRawToutMsg->VinId         PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_DBG1

        PRetVal = AmbaKAL_EventFlagSet(&(SvcVinErrCtrl.ErrState[pRawToutMsg->VinId].VinStateFlag),
                                       SVC_VIN_ERR_VIN_STATE_RAW_TOUT);
        if (PRetVal != 0U) {
            PRN_VIN_ERR_LOG "Fail to proc raw timeout msg - set event flag fail! ErrCode(0x%08x)"
                PRN_VIN_ERR_ARG_UINT32 PRetVal PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        }

    }
}

static void SvcVinErrTask_MonMsgProcDefRStart(const void *pMsg, UINT64 Flag, UINT32 MsgType)
{
    const AMBA_MON_DSP_RAW_STATE_s *pDefRawMsg = NULL;
    const SVC_VIN_ERR_MON_MSG_s    *pMsgCtrl   = NULL;

    if (SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl != NULL) {
        if (SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl[VIN_ERR_MON_PORT_VIN_STATE].pMsgList != NULL) {
            pMsgCtrl = &(SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl[VIN_ERR_MON_PORT_VIN_STATE].pMsgList[MON_VIN_STAT_MSG_DEFRAW_START]);
        }
    }

    AmbaMisra_TypeCast(&(pDefRawMsg), &(pMsg));

    if (pMsgCtrl == NULL) {
        PRN_VIN_ERR_LOG "Fail to proc def raw start msg - invalid msg control!" PRN_VIN_ERR_NG
    } else if ((pMsgCtrl->MsgFlag & Flag) == 0ULL) {
        PRN_VIN_ERR_LOG "Fail to proc def raw start msg - wrong flag!" PRN_VIN_ERR_NG
    } else if (pDefRawMsg == NULL) {
        PRN_VIN_ERR_LOG "Fail to proc def raw start msg - invalid msg!" PRN_VIN_ERR_NG
    } else if (pDefRawMsg->VinId >= AMBA_DSP_MAX_VIN_NUM) {
        PRN_VIN_ERR_LOG "Fail to proc def raw start msg - invalid VinID(%d)!"
            PRN_VIN_ERR_ARG_UINT32 pDefRawMsg->VinId PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_NG
    } else if ((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(pDefRawMsg->VinId)) == 0U) {
        PRN_VIN_ERR_LOG "Fail to proc def raw start msg - Not supported VinID(%d)! CurSelectBits 0x%x"
            PRN_VIN_ERR_ARG_UINT32 pDefRawMsg->VinId           PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.VinSelectBits PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_NG
    } else if ((MsgType & VIN_ERR_MON_MSG_TIMEOUT) > 0U) {
        PRN_VIN_ERR_LOG "Monitor VinID(%s%d%s) default raw msg timeout!"
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 pDefRawMsg->VinId          PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_DBG2
    } else {
        UINT32 VinID = pDefRawMsg->VinId;

        PRN_VIN_ERR_LOG "VinID(%s%d%s) def raw start!"
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_DBG1
    }
}

static void SvcVinErrTask_MonMsgProcLinkState(const void *pMsg, UINT64 Flag, UINT32 MsgType)
{
    const AMBA_MON_VIN_SERDES_STATE_s *pLinkStateMsg = NULL;
    const SVC_VIN_ERR_MON_MSG_s       *pMsgCtrl      = NULL;

    if (SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl != NULL) {
        if (SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl[VIN_ERR_MON_PORT_VIN_STATE].pMsgList != NULL) {
            pMsgCtrl = &(SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl[VIN_ERR_MON_PORT_VIN_STATE].pMsgList[MON_VIN_STAT_MSG_SERLNK_STATE]);
        }
    }

    AmbaMisra_TypeCast(&(pLinkStateMsg), &(pMsg));

    if (pMsgCtrl == NULL) {
        PRN_VIN_ERR_LOG "Fail to proc link state msg - invalid msg control!" PRN_VIN_ERR_NG
    } else if ((pMsgCtrl->MsgFlag & Flag) == 0ULL) {
        PRN_VIN_ERR_LOG "Fail to proc link state msg - wrong flag!" PRN_VIN_ERR_NG
    } else if (pLinkStateMsg == NULL) {
        PRN_VIN_ERR_LOG "Fail to proc link state msg - invalid msg!" PRN_VIN_ERR_NG
    } else if (pLinkStateMsg->VinId >= AMBA_DSP_MAX_VIN_NUM) {
        PRN_VIN_ERR_LOG "Fail to proc link state msg - invalid VinID(%d)!"
            PRN_VIN_ERR_ARG_UINT32 pLinkStateMsg->VinId PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_NG
    } else if ((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(pLinkStateMsg->VinId)) == 0U) {
        PRN_VIN_ERR_LOG "Fail to proc link state msg - Not supported VinID(%d)! CurSelectBits 0x%x"
            PRN_VIN_ERR_ARG_UINT32 pLinkStateMsg->VinId        PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.VinSelectBits PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_NG
    } else {

        PRN_VIN_ERR_LOG "Monitor VinID(%s%d%s) link state %s0x%x%s %s0x%x%s! %d"
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 pLinkStateMsg->VinId       PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0 PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 pLinkStateMsg->LinkId      PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0 PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 pLinkStateMsg->DataId      PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 MsgType                    PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_DBG1

        SvcVinErrTask_LinkStateChange(pLinkStateMsg->VinId, pLinkStateMsg->LinkId, pLinkStateMsg->DataId );
    }
}

static void SvcVinErrTask_MonMsgProcCfaState(const void *pMsg, UINT64 Flag, UINT32 MsgType)
{
#ifdef SVC_VIN_ERR_FLG_SEN_FRZE_AAA
    UINT32 PRetVal;
    const AMBA_MON_AAA_CFA_STATE_s *pCfaMsg  = NULL;
    const SVC_VIN_ERR_MON_MSG_s    *pMsgCtrl = NULL;

    if (SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl != NULL) {
        if (SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl[VIN_ERR_MON_PORT_VIN_STATE].pMsgList != NULL) {
            pMsgCtrl = &(SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl[VIN_ERR_MON_PORT_VIN_STATE].pMsgList[MON_VIN_STAT_MSG_CFA_STATE]);
        }
    }

    AmbaMisra_TypeCast(&(pCfaMsg), &(pMsg));

    if (pMsgCtrl == NULL) {
        PRN_VIN_ERR_LOG "Fail to proc aaa cfa state msg - invalid msg control!" PRN_VIN_ERR_NG
    } else if ((pMsgCtrl->MsgFlag & Flag) == 0ULL) {
        PRN_VIN_ERR_LOG "Fail to proc aaa cfa state msg - wrong flag!" PRN_VIN_ERR_NG
    } else if (pCfaMsg == NULL) {
        PRN_VIN_ERR_LOG "Fail to proc aaa cfa state msg - invalid msg!" PRN_VIN_ERR_NG
    } else if (pCfaMsg->FovId >= AMBA_DSP_MAX_VIEWZONE_NUM) {
        PRN_VIN_ERR_LOG "Fail to proc aaa cfa msg - invalid FovID(%d)"
            PRN_VIN_ERR_ARG_UINT32 pCfaMsg->FovId PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_NG
    } else if ((MsgType & VIN_ERR_MON_MSG_TIMEOUT) > 0U) {
        PRN_VIN_ERR_LOG "Monitor FovID(%s%d%s) cfa statistic msg timeout!"
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 pCfaMsg->FovId             PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_DBG2
    } else {
        UINT32 VinID = 0xFFFFFFFFU;

        if (0U == SvcResCfg_GetVinIDOfFovIdx(pCfaMsg->FovId, &VinID)) {

            PRN_VIN_ERR_LOG "Monitor VinID(%s%d%s) FovID(%s%d%s) aaa cfa cd zero state %s0x%x%s!"
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1    PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 VinID                         PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END        PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1    PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 pCfaMsg->FovId                PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END        PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0    PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 pCfaMsg->StatusId.Bits.CdZero PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END        PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_DBG1

            if (VinID < AMBA_DSP_MAX_VIN_NUM) {
                if (SvcVinErrCtrl.ErrState[VinID].FovFreezeSrc == SVC_VIN_ERR_FOV_FREZE_SRC_AAA) {
                    SVC_VIN_ERR_SENS_UPD_s SensorFreezeUpd;

                    AmbaSvcWrap_MisraMemset(&SensorFreezeUpd, 0, sizeof(SensorFreezeUpd));
                    SensorFreezeUpd.FovID  = pCfaMsg->FovId;
                    SensorFreezeUpd.CdZero = pCfaMsg->StatusId.Bits.CdZero;
                    PRetVal = SvcVinErrTask_SendMsg(SVC_VIN_ERR_CTRL_CMD_SENS_UPD, &SensorFreezeUpd, (UINT32)sizeof(SensorFreezeUpd), AMBA_KAL_NO_WAIT);
                    if (PRetVal != 0U) {
                        PRN_VIN_ERR_LOG "Fail to proc aaa cfa msg - send to queue fail. ErrCode(0x%08x)"
                            PRN_VIN_ERR_ARG_UINT32 PRetVal PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_NG
                    }
                }
            }
        }
    }
#else
    if (pMsg != NULL) {
        AmbaMisra_TouchUnused(&Flag);
        AmbaMisra_TouchUnused(&MsgType);
    }
#endif
}

static void SvcVinErrTask_MonMsgProcVinTout(const void *pMsg, UINT64 Flag, UINT32 MsgType)
{
    UINT32 PRetVal;
    const AMBA_MON_DSP_VIN_TIMEOUT_STATE_s *pToutMsg = NULL;
    const SVC_VIN_ERR_MON_MSG_s *pMsgCtrl   = NULL;

    if (SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl != NULL) {
        if (SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl[VIN_ERR_MON_PORT_VIN_STATE].pMsgList != NULL) {
            pMsgCtrl = &(SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl[VIN_ERR_MON_PORT_VIN_STATE].pMsgList[MON_VIN_STAT_MSG_VIN_TOUT]);
        }
    }

    AmbaMisra_TypeCast(&(pToutMsg), &(pMsg));

    if (pMsgCtrl == NULL) {
        PRN_VIN_ERR_LOG "Fail to proc vin timeout msg - invalid msg control!" PRN_VIN_ERR_NG
    } else if ((pMsgCtrl->MsgFlag & Flag) == 0ULL) {
        PRN_VIN_ERR_LOG "Fail to proc vin timeout msg - wrong flag!" PRN_VIN_ERR_NG
    } else if (pToutMsg == NULL) {
        PRN_VIN_ERR_LOG "Fail to proc vin timeout msg - invalid msg!" PRN_VIN_ERR_NG
    } else if (pToutMsg->VinId >= AMBA_DSP_MAX_VIN_NUM) {
        PRN_VIN_ERR_LOG "Fail to proc vin timeout msg - invalid VinID(%d)!"
            PRN_VIN_ERR_ARG_UINT32 pToutMsg->VinId PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_NG
    } else if ((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(pToutMsg->VinId)) == 0U) {
        PRN_VIN_ERR_LOG "Fail to proc vin timeout msg - Not supported VinID(%d)! CurSelectBits 0x%x"
            PRN_VIN_ERR_ARG_UINT32 pToutMsg->VinId             PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.VinSelectBits PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_NG
    } else if ((MsgType & VIN_ERR_MON_MSG_TIMEOUT) > 0U) {
        PRN_VIN_ERR_LOG "Monitor VinID(%s%d%s) default raw msg timeout!"
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 pToutMsg->VinId            PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_DBG2
    } else {
        UINT32 VinID = pToutMsg->VinId;

        PRN_VIN_ERR_LOG "VinID(%s%d%s) timeout!"
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_DBG1

        // Update vin state
        PRetVal = AmbaKAL_EventFlagClear(&(SvcVinErrCtrl.ErrState[VinID].VinStateFlag), SVC_VIN_ERR_VIN_STATE_LIV);
        if (PRetVal != 0U) {
            PRN_VIN_ERR_LOG "Fail to proc vin timeout msg - clear event flag fail! ErrCode(0x%08x)"
                PRN_VIN_ERR_ARG_UINT32 PRetVal PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        }
        PRetVal = AmbaKAL_EventFlagSet(&(SvcVinErrCtrl.ErrState[VinID].VinStateFlag), SVC_VIN_ERR_VIN_STATE_TOUT);
        if (PRetVal != 0U) {
            PRN_VIN_ERR_LOG "Fail to proc vin timeout msg - set event flag fail! ErrCode(0x%08x)"
                PRN_VIN_ERR_ARG_UINT32 PRetVal PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        }

        {
            SVC_VIN_ERR_LINK_UPD_s LinkUpd;

            AmbaSvcWrap_MisraMemset(&LinkUpd, 0, sizeof(LinkUpd));
            LinkUpd.VinID = VinID;
            PRetVal = SvcVinErrTask_SendMsg(SVC_VIN_ERR_CTRL_CMD_VIN_TOUT, &LinkUpd, (UINT32)sizeof(LinkUpd), AMBA_KAL_NO_WAIT);
            if (PRetVal != 0U) {
                PRN_VIN_ERR_LOG "Fail to proc vin timeout msg  - prepare VinID(%d) timeout cmd fail! ErrCode(0x%08x)"
                    PRN_VIN_ERR_ARG_UINT32 VinID   PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 PRetVal PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_NG
            } else {
                PRN_VIN_ERR_LOG "Send VinID(%s%d%s) timeout update!"
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_DBG1
            }
        }
    }
}

static void SvcVinErrTask_MonMsgProcFovLatency(const void *pMsg, UINT64 Flag, UINT32 MsgType)
{
    UINT32 PRetVal;
    const AMBA_MON_DSP_FOV_LATENCY_s *pLatencyMsg = NULL;
    const SVC_VIN_ERR_MON_MSG_s      *pMsgCtrl    = NULL;

    if (SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl != NULL) {
        if (SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl[VIN_ERR_MON_PORT_TIMELINE].pMsgList != NULL) {
            pMsgCtrl = &(SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl[VIN_ERR_MON_PORT_TIMELINE].pMsgList[MON_TIMELINE_MSG_FOV_LTY]);
        }
    }

    AmbaMisra_TypeCast(&(pLatencyMsg), &(pMsg));
    if (pLatencyMsg == NULL) {
        PRN_VIN_ERR_LOG "Fail to proc fov latency msg - invalid msg!" PRN_VIN_ERR_NG
    } else if (pMsgCtrl == NULL) {
        PRN_VIN_ERR_LOG "Fail to proc fov latency msg - invalid msg control!" PRN_VIN_ERR_NG
    } else if ((pMsgCtrl->MsgFlag & Flag) == 0ULL) {
        PRN_VIN_ERR_LOG "Fail to proc fov latency msg - wrong flag!" PRN_VIN_ERR_NG
    } else if ((MsgType & VIN_ERR_MON_MSG_TIMEOUT) > 0U) {
        PRN_VIN_ERR_LOG "Monitor FovID(%d) latency msg timeout!"
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 pLatencyMsg->FovId         PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_DBG4
    } else {
        SVC_VIN_ERR_FOV_LTN_UPD_s FovLatency;

        AmbaSvcWrap_MisraMemset(&FovLatency, 0, sizeof(FovLatency));
        FovLatency.FovID     = pLatencyMsg->FovId;
        FovLatency.YuvSeqNum = pLatencyMsg->YuvSeqNum;
        FovLatency.Latency   = pLatencyMsg->Latency;

        PRetVal = SvcVinErrTask_SendMsg(SVC_VIN_ERR_CTRL_CMD_FLTY_UPD, &FovLatency, (UINT32)sizeof(FovLatency), AMBA_KAL_NO_WAIT);
        if (PRetVal != 0U) {
            PRN_VIN_ERR_LOG "Fail to proc fov latency msg - send to queue fail! ErrCode(0x%08x)"
                PRN_VIN_ERR_ARG_UINT32 PRetVal PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        }
    }
}

static void SvcVinErrTask_MonMsgProcVinVoutLatency(const void *pMsg, UINT64 Flag, UINT32 MsgType)
{
    UINT32 PRetVal;
    const AMBA_MON_DSP_VOUT_LATENCY_s *pLatencyMsg = NULL;
    const SVC_VIN_ERR_MON_MSG_s       *pMsgCtrl    = NULL;

    if (SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl != NULL) {
        if (SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl[VIN_ERR_MON_PORT_TIMELINE].pMsgList != NULL) {
            pMsgCtrl = &(SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl[VIN_ERR_MON_PORT_TIMELINE].pMsgList[MON_TIMELINE_MSG_VOUT_LTY]);
        }
    }

    AmbaMisra_TypeCast(&(pLatencyMsg), &(pMsg));
    if (pLatencyMsg == NULL) {
        PRN_VIN_ERR_LOG "Fail to proc vout latency msg - invalid msg!" PRN_VIN_ERR_NG
    } else if (pMsgCtrl == NULL) {
        PRN_VIN_ERR_LOG "Fail to proc vout latency msg - invalid msg control!" PRN_VIN_ERR_NG
    } else if ((pMsgCtrl->MsgFlag & Flag) == 0ULL) {
        PRN_VIN_ERR_LOG "Fail to proc vout latency msg - wrong flag!" PRN_VIN_ERR_NG
    } else if ((MsgType & VIN_ERR_MON_MSG_TIMEOUT) > 0U) {
        PRN_VIN_ERR_LOG "Monitor VoutID(%s%d%s) latency msg timeout!"
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 pLatencyMsg->VoutId        PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_DBG3
    } else {
        SVC_VIN_ERR_VOUT_LTN_UPD_s VoutLatency;
        UINT32 FovIdx;

        AmbaSvcWrap_MisraMemset(&VoutLatency, 0, sizeof(VoutLatency));
        VoutLatency.VoutID        = pLatencyMsg->VoutId;
        VoutLatency.DispSeqCnt    = pLatencyMsg->DispSeqNum;
        VoutLatency.DispLatency   = (UINT16)(pLatencyMsg->DispLatency);
        VoutLatency.FovSelectBits = pLatencyMsg->FovSelectBits;

        for (FovIdx = 0U; FovIdx < AMBA_DSP_MAX_VIEWZONE_NUM; FovIdx ++) {
            if ((VoutLatency.FovSelectBits & SvcVinErrTask_BitGet(FovIdx)) > 0U) {

                if (pLatencyMsg->Latency[FovIdx] > 65535U) {
                    VoutLatency.FovVoutLatency[FovIdx] = 0xFFFFU;
                } else {
                    VoutLatency.FovVoutLatency[FovIdx] = (UINT16)(pLatencyMsg->Latency[FovIdx]);
                }
            }
        }

        PRetVal = SvcVinErrTask_SendMsg(SVC_VIN_ERR_CTRL_CMD_VLTY_UPD, &VoutLatency, (UINT32)sizeof(VoutLatency), AMBA_KAL_NO_WAIT);
        if (PRetVal != 0U) {
            PRN_VIN_ERR_LOG "Fail to proc vout latency msg - send to queue fail! ErrCode(0x%08x)"
                PRN_VIN_ERR_ARG_UINT32 PRetVal PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        }
    }
}

static void SvcVinErrTask_MonMsgInit(void)
{
    if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_MON_MSG_INIT) == 0U) {

        AmbaSvcWrap_MisraMemset(MonVinStateMsg, 0, sizeof(MonVinStateMsg));
        MonVinStateMsg[MON_VIN_STAT_MSG_RAW_START   ]  = (SVC_VIN_ERR_MON_MSG_s) { 1U, 0ULL, "raw_start",     SvcVinErrTask_MonMsgProcRawStart  };
        MonVinStateMsg[MON_VIN_STAT_MSG_RAW_TOUT    ]  = (SVC_VIN_ERR_MON_MSG_s) { 1U, 0ULL, "raw_timeout",   SvcVinErrTask_MonMsgProcRawTout   };
        MonVinStateMsg[MON_VIN_STAT_MSG_DEFRAW_START]  = (SVC_VIN_ERR_MON_MSG_s) { 1U, 0ULL, "def_raw_start", SvcVinErrTask_MonMsgProcDefRStart };
        MonVinStateMsg[MON_VIN_STAT_MSG_SERLNK_STATE]  = (SVC_VIN_ERR_MON_MSG_s) { 0U, 0ULL, "serdes_link",   SvcVinErrTask_MonMsgProcLinkState };
        MonVinStateMsg[MON_VIN_STAT_MSG_CFA_STATE   ]  = (SVC_VIN_ERR_MON_MSG_s) { 0U, 0ULL, "aaa_cfa",       SvcVinErrTask_MonMsgProcCfaState  };
        MonVinStateMsg[MON_VIN_STAT_MSG_VIN_TOUT    ]  = (SVC_VIN_ERR_MON_MSG_s) { 1U, 0ULL, "vin_timeout",   SvcVinErrTask_MonMsgProcVinTout   };

        AmbaSvcWrap_MisraMemset(MonDspListenMsg, 0, sizeof(MonDspListenMsg));
        MonDspListenMsg[MON_TIMELINE_MSG_RAW_DATA    ] = (SVC_VIN_ERR_MON_MSG_s) { 0U, 0ULL, "raw_rdy",      SvcVinErrTask_MonMsgProcRawRdy         };
        MonDspListenMsg[MON_TIMELINE_MSG_FOV_LTY     ] = (SVC_VIN_ERR_MON_MSG_s) { 1U, 0ULL, "fov_latency",  SvcVinErrTask_MonMsgProcFovLatency     };
        MonDspListenMsg[MON_TIMELINE_MSG_VOUT_LTY    ] = (SVC_VIN_ERR_MON_MSG_s) { 1U, 0ULL, "vout_latency", SvcVinErrTask_MonMsgProcVinVoutLatency };

        AmbaSvcWrap_MisraMemset(MonMsgPortList, 0, sizeof(MonMsgPortList));
        MonMsgPortList[VIN_ERR_MON_PORT_VIN_STATE ] = (SVC_VIN_ERR_MON_PORT_s) { 1U, VIN_ERR_MON_PORT_VIN_STATE, "vin_state",  MON_VIN_STAT_MSG_NUM, MonVinStateMsg };
        MonMsgPortList[VIN_ERR_MON_PORT_TIMELINE]   = (SVC_VIN_ERR_MON_PORT_s) { 1U, VIN_ERR_MON_PORT_TIMELINE,  "timeline",   MON_TIMELINE_MSG_NUM, MonDspListenMsg };

#ifdef CONFIG_BUILD_MONFRW_SERDES
        MonVinStateMsg[MON_VIN_STAT_MSG_SERLNK_STATE].Enable = 1U;
#endif

#ifdef SVC_VIN_ERR_FLG_SEN_FRZE_AAA
        MonVinStateMsg[MON_VIN_STAT_MSG_CFA_STATE].Enable = 1U;
        SafetyListenAaa_CdEnable(1U, MON_VIN_STAT_MSG_CFA_INTVAL, MON_VIN_STAT_MSG_CFA_SHIFT);
#endif

#ifdef SVC_VIN_ERR_FLG_SEN_FRZE_RAW
        MonDspListenMsg[MON_TIMELINE_MSG_RAW_DATA].Enable = 1U;
#endif

        SvcVinErrCtrlFlg |= SVC_VIN_ERR_FLG_MON_MSG_INIT;
    }
}

static UINT32 SvcVinErrTask_MonFrwkCfg(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    SVC_VIN_ERR_MON_CTRL_s *pMonFrwkCtrl = &(SvcVinErrCtrl.MonFrwkCtrl);
    UINT32 PortIdx;
    SVC_VIN_ERR_MON_TASK_s *pMonFrwkTask = &(SvcVinErrCtrl.MonFrwkTask);
    UINT32 TaskIdx;

    SvcVinErrTask_MonMsgInit();

    pMonFrwkCtrl->NumOfMonMsgPort = VIN_ERR_MON_PORT_NUM;
    pMonFrwkCtrl->pMonMsgPortCtrl = MonMsgPortList;

    for (PortIdx = 0U; PortIdx < pMonFrwkCtrl->NumOfMonMsgPort; PortIdx ++) {

        AmbaSvcWrap_MisraMemset(&(pMonFrwkCtrl->MonMsgPort[PortIdx]), 0, sizeof(AMBA_MON_MESSAGE_PORT_s));

        if ( pMonFrwkCtrl->pMonMsgPortCtrl[PortIdx].Enable > 0U ) {
            PRetVal = AmbaMonMessage_Open(&(pMonFrwkCtrl->MonMsgPort[PortIdx]),
                                          pMonFrwkCtrl->pMonMsgPortCtrl[PortIdx].MsgPortName,
                                          NULL);

            if (PRetVal != 0U) {
                RetVal = SVC_NG;

                PRN_VIN_ERR_LOG "Fail to create vin err control - open '%s' monitor frwk port fail"
                    PRN_VIN_ERR_ARG_CSTR   pMonFrwkCtrl->pMonMsgPortCtrl[PortIdx].MsgPortName PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_NG

                break;
            }
        }
    }

    /* create monitor task */
    pMonFrwkTask->NumOfTask = VIN_ERR_MON_PORT_NUM;
    for (TaskIdx = 0U; TaskIdx < pMonFrwkTask->NumOfTask; TaskIdx ++) {
        if (SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl[TaskIdx].Enable > 0U) {
            AmbaSvcWrap_MisraMemset(pMonFrwkTask->Name[TaskIdx], 0, 32);

            pMonFrwkTask->Ctrl[TaskIdx].CpuBits  = SVC_VIN_ERR_TASK_CPU_BITS;
            pMonFrwkTask->Ctrl[TaskIdx].Priority = SVC_VIN_ERR_MON_TASK_PRI;
            pMonFrwkTask->Ctrl[TaskIdx].EntryArg = SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl[TaskIdx].PortID;

            /* %s_%s */
            AmbaUtility_StringCopy  (pMonFrwkTask->Name[TaskIdx], 32U, SvcVinErrCtrl.Name);
            AmbaUtility_StringAppend(pMonFrwkTask->Name[TaskIdx], 32U, "_");
            AmbaUtility_StringAppend(pMonFrwkTask->Name[TaskIdx], 32U, SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl[TaskIdx].MsgPortName);

            pMonFrwkTask->Ctrl[TaskIdx].EntryFunc  = SvcVinErrTask_MonFrwkTaskEntry;
            pMonFrwkTask->Ctrl[TaskIdx].StackSize  = SVC_VIN_ERR_STACK_SZ;
            pMonFrwkTask->Ctrl[TaskIdx].pStackBase = &(pMonFrwkTask->Stack[TaskIdx][0]);

            PRetVal = SvcTask_Create(pMonFrwkTask->Name[TaskIdx], &(pMonFrwkTask->Ctrl[TaskIdx]));
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_VIN_ERR_LOG "Fail to create vin err control - create monitor task fail! ErrCode 0x%x"
                    PRN_VIN_ERR_ARG_UINT32 PRetVal PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_NG
            } else {
                PRN_VIN_ERR_LOG "Successful to create vin err monitor task - %s"
                    PRN_VIN_ERR_ARG_CSTR   pMonFrwkTask->Name[TaskIdx] PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_DBG1
            }
        }
    }

    return RetVal;
}

static void SvcVinErrTask_MonFrwkDeCfg(void)
{
    UINT32 Idx, PRetVal;

    /* delete monitor task */
    for (Idx = 0U; Idx < SvcVinErrCtrl.MonFrwkTask.NumOfTask; Idx ++) {
        PRetVal = SvcTask_Destroy(&(SvcVinErrCtrl.MonFrwkTask.Ctrl[Idx])); PRN_VIN_ERR_HDLR
    }

    /* close monitor framework port */
    for (Idx = 0U; Idx < SvcVinErrCtrl.MonFrwkCtrl.NumOfMonMsgPort; Idx ++) {
        PRetVal = AmbaMonMessage_Close(&(SvcVinErrCtrl.MonFrwkCtrl.MonMsgPort[Idx])); PRN_VIN_ERR_HDLR
    }

    AmbaMisra_TouchUnused(MonMsgPortList);
}

static void* SvcVinErrTask_MonFrwkTaskEntry(void* EntryArg)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    UINT32 MonitorPortID;
    INT32  TimeOut = 10000;
    const ULONG  *pArg;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);
    MonitorPortID = (UINT32)(*pArg);

    while ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_INIT) == 0U) {
        if (TimeOut > 0) {
            PRetVal = AmbaKAL_TaskSleep(10U); PRN_VIN_ERR_HDLR
            TimeOut -= 10;
        } else {
            RetVal = SVC_NG;
            break;
        }
    }

    if (RetVal != SVC_OK) {
        PRN_VIN_ERR_LOG "Fail to process task - create vin err control first!" PRN_VIN_ERR_NG
    } else if (MonitorPortID >= VIN_ERR_MON_PORT_NUM) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to process task - invalid monitor frwk port id(%d)"
            PRN_VIN_ERR_ARG_UINT32 MonitorPortID PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_NG
    } else if (SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl == NULL) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to process task - invalid monitor frwk port control" PRN_VIN_ERR_NG
    } else {
        AMBA_MON_MESSAGE_PORT_s *pMonMsgPort     = &(SvcVinErrCtrl.MonFrwkCtrl.MonMsgPort[MonitorPortID]);
        SVC_VIN_ERR_MON_PORT_s  *pMonMsgPortCtrl = &(SvcVinErrCtrl.MonFrwkCtrl.pMonMsgPortCtrl[MonitorPortID]);
        SVC_VIN_ERR_MON_MSG_s   *pMonitorMsg      = NULL;
        UINT32 MsgIdx, MsgType, EventID;
        UINT64 MsgFlag;
        UINT64 EventFlag = 0ULL;
        UINT64 ActualFlags;
        void *pMsg, *pStateMsg = NULL;
        AMBA_MON_MESSAGE_HEADER_s *pMsgHeader = NULL;
        UINT8 *pMsgData = NULL;
        ULONG LinkAddr;

        AmbaMisra_TouchUnused(&pMonMsgPort   );
        AmbaMisra_TouchUnused(pMonMsgPortCtrl);
        AmbaMisra_TouchUnused(pMonitorMsg    );

        for (MsgIdx = 0U; MsgIdx < pMonMsgPortCtrl->NumOfMsg; MsgIdx ++) {
            pMonitorMsg = &(pMonMsgPortCtrl->pMsgList[MsgIdx]);
            if (pMonitorMsg->Enable > 0U) {
                /* Configure msg */
                PRetVal = AmbaMonMessage_Find(pMonMsgPort, pMonitorMsg->MsgName, &EventID, &(pMonitorMsg->MsgFlag));
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    PRN_VIN_ERR_LOG "Fail to process task - find msg '%s' in monitor frwk port '%s' fail!"
                        PRN_VIN_ERR_ARG_CSTR   pMonitorMsg->MsgName         PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_CSTR   pMonMsgPortCtrl->MsgPortName PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_NG
                    break;
                } else {
                    EventFlag |= pMonitorMsg->MsgFlag;
                    PRN_VIN_ERR_LOG "Successful to find msg '%s' in monitor frwk port '%s'. Flag 0x%llx"
                        PRN_VIN_ERR_ARG_CSTR   pMonitorMsg->MsgName         PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_CSTR   pMonMsgPortCtrl->MsgPortName PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT64 pMonitorMsg->MsgFlag
                    PRN_VIN_ERR_DBG1

                }
            }
        }

        while (EventFlag > 0U) {

            ActualFlags = AmbaMonMessage_Wait(pMonMsgPort, EventFlag, (UINT32) MON_EVENT_FLAG_OR_CLR, (UINT32) AMBA_KAL_WAIT_FOREVER);
            if ((ActualFlags & EventFlag) > 0ULL) {
                do {
                    PRetVal = AmbaMonMessage_Get(pMonMsgPort, &pMsg);
                    if (PRetVal == 0U) {
                        /* msg get */
                        AmbaMisra_TypeCast(&(pMsgHeader), &(pMsg));

                        if (pMsgHeader != NULL) {

                            MsgFlag = pMsgHeader->Ctx.Event.Flag;
                            MsgType = 0U;

                            if ( pMsgHeader->Ctx.Com.Id == (UINT8) AMBA_MON_MSG_ID_LINK ) {
                                LinkAddr = (ULONG) (pMsgHeader->Ctx.Link.Pointer);
                                AmbaMisra_TypeCast(&(pStateMsg), &(LinkAddr));
                                AmbaMisra_TypeCast(&(pMsgHeader), &(LinkAddr));
                            } else {
                                AmbaMisra_TypeCast(&(pStateMsg), &(pMsgHeader));
                            }

                            if (pMsgHeader->Ctx.Chunk.Timeout != 0U) {
                                MsgType |= VIN_ERR_MON_MSG_TIMEOUT;
                            }

                            for (MsgIdx = 0U; MsgIdx < pMonMsgPortCtrl->NumOfMsg; MsgIdx ++) {
                                pMonitorMsg = &(pMonMsgPortCtrl->pMsgList[MsgIdx]);
                                if ((pMonitorMsg->Enable > 0U) && (pMonitorMsg->pMsgProc != NULL)) {
                                    if ((pMonitorMsg->MsgFlag & MsgFlag) > 0ULL) {

                                        AmbaMisra_TypeCast(&(pMsgData), &(pStateMsg));
                                        if (pMsgData != NULL) {
                                            (pMonitorMsg->pMsgProc)(&(pMsgData[sizeof(AMBA_MON_MESSAGE_HEADER_s)]), MsgFlag, MsgType);
                                        }
                                    }
                                }
                            }
                        } else {
                            PRN_VIN_ERR_LOG "Warning to process task - get invalid msg header" PRN_VIN_ERR_DBG1
                        }
                    } else if (PRetVal == MON_RING_OVERFLOW) {
                        PRetVal = AmbaMonMessage_RSeek(pMonMsgPort, 0U, (UINT32) R_SEEK_PASS_BEGIN);
                        if (PRetVal != 0U) {
                            PRN_VIN_ERR_LOG "Fail to process task - seek monitor frwk port '%s' fail! ErrCode(0x%08x)"
                                PRN_VIN_ERR_ARG_CSTR   pMonMsgPortCtrl->MsgPortName PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_NG
                        } else {
                            PRN_VIN_ERR_LOG "Monitor port '%s' overflow. RSeek it."
                                PRN_VIN_ERR_ARG_CSTR   pMonMsgPortCtrl->MsgPortName PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_DBG2
                        }
                        PRetVal = 0U;
                    } else {
                        // Do nothing
                    }
                } while (PRetVal == 0U);

                AmbaMisra_TouchUnused(&pMsg);
                AmbaMisra_TouchUnused(pStateMsg);
                AmbaMisra_TouchUnused(pMsgHeader);
            }

            AmbaMisra_TouchUnused(&EventFlag);
        }

        AmbaMisra_TouchUnused(pMsgData);
    }

    AmbaMisra_TouchUnused(&RetVal);

    return NULL;
}
#else

static UINT32 SvcVinErrTask_MsgQryCfg(void)
{
    UINT32 RetVal, Idx;
    UINT32 VinIDs[AMBA_DSP_MAX_VIN_NUM], NumVin = 0U;
    SVC_VIN_ERR_MSG_QRY_CTRL_s *pMsgQryCtrl;

    AmbaSvcWrap_MisraMemset(SvcVinErrCtrl.MsgQryCtrl, 0, sizeof(SVC_VIN_ERR_MSG_QRY_CTRL_s));

    AmbaSvcWrap_MisraMemset(VinIDs, 255, sizeof(VinIDs));
    RetVal = SvcResCfg_GetVinIDs(VinIDs, &NumVin);
    if (RetVal != 0U) {
        PRN_VIN_ERR_LOG "Fail to create vin err msg qry - get VinID fail" PRN_VIN_ERR_NG
    } else if (NumVin > AMBA_DSP_MAX_VIN_NUM) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to create vin err msg qry - invalid vin number(%d)"
            PRN_VIN_ERR_ARG_UINT32 NumVin PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_NG
    } else {
        UINT32 VinID;
        AMBA_SENSOR_STATUS_INFO_s SsStatus;
        AMBA_SENSOR_CHANNEL_s SsChan;

        for (Idx = 0U; Idx < NumVin; Idx ++) {
            VinID = VinIDs[Idx];
            if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
                PRN_VIN_ERR_LOG "Get invalid VinID(%d)"
                    PRN_VIN_ERR_ARG_UINT32 NumVin PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_NG
            } else {
                pMsgQryCtrl = &(SvcVinErrCtrl.MsgQryCtrl[VinID]);

                if (0U != SvcResCfg_GetSensorIDInVinID(VinID, &(pMsgQryCtrl->SerDesQrySensorID))) {
                    PRN_VIN_ERR_LOG "Get VinID(%d) SensorID fail"
                        PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_NG
                } else {
                    SsChan.VinID = VinID;
                    SsChan.SensorID = pMsgQryCtrl->SerDesQrySensorID;
                    AmbaSvcWrap_MisraMemset(&SsStatus, 0, sizeof(SsStatus));
                    RetVal = AmbaSensor_GetStatus(&SsChan, &SsStatus);
                    if (RetVal != 0U) {
                        PRN_VIN_ERR_LOG "Get VinID(%d) SensorID(%d) sensor status fail. ErrCode(0x%08X)"
                            PRN_VIN_ERR_ARG_UINT32 SsChan.VinID    PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 SsChan.SensorID PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 RetVal PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_NG
                    } else {
                        pMsgQryCtrl->SerDesQryTimeout = SsStatus.ModeInfo.FrameRate.NumUnitsInTick * 1000U;
                        if ((pMsgQryCtrl->SerDesQryTimeout % SsStatus.ModeInfo.FrameRate.TimeScale) > 0U) {
                            pMsgQryCtrl->SerDesQryTimeout /= SsStatus.ModeInfo.FrameRate.TimeScale;
                            pMsgQryCtrl->SerDesQryTimeout += 1U;
                        } else {
                            pMsgQryCtrl->SerDesQryTimeout /= SsStatus.ModeInfo.FrameRate.TimeScale;
                        }
                    }
                }

                // Create event flag
                AmbaUtility_StringCopy  (pMsgQryCtrl->EvtFlgName, 32U, SvcVinErrCtrl.Name);
                AmbaUtility_StringAppend(pMsgQryCtrl->EvtFlgName, 32U, "_MsgQryFlg_");
                AmbaUtility_StringAppendUInt32(pMsgQryCtrl->EvtFlgName, 32U, VinID, 10U);
                RetVal = AmbaKAL_EventFlagCreate(&(pMsgQryCtrl->EvtFlg), pMsgQryCtrl->EvtFlgName);
                if (RetVal != 0U) {
                    PRN_VIN_ERR_LOG "Fail to create vin err msg qry flag - create VinID(%d) event flag fail! ErrCode 0x%x"
                        PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 RetVal PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_NG
                } else {
                    RetVal = AmbaKAL_EventFlagClear(&(pMsgQryCtrl->EvtFlg), 0xFFFFFFFF);
                    if (RetVal != 0U) {
                        PRN_VIN_ERR_LOG "Fail to create vin err msg qry flag - clear VinID(%d) flag fail! ErrCode 0x%x"
                            PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 RetVal PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_NG
                    }
                }

                if (RetVal == 0U) {
                    AmbaUtility_StringCopy  (pMsgQryCtrl->TaskName, 32U, SvcVinErrCtrl.Name);
                    AmbaUtility_StringAppend(pMsgQryCtrl->TaskName, 32U, "_MsgQryTask_");
                    AmbaUtility_StringAppendUInt32(pMsgQryCtrl->TaskName, 32U, VinID, 10U);

                    pMsgQryCtrl->TaskCtrl.CpuBits    = SVC_VIN_ERR_TASK_CPU_BITS;
                    pMsgQryCtrl->TaskCtrl.Priority   = SVC_VIN_ERR_MON_TASK_PRI;
                    pMsgQryCtrl->TaskCtrl.EntryArg   = VinID;
                    pMsgQryCtrl->TaskCtrl.EntryFunc  = SvcVinErrTask_MsgQryTaskEntry;
                    pMsgQryCtrl->TaskCtrl.StackSize  = SVC_VIN_ERR_STACK_SZ;
                    pMsgQryCtrl->TaskCtrl.pStackBase = &(pMsgQryCtrl->TaskStack[0]);
                    RetVal = SvcTask_Create(pMsgQryCtrl->TaskName, &(pMsgQryCtrl->TaskCtrl));
                    if (RetVal != 0U) {
                        PRN_VIN_ERR_LOG "Fail to create vin err msg qry task - create VinID(%d) task fail! ErrCode 0x%x"
                            PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 RetVal PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_NG
                    }
                }
            }

            if (RetVal != 0U) {
                break;
            }
        }
    }

    if (RetVal == 0U) {
        RetVal = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_VIDEO_PATH_STATUS, SvcVinErrTask_VinToutHdlr);
        if (RetVal != 0U) {
            PRN_VIN_ERR_LOG "Fail to create vin err msg qry task - register video path event! ErrCode 0x%x"
                PRN_VIN_ERR_ARG_UINT32 RetVal PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        } else {
            RetVal = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_RAW_RDY, SvcVinErrTask_RawRdyHdlr);
            if (RetVal != 0U) {
                PRN_VIN_ERR_LOG "Fail to create vin err msg qry task - register raw rdy event! ErrCode 0x%x"
                    PRN_VIN_ERR_ARG_UINT32 RetVal PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_NG
            }
        }
    }

    if (RetVal == 0U) {
        if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_LOG_DBG1) > 0U) {
            SvcVinErrTask_MsgQryInfo();
        }
        PRN_VIN_ERR_LOG "Success to create vin err msg qry control" PRN_VIN_ERR_DBG1
    }

    return RetVal;
}

static void SvcVinErrTask_MsgQryDeCfg(void)
{
    UINT32 PRetVal, ActFlag = 0U;
    UINT32 VinID;
    SVC_VIN_ERR_MSG_QRY_CTRL_s *pMsgQryCtrl;

    PRetVal = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_VIDEO_PATH_STATUS, SvcVinErrTask_VinToutHdlr); PRN_VIN_ERR_HDLR
    PRetVal = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_LV_RAW_RDY, SvcVinErrTask_RawRdyHdlr); PRN_VIN_ERR_HDLR

    for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
        if ((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(VinID)) > 0U) {
            pMsgQryCtrl = &(SvcVinErrCtrl.MsgQryCtrl[VinID]);

            PRetVal = AmbaKAL_EventFlagSet(&(pMsgQryCtrl->EvtFlg), SVC_VIN_ERR_MSG_QRY_FLG_TSK_EXIT_REQ); PRN_VIN_ERR_HDLR
            PRetVal = AmbaKAL_EventFlagGet(&(pMsgQryCtrl->EvtFlg)
                                          , SVC_VIN_ERR_MSG_QRY_FLG_TSK_EXIT_DONE
                                          , AMBA_KAL_FLAGS_ANY
                                          , AMBA_KAL_FLAGS_CLEAR_AUTO
                                          , &ActFlag
                                          , 5000U); PRN_VIN_ERR_HDLR
            PRetVal = SvcTask_Destroy(&(pMsgQryCtrl->TaskCtrl)); PRN_VIN_ERR_HDLR
            PRetVal = AmbaKAL_EventFlagClear(&(pMsgQryCtrl->EvtFlg), 0xFFFFFFFFU); PRN_VIN_ERR_HDLR
            PRetVal = AmbaKAL_EventFlagDelete(&(pMsgQryCtrl->EvtFlg)); PRN_VIN_ERR_HDLR
        }
    }
}

static void SvcVinErrTask_MsgQryInfo(void)
{
    UINT32 PRetVal, VinID;
    SVC_VIN_ERR_MSG_QRY_CTRL_s *pCtrl;
    AMBA_KAL_EVENT_FLAG_INFO_s CurFlagInfo;

    PRN_VIN_ERR_LOG "" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "==== Msg Query Info ====" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "           TimeOut | TimeCnt | Proc | SensorID | PreLinkLock | PreVidLock | Rst | EvtFlag" PRN_VIN_ERR_API
    for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
        if ((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(VinID)) > 0U) {
            pCtrl = &(SvcVinErrCtrl.MsgQryCtrl[VinID]);

            AmbaSvcWrap_MisraMemset(&CurFlagInfo, 0, sizeof(CurFlagInfo));
            PRetVal = AmbaKAL_EventFlagQuery(&(pCtrl->EvtFlg), &CurFlagInfo); PRN_VIN_ERR_HDLR

            PRN_VIN_ERR_LOG  "   Vin%2d : %7d | %7d | %4d |  0x%05X |  0x%08X | 0x%08X | %3d | 0x%08X"
            PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 pCtrl->SerDesQryTimeout     PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 pCtrl->SerDesQryTimeCnt     PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 pCtrl->SerDesQryProc        PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 pCtrl->SerDesQrySensorID    PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 pCtrl->SerDesQryPreLinkLock PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 pCtrl->SerDesQryPreVidLock  PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 pCtrl->SerDesQryReset       PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 CurFlagInfo.CurrentFlags    PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API
        }
    }
}

static void SvcVinErrTask_MsgQryTimerHdlr(UINT32 EntryArg)
{
    UINT32 PRetVal;
    UINT32 VinID;
    SVC_VIN_ERR_MSG_QRY_CTRL_s *pCtrl;

    for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
        if ((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(VinID)) > 0U) {
            pCtrl = &(SvcVinErrCtrl.MsgQryCtrl[VinID]);

            if (pCtrl->SerDesQryTimeout > 0U) {
                if (pCtrl->SerDesQryReset > 0U) {
                    pCtrl->SerDesQryReset = 0U;
                    pCtrl->SerDesQryTimeCnt = pCtrl->SerDesQryTimeout;
                } else {
                    if (pCtrl->SerDesQryTimeCnt > 0U) {
                        pCtrl->SerDesQryTimeCnt --;
                    }
                    if (pCtrl->SerDesQryTimeCnt == 0U) {
                        pCtrl->SerDesQryTimeCnt = pCtrl->SerDesQryTimeout;
                        pCtrl->SerDesQryProc = 1U;
                        PRetVal = AmbaSYS_GetOrcTimer(&(pCtrl->SerDesQryTimeStampInTimer)); PRN_VIN_ERR_HDLR
                        PRetVal = AmbaKAL_EventFlagSet(&(pCtrl->EvtFlg), SVC_VIN_ERR_MSG_QRY_FLG_LINK_QRY); PRN_VIN_ERR_HDLR
                    }
                }
            }
        }
    }

    AmbaMisra_TouchUnused(&EntryArg);
}

static void* SvcVinErrTask_MsgQryTaskEntry(void* pEntryArg)
{
    const UINT32 *pArg;

    AmbaMisra_TouchUnused(pEntryArg);
    AmbaMisra_TypeCast(&pArg, &pEntryArg);

    if (pArg == NULL) {
        PRN_VIN_ERR_LOG "Fail to proc msg qry task - invalid task arg!" PRN_VIN_ERR_NG
    } else {
        UINT32 VinID = *pArg;
        if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
            PRN_VIN_ERR_LOG "Fail to proc msg qry task - invalid VinID(%d)"
            PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        } else {
            UINT32 PRetVal;
            UINT32 ActFlag;
            UINT32 SIdx;
            SVC_VIN_ERR_MSG_QRY_CTRL_s *pCtrl = &(SvcVinErrCtrl.MsgQryCtrl[VinID]);
            AMBA_SENSOR_CHANNEL_s       SsChan;
            AMBA_SENSOR_SERDES_STATUS_s SerDesStatus[SVC_VIN_ERR_MAX_SER_NUM];
            UINT32 CurLinkLock, CurVidLock;
            UINT32 ProcTimeS, ProcTimeE;
            UINT32 AudioFreq = 12288U;
            UINT32 ProcDiff, ProcDelay;

            if (0U == AmbaSYS_GetClkFreq(AMBA_SYS_CLK_AUD_0, &AudioFreq)) {
                AudioFreq = AudioFreq  / 1000U;
            }

            AmbaSvcWrap_MisraMemset(&SsChan, 0, sizeof(SsChan));
            SsChan.VinID = VinID;
            SsChan.SensorID = pCtrl->SerDesQrySensorID;

            PRetVal = AmbaKAL_EventFlagGet(&(SvcVinErrCtrl.VinLivFlag)
                                          , 0xFFFFFFFFU
                                          , AMBA_KAL_FLAGS_ANY
                                          , AMBA_KAL_FLAGS_CLEAR_NONE
                                          , &ActFlag
                                          , 10000U); PRN_VIN_ERR_HDLR

            PRN_VIN_ERR_LOG "Start process msg query task. 0x%08X"
                PRN_VIN_ERR_ARG_UINT32 ActFlag PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_DBG1

            while ( AudioFreq > 1U ) {
                ActFlag = 0U;
                PRetVal = AmbaKAL_EventFlagGet(&(pCtrl->EvtFlg)
                                              , 0xFFFFFFFFU
                                              , AMBA_KAL_FLAGS_ANY
                                              , AMBA_KAL_FLAGS_CLEAR_AUTO
                                              , &ActFlag
                                              , AMBA_KAL_WAIT_FOREVER); PRN_VIN_ERR_HDLR
                if ((ActFlag & SVC_VIN_ERR_MSG_QRY_FLG_LINK_QRY) > 0U) {
                    if (pCtrl->SerDesQryProc > 0U) {

                        ProcTimeS = 0U;
                        ProcTimeE = 0U;
                        PRetVal = AmbaSYS_GetOrcTimer(&ProcTimeS); PRN_VIN_ERR_HDLR

                        AmbaSvcWrap_MisraMemset(SerDesStatus, 0, sizeof(SerDesStatus));
                        PRetVal = AmbaSensor_GetSerdesStatus(&SsChan, SerDesStatus);
                        PRetVal = AmbaSYS_GetOrcTimer(&ProcTimeE); PRN_VIN_ERR_HDLR
                        {
                            CurLinkLock = 0U;
                            CurVidLock  = 0U;

                            for (SIdx = 0U; SIdx < SVC_VIN_ERR_MAX_SER_NUM; SIdx++) {
                                if ((pCtrl->SerDesQrySensorID & (((UINT32) 0x1U) << ((SIdx + 1U) << 2U))) > 0U) {
                                    CurLinkLock |= SerDesStatus[SIdx].LinkLock << (SIdx << 3U);
                                    CurVidLock  |= SerDesStatus[SIdx].VideoLock << (SIdx << 3U);
                                }
                            }

                            PRN_VIN_ERR_LOG "Query VinID(%s%d%s) LinkLock %d, %d, %d, %d -> cur: 0x%08X, pre:0x%08X"
                                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_UINT32 SerDesStatus[0].LinkLock   PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_UINT32 SerDesStatus[1].LinkLock   PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_UINT32 SerDesStatus[2].LinkLock   PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_UINT32 SerDesStatus[3].LinkLock   PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_UINT32 CurLinkLock                PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_UINT32 pCtrl->SerDesQryPreLinkLock PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_DBG5

                            if ((CurLinkLock != pCtrl->SerDesQryPreLinkLock)
                              ||(CurVidLock != pCtrl->SerDesQryPreVidLock)) {

                                pCtrl->SerDesQryPreLinkLock = CurLinkLock;
                                pCtrl->SerDesQryPreVidLock = CurVidLock;

                                PRN_VIN_ERR_LOG "Monitor VinID(%s%d%s) link state %s0x%x%s %s0x%x%s!"
                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0 PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT32 CurLinkLock                PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0 PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT32 CurVidLock                 PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_DBG1_5

                                SvcVinErrTask_LinkStateChange(VinID, CurLinkLock, CurVidLock );
                            }
                        }

                        if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_LOG_DBG5) > 0U) {
                            if (PRetVal != 0U) {
                                PRN_VIN_ERR_LOG "Fail to query VinID(%d)/SensorID(0x%X) SerDes status. ErrCode(0x%08X)"
                                PRN_VIN_ERR_ARG_UINT32 SsChan.VinID PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_UINT32 SsChan.SensorID PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT32 PRetVal PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_DBG1_5
                            }
                        }

                        if (ProcTimeS > ProcTimeE) {
                            ProcDiff = ((ProcTimeS + (0xFFFFFFFFU - ProcTimeE)) / AudioFreq);
                        } else {
                            ProcDiff = ((ProcTimeE - ProcTimeS) / AudioFreq);
                        }

                        if (pCtrl->SerDesQryTimeStampInTimer > ProcTimeS) {
                            ProcDelay = ((pCtrl->SerDesQryTimeStampInTimer + (0xFFFFFFFFU - ProcTimeS)) / AudioFreq);
                        } else {
                            ProcDelay = ((ProcTimeS - pCtrl->SerDesQryTimeStampInTimer) / AudioFreq);
                        }

                        PRN_VIN_ERR_LOG "Query VinID(%s%d%s) SerDes spending %s%d%s, Delay(%d), %10d, %10d, %10d"
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0 PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 ProcDiff                   PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 ProcDelay                        PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 pCtrl->SerDesQryTimeStampInTimer PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 ProcTimeS                        PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 ProcTimeE                        PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_DBG5
                    }
                    pCtrl->SerDesQryProc = 0U;
                }

                if ((ActFlag & SVC_VIN_ERR_MSG_QRY_FLG_TSK_EXIT_REQ) > 0U) {
                    break;
                }
            }

            PRetVal = AmbaKAL_EventFlagSet(&(pCtrl->EvtFlg), SVC_VIN_ERR_MSG_QRY_FLG_TSK_EXIT_DONE);
        }
    }

    return NULL;
}

static UINT32 SvcVinErrTask_RawRdyHdlr(const void *pEventData)
{
    UINT32 PRetVal;
    const AMBA_DSP_RAW_DATA_RDY_s *pData;

    AmbaMisra_TypeCast(&pData, &pEventData);

    if (pData != NULL) {
        UINT32 VinID = (UINT32)(pData->VinId);
        VinID &= 0xFFU;

        if (VinID < AMBA_DSP_MAX_VIN_NUM) {
            SVC_VIN_ERR_STATE_s *pState = &(SvcVinErrCtrl.ErrState[VinID]);
            AMBA_KAL_EVENT_FLAG_INFO_s CurFlag;

            AmbaSvcWrap_MisraMemset(&CurFlag, 0, sizeof(CurFlag));
            if (0U == AmbaKAL_EventFlagQuery(&(pState->VinStateFlag), &CurFlag)) {
                if ((pData->IsVirtChan & 0x4U) > 0U) {
                    if ((CurFlag.CurrentFlags & SVC_VIN_ERR_VIN_STATE_RAW_DEF) == 0U) {
                        PRN_VIN_ERR_LOG "VinID(%s%d%s) def raw start!"
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_DBG1

                        PRetVal = AmbaKAL_EventFlagSet(&(pState->VinStateFlag), SVC_VIN_ERR_VIN_STATE_RAW_DEF); PRN_VIN_ERR_HDLR
                    }
                } else {
                    if ((CurFlag.CurrentFlags & SVC_VIN_ERR_VIN_STATE_RAW_TOUT) > 0U) {
                        UINT32 SetFlag = SVC_VIN_ERR_VIN_STATE_LIV;
                        UINT32 ClrFlag = SVC_VIN_ERR_VIN_STATE_TOUT | SVC_VIN_ERR_VIN_STATE_RAW_TOUT | SVC_VIN_ERR_VIN_STATE_RAW_DEF;

                        PRN_VIN_ERR_LOG "Start VinID(%s%d%s) raw!"
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_API

                        PRetVal = AmbaKAL_EventFlagClear(&(pState->VinStateFlag), ClrFlag); PRN_VIN_ERR_HDLR
                        PRetVal = AmbaKAL_EventFlagSet(&(pState->VinStateFlag), SetFlag); PRN_VIN_ERR_HDLR
                    }

                    PRetVal = AmbaKAL_EventFlagSet(&(pState->VinStateFlag), SVC_VIN_ERR_VIN_STATE_RAW_RDY); PRN_VIN_ERR_HDLR
                    PRetVal = AmbaKAL_EventFlagSet(&(SvcVinErrCtrl.VinLivFlag), SvcVinErrTask_BitGet(VinID)); PRN_VIN_ERR_HDLR

                    if ((pState->FovFreezeSrc == SVC_VIN_ERR_FOV_FREZE_SRC_RAW) &&
                        ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_RAW_FRZ_OFF) == 0U)) {
                        SVC_VIN_ERR_RAW_FRZ_UPD_s RawDataUpd;

                        AmbaSvcWrap_MisraMemset(&RawDataUpd, 0, sizeof(RawDataUpd));
                        RawDataUpd.VinID   = VinID;
                        AmbaMisra_TypeCast(&(RawDataUpd.pRawBuf), &(pData->RawBuffer.BaseAddr));
                        RawDataUpd.Pitch   = (UINT32)(pData->RawBuffer.Pitch);         RawDataUpd.Pitch  &= 0xFFFFU;
                        RawDataUpd.Width   = (UINT32)(pData->RawBuffer.Window.Width);  RawDataUpd.Width  &= 0xFFFFU;
                        RawDataUpd.Height  = (UINT32)(pData->RawBuffer.Window.Height); RawDataUpd.Height &= 0xFFFFU;
                        PRetVal = SvcVinErrTask_SendMsg(SVC_VIN_ERR_CTRL_CMD_RAW_UPD, &RawDataUpd, (UINT32)sizeof(RawDataUpd), AMBA_KAL_NO_WAIT);
                        if (PRetVal != 0U) {
                            PRN_VIN_ERR_LOG "Fail to proc raw ready msg - push raw data msg to queue fail! ErrCode(0x%08x)"
                                PRN_VIN_ERR_ARG_UINT32 PRetVal PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_NG
                        }
                    }
                }
            }
        }
    }

    return 0U;
}

static UINT32 SvcVinErrTask_VinToutHdlr(const void *pEventData)
{
    UINT32 PRetVal;
    const AMBA_DSP_VIDEO_PATH_INFO_s *pInfo;

    AmbaMisra_TypeCast(&pInfo, &pEventData);

    if ((pInfo != NULL) &&
        (pInfo->Status == VID_PATH_STATUS_VIN_TIMEOUT) &&
        (pInfo->Data < AMBA_DSP_MAX_VIN_NUM)) {
        UINT32 VinID = pInfo->Data;

        SVC_VIN_ERR_STATE_s *pState = &(SvcVinErrCtrl.ErrState[VinID]);
        AMBA_KAL_EVENT_FLAG_INFO_s CurFlag;

        AmbaSvcWrap_MisraMemset(&CurFlag, 0, sizeof(CurFlag));
        if (0U == AmbaKAL_EventFlagQuery(&(pState->VinStateFlag), &CurFlag)) {
            if ((CurFlag.CurrentFlags & SVC_VIN_ERR_VIN_STATE_TOUT) == 0U) {
                UINT32 SetFlag = SVC_VIN_ERR_VIN_STATE_TOUT | SVC_VIN_ERR_VIN_STATE_RAW_TOUT;
                UINT32 ClrFlag = SVC_VIN_ERR_VIN_STATE_LIV  | SVC_VIN_ERR_VIN_STATE_RAW_RDY;

                PRN_VIN_ERR_LOG "VinID(%s%d%s) timeout!"
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_DBG1

                // Update vin state
                PRetVal = AmbaKAL_EventFlagClear(&(SvcVinErrCtrl.ErrState[VinID].VinStateFlag), ClrFlag); PRN_VIN_ERR_HDLR
                PRetVal = AmbaKAL_EventFlagSet(&(SvcVinErrCtrl.ErrState[VinID].VinStateFlag), SetFlag); PRN_VIN_ERR_HDLR

                {
                    SVC_VIN_ERR_LINK_UPD_s LinkUpd;

                    AmbaSvcWrap_MisraMemset(&LinkUpd, 0, sizeof(LinkUpd));
                    LinkUpd.VinID = VinID;
                    PRetVal = SvcVinErrTask_SendMsg(SVC_VIN_ERR_CTRL_CMD_VIN_TOUT, &LinkUpd, (UINT32)sizeof(LinkUpd), AMBA_KAL_NO_WAIT);
                    if (PRetVal != 0U) {
                        PRN_VIN_ERR_LOG "Fail to proc vin timeout msg  - prepare VinID(%d) timeout cmd fail! ErrCode(0x%08x)"
                            PRN_VIN_ERR_ARG_UINT32 VinID   PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 PRetVal PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_NG
                    } else {
                        PRN_VIN_ERR_LOG "Send VinID(%s%d%s) timeout update!"
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_DBG1
                    }
                }
            }
        }
    }

    return 0U;
}

#endif

static UINT32 SvcVinErrTask_VoutLatencyComp(const SVC_VIN_ERR_VOUT_LTN_UPD_s *pLatency)
{
    UINT32 RetVal = SVC_OK;

    if (pLatency == NULL) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to compare vout latency - invalid latency info" PRN_VIN_ERR_API
    } else if (pLatency->VoutID >= AMBA_DSP_MAX_VOUT_NUM) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to compare vout latency - invalid VoutID(%d)"
            PRN_VIN_ERR_ARG_UINT32 pLatency->VoutID PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_API
    } else {
        UINT32 Idx, VinID;
        UINT32 UpdOsdVinBits = 0U;
        UINT32 VoutLatency = 0U;
        SVC_VIN_ERR_FOV_INFO_s *pFovInfo;

        PRN_VIN_ERR_LOG "VoutID(%s%d%s) latency"
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0 PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 pLatency->VoutID           PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_DBG3

        for (Idx = 0U; Idx < AMBA_DSP_MAX_VIEWZONE_NUM; Idx ++) {
            if ((pLatency->FovSelectBits & SvcVinErrTask_BitGet(Idx)) > 0U) {

                pFovInfo = &(SvcVinErrCtrl.FovInfo[Idx]);
                VinID = pFovInfo->SrcVinID;

                if ((pFovInfo->State & SVC_VIN_ERR_FOV_INIT) == 0U) {
                    PRN_VIN_ERR_LOG "|-> FovID(%s%d%s) does not initialize. Only support fov source from sensor."
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0 PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 Idx                        PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_DBG3
                } else if ((pFovInfo->State & SVC_VIN_ERR_FOV_VISIBLE) == 0U) {
                    PRN_VIN_ERR_LOG "|-> FovID(%s%d%s) video signal lost."
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0 PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 Idx                        PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_DBG3
                } else if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
                    PRN_VIN_ERR_LOG "|-> FovID(%s%d%s) source VinID(%d) is out-of range"
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0 PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 Idx                        PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0 PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_DBG3
                } else {
                    if (pFovInfo->VoutLatencySeqCnt != pLatency->DispSeqCnt) {

                        VoutLatency = (UINT32)(pLatency->FovVoutLatency[Idx]); VoutLatency &= 0xFFFFU;
                        PRN_VIN_ERR_LOG "|-> Check FovID(%s%d%s) latency(%s%d%s) with threshold(%s%d%s)"
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0 PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 Idx                        PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0 PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 VoutLatency                PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0 PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 pFovInfo->VoutLatencyThreshold PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_DBG3

                        if (pFovInfo->VoutLatencyThreshold <= VoutLatency) {

                            if ((pFovInfo->State & SVC_VIN_ERR_FOV_VOUT_DELAY) == 0U) {
                                PRN_VIN_ERR_LOG "VoutID(%s%d%s) latency, FovID(%d) SeqCnt(0x%llx) latency(%u) is out of threshold(%d)"
                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0     PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT32 pLatency->VoutID               PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END         PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT32 Idx                            PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT64 pLatency->DispSeqCnt
                                    PRN_VIN_ERR_ARG_UINT32 VoutLatency                    PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT32 pFovInfo->VoutLatencyThreshold PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_API

                            }

                            pFovInfo->State |= SVC_VIN_ERR_FOV_VOUT_DELAY;
                        } else {

                            if ((pFovInfo->State & SVC_VIN_ERR_FOV_VOUT_DELAY) > 0U) {
                                PRN_VIN_ERR_LOG "VoutID(%s%d%s) latency, FovID(%d) SeqCnt(0x%llx) latency(%u) %sless than%s threshold(%u)"
                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0     PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT32 pLatency->VoutID               PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END         PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT32 Idx                            PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT64 pLatency->DispSeqCnt
                                    PRN_VIN_ERR_ARG_UINT32 VoutLatency                    PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_7     PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END         PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT32 pFovInfo->VoutLatencyThreshold PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_API
                            }

                            pFovInfo->State &= ~SVC_VIN_ERR_FOV_VOUT_DELAY;
                        }

                        pFovInfo->VoutLatencySeqCnt = pLatency->DispSeqCnt;

                        UpdOsdVinBits |= SvcVinErrTask_BitGet(pFovInfo->SrcVinID);
                    }
                }
            }
        }

        for (Idx = 0U; Idx < AMBA_DSP_MAX_VIN_NUM; Idx ++) {
            if ((UpdOsdVinBits & SvcVinErrTask_BitGet(Idx)) > 0U) {
                SvcVinErrTask_OsdUpdate(Idx);
            }
        }
    }

    return RetVal;
}

static UINT32 SvcVinErrTask_FovLatencyComp(const SVC_VIN_ERR_FOV_LTN_UPD_s *pLatency)
{
    UINT32 RetVal = SVC_OK;

    if (pLatency == NULL) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to compare fov latency - invalid latency info" PRN_VIN_ERR_API
    } else if (pLatency->FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to compare fov latency - invalid FovID(%d)"
            PRN_VIN_ERR_ARG_UINT32 pLatency->FovID PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_API
    } else {
        SVC_VIN_ERR_FOV_INFO_s *pFovInfo = &(SvcVinErrCtrl.FovInfo[pLatency->FovID]);

        if ((pFovInfo->State & SVC_VIN_ERR_FOV_INIT) == 0U) {
            PRN_VIN_ERR_LOG "FovID(%s%d%s) does not initialize. Only support fov source from sensor."
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0 PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 pLatency->FovID            PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_DBG4
        } else if ((pFovInfo->State & SVC_VIN_ERR_FOV_VISIBLE) == 0U) {
            PRN_VIN_ERR_LOG "FovID(%s%d%s) video signal lost."
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0 PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 pLatency->FovID            PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_DBG4
        } else {
            if (pFovInfo->FovLatencySeqCnt != pLatency->YuvSeqNum) {

                if (pFovInfo->FovLatencyThreshold <= pLatency->Latency) {

                    if ((pFovInfo->State & SVC_VIN_ERR_FOV_DELAY) == 0U) {
                        PRN_VIN_ERR_LOG "FovID(%s%d%s) latency, SeqCnt(0x%llx) latency(%u) is out of threshold(%d)"
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0     PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 pLatency->FovID                PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END         PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT64 pLatency->YuvSeqNum
                            PRN_VIN_ERR_ARG_UINT32 pLatency->Latency              PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 pFovInfo->FovLatencyThreshold  PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_API

                    }

                    pFovInfo->State |= SVC_VIN_ERR_FOV_DELAY;
                } else {

                    if ((pFovInfo->State & SVC_VIN_ERR_FOV_DELAY) > 0U) {
                        PRN_VIN_ERR_LOG "FovID(%s%d%s) latency, SeqCnt(0x%llx) latency(%u) %sless than%s threshold(%u)"
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0     PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 pLatency->FovID                PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END         PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT64 pLatency->YuvSeqNum
                            PRN_VIN_ERR_ARG_UINT32 pLatency->Latency              PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_7     PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END         PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 pFovInfo->FovLatencyThreshold  PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_API
                    }

                    pFovInfo->State &= ~SVC_VIN_ERR_FOV_DELAY;
                }

                pFovInfo->FovLatencySeqCnt = pLatency->YuvSeqNum;

                SvcVinErrTask_OsdUpdate(pFovInfo->SrcVinID);
            }
        }

    }

    return RetVal;
}

static UINT32 SvcVinErrTask_RawFrzCreate(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_RAW_FRZ_INIT) > 0U) {
        RetVal = SVC_OK;
        PRN_VIN_ERR_LOG "Vin err raw freeze moniter has been created!" PRN_VIN_ERR_API
    } else {
        const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

        if  (pResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to create vin err raw freeze - invalid resolution cfg!" PRN_VIN_ERR_NG
        } else {
            UINT32 VinIdx, SenIdx;
            UINT32 VinIDs[AMBA_DSP_MAX_VIN_NUM], VinNum = 0U;
            SVC_VIN_ERR_RAW_FRZ_INFO_s *pFrzeInfo = NULL;

            AmbaSvcWrap_MisraMemset(&SvcVinErrRawFrzCtrl, 0, sizeof(SvcVinErrRawFrzCtrl));
            AmbaSvcWrap_MisraMemset(VinIDs, 0, sizeof(VinIDs));

            PRetVal = SvcResCfg_GetVinIDs(VinIDs, &VinNum);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_VIN_ERR_LOG "Fail to create vin err raw frz - get vin info fail!" PRN_VIN_ERR_NG
            } else {
                UINT32 SenIdxs[SVC_MAX_NUM_SENSOR_PER_VIN], SenIdxNum = 0U;
                UINT32 VinSrc = 0xFFFFFFFFU;
                UINT32 Row, Col, RowShift = 0U, ColShift = 0U;
                AMBA_SENSOR_CHANNEL_s SsChan;
                AMBA_SENSOR_CONFIG_s SsModeCfg;
                AMBA_SENSOR_MODE_INFO_s SsModeInfo;
                AMBA_YUV_CHANNEL_s YuvChan;
                AMBA_YUV_STATUS_INFO_s YuvStatus;

                for (VinIdx = 0U; VinIdx < VinNum; VinIdx ++) {

                    if (VinIDs[VinIdx] >= AMBA_DSP_MAX_VIN_NUM) {
                        PRetVal = SVC_NG;
                    } else {
                        PRetVal = SvcResCfg_GetVinSrc(VinIDs[VinIdx], &VinSrc);
                        if (PRetVal == 0U) {
                            if ((VinSrc != SVC_VIN_SRC_SENSOR)
                             && (VinSrc != SVC_VIN_SRC_YUV)) {
                                PRetVal = SVC_NG;
                            }
                        }
                    }

                    // Get Sensor index/number
                    SenIdxNum = 0U;
                    AmbaSvcWrap_MisraMemset(SenIdxs, 0, sizeof(SenIdxs));
                    if (PRetVal == 0U) {
                        PRetVal = SvcResCfg_GetSensorIdxsInVinID(VinIDs[VinIdx], SenIdxs, &SenIdxNum);
                    }

                    // Get Sensor Status
                    if (PRetVal == 0U) {
                        if (VinSrc == SVC_VIN_SRC_SENSOR) {
                            AmbaSvcWrap_MisraMemset(&SsChan, 0, sizeof(SsChan));
                            SsChan.VinID = VinIDs[VinIdx];
                            AmbaSvcWrap_MisraMemset(&SsModeCfg, 0, sizeof(SsModeCfg));
                            SsModeCfg.ModeID = pResCfg->SensorCfg[VinIDs[VinIdx]][SenIdxs[0]].SensorMode;
                            AmbaSvcWrap_MisraMemset(&SsModeInfo, 0, sizeof(SsModeInfo));
                            PRetVal = AmbaSensor_GetModeInfo(&SsChan, &SsModeCfg, &SsModeInfo);
                            RowShift = (UINT32)(SsModeInfo.OutputInfo.RecordingPixels.Height); RowShift &= 0xFFFFU;
                            RowShift /= SVC_VIN_ERR_RAW_FRZ_TILE_ROW;
                            ColShift = (UINT32)(SsModeInfo.OutputInfo.RecordingPixels.Width); ColShift &= 0xFFFFU;
                            ColShift /= SVC_VIN_ERR_RAW_FRZ_TILE_COL;
                        } else {    //SVC_VIN_SRC_YUV
                            AmbaSvcWrap_MisraMemset(&YuvStatus, 0, sizeof(YuvStatus));
                            AmbaSvcWrap_MisraMemset(&YuvChan, 0, sizeof(YuvChan));
                            YuvChan.VinID = VinIDs[VinIdx];
                            PRetVal = AmbaYuv_GetStatus(&YuvChan, &YuvStatus);
                            RowShift = (UINT32)(YuvStatus.ModeInfo.OutputInfo.RecordingPixels.Height); RowShift &= 0xFFFFU;
                            RowShift /= SVC_VIN_ERR_RAW_FRZ_TILE_ROW;
                            ColShift = (UINT32)(YuvStatus.ModeInfo.OutputInfo.RecordingPixels.Width); ColShift &= 0xFFFFU;
                            ColShift /= SVC_VIN_ERR_RAW_FRZ_TILE_COL;
                        }
                    }

                    if (PRetVal == 0U) {
                        for (SenIdx = 0U; SenIdx < SenIdxNum; SenIdx ++) {
                            if (SenIdxs[SenIdx] >= SVC_VIN_ERR_MAX_SER_NUM) {
                                PRN_VIN_ERR_LOG "Fail to create vin err raw frz - VinID(%d) sensor index(%d) out-of range!"
                                    PRN_VIN_ERR_ARG_UINT32 VinIDs[VinIdx]  PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT32 SenIdxs[SenIdx] PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_NG
                            } else {
                                SvcVinErrRawFrzCtrl.SelectBits |= SvcVinErrTask_BitGet(VinIDs[VinIdx]);
                                pFrzeInfo = &(SvcVinErrRawFrzCtrl.FrzeInfo[VinIDs[VinIdx]][SenIdxs[SenIdx]]);
                                AmbaSvcWrap_MisraMemset(pFrzeInfo, 0, sizeof(SVC_VIN_ERR_RAW_FRZ_INFO_s));
                                pFrzeInfo->State = SVC_VIN_ERR_RAW_FRZ_STATE_ALIVE;

                                for (Col = 0U; Col < SVC_VIN_ERR_RAW_FRZ_TILE_COL; Col ++) {
                                    pFrzeInfo->OffsetCol[Col] = Col * ColShift;
                                }
                                for (Row = 0U; Row < SVC_VIN_ERR_RAW_FRZ_TILE_ROW; Row ++) {
                                    pFrzeInfo->OffsetRow[Row] = (Row * RowShift) * SenIdxNum;
                                    pFrzeInfo->OffsetRow[Row] += SenIdx;
                                }

                                pFrzeInfo->pPreBuf = NULL;

                                SvcVinErrRawFrzCtrl.RawDataInfo[VinIDs[VinIdx]].AccumLine += 1U;
                            }
                        }

                        for (Row = 0U; Row < SVC_VIN_ERR_RAW_FRZ_TILE_ROW; Row ++) {
                            SvcVinErrRawFrzCtrl.RawDataInfo[VinIDs[VinIdx]].DataOfsRow[Row] = SvcVinErrRawFrzCtrl.FrzeInfo[VinIDs[VinIdx]][SenIdxs[0U]].OffsetRow[Row];
                        }
                    }
                }
            }

            if (RetVal == SVC_OK) {

                /* %s_%s */
                  AmbaUtility_StringCopy(SvcVinErrRawFrzCtrl.Name, sizeof(SvcVinErrRawFrzCtrl.Name), SvcVinErrCtrl.Name);
                AmbaUtility_StringAppend(SvcVinErrRawFrzCtrl.Name, (UINT32)sizeof(SvcVinErrRawFrzCtrl.Name), "_");
                AmbaUtility_StringAppend(SvcVinErrRawFrzCtrl.Name, (UINT32)sizeof(SvcVinErrRawFrzCtrl.Name), "RawFrz");

                PRetVal = AmbaKAL_MutexCreate(&(SvcVinErrRawFrzCtrl.Mutex), SvcVinErrRawFrzCtrl.Name);
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    PRN_VIN_ERR_LOG "Fail to create vin err raw freeze - create mutex fail!" PRN_VIN_ERR_NG
                }
            }

            if (RetVal == SVC_OK) {
                SvcVinErrCtrlFlg |= SVC_VIN_ERR_FLG_RAW_FRZ_INIT;

                if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_LOG_DBG1) > 0U) {
                    SvcVinErrTask_RawFrzInfo();
                }

                PRN_VIN_ERR_LOG "Success to create raw freeze control!" PRN_VIN_ERR_DBG1
            }
        }
    }

    return RetVal;
}

static UINT32 SvcVinErrTask_RawFrzDelete(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_RAW_FRZ_INIT) > 0U) {
        PRetVal = AmbaKAL_MutexDelete(&(SvcVinErrRawFrzCtrl.Mutex)); PRN_VIN_ERR_HDLR

        AmbaSvcWrap_MisraMemset(&SvcVinErrRawFrzCtrl, 0, sizeof(SvcVinErrRawFrzCtrl));

        SvcVinErrCtrlFlg &= ~SVC_VIN_ERR_FLG_RAW_FRZ_INIT;
    }

    return RetVal;
}

static void SvcVinErrTask_RawFrzInfo(void)
{
    UINT32 PRetVal;

    if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_RAW_FRZ_INIT) > 0U) {
        UINT32 VinIdx, SenIdx, Row, Col, FmtStrLen = 0U, DataOffset;
        UINT32 SenIdxs[AMBA_DSP_MAX_VIEWZONE_NUM], SenIdxNum = 0U;
        char TmpStrFmt[512];
        char TmpStrUnit[6];
        const SVC_VIN_ERR_RAW_FRZ_INFO_s *pFrzeInfo;
        SVC_WRAP_SNPRINT_s LocalSnPrint;
        UINT8 CurData;

        if (0U == SvcVinErrTask_MutexTake(&(SvcVinErrRawFrzCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {

            PRN_VIN_ERR_LOG " " PRN_VIN_ERR_API
            PRN_VIN_ERR_LOG "---- %sRaw Freeze Info%s ----"
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0 PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API
            PRN_VIN_ERR_LOG "    SelectBits : 0x%X"
                PRN_VIN_ERR_ARG_UINT32 SvcVinErrRawFrzCtrl.SelectBits PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API

            PRN_VIN_ERR_LOG "    ---- RawDataInfo ----" PRN_VIN_ERR_API
            for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
                if ((SvcVinErrRawFrzCtrl.SelectBits & SvcVinErrTask_BitGet(VinIdx)) == 0U) {
                    continue;
                }

                PRN_VIN_ERR_LOG "      VinID: %d"
                    PRN_VIN_ERR_ARG_UINT32 VinIdx           PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_API
                PRN_VIN_ERR_LOG "      AccumLine: %d "
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrRawFrzCtrl.RawDataInfo[VinIdx].AccumLine PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_API
                PRN_VIN_ERR_LOG "      DataOfsRow: " PRN_VIN_ERR_API

                AmbaSvcWrap_MisraMemset(TmpStrFmt, 0, sizeof(TmpStrFmt)); FmtStrLen = 0U;
                TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;

                for (Row = 0U; Row < SVC_VIN_ERR_RAW_FRZ_TILE_ROW; Row ++) {
                    if (FmtStrLen >= 507U) {
                        SvcWrap_strcpy(TmpStrFmt, sizeof(TmpStrFmt), "Out-of Local String Buffer");
                        break;
                    }

                    AmbaSvcWrap_MisraMemset(TmpStrUnit, 0, sizeof(TmpStrUnit));

                    AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "%04u ";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SvcVinErrRawFrzCtrl.RawDataInfo[VinIdx].DataOfsRow[Row]); LocalSnPrint.Argc ++;
                    PRetVal = SvcWrap_snprintf(TmpStrUnit, (UINT32)sizeof(TmpStrUnit), &LocalSnPrint);
                    if (PRetVal > 0U) {
                        AmbaUtility_StringAppend(TmpStrFmt, (UINT32)sizeof(TmpStrFmt), TmpStrUnit);
                        FmtStrLen += 5U;
                    }
                }
                AmbaPrint_PrintUInt5(TmpStrFmt, 0U, 0U, 0U, 0U, 0U);
            }

            for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {

                if ((SvcVinErrRawFrzCtrl.SelectBits & SvcVinErrTask_BitGet(VinIdx)) == 0U) {
                    continue;
                }

                SenIdxNum = 0U;
                AmbaSvcWrap_MisraMemset(SenIdxs, 0, sizeof(SenIdxs));
                PRetVal = SvcResCfg_GetSensorIdxsInVinID(VinIdx, SenIdxs, &SenIdxNum); PRN_VIN_ERR_HDLR

                for (SenIdx = 0U; SenIdx < SenIdxNum; SenIdx ++) {

                    pFrzeInfo = &(SvcVinErrRawFrzCtrl.FrzeInfo[VinIdx][SenIdxs[SenIdx]]);

                    PRN_VIN_ERR_LOG "    VinID(%d) SensorIdx(%d) State(0x%08X)"
                        PRN_VIN_ERR_ARG_UINT32 VinIdx           PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 SenIdxs[SenIdx]  PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 pFrzeInfo->State PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_API

                    PRN_VIN_ERR_LOG "    ---- OffsetRow ----" PRN_VIN_ERR_API
                    AmbaSvcWrap_MisraMemset(TmpStrFmt, 0, sizeof(TmpStrFmt)); FmtStrLen = 0U;
                    TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                    TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                    TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                    TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                    TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                    TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;

                    for (Row = 0U; Row < SVC_VIN_ERR_RAW_FRZ_TILE_ROW; Row ++) {
                        if (FmtStrLen >= 507U) {
                            SvcWrap_strcpy(TmpStrFmt, sizeof(TmpStrFmt), "Out-of Local String Buffer");
                            break;
                        }

                        AmbaSvcWrap_MisraMemset(TmpStrUnit, 0, sizeof(TmpStrUnit));

                        AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                        LocalSnPrint.pStrFmt = "%04u ";
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pFrzeInfo->OffsetRow[Row]); LocalSnPrint.Argc ++;
                        PRetVal = SvcWrap_snprintf(TmpStrUnit, (UINT32)sizeof(TmpStrUnit), &LocalSnPrint);
                        if (PRetVal > 0U) {
                            AmbaUtility_StringAppend(TmpStrFmt, (UINT32)sizeof(TmpStrFmt), TmpStrUnit);
                            FmtStrLen += 5U;
                        }
                    }
                    AmbaPrint_PrintUInt5(TmpStrFmt, 0U, 0U, 0U, 0U, 0U);

                    PRN_VIN_ERR_LOG "    ---- OffsetCol ----" PRN_VIN_ERR_API
                    AmbaSvcWrap_MisraMemset(TmpStrFmt, 0, sizeof(TmpStrFmt)); FmtStrLen = 0U;
                    TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                    TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                    TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                    TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                    TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                    TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;

                    for (Col = 0U; Col < SVC_VIN_ERR_RAW_FRZ_TILE_COL; Col ++) {
                        if (FmtStrLen >= 507U) {
                            SvcWrap_strcpy(TmpStrFmt, sizeof(TmpStrFmt), "Out-of Local String Buffer");
                            break;
                        }

                        AmbaSvcWrap_MisraMemset(TmpStrUnit, 0, sizeof(TmpStrUnit));

                        AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                        LocalSnPrint.pStrFmt = "%04u ";
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pFrzeInfo->OffsetCol[Col]); LocalSnPrint.Argc ++;
                        PRetVal = SvcWrap_snprintf(TmpStrUnit, (UINT32)sizeof(TmpStrUnit), &LocalSnPrint);
                        if (PRetVal > 0U) {
                            AmbaUtility_StringAppend(TmpStrFmt, (UINT32)sizeof(TmpStrFmt), TmpStrUnit);
                            FmtStrLen += 5U;
                        }
                    }
                    AmbaPrint_PrintUInt5(TmpStrFmt, 0U, 0U, 0U, 0U, 0U);

                    if (pFrzeInfo->pPreBuf != NULL) {

                        PRN_VIN_ERR_LOG "    ---- PreData ( %d x %d ) ----"
                            PRN_VIN_ERR_ARG_UINT32 SVC_VIN_ERR_RAW_FRZ_TILE_ROW PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 SVC_VIN_ERR_RAW_FRZ_TILE_COL PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_API
                        AmbaSvcWrap_MisraMemset(TmpStrFmt, 0, sizeof(TmpStrFmt)); FmtStrLen = 0U;
                        TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                        TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                        TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                        TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                        TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                        TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                        TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                        TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                        TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                        TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;

                        for (Col = 0U; Col < SVC_VIN_ERR_RAW_FRZ_TILE_COL; Col ++) {

                            if (FmtStrLen >= 507U) {
                                SvcWrap_strcpy(TmpStrFmt, sizeof(TmpStrFmt), "Out-of Local String Buffer");
                                break;
                            }

                            AmbaSvcWrap_MisraMemset(TmpStrUnit, 0, sizeof(TmpStrUnit));

                            AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                            LocalSnPrint.pStrFmt = "C%02u ";
                            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)Col; LocalSnPrint.Argc ++;
                            PRetVal = SvcWrap_snprintf(TmpStrUnit, (UINT32)sizeof(TmpStrUnit), &LocalSnPrint);
                            if (PRetVal > 0U) {
                                AmbaUtility_StringAppend(TmpStrFmt, (UINT32)sizeof(TmpStrFmt), TmpStrUnit);
                                FmtStrLen += 4U;
                            }
                        }
                        AmbaPrint_PrintUInt5(TmpStrFmt, 0U, 0U, 0U, 0U, 0U);

                        for (Row = 0U; Row < SVC_VIN_ERR_RAW_FRZ_TILE_ROW; Row ++) {

                            AmbaSvcWrap_MisraMemset(TmpStrFmt, 0, sizeof(TmpStrFmt)); FmtStrLen = 0U;
                            TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                            TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                            TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                            TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                            TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;
                            TmpStrFmt[FmtStrLen] = ' '; FmtStrLen ++;

                            AmbaSvcWrap_MisraMemset(TmpStrUnit, 0, sizeof(TmpStrUnit));

                            AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                            LocalSnPrint.pStrFmt = "R%02u ";
                            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)Row; LocalSnPrint.Argc ++;
                            PRetVal = SvcWrap_snprintf(TmpStrUnit, (UINT32)sizeof(TmpStrUnit), &LocalSnPrint);
                            if (PRetVal > 0U) {
                                AmbaUtility_StringAppend(TmpStrFmt, (UINT32)sizeof(TmpStrFmt), TmpStrUnit);
                                FmtStrLen += 4U;
                            }

                            for (Col = 0U; Col < SVC_VIN_ERR_RAW_FRZ_TILE_COL; Col ++) {

                                DataOffset = ( pFrzeInfo->OffsetRow[Row] * pFrzeInfo->BufPitch ) + pFrzeInfo->OffsetCol[Col];

                                AmbaSvcWrap_MisraMemset(TmpStrUnit, 0, sizeof(TmpStrUnit));

                                CurData = pFrzeInfo->pPreBuf[DataOffset];

                                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                                LocalSnPrint.pStrFmt = "%03u ";
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)CurData;
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFULL;
                                LocalSnPrint.Argc ++;
                                PRetVal = SvcWrap_snprintf(TmpStrUnit, (UINT32)sizeof(TmpStrUnit), &LocalSnPrint);
                                if (PRetVal > 0U) {
                                    AmbaUtility_StringAppend(TmpStrFmt, (UINT32)sizeof(TmpStrFmt), TmpStrUnit);
                                    FmtStrLen += 4U;
                                }
                            }

                            AmbaPrint_PrintUInt5(TmpStrFmt, 0U, 0U, 0U, 0U, 0U);
                        }
                    }
                }
            }

            SvcVinErrTask_MutexGive(&(SvcVinErrRawFrzCtrl.Mutex));
        }
    } else {
        PRN_VIN_ERR_LOG "Vin err Control - Raw Freeze Info does not create!" PRN_VIN_ERR_API
    }
}

static UINT32 SvcVinErrTask_RawFrzProc(SVC_VIN_ERR_RAW_FRZ_UPD_s *pRawUpd)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    UINT32 VinID = 0xFFFFFFFFU;

    if (pRawUpd == NULL) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to compare raw data - input control cmd should not null!" PRN_VIN_ERR_NG
    } else {
        VinID = pRawUpd->VinID;

        AmbaMisra_TouchUnused(pRawUpd);

        if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to compare raw data - invalid VinID(%d)!"
                PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        } else if ((SvcVinErrRawFrzCtrl.SelectBits & SvcVinErrTask_BitGet(VinID)) == 0U) {
            PRN_VIN_ERR_LOG "VinID(%D) raw freeze detect is disable!"
                PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        } else {
            AMBA_KAL_EVENT_FLAG_INFO_s CurVinState;

            AmbaSvcWrap_MisraMemset(&CurVinState, 0, sizeof(CurVinState));
            PRetVal = AmbaKAL_EventFlagQuery(&(SvcVinErrCtrl.ErrState[VinID].VinStateFlag), &(CurVinState));
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_VIN_ERR_LOG "Fail to compare raw data - query VinID(%d) vin state fail! ErrCode(0x%08x)"
                    PRN_VIN_ERR_ARG_UINT32 VinID   PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 PRetVal PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_NG
            } else if (((CurVinState.CurrentFlags & SVC_VIN_ERR_VIN_STATE_LIV) == 0U) ||
                       ((CurVinState.CurrentFlags & SVC_VIN_ERR_VIN_STATE_TOUT) > 0U)) {
                PRN_VIN_ERR_LOG "Current VinID(%d) state(0x%08x) does not ready for raw compare"
                    PRN_VIN_ERR_ARG_UINT32 VinID                    PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 CurVinState.CurrentFlags PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_DBG2
            } else {
                UINT8 *pRawData = pRawUpd->pRawBuf;
                UINT32 Pitch;
                UINT32 Row, Col, DataOffset, IsFrze = 0U;
                UINT32 SenIdx;
                UINT32 SenIdxs[16U], SenIdxNum;
                UINT32 TimeStampS, TimeStampE, TimeStampDiff;
                SVC_VIN_ERR_RAW_FRZ_INFO_s *pFrzeInfo = NULL;
                UINT32 FovIdx;
                UINT32 FovIDs[AMBA_DSP_MAX_VIEWZONE_NUM], FovNum = 0U;

                PRetVal = AmbaKAL_GetSysTickCount(&TimeStampS); PRN_VIN_ERR_HDLR

                SvcVinErrTask_RawFrzInvalidate(pRawUpd);

                Pitch = pRawUpd->Pitch;

                SenIdxNum = 0U;
                AmbaSvcWrap_MisraMemset(SenIdxs, 0, sizeof(SenIdxs));
                PRetVal = SvcResCfg_GetSensorIdxsInVinID(VinID, SenIdxs, &SenIdxNum);
                if ((PRetVal == 0U) && (pRawData != NULL)) {

                    PRetVal = SvcVinErrTask_MutexTake(&(SvcVinErrRawFrzCtrl.Mutex), AMBA_KAL_NO_WAIT);
                    if (PRetVal == 0U) {

                        for (SenIdx = 0U; SenIdx < SenIdxNum; SenIdx ++) {
                            pFrzeInfo = &(SvcVinErrRawFrzCtrl.FrzeInfo[VinID][SenIdxs[SenIdx]]);
                            pFrzeInfo->BufPitch = Pitch;

                            if ((pFrzeInfo->State == SVC_VIN_ERR_RAW_FRZ_STATE_ALIVE) &&
                                (pFrzeInfo->pPreBuf != NULL)) {

                                IsFrze = 1U;

                                PRN_VIN_ERR_LOG "[RAW FREZ] VinID(%02d) SenIdx(%02d) Compare PreRawBuf(%p) and CurRawBuf(%p)"
                                    PRN_VIN_ERR_ARG_UINT32 VinID              PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT32 SenIdxs[SenIdx]    PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_CPOINT pFrzeInfo->pPreBuf PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_CPOINT pRawData           PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_DBG2

                                for (Row = 0U; Row < SVC_VIN_ERR_RAW_FRZ_TILE_ROW; Row ++) {
                                    for (Col = 0U; Col < SVC_VIN_ERR_RAW_FRZ_TILE_COL; Col ++) {

                                        DataOffset = ( pFrzeInfo->OffsetRow[Row] * Pitch ) + pFrzeInfo->OffsetCol[Col];
                                        if (pRawData[DataOffset] != pFrzeInfo->pPreBuf[DataOffset]) {
                                            IsFrze = 0U;
                                            break;
                                        }
                                    }

                                    if (IsFrze == 0U) {
                                        break;
                                    }
                                }

                                if (IsFrze > 0U) {
                                    PRN_VIN_ERR_LOG "VinID(%s%d%s) Sensor Index(%s%d%s) Raw %sFreeze%s"
                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                                        PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                                        PRN_VIN_ERR_ARG_UINT32 SenIdx                     PRN_VIN_ERR_ARG_POST
                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_DBG2

                                    AmbaSvcWrap_MisraMemset(FovIDs, 0, sizeof(FovIDs));
                                    PRetVal = SvcResCfg_GetFovIdxsInSensorIdx(VinID, SenIdx, FovIDs, &FovNum);
                                    if (PRetVal == 0U) {
                                        for (FovIdx = 0U; FovIdx < FovNum; FovIdx ++) {
                                            SvcVinErrTask_FovFreezeUpd(FovIDs[FovIdx], 1U);
                                        }
                                    }

                                    pFrzeInfo->State = SVC_VIN_ERR_RAW_FRZ_STATE_FREEZE;
                                } else {
                                    PRN_VIN_ERR_LOG "VinID(%s%d%s) Sensor Index(%s%d%s) Raw %sAlive%s"
                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                                        PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                                        PRN_VIN_ERR_ARG_UINT32 SenIdx                     PRN_VIN_ERR_ARG_POST
                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_7 PRN_VIN_ERR_ARG_POST
                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_DBG2

                                    pFrzeInfo->State = SVC_VIN_ERR_RAW_FRZ_STATE_ALIVE;
                                }
                            }

                            pFrzeInfo->pPreBuf = pRawData;
                            PRN_VIN_ERR_LOG "[RAW FREZ] VinID(%02d) SenIdx(%02d) Update PreRawBuf(%p)"
                                PRN_VIN_ERR_ARG_UINT32 VinID              PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_UINT32 SenIdxs[SenIdx]    PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_ARG_CPOINT pFrzeInfo->pPreBuf PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_DBG2
                        }

                        SvcVinErrTask_MutexGive(&(SvcVinErrRawFrzCtrl.Mutex));
                    }
                }

                PRetVal = AmbaKAL_GetSysTickCount(&TimeStampE); PRN_VIN_ERR_HDLR

                if (TimeStampE >= TimeStampS) {
                    TimeStampDiff = TimeStampE - TimeStampS;
                } else {
                    TimeStampDiff = ( 0xFFFFFFFFU - TimeStampS ) + TimeStampE;
                }

                PRN_VIN_ERR_LOG "VinID(%s%d%s) Raw compare spending time %s%d%s / %s%d%s / %s%d%s\n"
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 VinID                      PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0 PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 TimeStampS                 PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0 PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 TimeStampE                 PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0 PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 TimeStampDiff              PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_DBG2
            }
        }
    }

    return RetVal;
}

static void SvcVinErrTask_RawFrzInvalidate(const SVC_VIN_ERR_RAW_FRZ_UPD_s *pRawUpd)
{
    if (pRawUpd == NULL) {
        PRN_VIN_ERR_LOG "Fail to proc raw comp invalid data - input info should not null!" PRN_VIN_ERR_DBG2
    } else if (pRawUpd->VinID >= AMBA_DSP_MAX_VIN_NUM) {
        PRN_VIN_ERR_LOG "Fail to proc raw comp invalid data - invalid VinID(%d)"
            PRN_VIN_ERR_ARG_UINT32 pRawUpd->VinID PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_DBG2
    } else {
        UINT32 PRetVal;
        const UINT8 *pRawData = pRawUpd->pRawBuf;

        if (pRawData == NULL) {
            PRN_VIN_ERR_LOG "Fail to proc raw comp invalid data - raw data should not null" PRN_VIN_ERR_DBG2
        } else {
            const SVC_VIN_ERR_RAW_FRZ_DATA_REGION_s *pDataRegion = &(SvcVinErrRawFrzCtrl.RawDataInfo[pRawUpd->VinID]);
            UINT32 ReqSize = pRawUpd->Pitch * pDataRegion->AccumLine;

            if (ReqSize > 0U) {
                UINT32 RowIdx;

                for (RowIdx = 0U; RowIdx < SVC_VIN_ERR_RAW_FRZ_TILE_ROW; RowIdx ++) {
                    pRawData = &(pRawUpd->pRawBuf[pDataRegion->DataOfsRow[RowIdx] * pRawUpd->Pitch]);
                    PRetVal = SvcVinErrTask_DataInvalidate(pRawData, ReqSize); PRN_VIN_ERR_HDLR
                }
            }
        }
    }
}

static void SvcVinErrTask_RawFrzEnable(UINT32 FovID, UINT32 Enable)
{
    if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_RAW_FRZ_INIT) == 0U) {
        PRN_VIN_ERR_LOG "Fail to on/off raw frz - initial raw frz module first!" PRN_VIN_ERR_NG
    } else if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
        PRN_VIN_ERR_LOG "Fail to on/off raw frz - invalid FovID(%d)!"
            PRN_VIN_ERR_ARG_UINT32 FovID PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_NG
    } else if (0U != SvcVinErrTask_MutexTake(&(SvcVinErrRawFrzCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
        PRN_VIN_ERR_LOG "Fail to on/off raw frz - take mutex fail!" PRN_VIN_ERR_NG
    } else {
        UINT32 PRetVal;
        UINT32 VinID = 0xFFU;
        UINT32 SensorIdx = 0xFFU;

        PRetVal = SvcResCfg_GetSensorIdxOfFovIdx(FovID, &VinID, &SensorIdx);
        if (PRetVal != 0U) {
            PRN_VIN_ERR_LOG "Fail to on/off raw frz - get VinID and SensorIdx fail!" PRN_VIN_ERR_NG
        } else if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
            PRN_VIN_ERR_LOG "Fail to on/off raw frz - invalid VinID(%d)!"
                PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        } else if (SensorIdx >= SVC_VIN_ERR_MAX_SER_NUM) {
            PRN_VIN_ERR_LOG "Fail to on/off raw frz - SensorIdx(%d) out-of range!"
                PRN_VIN_ERR_ARG_UINT32 SensorIdx PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        } else {

            if (Enable > 0U) {
                SvcVinErrRawFrzCtrl.FrzeInfo[VinID][SensorIdx].State = SVC_VIN_ERR_RAW_FRZ_STATE_ALIVE;
            } else {
                SvcVinErrRawFrzCtrl.FrzeInfo[VinID][SensorIdx].State = SVC_VIN_ERR_RAW_FRZ_STATE_DISABLE;
            }

            PRN_VIN_ERR_LOG "Success to %s raw freeze. FovID(%d), VinID(%d)/SenIdx(%d)"
                PRN_VIN_ERR_ARG_CSTR   ((Enable>0U)?"enable":"disable") PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 FovID     PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 VinID     PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 SensorIdx PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_DBG1
        }


        SvcVinErrTask_MutexGive(&(SvcVinErrRawFrzCtrl.Mutex));
    }
}

static UINT32 SvcVinErrTask_RawFrzReset(UINT32 FovID)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_RAW_FRZ_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to reset raw frz - initial raw frz module first!" PRN_VIN_ERR_NG
    } else if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to reset raw frz - invalid FovID(%d)!"
            PRN_VIN_ERR_ARG_UINT32 FovID PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_NG
    } else if (0U != SvcVinErrTask_MutexTake(&(SvcVinErrRawFrzCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to reset raw frz - take mutex fail!" PRN_VIN_ERR_NG
    } else {
        UINT32 VinID = 0xFFU;
        UINT32 SensorIdx = 0xFFU;

        PRetVal = SvcResCfg_GetSensorIdxOfFovIdx(FovID, &VinID, &SensorIdx);
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to reset raw frz - get VinID and SensorIdx fail!" PRN_VIN_ERR_NG
        } else if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to reset raw frz - invalid VinID(%d)!"
                PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        } else if (SensorIdx >= SVC_VIN_ERR_MAX_SER_NUM) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to reset raw frz - SensorIdx(%d) out-of range!"
                PRN_VIN_ERR_ARG_UINT32 SensorIdx PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        } else {

            SvcVinErrTask_FovFreezeUpd(FovID, 0U);

            SvcVinErrRawFrzCtrl.FrzeInfo[VinID][SensorIdx].State = SVC_VIN_ERR_RAW_FRZ_STATE_ALIVE;
            SvcVinErrRawFrzCtrl.FrzeInfo[VinID][SensorIdx].pPreBuf = NULL;

            PRN_VIN_ERR_LOG "Success to reset raw freeze. FovID(%d), VinID(%d)/SenIdx(%d)"
                PRN_VIN_ERR_ARG_UINT32 FovID     PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 VinID     PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 SensorIdx PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_OK
        }


        SvcVinErrTask_MutexGive(&(SvcVinErrRawFrzCtrl.Mutex));

    }

    return RetVal;
}

static void SvcVinErrTask_RawFrzVinEnable(UINT32 VinID, UINT32 Enable)
{
    if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_RAW_FRZ_INIT) == 0U) {
        PRN_VIN_ERR_LOG "Fail to on/off raw(vin) frz - initial raw frz module first!" PRN_VIN_ERR_NG
    } else if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        PRN_VIN_ERR_LOG "Fail to on/off raw(vin) frz - invalid VinIDID(%d)!"
            PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_NG
    } else if (0U != SvcVinErrTask_MutexTake(&(SvcVinErrRawFrzCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
        PRN_VIN_ERR_LOG "Fail to on/off raw(vin) frz - take mutex fail!" PRN_VIN_ERR_NG
    } else {
        UINT32 Idx;

        for (Idx = 0U; Idx < SVC_VIN_ERR_MAX_SER_NUM; Idx ++) {
            if (Enable > 0U) {
                SvcVinErrRawFrzCtrl.FrzeInfo[VinID][Idx].State = SVC_VIN_ERR_RAW_FRZ_STATE_ALIVE;
            } else {
                SvcVinErrRawFrzCtrl.FrzeInfo[VinID][Idx].State = SVC_VIN_ERR_RAW_FRZ_STATE_DISABLE;
            }
        }

        SvcVinErrTask_MutexGive(&(SvcVinErrRawFrzCtrl.Mutex));
    }
}

static UINT32 SvcVinErrTask_RawFrzVinReset(UINT32 VinID)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_RAW_FRZ_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to reset raw(vin) frz - initial raw frz module first!" PRN_VIN_ERR_NG
    } else if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to reset raw(vin) frz - invalid VinIDID(%d)!"
            PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_NG
    } else if (0U != SvcVinErrTask_MutexTake(&(SvcVinErrRawFrzCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to reset raw(vin) frz - take mutex fail!" PRN_VIN_ERR_NG
    } else {
        UINT32 FovIDs[AMBA_DSP_MAX_VIEWZONE_NUM], FovNum = 0U, Idx;

        AmbaSvcWrap_MisraMemset(FovIDs, 0, sizeof(FovIDs));
        PRetVal = SvcResCfg_GetFovIdxsInVinID(VinID, FovIDs, &FovNum);
        if (PRetVal == 0U) {

            for (Idx = 0U; Idx < FovNum; Idx ++) {
                SvcVinErrTask_FovFreezeUpd(FovIDs[Idx], 0U);
            }

            for (Idx = 0U; Idx < SVC_VIN_ERR_MAX_SER_NUM; Idx ++) {
                SvcVinErrRawFrzCtrl.FrzeInfo[VinID][Idx].State = SVC_VIN_ERR_RAW_FRZ_STATE_ALIVE;
                SvcVinErrRawFrzCtrl.FrzeInfo[VinID][Idx].pPreBuf = NULL;
            }

            PRN_VIN_ERR_LOG "Success to reset raw freeze. VinID(%d)"
                PRN_VIN_ERR_ARG_UINT32 VinID     PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_OK
        }

        SvcVinErrTask_MutexGive(&(SvcVinErrRawFrzCtrl.Mutex));

        // Wait raw data event to make sure system ready to service it.
        PRetVal = AmbaKAL_EventFlagClear(&(SvcVinErrCtrl.ErrState[VinID].VinStateFlag), SVC_VIN_ERR_VIN_STATE_RAW_RDY); PRN_VIN_ERR_HDLR
        PRetVal = AmbaKAL_EventFlagGet(&(SvcVinErrCtrl.ErrState[VinID].VinStateFlag),
                                       SVC_VIN_ERR_VIN_STATE_RAW_RDY,
                                       AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_NONE, &PRetVal, 1000U); PRN_VIN_ERR_HDLR

    }

    return RetVal;
}

static UINT32 SvcVinErrTask_OsdCreate(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    static SVC_VIN_ERR_OSD_PTN_INFO_s SvcVinErrOsdPtnInfo[SVC_VIN_ERR_OSD_PTN_NUM] GNU_SECTION_NOZEROINIT;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_OSD_INIT) > 0U) {
        RetVal = SVC_OK;
        PRN_VIN_ERR_LOG "Vin err osd control has been created!" PRN_VIN_ERR_API
    } else if  (pResCfg == NULL) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to create vin err osd ctrl - invalid resolution cfg!" PRN_VIN_ERR_NG
    } else {
        UINT32 Idx, PtnIdx = 0U, ChanIdx;
        UINT32 VoutID, VoutWidth, VoutHeight, OsdWidth, OsdHeight;
        UINT16 TmpU16 = 0U;
        SVC_VIN_ERR_OSD_UNIT_s *pUnit;
        const SVC_CHAN_CFG_s *pChan;

        AmbaSvcWrap_MisraMemset(SvcVinErrOsdPtnInfo, 0, sizeof(SvcVinErrOsdPtnInfo));
        SvcVinErrOsdPtnInfo[PtnIdx] = (SVC_VIN_ERR_OSD_PTN_INFO_s) { SVC_VIN_ERR_OSD_PTN_ID_SIG_LST,  SVC_VIN_ERR_OSD_PTN_SIG_LOST,   11U, SVC_VIN_ERR_OSD_PTN_ALIGN_CC }; PtnIdx++;
        SvcVinErrOsdPtnInfo[PtnIdx] = (SVC_VIN_ERR_OSD_PTN_INFO_s) { SVC_VIN_ERR_OSD_PTN_ID_RAW_FRZ,  SVC_VIN_ERR_OSD_PTN_RAW_FREEZE, 10U, SVC_VIN_ERR_OSD_PTN_ALIGN_CC }; PtnIdx++;
        SvcVinErrOsdPtnInfo[PtnIdx] = (SVC_VIN_ERR_OSD_PTN_INFO_s) { SVC_VIN_ERR_OSD_PTN_ID_RCV_PROC, SVC_VIN_ERR_OSD_PTN_RCV_PROC,   14U, SVC_VIN_ERR_OSD_PTN_ALIGN_CC }; PtnIdx++;
        SvcVinErrOsdPtnInfo[PtnIdx] = (SVC_VIN_ERR_OSD_PTN_INFO_s) { SVC_VIN_ERR_OSD_PTN_ID_VOUT_LTN, SVC_VIN_ERR_OSD_PTN_VOUT_LTN,   11U, SVC_VIN_ERR_OSD_PTN_ALIGN_CC }; PtnIdx++;
        SvcVinErrOsdPtnInfo[PtnIdx] = (SVC_VIN_ERR_OSD_PTN_INFO_s) { SVC_VIN_ERR_OSD_PTN_ID_FOV_LTN,  SVC_VIN_ERR_OSD_PTN_FOV_LTN,     9U, SVC_VIN_ERR_OSD_PTN_ALIGN_BC }; PtnIdx++;

        if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_LOG_DBG1) > 0U) {
            for (Idx = 0U; Idx < PtnIdx; Idx ++) {
                PRN_VIN_ERR_LOG "OsdStrPtn id(%02d), Len(%02d), %s"
                    PRN_VIN_ERR_ARG_UINT32 Idx                                PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrOsdPtnInfo[Idx].StrPtnLen PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SvcVinErrOsdPtnInfo[Idx].StrPtn    PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_DBG1
            }
        }

        AmbaSvcWrap_MisraMemset(&SvcVinErrOsdCtrl, 0, sizeof(SvcVinErrOsdCtrl));
          AmbaUtility_StringCopy(SvcVinErrOsdCtrl.Name, sizeof(SvcVinErrOsdCtrl.Name), SvcVinErrCtrl.Name);
        AmbaUtility_StringAppend(SvcVinErrOsdCtrl.Name, (UINT32)sizeof(SvcVinErrOsdCtrl.Name), "_");
        AmbaUtility_StringAppend(SvcVinErrOsdCtrl.Name, (UINT32)sizeof(SvcVinErrOsdCtrl.Name), "Osd");

        SvcVinErrOsdCtrl.NumPtnInfos = PtnIdx;
        SvcVinErrOsdCtrl.pOsdPtnInfo = SvcVinErrOsdPtnInfo;
        SvcVinErrOsdCtrl.NumUnits    = pResCfg->FovNum;

        for (Idx = 0U; Idx < pResCfg->DispNum; Idx ++) {
            VoutID = pResCfg->DispStrm[Idx].VoutID;

            if (VoutID < AMBA_DSP_MAX_VOUT_NUM) {

                SvcVinErrOsdCtrl.SelectBits |= SvcVinErrTask_BitGet(VoutID);

                SvcVinErrOsdCtrl.OsdInfo[VoutID].DrawLevel = SVC_VIN_ERR_GUI_CANVAS_LVL;
                SvcVinErrOsdCtrl.OsdInfo[VoutID].pDrawFunc = SvcVinErrTask_OsdDrawFunc;
                SvcVinErrOsdCtrl.OsdInfo[VoutID].pUpdateFunc = SvcVinErrTask_OsdUpdateFunc;

                VoutWidth  = (UINT32)(pResCfg->DispStrm[Idx].StrmCfg.MaxWin.Width);  VoutWidth &= 0x0000FFFFU;
                VoutHeight = (UINT32)(pResCfg->DispStrm[Idx].StrmCfg.MaxWin.Height); VoutHeight &= 0x0000FFFFU;
                if (0U != SvcOsd_GetOsdBufSize(VoutID, &OsdWidth, &OsdHeight)) {
                    RetVal = SVC_NG;
                    PRN_VIN_ERR_LOG "Fail to create vin err osd ctrl - get vout(%d) osd buffer size fail!"
                        PRN_VIN_ERR_ARG_UINT32 VoutID PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_NG
                    break;
                }

                SvcVinErrOsdCtrl.OsdInfo[VoutID].VoutWidth  = VoutWidth;
                SvcVinErrOsdCtrl.OsdInfo[VoutID].VoutHeight = VoutHeight;
                SvcVinErrOsdCtrl.OsdInfo[VoutID].OsdWidth   = OsdWidth;
                SvcVinErrOsdCtrl.OsdInfo[VoutID].OsdHeight  = OsdHeight;

                for (ChanIdx = 0U; ChanIdx < pResCfg->DispStrm[Idx].StrmCfg.NumChan; ChanIdx ++ ) {
                    pChan = &(pResCfg->DispStrm[Idx].StrmCfg.ChanCfg[ChanIdx]);

                    if (pChan->FovId < AMBA_DSP_MAX_VIEWZONE_NUM) {
                        pUnit = &(SvcVinErrOsdCtrl.FovUnit[pChan->FovId]);

                        if (SvcVinErrCtrl.FovInfo[pChan->FovId].SrcVinID != 0xDEADBEEFU) {
                            pUnit->VoutSelectBits = SvcVinErrTask_BitGet(VoutID);

                            /* Configure each fov's draw window */
                            pUnit->DrawWin[VoutID].StartX = (UINT32)(pChan->DstWin.OffsetX) & 0x0000FFFFU;
                            pUnit->DrawWin[VoutID].StartX = ( pUnit->DrawWin[VoutID].StartX * OsdWidth ) / VoutWidth;

                            pUnit->DrawWin[VoutID].StartY = (UINT32)(pChan->DstWin.OffsetY) & 0x0000FFFFU;
                            pUnit->DrawWin[VoutID].StartY = ( pUnit->DrawWin[VoutID].StartY * OsdHeight ) / VoutHeight;

                            pUnit->DrawWin[VoutID].Width = (UINT32)(pChan->DstWin.Width) & 0x0000FFFFU;
                            pUnit->DrawWin[VoutID].Width = ( pUnit->DrawWin[VoutID].Width * OsdWidth ) / VoutWidth;

                            pUnit->DrawWin[VoutID].Height = (UINT32)(pChan->DstWin.Height) & 0x0000FFFFU;
                            pUnit->DrawWin[VoutID].Height = ( pUnit->DrawWin[VoutID].Height * OsdHeight ) / VoutHeight;

                            if (pResCfg->DispStrm[Idx].VideoRotateFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP) {
                                pUnit->DrawWin[VoutID].StartX += pUnit->DrawWin[VoutID].Width;
                                pUnit->DrawWin[VoutID].StartX  = OsdWidth - pUnit->DrawWin[VoutID].StartX;
                            }

                            if (pResCfg->DispStrm[Idx].StrmCfg.Win.Height < pResCfg->DispStrm[Idx].StrmCfg.MaxWin.Height) {
                                TmpU16 = pResCfg->DispStrm[Idx].StrmCfg.MaxWin.Height - pResCfg->DispStrm[Idx].StrmCfg.Win.Height;
                                PRetVal = (UINT32)TmpU16; PRetVal &= 0xFFFFU;
                                PRetVal = ( ( PRetVal >> 1UL ) * OsdHeight ) / VoutHeight;
                                pUnit->DrawWin[VoutID].StartY += PRetVal;
                            }

                            /* Configure each fov pattern draw info */
                            for (PtnIdx = 0U; PtnIdx < SvcVinErrOsdCtrl.NumPtnInfos; PtnIdx ++) {
                                SvcvinErrTask_OsdDrawStrCfg(VoutID, &(pUnit->DrawWin[VoutID]),
                                                            &(SvcVinErrOsdCtrl.pOsdPtnInfo[PtnIdx]),
                                                            &(pUnit->DrawStrWin[VoutID][PtnIdx]),
                                                            &(pUnit->DrawFontSize[VoutID][PtnIdx]));
                            }

                            pUnit->State |= (UINT32)(SVC_VIN_ERR_OSD_UNIT_ENABLE << (VoutID << 3U));
                            pUnit->State |= (UINT32)(SVC_VIN_ERR_OSD_UNIT_DISPLAY << (VoutID << 3U));

                            SvcVinErrOsdCtrl.OsdInfo[VoutID].DrawUpd = 1U;
                        }
                    }
                }
            }
        }

        if ( ( SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_LOG_DBG1 ) > 0U ) {
            SvcVinErrTask_OsdInfo();
        }

        if (RetVal == SVC_OK) {

            for (Idx = 0U; Idx < AMBA_DSP_MAX_VOUT_NUM; Idx ++) {
                if ((SvcVinErrOsdCtrl.SelectBits & SvcVinErrTask_BitGet(Idx)) > 0U) {
                    SvcGui_Register(Idx, SvcVinErrOsdCtrl.OsdInfo[Idx].DrawLevel,
                                         SvcVinErrOsdCtrl.Name,
                                         SvcVinErrOsdCtrl.OsdInfo[Idx].pDrawFunc,
                                         SvcVinErrOsdCtrl.OsdInfo[Idx].pUpdateFunc);
                }
            }
        }

        if (RetVal == SVC_OK) {
            PRetVal = AmbaKAL_MutexCreate(&(SvcVinErrOsdCtrl.Mutex), SvcVinErrOsdCtrl.Name);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_VIN_ERR_LOG "Fail to create vin err osd ctrl - create mutex fail!" PRN_VIN_ERR_NG
            }
        }

        if (RetVal == SVC_OK) {
            SvcVinErrCtrlFlg |= SVC_VIN_ERR_FLG_OSD_INIT;
            PRN_VIN_ERR_LOG "Success to create vin err osd ctrl" PRN_VIN_ERR_DBG1
        }
    }

    return RetVal;
}

static UINT32 SvcVinErrTask_OsdDelete(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_OSD_INIT) == 0U) {
        RetVal = SVC_OK;
        PRN_VIN_ERR_LOG "Vin err osd control be not created!" PRN_VIN_ERR_API
    } else {
        UINT32 Idx;

        for (Idx = 0U; Idx < AMBA_DSP_MAX_VOUT_NUM; Idx ++) {
            if ((SvcVinErrOsdCtrl.SelectBits & SvcVinErrTask_BitGet(Idx)) > 0U) {
                SvcGui_Unregister(Idx, SvcVinErrOsdCtrl.OsdInfo[Idx].DrawLevel);
            }
        }

        PRetVal = AmbaKAL_MutexDelete(&(SvcVinErrOsdCtrl.Mutex)); PRN_VIN_ERR_HDLR
        AmbaSvcWrap_MisraMemset(&SvcVinErrOsdCtrl, 0, sizeof(SvcVinErrOsdCtrl));
        PRN_VIN_ERR_LOG "Success to delete vin err osd ctrl!" PRN_VIN_ERR_DBG1

        SvcVinErrCtrlFlg &= ~SVC_VIN_ERR_FLG_OSD_INIT;
    }

    return RetVal;
}

static void SvcVinErrTask_OsdUpdateLock(UINT32 VinID, UINT32 UpdateLock)
{
    UINT32 PRetVal;

    if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_OSD_INIT) == 0U) {
        PRN_VIN_ERR_LOG "Fail to update osd lock - create vin err osd ctrl first!" PRN_VIN_ERR_NG
    } else {
        if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
            PRN_VIN_ERR_LOG "Fail to update osd lock - invalid VinID(%d)"
                PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        } else {
            PRetVal = SvcVinErrTask_MutexTake(&(SvcVinErrOsdCtrl.Mutex), AMBA_KAL_WAIT_FOREVER);
            if (PRetVal != 0U) {
                PRN_VIN_ERR_LOG "Fail to update osd - take mutex timeout!" PRN_VIN_ERR_NG
            } else {
                SVC_VIN_ERR_OSD_UNIT_s *pUnit;
                UINT32 FovIdx;

                for (FovIdx = 0U; FovIdx < SvcVinErrOsdCtrl.NumUnits; FovIdx ++) {
                    pUnit = &(SvcVinErrOsdCtrl.FovUnit[FovIdx]);

                    if (SvcVinErrCtrl.FovInfo[FovIdx].SrcVinID == VinID) {
                        if (UpdateLock > 0U) {
                            pUnit->State |= (UINT32)SVC_VIN_ERR_OSD_UPD_LOCK;
                        } else {
                            pUnit->State &= ~((UINT32)SVC_VIN_ERR_OSD_UPD_LOCK);
                        }
                    }
                }

                SvcVinErrTask_MutexGive(&(SvcVinErrOsdCtrl.Mutex));
            }
        }
    }
}

static void SvcVinErrTask_OsdUpdate(UINT32 VinID)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_OSD_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to update osd - create vin err osd ctrl first!" PRN_VIN_ERR_NG
    } else {
        if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to update osd - invalid VinID(%d)"
                PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        } else {
            UINT32 SerDesIdx, VidLockState = 0U, SensorFreezeState;
            SVC_VIN_ERR_OSD_UNIT_s *pUnit;
            AMBA_KAL_EVENT_FLAG_INFO_s CurEventInfo;
            SVC_VIN_ERR_FOV_INFO_s *pFovInfo = NULL;

            // Get vin state. Is vin timeout or not
            AmbaSvcWrap_MisraMemset(&CurEventInfo, 0, sizeof(CurEventInfo));
            PRetVal = AmbaKAL_EventFlagQuery(&(SvcVinErrCtrl.ErrState[VinID].VinStateFlag), &CurEventInfo); PRN_VIN_ERR_HDLR

            // Get video lock state
            for (SerDesIdx = 0U; SerDesIdx < SVC_VIN_ERR_MAX_SER_NUM; SerDesIdx ++) {
                if ((SvcVinErrCtrl.SerDesSelectBits[VinID] & SvcVinErrTask_BitGet(SerDesIdx)) > 0U) {
                    if ((SvcVinErrTask_GetSerDesState(VinID, SerDesIdx) & SVC_VIN_ERR_SER_STAT_VID_LOCK) > 0U) {
                        VidLockState |= SvcVinErrTask_BitGet(SerDesIdx);
                    }
                }
            }

            // Get raw freeze state
            SensorFreezeState = SvcVinErrCtrl.ErrState[VinID].FovFreezeState;

            // If vin timoeut, set vid lock state as zero ( all link lost )
            if ((CurEventInfo.CurrentFlags & SVC_VIN_ERR_VIN_STATE_TOUT) > 0U) {
                VidLockState = 0U;
            }

            PRetVal = SvcVinErrTask_MutexTake(&(SvcVinErrOsdCtrl.Mutex), AMBA_KAL_WAIT_FOREVER);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_VIN_ERR_LOG "Fail to update osd - take mutex timeout!" PRN_VIN_ERR_NG
            } else {
                UINT32 VoutIdx;
                UINT32 FovIdx;
                UINT32 ReqOsdPtn;

                for (FovIdx = 0U; FovIdx < SvcVinErrOsdCtrl.NumUnits; FovIdx ++) {
                    pUnit = &(SvcVinErrOsdCtrl.FovUnit[FovIdx]);
                    pFovInfo = &(SvcVinErrCtrl.FovInfo[FovIdx]);

                    if ((pFovInfo->SrcVinID == VinID) && (pFovInfo->SrcSerDesIdx < SVC_VIN_ERR_MAX_SELECT_BITS)) {

                        if ((pUnit->State & (UINT32)SVC_VIN_ERR_OSD_UPD_LOCK) == 0U) {
                            for ( VoutIdx = 0U; VoutIdx < AMBA_DSP_MAX_VOUT_NUM; VoutIdx ++) {
                                if ((pUnit->VoutSelectBits & SvcVinErrTask_BitGet(VoutIdx)) > 0U) {
                                    if ((pUnit->State & (UINT32)(SVC_VIN_ERR_OSD_UNIT_ENABLE << (VoutIdx << 3U))) > 0U) {

                                        /* video lock does not exist */
                                        if ((VidLockState & SvcVinErrTask_BitGet(pFovInfo->SrcSerDesIdx)) == 0U) {

                                            if ((CurEventInfo.CurrentFlags & SVC_VIN_ERR_VIN_STATE_RECOVER) > 0U) {
                                                // Configure osd signal recover pattern
                                                ReqOsdPtn = SvcVinErrTask_BitGet(SVC_VIN_ERR_OSD_PTN_ID_RCV_PROC);
                                            } else {
                                                // Configure osd signal lost pattern
                                                ReqOsdPtn = SvcVinErrTask_BitGet(SVC_VIN_ERR_OSD_PTN_ID_SIG_LST);
                                            }

                                            if ((pUnit->OsdPtnSelectBits & ReqOsdPtn) != ReqOsdPtn) {

                                                pUnit->OsdPtnSelectBits = ReqOsdPtn;

                                                // Enable osd draw function
                                                pUnit->State |= (UINT32)(SVC_VIN_ERR_OSD_UNIT_DISPLAY << (VoutIdx << 3U));
                                                SvcVinErrOsdCtrl.OsdInfo[VoutIdx].DrawUpd = 1U;

                                                PRN_VIN_ERR_LOG "VinID(%s%d%s) SerDesIdx(%s%d%s) FovID(%s%d%s) video lock lost. Enable VoutID(%s%d%s) osd timeout. %s0x%08x%s"
                                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_ARG_UINT32 pFovInfo->SrcVinID         PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_ARG_UINT32 pFovInfo->SrcSerDesIdx     PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_ARG_UINT32 FovIdx                     PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_ARG_UINT32 VoutIdx                    PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0 PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_ARG_UINT32 pUnit->State               PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                                PRN_VIN_ERR_DBG2
                                            }

                                        } else if ((SensorFreezeState & ((UINT32)SVC_VIN_ERR_FOV_STAT_FREZE << FovIdx)) > 0U) {

                                            // Configure osd raw freeze pattern
                                            ReqOsdPtn = SvcVinErrTask_BitGet(SVC_VIN_ERR_OSD_PTN_ID_RAW_FRZ);

                                            if ((pUnit->OsdPtnSelectBits & ReqOsdPtn) != ReqOsdPtn) {

                                                pUnit->OsdPtnSelectBits = ReqOsdPtn;

                                                // Enable osd draw function
                                                pUnit->State |= (UINT32)(SVC_VIN_ERR_OSD_UNIT_DISPLAY << (VoutIdx << 3U));
                                                SvcVinErrOsdCtrl.OsdInfo[VoutIdx].DrawUpd = 1U;

                                                PRN_VIN_ERR_LOG "VinID(%s%d%s) SerDesIdx(%s%d%s) FovID(%s%d%s) raw freeze. Enable VoutID(%s%d%s) osd. %s0x%08x%s"
                                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_ARG_UINT32 pFovInfo->SrcVinID         PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_ARG_UINT32 pFovInfo->SrcSerDesIdx     PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_ARG_UINT32 FovIdx                     PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_ARG_UINT32 VoutIdx                    PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0 PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_ARG_UINT32 pUnit->State               PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                                PRN_VIN_ERR_DBG2
                                            }

                                        } else {

                                            // Clear osd error pattern
                                            pUnit->OsdPtnSelectBits = 0U;

                                            if ((pFovInfo->State & SVC_VIN_ERR_FOV_VOUT_DELAY) > 0U) {

                                                // Configure osd image delay pattern
                                                ReqOsdPtn = SvcVinErrTask_BitGet(SVC_VIN_ERR_OSD_PTN_ID_VOUT_LTN);

                                                if ((pUnit->OsdPtnSelectBits & ReqOsdPtn) != ReqOsdPtn) {

                                                    pUnit->OsdPtnSelectBits |= ReqOsdPtn;

                                                    // Enable osd draw function
                                                    pUnit->State |= (UINT32)(SVC_VIN_ERR_OSD_UNIT_DISPLAY << (VoutIdx << 3U));
                                                    SvcVinErrOsdCtrl.OsdInfo[VoutIdx].DrawUpd = 1U;

                                                    PRN_VIN_ERR_LOG "VinID(%s%d%s) FovID(%s%d%s) vout delay. Enable VoutID(%s%d%s) osd. %s0x%08x%s"
                                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_UINT32 pFovInfo->SrcVinID         PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_UINT32 FovIdx                     PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_UINT32 VoutIdx                    PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0 PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_UINT32 pUnit->State               PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_DBG3
                                                }
                                            } else if ((pUnit->OsdPtnSelectBits == 0U) && ((pFovInfo->State & SVC_VIN_ERR_FOV_DELAY) > 0U)) {

                                                // Configure osd image delay pattern
                                                ReqOsdPtn = SvcVinErrTask_BitGet(SVC_VIN_ERR_OSD_PTN_ID_FOV_LTN);

                                                if ((pUnit->OsdPtnSelectBits & ReqOsdPtn) != ReqOsdPtn) {

                                                    pUnit->OsdPtnSelectBits |= ReqOsdPtn;

                                                    // Enable osd draw function
                                                    pUnit->State |= (UINT32)(SVC_VIN_ERR_OSD_UNIT_DISPLAY << (VoutIdx << 3U));
                                                    SvcVinErrOsdCtrl.OsdInfo[VoutIdx].DrawUpd = 1U;

                                                    PRN_VIN_ERR_LOG "VinID(%s%d%s) FovID(%s%d%s) fov delay. Enable VoutID(%s%d%s) osd. %s0x%08x%s"
                                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_UINT32 pFovInfo->SrcVinID         PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_UINT32 FovIdx                     PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_UINT32 VoutIdx                    PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0 PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_UINT32 pUnit->State               PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_DBG3
                                                }

                                            } else {

                                                // Clear osd error pattern
                                                pUnit->OsdPtnSelectBits = 0U;

                                                // Disable osd draw function
                                                if ((pUnit->State & (UINT32)(SVC_VIN_ERR_OSD_UNIT_DISPLAY << (VoutIdx << 3U))) > 0U) {
                                                    pUnit->State &= ~((UINT32)(SVC_VIN_ERR_OSD_UNIT_DISPLAY << (VoutIdx << 3U)));
                                                    SvcVinErrOsdCtrl.OsdInfo[VoutIdx].DrawUpd = 1U;

                                                    PRN_VIN_ERR_LOG "VinID(%s%d%s) SerDesIdx(%s%d%s) FovID(%s%d%s). Disable VoutID(%s%d%s) osd timeout. %s0x%08x%s"
                                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_UINT32 pFovInfo->SrcVinID         PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_UINT32 pFovInfo->SrcSerDesIdx     PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_UINT32 FovIdx                     PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_UINT32 VoutIdx                    PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0 PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_UINT32 pUnit->State               PRN_VIN_ERR_ARG_POST
                                                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                                                    PRN_VIN_ERR_DBG2
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                SvcVinErrTask_MutexGive(&(SvcVinErrOsdCtrl.Mutex));
            }

            AmbaMisra_TouchUnused(pFovInfo);
        }
    }

    AmbaMisra_TouchUnused(&RetVal);
}

static void SvcVinErrTask_OsdInfo(void)
{
    UINT32 VoutIdx, UnitIdx, PtnIdx;
    const void *pTmp;

    PRN_VIN_ERR_LOG " " PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "  ---- %sOsd Info%s ----"
        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0 PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
    PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "  Osd Vout SelectBits : 0x%x"
        PRN_VIN_ERR_ARG_UINT32 SvcVinErrOsdCtrl.SelectBits PRN_VIN_ERR_ARG_POST
    PRN_VIN_ERR_API

    PRN_VIN_ERR_LOG "    VoutID | VoutWidth | VoutHeight | OsdWidth | OsdHeight | GUI Level | Draw Func" PRN_VIN_ERR_API
    for (VoutIdx = 0U; VoutIdx < AMBA_DSP_MAX_VOUT_NUM; VoutIdx ++) {
        if ((SvcVinErrOsdCtrl.SelectBits & SvcVinErrTask_BitGet(VoutIdx)) > 0U) {

            AmbaMisra_TypeCast(&(pTmp), &(SvcVinErrOsdCtrl.OsdInfo[VoutIdx].pDrawFunc));

            PRN_VIN_ERR_LOG "        %02d |    %04d   |     %04d   |   %04d   |    %04d   |     %02d    | %p"
                PRN_VIN_ERR_ARG_UINT32 VoutIdx                                      PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 SvcVinErrOsdCtrl.OsdInfo[VoutIdx].VoutWidth  PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 SvcVinErrOsdCtrl.OsdInfo[VoutIdx].VoutHeight PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 SvcVinErrOsdCtrl.OsdInfo[VoutIdx].OsdWidth   PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 SvcVinErrOsdCtrl.OsdInfo[VoutIdx].OsdHeight  PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 SvcVinErrOsdCtrl.OsdInfo[VoutIdx].DrawLevel  PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CPOINT pTmp                                         PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API
        }
    }

    PRN_VIN_ERR_LOG "  ---- Osd Pattern Info ----" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "    PtnID | Length | String" PRN_VIN_ERR_API
    for (PtnIdx = 0U; PtnIdx < SVC_VIN_ERR_OSD_PTN_NUM; PtnIdx ++) {

        PRN_VIN_ERR_LOG "       %02d |   %04d | %s"
            PRN_VIN_ERR_ARG_UINT32 SvcVinErrOsdCtrl.pOsdPtnInfo[PtnIdx].PtnID     PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 SvcVinErrOsdCtrl.pOsdPtnInfo[PtnIdx].StrPtnLen PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SvcVinErrOsdCtrl.pOsdPtnInfo[PtnIdx].StrPtn    PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_API

    }

    PRN_VIN_ERR_LOG "  ---- Osd Units ----" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "    NumUnits : %d"
        PRN_VIN_ERR_ARG_UINT32 SvcVinErrOsdCtrl.NumUnits PRN_VIN_ERR_ARG_POST
    PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "    FovId |      State |  VoutBits  | PtnBits" PRN_VIN_ERR_API
    for (UnitIdx = 0U; UnitIdx < SvcVinErrOsdCtrl.NumUnits; UnitIdx ++) {
        PRN_VIN_ERR_LOG "       %02d | 0x%08x | 0x%08x | 0x%08x"
            PRN_VIN_ERR_ARG_UINT32 UnitIdx                                            PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 SvcVinErrOsdCtrl.FovUnit[UnitIdx].State            PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 SvcVinErrOsdCtrl.FovUnit[UnitIdx].VoutSelectBits   PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 SvcVinErrOsdCtrl.FovUnit[UnitIdx].OsdPtnSelectBits PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_API
    }

    for (VoutIdx = 0U; VoutIdx < AMBA_DSP_MAX_VOUT_NUM; VoutIdx ++) {
        if ((SvcVinErrOsdCtrl.SelectBits & SvcVinErrTask_BitGet(VoutIdx)) > 0U) {
            PRN_VIN_ERR_LOG "  ---- Osd Units VoutID(%d) Draw Window ----"
                PRN_VIN_ERR_ARG_UINT32 VoutIdx PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API
            PRN_VIN_ERR_LOG "    FovId | StartX | StartY | Width | Height | PtnIdx | StartX | StartY | Width | Height | FontSize" PRN_VIN_ERR_API
            for (UnitIdx = 0U; UnitIdx < SvcVinErrOsdCtrl.NumUnits; UnitIdx ++) {
                PRN_VIN_ERR_LOG "       %02d |   %04d |   %04d |  %04d |   %04d"
                    PRN_VIN_ERR_ARG_UINT32 UnitIdx                                                   PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrOsdCtrl.FovUnit[UnitIdx].DrawWin[VoutIdx].StartX PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrOsdCtrl.FovUnit[UnitIdx].DrawWin[VoutIdx].StartY PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrOsdCtrl.FovUnit[UnitIdx].DrawWin[VoutIdx].Width  PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrOsdCtrl.FovUnit[UnitIdx].DrawWin[VoutIdx].Height PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_API

                for (PtnIdx = 0U; PtnIdx < SvcVinErrOsdCtrl.NumPtnInfos; PtnIdx ++) {
                    PRN_VIN_ERR_LOG "                                                   %02d |   %04d |   %04d |  %04d |   %04d | %02d"
                        PRN_VIN_ERR_ARG_UINT32 PtnIdx                                                               PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 SvcVinErrOsdCtrl.FovUnit[UnitIdx].DrawStrWin[VoutIdx][PtnIdx].StartX PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 SvcVinErrOsdCtrl.FovUnit[UnitIdx].DrawStrWin[VoutIdx][PtnIdx].StartY PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 SvcVinErrOsdCtrl.FovUnit[UnitIdx].DrawStrWin[VoutIdx][PtnIdx].Width  PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 SvcVinErrOsdCtrl.FovUnit[UnitIdx].DrawStrWin[VoutIdx][PtnIdx].Height PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 SvcVinErrOsdCtrl.FovUnit[UnitIdx].DrawFontSize[VoutIdx][PtnIdx]      PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_API
                }
            }
        }
    }

#ifndef CONFIG_SVC_LVGL_USED
    PRN_VIN_ERR_LOG "  ---- Osd Base Font Width(%d + %d), Height(%d)"
        PRN_VIN_ERR_ARG_UINT32 SVC_OSD_CHAR_WIDTH  PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_ARG_UINT32 SVC_OSD_CHAR_GAP    PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_ARG_UINT32 SVC_OSD_CHAR_HEIGHT PRN_VIN_ERR_ARG_POST
    PRN_VIN_ERR_API
#else
    {
        UINT32 FontSize, FontWidth, FontHeight;

        PRN_VIN_ERR_LOG "  ---- Osd Font Size ----" PRN_VIN_ERR_API
        PRN_VIN_ERR_LOG "    Font | Width | Height" PRN_VIN_ERR_API

        for (FontSize = 3U; FontSize > 0U; FontSize --) {

            FontWidth = 0U; FontHeight = 0U;
            if (0U == SvcOsd_GetFontRes(FontSize, &FontWidth, &FontHeight)) {
                PRN_VIN_ERR_LOG "     %02d  |  %04d |   %04d"
                    PRN_VIN_ERR_ARG_UINT32 FontSize   PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 FontWidth  PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 FontHeight PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_API
            }
        }
    }
#endif
}

static void SvcvinErrTask_OsdDrawStrCfg(UINT32 VoutID, const SVC_GUI_CANVAS_s *pDrawWin, const SVC_VIN_ERR_OSD_PTN_INFO_s *pPtnInfo,
                                        SVC_GUI_CANVAS_s *pStrWin, UINT32 *pFontSize)
{
    if ((VoutID < AMBA_DSP_MAX_VOUT_NUM) &&
        (pDrawWin != NULL) &&
        (pPtnInfo != NULL) &&
        (pStrWin != NULL) &&
        (pFontSize != NULL)) {
        UINT32 GapX = 1U, GapY = 1U;

#ifndef CONFIG_SVC_LVGL_USED
        UINT32 PtnWidth  = pPtnInfo->StrPtnLen * ( SVC_OSD_CHAR_WIDTH + SVC_OSD_CHAR_GAP );
        UINT32 PtnHeight = SVC_OSD_CHAR_HEIGHT;

        pStrWin->Width  = (pDrawWin->Width * 90U) / 100U;

        *pFontSize = pStrWin->Width / PtnWidth;
        pStrWin->Width  = (*pFontSize) * PtnWidth;
        pStrWin->Height = (*pFontSize) * PtnHeight;
#else
        UINT32 FontSize, FontWidth, FontHeight;

        pStrWin->Width  = (pDrawWin->Width * 80U) / 100U;

        *pFontSize = 0U;
        for (FontSize = 3U; FontSize > 0U; FontSize --) {
            FontWidth = 0U; FontHeight = 0U;
            if (0U == SvcOsd_GetFontRes(FontSize, &FontWidth, &FontHeight)) {
                if ((FontWidth > 0U) && (FontHeight > 0U)) {
                    if (pStrWin->Width >= (FontWidth * pPtnInfo->StrPtnLen)) {
                        *pFontSize = FontSize;
                        break;
                    }
                }
            }
        }

        if (*pFontSize == 0U) {
            *pFontSize = 1U;
        }

        pStrWin->Width  = FontWidth * pPtnInfo->StrPtnLen;
        pStrWin->Height = FontHeight;
#endif

        switch(pPtnInfo->AlignType) {
        case SVC_VIN_ERR_OSD_PTN_ALIGN_TL:
            pStrWin->StartX = pDrawWin->StartX + GapX;
            pStrWin->StartY = pDrawWin->StartY + GapY;
            break;
        case SVC_VIN_ERR_OSD_PTN_ALIGN_TC:
            pStrWin->StartX = pDrawWin->StartX + ( ( pDrawWin->Width  - pStrWin->Width  ) / 2U ) + GapX;
            pStrWin->StartY = pDrawWin->StartY + GapY;
            break;
        case SVC_VIN_ERR_OSD_PTN_ALIGN_TR:
            pStrWin->StartX = pDrawWin->StartX + ( ( pDrawWin->Width  - pStrWin->Width  ) - GapX );
            pStrWin->StartY = pDrawWin->StartY + GapY;
            break;

        case SVC_VIN_ERR_OSD_PTN_ALIGN_CL:
            pStrWin->StartX = pDrawWin->StartX + GapX;
            pStrWin->StartY = pDrawWin->StartY + ( ( pDrawWin->Height - pStrWin->Height ) / 2U );
            break;
        case SVC_VIN_ERR_OSD_PTN_ALIGN_CC:
            pStrWin->StartX = pDrawWin->StartX + ( ( pDrawWin->Width  - pStrWin->Width  ) / 2U );
            pStrWin->StartY = pDrawWin->StartY + ( ( pDrawWin->Height - pStrWin->Height ) / 2U );
            break;
        case SVC_VIN_ERR_OSD_PTN_ALIGN_CR:
            pStrWin->StartX = pDrawWin->StartX + ( ( pDrawWin->Width  - pStrWin->Width  ) - GapX );
            pStrWin->StartY = pDrawWin->StartY + ( ( pDrawWin->Height - pStrWin->Height ) / 2U   );
            break;

        case SVC_VIN_ERR_OSD_PTN_ALIGN_BL:
            pStrWin->StartX = pDrawWin->StartX + GapX;
            pStrWin->StartY = pDrawWin->StartY + ( ( pDrawWin->Height - pStrWin->Height ) - GapY );
            break;
        case SVC_VIN_ERR_OSD_PTN_ALIGN_BC:
            pStrWin->StartX = pDrawWin->StartX + ( ( pDrawWin->Width  - pStrWin->Width  ) / 2U   );
            pStrWin->StartY = pDrawWin->StartY + ( ( pDrawWin->Height - pStrWin->Height ) - GapY );
            break;
        case SVC_VIN_ERR_OSD_PTN_ALIGN_BR:
            pStrWin->StartX = pDrawWin->StartX + ( ( pDrawWin->Width  - pStrWin->Width  ) - GapX );
            pStrWin->StartY = pDrawWin->StartY + ( ( pDrawWin->Height - pStrWin->Height ) - GapY );
            break;
        default :
            pStrWin->StartX = pDrawWin->StartX + ( ( pDrawWin->Width  - pStrWin->Width  ) / 2U );
            pStrWin->StartY = pDrawWin->StartY + ( ( pDrawWin->Height - pStrWin->Height ) / 2U );
            break;
        }

    }
}

static void SvcVinErrTask_OsdDrawFunc(UINT32 VoutID, UINT32 Level)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_OSD_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to draw osd - create vin err osd ctrl first!" PRN_VIN_ERR_NG
    } else if (VoutID >= AMBA_DSP_MAX_VOUT_NUM) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to draw osd - invalid VoutID(%d)"
            PRN_VIN_ERR_ARG_UINT32 VoutID PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_NG
    } else {

        if ((SvcVinErrOsdCtrl.SelectBits & SvcVinErrTask_BitGet(VoutID)) == 0U) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to draw osd - invalid VoutID(%d)"
                PRN_VIN_ERR_ARG_UINT32 VoutID PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        } else if (Level != SvcVinErrOsdCtrl.OsdInfo[VoutID].DrawLevel) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to draw osd - invalid gui canvas level (%d) != (%d)"
                PRN_VIN_ERR_ARG_UINT32 Level                                      PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 SvcVinErrOsdCtrl.OsdInfo[VoutID].DrawLevel PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        } else {
            if (0U == SvcVinErrTask_MutexTake(&(SvcVinErrOsdCtrl.Mutex), AMBA_KAL_NO_WAIT)) {
                UINT32 FovIdx, PtnIdx;
                const SVC_VIN_ERR_OSD_UNIT_s *pUnit;

                for (FovIdx = 0U; FovIdx < SvcVinErrOsdCtrl.NumUnits; FovIdx ++) {
                    pUnit = &(SvcVinErrOsdCtrl.FovUnit[FovIdx]);

                    if (((pUnit->VoutSelectBits & SvcVinErrTask_BitGet(VoutID)) > 0U) &&
                        ((pUnit->State & (UINT32)(SVC_VIN_ERR_OSD_UNIT_ENABLE << (VoutID << 3U))) > 0U)) {

                        if ((pUnit->State & ((UINT32)(SVC_VIN_ERR_OSD_UNIT_DISPLAY | SVC_VIN_ERR_OSD_UNIT_DBG) << (VoutID << 3U))) > 0U) {

                            for (PtnIdx = 0U; PtnIdx < SVC_VIN_ERR_OSD_PTN_NUM; PtnIdx ++) {
                                if ((pUnit->OsdPtnSelectBits & SvcVinErrTask_BitGet(PtnIdx)) > 0U) {
                                    if (pUnit->DrawStrWin[VoutID][PtnIdx].Width > 0U) {

                                        if ((pUnit->State & (UINT32)(SVC_VIN_ERR_OSD_UNIT_DISPLAY << (VoutID << 3U))) > 0U) {

                                            PRN_VIN_ERR_LOG "Draw FovID(%d) PtnIdx(%d) to %d,%d %dx%d FontSize: %d"
                                                PRN_VIN_ERR_ARG_UINT32 FovIdx                                   PRN_VIN_ERR_ARG_POST
                                                PRN_VIN_ERR_ARG_UINT32 PtnIdx                                   PRN_VIN_ERR_ARG_POST
                                                PRN_VIN_ERR_ARG_UINT32 pUnit->DrawStrWin[VoutID][PtnIdx].StartX PRN_VIN_ERR_ARG_POST
                                                PRN_VIN_ERR_ARG_UINT32 pUnit->DrawStrWin[VoutID][PtnIdx].StartY PRN_VIN_ERR_ARG_POST
                                                PRN_VIN_ERR_ARG_UINT32 pUnit->DrawStrWin[VoutID][PtnIdx].Width  PRN_VIN_ERR_ARG_POST
                                                PRN_VIN_ERR_ARG_UINT32 pUnit->DrawStrWin[VoutID][PtnIdx].Height PRN_VIN_ERR_ARG_POST
                                                PRN_VIN_ERR_ARG_UINT32 pUnit->DrawFontSize[VoutID][PtnIdx]      PRN_VIN_ERR_ARG_POST
                                            PRN_VIN_ERR_DBG2

                                            PRetVal = SvcOsd_DrawString(VoutID,
                                                                        pUnit->DrawStrWin[VoutID][PtnIdx].StartX,
                                                                        pUnit->DrawStrWin[VoutID][PtnIdx].StartY,
                                                                        pUnit->DrawFontSize[VoutID][PtnIdx],
                                                                        SVC_VIN_ERR_OSD_STR_COLOR,
                                                                        SvcVinErrOsdCtrl.pOsdPtnInfo[PtnIdx].StrPtn); PRN_VIN_ERR_HDLR
                                        }

                                        if ((pUnit->State & (UINT32)(SVC_VIN_ERR_OSD_UNIT_DBG << (VoutID << 3U))) > 0U) {
                                            PRetVal = SvcOsd_DrawRect(VoutID,
                                                pUnit->DrawStrWin[VoutID][PtnIdx].StartX,
                                                pUnit->DrawStrWin[VoutID][PtnIdx].StartY,
                                                pUnit->DrawStrWin[VoutID][PtnIdx].StartX + pUnit->DrawStrWin[VoutID][PtnIdx].Width,
                                                pUnit->DrawStrWin[VoutID][PtnIdx].StartY + pUnit->DrawStrWin[VoutID][PtnIdx].Height,
                                                SVC_VIN_ERR_OSD_STR_COLOR,
                                                2U); PRN_VIN_ERR_HDLR
                                        }
                                    }
                                }
                            }
                        } else {
                            /* Do nothing */
                        }

                    }
                }

                SvcVinErrTask_MutexGive(&(SvcVinErrOsdCtrl.Mutex));
            }
        }
    }

    AmbaMisra_TouchUnused(&RetVal);
}

static void SvcVinErrTask_OsdUpdateFunc(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    AmbaMisra_TouchUnused(&Level);

    if (pUpdate != NULL) {
        if (SvcVinErrOsdCtrl.OsdInfo[VoutIdx].DrawUpd == 1U) {
            SvcVinErrOsdCtrl.OsdInfo[VoutIdx].DrawUpd = 0U;
            *pUpdate = 1U;
        } else {
            *pUpdate = 0U;
        }
    }
}

static void SvcVinErrTask_ShellStrToU32(const char *pString, UINT32 *pValue)
{
    UINT32 ErrCode = AmbaUtility_StringToUInt32(pString, pValue);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }
}

static UINT32 SvcVinErrTask_ShellDbgMsg(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount < 3U) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to proc 'debug' cmd - invalid arg count!" PRN_VIN_ERR_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to proc 'debug' cmd - input arg should not null!" PRN_VIN_ERR_NG
    } else {
        UINT32 Enable = 0U, VinID = 0xFFFFFFFFU;

        SvcVinErrTask_ShellStrToU32(pArgVector[2U], &VinID);
        SvcVinErrTask_ShellStrToU32(pArgVector[3U], &Enable);

        if (Enable == 999U) {
            if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_LOG_MEM) > 0U) {
                SvcVinErrCtrlFlg &= ~SVC_VIN_ERR_FLG_LOG_MEM;
                PRN_VIN_ERR_LOG "disable mem log" PRN_VIN_ERR_API
            } else {
                SvcVinErrCtrlFlg |= SVC_VIN_ERR_FLG_LOG_MEM;
                PRN_VIN_ERR_LOG "enable mem log" PRN_VIN_ERR_API
            }
        } else if (Enable == 98U) {
            if ((ArgCount >= 5U) && (VinID < AMBA_DSP_MAX_VIN_NUM)) {
                UINT32 FovFreezeSrc;
                SvcVinErrTask_ShellStrToU32(pArgVector[4U], &FovFreezeSrc);
                if ( FovFreezeSrc == SVC_VIN_ERR_FOV_FREZE_SRC_RAW ) {
                    SvcVinErrCtrl.ErrState[VinID].FovFreezeSrc = FovFreezeSrc;
                } else if ( FovFreezeSrc == SVC_VIN_ERR_FOV_FREZE_SRC_AAA ) {
                    SvcVinErrCtrl.ErrState[VinID].FovFreezeSrc = FovFreezeSrc;
                } else {
                    /* Do nothing */
                }

                PRN_VIN_ERR_LOG "Configure VinID(%d) raw freeze source %d"
                    PRN_VIN_ERR_ARG_UINT32 VinID                                      PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.ErrState[VinID].FovFreezeSrc PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_API
            }
        } else if (Enable == 96U) {

            if (ArgCount >= 7U) {
                UINT32 VoutID = 0xFFFFU, FovID = 0xFFFFU, PtnID = 0xFFFFU;
                SVC_VIN_ERR_OSD_UNIT_s *pUnit;

                SvcVinErrTask_ShellStrToU32(pArgVector[4U], &VoutID);
                SvcVinErrTask_ShellStrToU32(pArgVector[5U], &FovID);
                SvcVinErrTask_ShellStrToU32(pArgVector[6U], &PtnID);

                if (VoutID < AMBA_DSP_MAX_VOUT_NUM) {
                    if (PtnID < SVC_VIN_ERR_OSD_PTN_NUM) {
                        if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_OSD_INIT) > 0U) {

                            pUnit = &(SvcVinErrOsdCtrl.FovUnit[FovID]);

                            if ((pUnit->State & (UINT32)(SVC_VIN_ERR_OSD_UNIT_DBG << (VoutID << 3U))) > 0U) {
                                pUnit->State &= ~((UINT32)(SVC_VIN_ERR_OSD_UNIT_DBG << (VoutID << 3U)));
                                if ((pUnit->State & (UINT32)(SVC_VIN_ERR_OSD_UNIT_DISPLAY << (VoutID << 3U))) == 0U) {
                                    SvcVinErrOsdCtrl.FovUnit[FovID].OsdPtnSelectBits &= ~SvcVinErrTask_BitGet(PtnID);
                                }
                            } else {
                                SvcVinErrOsdCtrl.FovUnit[FovID].State |= (UINT32)(SVC_VIN_ERR_OSD_UNIT_DBG << (VoutID << 3U));
                                SvcVinErrOsdCtrl.FovUnit[FovID].OsdPtnSelectBits |= SvcVinErrTask_BitGet(PtnID);
                            }

                            SvcVinErrOsdCtrl.OsdInfo[VoutID].DrawUpd = 1U;
                        }
                    }
                }
            }

        } else {
            if (Enable > 0U) {
                if ((Enable & 0x2U) > 0U) {
                    SvcVinErrCtrlFlg |= SVC_VIN_ERR_FLG_LOG_DBG2;
                } else {
                    SvcVinErrCtrlFlg &= ~SVC_VIN_ERR_FLG_LOG_DBG2;
                }
                if ((Enable & 0x4U) > 0U) {
                    SvcVinErrCtrlFlg |= SVC_VIN_ERR_FLG_LOG_DBG3;
                } else {
                    SvcVinErrCtrlFlg &= ~SVC_VIN_ERR_FLG_LOG_DBG3;
                }
                if ((Enable & 0x8U) > 0U) {
                    SvcVinErrCtrlFlg |= SVC_VIN_ERR_FLG_LOG_DBG4;
                } else {
                    SvcVinErrCtrlFlg &= ~SVC_VIN_ERR_FLG_LOG_DBG4;
                }
                if ((Enable & 0x10U) > 0U) {
                    SvcVinErrCtrlFlg |= SVC_VIN_ERR_FLG_LOG_DBG5;
                } else {
                    SvcVinErrCtrlFlg &= ~SVC_VIN_ERR_FLG_LOG_DBG5;
                }

                SvcVinErrCtrlFlg |= SVC_VIN_ERR_FLG_LOG_DBG1;
            } else {
                SvcVinErrCtrlFlg &= ~SVC_VIN_ERR_FLG_LOG_DBG5;
                SvcVinErrCtrlFlg &= ~SVC_VIN_ERR_FLG_LOG_DBG4;
                SvcVinErrCtrlFlg &= ~SVC_VIN_ERR_FLG_LOG_DBG3;
                SvcVinErrCtrlFlg &= ~SVC_VIN_ERR_FLG_LOG_DBG2;
                SvcVinErrCtrlFlg &= ~SVC_VIN_ERR_FLG_LOG_DBG1;
            }

            PRN_VIN_ERR_LOG "Vin err control debug message - %s, b'%s%s%s%s%s"
                PRN_VIN_ERR_ARG_CSTR   ( (Enable > 0U) ? ( "enable" ) : ( "disable" ) )                     PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   ( ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_LOG_DBG5) > 0U) ? "1" : "0" ) PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   ( ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_LOG_DBG4) > 0U) ? "1" : "0" ) PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   ( ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_LOG_DBG3) > 0U) ? "1" : "0" ) PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   ( ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_LOG_DBG2) > 0U) ? "1" : "0" ) PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   ( ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_LOG_DBG1) > 0U) ? "1" : "0" ) PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API
        }
    }

    return RetVal;
}

static void SvcVinErrTask_ShellDbgMsgU(void)
{
    PRN_VIN_ERR_LOG "  %sdebug%s      : enable/disable debug message"
        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
    PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "  ------------------------------------------------------" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "     VinID : configure vin id" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "    Enable : 0, disable" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "             bit_0(1),  enable normal debug message" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "             bit_1(2),  enable raw freeze debug message" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "             bit_2(4),  enable vin-vout latency debug message" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "             bit_3(8),  enable fov latency debug message" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "             bit_4(10), enable msg query debug message (only support in non-monfrw case)" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG " " PRN_VIN_ERR_API
}

static UINT32 SvcVinErrTask_ShellVinRecover(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount < 3U) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to proc 'recover' cmd - invalid arg count!" PRN_VIN_ERR_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to proc 'recover' cmd - input arg should not null!" PRN_VIN_ERR_NG
    } else if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to proc 'recover' cmd - initi module first!" PRN_VIN_ERR_NG
    } else {
        UINT32 VinID = 0xFFFFFFFFU;

        SvcVinErrTask_ShellStrToU32(pArgVector[2U], &VinID);

        if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
            PRN_VIN_ERR_LOG "Fail to proc 'debug' cmd - invalid VinID(%d)!"
                PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        } else {
            SVC_VIN_ERR_RECOVER_CMD_t RecoverCmd;

            AmbaSvcWrap_MisraMemset(&RecoverCmd, 0, sizeof(RecoverCmd));
            RecoverCmd.VinSelectBits = SvcVinErrTask_BitGet(VinID);
            RecoverCmd.SerDesSelectBits = SvcVinErrCtrl.SerDesSelectBits[VinID];
            RecoverCmd.CmdID = SVC_VIN_ERR_RCV_CMD_VIN;

            /* Update vin state flag */
            PRetVal = AmbaKAL_EventFlagSet(&(SvcVinErrCtrl.ErrState[VinID].VinStateFlag), SVC_VIN_ERR_VIN_STATE_TOUT); PRN_VIN_ERR_HDLR
            PRetVal = AmbaKAL_EventFlagSet(&(SvcVinErrCtrl.ErrState[VinID].VinStateFlag), SVC_VIN_ERR_VIN_STATE_RECOVER); PRN_VIN_ERR_HDLR

            /* Send cmd */
            PRetVal = AmbaKAL_MsgQueueSend(&(SvcVinErrCtrl.RecoverTask.Que), &RecoverCmd, 5U);
            if (PRetVal != 0U) {
                PRN_VIN_ERR_LOG "Fail to prepare VinID(%d) recover command"
                    PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_NG
            }
        }

    }

    return RetVal;
}

static void SvcVinErrTask_ShellVinRecoverU(void)
{
    PRN_VIN_ERR_LOG "  %srecover%s    : trigger vin recover flow"
        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
    PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "  ------------------------------------------------------" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "     VinID : configure vin id" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG " " PRN_VIN_ERR_API
}

static UINT32 SvcVinErrTask_ShellLivReStart(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount < 2U) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to proc 'restart' cmd - invalid arg count!" PRN_VIN_ERR_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to proc 'restart' cmd - input arg should not null!" PRN_VIN_ERR_NG
    } else if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to proc 'restart' cmd - initi module first!" PRN_VIN_ERR_NG
    } else{
        UINT32 VinIdx;
        SVC_VIN_ERR_RECOVER_CMD_t RecoverCmd;

        AmbaSvcWrap_MisraMemset(&RecoverCmd, 0, sizeof(RecoverCmd));
        RecoverCmd.CmdID = SVC_VIN_ERR_RCV_CMD_LIV;

        for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
            if((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(VinIdx)) > 0U) {

                /* Update vin state flag */
                PRetVal = AmbaKAL_EventFlagSet(&(SvcVinErrCtrl.ErrState[VinIdx].VinStateFlag), SVC_VIN_ERR_VIN_STATE_RECOVER); PRN_VIN_ERR_HDLR
            }
        }

        /* Send cmd */
        PRetVal = AmbaKAL_MsgQueueSend(&(SvcVinErrCtrl.RecoverTask.Que), &RecoverCmd, 5U);
        if (PRetVal != 0U) {
            PRN_VIN_ERR_LOG "Fail to prepare liveview re-program command" PRN_VIN_ERR_NG
        }

    }

    return RetVal;
}

static void SvcVinErrTask_ShellLivReStartU(void)
{
    PRN_VIN_ERR_LOG "  %srestart%s    : re-program liveview"
        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
    PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG " " PRN_VIN_ERR_API
}

static UINT32 SvcVinErrTask_ShellInfo(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount < 2U) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to proc 'info' cmd - invalid arg count!" PRN_VIN_ERR_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to proc 'info' cmd - input arg should not null!" PRN_VIN_ERR_NG
    } else if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to proc 'info' cmd - initi module first!" PRN_VIN_ERR_NG
    } else {
        UINT32 InfoFlag = 0U;

        if (ArgCount >= 3U) {
            SvcVinErrTask_ShellStrToU32(pArgVector[2U], &InfoFlag);
        } else {
            InfoFlag = 0xFFFFFFFFU;
        }

        if ((InfoFlag & (0x1U)) > 0U) {
            SvcVinErrTask_Info();
        }

        if ((InfoFlag & (0x2U)) > 0U) {
            SvcVinErrTask_RcvCtrlInfo();
        }

        if ((InfoFlag & (0x4U)) > 0U) {
            SvcVinErrTask_RawFrzInfo();
        }

        if ((InfoFlag & (0x8U)) > 0U) {
            SvcVinErrTask_OsdInfo();
        }

        if ((InfoFlag & (0x10U)) > 0U) {
#ifndef CONFIG_BUILD_MONFRW_GRAPH
            SvcVinErrTask_MsgQryInfo();
#else
            PRN_VIN_ERR_LOG "InfoFlg: 0x10 only support in non-monfrw case" PRN_VIN_ERR_API
#endif
        }
    }

    return RetVal;
}

static void SvcVinErrTask_ShellInfoU(void)
{
    PRN_VIN_ERR_LOG "  %sinfo%s       : print vin err setting"
        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
    PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "  ------------------------------------------------------" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "   InfoFlg : print module flag" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "             default is print all module" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "             bit 0 | b'00000001 | print vin err control" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "             bit 1 | b'00000010 | print recover control" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "             bit 2 | b'00000100 | print raw freeze" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "             bit 3 | b'00001000 | print osd info" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "             bit 4 | b'00010000 | print msg qury info (only support non-monfrw case)" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG " " PRN_VIN_ERR_API
}

static UINT32 SvcVinErrTask_ShellMCtrl(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount < 4U) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to proc 'mctrl' cmd - invalid arg count!" PRN_VIN_ERR_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to proc 'mctrl' cmd - input arg should not null!" PRN_VIN_ERR_NG
    } else if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to proc 'mctrl' cmd - initi module first!" PRN_VIN_ERR_NG
    } else {
        UINT32 VinID = 0xFFFFFFFFU;

        SvcVinErrTask_ShellStrToU32(pArgVector[2U], &VinID);

        if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
            PRN_VIN_ERR_LOG "Fail to proc 'mctrl' cmd - invalid VinID(%d)!"
                PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        } else {
            UINT32 Flag = 0U;
            if (0 == SvcWrap_strcmp(pArgVector[3U], "on")) {
                Flag = 0xFFFFFFFFU;
                SvcVinErrTask_RcvFlagClear(Flag);
            } else if (0 == SvcWrap_strcmp(pArgVector[3U], "off")) {
                Flag = 0xFFFFFFFFU;
                SvcVinErrTask_RcvFlagSet(Flag);
            } else if (0 == SvcWrap_strcmp(pArgVector[3U], "qry")) {
                if (SVC_OK == SvcVinErrTask_RcvFlagQry(&Flag)) {
                    UINT32 ProcIdx;

                    PRN_VIN_ERR_LOG " " PRN_VIN_ERR_API
                    PRN_VIN_ERR_LOG "--- Recover Control Flag (0x%08x) ---"
                        PRN_VIN_ERR_ARG_UINT32 Flag  PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_API

                    PRN_VIN_ERR_LOG "  %s Rcv Seq %s %s Proc Name      %s %s Proc ID %s %s Proc Flg   %s"
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_5 PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_5 PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_5 PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_5 PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_API

                    for (ProcIdx = 0U; ProcIdx < SVC_VIN_ERR_RCV_PROC_NUM; ProcIdx ++) {
                        if (SvcVinErrRcvSubProc[ProcIdx].Enable > 0U) {

                            if ((Flag & SvcVinErrRcvSubProc[ProcIdx].ProcFlg) == 0U) {
                                PRN_VIN_ERR_LOG "      %02d     %s      %02d      0x%08x"
                                    PRN_VIN_ERR_ARG_UINT32 ProcIdx                               PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_CSTR   SvcVinErrRcvSubProc[ProcIdx].ProcName PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrRcvSubProc[ProcIdx].ProcID   PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrRcvSubProc[ProcIdx].ProcFlg  PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_API
                            } else {

                                PRN_VIN_ERR_LOG "      %02d     %s%s%s      %02d      0x%08x"
                                    PRN_VIN_ERR_ARG_UINT32 ProcIdx                               PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_6            PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_CSTR   SvcVinErrRcvSubProc[ProcIdx].ProcName PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END                PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrRcvSubProc[ProcIdx].ProcID   PRN_VIN_ERR_ARG_POST
                                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrRcvSubProc[ProcIdx].ProcFlg  PRN_VIN_ERR_ARG_POST
                                PRN_VIN_ERR_API
                            }

                        }
                    }
                }
            } else if (0 == SvcWrap_strcmp(pArgVector[3U], "rcv_chk")) {
                UINT32 Enable;

                if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_RCV_CHK_OFF) > 0U) {
                    Enable = 1U;
                } else {
                    Enable = 0U;
                }

                if (ArgCount >= 5U) {
                    SvcVinErrTask_ShellStrToU32(pArgVector[4U], &Enable);
                }

                if (Enable > 0U) {
                    SvcVinErrCtrlFlg &= ~SVC_VIN_ERR_FLG_RCV_CHK_OFF;
                } else {
                    SvcVinErrCtrlFlg |= SVC_VIN_ERR_FLG_RCV_CHK_OFF;
                }

                PRN_VIN_ERR_LOG "%s recover check mechanism"
                    PRN_VIN_ERR_ARG_CSTR   (Enable > 0U)?"Enable":"Disable" PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_API

            } else if (0 == SvcWrap_strcmp(pArgVector[3U], "ser_rst")) {
                extern UINT32 AmbaUserGPIO_SerdesResetCtrl(UINT32 VinID, UINT32 PinLevel);
                extern UINT32 AmbaUserGPIO_SerdesPowerCtrl(UINT32 VinID, UINT32 PinLevel);

                if (ArgCount >= 5U) {
                    UINT32 Enable = 0U;

                    SvcVinErrTask_ShellStrToU32(pArgVector[4U], &Enable);

                    if (Enable > 0U) {
                        PRN_VIN_ERR_LOG "Enable VinID(%d) SerDes Power"
                            PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_API
                        PRetVal = AmbaUserGPIO_SerdesPowerCtrl(VinID, 1U);  PRN_VIN_ERR_HDLR
                        PRetVal = AmbaKAL_TaskSleep(100);                   PRN_VIN_ERR_HDLR
                        PRetVal = AmbaUserGPIO_SerdesPowerCtrl(VinID, 2U);  PRN_VIN_ERR_HDLR
                        PRetVal = AmbaKAL_TaskSleep(100);                   PRN_VIN_ERR_HDLR

                        PRetVal = AmbaUserGPIO_SerdesResetCtrl(VinID, 2U);  PRN_VIN_ERR_HDLR
                    } else {
                        PRN_VIN_ERR_LOG "Disable VinID(%d) SerDes Power"
                            PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_API
                        PRetVal = AmbaUserGPIO_SerdesResetCtrl(VinID, 1U);  PRN_VIN_ERR_HDLR
                    }
                }

            } else {
                SvcVinErrTask_ShellStrToU32(pArgVector[3U], &Flag);
                SvcVinErrTask_RcvFlagSet(Flag);
            }
        }

    }

    return RetVal;
}

static void SvcVinErrTask_ShellMCtrlU(void)
{
    UINT32 ProcIdx, PrnHdr = 0U;

    PRN_VIN_ERR_LOG "  %smctrl%s      : manual control"
        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
    PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "  ------------------------------------------------------" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "     VinID : vin id" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "      Ctrl : on      -> enable manual control" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "             off     -> disable manual control" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "             rcv_chk -> enable(1)/disable(0) recover control" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "             ser_rst -> enable(1)/disable(0) serdes power" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "             qry     -> query recover control flag" PRN_VIN_ERR_API
    for (ProcIdx = 0U; ProcIdx < SVC_VIN_ERR_RCV_PROC_NUM; ProcIdx ++) {
        if (SvcVinErrRcvSubProc[ProcIdx].Enable > 0U) {
            if (PrnHdr == 0U) {
                PRN_VIN_ERR_LOG "             flag -> 0x%08x, ProcID( %02d ) - %s"
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrRcvSubProc[ProcIdx].ProcFlg  PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrRcvSubProc[ProcIdx].ProcID   PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SvcVinErrRcvSubProc[ProcIdx].ProcName PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_API
            } else {
                PRN_VIN_ERR_LOG "                     0x%08x, ProcID( %02d ) - %s"
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrRcvSubProc[ProcIdx].ProcFlg  PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_UINT32 SvcVinErrRcvSubProc[ProcIdx].ProcID   PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_ARG_CSTR   SvcVinErrRcvSubProc[ProcIdx].ProcName PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_API
            }

            PrnHdr = 1U;
        }
    }
    PRN_VIN_ERR_LOG " " PRN_VIN_ERR_API
}

static UINT32 SvcVinErrTask_ShellVoutLatency(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount < 4U) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to proc 'latency' cmd - invalid arg count!" PRN_VIN_ERR_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to proc 'latency' cmd - invalid arg vector!" PRN_VIN_ERR_NG
    } else if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to proc 'latency' cmd - init module first!" PRN_VIN_ERR_NG
    } else {
        UINT32 FovID     = 0xFFFFFFFFU;
        UINT32 Threshold = 0;

        SvcVinErrTask_ShellStrToU32(pArgVector[2U], &FovID);
        SvcVinErrTask_ShellStrToU32(pArgVector[3U], &Threshold);

        if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
            PRN_VIN_ERR_LOG "Fail to proc 'latency' cmd - invalid FovID(%d)!"
                PRN_VIN_ERR_ARG_UINT32 FovID PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        } else {
            SvcVinErrCtrl.FovInfo[FovID].VoutLatencyThreshold = Threshold;

            PRN_VIN_ERR_LOG "Configure FovID(%d) vout latency threshold to %d"
                PRN_VIN_ERR_ARG_UINT32 FovID                                         PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.FovInfo[FovID].VoutLatencyThreshold PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API
        }
    }

    return RetVal;
}

static void SvcVinErrTask_ShellVoutLatencyU(void)
{
    PRN_VIN_ERR_LOG "  %slatency%s    : configure each latency threshold ( the latency between vin and vout )"
        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
    PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "  ------------------------------------------------------" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "     FovID : configure fov id" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG " threshold : configure latency threshold" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG " " PRN_VIN_ERR_API
}

static UINT32 SvcVinErrTask_ShellFovLatency(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount < 4U) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to proc 'fov_latency' cmd - invalid arg count!" PRN_VIN_ERR_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to proc 'fov_latency' cmd - invalid arg vector!" PRN_VIN_ERR_NG
    } else if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to proc 'fov_latency' cmd - init module first!" PRN_VIN_ERR_NG
    } else {
        UINT32 FovID     = 0xFFFFFFFFU;
        UINT32 Threshold = 0;

        SvcVinErrTask_ShellStrToU32(pArgVector[2U], &FovID);
        SvcVinErrTask_ShellStrToU32(pArgVector[3U], &Threshold);

        if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
            PRN_VIN_ERR_LOG "Fail to proc 'fov_latency' cmd - invalid FovID(%d)!"
                PRN_VIN_ERR_ARG_UINT32 FovID PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_NG
        } else {
            SvcVinErrCtrl.FovInfo[FovID].FovLatencyThreshold = Threshold;

            PRN_VIN_ERR_LOG "Configure FovID(%d) latency threshold to %d"
                PRN_VIN_ERR_ARG_UINT32 FovID                                            PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.FovInfo[FovID].FovLatencyThreshold PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API
        }
    }

    return RetVal;
}
static void SvcVinErrTask_ShellFovLatencyU(void)
{
    PRN_VIN_ERR_LOG "  %sfov_latency%s: configure each fov latency threshold ( the latency between vin and fov )"
        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
    PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "  ------------------------------------------------------" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "     FovID : configure fov id" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG " threshold : configure latency threshold" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG " " PRN_VIN_ERR_API
}

static UINT32 SvcVinErrTask_ShellRawFrz(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_NG;

    if (ArgCount < 2U) {
        PRN_VIN_ERR_LOG "Fail to proc 'raw_freeze' cmd - invalid arg count!" PRN_VIN_ERR_NG
    } else {
        UINT32 ShellIdx;
        UINT32 ShellCount = (UINT32)(sizeof(SvcVinErrShellRawFrz)) / (UINT32)(sizeof(SvcVinErrShellRawFrz[0]));

        for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
            if ((SvcVinErrShellRawFrz[ShellIdx].pFunc != NULL) && (SvcVinErrShellRawFrz[ShellIdx].Enable > 0U)) {
                if (0 == SvcWrap_strcmp(pArgVector[2U], SvcVinErrShellRawFrz[ShellIdx].ShellCmdName)) {
                    if (SVC_OK != (SvcVinErrShellRawFrz[ShellIdx].pFunc)(ArgCount, pArgVector)) {
                        if (SvcVinErrShellRawFrz[ShellIdx].pUsage != NULL) {
                            (SvcVinErrShellRawFrz[ShellIdx].pUsage)();
                        }
                    }
                    RetVal = SVC_OK;
                    break;
                }
            }
        }
    }

    return RetVal;
}

static void SvcVinErrTask_ShellRawFrzU(void)
{
    UINT32 ShellIdx;
    UINT32 ShellCount = (UINT32)(sizeof(SvcVinErrShellRawFrz)) / (UINT32)(sizeof(SvcVinErrShellRawFrz[0]));

    PRN_VIN_ERR_LOG " " PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "  %sraw_freeze%s : raw image freeze command"
        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
    PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "  ------------------------------------------------------" PRN_VIN_ERR_API
    for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
        if ((SvcVinErrShellRawFrz[ShellIdx].pFunc != NULL) && (SvcVinErrShellRawFrz[ShellIdx].Enable > 0U)) {
            if (SvcVinErrShellRawFrz[ShellIdx].pUsage == NULL) {
                PRN_VIN_ERR_LOG "    %s"
                    PRN_VIN_ERR_ARG_CSTR   SvcVinErrShellRawFrz[ShellIdx].ShellCmdName PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_API
            } else {
                (SvcVinErrShellRawFrz[ShellIdx].pUsage)();
            }
        }
    }

    PRN_VIN_ERR_LOG " " PRN_VIN_ERR_API
}

static UINT32 SvcVinErrTask_ShellRawFrzEna(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount < 4U) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to proc 'raw_freeze'->'enable' cmd - invalid arg count!" PRN_VIN_ERR_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to proc 'raw_freeze'->'enable' cmd - input arg should not null!" PRN_VIN_ERR_NG
    } else {
        if (ArgCount > 4U) {
            UINT32 FovID = 0xFFU;
            UINT32 Enable = 0U;

            if (0 == SvcWrap_strcmp(pArgVector[3U], "on")) {
                Enable = 1U;
            } else if (0 == SvcWrap_strcmp(pArgVector[3U], "off")) {
                Enable = 0U;
            } else {
                // misra-c
            }

            SvcVinErrTask_ShellStrToU32(pArgVector[4U], &FovID);
            SvcVinErrTask_RawFrzEnable(FovID, Enable);

        } else {
            if (0 == SvcWrap_strcmp(pArgVector[3U], "on")) {
                SvcVinErrCtrlFlg &= ~SVC_VIN_ERR_FLG_RAW_FRZ_OFF;
            } else if (0 == SvcWrap_strcmp(pArgVector[3U], "off")) {
                SvcVinErrCtrlFlg |= SVC_VIN_ERR_FLG_RAW_FRZ_OFF;
            } else {
                // misra-c
            }

            PRN_VIN_ERR_LOG "%s vin err raw freeze mechanism!"
                PRN_VIN_ERR_ARG_CSTR   (((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_RAW_FRZ_OFF) == 0U)?"Enable":"Disable") PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API
        }
    }

    return RetVal;
}

static void SvcVinErrTask_ShellRawFrzEnaU(void)
{
    PRN_VIN_ERR_LOG "    %senable%s   : enable/disable raw image freeze detection"
        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_3 PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
    PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "  ------------------------------------------------------" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "      on/off : enable/disable raw freeze mechanism" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG " " PRN_VIN_ERR_API
}

static UINT32 SvcVinErrTask_ShellRawFrzInfo(UINT32 ArgCount, char * const *pArgVector)
{
    if (pArgVector != NULL) {
        SvcVinErrTask_RawFrzInfo();

        AmbaMisra_TouchUnused(&ArgCount);
    }

    return SVC_OK;
}

static void SvcVinErrTask_ShellRawFrzInfoU(void)
{
    PRN_VIN_ERR_LOG "    %sinfo%s     : print the raw freeze info"
        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_3 PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
    PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG " " PRN_VIN_ERR_API
}

static UINT32 SvcVinErrTask_ShellRawFrzRst(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount < 4U) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to proc 'raw_freeze'->'reset' cmd - invalid arg count!" PRN_VIN_ERR_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to proc 'raw_freeze'->'reset' cmd - input arg should not null!" PRN_VIN_ERR_NG
    } else {
        UINT32 FovID = 0xFFFFU;

        SvcVinErrTask_ShellStrToU32(pArgVector[3U], &FovID);
        PRetVal = SvcVinErrTask_RawFrzReset(FovID); PRN_VIN_ERR_HDLR
    }

    return RetVal;
}

static void SvcVinErrTask_ShellRawFrzRstU(void)
{
    PRN_VIN_ERR_LOG "    %sreset%s    : reset raw freeze error state by fov"
        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_3 PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
    PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "  ------------------------------------------------------" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "       FovID : requested fov_id" PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG " " PRN_VIN_ERR_API
}

static void SvcVinErrTask_ShellUsage(void)
{
    UINT32 ShellIdx;
    UINT32 ShellCount = (UINT32)(sizeof(SvcVinErrShellFunc)) / (UINT32)(sizeof(SvcVinErrShellFunc[0]));

    PRN_VIN_ERR_LOG " " PRN_VIN_ERR_API
    PRN_VIN_ERR_LOG "====== %sVIN Err Ctrl Command Usage%s ======"
        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_0 PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
    PRN_VIN_ERR_API
    for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
        if ((SvcVinErrShellFunc[ShellIdx].pFunc != NULL) && (SvcVinErrShellFunc[ShellIdx].Enable > 0U)) {
            if (SvcVinErrShellFunc[ShellIdx].pUsage == NULL) {
                PRN_VIN_ERR_LOG "  %s"
                    PRN_VIN_ERR_ARG_CSTR   SvcVinErrShellFunc[ShellIdx].ShellCmdName PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_API
            } else {
                (SvcVinErrShellFunc[ShellIdx].pUsage)();
            }
        }
    }

    PRN_VIN_ERR_LOG " " PRN_VIN_ERR_API
}

static void SvcVinErrTask_ShellEntryInit(void)
{
    if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_SHELL_INIT) == 0U) {
        UINT32 Cnt = 0U;

        AmbaSvcWrap_MisraMemset(SvcVinErrShellRawFrz, 0, sizeof(SvcVinErrShellRawFrz));
        SvcVinErrShellRawFrz[Cnt] = (SVC_VIN_ERR_SHELL_FUNC_s) { 1U, "enable", SvcVinErrTask_ShellRawFrzEna,  SvcVinErrTask_ShellRawFrzEnaU  }; Cnt++;
        SvcVinErrShellRawFrz[Cnt] = (SVC_VIN_ERR_SHELL_FUNC_s) { 1U, "info",   SvcVinErrTask_ShellRawFrzInfo, SvcVinErrTask_ShellRawFrzInfoU }; Cnt++;
        SvcVinErrShellRawFrz[Cnt] = (SVC_VIN_ERR_SHELL_FUNC_s) { 1U, "reset",  SvcVinErrTask_ShellRawFrzRst,  SvcVinErrTask_ShellRawFrzRstU  };

        Cnt = 0U;
        AmbaSvcWrap_MisraMemset(SvcVinErrShellFunc, 0, sizeof(SvcVinErrShellFunc));
        SvcVinErrShellFunc[Cnt] = (SVC_VIN_ERR_SHELL_FUNC_s) { 1U, "debug",      SvcVinErrTask_ShellDbgMsg,      SvcVinErrTask_ShellDbgMsgU      }; Cnt++;
        SvcVinErrShellFunc[Cnt] = (SVC_VIN_ERR_SHELL_FUNC_s) { 1U, "recover",    SvcVinErrTask_ShellVinRecover,  SvcVinErrTask_ShellVinRecoverU  }; Cnt++;
        SvcVinErrShellFunc[Cnt] = (SVC_VIN_ERR_SHELL_FUNC_s) { 1U, "restart",    SvcVinErrTask_ShellLivReStart,  SvcVinErrTask_ShellLivReStartU  }; Cnt++;
        SvcVinErrShellFunc[Cnt] = (SVC_VIN_ERR_SHELL_FUNC_s) { 1U, "info",       SvcVinErrTask_ShellInfo,        SvcVinErrTask_ShellInfoU        }; Cnt++;
        SvcVinErrShellFunc[Cnt] = (SVC_VIN_ERR_SHELL_FUNC_s) { 1U, "mctrl",      SvcVinErrTask_ShellMCtrl,       SvcVinErrTask_ShellMCtrlU       }; Cnt++;
        SvcVinErrShellFunc[Cnt] = (SVC_VIN_ERR_SHELL_FUNC_s) { 1U, "raw_freeze", SvcVinErrTask_ShellRawFrz,      SvcVinErrTask_ShellRawFrzU      }; Cnt++;
        SvcVinErrShellFunc[Cnt] = (SVC_VIN_ERR_SHELL_FUNC_s) { 1U, "latency",    SvcVinErrTask_ShellVoutLatency, SvcVinErrTask_ShellVoutLatencyU }; Cnt++;
        SvcVinErrShellFunc[Cnt] = (SVC_VIN_ERR_SHELL_FUNC_s) { 1U, "fov_latency",SvcVinErrTask_ShellFovLatency,  SvcVinErrTask_ShellFovLatencyU  };

        SvcVinErrCtrlFlg |= SVC_VIN_ERR_FLG_SHELL_INIT;
    }
}

static void SvcVinErrTask_ShellEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal = SVC_OK;

    SvcVinErrTask_ShellEntryInit();

    if(ArgCount == 0xCafeU) {
        UINT32 SIdx;

        PRN_VIN_ERR_LOG " " PRN_VIN_ERR_API
        PRN_VIN_ERR_LOG "==== %sShell Arguments Setting%s (%s%d%s) ===="
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_TITLE_1 PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_NUM     PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_UINT32 ArgCount                   PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END     PRN_VIN_ERR_ARG_POST
        PRN_VIN_ERR_API

        for (SIdx = 0U; SIdx < ArgCount; SIdx ++) {
            PRN_VIN_ERR_LOG "  pArgVector[%s%d%s] : %s%s%s"
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_NUM PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_UINT32 SIdx                   PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_STR PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   pArgVector[SIdx]       PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_ARG_CSTR   SVC_VIN_ERR_LOG_HL_END PRN_VIN_ERR_ARG_POST
            PRN_VIN_ERR_API
        }
    }

    if (ArgCount < 2U) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to proc vin err ctrl shell cmd - invalid arg count!" PRN_VIN_ERR_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to proc vin err ctrl shell cmd - invalid arg vector!" PRN_VIN_ERR_NG
    } else {
        UINT32 ShellIdx;
        UINT32 ShellCount = (UINT32)(sizeof(SvcVinErrShellFunc)) / (UINT32)(sizeof(SvcVinErrShellFunc[0]));

        for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
            if ((SvcVinErrShellFunc[ShellIdx].pFunc != NULL) && (SvcVinErrShellFunc[ShellIdx].Enable > 0U)) {
                if (0 == SvcWrap_strcmp(pArgVector[1U], SvcVinErrShellFunc[ShellIdx].ShellCmdName)) {
                    if (SVC_OK != (SvcVinErrShellFunc[ShellIdx].pFunc)(ArgCount, pArgVector)) {
                        if (SvcVinErrShellFunc[ShellIdx].pUsage != NULL) {
                            (SvcVinErrShellFunc[ShellIdx].pUsage)();
                        }
                    }
                    break;
                }
            }
        }
    }

    if (RetVal != SVC_OK) {
        SvcVinErrTask_ShellUsage();
        AmbaMisra_TouchUnused(&PrintFunc);
    }
}

static void SvcVinErrTask_CommandInstall(void)
{
    if ((SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_CMD_INSTALL) == 0U) {
        AMBA_SHELL_COMMAND_s SvcVinErrCmdApp;

        SvcVinErrCmdApp.pName    = "svc_vinerr";
        SvcVinErrCmdApp.MainFunc = SvcVinErrTask_ShellEntry;
        SvcVinErrCmdApp.pNext    = NULL;

        if (SHELL_ERR_SUCCESS != SvcCmd_CommandRegister(&SvcVinErrCmdApp)) {
            PRN_VIN_ERR_LOG "Fail to install svc vin err control command!" PRN_VIN_ERR_NG
        } else {
            SvcVinErrCtrlFlg |= SVC_VIN_ERR_FLG_CMD_INSTALL;
        }
    }
}

static UINT32 SvcVinErrTask_SerDesQryConfig(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    if (pResCfg == NULL) {
        RetVal = SVC_NG;
    } else {
        UINT32 Idx, VinNum = 0U, VinIDs[AMBA_DSP_MAX_VIN_NUM];

        AmbaSvcWrap_MisraMemset(VinIDs, 0, sizeof(VinIDs));
        PRetVal = SvcResCfg_GetVinIDs(VinIDs, &VinNum); PRN_VIN_ERR_HDLR

        for (Idx = 0U; Idx < VinNum; Idx ++) {
            SvcVinErrTask_SerDesPreBootChk(VinIDs[Idx]);
        }
    }

    return RetVal;
}

static UINT32 SvcVinErrTask_Config(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    if (pResCfg == NULL) {
        RetVal = SVC_NG;
    } else {
        UINT32 Idx, VinNum = 0U, VinIDs[AMBA_DSP_MAX_VIN_NUM];
        UINT32 SensorIdx, SensorIdxNum, SensorIdxs[AMBA_DSP_MAX_VIEWZONE_NUM], SerDesIdx;
        SVC_VIN_ERR_STATE_s *pState;
        SVC_LIV_INFO_s CurLivInfo;

        /* Configure statistic mask */
        SvcVinErrCtrl.ImgStatisticMask = 0xFFFFFFFFU;

        AmbaSvcWrap_MisraMemset(VinIDs, 0, sizeof(VinIDs));
        PRetVal = SvcResCfg_GetVinIDs(VinIDs, &VinNum); PRN_VIN_ERR_HDLR

        for (Idx = 0U; Idx < VinNum; Idx ++) {

            if (VinIDs[Idx] < SVC_VIN_ERR_MAX_SELECT_BITS) {

                /* Configure serdes select bits */
                SvcVinErrCtrl.VinSelectBits |= SvcVinErrTask_BitGet(VinIDs[Idx]);

                /* Get the sensor index array by vin id */
                SensorIdxNum = 0U;
                AmbaSvcWrap_MisraMemset(SensorIdxs, 0, sizeof(SensorIdxs));
                PRetVal = SvcResCfg_GetSensorIdxsInVinID(VinIDs[Idx], SensorIdxs, &SensorIdxNum); PRN_VIN_ERR_HDLR

                for (SensorIdx = 0U; SensorIdx < SensorIdxNum; SensorIdx ++) {
                    SerDesIdx = 0xCafeCafeU;
                    PRetVal = SvcResCfg_GetSerdesIdxOfSensorIdx(VinIDs[Idx], SensorIdxs[SensorIdx], &SerDesIdx); PRN_VIN_ERR_HDLR
                    if (SerDesIdx == 0xDEADBEAFU) {
                        SvcVinErrCtrl.SerDesSelectBits[VinIDs[Idx]] = 0x1U;
                    } else if (SerDesIdx >= SVC_VIN_ERR_MAX_SER_NUM) {
                        RetVal = SVC_NG;
                        PRN_VIN_ERR_LOG "Fail to create vin err control - invalid serdes index(%d) fail! VinID(%d) sensor index(%d)"
                            PRN_VIN_ERR_ARG_UINT32 SerDesIdx             PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 VinIDs[Idx]           PRN_VIN_ERR_ARG_POST
                            PRN_VIN_ERR_ARG_UINT32 SensorIdxs[SensorIdx] PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_NG
                    } else {
                        SvcVinErrCtrl.SerDesSelectBits[VinIDs[Idx]] |= SvcVinErrTask_BitGet(SerDesIdx);
                    }

                    if ((RetVal != SVC_OK) || (SerDesIdx == 0xDEADBEAFU)) {
                        break;
                    }
                }
            }

            /* Configure err state */
            if (RetVal == SVC_OK) {

                /* Create event flag */
                pState = &(SvcVinErrCtrl.ErrState[VinIDs[Idx]]);

                AmbaSvcWrap_MisraMemset(&(pState->VinStateFlag), 0, sizeof(AMBA_KAL_EVENT_FLAG_t));
                PRetVal = AmbaKAL_EventFlagCreate(&(pState->VinStateFlag), SvcVinErrCtrl.Name);
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    PRN_VIN_ERR_LOG "Fail to create vin err ctrl - create Vin(%d) state flag fail!"
                        PRN_VIN_ERR_ARG_UINT32 VinIDs[Idx] PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_NG
                } else {
                    PRetVal = AmbaKAL_EventFlagClear(&(pState->VinStateFlag), 0xFFFFFFFFU);               PRN_VIN_ERR_HDLR
                    PRetVal = AmbaKAL_EventFlagSet(&(pState->VinStateFlag), SVC_VIN_ERR_VIN_STATE_IMGFRW_SYNC); PRN_VIN_ERR_HDLR
                    PRetVal = AmbaKAL_EventFlagSet(&(pState->VinStateFlag), SVC_VIN_ERR_VIN_STATE_IMGFRW_AAA); PRN_VIN_ERR_HDLR

                    pState->LinkLockState = 0xFFFFFFFFU;
                    pState->LinkLockPreState = 0xFFFFFFFFU;
                    pState->VideoLockState = 0xFFFFFFFFU;
                    pState->VideoLockPreState = 0xFFFFFFFFU;
                    pState->LinkUpdEnable = SVC_VIN_ERR_LNK_UPD_ON;

                    PRN_VIN_ERR_LOG "Configure VinID(%d) serdes select bits 0x%x"
                        PRN_VIN_ERR_ARG_UINT32 VinIDs[Idx]                                 PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_ARG_UINT32 SvcVinErrCtrl.SerDesSelectBits[VinIDs[Idx]] PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_API
                }
            }

            if (RetVal != SVC_OK) {
                break;
            }
        }

        /* create vin liveview flag */
        AmbaSvcWrap_MisraMemset(&(SvcVinErrCtrl.VinLivFlag), 0, sizeof(AMBA_KAL_EVENT_FLAG_t));
        PRetVal = AmbaKAL_EventFlagCreate(&(SvcVinErrCtrl.VinLivFlag), SvcVinErrCtrl.Name);
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to create vin err ctrl - create vin liveview flag fail!" PRN_VIN_ERR_NG
        } else {
            PRetVal = AmbaKAL_EventFlagClear(&(SvcVinErrCtrl.VinLivFlag), 0xFFFFFFFFU); PRN_VIN_ERR_HDLR
        }

        /* Configure each fov info */
        {
            UINT32 FovSrc;
            UINT32 DispIdx;
            UINT32 VoutID;

            for (Idx = 0U; Idx < pResCfg->FovNum; Idx ++) {
                SvcVinErrCtrl.FovSelectBits |= SvcVinErrTask_BitGet(Idx);

                FovSrc = 0xFFU;
                PRetVal = SvcResCfg_GetFovSrc(Idx, &FovSrc);
                if ((FovSrc != SVC_VIN_SRC_SENSOR) && (FovSrc != SVC_VIN_SRC_YUV)) {
                    SvcVinErrCtrl.FovInfo[Idx].SrcVinID     = 0xdeadbeefU;
                    SvcVinErrCtrl.FovInfo[Idx].SrcSerDesIdx = 0xdeadbeefU;
                } else {
                    PRetVal = SvcResCfg_GetVinIDOfFovIdx(Idx, &(SvcVinErrCtrl.FovInfo[Idx].SrcVinID)); PRN_VIN_ERR_HDLR
                    PRetVal = SvcResCfg_GetSerdesIdxOfFovIdx(Idx, &(SvcVinErrCtrl.FovInfo[Idx].SrcSerDesIdx)); PRN_VIN_ERR_HDLR
                    if (SvcVinErrCtrl.FovInfo[Idx].SrcSerDesIdx == 0xdeadbeafU) {
                        SvcVinErrCtrl.FovInfo[Idx].SrcSerDesIdx = 0x0U;
                    }
                    SvcVinErrCtrl.FovInfo[Idx].VoutLatencySeqCnt    = 0ULL;
                    SvcVinErrCtrl.FovInfo[Idx].VoutLatencyThreshold = SVC_VIN_ERR_VOUT_LATENCY_THD;
                    SvcVinErrCtrl.FovInfo[Idx].FovLatencySeqCnt     = 0ULL;
                    SvcVinErrCtrl.FovInfo[Idx].FovLatencyThreshold  = SVC_VIN_ERR_FOV_LATENCY_THD;
                    SvcVinErrCtrl.FovInfo[Idx].State = SVC_VIN_ERR_FOV_INIT;
                }
            }

            for (DispIdx = 0U; DispIdx < pResCfg->DispNum; DispIdx ++) {
                VoutID = pResCfg->DispStrm[DispIdx].VoutID;
                if (VoutID < AMBA_DSP_MAX_VOUT_NUM) {
                    for (Idx = 0U; Idx < pResCfg->DispStrm[DispIdx].StrmCfg.NumChan; Idx ++) {
                        if (pResCfg->DispStrm[DispIdx].StrmCfg.ChanCfg[Idx].FovId < AMBA_DSP_MAX_VIEWZONE_NUM) {
                            SvcVinErrCtrl.FovInfo[pResCfg->DispStrm[DispIdx].StrmCfg.ChanCfg[Idx].FovId].DstVoutBits |= SvcVinErrTask_BitGet(VoutID);
                        }
                    }
                }
            }
        }

        if (RetVal == SVC_OK) {
            RetVal = SvcVinErrTask_OsdCreate();
            if (RetVal != 0U) {
                PRN_VIN_ERR_LOG "Fail to create vin err control - create osd ctrl fail" PRN_VIN_ERR_NG
            } else {
                PRN_VIN_ERR_LOG "Successful to create vin err osd ctrl" PRN_VIN_ERR_DBG1
            }
        }

        /* Configure vin dependency setting */
        AmbaSvcWrap_MisraMemset(&CurLivInfo, 0, sizeof(CurLivInfo));
        SvcLiveview_InfoGet(&CurLivInfo);
        if ((CurLivInfo.pNumStrm != NULL) &&
            (CurLivInfo.pStrmCfg != NULL) &&
            (CurLivInfo.pStrmCfg->pChanCfg != NULL)) {
            UINT32 StrmIdx, ChanIdx;
            UINT32 DepVinBits;
            UINT32 CurViewZoneID, VinID;
            UINT32 IsVirtualChan, FromVinID;
            SVC_VIN_ERR_DEPENDENCY_CTRL_s *pDepCtrl = &(SvcVinErrCtrl.DependencyCtrl);

            AmbaSvcWrap_MisraMemset(pDepCtrl, 0, sizeof(SVC_VIN_ERR_DEPENDENCY_CTRL_s));

            /* vin timeout dependency */

            pDepCtrl->NumOfStrm = *(CurLivInfo.pNumStrm);

            for (StrmIdx = 0U; StrmIdx < pDepCtrl->NumOfStrm; StrmIdx ++) {

                DepVinBits = 0U;

                // Scan streaming channel setting to get dependency vin bits and stream bits
                for (ChanIdx = 0U; ChanIdx < CurLivInfo.pStrmCfg[StrmIdx].NumChan; ChanIdx ++) {

                    CurViewZoneID = (UINT32)(CurLivInfo.pStrmCfg[StrmIdx].pChanCfg[ChanIdx].ViewZoneId);
                    CurViewZoneID &= 0xFFFFU;

                    if (CurViewZoneID < AMBA_DSP_MAX_VIEWZONE_NUM) {
                        VinID = SvcVinErrCtrl.FovInfo[CurViewZoneID].SrcVinID;
                        if (VinID < AMBA_DSP_MAX_VIN_NUM) {
                            pDepCtrl->StrmSelBits[VinID] |= SvcVinErrTask_BitGet(StrmIdx);
                            DepVinBits |= SvcVinErrTask_BitGet(VinID);
                        }
                    }
                }

                // Configure the each vin dependency bits
                for (Idx = 0U; Idx < AMBA_DSP_MAX_VIN_NUM; Idx ++) {
                    if ((DepVinBits & SvcVinErrTask_BitGet(Idx)) > 0U) {
                        pDepCtrl->DepVinBits[Idx] |= DepVinBits;
                    }
                }
            }

            // Configure the each vin dependency number
            for (Idx = 0U; Idx < AMBA_DSP_MAX_VIN_NUM; Idx ++) {
                if (pDepCtrl->DepVinBits[Idx] > 0U) {
                    pDepCtrl->DepVinNum[Idx] = SvcVinErrTask_PopCount(pDepCtrl->DepVinBits[Idx]);
                    if (pDepCtrl->DepVinNum[Idx] > 1U) {
                        pDepCtrl->UpdCtrl[Idx] = SVC_VIN_ERR_DEP_UPD_DEF;
                    }
                }
            }


            /* vin recover dependency */
            for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
                if ((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(VinID)) > 0U) {
                    if (0U != SvcResCfg_GetVinVirtualChan(VinID, &IsVirtualChan, &FromVinID)) {
                        RetVal = SVC_NG;
                        PRN_VIN_ERR_LOG "Fail to create vin err control - Get VinID(%d) virt chan info fail!"
                            PRN_VIN_ERR_ARG_UINT32 VinID PRN_VIN_ERR_ARG_POST
                        PRN_VIN_ERR_NG
                    } else {
                        pDepCtrl->Recover[VinID].FromVinID = FromVinID;
                        pDepCtrl->Recover[VinID].DepVinBits |= SvcVinErrTask_BitGet(VinID);
                        pDepCtrl->Recover[VinID].DepVinNum ++;

                        if (FromVinID != VinID) {
                            pDepCtrl->Recover[FromVinID].DepVinBits |= SvcVinErrTask_BitGet(VinID);
                            pDepCtrl->Recover[FromVinID].DepVinNum ++;
                        }
                    }
                }
            }

            for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
                if (pDepCtrl->Recover[VinID].DepVinNum > 0U) {
                    if (pDepCtrl->Recover[VinID].FromVinID < AMBA_DSP_MAX_VIN_NUM) {
                        pDepCtrl->Recover[VinID].DepVinBits = pDepCtrl->Recover[pDepCtrl->Recover[VinID].FromVinID].DepVinBits;
                        pDepCtrl->Recover[VinID].DepVinNum  = pDepCtrl->Recover[pDepCtrl->Recover[VinID].FromVinID].DepVinNum;
                    }
                }
            }
        }

        // Get current each VIN timeout setting
        if (CurLivInfo.pDspRes != NULL) {
            for (Idx = 0U; Idx < AMBA_DSP_MAX_VIN_NUM; Idx ++) {
                if ((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(Idx)) > 0U) {
                    SvcVinErrCtrl.VinTimeOut[Idx] = CurLivInfo.pDspRes->LiveviewResource.MaxVinTimeout[Idx];
                }
            }
        }

    }

    return RetVal;
}

/**
 * Initial vin err control task
 *
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcVinErrTask_Init(void)
{
#ifdef CONFIG_BUILD_MONFRW_GRAPH
    AmbaSvcWrap_MisraMemset(MonVinStateMsg,       0, sizeof(MonVinStateMsg));
    AmbaSvcWrap_MisraMemset(MonDspListenMsg,      0, sizeof(MonDspListenMsg));
    AmbaSvcWrap_MisraMemset(MonMsgPortList,       0, sizeof(MonMsgPortList));
#endif
    AmbaSvcWrap_MisraMemset(SvcVinErrRcvSubProc,  0, sizeof(SvcVinErrRcvSubProc));
    AmbaSvcWrap_MisraMemset(&SvcVinErrOsdCtrl,    0, sizeof(SvcVinErrOsdCtrl));
    AmbaSvcWrap_MisraMemset(&SvcVinErrRawFrzCtrl, 0, sizeof(SvcVinErrRawFrzCtrl));
    AmbaSvcWrap_MisraMemset(&SvcVinErrCtrl,       0, sizeof(SvcVinErrCtrl));

    SvcVinErrTask_CommandInstall();

    return SVC_OK;
}

/**
 * Start vin err control task
 *
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcVinErrTask_Start(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ( ( SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_INIT ) > 0U ) {
        RetVal = 99U;
        PRN_VIN_ERR_LOG "Svc vin err control has been created!" PRN_VIN_ERR_API
    } else {
        UINT32 Idx, VinNum = 0U, VinIDs[AMBA_DSP_MAX_VIN_NUM], VinSrc = 255U;

        AmbaSvcWrap_MisraMemset(VinIDs, 0, sizeof(VinIDs));
        if (0U != SvcResCfg_GetVinIDs(VinIDs, &VinNum)) {
            RetVal = SVC_NG;
            PRN_VIN_ERR_LOG "Fail to create vin err control - get vin number and id fail!" PRN_VIN_ERR_NG
        } else {
            if (VinNum == 0U) {
                RetVal = SVC_NG;
                PRN_VIN_ERR_LOG "Fail to create vin err control - vin number should not zero!" PRN_VIN_ERR_NG
            } else {
                if (0U != SvcResCfg_GetVinSrc(VinIDs[0], &VinSrc)) {
                    RetVal = SVC_NG;
                    PRN_VIN_ERR_LOG "Fail to create vin err control - get VinID(%d) source fail!"
                        PRN_VIN_ERR_ARG_UINT32 VinIDs[0] PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_NG
                } else {
                    if (VinSrc == SVC_VIN_SRC_MEM) {
                        RetVal = 99U;
                        PRN_VIN_ERR_LOG "Not support for video raw encode" PRN_VIN_ERR_API
                    }
                }
            }
        }

        /* Reset the module setting */
        if (RetVal == SVC_OK) {
            AmbaSvcWrap_MisraMemset(&SvcVinErrCtrl, 0, sizeof(SvcVinErrCtrl));
            SvcWrap_strcpy(SvcVinErrCtrl.Name, sizeof(SvcVinErrCtrl.Name), "SvcVinErr");
            RetVal = SvcVinErrTask_Config();
        }

        /* Configure serdes query */
        if (RetVal == SVC_OK) {
            RetVal = SvcVinErrTask_SerDesQryConfig();
        }

#ifdef CONFIG_BUILD_MONFRW_GRAPH
        /* Configure monitor framework module */
        if (RetVal == SVC_OK) {
            RetVal = SvcVinErrTask_MonFrwkCfg();
        }
#else
        if (RetVal == SVC_OK) {
            RetVal = SvcVinErrTask_MsgQryCfg();
        }
#endif

        /* Create recover task */
        if (RetVal == SVC_OK) {
            RetVal = SvcVinErrTask_RcvCtrlCfg();
        }

        /* Create err control task */
        if (RetVal == SVC_OK) {
            SVC_VIN_ERR_CTRL_TASK_s *pCtrlTask = &(SvcVinErrCtrl.ErrCtrlTask);

            AmbaSvcWrap_MisraMemset(pCtrlTask, 0, sizeof(SVC_VIN_ERR_CTRL_TASK_s));

            /* %s_%s */
              AmbaUtility_StringCopy(pCtrlTask->Name, sizeof(pCtrlTask->Name), SvcVinErrCtrl.Name);
            AmbaUtility_StringAppend(pCtrlTask->Name, (UINT32)sizeof(pCtrlTask->Name), "_");
            AmbaUtility_StringAppend(pCtrlTask->Name, (UINT32)sizeof(pCtrlTask->Name), "Ctrl");

            /* Create queue */
            PRetVal = AmbaKAL_MsgQueueCreate( &(pCtrlTask->Que),
                                              pCtrlTask->Name,
                                              (UINT32)sizeof(SVC_VIN_ERR_CTRL_CMD_t),
                                              pCtrlTask->QueBuf,
                                              (UINT32)sizeof(pCtrlTask->QueBuf) );
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_VIN_ERR_LOG "Fail to create vin err control - create err ctrl queue fail!" PRN_VIN_ERR_NG
            }

            /* Create err control task */
            {
                pCtrlTask->Ctrl.CpuBits    = SVC_VIN_ERR_TASK_CPU_BITS;
                pCtrlTask->Ctrl.Priority   = SVC_VIN_ERR_CTRL_TASK_PRI;
                pCtrlTask->Ctrl.EntryFunc  = SvcVinErrTask_ErrCtrlTaskEntry;
                pCtrlTask->Ctrl.StackSize  = SVC_VIN_ERR_STACK_SZ;
                pCtrlTask->Ctrl.pStackBase = &(pCtrlTask->Stack[0]);

#if defined(SVC_DATA_LOADER_TASK_PRI) && defined(SVC_DATA_LOADER_TASK_CPU_BITS)
                AmbaMisra_TouchUnused(&(pCtrlTask->Ctrl.CpuBits));
                if (pCtrlTask->Ctrl.CpuBits == SVC_DATA_LOADER_TASK_CPU_BITS) {
                    pCtrlTask->Ctrl.Priority = SVC_DATA_LOADER_TASK_PRI - 1U;
                }
#endif

                PRetVal = SvcTask_Create(pCtrlTask->Name, &(pCtrlTask->Ctrl));
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    PRN_VIN_ERR_LOG "Fail to create vin err control - create err ctrl task fail! ErrCode 0x%x"
                        PRN_VIN_ERR_ARG_UINT32 PRetVal PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_NG
                } else {
                    PRN_VIN_ERR_LOG "Successful to create vin err ctrl task - %s"
                        PRN_VIN_ERR_ARG_CSTR   pCtrlTask->Name PRN_VIN_ERR_ARG_POST
                    PRN_VIN_ERR_DBG1
                }
            }
        }

        /* create mutex */
        if (RetVal == SVC_OK) {
            PRetVal = AmbaKAL_MutexCreate(&(SvcVinErrCtrl.Mutex), SvcVinErrCtrl.Name);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_VIN_ERR_LOG "Fail to create vin err control - create mutex fail! ErrCode 0x%x"
                    PRN_VIN_ERR_ARG_UINT32 PRetVal PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_NG
            }
        }

        /* Create timer */
        if (RetVal == SVC_OK) {

            AmbaSvcWrap_MisraMemset(&(SvcVinErrCtrl.SwTimerCtrl), 0, sizeof(SvcVinErrCtrl.SwTimerCtrl));

            /* %s_%s */
              AmbaUtility_StringCopy(SvcVinErrCtrl.SwTimerCtrl.Name, sizeof(SvcVinErrCtrl.SwTimerCtrl.Name), SvcVinErrCtrl.Name);
            AmbaUtility_StringAppend(SvcVinErrCtrl.SwTimerCtrl.Name, (UINT32)sizeof(SvcVinErrCtrl.SwTimerCtrl.Name), "_");
            AmbaUtility_StringAppend(SvcVinErrCtrl.SwTimerCtrl.Name, (UINT32)sizeof(SvcVinErrCtrl.SwTimerCtrl.Name), "Timer");

            SvcVinErrCtrl.SwTimerCtrl.SelectBits = 0U;
            SvcVinErrCtrl.SwTimerCtrl.TimerPeriod = SVC_VIN_ERR_TIMER_PERIOD;
            SvcVinErrCtrl.SwTimerCtrl.TimerSeqCnt = 0U;
            PRetVal = AmbaKAL_TimerCreate(&(SvcVinErrCtrl.SwTimerCtrl.Timer),
                                          SvcVinErrCtrl.SwTimerCtrl.Name,
                                          SvcVinErrTask_TimerHandler,
                                          0,
                                          1,
                                          SvcVinErrCtrl.SwTimerCtrl.TimerPeriod,
                                          AMBA_KAL_AUTO_START);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_VIN_ERR_LOG "Fail to create vin err control - create timer fail! ErrCode 0x%x"
                    PRN_VIN_ERR_ARG_UINT32 PRetVal PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_NG
            } else {
                PRN_VIN_ERR_LOG "Successful to create vin err timer - %s"
                    PRN_VIN_ERR_ARG_CSTR   SvcVinErrCtrl.SwTimerCtrl.Name PRN_VIN_ERR_ARG_POST
                PRN_VIN_ERR_DBG1

                SvcVinErrCtrl.SwTimerCtrl.State |= SVC_VIN_ERR_TIMER_READY;
            }
        }

        if (RetVal == SVC_OK) {
            RetVal = SvcVinErrTask_RawFrzCreate();
            if (RetVal != 0U) {
                PRN_VIN_ERR_LOG "Fail to create vin err control - create raw freeze ctrl fail" PRN_VIN_ERR_NG
            } else {
                PRN_VIN_ERR_LOG "Successful to create vin err raw freeze ctrl" PRN_VIN_ERR_DBG1

                for (Idx = 0U; Idx < AMBA_DSP_MAX_VIN_NUM; Idx ++) {
                    if ((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(Idx)) > 0U) {
#ifdef SVC_VIN_ERR_FLG_SEN_FRZE_AAA
                        SvcVinErrCtrl.ErrState[Idx].FovFreezeSrc = SVC_VIN_ERR_FOV_FREZE_SRC_AAA;
                        AmbaMisra_TouchUnused(&(SvcVinErrCtrl.ErrState[Idx].FovFreezeSrc));
#endif
#ifdef SVC_VIN_ERR_FLG_SEN_FRZE_RAW
                        SvcVinErrCtrl.ErrState[Idx].FovFreezeSrc = SVC_VIN_ERR_FOV_FREZE_SRC_RAW;
#endif
                    }
                }
            }
        }

        if (RetVal == SVC_OK) {
            SvcVinErrCtrlFlg |= SVC_VIN_ERR_FLG_INIT;
            PRN_VIN_ERR_LOG "Successful to create vin err control!" PRN_VIN_ERR_OK
        } else if (RetVal == 99U) {
            PRN_VIN_ERR_LOG "Not enable vin err ctrl for video raw encode" PRN_VIN_ERR_DBG1
        } else {
            PRN_VIN_ERR_LOG "Fail to create vin err control!" PRN_VIN_ERR_NG
        }
    }

    if (RetVal == 99U) {
        RetVal = SVC_OK;
    }

    return RetVal;
}

/**
 * Stop vin err control task
 *
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcVinErrTask_Stop(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ( ( SvcVinErrCtrlFlg & SVC_VIN_ERR_FLG_INIT ) == 0U ) {
        RetVal = SVC_NG;
        PRN_VIN_ERR_LOG "Fail to delete vin err control - create module first!" PRN_VIN_ERR_NG
    } else {
        UINT32 Idx;

#ifdef CONFIG_BUILD_MONFRW_GRAPH
        SvcVinErrTask_MonFrwkDeCfg();
#else
        SvcVinErrTask_MsgQryDeCfg();
#endif

        /* delete control task */
        PRetVal = SvcTask_Destroy( &(SvcVinErrCtrl.ErrCtrlTask.Ctrl) ); PRN_VIN_ERR_HDLR

        /* delete control queue */
        PRetVal = AmbaKAL_MsgQueueDelete( &(SvcVinErrCtrl.ErrCtrlTask.Que) ); PRN_VIN_ERR_HDLR

        /* delete recover task */
        PRetVal = SvcTask_Destroy( &(SvcVinErrCtrl.RecoverTask.Ctrl) ); PRN_VIN_ERR_HDLR

        /* delete recover queue */
        PRetVal = AmbaKAL_MsgQueueDelete( &(SvcVinErrCtrl.RecoverTask.Que) ); PRN_VIN_ERR_HDLR

        /* delete timer */
        PRetVal = AmbaKAL_TimerDelete(&(SvcVinErrCtrl.SwTimerCtrl.Timer)); PRN_VIN_ERR_HDLR

        /* delete vin liveview flag */
        PRetVal = AmbaKAL_EventFlagClear(&(SvcVinErrCtrl.VinLivFlag), 0xFFFFFFFFU); PRN_VIN_ERR_HDLR
        PRetVal = AmbaKAL_EventFlagDelete(&(SvcVinErrCtrl.VinLivFlag)); PRN_VIN_ERR_HDLR

        /* delete error state flag */
        for (Idx = 0U; Idx < AMBA_DSP_MAX_VIN_NUM; Idx ++) {
            if ((SvcVinErrCtrl.VinSelectBits & SvcVinErrTask_BitGet(Idx)) > 0U) {
                PRetVal = AmbaKAL_EventFlagClear(&(SvcVinErrCtrl.ErrState[Idx].VinStateFlag), 0xFFFFFFFFU); PRN_VIN_ERR_HDLR
                PRetVal = AmbaKAL_EventFlagDelete(&(SvcVinErrCtrl.ErrState[Idx].VinStateFlag)); PRN_VIN_ERR_HDLR
            }
        }

        /* delete recover control flag */
        PRetVal = AmbaKAL_EventFlagClear(&(SvcVinErrCtrl.RecoverTask.RcvCtrl.ActFlg), 0xFFFFFFFFU); PRN_VIN_ERR_HDLR
        PRetVal = AmbaKAL_EventFlagDelete(&(SvcVinErrCtrl.RecoverTask.RcvCtrl.ActFlg)); PRN_VIN_ERR_HDLR

        /* delete mutex */
        PRetVal = AmbaKAL_MutexDelete(&(SvcVinErrCtrl.Mutex)); PRN_VIN_ERR_HDLR

        /* delete osd ctrl */
        PRetVal = SvcVinErrTask_OsdDelete(); PRN_VIN_ERR_HDLR

        /* delete raw freeze */
        PRetVal = SvcVinErrTask_RawFrzDelete(); PRN_VIN_ERR_HDLR

        /* reset module setting */
        AmbaSvcWrap_MisraMemset(&SvcVinErrCtrl, 0, sizeof(SvcVinErrCtrl));


        SvcVinErrCtrlFlg &= ~SVC_VIN_ERR_FLG_INIT;
    }

    return RetVal;
}

