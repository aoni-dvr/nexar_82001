/**
*  @file SvcCalibWb.c
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
*  @details C file for Calibration White Balance
*
*/

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaVIN_Def.h"
#include "AmbaDSP_ImageUtility.h"

#include "AmbaImg_Proc.h"
#include "Amba_AwbCalib.h"

#include "SvcWrap.h"
#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcCalibMgr.h"

#define SVC_CALIB_WB_FLG_INIT           (0x1U)
#define SVC_CALIB_WB_FLG_SHADOW_INIT    (0x2U)
#define SVC_CALIB_WB_FLG_CALC_INIT      (0x4U)
#define SVC_CALIB_WB_FLG_CALC_VIN_SEN   (0x8U)
#define SVC_CALIB_WB_FLG_SHELL_INIT     (0x100U)
#define SVC_CALIB_WB_FLG_CMD_INIT       (0x200U)
/* Reserve 0x10000/0x20000/0x40000/0x80000 for SvcCalibMgr.h */

#define SVC_CALIB_WB_NAME               ("SvcCalib_Wb")
#define SVC_CALIB_WB_NAND_HEADER_SIZE   (0x200U)
#define SVC_CALIB_WB_DEF_GAIN           (4096U)

typedef struct {
    UINT32 Enable;
    UINT32 NumOfTable;
    UINT32 DebugMsgOn;
    UINT32 TableSelectBits;
    UINT32 Reserved[(SVC_CALIB_WB_NAND_HEADER_SIZE / 4U) - 4U];
} SVC_CALIB_WB_NAND_HEADER_s;

typedef struct {
    UINT32                        VinSelectBits;
    UINT32                        SensorSelectBits;
} SVC_CALIB_WB_TABLE_HEADER_s;

typedef struct {
    SVC_CALIB_WB_TABLE_HEADER_s Header;
    SVC_CALIB_GAIN_INFO_s       OriWb[SVC_CALIB_WB_CT_NUM];
    SVC_CALIB_GAIN_INFO_s       RefWb[SVC_CALIB_WB_CT_NUM];
} SVC_CALIB_WB_NAND_TABLE_s;

typedef struct {
    UINT32                     VinSelectBits;
    UINT32                     SensorSelectBits;
} SVC_CALIB_WB_TABLE_s;

typedef struct {
    UINT32                     TableIdx;
    UINT32                     CTType;                     // Color temperature type
    UINT32                     FlickerMode;
    SVC_CALIB_GAIN_INFO_s      RefWb[SVC_CALIB_WB_CT_NUM];
    UINT32                     ThresholdR;
    UINT32                     ThresholdB;
    UINT32                     SaveBinOn;
} SVC_CALIB_WB_CALC_CTRL_s;

typedef struct {
    char                          Name[16];
    AMBA_KAL_MUTEX_t              Mutex;
    SVC_CALIB_WB_NAND_HEADER_s   *pNandHeader;
    SVC_CALIB_WB_NAND_TABLE_s    *pNandTable;
    UINT32                        NumOfWbTable;
    SVC_CALIB_WB_TABLE_s         *pWbTable;
    SVC_CALIB_WB_CALC_CTRL_s      CalcCtrl;
    SVC_CALIB_CALC_CB_f           CalcCbFunc;
    SVC_CALIB_CHANNEL_s           CalibChan;
    UINT32                        EnableDebugMsg;
} SVC_CALIB_WB_CTRL_s;

static UINT32                 SvcCalib_WbMutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut);
static void                   SvcCalib_WbMutexGive(AMBA_KAL_MUTEX_t *pMutex);
static void                   SvcCalib_WbErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode);

static UINT32                 SvcCalib_WbGainCheck(const SVC_CALIB_GAIN_INFO_s *pCurGain, const SVC_CALIB_GAIN_INFO_s *pRefGain, UINT32 ThresholdR, UINT32 ThresholdB);
static UINT32                 SvcCalib_WbVinSensorIDCheck(UINT32 VinID, UINT32 SensorID);
static UINT32                 SvcCalib_WbMemQuery(UINT32 MaxTableNum, UINT32 *pShadowSize, UINT32 *pWorkMemSize, UINT32 *pCalcMemSize);
static UINT32                 SvcCalib_WbCreate(void);
static UINT32                 SvcCalib_WbShadowInit(const SVC_CALIB_OBJ_s *pCalObj);
static UINT32                 SvcCalib_WbTableCalc(const SVC_CALIB_CHANNEL_s *pCalibChan);
static UINT32                 SvcCalib_WbNandTableSearch(UINT32 VinID, UINT32 SensorID, UINT32 *pTblIdx);
static UINT32                 SvcCalib_WbGainGet(const SVC_CALIB_CHANNEL_s *pCalibChan, UINT32 FlickerMode);
static UINT32                 SvcCalib_WbTableDataGet(UINT32 TblID, SVC_CALIB_TBL_INFO_s *pTblInfo);
static UINT32                 SvcCalib_WbTableDataSet(UINT32 TblID, SVC_CALIB_TBL_INFO_s *pTblInfo);

// Private function definition for shell command.
typedef UINT32 (*SVC_CALIB_WB_SHELL_FUNC_f)(UINT32 ArgCount, char * const *pArgVector);
typedef void   (*SVC_CALIB_WB_SHELL_USAGE_f)(void);

typedef struct {
    char                       ShellCmdName[32];
    SVC_CALIB_WB_SHELL_FUNC_f  pFunc;
    SVC_CALIB_WB_SHELL_USAGE_f pUsageFunc;
} SVC_CALIB_WB_SHELL_FUNC_s;

static void   SvcCalib_WbShellStrToU32(const char *pString, UINT32 *pValue);
static UINT32 SvcCalib_WbShellCfgVinSen(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_WbShellCfgVinSenU(void);
static UINT32 SvcCalib_WbShellCfgTblID(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_WbShellCfgTblIDU(void);
static UINT32 SvcCalib_WbShellCfgDbgMsg(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_WbShellCfgDbgMsgU(void);
static UINT32 SvcCalib_WbShellGetWbGain(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_WbShellGetWbGainU(void);
static UINT32 SvcCalib_WbShellGenTbl(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_WbShellGenTblU(void);
static UINT32 SvcCalib_WbShellSet(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_WbShellSetU(void);
static void   SvcCalib_WbShellEntryInit(void);
static void   SvcCalib_WbShellUsage(void);

// Private function definition for calibration command
typedef UINT32 (*SVC_CALIB_WB_CMD_FUNC_f)(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
typedef struct {
     UINT32                  Enable;
     UINT32                  CmdID;
     SVC_CALIB_WB_CMD_FUNC_f pHandler;
} SVC_CALIB_WB_CMD_HDLR_s;

static void   SvcCalib_WbCmdHdlrInit(void);
static UINT32 SvcCalib_WbCmdMemQuery(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_WbCmdCreate(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_WbCmdDataInit(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_WbCmdTblDataGet(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_WbCmdTblDataSet(void *pParam1, void *pParam2, void *pParam3, void *pParam4);

#define SVC_CALIB_WB_SHELL_CMD_NUM  (6U)
static SVC_CALIB_WB_SHELL_FUNC_s CalibWbShellFunc[SVC_CALIB_WB_SHELL_CMD_NUM] GNU_SECTION_NOZEROINIT;
static SVC_CALIB_WB_CMD_HDLR_s SvcCalibWbCmdHdlr[SVC_CALIB_CMD_NUM] GNU_SECTION_NOZEROINIT;
static SVC_CALIB_WB_CTRL_s SVC_CalibWbCtrl GNU_SECTION_NOZEROINIT;
static UINT32 SVC_CalibWbCtrlFlag = SVC_LOG_CAL_DEF_FLG;

#define SVC_LOG_WB " WB"
#define PRN_CAL_WB_LOG        { SVC_WRAP_PRINT_s CalibWbPrint; AmbaSvcWrap_MisraMemset(&(CalibWbPrint), 0, sizeof(CalibWbPrint)); CalibWbPrint.Argc --; CalibWbPrint.pStrFmt =
#define PRN_CAL_WB_ARG_UINT32 ; CalibWbPrint.Argc ++; CalibWbPrint.Argv[CalibWbPrint.Argc].Uint64   = (UINT64)((
#define PRN_CAL_WB_ARG_CSTR   ; CalibWbPrint.Argc ++; CalibWbPrint.Argv[CalibWbPrint.Argc].pCStr    = ((
#define PRN_CAL_WB_ARG_CPOINT ; CalibWbPrint.Argc ++; CalibWbPrint.Argv[CalibWbPrint.Argc].pPointer = ((
#define PRN_CAL_WB_ARG_POST   ))
#define PRN_CAL_WB_OK         ; CalibWbPrint.Argc ++; SvcCalib_WbPrintLog(SVC_LOG_CAL_OK , &(CalibWbPrint)); }
#define PRN_CAL_WB_NG         ; CalibWbPrint.Argc ++; SvcCalib_WbPrintLog(SVC_LOG_CAL_NG , &(CalibWbPrint)); }
#define PRN_CAL_WB_API        ; CalibWbPrint.Argc ++; SvcCalib_WbPrintLog(SVC_LOG_CAL_API, &(CalibWbPrint)); }
#define PRN_CAL_WB_DBG        ; CalibWbPrint.Argc ++; SvcCalib_WbPrintLog(SVC_LOG_CAL_DBG, &(CalibWbPrint)); }
#define PRN_CAL_WB_ERR_HDLR   SvcCalib_WbErrHdlr(__func__, __LINE__, PRetVal);

static void SvcCalib_WbPrintLog(UINT32 LogLevel, SVC_WRAP_PRINT_s *pPrint)
{
    if (pPrint != NULL) {
        if ((SVC_CalibWbCtrlFlag & LogLevel) > 0U) {

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

            SvcWrap_Print(SVC_LOG_WB, pPrint);
        }
    }
}

static UINT32 SvcCalib_WbMutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut)
{
    return AmbaKAL_MutexTake(pMutex, TimeOut);
}

static void SvcCalib_WbMutexGive(AMBA_KAL_MUTEX_t *pMutex)
{
    UINT32 ErrCode = AmbaKAL_MutexGive(pMutex);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }
}

static void SvcCalib_WbErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode)
{
    if (pCaller != NULL) {
        if (ErrCode != 0U) {
            PRN_CAL_WB_LOG "Catch ErrCode(0x%08x) @ %s, %d"
                PRN_CAL_WB_ARG_UINT32 ErrCode  PRN_CAL_WB_ARG_POST
                PRN_CAL_WB_ARG_CSTR   pCaller  PRN_CAL_WB_ARG_POST
                PRN_CAL_WB_ARG_UINT32 CodeLine PRN_CAL_WB_ARG_POST
            PRN_CAL_WB_NG
        }
    }
}

static UINT32 SvcCalib_WbMemQuery(UINT32 MaxTableNum, UINT32 *pShadowSize, UINT32 *pWorkMemSize, UINT32 *pCalcMemSize)
{
    UINT32 RetVal = SVC_OK;

    UINT32 DramShadowSize, WorkingMemSize, CalcMemSize = 0;
    UINT32 DramShadowHeaderSize = (UINT32)(sizeof(SVC_CALIB_WB_NAND_HEADER_s));
    UINT32 DramShadowTableSize  = (UINT32)(sizeof(SVC_CALIB_WB_NAND_TABLE_s));
    UINT32 DramShadowDataSize   = DramShadowTableSize * MaxTableNum;
    UINT32 WbTableSize          = (UINT32)(sizeof(SVC_CALIB_WB_TABLE_s));
    UINT32 TotalWbTableSize     = WbTableSize * MaxTableNum;

    DramShadowSize  = DramShadowHeaderSize;
    DramShadowSize += DramShadowDataSize;
    WorkingMemSize  = TotalWbTableSize;

    PRN_CAL_WB_LOG " " PRN_CAL_WB_DBG
    PRN_CAL_WB_LOG "====== Calib WB Memory Query ======" PRN_CAL_WB_DBG
    PRN_CAL_WB_LOG "  MaxTableNum    : %d"
        PRN_CAL_WB_ARG_UINT32 MaxTableNum PRN_CAL_WB_ARG_POST
    PRN_CAL_WB_DBG
    PRN_CAL_WB_LOG "  DramShadowSize : 0x%X"
        PRN_CAL_WB_ARG_UINT32 DramShadowSize PRN_CAL_WB_ARG_POST
    PRN_CAL_WB_DBG
    PRN_CAL_WB_LOG "    DramShadowHeaderSize : 0x%X"
        PRN_CAL_WB_ARG_UINT32 DramShadowHeaderSize PRN_CAL_WB_ARG_POST
    PRN_CAL_WB_DBG
    PRN_CAL_WB_LOG "    DramShadowDataSize   : 0x%X ( 0x%X x %d )"
        PRN_CAL_WB_ARG_UINT32 DramShadowDataSize  PRN_CAL_WB_ARG_POST
        PRN_CAL_WB_ARG_UINT32 DramShadowTableSize PRN_CAL_WB_ARG_POST
        PRN_CAL_WB_ARG_UINT32 MaxTableNum         PRN_CAL_WB_ARG_POST
    PRN_CAL_WB_DBG
    PRN_CAL_WB_LOG "  WorkingMemSize : 0x%X"
        PRN_CAL_WB_ARG_UINT32 WorkingMemSize PRN_CAL_WB_ARG_POST
    PRN_CAL_WB_DBG
    PRN_CAL_WB_LOG "    TotalWbTableSize  : 0x%X ( 0x%X x %d )"
        PRN_CAL_WB_ARG_UINT32 TotalWbTableSize PRN_CAL_WB_ARG_POST
        PRN_CAL_WB_ARG_UINT32 WbTableSize      PRN_CAL_WB_ARG_POST
        PRN_CAL_WB_ARG_UINT32 MaxTableNum      PRN_CAL_WB_ARG_POST
    PRN_CAL_WB_DBG
    PRN_CAL_WB_LOG "  CalcMemSize : 0x%X"
        PRN_CAL_WB_ARG_UINT32 CalcMemSize PRN_CAL_WB_ARG_POST
    PRN_CAL_WB_DBG

    if (pShadowSize != NULL) {
        *pShadowSize  = DramShadowSize;
    }
    PRN_CAL_WB_LOG "Query the calib wb dram shadow size 0x%X ( table num %d )"
        PRN_CAL_WB_ARG_UINT32 DramShadowSize PRN_CAL_WB_ARG_POST
        PRN_CAL_WB_ARG_UINT32 MaxTableNum    PRN_CAL_WB_ARG_POST
    PRN_CAL_WB_DBG

    if (pWorkMemSize != NULL) {
        *pWorkMemSize = WorkingMemSize;
    }
    PRN_CAL_WB_LOG "Query the calib wb working memory size 0x%X ( table num %d )"
        PRN_CAL_WB_ARG_UINT32 WorkingMemSize PRN_CAL_WB_ARG_POST
        PRN_CAL_WB_ARG_UINT32 MaxTableNum    PRN_CAL_WB_ARG_POST
    PRN_CAL_WB_DBG

    if (pCalcMemSize != NULL) {
        *pCalcMemSize = CalcMemSize;
    }
    PRN_CAL_WB_LOG "Query the calib wb algo. memory size 0x%X ( table num %d )"
        PRN_CAL_WB_ARG_UINT32 CalcMemSize PRN_CAL_WB_ARG_POST
        PRN_CAL_WB_ARG_UINT32 MaxTableNum PRN_CAL_WB_ARG_POST
    PRN_CAL_WB_DBG

    return RetVal;
}


static UINT32 SvcCalib_WbCreate(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibWbCtrlFlag & SVC_CALIB_WB_FLG_INIT) > 0U) {
        RetVal = SVC_OK;
        PRN_CAL_WB_LOG "Calibration WB module has been created!" PRN_CAL_WB_DBG
    } else {
        // Reset the wb control module
        AmbaSvcWrap_MisraMemset(&SVC_CalibWbCtrl, 0, sizeof(SVC_CalibWbCtrl));

        // Configure the name
        SvcWrap_strcpy(SVC_CalibWbCtrl.Name, sizeof(SVC_CalibWbCtrl.Name), SVC_CALIB_WB_NAME);

        // Create the mutex
        RetVal = AmbaKAL_MutexCreate(&(SVC_CalibWbCtrl.Mutex), SVC_CalibWbCtrl.Name);
        if (RetVal != SVC_OK) {
            PRN_CAL_WB_LOG "Fail to create wb - create mutex fail!" PRN_CAL_WB_NG
        } else {
            PRetVal = (SVC_CalibWbCtrlFlag & (SVC_CALIB_WB_FLG_SHELL_INIT |
                                              SVC_CALIB_WB_FLG_CMD_INIT   |
                                              SVC_LOG_CAL_DEF_FLG         |
                                              SVC_LOG_CAL_DBG));
            SVC_CalibWbCtrlFlag = PRetVal;
            SVC_CalibWbCtrlFlag |= SVC_CALIB_WB_FLG_INIT;
            PRN_CAL_WB_LOG "Successful to create wb!" PRN_CAL_WB_OK
        }
    }

    return RetVal;
}


static UINT32 SvcCalib_WbShadowInit(const SVC_CALIB_OBJ_s *pCalObj)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibWbCtrlFlag & SVC_CALIB_WB_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to initial wb dram shodow - create wb first!" PRN_CAL_WB_NG
    } else if (pCalObj == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to initial wb dram shodow - invalid calib object!" PRN_CAL_WB_NG
    } else if (pCalObj->Enable == 0U) {
        RetVal = 99;
        PRN_CAL_WB_LOG "Disable wb dram shadow!!" PRN_CAL_WB_API
    } else {
        UINT32 DramShadowSize = 0, WorkingMemSize = 0;
        const SVC_CALIB_WB_NAND_HEADER_s *pHeader;
        AmbaMisra_TypeCast(&(pHeader), &(pCalObj->pShadowBuf));

        RetVal = SvcCalib_WbMemQuery(pCalObj->NumOfTable, &DramShadowSize, &WorkingMemSize, NULL);
        if (RetVal != SVC_OK) {
            PRN_CAL_WB_LOG "Fail to initial wb dram shadow - query memory fail!" PRN_CAL_WB_NG
        } else if ((pCalObj->pShadowBuf == NULL) || (pHeader == NULL)) {
            RetVal = SVC_NG;
            PRN_CAL_WB_LOG "Fail to initial wb dram shadow - dram shadow buffer should not null!" PRN_CAL_WB_NG
        } else if (pCalObj->ShadowBufSize < DramShadowSize) {
            RetVal = SVC_NG;
            PRN_CAL_WB_LOG "Fail to initial wb dram shadow - prepare dram shadow size is too small!" PRN_CAL_WB_NG
        } else if (pCalObj->pWorkingBuf == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_WB_LOG "Fail to initial wb dram shadow - working buffer should not null!" PRN_CAL_WB_NG
        } else if (pCalObj->WorkingBufSize < WorkingMemSize) {
            RetVal = SVC_NG;
            PRN_CAL_WB_LOG "Fail to initial wb dram shadow - working buffer size is too small!" PRN_CAL_WB_NG
        } else if (pHeader->NumOfTable > pCalObj->NumOfTable) {
            RetVal = SVC_NG;
            PRN_CAL_WB_LOG "Fail to initial wb dram shadow - calib nand table count(%d) > max wb dram shadow table count(%d)"
                PRN_CAL_WB_ARG_UINT32 pHeader->NumOfTable PRN_CAL_WB_ARG_POST
                PRN_CAL_WB_ARG_UINT32 pCalObj->NumOfTable PRN_CAL_WB_ARG_POST
            PRN_CAL_WB_NG
        } else {
            if (KAL_ERR_NONE != SvcCalib_WbMutexTake(&(SVC_CalibWbCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
                RetVal = SVC_NG;
                PRN_CAL_WB_LOG "Fail to initial wb dram shadow - take mutex fail!" PRN_CAL_WB_NG
            } else {
                UINT8 *pNandHeaderBuf    = pCalObj->pShadowBuf;
                UINT32 NandHeaderSize    = (UINT32)(sizeof(SVC_CALIB_WB_NAND_HEADER_s));
                UINT8 *pNandTableBuf = &(pNandHeaderBuf[NandHeaderSize]);
                UINT32 TotalWbTableSize  = (UINT32)(sizeof(SVC_CALIB_WB_TABLE_s)) * pCalObj->NumOfTable;

                PRN_CAL_WB_LOG "====== Shadow initialized Start ======" PRN_CAL_WB_DBG
                PRN_CAL_WB_LOG "  DramShadow Addr  : %p 0x%X"
                    PRN_CAL_WB_ARG_CPOINT pCalObj->pShadowBuf    PRN_CAL_WB_ARG_POST
                    PRN_CAL_WB_ARG_UINT32 pCalObj->ShadowBufSize PRN_CAL_WB_ARG_POST
                PRN_CAL_WB_DBG
                PRN_CAL_WB_LOG "    Nand Header Addr : %p 0x%X"
                    PRN_CAL_WB_ARG_CPOINT pNandHeaderBuf PRN_CAL_WB_ARG_POST
                    PRN_CAL_WB_ARG_UINT32 NandHeaderSize PRN_CAL_WB_ARG_POST
                PRN_CAL_WB_DBG
                PRN_CAL_WB_LOG "    Nand Table Addr  : %p"
                    PRN_CAL_WB_ARG_CPOINT pNandTableBuf PRN_CAL_WB_ARG_POST
                PRN_CAL_WB_DBG
                PRN_CAL_WB_LOG "  Working Mem Addr : %p 0x%X"
                    PRN_CAL_WB_ARG_CPOINT pCalObj->pWorkingBuf    PRN_CAL_WB_ARG_POST
                    PRN_CAL_WB_ARG_UINT32 pCalObj->WorkingBufSize PRN_CAL_WB_ARG_POST
                PRN_CAL_WB_DBG
                PRN_CAL_WB_LOG "    wb Table Addr    : %p 0x%08X"
                    PRN_CAL_WB_ARG_CPOINT pCalObj->pWorkingBuf PRN_CAL_WB_ARG_POST
                    PRN_CAL_WB_ARG_UINT32 TotalWbTableSize     PRN_CAL_WB_ARG_POST
                PRN_CAL_WB_DBG

                // Configure nand header
                AmbaMisra_TypeCast(&(SVC_CalibWbCtrl.pNandHeader), &(pNandHeaderBuf));
                // Configure nand table
                AmbaMisra_TypeCast(&(SVC_CalibWbCtrl.pNandTable),  &(pNandTableBuf)); AmbaMisra_TouchUnused(pNandTableBuf);
                // Configure working wb table
                AmbaMisra_TypeCast(&(SVC_CalibWbCtrl.pWbTable),  &(pCalObj->pWorkingBuf));
                if (SVC_CalibWbCtrl.pNandHeader == NULL) {
                    RetVal = SVC_NG;
                    PRN_CAL_WB_LOG "Fail to initial wb dram shadow - nand header should not null!" PRN_CAL_WB_NG
                } else if (SVC_CalibWbCtrl.pNandTable == NULL) {
                    RetVal = SVC_NG;
                    PRN_CAL_WB_LOG "Fail to initial wb dram shadow - nand table should not null!" PRN_CAL_WB_NG
                } else if (SVC_CalibWbCtrl.pWbTable == NULL) {
                    RetVal = SVC_NG;
                    PRN_CAL_WB_LOG "Fail to initial wb dram shadow - wb table should not null!" PRN_CAL_WB_NG
                } else {
                    SVC_CalibWbCtrl.NumOfWbTable = pCalObj->NumOfTable;
                    // Reset the working wb table memory
                    AmbaSvcWrap_MisraMemset(SVC_CalibWbCtrl.pWbTable, 0, TotalWbTableSize);

                    if (SVC_CalibWbCtrl.pNandHeader->DebugMsgOn > 0U) {
                        SVC_CalibWbCtrl.EnableDebugMsg |= SVC_CalibWbCtrl.pNandHeader->DebugMsgOn;
                    }

                    if (SVC_CalibWbCtrl.EnableDebugMsg > 0U) {
                        UINT32 Idx;

                        PRN_CAL_WB_LOG " " PRN_CAL_WB_API
                        PRN_CAL_WB_LOG "---- Wb Nand Info %d ----"
                            PRN_CAL_WB_ARG_UINT32 SVC_CalibWbCtrl.pNandHeader->Reserved[0] PRN_CAL_WB_ARG_POST
                        PRN_CAL_WB_API
                        PRN_CAL_WB_LOG "  Enable          : %d"
                            PRN_CAL_WB_ARG_UINT32 SVC_CalibWbCtrl.pNandHeader->Enable PRN_CAL_WB_ARG_POST
                        PRN_CAL_WB_API
                        PRN_CAL_WB_LOG "  NumOfTable      : %d"
                            PRN_CAL_WB_ARG_UINT32 SVC_CalibWbCtrl.pNandHeader->NumOfTable PRN_CAL_WB_ARG_POST
                        PRN_CAL_WB_API
                        PRN_CAL_WB_LOG "  DebugMsgOn      : %d"
                            PRN_CAL_WB_ARG_UINT32 SVC_CalibWbCtrl.pNandHeader->DebugMsgOn PRN_CAL_WB_ARG_POST
                        PRN_CAL_WB_API
                        PRN_CAL_WB_LOG "  TableSelectBits : 0x%X"
                            PRN_CAL_WB_ARG_UINT32 SVC_CalibWbCtrl.pNandHeader->TableSelectBits PRN_CAL_WB_ARG_POST
                        PRN_CAL_WB_API

                        {
                            const SVC_CALIB_WB_NAND_TABLE_s *pNandTable;

                            for (Idx = 0U; Idx < pCalObj->NumOfTable; Idx ++) {
                                pNandTable = &(SVC_CalibWbCtrl.pNandTable[Idx]);

                                PRN_CAL_WB_LOG "  ---- Nand Table[%d] %p -----"
                                    PRN_CAL_WB_ARG_UINT32 Idx        PRN_CAL_WB_ARG_POST
                                    PRN_CAL_WB_ARG_CPOINT pNandTable PRN_CAL_WB_ARG_POST
                                PRN_CAL_WB_API
                                PRN_CAL_WB_LOG "    VinSelectBits       : 0x%X"
                                    PRN_CAL_WB_ARG_UINT32 pNandTable->Header.VinSelectBits PRN_CAL_WB_ARG_POST
                                PRN_CAL_WB_API
                                PRN_CAL_WB_LOG "    SensorSelectBits    : 0x%X"
                                    PRN_CAL_WB_ARG_UINT32 pNandTable->Header.SensorSelectBits PRN_CAL_WB_ARG_POST
                                PRN_CAL_WB_API
                                PRN_CAL_WB_LOG "    OriWb[LCT]          : %d %d %d"
                                    PRN_CAL_WB_ARG_UINT32 pNandTable->OriWb[SVC_CALIB_WB_LCT].GainR PRN_CAL_WB_ARG_POST
                                    PRN_CAL_WB_ARG_UINT32 pNandTable->OriWb[SVC_CALIB_WB_LCT].GainG PRN_CAL_WB_ARG_POST
                                    PRN_CAL_WB_ARG_UINT32 pNandTable->OriWb[SVC_CALIB_WB_LCT].GainB PRN_CAL_WB_ARG_POST
                                PRN_CAL_WB_API
                                PRN_CAL_WB_LOG "    OriWb[HCT]          : %d %d %d"
                                    PRN_CAL_WB_ARG_UINT32 pNandTable->OriWb[SVC_CALIB_WB_HCT].GainR PRN_CAL_WB_ARG_POST
                                    PRN_CAL_WB_ARG_UINT32 pNandTable->OriWb[SVC_CALIB_WB_HCT].GainG PRN_CAL_WB_ARG_POST
                                    PRN_CAL_WB_ARG_UINT32 pNandTable->OriWb[SVC_CALIB_WB_HCT].GainB PRN_CAL_WB_ARG_POST
                                PRN_CAL_WB_API
                                PRN_CAL_WB_LOG "    RefWb[LCT]          : %d %d %d"
                                    PRN_CAL_WB_ARG_UINT32 pNandTable->RefWb[SVC_CALIB_WB_LCT].GainR PRN_CAL_WB_ARG_POST
                                    PRN_CAL_WB_ARG_UINT32 pNandTable->RefWb[SVC_CALIB_WB_LCT].GainG PRN_CAL_WB_ARG_POST
                                    PRN_CAL_WB_ARG_UINT32 pNandTable->RefWb[SVC_CALIB_WB_LCT].GainB PRN_CAL_WB_ARG_POST
                                PRN_CAL_WB_API
                                PRN_CAL_WB_LOG "    RefWb[HCT]          : %d %d %d"
                                    PRN_CAL_WB_ARG_UINT32 pNandTable->RefWb[SVC_CALIB_WB_HCT].GainR PRN_CAL_WB_ARG_POST
                                    PRN_CAL_WB_ARG_UINT32 pNandTable->RefWb[SVC_CALIB_WB_HCT].GainG PRN_CAL_WB_ARG_POST
                                    PRN_CAL_WB_ARG_UINT32 pNandTable->RefWb[SVC_CALIB_WB_HCT].GainB PRN_CAL_WB_ARG_POST
                                PRN_CAL_WB_API

                            }
                        }

                        PRN_CAL_WB_LOG " " PRN_CAL_WB_API
                        PRN_CAL_WB_LOG "---- Wb Table Info ----" PRN_CAL_WB_API
                        PRN_CAL_WB_LOG "  NumOfTable     : %d"
                            PRN_CAL_WB_ARG_UINT32 SVC_CalibWbCtrl.NumOfWbTable PRN_CAL_WB_ARG_POST
                        PRN_CAL_WB_API

                        {
                            const SVC_CALIB_WB_TABLE_s *pWbTable;

                            for (Idx = 0U; Idx < SVC_CalibWbCtrl.NumOfWbTable; Idx ++) {
                                pWbTable = &(SVC_CalibWbCtrl.pWbTable[Idx]);

                                PRN_CAL_WB_LOG "  ---- Wb Table[%d] %p -----"
                                    PRN_CAL_WB_ARG_UINT32 Idx      PRN_CAL_WB_ARG_POST
                                    PRN_CAL_WB_ARG_CPOINT pWbTable PRN_CAL_WB_ARG_POST
                                PRN_CAL_WB_API
                                PRN_CAL_WB_LOG "    VinSelectBits       : 0x%X"
                                    PRN_CAL_WB_ARG_UINT32 SVC_CalibWbCtrl.pWbTable[Idx].VinSelectBits PRN_CAL_WB_ARG_POST
                                PRN_CAL_WB_API
                                PRN_CAL_WB_LOG "    SensorSelectBits    : 0x%X"
                                    PRN_CAL_WB_ARG_UINT32 SVC_CalibWbCtrl.pWbTable[Idx].SensorSelectBits PRN_CAL_WB_ARG_POST
                                PRN_CAL_WB_API
                            }
                        }
                    }

                    SVC_CalibWbCtrlFlag |= SVC_CALIB_WB_FLG_SHADOW_INIT;
                    PRN_CAL_WB_LOG "Successful to initial wb dram shadow!" PRN_CAL_WB_OK
                }

                SvcCalib_WbMutexGive(&(SVC_CalibWbCtrl.Mutex));
            }
        }
    }

    return RetVal;
}


static UINT32 SvcCalib_WbGainCheck(const SVC_CALIB_GAIN_INFO_s *pCurGain, const SVC_CALIB_GAIN_INFO_s *pRefGain, UINT32 ThresholdR, UINT32 ThresholdB)
{
    UINT32 RetVal = SVC_OK;

    if (pCurGain == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to check wb gain value - current WB gain should not null!" PRN_CAL_WB_NG
    } else if ((pCurGain->GainR == 0U) ||
               (pCurGain->GainG == 0U) ||
               (pCurGain->GainB == 0U)) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to calc wb table - get invalid gain value! %d/%d/%d"
            PRN_CAL_WB_ARG_UINT32 pCurGain->GainR PRN_CAL_WB_ARG_POST
            PRN_CAL_WB_ARG_UINT32 pCurGain->GainG PRN_CAL_WB_ARG_POST
            PRN_CAL_WB_ARG_UINT32 pCurGain->GainB PRN_CAL_WB_ARG_POST
        PRN_CAL_WB_NG
    } else if (pRefGain == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to check wb gain value - reference WB gain should not null!" PRN_CAL_WB_NG
    } else {
        UINT32 Delta;

        if (pRefGain->GainR > pCurGain->GainR) {
            Delta = pRefGain->GainR - pCurGain->GainR;
        } else {
            Delta = pCurGain->GainR - pRefGain->GainR;
        }

        if (Delta > ThresholdR) {
            RetVal = SVC_NG;
            PRN_CAL_WB_LOG "The gain delta R is out-of threshold!" PRN_CAL_WB_NG
        } else {
            if (pRefGain->GainB > pCurGain->GainB) {
                Delta = pRefGain->GainB - pCurGain->GainB;
            } else {
                Delta = pCurGain->GainB - pRefGain->GainB;
            }

            if (Delta > ThresholdB) {
                RetVal = SVC_NG;
                PRN_CAL_WB_LOG "The gain delta B is out-of threshold!" PRN_CAL_WB_NG
            }
        }
    }

    return RetVal;
}


static UINT32 SvcCalib_WbVinSensorIDCheck(UINT32 VinID, UINT32 SensorID)
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


static UINT32 SvcCalib_WbTableCalc(const SVC_CALIB_CHANNEL_s *pCalibChan)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibWbCtrlFlag & SVC_CALIB_WB_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to calc wb table - Need to initial wb control module first!" PRN_CAL_WB_NG
    } else if ((SVC_CalibWbCtrlFlag & SVC_CALIB_WB_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to calc wb table - Need to initial wb dram shadow first!" PRN_CAL_WB_NG
    } else if (SVC_CalibWbCtrl.pNandHeader == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to calc wb table - invalid nand header!" PRN_CAL_WB_NG
    } else if (SVC_CalibWbCtrl.pNandTable == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to calc wb table - invalid nand table!" PRN_CAL_WB_NG
    } else if (pCalibChan == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to calc wb table - invalid calib channel!" PRN_CAL_WB_NG
    } else if (SVC_OK != SvcCalib_WbVinSensorIDCheck(pCalibChan->VinID, pCalibChan->SensorID)) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to calc wb table - invalid calib channel -> VinID(%d), SensorID(%d)!"
            PRN_CAL_WB_ARG_UINT32 pCalibChan->VinID    PRN_CAL_WB_ARG_POST
            PRN_CAL_WB_ARG_UINT32 pCalibChan->SensorID PRN_CAL_WB_ARG_POST
        PRN_CAL_WB_NG
    } else if (SVC_CalibWbCtrl.CalcCbFunc == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to calc wb table - calculate callback function should not null!" PRN_CAL_WB_NG
    } else {
        void *pExtData = NULL;

        AmbaMisra_TypeCast(&(pExtData), &(pCalibChan));
        PRetVal = (SVC_CalibWbCtrl.CalcCbFunc)(SVC_CALIB_WB_ID, SVC_CALIB_CALC_START, pExtData);  PRN_CAL_WB_ERR_HDLR

        if (KAL_ERR_NONE != SvcCalib_WbMutexTake(&(SVC_CalibWbCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
            RetVal = SVC_NG;
            PRN_CAL_WB_LOG "Fail to update wb table - take mutex fail!" PRN_CAL_WB_NG
        } else {
            SVC_CALIB_CALC_WB_INFO_s WbInfo;

            AmbaSvcWrap_MisraMemset(&WbInfo, 0, sizeof(WbInfo));
            WbInfo.CalibChan.VinID            = pCalibChan->VinID;
            WbInfo.CalibChan.SensorID         = pCalibChan->SensorID;
            WbInfo.CalibChan.VinSelectBits    = pCalibChan->VinSelectBits;
            WbInfo.CalibChan.SensorSelectBits = pCalibChan->SensorSelectBits;
            WbInfo.CalibChan.ExtendData       = pCalibChan->ExtendData;
            if (SVC_CalibWbCtrl.CalcCtrl.CTType == SVC_CALIB_WB_LCT) {
                WbInfo.WbIndex = WB_CAL_LCT;
            } else {
                WbInfo.WbIndex = WB_CAL_HCT;
            }
            WbInfo.FlickerMode = SVC_CalibWbCtrl.CalcCtrl.FlickerMode;
            RetVal = (SVC_CalibWbCtrl.CalcCbFunc)(SVC_CALIB_WB_ID, SVC_CALIB_CALC_WB_INFO, &WbInfo);
            if (RetVal != SVC_OK) {
                PRN_CAL_WB_LOG "Fail to calc wb table - get wb info fail!" PRN_CAL_WB_NG
            } else {
                const SVC_CALIB_GAIN_INFO_s *pRefGain;

                if (SVC_CalibWbCtrl.CalcCtrl.CTType == SVC_CALIB_WB_LCT) {
                    pRefGain = &(SVC_CalibWbCtrl.CalcCtrl.RefWb[SVC_CALIB_WB_LCT]);
                } else {
                    pRefGain = &(SVC_CalibWbCtrl.CalcCtrl.RefWb[SVC_CALIB_WB_HCT]);
                }

                RetVal = SvcCalib_WbGainCheck(&(WbInfo.CurGain), pRefGain, SVC_CalibWbCtrl.CalcCtrl.ThresholdR, SVC_CalibWbCtrl.CalcCtrl.ThresholdB);
                if (RetVal != SVC_OK) {
                    PRN_CAL_WB_LOG "Fail to calc wb table - check the gain value fail" PRN_CAL_WB_NG
                } else if (SVC_CalibWbCtrl.CalcCtrl.TableIdx >= 32U) {
                    RetVal = SVC_NG;
                    PRN_CAL_WB_LOG "Fail to calc wb table - table id(%d) is out-of range"
                        PRN_CAL_WB_ARG_UINT32 SVC_CalibWbCtrl.CalcCtrl.TableIdx PRN_CAL_WB_ARG_POST
                    PRN_CAL_WB_NG
                } else {
                    SVC_CALIB_WB_NAND_HEADER_s *pNandHeader = SVC_CalibWbCtrl.pNandHeader;
                    SVC_CALIB_WB_NAND_TABLE_s  *pNandTable = &(SVC_CalibWbCtrl.pNandTable[SVC_CalibWbCtrl.CalcCtrl.TableIdx]);

                    pNandHeader->Enable = 1;
                    pNandHeader->DebugMsgOn = SVC_CalibWbCtrl.EnableDebugMsg;
                    if ((pNandHeader->TableSelectBits & SvcCalib_BitGet(SVC_CalibWbCtrl.CalcCtrl.TableIdx)) == 0U) {
                        pNandHeader->TableSelectBits |= SvcCalib_BitGet(SVC_CalibWbCtrl.CalcCtrl.TableIdx);
                        pNandHeader->NumOfTable ++;
                    }

                    pNandTable->Header.VinSelectBits    = pCalibChan->VinSelectBits;
                    pNandTable->Header.SensorSelectBits = pCalibChan->SensorSelectBits;
                    if (SVC_CalibWbCtrl.CalcCtrl.CTType == SVC_CALIB_WB_LCT) {
                        pNandTable->OriWb[SVC_CALIB_WB_LCT].GainR = WbInfo.CurGain.GainR;
                        pNandTable->OriWb[SVC_CALIB_WB_LCT].GainG = WbInfo.CurGain.GainG;
                        pNandTable->OriWb[SVC_CALIB_WB_LCT].GainB = WbInfo.CurGain.GainB;
                        pNandTable->RefWb[SVC_CALIB_WB_LCT].GainR = SVC_CalibWbCtrl.CalcCtrl.RefWb[SVC_CALIB_WB_LCT].GainR;
                        pNandTable->RefWb[SVC_CALIB_WB_LCT].GainG = SVC_CalibWbCtrl.CalcCtrl.RefWb[SVC_CALIB_WB_LCT].GainG;
                        pNandTable->RefWb[SVC_CALIB_WB_LCT].GainB = SVC_CalibWbCtrl.CalcCtrl.RefWb[SVC_CALIB_WB_LCT].GainB;
                    } else {
                        pNandTable->OriWb[SVC_CALIB_WB_HCT].GainR = WbInfo.CurGain.GainR;
                        pNandTable->OriWb[SVC_CALIB_WB_HCT].GainG = WbInfo.CurGain.GainG;
                        pNandTable->OriWb[SVC_CALIB_WB_HCT].GainB = WbInfo.CurGain.GainB;
                        pNandTable->RefWb[SVC_CALIB_WB_HCT].GainR = SVC_CalibWbCtrl.CalcCtrl.RefWb[SVC_CALIB_WB_HCT].GainR;
                        pNandTable->RefWb[SVC_CALIB_WB_HCT].GainG = SVC_CalibWbCtrl.CalcCtrl.RefWb[SVC_CALIB_WB_HCT].GainG;
                        pNandTable->RefWb[SVC_CALIB_WB_HCT].GainB = SVC_CalibWbCtrl.CalcCtrl.RefWb[SVC_CALIB_WB_HCT].GainB;
                    }

                    if (SVC_CalibWbCtrl.EnableDebugMsg > 0U) {
                        PRN_CAL_WB_LOG " " PRN_CAL_WB_API
                        PRN_CAL_WB_LOG "====== Calibration wb info ======" PRN_CAL_WB_API
                        PRN_CAL_WB_LOG "  Table ID         : %d"
                            PRN_CAL_WB_ARG_UINT32 SVC_CalibWbCtrl.CalcCtrl.TableIdx PRN_CAL_WB_ARG_POST
                        PRN_CAL_WB_API
                        PRN_CAL_WB_LOG "  VinSelectBits    : 0x%X"
                            PRN_CAL_WB_ARG_UINT32 pNandTable->Header.VinSelectBits PRN_CAL_WB_ARG_POST
                        PRN_CAL_WB_API
                        PRN_CAL_WB_LOG "  SensorSelectBits : 0x%X"
                            PRN_CAL_WB_ARG_UINT32 pNandTable->Header.SensorSelectBits PRN_CAL_WB_ARG_POST
                        PRN_CAL_WB_API
                        if (SVC_CalibWbCtrl.CalcCtrl.CTType == SVC_CALIB_WB_LCT) {
                            PRN_CAL_WB_LOG "  ---- Low Color Temperature ----" PRN_CAL_WB_API
                            PRN_CAL_WB_LOG "    OriWb.GainR    : %d"
                                PRN_CAL_WB_ARG_UINT32 pNandTable->OriWb[SVC_CALIB_WB_LCT].GainR PRN_CAL_WB_ARG_POST
                            PRN_CAL_WB_API
                            PRN_CAL_WB_LOG "    OriWb.GainG    : %d"
                                PRN_CAL_WB_ARG_UINT32 pNandTable->OriWb[SVC_CALIB_WB_LCT].GainG PRN_CAL_WB_ARG_POST
                            PRN_CAL_WB_API
                            PRN_CAL_WB_LOG "    OriWb.GainB    : %d"
                                PRN_CAL_WB_ARG_UINT32 pNandTable->OriWb[SVC_CALIB_WB_LCT].GainB PRN_CAL_WB_ARG_POST
                            PRN_CAL_WB_API
                            PRN_CAL_WB_LOG "    RefWb.GainR    : %d"
                                PRN_CAL_WB_ARG_UINT32 pNandTable->RefWb[SVC_CALIB_WB_LCT].GainR PRN_CAL_WB_ARG_POST
                            PRN_CAL_WB_API
                            PRN_CAL_WB_LOG "    RefWb.GainG    : %d"
                                PRN_CAL_WB_ARG_UINT32 pNandTable->RefWb[SVC_CALIB_WB_LCT].GainG PRN_CAL_WB_ARG_POST
                            PRN_CAL_WB_API
                            PRN_CAL_WB_LOG "    RefWb.GainB    : %d"
                                PRN_CAL_WB_ARG_UINT32 pNandTable->RefWb[SVC_CALIB_WB_LCT].GainB PRN_CAL_WB_ARG_POST
                            PRN_CAL_WB_API
                        } else {
                            PRN_CAL_WB_LOG "  ---- High Color Temperature ----" PRN_CAL_WB_API
                            PRN_CAL_WB_LOG "    OriWb.GainR    : %d"
                                PRN_CAL_WB_ARG_UINT32 pNandTable->OriWb[SVC_CALIB_WB_HCT].GainR PRN_CAL_WB_ARG_POST
                            PRN_CAL_WB_API
                            PRN_CAL_WB_LOG "    OriWb.GainG    : %d"
                                PRN_CAL_WB_ARG_UINT32 pNandTable->OriWb[SVC_CALIB_WB_HCT].GainG PRN_CAL_WB_ARG_POST
                            PRN_CAL_WB_API
                            PRN_CAL_WB_LOG "    OriWb.GainB    : %d"
                                PRN_CAL_WB_ARG_UINT32 pNandTable->OriWb[SVC_CALIB_WB_HCT].GainB PRN_CAL_WB_ARG_POST
                            PRN_CAL_WB_API
                            PRN_CAL_WB_LOG "    RefWb.GainR    : %d"
                                PRN_CAL_WB_ARG_UINT32 pNandTable->RefWb[SVC_CALIB_WB_HCT].GainR PRN_CAL_WB_ARG_POST
                            PRN_CAL_WB_API
                            PRN_CAL_WB_LOG "    RefWb.GainG    : %d"
                                PRN_CAL_WB_ARG_UINT32 pNandTable->RefWb[SVC_CALIB_WB_HCT].GainG PRN_CAL_WB_ARG_POST
                            PRN_CAL_WB_API
                            PRN_CAL_WB_LOG "    RefWb.GainB    : %d"
                                PRN_CAL_WB_ARG_UINT32 pNandTable->RefWb[SVC_CALIB_WB_HCT].GainB PRN_CAL_WB_ARG_POST
                            PRN_CAL_WB_API
                        }
                    }

                    PRN_CAL_WB_LOG "Successful to calc wb table" PRN_CAL_WB_OK

                    /* Update to NAND */
                    RetVal = SvcCalib_DataSave(SVC_CALIB_WB_ID);
                    if (RetVal != OK) {
                        PRN_CAL_WB_LOG "Fail to save wb to nand!" PRN_CAL_WB_NG
                    }
                }
            }

            SvcCalib_WbMutexGive(&(SVC_CalibWbCtrl.Mutex));
        }

        PRetVal = (SVC_CalibWbCtrl.CalcCbFunc)(SVC_CALIB_WB_ID, SVC_CALIB_CALC_DONE, NULL);  PRN_CAL_WB_ERR_HDLR
    }

    return RetVal;
}


static UINT32 SvcCalib_WbNandTableSearch(UINT32 VinID, UINT32 SensorID, UINT32 *pTblIdx)
{
    UINT32 RetVal = SVC_OK;

    if (SVC_OK != SvcCalib_WbVinSensorIDCheck(VinID, SensorID)) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to search wb nand table - invalid calib channel -> VinID(%d), SensorID(%d)!"
            PRN_CAL_WB_ARG_UINT32 VinID    PRN_CAL_WB_ARG_POST
            PRN_CAL_WB_ARG_UINT32 SensorID PRN_CAL_WB_ARG_POST
        PRN_CAL_WB_NG
    } else {
        UINT32 Idx;

        if (pTblIdx != NULL) {
            *pTblIdx = 0xFFFFFFFFU;
        }

        for (Idx = 0U; Idx < SVC_CalibWbCtrl.NumOfWbTable; Idx ++) {
            if (((SVC_CalibWbCtrl.pNandTable[Idx].Header.VinSelectBits & SvcCalib_BitGet(VinID)) > 0U) &&
                ((SVC_CalibWbCtrl.pNandTable[Idx].Header.SensorSelectBits & SensorID) > 0U)) {
                if (pTblIdx != NULL) {
                    *pTblIdx = Idx;
                }
                break;
            }
        }
    }

    return RetVal;
}


static UINT32 SvcCalib_WbTableDataGet(UINT32 TblID, SVC_CALIB_TBL_INFO_s *pTblInfo)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibWbCtrlFlag & SVC_CALIB_WB_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to get wb table - initial wb module first!" PRN_CAL_WB_NG
    } else if ((SVC_CalibWbCtrlFlag & SVC_CALIB_WB_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to get wb table - initial wb dram shadow first!" PRN_CAL_WB_NG
    } else if (SVC_CalibWbCtrl.pNandHeader == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to get wb table - invalid nand header!" PRN_CAL_WB_NG
    } else if (SVC_CalibWbCtrl.pNandHeader->NumOfTable == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "There is not wb table!" PRN_CAL_WB_DBG
    } else if (pTblInfo == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to get wb table - output table info should not null!" PRN_CAL_WB_NG
    } else if (pTblInfo->pBuf == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to get wb table - input table buffer should not null!" PRN_CAL_WB_NG
    } else if (pTblInfo->BufSize < sizeof(SVC_CALIB_WB_TBL_DATA_s)) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to get wb table - input table buffer size is too small! 0x%X/0x%X"
            PRN_CAL_WB_ARG_UINT32 pTblInfo->BufSize               PRN_CAL_WB_ARG_POST
            PRN_CAL_WB_ARG_UINT32 sizeof(SVC_CALIB_WB_TBL_DATA_s) PRN_CAL_WB_ARG_POST
        PRN_CAL_WB_NG
    } else {

        if (KAL_ERR_NONE != SvcCalib_WbMutexTake(&(SVC_CalibWbCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
            RetVal = SVC_NG;
            PRN_CAL_WB_LOG "Fail to set wb table - take mutex fail!" PRN_CAL_WB_NG
        } else {
            UINT32 CurTblID = 0xFFFFFFFFU;
            SVC_CALIB_WB_TBL_DATA_s *pOutput = NULL;
            const SVC_CALIB_WB_NAND_TABLE_s *pNandTable = NULL;

            AmbaMisra_TypeCast(&(pOutput), &(pTblInfo->pBuf));
            if (TblID == 255U) {
                if (0U != SvcCalib_WbNandTableSearch(pTblInfo->CalChan.VinID, pTblInfo->CalChan.SensorID, &CurTblID)) {
                    CurTblID = 0xFFFFFFFFU;
                }
            } else {
                CurTblID = TblID;
            }

            if (CurTblID >= SVC_CalibWbCtrl.NumOfWbTable) {
                RetVal = SVC_NG;

                PRetVal = (SVC_CalibWbCtrl.NumOfWbTable == 0U)?0U:(SVC_CalibWbCtrl.NumOfWbTable - 1U);
                PRN_CAL_WB_LOG "Fail to get wb table - the TableID(0x%X) is out-of range(0 ~ %d)"
                    PRN_CAL_WB_ARG_UINT32 CurTblID PRN_CAL_WB_ARG_POST
                    PRN_CAL_WB_ARG_UINT32 PRetVal  PRN_CAL_WB_ARG_POST
                PRN_CAL_WB_DBG
            } else {
                pNandTable = &(SVC_CalibWbCtrl.pNandTable[CurTblID]);

                AmbaSvcWrap_MisraMemset(pOutput, 0, sizeof(SVC_CALIB_WB_TBL_DATA_s));
                pOutput->OriGain[SVC_CALIB_WB_LCT].GainR = pNandTable->OriWb[SVC_CALIB_WB_LCT].GainR;
                pOutput->OriGain[SVC_CALIB_WB_LCT].GainG = pNandTable->OriWb[SVC_CALIB_WB_LCT].GainG;
                pOutput->OriGain[SVC_CALIB_WB_LCT].GainB = pNandTable->OriWb[SVC_CALIB_WB_LCT].GainB;
                pOutput->OriGain[SVC_CALIB_WB_HCT].GainR = pNandTable->OriWb[SVC_CALIB_WB_HCT].GainR;
                pOutput->OriGain[SVC_CALIB_WB_HCT].GainG = pNandTable->OriWb[SVC_CALIB_WB_HCT].GainG;
                pOutput->OriGain[SVC_CALIB_WB_HCT].GainB = pNandTable->OriWb[SVC_CALIB_WB_HCT].GainB;
                pOutput->RefGain[SVC_CALIB_WB_LCT].GainR = pNandTable->RefWb[SVC_CALIB_WB_LCT].GainR;
                pOutput->RefGain[SVC_CALIB_WB_LCT].GainG = pNandTable->RefWb[SVC_CALIB_WB_LCT].GainG;
                pOutput->RefGain[SVC_CALIB_WB_LCT].GainB = pNandTable->RefWb[SVC_CALIB_WB_LCT].GainB;
                pOutput->RefGain[SVC_CALIB_WB_HCT].GainR = pNandTable->RefWb[SVC_CALIB_WB_HCT].GainR;
                pOutput->RefGain[SVC_CALIB_WB_HCT].GainG = pNandTable->RefWb[SVC_CALIB_WB_HCT].GainG;
                pOutput->RefGain[SVC_CALIB_WB_HCT].GainB = pNandTable->RefWb[SVC_CALIB_WB_HCT].GainB;

                pTblInfo->CalChan.VinSelectBits    = pNandTable->Header.VinSelectBits;
                pTblInfo->CalChan.SensorSelectBits = pNandTable->Header.SensorSelectBits;

                PRN_CAL_WB_LOG "Success to get wb table from TableID(%d), VinSelectBits(0x%X) SensorSelectBits(0x%X)"
                    PRN_CAL_WB_ARG_UINT32 CurTblID                           PRN_CAL_WB_ARG_POST
                    PRN_CAL_WB_ARG_UINT32 pTblInfo->CalChan.VinSelectBits    PRN_CAL_WB_ARG_POST
                    PRN_CAL_WB_ARG_UINT32 pTblInfo->CalChan.SensorSelectBits PRN_CAL_WB_ARG_POST
                PRN_CAL_WB_DBG
            }

            SvcCalib_WbMutexGive(&(SVC_CalibWbCtrl.Mutex));
        }
    }

    return RetVal;
}


static UINT32 SvcCalib_WbTableDataSet(UINT32 TblID, SVC_CALIB_TBL_INFO_s *pTblInfo)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibWbCtrlFlag & SVC_CALIB_WB_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to set wb table - initial wb module first!" PRN_CAL_WB_NG
    } else if ((SVC_CalibWbCtrlFlag & SVC_CALIB_WB_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to set wb table - initial wb dram shadow first!" PRN_CAL_WB_NG
    } else if (TblID >= SVC_CalibWbCtrl.NumOfWbTable) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to set wb table - TableID(%d) out-of range(%d)!"
            PRN_CAL_WB_ARG_UINT32 TblID                        PRN_CAL_WB_ARG_POST
            PRN_CAL_WB_ARG_UINT32 SVC_CalibWbCtrl.NumOfWbTable PRN_CAL_WB_ARG_POST
        PRN_CAL_WB_NG
    } else if (pTblInfo == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to set wb table - output table info should not null!" PRN_CAL_WB_NG
    } else if (pTblInfo->pBuf == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to set wb table - input table buffer should not null!" PRN_CAL_WB_NG
    } else if (pTblInfo->BufSize < sizeof(SVC_CALIB_WB_TBL_DATA_s)) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to set wb table - input table buffer size is too small! 0x%X/0x%X"
            PRN_CAL_WB_ARG_UINT32 pTblInfo->BufSize               PRN_CAL_WB_ARG_POST
            PRN_CAL_WB_ARG_UINT32 sizeof(SVC_CALIB_WB_TBL_DATA_s) PRN_CAL_WB_ARG_POST
        PRN_CAL_WB_NG
    } else {
        if (KAL_ERR_NONE != SvcCalib_WbMutexTake(&(SVC_CalibWbCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
            RetVal = SVC_NG;
            PRN_CAL_WB_LOG "Fail to set wb table - take mutex fail!" PRN_CAL_WB_NG
        } else {
            SVC_CALIB_WB_NAND_HEADER_s *pNandHeader = SVC_CalibWbCtrl.pNandHeader;
            SVC_CALIB_WB_TBL_DATA_s *pInput = NULL;

            AmbaMisra_TypeCast(&(pInput), &(pTblInfo->pBuf));
            if (pInput != NULL) {
                AmbaSvcWrap_MisraMemset(&(SVC_CalibWbCtrl.pNandTable[TblID]), 0, sizeof(SVC_CALIB_WB_NAND_TABLE_s));
                SVC_CalibWbCtrl.pNandTable[TblID].Header.VinSelectBits    = pTblInfo->CalChan.VinSelectBits   ;
                SVC_CalibWbCtrl.pNandTable[TblID].Header.SensorSelectBits = pTblInfo->CalChan.SensorSelectBits;
                SVC_CalibWbCtrl.pNandTable[TblID].OriWb[SVC_CALIB_WB_LCT].GainR = pInput->OriGain[SVC_CALIB_WB_LCT].GainR;
                SVC_CalibWbCtrl.pNandTable[TblID].OriWb[SVC_CALIB_WB_LCT].GainG = pInput->OriGain[SVC_CALIB_WB_LCT].GainG;
                SVC_CalibWbCtrl.pNandTable[TblID].OriWb[SVC_CALIB_WB_LCT].GainB = pInput->OriGain[SVC_CALIB_WB_LCT].GainB;
                SVC_CalibWbCtrl.pNandTable[TblID].OriWb[SVC_CALIB_WB_HCT].GainR = pInput->OriGain[SVC_CALIB_WB_HCT].GainR;
                SVC_CalibWbCtrl.pNandTable[TblID].OriWb[SVC_CALIB_WB_HCT].GainG = pInput->OriGain[SVC_CALIB_WB_HCT].GainG;
                SVC_CalibWbCtrl.pNandTable[TblID].OriWb[SVC_CALIB_WB_HCT].GainB = pInput->OriGain[SVC_CALIB_WB_HCT].GainB;
                SVC_CalibWbCtrl.pNandTable[TblID].RefWb[SVC_CALIB_WB_LCT].GainR = pInput->RefGain[SVC_CALIB_WB_LCT].GainR;
                SVC_CalibWbCtrl.pNandTable[TblID].RefWb[SVC_CALIB_WB_LCT].GainG = pInput->RefGain[SVC_CALIB_WB_LCT].GainG;
                SVC_CalibWbCtrl.pNandTable[TblID].RefWb[SVC_CALIB_WB_LCT].GainB = pInput->RefGain[SVC_CALIB_WB_LCT].GainB;
                SVC_CalibWbCtrl.pNandTable[TblID].RefWb[SVC_CALIB_WB_HCT].GainR = pInput->RefGain[SVC_CALIB_WB_HCT].GainR;
                SVC_CalibWbCtrl.pNandTable[TblID].RefWb[SVC_CALIB_WB_HCT].GainG = pInput->RefGain[SVC_CALIB_WB_HCT].GainG;
                SVC_CalibWbCtrl.pNandTable[TblID].RefWb[SVC_CALIB_WB_HCT].GainB = pInput->RefGain[SVC_CALIB_WB_HCT].GainB;

                // Update nand header
                if (pNandHeader->Enable == 0U) {
                    pNandHeader->Enable = 1U;
                }
                if (pNandHeader->DebugMsgOn == 0U) {
                    pNandHeader->DebugMsgOn = SVC_CalibWbCtrl.EnableDebugMsg;
                }
                if (pNandHeader->DebugMsgOn == 0U) {
                    pNandHeader->DebugMsgOn = pTblInfo->DbgMsgOn;
                }
                if (SVC_CalibWbCtrl.EnableDebugMsg == 0U) {
                    SVC_CalibWbCtrl.EnableDebugMsg = pTblInfo->DbgMsgOn;
                }
                if ((pNandHeader->TableSelectBits & SvcCalib_BitGet(TblID)) == 0U) {
                    pNandHeader->TableSelectBits |= SvcCalib_BitGet(TblID);
                    pNandHeader->NumOfTable += 1U;
                }

                /* Update to NAND */
                RetVal = SvcCalib_DataSave(SVC_CALIB_WB_ID);
                if (RetVal != OK) {
                    PRN_CAL_WB_LOG "Fail to save wb to nand!" PRN_CAL_WB_NG
                }

                AmbaMisra_TouchUnused(pInput);
                AmbaMisra_TouchUnused(pTblInfo);

            }
            SvcCalib_WbMutexGive(&(SVC_CalibWbCtrl.Mutex));
        }
    }

    return RetVal;
}


static UINT32 SvcCalib_WbGainGet(const SVC_CALIB_CHANNEL_s *pCalibChan, UINT32 FlickerMode)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (pCalibChan == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to get wb gain - invalid calib channel!" PRN_CAL_WB_NG
    } else if (SVC_OK != SvcCalib_WbVinSensorIDCheck(pCalibChan->VinID, pCalibChan->SensorID)) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to get wb gain - invalid calib channel -> VinID(%d), SensorID(%d)!"
            PRN_CAL_WB_ARG_UINT32 pCalibChan->VinID    PRN_CAL_WB_ARG_POST
            PRN_CAL_WB_ARG_UINT32 pCalibChan->SensorID PRN_CAL_WB_ARG_POST
        PRN_CAL_WB_NG
    } else if (SVC_CalibWbCtrl.CalcCbFunc == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to get wb gain - calculate callback function should not null!" PRN_CAL_WB_NG
    } else {
        void *pExtData = NULL;

        AmbaMisra_TypeCast(&(pExtData), &(pCalibChan));
        PRetVal = (SVC_CalibWbCtrl.CalcCbFunc)(SVC_CALIB_WB_ID, SVC_CALIB_CALC_START, pExtData);  PRN_CAL_WB_ERR_HDLR

        if (KAL_ERR_NONE != SvcCalib_WbMutexTake(&(SVC_CalibWbCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
            RetVal = SVC_NG;
            PRN_CAL_WB_LOG "Fail to get wb table - take mutex fail!" PRN_CAL_WB_NG
        } else {

            SVC_CALIB_CALC_WB_INFO_s WbInfo;

            AmbaSvcWrap_MisraMemset(&WbInfo, 0, sizeof(WbInfo));
            WbInfo.CalibChan.VinID    = pCalibChan->VinID;
            WbInfo.CalibChan.SensorID = pCalibChan->SensorID;
            WbInfo.WbIndex            = WB_CAL_TEST;
            WbInfo.FlickerMode        = FlickerMode;
            RetVal = (SVC_CalibWbCtrl.CalcCbFunc)(SVC_CALIB_WB_ID, SVC_CALIB_CALC_WB_INFO, &WbInfo);
            if (RetVal != SVC_OK) {
                PRN_CAL_WB_LOG "Fail to get wb gain - get wb info fail!" PRN_CAL_WB_NG
            } else {
                PRN_CAL_WB_LOG "%sSuccess to get wb gain. VinID(%d), SensorID(%d) Flicker(%d), Gain R/G/B %d/%d/%d%s"
                    PRN_CAL_WB_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1    PRN_CAL_WB_ARG_POST
                    PRN_CAL_WB_ARG_UINT32 WbInfo.CalibChan.VinID    PRN_CAL_WB_ARG_POST
                    PRN_CAL_WB_ARG_UINT32 WbInfo.CalibChan.SensorID PRN_CAL_WB_ARG_POST
                    PRN_CAL_WB_ARG_UINT32 WbInfo.FlickerMode        PRN_CAL_WB_ARG_POST
                    PRN_CAL_WB_ARG_UINT32 WbInfo.CurGain.GainR      PRN_CAL_WB_ARG_POST
                    PRN_CAL_WB_ARG_UINT32 WbInfo.CurGain.GainG      PRN_CAL_WB_ARG_POST
                    PRN_CAL_WB_ARG_UINT32 WbInfo.CurGain.GainB      PRN_CAL_WB_ARG_POST
                    PRN_CAL_WB_ARG_CSTR   SVC_LOG_CAL_HL_END        PRN_CAL_WB_ARG_POST
                PRN_CAL_WB_API
            }

            SvcCalib_WbMutexGive(&(SVC_CalibWbCtrl.Mutex));
        }

        PRetVal = (SVC_CalibWbCtrl.CalcCbFunc)(SVC_CALIB_WB_ID, SVC_CALIB_CALC_DONE, NULL);  PRN_CAL_WB_ERR_HDLR
    }

    return RetVal;
}

static void SvcCalib_WbShellStrToU32(const char *pString, UINT32 *pValue)
{
    UINT32 ErrCode = AmbaUtility_StringToUInt32(pString, pValue);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }
}

static UINT32 SvcCalib_WbShellCfgVinSen(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibWbCtrlFlag & SVC_CALIB_WB_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to cfg wb vin/sensor id - initial wb module first!" PRN_CAL_WB_NG
    } else if (ArgCount < 7U) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to cfg wb vin/sensor id - Argc should >= 8" PRN_CAL_WB_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to cfg wb vin/sensor id - Argv should not null!" PRN_CAL_WB_NG
    } else {
        UINT32 VinID            = 0U;
        UINT32 SensorID         = 0U;
        UINT32 VinSelectBits    = 0U;
        UINT32 SensorSelectBits = 0U;

        SvcCalib_WbShellStrToU32(pArgVector[3U], &VinID           );
        SvcCalib_WbShellStrToU32(pArgVector[4U], &SensorID        );
        SvcCalib_WbShellStrToU32(pArgVector[5U], &VinSelectBits   );
        SvcCalib_WbShellStrToU32(pArgVector[6U], &SensorSelectBits);

        if (SVC_OK != SvcCalib_WbVinSensorIDCheck(VinID, SensorID)) {
            RetVal = SVC_NG;
            PRN_CAL_WB_LOG "Fail to cfg wb vin/sensor id - invalid VinID %d, SensorID %d"
                PRN_CAL_WB_ARG_UINT32 VinID    PRN_CAL_WB_ARG_POST
                PRN_CAL_WB_ARG_UINT32 SensorID PRN_CAL_WB_ARG_POST
            PRN_CAL_WB_NG
        }

        if (RetVal == SVC_OK) {

            /* Initial the wb calculate parameters */
            AmbaSvcWrap_MisraMemset(&(SVC_CalibWbCtrl.CalcCtrl), 0, sizeof(SVC_CALIB_WB_CALC_CTRL_s));
            SVC_CalibWbCtrlFlag &= ~( SVC_CALIB_WB_FLG_CALC_INIT    |
                                      SVC_CALIB_WB_FLG_CALC_VIN_SEN );

            SVC_CalibWbCtrl.CalcCtrl.SaveBinOn                    = 1U;
            SVC_CalibWbCtrl.CalcCtrl.TableIdx                     = 0xFFFFFFFFU;
            SVC_CalibWbCtrl.CalcCtrl.RefWb[SVC_CALIB_WB_LCT].GainR = SVC_CALIB_WB_DEF_GAIN;
            SVC_CalibWbCtrl.CalcCtrl.RefWb[SVC_CALIB_WB_LCT].GainG = SVC_CALIB_WB_DEF_GAIN;
            SVC_CalibWbCtrl.CalcCtrl.RefWb[SVC_CALIB_WB_LCT].GainB = SVC_CALIB_WB_DEF_GAIN;
            SVC_CalibWbCtrl.CalcCtrl.RefWb[SVC_CALIB_WB_HCT].GainR = SVC_CALIB_WB_DEF_GAIN;
            SVC_CalibWbCtrl.CalcCtrl.RefWb[SVC_CALIB_WB_HCT].GainG = SVC_CALIB_WB_DEF_GAIN;
            SVC_CalibWbCtrl.CalcCtrl.RefWb[SVC_CALIB_WB_HCT].GainB = SVC_CALIB_WB_DEF_GAIN;
            SVC_CalibWbCtrlFlag |= SVC_CALIB_WB_FLG_CALC_INIT;

            PRN_CAL_WB_LOG " " PRN_CAL_WB_API
            PRN_CAL_WB_LOG "------ Calibration WB initial setting ------" PRN_CAL_WB_API
            PRN_CAL_WB_LOG "  SaveBinOn                    : %d"
                PRN_CAL_WB_ARG_UINT32 SVC_CalibWbCtrl.CalcCtrl.SaveBinOn PRN_CAL_WB_ARG_POST
            PRN_CAL_WB_API
            PRN_CAL_WB_LOG "  TableIdx                     : 0x%X"
                PRN_CAL_WB_ARG_UINT32 SVC_CalibWbCtrl.CalcCtrl.TableIdx PRN_CAL_WB_ARG_POST
            PRN_CAL_WB_API

            /* Configure the calib channel */
            AmbaSvcWrap_MisraMemset(&(SVC_CalibWbCtrl.CalibChan), 0, sizeof(SVC_CALIB_CHANNEL_s));
            SVC_CalibWbCtrl.CalibChan.VinSelectBits    = VinSelectBits   ;
            SVC_CalibWbCtrl.CalibChan.SensorSelectBits = SensorSelectBits;
            SVC_CalibWbCtrl.CalibChan.VinID            = VinID           ;
            SVC_CalibWbCtrl.CalibChan.SensorID         = SensorID        ;
            SVC_CalibWbCtrl.CalibChan.ExtendData       = 0xFFFFFFFFU     ;
            SVC_CalibWbCtrlFlag |= SVC_CALIB_WB_FLG_CALC_VIN_SEN;

            PRN_CAL_WB_LOG " " PRN_CAL_WB_API
            PRN_CAL_WB_LOG "------ Calibration WB Channel ------" PRN_CAL_WB_API
            PRN_CAL_WB_LOG "  VinID            : 0x%X"
                PRN_CAL_WB_ARG_UINT32 SVC_CalibWbCtrl.CalibChan.VinID PRN_CAL_WB_ARG_POST
            PRN_CAL_WB_API
            PRN_CAL_WB_LOG "  SensorID         : 0x%X"
                PRN_CAL_WB_ARG_UINT32 SVC_CalibWbCtrl.CalibChan.SensorID PRN_CAL_WB_ARG_POST
            PRN_CAL_WB_API
            PRN_CAL_WB_LOG "  VinSelectBits    : 0x%X"
                PRN_CAL_WB_ARG_UINT32 SVC_CalibWbCtrl.CalibChan.VinSelectBits PRN_CAL_WB_ARG_POST
            PRN_CAL_WB_API
            PRN_CAL_WB_LOG "  SensorSelectBits : 0x%X"
                PRN_CAL_WB_ARG_UINT32 SVC_CalibWbCtrl.CalibChan.SensorSelectBits PRN_CAL_WB_ARG_POST
            PRN_CAL_WB_API
        }
    }

    return RetVal;
}


static void SvcCalib_WbShellCfgVinSenU(void)
{
    PRN_CAL_WB_LOG "  %scfg_calib_chan%s        : reset all calibration setting as default"
        PRN_CAL_WB_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_WB_ARG_POST
        PRN_CAL_WB_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_WB_ARG_POST
    PRN_CAL_WB_API
    PRN_CAL_WB_LOG "  ------------------------------------------------------" PRN_CAL_WB_API
    PRN_CAL_WB_LOG "    [VinID]             : vin identify number." PRN_CAL_WB_API
    PRN_CAL_WB_LOG "    [SensorID]          : sensor identify number. 1/2/4/8" PRN_CAL_WB_API
    PRN_CAL_WB_LOG "    [VinSelectBits]     : bit0 : Vin0, bit1 : Vin1, ... " PRN_CAL_WB_API
    PRN_CAL_WB_LOG "    [SensorSelectBits]  : bit0 : sensor0, bit1 : sensor1, ..." PRN_CAL_WB_API
    PRN_CAL_WB_LOG " " PRN_CAL_WB_API
}


static UINT32 SvcCalib_WbShellCfgTblID(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibWbCtrlFlag & SVC_CALIB_WB_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to cfg wb table idx - initial wb module first!" PRN_CAL_WB_NG
    } else if (ArgCount < 3U) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to cfg wb table idx - Argc should >= 3" PRN_CAL_WB_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to cfg wb table idx - Argv should not null!" PRN_CAL_WB_NG
    } else {
        UINT32 TableIdx = 0U;

        SvcCalib_WbShellStrToU32(pArgVector[3U],  &TableIdx );

        SVC_CalibWbCtrl.CalcCtrl.TableIdx = TableIdx;

        PRN_CAL_WB_LOG " " PRN_CAL_WB_API
        PRN_CAL_WB_LOG "------ Calibration WB Table Index %d ------"
            PRN_CAL_WB_ARG_UINT32 SVC_CalibWbCtrl.CalcCtrl.TableIdx PRN_CAL_WB_ARG_POST
        PRN_CAL_WB_API

    }

    return RetVal;
}


static void SvcCalib_WbShellCfgTblIDU(void)
{
    PRN_CAL_WB_LOG "  %scfg_table_id%s          :"
        PRN_CAL_WB_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_WB_ARG_POST
        PRN_CAL_WB_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_WB_ARG_POST
    PRN_CAL_WB_API
    PRN_CAL_WB_LOG "  ------------------------------------------------------" PRN_CAL_WB_API
    PRN_CAL_WB_LOG "    [Nand Table ID]     : the nand table id." PRN_CAL_WB_API
    PRN_CAL_WB_LOG "                        : the max value depend on each app" PRN_CAL_WB_API
    PRN_CAL_WB_LOG " " PRN_CAL_WB_API
}


static UINT32 SvcCalib_WbShellCfgDbgMsg(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibWbCtrlFlag & SVC_CALIB_WB_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to switch wb debug msg - initial wb module first!" PRN_CAL_WB_NG
    } else if (ArgCount < 3U) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to switch wb debug msg - Argc should >= 3" PRN_CAL_WB_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to switch wb debug msg - Argv should not null!" PRN_CAL_WB_NG
    } else {
        UINT32 DbgMsgOn = 0U;

        SvcCalib_WbShellStrToU32(pArgVector[3U],  &DbgMsgOn );

        if ((DbgMsgOn & 0x1U) > 0U) {
            SVC_CalibWbCtrl.EnableDebugMsg = DbgMsgOn;
        }

        if ((DbgMsgOn & 0x2U) > 0U) {
            SVC_CalibWbCtrlFlag |= SVC_LOG_CAL_DBG;
        }

        PRN_CAL_WB_LOG " " PRN_CAL_WB_API
        PRN_CAL_WB_LOG "------ Calibration WB Debug Msg On/Off %d, lvl %d ------"
            PRN_CAL_WB_ARG_UINT32 SVC_CalibWbCtrl.EnableDebugMsg PRN_CAL_WB_ARG_POST
            PRN_CAL_WB_ARG_UINT32 DbgMsgOn                       PRN_CAL_WB_ARG_POST
        PRN_CAL_WB_API

    }

    return RetVal;
}


static void SvcCalib_WbShellCfgDbgMsgU(void)
{
    PRN_CAL_WB_LOG "  %scfg_dbg_msg%s           :"
        PRN_CAL_WB_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_WB_ARG_POST
        PRN_CAL_WB_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_WB_ARG_POST
    PRN_CAL_WB_API
    PRN_CAL_WB_LOG "  ------------------------------------------------------" PRN_CAL_WB_API
    PRN_CAL_WB_LOG "    [Enable]            : 0: disable debug message" PRN_CAL_WB_API
    PRN_CAL_WB_LOG "                        : 1: enable debug message" PRN_CAL_WB_API
    PRN_CAL_WB_LOG "                        : 2: enable debug message level 2" PRN_CAL_WB_API
    PRN_CAL_WB_LOG " " PRN_CAL_WB_API
}


static UINT32 SvcCalib_WbShellGetWbGain(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibWbCtrlFlag & SVC_CALIB_WB_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to set wb gain - initial wb module first!" PRN_CAL_WB_NG
    } else if (ArgCount < 5U) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to set wb gain - Argc should >= 5" PRN_CAL_WB_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to set wb gain - Argv should not null!" PRN_CAL_WB_NG
    } else {
        UINT32 VinID       = 0U;
        UINT32 SensorID    = 0U;
        UINT32 FlickerMode = 0U;
        SVC_CALIB_CHANNEL_s CalibChan;

        SvcCalib_WbShellStrToU32(pArgVector[3U], &VinID      );
        SvcCalib_WbShellStrToU32(pArgVector[4U], &SensorID   );
        SvcCalib_WbShellStrToU32(pArgVector[5U], &FlickerMode);

        AmbaSvcWrap_MisraMemset(&CalibChan, 0, sizeof(CalibChan));
        CalibChan.VinID = VinID;
        CalibChan.SensorID = SensorID;

        RetVal = SvcCalib_WbGainGet(&CalibChan, FlickerMode);
    }

    return RetVal;
}


static void SvcCalib_WbShellGetWbGainU(void)
{
    PRN_CAL_WB_LOG "  %sget_wb_gain%s           : get the current wb gain"
        PRN_CAL_WB_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_WB_ARG_POST
        PRN_CAL_WB_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_WB_ARG_POST
    PRN_CAL_WB_API
    PRN_CAL_WB_LOG "  ------------------------------------------------------" PRN_CAL_WB_API
    PRN_CAL_WB_LOG "    [VinID]             : vin identify number." PRN_CAL_WB_API
    PRN_CAL_WB_LOG "    [SensorID]          : sensor identify number. 1/2/4/8" PRN_CAL_WB_API
    PRN_CAL_WB_LOG " " PRN_CAL_WB_API
}


static UINT32 SvcCalib_WbShellGenTbl(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibWbCtrlFlag & SVC_CALIB_WB_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to set wb gain - initial wb module first!" PRN_CAL_WB_NG
    } else if (ArgCount < 9U) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to set wb gain - Argc should >= 9" PRN_CAL_WB_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to set wb gain - Argv should not null!" PRN_CAL_WB_NG
    } else {
        UINT32 CTType      = 0U;
        UINT32 FlickerMode = 0U;
        UINT32 RefGainR    = 0U;
        UINT32 RefGainG    = 0U;
        UINT32 RefGainB    = 0U;
        UINT32 ThdR        = 0U;
        UINT32 ThdB        = 0U;

        SvcCalib_WbShellStrToU32(pArgVector[3U], &CTType     );
        SvcCalib_WbShellStrToU32(pArgVector[4U], &FlickerMode);
        SvcCalib_WbShellStrToU32(pArgVector[5U], &RefGainR   );
        SvcCalib_WbShellStrToU32(pArgVector[6U], &RefGainG   );
        SvcCalib_WbShellStrToU32(pArgVector[7U], &RefGainB   );
        SvcCalib_WbShellStrToU32(pArgVector[8U], &ThdR       );
        SvcCalib_WbShellStrToU32(pArgVector[9U], &ThdB       );

        SVC_CalibWbCtrl.CalcCtrl.CTType      = CTType;
        SVC_CalibWbCtrl.CalcCtrl.FlickerMode = FlickerMode;
        SVC_CalibWbCtrl.CalcCtrl.ThresholdR  = ThdR;
        SVC_CalibWbCtrl.CalcCtrl.ThresholdB  = ThdB;
        if (CTType == SVC_CALIB_WB_LCT) {
            SVC_CalibWbCtrl.CalcCtrl.RefWb[SVC_CALIB_WB_LCT].GainR = RefGainR;
            SVC_CalibWbCtrl.CalcCtrl.RefWb[SVC_CALIB_WB_LCT].GainG = RefGainG;
            SVC_CalibWbCtrl.CalcCtrl.RefWb[SVC_CALIB_WB_LCT].GainB = RefGainB;
        } else {
            SVC_CalibWbCtrl.CalcCtrl.RefWb[SVC_CALIB_WB_HCT].GainR = RefGainR;
            SVC_CalibWbCtrl.CalcCtrl.RefWb[SVC_CALIB_WB_HCT].GainG = RefGainG;
            SVC_CalibWbCtrl.CalcCtrl.RefWb[SVC_CALIB_WB_HCT].GainB = RefGainB;
        }

        PRN_CAL_WB_LOG " " PRN_CAL_WB_API
        PRN_CAL_WB_LOG "------ Calibration WB Calc Param ------" PRN_CAL_WB_API
        PRN_CAL_WB_LOG "  CTType      : %d"
            PRN_CAL_WB_ARG_UINT32 CTType PRN_CAL_WB_ARG_POST
        PRN_CAL_WB_API
        PRN_CAL_WB_LOG "  FlickerMode : %d"
            PRN_CAL_WB_ARG_UINT32 FlickerMode PRN_CAL_WB_ARG_POST
        PRN_CAL_WB_API
        PRN_CAL_WB_LOG "  ThresholdR  : %d"
            PRN_CAL_WB_ARG_UINT32 ThdR PRN_CAL_WB_ARG_POST
        PRN_CAL_WB_API
        PRN_CAL_WB_LOG "  ThresholdB  : %d"
            PRN_CAL_WB_ARG_UINT32 ThdB PRN_CAL_WB_ARG_POST
        PRN_CAL_WB_API
        PRN_CAL_WB_LOG "  RefGainR    : %d"
            PRN_CAL_WB_ARG_UINT32 RefGainR PRN_CAL_WB_ARG_POST
        PRN_CAL_WB_API
        PRN_CAL_WB_LOG "  RefGainG    : %d"
            PRN_CAL_WB_ARG_UINT32 RefGainG PRN_CAL_WB_ARG_POST
        PRN_CAL_WB_API
        PRN_CAL_WB_LOG "  RefGainB    : %d"
            PRN_CAL_WB_ARG_UINT32 RefGainB PRN_CAL_WB_ARG_POST
        PRN_CAL_WB_API

        RetVal = SvcCalib_WbTableCalc(&(SVC_CalibWbCtrl.CalibChan));
    }

    return RetVal;
}


static void SvcCalib_WbShellGenTblU(void)
{
    PRN_CAL_WB_LOG "  %sset_wb_gain%s           : set the low/high color temperature gain"
        PRN_CAL_WB_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_WB_ARG_POST
        PRN_CAL_WB_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_WB_ARG_POST
    PRN_CAL_WB_API

    PRN_CAL_WB_LOG "  ------------------------------------------------------" PRN_CAL_WB_API
    PRN_CAL_WB_LOG "    [Color Temperature] : color temperature" PRN_CAL_WB_API
    PRN_CAL_WB_LOG "                        : 0 -> Low color temperature" PRN_CAL_WB_API
    PRN_CAL_WB_LOG "                        : 1 -> High color temperature" PRN_CAL_WB_API
    PRN_CAL_WB_LOG "    [Flicker]           : flicker mode" PRN_CAL_WB_API
    PRN_CAL_WB_LOG "    [Threshold R]       : WB gain R threshold" PRN_CAL_WB_API
    PRN_CAL_WB_LOG "    [Threshold B]       : WB gain B threshold" PRN_CAL_WB_API
    PRN_CAL_WB_LOG "    [Ref Gain R]        : Reference gain R" PRN_CAL_WB_API
    PRN_CAL_WB_LOG "    [Ref Gain G]        : Reference gain G (4096)" PRN_CAL_WB_API
    PRN_CAL_WB_LOG "    [Ref Gain B]        : Reference gain B" PRN_CAL_WB_API
    PRN_CAL_WB_LOG " " PRN_CAL_WB_API
}

static UINT32 SvcCalib_WbShellSet(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibWbCtrlFlag & SVC_CALIB_WB_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to proc wb->upd cmd - initial wb module first!" PRN_CAL_WB_NG
    } else if ((SVC_CalibWbCtrlFlag & SVC_CALIB_WB_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to proc wb->set cmd - initial wb shadow first!" PRN_CAL_WB_NG
    } else if (SVC_CalibWbCtrl.pNandTable == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to proc wb->set cmd - invalid wb nand table!" PRN_CAL_WB_NG
    } else if (ArgCount <= 5U) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to proc wb->set cmd - Argc should > 5" PRN_CAL_WB_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to proc wb->set cmd - Argv should not null!" PRN_CAL_WB_NG
    } else {
        UINT32 TblID = 0xFFU, ArgIdx = 3U;

        SvcCalib_WbShellStrToU32(pArgVector[ArgIdx],  &TblID); ArgIdx ++;
        if (TblID >= SVC_CalibWbCtrl.NumOfWbTable) {
            RetVal = SVC_NG;
            PRN_CAL_WB_LOG "Fail to proc wb->set cmd - insert table id(%d) is out-of range(%d)!"
                PRN_CAL_WB_ARG_UINT32 TblID                       PRN_CAL_WB_ARG_POST
                PRN_CAL_WB_ARG_UINT32 SVC_CalibWbCtrl.NumOfWbTable PRN_CAL_WB_ARG_POST
            PRN_CAL_WB_NG
        } else {
            SVC_CALIB_WB_NAND_TABLE_s CurWbTbl;
            SVC_CALIB_WB_NAND_TABLE_s *pNandTable = &(SVC_CalibWbCtrl.pNandTable[TblID]);
            UINT32 DbgMsgOn = 1U;

            AmbaSvcWrap_MisraMemset(&CurWbTbl, 0, sizeof(SVC_CALIB_WB_NAND_TABLE_s));
            SvcCalib_WbShellStrToU32(pArgVector[ArgIdx], &(CurWbTbl.Header.VinSelectBits   )); ArgIdx++;
            SvcCalib_WbShellStrToU32(pArgVector[ArgIdx], &(CurWbTbl.Header.SensorSelectBits)); ArgIdx++;

            SvcCalib_WbShellStrToU32(pArgVector[ArgIdx], &(CurWbTbl.OriWb[SVC_CALIB_WB_LCT].GainR)); ArgIdx++;
            SvcCalib_WbShellStrToU32(pArgVector[ArgIdx], &(CurWbTbl.OriWb[SVC_CALIB_WB_LCT].GainG)); ArgIdx++;
            SvcCalib_WbShellStrToU32(pArgVector[ArgIdx], &(CurWbTbl.OriWb[SVC_CALIB_WB_LCT].GainB)); ArgIdx++;
            SvcCalib_WbShellStrToU32(pArgVector[ArgIdx], &(CurWbTbl.OriWb[SVC_CALIB_WB_HCT].GainR)); ArgIdx++;
            SvcCalib_WbShellStrToU32(pArgVector[ArgIdx], &(CurWbTbl.OriWb[SVC_CALIB_WB_HCT].GainG)); ArgIdx++;
            SvcCalib_WbShellStrToU32(pArgVector[ArgIdx], &(CurWbTbl.OriWb[SVC_CALIB_WB_HCT].GainB)); ArgIdx++;
            SvcCalib_WbShellStrToU32(pArgVector[ArgIdx], &(CurWbTbl.RefWb[SVC_CALIB_WB_LCT].GainR)); ArgIdx++;
            SvcCalib_WbShellStrToU32(pArgVector[ArgIdx], &(CurWbTbl.RefWb[SVC_CALIB_WB_LCT].GainG)); ArgIdx++;
            SvcCalib_WbShellStrToU32(pArgVector[ArgIdx], &(CurWbTbl.RefWb[SVC_CALIB_WB_LCT].GainB)); ArgIdx++;
            SvcCalib_WbShellStrToU32(pArgVector[ArgIdx], &(CurWbTbl.RefWb[SVC_CALIB_WB_HCT].GainR)); ArgIdx++;
            SvcCalib_WbShellStrToU32(pArgVector[ArgIdx], &(CurWbTbl.RefWb[SVC_CALIB_WB_HCT].GainG)); ArgIdx++;
            SvcCalib_WbShellStrToU32(pArgVector[ArgIdx], &(CurWbTbl.RefWb[SVC_CALIB_WB_HCT].GainB)); ArgIdx++;
            SvcCalib_WbShellStrToU32(pArgVector[ArgIdx], &DbgMsgOn);

            if (KAL_ERR_NONE != SvcCalib_WbMutexTake(&(SVC_CalibWbCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
                RetVal = SVC_NG;
                PRN_CAL_WB_LOG "Fail to proc wb->set cmd - take mutex fail!" PRN_CAL_WB_NG
            } else {

                AmbaSvcWrap_MisraMemcpy(pNandTable, &CurWbTbl, sizeof(SVC_CALIB_WB_NAND_TABLE_s));

                SVC_CalibWbCtrl.pNandHeader->Enable = 1U;
                SVC_CalibWbCtrl.pNandHeader->DebugMsgOn = DbgMsgOn;
                if ((SVC_CalibWbCtrl.pNandHeader->TableSelectBits & SvcCalib_BitGet(TblID)) == 0U) {
                    SVC_CalibWbCtrl.pNandHeader->TableSelectBits |= SvcCalib_BitGet(TblID);
                    SVC_CalibWbCtrl.pNandHeader->NumOfTable ++;
                }

                PRN_CAL_WB_LOG " " PRN_CAL_WB_API
                PRN_CAL_WB_LOG "====== Calibration wb info ======" PRN_CAL_WB_API
                PRN_CAL_WB_LOG "  Table ID         : %d"
                    PRN_CAL_WB_ARG_UINT32 SVC_CalibWbCtrl.CalcCtrl.TableIdx PRN_CAL_WB_ARG_POST
                PRN_CAL_WB_API
                PRN_CAL_WB_LOG "  VinSelectBits    : 0x%X"
                    PRN_CAL_WB_ARG_UINT32 pNandTable->Header.VinSelectBits PRN_CAL_WB_ARG_POST
                PRN_CAL_WB_API
                PRN_CAL_WB_LOG "  SensorSelectBits : 0x%X"
                    PRN_CAL_WB_ARG_UINT32 pNandTable->Header.SensorSelectBits PRN_CAL_WB_ARG_POST
                PRN_CAL_WB_API
                PRN_CAL_WB_LOG "  ---- Low Color Temperature ----" PRN_CAL_WB_API
                PRN_CAL_WB_LOG "    OriWb.GainR/G/B  : %4d %4d %4d"
                    PRN_CAL_WB_ARG_UINT32 pNandTable->OriWb[SVC_CALIB_WB_LCT].GainR PRN_CAL_WB_ARG_POST
                    PRN_CAL_WB_ARG_UINT32 pNandTable->OriWb[SVC_CALIB_WB_LCT].GainG PRN_CAL_WB_ARG_POST
                    PRN_CAL_WB_ARG_UINT32 pNandTable->OriWb[SVC_CALIB_WB_LCT].GainB PRN_CAL_WB_ARG_POST
                PRN_CAL_WB_API
                PRN_CAL_WB_LOG "    RefWb.GainR/G/B  : %4d %4d %4d"
                    PRN_CAL_WB_ARG_UINT32 pNandTable->RefWb[SVC_CALIB_WB_LCT].GainR PRN_CAL_WB_ARG_POST
                    PRN_CAL_WB_ARG_UINT32 pNandTable->RefWb[SVC_CALIB_WB_LCT].GainG PRN_CAL_WB_ARG_POST
                    PRN_CAL_WB_ARG_UINT32 pNandTable->RefWb[SVC_CALIB_WB_LCT].GainB PRN_CAL_WB_ARG_POST
                PRN_CAL_WB_API
                PRN_CAL_WB_LOG "  ---- High Color Temperature ----" PRN_CAL_WB_API
                PRN_CAL_WB_LOG "    OriWb.GainR/G/B  : %4d %4d %4d"
                    PRN_CAL_WB_ARG_UINT32 pNandTable->OriWb[SVC_CALIB_WB_HCT].GainR PRN_CAL_WB_ARG_POST
                    PRN_CAL_WB_ARG_UINT32 pNandTable->OriWb[SVC_CALIB_WB_HCT].GainG PRN_CAL_WB_ARG_POST
                    PRN_CAL_WB_ARG_UINT32 pNandTable->OriWb[SVC_CALIB_WB_HCT].GainB PRN_CAL_WB_ARG_POST
                PRN_CAL_WB_API
                PRN_CAL_WB_LOG "    RefWb.GainR/G/B  : %4d %4d %4d"
                    PRN_CAL_WB_ARG_UINT32 pNandTable->RefWb[SVC_CALIB_WB_HCT].GainR PRN_CAL_WB_ARG_POST
                    PRN_CAL_WB_ARG_UINT32 pNandTable->RefWb[SVC_CALIB_WB_HCT].GainG PRN_CAL_WB_ARG_POST
                    PRN_CAL_WB_ARG_UINT32 pNandTable->RefWb[SVC_CALIB_WB_HCT].GainB PRN_CAL_WB_ARG_POST
                PRN_CAL_WB_API

                /* Update to NAND */
                RetVal = SvcCalib_DataSave(SVC_CALIB_WB_ID);
                if (RetVal != OK) {
                    PRN_CAL_WB_LOG "Fail to save wb to nand!" PRN_CAL_WB_NG
                }

                SvcCalib_WbMutexGive(&(SVC_CalibWbCtrl.Mutex));
            }
        }
    }

    return RetVal;
}
static void SvcCalib_WbShellSetU(void)
{
    PRN_CAL_WB_LOG "  %sset%s                   : set wb table manually"
        PRN_CAL_WB_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_WB_ARG_POST
        PRN_CAL_WB_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_WB_ARG_POST
    PRN_CAL_WB_API

    PRN_CAL_WB_LOG "  ------------------------------------------------------" PRN_CAL_WB_API
    PRN_CAL_WB_LOG "    [TblID]                 : inser table index" PRN_CAL_WB_API
    PRN_CAL_WB_LOG "    [VinSelectBits]         : the table supported vin select bits"    PRN_CAL_WB_API
    PRN_CAL_WB_LOG "    [SensorSelectBits]      : the table supported sensor select bits" PRN_CAL_WB_API
    PRN_CAL_WB_LOG "    [LCT R gain]            : Low  color temperature R gain"          PRN_CAL_WB_API
    PRN_CAL_WB_LOG "    [LCT G gain]            : Low  color temperature G gain"          PRN_CAL_WB_API
    PRN_CAL_WB_LOG "    [LCT B gain]            : Low  color temperature B gain"          PRN_CAL_WB_API
    PRN_CAL_WB_LOG "    [HCT R gain]            : High color temperature R gain"          PRN_CAL_WB_API
    PRN_CAL_WB_LOG "    [HCT G gain]            : High color temperature G gain"          PRN_CAL_WB_API
    PRN_CAL_WB_LOG "    [HCT B gain]            : High color temperature B gain"          PRN_CAL_WB_API
    PRN_CAL_WB_LOG "    [LCT R Ref gain]        : Low  color temperature R reference gain" PRN_CAL_WB_API
    PRN_CAL_WB_LOG "    [LCT G Ref gain]        : Low  color temperature G reference gain" PRN_CAL_WB_API
    PRN_CAL_WB_LOG "    [LCT B Ref gain]        : Low  color temperature B reference gain" PRN_CAL_WB_API
    PRN_CAL_WB_LOG "    [HCT R Ref gain]        : High color temperature R reference gain" PRN_CAL_WB_API
    PRN_CAL_WB_LOG "    [HCT G Ref gain]        : High color temperature G reference gain" PRN_CAL_WB_API
    PRN_CAL_WB_LOG "    [HCT B Ref gain]        : High color temperature B reference gain" PRN_CAL_WB_API

    PRN_CAL_WB_LOG " " PRN_CAL_WB_API
}

static void SvcCalib_WbShellEntryInit(void)
{
    if ((SVC_CalibWbCtrlFlag & SVC_CALIB_WB_FLG_SHELL_INIT) == 0U) {
        UINT32 ShellNum = 0U;

        AmbaSvcWrap_MisraMemset(CalibWbShellFunc, 0, sizeof(CalibWbShellFunc));

        CalibWbShellFunc[ShellNum] = (SVC_CALIB_WB_SHELL_FUNC_s) { "cfg_calib_chan", SvcCalib_WbShellCfgVinSen, SvcCalib_WbShellCfgVinSenU }; ShellNum ++;
        CalibWbShellFunc[ShellNum] = (SVC_CALIB_WB_SHELL_FUNC_s) { "cfg_table_id",   SvcCalib_WbShellCfgTblID,  SvcCalib_WbShellCfgTblIDU  }; ShellNum ++;
        CalibWbShellFunc[ShellNum] = (SVC_CALIB_WB_SHELL_FUNC_s) { "cfg_dbg_msg",    SvcCalib_WbShellCfgDbgMsg, SvcCalib_WbShellCfgDbgMsgU }; ShellNum ++;
        CalibWbShellFunc[ShellNum] = (SVC_CALIB_WB_SHELL_FUNC_s) { "get_wb_gain",    SvcCalib_WbShellGetWbGain, SvcCalib_WbShellGetWbGainU }; ShellNum ++;
        CalibWbShellFunc[ShellNum] = (SVC_CALIB_WB_SHELL_FUNC_s) { "set_wb_gain",    SvcCalib_WbShellGenTbl,    SvcCalib_WbShellGenTblU    }; ShellNum ++;
        CalibWbShellFunc[ShellNum] = (SVC_CALIB_WB_SHELL_FUNC_s) { "set",            SvcCalib_WbShellSet,       SvcCalib_WbShellSetU    };

        SVC_CalibWbCtrlFlag |= SVC_CALIB_WB_FLG_SHELL_INIT;
    }
}


UINT32 SvcCalib_WbShellFunc(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_NG;

    SvcCalib_WbShellEntryInit();

    if (ArgCount > 0U) {
        UINT32 ShellIdx;
        UINT32 ShellCount = (UINT32)(sizeof(CalibWbShellFunc)) / (UINT32)(sizeof(CalibWbShellFunc[0]));

        for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
            if (CalibWbShellFunc[ShellIdx].pFunc != NULL) {
                if (0 == SvcWrap_strcmp(pArgVector[2U], CalibWbShellFunc[ShellIdx].ShellCmdName)) {
                    if (SVC_OK != (CalibWbShellFunc[ShellIdx].pFunc)(ArgCount, pArgVector)) {
                        if (CalibWbShellFunc[ShellIdx].pUsageFunc != NULL) {
                            (CalibWbShellFunc[ShellIdx].pUsageFunc)();
                        }
                    }

                    RetVal = SVC_OK;
                    break;
                }
            }
        }

        if (RetVal != SVC_OK) {
            SvcCalib_WbShellUsage();
        }
    }

    return RetVal;
}


static void SvcCalib_WbShellUsage(void)
{
    UINT32 ShellIdx;
    UINT32 ShellCount = (UINT32)(sizeof(CalibWbShellFunc)) / (UINT32)(sizeof(CalibWbShellFunc[0]));

    PRN_CAL_WB_LOG " " PRN_CAL_WB_API
    PRN_CAL_WB_LOG "====== WB Command Usage ======" PRN_CAL_WB_API

    for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
        if (CalibWbShellFunc[ShellIdx].pFunc != NULL) {
            if (CalibWbShellFunc[ShellIdx].pUsageFunc == NULL) {
                PRN_CAL_WB_LOG "  %s"
                    PRN_CAL_WB_ARG_CSTR   CalibWbShellFunc[ShellIdx].ShellCmdName PRN_CAL_WB_ARG_POST
                PRN_CAL_WB_API
            } else {
                (CalibWbShellFunc[ShellIdx].pUsageFunc)();
            }
        }
    }
}


static void SvcCalib_WbCmdHdlrInit(void)
{
    if ((SVC_CalibWbCtrlFlag & SVC_CALIB_WB_FLG_CMD_INIT) == 0U) {

        AmbaSvcWrap_MisraMemset(SvcCalibWbCmdHdlr, 0, sizeof(SvcCalibWbCmdHdlr));

        SvcCalibWbCmdHdlr[SVC_CALIB_CMD_MEM_QUERY     ] = (SVC_CALIB_WB_CMD_HDLR_s) {1, SVC_CALIB_CMD_MEM_QUERY,       SvcCalib_WbCmdMemQuery   };
        SvcCalibWbCmdHdlr[SVC_CALIB_CMD_ITEM_CREATE   ] = (SVC_CALIB_WB_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_CREATE,     SvcCalib_WbCmdCreate     };
        SvcCalibWbCmdHdlr[SVC_CALIB_CMD_ITEM_DATA_INIT] = (SVC_CALIB_WB_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_DATA_INIT,  SvcCalib_WbCmdDataInit   };
        SvcCalibWbCmdHdlr[SVC_CALIB_CMD_ITEM_TBL_GET  ] = (SVC_CALIB_WB_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_TBL_GET,    SvcCalib_WbCmdTblDataGet };
        SvcCalibWbCmdHdlr[SVC_CALIB_CMD_ITEM_TBL_SET  ] = (SVC_CALIB_WB_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_TBL_SET,    SvcCalib_WbCmdTblDataSet };

        SVC_CalibWbCtrlFlag |= SVC_CALIB_WB_FLG_CMD_INIT;
    }
}

static UINT32 SvcCalib_WbCmdMemQuery(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    if (pParam1 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to query wb memory - input wb table number should not zero" PRN_CAL_WB_NG
    } else if (pParam2 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to query wb memory - output wb shadow buffer size should not null!" PRN_CAL_WB_NG
    } else if (pParam3 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to query wb memory - output wb working buffer size should not null!" PRN_CAL_WB_NG
    } else if (pParam4 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to query wb memory - wb calculation buffer size should not null!" PRN_CAL_WB_NG
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

        RetVal = SvcCalib_WbMemQuery(NomOfTable, pShadowBufSize, pWorkingBufSize, pCalcMemSize);
        if (RetVal != SVC_OK) {
            PRN_CAL_WB_LOG "Fail to query wb memory - query memory fail!" PRN_CAL_WB_NG
        }

        AmbaMisra_TouchUnused(pParam1);
        AmbaMisra_TouchUnused(pParam2);
        AmbaMisra_TouchUnused(pParam3);
        AmbaMisra_TouchUnused(pParam4);
        AmbaMisra_TouchUnused(pU32Val);
    }

    return RetVal;
}


static UINT32 SvcCalib_WbCmdCreate(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SvcCalib_WbCreate();
    if (RetVal != SVC_OK) {
        PRN_CAL_WB_LOG "Fail to create wb module fail!" PRN_CAL_WB_NG
    } else {
        AmbaMisra_TouchUnused(pParam1);
        AmbaMisra_TouchUnused(pParam2);
        AmbaMisra_TouchUnused(pParam3);
        AmbaMisra_TouchUnused(pParam4);
    }

    return RetVal;
}


static UINT32 SvcCalib_WbCmdDataInit(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    if (pParam1 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to init wb - input calib object should not null!" PRN_CAL_WB_NG
    } else {
        const SVC_CALIB_OBJ_s *pCalObj;

        AmbaMisra_TypeCast(&pCalObj, &pParam1);
        if (pCalObj == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_WB_LOG "Fail to init wb - invalid calib obj!" PRN_CAL_WB_NG
        } else {
            RetVal = SvcCalib_WbShadowInit(pCalObj);
            if (RetVal != SVC_OK) {
                PRN_CAL_WB_LOG "Fail to init wb - shadow initial fail!" PRN_CAL_WB_NG
            } else {
                SVC_CALIB_CALC_CB_f CbFunc = NULL;
                AmbaMisra_TypeCast(&(CbFunc), &(pParam2));
                SVC_CalibWbCtrl.CalcCbFunc = CbFunc;
            }
        }

        AmbaMisra_TouchUnused(pParam1);
        AmbaMisra_TouchUnused(pParam2);
        AmbaMisra_TouchUnused(pParam3);
        AmbaMisra_TouchUnused(pParam4);
    }

    return RetVal;
}

static UINT32 SvcCalib_WbCmdTblDataGet(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;
    UINT32 *pU32Val = NULL;
    SVC_CALIB_TBL_INFO_s *pTblInfo;

    AmbaMisra_TypeCast(&(pU32Val), &(pParam1));
    AmbaMisra_TypeCast(&(pTblInfo), &(pParam2));

    if (pU32Val == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to get table data - input table id should not null!" PRN_CAL_WB_NG
    } else {
        RetVal = SvcCalib_WbTableDataGet(*pU32Val, pTblInfo);
    }

    AmbaMisra_TouchUnused(pParam1);
    AmbaMisra_TouchUnused(pParam2);
    AmbaMisra_TouchUnused(pParam3);
    AmbaMisra_TouchUnused(pParam4);
    AmbaMisra_TouchUnused(pU32Val);

    return RetVal;
}


static UINT32 SvcCalib_WbCmdTblDataSet(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;
    UINT32 *pU32Val = NULL;
    SVC_CALIB_TBL_INFO_s *pTblInfo;

    AmbaMisra_TypeCast(&(pU32Val), &(pParam1));
    AmbaMisra_TypeCast(&(pTblInfo), &(pParam2));

    if (pU32Val == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to set table data - input table id should not null!" PRN_CAL_WB_NG
    } else {
        RetVal = SvcCalib_WbTableDataSet(*pU32Val, pTblInfo);
    }

    AmbaMisra_TouchUnused(pParam1);
    AmbaMisra_TouchUnused(pParam2);
    AmbaMisra_TouchUnused(pParam3);
    AmbaMisra_TouchUnused(pParam4);
    AmbaMisra_TouchUnused(pU32Val);

    return RetVal;
}

/**
 * calib wb command entry
 *
 * @param [in] CmdID  the bpc command id
 * @param [in] Param1 no.01 parameter
 * @param [in] Param2 no.02 parameter
 * @param [in] Param3 no.03 parameter
 * @param [in] Param4 no.04 parameter
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcCalib_WbCmdFunc(UINT32 CmdID, void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    SvcCalib_WbCmdHdlrInit();

    if (CmdID >= SVC_CALIB_CMD_NUM) {
        RetVal = SVC_NG;
        PRN_CAL_WB_LOG "Fail to handler cmd - invalid command id(%d)"
            PRN_CAL_WB_ARG_UINT32 CmdID PRN_CAL_WB_ARG_POST
        PRN_CAL_WB_NG
    } else {
        const UINT32 HdlrCount = (UINT32)(sizeof(SvcCalibWbCmdHdlr)) / ((UINT32)sizeof(SvcCalibWbCmdHdlr[0]));
        UINT32 HdlrIdx;

        for (HdlrIdx = 0U; HdlrIdx < HdlrCount; HdlrIdx ++) {
            if ((SvcCalibWbCmdHdlr[HdlrIdx].Enable > 0U) &&
                (SvcCalibWbCmdHdlr[HdlrIdx].CmdID == CmdID) &&
                (SvcCalibWbCmdHdlr[HdlrIdx].pHandler != NULL)) {
                RetVal = (SvcCalibWbCmdHdlr[HdlrIdx].pHandler)(pParam1, pParam2, pParam3, pParam4);
            }
        }
    }

    return RetVal;
}
