/**
 *  @file SvcVoutErrTask.c
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
 *  @details svc vout error handle task
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaShell.h"
#include "AmbaVOUT_Def.h"
#include "AmbaDSP.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaFPD.h"
#include "AmbaFPD_MAXIM_Define.h"
#include "AmbaSensor.h"
#include "AmbaDSP_Event.h"

#include "SvcErrCode.h"
#include "SvcWrap.h"
#include "SvcTask.h"
#include "AmbaMonDef.h"
#include "AmbaMonFramework.h"
#include "SvcCmd.h"
#include "SvcLog.h"
#include "SvcDSP.h"
#include "SvcOsd.h"
#include "SvcGui.h"

#include "SvcVinSrc.h"
#include "SvcVoutSrc.h"
#include "SvcDisplay.h"
#include "SvcIK.h"
#include "SvcLiveview.h"

#include "SvcResCfg.h"
#include "SvcTaskList.h"
#include "SvcInfoPack.h"
#include "SvcVoutErrTask.h"

#define SVC_VOUT_ERR_FLG_INIT           (0x00000001U)
#define SVC_VOUT_ERR_FLG_OSD_INIT       (0x00000002U)
#define SVC_VOUT_ERR_FLG_CMD_INSTALL    (0x00000004U)
#define SVC_VOUT_ERR_FLG_VOUT_FRZ_INIT  (0x00000200U)
#define SVC_VOUT_ERR_FLG_VOUT_FRZ_OFF   (0x00000400U)
#define SVC_VOUT_ERR_FLG_MON_MSG_INIT   (0x00001000U)
#define SVC_VOUT_ERR_FLG_SHELL_INIT     (0x00002000U)
#define SVC_VOUT_ERR_FLG_DEV_INIT       (0x00004000U)
#define SVC_VOUT_ERR_FLG_RCV_INIT       (0x00008000U)

#define SVC_VOUT_ERR_FLG_LOG_OK         (0x01000000U)
#define SVC_VOUT_ERR_FLG_LOG_NG         (0x02000000U)
#define SVC_VOUT_ERR_FLG_LOG_NG2        (0x04000000U)
#define SVC_VOUT_ERR_FLG_LOG_API        (0x08000000U)
#define SVC_VOUT_ERR_FLG_LOG_DBG1       (0x10000000U)  // Normal debug message
#define SVC_VOUT_ERR_FLG_LOG_DBG2       (0x20000000U)  //

#define SVC_VOUT_ERR_STACK_SZ            (0xA000U)
#define SVC_VOUT_ERR_MAX_SER_NUM         (2U)
#define SVC_VOUT_ERR_VOUT_RECOVER_TOUT   (5000U)
#define SVC_VOUT_ERR_CTRL_TASK_PRI       (SVC_VOUT_ERR_TASK_PRI + 1U)
#define SVC_VOUT_ERR_MON_TASK_PRI        (30U)
#define SVC_VOUT_ERR_RECOVER_TASK_PRI    (SVC_VOUT_ERR_TASK_PRI + 2U)
#define SVC_VOUT_ERR_TIMER_PERIOD        (5U)
#define SVC_VOUT_ERR_LNK_STA_CHG_THD     (5U)
#define SVC_VOUT_ERR_YUV_FRZ_CHG_THD     (3U)

/* =========================================================== *\
 *    For OSD
\* =========================================================== */
#define SVC_VOUT_ERR_GUI_CANVAS_LVL      (11U)
#ifndef CONFIG_ICAM_32BITS_OSD_USED
#define SVC_VOUT_ERR_OSD_STR_COLOR       (252U)
#else
#define SVC_VOUT_ERR_OSD_STR_COLOR       (0x800000FFU)
#endif
#define SVC_VOUT_ERR_OSD_STR_PTN_LEN     (20U)

#define SVC_VOUT_ERR_OSD_PTN_SIG_LOST    ("SIGNAL LOST")
#define SVC_VOUT_ERR_OSD_PTN_VOUT_FREEZE ("VOUT FREEZE")

#define SVC_VOUT_ERR_OSD_PTN_ID_SIG_LST   (0U)
#define SVC_VOUT_ERR_OSD_PTN_ID_VOUT_FRZ  (1U)
#define SVC_VOUT_ERR_OSD_PTN_NUM          (2U)

typedef struct {
#define SVC_VOUT_ERR_OSD_UNIT_ENABLE     (0x1U)
#define SVC_VOUT_ERR_OSD_UNIT_DISPLAY    (0x2U)
#define SVC_VOUT_ERR_OSD_UPD_LOCK        (0x4U)
    UINT32            State;
    UINT32            OsdPtnSelectBits;
    UINT32            DrawFontSize[SVC_VOUT_ERR_OSD_PTN_NUM];
    SVC_GUI_CANVAS_s  DrawStrWin[SVC_VOUT_ERR_OSD_PTN_NUM];
    UINT32            DrawStrCnt[SVC_VOUT_ERR_OSD_PTN_NUM];
    UINT32            DrawUpd;
} SVC_VOUT_ERR_OSD_DRAW_s;

typedef struct {
    UINT32              Width;
    UINT32              Height;
    UINT32              DrawLevel;
    SVC_GUI_CANVAS_s    DrawWin;
    SVC_GUI_DRAW_FUNC   pDrawFunc;
    SVC_GUI_UPDATE_FUNC pUpdateFunc;
} SVC_VOUT_ERR_OSD_INFO_s;

typedef struct {
    UINT32     PtnID;
    char       StrPtn[SVC_VOUT_ERR_OSD_STR_PTN_LEN];
    UINT32     StrPtnLen;
    UINT32     PtnWidth;
    UINT32     PtnHeight;
} SVC_VOUT_ERR_OSD_PTN_INFO_s;

typedef struct {
    char                        Name[32];
    AMBA_KAL_MUTEX_t            Mutex;
    UINT32                      SelectBits;
    SVC_VOUT_ERR_OSD_INFO_s     OsdInfo[AMBA_DSP_MAX_VOUT_NUM];
    SVC_VOUT_ERR_OSD_PTN_INFO_s OsdPtnInfo[SVC_VOUT_ERR_OSD_PTN_NUM];
    SVC_VOUT_ERR_OSD_DRAW_s     DrawCtrl[AMBA_DSP_MAX_VOUT_NUM];
} SVC_VOUT_ERR_OSD_CTRL_s;

static UINT32 SvcVoutErrTsk_OsdCreate(void);
static UINT32 SvcVoutErrTsk_OsdDelete(void);
static void   SvcVoutErrTsk_OsdUpdateLock(UINT32 VoutID, UINT32 UpdateLock);
static void   SvcVoutErrTsk_OsdUpdate(UINT32 VoutID);
static void   SvcVoutErrTsk_OsdInfo(void);
static void   SvcVoutErrTsk_OsdDrawFunc(UINT32 VoutID, UINT32 Level);
static void   SvcVoutErrTsk_OsdUpdateFunc(UINT32 VoutID, UINT32 Level, UINT32 *pUpdate);

static SVC_VOUT_ERR_OSD_CTRL_s SvcVoutErrOsdCtrl GNU_SECTION_NOZEROINIT;

/* =========================================================== *\
 *    For VOUT Recover
\* =========================================================== */

typedef struct {
#define SVC_VOUT_ERR_RCV_CMD_VOUT    (0U)
#define SVC_VOUT_ERR_RCV_CMD_SERDES  (1U)
#define SVC_VOUT_ERR_RCV_CMD_FPD     (2U)
#define SVC_VOUT_ERR_RCV_CMD_NUM     (3U)
    UINT32 CmdID;
    UINT32 VoutID;
    UINT32 SerDesSelectBits;
} SVC_VOUT_ERR_RECOVER_CMD_t;

#define SVC_VOUT_ERR_RCV_PROC_STP_VOUT      (0U)    // STOP VOUT
#define SVC_VOUT_ERR_RCV_PROC_RST_VOUT      (1U)    // Reset VOUT
#define SVC_VOUT_ERR_RCV_PROC_DIS_SER_QRY   (2U)    // Disable SerDes Query
#define SVC_VOUT_ERR_RCV_PROC_FPD_DIS       (3U)    // Disable FPD driver
#define SVC_VOUT_ERR_RCV_PROC_RST_PWR       (4U)    // Reset Power
#define SVC_VOUT_ERR_RCV_PROC_FPD_CFG       (5U)    // Config FPD driver
#define SVC_VOUT_ERR_RCV_PROC_FPD_BL        (6U)    // Enable FPD BackLight
#define SVC_VOUT_ERR_RCV_PROC_FPD_ENA       (7U)    // Enable FPD driver
#define SVC_VOUT_ERR_RCV_PROC_ENA_SER_QRY   (8U)    // Enable SerDes Query
#define SVC_VOUT_ERR_RCV_PROC_STR_VOUT      (9U)    // Start VOUT
#define SVC_VOUT_ERR_RCV_PROC_NUM          (16U)

typedef UINT32 (*SVC_VOUT_ERR_RCV_PROC_f)(UINT32 PreChkVal, const void *pCmd, const void *pProc);

typedef struct {
    UINT32                  Enable;
    UINT32                  ProcID;
    char                    ProcName[16];
    UINT32                  ProcBit;
    SVC_VOUT_ERR_RCV_PROC_f pFunc;
} SVC_VOUT_ERR_RCV_PROC_s;

typedef struct {
    AMBA_KAL_EVENT_FLAG_t    ActFlg;
    char                     Name[32];
    UINT32                   ProcSelectBits[SVC_VOUT_ERR_RCV_CMD_NUM];
    SVC_VOUT_ERR_RCV_PROC_s *pSubProc;
} SVC_VOUT_ERR_RCV_CTRL_s;

typedef struct {
    char                        Name[32];
    AMBA_KAL_MSG_QUEUE_t        Que;
    SVC_VOUT_ERR_RECOVER_CMD_t  QueBuf[AMBA_DSP_MAX_VOUT_NUM * SVC_VOUT_ERR_MAX_SER_NUM];
    SVC_TASK_CTRL_s             Ctrl;
    UINT8                       Stack[SVC_VOUT_ERR_STACK_SZ];
    SVC_VOUT_ERR_RCV_CTRL_s     RcvCtrl;
} SVC_VOUT_ERR_RECOVER_TASK_s;

static void   SvcVoutErrTsk_RcvSubProcInit(void);
static UINT32 SvcVoutErrTsk_RcvCtrlCfg(void);
static void   SvcVoutErrTsk_RcvCtrlInfo(void);
static UINT32 SvcVoutErrTsk_RcvFlagClear(UINT32 VoutID, UINT32 Flag);
static UINT32 SvcVoutErrTsk_RcvFlagSet(UINT32 VoutID, UINT32 Flag);
static UINT32 SvcVoutErrTsk_RcvFlagGet(UINT32 VoutID, UINT32 Flag, UINT32 Timeout);
static UINT32 SvcVoutErrTsk_RcvFlagQry(UINT32 VoutID, UINT32 *pFlag);
static UINT32 SvcVoutErrTsk_RcvProcFpd(SVC_VOUT_ERR_RECOVER_CMD_t *pCmd);
static void   SvcVoutErrTsk_RcvProcVout(SVC_VOUT_ERR_RECOVER_CMD_t *pCmd);
static void   SvcVoutErrTsk_RcvProcSerDesLink(SVC_VOUT_ERR_RECOVER_CMD_t *pCmd);
static UINT32 SvcVoutErrTsk_RcvProcStartVout(UINT32 PreChkVal, const void *pCmd, const void *pProc);
static UINT32 SvcVoutErrTsk_RcvProcStopVout(UINT32 PreChkVal, const void *pCmd, const void *pProc);
static UINT32 SvcVoutErrTsk_RcvProcResetVout(UINT32 PreChkVal, const void *pCmd, const void *pProc);
static UINT32 SvcVoutErrTsk_RcvProcResetPWR(UINT32 PreChkVal, const void *pCmd, const void *pProc);
static UINT32 SvcVoutErrTsk_RcvProcFpdDis(UINT32 PreChkVal, const void *pCmd, const void *pProc);
static UINT32 SvcVoutErrTsk_RcvProcFpdEna(UINT32 PreChkVal, const void *pCmd, const void *pProc);
static UINT32 SvcVoutErrTsk_RcvProcFpdCfg(UINT32 PreChkVal, const void *pCmd, const void *pProc);
static UINT32 SvcVoutErrTsk_RcvProcFpdBL(UINT32 PreChkVal, const void *pCmd, const void *pProc);
static UINT32 SvcVoutErrTsk_RcvProcEnaSerQry(UINT32 PreChkVal, const void *pCmd, const void *pProc);
static UINT32 SvcVoutErrTsk_RcvProcDisSerQry(UINT32 PreChkVal, const void *pCmd, const void *pProc);
static void*  SvcVoutErrTsk_RcvTaskEntry(void* EntryArg);

static SVC_VOUT_ERR_RCV_PROC_s  SvcVoutErrRcvSubProc[SVC_VOUT_ERR_RCV_PROC_NUM] GNU_SECTION_NOZEROINIT;

/* =========================================================== *\
 *    For AmbaMonitor Framework
\* =========================================================== */

#define VOUT_ERR_MON_MSG_TIMEOUT     (0x10U)

#define VOUT_ERR_MON_PORT_VOUT_STATE (0U)
#define VOUT_ERR_MON_PORT_DSP_LISTEN (1U)
#define VOUT_ERR_MON_PORT_NUM        (2U)

#define MON_VOUT_STAT_MSG_VOUT_START (0U)
#define MON_VOUT_STAT_MSG_VOUT_TOUT  (1U)
#define MON_VOUT_STAT_MSG_VOUT_LINK  (2U)
#define MON_VOUT_STAT_MSG_NUM        (3U)

#define VOUT_ERR_MON_DSP_LISTEN_MSG_VOUT_RDY  (0U)
#define VOUT_ERR_MON_DSP_LISTEN_MSG_DISP_RDY  (1U)
#define VOUT_ERR_MON_DSP_LISTEN_MSG_NUM       (2U)

typedef void (*SVC_VOUT_ERR_MON_MSG_HDLR_f)(const void *pMsg, UINT64 Flag, UINT32 MsgType);
typedef struct {
    UINT32                        Enable;
    UINT64                        MsgFlag;
    char                          MsgName[32];
    SVC_VOUT_ERR_MON_MSG_HDLR_f   pMsgProc;
} SVC_VOUT_ERR_MON_MSG_s;

typedef struct {
    UINT32                    Enable;
    UINT32                    PortID;
    char                      MsgPortName[32];
    UINT32                    NumOfMsg;
    SVC_VOUT_ERR_MON_MSG_s   *pMsgList;
} SVC_VOUT_ERR_MON_PORT_s;

typedef struct {
    UINT32                     NumOfMonitorPort;
    AMBA_MON_MESSAGE_PORT_s    MonitorPort[VOUT_ERR_MON_PORT_NUM];
    SVC_VOUT_ERR_MON_PORT_s   *pMonitorPortCtrl;
} SVC_VOUT_ERR_MON_CTRL_s;

typedef struct {
    UINT32                     NumOfTask;
    char                       Name[VOUT_ERR_MON_PORT_NUM][32];
    SVC_TASK_CTRL_s            Ctrl[VOUT_ERR_MON_PORT_NUM];
    UINT8                      Stack[VOUT_ERR_MON_PORT_NUM][SVC_VOUT_ERR_STACK_SZ];
} SVC_VOUT_ERR_MON_TASK_s;

static void   SvcVoutErrTsk_MonMsgInit(void);
static UINT32 SvcVoutErrTsk_MonFrwkCfg(void);
static void*  SvcVoutErrTsk_MonFrwkTskEntry(void* EntryArg);
static void   SvcVoutErrTsk_MonMsgProcVoutStart(const void *pMsg, UINT64 Flag, UINT32 MsgType);
static void   SvcVoutErrTsk_MonMsgProcVoutTout(const void *pMsg, UINT64 Flag, UINT32 MsgType);
static void   SvcVoutErrTsk_MonMsgProcLinkState(const void *pMsg, UINT64 Flag, UINT32 MsgType);
static void   SvcVoutErrTsk_MonMsgProcVoutRdy(const void *pMsg, UINT64 Flag, UINT32 MsgType);
static void   SvcVoutErrTsk_MonMsgProcDispRdy(const void *pMsg, UINT64 Flag, UINT32 MsgType);

static SVC_VOUT_ERR_MON_MSG_s  VoutErrMonVoutStateMsg[MON_VOUT_STAT_MSG_NUM] GNU_SECTION_NOZEROINIT;
static SVC_VOUT_ERR_MON_MSG_s  VoutErrMonDspListenMsg[VOUT_ERR_MON_DSP_LISTEN_MSG_NUM] GNU_SECTION_NOZEROINIT;
static SVC_VOUT_ERR_MON_PORT_s VoutErrMonMsgPortist[VOUT_ERR_MON_PORT_NUM] GNU_SECTION_NOZEROINIT;

/* =========================================================== *\
 *    For Device Info
\* =========================================================== */

#define SVC_VOUT_ERR_SUPPORTED_DEV_NUM  (5U)
#define SVC_VOUT_ERR_MAX_DEV_MODE_NUM   (64U)
typedef struct {
    char   DevName[64];
    UINT32 DevModeNum;
    UINT32 DevSerDesSelectBits[SVC_VOUT_ERR_MAX_DEV_MODE_NUM];
    UINT32 EnableErrCtrl;
} SVC_VOUT_ERR_DEV_INFO_s;

static void                     SvcVoutErrTsk_DevInfoInit(void);
static SVC_VOUT_ERR_DEV_INFO_s *SvcVoutErrTsk_DevInfoGet(UINT32 VoutID);
static void                     SvcVoutErrTsk_DevInfoPrn(void);

static SVC_VOUT_ERR_DEV_INFO_s SvcVoutErrDevInfo[SVC_VOUT_ERR_SUPPORTED_DEV_NUM] GNU_SECTION_NOZEROINIT;

/* =========================================================== *\
 *    For VOUT Error Control
\* =========================================================== */

typedef struct {
#define SVC_VOUT_ERR_CTRL_CMD_LNK_UPD   (0x1U)
#define SVC_VOUT_ERR_CTRL_CMD_VOUT_UPD  (0x2U)
#define SVC_VOUT_ERR_CTRL_CMD_DISP_UPD  (0x3U)
    UINT32 CmdID;
    UINT32 Data[8];
} SVC_VOUT_ERR_CTRL_CMD_t;

typedef struct {
    char                       Name[32];
    SVC_TASK_CTRL_s            Ctrl;
    AMBA_KAL_MSG_QUEUE_t       Que;
    SVC_VOUT_ERR_CTRL_CMD_t    QueBuf[AMBA_DSP_MAX_VOUT_NUM * 32U];
    UINT8                      Stack[SVC_VOUT_ERR_STACK_SZ];
} SVC_VOUT_ERR_CTRL_TASK_s;

typedef struct {
    char                 Name[32];
    UINT32               ChgState[AMBA_DSP_MAX_VOUT_NUM];
    UINT32               CurState[AMBA_DSP_MAX_VOUT_NUM];
    UINT32               SelectBits;
    AMBA_KAL_TIMER_t     Timer;
    UINT32               TimerPeriod;
    UINT32               ThersholdMS[AMBA_DSP_MAX_VOUT_NUM];
} SVC_VOUT_ERR_LINK_CHG_CTRL_s;

typedef struct {
    AMBA_KAL_EVENT_FLAG_t VoutStateFlag;
#define SVC_VOUT_ERR_VOUT_STATE_START     (0x1U)
#define SVC_VOUT_ERR_VOUT_STATE_TOUT      (0x2U)
#define SVC_VOUT_ERR_VOUT_STATE_RECOVER   (0x4U)

    UINT32 LinkUpdEnable;
#define SVC_VOUT_ERR_LNK_UPD_OFF         (0U)
#define SVC_VOUT_ERR_LNK_UPD_ON          (1U)
    UINT32 LinkBkpState;

    UINT32 LinkState;
#define SVC_VOUT_ERR_SER_STAT_LINK_LOCK  ( 0x1U )

    UINT32 FreezeState;
#define SVC_VOUT_ERR_VOUT_STATE_FREZE      ( 0x1UL )
} SVC_VOUT_ERR_STATE_s;

typedef struct {
    UINT32 MsgID;
#define SVC_VOUT_ERR_FRZ_MSG_ID_VOUT_RDY   (1U)
#define SVC_VOUT_ERR_FRZ_MSG_ID_DISP_RDY   (2U)
#define SVC_VOUT_ERR_FRZ_MSG_ID_FRZE_CHK   (3U)
#define SVC_VOUT_ERR_FRZ_MSG_ID_FRZE_HIT   (4U)
#define SVC_VOUT_ERR_FRZ_MSG_ID_NUM        (5U)
    UINT32 TimeStamp;
    UINT32 Pitch;
    UINT32 Width;
    UINT32 Height;
    UINT64 Data[4];
} SVC_VOUT_ERR_VOUT_FRZ_MSG_UNIT_s;

typedef struct {
    UINT32                           State;
#define SVC_VOUT_ERR_VOUT_FRZ_MSG_INIT  (0x1U)
#define SVC_VOUT_ERR_VOUT_FRZ_MSG_OFF   (0x2U)
    AMBA_KAL_MUTEX_t                 Mutex;
    char                             Name[32];
    UINT32                           MsgQueDepth;
#define SVC_VOUT_ERR_VOUT_FRZ_MSG_NUM   (48U)
    SVC_VOUT_ERR_VOUT_FRZ_MSG_UNIT_s MsgQue[SVC_VOUT_ERR_VOUT_FRZ_MSG_NUM];
    UINT32                           MsgQueIdx;
} SVC_VOUT_ERR_VOUT_FRZ_MSG_s;

typedef struct {
    UINT32                      State;
#define SVC_VOUT_ERR_VOUT_FRZ_ENABLE (0x1U)
    UINT32                      FovBits;
    UINT32                      CurBits;
    UINT32                      FrzCnt;
    UINT32                      PreLumaCheckSum;
    UINT32                      PreChromaCheckSum;
    SVC_VOUT_ERR_VOUT_FRZ_MSG_s FrzMsg;
} SVC_VOUT_ERR_VOUT_FRZ_CTRL_s;

typedef struct {
    AMBA_KAL_MUTEX_t             Mutex;
    char                         Name[32];

    /*  Global state */
    UINT32                       VoutSelectBits;
    UINT32                       VoutErrSelectBits;
    UINT32                       SerDesSelectBits[AMBA_DSP_MAX_VOUT_NUM];
    SVC_VOUT_ERR_STATE_s         ErrState[AMBA_DSP_MAX_VOUT_NUM];

    SVC_VOUT_ERR_LINK_CHG_CTRL_s LinkChgCtrl;
    SVC_VOUT_ERR_VOUT_FRZ_CTRL_s VoutFrzCtrl[AMBA_DSP_MAX_VOUT_NUM];
    SVC_VOUT_ERR_MON_CTRL_s      MonFrwkCtrl;
    SVC_VOUT_ERR_MON_TASK_s      MonFrwkTask;
    SVC_VOUT_ERR_CTRL_TASK_s     ErrCtrlTask;
    SVC_VOUT_ERR_RECOVER_TASK_s  RecoverTask;

} SVC_VOUT_ERR_CTRL_s;

static inline UINT32 SvcVoutErrTsk_BitGet(UINT32 Val);
static void   SvcVoutErrTsk_ErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode);
static UINT32 SvcVoutErrTsk_MutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut);
static void   SvcVoutErrTsk_MutexGive(AMBA_KAL_MUTEX_t *pMutex);
static UINT32 SvcVoutErrTsk_GetSerDesState(UINT32 VoutID, UINT32 SerDesID, UINT32 CurState);
static void*  SvcVoutErrTsk_ErrCtrlTskEntry(void* EntryArg);
static void   SvcVoutErrTsk_RecoverChk(UINT32 VoutID, UINT32 PreLink, UINT32 CurLink);
static void   SvcVoutErrTsk_LinkStateChange(UINT32 VoutID, UINT32 CurLinkState);
static void   SvcVoutErrTsk_LinkStateUpd(UINT32 VoutID, UINT32 CurLinkState);
static void   SvcVoutErrTsk_LinkStateUpdEna(UINT32 VoutID, UINT32 Enable);
static void   SvcVoutErrTsk_TimerHandler(UINT32 EntryArg);
static void   SvcVoutErrTsk_Info(void);
static UINT32 SvcVoutErrTsk_Config(void);
static UINT32 SvcVoutErrTsk_VoutFrzConfig(void);
static void   SvcVoutErrTsk_VoutFrzFovUpd(UINT32 VoutID, UINT32 CurBits);
static void   SvcVoutErrTsk_VoutFrzProc(UINT32 VoutID, UINT32 LumaCheckSum, UINT32 ChromaCheckSum);
static void   SvcVoutErrTsk_VoutFrzMsgConfig(UINT32 VoutID);
static void   SvcVoutErrTsk_VoutFrzMsgEnable(UINT32 VoutID, UINT32 Enable);
static void   SvcVoutErrTsk_VoutFrzMsgSned(UINT32 VoutID, const SVC_VOUT_ERR_VOUT_FRZ_MSG_UNIT_s *pMsg);
static void   SvcVoutErrTsk_VoutFrzMsgDump(UINT32 VoutID);
static void   SvcVoutErrTsk_FreezeUpd(UINT32 VoutID, UINT32 HitFreeze);

static SVC_VOUT_ERR_CTRL_s      SvcVoutErrCtrl GNU_SECTION_NOZEROINIT;
static UINT32                   SvcVoutErrCtrlFlg = ( SVC_VOUT_ERR_FLG_LOG_OK |
                                                      SVC_VOUT_ERR_FLG_LOG_NG |
                                                      SVC_VOUT_ERR_FLG_LOG_API ); //SVC_VOUT_ERR_FLG_DBG_MSG;

/* =========================================================== *\
 *    For Shell Command
\* =========================================================== */

typedef UINT32 (*SVC_VOUT_ERR_SHELL_FUNC_f)(UINT32 ArgCount, char * const *pArgVector);
typedef void   (*SVC_VOUT_ERR_SHELL_USAGE_f)(void);
typedef struct {
    UINT32                     Enable;
    char                       ShellCmdName[32];
    SVC_VOUT_ERR_SHELL_FUNC_f  pFunc;
    SVC_VOUT_ERR_SHELL_USAGE_f pUsage;
} SVC_VOUT_ERR_SHELL_FUNC_s;

static void   SvcVoutErrTsk_ShellStrToU32(const char *pString, UINT32 *pValue);
static UINT32 SvcVoutErrTsk_ShellDbgMsg(UINT32 ArgCount, char * const *pArgVector);
static void   SvcVoutErrTsk_ShellDbgMsgU(void);
static UINT32 SvcVoutErrTsk_ShellVoutRecover(UINT32 ArgCount, char * const *pArgVector);
static void   SvcVoutErrTsk_ShellVoutRecoverU(void);
static UINT32 SvcVoutErrTsk_ShellInfo(UINT32 ArgCount, char * const *pArgVector);
static void   SvcVoutErrTsk_ShellInfoU(void);
static UINT32 SvcVoutErrTsk_ShellMCtrl(UINT32 ArgCount, char * const *pArgVector);
static void   SvcVoutErrTsk_ShellMCtrlU(void);
static UINT32 SvcVoutErrTsk_ShellVoutFrz(UINT32 ArgCount, char * const *pArgVector);
static void   SvcVoutErrTsk_ShellVoutFrzU(void);
static UINT32 SvcVoutErrTsk_ShellVoutFrzEna(UINT32 ArgCount, char * const *pArgVector);
static void   SvcVoutErrTsk_ShellVoutFrzEnaU(void);
static UINT32 SvcVoutErrTsk_ShellVoutFrzRst(UINT32 ArgCount, char * const *pArgVector);
static void   SvcVoutErrTsk_ShellVoutFrzRstU(void);
static UINT32 SvcVoutErrTsk_ShellVoutFrzMD(UINT32 ArgCount, char * const *pArgVector);
static void   SvcVoutErrTsk_ShellVoutFrzMDU(void);
static void   SvcVoutErrTsk_ShellUsage(void);
static void   SvcVoutErrTsk_ShellEntryInit(void);
static void   SvcVoutErrTsk_ShellEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
static void   SvcVoutErrTsk_CommandInstall(void);

#define SVC_VOUT_ERR_SHELL_VOUT_FRZ_NUM   (3U)
static SVC_VOUT_ERR_SHELL_FUNC_s SvcVoutErrShellVoutFrz[SVC_VOUT_ERR_SHELL_VOUT_FRZ_NUM] GNU_SECTION_NOZEROINIT;

#define SVC_VOUT_ERR_SHELL_NUM   (5U)
static SVC_VOUT_ERR_SHELL_FUNC_s SvcVoutErrShellFunc[SVC_VOUT_ERR_SHELL_NUM] GNU_SECTION_NOZEROINIT;

#define SVC_LOG_VOUT_ERR             "VOUT_ERR_TSK"
#define SVC_LOG_VOUT_ERR_HL_TITLE_0  ("\033""[38;2;255;125;38m""\033""[48;2;0;0;0m")
#define SVC_LOG_VOUT_ERR_HL_TITLE_1  ("\033""[38;2;255;255;128m""\033""[48;2;0;0;0m")
#define SVC_LOG_VOUT_ERR_HL_TITLE_2  ("\033""[38;2;181;230;29m""\033""[48;2;0;0;0m")
#define SVC_LOG_VOUT_ERR_HL_TITLE_3  ("\033""[38;2;255;201;14m""\033""[48;2;0;0;0m")
#define SVC_LOG_VOUT_ERR_HL_NUM      ("\033""[38;2;255;128;128m""\033""[48;2;0;0;0m")
#define SVC_LOG_VOUT_ERR_HL_STR      ("\033""[38;2;0;255;128m""\033""[48;2;0;0;0m")
#define SVC_LOG_VOUT_ERR_HL_END      ("\033""[0m")

#define PRN_VOUT_ERR_LOG        { SVC_WRAP_PRINT_s SvcVoutErrLog; AmbaSvcWrap_MisraMemset(&(SvcVoutErrLog), 0, sizeof(SvcVoutErrLog)); SvcVoutErrLog.Argc --; SvcVoutErrLog.pStrFmt =
#define PRN_VOUT_ERR_ARG_UINT64 ; SvcVoutErrLog.Argc ++; SvcVoutErrLog.Argv[SvcVoutErrLog.Argc].Uint64 =
#define PRN_VOUT_ERR_ARG_UINT32 ; SvcVoutErrLog.Argc ++; SvcVoutErrLog.Argv[SvcVoutErrLog.Argc].Uint64   = (UINT64)((
#define PRN_VOUT_ERR_ARG_CSTR   ; SvcVoutErrLog.Argc ++; SvcVoutErrLog.Argv[SvcVoutErrLog.Argc].pCStr    = ((
#define PRN_VOUT_ERR_ARG_CPOINT ; SvcVoutErrLog.Argc ++; SvcVoutErrLog.Argv[SvcVoutErrLog.Argc].pPointer = ((
#define PRN_VOUT_ERR_ARG_POST   ))
#define PRN_VOUT_ERR_OK         ; SvcVoutErrLog.Argc ++; SvcVoutErrTsk_LogPrint(SVC_VOUT_ERR_FLG_LOG_OK  , &(SvcVoutErrLog)); }
#define PRN_VOUT_ERR_NG         ; SvcVoutErrLog.Argc ++; SvcVoutErrTsk_LogPrint(SVC_VOUT_ERR_FLG_LOG_NG  , &(SvcVoutErrLog)); }
#define PRN_VOUT_ERR_NG2        ; SvcVoutErrLog.Argc ++; SvcVoutErrTsk_LogPrint(SVC_VOUT_ERR_FLG_LOG_NG2 , &(SvcVoutErrLog)); }
#define PRN_VOUT_ERR_API        ; SvcVoutErrLog.Argc ++; SvcVoutErrTsk_LogPrint(SVC_VOUT_ERR_FLG_LOG_API , &(SvcVoutErrLog)); }
#define PRN_VOUT_ERR_DBG1       ; SvcVoutErrLog.Argc ++; SvcVoutErrTsk_LogPrint(SVC_VOUT_ERR_FLG_LOG_DBG1, &(SvcVoutErrLog)); }
#define PRN_VOUT_ERR_DBG2       ; SvcVoutErrLog.Argc ++; SvcVoutErrTsk_LogPrint(SVC_VOUT_ERR_FLG_LOG_DBG2, &(SvcVoutErrLog)); }
#define PRN_VOUT_ERR_HDLR       SvcVoutErrTsk_ErrHdlr(__func__, __LINE__, PRetVal);

static void SvcVoutErrTsk_NGLog(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if ((pModule != NULL) && (pFormat != NULL)) {
        AmbaPrint_PrintStr5("\033""[0;31m[%s|NG]: %s", pModule, pFormat, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(&Arg1);
    AmbaMisra_TouchUnused(&Arg2);
}

static void SvcVoutErrTsk_LogPrint(UINT32 LogLevel, SVC_WRAP_PRINT_s *pPrint)
{
    if (pPrint != NULL) {
        if ((SvcVoutErrCtrlFlg & LogLevel) > 0U) {

            switch (LogLevel) {
            case SVC_VOUT_ERR_FLG_LOG_OK :
                pPrint->pProc = SvcLog_OK;
                break;
            case SVC_VOUT_ERR_FLG_LOG_NG :
            case SVC_VOUT_ERR_FLG_LOG_NG2 :
                pPrint->pProc = SvcVoutErrTsk_NGLog;
                break;
            default :
                pPrint->pProc = SvcLog_DBG;
                break;
            }

            SvcWrap_Print(SVC_LOG_VOUT_ERR, pPrint);
        }
    }
}

static UINT32 SvcVoutErrTsk_MutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut)
{
    return AmbaKAL_MutexTake(pMutex, TimeOut);
}

static void SvcVoutErrTsk_MutexGive(AMBA_KAL_MUTEX_t *pMutex)
{
    UINT32 ErrCode = AmbaKAL_MutexGive(pMutex);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }
}

static inline UINT32 SvcVoutErrTsk_BitGet(UINT32 Val)
{
    return ((Val < 32U) ? (UINT32)(0x1UL << Val) : 0U);
}

static void SvcVoutErrTsk_ErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode)
{
    if (pCaller != NULL) {
        if (ErrCode != 0U) {
            if ((SvcVoutErrCtrlFlg & (SVC_VOUT_ERR_FLG_LOG_DBG1 | SVC_VOUT_ERR_FLG_LOG_DBG2)) > 0U) {
                PRN_VOUT_ERR_LOG "Catch ErrCode(0x%08x) @ %s, %d"
                    PRN_VOUT_ERR_ARG_UINT32 ErrCode  PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_CSTR   pCaller  PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_UINT32 CodeLine PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_NG
            }
        }
    }
}

static UINT32 SvcVoutErrTsk_GetSerDesState(UINT32 VoutID, UINT32 SerDesID, UINT32 CurState)
{
    UINT32 State = 0U;

    if (VoutID >= AMBA_DSP_MAX_VOUT_NUM) {
        PRN_VOUT_ERR_LOG "Fail to get serdes state - invalid VoutID(%d)"
            PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_NG
    } else if (SerDesID >= SVC_VOUT_ERR_MAX_SER_NUM) {
        PRN_VOUT_ERR_LOG "Fail to get serdes state - invalid SerDesID(%d)"
            PRN_VOUT_ERR_ARG_UINT32 SerDesID PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_NG
    } else {
        if ( ( SvcVoutErrCtrl.SerDesSelectBits[VoutID] & SvcVoutErrTsk_BitGet(SerDesID) ) == 0U) {
            PRN_VOUT_ERR_LOG "Fail to get serdes state - not supported SerDesID(%d) SerDesSelectBits(0x%x)"
                PRN_VOUT_ERR_ARG_UINT32 SerDesID                                PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrCtrl.SerDesSelectBits[VoutID] PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_NG
        } else {
#ifdef CONFIG_BUILD_MONFRW_SERDES
            // UINT32 CurState = SvcVoutErrCtrl.ErrState[VoutID].LinkState;

            if ( ( CurState & ( 0x100UL << (SerDesID << 2U) ) ) > 0U ) {
                State |= SVC_VOUT_ERR_SER_STAT_LINK_LOCK;
            }
#else
            State |= SVC_VOUT_ERR_SER_STAT_LINK_LOCK;
            AmbaMisra_TouchUnused(&CurState);
#endif
        }
    }

    return State;
}

static void* SvcVoutErrTsk_ErrCtrlTskEntry(void* EntryArg)
{
    ULONG                    ArgVal = 0U;
    UINT32                   PRetVal;
    SVC_VOUT_ERR_CTRL_CMD_t  ErrCtrlCmd;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaSvcWrap_MisraMemcpy(&ArgVal, EntryArg, sizeof(ULONG));

    while (ArgVal != 0xCafeU) {
        AmbaSvcWrap_MisraMemset(&ErrCtrlCmd, 0, sizeof(ErrCtrlCmd));
        PRetVal = AmbaKAL_MsgQueueReceive(&(SvcVoutErrCtrl.ErrCtrlTask.Que), &ErrCtrlCmd, AMBA_KAL_WAIT_FOREVER);
        if (PRetVal == 0U) {

            if (ErrCtrlCmd.CmdID == SVC_VOUT_ERR_CTRL_CMD_LNK_UPD) {
                SvcVoutErrTsk_LinkStateUpd(ErrCtrlCmd.Data[0], ErrCtrlCmd.Data[2]);
            } else if (ErrCtrlCmd.CmdID == SVC_VOUT_ERR_CTRL_CMD_VOUT_UPD) {
                SvcVoutErrTsk_VoutFrzProc(ErrCtrlCmd.Data[0], ErrCtrlCmd.Data[1], ErrCtrlCmd.Data[2]);
            } else if (ErrCtrlCmd.CmdID == SVC_VOUT_ERR_CTRL_CMD_DISP_UPD) {
                SvcVoutErrTsk_VoutFrzFovUpd(ErrCtrlCmd.Data[0], ErrCtrlCmd.Data[1]);
            } else {
                /* Do nothing */
            }
        }

        AmbaMisra_TouchUnused(&ArgVal);
    }

    return NULL;
}

static void SvcVoutErrTsk_RecoverChk(UINT32 VoutID, UINT32 PreLink, UINT32 CurLink)
{
    UINT32 PRetVal;

    if (VoutID >= AMBA_DSP_MAX_VOUT_NUM) {
        PRN_VOUT_ERR_LOG "Fail to process vout recover - invalid VoutID(%d)"
            PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_NG
    } else {
        AMBA_KAL_EVENT_FLAG_INFO_s CurEventInfo;

        AmbaSvcWrap_MisraMemset(&CurEventInfo, 0, sizeof(CurEventInfo));
        PRetVal = AmbaKAL_EventFlagQuery(&(SvcVoutErrCtrl.ErrState[VoutID].VoutStateFlag), &CurEventInfo); PRN_VOUT_ERR_HDLR

        /* vout recover */
        if ((CurEventInfo.CurrentFlags & SVC_VOUT_ERR_VOUT_STATE_RECOVER) == 0U) {
            UINT32 LinkLockSelectBits = 0U, PreLinkLockBits = 0U;
            UINT32 SerDesIdx;

            for (SerDesIdx = 0U; SerDesIdx < SVC_VOUT_ERR_MAX_SER_NUM; SerDesIdx ++) {
                if ((SvcVoutErrCtrl.SerDesSelectBits[VoutID] & SvcVoutErrTsk_BitGet(SerDesIdx)) > 0U) {
                    if ((SvcVoutErrTsk_GetSerDesState(VoutID, SerDesIdx, PreLink) & SVC_VOUT_ERR_SER_STAT_LINK_LOCK) > 0U) {
                        PreLinkLockBits |= SvcVoutErrTsk_BitGet(SerDesIdx);
                    }
                }
            }

            /* Get serdes link lock select bits */
            for (SerDesIdx = 0U; SerDesIdx < SVC_VOUT_ERR_MAX_SER_NUM; SerDesIdx ++) {
                if ((SvcVoutErrCtrl.SerDesSelectBits[VoutID] & SvcVoutErrTsk_BitGet(SerDesIdx)) > 0U) {
                    if ((SvcVoutErrTsk_GetSerDesState(VoutID, SerDesIdx, CurLink) & SVC_VOUT_ERR_SER_STAT_LINK_LOCK) > 0U) {
                        LinkLockSelectBits |= SvcVoutErrTsk_BitGet(SerDesIdx);
                    }
                }
            }

            /* If all serdes link lock ready, ... */
            if ((LinkLockSelectBits == SvcVoutErrCtrl.SerDesSelectBits[VoutID])
              &&(LinkLockSelectBits != PreLinkLockBits)) {
                SVC_VOUT_ERR_RECOVER_CMD_t RecoverCmd;

                AmbaSvcWrap_MisraMemset(&RecoverCmd, 0, sizeof(RecoverCmd));
                RecoverCmd.VoutID = VoutID;
                RecoverCmd.CmdID = SVC_VOUT_ERR_RCV_CMD_VOUT;
                RecoverCmd.SerDesSelectBits = SvcVoutErrCtrl.SerDesSelectBits[VoutID];

                /* Update vout state flag */
                PRetVal = AmbaKAL_EventFlagSet(&(SvcVoutErrCtrl.ErrState[VoutID].VoutStateFlag), SVC_VOUT_ERR_VOUT_STATE_RECOVER); PRN_VOUT_ERR_HDLR

                /* Send cmd */
                PRetVal = AmbaKAL_MsgQueueSend(&(SvcVoutErrCtrl.RecoverTask.Que), &RecoverCmd, 5U);
                if (PRetVal != 0U) {
                    PRN_VOUT_ERR_LOG "Fail to prepare VoutID(%d) recover command"
                        PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_NG
                }
            }

        }
    }
}

static UINT32 SvcVoutErrTsk_RcvProcFpd(SVC_VOUT_ERR_RECOVER_CMD_t *pCmd)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();

    if (pCmd == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to process vout recover - invalid recover cmd" PRN_VOUT_ERR_NG
    } else if (pCmd->VoutID >= AMBA_DSP_MAX_VOUT_NUM) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to process vout recover - invalid VoutID(%d)"
            PRN_VOUT_ERR_ARG_UINT32 pCmd->VoutID PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_NG
    } else if ((SvcVoutErrCtrl.VoutSelectBits & SvcVoutErrTsk_BitGet(pCmd->VoutID)) == 0U) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to process vout recover - Not supported VoutID(%d)"
            PRN_VOUT_ERR_ARG_UINT32 pCmd->VoutID PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_NG
    } else if (pCfg == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to process vout recover - invalid cfg" PRN_VOUT_ERR_NG
    } else {
        UINT32 ProcIdx;
        UINT32 VoutID = pCmd->VoutID;
        SVC_VOUT_ERR_RCV_CTRL_s *pRcvCtrl = (SVC_VOUT_ERR_RCV_CTRL_s *)&(SvcVoutErrCtrl.RecoverTask.RcvCtrl);


        PRN_VOUT_ERR_LOG " " PRN_VOUT_ERR_API
        PRN_VOUT_ERR_LOG "%sRecover VoutID(%d) FPD Start%s"
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_3 PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 VoutID                      PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_API

        PRetVal = SVC_OK;
        for (ProcIdx = SVC_VOUT_ERR_RCV_PROC_DIS_SER_QRY;
             ProcIdx <= SVC_VOUT_ERR_RCV_PROC_ENA_SER_QRY; ProcIdx ++) {

            if ((pRcvCtrl->pSubProc[ProcIdx].Enable > 0U) &&
                (pRcvCtrl->pSubProc[ProcIdx].pFunc != NULL)) {
                if ((pRcvCtrl->ProcSelectBits[pCmd->CmdID] & pRcvCtrl->pSubProc[ProcIdx].ProcBit) > 0U) {
                    PRetVal = (pRcvCtrl->pSubProc[ProcIdx].pFunc)(PRetVal, pCmd, &(pRcvCtrl->pSubProc[ProcIdx]));
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;
                        break;
                    }
                }
            }
        }

        PRetVal = AmbaKAL_EventFlagClear(&(SvcVoutErrCtrl.ErrState[VoutID].VoutStateFlag), SVC_VOUT_ERR_VOUT_STATE_RECOVER); PRN_VOUT_ERR_HDLR

        if (RetVal == 0U) {
            PRN_VOUT_ERR_LOG "%sRecover VoutID(%d) FPD done%s"
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_3 PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 VoutID                      PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_API
        }

        AmbaMisra_TouchUnused(pRcvCtrl);
    }

    AmbaMisra_TouchUnused(pCmd);

    return RetVal;
}

static void SvcVoutErrTsk_RcvProcVout(SVC_VOUT_ERR_RECOVER_CMD_t *pCmd)
{
    UINT32 PRetVal;
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();

    if (pCmd == NULL) {
        PRN_VOUT_ERR_LOG "Fail to process vout recover - invalid recover cmd" PRN_VOUT_ERR_NG
    } else if (pCmd->VoutID >= AMBA_DSP_MAX_VOUT_NUM) {
        PRN_VOUT_ERR_LOG "Fail to process vout recover - invalid VoutID(%d)"
            PRN_VOUT_ERR_ARG_UINT32 pCmd->VoutID PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_NG
    } else if ((SvcVoutErrCtrl.VoutErrSelectBits & SvcVoutErrTsk_BitGet(pCmd->VoutID)) == 0U) {
        PRN_VOUT_ERR_LOG "Fail to process vout recover - Not supported VoutID(%d)"
            PRN_VOUT_ERR_ARG_UINT32 pCmd->VoutID PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_NG
    } else if (pCfg == NULL) {
        PRN_VOUT_ERR_LOG "Fail to process vout recover - invalid cfg" PRN_VOUT_ERR_NG
    } else {
        UINT32 ActualFlag = 0U;
        UINT32 ProcIdx;
        UINT32 VoutID = pCmd->VoutID;
        SVC_VOUT_ERR_RCV_CTRL_s *pRcvCtrl = (SVC_VOUT_ERR_RCV_CTRL_s *)&(SvcVoutErrCtrl.RecoverTask.RcvCtrl);

        PRN_VOUT_ERR_LOG " " PRN_VOUT_ERR_API
        PRN_VOUT_ERR_LOG "%sRecover VoutID(%d) Start%s"
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_3 PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 VoutID                      PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_API

        /* Lock vout osd update */
        SvcVoutErrTsk_OsdUpdateLock(VoutID, 1U);

        PRetVal = SVC_OK;
        for (ProcIdx = 0U; ProcIdx < SVC_VOUT_ERR_RCV_PROC_NUM; ProcIdx ++) {

            if ((pRcvCtrl->pSubProc[ProcIdx].Enable > 0U) &&
                (pRcvCtrl->pSubProc[ProcIdx].pFunc != NULL)) {
                if ((pRcvCtrl->ProcSelectBits[pCmd->CmdID] & pRcvCtrl->pSubProc[ProcIdx].ProcBit) > 0U) {
                    PRetVal = (pRcvCtrl->pSubProc[ProcIdx].pFunc)(PRetVal, pCmd, &(pRcvCtrl->pSubProc[ProcIdx]));
                }
            }
        }

        if (PRetVal == 0U) {
            /* Wait vout state */
            PRetVal = AmbaKAL_EventFlagGet(&(SvcVoutErrCtrl.ErrState[VoutID].VoutStateFlag),
                                           SVC_VOUT_ERR_VOUT_STATE_START,
                                           AMBA_KAL_FLAGS_ALL,
                                           AMBA_KAL_FLAGS_CLEAR_NONE,
                                           &ActualFlag,
                                           SVC_VOUT_ERR_VOUT_RECOVER_TOUT);
            if (PRetVal != 0U) {
                PRN_VOUT_ERR_LOG "Wait VoutID(%d) ISR timeout(%u). ActFlag(0x%x)"
                    PRN_VOUT_ERR_ARG_UINT32 VoutID                         PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_UINT32 SVC_VOUT_ERR_VOUT_RECOVER_TOUT PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_UINT32 ActualFlag                     PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_NG
            } else {

                PRetVal = AmbaKAL_EventFlagClear(&(SvcVoutErrCtrl.ErrState[VoutID].VoutStateFlag), SVC_VOUT_ERR_VOUT_STATE_TOUT); PRN_VOUT_ERR_HDLR

                PRN_VOUT_ERR_LOG "%sRecover VoutID(%d) success%s"
                    PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_3 PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_UINT32 VoutID                      PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_API
            }
        }

        /* UnLock vout osd update */
        SvcVoutErrTsk_OsdUpdateLock(VoutID, 0U);

        PRetVal = AmbaKAL_EventFlagClear(&(SvcVoutErrCtrl.ErrState[VoutID].VoutStateFlag), SVC_VOUT_ERR_VOUT_STATE_RECOVER); PRN_VOUT_ERR_HDLR

        AmbaMisra_TouchUnused(pRcvCtrl);
    }

    AmbaMisra_TouchUnused(pCmd);
}

static void SvcVoutErrTsk_RcvProcSerDesLink(SVC_VOUT_ERR_RECOVER_CMD_t *pCmd)
{
    SvcVoutErrTsk_RcvProcVout(pCmd);
}

static void* SvcVoutErrTsk_RcvTaskEntry(void* EntryArg)
{
    UINT32 PRetVal;
    SVC_VOUT_ERR_RECOVER_CMD_t RecoverCmd;
    ULONG ArgVal = 0U;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaSvcWrap_MisraMemcpy(&ArgVal, EntryArg, sizeof(ULONG));

    while (ArgVal != 0xCafeU) {
        AmbaSvcWrap_MisraMemset(&RecoverCmd, 0, sizeof(RecoverCmd));
        PRetVal = AmbaKAL_MsgQueueReceive(&(SvcVoutErrCtrl.RecoverTask.Que), &RecoverCmd, AMBA_KAL_WAIT_FOREVER);
        if (PRetVal == 0U) {

            if (RecoverCmd.CmdID == SVC_VOUT_ERR_RCV_CMD_VOUT) {
                SvcVoutErrTsk_RcvProcVout(&RecoverCmd);
            } else if (RecoverCmd.CmdID == SVC_VOUT_ERR_RCV_CMD_SERDES) {
                SvcVoutErrTsk_RcvProcSerDesLink(&RecoverCmd);
            } else {
                /* Do nothing */
            }
        }
        
        AmbaMisra_TouchUnused(&ArgVal);
    }

    return NULL;
}

static void SvcVoutErrTsk_Info(void)
{
    UINT32 PRetVal;

    if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_INIT) > 0U) {
        UINT32 Idx;
        UINT32 TmpU32;
        AMBA_KAL_EVENT_FLAG_INFO_s CurEvtInfo;

        PRN_VOUT_ERR_LOG " " PRN_VOUT_ERR_API
        PRN_VOUT_ERR_LOG "====== %sVout Err Control%s ======"
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_0 PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_API
        PRN_VOUT_ERR_LOG "               Name : %s"
            PRN_VOUT_ERR_ARG_CSTR   SvcVoutErrCtrl.Name PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_API
        PRN_VOUT_ERR_LOG "     VoutSelectBits : 0x%08x"
            PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrCtrl.VoutSelectBits PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_API
        PRN_VOUT_ERR_LOG "  VoutErrSelectBits : 0x%08x"
            PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrCtrl.VoutErrSelectBits PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_API

        TmpU32 = 0U;
        for (Idx = 0U; Idx < AMBA_DSP_MAX_VOUT_NUM; Idx ++) {
            if ((SvcVoutErrCtrl.VoutSelectBits & SvcVoutErrTsk_BitGet(Idx)) > 0U) {
                if (TmpU32 == 0U) {
                    PRN_VOUT_ERR_LOG "   SerDesSelectBits : 0x%08x VoutID(%d)"
                        PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrCtrl.SerDesSelectBits[Idx] PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 Idx                                  PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_API
                } else {
                    PRN_VOUT_ERR_LOG "                      0x%08x VoutID(%d)"
                        PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrCtrl.SerDesSelectBits[Idx] PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 Idx                                  PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_API
                }
                TmpU32 += 1U;
            }
        }

        PRN_VOUT_ERR_LOG "  ErrState VoutID |  VoutState |  LinkState" PRN_VOUT_ERR_API
        for (Idx = 0U; Idx < AMBA_DSP_MAX_VOUT_NUM; Idx ++) {
            if ((SvcVoutErrCtrl.VoutSelectBits & SvcVoutErrTsk_BitGet(Idx)) > 0U) {
                AmbaSvcWrap_MisraMemset(&CurEvtInfo, 0, sizeof(CurEvtInfo));
                PRetVal = AmbaKAL_EventFlagQuery(&(SvcVoutErrCtrl.ErrState[Idx].VoutStateFlag), &CurEvtInfo); PRN_VOUT_ERR_HDLR
                PRN_VOUT_ERR_LOG "               %02d | 0x%08x | 0x%08x"
                    PRN_VOUT_ERR_ARG_UINT32 Idx                                    PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_UINT32 CurEvtInfo.CurrentFlags                PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrCtrl.ErrState[Idx].LinkState PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_API
            }
        }
        PRN_VOUT_ERR_LOG " " PRN_VOUT_ERR_API
        PRN_VOUT_ERR_LOG "  LinkChgCtrl :" PRN_VOUT_ERR_API
        PRN_VOUT_ERR_LOG "    TimerPeriod : %d "
            PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrCtrl.LinkChgCtrl.TimerPeriod PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_API
        PRN_VOUT_ERR_LOG "     SelectBits : 0x%x"
            PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrCtrl.LinkChgCtrl.SelectBits PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_API

        PRN_VOUT_ERR_LOG "     VoutID |  ChgState  |  CurState  | Threshold" PRN_VOUT_ERR_API
        for (Idx = 0U; Idx < AMBA_DSP_MAX_VOUT_NUM; Idx ++) {
            PRN_VOUT_ERR_LOG "         %02d | 0x%08x | 0x%08x | %d"
                PRN_VOUT_ERR_ARG_UINT32 Idx                                         PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrCtrl.LinkChgCtrl.ChgState[Idx]    PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrCtrl.LinkChgCtrl.CurState[Idx]    PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrCtrl.LinkChgCtrl.ThersholdMS[Idx] PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_API
        }
    }
}

static UINT32 SvcVoutErrTsk_Config(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();

    if (pCfg == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to create vout err control - invalid resolution configuration!" PRN_VOUT_ERR_NG
    } else {
        UINT32 DispIdx, VoutIdx;
        UINT32 DevMode;
        const SVC_VOUT_ERR_DEV_INFO_s *pDevInfo;
        SVC_VOUT_ERR_STATE_s *pState;

        SvcVoutErrTsk_DevInfoInit();

        for (DispIdx = 0U; DispIdx < pCfg->DispNum; DispIdx ++) {
            if (DispIdx < AMBA_DSP_MAX_VOUT_NUM) {
                VoutIdx = pCfg->DispStrm[DispIdx].VoutID;
                if (VoutIdx < AMBA_DSP_MAX_VOUT_NUM) {
                    SvcVoutErrCtrl.VoutSelectBits |= SvcVoutErrTsk_BitGet(VoutIdx);
                    pDevInfo = SvcVoutErrTsk_DevInfoGet(VoutIdx);
                    if (pDevInfo != NULL) {
                        if (pDevInfo->EnableErrCtrl > 0U) {

                            AmbaMisra_TypeCast(&(DevMode), &(pCfg->DispStrm[DispIdx].DevMode)); DevMode &= 0xFFUL;
                            DevMode &= ~(0x80UL);

                            if (DevMode < pDevInfo->DevModeNum) {
                                SvcVoutErrCtrl.SerDesSelectBits[VoutIdx] = pDevInfo->DevSerDesSelectBits[DevMode];
                                SvcVoutErrCtrl.VoutErrSelectBits |= SvcVoutErrTsk_BitGet(VoutIdx);

                                PRN_VOUT_ERR_LOG "Configure VoutID(%d) SerDesSelectBits 0x%08x"
                                    PRN_VOUT_ERR_ARG_UINT32 VoutIdx                                  PRN_VOUT_ERR_ARG_POST
                                    PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrCtrl.SerDesSelectBits[VoutIdx] PRN_VOUT_ERR_ARG_POST
                                PRN_VOUT_ERR_API
                            } else {
                                PRN_VOUT_ERR_LOG "VoutID(%d) DevMode(%d) does not ready to support it."
                                    PRN_VOUT_ERR_ARG_UINT32 VoutIdx PRN_VOUT_ERR_ARG_POST
                                    PRN_VOUT_ERR_ARG_UINT32 DevMode PRN_VOUT_ERR_ARG_POST
                                PRN_VOUT_ERR_API
                            }

                        } else {
                            PRN_VOUT_ERR_LOG "Disable VoutID(%d) - %s Err Control"
                                PRN_VOUT_ERR_ARG_UINT32 VoutIdx           PRN_VOUT_ERR_ARG_POST
                                PRN_VOUT_ERR_ARG_CSTR   pDevInfo->DevName PRN_VOUT_ERR_ARG_POST
                            PRN_VOUT_ERR_DBG1
                        }
                    } else {
                        PRN_VOUT_ERR_LOG "VoutID(%d) device does not ready to support it."
                            PRN_VOUT_ERR_ARG_UINT32 VoutIdx PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_API
                    }
                }
            }
        }

        /* Configure err state */
        for (VoutIdx = 0U; VoutIdx < AMBA_DSP_MAX_VOUT_NUM; VoutIdx ++) {
            if ((SvcVoutErrCtrl.VoutSelectBits & SvcVoutErrTsk_BitGet(VoutIdx)) > 0U) {
                pState = &(SvcVoutErrCtrl.ErrState[VoutIdx]);

                // Create event flag
                PRetVal = AmbaKAL_EventFlagCreate(&(pState->VoutStateFlag), SvcVoutErrCtrl.Name);
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    PRN_VOUT_ERR_LOG "Fail to create vout err ctrl - create VoutID(%d) event flag fail!"
                        PRN_VOUT_ERR_ARG_UINT32 VoutIdx PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_NG
                } else {
                    PRetVal = AmbaKAL_EventFlagClear(&(pState->VoutStateFlag), 0xFFFFFFFFU); PRN_VOUT_ERR_HDLR
                    pState->LinkState = 0xFFFFFFFFU;
#ifdef CONFIG_BUILD_MONFRW_SERDES
                    pState->LinkUpdEnable = SVC_VOUT_ERR_LNK_UPD_ON;
#else
                    pState->LinkUpdEnable = SVC_VOUT_ERR_LNK_UPD_OFF;
#endif
                }
            }

            if (RetVal != SVC_OK) {
                break;
            }
        }

        /* Configure vout freeze setting */
        if (RetVal == SVC_OK) {
            RetVal = SvcVoutErrTsk_VoutFrzConfig();
            if (RetVal == SVC_OK) {
                SvcVoutErrCtrlFlg |= SVC_VOUT_ERR_FLG_VOUT_FRZ_INIT;
            }
        }

        if (RetVal == SVC_OK) {
            RetVal = SvcVoutErrTsk_OsdCreate();
            if (RetVal != 0U) {
                PRN_VOUT_ERR_LOG "Fail to create vout err control - create osd ctrl fail" PRN_VOUT_ERR_NG
            } else {
                PRN_VOUT_ERR_LOG "Successful to create vout err osd ctrl" PRN_VOUT_ERR_DBG1
            }
        }
    }

    return RetVal;
}

static UINT32 SvcVoutErrTsk_VoutFrzConfig(void)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    if (pResCfg == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to config vout freeze - invalid resolution configuration!" PRN_VOUT_ERR_NG
    } else {
        SVC_VOUT_ERR_VOUT_FRZ_CTRL_s *pVoutFrz;
        UINT32 DispIdx, ChIdx, FovID;

        for (DispIdx = 0U; DispIdx < pResCfg->DispNum; DispIdx ++) {
            if (pResCfg->DispStrm[DispIdx].VoutID < AMBA_DSP_MAX_VOUT_NUM) {
                pVoutFrz = &(SvcVoutErrCtrl.VoutFrzCtrl[pResCfg->DispStrm[DispIdx].VoutID]);

                AmbaSvcWrap_MisraMemset(pVoutFrz, 0, sizeof(SVC_VOUT_ERR_VOUT_FRZ_CTRL_s));

                for (ChIdx = 0U; ChIdx < pResCfg->DispStrm[DispIdx].StrmCfg.NumChan; ChIdx ++) {
                    FovID = pResCfg->DispStrm[DispIdx].StrmCfg.ChanCfg[ChIdx].FovId;
                    if (FovID < AMBA_DSP_MAX_VIEWZONE_NUM) {
                        pVoutFrz->FovBits |= SvcVoutErrTsk_BitGet(FovID);
                    }
                }

                pVoutFrz->State |= SVC_VOUT_ERR_VOUT_FRZ_ENABLE;

                SvcVoutErrTsk_VoutFrzMsgConfig(pResCfg->DispStrm[DispIdx].VoutID);
            }
        }
    }

    return RetVal;
}

static void SvcVoutErrTsk_LinkStateChange(UINT32 VoutID, UINT32 CurLinkState)
{
    UINT32 PRetVal;

    if (VoutID < AMBA_DSP_MAX_VOUT_NUM) {
        if ((SvcVoutErrCtrl.ErrState[VoutID].LinkUpdEnable & SVC_VOUT_ERR_LNK_UPD_ON) > 0U) {
            SVC_VOUT_ERR_LINK_CHG_CTRL_s *pCtrl = &(SvcVoutErrCtrl.LinkChgCtrl);

            if ((pCtrl->SelectBits & SvcVoutErrTsk_BitGet(VoutID)) > 0U) {
                pCtrl->CurState[VoutID] = CurLinkState;

                PRN_VOUT_ERR_LOG "Change VoutID(%s%d%s) Link value. 0x%08x"
                    PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_1 PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_UINT32 VoutID                      PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_UINT32 CurLinkState                PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_DBG1
            } else {
                UINT32 TimeTicks = 0U;
                PRetVal = AmbaKAL_GetSysTickCount(&TimeTicks); PRN_VOUT_ERR_HDLR

                pCtrl->ThersholdMS[VoutID] = TimeTicks + SVC_VOUT_ERR_LNK_STA_CHG_THD;
                pCtrl->ChgState[VoutID] = CurLinkState;
                pCtrl->CurState[VoutID] = CurLinkState;
                pCtrl->SelectBits |= SvcVoutErrTsk_BitGet(VoutID);

                PRN_VOUT_ERR_LOG "Update VoutID(%s%d%s) Link State. 0x%08x, ThresholdMS: %d"
                    PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_1 PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_UINT32 VoutID                      PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_UINT32 CurLinkState                PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_UINT32 pCtrl->ThersholdMS[VoutID]  PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_DBG1
            }
        } else {

            PRN_VOUT_ERR_LOG "VoutID(%s%d%s) SerDes state bkp update. LinkLock %s0x%08x%s -> %s0x%08x%s!"
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_1                  PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 VoutID                                       PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END                      PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_0                  PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrCtrl.ErrState[VoutID].LinkBkpState PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END                      PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_0                  PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 CurLinkState                                 PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END                      PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_DBG1

            SvcVoutErrCtrl.ErrState[VoutID].LinkBkpState = CurLinkState;
        }
    }
}

static void SvcVoutErrTsk_LinkStateUpd(UINT32 VoutID, UINT32 CurLinkState)
{
    if (VoutID < AMBA_DSP_MAX_VOUT_NUM) {
        UINT32 OriLinkState = SvcVoutErrCtrl.ErrState[VoutID].LinkState;

        PRN_VOUT_ERR_LOG "VoutID(%s%d%s) link state update %s0x%08x%s -> %s0x%08x%s!"
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_1               PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 VoutID                                    PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END                   PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_0               PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrCtrl.ErrState[VoutID].LinkState PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END                   PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_0               PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 CurLinkState                              PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END                   PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_API

        SvcVoutErrCtrl.ErrState[VoutID].LinkState = CurLinkState;

        if (OriLinkState != 0xFFFFFFFFU) {
            SvcVoutErrTsk_RecoverChk(VoutID, OriLinkState, SvcVoutErrCtrl.ErrState[VoutID].LinkState);
        }
    }
}

static void SvcVoutErrTsk_LinkStateUpdEna(UINT32 VoutID, UINT32 Enable)
{
#ifdef CONFIG_BUILD_MONFRW_SERDES
    if (VoutID < AMBA_DSP_MAX_VOUT_NUM) {
        UINT32 OriLinkUpdEna = SvcVoutErrCtrl.ErrState[VoutID].LinkUpdEnable;

        SvcVoutErrCtrl.ErrState[VoutID].LinkUpdEnable = Enable;

        if (Enable != OriLinkUpdEna) {
            if (Enable > 0U) {
                SvcVoutErrCtrl.ErrState[VoutID].LinkBkpState = SvcVoutErrCtrl.ErrState[VoutID].LinkState;
            } else {
                SvcVoutErrTsk_LinkStateChange(VoutID, SvcVoutErrCtrl.ErrState[VoutID].LinkBkpState);
                SvcVoutErrCtrl.ErrState[VoutID].LinkBkpState = 0U;
            }
        }

        PRN_VOUT_ERR_LOG "VoutID(%s%d%s) SerDes state update %s%s%s!"
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_1                      PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 VoutID                                           PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_CSTR    SVC_LOG_VOUT_ERR_HL_END                         PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_STR                          PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_CSTR   ( (Enable > 0U) ? ( "enable" ) : ( "disable" ) ) PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END                          PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_API
    }
#else
    PRN_VOUT_ERR_LOG "Link Update mechanism does not active"
        PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_ARG_UINT32 Enable PRN_VOUT_ERR_ARG_POST
    PRN_VOUT_ERR_DBG1
#endif
}

static void SvcVoutErrTsk_TimerHandler(UINT32 EntryArg)
{
    UINT32 PRetVal;
    SVC_VOUT_ERR_LINK_CHG_CTRL_s *pCtrl = &(SvcVoutErrCtrl.LinkChgCtrl);

    AmbaMisra_TouchUnused(&EntryArg);

    if (pCtrl->SelectBits > 0U) {
        UINT32 VoutIdx, CurSysTicks = 0U;
        SVC_VOUT_ERR_CTRL_CMD_t ErrCtrlCmd;

        PRetVal = AmbaKAL_GetSysTickCount(&CurSysTicks); PRN_VOUT_ERR_HDLR

        for (VoutIdx = 0U; VoutIdx < AMBA_DSP_MAX_VOUT_NUM; VoutIdx ++) {
            if ((pCtrl->SelectBits & SvcVoutErrTsk_BitGet(VoutIdx)) > 0U) {
                if (pCtrl->CurState[VoutIdx] == pCtrl->ChgState[VoutIdx]) {
                    if (CurSysTicks >= pCtrl->ThersholdMS[VoutIdx]) {
                        pCtrl->SelectBits &= ~SvcVoutErrTsk_BitGet(VoutIdx);

                        pCtrl->ThersholdMS[VoutIdx] = 0U;

                        AmbaSvcWrap_MisraMemset(&ErrCtrlCmd, 0, sizeof(ErrCtrlCmd));
                        ErrCtrlCmd.CmdID   = SVC_VOUT_ERR_CTRL_CMD_LNK_UPD;
                        ErrCtrlCmd.Data[0] = VoutIdx;
                        ErrCtrlCmd.Data[2] = pCtrl->ChgState[VoutIdx];
                        PRetVal = AmbaKAL_MsgQueueSend(&(SvcVoutErrCtrl.ErrCtrlTask.Que), &ErrCtrlCmd, AMBA_KAL_NO_WAIT); PRN_VOUT_ERR_HDLR

                        pCtrl->ChgState[VoutIdx] = 0xFFFFFFFFU;
                        pCtrl->CurState[VoutIdx] = 0xFFFFFFFFU;
                    }
                } else {
                    pCtrl->ThersholdMS[VoutIdx] = CurSysTicks + SVC_VOUT_ERR_LNK_STA_CHG_THD;
                    pCtrl->ChgState[VoutIdx] = pCtrl->CurState[VoutIdx];
                }
            }
        }

    }
}

static void SvcVoutErrTsk_VoutFrzFovUpd(UINT32 VoutID, UINT32 CurBits)
{
    if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_VOUT_FRZ_INIT) == 0U) {
        PRN_VOUT_ERR_LOG "Fail to proc vout freeze - initial first!" PRN_VOUT_ERR_NG2
    } else if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_VOUT_FRZ_OFF) > 0U) {
        PRN_VOUT_ERR_LOG "VoutID(%d) vout freeze checking has been disable"
            PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_DBG2
    } else if (0U == SvcDSP_IsBootDone()) {
        PRN_VOUT_ERR_LOG "Wait boot done to process vout freeze checking" PRN_VOUT_ERR_DBG2
    } else if (VoutID >= AMBA_DSP_MAX_VOUT_NUM) {
        PRN_VOUT_ERR_LOG "Fail to proc vout freeze - initial VoutID(%d)"
            PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_NG2
    } else if ((SvcVoutErrCtrl.VoutFrzCtrl[VoutID].State & SVC_VOUT_ERR_VOUT_FRZ_ENABLE) == 0U) {
        PRN_VOUT_ERR_LOG "VoutID(%d) vout freeze is disable"
            PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_DBG2
    } else {
        SvcVoutErrCtrl.VoutFrzCtrl[VoutID].CurBits |= CurBits;

        PRN_VOUT_ERR_LOG "VoutID(%d) vout freeze bits update 0x%X"
            PRN_VOUT_ERR_ARG_UINT32 VoutID                                     PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrCtrl.VoutFrzCtrl[VoutID].CurBits PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_DBG2
    }
}

static void SvcVoutErrTsk_VoutFrzProc(UINT32 VoutID, UINT32 LumaCheckSum, UINT32 ChromaCheckSum)
{
    AMBA_KAL_EVENT_FLAG_INFO_s CurFlagInfo;

    AmbaSvcWrap_MisraMemset(&CurFlagInfo, 0, sizeof(CurFlagInfo));

    if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_VOUT_FRZ_INIT) == 0U) {
        PRN_VOUT_ERR_LOG "Fail to proc vout freeze - initial first!" PRN_VOUT_ERR_NG2
    } else if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_VOUT_FRZ_OFF) > 0U) {
        PRN_VOUT_ERR_LOG "VoutID(%d) vout freeze checking has been disable"
            PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_DBG2
    } else if (0U == SvcDSP_IsBootDone()) {
        PRN_VOUT_ERR_LOG "Wait boot done to process vout freeze checking" PRN_VOUT_ERR_DBG2
    } else if (VoutID >= AMBA_DSP_MAX_VOUT_NUM) {
        PRN_VOUT_ERR_LOG "Fail to proc vout freeze - initial VoutID(%d)"
            PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_NG2
    } else if ((SvcVoutErrCtrl.VoutFrzCtrl[VoutID].State & SVC_VOUT_ERR_VOUT_FRZ_ENABLE) == 0U) {
        PRN_VOUT_ERR_LOG "VoutID(%d) vout freeze is disable"
            PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_DBG2
    } else if (SvcVoutErrCtrl.VoutFrzCtrl[VoutID].FovBits != SvcVoutErrCtrl.VoutFrzCtrl[VoutID].CurBits) {
        PRN_VOUT_ERR_LOG "VoutID(%d) update does not ready! FovBits(0x%X) != CurBits(0x%X)"
            PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrCtrl.VoutFrzCtrl[VoutID].FovBits PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrCtrl.VoutFrzCtrl[VoutID].CurBits PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_DBG2
    } else if ((LumaCheckSum == 0U) || (ChromaCheckSum == 0U)) {
        PRN_VOUT_ERR_LOG "VoutID(%d) invalid check sum! Luma(0x%08X) Chroma(0x%08X)"
            PRN_VOUT_ERR_ARG_UINT32 VoutID         PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 LumaCheckSum   PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 ChromaCheckSum PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_DBG2
    } else {
        UINT32 FreezeDetect;
        SVC_VOUT_ERR_VOUT_FRZ_MSG_UNIT_s MsgUnit;

        SvcVoutErrCtrl.VoutFrzCtrl[VoutID].CurBits = 0U;

        AmbaSvcWrap_MisraMemset(&MsgUnit, 0, sizeof(MsgUnit));

        if ((LumaCheckSum == SvcVoutErrCtrl.VoutFrzCtrl[VoutID].PreLumaCheckSum) &&
            (ChromaCheckSum == SvcVoutErrCtrl.VoutFrzCtrl[VoutID].PreChromaCheckSum)) {
            FreezeDetect = 1U;
            MsgUnit.MsgID = SVC_VOUT_ERR_FRZ_MSG_ID_FRZE_HIT;
        } else {
            FreezeDetect = 0U;
            MsgUnit.MsgID = SVC_VOUT_ERR_FRZ_MSG_ID_FRZE_CHK;
        }

        MsgUnit.Data[0] = LumaCheckSum;                                         MsgUnit.Data[0] &= 0xFFFFFFFFULL;
        MsgUnit.Data[1] = ChromaCheckSum;                                       MsgUnit.Data[1] &= 0xFFFFFFFFULL;
        MsgUnit.Data[2] = SvcVoutErrCtrl.VoutFrzCtrl[VoutID].PreLumaCheckSum;   MsgUnit.Data[2] &= 0xFFFFFFFFULL;
        MsgUnit.Data[3] = SvcVoutErrCtrl.VoutFrzCtrl[VoutID].PreChromaCheckSum; MsgUnit.Data[3] &= 0xFFFFFFFFULL;
        SvcVoutErrTsk_VoutFrzMsgSned(VoutID, &MsgUnit);

        PRN_VOUT_ERR_LOG "VoutID(%s%d%s) vout freeze detect %d"
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_1 PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 VoutID                      PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 FreezeDetect                PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_DBG2

        if (FreezeDetect > 0U) {
            SvcVoutErrCtrl.VoutFrzCtrl[VoutID].FrzCnt ++;
            if (SvcVoutErrCtrl.VoutFrzCtrl[VoutID].FrzCnt >= SVC_VOUT_ERR_YUV_FRZ_CHG_THD) {
                // Pause vout freeze msg log
                SvcVoutErrTsk_VoutFrzMsgEnable(VoutID, 0U);

                // Update freeze error state
                SvcVoutErrTsk_FreezeUpd(VoutID, 1U);
            }
        } else {
            SvcVoutErrCtrl.VoutFrzCtrl[VoutID].FrzCnt = 0U;
        }

        SvcVoutErrCtrl.VoutFrzCtrl[VoutID].PreLumaCheckSum   = LumaCheckSum;
        SvcVoutErrCtrl.VoutFrzCtrl[VoutID].PreChromaCheckSum = ChromaCheckSum;
    }
}

static void SvcVoutErrTsk_VoutFrzMsgConfig(UINT32 VoutID)
{
    UINT32 PRetVal;

    if (VoutID >= AMBA_DSP_MAX_VOUT_NUM) {
        PRN_VOUT_ERR_LOG "Fail to config vout frz msg - invalid VoutID(%d)!"
            PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_NG
    } else {
        SVC_VOUT_ERR_VOUT_FRZ_MSG_s *pFrzMsg = &(SvcVoutErrCtrl.VoutFrzCtrl[VoutID].FrzMsg);

        AmbaSvcWrap_MisraMemset(pFrzMsg, 0, sizeof(SVC_VOUT_ERR_VOUT_FRZ_MSG_s));

        /* %s_%s */
                AmbaUtility_StringCopy(pFrzMsg->Name, sizeof(pFrzMsg->Name), SvcVoutErrCtrl.Name);
              AmbaUtility_StringAppend(pFrzMsg->Name, (UINT32)sizeof(pFrzMsg->Name), "_");
              AmbaUtility_StringAppend(pFrzMsg->Name, (UINT32)sizeof(pFrzMsg->Name), "FrzMsg_");
        AmbaUtility_StringAppendUInt32(pFrzMsg->Name, (UINT32)sizeof(pFrzMsg->Name), VoutID, 10U);

        PRetVal = AmbaKAL_MutexCreate(&(pFrzMsg->Mutex), pFrzMsg->Name);
        if (PRetVal != 0U) {
            PRN_VOUT_ERR_LOG "Fail to config VoutID(%d) frz msg - create mutex fail! ErrCode(0x%08X)"
                PRN_VOUT_ERR_ARG_UINT32 VoutID  PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 PRetVal PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_NG
        } else {

            pFrzMsg->MsgQueIdx = SVC_VOUT_ERR_VOUT_FRZ_MSG_NUM - 1U;

            PRN_VOUT_ERR_LOG "Success to config VoutID(%d) frz msg."
                PRN_VOUT_ERR_ARG_UINT32 VoutID  PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_DBG1
            pFrzMsg->State |= SVC_VOUT_ERR_VOUT_FRZ_MSG_INIT;
        }
    }
}

static void SvcVoutErrTsk_VoutFrzMsgEnable(UINT32 VoutID, UINT32 Enable)
{
    if (VoutID >= AMBA_DSP_MAX_VOUT_NUM) {
        PRN_VOUT_ERR_LOG "Fail to on/off vout frz msg - invalid VoutID(%d)!"
            PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_NG2
    } else if ((SvcVoutErrCtrl.VoutFrzCtrl[VoutID].FrzMsg.State & SVC_VOUT_ERR_VOUT_FRZ_MSG_INIT) == 0U) {
        PRN_VOUT_ERR_LOG "Fail to on/off VoutID(%d) frz msg - initial first!"
            PRN_VOUT_ERR_ARG_UINT32 VoutID      PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_NG2
    } else {
        if (Enable > 0U) {
            SvcVoutErrCtrl.VoutFrzCtrl[VoutID].FrzMsg.State &= ~SVC_VOUT_ERR_VOUT_FRZ_MSG_OFF;
        } else {
            SvcVoutErrCtrl.VoutFrzCtrl[VoutID].FrzMsg.State |= SVC_VOUT_ERR_VOUT_FRZ_MSG_OFF;
        }
    }
}

static void SvcVoutErrTsk_VoutFrzMsgSned(UINT32 VoutID, const SVC_VOUT_ERR_VOUT_FRZ_MSG_UNIT_s *pMsg)
{
    if (VoutID >= AMBA_DSP_MAX_VOUT_NUM) {
        PRN_VOUT_ERR_LOG "Fail to send vout frz msg - invalid VoutID(%d)!"
            PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_NG2
    } else if (pMsg == NULL) {
        PRN_VOUT_ERR_LOG "Fail to send VoutID(%d) frz msg - input msg should not null!"
            PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_NG2
    } else if ((SvcVoutErrCtrl.VoutFrzCtrl[VoutID].State & SVC_VOUT_ERR_VOUT_FRZ_ENABLE) == 0U) {
        PRN_VOUT_ERR_LOG "VoutID(%d) frz detection is not enable!"
            PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_DBG2
    } else if (0U != SvcVoutErrTsk_MutexTake(&(SvcVoutErrCtrl.VoutFrzCtrl[VoutID].FrzMsg.Mutex), AMBA_KAL_NO_WAIT)) {
        PRN_VOUT_ERR_LOG "Fail to send VoutID(%d) frz msg - take mutex fail! Pass MsgID(%d)"
            PRN_VOUT_ERR_ARG_UINT32 VoutID      PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 pMsg->MsgID PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_NG2
    } else {
        SVC_VOUT_ERR_VOUT_FRZ_MSG_s *pFrzMsg = &(SvcVoutErrCtrl.VoutFrzCtrl[VoutID].FrzMsg);

        if ((pFrzMsg->State & SVC_VOUT_ERR_VOUT_FRZ_MSG_OFF) == 0U) {
            UINT32 CurTimeStamp = 0U;

            if (0U != AmbaKAL_GetSysTickCount(&CurTimeStamp)) {
                CurTimeStamp = 0U;
            }

            pFrzMsg->MsgQueIdx ++;
            if (pFrzMsg->MsgQueIdx >= SVC_VOUT_ERR_VOUT_FRZ_MSG_NUM) {
                pFrzMsg->MsgQueIdx = 0U;
            }

            if (0U == AmbaWrap_memcpy(&(pFrzMsg->MsgQue[pFrzMsg->MsgQueIdx]), pMsg, sizeof(SVC_VOUT_ERR_VOUT_FRZ_MSG_UNIT_s))) {
                pFrzMsg->MsgQue[pFrzMsg->MsgQueIdx].TimeStamp = CurTimeStamp;
            }
        }

        SvcVoutErrTsk_MutexGive(&(SvcVoutErrCtrl.VoutFrzCtrl[VoutID].FrzMsg.Mutex));
    }
}

static void SvcVoutErrTsk_VoutFrzMsgDump(UINT32 VoutID)
{
    if (VoutID < AMBA_DSP_MAX_VOUT_NUM) {
        if (0U == SvcVoutErrTsk_MutexTake(&(SvcVoutErrCtrl.VoutFrzCtrl[VoutID].FrzMsg.Mutex), AMBA_KAL_WAIT_FOREVER)) {
            const SVC_VOUT_ERR_VOUT_FRZ_MSG_s *pFrzMsg = &(SvcVoutErrCtrl.VoutFrzCtrl[VoutID].FrzMsg);
            const SVC_VOUT_ERR_VOUT_FRZ_MSG_UNIT_s *pCurMsg;
            UINT32 CurIdx = pFrzMsg->MsgQueIdx;
            static UINT32 PreTimeStamp[SVC_VOUT_ERR_FRZ_MSG_ID_NUM] = {0};
            UINT32 TimeDiff = 0U, TimeStampID;

            PRN_VOUT_ERR_LOG " " PRN_VOUT_ERR_API
            PRN_VOUT_ERR_LOG "------ VOUT(%s%d%s) FREEZE MSG - %sBEGIN%s ------"
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_1 PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 VoutID                      PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_STR     PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_API

            do {
                CurIdx ++;
                if (CurIdx >= SVC_VOUT_ERR_VOUT_FRZ_MSG_NUM) {
                    CurIdx = 0U;
                }

                pCurMsg = &(pFrzMsg->MsgQue[CurIdx]);

                if (pCurMsg->MsgID < SVC_VOUT_ERR_FRZ_MSG_ID_NUM) {

                    if (pCurMsg->MsgID != SVC_VOUT_ERR_FRZ_MSG_ID_FRZE_HIT) {
                        TimeStampID = pCurMsg->MsgID;
                    } else {
                        TimeStampID = SVC_VOUT_ERR_FRZ_MSG_ID_FRZE_CHK;
                    }

                    if (PreTimeStamp[TimeStampID] > 0U) {
                        TimeDiff = pCurMsg->TimeStamp - PreTimeStamp[TimeStampID];
                    } else {
                        TimeDiff = 0U;
                    }

                    PreTimeStamp[TimeStampID] = pCurMsg->TimeStamp;
                }

                if (pCurMsg->MsgID == SVC_VOUT_ERR_FRZ_MSG_ID_VOUT_RDY) {
                    PRN_VOUT_ERR_LOG " [%08u, %04u] VOUT(%d) %04d/%04d/%04d Addr 0x%08X/0x%08X CRC 0x%08X/0x%08X"
                        PRN_VOUT_ERR_ARG_UINT32 pCurMsg->TimeStamp  PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 TimeDiff            PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 VoutID              PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 pCurMsg->Pitch      PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 pCurMsg->Width      PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 pCurMsg->Height     PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT64 pCurMsg->Data[0]
                        PRN_VOUT_ERR_ARG_UINT64 pCurMsg->Data[1]
                        PRN_VOUT_ERR_ARG_UINT64 pCurMsg->Data[2]
                        PRN_VOUT_ERR_ARG_UINT64 pCurMsg->Data[3]
                    PRN_VOUT_ERR_API
                } else if (pCurMsg->MsgID == SVC_VOUT_ERR_FRZ_MSG_ID_DISP_RDY) {
                    PRN_VOUT_ERR_LOG " [%08u, %04u] DISP(%d) %04d/%04d/%04d Addr 0x%08X/0x%08X PTS 0x%016X/0x%016X"
                        PRN_VOUT_ERR_ARG_UINT32 pCurMsg->TimeStamp  PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 TimeDiff            PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 VoutID              PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 pCurMsg->Pitch      PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 pCurMsg->Width      PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 pCurMsg->Height     PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT64 pCurMsg->Data[0]
                        PRN_VOUT_ERR_ARG_UINT64 pCurMsg->Data[1]
                        PRN_VOUT_ERR_ARG_UINT64 pCurMsg->Data[2]
                        PRN_VOUT_ERR_ARG_UINT64 pCurMsg->Data[3]
                    PRN_VOUT_ERR_API
                } else if (pCurMsg->MsgID == SVC_VOUT_ERR_FRZ_MSG_ID_FRZE_CHK) {
                    PRN_VOUT_ERR_LOG " [%08u, %04u]  CHK(%d) cur-CRC 0x%08X/0x%08X pre-CRC 0x%08X/0x%08X"
                        PRN_VOUT_ERR_ARG_UINT32 pCurMsg->TimeStamp  PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 TimeDiff            PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 VoutID              PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT64 pCurMsg->Data[0]
                        PRN_VOUT_ERR_ARG_UINT64 pCurMsg->Data[1]
                        PRN_VOUT_ERR_ARG_UINT64 pCurMsg->Data[2]
                        PRN_VOUT_ERR_ARG_UINT64 pCurMsg->Data[3]
                    PRN_VOUT_ERR_API
                } else if (pCurMsg->MsgID == SVC_VOUT_ERR_FRZ_MSG_ID_FRZE_HIT) {
                    PRN_VOUT_ERR_LOG " [%08u, %04u]  CHK(%d) cur-CRC 0x%08X/0x%08X pre-CRC 0x%08X/0x%08X. Hit %sfreeze%s"
                        PRN_VOUT_ERR_ARG_UINT32 pCurMsg->TimeStamp  PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 TimeDiff            PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 VoutID              PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT64 pCurMsg->Data[0]
                        PRN_VOUT_ERR_ARG_UINT64 pCurMsg->Data[1]
                        PRN_VOUT_ERR_ARG_UINT64 pCurMsg->Data[2]
                        PRN_VOUT_ERR_ARG_UINT64 pCurMsg->Data[3]
                        PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_STR PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_API
                } else {
                    PRN_VOUT_ERR_LOG " Not support MsgID(%d)"
                        PRN_VOUT_ERR_ARG_UINT32 pCurMsg->MsgID PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_API
                }

            } while (CurIdx != pFrzMsg->MsgQueIdx);

            PRN_VOUT_ERR_LOG "------ VOUT(%s%d%s) FREEZE MSG - %sEND%s ------"
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_1 PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 VoutID                      PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_STR     PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_API

            SvcVoutErrTsk_MutexGive(&(SvcVoutErrCtrl.VoutFrzCtrl[VoutID].FrzMsg.Mutex));
        }
    }
}

static void SvcVoutErrTsk_FreezeUpd(UINT32 VoutID, UINT32 HitFreeze)
{
    if (VoutID < AMBA_DSP_MAX_VOUT_NUM) {
        UINT32 OriState = SvcVoutErrCtrl.ErrState[VoutID].FreezeState;

        if (HitFreeze > 0U) {
            if ((SvcVoutErrCtrl.ErrState[VoutID].FreezeState & SVC_VOUT_ERR_VOUT_STATE_FREZE) == 0U) {
                SvcVoutErrCtrl.ErrState[VoutID].FreezeState |= SVC_VOUT_ERR_VOUT_STATE_FREZE;
            }
        } else {
            if ((SvcVoutErrCtrl.ErrState[VoutID].FreezeState & SVC_VOUT_ERR_VOUT_STATE_FREZE) > 0U) {
                SvcVoutErrCtrl.ErrState[VoutID].FreezeState &= ~SVC_VOUT_ERR_VOUT_STATE_FREZE;
            }
        }

        if (OriState != SvcVoutErrCtrl.ErrState[VoutID].FreezeState) {
            PRN_VOUT_ERR_LOG "VoutID(%s%d%s) vout freeze state update %s0x%08x%s -> %s0x%08x%s!"
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_1                    PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 VoutID                                         PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END                        PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_0                    PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 OriState                                       PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END                        PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_0                    PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrCtrl.ErrState[VoutID].FreezeState PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END                        PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_API

            SvcVoutErrTsk_OsdUpdate(VoutID);
        }
    }
}

void SvcVoutErrTask_FreezeReset(UINT32 VoutID)
{
    if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_VOUT_FRZ_INIT) == 0U) {
        PRN_VOUT_ERR_LOG "Fail to reset vout freeze - initial first!" PRN_VOUT_ERR_NG2
    } else if (VoutID >= AMBA_DSP_MAX_VOUT_NUM) {
        PRN_VOUT_ERR_LOG "Fail to proc reset vout freeze - invalid VoutID(%d)!"
            PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_NG
    } else {
        SvcVoutErrTsk_FreezeUpd(VoutID, 0U);

        SvcVoutErrTsk_VoutFrzMsgEnable(VoutID, 1U);

//         SvcVoutErrCtrl.ErrState[VoutID].FreezeState &= ~SVC_VOUT_ERR_VOUT_STATE_FREZE;

        PRN_VOUT_ERR_LOG "Success to reset VoutID(%d) freeze state"
            PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_OK
    }
}

static void SvcVoutErrTsk_RcvSubProcInit(void)
{
    if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_RCV_INIT) == 0U) {
        UINT32 RcvSeq = 0U;

        AmbaSvcWrap_MisraMemset(SvcVoutErrRcvSubProc, 0, sizeof(SvcVoutErrRcvSubProc));
        SvcVoutErrRcvSubProc[RcvSeq] = (SVC_VOUT_ERR_RCV_PROC_s) { 1U, SVC_VOUT_ERR_RCV_PROC_STP_VOUT,     "STOP VOUT    ", SvcVoutErrTsk_BitGet( SVC_VOUT_ERR_RCV_PROC_STP_VOUT    ), SvcVoutErrTsk_RcvProcStopVout   }; RcvSeq ++;
        SvcVoutErrRcvSubProc[RcvSeq] = (SVC_VOUT_ERR_RCV_PROC_s) { 1U, SVC_VOUT_ERR_RCV_PROC_RST_VOUT,     "RESET VOUT   ", SvcVoutErrTsk_BitGet( SVC_VOUT_ERR_RCV_PROC_RST_VOUT    ), SvcVoutErrTsk_RcvProcResetVout  }; RcvSeq ++;
        SvcVoutErrRcvSubProc[RcvSeq] = (SVC_VOUT_ERR_RCV_PROC_s) { 1U, SVC_VOUT_ERR_RCV_PROC_DIS_SER_QRY , "DIS SER QRY  ", SvcVoutErrTsk_BitGet( SVC_VOUT_ERR_RCV_PROC_DIS_SER_QRY ), SvcVoutErrTsk_RcvProcDisSerQry  }; RcvSeq ++;
        SvcVoutErrRcvSubProc[RcvSeq] = (SVC_VOUT_ERR_RCV_PROC_s) { 1U, SVC_VOUT_ERR_RCV_PROC_FPD_DIS ,     "DISABLE FPD  ", SvcVoutErrTsk_BitGet( SVC_VOUT_ERR_RCV_PROC_FPD_DIS     ), SvcVoutErrTsk_RcvProcFpdDis     }; RcvSeq ++;
        SvcVoutErrRcvSubProc[RcvSeq] = (SVC_VOUT_ERR_RCV_PROC_s) { 1U, SVC_VOUT_ERR_RCV_PROC_RST_PWR ,     "RESET POWER  ", SvcVoutErrTsk_BitGet( SVC_VOUT_ERR_RCV_PROC_RST_PWR     ), SvcVoutErrTsk_RcvProcResetPWR   }; RcvSeq ++;
        SvcVoutErrRcvSubProc[RcvSeq] = (SVC_VOUT_ERR_RCV_PROC_s) { 1U, SVC_VOUT_ERR_RCV_PROC_FPD_CFG ,     "CONFIG FPD   ", SvcVoutErrTsk_BitGet( SVC_VOUT_ERR_RCV_PROC_FPD_CFG     ), SvcVoutErrTsk_RcvProcFpdCfg     }; RcvSeq ++;
        SvcVoutErrRcvSubProc[RcvSeq] = (SVC_VOUT_ERR_RCV_PROC_s) { 1U, SVC_VOUT_ERR_RCV_PROC_FPD_BL  ,     "ENA BACKLIGHT", SvcVoutErrTsk_BitGet( SVC_VOUT_ERR_RCV_PROC_FPD_BL      ), SvcVoutErrTsk_RcvProcFpdBL      }; RcvSeq ++;
        SvcVoutErrRcvSubProc[RcvSeq] = (SVC_VOUT_ERR_RCV_PROC_s) { 1U, SVC_VOUT_ERR_RCV_PROC_FPD_ENA ,     "ENABLE FPD   ", SvcVoutErrTsk_BitGet( SVC_VOUT_ERR_RCV_PROC_FPD_ENA     ), SvcVoutErrTsk_RcvProcFpdEna     }; RcvSeq ++;
        SvcVoutErrRcvSubProc[RcvSeq] = (SVC_VOUT_ERR_RCV_PROC_s) { 1U, SVC_VOUT_ERR_RCV_PROC_ENA_SER_QRY , "ENA SER QRY  ", SvcVoutErrTsk_BitGet( SVC_VOUT_ERR_RCV_PROC_ENA_SER_QRY ), SvcVoutErrTsk_RcvProcEnaSerQry  }; RcvSeq ++;
        SvcVoutErrRcvSubProc[RcvSeq] = (SVC_VOUT_ERR_RCV_PROC_s) { 1U, SVC_VOUT_ERR_RCV_PROC_STR_VOUT,     "START VOUT   ", SvcVoutErrTsk_BitGet( SVC_VOUT_ERR_RCV_PROC_STR_VOUT    ), SvcVoutErrTsk_RcvProcStartVout  }; RcvSeq ++;

        AmbaMisra_TouchUnused(&RcvSeq);

        SvcVoutErrCtrlFlg |= SVC_VOUT_ERR_FLG_RCV_INIT;
    }
}

static UINT32 SvcVoutErrTsk_RcvCtrlCfg(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    SVC_VOUT_ERR_RECOVER_TASK_s *pRecoverTask = &(SvcVoutErrCtrl.RecoverTask);

    AmbaSvcWrap_MisraMemset(pRecoverTask, 0, sizeof(SVC_VOUT_ERR_RECOVER_TASK_s));

    /* %s_%s */
      AmbaUtility_StringCopy(pRecoverTask->Name, sizeof(pRecoverTask->Name), SvcVoutErrCtrl.Name);
    AmbaUtility_StringAppend(pRecoverTask->Name, (UINT32)sizeof(pRecoverTask->Name), "_");
    AmbaUtility_StringAppend(pRecoverTask->Name, (UINT32)sizeof(pRecoverTask->Name), "Recover");

    /* Create queue */
    PRetVal = AmbaKAL_MsgQueueCreate( &(pRecoverTask->Que),
                                      pRecoverTask->Name,
                                      (UINT32) sizeof(SVC_VOUT_ERR_RECOVER_CMD_t),
                                      pRecoverTask->QueBuf,
                                      (UINT32) sizeof(pRecoverTask->QueBuf) );
    if (PRetVal != 0U) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to create vout err control - create recover queue fail!" PRN_VOUT_ERR_NG
    }

    /* Create recover control flag */
    if (RetVal == SVC_OK) {

        SvcVoutErrTsk_RcvSubProcInit();

        pRecoverTask->RcvCtrl.pSubProc = SvcVoutErrRcvSubProc;

        PRetVal = AmbaKAL_EventFlagCreate(&(pRecoverTask->RcvCtrl.ActFlg), pRecoverTask->Name);
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_VOUT_ERR_LOG "Fail to create vout err control - create recover ctrl flag fail!" PRN_VOUT_ERR_NG
        } else {
            UINT32 ProcIdx, VoutIdx;
            PRetVal = AmbaKAL_EventFlagClear(&(pRecoverTask->RcvCtrl.ActFlg), 0xFFFFFFFFU);  PRN_VOUT_ERR_HDLR

            for (ProcIdx = 0U; ProcIdx < SVC_VOUT_ERR_RCV_PROC_NUM; ProcIdx ++) {
                if (pRecoverTask->RcvCtrl.pSubProc[ProcIdx].Enable > 0U) {
                    pRecoverTask->RcvCtrl.ProcSelectBits[SVC_VOUT_ERR_RCV_CMD_VOUT] |= pRecoverTask->RcvCtrl.pSubProc[ProcIdx].ProcBit;
                }

                if (pRecoverTask->RcvCtrl.pSubProc[ProcIdx].ProcID == SVC_VOUT_ERR_RCV_PROC_FPD_CFG) {
                    pRecoverTask->RcvCtrl.ProcSelectBits[SVC_VOUT_ERR_RCV_CMD_SERDES] = pRecoverTask->RcvCtrl.pSubProc[ProcIdx].ProcBit;
                }
            }

            pRecoverTask->RcvCtrl.ProcSelectBits[SVC_VOUT_ERR_RCV_CMD_FPD] |= pRecoverTask->RcvCtrl.pSubProc[SVC_VOUT_ERR_RCV_PROC_DIS_SER_QRY].ProcBit;
            pRecoverTask->RcvCtrl.ProcSelectBits[SVC_VOUT_ERR_RCV_CMD_FPD] |= pRecoverTask->RcvCtrl.pSubProc[SVC_VOUT_ERR_RCV_PROC_FPD_DIS    ].ProcBit;
            pRecoverTask->RcvCtrl.ProcSelectBits[SVC_VOUT_ERR_RCV_CMD_FPD] |= pRecoverTask->RcvCtrl.pSubProc[SVC_VOUT_ERR_RCV_PROC_RST_PWR    ].ProcBit;
            pRecoverTask->RcvCtrl.ProcSelectBits[SVC_VOUT_ERR_RCV_CMD_FPD] |= pRecoverTask->RcvCtrl.pSubProc[SVC_VOUT_ERR_RCV_PROC_FPD_CFG    ].ProcBit;
            pRecoverTask->RcvCtrl.ProcSelectBits[SVC_VOUT_ERR_RCV_CMD_FPD] |= pRecoverTask->RcvCtrl.pSubProc[SVC_VOUT_ERR_RCV_PROC_FPD_BL     ].ProcBit;
            pRecoverTask->RcvCtrl.ProcSelectBits[SVC_VOUT_ERR_RCV_CMD_FPD] |= pRecoverTask->RcvCtrl.pSubProc[SVC_VOUT_ERR_RCV_PROC_FPD_ENA    ].ProcBit;
            pRecoverTask->RcvCtrl.ProcSelectBits[SVC_VOUT_ERR_RCV_CMD_FPD] |= pRecoverTask->RcvCtrl.pSubProc[SVC_VOUT_ERR_RCV_PROC_ENA_SER_QRY].ProcBit;

            for (VoutIdx = 0U; VoutIdx < AMBA_DSP_MAX_VOUT_NUM; VoutIdx ++) {
                if ((SvcVoutErrCtrl.VoutErrSelectBits & SvcVoutErrTsk_BitGet(VoutIdx)) > 0U) {
                    PRetVal = SvcVoutErrTsk_RcvFlagSet(VoutIdx, pRecoverTask->RcvCtrl.ProcSelectBits[SVC_VOUT_ERR_RCV_CMD_VOUT]); PRN_VOUT_ERR_HDLR
                }
            }

            if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_LOG_DBG1) > 0U) {
                SvcVoutErrTsk_RcvCtrlInfo();
            }
        }
    }

    /* Create recover task */
    if (RetVal == SVC_OK) {
        pRecoverTask->Ctrl.CpuBits    = SVC_VOUT_ERR_TASK_CPU_BITS;
        pRecoverTask->Ctrl.Priority   = SVC_VOUT_ERR_RECOVER_TASK_PRI;
        pRecoverTask->Ctrl.EntryFunc  = SvcVoutErrTsk_RcvTaskEntry;
        pRecoverTask->Ctrl.StackSize  = SVC_VOUT_ERR_STACK_SZ;
        pRecoverTask->Ctrl.pStackBase = &(pRecoverTask->Stack[0]);

        PRetVal = SvcTask_Create(pRecoverTask->Name, &(pRecoverTask->Ctrl));
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_VOUT_ERR_LOG "Fail to create vout err control - create recover task fail! ErrCode 0x%x"
                PRN_VOUT_ERR_ARG_UINT32 PRetVal PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_NG
        } else {
            PRN_VOUT_ERR_LOG "Successful to create vout err recover task - %s"
                PRN_VOUT_ERR_ARG_CSTR   pRecoverTask->Name PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_DBG1
        }
    }

    return RetVal;
}

static void SvcVoutErrTsk_RcvCtrlInfo(void)
{
    if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_RCV_INIT) > 0U) {
        SVC_VOUT_ERR_RCV_CTRL_s *pCtrl = &(SvcVoutErrCtrl.RecoverTask.RcvCtrl);
        UINT32 PRetVal;
        UINT32 Idx, TempU32;
        AMBA_KAL_EVENT_FLAG_INFO_s CurFlgInfo;

        AmbaSvcWrap_MisraMemset(&CurFlgInfo, 0, sizeof(CurFlgInfo));
        PRetVal = AmbaKAL_EventFlagQuery(&(pCtrl->ActFlg), &(CurFlgInfo)); PRN_VOUT_ERR_HDLR

        PRN_VOUT_ERR_LOG " " PRN_VOUT_ERR_API
        PRN_VOUT_ERR_LOG "---- %sRecover Control Info%s ----"
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_0 PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_API
        PRN_VOUT_ERR_LOG "         ActFlg : 0x%08x ( wait task: %d )"
            PRN_VOUT_ERR_ARG_UINT32 CurFlgInfo.CurrentFlags PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 CurFlgInfo.NumWaitTask  PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_API
        PRN_VOUT_ERR_LOG "           Name : %s"
            PRN_VOUT_ERR_ARG_CSTR   pCtrl->Name PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_API
        PRN_VOUT_ERR_LOG " ProcSelectBits : Recover cmd index | process select bits" PRN_VOUT_ERR_API
        for (Idx = 0U; Idx < SVC_VOUT_ERR_RCV_CMD_NUM; Idx ++) {
            PRN_VOUT_ERR_LOG "                                 %02d | 0x%08x"
                PRN_VOUT_ERR_ARG_UINT32 Idx                        PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 pCtrl->ProcSelectBits[Idx] PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_API
        }
        PRN_VOUT_ERR_LOG "       pSubProc : %p"
            PRN_VOUT_ERR_ARG_CPOINT pCtrl->pSubProc PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_API
        if (pCtrl->pSubProc != NULL) {

            PRN_VOUT_ERR_LOG "                  Enable | ProcID |   ProcBit  |  Func Addr | ProcName" PRN_VOUT_ERR_API

            for (Idx = 0U; Idx < SVC_VOUT_ERR_RCV_PROC_NUM; Idx ++) {
                if (pCtrl->pSubProc[Idx].Enable > 0U) {
                    AmbaMisra_TypeCast(&(TempU32), &(pCtrl->pSubProc[Idx].pFunc));

                    PRN_VOUT_ERR_LOG "                    %02d   |   %02d   | 0x%08x | 0x%08x | %s"
                        PRN_VOUT_ERR_ARG_UINT32 pCtrl->pSubProc[Idx].Enable   PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 pCtrl->pSubProc[Idx].ProcID   PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 pCtrl->pSubProc[Idx].ProcBit  PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 TempU32                       PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_CSTR   pCtrl->pSubProc[Idx].ProcName PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_API
                }
            }
        }
    }
}

static UINT32 SvcVoutErrTsk_RcvFlagClear(UINT32 VoutID, UINT32 Flag)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (VoutID < AMBA_DSP_MAX_VOUT_NUM) {
        AMBA_KAL_EVENT_FLAG_t *pFlag = &(SvcVoutErrCtrl.RecoverTask.RcvCtrl.ActFlg);
        UINT32 CurFlag = Flag << (SVC_VOUT_ERR_RCV_PROC_NUM * VoutID);
        PRetVal = AmbaKAL_EventFlagClear(pFlag, CurFlag);
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_VOUT_ERR_LOG "Clear VoutID(%d) recover flag(0x%08x) fail!"
                PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 Flag   PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_NG
        }
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}

static UINT32 SvcVoutErrTsk_RcvFlagSet(UINT32 VoutID, UINT32 Flag)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (VoutID < AMBA_DSP_MAX_VOUT_NUM) {
        AMBA_KAL_EVENT_FLAG_t *pFlag = &(SvcVoutErrCtrl.RecoverTask.RcvCtrl.ActFlg);
        UINT32 CurFlag = Flag << (SVC_VOUT_ERR_RCV_PROC_NUM * VoutID);
        PRetVal = AmbaKAL_EventFlagSet(pFlag, CurFlag);
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_VOUT_ERR_LOG "Set VoutID(%d) recover flag(0x%08x) fail!"
                PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 Flag   PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_NG
        }
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}

static UINT32 SvcVoutErrTsk_RcvFlagGet(UINT32 VoutID, UINT32 Flag, UINT32 Timeout)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (VoutID < AMBA_DSP_MAX_VOUT_NUM) {
        AMBA_KAL_EVENT_FLAG_t *pFlag = &(SvcVoutErrCtrl.RecoverTask.RcvCtrl.ActFlg);
        UINT32 CurFlag = Flag << (SVC_VOUT_ERR_RCV_PROC_NUM * VoutID);
        UINT32 ActualFlags = 0U;
        PRetVal = AmbaKAL_EventFlagGet(pFlag, CurFlag, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlags, Timeout);
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_VOUT_ERR_LOG "Get VoutID(%d) recover flag(0x%08x) timeout(%u)"
                PRN_VOUT_ERR_ARG_UINT32 VoutID  PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 Flag    PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 Timeout PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_NG
        }
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}

static UINT32 SvcVoutErrTsk_RcvFlagQry(UINT32 VoutID, UINT32 *pFlag)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (VoutID < AMBA_DSP_MAX_VOUT_NUM) {
        AMBA_KAL_EVENT_FLAG_t *pActFlg = &(SvcVoutErrCtrl.RecoverTask.RcvCtrl.ActFlg);
        UINT32 FlagMask = ( ( SvcVoutErrTsk_BitGet( SVC_VOUT_ERR_RCV_PROC_NUM ) - 1U ) << ( SVC_VOUT_ERR_RCV_PROC_NUM * VoutID ) );
        AMBA_KAL_EVENT_FLAG_INFO_s CurFlgInfo;

        AmbaSvcWrap_MisraMemset(&CurFlgInfo, 0, sizeof(CurFlgInfo));
        PRetVal = AmbaKAL_EventFlagQuery(pActFlg, &CurFlgInfo);
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_VOUT_ERR_LOG "Query VoutID(%d) recover flag fail"
                PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_NG
        } else {
            *pFlag = CurFlgInfo.CurrentFlags & FlagMask;
            *pFlag = *pFlag >> ( SVC_VOUT_ERR_RCV_PROC_NUM * VoutID );
        }
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}

static UINT32 SvcVoutErrTsk_RcvProcStartVout(UINT32 PreChkVal, const void *pCmd, const void *pProc)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    SVC_VOUT_ERR_RECOVER_CMD_t *pRcvCmd;
    SVC_VOUT_ERR_RCV_PROC_s    *pProcInfo;

    AmbaMisra_TypeCast(&(pRcvCmd), &(pCmd));
    AmbaMisra_TypeCast(&(pProcInfo), &(pProc));

    if (pRcvCmd == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc recover - invalid recover command!" PRN_VOUT_ERR_NG
    } else if (pProcInfo == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc recover - invalid recover process info!" PRN_VOUT_ERR_NG
    } else {
        UINT32 VoutID = pRcvCmd->VoutID;

        if (VoutID >= AMBA_DSP_MAX_VOUT_NUM) {
            RetVal = SVC_NG;
            PRN_VOUT_ERR_LOG "Fail to proc %s - invalid VoutID(%d)"
                PRN_VOUT_ERR_ARG_CSTR   pProcInfo->ProcName PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 VoutID              PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_NG
        } else {
            UINT64 AttachedRawSeq = 0ULL;

            PRetVal = SvcVoutErrTsk_RcvFlagGet(VoutID, pProcInfo->ProcBit, AMBA_KAL_WAIT_FOREVER); PRN_VOUT_ERR_HDLR

            PRN_VOUT_ERR_LOG "---- %sStart VoutID(%d)%s ----"
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_2 PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 VoutID                      PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_DBG1

            PRetVal = AmbaDSP_VoutDisplayCtrl((UINT8)VoutID); AmbaMisra_TouchUnused(&AttachedRawSeq);
            if (PRetVal != 0U) {
                RetVal |= SVC_NG;
                PRN_VOUT_ERR_LOG "Fail to proc %s - enable VoutID(%d) display fail"
                    PRN_VOUT_ERR_ARG_CSTR   pProcInfo->ProcName PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_UINT32 VoutID              PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_NG
            }

            PRetVal = AmbaDSP_VoutMixerCtrl((UINT8)VoutID); AmbaMisra_TouchUnused(&AttachedRawSeq);
            if (PRetVal != 0U) {
                RetVal |= SVC_NG;
                PRN_VOUT_ERR_LOG "Fail to proc %s - enable VoutID(%d) mixer fail"
                    PRN_VOUT_ERR_ARG_CSTR   pProcInfo->ProcName PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_UINT32 VoutID              PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_NG
            }

            PRetVal = AmbaDSP_VoutOsdCtrl((UINT8)VoutID, 1, 0, &AttachedRawSeq); AmbaMisra_TouchUnused(&AttachedRawSeq);
            if (PRetVal != 0U) {
                RetVal |= SVC_NG;
                PRN_VOUT_ERR_LOG "Fail to proc %s - enable VoutID(%d) OSD fail"
                    PRN_VOUT_ERR_ARG_CSTR   pProcInfo->ProcName PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_UINT32 VoutID              PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_NG
            }

            PRetVal = AmbaDSP_VoutVideoCtrl((UINT8)VoutID, 1, 0, &AttachedRawSeq); AmbaMisra_TouchUnused(&AttachedRawSeq);
            if (PRetVal != 0U) {
                RetVal |= SVC_NG;
                PRN_VOUT_ERR_LOG "Fail to proc %s - enable VoutID(%d) video fail"
                    PRN_VOUT_ERR_ARG_CSTR   pProcInfo->ProcName PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_UINT32 VoutID              PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_NG
            }

            PRN_VOUT_ERR_LOG "Start VoutID(%d) done"
                PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_API
        }
    }

    AmbaMisra_TouchUnused(&PRetVal);
    AmbaMisra_TouchUnused(pRcvCmd);
    AmbaMisra_TouchUnused(pProcInfo);
    AmbaMisra_TouchUnused(&PreChkVal);

    return RetVal;

}

static UINT32 SvcVoutErrTsk_RcvProcStopVout(UINT32 PreChkVal, const void *pCmd, const void *pProc)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    SVC_VOUT_ERR_RECOVER_CMD_t *pRcvCmd;
    SVC_VOUT_ERR_RCV_PROC_s    *pProcInfo;

    AmbaMisra_TypeCast(&(pRcvCmd), &(pCmd));
    AmbaMisra_TypeCast(&(pProcInfo), &(pProc));

    if (pRcvCmd == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc recover - invalid recover command!" PRN_VOUT_ERR_NG
    } else if (pProcInfo == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc recover - invalid recover process info!" PRN_VOUT_ERR_NG
    } else {
        UINT32 VoutID = pRcvCmd->VoutID;

        if (VoutID >= AMBA_DSP_MAX_VOUT_NUM) {
            RetVal = SVC_NG;
            PRN_VOUT_ERR_LOG "Fail to proc %s - invalid VoutID(%d)"
                PRN_VOUT_ERR_ARG_CSTR   pProcInfo->ProcName PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 VoutID              PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_NG
        } else {
            UINT64 AttachedRawSeq = 0ULL;

            PRetVal = SvcVoutErrTsk_RcvFlagGet(VoutID, pProcInfo->ProcBit, AMBA_KAL_WAIT_FOREVER); PRN_VOUT_ERR_HDLR

            PRN_VOUT_ERR_LOG "---- %sStop VoutID(%d)%s ----"
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_2 PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 VoutID                      PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_DBG1

            PRetVal = AmbaDSP_VoutOsdCtrl((UINT8)VoutID, 0, 0, &AttachedRawSeq); AmbaMisra_TouchUnused(&AttachedRawSeq);
            if (PRetVal != 0U) {
                RetVal |= SVC_NG;
                PRN_VOUT_ERR_LOG "Fail to proc %s - disable VoutID(%d) OSD fail"
                    PRN_VOUT_ERR_ARG_CSTR   pProcInfo->ProcName PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_UINT32 VoutID              PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_NG
            }

            PRetVal = AmbaDSP_VoutVideoCtrl((UINT8)VoutID, 0, 0, &AttachedRawSeq); AmbaMisra_TouchUnused(&AttachedRawSeq);
            if (PRetVal != 0U) {
                RetVal |= SVC_NG;
                PRN_VOUT_ERR_LOG "Fail to proc %s - disable VoutID(%d) video fail"
                    PRN_VOUT_ERR_ARG_CSTR   pProcInfo->ProcName PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_UINT32 VoutID              PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_NG
            }

            PRN_VOUT_ERR_LOG "Stop VoutID(%d) done"
                PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_API
        }
    }

    AmbaMisra_TouchUnused(&PRetVal);
    AmbaMisra_TouchUnused(pRcvCmd);
    AmbaMisra_TouchUnused(pProcInfo);
    AmbaMisra_TouchUnused(&PreChkVal);

    return RetVal;
}

static UINT32 SvcVoutErrTsk_RcvProcResetVout(UINT32 PreChkVal, const void *pCmd, const void *pProc)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    SVC_VOUT_ERR_RECOVER_CMD_t *pRcvCmd;
    SVC_VOUT_ERR_RCV_PROC_s    *pProcInfo;

    AmbaMisra_TypeCast(&(pRcvCmd), &(pCmd));
    AmbaMisra_TypeCast(&(pProcInfo), &(pProc));

    if (pRcvCmd == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc recover - invalid recover command!" PRN_VOUT_ERR_NG
    } else if (pProcInfo == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc recover - invalid recover process info!" PRN_VOUT_ERR_NG
    } else {
        UINT32 VoutID = pRcvCmd->VoutID;

        if (VoutID >= AMBA_DSP_MAX_VOUT_NUM) {
            RetVal = SVC_NG;
            PRN_VOUT_ERR_LOG "Fail to proc %s - invalid VoutID(%d)"
                PRN_VOUT_ERR_ARG_CSTR   pProcInfo->ProcName PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 VoutID              PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_NG
        } else {

            PRetVal = SvcVoutErrTsk_RcvFlagGet(VoutID, pProcInfo->ProcBit, AMBA_KAL_WAIT_FOREVER); PRN_VOUT_ERR_HDLR

            PRN_VOUT_ERR_LOG "---- %sReset VoutID(%d)%s ----"
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_2 PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 VoutID                      PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_DBG1

            PRetVal = AmbaDSP_VoutReset((UINT8) VoutID);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_VOUT_ERR_LOG "Fail to proc %s - reset VoutID(%d) fail"
                    PRN_VOUT_ERR_ARG_CSTR   pProcInfo->ProcName PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_UINT32 VoutID              PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_NG
            }

            PRN_VOUT_ERR_LOG "Reset VoutID(%d) done"
                PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_API
        }
    }

    AmbaMisra_TouchUnused(&PRetVal);
    AmbaMisra_TouchUnused(pRcvCmd);
    AmbaMisra_TouchUnused(pProcInfo);
    AmbaMisra_TouchUnused(&PreChkVal);

    return RetVal;
}

static UINT32 SvcVoutErrTsk_RcvProcResetPWR(UINT32 PreChkVal, const void *pCmd, const void *pProc)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    SVC_VOUT_ERR_RECOVER_CMD_t *pRcvCmd;
    SVC_VOUT_ERR_RCV_PROC_s    *pProcInfo;

    AmbaMisra_TypeCast(&(pRcvCmd), &(pCmd));
    AmbaMisra_TypeCast(&(pProcInfo), &(pProc));

    if (pRcvCmd == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc recover - invalid recover command!" PRN_VOUT_ERR_NG
    } else if (pProcInfo == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc recover - invalid recover process info!" PRN_VOUT_ERR_NG
    } else {
        UINT32 VoutID = pRcvCmd->VoutID;

        if (VoutID >= AMBA_DSP_MAX_VOUT_NUM) {
            RetVal = SVC_NG;
            PRN_VOUT_ERR_LOG "Fail to proc %s - invalid VoutID(%d)"
                PRN_VOUT_ERR_ARG_CSTR   pProcInfo->ProcName PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 VoutID              PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_NG
        } else {

            PRetVal = SvcVoutErrTsk_RcvFlagGet(VoutID, pProcInfo->ProcBit, AMBA_KAL_WAIT_FOREVER); PRN_VOUT_ERR_HDLR

            PRN_VOUT_ERR_LOG "---- %sReset VoutID(%d) device power %s ----"
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_2 PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 VoutID                      PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_DBG1

            PRN_VOUT_ERR_LOG "Reset VoutID(%d) device power done"
                PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_API
        }
    }

    AmbaMisra_TouchUnused(&PRetVal);
    AmbaMisra_TouchUnused(pRcvCmd);
    AmbaMisra_TouchUnused(pProcInfo);
    AmbaMisra_TouchUnused(&PreChkVal);

    return RetVal;
}

static UINT32 SvcVoutErrTsk_RcvProcFpdDis(UINT32 PreChkVal, const void *pCmd, const void *pProc)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    SVC_VOUT_ERR_RECOVER_CMD_t *pRcvCmd;
    SVC_VOUT_ERR_RCV_PROC_s    *pProcInfo;

    AmbaMisra_TypeCast(&(pRcvCmd), &(pCmd));
    AmbaMisra_TypeCast(&(pProcInfo), &(pProc));

    if (pRcvCmd == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc recover - invalid recover command!" PRN_VOUT_ERR_NG
    } else if (pProcInfo == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc recover - invalid recover process info!" PRN_VOUT_ERR_NG
    } else {
        UINT32 VoutID = pRcvCmd->VoutID;

        if (VoutID >= AMBA_DSP_MAX_VOUT_NUM) {
            RetVal = SVC_NG;
            PRN_VOUT_ERR_LOG "Fail to proc %s - invalid VoutID(%d)"
                PRN_VOUT_ERR_ARG_CSTR   pProcInfo->ProcName PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 VoutID              PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_NG
        } else {

            PRetVal = SvcVoutErrTsk_RcvFlagGet(VoutID, pProcInfo->ProcBit, AMBA_KAL_WAIT_FOREVER); PRN_VOUT_ERR_HDLR

            PRN_VOUT_ERR_LOG "---- %sDisable VoutID(%d) FPD driver%s ----"
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_2 PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 VoutID                      PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_DBG1

            PRetVal = AmbaFPD_Disable(VoutID);
            if (PRetVal != 0U) {
                PRN_VOUT_ERR_LOG "Disable VoutID(%d) FPD driver fail!"
                    PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_DBG1
            }

            PRN_VOUT_ERR_LOG "Disable VoutID(%d) FPD driver done"
                PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_API
        }
    }

    AmbaMisra_TouchUnused(&PRetVal);
    AmbaMisra_TouchUnused(pRcvCmd);
    AmbaMisra_TouchUnused(pProcInfo);
    AmbaMisra_TouchUnused(&PreChkVal);

    return RetVal;
}

static UINT32 SvcVoutErrTsk_RcvProcFpdEna(UINT32 PreChkVal, const void *pCmd, const void *pProc)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    SVC_VOUT_ERR_RECOVER_CMD_t *pRcvCmd;
    SVC_VOUT_ERR_RCV_PROC_s    *pProcInfo;

    AmbaMisra_TypeCast(&(pRcvCmd), &(pCmd));
    AmbaMisra_TypeCast(&(pProcInfo), &(pProc));

    if (pRcvCmd == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc recover - invalid recover command!" PRN_VOUT_ERR_NG
    } else if (pProcInfo == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc recover - invalid recover process info!" PRN_VOUT_ERR_NG
    } else {
        UINT32 VoutID = pRcvCmd->VoutID;

        if (VoutID >= AMBA_DSP_MAX_VOUT_NUM) {
            RetVal = SVC_NG;
            PRN_VOUT_ERR_LOG "Fail to proc %s - invalid VoutID(%d)"
                PRN_VOUT_ERR_ARG_CSTR   pProcInfo->ProcName PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 VoutID              PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_NG
        } else {

            PRetVal = SvcVoutErrTsk_RcvFlagGet(VoutID, pProcInfo->ProcBit, AMBA_KAL_WAIT_FOREVER); PRN_VOUT_ERR_HDLR

            PRN_VOUT_ERR_LOG "---- %sEnable VoutID(%d) FPD driver%s ----"
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_2 PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 VoutID                      PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_DBG1

            PRetVal = AmbaFPD_Enable(VoutID);
            if (PRetVal != 0U) {
                PRN_VOUT_ERR_LOG "Enable VoutID(%d) FPD driver fail!"
                    PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_DBG1
            }
            PRN_VOUT_ERR_LOG "Enable VoutID(%d) FPD driver done"
                PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_API
        }
    }

    AmbaMisra_TouchUnused(&PRetVal);
    AmbaMisra_TouchUnused(pRcvCmd);
    AmbaMisra_TouchUnused(pProcInfo);
    AmbaMisra_TouchUnused(&PreChkVal);

    return RetVal;
}

static UINT32 SvcVoutErrTsk_RcvProcFpdCfg(UINT32 PreChkVal, const void *pCmd, const void *pProc)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    SVC_VOUT_ERR_RECOVER_CMD_t *pRcvCmd;
    SVC_VOUT_ERR_RCV_PROC_s    *pProcInfo;

    AmbaMisra_TypeCast(&(pRcvCmd), &(pCmd));
    AmbaMisra_TypeCast(&(pProcInfo), &(pProc));

    if (pRcvCmd == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc recover - invalid recover command!" PRN_VOUT_ERR_NG
    } else if (pProcInfo == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc recover - invalid recover process info!" PRN_VOUT_ERR_NG
    } else {
        UINT32 VoutID = pRcvCmd->VoutID;
        const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();

        if (VoutID >= AMBA_DSP_MAX_VOUT_NUM) {
            RetVal = SVC_NG;
            PRN_VOUT_ERR_LOG "Fail to proc %s - invalid VoutID(%d)"
                PRN_VOUT_ERR_ARG_CSTR   pProcInfo->ProcName PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 VoutID              PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_NG
        } else if (pCfg == NULL) {
            RetVal = SVC_NG;
            PRN_VOUT_ERR_LOG "Fail to proc %s - invalid cfg"
                PRN_VOUT_ERR_ARG_CSTR   pProcInfo->ProcName PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_NG
        } else {
            UINT32 DispIdx, DevMode = 0xFFFFFFFFU;

            PRetVal = SvcVoutErrTsk_RcvFlagGet(VoutID, pProcInfo->ProcBit, AMBA_KAL_WAIT_FOREVER); PRN_VOUT_ERR_HDLR

            for (DispIdx = 0U; DispIdx < pCfg->DispNum; DispIdx ++) {
                if (pCfg->DispStrm[DispIdx].VoutID == VoutID) {
                    DevMode = MAXIM_GetRawMode(pCfg->DispStrm[DispIdx].DevMode);
                    break;
                }
            }

            if (DevMode != 0xFFFFFFFFU) {
                PRN_VOUT_ERR_LOG "---- %sConfigure VoutID(%d) FPD driver%s DevMode(0x%x) ----"
                    PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_2 PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_UINT32 VoutID                      PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_UINT32 DevMode                     PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_DBG1

                PRetVal = AmbaFPD_Config(VoutID, (UINT8) DevMode);
                if (PRetVal != 0U) {
                    PRN_VOUT_ERR_LOG "Configure VoutID(%d) FPD driver fail!"
                        PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_DBG1
                }
            } else {
                RetVal = SVC_NG;
                PRN_VOUT_ERR_LOG "Fail to proc %s - invalid FPD DevMode(0x%x)"
                    PRN_VOUT_ERR_ARG_CSTR   pProcInfo->ProcName PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_UINT32 DevMode             PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_NG
            }

            PRN_VOUT_ERR_LOG "Config VoutID(%d) FPD driver done"
                PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_API
        }
    }

    AmbaMisra_TouchUnused(&PRetVal);
    AmbaMisra_TouchUnused(pRcvCmd);
    AmbaMisra_TouchUnused(pProcInfo);
    AmbaMisra_TouchUnused(&PreChkVal);

    return RetVal;
}

static UINT32 SvcVoutErrTsk_RcvProcFpdBL(UINT32 PreChkVal, const void *pCmd, const void *pProc)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    SVC_VOUT_ERR_RECOVER_CMD_t *pRcvCmd;
    SVC_VOUT_ERR_RCV_PROC_s    *pProcInfo;

    AmbaMisra_TypeCast(&(pRcvCmd), &(pCmd));
    AmbaMisra_TypeCast(&(pProcInfo), &(pProc));

    if (pRcvCmd == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc recover - invalid recover command!" PRN_VOUT_ERR_NG
    } else if (pProcInfo == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc recover - invalid recover process info!" PRN_VOUT_ERR_NG
    } else {
        UINT32 VoutID = pRcvCmd->VoutID;

        if (VoutID >= AMBA_DSP_MAX_VOUT_NUM) {
            RetVal = SVC_NG;
            PRN_VOUT_ERR_LOG "Fail to proc %s - invalid VoutID(%d)"
                PRN_VOUT_ERR_ARG_CSTR   pProcInfo->ProcName PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 VoutID              PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_NG
        } else {

            PRetVal = SvcVoutErrTsk_RcvFlagGet(VoutID, pProcInfo->ProcBit, AMBA_KAL_WAIT_FOREVER); PRN_VOUT_ERR_HDLR

            PRN_VOUT_ERR_LOG "  ---- %sEnable VoutID(%d) Black light%s ----"
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_2 PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 VoutID                      PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_DBG1
            PRetVal = AmbaFPD_SetBacklight(VoutID, 1U); PRN_VOUT_ERR_HDLR

            PRN_VOUT_ERR_LOG "Enable VoutID(%d) Black light done"
                PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_API
        }
    }

    AmbaMisra_TouchUnused(&PRetVal);
    AmbaMisra_TouchUnused(pRcvCmd);
    AmbaMisra_TouchUnused(pProcInfo);
    AmbaMisra_TouchUnused(&PreChkVal);

    return RetVal;
}

static UINT32 SvcVoutErrTsk_RcvProcEnaSerQry(UINT32 PreChkVal, const void *pCmd, const void *pProc)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    SVC_VOUT_ERR_RECOVER_CMD_t *pRcvCmd;
    SVC_VOUT_ERR_RCV_PROC_s    *pProcInfo;

    AmbaMisra_TypeCast(&(pRcvCmd), &(pCmd));
    AmbaMisra_TypeCast(&(pProcInfo), &(pProc));

    if (pRcvCmd == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc recover - invalid recover command!" PRN_VOUT_ERR_NG
    } else if (pProcInfo == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc recover - invalid recover process info!" PRN_VOUT_ERR_NG
    } else {
        UINT32 VoutID = pRcvCmd->VoutID;

        if (VoutID >= AMBA_DSP_MAX_VOUT_NUM) {
            RetVal = SVC_NG;
            PRN_VOUT_ERR_LOG "Fail to proc %s - invalid VoutID(%d)"
                PRN_VOUT_ERR_ARG_CSTR   pProcInfo->ProcName PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 VoutID              PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_NG
        } else {
            PRetVal = SvcVoutErrTsk_RcvFlagGet(VoutID, pProcInfo->ProcBit, AMBA_KAL_WAIT_FOREVER); PRN_VOUT_ERR_HDLR

            /* Enable serdes link update */
            PRN_VOUT_ERR_LOG "---- %sEnable VoutID(%d) SerDes Link Update%s ----"
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_2 PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 VoutID                      PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_DBG1

            SvcVoutErrTsk_LinkStateUpdEna(VoutID, SVC_VOUT_ERR_LNK_UPD_ON);

            PRN_VOUT_ERR_LOG "Enable VoutID(%d) SerDes Link Update done"
                PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_API
        }
    }

    AmbaMisra_TouchUnused(&PRetVal);
    AmbaMisra_TouchUnused(pRcvCmd);
    AmbaMisra_TouchUnused(pProcInfo);
    AmbaMisra_TouchUnused(&PreChkVal);

    return RetVal;

}

static UINT32 SvcVoutErrTsk_RcvProcDisSerQry(UINT32 PreChkVal, const void *pCmd, const void *pProc)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    SVC_VOUT_ERR_RECOVER_CMD_t *pRcvCmd;
    SVC_VOUT_ERR_RCV_PROC_s    *pProcInfo;

    AmbaMisra_TypeCast(&(pRcvCmd), &(pCmd));
    AmbaMisra_TypeCast(&(pProcInfo), &(pProc));

    if (pRcvCmd == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc recover - invalid recover command!" PRN_VOUT_ERR_NG
    } else if (pProcInfo == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc recover - invalid recover process info!" PRN_VOUT_ERR_NG
    } else {
        UINT32 VoutID = pRcvCmd->VoutID;

        if (VoutID >= AMBA_DSP_MAX_VOUT_NUM) {
            RetVal = SVC_NG;
            PRN_VOUT_ERR_LOG "Fail to proc %s - invalid VoutID(%d)"
                PRN_VOUT_ERR_ARG_CSTR   pProcInfo->ProcName PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 VoutID              PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_NG
        } else {
            PRetVal = SvcVoutErrTsk_RcvFlagGet(VoutID, pProcInfo->ProcBit, AMBA_KAL_WAIT_FOREVER); PRN_VOUT_ERR_HDLR

            /* Disable serdes link update */
            PRN_VOUT_ERR_LOG "---- %sDisable VoutID(%d) SerDes Link Update%s ----"
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_2 PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 VoutID                      PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_DBG1

            SvcVoutErrTsk_LinkStateUpdEna(VoutID, SVC_VOUT_ERR_LNK_UPD_OFF);

            PRN_VOUT_ERR_LOG "Disable VoutID(%d) SerDes Link Update done"
                PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_API
        }
    }

    AmbaMisra_TouchUnused(&PRetVal);
    AmbaMisra_TouchUnused(pRcvCmd);
    AmbaMisra_TouchUnused(pProcInfo);
    AmbaMisra_TouchUnused(&PreChkVal);

    return RetVal;

}

static void SvcVoutErrTsk_MonMsgProcVoutStart(const void *pMsg, UINT64 Flag, UINT32 MsgType)
{
    UINT32 PRetVal;
    UINT32 *pVoutId = NULL;
    const SVC_VOUT_ERR_MON_MSG_s *pMsgCtrl = NULL;

    if (SvcVoutErrCtrl.MonFrwkCtrl.pMonitorPortCtrl != NULL) {
        if (SvcVoutErrCtrl.MonFrwkCtrl.pMonitorPortCtrl[VOUT_ERR_MON_PORT_VOUT_STATE].pMsgList != NULL) {
            pMsgCtrl = &(SvcVoutErrCtrl.MonFrwkCtrl.pMonitorPortCtrl[VOUT_ERR_MON_PORT_VOUT_STATE].pMsgList[MON_VOUT_STAT_MSG_VOUT_START]);
        }
    }

    AmbaMisra_TypeCast(&(pVoutId), &(pMsg));

    if (pMsgCtrl == NULL) {
        PRN_VOUT_ERR_LOG "Fail to proc vout start msg - invalid msg control!" PRN_VOUT_ERR_NG
    } else if ((pMsgCtrl->MsgFlag & Flag) == 0ULL) {
        PRN_VOUT_ERR_LOG "Fail to proc vout start msg - wrong flag!" PRN_VOUT_ERR_NG
    } else if (pVoutId == NULL) {
        PRN_VOUT_ERR_LOG "Fail to proc vout start msg - invalid msg!" PRN_VOUT_ERR_NG
    } else if ((*pVoutId) >= AMBA_DSP_MAX_VOUT_NUM) {
        PRN_VOUT_ERR_LOG "Fail to proc vout start msg - invalid VoutID(%d)!"
            PRN_VOUT_ERR_ARG_UINT32 *pVoutId PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_NG
    } else if ((SvcVoutErrCtrl.VoutSelectBits & SvcVoutErrTsk_BitGet(*pVoutId)) == 0U) {
        if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_LOG_DBG1) > 0U) {
            PRN_VOUT_ERR_LOG "Fail to proc vout start msg - Not supported VoutID(%d)! CurSelectBits 0x%x"
                PRN_VOUT_ERR_ARG_UINT32 (*pVoutId)                    PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrCtrl.VoutSelectBits PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_NG
        }
    } else if ((MsgType & VOUT_ERR_MON_MSG_TIMEOUT) > 0U) {
        PRN_VOUT_ERR_LOG "Monitor VoutID(%s%d%s) start msg timeout!"
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_1 PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 *pVoutId                    PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_DBG1
    } else {

        PRN_VOUT_ERR_LOG "Start VoutID(%s%d%s) isr!"
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_1 PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 *pVoutId                    PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_DBG1

        PRetVal = AmbaKAL_EventFlagSet(&(SvcVoutErrCtrl.ErrState[*pVoutId].VoutStateFlag), SVC_VOUT_ERR_VOUT_STATE_START); PRN_VOUT_ERR_HDLR
    }

    AmbaMisra_TouchUnused(pVoutId);
}

static void SvcVoutErrTsk_MonMsgProcVoutTout(const void *pMsg, UINT64 Flag, UINT32 MsgType)
{
    UINT32 PRetVal;
    UINT32 *pVoutId = NULL;
    const SVC_VOUT_ERR_MON_MSG_s *pMsgCtrl = NULL;

    if (SvcVoutErrCtrl.MonFrwkCtrl.pMonitorPortCtrl != NULL) {
        if (SvcVoutErrCtrl.MonFrwkCtrl.pMonitorPortCtrl[VOUT_ERR_MON_PORT_VOUT_STATE].pMsgList != NULL) {
            pMsgCtrl = &(SvcVoutErrCtrl.MonFrwkCtrl.pMonitorPortCtrl[VOUT_ERR_MON_PORT_VOUT_STATE].pMsgList[MON_VOUT_STAT_MSG_VOUT_TOUT]);
        }
    }

    AmbaMisra_TypeCast(&(pVoutId), &(pMsg));

    if (pMsgCtrl == NULL) {
        PRN_VOUT_ERR_LOG "Fail to proc vout timeout msg - invalid msg control!" PRN_VOUT_ERR_NG
    } else if ((pMsgCtrl->MsgFlag & Flag) == 0ULL) {
        PRN_VOUT_ERR_LOG "Fail to proc vout timeout msg - wrong flag!" PRN_VOUT_ERR_NG
    } else if (pVoutId == NULL) {
        PRN_VOUT_ERR_LOG "Fail to proc vout timeout msg - invalid msg!" PRN_VOUT_ERR_NG
    } else if ((*pVoutId) >= AMBA_DSP_MAX_VOUT_NUM) {
        PRN_VOUT_ERR_LOG "Fail to proc vout timeout msg - invalid VoutID(%d)!"
            PRN_VOUT_ERR_ARG_UINT32 *pVoutId PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_NG
    } else if ((SvcVoutErrCtrl.VoutErrSelectBits & SvcVoutErrTsk_BitGet(*pVoutId)) == 0U) {
        if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_LOG_DBG1) > 0U) {
            PRN_VOUT_ERR_LOG "Fail to proc vout timeout msg - Not supported VoutID(%d)! CurSelectBits 0x%x"
                PRN_VOUT_ERR_ARG_UINT32 (*pVoutId)                       PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrCtrl.VoutErrSelectBits PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_NG
        }
    } else {
        PRN_VOUT_ERR_LOG "VoutID(%s%d%s) isr timeout!"
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_1 PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 *pVoutId                    PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_DBG1

        PRetVal = AmbaKAL_EventFlagSet(&(SvcVoutErrCtrl.ErrState[*pVoutId].VoutStateFlag), SVC_VOUT_ERR_VOUT_STATE_TOUT); PRN_VOUT_ERR_HDLR
    }

    AmbaMisra_TouchUnused(pVoutId);
    AmbaMisra_TouchUnused(&MsgType);
}

static void SvcVoutErrTsk_MonMsgProcLinkState(const void *pMsg, UINT64 Flag, UINT32 MsgType)
{
    const AMBA_MON_VOUT_SERDES_STATE_s *pLinkStateMsg = NULL;
    const SVC_VOUT_ERR_MON_MSG_s      *pMsgCtrl      = NULL;

    if (SvcVoutErrCtrl.MonFrwkCtrl.pMonitorPortCtrl != NULL) {
        if (SvcVoutErrCtrl.MonFrwkCtrl.pMonitorPortCtrl[VOUT_ERR_MON_PORT_VOUT_STATE].pMsgList != NULL) {
            pMsgCtrl = &(SvcVoutErrCtrl.MonFrwkCtrl.pMonitorPortCtrl[VOUT_ERR_MON_PORT_VOUT_STATE].pMsgList[MON_VOUT_STAT_MSG_VOUT_LINK]);
        }
    }

    AmbaMisra_TypeCast(&(pLinkStateMsg), &(pMsg));

    if (pMsgCtrl == NULL) {
        PRN_VOUT_ERR_LOG "Fail to proc link state msg - invalid msg control!" PRN_VOUT_ERR_NG
    } else if ((pMsgCtrl->MsgFlag & Flag) == 0ULL) {
        PRN_VOUT_ERR_LOG "Fail to proc link state msg - wrong flag!" PRN_VOUT_ERR_NG
    } else if (pLinkStateMsg == NULL) {
        PRN_VOUT_ERR_LOG "Fail to proc link state msg - invalid msg!" PRN_VOUT_ERR_NG
    } else if (pLinkStateMsg->VoutId >= AMBA_DSP_MAX_VOUT_NUM) {
        PRN_VOUT_ERR_LOG "Fail to proc link state msg - invalid VoutID(%d)!"
            PRN_VOUT_ERR_ARG_UINT32 pLinkStateMsg->VoutId PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_NG
    } else if ((SvcVoutErrCtrl.VoutErrSelectBits & SvcVoutErrTsk_BitGet(pLinkStateMsg->VoutId)) == 0U) {
        if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_LOG_DBG1) > 0U) {
            PRN_VOUT_ERR_LOG "Fail to proc vout timeout msg - Not supported VoutID(%d)! CurSelectBits 0x%x"
                PRN_VOUT_ERR_ARG_UINT32 pLinkStateMsg->VoutId            PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrCtrl.VoutErrSelectBits PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_NG
        }
    } else {

        PRN_VOUT_ERR_LOG "Monitor VoutID(%s%d%s) link state %s0x%x%s! %d"
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_1 PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 pLinkStateMsg->VoutId       PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_0 PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 pLinkStateMsg->LinkId       PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 MsgType                     PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_DBG1

        SvcVoutErrTsk_LinkStateChange(pLinkStateMsg->VoutId, pLinkStateMsg->LinkId);
    }
}

static void SvcVoutErrTsk_MonMsgProcVoutRdy(const void *pMsg, UINT64 Flag, UINT32 MsgType)
{
    AMBA_DSP_VOUT_DATA_INFO_s *pVoutMsg = NULL;
    const SVC_VOUT_ERR_MON_MSG_s *pMsgCtrl = NULL;

    if (SvcVoutErrCtrl.MonFrwkCtrl.pMonitorPortCtrl != NULL) {
        if (SvcVoutErrCtrl.MonFrwkCtrl.pMonitorPortCtrl[VOUT_ERR_MON_PORT_DSP_LISTEN].pMsgList != NULL) {
            pMsgCtrl = &(SvcVoutErrCtrl.MonFrwkCtrl.pMonitorPortCtrl[VOUT_ERR_MON_PORT_DSP_LISTEN].pMsgList[VOUT_ERR_MON_DSP_LISTEN_MSG_VOUT_RDY]);
        }
    }

    AmbaMisra_TypeCast(&(pVoutMsg), &(pMsg));

    if (pMsgCtrl == NULL) {
        PRN_VOUT_ERR_LOG "Fail to proc vout rdy msg - invalid msg control!" PRN_VOUT_ERR_NG
    } else if ((pMsgCtrl->MsgFlag & Flag) == 0ULL) {
        PRN_VOUT_ERR_LOG "Fail to proc vout rdy msg - wrong flag!" PRN_VOUT_ERR_NG
    } else if (pVoutMsg == NULL) {
        PRN_VOUT_ERR_LOG "Fail to proc vout rdy msg - invalid msg!" PRN_VOUT_ERR_NG
    } else if ((pVoutMsg->VoutIdx) >= AMBA_DSP_MAX_VOUT_NUM) {
        PRN_VOUT_ERR_LOG "Fail to proc vout rdy msg - invalid VoutID(%d)!"
            PRN_VOUT_ERR_ARG_UINT32 pVoutMsg->VoutIdx PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_NG
    } else if ((SvcVoutErrCtrl.VoutSelectBits & SvcVoutErrTsk_BitGet(pVoutMsg->VoutIdx)) == 0U) {
        if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_LOG_DBG1) > 0U) {
            PRN_VOUT_ERR_LOG "Fail to proc vout rdy msg - Not supported VoutID(%d)! CurSelectBits 0x%x"
                PRN_VOUT_ERR_ARG_UINT32 pVoutMsg->VoutIdx             PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrCtrl.VoutSelectBits PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_NG
        }
    } else {

        PRN_VOUT_ERR_LOG "Monitor VoutID(%s%d%s) %sdata%s ready %s!"
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_1                                            PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 pVoutMsg->VoutIdx                                                      PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END                                                PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_1                                            PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END                                                PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_CSTR   ( ( (MsgType & VOUT_ERR_MON_MSG_TIMEOUT) > 0U) ? "timeout" : "ready" ) PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_DBG2

#ifdef CONFIG_SOC_CV2FS
        if ((MsgType & VOUT_ERR_MON_MSG_TIMEOUT) == 0U) {
            UINT32 PRetVal;
            SVC_VOUT_ERR_CTRL_CMD_t ErrCtrlCmd;
            UINT32 Pitch, Width, Height;
            SVC_VOUT_ERR_VOUT_FRZ_MSG_UNIT_s MsgUnit;

            Pitch  = pVoutMsg->YuvBuf.Pitch;         Pitch  &= 0xFFFFU;
            Width  = pVoutMsg->YuvBuf.Window.Width;  Width  &= 0xFFFFU;
            Height = pVoutMsg->YuvBuf.Window.Height; Height &= 0xFFFFU;

            PRN_VOUT_ERR_LOG "  %04d/%04d/%04d, Y/UV(0x%08X/0x%08X), CRC Y/UV(0x%08X/0x%08X)"
                PRN_VOUT_ERR_ARG_UINT32 Pitch                       PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 Width                       PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 Height                      PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 pVoutMsg->YuvBuf.BaseAddrY  PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 pVoutMsg->YuvBuf.BaseAddrUV PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 pVoutMsg->DispImgCrcLuma    PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 pVoutMsg->DispImgCrcChroma  PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_DBG2

            AmbaSvcWrap_MisraMemset(&MsgUnit, 0, sizeof(SVC_VOUT_ERR_VOUT_FRZ_MSG_UNIT_s));
            MsgUnit.MsgID      = SVC_VOUT_ERR_FRZ_MSG_ID_VOUT_RDY;
            MsgUnit.Pitch      = Pitch;
            MsgUnit.Width      = Width;
            MsgUnit.Height     = Height;
            MsgUnit.Data[0]    = (UINT64)(pVoutMsg->YuvBuf.BaseAddrY);
            MsgUnit.Data[1]    = (UINT64)(pVoutMsg->YuvBuf.BaseAddrUV);
#ifndef CONFIG_ARM64
            MsgUnit.Data[0]   &= 0xFFFFFFFFULL;
            MsgUnit.Data[1]   &= 0xFFFFFFFFULL;
#endif
            MsgUnit.Data[2]    = (UINT64)(pVoutMsg->DispImgCrcLuma);   MsgUnit.Data[0] &= 0xFFFFFFFFULL;
            MsgUnit.Data[3]    = (UINT64)(pVoutMsg->DispImgCrcChroma); MsgUnit.Data[1] &= 0xFFFFFFFFULL;
            SvcVoutErrTsk_VoutFrzMsgSned(pVoutMsg->VoutIdx, &MsgUnit);

            AmbaSvcWrap_MisraMemset(&ErrCtrlCmd, 0, sizeof(ErrCtrlCmd));
            ErrCtrlCmd.CmdID   = SVC_VOUT_ERR_CTRL_CMD_VOUT_UPD;
            ErrCtrlCmd.Data[0] = pVoutMsg->VoutIdx;
            ErrCtrlCmd.Data[1] = pVoutMsg->DispImgCrcLuma;
            ErrCtrlCmd.Data[2] = pVoutMsg->DispImgCrcChroma;
            PRetVal = AmbaKAL_MsgQueueSend(&(SvcVoutErrCtrl.ErrCtrlTask.Que), &ErrCtrlCmd, AMBA_KAL_NO_WAIT); PRN_VOUT_ERR_HDLR
        }
#endif
    }

    AmbaMisra_TouchUnused(pVoutMsg);
}

static void SvcVoutErrTsk_MonMsgProcDispRdy(const void *pMsg, UINT64 Flag, UINT32 MsgType)
{
    UINT32 PRetVal;
    const AMBA_DSP_YUV_DATA_RDY_EXTEND_s *pDispMsg = NULL;
    const SVC_VOUT_ERR_MON_MSG_s *pMsgCtrl = NULL;

    if (SvcVoutErrCtrl.MonFrwkCtrl.pMonitorPortCtrl != NULL) {
        if (SvcVoutErrCtrl.MonFrwkCtrl.pMonitorPortCtrl[VOUT_ERR_MON_PORT_DSP_LISTEN].pMsgList != NULL) {
            pMsgCtrl = &(SvcVoutErrCtrl.MonFrwkCtrl.pMonitorPortCtrl[VOUT_ERR_MON_PORT_DSP_LISTEN].pMsgList[VOUT_ERR_MON_DSP_LISTEN_MSG_DISP_RDY]);
        }
    }

    AmbaMisra_TypeCast(&(pDispMsg), &(pMsg));

    if (pMsgCtrl == NULL) {
        PRN_VOUT_ERR_LOG "Fail to proc disp rdy msg - invalid msg control!" PRN_VOUT_ERR_NG
    } else if ((pMsgCtrl->MsgFlag & Flag) == 0ULL) {
        PRN_VOUT_ERR_LOG "Fail to proc disp rdy msg - wrong flag!" PRN_VOUT_ERR_NG
    } else if (pDispMsg == NULL) {
        PRN_VOUT_ERR_LOG "Fail to proc disp rdy msg - invalid msg!" PRN_VOUT_ERR_NG
    } else {
        UINT32 VoutID = pDispMsg->ViewZoneId;

        if (VoutID >= AMBA_DSP_MAX_VOUT_NUM) {
            PRN_VOUT_ERR_LOG "Fail to proc disp rdy msg - invalid VoutID(%d)!"
                PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_NG
        } else if ((SvcVoutErrCtrl.VoutSelectBits & SvcVoutErrTsk_BitGet(VoutID)) == 0U) {
            if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_LOG_DBG1) > 0U) {
                PRN_VOUT_ERR_LOG "Fail to proc disp rdy msg - Not supported VoutID(%d)! CurSelectBits 0x%x"
                    PRN_VOUT_ERR_ARG_UINT32 VoutID                        PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrCtrl.VoutSelectBits PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_NG
            }
        } else {
            PRN_VOUT_ERR_LOG "Monitor VoutID(%s%d%s) %sdisplay%s ready %s! 0x%X"
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_1                                            PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 VoutID                                                                 PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END                                                PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_2                                            PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END                                                PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   ( ( (MsgType & VOUT_ERR_MON_MSG_TIMEOUT) > 0U) ? "timeout" : "ready" ) PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 pDispMsg->SourceViewBit                                                PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_DBG2

            if ((MsgType & VOUT_ERR_MON_MSG_TIMEOUT) == 0U) {
                SVC_VOUT_ERR_CTRL_CMD_t ErrCtrlCmd;
                SVC_VOUT_ERR_VOUT_FRZ_MSG_UNIT_s MsgUnit;

                AmbaSvcWrap_MisraMemset(&MsgUnit, 0, sizeof(SVC_VOUT_ERR_VOUT_FRZ_MSG_UNIT_s));
                MsgUnit.MsgID      = SVC_VOUT_ERR_FRZ_MSG_ID_DISP_RDY;
                MsgUnit.Pitch      = pDispMsg->Buffer.Pitch;         MsgUnit.Pitch  &= 0xFFFFU;
                MsgUnit.Width      = pDispMsg->Buffer.Window.Width;  MsgUnit.Width  &= 0xFFFFU;
                MsgUnit.Height     = pDispMsg->Buffer.Window.Height; MsgUnit.Height &= 0xFFFFU;
                MsgUnit.Data[0]    = (UINT64)(pDispMsg->Buffer.BaseAddrY);
                MsgUnit.Data[1]    = (UINT64)(pDispMsg->Buffer.BaseAddrUV);
#ifndef CONFIG_ARM64
                MsgUnit.Data[0]   &= 0xFFFFFFFFULL;
                MsgUnit.Data[1]   &= 0xFFFFFFFFULL;
#endif
                MsgUnit.Data[2]    = pDispMsg->CapPts;
                MsgUnit.Data[3]    = pDispMsg->YuvPts;
                SvcVoutErrTsk_VoutFrzMsgSned(VoutID, &MsgUnit);

                AmbaSvcWrap_MisraMemset(&ErrCtrlCmd, 0, sizeof(ErrCtrlCmd));
                ErrCtrlCmd.CmdID   = SVC_VOUT_ERR_CTRL_CMD_DISP_UPD;
                ErrCtrlCmd.Data[0] = VoutID;
                ErrCtrlCmd.Data[1] = pDispMsg->SourceViewBit;
                PRetVal = AmbaKAL_MsgQueueSend(&(SvcVoutErrCtrl.ErrCtrlTask.Que), &ErrCtrlCmd, AMBA_KAL_NO_WAIT); PRN_VOUT_ERR_HDLR
            }
        }
    }
}

static void SvcVoutErrTsk_MonMsgInit(void)
{
    if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_MON_MSG_INIT) == 0U) {

        AmbaSvcWrap_MisraMemset(VoutErrMonVoutStateMsg, 0, sizeof(VoutErrMonVoutStateMsg));
        VoutErrMonVoutStateMsg[MON_VOUT_STAT_MSG_VOUT_START] = (SVC_VOUT_ERR_MON_MSG_s) { 1U, 0ULL, "isr_start",   SvcVoutErrTsk_MonMsgProcVoutStart  };
        VoutErrMonVoutStateMsg[MON_VOUT_STAT_MSG_VOUT_TOUT]  = (SVC_VOUT_ERR_MON_MSG_s) { 1U, 0ULL, "isr_timeout", SvcVoutErrTsk_MonMsgProcVoutTout   };
        VoutErrMonVoutStateMsg[MON_VOUT_STAT_MSG_VOUT_LINK]  = (SVC_VOUT_ERR_MON_MSG_s) { 0U, 0ULL, "serdes_link", SvcVoutErrTsk_MonMsgProcLinkState  };

        AmbaSvcWrap_MisraMemset(VoutErrMonDspListenMsg, 0, sizeof(VoutErrMonDspListenMsg));
        VoutErrMonDspListenMsg[VOUT_ERR_MON_DSP_LISTEN_MSG_VOUT_RDY]  = (SVC_VOUT_ERR_MON_MSG_s) { 0U, 0ULL, "vout_rdy", SvcVoutErrTsk_MonMsgProcVoutRdy };
        VoutErrMonDspListenMsg[VOUT_ERR_MON_DSP_LISTEN_MSG_DISP_RDY]  = (SVC_VOUT_ERR_MON_MSG_s) { 0U, 0ULL, "disp_rdy", SvcVoutErrTsk_MonMsgProcDispRdy };

        AmbaSvcWrap_MisraMemset(VoutErrMonMsgPortist, 0, sizeof(VoutErrMonMsgPortist));
        VoutErrMonMsgPortist[VOUT_ERR_MON_PORT_VOUT_STATE] = (SVC_VOUT_ERR_MON_PORT_s) { 1U, VOUT_ERR_MON_PORT_VOUT_STATE, "vout_state", MON_VOUT_STAT_MSG_NUM,  VoutErrMonVoutStateMsg };
        VoutErrMonMsgPortist[VOUT_ERR_MON_PORT_DSP_LISTEN] = (SVC_VOUT_ERR_MON_PORT_s) { 0U, VOUT_ERR_MON_PORT_DSP_LISTEN, "timeline",   VOUT_ERR_MON_DSP_LISTEN_MSG_NUM, VoutErrMonDspListenMsg };

#ifdef CONFIG_BUILD_MONFRW_SERDES
        VoutErrMonVoutStateMsg[MON_VOUT_STAT_MSG_VOUT_LINK].Enable = 1U;
#endif

#ifdef CONFIG_SOC_CV2FS
        /* Only CV2FS support VOUT freeze - CRC detection */
        VoutErrMonMsgPortist[VOUT_ERR_MON_PORT_DSP_LISTEN].Enable = 1U;
        VoutErrMonDspListenMsg[VOUT_ERR_MON_DSP_LISTEN_MSG_VOUT_RDY].Enable = 1U;
        VoutErrMonDspListenMsg[VOUT_ERR_MON_DSP_LISTEN_MSG_DISP_RDY].Enable = 1U;
#endif

        SvcVoutErrCtrlFlg |= SVC_VOUT_ERR_FLG_MON_MSG_INIT;
    }
}

static UINT32 SvcVoutErrTsk_MonFrwkCfg(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    SVC_VOUT_ERR_MON_CTRL_s *pMonFrwkCtrl = &(SvcVoutErrCtrl.MonFrwkCtrl);
    UINT32 PortIdx;

    // Open monitor msg port
    SvcVoutErrTsk_MonMsgInit();

    pMonFrwkCtrl->NumOfMonitorPort = VOUT_ERR_MON_PORT_NUM;
    pMonFrwkCtrl->pMonitorPortCtrl = VoutErrMonMsgPortist;

    for (PortIdx = 0U; PortIdx < pMonFrwkCtrl->NumOfMonitorPort; PortIdx ++) {

        AmbaSvcWrap_MisraMemset(&(pMonFrwkCtrl->MonitorPort[PortIdx]), 0, sizeof(AMBA_MON_MESSAGE_PORT_s));

        if ( pMonFrwkCtrl->pMonitorPortCtrl[PortIdx].Enable > 0U ) {
            PRetVal = AmbaMonMessage_Open(&(pMonFrwkCtrl->MonitorPort[PortIdx]),
                                          pMonFrwkCtrl->pMonitorPortCtrl[PortIdx].MsgPortName,
                                          NULL);

            if (PRetVal != 0U) {
                RetVal = SVC_NG;

                PRN_VOUT_ERR_LOG "Fail to create vout err control - open '%s' monitor port fail"
                    PRN_VOUT_ERR_ARG_CSTR   pMonFrwkCtrl->pMonitorPortCtrl[PortIdx].MsgPortName PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_NG
            }
        }

        if (RetVal != SVC_OK) {
            break;
        }
    }

    // create satety msg task
    if (RetVal == SVC_OK) {
        SVC_VOUT_ERR_MON_TASK_s *pMonFrwkTask = &(SvcVoutErrCtrl.MonFrwkTask);
        UINT32 TaskIdx;

        pMonFrwkTask->NumOfTask = VOUT_ERR_MON_PORT_NUM;

        for (TaskIdx = 0U; TaskIdx < pMonFrwkTask->NumOfTask; TaskIdx ++) {

            AmbaSvcWrap_MisraMemset(pMonFrwkTask->Name[TaskIdx], 0, 32);

            pMonFrwkTask->Ctrl[TaskIdx].CpuBits  = SVC_VOUT_ERR_TASK_CPU_BITS;
            pMonFrwkTask->Ctrl[TaskIdx].Priority = SVC_VOUT_ERR_MON_TASK_PRI;
            if (SvcVoutErrCtrl.MonFrwkCtrl.pMonitorPortCtrl != NULL) {
                pMonFrwkTask->Ctrl[TaskIdx].EntryArg = SvcVoutErrCtrl.MonFrwkCtrl.pMonitorPortCtrl[TaskIdx].PortID;

                /* %s_%s */
                  AmbaUtility_StringCopy(pMonFrwkTask->Name[TaskIdx], 32U, SvcVoutErrCtrl.Name);
                AmbaUtility_StringAppend(pMonFrwkTask->Name[TaskIdx], 32U, "_");
                AmbaUtility_StringAppend(pMonFrwkTask->Name[TaskIdx], 32U, SvcVoutErrCtrl.MonFrwkCtrl.pMonitorPortCtrl[TaskIdx].MsgPortName);

            } else {
                pMonFrwkTask->Ctrl[TaskIdx].EntryArg = 0U;
                SvcWrap_strcpy(pMonFrwkTask->Name[TaskIdx], 32, SvcVoutErrCtrl.Name);
            }
            pMonFrwkTask->Ctrl[TaskIdx].EntryFunc = SvcVoutErrTsk_MonFrwkTskEntry;
            pMonFrwkTask->Ctrl[TaskIdx].StackSize = SVC_VOUT_ERR_STACK_SZ;
            pMonFrwkTask->Ctrl[TaskIdx].pStackBase = &(pMonFrwkTask->Stack[TaskIdx][0]);

            PRetVal = SvcTask_Create(pMonFrwkTask->Name[TaskIdx], &(pMonFrwkTask->Ctrl[TaskIdx]));
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_VOUT_ERR_LOG "Fail to create vout err control - create monitor task fail! ErrCode 0x%x"
                    PRN_VOUT_ERR_ARG_UINT32 PRetVal PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_NG
            } else {
                PRN_VOUT_ERR_LOG "Successful to create vout err monitor task - %s"
                    PRN_VOUT_ERR_ARG_CSTR   pMonFrwkTask->Name[TaskIdx] PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_DBG1
            }

            if (RetVal != SVC_OK) {
                break;
            }
        }
    }

    return RetVal;
}

static void* SvcVoutErrTsk_MonFrwkTskEntry(void* EntryArg)
{
    const ULONG  *pArg;
    UINT32 RetVal = SVC_OK, PRetVal;
    UINT32 MonitorPortID;
    INT32  TimeOut = 10000;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);
    MonitorPortID = (UINT32)(*pArg);

    while ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_INIT) == 0U) {
        if (TimeOut > 0) {
            PRetVal = AmbaKAL_TaskSleep(10U); PRN_VOUT_ERR_HDLR
            TimeOut -= 10;
        } else {
            RetVal = SVC_NG;
            break;
        }
    }

    if (RetVal != SVC_OK) {
        PRN_VOUT_ERR_LOG "Fail to process task - create vout err control first!" PRN_VOUT_ERR_NG
    } else if (MonitorPortID >= VOUT_ERR_MON_PORT_NUM) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to process task - invalid monitor port id(%d)"
            PRN_VOUT_ERR_ARG_UINT32 MonitorPortID PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_NG
    } else {
        AMBA_MON_MESSAGE_PORT_s  *pMonitorPort     = &(SvcVoutErrCtrl.MonFrwkCtrl.MonitorPort[MonitorPortID]);
        SVC_VOUT_ERR_MON_PORT_s *pMonitorPortCtrl = &(SvcVoutErrCtrl.MonFrwkCtrl.pMonitorPortCtrl[MonitorPortID]);
        SVC_VOUT_ERR_MON_MSG_s  *pMonitorMsg      = NULL;
        UINT32 MsgIdx, MsgType, EventID;
        UINT64 MsgFlag;
        UINT64 EventFlag = 0ULL;
        UINT64 ActualFlags;
        void *pMsg, *pStateMsg = NULL;
        AMBA_MON_MESSAGE_HEADER_s *pMsgHeader = NULL;
        UINT8 *pMsgData = NULL;
        UINT32 LinkAddr;

        AmbaMisra_TouchUnused(&pMonitorPort    );
        AmbaMisra_TouchUnused(pMonitorPortCtrl);
        AmbaMisra_TouchUnused(pMonitorMsg     );

        for (MsgIdx = 0U; MsgIdx < pMonitorPortCtrl->NumOfMsg; MsgIdx ++) {

            pMonitorMsg = &(pMonitorPortCtrl->pMsgList[MsgIdx]);

            if (pMonitorMsg->Enable > 0U) {
                /* Configure msg */
                PRetVal = AmbaMonMessage_Find(pMonitorPort, pMonitorMsg->MsgName, &EventID, &(pMonitorMsg->MsgFlag));
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    PRN_VOUT_ERR_LOG "Fail to process task - find msg '%s' in monitor port '%s' fail!"
                        PRN_VOUT_ERR_ARG_CSTR   pMonitorMsg->MsgName          PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_CSTR   pMonitorPortCtrl->MsgPortName PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_NG
                    break;
                } else {
                    EventFlag |= pMonitorMsg->MsgFlag;
                    PRN_VOUT_ERR_LOG "Successful to find msg '%s' in monitor port '%s'. Flag 0x%llx"
                        PRN_VOUT_ERR_ARG_CSTR   pMonitorMsg->MsgName          PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_CSTR   pMonitorPortCtrl->MsgPortName PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT64 pMonitorMsg->MsgFlag
                    PRN_VOUT_ERR_DBG1
                }
            }
        }

        while (EventFlag > 0U) {

            ActualFlags = AmbaMonMessage_Wait(pMonitorPort, EventFlag, (UINT32) MON_EVENT_FLAG_OR_CLR, (UINT32) AMBA_KAL_WAIT_FOREVER);
            if ((ActualFlags & EventFlag) > 0ULL) {
                do {
                    PRetVal = AmbaMonMessage_Get(pMonitorPort, &pMsg);

                    if (PRetVal == 0U) {
                        /* msg get */
                        AmbaMisra_TypeCast(&(pMsgHeader), &(pMsg));
                        if (pMsgHeader != NULL) {

                            MsgFlag = pMsgHeader->Ctx.Event.Flag;
                            MsgType = 0U;

                            if ( pMsgHeader->Ctx.Com.Id == (UINT8) AMBA_MON_MSG_ID_LINK ) {
                                LinkAddr = (UINT32) (pMsgHeader->Ctx.Link.Pointer);
                                AmbaMisra_TypeCast(&(pStateMsg), &(LinkAddr));
                                AmbaMisra_TypeCast(&(pMsgHeader), &(LinkAddr));
                            } else {
                                AmbaMisra_TypeCast(&(pStateMsg), &(pMsgHeader));
                            }

                            if (pMsgHeader->Ctx.Chunk.Timeout != 0U) {
                                MsgType |= VOUT_ERR_MON_MSG_TIMEOUT;
                            }

                            for (MsgIdx = 0U; MsgIdx < pMonitorPortCtrl->NumOfMsg; MsgIdx ++) {
                                pMonitorMsg = &(pMonitorPortCtrl->pMsgList[MsgIdx]);
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
                            PRN_VOUT_ERR_LOG "Warning to process task - get invalid msg header" PRN_VOUT_ERR_DBG1
                        }
                    }
                } while (PRetVal == 0U);

                AmbaMisra_TouchUnused(&pMsg);
                AmbaMisra_TouchUnused(pStateMsg);
                AmbaMisra_TouchUnused(pMsgHeader);
                AmbaMisra_TouchUnused(&EventFlag);
            }
        }
        AmbaMisra_TouchUnused(pMsgData);
    }

    AmbaMisra_TouchUnused(&RetVal);

    return NULL;
}

static void SvcVoutErrTsk_DevInfoInit(void)
{
    if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_DEV_INIT) == 0U) {
        UINT32 CurDevNum = 0U;

        AmbaSvcWrap_MisraMemset(SvcVoutErrDevInfo, 0, sizeof(SvcVoutErrDevInfo));

        // HDMI without serdes
        SvcWrap_strcpy(SvcVoutErrDevInfo[CurDevNum].DevName, sizeof(SvcVoutErrDevInfo[CurDevNum].DevName), "HDMI");
        SvcVoutErrDevInfo[CurDevNum].DevModeNum = 1U;
        AmbaSvcWrap_MisraMemset(SvcVoutErrDevInfo[CurDevNum].DevSerDesSelectBits, 1, sizeof(SvcVoutErrDevInfo[CurDevNum].DevSerDesSelectBits));
        SvcVoutErrDevInfo[CurDevNum].EnableErrCtrl = 0U;
        CurDevNum ++;

        // LCD without serdes
        /*
        SvcWrap_strcpy(SvcVoutErrDevInfo[CurDevNum].DevName, sizeof(SvcVoutErrDevInfo[CurDevNum].DevName), "T30P61");
        SvcVoutErrDevInfo[CurDevNum].DevModeNum = 1U;
        AmbaSvcWrap_MisraMemset(SvcVoutErrDevInfo[CurDevNum].DevSerDesSelectBits, 1, sizeof(SvcVoutErrDevInfo[CurDevNum].DevSerDesSelectBits));
        SvcVoutErrDevInfo[CurDevNum].EnableErrCtrl = 1U;
        CurDevNum ++;
        */

        // TFT1280769, maxim 96755 + 96752 )
        SvcWrap_strcpy(SvcVoutErrDevInfo[CurDevNum].DevName, sizeof(SvcVoutErrDevInfo[CurDevNum].DevName), "MAXIM TFT1280768");
        SvcVoutErrDevInfo[CurDevNum].DevModeNum = 6U;
        SvcVoutErrDevInfo[CurDevNum].DevSerDesSelectBits[AMBA_FPD_MAXIM_DUAL_60HZ     ] = 0x3U;
        SvcVoutErrDevInfo[CurDevNum].DevSerDesSelectBits[AMBA_FPD_MAXIM_DUAL_A60HZ    ] = 0x3U;
        SvcVoutErrDevInfo[CurDevNum].DevSerDesSelectBits[AMBA_FPD_MAXIM_SINGLE_A_60HZ ] = 0x1U;
        SvcVoutErrDevInfo[CurDevNum].DevSerDesSelectBits[AMBA_FPD_MAXIM_SINGLE_A_A60HZ] = 0x1U;
        SvcVoutErrDevInfo[CurDevNum].DevSerDesSelectBits[AMBA_FPD_MAXIM_SINGLE_B_60HZ ] = 0x1U;
        SvcVoutErrDevInfo[CurDevNum].DevSerDesSelectBits[AMBA_FPD_MAXIM_SINGLE_B_A60HZ] = 0x1U;
        SvcVoutErrDevInfo[CurDevNum].EnableErrCtrl = 1U;
        CurDevNum ++;

        // Dual TFT1280769, maxim 96751 + 96752 )
        SvcWrap_strcpy(SvcVoutErrDevInfo[CurDevNum].DevName, sizeof(SvcVoutErrDevInfo[CurDevNum].DevName), "MAXIM Daul TFT1280768");
        SvcVoutErrDevInfo[CurDevNum].DevModeNum = MAXIM_STATE_VIEW_NUM;
        SvcVoutErrDevInfo[CurDevNum].DevSerDesSelectBits[MAXIM_NO_LINK                 ] = 0x1U;
        SvcVoutErrDevInfo[CurDevNum].DevSerDesSelectBits[MAXIM_MULTIVIEW_A_60Hz        ] = 0x1U;
        SvcVoutErrDevInfo[CurDevNum].DevSerDesSelectBits[MAXIM_MULTIVIEW_B_60Hz        ] = 0x2U;
        SvcVoutErrDevInfo[CurDevNum].DevSerDesSelectBits[MAXIM_MULTIVIEW_SPLITTER_60HZ ] = 0x3U;
        SvcVoutErrDevInfo[CurDevNum].DevSerDesSelectBits[MAXIM_SINGLEVIEW_A_60HZ       ] = 0x1U;
        SvcVoutErrDevInfo[CurDevNum].DevSerDesSelectBits[MAXIM_SINGLEVIEW_B_60HZ       ] = 0x1U;
        SvcVoutErrDevInfo[CurDevNum].DevSerDesSelectBits[MAXIM_SINGLEVIEW_AUTO_60HZ    ] = 0x1U;
        SvcVoutErrDevInfo[CurDevNum].DevSerDesSelectBits[MAXIM_MULTIVIEW_A_A60HZ       ] = 0x1U;
        SvcVoutErrDevInfo[CurDevNum].DevSerDesSelectBits[MAXIM_MULTIVIEW_B_A60HZ       ] = 0x2U;
        SvcVoutErrDevInfo[CurDevNum].DevSerDesSelectBits[MAXIM_MULTIVIEW_SPLITTER_A60HZ] = 0x3U;
        SvcVoutErrDevInfo[CurDevNum].DevSerDesSelectBits[MAXIM_SINGLEVIEW_A_A60HZ      ] = 0x1U;
        SvcVoutErrDevInfo[CurDevNum].DevSerDesSelectBits[MAXIM_SINGLEVIEW_B_A60HZ      ] = 0x1U;
        SvcVoutErrDevInfo[CurDevNum].DevSerDesSelectBits[MAXIM_SINGLEVIEW_AUTO_A60HZ   ] = 0x1U;
        SvcVoutErrDevInfo[CurDevNum].EnableErrCtrl = 1U;
        CurDevNum ++;

        // ZS095BH3001A8H3 BII, maxim 96755 + 96752 )
        SvcWrap_strcpy(SvcVoutErrDevInfo[CurDevNum].DevName, sizeof(SvcVoutErrDevInfo[CurDevNum].DevName), "MAXIM ZS095BH3001A8H3 BII");
        SvcVoutErrDevInfo[CurDevNum].DevModeNum = AMBA_FPD_MAXIM_SERDES_NUM_MODE;
        SvcVoutErrDevInfo[CurDevNum].DevSerDesSelectBits[AMBA_FPD_MAXIM_60HZ]  = 0x1U;
        SvcVoutErrDevInfo[CurDevNum].DevSerDesSelectBits[AMBA_FPD_MAXIM_A60HZ] = 0x1U;
        SvcVoutErrDevInfo[CurDevNum].EnableErrCtrl = 1U;
        CurDevNum ++;

        AmbaMisra_TouchUnused(&CurDevNum);

        SvcVoutErrCtrlFlg |= SVC_VOUT_ERR_FLG_DEV_INIT;

        if ( ( SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_LOG_DBG1 ) > 0U ) {
            SvcVoutErrTsk_DevInfoPrn();
        }
    }
}

static SVC_VOUT_ERR_DEV_INFO_s *SvcVoutErrTsk_DevInfoGet(UINT32 VoutID)
{
    SVC_VOUT_ERR_DEV_INFO_s *pDevInfo = NULL;
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();

    if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_DEV_INIT) == 0U) {
        PRN_VOUT_ERR_LOG "Fail to get device info - initial device info first!" PRN_VOUT_ERR_NG
    } else if (VoutID >= AMBA_DSP_MAX_VOUT_NUM) {
        PRN_VOUT_ERR_LOG "Fail to get device info - invalid VoutID(%d)"
            PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_NG
    } else if (pCfg == NULL) {
        PRN_VOUT_ERR_LOG "Fail to get device info - invalid resolution configuration!" PRN_VOUT_ERR_NG
    } else {
        UINT32 DispIdx, DevIdx;
        const AMBA_FPD_OBJECT_s *pFPDObj;

        for (DispIdx = 0U; DispIdx < pCfg->DispNum; DispIdx ++) {
            if (pCfg->DispStrm[DispIdx].VoutID == VoutID) {

                AmbaMisra_TypeCast(&(pFPDObj), &(pCfg->DispStrm[DispIdx].pDriver));
                if (pFPDObj != NULL) {
                    for (DevIdx = 0U; DevIdx < SVC_VOUT_ERR_SUPPORTED_DEV_NUM; DevIdx ++) {
                        if (0 == SvcWrap_strcmp(pFPDObj->pName, SvcVoutErrDevInfo[DevIdx].DevName)) {
                            pDevInfo = &SvcVoutErrDevInfo[DevIdx];
                        }
                    }
                }

                break;
            }
        }
    }

    return pDevInfo;
}

static void SvcVoutErrTsk_DevInfoPrn(void)
{
    if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_DEV_INIT) > 0U) {
        UINT32 DevIdx, ModeIdx;

        PRN_VOUT_ERR_LOG " " PRN_VOUT_ERR_API
        PRN_VOUT_ERR_LOG "---- %sSupported Device Info%s ----"
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_0 PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_API

        for (DevIdx = 0U; DevIdx < SVC_VOUT_ERR_SUPPORTED_DEV_NUM; DevIdx ++) {
            if (SvcVoutErrDevInfo[DevIdx].DevModeNum > 0U) {
                PRN_VOUT_ERR_LOG "Configure No.%02d Supported Device Info - %s"
                    PRN_VOUT_ERR_ARG_UINT32 DevIdx                            PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_CSTR   SvcVoutErrDevInfo[DevIdx].DevName PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_API
                PRN_VOUT_ERR_LOG "  Dev Mode Id | SerDes Select Bits" PRN_VOUT_ERR_API
                for (ModeIdx = 0U; ModeIdx < SvcVoutErrDevInfo[DevIdx].DevModeNum; ModeIdx ++) {
                    PRN_VOUT_ERR_LOG "          %03d | 0x%08x"
                        PRN_VOUT_ERR_ARG_UINT32 ModeIdx                                                PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrDevInfo[DevIdx].DevSerDesSelectBits[ModeIdx] PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_API

                }
            }
        }
    }
}

static UINT32 SvcVoutErrTsk_OsdCreate(void)
{
    UINT32 RetVal = SVC_OK, ErrCode;
    const SVC_RES_CFG_s* pResCfg = SvcResCfg_Get();

    if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_OSD_INIT) > 0U) {
        PRN_VOUT_ERR_LOG "Vout err ctrl osd has been created!" PRN_VOUT_ERR_API
    } else if (pResCfg == NULL) {
        PRN_VOUT_ERR_LOG "Fail to create vout err osd ctrl - invalid res_cfg" PRN_VOUT_ERR_API
    } else {
        SVC_VOUT_ERR_OSD_CTRL_s     *pCtrl = &SvcVoutErrOsdCtrl;
        SVC_VOUT_ERR_OSD_PTN_INFO_s *pPtnInfo  = pCtrl->OsdPtnInfo;
        SVC_VOUT_ERR_OSD_INFO_s     *pOsdInfo  = pCtrl->OsdInfo;
        SVC_VOUT_ERR_OSD_DRAW_s     *pDrawCtrl = pCtrl->DrawCtrl;
        UINT32 Idx, PtnIdx = 0U, VoutID;
        UINT32 MaxVoutW, MaxVoutH, VoutW, VoutH, DiffVal;

        AmbaSvcWrap_MisraMemset(pCtrl, 0, sizeof(SVC_VOUT_ERR_OSD_CTRL_s));

        /* Configure osd pattern */
        SvcWrap_strcpy(pPtnInfo->StrPtn, SVC_VOUT_ERR_OSD_STR_PTN_LEN, SVC_VOUT_ERR_OSD_PTN_SIG_LOST);
        pPtnInfo->StrPtnLen = SvcWrap_strlen(pPtnInfo->StrPtn);
        pPtnInfo->PtnID = SVC_VOUT_ERR_OSD_PTN_ID_SIG_LST;
        pPtnInfo->PtnWidth = pPtnInfo->StrPtnLen * ( SVC_OSD_CHAR_WIDTH + SVC_OSD_CHAR_GAP );
        pPtnInfo->PtnHeight = SVC_OSD_CHAR_HEIGHT;
        PRN_VOUT_ERR_LOG "OsdStrPtn : %s, Len(%d), Width(%d), Height(%d)"
            PRN_VOUT_ERR_ARG_CSTR   pPtnInfo->StrPtn    PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 pPtnInfo->StrPtnLen PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 pPtnInfo->PtnWidth  PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 pPtnInfo->PtnHeight PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_DBG1
        pPtnInfo ++;

        SvcWrap_strcpy(pPtnInfo->StrPtn, SVC_VOUT_ERR_OSD_STR_PTN_LEN, SVC_VOUT_ERR_OSD_PTN_VOUT_FREEZE);
        pPtnInfo->StrPtnLen = SvcWrap_strlen(pPtnInfo->StrPtn);
        pPtnInfo->PtnID = SVC_VOUT_ERR_OSD_PTN_ID_VOUT_FRZ;
        pPtnInfo->PtnWidth = pPtnInfo->StrPtnLen * ( SVC_OSD_CHAR_WIDTH + SVC_OSD_CHAR_GAP );
        pPtnInfo->PtnHeight = SVC_OSD_CHAR_HEIGHT;
        PRN_VOUT_ERR_LOG "OsdStrPtn : %s, Len(%d), Width(%d), Height(%d)"
            PRN_VOUT_ERR_ARG_CSTR   pPtnInfo->StrPtn    PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 pPtnInfo->StrPtnLen PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 pPtnInfo->PtnWidth  PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 pPtnInfo->PtnHeight PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_DBG1

        /* Configure osd info */
        for (Idx = 0U; Idx < pResCfg->DispNum; Idx ++) {
            VoutID = pResCfg->DispStrm[Idx].VoutID;

            if (VoutID < AMBA_DSP_MAX_VOUT_NUM) {
                pCtrl->SelectBits |= SvcVoutErrTsk_BitGet(VoutID);

                pOsdInfo[VoutID].DrawLevel   = SVC_VOUT_ERR_GUI_CANVAS_LVL;
                pOsdInfo[VoutID].pDrawFunc   = SvcVoutErrTsk_OsdDrawFunc;
                pOsdInfo[VoutID].pUpdateFunc = SvcVoutErrTsk_OsdUpdateFunc;

                ErrCode = SvcOsd_GetOsdBufSize(VoutID, &(pOsdInfo[VoutID].Width), &(pOsdInfo[VoutID].Height));
                if (ErrCode != 0U) {
                    RetVal = SVC_NG;
                    PRN_VOUT_ERR_LOG "Fail to create vout err osd ctrl - get vout(%d) osd buffer size fail! ErrCode(0x%08X)"
                        PRN_VOUT_ERR_ARG_UINT32 VoutID  PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 ErrCode PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_NG
                    break;
                } else {
                    AmbaMisra_TypeCast(&(VoutW),    &(pResCfg->DispStrm[Idx].StrmCfg.Win.Width));     VoutW    &= 0xFFFFU;
                    AmbaMisra_TypeCast(&(VoutH),    &(pResCfg->DispStrm[Idx].StrmCfg.Win.Height));    VoutH    &= 0xFFFFU;
                    AmbaMisra_TypeCast(&(MaxVoutW), &(pResCfg->DispStrm[Idx].StrmCfg.MaxWin.Width));  MaxVoutW &= 0xFFFFU;
                    AmbaMisra_TypeCast(&(MaxVoutH), &(pResCfg->DispStrm[Idx].StrmCfg.MaxWin.Height)); MaxVoutH &= 0xFFFFU;

                    if (MaxVoutW > VoutW) {
                        DiffVal = ( MaxVoutW - VoutW ) >> 1U;
                        pOsdInfo[VoutID].DrawWin.StartX = ( DiffVal * pOsdInfo[VoutID].Width ) / MaxVoutW;
                        pOsdInfo[VoutID].DrawWin.Width  = pOsdInfo[VoutID].Width - ( DiffVal << 1U );
                    } else {
                        pOsdInfo[VoutID].DrawWin.StartX = 0U;
                        pOsdInfo[VoutID].DrawWin.Width  = pOsdInfo[VoutID].Width;
                    }
                    // pOsdInfo[VoutID].DrawWin.Width  = (pOsdInfo[VoutID].DrawWin.Width >> 1U);

                    if (MaxVoutH > VoutH) {
                        DiffVal = ( MaxVoutH - VoutH ) >> 1U;
                        pOsdInfo[VoutID].DrawWin.StartY = ( DiffVal * pOsdInfo[VoutID].Height ) / MaxVoutH;
                        pOsdInfo[VoutID].DrawWin.Height = pOsdInfo[VoutID].Height - ( DiffVal << 1U );
                    } else {
                        pOsdInfo[VoutID].DrawWin.StartY = 0U;
                        pOsdInfo[VoutID].DrawWin.Height = pOsdInfo[VoutID].Height;
                    }
                    pOsdInfo[VoutID].DrawWin.Height = (pOsdInfo[VoutID].DrawWin.Height >> 1U);
                }
            }
        }

        /* Configure work control */
        if (RetVal == SVC_OK) {
            UINT32 FontSize;

            pPtnInfo = pCtrl->OsdPtnInfo;

            for (VoutID = 0U; VoutID < AMBA_DSP_MAX_VOUT_NUM; VoutID ++) {
                if ((pCtrl->SelectBits & SvcVoutErrTsk_BitGet(VoutID)) > 0U) {
                    pDrawCtrl[VoutID].OsdPtnSelectBits = 0U;

                    for (PtnIdx = 0U; PtnIdx < SVC_VOUT_ERR_OSD_PTN_NUM; PtnIdx ++) {
                        if (pPtnInfo[PtnIdx].StrPtnLen > 0U) {
                            pDrawCtrl[VoutID].DrawStrCnt[PtnIdx] = pPtnInfo[PtnIdx].StrPtnLen;

                            pDrawCtrl[VoutID].DrawStrWin[PtnIdx].Width = (pOsdInfo[VoutID].DrawWin.Width * 80U) / 100U;

                            FontSize = pDrawCtrl[VoutID].DrawStrWin[PtnIdx].Width / pPtnInfo[PtnIdx].PtnWidth;

#ifdef CONFIG_SVC_LVGL_USED
                            if (FontSize >= 5U) {
                                FontSize = 5U;
                            } else if (FontSize >= 2U) {
                                FontSize = 2U;
                            } else {
                                FontSize = 1U;
                            }
#endif

                            pDrawCtrl[VoutID].DrawStrWin[PtnIdx].Width  = FontSize * pPtnInfo[PtnIdx].PtnWidth;
                            pDrawCtrl[VoutID].DrawStrWin[PtnIdx].Height = FontSize * pPtnInfo[PtnIdx].PtnHeight;
                            pDrawCtrl[VoutID].DrawStrWin[PtnIdx].StartX = pOsdInfo[VoutID].DrawWin.StartX + ( ( pOsdInfo[VoutID].DrawWin.Width -  pDrawCtrl[VoutID].DrawStrWin[PtnIdx].Width ) / 2U );
                            //pDrawCtrl[VoutID].DrawStrWin[PtnIdx].StartY = pOsdInfo[VoutID].DrawWin.StartY + ( ( pOsdInfo[VoutID].DrawWin.Height - pDrawCtrl[VoutID].DrawStrWin[PtnIdx].Height ) / 2U );
                            pDrawCtrl[VoutID].DrawStrWin[PtnIdx].StartY = 10U;

#ifdef CONFIG_SVC_LVGL_USED
                            if (FontSize >= 5U) {
                                FontSize = 5U;
                            }
#endif

                            pDrawCtrl[VoutID].DrawFontSize[PtnIdx] = FontSize;
                        }
                    }
                }

                pDrawCtrl[VoutID].State |= SVC_VOUT_ERR_OSD_UNIT_ENABLE;
            }
        }

        if ( ( SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_LOG_DBG1 ) > 0U ) {
            SvcVoutErrTsk_OsdInfo();
        }

        /* Register OSD draw function */
        if (RetVal == SVC_OK) {
            for (VoutID = 0U; VoutID < AMBA_DSP_MAX_VOUT_NUM; VoutID ++) {
                if ((pCtrl->SelectBits & SvcVoutErrTsk_BitGet(VoutID)) > 0U) {
                    SvcGui_Register(VoutID, pOsdInfo[VoutID].DrawLevel, "VoutErr",
                                    pOsdInfo[VoutID].pDrawFunc, pOsdInfo[VoutID].pUpdateFunc);
                }
            }
        }

        /* create mutex */
        if (RetVal == SVC_OK) {
            AmbaUtility_StringAppend(pCtrl->Name, (UINT32)sizeof(pCtrl->Name), SvcVoutErrCtrl.Name);
            AmbaUtility_StringAppend(pCtrl->Name, (UINT32)sizeof(pCtrl->Name), "_");
            AmbaUtility_StringAppend(pCtrl->Name, (UINT32)sizeof(pCtrl->Name), "Osd");

            ErrCode = AmbaKAL_MutexCreate(&(pCtrl->Mutex), pCtrl->Name);
            if (ErrCode != 0U) {
                RetVal = SVC_NG;
                PRN_VOUT_ERR_LOG "Fail to create vout err osd ctrl - create mutex fail! ErrCode(0x%08X)"
                PRN_VOUT_ERR_ARG_UINT32 ErrCode PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_NG
            }
        }

        if (RetVal == SVC_OK) {
            SvcVoutErrCtrlFlg |= SVC_VOUT_ERR_FLG_OSD_INIT;
            if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_LOG_DBG1) > 0U) {
                PRN_VOUT_ERR_LOG "Success to create vout err osd module!" PRN_VOUT_ERR_OK
            }
        }
    }

    return RetVal;
}

static UINT32 SvcVoutErrTsk_OsdDelete(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_OSD_INIT) > 0U) {
        UINT32 Idx;

        for (Idx = 0U; Idx < AMBA_DSP_MAX_VOUT_NUM; Idx ++) {
            if ((SvcVoutErrOsdCtrl.SelectBits & SvcVoutErrTsk_BitGet(Idx)) > 0U) {
                SvcGui_Unregister(Idx, SvcVoutErrOsdCtrl.OsdInfo[Idx].DrawLevel);
            }
        }

        PRetVal = AmbaKAL_MutexDelete(&(SvcVoutErrOsdCtrl.Mutex)); PRN_VOUT_ERR_HDLR
        AmbaSvcWrap_MisraMemset(&SvcVoutErrOsdCtrl, 0, sizeof(SvcVoutErrOsdCtrl));


        SvcVoutErrCtrlFlg &= ~SVC_VOUT_ERR_FLG_OSD_INIT;
        if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_LOG_DBG1) > 0U) {
            PRN_VOUT_ERR_LOG "Success to delete vout err osd module!" PRN_VOUT_ERR_OK
        }
    }

    return RetVal;
}

static void SvcVoutErrTsk_OsdUpdateLock(UINT32 VoutID, UINT32 UpdateLock)
{
    UINT32 PRetVal;

    if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_OSD_INIT) == 0U) {
        PRN_VOUT_ERR_LOG "Fail to update osd lock - create vout err osd ctrl first!" PRN_VOUT_ERR_NG
    } else {
        if (VoutID >= AMBA_DSP_MAX_VOUT_NUM) {
            PRN_VOUT_ERR_LOG "Fail to update osd lock - invalid VoutID(%d)"
                PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_NG
        } else {
            PRetVal = SvcVoutErrTsk_MutexTake(&(SvcVoutErrOsdCtrl.Mutex), AMBA_KAL_WAIT_FOREVER);
            if (PRetVal != 0U) {
                PRN_VOUT_ERR_LOG "Fail to update osd - take mutex timeout!" PRN_VOUT_ERR_NG
            } else {

                if (UpdateLock > 0U) {
                    SvcVoutErrOsdCtrl.DrawCtrl[VoutID].State |= SVC_VOUT_ERR_OSD_UPD_LOCK;
                } else {
                    SvcVoutErrOsdCtrl.DrawCtrl[VoutID].State &= ~SVC_VOUT_ERR_OSD_UPD_LOCK;
                }

                SvcVoutErrTsk_MutexGive(&(SvcVoutErrOsdCtrl.Mutex));
            }
        }
    }
}

static void SvcVoutErrTsk_OsdUpdate(UINT32 VoutID)
{
    UINT32 RetVal = SVC_OK;

    if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_OSD_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to update osd - create vout err osd ctrl first!" PRN_VOUT_ERR_NG
    } else {
        if (VoutID >= AMBA_DSP_MAX_VOUT_NUM) {
            RetVal = SVC_NG;
            PRN_VOUT_ERR_LOG "Fail to update osd - invalid VoutID(%d)"
                PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_NG
        } else if (0U != SvcVoutErrTsk_MutexTake(&(SvcVoutErrOsdCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
            RetVal = SVC_NG;
            PRN_VOUT_ERR_LOG "Fail to update osd - take mutex timeout!" PRN_VOUT_ERR_NG
        } else {
            SVC_VOUT_ERR_OSD_DRAW_s *pDrawCtrl = &(SvcVoutErrOsdCtrl.DrawCtrl[VoutID]);

            if ((pDrawCtrl->State & SVC_VOUT_ERR_OSD_UNIT_ENABLE) > 0U) {
                if ((pDrawCtrl->State & SVC_VOUT_ERR_OSD_UPD_LOCK) == 0U) {

                    if ((SvcVoutErrCtrl.ErrState[VoutID].FreezeState & SVC_VOUT_ERR_VOUT_STATE_FREZE) > 0U) {
                        pDrawCtrl->OsdPtnSelectBits = SVC_VOUT_ERR_OSD_PTN_ID_VOUT_FRZ;
                        pDrawCtrl->State |= SVC_VOUT_ERR_OSD_UNIT_DISPLAY;
                        pDrawCtrl->DrawUpd = 1U;

                        PRN_VOUT_ERR_LOG "VoutID(%s%d%s) vout freeze. Draw OSD %s0x%08x%s"
                            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_1 PRN_VOUT_ERR_ARG_POST
                            PRN_VOUT_ERR_ARG_UINT32 VoutID                      PRN_VOUT_ERR_ARG_POST
                            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
                            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_0 PRN_VOUT_ERR_ARG_POST
                            PRN_VOUT_ERR_ARG_UINT32 pDrawCtrl->State            PRN_VOUT_ERR_ARG_POST
                            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_DBG2
                    } else {

                        // Clear osd error pattern
                        pDrawCtrl->OsdPtnSelectBits = 0U;

                        // Disable osd draw function
                        if ((pDrawCtrl->State & SVC_VOUT_ERR_OSD_UNIT_DISPLAY) > 0U) {
                            pDrawCtrl->State &= ~SVC_VOUT_ERR_OSD_UNIT_DISPLAY;
                            pDrawCtrl->DrawUpd = 1U;

                            PRN_VOUT_ERR_LOG "VoutID(%s%d%s) vout freeze. Clear OSD %s0x%08x%s"
                                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_1 PRN_VOUT_ERR_ARG_POST
                                PRN_VOUT_ERR_ARG_UINT32 VoutID                      PRN_VOUT_ERR_ARG_POST
                                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
                                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_0 PRN_VOUT_ERR_ARG_POST
                                PRN_VOUT_ERR_ARG_UINT32 pDrawCtrl->State            PRN_VOUT_ERR_ARG_POST
                                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
                            PRN_VOUT_ERR_DBG2
                        }
                    }
                }
            }

            SvcVoutErrTsk_MutexGive(&(SvcVoutErrOsdCtrl.Mutex));
        }
    }

    AmbaMisra_TouchUnused(&RetVal);
}

static void SvcVoutErrTsk_OsdInfo(void)
{
    const SVC_VOUT_ERR_OSD_CTRL_s *pCtrl = &SvcVoutErrOsdCtrl;
    UINT32 VoutIdx, PtnIdx;

    PRN_VOUT_ERR_LOG " " PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "---- %sOsd Info%s ----"
        PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_0 PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
    PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "  OSD Vout SelectBit"
        PRN_VOUT_ERR_ARG_UINT32 pCtrl->SelectBits PRN_VOUT_ERR_ARG_POST
    PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "  Name : %s"
        PRN_VOUT_ERR_ARG_CSTR   pCtrl->Name PRN_VOUT_ERR_ARG_POST
    PRN_VOUT_ERR_API

    PRN_VOUT_ERR_LOG "  GUI Info :" PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "    VoutID | Width | Height | Draw Lvl | Draw Win ( Start X, Start Y, Width, Height )" PRN_VOUT_ERR_API
    for (VoutIdx = 0U; VoutIdx < AMBA_DSP_MAX_VOUT_NUM; VoutIdx ++) {
        PRN_VOUT_ERR_LOG "        %02d |  %04d |   %04d |   %04d   |               %04d,    %04d,  %04d,   %04d"
            PRN_VOUT_ERR_ARG_UINT32 VoutIdx                                PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 pCtrl->OsdInfo[VoutIdx].Width          PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 pCtrl->OsdInfo[VoutIdx].Height         PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 pCtrl->OsdInfo[VoutIdx].DrawLevel      PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 pCtrl->OsdInfo[VoutIdx].DrawWin.StartX PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 pCtrl->OsdInfo[VoutIdx].DrawWin.StartY PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 pCtrl->OsdInfo[VoutIdx].DrawWin.Width  PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 pCtrl->OsdInfo[VoutIdx].DrawWin.Height PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_API

    }

    PRN_VOUT_ERR_LOG "  Pattern Info :" PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "    PtnID | Width | Height | Str Leng | String" PRN_VOUT_ERR_API
    for (PtnIdx = 0U; PtnIdx < SVC_VOUT_ERR_OSD_PTN_NUM; PtnIdx ++) {
        PRN_VOUT_ERR_LOG "       %02d |  %04d |   %04d |       %02d | %s"
            PRN_VOUT_ERR_ARG_UINT32 pCtrl->OsdPtnInfo[PtnIdx].PtnID     PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 pCtrl->OsdPtnInfo[PtnIdx].PtnWidth  PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 pCtrl->OsdPtnInfo[PtnIdx].PtnHeight PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 pCtrl->OsdPtnInfo[PtnIdx].StrPtnLen PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_CSTR   pCtrl->OsdPtnInfo[PtnIdx].StrPtn    PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_API
    }

    PRN_VOUT_ERR_LOG "  DrawCtrl :" PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "    VoutID |    State   | OsdPtnBit | DrawUpd | PtnID | StrCnt | FontSize | Win Start X, Start Y, Width, Height" PRN_VOUT_ERR_API
    for (VoutIdx = 0U; VoutIdx < AMBA_DSP_MAX_VOUT_NUM; VoutIdx ++) {
        if ((pCtrl->SelectBits & SvcVoutErrTsk_BitGet(VoutIdx)) > 0U) {
            for (PtnIdx = 0U; PtnIdx < SVC_VOUT_ERR_OSD_PTN_NUM; PtnIdx ++) {
                if (PtnIdx == 0U) {
                    PRN_VOUT_ERR_LOG "        %02d | 0x%08X |   0x%04X  |    %d    |    %02d |    %03d |    %02d    |        %04d,    %04d,  %04d,   %04d"
                        PRN_VOUT_ERR_ARG_UINT32 VoutIdx                                            PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 pCtrl->DrawCtrl[VoutIdx].State                     PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 pCtrl->DrawCtrl[VoutIdx].OsdPtnSelectBits          PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 pCtrl->DrawCtrl[VoutIdx].DrawUpd                   PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 PtnIdx                                             PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 pCtrl->DrawCtrl[VoutIdx].DrawStrCnt[PtnIdx]        PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 pCtrl->DrawCtrl[VoutIdx].DrawFontSize[PtnIdx]      PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 pCtrl->DrawCtrl[VoutIdx].DrawStrWin[PtnIdx].StartX PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 pCtrl->DrawCtrl[VoutIdx].DrawStrWin[PtnIdx].StartY PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 pCtrl->DrawCtrl[VoutIdx].DrawStrWin[PtnIdx].Width  PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 pCtrl->DrawCtrl[VoutIdx].DrawStrWin[PtnIdx].Height PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_API
                } else {
                    PRN_VOUT_ERR_LOG "                                                   %02d |    %03d |    %02d    |        %04d,    %04d,  %04d,   %04d"
                        PRN_VOUT_ERR_ARG_UINT32 PtnIdx                                             PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 pCtrl->DrawCtrl[VoutIdx].DrawStrCnt[PtnIdx]        PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 pCtrl->DrawCtrl[VoutIdx].DrawFontSize[PtnIdx]      PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 pCtrl->DrawCtrl[VoutIdx].DrawStrWin[PtnIdx].StartX PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 pCtrl->DrawCtrl[VoutIdx].DrawStrWin[PtnIdx].StartY PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 pCtrl->DrawCtrl[VoutIdx].DrawStrWin[PtnIdx].Width  PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 pCtrl->DrawCtrl[VoutIdx].DrawStrWin[PtnIdx].Height PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_API
                }
            }
        }
    }
}

static void SvcVoutErrTsk_OsdDrawFunc(UINT32 VoutID, UINT32 Level)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_OSD_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to draw osd - create vin err osd ctrl first!" PRN_VOUT_ERR_NG
    } else if (VoutID >= AMBA_DSP_MAX_VOUT_NUM) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to draw osd - invalid VoutID(%d)!"
            PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_NG
    } else if ((SvcVoutErrOsdCtrl.SelectBits & SvcVoutErrTsk_BitGet(VoutID)) == 0U) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to draw osd - invalid VoutID(%d)"
            PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_NG
    } else if (Level != SvcVoutErrOsdCtrl.OsdInfo[VoutID].DrawLevel) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to draw osd - invalid gui canvas level (%d) != (%d)"
            PRN_VOUT_ERR_ARG_UINT32 Level                                       PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrOsdCtrl.OsdInfo[VoutID].DrawLevel PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_NG
    } else if (0U != SvcVoutErrTsk_MutexTake(&(SvcVoutErrOsdCtrl.Mutex), AMBA_KAL_NO_WAIT)) {
        PRN_VOUT_ERR_LOG "Fail to draw osd - take mutex fail!" PRN_VOUT_ERR_DBG1
    } else {
        SVC_VOUT_ERR_OSD_DRAW_s *pDrawCtrl = &(SvcVoutErrOsdCtrl.DrawCtrl[VoutID]);
        UINT32 PtnIdx;
        char *pDrawStr = NULL;

        if ((pDrawCtrl->State & SVC_VOUT_ERR_OSD_UNIT_ENABLE) > 0U) {
            if ((pDrawCtrl->State & SVC_VOUT_ERR_OSD_UNIT_DISPLAY) > 0U) {

                for (PtnIdx = 0U; PtnIdx < SVC_VOUT_ERR_OSD_PTN_NUM; PtnIdx ++) {
                    if ((pDrawCtrl->DrawStrWin[PtnIdx].Width > 0U) &&
                        (pDrawCtrl->OsdPtnSelectBits == SvcVoutErrOsdCtrl.OsdPtnInfo[PtnIdx].PtnID)) {
                        pDrawStr = SvcVoutErrOsdCtrl.OsdPtnInfo[PtnIdx].StrPtn;

                        PRN_VOUT_ERR_LOG "Draw VoutID(%d) PtnIdx(%d) to %d,%d %dx%d FontSize: %d"
                            PRN_VOUT_ERR_ARG_UINT32 VoutID                               PRN_VOUT_ERR_ARG_POST
                            PRN_VOUT_ERR_ARG_UINT32 PtnIdx                               PRN_VOUT_ERR_ARG_POST
                            PRN_VOUT_ERR_ARG_UINT32 pDrawCtrl->DrawStrWin[PtnIdx].StartX PRN_VOUT_ERR_ARG_POST
                            PRN_VOUT_ERR_ARG_UINT32 pDrawCtrl->DrawStrWin[PtnIdx].StartY PRN_VOUT_ERR_ARG_POST
                            PRN_VOUT_ERR_ARG_UINT32 pDrawCtrl->DrawStrWin[PtnIdx].Width  PRN_VOUT_ERR_ARG_POST
                            PRN_VOUT_ERR_ARG_UINT32 pDrawCtrl->DrawStrWin[PtnIdx].Height PRN_VOUT_ERR_ARG_POST
                            PRN_VOUT_ERR_ARG_UINT32 pDrawCtrl->DrawFontSize[PtnIdx]      PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_DBG2

                        PRetVal = SvcOsd_DrawString(VoutID,
                                                    pDrawCtrl->DrawStrWin[PtnIdx].StartX,
                                                    pDrawCtrl->DrawStrWin[PtnIdx].StartY,
                                                    pDrawCtrl->DrawFontSize[PtnIdx],
                                                    SVC_VOUT_ERR_OSD_STR_COLOR,
                                                    pDrawStr); PRN_VOUT_ERR_HDLR
                    }
                }
            }
        }

        SvcVoutErrTsk_MutexGive(&(SvcVoutErrOsdCtrl.Mutex));

        AmbaMisra_TouchUnused(pDrawCtrl);
        AmbaMisra_TouchUnused(pDrawStr);
    }

    AmbaMisra_TouchUnused(&RetVal);
}

static void SvcVoutErrTsk_OsdUpdateFunc(UINT32 VoutID, UINT32 Level, UINT32 *pUpdate)
{
    AmbaMisra_TouchUnused(&Level);

    if (pUpdate != NULL) {
        if (SvcVoutErrOsdCtrl.DrawCtrl[VoutID].DrawUpd == 1U) {
            SvcVoutErrOsdCtrl.DrawCtrl[VoutID].DrawUpd = 0U;
            *pUpdate = 1U;
        } else {
            *pUpdate = 0U;
        }
    }
}

static void SvcVoutErrTsk_ShellStrToU32(const char *pString, UINT32 *pValue)
{
    UINT32 ErrCode = AmbaUtility_StringToUInt32(pString, pValue);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }
}

static UINT32 SvcVoutErrTsk_ShellDbgMsg(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount < 3U) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc 'debug' cmd - invalid arg count!" PRN_VOUT_ERR_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc 'debug' cmd - invalid arg vector!" PRN_VOUT_ERR_NG
    } else {
        UINT32 Enable = 0U, VoutID = 0xFFFFFFFFU;

        SvcVoutErrTsk_ShellStrToU32(pArgVector[2U], &VoutID);
        SvcVoutErrTsk_ShellStrToU32(pArgVector[3U], &Enable);

        if (Enable > 0U) {
            if (Enable == 2U) {
                SvcVoutErrCtrlFlg |= SVC_VOUT_ERR_FLG_LOG_NG2;
                SvcVoutErrCtrlFlg |= SVC_VOUT_ERR_FLG_LOG_DBG2;
            } else {
                SvcVoutErrCtrlFlg &= ~SVC_VOUT_ERR_FLG_LOG_NG2;
                SvcVoutErrCtrlFlg &= ~SVC_VOUT_ERR_FLG_LOG_DBG2;
            }
            SvcVoutErrCtrlFlg |= SVC_VOUT_ERR_FLG_LOG_DBG1;
        } else {
            SvcVoutErrCtrlFlg &= ~SVC_VOUT_ERR_FLG_LOG_DBG2;
            SvcVoutErrCtrlFlg &= ~SVC_VOUT_ERR_FLG_LOG_DBG1;
        }

        PRN_VOUT_ERR_LOG "Vout err control debug message - %s"
            PRN_VOUT_ERR_ARG_CSTR   ( (Enable > 0U) ? ( "enable" ) : ( "disable" ) ) PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_API
    }

    return RetVal;
}

static void SvcVoutErrTsk_ShellDbgMsgU(void)
{
    PRN_VOUT_ERR_LOG "  %sdebug%s    : enable/disable debug message"
        PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_1 PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
    PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "  ------------------------------------------------------" PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "    VoutID : configure vout id" PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "    Enable : 0, disable" PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "             1, enable vout err ctrl debug only" PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "             2, enable vout buffer debug msg" PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG " " PRN_VOUT_ERR_API

}

static UINT32 SvcVoutErrTsk_ShellVoutRecover(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount < 3U) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc 'recover' cmd - invalid arg count!" PRN_VOUT_ERR_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc 'recover' cmd - invalid arg vector!" PRN_VOUT_ERR_NG
    } else if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc 'recover' cmd - initi module first!" PRN_VOUT_ERR_NG
    } else {
        UINT32 VoutID = 0xFFFFFFFFU;

        SvcVoutErrTsk_ShellStrToU32(pArgVector[2U], &VoutID);

        if (VoutID >= AMBA_DSP_MAX_VOUT_NUM) {
            PRN_VOUT_ERR_LOG "Fail to proc 'recover' cmd - invalid VoutID(%d)!"
                PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_NG
        } else {
            SVC_VOUT_ERR_RECOVER_CMD_t RecoverCmd;

            AmbaSvcWrap_MisraMemset(&RecoverCmd, 0, sizeof(RecoverCmd));
            RecoverCmd.VoutID = VoutID;
            RecoverCmd.SerDesSelectBits = SvcVoutErrCtrl.SerDesSelectBits[VoutID];
            RecoverCmd.CmdID = SVC_VOUT_ERR_RCV_CMD_VOUT;

            /* Update vout state flag */
            PRetVal = AmbaKAL_EventFlagSet(&(SvcVoutErrCtrl.ErrState[VoutID].VoutStateFlag), SVC_VOUT_ERR_VOUT_STATE_RECOVER); PRN_VOUT_ERR_HDLR

            /* Send cmd */
            PRetVal = AmbaKAL_MsgQueueSend(&(SvcVoutErrCtrl.RecoverTask.Que), &RecoverCmd, 5U);
            if (PRetVal != 0U) {
                PRN_VOUT_ERR_LOG "Fail to prepare VoutID(%d) recover command"
                    PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_NG
            }
        }

    }

    return RetVal;
}

static void SvcVoutErrTsk_ShellVoutRecoverU(void)
{
    PRN_VOUT_ERR_LOG "  %srecover%s  : trigger vout recover flow"
        PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_1 PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
    PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "  ------------------------------------------------------" PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "    VoutID : configure vout id" PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG " " PRN_VOUT_ERR_API
}

static UINT32 SvcVoutErrTsk_ShellInfo(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount < 3U) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc 'info' cmd - invalid arg count!" PRN_VOUT_ERR_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc 'info' cmd - invalid arg vector!" PRN_VOUT_ERR_NG
    } else if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc 'info' cmd - initi module first!" PRN_VOUT_ERR_NG
    } else {
        UINT32 InfoFlag = 0xFFFFFFFFU;

        if (ArgCount > 3U) {
            SvcVoutErrTsk_ShellStrToU32(pArgVector[2U], &InfoFlag);
        }

        if ((InfoFlag & (0x1U)) > 0U) {
            SvcVoutErrTsk_Info();
        }

        if ((InfoFlag & (0x2U)) > 0U) {
            SvcVoutErrTsk_RcvCtrlInfo();
        }

        if ((InfoFlag & (0x4U)) > 0U) {
            SvcVoutErrTsk_DevInfoPrn();
        }

        if ((InfoFlag & (0x8U)) > 0U) {
            SvcVoutErrTsk_OsdInfo();
        }
    }

    return RetVal;
}

static void SvcVoutErrTsk_ShellInfoU(void)
{
    PRN_VOUT_ERR_LOG "  %sinfo%s     : print vout err setting"
        PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_1 PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
    PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "  ------------------------------------------------------" PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "   InfoFlg : print module flag" PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "             default is print all module" PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "             bit 0 | b'00000001 | print vout err control" PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "             bit 1 | b'00000010 | print recover control" PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "             bit 2 | b'00000100 | print device info" PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "             bit 3 | b'00001000 | print osd info" PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG " " PRN_VOUT_ERR_API
}

static UINT32 SvcVoutErrTsk_ShellVoutFrz(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_NG;

    if (ArgCount < 2U) {
        PRN_VOUT_ERR_LOG "Fail to proc 'vout_frz' cmd - invalid arg count!" PRN_VOUT_ERR_NG
    } else {
        UINT32 ShellIdx;
        UINT32 ShellCount = (UINT32)(sizeof(SvcVoutErrShellVoutFrz)) / (UINT32)(sizeof(SvcVoutErrShellVoutFrz[0]));

        for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
            if ((SvcVoutErrShellVoutFrz[ShellIdx].pFunc != NULL) && (SvcVoutErrShellVoutFrz[ShellIdx].Enable > 0U)) {
                if (0 == SvcWrap_strcmp(pArgVector[2U], SvcVoutErrShellVoutFrz[ShellIdx].ShellCmdName)) {
                    if (SVC_OK != (SvcVoutErrShellVoutFrz[ShellIdx].pFunc)(ArgCount, pArgVector)) {
                        if (SvcVoutErrShellVoutFrz[ShellIdx].pUsage != NULL) {
                            (SvcVoutErrShellVoutFrz[ShellIdx].pUsage)();
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

static void SvcVoutErrTsk_ShellVoutFrzU(void)
{
    UINT32 ShellIdx;
    UINT32 ShellCount = (UINT32)(sizeof(SvcVoutErrShellVoutFrz)) / (UINT32)(sizeof(SvcVoutErrShellVoutFrz[0]));

    PRN_VOUT_ERR_LOG " " PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "  %svout_freeze%s : vout freeze command"
        PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_1 PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
    PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "  ------------------------------------------------------" PRN_VOUT_ERR_API
    for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
        if ((SvcVoutErrShellVoutFrz[ShellIdx].pFunc != NULL) && (SvcVoutErrShellVoutFrz[ShellIdx].Enable > 0U)) {
            if (SvcVoutErrShellVoutFrz[ShellIdx].pUsage == NULL) {
                PRN_VOUT_ERR_LOG "    %s"
                    PRN_VOUT_ERR_ARG_CSTR   SvcVoutErrShellVoutFrz[ShellIdx].ShellCmdName PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_API
            } else {
                (SvcVoutErrShellVoutFrz[ShellIdx].pUsage)();
            }
        }
    }

    PRN_VOUT_ERR_LOG " " PRN_VOUT_ERR_API
}


static UINT32 SvcVoutErrTsk_ShellVoutFrzEna(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount < 4U) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc 'vout_frz'->'enable' cmd - invalid arg count!" PRN_VOUT_ERR_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc 'vout_frz'->'enable' cmd - input arg should not null!" PRN_VOUT_ERR_NG
    } else {
        if (0 == SvcWrap_strcmp(pArgVector[3U], "on")) {
            SvcVoutErrCtrlFlg &= ~SVC_VOUT_ERR_FLG_VOUT_FRZ_OFF;
        } else if (0 == SvcWrap_strcmp(pArgVector[3U], "off")) {
            SvcVoutErrCtrlFlg |= SVC_VOUT_ERR_FLG_VOUT_FRZ_OFF;
        } else {
            // misra-c
        }

        PRN_VOUT_ERR_LOG "%s vout err raw freeze mechanism!"
            PRN_VOUT_ERR_ARG_CSTR   (((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_VOUT_FRZ_OFF) == 0U)?"Enable":"Disable") PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_API
    }

    return RetVal;
}

static void SvcVoutErrTsk_ShellVoutFrzEnaU(void)
{
    PRN_VOUT_ERR_LOG "    %senable%s   : enable/disable vout freeze detection"
        PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_3 PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
    PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "  ------------------------------------------------------" PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "      on/off : enable/disable vout freeze mechanism" PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG " " PRN_VOUT_ERR_API
}

static UINT32 SvcVoutErrTsk_ShellVoutFrzRst(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount < 4U) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc 'vout_frz'->'reset' cmd - invalid arg count!" PRN_VOUT_ERR_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc 'vout_frz'->'reset' cmd - input arg should not null!" PRN_VOUT_ERR_NG
    } else {
        UINT32 VoutID = 0xFFFFU;

        SvcVoutErrTsk_ShellStrToU32(pArgVector[3U], &VoutID);
        SvcVoutErrTask_FreezeReset(VoutID);
    }

    return RetVal;
}

static void SvcVoutErrTsk_ShellVoutFrzRstU(void)
{
    PRN_VOUT_ERR_LOG "    %sreset%s    : reset vout freeze error state by vout"
        PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_3 PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
    PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "  ------------------------------------------------------" PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "      VoutID : requested vout_id" PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG " " PRN_VOUT_ERR_API
}

static UINT32 SvcVoutErrTsk_ShellVoutFrzMD(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount < 4U) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc 'vout_frz'->'msg_dump' cmd - invalid arg count!" PRN_VOUT_ERR_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc 'vout_frz'->'msg_dump' cmd - input arg should not null!" PRN_VOUT_ERR_NG
    } else {
        UINT32 VoutID = 0xFFFFU;

        SvcVoutErrTsk_ShellStrToU32(pArgVector[3U], &VoutID);
        SvcVoutErrTsk_VoutFrzMsgDump(VoutID);
    }

    return RetVal;
}

static void SvcVoutErrTsk_ShellVoutFrzMDU(void)
{
    PRN_VOUT_ERR_LOG "    %smsg_dump%s : dump vout freeze msg by VoutID"
        PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_3 PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
    PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "  ------------------------------------------------------" PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "      VoutID : requested vout_id" PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG " " PRN_VOUT_ERR_API
}

static UINT32 SvcVoutErrTsk_ShellMCtrl(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount < 4U) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc 'mctrl' cmd - invalid arg count!" PRN_VOUT_ERR_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc 'mctrl' cmd - invalid arg vector!" PRN_VOUT_ERR_NG
    } else if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc 'mctrl' cmd - initi module first!" PRN_VOUT_ERR_NG
    } else {
        UINT32 VoutID = 0xFFFFFFFFU;

        SvcVoutErrTsk_ShellStrToU32(pArgVector[2U], &VoutID);

        if (VoutID >= AMBA_DSP_MAX_VOUT_NUM) {
            PRN_VOUT_ERR_LOG "Fail to proc 'mctrl' cmd - invalid VoutID(%d)!"
                PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_NG
        } else {
            UINT32 Flag = 0U;
            if (0 == SvcWrap_strcmp(pArgVector[3U], "on")) {
                Flag = (0xFFFFUL << (16UL * VoutID));
                PRetVal = SvcVoutErrTsk_RcvFlagClear(VoutID, Flag); PRN_VOUT_ERR_HDLR
            } else if (0 == SvcWrap_strcmp(pArgVector[3U], "off")) {
                Flag = (0xFFFFUL << (16UL * VoutID));
                PRetVal = SvcVoutErrTsk_RcvFlagSet(VoutID, Flag); PRN_VOUT_ERR_HDLR
            } else if (0 == SvcWrap_strcmp(pArgVector[3U], "qry")) {
                if (SVC_OK == SvcVoutErrTsk_RcvFlagQry(VoutID, &Flag)) {
                    UINT32 ProcIdx;

                    PRN_VOUT_ERR_LOG " " PRN_VOUT_ERR_API
                    PRN_VOUT_ERR_LOG "--- VoutID(%d) Recover Control Flag (0x%08x) ---"
                        PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_UINT32 Flag   PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_API

                    PRN_VOUT_ERR_LOG "  %s Rcv Seq %s %s Proc Name      %s %s Proc ID %s %s Proc Bit   %s"
                        PRN_VOUT_ERR_ARG_CSTR   ("\033""[38;2;0;0;0m""\033""[48;2;255;201;14m") PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END                         PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_CSTR   ("\033""[38;2;0;0;0m""\033""[48;2;255;201;14m") PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END                         PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_CSTR   ("\033""[38;2;0;0;0m""\033""[48;2;255;201;14m") PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END                         PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_CSTR   ("\033""[38;2;0;0;0m""\033""[48;2;255;201;14m") PRN_VOUT_ERR_ARG_POST
                        PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END                         PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_API

                    for (ProcIdx = 0U; ProcIdx < SVC_VOUT_ERR_RCV_PROC_NUM; ProcIdx ++) {
                        if (SvcVoutErrRcvSubProc[ProcIdx].Enable > 0U) {

                            if ((Flag & SvcVoutErrRcvSubProc[ProcIdx].ProcBit) == 0U) {
                                PRN_VOUT_ERR_LOG "      %02d     %s      %02d      0x%08x"
                                    PRN_VOUT_ERR_ARG_UINT32 ProcIdx                                PRN_VOUT_ERR_ARG_POST
                                    PRN_VOUT_ERR_ARG_CSTR   SvcVoutErrRcvSubProc[ProcIdx].ProcName PRN_VOUT_ERR_ARG_POST
                                    PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrRcvSubProc[ProcIdx].ProcID   PRN_VOUT_ERR_ARG_POST
                                    PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrRcvSubProc[ProcIdx].ProcBit  PRN_VOUT_ERR_ARG_POST
                                PRN_VOUT_ERR_API
                            } else {

                                PRN_VOUT_ERR_LOG "      %02d     %s%s%s      %02d      0x%08x"
                                    PRN_VOUT_ERR_ARG_UINT32 ProcIdx                                          PRN_VOUT_ERR_ARG_POST
                                    PRN_VOUT_ERR_ARG_CSTR   ("\033""[38;2;255;174;201m""\033""[48;2;0;0;0m") PRN_VOUT_ERR_ARG_POST
                                    PRN_VOUT_ERR_ARG_CSTR   SvcVoutErrRcvSubProc[ProcIdx].ProcName           PRN_VOUT_ERR_ARG_POST
                                    PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END                          PRN_VOUT_ERR_ARG_POST
                                    PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrRcvSubProc[ProcIdx].ProcID             PRN_VOUT_ERR_ARG_POST
                                    PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrRcvSubProc[ProcIdx].ProcBit            PRN_VOUT_ERR_ARG_POST
                                PRN_VOUT_ERR_API
                            }

                        }
                    }
                }
            } else {
                SvcVoutErrTsk_ShellStrToU32(pArgVector[3U], &Flag);
                Flag &= 0xFFFFU;
                Flag = Flag << (16UL * VoutID);
                PRetVal = SvcVoutErrTsk_RcvFlagSet(VoutID, Flag); PRN_VOUT_ERR_HDLR
            }
        }
    }

    return RetVal;
}

static void SvcVoutErrTsk_ShellMCtrlU(void)
{
    UINT32 ProcIdx, PrnHdr = 0U;

    PRN_VOUT_ERR_LOG "  %smctrl%s    : manual control"
        PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_1 PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
    PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "  ------------------------------------------------------" PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "    VoutID : vout id" PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "      Ctrl : on   -> enable manual control" PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "             off  -> disable manual control" PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "             qry  -> query recover control flag" PRN_VOUT_ERR_API
    for (ProcIdx = 0U; ProcIdx < SVC_VOUT_ERR_RCV_PROC_NUM; ProcIdx ++) {
        if (SvcVoutErrRcvSubProc[ProcIdx].Enable > 0U) {
            if (PrnHdr == 0U) {
                PRN_VOUT_ERR_LOG "             flag -> 0x%08x, ProcID( %02d ) - %s"
                    PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrRcvSubProc[ProcIdx].ProcBit  PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrRcvSubProc[ProcIdx].ProcID   PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_CSTR   SvcVoutErrRcvSubProc[ProcIdx].ProcName PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_API
            } else {
                PRN_VOUT_ERR_LOG "                     0x%08x, ProcID( %02d ) - %s"
                    PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrRcvSubProc[ProcIdx].ProcBit  PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_UINT32 SvcVoutErrRcvSubProc[ProcIdx].ProcID   PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_ARG_CSTR   SvcVoutErrRcvSubProc[ProcIdx].ProcName PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_API
            }

            PrnHdr = 1U;
        }
    }
    PRN_VOUT_ERR_LOG " " PRN_VOUT_ERR_API
}

static void SvcVoutErrTsk_ShellUsage(void)
{
    UINT32 ShellIdx;
    UINT32 ShellCount = (UINT32)(sizeof(SvcVoutErrShellFunc)) / (UINT32)(sizeof(SvcVoutErrShellFunc[0]));

    PRN_VOUT_ERR_LOG " " PRN_VOUT_ERR_API
    PRN_VOUT_ERR_LOG "====== %sVOUT Err Ctrl Command Usage%s ======"
        PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_0 PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
    PRN_VOUT_ERR_API
    for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
        if ((SvcVoutErrShellFunc[ShellIdx].pFunc != NULL) && (SvcVoutErrShellFunc[ShellIdx].Enable > 0U)) {
            if (SvcVoutErrShellFunc[ShellIdx].pUsage == NULL) {
                PRN_VOUT_ERR_LOG "  %s"
                    PRN_VOUT_ERR_ARG_CSTR   SvcVoutErrShellFunc[ShellIdx].ShellCmdName PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_API
            } else {
                (SvcVoutErrShellFunc[ShellIdx].pUsage)();
            }
        }
    }

    PRN_VOUT_ERR_LOG " " PRN_VOUT_ERR_API
}

static void SvcVoutErrTsk_ShellEntryInit(void)
{
    if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_SHELL_INIT) == 0U) {
        UINT32 Cnt = 0U;

        AmbaSvcWrap_MisraMemset(SvcVoutErrShellVoutFrz, 0, sizeof(SvcVoutErrShellVoutFrz));
        SvcVoutErrShellVoutFrz[Cnt] = (SVC_VOUT_ERR_SHELL_FUNC_s) { 1U, "enable",   SvcVoutErrTsk_ShellVoutFrzEna, SvcVoutErrTsk_ShellVoutFrzEnaU }; Cnt++;
        SvcVoutErrShellVoutFrz[Cnt] = (SVC_VOUT_ERR_SHELL_FUNC_s) { 1U, "reset",    SvcVoutErrTsk_ShellVoutFrzRst, SvcVoutErrTsk_ShellVoutFrzRstU }; Cnt++;
        SvcVoutErrShellVoutFrz[Cnt] = (SVC_VOUT_ERR_SHELL_FUNC_s) { 1U, "msg_dump", SvcVoutErrTsk_ShellVoutFrzMD,  SvcVoutErrTsk_ShellVoutFrzMDU  };

        Cnt = 0U;
        AmbaSvcWrap_MisraMemset(SvcVoutErrShellFunc, 0, sizeof(SvcVoutErrShellFunc));
        SvcVoutErrShellFunc[Cnt] = (SVC_VOUT_ERR_SHELL_FUNC_s) { 1U, "debug",       SvcVoutErrTsk_ShellDbgMsg,       SvcVoutErrTsk_ShellDbgMsgU      }; Cnt++;
        SvcVoutErrShellFunc[Cnt] = (SVC_VOUT_ERR_SHELL_FUNC_s) { 1U, "recover",     SvcVoutErrTsk_ShellVoutRecover,  SvcVoutErrTsk_ShellVoutRecoverU }; Cnt++;
        SvcVoutErrShellFunc[Cnt] = (SVC_VOUT_ERR_SHELL_FUNC_s) { 1U, "info",        SvcVoutErrTsk_ShellInfo,         SvcVoutErrTsk_ShellInfoU        }; Cnt++;
        SvcVoutErrShellFunc[Cnt] = (SVC_VOUT_ERR_SHELL_FUNC_s) { 1U, "vout_freeze", SvcVoutErrTsk_ShellVoutFrz,      SvcVoutErrTsk_ShellVoutFrzU     }; Cnt++;
        SvcVoutErrShellFunc[Cnt] = (SVC_VOUT_ERR_SHELL_FUNC_s) { 1U, "mctrl",       SvcVoutErrTsk_ShellMCtrl,        SvcVoutErrTsk_ShellMCtrlU       };

        SvcVoutErrCtrlFlg |= SVC_VOUT_ERR_FLG_SHELL_INIT;
    }
}

static void SvcVoutErrTsk_ShellEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal = SVC_OK;

    SvcVoutErrTsk_ShellEntryInit();

    if(ArgCount == 0xCafeU) {
        UINT32 SIdx;

        PRN_VOUT_ERR_LOG " " PRN_VOUT_ERR_API
        PRN_VOUT_ERR_LOG "==== %sShell Arguments Setting%s (%s%d%s) ===="
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_TITLE_1 PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_NUM     PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_UINT32 ArgCount                    PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END     PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_API

        for (SIdx = 0U; SIdx < ArgCount; SIdx ++) {
            PRN_VOUT_ERR_LOG "  pArgVector[%s%d%s] : %s%s%s"
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_NUM PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_UINT32 SIdx                    PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_STR PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   pArgVector[SIdx]        PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_ARG_CSTR   SVC_LOG_VOUT_ERR_HL_END PRN_VOUT_ERR_ARG_POST
            PRN_VOUT_ERR_API
        }
    }

    if (ArgCount < 1U) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc vout err ctrl shell cmd - invalid arg count!" PRN_VOUT_ERR_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to proc vout err ctrl shell cmd - invalid arg vector!" PRN_VOUT_ERR_NG
    } else {
        UINT32 ShellIdx;
        UINT32 ShellCount = (UINT32)(sizeof(SvcVoutErrShellFunc)) / (UINT32)(sizeof(SvcVoutErrShellFunc[0]));

        RetVal = SVC_NG;

        for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
            if ((SvcVoutErrShellFunc[ShellIdx].pFunc != NULL) && (SvcVoutErrShellFunc[ShellIdx].Enable > 0U)) {
                if (0 == SvcWrap_strcmp(pArgVector[1U], SvcVoutErrShellFunc[ShellIdx].ShellCmdName)) {
                    if (SVC_OK != (SvcVoutErrShellFunc[ShellIdx].pFunc)(ArgCount, pArgVector)) {
                        if (SvcVoutErrShellFunc[ShellIdx].pUsage != NULL) {
                            (SvcVoutErrShellFunc[ShellIdx].pUsage)();
                        }
                    }
                    RetVal = SVC_OK;
                    break;
                }
            }
        }
    }

    if (RetVal != SVC_OK) {
        SvcVoutErrTsk_ShellUsage();
        AmbaMisra_TouchUnused(&PrintFunc);
    }
}

static void SvcVoutErrTsk_CommandInstall(void)
{
    if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_CMD_INSTALL) == 0U) {
        AMBA_SHELL_COMMAND_s SvcVoutErrCmdApp;

        SvcVoutErrCmdApp.pName    = "svc_vouterr";
        SvcVoutErrCmdApp.MainFunc = SvcVoutErrTsk_ShellEntry;
        SvcVoutErrCmdApp.pNext    = NULL;

        if (SHELL_ERR_SUCCESS != SvcCmd_CommandRegister(&SvcVoutErrCmdApp)) {
            PRN_VOUT_ERR_LOG "Fail to install svc vout err control command!" PRN_VOUT_ERR_NG
        } else {
            SvcVoutErrCtrlFlg |= SVC_VOUT_ERR_FLG_CMD_INSTALL;
        }
    }
}

/**
 * Initial vout err control task
 *
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcVoutErrTask_Init(void)
{
    AmbaSvcWrap_MisraMemset(VoutErrMonVoutStateMsg, 0, sizeof(VoutErrMonVoutStateMsg));
    AmbaSvcWrap_MisraMemset(VoutErrMonDspListenMsg, 0, sizeof(VoutErrMonDspListenMsg));
    AmbaSvcWrap_MisraMemset(VoutErrMonMsgPortist, 0, sizeof(VoutErrMonMsgPortist));
    AmbaSvcWrap_MisraMemset(SvcVoutErrRcvSubProc, 0, sizeof(SvcVoutErrRcvSubProc));
    AmbaSvcWrap_MisraMemset(&SvcVoutErrCtrl, 0, sizeof(SvcVoutErrCtrl));

    SvcVoutErrTsk_CommandInstall();

    return SVC_OK;
}

/**
 * Start vout err control task
 *
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcVoutErrTask_Start(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ( ( SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_INIT ) > 0U ) {
        RetVal = 99U;
        PRN_VOUT_ERR_LOG "Svc vout err control has been created!" PRN_VOUT_ERR_API
    } else {
        UINT32 VinNum = 0U, VinIDs[AMBA_DSP_MAX_VIN_NUM], VinSrc = 255U;

        AmbaSvcWrap_MisraMemset(VinIDs, 0, sizeof(VinIDs));
        if (0U != SvcResCfg_GetVinIDs(VinIDs, &VinNum)) {
            RetVal = SVC_NG;
            PRN_VOUT_ERR_LOG "Fail to create vout err control - get vin number and id fail!" PRN_VOUT_ERR_NG
        } else {
            if (VinNum == 0U) {
                RetVal = SVC_NG;
                PRN_VOUT_ERR_LOG "Fail to create vout err control - vin number should not zero!" PRN_VOUT_ERR_NG
            } else {
                if (0U != SvcResCfg_GetVinSrc(VinIDs[0], &VinSrc)) {
                    RetVal = SVC_NG;
                    PRN_VOUT_ERR_LOG "Fail to create vout err control - get VinID(%d) source fail!"
                        PRN_VOUT_ERR_ARG_UINT32 VinIDs[0] PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_NG
                } else {
                    if (VinSrc == SVC_VIN_SRC_MEM) {
                        RetVal = 99U;
                        PRN_VOUT_ERR_LOG "Not support for video raw encode" PRN_VOUT_ERR_API
                    }
                }
            }
        }

        /* Reset the module setting */
        if (RetVal == SVC_OK) {
            AmbaSvcWrap_MisraMemset(&SvcVoutErrCtrl, 0, sizeof(SvcVoutErrCtrl));
            SvcWrap_strcpy(SvcVoutErrCtrl.Name, sizeof(SvcVoutErrCtrl.Name), "SvcVoutErr");
            RetVal = SvcVoutErrTsk_Config();
        }

        /* Create monitor msg task */
        if (RetVal == SVC_OK) {
            RetVal = SvcVoutErrTsk_MonFrwkCfg();
        }

        /* Create recover task */
        if (RetVal == SVC_OK) {
            RetVal = SvcVoutErrTsk_RcvCtrlCfg();
        }

        /* Create err ctrl task */
        if (RetVal == SVC_OK) {
            SVC_VOUT_ERR_CTRL_TASK_s *pCtrlTask = &(SvcVoutErrCtrl.ErrCtrlTask);

            AmbaSvcWrap_MisraMemset(pCtrlTask, 0, sizeof(SVC_VOUT_ERR_CTRL_TASK_s));

            /* %s_%s */
              AmbaUtility_StringCopy(pCtrlTask->Name, sizeof(pCtrlTask->Name), SvcVoutErrCtrl.Name);
            AmbaUtility_StringAppend(pCtrlTask->Name, (UINT32)sizeof(pCtrlTask->Name), "_");
            AmbaUtility_StringAppend(pCtrlTask->Name, (UINT32)sizeof(pCtrlTask->Name), "Ctrl");

            /* Create queue */
            PRetVal = AmbaKAL_MsgQueueCreate( &(pCtrlTask->Que),
                                              pCtrlTask->Name,
                                              (UINT32)sizeof(SVC_VOUT_ERR_CTRL_CMD_t),
                                              pCtrlTask->QueBuf,
                                              (UINT32)sizeof(pCtrlTask->QueBuf) );
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_VOUT_ERR_LOG "Fail to create vout err control - create err ctrl queue fail!" PRN_VOUT_ERR_NG
            }

            /* Create err control task */
            if (RetVal == SVC_OK) {
                pCtrlTask->Ctrl.CpuBits    = SVC_VOUT_ERR_TASK_CPU_BITS;
                pCtrlTask->Ctrl.Priority   = SVC_VOUT_ERR_CTRL_TASK_PRI;
                pCtrlTask->Ctrl.EntryFunc  = SvcVoutErrTsk_ErrCtrlTskEntry;
                pCtrlTask->Ctrl.StackSize  = SVC_VOUT_ERR_STACK_SZ;
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
                    PRN_VOUT_ERR_LOG "Fail to create vout err control - create err ctrl task fail! ErrCode 0x%x"
                        PRN_VOUT_ERR_ARG_UINT32 PRetVal PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_NG
                } else {
                    PRN_VOUT_ERR_LOG "Successful to create vout err ctrl task - %s"
                        PRN_VOUT_ERR_ARG_CSTR   pCtrlTask->Name PRN_VOUT_ERR_ARG_POST
                    PRN_VOUT_ERR_DBG1
                }
            }
        }

        /* Create timer */
        if (RetVal == SVC_OK) {

            AmbaSvcWrap_MisraMemset(&(SvcVoutErrCtrl.LinkChgCtrl), 0, sizeof(SvcVoutErrCtrl.LinkChgCtrl));

            /* %s_%s */
              AmbaUtility_StringCopy(SvcVoutErrCtrl.LinkChgCtrl.Name, 32U, SvcVoutErrCtrl.Name);
            AmbaUtility_StringAppend(SvcVoutErrCtrl.LinkChgCtrl.Name, 32U, "_");
            AmbaUtility_StringAppend(SvcVoutErrCtrl.LinkChgCtrl.Name, 32U, "Timer");

            SvcVoutErrCtrl.LinkChgCtrl.SelectBits = 0U;
            SvcVoutErrCtrl.LinkChgCtrl.TimerPeriod = SVC_VOUT_ERR_TIMER_PERIOD;
            PRetVal = AmbaKAL_TimerCreate(&(SvcVoutErrCtrl.LinkChgCtrl.Timer),
                                          SvcVoutErrCtrl.LinkChgCtrl.Name,
                                          SvcVoutErrTsk_TimerHandler,
                                          0,
                                          1,
                                          SvcVoutErrCtrl.LinkChgCtrl.TimerPeriod,
                                          AMBA_KAL_AUTO_START);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_VOUT_ERR_LOG "Fail to create vout err control - create timer fail! ErrCode 0x%x"
                    PRN_VOUT_ERR_ARG_UINT32 PRetVal PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_NG
            } else {
                PRN_VOUT_ERR_LOG "Successful to create vout err timer - %s"
                    PRN_VOUT_ERR_ARG_CSTR   SvcVoutErrCtrl.LinkChgCtrl.Name PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_DBG1
            }
        }

        /* Create mutex */
        if (RetVal == SVC_OK) {
            PRetVal = AmbaKAL_MutexCreate(&(SvcVoutErrCtrl.Mutex), SvcVoutErrCtrl.Name);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_VOUT_ERR_LOG "Fail to create vout err ctrl - create mutex fail!, RetCode: 0x%x"
                    PRN_VOUT_ERR_ARG_UINT32 PRetVal PRN_VOUT_ERR_ARG_POST
                PRN_VOUT_ERR_NG
            }
        }

        if (RetVal == SVC_OK) {
            SvcVoutErrCtrlFlg |= SVC_VOUT_ERR_FLG_INIT;

            if ((SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_LOG_DBG1) > 0U) {
                SvcVoutErrTsk_Info();
            }

            PRN_VOUT_ERR_LOG "Successful to create vout err control!" PRN_VOUT_ERR_OK
        }
    }

    if (RetVal == 99U) {
        RetVal = SVC_OK;
        AmbaMisra_TouchUnused(VoutErrMonVoutStateMsg);
    }

    return RetVal;
}

/**
 * Stop vout err control task
 *
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcVoutErrTask_Stop(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ( ( SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_INIT ) == 0U ) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to delete vout err control - create module first!" PRN_VOUT_ERR_NG
    } else {
        UINT32 Idx;

        AmbaMisra_TouchUnused(VoutErrMonMsgPortist);

        /* delete monitor task */
        for (Idx = 0U; Idx < SvcVoutErrCtrl.MonFrwkTask.NumOfTask; Idx ++) {
            PRetVal = SvcTask_Destroy(&(SvcVoutErrCtrl.MonFrwkTask.Ctrl[Idx])); PRN_VOUT_ERR_HDLR
        }

        /* close monitor port */
        for (Idx = 0U; Idx < SvcVoutErrCtrl.MonFrwkCtrl.NumOfMonitorPort; Idx ++) {
            PRetVal = AmbaMonMessage_Close(&(SvcVoutErrCtrl.MonFrwkCtrl.MonitorPort[Idx])); PRN_VOUT_ERR_HDLR
        }

        /* delete control task */
        PRetVal = SvcTask_Destroy( &(SvcVoutErrCtrl.ErrCtrlTask.Ctrl) ); PRN_VOUT_ERR_HDLR

        /* delete control queue */
        PRetVal = AmbaKAL_MsgQueueDelete( &(SvcVoutErrCtrl.ErrCtrlTask.Que) ); PRN_VOUT_ERR_HDLR

        /* delete recover task */
        PRetVal = SvcTask_Destroy( &(SvcVoutErrCtrl.RecoverTask.Ctrl) ); PRN_VOUT_ERR_HDLR

        /* delete recover queue */
        PRetVal = AmbaKAL_MsgQueueDelete( &(SvcVoutErrCtrl.RecoverTask.Que) ); PRN_VOUT_ERR_HDLR

        /* delete timer */
        PRetVal = AmbaKAL_TimerDelete(&(SvcVoutErrCtrl.LinkChgCtrl.Timer)); PRN_VOUT_ERR_HDLR

        /* delete error state flag */
        for (Idx = 0U; Idx < AMBA_DSP_MAX_VOUT_NUM; Idx ++) {
            if ((SvcVoutErrCtrl.VoutSelectBits & SvcVoutErrTsk_BitGet(Idx)) > 0U) {
                PRetVal = AmbaKAL_EventFlagClear(&(SvcVoutErrCtrl.ErrState[Idx].VoutStateFlag), 0xFFFFFFFFU); PRN_VOUT_ERR_HDLR
                PRetVal = AmbaKAL_EventFlagDelete(&(SvcVoutErrCtrl.ErrState[Idx].VoutStateFlag)); PRN_VOUT_ERR_HDLR
            }
        }

        /* delete recover control flag */
        PRetVal = AmbaKAL_EventFlagClear(&(SvcVoutErrCtrl.RecoverTask.RcvCtrl.ActFlg), 0xFFFFFFFFU); PRN_VOUT_ERR_HDLR
        PRetVal = AmbaKAL_EventFlagDelete(&(SvcVoutErrCtrl.RecoverTask.RcvCtrl.ActFlg)); PRN_VOUT_ERR_HDLR

        /* delete mutex */
        PRetVal = AmbaKAL_MutexDelete(&(SvcVoutErrCtrl.Mutex)); PRN_VOUT_ERR_HDLR
        for (Idx = 0U; Idx < AMBA_DSP_MAX_VOUT_NUM; Idx ++) {
            PRetVal = AmbaKAL_MutexDelete(&(SvcVoutErrCtrl.VoutFrzCtrl[Idx].FrzMsg.Mutex)); PRN_VOUT_ERR_HDLR
        }

        /* delete osd ctrl */
        PRetVal = SvcVoutErrTsk_OsdDelete(); PRN_VOUT_ERR_HDLR

        /* reset module setting */
        AmbaSvcWrap_MisraMemset(&SvcVoutErrCtrl, 0, sizeof(SvcVoutErrCtrl));

        SvcVoutErrCtrlFlg &= ~SVC_VOUT_ERR_FLG_INIT;
        PRN_VOUT_ERR_LOG "Successful to delete vout err control!" PRN_VOUT_ERR_OK
    }

    return RetVal;
}

UINT32 SvcVoutErrTask_RcvFpd(UINT32 VoutID)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ( ( SvcVoutErrCtrlFlg & SVC_VOUT_ERR_FLG_INIT ) == 0U ) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to recover vout FPD driver - create module first!" PRN_VOUT_ERR_NG
    } else if (VoutID >= AMBA_DSP_MAX_VOUT_NUM) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to recover vout FPD driver - invalid VoutID(%d)!"
            PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_NG
    } else if ((SvcVoutErrCtrl.VoutSelectBits & SvcVoutErrTsk_BitGet(VoutID)) == 0U) {
        RetVal = SVC_NG;
        PRN_VOUT_ERR_LOG "Fail to recover vout FPD driver - not support VoutID(%d)!"
            PRN_VOUT_ERR_ARG_UINT32 VoutID PRN_VOUT_ERR_ARG_POST
        PRN_VOUT_ERR_NG
    } else {
        SVC_VOUT_ERR_RECOVER_CMD_t RcvCmd;

        AmbaSvcWrap_MisraMemset(&RcvCmd, 0, sizeof(RcvCmd));
        RcvCmd.CmdID = SVC_VOUT_ERR_RCV_CMD_FPD;
        RcvCmd.VoutID = VoutID;
        RcvCmd.SerDesSelectBits = SvcVoutErrCtrl.SerDesSelectBits[VoutID];

        PRetVal = SvcVoutErrTsk_RcvFlagSet(VoutID, SvcVoutErrCtrl.RecoverTask.RcvCtrl.ProcSelectBits[SVC_VOUT_ERR_RCV_CMD_FPD]); PRN_VOUT_ERR_HDLR

        RetVal = SvcVoutErrTsk_RcvProcFpd(&RcvCmd);

        PRetVal = SvcVoutErrTsk_RcvFlagClear(VoutID, 0xFFFFU); PRN_VOUT_ERR_HDLR
    }

    return RetVal;
}


