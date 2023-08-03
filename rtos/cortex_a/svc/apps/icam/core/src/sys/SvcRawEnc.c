/*
*  @file SvcRawEnc.c
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
*  @details  svc raw encode
*
*/

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaDSP.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaShell.h"
#include "AmbaFS.h"
#include "AmbaSensor.h"
#include "AmbaFPD.h"

#include "AmbaTUNE_HdlrManager.h"

#include "SvcErrCode.h"
#include "SvcWrap.h"
#include "SvcMem.h"
#include "SvcResCfg.h"
#include "SvcLog.h"
#include "SvcUtil.h"
#include "SvcLiveview.h"
#include "SvcBuffer.h"
#include "SvcRawEnc.h"
#include "SvcCmd.h"

#define SVC_RAW_ENC_FLG_INIT                (0x1U)
#define SVC_RAW_ENC_FLG_DEF_RES_CFG         (0x2U)
#define SVC_RAW_ENC_FLG_DEF_IDSP_CFG        (0x4U)
#define SVC_RAW_ENC_FLG_DEF_DMY_SENSOR      (0x8U)
#define SVC_RAW_ENC_FLG_DEF_VOUT_DEV        (0x10U)
#define SVC_RAW_ENC_FLG_SHELL_INIT          (0x1000U)
#define SVC_RAW_ENC_FLG_SHELL_USAGE_MORE    (0x2000U)
#define SVC_RAW_ENC_FLG_STRM_CAP_MEM_INIT   (0x4000U)

#define SVC_RAW_ENC_FLG_MSG_OK          (0x1000000U)
#define SVC_RAW_ENC_FLG_MSG_NG          (0x2000000U)
#define SVC_RAW_ENC_FLG_MSG_API         (0x4000000U)
#define SVC_RAW_ENC_FLG_MSG_DBG         (0x8000000U)
#define SVC_RAW_ENC_FLG_MSG_SHELL       (0x80000000U)
#define SVC_RAW_ENC_FLG_MSG_DEF         (SVC_RAW_ENC_FLG_MSG_OK | SVC_RAW_ENC_FLG_MSG_NG | SVC_RAW_ENC_FLG_MSG_API)

#define SVC_RAW_ENC_MAX_FILE_NAME       (128U)
#define SVC_RAW_ENC_MAX_MUTX_TIMEOUT    (10000U)
#define SVC_RAW_ENC_3A_MAX_CHAN         (4U)
#define SVC_RAW_ENC_MAX_VOUT_DEV_NUM    (16U)
#define SVC_RAW_ENC_RAW_PITCH_ALIGN     (64U)
#define SVC_RAW_ENC_BUF_CHK_IDX         (1U)

typedef struct {
    UINT32  Status;
#define SVC_RAW_ENC_BUF_FREE    (0x1U)
#define SVC_RAW_ENC_BUF_USED    (0x2U)
    UINT8  *pRawBuf;
    UINT32  RawBufSize;

    UINT8  *pHdsBuf;
    UINT32  HdsBufSize;
} SVC_RAW_ENC_BUF_INFO_s;

typedef struct {
    UINT32                   Status;
#define SVC_RAW_ENC_BUF_INIT  (0x1U)
#define SVC_RAW_ENC_BUF_READY (0x2U)
#define SVC_RAW_ENC_BUF_BUSY  (0x4U)
    UINT32                   VinID;
    char                     MutexName[32];
    AMBA_KAL_MUTEX_t         Mutex;
    UINT32                   RawBufSize;
    UINT32                   HdsBufSize;
    char                     FreeQueName[32];
    AMBA_KAL_MSG_QUEUE_t     FreeQue;
    SVC_RAW_ENC_BUF_INFO_s   FreeQueBuf[SVC_RAW_ENC_MAX_BUF_NUM];
    char                     DataQueName[32];
    AMBA_KAL_MSG_QUEUE_t     DataQue;
    SVC_RAW_ENC_BUF_INFO_s   DataQueBuf[SVC_RAW_ENC_MAX_BUF_NUM];
    UINT8                   *pMemBuf;
    UINT32                   MemSize;
} SVC_RAW_ENC_BUF_CTRL;

typedef struct {
    UINT32 Status;
#define SVC_RAW_ENC_RAW_UPDATE (0x1U)
#define SVC_RAW_ENC_HDS_UPDATE (0x2U)
#define SVC_RAW_ENC_AAA_UPDATE (0x4U)
    UINT8 *pPreRawBuf;
    UINT32 PreRawBufSize;
    UINT8 *pPreHdsBuf;
    UINT32 PreHdsBufSize;
    char   RawFilePath[SVC_RAW_ENC_MAX_FILE_NAME];
    char   HdsFilePath[SVC_RAW_ENC_MAX_FILE_NAME];
    AMBA_IK_MODE_CFG_s IKMode[SVC_RAW_ENC_3A_MAX_CHAN];
    UINT32 ImgChanSelBits;
    char   ImgChanFilePath[SVC_RAW_ENC_3A_MAX_CHAN][SVC_RAW_ENC_MAX_FILE_NAME];
} SVC_RAW_ENC_VIN_CFG_s;

typedef struct {
    UINT32             Status;
#define SVC_RAW_ENC_ITN_UPDATE (0x1U)
    AMBA_IK_MODE_CFG_s IKMode;
    char               ItnFilePath[SVC_RAW_ENC_MAX_FILE_NAME];
} SVC_RAW_ENC_VIEW_ZONE_CFG_s;

typedef struct {
    UINT32 DataFmt;
    UINT32 Pitch;
    UINT32 Width;
    UINT32 Height;
    UINT8 *pYBuf;
    UINT8 *pUVBuf;
} SVC_RAW_ENC_STRM_INFO_s;

typedef struct {
    AMBA_KAL_MUTEX_t             Mutex;
    AMBA_KAL_EVENT_FLAG_t        EventFlag;
#define SVC_RAW_ENC_EVT_FLG_RAW_RDY (0x10UL)
#define SVC_RAW_ENC_EVT_FLG_YUV_RDY (0x100UL)
    char                         Name[16];
    UINT32                       VinSelectBits;
    SVC_RAW_ENC_VIN_CFG_s        VinCfg[AMBA_DSP_MAX_VIN_NUM];
    UINT32                       ViewZoneSelectBits;
    SVC_RAW_ENC_VIEW_ZONE_CFG_s  ViewZoneCfg[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT32                       ViewZoneIdspBits;
    SVC_RAW_ENC_BUF_CTRL         BufCtrl[AMBA_DSP_MAX_VIN_NUM];
    UINT8                       *pMemBuf;
    UINT32                       MemSize;
    UINT8                       *pItnMemBuf;
    UINT32                       ItnMemSize;
    UINT32                       FeedCounter;
    SVC_LIV_INFO_s               LivInfo;
    UINT32                       EncStrmBits;
    SVC_RAW_ENC_STRM_INFO_s      EncStrmInfo[AMBA_DSP_MAX_STREAM_NUM];
    AMBA_FS_FILE                *pDumpLogFile;
} SVC_RAW_ENC_CTRL_s;

typedef struct {
    struct {
        UINT32  InputClk;
        UINT32  LineLengthPck;
        UINT32  FrameLengthLines;
        AMBA_VIN_FRAME_RATE_s FrameRate;
    } FrameTiming;
    FLOAT       RowTime;
} SVC_RAW_ENC_DMY_SEN_INFO_s;

typedef struct {
    AMBA_SENSOR_OUTPUT_INFO_s  OutputInfo;
    AMBA_SENSOR_INPUT_INFO_s   InputInfo;
    SVC_RAW_ENC_DMY_SEN_INFO_s ModeInfo;
    AMBA_SENSOR_HDR_INFO_s     HdrInfo;
} SVC_RAW_ENC_DMY_SEN_CFG_s;

typedef struct {
    UINT32             ID;
    char               DevName[32];
    AMBA_FPD_OBJECT_s *pObj;
} SVC_RAW_ENC_VOUT_DEV_INFO_s;

static void    SvcRawEnc_PrintLog(UINT32 LogLevel, SVC_WRAP_PRINT_s *pPrint);
static UINT32  SvcRawEnc_MutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut);
static void    SvcRawEnc_MutexGive(AMBA_KAL_MUTEX_t *pMutex);
static void    SvcRawEnc_ErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode);

static UINT32  SvcRawEnc_BufCtrlCreate(SVC_RAW_ENC_BUF_CTRL *pCtrl, UINT8 *pMemBuf, UINT32 MemSize, UINT32 VinID);
static UINT32  SvcRawEnc_BufCtrlDelete(SVC_RAW_ENC_BUF_CTRL *pCtrl);
static UINT32  SvcRawEnc_BufCtrlGetBuf(SVC_RAW_ENC_BUF_CTRL *pCtrl, SVC_RAW_ENC_BUF_INFO_s *pBuf);
static UINT32  SvcRawEnc_BufCtrlLockBuf(SVC_RAW_ENC_BUF_CTRL *pCtrl, SVC_RAW_ENC_BUF_INFO_s *pBuf);
static UINT32  SvcRawEnc_BufCtrlFreeBuf(SVC_RAW_ENC_BUF_CTRL *pCtrl, SVC_RAW_ENC_BUF_INFO_s *pBuf);
static void    SvcRawEnc_BufCtrlInfo(SVC_RAW_ENC_BUF_CTRL *pCtrl);

static UINT32  SvcRawEnc_DspEvtRawDataRdy(const void *pEventData);
static UINT32  SvcRawEnc_DspEvtLivYuvRdy(const void *pEventData);

static UINT32  SvcRawEnc_MemoryQuery(const SVC_RES_CFG_s *pCfg, UINT32 *pRawMemSize, UINT32 *pHdsMemSize);
static UINT32  SvcRawEnc_MemoryConfig(UINT8 *pMemBuf, UINT32 MemSize);
static UINT32  SvcRawEnc_RawDataCfg(UINT32 NumCfg, UINT32 *pVinIDArr, char **pRawFilePath);
static UINT32  SvcRawEnc_HdsDataCfg(UINT32 NumCfg, UINT32 *pVinIDArr, char **pHdsFilePath);
static UINT32  SvcRawEnc_AaaDataCfg(UINT32 VinID, UINT32 IkCtxID, UINT32 ImgChanID, char *pAaaFilePath);
static UINT32  SvcRawEnc_ViewZoneCfg(UINT32 NumViewZone, UINT32 *pViewZoneIDArr, char **pItnPath);
static UINT32  SvcRawEnc_ExecuteRawData(SVC_RAW_ENC_VIN_CFG_s *pCfg, UINT8 *pBuf, UINT32 BufSize);
static UINT32  SvcRawEnc_ExecuteHdsData(SVC_RAW_ENC_VIN_CFG_s *pCfg, UINT8 *pBuf, UINT32 BufSize);
static UINT32  SvcRawEnc_ExecuteAaaData(SVC_RAW_ENC_VIN_CFG_s *pCfg, UINT8 *pBuf, UINT32 BufSize);
static UINT32  SvcRawEnc_ExecuteItnData(void);
static UINT32  SvcRawEnc_ExecuteIdsp(void);
static UINT32  SvcRawEnc_ExecuteCmd(UINT32 NumVin, const UINT16 *pViewZoneIDArr, const AMBA_DSP_LIVEVIEW_EXT_RAW_s *pLivExtRaw, SVC_RAW_ENC_BUF_INFO_s *pBufInfo);
static UINT32  SvcRawEnc_VinSrcCfg(UINT32 NumCfg, const UINT32 *pVinIDArr, const UINT32 *pVinSrc);
static void    SvcRawEnc_DefResCfgInit(void);
static void    SvcRawEnc_ResCfgPrint(SVC_RES_CFG_s *pCfg);
static void    SvcRawEnc_DefDmySenCfgInit(void);
static void    SvcRawEnc_DmySenCfgPrint(void);
static SVC_RAW_ENC_DMY_SEN_CFG_s *SvcRawEnc_DmySenCfgGet(void);
static void    SvcRawEnc_CfgPrint(void);
static void    SvcRawEnc_ProcCallBack(UINT32 Code, void *pData);
static void    SvcRawEnc_VoutDevInit(void);
static void    SvcRawEnc_DumpYuvData(const char *pFilePath);

// Private function definition for shell command.
typedef UINT32 (*SVC_RAW_ENC_SHELL_FUNC_f)(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
typedef void   (*SVC_RAW_ENC_SHELL_USAGE_f)(UINT32 CtrlFlag, void *pFunc);
typedef struct {
    UINT32                    Enable;
    UINT32                    IndentLevel;
    char                      CmdName[32];
    char                      CmdDescription[64];
    SVC_RAW_ENC_SHELL_FUNC_f  pFunc;
    SVC_RAW_ENC_SHELL_USAGE_f pUsage;
    const void               *pSubFunc;
    UINT32                    NumSubFunc;
} SVC_RAW_ENC_SHELL_FUNC_s;

static char   SvcRawEncShellStr[1024] GNU_SECTION_NOZEROINIT;
static void   SvcRawEnc_CmdStrToPointer(const char *pStr, UINT8 **pPointer);
static void   SvcRawEnc_CmdStrToU32(const char *pString, UINT32 *pValue);
static void   SvcRawEnc_ShellLog(UINT32 LogLevel, SVC_WRAP_PRINT_s *pPrint, const void *pFunc);
static void   SvcRawEnc_ShellTitle(UINT32 CtrlFlag, UINT32 Level, const char *pCmdStr, const char *pDescription);
static void   SvcRawEnc_ShellEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
static void   SvcRawEnc_ShellUsage(UINT32 CtrlFlag);
static void   SvcRawEnc_ShellEntryInit(void);
static UINT32 SvcRawEnc_ShellCommEntry(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellCommEntryU(UINT32 CtrlFlag, void *pFunc);
static void   SvcRawEnc_ShellEmptyUsage(UINT32 CtrlFlag, void *pFunc);

static UINT32 SvcRawEnc_ShellCreate(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static UINT32 SvcRawEnc_ShellDelete(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static UINT32 SvcRawEnc_ShellConfig(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static UINT32 SvcRawEnc_ShellResCfg(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static UINT32 SvcRawEnc_ShellDmySen(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static UINT32 SvcRawEnc_ShellExecute(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static UINT32 SvcRawEnc_ShellDumpYuv(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellDumpYuvU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellDbgMsg(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellDbgMsgU(UINT32 CtrlFlag, void *pFunc);

#define SVC_RAW_ENC_SHELL_NUM (8U)
static SVC_RAW_ENC_SHELL_FUNC_s SvcRawEncShellFunc[SVC_RAW_ENC_SHELL_NUM] GNU_SECTION_NOZEROINIT;

static UINT32 SvcRawEnc_ShellCfgMem(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellCfgMemU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellCfgRaw(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellCfgRawU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellCfgHds(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellCfgHdsU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellCfgItn(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellCfgItnU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellCfgAaa(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellCfgAaaU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellCfgInfo(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static UINT32 SvcRawEnc_ShellBufCtrlInfo(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static UINT32 SvcRawEnc_ShellCfgSrc(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellCfgSrcU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellCfgIqTbl(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);

#define SVC_RAW_ENC_CFG_SHELL_NUM (9U)
static SVC_RAW_ENC_SHELL_FUNC_s SvcRawEncCfgShellFunc[SVC_RAW_ENC_CFG_SHELL_NUM] GNU_SECTION_NOZEROINIT;

static UINT32 SvcRawEnc_ShellCfgIqTblAdj(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellCfgIqTblAdjU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellCfgIqTblImg(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellCfgIqTblImgU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellCfgIqTblAaa(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellCfgIqTblAaaU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellCfgIqTblVideo(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellCfgIqTblVideoU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellCfgIqTblPhoto(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellCfgIqTblPhotoU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellCfgIqTblVidMSM(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellCfgIqTblVidMSMU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellCfgIqTblVidMSH(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellCfgIqTblVidMSHU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellCfgIqTblPhoMSM(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellCfgIqTblPhoMSMU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellCfgIqTblPhoMSH(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellCfgIqTblPhoMSHU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellCfgIqTblSLiso(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellCfgIqTblSLisoU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellCfgIqTblSHiso(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellCfgIqTblSHisoU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellCfgIqTblVidCc(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellCfgIqTblVidCcU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellCfgIqTblStiCc(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellCfgIqTblStiCcU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellCfgIqTblScene(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellCfgIqTblSceneU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellCfgIqTblDVid(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellCfgIqTblDVidU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellCfgIqTblDSti(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellCfgIqTblDStiU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellCfgIqTblVidPrm(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellCfgIqTblVidPrmU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellCfgIqTblStlPrm(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellCfgIqTblStlPrmU(UINT32 CtrlFlag, void *pFunc);

#define SVC_RAW_ENC_CFG_IQ_SHELL_NUM (18U)
static SVC_RAW_ENC_SHELL_FUNC_s SvcRawEncCfgIqTblShellFunc[SVC_RAW_ENC_CFG_IQ_SHELL_NUM] GNU_SECTION_NOZEROINIT;

static UINT32 SvcRawEnc_ShellResCfgVin(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static UINT32 SvcRawEnc_ShellResCfgSen(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellResCfgSenU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellResCfgFov(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static UINT32 SvcRawEnc_ShellResCfgDisp(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static UINT32 SvcRawEnc_ShellResCfgRec(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static UINT32 SvcRawEnc_ShellResCfgReset(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static UINT32 SvcRawEnc_ShellResCfgInfo(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);

#define SVC_RAW_ENC_RES_CFG_SHELL_NUM (7U)
static SVC_RAW_ENC_SHELL_FUNC_s SvcRawEncResCfgShellFunc[SVC_RAW_ENC_RES_CFG_SHELL_NUM] GNU_SECTION_NOZEROINIT;

static UINT32 SvcRawEnc_ShellResCfgVinTree(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellResCfgVinTreeU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellResCfgVinNum(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellResCfgVinNumU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellResCfgVinWin(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellResCfgVinWinU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellResCfgVinSCNum(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellResCfgVinSCNumU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellResCfgVinSChan(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellResCfgVinSChanU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellResCfgVinFrate(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellResCfgVinFrateU(UINT32 CtrlFlag, void *pFunc);

#define SVC_RAW_ENC_RES_VIN_SHELL_NUM (6U)
static SVC_RAW_ENC_SHELL_FUNC_s SvcRawEncResCfgVinShellFunc[SVC_RAW_ENC_RES_VIN_SHELL_NUM] GNU_SECTION_NOZEROINIT;

static UINT32 SvcRawEnc_ShellResCfgFovNum(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellResCfgFovNumU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellResCfgFovRaw(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellResCfgFovRawU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellResCfgFovAct(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellResCfgFovActU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellResCfgFovMain(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellResCfgFovMainU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellResCfgFovPyr(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellResCfgFovPyrU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellResCfgFovHier(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellResCfgFovHierU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellResCfgFovPipe(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellResCfgFovPipeU(UINT32 CtrlFlag, void *pFunc);

#define SVC_RAW_ENC_RES_FOV_SHELL_NUM (7U)
static SVC_RAW_ENC_SHELL_FUNC_s SvcRawEncResCfgFovShellFunc[SVC_RAW_ENC_RES_FOV_SHELL_NUM] GNU_SECTION_NOZEROINIT;

static UINT32 SvcRawEnc_ShellResCfgDispBits(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellResCfgDispBitsU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellResCfgDispId(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellResCfgDispIdU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellResCfgDispObj(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellResCfgDispObjU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellResCfgDispWin(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellResCfgDispWinU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellResCfgDispMWin(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellResCfgDispMWinU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellResCfgDispCNum(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellResCfgDispCNumU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellResCfgDispCCfg(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellResCfgDispCCfgU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellResCfgDispFrat(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellResCfgDispFratU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellResCfgDispCfg(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellResCfgDispCfgU(UINT32 CtrlFlag, void *pFunc);

#define SVC_RAW_ENC_RES_DISP_SHELL_NUM (9U)
static SVC_RAW_ENC_SHELL_FUNC_s SvcRawEncResCfgDispShellFunc[SVC_RAW_ENC_RES_DISP_SHELL_NUM] GNU_SECTION_NOZEROINIT;

static UINT32 SvcRawEnc_ShellResCfgRecBits(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellResCfgRecBitsU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellResCfgRecSetup(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellResCfgRecSetupU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellResCfgRecWin(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellResCfgRecWinU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellResCfgRecMWin(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellResCfgRecMWinU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellResCfgRecCNum(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellResCfgRecCNumU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellResCfgRecCCfg(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellResCfgRecCCfgU(UINT32 CtrlFlag, void *pFunc);

#define SVC_RAW_ENC_RES_REC_SHELL_NUM (6U)
static SVC_RAW_ENC_SHELL_FUNC_s SvcRawEncResCfgRecShellFunc[SVC_RAW_ENC_RES_REC_SHELL_NUM] GNU_SECTION_NOZEROINIT;

static UINT32 SvcRawEnc_ShellDmySenReset(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static UINT32 SvcRawEnc_ShellDmySenOutput(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellDmySenOutputU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellDmySenInput(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellDmySenInputU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellDmySenMInfo(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellDmySenMInfoU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellDmySenHdrCh(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellDmySenHdrChU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellDmySenHdr(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);
static void   SvcRawEnc_ShellDmySenHdrU(UINT32 CtrlFlag, void *pFunc);
static UINT32 SvcRawEnc_ShellDmySenInfo(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData);

#define SVC_RAW_ENC_DMY_SEN_SHELL_NUM (7U)
static SVC_RAW_ENC_SHELL_FUNC_s SvcRawEncDmySenShellFunc[SVC_RAW_ENC_DMY_SEN_SHELL_NUM] GNU_SECTION_NOZEROINIT;

static SVC_RAW_ENC_CTRL_s SVC_RawEncCtrl GNU_SECTION_NOZEROINIT;
static UINT32 SVC_RawEncCtrlFlag = SVC_RAW_ENC_FLG_MSG_DEF;

static SVC_RES_CFG_s RawEncResCfg GNU_SECTION_NOZEROINIT;
static UINT32        RawEncResCfgVinID = 255U;
static SVC_RAW_ENC_CALLBACK_f RawEncCallBackProc = NULL;
static SVC_RAW_ENC_DMY_SEN_CFG_s RawEncDmySenCfg GNU_SECTION_NOZEROINIT;
static AMBA_SENSOR_OBJ_s RawEncDmySenObj GNU_SECTION_NOZEROINIT;
static SVC_RAW_ENC_VOUT_DEV_INFO_s RawEncVoutDevInfo[SVC_RAW_ENC_MAX_VOUT_DEV_NUM] GNU_SECTION_NOZEROINIT;

static char RawEncStrmCapFileName[AMBA_DSP_MAX_STREAM_NUM][MAX_FILE_NAME_LEN] GNU_SECTION_NOZEROINIT;

static UINT32  SvcRawEnc_DmySenInit(const AMBA_SENSOR_CHANNEL_s *pChan);
static UINT32  SvcRawEnc_DmySenEnable(const AMBA_SENSOR_CHANNEL_s *pChan);
static UINT32  SvcRawEnc_DmySenDisable(const AMBA_SENSOR_CHANNEL_s *pChan);
static UINT32  SvcRawEnc_DmySenConfig(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode);
static UINT32  SvcRawEnc_DmySenGetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus);
static UINT32  SvcRawEnc_DmySenGetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo);
static UINT32  SvcRawEnc_DmySenGetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo);
static UINT32  SvcRawEnc_DmySenGetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo);
static UINT32  SvcRawEnc_DmySenGetGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor);
static UINT32  SvcRawEnc_DmySenGetShutterSpd(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime);
static UINT32  SvcRawEnc_DmySenCvtGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl);
static UINT32  SvcRawEnc_DmySenCvtShutterSpd(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl);
static UINT32  SvcRawEnc_DmySenSetAGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl);
static UINT32  SvcRawEnc_DmySenSetDGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pDigitalGainCtrl);
static UINT32  SvcRawEnc_DmySenSetWbGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl);
static UINT32  SvcRawEnc_DmySenSetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl);
static UINT32  SvcRawEnc_DmySenSetSlowShuCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl);
static UINT32  SvcRawEnc_DmySenRegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16* pData);
static UINT32  SvcRawEnc_DmySenRegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data);

#define SVC_LOG_RENC "RENC"
#define SVC_LOG_RENC_HL_TITLE_0   "\033""[38;2;255;125;38m"
#define SVC_LOG_RENC_HL_TITLE_1   "\033""[38;2;255;255;128m"
#define SVC_LOG_RENC_HL_TITLE_1_U "\033""[38;2;255;255;128m""\033""[4m"
#define SVC_LOG_RENC_HL_TITLE_2   "\033""[38;2;100;255;255m"
#define SVC_LOG_RENC_HL_TITLE_2_U "\033""[38;2;100;255;255m""\033""[4m"
#define SVC_LOG_RENC_HL_TITLE_3   "\033""[38;2;255;128;128m"
#define SVC_LOG_RENC_HL_TITLE_3_U "\033""[38;2;255;128;128m""\033""[4m"
#define SVC_LOG_RENC_HL_NUM       "\033""[38;2;153;217;234m"
#define SVC_LOG_RENC_HL_STR       "\033""[38;2;255;174;201m"
#define SVC_LOG_RENC_HL_DEF_FC    "\033""[39m"
#define SVC_LOG_RENC_HL_END       "\033""[0m"
#define PRN_RENC_LOG         { SVC_WRAP_PRINT_s SvcRawEncPrint; AmbaSvcWrap_MisraMemset(&(SvcRawEncPrint), 0, sizeof(SvcRawEncPrint)); SvcRawEncPrint.Argc --; SvcRawEncPrint.pStrFmt =
#define PRN_RENC_ARG_UINT32  ; SvcRawEncPrint.Argc ++; SvcRawEncPrint.Argv[SvcRawEncPrint.Argc].Uint64   = (UINT64)((
#define PRN_RENC_ARG_CSTR    ; SvcRawEncPrint.Argc ++; SvcRawEncPrint.Argv[SvcRawEncPrint.Argc].pCStr    = ((
#define PRN_RENC_ARG_CPOINT  ; SvcRawEncPrint.Argc ++; SvcRawEncPrint.Argv[SvcRawEncPrint.Argc].pPointer = ((
#define PRN_RENC_ARG_POST    ))
#define PRN_RENC_OK          ; SvcRawEncPrint.Argc ++; SvcRawEnc_PrintLog(SVC_RAW_ENC_FLG_MSG_OK ,   &(SvcRawEncPrint));        }
#define PRN_RENC_NG          ; SvcRawEncPrint.Argc ++; SvcRawEnc_PrintLog(SVC_RAW_ENC_FLG_MSG_NG ,   &(SvcRawEncPrint));        }
#define PRN_RENC_API         ; SvcRawEncPrint.Argc ++; SvcRawEnc_PrintLog(SVC_RAW_ENC_FLG_MSG_API,   &(SvcRawEncPrint));        }
#define PRN_RENC_DBG         ; SvcRawEncPrint.Argc ++; SvcRawEnc_PrintLog(SVC_RAW_ENC_FLG_MSG_DBG,   &(SvcRawEncPrint));        }
#define PRN_RENC_SHELL_USAGE ; SvcRawEncPrint.Argc ++; SvcRawEnc_ShellLog(SVC_RAW_ENC_FLG_MSG_API,   &(SvcRawEncPrint), pFunc); }
#define PRN_RENC_SHELL_NG    ; SvcRawEncPrint.Argc ++; SvcRawEnc_ShellLog(SVC_RAW_ENC_FLG_MSG_NG,    &(SvcRawEncPrint), NULL);  }
#define PRN_RENC_ERR_HDLR    SvcRawEnc_ErrHdlr(__func__, __LINE__, PRetVal );

static void SvcRawEnc_PrintLog(UINT32 LogLevel, SVC_WRAP_PRINT_s *pPrint)
{
    if (pPrint != NULL) {
        if ((SVC_RawEncCtrlFlag & LogLevel) > 0U) {
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

            SvcWrap_Print(SVC_LOG_RENC, pPrint);
        }
    }
}

static UINT32 SvcRawEnc_MutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut)
{
    return AmbaKAL_MutexTake(pMutex, TimeOut);
}

static void SvcRawEnc_MutexGive(AMBA_KAL_MUTEX_t *pMutex)
{
    UINT32 ErrCode = AmbaKAL_MutexGive(pMutex);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }
}

static void SvcRawEnc_ErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode)
{
    if (pCaller != NULL) {
        if (ErrCode != 0U) {
            PRN_RENC_LOG "Catch ErrCode(0x%08x) @ %s, %d"
                PRN_RENC_ARG_UINT32 ErrCode  PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   pCaller  PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 CodeLine PRN_RENC_ARG_POST
            PRN_RENC_NG
        }
    }
}

static UINT32 SvcRawEnc_Align(UINT32 Align, UINT32 Size)
{
    UINT32 RetVal;

    if (Align > 1U) {
        if ((0xFFFFFFFFU - Size) < Align) {
            RetVal = Size;
        } else {
            RetVal = ( ( (UINT32)(Size) + (UINT32)(Align - 1U) ) & ( ~( (UINT32)(Align - 1U) ) ) );
        }
    } else {
        RetVal = Size;
    }

    return RetVal;
}



static void SvcRawEnc_StrmCapInit(void)
{
    if ((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_STRM_CAP_MEM_INIT) == 0U) {
        AmbaSvcWrap_MisraMemset(RawEncStrmCapFileName, 0, sizeof(RawEncStrmCapFileName));

        SVC_RawEncCtrlFlag |= SVC_RAW_ENC_FLG_STRM_CAP_MEM_INIT;

        PRN_RENC_LOG "Initial video raw enc stream capture memory" PRN_RENC_DBG
    }
}

void SvcRawEnc_SetStrmCapFileName(UINT32 StreamID, const char *pFileName, UINT32 Length)
{
    SvcRawEnc_StrmCapInit();

    if (StreamID >= AMBA_DSP_MAX_STREAM_NUM) {
        PRN_RENC_LOG "Fail to set stream cap filename - invalid StreamID(%d)" PRN_RENC_NG
    } else if (pFileName == NULL) {
        PRN_RENC_LOG "Fail to set stream cap filename - input filename should not null!" PRN_RENC_NG
    } else if (Length > (UINT32)sizeof(RawEncStrmCapFileName[StreamID])) {
        PRN_RENC_LOG "Fail to set stream cap filename - input filename length(%d) > buffer size(%d)!"
            PRN_RENC_ARG_UINT32 Length                                  PRN_RENC_ARG_POST
            PRN_RENC_ARG_UINT32 sizeof(RawEncStrmCapFileName[StreamID]) PRN_RENC_ARG_POST
        PRN_RENC_NG
    } else {
        SvcWrap_strcpy(RawEncStrmCapFileName[StreamID], sizeof(RawEncStrmCapFileName[StreamID]), pFileName);

        PRN_RENC_LOG "Configure StreamID(%d) filename - %s"
            PRN_RENC_ARG_UINT32 StreamID                        PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR   RawEncStrmCapFileName[StreamID] PRN_RENC_ARG_POST
        PRN_RENC_DBG
    }
}

UINT32 SvcRawEnc_GetStrmCapFileName(UINT32 StreamID, char *pFileName, UINT32 Length)
{
    UINT32 RetVal = SVC_OK;

    SvcRawEnc_StrmCapInit();

    if (StreamID >= AMBA_DSP_MAX_STREAM_NUM) {
        PRN_RENC_LOG "Fail to get stream cap filename - invalid StreamID(%d)" PRN_RENC_NG
    } else if (pFileName == NULL) {
        PRN_RENC_LOG "Fail to get stream cap filename - output filename buffer should not null!" PRN_RENC_NG
    } else if ((UINT32)SvcWrap_strlen(RawEncStrmCapFileName[StreamID]) > Length) {
        PRN_RENC_LOG "Fail to get stream cap filename - output filename lenght(%d) > buffer size(%d)!"
        PRN_RENC_ARG_UINT32 sizeof(RawEncStrmCapFileName[StreamID]) PRN_RENC_ARG_POST
            PRN_RENC_ARG_UINT32 Length                                  PRN_RENC_ARG_POST
        PRN_RENC_NG
    } else {
        SvcWrap_strcpy(pFileName, Length, RawEncStrmCapFileName[StreamID]);

        PRN_RENC_LOG "Output StreamID(%d) filename - %s"
            PRN_RENC_ARG_UINT32 StreamID  PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR   pFileName PRN_RENC_ARG_POST
        PRN_RENC_DBG
    }

    return RetVal;
}

static UINT32 SvcRawEnc_BufCtrlCreate(SVC_RAW_ENC_BUF_CTRL *pCtrl, UINT8 *pMemBuf, UINT32 MemSize, UINT32 VinID)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (pCtrl == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to create buf ctrl - input control should not null!" PRN_RENC_NG
    } else if ((pCtrl->Status & SVC_RAW_ENC_BUF_INIT) > 0U) {
        PRN_RENC_LOG "current buffer control has been created!" PRN_RENC_API
    } else if (pCtrl->RawBufSize == 0U) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to create buf ctrl - invalid control setting!" PRN_RENC_NG
    } else if (pMemBuf == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to create buf ctrl - input memory buffer should not null!" PRN_RENC_NG
    } else if (MemSize == 0U) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to create buf ctrl - input memory size should not zero!" PRN_RENC_NG
    } else {
        UINT32 ReqMemSize;
        ReqMemSize  = pCtrl->RawBufSize;
        ReqMemSize += pCtrl->HdsBufSize;
        ReqMemSize *= SVC_RAW_ENC_MAX_BUF_NUM;

        if (ReqMemSize > MemSize) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "Fail to create buf ctrl - input memory size cannot handle it!" PRN_RENC_NG
        } else {
            UINT32 Idx;
            UINT32 MemOffset = 0U;
            SVC_RAW_ENC_BUF_INFO_s BufInfo;

            pCtrl->VinID   = VinID;
            pCtrl->pMemBuf = pMemBuf;
            pCtrl->MemSize = MemSize;

            AmbaSvcWrap_MisraMemset((pCtrl->MutexName), 0, sizeof(pCtrl->MutexName));
            SvcWrap_strcpy(pCtrl->MutexName, sizeof(pCtrl->MutexName), "SvcRawEnc_BufCtrl_Mutex");

            /* Create mutex */
            AmbaSvcWrap_MisraMemset(&(pCtrl->Mutex), 0, sizeof(pCtrl->Mutex));
            if (0U != AmbaKAL_MutexCreate(&(pCtrl->Mutex), pCtrl->MutexName)) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "Fail to create buf ctrl - create mutex fail!" PRN_RENC_NG
            }

            /* Create data queue */
            if (RetVal == SVC_OK) {
                AmbaSvcWrap_MisraMemset((pCtrl->DataQueName), 0, sizeof(pCtrl->DataQueName));
                SvcWrap_strcpy(pCtrl->DataQueName, sizeof(pCtrl->DataQueName), "SvcRawEnc_DataQue");

                AmbaSvcWrap_MisraMemset((pCtrl->DataQueBuf), 0, sizeof(pCtrl->DataQueBuf));
                AmbaSvcWrap_MisraMemset(&(pCtrl->DataQue),    0, sizeof(pCtrl->DataQue));
                if (0U != AmbaKAL_MsgQueueCreate(&(pCtrl->DataQue),
                                                 pCtrl->DataQueName,
                                                 (UINT32)sizeof(pCtrl->DataQueBuf[0]),
                                                 pCtrl->DataQueBuf,
                                                 (UINT32)sizeof(pCtrl->DataQueBuf))) {
                    RetVal = SVC_NG;
                    PRN_RENC_LOG "Fail to create buf ctrl - create data queue fail!" PRN_RENC_NG
                } else {
                    PRetVal = AmbaKAL_MsgQueueFlush(&(pCtrl->DataQue));
                    if (PRetVal != 0U) {
                        PRN_RENC_LOG "Fail to create buf ctrl - flush queue fail! ErrCode(0x%08X)"
                            PRN_RENC_ARG_UINT32 PRetVal PRN_RENC_ARG_POST
                        PRN_RENC_NG
                    }
                }
            }

            /* Create free queue */
            if (RetVal == SVC_OK) {
                AmbaSvcWrap_MisraMemset((pCtrl->FreeQueName), 0, sizeof(pCtrl->FreeQueName));
                SvcWrap_strcpy(pCtrl->FreeQueName, sizeof(pCtrl->FreeQueName), "SvcRawEnc_FreeQue");

                AmbaSvcWrap_MisraMemset((pCtrl->FreeQueBuf), 0, sizeof(pCtrl->FreeQueBuf));
                AmbaSvcWrap_MisraMemset(&(pCtrl->FreeQue),    0, sizeof(pCtrl->FreeQue));
                if (0U != AmbaKAL_MsgQueueCreate(&(pCtrl->FreeQue),
                                                 pCtrl->FreeQueName,
                                                 (UINT32)sizeof(pCtrl->FreeQueBuf[0]),
                                                 pCtrl->FreeQueBuf,
                                                 (UINT32)sizeof(pCtrl->FreeQueBuf))) {
                    RetVal = SVC_NG;
                    PRN_RENC_LOG "Fail to create buf ctrl - create free queue fail!" PRN_RENC_NG
                } else {
                    PRetVal = AmbaKAL_MsgQueueFlush(&(pCtrl->FreeQue));
                    if (PRetVal != 0U) {
                        PRN_RENC_LOG "Fail to create buf ctrl - flush queue fail! ErrCode(0x%08X)"
                            PRN_RENC_ARG_UINT32 PRetVal PRN_RENC_ARG_POST
                        PRN_RENC_NG
                    }

                    for (Idx = 0U; Idx < SVC_RAW_ENC_MAX_BUF_NUM; Idx ++) {
                        AmbaSvcWrap_MisraMemset(&BufInfo, 0, sizeof(BufInfo));
                        BufInfo.pRawBuf    = &(pMemBuf[MemOffset]);
                        BufInfo.RawBufSize = pCtrl->RawBufSize;
                        if (pCtrl->HdsBufSize > 0U) {
                            BufInfo.pHdsBuf    = &(BufInfo.pRawBuf[BufInfo.RawBufSize]);
                            BufInfo.HdsBufSize = pCtrl->HdsBufSize;
                        }

                        PRetVal = AmbaKAL_MsgQueueSend(&(pCtrl->FreeQue), &BufInfo, AMBA_KAL_NO_WAIT);
                        if (PRetVal != 0U) {
                            PRN_RENC_LOG "Fail to create buf ctrl - send queue fail! ErrCode(0x%08X)"
                                PRN_RENC_ARG_UINT32 PRetVal PRN_RENC_ARG_POST
                            PRN_RENC_NG
                        } else {
                            PRN_RENC_LOG "Send address %p/%p to free queue!"
                                PRN_RENC_ARG_CPOINT BufInfo.pRawBuf PRN_RENC_ARG_POST
                                PRN_RENC_ARG_CPOINT BufInfo.pHdsBuf PRN_RENC_ARG_POST
                            PRN_RENC_API
                        }

                        MemOffset += BufInfo.RawBufSize;
                        if (pCtrl->HdsBufSize > 0U) {
                            MemOffset += BufInfo.HdsBufSize;
                        }
                    }
                }
            }
        }

        if (RetVal == SVC_OK) {
            pCtrl->Status = SVC_RAW_ENC_BUF_INIT | SVC_RAW_ENC_BUF_READY;
        }
    }

    return RetVal;
}

static UINT32 SvcRawEnc_BufCtrlDelete(SVC_RAW_ENC_BUF_CTRL *pCtrl)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (pCtrl == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to delete buf ctrl - invalid buffer control" PRN_RENC_NG
    } else {
        if ((pCtrl->Status & SVC_RAW_ENC_BUF_INIT) != 0U) {
            AMBA_KAL_MSG_QUEUE_INFO_s QueInfo;
            UINT32 TimeOut = 1000U;

            /* Wait the queue idle */
            do {
                AmbaSvcWrap_MisraMemset(&(QueInfo), 0, sizeof(QueInfo));
                RetVal = AmbaKAL_MsgQueueQuery(&(pCtrl->DataQue), &QueInfo);
                if (0U != RetVal) {
                    if ((QueInfo.NumEnqueued == 0U) || ((TimeOut - 10U) == 0U)) {
                        break;
                    }
                }
                TimeOut -= 10U;
                PRetVal = AmbaKAL_TaskSleep(10U); PRN_RENC_ERR_HDLR
            } while (RetVal == 0U);

            if (0U != SvcRawEnc_MutexTake(&(pCtrl->Mutex), 1000U)) {
                PRN_RENC_LOG "Fail to delete buf ctrl - take mutex timeout!" PRN_RENC_NG
            }

            /* Delete data queue */
            PRetVal = AmbaKAL_MsgQueueDelete(&(pCtrl->FreeQue));
            if (PRetVal != 0U) {
                PRN_RENC_LOG "Fail to delete buf ctrl - delete data queue fail! ErrCode(0x%08X)"
                    PRN_RENC_ARG_UINT32 PRetVal PRN_RENC_ARG_POST
                PRN_RENC_NG
            }
            AmbaSvcWrap_MisraMemset(&(pCtrl->FreeQue), 0, sizeof(pCtrl->FreeQue));

            /* Delete free queue */
            PRetVal = AmbaKAL_MsgQueueDelete(&(pCtrl->DataQue));
            if (PRetVal != 0U) {
                PRN_RENC_LOG "Fail to delete buf ctrl - delete free queue fail! ErrCode(0x%08X)"
                    PRN_RENC_ARG_UINT32 PRetVal PRN_RENC_ARG_POST
                PRN_RENC_NG
            }
            AmbaSvcWrap_MisraMemset(&(pCtrl->DataQue), 0, sizeof(pCtrl->DataQue));

            SvcRawEnc_MutexGive(&(pCtrl->Mutex));
            PRetVal = AmbaKAL_MutexDelete(&(pCtrl->Mutex)); PRN_RENC_ERR_HDLR

            AmbaSvcWrap_MisraMemset(pCtrl, 0, sizeof(SVC_RAW_ENC_BUF_CTRL));
        }
    }

    return RetVal;
}

static UINT32 SvcRawEnc_BufCtrlGetBuf(SVC_RAW_ENC_BUF_CTRL *pCtrl, SVC_RAW_ENC_BUF_INFO_s *pBuf)
{
    UINT32 RetVal = SVC_OK;

    if (pCtrl == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to get raw enc buf info - input controller should not null!" PRN_RENC_NG
    } else if ((pCtrl->Status & SVC_RAW_ENC_BUF_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to get raw enc buf info - create the buffer controller first!" PRN_RENC_NG
    } else if (pBuf == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to get raw enc buf info - output buffer info should not null!" PRN_RENC_NG
    } else if (0U != SvcRawEnc_MutexTake(&(pCtrl->Mutex), SVC_RAW_ENC_MAX_MUTX_TIMEOUT)) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to get raw enc buf info - take mutex timeout!" PRN_RENC_NG
    } else {
        pCtrl->Status |= SVC_RAW_ENC_BUF_BUSY;
        pCtrl->Status &= ~SVC_RAW_ENC_BUF_READY;

        /* Get free buffer */
        AmbaSvcWrap_MisraMemset(pBuf, 0, sizeof(SVC_RAW_ENC_BUF_INFO_s));
        RetVal = AmbaKAL_MsgQueueReceive(&(pCtrl->FreeQue), pBuf, 10000U);
        if (RetVal != SVC_OK) {
            PRN_RENC_LOG "Fail to get raw enc buf info - get free buffer fail!" PRN_RENC_NG
        } else {
            pBuf->Status = SVC_RAW_ENC_BUF_FREE;
        }

        pCtrl->Status |= SVC_RAW_ENC_BUF_READY;
        pCtrl->Status &= ~SVC_RAW_ENC_BUF_BUSY;

        SvcRawEnc_MutexGive(&(pCtrl->Mutex));
    }

    return RetVal;
}

static UINT32 SvcRawEnc_BufCtrlLockBuf(SVC_RAW_ENC_BUF_CTRL *pCtrl, SVC_RAW_ENC_BUF_INFO_s *pBuf)
{
    UINT32 RetVal = SVC_OK;

    if (pCtrl == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to lock vid raw enc buffer - input controller should not null!" PRN_RENC_NG
    } else if ((pCtrl->Status & SVC_RAW_ENC_BUF_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to lock vid raw enc buffer - create the buffer controller first!" PRN_RENC_NG
    } else if (pBuf == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to lock vid raw enc buffer - output buffer info should not null!" PRN_RENC_NG
    } else if (0U != SvcRawEnc_MutexTake(&(pCtrl->Mutex), SVC_RAW_ENC_MAX_MUTX_TIMEOUT)) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to lock vid raw enc buffer - take mutex timeout!" PRN_RENC_NG
    } else {
        pCtrl->Status |= SVC_RAW_ENC_BUF_BUSY;
        pCtrl->Status &= ~SVC_RAW_ENC_BUF_READY;

        /* lock buffer */
        RetVal = AmbaKAL_MsgQueueSend(&(pCtrl->DataQue), pBuf, AMBA_KAL_NO_WAIT);
        if (RetVal != SVC_OK) {
            PRN_RENC_LOG "Fail to lock vid raw enc buffer - get free buffer fail!" PRN_RENC_NG
        }

        pCtrl->Status |= SVC_RAW_ENC_BUF_READY;
        pCtrl->Status &= ~SVC_RAW_ENC_BUF_BUSY;

        SvcRawEnc_MutexGive(&(pCtrl->Mutex));
    }

    return RetVal;
}

static UINT32 SvcRawEnc_BufCtrlFreeBuf(SVC_RAW_ENC_BUF_CTRL *pCtrl, SVC_RAW_ENC_BUF_INFO_s *pBuf)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (pCtrl == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to free vid raw enc buffer - input controller should not null!" PRN_RENC_NG
    } else if ((pCtrl->Status & SVC_RAW_ENC_BUF_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to free vid raw enc buffer - create the buffer controller first!" PRN_RENC_NG
    } else if (0U != SvcRawEnc_MutexTake(&(pCtrl->Mutex), SVC_RAW_ENC_MAX_MUTX_TIMEOUT)) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to free vid raw enc buffer - take mutex timeout!" PRN_RENC_NG
    } else {
        pCtrl->Status |= SVC_RAW_ENC_BUF_BUSY;
        pCtrl->Status &= ~SVC_RAW_ENC_BUF_READY;

        if (pBuf != NULL) {
            /* Free the buffer */
            PRetVal = AmbaKAL_MsgQueueSend(&(pCtrl->FreeQue), pBuf, AMBA_KAL_NO_WAIT); PRN_RENC_ERR_HDLR

            PRN_RENC_LOG "Successful to free vid raw enc buffer! %p"
                PRN_RENC_ARG_CPOINT pBuf->pRawBuf PRN_RENC_ARG_POST
            PRN_RENC_DBG
        } else {
            SVC_RAW_ENC_BUF_INFO_s LockedBuf;

            /* Get locked buffer */
            AmbaSvcWrap_MisraMemset(&LockedBuf, 0, sizeof(LockedBuf));
            RetVal = AmbaKAL_MsgQueueReceive(&(pCtrl->DataQue), &LockedBuf, 10000U);
            if (RetVal != SVC_OK) {
                PRN_RENC_LOG "Fail to free vid raw enc buffer - get locked buffer fail!" PRN_RENC_NG
            } else {
                /* Free the buffer */
                PRetVal = AmbaKAL_MsgQueueSend(&(pCtrl->FreeQue), &LockedBuf, AMBA_KAL_NO_WAIT); PRN_RENC_ERR_HDLR

                PRN_RENC_LOG "Successful to free vid raw enc buffer! %p"
                    PRN_RENC_ARG_CPOINT LockedBuf.pRawBuf PRN_RENC_ARG_POST
                PRN_RENC_DBG
            }
        }

        pCtrl->Status |= SVC_RAW_ENC_BUF_READY;
        pCtrl->Status &= ~SVC_RAW_ENC_BUF_BUSY;

        SvcRawEnc_MutexGive(&(pCtrl->Mutex));
    }

    return RetVal;
}

static void SvcRawEnc_BufCtrlInfo(SVC_RAW_ENC_BUF_CTRL *pCtrl)
{
    if ((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_INIT) > 0U) {
        if (pCtrl != NULL) {
            AMBA_KAL_MSG_QUEUE_INFO_s QueInfo;

            PRN_RENC_LOG " " PRN_RENC_API
            PRN_RENC_LOG "====== %sRaw Enc Buf Ctrl (%p)%s ======"
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_TITLE_1 PRN_RENC_ARG_POST
                PRN_RENC_ARG_CPOINT pCtrl                   PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END     PRN_RENC_ARG_POST
            PRN_RENC_API
            PRN_RENC_LOG "  Status     : 0x%x"
                PRN_RENC_ARG_UINT32 pCtrl->Status PRN_RENC_ARG_POST
            PRN_RENC_API
            PRN_RENC_LOG "  VinID      : %d"
                PRN_RENC_ARG_UINT32 pCtrl->VinID PRN_RENC_ARG_POST
            PRN_RENC_API
            PRN_RENC_LOG "  RawBufSize : 0x%08x"
                PRN_RENC_ARG_UINT32 pCtrl->RawBufSize PRN_RENC_ARG_POST
            PRN_RENC_API
            PRN_RENC_LOG "  HdsBufSize : 0x%08x"
                PRN_RENC_ARG_UINT32 pCtrl->HdsBufSize PRN_RENC_ARG_POST
            PRN_RENC_API
            PRN_RENC_LOG "  MemBase    : %p"
                PRN_RENC_ARG_CPOINT pCtrl->pMemBuf PRN_RENC_ARG_POST
            PRN_RENC_API
            PRN_RENC_LOG "  MemSize    : 0x%08x"
                PRN_RENC_ARG_UINT32 pCtrl->MemSize PRN_RENC_ARG_POST
            PRN_RENC_API

#ifndef CONFIG_BUILD_QNX_IPL
            PRN_RENC_LOG "%s  Que Name | ID         BufAddr    BufSize    NumEnqueued NumAvailable%s"
#else
            PRN_RENC_LOG "%s  Que Name | ID                 BufAddr            BufSize    NumEnqueued NumAvailable%s"
#endif
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_TITLE_2 PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END     PRN_RENC_ARG_POST
            PRN_RENC_API
            AmbaSvcWrap_MisraMemset(&QueInfo, 0, sizeof(QueInfo));
            if (0U == AmbaKAL_MsgQueueQuery(&(pCtrl->FreeQue), &QueInfo)) {
                PRN_RENC_LOG "  FREE QUE | %p %p 0x%08x %04d        %04d"
                    PRN_RENC_ARG_CPOINT &(pCtrl->FreeQue)                 PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CPOINT pCtrl->FreeQueBuf                 PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 (UINT32)sizeof(pCtrl->FreeQueBuf) PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 QueInfo.NumEnqueued               PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 QueInfo.NumAvailable              PRN_RENC_ARG_POST
                PRN_RENC_API
            }
            AmbaSvcWrap_MisraMemset(&QueInfo, 0, sizeof(QueInfo));
            if (0U == AmbaKAL_MsgQueueQuery(&(pCtrl->DataQue), &QueInfo)) {
                PRN_RENC_LOG "  DATA QUE | %p %p 0x%08x %04d        %04d"
                    PRN_RENC_ARG_CPOINT &(pCtrl->DataQue)                 PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CPOINT pCtrl->DataQueBuf                 PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 (UINT32)sizeof(pCtrl->DataQueBuf) PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 QueInfo.NumEnqueued               PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 QueInfo.NumAvailable              PRN_RENC_ARG_POST
                PRN_RENC_API
            }
        }
    }
}

static void SvcRawEnc_DefResCfgInit(void)
{
    if ((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_DEF_RES_CFG) == 0U) {
        const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
        UINT32 VinIDs[AMBA_DSP_MAX_VIN_NUM], VinNum = 0U;

        SvcRawEnc_VoutDevInit();

        AmbaSvcWrap_MisraMemset(VinIDs, 0, sizeof(VinIDs));

        if (pResCfg == NULL) {
            PRN_RENC_LOG "Fail to initial video raw enc res_cfg - invalid current res_cfg fail!" PRN_RENC_NG
        } else if (0U != SvcResCfg_GetVinIDs(VinIDs, &VinNum)) {
            PRN_RENC_LOG "Fail to initial video raw enc res_cfg - get VinID fail!" PRN_RENC_NG
        } else {
            AmbaSvcWrap_MisraMemset(&RawEncResCfg, 0, sizeof(RawEncResCfg));
            RawEncResCfgVinID = VinIDs[0];

            AmbaSvcWrap_MisraMemcpy(RawEncResCfg.GroupName,  pResCfg->GroupName, sizeof(RawEncResCfg.GroupName));
            AmbaSvcWrap_MisraMemcpy(RawEncResCfg.FormatName, pResCfg->FormatName, sizeof(RawEncResCfg.FormatName));
            AmbaSvcWrap_MisraMemcpy(RawEncResCfg.VinTree,    pResCfg->VinTree, sizeof(RawEncResCfg.VinTree));

            RawEncResCfg.VinNum = pResCfg->VinNum;
            AmbaSvcWrap_MisraMemcpy(RawEncResCfg.VinCfg, pResCfg->VinCfg, sizeof(RawEncResCfg.VinCfg));

            AmbaSvcWrap_MisraMemcpy(RawEncResCfg.SensorCfg, pResCfg->SensorCfg, sizeof(RawEncResCfg.SensorCfg));

            RawEncResCfg.FovNum = pResCfg->FovNum;
            AmbaSvcWrap_MisraMemcpy(RawEncResCfg.FovCfg, pResCfg->FovCfg, sizeof(RawEncResCfg.FovCfg));

            RawEncResCfg.DispNum = pResCfg->DispNum;
            RawEncResCfg.DispBits = pResCfg->DispBits;
            AmbaSvcWrap_MisraMemcpy(RawEncResCfg.DispStrm, pResCfg->DispStrm, sizeof(RawEncResCfg.DispStrm));

            RawEncResCfg.RecNum = pResCfg->RecNum;
            RawEncResCfg.RecBits = pResCfg->RecBits;
            AmbaSvcWrap_MisraMemcpy(RawEncResCfg.RecStrm, pResCfg->RecStrm, sizeof(RawEncResCfg.RecStrm));

            SVC_RawEncCtrlFlag |= SVC_RAW_ENC_FLG_DEF_RES_CFG;

            if ((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_DEF_DMY_SENSOR) > 0U) {
                UINT32 VinIdx;
                for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
                    RawEncResCfg.VinCfg[VinIdx].pDriver = &RawEncDmySenObj;
                }

                PRN_RENC_LOG "Configure video raw enc dummy sensor object" PRN_RENC_API
            }
        }
    }
}

static UINT32 SvcRawEnc_DspEvtRawDataRdy(const void *pEventData)
{
    AMBA_DSP_RAW_DATA_RDY_s *pRawRdy = NULL;
    AmbaMisra_TypeCast(&(pRawRdy), &(pEventData));

    if (pRawRdy == NULL) {
        PRN_RENC_LOG "Fail to proc raw data rdy - raw data ready msg should not null!" PRN_RENC_NG
    } else if (pRawRdy->VinId >= AMBA_DSP_MAX_VIN_NUM) {
        PRN_RENC_LOG "Fail to proc raw data rdy - invalid VinID(%d)"
            PRN_RENC_ARG_UINT32 pRawRdy->VinId PRN_RENC_ARG_POST
        PRN_RENC_NG
    } else {
        UINT32 PRetVal, VinID = (UINT32)(pRawRdy->VinId);
        PRN_RENC_LOG "VinID(%d) Raw Data Ready"
            PRN_RENC_ARG_UINT32 VinID PRN_RENC_ARG_POST
        PRN_RENC_DBG

        PRetVal = AmbaKAL_EventFlagSet(&(SVC_RawEncCtrl.EventFlag), (UINT32)(SVC_RAW_ENC_EVT_FLG_RAW_RDY << VinID));
        if (PRetVal != 0U) {
            PRN_RENC_LOG "Fail to set VinID(%d) raw data ready flag! ErrCode(0x%08X)"
                PRN_RENC_ARG_UINT32 pRawRdy->VinId PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 PRetVal PRN_RENC_ARG_POST
            PRN_RENC_NG
        }
    }

    AmbaMisra_TouchUnused(pRawRdy);

    return 0U;
}

static UINT32 SvcRawEnc_DspEvtLivYuvRdy(const void *pEventData)
{
    AMBA_DSP_YUV_DATA_RDY_s *pYuvRdy = NULL;
    AmbaMisra_TypeCast(&(pYuvRdy), &(pEventData));

    if (pYuvRdy != NULL) {
        UINT32 StrmID = ( pYuvRdy->ViewZoneId & 0x0000000FU );
        if ((pYuvRdy->ViewZoneId & 0x80000000U) > 0U) {     //  video raw enc main stream
            UINT32 PRetVal, VinIdx, ExitLoop = 0U;
            UINT32 FovIdxs[AMBA_DSP_MAX_VIEWZONE_NUM], FovNum = 0;
            AMBA_KAL_EVENT_FLAG_INFO_s CurFlagInfo;

            if (StrmID >= RawEncResCfg.DispNum) {
                StrmID -= RawEncResCfg.DispNum;

                if (StrmID < AMBA_DSP_MAX_STREAM_NUM) {
                    AmbaSvcWrap_MisraMemset(&(SVC_RawEncCtrl.EncStrmInfo[StrmID]), 0, sizeof(SVC_RAW_ENC_STRM_INFO_s));
                    SVC_RawEncCtrl.EncStrmInfo[StrmID].Pitch  = (UINT32)(pYuvRdy->Buffer.Pitch);         SVC_RawEncCtrl.EncStrmInfo[StrmID].Pitch  &= 0xFFFFU;
                    SVC_RawEncCtrl.EncStrmInfo[StrmID].Width  = (UINT32)(pYuvRdy->Buffer.Window.Width);  SVC_RawEncCtrl.EncStrmInfo[StrmID].Width  &= 0xFFFFU;
                    SVC_RawEncCtrl.EncStrmInfo[StrmID].Height = (UINT32)(pYuvRdy->Buffer.Window.Height); SVC_RawEncCtrl.EncStrmInfo[StrmID].Height &= 0xFFFFU;
                    AmbaMisra_TypeCast(&(SVC_RawEncCtrl.EncStrmInfo[StrmID].pYBuf),  &(pYuvRdy->Buffer.BaseAddrY));
                    AmbaMisra_TypeCast(&(SVC_RawEncCtrl.EncStrmInfo[StrmID].pUVBuf), &(pYuvRdy->Buffer.BaseAddrUV));
                    SVC_RawEncCtrl.EncStrmBits |= SvcRawEnc_BitGet(StrmID);

                    PRN_RENC_LOG "StrmID(%d), YUV %04d/%04d/%04d, YBuf(%p) UVBuf(%p)"
                        PRN_RENC_ARG_UINT32 StrmID                                    PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 SVC_RawEncCtrl.EncStrmInfo[StrmID].Pitch  PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 SVC_RawEncCtrl.EncStrmInfo[StrmID].Width  PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 SVC_RawEncCtrl.EncStrmInfo[StrmID].Height PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CPOINT SVC_RawEncCtrl.EncStrmInfo[StrmID].pYBuf  PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CPOINT SVC_RawEncCtrl.EncStrmInfo[StrmID].pUVBuf PRN_RENC_ARG_POST
                    PRN_RENC_DBG
                }

                AmbaSvcWrap_MisraMemset(&CurFlagInfo, 0, sizeof(CurFlagInfo));
                PRetVal = AmbaKAL_EventFlagQuery(&(SVC_RawEncCtrl.EventFlag), &CurFlagInfo); PRN_RENC_ERR_HDLR

                for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
                    if ((SVC_RawEncCtrl.VinSelectBits & SvcRawEnc_BitGet(VinIdx)) > 0U) {
                        if ((CurFlagInfo.CurrentFlags & SvcRawEnc_BitGet(VinIdx)) == 0U) {
                            if (SVC_RawEncCtrl.LivInfo.pStrmCfg == NULL) {
                                ExitLoop = 1U;
                                PRN_RENC_LOG "Invalid liveview stream configuration" PRN_RENC_NG
                            } else if (SVC_RawEncCtrl.LivInfo.pStrmCfg[StrmID].pChanCfg == NULL) {
                                ExitLoop = 1U;
                                PRN_RENC_LOG "Invalid liveview stream channel configuration!" PRN_RENC_NG
                            } else {
                                FovNum = 0U;
                                AmbaSvcWrap_MisraMemset(FovIdxs, 0, sizeof(FovIdxs));
                                if (0U != SvcResCfg_GetFovIdxsInVinID(VinIdx, FovIdxs, &FovNum)) {
                                    ExitLoop = 1U;
                                    PRN_RENC_LOG "Fail to get VinID(%d) Fov Info"
                                        PRN_RENC_ARG_UINT32 VinIdx PRN_RENC_ARG_POST
                                    PRN_RENC_NG
                                } else {
                                    if (FovNum > 0U) {

                                        if ((UINT16)(FovIdxs[0]) == SVC_RawEncCtrl.LivInfo.pStrmCfg[StrmID].pChanCfg[0].ViewZoneId) {

                                            PRetVal = AmbaKAL_EventFlagSet(&(SVC_RawEncCtrl.EventFlag), (UINT32)(SVC_RAW_ENC_EVT_FLG_RAW_RDY << (VinIdx)));
                                            if (PRetVal != 0U) {
                                                PRN_RENC_LOG "Fail to set VinID(%d) raw data rdy flag! ErrCode(0x%08x)"
                                                    PRN_RENC_ARG_UINT32 VinIdx PRN_RENC_ARG_POST
                                                PRN_RENC_NG
                                            }

                                            PRetVal = AmbaKAL_EventFlagSet(&(SVC_RawEncCtrl.EventFlag), (UINT32)(SVC_RAW_ENC_EVT_FLG_YUV_RDY << VinIdx));
                                            if (PRetVal != 0U) {
                                                PRN_RENC_LOG "Fail to set VinID(%d) yuv data rdy flag! ErrCode(0x%08x)"
                                                    PRN_RENC_ARG_UINT32 VinIdx PRN_RENC_ARG_POST
                                                PRN_RENC_NG
                                            }

                                            PRN_RENC_LOG "FovID(%d) YUV Data Ready"
                                                PRN_RENC_ARG_UINT32 FovIdxs[0] PRN_RENC_ARG_POST
                                            PRN_RENC_DBG

                                            ExitLoop = 1U;
                                        }
                                    }
                                }
                            }
                        }
                    }

                    if (ExitLoop != 0U) {
                        break;
                    }
                }
            }
        }
    }
    AmbaMisra_TouchUnused(pYuvRdy);

    return 0U;
}

static UINT32 SvcRawEnc_MemoryQuery(const SVC_RES_CFG_s *pCfg, UINT32 *pRawMemSize, UINT32 *pHdsMemSize)
{
    UINT32 RetVal = SVC_NG;

    if (pCfg != NULL) {

        UINT32 VinIdx;
        UINT32 RawMemSize = 0U, HdsMemSize = 0U;
        UINT32 CurRawMemSize, CurHdsMemSize;
        UINT32 Pitch, Width, Height;
        UINT32 HdsPitch, HdsWidth, HdsHeight;

        UINT32 FovIdxs[AMBA_DSP_MAX_VIEWZONE_NUM], FovNum = 0;

        PRN_RENC_LOG " " PRN_RENC_DBG
        PRN_RENC_LOG "====== Memory Query Info ======" PRN_RENC_DBG
        PRN_RENC_LOG "  VinNum : %d"
            PRN_RENC_ARG_UINT32 pCfg->VinNum PRN_RENC_ARG_POST
        PRN_RENC_DBG
        PRN_RENC_LOG "          | VIN                           | HDS ( CE Buffer )" PRN_RENC_DBG
        PRN_RENC_LOG "    VinID | BufSize    Pitch Width Height | BufSize    Pitch Width Height" PRN_RENC_DBG
        PRN_RENC_LOG "    -----------------------------------------------------------------------" PRN_RENC_DBG

        for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
            if ((pCfg->VinCfg[VinIdx].CapWin.Width > 0U) &&
                (pCfg->VinCfg[VinIdx].CapWin.Height > 0U)) {
                Width  = (UINT32)(pCfg->VinCfg[VinIdx].CapWin.Width ); Width  &= 0x0000FFFFU;
                Height = (UINT32)(pCfg->VinCfg[VinIdx].CapWin.Height); Height &= 0x0000FFFFU;
                Pitch  =  SvcRawEnc_Align(SVC_RAW_ENC_RAW_PITCH_ALIGN, Width << 1U);

                FovNum = 0U;
                AmbaSvcWrap_MisraMemset(FovIdxs, 0, sizeof(FovIdxs));
                RetVal = SvcResCfg_GetFovIdxsInVinID(VinIdx, FovIdxs, &FovNum);
                if (RetVal == SVC_OK) {
                    if (FovNum == 0U) {
                        RetVal = SVC_NG;
                        PRN_RENC_LOG "Fail to query memory - get invalid VinID(%d) fov setting!"
                            PRN_RENC_ARG_UINT32 VinIdx PRN_RENC_ARG_POST
                        PRN_RENC_NG
                    }
                }

                if (RetVal == SVC_OK) {
                    if ((pCfg->FovCfg[FovIdxs[0]].PipeCfg.HdrExposureNum > 1U) || 
                       (((pCfg->FovCfg[FovIdxs[0]].PipeCfg.LinearCE & LINEAR_CE_FORCE_DISABLE) == 0U) && ((pCfg->FovCfg[FovIdxs[0]].PipeCfg.LinearCE & LINEAR_CE_ENABLE) > 0U))) {
                        HdsWidth  = Width / 4U;
                        HdsHeight = Height;
                        HdsPitch  = SvcRawEnc_Align(SVC_RAW_ENC_RAW_PITCH_ALIGN, HdsWidth << 1U);
                    } else {
                        HdsWidth  = 0U;
                        HdsHeight = 0U;
                        HdsPitch  = 0U;
                    }

                    CurRawMemSize = Pitch * Height;
                    CurHdsMemSize = HdsPitch * HdsHeight;
                    PRN_RENC_LOG "    %d     | 0x%08x %4d  %4d  %4d   | 0x%08x %4d  %4d  %4d"
                        PRN_RENC_ARG_UINT32 VinIdx        PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 CurRawMemSize PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 Pitch         PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 Width         PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 Height        PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 CurHdsMemSize PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 HdsPitch      PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 HdsWidth      PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 HdsHeight     PRN_RENC_ARG_POST
                    PRN_RENC_DBG

                    RawMemSize += ( Pitch * Height );
                    HdsMemSize += ( HdsPitch * HdsHeight );
                } else {
                    break;
                }
            }
        }

        if (RetVal == SVC_OK) {
            UINT32 TotalRawBuf = RawMemSize * SVC_RAW_ENC_MAX_BUF_NUM;
            UINT32 TotalHdsBuf = HdsMemSize * SVC_RAW_ENC_MAX_BUF_NUM;
            PRN_RENC_LOG "    -----------------------------------------------------------------------" PRN_RENC_DBG
            PRN_RENC_LOG "    Total | 0x%08x x %2d = 0x%08x  | 0x%08x x %2d = 0x%08x "
                PRN_RENC_ARG_UINT32 RawMemSize               PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 SVC_RAW_ENC_MAX_BUF_NUM  PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 TotalRawBuf              PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 HdsMemSize               PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 SVC_RAW_ENC_MAX_BUF_NUM  PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 TotalHdsBuf              PRN_RENC_ARG_POST
            PRN_RENC_DBG

            if (pRawMemSize != NULL) {
                *pRawMemSize = RawMemSize * SVC_RAW_ENC_MAX_BUF_NUM;
            }

            if (pHdsMemSize != NULL) {
                *pHdsMemSize = HdsMemSize * SVC_RAW_ENC_MAX_BUF_NUM;
            }

            RetVal = SVC_OK;
        }
    }

    return RetVal;
}

static UINT32 SvcRawEnc_MemoryConfig(UINT8 *pMemBuf, UINT32 MemSize)
{
    UINT32 RetVal = SVC_OK;
    UINT32 VinIDs[AMBA_DSP_MAX_VIN_NUM], VinNum = 0U, VinIdx;
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();

    AmbaSvcWrap_MisraMemset(VinIDs, 255, sizeof(VinIDs));

    AmbaMisra_TouchUnused(&MemSize);

    if (pCfg == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to create raw encode - invalid resolution config!" PRN_RENC_NG
    } else if (SVC_OK != SvcResCfg_GetVinIDs(VinIDs, &VinNum)) { /* Get the vin info */
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to create raw encode - get vin info fail!" PRN_RENC_NG
    } else if (VinNum == 0U) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to create raw encode - vin number should not zero" PRN_RENC_NG
    } else {
        UINT32 Pitch, Width, Height;
        UINT32 MemOffset = 0U, RemainSize = SVC_RawEncCtrl.MemSize;
        UINT32 ReqMemSize = 0U;

        for (VinIdx = 0U; VinIdx < VinNum; VinIdx ++) {
            if (VinIDs[VinIdx] >= AMBA_DSP_MAX_VIN_NUM) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "Fail to create raw encode - invalid VinID(%d), %d"
                    PRN_RENC_ARG_UINT32 VinIDs[VinIdx] PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 VinIdx         PRN_RENC_ARG_POST
                PRN_RENC_NG
            } else {
                /* Configure VinSelectBits */
                SVC_RawEncCtrl.VinSelectBits |= SvcRawEnc_BitGet(VinIDs[VinIdx]);

                /* Configure the BufCtrl */
                Width  = (UINT32)(pCfg->VinCfg[VinIDs[VinIdx]].CapWin.Width);  Width  &= 0x0000FFFFU;
                Height = (UINT32)(pCfg->VinCfg[VinIDs[VinIdx]].CapWin.Height); Height &= 0x0000FFFFU;
                Pitch = SvcRawEnc_Align(SVC_RAW_ENC_RAW_PITCH_ALIGN, Width << 1U);
                SVC_RawEncCtrl.BufCtrl[VinIDs[VinIdx]].RawBufSize = Pitch * Height;
                if ((pCfg->FovCfg[0].PipeCfg.HdrExposureNum > 1U) || (((pCfg->FovCfg[0].PipeCfg.LinearCE & LINEAR_CE_FORCE_DISABLE) == 0U) && ((pCfg->FovCfg[0].PipeCfg.LinearCE & LINEAR_CE_ENABLE) > 0U))) {
                    Pitch = SvcRawEnc_Align(SVC_RAW_ENC_RAW_PITCH_ALIGN, Width >> 1U);   // HdsPitch = Align64( ( HdsWidth / 4U ) * 2U )
                    SVC_RawEncCtrl.BufCtrl[VinIDs[VinIdx]].HdsBufSize = Pitch * Height;
                }

                ReqMemSize =  SVC_RawEncCtrl.BufCtrl[VinIDs[VinIdx]].RawBufSize;
                ReqMemSize += SVC_RawEncCtrl.BufCtrl[VinIDs[VinIdx]].HdsBufSize;
                ReqMemSize *= SVC_RAW_ENC_MAX_BUF_NUM;

                if (RemainSize >= ReqMemSize) {
                    RetVal = SvcRawEnc_BufCtrlCreate(&(SVC_RawEncCtrl.BufCtrl[VinIDs[VinIdx]]),
                                                     &(pMemBuf[MemOffset]),
                                                     ReqMemSize,
                                                     VinIDs[VinIdx]);
                    if (RetVal != SVC_OK) {
                        PRN_RENC_LOG "Fail to create raw encode - configure VinID(%d) buf ctrl fail!"
                            PRN_RENC_ARG_UINT32 VinIDs[VinIdx] PRN_RENC_ARG_POST
                        PRN_RENC_NG
                    } else {

                        PRN_RENC_LOG "success to create VinID(%d) video raw enc buf ctrl."
                            PRN_RENC_ARG_UINT32 VinIDs[VinIdx] PRN_RENC_ARG_POST
                        PRN_RENC_API

                        MemOffset  += ReqMemSize;
                        RemainSize -= ReqMemSize;
                    }
                } else {
                    RetVal = SVC_NG;
                    PRN_RENC_LOG "Fail to create raw encode - the memory cannot cfg VinID(%d) buf ctrl"
                        PRN_RENC_ARG_UINT32 VinIDs[VinIdx] PRN_RENC_ARG_POST
                    PRN_RENC_NG
                }
            }

            if (RetVal != SVC_OK) {
                break;
            }
        }
    }

    return RetVal;
}

static UINT32 SvcRawEnc_RawDataCfg(UINT32 NumCfg, UINT32 *pVinIDArr, char **pRawFilePath)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to cfg raw setting - create raw encode module first!" PRN_RENC_NG
    } else if (pVinIDArr == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to cfg raw setting - vin id should not null!" PRN_RENC_NG
        AmbaMisra_TouchUnused(pVinIDArr);
    } else if (pRawFilePath == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to cfg raw setting - input hds file path should not null!" PRN_RENC_NG
        AmbaMisra_TouchUnused(pRawFilePath);
    } else if (0U != SvcRawEnc_MutexTake(&(SVC_RawEncCtrl.Mutex), SVC_RAW_ENC_MAX_MUTX_TIMEOUT)) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to cfg raw setting - take mutex timeout!" PRN_RENC_NG
    } else {
        UINT32 Idx;
        SVC_RAW_ENC_VIN_CFG_s *pCurCfg;

        for (Idx = 0U; Idx < NumCfg; Idx ++) {
            pCurCfg = &(SVC_RawEncCtrl.VinCfg[pVinIDArr[Idx]]);
            AmbaSvcWrap_MisraMemset(pCurCfg->RawFilePath, 0, sizeof(pCurCfg->RawFilePath));
            SvcWrap_strcpy(pCurCfg->RawFilePath, sizeof(pCurCfg->RawFilePath), pRawFilePath[Idx]);

            pCurCfg->Status |= SVC_RAW_ENC_RAW_UPDATE;
        }

        SvcRawEnc_MutexGive(&(SVC_RawEncCtrl.Mutex));
    }

    return RetVal;
}

static UINT32 SvcRawEnc_HdsDataCfg(UINT32 NumCfg, UINT32 *pVinIDArr, char **pHdsFilePath)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to cfg hds setting - create raw encode module first!" PRN_RENC_NG
    } else if (pVinIDArr == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to cfg hds setting - vin id should not null!" PRN_RENC_NG
        AmbaMisra_TouchUnused(pVinIDArr);
    } else if (pHdsFilePath == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to cfg hds setting - input hds file path should not null!" PRN_RENC_NG
        AmbaMisra_TouchUnused(pHdsFilePath);
    } else if (0U != SvcRawEnc_MutexTake(&(SVC_RawEncCtrl.Mutex), SVC_RAW_ENC_MAX_MUTX_TIMEOUT)) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to cfg hds setting - take mutex timeout!" PRN_RENC_NG
    } else {
        UINT32 Idx;
        SVC_RAW_ENC_VIN_CFG_s *pCurCfg;

        for (Idx = 0U; Idx < NumCfg; Idx ++) {
            pCurCfg = &(SVC_RawEncCtrl.VinCfg[pVinIDArr[Idx]]);
            AmbaSvcWrap_MisraMemset(pCurCfg->HdsFilePath, 0, sizeof(pCurCfg->HdsFilePath));
            SvcWrap_strcpy(pCurCfg->HdsFilePath, sizeof(pCurCfg->HdsFilePath), pHdsFilePath[Idx]);

            pCurCfg->Status |= SVC_RAW_ENC_HDS_UPDATE;
        }

        SvcRawEnc_MutexGive(&(SVC_RawEncCtrl.Mutex));
    }

    return RetVal;
}

static UINT32 SvcRawEnc_AaaDataCfg(UINT32 VinID, UINT32 IkCtxID, UINT32 ImgChanID, char *pAaaFilePath)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to cfg aaa setting - create raw encode module first!" PRN_RENC_NG
    } else if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to cfg aaa setting - input vin id is out-of range!" PRN_RENC_NG
    } else if (pAaaFilePath == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to cfg aaa setting - input aaa file path should not null!" PRN_RENC_NG
        AmbaMisra_TouchUnused(pAaaFilePath);
    } else if (ImgChanID >= SVC_RAW_ENC_3A_MAX_CHAN) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to cfg aaa setting - input image channel id is out-of range!" PRN_RENC_NG
    } else if (0U != SvcRawEnc_MutexTake(&(SVC_RawEncCtrl.Mutex), SVC_RAW_ENC_MAX_MUTX_TIMEOUT)) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to cfg aaa setting - take mutex timeout!" PRN_RENC_NG
    } else {
        UINT32 ChIdx;
        SVC_RAW_ENC_VIN_CFG_s *pCurCfg = &(SVC_RawEncCtrl.VinCfg[VinID]);

        if ((pCurCfg->Status & SVC_RAW_ENC_AAA_UPDATE) == 0U) {
            for (ChIdx = 0U; ChIdx < SVC_RAW_ENC_3A_MAX_CHAN; ChIdx ++) {
                AmbaSvcWrap_MisraMemset(pCurCfg->ImgChanFilePath[ChIdx], 0, SVC_RAW_ENC_MAX_FILE_NAME);
            }
            pCurCfg->ImgChanSelBits = 0U;
            AmbaSvcWrap_MisraMemset(pCurCfg->IKMode, 0, sizeof(pCurCfg->IKMode));
        }

        pCurCfg->IKMode[ImgChanID].ContextId = IkCtxID;
        pCurCfg->ImgChanSelBits |= SvcRawEnc_BitGet(ImgChanID);
        SvcWrap_strcpy(pCurCfg->ImgChanFilePath[ImgChanID], SVC_RAW_ENC_MAX_FILE_NAME, pAaaFilePath);

        pCurCfg->Status |= SVC_RAW_ENC_AAA_UPDATE;

        SvcRawEnc_MutexGive(&(SVC_RawEncCtrl.Mutex));
    }

    return RetVal;
}

static UINT32 SvcRawEnc_ViewZoneCfg(UINT32 NumViewZone, UINT32 *pViewZoneIDArr, char **pItnPath)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to cfg view zone setting - create raw encode module first!" PRN_RENC_NG
    } else if (pViewZoneIDArr == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to cfg view zone setting - view zone id should not null!" PRN_RENC_NG
        AmbaMisra_TouchUnused(pViewZoneIDArr);
    } else if (pItnPath == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to cfg view zone setting - input ituner path should not null!" PRN_RENC_NG
        AmbaMisra_TouchUnused(pItnPath);
    } else if (0U != SvcRawEnc_MutexTake(&(SVC_RawEncCtrl.Mutex), SVC_RAW_ENC_MAX_MUTX_TIMEOUT)) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to cfg view zone setting - take mutex timeout!" PRN_RENC_NG
    } else {
        UINT32 Idx;
        SVC_RAW_ENC_VIEW_ZONE_CFG_s *pCurCfg;

        for (Idx = 0U; Idx < NumViewZone; Idx ++) {
            pCurCfg = &(SVC_RawEncCtrl.ViewZoneCfg[Idx]);
            pCurCfg->IKMode.ContextId = pViewZoneIDArr[Idx];
            AmbaSvcWrap_MisraMemset(pCurCfg->ItnFilePath, 0, sizeof(pCurCfg->ItnFilePath));
            SvcWrap_strcpy(pCurCfg->ItnFilePath, sizeof(pCurCfg->ItnFilePath), pItnPath[Idx]);

            pCurCfg->Status |= SVC_RAW_ENC_ITN_UPDATE;
        }

        SvcRawEnc_MutexGive(&(SVC_RawEncCtrl.Mutex));
    }

    return RetVal;
}

static UINT32 SvcRawEnc_ExecuteRawData(SVC_RAW_ENC_VIN_CFG_s *pCfg, UINT8 *pBuf, UINT32 BufSize)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (pCfg == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to execute raw data - input cfg should not null!" PRN_RENC_NG
    } else if (pBuf == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to execute raw data - invalid raw buffer!" PRN_RENC_NG
    } else if (BufSize == 0U) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to execute raw data - external raw buffer size should not zero!" PRN_RENC_NG
    } else {
        if ((pCfg->Status & SVC_RAW_ENC_RAW_UPDATE) == SVC_RAW_ENC_RAW_UPDATE) {
            AMBA_FS_FILE_INFO_s FileInfo;
            UINT32 FileSize = 0U;

            AmbaSvcWrap_MisraMemset(&FileInfo, 0, sizeof(FileInfo));
            if (0U != AmbaFS_GetFileInfo(pCfg->RawFilePath, &FileInfo)) {
                RetVal = SVC_NG;

                PRN_RENC_LOG "Fail to execute raw data - get file info fail '%s'"
                    PRN_RENC_ARG_CSTR   pCfg->RawFilePath PRN_RENC_ARG_POST
                PRN_RENC_NG
            } else {
                FileSize = (UINT32)(FileInfo.Size);
                if (FileSize == 0U) {
                    RetVal = SVC_NG;
                    PRN_RENC_LOG "Fail to execute raw data - file size should not zero!" PRN_RENC_NG
                } else {
                    if (FileSize > BufSize) {
                        RetVal = SVC_NG;
                        PRN_RENC_LOG "Fail to execute raw data - raw buf(0x%x) is too small to service it! 0x%x"
                            PRN_RENC_ARG_UINT32 BufSize  PRN_RENC_ARG_POST
                            PRN_RENC_ARG_UINT32 FileSize PRN_RENC_ARG_POST
                        PRN_RENC_NG
                    }
                }
            }

            if (RetVal == SVC_OK) {
                AMBA_FS_FILE *pFile = NULL;

                PRetVal = AmbaFS_FileOpen(pCfg->RawFilePath, "rb", &pFile);
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    PRN_RENC_LOG "Fail to execute raw data - open file fail! ErrCode(0x%08X) - %s"
                        PRN_RENC_ARG_UINT32 PRetVal           PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   pCfg->RawFilePath PRN_RENC_ARG_POST
                    PRN_RENC_NG
                } else if (pFile == NULL) {
                    RetVal = SVC_NG;
                    PRN_RENC_LOG "Fail to execute raw data - invalid file pointer" PRN_RENC_NG
                } else {
                    UINT32 RetSize = 0U;

                    AmbaSvcWrap_MisraMemset(pBuf, 0, BufSize);

                    if (0U != AmbaFS_FileRead(pBuf, 1, FileSize, pFile, &RetSize)) {
                        RetVal = SVC_NG;
                        PRN_RENC_LOG "Fail to execute raw data - read file data fail" PRN_RENC_NG
                    } else {
                        if (FileSize != RetSize) {
                            PRN_RENC_LOG "Warning to read file - return size(0x%x) does not equal file size(0x%x)"
                                PRN_RENC_ARG_UINT32 RetSize  PRN_RENC_ARG_POST
                                PRN_RENC_ARG_UINT32 FileSize PRN_RENC_ARG_POST
                            PRN_RENC_DBG
                        }
                    }

                    PRetVal = AmbaFS_FileClose(pFile);
                    if (PRetVal != 0U) {
                        PRN_RENC_LOG "Fail to execute raw data - close file fail!" PRN_RENC_NG
                    }

                    if (RetVal == SVC_OK) {
                        PRN_RENC_LOG "Load the raw data from %s"
                            PRN_RENC_ARG_CSTR   pCfg->RawFilePath PRN_RENC_ARG_POST
                        PRN_RENC_DBG
                    }
                }
            }
        } else {
            if (pCfg->pPreRawBuf == NULL) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "Fail to execute raw enc - invalid previous raw buffer(%p)!"
                    PRN_RENC_ARG_CPOINT pCfg->pPreRawBuf PRN_RENC_ARG_POST
                PRN_RENC_NG
            } else {
                if (0U == AmbaWrap_memcpy(pBuf, pCfg->pPreRawBuf, pCfg->PreRawBufSize)) {
                    RetVal = SVC_OK;

                    PRN_RENC_LOG "Copy the raw data. buf(%p, 0x%x) -> buf(%p, 0x%x)"
                        PRN_RENC_ARG_CPOINT pCfg->pPreRawBuf    PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 pCfg->PreRawBufSize PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CPOINT pBuf                PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 BufSize             PRN_RENC_ARG_POST
                    PRN_RENC_DBG
                }
            }
        }
    }

    AmbaMisra_TouchUnused(pCfg);

    return RetVal;
}

static UINT32 SvcRawEnc_ExecuteHdsData(SVC_RAW_ENC_VIN_CFG_s *pCfg, UINT8 *pBuf, UINT32 BufSize)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (pCfg == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to execute hds data - input cfg should not null!" PRN_RENC_NG
    } else if (pBuf == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to execute hds data - invalid hds buffer!" PRN_RENC_NG
    } else if (BufSize == 0U) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to execute hds data - external hds buffer size should not zero!" PRN_RENC_NG
    } else {
        if ((pCfg->Status & SVC_RAW_ENC_HDS_UPDATE) == SVC_RAW_ENC_HDS_UPDATE) {
            AMBA_FS_FILE_INFO_s FileInfo;
            UINT32 FileSize = 0U;

            AmbaSvcWrap_MisraMemset(&FileInfo, 0, sizeof(FileInfo));
            if (0U != AmbaFS_GetFileInfo(pCfg->HdsFilePath, &FileInfo)) {
                RetVal = SVC_NG;

                PRN_RENC_LOG "Fail to execute hds data - get file info fail %s"
                    PRN_RENC_ARG_CSTR   pCfg->HdsFilePath PRN_RENC_ARG_POST
                PRN_RENC_NG
            } else {
                FileSize = (UINT32)(FileInfo.Size);
                if (FileSize == 0U) {
                    RetVal = SVC_NG;
                    PRN_RENC_LOG "Fail to execute hds data - file size should not zero!" PRN_RENC_NG
                } else {
                    if (FileSize > BufSize) {
                        RetVal = SVC_NG;
                        PRN_RENC_LOG "Fail to execute hds data - hds buf(0x%x) is too small to service it! 0x%x"
                            PRN_RENC_ARG_UINT32 BufSize  PRN_RENC_ARG_POST
                            PRN_RENC_ARG_UINT32 FileSize PRN_RENC_ARG_POST
                        PRN_RENC_NG
                    }
                }
            }

            if (RetVal == SVC_OK) {
                AMBA_FS_FILE *pFile = NULL;

                PRetVal = AmbaFS_FileOpen(pCfg->HdsFilePath, "rb", &pFile);
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    PRN_RENC_LOG "Fail to execute hds data - open file fail! ErrCode(0x%08X) - %s"
                        PRN_RENC_ARG_UINT32 PRetVal           PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   pCfg->HdsFilePath PRN_RENC_ARG_POST
                    PRN_RENC_NG
                } else if (pFile == NULL) {
                    RetVal = SVC_NG;
                    PRN_RENC_LOG "Fail to execute hds data - open file fail - %s"
                        PRN_RENC_ARG_CSTR   pCfg->HdsFilePath PRN_RENC_ARG_POST
                    PRN_RENC_NG
                } else {
                    UINT32 RetSize = 0U;
                    if (0U != AmbaFS_FileRead(pBuf, 1, FileSize, pFile, &RetSize)) {
                        RetVal = SVC_NG;
                        PRN_RENC_LOG "Fail to execute hds data - read file data fail" PRN_RENC_NG
                    } else {
                        if (FileSize != RetSize) {
                            PRN_RENC_LOG "Warning to read file - return size(0x%x) does not equal file size(0x%x)"
                                PRN_RENC_ARG_UINT32 RetSize  PRN_RENC_ARG_POST
                                PRN_RENC_ARG_UINT32 FileSize PRN_RENC_ARG_POST
                            PRN_RENC_DBG
                        }
                    }

                    PRetVal = AmbaFS_FileClose(pFile);
                    if (PRetVal != 0U) {
                        PRN_RENC_LOG "Fail to execute hds data - close file fail!" PRN_RENC_NG
                    }

                    if (RetVal == SVC_OK) {
                        PRN_RENC_LOG "Load the hds data from %s"
                            PRN_RENC_ARG_CSTR   pCfg->HdsFilePath PRN_RENC_ARG_POST
                        PRN_RENC_DBG
                    }
                }
            }
        } else {
            if (pCfg->pPreHdsBuf == NULL) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "Fail to execute raw enc - invalid previous hds buffer(%p)!"
                    PRN_RENC_ARG_CPOINT pCfg->pPreHdsBuf PRN_RENC_ARG_POST
                PRN_RENC_NG
            } else {
                if (0U == AmbaWrap_memcpy(pBuf, pCfg->pPreHdsBuf, pCfg->PreHdsBufSize)) {
                    RetVal = SVC_OK;
                    PRN_RENC_LOG "Copy the hds data. buf(%p, 0x%x) -> buf(%p, 0x%x)"
                        PRN_RENC_ARG_CPOINT pCfg->pPreHdsBuf    PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 pCfg->PreHdsBufSize PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CPOINT pBuf                PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 BufSize             PRN_RENC_ARG_POST
                    PRN_RENC_DBG
                }
            }
        }
    }

    AmbaMisra_TouchUnused(pCfg);

    return RetVal;
}

static UINT32 SvcRawEnc_ExecuteAaaData(SVC_RAW_ENC_VIN_CFG_s *pCfg, UINT8 *pBuf, UINT32 BufSize)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (pCfg == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to execute aaa data - input cfg should not null!" PRN_RENC_NG
    } else if (BufSize == 0U) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to execute aaa data - external aaa buffer size should not zero!" PRN_RENC_NG
    } else if (pBuf == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to execute aaa data - invalid aaa buffer!" PRN_RENC_NG
    } else if ((pCfg->Status & SVC_RAW_ENC_AAA_UPDATE) != SVC_RAW_ENC_AAA_UPDATE) {
        PRN_RENC_LOG "Not need to update aaa data!" PRN_RENC_DBG
    } else {
        UINT32 ChanIdx;
        AMBA_FS_FILE_INFO_s FileInfo;
        UINT32 FileSize = 0U;
        AMBA_FS_FILE *pFile;
        UINT64 TmpU64 = 0U;
        SVC_RAW_ENC_UPD_AAA_INFO UpdInfo;
        UINT32 RetSize = 0U;
        UINT32 FovID;

        TmpU64 = (UINT64)BufSize; TmpU64 &= 0xFFFFFFFFULL;

        for (ChanIdx = 0U; ChanIdx < SVC_RAW_ENC_3A_MAX_CHAN; ChanIdx ++) {
            if ((pCfg->ImgChanSelBits & SvcRawEnc_BitGet(ChanIdx)) > 0U) {

                FovID = pCfg->IKMode[ChanIdx].ContextId;

                /* Check the file size */
                AmbaSvcWrap_MisraMemset(&FileInfo, 0, sizeof(FileInfo));
                if (0U != AmbaFS_GetFileInfo(pCfg->ImgChanFilePath[ChanIdx], &FileInfo)) {
                    RetVal = SVC_NG;

                    PRN_RENC_LOG "Fail to execute aaa data - get img chan(%d) file info fail %s"
                        PRN_RENC_ARG_UINT32 ChanIdx                        PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   pCfg->ImgChanFilePath[ChanIdx] PRN_RENC_ARG_POST
                    PRN_RENC_NG
                } else if (FileInfo.Size == 0U) {
                    RetVal = SVC_NG;
                    PRN_RENC_LOG "Fail to execute aaa data - file size should not zero!" PRN_RENC_NG
                } else if (FileInfo.Size > TmpU64) {
                    RetVal = SVC_NG;
                    PRN_RENC_LOG "Fail to execute aaa data - aaa buf(0x%x) is too small to service it! 0x%x"
                        PRN_RENC_ARG_UINT32 (UINT32)TmpU64          PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 (UINT32)(FileInfo.Size) PRN_RENC_ARG_POST
                    PRN_RENC_NG
                } else if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
                    RetVal = SVC_NG;
                    PRN_RENC_LOG "Fail to execute aaa data - img chan(%d)'s ik ctx id(%d) is out-of range"
                        PRN_RENC_ARG_UINT32 ChanIdx PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 FovID   PRN_RENC_ARG_POST
                    PRN_RENC_NG
                } else {
                    pFile = NULL;
                    PRetVal = AmbaFS_FileOpen(pCfg->ImgChanFilePath[ChanIdx], "rb", &pFile);
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;
                        PRN_RENC_LOG "Fail to execute aaa data - open img chan(%d) file fail - %s"
                            PRN_RENC_ARG_UINT32 ChanIdx                        PRN_RENC_ARG_POST
                            PRN_RENC_ARG_CSTR   pCfg->ImgChanFilePath[ChanIdx] PRN_RENC_ARG_POST
                        PRN_RENC_NG
                    } else if (pFile == NULL) {
                        RetVal = SVC_NG;

                        PRN_RENC_LOG "Fail to execute aaa data - invalid file pointer" PRN_RENC_NG
                    } else {

                        FileSize = (UINT32)(FileInfo.Size);

                        if (0U != AmbaFS_FileRead(pBuf, 1, FileSize, pFile, &RetSize)) {
                            RetVal = SVC_NG;
                            PRN_RENC_LOG "Fail to execute aaa data - read file data fail" PRN_RENC_NG
                        } else {
                            if (FileSize != RetSize) {
                                PRN_RENC_LOG "Warning to read file - return size(0x%x) does not equal file size(0x%x)"
                                    PRN_RENC_ARG_UINT32 RetSize  PRN_RENC_ARG_POST
                                    PRN_RENC_ARG_UINT32 FileSize PRN_RENC_ARG_POST
                                PRN_RENC_DBG
                            }

                            PRN_RENC_LOG "Load the img chan(%d) aaa data from %s"
                                PRN_RENC_ARG_UINT32 ChanIdx                        PRN_RENC_ARG_POST
                                PRN_RENC_ARG_CSTR   pCfg->ImgChanFilePath[ChanIdx] PRN_RENC_ARG_POST
                            PRN_RENC_DBG

                            AmbaSvcWrap_MisraMemset(&UpdInfo, 0, sizeof(SVC_RAW_ENC_UPD_AAA_INFO));
                            UpdInfo.ImgChanId = ChanIdx;
                            UpdInfo.pAaaData  = pBuf;

                            // Update aaa data
                            SvcRawEnc_ProcCallBack(SVC_RAW_ENC_CODE_UPD_AAA, &UpdInfo);

                            SVC_RawEncCtrl.ViewZoneIdspBits |= SvcRawEnc_BitGet(FovID);
                        }

                        if(0U != AmbaFS_FileClose(pFile)) {
                            PRN_RENC_LOG "Fail to execute aaa data - close file fail!" PRN_RENC_NG
                        }
                    }
                }
            }
        }
    }

    AmbaMisra_TouchUnused(pCfg);

    return RetVal;
}

static UINT32 SvcRawEnc_ExecuteItnData(void)
{
    UINT32 RetVal = SVC_OK;

    if (SVC_RawEncCtrl.pItnMemBuf == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to execute ituner - ituner buffer should not null!" PRN_RENC_NG
    } else if (SVC_RawEncCtrl.ItnMemSize == 0U) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to execute ituner - ituner buffer size should not zero" PRN_RENC_NG
    } else {
        UINT32 ViewZoneIdx;
        SVC_RAW_ENC_VIEW_ZONE_CFG_s *pCfg;
        void *pMemBuf;

        // Configure the ituner buffer
        AmbaMisra_TypeCast(&(pMemBuf), &(SVC_RawEncCtrl.pItnMemBuf));

        for (ViewZoneIdx = 0U; ViewZoneIdx < AMBA_DSP_MAX_VIEWZONE_NUM; ViewZoneIdx ++) {
            if ((SVC_RawEncCtrl.ViewZoneSelectBits & SvcRawEnc_BitGet(ViewZoneIdx)) > 0U) {
                pCfg = &(SVC_RawEncCtrl.ViewZoneCfg[ViewZoneIdx]);

                if ((pCfg->Status & SVC_RAW_ENC_ITN_UPDATE) > 0U) {
                    AmbaSvcWrap_MisraMemset(pMemBuf, 0, SVC_RawEncCtrl.ItnMemSize);

                    PRN_RENC_LOG "====== Execute Ituner - IKContextID(%d) ======"
                        PRN_RENC_ARG_UINT32 pCfg->IKMode.ContextId PRN_RENC_ARG_POST
                    PRN_RENC_API

                    if (pCfg->IKMode.ContextId >= MAX_CONTEXT_NUM) {
                        PRN_RENC_LOG "Fail to execute idsp - Ik CtxId(%d) out-of range"
                            PRN_RENC_ARG_UINT32 pCfg->IKMode.ContextId PRN_RENC_ARG_POST
                        PRN_RENC_NG
                    } else {

                        // Change the tuner module mode as TEXT
                        if (0U != AmbaItn_Change_Parser_Mode(TEXT_TUNE)) {
                            PRN_RENC_LOG "Fail to execute idsp - set 'TEXT_TUNE' mode fail!" PRN_RENC_NG
                        }

                        // Initial ituner module
                        if (0U != AmbaItn_Init(pMemBuf, SVC_RawEncCtrl.ItnMemSize)) {
                            RetVal = SVC_NG;
                            PRN_RENC_LOG "Fail to execute idsp - initial tuner module fail!" PRN_RENC_NG
                        } else {
                            AMBA_ITN_Load_Param_s Ituner_LoadParam;

                            // Load the idsp setting from ituner file
                            AmbaSvcWrap_MisraMemset(&Ituner_LoadParam, 0, sizeof(Ituner_LoadParam));
                            Ituner_LoadParam.Text.FilePath = pCfg->ItnFilePath;
                            if (0U != AmbaItn_Load_IDSP(&Ituner_LoadParam)) {
                                RetVal = SVC_NG;
                                PRN_RENC_LOG "Fail to execute idsp - load ituner fail!" PRN_RENC_NG
                            } else {
                                // Execute idsp
                                if (0U != AmbaItn_Execute_IDSP(&(pCfg->IKMode))) {
                                    RetVal = SVC_NG;
                                    PRN_RENC_LOG "Fail to execute idsp - execute idsp fail!" PRN_RENC_NG
                                } else {
                                    SVC_RawEncCtrl.ViewZoneIdspBits |= SvcRawEnc_BitGet(pCfg->IKMode.ContextId);

                                    PRN_RENC_LOG "Successful to execute idsp!" PRN_RENC_OK
                                }
                            }
                        }

                        pCfg->Status &= ~(SVC_RAW_ENC_ITN_UPDATE);
                    }
                }
            }
        }

    }

    return RetVal;
}

static UINT32 SvcRawEnc_ExecuteIdsp(void)
{
    UINT32 RetVal = SVC_OK;

    if (((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_DEF_IDSP_CFG) > 0U)
       &&(SVC_RawEncCtrl.FeedCounter > 0U)) {
        UINT32                      ViewZoneIdx;
        AMBA_IK_MODE_CFG_s          IkMode;
        AMBA_IK_EXECUTE_CONTAINER_s IkExec;

        for (ViewZoneIdx = 0U; ViewZoneIdx < AMBA_DSP_MAX_VIEWZONE_NUM; ViewZoneIdx ++) {
            if ((SVC_RawEncCtrl.ViewZoneSelectBits & SvcRawEnc_BitGet(ViewZoneIdx)) > 0U) {

                AmbaSvcWrap_MisraMemset(&IkMode, 0, sizeof(IkMode));
                IkMode.ContextId = ViewZoneIdx;

                AmbaSvcWrap_MisraMemset(&IkExec, 0, sizeof(IkExec));
                if (0U != AmbaIK_ExecuteConfig(&(IkMode), &IkExec)) {
                    RetVal |= SVC_NG;
                    PRN_RENC_LOG "Fail to execute idsp - execute FovID(%d) ik cfg fail"
                        PRN_RENC_ARG_UINT32 ViewZoneIdx PRN_RENC_ARG_POST
                    PRN_RENC_NG
                } else {
                    AMBA_DSP_LIVEVIEW_ISOCFG_CTRL_s IsoCfg;
                    ULONG  Addr;
                    UINT64 RawSeqNum;

                    AmbaMisra_TypeCast(&(Addr), &(IkExec.pIsoCfg));

                    AmbaSvcWrap_MisraMemset(&IsoCfg, 0, sizeof(IsoCfg));
                    IsoCfg.ViewZoneId  = (UINT16) ( IkMode.ContextId );
                    IsoCfg.CtxIndex    = IkMode.ContextId;
                    IsoCfg.CfgIndex    = IkExec.IkId;
                    IsoCfg.HdrCfgIndex = IkExec.IkId;
                    IsoCfg.CfgAddress  = Addr;
                    if (0U != AmbaDSP_LiveviewUpdateIsoCfg(1U, &IsoCfg, &RawSeqNum)) {
                        RetVal |= SVC_NG;
                        PRN_RENC_LOG "Fail to execute idsp - update FovID(%d) iso cfg fail!"
                            PRN_RENC_ARG_UINT32 ViewZoneIdx PRN_RENC_ARG_POST
                        PRN_RENC_NG
                    }
                }

                SVC_RawEncCtrl.ViewZoneIdspBits &= ~SvcRawEnc_BitGet(ViewZoneIdx);
            }
        }

        if (RetVal == SVC_OK) {
            PRN_RENC_LOG "Successful to execute idsp!" PRN_RENC_OK
        }
    }

    return RetVal;
}

static UINT32 SvcRawEnc_ExecuteCmd(UINT32 NumVin, const UINT16 *pViewZoneIDArr, const AMBA_DSP_LIVEVIEW_EXT_RAW_s *pLivExtRaw, SVC_RAW_ENC_BUF_INFO_s *pBufInfo)
{
    UINT32 RetVal = SVC_OK;

    if (NumVin == 0U) {
        RetVal = SVC_NG;
    } else if (pViewZoneIDArr == NULL) {
        RetVal = SVC_NG;
    } else if (pLivExtRaw == NULL) {
        RetVal = SVC_NG;
    } else if (pBufInfo == NULL) {
        RetVal = SVC_NG;
    } else {
        UINT32 WaitRawFlag = 0U, WaitYuvFlag = 0U;
        UINT32 VinID = 0U, BufIdx;

        PRN_RENC_LOG " " PRN_RENC_API
        PRN_RENC_LOG "==== Feed No.%03d frame - BEGIN ===="
            PRN_RENC_ARG_UINT32 SVC_RawEncCtrl.FeedCounter PRN_RENC_ARG_POST
        PRN_RENC_API

        for (BufIdx = 0U; BufIdx < NumVin; BufIdx ++) {
            VinID = (UINT32)(pLivExtRaw[BufIdx].VinId); VinID &= 0x0000FFFFU;

            WaitRawFlag |= (UINT32)(SVC_RAW_ENC_EVT_FLG_RAW_RDY << VinID);
            WaitYuvFlag |= (UINT32)(SVC_RAW_ENC_EVT_FLG_YUV_RDY << VinID);

            PRN_RENC_LOG "---- Feed Raw Info (%d) ----"
                PRN_RENC_ARG_UINT32 BufIdx PRN_RENC_ARG_POST
            PRN_RENC_API
            PRN_RENC_LOG "  VinID : %d"
                PRN_RENC_ARG_UINT32 pLivExtRaw[BufIdx].VinId PRN_RENC_ARG_POST
            PRN_RENC_API
            PRN_RENC_LOG "  RawBuffer : " PRN_RENC_API
            PRN_RENC_LOG "    Compressed : %d"
                PRN_RENC_ARG_UINT32 pLivExtRaw[BufIdx].RawBuffer.Compressed PRN_RENC_ARG_POST
            PRN_RENC_API
            PRN_RENC_LOG "    Pitch      : %d"
                PRN_RENC_ARG_UINT32 pLivExtRaw[BufIdx].RawBuffer.Pitch PRN_RENC_ARG_POST
            PRN_RENC_API
            PRN_RENC_LOG "    Width      : %d"
                PRN_RENC_ARG_UINT32 pLivExtRaw[BufIdx].RawBuffer.Window.Width PRN_RENC_ARG_POST
            PRN_RENC_API
            PRN_RENC_LOG "    Height     : %d"
                PRN_RENC_ARG_UINT32 pLivExtRaw[BufIdx].RawBuffer.Window.Height PRN_RENC_ARG_POST
            PRN_RENC_API
        }

        SVC_RawEncCtrl.EncStrmBits = 0U;

        if (0U != AmbaKAL_EventFlagClear(&(SVC_RawEncCtrl.EventFlag), WaitRawFlag)) {
            PRN_RENC_LOG "Fail to execute raw enc - clear wait raw flag!" PRN_RENC_NG
        }
        if (0U != AmbaKAL_EventFlagClear(&(SVC_RawEncCtrl.EventFlag), WaitYuvFlag)) {
            PRN_RENC_LOG "Fail to execute raw enc - clear wait yuv flag!" PRN_RENC_NG
        }

        if (0U != AmbaDSP_LiveviewFeedRawData((UINT16)NumVin, pViewZoneIDArr, pLivExtRaw)) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "Fail to execute raw enc - feed raw data fail!" PRN_RENC_NG
        } else {

            SVC_RawEncCtrl.FeedCounter += 1U;

            if (SVC_RawEncCtrl.FeedCounter >= SVC_RAW_ENC_BUF_CHK_IDX) {
                UINT32 TempVal = 0U;
                UINT32 ActualFlag = 0U;

                if (0U != AmbaKAL_EventFlagGet(&(SVC_RawEncCtrl.EventFlag),
                                               WaitRawFlag,
                                               AMBA_KAL_FLAGS_ALL,
                                               AMBA_KAL_FLAGS_CLEAR_NONE,
                                               &ActualFlag,
                                               10000U)) {
                    RetVal = SVC_NG;
                    PRN_RENC_LOG "Fail to execute raw enc - cannot get current raw data event! 0x%x"
                        PRN_RENC_ARG_UINT32 ActualFlag PRN_RENC_ARG_POST
                    PRN_RENC_NG
                }

                if (RetVal == SVC_OK) {
                    for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
                        if ((SVC_RawEncCtrl.VinSelectBits & SvcRawEnc_BitGet(VinID)) > 0U) {
                            if (pBufInfo[VinID].Status == SVC_RAW_ENC_BUF_USED) {
                                if (SVC_OK != SvcRawEnc_BufCtrlLockBuf(&(SVC_RawEncCtrl.BufCtrl[VinID]), &(pBufInfo[VinID]))) {
                                    TempVal |= SVC_NG;
                                    PRN_RENC_LOG "Fail to execute raw enc - lock the VinID(%d) Buffer fail!"
                                        PRN_RENC_ARG_UINT32 VinID PRN_RENC_ARG_POST
                                    PRN_RENC_NG
                                } else {
                                    PRN_RENC_LOG "Successful to lock the VinID(%d) Buffer"
                                        PRN_RENC_ARG_UINT32 VinID PRN_RENC_ARG_POST
                                    PRN_RENC_DBG
                                }
                            } else if (pBufInfo[VinID].Status == SVC_RAW_ENC_BUF_FREE) {
                                if (SVC_OK != SvcRawEnc_BufCtrlFreeBuf(&(SVC_RawEncCtrl.BufCtrl[VinID]), &(pBufInfo[VinID]))) {
                                    TempVal |= SVC_NG;
                                    PRN_RENC_LOG "Fail to execute raw enc - free the VinID(%d) Buffer fail!"
                                        PRN_RENC_ARG_UINT32 VinID PRN_RENC_ARG_POST
                                    PRN_RENC_NG
                                } else {
                                    PRN_RENC_LOG "Successful to free the VinID(%d) Buffer"
                                        PRN_RENC_ARG_UINT32 VinID PRN_RENC_ARG_POST
                                    PRN_RENC_DBG
                                }
                            } else {
                                /* Do nothing */
                            }
                        }
                    }
                }

                if (SVC_OK == TempVal) {
                    PRN_RENC_LOG "Successful to execute raw enc!" PRN_RENC_OK
                }

                ActualFlag = 0U;
                if (0U != AmbaKAL_EventFlagGet(&(SVC_RawEncCtrl.EventFlag),
                                               WaitYuvFlag,
                                               AMBA_KAL_FLAGS_ALL,
                                               AMBA_KAL_FLAGS_CLEAR_NONE,
                                               &ActualFlag,
                                               10000U)) {
                    RetVal = SVC_NG;
                    PRN_RENC_LOG "Fail to execute raw enc - cannot get current yuv data event! 0x%x"
                        PRN_RENC_ARG_UINT32 ActualFlag PRN_RENC_ARG_POST
                    PRN_RENC_NG
                }

                if (RetVal == SVC_OK) {
                    for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
                        if ((SVC_RawEncCtrl.VinSelectBits & SvcRawEnc_BitGet(VinID)) > 0U) {
                            if (0U != SvcRawEnc_BufCtrlFreeBuf(&(SVC_RawEncCtrl.BufCtrl[VinID]), NULL)) {
                                PRN_RENC_LOG "Fail to execute raw enc - free VinID(%d) buffer ctrl!"
                                    PRN_RENC_ARG_UINT32 VinID PRN_RENC_ARG_POST
                                PRN_RENC_NG
                            }
                        }
                    }
                }
            } else {
                PRN_RENC_LOG "Successful to execute raw enc!" PRN_RENC_OK
            }
        }

        PRN_RENC_LOG "==== Feed frame - END ====" PRN_RENC_API
    }

    return RetVal;
}

static UINT32 SvcRawEnc_VinSrcCfg(UINT32 NumCfg, const UINT32 *pVinIDArr, const UINT32 *pVinSrc)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RES_CFG_s *pRawEncResCfg = SvcRawEnc_ResCfgGet();

    if  ((NumCfg == 1U)
    /* &&(*pVinIDArr == 0U)*/
       && (*pVinSrc == 0U)) {
        if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "Fail to config vin src - resolution config should not null" PRN_RENC_NG
        } else {
            RetVal = SvcResCfg_Config(pRawEncResCfg);
            if (RetVal != SVC_OK) {
                PRN_RENC_LOG "Fail to config vin src - re-config raw enc resolution cfg fail!" PRN_RENC_NG
            } else {
                PRN_RENC_LOG "Successful to config vin src - re-config raw enc resolution cfg done" PRN_RENC_OK

                RetVal = SvcResCfg_ConfigMax(pRawEncResCfg, 1U);
                if (RetVal == 0U) {
                    PRN_RENC_LOG "Successful to config vin src - re-config raw enc max resolution cfg done" PRN_RENC_OK
                }

                {
                    AMBA_SENSOR_CHANNEL_s SsChan = { .VinID = *pVinIDArr };
                    AmbaSensor_Hook(&SsChan, &RawEncDmySenObj);

                    PRN_RENC_LOG "Successful to hook VinID(%d) dummy sensor"
                        PRN_RENC_ARG_UINT32 SsChan.VinID PRN_RENC_ARG_POST
                    PRN_RENC_OK
                }
            }
        }
    } else {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to config vin src - Not support input VinID(%d) and VinSrc(%d)"
            PRN_RENC_ARG_UINT32 *pVinIDArr PRN_RENC_ARG_POST
            PRN_RENC_ARG_UINT32 *pVinSrc   PRN_RENC_ARG_POST
        PRN_RENC_NG
    }

    return RetVal;
}

static void SvcRawEnc_DefDmySenCfgInit(void)
{
    if ((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_DEF_DMY_SENSOR) == 0U) {
        AMBA_SENSOR_INPUT_INFO_s   *pInputInfo    = &RawEncDmySenCfg.InputInfo;
        AMBA_SENSOR_OUTPUT_INFO_s  *pOutputInfo   = &RawEncDmySenCfg.OutputInfo;
        SVC_RAW_ENC_DMY_SEN_INFO_s *pModeInfo     = &RawEncDmySenCfg.ModeInfo;
        AMBA_SENSOR_HDR_INFO_s     *pHdrInfo      = &RawEncDmySenCfg.HdrInfo;

        AmbaSvcWrap_MisraMemset(&RawEncDmySenCfg, 0, sizeof(RawEncDmySenCfg));

        pOutputInfo->DataRate                  = 1596000000;
        pOutputInfo->NumDataLanes              = 4;
        pOutputInfo->NumDataBits               = 10;
        pOutputInfo->BayerPattern              = AMBA_SENSOR_BAYER_PATTERN_RG;
        pOutputInfo->OutputWidth               = 3844;
        pOutputInfo->OutputHeight              = 4636;
        pOutputInfo->RecordingPixels.StartX    = 4;
        pOutputInfo->RecordingPixels.StartY    = 0;
        pOutputInfo->RecordingPixels.Width     = 3840;
        pOutputInfo->RecordingPixels.Height    = 4636;
        pOutputInfo->OpticalBlackPixels.StartX = 0;
        pOutputInfo->OpticalBlackPixels.StartY = 0;
        pOutputInfo->OpticalBlackPixels.Width  = 0;
        pOutputInfo->OpticalBlackPixels.Height = 0;

        pInputInfo->PhotodiodeArray.StartX   = 108;
        pInputInfo->PhotodiodeArray.StartY   = 440;
        pInputInfo->PhotodiodeArray.Width    = 3840;
        pInputInfo->PhotodiodeArray.Height   = 2160;
        pInputInfo->HSubsample.SubsampleType = AMBA_SENSOR_SUBSAMPLE_NORMAL;
        pInputInfo->HSubsample.FactorDen     = 1;
        pInputInfo->HSubsample.FactorNum     = 1;
        pInputInfo->VSubsample.SubsampleType = AMBA_SENSOR_SUBSAMPLE_NORMAL;
        pInputInfo->VSubsample.FactorDen     = 1;
        pInputInfo->VSubsample.FactorNum     = 1;
        pInputInfo->SummingFactor            = 1;

        pModeInfo->FrameTiming.InputClk                 = 23994772;
        pModeInfo->FrameTiming.LineLengthPck            = 4512;
        pModeInfo->FrameTiming.FrameLengthLines         = 4720;
        pModeInfo->FrameTiming.FrameRate.Interlace      = 0;
        pModeInfo->FrameTiming.FrameRate.TimeScale      = 30000;
        pModeInfo->FrameTiming.FrameRate.NumUnitsInTick = 1001;
        pModeInfo->RowTime                              = 0.0f;

        pHdrInfo->HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE;
        pHdrInfo->ActiveChannels = 2;

        pHdrInfo->ChannelInfo[0].EffectiveArea.StartX      = 0;
        pHdrInfo->ChannelInfo[0].EffectiveArea.StartY      = 0;
        pHdrInfo->ChannelInfo[0].EffectiveArea.Width       = 3840;
        pHdrInfo->ChannelInfo[0].EffectiveArea.Height      = 2160;
        pHdrInfo->ChannelInfo[0].OpticalBlackPixels.StartX = 0;
        pHdrInfo->ChannelInfo[0].OpticalBlackPixels.StartY = 0;
        pHdrInfo->ChannelInfo[0].OpticalBlackPixels.Width  = 0;
        pHdrInfo->ChannelInfo[0].OpticalBlackPixels.Height = 0;
        pHdrInfo->ChannelInfo[0].ShutterSpeedCtrlInfo.CommunicationTime   = AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK;
        pHdrInfo->ChannelInfo[0].ShutterSpeedCtrlInfo.FirstReflectedFrame = 2;
        pHdrInfo->ChannelInfo[0].ShutterSpeedCtrlInfo.FirstBadFrame       = 0;
        pHdrInfo->ChannelInfo[0].ShutterSpeedCtrlInfo.NumBadFrames        = 0;
        pHdrInfo->ChannelInfo[0].OutputFormatCtrlInfo.CommunicationTime   = AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK;
        pHdrInfo->ChannelInfo[0].OutputFormatCtrlInfo.FirstReflectedFrame = 2;
        pHdrInfo->ChannelInfo[0].OutputFormatCtrlInfo.FirstBadFrame       = 0;
        pHdrInfo->ChannelInfo[0].OutputFormatCtrlInfo.NumBadFrames        = 0;
        pHdrInfo->ChannelInfo[0].MaxExposureLine = 2180;
        pHdrInfo->ChannelInfo[0].MinExposureLine = 2;

        pHdrInfo->ChannelInfo[1].EffectiveArea.StartX      = 3840;
        pHdrInfo->ChannelInfo[1].EffectiveArea.StartY      = 158;
        pHdrInfo->ChannelInfo[1].EffectiveArea.Width       = 3840;
        pHdrInfo->ChannelInfo[1].EffectiveArea.Height      = 2160;
        pHdrInfo->ChannelInfo[1].OpticalBlackPixels.StartX = 0;
        pHdrInfo->ChannelInfo[1].OpticalBlackPixels.StartY = 0;
        pHdrInfo->ChannelInfo[1].OpticalBlackPixels.Width  = 0;
        pHdrInfo->ChannelInfo[1].OpticalBlackPixels.Height = 0;
        pHdrInfo->ChannelInfo[1].ShutterSpeedCtrlInfo.CommunicationTime   = AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK;
        pHdrInfo->ChannelInfo[1].ShutterSpeedCtrlInfo.FirstReflectedFrame = 2;
        pHdrInfo->ChannelInfo[1].ShutterSpeedCtrlInfo.FirstBadFrame       = 0;
        pHdrInfo->ChannelInfo[1].ShutterSpeedCtrlInfo.NumBadFrames        = 0;
        pHdrInfo->ChannelInfo[1].OutputFormatCtrlInfo.CommunicationTime   = AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK;
        pHdrInfo->ChannelInfo[1].OutputFormatCtrlInfo.FirstReflectedFrame = 2;
        pHdrInfo->ChannelInfo[1].OutputFormatCtrlInfo.FirstBadFrame       = 0;
        pHdrInfo->ChannelInfo[1].OutputFormatCtrlInfo.NumBadFrames        = 0;
        pHdrInfo->ChannelInfo[1].MaxExposureLine = 136;
        pHdrInfo->ChannelInfo[1].MinExposureLine = 2;

        AmbaSvcWrap_MisraMemset(&RawEncDmySenObj, 0, sizeof(RawEncDmySenObj));

        SvcWrap_strcpy(RawEncDmySenObj.SensorName, sizeof(RawEncDmySenObj.SensorName), "Dummy Sensor");
        SvcWrap_strcpy(RawEncDmySenObj.SerdesName, sizeof(RawEncDmySenObj.SerdesName), "N/A");
        RawEncDmySenObj.Init                   = SvcRawEnc_DmySenInit;
        RawEncDmySenObj.Enable                 = SvcRawEnc_DmySenEnable;
        RawEncDmySenObj.Disable                = SvcRawEnc_DmySenDisable;
        RawEncDmySenObj.Config                 = SvcRawEnc_DmySenConfig;
        RawEncDmySenObj.GetStatus              = SvcRawEnc_DmySenGetStatus;
        RawEncDmySenObj.GetModeInfo            = SvcRawEnc_DmySenGetModeInfo;
        RawEncDmySenObj.GetDeviceInfo          = SvcRawEnc_DmySenGetDeviceInfo;
        RawEncDmySenObj.GetHdrInfo             = SvcRawEnc_DmySenGetHdrInfo;
        RawEncDmySenObj.GetCurrentGainFactor   = SvcRawEnc_DmySenGetGainFactor;
        RawEncDmySenObj.GetCurrentShutterSpeed = SvcRawEnc_DmySenGetShutterSpd;
        RawEncDmySenObj.ConvertGainFactor      = SvcRawEnc_DmySenCvtGainFactor;
        RawEncDmySenObj.ConvertShutterSpeed    = SvcRawEnc_DmySenCvtShutterSpd;
        RawEncDmySenObj.SetAnalogGainCtrl      = SvcRawEnc_DmySenSetAGainCtrl;
        RawEncDmySenObj.SetDigitalGainCtrl     = SvcRawEnc_DmySenSetDGainCtrl;
        RawEncDmySenObj.SetWbGainCtrl          = SvcRawEnc_DmySenSetWbGainCtrl;
        RawEncDmySenObj.SetShutterCtrl         = SvcRawEnc_DmySenSetShutterCtrl;
        RawEncDmySenObj.SetSlowShutterCtrl     = SvcRawEnc_DmySenSetSlowShuCtrl;
        RawEncDmySenObj.RegisterRead           = SvcRawEnc_DmySenRegisterRead;
        RawEncDmySenObj.RegisterWrite          = SvcRawEnc_DmySenRegisterWrite;

        PRN_RENC_LOG "Successful to configure dummy sensor default setting!" PRN_RENC_OK

        SVC_RawEncCtrlFlag |= SVC_RAW_ENC_FLG_DEF_DMY_SENSOR;

        if ((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_DEF_RES_CFG) > 0U) {
            UINT32 VinIdx;
            for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
                RawEncResCfg.VinCfg[VinIdx].pDriver = &RawEncDmySenObj;
            }

            PRN_RENC_LOG "Configure video raw enc dummy sensor object" PRN_RENC_API
        }

        AmbaMisra_TouchUnused(&RawEncDmySenObj);
    }
}

static void SvcRawEnc_DmySenCfgPrint(void)
{
    UINT32 Idx;
    AMBA_SENSOR_INPUT_INFO_s   *pInputInfo    = &RawEncDmySenCfg.InputInfo;
    AMBA_SENSOR_OUTPUT_INFO_s  *pOutputInfo   = &RawEncDmySenCfg.OutputInfo;
    SVC_RAW_ENC_DMY_SEN_INFO_s *pModeInfo     = &RawEncDmySenCfg.ModeInfo;
    AMBA_SENSOR_HDR_INFO_s     *pHdrInfo      = &RawEncDmySenCfg.HdrInfo;

    const char SensorBayerStr[4U][29U] = {
        "AMBA_SENSOR_BAYER_PATTERN_RG",
        "AMBA_SENSOR_BAYER_PATTERN_BG",
        "AMBA_SENSOR_BAYER_PATTERN_GR",
        "AMBA_SENSOR_BAYER_PATTERN_GB",
    };

    const char SensorHdrStr[4U][37U] = {
        "AMBA_SENSOR_HDR_NONE",
        "AMBA_SENSOR_HDR_TYPE_MULTI_SLICE",
        "AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND",
        "AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL",
    };

    PRN_RENC_LOG " " PRN_RENC_API
    PRN_RENC_LOG "====== %sRaw Encode Dummy Sensor Configuration (%p)%s ======"
        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_TITLE_0 PRN_RENC_ARG_POST
        PRN_RENC_ARG_CPOINT &(RawEncDmySenCfg)      PRN_RENC_ARG_POST
        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END     PRN_RENC_ARG_POST
    PRN_RENC_API

    PRN_RENC_LOG "---- %sOutputInfo%s ----"
        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_TITLE_1 PRN_RENC_ARG_POST
        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END     PRN_RENC_ARG_POST
    PRN_RENC_API

    PRN_RENC_LOG "  { %d, %d, %d, %s, %d, %d, { %d, %d, %d, %d }, { %d, %d, %d, %d } }"
        PRN_RENC_ARG_UINT32 pOutputInfo->DataRate                     PRN_RENC_ARG_POST
        PRN_RENC_ARG_UINT32 pOutputInfo->NumDataLanes                 PRN_RENC_ARG_POST
        PRN_RENC_ARG_UINT32 pOutputInfo->NumDataBits                  PRN_RENC_ARG_POST
        PRN_RENC_ARG_CSTR   SensorBayerStr[pOutputInfo->BayerPattern] PRN_RENC_ARG_POST
        PRN_RENC_ARG_UINT32 pOutputInfo->OutputWidth                  PRN_RENC_ARG_POST
        PRN_RENC_ARG_UINT32 pOutputInfo->OutputHeight                 PRN_RENC_ARG_POST
        PRN_RENC_ARG_UINT32 pOutputInfo->RecordingPixels.StartX       PRN_RENC_ARG_POST
        PRN_RENC_ARG_UINT32 pOutputInfo->RecordingPixels.StartY       PRN_RENC_ARG_POST
        PRN_RENC_ARG_UINT32 pOutputInfo->RecordingPixels.Width        PRN_RENC_ARG_POST
        PRN_RENC_ARG_UINT32 pOutputInfo->RecordingPixels.Height       PRN_RENC_ARG_POST
        PRN_RENC_ARG_UINT32 pOutputInfo->OpticalBlackPixels.StartX    PRN_RENC_ARG_POST
        PRN_RENC_ARG_UINT32 pOutputInfo->OpticalBlackPixels.StartY    PRN_RENC_ARG_POST
        PRN_RENC_ARG_UINT32 pOutputInfo->OpticalBlackPixels.Width     PRN_RENC_ARG_POST
        PRN_RENC_ARG_UINT32 pOutputInfo->OpticalBlackPixels.Height    PRN_RENC_ARG_POST
    PRN_RENC_API

    PRN_RENC_LOG "---- %sInputInfo%s ----"
        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_TITLE_1 PRN_RENC_ARG_POST
        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END     PRN_RENC_ARG_POST
    PRN_RENC_API

    PRN_RENC_LOG "  {{ %d, %d, %d, %d}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, %d, %d }, { AMBA_SENSOR_SUBSAMPLE_NORMAL, %d, %d }, %d }"
        PRN_RENC_ARG_UINT32 pInputInfo->PhotodiodeArray.StartX PRN_RENC_ARG_POST
        PRN_RENC_ARG_UINT32 pInputInfo->PhotodiodeArray.StartY PRN_RENC_ARG_POST
        PRN_RENC_ARG_UINT32 pInputInfo->PhotodiodeArray.Width  PRN_RENC_ARG_POST
        PRN_RENC_ARG_UINT32 pInputInfo->PhotodiodeArray.Height PRN_RENC_ARG_POST
        PRN_RENC_ARG_UINT32 pInputInfo->HSubsample.FactorNum   PRN_RENC_ARG_POST
        PRN_RENC_ARG_UINT32 pInputInfo->HSubsample.FactorDen   PRN_RENC_ARG_POST
        PRN_RENC_ARG_UINT32 pInputInfo->VSubsample.FactorNum   PRN_RENC_ARG_POST
        PRN_RENC_ARG_UINT32 pInputInfo->VSubsample.FactorDen   PRN_RENC_ARG_POST
        PRN_RENC_ARG_UINT32 pInputInfo->SummingFactor          PRN_RENC_ARG_POST
    PRN_RENC_API

    PRN_RENC_LOG "---- %sModeInfo%s ----"
        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_TITLE_1 PRN_RENC_ARG_POST
        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END     PRN_RENC_ARG_POST
    PRN_RENC_API

    PRN_RENC_LOG "  { { %d, %d, %d, { .Interlace = %d, .TimeScale = %d, .NumUnitsInTick = %d } }, 0.0f }"
        PRN_RENC_ARG_UINT32 pModeInfo->FrameTiming.InputClk                 PRN_RENC_ARG_POST
        PRN_RENC_ARG_UINT32 pModeInfo->FrameTiming.LineLengthPck            PRN_RENC_ARG_POST
        PRN_RENC_ARG_UINT32 pModeInfo->FrameTiming.FrameLengthLines         PRN_RENC_ARG_POST
        PRN_RENC_ARG_UINT32 pModeInfo->FrameTiming.FrameRate.Interlace      PRN_RENC_ARG_POST
        PRN_RENC_ARG_UINT32 pModeInfo->FrameTiming.FrameRate.TimeScale      PRN_RENC_ARG_POST
        PRN_RENC_ARG_UINT32 pModeInfo->FrameTiming.FrameRate.NumUnitsInTick PRN_RENC_ARG_POST
    PRN_RENC_API

    PRN_RENC_LOG "---- %sHdrInfo%s ----"
        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_TITLE_1 PRN_RENC_ARG_POST
        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END     PRN_RENC_ARG_POST
    PRN_RENC_API

    PRN_RENC_LOG "  {" PRN_RENC_API
    PRN_RENC_LOG "     .HdrType = %s,"
        PRN_RENC_ARG_CSTR   SensorHdrStr[pHdrInfo->HdrType] PRN_RENC_ARG_POST
    PRN_RENC_API
    PRN_RENC_LOG "     .ActiveChannels = %d,"
        PRN_RENC_ARG_UINT32 pHdrInfo->ActiveChannels PRN_RENC_ARG_POST
    PRN_RENC_API
    PRN_RENC_LOG "     .ChannelInfo = {" PRN_RENC_API
    for (Idx = 0U; Idx < AMBA_SENSOR_NUM_MAX_HDR_CHAN; Idx ++) {
        PRN_RENC_LOG "       [%d] = {"
            PRN_RENC_ARG_UINT32 Idx PRN_RENC_ARG_POST
        PRN_RENC_API
        PRN_RENC_LOG "         .EffectiveArea = { %d, %d, %d, %d }"
            PRN_RENC_ARG_UINT32 pHdrInfo->ChannelInfo[Idx].EffectiveArea.StartX PRN_RENC_ARG_POST
            PRN_RENC_ARG_UINT32 pHdrInfo->ChannelInfo[Idx].EffectiveArea.StartY PRN_RENC_ARG_POST
            PRN_RENC_ARG_UINT32 pHdrInfo->ChannelInfo[Idx].EffectiveArea.Width  PRN_RENC_ARG_POST
            PRN_RENC_ARG_UINT32 pHdrInfo->ChannelInfo[Idx].EffectiveArea.Height PRN_RENC_ARG_POST
        PRN_RENC_API
        PRN_RENC_LOG "         .OpticalBlackPixels = { %d, %d, %d, %d }"
            PRN_RENC_ARG_UINT32 pHdrInfo->ChannelInfo[Idx].OpticalBlackPixels.StartX PRN_RENC_ARG_POST
            PRN_RENC_ARG_UINT32 pHdrInfo->ChannelInfo[Idx].OpticalBlackPixels.StartY PRN_RENC_ARG_POST
            PRN_RENC_ARG_UINT32 pHdrInfo->ChannelInfo[Idx].OpticalBlackPixels.Width  PRN_RENC_ARG_POST
            PRN_RENC_ARG_UINT32 pHdrInfo->ChannelInfo[Idx].OpticalBlackPixels.Height PRN_RENC_ARG_POST
        PRN_RENC_API
        PRN_RENC_LOG "         .ShutterSpeedCtrlInfo = { AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, %d, %d, %d }"
            PRN_RENC_ARG_UINT32 pHdrInfo->ChannelInfo[Idx].ShutterSpeedCtrlInfo.FirstReflectedFrame PRN_RENC_ARG_POST
            PRN_RENC_ARG_UINT32 pHdrInfo->ChannelInfo[Idx].ShutterSpeedCtrlInfo.FirstBadFrame       PRN_RENC_ARG_POST
            PRN_RENC_ARG_UINT32 pHdrInfo->ChannelInfo[Idx].ShutterSpeedCtrlInfo.NumBadFrames        PRN_RENC_ARG_POST
        PRN_RENC_API
        PRN_RENC_LOG "         .OutputFormatCtrlInfo = { AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, %d, %d, %d }"
            PRN_RENC_ARG_UINT32 pHdrInfo->ChannelInfo[Idx].OutputFormatCtrlInfo.FirstReflectedFrame PRN_RENC_ARG_POST
            PRN_RENC_ARG_UINT32 pHdrInfo->ChannelInfo[Idx].OutputFormatCtrlInfo.FirstBadFrame       PRN_RENC_ARG_POST
            PRN_RENC_ARG_UINT32 pHdrInfo->ChannelInfo[Idx].OutputFormatCtrlInfo.NumBadFrames        PRN_RENC_ARG_POST
        PRN_RENC_API
        PRN_RENC_LOG "         .MaxExposureLine = %d"
            PRN_RENC_ARG_UINT32 pHdrInfo->ChannelInfo[Idx].MaxExposureLine PRN_RENC_ARG_POST
        PRN_RENC_API
        PRN_RENC_LOG "         .MinExposureLine = %d"
            PRN_RENC_ARG_UINT32 pHdrInfo->ChannelInfo[Idx].MinExposureLine PRN_RENC_ARG_POST
        PRN_RENC_API
        PRN_RENC_LOG "       }," PRN_RENC_API
    }
    PRN_RENC_LOG "     }," PRN_RENC_API

    PRN_RENC_LOG " " PRN_RENC_API

    AmbaMisra_TouchUnused(pInputInfo   );
    AmbaMisra_TouchUnused(pOutputInfo  );
    AmbaMisra_TouchUnused(pModeInfo    );
    AmbaMisra_TouchUnused(pHdrInfo     );
}

static void SvcRawEnc_ResCfgPrint(SVC_RES_CFG_s *pCfg)
{
    if (pCfg != NULL) {
        UINT32 VinCfgIdx = RawEncResCfgVinID, SubChanIdx;
        UINT32 SsCfgIdx;
        UINT32 FovIdx;
        UINT32 StrmIdx, ChanIdx;

        PRN_RENC_LOG " " PRN_RENC_API
        PRN_RENC_LOG "====== %sRaw Encode Resolution Configuration (%p)%s ======"
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_TITLE_0 PRN_RENC_ARG_POST
            PRN_RENC_ARG_CPOINT pCfg                    PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END     PRN_RENC_ARG_POST
        PRN_RENC_API

        PRN_RENC_LOG ".VinTree : %s%s%s"
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_STR PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR   pCfg->VinTree       PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END PRN_RENC_ARG_POST
        PRN_RENC_API

        PRN_RENC_LOG "//---- %sVIN%s ----"
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_TITLE_1 PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END     PRN_RENC_ARG_POST
        PRN_RENC_API
        PRN_RENC_LOG ".VinNum : %s%d%s"
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM PRN_RENC_ARG_POST
            PRN_RENC_ARG_UINT32 pCfg->VinNum        PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END PRN_RENC_ARG_POST
        PRN_RENC_API

        {

            PRN_RENC_LOG ".VinCfg[%s%d%s] = {"
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 VinCfgIdx           PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END PRN_RENC_ARG_POST
            PRN_RENC_API
            PRN_RENC_LOG "  .pDriver : %s%p%s"
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM             PRN_RENC_ARG_POST
                PRN_RENC_ARG_CPOINT pCfg->VinCfg[VinCfgIdx].pDriver PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END             PRN_RENC_ARG_POST
            PRN_RENC_API
            PRN_RENC_LOG "  .SerdesType : %s%d%s"
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->VinCfg[VinCfgIdx].SerdesType PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                PRN_RENC_ARG_POST
            PRN_RENC_API
            PRN_RENC_LOG "  .CapWin = { .OffsetX = %s%d%s, .OffsetY = %s%d%s, .Width = %s%d%s, .Height = %s%d%s },"
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->VinCfg[VinCfgIdx].CapWin.OffsetX PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                    PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->VinCfg[VinCfgIdx].CapWin.OffsetY PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                    PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->VinCfg[VinCfgIdx].CapWin.Width   PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                    PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->VinCfg[VinCfgIdx].CapWin.Height  PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                    PRN_RENC_ARG_POST
            PRN_RENC_API
            PRN_RENC_LOG "  .SubChanNum : %s%d%s"
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->VinCfg[VinCfgIdx].SubChanNum PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                PRN_RENC_ARG_POST
            PRN_RENC_API

            for (SubChanIdx = 0U; SubChanIdx < pCfg->VinCfg[VinCfgIdx].SubChanNum; SubChanIdx ++) {

                PRN_RENC_LOG "  .SubChanCfg[%s%d%s] = {"
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->VinCfg[VinCfgIdx].SubChanNum PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                PRN_RENC_ARG_POST
                PRN_RENC_API

                PRN_RENC_LOG "    .SubChan = { .IsVirtual = %s%d%s, .Index = %s%d%s },"
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                              PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->VinCfg[VinCfgIdx].SubChanCfg[SubChanIdx].SubChan.IsVirtual PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                              PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                              PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->VinCfg[VinCfgIdx].SubChanCfg[SubChanIdx].SubChan.Index     PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                              PRN_RENC_ARG_POST
                PRN_RENC_API
                PRN_RENC_LOG "    .Option = %s%d%s,"
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                   PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->VinCfg[VinCfgIdx].SubChanCfg[SubChanIdx].Option PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                   PRN_RENC_ARG_POST
                PRN_RENC_API
                PRN_RENC_LOG "    .IntcNum = %s%d%s,"
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                    PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->VinCfg[VinCfgIdx].SubChanCfg[SubChanIdx].IntcNum PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                    PRN_RENC_ARG_POST
                PRN_RENC_API
                PRN_RENC_LOG "    .CaptureWindow = { .OffsetX = %s%d%s, .OffsetY = %s%d%s, .Width = %s%d%s, .Height = %s%d%s },"
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                                  PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->VinCfg[VinCfgIdx].SubChanCfg[SubChanIdx].CaptureWindow.OffsetX PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                                  PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                                  PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->VinCfg[VinCfgIdx].SubChanCfg[SubChanIdx].CaptureWindow.OffsetY PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                                  PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                                  PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->VinCfg[VinCfgIdx].SubChanCfg[SubChanIdx].CaptureWindow.Width   PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                                  PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                                  PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->VinCfg[VinCfgIdx].SubChanCfg[SubChanIdx].CaptureWindow.Height  PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                                  PRN_RENC_ARG_POST
                PRN_RENC_API

                PRN_RENC_LOG "  }," PRN_RENC_API
            }

            PRN_RENC_LOG "  .FrameRate = { .TimeScale = %s%d%s, .NumUnitsInTick = %s%d%s, },"
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                              PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->VinCfg[VinCfgIdx].FrameRate.TimeScale      PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                              PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                              PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->VinCfg[VinCfgIdx].FrameRate.NumUnitsInTick PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                              PRN_RENC_ARG_POST
            PRN_RENC_API

            PRN_RENC_LOG "}," PRN_RENC_API

        }

        PRN_RENC_LOG "//---- %sSENSOR%s ----"
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_TITLE_1 PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END     PRN_RENC_ARG_POST
        PRN_RENC_API
        {
            UINT32 SsIdxs[4U], SsNum = 0U;

            AmbaSvcWrap_MisraMemset(SsIdxs, 0, sizeof(SsIdxs));
            if (0U != SvcResCfg_GetSensorIdxsInVinID(VinCfgIdx, SsIdxs, &SsNum)) {
                PRN_RENC_LOG "Fail to get VinID(%d) sensor index"
                    PRN_RENC_ARG_UINT32 VinCfgIdx PRN_RENC_ARG_POST
                PRN_RENC_NG
            } else {
                PRN_RENC_LOG ".SensorCfg[%s%d%s] = {        // current sensor index number %s%d%s"
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 VinCfgIdx           PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 SsNum               PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END PRN_RENC_ARG_POST
                PRN_RENC_API

                if (SsNum == 0U) {
                    PRN_RENC_LOG "                         // change to 1 for video raw enc res_cfg info dump" PRN_RENC_API
                    SsNum = 1U;
                }

                for (SsCfgIdx = 0U; SsCfgIdx < SsNum; SsCfgIdx ++) {
                    PRN_RENC_LOG "  [%s%d%s] = {                // no.%d"
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 SsIdxs[SsCfgIdx]    PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 SsCfgIdx            PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END PRN_RENC_ARG_POST
                    PRN_RENC_API

                    PRN_RENC_LOG "    .SensorMode = %s%d%s,"
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                     PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 pCfg->SensorCfg[VinCfgIdx][SsIdxs[SsCfgIdx]].SensorMode PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                     PRN_RENC_ARG_POST
                    PRN_RENC_API
                    PRN_RENC_LOG "    .SensorGroup = %s%d%s,"
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                      PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 pCfg->SensorCfg[VinCfgIdx][SsIdxs[SsCfgIdx]].SensorGroup PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                      PRN_RENC_ARG_POST
                    PRN_RENC_API
                    PRN_RENC_LOG "    .IQTable = %s0x%x%s,"
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                  PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 pCfg->SensorCfg[VinCfgIdx][SsIdxs[SsCfgIdx]].IQTable PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                  PRN_RENC_ARG_POST
                    PRN_RENC_API

                    PRN_RENC_LOG "  }," PRN_RENC_API
                }

                PRN_RENC_LOG "}," PRN_RENC_API
            }
        }

        PRN_RENC_LOG "//---- %sFOV%s ----"
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_TITLE_1 PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END     PRN_RENC_ARG_POST
        PRN_RENC_API
        PRN_RENC_LOG ".FovNum : %s%d%s"
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM PRN_RENC_ARG_POST
            PRN_RENC_ARG_UINT32 pCfg->FovNum        PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END PRN_RENC_ARG_POST
        PRN_RENC_API

        for (FovIdx = 0U; FovIdx < pCfg->FovNum; FovIdx ++) {

            PRN_RENC_LOG ".FovCfg[%s%d%s] = {"
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 FovIdx              PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END PRN_RENC_ARG_POST
            PRN_RENC_API

            PRN_RENC_LOG "  .RawWin = { %s%d%s, %s%d%s, %s%d%s, %s%d%s },"
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                 PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->FovCfg[FovIdx].RawWin.OffsetX PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                 PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                 PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->FovCfg[FovIdx].RawWin.OffsetY PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                 PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                 PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->FovCfg[FovIdx].RawWin.Width   PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                 PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                 PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->FovCfg[FovIdx].RawWin.Height  PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                 PRN_RENC_ARG_POST
            PRN_RENC_API

            PRN_RENC_LOG "  .ActWin = { %s%d%s, %s%d%s, %s%d%s, %s%d%s },"
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                 PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->FovCfg[FovIdx].ActWin.OffsetX PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                 PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                 PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->FovCfg[FovIdx].ActWin.OffsetY PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                 PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                 PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->FovCfg[FovIdx].ActWin.Width   PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                 PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                 PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->FovCfg[FovIdx].ActWin.Height  PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                 PRN_RENC_ARG_POST
            PRN_RENC_API

            PRN_RENC_LOG "  .MainWin = { %s%d%s, %s%d%s, },"
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                 PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->FovCfg[FovIdx].MainWin.Width  PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                 PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                 PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->FovCfg[FovIdx].MainWin.Height PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                 PRN_RENC_ARG_POST
            PRN_RENC_API

            PRN_RENC_LOG "  .PyramidBits = %s0x%x%s, .HierWin = { %s%d%s, %s%d%s, },"
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                 PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->FovCfg[FovIdx].PyramidBits    PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                 PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                 PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->FovCfg[FovIdx].HierWin.Width  PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                 PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                 PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->FovCfg[FovIdx].HierWin.Height PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                 PRN_RENC_ARG_POST
            PRN_RENC_API

            PRN_RENC_LOG "  .PipeCfg = { .RotateFlip = %s%d%s, .HdrExposureNum = %s%d%s, .HdrBlendHieght = %s%d%s, .MctfDisable = %s%d%s, .MctsDisable = %s%d%s, .LinearCE = %s%d%s, "
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                         PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->FovCfg[FovIdx].PipeCfg.RotateFlip     PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                         PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                         PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->FovCfg[FovIdx].PipeCfg.HdrExposureNum PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                         PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                         PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->FovCfg[FovIdx].PipeCfg.HdrBlendHieght PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                         PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                         PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->FovCfg[FovIdx].PipeCfg.MctfDisable    PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                         PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                         PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->FovCfg[FovIdx].PipeCfg.MctsDisable    PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                         PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                         PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->FovCfg[FovIdx].PipeCfg.LinearCE       PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                         PRN_RENC_ARG_POST
            PRN_RENC_API

            PRN_RENC_LOG "               .RawCompression = %s%d%s, .ViewCtrlOperation = %s%d%s, .LowDelayParams = { %s%d%s, %s%d%s, %s%d%s }, .PipeMode = %s%d%s }"
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                            PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->FovCfg[FovIdx].PipeCfg.RawCompression    PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                            PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                            PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->FovCfg[FovIdx].PipeCfg.ViewCtrlOperation PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                            PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                            PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->FovCfg[FovIdx].PipeCfg.LowDelayParams[0] PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                            PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                            PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->FovCfg[FovIdx].PipeCfg.LowDelayParams[1] PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                            PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                            PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->FovCfg[FovIdx].PipeCfg.LowDelayParams[2] PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                            PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                            PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->FovCfg[FovIdx].PipeCfg.PipeMode          PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                            PRN_RENC_ARG_POST
            PRN_RENC_API

            PRN_RENC_LOG "}," PRN_RENC_API
        }

        PRN_RENC_LOG "//---- %sDISPLAY%s ----"
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_TITLE_1 PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END     PRN_RENC_ARG_POST
        PRN_RENC_API
        PRN_RENC_LOG ".DispNum = %s%d%s, .DispBits = %s0x%x%s,"
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM PRN_RENC_ARG_POST
            PRN_RENC_ARG_UINT32 pCfg->DispNum       PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM PRN_RENC_ARG_POST
            PRN_RENC_ARG_UINT32 pCfg->DispBits      PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END PRN_RENC_ARG_POST
        PRN_RENC_API

        for (StrmIdx = 0U; StrmIdx < AMBA_DSP_MAX_VOUT_NUM; StrmIdx ++ ) {

            if (pCfg->DispStrm[StrmIdx].StrmCfg.Win.Width > 0U) {

                PRN_RENC_LOG ".DispStrm[%s%d%s] = {"
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 StrmIdx             PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END PRN_RENC_ARG_POST
                PRN_RENC_API

                PRN_RENC_LOG "  .VoutID = %s%d%s,"
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM            PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->DispStrm[StrmIdx].VoutID PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END            PRN_RENC_ARG_POST
                PRN_RENC_API
                PRN_RENC_LOG "  .pDriver = %s%p%s,"
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM             PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CPOINT pCfg->DispStrm[StrmIdx].pDriver PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END             PRN_RENC_ARG_POST
                PRN_RENC_API
                PRN_RENC_LOG "  .VideoRotateFlip = %s%d%s,"
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                     PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->DispStrm[StrmIdx].VideoRotateFlip PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                     PRN_RENC_ARG_POST
                PRN_RENC_API

                PRN_RENC_LOG "  .StrmCfg = {" PRN_RENC_API

                PRN_RENC_LOG "    .Win = { %s%d%s, %s%d%s }, .MaxWin = { %s%d%s, %s%d%s }, .NumChan = %s%d%s,"
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                           PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->DispStrm[StrmIdx].StrmCfg.Win.Width     PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                           PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                           PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->DispStrm[StrmIdx].StrmCfg.Win.Height    PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                           PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                           PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->DispStrm[StrmIdx].StrmCfg.MaxWin.Width  PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                           PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                           PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->DispStrm[StrmIdx].StrmCfg.MaxWin.Height PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                           PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                           PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->DispStrm[StrmIdx].StrmCfg.NumChan       PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                           PRN_RENC_ARG_POST
                PRN_RENC_API

                for (ChanIdx = 0U; ChanIdx < pCfg->DispStrm[StrmIdx].StrmCfg.NumChan; ChanIdx ++) {

                    PRN_RENC_LOG "    .ChanCfg[%s%d%s] = {"
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 ChanIdx             PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END PRN_RENC_ARG_POST
                    PRN_RENC_API

                    PRN_RENC_LOG "      .FovId = %s%d%s, .SrcWin = { %s%d%s, %s%d%s, %s%d%s, %s%d%s }, .DstWin = { %s%d%s, %s%d%s, %s%d%s, %s%d%s }, "
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                             PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 pCfg->DispStrm[StrmIdx].StrmCfg.ChanCfg[ChanIdx].FovId          PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                             PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                             PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 pCfg->DispStrm[StrmIdx].StrmCfg.ChanCfg[ChanIdx].SrcWin.OffsetX PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                             PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                             PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 pCfg->DispStrm[StrmIdx].StrmCfg.ChanCfg[ChanIdx].SrcWin.OffsetY PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                             PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                             PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 pCfg->DispStrm[StrmIdx].StrmCfg.ChanCfg[ChanIdx].SrcWin.Width   PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                             PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                             PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 pCfg->DispStrm[StrmIdx].StrmCfg.ChanCfg[ChanIdx].SrcWin.Height  PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                             PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                             PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 pCfg->DispStrm[StrmIdx].StrmCfg.ChanCfg[ChanIdx].DstWin.OffsetX PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                             PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                             PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 pCfg->DispStrm[StrmIdx].StrmCfg.ChanCfg[ChanIdx].DstWin.OffsetY PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                             PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                             PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 pCfg->DispStrm[StrmIdx].StrmCfg.ChanCfg[ChanIdx].DstWin.Width   PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                             PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                             PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 pCfg->DispStrm[StrmIdx].StrmCfg.ChanCfg[ChanIdx].DstWin.Height  PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                             PRN_RENC_ARG_POST
                    PRN_RENC_API

                    PRN_RENC_LOG "    }," PRN_RENC_API
                }

                PRN_RENC_LOG "  }," PRN_RENC_API

                PRN_RENC_LOG "  .DevMode = %s%d%s,"
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM             PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->DispStrm[StrmIdx].DevMode PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END             PRN_RENC_ARG_POST
                PRN_RENC_API

                PRN_RENC_LOG "  .FrameRate = { .TimeScale = %s%d%s, .NumUnitsInTick = %s%d%s, },"
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                              PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->DispStrm[StrmIdx].FrameRate.TimeScale      PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                              PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                              PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->DispStrm[StrmIdx].FrameRate.NumUnitsInTick PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                              PRN_RENC_ARG_POST
                PRN_RENC_API

                PRN_RENC_LOG "}," PRN_RENC_API

            }
        }

        PRN_RENC_LOG "//---- %sRECORD%s ----"
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_TITLE_1 PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END     PRN_RENC_ARG_POST
        PRN_RENC_API
        PRN_RENC_LOG ".RecNum = %s%d%s, .RecBits = %s0x%x%s,"
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM PRN_RENC_ARG_POST
            PRN_RENC_ARG_UINT32 pCfg->RecNum        PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM PRN_RENC_ARG_POST
            PRN_RENC_ARG_UINT32 pCfg->RecBits       PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END PRN_RENC_ARG_POST
        PRN_RENC_API

        for (StrmIdx = 0U; StrmIdx < AMBA_DSP_MAX_VOUT_NUM; StrmIdx ++ ) {
            if (pCfg->RecStrm[StrmIdx].StrmCfg.Win.Width > 0U) {

                PRN_RENC_LOG ".RecStrm[%s%d%s] = {"
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 StrmIdx             PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END PRN_RENC_ARG_POST
                PRN_RENC_API

                PRN_RENC_LOG "  .RecSetting = {" PRN_RENC_API

                PRN_RENC_LOG "    .SrcBits = %s0x%x%s"
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                       PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->RecStrm[StrmIdx].RecSetting.SrcBits PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                       PRN_RENC_ARG_POST
                PRN_RENC_API

                PRN_RENC_LOG "    .DestBits = %s0x%x%s"
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                        PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->RecStrm[StrmIdx].RecSetting.DestBits PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                        PRN_RENC_ARG_POST
                PRN_RENC_API

                PRN_RENC_LOG "    .RecId = %s%d%s, .M = %s%d%s, .N = %s%d%s, .IdrInterval = %s%d%s,"
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                           PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->RecStrm[StrmIdx].RecSetting.RecId       PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                           PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                           PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->RecStrm[StrmIdx].RecSetting.M           PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                           PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                           PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->RecStrm[StrmIdx].RecSetting.N           PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                           PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                           PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->RecStrm[StrmIdx].RecSetting.IdrInterval PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                           PRN_RENC_ARG_POST
                PRN_RENC_API

                PRN_RENC_LOG "  .FrameRate = { .TimeScale = %s%d%s, .NumUnitsInTick = %s%d%s, .Interlace = %s%d%s },"
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                        PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->RecStrm[StrmIdx].RecSetting.FrameRate.TimeScale      PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                        PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                        PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->RecStrm[StrmIdx].RecSetting.FrameRate.NumUnitsInTick PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                        PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                        PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->RecStrm[StrmIdx].RecSetting.FrameRate.Interlace      PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                        PRN_RENC_ARG_POST
                PRN_RENC_API

                PRN_RENC_LOG "  }," PRN_RENC_API

                PRN_RENC_LOG "  .StrmCfg = {" PRN_RENC_API

                PRN_RENC_LOG "    .Win = { %s%d%s, %s%d%s }, .MaxWin = { %s%d%s, %s%d%s }, .NumChan = %s%d%s,"
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                          PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->RecStrm[StrmIdx].StrmCfg.Win.Width     PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                          PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                          PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->RecStrm[StrmIdx].StrmCfg.Win.Height    PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                          PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                          PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->RecStrm[StrmIdx].StrmCfg.MaxWin.Width  PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                          PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                          PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->RecStrm[StrmIdx].StrmCfg.MaxWin.Height PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                          PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                          PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 pCfg->RecStrm[StrmIdx].StrmCfg.NumChan       PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                          PRN_RENC_ARG_POST
                PRN_RENC_API

                for (ChanIdx = 0U; ChanIdx < pCfg->RecStrm[StrmIdx].StrmCfg.NumChan; ChanIdx ++) {

                    PRN_RENC_LOG "    .ChanCfg[%s%d%s] = {"
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 ChanIdx             PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END PRN_RENC_ARG_POST
                    PRN_RENC_API

                    PRN_RENC_LOG "      .FovId = %s%d%s, .SrcWin = { %s%d%s, %s%d%s, %s%d%s, %s%d%s }, .DstWin = { %s%d%s, %s%d%s, %s%d%s, %s%d%s }, "
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                            PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 pCfg->RecStrm[StrmIdx].StrmCfg.ChanCfg[ChanIdx].FovId          PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                            PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                            PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 pCfg->RecStrm[StrmIdx].StrmCfg.ChanCfg[ChanIdx].SrcWin.OffsetX PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                            PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                            PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 pCfg->RecStrm[StrmIdx].StrmCfg.ChanCfg[ChanIdx].SrcWin.OffsetY PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                            PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                            PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 pCfg->RecStrm[StrmIdx].StrmCfg.ChanCfg[ChanIdx].SrcWin.Width   PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                            PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                            PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 pCfg->RecStrm[StrmIdx].StrmCfg.ChanCfg[ChanIdx].SrcWin.Height  PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                            PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                            PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 pCfg->RecStrm[StrmIdx].StrmCfg.ChanCfg[ChanIdx].DstWin.OffsetX PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                            PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                            PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 pCfg->RecStrm[StrmIdx].StrmCfg.ChanCfg[ChanIdx].DstWin.OffsetY PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                            PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                            PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 pCfg->RecStrm[StrmIdx].StrmCfg.ChanCfg[ChanIdx].DstWin.Width   PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                            PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_NUM                                            PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 pCfg->RecStrm[StrmIdx].StrmCfg.ChanCfg[ChanIdx].DstWin.Height  PRN_RENC_ARG_POST
                        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END                                            PRN_RENC_ARG_POST
                    PRN_RENC_API

                    PRN_RENC_LOG "    }," PRN_RENC_API
                }

                PRN_RENC_LOG "  }," PRN_RENC_API

                PRN_RENC_LOG "}," PRN_RENC_API
            }
        }

        PRN_RENC_LOG "====== %sRaw Encode Resolution Configuration End%s ======"
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_TITLE_0 PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END     PRN_RENC_ARG_POST
        PRN_RENC_API

        AmbaMisra_TouchUnused(pCfg);
    }
}

static void SvcRawEnc_CfgPrint(void)
{
    if ((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_INIT) == 0U) {
        PRN_RENC_LOG "Fail to print resolution cfg - create raw encode module first!" PRN_RENC_NG
    } else {
        UINT32 Idx;
        const SVC_RAW_ENC_CTRL_s *pCfg = &(SVC_RawEncCtrl);

        PRN_RENC_LOG " " PRN_RENC_API
        PRN_RENC_LOG "====== %sRaw Encode Configuration (%p)%s ======"
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_TITLE_0 PRN_RENC_ARG_POST
            PRN_RENC_ARG_CPOINT pCfg                    PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END     PRN_RENC_ARG_POST
        PRN_RENC_API
        PRN_RENC_LOG "  Name          : %s"
            PRN_RENC_ARG_CSTR   pCfg->Name PRN_RENC_ARG_POST
        PRN_RENC_API
        PRN_RENC_LOG "  MemBase       : %p"
            PRN_RENC_ARG_CPOINT pCfg->pMemBuf PRN_RENC_ARG_POST
        PRN_RENC_API
        PRN_RENC_LOG "  MemSize       : 0x%08x"
            PRN_RENC_ARG_UINT32 pCfg->MemSize PRN_RENC_ARG_POST
        PRN_RENC_API
        PRN_RENC_LOG "  ItnMemBase    : %p"
            PRN_RENC_ARG_CPOINT pCfg->pItnMemBuf PRN_RENC_ARG_POST
        PRN_RENC_API
        PRN_RENC_LOG "  ItnMemSize    : 0x%08x"
            PRN_RENC_ARG_UINT32 pCfg->ItnMemSize PRN_RENC_ARG_POST
        PRN_RENC_API
        PRN_RENC_LOG "  VinSelectBits : 0x%x"
            PRN_RENC_ARG_UINT32 pCfg->VinSelectBits PRN_RENC_ARG_POST
        PRN_RENC_API
        PRN_RENC_LOG "  ---- %sVinCfg%s ----"
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_TITLE_1 PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END     PRN_RENC_ARG_POST
        PRN_RENC_API
        PRN_RENC_LOG "%s    VinID Status     RawFilePath%s"
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_TITLE_2 PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END     PRN_RENC_ARG_POST
        PRN_RENC_API
        for (Idx = 0U; Idx < AMBA_DSP_MAX_VIN_NUM; Idx ++) {
            PRN_RENC_LOG "      %02d  0x%08x %s"
                PRN_RENC_ARG_UINT32 Idx                           PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->VinCfg[Idx].Status      PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   pCfg->VinCfg[Idx].RawFilePath PRN_RENC_ARG_POST
            PRN_RENC_API
        }
        PRN_RENC_LOG "%s                     HdsFilePath%s"
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_TITLE_2 PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END     PRN_RENC_ARG_POST
        PRN_RENC_API
        for (Idx = 0U; Idx < AMBA_DSP_MAX_VIN_NUM; Idx ++) {
            PRN_RENC_LOG "                     %s"
                PRN_RENC_ARG_CSTR   pCfg->VinCfg[Idx].HdsFilePath PRN_RENC_ARG_POST
            PRN_RENC_API
        }
        PRN_RENC_LOG "  ViewZoneSelectBits : 0x%x"
            PRN_RENC_ARG_UINT32 pCfg->ViewZoneSelectBits PRN_RENC_ARG_POST
        PRN_RENC_API
        PRN_RENC_LOG "  ---- %sViewZoneCfg%s ----"
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_TITLE_1 PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END     PRN_RENC_ARG_POST
        PRN_RENC_API
        PRN_RENC_LOG "%s    ViewZoneID Status     ItnFilePath%s"
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_TITLE_2 PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END     PRN_RENC_ARG_POST
        PRN_RENC_API
        for (Idx = 0U; Idx < AMBA_DSP_MAX_VIEWZONE_NUM; Idx ++) {
            PRN_RENC_LOG "      %02d       0x%08x %s"
                PRN_RENC_ARG_UINT32 Idx                                PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCfg->ViewZoneCfg[Idx].Status      PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   pCfg->ViewZoneCfg[Idx].ItnFilePath PRN_RENC_ARG_POST
            PRN_RENC_API
        }
        PRN_RENC_LOG "  ViewZoneIdspBits : 0x%x"
            PRN_RENC_ARG_UINT32 pCfg->ViewZoneIdspBits PRN_RENC_ARG_POST
        PRN_RENC_API
    }
}

static void SvcRawEnc_ProcCallBack(UINT32 Code, void *pData)
{
    UINT32 PRetVal;

    if (RawEncCallBackProc != NULL) {
        const UINT32 *pU32 = &Code;
        UINT32 TimeStampStart = 0U, TimeStampEnd = 0U, TimeStampDiff;

        AmbaMisra_TypeCast(&(pU32), &(pData));

        PRetVal = (pU32 == NULL)?0U:(*pU32);
        PRN_RENC_LOG "Trigger raw enc callback. Code(0x%08x) pData(%p/%d)"
            PRN_RENC_ARG_UINT32 Code    PRN_RENC_ARG_POST
            PRN_RENC_ARG_CPOINT pU32    PRN_RENC_ARG_POST
            PRN_RENC_ARG_UINT32 PRetVal PRN_RENC_ARG_POST
        PRN_RENC_DBG

        PRetVal = AmbaKAL_GetSysTickCount(&TimeStampStart); PRN_RENC_ERR_HDLR
        (RawEncCallBackProc)(Code, pData);
        PRetVal = AmbaKAL_GetSysTickCount(&TimeStampEnd); PRN_RENC_ERR_HDLR

        if (TimeStampEnd >= TimeStampStart) {
            TimeStampDiff = TimeStampEnd - TimeStampStart;
        } else {
            TimeStampDiff = ( 0xFFFFFFFFU - TimeStampStart ) + TimeStampEnd;
        }

        PRetVal = (pU32 == NULL)?0U:(*pU32);
        PRN_RENC_LOG "Trigger raw enc callback done. Code(0x%08x) pData(%p/%d) SpendTime(%d)"
            PRN_RENC_ARG_UINT32 Code          PRN_RENC_ARG_POST
            PRN_RENC_ARG_CPOINT pU32          PRN_RENC_ARG_POST
            PRN_RENC_ARG_UINT32 PRetVal       PRN_RENC_ARG_POST
            PRN_RENC_ARG_UINT32 TimeStampDiff PRN_RENC_ARG_POST
        PRN_RENC_DBG
    }
}

static SVC_RAW_ENC_DMY_SEN_CFG_s *SvcRawEnc_DmySenCfgGet(void)
{
    SvcRawEnc_DefDmySenCfgInit();
    return &RawEncDmySenCfg;
}

static void SvcRawEnc_VoutDevInit(void)
{
    if ((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_DEF_VOUT_DEV) == 0U) {
        UINT32 VoutDevNum = 0U;
        AmbaSvcWrap_MisraMemset(RawEncVoutDevInfo, 255, sizeof(RawEncVoutDevInfo));

        {
            extern AMBA_FPD_OBJECT_s AmbaFPD_HDMI_Obj GNU_WEAK_SYMBOL;
            RawEncVoutDevInfo[VoutDevNum].pObj = &AmbaFPD_HDMI_Obj;
            RawEncVoutDevInfo[VoutDevNum].ID   = VoutDevNum;
            if (RawEncVoutDevInfo[VoutDevNum].pObj != NULL) {
                SvcWrap_strcpy(RawEncVoutDevInfo[VoutDevNum].DevName, sizeof(RawEncVoutDevInfo[VoutDevNum].DevName), RawEncVoutDevInfo[VoutDevNum].pObj->pName);
            } else {
                SvcWrap_strcpy(RawEncVoutDevInfo[VoutDevNum].DevName, sizeof(RawEncVoutDevInfo[VoutDevNum].DevName), "HDMI");
            }
            VoutDevNum ++;
        }

        {
            extern AMBA_FPD_OBJECT_s AmbaFPD_T30P61Obj GNU_WEAK_SYMBOL;
            RawEncVoutDevInfo[VoutDevNum].pObj = &AmbaFPD_T30P61Obj;
            RawEncVoutDevInfo[VoutDevNum].ID   = VoutDevNum;
            if (RawEncVoutDevInfo[VoutDevNum].pObj != NULL) {
                SvcWrap_strcpy(RawEncVoutDevInfo[VoutDevNum].DevName, sizeof(RawEncVoutDevInfo[VoutDevNum].DevName), RawEncVoutDevInfo[VoutDevNum].pObj->pName);
            } else {
                SvcWrap_strcpy(RawEncVoutDevInfo[VoutDevNum].DevName, sizeof(RawEncVoutDevInfo[VoutDevNum].DevName), "T30P61");
            }
            VoutDevNum ++;
        }

        {
            extern AMBA_FPD_OBJECT_s AmbaFPD_MAXIM_TFT128Obj GNU_WEAK_SYMBOL;
            RawEncVoutDevInfo[VoutDevNum].pObj = &AmbaFPD_MAXIM_TFT128Obj;
            RawEncVoutDevInfo[VoutDevNum].ID   = VoutDevNum;
            if (RawEncVoutDevInfo[VoutDevNum].pObj != NULL) {
                SvcWrap_strcpy(RawEncVoutDevInfo[VoutDevNum].DevName, sizeof(RawEncVoutDevInfo[VoutDevNum].DevName), RawEncVoutDevInfo[VoutDevNum].pObj->pName);
            } else {
                SvcWrap_strcpy(RawEncVoutDevInfo[VoutDevNum].DevName, sizeof(RawEncVoutDevInfo[VoutDevNum].DevName), "MAXIM_TFT128");
            }
            VoutDevNum ++;
        }

        {
            extern AMBA_FPD_OBJECT_s AmbaFPD_MAXIM_Dual_TFT128Obj GNU_WEAK_SYMBOL;
            RawEncVoutDevInfo[VoutDevNum].pObj = &AmbaFPD_MAXIM_Dual_TFT128Obj;
            RawEncVoutDevInfo[VoutDevNum].ID   = VoutDevNum;
            if (RawEncVoutDevInfo[VoutDevNum].pObj != NULL) {
                SvcWrap_strcpy(RawEncVoutDevInfo[VoutDevNum].DevName, sizeof(RawEncVoutDevInfo[VoutDevNum].DevName), RawEncVoutDevInfo[VoutDevNum].pObj->pName);
            } else {
                SvcWrap_strcpy(RawEncVoutDevInfo[VoutDevNum].DevName, sizeof(RawEncVoutDevInfo[VoutDevNum].DevName), "MAXIM_Dual_TFT128");
            }
            VoutDevNum ++;
        }

        {
            extern AMBA_FPD_OBJECT_s AmbaFPD_MAXIM_ZS095Obj GNU_WEAK_SYMBOL;
            RawEncVoutDevInfo[VoutDevNum].pObj = &AmbaFPD_MAXIM_ZS095Obj;
            RawEncVoutDevInfo[VoutDevNum].ID   = VoutDevNum;
            if (RawEncVoutDevInfo[VoutDevNum].pObj != NULL) {
                SvcWrap_strcpy(RawEncVoutDevInfo[VoutDevNum].DevName, sizeof(RawEncVoutDevInfo[VoutDevNum].DevName), RawEncVoutDevInfo[VoutDevNum].pObj->pName);
            } else {
                SvcWrap_strcpy(RawEncVoutDevInfo[VoutDevNum].DevName, sizeof(RawEncVoutDevInfo[VoutDevNum].DevName), "MAXIM_ZS095");
            }
            VoutDevNum ++;
        }

        {
            extern AMBA_FPD_OBJECT_s AmbaFPD_MAXIM_Dual_ZS095BHObj GNU_WEAK_SYMBOL;
            RawEncVoutDevInfo[VoutDevNum].pObj = &AmbaFPD_MAXIM_Dual_ZS095BHObj;
            RawEncVoutDevInfo[VoutDevNum].ID   = VoutDevNum;
            if (RawEncVoutDevInfo[VoutDevNum].pObj != NULL) {
                SvcWrap_strcpy(RawEncVoutDevInfo[VoutDevNum].DevName, sizeof(RawEncVoutDevInfo[VoutDevNum].DevName), RawEncVoutDevInfo[VoutDevNum].pObj->pName);
            } else {
                SvcWrap_strcpy(RawEncVoutDevInfo[VoutDevNum].DevName, sizeof(RawEncVoutDevInfo[VoutDevNum].DevName), "MAXIM_Dual_ZS095");
            }
            VoutDevNum ++;
        }

        {
            extern AMBA_FPD_OBJECT_s AmbaFPD_LT9611UXCObj GNU_WEAK_SYMBOL;
            RawEncVoutDevInfo[VoutDevNum].pObj = &AmbaFPD_LT9611UXCObj;
            RawEncVoutDevInfo[VoutDevNum].ID   = VoutDevNum;
            if (RawEncVoutDevInfo[VoutDevNum].pObj != NULL) {
                SvcWrap_strcpy(RawEncVoutDevInfo[VoutDevNum].DevName, sizeof(RawEncVoutDevInfo[VoutDevNum].DevName), RawEncVoutDevInfo[VoutDevNum].pObj->pName);
            } else {
                SvcWrap_strcpy(RawEncVoutDevInfo[VoutDevNum].DevName, sizeof(RawEncVoutDevInfo[VoutDevNum].DevName), "LT9611UXC");
            }
            VoutDevNum ++;
        }

        AmbaMisra_TouchUnused(&VoutDevNum);

        SVC_RawEncCtrlFlag |= SVC_RAW_ENC_FLG_DEF_VOUT_DEV;
    }
}

static void SvcRawEnc_DumpYuvData(const char *pFilePath)
{
    UINT32 StrmIdx, RetLen, RetVal = SVC_OK;
    char CurFilePath[128];
    SVC_WRAP_SNPRINT_s SnPrnCtrl;
    AMBA_FS_FILE *pFile = NULL;
    UINT8 *pBuf;
    UINT32 ReqSize, WriteSize, BufOfs;
    UINT32 Row, Height;
    const SVC_RAW_ENC_STRM_INFO_s *pCurStrmInfo;

    for (StrmIdx = 0U; StrmIdx < AMBA_DSP_MAX_STREAM_NUM; StrmIdx ++) {
        if ((SVC_RawEncCtrl.EncStrmBits & SvcRawEnc_BitGet(StrmIdx)) > 0U) {
            pCurStrmInfo = &(SVC_RawEncCtrl.EncStrmInfo[StrmIdx]);

            pBuf = pCurStrmInfo->pYBuf;

            if (pBuf == NULL) {
                PRN_RENC_LOG "Fail to dump EncStrmID(%d)  Y buffer - invalid Y buffer!"
                    PRN_RENC_ARG_UINT32 StrmIdx PRN_RENC_ARG_POST
                PRN_RENC_NG
            } else {
                AmbaSvcWrap_MisraMemset(CurFilePath, 0, sizeof(CurFilePath));
                AmbaSvcWrap_MisraMemset(&SnPrnCtrl, 0, sizeof(SnPrnCtrl));
                SnPrnCtrl.pStrFmt = "%s_strm_%02d_%dx%d.y";
                SnPrnCtrl.Argv[SnPrnCtrl.Argc].pCStr   = pFilePath;            SnPrnCtrl.Argc ++;
                SnPrnCtrl.Argv[SnPrnCtrl.Argc].Uint64  = (UINT64)(StrmIdx);              SnPrnCtrl.Argc ++;
                SnPrnCtrl.Argv[SnPrnCtrl.Argc].Uint64  = (UINT64)(pCurStrmInfo->Width);  SnPrnCtrl.Argc ++;
                SnPrnCtrl.Argv[SnPrnCtrl.Argc].Uint64  = (UINT64)(pCurStrmInfo->Height); SnPrnCtrl.Argc ++;
                RetLen = SvcWrap_snprintf(CurFilePath, (UINT32)sizeof(CurFilePath), &SnPrnCtrl);
                if (RetLen == 0U) {
                    PRN_RENC_LOG "Fail to dump EncStrmID(%d)  Y buffer - get file path fail!"
                        PRN_RENC_ARG_UINT32 StrmIdx PRN_RENC_ARG_POST
                    PRN_RENC_NG
                } else if (0U != AmbaFS_FileOpen(CurFilePath, "wb", &pFile)) {
                    PRN_RENC_LOG "Fail to dump EncStrmID(%d)  Y buffer - create file fail!"
                        PRN_RENC_ARG_UINT32 StrmIdx PRN_RENC_ARG_POST
                    PRN_RENC_NG
                } else if (pFile == NULL) {
                    PRN_RENC_LOG "Fail to dump EncStrmID(%d)  Y buffer - invalid file pointer!"
                        PRN_RENC_ARG_UINT32 StrmIdx PRN_RENC_ARG_POST
                    PRN_RENC_NG
                } else {
                    ReqSize = pCurStrmInfo->Width;

                    for (Row = 0U; Row < pCurStrmInfo->Height; Row ++) {
                        BufOfs = pCurStrmInfo->Pitch * Row;

                        WriteSize = 0U;
                        RetVal = AmbaFS_FileWrite(&(pBuf[BufOfs]), 1, ReqSize, pFile, &WriteSize);
                        if (RetVal != 0U) {
                            PRN_RENC_LOG "Fail to dump EncStrmID(%d)  Y buffer - write row(%d) fail! ErrCode(0x%08x)"
                                PRN_RENC_ARG_UINT32 StrmIdx PRN_RENC_ARG_POST
                                PRN_RENC_ARG_UINT32 RetVal  PRN_RENC_ARG_POST
                            PRN_RENC_NG
                        } else {
                            if (ReqSize != WriteSize) {
                                RetVal = SVC_NG;
                                PRN_RENC_LOG "Fail to dump EncStrmID(%d)  Y buffer - write row(%d) size(0x%x) does not same request size(0x%x)!"
                                    PRN_RENC_ARG_UINT32 StrmIdx   PRN_RENC_ARG_POST
                                    PRN_RENC_ARG_UINT32 Row       PRN_RENC_ARG_POST
                                    PRN_RENC_ARG_UINT32 WriteSize PRN_RENC_ARG_POST
                                    PRN_RENC_ARG_UINT32 ReqSize   PRN_RENC_ARG_POST
                                PRN_RENC_NG
                            }
                        }

                        if (RetVal != 0U) {
                            break;
                        }
                    }

                    (void) AmbaFS_FileClose(pFile); pFile = NULL;
                    if (RetVal == 0U) {
                        PRN_RENC_LOG "Success to dump EncStrmID(%d)  Y buffer to %s"
                            PRN_RENC_ARG_UINT32 StrmIdx     PRN_RENC_ARG_POST
                            PRN_RENC_ARG_CSTR   CurFilePath PRN_RENC_ARG_POST
                        PRN_RENC_API
                    }
                }
            }

            pBuf = pCurStrmInfo->pUVBuf;

            if (pBuf == NULL) {
                PRN_RENC_LOG "Fail to dump EncStrmID(%d) UV buffer - invalid UV buffer!"
                    PRN_RENC_ARG_UINT32 StrmIdx PRN_RENC_ARG_POST
                PRN_RENC_NG
            } else {
                AmbaSvcWrap_MisraMemset(CurFilePath, 0, sizeof(CurFilePath));
                AmbaSvcWrap_MisraMemset(&SnPrnCtrl, 0, sizeof(SnPrnCtrl));
                SnPrnCtrl.pStrFmt = "%s_strm_%02d_%dx%d.uv";
                SnPrnCtrl.Argv[SnPrnCtrl.Argc].pCStr  = pFilePath;            SnPrnCtrl.Argc ++;
                SnPrnCtrl.Argv[SnPrnCtrl.Argc].Uint64 = (UINT64)(StrmIdx);              SnPrnCtrl.Argc ++;
                SnPrnCtrl.Argv[SnPrnCtrl.Argc].Uint64 = (UINT64)(pCurStrmInfo->Width);  SnPrnCtrl.Argc ++;
                SnPrnCtrl.Argv[SnPrnCtrl.Argc].Uint64 = (UINT64)(pCurStrmInfo->Height); SnPrnCtrl.Argc ++;
                RetLen = SvcWrap_snprintf(CurFilePath, (UINT32)sizeof(CurFilePath), &SnPrnCtrl);
                if (RetLen == 0U) {
                    PRN_RENC_LOG "Fail to dump EncStrmID(%d) UV buffer - get file path fail!"
                        PRN_RENC_ARG_UINT32 StrmIdx PRN_RENC_ARG_POST
                    PRN_RENC_NG
                } else if (0U != AmbaFS_FileOpen(CurFilePath, "wb", &pFile)) {
                    PRN_RENC_LOG "Fail to dump EncStrmID(%d) UV buffer - create file fail!"
                        PRN_RENC_ARG_UINT32 StrmIdx PRN_RENC_ARG_POST
                    PRN_RENC_NG
                } else if (pFile == NULL) {
                    PRN_RENC_LOG "Fail to dump EncStrmID(%d) UV buffer - invalid file pointer!"
                        PRN_RENC_ARG_UINT32 StrmIdx PRN_RENC_ARG_POST
                    PRN_RENC_NG
                } else {
                    ReqSize = pCurStrmInfo->Width;
                    if (pCurStrmInfo->DataFmt == AMBA_DSP_YUV420) {
                        Height  = pCurStrmInfo->Height >> 1U;
                    } else {
                        Height  = pCurStrmInfo->Height;
                    }

                    for (Row = 0U; Row < Height; Row ++) {
                        BufOfs = pCurStrmInfo->Pitch * Row;

                        WriteSize = 0U;
                        RetVal = AmbaFS_FileWrite(&(pBuf[BufOfs]), 1, ReqSize, pFile, &WriteSize);
                        if (RetVal != 0U) {
                            PRN_RENC_LOG "Fail to dump EncStrmID(%d) UV buffer - write row(%d) fail! ErrCode(0x%08x)"
                                PRN_RENC_ARG_UINT32 StrmIdx PRN_RENC_ARG_POST
                                PRN_RENC_ARG_UINT32 RetVal  PRN_RENC_ARG_POST
                            PRN_RENC_NG
                        } else {
                            if (ReqSize != WriteSize) {
                                RetVal = SVC_NG;
                                PRN_RENC_LOG "Fail to dump EncStrmID(%d) UV buffer - write row(%d) size(0x%x) does not same request size(0x%x)!"
                                    PRN_RENC_ARG_UINT32 StrmIdx   PRN_RENC_ARG_POST
                                    PRN_RENC_ARG_UINT32 Row       PRN_RENC_ARG_POST
                                    PRN_RENC_ARG_UINT32 WriteSize PRN_RENC_ARG_POST
                                    PRN_RENC_ARG_UINT32 ReqSize   PRN_RENC_ARG_POST
                                PRN_RENC_NG
                            }
                        }

                        if (RetVal != 0U) {
                            break;
                        }
                    }

                    (void) AmbaFS_FileClose(pFile); pFile = NULL;
                    if (RetVal == 0U) {
                        PRN_RENC_LOG "Success to dump EncStrmID(%d) UV buffer to %s"
                            PRN_RENC_ARG_UINT32 StrmIdx     PRN_RENC_ARG_POST
                            PRN_RENC_ARG_CSTR   CurFilePath PRN_RENC_ARG_POST
                        PRN_RENC_API
                    }
                }
            }
        }
    }

    if (RetVal == 0U) {
        PRN_RENC_LOG "Dump YUV stream done" PRN_RENC_API
    }
}


static void SvcRawEnc_ShellUsage(UINT32 CtrlFlag)
{
    UINT32 ShellIdx;
    UINT32 ShellCount = (UINT32)(sizeof(SvcRawEncShellFunc)) / (UINT32)(sizeof(SvcRawEncShellFunc[0]));

    PRN_RENC_LOG " " PRN_RENC_API
    PRN_RENC_LOG "====== %sRaw Encode Command Usage%s ======"
        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_TITLE_0 PRN_RENC_ARG_POST
        PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END     PRN_RENC_ARG_POST
    PRN_RENC_API
    for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
        if (SvcRawEncShellFunc[ShellIdx].Enable > 0U) {
            if (SvcRawEncShellFunc[ShellIdx].pUsage != NULL) {
                SvcRawEnc_ShellCommEntryU(CtrlFlag, &(SvcRawEncShellFunc[ShellIdx]));
            } else {
                SvcRawEnc_ShellEmptyUsage(CtrlFlag, &(SvcRawEncShellFunc[ShellIdx]));
            }
        }
    }

    PRN_RENC_LOG " " PRN_RENC_API
}

static void SvcRawEnc_CmdStrToU32(const char *pString, UINT32 *pValue)
{
    UINT32 ErrCode = AmbaUtility_StringToUInt32(pString, pValue);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }
}

static void SvcRawEnc_CmdStrToPointer(const char *pStr, UINT8 **pPointer)
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

static void SvcRawEnc_ShellLog(UINT32 LogLevel, SVC_WRAP_PRINT_s *pPrint, const void *pFunc)
{
    if (pPrint != NULL) {
        char StrBuf[256];

        if ((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_MSG_SHELL) > 0U) {
            if (LogLevel == SVC_RAW_ENC_FLG_MSG_API) {
                if (SVC_RawEncCtrl.pDumpLogFile == NULL) {
                    PRN_RENC_LOG "Fail to dump shell log - invalid file pointer" PRN_RENC_NG
                } else if (0 == SvcWrap_strcmp(pPrint->pStrFmt, " ")) {
                    // ignore " "
                } else if (NULL != SvcWrap_strstr(pPrint->pStrFmt, "-------------")) {
                    const SVC_RAW_ENC_SHELL_FUNC_s *pCurShell;

                    AmbaMisra_TypeCast(&pCurShell, &pFunc);

                    if (pCurShell != NULL) {
                        const char *pChar;
                        UINT32 StrLeng, WriteSize, PRetVal;

                        pChar = SvcWrap_strstr(SvcRawEncShellStr, "svc_rawenc");

                        AmbaSvcWrap_MisraMemset(StrBuf, 0, sizeof(StrBuf));
                        AmbaUtility_StringAppend(StrBuf, (UINT32)sizeof(StrBuf), "\n\n;        Command: ");
                        if (pChar != NULL) {
                            AmbaUtility_StringAppend(StrBuf, (UINT32)sizeof(StrBuf), pChar);
                        } else {
                            AmbaUtility_StringAppend(StrBuf, (UINT32)sizeof(StrBuf), SvcRawEncShellStr);

                        }
                        StrLeng = (UINT32)SvcWrap_strlen(StrBuf);
                        if (StrLeng > 0U) {
                            WriteSize = 0U;
                            PRetVal = AmbaFS_FileWrite(StrBuf, 1, StrLeng, SVC_RawEncCtrl.pDumpLogFile, &WriteSize); PRN_RENC_ERR_HDLR
                        }

                        AmbaSvcWrap_MisraMemset(StrBuf, 0, sizeof(StrBuf));
                        AmbaUtility_StringAppend(StrBuf, (UINT32)sizeof(StrBuf), "\n;    Description: "   );
                        AmbaUtility_StringAppend(StrBuf, (UINT32)sizeof(StrBuf), pCurShell->CmdDescription);
                        StrLeng = (UINT32)SvcWrap_strlen(StrBuf);
                        if (StrLeng > 0U) {
                            WriteSize = 0U;
                            PRetVal = AmbaFS_FileWrite(StrBuf, 1, StrLeng, SVC_RawEncCtrl.pDumpLogFile, &WriteSize); PRN_RENC_ERR_HDLR
                        }

                        AmbaSvcWrap_MisraMemset(StrBuf, 0, sizeof(StrBuf));
                        AmbaUtility_StringAppend(StrBuf, (UINT32)sizeof(StrBuf), "\n;     Parameters:"   );
                        StrLeng = (UINT32)SvcWrap_strlen(StrBuf);
                        if (StrLeng > 0U) {
                            WriteSize = 0U;
                            PRetVal = AmbaFS_FileWrite(StrBuf, 1, StrLeng, SVC_RawEncCtrl.pDumpLogFile, &WriteSize); PRN_RENC_ERR_HDLR
                        }
                    }
                } else {
                    UINT32 StrLeng;
                    SVC_WRAP_SNPRINT_s SnPrnCtrl;

                    AmbaSvcWrap_MisraMemset(&SnPrnCtrl, 0, sizeof(SVC_WRAP_SNPRINT_s));
                    SnPrnCtrl.pStrFmt = pPrint->pStrFmt;
                    SnPrnCtrl.Argc    = pPrint->Argc;
                    if (SnPrnCtrl.Argc > 0U) {
                        AmbaSvcWrap_MisraMemcpy(SnPrnCtrl.Argv, pPrint->Argv, sizeof(SVC_WRAP_ARGV_s)*SnPrnCtrl.Argc);
                    }

                    StrBuf[0] = '\n';
                    StrBuf[1] = ';';

                    StrLeng = SvcWrap_snprintf(&(StrBuf[2]), (UINT32)sizeof(StrBuf) - 2U, &SnPrnCtrl);
                    if (StrLeng > 0U) {
                        UINT32 WriteSize = 0U;
                        StrLeng += 2U;

                        if (0U != AmbaFS_FileWrite(StrBuf, 1, StrLeng, SVC_RawEncCtrl.pDumpLogFile, &WriteSize)) {
                            PRN_RENC_LOG "Fail to dump shell log - write log fail" PRN_RENC_NG
                        } else if (WriteSize != StrLeng) {
                            PRN_RENC_LOG "Fail to dump shell log - write size(0x%x) does not same request size(0x%x)"
                                PRN_RENC_ARG_UINT32 WriteSize PRN_RENC_ARG_POST
                                PRN_RENC_ARG_UINT32 StrLeng   PRN_RENC_ARG_POST
                            PRN_RENC_NG
                        } else {
                            // misra-c
                        }
                    }
                }
            }
        } else if (LogLevel == SVC_RAW_ENC_FLG_MSG_NG) {
            UINT32           Idx;
            SVC_WRAP_PRINT_s CurPrnCtrl;

            AmbaSvcWrap_MisraMemset(StrBuf, 0, sizeof(StrBuf));
            AmbaUtility_StringCopy(StrBuf, sizeof(StrBuf), "%s' cmd - ");
            AmbaUtility_StringAppend(StrBuf, (UINT32)sizeof(StrBuf), pPrint->pStrFmt);

            AmbaSvcWrap_MisraMemset(&CurPrnCtrl, 0, sizeof(CurPrnCtrl));
            CurPrnCtrl.pStrFmt = StrBuf;
            CurPrnCtrl.Argv[CurPrnCtrl.Argc].pCStr = SvcRawEncShellStr; CurPrnCtrl.Argc ++;
            for (Idx = 0U; Idx < pPrint->Argc; Idx ++) {
                AmbaSvcWrap_MisraMemcpy(&(CurPrnCtrl.Argv[CurPrnCtrl.Argc])
                                ,&(pPrint->Argv[Idx])
                                ,sizeof(SVC_WRAP_ARGV_s));
                CurPrnCtrl.Argc ++;
            }

            SvcRawEnc_PrintLog(LogLevel, &CurPrnCtrl);
        } else {
            SvcRawEnc_PrintLog(SVC_RAW_ENC_FLG_MSG_API, pPrint);
        }
    }
}

static void SvcRawEnc_ShellTitle(UINT32 CtrlFlag, UINT32 Level, const char *pCmdStr, const char *pDescription)
{
    if ((pCmdStr != NULL) && (Level < 4U)) {
        const char FrontHL[4][2][40] = {
            { SVC_LOG_RENC_HL_TITLE_0, SVC_LOG_RENC_HL_TITLE_0   },
            { SVC_LOG_RENC_HL_TITLE_1, SVC_LOG_RENC_HL_TITLE_1_U },
            { SVC_LOG_RENC_HL_TITLE_2, SVC_LOG_RENC_HL_TITLE_2_U },
            { SVC_LOG_RENC_HL_TITLE_3, SVC_LOG_RENC_HL_TITLE_3_U },
        };
        const char MiddleHL[2][40] = {
            SVC_LOG_RENC_HL_DEF_FC, SVC_LOG_RENC_HL_END
        };
        char FrontPadding[10];
        char MiddlePadding[30];
        UINT32 FrontPaddingSize = Level << 1U;
        UINT32 CmdStrLen = (UINT32)SvcWrap_strlen(pCmdStr);
        UINT32 AlignPos = 24U - FrontPaddingSize;

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

        PRN_RENC_LOG "%s%s%s%s%s%s%s"
            PRN_RENC_ARG_CSTR FrontPadding                               PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR FrontHL[Level][CtrlFlag & 0x1U]            PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR pCmdStr                                    PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR MiddleHL[CtrlFlag & 0x1U]                  PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR MiddlePadding                              PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR ((pDescription==NULL)?(""):(pDescription)) PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR SVC_LOG_RENC_HL_END                        PRN_RENC_ARG_POST
        PRN_RENC_API
    }
}

static void SvcRawEnc_ShellEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal = SVC_NG;
    UINT32 CtrlFlag = 0U;

    SvcRawEnc_ShellEntryInit();

    SVC_RawEncCtrlFlag &= ~SVC_RAW_ENC_FLG_SHELL_USAGE_MORE;

    AmbaSvcWrap_MisraMemset(SvcRawEncShellStr, 0, sizeof(SvcRawEncShellStr));
    SvcWrap_strcpy(SvcRawEncShellStr, sizeof(SvcRawEncShellStr), "Fail to proc 'svc_rawenc");

    if (pArgVector == NULL) {
        PRN_RENC_LOG "invalid arg vector!" PRN_RENC_SHELL_NG
    } else {
        UINT32 CurArgCnt = ArgCount;

        if (0 == SvcWrap_strcmp(pArgVector[CurArgCnt - 1U], "arg_on")) {
            UINT32 SIdx;

            CurArgCnt -= 1U;

            PRN_RENC_LOG " " PRN_RENC_API
            PRN_RENC_LOG "==== %sShell Arguments Setting%s (%s%d%s) ===="
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_TITLE_1 PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END     PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_TITLE_3 PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 CurArgCnt                PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END     PRN_RENC_ARG_POST
            PRN_RENC_API

            for (SIdx = 0U; SIdx < CurArgCnt; SIdx ++) {
                PRN_RENC_LOG "  pArgVector[%s%d%s] : %s%s%s"
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_TITLE_3 PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 SIdx                    PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END     PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_STR     PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   pArgVector[SIdx]        PRN_RENC_ARG_POST
                    PRN_RENC_ARG_CSTR   SVC_LOG_RENC_HL_END     PRN_RENC_ARG_POST
                PRN_RENC_API
            }
        }

        if (CurArgCnt < 1U) {
            PRN_RENC_LOG "invalid arg count!" PRN_RENC_SHELL_NG
        } else {
            UINT32 ShellIdx;
            UINT32 ShellCount = (UINT32)(sizeof(SvcRawEncShellFunc)) / (UINT32)(sizeof(SvcRawEncShellFunc[0]));
            void  *pFunc = NULL;

            AmbaMisra_TouchUnused(pFunc);

            if (0 == SvcWrap_strcmp(pArgVector[1U], "more")) {
                CtrlFlag = 1U;
                SVC_RawEncCtrlFlag |= SVC_RAW_ENC_FLG_SHELL_USAGE_MORE;
            } else {

                if ((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_MSG_SHELL) > 0U) {
                    AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), " ");
                } else {
                    AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), "->");
                }

                for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
                    if ((SvcRawEncShellFunc[ShellIdx].pFunc != NULL) && (SvcRawEncShellFunc[ShellIdx].Enable > 0U)) {
                        if (0 == SvcWrap_strcmp(pArgVector[1U], SvcRawEncShellFunc[ShellIdx].CmdName)) {

                            if (CurArgCnt > (SvcRawEncShellFunc[ShellIdx].IndentLevel + 1U)) {
                                if (0 == SvcWrap_strcmp(pArgVector[SvcRawEncShellFunc[ShellIdx].IndentLevel + 1U], "more")) {
                                    CtrlFlag = 1U;
                                }
                            }

                            if (CtrlFlag == 0U) {
                                if (SVC_OK != (SvcRawEncShellFunc[ShellIdx].pFunc)(CurArgCnt, pArgVector, &(SvcRawEncShellFunc[ShellIdx]), NULL)) {
                                    CtrlFlag = 1U;
                                }
                            }

                            if (CtrlFlag > 0U) {
                                if (SvcRawEncShellFunc[ShellIdx].pUsage != NULL) {
                                    (SvcRawEncShellFunc[ShellIdx].pUsage)(CtrlFlag, &(SvcRawEncShellFunc[ShellIdx]));
                                } else {
                                    SvcRawEnc_ShellEmptyUsage(CtrlFlag, &(SvcRawEncShellFunc[ShellIdx]));
                                }
                            }

                            SVC_RawEncCtrlFlag |= SVC_RAW_ENC_FLG_SHELL_USAGE_MORE;
                            RetVal = SVC_OK;
                            break;
                        }
                    }
                }
            }
        }
    }

    if (RetVal != SVC_OK) {
        SvcRawEnc_ShellUsage(CtrlFlag);

        AmbaMisra_TouchUnused(&PrintFunc);
        AmbaMisra_TouchUnused(SvcRawEncShellFunc);
        AmbaMisra_TouchUnused(SvcRawEncCfgShellFunc);
        AmbaMisra_TouchUnused(SvcRawEncCfgIqTblShellFunc);
        AmbaMisra_TouchUnused(SvcRawEncDmySenShellFunc);
        AmbaMisra_TouchUnused(SvcRawEncResCfgShellFunc);
        AmbaMisra_TouchUnused(SvcRawEncResCfgVinShellFunc);
        AmbaMisra_TouchUnused(SvcRawEncResCfgFovShellFunc);
        AmbaMisra_TouchUnused(SvcRawEncResCfgDispShellFunc);
        AmbaMisra_TouchUnused(SvcRawEncResCfgRecShellFunc);
    }

    if ((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_SHELL_USAGE_MORE) == 0U) {
        PRN_RENC_LOG "Please type \"%smore%s\" to print fully command description"
            PRN_RENC_ARG_CSTR SVC_LOG_RENC_HL_TITLE_3_U PRN_RENC_ARG_POST
            PRN_RENC_ARG_CSTR SVC_LOG_RENC_HL_END       PRN_RENC_ARG_POST
        PRN_RENC_API
    }
}

static void SvcRawEnc_ShellEntryInit(void)
{
    if ((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_SHELL_INIT) == 0U) {
        UINT8 CmdIdx;
        SVC_RAW_ENC_SHELL_FUNC_s *pShellFunc;
#define SVC_RAW_ENC_SHELL_FQ CmdIdx=0U; pShellFunc[CmdIdx] = (SVC_RAW_ENC_SHELL_FUNC_s)
#define SVC_RAW_ENC_SHELL_EQ CmdIdx ++; pShellFunc[CmdIdx] = (SVC_RAW_ENC_SHELL_FUNC_s)

        AmbaSvcWrap_MisraMemset(SvcRawEncShellFunc, 0, sizeof(SvcRawEncShellFunc));
        pShellFunc = SvcRawEncShellFunc;
        SVC_RAW_ENC_SHELL_FQ { 1U, 1U, "create",   "create raw encode module",               SvcRawEnc_ShellCreate,  NULL,                      NULL, 0U};
        SVC_RAW_ENC_SHELL_EQ { 1U, 1U, "delete",   "delete raw encode module",               SvcRawEnc_ShellDelete,  NULL,                      NULL, 0U};
        SVC_RAW_ENC_SHELL_EQ { 1U, 1U, "cfg",      "raw enc config",                         SvcRawEnc_ShellConfig,  SvcRawEnc_ShellCommEntryU, SvcRawEncCfgShellFunc,    SVC_RAW_ENC_CFG_SHELL_NUM};
        SVC_RAW_ENC_SHELL_EQ { 1U, 1U, "res_cfg",  "raw enc resolution config",              SvcRawEnc_ShellResCfg,  SvcRawEnc_ShellCommEntryU, SvcRawEncResCfgShellFunc, SVC_RAW_ENC_RES_CFG_SHELL_NUM};
        SVC_RAW_ENC_SHELL_EQ { 1U, 1U, "dmy_sen",  "raw enc dummy sensor config",            SvcRawEnc_ShellDmySen,  SvcRawEnc_ShellCommEntryU, SvcRawEncDmySenShellFunc, SVC_RAW_ENC_DMY_SEN_SHELL_NUM};
        SVC_RAW_ENC_SHELL_EQ { 1U, 1U, "execute",  "execute to trigger feed raw and ituner", SvcRawEnc_ShellExecute, NULL,                      NULL, 0U};
        SVC_RAW_ENC_SHELL_EQ { 1U, 1U, "dump_yuv", "dump yuv stream",                        SvcRawEnc_ShellDumpYuv, SvcRawEnc_ShellDumpYuvU,   NULL, 0U};
        SVC_RAW_ENC_SHELL_EQ { 1U, 1U, "dbg_msg",  "enable/disable debug msg",               SvcRawEnc_ShellDbgMsg,  SvcRawEnc_ShellDbgMsgU,    NULL, 0U};

        AmbaSvcWrap_MisraMemset(SvcRawEncCfgShellFunc, 0, sizeof(SvcRawEncCfgShellFunc));
        pShellFunc = SvcRawEncCfgShellFunc;
        SVC_RAW_ENC_SHELL_FQ { 1U, 2U, "mem",          "Configure memory setting",          SvcRawEnc_ShellCfgMem,      SvcRawEnc_ShellCfgMemU,    NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 2U, "raw",          "Configure the raw path",            SvcRawEnc_ShellCfgRaw,      SvcRawEnc_ShellCfgRawU,    NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 2U, "hds",          "Configure the hds path",            SvcRawEnc_ShellCfgHds,      SvcRawEnc_ShellCfgHdsU,    NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 2U, "itn",          "Configure the ituner path",         SvcRawEnc_ShellCfgItn,      SvcRawEnc_ShellCfgItnU,    NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 2U, "aaa",          "Configure the 3A data path",        SvcRawEnc_ShellCfgAaa,      SvcRawEnc_ShellCfgAaaU,    NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 2U, "info",         "print raw enc configuration",       SvcRawEnc_ShellCfgInfo,     NULL, NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 2U, "bufctrl_info", "print raw enc buffer control info", SvcRawEnc_ShellBufCtrlInfo, NULL, NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 2U, "src",          "Configure the vin src",             SvcRawEnc_ShellCfgSrc,      SvcRawEnc_ShellCfgSrcU,    NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 2U, "iqtbl",        "configure iq table",                SvcRawEnc_ShellCfgIqTbl,    SvcRawEnc_ShellCommEntryU, SvcRawEncCfgIqTblShellFunc, SVC_RAW_ENC_CFG_IQ_SHELL_NUM };

        AmbaSvcWrap_MisraMemset(SvcRawEncCfgIqTblShellFunc, 0, sizeof(SvcRawEncCfgIqTblShellFunc));
        pShellFunc = SvcRawEncCfgIqTblShellFunc;
        SVC_RAW_ENC_SHELL_FQ { 1U, 3U, "adj",        "Configure the iq table adj path",                  SvcRawEnc_ShellCfgIqTblAdj,    SvcRawEnc_ShellCfgIqTblAdjU,    NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "img",        "Configure the iq table img param path",            SvcRawEnc_ShellCfgIqTblImg,    SvcRawEnc_ShellCfgIqTblImgU,    NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "aaa",        "Configure the iq table aaa path",                  SvcRawEnc_ShellCfgIqTblAaa,    SvcRawEnc_ShellCfgIqTblAaaU,    NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "video",      "Configure the iq table video path",                SvcRawEnc_ShellCfgIqTblVideo,  SvcRawEnc_ShellCfgIqTblVideoU,  NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "photo",      "Configure the iq table photo path",                SvcRawEnc_ShellCfgIqTblPhoto,  SvcRawEnc_ShellCfgIqTblPhotoU,  NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "video_msm",  "Configure the iq table video msm path",            SvcRawEnc_ShellCfgIqTblVidMSM, SvcRawEnc_ShellCfgIqTblVidMSMU, NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "video_msh",  "Configure the iq table video msh path",            SvcRawEnc_ShellCfgIqTblVidMSH, SvcRawEnc_ShellCfgIqTblVidMSHU, NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "photo_msm",  "Configure the iq table photo msm path",            SvcRawEnc_ShellCfgIqTblPhoMSM, SvcRawEnc_ShellCfgIqTblPhoMSMU, NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "photo_msh",  "Configure the iq table photo msh path",            SvcRawEnc_ShellCfgIqTblPhoMSH, SvcRawEnc_ShellCfgIqTblPhoMSHU, NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "still_liso", "Configure the iq table still low iso path",        SvcRawEnc_ShellCfgIqTblSLiso,  SvcRawEnc_ShellCfgIqTblSLisoU,  NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "still_hiso", "Configure the iq table still high iso path",       SvcRawEnc_ShellCfgIqTblSHiso,  SvcRawEnc_ShellCfgIqTblSHisoU,  NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "video_cc",   "Configure the iq table video cc path",             SvcRawEnc_ShellCfgIqTblVidCc,  SvcRawEnc_ShellCfgIqTblVidCcU,  NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "still_cc",   "Configure the iq table still cc path",             SvcRawEnc_ShellCfgIqTblStiCc,  SvcRawEnc_ShellCfgIqTblStiCcU,  NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "scene",      "Configure the iq table scene path",                SvcRawEnc_ShellCfgIqTblScene,  SvcRawEnc_ShellCfgIqTblSceneU,  NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "de_video",   "Configure the iq table digital effect video path", SvcRawEnc_ShellCfgIqTblDVid,   SvcRawEnc_ShellCfgIqTblDVidU,   NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "de_still",   "Configure the iq table digital effect still path", SvcRawEnc_ShellCfgIqTblDSti,   SvcRawEnc_ShellCfgIqTblDStiU,   NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "vid_param",  "Configure the iq table video param path",          SvcRawEnc_ShellCfgIqTblVidPrm, SvcRawEnc_ShellCfgIqTblVidPrmU, NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "stl_param",  "Configure the iq table still param path",          SvcRawEnc_ShellCfgIqTblStlPrm, SvcRawEnc_ShellCfgIqTblStlPrmU, NULL, 0U };

        AmbaSvcWrap_MisraMemset(SvcRawEncDmySenShellFunc, 0, sizeof(SvcRawEncDmySenShellFunc));
        pShellFunc = SvcRawEncDmySenShellFunc;
        SVC_RAW_ENC_SHELL_FQ { 1U, 2U, "reset",      "reset dummy sensor config",          SvcRawEnc_ShellDmySenReset,  NULL,    NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 2U, "output",     "configure dummy sensor output info", SvcRawEnc_ShellDmySenOutput, SvcRawEnc_ShellDmySenOutputU, NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 2U, "input",      "configure dummy sensor input info",  SvcRawEnc_ShellDmySenInput,  SvcRawEnc_ShellDmySenInputU,  NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 2U, "mode_info",  "configure dummy sensor mode info",   SvcRawEnc_ShellDmySenMInfo,  SvcRawEnc_ShellDmySenMInfoU,  NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 2U, "hdr_ch",     "configure dummy sensor hdr channel", SvcRawEnc_ShellDmySenHdrCh,  SvcRawEnc_ShellDmySenHdrChU,  NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 2U, "hdr",        "configure dummy sensor hdr info",    SvcRawEnc_ShellDmySenHdr,    SvcRawEnc_ShellDmySenHdrU,    NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 2U, "info",       "print dummy sensor setting",         SvcRawEnc_ShellDmySenInfo,   NULL,    NULL, 0U };

        AmbaSvcWrap_MisraMemset(SvcRawEncResCfgShellFunc, 0, sizeof(SvcRawEncResCfgShellFunc));
        pShellFunc = SvcRawEncResCfgShellFunc;
        SVC_RAW_ENC_SHELL_FQ { 1U, 2U, "vin",   "configure resolution cfg vin info", SvcRawEnc_ShellResCfgVin,   SvcRawEnc_ShellCommEntryU,   SvcRawEncResCfgVinShellFunc, SVC_RAW_ENC_RES_VIN_SHELL_NUM };
        SVC_RAW_ENC_SHELL_EQ { 1U, 2U, "sen",   "configure sensor setting",          SvcRawEnc_ShellResCfgSen,   SvcRawEnc_ShellResCfgSenU,   NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 2U, "fov",   "configure resolution cfg fov info", SvcRawEnc_ShellResCfgFov,   SvcRawEnc_ShellCommEntryU,   SvcRawEncResCfgFovShellFunc, SVC_RAW_ENC_RES_FOV_SHELL_NUM };
        SVC_RAW_ENC_SHELL_EQ { 1U, 2U, "disp",  "configure resolution cfg disp info",SvcRawEnc_ShellResCfgDisp,  SvcRawEnc_ShellCommEntryU,   SvcRawEncResCfgDispShellFunc, SVC_RAW_ENC_RES_DISP_SHELL_NUM };
        SVC_RAW_ENC_SHELL_EQ { 1U, 2U, "rec",   "configure resolution cfg rec info", SvcRawEnc_ShellResCfgRec,   SvcRawEnc_ShellCommEntryU,   SvcRawEncResCfgRecShellFunc, SVC_RAW_ENC_RES_REC_SHELL_NUM };
        SVC_RAW_ENC_SHELL_EQ { 1U, 2U, "reset", "reset raw enc resolution config",   SvcRawEnc_ShellResCfgReset, NULL,   NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 2U, "info",  "print raw enc resolution config",   SvcRawEnc_ShellResCfgInfo,  NULL,   NULL, 0U };

        AmbaSvcWrap_MisraMemset(SvcRawEncResCfgVinShellFunc, 0, sizeof(SvcRawEncResCfgVinShellFunc));
        pShellFunc = SvcRawEncResCfgVinShellFunc;
        SVC_RAW_ENC_SHELL_FQ { 1U, 3U, "vin_tree",      "configure vin tree string",    SvcRawEnc_ShellResCfgVinTree,  SvcRawEnc_ShellResCfgVinTreeU,  NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "vin_num",       "configure vin number",         SvcRawEnc_ShellResCfgVinNum,   SvcRawEnc_ShellResCfgVinNumU,   NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "cap_win",       "configure vin capture win",    SvcRawEnc_ShellResCfgVinWin,   SvcRawEnc_ShellResCfgVinWinU,   NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "sub_chan_num",  "configure sub channel number", SvcRawEnc_ShellResCfgVinSCNum, SvcRawEnc_ShellResCfgVinSCNumU, NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "sub_chan",      "configure sub channel",        SvcRawEnc_ShellResCfgVinSChan, SvcRawEnc_ShellResCfgVinSChanU, NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "frate",         "configure framerate",          SvcRawEnc_ShellResCfgVinFrate, SvcRawEnc_ShellResCfgVinFrateU, NULL, 0U };

        AmbaSvcWrap_MisraMemset(SvcRawEncResCfgFovShellFunc, 0, sizeof(SvcRawEncResCfgFovShellFunc));
        pShellFunc = SvcRawEncResCfgFovShellFunc;
        SVC_RAW_ENC_SHELL_FQ { 1U, 3U, "fov_num",  "configure fov number",          SvcRawEnc_ShellResCfgFovNum,  SvcRawEnc_ShellResCfgFovNumU,  NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "raw_win",  "configure fov raw win",         SvcRawEnc_ShellResCfgFovRaw,  SvcRawEnc_ShellResCfgFovRawU,  NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "act_win",  "configure fov act win",         SvcRawEnc_ShellResCfgFovAct,  SvcRawEnc_ShellResCfgFovActU,  NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "main_win", "configure fov main win",        SvcRawEnc_ShellResCfgFovMain, SvcRawEnc_ShellResCfgFovMainU, NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "pyramid",  "configure pyramid select bits", SvcRawEnc_ShellResCfgFovPyr,  SvcRawEnc_ShellResCfgFovPyrU,  NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "hier_win", "configure fov hier win",        SvcRawEnc_ShellResCfgFovHier, SvcRawEnc_ShellResCfgFovHierU, NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "pipe_cfg", "configure fov pipe cfg",        SvcRawEnc_ShellResCfgFovPipe, SvcRawEnc_ShellResCfgFovPipeU, NULL, 0U };

        AmbaSvcWrap_MisraMemset(SvcRawEncResCfgDispShellFunc, 0, sizeof(SvcRawEncResCfgDispShellFunc));
        pShellFunc = SvcRawEncResCfgDispShellFunc;
        SVC_RAW_ENC_SHELL_FQ { 1U, 3U, "sel_bits", "configure disp select bits",        SvcRawEnc_ShellResCfgDispBits, SvcRawEnc_ShellResCfgDispBitsU, NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "vout_id",  "configure disp cfg vout id",        SvcRawEnc_ShellResCfgDispId,   SvcRawEnc_ShellResCfgDispIdU,   NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "vout_obj", "configure vout driver obj",         SvcRawEnc_ShellResCfgDispObj,  SvcRawEnc_ShellResCfgDispObjU,  NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "win",      "configure disp cfg window",         SvcRawEnc_ShellResCfgDispWin,  SvcRawEnc_ShellResCfgDispWinU,  NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "max_win",  "configure disp cfg max window",     SvcRawEnc_ShellResCfgDispMWin, SvcRawEnc_ShellResCfgDispMWinU, NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "ch_num",   "configure disp cfg channel number", SvcRawEnc_ShellResCfgDispCNum, SvcRawEnc_ShellResCfgDispCNumU, NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "ch_cfg",   "configure disp cfg channel cfg",    SvcRawEnc_ShellResCfgDispCCfg, SvcRawEnc_ShellResCfgDispCCfgU, NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "frate",    "configure disp framerate",          SvcRawEnc_ShellResCfgDispFrat, SvcRawEnc_ShellResCfgDispFratU, NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "cfg",      "configure disp dev mode",           SvcRawEnc_ShellResCfgDispCfg,  SvcRawEnc_ShellResCfgDispCfgU,  NULL, 0U };

        AmbaSvcWrap_MisraMemset(SvcRawEncResCfgRecShellFunc, 0, sizeof(SvcRawEncResCfgRecShellFunc));
        pShellFunc = SvcRawEncResCfgRecShellFunc;
        SVC_RAW_ENC_SHELL_FQ { 1U, 3U, "sel_bits", "configure record select bits",       SvcRawEnc_ShellResCfgRecBits,  SvcRawEnc_ShellResCfgRecBitsU , NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "setting",  "configure record cfg setting",       SvcRawEnc_ShellResCfgRecSetup, SvcRawEnc_ShellResCfgRecSetupU, NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "win",      "configure record cfg window",        SvcRawEnc_ShellResCfgRecWin,   SvcRawEnc_ShellResCfgRecWinU  , NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "max_win",  "configure record cfg max window",    SvcRawEnc_ShellResCfgRecMWin,  SvcRawEnc_ShellResCfgRecMWinU , NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "ch_num",   "configure record cfg channel number",SvcRawEnc_ShellResCfgRecCNum,  SvcRawEnc_ShellResCfgRecCNumU , NULL, 0U };
        SVC_RAW_ENC_SHELL_EQ { 1U, 3U, "ch_cfg",   "configure record cfg channel cfg",   SvcRawEnc_ShellResCfgRecCCfg,  SvcRawEnc_ShellResCfgRecCCfgU , NULL, 0U };

        SVC_RawEncCtrlFlag |= SVC_RAW_ENC_FLG_SHELL_INIT;
    }
}

static UINT32 SvcRawEnc_ShellCommEntry(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_NG;
    UINT32 CtrlFlag = 0U;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        PRN_RENC_LOG "invalid sub shell entry!" PRN_RENC_NG
    } else {
        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < (pCurFunc->IndentLevel + 1U)) {
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount              PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 pCurFunc->IndentLevel PRN_RENC_ARG_POST
            PRN_RENC_NG
        } else {
            UINT32                    SubIdx;
            SVC_RAW_ENC_SHELL_FUNC_s *pSubFunc;

            AmbaMisra_TypeCast(&(pSubFunc), &(pCurFunc->pSubFunc));

            if (ArgCount > (pCurFunc->IndentLevel + 1U)) {
                if (0 == SvcWrap_strcmp(pArgVector[pCurFunc->IndentLevel + 1U], "more")) {
                    CtrlFlag = 1U;
                    SVC_RawEncCtrlFlag |= SVC_RAW_ENC_FLG_SHELL_USAGE_MORE;
                }
            }

            if ((CtrlFlag == 0U) && (pSubFunc != NULL)) {

                if ((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_MSG_SHELL) > 0U) {
                    AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), " ");
                } else {
                    AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), "->");
                }

                for (SubIdx = 0U; SubIdx < pCurFunc->NumSubFunc; SubIdx ++) {
                    if ((pSubFunc[SubIdx].pFunc != NULL) && (pSubFunc[SubIdx].Enable > 0U)) {
                        if (0 == SvcWrap_strcmp(pArgVector[pSubFunc[SubIdx].IndentLevel], pSubFunc[SubIdx].CmdName)) {

                            if (ArgCount > (pSubFunc[SubIdx].IndentLevel + 1U)) {
                                if (0 == SvcWrap_strcmp(pArgVector[pSubFunc[SubIdx].IndentLevel + 1U], "more")) {
                                    CtrlFlag = 1U;
                                }
                            }

                            if (CtrlFlag == 0U) {
                                if (OK != (pSubFunc[SubIdx].pFunc)(ArgCount, pArgVector, &(pSubFunc[SubIdx]), pData)) {
                                    CtrlFlag = 1U;
                                }
                            }

                            if (CtrlFlag > 0U) {
                                if (pSubFunc[SubIdx].pUsage != NULL) {
                                    (pSubFunc[SubIdx].pUsage)(CtrlFlag, &(pSubFunc[SubIdx]));
                                } else {
                                    SvcRawEnc_ShellEmptyUsage(CtrlFlag, &(pSubFunc[SubIdx]));
                                }
                            }

                            SVC_RawEncCtrlFlag |= SVC_RAW_ENC_FLG_SHELL_USAGE_MORE;
                            RetVal = SVC_OK;
                            break;
                        }
                    }
                }
            }
        }

        if (RetVal != SVC_OK) {
            SvcRawEnc_ShellCommEntryU(CtrlFlag, pFunc);
        }
    }

    return SVC_OK;
}

static void SvcRawEnc_ShellCommEntryU(UINT32 CtrlFlag, void *pFunc)
{
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc != NULL) {

        if ((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_MSG_SHELL) == 0U) {
            SvcRawEnc_ShellTitle(CtrlFlag, pCurFunc->IndentLevel, pCurFunc->CmdName, pCurFunc->CmdDescription);
        }

        if (CtrlFlag > 0U) {
            if (pCurFunc->pUsage != NULL) {
                (pCurFunc->pUsage)(CtrlFlag, NULL);
            }
        }

        if (pCurFunc->NumSubFunc > 0U) {
            UINT32                    SubIdx;
            SVC_RAW_ENC_SHELL_FUNC_s *pSubFunc;

            AmbaMisra_TypeCast(&(pSubFunc), &(pCurFunc->pSubFunc));

            if (pSubFunc != NULL) {
                for (SubIdx = 0U; SubIdx < pCurFunc->NumSubFunc; SubIdx ++) {
                    if (pSubFunc[SubIdx].Enable > 0U) {
                        if (pSubFunc[SubIdx].pUsage != NULL) {
                            if (pCurFunc->pUsage != NULL) {
                                (pCurFunc->pUsage)(CtrlFlag, &(pSubFunc[SubIdx]));
                            }
                        } else {
                            SvcRawEnc_ShellEmptyUsage(CtrlFlag, &(pSubFunc[SubIdx]));
                        }
                    }
                }
            }
        }

        if (CtrlFlag > 0U) {
            SVC_RawEncCtrlFlag |= SVC_RAW_ENC_FLG_SHELL_USAGE_MORE;
        }
    }

    AmbaMisra_TouchUnused(pFunc);
}

static void SvcRawEnc_ShellEmptyUsage(UINT32 CtrlFlag, void *pFunc)
{
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc != NULL) {

        if ((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_MSG_SHELL) == 0U) {
            SvcRawEnc_ShellTitle(CtrlFlag, pCurFunc->IndentLevel, pCurFunc->CmdName, pCurFunc->CmdDescription);
        }

        if (CtrlFlag > 0U) {
            PRN_RENC_LOG "  ------------------------------------------------------" PRN_RENC_SHELL_USAGE
            PRN_RENC_LOG "                   None" PRN_RENC_SHELL_USAGE
            PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
        }
    }

    AmbaMisra_TouchUnused(pFunc);
}

static UINT32 SvcRawEnc_ShellCreate(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 1U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            const SVC_RES_CFG_s *pResCfg = SvcRawEnc_ResCfgGet();

            if (pResCfg == NULL) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "invalid raw enc resolution cfg!" PRN_RENC_SHELL_NG
            } else {
                if (0U != SvcRawEnc_Create(pResCfg)) {
                    PRN_RENC_LOG "Fail to create video raw encode module" PRN_RENC_NG
                }
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static UINT32 SvcRawEnc_ShellDelete(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 1U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            UINT32 PRetVal = SvcRawEnc_Delete(); PRN_RENC_ERR_HDLR
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static UINT32 SvcRawEnc_ShellConfig(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    return SvcRawEnc_ShellCommEntry(ArgCount, pArgVector, pFunc, pData);
}

static UINT32 SvcRawEnc_ShellResCfg(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    AmbaMisra_TouchUnused(pData);

    return SvcRawEnc_ShellCommEntry(ArgCount, pArgVector, pFunc, SvcRawEnc_ResCfgGet());
}

static UINT32 SvcRawEnc_ShellDmySen(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    AmbaMisra_TouchUnused(pData);

    return SvcRawEnc_ShellCommEntry(ArgCount, pArgVector, pFunc, SvcRawEnc_DmySenCfgGet());
}

static UINT32 SvcRawEnc_ShellExecute(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 1U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            UINT32 PRetVal = SvcRawEnc_Execute(); PRN_RENC_ERR_HDLR
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static UINT32 SvcRawEnc_ShellDumpYuv(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 2U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx  = pCurFunc->IndentLevel + 1U;

            SvcRawEnc_DumpYuvData(pArgVector[ArgIdx]);
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellDumpYuvU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "  ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "        Prefix FilePath : output file path prefix" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellDbgMsg(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 2U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 DbgLvl = 0U;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &DbgLvl);

            if (DbgLvl > 0U) {
                SVC_RawEncCtrlFlag |= SVC_RAW_ENC_FLG_MSG_DBG;
            } else {
                SVC_RawEncCtrlFlag &= ~SVC_RAW_ENC_FLG_MSG_DBG;
            }

            PRN_RENC_LOG "%s debug message"
                PRN_RENC_ARG_CSTR (((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_MSG_DBG) > 0U)?"Enable":"Disable") PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellDbgMsgU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "  ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "            Debug Level : output file path prefix" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellCfgMem(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 3U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT8 *pMemBuf = NULL;
            UINT32 IemSize = 0U;

            SvcRawEnc_CmdStrToPointer(pArgVector[ArgIdx], &pMemBuf); ArgIdx ++;
                SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &IemSize);
            if (0U != SvcRawEnc_MemCfg(pMemBuf, IemSize)) {
                PRN_RENC_LOG "Fail to configure video raw encode memory" PRN_RENC_NG
            }

        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellCfgMemU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "    ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                MemAddr : Configure memory addr" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                MemSize : Configure memory size" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellCfgRaw(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 3U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 NumVin = 0U;
            UINT32 VinID = 0xFFFFFFFFU;
            char *pRawFilePath = NULL;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &VinID);  ArgIdx ++;
            if (VinID == 0xFFFFFFFFU) {
                RetVal = SVC_NG;
            } else {
                AmbaMisra_TypeCast(&(pRawFilePath), &(pArgVector[ArgIdx]));

                NumVin = 1U;
                if (0U != SvcRawEnc_RawDataCfg(NumVin, &VinID, &pRawFilePath)) {
                    PRN_RENC_LOG "Fail to cfg raw path" PRN_RENC_NG
                }
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellCfgRawU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "    ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                  VinID : Configure vin id" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "               raw path : Configure raw file path" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellCfgHds(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 3U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 NumVin = 0U;
            UINT32 VinID = 0xFFFFFFFFU;
            char *pHdsFilePath = NULL;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &VinID);  ArgIdx ++;
            if (VinID == 0xFFFFFFFFU) {
                RetVal = SVC_NG;
            } else {

                AmbaMisra_TypeCast(&(pHdsFilePath), &(pArgVector[ArgIdx]));

                NumVin = 1U;
                if (0U != SvcRawEnc_HdsDataCfg(NumVin, &VinID, &pHdsFilePath)) {
                    PRN_RENC_LOG "Fail to cfg hds path" PRN_RENC_NG
                }
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellCfgHdsU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "    ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                  VinID : Configure vin id" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "               hds path : Configure hds file path" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellCfgItn(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 3U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 NumViewZone = 0U;
            UINT32 ViewZoneID = 0xFFFFFFFFU;
            char  *pItnFilePath = NULL;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &ViewZoneID); ArgIdx ++;
            if (ViewZoneID == 0xFFFFFFFFU) {
                RetVal = SVC_NG;
            } else {
                AmbaMisra_TypeCast(&(pItnFilePath), &(pArgVector[ArgIdx]));

                NumViewZone = 1U;
                if (0U != SvcRawEnc_ViewZoneCfg(NumViewZone, &ViewZoneID, &pItnFilePath)) {
                    PRN_RENC_LOG "Fail to cfg itn path" PRN_RENC_NG
                }
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellCfgItnU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "    ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                IkCtxID : Configure ik context id" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "               itn path : Configure ituner file path" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellCfgAaa(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 4U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 VinID     = 0xFFFFFFFFU;
            UINT32 IkCtxID   = 0xFFFFFFFFU;
            UINT32 ImgChanID = 0xFFFFFFFFU;
            char  *pDataFilePath = NULL;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &VinID);     ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &IkCtxID);   ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &ImgChanID);
            if ((VinID >= AMBA_DSP_MAX_VIN_NUM) || (ImgChanID == 0xFFFFFFFFU)) {
                RetVal = SVC_NG;
            } else {

                AmbaMisra_TypeCast(&(pDataFilePath), &(pArgVector[6U]));

                if (0U != SvcRawEnc_AaaDataCfg(VinID, IkCtxID, ImgChanID, pDataFilePath)) {
                    PRN_RENC_LOG "Fail to cfg aaa path" PRN_RENC_NG
                }
            }

        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellCfgAaaU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "    ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                  VinID : Configure vin id" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                IkCtxID : Configure ik context id" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "              ImgChanID : Configure image channel id" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "              Data path : Configure aaa data path" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellCfgInfo(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 1U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            SvcRawEnc_CfgPrint();
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static UINT32 SvcRawEnc_ShellBufCtrlInfo(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 1U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if ((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_INIT) == 0U) {
            PRN_RENC_LOG "initial video raw enc module first" PRN_RENC_SHELL_NG
        } else {
            UINT32 Idx;

            for (Idx = 0U; Idx < AMBA_DSP_MAX_VIN_NUM; Idx ++) {
                if ((SVC_RawEncCtrl.VinSelectBits & SvcRawEnc_BitGet(Idx)) > 0U) {
                    SvcRawEnc_BufCtrlInfo(&(SVC_RawEncCtrl.BufCtrl[Idx]));
                }
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static UINT32 SvcRawEnc_ShellCfgSrc(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 3U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 NumVin = 0U;
            UINT32 VinID = 0xFFFFFFFFU;
            UINT32 VinSrc = 0xFFFFFFFFU;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &VinID);  ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &VinSrc);
            if (VinID == 0xFFFFFFFFU) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "invalid VinID(%d)"
                    PRN_RENC_ARG_UINT32 VinID PRN_RENC_ARG_POST
                PRN_RENC_SHELL_NG
            } else {
                if (VinSrc == 0xFFFFFFFFU) {
                    RetVal = SVC_NG;
                    PRN_RENC_LOG "invalid VinSrc(%d)"
                        PRN_RENC_ARG_UINT32 VinSrc PRN_RENC_ARG_POST
                    PRN_RENC_SHELL_NG
                } else {
                    NumVin = 1U;
                    if (0U != SvcRawEnc_VinSrcCfg(NumVin, &VinID, &VinSrc)) {
                        PRN_RENC_LOG "Fail to cfg vin source" PRN_RENC_NG
                    }
                }
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellCfgSrcU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "    ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                  VinID : Configure vin id" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                 Source : Configure vin src. 0 is supported" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellCfgIqTbl(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    AmbaMisra_TouchUnused(pData);

    return SvcRawEnc_ShellCommEntry(ArgCount, pArgVector, pFunc, NULL);
}

static UINT32 SvcRawEnc_ShellCfgIqTblAdj(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 2U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            SVC_RAW_ENC_UPD_IQPATH UpdIQPath;
            AmbaSvcWrap_MisraMemset(&UpdIQPath, 0, sizeof(UpdIQPath));
            UpdIQPath.ID = SVC_RAW_ENC_UPD_ADJ_PATH;
            AmbaMisra_TypeCast(&(UpdIQPath.pPath), &(pArgVector[ArgIdx]));
            SvcRawEnc_ProcCallBack(SVC_RAW_ENC_CODE_UPD_IQPATH, &UpdIQPath);

        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellCfgIqTblAdjU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "    ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                   Path : Configure iq table adj path" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellCfgIqTblImg(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 2U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            SVC_RAW_ENC_UPD_IQPATH UpdIQPath;
            AmbaSvcWrap_MisraMemset(&UpdIQPath, 0, sizeof(UpdIQPath));
            UpdIQPath.ID = SVC_RAW_ENC_UPD_IMG_PATH;
            AmbaMisra_TypeCast(&(UpdIQPath.pPath), &(pArgVector[ArgIdx]));
            SvcRawEnc_ProcCallBack(SVC_RAW_ENC_CODE_UPD_IQPATH, &UpdIQPath);

        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellCfgIqTblImgU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "    ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                   Path : Configure iq table img param path" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellCfgIqTblAaa(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 3U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 TableNo = 0xFFFFFFFFU;
            SVC_RAW_ENC_UPD_IQPATH UpdIQPath;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &TableNo); ArgIdx ++;
            AmbaSvcWrap_MisraMemset(&UpdIQPath, 0, sizeof(UpdIQPath));
            UpdIQPath.TableNo = TableNo;
            UpdIQPath.ID      = SVC_RAW_ENC_UPD_AAA_PATH;
            AmbaMisra_TypeCast(&(UpdIQPath.pPath), &(pArgVector[ArgIdx]));
            SvcRawEnc_ProcCallBack(SVC_RAW_ENC_CODE_UPD_IQPATH, &UpdIQPath);
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellCfgIqTblAaaU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "    ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                TableNo : Configure iq table aaa index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                   Path : Configure iq table aaa path" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellCfgIqTblVideo(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 3U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 TableNo = 0xFFFFFFFFU;
            SVC_RAW_ENC_UPD_IQPATH UpdIQPath;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &TableNo); ArgIdx ++;
            AmbaSvcWrap_MisraMemset(&UpdIQPath, 0, sizeof(UpdIQPath));
            UpdIQPath.TableNo = TableNo;
            UpdIQPath.ID      = SVC_RAW_ENC_UPD_VIDEO_PATH;
            AmbaMisra_TypeCast(&(UpdIQPath.pPath), &(pArgVector[ArgIdx]));
            SvcRawEnc_ProcCallBack(SVC_RAW_ENC_CODE_UPD_IQPATH, &UpdIQPath);
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellCfgIqTblVideoU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "    ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                TableNo : Configure iq table video index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                   Path : Configure iq table video path" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellCfgIqTblPhoto(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 3U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 TableNo = 0xFFFFFFFFU;
            SVC_RAW_ENC_UPD_IQPATH UpdIQPath;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &TableNo);    ArgIdx ++;
            AmbaSvcWrap_MisraMemset(&UpdIQPath, 0, sizeof(UpdIQPath));
            UpdIQPath.TableNo = TableNo;
            UpdIQPath.ID      = SVC_RAW_ENC_UPD_PHOTO_PATH;
            AmbaMisra_TypeCast(&(UpdIQPath.pPath), &(pArgVector[ArgIdx]));
            SvcRawEnc_ProcCallBack(SVC_RAW_ENC_CODE_UPD_IQPATH, &UpdIQPath);
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellCfgIqTblPhotoU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "    ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                TableNo : Configure iq table photo index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                   Path : Configure iq table photo path" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellCfgIqTblVidMSM(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 3U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 TableNo = 0xFFFFFFFFU;
            SVC_RAW_ENC_UPD_IQPATH UpdIQPath;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &TableNo);    ArgIdx ++;
            AmbaSvcWrap_MisraMemset(&UpdIQPath, 0, sizeof(UpdIQPath));
            UpdIQPath.TableNo = TableNo;
            UpdIQPath.ID      = SVC_RAW_ENC_UPD_VIDEO_MSM_PATH;
            AmbaMisra_TypeCast(&(UpdIQPath.pPath), &(pArgVector[ArgIdx]));
            SvcRawEnc_ProcCallBack(SVC_RAW_ENC_CODE_UPD_IQPATH, &UpdIQPath);

        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellCfgIqTblVidMSMU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "    ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                TableNo : Configure iq table video msm index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                   Path : Configure iq table video msm path" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellCfgIqTblVidMSH(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 3U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 TableNo = 0xFFFFFFFFU;
            SVC_RAW_ENC_UPD_IQPATH UpdIQPath;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &TableNo);    ArgIdx ++;
            AmbaSvcWrap_MisraMemset(&UpdIQPath, 0, sizeof(UpdIQPath));
            UpdIQPath.TableNo = TableNo;
            UpdIQPath.ID      = SVC_RAW_ENC_UPD_VIDEO_MSH_PATH;
            AmbaMisra_TypeCast(&(UpdIQPath.pPath), &(pArgVector[ArgIdx]));
            SvcRawEnc_ProcCallBack(SVC_RAW_ENC_CODE_UPD_IQPATH, &UpdIQPath);
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellCfgIqTblVidMSHU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "    ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                TableNo : Configure iq table video msh index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                   Path : Configure iq table video msh path" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellCfgIqTblPhoMSM(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 3U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 TableNo = 0xFFFFFFFFU;
            SVC_RAW_ENC_UPD_IQPATH UpdIQPath;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &TableNo);    ArgIdx ++;
            AmbaSvcWrap_MisraMemset(&UpdIQPath, 0, sizeof(UpdIQPath));
            UpdIQPath.TableNo = TableNo;
            UpdIQPath.ID      = SVC_RAW_ENC_UPD_PHOTO_MSM_PATH;
            AmbaMisra_TypeCast(&(UpdIQPath.pPath), &(pArgVector[ArgIdx]));
            SvcRawEnc_ProcCallBack(SVC_RAW_ENC_CODE_UPD_IQPATH, &UpdIQPath);
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellCfgIqTblPhoMSMU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "    ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                TableNo : Configure iq table photo msm index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                   Path : Configure iq table photo msm path" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellCfgIqTblPhoMSH(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 3U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 TableNo = 0xFFFFFFFFU;
            SVC_RAW_ENC_UPD_IQPATH UpdIQPath;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &TableNo);    ArgIdx ++;
            AmbaSvcWrap_MisraMemset(&UpdIQPath, 0, sizeof(UpdIQPath));
            UpdIQPath.TableNo = TableNo;
            UpdIQPath.ID      = SVC_RAW_ENC_UPD_PHOTO_MSH_PATH;
            AmbaMisra_TypeCast(&(UpdIQPath.pPath), &(pArgVector[ArgIdx]));
            SvcRawEnc_ProcCallBack(SVC_RAW_ENC_CODE_UPD_IQPATH, &UpdIQPath);
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellCfgIqTblPhoMSHU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "    ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                TableNo : Configure iq table photo msh index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                   Path : Configure iq table photo msh path" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellCfgIqTblSLiso(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 3U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 TableNo = 0xFFFFFFFFU;
            SVC_RAW_ENC_UPD_IQPATH UpdIQPath;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &TableNo);    ArgIdx ++;
            AmbaSvcWrap_MisraMemset(&UpdIQPath, 0, sizeof(UpdIQPath));
            UpdIQPath.TableNo = TableNo;
            UpdIQPath.ID      = SVC_RAW_ENC_UPD_SLISO_PATH;
            AmbaMisra_TypeCast(&(UpdIQPath.pPath), &(pArgVector[ArgIdx]));
            SvcRawEnc_ProcCallBack(SVC_RAW_ENC_CODE_UPD_IQPATH, &UpdIQPath);
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellCfgIqTblSLisoU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "    ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                TableNo : Configure iq table still low iso index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                   Path : Configure iq table still low iso path" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellCfgIqTblSHiso(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 3U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 TableNo = 0xFFFFFFFFU;
            SVC_RAW_ENC_UPD_IQPATH UpdIQPath;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &TableNo);    ArgIdx ++;
            AmbaSvcWrap_MisraMemset(&UpdIQPath, 0, sizeof(UpdIQPath));
            UpdIQPath.TableNo = TableNo;
            UpdIQPath.ID      = SVC_RAW_ENC_UPD_SHISO_PATH;
            AmbaMisra_TypeCast(&(UpdIQPath.pPath), &(pArgVector[ArgIdx]));
            SvcRawEnc_ProcCallBack(SVC_RAW_ENC_CODE_UPD_IQPATH, &UpdIQPath);
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellCfgIqTblSHisoU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "    ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                TableNo : Configure iq table still high iso index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                   Path : Configure iq table still high iso path" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellCfgIqTblVidCc(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 4U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 CcSetIdx = 0xFFFFFFFFU;
            UINT32 TableNo  = 0xFFFFFFFFU;
            SVC_RAW_ENC_UPD_IQPATH UpdIQPath;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &CcSetIdx); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &TableNo);  ArgIdx ++;

            AmbaSvcWrap_MisraMemset(&UpdIQPath, 0, sizeof(UpdIQPath));
            UpdIQPath.TableNo     = TableNo;
            UpdIQPath.CCSetPathNo = CcSetIdx;
            UpdIQPath.ID          = SVC_RAW_ENC_UPD_VIDCC_PATH;
            AmbaMisra_TypeCast(&(UpdIQPath.pPath), &(pArgVector[ArgIdx]));
            SvcRawEnc_ProcCallBack(SVC_RAW_ENC_CODE_UPD_IQPATH, &UpdIQPath);
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellCfgIqTblVidCcU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "    ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "               CcSetIdx : Configure iq table video cc set index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                TableNo : Configure iq table video cc index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                   Path : Configure iq table video cc path" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellCfgIqTblStiCc(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 4U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 CcSetIdx = 0xFFFFFFFFU;
            UINT32 TableNo  = 0xFFFFFFFFU;
            SVC_RAW_ENC_UPD_IQPATH UpdIQPath;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &CcSetIdx); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &TableNo);  ArgIdx ++;

            AmbaSvcWrap_MisraMemset(&UpdIQPath, 0, sizeof(UpdIQPath));
            UpdIQPath.TableNo     = TableNo;
            UpdIQPath.CCSetPathNo = CcSetIdx;
            UpdIQPath.ID          = SVC_RAW_ENC_UPD_STLCC_PATH;
            AmbaMisra_TypeCast(&(UpdIQPath.pPath), &(pArgVector[ArgIdx]));
            SvcRawEnc_ProcCallBack(SVC_RAW_ENC_CODE_UPD_IQPATH, &UpdIQPath);
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellCfgIqTblStiCcU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "    ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "               CcSetIdx : Configure iq table still cc set index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                TableNo : Configure iq table still cc index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                   Path : Configure iq table still cc path" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellCfgIqTblScene(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 3U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 TableNo = 0xFFFFFFFFU;
            SVC_RAW_ENC_UPD_IQPATH UpdIQPath;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &TableNo);    ArgIdx ++;
            AmbaSvcWrap_MisraMemset(&UpdIQPath, 0, sizeof(UpdIQPath));
            UpdIQPath.TableNo = TableNo;
            UpdIQPath.ID      = SVC_RAW_ENC_UPD_SCEEN_PATH;
            AmbaMisra_TypeCast(&(UpdIQPath.pPath), &(pArgVector[ArgIdx]));
            SvcRawEnc_ProcCallBack(SVC_RAW_ENC_CODE_UPD_IQPATH, &UpdIQPath);
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellCfgIqTblSceneU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "    ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                TableNo : Configure iq table photo index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                   Path : Configure iq table photo path" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellCfgIqTblDVid(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 3U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 TableNo = 0xFFFFFFFFU;
            SVC_RAW_ENC_UPD_IQPATH UpdIQPath;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &TableNo); ArgIdx ++;
            AmbaSvcWrap_MisraMemset(&UpdIQPath, 0, sizeof(UpdIQPath));
            UpdIQPath.TableNo = TableNo;
            UpdIQPath.ID      = SVC_RAW_ENC_UPD_DVID_PATH;
            AmbaMisra_TypeCast(&(UpdIQPath.pPath), &(pArgVector[ArgIdx]));
            SvcRawEnc_ProcCallBack(SVC_RAW_ENC_CODE_UPD_IQPATH, &UpdIQPath);
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellCfgIqTblDVidU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "    ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                TableNo : Configure iq table digital effect video index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                   Path : Configure iq table digital effect video path" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellCfgIqTblDSti(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 3U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 TableNo = 0xFFFFFFFFU;
            SVC_RAW_ENC_UPD_IQPATH UpdIQPath;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &TableNo); ArgIdx ++;
            AmbaSvcWrap_MisraMemset(&UpdIQPath, 0, sizeof(UpdIQPath));
            UpdIQPath.TableNo = TableNo;
            UpdIQPath.ID      = SVC_RAW_ENC_UPD_DSTL_PATH;
            AmbaMisra_TypeCast(&(UpdIQPath.pPath), &(pArgVector[ArgIdx]));
            SvcRawEnc_ProcCallBack(SVC_RAW_ENC_CODE_UPD_IQPATH, &UpdIQPath);
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellCfgIqTblDStiU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "    ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                TableNo : Configure iq table digital effect still index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                   Path : Configure iq table digital effect still path" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellCfgIqTblVidPrm(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 2U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            SVC_RAW_ENC_UPD_IQPATH UpdIQPath;
            AmbaSvcWrap_MisraMemset(&UpdIQPath, 0, sizeof(UpdIQPath));
            UpdIQPath.ID = SVC_RAW_ENC_UPD_VID_PARAM_PATH;
            AmbaMisra_TypeCast(&(UpdIQPath.pPath), &(pArgVector[ArgIdx]));
            SvcRawEnc_ProcCallBack(SVC_RAW_ENC_CODE_UPD_IQPATH, &UpdIQPath);
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellCfgIqTblVidPrmU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "    ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                   Path : Configure iq table video param path" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellCfgIqTblStlPrm(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 2U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            SVC_RAW_ENC_UPD_IQPATH UpdIQPath;
            AmbaSvcWrap_MisraMemset(&UpdIQPath, 0, sizeof(UpdIQPath));
            UpdIQPath.ID = SVC_RAW_ENC_UPD_STL_PARAM_PATH;
            AmbaMisra_TypeCast(&(UpdIQPath.pPath), &(pArgVector[ArgIdx]));
            SvcRawEnc_ProcCallBack(SVC_RAW_ENC_CODE_UPD_IQPATH, &UpdIQPath);
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellCfgIqTblStlPrmU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "    ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                   Path : Configure iq table still param path" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellResCfgVin(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    return SvcRawEnc_ShellCommEntry(ArgCount, pArgVector, pFunc, pData);
}

static UINT32 SvcRawEnc_ShellResCfgSen(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 6U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 VinID      = 0xFFFFFFFFU;
            UINT32 SensorIdx  = 0xFFFFFFFFU;
            UINT32 SensorMode = 0xFFFFFFFFU;
            UINT32 SensorGrp  = 0xFFFFFFFFU;
            UINT32 IqTable    = 0xFFFFFFFFU;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &VinID     ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &SensorIdx ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &SensorMode); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &SensorGrp ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &IqTable   );
            if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "invalid VinID(%d)!"
                    PRN_RENC_ARG_UINT32 VinID PRN_RENC_ARG_POST
                PRN_RENC_SHELL_NG
            } else if (SensorIdx >= 16U) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "invalid Sensor index(%d)!"
                    PRN_RENC_ARG_UINT32 SensorIdx PRN_RENC_ARG_POST
                PRN_RENC_SHELL_NG
            } else {
                pRawEncResCfg->SensorCfg[VinID][SensorIdx].SensorMode  = SensorMode;
                pRawEncResCfg->SensorCfg[VinID][SensorIdx].SensorGroup = SensorGrp;
                pRawEncResCfg->SensorCfg[VinID][SensorIdx].IQTable     = IqTable;
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellResCfgSenU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                  VinID : configure the vin id" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "              SensorIdx : configure the sensor index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "             SensorMode : configure the sensor mode" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "              SensorGrp : configure the sensor group setting" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                IqTable : configure the iq table" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellResCfgFov(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    return SvcRawEnc_ShellCommEntry(ArgCount, pArgVector, pFunc, pData);
}

static UINT32 SvcRawEnc_ShellResCfgDisp(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    return SvcRawEnc_ShellCommEntry(ArgCount, pArgVector, pFunc, pData);
}

static UINT32 SvcRawEnc_ShellResCfgRec(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    return SvcRawEnc_ShellCommEntry(ArgCount, pArgVector, pFunc, pData);
}

static UINT32 SvcRawEnc_ShellResCfgReset(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 1U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            UINT32 VinIdx;
            void *pSsObj = NULL;

            SVC_RawEncCtrlFlag &= ~(SVC_RAW_ENC_FLG_DEF_RES_CFG);
            SvcRawEnc_DefResCfgInit();

            for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
                if (pRawEncResCfg->VinCfg[VinIdx].pDriver != NULL) {
                    pSsObj = pRawEncResCfg->VinCfg[VinIdx].pDriver;
                    break;
                }
            }

            AmbaSvcWrap_MisraMemset(pRawEncResCfg, 0, sizeof(SVC_RES_CFG_s));
            for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
                pRawEncResCfg->VinCfg[VinIdx].pDriver = pSsObj;
            }

            PRN_RENC_LOG "Success reset all video raw enc res_cfg, except sensor object" PRN_RENC_API
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static UINT32 SvcRawEnc_ShellResCfgInfo(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 1U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            SvcRawEnc_ResCfgPrint(pRawEncResCfg);
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static UINT32 SvcRawEnc_ShellResCfgVinTree(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 2U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            AmbaSvcWrap_MisraMemset(pRawEncResCfg->VinTree, 0, sizeof(pRawEncResCfg->VinTree));
            SvcWrap_strcpy(pRawEncResCfg->VinTree, sizeof(pRawEncResCfg->VinTree), pArgVector[ArgIdx]);
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellResCfgVinTreeU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                VinTree : configure resolution cfg vin tree string" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellResCfgVinNum(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 2U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 NumVin = 0U;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &NumVin);
            pRawEncResCfg->VinNum = NumVin;
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellResCfgVinNumU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                 Number : vin number" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellResCfgVinWin(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 6U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 VinID = 0U;
            UINT32 OffsetX = 0U;
            UINT32 OffsetY = 0U;
            UINT32 Width   = 0U;
            UINT32 Height  = 0U;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &VinID);   ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &OffsetX); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &OffsetY); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &Width  ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &Height );

            if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "VinID(%d) is out-of range!"
                    PRN_RENC_ARG_UINT32 VinID PRN_RENC_ARG_POST
                PRN_RENC_SHELL_NG
            } else {
                pRawEncResCfg->VinCfg[VinID].CapWin.OffsetX = (UINT16) ( OffsetX & 0x0000FFFFU );
                pRawEncResCfg->VinCfg[VinID].CapWin.OffsetY = (UINT16) ( OffsetY & 0x0000FFFFU );
                pRawEncResCfg->VinCfg[VinID].CapWin.Width   = (UINT16) ( Width   & 0x0000FFFFU );
                pRawEncResCfg->VinCfg[VinID].CapWin.Height  = (UINT16) ( Height  & 0x0000FFFFU );

                RawEncResCfgVinID = VinID;
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellResCfgVinWinU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                  VinID : configure the vin id" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                OffsetX : configure capture offset_x" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                OffsetY : configure capture offset_y" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                  Width : configure capture width" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                 Height : configure capture height" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellResCfgVinSCNum(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 3U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 VinID = 0U;
            UINT32 SubChanNum = 0U;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &VinID);      ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &SubChanNum);

            if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
                PRN_RENC_LOG "invalid VinID(%d)!"
                    PRN_RENC_ARG_UINT32 VinID PRN_RENC_ARG_POST
                PRN_RENC_SHELL_NG
            } else if (SubChanNum > AMBA_DSP_MAX_VIRT_CHAN_NUM) {
                PRN_RENC_LOG "input sub-channel number(%d) out-of limitation(%d)!"
                    PRN_RENC_ARG_UINT32 SubChanNum                 PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 AMBA_DSP_MAX_VIRT_CHAN_NUM PRN_RENC_ARG_POST
                PRN_RENC_SHELL_NG
            } else {
                pRawEncResCfg->VinCfg[VinID].SubChanNum = SubChanNum;
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellResCfgVinSCNumU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                  VinID : configure the vin id" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "             SubChanNum : configure the sub channel number" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellResCfgVinSChan(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 12U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 VinID        = 0U;
            UINT32 CfgIdx       = 0U;
            UINT32 IsVirtual    = 0U;
            UINT32 Index        = 0U;
            UINT32 Option       = 0U;
            UINT32 ConCatNum    = 0U;
            UINT32 IntcNum      = 0U;
            UINT32 CapWinOfsX   = 0U;
            UINT32 CapWinOfsY   = 0U;
            UINT32 CapWinWidth  = 0U;
            UINT32 CapWinHeight = 0U;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &VinID       ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &CfgIdx      ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &IsVirtual   ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &Index       ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &Option      ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &ConCatNum   ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &IntcNum     ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &CapWinOfsX  ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &CapWinOfsY  ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &CapWinWidth ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &CapWinHeight);

            if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
                PRN_RENC_LOG "invalid VinID(%d)!"
                    PRN_RENC_ARG_UINT32 VinID PRN_RENC_ARG_POST
                PRN_RENC_SHELL_NG
            } else if (CfgIdx >= AMBA_DSP_MAX_VIRT_CHAN_NUM) {
                PRN_RENC_LOG "input config index(%d) out-of limitation(%d)!"
                    PRN_RENC_ARG_UINT32 CfgIdx                     PRN_RENC_ARG_POST
                    PRN_RENC_ARG_UINT32 AMBA_DSP_MAX_VIRT_CHAN_NUM PRN_RENC_ARG_POST
                PRN_RENC_SHELL_NG
            } else {
                pRawEncResCfg->VinCfg[VinID].SubChanCfg[CfgIdx].SubChan.IsVirtual     = (UINT16)IsVirtual   ;
                pRawEncResCfg->VinCfg[VinID].SubChanCfg[CfgIdx].SubChan.Index         = (UINT16)Index       ;
                pRawEncResCfg->VinCfg[VinID].SubChanCfg[CfgIdx].Option                = (UINT16)Option      ;
                pRawEncResCfg->VinCfg[VinID].SubChanCfg[CfgIdx].ConCatNum             = (UINT16)ConCatNum   ;
                pRawEncResCfg->VinCfg[VinID].SubChanCfg[CfgIdx].IntcNum               = (UINT16)IntcNum     ;
                pRawEncResCfg->VinCfg[VinID].SubChanCfg[CfgIdx].CaptureWindow.OffsetX = (UINT16)CapWinOfsX  ;
                pRawEncResCfg->VinCfg[VinID].SubChanCfg[CfgIdx].CaptureWindow.OffsetY = (UINT16)CapWinOfsY  ;
                pRawEncResCfg->VinCfg[VinID].SubChanCfg[CfgIdx].CaptureWindow.Width   = (UINT16)CapWinWidth ;
                pRawEncResCfg->VinCfg[VinID].SubChanCfg[CfgIdx].CaptureWindow.Height  = (UINT16)CapWinHeight;
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellResCfgVinSChanU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                  VinID : configure the vin id" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                 CfgIdx : configure the sub channel configuration index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "              IsVirtual : Is virtual channel or not" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                  Index : sub channel index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                 Option : capture option" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "              ConCatNum : Concate number" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                IntcNum : interleave number" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "             CapWinOfsX : Capture window offset X" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "             CapWinOfsY : Capture window offset Y" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "            CapWinWidth : Capture window width" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "           CapWinHeight : Capture window height" PRN_RENC_SHELL_USAGE

    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellResCfgVinFrate(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 5U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 VinID          = 0U;
            UINT32 Interlace      = 0U;
            UINT32 TimeScale      = 0U;
            UINT32 NumUnitsInTick = 0U;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &VinID          ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &Interlace      ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &TimeScale      ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &NumUnitsInTick );

            if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
                PRN_RENC_LOG "invalid VinID(%d)!"
                    PRN_RENC_ARG_UINT32 VinID PRN_RENC_ARG_POST
                PRN_RENC_SHELL_NG
            } else {
                pRawEncResCfg->VinCfg[VinID].FrameRate.Interlace      = (UINT8)Interlace;
                pRawEncResCfg->VinCfg[VinID].FrameRate.TimeScale      = TimeScale     ;
                pRawEncResCfg->VinCfg[VinID].FrameRate.NumUnitsInTick = NumUnitsInTick;
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellResCfgVinFrateU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                  VinID : configure the vin id" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "              Interlace : configure vin framerate" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "              TimeScale : " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "         NumUnitsInTick : " PRN_RENC_SHELL_USAGE

    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellResCfgFovNum(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 2U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 NumFov = 0U;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &NumFov);
            pRawEncResCfg->FovNum = NumFov;
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellResCfgFovNumU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                 Number : fov number" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellResCfgFovRaw(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 6U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 FovID   = 0U;
            UINT32 OffsetX = 0U;
            UINT32 OffsetY = 0U;
            UINT32 Width   = 0U;
            UINT32 Height  = 0U;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &FovID  ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &OffsetX); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &OffsetY); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &Width  ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &Height );

            if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "FovID(%d) is out-of range!"
                    PRN_RENC_ARG_UINT32 FovID PRN_RENC_ARG_POST
                PRN_RENC_SHELL_NG
            } else {
                pRawEncResCfg->FovCfg[FovID].RawWin.OffsetX = (UINT16) ( OffsetX & 0x0000FFFFU );
                pRawEncResCfg->FovCfg[FovID].RawWin.OffsetY = (UINT16) ( OffsetY & 0x0000FFFFU );
                pRawEncResCfg->FovCfg[FovID].RawWin.Width   = (UINT16) ( Width   & 0x0000FFFFU );
                pRawEncResCfg->FovCfg[FovID].RawWin.Height  = (UINT16) ( Height  & 0x0000FFFFU );
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellResCfgFovRawU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                  FovID : configure the fov id" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                OffsetX : configure for raw offset_x" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                OffsetY : configure for raw offset_y" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                  Width : configure for raw width" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                 Height : configure for raw height" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellResCfgFovAct(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 6U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 FovID   = 0U;
            UINT32 OffsetX = 0U;
            UINT32 OffsetY = 0U;
            UINT32 Width   = 0U;
            UINT32 Height  = 0U;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &FovID  ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &OffsetX); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &OffsetY); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &Width  ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &Height );

            if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "FovID(%d) is out-of range!"
                    PRN_RENC_ARG_UINT32 FovID PRN_RENC_ARG_POST
                PRN_RENC_SHELL_NG
            } else {
                pRawEncResCfg->FovCfg[FovID].ActWin.OffsetX = (UINT16) ( OffsetX & 0x0000FFFFU );
                pRawEncResCfg->FovCfg[FovID].ActWin.OffsetY = (UINT16) ( OffsetY & 0x0000FFFFU );
                pRawEncResCfg->FovCfg[FovID].ActWin.Width   = (UINT16) ( Width   & 0x0000FFFFU );
                pRawEncResCfg->FovCfg[FovID].ActWin.Height  = (UINT16) ( Height  & 0x0000FFFFU );
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellResCfgFovActU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                  FovID : configure the fov id" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                OffsetX : configure for act offset_x" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                OffsetY : configure for act offset_y" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                  Width : configure for act width" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                 Height : configure for act height" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellResCfgFovMain(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 4U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 FovID   = 0U;
            UINT32 Width   = 0U;
            UINT32 Height  = 0U;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &FovID  ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &Width  ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &Height );

            if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "FovID(%d) is out-of range!"
                    PRN_RENC_ARG_UINT32 FovID PRN_RENC_ARG_POST
                PRN_RENC_SHELL_NG
            } else {
                pRawEncResCfg->FovCfg[FovID].MainWin.Width   = (UINT16) ( Width   & 0x0000FFFFU );
                pRawEncResCfg->FovCfg[FovID].MainWin.Height  = (UINT16) ( Height  & 0x0000FFFFU );
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellResCfgFovMainU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                  FovID : configure the fov id" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                  Width : configure for main width" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                 Height : configure for main height" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellResCfgFovPyr(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 3U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 FovID   = 0U;
            UINT32 SelBits = 0U;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &FovID);   ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &SelBits);

            if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "FovID(%d) is out-of range!"
                    PRN_RENC_ARG_UINT32 FovID PRN_RENC_ARG_POST
                PRN_RENC_SHELL_NG
            } else {
                pRawEncResCfg->FovCfg[FovID].PyramidBits = SelBits;
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellResCfgFovPyrU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                  FovID : configure the fov id" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "             SelectBits : configure pyramid select bits" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellResCfgFovHier(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 4U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 FovID   = 0U;
            UINT32 Width   = 0U;
            UINT32 Height  = 0U;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &FovID ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &Width ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &Height);

            if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "FovID(%d) is out-of range!"
                    PRN_RENC_ARG_UINT32 FovID PRN_RENC_ARG_POST
                PRN_RENC_SHELL_NG
            } else {
                pRawEncResCfg->FovCfg[FovID].HierWin.Width   = (UINT16) ( Width   & 0x0000FFFFU );
                pRawEncResCfg->FovCfg[FovID].HierWin.Height  = (UINT16) ( Height  & 0x0000FFFFU );
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellResCfgFovHierU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                  FovID : configure the fov id" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                  Width : configure for hier width" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                 Height : configure for hier height" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellResCfgFovPipe(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 8U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 FovID          = 0U;
            UINT32 RotateFlip     = 0U;
            UINT32 HdrExposureNum = 0U;
            UINT32 HdrBlendHieght = 0U;
            UINT32 MctfDisable    = 0U;
            UINT32 MctsDisable    = 0U;
            UINT32 LinearCE       = 0U;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &FovID         ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &RotateFlip    ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &HdrExposureNum); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &HdrBlendHieght); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &MctfDisable   ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &MctsDisable   ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &LinearCE      );

            if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "FovID(%d) is out-of range!"
                    PRN_RENC_ARG_UINT32 FovID PRN_RENC_ARG_POST
                PRN_RENC_SHELL_NG
            } else {
                pRawEncResCfg->FovCfg[FovID].PipeCfg.RotateFlip     = RotateFlip;
                pRawEncResCfg->FovCfg[FovID].PipeCfg.HdrExposureNum = HdrExposureNum;
                pRawEncResCfg->FovCfg[FovID].PipeCfg.HdrBlendHieght = HdrBlendHieght;
                pRawEncResCfg->FovCfg[FovID].PipeCfg.MctfDisable    = MctfDisable;
                pRawEncResCfg->FovCfg[FovID].PipeCfg.MctsDisable    = MctsDisable;
                pRawEncResCfg->FovCfg[FovID].PipeCfg.LinearCE       = LinearCE;
                pRawEncResCfg->FovCfg[FovID].PipeCfg.RawCompression = 0U;
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellResCfgFovPipeU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                  FovID : configure the fov id" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "             RotateFlip : configure the fov rotation and flip" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "         HdrExposureNum : configure the fov hdr exposure" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "         HdrBlendHieght : configure the fov hdr blending height" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "            MctfDisable : configure the fov mctf" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "            MctsDisable : configure the fov mcts" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "               LinearCE : configure the fov ce" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellResCfgDispBits(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 2U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 SelBits = 0U, Idx, DispNum = 0U;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &SelBits);

            for (Idx = 0U; Idx < AMBA_DSP_MAX_VOUT_NUM; Idx ++) {
                if ((SelBits & SvcRawEnc_BitGet(Idx)) > 0U) {
                    DispNum += 1U;
                }
            }

            if (DispNum > 0U) {
                pRawEncResCfg->DispBits = SelBits;
                pRawEncResCfg->DispNum  = DispNum;
            } else {
                RetVal = SVC_NG;
                PRN_RENC_LOG "Fail tp proc res cfg 'disp->sel_bits' - invalid select bits value!" PRN_RENC_NG
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellResCfgDispBitsU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "             SelectBits : select bits value" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellResCfgDispId(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 3U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 DispIdx = 0U;
            UINT32 VoutID  = 0U;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &DispIdx); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &VoutID);

            if (DispIdx >= AMBA_DSP_MAX_VOUT_NUM) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "display index(%d) is out-of range!"
                    PRN_RENC_ARG_UINT32 DispIdx PRN_RENC_ARG_POST
                PRN_RENC_SHELL_NG
            } else {
                pRawEncResCfg->DispStrm[DispIdx].VoutID = VoutID;
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellResCfgDispIdU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                DispIdx : display index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                 VoutID : configure vout id" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellResCfgDispObj(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 3U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 DispIdx = 0U;
            UINT32 ObjID   = 0xCafeU;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &DispIdx); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &ObjID);

            if (DispIdx >= AMBA_DSP_MAX_VOUT_NUM) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "display index(%d) is out-of range!"
                    PRN_RENC_ARG_UINT32 DispIdx PRN_RENC_ARG_POST
                PRN_RENC_SHELL_NG
            } else {
                UINT32 Idx;
                AMBA_FPD_OBJECT_s *pObj = NULL;

                for (Idx = 0U; Idx < SVC_RAW_ENC_MAX_VOUT_DEV_NUM; Idx ++) {
                    if (RawEncVoutDevInfo[Idx].ID < SVC_RAW_ENC_MAX_VOUT_DEV_NUM) {
                        if (RawEncVoutDevInfo[Idx].ID == ObjID) {
                            pObj = RawEncVoutDevInfo[Idx].pObj;
                            break;
                        }
                    }
                }

                if (pObj != NULL) {
                    pRawEncResCfg->DispStrm[DispIdx].pDriver = pObj;
                } else {
                    PRN_RENC_LOG "The vout driver object id(%d) is not exist!"
                        PRN_RENC_ARG_UINT32 ObjID PRN_RENC_ARG_POST
                    PRN_RENC_NG
                }
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellResCfgDispObjU(UINT32 CtrlFlag, void *pFunc)
{
    UINT32 Idx;

    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                DispIdx : display index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                  ObjID : supported driver object id" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                        :  ID | Driver Obj | Name" PRN_RENC_SHELL_USAGE
    for (Idx = 0U; Idx < SVC_RAW_ENC_MAX_VOUT_DEV_NUM; Idx ++) {
        if (RawEncVoutDevInfo[Idx].ID < SVC_RAW_ENC_MAX_VOUT_DEV_NUM) {
            PRN_RENC_LOG "                        : %03d | %p | %s"
                PRN_RENC_ARG_UINT32 RawEncVoutDevInfo[Idx].ID      PRN_RENC_ARG_POST
                PRN_RENC_ARG_CPOINT RawEncVoutDevInfo[Idx].pObj    PRN_RENC_ARG_POST
                PRN_RENC_ARG_CSTR   RawEncVoutDevInfo[Idx].DevName PRN_RENC_ARG_POST
                PRN_RENC_SHELL_USAGE
        }
    }
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellResCfgDispWin(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 4U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 DispIdx = 0U;
            UINT32 Width   = 0U;
            UINT32 Height  = 0U;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &DispIdx); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &Width  ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &Height );

            if (DispIdx >= AMBA_DSP_MAX_VOUT_NUM) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "display index(%d) is out-of range!"
                    PRN_RENC_ARG_UINT32 DispIdx PRN_RENC_ARG_POST
                PRN_RENC_SHELL_NG
            } else {
                pRawEncResCfg->DispStrm[DispIdx].StrmCfg.Win.Width  = (UINT16) ( Width  & 0x0000FFFFU );
                pRawEncResCfg->DispStrm[DispIdx].StrmCfg.Win.Height = (UINT16) ( Height & 0x0000FFFFU );
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellResCfgDispWinU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                DispIdx : display index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                  Width : configure disp cfg win width" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                 Height : configure disp cfg win height" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellResCfgDispMWin(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 4U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 DispIdx = 0U;
            UINT32 Width   = 0U;
            UINT32 Height  = 0U;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &DispIdx); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &Width  ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &Height );

            if (DispIdx >= AMBA_DSP_MAX_VOUT_NUM) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "display index(%d) is out-of range!"
                    PRN_RENC_ARG_UINT32 DispIdx PRN_RENC_ARG_POST
                PRN_RENC_SHELL_NG
            } else {
                pRawEncResCfg->DispStrm[DispIdx].StrmCfg.MaxWin.Width  = (UINT16) ( Width  & 0x0000FFFFU );
                pRawEncResCfg->DispStrm[DispIdx].StrmCfg.MaxWin.Height = (UINT16) ( Height & 0x0000FFFFU );
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellResCfgDispMWinU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                DispIdx : display index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                  Width : configure disp cfg max win width" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                 Height : configure disp cfg max win height" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellResCfgDispCNum(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 3U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 DispIdx = 0U;
            UINT32 ChanNum = 0U;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &DispIdx); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &ChanNum);

            if (DispIdx >= AMBA_DSP_MAX_VOUT_NUM) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "display index(%d) is out-of range!"
                    PRN_RENC_ARG_UINT32 DispIdx PRN_RENC_ARG_POST
                PRN_RENC_SHELL_NG
            } else {
                pRawEncResCfg->DispStrm[DispIdx].StrmCfg.NumChan = ChanNum;
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellResCfgDispCNumU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                DispIdx : display index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                ChanNum : configure channel number" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellResCfgDispCCfg(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 12U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 DispIdx    = 0U;
            UINT32 ChanIdx    = 0U;
            UINT32 FovID      = 0U;
            UINT32 SrcOffsetX = 0U;
            UINT32 SrcOffsetY = 0U;
            UINT32 SrcWidth   = 0U;
            UINT32 SrcHeight  = 0U;
            UINT32 DstOffsetX = 0U;
            UINT32 DstOffsetY = 0U;
            UINT32 DstWidth   = 0U;
            UINT32 DstHeight  = 0U;
            UINT32 MarginTop    = 0U;
            UINT32 MarginRight  = 0U;
            UINT32 MarginBottom = 0U;
            UINT32 MarginLeft   = 0U;
            UINT32 RotateFlip   = 0U;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &DispIdx   ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &ChanIdx   ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &FovID     ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &SrcOffsetX); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &SrcOffsetY); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &SrcWidth  ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &SrcHeight ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &DstOffsetX); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &DstOffsetY); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &DstWidth  ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &DstHeight );

            if (ArgCount >= (ReqArgCount + 5U)) {
                ArgIdx ++;
                SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &MarginTop    ); ArgIdx ++;
                SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &MarginRight  ); ArgIdx ++;
                SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &MarginBottom ); ArgIdx ++;
                SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &MarginLeft   ); ArgIdx ++;
                SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &RotateFlip   );
            }

            if (DispIdx >= AMBA_DSP_MAX_VOUT_NUM) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "display index(%d) is out-of range!"
                    PRN_RENC_ARG_UINT32 DispIdx PRN_RENC_ARG_POST
                PRN_RENC_SHELL_NG
            } else {
                if (ChanIdx >= AMBA_DSP_MAX_YUVSTRM_VIEW_NUM) {
                    RetVal = SVC_NG;
                    PRN_RENC_LOG "channel index(%d) is out-of range!"
                        PRN_RENC_ARG_UINT32 ChanIdx PRN_RENC_ARG_POST
                    PRN_RENC_SHELL_NG
                } else {
                    pRawEncResCfg->DispStrm[DispIdx].StrmCfg.ChanCfg[ChanIdx].FovId          = FovID;
                    pRawEncResCfg->DispStrm[DispIdx].StrmCfg.ChanCfg[ChanIdx].SrcWin.OffsetX = (UINT16) ( SrcOffsetX   & 0x0000FFFFU );
                    pRawEncResCfg->DispStrm[DispIdx].StrmCfg.ChanCfg[ChanIdx].SrcWin.OffsetY = (UINT16) ( SrcOffsetY   & 0x0000FFFFU );
                    pRawEncResCfg->DispStrm[DispIdx].StrmCfg.ChanCfg[ChanIdx].SrcWin.Width   = (UINT16) ( SrcWidth     & 0x0000FFFFU );
                    pRawEncResCfg->DispStrm[DispIdx].StrmCfg.ChanCfg[ChanIdx].SrcWin.Height  = (UINT16) ( SrcHeight    & 0x0000FFFFU );
                    pRawEncResCfg->DispStrm[DispIdx].StrmCfg.ChanCfg[ChanIdx].DstWin.OffsetX = (UINT16) ( DstOffsetX   & 0x0000FFFFU );
                    pRawEncResCfg->DispStrm[DispIdx].StrmCfg.ChanCfg[ChanIdx].DstWin.OffsetY = (UINT16) ( DstOffsetY   & 0x0000FFFFU );
                    pRawEncResCfg->DispStrm[DispIdx].StrmCfg.ChanCfg[ChanIdx].DstWin.Width   = (UINT16) ( DstWidth     & 0x0000FFFFU );
                    pRawEncResCfg->DispStrm[DispIdx].StrmCfg.ChanCfg[ChanIdx].DstWin.Height  = (UINT16) ( DstHeight    & 0x0000FFFFU );
                    pRawEncResCfg->DispStrm[DispIdx].StrmCfg.ChanCfg[ChanIdx].Margin.Top     = (UINT16) ( MarginTop    & 0x0000FFFFU );
                    pRawEncResCfg->DispStrm[DispIdx].StrmCfg.ChanCfg[ChanIdx].Margin.Right   = (UINT16) ( MarginRight  & 0x0000FFFFU );
                    pRawEncResCfg->DispStrm[DispIdx].StrmCfg.ChanCfg[ChanIdx].Margin.Bottom  = (UINT16) ( MarginBottom & 0x0000FFFFU );
                    pRawEncResCfg->DispStrm[DispIdx].StrmCfg.ChanCfg[ChanIdx].Margin.Left    = (UINT16) ( MarginLeft   & 0x0000FFFFU );
                    pRawEncResCfg->DispStrm[DispIdx].StrmCfg.ChanCfg[ChanIdx].RotateFlip     = (UINT8)    RotateFlip                  ;
                }
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellResCfgDispCCfgU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                DispIdx : display index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                ChanIdx : channel index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                  FovID : fov id" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "             SrcOffsetX : configure source window" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "             SrcOffsetY : configure source window" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "               SrcWidth : configure source window" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "              SrcHeight : configure source window" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "             DstOffsetX : configure destination window" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "             DstOffsetY : configure destination window" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "               DstWidth : configure destination window" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "              DstHeight : configure destination window" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "              MarginTop : configure margin top" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "            MarginRight : configure margin right" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "           MarginBottom : configure margin bottom" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "             MarginLeft : configure margin left" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "             RotateFlip : configure rotation/flip" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellResCfgDispFrat(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 5U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 DispIdx        = 0U;
            UINT32 Interlace      = 0U;
            UINT32 TimeScale      = 0U;
            UINT32 NumUnitsInTick = 0U;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &DispIdx        ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &Interlace      ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &TimeScale      ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &NumUnitsInTick );

            if (DispIdx >= AMBA_DSP_MAX_VOUT_NUM) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "display index(%d) is out-of range!"
                    PRN_RENC_ARG_UINT32 DispIdx PRN_RENC_ARG_POST
                PRN_RENC_SHELL_NG
            } else {
                pRawEncResCfg->DispStrm[DispIdx].FrameRate.Interlace      = (UINT8)Interlace;
                pRawEncResCfg->DispStrm[DispIdx].FrameRate.TimeScale      = TimeScale     ;
                pRawEncResCfg->DispStrm[DispIdx].FrameRate.NumUnitsInTick = NumUnitsInTick;
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellResCfgDispFratU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                DispIdx : display index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "              Interlace : configure disp framerate" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "              TimeScale : " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "         NumUnitsInTick : " PRN_RENC_SHELL_USAGE

    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellResCfgDispCfg(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 4U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 DispIdx    = 0U;
            UINT32 RotateFlip = 0U;
            UINT32 DevMode    = 0U;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &DispIdx    ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &RotateFlip ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &DevMode    );

            if (DispIdx >= AMBA_DSP_MAX_VOUT_NUM) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "display index(%d) is out-of range!"
                    PRN_RENC_ARG_UINT32 DispIdx PRN_RENC_ARG_POST
                PRN_RENC_SHELL_NG
            } else {
                pRawEncResCfg->DispStrm[DispIdx].VideoRotateFlip = (UINT8)RotateFlip;
                pRawEncResCfg->DispStrm[DispIdx].DevMode         = (UINT8)DevMode;
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellResCfgDispCfgU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                DispIdx : display index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "             RotateFlip : configure disp rotation/flip" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                DevMode : configure disp device mode" PRN_RENC_SHELL_USAGE

    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellResCfgRecBits(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 2U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 SelBits = 0U, Idx, RecNum = 0U;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &SelBits);

            for (Idx = 0U; Idx < AMBA_DSP_MAX_STREAM_NUM; Idx ++) {
                if ((SelBits & SvcRawEnc_BitGet(Idx)) > 0U) {
                    RecNum += 1U;
                }
            }

            if (RecNum > 0U) {
                pRawEncResCfg->RecBits = SelBits;
                pRawEncResCfg->RecNum  = RecNum;
            } else {
                RetVal = SVC_NG;
                PRN_RENC_LOG "Fail tp proc res cfg 'rec->sel_bits' - invalid select bits value!" PRN_RENC_NG
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellResCfgRecBitsU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "             SelectBits : select bits value" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellResCfgRecSetup(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 13U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 RecStrmIdx     = 0U;
            UINT32 RecId          = 0U;
            UINT32 SrcBits        = 0U;
            UINT32 DstBits        = 0U;
            UINT32 M              = 0U;
            UINT32 N              = 0U;
            UINT32 IdrInterval    = 0U;
            UINT32 BitRate        = 0U;
            UINT32 TimeLapse      = 0U;
            UINT32 Interlace      = 0U;
            UINT32 TimeScale      = 0U;
            UINT32 NumUnitsInTick = 0U;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &RecStrmIdx     ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &RecId          ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &SrcBits        ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &DstBits        ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &M              ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &N              ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &IdrInterval    ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &BitRate        ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &Interlace      ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &TimeScale      ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &NumUnitsInTick ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &TimeLapse      );

            if (RecStrmIdx >= AMBA_DSP_MAX_STREAM_NUM) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "record index(%d) is out-of range!"
                    PRN_RENC_ARG_UINT32 RecStrmIdx PRN_RENC_ARG_POST
                PRN_RENC_SHELL_NG
            } else {
                pRawEncResCfg->RecStrm[RecStrmIdx].RecSetting.RecId          = RecId;
                pRawEncResCfg->RecStrm[RecStrmIdx].RecSetting.SrcBits        = SrcBits;
                pRawEncResCfg->RecStrm[RecStrmIdx].RecSetting.DestBits       = DstBits;
                pRawEncResCfg->RecStrm[RecStrmIdx].RecSetting.M              = M          ;
                pRawEncResCfg->RecStrm[RecStrmIdx].RecSetting.N              = N          ;
                pRawEncResCfg->RecStrm[RecStrmIdx].RecSetting.IdrInterval    = IdrInterval;
                pRawEncResCfg->RecStrm[RecStrmIdx].RecSetting.BitRate        = BitRate    ;
                pRawEncResCfg->RecStrm[RecStrmIdx].RecSetting.TimeLapse      = TimeLapse  ;
                pRawEncResCfg->RecStrm[RecStrmIdx].RecSetting.FrameRate.Interlace      = (UINT8) ( Interlace & 0x000000FFU );
                pRawEncResCfg->RecStrm[RecStrmIdx].RecSetting.FrameRate.TimeScale      = TimeScale     ;
                pRawEncResCfg->RecStrm[RecStrmIdx].RecSetting.FrameRate.NumUnitsInTick = NumUnitsInTick;
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellResCfgRecSetupU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                 RecIdx : record stream index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                  RecId : configure record id" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                SrcBits : configure record source bits" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "               DestBits : configure record destination bits" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                      M : configure record M" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                      N : configure record N" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "            IdrInterval : configure record IDR interval" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                BitRate : configure record Bitrate" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "              Interlace : configure record framerate interlace" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "              TimeScale : configure record framerate time scale" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "         NumUnitsInTick : configure record framerate NumUnitsInTick" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "              TimeLapse : configure record timelapse" PRN_RENC_SHELL_USAGE

    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellResCfgRecWin(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 4U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 RecStrmIdx = 0U;
            UINT32 Width   = 0U;
            UINT32 Height  = 0U;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &RecStrmIdx); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &Width  );    ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &Height );

            if (RecStrmIdx >= AMBA_DSP_MAX_STREAM_NUM) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "record index(%d) is out-of range!"
                    PRN_RENC_ARG_UINT32 RecStrmIdx PRN_RENC_ARG_POST
                PRN_RENC_SHELL_NG
            } else {
                pRawEncResCfg->RecStrm[RecStrmIdx].StrmCfg.Win.Width  = (UINT16) ( Width  & 0x0000FFFFU );
                pRawEncResCfg->RecStrm[RecStrmIdx].StrmCfg.Win.Height = (UINT16) ( Height & 0x0000FFFFU );
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellResCfgRecWinU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                 RecIdx : record stream index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                  Width : configure record cfg win width" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                 Height : configure record cfg win height" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellResCfgRecMWin(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 4U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 RecStrmIdx = 0U;
            UINT32 Width   = 0U;
            UINT32 Height  = 0U;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &RecStrmIdx); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &Width  );    ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &Height );

            if (RecStrmIdx >= AMBA_DSP_MAX_STREAM_NUM) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "record index(%d) is out-of range!"
                    PRN_RENC_ARG_UINT32 RecStrmIdx PRN_RENC_ARG_POST
                PRN_RENC_SHELL_NG
            } else {
                pRawEncResCfg->RecStrm[RecStrmIdx].StrmCfg.MaxWin.Width  = (UINT16) ( Width  & 0x0000FFFFU );
                pRawEncResCfg->RecStrm[RecStrmIdx].StrmCfg.MaxWin.Height = (UINT16) ( Height & 0x0000FFFFU );
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellResCfgRecMWinU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                 RecIdx : record stream index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                  Width : configure record cfg max win width" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                 Height : configure record cfg max win height" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellResCfgRecCNum(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 3U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 RecStrmIdx = 0U;
            UINT32 ChanNum = 0U;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &RecStrmIdx); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &ChanNum);

            if (RecStrmIdx >= AMBA_DSP_MAX_STREAM_NUM) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "record index(%d) is out-of range!"
                    PRN_RENC_ARG_UINT32 RecStrmIdx PRN_RENC_ARG_POST
                PRN_RENC_SHELL_NG
            } else {
                pRawEncResCfg->RecStrm[RecStrmIdx].StrmCfg.NumChan = ChanNum;
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellResCfgRecCNumU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                 RecIdx : record stream index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                ChanNum : configure channel number" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellResCfgRecCCfg(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 12U;
        SVC_RES_CFG_s *pRawEncResCfg;

        AmbaMisra_TypeCast(&(pRawEncResCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pRawEncResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid res cfg" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 RecStrmIdx = 0U;
            UINT32 ChanIdx    = 0U;
            UINT32 FovID      = 0U;
            UINT32 SrcOffsetX = 0U;
            UINT32 SrcOffsetY = 0U;
            UINT32 SrcWidth   = 0U;
            UINT32 SrcHeight  = 0U;
            UINT32 DstOffsetX = 0U;
            UINT32 DstOffsetY = 0U;
            UINT32 DstWidth   = 0U;
            UINT32 DstHeight  = 0U;
            UINT32 MarginTop    = 0U;
            UINT32 MarginRight  = 0U;
            UINT32 MarginBottom = 0U;
            UINT32 MarginLeft   = 0U;
            UINT32 RotateFlip   = 0U;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &RecStrmIdx); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &ChanIdx   ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &FovID     ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &SrcOffsetX); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &SrcOffsetY); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &SrcWidth  ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &SrcHeight ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &DstOffsetX); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &DstOffsetY); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &DstWidth  ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &DstHeight );

            if (ArgCount >= (ReqArgCount + 5U)) {
                ArgIdx ++;
                SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &MarginTop    ); ArgIdx ++;
                SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &MarginRight  ); ArgIdx ++;
                SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &MarginBottom ); ArgIdx ++;
                SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &MarginLeft   ); ArgIdx ++;
                SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &RotateFlip   );
            }

            if (RecStrmIdx >= AMBA_DSP_MAX_STREAM_NUM) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "record index(%d) is out-of range!"
                    PRN_RENC_ARG_UINT32 RecStrmIdx PRN_RENC_ARG_POST
                PRN_RENC_SHELL_NG
            } else {
                if (ChanIdx >= AMBA_DSP_MAX_YUVSTRM_VIEW_NUM) {
                    RetVal = SVC_NG;
                    PRN_RENC_LOG "channel index(%d) is out-of range!"
                        PRN_RENC_ARG_UINT32 ChanIdx PRN_RENC_ARG_POST
                    PRN_RENC_SHELL_NG
                } else {
                    pRawEncResCfg->RecStrm[RecStrmIdx].StrmCfg.ChanCfg[ChanIdx].FovId          = FovID;
                    pRawEncResCfg->RecStrm[RecStrmIdx].StrmCfg.ChanCfg[ChanIdx].SrcWin.OffsetX = (UINT16) ( SrcOffsetX & 0x0000FFFFU );
                    pRawEncResCfg->RecStrm[RecStrmIdx].StrmCfg.ChanCfg[ChanIdx].SrcWin.OffsetY = (UINT16) ( SrcOffsetY & 0x0000FFFFU );
                    pRawEncResCfg->RecStrm[RecStrmIdx].StrmCfg.ChanCfg[ChanIdx].SrcWin.Width   = (UINT16) ( SrcWidth   & 0x0000FFFFU );
                    pRawEncResCfg->RecStrm[RecStrmIdx].StrmCfg.ChanCfg[ChanIdx].SrcWin.Height  = (UINT16) ( SrcHeight  & 0x0000FFFFU );
                    pRawEncResCfg->RecStrm[RecStrmIdx].StrmCfg.ChanCfg[ChanIdx].DstWin.OffsetX = (UINT16) ( DstOffsetX & 0x0000FFFFU );
                    pRawEncResCfg->RecStrm[RecStrmIdx].StrmCfg.ChanCfg[ChanIdx].DstWin.OffsetY = (UINT16) ( DstOffsetY & 0x0000FFFFU );
                    pRawEncResCfg->RecStrm[RecStrmIdx].StrmCfg.ChanCfg[ChanIdx].DstWin.Width   = (UINT16) ( DstWidth   & 0x0000FFFFU );
                    pRawEncResCfg->RecStrm[RecStrmIdx].StrmCfg.ChanCfg[ChanIdx].DstWin.Height  = (UINT16) ( DstHeight  & 0x0000FFFFU );
                    pRawEncResCfg->RecStrm[RecStrmIdx].StrmCfg.ChanCfg[ChanIdx].Margin.Top     = (UINT16) ( MarginTop    & 0x0000FFFFU );
                    pRawEncResCfg->RecStrm[RecStrmIdx].StrmCfg.ChanCfg[ChanIdx].Margin.Right   = (UINT16) ( MarginRight  & 0x0000FFFFU );
                    pRawEncResCfg->RecStrm[RecStrmIdx].StrmCfg.ChanCfg[ChanIdx].Margin.Bottom  = (UINT16) ( MarginBottom & 0x0000FFFFU );
                    pRawEncResCfg->RecStrm[RecStrmIdx].StrmCfg.ChanCfg[ChanIdx].Margin.Left    = (UINT16) ( MarginLeft   & 0x0000FFFFU );
                    pRawEncResCfg->RecStrm[RecStrmIdx].StrmCfg.ChanCfg[ChanIdx].RotateFlip     = (UINT8) RotateFlip;
                }
            }
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellResCfgRecCCfgU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                 RecIdx : record stream index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                ChanIdx : channel index" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                  FovID : fov id" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "             SrcOffsetX : configure source window" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "             SrcOffsetY : configure source window" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "               SrcWidth : configure source window" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "              SrcHeight : configure source window" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "             DstOffsetX : configure destination window" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "             DstOffsetY : configure destination window" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "               DstWidth : configure destination window" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "              DstHeight : configure destination window" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "              MarginTop : configure margin top" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "            MarginRight : configure margin right" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "           MarginBottom : configure margin bottom" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "             MarginLeft : configure margin left" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "             RotateFlip : configure rotation/flip" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellDmySenReset(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 1U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            SVC_RawEncCtrlFlag &= ~(SVC_RAW_ENC_FLG_DEF_DMY_SENSOR);
            SvcRawEnc_DefDmySenCfgInit();
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static UINT32 SvcRawEnc_ShellDmySenOutput(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 15U;
        SVC_RAW_ENC_DMY_SEN_CFG_s *pDmySenCfg;

        AmbaMisra_TypeCast(&(pDmySenCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pDmySenCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid dummy sensor config" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 DataRate     = 0U;
            UINT32 NumDataLanes = 0U;
            UINT32 NumDataBits  = 0U;
            UINT32 BayerPattern = 0U;
            UINT32 OutputWidth  = 0U;
            UINT32 OutputHeight = 0U;
            UINT32 RecWinStartX = 0U;
            UINT32 RecWinStartY = 0U;
            UINT32 RecWinWidth  = 0U;
            UINT32 RecWinHeight = 0U;
            UINT32 OBWinStartX  = 0U;
            UINT32 OBWinStartY  = 0U;
            UINT32 OBWinWidth   = 0U;
            UINT32 OBWinHeight  = 0U;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &DataRate    ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &NumDataLanes); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &NumDataBits ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &BayerPattern); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &OutputWidth ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &OutputHeight); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &RecWinStartX); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &RecWinStartY); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &RecWinWidth ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &RecWinHeight); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &OBWinStartX ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &OBWinStartY ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &OBWinWidth  ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &OBWinHeight );

            pDmySenCfg->OutputInfo.DataRate                  =           DataRate                    ;
            pDmySenCfg->OutputInfo.NumDataLanes              = (UINT8) ( NumDataLanes & 0x000000FFU );
            pDmySenCfg->OutputInfo.NumDataBits               = (UINT8) ( NumDataBits  & 0x000000FFU );
            pDmySenCfg->OutputInfo.BayerPattern              = (UINT8) ( BayerPattern & 0x000000FFU );
            pDmySenCfg->OutputInfo.OutputWidth               = (UINT16)( OutputWidth  & 0x0000FFFFU );
            pDmySenCfg->OutputInfo.OutputHeight              = (UINT16)( OutputHeight & 0x0000FFFFU );
            pDmySenCfg->OutputInfo.RecordingPixels.StartX    = (UINT16)( RecWinStartX & 0x0000FFFFU );
            pDmySenCfg->OutputInfo.RecordingPixels.StartY    = (UINT16)( RecWinStartY & 0x0000FFFFU );
            pDmySenCfg->OutputInfo.RecordingPixels.Width     = (UINT16)( RecWinWidth  & 0x0000FFFFU );
            pDmySenCfg->OutputInfo.RecordingPixels.Height    = (UINT16)( RecWinHeight & 0x0000FFFFU );
            pDmySenCfg->OutputInfo.OpticalBlackPixels.StartX = (UINT16)( OBWinStartX  & 0x0000FFFFU );
            pDmySenCfg->OutputInfo.OpticalBlackPixels.StartY = (UINT16)( OBWinStartY  & 0x0000FFFFU );
            pDmySenCfg->OutputInfo.OpticalBlackPixels.Width  = (UINT16)( OBWinWidth   & 0x0000FFFFU );
            pDmySenCfg->OutputInfo.OpticalBlackPixels.Height = (UINT16)( OBWinHeight  & 0x0000FFFFU );
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellDmySenOutputU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "               DataRate : output bit/pixel clock frequency from image sensor" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "           NumDataLanes : active data channels   " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "            NumDataBits : pixel data bit depth   " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "           BayerPattern : 0=RG, 1=BG, 2=GR, 3=GB " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "            OutputWidth : valid pixels per line  " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "           OutputHeight : valid lines per frame  " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "          RecWin StartX : maximum recording frame size StartX " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "          RecWin StartY : maximum recording frame size StartY " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "           RecWin Width : maximum recording frame size Width  " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "          RecWin Height : maximum recording frame size Height " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "          OB Win StartX : user clamp area StartX " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "          OB Win StartY : user clamp area StartY " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "           OB Win Width : user clamp area Width  " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "          OB Win Height : user clamp area Height " PRN_RENC_SHELL_USAGE

    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellDmySenInput(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 12U;
        SVC_RAW_ENC_DMY_SEN_CFG_s *pDmySenCfg;

        AmbaMisra_TypeCast(&(pDmySenCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pDmySenCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid dummy sensor config" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 WinStartX      = 0U;
            UINT32 WinStartY      = 0U;
            UINT32 WinWidth       = 0U;
            UINT32 WinHeight      = 0U;
            UINT32 HSubsampleType = 0U;
            UINT32 HSubsampleFNum = 0U;
            UINT32 HSubsampleFDen = 0U;
            UINT32 VSubsampleType = 0U;
            UINT32 VSubsampleFNum = 0U;
            UINT32 VSubsampleFDen = 0U;
            UINT32 SummingFactor  = 0U;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &WinStartX     ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &WinStartY     ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &WinWidth      ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &WinHeight     ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &HSubsampleType); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &HSubsampleFNum); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &HSubsampleFDen); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &VSubsampleType); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &VSubsampleFNum); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &VSubsampleFDen); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &SummingFactor );

            pDmySenCfg->InputInfo.PhotodiodeArray.StartX   = (UINT16)( WinStartX      & 0x0000FFFFU );
            pDmySenCfg->InputInfo.PhotodiodeArray.StartY   = (UINT16)( WinStartY      & 0x0000FFFFU );
            pDmySenCfg->InputInfo.PhotodiodeArray.Width    = (UINT16)( WinWidth       & 0x0000FFFFU );
            pDmySenCfg->InputInfo.PhotodiodeArray.Height   = (UINT16)( WinHeight      & 0x0000FFFFU );
            pDmySenCfg->InputInfo.HSubsample.SubsampleType =           HSubsampleType ;
            pDmySenCfg->InputInfo.HSubsample.FactorDen     = (UINT8) ( HSubsampleFNum & 0x000000FFU );
            pDmySenCfg->InputInfo.HSubsample.FactorNum     = (UINT8) ( HSubsampleFDen & 0x000000FFU );
            pDmySenCfg->InputInfo.VSubsample.SubsampleType =           VSubsampleType ;
            pDmySenCfg->InputInfo.VSubsample.FactorDen     = (UINT8) ( VSubsampleFNum & 0x000000FFU );
            pDmySenCfg->InputInfo.VSubsample.FactorNum     = (UINT8) ( VSubsampleFDen & 0x000000FFU );
            pDmySenCfg->InputInfo.SummingFactor            = (UINT8) ( SummingFactor  & 0x000000FFU );
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellDmySenInputU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "             Win StartX : photodiode array StartX " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "             Win StartY : photodiode array StartY " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "              Win Width : photodiode array Width  " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "             Win Height : photodiode array Height " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "         HSubsampleType : horizontal sub sample type " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "         HSubsampleFNum : horizontal sub sample factor numerator " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "         HSubsampleFDen : horizontal sub sample factor denominator " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "         VSubsampleType : vertical sub sample type " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "         VSubsampleFNum : vertical sub sample factor numerator " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "         VSubsampleFDen : vertical sub sample factor denominator " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "          SummingFactor : number of pixel data to be sumed up" PRN_RENC_SHELL_USAGE

    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellDmySenMInfo(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 7U;
        SVC_RAW_ENC_DMY_SEN_CFG_s *pDmySenCfg;

        AmbaMisra_TypeCast(&(pDmySenCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pDmySenCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid dummy sensor config" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 InputClk         = 0U;
            UINT32 LineLengthPck    = 0U;
            UINT32 FrameLengthLines = 0U;
            UINT32 Interlace        = 0U;
            UINT32 TimeScale        = 0U;
            UINT32 NumUnitsInTick   = 0U;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx],  &InputClk        ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx],  &LineLengthPck   ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx],  &FrameLengthLines); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx],  &Interlace       ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx],  &TimeScale       ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx],  &NumUnitsInTick  );

            pDmySenCfg->ModeInfo.FrameTiming.InputClk                 = InputClk        ;
            pDmySenCfg->ModeInfo.FrameTiming.LineLengthPck            = LineLengthPck   ;
            pDmySenCfg->ModeInfo.FrameTiming.FrameLengthLines         = FrameLengthLines;
            pDmySenCfg->ModeInfo.FrameTiming.FrameRate.Interlace      = (UINT8) ( Interlace & 0x000000FFU );
            pDmySenCfg->ModeInfo.FrameTiming.FrameRate.TimeScale      = TimeScale     ;
            pDmySenCfg->ModeInfo.FrameTiming.FrameRate.NumUnitsInTick = NumUnitsInTick;
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellDmySenMInfoU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "               InputClk : Sensor side input clock frequency " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "          LineLengthPck : XHS period (in input clock cycles) " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "       FrameLengthLines : horizontal operating period (in number of XHS pulses) " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "              Interlace : framerate, 1 - Interlace; 0 - Progressive" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "              TimeScale : framerate, time scale " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "         NumUnitsInTick : framerate, frames per Second " PRN_RENC_SHELL_USAGE

    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellDmySenHdrCh(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 3U;
        SVC_RAW_ENC_DMY_SEN_CFG_s *pDmySenCfg;

        AmbaMisra_TypeCast(&(pDmySenCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pDmySenCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid dummy sensor config" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 HdrType    = 0U;
            UINT32 ChannelNum = 0U;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &HdrType   ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &ChannelNum);

            pDmySenCfg->HdrInfo.HdrType        = HdrType;
            pDmySenCfg->HdrInfo.ActiveChannels = (UINT8) ( ChannelNum & 0x000000FFU );
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellDmySenHdrChU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                HdrType : hdr Type " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "             ChannelNum : hdr exposure channel number " PRN_RENC_SHELL_USAGE

    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellDmySenHdr(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 20U;
        SVC_RAW_ENC_DMY_SEN_CFG_s *pDmySenCfg;

        AmbaMisra_TypeCast(&(pDmySenCfg), &(pData));

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else if (pDmySenCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid dummy sensor config" PRN_RENC_SHELL_NG
        } else {
            UINT32 ArgIdx = pCurFunc->IndentLevel + 1U;
            UINT32 Channel          = 0U;
            UINT32 WinStartX        = 0U;
            UINT32 WinStartY        = 0U;
            UINT32 WinWidth         = 0U;
            UINT32 WinHeight        = 0U;
            UINT32 OBStartX         = 0U;
            UINT32 OBStartY         = 0U;
            UINT32 OBWidth          = 0U;
            UINT32 OBHeight         = 0U;
            UINT32 ShuCommTime      = 0U;
            UINT32 ShuFirstRefFrame = 0U;
            UINT32 ShuFirstBadFrame = 0U;
            UINT32 ShuNumBadFrames  = 0U;
            UINT32 OutCommTime      = 0U;
            UINT32 OutFirstRefFrame = 0U;
            UINT32 OutFirstBadFrame = 0U;
            UINT32 OutNumBadFrames  = 0U;
            UINT32 MaxExposureLine  = 0U;
            UINT32 MinExposureLine  = 0U;

            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &Channel          ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &WinStartX        ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &WinStartY        ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &WinWidth         ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &WinHeight        ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &OBStartX         ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &OBStartY         ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &OBWidth          ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &OBHeight         ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &ShuCommTime      ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &ShuFirstRefFrame ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &ShuFirstBadFrame ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &ShuNumBadFrames  ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &OutCommTime      ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &OutFirstRefFrame ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &OutFirstBadFrame ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &OutNumBadFrames  ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &MaxExposureLine  ); ArgIdx ++;
            SvcRawEnc_CmdStrToU32(pArgVector[ArgIdx], &MinExposureLine  );

            pDmySenCfg->HdrInfo.ChannelInfo[Channel].EffectiveArea.StartX                     = (UINT16)WinStartX;
            pDmySenCfg->HdrInfo.ChannelInfo[Channel].EffectiveArea.StartY                     = (UINT16)WinStartY;
            pDmySenCfg->HdrInfo.ChannelInfo[Channel].EffectiveArea.Width                      = (UINT16)WinWidth;
            pDmySenCfg->HdrInfo.ChannelInfo[Channel].EffectiveArea.Height                     = (UINT16)WinHeight;
            pDmySenCfg->HdrInfo.ChannelInfo[Channel].OpticalBlackPixels.StartX                = (UINT16)OBStartX;
            pDmySenCfg->HdrInfo.ChannelInfo[Channel].OpticalBlackPixels.StartY                = (UINT16)OBStartY;
            pDmySenCfg->HdrInfo.ChannelInfo[Channel].OpticalBlackPixels.Width                 = (UINT16)OBWidth;
            pDmySenCfg->HdrInfo.ChannelInfo[Channel].OpticalBlackPixels.Height                = (UINT16)OBHeight;
            pDmySenCfg->HdrInfo.ChannelInfo[Channel].ShutterSpeedCtrlInfo.CommunicationTime   = ShuCommTime;
            pDmySenCfg->HdrInfo.ChannelInfo[Channel].ShutterSpeedCtrlInfo.FirstReflectedFrame = (UINT8)ShuFirstRefFrame;
            pDmySenCfg->HdrInfo.ChannelInfo[Channel].ShutterSpeedCtrlInfo.FirstBadFrame       = (UINT8)ShuFirstBadFrame;
            pDmySenCfg->HdrInfo.ChannelInfo[Channel].ShutterSpeedCtrlInfo.NumBadFrames        = (UINT8)ShuNumBadFrames;
            pDmySenCfg->HdrInfo.ChannelInfo[Channel].OutputFormatCtrlInfo.CommunicationTime   = OutCommTime;
            pDmySenCfg->HdrInfo.ChannelInfo[Channel].OutputFormatCtrlInfo.FirstReflectedFrame = (UINT8)OutFirstRefFrame;
            pDmySenCfg->HdrInfo.ChannelInfo[Channel].OutputFormatCtrlInfo.FirstBadFrame       = (UINT8)OutFirstBadFrame;
            pDmySenCfg->HdrInfo.ChannelInfo[Channel].OutputFormatCtrlInfo.NumBadFrames        = (UINT8)OutNumBadFrames;
            pDmySenCfg->HdrInfo.ChannelInfo[Channel].MaxExposureLine                          = MaxExposureLine;
            pDmySenCfg->HdrInfo.ChannelInfo[Channel].MinExposureLine                          = MinExposureLine;
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static void SvcRawEnc_ShellDmySenHdrU(UINT32 CtrlFlag, void *pFunc)
{
    AmbaMisra_TouchUnused(&CtrlFlag); AmbaMisra_TouchUnused(pFunc);

    PRN_RENC_LOG "      ------------------------------------------------------" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "              WinStartX : effective area start x" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "              WinStartY : effective area start y" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "               WinWidth : effective area width  " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "              WinHeight : effective area height " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "               OBStartX : ob area start x" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "               OBStartY : ob area start y" PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "                OBWidth : ob area width  " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "               OBHeight : ob area height " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "            ShuCommTime : " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "       ShuFirstRefFrame : " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "       ShuFirstBadFrame : " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "        ShuNumBadFrames : " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "            OutCommTime : " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "       OutFirstRefFrame : " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "       OutFirstBadFrame : " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "        OutNumBadFrames : " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "        MaxExposureLine : maximum exposure line (in row time) " PRN_RENC_SHELL_USAGE
    PRN_RENC_LOG "        MinExposureLine : minimum exposure line. (in row time)" PRN_RENC_SHELL_USAGE

    PRN_RENC_LOG " " PRN_RENC_SHELL_USAGE
}

static UINT32 SvcRawEnc_ShellDmySenInfo(UINT32 ArgCount, char * const *pArgVector, void *pFunc, void *pData)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RAW_ENC_SHELL_FUNC_s *pCurFunc;

    AmbaMisra_TypeCast(&pCurFunc, &pFunc);

    if (pCurFunc == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "invalid shell function entry" PRN_RENC_SHELL_NG
    } else {
        UINT32 ReqArgCount = pCurFunc->IndentLevel + 1U;

        AmbaUtility_StringAppend(SvcRawEncShellStr, (UINT32)sizeof(SvcRawEncShellStr), pCurFunc->CmdName);

        if (ArgCount < ReqArgCount) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg count! %d >= %d"
                PRN_RENC_ARG_UINT32 ArgCount    PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 ReqArgCount PRN_RENC_ARG_POST
            PRN_RENC_SHELL_NG
        } else if (pArgVector == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "invalid arg vector" PRN_RENC_SHELL_NG
        } else {
            SvcRawEnc_DmySenCfgPrint();
        }
    }

    AmbaMisra_TouchUnused(pFunc); AmbaMisra_TouchUnused(pData);

    return RetVal;
}

static UINT32 SvcRawEnc_DmySenInit(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    AmbaMisra_TouchUnused(&pChan);

    PRN_RENC_LOG "Initial dummy sensor for video raw enc" PRN_RENC_API

    return SVC_OK;
}

static UINT32 SvcRawEnc_DmySenEnable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    AmbaMisra_TouchUnused(&pChan);

    PRN_RENC_LOG "Enable dummy sensor for video raw enc" PRN_RENC_API

    return SVC_OK;
}

static UINT32 SvcRawEnc_DmySenDisable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    AmbaMisra_TouchUnused(&pChan);
    return SVC_OK;
}

static UINT32 SvcRawEnc_DmySenConfig(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    AmbaMisra_TouchUnused(&pChan);
    AmbaMisra_TouchUnused(&pMode);

    PRN_RENC_LOG "Configure dummy sensor for video raw enc" PRN_RENC_API

    return SVC_OK;
}

static UINT32 SvcRawEnc_DmySenGetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 PRetVal;
    AmbaMisra_TouchUnused(&pChan);

    if (pStatus != NULL) {
        AMBA_SENSOR_CONFIG_s SsMode = {0};
        PRetVal = SvcRawEnc_DmySenGetModeInfo(pChan, &SsMode, &(pStatus->ModeInfo)); PRN_RENC_ERR_HDLR
    }

    return SVC_OK;
}

static UINT32 SvcRawEnc_DmySenGetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else if (pMode == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else if (pModeInfo == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        AMBA_SENSOR_INPUT_INFO_s   *pInputInfo    = &RawEncDmySenCfg.InputInfo;
        AMBA_SENSOR_OUTPUT_INFO_s  *pOutputInfo   = &RawEncDmySenCfg.OutputInfo;
        SVC_RAW_ENC_DMY_SEN_INFO_s *pModeListInfo = &RawEncDmySenCfg.ModeInfo;
        AMBA_SENSOR_HDR_INFO_s     *pHdrInfo      = &RawEncDmySenCfg.HdrInfo;

        AmbaSvcWrap_MisraMemset(pModeInfo, 0, sizeof(AMBA_SENSOR_MODE_INFO_s));
        pModeInfo->LineLengthPck = pModeListInfo->FrameTiming.LineLengthPck;
        pModeInfo->FrameLengthLines = pModeListInfo->FrameTiming.FrameLengthLines;

        if (pHdrInfo->HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            pModeInfo->NumExposureStepPerFrame  = pModeInfo->FrameLengthLines / 2U;
        } else {
            pModeInfo->NumExposureStepPerFrame  = pModeInfo->FrameLengthLines;
        }

        pModeInfo->InputClk      = pModeListInfo->FrameTiming.InputClk;
        pModeInfo->RowTime       = pModeListInfo->RowTime;
        pModeInfo->Config.ModeID = 0U;
        AmbaSvcWrap_MisraMemcpy(&pModeInfo->Config,       pMode, sizeof(AMBA_SENSOR_CONFIG_s));
        AmbaSvcWrap_MisraMemcpy(&pModeInfo->FrameRate,    &(pModeListInfo->FrameTiming.FrameRate), sizeof(AMBA_VIN_FRAME_RATE_s));
        AmbaSvcWrap_MisraMemcpy(&pModeInfo->InputInfo,    pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
        AmbaSvcWrap_MisraMemcpy(&pModeInfo->OutputInfo,   pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
        AmbaSvcWrap_MisraMemcpy(&pModeInfo->MinFrameRate, &(pModeListInfo->FrameTiming.FrameRate), sizeof(AMBA_VIN_FRAME_RATE_s));
        AmbaSvcWrap_MisraMemcpy(&pModeInfo->HdrInfo,      pHdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s));

        /* updated minimum frame rate limitation */
        if (pModeInfo->HdrInfo.HdrType == 0U) {
            pModeInfo->MinFrameRate.TimeScale /= 8U;
        }

        AmbaMisra_TouchUnused(pInputInfo   );
        AmbaMisra_TouchUnused(pOutputInfo  );
        AmbaMisra_TouchUnused(pModeListInfo);
        AmbaMisra_TouchUnused(pHdrInfo     );
        AmbaMisra_TouchUnused(&pChan);
        AmbaMisra_TouchUnused(&pMode);
        AmbaMisra_TouchUnused(pModeInfo);
    }

    return RetVal;
}

static UINT32 SvcRawEnc_DmySenGetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    AmbaMisra_TouchUnused(&pChan);
    AmbaMisra_TouchUnused(pDeviceInfo);
    return SVC_OK;
}

static UINT32 SvcRawEnc_DmySenGetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 PRetVal;
    AmbaMisra_TouchUnused(&pShutterCtrl);

    if (pHdrInfo != NULL) {
        AMBA_SENSOR_CONFIG_s SsMode = {0};
        AMBA_SENSOR_MODE_INFO_s SsModeInfo = {0};
        PRetVal = SvcRawEnc_DmySenGetModeInfo(pChan, &SsMode, &(SsModeInfo)); PRN_RENC_ERR_HDLR
        AmbaSvcWrap_MisraMemcpy(pHdrInfo, &(SsModeInfo.HdrInfo), sizeof(AMBA_SENSOR_HDR_INFO_s));
    }

    return SVC_OK;
}

static UINT32 SvcRawEnc_DmySenGetGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    AmbaMisra_TouchUnused(&pChan);
    AmbaMisra_TouchUnused(pGainFactor);
    return SVC_OK;
}

static UINT32 SvcRawEnc_DmySenGetShutterSpd(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    AmbaMisra_TouchUnused(&pChan);
    AmbaMisra_TouchUnused(pExposureTime);
    return SVC_OK;
}

static UINT32 SvcRawEnc_DmySenCvtGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    AmbaMisra_TouchUnused(&pChan);
    AmbaMisra_TouchUnused(&pDesiredFactor);
    AmbaMisra_TouchUnused(pActualFactor);
    AmbaMisra_TouchUnused(pGainCtrl);
    return SVC_OK;
}

static UINT32 SvcRawEnc_DmySenCvtShutterSpd(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    AmbaMisra_TouchUnused(&pChan);
    AmbaMisra_TouchUnused(&pDesiredExposureTime);
    AmbaMisra_TouchUnused(pActualExposureTime);
    AmbaMisra_TouchUnused(pShutterCtrl);
    return SVC_OK;
}

static UINT32 SvcRawEnc_DmySenSetAGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    AmbaMisra_TouchUnused(&pChan);
    AmbaMisra_TouchUnused(&pAnalogGainCtrl);
    return SVC_OK;
}

static UINT32 SvcRawEnc_DmySenSetDGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pDigitalGainCtrl)
{
    AmbaMisra_TouchUnused(&pChan);
    AmbaMisra_TouchUnused(&pDigitalGainCtrl);
    return SVC_OK;
}

static UINT32 SvcRawEnc_DmySenSetWbGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl)
{
    AmbaMisra_TouchUnused(&pChan);
    AmbaMisra_TouchUnused(&pWbGainCtrl);
    return SVC_OK;
}

static UINT32 SvcRawEnc_DmySenSetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    AmbaMisra_TouchUnused(&pChan);
    AmbaMisra_TouchUnused(&pShutterCtrl);
    return SVC_OK;
}

static UINT32 SvcRawEnc_DmySenSetSlowShuCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    AmbaMisra_TouchUnused(&pChan);
    AmbaMisra_TouchUnused(&SlowShutterCtrl);
    return SVC_OK;
}

static UINT32 SvcRawEnc_DmySenRegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16* pData)
{
    AmbaMisra_TouchUnused(&pChan);
    AmbaMisra_TouchUnused(&Addr);
    AmbaMisra_TouchUnused(pData);
    return SVC_OK;
}

static UINT32 SvcRawEnc_DmySenRegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
{
    AmbaMisra_TouchUnused(&pChan);
    AmbaMisra_TouchUnused(&Addr);
    AmbaMisra_TouchUnused(&Data);
    return SVC_OK;
}

/**
* memory size query of raw encode module
* @param [in] pCfg configuration of record
* @param [out] pMemSize size of needed memory
* @return 0-OK, 1-NG
*/
UINT32 SvcRawEnc_MemQry(const SVC_RES_CFG_s *pCfg, UINT32 *pMemSize)
{
    UINT32 RetVal = SVC_OK;

    if (pCfg == NULL) {
        RetVal = SVC_NG;
        if ((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_MSG_DBG) > 0U) {
            PRN_RENC_LOG "Fail to query memory - input configuration should not null!" PRN_RENC_NG
        }
    } else {
        UINT32 RawMemSize = 0U, HdsMemSize = 0U, TotalMemSize;

        RetVal = SvcRawEnc_MemoryQuery(pCfg, &RawMemSize, &HdsMemSize);
        if (RetVal == SVC_OK) {
            TotalMemSize = RawMemSize + HdsMemSize;
            PRN_RENC_LOG "Query the raw enc memory size 0x%x = ( 0x%x + 0x%x ) x %d"
                PRN_RENC_ARG_UINT32 TotalMemSize            PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 RawMemSize              PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 HdsMemSize              PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 SVC_RAW_ENC_MAX_BUF_NUM PRN_RENC_ARG_POST
            PRN_RENC_DBG

            if (pMemSize != NULL) {
                *pMemSize = TotalMemSize;
            }
        }
    }

    return RetVal;
}

/**
* create of raw encode module
* @param [in] pCfg configuration of record
* @param [in] MemAddr base of memory buffer
* @param [in] MemSize size of memory buffer
* @param [in] MemSize base of ituner memory buffer
* @param [in] ItnMemSize size of ituner memory buffer
* @return 0-OK, 1-NG
*/
UINT32 SvcRawEnc_Create(const SVC_RES_CFG_s *pCfg)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_INIT) > 0U) {
        PRN_RENC_LOG "The raw encode module has been initialized!" PRN_RENC_OK
    } else if (pCfg == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to create raw encode - input cfg should not null!" PRN_RENC_NG
    } else {
        UINT32 VinIDs[AMBA_DSP_MAX_VIN_NUM], VinNum = 0U;

        AmbaSvcWrap_MisraMemset(VinIDs, 0, sizeof(VinIDs));

        if (SVC_OK != SvcResCfg_Config(pCfg)) { /* Configure raw encde res_cfg */
            RetVal = SVC_NG;
            PRN_RENC_LOG "Fail to create raw encode - configure the raw encode res cfg fail!" PRN_RENC_NG
        } else if (SVC_OK != SvcResCfg_GetVinIDs(VinIDs, &VinNum)) { /* Get the vin info */
            RetVal = SVC_NG;
            PRN_RENC_LOG "Fail to create raw encode - get vin info fail!" PRN_RENC_NG
        } else if (VinNum != 1U) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "Fail to create raw encode - invalid vin num(%d)!"
                PRN_RENC_ARG_UINT32 VinNum PRN_RENC_ARG_POST
            PRN_RENC_NG
        } else {

            /* Reset the raw enc control setting */
            AmbaSvcWrap_MisraMemset(&SVC_RawEncCtrl, 0, sizeof(SVC_RawEncCtrl));
            SvcWrap_strcpy(SVC_RawEncCtrl.Name, sizeof(SVC_RawEncCtrl.Name), "SvcRawEnc");

            /* Get liveview info */
            AmbaSvcWrap_MisraMemset(&(SVC_RawEncCtrl.LivInfo), 0, sizeof((SVC_RawEncCtrl.LivInfo)));
            SvcLiveview_InfoGet(&(SVC_RawEncCtrl.LivInfo));

            if (0U != AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_RAW_RDY, SvcRawEnc_DspEvtRawDataRdy)) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "Fail to create raw encode - register raw data event fail!" PRN_RENC_NG
            } else if (0U != AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_YUV_DATA_RDY, SvcRawEnc_DspEvtLivYuvRdy)) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "Fail to create raw encode - register liveview yuv event fail!" PRN_RENC_NG
            } else if (0U != AmbaKAL_MutexCreate(&(SVC_RawEncCtrl.Mutex), SVC_RawEncCtrl.Name)) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "Fail to create raw encode - create mutex fail!" PRN_RENC_NG
            } else if (0U != AmbaKAL_EventFlagCreate(&(SVC_RawEncCtrl.EventFlag), SVC_RawEncCtrl.Name)) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "Fail to create raw encode - create event flag fail!" PRN_RENC_NG
            } else {
                typedef UINT32 (*IMGPROC_RESET_TABLE)(void);
                extern UINT32 App_Image_Reset_Table(void) GNU_WEAK_SYMBOL;
                IMGPROC_RESET_TABLE pImgprocResetTbl = App_Image_Reset_Table;

                PRetVal = AmbaKAL_EventFlagClear(&(SVC_RawEncCtrl.EventFlag), 0xFFFFFFFFU);
                if (PRetVal != 0U) {
                    PRN_RENC_LOG "Fail to create raw encode - clear event flag fail!" PRN_RENC_NG
                }

                SvcRawEnc_DefDmySenCfgInit();

                PRN_RENC_LOG "Successful to create raw encode!" PRN_RENC_OK

                SVC_RawEncCtrlFlag &= ~(SVC_RAW_ENC_FLG_DEF_IDSP_CFG);
                SVC_RawEncCtrlFlag |= SVC_RAW_ENC_FLG_INIT;

                if (pImgprocResetTbl != NULL) {
                    if (0U == pImgprocResetTbl()) {
                        PRN_RENC_LOG "Successful to reset table map!" PRN_RENC_OK
                    } else {
                        PRN_RENC_LOG "Fail to reset table map!" PRN_RENC_NG
                    }
                }
            }
        }
    }

    return RetVal;
}

/**
* configure raw encode memory
* @param [in] pMemBuf pointer of memory buffer
* @param [in] MemSize size of memory buffer
* @return 0-OK, 1-NG
*/
UINT32 SvcRawEnc_MemCfg(UINT8 *pMemBuf, UINT32 MemSize)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    UINT8 *pCurMemBuf = pMemBuf;
    UINT32 CurMemSize = MemSize;

    if ((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to cfg raw enc memory - create video raw enc module first!" PRN_RENC_NG
    } else {
        const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
        UINT32 RawBufSize = 0U, HdsBufSize = 0U;

        if (pCfg == NULL) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "Fail to cfg raw enc memory - invalid res_cfg!" PRN_RENC_NG
        } else if (SVC_OK != SvcRawEnc_MemoryQuery(pCfg, &RawBufSize, &HdsBufSize)) { /* Query raw encode memory */
            RetVal = SVC_NG;
            PRN_RENC_LOG "Fail to cfg raw enc memory - get the raw/hds buffer fail!" PRN_RENC_NG
        } else {

            PRN_RENC_LOG "---- Configure Video Raw Enc Memory ----" PRN_RENC_API
            PRN_RENC_LOG "  RawBufSize: %d, HdsBufSize: %d"
                PRN_RENC_ARG_UINT32 RawBufSize PRN_RENC_ARG_POST
                PRN_RENC_ARG_UINT32 HdsBufSize PRN_RENC_ARG_POST
            PRN_RENC_API

            if (pCurMemBuf == NULL) {
                ULONG  FreeBase = 0U;
                UINT32 FreeSize = 0U;
                UINT8 *pFreeBuf;

#ifndef CONFIG_BUILD_QNX_IPL
                if (0U != SvcBuffer_LockFreeSpace(SVC_MEM_TYPE_CA, &FreeBase, &FreeSize)) {
                    FreeBase = 0U; FreeSize = 0U;
                    PRetVal = SvcBuffer_UnLockFreeSpace(SVC_MEM_TYPE_CA); PRN_RENC_ERR_HDLR
                } else {
                    AmbaMisra_TypeCast(&(pFreeBuf), &(FreeBase));

                    PRN_RENC_LOG "Get free Cache Buffer: %p / %d"
                        PRN_RENC_ARG_CPOINT pFreeBuf PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 FreeSize PRN_RENC_ARG_POST
                    PRN_RENC_API

                    if (FreeSize >= (RawBufSize + HdsBufSize)) {
                        pCurMemBuf = pFreeBuf;
                        CurMemSize = FreeSize;
                        PRN_RENC_LOG "-> Configure Free Cache Buffer for video raw enc" PRN_RENC_API
                    } else {
                        PRetVal = SvcBuffer_UnLockFreeSpace(SVC_MEM_TYPE_CA); PRN_RENC_ERR_HDLR
                    }
                }

                if (pCurMemBuf == NULL) {
                    FreeBase = 0U; FreeSize = 0U;
                    if (0U != SvcBuffer_LockFreeSpace(SVC_MEM_TYPE_NC, &FreeBase, &FreeSize)) {
                        PRetVal = SvcBuffer_UnLockFreeSpace(SVC_MEM_TYPE_NC); PRN_RENC_ERR_HDLR
                    } else {
                        AmbaMisra_TypeCast(&(pFreeBuf), &(FreeBase));

                        PRN_RENC_LOG "Get free Non-Cache Buffer: %p / %d"
                            PRN_RENC_ARG_CPOINT pFreeBuf PRN_RENC_ARG_POST
                            PRN_RENC_ARG_UINT32 FreeSize PRN_RENC_ARG_POST
                        PRN_RENC_API

                        if (FreeSize >= (RawBufSize + HdsBufSize)) {
                            pCurMemBuf = pFreeBuf;
                            CurMemSize = FreeSize;
                            PRN_RENC_LOG "-> Configure Free Non-Cache Buffer for video raw enc" PRN_RENC_API
                        } else {
                            PRetVal = SvcBuffer_UnLockFreeSpace(SVC_MEM_TYPE_NC); PRN_RENC_ERR_HDLR
                        }
                    }
                }
#else
                if (0U != SvcBuffer_LockFreeSpace(SVC_MEM_TYPE_DSP, &FreeBase, &FreeSize)) {
                    FreeBase = 0U; FreeSize = 0U;
                    PRetVal = SvcBuffer_UnLockFreeSpace(SVC_MEM_TYPE_DSP); PRN_RENC_ERR_HDLR
                } else {
                    AmbaMisra_TypeCast(&(pFreeBuf), &(FreeBase));

                    PRN_RENC_LOG "Get free DSP Buffer: %p / %d"
                    PRN_RENC_ARG_CPOINT pFreeBuf PRN_RENC_ARG_POST
                        PRN_RENC_ARG_UINT32 FreeSize PRN_RENC_ARG_POST
                    PRN_RENC_API

                    if (FreeSize >= (RawBufSize + HdsBufSize)) {
                        pCurMemBuf = pFreeBuf;
                        CurMemSize = FreeSize;
                        PRN_RENC_LOG "-> Configure Free DSP Buffer for video raw enc" PRN_RENC_API
                    } else {
                        PRetVal = SvcBuffer_UnLockFreeSpace(SVC_MEM_TYPE_DSP); PRN_RENC_ERR_HDLR
                    }
                }
#endif
            }

            if (pCurMemBuf == NULL) {
                RetVal = SVC_NG;
                PRN_RENC_LOG "Fail to cfg raw enc memory - invalid raw buffer!" PRN_RENC_NG
            } else {
                SVC_RawEncCtrl.pMemBuf = pCurMemBuf;
                SVC_RawEncCtrl.MemSize = CurMemSize;

                RetVal = SvcRawEnc_MemoryConfig(pCurMemBuf, CurMemSize);
                if (RetVal != SVC_OK) {
                    PRN_RENC_LOG "Fail to cfg raw enc memory - configure raw encode memory fail!" PRN_RENC_NG
                } else {
                    PRN_RENC_LOG "Success to cfg raw enc memory" PRN_RENC_OK
                }
            }
        }
    }

    return RetVal;
}

/**
* configure raw encode ituner memory
* @param [in] pMemBuf pointer of memory buffer
* @param [in] MemSize size of memory buffer
* @return 0-OK, 1-NG
*/
UINT32 SvcRawEnc_ItnMemCfg(UINT8 *pMemBuf, UINT32 MemSize)
{
    if ((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_INIT) > 0U) {
        SVC_RawEncCtrl.pItnMemBuf = pMemBuf;
        SVC_RawEncCtrl.ItnMemSize = MemSize;

        PRN_RENC_LOG "Success to cfg raw enc ituner memory as %p 0x%x"
            PRN_RENC_ARG_CPOINT SVC_RawEncCtrl.pItnMemBuf PRN_RENC_ARG_POST
            PRN_RENC_ARG_UINT32 SVC_RawEncCtrl.ItnMemSize PRN_RENC_ARG_POST
        PRN_RENC_OK
    }

    return SVC_OK;
}

/**
* delete of raw encode module
* @return 0-OK, 1-NG
*/
UINT32 SvcRawEnc_Delete(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_INIT) > 0U) {
        if (0U != SvcRawEnc_MutexTake(&(SVC_RawEncCtrl.Mutex), SVC_RAW_ENC_MAX_MUTX_TIMEOUT)) {
            RetVal = SVC_NG;
            PRN_RENC_LOG "Fail to delete raw enc - take mutex timeout!" PRN_RENC_NG
        } else {
            UINT32 Idx;

            /* Delete buffer control */
            for (Idx = 0U; Idx < AMBA_DSP_MAX_VIN_NUM; Idx ++) {
                if (0U != SvcRawEnc_BufCtrlDelete(&(SVC_RawEncCtrl.BufCtrl[Idx]))) {
                    PRN_RENC_LOG "Fail to delete raw enc - delete VinID(%d) buffer ctrl fail!"
                        PRN_RENC_ARG_UINT32 Idx PRN_RENC_ARG_POST
                    PRN_RENC_NG
                }

            }

            /* Delete mutex */
            SvcRawEnc_MutexGive(&(SVC_RawEncCtrl.Mutex));
            PRetVal = AmbaKAL_MutexDelete(&(SVC_RawEncCtrl.Mutex));
            if (PRetVal != 0U) {
                PRN_RENC_LOG "Fail to delete raw enc - delete mutex fail! ErrCode(0x%08x)"
                    PRN_RENC_ARG_UINT32 PRetVal PRN_RENC_ARG_POST
                PRN_RENC_NG
            }

            /* Reset setting */
            AmbaSvcWrap_MisraMemset(&SVC_RawEncCtrl, 0, sizeof(SVC_RawEncCtrl));

            SVC_RawEncCtrlFlag &= ~(SVC_RAW_ENC_FLG_DEF_IDSP_CFG);
            SVC_RawEncCtrlFlag &= ~(SVC_RAW_ENC_FLG_INIT);
        }
    }

    return RetVal;
}

/**
* default idsp configuration of raw encode module
* @param [in] NumIK number of IK
* @param [in] pViewZoneIDs id array of fov
* @param [in] pIKModeArr mode array of IK
* @return 0-OK, 1-NG
*/
UINT32 SvcRawEnc_DefIdspCfg(UINT32 NumIK, UINT32 *pViewZoneIDs, AMBA_IK_MODE_CFG_s *pIKModeArr)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to cfg default idsp - initial raw encode first!" PRN_RENC_NG
    } else if (NumIK == 0U) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to cfg default idsp - input ik number should not zero!" PRN_RENC_NG
    } else if (pViewZoneIDs == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to cfg default idsp - input view zone id array should not null!" PRN_RENC_NG
        AmbaMisra_TouchUnused(pViewZoneIDs);
    } else if (pIKModeArr == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to cfg default idsp - input ik mode cfg array should not null!" PRN_RENC_NG
        AmbaMisra_TouchUnused(pIKModeArr);
    } else if (0U != SvcRawEnc_MutexTake(&(SVC_RawEncCtrl.Mutex), SVC_RAW_ENC_MAX_MUTX_TIMEOUT)) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to cfg default idsp - take mutex timeout!" PRN_RENC_NG
    } else {
        UINT32 Idx;
        SVC_RAW_ENC_VIEW_ZONE_CFG_s *pCurViewZoneCfg;

        for (Idx = 0U; Idx < NumIK; Idx ++) {
            SVC_RawEncCtrl.ViewZoneSelectBits |= SvcRawEnc_BitGet(pViewZoneIDs[Idx]);

            pCurViewZoneCfg = &(SVC_RawEncCtrl.ViewZoneCfg[pViewZoneIDs[Idx]]);
            AmbaSvcWrap_MisraMemcpy(&(pCurViewZoneCfg->IKMode), &(pIKModeArr[Idx]), sizeof(AMBA_IK_MODE_CFG_s));
        }

        if (SVC_OK != SvcRawEnc_ExecuteItnData()) {
            PRN_RENC_LOG "Fail to cfg default idsp - execute idsp fail!" PRN_RENC_NG
        } else {
            SVC_RawEncCtrlFlag |= SVC_RAW_ENC_FLG_DEF_IDSP_CFG;
        }

        SvcRawEnc_MutexGive(&(SVC_RawEncCtrl.Mutex));
    }

    return RetVal;
}

/**
* execution of raw encode module
* @return 0-OK, 1-NG
*/
UINT32 SvcRawEnc_Execute(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    const SVC_RES_CFG_s *pResCfg = SvcRawEnc_ResCfgGet();

    if ((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to execute raw enc - create raw encode module first!" PRN_RENC_NG
    } else if ((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_DEF_IDSP_CFG) == 0U) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to execute raw enc - configure default idsp cfg first!" PRN_RENC_NG
    } else if (pResCfg == NULL) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to execute raw enc - get resolution cfg fail!" PRN_RENC_NG
    } else if (0U != SvcRawEnc_MutexTake(&(SVC_RawEncCtrl.Mutex), SVC_RAW_ENC_MAX_MUTX_TIMEOUT)) {
        RetVal = SVC_NG;
        PRN_RENC_LOG "Fail to execute raw enc - take mutex timeout!" PRN_RENC_NG
    } else {
        UINT32 VinIdx, Pitch, Width, Height;
        SVC_RAW_ENC_BUF_INFO_s BufInfo[AMBA_DSP_MAX_VIN_NUM];
        UINT32 NumVin = 0U;
        UINT16 ViewZoneIDArr[AMBA_DSP_MAX_VIN_NUM];
        AMBA_DSP_LIVEVIEW_EXT_RAW_s ExtBufs[AMBA_DSP_MAX_VIN_NUM];

        UINT32 ViewZoneNum;
        UINT32 ViewZoneIDs[AMBA_DSP_MAX_VIEWZONE_NUM];
        SVC_RAW_ENC_BUF_INFO_s      *pCurBufInfo;
        SVC_RAW_ENC_VIN_CFG_s       *pCurVinCfg;
        AMBA_DSP_LIVEVIEW_EXT_RAW_s *pCurExtBuf;

        AmbaSvcWrap_MisraMemset(BufInfo, 0, sizeof(BufInfo));
        AmbaSvcWrap_MisraMemset(ViewZoneIDArr, 0, sizeof(ViewZoneIDArr));
        AmbaSvcWrap_MisraMemset(ExtBufs, 0, sizeof(ExtBufs));

        for (VinIdx = 0U; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx ++) {
            if ((SVC_RawEncCtrl.VinSelectBits & SvcRawEnc_BitGet(VinIdx)) > 0U) {

                /* Get the ViewZone info by VinID */
                ViewZoneNum = 0U;
                AmbaSvcWrap_MisraMemset(ViewZoneIDs, 0, sizeof(ViewZoneIDs));
                if (0U != SvcResCfg_GetFovIdxsInVinID(VinIdx, ViewZoneIDs, &ViewZoneNum)) {
                    PRN_RENC_LOG "Fail to execute raw enc - get VinID(%d) fov info fail!"
                        PRN_RENC_ARG_UINT32 VinIdx PRN_RENC_ARG_POST
                    PRN_RENC_NG
                }

                /* Configure the feed raw data */

                pCurBufInfo = &(BufInfo[VinIdx]);
                pCurVinCfg  = &(SVC_RawEncCtrl.VinCfg[VinIdx]);
                pCurExtBuf  = &(ExtBufs[NumVin]);

                Width  = (UINT32)(pResCfg->VinCfg[VinIdx].CapWin.Width);  Width  &= 0x0000FFFFU;
                Height = (UINT32)(pResCfg->VinCfg[VinIdx].CapWin.Height); Height &= 0x0000FFFFU;

                /* Get the free buffer */
                AmbaSvcWrap_MisraMemset(pCurBufInfo, 0, sizeof(SVC_RAW_ENC_BUF_INFO_s));
                RetVal = SvcRawEnc_BufCtrlGetBuf(&(SVC_RawEncCtrl.BufCtrl[VinIdx]), pCurBufInfo);
                if (RetVal != SVC_OK) {
                    PRN_RENC_LOG "Fail to execute raw encode - get the VinID(%d) free buffer fail!"
                        PRN_RENC_ARG_UINT32 VinIdx PRN_RENC_ARG_POST
                    PRN_RENC_NG
                } else {
                    Pitch = SvcRawEnc_Align(SVC_RAW_ENC_RAW_PITCH_ALIGN, Width << 1U);

                    ViewZoneIDArr[NumVin]               = (UINT16) ViewZoneIDs[0];

                    pCurExtBuf->VinId                   = (UINT16) VinIdx;
                    pCurExtBuf->SubChan.IsVirtual       = 0;
                    pCurExtBuf->SubChan.Index           = (UINT16) VinIdx;
                    pCurExtBuf->RawBuffer.Compressed    = 0;
                    pCurExtBuf->RawBuffer.Window.Width  = (UINT16) Width;
                    pCurExtBuf->RawBuffer.Window.Height = (UINT16) Height;
                    pCurExtBuf->RawBuffer.Pitch         = (UINT16) Pitch;

                    if (pCurBufInfo->RawBufSize < (Pitch * Height)) {
                        RetVal = SVC_NG;

                        PRetVal = Pitch * Height;
                        PRN_RENC_LOG "Fail to execute raw enc - VinID(%d) buf size(0x%x) cannot service the raw size(0x%x)!"
                            PRN_RENC_ARG_UINT32 VinIdx                  PRN_RENC_ARG_POST
                            PRN_RENC_ARG_UINT32 pCurBufInfo->RawBufSize PRN_RENC_ARG_POST
                            PRN_RENC_ARG_UINT32 PRetVal                 PRN_RENC_ARG_POST
                        PRN_RENC_NG
                    } else {
                        AmbaMisra_TypeCast(&(pCurExtBuf->RawBuffer.BaseAddr), &(pCurBufInfo->pRawBuf));
                    }

                    if (pCurBufInfo->HdsBufSize > 0U) {
                        Pitch = SvcRawEnc_Align(SVC_RAW_ENC_RAW_PITCH_ALIGN, Width >> 1U);  // Hdr Pitch = align64( ( Raw Width / 4 ) x 2 )

                        pCurExtBuf->CeBuffer.Compressed    = 0;
                        pCurExtBuf->CeBuffer.Window.Width  = (UINT16) ( Width );
                        pCurExtBuf->CeBuffer.Window.Height = (UINT16) ( Height );
                        pCurExtBuf->CeBuffer.Pitch         = (UINT16) ( Pitch );

                        if (pCurBufInfo->HdsBufSize < (Pitch * Height)) {
                            RetVal = SVC_NG;

                            PRetVal = Pitch * Height;
                            PRN_RENC_LOG "Fail to execute raw enc - VinID(%d) buf size(0x%x) cannot service the ce size(0x%x)!"
                                PRN_RENC_ARG_UINT32 VinIdx                  PRN_RENC_ARG_POST
                                PRN_RENC_ARG_UINT32 pCurBufInfo->HdsBufSize PRN_RENC_ARG_POST
                                PRN_RENC_ARG_UINT32 PRetVal                 PRN_RENC_ARG_POST
                            PRN_RENC_NG
                        } else {
                            AmbaMisra_TypeCast(&(pCurExtBuf->CeBuffer.BaseAddr), &(pCurBufInfo->pHdsBuf));
                        }
                    }

                    NumVin += 1U;
                }

                /* Load the aaa data */
                if (RetVal == SVC_OK) {

                    /* Process callback */
                    PRetVal = VinIdx;
                    SvcRawEnc_ProcCallBack(SVC_RAW_ENC_CODE_PRE_AAA, &PRetVal);

                    /* Load the hds data */
                    RetVal = SvcRawEnc_ExecuteAaaData(pCurVinCfg, SVC_RawEncCtrl.pItnMemBuf, SVC_RawEncCtrl.ItnMemSize);

                    /* Process callback */
                    PRetVal = VinIdx;
                    SvcRawEnc_ProcCallBack(SVC_RAW_ENC_CODE_POST_AAA, &PRetVal);
                }

                /* Load the related ViewZone idsp setting */
                if (RetVal == SVC_OK) {

                    /* Process callback */
                    PRetVal = SVC_RawEncCtrl.ViewZoneSelectBits;
                    SvcRawEnc_ProcCallBack(SVC_RAW_ENC_CODE_PRE_ITN, &PRetVal);

                    /* Load the ituner data */
                    RetVal = SvcRawEnc_ExecuteItnData();

                    /* Process callback */
                    PRetVal = SVC_RawEncCtrl.ViewZoneSelectBits;
                    SvcRawEnc_ProcCallBack(SVC_RAW_ENC_CODE_POST_ITN, &PRetVal);
                }

                /* Execute idsp */
                if (RetVal == SVC_OK) {
                    RetVal = SvcRawEnc_ExecuteIdsp();
                }

                /* Load the external hds data */
                if ((RetVal == SVC_OK) &&
                    ((pCurVinCfg->Status & SVC_RAW_ENC_HDS_UPDATE) == SVC_RAW_ENC_HDS_UPDATE)) {

                    /* Process callback */
                    PRetVal = VinIdx;
                    SvcRawEnc_ProcCallBack(SVC_RAW_ENC_CODE_PRE_HDS, &PRetVal);

                    /* Load the hds data */
                    RetVal = SvcRawEnc_ExecuteHdsData(pCurVinCfg, pCurBufInfo->pHdsBuf, pCurBufInfo->HdsBufSize);

                    /* Process callback */
                    PRetVal = VinIdx;
                    SvcRawEnc_ProcCallBack(SVC_RAW_ENC_CODE_POST_HDS, &PRetVal);
                }

                /* Load the external raw data */
                if (RetVal == SVC_OK) {

                    /* Process callback */
                    PRetVal = VinIdx;
                    SvcRawEnc_ProcCallBack(SVC_RAW_ENC_CODE_PRE_RAW, &PRetVal);

                    /* Load the raw data */
                    RetVal = SvcRawEnc_ExecuteRawData(pCurVinCfg, pCurBufInfo->pRawBuf, pCurBufInfo->RawBufSize);
                    if (RetVal == SVC_NG) {
                        PRN_RENC_LOG "Fail to execute raw enc - execute VinID(%d) raw data fail!"
                            PRN_RENC_ARG_UINT32 VinIdx PRN_RENC_ARG_POST
                        PRN_RENC_NG
                    } else {
                        pCurBufInfo->Status = SVC_RAW_ENC_BUF_USED;
                    }

                    /* Process callback */
                    PRetVal = VinIdx;
                    SvcRawEnc_ProcCallBack(SVC_RAW_ENC_CODE_POST_RAW, &PRetVal);
                }
            }

            if (RetVal != SVC_OK) {
                break;
            }
        }

        if (RetVal == SVC_OK) {

            /* Process callback */
            SvcRawEnc_ProcCallBack(SVC_RAW_ENC_CODE_PRE_FEED, &(SVC_RawEncCtrl.VinSelectBits));

            PRetVal = SvcRawEnc_ExecuteCmd(NumVin, ViewZoneIDArr, ExtBufs, BufInfo); PRN_RENC_ERR_HDLR

            /* Process callback */
            SvcRawEnc_ProcCallBack(SVC_RAW_ENC_CODE_POST_FEED, &(SVC_RawEncCtrl.VinSelectBits));

        }

        SvcRawEnc_MutexGive(&(SVC_RawEncCtrl.Mutex));
    }

    return RetVal;
}

/**
* get the raw encode resolution configuration
* @return resolution configuration
*/
SVC_RES_CFG_s *SvcRawEnc_ResCfgGet(void)
{
    SvcRawEnc_DefResCfgInit();

    return &RawEncResCfg;
}

/**
* get the supported vout driver id
* @param [in] pDrv vout driver
* @param [out] pID id of vout driver
* @return 0-OK, 1-NG
*/
UINT32 SvcRawEnc_VoutDevGet(const void *pDrv, UINT32 *pID)
{
    UINT32 RetVal = SVC_OK;

    AmbaMisra_TouchUnused(&pDrv);

    if ((pDrv == NULL) || (pID == NULL)) {
        RetVal = SVC_NG;
    } else {
        UINT32 Idx;
        const void *pCurDrv;

        SvcRawEnc_VoutDevInit();

        *pID = 0xCafeU;

        for (Idx = 0U; Idx < SVC_RAW_ENC_MAX_VOUT_DEV_NUM; Idx ++) {
            pCurDrv = RawEncVoutDevInfo[Idx].pObj;
            if (pDrv == pCurDrv) {
                *pID = RawEncVoutDevInfo[Idx].ID;
                break;
            }
        }
    }

    return RetVal;
}

/**
* register callback of raw encode module
* @param [in] pCallBack callback function
* @return none
*/
void SvcRawEnc_CallBackRegister(SVC_RAW_ENC_CALLBACK_f pCallBack)
{
    RawEncCallBackProc = pCallBack;
}

/**
* shell commands installation of raw encode module
* @return none
*/
void SvcRawEnc_CmdInstall(void)
{
    AMBA_SHELL_COMMAND_s SvcRawEncCmdApp;

    SvcRawEncCmdApp.pName    = "svc_rawenc";
    SvcRawEncCmdApp.MainFunc = SvcRawEnc_ShellEntry;
    SvcRawEncCmdApp.pNext    = NULL;

    if (SHELL_ERR_SUCCESS != SvcCmd_CommandRegister(&SvcRawEncCmdApp)) {
        PRN_RENC_LOG "Fail to install svc raw encode command!" PRN_RENC_NG
    }
}

/**
* dump shell command usage to file
* @param [in] pFile file pointer
* @param [in] ArgCount shell command argument counter
* @param [in] pArgVector shell command argument vector
* @return none
*/
void SvcRawEnc_DumpCmdUsage(AMBA_FS_FILE *pFile, UINT32 ArgCount, char * const *pArgVector)
{
    if (pFile == NULL) {
        PRN_RENC_LOG "Fail to dump cmd usage - invalid file input" PRN_RENC_NG
    } else if (pArgVector == NULL) {
        PRN_RENC_LOG "Fail to dump cmd usage - invalid argument vector" PRN_RENC_NG
#if 0
    } else if ((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_INIT) == 0U) {
        PRN_RENC_LOG "Fail to dump cmd usage - initial video raw enc module first" PRN_RENC_NG
    } else if ((SVC_RawEncCtrlFlag & SVC_RAW_ENC_FLG_SHELL_INIT) == 0U) {
        PRN_RENC_LOG "Fail to dump cmd usage - initial video raw enc shell module first" PRN_RENC_NG
#endif
    } else {
        SVC_RawEncCtrl.pDumpLogFile = pFile;

        SVC_RawEncCtrlFlag |= SVC_RAW_ENC_FLG_MSG_SHELL;
        SvcRawEnc_ShellEntry(ArgCount, pArgVector, NULL);
        SVC_RawEncCtrlFlag &= ~SVC_RAW_ENC_FLG_MSG_SHELL;
    }
}
