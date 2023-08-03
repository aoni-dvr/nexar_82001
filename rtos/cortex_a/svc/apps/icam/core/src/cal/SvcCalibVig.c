/**
*  @file SvcCalibVig.c
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
*  @details C file for Calibration Vignette
*
*/

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaVIN_Def.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaFS.h"

#include "AmbaCalib_ErrNo.h"
#include "AmbaCalib_VignetteCV2IF.h"

#include "AmbaCT_TextHdlr.h"
#include "AmbaCT_1DVigTunerIF.h"

#include "SvcWrap.h"
#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcCalibMgr.h"

#define SVC_CALIB_VIG_FLG_INIT              (0x1U)
#define SVC_CALIB_VIG_FLG_SHADOW_INIT       (0x2U)
#define SVC_CALIB_VIG_FLG_CALC_INIT         (0x4U)
#define SVC_CALIB_VIG_FLG_CALC_VIN_SEN      (0x8U)
#define SVC_CALIB_VIG_FLG_CALC_CTN_MAX_WIN  (0x10U)
#define SVC_CALIB_VIG_FLG_SHELL_INIT        (0x100U)
#define SVC_CALIB_VIG_FLG_CMD_INIT          (0x200U)
/* Reserve 0x10000/0x20000/0x40000/0x80000 for SvcCalibMgr.h */

#define SVC_CALIB_VIG_NAME               ("SvcCalib_Vig")
#define SVC_CALIB_VIG_NAND_HEADER_SIZE   (0x200U)
#define SVC_CALIB_VIG_MAX_CHANNEL_NUM    (4U)
#define SVC_CALIB_VIG_VERSION            (0x20180401U)
#define SVC_CALIB_VIG_MAX_RAW_WIDTH      (3840U)
#define SVC_CALIB_VIG_MAX_RAW_HEIGHT     (2160U)
#define SVC_CALIB_VIG_MAX_RESOLUTION     (20U)

typedef struct {
    UINT32 Enable;
    UINT32 NumOfTable;
    UINT32 DebugMsgOn;
    UINT32 TableSelectBits;
    UINT32 Reserved[(SVC_CALIB_VIG_NAND_HEADER_SIZE / 4U) - 4U];
} SVC_CALIB_VIG_NAND_HEADER_s;

typedef struct {
    UINT32                     AlgoVersion;
    UINT32                     Version;
    SVC_CALIB_VIN_SENSOR_GEO_s VinSensorGeo;
    UINT32                     RadialCoarse;
    UINT32                     RadialCoarseLog;
    UINT32                     RadialBinsFine;
    UINT32                     RadialBinsFineLog;
    UINT32                     ModelCenterX_R;
    UINT32                     ModelCenterX_Gr;
    UINT32                     ModelCenterX_B;
    UINT32                     ModelCenterX_Gb;
    UINT32                     ModelCenterY_R;
    UINT32                     ModelCenterY_Gr;
    UINT32                     ModelCenterY_B;
    UINT32                     ModelCenterY_Gb;
    UINT32                     SyncCalInfo[4];
    UINT32                     VinSelectBits;
    UINT32                     SensorSelectBits;
} SVC_CALIB_VIG_TABLE_HEADER_s;

typedef struct {
    SVC_CALIB_VIG_TABLE_HEADER_s Header;
    UINT32                       GainTblR[4][128];
    UINT32                       GainTblGr[4][128];
    UINT32                       GainTblB[4][128];
    UINT32                       GainTblGb[4][128];
} SVC_CALIB_VIG_NAND_TABLE_s;

typedef struct {
    UINT32                       VinSelectBits;
    UINT32                       SensorSelectBits;
    UINT32                       SourceNandIdx;
    AMBA_IK_VIGNETTE_s           VigTable;
} SVC_CALIB_VIG_TABLE_s;

typedef struct {
    UINT32 TblSelectBits;
    UINT32 DiffThreshold;
} SVC_CALIB_VIG_SYNC_TBL_s;

typedef struct {
    UINT32                          TableIdx;
    SVC_CALIB_VIG_SYNC_TBL_s        SyncTbl;
    UINT32                          SaveBinOn;
    UINT32                          MaxRawWidth;
    UINT32                          MaxRawHeight;
    UINT32                          MaxResolution;
} SVC_CALIB_VIG_CALC_CTRL_s;

#define SVC_CALIB_VIG_MEM_CALC        (0x0U)
#define SVC_CALIB_VIG_MEM_CALC_CTUNER (0x1U)
#define SVC_CALIB_VIG_MEM_NUM         (0x2U)
typedef struct {
    UINT32  Enable;
    UINT8  *pBuf;
    UINT32  BufSize;
} SVC_CALIB_VIG_MEM_CTRL_s;

typedef struct {
    char                           Name[16];
    AMBA_KAL_MUTEX_t               Mutex;
    SVC_CALIB_VIG_NAND_HEADER_s   *pNandHeader;
    SVC_CALIB_VIG_NAND_TABLE_s    *pNandTable;
    UINT32                         NumOfTable;
    SVC_CALIB_VIG_TABLE_s         *pVigTable;

    SVC_CALIB_VIG_MEM_CTRL_s       MemCtrl[SVC_CALIB_VIG_MEM_NUM];
    SVC_CALIB_VIG_CALC_CTRL_s      CalcCtrl;
    SVC_CALIB_CALC_CB_f            CalcCbFunc;
    SVC_CALIB_CHANNEL_s            CalibChan[SVC_CALIB_VIG_MAX_CHANNEL_NUM];
    UINT32                         EnableDebugMsg;
} SVC_CALIB_VIG_CTRL_s;

static UINT32                  SvcCalib_DriverSlotCmp(char Val0, char Val1);
static UINT32                  SvcCalib_VigMutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut);
static void                    SvcCalib_VigMutexGive(AMBA_KAL_MUTEX_t *pMutex);
static void                    SvcCalib_VigErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode);

static UINT32                  SvcCalib_VigMemQuery(UINT32 MaxTableNum, UINT32 *pShadowSize, UINT32 *pWorkMemSize, UINT32 *pCalcMemSize);
static UINT32                  SvcCalib_VigMemLock(void);
static void                    SvcCalib_VigMemUnLock(void);
static UINT32                  SvcCalib_VigCtnPreFetch(const char *pScriptPath);

static UINT32                  SvcCalib_VigVinSensorIDCheck(UINT32 VinID, UINT32 SensorID);
static UINT32                  SvcCalib_VigCreate(void);
static UINT32                  SvcCalib_VigShadowInit(const SVC_CALIB_OBJ_s *pCalObj);
static UINT32                  SvcCalib_VigTableUpdate(const SVC_CALIB_CHANNEL_s *pCalibChan, const AMBA_IK_MODE_CFG_s *pImgMode);
static UINT32                  SvcCalib_VigTableCfg(UINT32 NandTableID, UINT32 TableID);
static SVC_CALIB_VIG_TABLE_s * SvcCalib_VigTableGet(const SVC_CALIB_CHANNEL_s *pCalibChan);
static UINT32                  SvcCalib_VigTableGenTbl(const SVC_CALIB_CHANNEL_s *pCalibChan, const char *pScriptPath);
static UINT32                  SvcCalib_VigTableSearch(UINT32 VinID, UINT32 SensorID, UINT32 *pTblIdx);
static UINT32                  SvcCalib_VigNandTableSearch(UINT32 VinID, UINT32 SensorID, UINT32 *pTblIdx);
static UINT32                  SvcCalib_VigTableSyncMemCfg(UINT8 *pMemBuf, UINT32 MemBufSize,
                                                           UINT32 *pNumOfTbl, UINT32 **pTblIDArr,
                                                           AMBA_CAL_1D_VIG_CALIB_DATA_V1_s **pSrcTblArr,
                                                           AMBA_CAL_1D_VIG_CALIB_DATA_V1_s **pDstTblArr);
static UINT32                  SvcCalib_VigTableSync(void);
static UINT32                  SvcCalib_VigTableDataGet(UINT32 TblID, SVC_CALIB_TBL_INFO_s *pTblInfo);
static UINT32                  SvcCalib_VigTableDataSet(UINT32 TblID, SVC_CALIB_TBL_INFO_s *pTblInfo);

// Private function definition for calibration command
typedef UINT32 (*SVC_CALIB_VIG_CMD_FUNC_f)(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
typedef struct {
     UINT32                   Enable;
     UINT32                   CmdID;
     SVC_CALIB_VIG_CMD_FUNC_f pHandler;
} SVC_CALIB_VIG_CMD_HDLR_s;

static void   SvcCalib_VigCmdHdlrInit(void);
static UINT32 SvcCalib_VigCmdMemQuery(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_VigCmdCreate(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_VigCmdDataInit(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_VigCmdTblUpdate(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_VigCmdTblDataGet(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_VigCmdTblDataSet(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
//UINT32 SvcCalib_VigCmdFunc(UINT32 CmdID, void *pParam1, void *pParam2, void *pParam3, void *pParam4);

// Private function definition for shell command.
typedef UINT32 (*SVC_CALIB_VIG_SHELL_FUNC_f)(UINT32 ArgCount, char * const *pArgVector);
typedef void   (*SVC_CALIB_VIG_SHELL_USAGE_f)(void);

typedef struct {
    UINT32                      Enable;
    char                        ShellCmdName[32];
    SVC_CALIB_VIG_SHELL_FUNC_f  pFunc;
    SVC_CALIB_VIG_SHELL_USAGE_f pUsageFunc;
} SVC_CALIB_VIG_SHELL_FUNC_s;

static void   SvcCalib_VigShellStrToU32(const char *pString, UINT32 *pValue);
static UINT32 SvcCalib_VigShellCfgCalChan(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_VigShellCfgCalChanU(void);
static UINT32 SvcCalib_VigShellCfgTblID(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_VigShellCfgTblIDU(void);
static UINT32 SvcCalib_VigShellCfgDbgMsg(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_VigShellCfgDbgMsgU(void);
static UINT32 SvcCalib_VigShellCfgSyncTbl(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_VigShellCfgSyncTblU(void);
static UINT32 SvcCalib_VigShellGenTbl(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_VigShellGenTblU(void);
static UINT32 SvcCalib_VigShellEnable(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_VigShellEnableU(void);
static UINT32 SvcCalib_VigShellUpd(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_VigShellUpdU(void);
static UINT32 SvcCalib_VigShellSet(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_VigShellSetU(void);
static void   SvcCalib_VigShellEntryInit(void);
static void   SvcCalib_VigShellUsage(void);

#define SVC_CALIB_VIG_SHELL_CMD_NUM  (8U)
static SVC_CALIB_VIG_SHELL_FUNC_s CalibVigShellFunc[SVC_CALIB_VIG_SHELL_CMD_NUM] GNU_SECTION_NOZEROINIT;
static SVC_CALIB_VIG_CMD_HDLR_s SvcCalibVigCmdHdlr[SVC_CALIB_CMD_NUM] GNU_SECTION_NOZEROINIT;
static SVC_CALIB_VIG_CTRL_s SVC_CalibVigCtrl GNU_SECTION_NOZEROINIT;
static UINT32 SVC_CalibVigCtrlFlag = SVC_LOG_CAL_DEF_FLG;

#define SVC_LOG_VIG "VIG"
#define PRN_CAL_VIG_LOG        { SVC_WRAP_PRINT_s CalibVigPrint; AmbaSvcWrap_MisraMemset(&(CalibVigPrint), 0, sizeof(CalibVigPrint)); CalibVigPrint.Argc --; CalibVigPrint.pStrFmt =
#define PRN_CAL_VIG_ARG_UINT32 ; CalibVigPrint.Argc ++; CalibVigPrint.Argv[CalibVigPrint.Argc].Uint64   = (UINT64)((
#define PRN_CAL_VIG_ARG_CSTR   ; CalibVigPrint.Argc ++; CalibVigPrint.Argv[CalibVigPrint.Argc].pCStr    = ((
#define PRN_CAL_VIG_ARG_CPOINT ; CalibVigPrint.Argc ++; CalibVigPrint.Argv[CalibVigPrint.Argc].pPointer = ((
#define PRN_CAL_VIG_ARG_POST   ))
#define PRN_CAL_VIG_OK         ; CalibVigPrint.Argc ++; SvcCalib_VigPrintLog(SVC_LOG_CAL_OK , &(CalibVigPrint)); }
#define PRN_CAL_VIG_NG         ; CalibVigPrint.Argc ++; SvcCalib_VigPrintLog(SVC_LOG_CAL_NG , &(CalibVigPrint)); }
#define PRN_CAL_VIG_API        ; CalibVigPrint.Argc ++; SvcCalib_VigPrintLog(SVC_LOG_CAL_API, &(CalibVigPrint)); }
#define PRN_CAL_VIG_DBG        ; CalibVigPrint.Argc ++; SvcCalib_VigPrintLog(SVC_LOG_CAL_DBG, &(CalibVigPrint)); }
#define PRN_CAL_VIG_ERR_HDLR   SvcCalib_VigErrHdlr(__func__, __LINE__, PRetVal);

static void SvcCalib_VigPrintLog(UINT32 LogLevel, SVC_WRAP_PRINT_s *pPrint)
{
    if (pPrint != NULL) {
        if ((SVC_CalibVigCtrlFlag & LogLevel) > 0U) {

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

            SvcWrap_Print(SVC_LOG_VIG, pPrint);
        }
    }
}

static INT32 SvcCalib_CvtCharToINT32(char Char)
{
    UINT32 Val = 0;

    AmbaMisra_TypeCast(&Val, &Char); Val &= 0xFFU;

    return (INT32)Val;
}

static UINT32 SvcCalib_DriverSlotCmp(char Val0, char Val1)
{
    UINT32 RetCmp;
    INT32 CurVal0, CurVal1;
    const INT32 CHAR_a = 97;
    const INT32 CHAR_A = 65;

    if ((Val0 >= 'a') && (Val0 <= 'z')) {
        CurVal0 = CHAR_A + (SvcCalib_CvtCharToINT32(Val0) - CHAR_a);
    } else if ((Val0 >= 'A' ) && (Val0 <= 'Z')) {
        CurVal0 = SvcCalib_CvtCharToINT32(Val0);
    } else {
        CurVal0 = 0;
    }

    if ((Val1 >= 'a') && (Val1 <= 'z')) {
        CurVal1 = CHAR_A + (SvcCalib_CvtCharToINT32(Val1) - CHAR_a);
    } else if ((Val1 >= 'A' ) && (Val1 <= 'Z')) {
        CurVal1 = SvcCalib_CvtCharToINT32(Val1);
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

static UINT32 SvcCalib_VigMutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut)
{
    return AmbaKAL_MutexTake(pMutex, TimeOut);
}

static void SvcCalib_VigMutexGive(AMBA_KAL_MUTEX_t *pMutex)
{
    UINT32 ErrCode = AmbaKAL_MutexGive(pMutex);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }
}

static void SvcCalib_VigErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode)
{
    if (pCaller != NULL) {
        if (ErrCode != 0U) {
            PRN_CAL_VIG_LOG "Catch ErrCode(0x%08x) @ %s, %d"
                PRN_CAL_VIG_ARG_UINT32 ErrCode  PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_ARG_CSTR   pCaller  PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_ARG_UINT32 CodeLine PRN_CAL_VIG_ARG_POST
            PRN_CAL_VIG_NG
        }
    }
}

static UINT32 SvcCalib_VigVinSensorIDCheck(UINT32 VinID, UINT32 SensorID)
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


static UINT32 SvcCalib_VigCreate(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibVigCtrlFlag & SVC_CALIB_VIG_FLG_INIT) > 0U) {
        RetVal = SVC_OK;
        PRN_CAL_VIG_LOG "Calibration VIG module has been created!" PRN_CAL_VIG_DBG
    } else {
        // Reset the vig control module
        AmbaSvcWrap_MisraMemset(&SVC_CalibVigCtrl, 0, sizeof(SVC_CalibVigCtrl));

        // Configure the name
        SvcWrap_strcpy(SVC_CalibVigCtrl.Name, sizeof(SVC_CalibVigCtrl.Name), SVC_CALIB_VIG_NAME);

        // Create the mutex
        RetVal = AmbaKAL_MutexCreate(&(SVC_CalibVigCtrl.Mutex), SVC_CalibVigCtrl.Name);
        if (RetVal != SVC_OK) {
            PRN_CAL_VIG_LOG "Fail to create vig - create mutex fail!" PRN_CAL_VIG_NG
        } else {
            PRetVal = (SVC_CalibVigCtrlFlag & (SVC_CALIB_VIG_FLG_SHELL_INIT |
                                               SVC_CALIB_VIG_FLG_CMD_INIT   |
                                               SVC_LOG_CAL_DEF_FLG          |
                                               SVC_LOG_CAL_DBG));
            SVC_CalibVigCtrlFlag = PRetVal;
            SVC_CalibVigCtrlFlag |= SVC_CALIB_VIG_FLG_INIT;
            PRN_CAL_VIG_LOG "Successful to create vig!" PRN_CAL_VIG_OK
        }
    }

    return RetVal;
}


static UINT32 SvcCalib_VigShadowInit(const SVC_CALIB_OBJ_s *pCalObj)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibVigCtrlFlag & SVC_CALIB_VIG_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to initial vig dram shodow - create vig first!" PRN_CAL_VIG_NG
    } else if (pCalObj == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to initial vig dram shodow - invalid calib object!" PRN_CAL_VIG_NG
    } else if (pCalObj->Enable == 0U) {
        RetVal = 99;
        PRN_CAL_VIG_LOG "Disable vig dram shadow!!" PRN_CAL_VIG_API
    } else {
        UINT32 DramShadowSize = 0, WorkingMemSize = 0;
        const SVC_CALIB_VIG_NAND_HEADER_s *pHeader;
        AmbaMisra_TypeCast(&(pHeader), &(pCalObj->pShadowBuf));

        RetVal = SvcCalib_VigMemQuery(pCalObj->NumOfTable, &DramShadowSize, &WorkingMemSize, NULL);
        if (RetVal != SVC_OK) {
            PRN_CAL_VIG_LOG "Fail to initial vig dram shadow - query memory fail!" PRN_CAL_VIG_NG
        } else if ((pCalObj->pShadowBuf == NULL) || (pHeader == NULL)) {
            RetVal = SVC_NG;
            PRN_CAL_VIG_LOG "Fail to initial vig dram shadow - dram shadow buffer should not null!" PRN_CAL_VIG_NG
        } else if (pCalObj->ShadowBufSize < DramShadowSize) {
            RetVal = SVC_NG;
            PRN_CAL_VIG_LOG "Fail to initial vig dram shadow - prepare dram shadow size is too small!" PRN_CAL_VIG_NG
        } else if (pHeader->NumOfTable > pCalObj->NumOfTable) {
            RetVal = SVC_NG;
            PRN_CAL_VIG_LOG "Fail to initial vig dram shadow - calib nand table count(%d) > max vig dram shadow table count(%d)"
                PRN_CAL_VIG_ARG_UINT32 pHeader->NumOfTable PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_ARG_UINT32 pCalObj->NumOfTable PRN_CAL_VIG_ARG_POST
            PRN_CAL_VIG_NG
        } else if (pCalObj->pWorkingBuf == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_VIG_LOG "Fail to initial vig dram shadow - working buffer should not null!" PRN_CAL_VIG_NG
        } else if (pCalObj->WorkingBufSize < WorkingMemSize) {
            RetVal = SVC_NG;
            PRN_CAL_VIG_LOG "Fail to initial vig dram shadow - working buffer size is too small!" PRN_CAL_VIG_NG
        } else if (KAL_ERR_NONE != SvcCalib_VigMutexTake(&(SVC_CalibVigCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
            RetVal = SVC_NG;
            PRN_CAL_VIG_LOG "Fail to initial vig dram shadow - take mutex fail!" PRN_CAL_VIG_NG
        } else {
            UINT8 *pNandHeaderBuf    = pCalObj->pShadowBuf;
            UINT32 NandHeaderSize    = (UINT32)(sizeof(SVC_CALIB_VIG_NAND_HEADER_s));
            UINT8 *pNandTableBuf     = &(pNandHeaderBuf[NandHeaderSize]);
            UINT32 TotalVigTableSize = (UINT32)(sizeof(SVC_CALIB_VIG_TABLE_s)) * pCalObj->NumOfTable;

            PRN_CAL_VIG_LOG "====== Shadow initialized Start ======" PRN_CAL_VIG_DBG
            PRN_CAL_VIG_LOG "  DramShadow Addr  : %p 0x%X"
                PRN_CAL_VIG_ARG_CPOINT pCalObj->pShadowBuf    PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_ARG_UINT32 pCalObj->ShadowBufSize PRN_CAL_VIG_ARG_POST
            PRN_CAL_VIG_DBG
            PRN_CAL_VIG_LOG "    Nand Header Addr : %p 0x%X"
                PRN_CAL_VIG_ARG_CPOINT pNandHeaderBuf PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_ARG_UINT32 NandHeaderSize PRN_CAL_VIG_ARG_POST
            PRN_CAL_VIG_DBG
            PRN_CAL_VIG_LOG "    Nand Table Addr  : %p"
                PRN_CAL_VIG_ARG_CPOINT pNandTableBuf PRN_CAL_VIG_ARG_POST
            PRN_CAL_VIG_DBG
            PRN_CAL_VIG_LOG "  Working Mem Addr : %p 0x%X"
                PRN_CAL_VIG_ARG_CPOINT pCalObj->pWorkingBuf    PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_ARG_UINT32 pCalObj->WorkingBufSize PRN_CAL_VIG_ARG_POST
            PRN_CAL_VIG_DBG
            PRN_CAL_VIG_LOG "    Vig Table Addr   : %p 0x%08X"
                PRN_CAL_VIG_ARG_CPOINT pCalObj->pWorkingBuf PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_ARG_UINT32 TotalVigTableSize    PRN_CAL_VIG_ARG_POST
            PRN_CAL_VIG_DBG

            // Configure nand header
            AmbaMisra_TypeCast(&(SVC_CalibVigCtrl.pNandHeader), &(pNandHeaderBuf));
            // Configure nand table
            AmbaMisra_TypeCast(&(SVC_CalibVigCtrl.pNandTable),  &(pNandTableBuf)); AmbaMisra_TouchUnused(pNandTableBuf);
            // Configure working vig table
            AmbaMisra_TypeCast(&(SVC_CalibVigCtrl.pVigTable),  &(pCalObj->pWorkingBuf));
            if (SVC_CalibVigCtrl.pNandHeader == NULL) {
                RetVal = SVC_NG;
                PRN_CAL_VIG_LOG "Fail to initial vig dram shadow - nand header should not null!" PRN_CAL_VIG_NG
            } else if (SVC_CalibVigCtrl.pNandTable == NULL) {
                RetVal = SVC_NG;
                PRN_CAL_VIG_LOG "Fail to initial vig dram shadow - nand table should not null!" PRN_CAL_VIG_NG
            } else if (SVC_CalibVigCtrl.pVigTable == NULL) {
                RetVal = SVC_NG;
                PRN_CAL_VIG_LOG "Fail to initial vig dram shadow - vig table should not null!" PRN_CAL_VIG_NG
            } else {

                SVC_CalibVigCtrl.NumOfTable = pCalObj->NumOfTable;
                // Reset the working vig table memory
                AmbaSvcWrap_MisraMemset(SVC_CalibVigCtrl.pVigTable, 0, TotalVigTableSize);

                if (SVC_CalibVigCtrl.pNandHeader->DebugMsgOn > 0U) {
                    SVC_CalibVigCtrl.EnableDebugMsg |= SVC_CalibVigCtrl.pNandHeader->DebugMsgOn;
                }

                if (SVC_CalibVigCtrl.EnableDebugMsg > 0U) {

                    PRN_CAL_VIG_LOG " " PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "---- Vig Nand Info ----"
                        PRN_CAL_VIG_ARG_UINT32 SVC_CalibVigCtrl.pNandHeader->Reserved[0] PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  Enable         : %d"
                        PRN_CAL_VIG_ARG_UINT32 SVC_CalibVigCtrl.pNandHeader->Enable PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  NumOfTable     : %d"
                        PRN_CAL_VIG_ARG_UINT32 SVC_CalibVigCtrl.pNandHeader->NumOfTable PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  DebugMsgOn     : %d"
                        PRN_CAL_VIG_ARG_UINT32 SVC_CalibVigCtrl.pNandHeader->DebugMsgOn PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  TblSelectButs  : 0x%X"
                        PRN_CAL_VIG_ARG_UINT32 SVC_CalibVigCtrl.pNandHeader->TableSelectBits PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API

                    if (SVC_CalibVigCtrl.pNandTable != NULL) {
                        UINT32 Idx;
                        const SVC_CALIB_VIG_NAND_TABLE_s *pNandTable;

                        for (Idx = 0U; Idx < pCalObj->NumOfTable; Idx ++) {
                            pNandTable = &(SVC_CalibVigCtrl.pNandTable[Idx]);

                            PRN_CAL_VIG_LOG "  ---- Nand Table[%d] %p -----"
                                PRN_CAL_VIG_ARG_UINT32 Idx        PRN_CAL_VIG_ARG_POST
                                PRN_CAL_VIG_ARG_CPOINT pNandTable PRN_CAL_VIG_ARG_POST
                            PRN_CAL_VIG_API
                            PRN_CAL_VIG_LOG "    VinSelectBits       : 0x%X"
                                PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.VinSelectBits PRN_CAL_VIG_ARG_POST
                            PRN_CAL_VIG_API
                            PRN_CAL_VIG_LOG "    SensorSelectBits    : 0x%X"
                                PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.SensorSelectBits PRN_CAL_VIG_ARG_POST
                            PRN_CAL_VIG_API
                            PRN_CAL_VIG_LOG "    Version             : 0x%X"
                                PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.Version PRN_CAL_VIG_ARG_POST
                            PRN_CAL_VIG_API
                            PRN_CAL_VIG_LOG "    VinSensorGeo.StartX : %d"
                                PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.VinSensorGeo.StartX PRN_CAL_VIG_ARG_POST
                            PRN_CAL_VIG_API
                            PRN_CAL_VIG_LOG "    VinSensorGeo.StartY : %d"
                                PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.VinSensorGeo.StartY PRN_CAL_VIG_ARG_POST
                            PRN_CAL_VIG_API
                            PRN_CAL_VIG_LOG "    VinSensorGeo.Width  : %d"
                                PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.VinSensorGeo.Width PRN_CAL_VIG_ARG_POST
                            PRN_CAL_VIG_API
                            PRN_CAL_VIG_LOG "    VinSensorGeo.Height : %d"
                                PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.VinSensorGeo.Height PRN_CAL_VIG_ARG_POST
                            PRN_CAL_VIG_API
                        }
                    }
                }


                SVC_CalibVigCtrlFlag |= SVC_CALIB_VIG_FLG_SHADOW_INIT;
                PRN_CAL_VIG_LOG "Successful to initial vig dram shadow!" PRN_CAL_VIG_OK
            }

            SvcCalib_VigMutexGive(&(SVC_CalibVigCtrl.Mutex));
        }
    }

    return RetVal;
}


static UINT32 SvcCalib_VigTableUpdate(const SVC_CALIB_CHANNEL_s *pCalibChan, const AMBA_IK_MODE_CFG_s *pImgMode)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibVigCtrlFlag & SVC_CALIB_VIG_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to update vig table - initial vig module first!" PRN_CAL_VIG_NG
    } else if ((SVC_CalibVigCtrlFlag & SVC_CALIB_VIG_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to update vig table - initial vig dram shadow first!" PRN_CAL_VIG_NG
    } else if (pCalibChan == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to update vig table - invalid calib channel!" PRN_CAL_VIG_NG
    } else if (pImgMode == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to update vig table - invalid image mode!" PRN_CAL_VIG_NG
    } else if (SVC_CalibVigCtrl.pNandHeader == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to update vig table - invalid nand table!" PRN_CAL_VIG_NG
    } else if ((SVC_CalibVigCtrl.pNandHeader->Enable == 0U) ||
               (SVC_CalibVigCtrl.pNandHeader->NumOfTable == 0U)) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "There is not vig table in shadow buffer!" PRN_CAL_VIG_DBG
    } else if (KAL_ERR_NONE != SvcCalib_VigMutexTake(&(SVC_CalibVigCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to update vig table - take mutex fail!" PRN_CAL_VIG_NG
    } else {
        const SVC_CALIB_VIG_TABLE_s *pVigTable = SvcCalib_VigTableGet(pCalibChan);

        if (pVigTable == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_VIG_LOG "Fail to update vig table - get vig table fail" PRN_CAL_VIG_NG
        } else {
            if (SVC_CalibVigCtrl.EnableDebugMsg > 0U) {
                UINT32 Idx;

                PRN_CAL_VIG_LOG " " PRN_CAL_VIG_API
                PRN_CAL_VIG_LOG "====== Calibration vig info ======" PRN_CAL_VIG_API
                PRN_CAL_VIG_LOG "  VinSelectBits                     : 0x%X"
                    PRN_CAL_VIG_ARG_UINT32 pVigTable->VinSelectBits PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_API
                PRN_CAL_VIG_LOG "  SensorSelectBits                  : 0x%X"
                    PRN_CAL_VIG_ARG_UINT32 pVigTable->SensorSelectBits PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_API
                PRN_CAL_VIG_LOG "  Version                           : 0x%X"
                    PRN_CAL_VIG_ARG_UINT32 pVigTable->VigTable.CalibVignetteInfo.Version PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_API
                PRN_CAL_VIG_LOG "  VinSensorGeo.StartX               : %d"
                    PRN_CAL_VIG_ARG_UINT32 pVigTable->VigTable.VinSensorGeo.StartX PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_API
                PRN_CAL_VIG_LOG "  VinSensorGeo.StartY               : %d"
                    PRN_CAL_VIG_ARG_UINT32 pVigTable->VigTable.VinSensorGeo.StartY PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_API
                PRN_CAL_VIG_LOG "  VinSensorGeo.Width                : %d"
                    PRN_CAL_VIG_ARG_UINT32 pVigTable->VigTable.VinSensorGeo.Width PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_API
                PRN_CAL_VIG_LOG "  VinSensorGeo.Height               : %d"
                    PRN_CAL_VIG_ARG_UINT32 pVigTable->VigTable.VinSensorGeo.Height PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_API
                PRN_CAL_VIG_LOG "  VinSensorGeo.HSubSample.FactorDen : %d"
                    PRN_CAL_VIG_ARG_UINT32 pVigTable->VigTable.VinSensorGeo.HSubSample.FactorDen PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_API
                PRN_CAL_VIG_LOG "  VinSensorGeo.HSubSample.FactorNum : %d"
                    PRN_CAL_VIG_ARG_UINT32 pVigTable->VigTable.VinSensorGeo.HSubSample.FactorNum PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_API
                PRN_CAL_VIG_LOG "  VinSensorGeo.VSubSample.FactorDen : %d"
                    PRN_CAL_VIG_ARG_UINT32 pVigTable->VigTable.VinSensorGeo.VSubSample.FactorDen PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_API
                PRN_CAL_VIG_LOG "  VinSensorGeo.VSubSample.FactorNum : %d"
                    PRN_CAL_VIG_ARG_UINT32 pVigTable->VigTable.VinSensorGeo.VSubSample.FactorNum PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_API
                PRN_CAL_VIG_LOG "  NumRadialBinsCoarse               : %d"
                    PRN_CAL_VIG_ARG_UINT32 pVigTable->VigTable.CalibVignetteInfo.NumRadialBinsCoarse PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_API
                PRN_CAL_VIG_LOG "  SizeRadialBinsCoarseLog           : %d"
                    PRN_CAL_VIG_ARG_UINT32 pVigTable->VigTable.CalibVignetteInfo.SizeRadialBinsCoarseLog PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_API
                PRN_CAL_VIG_LOG "  NumRadialBinsFine                 : %d"
                    PRN_CAL_VIG_ARG_UINT32 pVigTable->VigTable.CalibVignetteInfo.NumRadialBinsFine PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_API
                PRN_CAL_VIG_LOG "  SizeRadialBinsFineLog             : %d"
                    PRN_CAL_VIG_ARG_UINT32 pVigTable->VigTable.CalibVignetteInfo.SizeRadialBinsFineLog PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_API
                PRN_CAL_VIG_LOG "  ModelCenterXR  ModelCenterYR     : %d, %d"
                    PRN_CAL_VIG_ARG_UINT32 pVigTable->VigTable.CalibVignetteInfo.ModelCenterXR PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_ARG_UINT32 pVigTable->VigTable.CalibVignetteInfo.ModelCenterYR PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_API
                PRN_CAL_VIG_LOG "  ModelCenterXGr ModelCenterYGr    : %d, %d"
                    PRN_CAL_VIG_ARG_UINT32 pVigTable->VigTable.CalibVignetteInfo.ModelCenterXGr PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_ARG_UINT32 pVigTable->VigTable.CalibVignetteInfo.ModelCenterYGr PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_API
                PRN_CAL_VIG_LOG "  ModelCenterXB  ModelCenterYB     : %d, %d"
                    PRN_CAL_VIG_ARG_UINT32 pVigTable->VigTable.CalibVignetteInfo.ModelCenterXB PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_ARG_UINT32 pVigTable->VigTable.CalibVignetteInfo.ModelCenterYB PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_API
                PRN_CAL_VIG_LOG "  ModelCenterXGb ModelCenterYGb    : %d, %d"
                    PRN_CAL_VIG_ARG_UINT32 pVigTable->VigTable.CalibVignetteInfo.ModelCenterXGb PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_ARG_UINT32 pVigTable->VigTable.CalibVignetteInfo.ModelCenterYGb PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_API

                for (Idx = 0U; Idx < 4U; Idx ++) {
                    PRN_CAL_VIG_LOG "  VigGainTblR[%d]                    : %p"
                        PRN_CAL_VIG_ARG_UINT32 Idx                                                        PRN_CAL_VIG_ARG_POST
                        PRN_CAL_VIG_ARG_CPOINT &(pVigTable->VigTable.CalibVignetteInfo.VigGainTblR[0][0]) PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                }
                for (Idx = 0U; Idx < 4U; Idx ++) {
                    PRN_CAL_VIG_LOG "  VigGainTblGr[%d]                   : %p"
                        PRN_CAL_VIG_ARG_UINT32 Idx                                                         PRN_CAL_VIG_ARG_POST
                        PRN_CAL_VIG_ARG_CPOINT &(pVigTable->VigTable.CalibVignetteInfo.VigGainTblGr[0][0]) PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                }
                for (Idx = 0U; Idx < 4U; Idx ++) {
                    PRN_CAL_VIG_LOG "  VigGainTblB[%d]                    : %p"
                        PRN_CAL_VIG_ARG_UINT32 Idx                                                        PRN_CAL_VIG_ARG_POST
                        PRN_CAL_VIG_ARG_CPOINT &(pVigTable->VigTable.CalibVignetteInfo.VigGainTblB[0][0]) PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                }
                for (Idx = 0U; Idx < 4U; Idx ++) {
                    PRN_CAL_VIG_LOG "  VigGainTblGb[%d]                   : %p"
                        PRN_CAL_VIG_ARG_UINT32 Idx                                                         PRN_CAL_VIG_ARG_POST
                        PRN_CAL_VIG_ARG_CPOINT &(pVigTable->VigTable.CalibVignetteInfo.VigGainTblGb[0][0]) PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                }
            }

            if (0U != AmbaIK_SetVignette(pImgMode, &(pVigTable->VigTable))) {
                RetVal = SVC_NG;
                PRN_CAL_VIG_LOG "Fail to update vig table - set vig info fail!" PRN_CAL_VIG_NG
            } else {
                RetVal = AmbaIK_SetVignetteEnb(pImgMode, 1U);
                if (RetVal != SVC_OK) {
                    PRN_CAL_VIG_LOG "Fail to update vig table - enable warp fail!" PRN_CAL_VIG_NG
                }
            }

            if (RetVal == SVC_OK) {
                PRN_CAL_VIG_LOG "Successful to update the vig table VinID(%d) SensorID(%d) ImgContextID(%d)"
                    PRN_CAL_VIG_ARG_UINT32 pCalibChan->VinID    PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_ARG_UINT32 pCalibChan->SensorID PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_ARG_UINT32 pImgMode->ContextId  PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_OK
            }
        }

        SvcCalib_VigMutexGive(&(SVC_CalibVigCtrl.Mutex));
    }

    return RetVal;
}


static UINT32 SvcCalib_VigTableCfg(UINT32 NandTableID, UINT32 TableID)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibVigCtrlFlag & SVC_CALIB_VIG_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to cfg vignette table - initial vignette module first!" PRN_CAL_VIG_NG
    } else if ((SVC_CalibVigCtrlFlag & SVC_CALIB_VIG_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to cfg vignette table - initial vignette dram shadow first!" PRN_CAL_VIG_NG
    } else if ((SVC_CalibVigCtrl.pVigTable == NULL) || (SVC_CalibVigCtrl.pNandTable == NULL)) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to cfg vignette table - invalid table setting!" PRN_CAL_VIG_NG
    } else if (NandTableID >= SVC_CalibVigCtrl.NumOfTable) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to cfg vignette table - input nand table id is out-of max table size!" PRN_CAL_VIG_NG
    } else if (TableID >= SVC_CalibVigCtrl.NumOfTable) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to cfg vignette table - output vignette table id is out-of max table size!" PRN_CAL_VIG_NG
    } else {
        const SVC_CALIB_VIG_NAND_TABLE_s *pSrcTable = &(SVC_CalibVigCtrl.pNandTable[NandTableID]);
        SVC_CALIB_VIG_TABLE_s            *pDstTable = &(SVC_CalibVigCtrl.pVigTable[TableID]);

        AmbaSvcWrap_MisraMemset(pDstTable, 0, sizeof(SVC_CALIB_VIG_TABLE_s));
        pDstTable->VinSelectBits                              = pSrcTable->Header.VinSelectBits;
        pDstTable->SensorSelectBits                           = pSrcTable->Header.SensorSelectBits;
        pDstTable->SourceNandIdx                              = NandTableID;
        pDstTable->VigTable.CalibModeEnable                   = 1U;
        pDstTable->VigTable.VinSensorGeo.StartX               = pSrcTable->Header.VinSensorGeo.StartX               ;
        pDstTable->VigTable.VinSensorGeo.StartY               = pSrcTable->Header.VinSensorGeo.StartY               ;
        pDstTable->VigTable.VinSensorGeo.Width                = pSrcTable->Header.VinSensorGeo.Width                ;
        pDstTable->VigTable.VinSensorGeo.Height               = pSrcTable->Header.VinSensorGeo.Height               ;
        pDstTable->VigTable.VinSensorGeo.HSubSample.FactorDen = pSrcTable->Header.VinSensorGeo.HSubSample.FactorDen ;
        pDstTable->VigTable.VinSensorGeo.HSubSample.FactorNum = pSrcTable->Header.VinSensorGeo.HSubSample.FactorNum ;
        pDstTable->VigTable.VinSensorGeo.VSubSample.FactorDen = pSrcTable->Header.VinSensorGeo.VSubSample.FactorDen ;
        pDstTable->VigTable.VinSensorGeo.VSubSample.FactorNum = pSrcTable->Header.VinSensorGeo.VSubSample.FactorNum ;
        pDstTable->VigTable.CalibVignetteInfo.Version         = SVC_CALIB_VIG_VERSION;
        pDstTable->VigTable.CalibVignetteInfo.NumRadialBinsCoarse                 = pSrcTable->Header.RadialCoarse     ;
        pDstTable->VigTable.CalibVignetteInfo.SizeRadialBinsCoarseLog             = pSrcTable->Header.RadialCoarseLog  ;
        pDstTable->VigTable.CalibVignetteInfo.NumRadialBinsFine                   = pSrcTable->Header.RadialBinsFine   ;
        pDstTable->VigTable.CalibVignetteInfo.SizeRadialBinsFineLog               = pSrcTable->Header.RadialBinsFineLog;
        pDstTable->VigTable.CalibVignetteInfo.ModelCenterXR                       = pSrcTable->Header.ModelCenterX_R   ;
        pDstTable->VigTable.CalibVignetteInfo.ModelCenterXGr                      = pSrcTable->Header.ModelCenterX_Gr  ;
        pDstTable->VigTable.CalibVignetteInfo.ModelCenterXB                       = pSrcTable->Header.ModelCenterX_B   ;
        pDstTable->VigTable.CalibVignetteInfo.ModelCenterXGb                      = pSrcTable->Header.ModelCenterX_Gb  ;
        pDstTable->VigTable.CalibVignetteInfo.ModelCenterYR                       = pSrcTable->Header.ModelCenterY_R   ;
        pDstTable->VigTable.CalibVignetteInfo.ModelCenterYGr                      = pSrcTable->Header.ModelCenterY_Gr  ;
        pDstTable->VigTable.CalibVignetteInfo.ModelCenterYB                       = pSrcTable->Header.ModelCenterY_B   ;
        pDstTable->VigTable.CalibVignetteInfo.ModelCenterYGb                      = pSrcTable->Header.ModelCenterY_Gb  ;
        AmbaSvcWrap_MisraMemcpy(&(pDstTable->VigTable.CalibVignetteInfo.VigGainTblR[0][0]),  &(pSrcTable->GainTblR[0][0]),  (UINT32)(sizeof(UINT32)) * 4U * 128U);
        AmbaSvcWrap_MisraMemcpy(&(pDstTable->VigTable.CalibVignetteInfo.VigGainTblGr[0][0]), &(pSrcTable->GainTblGr[0][0]), (UINT32)(sizeof(UINT32)) * 4U * 128U);
        AmbaSvcWrap_MisraMemcpy(&(pDstTable->VigTable.CalibVignetteInfo.VigGainTblB[0][0]),  &(pSrcTable->GainTblB[0][0]),  (UINT32)(sizeof(UINT32)) * 4U * 128U);
        AmbaSvcWrap_MisraMemcpy(&(pDstTable->VigTable.CalibVignetteInfo.VigGainTblGb[0][0]), &(pSrcTable->GainTblGb[0][0]), (UINT32)(sizeof(UINT32)) * 4U * 128U);
    }

    return RetVal;
}


static SVC_CALIB_VIG_TABLE_s * SvcCalib_VigTableGet(const SVC_CALIB_CHANNEL_s *pCalibChan)
{
    UINT32 RetVal = SVC_OK;
    SVC_CALIB_VIG_TABLE_s *pTable = NULL;

    if ((SVC_CalibVigCtrlFlag & SVC_CALIB_VIG_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to get vignette table - initial vignette control module first!" PRN_CAL_VIG_NG
    } else if ((SVC_CalibVigCtrlFlag & SVC_CALIB_VIG_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to get vignette table - initial vignette dram shadow first!" PRN_CAL_VIG_NG
    } else if ((SVC_CalibVigCtrl.pVigTable == NULL) ||
               (SVC_CalibVigCtrl.pNandHeader == NULL) ||
               (SVC_CalibVigCtrl.pNandTable == NULL)) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to get vignette table - invalid vignette table setting!" PRN_CAL_VIG_NG
    } else if (SVC_CalibVigCtrl.pNandHeader->NumOfTable == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to get vignette table - there is not table in nand" PRN_CAL_VIG_NG
    } else if (pCalibChan == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to get vignette table - invalid calib channel!" PRN_CAL_VIG_NG
    } else {
        UINT32 Idx;
        UINT32 TableIdx     = 0xFFFFFFFFU;
        UINT32 NandTableIdx = 0xFFFFFFFFU;

        RetVal  = SvcCalib_VigTableSearch(pCalibChan->VinID, pCalibChan->SensorID, &TableIdx);
        RetVal |= SvcCalib_VigNandTableSearch(pCalibChan->VinID, pCalibChan->SensorID, &NandTableIdx);
        if (RetVal == SVC_OK) {
            /* If system cannot get vignette table, need to configure it from nand table */
            if (TableIdx == 0xFFFFFFFFU) {
                if (NandTableIdx < SVC_CalibVigCtrl.NumOfTable) {
                    /* Search the vignette free table */
                    for (Idx = 0; Idx < SVC_CalibVigCtrl.NumOfTable; Idx ++) {
                        if ((SVC_CalibVigCtrl.pVigTable[Idx].VinSelectBits == 0U) &&
                            (SVC_CalibVigCtrl.pVigTable[Idx].SensorSelectBits == 0U)) {
                            TableIdx = Idx;
                            PRN_CAL_VIG_LOG "Successful to get vignette free table(%d)."
                                PRN_CAL_VIG_ARG_UINT32 TableIdx PRN_CAL_VIG_ARG_POST
                            PRN_CAL_VIG_DBG
                            break;
                        }
                    }

                    /* There is not free vignette table to service it */
                    if (TableIdx == 0xFFFFFFFFU) {
                        RetVal = SVC_NG;
                        PRN_CAL_VIG_LOG "Fail to get vignette table - there is not free table to support VinID(%d), SensorID(0x%X)"
                            PRN_CAL_VIG_ARG_UINT32 pCalibChan->VinID    PRN_CAL_VIG_ARG_POST
                            PRN_CAL_VIG_ARG_UINT32 pCalibChan->SensorID PRN_CAL_VIG_ARG_POST
                        PRN_CAL_VIG_NG
                    }
                } else {
                    /* There is not nand table to support it. */
                    RetVal = SVC_NG;
                    PRN_CAL_VIG_LOG "There is not correct vig table Current to support VinID(%d), SensorID(0x%X)"
                        PRN_CAL_VIG_ARG_UINT32 pCalibChan->VinID    PRN_CAL_VIG_ARG_POST
                        PRN_CAL_VIG_ARG_UINT32 pCalibChan->SensorID PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_DBG
                }
            }

            /* Re-configure vig table */
            if ((TableIdx < SVC_CalibVigCtrl.NumOfTable) && (NandTableIdx < SVC_CalibVigCtrl.NumOfTable)) {
                RetVal = SvcCalib_VigTableCfg(NandTableIdx, TableIdx);
                if (RetVal != SVC_OK) {
                    PRN_CAL_VIG_LOG "Fail to get vignette table - re-configure vig table fail!" PRN_CAL_VIG_NG
                    pTable = NULL;
                } else {
                    PRN_CAL_VIG_LOG "Successful to get vignette table - configure done! %d -> %d"
                        PRN_CAL_VIG_ARG_UINT32 NandTableIdx PRN_CAL_VIG_ARG_POST
                        PRN_CAL_VIG_ARG_UINT32 TableIdx     PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_OK
                    pTable = &(SVC_CalibVigCtrl.pVigTable[TableIdx]);
                }
            } else {
                if (TableIdx < SVC_CalibVigCtrl.NumOfTable) {
                    pTable = &(SVC_CalibVigCtrl.pVigTable[TableIdx]);
                    PRN_CAL_VIG_LOG "Successful to get vignette table - existed table!" PRN_CAL_VIG_OK
                }
            }
        }
    }

    AmbaMisra_TouchUnused(&RetVal);

    return pTable;
}


static UINT32 SvcCalib_VigTableGenTbl(const SVC_CALIB_CHANNEL_s *pCalibChan, const char *pScriptPath)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibVigCtrlFlag & SVC_CALIB_VIG_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to calc vig table - Need to initial vig control module first!" PRN_CAL_VIG_NG
    } else if ((SVC_CalibVigCtrlFlag & SVC_CALIB_VIG_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to calc vig table - Need to initial vig dram shadow first!" PRN_CAL_VIG_NG
    } else if (SVC_CalibVigCtrl.pNandHeader == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to calc vig table - invalid nand header!" PRN_CAL_VIG_NG
    } else if (SVC_CalibVigCtrl.pNandTable == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to calc vig table - invalid nand table!" PRN_CAL_VIG_NG
    } else if (SVC_CalibVigCtrl.NumOfTable >= 32U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to calc vig table - NVM table number is out-of range!" PRN_CAL_VIG_NG
    } else if (SVC_CalibVigCtrl.CalcCtrl.TableIdx >= 32U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to calc vig table - request table id is out-of range!" PRN_CAL_VIG_NG
    } else if (pCalibChan == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to calc vig table - invalid calib channel!" PRN_CAL_VIG_NG
    } else if (SVC_OK != SvcCalib_VigCtnPreFetch(pScriptPath)) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to calc vig table - try to fetch raw win from ctuner fail!" PRN_CAL_VIG_NG
    } else if (SVC_OK != SvcCalib_VigMemLock()) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to calc vig table - lock vig memory fail!" PRN_CAL_VIG_NG
    } else {
        if (KAL_ERR_NONE != SvcCalib_VigMutexTake(&(SVC_CalibVigCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
            RetVal = SVC_NG;
            PRN_CAL_VIG_LOG "Fail to calc vig table - take mutex fail!" PRN_CAL_VIG_NG
        } else {
            void *pBuf;
            AMBA_CT_INITIAL_CONFIG_s CtnCfg;

            AmbaSvcWrap_MisraMemset(&CtnCfg, 0, sizeof(CtnCfg));

            AmbaMisra_TypeCast(&(pBuf), &(SVC_CalibVigCtrl.MemCtrl[SVC_CALIB_VIG_MEM_CALC_CTUNER].pBuf));
            CtnCfg.pTunerWorkingBuf    = pBuf;
            CtnCfg.TunerWorkingBufSize = SVC_CalibVigCtrl.MemCtrl[SVC_CALIB_VIG_MEM_CALC_CTUNER].BufSize;

            AmbaMisra_TypeCast(&(pBuf), &(SVC_CalibVigCtrl.MemCtrl[SVC_CALIB_VIG_MEM_CALC].pBuf));
            CtnCfg.Vig1d.pCalibWorkingBuf    = pBuf;
            CtnCfg.Vig1d.CalibWorkingBufSize = SVC_CalibVigCtrl.MemCtrl[SVC_CALIB_VIG_MEM_CALC].BufSize;

            PRetVal = AmbaCT_Init(AMBA_CT_TYPE_1D_VIG, &CtnCfg);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_CAL_VIG_LOG "Fail to calc vig by ctuner - initial ctuner fail! 0x%08X"
                    PRN_CAL_VIG_ARG_UINT32 PRetVal PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_NG
            } else {
                PRetVal = AmbaCT_Load(pScriptPath);
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    PRN_CAL_VIG_LOG "Fail to calc vig by ctuner - load ctuner script fail! 0x%08X"
                        PRN_CAL_VIG_ARG_UINT32 PRetVal PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_NG
                } else {
                    PRetVal = AmbaCT_Execute();
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;
                        PRN_CAL_VIG_LOG "Fail to calc vig by ctuner - execute ctuner fail! 0x%08X"
                            PRN_CAL_VIG_ARG_UINT32 PRetVal PRN_CAL_VIG_ARG_POST
                        PRN_CAL_VIG_NG
                    } else {
                        const AMBA_CAL_1D_VIG_CALIB_DATA_V1_s *pData;
                        const AMBA_CT_1D_VIG_USET_SETTING_s *pVigUserSetting = AmbaCT_1DVigGetUserSetting();
                        AMBA_CT_1D_VIG_CALIB_DATA_V2_s VigData;
                        UINT32 ChanNum = pVigUserSetting->System.ChanNum;
                        UINT32 ChanIdx;
                        UINT32 CurTblIdx = SVC_CalibVigCtrl.CalcCtrl.TableIdx;
                        SVC_CALIB_VIG_NAND_HEADER_s *pNandHeader = SVC_CalibVigCtrl.pNandHeader;
                        SVC_CALIB_VIG_NAND_TABLE_s  *pNandTable;

                        if (ChanNum == 0U) {
                            ChanNum = 1U;
                        }
                        if (ChanNum > SVC_CalibVigCtrl.NumOfTable) {
                            ChanNum = SVC_CalibVigCtrl.NumOfTable;
                        }

                        for (ChanIdx = 0U; ChanIdx < ChanNum; ChanIdx ++) {

                            if (ChanNum > 1U) {
                                CurTblIdx = ChanIdx;
                            }
                            pNandTable = &(SVC_CalibVigCtrl.pNandTable[CurTblIdx]);

                            AmbaSvcWrap_MisraMemset(&VigData, 0, sizeof(VigData));

                            PRetVal = AmbaCT_1DVigGetCalibDataV2(ChanIdx, &VigData); PRN_CAL_VIG_ERR_HDLR

                            pData = VigData.pCalibData;

                            if (pData == NULL) {
                                RetVal = SVC_NG;
                                PRN_CAL_VIG_LOG "Fail to calc vig table - invalid ctuner result!" PRN_CAL_VIG_NG
                            } else {

                                /* Update dram shadow data */
                                AmbaSvcWrap_MisraMemset(pNandTable, 0, sizeof(SVC_CALIB_VIG_NAND_TABLE_s));
                                pNandTable->Header.Version                           = SVC_CALIB_VIG_VERSION;
                                pNandTable->Header.AlgoVersion                       = pData->Version;
                                pNandTable->Header.VinSelectBits                     = SVC_CalibVigCtrl.CalibChan[ChanIdx].VinSelectBits;
                                pNandTable->Header.SensorSelectBits                  = SVC_CalibVigCtrl.CalibChan[ChanIdx].SensorSelectBits;
                                pNandTable->Header.VinSensorGeo.StartX               = pData->CalibSensorGeo.StartX;
                                pNandTable->Header.VinSensorGeo.StartY               = pData->CalibSensorGeo.StartY;
                                pNandTable->Header.VinSensorGeo.Width                = pData->CalibSensorGeo.Width;
                                pNandTable->Header.VinSensorGeo.Height               = pData->CalibSensorGeo.Height;
                                pNandTable->Header.VinSensorGeo.HSubSample.FactorDen = pData->CalibSensorGeo.HSubSample.FactorDen;
                                pNandTable->Header.VinSensorGeo.HSubSample.FactorNum = pData->CalibSensorGeo.HSubSample.FactorNum;
                                pNandTable->Header.VinSensorGeo.VSubSample.FactorDen = pData->CalibSensorGeo.VSubSample.FactorDen;
                                pNandTable->Header.VinSensorGeo.VSubSample.FactorNum = pData->CalibSensorGeo.VSubSample.FactorNum;
                                pNandTable->Header.RadialCoarse                      = pData->RadialCoarse     ;
                                pNandTable->Header.RadialCoarseLog                   = pData->RadialCoarseLog  ;
                                pNandTable->Header.RadialBinsFine                    = pData->RadialBinsFine   ;
                                pNandTable->Header.RadialBinsFineLog                 = pData->RadialBinsFineLog;
                                pNandTable->Header.ModelCenterX_R                    = pData->ModelCenterX_R   ;
                                pNandTable->Header.ModelCenterX_Gr                   = pData->ModelCenterX_Gr  ;
                                pNandTable->Header.ModelCenterX_B                    = pData->ModelCenterX_B   ;
                                pNandTable->Header.ModelCenterX_Gb                   = pData->ModelCenterX_Gb  ;
                                pNandTable->Header.ModelCenterY_R                    = pData->ModelCenterY_R   ;
                                pNandTable->Header.ModelCenterY_Gr                   = pData->ModelCenterY_Gr  ;
                                pNandTable->Header.ModelCenterY_B                    = pData->ModelCenterY_B   ;
                                pNandTable->Header.ModelCenterY_Gb                   = pData->ModelCenterY_Gb  ;
                                pNandTable->Header.SyncCalInfo[0]                    = pData->SyncCalInfo[0];
                                pNandTable->Header.SyncCalInfo[1]                    = pData->SyncCalInfo[1];
                                pNandTable->Header.SyncCalInfo[2]                    = pData->SyncCalInfo[2];
                                pNandTable->Header.SyncCalInfo[3]                    = pData->SyncCalInfo[3];
                                AmbaSvcWrap_MisraMemcpy(&(pNandTable->GainTblR[0][0]),  &(pData->VigGainTbl_R[0][0]),  (UINT32)(sizeof(UINT32)) * 4U * 128U);
                                AmbaSvcWrap_MisraMemcpy(&(pNandTable->GainTblGr[0][0]), &(pData->VigGainTbl_Gr[0][0]), (UINT32)(sizeof(UINT32)) * 4U * 128U);
                                AmbaSvcWrap_MisraMemcpy(&(pNandTable->GainTblB[0][0]),  &(pData->VigGainTbl_B[0][0]),  (UINT32)(sizeof(UINT32)) * 4U * 128U);
                                AmbaSvcWrap_MisraMemcpy(&(pNandTable->GainTblGb[0][0]), &(pData->VigGainTbl_Gb[0][0]), (UINT32)(sizeof(UINT32)) * 4U * 128U);

                                pNandHeader->Enable = 1U;
                                pNandHeader->DebugMsgOn = SVC_CalibVigCtrl.EnableDebugMsg;
                                if ((pNandHeader->TableSelectBits & SvcCalib_BitGet(CurTblIdx)) == 0U) {
                                    pNandHeader->TableSelectBits |= SvcCalib_BitGet(CurTblIdx);
                                    pNandHeader->NumOfTable ++;
                                }

                                PRN_CAL_VIG_LOG " " PRN_CAL_VIG_API
                                PRN_CAL_VIG_LOG "====== Calibration vig %02d info ====== 0x%x"
                                    PRN_CAL_VIG_ARG_UINT32 ChanIdx        PRN_CAL_VIG_ARG_POST
                                    PRN_CAL_VIG_ARG_UINT32 pData->Version PRN_CAL_VIG_ARG_POST
                                PRN_CAL_VIG_API
                                PRN_CAL_VIG_LOG "  Version                           : 0x%08x"
                                    PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.Version PRN_CAL_VIG_ARG_POST
                                PRN_CAL_VIG_API
                                PRN_CAL_VIG_LOG "  AlgoVersion                       : %d"
                                    PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.AlgoVersion PRN_CAL_VIG_ARG_POST
                                PRN_CAL_VIG_API
                                PRN_CAL_VIG_LOG "  VinSelectBits                     : 0x%x"
                                    PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.VinSelectBits PRN_CAL_VIG_ARG_POST
                                PRN_CAL_VIG_API
                                PRN_CAL_VIG_LOG "  SensorSelectBits                  : 0x%x"
                                    PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.SensorSelectBits PRN_CAL_VIG_ARG_POST
                                PRN_CAL_VIG_API
                                PRN_CAL_VIG_LOG "  VinSensorGeo.StartX               : %d"
                                    PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.VinSensorGeo.StartX PRN_CAL_VIG_ARG_POST
                                PRN_CAL_VIG_API
                                PRN_CAL_VIG_LOG "  VinSensorGeo.StartY               : %d"
                                    PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.VinSensorGeo.StartY PRN_CAL_VIG_ARG_POST
                                PRN_CAL_VIG_API
                                PRN_CAL_VIG_LOG "  VinSensorGeo.Width                : %d"
                                    PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.VinSensorGeo.Width PRN_CAL_VIG_ARG_POST
                                PRN_CAL_VIG_API
                                PRN_CAL_VIG_LOG "  VinSensorGeo.Height               : %d"
                                    PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.VinSensorGeo.Height PRN_CAL_VIG_ARG_POST
                                PRN_CAL_VIG_API
                                PRN_CAL_VIG_LOG "  VinSensorGeo.HSubSample.FactorDen : %d"
                                    PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.VinSensorGeo.HSubSample.FactorDen PRN_CAL_VIG_ARG_POST
                                PRN_CAL_VIG_API
                                PRN_CAL_VIG_LOG "  VinSensorGeo.HSubSample.FactorNum : %d"
                                    PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.VinSensorGeo.HSubSample.FactorNum PRN_CAL_VIG_ARG_POST
                                PRN_CAL_VIG_API
                                PRN_CAL_VIG_LOG "  VinSensorGeo.VSubSample.FactorDen : %d"
                                    PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.VinSensorGeo.VSubSample.FactorDen PRN_CAL_VIG_ARG_POST
                                PRN_CAL_VIG_API
                                PRN_CAL_VIG_LOG "  VinSensorGeo.VSubSample.FactorNum : %d"
                                    PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.VinSensorGeo.VSubSample.FactorNum PRN_CAL_VIG_ARG_POST
                                PRN_CAL_VIG_API
                                PRN_CAL_VIG_LOG "  RadialCoarse                      : %d"
                                    PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.RadialCoarse PRN_CAL_VIG_ARG_POST
                                PRN_CAL_VIG_API
                                PRN_CAL_VIG_LOG "  RadialCoarseLog                   : %d"
                                    PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.RadialCoarseLog PRN_CAL_VIG_ARG_POST
                                PRN_CAL_VIG_API
                                PRN_CAL_VIG_LOG "  RadialBinsFine                    : %d"
                                    PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.RadialBinsFine PRN_CAL_VIG_ARG_POST
                                PRN_CAL_VIG_API
                                PRN_CAL_VIG_LOG "  RadialBinsFineLog                 : %d"
                                    PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.RadialBinsFineLog PRN_CAL_VIG_ARG_POST
                                PRN_CAL_VIG_API
                                PRN_CAL_VIG_LOG "  ModelCenterX_R  ModelCenterY_R   : %d, %d"
                                    PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.ModelCenterX_R PRN_CAL_VIG_ARG_POST
                                    PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.ModelCenterY_R PRN_CAL_VIG_ARG_POST
                                PRN_CAL_VIG_API
                                PRN_CAL_VIG_LOG "  ModelCenterX_Gr ModelCenterY_Gr  : %d, %d"
                                    PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.ModelCenterX_Gr PRN_CAL_VIG_ARG_POST
                                    PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.ModelCenterY_Gr PRN_CAL_VIG_ARG_POST
                                PRN_CAL_VIG_API
                                PRN_CAL_VIG_LOG "  ModelCenterX_B  ModelCenterY_B   : %d, %d"
                                    PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.ModelCenterX_B PRN_CAL_VIG_ARG_POST
                                    PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.ModelCenterY_B PRN_CAL_VIG_ARG_POST
                                PRN_CAL_VIG_API
                                PRN_CAL_VIG_LOG "  ModelCenterX_Gb ModelCenterY_Gb  : %d, %d"
                                    PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.ModelCenterX_Gb PRN_CAL_VIG_ARG_POST
                                    PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.ModelCenterY_Gb PRN_CAL_VIG_ARG_POST
                                PRN_CAL_VIG_API
                                PRN_CAL_VIG_LOG "  SyncCalInfo[0]                    : %d"
                                    PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.SyncCalInfo[0] PRN_CAL_VIG_ARG_POST
                                PRN_CAL_VIG_API
                                PRN_CAL_VIG_LOG "  SyncCalInfo[1]                    : %d"
                                    PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.SyncCalInfo[1] PRN_CAL_VIG_ARG_POST
                                PRN_CAL_VIG_API
                                PRN_CAL_VIG_LOG "  SyncCalInfo[2]                    : %d"
                                    PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.SyncCalInfo[2] PRN_CAL_VIG_ARG_POST
                                PRN_CAL_VIG_API
                                PRN_CAL_VIG_LOG "  SyncCalInfo[3]                    : %d"
                                    PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.SyncCalInfo[3] PRN_CAL_VIG_ARG_POST
                                PRN_CAL_VIG_API
                                PRN_CAL_VIG_LOG "  GainTblR                          : %p"
                                    PRN_CAL_VIG_ARG_CPOINT &(pNandTable->GainTblR[0][0]) PRN_CAL_VIG_ARG_POST
                                PRN_CAL_VIG_API
                                PRN_CAL_VIG_LOG "  GainTblGr                         : %p"
                                    PRN_CAL_VIG_ARG_CPOINT &(pNandTable->GainTblGr[0][0]) PRN_CAL_VIG_ARG_POST
                                PRN_CAL_VIG_API
                                PRN_CAL_VIG_LOG "  GainTblB                          : %p"
                                    PRN_CAL_VIG_ARG_CPOINT &(pNandTable->GainTblB[0][0]) PRN_CAL_VIG_ARG_POST
                                PRN_CAL_VIG_API
                                PRN_CAL_VIG_LOG "  GainTblGb                         : %p"
                                    PRN_CAL_VIG_ARG_CPOINT &(pNandTable->GainTblGb[0][0]) PRN_CAL_VIG_ARG_POST
                                PRN_CAL_VIG_API
                            }
                        }

                        if (RetVal == SVC_OK) {

                            PRN_CAL_VIG_LOG "Successful to calc vig table" PRN_CAL_VIG_OK

                            /* Update to NAND */
                            RetVal = SvcCalib_DataSave(SVC_CALIB_VIGNETTE_ID);
                            if (RetVal != OK) {
                                PRN_CAL_VIG_LOG "Fail to save vig to nand!" PRN_CAL_VIG_NG
                            }
                        }
                    }
                }
            }

            SvcCalib_VigMutexGive(&(SVC_CalibVigCtrl.Mutex));
        }

        SvcCalib_VigMemUnLock();
    }

    return RetVal;
}


static UINT32 SvcCalib_VigTableSearch(UINT32 VinID, UINT32 SensorID, UINT32 *pTblIdx)
{
    UINT32 RetVal = SVC_OK;

    if (SVC_OK != SvcCalib_VigVinSensorIDCheck(VinID, SensorID)) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to search vig table - invalid calib channel -> VinID(%d), SensorID(%d)!"
            PRN_CAL_VIG_ARG_UINT32 VinID    PRN_CAL_VIG_ARG_POST
            PRN_CAL_VIG_ARG_UINT32 SensorID PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_NG
    } else {
        UINT32 Idx;

        if (pTblIdx != NULL) {
            *pTblIdx = 0xFFFFFFFFU;
        }

        for (Idx = 0U; Idx < SVC_CalibVigCtrl.NumOfTable; Idx ++) {
            if (((SVC_CalibVigCtrl.pVigTable[Idx].VinSelectBits & SvcCalib_BitGet(VinID)) > 0U) &&
                ((SVC_CalibVigCtrl.pVigTable[Idx].SensorSelectBits & SensorID) > 0U)) {
                if (pTblIdx != NULL) {
                    *pTblIdx = Idx;
                }
                break;
            }
        }
    }

    return RetVal;
}


static UINT32 SvcCalib_VigNandTableSearch(UINT32 VinID, UINT32 SensorID, UINT32 *pTblIdx)
{
    UINT32 RetVal = SVC_OK;

    if (SVC_OK != SvcCalib_VigVinSensorIDCheck(VinID, SensorID)) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to search vig nand table - invalid calib channel -> VinID(%d), SensorID(%d)!"
            PRN_CAL_VIG_ARG_UINT32 VinID    PRN_CAL_VIG_ARG_POST
            PRN_CAL_VIG_ARG_UINT32 SensorID PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_NG
    } else {
        UINT32 Idx;

        if (pTblIdx != NULL) {
            *pTblIdx = 0xFFFFFFFFU;
        }

        for (Idx = 0U; Idx < SVC_CalibVigCtrl.NumOfTable; Idx ++) {
            if (((SVC_CalibVigCtrl.pNandTable[Idx].Header.VinSelectBits & SvcCalib_BitGet(VinID)) > 0U) &&
                ((SVC_CalibVigCtrl.pNandTable[Idx].Header.SensorSelectBits & SensorID) > 0U) &&
                ((SVC_CalibVigCtrl.pNandHeader->TableSelectBits & SvcCalib_BitGet(Idx)) > 0U)) {
                if (pTblIdx != NULL) {
                    *pTblIdx = Idx;
                }
                break;
            }
        }
    }

    return RetVal;
}


static UINT32 SvcCalib_VigTableSyncMemCfg(UINT8 *pMemBuf, UINT32 MemBufSize,
                                          UINT32 *pNumOfTbl, UINT32 **pTblIDArr,
                                          AMBA_CAL_1D_VIG_CALIB_DATA_V1_s **pSrcTblArr,
                                          AMBA_CAL_1D_VIG_CALIB_DATA_V1_s **pDstTblArr)
{
    UINT32 RetVal = SVC_OK;

    if (pMemBuf == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to config sync table memory - invalid memory buffer base!" PRN_CAL_VIG_NG
    } else if (MemBufSize == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to config sync table memory - invalid memory buffer size!" PRN_CAL_VIG_NG
    } else if (pNumOfTbl == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to config sync table memory - output sync table number!" PRN_CAL_VIG_NG
    } else if (pTblIDArr == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to config sync table memory - output sync table array!" PRN_CAL_VIG_NG
    } else if (pSrcTblArr == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to config sync table memory - output sync table source table array!" PRN_CAL_VIG_NG
    } else if (pDstTblArr == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to config sync table memory - output sync table destination table array!" PRN_CAL_VIG_NG
    } else if (SVC_CalibVigCtrl.NumOfTable >= SVC_CALIB_MAX_TABLE_NUM) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to config sync table memory - invalid Nvm table number" PRN_CAL_VIG_NG
    } else {
        UINT32 Idx, TblMask, TblSelectBits;
        UINT32 NumOfSyncTbl = 0U;

        TblMask = SvcCalib_BitGet(SVC_CalibVigCtrl.NumOfTable) - 1U;
        TblSelectBits = SVC_CalibVigCtrl.CalcCtrl.SyncTbl.TblSelectBits & TblMask;

        for (Idx = 0U; Idx < SVC_CalibVigCtrl.NumOfTable; Idx ++) {
            if ((TblSelectBits & SvcCalib_BitGet(Idx)) > 0U) {
                NumOfSyncTbl += 1U;
            }
        }

        if (NumOfSyncTbl > 0U) {
            UINT8 *pTblIDArrBuf = NULL;
            UINT32 TblIDArrSize;
            UINT8 *pSrcTblBuf = NULL;
            UINT32 SrcTblBufSize;
            UINT8 *DstTblBuf = NULL;
            UINT32 DstTblBufSize;

            TblIDArrSize  = (UINT32)(sizeof(UINT32)) * NumOfSyncTbl;
            SrcTblBufSize = (UINT32)(sizeof(AMBA_CAL_1D_VIG_CALIB_DATA_V1_s)) * NumOfSyncTbl;
            DstTblBufSize = (UINT32)(sizeof(AMBA_CAL_1D_VIG_CALIB_DATA_V1_s)) * NumOfSyncTbl;

            if (MemBufSize < (TblIDArrSize + SrcTblBufSize + DstTblBufSize)) {
                RetVal = SVC_NG;
                PRN_CAL_VIG_LOG "Fail to sync vig table - vig calc memory is too small to handler sync table! 0x%X, 0x%X + 0x%X + 0x%X"
                    PRN_CAL_VIG_ARG_UINT32 MemBufSize    PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_ARG_UINT32 TblIDArrSize  PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_ARG_UINT32 SrcTblBufSize PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_ARG_UINT32 DstTblBufSize PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_NG

                AmbaMisra_TouchUnused(pTblIDArrBuf);
                AmbaMisra_TouchUnused(pSrcTblBuf);
                AmbaMisra_TouchUnused(DstTblBuf);

            } else {
                pTblIDArrBuf  = pMemBuf;
                pSrcTblBuf    = &(pTblIDArrBuf[TblIDArrSize]);
                DstTblBuf     = &(pSrcTblBuf[SrcTblBufSize]);

                *pNumOfTbl = NumOfSyncTbl;
                AmbaMisra_TypeCast(&(*pTblIDArr), &(pTblIDArrBuf));
                AmbaMisra_TypeCast(&(*pSrcTblArr),&(pSrcTblBuf));
                AmbaMisra_TypeCast(&(*pDstTblArr),&(DstTblBuf));

                AmbaSvcWrap_MisraMemset(*pTblIDArr,  0, TblIDArrSize);
                AmbaSvcWrap_MisraMemset(*pSrcTblArr, 0, SrcTblBufSize);
                AmbaSvcWrap_MisraMemset(*pDstTblArr, 0, DstTblBufSize);
            }
        }
    }

    return RetVal;
}

static UINT32 SvcCalib_VigTableSync(void)
{
    UINT32 RetVal = SVC_OK;
    UINT8 *pCalcMem = NULL;
    UINT32 CalcMemSize = 0U;

    if ((SVC_CalibVigCtrlFlag & SVC_CALIB_VIG_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to sync vig table - Need to initial vig control module first!" PRN_CAL_VIG_NG
    } else if ((SVC_CalibVigCtrlFlag & SVC_CALIB_VIG_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to sync vig table - Need to initial vig dram shadow first!" PRN_CAL_VIG_NG
    } else if (SVC_CalibVigCtrl.pNandHeader == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to sync vig table - invalid nand header!" PRN_CAL_VIG_NG
    } else if (SVC_CalibVigCtrl.pNandTable == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to sync vig table - invalid nand table!" PRN_CAL_VIG_NG
    } else if (SVC_CalibVigCtrl.CalcCtrl.SyncTbl.TblSelectBits == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to sync vig table - invalid sync table setting!" PRN_CAL_VIG_NG
    } else if (SVC_CalibVigCtrl.CalcCtrl.SyncTbl.DiffThreshold == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to sync vig table - invalid threshold setting!" PRN_CAL_VIG_NG
    } else if (SVC_CalibVigCtrl.NumOfTable >= 32U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to sync vig table - NVM table number is out-of range!" PRN_CAL_VIG_NG
    } else {
        UINT32 Idx, TblMask, TblSelectBits;
        UINT32 NumOfSyncTbl = 0U;
        UINT32 *pTblIDArr = NULL;
        AMBA_CAL_1D_VIG_CALIB_DATA_V1_s *pSrc = NULL;
        AMBA_CAL_1D_VIG_CALIB_DATA_V1_s *pDst = NULL;

        TblMask = SvcCalib_BitGet(SVC_CalibVigCtrl.NumOfTable) - 1U;
        TblSelectBits = SVC_CalibVigCtrl.CalcCtrl.SyncTbl.TblSelectBits & TblMask;

        PRN_CAL_VIG_LOG "Vignette Sync Table 0x%X"
            PRN_CAL_VIG_ARG_UINT32 TblSelectBits PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_API

        RetVal = SvcCalib_ItemCalcMemGet(SVC_CALIB_VIGNETTE_ID, 1U, &pCalcMem, &CalcMemSize);
        if (RetVal != SVC_OK) {
            PRN_CAL_VIG_LOG "Fail to sync vig table - get the calculation memory fail!" PRN_CAL_VIG_NG
        } else {
            PRN_CAL_VIG_LOG "Successful to lock vig calc memory before sync table process!" PRN_CAL_VIG_OK

            RetVal = SvcCalib_VigTableSyncMemCfg(pCalcMem, CalcMemSize,
                                                 &NumOfSyncTbl,
                                                 &pTblIDArr,
                                                 &pSrc, &pDst);

            if ((RetVal == SVC_OK) && (pTblIDArr != NULL) && (pSrc != NULL) && (pDst != NULL)) {
                UINT32 NumTbl = 0U, CalVal;

                for (Idx = 0U; Idx < SVC_CalibVigCtrl.NumOfTable; Idx ++) {
                    if ((TblSelectBits & SvcCalib_BitGet(Idx)) > 0U) {
                        pTblIDArr[NumTbl] = Idx;

                        AmbaSvcWrap_MisraMemset(&(pSrc[NumTbl]), 0, sizeof(AMBA_CAL_1D_VIG_CALIB_DATA_V1_s));
                        pSrc[NumTbl].Version                             = SVC_CalibVigCtrl.pNandTable[Idx].Header.AlgoVersion;
                        pSrc[NumTbl].CalibSensorGeo.StartX               = SVC_CalibVigCtrl.pNandTable[Idx].Header.VinSensorGeo.StartX               ;
                        pSrc[NumTbl].CalibSensorGeo.StartY               = SVC_CalibVigCtrl.pNandTable[Idx].Header.VinSensorGeo.StartY               ;
                        pSrc[NumTbl].CalibSensorGeo.Width                = SVC_CalibVigCtrl.pNandTable[Idx].Header.VinSensorGeo.Width                ;
                        pSrc[NumTbl].CalibSensorGeo.Height               = SVC_CalibVigCtrl.pNandTable[Idx].Header.VinSensorGeo.Height               ;
                        pSrc[NumTbl].CalibSensorGeo.HSubSample.FactorDen = SVC_CalibVigCtrl.pNandTable[Idx].Header.VinSensorGeo.HSubSample.FactorDen ;
                        pSrc[NumTbl].CalibSensorGeo.HSubSample.FactorNum = SVC_CalibVigCtrl.pNandTable[Idx].Header.VinSensorGeo.HSubSample.FactorNum ;
                        pSrc[NumTbl].CalibSensorGeo.VSubSample.FactorDen = SVC_CalibVigCtrl.pNandTable[Idx].Header.VinSensorGeo.VSubSample.FactorDen ;
                        pSrc[NumTbl].CalibSensorGeo.VSubSample.FactorNum = SVC_CalibVigCtrl.pNandTable[Idx].Header.VinSensorGeo.VSubSample.FactorNum ;
                        pSrc[NumTbl].RadialCoarse                        = SVC_CalibVigCtrl.pNandTable[Idx].Header.RadialCoarse      ;
                        pSrc[NumTbl].RadialCoarseLog                     = SVC_CalibVigCtrl.pNandTable[Idx].Header.RadialCoarseLog   ;
                        pSrc[NumTbl].RadialBinsFine                      = SVC_CalibVigCtrl.pNandTable[Idx].Header.RadialBinsFine    ;
                        pSrc[NumTbl].RadialBinsFineLog                   = SVC_CalibVigCtrl.pNandTable[Idx].Header.RadialBinsFineLog ;
                        pSrc[NumTbl].ModelCenterX_R                      = SVC_CalibVigCtrl.pNandTable[Idx].Header.ModelCenterX_R    ;
                        pSrc[NumTbl].ModelCenterX_Gr                     = SVC_CalibVigCtrl.pNandTable[Idx].Header.ModelCenterX_Gr   ;
                        pSrc[NumTbl].ModelCenterX_B                      = SVC_CalibVigCtrl.pNandTable[Idx].Header.ModelCenterX_B    ;
                        pSrc[NumTbl].ModelCenterX_Gb                     = SVC_CalibVigCtrl.pNandTable[Idx].Header.ModelCenterX_Gb   ;
                        pSrc[NumTbl].ModelCenterY_R                      = SVC_CalibVigCtrl.pNandTable[Idx].Header.ModelCenterY_R    ;
                        pSrc[NumTbl].ModelCenterY_Gr                     = SVC_CalibVigCtrl.pNandTable[Idx].Header.ModelCenterY_Gr   ;
                        pSrc[NumTbl].ModelCenterY_B                      = SVC_CalibVigCtrl.pNandTable[Idx].Header.ModelCenterY_B    ;
                        pSrc[NumTbl].ModelCenterY_Gb                     = SVC_CalibVigCtrl.pNandTable[Idx].Header.ModelCenterY_Gb   ;
                        pSrc[NumTbl].SyncCalInfo[0]                      = SVC_CalibVigCtrl.pNandTable[Idx].Header.SyncCalInfo[0] ;
                        pSrc[NumTbl].SyncCalInfo[1]                      = SVC_CalibVigCtrl.pNandTable[Idx].Header.SyncCalInfo[1] ;
                        pSrc[NumTbl].SyncCalInfo[2]                      = SVC_CalibVigCtrl.pNandTable[Idx].Header.SyncCalInfo[2] ;
                        pSrc[NumTbl].SyncCalInfo[3]                      = SVC_CalibVigCtrl.pNandTable[Idx].Header.SyncCalInfo[3] ;
                        AmbaSvcWrap_MisraMemcpy(&(pSrc[NumTbl].VigGainTbl_R[0][0]),  &(SVC_CalibVigCtrl.pNandTable[Idx].GainTblR[0][0]),  (UINT32)(sizeof(UINT32)) * 4U * 128U);
                        AmbaSvcWrap_MisraMemcpy(&(pSrc[NumTbl].VigGainTbl_Gr[0][0]), &(SVC_CalibVigCtrl.pNandTable[Idx].GainTblGr[0][0]), (UINT32)(sizeof(UINT32)) * 4U * 128U);
                        AmbaSvcWrap_MisraMemcpy(&(pSrc[NumTbl].VigGainTbl_B[0][0]),  &(SVC_CalibVigCtrl.pNandTable[Idx].GainTblB[0][0]),  (UINT32)(sizeof(UINT32)) * 4U * 128U);
                        AmbaSvcWrap_MisraMemcpy(&(pSrc[NumTbl].VigGainTbl_Gb[0][0]), &(SVC_CalibVigCtrl.pNandTable[Idx].GainTblGb[0][0]), (UINT32)(sizeof(UINT32)) * 4U * 128U);

                        NumTbl += 1U;
                    }
                }

                CalVal = AmbaCal_1DVigSyncTblV1( NumTbl, SVC_CalibVigCtrl.CalcCtrl.SyncTbl.DiffThreshold, pSrc, pDst );
                if (CalVal != CAL_OK) {
                    RetVal = SVC_NG;
                    PRN_CAL_VIG_LOG "Fail to sync vig table - ErrCode: %d ErrMsg: %s"
                        PRN_CAL_VIG_ARG_UINT32 CalVal                     PRN_CAL_VIG_ARG_POST
                        PRN_CAL_VIG_ARG_CSTR   AmbaCal_ErrNoToMsg(CalVal) PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_NG

                } else {
                    UINT32 TblIdx;
                    for (Idx = 0U; Idx < NumTbl; Idx ++) {
                        TblIdx = pTblIDArr[Idx];
                        SVC_CalibVigCtrl.pNandTable[TblIdx].Header.VinSensorGeo.StartX               = pDst[TblIdx].CalibSensorGeo.StartX               ;
                        SVC_CalibVigCtrl.pNandTable[TblIdx].Header.VinSensorGeo.StartY               = pDst[TblIdx].CalibSensorGeo.StartY               ;
                        SVC_CalibVigCtrl.pNandTable[TblIdx].Header.VinSensorGeo.Width                = pDst[TblIdx].CalibSensorGeo.Width                ;
                        SVC_CalibVigCtrl.pNandTable[TblIdx].Header.VinSensorGeo.Height               = pDst[TblIdx].CalibSensorGeo.Height               ;
                        SVC_CalibVigCtrl.pNandTable[TblIdx].Header.VinSensorGeo.HSubSample.FactorDen = pDst[TblIdx].CalibSensorGeo.HSubSample.FactorDen ;
                        SVC_CalibVigCtrl.pNandTable[TblIdx].Header.VinSensorGeo.HSubSample.FactorNum = pDst[TblIdx].CalibSensorGeo.HSubSample.FactorNum ;
                        SVC_CalibVigCtrl.pNandTable[TblIdx].Header.VinSensorGeo.VSubSample.FactorDen = pDst[TblIdx].CalibSensorGeo.VSubSample.FactorDen ;
                        SVC_CalibVigCtrl.pNandTable[TblIdx].Header.VinSensorGeo.VSubSample.FactorNum = pDst[TblIdx].CalibSensorGeo.VSubSample.FactorNum ;
                        SVC_CalibVigCtrl.pNandTable[TblIdx].Header.RadialCoarse                      = pDst[TblIdx].RadialCoarse                        ;
                        SVC_CalibVigCtrl.pNandTable[TblIdx].Header.RadialCoarseLog                   = pDst[TblIdx].RadialCoarseLog                     ;
                        SVC_CalibVigCtrl.pNandTable[TblIdx].Header.RadialBinsFine                    = pDst[TblIdx].RadialBinsFine                      ;
                        SVC_CalibVigCtrl.pNandTable[TblIdx].Header.RadialBinsFineLog                 = pDst[TblIdx].RadialBinsFineLog                   ;
                        SVC_CalibVigCtrl.pNandTable[TblIdx].Header.ModelCenterX_R                    = pDst[TblIdx].ModelCenterX_R                      ;
                        SVC_CalibVigCtrl.pNandTable[TblIdx].Header.ModelCenterX_Gr                   = pDst[TblIdx].ModelCenterX_Gr                     ;
                        SVC_CalibVigCtrl.pNandTable[TblIdx].Header.ModelCenterX_B                    = pDst[TblIdx].ModelCenterX_B                      ;
                        SVC_CalibVigCtrl.pNandTable[TblIdx].Header.ModelCenterX_Gb                   = pDst[TblIdx].ModelCenterX_Gb                     ;
                        SVC_CalibVigCtrl.pNandTable[TblIdx].Header.ModelCenterY_R                    = pDst[TblIdx].ModelCenterY_R                      ;
                        SVC_CalibVigCtrl.pNandTable[TblIdx].Header.ModelCenterY_Gr                   = pDst[TblIdx].ModelCenterY_Gr                     ;
                        SVC_CalibVigCtrl.pNandTable[TblIdx].Header.ModelCenterY_B                    = pDst[TblIdx].ModelCenterY_B                      ;
                        SVC_CalibVigCtrl.pNandTable[TblIdx].Header.ModelCenterY_Gb                   = pDst[TblIdx].ModelCenterY_Gb                     ;
                        SVC_CalibVigCtrl.pNandTable[TblIdx].Header.SyncCalInfo[0]                    = pDst[TblIdx].SyncCalInfo[0]                      ;
                        SVC_CalibVigCtrl.pNandTable[TblIdx].Header.SyncCalInfo[1]                    = pDst[TblIdx].SyncCalInfo[1]                      ;
                        SVC_CalibVigCtrl.pNandTable[TblIdx].Header.SyncCalInfo[2]                    = pDst[TblIdx].SyncCalInfo[2]                      ;
                        SVC_CalibVigCtrl.pNandTable[TblIdx].Header.SyncCalInfo[3]                    = pDst[TblIdx].SyncCalInfo[3]                      ;
                        AmbaSvcWrap_MisraMemcpy(&(SVC_CalibVigCtrl.pNandTable[TblIdx].GainTblR[0][0]),  &(pDst[TblIdx].VigGainTbl_R[0][0]),  (UINT32)(sizeof(UINT32)) * 4U * 128U);
                        AmbaSvcWrap_MisraMemcpy(&(SVC_CalibVigCtrl.pNandTable[TblIdx].GainTblGr[0][0]), &(pDst[TblIdx].VigGainTbl_Gr[0][0]), (UINT32)(sizeof(UINT32)) * 4U * 128U);
                        AmbaSvcWrap_MisraMemcpy(&(SVC_CalibVigCtrl.pNandTable[TblIdx].GainTblB[0][0]),  &(pDst[TblIdx].VigGainTbl_B[0][0]),  (UINT32)(sizeof(UINT32)) * 4U * 128U);
                        AmbaSvcWrap_MisraMemcpy(&(SVC_CalibVigCtrl.pNandTable[TblIdx].GainTblGb[0][0]), &(pDst[TblIdx].VigGainTbl_Gb[0][0]), (UINT32)(sizeof(UINT32)) * 4U * 128U);
                    }

                    PRN_CAL_VIG_LOG "Successful to sync vig table!" PRN_CAL_VIG_OK

                    /* Update to NAND */
                    RetVal = SvcCalib_DataSave(SVC_CALIB_VIGNETTE_ID);
                    if (RetVal != OK) {
                        PRN_CAL_VIG_LOG "Fail to save vig to nand!" PRN_CAL_VIG_NG
                    }
                }
            }
        }
    }

    if (SVC_OK != SvcCalib_ItemCalcMemFree(SVC_CALIB_VIGNETTE_ID)) {
        PRN_CAL_VIG_LOG "Fail to sync vig table - init calculation memory control fail!" PRN_CAL_VIG_NG
    } else {
        PRN_CAL_VIG_LOG "Successful to un-lock vig calc memory after sync table process!" PRN_CAL_VIG_OK
    }

    return RetVal;
}


static UINT32 SvcCalib_VigTableDataGet(UINT32 TblID, SVC_CALIB_TBL_INFO_s *pTblInfo)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibVigCtrlFlag & SVC_CALIB_VIG_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to get vig table - initial vig module first!" PRN_CAL_VIG_NG
    } else if ((SVC_CalibVigCtrlFlag & SVC_CALIB_VIG_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to get vig table - initial vig dram shadow first!" PRN_CAL_VIG_NG
    } else if (SVC_CalibVigCtrl.pNandTable == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to get vig table - invalid nand table!" PRN_CAL_VIG_NG
    } else if (SVC_CalibVigCtrl.pNandHeader->NumOfTable == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "There is not vig table!" PRN_CAL_VIG_DBG
    } else if (pTblInfo == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to get vig table - output table info should not null!" PRN_CAL_VIG_NG
    } else if (pTblInfo->pBuf == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to get vig table - output table buffer should not null!" PRN_CAL_VIG_NG
    } else if (pTblInfo->BufSize < sizeof(AMBA_CAL_1D_VIG_CALIB_DATA_V1_s)) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to get vig table - output table buffer size is too small! 0x%X/0x%X"
            PRN_CAL_VIG_ARG_UINT32 pTblInfo->BufSize                    PRN_CAL_VIG_ARG_POST
            PRN_CAL_VIG_ARG_UINT32 sizeof(AMBA_CAL_1D_VIG_CALIB_DATA_V1_s) PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_NG
    } else {
        if (KAL_ERR_NONE != SvcCalib_VigMutexTake(&(SVC_CalibVigCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
            RetVal = SVC_NG;
            PRN_CAL_VIG_LOG "Fail to get vig table - take mutex fail!" PRN_CAL_VIG_NG
        } else {
            UINT32 CurTblID = 0xFFFFFFFFU;
            AMBA_CAL_1D_VIG_CALIB_DATA_V1_s  *pOutput = NULL;
            const SVC_CALIB_VIG_NAND_TABLE_s *pNandTbl;
            if (TblID == 255U) {
                if (0U != SvcCalib_VigNandTableSearch(pTblInfo->CalChan.VinID, pTblInfo->CalChan.SensorID, &CurTblID)) {
                    CurTblID = 0xFFFFFFFFU;
                }
            } else {
                CurTblID = TblID;
            }

            if (CurTblID >= SVC_CalibVigCtrl.NumOfTable) {
                RetVal = SVC_NG;

                PRetVal = (SVC_CalibVigCtrl.NumOfTable == 0U)?0U:(SVC_CalibVigCtrl.NumOfTable - 1U);
                PRN_CAL_VIG_LOG "Fail to get vig table - the TableID(0x%X) is out-of range(0 ~ %d)"
                    PRN_CAL_VIG_ARG_UINT32 CurTblID PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_ARG_UINT32 PRetVal  PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_NG
            } else {
                pNandTbl = &(SVC_CalibVigCtrl.pNandTable[CurTblID]);

                AmbaMisra_TypeCast(&(pOutput), &(pTblInfo->pBuf));

                if (pOutput != NULL) {
                    AmbaSvcWrap_MisraMemset(pOutput, 0, sizeof(AMBA_CAL_1D_VIG_CALIB_DATA_V1_s));
                    pOutput->Version                             = pNandTbl->Header.AlgoVersion;
                    pOutput->CalibSensorGeo.StartX               = pNandTbl->Header.VinSensorGeo.StartX               ;
                    pOutput->CalibSensorGeo.StartY               = pNandTbl->Header.VinSensorGeo.StartY               ;
                    pOutput->CalibSensorGeo.Width                = pNandTbl->Header.VinSensorGeo.Width                ;
                    pOutput->CalibSensorGeo.Height               = pNandTbl->Header.VinSensorGeo.Height               ;
                    pOutput->CalibSensorGeo.HSubSample.FactorDen = pNandTbl->Header.VinSensorGeo.HSubSample.FactorDen ;
                    pOutput->CalibSensorGeo.HSubSample.FactorNum = pNandTbl->Header.VinSensorGeo.HSubSample.FactorNum ;
                    pOutput->CalibSensorGeo.VSubSample.FactorDen = pNandTbl->Header.VinSensorGeo.VSubSample.FactorDen ;
                    pOutput->CalibSensorGeo.VSubSample.FactorNum = pNandTbl->Header.VinSensorGeo.VSubSample.FactorNum ;
                    pOutput->RadialCoarse                        = pNandTbl->Header.RadialCoarse      ;
                    pOutput->RadialCoarseLog                     = pNandTbl->Header.RadialCoarseLog   ;
                    pOutput->RadialBinsFine                      = pNandTbl->Header.RadialBinsFine    ;
                    pOutput->RadialBinsFineLog                   = pNandTbl->Header.RadialBinsFineLog ;
                    pOutput->ModelCenterX_R                      = pNandTbl->Header.ModelCenterX_R    ;
                    pOutput->ModelCenterX_Gr                     = pNandTbl->Header.ModelCenterX_Gr   ;
                    pOutput->ModelCenterX_B                      = pNandTbl->Header.ModelCenterX_B    ;
                    pOutput->ModelCenterX_Gb                     = pNandTbl->Header.ModelCenterX_Gb   ;
                    pOutput->ModelCenterY_R                      = pNandTbl->Header.ModelCenterY_R    ;
                    pOutput->ModelCenterY_Gr                     = pNandTbl->Header.ModelCenterY_Gr   ;
                    pOutput->ModelCenterY_B                      = pNandTbl->Header.ModelCenterY_B    ;
                    pOutput->ModelCenterY_Gb                     = pNandTbl->Header.ModelCenterY_Gb   ;
                    pOutput->SyncCalInfo[0]                      = pNandTbl->Header.SyncCalInfo[0] ;
                    pOutput->SyncCalInfo[1]                      = pNandTbl->Header.SyncCalInfo[1] ;
                    pOutput->SyncCalInfo[2]                      = pNandTbl->Header.SyncCalInfo[2] ;
                    pOutput->SyncCalInfo[3]                      = pNandTbl->Header.SyncCalInfo[3] ;
                    AmbaSvcWrap_MisraMemcpy(&(pOutput->VigGainTbl_R[0][0]),  &(pNandTbl->GainTblR[0][0]),  (UINT32)(sizeof(UINT32)) * 4U * 128U);
                    AmbaSvcWrap_MisraMemcpy(&(pOutput->VigGainTbl_Gr[0][0]), &(pNandTbl->GainTblGr[0][0]), (UINT32)(sizeof(UINT32)) * 4U * 128U);
                    AmbaSvcWrap_MisraMemcpy(&(pOutput->VigGainTbl_B[0][0]),  &(pNandTbl->GainTblB[0][0]),  (UINT32)(sizeof(UINT32)) * 4U * 128U);
                    AmbaSvcWrap_MisraMemcpy(&(pOutput->VigGainTbl_Gb[0][0]), &(pNandTbl->GainTblGb[0][0]), (UINT32)(sizeof(UINT32)) * 4U * 128U);

                    pTblInfo->CalChan.VinSelectBits    = pNandTbl->Header.VinSelectBits;
                    pTblInfo->CalChan.SensorSelectBits = pNandTbl->Header.SensorSelectBits;

                    PRN_CAL_VIG_LOG "Success to get vig table from TableID(%d), VinSelectBits(0x%X) SensorSelectBits(0x%X)"
                        PRN_CAL_VIG_ARG_UINT32 CurTblID                           PRN_CAL_VIG_ARG_POST
                        PRN_CAL_VIG_ARG_UINT32 pTblInfo->CalChan.VinSelectBits    PRN_CAL_VIG_ARG_POST
                        PRN_CAL_VIG_ARG_UINT32 pTblInfo->CalChan.SensorSelectBits PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_OK

                }
            }
            SvcCalib_VigMutexGive(&(SVC_CalibVigCtrl.Mutex));
        }
    }

    return RetVal;
}


static UINT32 SvcCalib_VigTableDataSet(UINT32 TblID, SVC_CALIB_TBL_INFO_s *pTblInfo)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibVigCtrlFlag & SVC_CALIB_VIG_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to set vig table - initial vig module first!" PRN_CAL_VIG_NG
    } else if ((SVC_CalibVigCtrlFlag & SVC_CALIB_VIG_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to set vig table - initial vig dram shadow first!" PRN_CAL_VIG_NG
    } else if (SVC_CalibVigCtrl.pNandHeader == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to set vig table - invalid nand header!" PRN_CAL_VIG_NG
    } else if (SVC_CalibVigCtrl.pNandTable == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to set vig table - invalid nand table!" PRN_CAL_VIG_NG
    } else if (TblID >= SVC_CalibVigCtrl.NumOfTable) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to set vig table - TableID(%d) out-of range(%d)!"
            PRN_CAL_VIG_ARG_UINT32 TblID                       PRN_CAL_VIG_ARG_POST
            PRN_CAL_VIG_ARG_UINT32 SVC_CalibVigCtrl.NumOfTable PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_NG
    } else if (pTblInfo == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to set vig table - output table info should not null!" PRN_CAL_VIG_NG
    } else if (pTblInfo->pBuf == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to set vig table - input table buffer should not null!" PRN_CAL_VIG_NG
    } else if (pTblInfo->BufSize < sizeof(AMBA_CAL_1D_VIG_CALIB_DATA_V1_s)) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to set vig table - input table buffer size is too small! 0x%X/0x%X"
            PRN_CAL_VIG_ARG_UINT32 pTblInfo->BufSize                    PRN_CAL_VIG_ARG_POST
            PRN_CAL_VIG_ARG_UINT32 sizeof(AMBA_CAL_1D_VIG_CALIB_DATA_V1_s) PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_NG
    } else {

        if (KAL_ERR_NONE != SvcCalib_VigMutexTake(&(SVC_CalibVigCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
            RetVal = SVC_NG;
            PRN_CAL_VIG_LOG "Fail to set vig table - take mutex fail!" PRN_CAL_VIG_NG
        } else {
            AMBA_CAL_1D_VIG_CALIB_DATA_V1_s *pInput = NULL;
            SVC_CALIB_VIG_NAND_TABLE_s  *pNandTbl    = &(SVC_CalibVigCtrl.pNandTable[TblID]);
            SVC_CALIB_VIG_NAND_HEADER_s *pNandHeader = SVC_CalibVigCtrl.pNandHeader;

            AmbaMisra_TypeCast(&(pInput), &(pTblInfo->pBuf));

            if (pInput == NULL) {
                RetVal = SVC_NG;
                PRN_CAL_VIG_LOG "Fail to set vig table - input vig table data should not null!" PRN_CAL_VIG_NG
            } else {

                AmbaSvcWrap_MisraMemset(pNandTbl, 0, sizeof(SVC_CALIB_VIG_NAND_TABLE_s));
                pNandTbl->Header.VinSelectBits                     = pTblInfo->CalChan.VinSelectBits             ;
                pNandTbl->Header.SensorSelectBits                  = pTblInfo->CalChan.SensorSelectBits          ;
                pNandTbl->Header.Version                           = SVC_CALIB_VIG_VERSION                       ;
                pNandTbl->Header.AlgoVersion                       = pInput->Version                             ;
                pNandTbl->Header.VinSensorGeo.StartX               = pInput->CalibSensorGeo.StartX               ;
                pNandTbl->Header.VinSensorGeo.StartY               = pInput->CalibSensorGeo.StartY               ;
                pNandTbl->Header.VinSensorGeo.Width                = pInput->CalibSensorGeo.Width                ;
                pNandTbl->Header.VinSensorGeo.Height               = pInput->CalibSensorGeo.Height               ;
                pNandTbl->Header.VinSensorGeo.HSubSample.FactorDen = pInput->CalibSensorGeo.HSubSample.FactorDen ;
                pNandTbl->Header.VinSensorGeo.HSubSample.FactorNum = pInput->CalibSensorGeo.HSubSample.FactorNum ;
                pNandTbl->Header.VinSensorGeo.VSubSample.FactorDen = pInput->CalibSensorGeo.VSubSample.FactorDen ;
                pNandTbl->Header.VinSensorGeo.VSubSample.FactorNum = pInput->CalibSensorGeo.VSubSample.FactorNum ;
                pNandTbl->Header.RadialCoarse                      = pInput->RadialCoarse                        ;
                pNandTbl->Header.RadialCoarseLog                   = pInput->RadialCoarseLog                     ;
                pNandTbl->Header.RadialBinsFine                    = pInput->RadialBinsFine                      ;
                pNandTbl->Header.RadialBinsFineLog                 = pInput->RadialBinsFineLog                   ;
                pNandTbl->Header.ModelCenterX_R                    = pInput->ModelCenterX_R                      ;
                pNandTbl->Header.ModelCenterX_Gr                   = pInput->ModelCenterX_Gr                     ;
                pNandTbl->Header.ModelCenterX_B                    = pInput->ModelCenterX_B                      ;
                pNandTbl->Header.ModelCenterX_Gb                   = pInput->ModelCenterX_Gb                     ;
                pNandTbl->Header.ModelCenterY_R                    = pInput->ModelCenterY_R                      ;
                pNandTbl->Header.ModelCenterY_Gr                   = pInput->ModelCenterY_Gr                     ;
                pNandTbl->Header.ModelCenterY_B                    = pInput->ModelCenterY_B                      ;
                pNandTbl->Header.ModelCenterY_Gb                   = pInput->ModelCenterY_Gb                     ;
                pNandTbl->Header.SyncCalInfo[0]                    = pInput->SyncCalInfo[0]                      ;
                pNandTbl->Header.SyncCalInfo[1]                    = pInput->SyncCalInfo[1]                      ;
                pNandTbl->Header.SyncCalInfo[2]                    = pInput->SyncCalInfo[2]                      ;
                pNandTbl->Header.SyncCalInfo[3]                    = pInput->SyncCalInfo[3]                      ;
                AmbaSvcWrap_MisraMemcpy(&(pNandTbl->GainTblR[0][0]),  &(pInput->VigGainTbl_R[0][0]),  (UINT32)(sizeof(UINT32)) * 4U * 128U);
                AmbaSvcWrap_MisraMemcpy(&(pNandTbl->GainTblGr[0][0]), &(pInput->VigGainTbl_Gr[0][0]), (UINT32)(sizeof(UINT32)) * 4U * 128U);
                AmbaSvcWrap_MisraMemcpy(&(pNandTbl->GainTblB[0][0]),  &(pInput->VigGainTbl_B[0][0]),  (UINT32)(sizeof(UINT32)) * 4U * 128U);
                AmbaSvcWrap_MisraMemcpy(&(pNandTbl->GainTblGb[0][0]), &(pInput->VigGainTbl_Gb[0][0]), (UINT32)(sizeof(UINT32)) * 4U * 128U);

                // Update nand header
                if (pNandHeader->Enable == 0U) {
                    pNandHeader->Enable = 1U;
                }
                if (pNandHeader->DebugMsgOn == 0U) {
                    pNandHeader->DebugMsgOn = SVC_CalibVigCtrl.EnableDebugMsg;
                }
                if (pNandHeader->DebugMsgOn == 0U) {
                    pNandHeader->DebugMsgOn = pTblInfo->DbgMsgOn;
                }
                if (SVC_CalibVigCtrl.EnableDebugMsg == 0U) {
                    SVC_CalibVigCtrl.EnableDebugMsg = pTblInfo->DbgMsgOn;
                }
                if ((pNandHeader->TableSelectBits & SvcCalib_BitGet(TblID)) == 0U) {
                    pNandHeader->TableSelectBits |= SvcCalib_BitGet(TblID);
                    pNandHeader->NumOfTable += 1U;
                }

                /* Update to NAND */
                RetVal = SvcCalib_DataSave(SVC_CALIB_VIGNETTE_ID);
                if (RetVal != OK) {
                    PRN_CAL_VIG_LOG "Fail to save vig to nand!" PRN_CAL_VIG_NG
                }

                AmbaMisra_TouchUnused(pInput);
                AmbaMisra_TouchUnused(pTblInfo);

            }

            SvcCalib_VigMutexGive(&(SVC_CalibVigCtrl.Mutex));
        }
    }

    return RetVal;
}


static UINT32 SvcCalib_VigMemQuery(UINT32 MaxTableNum, UINT32 *pShadowSize, UINT32 *pWorkMemSize, UINT32 *pCalcMemSize)
{
    UINT32 RetVal = SVC_OK;

    UINT32 DramShadowSize = 0, WorkingMemSize = 0, CalcMemSize = 0;
    UINT32 DramShadowHeaderSize = (UINT32)(sizeof(SVC_CALIB_VIG_NAND_HEADER_s));
    UINT32 DramShadowTableSize  = (UINT32)(sizeof(SVC_CALIB_VIG_NAND_TABLE_s));
    UINT32 DramShadowDataSize   = DramShadowTableSize * MaxTableNum;
    UINT32 VigTableSize         = (UINT32)(sizeof(SVC_CALIB_VIG_TABLE_s));
    UINT32 TotalVigTableSize    = VigTableSize * MaxTableNum;
    SIZE_t CalcMemAlgoSize      = 0;
    SIZE_t CalcMemCtnSize       = 0;
    AMBA_CT_CONFIG_s CtnCfg;

    AmbaSvcWrap_MisraMemset(&CtnCfg, 0, sizeof(CtnCfg));
    if ((SVC_CalibVigCtrlFlag & SVC_CALIB_VIG_FLG_CALC_CTN_MAX_WIN) > 0U) {
        CtnCfg.Type.Vig1d.MaxImageSize.Width  = SVC_CalibVigCtrl.CalcCtrl.MaxRawWidth;
        CtnCfg.Type.Vig1d.MaxImageSize.Height = SVC_CalibVigCtrl.CalcCtrl.MaxRawHeight;
        CtnCfg.Type.Vig1d.Resolution          = SVC_CalibVigCtrl.CalcCtrl.MaxResolution;
    } else {
        CtnCfg.Type.Vig1d.MaxImageSize.Width  = SVC_CALIB_VIG_MAX_RAW_WIDTH;
        CtnCfg.Type.Vig1d.MaxImageSize.Height = SVC_CALIB_VIG_MAX_RAW_HEIGHT;
        CtnCfg.Type.Vig1d.Resolution          = SVC_CALIB_VIG_MAX_RESOLUTION;
    }

    if (CAL_OK != AmbaCal_1DVigGetWorkingBufSizeV1(&(CtnCfg.Type.Vig1d.MaxImageSize), CtnCfg.Type.Vig1d.Resolution, &CalcMemAlgoSize)) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to query vig module memory - get vig algo working memory fail!" PRN_CAL_VIG_NG
    } else if (0U != AmbaCT_Cfg(AMBA_CT_TYPE_1D_VIG, &CtnCfg)) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to query vig module memory - config ctuner module fail!" PRN_CAL_VIG_NG
    } else if (0U != AmbaCT_GetBufSize(AMBA_CT_TYPE_1D_VIG, &CalcMemCtnSize)) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to query vig module memory - get vig ctuner working memory size fail!" PRN_CAL_VIG_NG
    } else {
        DramShadowSize  = DramShadowHeaderSize;
        DramShadowSize += DramShadowDataSize;

        WorkingMemSize  = TotalVigTableSize;

        CalcMemSize    += (UINT32)CalcMemAlgoSize;
        CalcMemSize    += (UINT32)CalcMemCtnSize;

        PRN_CAL_VIG_LOG " " PRN_CAL_VIG_DBG
        PRN_CAL_VIG_LOG "====== Calib VIG Memory Query ======" PRN_CAL_VIG_DBG
        PRN_CAL_VIG_LOG "  MaxTableNum    : %d"
            PRN_CAL_VIG_ARG_UINT32 MaxTableNum PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_DBG
        PRN_CAL_VIG_LOG "  DramShadowSize : 0x%X"
            PRN_CAL_VIG_ARG_UINT32 DramShadowSize PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_DBG
        PRN_CAL_VIG_LOG "    DramShadowHeaderSize : 0x%X"
            PRN_CAL_VIG_ARG_UINT32 DramShadowHeaderSize PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_DBG
        PRN_CAL_VIG_LOG "    DramShadowDataSize   : 0x%X ( 0x%X x %d )"
            PRN_CAL_VIG_ARG_UINT32 DramShadowDataSize  PRN_CAL_VIG_ARG_POST
            PRN_CAL_VIG_ARG_UINT32 DramShadowTableSize PRN_CAL_VIG_ARG_POST
            PRN_CAL_VIG_ARG_UINT32 MaxTableNum         PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_DBG
        PRN_CAL_VIG_LOG "  WorkingMemSize : 0x%X"
            PRN_CAL_VIG_ARG_UINT32 WorkingMemSize PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_DBG
        PRN_CAL_VIG_LOG "    TotalVigTableSize : 0x%X ( 0x%X x %d )"
            PRN_CAL_VIG_ARG_UINT32 TotalVigTableSize  PRN_CAL_VIG_ARG_POST
            PRN_CAL_VIG_ARG_UINT32 VigTableSize       PRN_CAL_VIG_ARG_POST
            PRN_CAL_VIG_ARG_UINT32 MaxTableNum        PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_DBG
        PRN_CAL_VIG_LOG "  AlgoMemSize : 0x%X"
            PRN_CAL_VIG_ARG_UINT32 CalcMemSize PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_DBG
        PRN_CAL_VIG_LOG "    CalcMemAlgoSize    : 0x%X"
            PRN_CAL_VIG_ARG_UINT32 CalcMemAlgoSize PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_DBG
        PRN_CAL_VIG_LOG "    CalcMemCtnSize     : 0x%X"
            PRN_CAL_VIG_ARG_UINT32 CalcMemCtnSize PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_DBG

        if (pShadowSize != NULL) {
            *pShadowSize  = DramShadowSize;
        }
        PRN_CAL_VIG_LOG "Query the calib vig dram shadow size 0x%X ( table num %d )"
            PRN_CAL_VIG_ARG_UINT32 DramShadowSize PRN_CAL_VIG_ARG_POST
            PRN_CAL_VIG_ARG_UINT32 MaxTableNum    PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_DBG

        if (pWorkMemSize != NULL) {
            *pWorkMemSize = WorkingMemSize;
        }
        PRN_CAL_VIG_LOG "Query the calib vig working memory size 0x%X ( table num %d )"
            PRN_CAL_VIG_ARG_UINT32 WorkingMemSize PRN_CAL_VIG_ARG_POST
            PRN_CAL_VIG_ARG_UINT32 MaxTableNum    PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_DBG

        if (pCalcMemSize != NULL) {
            *pCalcMemSize = CalcMemSize;
        }
        PRN_CAL_VIG_LOG "Query the calib vig algo. memory size 0x%X ( table num %d )"
            PRN_CAL_VIG_ARG_UINT32 CalcMemSize PRN_CAL_VIG_ARG_POST
            PRN_CAL_VIG_ARG_UINT32 MaxTableNum PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_DBG
    }

    return RetVal;
}


static UINT32 SvcCalib_VigMemLock(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    SIZE_t CalcMemAlgoSize = 0U;
    SIZE_t CalcMemCtnSize = 0U;
    AMBA_CT_CONFIG_s CtnCfg;

    AmbaSvcWrap_MisraMemset(&CtnCfg, 0, sizeof(CtnCfg));
    if ((SVC_CalibVigCtrlFlag & SVC_CALIB_VIG_FLG_CALC_CTN_MAX_WIN) > 0U) {
        CtnCfg.Type.Vig1d.MaxImageSize.Width  = SVC_CalibVigCtrl.CalcCtrl.MaxRawWidth;
        CtnCfg.Type.Vig1d.MaxImageSize.Height = SVC_CalibVigCtrl.CalcCtrl.MaxRawHeight;
        CtnCfg.Type.Vig1d.Resolution          = SVC_CalibVigCtrl.CalcCtrl.MaxResolution;
    } else {
        CtnCfg.Type.Vig1d.MaxImageSize.Width  = SVC_CALIB_VIG_MAX_RAW_WIDTH;
        CtnCfg.Type.Vig1d.MaxImageSize.Height = SVC_CALIB_VIG_MAX_RAW_HEIGHT;
        CtnCfg.Type.Vig1d.Resolution          = SVC_CALIB_VIG_MAX_RESOLUTION;
    }

    PRN_CAL_VIG_LOG "Ctuner max window %04dx%04d, %d"
        PRN_CAL_VIG_ARG_UINT32 CtnCfg.Type.Vig1d.MaxImageSize.Width  PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_ARG_UINT32 CtnCfg.Type.Vig1d.MaxImageSize.Height PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_ARG_UINT32 CtnCfg.Type.Vig1d.Resolution          PRN_CAL_VIG_ARG_POST
    PRN_CAL_VIG_DBG

    /* Configure the vig algo memory */
    if (0U != AmbaCT_Cfg(AMBA_CT_TYPE_1D_VIG, &CtnCfg)) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to cfg vig table memory - config ctuner module fail!" PRN_CAL_VIG_NG
    } else if (0U != AmbaCT_GetBufSize(AMBA_CT_TYPE_1D_VIG, &CalcMemCtnSize)) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to cfg vig table memory - query vig ctuner working memory size fail!" PRN_CAL_VIG_NG
    } else if (CAL_OK != AmbaCal_1DVigGetWorkingBufSizeV1(&(CtnCfg.Type.Vig1d.MaxImageSize), CtnCfg.Type.Vig1d.Resolution, &CalcMemAlgoSize)) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to cfg vig module memory - get vig algo working memory fail!" PRN_CAL_VIG_NG
    } else {
        const UINT32 CaleMemBufNum = 2U;
        UINT8 *pCalcMemBuf[CaleMemBufNum];
        UINT32 CalcMemBufSize[CaleMemBufNum];

        AmbaSvcWrap_MisraMemset(pCalcMemBuf, 0, sizeof(pCalcMemBuf));
        AmbaSvcWrap_MisraMemset(CalcMemBufSize, 0, sizeof(CalcMemBufSize));

        if (0U != SvcCalib_ItemCalcMemGet(SVC_CALIB_VIGNETTE_ID, CaleMemBufNum, pCalcMemBuf, CalcMemBufSize)) {
            RetVal = SVC_NG;
            PRN_CAL_VIG_LOG "Fail to cfg vig table memory - query vig ctuner working memory size fail!" PRN_CAL_VIG_NG
        } else {
            UINT32 ReqIdx, MemIdx;
            SVC_CALIB_VIG_MEM_CTRL_s *pMemCtrl[SVC_CALIB_VIG_MEM_NUM];

            PRN_CAL_VIG_LOG "---- Calc Memory Info ----" PRN_CAL_VIG_DBG
            for (MemIdx = 0U; MemIdx < CaleMemBufNum; MemIdx ++) {
                PRN_CAL_VIG_LOG "[%02d] pCalcMemBuf   : %p"
                    PRN_CAL_VIG_ARG_UINT32 MemIdx              PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_ARG_CPOINT pCalcMemBuf[MemIdx] PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_DBG
                PRN_CAL_VIG_LOG "[%02d] CalcMemBufSize: 0x%X"
                    PRN_CAL_VIG_ARG_UINT32 MemIdx                 PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_ARG_UINT32 CalcMemBufSize[MemIdx] PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_DBG
            }

            PRN_CAL_VIG_LOG "Query algo/ctuner memory size 0x%08x/0x%08x"
                PRN_CAL_VIG_ARG_UINT32 CalcMemAlgoSize PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_ARG_UINT32 CalcMemCtnSize  PRN_CAL_VIG_ARG_POST
            PRN_CAL_VIG_DBG

            AmbaSvcWrap_MisraMemset(SVC_CalibVigCtrl.MemCtrl, 0, sizeof(SVC_CalibVigCtrl.MemCtrl));
            SVC_CalibVigCtrl.MemCtrl[SVC_CALIB_VIG_MEM_CALC].BufSize        = (UINT32)CalcMemAlgoSize;
            SVC_CalibVigCtrl.MemCtrl[SVC_CALIB_VIG_MEM_CALC_CTUNER].BufSize = (UINT32)CalcMemCtnSize;

            AmbaSvcWrap_MisraMemset(pMemCtrl, 0, sizeof(pMemCtrl));

            if (SVC_CalibVigCtrl.MemCtrl[SVC_CALIB_VIG_MEM_CALC].BufSize > SVC_CalibVigCtrl.MemCtrl[SVC_CALIB_VIG_MEM_CALC_CTUNER].BufSize) {
                pMemCtrl[0] = &(SVC_CalibVigCtrl.MemCtrl[SVC_CALIB_VIG_MEM_CALC]);
                pMemCtrl[1] = &(SVC_CalibVigCtrl.MemCtrl[SVC_CALIB_VIG_MEM_CALC_CTUNER]);
            } else {
                pMemCtrl[0] = &(SVC_CalibVigCtrl.MemCtrl[SVC_CALIB_VIG_MEM_CALC_CTUNER]);
                pMemCtrl[1] = &(SVC_CalibVigCtrl.MemCtrl[SVC_CALIB_VIG_MEM_CALC]);
            }

            for (ReqIdx = 0U; ReqIdx < SVC_CALIB_VIG_MEM_NUM; ReqIdx ++) {
                for (MemIdx = 0U; MemIdx < CaleMemBufNum; MemIdx ++) {
                    if (pMemCtrl[ReqIdx]->BufSize <= CalcMemBufSize[MemIdx]) {
                        pMemCtrl[ReqIdx]->pBuf = pCalcMemBuf[MemIdx];
                        pMemCtrl[ReqIdx]->Enable = 1U;

                        CalcMemBufSize[MemIdx] -= pMemCtrl[ReqIdx]->BufSize;
                        pCalcMemBuf[MemIdx] = &(pCalcMemBuf[MemIdx][pMemCtrl[ReqIdx]->BufSize]);
                        break;
                    }
                }

                if (pMemCtrl[ReqIdx]->pBuf == NULL) {
                    RetVal = SVC_NG;
                    PRN_CAL_VIG_LOG "Fail to cfg vig table memory - calib calculation memory is not enough to service 0x%X!"
                        PRN_CAL_VIG_ARG_UINT32 pMemCtrl[ReqIdx]->BufSize PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_NG
                }
            }

            if (RetVal != SVC_OK) {
                // Free calib calculation memory
                PRetVal = SvcCalib_ItemCalcMemFree(SVC_CALIB_VIGNETTE_ID); PRN_CAL_VIG_ERR_HDLR
            } else {
                PRN_CAL_VIG_LOG "---- Calc Memory Ctrl ----" PRN_CAL_VIG_API
                for (ReqIdx = 0U; ReqIdx < SVC_CALIB_VIG_MEM_NUM; ReqIdx ++) {
                    PRN_CAL_VIG_LOG "[%02d] pBuf    : %p"
                        PRN_CAL_VIG_ARG_UINT32 ReqIdx                 PRN_CAL_VIG_ARG_POST
                        PRN_CAL_VIG_ARG_CPOINT pMemCtrl[ReqIdx]->pBuf PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "[%02d] BufSize : 0x%X"
                        PRN_CAL_VIG_ARG_UINT32 ReqIdx                    PRN_CAL_VIG_ARG_POST
                        PRN_CAL_VIG_ARG_UINT32 pMemCtrl[ReqIdx]->BufSize PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                }
            }
        }
    }

    return RetVal;
}


static void SvcCalib_VigMemUnLock(void)
{
    // Reset the memory control setting
    AmbaSvcWrap_MisraMemset(SVC_CalibVigCtrl.MemCtrl, 0, sizeof(SVC_CalibVigCtrl.MemCtrl));

    // Free the calculation memory
    if (SVC_OK != SvcCalib_ItemCalcMemFree(SVC_CALIB_VIGNETTE_ID)) {
        PRN_CAL_VIG_LOG "Fail to un-lock vig calc memory - init calculation memory control fail!" PRN_CAL_VIG_NG
    } else {
        PRN_CAL_VIG_LOG "Successful to un-lock vig calc memory!" PRN_CAL_VIG_OK
    }
}

static UINT32 SvcCalib_VigCtnReadLine(AMBA_FS_FILE *pFile, char *pDstBuf, UINT32 DstBufSize)
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
                pDstBuf[RetLeng] = TmpChar;
                RetLeng += 1U;
            }
        }

        pDstBuf[RetLeng] = '\0';
    }

    return RetLeng;
}

static void SvcCalib_VigCtnGetU32Val(const char *pStrBuf, UINT32 *pVal)
{
    if ((pStrBuf != NULL) && (pVal != NULL)) {
        UINT32 CvtVal = 0U;
        UINT32 PRetVal = SvcWrap_strtoul(pStrBuf, &CvtVal); PRN_CAL_VIG_ERR_HDLR;
        if (PRetVal != 0U) {
            CvtVal = 0U;
        }

        *pVal = CvtVal;
    }
}

static UINT32 SvcCalib_VigCtnPreFetch(const char *pScriptPath)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    AMBA_FS_FILE *pFile = NULL;

    if (pScriptPath == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to pre-fetch ctuner - input script path should not null!" PRN_CAL_VIG_NG
    } else if (0U != AmbaFS_FileOpen(pScriptPath, "rb", &pFile)) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to pre-fetch ctuner - open script fail! script:'%s'"
            PRN_CAL_VIG_ARG_CSTR   pScriptPath PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_NG
    } else if (pFile == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to pre-fetch ctuner - invalid file pointer" PRN_CAL_VIG_NG
    } else {
        char LineBuf[128U];
        char *pCResult = NULL, *pCSearch = NULL;
        UINT32 TmpU32;
        UINT32 MaxWidth      = 0U;
        UINT32 MaxHeight     = 0U;
        UINT32 MaxResolution = 0U;

        while (0U < SvcCalib_VigCtnReadLine(pFile, LineBuf, (UINT32)sizeof(LineBuf))) {

            pCSearch = LineBuf;
            pCResult = SvcWrap_strstr(pCSearch, "raw[");
            if (pCResult != NULL) {
                pCSearch = pCResult;

                // Try to fetch 'raw[0].width 1920' pattern
                pCResult = SvcWrap_strstr(pCSearch, ".width ");
                if (pCResult != NULL) {
                    TmpU32 = 0U;
                    SvcCalib_VigCtnGetU32Val(&(pCResult[7]), &TmpU32);
                    if (TmpU32 > MaxWidth) {
                        MaxWidth = TmpU32;
                    }
                    continue;
                }

                // Try to fetch 'raw[0].height 1080' pattern
                pCResult = SvcWrap_strstr(pCSearch, ".height ");
                if (pCResult != NULL) {
                    TmpU32 = 0U;
                    SvcCalib_VigCtnGetU32Val(&(pCResult[8]), &TmpU32);
                    if (TmpU32 > MaxHeight) {
                        MaxHeight = TmpU32;
                    }
                    continue;
                }

                // Try to fetch 'raw[0].resolution 12' pattern
                pCResult = SvcWrap_strstr(pCSearch, ".resolution ");
                if (pCResult != NULL) {
                    TmpU32 = 0U;
                    SvcCalib_VigCtnGetU32Val(&(pCResult[12]), &TmpU32);
                    if (TmpU32 > MaxResolution) {
                        MaxResolution = TmpU32;
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
                        if (0U != SvcCalib_DriverSlotCmp(*pCResult, pScriptPath[0])) {
                            PRN_CAL_VIG_LOG "Ctuner raw file device slot('%c') does not same with Ctuner script file device slot('%c')"
                                ; CalibVigPrint.Argc ++; CalibVigPrint.Argv[CalibVigPrint.Argc].Char = *pCResult
                                ; CalibVigPrint.Argc ++; CalibVigPrint.Argv[CalibVigPrint.Argc].Char = pScriptPath[0]
                            PRN_CAL_VIG_NG

                            RetVal = SVC_NG;
                        }
                    }

                    continue;
                }
            }
        }

        if ((MaxWidth > 0U) && (MaxHeight > 0U) && (MaxResolution > 0U)) {
            SVC_CalibVigCtrl.CalcCtrl.MaxRawWidth   = MaxWidth;
            SVC_CalibVigCtrl.CalcCtrl.MaxRawHeight  = MaxHeight;
            SVC_CalibVigCtrl.CalcCtrl.MaxResolution = MaxResolution;
            SVC_CalibVigCtrlFlag |= SVC_CALIB_VIG_FLG_CALC_CTN_MAX_WIN;

            PRN_CAL_VIG_LOG "Update VIG max raw window: %04dx%04d, %d"
                PRN_CAL_VIG_ARG_UINT32 SVC_CalibVigCtrl.CalcCtrl.MaxRawWidth   PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_ARG_UINT32 SVC_CalibVigCtrl.CalcCtrl.MaxRawHeight  PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_ARG_UINT32 SVC_CalibVigCtrl.CalcCtrl.MaxResolution PRN_CAL_VIG_ARG_POST
            PRN_CAL_VIG_API
        }

        PRetVal = AmbaFS_FileClose(pFile); PRN_CAL_VIG_ERR_HDLR

        AmbaMisra_TouchUnused(pCSearch);
        AmbaMisra_TouchUnused(pCResult);
    }

    return RetVal;
}

static void SvcCalib_VigShellStrToU32(const char *pString, UINT32 *pValue)
{
    UINT32 ErrCode = AmbaUtility_StringToUInt32(pString, pValue);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }
}

static UINT32 SvcCalib_VigShellCfgCalChan(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibVigCtrlFlag & SVC_CALIB_VIG_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to cfg vig vin/sensor id - initial vig module first!" PRN_CAL_VIG_NG
    } else if (ArgCount <= 6U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to cfg vig vin/sensor id - Argc should > 6" PRN_CAL_VIG_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to cfg vig vin/sensor id - Argv should not null!" PRN_CAL_VIG_NG
    } else {
        UINT32 ArgIdx = 3U;
        UINT32 CurArgCnt, ArgParsingNum, Idx;
        UINT32 VinID            = 0U;
        UINT32 SensorID         = 0U;
        UINT32 VinSelectBits    = 0U;
        UINT32 SensorSelectBits = 0U;

        CurArgCnt = ArgCount - ArgIdx;

        ArgParsingNum = CurArgCnt / 4U;

        if ((CurArgCnt != 4U) &&
            (CurArgCnt != 8U) &&
            (CurArgCnt != 12U) &&
            (CurArgCnt != 16U) ) {
            RetVal = SVC_NG;
            PRN_CAL_VIG_LOG "Fail to cfg vig vin/sensor id - Argc(%d) should be follow list!"
                PRN_CAL_VIG_ARG_UINT32 ArgCount PRN_CAL_VIG_ARG_POST
            PRN_CAL_VIG_NG
            PRN_CAL_VIG_LOG "  Chan num | arg num" PRN_CAL_VIG_NG
            PRN_CAL_VIG_LOG "     1     |    7" PRN_CAL_VIG_NG
            PRN_CAL_VIG_LOG "     2     |   11" PRN_CAL_VIG_NG
            PRN_CAL_VIG_LOG "     3     |   15" PRN_CAL_VIG_NG
            PRN_CAL_VIG_LOG "     4     |   19" PRN_CAL_VIG_NG
        } else if (ArgParsingNum >= SVC_CALIB_VIG_MAX_CHANNEL_NUM) {
            RetVal = SVC_NG;
            PRN_CAL_VIG_LOG "Fail to cfg vig vin/sensor id - argument parsing turn(%d) is out-of limitation(%d)!"
                PRN_CAL_VIG_ARG_UINT32 ArgParsingNum                 PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_ARG_UINT32 SVC_CALIB_VIG_MAX_CHANNEL_NUM PRN_CAL_VIG_ARG_POST
            PRN_CAL_VIG_NG
        } else {

            AmbaSvcWrap_MisraMemset(&(SVC_CalibVigCtrl.CalcCtrl), 0, sizeof(SVC_CALIB_VIG_CALC_CTRL_s));
            AmbaSvcWrap_MisraMemset(SVC_CalibVigCtrl.CalibChan, 0, sizeof(SVC_CalibVigCtrl.CalibChan));
            SVC_CalibVigCtrlFlag &= ~( SVC_CALIB_VIG_FLG_CALC_INIT | SVC_CALIB_VIG_FLG_CALC_VIN_SEN | SVC_CALIB_VIG_FLG_CALC_CTN_MAX_WIN);

            for (Idx = 0U; Idx < ArgParsingNum; Idx ++) {

                SvcCalib_VigShellStrToU32(pArgVector[ArgIdx     ], &VinID           );
                SvcCalib_VigShellStrToU32(pArgVector[ArgIdx + 1U], &SensorID        );
                SvcCalib_VigShellStrToU32(pArgVector[ArgIdx + 2U], &VinSelectBits   );
                SvcCalib_VigShellStrToU32(pArgVector[ArgIdx + 3U], &SensorSelectBits);
                ArgIdx += 4U;

                if (SVC_OK != SvcCalib_VigVinSensorIDCheck(VinID, SensorID)) {
                    RetVal = SVC_NG;
                    PRN_CAL_VIG_LOG "Fail to cfg vig vin/sensor id - invalid VinID %d, SensorID %d"
                        PRN_CAL_VIG_ARG_UINT32 VinID    PRN_CAL_VIG_ARG_POST
                        PRN_CAL_VIG_ARG_UINT32 SensorID PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_NG
                    break;
                } else {
                    SVC_CalibVigCtrl.CalibChan[Idx].VinSelectBits    = VinSelectBits   ;
                    SVC_CalibVigCtrl.CalibChan[Idx].SensorSelectBits = SensorSelectBits;
                    SVC_CalibVigCtrl.CalibChan[Idx].VinID            = VinID           ;
                    SVC_CalibVigCtrl.CalibChan[Idx].SensorID         = SensorID        ;
                    SVC_CalibVigCtrl.CalibChan[Idx].ExtendData       = 0xFFFFFFFFU     ;
                }
            }
        }

        if (RetVal == SVC_OK) {

            /* Initial the vig calculate parameters */
            SVC_CalibVigCtrl.CalcCtrl.SaveBinOn                    = 1U;
            SVC_CalibVigCtrl.CalcCtrl.TableIdx                     = 0xFFFFFFFFU;
            SVC_CalibVigCtrl.CalcCtrl.MaxRawWidth                  = SVC_CALIB_VIG_MAX_RAW_WIDTH;
            SVC_CalibVigCtrl.CalcCtrl.MaxRawHeight                 = SVC_CALIB_VIG_MAX_RAW_HEIGHT;
            SVC_CalibVigCtrl.CalcCtrl.MaxResolution                = SVC_CALIB_VIG_MAX_RESOLUTION;
            SVC_CalibVigCtrlFlag |= SVC_CALIB_VIG_FLG_CALC_INIT;

            /* Configure the calib channel */
            SVC_CalibVigCtrlFlag |= SVC_CALIB_VIG_FLG_CALC_VIN_SEN;

            for (Idx = 0U; Idx < ArgParsingNum; Idx ++) {
                PRN_CAL_VIG_LOG " " PRN_CAL_VIG_API
                PRN_CAL_VIG_LOG "------ Calibration VIG Channel %02d ------"
                    PRN_CAL_VIG_ARG_UINT32 Idx PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_API
                PRN_CAL_VIG_LOG "  VinSelectBits    : 0x%X"
                    PRN_CAL_VIG_ARG_UINT32 SVC_CalibVigCtrl.CalibChan[Idx].VinSelectBits PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_API
                PRN_CAL_VIG_LOG "  SensorSelectBits : 0x%X"
                    PRN_CAL_VIG_ARG_UINT32 SVC_CalibVigCtrl.CalibChan[Idx].SensorSelectBits PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_API
                PRN_CAL_VIG_LOG "  VinID            : 0x%X"
                    PRN_CAL_VIG_ARG_UINT32 SVC_CalibVigCtrl.CalibChan[Idx].VinID PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_API
                PRN_CAL_VIG_LOG "  SensorID         : 0x%X"
                    PRN_CAL_VIG_ARG_UINT32 SVC_CalibVigCtrl.CalibChan[Idx].SensorID PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_API
            }
        }
    }

    return RetVal;
}


static void SvcCalib_VigShellCfgCalChanU(void)
{
    PRN_CAL_VIG_LOG "  %scfg_calib_chan%s        : reset all calibration setting as default"
        PRN_CAL_VIG_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_VIG_ARG_POST
    PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "  ------------------------------------------------------" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [VinID]             : vin identify number." PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [SensorID]          : sensor identify number. 1/2/4/8" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [VinSelectBits]     : bit0 : Vin0, bit1 : Vin1, ... " PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [SensorSelectBits]  : bit0 : sensor0, bit1 : sensor1, ..." PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG " " PRN_CAL_VIG_API
}


static UINT32 SvcCalib_VigShellCfgTblID(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibVigCtrlFlag & SVC_CALIB_VIG_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to cfg vig table idx - initial vig module first!" PRN_CAL_VIG_NG
    } else if (ArgCount <= 3U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to cfg vig table idx - Argc should > 3" PRN_CAL_VIG_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to cfg vig table idx - Argv should not null!" PRN_CAL_VIG_NG
    } else {
        UINT32 TableIdx = 0U;

        SvcCalib_VigShellStrToU32(pArgVector[3U],  &TableIdx );

        SVC_CalibVigCtrl.CalcCtrl.TableIdx = TableIdx;

        PRN_CAL_VIG_LOG " " PRN_CAL_VIG_API
        PRN_CAL_VIG_LOG "------ Calibration VIG Table Index %d ------"
            PRN_CAL_VIG_ARG_UINT32 SVC_CalibVigCtrl.CalcCtrl.TableIdx PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_API

    }

    return RetVal;
}


static void SvcCalib_VigShellCfgTblIDU(void)
{
    PRN_CAL_VIG_LOG "  %scfg_table_id%s          :"
        PRN_CAL_VIG_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_VIG_ARG_POST
    PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "  ------------------------------------------------------" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [Nand Table ID]     : the nand table id." PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "                        : the max value depend on each app" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG " " PRN_CAL_VIG_API
}


static UINT32 SvcCalib_VigShellCfgDbgMsg(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibVigCtrlFlag & SVC_CALIB_VIG_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to switch vig debug msg - initial vig module first!" PRN_CAL_VIG_NG
    } else if (ArgCount <= 3U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to switch vig debug msg - Argc should > 3" PRN_CAL_VIG_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to switch vig debug msg - Argv should not null!" PRN_CAL_VIG_NG
    } else {
        UINT32 DbgMsgOn = 0U;

        SvcCalib_VigShellStrToU32(pArgVector[3U],  &DbgMsgOn );

        if ((DbgMsgOn & 0x1U) > 0U) {
            SVC_CalibVigCtrl.EnableDebugMsg = DbgMsgOn;
        }

        if ((DbgMsgOn & 0x2U) > 0U) {
            SVC_CalibVigCtrlFlag |= SVC_LOG_CAL_DBG;
        }

        PRN_CAL_VIG_LOG " " PRN_CAL_VIG_API
        PRN_CAL_VIG_LOG "------ Calibration VIG Debug Msg On/Off %d, lvl %d ------"
            PRN_CAL_VIG_ARG_UINT32 SVC_CalibVigCtrl.EnableDebugMsg PRN_CAL_VIG_ARG_POST
            PRN_CAL_VIG_ARG_UINT32 DbgMsgOn                        PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_API

    }

    return RetVal;
}


static void SvcCalib_VigShellCfgDbgMsgU(void)
{
    PRN_CAL_VIG_LOG "  %scfg_dbg_msg%s           :"
        PRN_CAL_VIG_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_VIG_ARG_POST
    PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "  ------------------------------------------------------" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [Enable]            : 0: disable debug message" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "                        : 1: enable debug message" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "                        : 2: enable debug message level 2" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG " " PRN_CAL_VIG_API
}


static UINT32 SvcCalib_VigShellCfgSyncTbl(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibVigCtrlFlag & SVC_CALIB_VIG_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to sync vig table - initial vig module first!" PRN_CAL_VIG_NG
    } else if (ArgCount <= 4U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to sync vig table - Argc should > 4" PRN_CAL_VIG_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to sync vig table - Argv should not null!" PRN_CAL_VIG_NG
    } else {
        UINT32 TableSelectBits  = 0U;
        UINT32 MaxDiffThreshold = 0U;

        SvcCalib_VigShellStrToU32(pArgVector[3U], &(TableSelectBits ) );
        SvcCalib_VigShellStrToU32(pArgVector[4U], &(MaxDiffThreshold) );

        AmbaSvcWrap_MisraMemset(&(SVC_CalibVigCtrl.CalcCtrl.SyncTbl), 0, sizeof(SVC_CALIB_VIG_SYNC_TBL_s));
        SVC_CalibVigCtrl.CalcCtrl.SyncTbl.TblSelectBits = TableSelectBits;
        SVC_CalibVigCtrl.CalcCtrl.SyncTbl.DiffThreshold = MaxDiffThreshold;

        PRN_CAL_VIG_LOG " " PRN_CAL_VIG_API
        PRN_CAL_VIG_LOG "------ Calibration VIG Sync Table Param ------" PRN_CAL_VIG_API
        PRN_CAL_VIG_LOG "  TableSelectBits  : 0x%X"
            PRN_CAL_VIG_ARG_UINT32 SVC_CalibVigCtrl.CalcCtrl.SyncTbl.TblSelectBits PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_API
        PRN_CAL_VIG_LOG "  MaxDiffThreshold : %d"
            PRN_CAL_VIG_ARG_UINT32 SVC_CalibVigCtrl.CalcCtrl.SyncTbl.DiffThreshold PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_API

        PRetVal = SvcCalib_VigTableSync(); PRN_CAL_VIG_ERR_HDLR

    }

    return RetVal;
}


static void SvcCalib_VigShellCfgSyncTblU(void)
{
    PRN_CAL_VIG_LOG "  %scfg_sync_table%s        :"
        PRN_CAL_VIG_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_VIG_ARG_POST
    PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "  ------------------------------------------------------" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [TableSelectBits]   : The NAND table select bits." PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "                        : In AVM case, there are 4 vig table in NAND." PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "                        : If we want to sync all table, the TableSelectBits = 0xF" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "                        : If we want to sync no.0 and no.2 table, the TableSelectBits = 0x5" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [MaxDiffThreshold]  : Configure the threshold for max strength difference" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG " " PRN_CAL_VIG_API
}


static UINT32 SvcCalib_VigShellGenTbl(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibVigCtrlFlag & SVC_CALIB_VIG_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to proc ldc 'gen_table' - initial vig module first!" PRN_CAL_VIG_NG
    } else if (ArgCount <= 3U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to proc ldc 'gen_table' - Argc should > 3" PRN_CAL_VIG_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to proc ldc 'gen_table' - Argv should not null!" PRN_CAL_VIG_NG
    } else {
        RetVal = SvcCalib_VigTableGenTbl(SVC_CalibVigCtrl.CalibChan, pArgVector[3U]);
    }

    return RetVal;
}


static void SvcCalib_VigShellGenTblU(void)
{
    PRN_CAL_VIG_LOG "  %sgen_table%s             : start calculate VIG table and save to NVM"
        PRN_CAL_VIG_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_VIG_ARG_POST
    PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "  ------------------------------------------------------" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [Script]            : VIG ctuner script path." PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG " " PRN_CAL_VIG_API
}


static UINT32 SvcCalib_VigShellEnable(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibVigCtrlFlag & SVC_CALIB_VIG_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to enable/disable vig - initial vig module first!" PRN_CAL_VIG_NG
    } else if (ArgCount <= 4U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to enable/disable vig - Argc should > 4" PRN_CAL_VIG_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to enable/disable vig - Argv should not null!" PRN_CAL_VIG_NG
    } else {
        UINT32 IkContextID = 0xFFFFFFFFU, Enable = 0U;

        SvcCalib_VigShellStrToU32(pArgVector[3U],  &IkContextID );
        SvcCalib_VigShellStrToU32(pArgVector[4U],  &Enable      );
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
            PRetVal = AmbaIK_GetVignetteEnb(&ImgMode, &CurIkState);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_CAL_VIG_LOG "Fail to enable/disable vig - get vignette enable/disable state fail" PRN_CAL_VIG_NG
            } else {
                if (CurIkState != Enable) {
                    PRetVal = AmbaIK_SetVignetteEnb(&ImgMode, Enable);
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;

                        PRN_CAL_VIG_LOG "Fail to enable/disable vig - %s vignette fail!"
                            PRN_CAL_VIG_ARG_CSTR   ( (Enable > 0U) ? ( "enable" ) : ( "disable" ) ) PRN_CAL_VIG_ARG_POST
                        PRN_CAL_VIG_NG
                    } else {
                        PRN_CAL_VIG_LOG "%s vignette done!"
                            PRN_CAL_VIG_ARG_CSTR   ( (Enable > 0U) ? ( "enable" ) : ( "disable" ) ) PRN_CAL_VIG_ARG_POST
                        PRN_CAL_VIG_API
                    }
                } else {
                    PRN_CAL_VIG_LOG "current IK state: %s!"
                        PRN_CAL_VIG_ARG_CSTR   ( (CurIkState > 0U) ? ( "enable" ) : ( "disable" ) ) PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                }
            }
        } else {
            RetVal = SVC_NG;
            PRN_CAL_VIG_LOG "Fail to enable/disable vig - invalid ik context id(%d)"
                PRN_CAL_VIG_ARG_UINT32 IkContextID PRN_CAL_VIG_ARG_POST
            PRN_CAL_VIG_NG
        }
    }

    return RetVal;
}


static void SvcCalib_VigShellEnableU(void)
{
    PRN_CAL_VIG_LOG "  %senable%s                : enable/disable Vignette effect"
        PRN_CAL_VIG_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_VIG_ARG_POST
    PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "  ------------------------------------------------------" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [IK Context ID]     : image kernel context id." PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [Enable]            : 0 => disable" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "                        : 1 => enable" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG " " PRN_CAL_VIG_API
}


static UINT32 SvcCalib_VigShellUpd(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibVigCtrlFlag & SVC_CALIB_VIG_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to proc vig->upd cmd - initial vig module first!" PRN_CAL_VIG_NG
    } else if (ArgCount <= 5U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to proc vig->upd cmd - Argc should > 5" PRN_CAL_VIG_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to proc vig->upd cmd - Argv should not null!" PRN_CAL_VIG_NG
    } else {
        UINT32 VinID       = 0xFFFFFFFFU;
        UINT32 SensorID    = 0xFFFFFFFFU;
        UINT32 IkContextID = 0xFFFFFFFFU;

        SvcCalib_VigShellStrToU32(pArgVector[3U],  &VinID       );
        SvcCalib_VigShellStrToU32(pArgVector[4U],  &SensorID    );
        SvcCalib_VigShellStrToU32(pArgVector[5U],  &IkContextID );

        if (SVC_OK != SvcCalib_VigVinSensorIDCheck(VinID, SensorID)) {
            RetVal = SVC_NG;
            PRN_CAL_VIG_LOG "Fail to proc vig->upd cmd - invalid VinID(%d), SensorID(%d)!"
                PRN_CAL_VIG_ARG_UINT32 VinID    PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_ARG_UINT32 SensorID PRN_CAL_VIG_ARG_POST
            PRN_CAL_VIG_NG
        } else if (IkContextID == 0xFFFFFFFFU) {
            RetVal = SVC_NG;
            PRN_CAL_VIG_LOG "Fail to proc vig->upd cmd - invalid IKContextID(%d)!"
                PRN_CAL_VIG_ARG_UINT32 IkContextID PRN_CAL_VIG_ARG_POST
            PRN_CAL_VIG_NG
        } else {
            UINT32 CurIkEnable = 0U;
            AMBA_IK_MODE_CFG_s ImgMode;

            AmbaSvcWrap_MisraMemset(&ImgMode, 0, sizeof(ImgMode));
            ImgMode.ContextId = IkContextID;
            PRetVal = AmbaIK_GetVignetteEnb(&ImgMode, &CurIkEnable);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_CAL_VIG_LOG "Fail to proc vig->upd cmd - get vig enable/disable state fail! ErrCode: 0x%x"
                    PRN_CAL_VIG_ARG_UINT32 PRetVal PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_NG
            } else {

                // If the vig has been enable, disable it before update vig table
                if (CurIkEnable > 0U) {
                    PRetVal = AmbaIK_SetVignetteEnb(&ImgMode, 0U);
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;
                        PRN_CAL_VIG_LOG "Fail to proc vig->upd cmd - disable vig failure! ErrCode: 0x%x"
                            PRN_CAL_VIG_ARG_UINT32 PRetVal PRN_CAL_VIG_ARG_POST
                        PRN_CAL_VIG_NG
                    }
                }

                if (RetVal == SVC_OK) {
                    UINT32 WorkTblIdx = 0xFFFFFFFFU;
                    SVC_CALIB_CHANNEL_s CalibChan;

                    // If working buffer table exist, reset it first
                    PRetVal = SvcCalib_VigTableSearch(VinID, SensorID, &WorkTblIdx); PRN_CAL_VIG_ERR_HDLR
                    if (WorkTblIdx < SVC_CalibVigCtrl.NumOfTable) {
                        AmbaSvcWrap_MisraMemset(&(SVC_CalibVigCtrl.pVigTable[WorkTblIdx]), 0, sizeof(SVC_CALIB_VIG_TABLE_s));
                    }

                    // Update the calib table from nand
                    AmbaSvcWrap_MisraMemset(&CalibChan, 0, sizeof(CalibChan));
                    CalibChan.VinID    = VinID;
                    CalibChan.SensorID = SensorID;
                    RetVal = SvcCalib_VigTableUpdate(&CalibChan, &ImgMode);
                    if (RetVal != 0U) {
                        PRN_CAL_VIG_LOG "Fail to proc vig->upd cmd - update calib table fail! VinID(%d) SensorID(%d), IkContextID(%d)"
                            PRN_CAL_VIG_ARG_UINT32 VinID       PRN_CAL_VIG_ARG_POST
                            PRN_CAL_VIG_ARG_UINT32 SensorID    PRN_CAL_VIG_ARG_POST
                            PRN_CAL_VIG_ARG_UINT32 IkContextID PRN_CAL_VIG_ARG_POST
                        PRN_CAL_VIG_NG
                    }
                }
            }
        }
    }

    return RetVal;
}


static void SvcCalib_VigShellUpdU(void)
{
    PRN_CAL_VIG_LOG "  %supd%s                   : update Vignette table"
        PRN_CAL_VIG_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_VIG_ARG_POST
    PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "  ------------------------------------------------------" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [VinID]             : vin identify number." PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [SensorID]          : sensor identify number. 1/2/4/8" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [IK Context ID]     : image kernel context id." PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG " " PRN_CAL_VIG_API
}


static UINT32 SvcCalib_VigShellSet(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibVigCtrlFlag & SVC_CALIB_VIG_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to proc vig->upd cmd - initial vig module first!" PRN_CAL_VIG_NG
    } else if ((SVC_CalibVigCtrlFlag & SVC_CALIB_VIG_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to proc vig->set cmd - initial vig shadow first!" PRN_CAL_VIG_NG
    } else if (SVC_CalibVigCtrl.pNandTable == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to proc vig->set cmd - invalid vig nand table!" PRN_CAL_VIG_NG
    } else if (ArgCount <= 5U) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to proc vig->set cmd - Argc should > 5" PRN_CAL_VIG_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to proc vig->set cmd - Argv should not null!" PRN_CAL_VIG_NG
    } else {
        UINT32 TblID = 0xFFU, ArgIdx = 3U;

        SvcCalib_VigShellStrToU32(pArgVector[ArgIdx],  &TblID); ArgIdx ++;
        if (TblID >= SVC_CalibVigCtrl.NumOfTable) {
            RetVal = SVC_NG;
            PRN_CAL_VIG_LOG "Fail to proc vig->set cmd - insert table id(%d) is out-of range(%d)!"
                PRN_CAL_VIG_ARG_UINT32 TblID                       PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_ARG_UINT32 SVC_CalibVigCtrl.NumOfTable PRN_CAL_VIG_ARG_POST
            PRN_CAL_VIG_NG
        } else {
            SVC_CALIB_VIG_NAND_TABLE_s CurVigTbl;
            const UINT32 *pU32 = &(CurVigTbl.GainTblR[0][0]);
            void         *pVigBuf = NULL;
            const UINT32 VigBufSize = 8192U; // ( sizeof(UINT32) x 4 x 128 ) x 4-channel
            const char *pVigFilePath = NULL;
            SVC_CALIB_VIG_NAND_TABLE_s *pNandTable = &(SVC_CalibVigCtrl.pNandTable[TblID]);
            AMBA_FS_FILE_INFO_s VigFileInfo;
            UINT32 DbgMsgOn = 1U;

            AmbaMisra_TypeCast(&pVigBuf, &pU32);

            AmbaSvcWrap_MisraMemset(&CurVigTbl, 0, sizeof(SVC_CALIB_VIG_NAND_TABLE_s));
            CurVigTbl.Header.AlgoVersion = 0xFFFFFFFFU;
            CurVigTbl.Header.Version     = SVC_CALIB_VIG_VERSION;

            SvcCalib_VigShellStrToU32(pArgVector[ArgIdx], &(CurVigTbl.Header.VinSelectBits   )); ArgIdx++;
            SvcCalib_VigShellStrToU32(pArgVector[ArgIdx], &(CurVigTbl.Header.SensorSelectBits)); ArgIdx++;

            SvcCalib_VigShellStrToU32(pArgVector[ArgIdx], &(CurVigTbl.Header.VinSensorGeo.StartX              )); ArgIdx++;
            SvcCalib_VigShellStrToU32(pArgVector[ArgIdx], &(CurVigTbl.Header.VinSensorGeo.StartY              )); ArgIdx++;
            SvcCalib_VigShellStrToU32(pArgVector[ArgIdx], &(CurVigTbl.Header.VinSensorGeo.Width               )); ArgIdx++;
            SvcCalib_VigShellStrToU32(pArgVector[ArgIdx], &(CurVigTbl.Header.VinSensorGeo.Height              )); ArgIdx++;
            SvcCalib_VigShellStrToU32(pArgVector[ArgIdx], &(CurVigTbl.Header.VinSensorGeo.HSubSample.FactorDen)); ArgIdx++;
            SvcCalib_VigShellStrToU32(pArgVector[ArgIdx], &(CurVigTbl.Header.VinSensorGeo.HSubSample.FactorNum)); ArgIdx++;
            SvcCalib_VigShellStrToU32(pArgVector[ArgIdx], &(CurVigTbl.Header.VinSensorGeo.VSubSample.FactorDen)); ArgIdx++;
            SvcCalib_VigShellStrToU32(pArgVector[ArgIdx], &(CurVigTbl.Header.VinSensorGeo.VSubSample.FactorNum)); ArgIdx++;

            SvcCalib_VigShellStrToU32(pArgVector[ArgIdx], &(CurVigTbl.Header.RadialCoarse     )); ArgIdx++;
            SvcCalib_VigShellStrToU32(pArgVector[ArgIdx], &(CurVigTbl.Header.RadialCoarseLog  )); ArgIdx++;
            SvcCalib_VigShellStrToU32(pArgVector[ArgIdx], &(CurVigTbl.Header.RadialBinsFine   )); ArgIdx++;
            SvcCalib_VigShellStrToU32(pArgVector[ArgIdx], &(CurVigTbl.Header.RadialBinsFineLog)); ArgIdx++;
            SvcCalib_VigShellStrToU32(pArgVector[ArgIdx], &(CurVigTbl.Header.ModelCenterX_R   )); ArgIdx++;
            SvcCalib_VigShellStrToU32(pArgVector[ArgIdx], &(CurVigTbl.Header.ModelCenterX_Gr  )); ArgIdx++;
            SvcCalib_VigShellStrToU32(pArgVector[ArgIdx], &(CurVigTbl.Header.ModelCenterX_B   )); ArgIdx++;
            SvcCalib_VigShellStrToU32(pArgVector[ArgIdx], &(CurVigTbl.Header.ModelCenterX_Gb  )); ArgIdx++;
            SvcCalib_VigShellStrToU32(pArgVector[ArgIdx], &(CurVigTbl.Header.ModelCenterY_R   )); ArgIdx++;
            SvcCalib_VigShellStrToU32(pArgVector[ArgIdx], &(CurVigTbl.Header.ModelCenterY_Gr  )); ArgIdx++;
            SvcCalib_VigShellStrToU32(pArgVector[ArgIdx], &(CurVigTbl.Header.ModelCenterY_B   )); ArgIdx++;
            SvcCalib_VigShellStrToU32(pArgVector[ArgIdx], &(CurVigTbl.Header.ModelCenterY_Gb  )); ArgIdx++;

            SvcCalib_VigShellStrToU32(pArgVector[ArgIdx], &(CurVigTbl.Header.SyncCalInfo[0])); ArgIdx++;
            SvcCalib_VigShellStrToU32(pArgVector[ArgIdx], &(CurVigTbl.Header.SyncCalInfo[1])); ArgIdx++;
            SvcCalib_VigShellStrToU32(pArgVector[ArgIdx], &(CurVigTbl.Header.SyncCalInfo[2])); ArgIdx++;
            SvcCalib_VigShellStrToU32(pArgVector[ArgIdx], &(CurVigTbl.Header.SyncCalInfo[3])); ArgIdx++;

            pVigFilePath = pArgVector[ArgIdx]; ArgIdx++;
            SvcCalib_VigShellStrToU32(pArgVector[ArgIdx], &DbgMsgOn);

            AmbaSvcWrap_MisraMemset(&VigFileInfo, 0, sizeof(VigFileInfo));
            PRetVal = AmbaFS_GetFileInfo(pVigFilePath, &VigFileInfo);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_CAL_VIG_LOG "Fail to proc vig->set cmd - get file info fail! ErrCode(0x%08X) '%s'"
                    PRN_CAL_VIG_ARG_UINT32 PRetVal      PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_ARG_CSTR   pVigFilePath PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_NG
            } else if ((UINT32)(VigFileInfo.Size) < VigBufSize) {
                RetVal = SVC_NG;
                PRN_CAL_VIG_LOG "Fail to proc vig->set cmd - file size(0x%08X) does not match buffer(0x%08X)!"
                    PRN_CAL_VIG_ARG_UINT32 VigFileInfo.Size PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_ARG_UINT32 VigBufSize       PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_NG
            } else {
                AMBA_FS_FILE *pVigFile = NULL;

                PRetVal = AmbaFS_FileOpen(pVigFilePath, "rb", &pVigFile);
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    PRN_CAL_VIG_LOG "Fail to proc vig->set cmd - open file fail! ErrCode(0x%08X)"
                        PRN_CAL_VIG_ARG_UINT32 PRetVal PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_NG
                } else if (pVigFile == NULL) {
                    RetVal = SVC_NG;
                    PRN_CAL_VIG_LOG "Fail to proc vig->set cmd - invalid file pointer" PRN_CAL_VIG_NG
                } else {
                    UINT32 NumSuccess = 0U;
                    PRetVal = AmbaFS_FileRead(pVigBuf, 1, VigBufSize, pVigFile, &NumSuccess);
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;
                        PRN_CAL_VIG_LOG "Fail to proc vig->set cmd - read data fail! ErrCode(0x%08X)"
                            PRN_CAL_VIG_ARG_UINT32 PRetVal PRN_CAL_VIG_ARG_POST
                        PRN_CAL_VIG_NG
                    } else if (NumSuccess != VigBufSize) {
                        RetVal = SVC_NG;
                        PRN_CAL_VIG_LOG "Fail to proc vig->set cmd - read size(0x%X) does not same with request size(0x%X) fail!"
                            PRN_CAL_VIG_ARG_UINT32 NumSuccess PRN_CAL_VIG_ARG_POST
                            PRN_CAL_VIG_ARG_UINT32 VigBufSize PRN_CAL_VIG_ARG_POST
                        PRN_CAL_VIG_NG
                    } else {
                        // misra-c
                    }

                    PRetVal = AmbaFS_FileClose(pVigFile); PRN_CAL_VIG_ERR_HDLR
                }
            }

            if (RetVal == 0U) {
                if (KAL_ERR_NONE != SvcCalib_VigMutexTake(&(SVC_CalibVigCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
                    RetVal = SVC_NG;
                    PRN_CAL_VIG_LOG "Fail to proc vig->set cmd - take mutex fail!" PRN_CAL_VIG_NG
                } else {

                    AmbaSvcWrap_MisraMemcpy(pNandTable, &CurVigTbl, sizeof(SVC_CALIB_VIG_NAND_TABLE_s));

                    SVC_CalibVigCtrl.pNandHeader->Enable = 1U;
                    SVC_CalibVigCtrl.pNandHeader->DebugMsgOn = DbgMsgOn;
                    if ((SVC_CalibVigCtrl.pNandHeader->TableSelectBits & SvcCalib_BitGet(TblID)) == 0U) {
                        SVC_CalibVigCtrl.pNandHeader->TableSelectBits |= SvcCalib_BitGet(TblID);
                        SVC_CalibVigCtrl.pNandHeader->NumOfTable ++;
                    }

                    PRN_CAL_VIG_LOG " " PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "====== Calibration vig %02d info ======"
                        PRN_CAL_VIG_ARG_UINT32 TblID        PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  Version                           : 0x%08x"
                        PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.Version PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  AlgoVersion                       : %d"
                        PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.AlgoVersion PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  VinSelectBits                     : 0x%x"
                        PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.VinSelectBits PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  SensorSelectBits                  : 0x%x"
                        PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.SensorSelectBits PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  VinSensorGeo.StartX               : %d"
                        PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.VinSensorGeo.StartX PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  VinSensorGeo.StartY               : %d"
                        PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.VinSensorGeo.StartY PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  VinSensorGeo.Width                : %d"
                        PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.VinSensorGeo.Width PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  VinSensorGeo.Height               : %d"
                        PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.VinSensorGeo.Height PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  VinSensorGeo.HSubSample.FactorDen : %d"
                        PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.VinSensorGeo.HSubSample.FactorDen PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  VinSensorGeo.HSubSample.FactorNum : %d"
                        PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.VinSensorGeo.HSubSample.FactorNum PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  VinSensorGeo.VSubSample.FactorDen : %d"
                        PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.VinSensorGeo.VSubSample.FactorDen PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  VinSensorGeo.VSubSample.FactorNum : %d"
                        PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.VinSensorGeo.VSubSample.FactorNum PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  RadialCoarse                      : %d"
                        PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.RadialCoarse PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  RadialCoarseLog                   : %d"
                        PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.RadialCoarseLog PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  RadialBinsFine                    : %d"
                        PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.RadialBinsFine PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  RadialBinsFineLog                 : %d"
                        PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.RadialBinsFineLog PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  ModelCenterX_R  ModelCenterY_R   : %d, %d"
                        PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.ModelCenterX_R PRN_CAL_VIG_ARG_POST
                        PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.ModelCenterY_R PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  ModelCenterX_Gr ModelCenterY_Gr  : %d, %d"
                        PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.ModelCenterX_Gr PRN_CAL_VIG_ARG_POST
                        PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.ModelCenterY_Gr PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  ModelCenterX_B  ModelCenterY_B   : %d, %d"
                        PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.ModelCenterX_B PRN_CAL_VIG_ARG_POST
                        PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.ModelCenterY_B PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  ModelCenterX_Gb ModelCenterY_Gb  : %d, %d"
                        PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.ModelCenterX_Gb PRN_CAL_VIG_ARG_POST
                        PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.ModelCenterY_Gb PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  SyncCalInfo[0]                    : %d"
                        PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.SyncCalInfo[0] PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  SyncCalInfo[1]                    : %d"
                        PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.SyncCalInfo[1] PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  SyncCalInfo[2]                    : %d"
                        PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.SyncCalInfo[2] PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  SyncCalInfo[3]                    : %d"
                        PRN_CAL_VIG_ARG_UINT32 pNandTable->Header.SyncCalInfo[3] PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  GainTblR                          : %p"
                        PRN_CAL_VIG_ARG_CPOINT &(pNandTable->GainTblR[0][0]) PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  GainTblGr                         : %p"
                        PRN_CAL_VIG_ARG_CPOINT &(pNandTable->GainTblGr[0][0]) PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  GainTblB                          : %p"
                        PRN_CAL_VIG_ARG_CPOINT &(pNandTable->GainTblB[0][0]) PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API
                    PRN_CAL_VIG_LOG "  GainTblGb                         : %p"
                        PRN_CAL_VIG_ARG_CPOINT &(pNandTable->GainTblGb[0][0]) PRN_CAL_VIG_ARG_POST
                    PRN_CAL_VIG_API

                    /* Update to NAND */
                    RetVal = SvcCalib_DataSave(SVC_CALIB_VIGNETTE_ID);
                    if (RetVal != OK) {
                        PRN_CAL_VIG_LOG "Fail to save vig to nand!" PRN_CAL_VIG_NG
                    }

                    SvcCalib_VigMutexGive(&(SVC_CalibVigCtrl.Mutex));
                }
            }
        }
    }

    return RetVal;
}

static void SvcCalib_VigShellSetU(void)
{
    PRN_CAL_VIG_LOG "  %sset%s                   : set Vignette table manually"
        PRN_CAL_VIG_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_VIG_ARG_POST
    PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "  ------------------------------------------------------" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [TblID]                 : inser table index" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [VinSelectBits]         : the table supported vin select bits" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [SensorSelectBits]      : the table supported sensor select bits" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [Calib StartX]          : calibration geometric StartX              " PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [Calib StartY]          : calibration geometric StartY              " PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [Calib Width]           : calibration geometric Width               " PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [Calib Height]          : calibration geometric Height              " PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [Calib HbSampleDen]     : calibration geometric HSubSample.FactorDen" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [Calib HbSampleNum]     : calibration geometric HSubSample.FactorNum" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [Calib VbSampleDen]     : calibration geometric VSubSample.FactorDen" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [Calib VbSampleNum]     : calibration geometric VSubSample.FactorNum" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [Vig RadialCoarse]      : vignette table parameters" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [Vig RadialCoarseLog]   : vignette table parameters" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [Vig RadialBinsFine]    : vignette table parameters" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [Vig RadialBinsFineLog] : vignette table parameters" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [Vig ModelCenterX_R]    : vignette table parameters" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [Vig ModelCenterX_Gr]   : vignette table parameters" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [Vig ModelCenterX_B]    : vignette table parameters" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [Vig ModelCenterX_Gb]   : vignette table parameters" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [Vig ModelCenterY_R]    : vignette table parameters" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [Vig ModelCenterY_Gr]   : vignette table parameters" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [Vig ModelCenterY_B]    : vignette table parameters" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [Vig ModelCenterY_Gb]   : vignette table parameters" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [Vig SyncCalInfo_1]     : vignette sync info" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [Vig SyncCalInfo_2]     : vignette sync info" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [Vig SyncCalInfo_3]     : vignette sync info" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [Vig SyncCalInfo_4]     : vignette sync info" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [Vig File Path]         : vignette file path" PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "    [DebugMsgOn]            : configure debug msg" PRN_CAL_VIG_API

    PRN_CAL_VIG_LOG " " PRN_CAL_VIG_API
}

static void SvcCalib_VigShellEntryInit(void)
{
    if ((SVC_CalibVigCtrlFlag & SVC_CALIB_VIG_FLG_SHELL_INIT) == 0U) {
        UINT32 ShellNum = 0U;

        AmbaSvcWrap_MisraMemset(CalibVigShellFunc, 0, sizeof(CalibVigShellFunc));

        CalibVigShellFunc[ShellNum] = (SVC_CALIB_VIG_SHELL_FUNC_s) { 1U, "cfg_calib_chan",  SvcCalib_VigShellCfgCalChan, SvcCalib_VigShellCfgCalChanU  }; ShellNum++;
        CalibVigShellFunc[ShellNum] = (SVC_CALIB_VIG_SHELL_FUNC_s) { 1U, "cfg_table_id",    SvcCalib_VigShellCfgTblID,   SvcCalib_VigShellCfgTblIDU    }; ShellNum++;
        CalibVigShellFunc[ShellNum] = (SVC_CALIB_VIG_SHELL_FUNC_s) { 1U, "cfg_dbg_msg",     SvcCalib_VigShellCfgDbgMsg,  SvcCalib_VigShellCfgDbgMsgU   }; ShellNum++;
        CalibVigShellFunc[ShellNum] = (SVC_CALIB_VIG_SHELL_FUNC_s) { 1U, "cfg_sync_table",  SvcCalib_VigShellCfgSyncTbl, SvcCalib_VigShellCfgSyncTblU  }; ShellNum++;
        CalibVigShellFunc[ShellNum] = (SVC_CALIB_VIG_SHELL_FUNC_s) { 1U, "gen_table",       SvcCalib_VigShellGenTbl,     SvcCalib_VigShellGenTblU      }; ShellNum++;
        CalibVigShellFunc[ShellNum] = (SVC_CALIB_VIG_SHELL_FUNC_s) { 1U, "enable",          SvcCalib_VigShellEnable,     SvcCalib_VigShellEnableU      }; ShellNum++;
        CalibVigShellFunc[ShellNum] = (SVC_CALIB_VIG_SHELL_FUNC_s) { 1U, "upd",             SvcCalib_VigShellUpd,        SvcCalib_VigShellUpdU         }; ShellNum++;
        CalibVigShellFunc[ShellNum] = (SVC_CALIB_VIG_SHELL_FUNC_s) { 1U, "set",             SvcCalib_VigShellSet,        SvcCalib_VigShellSetU         };

        SVC_CalibVigCtrlFlag |= SVC_CALIB_VIG_FLG_SHELL_INIT;
    }
}


UINT32 SvcCalib_VigShellFunc(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_NG;

    SvcCalib_VigShellEntryInit();

    if (ArgCount > 0U) {
        UINT32 ShellIdx;
        UINT32 ShellCount = (UINT32)(sizeof(CalibVigShellFunc)) / (UINT32)(sizeof(CalibVigShellFunc[0]));

        for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
            if ((CalibVigShellFunc[ShellIdx].pFunc != NULL) && (CalibVigShellFunc[ShellIdx].Enable > 0U)) {
                if (0 == SvcWrap_strcmp(pArgVector[2U], CalibVigShellFunc[ShellIdx].ShellCmdName)) {
                     if (SVC_OK != (CalibVigShellFunc[ShellIdx].pFunc)(ArgCount, pArgVector)) {
                         if (CalibVigShellFunc[ShellIdx].pUsageFunc != NULL) {
                             (CalibVigShellFunc[ShellIdx].pUsageFunc)();
                         }
                     }

                    RetVal = SVC_OK;
                    break;
                }
            }
        }

        if (RetVal != SVC_OK) {
            SvcCalib_VigShellUsage();
        }
    }

    return RetVal;
}


static void SvcCalib_VigShellUsage(void)
{
    UINT32 ShellIdx;
    UINT32 ShellCount = (UINT32)(sizeof(CalibVigShellFunc)) / (UINT32)(sizeof(CalibVigShellFunc[0]));

    PRN_CAL_VIG_LOG " " PRN_CAL_VIG_API
    PRN_CAL_VIG_LOG "====== %sVIG Command Usage%s ======"
        PRN_CAL_VIG_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_0 PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_VIG_ARG_POST
    PRN_CAL_VIG_API

    for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
        if ((CalibVigShellFunc[ShellIdx].pFunc != NULL) && (CalibVigShellFunc[ShellIdx].Enable > 0U)) {
            if (CalibVigShellFunc[ShellIdx].pUsageFunc == NULL) {
                PRN_CAL_VIG_LOG "  %s"
                    PRN_CAL_VIG_ARG_CSTR   CalibVigShellFunc[ShellIdx].ShellCmdName PRN_CAL_VIG_ARG_POST
                PRN_CAL_VIG_API
            } else {
                (CalibVigShellFunc[ShellIdx].pUsageFunc)();
            }
        }
    }
}


static void SvcCalib_VigCmdHdlrInit(void)
{
    if ((SVC_CalibVigCtrlFlag & SVC_CALIB_VIG_FLG_CMD_INIT) == 0U) {

        AmbaSvcWrap_MisraMemset(SvcCalibVigCmdHdlr, 0, sizeof(SvcCalibVigCmdHdlr));

        SvcCalibVigCmdHdlr[SVC_CALIB_CMD_MEM_QUERY      ] = (SVC_CALIB_VIG_CMD_HDLR_s) {1, SVC_CALIB_CMD_MEM_QUERY,       SvcCalib_VigCmdMemQuery  };
        SvcCalibVigCmdHdlr[SVC_CALIB_CMD_ITEM_CREATE    ] = (SVC_CALIB_VIG_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_CREATE,     SvcCalib_VigCmdCreate    };
        SvcCalibVigCmdHdlr[SVC_CALIB_CMD_ITEM_DATA_INIT ] = (SVC_CALIB_VIG_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_DATA_INIT,  SvcCalib_VigCmdDataInit  };
        SvcCalibVigCmdHdlr[SVC_CALIB_CMD_ITEM_TBL_UPDATE] = (SVC_CALIB_VIG_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_TBL_UPDATE, SvcCalib_VigCmdTblUpdate };
        SvcCalibVigCmdHdlr[SVC_CALIB_CMD_ITEM_TBL_GET   ] = (SVC_CALIB_VIG_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_TBL_GET,    SvcCalib_VigCmdTblDataGet};
        SvcCalibVigCmdHdlr[SVC_CALIB_CMD_ITEM_TBL_SET   ] = (SVC_CALIB_VIG_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_TBL_SET,    SvcCalib_VigCmdTblDataSet};

        SVC_CalibVigCtrlFlag |= SVC_CALIB_VIG_FLG_CMD_INIT;
    }
}

static UINT32 SvcCalib_VigCmdMemQuery(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    if (pParam1 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to query vig memory - input vig table number should not zero. %d" PRN_CAL_VIG_NG
    } else if (pParam2 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to query vig memory - output vig shadow buffer size should not null!" PRN_CAL_VIG_NG
    } else if (pParam3 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to query vig memory - output vig working buffer size should not null!" PRN_CAL_VIG_NG
    } else if (pParam4 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to query vig memory - output vig alg. buffer size should not null!" PRN_CAL_VIG_NG
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

        RetVal = SvcCalib_VigMemQuery(NomOfTable, pShadowBufSize, pWorkingBufSize, pCalcMemSize);
        if (RetVal != SVC_OK) {
            PRN_CAL_VIG_LOG "Fail to query vig memory - query memory fail!" PRN_CAL_VIG_NG
        }

        AmbaMisra_TouchUnused(pParam1);
        AmbaMisra_TouchUnused(pParam2);
        AmbaMisra_TouchUnused(pParam3);
        AmbaMisra_TouchUnused(pParam4);
        AmbaMisra_TouchUnused(pU32Val);
    }

    return RetVal;
}


static UINT32 SvcCalib_VigCmdCreate(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SvcCalib_VigCreate();
    if (RetVal != SVC_OK) {
        PRN_CAL_VIG_LOG "Fail to create vig module fail!" PRN_CAL_VIG_NG
    } else {
        AmbaMisra_TouchUnused(pParam1);
        AmbaMisra_TouchUnused(pParam2);
        AmbaMisra_TouchUnused(pParam3);
        AmbaMisra_TouchUnused(pParam4);
    }

    return RetVal;
}


static UINT32 SvcCalib_VigCmdDataInit(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    if (pParam1 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to init vig - input calib object should not null!" PRN_CAL_VIG_NG
    } else {
        const SVC_CALIB_OBJ_s *pCalObj;

        AmbaMisra_TypeCast(&pCalObj, &pParam1);
        if (pCalObj == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_VIG_LOG "Fail to init vig - invalid calib obj!" PRN_CAL_VIG_NG
        } else {
            RetVal = SvcCalib_VigShadowInit(pCalObj);
            if (RetVal != SVC_OK) {
                PRN_CAL_VIG_LOG "Fail to init vig - shadow initial fail!" PRN_CAL_VIG_NG
            } else {
                SVC_CALIB_CALC_CB_f CbFunc = NULL;
                AmbaMisra_TypeCast(&(CbFunc), &(pParam2));
                SVC_CalibVigCtrl.CalcCbFunc = CbFunc;
            }
        }

        AmbaMisra_TouchUnused(pParam1);
        AmbaMisra_TouchUnused(pParam2);
        AmbaMisra_TouchUnused(pParam3);
        AmbaMisra_TouchUnused(pParam4);
    }

    return RetVal;
}


static UINT32 SvcCalib_VigCmdTblUpdate(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    if (pParam1 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to update vig table - input calib channel should not null!" PRN_CAL_VIG_NG
    } else if (pParam2 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to update vig table - input IK mode cfg should not null!" PRN_CAL_VIG_NG
    } else {
        const SVC_CALIB_CHANNEL_s     *pChan;
        const AMBA_IK_MODE_CFG_s      *pImgMode;

        AmbaMisra_TypeCast(&(pChan), &pParam1);
        AmbaMisra_TypeCast(&(pImgMode), &pParam2);

        RetVal = SvcCalib_VigTableUpdate(pChan, pImgMode);
    }

    AmbaMisra_TouchUnused(pParam1);
    AmbaMisra_TouchUnused(pParam2);
    AmbaMisra_TouchUnused(pParam3);
    AmbaMisra_TouchUnused(pParam4);

    return RetVal;
}

static UINT32 SvcCalib_VigCmdTblDataGet(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;
    UINT32 *pU32Val = NULL;
    SVC_CALIB_TBL_INFO_s *pTblInfo;

    AmbaMisra_TypeCast(&(pU32Val), &(pParam1));
    AmbaMisra_TypeCast(&(pTblInfo), &(pParam2));

    if (pU32Val == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to get table data - input table id should not null!" PRN_CAL_VIG_NG
    } else {
        RetVal = SvcCalib_VigTableDataGet(*pU32Val, pTblInfo);
    }

    AmbaMisra_TouchUnused(pParam1);
    AmbaMisra_TouchUnused(pParam2);
    AmbaMisra_TouchUnused(pParam3);
    AmbaMisra_TouchUnused(pParam4);
    AmbaMisra_TouchUnused(pU32Val);

    return RetVal;
}


static UINT32 SvcCalib_VigCmdTblDataSet(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;
    UINT32 *pU32Val = NULL;
    SVC_CALIB_TBL_INFO_s *pTblInfo;

    AmbaMisra_TypeCast(&(pU32Val), &(pParam1));
    AmbaMisra_TypeCast(&(pTblInfo), &(pParam2));

    if (pU32Val == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to set table data - input table id should not null!" PRN_CAL_VIG_NG
    } else {
        RetVal = SvcCalib_VigTableDataSet(*pU32Val, pTblInfo);
    }

    AmbaMisra_TouchUnused(pParam1);
    AmbaMisra_TouchUnused(pParam2);
    AmbaMisra_TouchUnused(pParam3);
    AmbaMisra_TouchUnused(pParam4);
    AmbaMisra_TouchUnused(pU32Val);

    return RetVal;

}

/**
 * calib vig command entry
 *
 * @param [in] CmdID  the bpc command id
 * @param [in] Param1 no.01 parameter
 * @param [in] Param2 no.02 parameter
 * @param [in] Param3 no.03 parameter
 * @param [in] Param4 no.04 parameter
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcCalib_VigCmdFunc(UINT32 CmdID, void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    SvcCalib_VigCmdHdlrInit();

    if (CmdID >= SVC_CALIB_CMD_NUM) {
        RetVal = SVC_NG;
        PRN_CAL_VIG_LOG "Fail to handler cmd - invalid command id(%d)"
            PRN_CAL_VIG_ARG_UINT32 CmdID PRN_CAL_VIG_ARG_POST
        PRN_CAL_VIG_NG
    } else {
        const UINT32 HdlrCount = (UINT32)(sizeof(SvcCalibVigCmdHdlr)) / ((UINT32)sizeof(SvcCalibVigCmdHdlr[0]));
        UINT32 HdlrIdx;

        for (HdlrIdx = 0U; HdlrIdx < HdlrCount; HdlrIdx ++) {
            if ((SvcCalibVigCmdHdlr[HdlrIdx].Enable > 0U) &&
                (SvcCalibVigCmdHdlr[HdlrIdx].CmdID == CmdID) &&
                (SvcCalibVigCmdHdlr[HdlrIdx].pHandler != NULL)) {
                RetVal = (SvcCalibVigCmdHdlr[HdlrIdx].pHandler)(pParam1, pParam2, pParam3, pParam4);
            }
        }
    }

    return RetVal;
}


