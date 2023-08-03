/**
*  @file SvcCalibBpc.c
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
*  @details C file for Calibration Bad Pixel
*
*/

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaSvcWrap.h"
#include "AmbaUtility.h"
#include "AmbaFS.h"
#include "AmbaVIN_Def.h"
#include "AmbaDSP_ImageUtility.h"

#include "AmbaCalib_ErrNo.h"
#include "AmbaCalib_StaticBadPixelIF.h"

#include "AmbaCT_TextHdlr.h"
#include "AmbaCT_BpcTunerIF.h"

#include "SvcWrap.h"
#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcCalibMgr.h"

#define SVC_CALIB_BPC_FLG_INIT              (0x1U)
#define SVC_CALIB_BPC_FLG_SHADOW_INIT       (0x2U)
#define SVC_CALIB_BPC_FLG_CALC_INIT         (0x4U)
#define SVC_CALIB_BPC_FLG_CALC_VIN_SEN      (0x8U)
#define SVC_CALIB_BPC_FLG_CALC_CTN_MAX_WIN  (0x10U)
#define SVC_CALIB_BPC_FLG_SHELL_INIT        (0x100U)
#define SVC_CALIB_BPC_FLG_CMD_INIT          (0x200U)
/* Reserve 0x10000/0x20000/0x40000/0x80000 for SvcCalibMgr.h */

#define SVC_CALIB_BPC_NAME               ("SvcCalib_Bpc")
#define SVC_CALIB_BPC_NAND_HEADER_SIZE   (0x200U)
#define SVC_CALIB_BPC_VERSION            (0x20180401U)
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#define SVC_CALIB_BPC_MAX_RAW_WIDTH      (7680U)
#define SVC_CALIB_BPC_MAX_RAW_HEIGHT     (4320U)
#define SVC_CALIB_BPC_MAX_DATA_SIZE      (0x3F4800U)
#else
#define SVC_CALIB_BPC_MAX_RAW_WIDTH      (3840U)
#define SVC_CALIB_BPC_MAX_RAW_HEIGHT     (2160U)
#define SVC_CALIB_BPC_MAX_DATA_SIZE      (0x16E360U)
#endif
#define SVC_CALIB_BPC_MAX_RESOLUTION     (16U)

typedef struct {
    UINT32 Enable;
    UINT32 NumOfTable;
    UINT32 DebugMsgOn;
    UINT32 TableSelectBits;
    UINT32 Reserved[(SVC_CALIB_BPC_NAND_HEADER_SIZE / 4U) - 4U];
} SVC_CALIB_BPC_NAND_HEADER_s;

typedef struct {
    UINT32                     Version;
    SVC_CALIB_VIN_SENSOR_GEO_s VinSensorGeo;
    UINT32                     VinSelectBits;
    UINT32                     SensorSelectBits;
    UINT32                     SensorModeBits;
    UINT32                     BpcCount;
} SVC_CALIB_BPC_TABLE_HEADER_s;

typedef struct {
    SVC_CALIB_BPC_TABLE_HEADER_s Header;
    UINT32                       DataType;
    UINT32                       DataOfs;
    UINT32                       DataSize;
} SVC_CALIB_BPC_NAND_TABLE_s;

typedef struct {
    UINT32  CfgDone;
} SVC_CALIB_BPC_TABLE_STATE_s;

typedef struct {
    UINT32                       VinSelectBits;
    UINT32                       SensorSelectBits;
    UINT32                       SensorModeBits;
    SVC_CALIB_BPC_TABLE_STATE_s  TableState;
    AMBA_CAL_BPC_CALIB_DATA_s    Table;
} SVC_CALIB_BPC_TABLE_s;

typedef struct {
    UINT32                          TableIdx;
    UINT32                          DataType;
    UINT32                          Lz77SearchWin;
    UINT32                          SaveBinOn;
    UINT32                          MaxRawWidth;
    UINT32                          MaxRawHeight;
} SVC_CALIB_BPC_CALC_CTRL_s;

#define SVC_CALIB_BPC_MEM_CALC        (0x0U)
#define SVC_CALIB_BPC_MEM_CALC_CTUNER (0x1U)
#define SVC_CALIB_BPC_MEM_NUM         (0x2U)
typedef struct {
    UINT32  Enable;
    UINT8  *pBuf;
    UINT32  BufSize;
} SVC_CALIB_BPC_MEM_CTRL_s;

typedef struct {
    char                           Name[16];
    AMBA_KAL_MUTEX_t               Mutex;

    SVC_CALIB_BPC_NAND_HEADER_s   *pNandHeader;
    SVC_CALIB_BPC_NAND_TABLE_s    *pNandTable;
    UINT32                         NumOfTable;
    SVC_CALIB_BPC_TABLE_s         *pBpcTable;

    UINT8                         *pNandDataBase;
    UINT8                         *pNandDataLimit;
    UINT8                         *pNandDataFree;

    SVC_CALIB_BPC_MEM_CTRL_s       MemCtrl[SVC_CALIB_BPC_MEM_NUM];
    SVC_CALIB_BPC_CALC_CTRL_s      CalcCtrl;
    SVC_CALIB_CALC_CB_f            CalcCbFunc;
    SVC_CALIB_CHANNEL_s            CalibChan;
    UINT32                         EnableDebugMsg;
} SVC_CALIB_BPC_CTRL_s;

static UINT32                  SvcCalib_BpcDriverSlotCmp(char Val0, char Val1);
static UINT32                  SvcCalib_BpcMutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut);
static void                    SvcCalib_BpcMutexGive(AMBA_KAL_MUTEX_t *pMutex);
static void                    SvcCalib_BpcErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode);

static UINT32                  SvcCalib_BpcMemAddrComp(const UINT8 *pVal0, const UINT8 *pVal1);
static UINT32                  SvcCalib_BpcMemAddrDiff(const UINT8 *pVal0, const UINT8 *pVal1);
static UINT32                  SvcCalib_BpcMemQuery(UINT32 MaxTableNum, UINT32 *pShadowSize, UINT32 *pWorkMemSize, UINT32 *pCalcMemSize);
static UINT32                  SvcCalib_BpcMemLock(void);
static void                    SvcCalib_BpcMemUnLock(void);
static UINT32                  SvcCalib_BpcCtnPreFetch(const char *pScriptPath);
static UINT32                  SvcCalib_BpcVinSensorIDCheck(UINT32 VinID, UINT32 SensorID);
static UINT32                  SvcCalib_BpcCreate(void);
static UINT32                  SvcCalib_BpcShadowInit(const SVC_CALIB_OBJ_s *pCalObj);
static UINT32                  SvcCalib_BpcTableUpdate(const SVC_CALIB_CHANNEL_s *pCalibChan, const AMBA_IK_MODE_CFG_s *pImgMode);
static UINT32                  SvcCalib_BpcTableCfg(UINT32 NandTableID, UINT32 TableID);
static SVC_CALIB_BPC_TABLE_s * SvcCalib_BpcTableGet(const SVC_CALIB_CHANNEL_s *pCalibChan);
static UINT32                  SvcCalib_BpcTableGenTbl(const SVC_CALIB_CHANNEL_s *pCalibChan, const char *pScriptPath);
static UINT32                  SvcCalib_BpcTableDataGet(UINT32 TblID, SVC_CALIB_TBL_INFO_s *pTblInfo);
static UINT32                  SvcCalib_BpcTableDataSet(UINT32 TblID, SVC_CALIB_TBL_INFO_s *pTblInfo);
static UINT32                  SvcCalib_BpcTableSearch(UINT32 VinID, UINT32 SensorID, UINT32 SensorMode, UINT32 *pTblIdx);
static UINT32                  SvcCalib_BpcNandTableSearch(UINT32 VinID, UINT32 SensorID, UINT32 SensorMode, UINT32 *pTblIdx);

// Private function definition for calibration command
typedef UINT32 (*SVC_CALIB_BPC_CMD_FUNC_f)(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
typedef struct {
     UINT32                   Enable;
     UINT32                   CmdID;
     SVC_CALIB_BPC_CMD_FUNC_f pHandler;
} SVC_CALIB_BPC_CMD_HDLR_s;

static void   SvcCalib_BpcCmdHdlrInit(void);
static UINT32 SvcCalib_BpcCmdMemQuery(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_BpcCmdCreate(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_BpcCmdDataInit(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_BpcCmdTblUpdate(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_BpcCmdTblDataGet(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_BpcCmdTblDataSet(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
// UINT32 SvcCalib_BpcCmdFunc(UINT32 CmdID, UINT32 Param1, UINT32 Param2, UINT32 Param3, UINT32 Param4);

// Private function definition for shell command.
typedef UINT32 (*SVC_CALIB_BPC_SHELL_FUNC_f)(UINT32 ArgCount, char * const *pArgVector);
typedef void   (*SVC_CALIB_BPC_SHELL_USAGE_f)(void);

typedef struct {
    UINT32                      Enable;
    char                        ShellCmdName[32];
    SVC_CALIB_BPC_SHELL_FUNC_f  pFunc;
    SVC_CALIB_BPC_SHELL_USAGE_f pUsageFunc;
} SVC_CALIB_BPC_SHELL_FUNC_s;

static void   SvcCalib_BpcShellStrToU32(const char *pString, UINT32 *pValue);
static UINT32 SvcCalib_BpcShellCfgCalChan(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_BpcShellCfgCalChanU(void);
static UINT32 SvcCalib_BpcShellCfgTblID(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_BpcShellCfgTblIDU(void);
static UINT32 SvcCalib_BpcShellCfgDbgMsg(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_BpcShellCfgDbgMsgU(void);
static UINT32 SvcCalib_BpcShellCfgDataType(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_BpcShellCfgDataTypeU(void);
static UINT32 SvcCalib_BpcShellGenTbl(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_BpcShellGenTblU(void);
static UINT32 SvcCalib_BpcShellEnable(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_BpcShellEnableU(void);
static UINT32 SvcCalib_BpcShellUpd(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_BpcShellUpdU(void);
static UINT32 SvcCalib_BpcShellSet(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_BpcShellSetU(void);
static void   SvcCalib_BpcShellEntryInit(void);
static void   SvcCalib_BpcShellUsage(void);

#define SVC_CALIB_BPC_SHELL_CMD_NUM  (8U)
static SVC_CALIB_BPC_SHELL_FUNC_s CalibBpcShellFunc[SVC_CALIB_BPC_SHELL_CMD_NUM]  GNU_SECTION_NOZEROINIT;
static SVC_CALIB_BPC_CMD_HDLR_s SvcCalibBpcCmdHdlr[SVC_CALIB_CMD_NUM] GNU_SECTION_NOZEROINIT;
static SVC_CALIB_BPC_CTRL_s SVC_CalibBpcCtrl GNU_SECTION_NOZEROINIT;
static UINT32 SVC_CalibBpcCtrlFlag = SVC_LOG_CAL_DEF_FLG;

#define SVC_LOG_BPC "BPC"
#define PRN_CAL_BPC_LOG        { SVC_WRAP_PRINT_s CalibBpcPrint; AmbaSvcWrap_MisraMemset(&(CalibBpcPrint), 0, sizeof(CalibBpcPrint)); CalibBpcPrint.Argc --; CalibBpcPrint.pStrFmt =
#define PRN_CAL_BPC_ARG_UINT32 ; CalibBpcPrint.Argc ++; CalibBpcPrint.Argv[CalibBpcPrint.Argc].Uint64   = (UINT64)((
#define PRN_CAL_BPC_ARG_CSTR   ; CalibBpcPrint.Argc ++; CalibBpcPrint.Argv[CalibBpcPrint.Argc].pCStr    = ((
#define PRN_CAL_BPC_ARG_CPOINT ; CalibBpcPrint.Argc ++; CalibBpcPrint.Argv[CalibBpcPrint.Argc].pPointer = ((
#define PRN_CAL_BPC_ARG_POST   ))
#define PRN_CAL_BPC_OK         ; CalibBpcPrint.Argc ++; SvcCalib_BpcPrintLog(SVC_LOG_CAL_OK , &(CalibBpcPrint)); }
#define PRN_CAL_BPC_NG         ; CalibBpcPrint.Argc ++; SvcCalib_BpcPrintLog(SVC_LOG_CAL_NG , &(CalibBpcPrint)); }
#define PRN_CAL_BPC_API        ; CalibBpcPrint.Argc ++; SvcCalib_BpcPrintLog(SVC_LOG_CAL_API, &(CalibBpcPrint)); }
#define PRN_CAL_BPC_DBG        ; CalibBpcPrint.Argc ++; SvcCalib_BpcPrintLog(SVC_LOG_CAL_DBG, &(CalibBpcPrint)); }
#define PRN_CAL_BPC_ERR_HDLR   SvcCalib_BpcErrHdlr(__func__, __LINE__, PRetVal);

static void SvcCalib_BpcPrintLog(UINT32 LogLevel, SVC_WRAP_PRINT_s *pPrint)
{
    if (pPrint != NULL) {
        if ((SVC_CalibBpcCtrlFlag & LogLevel) > 0U) {

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

            SvcWrap_Print(SVC_LOG_BPC, pPrint);
        }
    }
}

static UINT32 SvcCalib_BpcMutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut)
{
    return AmbaKAL_MutexTake(pMutex, TimeOut);
}

static void SvcCalib_BpcMutexGive(AMBA_KAL_MUTEX_t *pMutex)
{
    UINT32 ErrCode = AmbaKAL_MutexGive(pMutex);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }
}

static void SvcCalib_BpcErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode)
{
    if (pCaller != NULL) {
        if (ErrCode != 0U) {
            PRN_CAL_BPC_LOG "Catch ErrCode(0x%08x) @ %s, %d"
                PRN_CAL_BPC_ARG_UINT32 ErrCode  PRN_CAL_BPC_ARG_POST
                PRN_CAL_BPC_ARG_CSTR   pCaller  PRN_CAL_BPC_ARG_POST
                PRN_CAL_BPC_ARG_UINT32 CodeLine PRN_CAL_BPC_ARG_POST
            PRN_CAL_BPC_NG
        }
    }
}

static UINT32 SvcCalib_BpcMemQuery(UINT32 MaxTableNum, UINT32 *pShadowSize, UINT32 *pWorkMemSize, UINT32 *pCalcMemSize)
{
    UINT32 RetVal = SVC_OK;

    UINT32 DramShadowSize = 0, WorkingMemSize = 0, CalcMemSize = 0;
    UINT32 DramShadowHeaderSize = (UINT32)(sizeof(SVC_CALIB_BPC_NAND_HEADER_s));
    UINT32 DramShadowTableSize  = (UINT32)(sizeof(SVC_CALIB_BPC_NAND_TABLE_s));
    UINT32 DramShadowDataSize;
    UINT32 BpcTableSize         = (UINT32)(sizeof(SVC_CALIB_BPC_TABLE_s));
    UINT32 TotalBpcTableSize    = BpcTableSize * MaxTableNum;
    SIZE_t CalcMemAlgoSize      = 0U;
    SIZE_t CalcMemCtnSize       = 0U;
    AMBA_CT_CONFIG_s CtnCfg;
    AMBA_CAL_SIZE_s  MaxRawSize;

    AmbaSvcWrap_MisraMemset(&CtnCfg, 0, sizeof(CtnCfg));
    if ((SVC_CalibBpcCtrlFlag & SVC_CALIB_BPC_FLG_CALC_CTN_MAX_WIN) > 0U) {
        CtnCfg.Type.Bpc.MaxImageSize.Width  = SVC_CalibBpcCtrl.CalcCtrl.MaxRawWidth;
        CtnCfg.Type.Bpc.MaxImageSize.Height = SVC_CalibBpcCtrl.CalcCtrl.MaxRawHeight;
    } else {
        CtnCfg.Type.Bpc.MaxImageSize.Width  = SVC_CALIB_BPC_MAX_RAW_WIDTH;
        CtnCfg.Type.Bpc.MaxImageSize.Height = SVC_CALIB_BPC_MAX_RAW_HEIGHT;
    }
    CtnCfg.Type.Bpc.Resolution = SVC_CALIB_BPC_MAX_RESOLUTION;

    PRN_CAL_BPC_LOG "Ctuner max window %04dx%04d, %d"
        PRN_CAL_BPC_ARG_UINT32 CtnCfg.Type.Bpc.MaxImageSize.Width  PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_ARG_UINT32 CtnCfg.Type.Bpc.MaxImageSize.Height PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_ARG_UINT32 CtnCfg.Type.Bpc.Resolution          PRN_CAL_BPC_ARG_POST
    PRN_CAL_BPC_DBG

    AmbaSvcWrap_MisraMemset(&MaxRawSize, 0, sizeof(MaxRawSize));
    MaxRawSize.Width  = CtnCfg.Type.Bpc.MaxImageSize.Width;
    MaxRawSize.Height = CtnCfg.Type.Bpc.MaxImageSize.Height;

    DramShadowTableSize += ((MaxRawSize.Width * MaxRawSize.Height) / 8U);
    DramShadowDataSize  = DramShadowTableSize * MaxTableNum;

    if (0U != AmbaCT_Cfg(AMBA_CT_TYPE_BPC, &CtnCfg)) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to query bpc table memory - config ctuner module fail!" PRN_CAL_BPC_NG
    } else if (0U != AmbaCT_GetBufSize(AMBA_CT_TYPE_BPC, &CalcMemCtnSize)) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to query bpc module memory - query bpc ctuner working memory size fail!" PRN_CAL_BPC_NG
    } else if (0U != AmbaCal_BpcGetWorkingBufSize(&MaxRawSize, &CalcMemAlgoSize)) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to query bpc table memory - get bpc algo working memory fail!" PRN_CAL_BPC_NG
    } else {
        DramShadowSize  = DramShadowHeaderSize;
        DramShadowSize += DramShadowDataSize;

        WorkingMemSize  = TotalBpcTableSize;

        CalcMemSize    += (UINT32)CalcMemAlgoSize;
        CalcMemSize    += (UINT32)CalcMemCtnSize;

        PRN_CAL_BPC_LOG " " PRN_CAL_BPC_DBG
        PRN_CAL_BPC_LOG "====== Calib BPC Memory Query ======" PRN_CAL_BPC_DBG
        PRN_CAL_BPC_LOG "  MaxTableNum    : %d"
            PRN_CAL_BPC_ARG_UINT32 MaxTableNum PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_DBG
        PRN_CAL_BPC_LOG "  DramShadowSize : 0x%X"
            PRN_CAL_BPC_ARG_UINT32 DramShadowSize PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_DBG
        PRN_CAL_BPC_LOG "    DramShadowHeaderSize : 0x%X"
            PRN_CAL_BPC_ARG_UINT32 DramShadowHeaderSize PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_DBG
        PRN_CAL_BPC_LOG "    DramShadowDataSize   : 0x%X ( 0x%X x %d )"
            PRN_CAL_BPC_ARG_UINT32 DramShadowDataSize  PRN_CAL_BPC_ARG_POST
            PRN_CAL_BPC_ARG_UINT32 DramShadowTableSize PRN_CAL_BPC_ARG_POST
            PRN_CAL_BPC_ARG_UINT32 MaxTableNum         PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_DBG
        PRN_CAL_BPC_LOG "  WorkingMemSize : 0x%X"
            PRN_CAL_BPC_ARG_UINT32 WorkingMemSize PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_DBG
        PRN_CAL_BPC_LOG "    TotalLdcTableSize : 0x%X ( 0x%X x %d )"
            PRN_CAL_BPC_ARG_UINT32 TotalBpcTableSize PRN_CAL_BPC_ARG_POST
            PRN_CAL_BPC_ARG_UINT32 BpcTableSize      PRN_CAL_BPC_ARG_POST
            PRN_CAL_BPC_ARG_UINT32 MaxTableNum       PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_DBG
        PRN_CAL_BPC_LOG "  CalcMemSize : 0x%X"
            PRN_CAL_BPC_ARG_UINT32 CalcMemSize PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_DBG
        PRN_CAL_BPC_LOG "    CalcMemAlgoSize    : 0x%X"
            PRN_CAL_BPC_ARG_UINT32 CalcMemAlgoSize PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_DBG
        PRN_CAL_BPC_LOG "    CalcMemCtnSize     : 0x%X"
            PRN_CAL_BPC_ARG_UINT32 CalcMemCtnSize PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_DBG

        if (pShadowSize != NULL) {
            *pShadowSize  = DramShadowSize;
        }
        PRN_CAL_BPC_LOG "Query the calib bpc dram shadow size 0x%X ( table num %d )"
            PRN_CAL_BPC_ARG_UINT32 DramShadowSize PRN_CAL_BPC_ARG_POST
            PRN_CAL_BPC_ARG_UINT32 MaxTableNum    PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_DBG

        if (pWorkMemSize != NULL) {
            *pWorkMemSize = WorkingMemSize;
        }
        PRN_CAL_BPC_LOG "Query the calib bpc working memory size 0x%X ( table num %d )"
            PRN_CAL_BPC_ARG_UINT32 WorkingMemSize PRN_CAL_BPC_ARG_POST
            PRN_CAL_BPC_ARG_UINT32 MaxTableNum    PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_DBG

        if (pCalcMemSize != NULL) {
            *pCalcMemSize = CalcMemSize;
        }
        PRN_CAL_BPC_LOG "Query the calib bpc algo. memory size 0x%X"
            PRN_CAL_BPC_ARG_UINT32 CalcMemSize PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_DBG
    }

    return RetVal;
}

static UINT32 SvcCalib_BpcMemAddrComp(const UINT8 *pVal0, const UINT8 *pVal1)
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

static UINT32 SvcCalib_BpcMemAddrDiff(const UINT8 *pVal0, const UINT8 *pVal1)
{
    UINT32 RetVal = 0U;

#ifdef CONFIG_ARM64
    UINT64 Val_A = 0U, Val_B = 0U, Diff;
#else
    UINT32 Val_A = 0U, Val_B = 0U, Diff;
#endif

    AmbaMisra_TypeCast(&(Val_A), &(pVal0));
    AmbaMisra_TypeCast(&(Val_B), &(pVal1));

    if (Val_A > Val_B) {
        Diff = Val_A - Val_B;
    } else {
        Diff = Val_B - Val_A;
    }
    RetVal = (UINT32)Diff;

    return RetVal;
}

static UINT32 SvcCalib_BpcMemLock(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    SIZE_t CalcMemAlgoSize = 0U;
    SIZE_t CalcMemCtnSize = 0U;
    UINT8 *pCalcMem = NULL;
    UINT32 CalcMemSize = 0U;
    AMBA_CT_CONFIG_s CtnCfg;
    AMBA_CAL_SIZE_s MaxRawSize;

    AmbaSvcWrap_MisraMemset(&CtnCfg, 0, sizeof(CtnCfg));
    if ((SVC_CalibBpcCtrlFlag & SVC_CALIB_BPC_FLG_CALC_CTN_MAX_WIN) > 0U) {
        CtnCfg.Type.Bpc.MaxImageSize.Width  = SVC_CalibBpcCtrl.CalcCtrl.MaxRawWidth;
        CtnCfg.Type.Bpc.MaxImageSize.Height = SVC_CalibBpcCtrl.CalcCtrl.MaxRawHeight;
    } else {
        CtnCfg.Type.Bpc.MaxImageSize.Width  = SVC_CALIB_BPC_MAX_RAW_WIDTH;
        CtnCfg.Type.Bpc.MaxImageSize.Height = SVC_CALIB_BPC_MAX_RAW_HEIGHT;
    }
    CtnCfg.Type.Bpc.Resolution          = SVC_CALIB_BPC_MAX_RESOLUTION;

    PRN_CAL_BPC_LOG "Ctuner max window %04dx%04d, %d"
        PRN_CAL_BPC_ARG_UINT32 CtnCfg.Type.Bpc.MaxImageSize.Width  PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_ARG_UINT32 CtnCfg.Type.Bpc.MaxImageSize.Height PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_ARG_UINT32 CtnCfg.Type.Bpc.Resolution          PRN_CAL_BPC_ARG_POST
    PRN_CAL_BPC_DBG

    AmbaSvcWrap_MisraMemset(&MaxRawSize, 0, sizeof(MaxRawSize));
    MaxRawSize.Width = CtnCfg.Type.Bpc.MaxImageSize.Width;
    MaxRawSize.Height = CtnCfg.Type.Bpc.MaxImageSize.Height;

    /* Configure the bpc algo memory */
    if (0U != AmbaCT_Cfg(AMBA_CT_TYPE_BPC, &CtnCfg)) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to cfg bpc table memory - config ctuner module fail!" PRN_CAL_BPC_NG
    } else if (0U != AmbaCT_GetBufSize(AMBA_CT_TYPE_BPC, &CalcMemCtnSize)) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to cfg bpc table memory - query bpc ctuner working memory size fail!" PRN_CAL_BPC_NG
    } else if (0U != AmbaCal_BpcGetWorkingBufSize(&MaxRawSize, &CalcMemAlgoSize)) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to cfg bpc table memory - get bpc algo working memory fail!" PRN_CAL_BPC_NG
    } else {
        CalcMemSize = (UINT32)CalcMemAlgoSize + (UINT32)CalcMemCtnSize;

        if (0U != SvcCalib_ItemCalcMemGet(SVC_CALIB_BPC_ID, 1U, &pCalcMem, &CalcMemSize)) {
            RetVal = SVC_NG;
            PRN_CAL_BPC_LOG "Fail to cfg bpc table memory - query bpc ctuner working memory size fail!" PRN_CAL_BPC_NG
        } else {
            if (CalcMemSize < ((UINT32)CalcMemAlgoSize + (UINT32)CalcMemCtnSize)) {
                RetVal = SVC_NG;
                PRN_CAL_BPC_LOG "Fail to cfg bpc table memory - calib calculation memory is not enough to service it!" PRN_CAL_BPC_NG
            } else if (pCalcMem == NULL) {
                RetVal = SVC_NG;
                PRN_CAL_BPC_LOG "Fail to cfg bpc table memory - invalid calib calculation memory!" PRN_CAL_BPC_NG
            } else {
                AmbaSvcWrap_MisraMemset(&(SVC_CalibBpcCtrl.MemCtrl[SVC_CALIB_BPC_MEM_CALC_CTUNER]), 0, sizeof(SVC_CALIB_BPC_MEM_CTRL_s));
                SVC_CalibBpcCtrl.MemCtrl[SVC_CALIB_BPC_MEM_CALC_CTUNER].BufSize = (UINT32)CalcMemCtnSize;
                SVC_CalibBpcCtrl.MemCtrl[SVC_CALIB_BPC_MEM_CALC_CTUNER].pBuf    = pCalcMem;
                SVC_CalibBpcCtrl.MemCtrl[SVC_CALIB_BPC_MEM_CALC_CTUNER].Enable  = 1U;

                AmbaSvcWrap_MisraMemset(&(SVC_CalibBpcCtrl.MemCtrl[SVC_CALIB_BPC_MEM_CALC]), 0, sizeof(SVC_CALIB_BPC_MEM_CTRL_s));
                SVC_CalibBpcCtrl.MemCtrl[SVC_CALIB_BPC_MEM_CALC].BufSize = CalcMemSize - (UINT32)CalcMemAlgoSize;
                SVC_CalibBpcCtrl.MemCtrl[SVC_CALIB_BPC_MEM_CALC].pBuf    = &(pCalcMem[CalcMemCtnSize]);
                SVC_CalibBpcCtrl.MemCtrl[SVC_CALIB_BPC_MEM_CALC].Enable  = 1U;

                PRN_CAL_BPC_LOG "Successful to lock bpc calc memory!" PRN_CAL_BPC_API
                PRN_CAL_BPC_LOG "  Ctuner: %p/0x%X, AlgoCalc: %p/0x%X"
                PRN_CAL_BPC_ARG_CPOINT SVC_CalibBpcCtrl.MemCtrl[SVC_CALIB_BPC_MEM_CALC_CTUNER].pBuf    PRN_CAL_BPC_ARG_POST
                PRN_CAL_BPC_ARG_UINT32 SVC_CalibBpcCtrl.MemCtrl[SVC_CALIB_BPC_MEM_CALC_CTUNER].BufSize PRN_CAL_BPC_ARG_POST
                PRN_CAL_BPC_ARG_CPOINT SVC_CalibBpcCtrl.MemCtrl[SVC_CALIB_BPC_MEM_CALC].pBuf    PRN_CAL_BPC_ARG_POST
                PRN_CAL_BPC_ARG_UINT32 SVC_CalibBpcCtrl.MemCtrl[SVC_CALIB_BPC_MEM_CALC].BufSize PRN_CAL_BPC_ARG_POST
                PRN_CAL_BPC_API
            }

            if (RetVal != SVC_OK) {
                // Free calib calculation memory
                PRetVal = SvcCalib_ItemCalcMemFree(SVC_CALIB_BPC_ID); PRN_CAL_BPC_ERR_HDLR
            }
        }
    }

    return RetVal;
}


static void SvcCalib_BpcMemUnLock(void)
{
    // Reset the memory control setting
    AmbaSvcWrap_MisraMemset(SVC_CalibBpcCtrl.MemCtrl, 0, sizeof(SVC_CalibBpcCtrl.MemCtrl));

    // Free the calculation memory
    if (SVC_OK != SvcCalib_ItemCalcMemFree(SVC_CALIB_BPC_ID)) {
        PRN_CAL_BPC_LOG "Fail to un-lock bpc calc memory - init calculation memory control fail!" PRN_CAL_BPC_NG
    } else {
        PRN_CAL_BPC_LOG "Successful to un-lock bpc calc memory!" PRN_CAL_BPC_OK
    }
}

static INT32 SvcCalib_BpcCvtCharToINT32(char Char)
{
    UINT32 Val = 0;

    AmbaMisra_TypeCast(&Val, &Char); Val &= 0xFFU;

    return (INT32)Val;
}

static UINT32 SvcCalib_BpcDriverSlotCmp(char Val0, char Val1)
{
    UINT32 RetCmp;
    INT32 CurVal0, CurVal1;
    const INT32 CHAR_a = 97;
    const INT32 CHAR_A = 65;

    if ((Val0 >= 'a') && (Val0 <= 'z')) {
        CurVal0 = CHAR_A + (SvcCalib_BpcCvtCharToINT32(Val0) - CHAR_a);
    } else if ((Val0 >= 'A' ) && (Val0 <= 'Z')) {
        CurVal0 = SvcCalib_BpcCvtCharToINT32(Val0);
    } else {
        CurVal0 = 0;
    }

    if ((Val1 >= 'a') && (Val1 <= 'z')) {
        CurVal1 = CHAR_A + (SvcCalib_BpcCvtCharToINT32(Val1) - CHAR_a);
    } else if ((Val1 >= 'A' ) && (Val1 <= 'Z')) {
        CurVal1 = SvcCalib_BpcCvtCharToINT32(Val1);
    } else {
        CurVal1 = 0;
    }

    if ((CurVal0 == 0) || (CurVal1 == 0)) {
        RetCmp = 1U;                    // invalid input
    } else if (CurVal0 == CurVal1) {
        RetCmp = 0U;                    // Same
    } else {
        RetCmp = 1U;                    // Not same
    }

    return RetCmp;
}

static UINT32 SvcCalib_BpcCtnReadLine(AMBA_FS_FILE *pFile, char *pDstBuf, UINT32 DstBufSize)
{
    UINT32 RetLeng = 0U;

    if (pFile == NULL) {
        RetLeng = 0U;
    } else if (pDstBuf == NULL) {
        RetLeng = 0U;
    } else {
        UINT32 ErrCode = 0xFFFFFFFFU;
        char TmpChar;
        UINT32 NumSuccess;

        while ((0U == AmbaFS_FileEof(pFile))
            && (RetLeng < (DstBufSize - 1U))) {

            ErrCode = AmbaFS_FileRead(&TmpChar, 1, 1, pFile, &NumSuccess);
            if ((NumSuccess != 1U) || (ErrCode != 0U)) {
                TmpChar = '\n';
            }

            if (TmpChar == '\n') {
                break;
            } else {
                if (TmpChar != '\r') {
                    pDstBuf[RetLeng] = TmpChar;
                    RetLeng += 1U;
                }
            }
        }

        if (pDstBuf[RetLeng - 1U] == '\n') {
            pDstBuf[RetLeng - 1U] = '\0';
        } else {
            pDstBuf[RetLeng] = '\0';
        }
    }

    return RetLeng;
}

static void SvcCalib_BpcCtnGetU32Val(const char *pStrBuf, UINT32 *pVal)
{
    if ((pStrBuf != NULL) && (pVal != NULL)) {
        UINT32 CvtVal = 0U;
        UINT32 PRetVal = SvcWrap_strtoul(pStrBuf, &CvtVal); PRN_CAL_BPC_ERR_HDLR;
        if (PRetVal != 0U) {
            CvtVal = 0U;
        }

        *pVal = CvtVal;
    }
}

static UINT32 SvcCalib_BpcCtnPreFetch(const char *pScriptPath)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    AMBA_FS_FILE *pFile = NULL;

    if (pScriptPath == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to pre-fetch ctuner - input script path should not null!" PRN_CAL_BPC_NG
    } else if (0U != AmbaFS_FileOpen(pScriptPath, "rb", &pFile)) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to pre-fetch ctuner - open script fail! script:'%s'"
            PRN_CAL_BPC_ARG_CSTR   pScriptPath PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_NG
    } else if (pFile == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to pre-fetch ctuner - invalid file pointer" PRN_CAL_BPC_NG
    } else {
        char LineBuf[128U];
        char *pCResult = NULL, *pCSearch = NULL;
        UINT32 TmpU32;
        UINT32 MaxWidth      = 0U;
        UINT32 MaxHeight     = 0U;

        AmbaSvcWrap_MisraMemset(LineBuf, 0, sizeof(LineBuf));
        while (0U < SvcCalib_BpcCtnReadLine(pFile, LineBuf, (UINT32)sizeof(LineBuf))) {

            pCSearch = LineBuf;
            pCResult = SvcWrap_strstr(pCSearch, "raw");
            if (pCResult != NULL) {
                pCSearch = pCResult;

                // Try to fetch 'raw[0].width 1920' pattern
                pCResult = SvcWrap_strstr(pCSearch, ".width ");
                if (pCResult != NULL) {
                    TmpU32 = 0U;
                    SvcCalib_BpcCtnGetU32Val(&(pCResult[7]), &TmpU32);
                    if (TmpU32 > MaxWidth) {
                        MaxWidth = TmpU32;
                    }
                    continue;
                }

                // Try to fetch 'raw[0].height 1080' pattern
                pCResult = SvcWrap_strstr(pCSearch, ".height ");
                if (pCResult != NULL) {
                    TmpU32 = 0U;
                    SvcCalib_BpcCtnGetU32Val(&(pCResult[8]), &TmpU32);
                    if (TmpU32 > MaxHeight) {
                        MaxHeight = TmpU32;
                    }
                    continue;
                }

                // Try to fetch 'raw[0].path ' pattern
                pCResult = SvcWrap_strstr(pCSearch, ".path ");
                if (pCResult != NULL) {
                    pCSearch = pCResult;
                    pCResult = SvcWrap_strstr(pCSearch, ":\\");
                    if (pCResult != NULL) {
                        pCResult --;
                        if (0U != SvcCalib_BpcDriverSlotCmp(*pCResult, pScriptPath[0])) {
                            PRN_CAL_BPC_LOG "Ctuner raw file device slot('%c') does not same with Ctuner script file device slot('%c')"
                                ; CalibBpcPrint.Argc ++; CalibBpcPrint.Argv[CalibBpcPrint.Argc].Char = *pCResult
                                ; CalibBpcPrint.Argc ++; CalibBpcPrint.Argv[CalibBpcPrint.Argc].Char = pScriptPath[0]
                            PRN_CAL_BPC_NG

                            RetVal = SVC_NG;
                        }
                    }

                    continue;
                }
            }
        }

        if ((MaxWidth > 0U) && (MaxHeight > 0U)) {
            SVC_CalibBpcCtrl.CalcCtrl.MaxRawWidth   = MaxWidth;
            SVC_CalibBpcCtrl.CalcCtrl.MaxRawHeight  = MaxHeight;
            SVC_CalibBpcCtrlFlag |= SVC_CALIB_BPC_FLG_CALC_CTN_MAX_WIN;

            PRN_CAL_BPC_LOG "Update BPC max raw window: %04dx%04d"
                PRN_CAL_BPC_ARG_UINT32 SVC_CalibBpcCtrl.CalcCtrl.MaxRawWidth   PRN_CAL_BPC_ARG_POST
                PRN_CAL_BPC_ARG_UINT32 SVC_CalibBpcCtrl.CalcCtrl.MaxRawHeight  PRN_CAL_BPC_ARG_POST
            PRN_CAL_BPC_API
        }

        PRetVal = AmbaFS_FileClose(pFile); PRN_CAL_BPC_ERR_HDLR

        AmbaMisra_TouchUnused(pCSearch);
        AmbaMisra_TouchUnused(pCResult);
    }

    return RetVal;
}

static UINT32 SvcCalib_BpcVinSensorIDCheck(UINT32 VinID, UINT32 SensorID)
{
    UINT32 RetVal = SVC_OK;

    if (VinID >= AMBA_NUM_VIN_CHANNEL) {
        RetVal = SVC_NG;
    }

    if ((SensorID != 1U) && (SensorID != 2U) &&
        (SensorID != 4U) && (SensorID != 8U)) {
        RetVal = SVC_NG;
    }

    return RetVal;
}


static UINT32 SvcCalib_BpcCreate(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibBpcCtrlFlag & SVC_CALIB_BPC_FLG_INIT) > 0U) {
        RetVal = SVC_OK;
        PRN_CAL_BPC_LOG "Calibration BPC module has been created!" PRN_CAL_BPC_DBG
    } else {
        // Reset the bpc control module
        AmbaSvcWrap_MisraMemset(&SVC_CalibBpcCtrl, 0, sizeof(SVC_CalibBpcCtrl));

        // Configure the name
        SvcWrap_strcpy(SVC_CalibBpcCtrl.Name, sizeof(SVC_CalibBpcCtrl.Name), SVC_CALIB_BPC_NAME);

        // Create the mutex
        RetVal = AmbaKAL_MutexCreate(&(SVC_CalibBpcCtrl.Mutex), SVC_CalibBpcCtrl.Name);
        if (RetVal != SVC_OK) {
            PRN_CAL_BPC_LOG "Fail to create bpc - create mutex fail!" PRN_CAL_BPC_NG
        } else {
            PRetVal = (SVC_CalibBpcCtrlFlag & (SVC_CALIB_BPC_FLG_SHELL_INIT |
                                               SVC_CALIB_BPC_FLG_CMD_INIT   |
                                               SVC_LOG_CAL_DEF_FLG          |
                                               SVC_LOG_CAL_DBG));
            SVC_CalibBpcCtrlFlag = PRetVal;
            SVC_CalibBpcCtrlFlag |= SVC_CALIB_BPC_FLG_INIT;
            PRN_CAL_BPC_LOG "Successful to create bpc!" PRN_CAL_BPC_OK
        }
    }

    return RetVal;
}


static UINT32 SvcCalib_BpcShadowInit(const SVC_CALIB_OBJ_s *pCalObj)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibBpcCtrlFlag & SVC_CALIB_BPC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to initial bpc dram shodow - create bpc first!" PRN_CAL_BPC_NG
    } else if (pCalObj == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to initial bpc dram shodow - invalid calib object!" PRN_CAL_BPC_NG
    } else if (pCalObj->Enable == 0U) {
        RetVal = 99;
        PRN_CAL_BPC_LOG "Disable bpc dram shadow!!" PRN_CAL_BPC_API
    } else {
        UINT32 WorkingMemSize = 0;
        const SVC_CALIB_BPC_NAND_HEADER_s *pHeader;
        AmbaMisra_TypeCast(&(pHeader), &(pCalObj->pShadowBuf));

        RetVal = SvcCalib_BpcMemQuery(pCalObj->NumOfTable, NULL, &WorkingMemSize, NULL);
        if (RetVal != SVC_OK) {
            PRN_CAL_BPC_LOG "Fail to initial bpc dram shadow - query memory fail!" PRN_CAL_BPC_NG
        } else if ((pCalObj->pShadowBuf == NULL) || (pHeader == NULL)) {
            RetVal = SVC_NG;
            PRN_CAL_BPC_LOG "Fail to initial bpc dram shadow - dram shadow buffer should not null!" PRN_CAL_BPC_NG
        } else if (pHeader->NumOfTable > pCalObj->NumOfTable) {
            RetVal = SVC_NG;
            PRN_CAL_BPC_LOG "Fail to initial bpc dram shadow - calib nand table count(%d) > max bpc dram shadow table count(%d)"
                PRN_CAL_BPC_ARG_UINT32 pHeader->NumOfTable PRN_CAL_BPC_ARG_POST
                PRN_CAL_BPC_ARG_UINT32 pCalObj->NumOfTable PRN_CAL_BPC_ARG_POST
            PRN_CAL_BPC_NG
        } else if (pCalObj->pWorkingBuf == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_BPC_LOG "Fail to initial bpc dram shadow - working buffer should not null!" PRN_CAL_BPC_NG
        } else if (pCalObj->WorkingBufSize < WorkingMemSize) {
            RetVal = SVC_NG;
            PRN_CAL_BPC_LOG "Fail to initial bpc dram shadow - working buffer size is too small!" PRN_CAL_BPC_NG
        } else {
            UINT8 *pNandHeaderBuf     = pCalObj->pShadowBuf;
            UINT32 NandHeaderSize     = (UINT32)(sizeof(SVC_CALIB_BPC_NAND_HEADER_s));
            UINT32 TotalNandTableSize = (UINT32)(sizeof(SVC_CALIB_BPC_NAND_TABLE_s)) * pCalObj->NumOfTable;
            UINT8 *pNandTableBuf      = &(pNandHeaderBuf[NandHeaderSize]);
            UINT8 *pNandDataBuf       = &(pNandTableBuf[TotalNandTableSize]);
            UINT32 TotalBpcTableSize  = (UINT32)(sizeof(SVC_CALIB_BPC_TABLE_s)) * pCalObj->NumOfTable;

            if (pCalObj->ShadowBufSize <= (NandHeaderSize + TotalNandTableSize)) {
                RetVal = SVC_NG;

                PRetVal = (NandHeaderSize + TotalNandTableSize);
                PRN_CAL_BPC_LOG "Fail to initial bpc dram shadow - configure shadow buffer is too small! 0x%X <= 0x%X"
                    PRN_CAL_BPC_ARG_UINT32 pCalObj->ShadowBufSize PRN_CAL_BPC_ARG_POST
                    PRN_CAL_BPC_ARG_UINT32 PRetVal                PRN_CAL_BPC_ARG_POST
                PRN_CAL_BPC_NG
            } else if (KAL_ERR_NONE != SvcCalib_BpcMutexTake(&(SVC_CalibBpcCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
                RetVal = SVC_NG;
                PRN_CAL_BPC_LOG "Fail to initial bpc dram shadow - take mutex fail!" PRN_CAL_BPC_NG
            } else {

                if (SVC_CalibBpcCtrl.EnableDebugMsg > 0U) {
                    PRN_CAL_BPC_LOG "====== Shadow initialized Start ======" PRN_CAL_BPC_DBG
                    PRN_CAL_BPC_LOG "  DramShadow Addr  : %p 0x%x"
                        PRN_CAL_BPC_ARG_CPOINT pCalObj->pShadowBuf    PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_ARG_UINT32 pCalObj->ShadowBufSize PRN_CAL_BPC_ARG_POST
                    PRN_CAL_BPC_DBG
                    PRN_CAL_BPC_LOG "    Nand Header Addr : %p 0x%x"
                        PRN_CAL_BPC_ARG_CPOINT pNandHeaderBuf PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_ARG_UINT32 NandHeaderSize PRN_CAL_BPC_ARG_POST
                    PRN_CAL_BPC_DBG
                    PRN_CAL_BPC_LOG "    Nand Table Addr  : %p 0x%x x %d"
                        PRN_CAL_BPC_ARG_CPOINT pNandTableBuf                      PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_ARG_UINT32 sizeof(SVC_CALIB_BPC_NAND_TABLE_s) PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_ARG_UINT32 pCalObj->NumOfTable                PRN_CAL_BPC_ARG_POST
                    PRN_CAL_BPC_DBG
                    PRN_CAL_BPC_LOG "    Nand Table Data  : %p"
                        PRN_CAL_BPC_ARG_CPOINT pNandDataBuf PRN_CAL_BPC_ARG_POST
                    PRN_CAL_BPC_DBG
                    PRN_CAL_BPC_LOG "  Working Mem Addr : %p 0x%x"
                        PRN_CAL_BPC_ARG_CPOINT pCalObj->pWorkingBuf    PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_ARG_UINT32 pCalObj->WorkingBufSize PRN_CAL_BPC_ARG_POST
                    PRN_CAL_BPC_DBG
                    PRN_CAL_BPC_LOG "    Bpc Table Addr   : %p 0x%08x"
                        PRN_CAL_BPC_ARG_CPOINT pCalObj->pWorkingBuf PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_ARG_UINT32 TotalBpcTableSize    PRN_CAL_BPC_ARG_POST
                    PRN_CAL_BPC_DBG
                }

                // Configure nand header
                AmbaMisra_TypeCast(&(SVC_CalibBpcCtrl.pNandHeader), &(pNandHeaderBuf));
                // Configure nand table
                AmbaMisra_TypeCast(&(SVC_CalibBpcCtrl.pNandTable), &(pNandTableBuf));
                // Configure working bpc table
                AmbaMisra_TypeCast(&(SVC_CalibBpcCtrl.pBpcTable),  &(pCalObj->pWorkingBuf));

                if (SVC_CalibBpcCtrl.pNandHeader == NULL) {
                    RetVal = SVC_NG;
                    PRN_CAL_BPC_LOG "Fail to initial bpc dram shadow - nand header should not null!" PRN_CAL_BPC_NG
                } else if (SVC_CalibBpcCtrl.pNandTable == NULL) {
                    RetVal = SVC_NG;
                    PRN_CAL_BPC_LOG "Fail to initial bpc dram shadow - nand table should not null!" PRN_CAL_BPC_NG
                } else if (SVC_CalibBpcCtrl.pBpcTable == NULL) {
                    RetVal = SVC_NG;
                    PRN_CAL_BPC_LOG "Fail to initial bpc dram shadow - bpc table should not null!" PRN_CAL_BPC_NG
                } else {
                    UINT32 Idx, DataOfs = 0xFFFFFFFFU, DataSize = 0U;

                    AmbaSvcWrap_MisraMemset(SVC_CalibBpcCtrl.pBpcTable, 0, TotalBpcTableSize);

                    SVC_CalibBpcCtrl.NumOfTable    = pCalObj->NumOfTable;
                    SVC_CalibBpcCtrl.pNandDataBase  = pNandDataBuf;
                    SVC_CalibBpcCtrl.pNandDataLimit = &(pCalObj->pShadowBuf[pCalObj->ShadowBufSize]);
                    SVC_CalibBpcCtrl.pNandDataFree  = pNandDataBuf;
                    for (Idx = 0U; Idx < SVC_CalibBpcCtrl.NumOfTable; Idx ++) {
                        if ((SVC_CalibBpcCtrl.pNandHeader->TableSelectBits & SvcCalib_BitGet(Idx)) > 0U) {
                            if (DataOfs == 0xFFFFFFFFU) {
                                DataOfs  = SVC_CalibBpcCtrl.pNandTable[Idx].DataOfs;
                                DataSize = SVC_CalibBpcCtrl.pNandTable[Idx].DataSize;
                            } else if (DataOfs < SVC_CalibBpcCtrl.pNandTable[Idx].DataOfs) {
                                DataOfs  = SVC_CalibBpcCtrl.pNandTable[Idx].DataOfs;
                                DataSize = SVC_CalibBpcCtrl.pNandTable[Idx].DataSize;
                            } else {
                                // Do nothing
                            }
                        }
                    }
                    if (DataOfs == 0xFFFFFFFFU) {
                        DataOfs = 0U;
                    }
                    SVC_CalibBpcCtrl.pNandDataFree = &(SVC_CalibBpcCtrl.pNandDataFree[DataOfs + DataSize]);

                    if (SVC_CalibBpcCtrl.pNandHeader->DebugMsgOn > 0U) {
                        SVC_CalibBpcCtrl.EnableDebugMsg |= SVC_CalibBpcCtrl.pNandHeader->DebugMsgOn;
                    }
                    if (SVC_CalibBpcCtrl.EnableDebugMsg > 0U) {

                        PRN_CAL_BPC_LOG " " PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "---- Bpc Nand Info ----"
                            PRN_CAL_BPC_ARG_UINT32 SVC_CalibBpcCtrl.pNandHeader->Reserved[0] PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "  Enable          : %d"
                            PRN_CAL_BPC_ARG_UINT32 SVC_CalibBpcCtrl.pNandHeader->Enable PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "  NumOfTable      : %d"
                            PRN_CAL_BPC_ARG_UINT32 SVC_CalibBpcCtrl.pNandHeader->NumOfTable PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "  DebugMsgOn      : %d"
                            PRN_CAL_BPC_ARG_UINT32 SVC_CalibBpcCtrl.pNandHeader->DebugMsgOn PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "  TblSelectBits   : 0x%x"
                            PRN_CAL_BPC_ARG_UINT32 SVC_CalibBpcCtrl.pNandHeader->TableSelectBits PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_API

                        PRN_CAL_BPC_LOG " " PRN_CAL_BPC_API
                        for (Idx = 0U; Idx < SVC_CalibBpcCtrl.NumOfTable; Idx ++) {
                            PRN_CAL_BPC_LOG "---- Bpc Nand Table[%d] 0x%08x ----"
                                PRN_CAL_BPC_ARG_UINT32 Idx                                 PRN_CAL_BPC_ARG_POST
                                PRN_CAL_BPC_ARG_CPOINT &(SVC_CalibBpcCtrl.pNandTable[Idx]) PRN_CAL_BPC_ARG_POST
                            PRN_CAL_BPC_API
                            PRN_CAL_BPC_LOG "  Header.VinSelectBits    : 0x%X"
                                PRN_CAL_BPC_ARG_UINT32 SVC_CalibBpcCtrl.pNandTable[Idx].Header.VinSelectBits PRN_CAL_BPC_ARG_POST     
                            PRN_CAL_BPC_API
                            PRN_CAL_BPC_LOG "  Header.SensorSelectBits : 0x%X"
                                PRN_CAL_BPC_ARG_UINT32 SVC_CalibBpcCtrl.pNandTable[Idx].Header.SensorSelectBits PRN_CAL_BPC_ARG_POST  
                            PRN_CAL_BPC_API
                            PRN_CAL_BPC_LOG "  Header.SensorModeBits   : 0x%X"
                                PRN_CAL_BPC_ARG_UINT32 SVC_CalibBpcCtrl.pNandTable[Idx].Header.SensorModeBits PRN_CAL_BPC_ARG_POST    
                            PRN_CAL_BPC_API
                            PRN_CAL_BPC_LOG "  DataType                : %d"
                                PRN_CAL_BPC_ARG_UINT32 SVC_CalibBpcCtrl.pNandTable[Idx].DataType PRN_CAL_BPC_ARG_POST                 
                            PRN_CAL_BPC_API
                            PRN_CAL_BPC_LOG "  DataOfs                 : 0x%X"
                                PRN_CAL_BPC_ARG_UINT32 SVC_CalibBpcCtrl.pNandTable[Idx].DataOfs PRN_CAL_BPC_ARG_POST                  
                            PRN_CAL_BPC_API
                            PRN_CAL_BPC_LOG "  DataSize                : 0x%X"
                                PRN_CAL_BPC_ARG_UINT32 SVC_CalibBpcCtrl.pNandTable[Idx].DataSize PRN_CAL_BPC_ARG_POST                 
                            PRN_CAL_BPC_API
                        }

                        PRN_CAL_BPC_LOG " " PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "---- Bpc Nand Data Info ----" PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "  DataBase        : %p"
                            PRN_CAL_BPC_ARG_CPOINT SVC_CalibBpcCtrl.pNandDataBase PRN_CAL_BPC_ARG_POST 
                        PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "  DataLimit       : %p"
                            PRN_CAL_BPC_ARG_CPOINT SVC_CalibBpcCtrl.pNandDataLimit PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "  DataFree        : %p"
                            PRN_CAL_BPC_ARG_CPOINT SVC_CalibBpcCtrl.pNandDataFree PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_API

                        PRN_CAL_BPC_LOG " " PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "---- Bpc Table Info ----" PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "  NumOfTable     : %d"
                            PRN_CAL_BPC_ARG_UINT32 SVC_CalibBpcCtrl.NumOfTable PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_API
                        for (Idx = 0U; Idx < SVC_CalibBpcCtrl.NumOfTable; Idx ++) {
                            PRN_CAL_BPC_LOG "  ---- Bpc Table[%d] %p -----"
                                PRN_CAL_BPC_ARG_UINT32 Idx                                PRN_CAL_BPC_ARG_POST
                                PRN_CAL_BPC_ARG_CPOINT &(SVC_CalibBpcCtrl.pBpcTable[Idx]) PRN_CAL_BPC_ARG_POST
                                PRN_CAL_BPC_ARG_CPOINT &(SVC_CalibBpcCtrl.pBpcTable[Idx]) PRN_CAL_BPC_ARG_POST
                            PRN_CAL_BPC_API
                            PRN_CAL_BPC_LOG "    VinSelectBits       : 0x%X"
                                PRN_CAL_BPC_ARG_UINT32 SVC_CalibBpcCtrl.pBpcTable[Idx].VinSelectBits PRN_CAL_BPC_ARG_POST      
                            PRN_CAL_BPC_API
                            PRN_CAL_BPC_LOG "    SensorSelectBits    : 0x%X"
                                PRN_CAL_BPC_ARG_UINT32 SVC_CalibBpcCtrl.pBpcTable[Idx].SensorSelectBits PRN_CAL_BPC_ARG_POST   
                            PRN_CAL_BPC_API
                        }
                    }

                    if (0U < SvcCalib_BpcMemAddrComp(SVC_CalibBpcCtrl.pNandDataFree, SVC_CalibBpcCtrl.pNandDataLimit)) {
                        RetVal = SVC_NG;
                        PRN_CAL_BPC_LOG "Fail to initial bpc dram shadow - invalid data base!" PRN_CAL_BPC_NG
                    } else {
                        SVC_CalibBpcCtrlFlag |= SVC_CALIB_BPC_FLG_SHADOW_INIT;
                        PRN_CAL_BPC_LOG "Successful to initial bpc dram shadow!" PRN_CAL_BPC_OK
                    }
                }

                SvcCalib_BpcMutexGive(&(SVC_CalibBpcCtrl.Mutex));
            }
        }
    }

    return RetVal;
}


static UINT32 SvcCalib_BpcTableUpdate(const SVC_CALIB_CHANNEL_s *pCalibChan, const AMBA_IK_MODE_CFG_s *pImgMode)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibBpcCtrlFlag & SVC_CALIB_BPC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to update bpc table - initial bpc module first!" PRN_CAL_BPC_NG
    } else if ((SVC_CalibBpcCtrlFlag & SVC_CALIB_BPC_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to update bpc table - initial bpc dram shadow first!" PRN_CAL_BPC_NG
    } else if (pCalibChan == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to update bpc table - invalid calib channel!" PRN_CAL_BPC_NG
    } else if (pImgMode == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to update bpc table - invalid image mode!" PRN_CAL_BPC_NG
    } else if (SVC_CalibBpcCtrl.pNandHeader == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to update bpc table - invalid nand table!" PRN_CAL_BPC_NG
    } else if ((SVC_CalibBpcCtrl.pNandHeader->Enable == 0U) ||
               (SVC_CalibBpcCtrl.pNandHeader->NumOfTable == 0U)) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "There is not bpc table in shadow buffer!" PRN_CAL_BPC_DBG
    } else {
        if (KAL_ERR_NONE != SvcCalib_BpcMutexTake(&(SVC_CalibBpcCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
            RetVal = SVC_NG;
            PRN_CAL_BPC_LOG "Fail to update bpc table - take mutex fail!" PRN_CAL_BPC_NG
        } else {
            SVC_CALIB_BPC_TABLE_s *pBpcTable = SvcCalib_BpcTableGet(pCalibChan);

            if (pBpcTable == NULL) {
                RetVal = SVC_NG;
                PRN_CAL_BPC_LOG "Fail to update bpc table - get bpc table fail" PRN_CAL_BPC_NG
            } else {
                AMBA_IK_STATIC_BAD_PXL_COR_s BpcData;

                AmbaSvcWrap_MisraMemset(&BpcData, 0, sizeof(BpcData));
                BpcData.VinSensorGeo.StartX               = pBpcTable->Table.CalibSensorGeo.StartX               ;
                BpcData.VinSensorGeo.StartY               = pBpcTable->Table.CalibSensorGeo.StartY               ;
                BpcData.VinSensorGeo.Width                = pBpcTable->Table.CalibSensorGeo.Width                ;
                BpcData.VinSensorGeo.Height               = pBpcTable->Table.CalibSensorGeo.Height               ;
                BpcData.VinSensorGeo.HSubSample.FactorDen = pBpcTable->Table.CalibSensorGeo.HSubSample.FactorDen ;
                BpcData.VinSensorGeo.HSubSample.FactorNum = pBpcTable->Table.CalibSensorGeo.HSubSample.FactorNum ;
                BpcData.VinSensorGeo.VSubSample.FactorDen = pBpcTable->Table.CalibSensorGeo.VSubSample.FactorDen ;
                BpcData.VinSensorGeo.VSubSample.FactorNum = pBpcTable->Table.CalibSensorGeo.VSubSample.FactorNum ;
                BpcData.CalibSbpInfo.Version              = pBpcTable->Table.Version;
                BpcData.CalibSbpInfo.pSbpBuffer           = pBpcTable->Table.SbpTbl;

                if (SVC_CalibBpcCtrl.EnableDebugMsg > 0U) {
                    PRN_CAL_BPC_LOG " " PRN_CAL_BPC_API
                    PRN_CAL_BPC_LOG "====== Calibration bpc info ======" PRN_CAL_BPC_API
                    PRN_CAL_BPC_LOG "  VinSelectBits                     : 0x%X"
                        PRN_CAL_BPC_ARG_UINT32 pBpcTable->VinSelectBits PRN_CAL_BPC_ARG_POST                        
                    PRN_CAL_BPC_API
                    PRN_CAL_BPC_LOG "  SensorSelectBits                  : 0x%X"
                        PRN_CAL_BPC_ARG_UINT32 pBpcTable->SensorSelectBits PRN_CAL_BPC_ARG_POST                     
                    PRN_CAL_BPC_API
                    PRN_CAL_BPC_LOG "  Version                           : 0x%X"
                        PRN_CAL_BPC_ARG_UINT32 BpcData.CalibSbpInfo.Version PRN_CAL_BPC_ARG_POST                    
                    PRN_CAL_BPC_API
                    PRN_CAL_BPC_LOG "  VinSensorGeo.StartX               : %d"
                        PRN_CAL_BPC_ARG_UINT32 BpcData.VinSensorGeo.StartX PRN_CAL_BPC_ARG_POST                     
                    PRN_CAL_BPC_API
                    PRN_CAL_BPC_LOG "  VinSensorGeo.StartY               : %d"
                        PRN_CAL_BPC_ARG_UINT32 BpcData.VinSensorGeo.StartY PRN_CAL_BPC_ARG_POST                     
                    PRN_CAL_BPC_API
                    PRN_CAL_BPC_LOG "  VinSensorGeo.Width                : %d"
                        PRN_CAL_BPC_ARG_UINT32 BpcData.VinSensorGeo.Width PRN_CAL_BPC_ARG_POST                      
                    PRN_CAL_BPC_API
                    PRN_CAL_BPC_LOG "  VinSensorGeo.Height               : %d"
                        PRN_CAL_BPC_ARG_UINT32 BpcData.VinSensorGeo.Height PRN_CAL_BPC_ARG_POST                     
                    PRN_CAL_BPC_API
                    PRN_CAL_BPC_LOG "  VinSensorGeo.HSubSample.FactorDen : %d"
                        PRN_CAL_BPC_ARG_UINT32 BpcData.VinSensorGeo.HSubSample.FactorDen PRN_CAL_BPC_ARG_POST       
                    PRN_CAL_BPC_API
                    PRN_CAL_BPC_LOG "  VinSensorGeo.HSubSample.FactorNum : %d"
                        PRN_CAL_BPC_ARG_UINT32 BpcData.VinSensorGeo.HSubSample.FactorNum PRN_CAL_BPC_ARG_POST       
                    PRN_CAL_BPC_API
                    PRN_CAL_BPC_LOG "  VinSensorGeo.VSubSample.FactorDen : %d"
                        PRN_CAL_BPC_ARG_UINT32 BpcData.VinSensorGeo.VSubSample.FactorDen PRN_CAL_BPC_ARG_POST       
                    PRN_CAL_BPC_API
                    PRN_CAL_BPC_LOG "  VinSensorGeo.VSubSample.FactorNum : %d"
                        PRN_CAL_BPC_ARG_UINT32 BpcData.VinSensorGeo.VSubSample.FactorNum PRN_CAL_BPC_ARG_POST       
                    PRN_CAL_BPC_API
                    PRN_CAL_BPC_LOG "  pSbpBuffer                        : %p"
                        PRN_CAL_BPC_ARG_CPOINT BpcData.CalibSbpInfo.pSbpBuffer PRN_CAL_BPC_ARG_POST
                    PRN_CAL_BPC_API
                }

                if (0U != AmbaIK_SetStaticBadPxlCor(pImgMode, &BpcData)) {
                    RetVal = SVC_NG;
                    PRN_CAL_BPC_LOG "Fail to update bpc table - set bpc info fail!" PRN_CAL_BPC_NG
                } else {
                    RetVal = AmbaIK_SetStaticBadPxlCorEnb(pImgMode, 1U);
                    if (RetVal != SVC_OK) {
                        PRN_CAL_BPC_LOG "Fail to update bpc table - enable warp fail!" PRN_CAL_BPC_NG
                    }
                }

                if (RetVal == SVC_OK) {

                    PRN_CAL_BPC_LOG "Successful to update the bpc table VinID(%d) SensorID(%d) ImgContextID(%d)"
                        PRN_CAL_BPC_ARG_UINT32 pCalibChan->VinID    PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_ARG_UINT32 pCalibChan->SensorID PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_ARG_UINT32 pImgMode->ContextId  PRN_CAL_BPC_ARG_POST
                    PRN_CAL_BPC_OK

                }
            }

            SvcCalib_BpcMutexGive(&(SVC_CalibBpcCtrl.Mutex));
        }
    }

    return RetVal;
}


static UINT32 SvcCalib_BpcTableCfg(UINT32 NandTableID, UINT32 TableID)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibBpcCtrlFlag & SVC_CALIB_BPC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to cfg bpc table - initial bpc module first!" PRN_CAL_BPC_NG
    } else if ((SVC_CalibBpcCtrlFlag & SVC_CALIB_BPC_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to cfg bpc table - initial bpc dram shadow first!" PRN_CAL_BPC_NG
    } else if ((SVC_CalibBpcCtrl.pBpcTable == NULL) ||
               (SVC_CalibBpcCtrl.pNandTable == NULL)) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to cfg bpc table - invalid table setting!" PRN_CAL_BPC_NG
    } else if (NandTableID >= SVC_CalibBpcCtrl.NumOfTable) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to cfg bpc table - input nand table id is out-of max table size!" PRN_CAL_BPC_NG
    } else if (TableID >= SVC_CalibBpcCtrl.NumOfTable) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to cfg bpc table - output bpc table id is out-of max table size!" PRN_CAL_BPC_NG
    } else if (SVC_CalibBpcCtrl.pNandDataBase == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to cfg bpc table - bpc nand data base should not null!" PRN_CAL_BPC_NG
    } else {
        const SVC_CALIB_BPC_NAND_TABLE_s *pSrcTable = &(SVC_CalibBpcCtrl.pNandTable[NandTableID]);
        SVC_CALIB_BPC_TABLE_s      *pDstTable = &(SVC_CalibBpcCtrl.pBpcTable[TableID]);
        UINT32 CopyTblData = 0U;
        UINT8 *pSrcData = &(SVC_CalibBpcCtrl.pNandDataBase[pSrcTable->DataOfs]);
        UINT32 SrcDataSize = pSrcTable->DataSize;

        AmbaSvcWrap_MisraMemset(pDstTable, 0, sizeof(SVC_CALIB_BPC_TABLE_s));

        if (pSrcTable->DataType != SVC_CALIB_CMPR_NONE) {

            SVC_CALIB_DECOMPRESS_s DeCmprCtrl;
            UINT32                 DeCmprSize = 0U;

            AmbaSvcWrap_MisraMemset(&DeCmprCtrl, 0, sizeof(DeCmprCtrl));
            DeCmprCtrl.CmprType        = pSrcTable->DataType;
            DeCmprCtrl.pDeCmprSize     = &DeCmprSize;
            DeCmprCtrl.DestinationSize = (UINT32)sizeof(pDstTable->Table.SbpTbl);
            DeCmprCtrl.pDestination    = pDstTable->Table.SbpTbl;
            DeCmprCtrl.SourceSize      = SrcDataSize;
            DeCmprCtrl.pSource         = pSrcData;

            if (0U != SvcCalib_DataDecompress(&DeCmprCtrl)) {
                CopyTblData = 1U;
                PRN_CAL_BPC_LOG "Decompress bpc table fail! Copy the compression data directly." PRN_CAL_BPC_API
            }
        } else {
            CopyTblData = 1U;
        }

        if (CopyTblData > 0U) {
            if (pSrcTable->DataSize > (UINT32)sizeof(pDstTable->Table.SbpTbl)) {
                RetVal = SVC_NG;
                PRN_CAL_BPC_LOG "Fail to get bpc table - output spb table is too small! 0x%X > 0x%X!"
                    PRN_CAL_BPC_ARG_UINT32 pSrcTable->DataSize             PRN_CAL_BPC_ARG_POST
                    PRN_CAL_BPC_ARG_UINT32 sizeof(pDstTable->Table.SbpTbl) PRN_CAL_BPC_ARG_POST
                PRN_CAL_BPC_NG
            } else {
                void *pDst;
                pDst = pDstTable->Table.SbpTbl;

                AmbaSvcWrap_MisraMemcpy(pDst, pSrcData, SrcDataSize);
            }
        }

        if (RetVal == SVC_OK) {
            pDstTable->VinSelectBits                             = pSrcTable->Header.VinSelectBits;
            pDstTable->SensorSelectBits                          = pSrcTable->Header.SensorSelectBits;
            pDstTable->SensorModeBits                            = pSrcTable->Header.SensorModeBits;
            pDstTable->Table.Version                             = SVC_CALIB_BPC_VERSION;
            pDstTable->Table.CalibSensorGeo.StartX               = pSrcTable->Header.VinSensorGeo.StartX               ;
            pDstTable->Table.CalibSensorGeo.StartY               = pSrcTable->Header.VinSensorGeo.StartY               ;
            pDstTable->Table.CalibSensorGeo.Width                = pSrcTable->Header.VinSensorGeo.Width                ;
            pDstTable->Table.CalibSensorGeo.Height               = pSrcTable->Header.VinSensorGeo.Height               ;
            pDstTable->Table.CalibSensorGeo.HSubSample.FactorDen = pSrcTable->Header.VinSensorGeo.HSubSample.FactorDen ;
            pDstTable->Table.CalibSensorGeo.HSubSample.FactorNum = pSrcTable->Header.VinSensorGeo.HSubSample.FactorNum ;
            pDstTable->Table.CalibSensorGeo.VSubSample.FactorDen = pSrcTable->Header.VinSensorGeo.VSubSample.FactorDen ;
            pDstTable->Table.CalibSensorGeo.VSubSample.FactorNum = pSrcTable->Header.VinSensorGeo.VSubSample.FactorNum ;
            pDstTable->Table.Count                               = pSrcTable->Header.BpcCount;
        }

        pDstTable->TableState.CfgDone = 1U;
    }

    return RetVal;
}


static SVC_CALIB_BPC_TABLE_s *SvcCalib_BpcTableGet(const SVC_CALIB_CHANNEL_s *pCalibChan)
{
    UINT32 RetVal = SVC_OK;
    SVC_CALIB_BPC_TABLE_s *pTable = NULL;

    if ((SVC_CalibBpcCtrlFlag & SVC_CALIB_BPC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to get bpc table - initial bpc control module first!" PRN_CAL_BPC_NG
    } else if ((SVC_CalibBpcCtrlFlag & SVC_CALIB_BPC_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to get bpc table - initial bpc dram shadow first!" PRN_CAL_BPC_NG
    } else if ((SVC_CalibBpcCtrl.pBpcTable == NULL) ||
               (SVC_CalibBpcCtrl.pNandHeader == NULL) ||
               (SVC_CalibBpcCtrl.pNandTable == NULL)) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to get bpc table - invalid bpc table setting!" PRN_CAL_BPC_NG
    } else if (SVC_CalibBpcCtrl.pNandHeader->NumOfTable == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to get bpc table - there is not table in nand" PRN_CAL_BPC_NG
    } else if (pCalibChan == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to get bpc table - invalid calib channel!" PRN_CAL_BPC_NG
    } else {
        UINT32 Idx;
        UINT32 TableIdx     = 0xFFFFFFFFU;
        UINT32 NandTableIdx = 0xFFFFFFFFU;

        RetVal  = SvcCalib_BpcTableSearch(pCalibChan->VinID, pCalibChan->SensorID, pCalibChan->ExtendData, &TableIdx);
        RetVal |= SvcCalib_BpcNandTableSearch(pCalibChan->VinID, pCalibChan->SensorID, pCalibChan->ExtendData, &NandTableIdx);
        if (RetVal == SVC_OK) {
            /* If system cannot get bpc table, need to configure it from nand table */
            if (TableIdx == 0xFFFFFFFFU) {
                if (NandTableIdx < SVC_CalibBpcCtrl.NumOfTable) {
                    /* Search the bpc free table */
                    for (Idx = 0; Idx < SVC_CalibBpcCtrl.NumOfTable; Idx ++) {
                        if ((SVC_CalibBpcCtrl.pBpcTable[Idx].VinSelectBits == 0U) &&
                            (SVC_CalibBpcCtrl.pBpcTable[Idx].SensorSelectBits == 0U)) {
                            TableIdx = Idx;
                            PRN_CAL_BPC_LOG "Successful to get bpc free table(%d)."
                                PRN_CAL_BPC_ARG_UINT32 TableIdx PRN_CAL_BPC_ARG_POST
                            PRN_CAL_BPC_DBG
                            break;
                        }
                    }

                    /* There is not free bpc table to service it */
                    if (TableIdx == 0xFFFFFFFFU) {
                        RetVal = SVC_NG;
                        PRN_CAL_BPC_LOG "Fail to get bpc table - there is not free table to support VinID(%d), SensorID(0x%X), SensorMode(%d)"
                            PRN_CAL_BPC_ARG_UINT32 pCalibChan->VinID    PRN_CAL_BPC_ARG_POST
                            PRN_CAL_BPC_ARG_UINT32 pCalibChan->SensorID PRN_CAL_BPC_ARG_POST
                            PRN_CAL_BPC_ARG_UINT32 pCalibChan->ExtendData PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_NG
                    }
                } else {
                    /* There is not nand table to support it. */
                    RetVal = SVC_NG;
                    PRN_CAL_BPC_LOG "There is not correct bpc table Current to support VinID(%d), SensorID(0x%X), SensorMode(%d)"
                        PRN_CAL_BPC_ARG_UINT32 pCalibChan->VinID      PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_ARG_UINT32 pCalibChan->SensorID   PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_ARG_UINT32 pCalibChan->ExtendData PRN_CAL_BPC_ARG_POST
                    PRN_CAL_BPC_DBG
                }
            }

            /* Re-configure bpc table */
            if ((TableIdx < SVC_CalibBpcCtrl.NumOfTable) && (NandTableIdx < SVC_CalibBpcCtrl.NumOfTable)) {
                RetVal = SvcCalib_BpcTableCfg(NandTableIdx, TableIdx);
                if (RetVal != SVC_OK) {
                    PRN_CAL_BPC_LOG "Fail to get bpc table - re-configure bpc table fail!" PRN_CAL_BPC_NG
                    pTable = NULL;
                } else {
                    PRN_CAL_BPC_LOG "Successful to get bpc table - configure done! %d -> %d"
                        PRN_CAL_BPC_ARG_UINT32 NandTableIdx PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_ARG_UINT32 TableIdx     PRN_CAL_BPC_ARG_POST
                    PRN_CAL_BPC_OK
                    pTable = &(SVC_CalibBpcCtrl.pBpcTable[TableIdx]);
                }
            } else {
                if (TableIdx < SVC_CalibBpcCtrl.NumOfTable) {
                    pTable = &(SVC_CalibBpcCtrl.pBpcTable[TableIdx]);
                    PRN_CAL_BPC_LOG "Successful to get bpc table - existed table!" PRN_CAL_BPC_OK
                }
            }
        }
    }

    AmbaMisra_TouchUnused(&RetVal);

    return pTable;
}


static UINT32 SvcCalib_BpcTableGenTbl(const SVC_CALIB_CHANNEL_s *pCalibChan, const char *pScriptPath)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibBpcCtrlFlag & SVC_CALIB_BPC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to calc bpc table - Need to initial bpc control module first!" PRN_CAL_BPC_NG
    } else if ((SVC_CalibBpcCtrlFlag & SVC_CALIB_BPC_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to calc bpc table - Need to initial bpc dram shadow first!" PRN_CAL_BPC_NG
    } else if (SVC_CalibBpcCtrl.pNandHeader == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to calc bpc table - invalid nand header!" PRN_CAL_BPC_NG
    } else if (SVC_CalibBpcCtrl.pNandTable == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to calc bpc table - invalid nand table!" PRN_CAL_BPC_NG
    } else if (pCalibChan == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to calc bpc table - invalid calib channel!" PRN_CAL_BPC_NG
    } else if (SVC_OK != SvcCalib_BpcVinSensorIDCheck(pCalibChan->VinID, pCalibChan->SensorID)) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to calc bpc table - invalid calib channel -> VinID(%d), SensorID(0x%x)!"
            PRN_CAL_BPC_ARG_UINT32 pCalibChan->VinID    PRN_CAL_BPC_ARG_POST
            PRN_CAL_BPC_ARG_UINT32 pCalibChan->SensorID PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_NG
    } else if (SVC_OK != SvcCalib_BpcCtnPreFetch(pScriptPath)) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to calc bpc table - try to fetch raw win from ctuner fail!" PRN_CAL_BPC_NG
    } else if (SVC_OK != SvcCalib_BpcMemLock()) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to calc bpc table - lock bpc memory fail!" PRN_CAL_BPC_NG
    } else if (KAL_ERR_NONE != SvcCalib_BpcMutexTake(&(SVC_CalibBpcCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to calc bpc table - take mutex fail!" PRN_CAL_BPC_NG
    } else {
        AMBA_CT_INITIAL_CONFIG_s CtnCfg;

        AmbaSvcWrap_MisraMemset(&CtnCfg, 0, sizeof(CtnCfg));
        CtnCfg.pTunerWorkingBuf    = SVC_CalibBpcCtrl.MemCtrl[SVC_CALIB_BPC_MEM_CALC_CTUNER].pBuf;
        CtnCfg.TunerWorkingBufSize = SVC_CalibBpcCtrl.MemCtrl[SVC_CALIB_BPC_MEM_CALC_CTUNER].BufSize;

        CtnCfg.Bpc.pCalibWorkingBuf    = SVC_CalibBpcCtrl.MemCtrl[SVC_CALIB_BPC_MEM_CALC].pBuf;
        CtnCfg.Bpc.CalibWorkingBufSize = SVC_CalibBpcCtrl.MemCtrl[SVC_CALIB_BPC_MEM_CALC].BufSize;

        PRetVal = AmbaCT_Init(AMBA_CT_TYPE_BPC, &CtnCfg);
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_CAL_BPC_LOG "Fail to calc bpc by ctuner - initial ctuner fail! 0x%08X"
                PRN_CAL_BPC_ARG_UINT32 PRetVal PRN_CAL_BPC_ARG_POST
            PRN_CAL_BPC_NG
        } else {
            PRetVal = AmbaCT_Load(pScriptPath);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_CAL_BPC_LOG "Fail to calc bpc by ctuner - load ctuner script fail! 0x%08X"
                    PRN_CAL_BPC_ARG_UINT32 PRetVal PRN_CAL_BPC_ARG_POST
                PRN_CAL_BPC_NG
            } else {
                PRetVal = AmbaCT_Execute();
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    PRN_CAL_BPC_LOG "Fail to calc bpc by ctuner - execute ctuner fail! 0x%08X"
                        PRN_CAL_BPC_ARG_UINT32 PRetVal PRN_CAL_BPC_ARG_POST
                    PRN_CAL_BPC_NG
                } else {
                    const AMBA_CAL_BPC_CALIB_DATA_V1_s *pData;
                    AMBA_CT_BPC_CALIB_DATA_s BpcData;

                    AmbaSvcWrap_MisraMemset(&BpcData, 0, sizeof(BpcData));

                    PRetVal = AmbaCT_BpcGetCalibData(&BpcData); PRN_CAL_BPC_ERR_HDLR

                    pData = BpcData.pCalibData;

                    if (pData == NULL) {
                        RetVal = SVC_NG;
                        PRN_CAL_BPC_LOG "Fail to calc bpc table - invalid ctuner result!" PRN_CAL_BPC_NG
                    } else {
                        UINT32 CurDataType = SVC_CalibBpcCtrl.CalcCtrl.DataType;
                        UINT8 *pDataBase = NULL;
                        UINT32 DataSize  = pData->SbpTblSize;

                        PRN_CAL_BPC_LOG " " PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "====== Ctuner BPC Info ======" PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "  Version    : 0x%08X"
                            PRN_CAL_BPC_ARG_UINT32 pData->Version PRN_CAL_BPC_ARG_POST    
                        PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "  Count      : %d"
                            PRN_CAL_BPC_ARG_UINT32 pData->Count PRN_CAL_BPC_ARG_POST       
                        PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "  SbpTbl     : %p"
                            PRN_CAL_BPC_ARG_CPOINT pData->pSbpTbl PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "  SbpTblSize : 0x%X"
                            PRN_CAL_BPC_ARG_UINT32 pData->SbpTblSize PRN_CAL_BPC_ARG_POST 
                        PRN_CAL_BPC_API

                        pDataBase = pData->pSbpTbl;

                        if (CurDataType != SVC_CALIB_CMPR_NONE) {
                            UINT32 CmprSize = 0U;
                            SVC_CALIB_COMPRESS_s CmprCtrl;

                            AmbaSvcWrap_MisraMemset(&CmprCtrl, 0, sizeof(CmprCtrl));
                            CmprCtrl.CmprType        = CurDataType;
                            CmprCtrl.pCmprSize       = &CmprSize;
                            CmprCtrl.pSource         = pData->pSbpTbl;
                            CmprCtrl.SourceSize      = pData->SbpTblSize;
                            CmprCtrl.DestinationSize = SVC_CalibBpcCtrl.MemCtrl[SVC_CALIB_BPC_MEM_CALC].BufSize;
                            CmprCtrl.pDestination    = SVC_CalibBpcCtrl.MemCtrl[SVC_CALIB_BPC_MEM_CALC].pBuf;
                            CmprCtrl.Lz77.SearchWin  = SVC_CalibBpcCtrl.CalcCtrl.Lz77SearchWin;

                            if (0U != SvcCalib_DataCompress(&CmprCtrl)) {
                                CurDataType = SVC_CALIB_CMPR_NONE;
                                PRN_CAL_BPC_LOG "Fail to calc bpc table - compress data fail! Write the data directly!" PRN_CAL_BPC_NG
                                AmbaMisra_TouchUnused(pDataBase);
                            } else if (pData->SbpTblSize <= CmprSize) {
                                CurDataType = SVC_CALIB_CMPR_NONE;
                                PRN_CAL_BPC_LOG "lz77 compress result larger than dst buf size. Write the data directly!" PRN_CAL_BPC_API
                            } else {
                                pDataBase = SVC_CalibBpcCtrl.MemCtrl[SVC_CALIB_BPC_MEM_CALC].pBuf;
                                DataSize  = CmprSize;
                            }
                        }

                        if (0U < SvcCalib_BpcMemAddrComp(&(SVC_CalibBpcCtrl.pNandDataFree[DataSize]), SVC_CalibBpcCtrl.pNandDataLimit)) {
                            RetVal = SVC_NG;
                            PRN_CAL_BPC_LOG "Fail to calc bpc table - free data nand size does not enough to service it!" PRN_CAL_BPC_NG
                            PRN_CAL_BPC_LOG "  DataSize      : 0x%X"
                                PRN_CAL_BPC_ARG_UINT32 DataSize PRN_CAL_BPC_ARG_POST
                            PRN_CAL_BPC_NG
                            PRN_CAL_BPC_LOG "  NandDataFree  : %p"
                                PRN_CAL_BPC_ARG_CPOINT SVC_CalibBpcCtrl.pNandDataFree PRN_CAL_BPC_ARG_POST
                            PRN_CAL_BPC_NG
                            PRN_CAL_BPC_LOG "  NandDataLimit : %p"
                                PRN_CAL_BPC_ARG_CPOINT SVC_CalibBpcCtrl.pNandDataLimit PRN_CAL_BPC_ARG_POST
                            PRN_CAL_BPC_NG
                        } else if (SVC_CalibBpcCtrl.CalcCtrl.TableIdx >= SVC_CALIB_MAX_TABLE_NUM) {
                            RetVal = SVC_NG;
                            PRN_CAL_BPC_LOG "Fail to calc bpc table - table id(%d) is out-of range"
                                PRN_CAL_BPC_ARG_UINT32 SVC_CalibBpcCtrl.CalcCtrl.TableIdx PRN_CAL_BPC_ARG_POST
                            PRN_CAL_BPC_NG
                        } else {
                            SVC_CALIB_BPC_NAND_HEADER_s *pNandHeader = SVC_CalibBpcCtrl.pNandHeader;
                            SVC_CALIB_BPC_NAND_TABLE_s  *pNandTbl    = &(SVC_CalibBpcCtrl.pNandTable[SVC_CalibBpcCtrl.CalcCtrl.TableIdx]);

                            AmbaSvcWrap_MisraMemcpy(SVC_CalibBpcCtrl.pNandDataFree, pDataBase, DataSize);

                            pNandTbl->Header.VinSelectBits                     = pCalibChan->VinSelectBits            ;
                            pNandTbl->Header.SensorSelectBits                  = pCalibChan->SensorSelectBits         ;
                            pNandTbl->Header.SensorModeBits                    = pCalibChan->ExtendData               ;
                            pNandTbl->Header.Version                           = SVC_CALIB_BPC_VERSION                ;
                            pNandTbl->Header.VinSensorGeo.StartX               = pData->CalibSensorGeo.StartX              ;
                            pNandTbl->Header.VinSensorGeo.StartY               = pData->CalibSensorGeo.StartY              ;
                            pNandTbl->Header.VinSensorGeo.Width                = pData->CalibSensorGeo.Width               ;
                            pNandTbl->Header.VinSensorGeo.Height               = pData->CalibSensorGeo.Height              ;
                            pNandTbl->Header.VinSensorGeo.HSubSample.FactorDen = pData->CalibSensorGeo.HSubSample.FactorDen;
                            pNandTbl->Header.VinSensorGeo.HSubSample.FactorNum = pData->CalibSensorGeo.HSubSample.FactorNum;
                            pNandTbl->Header.VinSensorGeo.VSubSample.FactorDen = pData->CalibSensorGeo.VSubSample.FactorDen;
                            pNandTbl->Header.VinSensorGeo.VSubSample.FactorNum = pData->CalibSensorGeo.VSubSample.FactorNum;
                            pNandTbl->Header.BpcCount                          = pData->Count                         ;
                            pNandTbl->DataType                                 = CurDataType                          ;
                            pNandTbl->DataSize                                 = DataSize                             ;
                            pNandTbl->DataOfs                                  = SvcCalib_BpcMemAddrDiff(SVC_CalibBpcCtrl.pNandDataFree, SVC_CalibBpcCtrl.pNandDataBase);

                            PRN_CAL_BPC_LOG " " PRN_CAL_BPC_API
                            PRN_CAL_BPC_LOG "====== Calibration bpc info ====== 0x%x"
                                PRN_CAL_BPC_ARG_UINT32 pData->Version PRN_CAL_BPC_ARG_POST
                            PRN_CAL_BPC_API
                            PRN_CAL_BPC_LOG "  pNandTbl->Header.VinSelectBits                     : 0x%X"
                                PRN_CAL_BPC_ARG_UINT32 pNandTbl->Header.VinSelectBits PRN_CAL_BPC_ARG_POST                   
                            PRN_CAL_BPC_API
                            PRN_CAL_BPC_LOG "  pNandTbl->Header.SensorSelectBits                  : 0x%X"
                                PRN_CAL_BPC_ARG_UINT32 pNandTbl->Header.SensorSelectBits PRN_CAL_BPC_ARG_POST                
                            PRN_CAL_BPC_API
                            PRN_CAL_BPC_LOG "  pNandTbl->Header.SensorModeBits                    : 0x%X"
                                PRN_CAL_BPC_ARG_UINT32 pNandTbl->Header.SensorModeBits PRN_CAL_BPC_ARG_POST                  
                            PRN_CAL_BPC_API
                            PRN_CAL_BPC_LOG "  pNandTbl->Header.Version                           : 0x%X"
                                PRN_CAL_BPC_ARG_UINT32 pNandTbl->Header.Version PRN_CAL_BPC_ARG_POST                         
                            PRN_CAL_BPC_API
                            PRN_CAL_BPC_LOG "  pNandTbl->Header.VinSensorGeo.StartX               : %d"
                                PRN_CAL_BPC_ARG_UINT32 pNandTbl->Header.VinSensorGeo.StartX PRN_CAL_BPC_ARG_POST             
                            PRN_CAL_BPC_API
                            PRN_CAL_BPC_LOG "  pNandTbl->Header.VinSensorGeo.StartY               : %d"
                                PRN_CAL_BPC_ARG_UINT32 pNandTbl->Header.VinSensorGeo.StartY PRN_CAL_BPC_ARG_POST             
                            PRN_CAL_BPC_API
                            PRN_CAL_BPC_LOG "  pNandTbl->Header.VinSensorGeo.Width                : %d"
                                PRN_CAL_BPC_ARG_UINT32 pNandTbl->Header.VinSensorGeo.Width PRN_CAL_BPC_ARG_POST              
                            PRN_CAL_BPC_API
                            PRN_CAL_BPC_LOG "  pNandTbl->Header.VinSensorGeo.Height               : %d"
                                PRN_CAL_BPC_ARG_UINT32 pNandTbl->Header.VinSensorGeo.Height PRN_CAL_BPC_ARG_POST             
                            PRN_CAL_BPC_API
                            PRN_CAL_BPC_LOG "  pNandTbl->Header.VinSensorGeo.HSubSample.FactorDen : 0x%X"
                                PRN_CAL_BPC_ARG_UINT32 pNandTbl->Header.VinSensorGeo.HSubSample.FactorDen PRN_CAL_BPC_ARG_POST
                            PRN_CAL_BPC_API
                            PRN_CAL_BPC_LOG "  pNandTbl->Header.VinSensorGeo.HSubSample.FactorNum : 0x%X"
                                PRN_CAL_BPC_ARG_UINT32 pNandTbl->Header.VinSensorGeo.HSubSample.FactorNum PRN_CAL_BPC_ARG_POST
                            PRN_CAL_BPC_API
                            PRN_CAL_BPC_LOG "  pNandTbl->Header.VinSensorGeo.VSubSample.FactorDen : 0x%X"
                                PRN_CAL_BPC_ARG_UINT32 pNandTbl->Header.VinSensorGeo.VSubSample.FactorDen PRN_CAL_BPC_ARG_POST
                            PRN_CAL_BPC_API
                            PRN_CAL_BPC_LOG "  pNandTbl->Header.VinSensorGeo.VSubSample.FactorNum : 0x%X"
                                PRN_CAL_BPC_ARG_UINT32 pNandTbl->Header.VinSensorGeo.VSubSample.FactorNum PRN_CAL_BPC_ARG_POST
                            PRN_CAL_BPC_API
                            PRN_CAL_BPC_LOG "  pNandTbl->Header.BpcCount                          : %d"
                                PRN_CAL_BPC_ARG_UINT32 pNandTbl->Header.BpcCount PRN_CAL_BPC_ARG_POST                        
                            PRN_CAL_BPC_API
                            PRN_CAL_BPC_LOG "  pNandTbl->DataType                                 : 0x%X"
                                PRN_CAL_BPC_ARG_UINT32 pNandTbl->DataType PRN_CAL_BPC_ARG_POST                               
                            PRN_CAL_BPC_API
                            PRN_CAL_BPC_LOG "  pNandTbl->DataSize                                 : 0x%X"
                                PRN_CAL_BPC_ARG_UINT32 pNandTbl->DataSize PRN_CAL_BPC_ARG_POST                               
                            PRN_CAL_BPC_API
                            PRN_CAL_BPC_LOG "  pNandTbl->DataOfs                                  : 0x%X"
                                PRN_CAL_BPC_ARG_UINT32 pNandTbl->DataOfs PRN_CAL_BPC_ARG_POST                                
                            PRN_CAL_BPC_API

                            // Update nand header
                            if (pNandHeader->Enable == 0U) {
                                pNandHeader->Enable = 1U;
                            }
                            if (pNandHeader->DebugMsgOn == 0U) {
                                pNandHeader->DebugMsgOn = SVC_CalibBpcCtrl.EnableDebugMsg;
                            }
                            if ((pNandHeader->TableSelectBits & SvcCalib_BitGet(SVC_CalibBpcCtrl.CalcCtrl.TableIdx)) == 0U) {
                                pNandHeader->TableSelectBits |= SvcCalib_BitGet(SVC_CalibBpcCtrl.CalcCtrl.TableIdx);
                                pNandHeader->NumOfTable += 1U;
                            }

                            /* Update to NAND */
                            RetVal = SvcCalib_DataSave(SVC_CALIB_BPC_ID);
                            if (RetVal != OK) {
                                PRN_CAL_BPC_LOG "Fail to save bpc to nand!" PRN_CAL_BPC_NG
                            } else {
                                SVC_CalibBpcCtrl.pNandDataFree = &(SVC_CalibBpcCtrl.pNandDataFree[DataSize]);
                            }
                        }
                    }
                }
            }
        }

        SvcCalib_BpcMutexGive(&(SVC_CalibBpcCtrl.Mutex));

        SvcCalib_BpcMemUnLock();
    }

    return RetVal;
}

static UINT32 SvcCalib_BpcTableSearch(UINT32 VinID, UINT32 SensorID, UINT32 SensorMode, UINT32 *pTblIdx)
{
    UINT32 RetVal = SVC_OK;

    if (SVC_OK != SvcCalib_BpcVinSensorIDCheck(VinID, SensorID)) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to search bpc table - invalid calib channel -> VinID(%d), SensorID(%d)!"
            PRN_CAL_BPC_ARG_UINT32 VinID    PRN_CAL_BPC_ARG_POST
            PRN_CAL_BPC_ARG_UINT32 SensorID PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_NG
    } else if (SensorMode >= 32U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to search bpc table - SensorMode(%d) out-of range"
            PRN_CAL_BPC_ARG_UINT32 SensorMode PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_NG
    } else {
        UINT32 Idx;

        if (pTblIdx != NULL) {
            *pTblIdx = 0xFFFFFFFFU;
        }

        for (Idx = 0U; Idx < SVC_CalibBpcCtrl.NumOfTable; Idx ++) {
            if (((SVC_CalibBpcCtrl.pBpcTable[Idx].VinSelectBits & SvcCalib_BitGet(VinID)) > 0U) &&
                ((SVC_CalibBpcCtrl.pBpcTable[Idx].SensorSelectBits & SensorID) > 0U) &&
                ((SVC_CalibBpcCtrl.pBpcTable[Idx].SensorModeBits & SvcCalib_BitGet(SensorMode)) > 0U)) {
                if (pTblIdx != NULL) {
                    *pTblIdx = Idx;
                }
                break;
            }
        }
    }

    return RetVal;
}

static UINT32 SvcCalib_BpcNandTableSearch(UINT32 VinID, UINT32 SensorID, UINT32 SensorMode, UINT32 *pTblIdx)
{
    UINT32 RetVal = SVC_OK;

    if (SVC_OK != SvcCalib_BpcVinSensorIDCheck(VinID, SensorID)) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to search bpc nand table - invalid calib channel -> VinID(%d), SensorID(%d)!"
            PRN_CAL_BPC_ARG_UINT32 VinID    PRN_CAL_BPC_ARG_POST
            PRN_CAL_BPC_ARG_UINT32 SensorID PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_NG
    } else if (SensorMode >= 32U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to search bpc nand table - SensorMode(%d) is out-of range"
            PRN_CAL_BPC_ARG_UINT32 SensorMode PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_NG
    } else {
        UINT32 Idx;

        if (pTblIdx != NULL) {
            *pTblIdx = 0xFFFFFFFFU;
        }

        for (Idx = 0U; Idx < SVC_CalibBpcCtrl.NumOfTable; Idx ++) {
            if ((SVC_CalibBpcCtrl.pNandHeader->TableSelectBits & SvcCalib_BitGet(Idx)) > 0U) {
                if (((SVC_CalibBpcCtrl.pNandTable[Idx].Header.VinSelectBits & SvcCalib_BitGet(VinID)) > 0U) &&
                    ((SVC_CalibBpcCtrl.pNandTable[Idx].Header.SensorSelectBits & SensorID) > 0U) &&
                    ((SVC_CalibBpcCtrl.pNandTable[Idx].Header.SensorModeBits & SvcCalib_BitGet(SensorMode)) > 0U)) {
                    if (pTblIdx != NULL) {
                        *pTblIdx = Idx;
                    }
                    break;
                }
            }
        }
    }

    return RetVal;
}

static UINT32 SvcCalib_BpcTableDataGet(UINT32 TblID, SVC_CALIB_TBL_INFO_s *pTblInfo)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibBpcCtrlFlag & SVC_CALIB_BPC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to get bpc table - initial bpc module first!" PRN_CAL_BPC_NG
    } else if ((SVC_CalibBpcCtrlFlag & SVC_CALIB_BPC_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to get bpc table - initial bpc dram shadow first!" PRN_CAL_BPC_NG
    } else if (SVC_CalibBpcCtrl.pNandHeader->NumOfTable == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "There is not bpc table!" PRN_CAL_BPC_NG
    } else if (pTblInfo == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to get bpc table - output table info should not null!" PRN_CAL_BPC_NG
    } else if (pTblInfo->pBuf == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to get bpc table - output table buffer should not null!" PRN_CAL_BPC_NG
    } else if (pTblInfo->BufSize < (UINT32)sizeof(SVC_CALIB_BPC_TBL_DATA_s)) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to get bpc table - output table buffer size is too small! 0x%X/0x%X"
            PRN_CAL_BPC_ARG_UINT32 pTblInfo->BufSize                PRN_CAL_BPC_ARG_POST
            PRN_CAL_BPC_ARG_UINT32 sizeof(SVC_CALIB_BPC_TBL_DATA_s) PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_NG
    } else {
        if (KAL_ERR_NONE != SvcCalib_BpcMutexTake(&(SVC_CalibBpcCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
            RetVal = SVC_NG;
            PRN_CAL_BPC_LOG "Fail to get bpc table - take mutex fail!" PRN_CAL_BPC_NG
        } else {
            UINT32 CurTblID = 0xFFFFFFFFU;
            SVC_CALIB_BPC_TBL_DATA_s         *pOutput = NULL;
            const SVC_CALIB_BPC_NAND_TABLE_s *pNandTbl;
            if (TblID == 255U) {
                if (0U != SvcCalib_BpcNandTableSearch(pTblInfo->CalChan.VinID, pTblInfo->CalChan.SensorID, pTblInfo->CalChan.ExtendData, &CurTblID)) {
                    CurTblID = 0xFFFFFFFFU;
                }
            } else {
                CurTblID = TblID;
            }

            if (CurTblID >= SVC_CalibBpcCtrl.NumOfTable) {
                RetVal = SVC_NG;

                PRetVal = (SVC_CalibBpcCtrl.NumOfTable == 0U)?0U:(SVC_CalibBpcCtrl.NumOfTable - 1U);
                PRN_CAL_BPC_LOG "Fail to get bpc table - the TableID(0x%X) is out-of range(0 ~ %d)"
                    PRN_CAL_BPC_ARG_UINT32 CurTblID PRN_CAL_BPC_ARG_POST
                    PRN_CAL_BPC_ARG_UINT32 PRetVal  PRN_CAL_BPC_ARG_POST
                PRN_CAL_BPC_NG
            } else {
                pNandTbl = &(SVC_CalibBpcCtrl.pNandTable[CurTblID]);

                AmbaMisra_TypeCast(&(pOutput), &(pTblInfo->pBuf));

                if (pOutput == NULL) {
                    RetVal = SVC_NG;
                } else if (pOutput->pSbpTbl == NULL) {
                    RetVal = SVC_NG;
                    PRN_CAL_BPC_LOG "Fail to get bpc table - invalid sbp buffer!" PRN_CAL_BPC_NG
                } else {
                    UINT32 CopyTblData = 0U;
                    UINT8 *pSrcData = &(SVC_CalibBpcCtrl.pNandDataBase[pNandTbl->DataOfs]);
                    UINT32 SrcDataSize = pNandTbl->DataSize;

                    if (pNandTbl->DataType != SVC_CALIB_CMPR_NONE) {
                        SVC_CALIB_DECOMPRESS_s DeCmprCtrl;
                        UINT32                 DeCmprSize = 0U;

                        AmbaSvcWrap_MisraMemset(&DeCmprCtrl, 0, sizeof(DeCmprCtrl));
                        DeCmprCtrl.CmprType        = pNandTbl->DataType;
                        DeCmprCtrl.pDeCmprSize     = &DeCmprSize;
                        DeCmprCtrl.DestinationSize = pOutput->SbpTblSize;
                        DeCmprCtrl.pDestination    = pOutput->pSbpTbl;
                        DeCmprCtrl.SourceSize      = SrcDataSize;
                        DeCmprCtrl.pSource         = pSrcData;

                        if (0U != SvcCalib_DataDecompress(&DeCmprCtrl)) {
                            CopyTblData = 1U;
                            pOutput->DataType = pNandTbl->DataType;
                            PRN_CAL_BPC_LOG "Decompress bpc table fail! Copy the compression data directly." PRN_CAL_BPC_API
                        } else {
                            pOutput->DataType = SVC_CALIB_CMPR_NONE;
                        }
                    } else {
                        CopyTblData = 1U;
                        pOutput->DataType = SVC_CALIB_CMPR_NONE;
                    }

                    if (CopyTblData > 0U) {
                        if (pNandTbl->DataSize > pOutput->SbpTblSize) {
                            RetVal = SVC_NG;
                            PRN_CAL_BPC_LOG "Fail to get bpc table - output spb table is too small! 0x%X > 0x%X!"
                                PRN_CAL_BPC_ARG_UINT32 pNandTbl->DataSize  PRN_CAL_BPC_ARG_POST
                                PRN_CAL_BPC_ARG_UINT32 pOutput->SbpTblSize PRN_CAL_BPC_ARG_POST
                            PRN_CAL_BPC_NG
                        } else {

                            AmbaSvcWrap_MisraMemcpy(pOutput->pSbpTbl, pSrcData, SrcDataSize);
                        }
                    }

                    if (RetVal == SVC_OK) {
                        pOutput->Version = pNandTbl->Header.Version;
                        pOutput->CalibGeo.StartX               = pNandTbl->Header.VinSensorGeo.StartX              ;
                        pOutput->CalibGeo.StartY               = pNandTbl->Header.VinSensorGeo.StartY              ;
                        pOutput->CalibGeo.Width                = pNandTbl->Header.VinSensorGeo.Width               ;
                        pOutput->CalibGeo.Height               = pNandTbl->Header.VinSensorGeo.Height              ;
                        pOutput->CalibGeo.HSubSample.FactorDen = pNandTbl->Header.VinSensorGeo.HSubSample.FactorDen;
                        pOutput->CalibGeo.HSubSample.FactorNum = pNandTbl->Header.VinSensorGeo.HSubSample.FactorNum;
                        pOutput->CalibGeo.VSubSample.FactorDen = pNandTbl->Header.VinSensorGeo.VSubSample.FactorDen;
                        pOutput->CalibGeo.VSubSample.FactorNum = pNandTbl->Header.VinSensorGeo.VSubSample.FactorNum;
                        pOutput->SbpCount                      = pNandTbl->Header.BpcCount;
                    }
                }
            }

            SvcCalib_BpcMutexGive(&(SVC_CalibBpcCtrl.Mutex));

            AmbaMisra_TouchUnused(pTblInfo);
        }
    }

    return RetVal;
}

static UINT32 SvcCalib_BpcTableDataSet(UINT32 TblID, SVC_CALIB_TBL_INFO_s *pTblInfo)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibBpcCtrlFlag & SVC_CALIB_BPC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to set bpc table - initial bpc module first!" PRN_CAL_BPC_NG
    } else if ((SVC_CalibBpcCtrlFlag & SVC_CALIB_BPC_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to set bpc table - initial bpc dram shadow first!" PRN_CAL_BPC_NG
    } else if (SVC_CalibBpcCtrl.pNandHeader == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to set bpc table - invalid nand header!" PRN_CAL_BPC_NG
    } else if (SVC_CalibBpcCtrl.pNandTable == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to set bpc table - invalid nand table!" PRN_CAL_BPC_NG
    } else if (TblID >= SVC_CalibBpcCtrl.NumOfTable) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to set bpc table - TableID(%d) out-of range(%d)!"
            PRN_CAL_BPC_ARG_UINT32 TblID                       PRN_CAL_BPC_ARG_POST
            PRN_CAL_BPC_ARG_UINT32 SVC_CalibBpcCtrl.NumOfTable PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_NG
    } else if (pTblInfo == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to set bpc table - output table info should not null!" PRN_CAL_BPC_NG
    } else if (pTblInfo->pBuf == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to set bpc table - input table buffer should not null!" PRN_CAL_BPC_NG
    } else if (pTblInfo->BufSize < (UINT32)sizeof(SVC_CALIB_BPC_TBL_DATA_s)) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to set bpc table - input table buffer size is too small! 0x%X/0x%X"
            PRN_CAL_BPC_ARG_UINT32 pTblInfo->BufSize                PRN_CAL_BPC_ARG_POST
            PRN_CAL_BPC_ARG_UINT32 sizeof(SVC_CALIB_BPC_TBL_DATA_s) PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_NG
    } else if (pTblInfo->CalChan.ExtendData >= 32U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to set bpc table - input sensor mode(%d) is out-of range"
            PRN_CAL_BPC_ARG_UINT32 pTblInfo->CalChan.ExtendData PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_NG
    } else {

        if (KAL_ERR_NONE != SvcCalib_BpcMutexTake(&(SVC_CalibBpcCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
            RetVal = SVC_NG;
            PRN_CAL_BPC_LOG "Fail to set bpc table - take mutex fail!" PRN_CAL_BPC_NG
        } else {
            SVC_CALIB_BPC_TBL_DATA_s    *pInput = NULL;
            SVC_CALIB_BPC_NAND_TABLE_s  *pNandTbl    = &(SVC_CalibBpcCtrl.pNandTable[TblID]);
            SVC_CALIB_BPC_NAND_HEADER_s *pNandHeader = SVC_CalibBpcCtrl.pNandHeader;

            AmbaMisra_TypeCast(&(pInput), &(pTblInfo->pBuf));

            if (pInput == NULL) {
                RetVal = SVC_NG;
                PRN_CAL_BPC_LOG "Fail to set bpc table - input bpc table data should not null!" PRN_CAL_BPC_NG
            } else if (pInput->pSbpTbl == NULL) {
                RetVal = SVC_NG;
                PRN_CAL_BPC_LOG "Fail to set bpc table - input bpc data should not null!" PRN_CAL_BPC_NG
            } else {
                UINT32 CurDataType = pInput->DataType;
                UINT32 NeedFreeMem = 0U;
                UINT8 *pData    = pInput->pSbpTbl;
                UINT32 DataSize = pInput->SbpTblSize;

                if (CurDataType != SVC_CALIB_CMPR_NONE) {
                    if (0U != SvcCalib_BpcMemLock()) {
                        CurDataType = SVC_CALIB_CMPR_NONE;
                        PRN_CAL_BPC_LOG "Fail to set bpc table - lock memory fail! Write the data directly!" PRN_CAL_BPC_NG
                        AmbaMisra_TouchUnused(pData);
                    } else {
                        UINT32 CmprSize = 0U;
                        SVC_CALIB_COMPRESS_s CmprCtrl;

                        NeedFreeMem = 1U;

                        AmbaSvcWrap_MisraMemset(&CmprCtrl, 0, sizeof(CmprCtrl));
                        CmprCtrl.CmprType = CurDataType;
                        CmprCtrl.pDestination = &CmprSize;
                        CmprCtrl.pSource      = pInput->pSbpTbl;
                        CmprCtrl.SourceSize   = pInput->SbpTblSize;
                        CmprCtrl.pDestination = SVC_CalibBpcCtrl.MemCtrl[SVC_CALIB_BPC_MEM_CALC].pBuf;
                        CmprCtrl.DestinationSize = SVC_CalibBpcCtrl.MemCtrl[SVC_CALIB_BPC_MEM_CALC].BufSize;
                        CmprCtrl.Lz77.SearchWin = SVC_CalibBpcCtrl.CalcCtrl.Lz77SearchWin;

                        if (0U != SvcCalib_DataCompress(&CmprCtrl)) {
                            CurDataType = SVC_CALIB_CMPR_NONE;
                            PRN_CAL_BPC_LOG "Fail to set bpc table - compress data fail! Write the data directly!" PRN_CAL_BPC_NG
                        } else {
                            pData    = SVC_CalibBpcCtrl.MemCtrl[SVC_CALIB_BPC_MEM_CALC].pBuf;
                            DataSize = CmprSize;
                        }
                    }
                }

                if (0U < SvcCalib_BpcMemAddrComp(&(SVC_CalibBpcCtrl.pNandDataFree[DataSize]), SVC_CalibBpcCtrl.pNandDataLimit)) {
                    RetVal = SVC_NG;
                    PRN_CAL_BPC_LOG "Fail to set bpc table - free data nand size does not enough to service it!" PRN_CAL_BPC_NG
                    PRN_CAL_BPC_LOG "  DataSize      : 0x%X"
                        PRN_CAL_BPC_ARG_UINT32 DataSize PRN_CAL_BPC_ARG_POST
                    PRN_CAL_BPC_NG
                    PRN_CAL_BPC_LOG "  NandDataFree  : %p"
                        PRN_CAL_BPC_ARG_CPOINT SVC_CalibBpcCtrl.pNandDataFree PRN_CAL_BPC_ARG_POST
                    PRN_CAL_BPC_NG
                    PRN_CAL_BPC_LOG "  NandDataLimit : %p"
                        PRN_CAL_BPC_ARG_CPOINT SVC_CalibBpcCtrl.pNandDataLimit PRN_CAL_BPC_ARG_POST
                    PRN_CAL_BPC_NG
                } else {

                    AmbaSvcWrap_MisraMemcpy(SVC_CalibBpcCtrl.pNandDataFree, pData, DataSize);

                    pNandTbl->Header.VinSelectBits                     = pTblInfo->CalChan.VinSelectBits         ;
                    pNandTbl->Header.SensorSelectBits                  = pTblInfo->CalChan.SensorSelectBits      ;
                    pNandTbl->Header.SensorModeBits                    = SvcCalib_BitGet(pTblInfo->CalChan.ExtendData );
                    pNandTbl->Header.Version                           = SVC_CALIB_BPC_VERSION                   ;
                    pNandTbl->Header.VinSensorGeo.StartX               = pInput->CalibGeo.StartX                 ;
                    pNandTbl->Header.VinSensorGeo.StartY               = pInput->CalibGeo.StartY                 ;
                    pNandTbl->Header.VinSensorGeo.Width                = pInput->CalibGeo.Width                  ;
                    pNandTbl->Header.VinSensorGeo.Height               = pInput->CalibGeo.Height                 ;
                    pNandTbl->Header.VinSensorGeo.HSubSample.FactorDen = pInput->CalibGeo.HSubSample.FactorDen   ;
                    pNandTbl->Header.VinSensorGeo.HSubSample.FactorNum = pInput->CalibGeo.HSubSample.FactorNum   ;
                    pNandTbl->Header.VinSensorGeo.VSubSample.FactorDen = pInput->CalibGeo.VSubSample.FactorDen   ;
                    pNandTbl->Header.VinSensorGeo.VSubSample.FactorNum = pInput->CalibGeo.VSubSample.FactorNum   ;
                    pNandTbl->Header.BpcCount                          = pInput->SbpCount                        ;
                    pNandTbl->DataType                                 = CurDataType                             ;
                    pNandTbl->DataSize                                 = DataSize                                ;
                    pNandTbl->DataOfs                                  = SvcCalib_BpcMemAddrDiff(SVC_CalibBpcCtrl.pNandDataFree, SVC_CalibBpcCtrl.pNandDataBase);

                    // Update nand header
                    if (pNandHeader->Enable == 0U) {
                        pNandHeader->Enable = 1U;
                    }
                    if (pNandHeader->DebugMsgOn == 0U) {
                        pNandHeader->DebugMsgOn = SVC_CalibBpcCtrl.EnableDebugMsg;
                    }
                    if (pNandHeader->DebugMsgOn == 0U) {
                        pNandHeader->DebugMsgOn = pTblInfo->DbgMsgOn;
                    }
                    if (SVC_CalibBpcCtrl.EnableDebugMsg == 0U) {
                        SVC_CalibBpcCtrl.EnableDebugMsg = pTblInfo->DbgMsgOn;
                    }
                    if ((pNandHeader->TableSelectBits & SvcCalib_BitGet(TblID)) == 0U) {
                        pNandHeader->TableSelectBits |= SvcCalib_BitGet(TblID);
                        pNandHeader->NumOfTable += 1U;
                    }

                    /* Update to NAND */
                    RetVal = SvcCalib_DataSave(SVC_CALIB_BPC_ID);
                    if (RetVal != OK) {
                        PRN_CAL_BPC_LOG "Fail to save bpc to nand!" PRN_CAL_BPC_NG
                    } else {
                        SVC_CalibBpcCtrl.pNandDataFree = &(SVC_CalibBpcCtrl.pNandDataFree[DataSize]);
                    }
                }

                if (NeedFreeMem > 0U) {
                    SvcCalib_BpcMemUnLock();
                }
            }

            SvcCalib_BpcMutexGive(&(SVC_CalibBpcCtrl.Mutex));

            AmbaMisra_TouchUnused(pInput);
            AmbaMisra_TouchUnused(pTblInfo);
        }
    }

    return RetVal;
}

static void SvcCalib_BpcShellStrToU32(const char *pString, UINT32 *pValue)
{
    UINT32 ErrCode = AmbaUtility_StringToUInt32(pString, pValue);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }
}

static UINT32 SvcCalib_BpcShellCfgCalChan(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibBpcCtrlFlag & SVC_CALIB_BPC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to cfg bpc vin/sensor id - initial bpc module first!" PRN_CAL_BPC_NG
    } else if (ArgCount <= 7U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to cfg bpc vin/sensor id - Argc should > 7" PRN_CAL_BPC_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to cfg bpc vin/sensor id - Argv should not null!" PRN_CAL_BPC_NG
    } else {
        UINT32 VinID            = 0U;
        UINT32 SensorID         = 0U;
        UINT32 VinSelectBits    = 0U;
        UINT32 SensorSelectBits = 0U;
        UINT32 SensorModeBits   = 0U;

        SvcCalib_BpcShellStrToU32(pArgVector[3U], &VinID           );
        SvcCalib_BpcShellStrToU32(pArgVector[4U], &SensorID        );
        SvcCalib_BpcShellStrToU32(pArgVector[5U], &VinSelectBits   );
        SvcCalib_BpcShellStrToU32(pArgVector[6U], &SensorSelectBits);
        SvcCalib_BpcShellStrToU32(pArgVector[7U], &SensorModeBits  );

        if (SVC_OK != SvcCalib_BpcVinSensorIDCheck(VinID, SensorID)) {
            RetVal = SVC_NG;
            PRN_CAL_BPC_LOG "Fail to cfg bpc vin/sensor id - invalid VinID %d, SensorID 0x%x"
                PRN_CAL_BPC_ARG_UINT32 VinID    PRN_CAL_BPC_ARG_POST
                PRN_CAL_BPC_ARG_UINT32 SensorID PRN_CAL_BPC_ARG_POST
            PRN_CAL_BPC_NG
        } else {

            /* Initial the bpc calculate parameters */
            AmbaSvcWrap_MisraMemset(&(SVC_CalibBpcCtrl.CalcCtrl), 0, sizeof(SVC_CALIB_BPC_CALC_CTRL_s));
            SVC_CalibBpcCtrlFlag &= ~( SVC_CALIB_BPC_FLG_CALC_INIT | SVC_CALIB_BPC_FLG_CALC_VIN_SEN );

            SVC_CalibBpcCtrl.CalcCtrl.SaveBinOn                    = 1U;
            SVC_CalibBpcCtrl.CalcCtrl.TableIdx                     = 0xFFFFFFFFU;
            SVC_CalibBpcCtrlFlag |= SVC_CALIB_BPC_FLG_CALC_INIT;

            /* Configure the calib channel */
            AmbaSvcWrap_MisraMemset(&(SVC_CalibBpcCtrl.CalibChan), 0, sizeof(SVC_CALIB_CHANNEL_s));
            SVC_CalibBpcCtrl.CalibChan.VinSelectBits    = VinSelectBits   ;
            SVC_CalibBpcCtrl.CalibChan.SensorSelectBits = SensorSelectBits;
            SVC_CalibBpcCtrl.CalibChan.VinID            = VinID           ;
            SVC_CalibBpcCtrl.CalibChan.SensorID         = SensorID        ;
            SVC_CalibBpcCtrl.CalibChan.ExtendData       = SensorModeBits  ;

            PRN_CAL_BPC_LOG " " PRN_CAL_BPC_API
            PRN_CAL_BPC_LOG "------ Calibration BPC Channel ------" PRN_CAL_BPC_API
            PRN_CAL_BPC_LOG "  VinID            : 0x%x"
                PRN_CAL_BPC_ARG_UINT32 SVC_CalibBpcCtrl.CalibChan.VinID PRN_CAL_BPC_ARG_POST          
            PRN_CAL_BPC_API
            PRN_CAL_BPC_LOG "  SensorID         : 0x%x"
                PRN_CAL_BPC_ARG_UINT32 SVC_CalibBpcCtrl.CalibChan.SensorID PRN_CAL_BPC_ARG_POST       
            PRN_CAL_BPC_API
            PRN_CAL_BPC_LOG "  VinSelectBits    : 0x%x"
                PRN_CAL_BPC_ARG_UINT32 SVC_CalibBpcCtrl.CalibChan.VinSelectBits PRN_CAL_BPC_ARG_POST  
            PRN_CAL_BPC_API
            PRN_CAL_BPC_LOG "  SensorSelectBits : 0x%x"
                PRN_CAL_BPC_ARG_UINT32 SVC_CalibBpcCtrl.CalibChan.SensorSelectBits PRN_CAL_BPC_ARG_POST
            PRN_CAL_BPC_API
            PRN_CAL_BPC_LOG "  SensorModeBits   : 0x%x"
                PRN_CAL_BPC_ARG_UINT32 SVC_CalibBpcCtrl.CalibChan.ExtendData PRN_CAL_BPC_ARG_POST     
            PRN_CAL_BPC_API
        }
    }

    return RetVal;
}


static void SvcCalib_BpcShellCfgCalChanU(void)
{
    PRN_CAL_BPC_LOG "  %scfg_calib_chan%s        : reset all calibration setting as default"
        PRN_CAL_BPC_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_BPC_ARG_POST
    PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "  ------------------------------------------------------" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [VinID]             : vin identify number." PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [SensorID]          : sensor identify number. 1/2/4/8" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [VinSelectBits]     : bit0 : Vin0, bit1 : Vin1, ... " PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [SensorSelectBits]  : bit0 : sensor0, bit1 : sensor1, ..." PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [SensorModeBits]    : supported sensor mode bits" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG " " PRN_CAL_BPC_API
}


static UINT32 SvcCalib_BpcShellCfgTblID(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibBpcCtrlFlag & SVC_CALIB_BPC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to cfg bpc table idx - initial bpc module first!" PRN_CAL_BPC_NG
    } else if (ArgCount <= 3U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to cfg bpc table idx - Argc should >= 3" PRN_CAL_BPC_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to cfg bpc table idx - Argv should not null!" PRN_CAL_BPC_NG
    } else {
        UINT32 TableIdx = 0U;

        SvcCalib_BpcShellStrToU32(pArgVector[3U],  &TableIdx );

        SVC_CalibBpcCtrl.CalcCtrl.TableIdx = TableIdx;

        PRN_CAL_BPC_LOG " " PRN_CAL_BPC_API
        PRN_CAL_BPC_LOG "------ Calibration BPC Table Index %d ------"
            PRN_CAL_BPC_ARG_UINT32 SVC_CalibBpcCtrl.CalcCtrl.TableIdx PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_API

    }

    return RetVal;
}


static void SvcCalib_BpcShellCfgTblIDU(void)
{
    PRN_CAL_BPC_LOG "  %scfg_table_id%s          :"
        PRN_CAL_BPC_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_BPC_ARG_POST
    PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "  ------------------------------------------------------" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [Nand Table ID]     : the nand table id." PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "                        : the max value depend on each app" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG " " PRN_CAL_BPC_API
}


static UINT32 SvcCalib_BpcShellCfgDataType(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibBpcCtrlFlag & SVC_CALIB_BPC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to cfg data type - initial bpc module first!" PRN_CAL_BPC_NG
    } else if (ArgCount <= 3U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to cfg data type - Argc should >= 3" PRN_CAL_BPC_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to cfg data type - Argv should not null!" PRN_CAL_BPC_NG
    } else {
        UINT32 DataType = 0U;

        SvcCalib_BpcShellStrToU32(pArgVector[3U],  &DataType );

        if (ArgCount >= 5U) {
            SvcCalib_BpcShellStrToU32(pArgVector[4U], &(SVC_CalibBpcCtrl.CalcCtrl.Lz77SearchWin));
        }

        SVC_CalibBpcCtrl.CalcCtrl.DataType = DataType;

        PRN_CAL_BPC_LOG " " PRN_CAL_BPC_API
        PRN_CAL_BPC_LOG "------ Calibration BPC Data Type %d ------"
            PRN_CAL_BPC_ARG_UINT32 SVC_CalibBpcCtrl.CalcCtrl.DataType PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_API

    }

    return RetVal;
}


static void SvcCalib_BpcShellCfgDataTypeU(void)
{
    PRN_CAL_BPC_LOG "  %scfg_data_type%s         :"
        PRN_CAL_BPC_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_BPC_ARG_POST
    PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "  ------------------------------------------------------" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [Data Type]         : 0: Un-compression bpc table" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "                        : 1: Lz77 Compression bpc table" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG " " PRN_CAL_BPC_API
}


static UINT32 SvcCalib_BpcShellCfgDbgMsg(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibBpcCtrlFlag & SVC_CALIB_BPC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to switch bpc debug msg - initial bpc module first!" PRN_CAL_BPC_NG
    } else if (ArgCount <= 3U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to switch bpc debug msg - Argc should >= 3" PRN_CAL_BPC_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to switch bpc debug msg - Argv should not null!" PRN_CAL_BPC_NG
    } else {
        UINT32 DbgMsgOn = 0U;

        SvcCalib_BpcShellStrToU32(pArgVector[3U],  &DbgMsgOn );

        if ((DbgMsgOn & 0x1U) > 0U) {
            SVC_CalibBpcCtrl.EnableDebugMsg = DbgMsgOn;
        }

        if ((DbgMsgOn & 0x2U) > 0U) {
            SVC_CalibBpcCtrlFlag |= SVC_LOG_CAL_DBG;
        }


        PRN_CAL_BPC_LOG " " PRN_CAL_BPC_API
        PRN_CAL_BPC_LOG "------ Calibration BPC Debug Msg On/Off %d, lvl %d ------"
            PRN_CAL_BPC_ARG_UINT32 SVC_CalibBpcCtrl.EnableDebugMsg PRN_CAL_BPC_ARG_POST
            PRN_CAL_BPC_ARG_UINT32 DbgMsgOn                        PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_API

    }

    return RetVal;
}


static void SvcCalib_BpcShellCfgDbgMsgU(void)
{
    PRN_CAL_BPC_LOG "  %scfg_dbg_msg%s           :"
        PRN_CAL_BPC_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_BPC_ARG_POST
    PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "  ------------------------------------------------------" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [Enable]            : 0: disable debug message" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "                        : 1: enable debug message" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "                        : 2: enable debug message level 2" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG " " PRN_CAL_BPC_API
}


static UINT32 SvcCalib_BpcShellGenTbl(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibBpcCtrlFlag & SVC_CALIB_BPC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to proc bpc 'gen_table' - initial bpc module first!" PRN_CAL_BPC_NG
    } else if (ArgCount <= 3U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to proc bpc 'gen_table' - Argc should > 3" PRN_CAL_BPC_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to proc bpc 'gen_table' - Argv should not null!" PRN_CAL_BPC_NG
    } else {
        RetVal = SvcCalib_BpcTableGenTbl(&(SVC_CalibBpcCtrl.CalibChan), pArgVector[3U]);
    }

    return RetVal;
}


static void SvcCalib_BpcShellGenTblU(void)
{
    PRN_CAL_BPC_LOG "  %sgen_table%s             : start calculate BPC table and save to NVM"
        PRN_CAL_BPC_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_BPC_ARG_POST
    PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "  ------------------------------------------------------" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [Script]            : bpc ctuner script path." PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG " " PRN_CAL_BPC_API
}


static UINT32 SvcCalib_BpcShellEnable(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibBpcCtrlFlag & SVC_CALIB_BPC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to enable/disable bpc - initial bpc module first!" PRN_CAL_BPC_NG
    } else if (ArgCount <= 4U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to enable/disable bpc - Argc should > 4" PRN_CAL_BPC_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to enable/disable bpc - Argv should not null!" PRN_CAL_BPC_NG
    } else {
        UINT32 IkContextID = 0xFFFFFFFFU, Enable = 0U;

        SvcCalib_BpcShellStrToU32(pArgVector[3U],  &IkContextID );
        SvcCalib_BpcShellStrToU32(pArgVector[4U],  &Enable      );
        if (Enable > 0U) {
            Enable = 1U;
        } else {
            Enable = 0U;
        }

        if (IkContextID != 0xFFFFFFFFU) {
            UINT32 CurIkState = 0U;
            AMBA_IK_MODE_CFG_s ImgMode;

            AmbaSvcWrap_MisraMemset(&ImgMode, 0, sizeof(ImgMode));
            ImgMode.ContextId = IkContextID;
            PRetVal = AmbaIK_GetStaticBadPxlCorEnb(&ImgMode, &CurIkState);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_CAL_BPC_LOG "Fail to enable/disable bpc - get bpc enable/disable state fail" PRN_CAL_BPC_NG
            } else {
                if (CurIkState != Enable) {
                    PRetVal = AmbaIK_SetStaticBadPxlCorEnb(&ImgMode, Enable);
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;

                        PRN_CAL_BPC_LOG "Fail to enable/disable bpc - %s bpc fail!"
                            PRN_CAL_BPC_ARG_CSTR   ( (Enable > 0U) ? ( "enable" ) : ( "disable" ) ) PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_NG
                    } else {
                        PRN_CAL_BPC_LOG "%s bpc done!"
                            PRN_CAL_BPC_ARG_CSTR   ( (Enable > 0U) ? ( "enable" ) : ( "disable" ) ) PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_API
                    }
                } else {
                    PRN_CAL_BPC_LOG "current IK bpc state: %s!"
                        PRN_CAL_BPC_ARG_CSTR   ( (CurIkState > 0U) ? ( "enable" ) : ( "disable" ) ) PRN_CAL_BPC_ARG_POST
                    PRN_CAL_BPC_API
                }
            }
        } else {
            RetVal = SVC_NG;
            PRN_CAL_BPC_LOG "Fail to enable/disable bpc - invalid ik context id(%d)"
                PRN_CAL_BPC_ARG_UINT32 IkContextID PRN_CAL_BPC_ARG_POST
            PRN_CAL_BPC_NG
        }
    }

    return RetVal;
}


static void SvcCalib_BpcShellEnableU(void)
{
    PRN_CAL_BPC_LOG "  %senable%s                : enable/disable bpc effect"
        PRN_CAL_BPC_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_BPC_ARG_POST
    PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "  ------------------------------------------------------" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [IK Context ID]     : image kernel context id." PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [Enable]            : 0 => disable" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "                        : 1 => enable" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG " " PRN_CAL_BPC_API
}


static UINT32 SvcCalib_BpcShellUpd(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibBpcCtrlFlag & SVC_CALIB_BPC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to proc bpc->upd cmd - initial bpc module first!" PRN_CAL_BPC_NG
    } else if (ArgCount <= 6U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to proc bpc->upd cmd - Argc should > 6" PRN_CAL_BPC_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to proc bpc->upd cmd - Argv should not null!" PRN_CAL_BPC_NG
    } else {
        UINT32 VinID       = 0xFFFFFFFFU;
        UINT32 SensorID    = 0xFFFFFFFFU;
        UINT32 SensorMode  = 0xFFFFFFFFU;
        UINT32 IkContextID = 0xFFFFFFFFU;

        SvcCalib_BpcShellStrToU32(pArgVector[3U],  &VinID       );
        SvcCalib_BpcShellStrToU32(pArgVector[4U],  &SensorID    );
        SvcCalib_BpcShellStrToU32(pArgVector[5U],  &SensorMode  );
        SvcCalib_BpcShellStrToU32(pArgVector[6U],  &IkContextID );

        if (SVC_OK != SvcCalib_BpcVinSensorIDCheck(VinID, SensorID)) {
            RetVal = SVC_NG;
            PRN_CAL_BPC_LOG "Fail to proc bpc->upd cmd - invalid VinID(%d), SensorID(%d)!"
                PRN_CAL_BPC_ARG_UINT32 VinID    PRN_CAL_BPC_ARG_POST
                PRN_CAL_BPC_ARG_UINT32 SensorID PRN_CAL_BPC_ARG_POST
            PRN_CAL_BPC_NG
        } else if (IkContextID == 0xFFFFFFFFU) {
            RetVal = SVC_NG;
            PRN_CAL_BPC_LOG "Fail to proc bpc->upd cmd - invalid IKContextID(%d)!"
                PRN_CAL_BPC_ARG_UINT32 IkContextID PRN_CAL_BPC_ARG_POST
            PRN_CAL_BPC_NG
        } else {
            UINT32 CurIkEnable = 0U;
            AMBA_IK_MODE_CFG_s ImgMode;

            AmbaSvcWrap_MisraMemset(&ImgMode, 0, sizeof(ImgMode));
            ImgMode.ContextId = IkContextID;
            PRetVal = AmbaIK_GetStaticBadPxlCorEnb(&ImgMode, &CurIkEnable);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_CAL_BPC_LOG "Fail to proc bpc->upd cmd - get bpc enable/disable state fail! ErrCode: 0x%x"
                    PRN_CAL_BPC_ARG_UINT32 PRetVal PRN_CAL_BPC_ARG_POST
                PRN_CAL_BPC_NG
            } else {

                // If the bpc has been enable, disable it before update bpc table
                if (CurIkEnable > 0U) {
                    PRetVal = AmbaIK_SetStaticBadPxlCorEnb(&ImgMode, 0U);
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;
                        PRN_CAL_BPC_LOG "Fail to proc bpc->upd cmd - disable bpc failure! ErrCode: 0x%x"
                            PRN_CAL_BPC_ARG_UINT32 PRetVal PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_NG
                    }
                }

                if (RetVal == SVC_OK) {
                    UINT32 WorkTblIdx = 0xFFFFFFFFU;
                    SVC_CALIB_CHANNEL_s CalibChan;

                    // If working buffer table exist, reset it first
                    PRetVal = SvcCalib_BpcTableSearch(VinID, SensorID, SensorMode, &WorkTblIdx); PRN_CAL_BPC_ERR_HDLR
                    if (WorkTblIdx < SVC_CalibBpcCtrl.NumOfTable) {
                        AmbaSvcWrap_MisraMemset(&(SVC_CalibBpcCtrl.pBpcTable[WorkTblIdx]), 0, sizeof(SVC_CALIB_BPC_TABLE_s));
                    }

                    // Update the calib table from nand
                    AmbaSvcWrap_MisraMemset(&CalibChan, 0, sizeof(CalibChan));
                    CalibChan.VinID    = VinID;
                    CalibChan.SensorID = SensorID;
                    RetVal = SvcCalib_BpcTableUpdate(&CalibChan, &ImgMode);
                    if (RetVal != 0U) {
                        PRN_CAL_BPC_LOG "Fail to proc bpc->upd cmd - update calib table fail! VinID(%d), SensorID(%d), IkContextID(%d)"
                            PRN_CAL_BPC_ARG_UINT32 VinID       PRN_CAL_BPC_ARG_POST
                            PRN_CAL_BPC_ARG_UINT32 SensorID    PRN_CAL_BPC_ARG_POST
                            PRN_CAL_BPC_ARG_UINT32 IkContextID PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_NG
                    }
                }
            }
        }
    }

    return RetVal;
}


static void SvcCalib_BpcShellUpdU(void)
{
    PRN_CAL_BPC_LOG "  %supd%s                   : update bpc table"
        PRN_CAL_BPC_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_BPC_ARG_POST
    PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "  ------------------------------------------------------" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [VinID]             : vin identify number." PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [SensorID]          : sensor identify number. 1/2/4/8" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [SensorMode]        : sensor mode." PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [IK Context ID]     : image kernel context id." PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG " " PRN_CAL_BPC_API
}


static UINT32 SvcCalib_BpcShellSet(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibBpcCtrlFlag & SVC_CALIB_BPC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to proc bpc->upd cmd - initial bpc module first!" PRN_CAL_BPC_NG
    } else if ((SVC_CalibBpcCtrlFlag & SVC_CALIB_BPC_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to proc bpc->set cmd - initial bpc shadow first!" PRN_CAL_BPC_NG
    } else if (SVC_CalibBpcCtrl.pNandTable == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to proc bpc->set cmd - invalid bpc nand table!" PRN_CAL_BPC_NG
    } else if ((SVC_CalibBpcCtrl.pNandDataBase == NULL)
             ||(SVC_CalibBpcCtrl.pNandDataFree == NULL)
             ||(SVC_CalibBpcCtrl.pNandDataLimit == NULL)) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to proc bpc->set cmd - take mutex fail!" PRN_CAL_BPC_NG
    } else if (ArgCount <= 5U) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to proc bpc->set cmd - Argc should > 5" PRN_CAL_BPC_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to proc bpc->set cmd - Argv should not null!" PRN_CAL_BPC_NG
    } else {
        UINT32 TblID = 0xFFU, ArgIdx = 3U;
        UINT8 *pCalcMem = NULL;
        UINT32 CalcMemSize = 0U;

        SvcCalib_BpcShellStrToU32(pArgVector[ArgIdx],  &TblID); ArgIdx ++;
        if (TblID >= SVC_CalibBpcCtrl.NumOfTable) {
            RetVal = SVC_NG;
            PRN_CAL_BPC_LOG "Fail to proc bpc->set cmd - insert table id(%d) is out-of range(%d)!"
                PRN_CAL_BPC_ARG_UINT32 TblID                       PRN_CAL_BPC_ARG_POST
                PRN_CAL_BPC_ARG_UINT32 SVC_CalibBpcCtrl.NumOfTable PRN_CAL_BPC_ARG_POST
            PRN_CAL_BPC_NG
        } else if (0U != SvcCalib_ItemCalcMemGet(SVC_CALIB_BPC_ID, 1U, &pCalcMem, &CalcMemSize)) {
            RetVal = SVC_NG;
            PRN_CAL_BPC_LOG "Fail to proc bpc->set cmd - query bpc working memory size fail!" PRN_CAL_BPC_NG
        } else if ((pCalcMem == NULL) || (CalcMemSize == 0U)) {
            RetVal = SVC_NG;
            PRN_CAL_BPC_LOG "Fail to proc bpc->set cmd - invalid bpc working memory" PRN_CAL_BPC_NG
        } else {
            SVC_CALIB_BPC_NAND_TABLE_s CurBpcTbl;
            const char *pFilePath = NULL;
            SVC_CALIB_BPC_NAND_TABLE_s *pNandTable = &(SVC_CalibBpcCtrl.pNandTable[TblID]);
            AMBA_FS_FILE_INFO_s FileInfo;
            UINT8 *pDataBuf = NULL;
            UINT32 DataSize = 0U;
            UINT32 DbgMsgOn = 1U;

            AmbaSvcWrap_MisraMemset(&CurBpcTbl, 0, sizeof(SVC_CALIB_BPC_NAND_TABLE_s));
            CurBpcTbl.Header.Version     = SVC_CALIB_BPC_VERSION;

            SvcCalib_BpcShellStrToU32(pArgVector[ArgIdx], &(CurBpcTbl.Header.VinSelectBits   )); ArgIdx++;
            SvcCalib_BpcShellStrToU32(pArgVector[ArgIdx], &(CurBpcTbl.Header.SensorSelectBits)); ArgIdx++;
            SvcCalib_BpcShellStrToU32(pArgVector[ArgIdx], &(CurBpcTbl.Header.SensorModeBits  )); ArgIdx++;
            SvcCalib_BpcShellStrToU32(pArgVector[ArgIdx], &(CurBpcTbl.Header.BpcCount        )); ArgIdx++;

            SvcCalib_BpcShellStrToU32(pArgVector[ArgIdx], &(CurBpcTbl.Header.VinSensorGeo.StartX              )); ArgIdx++;
            SvcCalib_BpcShellStrToU32(pArgVector[ArgIdx], &(CurBpcTbl.Header.VinSensorGeo.StartY              )); ArgIdx++;
            SvcCalib_BpcShellStrToU32(pArgVector[ArgIdx], &(CurBpcTbl.Header.VinSensorGeo.Width               )); ArgIdx++;
            SvcCalib_BpcShellStrToU32(pArgVector[ArgIdx], &(CurBpcTbl.Header.VinSensorGeo.Height              )); ArgIdx++;
            SvcCalib_BpcShellStrToU32(pArgVector[ArgIdx], &(CurBpcTbl.Header.VinSensorGeo.HSubSample.FactorDen)); ArgIdx++;
            SvcCalib_BpcShellStrToU32(pArgVector[ArgIdx], &(CurBpcTbl.Header.VinSensorGeo.HSubSample.FactorNum)); ArgIdx++;
            SvcCalib_BpcShellStrToU32(pArgVector[ArgIdx], &(CurBpcTbl.Header.VinSensorGeo.VSubSample.FactorDen)); ArgIdx++;
            SvcCalib_BpcShellStrToU32(pArgVector[ArgIdx], &(CurBpcTbl.Header.VinSensorGeo.VSubSample.FactorNum)); ArgIdx++;

            pFilePath = pArgVector[ArgIdx]; ArgIdx++;
            SvcCalib_BpcShellStrToU32(pArgVector[ArgIdx], &(CurBpcTbl.DataType)); ArgIdx++;

            SvcCalib_BpcShellStrToU32(pArgVector[ArgIdx], &DbgMsgOn);

            if (CurBpcTbl.DataType > SVC_CALIB_CMPR_LZ77) {
                RetVal = SVC_NG;
                PRN_CAL_BPC_LOG "Fail to proc bpc->set cmd - not supported data type(%d)"
                    PRN_CAL_BPC_ARG_UINT32 CurBpcTbl.DataType PRN_CAL_BPC_ARG_POST
                PRN_CAL_BPC_NG
            } else {

                AmbaSvcWrap_MisraMemset(&FileInfo, 0, sizeof(FileInfo));
                PRetVal = AmbaFS_GetFileInfo(pFilePath, &FileInfo);
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    PRN_CAL_BPC_LOG "Fail to proc bpc->set cmd - get file info fail! ErrCode(0x%08X) '%s'"
                        PRN_CAL_BPC_ARG_UINT32 PRetVal   PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_ARG_CSTR   pFilePath PRN_CAL_BPC_ARG_POST
                    PRN_CAL_BPC_NG
                } else if ((CurBpcTbl.DataType == SVC_CALIB_CMPR_NONE) && ((UINT32)(FileInfo.Size) > CalcMemSize)) {
                    RetVal = SVC_NG;
                    PRN_CAL_BPC_LOG "Fail to proc bpc->set cmd - local buffer size(0x%X) too small. It cannot handle bpc file size(0x%X)!"
                        PRN_CAL_BPC_ARG_UINT32 CalcMemSize   PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_ARG_UINT32 FileInfo.Size PRN_CAL_BPC_ARG_POST
                    PRN_CAL_BPC_NG
                } else if ((CurBpcTbl.DataType == SVC_CALIB_CMPR_LZ77) && (((UINT32)(FileInfo.Size) * 2U) > CalcMemSize)) {
                    RetVal = SVC_NG;
                    PRN_CAL_BPC_LOG "Fail to proc bpc->set cmd - local buffer size(0x%X) too small. It cannot handle bpc file size(0x%X) + data compress type!"
                        PRN_CAL_BPC_ARG_UINT32 CalcMemSize   PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_ARG_UINT32 FileInfo.Size PRN_CAL_BPC_ARG_POST
                    PRN_CAL_BPC_NG
                } else {
                    AMBA_FS_FILE *pFile = NULL;

                    DataSize = (UINT32)(FileInfo.Size);
                    pDataBuf = pCalcMem;

                    PRetVal = AmbaFS_FileOpen(pFilePath, "rb", &pFile);
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;
                        PRN_CAL_BPC_LOG "Fail to proc bpc->set cmd - open file fail! ErrCode(0x%08X)"
                            PRN_CAL_BPC_ARG_UINT32 PRetVal PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_NG
                    } else if (pFile == NULL) {
                        RetVal = SVC_NG;
                        PRN_CAL_BPC_LOG "Fail to proc bpc->set cmd - invalid file pointer" PRN_CAL_BPC_NG
                    } else {
                        UINT32 NumSuccess = 0U;
                        PRetVal = AmbaFS_FileRead(pDataBuf, 1, DataSize, pFile, &NumSuccess);
                        if (PRetVal != 0U) {
                            RetVal = SVC_NG;
                            PRN_CAL_BPC_LOG "Fail to proc bpc->set cmd - read data fail! ErrCode(0x%08X)"
                                PRN_CAL_BPC_ARG_UINT32 PRetVal PRN_CAL_BPC_ARG_POST
                            PRN_CAL_BPC_NG
                        } else if (NumSuccess != DataSize) {
                            RetVal = SVC_NG;
                            PRN_CAL_BPC_LOG "Fail to proc bpc->set cmd - read size(0x%X) does not same with request size(0x%X) fail!"
                                PRN_CAL_BPC_ARG_UINT32 NumSuccess PRN_CAL_BPC_ARG_POST
                                PRN_CAL_BPC_ARG_UINT32 DataSize   PRN_CAL_BPC_ARG_POST
                            PRN_CAL_BPC_NG
                        } else {
                            pCalcMem = &(pCalcMem[DataSize]);
                            CalcMemSize -= DataSize;

                            if (CurBpcTbl.DataType != SVC_CALIB_CMPR_LZ77) {
                                UINT32 CmprSize = 0U;
                                SVC_CALIB_COMPRESS_s CmprCtrl;

                                AmbaSvcWrap_MisraMemset(&CmprCtrl, 0, sizeof(CmprCtrl));
                                CmprCtrl.CmprType        = CurBpcTbl.DataType;
                                CmprCtrl.pCmprSize       = &CmprSize;
                                CmprCtrl.pSource         = pDataBuf;
                                CmprCtrl.SourceSize      = DataSize;
                                CmprCtrl.pDestination    = pCalcMem;
                                CmprCtrl.DestinationSize = CalcMemSize;
                                CmprCtrl.Lz77.SearchWin  = SVC_CalibBpcCtrl.CalcCtrl.Lz77SearchWin;

                                if (0U != SvcCalib_DataCompress(&CmprCtrl)) {
                                    CurBpcTbl.DataType = SVC_CALIB_CMPR_NONE;
                                    PRN_CAL_BPC_LOG "Fail to calc bpc table - compress data fail! Write the data directly!" PRN_CAL_BPC_NG
                                } else {
                                    PRN_CAL_BPC_LOG "Success proc lz77 compression. Src(%p, %d) -> Dst(%p, %d)"
                                        PRN_CAL_BPC_ARG_CPOINT pDataBuf PRN_CAL_BPC_ARG_POST
                                        PRN_CAL_BPC_ARG_UINT32 DataSize PRN_CAL_BPC_ARG_POST
                                        PRN_CAL_BPC_ARG_CPOINT pCalcMem PRN_CAL_BPC_ARG_POST
                                        PRN_CAL_BPC_ARG_UINT32 CmprSize PRN_CAL_BPC_ARG_POST
                                    PRN_CAL_BPC_API

                                    pDataBuf = pCalcMem;
                                    DataSize = CmprSize;
                                }
                            }
                        }

                        PRetVal = AmbaFS_FileClose(pFile); PRN_CAL_BPC_ERR_HDLR
                    }
                }

            }

            if (RetVal == 0U) {
                if (KAL_ERR_NONE != SvcCalib_BpcMutexTake(&(SVC_CalibBpcCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
                    RetVal = SVC_NG;
                    PRN_CAL_BPC_LOG "Fail to proc bpc->set cmd - take mutex fail!" PRN_CAL_BPC_NG
                } else {
                    if (0U < SvcCalib_BpcMemAddrComp(&(SVC_CalibBpcCtrl.pNandDataFree[DataSize]), SVC_CalibBpcCtrl.pNandDataLimit)) {
                        RetVal = SVC_NG;
                        PRN_CAL_BPC_LOG "Fail to proc bpc->set cmd - free data nand size does not enough to service it!" PRN_CAL_BPC_NG
                        PRN_CAL_BPC_LOG "  DataSize      : 0x%X"
                            PRN_CAL_BPC_ARG_UINT32 DataSize PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_NG
                        PRN_CAL_BPC_LOG "  NandDataFree  : %p"
                            PRN_CAL_BPC_ARG_CPOINT SVC_CalibBpcCtrl.pNandDataFree PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_NG
                        PRN_CAL_BPC_LOG "  NandDataLimit : %p"
                            PRN_CAL_BPC_ARG_CPOINT SVC_CalibBpcCtrl.pNandDataLimit PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_NG
                    } else {
                        CurBpcTbl.DataSize = DataSize;
                        CurBpcTbl.DataOfs  = SvcCalib_BpcMemAddrDiff(SVC_CalibBpcCtrl.pNandDataFree, SVC_CalibBpcCtrl.pNandDataBase);
                        AmbaSvcWrap_MisraMemcpy(pNandTable, &CurBpcTbl, sizeof(SVC_CALIB_BPC_NAND_TABLE_s));

                        AmbaSvcWrap_MisraMemcpy(SVC_CalibBpcCtrl.pNandDataFree, pDataBuf, DataSize);

                        PRN_CAL_BPC_LOG " " PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "====== Calibration bpc info ======" PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "  pNandTbl->Header.VinSelectBits                     : 0x%X"
                            PRN_CAL_BPC_ARG_UINT32 pNandTable->Header.VinSelectBits PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "  pNandTbl->Header.SensorSelectBits                  : 0x%X"
                            PRN_CAL_BPC_ARG_UINT32 pNandTable->Header.SensorSelectBits PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "  pNandTbl->Header.SensorModeBits                    : 0x%X"
                            PRN_CAL_BPC_ARG_UINT32 pNandTable->Header.SensorModeBits PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "  pNandTbl->Header.Version                           : 0x%X"
                            PRN_CAL_BPC_ARG_UINT32 pNandTable->Header.Version PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "  pNandTbl->Header.VinSensorGeo.StartX               : %d"
                            PRN_CAL_BPC_ARG_UINT32 pNandTable->Header.VinSensorGeo.StartX PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "  pNandTbl->Header.VinSensorGeo.StartY               : %d"
                            PRN_CAL_BPC_ARG_UINT32 pNandTable->Header.VinSensorGeo.StartY PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "  pNandTbl->Header.VinSensorGeo.Width                : %d"
                            PRN_CAL_BPC_ARG_UINT32 pNandTable->Header.VinSensorGeo.Width PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "  pNandTbl->Header.VinSensorGeo.Height               : %d"
                            PRN_CAL_BPC_ARG_UINT32 pNandTable->Header.VinSensorGeo.Height PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "  pNandTbl->Header.VinSensorGeo.HSubSample.FactorDen : 0x%X"
                            PRN_CAL_BPC_ARG_UINT32 pNandTable->Header.VinSensorGeo.HSubSample.FactorDen PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "  pNandTbl->Header.VinSensorGeo.HSubSample.FactorNum : 0x%X"
                            PRN_CAL_BPC_ARG_UINT32 pNandTable->Header.VinSensorGeo.HSubSample.FactorNum PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "  pNandTbl->Header.VinSensorGeo.VSubSample.FactorDen : 0x%X"
                            PRN_CAL_BPC_ARG_UINT32 pNandTable->Header.VinSensorGeo.VSubSample.FactorDen PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "  pNandTbl->Header.VinSensorGeo.VSubSample.FactorNum : 0x%X"
                            PRN_CAL_BPC_ARG_UINT32 pNandTable->Header.VinSensorGeo.VSubSample.FactorNum PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "  pNandTbl->Header.BpcCount                          : 0x%X"
                            PRN_CAL_BPC_ARG_UINT32 pNandTable->Header.BpcCount PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "  pNandTbl->DataType                                 : 0x%X"
                            PRN_CAL_BPC_ARG_UINT32 pNandTable->DataType PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "  pNandTbl->DataSize                                 : 0x%X"
                            PRN_CAL_BPC_ARG_UINT32 pNandTable->DataSize PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_API
                        PRN_CAL_BPC_LOG "  pNandTbl->DataOfs                                  : 0x%X"
                            PRN_CAL_BPC_ARG_UINT32 pNandTable->DataOfs PRN_CAL_BPC_ARG_POST
                        PRN_CAL_BPC_API

                        // Update nand header
                        SVC_CalibBpcCtrl.pNandHeader->Enable = 1U;
                        SVC_CalibBpcCtrl.pNandHeader->DebugMsgOn = DbgMsgOn;
                        if ((SVC_CalibBpcCtrl.pNandHeader->TableSelectBits & SvcCalib_BitGet(TblID)) == 0U) {
                            SVC_CalibBpcCtrl.pNandHeader->TableSelectBits |= SvcCalib_BitGet(TblID);
                            SVC_CalibBpcCtrl.pNandHeader->NumOfTable ++;
                        }

                        /* Update to NAND */
                        RetVal = SvcCalib_DataSave(SVC_CALIB_BPC_ID);
                        if (RetVal != OK) {
                            PRN_CAL_BPC_LOG "Fail to save bpc to nand!" PRN_CAL_BPC_NG
                        } else {
                            SVC_CalibBpcCtrl.pNandDataFree = &(SVC_CalibBpcCtrl.pNandDataFree[DataSize]);
                        }
                    }

                    SvcCalib_BpcMutexGive(&(SVC_CalibBpcCtrl.Mutex));
                }
            }

            PRetVal = SvcCalib_ItemCalcMemFree(SVC_CALIB_BPC_ID); PRN_CAL_BPC_ERR_HDLR
        }
    }

    return RetVal;
}

static void SvcCalib_BpcShellSetU(void)
{
    PRN_CAL_BPC_LOG "  %sset%s                   : set BPC table manually"
        PRN_CAL_BPC_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_BPC_ARG_POST
    PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "  ------------------------------------------------------" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [TblID]                 : inser table index" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [VinSelectBits]         : the table supported vin select bits" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [SensorSelectBits]      : the table supported sensor select bits" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [SensorModeBits]        : the table supported sensor mode bits" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [BPC counter]           : bpc count" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [Calib StartX]          : calibration geometric StartX              " PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [Calib StartY]          : calibration geometric StartY              " PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [Calib Width]           : calibration geometric Width               " PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [Calib Height]          : calibration geometric Height              " PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [Calib HbSampleDen]     : calibration geometric HSubSample.FactorDen" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [Calib HbSampleNum]     : calibration geometric HSubSample.FactorNum" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [Calib VbSampleDen]     : calibration geometric VSubSample.FactorDen" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [Calib VbSampleNum]     : calibration geometric VSubSample.FactorNum" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [Bpc File Path]         : bpc file path" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [DataCmprType]          : bpc table compression type" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "                            : 0: un-compression" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "                            : 1: lz77 data compression" PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "    [DebugMsgOn]            : configure debug msg" PRN_CAL_BPC_API

    PRN_CAL_BPC_LOG " " PRN_CAL_BPC_API

}

static void SvcCalib_BpcShellEntryInit(void)
{
    if ((SVC_CalibBpcCtrlFlag & SVC_CALIB_BPC_FLG_SHELL_INIT) == 0U) {
        UINT32 ShellNum = 0U;

        AmbaSvcWrap_MisraMemset(CalibBpcShellFunc, 0, sizeof(CalibBpcShellFunc));

        CalibBpcShellFunc[ShellNum] = (SVC_CALIB_BPC_SHELL_FUNC_s) { 1U, "cfg_calib_chan", SvcCalib_BpcShellCfgCalChan,  SvcCalib_BpcShellCfgCalChanU  }; ShellNum++;
        CalibBpcShellFunc[ShellNum] = (SVC_CALIB_BPC_SHELL_FUNC_s) { 1U, "cfg_table_id",   SvcCalib_BpcShellCfgTblID,    SvcCalib_BpcShellCfgTblIDU    }; ShellNum++;
        CalibBpcShellFunc[ShellNum] = (SVC_CALIB_BPC_SHELL_FUNC_s) { 1U, "cfg_data_type",  SvcCalib_BpcShellCfgDataType, SvcCalib_BpcShellCfgDataTypeU }; ShellNum++;
        CalibBpcShellFunc[ShellNum] = (SVC_CALIB_BPC_SHELL_FUNC_s) { 1U, "cfg_dbg_msg",    SvcCalib_BpcShellCfgDbgMsg,   SvcCalib_BpcShellCfgDbgMsgU   }; ShellNum++;
        CalibBpcShellFunc[ShellNum] = (SVC_CALIB_BPC_SHELL_FUNC_s) { 1U, "gen_table",      SvcCalib_BpcShellGenTbl,      SvcCalib_BpcShellGenTblU      }; ShellNum++;
        CalibBpcShellFunc[ShellNum] = (SVC_CALIB_BPC_SHELL_FUNC_s) { 1U, "enable",         SvcCalib_BpcShellEnable,      SvcCalib_BpcShellEnableU      }; ShellNum++;
        CalibBpcShellFunc[ShellNum] = (SVC_CALIB_BPC_SHELL_FUNC_s) { 1U, "upd",            SvcCalib_BpcShellUpd,         SvcCalib_BpcShellUpdU         }; ShellNum++;
        CalibBpcShellFunc[ShellNum] = (SVC_CALIB_BPC_SHELL_FUNC_s) { 1U, "set",            SvcCalib_BpcShellSet,         SvcCalib_BpcShellSetU         };

        SVC_CalibBpcCtrlFlag |= SVC_CALIB_BPC_FLG_SHELL_INIT;
    }
}


UINT32 SvcCalib_BpcShellFunc(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_NG;

    SvcCalib_BpcShellEntryInit();

    if (ArgCount > 0U) {
        UINT32 ShellIdx;
        UINT32 ShellCount = (UINT32)(sizeof(CalibBpcShellFunc)) / (UINT32)(sizeof(CalibBpcShellFunc[0]));

        for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
            if ((CalibBpcShellFunc[ShellIdx].pFunc != NULL) && (CalibBpcShellFunc[ShellIdx].Enable > 0U)) {
                if (0 == SvcWrap_strcmp(pArgVector[2U], CalibBpcShellFunc[ShellIdx].ShellCmdName)) {
                    if (SVC_OK != (CalibBpcShellFunc[ShellIdx].pFunc)(ArgCount, pArgVector)) {
                        if (CalibBpcShellFunc[ShellIdx].pUsageFunc != NULL) {
                            (CalibBpcShellFunc[ShellIdx].pUsageFunc)();
                        }
                    }

                    RetVal = SVC_OK;
                    break;
                }
            }
        }

        if (RetVal != SVC_OK) {
            SvcCalib_BpcShellUsage();
        }
    }

    return RetVal;
}


static void SvcCalib_BpcShellUsage(void)
{
    UINT32 ShellIdx;
    UINT32 ShellCount = (UINT32)(sizeof(CalibBpcShellFunc)) / (UINT32)(sizeof(CalibBpcShellFunc[0]));

    PRN_CAL_BPC_LOG " " PRN_CAL_BPC_API
    PRN_CAL_BPC_LOG "====== %sBPC Command Usage%s ======"
        PRN_CAL_BPC_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_0 PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_BPC_ARG_POST
    PRN_CAL_BPC_API

    for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
        if ((CalibBpcShellFunc[ShellIdx].pFunc != NULL) && (CalibBpcShellFunc[ShellIdx].Enable > 0U)) {
            if (CalibBpcShellFunc[ShellIdx].pUsageFunc == NULL) {
                PRN_CAL_BPC_LOG "  %s"
                    PRN_CAL_BPC_ARG_CSTR   CalibBpcShellFunc[ShellIdx].ShellCmdName PRN_CAL_BPC_ARG_POST
                PRN_CAL_BPC_API
            } else {
                (CalibBpcShellFunc[ShellIdx].pUsageFunc)();
            }
        }
    }
}


static void SvcCalib_BpcCmdHdlrInit(void)
{
    if ((SVC_CalibBpcCtrlFlag & SVC_CALIB_BPC_FLG_CMD_INIT) == 0U) {

        AmbaSvcWrap_MisraMemset(SvcCalibBpcCmdHdlr, 0, sizeof(SvcCalibBpcCmdHdlr));

        SvcCalibBpcCmdHdlr[SVC_CALIB_CMD_MEM_QUERY      ] = (SVC_CALIB_BPC_CMD_HDLR_s) {1, SVC_CALIB_CMD_MEM_QUERY,       SvcCalib_BpcCmdMemQuery  };
        SvcCalibBpcCmdHdlr[SVC_CALIB_CMD_ITEM_CREATE    ] = (SVC_CALIB_BPC_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_CREATE,     SvcCalib_BpcCmdCreate    };
        SvcCalibBpcCmdHdlr[SVC_CALIB_CMD_ITEM_DATA_INIT ] = (SVC_CALIB_BPC_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_DATA_INIT,  SvcCalib_BpcCmdDataInit  };
        SvcCalibBpcCmdHdlr[SVC_CALIB_CMD_ITEM_TBL_UPDATE] = (SVC_CALIB_BPC_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_TBL_UPDATE, SvcCalib_BpcCmdTblUpdate };
        SvcCalibBpcCmdHdlr[SVC_CALIB_CMD_ITEM_TBL_GET   ] = (SVC_CALIB_BPC_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_TBL_GET,    SvcCalib_BpcCmdTblDataGet};
        SvcCalibBpcCmdHdlr[SVC_CALIB_CMD_ITEM_TBL_SET   ] = (SVC_CALIB_BPC_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_TBL_SET,    SvcCalib_BpcCmdTblDataSet};

        SVC_CalibBpcCtrlFlag |= SVC_CALIB_BPC_FLG_CMD_INIT;
    }
}

static UINT32 SvcCalib_BpcCmdMemQuery(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    if (pParam1 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to query bpc memory - input bpc table number should not zero. %d" PRN_CAL_BPC_NG
    } else if (pParam2 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to query bpc memory - output bpc shadow buffer size should not null!" PRN_CAL_BPC_NG
    } else if (pParam3 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to query bpc memory - output bpc working buffer size should not null!" PRN_CAL_BPC_NG
    } else if (pParam4 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to query bpc memory - output bpc alg. buffer size should not null!" PRN_CAL_BPC_NG
    } else {
        UINT32 *pU32Val;
        UINT32 NomOfTable = 0U;
        UINT32 *pShadowBufSize, *pWorkingBufSize, *pCalcMemSize;

        AmbaMisra_TypeCast(&(pU32Val), &(pParam1));
        if (pU32Val != NULL) {
            NomOfTable = *pU32Val;
        }

        AmbaMisra_TypeCast(&(pShadowBufSize),  &(pParam2));
        AmbaMisra_TypeCast(&(pWorkingBufSize), &(pParam3));
        AmbaMisra_TypeCast(&(pCalcMemSize),    &(pParam4));

        RetVal = SvcCalib_BpcMemQuery(NomOfTable, pShadowBufSize, pWorkingBufSize, pCalcMemSize);
        if (RetVal != SVC_OK) {
            PRN_CAL_BPC_LOG "Fail to query bpc memory - query memory fail!" PRN_CAL_BPC_NG
        }

        AmbaMisra_TouchUnused(pParam1);
        AmbaMisra_TouchUnused(pParam2);
        AmbaMisra_TouchUnused(pParam3);
        AmbaMisra_TouchUnused(pParam4);
        AmbaMisra_TouchUnused(pU32Val);
    }

    return RetVal;
}


static UINT32 SvcCalib_BpcCmdCreate(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SvcCalib_BpcCreate();
    if (RetVal != SVC_OK) {
        PRN_CAL_BPC_LOG "Fail to init bpc - create module fail!" PRN_CAL_BPC_NG
    } else {
        AmbaMisra_TouchUnused(pParam1);
        AmbaMisra_TouchUnused(pParam2);
        AmbaMisra_TouchUnused(pParam3);
        AmbaMisra_TouchUnused(pParam4);
    }

    return RetVal;
}


static UINT32 SvcCalib_BpcCmdDataInit(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    if (pParam1 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to init bpc - input calib object should not null!" PRN_CAL_BPC_NG
    } else {
        const SVC_CALIB_OBJ_s *pCalObj;

        AmbaMisra_TypeCast(&pCalObj, &pParam1);
        if (pCalObj == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_BPC_LOG "Fail to init bpc - invalid calib obj!" PRN_CAL_BPC_NG
        } else {
            RetVal = SvcCalib_BpcShadowInit(pCalObj);
            if (RetVal != SVC_OK) {
                PRN_CAL_BPC_LOG "Fail to init bpc - shadow initial fail!" PRN_CAL_BPC_NG
            } else {
                SVC_CALIB_CALC_CB_f CbFunc = NULL;
                AmbaMisra_TypeCast(&(CbFunc), &(pParam2));
                SVC_CalibBpcCtrl.CalcCbFunc = CbFunc;
            }
        }

        AmbaMisra_TouchUnused(pParam1);
        AmbaMisra_TouchUnused(pParam2);
        AmbaMisra_TouchUnused(pParam3);
        AmbaMisra_TouchUnused(pParam4);
    }

    return RetVal;
}


static UINT32 SvcCalib_BpcCmdTblUpdate(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    if (pParam1 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to update bpc table - input calib channel should not null!" PRN_CAL_BPC_NG
    } else if (pParam2 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to update bpc table - input IK mode cfg should not null!" PRN_CAL_BPC_NG
    } else {
        const SVC_CALIB_CHANNEL_s     *pChan;
        const AMBA_IK_MODE_CFG_s      *pImgMode;

        AmbaMisra_TypeCast(&(pChan), &pParam1);
        AmbaMisra_TypeCast(&(pImgMode), &pParam2);

        RetVal = SvcCalib_BpcTableUpdate(pChan, pImgMode);

    }

    AmbaMisra_TouchUnused(pParam1);
    AmbaMisra_TouchUnused(pParam2);
    AmbaMisra_TouchUnused(pParam3);
    AmbaMisra_TouchUnused(pParam4);

    return RetVal;
}

static UINT32 SvcCalib_BpcCmdTblDataGet(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;
    UINT32 *pU32Val = NULL;
    SVC_CALIB_TBL_INFO_s *pTblInfo;

    AmbaMisra_TypeCast(&(pU32Val), &(pParam1));
    AmbaMisra_TypeCast(&(pTblInfo), &(pParam2));

    if (pU32Val == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to get table data - input table id should not null!" PRN_CAL_BPC_NG
    } else {
        RetVal = SvcCalib_BpcTableDataGet(*pU32Val, pTblInfo);
    }

    AmbaMisra_TouchUnused(pParam1);
    AmbaMisra_TouchUnused(pParam2);
    AmbaMisra_TouchUnused(pParam3);
    AmbaMisra_TouchUnused(pParam4);
    AmbaMisra_TouchUnused(pU32Val);

    return RetVal;

}

static UINT32 SvcCalib_BpcCmdTblDataSet(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;
    UINT32 *pU32Val = NULL;
    SVC_CALIB_TBL_INFO_s *pTblInfo;

    AmbaMisra_TypeCast(&(pU32Val), &(pParam1));
    AmbaMisra_TypeCast(&(pTblInfo), &(pParam2));

    if (pU32Val == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to set table data - input table id should not null!" PRN_CAL_BPC_NG
    } else {
        RetVal = SvcCalib_BpcTableDataSet(*pU32Val, pTblInfo);
    }

    AmbaMisra_TouchUnused(pParam1);
    AmbaMisra_TouchUnused(pParam2);
    AmbaMisra_TouchUnused(pParam3);
    AmbaMisra_TouchUnused(pParam4);
    AmbaMisra_TouchUnused(pU32Val);

    return RetVal;
}


UINT32 SvcCalib_BpcCmdFunc(UINT32 CmdID, void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    SvcCalib_BpcCmdHdlrInit();

    if (CmdID >= SVC_CALIB_CMD_NUM) {
        RetVal = SVC_NG;
        PRN_CAL_BPC_LOG "Fail to handler cmd - invalid command id(%d)"
            PRN_CAL_BPC_ARG_UINT32 CmdID PRN_CAL_BPC_ARG_POST
        PRN_CAL_BPC_NG
    } else {
        const UINT32 HdlrCount = (UINT32)(sizeof(SvcCalibBpcCmdHdlr)) / ((UINT32)sizeof(SvcCalibBpcCmdHdlr[0]));
        UINT32 HdlrIdx;

        for (HdlrIdx = 0U; HdlrIdx < HdlrCount; HdlrIdx ++) {
            if ((SvcCalibBpcCmdHdlr[HdlrIdx].Enable > 0U) &&
                (SvcCalibBpcCmdHdlr[HdlrIdx].CmdID == CmdID) &&
                (SvcCalibBpcCmdHdlr[HdlrIdx].pHandler != NULL)) {
                RetVal = (SvcCalibBpcCmdHdlr[HdlrIdx].pHandler)(pParam1, pParam2, pParam3, pParam4);
            }
        }
    }

    return RetVal;
}

