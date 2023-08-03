/*
*  @file SvcRawCap.c
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
*  @details  raw sequence data capture functions
*
*/

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaSvcWrap.h"
#include "AmbaUtility.h"
#include "AmbaFS.h"
#include "AmbaSensor.h"
#include "AmbaCache.h"
#include "AmbaDSP_Liveview.h"
#include "bsp.h"

#include "AmbaDSP.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_StillCapture.h"
#include "AmbaDSP_ImageUtility.h"
#if defined(CONFIG_ICAM_IMGITUNER_USED)
#include "AmbaTUNE_HdlrManager.h"
#endif

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcMem.h"
#include "SvcPlat.h"
#include "SvcBuffer.h"
#include "SvcTask.h"
#include "SvcResCfg.h"
#include "SvcRawCap.h"

#define SVC_RAW_CAP_FLAG_INIT           (0x1U)
#define SVC_RAW_CAP_FLAG_CFG            (0x2U)
#define SVC_RAW_CAP_FLAG_CFG_MEM        (0x4U)
#define SVC_RAW_CAP_FLAG_CFG_RAW        (0x8U)
#define SVC_RAW_CAP_FLAG_CFG_HDS        (0x10U)
#define SVC_RAW_CAP_FLAG_CFG_AAA        (0x20U)
#define SVC_RAW_CAP_FLAG_MEM_SETUP      (0x40U)
#define SVC_RAW_CAP_FLAG_TASK_SETUP     (0x80U)
#define SVC_RAW_CAP_FLAG_CAP_SETUP      (0x100U)
#define SVC_RAW_CAP_FLAG_QUERY          (0x200U)
#define SVC_RAW_CAP_FLAG_CAP            (0x400U)
#define SVC_RAW_CAP_FLAG_CAP_INT_BUF    (0x800U)
#define SVC_RAW_CAP_FLAG_SHELL_INIT     (0x20000U)
#define SVC_RAW_CAP_FLAG_PRN_DSP_EVT    (0x40000U)
#define SVC_RAW_CAP_FLAG_MSG_OK         (0x1000000U)
#define SVC_RAW_CAP_FLAG_MSG_NG         (0x2000000U)
#define SVC_RAW_CAP_FLAG_MSG_API        (0x4000000U)
#define SVC_RAW_CAP_FLAG_MSG_DBG        (0x8000000U)
#define SVC_RAW_CAP_FLAG_MSG_DEF        (SVC_RAW_CAP_FLAG_MSG_OK | SVC_RAW_CAP_FLAG_MSG_NG | SVC_RAW_CAP_FLAG_MSG_API)

#define SVC_RAW_CAP_ITEM_ID_RAW    (0U)
#define SVC_RAW_CAP_ITEM_ID_HDS    (1U)
#define SVC_RAW_CAP_ITEM_ID_3A     (3U)
#define SVC_RAW_CAP_ITEM_BUF_DEPTH (610U)

#define SVC_RAW_CAP_MEM_USAGE_RAW       (0U)
#define SVC_RAW_CAP_MEM_USAGE_HDS       (1U)
#define SVC_RAW_CAP_MEM_USAGE_3A        (2U)
#define SVC_RAW_CAP_MEM_USAGE_BMP       (3U)
#define SVC_RAW_CAP_MEM_USAGE_UNPACK    (4U)
#define SVC_RAW_CAP_MEM_USAGE_RAW_TBL   (5U)
#define SVC_RAW_CAP_MEM_USAGE_HDS_TBL   (6U)
#define SVC_RAW_CAP_MEM_USAGE_NUM       (7U)

#define SVC_RAW_CAP_TASK_STACK_SZ  (0x4000U)
#define SVC_RAW_CAP_BUF_THRESHOLD  (10U)
#define SVC_RAW_CAP_MSG_QUE_DEPTH  (32U)
#define SVC_RAW_CAP_3A_MAX_CHAN    (32U)

#define SVC_RAW_CAP_ITEM_INIT   (0x1U)
#define SVC_RAW_CAP_ITEM_CFG    (0x2U)
#define SVC_RAW_CAP_ITEM_MEM    (0x4U)
#define SVC_RAW_CAP_ITEM_RDY    (0x8U)

#define SVC_RAW_CAP_VIN_RESOLUTION  (16U)

#ifdef IK_RAW_COMPACT_8B
#define SVC_RAW_CAP_VIN_COMPACT_BASE    IK_RAW_COMPACT_8B
#else
#define SVC_RAW_CAP_VIN_COMPACT_BASE    IK_RAW_COMPACT_10B
#endif

typedef struct {
    UINT32             State;
    char               FileName[64U];
    UINT32             IkSelectBits;
    UINT8             *pBuf;
    UINT32             BufSize;
} SVC_RAW_CAP_ITN_INFO_s;

typedef struct {
    UINT32             Pitch;
    UINT32             Width;
    UINT32             Height;
    UINT8             *pBuf;
    UINT32             VinType;
    UINT32             Bayer;
    UINT32             CmprType;
} SVC_RAW_CAP_RAW_INFO_s;

typedef struct {
    UINT32             ImgChanSelectBits;
    UINT32             ImgAaaBufOfs[SVC_RAW_CAP_3A_MAX_CHAN];
    UINT32             ImgAaaBufSize[SVC_RAW_CAP_3A_MAX_CHAN];
    UINT32             BufSize;
} SVC_RAW_CAP_3A_INFO_s;

typedef struct {
    UINT32                 State;
    char                   FileName[64U];
    SVC_RAW_CAP_RAW_INFO_s RawInfo;
    UINT32                 CurSeqIdx;
    UINT32                 NumCaptured;
    UINT32                 NumSeq;
    UINT8                **pBufSeqTbl;
    UINT32                 RawSeqArr[SVC_RAW_CAP_ITEM_BUF_DEPTH];
} SVC_RAW_CAP_ITEM_RAW_s;

typedef struct {
    UINT32             State;
    char               FileName[64U];
    SVC_RAW_CAP_INFO_s HdsInfo;
    UINT32             CurSeqIdx;
    UINT32             NumCaptured;
    UINT32             NumSeq;
    UINT8            **pBufSeqTbl;
    UINT32             RawSeqArr[SVC_RAW_CAP_ITEM_BUF_DEPTH];
} SVC_RAW_CAP_ITEM_HDS_s;

typedef struct {
    UINT32                State;
    char                  FileName[64U];
    SVC_RAW_CAP_3A_INFO_s AaaInfo;
    UINT32                CurSeqIdx;
    UINT32                NumCaptured;
    UINT32                NumSeq;
    UINT8                *pBufSeq[SVC_RAW_CAP_ITEM_BUF_DEPTH];
    UINT32                RawSeqArr[SVC_RAW_CAP_ITEM_BUF_DEPTH];
} SVC_RAW_CAP_ITEM_AAA_s;

#define SVC_RAW_CAP_MEM_SLOT_MAX    (32U)
typedef struct {
    UINT32 SlotID;
    UINT32 UsedStatus[SVC_RAW_CAP_MEM_USAGE_NUM];
    UINT8 *pUsedBuf;
    UINT32 UsedSize;
    UINT8 *pMemBuf;
    UINT32 MemSize;
    UINT8 *pMemAlgnedBuf;
    UINT32 MemAlgnedSize;
} SVC_RAW_CAP_MEM_SLOT_s;

typedef struct {
    UINT32                 NumSlot;
    SVC_RAW_CAP_MEM_SLOT_s MemSlot[SVC_RAW_CAP_MEM_SLOT_MAX];
} SVC_RAW_CAP_MEM_CTRL_s;

typedef struct {
    UINT32                  NumCapInstance;
    UINT16                  CapID[AMBA_DSP_MAX_VIN_NUM];
    AMBA_DSP_DATACAP_CFG_s  CapCfg[AMBA_DSP_MAX_VIN_NUM];
    AMBA_DSP_DATACAP_CTRL_s CapCtrl[AMBA_DSP_MAX_VIN_NUM];
    UINT32                  VinID2CapIdx[AMBA_DSP_MAX_VIN_NUM];
} SVC_RAW_CAP_CMD_s;

typedef struct {
    UINT32             State;
    UINT8             *pBuf;
    UINT32             BufSize;
} SVC_RAW_CAP_BMP_INFO_s;

typedef struct {
    UINT32             State;
    UINT8             *pBuf;
    UINT32             BufSize;
} SVC_RAW_CAP_UNPACK_INFO_s;

#pragma pack(1)
typedef struct {
    UINT16 Type;
    UINT32 Size;
    UINT32 reserved;
    UINT32 Offset;
} __attribute__((packed)) SVC_RAW_CAP_BITMAP_HEADER_s;

#pragma pack(1)
typedef struct {
    UINT32 Size;
    UINT32 Width;
    UINT32 Height;
    UINT16 Planes;
    UINT16 BitsPerPixel;
    UINT32 CmprType;
    UINT32 ImageSize;
    UINT32 Xresolution;
    UINT32 Yresolution;
    UINT32 NumOfColours;
    UINT32 ImportantColours;
} __attribute__((packed)) SVC_RAW_CAP_BITMAP_INFO_s;

#pragma pack(1)
typedef struct {
    struct {
        UINT8 Blue;
        UINT8 Green;
        UINT8 Red;
    } Pixel;
} __attribute__((packed)) SVC_RAW_CAP_BITMAP_RGB_s;

typedef struct {
    AMBA_DSP_RAW_DATA_RDY_s *pData;
} SVC_RAW_CAP_MSG_QUE_UNIT_s;

typedef struct {
    AMBA_KAL_EVENT_FLAG_t        Flag;
#define SVC_RAW_CAP_TSK_ON          (0x1U)
    AMBA_KAL_EVENT_FLAG_t        CapStartFlag;
    AMBA_KAL_EVENT_FLAG_t        CapRdyFlag;
    AMBA_KAL_MUTEX_t             Mutex;
    AMBA_KAL_MSG_QUEUE_t         MsgQue;
    SVC_RAW_CAP_MSG_QUE_UNIT_s   MsgQueBuf[SVC_RAW_CAP_MSG_QUE_DEPTH];
    AMBA_KAL_MSG_QUEUE_t         FreeQue;
    SVC_RAW_CAP_MSG_QUE_UNIT_s   FreeQueBuf[SVC_RAW_CAP_MSG_QUE_DEPTH];
    AMBA_DSP_RAW_DATA_RDY_s      QueDataBuf[SVC_RAW_CAP_MSG_QUE_DEPTH];
    UINT32                       VinSelectBits;
    UINT32                       ItmSelectBits;
    SVC_TASK_CTRL_s              TaskCtrl;
    UINT8                        TaskStack[SVC_RAW_CAP_TASK_STACK_SZ];
    SVC_RAW_CAP_ITEM_RAW_s       ItemCtrlRaw[AMBA_DSP_MAX_VIN_NUM];
    SVC_RAW_CAP_ITEM_HDS_s       ItemCtrlHds[AMBA_DSP_MAX_VIN_NUM];
    SVC_RAW_CAP_ITEM_AAA_s       ItemCtrlAaa[AMBA_DSP_MAX_VIN_NUM];
    SVC_RAW_CAP_MEM_CTRL_s       MemCtrl;
    SVC_RAW_CAP_ITN_INFO_s       ItnInfo;
    SVC_RAW_CAP_BMP_INFO_s       BmpInfo;
    SVC_RAW_CAP_UNPACK_INFO_s    UnPackInfo;
    SVC_RAW_CAP_CMD_s            CmdInfo;
    SVC_RAW_CAP_EXTRA_CAP_FUNC_f pExtraCapFunc;
} SVC_RAW_CAP_CTRL_s;

static inline UINT32 SvcRawCap_BitGet(UINT32 Val) { return ((Val < 32U) ? (UINT32)(0x1UL << Val) : 0U); }
static UINT32 SvcRawCap_MutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut);
static void   SvcRawCap_MutexGive(AMBA_KAL_MUTEX_t *pMutex);
static void   SvcRawCap_snprintf(char *pBuf, UINT32 BufSize, SVC_WRAP_SNPRINT_s *pFmt);
static UINT8 *SvcRawCap_MemAlign(const UINT8 *pMemBuf, UINT32 Align);
static UINT32 SvcRawCap_MemDiff(const UINT8 *pVal0, const UINT8 *pVal1);
static UINT32 SvcRawCap_MemCmp(const UINT8 *pVal0, const UINT8 *pVal1);
static UINT32 SvcRawCap_CleanCache(const void *pDataBuf, UINT32 DataSize);
static UINT32 SvcRawCap_CleanInvalid(const void *pDataBuf, UINT32 DataSize);

static UINT32 SvcRawCap_RawInfoQuery(UINT32 VinID, const AMBA_DSP_RAW_DATA_RDY_s *pRawData);
static UINT32 SvcRawCap_RawDataCap(UINT32 VinID, const AMBA_DSP_RAW_DATA_RDY_s *pRawData);
static UINT32 SvcRawCap_RawDataCap_CapIntBuf(UINT32 VinID, const AMBA_DSP_RAW_DATA_RDY_s *pRawData);
static UINT32 SvcRawCap_RawDataCap_StopVin(UINT32 VinID);
static UINT32 SvcRawCap_RawDataRdyHdlr(const void *pEventData);

static void   SvcRawCap_MemQuery(UINT32 *pRawBufSize, UINT32 *pHdsBufSize, UINT32 *p3ABufSize);
static UINT32 SvcRawCap_MemCheck(void *pCheckBuf, UINT32 CheckBufSize);
static void   SvcRawCap_MemSetupBufTbl(void);
static void   SvcRawCap_MemSetupRaw(UINT32 ReqMemDepth, UINT32 OneSyncBufSize);
static void   SvcRawCap_MemSetupHds(UINT32 ReqMemDepth, UINT32 OneSyncBufSize);
static void   SvcRawCap_MemSetupAaa(UINT32 ReqMemDepth, UINT32 OneSyncBufSize);
static void   SvcRawCap_MemSetupBmp(void);
static void   SvcRawCap_MemSetupUnPack(void);
static UINT32 SvcRawCap_MemSetup(UINT32 ReqMemDepth);
static UINT32 SvcRawCap_CapReset(void);
static UINT32 SvcRawCap_CapSetup(void);
static UINT32 SvcRawCap_CapStart(void);
static UINT32 SvcRawCap_CapUpdate(UINT32 VinID);
static UINT32 SvcRawCap_CapAaa(UINT32 VinID, UINT32 RawCapSeq);
static UINT32 SvcRawCap_CapDone(UINT32 CapNum, UINT32 IsBlocked);
static UINT32 SvcRawCap_DumpRawData(UINT32 VinID, UINT32 BufIdx);
static UINT32 SvcRawCap_DumpHdsData(UINT32 VinID, UINT32 BufIdx);
static UINT32 SvcRawCap_Dump3AData(UINT32 VinID, UINT32 BufIdx);
static UINT32 SvcRawCap_DumpYuvData(UINT32 VinID, UINT32 BufIdx);
static UINT32 SvcRawCap_DumpData(UINT32 VinID);
static UINT32 SvcRawCap_DumpItn(void);
static void   SvcRawCap_DumpBmp(const void *pRawBuf, UINT32 Bayer, UINT32 Pitch, UINT32 Width, UINT32 Height, const char *pFileName);
static void   SvcRawCap_DumpBmpDiagkInt(UINT16 *pData, UINT32 Width, UINT32 Height, UINT32 Row, UINT32 Col);
static void   SvcRawCap_DumpBmpCrossInt(UINT16 *pData, UINT32 Width, UINT32 Height, UINT32 Row, UINT32 Col);
static void   SvcRawCap_DumpBmpBayer2Red(UINT32 Bayer, UINT32 Pitch, UINT32 Width, UINT32 Height, const UINT16 *pCurRawBuf, UINT16 *pWorkBuf, SVC_RAW_CAP_BITMAP_RGB_s *pBmpBuf);
static void   SvcRawCap_DumpBmpBayer2Blue(UINT32 Bayer, UINT32 Pitch, UINT32 Width, UINT32 Height, const UINT16 *pCurRawBuf, UINT16 *pWorkBuf, SVC_RAW_CAP_BITMAP_RGB_s *pBmpBuf);
static void   SvcRawCap_DumpBmpBayer2Green(UINT32 Bayer, UINT32 Pitch, UINT32 Width, UINT32 Height, const UINT16 *pCurRawBuf, UINT16 *pWorkBuf, SVC_RAW_CAP_BITMAP_RGB_s *pBmpBuf);
static UINT32 SvcRawCap_UnPackRawData(UINT32 DataBits, UINT32 Pitch, UINT32 Width, UINT32 Height, void *pComPackBuf, void *pUnPackBuf);
static UINT32 SvcRawCap_TaskCreate(UINT32 TaskPriority, UINT32 TaskCpuBits);
static UINT32 SvcRawCap_TaskDelete(void);
static void*  SvcRawCap_TaskEntry(void* EntryArg);
static void   SvcRawCap_DbgInfo(void);

static SVC_RAW_CAP_CTRL_s RawCapCtrl GNU_SECTION_NOZEROINIT;
#ifndef SVC_RAW_CAP_DBG_MSG_ON
static UINT32 RawCapCtrlFlag = SVC_RAW_CAP_FLAG_MSG_DEF;
#else
static UINT32 RawCapCtrlFlag = SVC_RAW_CAP_FLAG_MSG_DEF | SVC_RAW_CAP_FLAG_MSG_DBG;
#endif

typedef UINT32 (*SVC_RAW_CAP_SHELL_FUNC_f)(UINT32 ArgCount, char * const *pArgVector);
typedef void (*SVC_RAW_CAP_USAGE_f)(UINT32 CtrlFlag);
typedef struct {
    char                     CmdName[32];
    SVC_RAW_CAP_SHELL_FUNC_f pFunc;
    SVC_RAW_CAP_USAGE_f      pUsage;
} SVC_RAW_CAP_SHELL_FUNC_s;

static void   SvcRawCap_CmdStrToU32(const char *pString, UINT32 *pValue);
static void   SvcRawCap_CmdStrToPointer(const char *pStr, UINT8 **pPointer);
static UINT32 SvcRawCap_CmdCreate(UINT32 ArgCount, char * const *pArgVector);
static void   SvcRawCap_CmdCreateU(UINT32 CtrlFlag);
static UINT32 SvcRawCap_CmdDelete(UINT32 ArgCount, char * const *pArgVector);
static void   SvcRawCap_CmdDeleteU(UINT32 CtrlFlag);
static UINT32 SvcRawCap_CmdCfg(UINT32 ArgCount, char * const *pArgVector);
static void   SvcRawCap_CmdCfgU(UINT32 CtrlFlag);
static UINT32 SvcRawCap_CmdCfgMem(UINT32 ArgCount, char * const *pArgVector);
static void   SvcRawCap_CmdCfgMemU(UINT32 CtrlFlag);
static UINT32 SvcRawCap_CmdCfgItnMem(UINT32 ArgCount, char * const *pArgVector);
static void   SvcRawCap_CmdCfgItnMemU(UINT32 CtrlFlag);
static UINT32 SvcRawCap_CmdCfgRaw(UINT32 ArgCount, char * const *pArgVector);
static void   SvcRawCap_CmdCfgRawU(UINT32 CtrlFlag);
static UINT32 SvcRawCap_CmdCfgVinType(UINT32 ArgCount, char * const *pArgVector);
static void   SvcRawCap_CmdCfgVinTypeU(UINT32 CtrlFlag);
static UINT32 SvcRawCap_CmdCfgHds(UINT32 ArgCount, char * const *pArgVector);
static void   SvcRawCap_CmdCfgHdsU(UINT32 CtrlFlag);
static UINT32 SvcRawCap_CmdCfgAaa(UINT32 ArgCount, char * const *pArgVector);
static void   SvcRawCap_CmdCfgAaaU(UINT32 CtrlFlag);
static UINT32 SvcRawCap_CmdCfgItn(UINT32 ArgCount, char * const *pArgVector);
static void   SvcRawCap_CmdCfgItnU(UINT32 CtrlFlag);
static UINT32 SvcRawCap_CmdInfo(UINT32 ArgCount, char * const *pArgVector);
static void   SvcRawCap_CmdInfoU(UINT32 CtrlFlag);
static UINT32 SvcRawCap_CmdMemQry(UINT32 ArgCount, char * const *pArgVector);
static void   SvcRawCap_CmdMemQryU(UINT32 CtrlFlag);
static UINT32 SvcRawCap_CmdCap(UINT32 ArgCount, char * const *pArgVector);
static void   SvcRawCap_CmdCapU(UINT32 CtrlFlag);
static UINT32 SvcRawCap_CmdCapSeq(UINT32 ArgCount, char * const *pArgVector);
static void   SvcRawCap_CmdCapSeqU(UINT32 CtrlFlag);
static UINT32 SvcRawCap_CmdDump(UINT32 ArgCount, char * const *pArgVector);
static void   SvcRawCap_CmdDumpU(UINT32 CtrlFlag);
static UINT32 SvcRawCap_CmdDbgInfo(UINT32 ArgCount, char * const *pArgVector);
static void   SvcRawCap_CmdDbgInfoU(UINT32 CtrlFlag);
static void   SvcRawCap_CmdAppEntryInit(void);
static void   SvcRawCap_CmdAppUsage(UINT32 CtrlFlag);

#define SVC_RAW_CAP_SHELL_CMD_NUM  (16U)
static SVC_RAW_CAP_SHELL_FUNC_s SvcRawCapShellFunc[SVC_RAW_CAP_SHELL_CMD_NUM] GNU_SECTION_NOZEROINIT;

#define SVC_LOG_RC              "RCP"
#define SVC_LOG_RCP_HL_TITLE_0 ("\033""[38;2;255;125;38m")
#define SVC_LOG_RCP_HL_TITLE_1 ("\033""[38;2;255;255;128m")
#define SVC_LOG_RCP_HL_TITLE_2 ("\033""[38;2;255;255;128m""\033""[4m")
#define SVC_LOG_RCP_HL_NUM     ("\033""[38;2;153;217;234m")
#define SVC_LOG_RCP_HL_STR     ("\033""[38;2;255;174;201m")
#define SVC_LOG_RCP_HL_DEF     ("\033""[39m")
#define SVC_LOG_RCP_HL_END     ("\033""[0m")
#define PRN_RCP_MSG_LOG        { SVC_WRAP_PRINT_s SvcRawCapPrint; AmbaSvcWrap_MisraMemset(&(SvcRawCapPrint), 0, sizeof(SvcRawCapPrint)); SvcRawCapPrint.Argc --; SvcRawCapPrint.pStrFmt =
#define PRN_RCP_MSG_ARG_UINT64 ; SvcRawCapPrint.Argc ++; SvcRawCapPrint.Argv[SvcRawCapPrint.Argc].Uint64   =
#define PRN_RCP_MSG_ARG_UINT32 ; SvcRawCapPrint.Argc ++; SvcRawCapPrint.Argv[SvcRawCapPrint.Argc].Uint64   = (UINT64)((
#define PRN_RCP_MSG_ARG_CSTR   ; SvcRawCapPrint.Argc ++; SvcRawCapPrint.Argv[SvcRawCapPrint.Argc].pCStr    = ((
#define PRN_RCP_MSG_ARG_CPOINT ; SvcRawCapPrint.Argc ++; SvcRawCapPrint.Argv[SvcRawCapPrint.Argc].pPointer = ((
#define PRN_RCP_MSG_ARG_POST   ))
#define PRN_RCP_MSG_OK         ; SvcRawCapPrint.Argc ++; SvcRawCap_PrintLog(SVC_RAW_CAP_FLAG_MSG_OK , &(SvcRawCapPrint)); }
#define PRN_RCP_MSG_NG         ; SvcRawCapPrint.Argc ++; SvcRawCap_PrintLog(SVC_RAW_CAP_FLAG_MSG_NG , &(SvcRawCapPrint)); }
#define PRN_RCP_MSG_API        ; SvcRawCapPrint.Argc ++; SvcRawCap_PrintLog(SVC_RAW_CAP_FLAG_MSG_API, &(SvcRawCapPrint)); }
#define PRN_RCP_MSG_DBG        ; SvcRawCapPrint.Argc ++; SvcRawCap_PrintLog(SVC_RAW_CAP_FLAG_MSG_DBG, &(SvcRawCapPrint)); }
#define PRN_RCP_ERR_HDLR       SvcRawCap_ErrHdlr(__func__, __LINE__, PRetVal);

static void SvcRawCap_PrintLog(UINT32 LogLevel, SVC_WRAP_PRINT_s *pPrint)
{
    if (pPrint != NULL) {
        if ((RawCapCtrlFlag & LogLevel) > 0U) {

            switch (LogLevel) {
            case SVC_RAW_CAP_FLAG_MSG_OK :
                pPrint->pProc = SvcLog_OK;
                break;
            case SVC_RAW_CAP_FLAG_MSG_NG :
                pPrint->pProc = SvcLog_NG;
                break;
            default :
                pPrint->pProc = SvcLog_DBG;
                break;
            }

            SvcWrap_Print(SVC_LOG_RC, pPrint);
        }
    }
}

static UINT32 SvcRawCap_MutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut)
{
    return AmbaKAL_MutexTake(pMutex, TimeOut);
}

static void SvcRawCap_MutexGive(AMBA_KAL_MUTEX_t *pMutex)
{
    UINT32 ErrCode = AmbaKAL_MutexGive(pMutex);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }
}

static void SvcRawCap_snprintf(char *pBuf, UINT32 BufSize, SVC_WRAP_SNPRINT_s *pFmt)
{
    UINT32 RetStrLen = SvcWrap_snprintf(pBuf, BufSize, pFmt);
    if ((RetStrLen == 0U) && (pFmt != NULL)) {
        PRN_RCP_MSG_LOG "Fail to gen string - %s"
            PRN_RCP_MSG_ARG_CSTR pFmt->pStrFmt PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_NG
    }
}

static void SvcRawCap_ErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode)
{
    if (pCaller != NULL) {
        if (ErrCode != 0U) {
            PRN_RCP_MSG_LOG "Catch ErrCode(0x%08x) @ %s, %d"
                PRN_RCP_MSG_ARG_UINT32 ErrCode  PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_ARG_CSTR   pCaller  PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_ARG_UINT32 CodeLine PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_NG
        }
    }
}

static UINT32 SvcRawCap_CmdCreate(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount <= 3U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to process raw cap 'create' cmd - Parameters should > 3" PRN_RCP_MSG_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to process raw cap 'create' cmd - Parameters should not null!" PRN_RCP_MSG_NG
    } else {
        UINT32 TaskPriority = 0U;
        UINT32 TaskCpuBits  = 0U;

        SvcRawCap_CmdStrToU32(pArgVector[2U], &TaskPriority);
        SvcRawCap_CmdStrToU32(pArgVector[3U], &TaskCpuBits);

        if (0U != SvcRawCap_Create(TaskPriority, TaskCpuBits)) {
            PRN_RCP_MSG_LOG "Fail to process raw cap 'create' cmd - create capture module fail!" PRN_RCP_MSG_NG
        }
    }

    return RetVal;
}

static void SvcRawCap_CmdCreateU(UINT32 CtrlFlag)
{
    PRN_RCP_MSG_LOG "    %screate%s            : create the capture module%s"
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_TITLE_2:SVC_LOG_RCP_HL_TITLE_1 PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_DEF:SVC_LOG_RCP_HL_END         PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   SVC_LOG_RCP_HL_END                                            PRN_RCP_MSG_ARG_POST
    PRN_RCP_MSG_API
    if (CtrlFlag > 0U) {
        PRN_RCP_MSG_LOG "      TaskPriority    : configure capture task priority" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "      TaskCpuBits     : configure capture task cpu bits" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG " " PRN_RCP_MSG_API
    }
}

static UINT32 SvcRawCap_CmdDelete(UINT32 ArgCount, char * const *pArgVector)
{
    if ((ArgCount > 0U) && (pArgVector != NULL)) {
        if (0U != SvcRawCap_Delete()) {
            PRN_RCP_MSG_LOG "Delete capture module fail!" PRN_RCP_MSG_NG
        }
    }

    return SVC_OK;
}

static void SvcRawCap_CmdDeleteU(UINT32 CtrlFlag)
{
    PRN_RCP_MSG_LOG "    %sdelete%s            : delete the capture module%s"
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_TITLE_2:SVC_LOG_RCP_HL_TITLE_1 PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_DEF:SVC_LOG_RCP_HL_END         PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   SVC_LOG_RCP_HL_END                                            PRN_RCP_MSG_ARG_POST
    PRN_RCP_MSG_API
    if (CtrlFlag > 0U) {
        PRN_RCP_MSG_LOG " " PRN_RCP_MSG_API
    }
}

static UINT32 SvcRawCap_CmdCfg(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount <= 3U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to process raw cap 'cfg' cmd - Parameters should > 3" PRN_RCP_MSG_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to process raw cap 'cfg' cmd - Parameters should not null!" PRN_RCP_MSG_NG
    } else {
        UINT32 VinSelectBits = 0U;
        UINT32 CfgSelectBits = 0U;

        SvcRawCap_CmdStrToU32(pArgVector[2U], &VinSelectBits);
        SvcRawCap_CmdStrToU32(pArgVector[3U], &CfgSelectBits);

        if (0U != SvcRawCap_CfgCap(VinSelectBits, CfgSelectBits)) {
            PRN_RCP_MSG_LOG "Fail to process raw cap 'cfg' cmd - configure cap module fail!" PRN_RCP_MSG_NG
        }
    }

    return RetVal;
}

static void SvcRawCap_CmdCfgU(UINT32 CtrlFlag)
{
    PRN_RCP_MSG_LOG "    %scfg%s               : Configure raw capture type%s"
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_TITLE_2:SVC_LOG_RCP_HL_TITLE_1 PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_DEF:SVC_LOG_RCP_HL_END         PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   SVC_LOG_RCP_HL_END                                            PRN_RCP_MSG_ARG_POST
    PRN_RCP_MSG_API
    if (CtrlFlag > 0U) {
        PRN_RCP_MSG_LOG "      VinSelectBits   : Configure capture vin select bits" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "      CfgSelectBits   : bit 0 | b'00000001 | dump raw data" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "                      : bit 1 | b'00000010 | dump hds data" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "                      : bit 2 | b'00000100 | dump ituner" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "                      : bit 3 | b'00001000 | dump aaa data" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "                      : bit 4 | b'00010000 | do not save bitmap" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "                      : bit 5 | b'00100000 | do not unpack raw data" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "                      : " PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "                      : raw | hds | itn | aaa | CfgSelectBits" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "                      :  o  |     |     |     |  1" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "                      :  o  |  o  |     |     |  3" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "                      :  o  |  o  |  o  |  o  |  15" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "                      :  o  |  o  |  o  |     |  7" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "                      :  o  |     |  o  |     |  5" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG " " PRN_RCP_MSG_API
    }
}

static UINT32 SvcRawCap_IsCacheMemory(void *pBuf)
{
    UINT32 RetVal = SVC_NG;

    if (pBuf != NULL) {
        RetVal = SVC_OK;    //  cached
        if ((g_pPlatCbEntry != NULL) && (g_pPlatCbEntry->pCbCacheChk != NULL)) {
            if (g_pPlatCbEntry->pCbCacheChk(pBuf, 1U) == 0U) {
                RetVal = SVC_NG;    //  non-cached
            }
        }
    }

    return RetVal;
}

static UINT32 SvcRawCap_CmdCfgMem(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount <= 3U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to process raw cap 'cfg_mem' cmd - Parameters should > 3" PRN_RCP_MSG_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to process raw cap 'cfg_mem' cmd - Parameters should not null!" PRN_RCP_MSG_NG
    } else {
        UINT8 *pMemBuf = NULL;
        UINT32 MemSize = 0U;

        SvcRawCap_CmdStrToPointer(pArgVector[2U], &pMemBuf);
        SvcRawCap_CmdStrToU32(pArgVector[3U], &MemSize);
        PRetVal = SvcRawCap_IsCacheMemory(pMemBuf); PRN_RCP_ERR_HDLR

#ifdef CONFIG_SOC_CV2FS
        if (SVC_OK != PRetVal) {
            RetVal = SVC_NG;
            PRN_RCP_MSG_LOG "Fail to process raw cap 'cfg_mem' cmd - only support cache memory in CV2FS!" PRN_RCP_MSG_NG
        } else
#endif
        if (0U != SvcRawCap_CfgMem(pMemBuf, MemSize)) {
            PRN_RCP_MSG_LOG "Fail to process raw cap 'cfg_mem' cmd - configure cap memory fail!" PRN_RCP_MSG_NG
        } else {
            /* Do nothing */
        }
    }

    return RetVal;
}

static void SvcRawCap_CmdCfgMemU(UINT32 CtrlFlag)
{
    PRN_RCP_MSG_LOG "    %scfg_mem%s           : Configure raw/hds/3a buffer memory%s"
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_TITLE_2:SVC_LOG_RCP_HL_TITLE_1 PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_DEF:SVC_LOG_RCP_HL_END         PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   SVC_LOG_RCP_HL_END                                            PRN_RCP_MSG_ARG_POST
    PRN_RCP_MSG_API
    if (CtrlFlag > 0U) {
#ifdef CONFIG_SOC_CV2FS
        PRN_RCP_MSG_LOG "      MemAddr         : configure the cache memory base" PRN_RCP_MSG_API
#else
        PRN_RCP_MSG_LOG "      MemAddr         : configure the memory base" PRN_RCP_MSG_API
#endif
        PRN_RCP_MSG_LOG "      MemSize         : configure the memory size" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG " " PRN_RCP_MSG_API
    }
}

static UINT32 SvcRawCap_CmdCfgItnMem(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount <= 3U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to process raw cap 'cfg_itn_mem' cmd - Parameters should > 3" PRN_RCP_MSG_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to process raw cap 'cfg_itn_mem' cmd - Parameters should not null!" PRN_RCP_MSG_NG
    } else {
        UINT8 *pMemBuf = NULL;
        UINT32 MemSize = 0U;

        SvcRawCap_CmdStrToPointer(pArgVector[2U], &pMemBuf);
        SvcRawCap_CmdStrToU32(pArgVector[3U], &MemSize);

        if (0U != SvcRawCap_CfgItnMem(pMemBuf, MemSize)) {
            PRN_RCP_MSG_LOG "Fail to process raw cap 'cfg_itn_mem' cmd - configure cap memory fail!" PRN_RCP_MSG_NG
        }
    }

    return RetVal;
}

static void SvcRawCap_CmdCfgItnMemU(UINT32 CtrlFlag)
{
    PRN_RCP_MSG_LOG "    %scfg_itn_mem%s       : Configure ituner memory%s"
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_TITLE_2:SVC_LOG_RCP_HL_TITLE_1 PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_DEF:SVC_LOG_RCP_HL_END         PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   SVC_LOG_RCP_HL_END                                            PRN_RCP_MSG_ARG_POST
    PRN_RCP_MSG_API
    if (CtrlFlag > 0U) {
        PRN_RCP_MSG_LOG "      MemAddr         : configure the memory base" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "      MemSize         : configure the memory size" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG " " PRN_RCP_MSG_API
    }
}

static UINT32 SvcRawCap_CmdCfgRaw(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount <= 3U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to process raw cap 'cfg_raw' cmd - Parameters should > 3" PRN_RCP_MSG_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to process raw cap 'cfg_raw' cmd - Parameters should not null!" PRN_RCP_MSG_NG
    } else {
        UINT32 VinID = 0U;

        SvcRawCap_CmdStrToU32(pArgVector[2U], &VinID);

        if (0U != SvcRawCap_CfgRaw(VinID, pArgVector[3U])) {
            PRN_RCP_MSG_LOG "Fail to process raw cap 'cfg_raw' cmd - configure raw setting fail!" PRN_RCP_MSG_NG
        }
    }

    return RetVal;
}

static void SvcRawCap_CmdCfgRawU(UINT32 CtrlFlag)
{
    PRN_RCP_MSG_LOG "    %scfg_raw%s           : Configure raw setting%s"
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_TITLE_2:SVC_LOG_RCP_HL_TITLE_1 PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_DEF:SVC_LOG_RCP_HL_END         PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   SVC_LOG_RCP_HL_END                                            PRN_RCP_MSG_ARG_POST
    PRN_RCP_MSG_API
    PRN_RCP_MSG_LOG "    %scfg_raw%s           : Configure raw setting%s"
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_TITLE_2:SVC_LOG_RCP_HL_TITLE_1 PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_DEF:SVC_LOG_RCP_HL_END         PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   SVC_LOG_RCP_HL_END                                            PRN_RCP_MSG_ARG_POST
    PRN_RCP_MSG_API
    if (CtrlFlag > 0U) {
        PRN_RCP_MSG_LOG "      VinID           : configure vin_id" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "      Raw Filename    : configure the raw filename" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG " " PRN_RCP_MSG_API
    }
}

static UINT32 SvcRawCap_CmdCfgVinType(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount <= 3U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to process raw cap 'cfg_type' cmd - Parameters should > 3" PRN_RCP_MSG_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to process raw cap 'cfg_type' cmd - Parameters should not null!" PRN_RCP_MSG_NG
    } else {
        UINT32 VinID = 0U;
        UINT32 Type  = 0U;

        SvcRawCap_CmdStrToU32(pArgVector[2U], &VinID);
        SvcRawCap_CmdStrToU32(pArgVector[3U], &Type);

        if (0U != SvcRawCap_CfgVinType(VinID, Type)) {
            PRN_RCP_MSG_LOG "Fail to process raw cap 'cfg_type' cmd - configure vin type fail!" PRN_RCP_MSG_NG
        }
    }

    return RetVal;
}

static void SvcRawCap_CmdCfgVinTypeU(UINT32 CtrlFlag)
{
    PRN_RCP_MSG_LOG "    %scfg_type%s          : Configure vin type%s"
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_TITLE_2:SVC_LOG_RCP_HL_TITLE_1 PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_DEF:SVC_LOG_RCP_HL_END         PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   SVC_LOG_RCP_HL_END                                            PRN_RCP_MSG_ARG_POST
    PRN_RCP_MSG_API
    if (CtrlFlag > 0U) {
        PRN_RCP_MSG_LOG "      VinID           : configure vin_id" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "      Type            : CFA(0), YUV(1)" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG " " PRN_RCP_MSG_API
    }
}

static UINT32 SvcRawCap_CmdCfgHds(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount <= 3U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to process raw cap 'cfg_hds' cmd - Parameters should > 3" PRN_RCP_MSG_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to process raw cap 'cfg_hds' cmd - Parameters should not null!" PRN_RCP_MSG_NG
    } else {
        UINT32 VinID = 0U;

        SvcRawCap_CmdStrToU32(pArgVector[2U], &VinID);

        if (0U != SvcRawCap_CfgHds(VinID, pArgVector[3U])) {
            PRN_RCP_MSG_LOG "Fail to process raw cap 'cfg_hds' cmd - configure hds setting fail!" PRN_RCP_MSG_NG
        }
    }

    return RetVal;
}

static void SvcRawCap_CmdCfgHdsU(UINT32 CtrlFlag)
{
    PRN_RCP_MSG_LOG "    %scfg_hds%s           : Configure hds setting%s"
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_TITLE_2:SVC_LOG_RCP_HL_TITLE_1 PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_DEF:SVC_LOG_RCP_HL_END         PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   SVC_LOG_RCP_HL_END                                            PRN_RCP_MSG_ARG_POST
    PRN_RCP_MSG_API
    if (CtrlFlag > 0U) {
        PRN_RCP_MSG_LOG "      VinID           : configure vin_id" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "      Hds Filename    : configure the raw filename" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG " " PRN_RCP_MSG_API
    }
}

static UINT32 SvcRawCap_CmdCfgAaa(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount <= 4U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to process raw cap 'cfg_aaa' cmd - Parameters should > 4" PRN_RCP_MSG_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to process raw cap 'cfg_aaa' cmd - Parameters should not null!" PRN_RCP_MSG_NG
    } else {
        UINT32 VinID = 0U;
        UINT32 ImgAlgSelectBits = 0U;

        SvcRawCap_CmdStrToU32(pArgVector[2U], &VinID);
        SvcRawCap_CmdStrToU32(pArgVector[3U], &ImgAlgSelectBits);

        if (0U != SvcRawCap_Cfg3A(VinID, ImgAlgSelectBits, pArgVector[4U])) {
            PRN_RCP_MSG_LOG "Fail to process raw cap 'cfg_aaa' cmd - configure aaa setting fail!" PRN_RCP_MSG_NG
        }
    }

    return RetVal;
}

static void SvcRawCap_CmdCfgAaaU(UINT32 CtrlFlag)
{
    PRN_RCP_MSG_LOG "    %scfg_aaa%s           : Configure 3A setting%s"
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_TITLE_2:SVC_LOG_RCP_HL_TITLE_1 PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_DEF:SVC_LOG_RCP_HL_END         PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   SVC_LOG_RCP_HL_END                                            PRN_RCP_MSG_ARG_POST
    PRN_RCP_MSG_API
    if (CtrlFlag > 0U) {
        PRN_RCP_MSG_LOG "      VinID           : configure vin_id" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "      ImgChanBits     : configure image algo. channel select bits" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "      Aaa Filename    : configure the raw filename" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG " " PRN_RCP_MSG_API
    }
}

static UINT32 SvcRawCap_CmdCfgItn(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount <= 3U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to process raw cap 'cfg_itn' cmd - Parameters should > 3" PRN_RCP_MSG_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to process raw cap 'cfg_itn' cmd - Parameters should not null!" PRN_RCP_MSG_NG
    } else {
        UINT32 IkSelectBits = 0U;

        SvcRawCap_CmdStrToU32(pArgVector[2U], &IkSelectBits);

        if (0U != SvcRawCap_CfgItn(IkSelectBits, pArgVector[3])) {
            PRN_RCP_MSG_LOG "Fail to process raw cap 'cfg_itn' cmd - configure ituner fail!" PRN_RCP_MSG_NG
        }
    }

    return RetVal;
}

static void SvcRawCap_CmdCfgItnU(UINT32 CtrlFlag)
{
    PRN_RCP_MSG_LOG "    %scfg_itn%s           : Configure ituner setting%s"
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_TITLE_2:SVC_LOG_RCP_HL_TITLE_1 PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_DEF:SVC_LOG_RCP_HL_END         PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   SVC_LOG_RCP_HL_END                                            PRN_RCP_MSG_ARG_POST
    PRN_RCP_MSG_API
    if (CtrlFlag > 0U) {
        PRN_RCP_MSG_LOG "      Ik context bits : configure Image kernel context selected bits" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "      Filename        : configure saved file path" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG " " PRN_RCP_MSG_API
    }
}

static UINT32 SvcRawCap_CmdInfo(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount <= 2U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to process raw cap 'info' cmd - Parameters should > 2" PRN_RCP_MSG_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to process raw cap 'info' cmd - Parameters should not null!" PRN_RCP_MSG_NG
    } else {
        UINT32 VinID = 0U;
        SVC_RAW_CAP_INFO_s RawInfo;
        SVC_RAW_CAP_INFO_s HdsInfo;

        SvcRawCap_CmdStrToU32(pArgVector[2U], &VinID);

        AmbaSvcWrap_MisraMemset(&RawInfo, 0, sizeof(RawInfo));
        AmbaSvcWrap_MisraMemset(&HdsInfo, 0, sizeof(HdsInfo));

        if (0U != SvcRawCap_Info(VinID, &RawInfo, &HdsInfo)) {
            PRN_RCP_MSG_LOG "Fail to print the raw/hds info" PRN_RCP_MSG_NG
        } else {

            PRN_RCP_MSG_LOG " " PRN_RCP_MSG_API
            PRN_RCP_MSG_LOG "---- Raw Info ----" PRN_RCP_MSG_API
            PRN_RCP_MSG_LOG "   Pitch : %d"
                PRN_RCP_MSG_ARG_UINT32 RawInfo.Pitch PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_API
            PRN_RCP_MSG_LOG "   Width : %d"
                PRN_RCP_MSG_ARG_UINT32 RawInfo.Width PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_API
            PRN_RCP_MSG_LOG "  Height : %d"
                PRN_RCP_MSG_ARG_UINT32 RawInfo.Height PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_API
            PRN_RCP_MSG_LOG "    Addr : %p"
                PRN_RCP_MSG_ARG_CPOINT RawInfo.pBuf PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_API
            PRN_RCP_MSG_LOG "---- Hds Info ----" PRN_RCP_MSG_API
            PRN_RCP_MSG_LOG "   Pitch : %d"
                PRN_RCP_MSG_ARG_UINT32 HdsInfo.Pitch PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_API
            PRN_RCP_MSG_LOG "   Width : %d"
                PRN_RCP_MSG_ARG_UINT32 HdsInfo.Width PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_API
            PRN_RCP_MSG_LOG "  Height : %d"
                PRN_RCP_MSG_ARG_UINT32 HdsInfo.Height PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_API
            PRN_RCP_MSG_LOG "    Addr : %p"
                PRN_RCP_MSG_ARG_CPOINT HdsInfo.pBuf PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_API

        }
    }

    return RetVal;
}

static void SvcRawCap_CmdInfoU(UINT32 CtrlFlag)
{
    PRN_RCP_MSG_LOG "    %sinfo%s              : print the raw info%s"
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_TITLE_2:SVC_LOG_RCP_HL_TITLE_1 PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_DEF:SVC_LOG_RCP_HL_END         PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   SVC_LOG_RCP_HL_END                                            PRN_RCP_MSG_ARG_POST
    PRN_RCP_MSG_API
    if (CtrlFlag > 0U) {
        PRN_RCP_MSG_LOG "      VinID           : vin id" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG " " PRN_RCP_MSG_API
    }
}

static UINT32 SvcRawCap_CmdMemQry(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount <= 2U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to process raw cap 'mem_qry' cmd - Parameters should > 2" PRN_RCP_MSG_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to process raw cap 'mem_qry' cmd - Parameters should not null!" PRN_RCP_MSG_NG
    } else {
        UINT32 CapNum = 0U;
        UINT32 BufSize = 0U;

        SvcRawCap_CmdStrToU32(pArgVector[2U], &CapNum);

        SvcRawCap_MemQry(CapNum, &BufSize);
        PRN_RCP_MSG_LOG "Successful to query requested capture buffer 0x%x"
            PRN_RCP_MSG_ARG_UINT32 BufSize PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_API
    }

    return RetVal;
}

static void SvcRawCap_CmdMemQryU(UINT32 CtrlFlag)
{
    PRN_RCP_MSG_LOG "    %smem_qry%s           : print the requested memory size%s"
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_TITLE_2:SVC_LOG_RCP_HL_TITLE_1 PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_DEF:SVC_LOG_RCP_HL_END         PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   SVC_LOG_RCP_HL_END                                            PRN_RCP_MSG_ARG_POST
    PRN_RCP_MSG_API
    if (CtrlFlag > 0U) {
        PRN_RCP_MSG_LOG "      CapNum          : configure capture number" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG " " PRN_RCP_MSG_API
    }
}

static UINT32 SvcRawCap_CmdCap(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount <= 3U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to process raw cap 'cap' cmd - Parameters should > 3" PRN_RCP_MSG_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to process raw cap 'cap' cmd - Parameters should not null!" PRN_RCP_MSG_NG
    } else {
        UINT32 VinID = 0U;

        SvcRawCap_CmdStrToU32(pArgVector[2U], &VinID);

        /* Configure the raw setting */
        PRetVal = SvcRawCap_CfgRaw(VinID, pArgVector[3U]);

        /* Configure the hds setting */
        if (PRetVal == SVC_OK) {
            if (ArgCount >= 5U) {
                if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_HDS) > 0U) {
                    PRetVal = SvcRawCap_CfgHds(VinID, pArgVector[4U]);
                }
            }
        }

        /* Configure the ituner setting */
        if (PRetVal == SVC_OK) {
            if (ArgCount >= 7U) {
                if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_TUNER) > 0U) {
                    UINT32 IkContextID = 0;
                    const char *pItnFileName = pArgVector[5U];

                    SvcRawCap_CmdStrToU32(pArgVector[6U], &IkContextID);
                    if (IkContextID < AMBA_DSP_MAX_VIEWZONE_NUM) {
                        if ((IkContextID & 0xF0000000U) > 0U) {
                            PRetVal = SvcRawCap_CfgItn(IkContextID & 0x0000FFFFU, pItnFileName);
                        } else {
                            UINT32 IkBits = SvcRawCap_BitGet(IkContextID);
                            PRetVal = SvcRawCap_CfgItn(IkBits, pItnFileName);
                        }
                    }
                }
            }
        }

        if (PRetVal == SVC_OK) {
            SvcRawCap_DbgInfo();
        }

        /* Start capture */
        if (PRetVal == OK) {
            PRetVal = SvcRawCap_CapSeq(1U, 1U);
        }

        /* Dump data to file */
        if (PRetVal == OK) {
            PRetVal = SvcRawCap_DumpData(VinID); PRN_RCP_ERR_HDLR
        }
    }

    return RetVal;
}

static void SvcRawCap_CmdCapU(UINT32 CtrlFlag)
{
    PRN_RCP_MSG_LOG "    %scap%s               : start capture raw/hds data%s"
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_TITLE_2:SVC_LOG_RCP_HL_TITLE_1 PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_DEF:SVC_LOG_RCP_HL_END         PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   SVC_LOG_RCP_HL_END                                            PRN_RCP_MSG_ARG_POST
    PRN_RCP_MSG_API
    if (CtrlFlag > 0U) {
        PRN_RCP_MSG_LOG "      VinID           : vin id" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "      raw path        : saving raw file path" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "      hds path        : saving hds file path" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "      ItnFileName     : configure the ituner filename" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "      IK context ID   : image kernel context id" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG " " PRN_RCP_MSG_API
    }
}

static UINT32 SvcRawCap_CmdCapSeq(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount <= 2U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to process raw cap 'cap_seq' cmd - Parameters should > 2" PRN_RCP_MSG_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to process raw cap 'cap_seq' cmd - Parameters should not null!" PRN_RCP_MSG_NG
    } else {
        UINT32 CapNum = 0U;

        SvcRawCap_CmdStrToU32(pArgVector[2U], &CapNum);

        if (SVC_OK != SvcRawCap_CapSeq(CapNum, 0U)) {
            PRN_RCP_MSG_LOG "Fail to process raw cap 'cap_seq' cmd - capture sequence data fail!" PRN_RCP_MSG_NG
        }
    }

    return RetVal;
}

static void SvcRawCap_CmdCapSeqU(UINT32 CtrlFlag)
{
    PRN_RCP_MSG_LOG "    %scap_seq%s           : start capture sequence data%s"
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_TITLE_2:SVC_LOG_RCP_HL_TITLE_1 PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_DEF:SVC_LOG_RCP_HL_END         PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   SVC_LOG_RCP_HL_END                                            PRN_RCP_MSG_ARG_POST
    PRN_RCP_MSG_API
    if (CtrlFlag > 0U) {
        PRN_RCP_MSG_LOG "      CapNum          : configure capture number" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG " " PRN_RCP_MSG_API
    }
}

static UINT32 SvcRawCap_CmdDump(UINT32 ArgCount, char * const *pArgVector)
{
    if ((ArgCount > 0U) && (pArgVector != NULL)) {
        if (SVC_OK != SvcRawCap_Dump()) {
            PRN_RCP_MSG_LOG "Fail to process raw cap 'dump' cmd - dump data file!" PRN_RCP_MSG_NG
        }
    }

    return SVC_OK;
}

static void SvcRawCap_CmdDumpU(UINT32 CtrlFlag)
{
    PRN_RCP_MSG_LOG "    %sdump%s              : dump the related captured data to files%s"
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_TITLE_2:SVC_LOG_RCP_HL_TITLE_1 PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_DEF:SVC_LOG_RCP_HL_END         PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   SVC_LOG_RCP_HL_END                                            PRN_RCP_MSG_ARG_POST
    PRN_RCP_MSG_API
    if (CtrlFlag > 0U) {
        PRN_RCP_MSG_LOG " " PRN_RCP_MSG_API
    }
}

static UINT32 SvcRawCap_CmdDbgInfo(UINT32 ArgCount, char * const *pArgVector)
{
    if ((ArgCount > 0U) && (pArgVector != NULL)) {
        UINT32 DbgType = 0U;

        if (ArgCount > 2U) {
            SvcRawCap_CmdStrToU32(pArgVector[2U], &DbgType);
        }

        if (DbgType == 0U) {
            SvcRawCap_DbgInfo();
        } else {
            if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_PRN_DSP_EVT) > 0U) {
                RawCapCtrlFlag &= ~SVC_RAW_CAP_FLAG_PRN_DSP_EVT;
            } else {
                RawCapCtrlFlag |= SVC_RAW_CAP_FLAG_PRN_DSP_EVT;
            }
        }
    }

    return SVC_OK;
}

static void SvcRawCap_CmdDbgInfoU(UINT32 CtrlFlag)
{
    PRN_RCP_MSG_LOG "    %sdbg%s               : print debug info%s"
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_TITLE_2:SVC_LOG_RCP_HL_TITLE_1 PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_RCP_HL_DEF:SVC_LOG_RCP_HL_END         PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   SVC_LOG_RCP_HL_END                                            PRN_RCP_MSG_ARG_POST
    PRN_RCP_MSG_API
    if (CtrlFlag > 0U) {
        PRN_RCP_MSG_LOG "      Type            : configure debug type" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "                        0 -> enable raw capture ctrl info" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "                        1 -> on/off raw data event info" PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG " " PRN_RCP_MSG_API
    }
}

static void SvcRawCap_CmdAppEntryInit(void)
{
    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_SHELL_INIT) == 0U) {
        UINT32 Cnt = 0U;

        AmbaSvcWrap_MisraMemset(SvcRawCapShellFunc, 0, sizeof(SvcRawCapShellFunc));
        SvcRawCapShellFunc[Cnt] = (SVC_RAW_CAP_SHELL_FUNC_s) { "create",       SvcRawCap_CmdCreate,       SvcRawCap_CmdCreateU    }; Cnt++;
        SvcRawCapShellFunc[Cnt] = (SVC_RAW_CAP_SHELL_FUNC_s) { "delete",       SvcRawCap_CmdDelete,       SvcRawCap_CmdDeleteU    }; Cnt++;
        SvcRawCapShellFunc[Cnt] = (SVC_RAW_CAP_SHELL_FUNC_s) { "cfg",          SvcRawCap_CmdCfg,          SvcRawCap_CmdCfgU       }; Cnt++;
        SvcRawCapShellFunc[Cnt] = (SVC_RAW_CAP_SHELL_FUNC_s) { "cfg_mem",      SvcRawCap_CmdCfgMem,       SvcRawCap_CmdCfgMemU    }; Cnt++;
        SvcRawCapShellFunc[Cnt] = (SVC_RAW_CAP_SHELL_FUNC_s) { "cfg_itn_mem",  SvcRawCap_CmdCfgItnMem,    SvcRawCap_CmdCfgItnMemU }; Cnt++;
        SvcRawCapShellFunc[Cnt] = (SVC_RAW_CAP_SHELL_FUNC_s) { "cfg_raw",      SvcRawCap_CmdCfgRaw,       SvcRawCap_CmdCfgRawU    }; Cnt++;
        SvcRawCapShellFunc[Cnt] = (SVC_RAW_CAP_SHELL_FUNC_s) { "cfg_hds",      SvcRawCap_CmdCfgHds,       SvcRawCap_CmdCfgHdsU    }; Cnt++;
        SvcRawCapShellFunc[Cnt] = (SVC_RAW_CAP_SHELL_FUNC_s) { "cfg_aaa",      SvcRawCap_CmdCfgAaa,       SvcRawCap_CmdCfgAaaU    }; Cnt++;
        SvcRawCapShellFunc[Cnt] = (SVC_RAW_CAP_SHELL_FUNC_s) { "cfg_itn",      SvcRawCap_CmdCfgItn,       SvcRawCap_CmdCfgItnU    }; Cnt++;
        SvcRawCapShellFunc[Cnt] = (SVC_RAW_CAP_SHELL_FUNC_s) { "cfg_type",     SvcRawCap_CmdCfgVinType,   SvcRawCap_CmdCfgVinTypeU}; Cnt++;
        SvcRawCapShellFunc[Cnt] = (SVC_RAW_CAP_SHELL_FUNC_s) { "info",         SvcRawCap_CmdInfo,         SvcRawCap_CmdInfoU      }; Cnt++;
        SvcRawCapShellFunc[Cnt] = (SVC_RAW_CAP_SHELL_FUNC_s) { "mem_qry",      SvcRawCap_CmdMemQry,       SvcRawCap_CmdMemQryU    }; Cnt++;
        SvcRawCapShellFunc[Cnt] = (SVC_RAW_CAP_SHELL_FUNC_s) { "cap",          SvcRawCap_CmdCap,          SvcRawCap_CmdCapU       }; Cnt++;
        SvcRawCapShellFunc[Cnt] = (SVC_RAW_CAP_SHELL_FUNC_s) { "cap_seq",      SvcRawCap_CmdCapSeq,       SvcRawCap_CmdCapSeqU    }; Cnt++;
        SvcRawCapShellFunc[Cnt] = (SVC_RAW_CAP_SHELL_FUNC_s) { "dump",         SvcRawCap_CmdDump,         SvcRawCap_CmdDumpU      }; Cnt++;
        SvcRawCapShellFunc[Cnt] = (SVC_RAW_CAP_SHELL_FUNC_s) { "dbg",          SvcRawCap_CmdDbgInfo,      SvcRawCap_CmdDbgInfoU   };

        RawCapCtrlFlag |= SVC_RAW_CAP_FLAG_SHELL_INIT;
    }
}

/**
* command entry of raw capture module
* @param [in] ArgCount count of arguments
* @param [in] pArgVector array of arguments
* @return none
*/
void SvcRawCap_CmdAppEntry(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_NG;
    UINT32 CtrlFlag = 0U;

    SvcRawCap_CmdAppEntryInit();

    if(ArgCount == 0xCafeU) {
        UINT32 SIdx;

        PRN_RCP_MSG_LOG " " PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "==== %sShell Arguments Setting%s (%s%d%s) ===="
            PRN_RCP_MSG_ARG_CSTR   SVC_LOG_RCP_HL_TITLE_1 PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_ARG_CSTR   SVC_LOG_RCP_HL_END     PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_ARG_CSTR   SVC_LOG_RCP_HL_NUM     PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_ARG_UINT32 ArgCount               PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_ARG_CSTR   SVC_LOG_RCP_HL_END     PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_API

        for (SIdx = 0U; SIdx < ArgCount; SIdx ++) {
            PRN_RCP_MSG_LOG "  pArgVector[%s%d%s] : %s%s%s"
                PRN_RCP_MSG_ARG_CSTR   SVC_LOG_RCP_HL_NUM PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_ARG_UINT32 SIdx               PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_ARG_CSTR   SVC_LOG_RCP_HL_END PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_ARG_CSTR   SVC_LOG_RCP_HL_STR PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_ARG_CSTR   pArgVector[SIdx]   PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_ARG_CSTR   SVC_LOG_RCP_HL_END PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_API
        }
    }

    if (2U <= ArgCount) {
        UINT32 CmdCnt = (UINT32)sizeof(SvcRawCapShellFunc) / (UINT32)sizeof(SvcRawCapShellFunc[0]);
        UINT32 CmdIdx;

        if (0 == SvcWrap_strcmp(pArgVector[1U], "more")) {
            CtrlFlag = 1U;
        } else {

            for (CmdIdx = 0; CmdIdx < CmdCnt; CmdIdx ++) {
                if (0 == SvcWrap_strcmp(pArgVector[1U], SvcRawCapShellFunc[CmdIdx].CmdName)) {
                    if (SvcRawCapShellFunc[CmdIdx].pFunc != NULL) {
                        if (0U != (SvcRawCapShellFunc[CmdIdx].pFunc)(ArgCount, pArgVector)) {
                            if (SvcRawCapShellFunc[CmdIdx].pUsage != NULL) {
                                (SvcRawCapShellFunc[CmdIdx].pUsage)(1U);
                            }
                        }
                        RetVal = SVC_OK;
                    }
                }
            }
        }
    }

    if (RetVal != SVC_OK) {
        SvcRawCap_CmdAppUsage(CtrlFlag);
    }
}

static void SvcRawCap_CmdAppUsage(UINT32 CtrlFlag)
{
    UINT32 ShellIdx, ShellCnt = ((UINT32)(sizeof(SvcRawCapShellFunc))) / ((UINT32)(sizeof(SvcRawCapShellFunc[0])));

    PRN_RCP_MSG_LOG " " PRN_RCP_MSG_API
    PRN_RCP_MSG_LOG "====== %sSVC Raw Cap Command Usage%s ======"
        PRN_RCP_MSG_ARG_CSTR   SVC_LOG_RCP_HL_TITLE_0 PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_ARG_CSTR   SVC_LOG_RCP_HL_END     PRN_RCP_MSG_ARG_POST
    PRN_RCP_MSG_API

    for (ShellIdx = 0U; ShellIdx < ShellCnt; ShellIdx ++) {
        if (SvcRawCapShellFunc[ShellIdx].pUsage == NULL) {
            PRN_RCP_MSG_LOG "  %s"
                PRN_RCP_MSG_ARG_CSTR   SvcRawCapShellFunc[ShellIdx].CmdName PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_API
        } else {
            (SvcRawCapShellFunc[ShellIdx].pUsage)(CtrlFlag);
        }
    }

    PRN_RCP_MSG_LOG "Type 'more' option to print more command description" PRN_RCP_MSG_API
}

static void SvcRawCap_CmdStrToU32(const char *pString, UINT32 *pValue)
{
    UINT32 ErrCode = AmbaUtility_StringToUInt32(pString, pValue);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }
}

static void SvcRawCap_CmdStrToPointer(const char *pStr, UINT8 **pPointer)
{
    if ((pStr != NULL) && (pPointer != NULL)) {
        UINT8 *pCurPointer = NULL;
#ifdef CONFIG_ARM64
        UINT64 U64Val = 0U;
        if (0U == SvcWrap_strtoull(pStr, &U64Val)) {
            AmbaMisra_TypeCast(&(pCurPointer), &(U64Val));
        }
#else
        UINT32 U32Val = 0U;
        if (0U == SvcWrap_strtoul(pStr, &U32Val)) {
            AmbaMisra_TypeCast(&(pCurPointer), &(U32Val));
        }
#endif
        *pPointer = pCurPointer;
    }
}

static UINT8 *SvcRawCap_MemAlign(const UINT8 *pMemBuf, UINT32 Align)
{
    UINT8 *pAlignDone = NULL;

    if (pMemBuf != NULL) {
#ifdef CONFIG_ARM64
        UINT64 Val, Ret;
        UINT64 AlignBase;
        AmbaMisra_TypeCast(&Val, &pMemBuf);
        AmbaMisra_TypeCast(&AlignBase, &Align); AlignBase &= 0xFFFFFFFFU;
        Ret = GetAlignedValU64(Val, AlignBase);
        AmbaMisra_TypeCast(&pAlignDone, &Ret);
#else
        UINT32 Val, Ret;
        AmbaMisra_TypeCast(&Val, &pMemBuf);
        Ret = GetAlignedValU32(Val, Align);
        AmbaMisra_TypeCast(&pAlignDone, &Ret);
#endif
    }

    return pAlignDone;
}

static UINT32 SvcRawCap_MemDiff(const UINT8 *pVal0, const UINT8 *pVal1)
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

static UINT32 SvcRawCap_MemCmp(const UINT8 *pVal0, const UINT8 *pVal1)
{
    UINT32 RetVal = 0U;

#ifdef CONFIG_ARM64
    UINT64 Val_A = 0U, Val_B = 0U;
#else
    UINT32 Val_A = 0U, Val_B = 0U;
#endif

    AmbaMisra_TypeCast(&(Val_A), &(pVal0));
    AmbaMisra_TypeCast(&(Val_B), &(pVal1));

    if (Val_A > Val_B) {
        RetVal = 1U;
    }

    return RetVal;
}

static UINT32 SvcRawCap_CleanCache(const void *pDataBuf, UINT32 DataSize)
{
    UINT32 RetVal = SVC_NG;
#ifdef CONFIG_ARM64
    UINT64 DataAddr, AlignAddr, AlignSize;

    AmbaMisra_TypeCast(&(DataAddr), &(pDataBuf));

    if (DataAddr >= 64ULL) {
        AlignAddr = DataAddr & 0xFFFFFFFFFFFFFFC0ULL;
        AlignSize = DataSize;
        if (AlignAddr < DataAddr) {
            AlignSize += DataAddr - AlignAddr;
        }
        AlignSize = GetAlignedValU64(AlignSize, 64ULL);

        RetVal = SvcPlat_CacheClean((ULONG)AlignAddr, (ULONG)AlignSize);
    }
#else
    UINT32 DataAddr, AlignAddr, AlignSize;

    AmbaMisra_TypeCast(&(DataAddr), &(pDataBuf));

    if (DataAddr >= 64U) {
        AlignAddr = DataAddr & 0xFFFFFFC0U;
        AlignSize = DataSize;
        if (AlignAddr < DataAddr) {
            AlignSize += DataAddr - AlignAddr;
        }
        AlignSize = GetAlignedValU32(AlignSize, 64U);

        RetVal = SvcPlat_CacheClean(AlignAddr, AlignSize);
    }
#endif

    return RetVal;
}

static UINT32 SvcRawCap_CleanInvalid(const void *pDataBuf, UINT32 DataSize)
{
    UINT32 RetVal = SVC_NG;
#ifdef CONFIG_ARM64
    UINT64 DataAddr, AlignAddr, AlignSize;

    AmbaMisra_TypeCast(&(DataAddr), &(pDataBuf));

    if (DataAddr >= 64ULL) {
        AlignAddr = DataAddr & 0xFFFFFFFFFFFFFFC0ULL;
        AlignSize = DataSize;
        if (AlignAddr < DataAddr) {
            AlignSize += DataAddr - AlignAddr;
        }
        AlignSize = GetAlignedValU64(AlignSize, 64ULL);

        RetVal = SvcPlat_CacheInvalidate((ULONG)AlignAddr, (ULONG)AlignSize);
    }
#else
    UINT32 DataAddr, AlignAddr, AlignSize;

    AmbaMisra_TypeCast(&(DataAddr), &(pDataBuf));

    if (DataAddr >= 64U) {
        AlignAddr = DataAddr & 0xFFFFFFC0U;
        AlignSize = DataSize;
        if (AlignAddr < DataAddr) {
            AlignSize += DataAddr - AlignAddr;
        }
        AlignSize = GetAlignedValU32(AlignSize, 64U);

        RetVal = SvcPlat_CacheInvalidate(AlignAddr, AlignSize);
    }
#endif

    return RetVal;
}

static UINT32 SvcRawCap_RawInfoQuery(UINT32 VinID, const AMBA_DSP_RAW_DATA_RDY_s *pRawData)
{
    UINT32 RetVal = SVC_OK;

    if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to query raw info - invalid VinID(%d)!"
            PRN_RCP_MSG_ARG_UINT32 VinID PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_NG
    } else if (pRawData == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to query raw info - invalid raw data!" PRN_RCP_MSG_NG
    } else {
        SVC_RAW_CAP_RAW_INFO_s *pRaw = &(RawCapCtrl.ItemCtrlRaw[VinID].RawInfo);
        SVC_RAW_CAP_INFO_s     *pHds = &(RawCapCtrl.ItemCtrlHds[VinID].HdsInfo);
        UINT32 FovIDs[16U], FovNum = 0U;
        UINT32 VinBits;

        pRaw->Pitch  = (UINT32)(pRawData->RawBuffer.Pitch);         pRaw->Pitch &= 0x0000FFFFU;
        pRaw->Width  = (UINT32)(pRawData->RawBuffer.Window.Width);  pRaw->Width &= 0x0000FFFFU;
        pRaw->Height = (UINT32)(pRawData->RawBuffer.Window.Height); pRaw->Height &= 0x0000FFFFU;
        AmbaMisra_TypeCast(&(pRaw->pBuf), &(pRawData->RawBuffer.BaseAddr));

        pRaw->Bayer = 0xFFFFFFFFU;
        AmbaSvcWrap_MisraMemset(FovIDs, 0, sizeof(FovIDs));
        if (0U == SvcResCfg_GetFovIdxsInVinID(VinID, FovIDs, &FovNum)) {
            AMBA_IK_MODE_CFG_s ImgMode;
            AMBA_IK_VIN_SENSOR_INFO_s ImgSenInfo;

            AmbaSvcWrap_MisraMemset(&ImgMode, 0, sizeof(ImgMode));
            ImgMode.ContextId = FovIDs[0U];

            AmbaSvcWrap_MisraMemset(&ImgSenInfo, 0, sizeof(ImgSenInfo));
            if (0U == AmbaIK_GetVinSensorInfo(&ImgMode, &ImgSenInfo)) {
                pRaw->Bayer = ImgSenInfo.SensorPattern;
                pRaw->CmprType = ImgSenInfo.Compression;
            }
        }

        if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_HDS) > 0U) {

            AmbaSvcWrap_MisraMemset(pHds, 0, sizeof(SVC_RAW_CAP_INFO_s));

            pHds->Pitch  = (UINT32)(pRawData->AuxBuffer.Pitch);         pHds->Pitch  &= 0x0000FFFFU;
            pHds->Width  = (UINT32)(pRawData->AuxBuffer.Window.Width);  pHds->Width  &= 0x0000FFFFU;
            pHds->Height = (UINT32)(pRawData->AuxBuffer.Window.Height); pHds->Height &= 0x0000FFFFU;
            AmbaMisra_TypeCast(&(pHds->pBuf), &(pRawData->AuxBuffer.BaseAddr));
        }

        VinBits = SvcRawCap_BitGet(VinID);
        if (0U != AmbaKAL_EventFlagSet(&(RawCapCtrl.CapStartFlag), VinBits)) {
            RetVal = SVC_NG;
            PRN_RCP_MSG_LOG "Fail to query raw info - set VinID(%d) start flag fail!"
                PRN_RCP_MSG_ARG_UINT32 VinID PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_NG
        } else if (0U != AmbaKAL_EventFlagSet(&(RawCapCtrl.CapRdyFlag), VinBits)) {
            RetVal = SVC_NG;
            PRN_RCP_MSG_LOG "Fail to query raw info - set VinID(%d) ready flag fail!"
                PRN_RCP_MSG_ARG_UINT32 VinID PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_NG
        } else {
            RawCapCtrlFlag &= ~(SVC_RAW_CAP_FLAG_QUERY);
        }
    }

    return RetVal;
}

static UINT32 SvcRawCap_RawDataCap(UINT32 VinID, const AMBA_DSP_RAW_DATA_RDY_s *pRawData)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to proc raw data - invalid VinID(%d)!"
            PRN_RCP_MSG_ARG_UINT32 VinID PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_NG
    } else if (pRawData == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to proc raw data - invalid raw data!" PRN_RCP_MSG_NG
    } else {
        UINT32 VinIdx, CapDoneSelectBits;
        SVC_RAW_CAP_ITEM_RAW_s *pRawItmCtrl = &(RawCapCtrl.ItemCtrlRaw[VinID]);
        SVC_RAW_CAP_ITEM_HDS_s *pHdsItmCtrl = &(RawCapCtrl.ItemCtrlHds[VinID]);
        UINT8 *pRawBuffer = NULL, *pAuxBuffer = NULL;

        AmbaMisra_TypeCast(&(pRawBuffer), &(pRawData->RawBuffer.BaseAddr)); AmbaMisra_TouchUnused(pRawBuffer);
        AmbaMisra_TypeCast(&(pAuxBuffer), &(pRawData->AuxBuffer.BaseAddr)); AmbaMisra_TouchUnused(pAuxBuffer);

        if ((pRawItmCtrl->NumCaptured < pRawItmCtrl->NumSeq) &&
            (pRawItmCtrl->pBufSeqTbl != NULL)) {
            if (pRawItmCtrl->NumCaptured == 0U) {
                if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_HDS) > 0U) {
                    if (pRawBuffer == pHdsItmCtrl->pBufSeqTbl[0]) {
                        pHdsItmCtrl->NumCaptured = 1U;
                    }
                }

                if (pRawBuffer == pRawItmCtrl->pBufSeqTbl[0]) {
                    pRawItmCtrl->NumCaptured = 1U;
                }
            } else {
                if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_HDS) > 0U) {
                    pHdsItmCtrl->NumCaptured += 1U;
                }
                pRawItmCtrl->NumCaptured += 1U;
            }

            if (pRawItmCtrl->NumCaptured > 0U) {
                PRN_RCP_MSG_LOG "Capture VinID(%d) no.%03d data"
                    PRN_RCP_MSG_ARG_UINT32 VinID                                PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 pRawItmCtrl->NumCaptured PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_OK

                /* Prepare the next buffer */
                if (SVC_OK != SvcRawCap_CapUpdate(VinID)) {
                    PRN_RCP_MSG_LOG "Fail to update cap buffer! VinID(%d)"
                        PRN_RCP_MSG_ARG_UINT32 VinID PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_NG
                }

                /* Capture 3A data */
                if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_3A) > 0U) {
                    UINT32 TimeStampStart, TimeStampEnd, TimeStampDiff;
                    UINT32 MsgCntStart, MsgCntEnd;
                    AMBA_KAL_MSG_QUEUE_INFO_s QueInfo;
                    UINT32 RawCapSeq = (UINT32)(pRawData->CapSequence);

                    pRawItmCtrl->RawSeqArr[pRawItmCtrl->NumCaptured - 1U] = RawCapSeq;

                    AmbaSvcWrap_MisraMemset(&QueInfo, 0, sizeof(QueInfo));

                    PRetVal = AmbaKAL_MsgQueueQuery(&(RawCapCtrl.MsgQue), &QueInfo); PRN_RCP_ERR_HDLR
                    MsgCntStart = QueInfo.NumEnqueued;

                    PRetVal = AmbaKAL_GetSysTickCount(&TimeStampStart); PRN_RCP_ERR_HDLR

                    if (SVC_OK != SvcRawCap_CapAaa(VinID, RawCapSeq)) {
                        PRN_RCP_MSG_LOG "Fail to capture 3A data! VinID(%d)"
                            PRN_RCP_MSG_ARG_UINT32 VinID PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_NG
                    }

                    PRetVal = AmbaKAL_GetSysTickCount(&TimeStampEnd); PRN_RCP_ERR_HDLR

                    PRetVal = AmbaKAL_MsgQueueQuery(&(RawCapCtrl.MsgQue), &QueInfo); PRN_RCP_ERR_HDLR
                    MsgCntEnd = QueInfo.NumEnqueued;

                    if (TimeStampEnd >= TimeStampStart) {
                        TimeStampDiff = TimeStampEnd - TimeStampStart;
                    } else {
                        TimeStampDiff = (0xFFFFFFFFU - TimeStampStart) + TimeStampEnd;
                    }

                    if (MsgCntEnd != MsgCntStart) {

                        PRN_RCP_MSG_LOG "Spend too much time(%d) to capture aaa data"
                            PRN_RCP_MSG_ARG_UINT32 TimeStampDiff PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_NG
                    } else {
                        PRN_RCP_MSG_LOG "Spend %d ms to capture aaa data"
                            PRN_RCP_MSG_ARG_UINT32 TimeStampDiff PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_DBG
                    }
                }
            }
        }

        /* Check the capture done or not */
        CapDoneSelectBits = 0U;
        for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
            if ((RawCapCtrl.VinSelectBits & SvcRawCap_BitGet(VinIdx)) > 0U) {
                if (pRawItmCtrl->NumCaptured >= pRawItmCtrl->NumSeq) {
                    CapDoneSelectBits |= SvcRawCap_BitGet(VinIdx);
                }
            }
        }

        /* If all capture is done, reset flow. */
        if (CapDoneSelectBits == RawCapCtrl.VinSelectBits) {
            PRetVal = SvcRawCap_CapReset(); PRN_RCP_ERR_HDLR
        }
    }

    return RetVal;
}

static UINT32 SvcRawCap_RawDataCap_CapIntBuf(UINT32 VinID, const AMBA_DSP_RAW_DATA_RDY_s *pRawData)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to proc raw data - invalid VinID(%d)!"
            PRN_RCP_MSG_ARG_UINT32 VinID PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_NG
    } else if (pRawData == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to proc raw data - invalid raw data!" PRN_RCP_MSG_NG
    } else {
        SVC_RAW_CAP_ITEM_RAW_s *pRawItmCtrl = &(RawCapCtrl.ItemCtrlRaw[VinID]);
        SVC_RAW_CAP_ITEM_HDS_s *pHdsItmCtrl = &(RawCapCtrl.ItemCtrlHds[VinID]);

        if (pRawItmCtrl->NumCaptured == 0U) {
            PRetVal = SvcRawCap_RawDataCap_StopVin(VinID); PRN_RCP_ERR_HDLR
        }

        AmbaMisra_TypeCast(&(pRawItmCtrl->RawInfo.pBuf),  &(pRawData->RawBuffer.BaseAddr));
        pRawItmCtrl->RawInfo.Pitch  = (UINT32)(pRawData->RawBuffer.Pitch);
        pRawItmCtrl->RawInfo.Pitch  &= 0xFFFFU;
        pRawItmCtrl->RawInfo.Width  = (UINT32)(pRawData->RawBuffer.Window.Width);
        pRawItmCtrl->RawInfo.Width  &= 0xFFFFU;
        pRawItmCtrl->RawInfo.Height = (UINT32)(pRawData->RawBuffer.Window.Height);
        pRawItmCtrl->RawInfo.Height &= 0xFFFFU;
        pRawItmCtrl->NumCaptured ++;

        if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_HDS) > 0U) {
            AmbaMisra_TypeCast(&(pHdsItmCtrl->HdsInfo.pBuf),  &(pRawData->AuxBuffer.BaseAddr));
            pHdsItmCtrl->HdsInfo.Pitch  = (UINT32)(pRawData->AuxBuffer.Pitch);
            pHdsItmCtrl->HdsInfo.Pitch  &= 0xFFFFU;
            pHdsItmCtrl->HdsInfo.Width  = (UINT32)(pRawData->AuxBuffer.Window.Width);
            pHdsItmCtrl->HdsInfo.Width  &= 0xFFFFU;
            pHdsItmCtrl->HdsInfo.Height = (UINT32)(pRawData->AuxBuffer.Window.Height);
            pHdsItmCtrl->HdsInfo.Height &= 0xFFFFU;
            pHdsItmCtrl->NumCaptured ++;
        }

        PRN_RCP_MSG_LOG "Keep the VinID(%d) raw %04u/%04u/%04u, %p"
            PRN_RCP_MSG_ARG_UINT32 VinID                                    PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_ARG_UINT32 pRawItmCtrl->RawInfo.Pitch  PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_ARG_UINT32 pRawItmCtrl->RawInfo.Width  PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_ARG_UINT32 pRawItmCtrl->RawInfo.Height PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_ARG_CPOINT pRawItmCtrl->RawInfo.pBuf   PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_DBG

        if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_HDS) > 0U) {
            PRN_RCP_MSG_LOG "Keep the VinID(%d) hds %04u/%04u/%04u, %p"
                PRN_RCP_MSG_ARG_UINT32 VinID                                    PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_ARG_UINT32 pHdsItmCtrl->HdsInfo.Pitch  PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_ARG_UINT32 pHdsItmCtrl->HdsInfo.Width  PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_ARG_UINT32 pHdsItmCtrl->HdsInfo.Height PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_ARG_CPOINT pHdsItmCtrl->HdsInfo.pBuf   PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_DBG
        }
    }


    return RetVal;
}

static UINT32 SvcRawCap_RawDataCap_StopVin(UINT32 VinID)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to stop vin - invalid VinID(%d)!"
            PRN_RCP_MSG_ARG_UINT32 VinID PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_NG
    } else {
        AMBA_SENSOR_CHANNEL_s SsChan;

        AmbaSvcWrap_MisraMemset(&SsChan, 0, sizeof(SsChan));
        SsChan.VinID = VinID;
        PRetVal = AmbaSensor_Disable(&SsChan); PRN_RCP_ERR_HDLR

        PRN_RCP_MSG_LOG "Stop Vin(%d)!"
            PRN_RCP_MSG_ARG_UINT32 VinID PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_API
    }

    return RetVal;
}

static UINT32 SvcRawCap_RawDataRdyHdlr(const void *pEventData)
{
    UINT32 PRetVal;

    if (pEventData != NULL) {
        if ((RawCapCtrlFlag & (SVC_RAW_CAP_FLAG_QUERY | SVC_RAW_CAP_FLAG_CAP)) > 0U) {
            const AMBA_DSP_RAW_DATA_RDY_s *pData;
            AmbaMisra_TypeCast(&(pData), &(pEventData));

            if (pData != NULL) {
                UINT32 VinID = (UINT32)(pData->VinId);
                VinID &= 0x0000FFFFU;

                if ((RawCapCtrl.VinSelectBits & SvcRawCap_BitGet(VinID)) > 0U) {
                    SVC_RAW_CAP_MSG_QUE_UNIT_s MsgUnit;

                    AmbaSvcWrap_MisraMemset(&MsgUnit, 0, sizeof(MsgUnit));
                    PRetVal = AmbaKAL_MsgQueueReceive(&(RawCapCtrl.FreeQue), &MsgUnit, AMBA_KAL_NO_WAIT);
                    if (PRetVal != 0U) {
                        PRN_RCP_MSG_LOG "Fail to proc raw evt - get free queue for VinID(%d)"
                            PRN_RCP_MSG_ARG_UINT32 VinID PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_DBG
                    } else if (MsgUnit.pData == NULL) {
                        PRN_RCP_MSG_LOG "Fail to proc raw evt - invalid free queue buffer" PRN_RCP_MSG_DBG
                    } else {
                        AmbaSvcWrap_MisraMemcpy(MsgUnit.pData, pData, sizeof(AMBA_DSP_RAW_DATA_RDY_s));
                        PRetVal = AmbaKAL_MsgQueueSend(&(RawCapCtrl.MsgQue), &MsgUnit, AMBA_KAL_NO_WAIT); PRN_RCP_ERR_HDLR
                    }
                }
            }
        }

        if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_PRN_DSP_EVT) > 0U) {
            const AMBA_DSP_RAW_DATA_RDY_s *pData;
            AmbaMisra_TypeCast(&(pData), &(pEventData));

            if (pData != NULL) {
                const void *pBuf = NULL;
                AmbaMisra_TypeCast(&(pBuf), &(pData->RawBuffer.BaseAddr));
                PRN_RCP_MSG_LOG "VinID(%d) p/w/h(%04d/%04d/%04d) addr(%p), raq_seq(%11u)"
                    PRN_RCP_MSG_ARG_UINT32 pData->VinId                   PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 pData->RawBuffer.Pitch         PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 pData->RawBuffer.Window.Width  PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 pData->RawBuffer.Window.Height PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_CPOINT pBuf                           PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT64 pData->CapSequence
                PRN_RCP_MSG_DBG
            }
        }
    }

    return 0U;
}

static void SvcRawCap_MemQuery(UINT32 *pRawBufSize, UINT32 *pHdsBufSize, UINT32 *p3ABufSize)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to query memory - create raw cap module first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to query memory - config raw cap module first!" PRN_RCP_MSG_NG
    } else if (pRawBufSize == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to query memory - invalid output raw buffer size!" PRN_RCP_MSG_NG
    } else if (pHdsBufSize == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to query memory - invalid output hds buffer size!" PRN_RCP_MSG_NG
    } else if (p3ABufSize == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to query memory - invalid output 3A buffer size!" PRN_RCP_MSG_NG
    } else {
        UINT32 VinIdx;
        UINT32 ActualFlags = 0U, WaitFlags = 0U;

        /* Clear flag */
        PRetVal = AmbaKAL_EventFlagClear(&(RawCapCtrl.Flag), 0xFFFFFFFFU); PRN_RCP_ERR_HDLR

        /* Configure wait flag */
        for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
            if ((RawCapCtrl.VinSelectBits & SvcRawCap_BitGet(VinIdx)) > 0U) {
                WaitFlags |= SvcRawCap_BitGet(VinIdx);
            }
        }

        /* Set flag to query raw info */
        RawCapCtrlFlag |= SVC_RAW_CAP_FLAG_QUERY;

        /* Wait flag for raw info ready */
        PRetVal = AmbaKAL_EventFlagGet(&(RawCapCtrl.CapRdyFlag), WaitFlags, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_AUTO, &ActualFlags, 1000U);
        if (PRetVal != SVC_OK) {
            PRN_RCP_MSG_LOG "Fail to query memory - get info fail! flag: 0x%x != 0x%x"
                PRN_RCP_MSG_ARG_UINT32 WaitFlags   PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_ARG_UINT32 ActualFlags PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_NG
        } else {
            const SVC_RAW_CAP_ITEM_RAW_s *pRawItmCtrl;
            const SVC_RAW_CAP_ITEM_HDS_s *pHdsItmCtrl;
            const SVC_RAW_CAP_ITEM_AAA_s *p3aItmCtrl;
            UINT32 Temp_U32;

            /* Clear the flag */
            PRetVal = AmbaKAL_EventFlagClear(&(RawCapCtrl.Flag), 0xFFFFFFFFU); PRN_RCP_ERR_HDLR

            PRN_RCP_MSG_LOG " " PRN_RCP_MSG_DBG
            PRN_RCP_MSG_LOG "====== Memory Query ======" PRN_RCP_MSG_DBG
            PRN_RCP_MSG_LOG "  VinSelectBits : 0x%x"
                PRN_RCP_MSG_ARG_UINT32 RawCapCtrl.VinSelectBits PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_DBG
            PRN_RCP_MSG_LOG "  ItmSelectBits : 0x%x"
                PRN_RCP_MSG_ARG_UINT32 RawCapCtrl.ItmSelectBits PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_DBG

            *pRawBufSize = 0U;
            if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_HDS) > 0U) {
                *pHdsBufSize = 0U;
            }
            if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_3A) > 0U) {
                *p3ABufSize = 0U;
            }

            /* Sum the each supported vin's raw/ce buffer */
            for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
                if ((RawCapCtrl.VinSelectBits & SvcRawCap_BitGet(VinIdx)) > 0U) {
                    pRawItmCtrl = &(RawCapCtrl.ItemCtrlRaw[VinIdx]);
                    pHdsItmCtrl = &(RawCapCtrl.ItemCtrlHds[VinIdx]);
                    p3aItmCtrl  = &(RawCapCtrl.ItemCtrlAaa[VinIdx]);

                    PRN_RCP_MSG_LOG "  ---- VinID(%d) ----"
                        PRN_RCP_MSG_ARG_UINT32 VinIdx PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_DBG

                    Temp_U32 = pRawItmCtrl->RawInfo.Pitch * pRawItmCtrl->RawInfo.Height;
                    if (pRawItmCtrl->RawInfo.VinType == SVC_RAW_CAP_VIN_TYPE_YUV) {
                        Temp_U32 *= 2U; // YUV422
                    }
                    Temp_U32 = GetAlignedValU32(Temp_U32, 64U);

                    *pRawBufSize += Temp_U32;
                    PRN_RCP_MSG_LOG "          Raw Buf Size: 0x%x"
                        PRN_RCP_MSG_ARG_UINT32 Temp_U32 PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_DBG

                    if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_HDS) > 0U) {
                        Temp_U32 = pHdsItmCtrl->HdsInfo.Pitch * pHdsItmCtrl->HdsInfo.Height;
                        Temp_U32 = GetAlignedValU32(Temp_U32, 64U);
                        *pHdsBufSize += Temp_U32;
                        PRN_RCP_MSG_LOG "          Hds Buf Size: 0x%x"
                            PRN_RCP_MSG_ARG_UINT32 Temp_U32 PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_DBG
                    }

                    if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_3A) > 0U) {
                        if ((p3aItmCtrl->State & SVC_RAW_CAP_ITEM_RDY) > 0U) {
                            Temp_U32 = p3aItmCtrl->AaaInfo.BufSize;
                            Temp_U32 = GetAlignedValU32(Temp_U32, 64U);
                        } else {
                            Temp_U32 = 0U;
                            RetVal = SVC_NG;
                            PRN_RCP_MSG_LOG "Fail to query memory - need to config aaa setting first!" PRN_RCP_MSG_NG
                        }
                        *p3ABufSize += Temp_U32;
                        PRN_RCP_MSG_LOG "           3A Buf Size: 0x%x"
                            PRN_RCP_MSG_ARG_UINT32 Temp_U32 PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_DBG
                    }
                }
            }

            PRN_RCP_MSG_LOG "  -------------------" PRN_RCP_MSG_DBG
            PRN_RCP_MSG_LOG "     Total Raw Size: 0x%x"
                PRN_RCP_MSG_ARG_UINT32 *pRawBufSize PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_DBG
            PRN_RCP_MSG_LOG "     Total Hds Size: 0x%x"
                PRN_RCP_MSG_ARG_UINT32 *pHdsBufSize PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_DBG
            PRN_RCP_MSG_LOG "      Total 3A Size: 0x%x"
                PRN_RCP_MSG_ARG_UINT32 *p3ABufSize PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_DBG
            PRN_RCP_MSG_LOG " " PRN_RCP_MSG_DBG
        }
    }
    AmbaMisra_TouchUnused(&RetVal);
}

static UINT32 SvcRawCap_MemCheck(void *pCheckBuf, UINT32 CheckBufSize)
{
    UINT32 RetVal = SVC_OK;
    UINT8 *pBuf = NULL;

    AmbaMisra_TypeCast(&(pBuf), &(pCheckBuf)); AmbaMisra_TouchUnused(pCheckBuf);

    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to check memory - create raw cap module first!" PRN_RCP_MSG_NG
    } else if (pBuf == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to check memory - input buffer addr should not zero!" PRN_RCP_MSG_NG
    } else if (CheckBufSize == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to check memory - input buffer size should not zero!" PRN_RCP_MSG_NG
    } else {
        UINT32 SlotIdx, ValidBuf = 1U;
        UINT8 *pBufLimit = &(pBuf[CheckBufSize - 1U]);
        const UINT8 *pSlotBufLimit;
        const SVC_RAW_CAP_MEM_SLOT_s *pMemSlot = RawCapCtrl.MemCtrl.MemSlot;

        AmbaMisra_TouchUnused(pBufLimit);

        for (SlotIdx = 0U; SlotIdx < RawCapCtrl.MemCtrl.NumSlot; SlotIdx ++) {
            pSlotBufLimit = &(pMemSlot->pMemBuf[pMemSlot->MemSize]);

            if (0U < SvcRawCap_MemCmp(pMemSlot->pMemBuf, pBufLimit)) {
                ValidBuf = 1U;
            } else if (0U < SvcRawCap_MemCmp(pBuf, pSlotBufLimit)) {
                ValidBuf = 1U;
            } else {
                ValidBuf = 0U;
                break;
            }

            pMemSlot ++;
        }

        if (ValidBuf == 0U) {
            RetVal = SVC_NG;
            PRN_RCP_MSG_LOG "Input buffer setting cross existed memory! pBuf(%p) BufSize(0x%x)"
                PRN_RCP_MSG_ARG_CPOINT pBuf         PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_ARG_UINT32 CheckBufSize PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_API
            PRN_RCP_MSG_LOG "  MemSlotIdx(%02d), pMemBuf(%p) MemSize(0x%x)"
                PRN_RCP_MSG_ARG_UINT32 SlotIdx           PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_ARG_CPOINT pMemSlot->pMemBuf PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_ARG_UINT32 pMemSlot->MemSize PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_API
        }
    }

    return RetVal;
}

static void SvcRawCap_MemSetupBufTbl(void)
{
    UINT32 SeqBufTblSize = (UINT32)sizeof(UINT8 *)*SVC_RAW_CAP_ITEM_BUF_DEPTH;
    UINT32 VinIdx, SlotIdx = 0U;
    SVC_RAW_CAP_MEM_SLOT_s *pMemSlot;
    UINT8                  *pReqBuf = NULL;
    struct { UINT8 **pBufTbl; } TypeCast2D;

    AmbaSvcWrap_MisraMemset(&TypeCast2D, 0, sizeof(TypeCast2D));

    for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
        if ((RawCapCtrl.VinSelectBits & SvcRawCap_BitGet(VinIdx)) > 0U) {

            pReqBuf = NULL;

            while (SlotIdx < RawCapCtrl.MemCtrl.NumSlot) {
                pMemSlot = &(RawCapCtrl.MemCtrl.MemSlot[SlotIdx]);

                if ((pMemSlot->SlotID > 0U) &&
                    (pMemSlot->UsedSize >= SeqBufTblSize)) {

                    pReqBuf = pMemSlot->pUsedBuf;

                    pMemSlot->UsedStatus[SVC_RAW_CAP_MEM_USAGE_RAW_TBL] += 1U;
                    pMemSlot->UsedSize -= SeqBufTblSize;
                    pMemSlot->pUsedBuf = &(pMemSlot->pUsedBuf[SeqBufTblSize]);

                    break;
                }

                SlotIdx ++;
            }

            if (pReqBuf == NULL) {
                PRN_RCP_MSG_LOG "Fail to setup raw memory - VinID(%d) raw seq buffer table setup fail"
                    PRN_RCP_MSG_ARG_UINT32 VinIdx PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_NG

                break;
            } else {
                AmbaSvcWrap_MisraMemset(&TypeCast2D, 0, sizeof(TypeCast2D));
                AmbaMisra_TypeCast(&(TypeCast2D), &(pReqBuf));
                RawCapCtrl.ItemCtrlRaw[VinIdx].pBufSeqTbl = TypeCast2D.pBufTbl;
            }
        }
    }

    if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_HDS) > 0U) {
        for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
            if ((RawCapCtrl.VinSelectBits & SvcRawCap_BitGet(VinIdx)) > 0U) {

                pReqBuf = NULL;

                while (SlotIdx < RawCapCtrl.MemCtrl.NumSlot) {
                    pMemSlot = &(RawCapCtrl.MemCtrl.MemSlot[SlotIdx]);

                    if ((pMemSlot->SlotID > 0U) &&
                        (pMemSlot->UsedSize >= SeqBufTblSize)) {

                        pReqBuf = pMemSlot->pUsedBuf;

                        pMemSlot->UsedStatus[SVC_RAW_CAP_MEM_USAGE_HDS_TBL] += 1U;
                        pMemSlot->UsedSize -= SeqBufTblSize;
                        pMemSlot->pUsedBuf = &(pMemSlot->pUsedBuf[SeqBufTblSize]);

                        break;
                    }

                    SlotIdx ++;
                }

                if (pReqBuf == NULL) {
                    PRN_RCP_MSG_LOG "Fail to setup raw memory - VinID(%d) hds seq buffer table setup fail"
                        PRN_RCP_MSG_ARG_UINT32 VinIdx PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_NG

                    break;
                } else {
                    AmbaSvcWrap_MisraMemset(&TypeCast2D, 0, sizeof(TypeCast2D));
                    AmbaMisra_TypeCast(&(TypeCast2D), &(pReqBuf));
                    RawCapCtrl.ItemCtrlHds[VinIdx].pBufSeqTbl = TypeCast2D.pBufTbl;
                }
            }
        }
    }

    AmbaMisra_TouchUnused(pReqBuf);
    AmbaMisra_TouchUnused(TypeCast2D.pBufTbl);
}

static void SvcRawCap_MemSetupRaw(UINT32 ReqMemDepth, UINT32 OneSyncBufSize)
{
    if ((ReqMemDepth > 0U) && (OneSyncBufSize > 0U)) {
        UINT32 VinIdx, SlotIdx, BufIdx;
        UINT32 ValidRawDepth, RemainSize;
        UINT32 SeqBufTblSize = (UINT32)sizeof(UINT8 *)*SVC_RAW_CAP_ITEM_BUF_DEPTH;
        UINT8 **pCurBuf;
        SVC_RAW_CAP_MEM_SLOT_s *pMemSlot;
        SVC_RAW_CAP_ITEM_RAW_s *pRawItmCtrl;

        for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
            if ((RawCapCtrl.VinSelectBits & SvcRawCap_BitGet(VinIdx)) > 0U) {
                pRawItmCtrl = &(RawCapCtrl.ItemCtrlRaw[VinIdx]);
                if (pRawItmCtrl->pBufSeqTbl == NULL) {
                    PRN_RCP_MSG_LOG "Fail to setup raw memory - VinID(%d) raw seq buffer table should not null"
                        PRN_RCP_MSG_ARG_UINT32 VinIdx PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_NG
                } else {
                    /* Reset the buffer setting */
                    pRawItmCtrl->CurSeqIdx   = 0U;
                    pRawItmCtrl->NumCaptured = 0U;
                    pRawItmCtrl->NumSeq      = 0U;
                    AmbaSvcWrap_MisraMemset(pRawItmCtrl->pBufSeqTbl, 0, SeqBufTblSize);
                    AmbaSvcWrap_MisraMemset(pRawItmCtrl->RawSeqArr,  0, sizeof(pRawItmCtrl->RawSeqArr));

                    /* Configure the buffer setting */
                    for (SlotIdx = 0U; SlotIdx < RawCapCtrl.MemCtrl.NumSlot; SlotIdx ++) {
                        if (pRawItmCtrl->NumSeq < ReqMemDepth) {
                            pMemSlot = &(RawCapCtrl.MemCtrl.MemSlot[SlotIdx]);
                            if (pMemSlot->SlotID > 0U) {
                                RemainSize = pMemSlot->UsedSize;
                                ValidRawDepth = RemainSize / OneSyncBufSize;
                                if (ValidRawDepth > 0U) {

                                    if ((pRawItmCtrl->NumSeq + ValidRawDepth) > ReqMemDepth) {
                                        ValidRawDepth = ReqMemDepth - pRawItmCtrl->NumSeq;
                                    }

                                    pCurBuf = &(pRawItmCtrl->pBufSeqTbl[pRawItmCtrl->NumSeq]);
                                    for (BufIdx = 0U; BufIdx < ValidRawDepth; BufIdx ++) {
                                        *pCurBuf = &(pMemSlot->pUsedBuf[OneSyncBufSize * BufIdx]);
                                        pCurBuf ++;
                                    }

                                    pRawItmCtrl->NumSeq += ValidRawDepth;
                                    pMemSlot->UsedStatus[SVC_RAW_CAP_MEM_USAGE_RAW] += ValidRawDepth;
                                    pMemSlot->UsedSize = RemainSize - ( OneSyncBufSize * ValidRawDepth );
                                    pMemSlot->pUsedBuf = &(pMemSlot->pUsedBuf[OneSyncBufSize * ValidRawDepth]);
                                }
                            }
                        } else {
                            break;
                        }
                    }

                }
            }
        }
    }
}

static void SvcRawCap_MemSetupHds(UINT32 ReqMemDepth, UINT32 OneSyncBufSize)
{
    if ((ReqMemDepth > 0U) && (OneSyncBufSize > 0U)) {
        UINT32 VinIdx, SlotIdx, BufIdx;
        UINT32 ValidHdsDepth, RemainSize;
        UINT32 SeqBufTblSize = (UINT32)sizeof(UINT8 *)*SVC_RAW_CAP_ITEM_BUF_DEPTH;
        UINT8 **pCurBuf;
        SVC_RAW_CAP_MEM_SLOT_s *pMemSlot;
        SVC_RAW_CAP_ITEM_HDS_s *pHdsItmCtrl;

        if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_HDS) > 0U) {
            for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
                if ((RawCapCtrl.VinSelectBits & SvcRawCap_BitGet(VinIdx)) > 0U) {
                    pHdsItmCtrl = &(RawCapCtrl.ItemCtrlHds[VinIdx]);

                    if (pHdsItmCtrl->pBufSeqTbl == NULL) {
                        PRN_RCP_MSG_LOG "Fail to setup hds memory - VinID(%d) hds seq buffer table should not null"
                            PRN_RCP_MSG_ARG_UINT32 VinIdx PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_NG
                    } else {
                        /* Reset the hds item buffer setting */
                        pHdsItmCtrl->CurSeqIdx   = 0U;
                        pHdsItmCtrl->NumCaptured = 0U;
                        pHdsItmCtrl->NumSeq      = 0U;
                        AmbaSvcWrap_MisraMemset(pHdsItmCtrl->pBufSeqTbl, 0, SeqBufTblSize);
                        AmbaSvcWrap_MisraMemset(pHdsItmCtrl->RawSeqArr,  0, sizeof(pHdsItmCtrl->RawSeqArr));

                        /* Configure the hds item buffer setting */
                        for (SlotIdx = 0U; SlotIdx < RawCapCtrl.MemCtrl.NumSlot; SlotIdx ++) {
                            if (pHdsItmCtrl->NumSeq < ReqMemDepth) {
                                pMemSlot = &(RawCapCtrl.MemCtrl.MemSlot[SlotIdx]);
                                if (pMemSlot->SlotID > 0U) {
                                    RemainSize = pMemSlot->UsedSize;
                                    ValidHdsDepth = RemainSize / OneSyncBufSize;
                                    if (ValidHdsDepth > 0U) {

                                        if ((pHdsItmCtrl->NumSeq + ValidHdsDepth) > ReqMemDepth) {
                                            ValidHdsDepth = ReqMemDepth - pHdsItmCtrl->NumSeq;
                                        }

                                        pCurBuf = &(pHdsItmCtrl->pBufSeqTbl[pHdsItmCtrl->NumSeq]);
                                        for (BufIdx = 0U; BufIdx < ValidHdsDepth; BufIdx ++) {
                                            *pCurBuf = &(pMemSlot->pUsedBuf[OneSyncBufSize * BufIdx]);
                                            pCurBuf ++;
                                        }

                                        pHdsItmCtrl->NumSeq += ValidHdsDepth;
                                        pMemSlot->UsedStatus[SVC_RAW_CAP_MEM_USAGE_HDS] += ValidHdsDepth;
                                        pMemSlot->UsedSize = RemainSize - ( OneSyncBufSize * ValidHdsDepth );
                                        pMemSlot->pUsedBuf = &(pMemSlot->pUsedBuf[OneSyncBufSize * ValidHdsDepth]);
                                    }
                                }
                            } else {
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

static void SvcRawCap_MemSetupAaa(UINT32 ReqMemDepth, UINT32 OneSyncBufSize)
{
    if ((ReqMemDepth > 0U) && (OneSyncBufSize > 0U)) {
        UINT32 VinIdx, SlotIdx, BufIdx;
        UINT32 Valid3ADepth, RemainSize;
        UINT8 **pCurBuf;
        SVC_RAW_CAP_MEM_SLOT_s *pMemSlot;
        SVC_RAW_CAP_ITEM_AAA_s *p3AItmCtrl;

        if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_3A) > 0U) {
            for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
                p3AItmCtrl = &(RawCapCtrl.ItemCtrlAaa[VinIdx]);

                /* Reset the 3a item buffer setting */
                p3AItmCtrl->CurSeqIdx   = 0U;
                p3AItmCtrl->NumCaptured = 0U;
                p3AItmCtrl->NumSeq      = 0U;
                AmbaSvcWrap_MisraMemset(p3AItmCtrl->pBufSeq,   0, sizeof(p3AItmCtrl->pBufSeq));
                AmbaSvcWrap_MisraMemset(p3AItmCtrl->RawSeqArr, 0, sizeof(p3AItmCtrl->RawSeqArr));

                /* Configure the 3a item buffer setting */
                if ((RawCapCtrl.VinSelectBits & SvcRawCap_BitGet(VinIdx)) > 0U) {

                    for (SlotIdx = 0U; SlotIdx < RawCapCtrl.MemCtrl.NumSlot; SlotIdx ++) {
                        if (p3AItmCtrl->NumSeq < ReqMemDepth) {
                            pMemSlot = &(RawCapCtrl.MemCtrl.MemSlot[SlotIdx]);
                            if (pMemSlot->SlotID > 0U) {
                                RemainSize = pMemSlot->UsedSize;
                                Valid3ADepth = RemainSize / OneSyncBufSize;
                                if (Valid3ADepth > 0U) {

                                    if ((p3AItmCtrl->NumSeq + Valid3ADepth) > ReqMemDepth) {
                                        Valid3ADepth = ReqMemDepth - p3AItmCtrl->NumSeq;
                                    }

                                    pCurBuf = &(p3AItmCtrl->pBufSeq[p3AItmCtrl->NumSeq]);
                                    for (BufIdx = 0U; BufIdx < Valid3ADepth; BufIdx ++) {
                                        *pCurBuf = &(pMemSlot->pUsedBuf[OneSyncBufSize * BufIdx]);
                                        pCurBuf ++;
                                    }

                                    p3AItmCtrl->NumSeq += Valid3ADepth;
                                    pMemSlot->UsedStatus[SVC_RAW_CAP_MEM_USAGE_3A] += Valid3ADepth;
                                    pMemSlot->UsedSize = RemainSize - ( OneSyncBufSize * Valid3ADepth );
                                    pMemSlot->pUsedBuf = &(pMemSlot->pUsedBuf[OneSyncBufSize * Valid3ADepth]);
                                }
                            }
                        } else {
                            break;
                        }
                    }
                }
            }
        }
    }
}

static void SvcRawCap_MemSetupBmp(void)
{
    if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_NON_BMP) == 0U) {
        UINT32 VinIdx;
        UINT32 CurSize, MaxReqSize = 0U;
        SVC_RAW_CAP_BMP_INFO_s *pBmpInfo = &(RawCapCtrl.BmpInfo);
        const SVC_RAW_CAP_ITEM_RAW_s *pRawItmCtrl;

        // Get the max raw width and height
        for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
            if ((RawCapCtrl.VinSelectBits & SvcRawCap_BitGet(VinIdx)) > 0U) {
                pRawItmCtrl = &(RawCapCtrl.ItemCtrlRaw[VinIdx]);

                if ((pRawItmCtrl->State & SVC_RAW_CAP_ITEM_CFG) > 0U) {
                    if (pRawItmCtrl->RawInfo.VinType == SVC_RAW_CAP_VIN_TYPE_CFA) {
                        CurSize = pRawItmCtrl->RawInfo.Width * pRawItmCtrl->RawInfo.Height;
                        CurSize *= (2U + (UINT32)sizeof(SVC_RAW_CAP_BITMAP_RGB_s));
                        if (MaxReqSize < CurSize) {
                            MaxReqSize = CurSize;
                        }
                    }
                }
            }
        }

        if (MaxReqSize > 0U) {
            UINT32 SlotIdx;
            SVC_RAW_CAP_MEM_SLOT_s *pMemSlot;

            MaxReqSize = GetAlignedValU32(MaxReqSize, 64U);

            for (SlotIdx = 0U; SlotIdx < RawCapCtrl.MemCtrl.NumSlot; SlotIdx ++) {
                pMemSlot = &(RawCapCtrl.MemCtrl.MemSlot[SlotIdx]);
                if (pMemSlot->SlotID > 0U) {
                    if (pMemSlot->UsedSize >= MaxReqSize) {

                        pBmpInfo->pBuf = pMemSlot->pUsedBuf;
                        pBmpInfo->BufSize = MaxReqSize;
                        pBmpInfo->State |= (SVC_RAW_CAP_ITEM_CFG | SVC_RAW_CAP_ITEM_MEM);

                        pMemSlot->pUsedBuf  = &(pMemSlot->pUsedBuf[MaxReqSize]);
                        pMemSlot->UsedSize += MaxReqSize;
                        pMemSlot->UsedStatus[SVC_RAW_CAP_MEM_USAGE_BMP] = 1U;

                        break;
                    }
                }
            }

            if ((pBmpInfo->State & SVC_RAW_CAP_ITEM_MEM) == 0U) {
                PRN_RCP_MSG_LOG "There is no enough memory to service raw -> bitmap converter" PRN_RCP_MSG_API
            }
        }
    }
}

static void SvcRawCap_MemSetupUnPack(void)
{
    if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_NON_UNPACK) == 0U) {
        UINT32 VinIdx;
        UINT32 CurSize, MaxReqSize = 0U;
        SVC_RAW_CAP_UNPACK_INFO_s *pUnPackInfo = &(RawCapCtrl.UnPackInfo);
        const SVC_RAW_CAP_ITEM_RAW_s *pRawItmCtrl;

        // Get the max raw width and height
        for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
            if ((RawCapCtrl.VinSelectBits & SvcRawCap_BitGet(VinIdx)) > 0U) {
                pRawItmCtrl = &(RawCapCtrl.ItemCtrlRaw[VinIdx]);

                if ((pRawItmCtrl->State & SVC_RAW_CAP_ITEM_CFG) > 0U) {
                    if (pRawItmCtrl->RawInfo.VinType == SVC_RAW_CAP_VIN_TYPE_CFA) {
                        CurSize = (pRawItmCtrl->RawInfo.Width * pRawItmCtrl->RawInfo.Height) << 1U;
                        if (MaxReqSize < CurSize) {
                            MaxReqSize = CurSize;
                        }
                    }
                }
            }
        }

        if (MaxReqSize > 0U) {
            UINT32 SlotIdx;
            SVC_RAW_CAP_MEM_SLOT_s *pMemSlot;

            MaxReqSize = GetAlignedValU32(MaxReqSize, 64U);

            for (SlotIdx = 0U; SlotIdx < RawCapCtrl.MemCtrl.NumSlot; SlotIdx ++) {
                pMemSlot = &(RawCapCtrl.MemCtrl.MemSlot[SlotIdx]);
                if (pMemSlot->SlotID > 0U) {
                    if (pMemSlot->UsedSize >= MaxReqSize) {

                        pUnPackInfo->pBuf = pMemSlot->pUsedBuf;
                        pUnPackInfo->BufSize = MaxReqSize;
                        pUnPackInfo->State |= (SVC_RAW_CAP_ITEM_CFG | SVC_RAW_CAP_ITEM_MEM);

                        pMemSlot->pUsedBuf  = &(pMemSlot->pUsedBuf[MaxReqSize]);
                        pMemSlot->UsedSize += MaxReqSize;
                        pMemSlot->UsedStatus[SVC_RAW_CAP_MEM_USAGE_UNPACK] = 1U;

                        break;
                    }
                }
            }

            if ((pUnPackInfo->State & SVC_RAW_CAP_ITEM_MEM) == 0U) {
                PRN_RCP_MSG_LOG "There is no enough memory to service unpack raw process" PRN_RCP_MSG_API
            }
        }
    }
}

static UINT32 SvcRawCap_MemSetup(UINT32 ReqMemDepth)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to setup raw cap memory - create raw cap first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to setup raw cap memory - config raw cap first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG_MEM) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to setup raw cap memory - config raw cap memory first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG_RAW) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to setup raw cap memory - config raw info first!" PRN_RCP_MSG_NG
    } else if (((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_HDS) > 0U) &&
               ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG_HDS) == 0U)) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to setup raw cap memory - config hds info first!" PRN_RCP_MSG_NG
    } else if (((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_3A) > 0U) &&
            ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG_AAA) == 0U)) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to setup raw cap memory - config 3a info first!" PRN_RCP_MSG_NG
    } else if (ReqMemDepth > SVC_RAW_CAP_ITEM_BUF_DEPTH) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to setup raw cap memory - requested capture number is out-of limitation! %d > %d!"
            PRN_RCP_MSG_ARG_UINT32 ReqMemDepth                PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_ARG_UINT32 SVC_RAW_CAP_ITEM_BUF_DEPTH PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_NG
    } else {
        UINT32 VinIdx, SlotIdx;
        UINT32 OneSyncRawSize = 0U, OneSyncHdsSize = 0U, OneSync3ASize = 0U;
        SVC_RAW_CAP_MEM_SLOT_s *pMemSlot;
        const SVC_RAW_CAP_ITEM_RAW_s *pRawItmCtrl;
        const SVC_RAW_CAP_ITEM_HDS_s *pHdsItmCtrl;
        const SVC_RAW_CAP_ITEM_AAA_s *p3AItmCtrl;

        /* Reset memory slot setting */
        for (SlotIdx = 0U; SlotIdx < RawCapCtrl.MemCtrl.NumSlot; SlotIdx ++) {
            pMemSlot = &(RawCapCtrl.MemCtrl.MemSlot[SlotIdx]);
            pMemSlot->pUsedBuf = pMemSlot->pMemAlgnedBuf;
            pMemSlot->UsedSize = pMemSlot->MemAlgnedSize;
            AmbaSvcWrap_MisraMemset(pMemSlot->UsedStatus, 0, sizeof(pMemSlot->UsedStatus));

            if (pMemSlot->pMemBuf != NULL) {
                AmbaSvcWrap_MisraMemset(pMemSlot->pMemBuf, 0, pMemSlot->MemSize);
                PRetVal = SvcRawCap_CleanCache(pMemSlot->pMemBuf, pMemSlot->MemSize);
                if (PRetVal != 0U) {
                    PRN_RCP_MSG_LOG "Fail to setup raw cap memory - memory slot %02d clean cache fail! 0x%08X"
                        PRN_RCP_MSG_ARG_UINT32 SlotIdx PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_ARG_UINT32 PRetVal PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_NG
                }
            }
        }

        /* setup raw/hds buffer table */
        SvcRawCap_MemSetupBufTbl();

        /* Query one sync each item requested size */
        SvcRawCap_MemQuery(&OneSyncRawSize, &OneSyncHdsSize, &OneSync3ASize);

        /* setup raw item memory setting */
        SvcRawCap_MemSetupRaw(ReqMemDepth, OneSyncRawSize);

        /* setup hds item memory setting */
        SvcRawCap_MemSetupHds(ReqMemDepth, OneSyncHdsSize);

        /* setup 3A item memory setting */
        SvcRawCap_MemSetupAaa(ReqMemDepth, OneSync3ASize);

        /* setup BMP memory setting */
        SvcRawCap_MemSetupBmp();

        /* setup UnPack memory setting */
        SvcRawCap_MemSetupUnPack();

        /* Check the memory setting */
        for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {

            if ((RawCapCtrl.VinSelectBits & SvcRawCap_BitGet(VinIdx)) > 0U) {

                pRawItmCtrl = &(RawCapCtrl.ItemCtrlRaw[VinIdx]);
                pHdsItmCtrl = &(RawCapCtrl.ItemCtrlHds[VinIdx]);
                p3AItmCtrl  = &(RawCapCtrl.ItemCtrlAaa[VinIdx]);

                if (pRawItmCtrl->NumSeq < ReqMemDepth) {
                    RetVal = SVC_NG;
                    PRN_RCP_MSG_LOG "Fail to setup raw cap memory - VinID(%d) raw item buffer depth(%d) smaller requested number(%d)!"
                        PRN_RCP_MSG_ARG_UINT32 VinIdx              PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_ARG_UINT32 pRawItmCtrl->NumSeq PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_ARG_UINT32 ReqMemDepth         PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_NG
                }

                if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_HDS) > 0U) {

                    if (pHdsItmCtrl->NumSeq < ReqMemDepth) {
                        RetVal = SVC_NG;
                        PRN_RCP_MSG_LOG "Fail to setup raw cap memory - VinID(%d) hds item buffer depth(%d) smaller requested number(%d)!"
                            PRN_RCP_MSG_ARG_UINT32 VinIdx              PRN_RCP_MSG_ARG_POST
                            PRN_RCP_MSG_ARG_UINT32 pHdsItmCtrl->NumSeq PRN_RCP_MSG_ARG_POST
                            PRN_RCP_MSG_ARG_UINT32 ReqMemDepth         PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_NG
                    }

                }

                if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_3A) > 0U) {

                    if (p3AItmCtrl->NumSeq < ReqMemDepth) {
                        RetVal = SVC_NG;
                        PRN_RCP_MSG_LOG "Fail to setup raw cap memory - VinID(%d) 3A item buffer depth(%d) smaller requested number(%d)!"
                            PRN_RCP_MSG_ARG_UINT32 VinIdx             PRN_RCP_MSG_ARG_POST
                            PRN_RCP_MSG_ARG_UINT32 p3AItmCtrl->NumSeq PRN_RCP_MSG_ARG_POST
                            PRN_RCP_MSG_ARG_UINT32 ReqMemDepth        PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_NG
                    }

                }
            }
        }

        if (RetVal == SVC_OK) {
            RawCapCtrlFlag |= SVC_RAW_CAP_FLAG_MEM_SETUP;
        }
    }

    return RetVal;
}

static UINT32 SvcRawCap_CapReset(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_INIT) > 0U) {
        SVC_RAW_CAP_MSG_QUE_UNIT_s MsgUnit;

        if ((RawCapCtrlFlag & (SVC_RAW_CAP_FLAG_QUERY | SVC_RAW_CAP_FLAG_CAP)) > 0U) {
            RawCapCtrlFlag &= ~(SVC_RAW_CAP_FLAG_QUERY);
            RawCapCtrlFlag &= ~(SVC_RAW_CAP_FLAG_CAP);

            /* Wait some time to make sure the original request has been abandom */
            PRetVal = AmbaKAL_TaskSleep(100U); PRN_RCP_ERR_HDLR
        }

        /* restore to buffer to free queue*/
        AmbaSvcWrap_MisraMemset(&MsgUnit, 0, sizeof(MsgUnit));
        while (0U == AmbaKAL_MsgQueueReceive(&(RawCapCtrl.MsgQue), &MsgUnit, 100U)) {
            PRetVal = AmbaKAL_MsgQueueSend(&(RawCapCtrl.FreeQue), &MsgUnit, AMBA_KAL_NO_WAIT);
            if (PRetVal != 0U) {
                PRN_RCP_MSG_LOG "Fail to restore buffer to free queue! ErrCode(0x%X)"
                    PRN_RCP_MSG_ARG_UINT32 PRetVal PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_NG
            }
        }

        /* Reset flag */
        PRetVal = AmbaKAL_EventFlagClear(&(RawCapCtrl.Flag), 0xFFFFFFFFU); PRN_RCP_ERR_HDLR
    }

    return RetVal;
}

static UINT32 SvcRawCap_CapSetup(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to setup cap cfg - create raw cap first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to setup cap cfg - config raw cap first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG_MEM) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to setup cap cfg - config raw cap memory first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG_RAW) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to setup cap cfg - config raw setting first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_MEM_SETUP) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to setup cap cfg - setup capture buffer setting first!" PRN_RCP_MSG_NG
    } else {
        UINT32 Idx, NumCapInstance = 0U, RemainBuf;
        UINT16                  *pCapID        = RawCapCtrl.CmdInfo.CapID;
        AMBA_DSP_DATACAP_CFG_s  *pCapCfg       = RawCapCtrl.CmdInfo.CapCfg;
        AMBA_DSP_DATACAP_CTRL_s *pCapCtrl      = RawCapCtrl.CmdInfo.CapCtrl;
        UINT32                  *pVinID2CapIdx = RawCapCtrl.CmdInfo.VinID2CapIdx;
        const SVC_RAW_CAP_ITEM_RAW_s *pRawItmCtrl;
        const SVC_RAW_CAP_ITEM_HDS_s *pHdsItmCtrl;
        UINT32                  SeqBufTblSize  = (UINT32)sizeof(UINT8 *)*SVC_RAW_CAP_ITEM_BUF_DEPTH;
        const void              *pCvtPoint;

        AmbaSvcWrap_MisraMemset(RawCapCtrl.CmdInfo.CapID,        0, sizeof(RawCapCtrl.CmdInfo.CapID));
        AmbaSvcWrap_MisraMemset(RawCapCtrl.CmdInfo.CapCfg,       0, sizeof(RawCapCtrl.CmdInfo.CapCfg));
        AmbaSvcWrap_MisraMemset(RawCapCtrl.CmdInfo.CapCtrl,      0, sizeof(RawCapCtrl.CmdInfo.CapCtrl));
        AmbaSvcWrap_MisraMemset(RawCapCtrl.CmdInfo.VinID2CapIdx, 0, sizeof(RawCapCtrl.CmdInfo.VinID2CapIdx));

        /* configure the dsp capture setting */
        for (Idx = 0U; Idx < AMBA_DSP_MAX_VIN_NUM; Idx ++) {
            if ((RawCapCtrl.VinSelectBits & SvcRawCap_BitGet(Idx)) > 0U) {

                pRawItmCtrl = &(RawCapCtrl.ItemCtrlRaw[Idx]);
                pHdsItmCtrl = &(RawCapCtrl.ItemCtrlHds[Idx]);

                RemainBuf = pRawItmCtrl->NumSeq;

                if (RemainBuf == 0U) {
                    RetVal = SVC_NG;
                    PRN_RCP_MSG_LOG "Fail to setup cap cfg - raw buffer depth should not zero!" PRN_RCP_MSG_NG
                } else if (pRawItmCtrl->pBufSeqTbl == NULL) {
                    RetVal = SVC_NG;
                    PRN_RCP_MSG_LOG "Fail to setup cap cfg - VinID(%d) raw seq buffer table should not null"
                        PRN_RCP_MSG_ARG_UINT32 Idx PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_NG
                } else if (((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_HDS) > 0U) && (pHdsItmCtrl->pBufSeqTbl == NULL)) {
                    RetVal = SVC_NG;
                    PRN_RCP_MSG_LOG "Fail to setup cap cfg - VinID(%d) hds seq buffer table should not null"
                        PRN_RCP_MSG_ARG_UINT32 Idx PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_NG
                } else {
                    pCapID[NumCapInstance] = (UINT16)NumCapInstance;

                    pCapCfg[NumCapInstance].CapDataType = DSP_DATACAP_TYPE_RAW;
                    if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_HDS) > 0U) {
                        pCapCfg[NumCapInstance].AuxDataNeeded = 1;
                    }
                    pCapCfg[NumCapInstance].Index     = (UINT16)Idx;
                    pCapCfg[NumCapInstance].AllocType = ALLOC_EXTERNAL_DISTINCT;
                    if (RemainBuf > 64U) {
                        pCapCfg[NumCapInstance].BufNum = 64;
                        RemainBuf -= 64U;
                    } else {
                        pCapCfg[NumCapInstance].BufNum = (UINT16)RemainBuf;
                        RemainBuf = 0U;
                    }

                    if (RawCapCtrl.ItemCtrlRaw[Idx].RawInfo.CmprType >= (UINT32)SVC_RAW_CAP_VIN_COMPACT_BASE) {
                        pCapCfg[NumCapInstance].CmprRate = 0U;
                        pCapCfg[NumCapInstance].CmptRate = (UINT8)(RawCapCtrl.ItemCtrlRaw[Idx].RawInfo.CmprType - 255U);
                    } else {
                        pCapCfg[NumCapInstance].CmprRate = (UINT8) (RawCapCtrl.ItemCtrlRaw[Idx].RawInfo.CmprType);
                    }
                    pCapCfg[NumCapInstance].DataBuf.Pitch         = (UINT16)(pRawItmCtrl->RawInfo.Pitch);
                    pCapCfg[NumCapInstance].DataBuf.Window.Width  = (UINT16)(pRawItmCtrl->RawInfo.Width);
                    pCapCfg[NumCapInstance].DataBuf.Window.Height = (UINT16)(pRawItmCtrl->RawInfo.Height);

                    AmbaMisra_TypeCast(&(pCapCfg[NumCapInstance].DataBuf.BaseAddr), &(pRawItmCtrl->pBufSeqTbl[0]));
                    pCvtPoint = pRawItmCtrl->pBufSeqTbl;
                    AmbaMisra_TypeCast(&(pCapCfg[NumCapInstance].pBufTbl), &(pCvtPoint));
                    PRetVal = SvcRawCap_CleanCache(pRawItmCtrl->pBufSeqTbl, SeqBufTblSize);
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;
                        PRN_RCP_MSG_LOG "Fail to setup cap cfg - raw buffer table clean cache fail! ErrCode(0x%X)"
                            PRN_RCP_MSG_ARG_UINT32 PRetVal PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_NG
                    }

                    if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_HDS) > 0U) {
                        pCapCfg[NumCapInstance].AuxDataBuf.Pitch         = (UINT16)(pHdsItmCtrl->HdsInfo.Pitch);
                        pCapCfg[NumCapInstance].AuxDataBuf.Window.Width  = (UINT16)(pHdsItmCtrl->HdsInfo.Width);
                        pCapCfg[NumCapInstance].AuxDataBuf.Window.Height = (UINT16)(pHdsItmCtrl->HdsInfo.Height);
                        AmbaMisra_TypeCast(&(pCapCfg[NumCapInstance].AuxDataBuf.BaseAddr), &(pHdsItmCtrl->pBufSeqTbl[0]));
                        pCvtPoint = pHdsItmCtrl->pBufSeqTbl;
                        AmbaMisra_TypeCast(&(pCapCfg[NumCapInstance].pAuxBufTbl), &(pCvtPoint));
                        PRetVal = SvcRawCap_CleanCache(pHdsItmCtrl->pBufSeqTbl, SeqBufTblSize);
                        if (PRetVal != 0U) {
                            RetVal = SVC_NG;
                            PRN_RCP_MSG_LOG "Fail to setup cap cfg - hds buffer table clean cache fail! ErrCode(0x%X)"
                                PRN_RCP_MSG_ARG_UINT32 PRetVal PRN_RCP_MSG_ARG_POST
                            PRN_RCP_MSG_NG
                        }
                    }

                    pCapCtrl[NumCapInstance].CapNum = pRawItmCtrl->NumSeq;

                    pVinID2CapIdx[Idx] = NumCapInstance;

                    NumCapInstance ++;

                    AmbaMisra_TouchUnused(&RemainBuf);
                }

                if (RetVal != 0U) {
                    break;
                }
            }
        }

        RawCapCtrl.CmdInfo.NumCapInstance = NumCapInstance;

        if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_MSG_DBG) > 0U) {
            SvcRawCap_DbgInfo();
        }

        /* configure capture */
        if (RetVal == SVC_OK) {
            for (Idx = 0U; Idx < RawCapCtrl.CmdInfo.NumCapInstance; Idx ++) {
                if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CAP_INT_BUF) > 0U) {
                    PRetVal = 0U;
                } else {
                    PRetVal = AmbaDSP_DataCapCfg(pCapID[Idx], &(pCapCfg[Idx]));
                }
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;

                    PRN_RCP_MSG_LOG "Fail to setup cap cfg - config capture fail! cap id(%d) fail! VinID(%d), ErrCode(0x%X)"
                        PRN_RCP_MSG_ARG_UINT32 Idx                PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_ARG_UINT32 pCapCfg[Idx].Index PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_ARG_UINT32 PRetVal            PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_NG

                    break;
                } else {
                    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CAP_INT_BUF) > 0U) {
                        RawCapCtrl.ItemCtrlRaw[pCapCfg[Idx].Index].CurSeqIdx = 0xDEADBEEFU;
                        RawCapCtrl.ItemCtrlHds[pCapCfg[Idx].Index].CurSeqIdx = 0xDEADBEEFU;
                    } else {
                        RawCapCtrl.ItemCtrlRaw[pCapCfg[Idx].Index].CurSeqIdx = pCapCfg[Idx].BufNum;

                        if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_HDS) > 0U) {
                            RawCapCtrl.ItemCtrlHds[pCapCfg[Idx].Index].CurSeqIdx = pCapCfg[Idx].BufNum;
                        }
                    }
                }
            }

            if (RetVal == SVC_OK) {
                RawCapCtrlFlag |= SVC_RAW_CAP_FLAG_CAP_SETUP;
                PRN_RCP_MSG_LOG "Successful to setup cap cfg!" PRN_RCP_MSG_OK
            }
        }
    }

    return RetVal;
}

static UINT32 SvcRawCap_CapStart(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to start capture - create raw cap first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to start capture - config raw cap first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG_MEM) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to start capture - config raw cap memory first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG_RAW) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to start capture - config raw setting first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_MEM_SETUP) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to start capture - setup memory setting first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_TASK_SETUP) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to start capture - create capture task first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CAP_SETUP) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to start capture - setup capture setting first!" PRN_RCP_MSG_NG
    } else if (RawCapCtrl.CmdInfo.NumCapInstance == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to start capture - capture instance number should not zero!" PRN_RCP_MSG_NG
    } else {
        UINT64 AttachedRawSeq = 0ULL;

        RawCapCtrlFlag |= SVC_RAW_CAP_FLAG_CAP;

        if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CAP_INT_BUF) > 0U) {
            PRetVal = 0U;
        } else {
            PRetVal = AmbaDSP_DataCapCtrl((UINT16)(RawCapCtrl.CmdInfo.NumCapInstance)
                                         , RawCapCtrl.CmdInfo.CapID
                                         , RawCapCtrl.CmdInfo.CapCtrl
                                         , &AttachedRawSeq);
        }
        if (PRetVal != 0U) {
            RetVal = SVC_NG;

            PRN_RCP_MSG_LOG "Fail to setup cap cfg - start capture fail! ErrCode(0x%X)"
                PRN_RCP_MSG_ARG_UINT32 PRetVal PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_NG

        } else {
            PRN_RCP_MSG_LOG "Successful to start capture!" PRN_RCP_MSG_OK

            /* Trigger ituner save */
            if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_TUNER) > 0U) {
                RetVal = SvcRawCap_DumpItn();
                if (RetVal != SVC_OK) {
                    PRN_RCP_MSG_LOG "Fail to capture ituner fail!" PRN_RCP_MSG_NG
                }
            }
        }
    }

    return RetVal;
}

static UINT32 SvcRawCap_CapUpdate(UINT32 VinID)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to update capture buffer - create raw cap first!" PRN_RCP_MSG_NG
    } else if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to update capture buffer - VinID(%d) is out-of range"
            PRN_RCP_MSG_ARG_UINT32 VinID PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_NG
    } else {
        SVC_RAW_CAP_ITEM_RAW_s *pRawItmCtrl = &(RawCapCtrl.ItemCtrlRaw[VinID]);
        SVC_RAW_CAP_ITEM_HDS_s *pHdsItmCtrl = &(RawCapCtrl.ItemCtrlHds[VinID]);
        const void *pBufTbl;

        if (pRawItmCtrl->pBufSeqTbl == NULL) {
            RetVal = SVC_NG;
            PRN_RCP_MSG_LOG "Fail to setup hds memory - VinID(%d) raw seq buffer table should not null"
                PRN_RCP_MSG_ARG_UINT32 VinID PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_NG
        } else if ((pHdsItmCtrl->pBufSeqTbl == NULL) && ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_HDS) > 0U)) {
            RetVal = SVC_NG;
            PRN_RCP_MSG_LOG "Fail to setup hds memory - VinID(%d) hds seq buffer table should not null"
                PRN_RCP_MSG_ARG_UINT32 VinID PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_NG
        } else {
            if ((pRawItmCtrl->NumSeq > 64U) &&
                (pRawItmCtrl->CurSeqIdx < pRawItmCtrl->NumSeq) &&
                (pRawItmCtrl->NumCaptured >= SVC_RAW_CAP_BUF_THRESHOLD)) {

                AMBA_DSP_DATACAP_BUF_CFG_s CapBufUpd;
                UINT64 AttachedRawSeq;

                AmbaSvcWrap_MisraMemset(&CapBufUpd, 0, sizeof(CapBufUpd));
                CapBufUpd.BufNum    = 1;
                CapBufUpd.AllocType = ALLOC_EXTERNAL_DISTINCT;
                pBufTbl = &(pRawItmCtrl->pBufSeqTbl[pRawItmCtrl->CurSeqIdx]);
                AmbaMisra_TypeCast(&(CapBufUpd.BufAddr), &(pBufTbl));
                AmbaMisra_TypeCast(&(CapBufUpd.pBufTbl), &(pBufTbl));

                if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_HDS) > 0U) {
                    pBufTbl = &(pHdsItmCtrl->pBufSeqTbl[pHdsItmCtrl->CurSeqIdx]);
                    AmbaMisra_TypeCast(&(CapBufUpd.AuxBufAddr), &(pBufTbl));
                    AmbaMisra_TypeCast(&(CapBufUpd.pAuxBufTbl), &(pBufTbl));
                }

                PRetVal = AmbaDSP_UpdateCapBuffer(RawCapCtrl.CmdInfo.CapID[RawCapCtrl.CmdInfo.VinID2CapIdx[VinID]], &(CapBufUpd), &AttachedRawSeq);
                if (PRetVal != 0U) {
                    UINT32 TmpU32 = (UINT32)(RawCapCtrl.CmdInfo.CapID[RawCapCtrl.CmdInfo.VinID2CapIdx[VinID]]);
                    TmpU32 &= 0x0000FFFFU;
                    PRN_RCP_MSG_LOG "Fail to capture raw - update CapID(%d) next buffer fail!"
                        PRN_RCP_MSG_ARG_UINT32 TmpU32 PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_NG
                } else {
                    pRawItmCtrl->CurSeqIdx += 1U;
                    if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_HDS) > 0U) {
                        pHdsItmCtrl->CurSeqIdx += 1U;
                    }
                }
            }
        }
    }

    return RetVal;
}

static UINT32 SvcRawCap_CapAaa(UINT32 VinID, UINT32 RawCapSeq)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cap aaa - create raw cap first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cap aaa - config raw cap first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG_AAA) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cap aaa - config aaa setting first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_MEM_SETUP) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cap aaa - setup capture buffer setting first!" PRN_RCP_MSG_NG
    } else if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cap aaa - VinID(%d) is out-of range"
            PRN_RCP_MSG_ARG_UINT32 VinID PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_NG
    } else {
        SVC_RAW_CAP_ITEM_AAA_s *p3AItmCtrl  = &(RawCapCtrl.ItemCtrlAaa[VinID]);

        if ((p3AItmCtrl->State & SVC_RAW_CAP_ITEM_RDY) == 0U) {
            RetVal = SVC_NG;
            PRN_RCP_MSG_LOG "Fail to cap aaa - configure aaa first!" PRN_RCP_MSG_NG

        } else if (RawCapCtrl.pExtraCapFunc == NULL) {
            RetVal = SVC_NG;
            PRN_RCP_MSG_LOG "Fail to cap aaa - need to configure the extra capture callback first!" PRN_RCP_MSG_NG
        } else {
            UINT32 ImgChIdx;
            UINT8 *pCurBuf;
            SVC_RAW_CAP_AAA_DATA_INFO_s AaaDataInfo;

            pCurBuf = p3AItmCtrl->pBufSeq[p3AItmCtrl->NumCaptured];

            for (ImgChIdx = 0U; ImgChIdx < SVC_RAW_CAP_3A_MAX_CHAN; ImgChIdx ++) {
                if ((p3AItmCtrl->AaaInfo.ImgChanSelectBits & SvcRawCap_BitGet(ImgChIdx)) > 0U) {
                    AmbaSvcWrap_MisraMemset(&AaaDataInfo, 0, sizeof(AaaDataInfo));
                    AaaDataInfo.VinID = VinID;
                    AaaDataInfo.ChanID = ImgChIdx;
                    AaaDataInfo.CapSeq = RawCapSeq;
                    AaaDataInfo.pDataBuf    = &(pCurBuf[p3AItmCtrl->AaaInfo.ImgAaaBufOfs[ImgChIdx]]);
                    AaaDataInfo.DataBufSize = p3AItmCtrl->AaaInfo.ImgAaaBufSize[ImgChIdx];
                    PRetVal = (RawCapCtrl.pExtraCapFunc)(SVC_RAW_CAP_CMD_AAA_DATA_CAP, &AaaDataInfo);
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;
                        PRN_RCP_MSG_LOG "Fail to cap aaa - capture extra data fail!" PRN_RCP_MSG_NG
                    } else {
                        PRetVal = p3AItmCtrl->NumCaptured + 1U;
                        PRN_RCP_MSG_LOG "Capture VinID(%d) no.%03d image algo channel(%d) data"
                            PRN_RCP_MSG_ARG_UINT32 VinID    PRN_RCP_MSG_ARG_POST
                            PRN_RCP_MSG_ARG_UINT32 PRetVal  PRN_RCP_MSG_ARG_POST
                            PRN_RCP_MSG_ARG_UINT32 ImgChIdx PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_OK
                    }
                }

                if (RetVal != SVC_OK) {
                    break;
                }
            }

            if (RetVal == SVC_OK) {
                p3AItmCtrl->CurSeqIdx   += 1U;
                p3AItmCtrl->NumCaptured += 1U;
            }
        }

    }

    return RetVal;
}

static UINT32 SvcRawCap_CapDone(UINT32 CapNum, UINT32 IsBlocked)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    UINT32 VinIdx;

    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CAP_INT_BUF) > 0U) {
        UINT32 VinStopBits = 0U;
        INT32 TimeOut = 1000;
        SVC_RAW_CAP_ITEM_RAW_s *pItmRawCtrl;

        /* Wait all vin stop */
        do {
            for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
                if ((RawCapCtrl.VinSelectBits & SvcRawCap_BitGet(VinIdx)) > 0U) {

                    pItmRawCtrl = &(RawCapCtrl.ItemCtrlRaw[VinIdx]);

                    if (pItmRawCtrl->CurSeqIdx == pItmRawCtrl->NumCaptured) {
                        VinStopBits = SvcRawCap_BitGet(VinIdx);
                    }

                    if (pItmRawCtrl->NumCaptured > 0U) {
                        pItmRawCtrl->CurSeqIdx = pItmRawCtrl->NumCaptured;
                    }
                }
            }

            if (RawCapCtrl.VinSelectBits != VinStopBits) {
                TimeOut -= 100;
                PRetVal = AmbaKAL_TaskSleep(100U); PRN_RCP_ERR_HDLR
            } else {
                break;
            }

        } while (TimeOut > 0);

        if (RawCapCtrl.VinSelectBits != VinStopBits) {
            PRN_RCP_MSG_LOG "Fail to capture raw - wait vin stop timeout!" PRN_RCP_MSG_API
        } else {

            PRetVal = SvcRawCap_CapReset(); PRN_RCP_ERR_HDLR

            for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
                if ((RawCapCtrl.VinSelectBits & SvcRawCap_BitGet(VinIdx)) > 0U) {
                    if (RawCapCtrl.ItemCtrlRaw[VinIdx].pBufSeqTbl != NULL) {
                        RawCapCtrl.ItemCtrlRaw[VinIdx].pBufSeqTbl[0U] = RawCapCtrl.ItemCtrlRaw[VinIdx].RawInfo.pBuf;
                    }
                    RawCapCtrl.ItemCtrlRaw[VinIdx].NumCaptured = 1U;
                    if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_HDS) > 0U) {
                        if (RawCapCtrl.ItemCtrlHds[VinIdx].pBufSeqTbl != NULL) {
                            RawCapCtrl.ItemCtrlHds[VinIdx].pBufSeqTbl[0U] = RawCapCtrl.ItemCtrlHds[VinIdx].HdsInfo.pBuf;
                        }
                        RawCapCtrl.ItemCtrlHds[VinIdx].NumCaptured = 1U;
                    }
                }
            }
            PRN_RCP_MSG_LOG "Successful to wait cap ready!" PRN_RCP_MSG_OK
        }

    } else {
        /* Wait capture done */
        if (IsBlocked > 0U) {
            UINT32 ChkVal, TimeOut = 1000U * CapNum;

            do {
                ChkVal = 0U;
                for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
                    if ((RawCapCtrl.VinSelectBits & SvcRawCap_BitGet(VinIdx)) > 0U) {
                        if (RawCapCtrl.ItemCtrlRaw[VinIdx].NumCaptured < CapNum) {
                            ChkVal ++;
                        }
                    }
                }

                if (ChkVal == 0U) {
                    break;
                } else {
                    TimeOut -= 100U;
                    PRetVal = AmbaKAL_TaskSleep(100U); PRN_RCP_ERR_HDLR
                }
            } while (TimeOut > 0U);

            if (ChkVal > 0U) {
                PRN_RCP_MSG_LOG "Fail to capture raw - wait all raw data timeout!" PRN_RCP_MSG_API
            }
        }
    }

    return RetVal;
}

static UINT32 SvcRawCap_DumpRawData(UINT32 VinID, UINT32 BufIdx)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to dump raw data - VinID(%d) is out-of range"
            PRN_RCP_MSG_ARG_UINT32 VinID PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_NG
    } else if (BufIdx >= SVC_RAW_CAP_ITEM_BUF_DEPTH) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to dump raw data - BufIdx(%d) is out-of range"
            PRN_RCP_MSG_ARG_UINT32 BufIdx PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_NG
    } else if (RawCapCtrl.ItemCtrlRaw[VinID].pBufSeqTbl == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to dump raw data - VinID(%d) raw seq buf table should not null!"
            PRN_RCP_MSG_ARG_UINT32 VinID PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_NG
    } else {
        const SVC_RAW_CAP_ITEM_RAW_s *pRawItmCtrl = &(RawCapCtrl.ItemCtrlRaw[VinID]);
        UINT32 ReturnSize, WriteSize = 0U;
        UINT8 *pWriteBuf = NULL;
        char          FilePath[128];
        AMBA_FS_FILE *pFile;
        UINT32 Pitch, ComPackDataBits = 0U;
        SVC_WRAP_SNPRINT_s LocalSnPrint;

        /* Check the raw buffer is ready or not */
        if (pRawItmCtrl->CurSeqIdx <= BufIdx) {
            PRN_RCP_MSG_LOG "Warning to dump data - VinID(%d) no.%03d raw data may not ready!"
                PRN_RCP_MSG_ARG_UINT32 VinID  PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_ARG_UINT32 BufIdx PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_NG
        }

        if (pRawItmCtrl->NumSeq <= BufIdx) {
            PRN_RCP_MSG_LOG "Warning to dump data - VinID(%d) no.%03d raw data maybe invalid!"
                PRN_RCP_MSG_ARG_UINT32 VinID  PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_ARG_UINT32 BufIdx PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_NG
        }

        Pitch     = pRawItmCtrl->RawInfo.Pitch;
        pWriteBuf = pRawItmCtrl->pBufSeqTbl[BufIdx];
        WriteSize = pRawItmCtrl->RawInfo.Pitch * pRawItmCtrl->RawInfo.Height;
        if (pWriteBuf == NULL) {
            RetVal = SVC_NG;
            PRN_RCP_MSG_LOG "Warning to dump data - VinID(%d) no.%03d raw buf should not null!"
                PRN_RCP_MSG_ARG_UINT32 VinID  PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_ARG_UINT32 BufIdx PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_NG
        } else {

            PRetVal = GetAlignedValU32(WriteSize, 64U);
            PRetVal = SvcRawCap_CleanInvalid(pWriteBuf, PRetVal);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_RCP_MSG_LOG "Fail to setup cap cfg - VinID(%d) no.%03d raw data invalid cache fail! ErrCode(0x%X)"
                    PRN_RCP_MSG_ARG_UINT32 VinID   PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 BufIdx  PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 PRetVal PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_NG
            } else {
                AmbaSvcWrap_MisraMemset(FilePath, 0, sizeof(FilePath));

                if (pRawItmCtrl->RawInfo.CmprType >= (UINT32)SVC_RAW_CAP_VIN_COMPACT_BASE) {

                    switch(pRawItmCtrl->RawInfo.CmprType) {
#ifdef IK_RAW_COMPACT_8B
                    case (UINT32)IK_RAW_COMPACT_8B :
                        ComPackDataBits = 8;
                        break;
#endif
                    case (UINT32)IK_RAW_COMPACT_10B :
                        ComPackDataBits = 10;
                        break;
                    case (UINT32)IK_RAW_COMPACT_12B :
                        ComPackDataBits = 12;
                        break;
                    case (UINT32)IK_RAW_COMPACT_14B :
                        ComPackDataBits = 14;
                        break;
                    default :
                        ComPackDataBits = 0U;
                        break;
                    }

                    AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                        LocalSnPrint.pStrFmt = "%s_%d_%dx%d_%03d_cmpt%d.raw";
                        LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr   = pRawItmCtrl->FileName; LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pRawItmCtrl->RawInfo.Pitch);
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pRawItmCtrl->RawInfo.Width);
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pRawItmCtrl->RawInfo.Height);
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(BufIdx);
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(ComPackDataBits);
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                    SvcRawCap_snprintf(FilePath, (UINT32)sizeof(FilePath), &LocalSnPrint);

                } else if (pRawItmCtrl->RawInfo.CmprType == IK_RAW_COMPRESS_6P75) {

                    AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                        LocalSnPrint.pStrFmt = "%s_%d_%dx%d_%03d_cmpr.raw";
                        LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr   = pRawItmCtrl->FileName; LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pRawItmCtrl->RawInfo.Pitch);
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pRawItmCtrl->RawInfo.Width);
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pRawItmCtrl->RawInfo.Height);
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(BufIdx);
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                    SvcRawCap_snprintf(FilePath, (UINT32)sizeof(FilePath), &LocalSnPrint);

                } else {

                    AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                        LocalSnPrint.pStrFmt = "%s_%d_%dx%d_%03d_ori.raw";
                        LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr   = pRawItmCtrl->FileName; LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pRawItmCtrl->RawInfo.Pitch);
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pRawItmCtrl->RawInfo.Width);
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pRawItmCtrl->RawInfo.Height);
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(BufIdx);
                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                    SvcRawCap_snprintf(FilePath, (UINT32)sizeof(FilePath), &LocalSnPrint);

                }

                if (0U != AmbaFS_FileOpen(FilePath, "wb", &pFile)) {
                    RetVal = SVC_NG;
                    PRN_RCP_MSG_LOG "Fail to dump data - create VinID(%d) no.%03d raw data fail!"
                        PRN_RCP_MSG_ARG_UINT32 VinID  PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_ARG_UINT32 BufIdx PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_NG
                } else {

                    PRN_RCP_MSG_LOG "Write VinID(%d) no.%03d raw buffer addr(%p) size(0x%x)"
                        PRN_RCP_MSG_ARG_UINT32 VinID     PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_ARG_UINT32 BufIdx    PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_ARG_CPOINT pWriteBuf PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_ARG_UINT32 WriteSize PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_DBG

                    PRetVal = AmbaFS_FileWrite(pWriteBuf, 1, WriteSize, pFile, &ReturnSize); PRN_RCP_ERR_HDLR
                    PRetVal = AmbaFS_FileClose(pFile); PRN_RCP_ERR_HDLR

                    PRN_RCP_MSG_LOG "Successful to dump VinID(%d) no.%03d raw data to file %s"
                        PRN_RCP_MSG_ARG_UINT32 VinID    PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_ARG_UINT32 BufIdx   PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_ARG_CSTR   FilePath PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_OK

                    if ((ComPackDataBits > 0U) &&
                        ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_NON_UNPACK) == 0U)) {
                        UINT32 ReqMemSize = ( pRawItmCtrl->RawInfo.Width * pRawItmCtrl->RawInfo.Height ) << 1U;
                        UINT16 *pUnPackBuf = NULL;

                        if ((RawCapCtrl.UnPackInfo.State & SVC_RAW_CAP_ITEM_MEM) == 0U) {
                            PRN_RCP_MSG_LOG "Stop raw unpack process - configure memory first!" PRN_RCP_MSG_API
                        } else if (RawCapCtrl.UnPackInfo.BufSize < ReqMemSize) {
                            PRN_RCP_MSG_LOG "Stop raw unpack process - memory does not enough to service it!" PRN_RCP_MSG_API
                        } else {
                            AmbaMisra_TypeCast(&(pUnPackBuf), &(RawCapCtrl.UnPackInfo.pBuf));
                            if (pUnPackBuf == NULL) {
                                PRN_RCP_MSG_LOG "Stop raw unpack process - unpack buffer should not null!" PRN_RCP_MSG_API
                            } else {

                                AmbaSvcWrap_MisraMemset(pUnPackBuf, 0, ReqMemSize);

                                if (SVC_OK == SvcRawCap_UnPackRawData(ComPackDataBits,
                                                                      pRawItmCtrl->RawInfo.Pitch,
                                                                      pRawItmCtrl->RawInfo.Width,
                                                                      pRawItmCtrl->RawInfo.Height,
                                                                      pWriteBuf, pUnPackBuf)) {
                                    AmbaMisra_TypeCast(&(pWriteBuf), &(pUnPackBuf));

                                    Pitch = pRawItmCtrl->RawInfo.Width << 1U;
                                    WriteSize = Pitch * pRawItmCtrl->RawInfo.Height;

                                    AmbaSvcWrap_MisraMemset(FilePath, 0, sizeof(FilePath));

                                    AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                                        LocalSnPrint.pStrFmt = "%s_%d_%dx%d_%03d_unpack.raw";
                                        LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr   = pRawItmCtrl->FileName; LocalSnPrint.Argc ++;
                                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pRawItmCtrl->RawInfo.Pitch);
                                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pRawItmCtrl->RawInfo.Width);
                                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pRawItmCtrl->RawInfo.Height);
                                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(BufIdx);
                                        LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                                    SvcRawCap_snprintf(FilePath, (UINT32)sizeof(FilePath), &LocalSnPrint);

                                    pFile = NULL;
                                    if (0U != AmbaFS_FileOpen(FilePath, "wb", &pFile)) {
                                        RetVal = SVC_NG;
                                        PRN_RCP_MSG_LOG "Fail to dump data - create VinID(%d) no.%03d raw data fail!"
                                            PRN_RCP_MSG_ARG_UINT32 VinID  PRN_RCP_MSG_ARG_POST
                                            PRN_RCP_MSG_ARG_UINT32 BufIdx PRN_RCP_MSG_ARG_POST
                                        PRN_RCP_MSG_NG
                                    } else {
                                        if (pWriteBuf != NULL) {
                                            PRetVal = AmbaFS_FileWrite(pWriteBuf, 1, WriteSize, pFile, &ReturnSize); PRN_RCP_ERR_HDLR
                                        }
                                        PRetVal = AmbaFS_FileClose(pFile); PRN_RCP_ERR_HDLR

                                        PRN_RCP_MSG_LOG "Successful to dump VinID(%d) no.%03d raw unpack data to file %s"
                                            PRN_RCP_MSG_ARG_UINT32 VinID    PRN_RCP_MSG_ARG_POST
                                            PRN_RCP_MSG_ARG_UINT32 BufIdx   PRN_RCP_MSG_ARG_POST
                                            PRN_RCP_MSG_ARG_CSTR   FilePath PRN_RCP_MSG_ARG_POST
                                        PRN_RCP_MSG_OK
                                    }
                                }
                            }
                        }
                    }

                    if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_NON_BMP) == 0U) {
                        if (Pitch >= (pRawItmCtrl->RawInfo.Width << 1U)) {
                            AmbaSvcWrap_MisraMemset(FilePath, 0, sizeof(FilePath));

                            AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                                LocalSnPrint.pStrFmt = "%s_%d_%dx%d_%03d.bmp";
                                LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr   = pRawItmCtrl->FileName; LocalSnPrint.Argc ++;
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pRawItmCtrl->RawInfo.Pitch);
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pRawItmCtrl->RawInfo.Width);
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pRawItmCtrl->RawInfo.Height);
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(BufIdx);
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                            SvcRawCap_snprintf(FilePath, (UINT32)sizeof(FilePath), &LocalSnPrint); PRN_RCP_ERR_HDLR

                            if (PRetVal > 0U) {
                                SvcRawCap_DumpBmp(pWriteBuf,
                                                  pRawItmCtrl->RawInfo.Bayer,
                                                  Pitch >> 1U,
                                                  pRawItmCtrl->RawInfo.Width,
                                                  pRawItmCtrl->RawInfo.Height,
                                                  FilePath);
                            }
                        }
                    }
                }
            }
        }
    }

    return RetVal;
}

static UINT32 SvcRawCap_DumpHdsData(UINT32 VinID, UINT32 BufIdx)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to dump hds data - VinID(%d) is out-of range"
            PRN_RCP_MSG_ARG_UINT32 VinID PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_NG
    } else if (BufIdx >= SVC_RAW_CAP_ITEM_BUF_DEPTH) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to dump hds data - BufIdx(%d) is out-of range"
            PRN_RCP_MSG_ARG_UINT32 BufIdx PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_NG
    } else {
        if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_HDS) > 0U) {
            const SVC_RAW_CAP_ITEM_RAW_s *pRawItmCtrl = &(RawCapCtrl.ItemCtrlRaw[VinID]);
            const SVC_RAW_CAP_ITEM_HDS_s *pHdsItmCtrl = &(RawCapCtrl.ItemCtrlHds[VinID]);

            if (pHdsItmCtrl->pBufSeqTbl == NULL) {
                RetVal = SVC_NG;
                PRN_RCP_MSG_LOG "Fail to dump raw data - VinID(%d) hds seq buf table should not null!"
                    PRN_RCP_MSG_ARG_UINT32 VinID PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_NG
            } else {
                UINT32 ReturnSize, WriteSize = 0U;
                UINT8 *pWriteBuf = NULL;
                char               FilePath[128];
                AMBA_FS_FILE      *pFile;
                UINT32 ComPackDataBits = 0U;
                SVC_WRAP_SNPRINT_s LocalSnPrint;

                /* Check the hds buffer is ready or not */
                if (pHdsItmCtrl->CurSeqIdx <= BufIdx) {
                    PRN_RCP_MSG_LOG "Warning to dump data - VinID(%d) no.%03d hds data may not ready!"
                        PRN_RCP_MSG_ARG_UINT32 VinID  PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_ARG_UINT32 BufIdx PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_NG
                }

                if (pHdsItmCtrl->NumSeq <= BufIdx) {
                    PRN_RCP_MSG_LOG "Warning to dump data - VinID(%d) no.%03d hds data maybe invalid!"
                        PRN_RCP_MSG_ARG_UINT32 VinID  PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_ARG_UINT32 BufIdx PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_NG
                }

                pWriteBuf = pHdsItmCtrl->pBufSeqTbl[BufIdx];
                WriteSize = pHdsItmCtrl->HdsInfo.Pitch * pHdsItmCtrl->HdsInfo.Height;
                if (pWriteBuf == NULL) {
                    RetVal = SVC_NG;
                    PRN_RCP_MSG_LOG "Fail to dump data - write hds buf should not null! VinID(%d) no.%03d"
                        PRN_RCP_MSG_ARG_UINT32 VinID  PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_ARG_UINT32 BufIdx PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_NG
                } else {

                    /* Write the hds data to file */

                    PRetVal = GetAlignedValU32(WriteSize, 64U);
                    PRetVal = SvcRawCap_CleanInvalid(pWriteBuf, PRetVal);
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;
                        PRN_RCP_MSG_LOG "Fail to setup cap cfg - VinID(%d) no.%03d hds data invalid cache fail! ErrCode(0x%X)"
                            PRN_RCP_MSG_ARG_UINT32 VinID   PRN_RCP_MSG_ARG_POST
                            PRN_RCP_MSG_ARG_UINT32 BufIdx  PRN_RCP_MSG_ARG_POST
                            PRN_RCP_MSG_ARG_UINT32 PRetVal PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_NG
                    } else {

                        AmbaSvcWrap_MisraMemset(FilePath, 0, sizeof(FilePath));

                        if (pRawItmCtrl->RawInfo.CmprType >= (UINT32)SVC_RAW_CAP_VIN_COMPACT_BASE) {

                            switch(pRawItmCtrl->RawInfo.CmprType) {
#ifdef IK_RAW_COMPACT_8B
                            case (UINT32)IK_RAW_COMPACT_8B :
                                ComPackDataBits = 8;
                                break;
#endif
                            case (UINT32)IK_RAW_COMPACT_10B :
                                ComPackDataBits = 10;
                                break;
                            case (UINT32)IK_RAW_COMPACT_12B :
                                ComPackDataBits = 12;
                                break;
                            case (UINT32)IK_RAW_COMPACT_14B :
                                ComPackDataBits = 14;
                                break;
                            default :
                                ComPackDataBits = 0U;
                                break;
                            }

                            AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                                LocalSnPrint.pStrFmt = "%s_%d_%dx%d_%03d_hds_cmpt%d.raw";
                                LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr   = pHdsItmCtrl->FileName; LocalSnPrint.Argc ++;
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pHdsItmCtrl->HdsInfo.Pitch);
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pHdsItmCtrl->HdsInfo.Width);
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pHdsItmCtrl->HdsInfo.Height);
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(BufIdx);
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(ComPackDataBits);
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                            SvcRawCap_snprintf(FilePath, (UINT32)sizeof(FilePath), &LocalSnPrint);

                        } else if (pRawItmCtrl->RawInfo.CmprType == IK_RAW_COMPRESS_6P75) {

                            AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                                LocalSnPrint.pStrFmt = "%s_%d_%dx%d_%03d_hds_cmpr.raw";
                                LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr   = pHdsItmCtrl->FileName; LocalSnPrint.Argc ++;
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pHdsItmCtrl->HdsInfo.Pitch);
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pHdsItmCtrl->HdsInfo.Width);
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pHdsItmCtrl->HdsInfo.Height);
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(BufIdx);
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                            SvcRawCap_snprintf(FilePath, (UINT32)sizeof(FilePath), &LocalSnPrint);

                        } else {

                            AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                                LocalSnPrint.pStrFmt = "%s_%d_%dx%d_%03d_hds_ori.raw";
                                LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr   = pHdsItmCtrl->FileName; LocalSnPrint.Argc ++;
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pHdsItmCtrl->HdsInfo.Pitch);
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pHdsItmCtrl->HdsInfo.Width);
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pHdsItmCtrl->HdsInfo.Height);
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(BufIdx);
                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                            SvcRawCap_snprintf(FilePath, (UINT32)sizeof(FilePath), &LocalSnPrint);

                        }

                        if (0U != AmbaFS_FileOpen(FilePath, "wb", &pFile)) {
                            RetVal = SVC_NG;
                            PRN_RCP_MSG_LOG "Fail to dump data - create VinID(%d) no.%03d hds data fail!"
                                PRN_RCP_MSG_ARG_UINT32 VinID  PRN_RCP_MSG_ARG_POST
                                PRN_RCP_MSG_ARG_UINT32 BufIdx PRN_RCP_MSG_ARG_POST
                            PRN_RCP_MSG_NG
                        } else {

                            PRN_RCP_MSG_LOG "Write VinID(%d) no.%03d hds buffer addr(%p) size(0x%x)"
                                PRN_RCP_MSG_ARG_UINT32 VinID     PRN_RCP_MSG_ARG_POST
                                PRN_RCP_MSG_ARG_UINT32 BufIdx    PRN_RCP_MSG_ARG_POST
                                PRN_RCP_MSG_ARG_CPOINT pWriteBuf PRN_RCP_MSG_ARG_POST
                                PRN_RCP_MSG_ARG_UINT32 WriteSize PRN_RCP_MSG_ARG_POST
                            PRN_RCP_MSG_DBG

                            PRetVal = AmbaFS_FileWrite(pWriteBuf, 1, WriteSize, pFile, &ReturnSize); PRN_RCP_ERR_HDLR
                            PRetVal = AmbaFS_FileClose(pFile); PRN_RCP_ERR_HDLR

                            PRN_RCP_MSG_LOG "Successful to dump VinID(%d) no.%03d hds data to file %s"
                                PRN_RCP_MSG_ARG_UINT32 VinID    PRN_RCP_MSG_ARG_POST
                                PRN_RCP_MSG_ARG_UINT32 BufIdx   PRN_RCP_MSG_ARG_POST
                                PRN_RCP_MSG_ARG_CSTR   FilePath PRN_RCP_MSG_ARG_POST
                            PRN_RCP_MSG_OK

                            if ((ComPackDataBits > 0U) &&
                                ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_NON_UNPACK) == 0U)) {
                                UINT32 ReqMemSize = ( pHdsItmCtrl->HdsInfo.Width * pHdsItmCtrl->HdsInfo.Height ) << 1U;
                                UINT16 *pUnPackBuf = NULL;

                                if ((RawCapCtrl.UnPackInfo.State & SVC_RAW_CAP_ITEM_MEM) == 0U) {
                                    PRN_RCP_MSG_LOG "Stop hds unpack process - configure memory first!" PRN_RCP_MSG_API
                                } else if (RawCapCtrl.UnPackInfo.BufSize < ReqMemSize) {
                                    PRN_RCP_MSG_LOG "Stop hds unpack process - memory does not enough to service it!" PRN_RCP_MSG_API
                                } else {
                                    AmbaMisra_TypeCast(&(pUnPackBuf), &(RawCapCtrl.UnPackInfo.pBuf));
                                    if (pUnPackBuf == NULL) {
                                        PRN_RCP_MSG_LOG "Stop hds unpack process - unpack buffer should not null!" PRN_RCP_MSG_API
                                    } else {

                                        AmbaSvcWrap_MisraMemset(pUnPackBuf, 0, ReqMemSize);

                                        if (SVC_OK == SvcRawCap_UnPackRawData(ComPackDataBits,
                                                                              pHdsItmCtrl->HdsInfo.Pitch,
                                                                              pHdsItmCtrl->HdsInfo.Width,
                                                                              pHdsItmCtrl->HdsInfo.Height,
                                                                              pWriteBuf, pUnPackBuf)) {
                                            AmbaMisra_TypeCast(&(pWriteBuf), &(pUnPackBuf));

                                            WriteSize  = pHdsItmCtrl->HdsInfo.Width << 1U;
                                            WriteSize *= pHdsItmCtrl->HdsInfo.Height;

                                            AmbaSvcWrap_MisraMemset(FilePath, 0, sizeof(FilePath));

                                            AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                                                LocalSnPrint.pStrFmt = "%s_%d_%dx%d_%03d_hds_unpack.raw";
                                                LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr   = pHdsItmCtrl->FileName; LocalSnPrint.Argc ++;
                                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pHdsItmCtrl->HdsInfo.Pitch);
                                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pHdsItmCtrl->HdsInfo.Width);
                                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pHdsItmCtrl->HdsInfo.Height);
                                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(BufIdx);
                                                LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                                            SvcRawCap_snprintf(FilePath, (UINT32)sizeof(FilePath), &LocalSnPrint);

                                            pFile = NULL;
                                            if (0U != AmbaFS_FileOpen(FilePath, "wb", &pFile)) {
                                                RetVal = SVC_NG;
                                                PRN_RCP_MSG_LOG "Fail to dump data - create VinID(%d) no.%03d hds data fail!"
                                                    PRN_RCP_MSG_ARG_UINT32 VinID  PRN_RCP_MSG_ARG_POST
                                                    PRN_RCP_MSG_ARG_UINT32 BufIdx PRN_RCP_MSG_ARG_POST
                                                PRN_RCP_MSG_NG
                                            } else {
                                                if (pWriteBuf != NULL) {
                                                    PRetVal = AmbaFS_FileWrite(pWriteBuf, 1, WriteSize, pFile, &ReturnSize); PRN_RCP_ERR_HDLR
                                                }
                                                PRetVal = AmbaFS_FileClose(pFile); PRN_RCP_ERR_HDLR

                                                PRN_RCP_MSG_LOG "Successful to dump VinID(%d) no.%03d hds unpack data to file %s"
                                                    PRN_RCP_MSG_ARG_UINT32 VinID    PRN_RCP_MSG_ARG_POST
                                                    PRN_RCP_MSG_ARG_UINT32 BufIdx   PRN_RCP_MSG_ARG_POST
                                                    PRN_RCP_MSG_ARG_CSTR   FilePath PRN_RCP_MSG_ARG_POST
                                                PRN_RCP_MSG_OK
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return RetVal;
}

static UINT32 SvcRawCap_Dump3AData(UINT32 VinID, UINT32 BufIdx)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to dump 3a data - VinID(%d) is out-of range"
            PRN_RCP_MSG_ARG_UINT32 VinID PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_NG
    } else if (BufIdx >= SVC_RAW_CAP_ITEM_BUF_DEPTH) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to dump 3a data - BufIdx(%d) is out-of range"
            PRN_RCP_MSG_ARG_UINT32 BufIdx PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_NG
    } else {
        if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_3A) > 0U) {
            UINT32 ReturnSize, WriteSize = 0U;
            UINT8 *pWriteBuf = NULL, *pCurBuf = NULL;
            UINT32 ImgChIdx;
            const SVC_RAW_CAP_ITEM_AAA_s *p3AItmCtrl  = &(RawCapCtrl.ItemCtrlAaa[VinID]);
            char               FilePath[128];
            AMBA_FS_FILE      *pFile;
            SVC_WRAP_SNPRINT_s LocalSnPrint;

            /* Check the 3a buffer is ready or not */
            if (p3AItmCtrl->CurSeqIdx <= BufIdx) {
                PRN_RCP_MSG_LOG "Warning to dump data - VinID(%d) no.%03d 3a data may not ready!"
                    PRN_RCP_MSG_ARG_UINT32 VinID  PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 BufIdx PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_NG
            }

            if (p3AItmCtrl->NumSeq <= BufIdx) {
                PRN_RCP_MSG_LOG "Warning to dump data - VinID(%d) no.%03d 3a data maybe invalid!"
                    PRN_RCP_MSG_ARG_UINT32 VinID  PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 BufIdx PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_NG
            }

            pCurBuf = p3AItmCtrl->pBufSeq[BufIdx];

            if (pCurBuf == NULL) {
                RetVal = SVC_NG;
                PRN_RCP_MSG_LOG "Fail to dump data - aaa source buffer should not null!" PRN_RCP_MSG_NG
            } else {
                for (ImgChIdx = 0U; ImgChIdx < SVC_RAW_CAP_3A_MAX_CHAN; ImgChIdx ++) {
                    if ((p3AItmCtrl->AaaInfo.ImgChanSelectBits & SvcRawCap_BitGet(ImgChIdx)) > 0U) {
                        pWriteBuf = &(pCurBuf[p3AItmCtrl->AaaInfo.ImgAaaBufOfs[ImgChIdx]]);
                        WriteSize = p3AItmCtrl->AaaInfo.ImgAaaBufSize[ImgChIdx];

                        /* Write the 3a data to file */
                        AmbaSvcWrap_MisraMemset(FilePath, 0, sizeof(FilePath));

                        AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                            LocalSnPrint.pStrFmt = "%s_%03d_3a_%d.bin";
                            LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr   = p3AItmCtrl->FileName; LocalSnPrint.Argc ++;
                            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(BufIdx);
                            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(ImgChIdx);
                            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                        SvcRawCap_snprintf(FilePath, (UINT32)sizeof(FilePath), &LocalSnPrint);

                        if (0U != AmbaFS_FileOpen(FilePath, "wb", &pFile)) {
                            RetVal = SVC_NG;
                            PRN_RCP_MSG_LOG "Fail to dump data - create VinID(%d) no.%03d aaa img ch(%d) data fail!"
                                PRN_RCP_MSG_ARG_UINT32 VinID    PRN_RCP_MSG_ARG_POST
                                PRN_RCP_MSG_ARG_UINT32 BufIdx   PRN_RCP_MSG_ARG_POST
                                PRN_RCP_MSG_ARG_UINT32 ImgChIdx PRN_RCP_MSG_ARG_POST
                            PRN_RCP_MSG_NG
                        } else {

                            PRN_RCP_MSG_LOG "Write VinID(%d) no.%03d aaa img ch(%d) buffer addr(%p) size(0x%x)"
                                PRN_RCP_MSG_ARG_UINT32 VinID     PRN_RCP_MSG_ARG_POST
                                PRN_RCP_MSG_ARG_UINT32 BufIdx    PRN_RCP_MSG_ARG_POST
                                PRN_RCP_MSG_ARG_UINT32 ImgChIdx  PRN_RCP_MSG_ARG_POST
                                PRN_RCP_MSG_ARG_CPOINT pWriteBuf PRN_RCP_MSG_ARG_POST
                                PRN_RCP_MSG_ARG_UINT32 WriteSize PRN_RCP_MSG_ARG_POST
                            PRN_RCP_MSG_DBG

                            PRetVal = AmbaFS_FileWrite(pWriteBuf, 1, WriteSize, pFile, &ReturnSize); PRN_RCP_ERR_HDLR
                            PRetVal = AmbaFS_FileClose(pFile); PRN_RCP_ERR_HDLR

                            PRN_RCP_MSG_LOG "Successful to dump VinID(%d) no.%03d aaa img ch(%d) data to file %s"
                                PRN_RCP_MSG_ARG_UINT32 VinID    PRN_RCP_MSG_ARG_POST
                                PRN_RCP_MSG_ARG_UINT32 BufIdx   PRN_RCP_MSG_ARG_POST
                                PRN_RCP_MSG_ARG_UINT32 ImgChIdx PRN_RCP_MSG_ARG_POST
                                PRN_RCP_MSG_ARG_CSTR   FilePath PRN_RCP_MSG_ARG_POST
                            PRN_RCP_MSG_OK
                        }
                    }

                    if (RetVal != SVC_OK) {
                        break;
                    }
                }
            }
        }
    }

    return RetVal;
}

static UINT32 SvcRawCap_DumpYuvData(UINT32 VinID, UINT32 BufIdx)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to dump yuv data - VinID(%d) is out-of range"
            PRN_RCP_MSG_ARG_UINT32 VinID PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_NG
    } else if (BufIdx >= SVC_RAW_CAP_ITEM_BUF_DEPTH) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to dump yuv data - BufIdx(%d) is out-of range"
            PRN_RCP_MSG_ARG_UINT32 BufIdx PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_NG
    } else if (RawCapCtrl.ItemCtrlRaw[VinID].pBufSeqTbl == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to dump yuv data - VinID(%d) raw seq buf table should not null!"
            PRN_RCP_MSG_ARG_UINT32 VinID PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_NG
    } else {
        const SVC_RAW_CAP_ITEM_RAW_s *pRawItmCtrl = &(RawCapCtrl.ItemCtrlRaw[VinID]);
        UINT32 ReturnSize, WriteSize = 0U;
        UINT8 *pWriteBuf = NULL;
        char               FilePath[128];
        AMBA_FS_FILE      *pFile;
        SVC_WRAP_SNPRINT_s LocalSnPrint;

        /* Check the raw buffer is ready or not */
        if (pRawItmCtrl->CurSeqIdx <= BufIdx) {
            PRN_RCP_MSG_LOG "Warning to dump yuv data - VinID(%d) no.%03d raw data may not ready!"
                PRN_RCP_MSG_ARG_UINT32 VinID  PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_ARG_UINT32 BufIdx PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_NG
        }

        if (pRawItmCtrl->NumSeq <= BufIdx) {
            PRN_RCP_MSG_LOG "Warning to dump yuv data - VinID(%d) no.%03d raw data maybe invalid!"
                PRN_RCP_MSG_ARG_UINT32 VinID  PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_ARG_UINT32 BufIdx PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_NG
        }

        /* Y channel */
        pWriteBuf = pRawItmCtrl->pBufSeqTbl[BufIdx];
        WriteSize = pRawItmCtrl->RawInfo.Pitch * pRawItmCtrl->RawInfo.Height;
        if (pWriteBuf == NULL) {
            RetVal = SVC_NG;
            PRN_RCP_MSG_LOG "Warning to dump data - VinID(%d) no.%03d y-channel buf should not null!"
                PRN_RCP_MSG_ARG_UINT32 VinID  PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_ARG_UINT32 BufIdx PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_NG
        } else {
            PRetVal = GetAlignedValU32(WriteSize, 64U);
            PRetVal = SvcRawCap_CleanInvalid(pWriteBuf, PRetVal);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_RCP_MSG_LOG "Fail to setup cap cfg - VinID(%d) no.%03d y-channel data invalid cache fail! 0x%08X"
                    PRN_RCP_MSG_ARG_UINT32 VinID   PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 BufIdx  PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 PRetVal PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_NG
            } else {

                AmbaSvcWrap_MisraMemset(FilePath, 0, sizeof(FilePath));

                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "%s_%d_%dx%d_%03d.y";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr   = pRawItmCtrl->FileName; LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pRawItmCtrl->RawInfo.Pitch);
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pRawItmCtrl->RawInfo.Width);
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pRawItmCtrl->RawInfo.Height);
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(BufIdx);
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                SvcRawCap_snprintf(FilePath, (UINT32)sizeof(FilePath), &LocalSnPrint);

                if (0U != AmbaFS_FileOpen(FilePath, "wb", &pFile)) {
                    RetVal = SVC_NG;
                    PRN_RCP_MSG_LOG "Fail to dump data - create VinID(%d) no.%03d y-channel data fail!"
                        PRN_RCP_MSG_ARG_UINT32 VinID  PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_ARG_UINT32 BufIdx PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_NG
                } else {

                    PRN_RCP_MSG_LOG "Write VinID(%d) no.%03d y-channel buffer addr(%p) size(0x%x)"
                        PRN_RCP_MSG_ARG_UINT32 VinID     PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_ARG_UINT32 BufIdx    PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_ARG_CPOINT pWriteBuf PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_ARG_UINT32 WriteSize PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_DBG

                    PRetVal = AmbaFS_FileWrite(pWriteBuf, 1, WriteSize, pFile, &ReturnSize); PRN_RCP_ERR_HDLR
                    PRetVal = AmbaFS_FileClose(pFile); PRN_RCP_ERR_HDLR

                    PRN_RCP_MSG_LOG "Successful to dump VinID(%d) no.%03d y-channel data to file %s"
                        PRN_RCP_MSG_ARG_UINT32 VinID    PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_ARG_UINT32 BufIdx   PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_ARG_CSTR   FilePath PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_OK
                }
            }
        }
        /* UV channel */
        pWriteBuf = pRawItmCtrl->pBufSeqTbl[BufIdx];
        if (pWriteBuf != NULL) {
            pWriteBuf = &(pWriteBuf[WriteSize]);
        }
        WriteSize = pRawItmCtrl->RawInfo.Pitch * pRawItmCtrl->RawInfo.Height;
        if (pWriteBuf == NULL) {
            RetVal = SVC_NG;
            PRN_RCP_MSG_LOG "Warning to dump data - VinID(%d) no.%03d uv-channel buf should not null!"
                PRN_RCP_MSG_ARG_UINT32 VinID  PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_ARG_UINT32 BufIdx PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_NG
        } else {
            PRetVal = GetAlignedValU32(WriteSize, 64U);
            PRetVal = SvcRawCap_CleanInvalid(pWriteBuf, PRetVal);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_RCP_MSG_LOG "Fail to setup cap cfg - VinID(%d) no.%03d uv-channel data invalid cache fail! 0x%08X"
                    PRN_RCP_MSG_ARG_UINT32 VinID   PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 BufIdx  PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 PRetVal PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_NG
            } else {

                AmbaSvcWrap_MisraMemset(FilePath, 0, sizeof(FilePath));

                AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                    LocalSnPrint.pStrFmt = "%s_%d_%dx%d_%03d.uv";
                    LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr   = pRawItmCtrl->FileName; LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pRawItmCtrl->RawInfo.Pitch);
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pRawItmCtrl->RawInfo.Width);
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(pRawItmCtrl->RawInfo.Height);
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(BufIdx);
                    LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                SvcRawCap_snprintf(FilePath, (UINT32)sizeof(FilePath), &LocalSnPrint);

                if (0U != AmbaFS_FileOpen(FilePath, "wb", &pFile)) {
                    RetVal = SVC_NG;
                    PRN_RCP_MSG_LOG "Fail to dump data - create VinID(%d) no.%03d uv-channel data fail!"
                        PRN_RCP_MSG_ARG_UINT32 VinID  PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_ARG_UINT32 BufIdx PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_NG
                } else {

                    PRN_RCP_MSG_LOG "Write VinID(%d) no.%03d uv-channel buffer addr(%p) size(0x%x)"
                        PRN_RCP_MSG_ARG_UINT32 VinID     PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_ARG_UINT32 BufIdx    PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_ARG_CPOINT pWriteBuf PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_ARG_UINT32 WriteSize PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_DBG

                    PRetVal = AmbaFS_FileWrite(pWriteBuf, 1, WriteSize, pFile, &ReturnSize); PRN_RCP_ERR_HDLR
                    PRetVal = AmbaFS_FileClose(pFile); PRN_RCP_ERR_HDLR

                    PRN_RCP_MSG_LOG "Successful to dump VinID(%d) no.%03d uv-channel data to file %s"
                        PRN_RCP_MSG_ARG_UINT32 VinID    PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_ARG_UINT32 BufIdx   PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_ARG_CSTR   FilePath PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_OK
                }
            }
        }
    }

    return RetVal;
}

static UINT32 SvcRawCap_DumpData(UINT32 VinID)
{
    UINT32 RetVal = SVC_OK;

    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to dump data - create raw cap first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to dump data - config raw cap first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG_MEM) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to dump data - config raw cap memory first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG_RAW) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to dump data - config raw setting first!" PRN_RCP_MSG_NG
    } else if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to dump data - VinID(%d) is out-of range"
            PRN_RCP_MSG_ARG_UINT32 VinID PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_NG
    } else {
        UINT32 BufIdx;

        for (BufIdx = 0U; BufIdx < RawCapCtrl.ItemCtrlRaw[VinID].NumCaptured; BufIdx ++) {

            if (RawCapCtrl.ItemCtrlRaw[VinID].RawInfo.VinType == SVC_RAW_CAP_VIN_TYPE_CFA) {
                RetVal |= SvcRawCap_DumpRawData(VinID, BufIdx);
                RetVal |= SvcRawCap_DumpHdsData(VinID, BufIdx);
                RetVal |= SvcRawCap_Dump3AData(VinID, BufIdx);
            } else {
                RetVal  = SvcRawCap_DumpYuvData(VinID, BufIdx);
            }
        }

        if (RetVal == SVC_OK) {
            PRN_RCP_MSG_LOG "Successful to dump data!" PRN_RCP_MSG_OK
        }
    }

    return RetVal;
}

static UINT32 SvcRawCap_DumpItn(void)
{
    UINT32 RetVal = SVC_OK;
#if defined(CONFIG_ICAM_IMGITUNER_USED)
    UINT32 PRetVal;

    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to dump ituner - create raw cap first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to dump ituner - config raw cap first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG_MEM) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to dump ituner - config raw cap memory first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG_RAW) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to dump ituner - config raw setting first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_TUNER) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to dump ituner - configure ituner first!" PRN_RCP_MSG_NG
    } else {
        SVC_RAW_CAP_ITN_INFO_s *pItnInfo = &(RawCapCtrl.ItnInfo);
        UINT32 ItnBufSize = 0U;

        PRetVal = AmbaItn_QueryItuner(&ItnBufSize); PRN_RCP_ERR_HDLR

        if ((pItnInfo->State & SVC_RAW_CAP_ITEM_RDY) == 0U) {
            RetVal = SVC_NG;
            PRN_RCP_MSG_LOG "Fail to dump ituner - configure ituner setting or memory first!" PRN_RCP_MSG_NG
            AmbaMisra_TouchUnused(pItnInfo);
        } else if (ItnBufSize > pItnInfo->BufSize) {
            RetVal = SVC_NG;
            PRN_RCP_MSG_LOG "Fail to dump ituner - configure ituner memory is too small to service it! 0x%x > 0x%x"
                PRN_RCP_MSG_ARG_UINT32 ItnBufSize        PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_ARG_UINT32 pItnInfo->BufSize PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_NG
        } else if (pItnInfo->pBuf == NULL) {
            RetVal = SVC_NG;
            PRN_RCP_MSG_LOG "Fail to dump ituner - invalid ituner buffer" PRN_RCP_MSG_NG
        } else {
            UINT32 IkIdx;
            char ItnFilePath[128];
            AMBA_IK_MODE_CFG_s ImgMode;
            AMBA_ITN_SYSTEM_s Ituner_SysInfo;
            AMBA_IK_WINDOW_SIZE_INFO_s ImgWinInfo;
            AMBA_ITN_Save_Param_s Ituner_SaveParam;
            SVC_WRAP_SNPRINT_s LocalSnPrint;

            for (IkIdx = 0U; IkIdx < 32U; IkIdx ++) {
                if ((pItnInfo->IkSelectBits & SvcRawCap_BitGet(IkIdx)) > 0U) {

                    // Configure the image mode
                    AmbaSvcWrap_MisraMemset(&ImgMode, 0, sizeof(ImgMode));
                    ImgMode.ContextId = IkIdx;

                    // Change the tuner module mode as TEXT
                    PRetVal = AmbaItn_Change_Parser_Mode(TEXT_TUNE); PRN_RCP_ERR_HDLR

                    // Initial ituner module
                    PRetVal = AmbaItn_Init(pItnInfo->pBuf, pItnInfo->BufSize);
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;
                        PRN_RCP_MSG_LOG "Fail to dump ituner - initial ituner module fail! RetVal(0x%08x)"
                            PRN_RCP_MSG_ARG_UINT32 PRetVal PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_NG
                    }

                    // Update the ituner system info
                    if (RetVal == SVC_OK) {

                        AmbaSvcWrap_MisraMemset(&Ituner_SysInfo, 0, sizeof(Ituner_SysInfo));

                        // Get current system info
                        PRetVal = AmbaItn_Get_SystemInfo(&Ituner_SysInfo);
                        if (PRetVal != 0U) {
                            RetVal = SVC_NG;
                            PRN_RCP_MSG_LOG "Fail to dump ituner - get current ituner system info fail! RetVal(0x%08x)"
                                PRN_RCP_MSG_ARG_UINT32 PRetVal PRN_RCP_MSG_ARG_POST
                            PRN_RCP_MSG_NG
                        } else {

                            // Get image win info by IK context id
                            AmbaSvcWrap_MisraMemset(&ImgWinInfo, 0, sizeof(ImgWinInfo));
                            PRetVal = AmbaIK_GetWindowSizeInfo( &ImgMode, &ImgWinInfo );
                            if ( PRetVal != 0U ) {
                                RetVal = SVC_NG;
                                PRN_RCP_MSG_LOG "Fail to configure output img tuner - get current image win info fail! RetVal(0x%08x)"
                                    PRN_RCP_MSG_ARG_UINT32 PRetVal PRN_RCP_MSG_ARG_POST
                                PRN_RCP_MSG_NG
                            } else {

                                // Update system raw info
                                Ituner_SysInfo.RawStartX           = 0U;
                                Ituner_SysInfo.RawStartY           = 0U;
                                Ituner_SysInfo.RawWidth            = ImgWinInfo.VinSensor.Width;
                                Ituner_SysInfo.RawHeight           = ImgWinInfo.VinSensor.Height;
                                Ituner_SysInfo.HSubSampleFactorNum = ImgWinInfo.VinSensor.HSubSample.FactorNum;
                                Ituner_SysInfo.HSubSampleFactorDen = ImgWinInfo.VinSensor.HSubSample.FactorDen;
                                Ituner_SysInfo.VSubSampleFactorNum = ImgWinInfo.VinSensor.VSubSample.FactorNum;
                                Ituner_SysInfo.VSubSampleFactorDen = ImgWinInfo.VinSensor.VSubSample.FactorDen;
                            }
                        }

                        // Update the system info
                        if (RetVal == SVC_OK) {
                            PRetVal = AmbaItn_Set_SystemInfo(&Ituner_SysInfo);
                            if (PRetVal != 0U) {
                                RetVal = SVC_NG;
                                PRN_RCP_MSG_LOG "Fail to configure output img tuner - set ituner system info fail! RetVal(0x%08x)"
                                    PRN_RCP_MSG_ARG_UINT32 PRetVal PRN_RCP_MSG_ARG_POST
                                PRN_RCP_MSG_NG
                            }
                        }
                    }

                    // Save the ituner to file
                    if (RetVal == SVC_OK) {

                        AmbaSvcWrap_MisraMemset(ItnFilePath, 0, sizeof(ItnFilePath));

                        AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                            LocalSnPrint.pStrFmt = "%s_%02d.txt";
                            LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr   = pItnInfo->FileName; LocalSnPrint.Argc ++;
                            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64  = (UINT64)(IkIdx);
                            LocalSnPrint.Argv[LocalSnPrint.Argc].Uint64 &= 0xFFFFFFFFULL; LocalSnPrint.Argc ++;
                        SvcRawCap_snprintf(ItnFilePath, (UINT32)sizeof(ItnFilePath), &LocalSnPrint);

                        AmbaSvcWrap_MisraMemset(&Ituner_SaveParam, 0, sizeof(Ituner_SaveParam));

                        Ituner_SaveParam.Text.Filepath = ItnFilePath;

                        PRetVal = AmbaItn_Save_IDSP(&ImgMode, &Ituner_SaveParam);
                        if (PRetVal != 0U) {
                            RetVal = SVC_NG;
                            PRN_RCP_MSG_LOG "Fail to configure output img tuner - save ituner fail! RetVal(0x%08x)"
                                PRN_RCP_MSG_ARG_UINT32 PRetVal PRN_RCP_MSG_ARG_POST
                            PRN_RCP_MSG_NG
                        }
                    }

                    if (RetVal != SVC_OK) {
                        break;
                    } else {
                        PRN_RCP_MSG_LOG "Successful to dump IKContextID(%d) ituner data to file - %s"
                            PRN_RCP_MSG_ARG_UINT32 IkIdx       PRN_RCP_MSG_ARG_POST
                            PRN_RCP_MSG_ARG_CSTR   ItnFilePath PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_OK
                    }
                }
            }
        }
    }
#endif

    return RetVal;
}

static void SvcRawCap_DumpBmpDiagkInt(UINT16 *pData, UINT32 Width, UINT32 Height, UINT32 Row, UINT32 Col)
{
    if (pData != NULL) {
        UINT32 Sum = 0U, AvgCnt = 0U, TmpU32;

        /* 3x3, top-left */
        if ((Row >= 1U) && (Col >= 1U)) {
            TmpU32 = (UINT32)(pData[((Row - 1U) * Width) + (Col - 1U)]); TmpU32 &= 0xFFFFU;
            if (TmpU32 > 0U) {
                Sum += TmpU32; AvgCnt ++;
            }
        }

        /* 3x3, top-right */
        if ((Row >= 1U) && ((Col + 1U) < Width)) {
            TmpU32 = (UINT32)(pData[((Row - 1U) * Width) + (Col + 1U)]); TmpU32 &= 0xFFFFU;
            if (TmpU32 > 0U) {
                Sum += TmpU32; AvgCnt ++;
            }
        }

        /* 3x3, bottom-left */
        if (((Row + 1U) < Height) && (Col >= 1U)) {
            TmpU32 = (UINT32)(pData[((Row + 1U) * Width) + (Col - 1U)]); TmpU32 &= 0xFFFFU;
            if (TmpU32 > 0U) {
                Sum += TmpU32; AvgCnt ++;
            }
        }

        /* 3x3, bottom-right */
        if (((Row + 1U) < Height) && ((Col + 1U) < Width)) {
            TmpU32 = (UINT32)(pData[((Row + 1U) * Width) + (Col + 1U)]); TmpU32 &= 0xFFFFU;
            if (TmpU32 > 0U) {
                Sum += TmpU32; AvgCnt ++;
            }
        }

        if (AvgCnt > 0U) {
            Sum /= AvgCnt;
        }
        pData[(Row * Width) + Col] = (UINT16)Sum;
    }
}

static void SvcRawCap_DumpBmpCrossInt(UINT16 *pData, UINT32 Width, UINT32 Height, UINT32 Row, UINT32 Col)
{
    if (pData != NULL) {
        UINT32 Sum = 0U, AvgCnt = 0U, TmpU32;

        /* 3x3, top */
        if (Row >= 1U) {
            TmpU32 = (UINT32)(pData[((Row - 1U) * Width) + Col]); TmpU32 &= 0xFFFFU;
            if (TmpU32 > 0U) {
                Sum += TmpU32; AvgCnt ++;
            }
        }

        /* 3x3, bottom */
        if ((Row + 1U) < Height) {
            TmpU32 = (UINT32)(pData[((Row + 1U) * Width) + Col]); TmpU32 &= 0xFFFFU;
            if (TmpU32 > 0U) {
                Sum += TmpU32; AvgCnt ++;
            }
        }

        /* 3x3, left */
        if (Col >= 1U) {
            TmpU32 = (UINT32)(pData[(Row * Width) + (Col - 1U)]); TmpU32 &= 0xFFFFU;
            if (TmpU32 > 0U) {
                Sum += TmpU32; AvgCnt ++;
            }
        }

        /* 3x3, right */
        if ((Col + 1U) < Width) {
            TmpU32 = (UINT32)(pData[(Row * Width) + (Col + 1U)]); TmpU32 &= 0xFFFFU;
            if (TmpU32 > 0U) {
                Sum += TmpU32; AvgCnt ++;
            }
        }

        if (AvgCnt > 0U) {
            Sum /= AvgCnt;
        }
        pData[(Row * Width) + Col] = (UINT16)Sum;

    }
}

static void SvcRawCap_DumpBmpBayer2Red(UINT32 Bayer, UINT32 Pitch, UINT32 Width, UINT32 Height, const UINT16 *pCurRawBuf, UINT16 *pWorkBuf, SVC_RAW_CAP_BITMAP_RGB_s *pBmpBuf)
{
    if ((pCurRawBuf != NULL) && (pWorkBuf != NULL) && (pBmpBuf != NULL)) {
        UINT32 Row, Col;
        UINT32 Row1st, Col1st;
        UINT32 RowInt, ColInt;
        UINT32 BmpRow;
        UINT32 PixelValue = 0U;

        /* Get Red Channel Pixel Value */
        switch (Bayer) {
        case AMBA_SENSOR_BAYER_PATTERN_RG :
            Row1st = 0U;
            Col1st = 0U;
            RowInt = 1U;
            ColInt = 1U;
            break;
        case AMBA_SENSOR_BAYER_PATTERN_BG :
            Row1st = 1U;
            Col1st = 1U;
            RowInt = 0U;
            ColInt = 0U;
            break;
        case AMBA_SENSOR_BAYER_PATTERN_GR :
            Row1st = 0U;
            Col1st = 1U;
            RowInt = 1U;
            ColInt = 0U;
            break;
        case AMBA_SENSOR_BAYER_PATTERN_GB :
            Row1st = 1U;
            Col1st = 0U;
            RowInt = 0U;
            ColInt = 1U;
            break;
        default :
            Row1st = 0xFFFFFFFFU;
            Col1st = 0xFFFFFFFFU;
            RowInt = 0xFFFFFFFFU;
            ColInt = 0xFFFFFFFFU;
            break;
        }

        if ((Row1st < Height) && (Col1st < Width)) {

            /* U16-to-U16, U14, U12 or U10 from raw buffer to working buffer */
            for(Row = Row1st; Row < Height; Row +=2U) {
                for (Col = Col1st; Col < Width; Col +=2U) {
                    PixelValue = (UINT32)(pCurRawBuf[(Row * Pitch) + Col]);
                    PixelValue &= 0xFFFFU;
                    PixelValue &= (SvcRawCap_BitGet(SVC_RAW_CAP_VIN_RESOLUTION + 1U) - 1U);
                    pWorkBuf[(Row * Width) + Col] = (UINT16)PixelValue;
                }
            }

            /* Interpolation */
            /* --- 1st --- */
            /*  O |   | O  */
            /* ----------- */
            /*    | ? |    */
            /* ----------- */
            /*  O |   | O  */
            for (Row = RowInt; Row < Height; Row +=2U) {
                for (Col = ColInt; Col < Width; Col +=2U) {
                    SvcRawCap_DumpBmpDiagkInt(pWorkBuf, Width, Height, Row, Col);
                }
            }

            /* --- 2nd --- */
            /*    | O |    */
            /* ----------- */
            /*  O | ? | O  */
            /* ----------- */
            /*    | O |    */
            for (Row = 0U; Row < Height; Row ++) {
                for (Col = 0U; Col < Width; Col ++) {
                    if (pWorkBuf[(Row * Width) + Col] == 0U) {
                        SvcRawCap_DumpBmpCrossInt(pWorkBuf, Width, Height, Row, Col);
                    }
                }
            }

            /* U16-to-U8 */
            for (Row = 0U; Row < Height; Row ++) {
                BmpRow = (Height - Row) - 1U;
                for (Col = 0U; Col < Width; Col ++) {
                    PixelValue = (UINT32)(pWorkBuf[(Row * Width) + Col]);
                    PixelValue &= 0xFFFFU;
                    PixelValue >>= (SVC_RAW_CAP_VIN_RESOLUTION - 8U);
                    pBmpBuf[(BmpRow * Width) + Col].Pixel.Red = (UINT8)PixelValue;
                }
            }

        }
    }
}

static void SvcRawCap_DumpBmpBayer2Blue(UINT32 Bayer, UINT32 Pitch, UINT32 Width, UINT32 Height, const UINT16 *pCurRawBuf, UINT16 *pWorkBuf, SVC_RAW_CAP_BITMAP_RGB_s *pBmpBuf)
{
    if ((pCurRawBuf != NULL) && (pWorkBuf != NULL) && (pBmpBuf != NULL)) {
        UINT32 Row, Col;
        UINT32 Row1st, Col1st;
        UINT32 RowInt, ColInt;
        UINT32 BmpRow;
        UINT32 PixelValue = 0U;

        /* Get Red Channel Pixel Value */
        switch (Bayer) {
        case AMBA_SENSOR_BAYER_PATTERN_RG :
            Row1st = 1U;
            Col1st = 1U;
            RowInt = 0U;
            ColInt = 0U;
            break;
        case AMBA_SENSOR_BAYER_PATTERN_BG :
            Row1st = 0U;
            Col1st = 0U;
            RowInt = 1U;
            ColInt = 1U;
            break;
        case AMBA_SENSOR_BAYER_PATTERN_GR :
            Row1st = 1U;
            Col1st = 0U;
            RowInt = 0U;
            ColInt = 1U;
            break;
        case AMBA_SENSOR_BAYER_PATTERN_GB :
            Row1st = 0U;
            Col1st = 1U;
            RowInt = 1U;
            ColInt = 0U;
            break;
        default :
            Row1st = 0xFFFFFFFFU;
            Col1st = 0xFFFFFFFFU;
            RowInt = 0xFFFFFFFFU;
            ColInt = 0xFFFFFFFFU;
            break;
        }

        if ((Row1st < Height) && (Col1st < Width)) {

            /* U16-to-U16, U14, U12 or U10 from raw buffer to working buffer */
            for(Row = Row1st; Row < Height; Row +=2U) {
                for (Col = Col1st; Col < Width; Col +=2U) {
                    PixelValue = (UINT32)(pCurRawBuf[(Row * Pitch) + Col]);
                    PixelValue &= 0xFFFFU;
                    PixelValue &= (SvcRawCap_BitGet(SVC_RAW_CAP_VIN_RESOLUTION + 1U) - 1U);
                    pWorkBuf[(Row * Width) + Col] = (UINT16)PixelValue;
                }
            }

            /* Interpolation */
            /* --- 1st --- */
            /*  O |   | O  */
            /* ----------- */
            /*    | ? |    */
            /* ----------- */
            /*  O |   | O  */
            for (Row = RowInt; Row < Height; Row +=2U) {
                for (Col = ColInt; Col < Width; Col +=2U) {
                    SvcRawCap_DumpBmpDiagkInt(pWorkBuf, Width, Height, Row, Col);
                }
            }

            /* --- 2nd --- */
            /*    | O |    */
            /* ----------- */
            /*  O | ? | O  */
            /* ----------- */
            /*    | O |    */
            for (Row = 0U; Row < Height; Row ++) {
                for (Col = 0U; Col < Width; Col ++) {
                    if (pWorkBuf[(Row * Width) + Col] == 0U) {
                        SvcRawCap_DumpBmpCrossInt(pWorkBuf, Width, Height, Row, Col);
                    }
                }
            }

            /* U16-to-U8 for bitmap Blue channel */
            for (Row = 0U; Row < Height; Row ++) {
                BmpRow = (Height - Row) - 1U;
                for (Col = 0U; Col < Width; Col ++) {
                    PixelValue = (UINT32)(pWorkBuf[(Row * Width) + Col]);
                    PixelValue &= 0xFFFFU;
                    PixelValue >>= (SVC_RAW_CAP_VIN_RESOLUTION - 8U);
                    pBmpBuf[(BmpRow * Width) + Col].Pixel.Blue = (UINT8)PixelValue;
                }
            }

        }
    }
}

static void SvcRawCap_DumpBmpBayer2Green(UINT32 Bayer, UINT32 Pitch, UINT32 Width, UINT32 Height, const UINT16 *pCurRawBuf, UINT16 *pWorkBuf, SVC_RAW_CAP_BITMAP_RGB_s *pBmpBuf)
{
    if ((pCurRawBuf != NULL) && (pWorkBuf != NULL) && (pBmpBuf != NULL)) {
        UINT32 Row, Col, Type = 0xFFFFFFFFU, GreenIdx[4];
        UINT32 BmpRow;
        UINT32 PixelValue = 0U;

        /* Get Red Channel Pixel Value */
        switch (Bayer) {
        case AMBA_SENSOR_BAYER_PATTERN_RG :
        case AMBA_SENSOR_BAYER_PATTERN_BG :
            Type = 0U;
            break;
        case AMBA_SENSOR_BAYER_PATTERN_GR :
        case AMBA_SENSOR_BAYER_PATTERN_GB :
            Type = 1U;
            break;
        default :
            Type = 0xFFFFFFFFU;
            break;
        }

        if (Type <= 1U) {
            for(Row = 0U; Row < Height; Row +=2U) {
                for (Col = 0U; Col < Width; Col +=2U) {

                    if (Type == 0U) {
                        GreenIdx[0] = (Row * Pitch) + Col + 1U;
                        GreenIdx[1] = (Row * Width) + Col + 1U;
                        GreenIdx[2] = ((Row + 1U) * Pitch) + Col;
                        GreenIdx[3] = ((Row + 1U) * Width) + Col;
                    } else {
                        GreenIdx[0] = (Row * Pitch) + Col;
                        GreenIdx[1] = (Row * Width) + Col;
                        GreenIdx[2] = ((Row + 1U) * Pitch) + (Col + 1U);
                        GreenIdx[3] = ((Row + 1U) * Width) + (Col + 1U);
                    }

                    PixelValue = (UINT32)(pCurRawBuf[GreenIdx[0]]);
                    PixelValue &= 0xFFFFU;
                    PixelValue &= (SvcRawCap_BitGet(SVC_RAW_CAP_VIN_RESOLUTION + 1U) - 1U);
                    pWorkBuf[GreenIdx[1]] = (UINT16)PixelValue;

                    PixelValue = (UINT32)(pCurRawBuf[GreenIdx[2]]);
                    PixelValue &= 0xFFFFU;
                    PixelValue &= (SvcRawCap_BitGet(SVC_RAW_CAP_VIN_RESOLUTION + 1U) - 1U);
                    pWorkBuf[GreenIdx[3]] = (UINT16)PixelValue;
                }
            }

            /* Interpolation */
            /*    | O |    */
            /* ----------- */
            /*  O | ? | O  */
            /* ----------- */
            /*    | O |    */
            for (Row = 0U; Row < Height; Row ++) {
                for (Col = 0U; Col < Width; Col ++) {
                    if (pWorkBuf[(Row * Width) + Col] == 0U) {
                        SvcRawCap_DumpBmpCrossInt(pWorkBuf, Width, Height, Row, Col);
                    }
                }
            }

            /* U16-to-U8 for bitmap Green channel */
            for (Row = 0U; Row < Height; Row ++) {
                BmpRow = (Height - Row) - 1U;
                for (Col = 0U; Col < Width; Col ++) {
                    PixelValue = (UINT32)(pWorkBuf[(Row * Width) + Col]);
                    PixelValue &= 0xFFFFU;
                    PixelValue >>= (SVC_RAW_CAP_VIN_RESOLUTION - 8U);
                    pBmpBuf[(BmpRow * Width) + Col].Pixel.Green = (UINT8)PixelValue;
                }
            }
        }
    }
}

static void SvcRawCap_DumpBmp(const void *pRawBuf, UINT32 Bayer, UINT32 Pitch, UINT32 Width, UINT32 Height, const char *pFileName)
{
    UINT32 PRetVal;
    const UINT16 *pCurRawBuf;

    AmbaMisra_TypeCast(&(pCurRawBuf), &(pRawBuf));

    if (pCurRawBuf == NULL) {
        PRN_RCP_MSG_LOG "Fail to dump bitmap fail - invalid raw buffer!" PRN_RCP_MSG_NG
    } else if (pFileName == NULL) {
        PRN_RCP_MSG_LOG "Fail to dump bitmap fail - invalid file name!" PRN_RCP_MSG_NG
    } else if (124U < SvcWrap_strlen(pFileName)) {
        PRN_RCP_MSG_LOG "Fail to dump bitmap fail - file name length is out-of range!" PRN_RCP_MSG_NG
    } else if (Bayer > AMBA_SENSOR_BAYER_PATTERN_GB) {
        PRN_RCP_MSG_LOG "Fail to dump bitmap fail - invalid bayer pattern(%d)!"
            PRN_RCP_MSG_ARG_UINT32 Bayer PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_NG
    } else if ((RawCapCtrl.BmpInfo.State & SVC_RAW_CAP_ITEM_MEM) == 0U) {
        PRN_RCP_MSG_LOG "Stop bitmap convert - configure memory first!" PRN_RCP_MSG_API
    } else {
        UINT16                       *pWorkingBuf = NULL;
        UINT32                        WorkingBufSize = ( Width * Height ) << 1U;                                 // Temp working buffer size
        SVC_RAW_CAP_BITMAP_RGB_s     *pBmpDataBuf = NULL;
        UINT32                        BmpDataBufSize = ( Width * Height ) * (UINT32)sizeof(SVC_RAW_CAP_BITMAP_RGB_s);    // Bitmap data buffer size
        SVC_RAW_CAP_BITMAP_HEADER_s   BmpHeader;
        SVC_RAW_CAP_BITMAP_INFO_s     BmpInfo;
        AMBA_FS_FILE                 *pFile = NULL;

        if ((WorkingBufSize + BmpDataBufSize) <= RawCapCtrl.BmpInfo.BufSize) {
            const UINT8 *pCurBuf;
            AmbaMisra_TypeCast(&(pWorkingBuf), &(RawCapCtrl.BmpInfo.pBuf));
            pCurBuf = &(RawCapCtrl.BmpInfo.pBuf[WorkingBufSize]);
            AmbaMisra_TypeCast(&(pBmpDataBuf), &(pCurBuf));
        }

        if ((pWorkingBuf != NULL) && (pBmpDataBuf != NULL)) {

            AmbaSvcWrap_MisraMemset(pWorkingBuf, 0, WorkingBufSize);
            SvcRawCap_DumpBmpBayer2Red(Bayer, Pitch, Width, Height, pCurRawBuf, pWorkingBuf, pBmpDataBuf);
            AmbaSvcWrap_MisraMemset(pWorkingBuf, 0, WorkingBufSize);
            SvcRawCap_DumpBmpBayer2Green(Bayer, Pitch, Width, Height, pCurRawBuf, pWorkingBuf, pBmpDataBuf);
            AmbaSvcWrap_MisraMemset(pWorkingBuf, 0, WorkingBufSize);
            SvcRawCap_DumpBmpBayer2Blue(Bayer, Pitch, Width, Height, pCurRawBuf, pWorkingBuf, pBmpDataBuf);

            AmbaSvcWrap_MisraMemset(&BmpHeader, 0, sizeof(BmpHeader));
            BmpHeader.Type   = 0x4D42U;
            BmpHeader.Offset = (UINT32)sizeof(SVC_RAW_CAP_BITMAP_HEADER_s) + (UINT32)sizeof(SVC_RAW_CAP_BITMAP_INFO_s);
            BmpHeader.Size   = ((Width * Height) * 3U) + BmpHeader.Offset;

            AmbaSvcWrap_MisraMemset(&BmpInfo, 0, sizeof(BmpInfo));
            BmpInfo.Size         = (UINT32)sizeof(SVC_RAW_CAP_BITMAP_INFO_s);
            BmpInfo.Width        = Width;
            BmpInfo.Height       = Height;
            BmpInfo.Planes       = 1U;
            BmpInfo.BitsPerPixel = 24U;
            BmpInfo.ImageSize    = (Width * Height) * 3U;

            // I/O
            if (0U != AmbaFS_FileOpen(pFileName, "wb", &pFile)) {
                PRN_RCP_MSG_LOG "Fail to dump bitmap fail - create bmp file fail" PRN_RCP_MSG_NG
            } else if (pFile == NULL) {
                PRN_RCP_MSG_LOG "Fail to dump bitmap fail - invalid file pointer" PRN_RCP_MSG_NG
            } else {
                UINT32 WriteSize, ReturnSize;

                WriteSize = (UINT32)sizeof(SVC_RAW_CAP_BITMAP_HEADER_s);
                PRetVal = AmbaFS_FileWrite(&BmpHeader, 1, WriteSize, pFile, &ReturnSize);
                if (PRetVal != 0U) {
                    PRN_RCP_MSG_LOG "Warning to dump bitmap fail - write bitmap header fail! ErrCode: 0x%X"
                        PRN_RCP_MSG_ARG_UINT32 PRetVal PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                } else {
                    if (ReturnSize != WriteSize) {
                        PRN_RCP_MSG_LOG "Warning to dump bitmap fail - Written header size (0x%x) does not equal request size (0x%x)"
                            PRN_RCP_MSG_ARG_UINT32 ReturnSize PRN_RCP_MSG_ARG_POST
                            PRN_RCP_MSG_ARG_UINT32 WriteSize  PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_API
                    }
                }

                WriteSize = (UINT32)sizeof(SVC_RAW_CAP_BITMAP_INFO_s);
                PRetVal = AmbaFS_FileWrite(&BmpInfo, 1, WriteSize, pFile, &ReturnSize);
                if (PRetVal != 0U) {
                    PRN_RCP_MSG_LOG "Warning to dump bitmap fail - write bitmap info fail! ErrCode: 0x%X"
                        PRN_RCP_MSG_ARG_UINT32 PRetVal PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                } else {
                    if (ReturnSize != WriteSize) {
                        PRN_RCP_MSG_LOG "Warning to dump bitmap fail - Written info size (0x%x) does not equal request size (0x%x)"
                            PRN_RCP_MSG_ARG_UINT32 ReturnSize PRN_RCP_MSG_ARG_POST
                            PRN_RCP_MSG_ARG_UINT32 WriteSize  PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_API
                    }
                }

                WriteSize = BmpDataBufSize;
                PRetVal = AmbaFS_FileWrite(pBmpDataBuf, 1, WriteSize, pFile, &ReturnSize);
                if (PRetVal != 0U) {
                    PRN_RCP_MSG_LOG "Warning to dump bitmap fail - write bitmap data fail! ErrCode: 0x%X"
                        PRN_RCP_MSG_ARG_UINT32 PRetVal PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                } else {
                    if (ReturnSize != WriteSize) {
                        PRN_RCP_MSG_LOG "Warning to dump bitmap fail - Written data size (0x%x) does not equal request size (0x%x)"
                            PRN_RCP_MSG_ARG_UINT32 ReturnSize PRN_RCP_MSG_ARG_POST
                            PRN_RCP_MSG_ARG_UINT32 WriteSize  PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_API
                    }
                }

                PRetVal = AmbaFS_FileClose(pFile); PRN_RCP_ERR_HDLR

                PRN_RCP_MSG_LOG "Successful to convert raw data to bitmap %s"
                    PRN_RCP_MSG_ARG_CSTR   pFileName PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_OK
            }
        } else {
            PRN_RCP_MSG_LOG "There is not enough memory for bitmap dump" PRN_RCP_MSG_API
            PRN_RCP_MSG_LOG "  Bitmap working buffer size 0x%X"
                PRN_RCP_MSG_ARG_UINT32 WorkingBufSize PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_API
            PRN_RCP_MSG_LOG "     Bitmap data buffer size 0x%X"
                PRN_RCP_MSG_ARG_UINT32 BmpDataBufSize PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_API
        }
    }
}

static UINT16 SvcRawCap_GetTopSignBit(UINT16 x)
{
    UINT16 RetVal = 0U;

    if (x > 0U) {
        UINT32 Idx;

        RetVal = 255U;

        for (Idx = 16U; Idx > 0U; Idx --) {
            if ((x & (UINT16)(SvcRawCap_BitGet(Idx - 1U))) > 0U) {
                RetVal = (UINT16)Idx;
                break;
            }
        }
    } else {
        RetVal = 0U;
    }

    return RetVal;
}

static UINT32 SvcRawCap_GetFirstPopIdx(UINT16 x)
{
    UINT32 RetVal = 0U;

    if (x > 0U) {
        UINT32 Idx;

        RetVal = 255U;

        for (Idx = 0U; Idx < 16U; Idx ++) {
            if ((x & SvcRawCap_BitGet(Idx)) > 0U) {
                RetVal = Idx;
                break;
            }
        }
    } else {
        RetVal = 0U;
    }

    if (RetVal > 15U) {
        RetVal = 15U;
    }

    return RetVal;
}

static UINT32 SvcRawCap_CalcUnpackMask(UINT32 PackDataBits, UINT16 DataMask, UINT16 *pDataMask, UINT16 *pPreDataMask)
{
    UINT32 RetVal = SVC_OK;

    if (pDataMask == NULL) {
        RetVal = SVC_NG;
    } else if (pPreDataMask == NULL) {
        RetVal = SVC_NG;
    } else if (DataMask > 0U) {
        UINT32 DataMaskTSB = SvcRawCap_GetTopSignBit(DataMask);
        if (DataMaskTSB <= 16U) {
            UINT32 DataMaskRemainBits = 16U - DataMaskTSB;
            UINT32 CalcDataMask = 0U;
            UINT32 CalcPreDataMask = 0U;

            if (DataMaskRemainBits > PackDataBits) {
                CalcPreDataMask = 0U;
                CalcDataMask    = ( SvcRawCap_BitGet( DataMaskTSB + PackDataBits ) - 1U ) ^ DataMask;
            } else if (DataMaskRemainBits == PackDataBits) {
                CalcPreDataMask = 0U;
                CalcDataMask    = ~( (UINT32)( 0xFFFFUL >> DataMaskRemainBits) );
            } else {
                CalcPreDataMask = ~( (UINT32)( 0xFFFFUL >> DataMaskRemainBits) );
                CalcDataMask    = SvcRawCap_BitGet( PackDataBits - DataMaskRemainBits ) - 1U;
            }

            *pPreDataMask = (UINT16) CalcPreDataMask;
            *pDataMask    = (UINT16) CalcDataMask;
        }
    } else {
        RetVal = SVC_OK;
    }

    return RetVal;
}

static void SvcRawCap_UnPackLineBuf(UINT32 DataBits, UINT32 Pitch, UINT32 Width, UINT16 *pComPackBuf, UINT16 *pUnPackBuf)
{
    UINT32 PRetVal;

    if ((DataBits != 10U)
     && (DataBits != 12U)
     && (DataBits != 14U)) {
        PRN_RCP_MSG_LOG "Fail to unpack line buffer - only support 10/12/14 data bits!" PRN_RCP_MSG_NG
    } else if (pComPackBuf == NULL) {
        PRN_RCP_MSG_LOG "Fail to unpack line buffer - input compact buffer should not null!" PRN_RCP_MSG_NG
    } else if (pUnPackBuf == NULL) {
        PRN_RCP_MSG_LOG "Fail to unpack line buffer - output un-pack buffer should not null!" PRN_RCP_MSG_NG
    } else {
        UINT16 CurDataMask = (UINT16)(SvcRawCap_BitGet(DataBits) - 1U);
        UINT16 PreDataMask = 0U;
        UINT32 UnPackIdx   = 0U;
        UINT32 ComPackIdx  = 0U;
        UINT32 PostRShift  = 0U;

        AmbaMisra_TouchUnused(pComPackBuf);
        AmbaMisra_TouchUnused(pUnPackBuf);

        while ((UnPackIdx < Width) && (ComPackIdx < (Pitch >> 1U))) {

            PostRShift = SvcRawCap_GetFirstPopIdx(CurDataMask);

            pUnPackBuf[UnPackIdx] = (pComPackBuf[ComPackIdx] & CurDataMask) >> PostRShift;

            if ((PreDataMask > 0U) && (ComPackIdx > 0U)) {

                PostRShift = SvcRawCap_GetFirstPopIdx(PreDataMask);

                pUnPackBuf[UnPackIdx] = (UINT16)(((UINT32)pUnPackBuf[UnPackIdx]) << (16U - PostRShift));

                pUnPackBuf[UnPackIdx] |= (pComPackBuf[ComPackIdx - 1U] & PreDataMask) >> PostRShift;
            }

            pUnPackBuf[UnPackIdx] <<= (16U - DataBits);

            UnPackIdx ++;

            PreDataMask = 0U;
            PRetVal = SvcRawCap_CalcUnpackMask(DataBits, CurDataMask, &CurDataMask, &PreDataMask); PRN_RCP_ERR_HDLR

            if (PreDataMask > 0U) {
                ComPackIdx ++;
            } else if (SvcRawCap_GetTopSignBit(CurDataMask) == DataBits) {
                ComPackIdx ++;
            } else {
                // Do nothing
            }

            AmbaMisra_TouchUnused(&ComPackIdx);
        }
    }
}

static UINT32 SvcRawCap_UnPackRawData(UINT32 DataBits, UINT32 Pitch, UINT32 Width, UINT32 Height, void *pComPackBuf, void *pUnPackBuf)
{
    UINT32 RetVal = SVC_OK;
    UINT16 *pSrcComPackBuf, *pDstUnPackBuf;

    AmbaMisra_TypeCast(&(pSrcComPackBuf), &(pComPackBuf)); AmbaMisra_TouchUnused(pComPackBuf);
    AmbaMisra_TypeCast(&(pDstUnPackBuf), &(pUnPackBuf));   AmbaMisra_TouchUnused(pUnPackBuf);

    if (pSrcComPackBuf == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to unpack raw data - input compact raw buffer should not null!" PRN_RCP_MSG_NG
    } else if (pDstUnPackBuf == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to unpack raw data - output un-pack raw buffer should not null!" PRN_RCP_MSG_NG
    } else {
        UINT32 RowIdx, ComPackOfs, UnPackOfs;

        for (RowIdx = 0U; RowIdx < Height; RowIdx ++) {
            ComPackOfs = ( Pitch >> 1U ) * RowIdx;
            UnPackOfs  = Width * RowIdx;
            SvcRawCap_UnPackLineBuf(DataBits, Pitch, Width,
                                    &(pSrcComPackBuf[ComPackOfs]),
                                    &(pDstUnPackBuf[UnPackOfs]));
        }
    }

    return RetVal;
}

static UINT32 SvcRawCap_TaskCreate(UINT32 TaskPriority, UINT32 TaskCpuBits)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    static const char TaskName[16] = "SvcRawCap_Task";

    AmbaSvcWrap_MisraMemset(&(RawCapCtrl.TaskCtrl), 0, sizeof(RawCapCtrl.TaskCtrl));
    AmbaSvcWrap_MisraMemset(RawCapCtrl.TaskStack, 0, sizeof(RawCapCtrl.TaskStack));

    RawCapCtrl.TaskCtrl.Priority   = TaskPriority;
    RawCapCtrl.TaskCtrl.CpuBits    = TaskCpuBits;
    RawCapCtrl.TaskCtrl.pStackBase = RawCapCtrl.TaskStack;
    RawCapCtrl.TaskCtrl.StackSize  = (UINT32)sizeof(RawCapCtrl.TaskStack);
    RawCapCtrl.TaskCtrl.EntryArg   = 0U;
    RawCapCtrl.TaskCtrl.EntryFunc  = SvcRawCap_TaskEntry;

    PRetVal = SvcTask_Create(TaskName, &(RawCapCtrl.TaskCtrl));
    if (PRetVal != 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to create task! RetVal(0x%x)"
            PRN_RCP_MSG_ARG_UINT32 PRetVal PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_NG
    } else {
        RawCapCtrlFlag |= SVC_RAW_CAP_FLAG_TASK_SETUP;
        PRN_RCP_MSG_LOG "Successful to create task!" PRN_RCP_MSG_DBG
    }

    return RetVal;
}

static UINT32 SvcRawCap_TaskDelete(void)
{
    UINT32 RetVal = SVC_OK;

    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_TASK_SETUP) == 0U) {
        PRN_RCP_MSG_LOG "raw cap task is not created" PRN_RCP_MSG_DBG
    } else {
        RetVal = SvcTask_Destroy(&(RawCapCtrl.TaskCtrl));
        if (RetVal != SVC_OK) {
            PRN_RCP_MSG_LOG "Fail to delete raw cap task!" PRN_RCP_MSG_NG
        } else {
            RawCapCtrl.TaskCtrl.EntryArg = 0U;
            RawCapCtrl.TaskCtrl.EntryFunc = NULL;
            RawCapCtrlFlag &= ~(SVC_RAW_CAP_FLAG_TASK_SETUP);
            PRN_RCP_MSG_LOG "Successful to delete task!" PRN_RCP_MSG_DBG
        }
    }

    return RetVal;
}

static void* SvcRawCap_TaskEntry(void* EntryArg)
{
    UINT32 VinID, PRetVal, ActFlags = 0U;
    AMBA_DSP_RAW_DATA_RDY_s RawData;
    SVC_RAW_CAP_MSG_QUE_UNIT_s MsgUnit;

    PRetVal = AmbaKAL_EventFlagGet(&(RawCapCtrl.Flag), SVC_RAW_CAP_TSK_ON,
                                   AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_NONE, &ActFlags,
                                   AMBA_KAL_WAIT_FOREVER); PRN_RCP_ERR_HDLR

    while (0U == AmbaKAL_MsgQueueReceive(&(RawCapCtrl.MsgQue), &MsgUnit, AMBA_KAL_WAIT_FOREVER)) {
        if (MsgUnit.pData != NULL) {
            AmbaSvcWrap_MisraMemset(&RawData, 0, sizeof(AMBA_DSP_RAW_DATA_RDY_s));
            AmbaSvcWrap_MisraMemcpy(&RawData, MsgUnit.pData, sizeof(AMBA_DSP_RAW_DATA_RDY_s));
            PRetVal = AmbaKAL_MsgQueueSend(&(RawCapCtrl.FreeQue), &MsgUnit, AMBA_KAL_NO_WAIT); PRN_RCP_ERR_HDLR

            VinID = (UINT32)(RawData.VinId); VinID &= 0x0000FFFFU;

            if (VinID < AMBA_DSP_MAX_VIN_NUM) {

                if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_QUERY) > 0U) {

                    if (SVC_OK != SvcRawCap_RawInfoQuery(VinID, &RawData)) {

                        if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_MSG_DBG) > 0U) {
                            PRN_RCP_MSG_LOG "Fail to handler video raw data rdy - process raw info fail! VinID(%d)"
                                PRN_RCP_MSG_ARG_UINT32 VinID PRN_RCP_MSG_ARG_POST
                            PRN_RCP_MSG_NG
                        }

                    }

                } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CAP) > 0U) {
                    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CAP_INT_BUF) > 0U) {
                        PRetVal = SvcRawCap_RawDataCap_CapIntBuf(VinID, &RawData);
                    } else {
                        PRetVal = SvcRawCap_RawDataCap(VinID, &RawData);
                    }
                    if (SVC_OK != PRetVal) {

                        if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_MSG_DBG) > 0U) {
                            PRN_RCP_MSG_LOG "Fail to handler video raw data rdy - process raw data fail! VinID(%d)"
                                PRN_RCP_MSG_ARG_UINT32 VinID PRN_RCP_MSG_ARG_POST
                            PRN_RCP_MSG_NG
                        }

                    }

                } else {
                    /* Do nothing */
                }

            }
        }
    }

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TouchUnused(&ActFlags);

    return NULL;
}

static void SvcRawCap_DbgInfo(void)
{
    UINT32 PRetVal;

    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_INIT) == 0U) {
        PRN_RCP_MSG_LOG "Fail to show cap module info - create capture module first!" PRN_RCP_MSG_API
    } else {
        AMBA_KAL_EVENT_FLAG_INFO_s EventInfo;
        AmbaSvcWrap_MisraMemset(&EventInfo, 0, sizeof(EventInfo));
        PRetVal = AmbaKAL_EventFlagQuery(&(RawCapCtrl.Flag), &EventInfo); PRN_RCP_ERR_HDLR

        PRN_RCP_MSG_LOG " " PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "====== %sRaw Capture Info%s ======"
            PRN_RCP_MSG_ARG_CSTR   SVC_LOG_RCP_HL_TITLE_1 PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_ARG_CSTR   SVC_LOG_RCP_HL_END     PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "  Current Control Flag : 0x%x"
            PRN_RCP_MSG_ARG_UINT32 RawCapCtrlFlag PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "          Capture Flag : 0x%x"
            PRN_RCP_MSG_ARG_UINT32 EventInfo.CurrentFlags PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "         Task Priority : %d"
            PRN_RCP_MSG_ARG_UINT32 RawCapCtrl.TaskCtrl.Priority PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "          Task CpuBits : %d"
            PRN_RCP_MSG_ARG_UINT32 RawCapCtrl.TaskCtrl.CpuBits PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "        Task StackBase : %p"
            PRN_RCP_MSG_ARG_CPOINT RawCapCtrl.TaskCtrl.pStackBase PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_API
        PRN_RCP_MSG_LOG "        Task StackSize : 0x%x"
            PRN_RCP_MSG_ARG_UINT32 RawCapCtrl.TaskCtrl.StackSize PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_API
        if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG) > 0U) {

            PRN_RCP_MSG_LOG "  ---- Cap Configuration ----" PRN_RCP_MSG_API
            PRN_RCP_MSG_LOG "         VinSelectBits : 0x%x"
                PRN_RCP_MSG_ARG_UINT32 RawCapCtrl.VinSelectBits PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_API
            PRN_RCP_MSG_LOG "         ItmSelectBits : 0x%x"
                PRN_RCP_MSG_ARG_UINT32 RawCapCtrl.ItmSelectBits PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_API
        }

        if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG_MEM) > 0U) {
            UINT32 SlotIdx;
            const SVC_RAW_CAP_MEM_SLOT_s *pMemSlot = RawCapCtrl.MemCtrl.MemSlot;

            PRN_RCP_MSG_LOG "  ---- Cap Memory Info ----" PRN_RCP_MSG_API
#if !defined(CONFIG_ARM64)
            PRN_RCP_MSG_LOG "  SlotID |    MemBase |    MemSize | Aligned Base | Aligned Size |  Free Addr |  Free Size | Usage Status" PRN_RCP_MSG_API

            for (SlotIdx = 0U; SlotIdx < RawCapCtrl.MemCtrl.NumSlot; SlotIdx ++) {

                PRN_RCP_MSG_LOG "      %02d | %p | 0x%08x |   %p |   0x%08x | %p | 0x%08x | %03d / %03d / %03d / %03d / %03d"
                    PRN_RCP_MSG_ARG_UINT32 pMemSlot[SlotIdx].SlotID                                   PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_CPOINT pMemSlot[SlotIdx].pMemBuf                                  PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 pMemSlot[SlotIdx].MemSize                                  PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_CPOINT pMemSlot[SlotIdx].pMemAlgnedBuf                            PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 pMemSlot[SlotIdx].MemAlgnedSize                            PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_CPOINT pMemSlot[SlotIdx].pUsedBuf                                 PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 pMemSlot[SlotIdx].UsedSize                                 PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 pMemSlot[SlotIdx].UsedStatus[SVC_RAW_CAP_MEM_USAGE_RAW]    PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 pMemSlot[SlotIdx].UsedStatus[SVC_RAW_CAP_MEM_USAGE_HDS]    PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 pMemSlot[SlotIdx].UsedStatus[SVC_RAW_CAP_MEM_USAGE_3A]     PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 pMemSlot[SlotIdx].UsedStatus[SVC_RAW_CAP_MEM_USAGE_BMP]    PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 pMemSlot[SlotIdx].UsedStatus[SVC_RAW_CAP_MEM_USAGE_UNPACK] PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_API
            }
#else
            PRN_RCP_MSG_LOG "  SlotID |            MemBase |    MemSize |       Aligned Base | Aligned Size |          Free Addr |  Free Size" PRN_RCP_MSG_API

            for (SlotIdx = 0U; SlotIdx < RawCapCtrl.MemCtrl.NumSlot; SlotIdx ++) {

                PRN_RCP_MSG_LOG "      %02d | %p | 0x%08x | %p |   0x%08x | %p | 0x%08x"
                    PRN_RCP_MSG_ARG_UINT32 pMemSlot[SlotIdx].SlotID                                   PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_CPOINT pMemSlot[SlotIdx].pMemBuf                                  PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 pMemSlot[SlotIdx].MemSize                                  PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_CPOINT pMemSlot[SlotIdx].pMemAlgnedBuf                            PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 pMemSlot[SlotIdx].MemAlgnedSize                            PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_CPOINT pMemSlot[SlotIdx].pUsedBuf                                 PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 pMemSlot[SlotIdx].UsedSize                                 PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_API
            }

            PRN_RCP_MSG_LOG "  SlotID | Raw | RawTbl | Hds | HdsTbl | AAA | BMP | UNPACK" PRN_RCP_MSG_API
            for (SlotIdx = 0U; SlotIdx < RawCapCtrl.MemCtrl.NumSlot; SlotIdx ++) {
                PRN_RCP_MSG_LOG "      %02d | %03d |   %03d  | %03d |   %03d  | %03d | %03d | %03d "
                    PRN_RCP_MSG_ARG_UINT32 pMemSlot[SlotIdx].SlotID                                    PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 pMemSlot[SlotIdx].UsedStatus[SVC_RAW_CAP_MEM_USAGE_RAW]     PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 pMemSlot[SlotIdx].UsedStatus[SVC_RAW_CAP_MEM_USAGE_RAW_TBL] PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 pMemSlot[SlotIdx].UsedStatus[SVC_RAW_CAP_MEM_USAGE_HDS]     PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 pMemSlot[SlotIdx].UsedStatus[SVC_RAW_CAP_MEM_USAGE_HDS_TBL] PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 pMemSlot[SlotIdx].UsedStatus[SVC_RAW_CAP_MEM_USAGE_3A]      PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 pMemSlot[SlotIdx].UsedStatus[SVC_RAW_CAP_MEM_USAGE_BMP]     PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 pMemSlot[SlotIdx].UsedStatus[SVC_RAW_CAP_MEM_USAGE_UNPACK]  PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_API
            }
#endif
        }

        if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_TUNER) > 0U) {
            PRN_RCP_MSG_LOG "  ---- Cap Ituner Info ----" PRN_RCP_MSG_API
            PRN_RCP_MSG_LOG "                 State : 0x%x"
                PRN_RCP_MSG_ARG_UINT32 RawCapCtrl.ItnInfo.State PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_API
            PRN_RCP_MSG_LOG "          IkSelectBits : 0x%x"
                PRN_RCP_MSG_ARG_UINT32 RawCapCtrl.ItnInfo.IkSelectBits PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_API
            PRN_RCP_MSG_LOG "              FileName : %s"
                PRN_RCP_MSG_ARG_CSTR   RawCapCtrl.ItnInfo.FileName PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_API
            PRN_RCP_MSG_LOG "               BufBase : %p"
                PRN_RCP_MSG_ARG_CPOINT RawCapCtrl.ItnInfo.pBuf PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_API
            PRN_RCP_MSG_LOG "               BufSize : 0x%x"
                PRN_RCP_MSG_ARG_UINT32 RawCapCtrl.ItnInfo.BufSize PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_API
        }

        if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_NON_BMP) == 0U) {
            PRN_RCP_MSG_LOG "  ---- Cap Bitmap Converter ----" PRN_RCP_MSG_API
            PRN_RCP_MSG_LOG "                 State : 0x%x"
                PRN_RCP_MSG_ARG_UINT32 RawCapCtrl.BmpInfo.State PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_API
            PRN_RCP_MSG_LOG "               BufBase : %p"
                PRN_RCP_MSG_ARG_CPOINT   RawCapCtrl.BmpInfo.pBuf PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_API
            PRN_RCP_MSG_LOG "               BufSize : 0x%x"
                PRN_RCP_MSG_ARG_UINT32 RawCapCtrl.BmpInfo.BufSize PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_API
        }

        if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_NON_UNPACK) == 0U) {
            PRN_RCP_MSG_LOG "  ---- Cap Raw UnPack Process ----" PRN_RCP_MSG_API
            PRN_RCP_MSG_LOG "                 State : 0x%x"
                PRN_RCP_MSG_ARG_UINT32 RawCapCtrl.UnPackInfo.State PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_API
            PRN_RCP_MSG_LOG "               BufBase : %p"
                PRN_RCP_MSG_ARG_CPOINT   RawCapCtrl.UnPackInfo.pBuf PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_API
            PRN_RCP_MSG_LOG "               BufSize : 0x%x"
                PRN_RCP_MSG_ARG_UINT32 RawCapCtrl.UnPackInfo.BufSize PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_API
        }

        if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_MEM_SETUP) > 0U) {
            UINT32 VinIdx, SeqBufIdx, CapIdx;

            const SVC_RAW_CAP_ITEM_RAW_s *pRawItmCtrl;
            const SVC_RAW_CAP_ITEM_HDS_s *pHdsItmCtrl;
            const SVC_RAW_CAP_ITEM_AAA_s *p3AItmCtrl ;

            PRN_RCP_MSG_LOG "  ---- Cap Setup Info ----" PRN_RCP_MSG_API

            for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
                if ((RawCapCtrl.VinSelectBits & SvcRawCap_BitGet(VinIdx)) > 0U) {

                    pRawItmCtrl = &(RawCapCtrl.ItemCtrlRaw[VinIdx]);
                    pHdsItmCtrl = &(RawCapCtrl.ItemCtrlHds[VinIdx]);
                    p3AItmCtrl  = &(RawCapCtrl.ItemCtrlAaa[VinIdx]);

                    PRN_RCP_MSG_LOG "    VinID : %d"
                        PRN_RCP_MSG_ARG_UINT32 VinIdx PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                    PRN_RCP_MSG_LOG "    ---- Raw Item Control ----" PRN_RCP_MSG_API
                    PRN_RCP_MSG_LOG "            State : 0x%x"
                        PRN_RCP_MSG_ARG_UINT32 pRawItmCtrl->State PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                    PRN_RCP_MSG_LOG "         FileName : %s"
                        PRN_RCP_MSG_ARG_CSTR   pRawItmCtrl->FileName PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                    PRN_RCP_MSG_LOG "         Vin Type : %d"
                        PRN_RCP_MSG_ARG_UINT32 pRawItmCtrl->RawInfo.VinType PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                    PRN_RCP_MSG_LOG "            Pitch : %d"
                        PRN_RCP_MSG_ARG_UINT32 pRawItmCtrl->RawInfo.Pitch PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                    PRN_RCP_MSG_LOG "            Width : %d"
                        PRN_RCP_MSG_ARG_UINT32 pRawItmCtrl->RawInfo.Width PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                    PRN_RCP_MSG_LOG "           Height : %d"
                        PRN_RCP_MSG_ARG_UINT32 pRawItmCtrl->RawInfo.Height PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                    PRN_RCP_MSG_LOG "        CurSeqIdx : %d"
                        PRN_RCP_MSG_ARG_UINT32 pRawItmCtrl->CurSeqIdx PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                    PRN_RCP_MSG_LOG "      NumCaptured : %d"
                        PRN_RCP_MSG_ARG_UINT32 pRawItmCtrl->NumCaptured PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                    PRN_RCP_MSG_LOG "           NumSeq : %d"
                        PRN_RCP_MSG_ARG_UINT32 pRawItmCtrl->NumSeq PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                    PRN_RCP_MSG_LOG "      SeqBuf Addr : %p"
                        PRN_RCP_MSG_ARG_CPOINT pRawItmCtrl->pBufSeqTbl PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                    if (pRawItmCtrl->pBufSeqTbl != NULL) {
                        for (SeqBufIdx = 0U; SeqBufIdx < pRawItmCtrl->NumSeq; SeqBufIdx ++) {
                            PRN_RCP_MSG_LOG "      SeqBuf[%3d] : Addr(%p), SeqNum(%d)"
                                PRN_RCP_MSG_ARG_UINT32 SeqBufIdx                          PRN_RCP_MSG_ARG_POST
                                PRN_RCP_MSG_ARG_CPOINT pRawItmCtrl->pBufSeqTbl[SeqBufIdx] PRN_RCP_MSG_ARG_POST
                                PRN_RCP_MSG_ARG_UINT32 pRawItmCtrl->RawSeqArr[SeqBufIdx]  PRN_RCP_MSG_ARG_POST
                            PRN_RCP_MSG_API
                        }
                    }

                    if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_HDS) > 0U) {

                        PRN_RCP_MSG_LOG "    ---- Hds Item Control ----" PRN_RCP_MSG_API
                        PRN_RCP_MSG_LOG "            State : 0x%x"
                            PRN_RCP_MSG_ARG_UINT32 pHdsItmCtrl->State PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_API
                        PRN_RCP_MSG_LOG "         FileName : %s"
                            PRN_RCP_MSG_ARG_CSTR   pHdsItmCtrl->FileName PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_API
                        PRN_RCP_MSG_LOG "            Pitch : %d"
                            PRN_RCP_MSG_ARG_UINT32 pHdsItmCtrl->HdsInfo.Pitch PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_API
                        PRN_RCP_MSG_LOG "            Width : %d"
                            PRN_RCP_MSG_ARG_UINT32 pHdsItmCtrl->HdsInfo.Width PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_API
                        PRN_RCP_MSG_LOG "           Height : %d"
                            PRN_RCP_MSG_ARG_UINT32 pHdsItmCtrl->HdsInfo.Height PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_API
                        PRN_RCP_MSG_LOG "        CurSeqIdx : %d"
                            PRN_RCP_MSG_ARG_UINT32 pHdsItmCtrl->CurSeqIdx PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_API
                        PRN_RCP_MSG_LOG "      NumCaptured : %d"
                            PRN_RCP_MSG_ARG_UINT32 pHdsItmCtrl->NumCaptured PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_API
                        PRN_RCP_MSG_LOG "           NumSeq : %d"
                            PRN_RCP_MSG_ARG_UINT32 pHdsItmCtrl->NumSeq PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_API
                        PRN_RCP_MSG_LOG "      SeqBuf Addr : %p"
                            PRN_RCP_MSG_ARG_CPOINT pHdsItmCtrl->pBufSeqTbl PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_API
                        if (pHdsItmCtrl->pBufSeqTbl != NULL) {
                            for (SeqBufIdx = 0U; SeqBufIdx < pHdsItmCtrl->NumSeq; SeqBufIdx ++) {
                                PRN_RCP_MSG_LOG "      SeqBuf[%3d] : %p"
                                    PRN_RCP_MSG_ARG_UINT32 SeqBufIdx                          PRN_RCP_MSG_ARG_POST
                                    PRN_RCP_MSG_ARG_CPOINT pHdsItmCtrl->pBufSeqTbl[SeqBufIdx] PRN_RCP_MSG_ARG_POST
                                PRN_RCP_MSG_API
                            }
                        }
                    }

                    if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_3A) > 0U) {
                        UINT32 ImgChIdx;

                        PRN_RCP_MSG_LOG "    ---- Aaa Item Control ----" PRN_RCP_MSG_API
                        PRN_RCP_MSG_LOG "            State : 0x%x"
                            PRN_RCP_MSG_ARG_UINT32 p3AItmCtrl->State PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_API
                        PRN_RCP_MSG_LOG "         FileName : %s"
                            PRN_RCP_MSG_ARG_CSTR   p3AItmCtrl->FileName PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_API
                        PRN_RCP_MSG_LOG "          BufSize : 0x%x"
                            PRN_RCP_MSG_ARG_UINT32 p3AItmCtrl->AaaInfo.BufSize PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_API
                        PRN_RCP_MSG_LOG "    AaaSelectBits : 0x%x"
                            PRN_RCP_MSG_ARG_UINT32 p3AItmCtrl->AaaInfo.ImgChanSelectBits PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_API
                        PRN_RCP_MSG_LOG "         Buf Info : ImgAlgoID | Buf Offset | Buf Size" PRN_RCP_MSG_API

                        for (ImgChIdx = 0U; ImgChIdx < 32U; ImgChIdx ++) {
                            if ((p3AItmCtrl->AaaInfo.ImgChanSelectBits & SvcRawCap_BitGet(ImgChIdx)) > 0U) {
                                PRN_RCP_MSG_LOG "                           %02d | 0x%08x | 0x%08x"
                                    PRN_RCP_MSG_ARG_UINT32 ImgChIdx                                                  PRN_RCP_MSG_ARG_POST
                                    PRN_RCP_MSG_ARG_UINT32 p3AItmCtrl->AaaInfo.ImgAaaBufOfs[ImgChIdx]  PRN_RCP_MSG_ARG_POST
                                    PRN_RCP_MSG_ARG_UINT32 p3AItmCtrl->AaaInfo.ImgAaaBufSize[ImgChIdx] PRN_RCP_MSG_ARG_POST
                                PRN_RCP_MSG_API
                            }
                        }

                        PRN_RCP_MSG_LOG "        CurSeqIdx : %d"
                            PRN_RCP_MSG_ARG_UINT32 p3AItmCtrl->CurSeqIdx PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_API
                        PRN_RCP_MSG_LOG "      NumCaptured : %d"
                            PRN_RCP_MSG_ARG_UINT32 p3AItmCtrl->NumCaptured PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_API
                        PRN_RCP_MSG_LOG "           NumSeq : %d"
                            PRN_RCP_MSG_ARG_UINT32 p3AItmCtrl->NumSeq PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_API
                        PRN_RCP_MSG_LOG "      SeqBuf Addr : %p"
                            PRN_RCP_MSG_ARG_CPOINT p3AItmCtrl->pBufSeq PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_API
                        for (SeqBufIdx = 0U; SeqBufIdx < p3AItmCtrl->NumSeq; SeqBufIdx ++) {
                            PRN_RCP_MSG_LOG "      SeqBuf[%3d] : %p"
                                PRN_RCP_MSG_ARG_UINT32 SeqBufIdx                                   PRN_RCP_MSG_ARG_POST
                                PRN_RCP_MSG_ARG_CPOINT p3AItmCtrl->pBufSeq[SeqBufIdx] PRN_RCP_MSG_ARG_POST
                            PRN_RCP_MSG_API
                        }
                    }

                }
            }

            {
                const UINT16                  *pCapID  = RawCapCtrl.CmdInfo.CapID;
                const AMBA_DSP_DATACAP_CFG_s  *pCapCfg = RawCapCtrl.CmdInfo.CapCfg;
                const AMBA_DSP_DATACAP_CTRL_s *pCaCtrl = RawCapCtrl.CmdInfo.CapCtrl;
                const void *pBufBase;

                PRN_RCP_MSG_LOG "  ---- Cap Cmd Info ----" PRN_RCP_MSG_API

                for (CapIdx = 0U; CapIdx < RawCapCtrl.CmdInfo.NumCapInstance; CapIdx ++) {
                    PRN_RCP_MSG_LOG "      Cap Instance ID : %d"
                        PRN_RCP_MSG_ARG_UINT32 pCapID[CapIdx] PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                    PRN_RCP_MSG_LOG "          CapDataType : %d"
                        PRN_RCP_MSG_ARG_UINT32 pCapCfg[CapIdx].CapDataType PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                    PRN_RCP_MSG_LOG "             CmprRate : %d"
                        PRN_RCP_MSG_ARG_UINT32 pCapCfg[CapIdx].CmprRate PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                    PRN_RCP_MSG_LOG "             CmptRate : %d"
                        PRN_RCP_MSG_ARG_UINT32 pCapCfg[CapIdx].CmptRate PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                    PRN_RCP_MSG_LOG "        AuxDataNeeded : %d"
                        PRN_RCP_MSG_ARG_UINT32 pCapCfg[CapIdx].AuxDataNeeded PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                    PRN_RCP_MSG_LOG "                Index : %d"
                        PRN_RCP_MSG_ARG_UINT32 pCapCfg[CapIdx].Index PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                    PRN_RCP_MSG_LOG "            AllocType : %d"
                        PRN_RCP_MSG_ARG_UINT32 pCapCfg[CapIdx].AllocType PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                    PRN_RCP_MSG_LOG "               BufNum : %d"
                        PRN_RCP_MSG_ARG_UINT32 pCapCfg[CapIdx].BufNum PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                    PRN_RCP_MSG_LOG "         OverFlowCtrl : %d"
                        PRN_RCP_MSG_ARG_UINT32 pCapCfg[CapIdx].OverFlowCtrl PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                    PRN_RCP_MSG_LOG "        DataBuf.Pitch : %d"
                        PRN_RCP_MSG_ARG_UINT32 pCapCfg[CapIdx].DataBuf.Pitch PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                    PRN_RCP_MSG_LOG "        DataBuf.Width : %d"
                        PRN_RCP_MSG_ARG_UINT32 pCapCfg[CapIdx].DataBuf.Window.Width PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                    PRN_RCP_MSG_LOG "       DataBuf.Height : %d"
                        PRN_RCP_MSG_ARG_UINT32 pCapCfg[CapIdx].DataBuf.Window.Height PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                    AmbaMisra_TypeCast(&(pBufBase), &(pCapCfg[CapIdx].DataBuf.BaseAddr));
                    PRN_RCP_MSG_LOG "     DataBuf.BaseAddr : %p"
                        PRN_RCP_MSG_ARG_CPOINT pBufBase PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                    PRN_RCP_MSG_LOG "     AuxDataBuf.Pitch : %d"
                        PRN_RCP_MSG_ARG_UINT32 pCapCfg[CapIdx].AuxDataBuf.Pitch PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                    PRN_RCP_MSG_LOG "    AuxDataBuf.Width  : %d"
                        PRN_RCP_MSG_ARG_UINT32 pCapCfg[CapIdx].AuxDataBuf.Window.Width PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                    PRN_RCP_MSG_LOG "    AuxDataBuf.Height : %d"
                        PRN_RCP_MSG_ARG_UINT32 pCapCfg[CapIdx].AuxDataBuf.Window.Height PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                    PRN_RCP_MSG_LOG "              pBufTbl : %p"
                        PRN_RCP_MSG_ARG_CPOINT pCapCfg[CapIdx].pBufTbl PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                    PRN_RCP_MSG_LOG "           pAuxBufTbl : %p"
                        PRN_RCP_MSG_ARG_CPOINT pCapCfg[CapIdx].pAuxBufTbl PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                    PRN_RCP_MSG_LOG "               CapNum : %d"
                        PRN_RCP_MSG_ARG_UINT32 pCaCtrl[CapIdx].CapNum PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                }
            }
        }
    }
}

/**
* create of raw capture module
* @param [in] TaskPriority task priority
* @param [in] TaskCpuBits task cpu bits
* @return 0-OK, 1-NG
*/
UINT32 SvcRawCap_Create(UINT32 TaskPriority, UINT32 TaskCpuBits)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_INIT) > 0U) {
        PRN_RCP_MSG_LOG "The raw cap module has been created" PRN_RCP_MSG_API
    } else {
        UINT32 TempBits = 0U;
        static char FlagName[16] = "SvcRawCap_Flag";
        static char CapStarName[32]  = "SvcRawCap_CapStart";
        static char CapRdyName[32]   = "SvcRawCap_CapRdy";
        static char MutexName[16]    = "SvcRawCap_Mutex";
        static char FreeQueName[32] = "SvcRawCap_FreeQue";
        static char NsgQueName[32] = "SvcRawCap_MsgQue";

        /* Reset the module setting */
        AmbaSvcWrap_MisraMemset(&RawCapCtrl, 0, sizeof(RawCapCtrl));

        /* Create event flag */
        AmbaSvcWrap_MisraMemset(&(RawCapCtrl.Flag), 0, sizeof(AMBA_KAL_EVENT_FLAG_t));
        PRetVal = AmbaKAL_EventFlagCreate(&(RawCapCtrl.Flag), FlagName);
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_RCP_MSG_LOG "Fail to create raw cap module - create event flag fail!. RetVal: 0x%x"
                PRN_RCP_MSG_ARG_UINT32 PRetVal PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_NG
        } else {
            TempBits |= 0x1U;
            PRetVal = AmbaKAL_EventFlagClear(&(RawCapCtrl.Flag), 0xFFFFFFFFU); PRN_RCP_ERR_HDLR
        }
        AmbaSvcWrap_MisraMemset(&(RawCapCtrl.CapStartFlag), 0, sizeof(AMBA_KAL_EVENT_FLAG_t));
        PRetVal = AmbaKAL_EventFlagCreate(&(RawCapCtrl.CapStartFlag), CapStarName);
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_RCP_MSG_LOG "Fail to create raw cap module - create event cap start flag fail!. RetVal: 0x%x"
                PRN_RCP_MSG_ARG_UINT32 PRetVal PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_NG
        } else {
            TempBits |= 0x20U;
            PRetVal = AmbaKAL_EventFlagClear(&(RawCapCtrl.CapStartFlag), 0xFFFFFFFFU); PRN_RCP_ERR_HDLR
        }
        AmbaSvcWrap_MisraMemset(&(RawCapCtrl.CapRdyFlag), 0, sizeof(AMBA_KAL_EVENT_FLAG_t));
        PRetVal = AmbaKAL_EventFlagCreate(&(RawCapCtrl.CapRdyFlag), CapRdyName);
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_RCP_MSG_LOG "Fail to create raw cap module - create event cap ready flag fail!. RetVal: 0x%x"
                PRN_RCP_MSG_ARG_UINT32 PRetVal PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_NG
        } else {
            TempBits |= 0x40U;
            PRetVal = AmbaKAL_EventFlagClear(&(RawCapCtrl.CapRdyFlag), 0xFFFFFFFFU); PRN_RCP_ERR_HDLR
        }

        /* Create mutex */
        if (RetVal == SVC_OK) {
            AmbaSvcWrap_MisraMemset(&(RawCapCtrl.Mutex), 0, sizeof(AMBA_KAL_MUTEX_t));
            PRetVal = AmbaKAL_MutexCreate(&(RawCapCtrl.Mutex), MutexName);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_RCP_MSG_LOG "Fail to create raw cap - create mutex fail! RetVal: 0x%x"
                    PRN_RCP_MSG_ARG_UINT32 PRetVal PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_NG
            } else {
                TempBits |= 0x2U;
            }
        }

        /* Create free queue */
        if (RetVal == SVC_OK) {
            AmbaSvcWrap_MisraMemset(RawCapCtrl.QueDataBuf, 0, sizeof(RawCapCtrl.QueDataBuf));

            AmbaSvcWrap_MisraMemset(RawCapCtrl.FreeQueBuf, 0, sizeof(RawCapCtrl.FreeQueBuf));
            AmbaSvcWrap_MisraMemset(&(RawCapCtrl.FreeQue), 0, sizeof(RawCapCtrl.FreeQue));
            PRetVal = AmbaKAL_MsgQueueCreate(&(RawCapCtrl.FreeQue),
                                             FreeQueName,
                                             (UINT32)sizeof(RawCapCtrl.FreeQueBuf[0]),
                                             RawCapCtrl.FreeQueBuf,
                                             (UINT32)sizeof(RawCapCtrl.FreeQueBuf));
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_RCP_MSG_LOG "Fail to create raw cap - create free queue fail! RetVal: 0x%x"
                    PRN_RCP_MSG_ARG_UINT32 PRetVal PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_NG
            } else if (0U != AmbaKAL_MsgQueueFlush(&(RawCapCtrl.FreeQue))) {
                RetVal = SVC_NG;
                PRN_RCP_MSG_LOG "Fail to create raw cap - flush free queue fail!" PRN_RCP_MSG_NG
            } else {
                UINT32 Idx;
                SVC_RAW_CAP_MSG_QUE_UNIT_s MsgUnit;

                TempBits |= 0x10U;

                for (Idx = 0U; Idx < SVC_RAW_CAP_MSG_QUE_DEPTH; Idx ++) {
                    AmbaSvcWrap_MisraMemset(&MsgUnit, 0, sizeof(MsgUnit));
                    MsgUnit.pData = &(RawCapCtrl.QueDataBuf[Idx]);
                    PRetVal = AmbaKAL_MsgQueueSend(&(RawCapCtrl.FreeQue), &MsgUnit, AMBA_KAL_NO_WAIT);
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;
                        PRN_RCP_MSG_LOG "Fail to create raw cap - set free queue no.%d unit! RetVal: 0x%x"
                            PRN_RCP_MSG_ARG_UINT32 Idx     PRN_RCP_MSG_ARG_POST
                            PRN_RCP_MSG_ARG_UINT32 PRetVal PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_NG
                        break;
                    }
                }
            }
        }

        /* Create msg queue */
        if (RetVal == SVC_OK) {
            AmbaSvcWrap_MisraMemset(RawCapCtrl.MsgQueBuf, 0, sizeof(RawCapCtrl.MsgQueBuf));
            AmbaSvcWrap_MisraMemset(&(RawCapCtrl.MsgQue), 0, sizeof(AMBA_KAL_MSG_QUEUE_t));
            PRetVal = AmbaKAL_MsgQueueCreate(&(RawCapCtrl.MsgQue),
                                             NsgQueName,
                                             (UINT32)sizeof(RawCapCtrl.MsgQueBuf[0]),
                                             RawCapCtrl.MsgQueBuf,
                                             (UINT32)sizeof(RawCapCtrl.MsgQueBuf));
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_RCP_MSG_LOG "Fail to create raw cap - create msg queue fail! RetVal: 0x%x"
                    PRN_RCP_MSG_ARG_UINT32 PRetVal PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_NG
            } else if (0U != AmbaKAL_MsgQueueFlush(&(RawCapCtrl.MsgQue))) {
                RetVal = SVC_NG;
                PRN_RCP_MSG_LOG "Fail to create raw cap - msg free queue fail!" PRN_RCP_MSG_NG
            } else {
                TempBits |= 0x4U;
            }
        }

        /* Register dsp event */
        if (RetVal == SVC_OK) {
            RetVal = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_RAW_RDY, SvcRawCap_RawDataRdyHdlr);
            if (RetVal != SVC_OK) {
                PRN_RCP_MSG_LOG "Fail to create raw cap module - register LV_RAW_RDY event fail!" PRN_RCP_MSG_NG
            } else {
                TempBits |= 0x8U;
            }
        }

        /* Setup task */
        if (RetVal == SVC_OK) {
            RetVal = SvcRawCap_TaskCreate(TaskPriority, TaskCpuBits);
            if (RetVal != SVC_OK) {
                PRN_RCP_MSG_LOG "Fail to capture raw sequence - setup capture task fail!" PRN_RCP_MSG_NG
            }
        }

        /* Update the state */
        if (RetVal == SVC_OK) {
            PRetVal = (RawCapCtrlFlag & (SVC_RAW_CAP_FLAG_MSG_DEF | SVC_RAW_CAP_FLAG_MSG_DBG | SVC_RAW_CAP_FLAG_SHELL_INIT | SVC_RAW_CAP_FLAG_TASK_SETUP));
            RawCapCtrlFlag = PRetVal;
            RawCapCtrlFlag |= SVC_RAW_CAP_FLAG_INIT;
            PRN_RCP_MSG_LOG "Successful to create raw cap module! %d 0x%x"
                PRN_RCP_MSG_ARG_UINT32 RawCapCtrl.TaskCtrl.Priority PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_ARG_UINT32 RawCapCtrl.TaskCtrl.CpuBits  PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_OK
        } else {
            /* Delete event flag */
            if ((TempBits & 0x1UL) > 0U) {
                PRetVal = AmbaKAL_EventFlagClear(&(RawCapCtrl.Flag), 0xFFFFFFFFU); PRN_RCP_ERR_HDLR
                PRetVal = AmbaKAL_EventFlagDelete(&(RawCapCtrl.Flag)); PRN_RCP_ERR_HDLR
            }
            if ((TempBits & 0x20UL) > 0U) {
                PRetVal = AmbaKAL_EventFlagClear(&(RawCapCtrl.CapStartFlag), 0xFFFFFFFFU); PRN_RCP_ERR_HDLR
                PRetVal = AmbaKAL_EventFlagDelete(&(RawCapCtrl.CapStartFlag)); PRN_RCP_ERR_HDLR
            }
            if ((TempBits & 0x40UL) > 0U) {
                PRetVal = AmbaKAL_EventFlagClear(&(RawCapCtrl.CapRdyFlag), 0xFFFFFFFFU); PRN_RCP_ERR_HDLR
                PRetVal = AmbaKAL_EventFlagDelete(&(RawCapCtrl.CapRdyFlag)); PRN_RCP_ERR_HDLR
            }

            /* Delete mutex */
            if ((TempBits & 0x2UL) > 0U) {
                SvcRawCap_MutexGive(&(RawCapCtrl.Mutex));
                PRetVal = AmbaKAL_MutexDelete(&(RawCapCtrl.Mutex)); PRN_RCP_ERR_HDLR
            }

            /* Delete msg queue */
            if ((TempBits & 0x4UL) > 0U) {
                PRetVal = AmbaKAL_MsgQueueDelete(&(RawCapCtrl.MsgQue)); PRN_RCP_ERR_HDLR
            }

            /* Delete msg queue */
            if ((TempBits & 0x10UL) > 0U) {
                PRetVal = AmbaKAL_MsgQueueDelete(&(RawCapCtrl.FreeQue)); PRN_RCP_ERR_HDLR
            }

            /* Unregister event */
            if ((TempBits & 0x8UL) > 0U) {
                PRetVal = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_LV_RAW_RDY, SvcRawCap_RawDataRdyHdlr); PRN_RCP_ERR_HDLR
            }

            PRN_RCP_MSG_LOG "Fail to create raw cap module!" PRN_RCP_MSG_NG
        }
    }

    return RetVal;
}

/**
* delete of raw capture module
* @return 0-OK, 1-NG
*/
UINT32 SvcRawCap_Delete(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_INIT) > 0U) {
        {
            INT32 TimeOut = 1000;
            while ((RawCapCtrlFlag & (SVC_RAW_CAP_FLAG_QUERY | SVC_RAW_CAP_FLAG_CAP)) > 0U) {
                TimeOut -= 10;
                if (TimeOut <= 0) {
                    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_MSG_DBG) > 0U) {
                        PRN_RCP_MSG_LOG "Fail to delete raw cap - wait event flag idle timeout! 0x%x"
                            PRN_RCP_MSG_ARG_UINT32 RawCapCtrlFlag PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_NG
                    }
                    break;
                }
                PRetVal = AmbaKAL_TaskSleep(10U); PRN_RCP_ERR_HDLR
            }

            /* Delete capture task */
            RetVal = SvcRawCap_TaskDelete();

            PRetVal = (RawCapCtrlFlag & (SVC_RAW_CAP_FLAG_MSG_DBG | SVC_RAW_CAP_FLAG_SHELL_INIT));
            RawCapCtrlFlag = PRetVal;

            /* Unregister event */
            PRetVal = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_LV_RAW_RDY, SvcRawCap_RawDataRdyHdlr); PRN_RCP_ERR_HDLR

            /* Delete event flag */
            PRetVal = AmbaKAL_EventFlagClear(&(RawCapCtrl.Flag), 0xFFFFFFFFU); PRN_RCP_ERR_HDLR
            PRetVal = AmbaKAL_EventFlagDelete(&(RawCapCtrl.Flag)); PRN_RCP_ERR_HDLR
            PRetVal = AmbaKAL_EventFlagClear(&(RawCapCtrl.CapStartFlag), 0xFFFFFFFFU); PRN_RCP_ERR_HDLR
            PRetVal = AmbaKAL_EventFlagDelete(&(RawCapCtrl.CapStartFlag)); PRN_RCP_ERR_HDLR
            PRetVal = AmbaKAL_EventFlagClear(&(RawCapCtrl.CapRdyFlag), 0xFFFFFFFFU); PRN_RCP_ERR_HDLR
            PRetVal = AmbaKAL_EventFlagDelete(&(RawCapCtrl.CapRdyFlag)); PRN_RCP_ERR_HDLR

            /* Delete msg queue */
            PRetVal = AmbaKAL_MsgQueueDelete(&(RawCapCtrl.MsgQue)); PRN_RCP_ERR_HDLR
            /* Delete free queue */
            PRetVal = AmbaKAL_MsgQueueDelete(&(RawCapCtrl.FreeQue)); PRN_RCP_ERR_HDLR

            /* Delete mutex */
            SvcRawCap_MutexGive(&(RawCapCtrl.Mutex));
            PRetVal = AmbaKAL_MutexDelete(&(RawCapCtrl.Mutex)); PRN_RCP_ERR_HDLR

            /* Reset the raw cap module */
            AmbaSvcWrap_MisraMemset(&RawCapCtrl, 0, sizeof(RawCapCtrl));

            PRN_RCP_MSG_LOG "Successful to delete raw cap module!" PRN_RCP_MSG_OK
        }
    }

    return RetVal;
}

/**
* capture configuration of raw capture module
* @param [in] VinSelectBits vin bits
* @param [in] CfgSelectBits selected bits
* @return 0-OK, 1-NG
*/
UINT32 SvcRawCap_CfgCap(UINT32 VinSelectBits, UINT32 CfgSelectBits)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg raw cap - create raw cap module first!" PRN_RCP_MSG_NG
    } else if (VinSelectBits == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg raw cap - input VinSelectBits should not zero!" PRN_RCP_MSG_NG
    } else if (CfgSelectBits == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg raw cap - input Cfg should not zero!" PRN_RCP_MSG_NG
    } else if (0U != SvcRawCap_MutexTake(&(RawCapCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg raw cap - take mutex fail" PRN_RCP_MSG_NG
    } else {
        UINT32 VinIdx, CurSelectBits = 0U;

        PRetVal = SvcRawCap_CapReset();
        if (PRetVal != SVC_OK) {
            PRN_RCP_MSG_LOG "Warning to reset raw capture" PRN_RCP_MSG_DBG
        }
        PRetVal = AmbaKAL_EventFlagSet(&(RawCapCtrl.Flag), SVC_RAW_CAP_TSK_ON); PRN_RCP_ERR_HDLR

        RawCapCtrl.VinSelectBits = VinSelectBits;
        RawCapCtrl.ItmSelectBits = CfgSelectBits;

        for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
            if ((RawCapCtrl.VinSelectBits & (0x1UL << VinIdx)) > 0U) {
                if (CurSelectBits == 0U) {
                    CurSelectBits |= SvcRawCap_BitGet(VinIdx);
                } else {
                    PRN_RCP_MSG_LOG "Only support one vin capture now. Refine VinSelectBits 0x%x -> 0x%x"
                        PRN_RCP_MSG_ARG_UINT32 VinSelectBits PRN_RCP_MSG_ARG_POST
                        PRN_RCP_MSG_ARG_UINT32 CurSelectBits PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_API
                    RawCapCtrl.VinSelectBits = CurSelectBits;
                    break;
                }
            }
        }

        for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
            /* Reset item control state */
            RawCapCtrl.ItemCtrlRaw[VinIdx].State = 0U;
            RawCapCtrl.ItemCtrlHds[VinIdx].State = 0U;
            RawCapCtrl.ItemCtrlAaa[VinIdx].State = 0U;

            /* Initial item control */
            if ( ( RawCapCtrl.VinSelectBits & (0x1UL << VinIdx) ) > 0U ) {
                if ( ( RawCapCtrl.ItmSelectBits & (0x1UL << SVC_RAW_CAP_ITEM_ID_RAW) ) > 0U ) {
                    RawCapCtrl.ItemCtrlRaw[VinIdx].RawInfo.VinType = SVC_RAW_CAP_VIN_TYPE_CFA;
                    RawCapCtrl.ItemCtrlRaw[VinIdx].State |= SVC_RAW_CAP_ITEM_INIT;
                }

                if ( ( RawCapCtrl.ItmSelectBits & (0x1UL << SVC_RAW_CAP_ITEM_ID_HDS) ) > 0U ) {
                    RawCapCtrl.ItemCtrlHds[VinIdx].State |= SVC_RAW_CAP_ITEM_INIT;
                }

                if ( ( RawCapCtrl.ItmSelectBits & (0x1UL << SVC_RAW_CAP_ITEM_ID_3A) ) > 0U ) {
                    RawCapCtrl.ItemCtrlAaa[VinIdx].State |= SVC_RAW_CAP_ITEM_INIT;
                }
            }
        }

        RawCapCtrl.ItnInfo.State    &= ~(SVC_RAW_CAP_ITEM_CFG | SVC_RAW_CAP_ITEM_RDY);
        RawCapCtrl.ItnInfo.State    |= SVC_RAW_CAP_ITEM_INIT;
        RawCapCtrl.BmpInfo.State    &= ~(SVC_RAW_CAP_ITEM_CFG | SVC_RAW_CAP_ITEM_RDY | SVC_RAW_CAP_ITEM_MEM);
        RawCapCtrl.BmpInfo.State    |= SVC_RAW_CAP_ITEM_INIT;
        RawCapCtrl.UnPackInfo.State &= ~(SVC_RAW_CAP_ITEM_CFG | SVC_RAW_CAP_ITEM_RDY | SVC_RAW_CAP_ITEM_MEM);
        RawCapCtrl.UnPackInfo.State |= SVC_RAW_CAP_ITEM_INIT;

        PRN_RCP_MSG_LOG " " PRN_RCP_MSG_DBG
        PRN_RCP_MSG_LOG "==== Configure Raw Cap ====" PRN_RCP_MSG_DBG
        PRN_RCP_MSG_LOG "  VinSelectBits : 0x%x"
            PRN_RCP_MSG_ARG_UINT32 RawCapCtrl.VinSelectBits PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_DBG
        PRN_RCP_MSG_LOG "  ItmSelectBits : 0x%x"
            PRN_RCP_MSG_ARG_UINT32 RawCapCtrl.ItmSelectBits PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_DBG

        if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG) > 0U) {
            RawCapCtrlFlag &= ~(SVC_RAW_CAP_FLAG_CFG_RAW   );
            RawCapCtrlFlag &= ~(SVC_RAW_CAP_FLAG_CFG_HDS   );
            RawCapCtrlFlag &= ~(SVC_RAW_CAP_FLAG_CFG_AAA   );
            RawCapCtrlFlag &= ~(SVC_RAW_CAP_FLAG_MEM_SETUP );
            RawCapCtrlFlag &= ~(SVC_RAW_CAP_FLAG_CAP_SETUP );
        } else {
            RawCapCtrlFlag |= SVC_RAW_CAP_FLAG_CFG;
        }

        PRN_RCP_MSG_LOG "Successful to cfg raw cap module! 0x%x"
            PRN_RCP_MSG_ARG_UINT32 RawCapCtrlFlag PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_API

        SvcRawCap_MutexGive(&(RawCapCtrl.Mutex));
    }

    return RetVal;
}

/**
* buffer configuration of raw capture module
* @param [in] pBuf base of buffer
* @param [in] BufSize size of buffer
* @return 0-OK, 1-NG
*/
UINT32 SvcRawCap_CfgMem(UINT8 *pBuf, UINT32 BufSize)
{
    UINT32 RetVal = SVC_OK;

    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg memory - create raw cap module first!" PRN_RCP_MSG_NG
    } else if (pBuf == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg memory - input buffer addr should not zero!" PRN_RCP_MSG_NG
    } else if (BufSize == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg memory - input buffer size should not zero!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrl.MemCtrl.NumSlot + 1U) > SVC_RAW_CAP_MEM_SLOT_MAX) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg memory - max memory slot is %d"
            PRN_RCP_MSG_ARG_UINT32 SVC_RAW_CAP_MEM_SLOT_MAX PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_NG
    } else if (0U != SvcRawCap_MutexTake(&(RawCapCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg memory - take mutex fail" PRN_RCP_MSG_NG
    } else {
        if (SVC_OK != SvcRawCap_MemCheck(pBuf, BufSize)) {
            RetVal = SVC_NG;
            PRN_RCP_MSG_LOG "Fail to cfg memory - input buffer setting cross existed memory!" PRN_RCP_MSG_NG
        } else {
            SVC_RAW_CAP_MEM_SLOT_s *pMemSlot = &(RawCapCtrl.MemCtrl.MemSlot[RawCapCtrl.MemCtrl.NumSlot]);

            AmbaSvcWrap_MisraMemset(pMemSlot, 0, sizeof(SVC_RAW_CAP_MEM_SLOT_s));
            pMemSlot->SlotID        = RawCapCtrl.MemCtrl.NumSlot + 1U;
            pMemSlot->pMemBuf       = pBuf;
            pMemSlot->MemSize       = BufSize;
            pMemSlot->pMemAlgnedBuf = SvcRawCap_MemAlign(pMemSlot->pMemBuf, 64U);
            pMemSlot->MemAlgnedSize = BufSize - SvcRawCap_MemDiff(pMemSlot->pMemAlgnedBuf, pMemSlot->pMemBuf);
            pMemSlot->pUsedBuf      = pMemSlot->pMemAlgnedBuf;
            pMemSlot->UsedSize      = pMemSlot->MemAlgnedSize;

            RawCapCtrl.MemCtrl.NumSlot += 1U;

            RawCapCtrlFlag |= SVC_RAW_CAP_FLAG_CFG_MEM;
            PRN_RCP_MSG_LOG "Successful to cfg memory! SlotID(%d)"
                PRN_RCP_MSG_ARG_UINT32 pMemSlot->SlotID PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_OK
        }

        SvcRawCap_MutexGive(&(RawCapCtrl.Mutex));
    }

    return RetVal;
}

/**
* buffer configuration of ituner
* @param [in] BufAddr base of buffer
* @param [in] BufSize size of buffer
* @return 0-OK, 1-NG
*/
UINT32 SvcRawCap_CfgItnMem(UINT8 *pBuf, UINT32 BufSize)
{
    UINT32 RetVal = SVC_OK;

    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg ituner memory - create raw cap module first!" PRN_RCP_MSG_NG
    } else if (pBuf == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg ituner memory - invalid buffer address!" PRN_RCP_MSG_NG
    } else if (BufSize == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg ituner memory - buffer size should not zero!" PRN_RCP_MSG_NG
    } else if (0U != SvcRawCap_MutexTake(&(RawCapCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg ituner memory - take mutex fail" PRN_RCP_MSG_NG
    } else {
        SVC_RAW_CAP_ITN_INFO_s *pItnInfo = &(RawCapCtrl.ItnInfo);
        UINT32 ChkFlag = SVC_RAW_CAP_ITEM_INIT | SVC_RAW_CAP_ITEM_CFG | SVC_RAW_CAP_ITEM_MEM;

        pItnInfo->pBuf    = pBuf;
        pItnInfo->BufSize = BufSize;
        pItnInfo->State |= SVC_RAW_CAP_ITEM_MEM;

        if (ChkFlag == ( pItnInfo->State & ChkFlag )) {
            pItnInfo->State |= SVC_RAW_CAP_ITEM_RDY;
        }

        PRN_RCP_MSG_LOG "Successful to cfg ituner memory !" PRN_RCP_MSG_API

        SvcRawCap_MutexGive(&(RawCapCtrl.Mutex));
    }

    return RetVal;
}

/**
* configuration of ituner
* @param [in] IkSelectBits selected bits of IK
* @param [in] pFileName file name
* @return 0-OK, 1-NG
*/
UINT32 SvcRawCap_CfgItn(UINT32 IkSelectBits, const char *pFileName)
{
    UINT32 RetVal = SVC_OK;

    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg ituner - create raw cap module first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg ituner - config raw cap module first!" PRN_RCP_MSG_NG
    } else if (pFileName == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg ituner - output file name should not null!" PRN_RCP_MSG_NG
    } else if (IkSelectBits == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg ituner - configure ik context id select bits should not zero!" PRN_RCP_MSG_NG
    } else if (0U != SvcRawCap_MutexTake(&(RawCapCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg ituner - take mutex fail" PRN_RCP_MSG_NG
    } else {
        SVC_RAW_CAP_ITN_INFO_s *pItnInfo = &(RawCapCtrl.ItnInfo);

        if ((pItnInfo->State & SVC_RAW_CAP_ITEM_INIT) == 0U) {
            RetVal = SVC_NG;
            PRN_RCP_MSG_LOG "Fail to cfg ituner - initial ituner info first!" PRN_RCP_MSG_NG
        } else {
            UINT32 ChkFlg = SVC_RAW_CAP_ITEM_INIT | SVC_RAW_CAP_ITEM_CFG | SVC_RAW_CAP_ITEM_MEM;

            pItnInfo->IkSelectBits = IkSelectBits;
            SvcWrap_strcpy(pItnInfo->FileName, sizeof(pItnInfo->FileName), pFileName);
            pItnInfo->State |= SVC_RAW_CAP_ITEM_CFG;

            if ((pItnInfo->State & (ChkFlg)) == ChkFlg) {
                pItnInfo->State |= SVC_RAW_CAP_ITEM_RDY;
            }

            PRN_RCP_MSG_LOG "Successful to cfg ituner!" PRN_RCP_MSG_OK
        }

        SvcRawCap_MutexGive(&(RawCapCtrl.Mutex));
    }

    return RetVal;
}

/**
* vin type configuration of raw capture module
* @param [in] VinID id of vin
* @param [in] Type type, 0-cfa, 1-yuv
* @return 0-OK, 1-NG
*/
UINT32 SvcRawCap_CfgVinType(UINT32 VinID, UINT32 Type)
{
    UINT32 RetVal = SVC_OK;

    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg vin type - create raw cap module first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg vin type - config raw cap module first!" PRN_RCP_MSG_NG
    } else if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg vin type - input VinID(%d) is not valid!"
            PRN_RCP_MSG_ARG_UINT32 VinID PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_NG
    } else if (Type >= SVC_RAW_CAP_VIN_TYPE_NUM) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg vin type - input Type(%d) is not valid!"
            PRN_RCP_MSG_ARG_UINT32 Type PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_NG
    } else if (0U != SvcRawCap_MutexTake(&(RawCapCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg vin type - take mutex fail" PRN_RCP_MSG_NG
    } else {

        if ((RawCapCtrl.ItemCtrlRaw[VinID].State & SVC_RAW_CAP_ITEM_INIT) == 0U) {
            RetVal = SVC_NG;
            PRN_RCP_MSG_LOG "Fail to cfg vin type - initial raw item control first!" PRN_RCP_MSG_NG
        } else {
            RawCapCtrl.ItemCtrlRaw[VinID].RawInfo.VinType = Type;

            PRN_RCP_MSG_LOG "Successful to cfg VinID(%d) type(%d)!"
                PRN_RCP_MSG_ARG_UINT32 VinID PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_ARG_UINT32 RawCapCtrl.ItemCtrlRaw[VinID].RawInfo.VinType PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_OK
        }

        SvcRawCap_MutexGive(&(RawCapCtrl.Mutex));
    }

    return RetVal;
}

/**
* configuration of raw data
* @param [in] VinID id of vin
* @param [in] pFileName file name
* @return 0-OK, 1-NG
*/
UINT32 SvcRawCap_CfgRaw(UINT32 VinID, const char *pFileName)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg raw - create raw cap module first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg raw - config raw cap module first!" PRN_RCP_MSG_NG
    } else if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg raw - input VinID is not valid!" PRN_RCP_MSG_NG
    } else if (pFileName == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg raw - output file name should not null!" PRN_RCP_MSG_NG
    } else if (0U != SvcRawCap_MutexTake(&(RawCapCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg raw - take mutex fail" PRN_RCP_MSG_NG
    } else {
        SVC_RAW_CAP_ITEM_RAW_s *pItem = &(RawCapCtrl.ItemCtrlRaw[VinID]);

        if ((pItem->State & SVC_RAW_CAP_ITEM_INIT) == 0U) {
            RetVal = SVC_NG;
            PRN_RCP_MSG_LOG "Fail to cfg raw - initial raw item control first!" PRN_RCP_MSG_NG
        } else {

            UINT32 VinIdx;
            UINT32 ActualFlags = 0U, WaitFlags = 0U;

            /* Clear flag */
            PRetVal = AmbaKAL_EventFlagClear(&(RawCapCtrl.Flag), 0xFFFFFFFFU); PRN_RCP_ERR_HDLR

            /* Configure wait flag */
            for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
                if ((RawCapCtrl.VinSelectBits & (0x1UL << VinIdx)) > 0U) {
                    WaitFlags |= SvcRawCap_BitGet(VinIdx);
                }
            }

            /* Set flag to query raw info */
            RawCapCtrlFlag |= SVC_RAW_CAP_FLAG_QUERY;

            /* Wait flag for raw info ready */
            PRetVal = AmbaKAL_EventFlagGet(&(RawCapCtrl.CapRdyFlag), WaitFlags, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_AUTO, &ActualFlags, 1000U);
            if (PRetVal != SVC_OK) {
                PRN_RCP_MSG_LOG "Fail to cfg raw - get info fail! flag: 0x%x != 0x%x"
                    PRN_RCP_MSG_ARG_UINT32 WaitFlags   PRN_RCP_MSG_ARG_POST
                    PRN_RCP_MSG_ARG_UINT32 ActualFlags PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_NG
            } else {
                /* Clear flag */
                PRetVal = AmbaKAL_EventFlagClear(&(RawCapCtrl.Flag), 0xFFFFFFFFU); PRN_RCP_ERR_HDLR

                SvcWrap_strcpy(pItem->FileName, sizeof(pItem->FileName), pFileName);

                pItem->State |= SVC_RAW_CAP_ITEM_CFG;
                PRN_RCP_MSG_LOG "Successful to cfg raw!" PRN_RCP_MSG_OK

                RawCapCtrlFlag |= SVC_RAW_CAP_FLAG_CFG_RAW;
            }
        }

        SvcRawCap_MutexGive(&(RawCapCtrl.Mutex));
    }

    return RetVal;
}

/**
* configuration of HDS
* @param [in] VinID id of vin
* @param [in] pFileName file name
* @return 0-OK, 1-NG
*/
UINT32 SvcRawCap_CfgHds(UINT32 VinID, const char *pFileName)
{
    UINT32 RetVal = SVC_OK;

    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg hds - create raw cap module first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg hds - config raw cap module first!" PRN_RCP_MSG_NG
    } else if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg hds - input VinID is not valid!" PRN_RCP_MSG_NG
    } else if (pFileName == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg hds - output file name should not null!" PRN_RCP_MSG_NG
    } else if (0U != SvcRawCap_MutexTake(&(RawCapCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg hds - take mutex fail" PRN_RCP_MSG_NG
    } else {
        SVC_RAW_CAP_ITEM_HDS_s *pItem = &(RawCapCtrl.ItemCtrlHds[VinID]);

        if ((pItem->State & SVC_RAW_CAP_ITEM_INIT) == 0U) {
            RetVal = SVC_NG;
            PRN_RCP_MSG_LOG "Fail to cfg hds - initial raw item control first!" PRN_RCP_MSG_NG
        } else {
            SvcWrap_strcpy(pItem->FileName, sizeof(pItem->FileName), pFileName);

            pItem->State |= SVC_RAW_CAP_ITEM_CFG;
            PRN_RCP_MSG_LOG "Successful to cfg hds!" PRN_RCP_MSG_OK

            RawCapCtrlFlag |= SVC_RAW_CAP_FLAG_CFG_HDS;
        }

        SvcRawCap_MutexGive(&(RawCapCtrl.Mutex));
    }

    return RetVal;
}

/**
* configuration of 3A
* @param [in] VinID id of vin
* @param [in] ImgChanSelectBits selected bits of image channel
* @param [in] pFileName file name
* @return 0-OK, 1-NG
*/
UINT32 SvcRawCap_Cfg3A(UINT32 VinID, UINT32 ImgChanSelectBits, const char *pFileName)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg 3a - create raw cap module first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg 3a - config raw cap module first!" PRN_RCP_MSG_NG
    } else if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg 3a - input VinID is not valid!" PRN_RCP_MSG_NG
    } else if (pFileName == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg 3a - output file name should not null!" PRN_RCP_MSG_NG
    } else if (0U != SvcRawCap_MutexTake(&(RawCapCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg 3a - take mutex fail" PRN_RCP_MSG_NG
    } else {
        SVC_RAW_CAP_ITEM_AAA_s *pItem = &(RawCapCtrl.ItemCtrlAaa[VinID]);

        if ((pItem->State & SVC_RAW_CAP_ITEM_INIT) == 0U) {
            RetVal = SVC_NG;
            PRN_RCP_MSG_LOG "Fail to cfg 3a - initial raw item control first!" PRN_RCP_MSG_NG
        } else if (RawCapCtrl.pExtraCapFunc == NULL) {
            RetVal = SVC_NG;
            PRN_RCP_MSG_LOG "Fail to cfg 3a - need to configure the extra capture callback first!" PRN_RCP_MSG_NG
        } else {
            SVC_RAW_CAP_AAA_DATA_INFO_s AaaDataInfo;

            AmbaSvcWrap_MisraMemset(&AaaDataInfo, 0, sizeof(AaaDataInfo));
            PRetVal = (RawCapCtrl.pExtraCapFunc)(SVC_RAW_CAP_CMD_AAA_INFO_GET, &AaaDataInfo);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_RCP_MSG_LOG "Fail to cfg 3a - get aaa data size fail!" PRN_RCP_MSG_NG
            } else {
                UINT32 ImgChIdx, CurBufSize, ChkFlag = SVC_RAW_CAP_ITEM_INIT | SVC_RAW_CAP_ITEM_CFG;

                CurBufSize = GetAlignedValU32(AaaDataInfo.DataBufSize, 64U);

                pItem->AaaInfo.ImgChanSelectBits = ImgChanSelectBits;
                SvcWrap_strcpy(pItem->FileName, sizeof(pItem->FileName), pFileName);

                pItem->AaaInfo.BufSize = 0U;
                for (ImgChIdx = 0U; ImgChIdx < SVC_RAW_CAP_3A_MAX_CHAN; ImgChIdx ++) {
                    if ((ImgChanSelectBits & (0x1UL << ImgChIdx)) > 0U) {
                        pItem->AaaInfo.ImgAaaBufOfs[ImgChIdx]  = pItem->AaaInfo.BufSize;
                        pItem->AaaInfo.ImgAaaBufSize[ImgChIdx] = CurBufSize;
                        pItem->AaaInfo.BufSize += CurBufSize;
                    }
                }

                pItem->State |= SVC_RAW_CAP_ITEM_CFG;
                if ((pItem->State & ChkFlag) == ChkFlag) {
                    pItem->State |= SVC_RAW_CAP_ITEM_RDY;
                }

                PRN_RCP_MSG_LOG "Successful to cfg 3a!" PRN_RCP_MSG_OK

                RawCapCtrlFlag |= SVC_RAW_CAP_FLAG_CFG_AAA;
            }
        }

        SvcRawCap_MutexGive(&(RawCapCtrl.Mutex));
    }

    return RetVal;
}

/**
* callback function configuration of extra capture
* @param [in] pFunc callback function
* @return 0-OK, 1-NG
*/
UINT32 SvcRawCap_CfgExtCapCb(SVC_RAW_CAP_EXTRA_CAP_FUNC_f pFunc)
{
    UINT32 RetVal = SVC_OK;

    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg extra capture callback - create raw cap module first!" PRN_RCP_MSG_NG
        AmbaMisra_TouchUnused(&pFunc);
    } else if (0U != SvcRawCap_MutexTake(&(RawCapCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to cfg extra capture callback - take mutex fail" PRN_RCP_MSG_NG
    } else {
        RawCapCtrl.pExtraCapFunc = pFunc;

        PRN_RCP_MSG_LOG "Successful to cfg extra capture callback!" PRN_RCP_MSG_OK

        SvcRawCap_MutexGive(&(RawCapCtrl.Mutex));
    }

    return RetVal;
}

/**
* info get of raw and HDS
* @param [in] VinID id of vin
* @param [out] pRawInfo info of raw data
* @param [out] pHdsInfo info of HDS data
* @return 0-OK, 1-NG
*/
UINT32 SvcRawCap_Info(UINT32 VinID, SVC_RAW_CAP_INFO_s *pRawInfo, SVC_RAW_CAP_INFO_s *pHdsInfo)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to get raw/hds info - create raw cap module first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to get raw/hds info - config raw cap module first!" PRN_RCP_MSG_NG
    } else if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to get raw/hds info - input VinID is not valid!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrl.VinSelectBits & (0x1UL << VinID)) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to get raw/hds info - request VinID(%d) is not configured"
            PRN_RCP_MSG_ARG_UINT32 VinID PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_NG
    } else if ((pRawInfo == NULL) && (pHdsInfo == NULL)) {
        PRN_RCP_MSG_LOG "Output raw/hds info is null!" PRN_RCP_MSG_API
    } else if (0U != SvcRawCap_MutexTake(&(RawCapCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to get raw/hds info - take mutex fail" PRN_RCP_MSG_NG
    } else {
        UINT32 VinIdx;
        UINT32 ActualFlags = 0U, WaitFlags = 0U;

        /* Clear flag */
        PRetVal = AmbaKAL_EventFlagClear(&(RawCapCtrl.Flag), 0xFFFFFFFFU); PRN_RCP_ERR_HDLR

        /* Configure wait flag */
        for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
            if ((RawCapCtrl.VinSelectBits & (0x1UL << VinIdx)) > 0U) {
                WaitFlags |= SvcRawCap_BitGet(VinIdx);
            }
        }

        /* Set flag to query raw info */
        RawCapCtrlFlag |= SVC_RAW_CAP_FLAG_QUERY;

        /* Wait flag for raw info ready */
        PRetVal = AmbaKAL_EventFlagGet(&(RawCapCtrl.CapRdyFlag), WaitFlags, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_AUTO, &ActualFlags, 1000U);
        if (PRetVal != SVC_OK) {
            PRN_RCP_MSG_LOG "Fail to get raw/hds info - get info fail! flag: 0x%x != 0x%x"
                PRN_RCP_MSG_ARG_UINT32 WaitFlags   PRN_RCP_MSG_ARG_POST
                PRN_RCP_MSG_ARG_UINT32 ActualFlags PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_NG
        } else {

            /* Clear flag */
            PRetVal = AmbaKAL_EventFlagClear(&(RawCapCtrl.Flag), 0xFFFFFFFFU); PRN_RCP_ERR_HDLR

            if (pRawInfo != NULL) {
                AmbaSvcWrap_MisraMemset(pRawInfo, 0, sizeof(SVC_RAW_CAP_INFO_s));
                pRawInfo->Pitch  = RawCapCtrl.ItemCtrlRaw[VinID].RawInfo.Pitch ;
                pRawInfo->Width  = RawCapCtrl.ItemCtrlRaw[VinID].RawInfo.Width ;
                pRawInfo->Height = RawCapCtrl.ItemCtrlRaw[VinID].RawInfo.Height;
                pRawInfo->pBuf   = RawCapCtrl.ItemCtrlRaw[VinID].RawInfo.pBuf  ;
            }

            if (pHdsInfo != NULL) {

                AmbaSvcWrap_MisraMemset(pHdsInfo, 0, sizeof(SVC_RAW_CAP_INFO_s));
                if ((RawCapCtrl.ItmSelectBits & SVC_RAW_CAP_CFG_HDS) > 0U) {
                    AmbaSvcWrap_MisraMemcpy(pHdsInfo,
                                              &(RawCapCtrl.ItemCtrlHds[VinID].HdsInfo),
                                              sizeof(SVC_RAW_CAP_INFO_s));
                }
            }
        }

        SvcRawCap_MutexGive(&(RawCapCtrl.Mutex));
    }


    return RetVal;
}

/**
* memory query of raw capture module
* @param [in] CapNum number of capture
* @param [out] pBufSize size of buffer
* @return none
*/
void SvcRawCap_MemQry(UINT32 CapNum, UINT32 *pBufSize)
{
    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_INIT) == 0U) {
        PRN_RCP_MSG_LOG "Fail to query memory - create raw cap module first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG) == 0U) {
        PRN_RCP_MSG_LOG "Fail to query memory - config raw cap module first!" PRN_RCP_MSG_NG
    } else if (pBufSize == NULL) {
        PRN_RCP_MSG_LOG "Fail to query memory - buffer size variable should not null!" PRN_RCP_MSG_NG
    } else if (0U != SvcRawCap_MutexTake(&(RawCapCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
        PRN_RCP_MSG_LOG "Fail to query memory - take mutex fail" PRN_RCP_MSG_NG
    } else {
        UINT32 RawBufSize = 0U, HdsBufSize = 0U, AaaBufSize = 0U;

        SvcRawCap_MemQuery(&RawBufSize, &HdsBufSize, &AaaBufSize);
        *pBufSize = (RawBufSize + HdsBufSize) + AaaBufSize;
        if (CapNum > 0U) {
            *pBufSize *= CapNum;
        }

        SvcRawCap_MutexGive(&(RawCapCtrl.Mutex));
    }
}

/**
* do raw data capture
* @param [in] CapNum number of requested capture
* @param [in] IsBlocked block task or not
* @return 0-OK, 1-NG
*/
UINT32 SvcRawCap_CapSeq(UINT32 CapNum, UINT32 IsBlocked)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to capture raw sequence - create raw cap module first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to capture raw sequence - config raw cap module first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG_MEM) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to capture raw sequence - config raw cap memory first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG_RAW) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to capture raw sequence - config raw cap module first!" PRN_RCP_MSG_NG
    } else if (RawCapCtrl.VinSelectBits == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to capture raw sequence - invalid vin selected bits!" PRN_RCP_MSG_NG
    } else if (0U != SvcRawCap_MutexTake(&(RawCapCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to capture raw sequence - take mutex fail" PRN_RCP_MSG_NG
    } else {

        /* Reset all flag setting */
        RetVal = SvcRawCap_CapReset();
        if (RetVal != SVC_OK) {
            PRN_RCP_MSG_LOG "Fail to capture raw sequence - reset cap setting fail!" PRN_RCP_MSG_NG
        } else {
            PRN_RCP_MSG_LOG "Successful reset cap setting!" PRN_RCP_MSG_DBG
        }

        /* Setup memory */
        if (RetVal == SVC_OK) {
            if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CAP_INT_BUF) > 0U) {
                if (CapNum > 1U) {
                    PRN_RCP_MSG_LOG "Current capture module only support 1 frame!" PRN_RCP_MSG_API
                }
                RetVal = SvcRawCap_MemSetup(1U);
            } else {
                RetVal = SvcRawCap_MemSetup(CapNum);
            }
            if (RetVal != SVC_OK) {
                PRN_RCP_MSG_LOG "Fail to capture raw sequence - setup memory fail" PRN_RCP_MSG_NG
            } else {
                PRN_RCP_MSG_LOG "Successful to setup memory!" PRN_RCP_MSG_DBG
            }
        }

        /* Setup capture */
        if (RetVal == SVC_OK) {
            RetVal = SvcRawCap_CapSetup();
            if (RetVal != SVC_OK) {
                PRN_RCP_MSG_LOG "Fail to capture raw sequence - setup capture fail!" PRN_RCP_MSG_NG
            }
        }

        /* Trigger capture */
        if (RetVal == SVC_OK) {
            RetVal = SvcRawCap_CapStart();
            if (RetVal != SVC_OK) {
                PRN_RCP_MSG_LOG "Fail to capture raw sequence - capture raw fail!" PRN_RCP_MSG_NG
            }
        }

        if (RetVal == SVC_OK) {
            PRN_RCP_MSG_LOG "Successful to start capture raw sequence!" PRN_RCP_MSG_OK
        }

        SvcRawCap_MutexGive(&(RawCapCtrl.Mutex));

        PRetVal = SvcRawCap_CapDone(CapNum, IsBlocked); PRN_RCP_ERR_HDLR
    }

    return RetVal;
}

/**
* status dump of raw capture module
* @return 0-OK, 1-NG
*/
UINT32 SvcRawCap_Dump(void)
{
    UINT32 RetVal = SVC_OK;

    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to capture raw - create raw cap first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to capture raw - config raw cap first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG_MEM) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to capture raw - config raw cap memory first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG_RAW) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to capture raw - config raw setting first!" PRN_RCP_MSG_NG
    } else if (0U != SvcRawCap_MutexTake(&(RawCapCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to capture raw sequence - take mutex fail" PRN_RCP_MSG_NG
    } else {
        UINT32 VinIdx;

        for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
            if ((RawCapCtrl.VinSelectBits & (0x1UL << VinIdx)) > 0U) {
                RetVal = SvcRawCap_DumpData(VinIdx);
            }
        }

        SvcRawCap_MutexGive(&(RawCapCtrl.Mutex));
    }

    return RetVal;
}

/**
* raw data get
* @param [in] VinID id of vin
* @param [in] pRawInfo info block of raw data
* @return 0-OK, 1-NG
*/
UINT32 SvcRawCap_GetRaw(UINT32 VinID, SVC_RAW_CAP_INFO_s *pRawInfo)
{
    UINT32 RetVal = SVC_OK;

    if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to get raw - create raw cap first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to get raw - config raw cap first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG_MEM) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to get raw - config raw cap memory first!" PRN_RCP_MSG_NG
    } else if ((RawCapCtrlFlag & SVC_RAW_CAP_FLAG_CFG_RAW) == 0U) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to get raw - config raw setting first!" PRN_RCP_MSG_NG
    } else if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to get raw - VinID(%d) is out-of range!"
            PRN_RCP_MSG_ARG_UINT32 VinID PRN_RCP_MSG_ARG_POST
        PRN_RCP_MSG_NG
    } else if (pRawInfo == NULL) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to get raw - invalid output raw info!" PRN_RCP_MSG_NG
    } else if (0U != SvcRawCap_MutexTake(&(RawCapCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
        RetVal = SVC_NG;
        PRN_RCP_MSG_LOG "Fail to get raw - take mutex fail" PRN_RCP_MSG_NG
    } else {
        if (RawCapCtrl.ItemCtrlRaw[VinID].NumCaptured == 0U) {
            RetVal = SVC_NG;
            PRN_RCP_MSG_LOG "Fail to get raw - the raw queue is empty!" PRN_RCP_MSG_NG
        } else if (RawCapCtrl.ItemCtrlRaw[VinID].pBufSeqTbl == NULL) {
            RetVal = SVC_NG;
            PRN_RCP_MSG_LOG "Fail to get raw - raw seq buf table should not null!" PRN_RCP_MSG_NG
        } else {
            UINT32 RawBufSize;
            UINT8 *pRawBuf;

            pRawBuf    = RawCapCtrl.ItemCtrlRaw[VinID].pBufSeqTbl[0];
            RawBufSize = RawCapCtrl.ItemCtrlRaw[VinID].RawInfo.Pitch * RawCapCtrl.ItemCtrlRaw[VinID].RawInfo.Height;

            pRawInfo->Width  = RawCapCtrl.ItemCtrlRaw[VinID].RawInfo.Width;
            pRawInfo->Height = RawCapCtrl.ItemCtrlRaw[VinID].RawInfo.Height;
            pRawInfo->Pitch  = RawCapCtrl.ItemCtrlRaw[VinID].RawInfo.Pitch;

            if (pRawInfo->pBuf != NULL) {
                AmbaSvcWrap_MisraMemcpy(pRawInfo->pBuf, pRawBuf, RawBufSize);
            } else {
                pRawInfo->pBuf = pRawBuf;
                PRN_RCP_MSG_LOG "Output raw capture extra buffer" PRN_RCP_MSG_DBG
            }

            PRN_RCP_MSG_LOG "Successful to get VinID(%d) raw data"
                PRN_RCP_MSG_ARG_UINT32 VinID PRN_RCP_MSG_ARG_POST
            PRN_RCP_MSG_DBG
        }

        SvcRawCap_MutexGive(&(RawCapCtrl.Mutex));
    }

    return RetVal;
}


