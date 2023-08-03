/**
 *  @file SvcRawEncTask.c
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
 *  @details svc raw encode task
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaSvcWrap.h"
#include "AmbaUtility.h"
#include "AmbaSensor.h"
#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaShell.h"
#include "AmbaFS.h"
#include "AmbaImg_External_CtrlFunc.h"
#include "AmbaImg_Proc.h"
#include "AmbaTUNE_HdlrManager.h"

#include "AmbaImg_Adjustment.h"
#include "AmbaIQParamHandlerSample.h"

#include "AmbaVfs.h"

/* svc framework */
#include "SvcErrCode.h"
#include "SvcWrap.h"
#include "SvcLog.h"
#include "SvcCmd.h"
#include "SvcTask.h"
#include "SvcMem.h"
#include "SvcPref.h"
#include "SvcRawCap.h"

/* svc shared */
#include "SvcResCfg.h"
#include "SvcVinSrc.h"
#include "SvcRecMain.h"
#include "SvcImg.h"
#include "SvcBuffer.h"
#include "SvcRawEnc.h"

/* svc app */
#include "SvcUserPref.h"
#include "SvcRecTask.h"
#include "SvcTaskList.h"
#include "SvcBufMap.h"
#include "SvcRecTask.h"
#include "SvcRawCapTask.h"
#include "SvcRawEncTask.h"

#define SVC_RAW_ENC_FLG_INIT                (0x1U)
#define SVC_RAW_ENC_FLG_TASK_INIT           (0x10U)
#define SVC_RAW_ENC_FLG_STRM_CAP_INIT       (0x100U)
#define SVC_RAW_ENC_FLG_STRM_CAP_SEPERATE   (0x200U)
#define SVC_RAW_ENC_FLG_STRM_CAP_POST_TEXT  (0x400U)
#define SVC_RAW_ENC_FLG_STRM_CAP_RAW_ENC    (0x800U)
#define SVC_RAW_ENC_FLG_STRM_CAP_USER_CAP   (0x1000U)
#define SVC_RAW_ENC_FLG_SHELL_INIT          (0x100000U)
#define SVC_RAW_ENC_FLG_MSG_OK              (0x1000000U)
#define SVC_RAW_ENC_FLG_MSG_NG              (0x2000000U)
#define SVC_RAW_ENC_FLG_MSG_API             (0x4000000U)
#define SVC_RAW_ENC_FLG_MSG_DBG             (0x8000000U)
#define SVC_RAW_ENC_FLG_MSG_DBG1            (0x10000000U)
#define SVC_RAW_ENC_FLG_MSG_DEF             (SVC_RAW_ENC_FLG_MSG_OK | SVC_RAW_ENC_FLG_MSG_NG | SVC_RAW_ENC_FLG_MSG_API)
#define SVC_RAW_ENC_FLG_RAW_ENC_DEF         (SVC_RAW_ENC_FLG_STRM_CAP_RAW_ENC | SVC_RAW_ENC_FLG_STRM_CAP_POST_TEXT)

#define SVC_RAW_ENC_TSK_STACK_SIZE      (0x40000U)
#define RAW_ENC_STRM_CAP_BUF_NUM        (10U * AMBA_DSP_MAX_VIN_NUM * AMBA_DSP_MAX_VIEWZONE_NUM)
#define RAW_ENC_STRM_CAP_MSG_NUM        (10U * AMBA_DSP_MAX_VIN_NUM)
#define RAW_ENC_STRM_CAP_SCAN_NUM       (5U)
#define RAW_ENC_STRM_CAP_PREFIX_PATH    "c:\\"
#define RAW_ENC_STRM_CAP_TEMP_PATH      "_aaa_tmp_seq_file"
#define RAW_ENC_STRM_CAP_TEMP_STR_SIZE  (128U)
#define RAW_ENC_STRM_CAP_MAX_REC_MINS   (720U)      // 12hrs x 60mins

#define RAW_ENC_STRM_CAP_UPD_AAA        (0U)
#define RAW_ENC_STRM_CAP_UPD_FOV_PIPE   (1U)
#define RAW_ENC_STRM_CAP_UPD_NUM        (32U)

#define RAW_ENC_ASCII_DOT               (46)

#define RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE    (32U)

typedef struct {
    UINT32                  UpdBits;
    SVC_RAW_CAP_AAA_DATA_s  AaaData;
    SVC_IMG_ALGO_FOV_PIPE_s FovPipeData;
} RAW_ENC_STRM_3A_DATA_s;

typedef struct {
    UINT32                  RecID;
    UINT32                  VinID;
    UINT32                  FovID;
    UINT32                  FrameNo;
    UINT64                  TimeStamp;
    UINT64                  RawSeq[AMBA_DSP_MAX_VIN_NUM];
} RAW_ENC_STRM_CAP_INFO_s;

typedef struct {
#define RAW_ENC_STRM_3A_DATA_DATA   (0x1U)
#define RAW_ENC_STRM_3A_DATA_SPLIT  (0x2U)
#define RAW_ENC_STRM_3A_DATA_EOS    (0x4U)
    UINT32                  State;
    RAW_ENC_STRM_CAP_INFO_s *pInfo;
    RAW_ENC_STRM_3A_DATA_s  *pData;
} RAW_ENC_STRM_CAP_QUE_UNIT_s;

typedef struct {
    UINT32 VinID;
    UINT64 CapPts;
    UINT64 CapSequence;
} RAW_ENC_STRM_CAP_RAW_INFO_s;

typedef struct {
    UINT64 CaptureTimeStamp;
    UINT32 FrmNo;
    UINT32 RecID;
    UINT32 FrmType;
} RAW_ENC_STRM_CAP_VIDEO_INFO_s;

typedef struct {
    UINT32 Type;
#define SVC_RAW_ENC_STRM_CAP_MSG_RAW    (1U)
#define SVC_RAW_ENC_STRM_CAP_MSG_VIDEO  (2U)
    UINT32 MsgData[6U];
} RAW_ENC_STRM_CAP_MSG_UNIT_s;

typedef struct {
    AMBA_KAL_MUTEX_t            Mutex;
    UINT32                      WriteIdx;
    RAW_ENC_STRM_CAP_RAW_INFO_s Array[RAW_ENC_STRM_CAP_SCAN_NUM];
} RAW_ENC_STRM_CAP_RAW_SEQ_s;

typedef struct {
#define RAW_ENC_STRM_CAP_FILE_OPEN  (0x1U)
    UINT32          State;
    AMBA_VFS_FILE_s VfsFile;
    UINT64          PreRawSeq;
    char            FilePath[RAW_ENC_STRM_CAP_TEMP_STR_SIZE];
} RAW_ENC_STRM_CAP_FILE_s;

typedef struct {
    char            FilePath[RAW_ENC_STRM_CAP_MAX_REC_MINS][RAW_ENC_STRM_CAP_TEMP_STR_SIZE];
} RAW_ENC_STRM_CAP_TXT_s;

typedef struct {
    UINT32                  CurTxtNum;
    UINT32                  MaxTxtBuf;
    RAW_ENC_STRM_CAP_TXT_s *pTxtBuf;
} RAW_ENC_STRM_CAP_TXT_CTRL_s;

#pragma pack(1)
typedef struct {
    UINT32                  VinID;
    UINT32                  FovID;
    UINT32                  UnitSize;
} __attribute__((packed)) RAW_ENC_STRM_CAP_SEQ_HEADER_s;

typedef struct {
    UINT32  Offset;
    UINT32  Size;
} RAW_ENC_STRM_CAP_SEQ_UPD_INFO_s;

#pragma pack(1)
typedef struct {
    UINT32                          FrameNo;
    UINT32                          UpdBits;
    RAW_ENC_STRM_CAP_SEQ_UPD_INFO_s UpdInfo[RAW_ENC_STRM_CAP_UPD_NUM];
    SVC_RAW_CAP_AAA_DATA_s          AaaData;
    SVC_IMG_ALGO_FOV_PIPE_s         FovPipeData;
} __attribute__((packed)) RAW_ENC_STRM_CAP_SEQ_UNIT_s;

typedef struct {
    UINT32                      BufType;
    UINT8                      *pBuf;
    UINT32                      BufSize;

    UINT32                      RecSelectBits;
    UINT32                      RecToVinBits[AMBA_DSP_MAX_STREAM_NUM];
    UINT32                      RecToFovBits[AMBA_DSP_MAX_STREAM_NUM];
    UINT32                      FovToRecID[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT32                      RecFrameCnt[AMBA_DSP_MAX_STREAM_NUM];
    UINT32                      RecSplitCnt[AMBA_DSP_MAX_STREAM_NUM];
    RAW_ENC_STRM_CAP_FILE_s     AaaFile[AMBA_DSP_MAX_VIEWZONE_NUM];

    SVC_TASK_CTRL_s             MsgTaskCtrl;
    UINT8                       MsgTaskStack[SVC_RAW_ENC_TSK_STACK_SIZE];
    SVC_TASK_CTRL_s             FifoTaskCtrl;
    UINT8                       FifoTaskStack[SVC_RAW_ENC_TSK_STACK_SIZE];

    RAW_ENC_STRM_CAP_QUE_UNIT_s FreeQueBuf[RAW_ENC_STRM_CAP_BUF_NUM];
    AMBA_KAL_MSG_QUEUE_t        FreeQue;
    RAW_ENC_STRM_CAP_QUE_UNIT_s DataQueBuf[RAW_ENC_STRM_CAP_BUF_NUM];
    AMBA_KAL_MSG_QUEUE_t        DataQue;
    RAW_ENC_STRM_CAP_MSG_UNIT_s MsgQueBuf[RAW_ENC_STRM_CAP_MSG_NUM];
    AMBA_KAL_MSG_QUEUE_t        MsgQue;
    RAW_ENC_STRM_CAP_RAW_SEQ_s  RawSeqScanner[AMBA_DSP_MAX_VIN_NUM];
    RAW_ENC_STRM_CAP_TXT_CTRL_s CvtTxtCtrl;
    UINT32                      FileCnt;
} RAW_ENC_STRM_CAP_CTRL_s;

typedef struct {
    SVC_TASK_CTRL_s             TaskCtrl;
    UINT8                       TaskStack[SVC_RAW_ENC_TSK_STACK_SIZE];
    UINT32                      FrameInterval;
    UINT32                      FeedDataTimeStamp;
} RAW_ENC_TSK_CTRL_s;

typedef struct {
    AMBA_KAL_EVENT_FLAG_t     Flag;
#define SVC_RAW_ENC_CTRL_FLG_REC_START      (0x1U)
#define SVC_RAW_ENC_CTRL_FLG_REC_STOP       (0x2U)
#define SVC_RAW_ENC_CTRL_FLG_DAT_LOAD_DONE  (0x4U)

#define SVC_RAW_ENC_CTRL_FLG_MSG_UPD        (0x10U)
#define SVC_RAW_ENC_CTRL_FLG_MSG_IDLE       (0x20U)
#define SVC_RAW_ENC_CTRL_FLG_FIFO_IDLE      (0x40U)
#define SVC_RAW_ENC_CTRL_FLG_FIFO_PROC      (0x80U)
    char                      Name[32];
    RAW_ENC_TSK_CTRL_s        RawEnc;
    RAW_ENC_STRM_CAP_CTRL_s   StrmCap;
} SVC_RAW_ENC_TASK_CTRL_s;

static UINT32 RawEncTask_MutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut);
static void   RawEncTask_MutexGive(AMBA_KAL_MUTEX_t *pMutex);
static void   RawEncTask_snprintf(char *pBuf, UINT32 BufSize, SVC_WRAP_SNPRINT_s *pFmt);
static void   RawEncTask_ErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode);

static void  RawEncTask_TaskCreate(void);
static void* RawEncTask_TaskEntry(void* EntryArg);
static void  RawEncTask_PreFeedData(UINT32 VinSelBits);
static void  RawEncTask_PostFeedData(void);
static void  RawEncTask_UpdateAaa(const SVC_RAW_ENC_UPD_AAA_INFO *pUpdInfo);
static void  RawEncTask_UpdateIqPath(const SVC_RAW_ENC_UPD_IQPATH *pUpdIQPath);
static void  RawEncTask_UpdateIqPathEx(const SVC_RAW_ENC_UPD_IQPATH *pUpdIQPath);
static void  RawEncTask_UpdateIqPathEx1(const SVC_RAW_ENC_UPD_IQPATH *pUpdIQPath);
static void  RawEncTask_CallBackProc(UINT32 ProcCode, void *pData);
static void  RawEncTask_IqTblPathProc(IQ_TABLE_PATH_s *pIqTblPath);
static void  RawEncTask_fprintf(AMBA_FS_FILE *pFile, const char *pStr);
static void  RawEncTask_strcat(char *pStr1, UINT32 Str1Size, const char *pStr2);
static void  RawEncTask_GenScriptDirClear(const char *pScriptDir);
static void  RawEncTask_GenScriptDir(char *pScriptPath);
static void  RawEncTask_GenScriptItn(char *pScriptPath);
static void  RawEncTask_GenScriptRaw(UINT32 VinID, char *pScriptPath, UINT32 ReqFrameNum);
static void  RawEncTask_GenScript(char *pScriptPath, UINT32 CapFrameNum, UINT32 FeedFrameNum, UINT32 ReqVinID);
static void  RawEncTask_GenScriptDmySen(AMBA_FS_FILE *pFile, char *pStrBuf, UINT32 StrBufSize, UINT32 VinID);
static void  RawEncTask_GenScriptResCfg(AMBA_FS_FILE *pFile, char *pStrBuf, UINT32 StrBufSize, const SVC_RES_CFG_s* pResCfg, UINT32 VinID);
static void  RawEncTask_GenScriptIqTable(AMBA_FS_FILE *pFile, char *pStrBuf, UINT32 StrBufSize, const char *pScriptPrefix);
static void  RawEncTask_GenScriptFeed(AMBA_FS_FILE *pFile, char *pStrBuf, UINT32 StrBufSize, const char *pScriptPrefix, UINT32 CapFrameNum, UINT32 FeedFrameNum, UINT32 VinID);

static        UINT32 RawEncTask_MemDiff(const void *pVal0, const void *pVal1);
static inline UINT32 RawEncTask_MsgDataRawInfoGet(const RAW_ENC_STRM_CAP_MSG_UNIT_s *pMsgData, RAW_ENC_STRM_CAP_RAW_INFO_s **pInfo, UINT32 InfoSize);
static inline UINT32 RawEncTask_MsgDataVidInfoGet(const RAW_ENC_STRM_CAP_MSG_UNIT_s *pMsgData, RAW_ENC_STRM_CAP_VIDEO_INFO_s **pInfo, UINT32 InfoSize);
static        UINT32 RawEncTask_GetStrmCapFileName(UINT32 StreamID, char *pFileName, UINT32 Length, UINT32 TimeOut);
static        void*  RawEncTask_StrmCapMsgTaskEntry(void* EntryArg);
static        void*  RawEncTask_StrmCapFifoTaskEntry(void* EntryArg);
static        UINT32 RawEncTask_StrmCapRawDataRdyHdlr(const void *pEventData);
static        UINT32 RawEncTask_StrmCapVidDataRdyHdlr(const void *pEventData);
static        void   RawEncTask_StrmCapScannerUpd(const RAW_ENC_STRM_CAP_RAW_INFO_s *pInfo);
static        void   RawEncTask_StrmCapScannerProc(UINT32 VinID, UINT64 TimeStamp, UINT64 *pRawSeq);
static        void   RawEncTask_StrmCapDataCap(const RAW_ENC_STRM_CAP_VIDEO_INFO_s *pInfo);
static        void   RawEncTask_StrmCapFileOpen(const RAW_ENC_STRM_CAP_QUE_UNIT_s *pUnit);
static        void   RawEncTask_StrmCapFileClose(const RAW_ENC_STRM_CAP_QUE_UNIT_s *pUnit);
static        void   RawEncTask_StrmCapFileWrite(const RAW_ENC_STRM_CAP_QUE_UNIT_s *pUnit);
static        void   RawEncTask_StrmCapUserCap(const RAW_ENC_STRM_CAP_VIDEO_INFO_s *pInfo);
static        void   RawEncTask_StrmCapUserFileOpen(const RAW_ENC_STRM_CAP_QUE_UNIT_s *pUnit);
static        void   RawEncTask_StrmCapUserFileClose(const RAW_ENC_STRM_CAP_QUE_UNIT_s *pUnit);
static        void   RawEncTask_StrmCapUserFileWrite(const RAW_ENC_STRM_CAP_QUE_UNIT_s *pUnit);
static        void   RawEncTask_StrmCapSaveBinFile(const char *pFileName, void *pData, UINT32 DataSize);
static        void   RawEncTask_StrmCapPostProc(const char *pSeqFilePath);
static        void   RawEncTask_StrmCapPostSeperate(const char *pSeqFilePath);
static        void   RawEncTask_StrmCapPostText(const char *pSeqFilePath);

typedef UINT32 (*SVC_RAW_ENC_TSK_SHELL_FUNC_f)(UINT32 ID, UINT32 ArgCount, char * const *pArgVector);
typedef void   (*SVC_RAW_ENC_TSK_SHELL_USAGE_f)(UINT32 ID, UINT32 CtrlFlag);
typedef struct {
    UINT32                    Enable;
    UINT32                    Id;
    char                      CmdName[32];
    SVC_RAW_ENC_TSK_SHELL_FUNC_f  pFunc;
    SVC_RAW_ENC_TSK_SHELL_USAGE_f pUsage;
} SVC_RAW_ENC_TSK_SHELL_FUNC_s;

static void   RawEncTask_CmdStrToU32(const char *pString, UINT32 *pValue);
static UINT32 RawEncTask_ShellArgChk(UINT32 ArgCount, char * const *pArgVector, UINT32 ArgChkCount);
static void   RawEncTask_ShellTitle(UINT32 CtrlFlag, UINT32 Level, const char *pCmdStr, const char *pDescription);
static UINT32 RawEncTask_ShellRecStart(UINT32 ID, UINT32 ArgCount, char * const *pArgVector);
static void   RawEncTask_ShellRecStartU(UINT32 ID, UINT32 CtrlFlag);
static UINT32 RawEncTask_ShellRecStop(UINT32 ID, UINT32 ArgCount, char * const *pArgVector);
static void   RawEncTask_ShellRecStopU(UINT32 ID, UINT32 CtrlFlag);
static UINT32 RawEncTask_ShellGenScript(UINT32 ID, UINT32 ArgCount, char * const *pArgVector);
static void   RawEncTask_ShellGenScriptU(UINT32 ID, UINT32 CtrlFlag);
static UINT32 RawEncTask_ShellStrmCap(UINT32 ID, UINT32 ArgCount, char * const *pArgVector);
static void   RawEncTask_ShellStrmCapU(UINT32 ID, UINT32 CtrlFlag);
static UINT32 RawEncTask_ShellDbgMsg(UINT32 ID, UINT32 ArgCount, char * const *pArgVector);
static void   RawEncTask_ShellDbgMsgU(UINT32 ID, UINT32 CtrlFlag);
static void   RawEncTask_ShellEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
static void   RawEncTask_ShellUsage(UINT32 CtrlFlag);
static void   RawEncTask_ShellInit(void);
static void   RawEncTask_CmdInstall(void);

#define SVC_RAW_ENC_SHELL_NUM (5U)
static SVC_RAW_ENC_TSK_SHELL_FUNC_s RawEncShellFunc[SVC_RAW_ENC_SHELL_NUM] GNU_SECTION_NOZEROINIT;
static SVC_RAW_ENC_TASK_CTRL_s RawEncCtrl GNU_SECTION_NOZEROINIT;
static IQ_TABLE_PATH_s RawEncIqTblPath GNU_SECTION_NOZEROINIT;
static UINT32 RawEncTskCtrlFlg = SVC_RAW_ENC_FLG_MSG_DEF;
static SVC_RAW_ENC_STRM_CAP_USER_f pRawEncTskUserCallBack = NULL;

#define SVC_LOG_RENC_TSK "RENC_TSK"
#define SVC_LOG_RENC_TSK_HL_MAX_TITLE_NUM   (4U)
#define SVC_LOG_RENC_TSK_HL_ALIGN_POS       (24U)
#define SVC_LOG_RENC_TSK_HL_TITLE_0   "\033""[38;2;255;125;38m"
#define SVC_LOG_RENC_TSK_HL_TITLE_1   "\033""[38;2;255;255;128m"
#define SVC_LOG_RENC_TSK_HL_TITLE_1_U "\033""[38;2;255;255;128m""\033""[4m"
#define SVC_LOG_RENC_TSK_HL_TITLE_2   "\033""[38;2;100;255;255m"
#define SVC_LOG_RENC_TSK_HL_TITLE_2_U "\033""[38;2;100;255;255m""\033""[4m"
#define SVC_LOG_RENC_TSK_HL_TITLE_3   "\033""[38;2;255;128;128m"
#define SVC_LOG_RENC_TSK_HL_TITLE_3_U "\033""[38;2;255;128;128m""\033""[4m"
#define SVC_LOG_RENC_TSK_HL_RAW_SCAN  "\033""[38;2;25;237;147m"             // green
#define SVC_LOG_RENC_TSK_HL_STRM_CAP  "\033""[38;2;190;190;18m"             // yellow
#define SVC_LOG_RENC_TSK_HL_FIFO      "\033""[38;2;18;150;190m"             // blue
#define SVC_LOG_RENC_TSK_HL_NUM       "\033""[38;2;153;217;234m"
#define SVC_LOG_RENC_TSK_HL_STR       "\033""[38;2;255;174;201m"
#define SVC_LOG_RENC_TSK_HL_DEF_FC    "\033""[39m"
#define SVC_LOG_RENC_TSK_HL_END       "\033""[0m"

#define PRN_RENC_TSK_LOG        { SVC_WRAP_PRINT_s SvcRawEncTskPrint; AmbaSvcWrap_MisraMemset(&(SvcRawEncTskPrint), 0, sizeof(SvcRawEncTskPrint)); SvcRawEncTskPrint.Argc --; SvcRawEncTskPrint.pStrFmt =
#define PRN_RENC_TSK_ARG_UINT64 ; SvcRawEncTskPrint.Argc ++; SvcRawEncTskPrint.Argv[SvcRawEncTskPrint.Argc].Uint64 =
#define PRN_RENC_TSK_ARG_UINT32 ; SvcRawEncTskPrint.Argc ++; SvcRawEncTskPrint.Argv[SvcRawEncTskPrint.Argc].Uint64   = (UINT64)((
#define PRN_RENC_TSK_ARG_CSTR   ; SvcRawEncTskPrint.Argc ++; SvcRawEncTskPrint.Argv[SvcRawEncTskPrint.Argc].pCStr    = ((
#define PRN_RENC_TSK_ARG_CPOINT ; SvcRawEncTskPrint.Argc ++; SvcRawEncTskPrint.Argv[SvcRawEncTskPrint.Argc].pPointer = ((
#define PRN_RENC_TSK_ARG_POST   ))
#define PRN_RENC_TSK_OK         ; SvcRawEncTskPrint.Argc ++; RawEncTask_PrintLog(SVC_RAW_ENC_FLG_MSG_OK ,  &(SvcRawEncTskPrint)); }
#define PRN_RENC_TSK_NG         ; SvcRawEncTskPrint.Argc ++; RawEncTask_PrintLog(SVC_RAW_ENC_FLG_MSG_NG ,  &(SvcRawEncTskPrint)); }
#define PRN_RENC_TSK_API        ; SvcRawEncTskPrint.Argc ++; RawEncTask_PrintLog(SVC_RAW_ENC_FLG_MSG_API,  &(SvcRawEncTskPrint)); }
#define PRN_RENC_TSK_DBG        ; SvcRawEncTskPrint.Argc ++; RawEncTask_PrintLog(SVC_RAW_ENC_FLG_MSG_DBG,  &(SvcRawEncTskPrint)); }
#define PRN_RENC_TSK_DBG1       ; SvcRawEncTskPrint.Argc ++; RawEncTask_PrintLog(SVC_RAW_ENC_FLG_MSG_DBG1, &(SvcRawEncTskPrint)); }
#define PRN_RENC_TSK_ERR_HDLR   RawEncTask_ErrHdlr(__func__, __LINE__, ErrCode );

static void RawEncTask_PrintLog(UINT32 LogLevel, SVC_WRAP_PRINT_s *pPrint)
{
    if (pPrint != NULL) {
        if ((RawEncTskCtrlFlg & LogLevel) > 0U) {

            switch (LogLevel) {
            case SVC_RAW_ENC_FLG_MSG_OK :
                pPrint->pProc = SvcLog_OK;
                break;
            case SVC_RAW_ENC_FLG_MSG_NG :
                pPrint->pProc = SvcLog_NG;
                break;
            default :
                pPrint->pProc = SvcLog_DBG;
                break;
            }

            SvcWrap_Print(SVC_LOG_RENC_TSK, pPrint);
        }
    }
}

static UINT32 RawEncTask_MutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut)
{
    return AmbaKAL_MutexTake(pMutex, TimeOut);
}

static void RawEncTask_MutexGive(AMBA_KAL_MUTEX_t *pMutex)
{
    UINT32 ErrCode = AmbaKAL_MutexGive(pMutex);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }
}

static void RawEncTask_ErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode)
{
    if (pCaller != NULL) {
        if (ErrCode != 0U) {
            PRN_RENC_TSK_LOG "Catch ErrCode(0x%08X) @ %s, %d"
            PRN_RENC_TSK_ARG_UINT32 ErrCode  PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_CSTR   pCaller  PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_UINT32 CodeLine PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_NG
        }
    }
}

static void RawEncTask_TaskCreate(void)
{
    if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_TASK_INIT) == 0U) {

        RawEncCtrl.RawEnc.TaskCtrl.Priority   = (UINT32)SVC_RAW_ENC_TASK_PRI;
        RawEncCtrl.RawEnc.TaskCtrl.EntryFunc  = RawEncTask_TaskEntry;
        RawEncCtrl.RawEnc.TaskCtrl.EntryArg   = 0U;
        RawEncCtrl.RawEnc.TaskCtrl.pStackBase = RawEncCtrl.RawEnc.TaskStack;
        RawEncCtrl.RawEnc.TaskCtrl.StackSize  = (UINT32)sizeof(RawEncCtrl.RawEnc.TaskStack);
        RawEncCtrl.RawEnc.TaskCtrl.CpuBits    = SVC_RAW_ENC_TASK_CPU_BITS;

        if (0U != SvcTask_Create(RawEncCtrl.Name, &(RawEncCtrl.RawEnc.TaskCtrl))) {
            PRN_RENC_TSK_LOG "Fail to init raw enc - create raw enc task fail!" PRN_RENC_TSK_NG
        } else {
            RawEncTskCtrlFlg |= SVC_RAW_ENC_FLG_TASK_INIT;
        }
    }
}

static void* RawEncTask_TaskEntry(void* EntryArg)
{
    UINT32 ActualFlags = 0U, ErrCode;
    ULONG ArgVal = 0U;
    UINT32 RecBits = 0x1U;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    if (pResCfg != NULL) {
        RecBits = pResCfg->RecBits;
    }

    AmbaMisra_TouchUnused(EntryArg);
    AmbaSvcWrap_MisraMemcpy(&ArgVal, EntryArg, sizeof(ULONG));

    while(ArgVal != 0xFFFFFFFFU) {
        if (SVC_OK == AmbaKAL_EventFlagGet(&(RawEncCtrl.Flag),
                                           (SVC_RAW_ENC_CTRL_FLG_REC_START | SVC_RAW_ENC_CTRL_FLG_REC_STOP),
                                           AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_NONE,
                                           &ActualFlags, AMBA_KAL_WAIT_FOREVER)) {

            if ((ActualFlags & SVC_RAW_ENC_CTRL_FLG_REC_START) > 0U) {
                if (0U == AmbaKAL_EventFlagGet(&(RawEncCtrl.Flag),
                                               SVC_RAW_ENC_CTRL_FLG_DAT_LOAD_DONE,
                                               AMBA_KAL_FLAGS_ANY,
                                               AMBA_KAL_FLAGS_CLEAR_AUTO,
                                               &ActualFlags,
                                               30000U)) {

                    PRN_RENC_TSK_LOG "Start video recording with RecBits(0x%x)"
                        PRN_RENC_TSK_ARG_UINT32 RecBits PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_API
                    SvcRecMain_Start(RecBits, 0U);
                } else {
                    PRN_RENC_TSK_LOG "Fail to start video recording - load data first!" PRN_RENC_TSK_NG
                }

                ErrCode = AmbaKAL_EventFlagClear(&(RawEncCtrl.Flag), SVC_RAW_ENC_CTRL_FLG_REC_START); PRN_RENC_TSK_ERR_HDLR

            } else if ((ActualFlags & SVC_RAW_ENC_CTRL_FLG_REC_STOP) > 0U) {
                SvcRecMain_Stop(RecBits, 0U);
                ErrCode = AmbaKAL_EventFlagClear(&(RawEncCtrl.Flag), SVC_RAW_ENC_CTRL_FLG_REC_STOP); PRN_RENC_TSK_ERR_HDLR
            } else {
                // Do nothing
            }
        }

        AmbaMisra_TouchUnused(&ArgVal);
    }

    return NULL;
}

static void RawEncTask_PreFeedData(UINT32 VinSelBits)
{
    UINT32 ErrCode;

    if (RawEncCtrl.RawEnc.FrameInterval == 0U) {
        UINT32 VinIdx;
        AMBA_SENSOR_CHANNEL_s SenChan;
        AMBA_SENSOR_STATUS_INFO_s SenStatus;
        AMBA_KAL_EVENT_FLAG_INFO_s FlagInfo;

        for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
            if ((VinSelBits & SvcRawEnc_BitGet(VinIdx)) > 0U) {
                AmbaSvcWrap_MisraMemset(&SenChan,   0, sizeof(SenChan));
                SenChan.VinID    = VinIdx;
                SenChan.SensorID = 1;
                AmbaSvcWrap_MisraMemset(&SenStatus, 0, sizeof(SenStatus));
                ErrCode = AmbaSensor_GetStatus(&SenChan, &SenStatus); PRN_RENC_TSK_ERR_HDLR
                RawEncCtrl.RawEnc.FrameInterval = ( 1000U * SenStatus.ModeInfo.FrameRate.NumUnitsInTick );
                if (SenStatus.ModeInfo.FrameRate.TimeScale > 0U) {
                    RawEncCtrl.RawEnc.FrameInterval /= SenStatus.ModeInfo.FrameRate.TimeScale;
                }
                break;
            }
        }

        PRN_RENC_TSK_LOG "Configure frame interval %d ms"
            PRN_RENC_TSK_ARG_UINT32 RawEncCtrl.RawEnc.FrameInterval PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_API

        AmbaSvcWrap_MisraMemset(&FlagInfo, 0, sizeof(FlagInfo));
        ErrCode = AmbaKAL_EventFlagQuery(&(RawEncCtrl.Flag), &FlagInfo);
        if (ErrCode == 0U) {
            if ((FlagInfo.CurrentFlags & SVC_RAW_ENC_CTRL_FLG_REC_START) > 0U) {
                ErrCode = AmbaKAL_EventFlagSet(&(RawEncCtrl.Flag), SVC_RAW_ENC_CTRL_FLG_DAT_LOAD_DONE);
                if (ErrCode != 0U) {
                    PRN_RENC_TSK_LOG "Fail to proc post data feeding - set load done flag! ErrCode(0x%08X)"
                        PRN_RENC_TSK_ARG_UINT32 ErrCode PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_NG
                }
            }
        }
    }
}

static void RawEncTask_PostFeedData(void)
{
    if (RawEncCtrl.RawEnc.FrameInterval > 0U) {
        UINT32 CurTimeStamp, TimeStampDiff;

        if (0U != AmbaKAL_GetSysTickCount(&CurTimeStamp)) {
            CurTimeStamp = 0U;
        }

        if (CurTimeStamp >= RawEncCtrl.RawEnc.FeedDataTimeStamp) {
            TimeStampDiff = CurTimeStamp - RawEncCtrl.RawEnc.FeedDataTimeStamp;
        } else {
            TimeStampDiff = ( 0xFFFFFFFFU - RawEncCtrl.RawEnc.FeedDataTimeStamp ) + CurTimeStamp;
        }

        if (TimeStampDiff < RawEncCtrl.RawEnc.FrameInterval) {
            UINT32 WaitTime = RawEncCtrl.RawEnc.FrameInterval - TimeStampDiff;
            PRN_RENC_TSK_LOG "Wait %d ms before feed data!"
                PRN_RENC_TSK_ARG_UINT32 WaitTime PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_API
            if (0U != AmbaKAL_TaskSleep(WaitTime)) {
                PRN_RENC_TSK_LOG "Wait %d ms fail!"
                    PRN_RENC_TSK_ARG_UINT32 WaitTime PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_API
            }
        }

        if (0U != AmbaKAL_GetSysTickCount(&CurTimeStamp)) {
            CurTimeStamp = 0U;
        }
        RawEncCtrl.RawEnc.FeedDataTimeStamp = CurTimeStamp;
    }

}

static void RawEncTask_UpdateAaa(const SVC_RAW_ENC_UPD_AAA_INFO *pUpdInfo)
{
    UINT32 PRetVal, ErrCode;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    if ((pUpdInfo != NULL) && (pResCfg != NULL)) {
        SVC_RAW_CAP_AAA_DATA_s          *pAaaData = NULL;
        AMBA_AE_INFO_s                  *pAeInfo  = NULL;
        AMBA_IMGPROC_OFFLINE_AAA_INFO_s *pAaaInfo = NULL;
        UINT32                           ExpNum = 0U;

        AmbaMisra_TypeCast(&(pAaaData), &(pUpdInfo->pAaaData));

        if (pAaaData != NULL) {
            if (pAaaData->MagicCode == SVC_RAW_CAP_AAA_MAGIC_CODE) {
                pAeInfo  = pAaaData->AeInfo;
                pAaaInfo = &(pAaaData->AaaInfo);
                ExpNum   = pAaaData->ExposureNum;

            } else {
                UINT32 DataBase, InfoBase;

                ExpNum = pResCfg->FovCfg[0U].PipeCfg.HdrExposureNum;

                AmbaMisra_TypeCast(&(DataBase), &(pUpdInfo->pAaaData));
                InfoBase = DataBase + 0x64U;
                AmbaMisra_TypeCast(&(pAeInfo),  &(InfoBase));
                InfoBase = DataBase + 0x1560U;
                AmbaMisra_TypeCast(&(pAaaInfo), &(InfoBase));
            }

            if ((pAeInfo == NULL) || (pAaaInfo == NULL)) {
                PRN_RENC_TSK_LOG "Fail to execute aaa data - invalid AeInfo(%p) or AaaInfo(%p)!"
                    PRN_RENC_TSK_ARG_CPOINT pAeInfo  PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_ARG_CPOINT pAaaInfo PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_NG
            } else {
                UINT32 ExpIdx;

                if (ExpNum == 0U) {
                    AMBA_SENSOR_CHANNEL_s SsChan;
                    AMBA_SENSOR_STATUS_INFO_s SsInfo;

                    AmbaSvcWrap_MisraMemset(&SsChan, 0, sizeof(SsChan));
                    SsChan.VinID = 0U;

                    AmbaSvcWrap_MisraMemset(&SsInfo, 0, sizeof(SsInfo));
                    ErrCode = AmbaSensor_GetStatus(&SsChan, &SsInfo); PRN_RENC_TSK_ERR_HDLR
                    if (SsInfo.ModeInfo.HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
                        ExpNum = SsInfo.ModeInfo.HdrInfo.ActiveChannels;
                    } else {
                        ExpNum = 1U;
                    }
                }

                if (ExpNum > SVC_RAW_CAP_AAA_MAX_EXP_NUM) {
                    ExpNum = 0U;
                    PRN_RENC_TSK_LOG "Fail to execute aaa data - invalid img chan(%d) exp num(%d)!"
                        PRN_RENC_TSK_ARG_UINT32 pUpdInfo->ImgChanId PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_ARG_UINT32 ExpNum              PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_NG
                }

                for (ExpIdx = 0U; ExpIdx < ExpNum; ExpIdx ++) {
                    PRetVal = AmbaImgProc_AESetExpInfo(pUpdInfo->ImgChanId, ExpIdx, IP_MODE_VIDEO, &(pAeInfo[ExpIdx]));
                    if (PRetVal != 0U) {
                        PRN_RENC_TSK_LOG "Fail to execute aaa data - set the img chan(%d) no.%d exp ae info fail!"
                            PRN_RENC_TSK_ARG_UINT32 pUpdInfo->ImgChanId PRN_RENC_TSK_ARG_POST
                            PRN_RENC_TSK_ARG_UINT32 ExpIdx              PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_NG
                        break;
                    }
                }

                PRetVal = AmbaImgProc_SetOfflineAAAInfo(pUpdInfo->ImgChanId, *pAaaInfo);
                if (PRetVal != 0U) {
                    PRN_RENC_TSK_LOG "Fail to execute aaa data - set the img chan(%d) offline aaa info fail!"
                        PRN_RENC_TSK_ARG_UINT32 pUpdInfo->ImgChanId PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_NG
                }
            }
        }

        AmbaMisra_TouchUnused(pAaaInfo);
        AmbaMisra_TouchUnused(pAeInfo);
    }

}

static void RawEncTask_UpdateIqPath(const SVC_RAW_ENC_UPD_IQPATH *pUpdIQPath)
{
    UINT32 PRetVal;
    IQ_TABLE_PATH_s *pIqTblPath = &RawEncIqTblPath;

    if (pUpdIQPath != NULL) {
        TABLE_PATH *pTblPath = NULL;

        switch (pUpdIQPath->ID) {
        case SVC_RAW_ENC_UPD_ADJ_PATH :
            pTblPath = &(pIqTblPath->ADJTable);
            break;
        case SVC_RAW_ENC_UPD_IMG_PATH :
            pTblPath = &(pIqTblPath->ImgParam);
            break;
        case SVC_RAW_ENC_UPD_AAA_PATH :
            if (pUpdIQPath->TableNo < AAA_TABLE_MAX_NO) {
                pTblPath = &(pIqTblPath->aaaDefault[pUpdIQPath->TableNo]);
            }
            break;
        case SVC_RAW_ENC_UPD_VIDEO_PATH :
            if (pUpdIQPath->TableNo < ADJ_VIDEO_TABLE_MAX_NO) {
                pTblPath = &(pIqTblPath->video[pUpdIQPath->TableNo]);
            }
            break;
        case SVC_RAW_ENC_UPD_PHOTO_PATH :
            if (pUpdIQPath->TableNo < ADJ_PHOTO_TABLE_MAX_NO) {
                pTblPath = &(pIqTblPath->photo[pUpdIQPath->TableNo]);
            }
            break;
        case SVC_RAW_ENC_UPD_SLISO_PATH :
            if (pUpdIQPath->TableNo < ADJ_STILL_LISO_TABLE_MAX_NO) {
                pTblPath = &(pIqTblPath->stillLISO[pUpdIQPath->TableNo]);
            }
            break;
        case SVC_RAW_ENC_UPD_SHISO_PATH :
            if (pUpdIQPath->TableNo < ADJ_STILL_HISO_TABLE_MAX_NO) {
                pTblPath = &(pIqTblPath->stillHISO[pUpdIQPath->TableNo]);
            }
            break;
        default :
            // Do nothing
            break;
        }

        if ((pTblPath != NULL) && (pUpdIQPath->pPath != NULL)) {
            AmbaSvcWrap_MisraMemset(pTblPath, 0, sizeof(TABLE_PATH));
            SvcWrap_strcpy(pTblPath->path, PATHSIZE, pUpdIQPath->pPath);
            PRetVal = Amba_IQParam_RegisterIQPathProc(RawEncTask_IqTblPathProc);
            if (PRetVal != 0U) {
                PRN_RENC_TSK_LOG "Fail to register IQ patch callback! ErrCode(0x%08X)"
                    PRN_RENC_TSK_ARG_UINT32 PRetVal PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_NG
            }

            PRN_RENC_TSK_LOG "path : %s"
                PRN_RENC_TSK_ARG_CSTR   pTblPath->path PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_API
        }
    }
}

static void RawEncTask_UpdateIqPathEx(const SVC_RAW_ENC_UPD_IQPATH *pUpdIQPath)
{
    UINT32 PRetVal;
    IQ_TABLE_PATH_s *pIqTblPath = &RawEncIqTblPath;

    if (pUpdIQPath != NULL) {
        TABLE_PATH *pTblPath = NULL;

        switch (pUpdIQPath->ID) {
        case SVC_RAW_ENC_UPD_VIDCC_PATH :
            if ((pUpdIQPath->TableNo < DEF_CC_TABLE_NO) && (pUpdIQPath->CCSetPathNo < CC_SET_MAX_NO)) {
                pTblPath = &(pIqTblPath->CCSetPaths[pUpdIQPath->CCSetPathNo].videoCC[pUpdIQPath->TableNo]);
            }
            break;
        case SVC_RAW_ENC_UPD_STLCC_PATH :
            if ((pUpdIQPath->TableNo < DEF_CC_TABLE_NO) && (pUpdIQPath->CCSetPathNo < CC_SET_MAX_NO)) {
                pTblPath = &(pIqTblPath->CCSetPaths[pUpdIQPath->CCSetPathNo].stillCC[pUpdIQPath->TableNo]);
            }
            break;
        case SVC_RAW_ENC_UPD_SCEEN_PATH :
            if (pUpdIQPath->TableNo < SCENE_TABLE_MAX_NO) {
                pTblPath = &(pIqTblPath->scene[pUpdIQPath->TableNo]);
            }
            break;
        case SVC_RAW_ENC_UPD_DVID_PATH :
            if (pUpdIQPath->TableNo < DE_VIDEO_TABLE_MAX_NO) {
                pTblPath = &(pIqTblPath->DEVideo[pUpdIQPath->TableNo]);
            }
            break;
        case SVC_RAW_ENC_UPD_DSTL_PATH :
            if (pUpdIQPath->TableNo < DE_STILL_TABLE_MAX_NO) {
                pTblPath = &(pIqTblPath->DEStill[pUpdIQPath->TableNo]);
            }
            break;
        default :
            // Do nothing
            break;
        }

        if ((pTblPath != NULL) && (pUpdIQPath->pPath != NULL)) {
            AmbaSvcWrap_MisraMemset(pTblPath, 0, sizeof(TABLE_PATH));
            SvcWrap_strcpy(pTblPath->path, PATHSIZE, pUpdIQPath->pPath);
            PRetVal = Amba_IQParam_RegisterIQPathProc(RawEncTask_IqTblPathProc);
            if (PRetVal != 0U) {
                PRN_RENC_TSK_LOG "Fail to register IQ patch callback! ErrCode(0x%08X)"
                    PRN_RENC_TSK_ARG_UINT32 PRetVal PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_NG
            }

            PRN_RENC_TSK_LOG "path : %s"
                PRN_RENC_TSK_ARG_CSTR   pTblPath->path PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_API
        }
    }
}

static void RawEncTask_UpdateIqPathEx1(const SVC_RAW_ENC_UPD_IQPATH *pUpdIQPath)
{
    UINT32 PRetVal;
    IQ_TABLE_PATH_s *pIqTblPath = &RawEncIqTblPath;

    if (pUpdIQPath != NULL) {
        TABLE_PATH *pTblPath = NULL;

        switch (pUpdIQPath->ID) {
        case SVC_RAW_ENC_UPD_VIDEO_MSM_PATH :
            if ((pUpdIQPath->TableNo < ADJ_VIDEO_MSM_TABLE_MAX_NO)) {
                pTblPath = &(pIqTblPath->videoMSM[pUpdIQPath->TableNo]);
            }
            break;
        case SVC_RAW_ENC_UPD_VIDEO_MSH_PATH :
            if ((pUpdIQPath->TableNo < ADJ_VIDEO_MSH_TABLE_MAX_NO)) {
                pTblPath = &(pIqTblPath->videoMSH[pUpdIQPath->TableNo]);
            }
            break;
        case SVC_RAW_ENC_UPD_STL_PARAM_PATH :
            pTblPath = &(pIqTblPath->stillIdxInfo);
            break;
        default :
            // Do nothing
            break;
        }

        if ((pTblPath != NULL) && (pUpdIQPath->pPath != NULL)) {
            AmbaSvcWrap_MisraMemset(pTblPath, 0, sizeof(TABLE_PATH));
            SvcWrap_strcpy(pTblPath->path, PATHSIZE, pUpdIQPath->pPath);
            PRetVal = Amba_IQParam_RegisterIQPathProc(RawEncTask_IqTblPathProc);
            if (PRetVal != 0U) {
                PRN_RENC_TSK_LOG "Fail to register IQ patch callback! ErrCode(0x%08X)"
                    PRN_RENC_TSK_ARG_UINT32 PRetVal PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_NG
            }

            PRN_RENC_TSK_LOG "path : %s"
                PRN_RENC_TSK_ARG_CSTR   pTblPath->path PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_API
        }
    }
}

static void RawEncTask_CallBackProc(UINT32 ProcCode, void *pData)
{
    if (ProcCode == SVC_RAW_ENC_CODE_PRE_FEED) {
        const UINT32 *pVinSelBits;
        AmbaMisra_TypeCast(&(pVinSelBits), &(pData));

        if (pVinSelBits != NULL) {
            RawEncTask_PreFeedData(*pVinSelBits);
        }
    }

    if (ProcCode == SVC_RAW_ENC_CODE_POST_FEED) {
        RawEncTask_PostFeedData();
    }

    if (ProcCode == SVC_RAW_ENC_CODE_UPD_AAA) {
        const SVC_RAW_ENC_UPD_AAA_INFO *pUpdInfo;

        AmbaMisra_TypeCast(&(pUpdInfo), &(pData));

        RawEncTask_UpdateAaa(pUpdInfo);
    }

    if (ProcCode == SVC_RAW_ENC_CODE_UPD_IQPATH) {
        const SVC_RAW_ENC_UPD_IQPATH *pUpdIQPath;

        AmbaMisra_TypeCast(&(pUpdIQPath), &(pData));

        RawEncTask_UpdateIqPath(pUpdIQPath);
        RawEncTask_UpdateIqPathEx(pUpdIQPath);
        RawEncTask_UpdateIqPathEx1(pUpdIQPath);
    }

    AmbaMisra_TouchUnused(pData);
}

static void RawEncTask_IqTblPathProc(IQ_TABLE_PATH_s *pIqTblPath)
{
    if (pIqTblPath != NULL) {
        AmbaSvcWrap_MisraMemcpy(pIqTblPath, &RawEncIqTblPath, sizeof(IQ_TABLE_PATH_s));
    }
}

static void RawEncTask_CmdStrToU32(const char *pString, UINT32 *pValue)
{
    UINT32 ErrCode = AmbaUtility_StringToUInt32(pString, pValue);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }
}

static UINT32 RawEncTask_ShellArgChk(UINT32 ArgCount, char * const *pArgVector, UINT32 ArgChkCount)
{
    UINT32 RetVal;

    if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_TSK_LOG "Fail to proc chk cmd - arg vector should not null!" PRN_RENC_TSK_NG
    } else if (pArgVector[0] == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_TSK_LOG "Fail to proc chk cmd - invalid arg!" PRN_RENC_TSK_NG
    } else if (ArgCount < ArgChkCount) {
        RetVal = SVC_NG;
        PRN_RENC_TSK_LOG "Fail to proc chk cmd - '%s' arg cnt is not correct! %d != %d"
            PRN_RENC_TSK_ARG_CSTR   pArgVector[0] PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_UINT32 ArgCount      PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_UINT32 ArgChkCount   PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_NG
    } else {
        RetVal = SVC_OK;
    }

    return RetVal;
}

static void RawEncTask_ShellTitle(UINT32 CtrlFlag, UINT32 Level, const char *pCmdStr, const char *pDescription)
{
    if ((pCmdStr != NULL) && (Level < SVC_LOG_RENC_TSK_HL_MAX_TITLE_NUM)) {
        const char FrontHL[SVC_LOG_RENC_TSK_HL_MAX_TITLE_NUM][2][40] = {
            { SVC_LOG_RENC_TSK_HL_TITLE_0, SVC_LOG_RENC_TSK_HL_TITLE_0   },
            { SVC_LOG_RENC_TSK_HL_TITLE_1, SVC_LOG_RENC_TSK_HL_TITLE_1_U },
            { SVC_LOG_RENC_TSK_HL_TITLE_2, SVC_LOG_RENC_TSK_HL_TITLE_2_U },
            { SVC_LOG_RENC_TSK_HL_TITLE_3, SVC_LOG_RENC_TSK_HL_TITLE_3_U },
        };
        const char MiddleHL[2][40] = {
            SVC_LOG_RENC_TSK_HL_DEF_FC, SVC_LOG_RENC_TSK_HL_END
        };
        char FrontPadding[10];
        char MiddlePadding[30];
        UINT32 FrontPaddingSize = Level << 1UL;
        UINT32 CmdStrLen = (UINT32)SvcWrap_strlen(pCmdStr);
        UINT32 AlignPos = SVC_LOG_RENC_TSK_HL_ALIGN_POS - FrontPaddingSize;

        if (AlignPos > CmdStrLen) {
            AlignPos -= CmdStrLen;
        } else {
            AlignPos = 0U;
        }

        AmbaSvcWrap_MisraMemset(FrontPadding, 32, FrontPaddingSize);
        FrontPadding[FrontPaddingSize] = '\0';

        AmbaSvcWrap_MisraMemset(MiddlePadding, 32, AlignPos);
        MiddlePadding[AlignPos] = ':';
        MiddlePadding[AlignPos + 1U] = ' ';
        MiddlePadding[AlignPos + 2U] = '\0';

        PRN_RENC_TSK_LOG "%s%s%s%s%s%s%s"
            PRN_RENC_TSK_ARG_CSTR   FrontPadding                             PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_CSTR   FrontHL[Level][CtrlFlag & 0x1UL]         PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_CSTR   pCmdStr                                  PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_CSTR   MiddleHL[CtrlFlag & 0x1UL]               PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_CSTR   MiddlePadding                            PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_CSTR   (pDescription==NULL)?(""):(pDescription) PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_END                  PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_API
    }
}

static UINT32 RawEncTask_ShellRecStart(UINT32 ID, UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal;

    RetVal = RawEncTask_ShellArgChk(ArgCount, pArgVector, 1U);
    if ((RetVal == SVC_OK) && (ID < SVC_RAW_ENC_SHELL_NUM)) {
        const char *pCmdName = RawEncShellFunc[ID].CmdName;
        UINT32 PRetVal;
        UINT32 VinSrc = 255U;

        if (0U != SvcResCfg_GetVinSrc(0U, &VinSrc)) {
            RetVal = SVC_NG;
            PRN_RENC_TSK_LOG "Fail to proc '%s' cmd - get Vin0 source fail!"
                PRN_RENC_TSK_ARG_CSTR   pCmdName PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_NG
        } else if (VinSrc != SVC_VIN_SRC_MEM) {
            RetVal = SVC_NG;
            PRN_RENC_TSK_LOG "Fail to proc '%s' cmd - boot to raw enc mode first! VinSrc(%d)"
                PRN_RENC_TSK_ARG_CSTR   pCmdName PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_ARG_UINT32 VinSrc   PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_NG
        } else if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_INIT) == 0U) {
            RetVal = SVC_NG;
            PRN_RENC_TSK_LOG "Fail to proc '%s' cmd - initial video raw encode first!"
                PRN_RENC_TSK_ARG_CSTR   pCmdName PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_NG
        } else {

            RawEncTask_TaskCreate();

            PRetVal = AmbaKAL_EventFlagSet(&(RawEncCtrl.Flag), SVC_RAW_ENC_CTRL_FLG_REC_START);
            if (PRetVal != 0U) {
                PRN_RENC_TSK_LOG "Fail to proc '%s' cmd - set flag fail! ErrCode(0x%08X)"
                    PRN_RENC_TSK_ARG_CSTR   pCmdName PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_ARG_UINT32 PRetVal  PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_NG
            }
        }
    }

    return RetVal;
}

static void RawEncTask_ShellRecStartU(UINT32 ID, UINT32 CtrlFlag)
{
    if (ID < SVC_RAW_ENC_SHELL_NUM) {
        const char *pCmdName = RawEncShellFunc[ID].CmdName;

        RawEncTask_ShellTitle(CtrlFlag, 1U, pCmdName, "start video raw encode recorder");

        if (CtrlFlag > 0U) {
            PRN_RENC_TSK_LOG " " PRN_RENC_TSK_API
        }
    }
}

static UINT32 RawEncTask_ShellRecStop(UINT32 ID, UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal;

    RetVal = RawEncTask_ShellArgChk(ArgCount, pArgVector, 1U);
    if ((RetVal == SVC_OK) && (ID < SVC_RAW_ENC_SHELL_NUM)) {
        const char *pCmdName = RawEncShellFunc[ID].CmdName;
        UINT32 PRetVal;
        UINT32 VinSrc = 255U;

        if (0U != SvcResCfg_GetVinSrc(0U, &VinSrc)) {
            RetVal = SVC_NG;
            PRN_RENC_TSK_LOG "Fail to proc '%s' cmd - get Vin0 source fail!"
                PRN_RENC_TSK_ARG_CSTR   pCmdName PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_NG
        } else if (VinSrc != SVC_VIN_SRC_MEM) {
            RetVal = SVC_NG;
            PRN_RENC_TSK_LOG "Fail to proc '%s' cmd - boot to raw enc mode first! VinSrc(%d)"
                PRN_RENC_TSK_ARG_CSTR   pCmdName PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_ARG_UINT32 VinSrc   PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_NG
        } else if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_INIT) == 0U) {
            RetVal = SVC_NG;
            PRN_RENC_TSK_LOG "Fail to proc '%s' cmd - initial video raw encode first!"
                PRN_RENC_TSK_ARG_CSTR   pCmdName PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_NG
        } else {

            RawEncTask_TaskCreate();

            PRetVal = AmbaKAL_EventFlagSet(&(RawEncCtrl.Flag), SVC_RAW_ENC_CTRL_FLG_REC_STOP);
            if (PRetVal != 0U) {
                PRN_RENC_TSK_LOG "Fail to proc '%s' cmd - set flag fail! ErrCode(0x%08X)"
                    PRN_RENC_TSK_ARG_CSTR   pCmdName PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_ARG_UINT32 PRetVal  PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_NG
            }
        }
    }

    return RetVal;
}

static void RawEncTask_ShellRecStopU(UINT32 ID, UINT32 CtrlFlag)
{
    if (ID < SVC_RAW_ENC_SHELL_NUM) {
        const char *pCmdName = RawEncShellFunc[ID].CmdName;

        RawEncTask_ShellTitle(CtrlFlag, 1U, pCmdName, "stop video raw encode recorder");

        if (CtrlFlag > 0U) {
            PRN_RENC_TSK_LOG " " PRN_RENC_TSK_API
        }
    }
}

static UINT32 RawEncTask_ShellGenScript(UINT32 ID, UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal;

    RetVal = RawEncTask_ShellArgChk(ArgCount, pArgVector, 2U);
    if ((RetVal == SVC_OK) && (ID < SVC_RAW_ENC_SHELL_NUM)) {
        UINT32 CapNum = 0U;
        UINT32 FeedNum = 0xFFFFFFFFU;
        UINT32 ReqVinID = 0U;

        if (ArgCount >= 3U) {
            RawEncTask_CmdStrToU32(pArgVector[2U], &CapNum);
        }
        if (ArgCount >= 4U) {
            RawEncTask_CmdStrToU32(pArgVector[3U], &FeedNum);
        }
        if (FeedNum == 0U) {
            FeedNum = CapNum;
        }
        if (ArgCount >= 4U) {
            RawEncTask_CmdStrToU32(pArgVector[4U], &ReqVinID);
        }
        RawEncTask_GenScript(pArgVector[1U], CapNum, FeedNum, ReqVinID);
    }

    return RetVal;
}

static void RawEncTask_ShellGenScriptU(UINT32 ID, UINT32 CtrlFlag)
{
    if (ID < SVC_RAW_ENC_SHELL_NUM) {
        const char *pCmdName = RawEncShellFunc[ID].CmdName;

        RawEncTask_ShellTitle(CtrlFlag, 1U, pCmdName,  "generate video raw encode script by current resolution config");
        if (CtrlFlag > 0U) {
            PRN_RENC_TSK_LOG "               [Script] : output script file path" PRN_RENC_TSK_API
            PRN_RENC_TSK_LOG "               [CapNum] : if cap_num = 0 or empty, generate script with raw/hds/aaa data" PRN_RENC_TSK_API
            PRN_RENC_TSK_LOG "              [FeedNum] : if feed_num is empty, it will be set as cap_num" PRN_RENC_TSK_API
            PRN_RENC_TSK_LOG "                          if feed_num is 0, generate some feeding example" PRN_RENC_TSK_API
            PRN_RENC_TSK_LOG "                [VinID] : Configure current VinID." PRN_RENC_TSK_API
            PRN_RENC_TSK_LOG "                          if user does not set it, using default value to handle it" PRN_RENC_TSK_API
            PRN_RENC_TSK_LOG "                          Using VinID(8) for cv5. Othereise use VinID(0)" PRN_RENC_TSK_API
            PRN_RENC_TSK_LOG " " PRN_RENC_TSK_API
        }
    }
}

static UINT32 RawEncTask_ShellStrmCap(UINT32 ID, UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal, ErrCode;

    RetVal = RawEncTask_ShellArgChk(ArgCount, pArgVector, 3U);
    if ((RetVal == SVC_OK) && (ID < SVC_RAW_ENC_SHELL_NUM)) {
        UINT32 VidStrmCapOn = 0xFFU;

        if (0 == SvcWrap_strcmp(pArgVector[1U], "cfg_rawenc")) {
            if (0 == SvcWrap_strcmp(pArgVector[2U], "on")) {
                RawEncTskCtrlFlg &= ~SVC_RAW_ENC_FLG_STRM_CAP_USER_CAP;
                RawEncTskCtrlFlg |= SVC_RAW_ENC_FLG_STRM_CAP_RAW_ENC | SVC_RAW_ENC_FLG_STRM_CAP_POST_TEXT;
                VidStrmCapOn = 2U;
            } else {
                RawEncTskCtrlFlg &= ~(SVC_RAW_ENC_FLG_STRM_CAP_RAW_ENC | SVC_RAW_ENC_FLG_STRM_CAP_POST_TEXT);
                VidStrmCapOn = 0U;
            }

            PRN_RENC_TSK_LOG "%s video strm cap for raw enc"
                PRN_RENC_TSK_ARG_CSTR (((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_STRM_CAP_RAW_ENC)>0U)?"Enable":"Disable") PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_API

        } else if (0 == SvcWrap_strcmp(pArgVector[1U], "cfg_split")) {
            if (0 == SvcWrap_strcmp(pArgVector[2U], "on")) {
                RawEncTskCtrlFlg |= SVC_RAW_ENC_FLG_STRM_CAP_SEPERATE;
            } else {
                RawEncTskCtrlFlg &= ~SVC_RAW_ENC_FLG_STRM_CAP_SEPERATE;
            }
        } else if (0 == SvcWrap_strcmp(pArgVector[1U], "cfg_user")) {
            if (0 == SvcWrap_strcmp(pArgVector[2U], "on")) {
                RawEncTskCtrlFlg &= ~(SVC_RAW_ENC_FLG_STRM_CAP_RAW_ENC | SVC_RAW_ENC_FLG_STRM_CAP_POST_TEXT);
                RawEncTskCtrlFlg |= SVC_RAW_ENC_FLG_STRM_CAP_USER_CAP;
                VidStrmCapOn = 1U;
            } else {
                RawEncTskCtrlFlg &= ~SVC_RAW_ENC_FLG_STRM_CAP_USER_CAP;
                VidStrmCapOn = 0U;
            }

            PRN_RENC_TSK_LOG "%s video strm cap for user write"
                PRN_RENC_TSK_ARG_CSTR (((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_STRM_CAP_USER_CAP)>0U)?"Enable":"Disable") PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_API
        } else {
            // misra-c
        }

        if (VidStrmCapOn != 0xFFU) {
            SVC_USER_PREF_s* pSvcUserPref = NULL;

            if (0U == SvcUserPref_Get(&pSvcUserPref)) {
                if (pSvcUserPref != NULL) {
                    ULONG  PrefBase = 0U;
                    UINT32 PrefSize = 0U;

                    pSvcUserPref->VidStrmCapOn = VidStrmCapOn;
                    SvcPref_GetPrefBuf(&PrefBase, &PrefSize);
                    ErrCode = SvcPref_Save(PrefBase, PrefSize); PRN_RENC_TSK_ERR_HDLR

                    PRN_RENC_TSK_LOG "Update pref to %s video stream capture"
                        PRN_RENC_TSK_ARG_CSTR ((pSvcUserPref->VidStrmCapOn > 0U)?"enable":"disable") PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_API
                }
            }
        }

    }

    return 0U;
}

static void RawEncTask_ShellStrmCapU(UINT32 ID, UINT32 CtrlFlag)
{
    if (ID < SVC_RAW_ENC_SHELL_NUM) {
        const char *pCmdName = RawEncShellFunc[ID].CmdName;

        RawEncTask_ShellTitle(CtrlFlag, 1U, pCmdName,  "configure video stream capture");
        if (CtrlFlag > 0U) {
            PRN_RENC_TSK_LOG "           [cfg_rawenc] : on/off: enable/disable capture raw enc data format" PRN_RENC_TSK_API
            PRN_RENC_TSK_LOG "         [cfg_seperate] : on/off: enable/disable split raw enc data sequence during module delete" PRN_RENC_TSK_API
            PRN_RENC_TSK_LOG "             [cfg_user] : on/off: enable/disable capture user callback" PRN_RENC_TSK_API

            PRN_RENC_TSK_LOG " " PRN_RENC_TSK_API
        }
    }
}

static UINT32 RawEncTask_ShellDbgMsg(UINT32 ID, UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal;

    RetVal = RawEncTask_ShellArgChk(ArgCount, pArgVector, 2U);
    if ((RetVal == SVC_OK) && (ID < SVC_RAW_ENC_SHELL_NUM)) {
        UINT32 DbgLvl = 0U;

        RawEncTask_CmdStrToU32(pArgVector[1U], &DbgLvl);
        if ((DbgLvl & 0x1UL) > 0U) {
            if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_MSG_DBG) == 0U) {
                RawEncTskCtrlFlg |= SVC_RAW_ENC_FLG_MSG_DBG;
                PRN_RENC_TSK_LOG "Enable normal debug message" PRN_RENC_TSK_OK
            } else {
                RawEncTskCtrlFlg &= ~SVC_RAW_ENC_FLG_MSG_DBG;
                PRN_RENC_TSK_LOG "Disable normal debug message" PRN_RENC_TSK_OK
            }
        }

        if ((DbgLvl & 0x2UL) > 0U) {
            if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_MSG_DBG1) == 0U) {
                RawEncTskCtrlFlg |= SVC_RAW_ENC_FLG_MSG_DBG1;
                PRN_RENC_TSK_LOG "Enable continue debug message" PRN_RENC_TSK_OK
            } else {
                RawEncTskCtrlFlg &= ~SVC_RAW_ENC_FLG_MSG_DBG1;
                PRN_RENC_TSK_LOG "Disable continue debug message" PRN_RENC_TSK_OK
            }
        }
    }

    return 0U;
}

static void RawEncTask_ShellDbgMsgU(UINT32 ID, UINT32 CtrlFlag)
{
    if (ID < SVC_RAW_ENC_SHELL_NUM) {
        const char *pCmdName = RawEncShellFunc[ID].CmdName;

        RawEncTask_ShellTitle(CtrlFlag, 1U, pCmdName,  "enable/disable debug message");
        if (CtrlFlag > 0U) {
            PRN_RENC_TSK_LOG "          [debug level] : 1 -> normal debug message" PRN_RENC_TSK_API
            PRN_RENC_TSK_LOG "                        : 2 -> continue debug message" PRN_RENC_TSK_API
            PRN_RENC_TSK_LOG " " PRN_RENC_TSK_API
        }
    }
}

static void RawEncTask_ShellEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal = SVC_NG;
    UINT32 CtrlFlag = 0U;

    if(ArgCount == 0xCafeU) {
        UINT32 SIdx;

        PRN_RENC_TSK_LOG " " PRN_RENC_TSK_API
        PRN_RENC_TSK_LOG "==== %sShell Arguments Setting%s (%s%d%s) ===="
            PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_TITLE_1 PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_END     PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_NUM     PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_UINT32 ArgCount                    PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_END     PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_API

        for (SIdx = 0U; SIdx < ArgCount; SIdx ++) {
            PRN_RENC_TSK_LOG "  pArgVector[%s%d%s] : %s%s%s"
                PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_NUM PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_ARG_UINT32 SIdx                    PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_END PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_STR PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_ARG_CSTR   pArgVector[SIdx]        PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_END PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_API
        }
    }

    if (ArgCount < 1U) {
        PRN_RENC_TSK_LOG "Fail to proc raw enc tsk shell cmd - invalid arg count!" PRN_RENC_TSK_NG
    } else if (pArgVector == NULL) {
        PRN_RENC_TSK_LOG "Fail to proc raw enc tsk shell cmd - invalid arg vector!" PRN_RENC_TSK_NG
    } else {
        UINT32 ShellIdx;
        UINT32 ShellCount = (UINT32)(sizeof(RawEncShellFunc)) / (UINT32)(sizeof(RawEncShellFunc[0]));

        if (0 == SvcWrap_strcmp(pArgVector[1U], "more")) {
            CtrlFlag = 1U;
        } else {
            for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
                if ((RawEncShellFunc[ShellIdx].pFunc != NULL) && (RawEncShellFunc[ShellIdx].Enable > 0U)) {
                    if (0 == SvcWrap_strcmp(pArgVector[1U], RawEncShellFunc[ShellIdx].CmdName)) {
                        if (SVC_OK != (RawEncShellFunc[ShellIdx].pFunc)(RawEncShellFunc[ShellIdx].Id, ArgCount - 1U, &pArgVector[1U])) {
                            if (RawEncShellFunc[ShellIdx].pUsage != NULL) {
                                (RawEncShellFunc[ShellIdx].pUsage)(RawEncShellFunc[ShellIdx].Id, 1U);
                            }
                        }
                        RetVal = SVC_OK;
                        break;
                    }
                }
            }
        }
    }

    if (RetVal != SVC_OK) {
        RawEncTask_ShellUsage(CtrlFlag);
        AmbaMisra_TouchUnused(&PrintFunc);
    }
}

static void RawEncTask_ShellUsage(UINT32 CtrlFlag)
{
    UINT32 ShellIdx;
    UINT32 ShellCount = (UINT32)(sizeof(RawEncShellFunc)) / (UINT32)(sizeof(RawEncShellFunc[0]));

    PRN_RENC_TSK_LOG " " PRN_RENC_TSK_API
    PRN_RENC_TSK_LOG "====== %sRaw Enc Task Command Usage%s ======"
        PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_TITLE_0 PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_END     PRN_RENC_TSK_ARG_POST
    PRN_RENC_TSK_API
    for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
        if ((RawEncShellFunc[ShellIdx].pFunc != NULL) && (RawEncShellFunc[ShellIdx].Enable > 0U)) {
            if (RawEncShellFunc[ShellIdx].pUsage == NULL) {
                PRN_RENC_TSK_LOG "  %s"
                    PRN_RENC_TSK_ARG_CSTR   RawEncShellFunc[ShellIdx].CmdName PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_API
            } else {
                (RawEncShellFunc[ShellIdx].pUsage)(RawEncShellFunc[ShellIdx].Id, CtrlFlag);
            }
        }
    }

    PRN_RENC_TSK_LOG " " PRN_RENC_TSK_API
}

static void RawEncTask_ShellInit(void)
{
    if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_SHELL_INIT) == 0U) {
        UINT32 ShellCnt = 0U;

        AmbaSvcWrap_MisraMemset(RawEncShellFunc, 0, sizeof(RawEncShellFunc));
        RawEncShellFunc[ShellCnt] = (SVC_RAW_ENC_TSK_SHELL_FUNC_s) { 1U, ShellCnt, "rec_start",  RawEncTask_ShellRecStart,  RawEncTask_ShellRecStartU  }; ShellCnt ++;
        RawEncShellFunc[ShellCnt] = (SVC_RAW_ENC_TSK_SHELL_FUNC_s) { 1U, ShellCnt, "rec_stop",   RawEncTask_ShellRecStop,   RawEncTask_ShellRecStopU   }; ShellCnt ++;
        RawEncShellFunc[ShellCnt] = (SVC_RAW_ENC_TSK_SHELL_FUNC_s) { 1U, ShellCnt, "gen_script", RawEncTask_ShellGenScript, RawEncTask_ShellGenScriptU }; ShellCnt ++;
        RawEncShellFunc[ShellCnt] = (SVC_RAW_ENC_TSK_SHELL_FUNC_s) { 1U, ShellCnt, "strm_cap",   RawEncTask_ShellStrmCap,   RawEncTask_ShellStrmCapU   }; ShellCnt ++;
        RawEncShellFunc[ShellCnt] = (SVC_RAW_ENC_TSK_SHELL_FUNC_s) { 1U, ShellCnt, "dbg_msg",    RawEncTask_ShellDbgMsg,    RawEncTask_ShellDbgMsgU    };

        RawEncTskCtrlFlg |= SVC_RAW_ENC_FLG_SHELL_INIT;
    }
}

static void RawEncTask_CmdInstall(void)
{
    AMBA_SHELL_COMMAND_s  SvcRawEncCmd;
    UINT32  RetVal;

    RawEncTask_ShellInit();

    SvcRawEncCmd.pName    = "svc_rawenc_task";
    SvcRawEncCmd.MainFunc = RawEncTask_ShellEntry;
    SvcRawEncCmd.pNext    = NULL;

    RetVal = SvcCmd_CommandRegister(&SvcRawEncCmd);
    if (SHELL_ERR_SUCCESS != RetVal) {
        PRN_RENC_TSK_LOG "## fail to install svc emr command" PRN_RENC_TSK_NG
    }
}

/**
 * Initial raw encode task
 *
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcRawEncTask_Init(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    AmbaSvcWrap_MisraMemset(&RawEncIqTblPath, 0, sizeof(RawEncIqTblPath));
    AmbaSvcWrap_MisraMemset(&RawEncCtrl, 0, sizeof(RawEncCtrl));
    SvcWrap_strcpy(RawEncCtrl.Name, sizeof(RawEncCtrl.Name), "SvcRawEnc");

    /* Create event flag */
    PRetVal = AmbaKAL_EventFlagCreate(&(RawEncCtrl.Flag), RawEncCtrl.Name);
    if (PRetVal != 0U) {
        RetVal = SVC_NG;
        PRN_RENC_TSK_LOG "Fail to init raw enc task - create event flag fail!" PRN_RENC_TSK_NG
    } else {
        PRetVal = AmbaKAL_EventFlagClear(&(RawEncCtrl.Flag), 0xFFFFFFFFU);
        if (PRetVal != 0U) {
            PRN_RENC_TSK_LOG "Fail to init raw enc task - clear flag! ErrCode(0x%08X)"
                PRN_RENC_TSK_ARG_UINT32 PRetVal PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_NG
        }
    }

    SvcRawEnc_CallBackRegister(RawEncTask_CallBackProc);
    RawEncTask_CmdInstall();
    SvcRawEnc_CmdInstall();

    if (RetVal == SVC_OK) {
        PRN_RENC_TSK_LOG "Successful to init raw enc task!" PRN_RENC_TSK_OK

        RawEncTskCtrlFlg |= SVC_RAW_ENC_FLG_INIT;
    }

    if (RetVal == SVC_OK) {
        SVC_USER_PREF_s* pSvcUserPref = NULL;

        if (0U == SvcUserPref_Get(&pSvcUserPref)) {
            if (pSvcUserPref != NULL) {

                if (pSvcUserPref->VidStrmCapOn == 1U) {
                    RawEncTskCtrlFlg |= SVC_RAW_ENC_FLG_STRM_CAP_USER_CAP;
                    PRN_RENC_TSK_LOG "Enable video stream capture user write" PRN_RENC_TSK_OK
                } else if (pSvcUserPref->VidStrmCapOn == 2U) {
                    RawEncTskCtrlFlg |= SVC_RAW_ENC_FLG_RAW_ENC_DEF;
                    PRN_RENC_TSK_LOG "Enable video stream capture for raw enc" PRN_RENC_TSK_OK
                } else {
                    // misra-c
                }
            }
        }
    }

    return RetVal;
}

/**
 * Raw encode iso config
 *
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcRawEncTask_IsoConfig(void)
{
    UINT32 RetVal = SVC_OK;
    ULONG  BufBase = 0U;
    UINT32 VinIDs[AMBA_DSP_MAX_VIN_NUM], VinNum = 0U;
    UINT32 VinSrc = 0U, BufSize = 0U;

    AmbaSvcWrap_MisraMemset(VinIDs, 255, sizeof(VinIDs));

    if (SVC_OK != SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_ITUNER, &BufBase, &BufSize)) {
        RetVal = SVC_NG;
        PRN_RENC_TSK_LOG "@@ SvcRawEncTask_IsoConfig failed. Get ituner buffer address fail" PRN_RENC_TSK_NG
    } else if (SVC_OK != SvcResCfg_GetVinIDs(VinIDs, &VinNum)) {
        RetVal = SVC_NG;
        PRN_RENC_TSK_LOG "@@ SvcRawEncTask_IsoConfig failed. Get VinID fail" PRN_RENC_TSK_NG
    } else if (VinNum == 0U) {
        RetVal = SVC_NG;
        PRN_RENC_TSK_LOG "@@ SvcRawEncTask_IsoConfig failed. Vin number should not zero" PRN_RENC_TSK_DBG
    } else if (VinIDs[0] >= AMBA_DSP_MAX_VIN_NUM) {
        RetVal = SVC_NG;
        PRN_RENC_TSK_LOG "@@ SvcRawEncTask_IsoConfig failed. invalid VinID(%d)"
            PRN_RENC_TSK_ARG_UINT32 VinIDs[0] PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_NG
    } else if (SVC_OK == SvcResCfg_GetVinSrc(VinIDs[0], &VinSrc)) {
        if (VinSrc == SVC_VIN_SRC_MEM) {
            UINT8 *pBuf;

            AmbaMisra_TypeCast(&(pBuf), &(BufBase));
            if (SVC_OK != SvcRawEnc_ItnMemCfg(pBuf, BufSize)) {
                RetVal = SVC_NG;
                PRN_RENC_TSK_LOG "@@ SvcRawEncTask_IsoConfig failed. Config ituner buffer" PRN_RENC_TSK_NG
            } else {
                UINT32 NumViewZone;
                UINT32 ViewZoneIDs[AMBA_DSP_MAX_VIEWZONE_NUM];
                AMBA_IK_MODE_CFG_s ImgMode[AMBA_DSP_MAX_VIEWZONE_NUM];

                PRN_RENC_TSK_LOG "@@ SvcRawEncTask_IsoConfig begin" PRN_RENC_TSK_API

                NumViewZone = 0U;
                AmbaSvcWrap_MisraMemset(ViewZoneIDs, 0, sizeof(ViewZoneIDs));
                AmbaSvcWrap_MisraMemset(ImgMode,     0, sizeof(ImgMode));

                RetVal = SvcResCfg_GetFovIdxs(ViewZoneIDs, &NumViewZone);
                if (SVC_OK != RetVal) {
                    PRN_RENC_TSK_LOG "## fail to get fov idx and fov num" PRN_RENC_TSK_NG
                } else {
                    UINT32 Idx;

                    for (Idx = 0U; Idx < NumViewZone; Idx ++) {
                        ImgMode[Idx].ContextId = ViewZoneIDs[Idx];
                    }

                    RetVal = SvcRawEnc_DefIdspCfg(NumViewZone, ViewZoneIDs, ImgMode);
                }

                if (SVC_OK != RetVal) {
                    PRN_RENC_TSK_LOG "@@ SvcRawEncTask_IsoConfig failed (%d)"
                        PRN_RENC_TSK_ARG_UINT32 RetVal PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_NG
                } else {
                    PRN_RENC_TSK_LOG "@@ SvcRawEncTask_IsoConfig end" PRN_RENC_TSK_OK
                }
            }
        }
    } else {
        // misra-c
    }

    return RetVal;
}

static void RawEncTask_fCmdPrn(AMBA_FS_FILE *pFile, char *pBuf, UINT32 BufSize, SVC_WRAP_SNPRINT_s *pFmt)
{
    if ((pFile != NULL) && (pBuf != NULL) && (pFmt != NULL) && (pFmt->pStrFmt != NULL)) {
        UINT32 RetStrLen;
        char *pChar = SvcWrap_strstr(pFmt->pStrFmt, "svc_rawenc");
        if (pChar != NULL) {
            UINT32 ArgCount = 0U;
            char *pArgVector[6U];
            char *pNext;

            AmbaSvcWrap_MisraMemset(pBuf, 0, BufSize);
            SvcWrap_strcpy(pBuf, BufSize, pChar);
            pChar = SvcWrap_strstr(pBuf, " 0x%");
            if (pChar != NULL) {
                *pChar = '\0';
            }
            pChar = SvcWrap_strstr(pBuf, " %");
            if (pChar != NULL) {
                *pChar = '\0';
            }

            AmbaUtility_StringAppend(pBuf, BufSize, " more end");

            RetStrLen = (UINT32)SvcWrap_strlen(pBuf);

            AmbaSvcWrap_MisraMemset(pArgVector, 0, sizeof(pArgVector));
            pChar = pBuf;
            do {
                pNext = SvcWrap_strstr(pChar, " ");
                if (pNext != NULL) {
                    *pNext = '\0';

                    if ((UINT32)SvcWrap_strlen(pChar) > 0U) {
                        if (ArgCount < 5U) {
                            pArgVector[ArgCount] = pChar;
                            ArgCount ++;
                        }
                    }

                    pChar = &(pNext[1]);
                    while ((*pChar == ' ') && (RetStrLen > 0U)) {
                        pChar ++; RetStrLen --;
                    }
                } else {
                    break;
                }

            } while (*pChar != '\0');

            if (ArgCount > 0U) {
                SvcRawEnc_DumpCmdUsage(pFile, ArgCount, pArgVector);
            }
        }

        AmbaSvcWrap_MisraMemset(pBuf, 0, BufSize);
        RetStrLen = SvcWrap_snprintf(pBuf, BufSize, pFmt);
        if (RetStrLen == 0U) {
            PRN_RENC_TSK_LOG "Fail to gen string - %s"
                PRN_RENC_TSK_ARG_CSTR pFmt->pStrFmt PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_NG
        } else {
            UINT32 StrLen = (UINT32)SvcWrap_strlen(pBuf);

            if (StrLen > 0U) {
                UINT32 NumSuccess, ErrCode;

                ErrCode = AmbaFS_FileWrite(pBuf, 1, StrLen, pFile, &NumSuccess);
                if (ErrCode != 0U) {
                    PRN_RENC_TSK_LOG "Fail to write string to file! ErrCode(0x%08X)"
                        PRN_RENC_TSK_ARG_UINT32 ErrCode PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_DBG
                } else {
                    if (NumSuccess != StrLen) {
                        PRN_RENC_TSK_LOG "Warning to write string to file! write size(0x%X) does not same with req size(0x%X)"
                            PRN_RENC_TSK_ARG_UINT32 NumSuccess PRN_RENC_TSK_ARG_POST
                            PRN_RENC_TSK_ARG_UINT32 StrLen     PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_DBG
                    }
                }
            }

        }
    }
}

static void RawEncTask_snprintf(char *pBuf, UINT32 BufSize, SVC_WRAP_SNPRINT_s *pFmt)
{
    UINT32 RetStrLen = SvcWrap_snprintf(pBuf, BufSize, pFmt);
    if ((RetStrLen == 0U) && (pFmt != NULL)) {
        PRN_RENC_TSK_LOG "Fail to gen string - %s"
        PRN_RENC_TSK_ARG_CSTR pFmt->pStrFmt PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_NG
    }
}

static void RawEncTask_fprintf(AMBA_FS_FILE *pFile, const char *pStr)
{
    if ((pFile != NULL) && (pStr != NULL)) {
        UINT32 StrLen = (UINT32)SvcWrap_strlen(pStr);

        if (StrLen > 0U) {
            UINT32 NumSuccess, ErrCode;
            void *pBuf;

            AmbaMisra_TypeCast(&(pBuf), &(pStr));
            ErrCode = AmbaFS_FileWrite(pBuf, 1, StrLen, pFile, &NumSuccess);
            if (ErrCode != 0U) {
                PRN_RENC_TSK_LOG "Fail to write string to file! ErrCode(0x%08X)"
                    PRN_RENC_TSK_ARG_UINT32 ErrCode PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_DBG
            } else {
                if (NumSuccess != StrLen) {
                    PRN_RENC_TSK_LOG "Warning to write string to file! write size(0x%X) does not same with req size(0x%X)"
                        PRN_RENC_TSK_ARG_UINT32 NumSuccess PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_ARG_UINT32 StrLen     PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_DBG
                }
            }
        }
    }
}

static void RawEncTask_strcat(char *pStr1, UINT32 Str1Size, const char *pStr2)
{
    if ((pStr1 != NULL) && (pStr2 != NULL)) {
        UINT32 Str1Len = (UINT32)SvcWrap_strlen(pStr1);
        UINT32 Str2Len = (UINT32)SvcWrap_strlen(pStr2);

        if (Str1Len < Str1Size) {
            UINT32 CpyCnt = Str1Size - Str1Len;
            char *pDst = &(pStr1[Str1Len]), *pSrc;

            AmbaMisra_TypeCast(&(pSrc), &(pStr2));

            if (CpyCnt > Str2Len) {
                CpyCnt = Str2Len;
            }

            while (CpyCnt > 0U) {
                *pDst = *pSrc;
                pDst ++;
                pSrc ++;
                CpyCnt --;
            }
            *pDst = '\0';

            AmbaMisra_TouchUnused(pSrc);
        }
    }
}

static void RawEncTask_GenScriptDirClear(const char *pScriptDir)
{
    if (pScriptDir != NULL) {
#ifndef CONFIG_BUILD_QNX_IPL
        UINT32 PRetVal;

        PRN_RENC_TSK_LOG "Clear folder files - %s"
            PRN_RENC_TSK_ARG_CSTR pScriptDir PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_DBG
        PRetVal = AmbaFS_ChangeDir(pScriptDir);
        if (0U != PRetVal) {
            if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_MSG_DBG) > 0U) {
                PRN_RENC_TSK_LOG "Fail to clear folder - change dir fail! ErrCode(0x%08X), %s"
                    PRN_RENC_TSK_ARG_UINT32 PRetVal    PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_ARG_CSTR   pScriptDir PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_NG
            }
        } else {
            PRN_RENC_TSK_LOG "  Change dir to %s"
                PRN_RENC_TSK_ARG_CSTR pScriptDir PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_DBG
            PRetVal = AmbaFS_CleanDir(pScriptDir);
            if (PRetVal != 0U) {
                PRN_RENC_TSK_LOG "Fail to clear folder - clean dir fail! ErrCode(0x%08X), %s"
                    PRN_RENC_TSK_ARG_UINT32 PRetVal    PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_ARG_CSTR   pScriptDir PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_NG
            } else {
                PRN_RENC_TSK_LOG "  Success to clean dir - %s"
                    PRN_RENC_TSK_ARG_CSTR pScriptDir PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_DBG
            }
        }

#endif
    }
}

static void RawEncTask_GenScriptDir(char *pScriptPath)
{
    UINT32 PRetVal = 0U;

    if (pScriptPath == NULL) {
        /* Do nothing */
    } else if (0U == (UINT32)SvcWrap_strlen(pScriptPath)) {
        /* Do nothing */
    } else {
        const char SubFolder[6][10] = {
              "raw"
            , "hds"
            , "aaa"
            , "ituner"
            , "iqtable"
            , "output"
        };
        UINT32 SubFolderNum = (UINT32)(sizeof(SubFolder)) / (UINT32)(sizeof(SubFolder[0]));
        char ScriptDir[128], *pChar, *pNext;
        UINT32 ScriptDirSize = (UINT32) sizeof(ScriptDir);
        UINT32 SwitchBackCnt = 0U, Idx;

        AmbaSvcWrap_MisraMemset(ScriptDir, 0, ScriptDirSize);
        SvcWrap_strcpy(ScriptDir, ScriptDirSize, pScriptPath);

        pChar = SvcWrap_strrchr(ScriptDir, 92); // ASICC 92 : '//'
        if (pChar != NULL) {
            pChar[1] = '\0';
        }

        pChar = SvcWrap_strstr(ScriptDir, ":\\");
        if ((pChar == NULL) || (pChar != &(ScriptDir[1]))) {
            PRN_RENC_TSK_LOG "Fail to create script folder - invalid script path. %s"
                PRN_RENC_TSK_ARG_CSTR ScriptDir PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_NG
        } else {

            PRN_RENC_TSK_LOG "------ %sCreate Script Folder%s ------"
                PRN_RENC_TSK_ARG_CSTR SVC_LOG_RENC_TSK_HL_TITLE_1 PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_ARG_CSTR SVC_LOG_RENC_TSK_HL_END     PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_API

            PRN_RENC_TSK_LOG "Create script folder - %s"
                PRN_RENC_TSK_ARG_CSTR ScriptDir PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_API

            pChar = &(ScriptDir[3U]);
            do {
                pNext = SvcWrap_strstr(pChar, "\\");
                if (pNext != NULL) {
                    *pNext = '\0';

                    PRN_RENC_TSK_LOG "  Create script folder - %s"
                        PRN_RENC_TSK_ARG_CSTR ScriptDir PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_DBG
                    PRetVal = AmbaFS_MakeDir(ScriptDir);
                    if (0x00110004U == PRetVal) {
                        PRetVal = 0U;
                        PRN_RENC_TSK_LOG "  Script folder is exist - %s"
                            PRN_RENC_TSK_ARG_CSTR   ScriptDir PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_DBG
                    } else if (0U != PRetVal) {
                        PRN_RENC_TSK_LOG "  Fail to create folder! ErrCode(0x%08X), %s"
                            PRN_RENC_TSK_ARG_UINT32 PRetVal   PRN_RENC_TSK_ARG_POST
                            PRN_RENC_TSK_ARG_CSTR   ScriptDir PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_DBG
                    } else {
                        // Misra-c
                    }

                    *pNext = '\\';
                    pChar = &(pNext[1]);
                    SwitchBackCnt ++;
                }
            } while (pNext != NULL);

            if (PRetVal == 0U) {

                pChar = &(ScriptDir[SvcWrap_strlen(ScriptDir)]);

                for (Idx = 0U; Idx < SubFolderNum; Idx ++) {
                    *pChar = '\0';
                    AmbaUtility_StringAppend(pChar, (UINT32)sizeof(ScriptDir) - (UINT32)SvcWrap_strlen(ScriptDir), SubFolder[Idx]);
                    PRetVal = AmbaFS_MakeDir(ScriptDir);
                    if (0x00110004U == PRetVal) {
                        PRetVal = 0U;
                        PRN_RENC_TSK_LOG "Script sub folder is exist - %s"
                            PRN_RENC_TSK_ARG_CSTR ScriptDir PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_API
                    } else if (PRetVal != 0U) {
                        PRN_RENC_TSK_LOG "Fail to create script folder - create sub folder fail! ErrCode(0x%08X), %s"
                            PRN_RENC_TSK_ARG_CSTR ScriptDir PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_NG
                    } else {
                        PRN_RENC_TSK_LOG "Success to create sub folder - %s"
                            PRN_RENC_TSK_ARG_CSTR ScriptDir PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_API
                    }
                }
            }
        }
    }

    AmbaMisra_TouchUnused(pScriptPath);
}

static void RawEncTask_GenScriptItn(char *pScriptPath)
{
    UINT32 ErrCode;
    UINT32 ItnMemSize = 0U;
    ULONG  ItnMemBase = 0U;

    if (pScriptPath == NULL) {
        PRN_RENC_TSK_LOG "Fail to dump ituner for script generation - invalid script path!" PRN_RENC_TSK_NG
    } else if (0U == (UINT32)SvcWrap_strlen(pScriptPath)) {
        PRN_RENC_TSK_LOG "Fail to dump ituner for script generation - script path length should not zero!" PRN_RENC_TSK_NG
    } else if (0U != SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_ITUNER, &ItnMemBase, &ItnMemSize)) {
        PRN_RENC_TSK_LOG "Fail to dump ituner for script generation - request buffer fail!" PRN_RENC_TSK_NG
    } else if ((ItnMemBase == 0U) || (ItnMemSize == 0U)) {
        PRN_RENC_TSK_LOG "Fail to dump ituner for script generation - invalid ituner buffer!" PRN_RENC_TSK_NG
    } else {
        void *pMemBuf;
        UINT32 FovNum = 0U, FovIdxs[AMBA_DSP_MAX_VIEWZONE_NUM], FovId;

        AmbaMisra_TypeCast(&(pMemBuf), &(ItnMemBase));

        AmbaSvcWrap_MisraMemset(FovIdxs, 0, sizeof(FovIdxs));

        if (0U != SvcResCfg_GetFovIdxs(FovIdxs, &FovNum)) {
            PRN_RENC_TSK_LOG "Fail to dump ituner for script generation - get fov info fail!" PRN_RENC_TSK_NG
        } else {
            char ScriptDir[512], *pChar;
            UINT32 ScriptDirSize = (UINT32) sizeof(ScriptDir);
            AMBA_IK_MODE_CFG_s ImgMode;
            AMBA_IK_WINDOW_SIZE_INFO_s ImgWinInfo;
            AMBA_ITN_SYSTEM_s ItnSysInfo;
            AMBA_ITN_Save_Param_s ItnSaveParam;
            SVC_WRAP_SNPRINT_s LocalSnPrint;

            PRN_RENC_TSK_LOG "------ %sDump Ituner%s ------"
                PRN_RENC_TSK_ARG_CSTR SVC_LOG_RENC_TSK_HL_TITLE_1 PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_ARG_CSTR SVC_LOG_RENC_TSK_HL_END     PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_API

            AmbaSvcWrap_MisraMemset(ScriptDir, 0, ScriptDirSize);
            SvcWrap_strcpy(ScriptDir, ScriptDirSize, pScriptPath);

            pChar = SvcWrap_strrchr(ScriptDir, 92); // ASICC 92 : '//'
            if (pChar != NULL) {
                *pChar = '\0';
            }
            RawEncTask_strcat(ScriptDir, ScriptDirSize, "\\ituner\\");
            ScriptDirSize = (ScriptDirSize - (UINT32)SvcWrap_strlen(ScriptDir)) - 1U;
            pChar = &(ScriptDir[SvcWrap_strlen(ScriptDir)]);

            /* Clear older ituner file before dump new ituner data */
            RawEncTask_GenScriptDirClear(ScriptDir);

            ErrCode = AmbaItn_Change_Parser_Mode(TEXT_TUNE); PRN_RENC_TSK_ERR_HDLR

            for (FovId = 0U; FovId < FovNum; FovId ++) {

                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "itn_%02d.txt";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)FovId; LocalSnPrint.Argc ++;
                RawEncTask_snprintf(pChar, ScriptDirSize, &LocalSnPrint);

                AmbaSvcWrap_MisraMemset(&ImgMode, 0, sizeof(ImgMode));
                ImgMode.ContextId = FovIdxs[FovId];

                ErrCode = AmbaItn_Init(pMemBuf, ItnMemSize);
                if (ErrCode != 0U) {
                    PRN_RENC_TSK_LOG "Fail to dump ituner for script generation - initial tuner module fail!" PRN_RENC_TSK_NG
                }

                // Update the ituner system info
                if (ErrCode == SVC_OK) {

                    AmbaSvcWrap_MisraMemset(&ItnSysInfo, 0, sizeof(ItnSysInfo));

                    // Get current system info
                    ErrCode = AmbaItn_Get_SystemInfo(&ItnSysInfo);
                    if (ErrCode != 0U) {
                        PRN_RENC_TSK_LOG "Fail to dump ituner for script generation - get current ituner system info fail!" PRN_RENC_TSK_NG
                    } else {
                        // Get image win info by IK context id
                        AmbaSvcWrap_MisraMemset(&ImgWinInfo, 0, sizeof(ImgWinInfo));

                        ErrCode = AmbaIK_GetWindowSizeInfo( &ImgMode, &ImgWinInfo );
                        if ( ErrCode != 0U ) {
                            PRN_RENC_TSK_LOG "Fail to dump ituner for script generation - get current image win info fail!" PRN_RENC_TSK_NG
                        } else {

                            // Update system raw info
                            ItnSysInfo.RawStartX           = 0;
                            ItnSysInfo.RawStartY           = 0;
                            ItnSysInfo.RawWidth            = ImgWinInfo.VinSensor.Width;
                            ItnSysInfo.RawHeight           = ImgWinInfo.VinSensor.Height;
                            ItnSysInfo.HSubSampleFactorNum = ImgWinInfo.VinSensor.HSubSample.FactorNum;
                            ItnSysInfo.HSubSampleFactorDen = ImgWinInfo.VinSensor.HSubSample.FactorDen;
                            ItnSysInfo.VSubSampleFactorNum = ImgWinInfo.VinSensor.VSubSample.FactorNum;
                            ItnSysInfo.VSubSampleFactorDen = ImgWinInfo.VinSensor.VSubSample.FactorDen;
                            ItnSysInfo.CompressedRaw       = 0U;
                        }
                    }

                    // Update the system info
                    if (ErrCode == SVC_OK) {
                        ErrCode = AmbaItn_Set_SystemInfo(&ItnSysInfo);
                        if (ErrCode != 0U) {
                            PRN_RENC_TSK_LOG "Fail to dump ituner for script generation - set ituner system info fail!" PRN_RENC_TSK_NG
                        }
                    }
                }

                // Save the ituner to file
                if (ErrCode == SVC_OK) {

                    AmbaSvcWrap_MisraMemset(&ItnSaveParam, 0, sizeof(ItnSaveParam));

                    ItnSaveParam.Text.Filepath = ScriptDir;

                    ErrCode = AmbaItn_Save_IDSP(&ImgMode, &ItnSaveParam);
                    if ( ErrCode != 0U ) {
                        PRN_RENC_TSK_LOG "Fail to dump ituner for script generation - save ituner fail!" PRN_RENC_TSK_NG
                    } else {

                        PRN_RENC_TSK_LOG "Success to dump ituner for script generate - %s"
                            PRN_RENC_TSK_ARG_CSTR   ScriptDir PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_API
                    }
                }

                if (ErrCode != SVC_OK) {
                    break;
                }
            }
        }
    }

    AmbaMisra_TouchUnused(pScriptPath);
}

static void RawEncTask_GenScriptRaw(UINT32 VinID, char *pScriptPath, UINT32 ReqFrameNum)
{
    UINT32 PRetVal = SVC_OK, ErrCode;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        PRN_RENC_TSK_LOG "Fail to dump raw/hds for script generation - invalid VinID(%d)!"
            PRN_RENC_TSK_ARG_UINT32 VinID PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_NG
    } else if (pScriptPath == NULL) {
        PRN_RENC_TSK_LOG "Fail to dump raw/hds for script generation - invalid script path!" PRN_RENC_TSK_NG
    } else {
        char ScriptDir[512], *pChar;
        UINT32 ScriptDirSize = (UINT32) sizeof(ScriptDir);
        UINT32 VinBits;

        AmbaSvcWrap_MisraMemset(ScriptDir, 0, ScriptDirSize);
        SvcWrap_strcpy(ScriptDir, ScriptDirSize, pScriptPath);

        pChar = SvcWrap_strrchr(ScriptDir, 92); // ASICC 92 : '//'
        if (pChar != NULL) {
            AMBA_SENSOR_CHANNEL_s     SsChan;
            AMBA_SENSOR_STATUS_INFO_s SsStatus;
            UINT32 RawCapCfg = SVC_RAW_CAP_CFG_RAW | SVC_RAW_CAP_CFG_NON_BMP;
            UINT32 CapNum = 1U;

            *pChar = '\0';

            PRN_RENC_TSK_LOG "------ %sCapture Video Raw Data%s ------"
                PRN_RENC_TSK_ARG_CSTR SVC_LOG_RENC_TSK_HL_TITLE_1 PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_ARG_CSTR SVC_LOG_RENC_TSK_HL_END     PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_API

            AmbaSvcWrap_MisraMemset(&SsChan, 0, sizeof(SsChan));
            SsChan.VinID = VinID;
            AmbaSvcWrap_MisraMemset(&SsStatus, 0, sizeof(SsStatus));
            ErrCode = AmbaSensor_GetStatus(&SsChan, &SsStatus); PRN_RENC_TSK_ERR_HDLR
            //if (SsStatus.ModeInfo.HdrInfo.ActiveChannels > 1U) {
            if ((pResCfg->FovCfg[0].PipeCfg.LinearCE > 0U)
             || (pResCfg->FovCfg[0].PipeCfg.HdrExposureNum > 1U)) {
                RawCapCfg |= SVC_RAW_CAP_CFG_3A;
                RawCapCfg |= SVC_RAW_CAP_CFG_HDS;
            }

            // Configure raw capture
            VinBits = SvcRawEnc_BitGet(VinID);
            PRetVal  = SvcRawCap_CfgCap(VinBits, RawCapCfg);
            // Configure vin type
            PRetVal |= SvcRawCap_CfgVinType(VinID, SVC_RAW_CAP_VIN_TYPE_CFA);
            // Configure raw path
            RawEncTask_strcat(ScriptDir, ScriptDirSize, "\\raw\\");
            /* Clear older raw file before dump new raw data */
            RawEncTask_GenScriptDirClear(ScriptDir);
            RawEncTask_strcat(ScriptDir, ScriptDirSize, "raw");
            PRetVal |= SvcRawCap_CfgRaw(VinID, ScriptDir);
            *pChar = '\0';
            // Configure hds path
            if ((RawCapCfg & SVC_RAW_CAP_CFG_HDS) > 0U) {
                RawEncTask_strcat(ScriptDir, ScriptDirSize, "\\hds\\hds");
                RawEncTask_GenScriptDirClear(ScriptDir);
                RawEncTask_strcat(ScriptDir, ScriptDirSize, "hds");
                PRetVal |= SvcRawCap_CfgHds(VinID, ScriptDir);
                *pChar = '\0';
            }
            // Configure aaa path
            if ((RawCapCfg & SVC_RAW_CAP_CFG_3A) > 0U) {
                UINT32 Idx, ImgIdx;
                UINT32 SsIdx[AMBA_DSP_MAX_VIEWZONE_NUM], SsIdxNum = 0U;
                UINT32 ImgAlgoId[AMBA_DSP_MAX_VIEWZONE_NUM], ImgAlgoNum = 0U;
                UINT32 ImgAlgoIdBits = 0U;

                AmbaSvcWrap_MisraMemset(SsIdx, 0, sizeof(SsIdx));
                if (0U == SvcResCfg_GetSensorIdxsInVinID(VinID, SsIdx, &SsIdxNum)) {

                    for (Idx = 0U; Idx < SsIdxNum; Idx ++) {
                        ImgAlgoNum = 0U;
                        AmbaSvcWrap_MisraMemset(ImgAlgoId, 0, sizeof(ImgAlgoId));
                        ErrCode = SvcImg_AlgoIdGetEx(VinID, SsIdx[Idx], ImgAlgoId, &ImgAlgoNum); PRN_RENC_TSK_ERR_HDLR
                        for (ImgIdx = 0U; ImgIdx < ImgAlgoNum; ImgIdx ++) {
                            if (ImgAlgoId[ImgIdx] < AMBA_DSP_MAX_VIEWZONE_NUM) {
                                ImgAlgoIdBits |= SvcRawEnc_BitGet(ImgAlgoId[ImgIdx]);
                            }
                        }
                    }

                    RawEncTask_strcat(ScriptDir, ScriptDirSize, "\\aaa\\");
                    RawEncTask_GenScriptDirClear(ScriptDir);
                    RawEncTask_strcat(ScriptDir, ScriptDirSize, "aaa");
                    PRetVal |= SvcRawCap_Cfg3A(VinID, ImgAlgoIdBits, ScriptDir);
                    *pChar = '\0';
                }
            }

            // Hook free cache/non-cache memory
            if (PRetVal == 0U) {
                UINT32 MemSize = 0U, TotalMemSize = 0U;
                ULONG  MemBase = 0U;
                UINT8 *pMemBuf;

#if defined(CONFIG_ICAM_TRANSIENT_DRAM_SIZE) && defined(FMEM_ID_TRANSIENT_BUF)
                ErrCode = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_TRANSIENT_BUF, &MemBase, &MemSize); PRN_RENC_TSK_ERR_HDLR
                if (MemSize > 0U) {
                    TotalMemSize += MemSize;
                    AmbaMisra_TypeCast(&(pMemBuf), &(MemBase));
                    if (0U == SvcRawCap_CfgMem(pMemBuf, MemSize)) {
                        PRN_RENC_TSK_LOG "Hook transient memory %p 0x%X"
                            PRN_RENC_TSK_ARG_CPOINT pMemBuf PRN_RENC_TSK_ARG_POST
                            PRN_RENC_TSK_ARG_UINT32 MemSize PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_DBG
                    } else {
                        PRN_RENC_TSK_LOG "Transient memory has been hook %p 0x%X"
                            PRN_RENC_TSK_ARG_CPOINT pMemBuf PRN_RENC_TSK_ARG_POST
                            PRN_RENC_TSK_ARG_UINT32 MemSize PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_DBG
                    }
                }
#endif

#ifndef CONFIG_BUILD_QNX_IPL
                ErrCode = SvcBuffer_LockFreeSpace(SVC_MEM_TYPE_CA, &MemBase, &MemSize); PRN_RENC_TSK_ERR_HDLR
                if (MemSize > 0U) {
                    AmbaMisra_TypeCast(&(pMemBuf), &(MemBase));
                    if (0U == SvcRawCap_CfgMem(pMemBuf, MemSize)) {
                        TotalMemSize += MemSize;

                        PRN_RENC_TSK_LOG "Hook free cache memory %p 0x%X"
                            PRN_RENC_TSK_ARG_CPOINT pMemBuf PRN_RENC_TSK_ARG_POST
                            PRN_RENC_TSK_ARG_UINT32 MemSize PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_DBG
                    }
                }
                ErrCode = SvcBuffer_UnLockFreeSpace(SVC_MEM_TYPE_CA); PRN_RENC_TSK_ERR_HDLR

                if (SsStatus.ModeInfo.HdrInfo.HdrType != AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
                    ErrCode = SvcBuffer_LockFreeSpace(SVC_MEM_TYPE_NC, &MemBase, &MemSize); PRN_RENC_TSK_ERR_HDLR
                    if (MemSize > 0U) {
                        AmbaMisra_TypeCast(&(pMemBuf), &(MemBase));
                        if (0U == SvcRawCap_CfgMem(pMemBuf, MemSize)) {
                            TotalMemSize += MemSize;

                            PRN_RENC_TSK_LOG "Hook free non-cache memory %p 0x%X"
                                PRN_RENC_TSK_ARG_CPOINT pMemBuf PRN_RENC_TSK_ARG_POST
                                PRN_RENC_TSK_ARG_UINT32 MemSize PRN_RENC_TSK_ARG_POST
                            PRN_RENC_TSK_DBG
                        }
                    }
                    ErrCode = SvcBuffer_UnLockFreeSpace(SVC_MEM_TYPE_NC); PRN_RENC_TSK_ERR_HDLR
                } else {
                    PRN_RENC_TSK_LOG "Keep free non-cache memory for unpack sensor hdr raw proc" PRN_RENC_TSK_DBG
                }
#else
                ErrCode = SvcBuffer_LockFreeSpace(SVC_MEM_TYPE_DSP, &MemBase, &MemSize); PRN_RENC_TSK_ERR_HDLR
                if (MemSize > 0U) {
                    AmbaMisra_TypeCast(&(pMemBuf), &(MemBase));
                    if (0U == SvcRawCap_CfgMem(pMemBuf, MemSize)) {
                        TotalMemSize += MemSize;

                        PRN_RENC_TSK_LOG "Hook free dsp pool memory %p 0x%X"
                            PRN_RENC_TSK_ARG_CPOINT pMemBuf PRN_RENC_TSK_ARG_POST
                            PRN_RENC_TSK_ARG_UINT32 MemSize PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_DBG
                    }
                }
                ErrCode = SvcBuffer_UnLockFreeSpace(SVC_MEM_TYPE_CA); PRN_RENC_TSK_ERR_HDLR
#endif

                SvcRawCap_MemQry(1U, &MemSize);
                if (MemSize > 0U) {
                    CapNum = TotalMemSize / MemSize;
                    if (CapNum > 0U) {
                        CapNum --;
                    }

                    if (CapNum > ReqFrameNum) {
                        CapNum = ReqFrameNum;
                    }
                }

                PRN_RENC_TSK_LOG "max capture frames %d"
                    PRN_RENC_TSK_ARG_UINT32 CapNum PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_API
            }

            // Capture raw/hds/aaa data
            if (PRetVal == 0U) {
                PRetVal = SvcRawCap_CapSeq(CapNum, 1U);
                if (PRetVal != 0U) {
                    PRN_RENC_TSK_LOG "Fail to dump raw/hds for script generation - capture VinID(%d) data fail!"
                        PRN_RENC_TSK_ARG_UINT32 VinID PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_DBG
                } else {
                    ErrCode = SvcRawCap_Dump(); PRN_RENC_TSK_ERR_HDLR
                }
            }
        }
    }

    AmbaMisra_TouchUnused(&VinID);

    AmbaMisra_TouchUnused(pScriptPath);
}

static void RawEncTask_GenScript(char *pScriptPath, UINT32 CapFrameNum, UINT32 FeedFrameNum, UINT32 ReqVinID)
{
    UINT32 ErrCode;
    UINT32 VinID = ReqVinID;
    const SVC_RES_CFG_s* pResCfg = SvcResCfg_Get();
    UINT32 FovIDs[AMBA_DSP_MAX_VIEWZONE_NUM], FovNum = 0U;

    AmbaSvcWrap_MisraMemset(FovIDs, 0, sizeof(FovIDs));

    if (pResCfg == NULL) {
        PRN_RENC_TSK_LOG "Fail to generate raw encode script - invalid resolution config!" PRN_RENC_TSK_NG
    } else if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        PRN_RENC_TSK_LOG "Fail to generate raw encode script - invalid VinID(%d)!"
            PRN_RENC_TSK_ARG_UINT32 VinID PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_NG
    } else if ((pResCfg->VinNum > 1U) || (pResCfg->VinCfg[VinID].CapWin.Width == 0U)) {
        PRN_RENC_TSK_LOG "Fail to generate raw encode script - vin%d only"
            PRN_RENC_TSK_ARG_UINT32 VinID PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_NG
    } else if (pResCfg->FovNum > 1U) {
        PRN_RENC_TSK_LOG "Fail to generate raw encode script - single fov only" PRN_RENC_TSK_NG
    } else if (0U != SvcResCfg_GetFovIdxsInVinID(VinID, FovIDs, &FovNum)) {
        PRN_RENC_TSK_LOG "Fail to generate raw encode script - get VinID(%d) fov info fail"
            PRN_RENC_TSK_ARG_UINT32 VinID PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_NG
    } else {
        UINT32 Idx;
        AMBA_IK_MODE_CFG_s ImgMode;
        AMBA_IK_VIN_SENSOR_INFO_s ImgSenInfo;
        AMBA_FS_FILE *pFile = NULL;

        ImgMode.ContextId = FovIDs[0];

        AmbaSvcWrap_MisraMemset(&ImgSenInfo, 0, sizeof(ImgSenInfo));
        ErrCode = AmbaIK_GetVinSensorInfo(&ImgMode, &ImgSenInfo);
        if (ErrCode != 0U) {
            PRN_RENC_TSK_ERR_HDLR
        } else if ((ImgSenInfo.Compression > 0U) && (ImgSenInfo.Compression < 255U)) {
            ErrCode = 1U;
            PRN_RENC_TSK_LOG "Fail to generate raw encode script - Only support un-compression type. CurCmprType(%d)"
                PRN_RENC_TSK_ARG_UINT32 ImgSenInfo.Compression PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_NG
        } else if (ImgSenInfo.Compression >= 255U) {
            PRN_RENC_TSK_LOG "Detect current pipeline is not pack mode. Please manually check ituner and raw data. CurCmprType(%d)"
                PRN_RENC_TSK_ARG_UINT32 ImgSenInfo.Compression PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_API
        } else {
            // misra-c
        }

        if (ErrCode == 0U) {
            PRN_RENC_TSK_LOG "==== %sVideo Raw Enc Script Generation%s ===="
                PRN_RENC_TSK_ARG_CSTR SVC_LOG_RENC_TSK_HL_TITLE_0 PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_ARG_CSTR SVC_LOG_RENC_TSK_HL_END     PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_API

            RawEncTask_GenScriptDir(pScriptPath);
            if ((CapFrameNum > 0U) && (CapFrameNum != 0xFFFFFFFFU)) {
                RawEncTask_GenScriptItn(pScriptPath);
                RawEncTask_GenScriptRaw(VinID, pScriptPath, CapFrameNum);
            }

            if (0U != AmbaFS_FileOpen(pScriptPath, "w", &pFile)) {
                PRN_RENC_TSK_LOG "Fail to generate raw encode script - create script file fail!" PRN_RENC_TSK_NG
            } else {
                char ScriptPrefix[64], *pChar;
                char StrBuf[1024];
                UINT32 StrBufSize = (UINT32)sizeof(StrBuf);
                SVC_WRAP_SNPRINT_s LocalSnPrint;

                PRN_RENC_TSK_LOG "------ %sGenerate Script%s ------"
                    PRN_RENC_TSK_ARG_CSTR SVC_LOG_RENC_TSK_HL_TITLE_1 PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_ARG_CSTR SVC_LOG_RENC_TSK_HL_END     PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_API

                AmbaSvcWrap_MisraMemset(ScriptPrefix, 0, sizeof(ScriptPrefix));
                pChar = SvcWrap_strrchr(pScriptPath, 92);
                if (pChar != NULL) {
                    *pChar = '\0';
                    SvcWrap_strcpy(ScriptPrefix, sizeof(ScriptPrefix), pScriptPath);
                    *pChar = '\\';
                }

                RawEncTask_fprintf(pFile, "\n; -----------------------------------------------------"
                                          "\n;                  TERA TERM GLOBAL SETTING            "
                                          "\n; -----------------------------------------------------"
                                          "\ntimeout = 30"
                                          "\n");

                RawEncTask_fprintf(pFile, "\n; -----------------------------------------------------"
                                          "\n;                    SVC Video Raw Encode"
                                          "\n; -----------------------------------------------------"
                                          "\n");

                RawEncTask_fprintf(pFile, "\n; -----------------------------------------------------"
                                          "\n;                      SCRIPT PARAMETER"
                                          "\n; -----------------------------------------------------"
                                          "\nSvcRawEnc_RecorderOn = 0"
                                          "\nSvcRawEnc_UpdIqTblOn = 0"
                                          "\nSvcRawEnc_AaaOn      = 0"
                                          "\n");

                RawEncTask_fprintf(pFile, "\n; -----------------------------------------------------"
                                          "\n;                        SCRIPT BEGIN"
                                          "\n; -----------------------------------------------------"
                                          "\n");

                RawEncTask_fprintf(pFile, "\n; ---- Re-boot to standby mode");
                RawEncTask_fprintf(pFile, "\nsprintf 'svc_app standby'"
                                          "\nsprintf2 waitstr 'ControlTask_CmdMode() done'"
                                          "\ncall _SUB_FUNC_CMD_PROC"
                                          "\npause 5");

                RawEncTask_fprintf(pFile, "\n\n; -----------------------------------------------------");
                RawEncTask_fprintf(pFile,   "\n;                   RESOLUTION CONFIG BEGIN            ");
                RawEncTask_fprintf(pFile,   "\n; -----------------------------------------------------");
                RawEncTask_GenScriptResCfg(pFile, StrBuf, StrBufSize, pResCfg, VinID);
                AmbaSvcWrap_MisraMemset(StrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                LocalSnPrint.pStrFmt = "\nsprintf 'svc_rawenc res_cfg info %s'";
                LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = " ";  LocalSnPrint.Argc ++;
                RawEncTask_fCmdPrn(pFile, StrBuf, StrBufSize, &LocalSnPrint);
                RawEncTask_fprintf(pFile, "\nsprintf2 waitstr 'Raw Encode Resolution Configuration End'"
                                          "\ncall _SUB_FUNC_CMD_PROC"
                                          "\nmpause 300");
                RawEncTask_fprintf(pFile, "\n\n; -----------------------------------------------------");
                RawEncTask_fprintf(pFile,   "\n;                   RESOLUTION CONFIG END              ");
                RawEncTask_fprintf(pFile,   "\n; -----------------------------------------------------");

                RawEncTask_fprintf(pFile, "\n\n; ---- Create video raw encode module");
                AmbaSvcWrap_MisraMemset(StrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "\nsprintf 'svc_rawenc create %s'";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = " "; LocalSnPrint.Argc ++;
                RawEncTask_fCmdPrn(pFile, StrBuf, StrBufSize, &LocalSnPrint);
                RawEncTask_fprintf(pFile, "\nsprintf2 waitstr 'Successful to create raw encode'"
                                          "\ncall _SUB_FUNC_CMD_PROC"
                                          "\npause 1");

                RawEncTask_fprintf(pFile, "\n\n; -----------------------------------------------------");
                RawEncTask_fprintf(pFile,   "\n;                     DUMMY SENSOR BEGIN               ");
                RawEncTask_fprintf(pFile,   "\n; -----------------------------------------------------");
                RawEncTask_GenScriptDmySen(pFile, StrBuf, StrBufSize, VinID);
                AmbaSvcWrap_MisraMemset(StrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                LocalSnPrint.pStrFmt = "\nsprintf 'svc_rawenc dmy_sen info %s'";
                LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = " ";  LocalSnPrint.Argc ++;
                RawEncTask_fCmdPrn(pFile, StrBuf, StrBufSize, &LocalSnPrint);
                RawEncTask_fprintf(pFile, "\nsprintf2 waitstr '[2] = {'"
                                          "\ncall _SUB_FUNC_CMD_PROC"
                                          "\nmpause 300");
                RawEncTask_fprintf(pFile, "\n\n; -----------------------------------------------------");
                RawEncTask_fprintf(pFile,   "\n;                     DUMMY SENSOR END                 ");
                RawEncTask_fprintf(pFile,   "\n; -----------------------------------------------------");

                RawEncTask_fprintf(pFile, "\n\n; -----------------------------------------------------");
                RawEncTask_fprintf(pFile,   "\n;                      IQ TABLE BEGIN                  ");
                RawEncTask_fprintf(pFile,   "\n; -----------------------------------------------------");
                RawEncTask_GenScriptIqTable(pFile, StrBuf, StrBufSize, ScriptPrefix);
                RawEncTask_fprintf(pFile, "\n\n; -----------------------------------------------------");
                RawEncTask_fprintf(pFile,   "\n;                      IQ TABLE END                    ");
                RawEncTask_fprintf(pFile,   "\n; -----------------------------------------------------");

                RawEncTask_fprintf(pFile, "\n\n; Update video raw encode resolution configuration");
                AmbaSvcWrap_MisraMemset(StrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                LocalSnPrint.pStrFmt = "\nsprintf 'svc_rawenc cfg src %d 0'";
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)VinID; LocalSnPrint.Argc ++;
                RawEncTask_fCmdPrn(pFile, StrBuf, StrBufSize, &LocalSnPrint);
                RawEncTask_fprintf(pFile, "\nsprintf2 waitstr ''"
                                          "\ncall _SUB_FUNC_CMD_PROC"
                                          "\nmpause 100");

                RawEncTask_fprintf(pFile, "\n\n; ---- Configure video raw encode default ituner path");
                for (Idx = 0U; Idx < pResCfg->FovNum; Idx ++) {
                    AmbaSvcWrap_MisraMemset(StrBuf, 0, StrBufSize);
                    AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                        LocalSnPrint.pStrFmt = "\nsprintf 'svc_rawenc cfg itn %d %s\\ituner\\itn_%02d.txt'";
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(Idx); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr  = ScriptPrefix;  LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(Idx); LocalSnPrint.Argc ++;
                    if (Idx == 0U) {
                        RawEncTask_fCmdPrn(pFile, StrBuf, StrBufSize, &LocalSnPrint);
                    } else {
                        RawEncTask_snprintf(StrBuf, StrBufSize, &LocalSnPrint);
                        RawEncTask_fprintf(pFile, StrBuf);
                    }
                    RawEncTask_fprintf(pFile, "\nsprintf2 waitstr ''"
                                              "\ncall _SUB_FUNC_CMD_PROC"
                                              "\npause 1");
                }

                RawEncTask_fprintf(pFile, "\n\n; Boot to video raw encode");
                RawEncTask_fprintf(pFile, "\nsprintf 'svc_flow liveview'"
                                          "\nsprintf2 waitstr 'SRC_BOOT_DONE Check'"
                                          "\ncall _SUB_FUNC_CMD_PROC"
                                          "\npause 2");

                RawEncTask_fprintf(pFile, "\n\n; Configure video raw buffer setting");
                AmbaSvcWrap_MisraMemset(StrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc cfg mem %d %d'";
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = 0U;  LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = 0U;  LocalSnPrint.Argc ++;
                RawEncTask_fCmdPrn(pFile, StrBuf, StrBufSize, &LocalSnPrint);
                AmbaSvcWrap_MisraMemset(StrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc cfg bufctrl_info %s'";
                LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = " ";  LocalSnPrint.Argc ++;
                RawEncTask_fCmdPrn(pFile, StrBuf, StrBufSize, &LocalSnPrint);
                RawEncTask_fprintf(pFile, "\npause 1");

                RawEncTask_fprintf(pFile, "\n\n; Configure AAA/Image Framework for video raw enc");
                RawEncTask_fprintf(pFile, "\n\nif SvcRawEnc_AaaOn=1 then");
                RawEncTask_fprintf(pFile, "\n  ; Enable manually AE"
                                          "\n  sendln 'imgproc -ae off'"
                                          "\n  pause 1");
                RawEncTask_fprintf(pFile, "\nelse");
                RawEncTask_fprintf(pFile, "\n  sendln 'imgproc -3a off'"
                                          "\n  pause 1");
                RawEncTask_fprintf(pFile, "\n  sendln 'imgfrw load'"
                                          "\n  pause 1");
                RawEncTask_fprintf(pFile, "\n  sendln 'imgfrw app load'"
                                          "\n  pause 1");
                RawEncTask_fprintf(pFile, "\n  sendln 'imgfrw app stat_mask 0'"
                                          "\n  pause 1");
                RawEncTask_fprintf(pFile, "\nendif");

                RawEncTask_fprintf(pFile, "\n\n; Start video record after boot done");
                RawEncTask_fprintf(pFile, "\nif SvcRawEnc_RecorderOn=1 then");
                RawEncTask_fprintf(pFile, "\n  sprintf 'svc_rawenc_task rec_start'"
                                          "\n  sprintf2 waitstr ''"
                                          "\n  call _SUB_FUNC_CMD_PROC");
                RawEncTask_fprintf(pFile, "\nendif");

                RawEncTask_GenScriptFeed(pFile, StrBuf, StrBufSize, ScriptPrefix, CapFrameNum, FeedFrameNum, VinID);

                RawEncTask_fprintf(pFile, "\n\n; Stop video record");
                RawEncTask_fprintf(pFile, "\nif SvcRawEnc_RecorderOn=1 then");
                RawEncTask_fprintf(pFile, "\n  sprintf 'svc_rawenc_task rec_stop'"
                                          "\n  sprintf2 waitstr ''"
                                          "\n  call _SUB_FUNC_CMD_PROC"
                                          "\n  "
                                          "\n  for feed_dummy_frame_idx 1 3"
                                          "\n      sprintf 'svc_rawenc execute'"
                                          "\n      sprintf2 waitstr 'Successful to execute raw enc'"
                                          "\n      call _SUB_FUNC_CMD_PROC"
                                          "\n      mpause 50"
                                          "\n  next"
                                          "\n  ");
                RawEncTask_fprintf(pFile, "\nendif");

                RawEncTask_fprintf(pFile, "\n"
                                          "\n; -----------------------------------------------------"
                                          "\n;                        SCRIPT END"
                                          "\n; -----------------------------------------------------"
                                          "\nexit"
                                          "\n"
                                          "\n; -----------------------------------------------------"
                                          "\n;                        SUB_FUNCTION BEGIN"
                                          "\n; -----------------------------------------------------"
                                          "\n"
                                          "\n:_SUB_FUNC_CMD_PROC"
                                          "\n    strlen inputstr"
                                          "\n    "
                                          "\n    if result>0 then"
                                          "\n        sendln ''"
                                          "\n        sendln ''"
                                          "\n        sendln inputstr"
                                          "\n        "
                                          "\n        strlen waitstr"
                                          "\n        if result>0 then"
                                          "\n            wait waitstr"
                                          "\n            if result=0 goto _ERROR_process_cmd_fail"
                                          "\n            waitstr = ''"
                                          "\n        endif"
                                          "\n    endif"
                                          "\n    "
                                          "\nreturn"
                                          "\n:_ERROR_process_cmd_fail"
                                          "\n"
                                          "\n    sendln 'svc_rawenc cfg info'"
                                          "\n    pause 1"
                                          "\n    sendln 'svc_rawenc cfg bufctrl_info'"
                                          "\n    pause 1    "
                                          "\n"
                                          "\n    sprintf 'Fail to process command - %s' inputstr"
                                          "\n    "
                                          "\n    dispstr inputstr"
                                          "\nexit"
                                          "\n"
                                          "\n; -----------------------------------------------------"
                                          "\n;                        SUB_FUNCTION END"
                                          "\n; -----------------------------------------------------"
                                          "\n");

                if (0U != AmbaFS_FileClose(pFile)) {
                    PRN_RENC_TSK_LOG "Fail to generate raw encode script - close file fail!" PRN_RENC_TSK_NG
                }

                PRN_RENC_TSK_LOG "Success to create script - %s"
                    PRN_RENC_TSK_ARG_CSTR   pScriptPath PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_API
            }
        }
    }
}

static void RawEncTask_GenScriptDmySen(AMBA_FS_FILE *pFile, char *pStrBuf, UINT32 StrBufSize, UINT32 VinID)
{
    if ((pFile != NULL) && (pStrBuf != NULL) && (VinID < AMBA_DSP_MAX_VIN_NUM)) {
        UINT32 ErrCode;
        UINT32 Idx;
        AMBA_SENSOR_CHANNEL_s     SsChan;
        AMBA_SENSOR_STATUS_INFO_s SsStatus;
        SVC_WRAP_SNPRINT_s LocalSnPrint;

        PRN_RENC_TSK_LOG "------ %sGenerate Script - Dummy Sensor%s ------"
            PRN_RENC_TSK_ARG_CSTR SVC_LOG_RENC_TSK_HL_TITLE_2 PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_CSTR SVC_LOG_RENC_TSK_HL_END     PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_API

        AmbaSvcWrap_MisraMemset(&SsChan, 0, sizeof(SsChan));
        SsChan.VinID = VinID;

        AmbaSvcWrap_MisraMemset(&SsStatus, 0, sizeof(SsStatus));
        ErrCode = AmbaSensor_GetStatus(&SsChan, &SsStatus); PRN_RENC_TSK_ERR_HDLR

        AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
        AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
            LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc dmy_sen output %d %d %d %d %d %d %d %d %d %d %d %d %d %d'\nmpause 100";
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.OutputInfo.DataRate); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.OutputInfo.NumDataLanes); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.OutputInfo.NumDataBits); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.OutputInfo.BayerPattern); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.OutputInfo.OutputWidth); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.OutputInfo.OutputHeight); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.OutputInfo.RecordingPixels.StartX); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.OutputInfo.RecordingPixels.StartY); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.OutputInfo.RecordingPixels.Width); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.OutputInfo.RecordingPixels.Height); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.OutputInfo.OpticalBlackPixels.StartX); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.OutputInfo.OpticalBlackPixels.StartY); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.OutputInfo.OpticalBlackPixels.Width); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.OutputInfo.OpticalBlackPixels.Height); LocalSnPrint.Argc ++;
        RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);

        AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
        AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
            LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc dmy_sen input %d %d %d %d %d %d %d %d %d %d %d'\nmpause 100";
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.InputInfo.PhotodiodeArray.StartX); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.InputInfo.PhotodiodeArray.StartY); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.InputInfo.PhotodiodeArray.Width); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.InputInfo.PhotodiodeArray.Height); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.InputInfo.HSubsample.SubsampleType); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.InputInfo.HSubsample.FactorNum); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.InputInfo.HSubsample.FactorDen); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.InputInfo.VSubsample.SubsampleType); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.InputInfo.VSubsample.FactorNum); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.InputInfo.VSubsample.FactorDen); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.InputInfo.SummingFactor); LocalSnPrint.Argc ++;
        RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);

        AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
        AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
            LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc dmy_sen mode_info %d %d %d %d %d %d'\nmpause 100";
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.InputClk); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.LineLengthPck); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.FrameLengthLines); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.FrameRate.Interlace); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.FrameRate.TimeScale); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.FrameRate.NumUnitsInTick); LocalSnPrint.Argc ++;
        RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);

        AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
        AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
            LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc dmy_sen hdr_ch %d %d'\nmpause 100";
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.HdrInfo.HdrType); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.HdrInfo.ActiveChannels); LocalSnPrint.Argc ++;
        RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);

        for (Idx = 0U; Idx < SsStatus.ModeInfo.HdrInfo.ActiveChannels; Idx ++) {

            AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
            AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc dmy_sen hdr %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d'\nmpause 100";
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(Idx); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.HdrInfo.ChannelInfo[Idx].EffectiveArea.StartX); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.HdrInfo.ChannelInfo[Idx].EffectiveArea.StartY); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.HdrInfo.ChannelInfo[Idx].EffectiveArea.Width); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.HdrInfo.ChannelInfo[Idx].EffectiveArea.Height); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.HdrInfo.ChannelInfo[Idx].OpticalBlackPixels.StartX); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.HdrInfo.ChannelInfo[Idx].OpticalBlackPixels.StartY); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.HdrInfo.ChannelInfo[Idx].OpticalBlackPixels.Width); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.HdrInfo.ChannelInfo[Idx].OpticalBlackPixels.Height); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.HdrInfo.ChannelInfo[Idx].ShutterSpeedCtrlInfo.CommunicationTime); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.HdrInfo.ChannelInfo[Idx].ShutterSpeedCtrlInfo.FirstReflectedFrame); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.HdrInfo.ChannelInfo[Idx].ShutterSpeedCtrlInfo.FirstBadFrame); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.HdrInfo.ChannelInfo[Idx].ShutterSpeedCtrlInfo.NumBadFrames); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.HdrInfo.ChannelInfo[Idx].OutputFormatCtrlInfo.CommunicationTime); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.HdrInfo.ChannelInfo[Idx].OutputFormatCtrlInfo.FirstReflectedFrame); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.HdrInfo.ChannelInfo[Idx].OutputFormatCtrlInfo.FirstBadFrame); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.HdrInfo.ChannelInfo[Idx].OutputFormatCtrlInfo.NumBadFrames); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.HdrInfo.ChannelInfo[Idx].MaxExposureLine); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SsStatus.ModeInfo.HdrInfo.ChannelInfo[Idx].MinExposureLine); LocalSnPrint.Argc ++;
            if (Idx == 0U) {
                RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
            } else {
                RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                RawEncTask_fprintf(pFile, pStrBuf);
            }
        }
    }
}

static void RawEncTask_GenScriptResCfg(AMBA_FS_FILE *pFile, char *pStrBuf, UINT32 StrBufSize, const SVC_RES_CFG_s* pResCfg, UINT32 VinID)
{
    UINT32 PRetVal, ErrCode;

    if ((pFile != NULL) && (pStrBuf != NULL) && (pResCfg != NULL) && (VinID < AMBA_DSP_MAX_VIN_NUM)) {
        UINT32 Idx;
        UINT8 IsPrnCmdUsage[2];
        UINT32 ChanIdx;
        SVC_WRAP_SNPRINT_s LocalSnPrint;
        UINT32 TempVal;

        PRN_RENC_TSK_LOG "------ %sGenerate Script - SvcResCfg%s ------"
            PRN_RENC_TSK_ARG_CSTR SVC_LOG_RENC_TSK_HL_TITLE_2 PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_CSTR SVC_LOG_RENC_TSK_HL_END     PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_API

        AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
        AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
            LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc res_cfg reset %s'";
            LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = " "; LocalSnPrint.Argc ++;
        RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);

        RawEncTask_fprintf(pFile, "\n; - VIN Tree");
        {
            AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
            AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc res_cfg vin vin_tree %s";
                LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = " "; LocalSnPrint.Argc ++;
            RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);

            AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
            AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                LocalSnPrint.pStrFmt = "$[VIN_%d][DMY_SENSOR_0][M_RAW_0]";
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)VinID; LocalSnPrint.Argc ++;
            RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);

            for (Idx = 0U; Idx < pResCfg->FovNum; Idx ++) {
                AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "[FOV_%d]";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)Idx; LocalSnPrint.Argc ++;
                RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
            }
            RawEncTask_fprintf(pFile, "#!'\nmpause 100");
        }

        RawEncTask_fprintf(pFile, "\n\n; - VIN setting");
        AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
        AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
            LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc res_cfg vin vin_num %d'\nmpause 100";
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->VinNum); LocalSnPrint.Argc ++;
        RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);

        AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
        AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
            LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc res_cfg vin cap_win %d %d %d %d %d'\nmpause 100";
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(VinID);                                  LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->VinCfg[VinID].CapWin.OffsetX);  LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->VinCfg[VinID].CapWin.OffsetY);  LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->VinCfg[VinID].CapWin.Width)  ;  LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->VinCfg[VinID].CapWin.Height) ;  LocalSnPrint.Argc ++;
        RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);

        AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
        AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
            LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc res_cfg vin sub_chan_num %d %d'\nmpause 100";
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(VinID); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->VinCfg[VinID].SubChanNum); LocalSnPrint.Argc ++;
        RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);

        AmbaSvcWrap_MisraMemset(IsPrnCmdUsage, 0, sizeof(IsPrnCmdUsage));
        for (Idx = 0U; Idx < pResCfg->VinCfg[VinID].SubChanNum; Idx ++) {
            AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
            AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc res_cfg vin sub_chan %d %d %d %d %d %d %d %d %d %d %d'\nmpause 100";
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(VinID); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(Idx); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->VinCfg[VinID].SubChanCfg[Idx].SubChan.IsVirtual); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->VinCfg[VinID].SubChanCfg[Idx].SubChan.Index); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->VinCfg[VinID].SubChanCfg[Idx].Option); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->VinCfg[VinID].SubChanCfg[Idx].ConCatNum); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->VinCfg[VinID].SubChanCfg[Idx].IntcNum); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->VinCfg[VinID].SubChanCfg[Idx].CaptureWindow.OffsetX); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->VinCfg[VinID].SubChanCfg[Idx].CaptureWindow.OffsetY); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->VinCfg[VinID].SubChanCfg[Idx].CaptureWindow.Width); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->VinCfg[VinID].SubChanCfg[Idx].CaptureWindow.Height); LocalSnPrint.Argc ++;
            if (IsPrnCmdUsage[0] == 0U) {
                RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
            } else {
                RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                RawEncTask_fprintf(pFile, pStrBuf);
            }

            IsPrnCmdUsage[0] = 1U;
        }

        AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
        AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
            LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc res_cfg vin frate %d %d %d %d'\nmpause 100";
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(VinID); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->VinCfg[VinID].FrameRate.Interlace); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->VinCfg[VinID].FrameRate.TimeScale); LocalSnPrint.Argc ++;
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->VinCfg[VinID].FrameRate.NumUnitsInTick); LocalSnPrint.Argc ++;
        RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);

        RawEncTask_fprintf(pFile, "\n\n; - SENSOR setting");
        AmbaSvcWrap_MisraMemset(IsPrnCmdUsage, 0, sizeof(IsPrnCmdUsage));
        for (Idx = 0U; Idx < 16U; Idx ++) {
            if (pResCfg->SensorCfg[VinID][Idx].IQTable != 0U) {
                AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc res_cfg sen %d %d %d %d %d'\nmpause 100";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(VinID); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(Idx); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->SensorCfg[VinID][Idx].SensorMode); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->SensorCfg[VinID][Idx].SensorGroup); LocalSnPrint.Argc ++;
                    TempVal = ( pResCfg->SensorCfg[VinID][Idx].IQTable & 0xFFFF0000U);
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(TempVal); LocalSnPrint.Argc ++;
                if (IsPrnCmdUsage[0] == 0U) {
                    RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
                } else {
                    RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                    RawEncTask_fprintf(pFile, pStrBuf);
                }
                IsPrnCmdUsage[0] = 1U;
            }
        }

        RawEncTask_fprintf(pFile, "\n\n; - FOV setting");
        AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
        AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
            LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc res_cfg fov fov_num %d'\nmpause 100";
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->FovNum); LocalSnPrint.Argc ++;
        RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);

        AmbaSvcWrap_MisraMemset(IsPrnCmdUsage, 0, sizeof(IsPrnCmdUsage));
        for(Idx = 0U; Idx < pResCfg->FovNum; Idx ++) {

            AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
            AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc res_cfg fov raw_win %d %d %d %d %d'\nmpause 100";
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(Idx); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->FovCfg[Idx].RawWin.OffsetX); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->FovCfg[Idx].RawWin.OffsetY); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->FovCfg[Idx].RawWin.Width); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->FovCfg[Idx].RawWin.Height); LocalSnPrint.Argc ++;
            if (IsPrnCmdUsage[0] == 0U) {
                RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
            } else {
                RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                RawEncTask_fprintf(pFile, pStrBuf);
            }

            AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
            AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc res_cfg fov act_win %d %d %d %d %d'\nmpause 100";
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(Idx); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->FovCfg[Idx].ActWin.OffsetX); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->FovCfg[Idx].ActWin.OffsetY); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->FovCfg[Idx].ActWin.Width); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->FovCfg[Idx].ActWin.Height); LocalSnPrint.Argc ++;
            if (IsPrnCmdUsage[0] == 0U) {
                RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
            } else {
                RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                RawEncTask_fprintf(pFile, pStrBuf);
            }

            AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
            AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc res_cfg fov main_win %d %d %d'\nmpause 100";
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(Idx); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->FovCfg[Idx].MainWin.Width); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->FovCfg[Idx].MainWin.Height); LocalSnPrint.Argc ++;
            if (IsPrnCmdUsage[0] == 0U) {
                RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
            } else {
                RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                RawEncTask_fprintf(pFile, pStrBuf);
            }

            AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
            AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc res_cfg fov pyramid %d %d'\nmpause 100";
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(Idx); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->FovCfg[Idx].PyramidBits); LocalSnPrint.Argc ++;
            if (IsPrnCmdUsage[0] == 0U) {
                RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
            } else {
                RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                RawEncTask_fprintf(pFile, pStrBuf);
            }

            AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
            AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc res_cfg fov hier_win %d %d %d'\nmpause 100";
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(Idx); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->FovCfg[Idx].HierWin.Width); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->FovCfg[Idx].HierWin.Height); LocalSnPrint.Argc ++;
            if (IsPrnCmdUsage[0] == 0U) {
                RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
            } else {
                RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                RawEncTask_fprintf(pFile, pStrBuf);
            }

            AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
            AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc res_cfg fov pipe_cfg %d %d %d %d %d %d %d'\nmpause 100";
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(Idx); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->FovCfg[Idx].PipeCfg.RotateFlip); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->FovCfg[Idx].PipeCfg.HdrExposureNum); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->FovCfg[Idx].PipeCfg.HdrBlendHieght); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->FovCfg[Idx].PipeCfg.MctfDisable); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->FovCfg[Idx].PipeCfg.MctsDisable); LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->FovCfg[Idx].PipeCfg.LinearCE); LocalSnPrint.Argc ++;
            if (IsPrnCmdUsage[0] == 0U) {
                RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
            } else {
                RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                RawEncTask_fprintf(pFile, pStrBuf);
            }

            IsPrnCmdUsage[0] = 1U;
        }

        RawEncTask_fprintf(pFile, "\n\n; - DISPLAY setting");
        AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
        AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
            LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc res_cfg disp sel_bits 0x%x'\nmpause 100";
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->DispBits); LocalSnPrint.Argc ++;
        RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);

        AmbaSvcWrap_MisraMemset(IsPrnCmdUsage, 0, sizeof(IsPrnCmdUsage));
        for (Idx = 0U; Idx < (UINT32)AMBA_DSP_MAX_VOUT_NUM; Idx ++) {
            if (pResCfg->DispStrm[Idx].pDriver != NULL) {

                AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc res_cfg disp vout_id %d %d'\nmpause 100";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(Idx); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->DispStrm[Idx].VoutID); LocalSnPrint.Argc ++;
                if (IsPrnCmdUsage[0] == 0U) {
                    RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
                } else {
                    RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                    RawEncTask_fprintf(pFile, pStrBuf);
                }

                PRetVal = 0xCafeU;
#ifndef CONFIG_BUILD_QNX_IPL
                ErrCode = SvcRawEnc_VoutDevGet(pResCfg->DispStrm[Idx].pDriver, &PRetVal); PRN_RENC_TSK_ERR_HDLR
#else
                PRetVal = 0U;   // HDMI only
                AmbaMisra_TouchUnused(&ErrCode);
#endif
                AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc res_cfg disp vout_obj %d 0x%x'\nmpause 100";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(Idx); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(PRetVal); LocalSnPrint.Argc ++;
                if (IsPrnCmdUsage[0] == 0U) {
                    RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
                } else {
                    RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                    RawEncTask_fprintf(pFile, pStrBuf);
                }

                AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc res_cfg disp win %d %d %d'\nmpause 100";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(Idx); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->DispStrm[Idx].StrmCfg.Win.Width); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->DispStrm[Idx].StrmCfg.Win.Height); LocalSnPrint.Argc ++;
                if (IsPrnCmdUsage[0] == 0U) {
                    RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
                } else {
                    RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                    RawEncTask_fprintf(pFile, pStrBuf);
                }

                AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc res_cfg disp max_win %d %d %d'\nmpause 100";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(Idx); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->DispStrm[Idx].StrmCfg.MaxWin.Width); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->DispStrm[Idx].StrmCfg.MaxWin.Height); LocalSnPrint.Argc ++;
                if (IsPrnCmdUsage[0] == 0U) {
                    RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
                } else {
                    RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                    RawEncTask_fprintf(pFile, pStrBuf);
                }

                AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc res_cfg disp ch_num %d %d'\nmpause 100";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(Idx); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->DispStrm[Idx].StrmCfg.NumChan); LocalSnPrint.Argc ++;
                if (IsPrnCmdUsage[0] == 0U) {
                    RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
                } else {
                    RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                    RawEncTask_fprintf(pFile, pStrBuf);
                }

                for (ChanIdx = 0U; ChanIdx < pResCfg->DispStrm[Idx].StrmCfg.NumChan; ChanIdx ++) {

                    AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                    AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                        LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc res_cfg disp ch_cfg %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d'\nmpause 100";
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(Idx); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(ChanIdx); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->DispStrm[Idx].StrmCfg.ChanCfg[ChanIdx].FovId); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->DispStrm[Idx].StrmCfg.ChanCfg[ChanIdx].SrcWin.OffsetX); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->DispStrm[Idx].StrmCfg.ChanCfg[ChanIdx].SrcWin.OffsetY); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->DispStrm[Idx].StrmCfg.ChanCfg[ChanIdx].SrcWin.Width); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->DispStrm[Idx].StrmCfg.ChanCfg[ChanIdx].SrcWin.Height); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->DispStrm[Idx].StrmCfg.ChanCfg[ChanIdx].DstWin.OffsetX); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->DispStrm[Idx].StrmCfg.ChanCfg[ChanIdx].DstWin.OffsetY); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->DispStrm[Idx].StrmCfg.ChanCfg[ChanIdx].DstWin.Width); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->DispStrm[Idx].StrmCfg.ChanCfg[ChanIdx].DstWin.Height); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->DispStrm[Idx].StrmCfg.ChanCfg[ChanIdx].Margin.Top); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->DispStrm[Idx].StrmCfg.ChanCfg[ChanIdx].Margin.Right); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->DispStrm[Idx].StrmCfg.ChanCfg[ChanIdx].Margin.Bottom); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->DispStrm[Idx].StrmCfg.ChanCfg[ChanIdx].Margin.Left); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->DispStrm[Idx].StrmCfg.ChanCfg[ChanIdx].RotateFlip); LocalSnPrint.Argc ++;
                    if ((IsPrnCmdUsage[0] == 0U) && (IsPrnCmdUsage[1] == 0U)) {
                        RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
                    } else {
                        RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                        RawEncTask_fprintf(pFile, pStrBuf);
                    }
                    IsPrnCmdUsage[1] = 1U;
                }

                AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc res_cfg disp frate %d %d %d %d'\nmpause 100";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(Idx); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->DispStrm[Idx].FrameRate.Interlace); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->DispStrm[Idx].FrameRate.TimeScale); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->DispStrm[Idx].FrameRate.NumUnitsInTick); LocalSnPrint.Argc ++;
                if (IsPrnCmdUsage[0] == 0U) {
                    RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
                } else {
                    RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                    RawEncTask_fprintf(pFile, pStrBuf);
                }

                AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc res_cfg disp cfg %d %d %d'\nmpause 100";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(Idx); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->DispStrm[Idx].VideoRotateFlip); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->DispStrm[Idx].DevMode); LocalSnPrint.Argc ++;
                if (IsPrnCmdUsage[0] == 0U) {
                    RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
                } else {
                    RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                    RawEncTask_fprintf(pFile, pStrBuf);
                }

                IsPrnCmdUsage[0] = 1U;
            }
        }

        RawEncTask_fprintf(pFile, "\n\n; - REC setting");
        AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
        AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
            LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc res_cfg rec sel_bits 0x%x'\nmpause 100";
            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->RecBits); LocalSnPrint.Argc ++;
        RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);

        AmbaSvcWrap_MisraMemset(IsPrnCmdUsage, 0, sizeof(IsPrnCmdUsage));
        for (Idx = 0U; Idx < (UINT32)AMBA_DSP_MAX_STREAM_NUM; Idx ++) {
            if (pResCfg->RecStrm[Idx].StrmCfg.Win.Width > 0U) {

                AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc res_cfg rec setting %d %d %d %d %d %d %d %d %d %d %d %d'\nmpause 100";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(Idx); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->RecStrm[Idx].RecSetting.RecId); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SVC_REC_SRC_VIDEO); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(SVC_REC_DST_FILE); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->RecStrm[Idx].RecSetting.M); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->RecStrm[Idx].RecSetting.N); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->RecStrm[Idx].RecSetting.IdrInterval); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->RecStrm[Idx].RecSetting.BitRate); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->RecStrm[Idx].RecSetting.FrameRate.Interlace); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->RecStrm[Idx].RecSetting.FrameRate.TimeScale); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->RecStrm[Idx].RecSetting.FrameRate.NumUnitsInTick); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->RecStrm[Idx].RecSetting.TimeLapse); LocalSnPrint.Argc ++;
                if (IsPrnCmdUsage[0] == 0U) {
                    RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
                } else {
                    RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                    RawEncTask_fprintf(pFile, pStrBuf);
                }

                AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc res_cfg rec win %d %d %d'\nmpause 100";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(Idx); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->RecStrm[Idx].StrmCfg.Win.Width); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->RecStrm[Idx].StrmCfg.Win.Height); LocalSnPrint.Argc ++;
                if (IsPrnCmdUsage[0] == 0U) {
                    RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
                } else {
                    RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                    RawEncTask_fprintf(pFile, pStrBuf);
                }

                AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc res_cfg rec max_win %d %d %d'\nmpause 100";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(Idx); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->RecStrm[Idx].StrmCfg.MaxWin.Width); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->RecStrm[Idx].StrmCfg.MaxWin.Height); LocalSnPrint.Argc ++;
                if (IsPrnCmdUsage[0] == 0U) {
                    RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
                } else {
                    RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                    RawEncTask_fprintf(pFile, pStrBuf);
                }

                AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc res_cfg rec ch_num %d %d'\nmpause 100";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(Idx); LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->RecStrm[Idx].StrmCfg.NumChan); LocalSnPrint.Argc ++;
                if (IsPrnCmdUsage[0] == 0U) {
                    RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
                } else {
                    RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                    RawEncTask_fprintf(pFile, pStrBuf);
                }

                for (ChanIdx = 0U; ChanIdx < pResCfg->RecStrm[Idx].StrmCfg.NumChan; ChanIdx ++) {

                    AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                    AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                        LocalSnPrint.pStrFmt = "\nsendln 'svc_rawenc res_cfg rec ch_cfg %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d'\nmpause 100";
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(Idx); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(ChanIdx); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->RecStrm[Idx].StrmCfg.ChanCfg[ChanIdx].FovId); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->RecStrm[Idx].StrmCfg.ChanCfg[ChanIdx].SrcWin.OffsetX); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->RecStrm[Idx].StrmCfg.ChanCfg[ChanIdx].SrcWin.OffsetY); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->RecStrm[Idx].StrmCfg.ChanCfg[ChanIdx].SrcWin.Width); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->RecStrm[Idx].StrmCfg.ChanCfg[ChanIdx].SrcWin.Height); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->RecStrm[Idx].StrmCfg.ChanCfg[ChanIdx].DstWin.OffsetX); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->RecStrm[Idx].StrmCfg.ChanCfg[ChanIdx].DstWin.OffsetY); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->RecStrm[Idx].StrmCfg.ChanCfg[ChanIdx].DstWin.Width); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->RecStrm[Idx].StrmCfg.ChanCfg[ChanIdx].DstWin.Height); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->RecStrm[Idx].StrmCfg.ChanCfg[ChanIdx].Margin.Top); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->RecStrm[Idx].StrmCfg.ChanCfg[ChanIdx].Margin.Right); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->RecStrm[Idx].StrmCfg.ChanCfg[ChanIdx].Margin.Bottom); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->RecStrm[Idx].StrmCfg.ChanCfg[ChanIdx].Margin.Left); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(pResCfg->RecStrm[Idx].StrmCfg.ChanCfg[ChanIdx].RotateFlip); LocalSnPrint.Argc ++;
                    if ((IsPrnCmdUsage[0] == 0U) && (IsPrnCmdUsage[1] == 0U)) {
                        RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
                    } else {
                        RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                        RawEncTask_fprintf(pFile, pStrBuf);
                    }

                    IsPrnCmdUsage[1] = 1U;
                }

                IsPrnCmdUsage[0] = 1U;
            }
        }
    }
}

static void RawEncTask_GenScriptIqTable(AMBA_FS_FILE *pFile, char *pStrBuf, UINT32 StrBufSize, const char *pScriptPrefix)
{
    if ((pFile != NULL) && (pStrBuf != NULL)) {
        UINT8 Idx;
        const char IqTblList[42U][90U] = {
            "\n  sendln 'svc_rawenc cfg iqtbl adj %s%sbins\\AdjTableParam.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl img %s%sbins\\ImageParam.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl aaa 0 %s%sbins\\DefaultParams.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl aaa 1 %s%sbins\\DefaultParams01.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl video 0 %s%sbins\\Adj_VideoPc00.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl video 1 %s%sbins\\Adj_VideoPc01.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl video 2 %s%sbins\\Adj_VideoPc02.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl photo 0 %s%sbins\\Adj_PhotoPreview00.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl photo 1 %s%sbins\\Adj_PhotoPreview01.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl still_liso 0 %s%sbins\\Adj_StillLIso00.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl still_liso 1 %s%sbins\\Adj_StillLIso01.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl still_hiso 0 %s%sbins\\Adj_StillHIso00.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl still_hiso 1 %s%sbins\\Adj_StillHIso01.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl video_cc 0 0 %s%sidspbins\\Cc_Video0.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl video_cc 0 1 %s%sidspbins\\Cc_Video1.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl video_cc 0 2 %s%sidspbins\\Cc_Video2.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl video_cc 0 3 %s%sidspbins\\Cc_Video3.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl video_cc 0 4 %s%sidspbins\\Cc_Video4.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl still_cc 0 0 %s%sidspbins\\Cc_Still0.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl still_cc 0 1 %s%sidspbins\\Cc_Still1.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl still_cc 0 2 %s%sidspbins\\Cc_Still2.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl still_cc 0 3 %s%sidspbins\\Cc_Still3.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl still_cc 0 4 %s%sidspbins\\Cc_Still4.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl video_cc 1 0 %s%sidspbins\\Cc_bw_gamma_lin_video.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl video_cc 1 1 %s%sidspbins\\Cc_bw_gamma_lin_video.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl video_cc 1 2 %s%sidspbins\\Cc_bw_gamma_lin_video.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl video_cc 1 3 %s%sidspbins\\Cc_bw_gamma_lin_video.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl video_cc 1 4 %s%sidspbins\\Cc_bw_gamma_lin_video.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl still_cc 1 0 %s%sidspbins\\Cc_bw_gamma_lin_still.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl still_cc 1 1 %s%sidspbins\\Cc_bw_gamma_lin_still.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl still_cc 1 2 %s%sidspbins\\Cc_bw_gamma_lin_still.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl still_cc 1 3 %s%sidspbins\\Cc_bw_gamma_lin_still.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl still_cc 1 4 %s%sidspbins\\Cc_bw_gamma_lin_still.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl scene 0 %s%sbins\\ScSet01Param.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl scene 1 %s%sbins\\ScSet02Param.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl scene 2 %s%sbins\\ScSet03Param.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl scene 3 %s%sbins\\ScSet04Param.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl scene 4 %s%sbins\\ScSet05Param.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl de_video 0 %s%sbins\\DeVideoParam.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl de_still 0 %s%sbins\\DeStillParam.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl vid_param %s%sbins\\VideoParam.bin%s",
            "\n  sendln 'svc_rawenc cfg iqtbl stl_param %s%sbins\\StillParam.bin%s",
        };
        UINT32 IqTblNum;
        SVC_WRAP_SNPRINT_s LocalSnPrint;

        PRN_RENC_TSK_LOG "------ %sGenerate Script - IQ Table%s ------"
            PRN_RENC_TSK_ARG_CSTR SVC_LOG_RENC_TSK_HL_TITLE_2 PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_CSTR SVC_LOG_RENC_TSK_HL_END     PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_API

        RawEncTask_fprintf(pFile, "\nif SvcRawEnc_UpdIqTblOn=1 then");

        IqTblNum = (UINT32)(sizeof(IqTblList)) / (UINT32)(sizeof(IqTblList[0]));

        for (Idx = 0U; Idx < IqTblNum; Idx ++) {
            AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);

            AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                LocalSnPrint.pStrFmt = IqTblList[Idx];
                LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = pScriptPrefix;                       LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = "\\iqtable\\";                       LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = "'\n  mpause 100";                   LocalSnPrint.Argc ++;
            RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
        }

        RawEncTask_fprintf(pFile, "\nendif");
    }
}

static void RawEncTask_GenScriptFeed(AMBA_FS_FILE *pFile, char *pStrBuf, UINT32 StrBufSize, const char *pScriptPrefix, UINT32 CapFrameNum, UINT32 FeedFrameNum, UINT32 VinID)
{
    UINT32 PRetVal;

    if ((pFile != NULL) && (pStrBuf != NULL) && (VinID < AMBA_DSP_MAX_VIN_NUM)) {
        SVC_WRAP_SNPRINT_s LocalSnPrint;

        PRN_RENC_TSK_LOG "------ %sGenerate Script - Feeding Frame%s ------"
            PRN_RENC_TSK_ARG_CSTR SVC_LOG_RENC_TSK_HL_TITLE_2 PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_CSTR SVC_LOG_RENC_TSK_HL_END     PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_API

        if (CapFrameNum == 0U) {

            if (FeedFrameNum == 0U) {

                RawEncTask_fprintf(pFile, "\n\nexit");

                RawEncTask_fprintf(pFile, "\n\n; The follow is an example to update idsp and feed one frame");

                RawEncTask_fprintf(pFile, "\n\n; -- Configure idsp update by ituner");
                AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "\n  sendln 'svc_rawenc cfg itn %d %s\\ituner\\itn_00.txt'\n  mpause 100";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)VinID;  LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr  = pScriptPrefix;  LocalSnPrint.Argc ++;
                RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);

                RawEncTask_fprintf(pFile, "\n\nif SvcRawEnc_AaaOn=1 then");
                RawEncTask_fprintf(pFile, "\n; -- Configure idsp update by extra aaa file");
                AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "\n    sendln 'svc_rawenc cfg aaa %s0 0 %s\\aaa\\aaa_00.bin'\n    mpause 100";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)VinID;  LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr  = pScriptPrefix;  LocalSnPrint.Argc ++;
                RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
                RawEncTask_fprintf(pFile, "\n\nendif");

                RawEncTask_fprintf(pFile, "\n\n; -- Configure feeding hdr file path if LinearCE or HDR");
                AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "\n  sendln 'svc_rawenc cfg hds %d %s\\hds\\hds_00.bin'\n  mpause 100";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)VinID;  LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr  = pScriptPrefix;  LocalSnPrint.Argc ++;
                RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);

                RawEncTask_fprintf(pFile, "\n\n; -- Configure feeding raw file path");
                AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "\n  sendln 'svc_rawenc cfg raw %d %s\\raw\\raw_00.bin'\n  mpause 100";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)VinID;  LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr  = pScriptPrefix;  LocalSnPrint.Argc ++;
                RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);

                RawEncTask_fprintf(pFile, "\n\n; -- Process above idsp and frame request");
                AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "\n  sprintf 'svc_rawenc execute %s'";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = " ";  LocalSnPrint.Argc ++;
                RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
                RawEncTask_fprintf(pFile, "\n  sprintf2 waitstr 'Successful to execute raw enc'"
                                          "\n  call _SUB_FUNC_CMD_PROC"
                                          "\n  pause 1");

                RawEncTask_fprintf(pFile, "\n\n; -- Dump raw enc result to file");
                AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "\n  sprintf 'svc_rawenc dump_yuv %s\\output\\yuv_00'";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = pScriptPrefix;  LocalSnPrint.Argc ++;
                RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
                RawEncTask_fprintf(pFile, "\n  sprintf2 waitstr 'Dump YUV stream done'"
                                          "\n  call _SUB_FUNC_CMD_PROC"
                                          "\n  pause 1");
            } else {

                AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "\n\n; Update idsp and feeding frame by follow loop from 1 to %s";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)FeedFrameNum; LocalSnPrint.Argc ++;
                RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                RawEncTask_fprintf(pFile, pStrBuf);

                AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "\n\n  for Idx 1 %d";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)FeedFrameNum; LocalSnPrint.Argc ++;
                RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                RawEncTask_fprintf(pFile, pStrBuf);


                RawEncTask_fprintf(pFile, "\n\n;   -- Configure idsp update by ituner");
                AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "\n    sprintf 'svc_rawenc cfg itn %d %s\\ituner\\itn_%s.txt' Idx";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)VinID;  LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr  = pScriptPrefix;  LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr  = "%d";  LocalSnPrint.Argc ++;
                RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
                RawEncTask_fprintf(pFile, "\n    sendln inputstr\n    mpause 100");

                RawEncTask_fprintf(pFile, "\n;   -- Configure idsp update by extra aaa file");
                RawEncTask_fprintf(pFile, "\n\n    if SvcRawEnc_AaaOn=1 then");
                AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "\n      sprintf 'svc_rawenc cfg aaa %d 0 0 %s\\aaa\\aaa_%s.bin' Idx";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)VinID;  LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr  = pScriptPrefix;  LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr  = "%d";  LocalSnPrint.Argc ++;
                RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
                RawEncTask_fprintf(pFile, "\n      sendln inputstr\n      mpause 100");
                RawEncTask_fprintf(pFile, "\n\n    endif");

                RawEncTask_fprintf(pFile, "\n\n;   -- Configure feeding hdr file path if LinearCE or HDR");
                AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "\n    sprintf 'svc_rawenc cfg hds %d %s\\hds\\hds_%s.bin' Idx";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)VinID;  LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr  = pScriptPrefix;  LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr  = "%d";  LocalSnPrint.Argc ++;
                RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
                RawEncTask_fprintf(pFile, "\n    sendln inputstr\n    mpause 100");

                RawEncTask_fprintf(pFile, "\n\n;   -- Configure feeding raw file path");
                AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "\n    sprintf 'svc_rawenc cfg raw %d %s\\raw\\raw_%s.bin' Idx";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)VinID;  LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr  = pScriptPrefix;  LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr  = "%d";  LocalSnPrint.Argc ++;
                RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
                RawEncTask_fprintf(pFile, "\n    sendln inputstr\n    mpause 100");

                RawEncTask_fprintf(pFile, "\n\n;   -- Process above idsp and frame request");
                AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "\n    sprintf 'svc_rawenc execute %s'";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = " ";  LocalSnPrint.Argc ++;
                RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
                RawEncTask_fprintf(pFile, "\n    sprintf2 waitstr 'Successful to execute raw enc'"
                                          "\n    call _SUB_FUNC_CMD_PROC"
                                          "\n    pause 1");

                RawEncTask_fprintf(pFile, "\n\n;   -- Dump raw enc result to file");
                AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "\n    sprintf 'svc_rawenc dump_yuv %s\\output\\yuv_%s' Idx";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = pScriptPrefix;  LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = "%d";  LocalSnPrint.Argc ++;
                RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
                RawEncTask_fprintf(pFile, "\n    sprintf2 waitstr 'Dump YUV stream done'"
                                          "\n    call _SUB_FUNC_CMD_PROC"
                                          "\n    pause 1");

                RawEncTask_fprintf(pFile, "\n\n next");
            }
        } else {
#define SVC_RAW_ENC_SCAN_RAW    (0x1U)
#define SVC_RAW_ENC_SCAN_HDS    (0x2U)
#define SVC_RAW_ENC_SCAN_ITN    (0x4U)
#define SVC_RAW_ENC_SCAN_AAA    (0x8U)

            UINT32 GetFirstFile = 0U, ProcCnt = 0U, CmdUsage = 0xFFFFFFFFU;
            AMBA_FS_DTA_t ItnFS, RawFS, HdsFS, AaaFS;
            char *pLongName = NULL;
            AMBA_IK_VIN_SENSOR_INFO_s IkSsInfo;
            AMBA_IK_MODE_CFG_s ImgMode = { .ContextId = 0U, };

            AmbaSvcWrap_MisraMemset(&IkSsInfo, 0, sizeof(IkSsInfo));
            PRetVal = AmbaIK_GetVinSensorInfo(&ImgMode, &IkSsInfo);
            if (PRetVal != 0U) {
                PRN_RENC_TSK_LOG "Fail to generate feed script - get IkContextID(%d) VinSensor info fail! ErrCode(0x%08X)"
                    PRN_RENC_TSK_ARG_UINT32 ImgMode.ContextId PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_ARG_UINT32 PRetVal           PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_NG
            }

            /* Try to scan first ituner file */
            AmbaSvcWrap_MisraMemset(&ItnFS, 0, sizeof(ItnFS));
            AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
            AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                LocalSnPrint.pStrFmt = "%s\\ituner\\*.txt";
                LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = pScriptPrefix; LocalSnPrint.Argc ++;
            RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
            PRN_RENC_TSK_LOG "Scan ituner folder to get 1st ituner text - %s"
                PRN_RENC_TSK_ARG_CSTR pStrBuf PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_API
            PRetVal = AmbaFS_FileSearchFirst(pStrBuf, 0x007FU, &ItnFS);
            if (PRetVal == 0U) {
                GetFirstFile |= SVC_RAW_ENC_SCAN_ITN;
            } else {
                PRN_RENC_TSK_LOG "-> There is not ituner text file" PRN_RENC_TSK_API
            }

            /* Try to scan first aaa file */
            AmbaSvcWrap_MisraMemset(&AaaFS, 0, sizeof(AaaFS));
            AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
            AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                LocalSnPrint.pStrFmt = "%s\\aaa\\*.bin";
                LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = pScriptPrefix;  LocalSnPrint.Argc ++;
            RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
            PRN_RENC_TSK_LOG "Scan aaa folder to get 1st aaa binary file - %s"
                PRN_RENC_TSK_ARG_CSTR pStrBuf PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_API
            PRetVal = AmbaFS_FileSearchFirst(pStrBuf, 0x007FU, &AaaFS);
            if (PRetVal == 0U) {
                GetFirstFile |= SVC_RAW_ENC_SCAN_AAA;
            } else {
                PRN_RENC_TSK_LOG "-> There is not aaa binary file" PRN_RENC_TSK_API
            }

            /* Try to scan first hds file */
            AmbaSvcWrap_MisraMemset(&HdsFS, 0, sizeof(HdsFS));
            AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
            if (IkSsInfo.Compression >= 255U) {
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "%s\\hds\\*_unpack.raw";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = pScriptPrefix;  LocalSnPrint.Argc ++;
                RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
            } else if (IkSsInfo.Compression == IK_RAW_COMPRESS_6P75) {
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "%s\\hds\\*_cmpr.raw";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = pScriptPrefix;  LocalSnPrint.Argc ++;
                RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
            } else {
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "%s\\hds\\*.raw";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = pScriptPrefix;  LocalSnPrint.Argc ++;
                RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
            }
            PRN_RENC_TSK_LOG "Scan hds folder to get 1st hds file - %s"
                PRN_RENC_TSK_ARG_CSTR pStrBuf PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_API
            PRetVal = AmbaFS_FileSearchFirst(pStrBuf, 0x007FU, &HdsFS);
            if (PRetVal == 0U) {
                GetFirstFile |= SVC_RAW_ENC_SCAN_HDS;
            } else {
                PRN_RENC_TSK_LOG "-> There is not hds file" PRN_RENC_TSK_API
            }

            /* Try to scan first raw file */
            AmbaSvcWrap_MisraMemset(&RawFS, 0, sizeof(RawFS));
            AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
            if (IkSsInfo.Compression >= 255U) {
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "%s\\raw\\*_unpack.raw";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = pScriptPrefix;  LocalSnPrint.Argc ++;
                RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
            } else if (IkSsInfo.Compression == IK_RAW_COMPRESS_6P75) {
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "%s\\raw\\*_cmpr.raw";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = pScriptPrefix;  LocalSnPrint.Argc ++;
                RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
            } else {
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "%s\\raw\\*.raw";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = pScriptPrefix;  LocalSnPrint.Argc ++;
                RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
            }
            PRN_RENC_TSK_LOG "Scan raw folder to get 1st raw file - %s"
                PRN_RENC_TSK_ARG_CSTR pStrBuf PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_API
            PRetVal = AmbaFS_FileSearchFirst(pStrBuf, 0x007FU, &RawFS);
            if (PRetVal == 0U) {
                GetFirstFile |= SVC_RAW_ENC_SCAN_RAW;

                RawEncTask_fprintf(pFile, "\n\n; Start update idsp and feeding frame by scan result");
            } else {
                PRN_RENC_TSK_LOG "-> There is not raw file" PRN_RENC_TSK_API
            }

            while ((GetFirstFile & SVC_RAW_ENC_SCAN_RAW) > 0U) {

                RawEncTask_fprintf(pFile, "\n\n; -----------------------------------------------------");
                AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "\n;                   FEED %03d FRAME BEGIN              ";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)ProcCnt; LocalSnPrint.Argc ++;
                RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                RawEncTask_fprintf(pFile, pStrBuf);
                RawEncTask_fprintf(pFile,   "\n; -----------------------------------------------------");

                /* Search next aaa file */
                RawEncTask_fprintf(pFile, "\n  if SvcRawEnc_AaaOn=1 then");
                if ((GetFirstFile & SVC_RAW_ENC_SCAN_AAA) > 0U) {
                    pLongName = (char *)(AaaFS.LongName);

                    PRN_RENC_TSK_LOG "Configure %02d aaa file - %s"
                        PRN_RENC_TSK_ARG_UINT32 ProcCnt PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_ARG_CSTR pLongName PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_API

                    AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                    AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                        LocalSnPrint.pStrFmt = "\n  sendln 'svc_rawenc cfg aaa %d %d %d %s\\aaa\\%s'\n  mpause 100";
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(VinID); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = 0U; LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = 0U; LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = pScriptPrefix;  LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = pLongName;      LocalSnPrint.Argc ++;
                    if ((CmdUsage & SVC_RAW_ENC_SCAN_AAA) > 0U) {
                        CmdUsage &= ~SVC_RAW_ENC_SCAN_AAA;
                        RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
                    } else {
                        RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                        RawEncTask_fprintf(pFile, pStrBuf);
                    }

                    PRN_RENC_TSK_LOG "-> Get next aaa binary file" PRN_RENC_TSK_API
                    if (0U != AmbaFS_FileSearchNext(&AaaFS)) {
                        GetFirstFile &= ~(SVC_RAW_ENC_SCAN_AAA);
                        PRetVal |= SVC_NG;
                        PRN_RENC_TSK_LOG "-> There is not next aaa binray file" PRN_RENC_TSK_API
                    }
                } else {
                    RawEncTask_fprintf(pFile, "\n;  Not aaa file to update");
                }

                RawEncTask_fprintf(pFile, "\n  else");

                /* Search next ituner file */
                if ((GetFirstFile & SVC_RAW_ENC_SCAN_ITN) > 0U) {
                    pLongName = (char *)(ItnFS.LongName);
                    PRN_RENC_TSK_LOG "Configure %02d ituner text file - %s"
                        PRN_RENC_TSK_ARG_UINT32 ProcCnt PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_ARG_CSTR pLongName PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_API
                    AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                    AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                        LocalSnPrint.pStrFmt = "\n    sendln 'svc_rawenc cfg itn %d %s\\ituner\\%s'\n    mpause 100";
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = 0U; LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = pScriptPrefix;  LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = pLongName;      LocalSnPrint.Argc ++;
                    if ((CmdUsage & SVC_RAW_ENC_SCAN_ITN) > 0U) {
                        CmdUsage &= ~SVC_RAW_ENC_SCAN_ITN;
                        RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
                    } else {
                        RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                        RawEncTask_fprintf(pFile, pStrBuf);
                    }

                    PRN_RENC_TSK_LOG "-> Get next ituner text file" PRN_RENC_TSK_API
                    if (0U != AmbaFS_FileSearchNext(&ItnFS)) {
                        GetFirstFile &= ~(SVC_RAW_ENC_SCAN_ITN);
                        PRetVal |= SVC_NG;
                        PRN_RENC_TSK_LOG "-> There is not next ituner text file" PRN_RENC_TSK_API
                    }
                } else {
                    RawEncTask_fprintf(pFile, "\n;  Not ituner file to update");
                }

                RawEncTask_fprintf(pFile, "\n  endif");

                if ((GetFirstFile & SVC_RAW_ENC_SCAN_HDS) > 0U) {
                    pLongName = (char *)(HdsFS.LongName);
                    PRN_RENC_TSK_LOG "Configure %02d hds file - %s"
                        PRN_RENC_TSK_ARG_UINT32 ProcCnt PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_ARG_CSTR pLongName PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_API
                    AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                    AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                        LocalSnPrint.pStrFmt = "\n  sendln 'svc_rawenc cfg hds %d %s\\hds\\%s'\n  mpause 100";
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(VinID); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = pScriptPrefix;  LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = pLongName;      LocalSnPrint.Argc ++;
                    if ((CmdUsage & SVC_RAW_ENC_SCAN_HDS) > 0U) {
                        CmdUsage &= ~SVC_RAW_ENC_SCAN_HDS;
                        RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
                    } else {
                        RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                        RawEncTask_fprintf(pFile, pStrBuf);
                    }

                    PRN_RENC_TSK_LOG "-> Get next hds file" PRN_RENC_TSK_API
                    if (0U != AmbaFS_FileSearchNext(&HdsFS)) {
                        GetFirstFile &= ~(SVC_RAW_ENC_SCAN_HDS);
                        PRetVal |= SVC_NG;
                        PRN_RENC_TSK_LOG "-> There is not next hds file" PRN_RENC_TSK_API
                    }
                }

                if ((GetFirstFile & SVC_RAW_ENC_SCAN_RAW) > 0U) {
                    pLongName = (char *)(RawFS.LongName);
                    PRN_RENC_TSK_LOG "Configure %02d raw file - %s"
                        PRN_RENC_TSK_ARG_UINT32 ProcCnt PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_ARG_CSTR pLongName PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_API
                    AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                    AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                        LocalSnPrint.pStrFmt = "\n  sendln 'svc_rawenc cfg raw %d %s\\raw\\%s'\n  mpause 100";
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(VinID); LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = pScriptPrefix;  LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = pLongName;      LocalSnPrint.Argc ++;
                    if ((CmdUsage & SVC_RAW_ENC_SCAN_RAW) > 0U) {
                        RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
                    } else {
                        RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                        RawEncTask_fprintf(pFile, pStrBuf);
                    }

                    AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                    AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                        LocalSnPrint.pStrFmt = "\n  sprintf 'svc_rawenc execute %s'";
                        LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = " "; LocalSnPrint.Argc ++;
                    if ((CmdUsage & SVC_RAW_ENC_SCAN_RAW) > 0U) {
                        RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
                    } else {
                        RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                        RawEncTask_fprintf(pFile, pStrBuf);
                    }

                    RawEncTask_fprintf(pFile, "\n  sprintf2 waitstr 'Successful to execute raw enc'"
                                              "\n  call _SUB_FUNC_CMD_PROC"
                                              "\n  pause 1"
                                              "\n" );

                    AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                    AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                        LocalSnPrint.pStrFmt = "\n  sprintf 'svc_rawenc dump_yuv %s\\output\\%s'";
                        LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = pScriptPrefix;  LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = pLongName;      LocalSnPrint.Argc ++;
                    if ((CmdUsage & SVC_RAW_ENC_SCAN_RAW) > 0U) {
                        RawEncTask_fCmdPrn(pFile, pStrBuf, StrBufSize, &LocalSnPrint);
                    } else {
                        RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                        RawEncTask_fprintf(pFile, pStrBuf);
                    }

                    RawEncTask_fprintf(pFile, "\n  sprintf2 waitstr 'Dump YUV stream done'"
                                              "\n  call _SUB_FUNC_CMD_PROC"
                                              "\n  pause 1"
                                              "\n" );

                    PRN_RENC_TSK_LOG "-> Get next raw file" PRN_RENC_TSK_API
                    if (0U != AmbaFS_FileSearchNext(&RawFS)) {
                        GetFirstFile &= ~(SVC_RAW_ENC_SCAN_RAW);
                        PRetVal |= SVC_NG;
                        PRN_RENC_TSK_LOG "-> There is not next raw file" PRN_RENC_TSK_API
                        RawEncTask_fprintf(pFile, "\n;  There are not next raw files");
                    }

                    CmdUsage &= ~SVC_RAW_ENC_SCAN_RAW;
                }

                RawEncTask_fprintf(pFile, "\n\n; -----------------------------------------------------");
                AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "\n;                   FEED %03d FRAME END              ";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)ProcCnt; LocalSnPrint.Argc ++;
                RawEncTask_snprintf(pStrBuf, StrBufSize, &LocalSnPrint);
                RawEncTask_fprintf(pFile, pStrBuf);
                RawEncTask_fprintf(pFile,   "\n; -----------------------------------------------------");

                ProcCnt ++;
            }

            AmbaMisra_TouchUnused(pLongName);
        }
    }
}

static UINT32 RawEncTask_MemDiff(const void *pVal0, const void *pVal1)
{
    UINT32 RetVal = 0U;

    if ((pVal0 != NULL) && (pVal1 != NULL)) {
#ifdef CONFIG_ARM64
        UINT64 Val_A = 0U, Val_B = 0U, CurDiff = 0U;
#else
        UINT32 Val_A = 0U, Val_B = 0U, CurDiff = 0U;
#endif

        AmbaMisra_TypeCast(&(Val_A), &(pVal0));
        AmbaMisra_TypeCast(&(Val_B), &(pVal1));

        if (Val_A > Val_B) {
            CurDiff = Val_A - Val_B;
        } else {
            CurDiff = Val_B - Val_A;
        }

        RetVal = (UINT32)CurDiff;
    }

    return RetVal;
}

static inline UINT32 RawEncTask_MsgDataRawInfoGet(const RAW_ENC_STRM_CAP_MSG_UNIT_s *pMsgData, RAW_ENC_STRM_CAP_RAW_INFO_s **pInfo, UINT32 InfoSize)
{
    UINT32 RetVal = SVC_NG;

    if (pMsgData == NULL) {
        PRN_RENC_TSK_LOG "Fail to get msg info - input msg data should not null!" PRN_RENC_TSK_NG
    } else if (pInfo == NULL) {
        PRN_RENC_TSK_LOG "Fail to get msg info - output info should not null!" PRN_RENC_TSK_NG
    } else if (InfoSize == 0U) {
        PRN_RENC_TSK_LOG "Fail to get msg info - output info size should not zero!" PRN_RENC_TSK_NG
    } else {
        if (InfoSize > ((UINT32)sizeof(pMsgData->MsgData))) {
            PRN_RENC_TSK_LOG "Fail to get msg info - info size(0x%X) should not larger then msg data size(0x%X)"
                PRN_RENC_TSK_ARG_UINT32 ((UINT32)sizeof(RAW_ENC_STRM_CAP_RAW_INFO_s))   PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_ARG_UINT32 ((UINT32)sizeof(pMsgData->MsgData))             PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_NG
        } else {
            RAW_ENC_STRM_CAP_RAW_INFO_s *pCurInfo = NULL;
            const UINT32 *pData = pMsgData->MsgData;
            AmbaMisra_TypeCast(&(pCurInfo), &pData);
            *pInfo = pCurInfo;

            RetVal = SVC_OK;
        }
    }

    return RetVal;
}

static inline UINT32 RawEncTask_MsgDataVidInfoGet(const RAW_ENC_STRM_CAP_MSG_UNIT_s *pMsgData, RAW_ENC_STRM_CAP_VIDEO_INFO_s **pInfo, UINT32 InfoSize)
{
    UINT32 RetVal = SVC_NG;

    if (pMsgData == NULL) {
        PRN_RENC_TSK_LOG "Fail to get msg info - input msg data should not null!" PRN_RENC_TSK_NG
    } else if (pInfo == NULL) {
        PRN_RENC_TSK_LOG "Fail to get msg info - output info should not null!" PRN_RENC_TSK_NG
    } else if (InfoSize == 0U) {
        PRN_RENC_TSK_LOG "Fail to get msg info - output info size should not zero!" PRN_RENC_TSK_NG
    } else {
        if (InfoSize > ((UINT32)sizeof(pMsgData->MsgData))) {
            PRN_RENC_TSK_LOG "Fail to get msg info - info size(0x%X) should not larger then msg data size(0x%X)"
                PRN_RENC_TSK_ARG_UINT32 ((UINT32)sizeof(RAW_ENC_STRM_CAP_VIDEO_INFO_s))   PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_ARG_UINT32 ((UINT32)sizeof(pMsgData->MsgData))             PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_NG
        } else {
            RAW_ENC_STRM_CAP_VIDEO_INFO_s *pCurInfo = NULL;
            const UINT32 *pData = pMsgData->MsgData;
            AmbaMisra_TypeCast(&(pCurInfo), &pData);
            *pInfo = pCurInfo;

            RetVal = SVC_OK;
        }
    }

    return RetVal;
}


static UINT32 RawEncTask_GetStrmCapFileName(UINT32 StreamID, char *pFileName, UINT32 Length, UINT32 TimeOut)
{
    UINT32 ErrCode = SVC_OK;

    if (StreamID >= AMBA_DSP_MAX_STREAM_NUM) {
        ErrCode = SVC_NG;
    } else if (pFileName == NULL) {
        ErrCode = SVC_NG;
    } else {
        UINT32 ReTryTime = GetAlignedValU32(TimeOut, 16U);
        char ReqFilePath[RAW_ENC_STRM_CAP_TEMP_STR_SIZE];
        char *pChar = NULL;

        if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_STRM_CAP_RAW_ENC) > 0U) {
            pChar = SvcWrap_strstr(pFileName, "_aaa_tmp_seq_file");
        } else if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_STRM_CAP_USER_CAP) > 0U) {
            pChar = SvcWrap_strstr(pFileName, "_user");
        } else {
            // misra-c
        }

        if (pChar != NULL) {
            *pChar = '\0';
        }

        do {
            AmbaSvcWrap_MisraMemset(ReqFilePath, 0, RAW_ENC_STRM_CAP_TEMP_STR_SIZE);
            ErrCode = SvcRawEnc_GetStrmCapFileName(StreamID, ReqFilePath, RAW_ENC_STRM_CAP_TEMP_STR_SIZE);
            if (ErrCode == 0U) {
                if ((UINT32)SvcWrap_strlen(ReqFilePath) == 0U) {
                    ErrCode = 8U;
                } else {
                    pChar = SvcWrap_strrchr(ReqFilePath, RAW_ENC_ASCII_DOT);
                    if (pChar != NULL) {
                        *pChar = '\0';
                    }

                    if (0 == SvcWrap_strcmp(ReqFilePath, pFileName)) {
                        ErrCode = 9U;
                        PRN_RENC_TSK_LOG "Request RecID(%d) filename fail! %s -> %s"
                            PRN_RENC_TSK_ARG_UINT32 StreamID    PRN_RENC_TSK_ARG_POST
                            PRN_RENC_TSK_ARG_CSTR   pFileName   PRN_RENC_TSK_ARG_POST
                            PRN_RENC_TSK_ARG_CSTR   ReqFilePath PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_DBG1
                    } else {
                        if (pChar != NULL) {
                            *pChar = '.';
                        }

                        SvcWrap_strcpy(pFileName, Length, ReqFilePath);
                    }
                }
            }

            if ((ReTryTime > 0U) && (ErrCode > 0U)) {
                ReTryTime -= 16U;
                RawEncTask_ErrHdlr(__func__, __LINE__, AmbaKAL_TaskSleep(16U));
            }
        } while ((ReTryTime > 0U) && (ErrCode > 0U));
    }

    return ErrCode;
}

static void* RawEncTask_StrmCapMsgTaskEntry(void* EntryArg)
{
    UINT32 ActualFlags, ErrCode;
    RAW_ENC_STRM_CAP_CTRL_s *pCapCtrl = &(RawEncCtrl.StrmCap);
    RAW_ENC_STRM_CAP_MSG_UNIT_s CurMsgUnit;
    RAW_ENC_STRM_CAP_RAW_INFO_s   *pRawInfo = NULL;
    RAW_ENC_STRM_CAP_VIDEO_INFO_s *pVidInfo = NULL;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaSvcWrap_MisraMemset(&CurMsgUnit, 0, sizeof(CurMsgUnit));
    ErrCode = RawEncTask_MsgDataRawInfoGet(&CurMsgUnit, &pRawInfo, (UINT32)sizeof(RAW_ENC_STRM_CAP_RAW_INFO_s));   PRN_RENC_TSK_ERR_HDLR
    ErrCode = RawEncTask_MsgDataVidInfoGet(&CurMsgUnit, &pVidInfo, (UINT32)sizeof(RAW_ENC_STRM_CAP_VIDEO_INFO_s)); PRN_RENC_TSK_ERR_HDLR
    if (ErrCode != SVC_OK) {
        PRN_RENC_TSK_LOG "Fail to proc stream cap task - get info fail" PRN_RENC_TSK_NG
    } else {
        while((pRawInfo != NULL) && (pVidInfo != NULL)) {
            ActualFlags = 0U;

            ErrCode = AmbaKAL_EventFlagSet(&(RawEncCtrl.Flag), SVC_RAW_ENC_CTRL_FLG_MSG_IDLE); PRN_RENC_TSK_ERR_HDLR

            ErrCode = AmbaKAL_EventFlagGet(&(RawEncCtrl.Flag),
                                        SVC_RAW_ENC_CTRL_FLG_MSG_UPD,
                                        AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_AUTO,
                                        &ActualFlags, AMBA_KAL_WAIT_FOREVER); PRN_RENC_TSK_ERR_HDLR

            ErrCode = AmbaKAL_EventFlagClear(&(RawEncCtrl.Flag), SVC_RAW_ENC_CTRL_FLG_MSG_IDLE); PRN_RENC_TSK_ERR_HDLR

            if ((ActualFlags & SVC_RAW_ENC_CTRL_FLG_MSG_UPD) > 0U) {

                AmbaSvcWrap_MisraMemset(&CurMsgUnit, 0, sizeof(CurMsgUnit));
                while (0U == AmbaKAL_MsgQueueReceive(&(pCapCtrl->MsgQue), &CurMsgUnit, AMBA_KAL_NO_WAIT)) {

                    if (CurMsgUnit.Type == SVC_RAW_ENC_STRM_CAP_MSG_RAW) {

                        RawEncTask_StrmCapScannerUpd(pRawInfo);

                    } else if (CurMsgUnit.Type == SVC_RAW_ENC_STRM_CAP_MSG_VIDEO) {

                        if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_STRM_CAP_RAW_ENC) > 0U) {
                            RawEncTask_StrmCapDataCap(pVidInfo);
                        } else if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_STRM_CAP_USER_CAP) > 0U) {
                            RawEncTask_StrmCapUserCap(pVidInfo);
                        } else {
                            // misra-c
                        }

                    } else {
                        // misra-c
                    }
                }
            }
        }
    }

    return NULL;
}

static void* RawEncTask_StrmCapFifoTaskEntry(void* EntryArg)
{
    ULONG ArgVal = 0U;
    UINT32 ErrCode;
    UINT32 ActualFlags;
    RAW_ENC_STRM_CAP_CTRL_s *pCapCtrl = &(RawEncCtrl.StrmCap);
    RAW_ENC_STRM_CAP_QUE_UNIT_s CurQueUnit;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaSvcWrap_MisraMemcpy(&ArgVal, EntryArg, sizeof(ULONG));

    while (ArgVal != 0xCafeU) {
        ErrCode = AmbaKAL_EventFlagSet(&(RawEncCtrl.Flag), SVC_RAW_ENC_CTRL_FLG_FIFO_IDLE);
        if (ErrCode != 0U) {
            PRN_RENC_TSK_LOG "Fail to proc save aaa to file - set fifo idle flag! ErrCode(0x%08X)"
                PRN_RENC_TSK_ARG_UINT32 ErrCode PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_NG
        }

        ErrCode = AmbaKAL_EventFlagGet(&(RawEncCtrl.Flag), SVC_RAW_ENC_CTRL_FLG_FIFO_PROC,
                                    AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlags, AMBA_KAL_WAIT_FOREVER); PRN_RENC_TSK_ERR_HDLR

        ErrCode = AmbaKAL_EventFlagClear(&(RawEncCtrl.Flag), SVC_RAW_ENC_CTRL_FLG_FIFO_IDLE); PRN_RENC_TSK_ERR_HDLR

        AmbaSvcWrap_MisraMemset(&CurQueUnit, 0, sizeof(CurQueUnit));
        while (0U == AmbaKAL_MsgQueueReceive(&(pCapCtrl->DataQue), &CurQueUnit, AMBA_KAL_NO_WAIT)) {

            if (((CurQueUnit.State & RAW_ENC_STRM_3A_DATA_EOS) > 0U)
              ||((CurQueUnit.State & RAW_ENC_STRM_3A_DATA_SPLIT) > 0U)) {

                if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_STRM_CAP_RAW_ENC) > 0U) {
                    RawEncTask_StrmCapFileClose(&CurQueUnit);
                } else if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_STRM_CAP_USER_CAP) > 0U) {
                    RawEncTask_StrmCapUserFileClose(&CurQueUnit);
                } else {
                    // misra-c
                }
            }

            if ((CurQueUnit.State & RAW_ENC_STRM_3A_DATA_DATA) > 0U) {
                if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_STRM_CAP_RAW_ENC) > 0U) {
                    RawEncTask_StrmCapFileOpen(&CurQueUnit);
                    RawEncTask_StrmCapFileWrite(&CurQueUnit);
                } else if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_STRM_CAP_USER_CAP) > 0U) {
                    RawEncTask_StrmCapUserFileOpen(&CurQueUnit);
                    RawEncTask_StrmCapUserFileWrite(&CurQueUnit);
                } else {
                    // misra-c
                }

                CurQueUnit.State     = 0U;
                if (CurQueUnit.pInfo != NULL) {
                    AmbaSvcWrap_MisraMemset(CurQueUnit.pInfo, 0, sizeof(RAW_ENC_STRM_CAP_INFO_s));
                }
                ErrCode = AmbaKAL_MsgQueueSend(&(RawEncCtrl.StrmCap.FreeQue), &CurQueUnit, AMBA_KAL_NO_WAIT);
                if (ErrCode != 0U) {
                    PRN_RENC_TSK_LOG "Fail to send buffer info to free queue! ErrCode(0x%08X)"
                        PRN_RENC_TSK_ARG_UINT32 ErrCode PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_NG
                }
            }
        }

        ErrCode = AmbaKAL_EventFlagClear(&(RawEncCtrl.Flag), SVC_RAW_ENC_CTRL_FLG_FIFO_PROC); PRN_RENC_TSK_ERR_HDLR

        AmbaMisra_TouchUnused(&ArgVal);
    }

    return NULL;
}

static UINT32 RawEncTask_StrmCapRawDataRdyHdlr(const void *pEventData)
{
    if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_STRM_CAP_INIT) > 0U) {
        const AMBA_DSP_RAW_DATA_RDY_s *pData;
        AmbaMisra_TypeCast(&(pData), &(pEventData));

        if (pData != NULL) {
            UINT32 PRetVal;
            RAW_ENC_STRM_CAP_MSG_UNIT_s CurMsgInfo;
            RAW_ENC_STRM_CAP_RAW_INFO_s *pRawInfo = NULL;

            if (SVC_OK == RawEncTask_MsgDataRawInfoGet(&CurMsgInfo, &pRawInfo, (UINT32)sizeof(RAW_ENC_STRM_CAP_RAW_INFO_s))) {

                AmbaSvcWrap_MisraMemset(&CurMsgInfo, 0, sizeof(CurMsgInfo));
                CurMsgInfo.Type       = SVC_RAW_ENC_STRM_CAP_MSG_RAW;
                pRawInfo->CapPts      = pData->CapPts;
                pRawInfo->CapSequence = pData->CapSequence;
                pRawInfo->VinID       = (UINT32)(pData->VinId); pRawInfo->VinID &= 0xFFFFU;

                PRetVal = AmbaKAL_MsgQueueSend(&(RawEncCtrl.StrmCap.MsgQue), &CurMsgInfo, AMBA_KAL_NO_WAIT);
                if (PRetVal != 0U) {
                    PRN_RENC_TSK_LOG "Fail to send raw info to queue! ErrCode(0x%08X)"
                        PRN_RENC_TSK_ARG_UINT32 PRetVal PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_NG
                } else {
                    PRetVal = AmbaKAL_EventFlagSet(&(RawEncCtrl.Flag), SVC_RAW_ENC_CTRL_FLG_MSG_UPD);
                    if (PRetVal != 0U) {
                        PRN_RENC_TSK_LOG "Fail to proc raw data rdy - set msg flag! ErrCode(0x%08X)"
                            PRN_RENC_TSK_ARG_UINT32 PRetVal PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_NG
                    }
                }
            }
        }
    }

    return 0U;
}

static UINT32 RawEncTask_StrmCapVidDataRdyHdlr(const void *pEventData)
{
    if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_STRM_CAP_INIT) > 0U) {
        const AMBA_DSP_ENC_PIC_RDY_s *pData;
        AmbaMisra_TypeCast(&(pData), &(pEventData));

        if (pData != NULL) {
            UINT32 PRetVal;
            RAW_ENC_STRM_CAP_MSG_UNIT_s CurMsgInfo;
            RAW_ENC_STRM_CAP_VIDEO_INFO_s *pVidInfo = NULL;

            if (SVC_OK == RawEncTask_MsgDataVidInfoGet(&CurMsgInfo, &pVidInfo, (UINT32)sizeof(RAW_ENC_STRM_CAP_VIDEO_INFO_s))) {
                AmbaSvcWrap_MisraMemset(&CurMsgInfo, 0, sizeof(CurMsgInfo));
                CurMsgInfo.Type            = SVC_RAW_ENC_STRM_CAP_MSG_VIDEO;
                pVidInfo->FrmNo            = pData->FrmNo;
                pVidInfo->CaptureTimeStamp = pData->CaptureTimeStamp;
                pVidInfo->RecID            = (UINT32)pData->StreamId;  pVidInfo->RecID &= 0xFFU;
                pVidInfo->FrmType          = (UINT32)pData->FrameType; pVidInfo->FrmType &= 0xFFU;

                PRetVal = AmbaKAL_MsgQueueSend(&(RawEncCtrl.StrmCap.MsgQue), &CurMsgInfo, AMBA_KAL_NO_WAIT);
                if (PRetVal != 0U) {
                    PRN_RENC_TSK_LOG "Fail to send video info to queue! ErrCode(0x%08X)"
                        PRN_RENC_TSK_ARG_UINT32 PRetVal PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_NG
                } else {
                    PRetVal = AmbaKAL_EventFlagSet(&(RawEncCtrl.Flag), SVC_RAW_ENC_CTRL_FLG_MSG_UPD);
                    if (PRetVal != 0U) {
                        PRN_RENC_TSK_LOG "Fail to proc vid data rdy - set msg flag! ErrCode(0x%08X)"
                            PRN_RENC_TSK_ARG_UINT32 PRetVal PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_NG
                    }
                }
            }
        }
    }

    return 0U;
}

static void RawEncTask_StrmCapScannerUpd(const RAW_ENC_STRM_CAP_RAW_INFO_s *pInfo)
{
    RAW_ENC_STRM_CAP_CTRL_s *pStrmCap = &(RawEncCtrl.StrmCap);

    if (pInfo == NULL) {
        PRN_RENC_TSK_LOG "Fail to update raw info scanner - input raw info should not null!" PRN_RENC_TSK_NG
    } else if (pInfo->VinID >= AMBA_DSP_MAX_VIN_NUM) {
        PRN_RENC_TSK_LOG "Fail to update raw info scanner - invalid VinID(%d)"
            PRN_RENC_TSK_ARG_UINT32 pInfo->VinID PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_NG
    } else if (0U != RawEncTask_MutexTake(&(pStrmCap->RawSeqScanner[pInfo->VinID].Mutex), 10000U)) {
        PRN_RENC_TSK_LOG "Fail to update raw info scanner - take mutex timeout!" PRN_RENC_TSK_NG
    } else {
        RAW_ENC_STRM_CAP_RAW_SEQ_s *pScanner = &(pStrmCap->RawSeqScanner[pInfo->VinID]);
        RAW_ENC_STRM_CAP_RAW_INFO_s *pUpdInfo = &(pScanner->Array[pScanner->WriteIdx]);

        pUpdInfo->VinID       = pInfo->VinID;
        pUpdInfo->CapPts      = pInfo->CapPts;
        pUpdInfo->CapSequence = pInfo->CapSequence;

        PRN_RENC_TSK_LOG "%s[SCAN] Upd raw info to scanner %02d, %02d, 0x%016llX, 0x%016llX %s"
            PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_RAW_SCAN PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_UINT32 pInfo->VinID                 PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_UINT32 pScanner->WriteIdx           PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_UINT64 pUpdInfo->CapPts
            PRN_RENC_TSK_ARG_UINT64 pUpdInfo->CapSequence
            PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_END      PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_DBG1

        pScanner->WriteIdx ++;
        if (pScanner->WriteIdx >= RAW_ENC_STRM_CAP_SCAN_NUM) {
            pScanner->WriteIdx = 0U;
        }

        RawEncTask_MutexGive(&(pStrmCap->RawSeqScanner[pInfo->VinID].Mutex));
    }
}

static void RawEncTask_StrmCapScannerProc(UINT32 VinID, UINT64 TimeStamp, UINT64 *pRawSeq)
{
    RAW_ENC_STRM_CAP_CTRL_s *pStrmCap = &(RawEncCtrl.StrmCap);

    if (pRawSeq == NULL) {
        PRN_RENC_TSK_LOG "Fail to scan raw info - output raw seq num should not null!" PRN_RENC_TSK_NG
    } else if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        PRN_RENC_TSK_LOG "Fail to scan raw info - invalid VinID(%d)!"
            PRN_RENC_TSK_ARG_UINT32 VinID PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_NG
    } else if (0U != RawEncTask_MutexTake(&(pStrmCap->RawSeqScanner[VinID].Mutex), 10000U)) {
        PRN_RENC_TSK_LOG "Fail to update raw info scanner - take mutex timeout!" PRN_RENC_TSK_NG
    } else {
        const RAW_ENC_STRM_CAP_RAW_SEQ_s *pScanner = &(pStrmCap->RawSeqScanner[VinID]);
        UINT32 ScanIdx = pScanner->WriteIdx;
        UINT32 ScanCnt = RAW_ENC_STRM_CAP_SCAN_NUM;

        *pRawSeq = 0U;

        while (ScanCnt > 0U) {

            if (ScanIdx == 0U) {
                ScanIdx = RAW_ENC_STRM_CAP_SCAN_NUM - 1U;
            } else {
                ScanIdx --;
            }

            if (pScanner->Array[ScanIdx].CapPts == TimeStamp) {
                *pRawSeq = pScanner->Array[ScanIdx].CapSequence;
                break;
            }

            ScanCnt --;
        }

        RawEncTask_MutexGive(&(pStrmCap->RawSeqScanner[VinID].Mutex));
    }
}

static void RawEncTask_StrmCapDataCap(const RAW_ENC_STRM_CAP_VIDEO_INFO_s *pInfo)
{
    RAW_ENC_STRM_CAP_CTRL_s *pStrmCap = &(RawEncCtrl.StrmCap);

    if (pInfo == NULL) {
        PRN_RENC_TSK_LOG "Fail to cap data - input video info should not null!" PRN_RENC_TSK_NG
    } else if (pInfo->CaptureTimeStamp == 0U) {
        PRN_RENC_TSK_LOG "Fail to cap data - video info cap time stamp should not zero!" PRN_RENC_TSK_NG
    } else if (pInfo->RecID >= AMBA_DSP_MAX_STREAM_NUM) {
        PRN_RENC_TSK_LOG "Fail to cap data - invalid RecID(%d)"
            PRN_RENC_TSK_ARG_UINT32 pInfo->RecID PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_NG
    } else if ((pStrmCap->RecSelectBits & SvcRawEnc_BitGet(pInfo->RecID)) == 0U) {
        PRN_RENC_TSK_LOG "Not process RecID(%d) stream capture" PRN_RENC_TSK_DBG
    } else if (pStrmCap->RecToVinBits[pInfo->RecID] == 0U) {
        PRN_RENC_TSK_LOG "Ignore RecID(%d) stream capture!" PRN_RENC_TSK_DBG
    } else {
        UINT32              PRetVal;
        UINT32              VinID, FovID;
        UINT64              RawSeq = 0U;
        UINT32              ImgAlgoIdx;
        UINT32              ImgAlgoInfoNum = 0U;
        SVC_IMG_ALGO_INFO_s ImgAlgoInfo[AMBA_DSP_MAX_VIEWZONE_NUM];
        RAW_ENC_STRM_CAP_INFO_s     CurUnitInfo;
        RAW_ENC_STRM_CAP_QUE_UNIT_s CurUnit;

        PRetVal = pStrmCap->RecFrameCnt[pInfo->RecID] + 1U;
        PRN_RENC_TSK_LOG "%s[ACAP] Capture AAA data by %02d, %04d, %d, 0x%016llX%s, %6d -> %6d"
            PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_STRM_CAP        PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_UINT32 pInfo->RecID                        PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_UINT32 pInfo->FrmNo                        PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_UINT32 pInfo->FrmType                      PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_UINT64 pInfo->CaptureTimeStamp
            PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_END             PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_UINT32 pStrmCap->RecFrameCnt[pInfo->RecID] PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_UINT32 PRetVal                             PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_DBG1

        pStrmCap->RecFrameCnt[pInfo->RecID] += 1U;

        for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
            if ((pStrmCap->RecToVinBits[pInfo->RecID] & SvcRawEnc_BitGet(VinID)) > 0U) {

                PRN_RENC_TSK_LOG "%s[ACAP] Try to get RawSeq by VinID(%02d), CaptureTimeStamp(0x%016llX)%s"
                    PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_STRM_CAP PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_ARG_UINT32 VinID                        PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_ARG_UINT64 pInfo->CaptureTimeStamp
                    PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_END      PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_DBG1

                RawSeq = 0U;
                RawEncTask_StrmCapScannerProc(VinID, pInfo->CaptureTimeStamp, &RawSeq);

                PRN_RENC_TSK_LOG "%s[ACAP] RawSeq(0x%016llX)%s"
                    PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_STRM_CAP PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_ARG_UINT64 RawSeq
                    PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_END      PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_DBG1

                if (RawSeq > 0U) {
                    ImgAlgoInfoNum = 0U;
                    AmbaSvcWrap_MisraMemset(ImgAlgoInfo, 0, sizeof(ImgAlgoInfo));
                    SvcImg_AlgoInfoGet(VinID, (UINT32)RawSeq, ImgAlgoInfo, &ImgAlgoInfoNum);

                    for (ImgAlgoIdx = 0U; ImgAlgoIdx < ImgAlgoInfoNum; ImgAlgoIdx ++) {
                        for (FovID = 0U; FovID < AMBA_DSP_MAX_VIEWZONE_NUM; FovID ++) {
                            if (((ImgAlgoInfo[ImgAlgoIdx].Fov.SelectBits & SvcRawEnc_BitGet(FovID)) > 0U)
                              &&((pStrmCap->RecToFovBits[pInfo->RecID] & SvcRawEnc_BitGet(FovID)) > 0U)) {
                                // Get the buffer from free queue
                                AmbaSvcWrap_MisraMemset(&CurUnit, 0, sizeof(CurUnit));
                                PRetVal = AmbaKAL_MsgQueueReceive(&(pStrmCap->FreeQue), &CurUnit, AMBA_KAL_NO_WAIT);
                                if (PRetVal != 0U) {
                                    PRN_RENC_TSK_LOG "Fail to cap data - get free buffer fail! ErrCode(0x%08X)"
                                        PRN_RENC_TSK_ARG_UINT32 PRetVal PRN_RENC_TSK_ARG_POST
                                    PRN_RENC_TSK_NG
                                } else if (CurUnit.pInfo == NULL) {
                                    PRN_RENC_TSK_LOG "Fail to cap data - invalid info!" PRN_RENC_TSK_NG
                                } else if (CurUnit.pData == NULL) {
                                    PRN_RENC_TSK_LOG "Fail to cap data - invalid buffer info!" PRN_RENC_TSK_NG
                                } else {

                                    AmbaSvcWrap_MisraMemset(CurUnit.pData, 0, sizeof(RAW_ENC_STRM_3A_DATA_s));
                                    CurUnit.State           |= RAW_ENC_STRM_3A_DATA_DATA;
                                    CurUnit.pInfo->VinID     = VinID;
                                    CurUnit.pInfo->FovID     = FovID;
                                    CurUnit.pInfo->FrameNo   = pInfo->FrmNo;
                                    CurUnit.pInfo->TimeStamp = pInfo->CaptureTimeStamp;
                                    CurUnit.pInfo->RawSeq[0] = RawSeq;
                                    CurUnit.pData->UpdBits   = 0U;
                                    CurUnit.pData->AaaData.MagicCode = SVC_RAW_CAP_AAA_MAGIC_CODE;
                                    CurUnit.pData->AaaData.ExposureNum = ImgAlgoInfo[ImgAlgoIdx].ExposureNum;
                                    if (ImgAlgoInfo[ImgAlgoIdx].Ctx.pAeInfo != NULL) {
                                        AmbaSvcWrap_MisraMemcpy(&(CurUnit.pData->AaaData.AeInfo),
                                                               ImgAlgoInfo[ImgAlgoIdx].Ctx.pAeInfo,
                                                               sizeof(AMBA_AE_INFO_s) * SVC_RAW_CAP_AAA_MAX_EXP_NUM);
                                        CurUnit.pData->UpdBits |= SvcRawEnc_BitGet(RAW_ENC_STRM_CAP_UPD_AAA);
                                    }
                                    if (ImgAlgoInfo[ImgAlgoIdx].Ctx.pAaaInfo != NULL) {
                                        AmbaSvcWrap_MisraMemcpy(&(CurUnit.pData->AaaData.AaaInfo),
                                                               ImgAlgoInfo[ImgAlgoIdx].Ctx.pAaaInfo,
                                                               sizeof(AMBA_IMGPROC_OFFLINE_AAA_INFO_s));
                                        CurUnit.pData->UpdBits |= SvcRawEnc_BitGet(RAW_ENC_STRM_CAP_UPD_AAA);
                                    }
                                    if (ImgAlgoInfo[ImgAlgoIdx].Fov.pPipe[FovID] != NULL) {
                                        AmbaSvcWrap_MisraMemcpy(&(CurUnit.pData->FovPipeData),
                                                               ImgAlgoInfo[ImgAlgoIdx].Fov.pPipe[FovID],
                                                               sizeof(SVC_IMG_ALGO_FOV_PIPE_s));
                                        CurUnit.pData->UpdBits |= SvcRawEnc_BitGet(RAW_ENC_STRM_CAP_UPD_FOV_PIPE);
                                    }
                                    if (pStrmCap->RecFrameCnt[pInfo->RecID] >= pStrmCap->RecSplitCnt[pInfo->RecID]) {
                                        if (pInfo->FrmType == PIC_FRAME_IDR) {
                                            CurUnit.State |= RAW_ENC_STRM_3A_DATA_SPLIT;
                                            pStrmCap->RecFrameCnt[pInfo->RecID] = 1U;
                                        }
                                    }

                                    PRetVal = AmbaKAL_MsgQueueSend(&(pStrmCap->DataQue), &CurUnit, AMBA_KAL_NO_WAIT);
                                    if (PRetVal != 0U) {
                                        PRN_RENC_TSK_LOG "Fail to cap data - write to data queue fail! ErrCode(0x%08X)"
                                            PRN_RENC_TSK_ARG_UINT32 PRetVal PRN_RENC_TSK_ARG_POST
                                        PRN_RENC_TSK_NG
                                    } else {
                                        PRN_RENC_TSK_LOG "%s[CAP] Success to write RecID(%02d) VinID(%02d) FovID(%02d) aaa to data queue!%s"
                                            PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_STRM_CAP PRN_RENC_TSK_ARG_POST
                                            PRN_RENC_TSK_ARG_UINT32 pInfo->RecID                 PRN_RENC_TSK_ARG_POST
                                            PRN_RENC_TSK_ARG_UINT32 CurUnit.pInfo->VinID         PRN_RENC_TSK_ARG_POST
                                            PRN_RENC_TSK_ARG_UINT32 CurUnit.pInfo->FovID         PRN_RENC_TSK_ARG_POST
                                            PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_END      PRN_RENC_TSK_ARG_POST
                                        PRN_RENC_TSK_DBG1

                                        PRetVal = AmbaKAL_EventFlagSet(&(RawEncCtrl.Flag), SVC_RAW_ENC_CTRL_FLG_FIFO_PROC);
                                        if (PRetVal != 0U) {
                                            PRN_RENC_TSK_LOG "Fail to cap data - set fifo flag! ErrCode(0x%08X)"
                                                PRN_RENC_TSK_ARG_UINT32 PRetVal PRN_RENC_TSK_ARG_POST
                                            PRN_RENC_TSK_NG
                                        }
                                    }
                                }
                            }
                        }
                    }
                } else {
                    // Get EOS frame
                    for (FovID = 0U; FovID < AMBA_DSP_MAX_VIEWZONE_NUM; FovID ++) {
                        if ((pStrmCap->RecToFovBits[pInfo->RecID] & SvcRawEnc_BitGet(FovID)) > 0U) {
                            AmbaSvcWrap_MisraMemset(&CurUnitInfo, 0, sizeof(CurUnitInfo));
                            CurUnitInfo.VinID     = VinID;
                            CurUnitInfo.FovID     = FovID;
                            CurUnitInfo.FrameNo   = 0xFFFFFFFFU;
                            CurUnitInfo.TimeStamp = 0xFFFFFFFFFFFFFFFFU;
                            CurUnitInfo.RawSeq[0] = 0xFFFFFFFFFFFFFFFFU;

                            AmbaSvcWrap_MisraMemset(&CurUnit, 0, sizeof(CurUnit));
                            CurUnit.State = RAW_ENC_STRM_3A_DATA_EOS;
                            CurUnit.pInfo = &CurUnitInfo;

                            PRetVal = AmbaKAL_MsgQueueSend(&(pStrmCap->DataQue), &CurUnit, AMBA_KAL_NO_WAIT);
                            if (PRetVal != 0U) {
                                PRN_RENC_TSK_LOG "Fail to cap data - write eos to data queue fail! ErrCode(0x%08X)"
                                    PRN_RENC_TSK_ARG_UINT32 PRetVal PRN_RENC_TSK_ARG_POST
                                PRN_RENC_TSK_NG
                            } else {
                                PRN_RENC_TSK_LOG "%s[CAP] Success to write RecID(%02d) VinID(%02d) FovID(%02d) eos to data queue!%s"
                                    PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_STRM_CAP PRN_RENC_TSK_ARG_POST
                                    PRN_RENC_TSK_ARG_UINT32 pInfo->RecID                 PRN_RENC_TSK_ARG_POST
                                    PRN_RENC_TSK_ARG_UINT32 CurUnit.pInfo->VinID         PRN_RENC_TSK_ARG_POST
                                    PRN_RENC_TSK_ARG_UINT32 CurUnit.pInfo->FovID         PRN_RENC_TSK_ARG_POST
                                    PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_END      PRN_RENC_TSK_ARG_POST
                                PRN_RENC_TSK_DBG1

                                PRetVal = AmbaKAL_EventFlagSet(&(RawEncCtrl.Flag), SVC_RAW_ENC_CTRL_FLG_FIFO_PROC);
                                if (PRetVal != 0U) {
                                    PRN_RENC_TSK_LOG "Fail to cap data - set fifo flag! ErrCode(0x%08X)"
                                        PRN_RENC_TSK_ARG_UINT32 PRetVal PRN_RENC_TSK_ARG_POST
                                    PRN_RENC_TSK_NG
                                }
                            }
                        }
                    }
                }
            }
        }

        PRN_RENC_TSK_LOG "%s[ACAP] Capture AAA data done%s"
            PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_STRM_CAP PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_END      PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_DBG1

    }
}

static void RawEncTask_StrmCapFileOpen(const RAW_ENC_STRM_CAP_QUE_UNIT_s *pUnit)
{
    if (pUnit == NULL) {
        PRN_RENC_TSK_LOG "Fail to open file - invalid input info" PRN_RENC_TSK_NG
    } else if (pUnit->pInfo == NULL) {
        PRN_RENC_TSK_LOG "Fail to open file - invalid unit info" PRN_RENC_TSK_NG
    } else if (pUnit->pInfo->VinID >= AMBA_DSP_MAX_VIN_NUM) {
        PRN_RENC_TSK_LOG "Fail to open file - invalid VinID(%d)"
            PRN_RENC_TSK_ARG_UINT32 pUnit->pInfo->VinID PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_NG
    } else if (pUnit->pInfo->FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
        PRN_RENC_TSK_LOG "Fail to open file - invalid FovID(%d)"
            PRN_RENC_TSK_ARG_UINT32 pUnit->pInfo->FovID PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_NG
    } else {
        RAW_ENC_STRM_CAP_CTRL_s *pCapCtrl = &(RawEncCtrl.StrmCap);
        RAW_ENC_STRM_CAP_FILE_s *pFileCtrl = &(pCapCtrl->AaaFile[pUnit->pInfo->FovID]);
        char *pFilePath, *pChar;
        UINT32 ErrCode, NumWriteSize, WriteSize;
        RAW_ENC_STRM_CAP_SEQ_HEADER_s CapSeqHeader;

        // If the specify FovID sequence file is not created, create it and write the header first.
        if ((pFileCtrl->State & RAW_ENC_STRM_CAP_FILE_OPEN) == 0U) {
            pFilePath = pFileCtrl->FilePath;

            if ((pUnit->State & RAW_ENC_STRM_3A_DATA_SPLIT) == 0U) {
                AmbaSvcWrap_MisraMemset(pFilePath, 0, RAW_ENC_STRM_CAP_TEMP_STR_SIZE);
            }
            ErrCode = RawEncTask_GetStrmCapFileName(pCapCtrl->FovToRecID[pUnit->pInfo->FovID], pFilePath, RAW_ENC_STRM_CAP_TEMP_STR_SIZE, 5008U);
            if (ErrCode == 0U) {
                PRN_RENC_TSK_LOG "Get stream filename. FovID(%d), RecID(%d) -> '%s'"
                    PRN_RENC_TSK_ARG_UINT32 pUnit->pInfo->FovID                       PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_ARG_UINT32 pCapCtrl->FovToRecID[pUnit->pInfo->FovID] PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_ARG_CSTR   pFilePath                                 PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_DBG

                pChar = SvcWrap_strrchr(pFilePath, RAW_ENC_ASCII_DOT);
                if (pChar != NULL) {
                    *pChar = '\0';
                }

                //  xxx_aaa_tmp_seq_file_[vin]_[fov].bin
                AmbaUtility_StringAppend(pFilePath, RAW_ENC_STRM_CAP_TEMP_STR_SIZE, RAW_ENC_STRM_CAP_TEMP_PATH);
                AmbaUtility_StringAppend(pFilePath, RAW_ENC_STRM_CAP_TEMP_STR_SIZE, "_");
                AmbaUtility_StringAppendUInt32(pFilePath, RAW_ENC_STRM_CAP_TEMP_STR_SIZE, pUnit->pInfo->VinID, 10U);
                AmbaUtility_StringAppend(pFilePath, RAW_ENC_STRM_CAP_TEMP_STR_SIZE, "_");
                AmbaUtility_StringAppendUInt32(pFilePath, RAW_ENC_STRM_CAP_TEMP_STR_SIZE, pUnit->pInfo->FovID, 10U);
                AmbaUtility_StringAppend(pFilePath, RAW_ENC_STRM_CAP_TEMP_STR_SIZE, ".bin");

                AmbaSvcWrap_MisraMemset(&(pFileCtrl->VfsFile), 0, sizeof(AMBA_VFS_FILE_s));
                ErrCode = AmbaVFS_Open(pFilePath, "w", 1U, &(pFileCtrl->VfsFile));
                if (ErrCode != 0U) {
                    PRN_RENC_TSK_LOG "Fail to create aaa temp file! ErrCode(0x%08X) %s"
                        PRN_RENC_TSK_ARG_UINT32 ErrCode   PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_ARG_CSTR   pFilePath PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_NG
                } else if (pFileCtrl->VfsFile.pFile == NULL) {
                    PRN_RENC_TSK_LOG "Fail to create aaa temp file - invalid file pointer!" PRN_RENC_TSK_NG
                } else {

                    pFileCtrl->State |= RAW_ENC_STRM_CAP_FILE_OPEN;

                    PRN_RENC_TSK_LOG "%s[FIFO] Success to create VinID(%d) FovID(%d) aaa temp file %s %s"
                        PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_FIFO PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_ARG_UINT32 pUnit->pInfo->VinID      PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_ARG_UINT32 pUnit->pInfo->FovID      PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_ARG_CSTR   pFilePath                PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_END  PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_DBG1

                    AmbaSvcWrap_MisraMemset(&CapSeqHeader, 0, sizeof(CapSeqHeader));
                    CapSeqHeader.VinID    = pUnit->pInfo->VinID;
                    CapSeqHeader.FovID    = pUnit->pInfo->FovID;
                    CapSeqHeader.UnitSize = (UINT32)sizeof(RAW_ENC_STRM_CAP_SEQ_UNIT_s);

                    NumWriteSize = 0U;
                    WriteSize    = (UINT32)sizeof(CapSeqHeader);
                    ErrCode = AmbaVFS_Write(&CapSeqHeader, 1U, WriteSize, &(pFileCtrl->VfsFile), &NumWriteSize);
                    if (ErrCode != 0U) {
                        PRN_RENC_TSK_LOG "Fail to write aaa header! ErrCode(0x%08X)"
                            PRN_RENC_TSK_ARG_UINT32 ErrCode PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_NG
                    } else if (NumWriteSize != WriteSize) {
                        ErrCode = 1;
                        PRN_RENC_TSK_LOG "Fail to write aaa header size(0x%X) does not same with request size(0x%X)"
                            PRN_RENC_TSK_ARG_UINT32 NumWriteSize PRN_RENC_TSK_ARG_POST
                            PRN_RENC_TSK_ARG_UINT32 WriteSize    PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_NG
                    } else {
                        PRN_RENC_TSK_LOG "%s[FIFO] Success to write aaa header. VinID(%d), FovID(%d) %s"
                            PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_FIFO PRN_RENC_TSK_ARG_POST
                            PRN_RENC_TSK_ARG_UINT32 pUnit->pInfo->VinID      PRN_RENC_TSK_ARG_POST
                            PRN_RENC_TSK_ARG_UINT32 pUnit->pInfo->FovID      PRN_RENC_TSK_ARG_POST
                            PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_END  PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_DBG1
                    }

                    if (ErrCode != 0U) {
                        pFileCtrl->State &= ~RAW_ENC_STRM_CAP_FILE_OPEN;

                        ErrCode = AmbaVFS_Close(&(pFileCtrl->VfsFile)); PRN_RENC_TSK_ERR_HDLR
                        ErrCode = AmbaFS_Remove(pFilePath); PRN_RENC_TSK_ERR_HDLR
                        AmbaSvcWrap_MisraMemset(&(pFileCtrl->VfsFile), 0, sizeof(AMBA_VFS_FILE_s));
                        PRN_RENC_TSK_LOG "Fail to write aaa header! Close and Delete file" PRN_RENC_TSK_NG
                    }
                }
            }
        }
    }
}

static void RawEncTask_StrmCapFileClose(const RAW_ENC_STRM_CAP_QUE_UNIT_s *pUnit)
{
    if (pUnit == NULL) {
        PRN_RENC_TSK_LOG "Fail to close file - invalid input info" PRN_RENC_TSK_NG
    } else if (pUnit->pInfo == NULL) {
        PRN_RENC_TSK_LOG "Fail to close file - invalid unit info" PRN_RENC_TSK_NG
    } else if (pUnit->pInfo->VinID >= AMBA_DSP_MAX_VIN_NUM) {
        PRN_RENC_TSK_LOG "Fail to close file - invalid VinID(%d)"
            PRN_RENC_TSK_ARG_UINT32 pUnit->pInfo->VinID PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_NG
    } else if (pUnit->pInfo->FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
        PRN_RENC_TSK_LOG "Fail to close file - invalid FovID(%d)"
            PRN_RENC_TSK_ARG_UINT32 pUnit->pInfo->FovID PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_NG
    } else {
        RAW_ENC_STRM_CAP_CTRL_s *pCapCtrl = &(RawEncCtrl.StrmCap);
        RAW_ENC_STRM_CAP_FILE_s *pFileCtrl = &(pCapCtrl->AaaFile[pUnit->pInfo->FovID]);
        UINT32 ErrCode;

        if ((pFileCtrl->State & RAW_ENC_STRM_CAP_FILE_OPEN) == 0U) {
            PRN_RENC_TSK_LOG "[FIFO] The VinID(%d) FovID(%d) aaa seq file did not create"
                PRN_RENC_TSK_ARG_UINT32 pUnit->pInfo->VinID PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_ARG_UINT32 pUnit->pInfo->FovID PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_DBG
        } else {

            ErrCode = AmbaVFS_Sync(&(pFileCtrl->VfsFile));
            if (ErrCode != 0U) {
                PRN_RENC_TSK_LOG "Fail to close file - file sync fail! ErrCode(0x%08X)"
                    PRN_RENC_TSK_ARG_UINT32 ErrCode PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_NG
            }

            ErrCode = AmbaVFS_Close(&(pFileCtrl->VfsFile));
            if (ErrCode != 0U) {
                PRN_RENC_TSK_LOG "Fail to close file - close aaa seq data fail! ErrCode(0x%08X)"
                    PRN_RENC_TSK_ARG_UINT32 ErrCode PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_NG
            } else {
                pFileCtrl->State &= ~RAW_ENC_STRM_CAP_FILE_OPEN;

                PRN_RENC_TSK_LOG "[FIFO] Success to save VinID(%d) FovID(%d) aaa seq file - %s"
                    PRN_RENC_TSK_ARG_UINT32 pUnit->pInfo->VinID PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_ARG_UINT32 pUnit->pInfo->FovID PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_ARG_CSTR   pFileCtrl->FilePath PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_API

                AmbaSvcWrap_MisraMemset(&(pFileCtrl->VfsFile), 0, sizeof(AMBA_VFS_FILE_s));

                if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_STRM_CAP_POST_TEXT) > 0U) {

                    if (pCapCtrl->CvtTxtCtrl.pTxtBuf == NULL) {
                        PRN_RENC_TSK_LOG "Fail to close file - invalid covert txt buffer" PRN_RENC_TSK_NG
                    } else {

                        if ((pUnit->State & RAW_ENC_STRM_3A_DATA_EOS) == 0U) {
                            if((pCapCtrl->CvtTxtCtrl.CurTxtNum + 1U) > pCapCtrl->CvtTxtCtrl.MaxTxtBuf) {
                                PRN_RENC_TSK_LOG "Fail to close file - covert txt buffer full" PRN_RENC_TSK_NG
                            } else {
                                SvcWrap_strcpy(pCapCtrl->CvtTxtCtrl.pTxtBuf->FilePath[pCapCtrl->CvtTxtCtrl.CurTxtNum]
                                              , RAW_ENC_STRM_CAP_TEMP_STR_SIZE
                                              , pFileCtrl->FilePath );

                                PRN_RENC_TSK_LOG "[FIFO] Success to add covert txt to No.%03d slot - %s"
                                    PRN_RENC_TSK_ARG_UINT32 pCapCtrl->CvtTxtCtrl.CurTxtNum PRN_RENC_TSK_ARG_POST
                                    PRN_RENC_TSK_ARG_CSTR   pCapCtrl->CvtTxtCtrl.pTxtBuf->FilePath[pCapCtrl->CvtTxtCtrl.CurTxtNum] PRN_RENC_TSK_ARG_POST
                                PRN_RENC_TSK_API

                                pCapCtrl->CvtTxtCtrl.CurTxtNum += 1U;
                            }
                        } else {

                            RawEncTask_StrmCapPostProc(pFileCtrl->FilePath);

                            while (pCapCtrl->CvtTxtCtrl.CurTxtNum > 0U) {

                                pCapCtrl->CvtTxtCtrl.CurTxtNum -= 1U;

                                RawEncTask_StrmCapPostProc(pCapCtrl->CvtTxtCtrl.pTxtBuf->FilePath[pCapCtrl->CvtTxtCtrl.CurTxtNum]);
                            }
                        }
                    }
                }
            }
        }

    }
}

static void RawEncTask_StrmCapFileWrite(const RAW_ENC_STRM_CAP_QUE_UNIT_s *pUnit)
{
    if (pUnit == NULL) {
        PRN_RENC_TSK_LOG "Fail to write aaa file - invalid input info" PRN_RENC_TSK_NG
    } else if ((pUnit->State & RAW_ENC_STRM_3A_DATA_DATA) == 0U) {
        PRN_RENC_TSK_LOG "Fail to write aaa file - invalid input info tpye." PRN_RENC_TSK_NG
    } else if (pUnit->pInfo == NULL) {
        PRN_RENC_TSK_LOG "Fail to write aaa file - invalid unit info tpye." PRN_RENC_TSK_NG
    } else if (pUnit->pInfo->VinID >= AMBA_DSP_MAX_VIN_NUM) {
        PRN_RENC_TSK_LOG "Fail to write aaa file - invalid VinID(%d)"
            PRN_RENC_TSK_ARG_UINT32 pUnit->pInfo->VinID PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_NG
    } else if (pUnit->pInfo->FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
        PRN_RENC_TSK_LOG "Fail to write aaa file - invalid FovID(%d)"
            PRN_RENC_TSK_ARG_UINT32 pUnit->pInfo->FovID PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_NG
    } else if (pUnit->pData == NULL) {
        PRN_RENC_TSK_LOG "Fail to write aaa file - invalid data." PRN_RENC_TSK_NG
    } else if (pUnit->pData->UpdBits == 0U) {
        PRN_RENC_TSK_LOG "There is not aaa data need to write" PRN_RENC_TSK_DBG1
    } else {
        const RAW_ENC_STRM_CAP_CTRL_s *pCapCtrl = &(RawEncCtrl.StrmCap);
        const RAW_ENC_STRM_CAP_FILE_s *pFileCtrl = &(pCapCtrl->AaaFile[pUnit->pInfo->FovID]);
        UINT32 ErrCode, NumWriteSize, WriteSize;
        RAW_ENC_STRM_CAP_SEQ_UNIT_s   CapSeqData;

        if ((pFileCtrl->State & RAW_ENC_STRM_CAP_FILE_OPEN) == 0U) {
            PRN_RENC_TSK_LOG "Fail to write aaa file - create file first. VinID(%d) FovID(%d)"
                PRN_RENC_TSK_ARG_UINT32 pUnit->pInfo->VinID PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_ARG_UINT32 pUnit->pInfo->FovID PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_API
        } else {
            if ((pUnit->pInfo->RawSeq[0] > 0U) && (pUnit->pInfo->RawSeq[0] == pFileCtrl->PreRawSeq)) {
                PRN_RENC_TSK_LOG "%s[FIFO] VinID(%d) FovID(%d) RawSeq(0x%016llX) has been updated. Ignore it %s"
                    PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_FIFO PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_ARG_UINT32 pUnit->pInfo->VinID      PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_ARG_UINT32 pUnit->pInfo->FovID      PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_ARG_UINT64 pUnit->pInfo->RawSeq[0]
                    PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_END  PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_DBG1
            } else {

                AmbaSvcWrap_MisraMemset(&CapSeqData, 0, sizeof(CapSeqData));
                CapSeqData.FrameNo = pUnit->pInfo->FrameNo;
                CapSeqData.UpdBits = pUnit->pData->UpdBits;
                if ((CapSeqData.UpdBits & SvcRawEnc_BitGet(RAW_ENC_STRM_CAP_UPD_AAA)) > 0U) {
                    CapSeqData.UpdInfo[RAW_ENC_STRM_CAP_UPD_AAA].Offset = RawEncTask_MemDiff(&CapSeqData, &(CapSeqData.AaaData));
                    CapSeqData.UpdInfo[RAW_ENC_STRM_CAP_UPD_AAA].Size   = (UINT32)sizeof(SVC_RAW_CAP_AAA_DATA_s);
                    AmbaSvcWrap_MisraMemcpy(&(CapSeqData.AaaData), &(pUnit->pData->AaaData), sizeof(SVC_RAW_CAP_AAA_DATA_s));
                }
                if ((CapSeqData.UpdBits & SvcRawEnc_BitGet(RAW_ENC_STRM_CAP_UPD_FOV_PIPE)) > 0U) {
                    CapSeqData.UpdInfo[RAW_ENC_STRM_CAP_UPD_FOV_PIPE].Offset = RawEncTask_MemDiff(&CapSeqData, &(CapSeqData.FovPipeData));
                    CapSeqData.UpdInfo[RAW_ENC_STRM_CAP_UPD_FOV_PIPE].Size   = (UINT32)sizeof(SVC_IMG_ALGO_FOV_PIPE_s);
                    AmbaSvcWrap_MisraMemcpy(&(CapSeqData.FovPipeData), &(pUnit->pData->FovPipeData), sizeof(SVC_IMG_ALGO_FOV_PIPE_s));

                }

                NumWriteSize = 0U;
                WriteSize    = (UINT32)sizeof(CapSeqData);
                ErrCode = AmbaVFS_Write(&CapSeqData, 1U, WriteSize, &(pFileCtrl->VfsFile), &NumWriteSize);
                if (ErrCode != 0U) {
                    PRN_RENC_TSK_LOG "Fail to write aaa data! ErrCode(0x%08X)"
                        PRN_RENC_TSK_ARG_UINT32 ErrCode PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_NG
                } else if (NumWriteSize != WriteSize) {
                    PRN_RENC_TSK_LOG "Fail to write aaa data size(0x%X) does not same with request size(0x%X)"
                        PRN_RENC_TSK_ARG_UINT32 NumWriteSize PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_ARG_UINT32 WriteSize    PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_NG
                } else {

                    PRN_RENC_TSK_LOG "%s[FIFO] Success to write aaa data to seq file. VinID(%d), FovID(%d), FrameNo(%5d) %s"
                        PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_FIFO  PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_ARG_UINT32 pUnit->pInfo->VinID       PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_ARG_UINT32 pUnit->pInfo->FovID       PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_ARG_UINT32 pUnit->pInfo->FrameNo     PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_END   PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_DBG1
                }
            }
        }
    }
}

static void RawEncTask_StrmCapUserCap(const RAW_ENC_STRM_CAP_VIDEO_INFO_s *pInfo)
{
    RAW_ENC_STRM_CAP_CTRL_s *pStrmCap = &(RawEncCtrl.StrmCap);

    if (pInfo == NULL) {
        PRN_RENC_TSK_LOG "Fail to cap data - input video info should not null!" PRN_RENC_TSK_NG
    } else if (pInfo->CaptureTimeStamp == 0U) {
        PRN_RENC_TSK_LOG "Fail to cap data - video info cap time stamp should not zero!" PRN_RENC_TSK_NG
    } else if (pInfo->RecID >= AMBA_DSP_MAX_STREAM_NUM) {
        PRN_RENC_TSK_LOG "Fail to cap data - invalid RecID(%d)!"
            PRN_RENC_TSK_ARG_UINT32 pInfo->RecID PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_NG
    } else if ((pStrmCap->RecSelectBits & SvcRawEnc_BitGet(pInfo->RecID)) == 0U) {
        PRN_RENC_TSK_LOG "Not process RecID(%d) stream capture" PRN_RENC_TSK_DBG
    } else if (pStrmCap->RecToVinBits[pInfo->RecID] == 0U) {
        PRN_RENC_TSK_LOG "Ignore RecID(%d) stream capture!" PRN_RENC_TSK_DBG
    } else {
        UINT32              PRetVal;
        UINT32              VinID;
        UINT64              RawSeq = 0U;
        RAW_ENC_STRM_CAP_QUE_UNIT_s CurUnit;

        PRetVal = pStrmCap->RecFrameCnt[pInfo->RecID] + 1U;
        PRN_RENC_TSK_LOG "%s[ACAP] Capture AAA data by %02d, %04d, 0x%016llX%s, %6d -> %6d"
            PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_STRM_CAP PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_UINT32 pInfo->RecID                 PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_UINT32 pInfo->FrmNo                 PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_UINT64 pInfo->CaptureTimeStamp
            PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_END      PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_UINT32 pStrmCap->RecFrameCnt[pInfo->RecID] PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_UINT32 PRetVal                             PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_DBG1

        pStrmCap->RecFrameCnt[pInfo->RecID] += 1U;

        AmbaSvcWrap_MisraMemset(&CurUnit, 0, sizeof(CurUnit));
        PRetVal = AmbaKAL_MsgQueueReceive(&(pStrmCap->FreeQue), &CurUnit, AMBA_KAL_NO_WAIT);
        if (PRetVal != 0U) {
            PRN_RENC_TSK_LOG "Fail to cap data - get free buffer fail! ErrCode(0x%08X)"
                PRN_RENC_TSK_ARG_UINT32 PRetVal PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_NG
        } else if (CurUnit.pInfo == NULL) {
            PRN_RENC_TSK_LOG "Fail to cap data - invalid unit info" PRN_RENC_TSK_NG
        } else {
            CurUnit.State            = RAW_ENC_STRM_3A_DATA_DATA;
            CurUnit.pInfo->RecID     = pInfo->RecID;
            CurUnit.pInfo->FrameNo   = pInfo->FrmNo;
            CurUnit.pInfo->TimeStamp = pInfo->CaptureTimeStamp;
            CurUnit.pInfo->VinID     = pStrmCap->RecToVinBits[CurUnit.pInfo->RecID];     // vin select bits
            CurUnit.pInfo->FovID     = pStrmCap->RecToFovBits[CurUnit.pInfo->RecID];     // fov select bits

            for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
                if ((pStrmCap->RecToVinBits[pInfo->RecID] & SvcRawEnc_BitGet(VinID)) > 0U) {

                    PRN_RENC_TSK_LOG "%s[ACAP] Try to get RawSeq by VinID(%02d), CaptureTimeStamp(0x%016llX)%s"
                        PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_STRM_CAP PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_ARG_UINT32 VinID                        PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_ARG_UINT64 pInfo->CaptureTimeStamp
                        PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_END      PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_DBG1

                    RawSeq = 0U;
                    RawEncTask_StrmCapScannerProc(VinID, pInfo->CaptureTimeStamp, &RawSeq);

                    PRN_RENC_TSK_LOG "%s[ACAP] RawSeq(0x%016llX)%s"
                        PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_STRM_CAP PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_ARG_UINT64 RawSeq
                        PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_END      PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_DBG1

                    CurUnit.pInfo->RawSeq[VinID] = RawSeq;
                }
            }

            if (pStrmCap->RecFrameCnt[pInfo->RecID] >= pStrmCap->RecSplitCnt[pInfo->RecID]) {
                if (pInfo->FrmType == PIC_FRAME_IDR) {
                    CurUnit.State |= RAW_ENC_STRM_3A_DATA_SPLIT;
                    pStrmCap->RecFrameCnt[pInfo->RecID] = 1U;
                }
            }
            PRetVal = AmbaKAL_MsgQueueSend(&(pStrmCap->DataQue), &CurUnit, AMBA_KAL_NO_WAIT);
            if (PRetVal != 0U) {
                PRN_RENC_TSK_LOG "Fail to cap data - write to data queue fail! ErrCode(0x%08X)"
                    PRN_RENC_TSK_ARG_UINT32 PRetVal PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_NG
            } else {
                PRN_RENC_TSK_LOG "%s[CAP] Success to write RecID(%02d) user cap info to data queue!%s"
                    PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_STRM_CAP PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_ARG_UINT32 pInfo->RecID                 PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_END      PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_DBG1

                PRetVal = AmbaKAL_EventFlagSet(&(RawEncCtrl.Flag), SVC_RAW_ENC_CTRL_FLG_FIFO_PROC);
                if (PRetVal != 0U) {
                    PRN_RENC_TSK_LOG "Fail to cap data - set fifo flag! ErrCode(0x%08X)"
                        PRN_RENC_TSK_ARG_UINT32 PRetVal PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_NG
                }
            }
        }

        PRN_RENC_TSK_LOG "%s[ACAP] Capture AAA data done%s"
            PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_STRM_CAP PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_END      PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_DBG1
    }
}

static void RawEncTask_StrmCapUserFileOpen(const RAW_ENC_STRM_CAP_QUE_UNIT_s *pUnit)
{
    if (pUnit == NULL) {
        PRN_RENC_TSK_LOG "Fail to create user text file - invalid input" PRN_RENC_TSK_NG
    } else if (pUnit->pInfo == NULL) {
        PRN_RENC_TSK_LOG "Fail to create user text file - invalid unit info" PRN_RENC_TSK_NG
    } else if (pUnit->pInfo->RecID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
        PRN_RENC_TSK_LOG "Fail to create user text file - invalid RecID(%d)"
            PRN_RENC_TSK_ARG_UINT32 pUnit->pInfo->RecID PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_NG
    } else {
        UINT32 ErrCode;
        RAW_ENC_STRM_CAP_CTRL_s *pCapCtrl = &(RawEncCtrl.StrmCap);
        RAW_ENC_STRM_CAP_FILE_s *pFileCtrl = &(pCapCtrl->AaaFile[pUnit->pInfo->RecID]);

        char *pFilePath, *pChar;

        // If the specify RecID user text file is not created, create it and write the header first.
        if ((pFileCtrl->State & RAW_ENC_STRM_CAP_FILE_OPEN) == 0U) {
            pFilePath = pFileCtrl->FilePath;

            if ((pUnit->State & RAW_ENC_STRM_3A_DATA_SPLIT) == 0U) {
                AmbaSvcWrap_MisraMemset(pFilePath, 0, RAW_ENC_STRM_CAP_TEMP_STR_SIZE);
            }
            ErrCode = RawEncTask_GetStrmCapFileName(pUnit->pInfo->RecID, pFilePath, RAW_ENC_STRM_CAP_TEMP_STR_SIZE, 5008U);
            if (ErrCode == 0U) {
                PRN_RENC_TSK_LOG "Get stream filename. RecID(%d) -> '%s'"
                    PRN_RENC_TSK_ARG_UINT32 pUnit->pInfo->RecID PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_ARG_CSTR   pFilePath           PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_DBG

                pChar = SvcWrap_strrchr(pFilePath, RAW_ENC_ASCII_DOT);
                if (pChar != NULL) {
                    *pChar = '\0';
                }
                AmbaUtility_StringAppend(pFilePath, RAW_ENC_STRM_CAP_TEMP_STR_SIZE, "_user.txt");

                AmbaSvcWrap_MisraMemset(&(pFileCtrl->VfsFile), 0, sizeof(AMBA_VFS_FILE_s));
                ErrCode = AmbaVFS_Open(pFilePath, "w", 1U, &(pFileCtrl->VfsFile));
                if (ErrCode != 0U) {
                    PRN_RENC_TSK_LOG "Fail to create user text file! ErrCode(0x%08X) %s"
                        PRN_RENC_TSK_ARG_UINT32 ErrCode   PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_ARG_CSTR   pFilePath PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_NG
                } else if (pFileCtrl->VfsFile.pFile == NULL) {
                    PRN_RENC_TSK_LOG "Fail to create user text file - invalid file pointer!" PRN_RENC_TSK_NG
                } else {
                    pFileCtrl->State |= RAW_ENC_STRM_CAP_FILE_OPEN;

                    PRN_RENC_TSK_LOG "%s[FIFO] Success to create RecID(%d) user text file %s %s"
                        PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_FIFO PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_ARG_UINT32 pUnit->pInfo->RecID      PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_ARG_CSTR   pFilePath                PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_END  PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_DBG1

                    pCapCtrl->FileCnt ++;
                }
            }
        }
    }
}

static void RawEncTask_StrmCapUserFileClose(const RAW_ENC_STRM_CAP_QUE_UNIT_s *pUnit)
{
    if (pUnit == NULL) {
        PRN_RENC_TSK_LOG "Fail to close user text file - invalid input" PRN_RENC_TSK_NG
    } else if (pUnit->pInfo == NULL) {
        PRN_RENC_TSK_LOG "Fail to close user text file - invalid unit info" PRN_RENC_TSK_NG
    } else if (pUnit->pInfo->RecID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
        PRN_RENC_TSK_LOG "Fail to close user text file - invalid RecID(%d)"
            PRN_RENC_TSK_ARG_UINT32 pUnit->pInfo->RecID PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_NG
    } else {
        RAW_ENC_STRM_CAP_CTRL_s *pCapCtrl = &(RawEncCtrl.StrmCap);
        RAW_ENC_STRM_CAP_FILE_s *pFileCtrl = &(pCapCtrl->AaaFile[pUnit->pInfo->RecID]);
        UINT32 ErrCode;

        if ((pFileCtrl->State & RAW_ENC_STRM_CAP_FILE_OPEN) == 0U) {
            PRN_RENC_TSK_LOG "[FIFO] The RecID(%d) user text file did not create"
                PRN_RENC_TSK_ARG_UINT32 pUnit->pInfo->RecID PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_DBG
        } else {

            ErrCode = AmbaVFS_Sync(&(pFileCtrl->VfsFile));
            if (ErrCode != 0U) {
                PRN_RENC_TSK_LOG "Fail to close user text file - file sync fail! ErrCode(0x%08X)"
                    PRN_RENC_TSK_ARG_UINT32 ErrCode PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_NG
            }

            ErrCode = AmbaVFS_Close(&(pFileCtrl->VfsFile));
            if (ErrCode != 0U) {
                PRN_RENC_TSK_LOG "Fail to close user text file - close user text fail! ErrCode(0x%08X)"
                    PRN_RENC_TSK_ARG_UINT32 ErrCode PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_NG
            } else {
                pFileCtrl->State &= ~RAW_ENC_STRM_CAP_FILE_OPEN;

                PRN_RENC_TSK_LOG "[FIFO] Success to save RecID(%d) user text file - %s"
                    PRN_RENC_TSK_ARG_UINT32 pUnit->pInfo->RecID PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_ARG_CSTR   pFileCtrl->FilePath PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_API

                AmbaSvcWrap_MisraMemset(&(pFileCtrl->VfsFile), 0, sizeof(AMBA_VFS_FILE_s));
            }
        }

    }
}

static void RawEncTask_StrmCapUserFileWrite(const RAW_ENC_STRM_CAP_QUE_UNIT_s *pUnit)
{
    UINT32 ErrCode;

    if (pUnit == NULL) {
        PRN_RENC_TSK_LOG "Fail to proc user write - invalid input" PRN_RENC_TSK_NG
    } else if (pUnit->pInfo == NULL) {
        PRN_RENC_TSK_LOG "Fail to proc user write - invalid unit info" PRN_RENC_TSK_NG
    } else if (pUnit->pInfo->RecID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
        PRN_RENC_TSK_LOG "Fail to proc user write - invalid RecID(%d)"
            PRN_RENC_TSK_ARG_UINT32 pUnit->pInfo->RecID PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_NG
    } else if (pRawEncTskUserCallBack != NULL) {
        const RAW_ENC_STRM_CAP_CTRL_s *pCapCtrl = &(RawEncCtrl.StrmCap);
        const RAW_ENC_STRM_CAP_FILE_s *pFileCtrl = &(pCapCtrl->AaaFile[pUnit->pInfo->RecID]);
        if (pFileCtrl->VfsFile.pFile == NULL) {
            PRN_RENC_TSK_LOG "Fail to proc user write - RecID(%d) file does not ready to write data"
                PRN_RENC_TSK_ARG_UINT32 pUnit->pInfo->RecID PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_NG
        } else {
            UINT32 CurTimeTick_S = 0U, CurTimeTick_E = 0U, CurTimeDiff;
            SVC_RAW_ENC_STRM_CAP_USER_s CurUserData;

            AmbaSvcWrap_MisraMemset(&CurUserData, 0, sizeof(CurUserData));
            CurUserData.RecID        = pUnit->pInfo->RecID;
            CurUserData.FrameCnt     = pUnit->pInfo->FrameNo;
            CurUserData.VinBits      = pUnit->pInfo->VinID;
            CurUserData.FovBits      = pUnit->pInfo->FovID;
            CurUserData.CapTimeStamp = pUnit->pInfo->TimeStamp;
            AmbaSvcWrap_MisraMemcpy(CurUserData.RawSeqCnt, pUnit->pInfo->RawSeq, sizeof(CurUserData.RawSeqCnt));
            CurUserData.FileCnt      = pCapCtrl->FileCnt;

            PRN_RENC_TSK_LOG "%s[FIFO] Call user callback to write data ... %s"
                PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_FIFO  PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_END   PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_DBG1

            ErrCode = AmbaKAL_GetSysTickCount(&CurTimeTick_S); PRN_RENC_TSK_ERR_HDLR
            pRawEncTskUserCallBack(&(pFileCtrl->VfsFile), &CurUserData);
            ErrCode = AmbaKAL_GetSysTickCount(&CurTimeTick_E); PRN_RENC_TSK_ERR_HDLR

            if (CurTimeTick_E >= CurTimeTick_S) {
                CurTimeDiff = CurTimeTick_E - CurTimeTick_S;
            } else {
                CurTimeDiff = ( 0xFFFFFFFFU - CurTimeTick_S ) + CurTimeTick_E;
            }

            PRN_RENC_TSK_LOG "%s[FIFO] Call user callback to write data ... Done. Spending Time: %d ms %s"
                PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_FIFO  PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_ARG_UINT32 CurTimeDiff               PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_ARG_CSTR   SVC_LOG_RENC_TSK_HL_END   PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_DBG1
        }

    } else {
        // misra-c
    }
}

static void RawEncTask_StrmCapSaveBinFile(const char *pFileName, void *pData, UINT32 DataSize)
{
    UINT32 ErrCode;

    if ((pFileName != NULL) && (pData != NULL)) {
        UINT32 ReqSize, NumSuccess;
        AMBA_FS_FILE *pFile = NULL;

        ErrCode = AmbaFS_FileOpen(pFileName, "wb", &pFile);
        if (ErrCode != 0U) {
            PRN_RENC_TSK_LOG "Fail to save binary file - create file fail! ErrCode(0x%08X)"
                PRN_RENC_TSK_ARG_UINT32 ErrCode PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_NG
        } else if (pFile == NULL) {
            PRN_RENC_TSK_LOG "Fail to save binary file - invalid file pointer!" PRN_RENC_TSK_NG
        } else {
            ReqSize = DataSize;
            NumSuccess = 0U;
            ErrCode = AmbaFS_FileWrite(pData, 1U, ReqSize, pFile, &NumSuccess);
            if (ErrCode != 0U) {
                PRN_RENC_TSK_LOG "Fail to save binary file - write data fail! ErrCode(0x%08X)"
                    PRN_RENC_TSK_ARG_UINT32 ErrCode PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_NG
            } else if (ReqSize != NumSuccess) {
                PRN_RENC_TSK_LOG "Fail to save binary file - write size(0x%X) does not same with request size(0x%X)"
                    PRN_RENC_TSK_ARG_UINT32 NumSuccess PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_ARG_UINT32 ReqSize    PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_NG
            } else {
                PRN_RENC_TSK_LOG "Success to save data to %s"
                    PRN_RENC_TSK_ARG_CSTR pFileName PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_OK
            }

            if (0U != AmbaFS_FileClose(pFile)) {
                PRN_RENC_TSK_LOG "Fail to save binary file - close file fail!" PRN_RENC_TSK_NG
            }
        }
    }
}

static void RawEncTask_StrmCapPostProc(const char *pSeqFilePath)
{
    if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_STRM_CAP_POST_TEXT) > 0U) {
        RawEncTask_StrmCapPostText(pSeqFilePath);
    }

    if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_STRM_CAP_SEPERATE) > 0U) {
        RawEncTask_StrmCapPostSeperate(pSeqFilePath);
    }
}

static void RawEncTask_StrmCapPostSeperate(const char *pSeqFilePath)
{
    UINT32 ErrCode = 0U;

    if (pSeqFilePath == NULL) {
        PRN_RENC_TSK_LOG "Fail to post proc seperate file - input sequence file path should not null" PRN_RENC_TSK_NG
    } else {
        AMBA_FS_FILE *pSeqFile = NULL;
        char SeqFilePath[64U];
        SVC_WRAP_SNPRINT_s LocalSnPrint;

        AmbaSvcWrap_MisraMemset(SeqFilePath, 0, sizeof(SeqFilePath));
        SvcWrap_strcpy(SeqFilePath, sizeof(SeqFilePath), pSeqFilePath);

        ErrCode = AmbaFS_FileOpen(SeqFilePath, "rb", &pSeqFile);
        if (ErrCode != 0U) {
            PRN_RENC_TSK_LOG "Fail to handle post proc - open aaa sequence file fail! ErrCode(0x%08X)"
                PRN_RENC_TSK_ARG_UINT32 ErrCode PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_NG
        } else if (pSeqFile == NULL) {
            PRN_RENC_TSK_LOG "Fail to handle post proc - invalid seq file pointer!" PRN_RENC_TSK_NG
        } else {
            RAW_ENC_STRM_CAP_SEQ_HEADER_s CapSeqHeader;
            RAW_ENC_STRM_CAP_SEQ_UNIT_s   CapSeqData;
            UINT32 ReqSize, NumSuccess;
            char FilePath[64U];

            // Read sequence file header
            AmbaSvcWrap_MisraMemset(&CapSeqHeader, 0, sizeof(CapSeqHeader));
            ReqSize = (UINT32)sizeof(CapSeqHeader);
            NumSuccess = 0U;
            ErrCode = AmbaFS_FileRead(&CapSeqHeader, 1U, ReqSize, pSeqFile, &NumSuccess);
            if (ErrCode != 0U) {
                PRN_RENC_TSK_LOG "Fail to handle post proc - read seq file header fail! ErrCode(0x%08X)"
                    PRN_RENC_TSK_ARG_UINT32 ErrCode PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_NG
            } else if (ReqSize != NumSuccess) {
                PRN_RENC_TSK_LOG "Fail to handle post proc - read header size(0x%X) does not same with request size(0x%X)"
                    PRN_RENC_TSK_ARG_UINT32 NumSuccess PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_ARG_UINT32 ReqSize    PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_NG
            } else {

                while (0U == AmbaFS_FileEof(pSeqFile)) {
                    AmbaSvcWrap_MisraMemset(&CapSeqData, 0, sizeof(CapSeqData));
                    ReqSize = (UINT32) sizeof(CapSeqData);
                    NumSuccess = 0U;
                    ErrCode = AmbaFS_FileRead(&CapSeqData, 1U, ReqSize, pSeqFile, &NumSuccess);
                    if (ErrCode != 0U) {
                        PRN_RENC_TSK_LOG "Fail to handle post proc - read seq frame fail! ErrCode(0x%08X)"
                            PRN_RENC_TSK_ARG_UINT32 ErrCode PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_NG
                    } else if (ReqSize != NumSuccess) {
                        ErrCode = 0xFFFFFFFFU;
                        PRN_RENC_TSK_LOG "Fail to handle post proc - read frame size(0x%X) does not same with request size(0x%X)"
                            PRN_RENC_TSK_ARG_UINT32 NumSuccess PRN_RENC_TSK_ARG_POST
                            PRN_RENC_TSK_ARG_UINT32 ReqSize    PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_NG
                    } else {
                        const char *pPrefix = RAW_ENC_STRM_CAP_PREFIX_PATH;

                        if ((CapSeqData.UpdBits & SvcRawEnc_BitGet(RAW_ENC_STRM_CAP_UPD_AAA)) > 0U) {
                            AmbaSvcWrap_MisraMemset(FilePath, 0, sizeof(FilePath));
                            AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                                LocalSnPrint.pStrFmt = "%saaa_%02d_%02d_%04d.bin";
                                LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr  = pPrefix;                      LocalSnPrint.Argc ++;
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(CapSeqHeader.VinID); LocalSnPrint.Argc ++;
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(CapSeqHeader.FovID); LocalSnPrint.Argc ++;
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(CapSeqData.FrameNo); LocalSnPrint.Argc ++;
                            RawEncTask_snprintf(FilePath, (UINT32)sizeof(FilePath), &LocalSnPrint);

                            RawEncTask_StrmCapSaveBinFile(FilePath, &(CapSeqData.AaaData), (UINT32)sizeof(SVC_RAW_CAP_AAA_DATA_s));
                        }

                        if ((CapSeqData.UpdBits & SvcRawEnc_BitGet(RAW_ENC_STRM_CAP_UPD_FOV_PIPE)) > 0U) {
                            AmbaSvcWrap_MisraMemset(FilePath, 0, sizeof(FilePath));
                            AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                                LocalSnPrint.pStrFmt = "%sfov_pipe_%02d_%02d_%04d.bin";
                                LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr  = pPrefix;                      LocalSnPrint.Argc ++;
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(CapSeqHeader.VinID); LocalSnPrint.Argc ++;
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(CapSeqHeader.FovID); LocalSnPrint.Argc ++;
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(CapSeqData.FrameNo); LocalSnPrint.Argc ++;
                            RawEncTask_snprintf(FilePath, (UINT32)sizeof(FilePath), &LocalSnPrint);

                            RawEncTask_StrmCapSaveBinFile(FilePath, &(CapSeqData.FovPipeData), (UINT32)sizeof(SVC_IMG_ALGO_FOV_PIPE_s));
                        }
                    }

                    if (ErrCode != 0U) {
                        break;
                    }
                }
            }

            if (0U != AmbaFS_FileClose(pSeqFile)) {
                PRN_RENC_TSK_LOG "Fail to handle post proc - close file fail!" PRN_RENC_TSK_NG
            }

            if (ErrCode == 0U) {
                if (0U != AmbaFS_Remove(SeqFilePath)) {
                    PRN_RENC_TSK_LOG "Fail to handle post proc - remove file fail! %s"
                        PRN_RENC_TSK_ARG_CSTR SeqFilePath PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_NG
                }
            }
        }
    }
}

static void RawEncTask_StrmCapWriteUINT(AMBA_FS_FILE *pFile, UINT32 AlignSize, const char *pFmt, UINT64 Val_0, UINT64 Val_1, UINT64 Val_2)
{
    if ((pFile != NULL) && (pFmt != NULL)) {
        char StrBuf[64];

        if (AlignSize > ((UINT32)sizeof(StrBuf) - 1U)) {
            UINT32 TmpU32 = (UINT32)sizeof(StrBuf) - 1U;
            PRN_RENC_TSK_LOG "Fail to write string with uint - align size(%d) is out-of limitation(%d)"
                PRN_RENC_TSK_ARG_UINT32 AlignSize PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_ARG_UINT32 TmpU32    PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_NG
        } else {
            SVC_WRAP_SNPRINT_s LocalSnPrint;
            UINT32 CurAlignSize, RetStrLen;

            AmbaSvcWrap_MisraMemset(StrBuf, 0, sizeof(StrBuf));
            AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                LocalSnPrint.pStrFmt = pFmt;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = Val_0; LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = Val_1; LocalSnPrint.Argc ++;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = Val_2; LocalSnPrint.Argc ++;
            RetStrLen = SvcWrap_snprintf(StrBuf, (UINT32)sizeof(StrBuf), &LocalSnPrint);
            if (RetStrLen < AlignSize) {
                char *pChar;
                CurAlignSize = AlignSize - RetStrLen;
                pChar = &(StrBuf[SvcWrap_strlen(StrBuf)]);
                AmbaSvcWrap_MisraMemset(pChar, 32, CurAlignSize);   // ASCII 32 = ' '
                pChar[CurAlignSize] = '\0';
            }

            RawEncTask_fprintf(pFile, StrBuf);
        }
    }
}

static void RawEncTask_StrmCapWriteFLOAT(AMBA_FS_FILE *pFile, UINT32 AlignSize, const char *pFmt, DOUBLE Val)
{
    if ((pFile != NULL) && (pFmt != NULL)) {
        char StrBuf[64];
        UINT32 TmpU32 = (UINT32)sizeof(StrBuf) - 1U;

        if (AlignSize > TmpU32) {
            PRN_RENC_TSK_LOG "Fail to write string with uint - align size(%d) is out-of limitation(%d)"
                PRN_RENC_TSK_ARG_UINT32 AlignSize PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_ARG_UINT32 TmpU32    PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_NG
        } else {
            SVC_WRAP_SNPRINT_s LocalSnPrint;
            UINT32 CurAlignSize, RetStrLen;

            AmbaSvcWrap_MisraMemset(StrBuf, 0, sizeof(StrBuf));
            AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                LocalSnPrint.pStrFmt = pFmt;
                LocalSnPrint.Argv[LocalSnPrint.Argc].Doubld = Val; LocalSnPrint.Argc ++;
            RetStrLen = SvcWrap_snprintf(StrBuf, (UINT32)sizeof(StrBuf), &LocalSnPrint);
            if (RetStrLen < AlignSize) {
                char *pChar;
                CurAlignSize = AlignSize - RetStrLen;
                pChar = &(StrBuf[SvcWrap_strlen(StrBuf)]);
                AmbaSvcWrap_MisraMemset(pChar, 32, CurAlignSize);   // ASCII 32 = ' '
                pChar[CurAlignSize] = '\0';
            }

            RawEncTask_fprintf(pFile, StrBuf);
        }
    }
}

static void RawEncTask_StrmCapPostText(const char *pSeqFilePath)
{
    UINT32 ErrCode = 0U;

    if (pSeqFilePath == NULL) {
        PRN_RENC_TSK_LOG "Fail to post proc txt file - input sequence file path should not null" PRN_RENC_TSK_NG
    } else {
        AMBA_FS_FILE *pSeqFile = NULL, *pTxtFile = NULL;
        char TxtFilePath[64U], *pChar;
        RAW_ENC_STRM_CAP_SEQ_HEADER_s CapSeqHeader;
        RAW_ENC_STRM_CAP_SEQ_UNIT_s   CapSeqData;
        SVC_RAW_CAP_AAA_DATA_s        CapAAAData;
        UINT32 ReqSize, NumSuccess;

        AmbaSvcWrap_MisraMemset(&CapAAAData, 0, sizeof(CapAAAData));

        AmbaSvcWrap_MisraMemset(TxtFilePath, 0, sizeof(TxtFilePath));
        SvcWrap_strcpy(TxtFilePath, sizeof(TxtFilePath), pSeqFilePath);
        pChar = SvcWrap_strrchr(TxtFilePath, RAW_ENC_ASCII_DOT);   // ASCII '.'
        if (pChar == NULL) {
            pChar = &TxtFilePath[SvcWrap_strlen(TxtFilePath)];
        }
        pChar[0] = '.'; pChar[1] = 't'; pChar[2] = 'x'; pChar[3] = 't';

        ErrCode = AmbaFS_FileOpen(pSeqFilePath, "rb", &pSeqFile);
        if (ErrCode != 0U) {
            PRN_RENC_TSK_LOG "Fail to post proc txt file - open aaa sequence file fail! ErrCode(0x%08X)"
                PRN_RENC_TSK_ARG_UINT32 ErrCode PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_NG
        } else if (pSeqFile == NULL) {
            ErrCode = 0xFFFFFFFFU;
            PRN_RENC_TSK_LOG "Fail to post proc txt file - invalid seq file pointer!" PRN_RENC_TSK_NG
        } else {
            // Read sequence file header
            AmbaSvcWrap_MisraMemset(&CapSeqHeader, 0, sizeof(CapSeqHeader));
            ReqSize = (UINT32)sizeof(CapSeqHeader);
            NumSuccess = 0U;
            ErrCode = AmbaFS_FileRead(&CapSeqHeader, 1U, ReqSize, pSeqFile, &NumSuccess);
            if (ErrCode != 0U) {
                PRN_RENC_TSK_LOG "Fail to post proc txt file - read seq file header fail! ErrCode(0x%08X)"
                    PRN_RENC_TSK_ARG_UINT32 ErrCode PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_NG
            } else if (ReqSize != NumSuccess) {
                ErrCode = 0xFFFFFFFFU;
                PRN_RENC_TSK_LOG "Fail to post proc txt file - read header size(0x%X) does not same with request size(0x%X)"
                    PRN_RENC_TSK_ARG_UINT32 NumSuccess PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_ARG_UINT32 ReqSize    PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_NG
            } else {
                AmbaSvcWrap_MisraMemset(&CapSeqData, 0, sizeof(CapSeqData));
                ReqSize = (UINT32) sizeof(CapSeqData);
                NumSuccess = 0U;
                ErrCode = AmbaFS_FileRead(&CapSeqData, 1U, ReqSize, pSeqFile, &NumSuccess);
                if (ErrCode != 0U) {
                    PRN_RENC_TSK_LOG "Fail to post proc txt file - read seq frame fail! ErrCode(0x%08X)"
                        PRN_RENC_TSK_ARG_UINT32 ErrCode PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_NG
                } else if (ReqSize != NumSuccess) {
                    ErrCode = 0xFFFFFFFFU;
                    PRN_RENC_TSK_LOG "Fail to post proc txt file - read frame size(0x%X) does not same with request size(0x%X)"
                        PRN_RENC_TSK_ARG_UINT32 NumSuccess PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_ARG_UINT32 ReqSize    PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_NG
                } else {
                    AmbaSvcWrap_MisraMemcpy(&CapAAAData, &(CapSeqData.AaaData), sizeof(SVC_RAW_CAP_AAA_DATA_s));
                    PRN_RENC_TSK_LOG "Get 1st AAA data done" PRN_RENC_TSK_API
                }
            }

            if (0U != AmbaFS_FileClose(pSeqFile)) {
                PRN_RENC_TSK_LOG "Fail to post proc txt file - close file fail!" PRN_RENC_TSK_NG
            }
        }

        if (ErrCode == 0U) {
            ErrCode = AmbaFS_FileOpen(TxtFilePath, "wb", &pTxtFile);
            if (ErrCode != 0U) {
                PRN_RENC_TSK_LOG "Fail to post proc txt file - create txt file fail! ErrCode(0x%08X)"
                    PRN_RENC_TSK_ARG_UINT32 ErrCode PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_NG
            } else if (pTxtFile == NULL) {
                ErrCode = 0xFFFFFFFFU;
                PRN_RENC_TSK_LOG "Fail to post proc txt file - invalid txt file pointer!" PRN_RENC_TSK_NG
            } else {
                UINT32 Idx;
                const AMBA_AE_INFO_s *pCurAeInfo;

                pCurAeInfo = CapAAAData.AeInfo;

                RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "ExposureNum: %d"
                                           , (UINT64)(CapAAAData.ExposureNum), 0ULL, 0ULL);
                RawEncTask_fprintf(pTxtFile, "\n");

                for (Idx = 0U; Idx < (UINT32)SVC_RAW_CAP_AAA_MAX_EXP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "AEInfo[%d]", (UINT64)Idx, 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)SVC_RAW_CAP_AAA_MAX_EXP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "{", 0ULL, 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)SVC_RAW_CAP_AAA_MAX_EXP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  EvIndex: %d", (UINT64)(pCurAeInfo[Idx].EvIndex), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)SVC_RAW_CAP_AAA_MAX_EXP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  NfIndex: %d", (UINT64)(pCurAeInfo[Idx].NfIndex), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)SVC_RAW_CAP_AAA_MAX_EXP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  ShutterIndex: %d", (UINT64)(pCurAeInfo[Idx].ShutterIndex), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)SVC_RAW_CAP_AAA_MAX_EXP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  AgcIndex: %d", (UINT64)(pCurAeInfo[Idx].AgcIndex), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)SVC_RAW_CAP_AAA_MAX_EXP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  IrisIndex: %d", (UINT64)(pCurAeInfo[Idx].IrisIndex), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)SVC_RAW_CAP_AAA_MAX_EXP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  Dgain: %d", (UINT64)(pCurAeInfo[Idx].Dgain), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)SVC_RAW_CAP_AAA_MAX_EXP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  IsoValue: %d", (UINT64)(pCurAeInfo[Idx].IsoValue), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)SVC_RAW_CAP_AAA_MAX_EXP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  Flash: %d", (UINT64)(pCurAeInfo[Idx].Flash), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)SVC_RAW_CAP_AAA_MAX_EXP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  Mode: %d", (UINT64)(pCurAeInfo[Idx].Mode), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)SVC_RAW_CAP_AAA_MAX_EXP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteFLOAT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  ShutterTime: %f", (DOUBLE)(pCurAeInfo[Idx].ShutterTime));
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)SVC_RAW_CAP_AAA_MAX_EXP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteFLOAT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  AgcGain: %f", (DOUBLE)(pCurAeInfo[Idx].AgcGain));
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)SVC_RAW_CAP_AAA_MAX_EXP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  Target: %d", (UINT64)(pCurAeInfo[Idx].Target), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)SVC_RAW_CAP_AAA_MAX_EXP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  LumaStat: %d", (UINT64)(pCurAeInfo[Idx].LumaStat), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)SVC_RAW_CAP_AAA_MAX_EXP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  LimitStatus: %d", (UINT64)(pCurAeInfo[Idx].LimitStatus), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)SVC_RAW_CAP_AAA_MAX_EXP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteFLOAT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  SensorDgain: %f", (DOUBLE)(pCurAeInfo[Idx].SensorDgain));
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)SVC_RAW_CAP_AAA_MAX_EXP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteFLOAT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  HdrRatio: %f", (DOUBLE)(pCurAeInfo[Idx].HdrRatio));
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)SVC_RAW_CAP_AAA_MAX_EXP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  FogStrength: %d", (UINT64)(pCurAeInfo[Idx].FogStrength), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)SVC_RAW_CAP_AAA_MAX_EXP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "}", 0ULL, 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n\n");

                pCurAeInfo = CapAAAData.AaaInfo.BeforeHdrAEInfo;

                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "BeforeHdrAEInfo[%d]", (UINT64)Idx, 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "{", 0ULL, 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  EvIndex: %d", (UINT64)(pCurAeInfo[Idx].EvIndex), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  NfIndex: %d", (UINT64)(pCurAeInfo[Idx].NfIndex), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  ShutterIndex: %d", (UINT64)(pCurAeInfo[Idx].ShutterIndex), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  AgcIndex: %d", (UINT64)(pCurAeInfo[Idx].AgcIndex), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  IrisIndex: %d", (UINT64)(pCurAeInfo[Idx].IrisIndex), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  Dgain: %d", (UINT64)(pCurAeInfo[Idx].Dgain), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  IsoValue: %d", (UINT64)(pCurAeInfo[Idx].IsoValue), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  Flash: %d", (UINT64)(pCurAeInfo[Idx].Flash), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  Mode: %d", (UINT64)(pCurAeInfo[Idx].Mode), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteFLOAT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  ShutterTime: %f", (DOUBLE)(pCurAeInfo[Idx].ShutterTime));
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteFLOAT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  AgcGain: %f", (DOUBLE)(pCurAeInfo[Idx].AgcGain));
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  Target: %d", (UINT64)(pCurAeInfo[Idx].Target), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  LumaStat: %d", (UINT64)(pCurAeInfo[Idx].LumaStat), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  LimitStatus: %d", (UINT64)(pCurAeInfo[Idx].LimitStatus), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteFLOAT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  SensorDgain: %f", (DOUBLE)(pCurAeInfo[Idx].SensorDgain));
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteFLOAT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  HdrRatio: %f", (DOUBLE)(pCurAeInfo[Idx].HdrRatio));
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  FogStrength: %d", (UINT64)(pCurAeInfo[Idx].FogStrength), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "}", 0ULL, 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n\n");

                pCurAeInfo = CapAAAData.AaaInfo.AfterHdrAEInfo;

                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "AfterHdrAEInfo[%d]", (UINT64)Idx, 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "{", 0ULL, 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  EvIndex: %d", (UINT64)(pCurAeInfo[Idx].EvIndex), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  NfIndex: %d", (UINT64)(pCurAeInfo[Idx].NfIndex), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  ShutterIndex: %d", (UINT64)(pCurAeInfo[Idx].ShutterIndex), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  AgcIndex: %d", (UINT64)(pCurAeInfo[Idx].AgcIndex), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  IrisIndex: %d", (UINT64)(pCurAeInfo[Idx].IrisIndex), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  Dgain: %d", (UINT64)(pCurAeInfo[Idx].Dgain), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  IsoValue: %d", (UINT64)(pCurAeInfo[Idx].IsoValue), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  Flash: %d", (UINT64)(pCurAeInfo[Idx].Flash), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  Mode: %d", (UINT64)(pCurAeInfo[Idx].Mode), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteFLOAT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  ShutterTime: %f", (DOUBLE)(pCurAeInfo[Idx].ShutterTime));
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteFLOAT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  AgcGain: %f", (DOUBLE)(pCurAeInfo[Idx].AgcGain));
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  Target: %d", (UINT64)(pCurAeInfo[Idx].Target), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  LumaStat: %d", (UINT64)(pCurAeInfo[Idx].LumaStat), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  LimitStatus: %d", (UINT64)(pCurAeInfo[Idx].LimitStatus), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteFLOAT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  SensorDgain: %f", (DOUBLE)(pCurAeInfo[Idx].SensorDgain));
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteFLOAT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  HdrRatio: %f", (DOUBLE)(pCurAeInfo[Idx].HdrRatio));
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "  FogStrength: %d", (UINT64)(pCurAeInfo[Idx].FogStrength), 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n");
                for (Idx = 0U; Idx < (UINT32)MAX_EXP_GROUP_NUM; Idx ++) {
                    RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "}", 0ULL, 0ULL, 0ULL);
                }
                RawEncTask_fprintf(pTxtFile, "\n\n");

                RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "RGB WbGain: %6d/%6d/%6d"
                                           , (UINT64)(CapAAAData.AaaInfo.WbGain.GainR)
                                           , (UINT64)(CapAAAData.AaaInfo.WbGain.GainG)
                                           , (UINT64)(CapAAAData.AaaInfo.WbGain.GainB));
                RawEncTask_fprintf(pTxtFile, "\n");

                RawEncTask_StrmCapWriteFLOAT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "ShiftAeGain: %f", (DOUBLE)(CapAAAData.AaaInfo.ShiftAeGain));
                RawEncTask_fprintf(pTxtFile, "\n");

                RawEncTask_StrmCapWriteFLOAT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "BlendRatio: %f", (DOUBLE)(CapAAAData.AaaInfo.BlendRatio));
                RawEncTask_fprintf(pTxtFile, "\n");

                RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "MinBlendRatio: %d", (UINT64)(CapAAAData.AaaInfo.MinBlendRatio), 0ULL, 0ULL);
                RawEncTask_fprintf(pTxtFile, "\n");

                RawEncTask_StrmCapWriteUINT(pTxtFile, RAW_ENC_STRM_CAP_TEXT_ALIGN_SIZE, "MaxBlendRatio: %d", (UINT64)(CapAAAData.AaaInfo.MaxBlendRatio), 0ULL, 0ULL);
                RawEncTask_fprintf(pTxtFile, "\n");

                if (0U != AmbaFS_FileClose(pTxtFile)) {
                    PRN_RENC_TSK_LOG "Fail to post proc txt file - close txt file fail!" PRN_RENC_TSK_NG
                } else {
                    PRN_RENC_TSK_LOG "Success to dump txt file - %s"
                        PRN_RENC_TSK_ARG_CSTR TxtFilePath PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_API
                }
            }
        }
    }

    AmbaMisra_TouchUnused(&ErrCode);
}

void SvcRawEncTask_StrmCapCreate(UINT32 RecBits, UINT8 *pBuf, UINT32 BufSize)
{
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    SVC_USER_PREF_s *pCurPref = NULL;

    if ((RawEncTskCtrlFlg & (SVC_RAW_ENC_FLG_STRM_CAP_RAW_ENC | SVC_RAW_ENC_FLG_STRM_CAP_USER_CAP)) == 0U) {
        if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_MSG_DBG) > 0U) {
            PRN_RENC_TSK_LOG "Fail to create strm cap - set strm cap for 'raw enc' or 'user cap'" PRN_RENC_TSK_NG
        }
    } else if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_STRM_CAP_INIT) > 0U) {
        PRN_RENC_TSK_LOG "Video raw encode stream AAA capture has been created!" PRN_RENC_TSK_API
    } else if (pResCfg == NULL) {
        PRN_RENC_TSK_LOG "Fail to create strm cap - invalid res_cfg!" PRN_RENC_TSK_NG
    } else if (RecBits == 0U) {
        PRN_RENC_TSK_LOG "RecBits(0x%X) is zero!"
            PRN_RENC_TSK_ARG_UINT32 RecBits PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_API
    } else if (0U != SvcUserPref_Get(&pCurPref)) {
        PRN_RENC_TSK_LOG "Fail to create strm cap - get current user pref fail!" PRN_RENC_TSK_NG
    } else if (pCurPref == NULL) {
        PRN_RENC_TSK_LOG "Fail to create strm cap - invalid user pref fail!" PRN_RENC_TSK_NG
    } else {
        UINT32 PRetVal = SVC_OK, ErrCode;
        UINT8 *pCurBuf    = NULL;
        UINT32 CurBufSize = 0U;
        UINT32 CurBufType = 0xFFFFFFFFU;
        UINT32 IntoUnitSize;
        UINT32 BufUnitSize;
        UINT32 ReqBufSize;
        UINT8 *pCurTxtBuf = NULL;
        UINT32 CurTxtBufSize = 0U;
        UINT32 ReqTxtBufSize = 0U;
        ULONG  FreeBase;
        UINT32 FreeSize;
        SVC_WRAP_SNPRINT_s LocalSnPrint;

        IntoUnitSize = GetAlignedValU32((UINT32)sizeof(RAW_ENC_STRM_CAP_INFO_s), 64U);
        if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_STRM_CAP_RAW_ENC) > 0U) {
            // Get the request buffer size
            BufUnitSize = GetAlignedValU32((UINT32)sizeof(RAW_ENC_STRM_3A_DATA_s), 64U);
        } else {
            BufUnitSize = 0U;
        }
        ReqBufSize  = (IntoUnitSize + BufUnitSize) * RAW_ENC_STRM_CAP_BUF_NUM;

        FreeSize = RAW_ENC_STRM_CAP_BUF_NUM;
        PRN_RENC_TSK_LOG "Current request buffer size: 0x%X = ( 0x%X + 0x%X) x %d"
            PRN_RENC_TSK_ARG_UINT32 ReqBufSize   PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_UINT32 IntoUnitSize PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_UINT32 BufUnitSize  PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_ARG_UINT32 FreeSize     PRN_RENC_TSK_ARG_POST
        PRN_RENC_TSK_API

        if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_STRM_CAP_POST_TEXT) > 0U) {
            ReqTxtBufSize = (UINT32)sizeof(RAW_ENC_STRM_CAP_TXT_s);
            PRN_RENC_TSK_LOG "Current request txt buffer size: 0x%X"
                PRN_RENC_TSK_ARG_UINT32 ReqTxtBufSize  PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_API
        }

        if (pBuf != NULL) {
            if (BufSize < ReqBufSize) {
                PRetVal = SVC_NG;
                PRN_RENC_TSK_LOG "Fail to create strm cap - input buffer is smaller than request size. 0x%X < 0x%X"
                    PRN_RENC_TSK_ARG_UINT32 BufSize    PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_ARG_UINT32 ReqBufSize PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_NG
            } else {
                pCurBuf    = pBuf;
                CurBufSize = BufSize;
            }
        }

        if ((PRetVal == SVC_OK) && (pBuf == NULL)) {
            // Get free cache
            FreeBase = 0U; FreeSize = 0U;
            if (0U != SvcBuffer_LockFreeSpace(SVC_MEM_TYPE_CA, &FreeBase, &FreeSize)) {
                PRN_RENC_TSK_LOG "Fail to create strm cap - lock free cache buffer fail" PRN_RENC_TSK_NG
            } else {

                CurBufType = SVC_MEM_TYPE_CA;

                if (FreeSize >= ReqBufSize) {
                    CurBufSize = ReqBufSize;
                    AmbaMisra_TypeCast(&(pCurBuf), &(FreeBase));
                    PRN_RENC_TSK_LOG "Lock free cache buffer for recording aaa buffer: %p, 0x%X"
                        PRN_RENC_TSK_ARG_CPOINT pCurBuf    PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_ARG_UINT32 CurBufSize PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_API

                    FreeBase += CurBufSize;
                    FreeSize -= CurBufSize;
                }

                if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_STRM_CAP_POST_TEXT) > 0U) {
                    if (FreeSize >= ReqTxtBufSize) {
                        CurTxtBufSize = ReqTxtBufSize;
                        AmbaMisra_TypeCast(&(pCurTxtBuf), &(FreeBase));
                        PRN_RENC_TSK_LOG "Lock free cache buffer for covert txt buffer: %p, 0x%X"
                            PRN_RENC_TSK_ARG_CPOINT pCurTxtBuf    PRN_RENC_TSK_ARG_POST
                            PRN_RENC_TSK_ARG_UINT32 CurTxtBufSize PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_API
                    } else {
                        PRN_RENC_TSK_LOG "There is not free space to support covert txt process. free(0x%X), Request(0x%X)"
                            PRN_RENC_TSK_ARG_UINT32 FreeSize      PRN_RENC_TSK_ARG_POST
                            PRN_RENC_TSK_ARG_UINT32 ReqTxtBufSize PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_NG

                        RawEncTskCtrlFlg &= ~SVC_RAW_ENC_FLG_STRM_CAP_POST_TEXT;
                    }
                }
            }
        }

        if (pCurBuf == NULL) {
            PRetVal = SVC_NG;
            PRN_RENC_TSK_LOG "Fail to create strm cap - invalid aaa buffer fail!" PRN_RENC_TSK_NG
        } else if ((pCurTxtBuf == NULL) && ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_STRM_CAP_POST_TEXT) > 0U)) {
            PRetVal = SVC_NG;
            PRN_RENC_TSK_LOG "Fail to create strm cap - invalid txt buffer fail!" PRN_RENC_TSK_NG
        } else {
            RAW_ENC_STRM_CAP_CTRL_s *pCapCtrl = &(RawEncCtrl.StrmCap);
            UINT32 RecIdx, CurVinBits = 0U, CurFovBits = 0U, CurSplitCnt;
            UINT32 Idx, TmpBits, RecId;
            char ModuleName[32];

            AmbaSvcWrap_MisraMemset(pCapCtrl, 0, sizeof(RAW_ENC_STRM_CAP_CTRL_s));
            pCapCtrl->pBuf    = pCurBuf;
            pCapCtrl->BufSize = CurBufSize;
            pCapCtrl->BufType = CurBufType;

            AmbaSvcWrap_MisraMemset(pCapCtrl->FovToRecID, 255, sizeof(pCapCtrl->FovToRecID));

            for (RecIdx = 0U; RecIdx < pResCfg->RecNum; RecIdx ++) {
                RecId = pResCfg->RecStrm[RecIdx].RecSetting.RecId;

                if (RecId < AMBA_DSP_MAX_STREAM_NUM) {

                    if ((RecBits & SvcRawEnc_BitGet(RecId)) > 0U) {
                        pCapCtrl->RecSelectBits |= SvcRawEnc_BitGet(RecId);

                        CurSplitCnt  = pCurPref->FileSplitTimeMin;
                        CurSplitCnt *= 60U;
                        CurSplitCnt *= GetRoundUpValU32(pResCfg->RecStrm[RecIdx].RecSetting.FrameRate.TimeScale,
                                                        pResCfg->RecStrm[RecIdx].RecSetting.FrameRate.NumUnitsInTick);
                        if (CurSplitCnt > 0U) {
                            pCapCtrl->RecSplitCnt[RecIdx] = CurSplitCnt;
                        } else {
                            pCapCtrl->RecSplitCnt[RecIdx] = 0xFFFFFFFFU;
                        }

                        TmpBits = 0U;
                        SvcResCfg_GetVinBitsOfRecIdx(RecId, &TmpBits);
                        if (TmpBits > 0U) {
                            pCapCtrl->RecToVinBits[RecId] = CurVinBits ^ TmpBits;
                            CurVinBits |= TmpBits;
                        }

                        TmpBits = 0U;
                        SvcResCfg_GetFovBitsOfRecIdx(RecId, &TmpBits);
                        if (TmpBits > 0U) {
                            pCapCtrl->RecToFovBits[RecId] = CurFovBits ^ TmpBits;
                            CurFovBits |= TmpBits;

                            for (Idx = 0U; Idx < pResCfg->FovNum; Idx ++) {
                                if ((TmpBits & SvcRawEnc_BitGet(Idx)) > 0U) {
                                    if (pCapCtrl->FovToRecID[Idx] > pResCfg->RecNum) {
                                        pCapCtrl->FovToRecID[Idx] = RecIdx;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // Configure covert txt buffer
            if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_STRM_CAP_POST_TEXT) > 0U) {
                pCapCtrl->CvtTxtCtrl.MaxTxtBuf = RAW_ENC_STRM_CAP_MAX_REC_MINS;
                AmbaMisra_TypeCast(&(pCapCtrl->CvtTxtCtrl.pTxtBuf), &(pCurTxtBuf));
            }

            PRN_RENC_TSK_LOG " " PRN_RENC_TSK_DBG
            PRN_RENC_TSK_LOG "==== Video Stream AAA Capture ====" PRN_RENC_TSK_DBG
            PRN_RENC_TSK_LOG "    pBuf / Size : %p / 0x%X / %d"
                PRN_RENC_TSK_ARG_CPOINT pCapCtrl->pBuf    PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_ARG_UINT32 pCapCtrl->BufSize PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_ARG_UINT32 pCapCtrl->BufType PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_DBG
            if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_STRM_CAP_POST_TEXT) > 0U) {
                PRN_RENC_TSK_LOG "    pTxtBuf / Size : %p / 0x%X / %d"
                    PRN_RENC_TSK_ARG_CPOINT pCapCtrl->CvtTxtCtrl.pTxtBuf   PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_ARG_UINT32 CurTxtBufSize                  PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_ARG_UINT32 pCapCtrl->CvtTxtCtrl.MaxTxtBuf PRN_RENC_TSK_ARG_POST
                PRN_RENC_TSK_DBG
            }
            PRN_RENC_TSK_LOG "  RecSelectBits : 0x%X"
                PRN_RENC_TSK_ARG_UINT32 pCapCtrl->RecSelectBits PRN_RENC_TSK_ARG_POST
            PRN_RENC_TSK_DBG
            if (pCapCtrl->RecSelectBits > 0U) {
                PRN_RENC_TSK_LOG "  RecID | VinBits | FovBits | SplitCnt" PRN_RENC_TSK_DBG
                for (RecIdx = 0U; RecIdx < AMBA_DSP_MAX_STREAM_NUM; RecIdx ++) {
                    if ((pCapCtrl->RecSelectBits & SvcRawEnc_BitGet(RecIdx)) > 0U) {
                        PRN_RENC_TSK_LOG "     %02d | 0x%05X | 0x%05X | %d"
                            PRN_RENC_TSK_ARG_UINT32 RecIdx                         PRN_RENC_TSK_ARG_POST
                            PRN_RENC_TSK_ARG_UINT32 pCapCtrl->RecToVinBits[RecIdx] PRN_RENC_TSK_ARG_POST
                            PRN_RENC_TSK_ARG_UINT32 pCapCtrl->RecToFovBits[RecIdx] PRN_RENC_TSK_ARG_POST
                            PRN_RENC_TSK_ARG_UINT32 pCapCtrl->RecSplitCnt[RecIdx]  PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_DBG
                    }
                }

                PRN_RENC_TSK_LOG "  FovID | RecID" PRN_RENC_TSK_DBG
                for (Idx = 0U; Idx < pResCfg->FovNum; Idx ++) {
                    PRN_RENC_TSK_LOG "     %02d | %02d"
                        PRN_RENC_TSK_ARG_UINT32 Idx                       PRN_RENC_TSK_ARG_POST
                        PRN_RENC_TSK_ARG_UINT32 pCapCtrl->FovToRecID[Idx] PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_DBG
                }
            }

            // Create mutex
            for (Idx = 0U; Idx < AMBA_DSP_MAX_VIN_NUM; Idx ++) {

                AmbaSvcWrap_MisraMemset(ModuleName, 0, sizeof(ModuleName));
                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "SvcRawEnc_SeqMutex_%d";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 = (UINT64)(Idx); LocalSnPrint.Argc ++;
                RawEncTask_snprintf(ModuleName, (UINT32)sizeof(ModuleName), &LocalSnPrint);

                PRetVal = AmbaKAL_MutexCreate(&(pCapCtrl->RawSeqScanner[Idx].Mutex), ModuleName);
                if (PRetVal != 0U) {
                    PRN_RENC_TSK_LOG "Fail to create strm cap - create mutex fail! ErrCode(0x%08X)"
                        PRN_RENC_TSK_ARG_UINT32 PRetVal PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_NG
                    break;
                }
            }

            // Create free/data queue
            if (PRetVal == 0U) {

                AmbaSvcWrap_MisraMemset(ModuleName, 0, sizeof(ModuleName));
                SvcWrap_strcpy(ModuleName, sizeof(ModuleName), "SvcRawEnc_FreeQue");

                PRetVal = AmbaKAL_MsgQueueCreate(&(pCapCtrl->FreeQue),
                                                 ModuleName,
                                                 (UINT32)sizeof(pCapCtrl->FreeQueBuf[0]),
                                                 pCapCtrl->FreeQueBuf,
                                                 (UINT32)sizeof(pCapCtrl->FreeQueBuf));
                if (PRetVal != 0U) {
                    PRN_RENC_TSK_LOG "Fail to create strm cap - create free queue fail! ErrCode(0x%08X)"
                        PRN_RENC_TSK_ARG_UINT32 PRetVal PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_NG
                } else {
                    UINT32 BufIdx;
                    RAW_ENC_STRM_CAP_QUE_UNIT_s CurQueUnit;

                    ErrCode = AmbaKAL_MsgQueueFlush(&(pCapCtrl->FreeQue)); PRN_RENC_TSK_ERR_HDLR

                    for (BufIdx = 0U; BufIdx < RAW_ENC_STRM_CAP_BUF_NUM; BufIdx ++) {
                        if (pCapCtrl->BufSize >= (IntoUnitSize + BufUnitSize)) {
                            AmbaSvcWrap_MisraMemset(&CurQueUnit, 0, sizeof(CurQueUnit));

                            AmbaMisra_TypeCast(&(CurQueUnit.pInfo), &(pCapCtrl->pBuf));
                            pCapCtrl->pBuf = &(pCapCtrl->pBuf[IntoUnitSize]);
                            pCapCtrl->BufSize -= IntoUnitSize;

                            if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_STRM_CAP_RAW_ENC) > 0U) {
                                AmbaMisra_TypeCast(&(CurQueUnit.pData), &(pCapCtrl->pBuf));
                                pCapCtrl->pBuf = &(pCapCtrl->pBuf[BufUnitSize]);
                                pCapCtrl->BufSize -= BufUnitSize;
                            }

                            PRetVal = AmbaKAL_MsgQueueSend(&(pCapCtrl->FreeQue), &CurQueUnit, AMBA_KAL_NO_WAIT);
                            if (PRetVal != 0U) {
                                PRN_RENC_TSK_LOG "Fail to create strm cap - send buf unit to free queue fail! ErrCode(0x%08X)"
                                    PRN_RENC_TSK_ARG_UINT32 PRetVal PRN_RENC_TSK_ARG_POST
                                PRN_RENC_TSK_NG
                            }
                        } else {
                            PRetVal = SVC_NG;
                            PRN_RENC_TSK_LOG "Fail to create strm cap - there is not enough buffer for BufIdx(%d)"
                                PRN_RENC_TSK_ARG_UINT32 BufIdx PRN_RENC_TSK_ARG_POST
                            PRN_RENC_TSK_NG
                        }

                        if (PRetVal != SVC_OK) {
                            break;
                        }
                    }
                }
            }

            if (PRetVal == 0U) {

                AmbaSvcWrap_MisraMemset(ModuleName, 0, sizeof(ModuleName));
                SvcWrap_strcpy(ModuleName, sizeof(ModuleName), "SvcRawEnc_DataQue");

                PRetVal = AmbaKAL_MsgQueueCreate(&(pCapCtrl->DataQue),
                                                 ModuleName,
                                                 (UINT32)sizeof(pCapCtrl->DataQueBuf[0]),
                                                 pCapCtrl->DataQueBuf,
                                                 (UINT32)sizeof(pCapCtrl->DataQueBuf));
                if (PRetVal != 0U) {
                    PRN_RENC_TSK_LOG "Fail to create strm cap - create data queue fail! ErrCode(0x%08X)"
                        PRN_RENC_TSK_ARG_UINT32 PRetVal PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_NG
                } else {
                    ErrCode = AmbaKAL_MsgQueueFlush(&(pCapCtrl->DataQue)); PRN_RENC_TSK_ERR_HDLR
                }
            }
            if (PRetVal == 0U) {

                AmbaSvcWrap_MisraMemset(ModuleName, 0, sizeof(ModuleName));
                SvcWrap_strcpy(ModuleName, sizeof(ModuleName), "SvcRawEnc_MsgQue");

                PRetVal = AmbaKAL_MsgQueueCreate(&(pCapCtrl->MsgQue),
                                                 ModuleName,
                                                 (UINT32)sizeof(pCapCtrl->MsgQueBuf[0]),
                                                 pCapCtrl->MsgQueBuf,
                                                 (UINT32)sizeof(pCapCtrl->MsgQueBuf));
                if (PRetVal != 0U) {
                    PRN_RENC_TSK_LOG "Fail to create strm cap - create msg queue fail! ErrCode(0x%08X)"
                        PRN_RENC_TSK_ARG_UINT32 PRetVal PRN_RENC_TSK_ARG_POST
                    PRN_RENC_TSK_NG
                } else {
                    ErrCode = AmbaKAL_MsgQueueFlush(&(pCapCtrl->MsgQue)); PRN_RENC_TSK_ERR_HDLR
                }
            }

            // Create fifo task
            if (PRetVal == 0U) {
                pCapCtrl->MsgTaskCtrl.Priority   = SVC_RAW_ENC_TASK_PRI;
                pCapCtrl->MsgTaskCtrl.EntryFunc  = RawEncTask_StrmCapMsgTaskEntry;
                pCapCtrl->MsgTaskCtrl.EntryArg   = 0U;
                pCapCtrl->MsgTaskCtrl.pStackBase = pCapCtrl->MsgTaskStack;
                pCapCtrl->MsgTaskCtrl.StackSize  = (UINT32)sizeof(pCapCtrl->MsgTaskStack);
                pCapCtrl->MsgTaskCtrl.CpuBits    = SVC_RAW_ENC_TASK_CPU_BITS;
                PRetVal = SvcTask_Create("SvcRawEnc_MsgTask", &(pCapCtrl->MsgTaskCtrl));
                if (PRetVal != 0U) {
                    PRN_RENC_TSK_LOG "Fail to create strm cap - create msg task fail!" PRN_RENC_TSK_NG
                }
            }
            if (PRetVal == 0U) {
                pCapCtrl->FifoTaskCtrl.Priority   = SVC_RAW_ENC_TASK_PRI - 20U;
                pCapCtrl->FifoTaskCtrl.EntryFunc  = RawEncTask_StrmCapFifoTaskEntry;
                pCapCtrl->FifoTaskCtrl.EntryArg   = 0U;
                pCapCtrl->FifoTaskCtrl.pStackBase = pCapCtrl->FifoTaskStack;
                pCapCtrl->FifoTaskCtrl.StackSize  = (UINT32)sizeof(pCapCtrl->FifoTaskStack);
                pCapCtrl->FifoTaskCtrl.CpuBits    = SVC_RAW_ENC_TASK_CPU_BITS;
                PRetVal = SvcTask_Create("SvcRawEnc_FifoTask", &(pCapCtrl->FifoTaskCtrl));
                if (PRetVal != 0U) {
                    PRN_RENC_TSK_LOG "Fail to create strm cap - create fifo task fail!" PRN_RENC_TSK_NG
                }
            }

            // Register event handler
            if (PRetVal == 0U) {
                PRetVal = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_RAW_RDY, RawEncTask_StrmCapRawDataRdyHdlr);
                if (PRetVal != SVC_OK) {
                    PRN_RENC_TSK_LOG "Fail to create strm cap - register LV_RAW_RDY event fail!" PRN_RENC_TSK_NG
                }
            }
            if (PRetVal == 0U) {
                PRetVal = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_VIDEO_DATA_RDY, RawEncTask_StrmCapVidDataRdyHdlr);
                if (PRetVal != SVC_OK) {
                    PRN_RENC_TSK_LOG "Fail to create strm cap - register VIDEO_DATA_RDY event fail!" PRN_RENC_TSK_NG
                }
            }

            if (PRetVal == 0U) {
                PRN_RENC_TSK_LOG "Success to create video raw enc stream capture" PRN_RENC_TSK_OK
                RawEncTskCtrlFlg |= SVC_RAW_ENC_FLG_STRM_CAP_INIT;
            }
        }

        if (PRetVal != 0U) {
            if (CurBufType != 0xFFFFFFFFU) {
                ErrCode = SvcBuffer_UnLockFreeSpace(CurBufType); PRN_RENC_TSK_ERR_HDLR
                CurBufType = 0xFFFFFFFFU;
            }
        }

        AmbaMisra_TouchUnused(pCurTxtBuf);
        AmbaMisra_TouchUnused(&CurBufType);
    }
}

void SvcRawEncTask_StrmCapDelete(void)
{
    if ((RawEncTskCtrlFlg & (SVC_RAW_ENC_FLG_STRM_CAP_RAW_ENC | SVC_RAW_ENC_FLG_STRM_CAP_USER_CAP)) == 0U) {
        if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_MSG_DBG) > 0U) {
            PRN_RENC_TSK_LOG "Fail to create strm cap - set strm cap for 'raw enc' or 'user cap'" PRN_RENC_TSK_NG
        }
    } else if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_STRM_CAP_INIT) == 0U) {
        PRN_RENC_TSK_LOG "No need to delete Video raw encode stream AAA capture module!" PRN_RENC_TSK_DBG
    } else {
        UINT32 ErrCode;
        UINT32 ActualFlags = 0U;
        UINT32 Idx;
        UINT32 TimeOut = 1000U;
        RAW_ENC_STRM_CAP_CTRL_s *pCapCtrl = &(RawEncCtrl.StrmCap);

        // Un-register event handler
        ErrCode = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_LV_RAW_RDY, RawEncTask_StrmCapRawDataRdyHdlr); PRN_RENC_TSK_ERR_HDLR
        ErrCode = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_VIDEO_DATA_RDY, RawEncTask_StrmCapVidDataRdyHdlr); PRN_RENC_TSK_ERR_HDLR
        ErrCode = AmbaKAL_TaskSleep(50U); PRN_RENC_TSK_ERR_HDLR

        ErrCode = AmbaKAL_EventFlagGet(&(RawEncCtrl.Flag)
                                      , SVC_RAW_ENC_CTRL_FLG_MSG_IDLE | SVC_RAW_ENC_CTRL_FLG_FIFO_IDLE
                                      , AMBA_KAL_FLAGS_ALL
                                      , AMBA_KAL_FLAGS_CLEAR_NONE
                                      , &ActualFlags, (TimeOut - 50U)); PRN_RENC_TSK_ERR_HDLR

        // Destroy fifo task
        ErrCode = SvcTask_Destroy(&(pCapCtrl->FifoTaskCtrl)); PRN_RENC_TSK_ERR_HDLR
        ErrCode = SvcTask_Destroy(&(pCapCtrl->MsgTaskCtrl)); PRN_RENC_TSK_ERR_HDLR

        // Close file
        if (pCapCtrl->RecSelectBits > 0U) {
            RAW_ENC_STRM_CAP_INFO_s     CapInfo;
            RAW_ENC_STRM_CAP_QUE_UNIT_s CapUnit;

            if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_STRM_CAP_RAW_ENC) > 0U) {
                UINT32 FovIdx;

                for (Idx = 0U; Idx < AMBA_DSP_MAX_STREAM_NUM; Idx ++) {
                    if ((pCapCtrl->RecSelectBits & SvcRawEnc_BitGet(Idx)) > 0U) {
                        for (FovIdx = 0U; FovIdx < AMBA_DSP_MAX_VIEWZONE_NUM; FovIdx ++) {
                            if ((pCapCtrl->RecToFovBits[Idx] & SvcRawEnc_BitGet(FovIdx)) > 0U) {
                                AmbaSvcWrap_MisraMemset(&CapInfo, 0, sizeof(CapInfo));
                                CapInfo.FovID = FovIdx;
                                AmbaSvcWrap_MisraMemset(&CapUnit, 0, sizeof(CapUnit));
                                CapUnit.State = RAW_ENC_STRM_3A_DATA_EOS;
                                CapUnit.pInfo = &CapInfo;
                                if (0U == SvcResCfg_GetVinIDOfFovIdx(CapUnit.pInfo->FovID, &(CapUnit.pInfo->VinID))) {
                                    RawEncTask_StrmCapFileClose(&CapUnit);
                                }
                            }
                        }
                    }
                }
            } else {
                for (Idx = 0U; Idx < AMBA_DSP_MAX_STREAM_NUM; Idx ++) {
                    if ((pCapCtrl->RecSelectBits & SvcRawEnc_BitGet(Idx)) > 0U) {
                        AmbaSvcWrap_MisraMemset(&CapInfo, 0, sizeof(CapInfo));
                        CapInfo.RecID = Idx;
                        AmbaSvcWrap_MisraMemset(&CapUnit, 0, sizeof(CapUnit));
                        CapUnit.State = RAW_ENC_STRM_3A_DATA_EOS;
                        CapUnit.pInfo = &CapInfo;
                        RawEncTask_StrmCapUserFileClose(&CapUnit);
                    }
                }
            }
        }

        // Clear event flag
        ErrCode = AmbaKAL_EventFlagClear(&(RawEncCtrl.Flag), 0xFFFFFFFFU); PRN_RENC_TSK_ERR_HDLR

        // Delete free/data queue
        ErrCode = AmbaKAL_MsgQueueFlush(&(pCapCtrl->DataQue)); PRN_RENC_TSK_ERR_HDLR
        ErrCode = AmbaKAL_MsgQueueDelete(&(pCapCtrl->DataQue)); PRN_RENC_TSK_ERR_HDLR
        ErrCode = AmbaKAL_MsgQueueFlush(&(pCapCtrl->FreeQue));  PRN_RENC_TSK_ERR_HDLR
        ErrCode = AmbaKAL_MsgQueueDelete(&(pCapCtrl->FreeQue)); PRN_RENC_TSK_ERR_HDLR
        ErrCode = AmbaKAL_MsgQueueFlush(&(pCapCtrl->MsgQue));   PRN_RENC_TSK_ERR_HDLR
        ErrCode = AmbaKAL_MsgQueueDelete(&(pCapCtrl->MsgQue));  PRN_RENC_TSK_ERR_HDLR

        // Delete mutex
        for (Idx = 0U; Idx < AMBA_DSP_MAX_VIN_NUM; Idx ++) {
            ErrCode = AmbaKAL_MutexDelete(&(pCapCtrl->RawSeqScanner[Idx].Mutex)); PRN_RENC_TSK_ERR_HDLR
        }

        if (pCapCtrl->BufType != 0xFFFFFFFFU) {
            ErrCode = SvcBuffer_UnLockFreeSpace(pCapCtrl->BufType); PRN_RENC_TSK_ERR_HDLR
        }

        AmbaSvcWrap_MisraMemset(pCapCtrl, 0, sizeof(RAW_ENC_STRM_CAP_CTRL_s));

        PRN_RENC_TSK_LOG "Success to delete video raw enc stream capture!" PRN_RENC_TSK_OK
        RawEncTskCtrlFlg &= ~(SVC_RAW_ENC_FLG_STRM_CAP_INIT | SVC_RAW_ENC_FLG_STRM_CAP_SEPERATE);
    }
}

void SvcRawEncTask_StrmCapRegister(SVC_RAW_ENC_STRM_CAP_USER_f pFunc)
{
    if ((RawEncTskCtrlFlg & SVC_RAW_ENC_FLG_STRM_CAP_INIT) > 0U) {
        PRN_RENC_TSK_LOG "Please configure callback before start video stream capture." PRN_RENC_TSK_NG
    } else {
        pRawEncTskUserCallBack = pFunc;
    }
}

