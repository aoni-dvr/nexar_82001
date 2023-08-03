/**
*  @file SvcCalibLdc.c
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
*  @details C file for Calibration Lens Distortion
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
#include "AmbaCalib_LDCIF.h"

#include "AmbaCT_TextHdlr.h"
#include "AmbaCT_LdcTunerIF.h"

#include "SvcWrap.h"
#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcCalibMgr.h"

#define SVC_CALIB_LDC_FLG_INIT           (0x1U)
#define SVC_CALIB_LDC_FLG_SHADOW_INIT    (0x2U)
#define SVC_CALIB_LDC_FLG_CALC_INIT      (0x4U)
#define SVC_CALIB_LDC_FLG_CALC_VIN_SEN   (0x8U)
#define SVC_CALIB_LDC_FLG_SHELL_INIT     (0x100U)
#define SVC_CALIB_LDC_FLG_CMD_INIT       (0x200U)
/* Reserve 0x10000/0x20000/0x40000/0x80000 for SvcCalibMgr.h */

#define SVC_CALIB_LDC_NAME               ("SvcCalib_Ldc")
#define SVC_CALIB_LDC_NAND_HEADER_SIZE   (0x200U)
#define SVC_CALIB_LDC_VERSION            (0x20180401U)
#define SVC_CALIB_LDC_IK_2_STAGE         (0x1U)
#define SVC_CALIB_LDC_MAX_TBL_LEN        ((UINT32)MAX_WARP_TBL_LEN )
#define SVC_CALIB_LDC_REMAP_BUF_SIZE     ( 393984U )
#define SVC_CALIB_LDC_MAX_SELECT_BIT     (32U)

typedef struct {
    UINT32 Enable;
    UINT32 NumOfTable;
    UINT32 DebugMsgOn;
    UINT32 TableSelectBits;
    UINT32 Reserved[(SVC_CALIB_LDC_NAND_HEADER_SIZE / 4U) - 4U];
} SVC_CALIB_LDC_NAND_HEADER_s;

typedef struct {
    UINT32                     Version;
    UINT32                     HorGridNum;
    UINT32                     VerGridNum;
    UINT32                     TileWidthExp;
    UINT32                     TileHeightExp;
    SVC_CALIB_VIN_SENSOR_GEO_s VinSensorGeo;
    UINT32                     TwoStageDone;
    UINT32                     VinSelectBits;
    UINT32                     SensorSelectBits;
    UINT32                     FmtSelectBits;
    UINT32                     FovSelectBits;
} SVC_CALIB_LDC_TABLE_HEADER_s;

typedef struct {
    SVC_CALIB_LDC_TABLE_HEADER_s Header;
    AMBA_IK_GRID_POINT_s         LdcVector[SVC_CALIB_LDC_MAX_TBL_LEN];
} SVC_CALIB_LDC_NAND_TABLE_s;

typedef struct {
    UINT32                        VinSelectBits;
    UINT32                        SensorSelectBits;
    UINT32                        FmtSelectBits;
    UINT32                        FovSelectBits;
    AMBA_IK_WARP_INFO_s           WarpInfo;
    AMBA_IK_GRID_POINT_s          LdcVector[SVC_CALIB_LDC_MAX_TBL_LEN];
} SVC_CALIB_LDC_TABLE_s;

typedef struct {
    UINT32                          SaveBinOn;
    UINT32                          TableIdx;
} SVC_CALIB_LDC_CALCULATE_CTRL_s;

#define SVC_CALIB_LDC_MEM_CALC          (0x0U)
#define SVC_CALIB_LDC_MEM_CALC_CTUNER   (0x1U)
#define SVC_CALIB_LDC_MEM_CALC_REMAP    (0x2U)
#define SVC_CALIB_LDC_MEM_NUM           (0x3U)
typedef struct {
    UINT32  Enable;
    UINT8  *pBuf;
    UINT32  BufSize;
} SVC_CALIB_LDC_MEM_CTRL_s;

typedef struct {
    char                           Name[16];
    AMBA_KAL_MUTEX_t               Mutex;
    SVC_CALIB_LDC_NAND_HEADER_s   *pNandHeader;
    SVC_CALIB_LDC_NAND_TABLE_s    *pNandTable;
    UINT32                         NumOfLdcTable;
    SVC_CALIB_LDC_TABLE_s         *pLdcTable;

    SVC_CALIB_LDC_MEM_CTRL_s       MemCtrl[SVC_CALIB_LDC_MEM_NUM];
    SVC_CALIB_LDC_CALCULATE_CTRL_s CalcLdcCtrl;
    SVC_CALIB_CALC_CB_f            CalcCbFunc;
    SVC_CALIB_CHANNEL_s            CalibChan;
    UINT32                         CalUpdCtrl;
    UINT32                         EnableDebugMsg;
} SVC_CALIB_LDC_CTRL_s;

static UINT32                  SvcCalib_LdcMutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut);
static void                    SvcCalib_LdcMutexGive(AMBA_KAL_MUTEX_t *pMutex);
static void                    SvcCalib_LdcErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode);

static UINT32                  SvcCalib_LdcMemQuery(UINT32 MaxTableNum, UINT32 *pShadowSize, UINT32 *pWorkMemSize, UINT32 *pCalcMemSize);
static UINT32                  SvcCalib_LdcMemLock(void);
static void                    SvcCalib_LdcMemUnLock(void);
static UINT32                  SvcCalib_LdcVinSensorIDCheck(UINT32 VinID, UINT32 SensorID);
static UINT32                  SvcCalib_LdcFmtFovIDCheck(UINT32 FmtID, UINT32 FovID);
static UINT32                  SvcCalib_LdcCreate(void);
static UINT32                  SvcCalib_LdcShadowInit(const SVC_CALIB_OBJ_s *pCalObj);
static UINT32                  SvcCalib_LdcTableUpdate(const SVC_CALIB_CHANNEL_s *pCalibChan, const AMBA_IK_MODE_CFG_s *pImgMode);
static UINT32                  SvcCalib_LdcTableCfg(UINT32 NandTableID, UINT32 TableID);
static UINT32                  SvcCalib_LdcTableSearch(UINT32 ID_1, UINT32 ID_2, UINT32 *pTblIdx);
static UINT32                  SvcCalib_LdcNandTableSearch(UINT32 ID_1, UINT32 ID_2, UINT32 *pTblIdx);
static SVC_CALIB_LDC_TABLE_s * SvcCalib_LdcTableGet(const SVC_CALIB_CHANNEL_s *pCalibChan);
static UINT32                  SvcCalib_LdcTableGenTbl(const SVC_CALIB_CHANNEL_s *pCalibChan, const char *pScriptPath);
static UINT32                  SvcCalib_LdcTableDataSet(UINT32 TblID, SVC_CALIB_TBL_INFO_s *pTblInfo);
static UINT32                  SvcCalib_LdcTableDataGet(UINT32 TblID, SVC_CALIB_TBL_INFO_s *pTblInfo);

// Private function definition for calibration command
typedef UINT32 (*SVC_CALIB_LDC_CMD_FUNC_f)(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
typedef struct {
     UINT32                   Enable;
     UINT32                   CmdID;
     SVC_CALIB_LDC_CMD_FUNC_f pHandler;
} SVC_CALIB_LDC_CMD_HDLR_s;

static void   SvcCalib_LdcCmdHdlrInit(void);
static UINT32 SvcCalib_LdcCmdMemQuery(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_LdcCmdCreate(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_LdcCmdDataInit(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_LdcCmdTblCfg(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_LdcCmdTblUpdate(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_LdcCmdTblDataSet(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_LdcCmdTblDataGet(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
//UINT32 SvcCalib_LdcCmdFunc(UINT32 CmdID, UINT32 Param1, UINT32 Param2, UINT32 Param3, UINT32 Param4);

// Private function definition for shell command.
typedef UINT32 (*SVC_CALIB_LDC_SHELL_FUNC_f)(UINT32 ArgCount, char * const *pArgVector);
typedef void   (*SVC_CALIB_LDC_SHELL_USAGE_f)(void);

typedef struct {
    UINT32                      Enable;
    char                        ShellCmdName[32];
    SVC_CALIB_LDC_SHELL_FUNC_f  pFunc;
    SVC_CALIB_LDC_SHELL_USAGE_f pUsageFunc;
} SVC_CALIB_LDC_SHELL_FUNC_s;

static void   SvcCalib_LdcShellStrToU32(const char *pString, UINT32 *pValue);
static UINT32 SvcCalib_LdcShellCfgCalChn(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_LdcShellCfgCalChnU(void);
static UINT32 SvcCalib_LdcShellCfgTblID(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_LdcShellCfgTblIDU(void);
static UINT32 SvcCalib_LdcShellCfgDbgMsg(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_LdcShellCfgDbgMsgU(void);
static UINT32 SvcCalib_LdcShellGenTbl(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_LdcShellGenTblU(void);
static UINT32 SvcCalib_LdcShellEnable(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_LdcShellEnableU(void);
static UINT32 SvcCalib_LdcShellUpd(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_LdcShellUpdU(void);
static UINT32 SvcCalib_LdcShellSet(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_LdcShellSetU(void);
static void   SvcCalib_LdcShellEntryInit(void);
static void   SvcCalib_LdcShellUsage(void);

#define SVC_CALIB_LDC_SHELL_CMD_NUM  (7U)
static SVC_CALIB_LDC_SHELL_FUNC_s SVC_CalibLdcShellFunc[SVC_CALIB_LDC_SHELL_CMD_NUM] GNU_SECTION_NOZEROINIT;
static SVC_CALIB_LDC_CMD_HDLR_s   SVC_CalibLdcCmdHdlr[SVC_CALIB_CMD_NUM] GNU_SECTION_NOZEROINIT;
static SVC_CALIB_LDC_CTRL_s       SVC_CalibLdcCtrl GNU_SECTION_NOZEROINIT;
static UINT32 SVC_CalibLdcCtrlFlag = SVC_LOG_CAL_DEF_FLG;

#define SVC_LOG_LDC "LDC"
#define PRN_CAL_LDC_LOG        { SVC_WRAP_PRINT_s CalibLdcPrint; AmbaSvcWrap_MisraMemset(&(CalibLdcPrint), 0, sizeof(CalibLdcPrint)); CalibLdcPrint.Argc --; CalibLdcPrint.pStrFmt =
#define PRN_CAL_LDC_ARG_UINT32 ; CalibLdcPrint.Argc ++; CalibLdcPrint.Argv[CalibLdcPrint.Argc].Uint64   = (UINT64)((
#define PRN_CAL_LDC_ARG_CSTR   ; CalibLdcPrint.Argc ++; CalibLdcPrint.Argv[CalibLdcPrint.Argc].pCStr    = ((
#define PRN_CAL_LDC_ARG_CPOINT ; CalibLdcPrint.Argc ++; CalibLdcPrint.Argv[CalibLdcPrint.Argc].pPointer = ((
#define PRN_CAL_LDC_ARG_POST   ))
#define PRN_CAL_LDC_OK         ; CalibLdcPrint.Argc ++; SvcCalib_LdcPrintLog(SVC_LOG_CAL_OK , &(CalibLdcPrint)); }
#define PRN_CAL_LDC_NG         ; CalibLdcPrint.Argc ++; SvcCalib_LdcPrintLog(SVC_LOG_CAL_NG , &(CalibLdcPrint)); }
#define PRN_CAL_LDC_API        ; CalibLdcPrint.Argc ++; SvcCalib_LdcPrintLog(SVC_LOG_CAL_API, &(CalibLdcPrint)); }
#define PRN_CAL_LDC_DBG        ; CalibLdcPrint.Argc ++; SvcCalib_LdcPrintLog(SVC_LOG_CAL_DBG, &(CalibLdcPrint)); }
#define PRN_CAL_LDC_ERR_HDLR   SvcCalib_LdcErrHdlr(__func__, __LINE__, PRetVal);

static void SvcCalib_LdcPrintLog(UINT32 LogLevel, SVC_WRAP_PRINT_s *pPrint)
{
    if (pPrint != NULL) {
        if ((SVC_CalibLdcCtrlFlag & LogLevel) > 0U) {

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

            SvcWrap_Print(SVC_LOG_LDC, pPrint);
        }
    }
}

static UINT32 SvcCalib_LdcMutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut)
{
    return AmbaKAL_MutexTake(pMutex, TimeOut);
}

static void SvcCalib_LdcMutexGive(AMBA_KAL_MUTEX_t *pMutex)
{
    UINT32 ErrCode = AmbaKAL_MutexGive(pMutex);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }
}

static void SvcCalib_LdcErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode)
{
    if (pCaller != NULL) {
        if (ErrCode != 0U) {
            PRN_CAL_LDC_LOG "Catch ErrCode(0x%08x) @ %s, %d"
                PRN_CAL_LDC_ARG_UINT32 ErrCode  PRN_CAL_LDC_ARG_POST
                PRN_CAL_LDC_ARG_CSTR   pCaller  PRN_CAL_LDC_ARG_POST
                PRN_CAL_LDC_ARG_UINT32 CodeLine PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_NG
        }
    }
}

static UINT32 SvcCalib_LdcMemQuery(UINT32 MaxTableNum, UINT32 *pShadowSize, UINT32 *pWorkMemSize, UINT32 *pCalcMemSize)
{
    UINT32 RetVal = SVC_OK;

    UINT32 DramShadowSize = 0, WorkingMemSize = 0, CalcMemSize = 0;
    UINT32 DramShadowHeaderSize = (UINT32)(sizeof(SVC_CALIB_LDC_NAND_HEADER_s));
    UINT32 DramShadowTableSize  = (UINT32)(sizeof(SVC_CALIB_LDC_NAND_TABLE_s));
    UINT32 DramShadowDataSize   = DramShadowTableSize * MaxTableNum;
    UINT32 LdcTableSize         = (UINT32)(sizeof(SVC_CALIB_LDC_TABLE_s));
    UINT32 TotalLdcTableSize    = LdcTableSize * MaxTableNum;
    SIZE_t CalcMemAlgoSize      = 0;
    SIZE_t CalcMemCtnSize       = 0;

    if (CAL_OK != AmbaCal_LdcGetBufSize(&CalcMemAlgoSize)) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to query ldc module memory - query ldc algo working memory size fail!" PRN_CAL_LDC_NG
    } else if (0U != AmbaCT_GetBufSize(AMBA_CT_TYPE_LDC, &CalcMemCtnSize)) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to query ldc module memory - query ldc ctuner working memory size fail!" PRN_CAL_LDC_NG
    } else {
        DramShadowSize  = DramShadowHeaderSize;
        DramShadowSize += DramShadowDataSize;

        WorkingMemSize  = TotalLdcTableSize;

        CalcMemSize    += (UINT32)CalcMemAlgoSize;
        CalcMemSize    += (UINT32)CalcMemCtnSize;
        CalcMemSize    += SVC_CALIB_LDC_REMAP_BUF_SIZE;

        PRN_CAL_LDC_LOG " " PRN_CAL_LDC_DBG
        PRN_CAL_LDC_LOG "====== Calib LDC Memory Query ======" PRN_CAL_LDC_DBG
        PRN_CAL_LDC_LOG "  MaxTableNum    : %d"
            PRN_CAL_LDC_ARG_UINT32 MaxTableNum PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_DBG
        PRN_CAL_LDC_LOG "  DramShadowSize : 0x%X"
            PRN_CAL_LDC_ARG_UINT32 DramShadowSize PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_DBG
        PRN_CAL_LDC_LOG "    DramShadowHeaderSize : 0x%X"
            PRN_CAL_LDC_ARG_UINT32 DramShadowHeaderSize PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_DBG
        PRN_CAL_LDC_LOG "    DramShadowDataSize   : 0x%X ( 0x%X x %d )"
            PRN_CAL_LDC_ARG_UINT32 DramShadowDataSize  PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_ARG_UINT32 DramShadowTableSize PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_ARG_UINT32 MaxTableNum         PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_DBG
        PRN_CAL_LDC_LOG "  WorkingMemSize : 0x%X"
            PRN_CAL_LDC_ARG_UINT32 WorkingMemSize PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_DBG
        PRN_CAL_LDC_LOG "    TotalLdcTableSize : 0x%X ( 0x%X x %d )"
            PRN_CAL_LDC_ARG_UINT32 TotalLdcTableSize PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_ARG_UINT32 LdcTableSize      PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_ARG_UINT32 MaxTableNum       PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_DBG

        PRN_CAL_LDC_LOG "  CalcMemSize : 0x%X"
            PRN_CAL_LDC_ARG_UINT32 CalcMemSize PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_DBG
        PRN_CAL_LDC_LOG "    CalcMemAlgoSize    : 0x%X"
            PRN_CAL_LDC_ARG_UINT32 CalcMemAlgoSize PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_DBG
        PRN_CAL_LDC_LOG "    CalcMemCtnSize     : 0x%X"
            PRN_CAL_LDC_ARG_UINT32 CalcMemCtnSize PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_DBG
        PRN_CAL_LDC_LOG "    CalcMemRemapSize   : 0x%X"
            PRN_CAL_LDC_ARG_UINT32 SVC_CALIB_LDC_REMAP_BUF_SIZE PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_DBG

        if (pShadowSize != NULL) {
            *pShadowSize  = DramShadowSize;
        }
        PRN_CAL_LDC_LOG "Query the calib ldc dram shadow size 0x%X ( table num %d )"
            PRN_CAL_LDC_ARG_UINT32 DramShadowSize PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_ARG_UINT32 MaxTableNum    PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_DBG

        if (pWorkMemSize != NULL) {
            *pWorkMemSize = WorkingMemSize;
        }
        PRN_CAL_LDC_LOG "Query the calib ldc working memory size 0x%X ( table num %d )"
            PRN_CAL_LDC_ARG_UINT32 WorkingMemSize PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_ARG_UINT32 MaxTableNum    PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_DBG

        if (pCalcMemSize != NULL) {
            *pCalcMemSize = CalcMemSize;
        }
        PRN_CAL_LDC_LOG "Query the calib ldc algo. memory size 0x%X"
            PRN_CAL_LDC_ARG_UINT32 CalcMemSize PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_DBG
    }

    return RetVal;
}


static UINT32 SvcCalib_LdcMemLock(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    SIZE_t CalcMemAlgoSize = 0U;
    SIZE_t CalcMemCtnSize = 0U;
    UINT32 CalcMemRemapSize = SVC_CALIB_LDC_REMAP_BUF_SIZE;
    UINT8 *pCalcMem = NULL;
    UINT32 CalcMemSize = 0U;

    /* Configure the ldc algo memory */
    if (CAL_OK != AmbaCal_LdcGetBufSize(&CalcMemAlgoSize)) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to cfg ldc table memory - query ldc algo working memory size fail!" PRN_CAL_LDC_NG
    } else if (0U != AmbaCT_GetBufSize(AMBA_CT_TYPE_LDC, &CalcMemCtnSize)) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to cfg ldc table memory - query ldc ctuner working memory size fail!" PRN_CAL_LDC_NG
    } else {

        CalcMemSize = (UINT32)CalcMemAlgoSize + (UINT32)CalcMemCtnSize + CalcMemRemapSize;

        if (0U != SvcCalib_ItemCalcMemGet(SVC_CALIB_LDC_ID, 1U, &pCalcMem, &CalcMemSize)) {
            RetVal = SVC_NG;
            PRN_CAL_LDC_LOG "Fail to cfg ldc table memory - query ldc ctuner working memory size fail!" PRN_CAL_LDC_NG
        } else {

            if (CalcMemSize < ((UINT32)CalcMemAlgoSize + (UINT32)CalcMemCtnSize + CalcMemRemapSize)) {
                RetVal = SVC_NG;
                PRN_CAL_LDC_LOG "Fail to cfg ldc table memory - calib calculation memory is not enough to service it!" PRN_CAL_LDC_NG
            } else if (pCalcMem == NULL) {
                RetVal = SVC_NG;
                PRN_CAL_LDC_LOG "Fail to cfg ldc table memory - invalid calib calculation memory!" PRN_CAL_LDC_NG
            } else {
                AmbaSvcWrap_MisraMemset(&(SVC_CalibLdcCtrl.MemCtrl[SVC_CALIB_LDC_MEM_CALC]), 0, sizeof(SVC_CALIB_LDC_MEM_CTRL_s));
                SVC_CalibLdcCtrl.MemCtrl[SVC_CALIB_LDC_MEM_CALC].BufSize = (UINT32)CalcMemAlgoSize;
                SVC_CalibLdcCtrl.MemCtrl[SVC_CALIB_LDC_MEM_CALC].pBuf    = pCalcMem;
                SVC_CalibLdcCtrl.MemCtrl[SVC_CALIB_LDC_MEM_CALC].Enable  = 1U;
                pCalcMem = &(pCalcMem[(UINT32)CalcMemAlgoSize]);

                AmbaSvcWrap_MisraMemset(&(SVC_CalibLdcCtrl.MemCtrl[SVC_CALIB_LDC_MEM_CALC_CTUNER]), 0, sizeof(SVC_CALIB_LDC_MEM_CTRL_s));
                SVC_CalibLdcCtrl.MemCtrl[SVC_CALIB_LDC_MEM_CALC_CTUNER].BufSize = (UINT32)CalcMemCtnSize;
                SVC_CalibLdcCtrl.MemCtrl[SVC_CALIB_LDC_MEM_CALC_CTUNER].pBuf    = pCalcMem;
                SVC_CalibLdcCtrl.MemCtrl[SVC_CALIB_LDC_MEM_CALC_CTUNER].Enable  = 1U;
                pCalcMem = &(pCalcMem[(UINT32)CalcMemCtnSize]);

                AmbaSvcWrap_MisraMemset(&(SVC_CalibLdcCtrl.MemCtrl[SVC_CALIB_LDC_MEM_CALC_REMAP]), 0, sizeof(SVC_CALIB_LDC_MEM_CTRL_s));
                SVC_CalibLdcCtrl.MemCtrl[SVC_CALIB_LDC_MEM_CALC_REMAP].BufSize = CalcMemRemapSize;
                SVC_CalibLdcCtrl.MemCtrl[SVC_CALIB_LDC_MEM_CALC_REMAP].pBuf    = pCalcMem;
                SVC_CalibLdcCtrl.MemCtrl[SVC_CALIB_LDC_MEM_CALC_REMAP].Enable  = 1U;
            }

            if (RetVal != SVC_OK) {
                // Free calib calculation memory
                PRetVal = SvcCalib_ItemCalcMemFree(SVC_CALIB_LDC_ID); PRN_CAL_LDC_ERR_HDLR
            }
        }
    }

    return RetVal;
}


static void SvcCalib_LdcMemUnLock(void)
{
    // Reset the memory control setting
    AmbaSvcWrap_MisraMemset(SVC_CalibLdcCtrl.MemCtrl, 0, sizeof(SVC_CalibLdcCtrl.MemCtrl));

    // Free the calculation memory
    if (SVC_OK != SvcCalib_ItemCalcMemFree(SVC_CALIB_LDC_ID)) {
        PRN_CAL_LDC_LOG "Fail to un-lock ldc calc memory - init calculation memory control fail!" PRN_CAL_LDC_NG
    } else {
        PRN_CAL_LDC_LOG "Successful to un-lock ldc calc memory!" PRN_CAL_LDC_OK
    }
}


static UINT32 SvcCalib_LdcVinSensorIDCheck(UINT32 VinID, UINT32 SensorID)
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

static UINT32 SvcCalib_LdcFmtFovIDCheck(UINT32 FmtID, UINT32 FovID)
{
    UINT32 RetVal = SVC_OK;

    if (FmtID >= SVC_CALIB_LDC_MAX_SELECT_BIT) {
        RetVal = SVC_NG;
    }

    if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
        RetVal = SVC_NG;
    }

    return RetVal;
}

static UINT32 SvcCalib_LdcCreate(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibLdcCtrlFlag & SVC_CALIB_LDC_FLG_INIT) > 0U) {
        RetVal = SVC_OK;
        PRN_CAL_LDC_LOG "Calibration LDC module has been created!" PRN_CAL_LDC_DBG
    } else {
        // Reset the ldc control module
        AmbaSvcWrap_MisraMemset(&SVC_CalibLdcCtrl, 0, sizeof(SVC_CalibLdcCtrl));
        SVC_CalibLdcCtrl.CalUpdCtrl = SVC_CALIB_LDC_UPD_BY_VIN_SS;

        // Configure the name
        SvcWrap_strcpy(SVC_CalibLdcCtrl.Name, sizeof(SVC_CalibLdcCtrl.Name), SVC_CALIB_LDC_NAME);

        // Create the mutex
        RetVal = AmbaKAL_MutexCreate(&(SVC_CalibLdcCtrl.Mutex), SVC_CalibLdcCtrl.Name);
        if (RetVal != SVC_OK) {
            PRN_CAL_LDC_LOG "Fail to create ldc - create mutex fail!" PRN_CAL_LDC_NG
        } else {
            PRetVal = (SVC_CalibLdcCtrlFlag & ( SVC_CALIB_LDC_FLG_SHELL_INIT |
                                                SVC_CALIB_LDC_FLG_CMD_INIT   |
                                                SVC_LOG_CAL_DEF_FLG          |
                                                SVC_LOG_CAL_DBG));
            SVC_CalibLdcCtrlFlag  = PRetVal;
            SVC_CalibLdcCtrlFlag |= SVC_CALIB_LDC_FLG_INIT;
            PRN_CAL_LDC_LOG "Successful to create ldc!" PRN_CAL_LDC_OK
        }
    }

    return RetVal;
}


static UINT32 SvcCalib_LdcShadowInit(const SVC_CALIB_OBJ_s *pCalObj)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibLdcCtrlFlag & SVC_CALIB_LDC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to initial ldc dram shodow - create ldc first!" PRN_CAL_LDC_NG
    } else if (pCalObj == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to initial ldc dram shodow - invalid calib object!" PRN_CAL_LDC_NG
    } else if (pCalObj->Enable == 0U) {
        RetVal = 99;
        PRN_CAL_LDC_LOG "Disable ldc dram shadow!!" PRN_CAL_LDC_API
    } else {
        UINT32 DramShadowSize = 0, WorkingMemSize = 0;
        const SVC_CALIB_LDC_NAND_HEADER_s *pHeader;

        AmbaMisra_TypeCast(&(pHeader), &(pCalObj->pShadowBuf));

        RetVal = SvcCalib_LdcMemQuery(pCalObj->NumOfTable, &DramShadowSize, &WorkingMemSize, NULL);
        if (RetVal != SVC_OK) {
            PRN_CAL_LDC_LOG "Fail to initial ldc dram shadow - query memory fail!" PRN_CAL_LDC_NG
        } else if ((pCalObj->pShadowBuf == NULL) || (pHeader == NULL)) {
            RetVal = SVC_NG;
            PRN_CAL_LDC_LOG "Fail to initial ldc dram shadow - dram shadow buffer should not null!" PRN_CAL_LDC_NG
        } else if (pCalObj->ShadowBufSize < DramShadowSize) {
            RetVal = SVC_NG;
            PRN_CAL_LDC_LOG "Fail to initial ldc dram shadow - prepare dram shadow size is too small!" PRN_CAL_LDC_NG
        } else if (pCalObj->pWorkingBuf == NULL) {
                RetVal = SVC_NG;
                PRN_CAL_LDC_LOG "Fail to initial ldc dram shadow - working buffer should not null!" PRN_CAL_LDC_NG
        } else if (pCalObj->WorkingBufSize < WorkingMemSize) {
            RetVal = SVC_NG;
            PRN_CAL_LDC_LOG "Fail to initial ldc dram shadow - working buffer size is too small!" PRN_CAL_LDC_NG
        } else if (pHeader->NumOfTable > pCalObj->NumOfTable) {
            RetVal = SVC_NG;
            PRN_CAL_LDC_LOG "Fail to initial ldc dram shadow - calib nand table count(%d) > max ldc dram shadow table count(%d)"
                PRN_CAL_LDC_ARG_UINT32 pHeader->NumOfTable PRN_CAL_LDC_ARG_POST
                PRN_CAL_LDC_ARG_UINT32 pCalObj->NumOfTable PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_NG
        } else if (KAL_ERR_NONE != SvcCalib_LdcMutexTake(&(SVC_CalibLdcCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
            RetVal = SVC_NG;
            PRN_CAL_LDC_LOG "Fail to initial ldc dram shadow - take mutex fail!" PRN_CAL_LDC_NG
        } else {
            UINT8 *pNandHeaderBuf    = pCalObj->pShadowBuf;
            UINT32 NandHeaderSize    = (UINT32)(sizeof(SVC_CALIB_LDC_NAND_HEADER_s));
            UINT8 *pNandTableBuf     = &(pNandHeaderBuf[NandHeaderSize]);
            UINT32 TotalLdcTableSize = (UINT32)(sizeof(SVC_CALIB_LDC_TABLE_s)) * pCalObj->NumOfTable;

            PRN_CAL_LDC_LOG "====== Shadow initialized Start ======" PRN_CAL_LDC_DBG
            PRN_CAL_LDC_LOG "  DramShadow Addr  : %p 0x%X"
                PRN_CAL_LDC_ARG_CPOINT pCalObj->pShadowBuf    PRN_CAL_LDC_ARG_POST
                PRN_CAL_LDC_ARG_UINT32 pCalObj->ShadowBufSize PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_DBG
            PRN_CAL_LDC_LOG "    Nand Header Addr : %p 0x%X"
                PRN_CAL_LDC_ARG_CPOINT pNandHeaderBuf PRN_CAL_LDC_ARG_POST
                PRN_CAL_LDC_ARG_UINT32 NandHeaderSize PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_DBG
            PRN_CAL_LDC_LOG "    Nand Table Addr  : %p"
                PRN_CAL_LDC_ARG_CPOINT pNandTableBuf PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_DBG
            PRN_CAL_LDC_LOG "  Working Mem Addr : %p 0x%X"
                PRN_CAL_LDC_ARG_CPOINT pCalObj->pWorkingBuf    PRN_CAL_LDC_ARG_POST
                PRN_CAL_LDC_ARG_UINT32 pCalObj->WorkingBufSize PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_DBG
            PRN_CAL_LDC_LOG "    Ldc Table Addr   : %p 0x%08X"
                PRN_CAL_LDC_ARG_CPOINT pCalObj->pWorkingBuf PRN_CAL_LDC_ARG_POST
                PRN_CAL_LDC_ARG_UINT32 TotalLdcTableSize    PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_DBG

            // Configure nand header
            AmbaMisra_TypeCast(&(SVC_CalibLdcCtrl.pNandHeader), &(pNandHeaderBuf));
            // Configure nand table
            AmbaMisra_TypeCast(&(SVC_CalibLdcCtrl.pNandTable),  &(pNandTableBuf)); AmbaMisra_TouchUnused(pNandTableBuf);
            // Configure working ldc table
            AmbaMisra_TypeCast(&(SVC_CalibLdcCtrl.pLdcTable),  &(pCalObj->pWorkingBuf));
            if (SVC_CalibLdcCtrl.pNandHeader == NULL) {
                RetVal = SVC_NG;
                PRN_CAL_LDC_LOG "Fail to initial ldc dram shadow - nand header should not null!" PRN_CAL_LDC_NG
            } else if (SVC_CalibLdcCtrl.pNandTable == NULL) {
                RetVal = SVC_NG;
                PRN_CAL_LDC_LOG "Fail to initial ldc dram shadow - nand table should not null!" PRN_CAL_LDC_NG
            } else if (SVC_CalibLdcCtrl.pLdcTable == NULL) {
                RetVal = SVC_NG;
                PRN_CAL_LDC_LOG "Fail to initial ldc dram shadow - ldc table should not null!" PRN_CAL_LDC_NG
            } else {
                SVC_CalibLdcCtrl.NumOfLdcTable = pCalObj->NumOfTable;
                AmbaSvcWrap_MisraMemset(SVC_CalibLdcCtrl.pLdcTable, 0, TotalLdcTableSize);

                if (SVC_CalibLdcCtrl.pNandHeader->DebugMsgOn > 0U) {
                    SVC_CalibLdcCtrl.EnableDebugMsg |= SVC_CalibLdcCtrl.pNandHeader->DebugMsgOn;
                }

                if (SVC_CalibLdcCtrl.EnableDebugMsg > 0U) {
                    UINT32 Idx;

                    PRN_CAL_LDC_LOG " " PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "---- Ldc Nand Info %d ----"
                        PRN_CAL_LDC_ARG_UINT32 SVC_CalibLdcCtrl.pNandHeader->Reserved[0] PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  Enable         : %d"
                        PRN_CAL_LDC_ARG_UINT32 SVC_CalibLdcCtrl.pNandHeader->Enable PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  NumOfTable     : %d"
                        PRN_CAL_LDC_ARG_UINT32 SVC_CalibLdcCtrl.pNandHeader->NumOfTable PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  DebugMsgOn     : %d"
                        PRN_CAL_LDC_ARG_UINT32 SVC_CalibLdcCtrl.pNandHeader->DebugMsgOn PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  TblSelectButs  : 0x%X"
                        PRN_CAL_LDC_ARG_UINT32 SVC_CalibLdcCtrl.pNandHeader->TableSelectBits PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API

                    {
                        const SVC_CALIB_LDC_NAND_TABLE_s *pNandTable;

                        for (Idx = 0U; Idx < pCalObj->NumOfTable; Idx ++) {
                            pNandTable = &(SVC_CalibLdcCtrl.pNandTable[Idx]);

                            PRN_CAL_LDC_LOG "  ---- Nand Table[%d] %p -----"
                                PRN_CAL_LDC_ARG_UINT32 Idx        PRN_CAL_LDC_ARG_POST
                                PRN_CAL_LDC_ARG_CPOINT pNandTable PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "    VinSelectBits       : 0x%X"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.VinSelectBits PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "    SensorSelectBits    : 0x%X"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.SensorSelectBits PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "    FmtSelectBits       : 0x%X"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.FmtSelectBits PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "    FovSelectBits       : 0x%X"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.FovSelectBits PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "    Version             : 0x%X"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.Version PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "    HorGridNum          : %d"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.HorGridNum PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "    VerGridNum          : %d"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.VerGridNum PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "    TileWidthExp        : %d"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.TileWidthExp PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "    TileHeightExp       : %d"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.TileHeightExp PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "    TwoStageDone        : %d"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.TwoStageDone PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "    VinSensorGeo.StartX : %d"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.VinSensorGeo.StartX PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "    VinSensorGeo.StartY : %d"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.VinSensorGeo.StartY PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "    VinSensorGeo.Width  : %d"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.VinSensorGeo.Width PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "    VinSensorGeo.Height : %d"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.VinSensorGeo.Height PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "    LdcVector           : %p"
                                PRN_CAL_LDC_ARG_CPOINT pNandTable->LdcVector PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                        }
                    }

                    PRN_CAL_LDC_LOG " " PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "---- Ldc Table Info ----" PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  NumOfTable     : %d"
                        PRN_CAL_LDC_ARG_UINT32 SVC_CalibLdcCtrl.NumOfLdcTable PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API

                    {
                        const SVC_CALIB_LDC_TABLE_s *pLdcTable;

                        for (Idx = 0U; Idx < SVC_CalibLdcCtrl.NumOfLdcTable; Idx ++) {
                            pLdcTable = &(SVC_CalibLdcCtrl.pLdcTable[Idx]);

                            PRN_CAL_LDC_LOG "  ---- Ldc Table[%d] %p -----"
                                PRN_CAL_LDC_ARG_UINT32 Idx       PRN_CAL_LDC_ARG_POST
                                PRN_CAL_LDC_ARG_CPOINT pLdcTable PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "    VinSelectBits       : 0x%X"
                                PRN_CAL_LDC_ARG_UINT32 pLdcTable->VinSelectBits PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "    SensorSelectBits    : 0x%X"
                                PRN_CAL_LDC_ARG_UINT32 pLdcTable->SensorSelectBits PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "    FmtSelectBits       : 0x%X"
                                PRN_CAL_LDC_ARG_UINT32 pLdcTable->FmtSelectBits PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "    FovSelectBits       : 0x%X"
                                PRN_CAL_LDC_ARG_UINT32 pLdcTable->FovSelectBits PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "    Table Addr          : %p"
                                PRN_CAL_LDC_ARG_CPOINT pLdcTable->LdcVector PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                        }
                    }
                }

                SVC_CalibLdcCtrlFlag |= SVC_CALIB_LDC_FLG_SHADOW_INIT;
                PRN_CAL_LDC_LOG "Successful to initial ldc dram shadow!" PRN_CAL_LDC_OK
            }

            SvcCalib_LdcMutexGive(&(SVC_CalibLdcCtrl.Mutex));
        }
    }

    return RetVal;
}


static UINT32 SvcCalib_LdcTableUpdate(const SVC_CALIB_CHANNEL_s *pCalibChan, const AMBA_IK_MODE_CFG_s *pImgMode)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibLdcCtrlFlag & SVC_CALIB_LDC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to update ldc table - initial ldc module first!" PRN_CAL_LDC_NG
    } else if ((SVC_CalibLdcCtrlFlag & SVC_CALIB_LDC_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to update ldc table - initial ldc dram shadow first!" PRN_CAL_LDC_NG
    } else if (pCalibChan == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to update ldc table - invalid calib channel!" PRN_CAL_LDC_NG
    } else if (pImgMode == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to update ldc table - invalid image mode!" PRN_CAL_LDC_NG
    } else if (SVC_CalibLdcCtrl.pNandHeader == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to update ldc table - invalid nand table header!" PRN_CAL_LDC_NG
    } else if ((SVC_CalibLdcCtrl.pNandHeader->Enable == 0U) ||
               (SVC_CalibLdcCtrl.pNandHeader->NumOfTable == 0U)) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "There is not ldc table in shadow buffer!" PRN_CAL_LDC_DBG
    } else {
        if (KAL_ERR_NONE != SvcCalib_LdcMutexTake(&(SVC_CalibLdcCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
            RetVal = SVC_NG;
            PRN_CAL_LDC_LOG "Fail to update ldc table - take mutex fail!" PRN_CAL_LDC_NG
        } else {
            const SVC_CALIB_LDC_TABLE_s  *pLdcTable = SvcCalib_LdcTableGet(pCalibChan);

            if (pLdcTable == NULL) {
                RetVal = SVC_NG;
                PRN_CAL_LDC_LOG "Fail to update ldc table - get ldc table fail" PRN_CAL_LDC_NG
            } else {
                if (SVC_CalibLdcCtrl.EnableDebugMsg > 0U) {

                    PRN_CAL_LDC_LOG " " PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "====== Calibration ldc info ======" PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  VinSelectBits                     : 0x%X"
                        PRN_CAL_LDC_ARG_UINT32 pLdcTable->VinSelectBits PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  SensorSelectBits                  : 0x%X"
                        PRN_CAL_LDC_ARG_UINT32 pLdcTable->SensorSelectBits PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  FmtSelectBits                     : 0x%X"
                        PRN_CAL_LDC_ARG_UINT32 pLdcTable->FmtSelectBits PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  FovSelectBits                     : 0x%X"
                        PRN_CAL_LDC_ARG_UINT32 pLdcTable->FovSelectBits PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  Version                           : 0x%X"
                        PRN_CAL_LDC_ARG_UINT32 pLdcTable->WarpInfo.Version PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  HorGridNum                        : %d"
                        PRN_CAL_LDC_ARG_UINT32 pLdcTable->WarpInfo.HorGridNum PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  VerGridNum                        : %d"
                        PRN_CAL_LDC_ARG_UINT32 pLdcTable->WarpInfo.VerGridNum PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  TileWidthExp                      : %d"
                        PRN_CAL_LDC_ARG_UINT32 pLdcTable->WarpInfo.TileWidthExp PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  TileHeightExp                     : %d"
                        PRN_CAL_LDC_ARG_UINT32 pLdcTable->WarpInfo.TileHeightExp PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  VinSensorGeo.StartX               : %d"
                        PRN_CAL_LDC_ARG_UINT32 pLdcTable->WarpInfo.VinSensorGeo.StartX PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  VinSensorGeo.StartY               : %d"
                        PRN_CAL_LDC_ARG_UINT32 pLdcTable->WarpInfo.VinSensorGeo.StartY PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  VinSensorGeo.Width                : %d"
                        PRN_CAL_LDC_ARG_UINT32 pLdcTable->WarpInfo.VinSensorGeo.Width PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  VinSensorGeo.Height               : %d"
                        PRN_CAL_LDC_ARG_UINT32 pLdcTable->WarpInfo.VinSensorGeo.Height PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  VinSensorGeo.HSubSample.FactorDen : %d"
                        PRN_CAL_LDC_ARG_UINT32 pLdcTable->WarpInfo.VinSensorGeo.HSubSample.FactorDen PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  VinSensorGeo.HSubSample.FactorNum : %d"
                        PRN_CAL_LDC_ARG_UINT32 pLdcTable->WarpInfo.VinSensorGeo.HSubSample.FactorNum PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  VinSensorGeo.VSubSample.FactorDen : %d"
                        PRN_CAL_LDC_ARG_UINT32 pLdcTable->WarpInfo.VinSensorGeo.VSubSample.FactorDen PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  VinSensorGeo.VSubSample.FactorNum : %d"
                        PRN_CAL_LDC_ARG_UINT32 pLdcTable->WarpInfo.VinSensorGeo.VSubSample.FactorNum PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  Enb2StageCompensation             : %d"
                        PRN_CAL_LDC_ARG_UINT32 pLdcTable->WarpInfo.Enb_2StageCompensation PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  pWarp                             : %p"
                        PRN_CAL_LDC_ARG_CPOINT pLdcTable->WarpInfo.pWarp PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                }

                if (0U != AmbaIK_SetWarpInfo(pImgMode, &(pLdcTable->WarpInfo))) {
                    RetVal = SVC_NG;
                    PRN_CAL_LDC_LOG "Fail to update ldc table - set ldc info fail!" PRN_CAL_LDC_NG
                } else {
                    RetVal = AmbaIK_SetWarpEnb(pImgMode, 1U);
                    if (RetVal != SVC_OK) {
                        PRN_CAL_LDC_LOG "Fail to update ldc table - enable warp fail!" PRN_CAL_LDC_NG
                    }
                }

                if (RetVal == SVC_OK) {

                    if (SVC_CalibLdcCtrl.CalUpdCtrl == SVC_CALIB_LDC_UPD_BY_FMT_FOV) {
                        PRN_CAL_LDC_LOG "Successful to update the ldc table FmtID(%d) FovID(%d) ImgContextID(%d)"
                            PRN_CAL_LDC_ARG_UINT32 pCalibChan->FmtID PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_ARG_UINT32 pCalibChan->FovID PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_ARG_UINT32 pImgMode->ContextId  PRN_CAL_LDC_ARG_POST
                        PRN_CAL_LDC_OK
                    } else {
                        PRN_CAL_LDC_LOG "Successful to update the ldc table VinID(%d) SensorID(%d) ImgContextID(%d)"
                            PRN_CAL_LDC_ARG_UINT32 pCalibChan->VinID    PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_ARG_UINT32 pCalibChan->SensorID PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_ARG_UINT32 pImgMode->ContextId  PRN_CAL_LDC_ARG_POST
                        PRN_CAL_LDC_OK
                    }

                }
            }

            SvcCalib_LdcMutexGive(&(SVC_CalibLdcCtrl.Mutex));
        }
    }

    return RetVal;
}


static UINT32 SvcCalib_LdcTableCfg(UINT32 NandTableID, UINT32 TableID)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibLdcCtrlFlag & SVC_CALIB_LDC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG " Failure to configure ldc table - Need to initial ldc control module first!" PRN_CAL_LDC_NG
    } else if ((SVC_CalibLdcCtrlFlag & SVC_CALIB_LDC_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG " Failure to configure ldc table - Need to initial ldc dram shadow first!" PRN_CAL_LDC_NG
    } else if (SVC_CalibLdcCtrl.pNandTable == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG " Failure to configure ldc table - invalid nand table!" PRN_CAL_LDC_NG
    } else if (SVC_CalibLdcCtrl.pLdcTable == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG " Failure to configure ldc table - invalid ldc table!" PRN_CAL_LDC_NG
    } else if (TableID >= SVC_CalibLdcCtrl.NumOfLdcTable) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG " Failure to configure ldc table - output ldc table id is out-of max table size!" PRN_CAL_LDC_NG
    } else if (NandTableID >= SVC_CalibLdcCtrl.NumOfLdcTable) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG " Failure to configure ldc table - nand ldc table id is out-of max table size!" PRN_CAL_LDC_NG
    } else {
        SVC_CALIB_LDC_TABLE_s *pLdcTable = &(SVC_CalibLdcCtrl.pLdcTable[TableID]);

        /* Reset the ldc table context */
        AmbaSvcWrap_MisraMemset(pLdcTable, 0, sizeof(SVC_CALIB_LDC_TABLE_s));

        /* Config ldc table */
        {
            const SVC_CALIB_LDC_NAND_TABLE_s *pNandTable = &(SVC_CalibLdcCtrl.pNandTable[NandTableID]);

            if ((pNandTable->Header.HorGridNum * pNandTable->Header.VerGridNum) > (SVC_CALIB_LDC_MAX_TBL_LEN)) {
                RetVal = SVC_NG;
                PRN_CAL_LDC_LOG " Warp table grid is out-of range! %dx%d"
                    PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.HorGridNum PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.VerGridNum PRN_CAL_LDC_ARG_POST
                PRN_CAL_LDC_NG
            } else {
                pLdcTable->WarpInfo.Version                           = pNandTable->Header.Version;
                pLdcTable->WarpInfo.HorGridNum                        = pNandTable->Header.HorGridNum;
                pLdcTable->WarpInfo.VerGridNum                        = pNandTable->Header.VerGridNum;
                pLdcTable->WarpInfo.TileWidthExp                      = pNandTable->Header.TileWidthExp;
                pLdcTable->WarpInfo.TileHeightExp                     = pNandTable->Header.TileHeightExp;
                pLdcTable->WarpInfo.VinSensorGeo.StartX               = pNandTable->Header.VinSensorGeo.StartX;
                pLdcTable->WarpInfo.VinSensorGeo.StartY               = pNandTable->Header.VinSensorGeo.StartY;
                pLdcTable->WarpInfo.VinSensorGeo.Width                = pNandTable->Header.VinSensorGeo.Width;
                pLdcTable->WarpInfo.VinSensorGeo.Height               = pNandTable->Header.VinSensorGeo.Height;
                pLdcTable->WarpInfo.VinSensorGeo.HSubSample.FactorDen = pNandTable->Header.VinSensorGeo.HSubSample.FactorDen;
                pLdcTable->WarpInfo.VinSensorGeo.HSubSample.FactorNum = pNandTable->Header.VinSensorGeo.HSubSample.FactorNum;
                pLdcTable->WarpInfo.VinSensorGeo.VSubSample.FactorDen = pNandTable->Header.VinSensorGeo.VSubSample.FactorDen;
                pLdcTable->WarpInfo.VinSensorGeo.VSubSample.FactorNum = pNandTable->Header.VinSensorGeo.VSubSample.FactorNum;
                pLdcTable->WarpInfo.pWarp                             = pLdcTable->LdcVector;
                if (pNandTable->Header.TwoStageDone == 0U) {
                    pLdcTable->WarpInfo.Enb_2StageCompensation        = SVC_CALIB_LDC_IK_2_STAGE;
                } else {
                    pLdcTable->WarpInfo.Enb_2StageCompensation        = 0U;
                }
                pLdcTable->VinSelectBits                              = pNandTable->Header.VinSelectBits;
                pLdcTable->SensorSelectBits                           = pNandTable->Header.SensorSelectBits;
                pLdcTable->FmtSelectBits                              = pNandTable->Header.FmtSelectBits;
                pLdcTable->FovSelectBits                              = pNandTable->Header.FovSelectBits;

                AmbaSvcWrap_MisraMemcpy(pLdcTable->LdcVector, pNandTable->LdcVector,
                       (sizeof(AMBA_IK_GRID_POINT_s) * pNandTable->Header.HorGridNum * pNandTable->Header.VerGridNum));
            }
        }
    }

    return RetVal;
}


static UINT32 SvcCalib_LdcTableSearch(UINT32 ID_1, UINT32 ID_2, UINT32 *pTblIdx)
{
    UINT32 RetVal = SVC_OK;
    UINT32 Idx, TableIdx = 0xFFFFFFFFU;

    if (SVC_CalibLdcCtrl.CalUpdCtrl == SVC_CALIB_LDC_UPD_BY_FMT_FOV) {
        if (SVC_OK != SvcCalib_LdcFmtFovIDCheck(ID_1, ID_2)) {
            RetVal = SVC_NG;
            PRN_CAL_LDC_LOG "Fail to search ldc table - invalid calib channel -> FmtID(%d), FovID(%d)!"
                PRN_CAL_LDC_ARG_UINT32 ID_1 PRN_CAL_LDC_ARG_POST
                PRN_CAL_LDC_ARG_UINT32 ID_2 PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_NG
        } else {
            for (Idx = 0U; Idx < SVC_CalibLdcCtrl.NumOfLdcTable; Idx ++) {
                if (((SVC_CalibLdcCtrl.pLdcTable[Idx].FmtSelectBits & SvcCalib_BitGet(ID_1)) > 0U) &&
                    ((SVC_CalibLdcCtrl.pLdcTable[Idx].FovSelectBits & SvcCalib_BitGet(ID_2)) > 0U)) {
                    TableIdx = Idx;
                    break;
                }
            }
        }
    } else {
        if (SVC_OK != SvcCalib_LdcVinSensorIDCheck(ID_1, ID_2)) {
            RetVal = SVC_NG;
            PRN_CAL_LDC_LOG "Fail to search ldc table - invalid calib channel -> VinID(%d), SensorID(%d)!"
                PRN_CAL_LDC_ARG_UINT32 ID_1 PRN_CAL_LDC_ARG_POST
                PRN_CAL_LDC_ARG_UINT32 ID_2 PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_NG
        } else {
            for (Idx = 0U; Idx < SVC_CalibLdcCtrl.NumOfLdcTable; Idx ++) {
                if (((SVC_CalibLdcCtrl.pLdcTable[Idx].VinSelectBits & SvcCalib_BitGet(ID_1)) > 0U) &&
                    ((SVC_CalibLdcCtrl.pLdcTable[Idx].SensorSelectBits & ID_2) > 0U)) {
                    TableIdx = Idx;
                    break;
                }
            }
        }
    }

    if (pTblIdx != NULL) {
        *pTblIdx = TableIdx;
    }

    return RetVal;
}


static UINT32 SvcCalib_LdcNandTableSearch(UINT32 ID_1, UINT32 ID_2, UINT32 *pTblIdx)
{
    UINT32 RetVal = SVC_OK;
    UINT32 Idx, TableIdx = 0xFFFFFFFFU;

    if (SVC_CalibLdcCtrl.CalUpdCtrl == SVC_CALIB_LDC_UPD_BY_FMT_FOV) {
        if (SVC_OK != SvcCalib_LdcFmtFovIDCheck(ID_1, ID_2)) {
            RetVal = SVC_NG;
            PRN_CAL_LDC_LOG "Fail to search ldc nand table - invalid calib channel -> FmtID(%d), FovID(%d)!"
                PRN_CAL_LDC_ARG_UINT32 ID_1 PRN_CAL_LDC_ARG_POST
                PRN_CAL_LDC_ARG_UINT32 ID_2 PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_NG
        } else {
            for (Idx = 0U; Idx < SVC_CalibLdcCtrl.NumOfLdcTable; Idx ++) {
                if (((SVC_CalibLdcCtrl.pNandTable[Idx].Header.FmtSelectBits & SvcCalib_BitGet(ID_1)) > 0U) &&
                    ((SVC_CalibLdcCtrl.pNandTable[Idx].Header.FovSelectBits & SvcCalib_BitGet(ID_2)) > 0U) &&
                    ((SVC_CalibLdcCtrl.pNandHeader->TableSelectBits & SvcCalib_BitGet(Idx)) > 0U)) {
                    TableIdx = Idx;
                    break;
                }
            }
        }
    } else {
        if (SVC_OK != SvcCalib_LdcVinSensorIDCheck(ID_1, ID_2)) {
            RetVal = SVC_NG;
            PRN_CAL_LDC_LOG "Fail to search ldc nand table - invalid calib channel -> VinID(%d), SensorID(%d)!"
                PRN_CAL_LDC_ARG_UINT32 ID_1 PRN_CAL_LDC_ARG_POST
                PRN_CAL_LDC_ARG_UINT32 ID_2 PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_NG
        } else {
            for (Idx = 0U; Idx < SVC_CalibLdcCtrl.NumOfLdcTable; Idx ++) {
                if (((SVC_CalibLdcCtrl.pNandTable[Idx].Header.VinSelectBits & SvcCalib_BitGet(ID_1)) > 0U) &&
                    ((SVC_CalibLdcCtrl.pNandTable[Idx].Header.SensorSelectBits & ID_2) > 0U) &&
                    ((SVC_CalibLdcCtrl.pNandHeader->TableSelectBits & SvcCalib_BitGet(Idx)) > 0U)) {
                    TableIdx = Idx;
                    break;
                }
            }
        }
    }

    if (pTblIdx != NULL) {
        *pTblIdx = TableIdx;
    }


    return RetVal;
}


static SVC_CALIB_LDC_TABLE_s * SvcCalib_LdcTableGet(const SVC_CALIB_CHANNEL_s *pCalibChan)
{
    UINT32 RetVal = SVC_OK;
    SVC_CALIB_LDC_TABLE_s *pTable = NULL;

    if ((SVC_CalibLdcCtrlFlag & SVC_CALIB_LDC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to get ldc table - Need to initial ldc control module first!" PRN_CAL_LDC_NG
    } else if ((SVC_CalibLdcCtrlFlag & SVC_CALIB_LDC_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to get ldc table - Need to initial ldc dram shadow first!" PRN_CAL_LDC_NG
    } else if (SVC_CalibLdcCtrl.pLdcTable == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to get ldc table - invalid ldc table!" PRN_CAL_LDC_NG
    } else if (pCalibChan == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to get ldc table - invalid calib channel!" PRN_CAL_LDC_NG
    } else if ((SVC_CalibLdcCtrl.CalUpdCtrl == SVC_CALIB_LDC_UPD_BY_VIN_SS)
            && (SVC_OK != SvcCalib_LdcVinSensorIDCheck(pCalibChan->VinID, pCalibChan->SensorID))) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to get ldc table - invalid calib channel -> VinID(%d), SensorID(0x%X)!"
            PRN_CAL_LDC_ARG_UINT32 pCalibChan->VinID    PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_ARG_UINT32 pCalibChan->SensorID PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_NG
    } else if ((SVC_CalibLdcCtrl.CalUpdCtrl == SVC_CALIB_LDC_UPD_BY_FMT_FOV)
            && (SVC_OK != SvcCalib_LdcFmtFovIDCheck(pCalibChan->FmtID, pCalibChan->FovID))) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to get ldc table - invalid calib channel -> FmtID(%d), FovID(%d)!"
            PRN_CAL_LDC_ARG_UINT32 pCalibChan->FmtID PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_ARG_UINT32 pCalibChan->FovID PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_NG
    } else {
        UINT32 Idx;
        UINT32 TableIdx     = 0xFFFFFFFFU;
        UINT32 NandTableIdx = 0xFFFFFFFFU;

        if (SVC_CalibLdcCtrl.CalUpdCtrl == SVC_CALIB_LDC_UPD_BY_FMT_FOV) {
            RetVal  = SvcCalib_LdcTableSearch(pCalibChan->FmtID, pCalibChan->FovID, &TableIdx);
            RetVal |= SvcCalib_LdcNandTableSearch(pCalibChan->FmtID, pCalibChan->FovID, &NandTableIdx);
        } else {
            RetVal  = SvcCalib_LdcTableSearch(pCalibChan->VinID, pCalibChan->SensorID, &TableIdx);
            RetVal |= SvcCalib_LdcNandTableSearch(pCalibChan->VinID, pCalibChan->SensorID, &NandTableIdx);
        }
        if (RetVal == SVC_OK) {
            /* If not get ldc table, search the nand table by calib channel */
            if (TableIdx == 0xFFFFFFFFU) {
                if (NandTableIdx < SVC_CalibLdcCtrl.NumOfLdcTable) {
                    /* Search free ldc table */
                    for (Idx = 0; Idx < SVC_CalibLdcCtrl.NumOfLdcTable; Idx ++) {
                        if ((SVC_CalibLdcCtrl.pLdcTable[Idx].VinSelectBits == 0U) &&
                            (SVC_CalibLdcCtrl.pLdcTable[Idx].SensorSelectBits == 0U)) {
                            TableIdx = Idx;
                            PRN_CAL_LDC_LOG "Successful to get ldc free table(%d)."
                                PRN_CAL_LDC_ARG_UINT32 TableIdx PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_DBG
                            break;
                        }
                    }

                    /* There is not free ldc table to service it */
                    if (TableIdx == 0xFFFFFFFFU) {
                        RetVal = SVC_NG;
                        if (SVC_CalibLdcCtrl.CalUpdCtrl == SVC_CALIB_LDC_UPD_BY_FMT_FOV) {
                            PRN_CAL_LDC_LOG "Fail to get ldc table - there is not free table to support FmtID(%d), FovID(%d)"
                                PRN_CAL_LDC_ARG_UINT32 pCalibChan->FmtID PRN_CAL_LDC_ARG_POST
                                PRN_CAL_LDC_ARG_UINT32 pCalibChan->FovID PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_DBG
                        } else {
                            PRN_CAL_LDC_LOG "Fail to get ldc table - there is not free table to support VinID(%d), SensorID(0x%X)"
                                PRN_CAL_LDC_ARG_UINT32 pCalibChan->VinID    PRN_CAL_LDC_ARG_POST
                                PRN_CAL_LDC_ARG_UINT32 pCalibChan->SensorID PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_DBG
                        }
                    }
                } else {
                    RetVal = SVC_NG;
                    if (SVC_CalibLdcCtrl.CalUpdCtrl == SVC_CALIB_LDC_UPD_BY_FMT_FOV) {
                        PRN_CAL_LDC_LOG "Fail to get ldc table - current ldc cannot support FmtID(%d), FovID(%d)"
                            PRN_CAL_LDC_ARG_UINT32 pCalibChan->FmtID PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_ARG_UINT32 pCalibChan->FovID PRN_CAL_LDC_ARG_POST
                        PRN_CAL_LDC_DBG
                    } else {
                        PRN_CAL_LDC_LOG "Fail to get ldc table - current ldc cannot support VinID(%d), SensorID(0x%X)"
                            PRN_CAL_LDC_ARG_UINT32 pCalibChan->VinID    PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_ARG_UINT32 pCalibChan->SensorID PRN_CAL_LDC_ARG_POST
                        PRN_CAL_LDC_DBG
                    }
                }
            }

            /* generate ldc table */
            if ((TableIdx < SVC_CalibLdcCtrl.NumOfLdcTable) && (NandTableIdx < SVC_CalibLdcCtrl.NumOfLdcTable)) {
                RetVal = SvcCalib_LdcTableCfg(NandTableIdx, TableIdx);
                if (RetVal != SVC_OK) {
                    PRN_CAL_LDC_LOG "Fail to get ldc table - re-configure ldc table fail!" PRN_CAL_LDC_DBG
                    pTable = NULL;
                } else {
                    PRN_CAL_LDC_LOG "Successful to get ldc table - configure done!" PRN_CAL_LDC_DBG
                    pTable = &(SVC_CalibLdcCtrl.pLdcTable[TableIdx]);
                }
            } else {
                if (TableIdx < SVC_CalibLdcCtrl.NumOfLdcTable) {
                    pTable = &(SVC_CalibLdcCtrl.pLdcTable[TableIdx]);
                    PRN_CAL_LDC_LOG "Successful to get ldc table - existed table!" PRN_CAL_LDC_OK
                }
            }
        }
    }

    AmbaMisra_TouchUnused(&RetVal);

    return pTable;
}


static UINT32 SvcCalib_LdcTableGenTbl(const SVC_CALIB_CHANNEL_s *pCalibChan, const char *pScriptPath)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibLdcCtrlFlag & SVC_CALIB_LDC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to calc ldc table - Need to initial ldc control module first!" PRN_CAL_LDC_NG
    } else if ((SVC_CalibLdcCtrlFlag & SVC_CALIB_LDC_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to calc ldc table - Need to initial ldc dram shadow first!" PRN_CAL_LDC_NG
    } else if (SVC_CalibLdcCtrl.pNandHeader == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to calc ldc table - invalid nand header!" PRN_CAL_LDC_NG
    } else if (SVC_CalibLdcCtrl.pNandTable == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to calc ldc table - invalid nand table!" PRN_CAL_LDC_NG
    } else if (pCalibChan == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to calc ldc table - invalid calib channel!" PRN_CAL_LDC_NG
    } else if (SVC_OK != SvcCalib_LdcMemLock()) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to calc ldc table - lock ldc memory fail!" PRN_CAL_LDC_NG
    } else if (KAL_ERR_NONE != SvcCalib_LdcMutexTake(&(SVC_CalibLdcCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to calc ldc table - take mutex fail!" PRN_CAL_LDC_NG
    } else {
        AMBA_CT_INITIAL_CONFIG_s CtnCfg;

        AmbaSvcWrap_MisraMemset(&CtnCfg, 0, sizeof(CtnCfg));

        CtnCfg.pTunerWorkingBuf    = SVC_CalibLdcCtrl.MemCtrl[SVC_CALIB_LDC_MEM_CALC_CTUNER].pBuf;
        CtnCfg.TunerWorkingBufSize = SVC_CalibLdcCtrl.MemCtrl[SVC_CALIB_LDC_MEM_CALC_CTUNER].BufSize;

        CtnCfg.Ldc.pCalibWorkingBuf    = SVC_CalibLdcCtrl.MemCtrl[SVC_CALIB_LDC_MEM_CALC].pBuf;
        CtnCfg.Ldc.CalibWorkingBufSize = SVC_CalibLdcCtrl.MemCtrl[SVC_CALIB_LDC_MEM_CALC].BufSize;

        PRetVal = AmbaCT_Init(AMBA_CT_TYPE_LDC, &CtnCfg);
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_CAL_LDC_LOG "Fail to calc ldc by ctuner - initial ctuner fail! 0x%08X"
                PRN_CAL_LDC_ARG_UINT32 PRetVal PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_NG
        } else {
            PRetVal = AmbaCT_Load(pScriptPath);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_CAL_LDC_LOG "Fail to calc ldc by ctuner - load ctuner script fail! 0x%08X"
                    PRN_CAL_LDC_ARG_UINT32 PRetVal PRN_CAL_LDC_ARG_POST
                PRN_CAL_LDC_NG
            } else {
                PRetVal = AmbaCT_Execute();
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    PRN_CAL_LDC_LOG "Fail to calc ldc by ctuner - execute ctuner fail! 0x%08X"
                        PRN_CAL_LDC_ARG_UINT32 PRetVal PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_NG
                } else {
                    AMBA_CT_LDC_CALIB_DATA_s LdcData;
                    AMBA_CAL_LDC_DATA_s *pData;

                    AmbaSvcWrap_MisraMemset(&LdcData, 0, sizeof(LdcData));

                    PRetVal = AmbaCT_LdcGetCalibData(&LdcData); PRN_CAL_LDC_ERR_HDLR

                    pData = LdcData.pCalibData;

                    if (pData == NULL) {
                        RetVal = SVC_NG;
                        PRN_CAL_LDC_LOG "Fail to calc ldc table - invalid ctuner result!" PRN_CAL_LDC_NG
                    } else if ((pData->WarpTbl.HorGridNum * pData->WarpTbl.VerGridNum) > SVC_CALIB_LDC_MAX_TBL_LEN) {
                        PRetVal = SVC_CALIB_LDC_MAX_TBL_LEN;
                        PRN_CAL_LDC_LOG "Fail to calc ldc table - the grid number is out-of range! %dx%d > %d"
                            PRN_CAL_LDC_ARG_UINT32 pData->WarpTbl.HorGridNum PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_ARG_UINT32 pData->WarpTbl.VerGridNum PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_ARG_UINT32 PRetVal                   PRN_CAL_LDC_ARG_POST
                        PRN_CAL_LDC_NG
                        RetVal = SVC_NG;
                    } else if (SVC_CalibLdcCtrl.CalcLdcCtrl.TableIdx >= 32U) {
                        RetVal = SVC_NG;
                        PRN_CAL_LDC_LOG "Fail to calc ldc table - table id(%d) is out-of range"
                            PRN_CAL_LDC_ARG_UINT32 SVC_CalibLdcCtrl.CalcLdcCtrl.TableIdx PRN_CAL_LDC_ARG_POST
                        PRN_CAL_LDC_NG
                    } else {
                        SVC_CALIB_LDC_NAND_HEADER_s *pNandHeader = SVC_CalibLdcCtrl.pNandHeader;
                        SVC_CALIB_LDC_NAND_TABLE_s  *pNandTable = &(SVC_CalibLdcCtrl.pNandTable[SVC_CalibLdcCtrl.CalcLdcCtrl.TableIdx]);

                        /* Update dram shadow data */
                        AmbaSvcWrap_MisraMemset(pNandTable, 0, sizeof(SVC_CALIB_LDC_NAND_TABLE_s));
                        pNandTable->Header.VinSelectBits                     = SVC_CalibLdcCtrl.CalibChan.VinSelectBits;
                        pNandTable->Header.SensorSelectBits                  = SVC_CalibLdcCtrl.CalibChan.SensorSelectBits;
                        pNandTable->Header.FmtSelectBits                     = SVC_CalibLdcCtrl.CalibChan.FmtSelectBits;
                        pNandTable->Header.FovSelectBits                     = SVC_CalibLdcCtrl.CalibChan.FovSelectBits;
#ifndef CONFIG_SOC_CV2FS
                        pNandTable->Header.Version                           = SVC_CALIB_LDC_VERSION;
                        pNandTable->Header.HorGridNum                        = pData->WarpTbl.HorGridNum;
                        pNandTable->Header.VerGridNum                        = pData->WarpTbl.VerGridNum;
                        pNandTable->Header.TileWidthExp                      = pData->WarpTbl.TileWidthExp;
                        pNandTable->Header.TileHeightExp                     = pData->WarpTbl.TileHeightExp;
                        pNandTable->Header.VinSensorGeo.StartX               = pData->WarpTbl.CalibSensorGeo.StartX;
                        pNandTable->Header.VinSensorGeo.StartY               = pData->WarpTbl.CalibSensorGeo.StartY;
                        pNandTable->Header.VinSensorGeo.Width                = pData->WarpTbl.CalibSensorGeo.Width;
                        pNandTable->Header.VinSensorGeo.Height               = pData->WarpTbl.CalibSensorGeo.Height;
                        pNandTable->Header.VinSensorGeo.HSubSample.FactorDen = pData->WarpTbl.CalibSensorGeo.HSubSample.FactorDen;
                        pNandTable->Header.VinSensorGeo.HSubSample.FactorNum = pData->WarpTbl.CalibSensorGeo.HSubSample.FactorNum;
                        pNandTable->Header.VinSensorGeo.VSubSample.FactorDen = pData->WarpTbl.CalibSensorGeo.VSubSample.FactorDen;
                        pNandTable->Header.VinSensorGeo.VSubSample.FactorNum = pData->WarpTbl.CalibSensorGeo.VSubSample.FactorNum;
                        pNandTable->Header.TwoStageDone                      = 0U;
                        AmbaSvcWrap_MisraMemcpy(pNandTable->LdcVector, pData->WarpTbl.WarpVector, sizeof(AMBA_IK_GRID_POINT_s)*MAX_WARP_TBL_LEN);
                        if (PRetVal != 0U) {
                            RetVal = SVC_NG;
                        }
#else
                        {
                            AMBA_IK_WARP_INFO_s InputInfo, ResultInfo;
                            AMBA_IK_IN_WARP_PRE_PROC_s TwoStageProcIn;
                            AMBA_IK_OUT_WARP_PRE_PROC_s TwoStageProcOut;

                            AmbaSvcWrap_MisraMemset(&InputInfo, 0, sizeof(InputInfo));
                            InputInfo.Version                           = SVC_CALIB_LDC_VERSION;
                            InputInfo.HorGridNum                        = pData->WarpTbl.HorGridNum;
                            InputInfo.VerGridNum                        = pData->WarpTbl.VerGridNum;
                            InputInfo.TileWidthExp                      = pData->WarpTbl.TileWidthExp;
                            InputInfo.TileHeightExp                     = pData->WarpTbl.TileHeightExp;
                            InputInfo.VinSensorGeo.StartX               = pData->WarpTbl.CalibSensorGeo.StartX;
                            InputInfo.VinSensorGeo.StartY               = pData->WarpTbl.CalibSensorGeo.StartY;
                            InputInfo.VinSensorGeo.Width                = pData->WarpTbl.CalibSensorGeo.Width;
                            InputInfo.VinSensorGeo.Height               = pData->WarpTbl.CalibSensorGeo.Height;
                            InputInfo.VinSensorGeo.HSubSample.FactorDen = pData->WarpTbl.CalibSensorGeo.HSubSample.FactorDen;
                            InputInfo.VinSensorGeo.HSubSample.FactorNum = pData->WarpTbl.CalibSensorGeo.HSubSample.FactorNum;
                            InputInfo.VinSensorGeo.VSubSample.FactorDen = pData->WarpTbl.CalibSensorGeo.VSubSample.FactorDen;
                            InputInfo.VinSensorGeo.VSubSample.FactorNum = pData->WarpTbl.CalibSensorGeo.VSubSample.FactorNum;
                            InputInfo.pWarp                             = pData->WarpTbl.WarpVector;
                            AmbaSvcWrap_MisraMemset(&TwoStageProcIn, 0, sizeof(TwoStageProcIn));
                            TwoStageProcIn.pInputInfo        = &InputInfo;
                            TwoStageProcIn.pWorkingBuffer    = SVC_CalibLdcCtrl.MemCtrl[SVC_CALIB_LDC_MEM_CALC_REMAP].pBuf;
                            TwoStageProcIn.WorkingBufferSize = SVC_CalibLdcCtrl.MemCtrl[SVC_CALIB_LDC_MEM_CALC_REMAP].BufSize;

                            AmbaSvcWrap_MisraMemset(&ResultInfo, 0, sizeof(ResultInfo));
                            ResultInfo.pWarp = pNandTable->LdcVector;
                            AmbaSvcWrap_MisraMemset(&TwoStageProcOut, 0, sizeof(TwoStageProcOut));
                            TwoStageProcOut.pResultInfo = &ResultInfo;

                            PRetVal = AmbaIK_PreProcWarpTbl(&TwoStageProcIn, &TwoStageProcOut);
                            if (PRetVal != 0U) {
                                PRN_CAL_LDC_LOG "Fail to calc ldc table - two stage remap fail! ErrCode(0x%08x)"
                                    PRN_CAL_LDC_ARG_UINT32 PRetVal PRN_CAL_LDC_ARG_POST
                                PRN_CAL_LDC_NG
                                RetVal = SVC_NG;
                            } else {
                                PRN_CAL_LDC_LOG "Two stage re-map done!" PRN_CAL_LDC_API

                                pNandTable->Header.Version                           = ResultInfo.Version;
                                pNandTable->Header.HorGridNum                        = ResultInfo.HorGridNum;
                                pNandTable->Header.VerGridNum                        = ResultInfo.VerGridNum;
                                pNandTable->Header.TileWidthExp                      = ResultInfo.TileWidthExp;
                                pNandTable->Header.TileHeightExp                     = ResultInfo.TileHeightExp;
                                pNandTable->Header.VinSensorGeo.StartX               = ResultInfo.VinSensorGeo.StartX;
                                pNandTable->Header.VinSensorGeo.StartY               = ResultInfo.VinSensorGeo.StartY;
                                pNandTable->Header.VinSensorGeo.Width                = ResultInfo.VinSensorGeo.Width;
                                pNandTable->Header.VinSensorGeo.Height               = ResultInfo.VinSensorGeo.Height;
                                pNandTable->Header.VinSensorGeo.HSubSample.FactorDen = ResultInfo.VinSensorGeo.HSubSample.FactorDen;
                                pNandTable->Header.VinSensorGeo.HSubSample.FactorNum = ResultInfo.VinSensorGeo.HSubSample.FactorNum;
                                pNandTable->Header.VinSensorGeo.VSubSample.FactorDen = ResultInfo.VinSensorGeo.VSubSample.FactorDen;
                                pNandTable->Header.VinSensorGeo.VSubSample.FactorNum = ResultInfo.VinSensorGeo.VSubSample.FactorNum;
                                pNandTable->Header.TwoStageDone = 1U;
                            }
                        }
#endif
                        AmbaMisra_TouchUnused(&PRetVal);
                        AmbaMisra_TouchUnused(pData);

                        if (RetVal == SVC_OK) {
                            pNandHeader->Enable = 1U;
                            pNandHeader->DebugMsgOn = SVC_CalibLdcCtrl.EnableDebugMsg;
                            if ((pNandHeader->TableSelectBits & SvcCalib_BitGet(SVC_CalibLdcCtrl.CalcLdcCtrl.TableIdx)) == 0U) {
                                pNandHeader->TableSelectBits |= SvcCalib_BitGet(SVC_CalibLdcCtrl.CalcLdcCtrl.TableIdx);
                                pNandHeader->NumOfTable ++;
                            }

                            PRN_CAL_LDC_LOG " " PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "====== Calibration ldc info ======" PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "  Version                             : 0x%X"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.Version PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "  HorGridNum                          : %d"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.HorGridNum PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "  VerGridNum                          : %d"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.VerGridNum PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "  TileWidthExp                        : %d"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.TileWidthExp PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "  TileHeightExp                       : %d"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.TileHeightExp PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "  CalibSensorGeo.StartX               : %d"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.VinSensorGeo.StartX PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "  CalibSensorGeo.StartY               : %d"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.VinSensorGeo.StartY PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "  CalibSensorGeo.Width                : %d"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.VinSensorGeo.Width PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "  CalibSensorGeo.Height               : %d"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.VinSensorGeo.Height PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "  CalibSensorGeo.HSubSample.FactorDen : %d"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.VinSensorGeo.HSubSample.FactorDen PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "  CalibSensorGeo.HSubSample.FactorNum : %d"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.VinSensorGeo.HSubSample.FactorNum PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "  CalibSensorGeo.VSubSample.FactorDen : %d"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.VinSensorGeo.VSubSample.FactorDen PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "  CalibSensorGeo.VSubSample.FactorNum : %d"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.VinSensorGeo.VSubSample.FactorNum PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "  TwoStageDone                        : %d"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.TwoStageDone PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "  VinSelectBits                       : 0x%x"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.VinSelectBits PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "  SensorSelectBits                    : 0x%x"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.SensorSelectBits PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "  FmtSelectBits                       : 0x%x"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.FmtSelectBits PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "  FovSelectBits                       : 0x%x"
                                PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.FovSelectBits PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API
                            PRN_CAL_LDC_LOG "  WarpVector                          : %p"
                                PRN_CAL_LDC_ARG_CPOINT pNandTable->LdcVector PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_API

                            PRN_CAL_LDC_LOG "Successful to calc ldc table" PRN_CAL_LDC_OK

                            /* Update to NAND */
                            RetVal = SvcCalib_DataSave(SVC_CALIB_LDC_ID);
                            if (RetVal != OK) {
                                PRN_CAL_LDC_LOG "Fail to save ldc to nand!" PRN_CAL_LDC_NG
                            }
                        }
                    }
                }
            }
        }

        SvcCalib_LdcMutexGive(&(SVC_CalibLdcCtrl.Mutex));

        SvcCalib_LdcMemUnLock();
    }

    return RetVal;
}


static UINT32 SvcCalib_LdcTableDataSet(UINT32 TblID, SVC_CALIB_TBL_INFO_s *pTblInfo)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibLdcCtrlFlag & SVC_CALIB_LDC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to set ldc table - Need to initial ldc control module first!" PRN_CAL_LDC_NG
    } else if ((SVC_CalibLdcCtrlFlag & SVC_CALIB_LDC_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to set ldc table - Need to initial ldc dram shadow first!" PRN_CAL_LDC_NG
    } else if (SVC_CalibLdcCtrl.pNandHeader == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to set ldc table - invalid nand header!" PRN_CAL_LDC_NG
    } else if (SVC_CalibLdcCtrl.pNandTable == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to set ldc table - invalid nand table!" PRN_CAL_LDC_NG
    } else if (TblID >= SVC_CalibLdcCtrl.NumOfLdcTable) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to set ldc table - the TableID(%d) is out-of range(%d)!"
            PRN_CAL_LDC_ARG_UINT32 TblID                          PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_ARG_UINT32 SVC_CalibLdcCtrl.NumOfLdcTable PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_NG
    } else if (pTblInfo == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to set ldc table - invalid table info!" PRN_CAL_LDC_NG
    } else if (pTblInfo->pBuf == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to set ldc table - invalid table!" PRN_CAL_LDC_NG
    } else if (pTblInfo->BufSize < (UINT32)sizeof(SVC_CALIB_LDC_TBL_DATA_s)) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to set ldc table - input table size(0x%X) does not match support table size(0x%X)!"
            PRN_CAL_LDC_ARG_UINT32 pTblInfo->BufSize                        PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_ARG_UINT32 (UINT32)sizeof(SVC_CALIB_LDC_TBL_DATA_s) PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_NG
    } else if (KAL_ERR_NONE != SvcCalib_LdcMutexTake(&(SVC_CalibLdcCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to set ldc table - take mutex fail!" PRN_CAL_LDC_NG
    } else {
        SVC_CALIB_LDC_TBL_DATA_s    *pInput      = NULL;
        SVC_CALIB_LDC_NAND_TABLE_s  *pNandTbl    = &(SVC_CalibLdcCtrl.pNandTable[TblID]);
        SVC_CALIB_LDC_NAND_HEADER_s *pNandHeader = SVC_CalibLdcCtrl.pNandHeader;

        AmbaMisra_TypeCast(&(pInput), &(pTblInfo->pBuf));

        if (pInput->pLdcVector == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_LDC_LOG "Fail to set ldc table - input ldc vector should not null!" PRN_CAL_LDC_NG
        } else if (pInput->LdcVectorLength > SVC_CALIB_LDC_MAX_TBL_LEN) {
            UINT32 TmpU32 = SVC_CALIB_LDC_MAX_TBL_LEN;
            RetVal = SVC_NG;
            PRN_CAL_LDC_LOG "Fail to set ldc table - input ldc vector(%d) is out-of range(%d)!"
                PRN_CAL_LDC_ARG_UINT32 pInput->LdcVectorLength PRN_CAL_LDC_ARG_POST
                PRN_CAL_LDC_ARG_UINT32 TmpU32                  PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_NG
        } else {

            AmbaSvcWrap_MisraMemset(pNandTbl, 0, sizeof(SVC_CALIB_LDC_NAND_TABLE_s));

            pNandTbl->Header.Version          = SVC_CALIB_LDC_VERSION;
            pNandTbl->Header.HorGridNum       = pInput->HorGridNum;
            pNandTbl->Header.VerGridNum       = pInput->VerGridNum;
            pNandTbl->Header.TileWidthExp     = pInput->TileWidthExp;
            pNandTbl->Header.TileHeightExp    = pInput->TileHeightExp;
            pNandTbl->Header.VinSelectBits    = pTblInfo->CalChan.VinSelectBits;
            pNandTbl->Header.SensorSelectBits = pTblInfo->CalChan.SensorSelectBits;
            pNandTbl->Header.FmtSelectBits    = pTblInfo->CalChan.FmtSelectBits;
            pNandTbl->Header.FovSelectBits    = pTblInfo->CalChan.FovSelectBits;
            AmbaSvcWrap_MisraMemcpy(&(pNandTbl->Header.VinSensorGeo), &(pInput->VinSensorGeo), sizeof(SVC_CALIB_VIN_SENSOR_GEO_s));
            AmbaSvcWrap_MisraMemcpy(pNandTbl->LdcVector, pInput->pLdcVector, sizeof(AMBA_IK_GRID_POINT_s)*(pInput->LdcVectorLength));

            // Update nand header
            if (pNandHeader->Enable == 0U) {
                pNandHeader->Enable = 1U;
            }
            if (pNandHeader->DebugMsgOn == 0U) {
                pNandHeader->DebugMsgOn = SVC_CalibLdcCtrl.EnableDebugMsg;
            }
            if (pNandHeader->DebugMsgOn == 0U) {
                pNandHeader->DebugMsgOn = pTblInfo->DbgMsgOn;
            }
            if (SVC_CalibLdcCtrl.EnableDebugMsg == 0U) {
                SVC_CalibLdcCtrl.EnableDebugMsg = pTblInfo->DbgMsgOn;
            }
            if ((pNandHeader->TableSelectBits & SvcCalib_BitGet(TblID)) == 0U) {
                pNandHeader->TableSelectBits |= SvcCalib_BitGet(TblID);
                pNandHeader->NumOfTable += 1U;
            }

            /* Update to NAND */
            RetVal = SvcCalib_DataSave(SVC_CALIB_LDC_ID);
            if (RetVal != OK) {
                PRN_CAL_LDC_LOG "Fail to save ldc to nand!" PRN_CAL_LDC_NG
            }

            AmbaMisra_TouchUnused(pInput);
            AmbaMisra_TouchUnused(pTblInfo);
        }

        SvcCalib_LdcMutexGive(&(SVC_CalibLdcCtrl.Mutex));

    }

    return RetVal;
}


static UINT32 SvcCalib_LdcTableDataGet(UINT32 TblID, SVC_CALIB_TBL_INFO_s *pTblInfo)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibLdcCtrlFlag & SVC_CALIB_LDC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to get ldc table - Need to initial ldc control module first!" PRN_CAL_LDC_NG
    } else if ((SVC_CalibLdcCtrlFlag & SVC_CALIB_LDC_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to get ldc table - Need to initial ldc dram shadow first!" PRN_CAL_LDC_NG
    } else if (SVC_CalibLdcCtrl.pNandTable == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to get ldc table - invalid nand table!" PRN_CAL_LDC_NG
    } else if (SVC_CalibLdcCtrl.pNandHeader->NumOfTable == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "There is not ldc table!" PRN_CAL_LDC_DBG
    } else if (pTblInfo == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to get ldc table - invalid table info!" PRN_CAL_LDC_NG
    } else if (pTblInfo->pBuf == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to get ldc table - invalid table!" PRN_CAL_LDC_NG
    } else if (pTblInfo->BufSize < (UINT32)sizeof(SVC_CALIB_LDC_TBL_DATA_s)) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to get ldc table - output table size(0x%X) is too small to service ldc table size(0x%X)!"
            PRN_CAL_LDC_ARG_UINT32 pTblInfo->BufSize                        PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_ARG_UINT32 (UINT32)sizeof(SVC_CALIB_LDC_TBL_DATA_s) PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_NG
    } else if (KAL_ERR_NONE != SvcCalib_LdcMutexTake(&(SVC_CalibLdcCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to get ldc table - take mutex fail!" PRN_CAL_LDC_NG
    } else {
        UINT32 CurTblID = 0xFFFFFFFFU;
        SVC_CALIB_LDC_TBL_DATA_s         *pOutput = NULL;
        const SVC_CALIB_LDC_NAND_TABLE_s *pNandTbl;

        AmbaMisra_TypeCast(&(pOutput), &(pTblInfo->pBuf));
        if (TblID == 255U) {
            if (SVC_CalibLdcCtrl.CalUpdCtrl == SVC_CALIB_LDC_UPD_BY_FMT_FOV) {
                if (0U != SvcCalib_LdcNandTableSearch(pTblInfo->CalChan.FmtID, pTblInfo->CalChan.FovID, &CurTblID)) {
                    CurTblID = 0xFFFFFFFFU;
                }
            } else {
                if (0U != SvcCalib_LdcNandTableSearch(pTblInfo->CalChan.VinID, pTblInfo->CalChan.SensorID, &CurTblID)) {
                    CurTblID = 0xFFFFFFFFU;
                }
            }
        } else {
            CurTblID = TblID;
        }

        if (CurTblID >= SVC_CalibLdcCtrl.NumOfLdcTable) {
            UINT32 TempU32 = (SVC_CalibLdcCtrl.NumOfLdcTable == 0U)?0U:(SVC_CalibLdcCtrl.NumOfLdcTable - 1U);

            RetVal = SVC_NG;
            PRN_CAL_LDC_LOG "Fail to get ldc table - the TableID(0x%X) is out-of range(0 ~ %d)"
                PRN_CAL_LDC_ARG_UINT32 CurTblID PRN_CAL_LDC_ARG_POST
                PRN_CAL_LDC_ARG_UINT32 TempU32  PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_NG
        } else {
            pNandTbl = &(SVC_CalibLdcCtrl.pNandTable[CurTblID]);

            if (pOutput->pLdcVector == NULL) {
                RetVal = SVC_NG;
                PRN_CAL_LDC_LOG "Fail to get ldc table - invalid output ldc table address!" PRN_CAL_LDC_NG
            } else if (pOutput->LdcVectorLength < SVC_CALIB_LDC_MAX_TBL_LEN) {
                UINT32 TempU32 = SVC_CALIB_LDC_MAX_TBL_LEN;
                RetVal = SVC_NG;
                PRN_CAL_LDC_LOG "Fail to get ldc table - output ldc table length(%d) is too small to service cur ldc table(%d)!"
                    PRN_CAL_LDC_ARG_UINT32 pOutput->LdcVectorLength PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_ARG_UINT32 TempU32                  PRN_CAL_LDC_ARG_POST
                PRN_CAL_LDC_NG
            } else {
                UINT32 CurTblLen       = pNandTbl->Header.HorGridNum * pNandTbl->Header.VerGridNum;
                pOutput->HorGridNum    = pNandTbl->Header.HorGridNum;
                pOutput->VerGridNum    = pNandTbl->Header.VerGridNum;
                pOutput->TileWidthExp  = pNandTbl->Header.TileWidthExp;
                pOutput->TileHeightExp = pNandTbl->Header.TileHeightExp;
                AmbaSvcWrap_MisraMemcpy(&(pOutput->VinSensorGeo), &(pNandTbl->Header.VinSensorGeo), sizeof(SVC_CALIB_VIN_SENSOR_GEO_s));
                AmbaSvcWrap_MisraMemset(pOutput->pLdcVector, 0, sizeof(AMBA_IK_GRID_POINT_s)*pOutput->LdcVectorLength);
                AmbaSvcWrap_MisraMemcpy(pOutput->pLdcVector, pNandTbl->LdcVector, sizeof(AMBA_IK_GRID_POINT_s)*CurTblLen);

                pTblInfo->CalChan.VinSelectBits    = pNandTbl->Header.VinSelectBits;
                pTblInfo->CalChan.SensorSelectBits = pNandTbl->Header.SensorSelectBits;
                pTblInfo->CalChan.FmtSelectBits    = pNandTbl->Header.FmtSelectBits;
                pTblInfo->CalChan.FovSelectBits    = pNandTbl->Header.FovSelectBits;

                if (SVC_CalibLdcCtrl.CalUpdCtrl == SVC_CALIB_LDC_UPD_BY_FMT_FOV) {
                    PRN_CAL_LDC_LOG "Success to get ldc table from TableID(%d), FmtSelectBits(0x%X) FovSelectBits(0x%X)"
                        PRN_CAL_LDC_ARG_UINT32 CurTblID                        PRN_CAL_LDC_ARG_POST
                        PRN_CAL_LDC_ARG_UINT32 pTblInfo->CalChan.FmtSelectBits PRN_CAL_LDC_ARG_POST
                        PRN_CAL_LDC_ARG_UINT32 pTblInfo->CalChan.FovSelectBits PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_DBG
                } else {
                    PRN_CAL_LDC_LOG "Success to get ldc table from TableID(%d), VinSelectBits(0x%X) SensorSelectBits(0x%X)"
                        PRN_CAL_LDC_ARG_UINT32 CurTblID                           PRN_CAL_LDC_ARG_POST
                        PRN_CAL_LDC_ARG_UINT32 pTblInfo->CalChan.VinSelectBits    PRN_CAL_LDC_ARG_POST
                        PRN_CAL_LDC_ARG_UINT32 pTblInfo->CalChan.SensorSelectBits PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_DBG
                }
            }
        }

        SvcCalib_LdcMutexGive(&(SVC_CalibLdcCtrl.Mutex));

    }

    return RetVal;
}

static void SvcCalib_LdcShellStrToU32(const char *pString, UINT32 *pValue)
{
    UINT32 ErrCode = AmbaUtility_StringToUInt32(pString, pValue);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }
}

static UINT32 SvcCalib_LdcShellCfgCalChn(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibLdcCtrlFlag & SVC_CALIB_LDC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to cfg ldc vin/sensor id - initial ldc module first!" PRN_CAL_LDC_NG
    } else if (ArgCount < 7U) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to cfg ldc vin/sensor id - Argc should >= 8" PRN_CAL_LDC_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to cfg ldc vin/sensor id - Argv should not null!" PRN_CAL_LDC_NG
    } else {
        UINT32 VinID            = 0xFFU;
        UINT32 SensorID         = 0xFFU;
        UINT32 VinSelectBits    = 0U;
        UINT32 SensorSelectBits = 0U;
        UINT32 FmtSelectBits    = 0U;
        UINT32 FovSelectBits    = 0U;

        SvcCalib_LdcShellStrToU32(pArgVector[3U], &VinID           );
        SvcCalib_LdcShellStrToU32(pArgVector[4U], &SensorID        );
        SvcCalib_LdcShellStrToU32(pArgVector[5U], &VinSelectBits   );
        SvcCalib_LdcShellStrToU32(pArgVector[6U], &SensorSelectBits);
        if (ArgCount >= 9U) {
            SvcCalib_LdcShellStrToU32(pArgVector[7U], &FmtSelectBits   );
            SvcCalib_LdcShellStrToU32(pArgVector[8U], &FovSelectBits   );
        }
        AmbaMisra_TouchUnused(&VinID);
        AmbaMisra_TouchUnused(&SensorID);

        /* Initial the ldc calculate parameters */
        AmbaSvcWrap_MisraMemset(&(SVC_CalibLdcCtrl.CalcLdcCtrl), 0, sizeof(SVC_CALIB_LDC_CALCULATE_CTRL_s));
        SVC_CalibLdcCtrlFlag &= ~( SVC_CALIB_LDC_FLG_CALC_INIT | SVC_CALIB_LDC_FLG_CALC_VIN_SEN );

        SVC_CalibLdcCtrl.CalcLdcCtrl.SaveBinOn                    = 1U;
        SVC_CalibLdcCtrl.CalcLdcCtrl.TableIdx                     = 0xFFFFFFFFU;
        SVC_CalibLdcCtrlFlag |= SVC_CALIB_LDC_FLG_CALC_INIT;

        PRN_CAL_LDC_LOG " " PRN_CAL_LDC_API
        PRN_CAL_LDC_LOG "------ Calibration LDC initial setting ------" PRN_CAL_LDC_API
        PRN_CAL_LDC_LOG "  SaveBinOn                    : %d"
            PRN_CAL_LDC_ARG_UINT32 SVC_CalibLdcCtrl.CalcLdcCtrl.SaveBinOn PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_API
        PRN_CAL_LDC_LOG "  TableIdx                     : 0x%X"
            PRN_CAL_LDC_ARG_UINT32 SVC_CalibLdcCtrl.CalcLdcCtrl.TableIdx PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_API

        /* Configure the calib channel */
        AmbaSvcWrap_MisraMemset(&(SVC_CalibLdcCtrl.CalibChan), 0, sizeof(SVC_CALIB_CHANNEL_s));
        SVC_CalibLdcCtrl.CalibChan.VinSelectBits    = VinSelectBits   ;
        SVC_CalibLdcCtrl.CalibChan.SensorSelectBits = SensorSelectBits;
        SVC_CalibLdcCtrl.CalibChan.FmtSelectBits    = FmtSelectBits;
        SVC_CalibLdcCtrl.CalibChan.FovSelectBits    = FovSelectBits;
        SVC_CalibLdcCtrlFlag |= SVC_CALIB_LDC_FLG_CALC_VIN_SEN;

        PRN_CAL_LDC_LOG " " PRN_CAL_LDC_API
        PRN_CAL_LDC_LOG "------ Calibration LDC Channel ------" PRN_CAL_LDC_API

        PRN_CAL_LDC_LOG "  VinSelectBits    : 0x%X"
            PRN_CAL_LDC_ARG_UINT32 SVC_CalibLdcCtrl.CalibChan.VinSelectBits PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_API
        PRN_CAL_LDC_LOG "  SensorSelectBits : 0x%X"
            PRN_CAL_LDC_ARG_UINT32 SVC_CalibLdcCtrl.CalibChan.SensorSelectBits PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_API
        PRN_CAL_LDC_LOG "  FmtSelectBits    : 0x%X"
            PRN_CAL_LDC_ARG_UINT32 SVC_CalibLdcCtrl.CalibChan.FmtSelectBits PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_API
        PRN_CAL_LDC_LOG "  FovSelectBits    : 0x%X"
            PRN_CAL_LDC_ARG_UINT32 SVC_CalibLdcCtrl.CalibChan.FovSelectBits PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_API
    }

    return RetVal;
}


static void SvcCalib_LdcShellCfgCalChnU(void)
{
    PRN_CAL_LDC_LOG "  %scfg_calib_chan%s        : reset all calibration setting as default"
        PRN_CAL_LDC_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_LDC_ARG_POST
    PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "  ------------------------------------------------------" PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [VinSelectBits]     : bit0 : Vin0, bit1 : Vin1, ... " PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [SensorSelectBits]  : bit0 : sensor0, bit1 : sensor1, ..." PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [FmtSelectBits]     : bit0 : format_0, bit1 : format_1, ... " PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [FovSelectBits]     : bit0 : fov_0, bit1 : fov_1, ... " PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG " " PRN_CAL_LDC_API
}


static UINT32 SvcCalib_LdcShellCfgTblID(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibLdcCtrlFlag & SVC_CALIB_LDC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to cfg ldc table idx - initial ldc module first!" PRN_CAL_LDC_NG
    } else if (ArgCount < 3U) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to cfg ldc table idx - Argc should >= 3" PRN_CAL_LDC_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to cfg ldc table idx - Argv should not null!" PRN_CAL_LDC_NG
    } else {
        UINT32 TableIdx = 0U;

        SvcCalib_LdcShellStrToU32(pArgVector[3U],  &TableIdx );

        SVC_CalibLdcCtrl.CalcLdcCtrl.TableIdx = TableIdx;

        PRN_CAL_LDC_LOG " " PRN_CAL_LDC_API
        PRN_CAL_LDC_LOG "------ Calibration LDC Table Index %d ------"
            PRN_CAL_LDC_ARG_UINT32 SVC_CalibLdcCtrl.CalcLdcCtrl.TableIdx PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_API

    }

    return RetVal;
}


static void SvcCalib_LdcShellCfgTblIDU(void)
{
    PRN_CAL_LDC_LOG "  %scfg_table_id%s          :"
        PRN_CAL_LDC_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_LDC_ARG_POST
    PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "  ------------------------------------------------------" PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [Nand Table ID]     : the nand table id." PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "                        : the max value depend on each app" PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG " " PRN_CAL_LDC_API
}


static UINT32 SvcCalib_LdcShellCfgDbgMsg(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibLdcCtrlFlag & SVC_CALIB_LDC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to switch ldc debug msg - initial ldc module first!" PRN_CAL_LDC_NG
    } else if (ArgCount < 3U) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to switch ldc debug msg - Argc should >= 3" PRN_CAL_LDC_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to switch ldc debug msg - Argv should not null!" PRN_CAL_LDC_NG
    } else {
        UINT32 DbgMsgOn = 0U;

        SvcCalib_LdcShellStrToU32(pArgVector[3U],  &DbgMsgOn );

        if ((DbgMsgOn & 0x1U) > 0U) {
            SVC_CalibLdcCtrl.EnableDebugMsg = DbgMsgOn;
        }

        if ((DbgMsgOn & 0x2U) > 0U) {
            SVC_CalibLdcCtrlFlag |= SVC_LOG_CAL_DBG;
        }

        PRN_CAL_LDC_LOG " " PRN_CAL_LDC_API
        PRN_CAL_LDC_LOG "------ Calibration LDC Debug Msg On/Off %d, lvl %d ------"
            PRN_CAL_LDC_ARG_UINT32 SVC_CalibLdcCtrl.EnableDebugMsg PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_ARG_UINT32 DbgMsgOn                        PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_API

    }

    return RetVal;
}


static void SvcCalib_LdcShellCfgDbgMsgU(void)
{
    PRN_CAL_LDC_LOG "  %scfg_dbg_msg%s           :"
        PRN_CAL_LDC_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_LDC_ARG_POST
    PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "  ------------------------------------------------------" PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [Enable]            : 0: disable debug message" PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "                        : 1: enable debug message" PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "                        : 2: enable debug message level 2" PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG " " PRN_CAL_LDC_API
}


static UINT32 SvcCalib_LdcShellGenTbl(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibLdcCtrlFlag & SVC_CALIB_LDC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to proc ldc 'gen_table' - initial ldc module first!" PRN_CAL_LDC_NG
    } else if (ArgCount < 3U) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to proc ldc 'gen_table' - Argc should >= 3" PRN_CAL_LDC_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to proc ldc 'gen_table' - Argv should not null!" PRN_CAL_LDC_NG
    } else {
        RetVal = SvcCalib_LdcTableGenTbl(&(SVC_CalibLdcCtrl.CalibChan), pArgVector[3U]);
    }

    return RetVal;
}


static void SvcCalib_LdcShellGenTblU(void)
{
    PRN_CAL_LDC_LOG "  %sgen_table%s             : start calculate LDC table and save to NVM"
        PRN_CAL_LDC_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_LDC_ARG_POST
    PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "  ------------------------------------------------------" PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [Script]            : LDC ctuner script path." PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG " " PRN_CAL_LDC_API
}


static UINT32 SvcCalib_LdcShellEnable(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibLdcCtrlFlag & SVC_CALIB_LDC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to enable/disable ldc - initial ldc module first!" PRN_CAL_LDC_NG
    } else if (ArgCount <= 4U) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to enable/disable ldc - Argc should > 4" PRN_CAL_LDC_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to enable/disable ldc - Argv should not null!" PRN_CAL_LDC_NG
    } else {
        UINT32 IkContextID = 0xFFFFFFFFU, Enable = 0U;

        SvcCalib_LdcShellStrToU32(pArgVector[3U],  &IkContextID );
        SvcCalib_LdcShellStrToU32(pArgVector[4U],  &Enable      );
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
            PRetVal = AmbaIK_GetWarpEnb(&ImgMode, &CurIkState);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_CAL_LDC_LOG "Fail to enable/disable ldc - get ldc enable/disable state fail" PRN_CAL_LDC_NG
            } else {
                if (CurIkState != Enable) {
                    PRetVal = AmbaIK_SetWarpEnb(&ImgMode, Enable);
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;

                        PRN_CAL_LDC_LOG "Fail to enable/disable ldc - %s ldc fail!"
                            PRN_CAL_LDC_ARG_CSTR   ( (Enable > 0U) ? ( "enable" ) : ( "disable" ) ) PRN_CAL_LDC_ARG_POST
                        PRN_CAL_LDC_NG
                    } else {
                        PRN_CAL_LDC_LOG "%s ldc done!"
                            PRN_CAL_LDC_ARG_CSTR   ( (Enable > 0U) ? ( "enable" ) : ( "disable" ) ) PRN_CAL_LDC_ARG_POST
                        PRN_CAL_LDC_API
                    }
                } else {
                    PRN_CAL_LDC_LOG "current IK LDC state: %s!"
                        PRN_CAL_LDC_ARG_CSTR   ( (CurIkState > 0U) ? ( "enable" ) : ( "disable" ) ) PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                }
            }
        } else {
            RetVal = SVC_NG;
            PRN_CAL_LDC_LOG "Fail to enable/disable ldc - invalid ik context id(%d)"
                PRN_CAL_LDC_ARG_UINT32 IkContextID PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_NG
        }
    }

    return RetVal;
}


static void SvcCalib_LdcShellEnableU(void)
{
    PRN_CAL_LDC_LOG "  %senable%s                : enable/disable LDC effect"
        PRN_CAL_LDC_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_LDC_ARG_POST
    PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "  ------------------------------------------------------" PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [IK Context ID]     : image kernel context id." PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [Enable]            : 0 => disable" PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "                        : 1 => enable" PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG " " PRN_CAL_LDC_API
}


static UINT32 SvcCalib_LdcShellUpd(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibLdcCtrlFlag & SVC_CALIB_LDC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to proc ldc->upd cmd - initial ldc module first!" PRN_CAL_LDC_NG
    } else if (ArgCount <= 5U) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to proc ldc->upd cmd - Argc should > 5" PRN_CAL_LDC_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to proc ldc->upd cmd - Argv should not null!" PRN_CAL_LDC_NG
    } else {
        UINT32 VinID       = 0xFFFFFFFFU;
        UINT32 SensorID    = 0xFFFFFFFFU;
        UINT32 IkContextID = 0xFFFFFFFFU;

        SvcCalib_LdcShellStrToU32(pArgVector[3U],  &VinID       );
        SvcCalib_LdcShellStrToU32(pArgVector[4U],  &SensorID    );
        SvcCalib_LdcShellStrToU32(pArgVector[5U],  &IkContextID );

        if (SVC_OK != SvcCalib_LdcVinSensorIDCheck(VinID, SensorID)) {
            RetVal = SVC_NG;
            PRN_CAL_LDC_LOG "Fail to proc ldc->upd cmd - invalid VinID(%d), SensorID(%d)!"
                PRN_CAL_LDC_ARG_UINT32 VinID    PRN_CAL_LDC_ARG_POST
                PRN_CAL_LDC_ARG_UINT32 SensorID PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_NG
        } else if (IkContextID == 0xFFFFFFFFU) {
            RetVal = SVC_NG;
            PRN_CAL_LDC_LOG "Fail to proc ldc->upd cmd - invalid IKContextID(%d)!"
                PRN_CAL_LDC_ARG_UINT32 IkContextID PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_NG
        } else {
            UINT32 CurIkEnable = 0U;
            AMBA_IK_MODE_CFG_s ImgMode;

            AmbaSvcWrap_MisraMemset(&ImgMode, 0, sizeof(ImgMode));
            ImgMode.ContextId = IkContextID;
            PRetVal = AmbaIK_GetWarpEnb(&ImgMode, &CurIkEnable);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_CAL_LDC_LOG "Fail to proc ldc->upd cmd - get ldc enable/disable state fail! ErrCode: 0x%x"
                    PRN_CAL_LDC_ARG_UINT32 PRetVal PRN_CAL_LDC_ARG_POST
                PRN_CAL_LDC_NG
            } else {

                // If the ldc has been enable, disable it before update ldc table
                if (CurIkEnable > 0U) {
                    PRetVal = AmbaIK_SetWarpEnb(&ImgMode, 0U);
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;
                        PRN_CAL_LDC_LOG "Fail to proc ldc->upd cmd - disable ldc failure! ErrCode: 0x%x"
                            PRN_CAL_LDC_ARG_UINT32 PRetVal PRN_CAL_LDC_ARG_POST
                        PRN_CAL_LDC_NG
                    }
                }

                if (RetVal == SVC_OK) {
                    UINT32 WorkTblIdx = 0xFFFFFFFFU;
                    SVC_CALIB_CHANNEL_s CalibChan;

                    // If working buffer table exist, reset it first
                    PRetVal = SvcCalib_LdcTableSearch(VinID, SensorID, &WorkTblIdx); PRN_CAL_LDC_ERR_HDLR
                    if (WorkTblIdx < SVC_CalibLdcCtrl.NumOfLdcTable) {
                        AmbaSvcWrap_MisraMemset(&(SVC_CalibLdcCtrl.pLdcTable[WorkTblIdx]), 0, sizeof(SVC_CALIB_LDC_TABLE_s));
                    }

                    // Update the calib table from nand
                    AmbaSvcWrap_MisraMemset(&CalibChan, 0, sizeof(CalibChan));
                    CalibChan.VinID    = VinID;
                    CalibChan.SensorID = SensorID;
                    RetVal = SvcCalib_LdcTableUpdate(&CalibChan, &ImgMode);
                    if (RetVal != 0U) {
                        PRN_CAL_LDC_LOG "Fail to proc ldc->upd cmd - update calib table fail! VinID(%d), SensorID(%d), IkContextID(%d)"
                            PRN_CAL_LDC_ARG_UINT32 VinID       PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_ARG_UINT32 SensorID    PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_ARG_UINT32 IkContextID PRN_CAL_LDC_ARG_POST
                        PRN_CAL_LDC_NG
                    }
                }
            }
        }
    }

    return RetVal;
}

static void SvcCalib_LdcShellUpdU(void)
{
    PRN_CAL_LDC_LOG "  %supd%s                   : update LDC table"
        PRN_CAL_LDC_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_LDC_ARG_POST
    PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "  ------------------------------------------------------" PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [VinID]             : vin identify number." PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [SensorID]          : sensor identify number. 1/2/4/8" PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [IK Context ID]     : image kernel context id." PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG " " PRN_CAL_LDC_API
}

static UINT32 SvcCalib_LdcShellSet(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibLdcCtrlFlag & SVC_CALIB_LDC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to proc ldc->upd cmd - initial ldc module first!" PRN_CAL_LDC_NG
    } else if ((SVC_CalibLdcCtrlFlag & SVC_CALIB_LDC_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to proc ldc->set cmd - initial ldc shadow first!" PRN_CAL_LDC_NG
    } else if (SVC_CalibLdcCtrl.pNandTable == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to proc ldc->set cmd - invalid ldc nand table!" PRN_CAL_LDC_NG
    } else if (ArgCount <= 20U) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to proc ldc->set cmd - Argc should > 20" PRN_CAL_LDC_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to proc ldc->set cmd - Argv should not null!" PRN_CAL_LDC_NG
    } else {
        UINT32 TblID = 0xFFU, ArgIdx = 3U;

        SvcCalib_LdcShellStrToU32(pArgVector[ArgIdx],  &TblID); ArgIdx ++;
        if (TblID >= SVC_CalibLdcCtrl.NumOfLdcTable) {
            RetVal = SVC_NG;
            PRN_CAL_LDC_LOG "Fail to proc ldc->set cmd - insert table id(%d) is out-of range(%d)!"
                PRN_CAL_LDC_ARG_UINT32 TblID                       PRN_CAL_LDC_ARG_POST
                PRN_CAL_LDC_ARG_UINT32 SVC_CalibLdcCtrl.NumOfLdcTable PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_NG
        } else {
            SVC_CALIB_LDC_NAND_TABLE_s CurLdcTbl;
            void  *pLdcBuf = CurLdcTbl.LdcVector;
            UINT32 LdcBufSize;
            const char *pFilePath = NULL;
            SVC_CALIB_LDC_NAND_TABLE_s *pNandTable = &(SVC_CalibLdcCtrl.pNandTable[TblID]);
            AMBA_FS_FILE_INFO_s FileInfo;
            UINT32 DbgMsgOn = 1U;

            AmbaSvcWrap_MisraMemset(&CurLdcTbl, 0, sizeof(SVC_CALIB_LDC_NAND_TABLE_s));
            CurLdcTbl.Header.Version      = SVC_CALIB_LDC_VERSION;
            CurLdcTbl.Header.TwoStageDone = 0U;

            SvcCalib_LdcShellStrToU32(pArgVector[ArgIdx], &(CurLdcTbl.Header.VinSelectBits   )); ArgIdx++;
            SvcCalib_LdcShellStrToU32(pArgVector[ArgIdx], &(CurLdcTbl.Header.SensorSelectBits)); ArgIdx++;

            SvcCalib_LdcShellStrToU32(pArgVector[ArgIdx], &(CurLdcTbl.Header.FmtSelectBits   )); ArgIdx++;
            SvcCalib_LdcShellStrToU32(pArgVector[ArgIdx], &(CurLdcTbl.Header.FovSelectBits   )); ArgIdx++;

            SvcCalib_LdcShellStrToU32(pArgVector[ArgIdx], &(CurLdcTbl.Header.VinSensorGeo.StartX              )); ArgIdx++;
            SvcCalib_LdcShellStrToU32(pArgVector[ArgIdx], &(CurLdcTbl.Header.VinSensorGeo.StartY              )); ArgIdx++;
            SvcCalib_LdcShellStrToU32(pArgVector[ArgIdx], &(CurLdcTbl.Header.VinSensorGeo.Width               )); ArgIdx++;
            SvcCalib_LdcShellStrToU32(pArgVector[ArgIdx], &(CurLdcTbl.Header.VinSensorGeo.Height              )); ArgIdx++;
            SvcCalib_LdcShellStrToU32(pArgVector[ArgIdx], &(CurLdcTbl.Header.VinSensorGeo.HSubSample.FactorDen)); ArgIdx++;
            SvcCalib_LdcShellStrToU32(pArgVector[ArgIdx], &(CurLdcTbl.Header.VinSensorGeo.HSubSample.FactorNum)); ArgIdx++;
            SvcCalib_LdcShellStrToU32(pArgVector[ArgIdx], &(CurLdcTbl.Header.VinSensorGeo.VSubSample.FactorDen)); ArgIdx++;
            SvcCalib_LdcShellStrToU32(pArgVector[ArgIdx], &(CurLdcTbl.Header.VinSensorGeo.VSubSample.FactorNum)); ArgIdx++;

            SvcCalib_LdcShellStrToU32(pArgVector[ArgIdx], &(CurLdcTbl.Header.HorGridNum   )); ArgIdx++;
            SvcCalib_LdcShellStrToU32(pArgVector[ArgIdx], &(CurLdcTbl.Header.VerGridNum   )); ArgIdx++;
            SvcCalib_LdcShellStrToU32(pArgVector[ArgIdx], &(CurLdcTbl.Header.TileWidthExp )); ArgIdx++;
            SvcCalib_LdcShellStrToU32(pArgVector[ArgIdx], &(CurLdcTbl.Header.TileHeightExp)); ArgIdx++;

            pFilePath = pArgVector[ArgIdx]; ArgIdx++;
            SvcCalib_LdcShellStrToU32(pArgVector[ArgIdx], &DbgMsgOn);

            LdcBufSize = ((UINT32)sizeof(AMBA_IK_GRID_POINT_s)) * ( CurLdcTbl.Header.HorGridNum * CurLdcTbl.Header.VerGridNum );

            AmbaSvcWrap_MisraMemset(&FileInfo, 0, sizeof(FileInfo));
            PRetVal = AmbaFS_GetFileInfo(pFilePath, &FileInfo);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_CAL_LDC_LOG "Fail to proc ldc->set cmd - get file info fail! ErrCode(0x%08X) '%s'"
                    PRN_CAL_LDC_ARG_UINT32 PRetVal   PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_ARG_CSTR   pFilePath PRN_CAL_LDC_ARG_POST
                PRN_CAL_LDC_NG
            } else if ((UINT32)(FileInfo.Size) != LdcBufSize) {
                RetVal = SVC_NG;
                PRN_CAL_LDC_LOG "Fail to proc ldc->set cmd - file size(0x%08X) does not match Hor(%d) x Ver(%d) x Grid(%d)!"
                    PRN_CAL_LDC_ARG_UINT32 FileInfo.Size PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_ARG_UINT32 CurLdcTbl.Header.HorGridNum  PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_ARG_UINT32 CurLdcTbl.Header.VerGridNum  PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_ARG_UINT32 sizeof(AMBA_IK_GRID_POINT_s) PRN_CAL_LDC_ARG_POST
                PRN_CAL_LDC_NG
            } else {
                AMBA_FS_FILE *pFile = NULL;

                PRetVal = AmbaFS_FileOpen(pFilePath, "rb", &pFile);
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    PRN_CAL_LDC_LOG "Fail to proc ldc->set cmd - open file fail! ErrCode(0x%08X)"
                        PRN_CAL_LDC_ARG_UINT32 PRetVal PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_NG
                } else if (pFile == NULL) {
                    RetVal = SVC_NG;
                    PRN_CAL_LDC_LOG "Fail to proc ldc->set cmd - invalid file pointer" PRN_CAL_LDC_NG
                } else {
                    UINT32 NumSuccess = 0U;
                    PRetVal = AmbaFS_FileRead(pLdcBuf, 1, LdcBufSize, pFile, &NumSuccess);
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;
                        PRN_CAL_LDC_LOG "Fail to proc ldc->set cmd - read data fail! ErrCode(0x%08X)"
                            PRN_CAL_LDC_ARG_UINT32 PRetVal PRN_CAL_LDC_ARG_POST
                        PRN_CAL_LDC_NG
                    } else if (NumSuccess != LdcBufSize) {
                        RetVal = SVC_NG;
                        PRN_CAL_LDC_LOG "Fail to proc ldc->set cmd - read size(0x%X) does not same with request size(0x%X) fail!"
                            PRN_CAL_LDC_ARG_UINT32 NumSuccess PRN_CAL_LDC_ARG_POST
                            PRN_CAL_LDC_ARG_UINT32 LdcBufSize PRN_CAL_LDC_ARG_POST
                        PRN_CAL_LDC_NG
                    } else {
                        // misra-c
                    }

                    PRetVal = AmbaFS_FileClose(pFile); PRN_CAL_LDC_ERR_HDLR
                }
            }

            if (RetVal == 0U) {
                if (KAL_ERR_NONE != SvcCalib_LdcMutexTake(&(SVC_CalibLdcCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
                    RetVal = SVC_NG;
                    PRN_CAL_LDC_LOG "Fail to proc ldc->set cmd - take mutex fail!" PRN_CAL_LDC_NG
                } else {

                    AmbaSvcWrap_MisraMemcpy(pNandTable, &CurLdcTbl, sizeof(SVC_CALIB_LDC_NAND_TABLE_s));

                    SVC_CalibLdcCtrl.pNandHeader->Enable = 1U;
                    SVC_CalibLdcCtrl.pNandHeader->DebugMsgOn = DbgMsgOn;
                    if ((SVC_CalibLdcCtrl.pNandHeader->TableSelectBits & SvcCalib_BitGet(TblID)) == 0U) {
                        SVC_CalibLdcCtrl.pNandHeader->TableSelectBits |= SvcCalib_BitGet(TblID);
                        SVC_CalibLdcCtrl.pNandHeader->NumOfTable ++;
                    }

                    PRN_CAL_LDC_LOG " " PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "====== Calibration ldc info ======" PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  Version                             : 0x%X"
                        PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.Version PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  HorGridNum                          : %d"
                        PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.HorGridNum PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  VerGridNum                          : %d"
                        PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.VerGridNum PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  TileWidthExp                        : %d"
                        PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.TileWidthExp PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  TileHeightExp                       : %d"
                        PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.TileHeightExp PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  CalibSensorGeo.StartX               : %d"
                        PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.VinSensorGeo.StartX PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  CalibSensorGeo.StartY               : %d"
                        PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.VinSensorGeo.StartY PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  CalibSensorGeo.Width                : %d"
                        PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.VinSensorGeo.Width PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  CalibSensorGeo.Height               : %d"
                        PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.VinSensorGeo.Height PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  CalibSensorGeo.HSubSample.FactorDen : %d"
                        PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.VinSensorGeo.HSubSample.FactorDen PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  CalibSensorGeo.HSubSample.FactorNum : %d"
                        PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.VinSensorGeo.HSubSample.FactorNum PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  CalibSensorGeo.VSubSample.FactorDen : %d"
                        PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.VinSensorGeo.VSubSample.FactorDen PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  CalibSensorGeo.VSubSample.FactorNum : %d"
                        PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.VinSensorGeo.VSubSample.FactorNum PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  TwoStageDone                        : %d"
                        PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.TwoStageDone PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  VinSelectBits                       : 0x%x"
                        PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.VinSelectBits PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  SensorSelectBits                    : 0x%x"
                        PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.SensorSelectBits PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  FmtSelectBits                       : 0x%x"
                        PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.FmtSelectBits PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  FovSelectBits                       : 0x%x"
                        PRN_CAL_LDC_ARG_UINT32 pNandTable->Header.FovSelectBits PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API
                    PRN_CAL_LDC_LOG "  WarpVector                          : %p"
                        PRN_CAL_LDC_ARG_CPOINT pNandTable->LdcVector PRN_CAL_LDC_ARG_POST
                    PRN_CAL_LDC_API

                    /* Update to NAND */
                    RetVal = SvcCalib_DataSave(SVC_CALIB_LDC_ID);
                    if (RetVal != OK) {
                        PRN_CAL_LDC_LOG "Fail to save ldc to nand!" PRN_CAL_LDC_NG
                    }

                    SvcCalib_LdcMutexGive(&(SVC_CalibLdcCtrl.Mutex));
                }
            }
        }
    }

    return RetVal;
}

static void SvcCalib_LdcShellSetU(void)
{
    PRN_CAL_LDC_LOG "  %sset%s                   : set ldc table manually"
        PRN_CAL_LDC_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_LDC_ARG_POST
    PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "  ------------------------------------------------------" PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [TblID]                 : inser table index" PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [VinSelectBits]         : the table supported vin select bits" PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [SensorSelectBits]      : the table supported sensor select bits" PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [FmtSelectBits]         : the table supported format select bits" PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [FovSelectBits]         : the table supported fov select bits" PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [Calib StartX]          : calibration geometric StartX              " PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [Calib StartY]          : calibration geometric StartY              " PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [Calib Width]           : calibration geometric Width               " PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [Calib Height]          : calibration geometric Height              " PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [Calib HbSampleDen]     : calibration geometric HSubSample.FactorDen" PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [Calib HbSampleNum]     : calibration geometric HSubSample.FactorNum" PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [Calib VbSampleDen]     : calibration geometric VSubSample.FactorDen" PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [Calib VbSampleNum]     : calibration geometric VSubSample.FactorNum" PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [Ldc HorGridNum]        : Ldc horizontal grid number" PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [Ldc VerGridNum]        : Ldc vertical grid number" PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [Ldc TileWidthExp]      : Ldc tile width exponent" PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [Ldc TileHeightExp]     : Ldc tile height exponent" PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [Ldc File Path]         : Ldc file path" PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "    [DebugMsgOn]            : configure debug msg" PRN_CAL_LDC_API

    PRN_CAL_LDC_LOG " " PRN_CAL_LDC_API

}

static void SvcCalib_LdcShellEntryInit(void)
{
    if ((SVC_CalibLdcCtrlFlag & SVC_CALIB_LDC_FLG_SHELL_INIT) == 0U) {
        UINT32 ShellNum = 0U;

        AmbaSvcWrap_MisraMemset(SVC_CalibLdcShellFunc, 0, sizeof(SVC_CalibLdcShellFunc));

        SVC_CalibLdcShellFunc[ShellNum] = (SVC_CALIB_LDC_SHELL_FUNC_s) { 1U, "cfg_calib_chan",  SvcCalib_LdcShellCfgCalChn,  SvcCalib_LdcShellCfgCalChnU  }; ShellNum ++;
        SVC_CalibLdcShellFunc[ShellNum] = (SVC_CALIB_LDC_SHELL_FUNC_s) { 1U, "cfg_table_id",    SvcCalib_LdcShellCfgTblID,   SvcCalib_LdcShellCfgTblIDU   }; ShellNum ++;
        SVC_CalibLdcShellFunc[ShellNum] = (SVC_CALIB_LDC_SHELL_FUNC_s) { 1U, "cfg_dbg_msg",     SvcCalib_LdcShellCfgDbgMsg,  SvcCalib_LdcShellCfgDbgMsgU  }; ShellNum ++;
        SVC_CalibLdcShellFunc[ShellNum] = (SVC_CALIB_LDC_SHELL_FUNC_s) { 1U, "gen_table",       SvcCalib_LdcShellGenTbl,     SvcCalib_LdcShellGenTblU     }; ShellNum ++;
        SVC_CalibLdcShellFunc[ShellNum] = (SVC_CALIB_LDC_SHELL_FUNC_s) { 1U, "enable",          SvcCalib_LdcShellEnable,     SvcCalib_LdcShellEnableU     }; ShellNum ++;
        SVC_CalibLdcShellFunc[ShellNum] = (SVC_CALIB_LDC_SHELL_FUNC_s) { 1U, "upd",             SvcCalib_LdcShellUpd,        SvcCalib_LdcShellUpdU        }; ShellNum ++;
        SVC_CalibLdcShellFunc[ShellNum] = (SVC_CALIB_LDC_SHELL_FUNC_s) { 1U, "set",             SvcCalib_LdcShellSet,        SvcCalib_LdcShellSetU        };

        SVC_CalibLdcCtrlFlag |= SVC_CALIB_LDC_FLG_SHELL_INIT;
    }
}


UINT32 SvcCalib_LdcShellFunc(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_NG;

    SvcCalib_LdcShellEntryInit();

    if(ArgCount == 0xCafeU) {
        UINT32 SIdx;

        PRN_CAL_LDC_LOG " " PRN_CAL_LDC_API
        PRN_CAL_LDC_LOG "==== %sShell Arguments Setting%s (%s%d%s) ===="
            PRN_CAL_LDC_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_ARG_CSTR   SVC_LOG_CAL_HL_NUM     PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_ARG_UINT32 ArgCount               PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_API

        for (SIdx = 0U; SIdx < ArgCount; SIdx ++) {
            PRN_CAL_LDC_LOG "  pArgVector[%s%d%s] : %s%s%s"
                PRN_CAL_LDC_ARG_CSTR   SVC_LOG_CAL_HL_NUM PRN_CAL_LDC_ARG_POST
                PRN_CAL_LDC_ARG_UINT32 SIdx               PRN_CAL_LDC_ARG_POST
                PRN_CAL_LDC_ARG_CSTR   SVC_LOG_CAL_HL_END PRN_CAL_LDC_ARG_POST
                PRN_CAL_LDC_ARG_CSTR   SVC_LOG_CAL_HL_STR PRN_CAL_LDC_ARG_POST
                PRN_CAL_LDC_ARG_CSTR   pArgVector[SIdx]   PRN_CAL_LDC_ARG_POST
                PRN_CAL_LDC_ARG_CSTR   SVC_LOG_CAL_HL_END PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_API
        }
    }

    if (ArgCount > 0U) {
        UINT32 ShellIdx;
        UINT32 ShellCount = (UINT32)(sizeof(SVC_CalibLdcShellFunc)) / (UINT32)(sizeof(SVC_CalibLdcShellFunc[0]));

        for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
            if ((SVC_CalibLdcShellFunc[ShellIdx].pFunc != NULL) && (SVC_CalibLdcShellFunc[ShellIdx].Enable > 0U)) {
                if (0 == SvcWrap_strcmp(pArgVector[2U], SVC_CalibLdcShellFunc[ShellIdx].ShellCmdName)) {
                    if (SVC_OK != (SVC_CalibLdcShellFunc[ShellIdx].pFunc)(ArgCount, pArgVector)) {
                        if (SVC_CalibLdcShellFunc[ShellIdx].pUsageFunc != NULL) {
                            (SVC_CalibLdcShellFunc[ShellIdx].pUsageFunc)();
                        }
                    }

                    RetVal = SVC_OK;
                    break;
                }
            }
        }

        if (RetVal != SVC_OK) {
            SvcCalib_LdcShellUsage();
        }
    }

    return RetVal;
}


static void SvcCalib_LdcShellUsage(void)
{
    UINT32 ShellIdx;
    UINT32 ShellCount = (UINT32)(sizeof(SVC_CalibLdcShellFunc)) / (UINT32)(sizeof(SVC_CalibLdcShellFunc[0]));

    PRN_CAL_LDC_LOG " " PRN_CAL_LDC_API
    PRN_CAL_LDC_LOG "====== %sLDC Command Usage%s ======"
        PRN_CAL_LDC_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_0 PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_LDC_ARG_POST
    PRN_CAL_LDC_API

    for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
        if ((SVC_CalibLdcShellFunc[ShellIdx].pFunc != NULL) && (SVC_CalibLdcShellFunc[ShellIdx].Enable > 0U)) {
            if (SVC_CalibLdcShellFunc[ShellIdx].pUsageFunc == NULL) {
                PRN_CAL_LDC_LOG "  %s"
                    PRN_CAL_LDC_ARG_CSTR   SVC_CalibLdcShellFunc[ShellIdx].ShellCmdName PRN_CAL_LDC_ARG_POST
                PRN_CAL_LDC_API
            } else {
                (SVC_CalibLdcShellFunc[ShellIdx].pUsageFunc)();
            }
        }
    }
}


static void SvcCalib_LdcCmdHdlrInit(void)
{
    if ((SVC_CalibLdcCtrlFlag & SVC_CALIB_LDC_FLG_CMD_INIT) == 0U) {

        AmbaSvcWrap_MisraMemset(SVC_CalibLdcCmdHdlr, 0, sizeof(SVC_CalibLdcCmdHdlr));

        SVC_CalibLdcCmdHdlr[SVC_CALIB_CMD_MEM_QUERY      ] = (SVC_CALIB_LDC_CMD_HDLR_s) {1, SVC_CALIB_CMD_MEM_QUERY,       SvcCalib_LdcCmdMemQuery   };
        SVC_CalibLdcCmdHdlr[SVC_CALIB_CMD_ITEM_CREATE    ] = (SVC_CALIB_LDC_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_CREATE,     SvcCalib_LdcCmdCreate     };
        SVC_CalibLdcCmdHdlr[SVC_CALIB_CMD_ITEM_DATA_INIT ] = (SVC_CALIB_LDC_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_DATA_INIT,  SvcCalib_LdcCmdDataInit   };
        SVC_CalibLdcCmdHdlr[SVC_CALIB_CMD_ITEM_TBL_CFG   ] = (SVC_CALIB_LDC_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_TBL_CFG,    SvcCalib_LdcCmdTblCfg     };
        SVC_CalibLdcCmdHdlr[SVC_CALIB_CMD_ITEM_TBL_UPDATE] = (SVC_CALIB_LDC_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_TBL_UPDATE, SvcCalib_LdcCmdTblUpdate  };
        SVC_CalibLdcCmdHdlr[SVC_CALIB_CMD_ITEM_TBL_SET   ] = (SVC_CALIB_LDC_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_TBL_SET,    SvcCalib_LdcCmdTblDataSet };
        SVC_CalibLdcCmdHdlr[SVC_CALIB_CMD_ITEM_TBL_GET   ] = (SVC_CALIB_LDC_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_TBL_GET,    SvcCalib_LdcCmdTblDataGet };

        SVC_CalibLdcCtrlFlag |= SVC_CALIB_LDC_FLG_CMD_INIT;
    }
}

static UINT32 SvcCalib_LdcCmdMemQuery(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    if (pParam1 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to query ldc memory - input ldc table number should not zero." PRN_CAL_LDC_NG
    } else if (pParam2 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to query ldc memory - output ldc shadow buffer size should not null!" PRN_CAL_LDC_NG
    } else if (pParam3 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to query ldc memory - output ldc working buffer size should not null!" PRN_CAL_LDC_NG
    } else if (pParam4 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to query ldc memory - ldc calculation buffer size should not null!" PRN_CAL_LDC_NG
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

        RetVal = SvcCalib_LdcMemQuery(NomOfTable, pShadowBufSize, pWorkingBufSize, pCalcMemSize);
        if (RetVal != SVC_OK) {
            PRN_CAL_LDC_LOG "Fail to query ldc memory - query memory fail!" PRN_CAL_LDC_NG
        }

        AmbaMisra_TouchUnused(pParam1);
        AmbaMisra_TouchUnused(pParam2);
        AmbaMisra_TouchUnused(pParam3);
        AmbaMisra_TouchUnused(pParam4);
        AmbaMisra_TouchUnused(pU32Val);
    }

    return RetVal;
}


static UINT32 SvcCalib_LdcCmdCreate(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SvcCalib_LdcCreate();
    if (RetVal != SVC_OK) {
        PRN_CAL_LDC_LOG "Fail to create ldc module fail!" PRN_CAL_LDC_NG
    } else {
        AmbaMisra_TouchUnused(pParam1);
        AmbaMisra_TouchUnused(pParam2);
        AmbaMisra_TouchUnused(pParam3);
        AmbaMisra_TouchUnused(pParam4);
    }

    return RetVal;
}


static UINT32 SvcCalib_LdcCmdDataInit(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    if (pParam1 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to init ldc - input calib object should not null!" PRN_CAL_LDC_NG
    } else {
        const SVC_CALIB_OBJ_s *pCalObj;

        AmbaMisra_TypeCast(&pCalObj, &pParam1);
        if (pCalObj == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_LDC_LOG "Fail to init ldc - invalid calib obj!" PRN_CAL_LDC_NG
        } else {
            RetVal = SvcCalib_LdcShadowInit(pCalObj);
            if (RetVal != SVC_OK) {
                PRN_CAL_LDC_LOG "Fail to init ldc - shadow initial fail!" PRN_CAL_LDC_NG
            } else {
                SVC_CALIB_CALC_CB_f CbFunc = NULL;
                AmbaMisra_TypeCast(&(CbFunc), &(pParam2));
                SVC_CalibLdcCtrl.CalcCbFunc = CbFunc;
            }
        }

        AmbaMisra_TouchUnused(pParam1);
        AmbaMisra_TouchUnused(pParam2);
        AmbaMisra_TouchUnused(pParam3);
        AmbaMisra_TouchUnused(pParam4);
    }

    return RetVal;
}

static UINT32 SvcCalib_LdcCmdTblCfg(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    if (pParam1 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to cfg ldc - config command should not null!" PRN_CAL_LDC_NG
    } else if ((SVC_CalibLdcCtrlFlag & SVC_CALIB_LDC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to cfg ldc - create ldc module first!" PRN_CAL_LDC_NG
    } else {
        UINT32 *pCmdID = NULL;

        AmbaMisra_TypeCast(&(pCmdID), &pParam1);

        if (pCmdID != NULL) {
            SVC_CalibLdcCtrl.CalUpdCtrl = *pCmdID;
            PRN_CAL_LDC_LOG "Configure ldc update ctrl %d"
                PRN_CAL_LDC_ARG_UINT32 SVC_CalibLdcCtrl.CalUpdCtrl PRN_CAL_LDC_ARG_POST
            PRN_CAL_LDC_API
        }

        AmbaMisra_TouchUnused(pCmdID);
    }

    AmbaMisra_TouchUnused(pParam1);
    AmbaMisra_TouchUnused(pParam2);
    AmbaMisra_TouchUnused(pParam3);
    AmbaMisra_TouchUnused(pParam4);

    return RetVal;
}

static UINT32 SvcCalib_LdcCmdTblUpdate(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    if (pParam1 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to update ldc table - input calib channel should not null!" PRN_CAL_LDC_NG
    } else if (pParam2 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to update ldc table - input IK mode cfg should not null!" PRN_CAL_LDC_NG
    } else {
        const SVC_CALIB_CHANNEL_s     *pChan;
        const AMBA_IK_MODE_CFG_s      *pImgMode;

        AmbaMisra_TypeCast(&(pChan), &pParam1);
        AmbaMisra_TypeCast(&(pImgMode), &pParam2);

        RetVal = SvcCalib_LdcTableUpdate(pChan, pImgMode);
    }

    AmbaMisra_TouchUnused(pParam1);
    AmbaMisra_TouchUnused(pParam2);
    AmbaMisra_TouchUnused(pParam3);
    AmbaMisra_TouchUnused(pParam4);

    return RetVal;
}


static UINT32 SvcCalib_LdcCmdTblDataSet(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;
    UINT32 *pU32Val = NULL;
    SVC_CALIB_TBL_INFO_s *pTblInfo;

    AmbaMisra_TypeCast(&(pU32Val), &(pParam1));
    AmbaMisra_TypeCast(&(pTblInfo), &(pParam2));

    if (pU32Val == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to set table data - input table id should not null!" PRN_CAL_LDC_NG
    } else {
        RetVal = SvcCalib_LdcTableDataSet(*pU32Val, pTblInfo);
    }

    AmbaMisra_TouchUnused(pParam1);
    AmbaMisra_TouchUnused(pParam2);
    AmbaMisra_TouchUnused(pParam3);
    AmbaMisra_TouchUnused(pParam4);
    AmbaMisra_TouchUnused(pU32Val);

    return RetVal;
}


static UINT32 SvcCalib_LdcCmdTblDataGet(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;
    UINT32 *pU32Val = NULL;
    SVC_CALIB_TBL_INFO_s *pTblInfo;

    AmbaMisra_TypeCast(&(pU32Val), &(pParam1));
    AmbaMisra_TypeCast(&(pTblInfo), &(pParam2));

    if (pU32Val == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to get table data - input table id should not null!" PRN_CAL_LDC_NG
    } else {
        RetVal = SvcCalib_LdcTableDataGet(*pU32Val, pTblInfo);
    }

    AmbaMisra_TouchUnused(pParam1);
    AmbaMisra_TouchUnused(pParam2);
    AmbaMisra_TouchUnused(pParam3);
    AmbaMisra_TouchUnused(pParam4);
    AmbaMisra_TouchUnused(pU32Val);

    return RetVal;
}

/**
 * calib ldc command entry
 *
 * @param [in] CmdID  the bpc command id
 * @param [in] Param1 no.01 parameter
 * @param [in] Param2 no.02 parameter
 * @param [in] Param3 no.03 parameter
 * @param [in] Param4 no.04 parameter
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcCalib_LdcCmdFunc(UINT32 CmdID, void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    SvcCalib_LdcCmdHdlrInit();

    if (CmdID >= SVC_CALIB_CMD_NUM) {
        RetVal = SVC_NG;
        PRN_CAL_LDC_LOG "Fail to handler cmd - invalid command id(%d)"
            PRN_CAL_LDC_ARG_UINT32 CmdID PRN_CAL_LDC_ARG_POST
        PRN_CAL_LDC_NG
    } else {
        const UINT32 HdlrCount = (UINT32)(sizeof(SVC_CalibLdcCmdHdlr)) / ((UINT32)sizeof(SVC_CalibLdcCmdHdlr[0]));
        UINT32 HdlrIdx;

        for (HdlrIdx = 0U; HdlrIdx < HdlrCount; HdlrIdx ++) {
            if ((SVC_CalibLdcCmdHdlr[HdlrIdx].Enable > 0U) &&
                (SVC_CalibLdcCmdHdlr[HdlrIdx].CmdID == CmdID) &&
                (SVC_CalibLdcCmdHdlr[HdlrIdx].pHandler != NULL)) {
                RetVal = (SVC_CalibLdcCmdHdlr[HdlrIdx].pHandler)(pParam1, pParam2, pParam3, pParam4);
            }
        }
    }

    return RetVal;
}

