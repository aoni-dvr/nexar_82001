/**
*  @file SvcCalibCa.c
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
*  @details C file for Calibration Chromatic Aberration Distortion
*
*/

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaMisraFix.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaVIN_Def.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaFS.h"

#include "AmbaCalib_ErrNo.h"
#include "AmbaCalib_CaIF.h"

#include "AmbaCT_TextHdlr.h"
#include "AmbaCT_CaTunerIF.h"

#include "SvcWrap.h"
#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcCalibMgr.h"

#define SVC_CALIB_CA_FLG_INIT           (0x1U)
#define SVC_CALIB_CA_FLG_SHADOW_INIT    (0x2U)
#define SVC_CALIB_CA_FLG_CALC_INIT      (0x4U)
#define SVC_CALIB_CA_FLG_CALC_VIN_SEN   (0x8U)
#define SVC_CALIB_CA_FLG_SHELL_INIT     (0x100U)
#define SVC_CALIB_CA_FLG_CMD_INIT       (0x200U)
/* Reserve 0x10000/0x20000/0x40000/0x80000 for SvcCalibMgr.h */

#define SVC_CALIB_CA_NAME               ("SvcCalib_Ca")
#define SVC_CALIB_CA_NAND_HEADER_SIZE   (0x200U)
#define SVC_CALIB_CA_VERSION            (0x20180401U)
#define SVC_CALIB_CA_IK_2_STAGE         (0x1U)
#define SVC_CALIB_CA_MAX_TBL_LEN        ( CA_MAX_GRID_SIZE )
#define SVC_CALIB_CA_REMAP_BUF_SIZE     ( 384U )

typedef struct {
    UINT32 Enable;
    UINT32 NumOfTable;
    UINT32 DebugMsgOn;
    UINT32 TableSelectBits;
    UINT32 Reserved[(SVC_CALIB_CA_NAND_HEADER_SIZE / 4U) - 4U];
} SVC_CALIB_CA_NAND_HEADER_s;

typedef struct {
    UINT32                        Version;
    UINT32                        HorGridNum;
    UINT32                        VerGridNum;
    UINT32                        TileWidthExp;
    UINT32                        TileHeightExp;
    SVC_CALIB_VIN_SENSOR_GEO_s    VinSensorGeo;
    UINT32                        TwoStageDone;
    UINT32                        VinSelectBits;
    UINT32                        SensorSelectBits;
} SVC_CALIB_CA_TABLE_HEADER_s;

typedef struct {
    SVC_CALIB_CA_TABLE_HEADER_s Header;
    AMBA_IK_GRID_POINT_s        CaRVector[SVC_CALIB_CA_MAX_TBL_LEN];
    AMBA_IK_GRID_POINT_s        CaBVector[SVC_CALIB_CA_MAX_TBL_LEN];
} SVC_CALIB_CA_NAND_TABLE_s;

typedef struct {
    UINT32                     VinSelectBits;
    UINT32                     SensorSelectBits;
    AMBA_IK_CAWARP_INFO_s      CaInfo;
    AMBA_IK_GRID_POINT_s       CaRVector[SVC_CALIB_CA_MAX_TBL_LEN];
    AMBA_IK_GRID_POINT_s       CaBVector[SVC_CALIB_CA_MAX_TBL_LEN];
} SVC_CALIB_CA_TABLE_s;

typedef struct {
    UINT32                      SaveBinOn;
    UINT32                      TableIdx;
} SVC_CALIB_CA_CALCULATE_CTRL_s;

#define SVC_CALIB_CA_MEM_CALC           (0x0U)
#define SVC_CALIB_CA_MEM_CALC_CTUNER    (0x1U)
#define SVC_CALIB_CA_MEM_CALC_REMAP     (0x2U)
#define SVC_CALIB_CA_MEM_CALC_REMAP_R   (0x3U)
#define SVC_CALIB_CA_MEM_CALC_REMAP_B   (0x4U)
#define SVC_CALIB_CA_MEM_NUM            (0x5U)
typedef struct {
    UINT32  Enable;
    UINT8  *pBuf;
    UINT32  BufSize;
} SVC_CALIB_CA_MEM_CTRL_s;

typedef struct {
    char                          Name[16];
    AMBA_KAL_MUTEX_t              Mutex;
    SVC_CALIB_CA_NAND_HEADER_s   *pNandHeader;
    SVC_CALIB_CA_NAND_TABLE_s    *pNandTable;
    UINT32                        NumOfCaTable;
    SVC_CALIB_CA_TABLE_s         *pCaTable;

    SVC_CALIB_CA_MEM_CTRL_s       MemCtrl[SVC_CALIB_CA_MEM_NUM];
    SVC_CALIB_CA_CALCULATE_CTRL_s CalcCaCtrl;
    SVC_CALIB_CALC_CB_f           CalcCbFunc;
    SVC_CALIB_CHANNEL_s           CalibChan;
    UINT32                        EnableDebugMsg;
} SVC_CALIB_CA_CTRL_s;

static UINT32                 SvcCalib_CaMutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut);
static void                   SvcCalib_CaMutexGive(AMBA_KAL_MUTEX_t *pMutex);
static void                   SvcCalib_CaErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode);

static UINT32                 SvcCalib_CaMemQuery(UINT32 MaxTableNum, UINT32 *pShadowSize, UINT32 *pWorkMemSize, UINT32 *pCalcMemSize);
static UINT32                 SvcCalib_CaMemLock(void);
static void                   SvcCalib_CaMemUnLock(void);
static UINT32                 SvcCalib_CaVinSensorIDCheck(UINT32 VinID, UINT32 SensorID);
static UINT32                 SvcCalib_CaCreate(void);
static UINT32                 SvcCalib_CaShadowInit(const SVC_CALIB_OBJ_s *pCalObj);
static UINT32                 SvcCalib_CaTableUpdate(const SVC_CALIB_CHANNEL_s *pCalibChan, const AMBA_IK_MODE_CFG_s *pImgMode);
static UINT32                 SvcCalib_CaTableCfg(UINT32 NandTableID, UINT32 TableID);
static UINT32                 SvcCalib_CaTableSearch(UINT32 VinID, UINT32 SensorID, UINT32 *pTblIdx);
static UINT32                 SvcCalib_CaNandTableSearch(UINT32 VinID, UINT32 SensorID, UINT32 *pTblIdx);
static SVC_CALIB_CA_TABLE_s * SvcCalib_CaTableGet(const SVC_CALIB_CHANNEL_s *pCalibChan);
static UINT32                 SvcCalib_CaTableGenTbl(const SVC_CALIB_CHANNEL_s *pCalibChan, const char *pScriptPath);
static UINT32                 SvcCalib_CaTableRemap(SVC_CALIB_CA_NAND_TABLE_s *pNandTbl, AMBA_CAL_GRID_POINT_s *pCaRedTbl, AMBA_CAL_GRID_POINT_s *pCaBlueTbl);
static UINT32                 SvcCalib_CaTableDataSet(UINT32 TblID, SVC_CALIB_TBL_INFO_s *pTblInfo);
static UINT32                 SvcCalib_CaTableDataGet(UINT32 TblID, SVC_CALIB_TBL_INFO_s *pTblInfo);

// Private function definition for calibration command
typedef UINT32 (*SVC_CALIB_CA_CMD_FUNC_f)(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
typedef struct {
     UINT32                  Enable;
     UINT32                  CmdID;
     SVC_CALIB_CA_CMD_FUNC_f pHandler;
} SVC_CALIB_CA_CMD_HDLR_s;

static void   SvcCalib_CaCmdHdlrInit(void);
static UINT32 SvcCalib_CaCmdMemQuery(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_CaCmdCreate(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_CaCmdDataInit(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_CaCmdTblUpdate(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_CaCmdTblDataSet(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_CaCmdTblDataGet(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
//UINT32 SvcCalib_CaCmdFunc(UINT32 CmdID, UINT32 Param1, UINT32 Param2, UINT32 Param3, UINT32 Param4);

// Private function definition for shell command.
typedef UINT32 (*SVC_CALIB_CA_SHELL_FUNC_f)(UINT32 ArgCount, char * const *pArgVector);
typedef void   (*SVC_CALIB_CA_SHELL_USAGE_f)(void);

typedef struct {
    UINT32                     Enable;
    char                       ShellCmdName[32];
    SVC_CALIB_CA_SHELL_FUNC_f  pFunc;
    SVC_CALIB_CA_SHELL_USAGE_f pUsageFunc;
} SVC_CALIB_CA_SHELL_FUNC_s;

static void   SvcCalib_CaShellStrToU32(const char *pString, UINT32 *pValue);
static UINT32 SvcCalib_CaShellCfgVinSen(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_CaShellCfgVinSenU(void);
static UINT32 SvcCalib_CaShellCfgTblID(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_CaShellCfgTblIDU(void);
static UINT32 SvcCalib_CaShellCfgDbgMsg(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_CaShellCfgDbgMsgU(void);
static UINT32 SvcCalib_CaShellGenTbl(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_CaShellGenTblU(void);
static UINT32 SvcCalib_CaShellEnable(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_CaShellEnableU(void);
static UINT32 SvcCalib_CaShellUpd(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_CaShellUpdU(void);
static UINT32 SvcCalib_CaShellSet(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_CaShellSetU(void);
static void   SvcCalib_CaShellEntryInit(void);
static void   SvcCalib_CaShellUsage(void);

#define SVC_CALIB_CA_SHELL_CMD_NUM  (7U)
static SVC_CALIB_CA_SHELL_FUNC_s SVC_CalibCaShellFunc[SVC_CALIB_CA_SHELL_CMD_NUM] GNU_SECTION_NOZEROINIT;
static SVC_CALIB_CA_CMD_HDLR_s SVC_CalibCaCmdHdlr[SVC_CALIB_CMD_NUM] GNU_SECTION_NOZEROINIT;
static SVC_CALIB_CA_CTRL_s SVC_CalibCaCtrl GNU_SECTION_NOZEROINIT;
static UINT32 SVC_CalibCaCtrlFlag = SVC_LOG_CAL_DEF_FLG;

#define SVC_LOG_CA " CA"
#define PRN_CAL_CA_LOG        { SVC_WRAP_PRINT_s CalibCaPrint; AmbaSvcWrap_MisraMemset(&(CalibCaPrint), 0, sizeof(CalibCaPrint)); CalibCaPrint.Argc --; CalibCaPrint.pStrFmt =
#define PRN_CAL_CA_ARG_UINT32 ; CalibCaPrint.Argc ++; CalibCaPrint.Argv[CalibCaPrint.Argc].Uint64   = (UINT64)((
#define PRN_CAL_CA_ARG_CSTR   ; CalibCaPrint.Argc ++; CalibCaPrint.Argv[CalibCaPrint.Argc].pCStr    = ((
#define PRN_CAL_CA_ARG_CPOINT ; CalibCaPrint.Argc ++; CalibCaPrint.Argv[CalibCaPrint.Argc].pPointer = ((
#define PRN_CAL_CA_ARG_POST   ))
#define PRN_CAL_CA_OK         ; CalibCaPrint.Argc ++; SvcCalib_CaPrintLog(SVC_LOG_CAL_OK , &(CalibCaPrint)); }
#define PRN_CAL_CA_NG         ; CalibCaPrint.Argc ++; SvcCalib_CaPrintLog(SVC_LOG_CAL_NG , &(CalibCaPrint)); }
#define PRN_CAL_CA_API        ; CalibCaPrint.Argc ++; SvcCalib_CaPrintLog(SVC_LOG_CAL_API, &(CalibCaPrint)); }
#define PRN_CAL_CA_DBG        ; CalibCaPrint.Argc ++; SvcCalib_CaPrintLog(SVC_LOG_CAL_DBG, &(CalibCaPrint)); }
#define PRN_CAL_CA_ERR_HDLR   SvcCalib_CaErrHdlr(__func__, __LINE__, PRetVal);

static void SvcCalib_CaPrintLog(UINT32 LogLevel, SVC_WRAP_PRINT_s *pPrint)
{
    if (pPrint != NULL) {
        if ((SVC_CalibCaCtrlFlag & LogLevel) > 0U) {

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

            SvcWrap_Print(SVC_LOG_CA, pPrint);
        }
    }
}

static UINT32 SvcCalib_CaMutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut)
{
    return AmbaKAL_MutexTake(pMutex, TimeOut);
}

static void SvcCalib_CaMutexGive(AMBA_KAL_MUTEX_t *pMutex)
{
    UINT32 ErrCode = AmbaKAL_MutexGive(pMutex);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }
}

static void SvcCalib_CaErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode)
{
    if (pCaller != NULL) {
        if (ErrCode != 0U) {
            PRN_CAL_CA_LOG "Catch ErrCode(0x%08x) @ %s, %d"
                PRN_CAL_CA_ARG_UINT32 ErrCode  PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_ARG_CSTR   pCaller  PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_ARG_UINT32 CodeLine PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_NG
        }
    }
}

static UINT32 SvcCalib_CaMemQuery(UINT32 MaxTableNum, UINT32 *pShadowSize, UINT32 *pWorkMemSize, UINT32 *pCalcMemSize)
{
    UINT32 RetVal = SVC_OK;

    UINT32 DramShadowSize = 0, WorkingMemSize = 0, CalcMemSize = 0U;
    UINT32 DramShadowHeaderSize = (UINT32)(sizeof(SVC_CALIB_CA_NAND_HEADER_s));
    UINT32 DramShadowTableSize  = (UINT32)(sizeof(SVC_CALIB_CA_NAND_TABLE_s));
    UINT32 DramShadowDataSize   = DramShadowTableSize * MaxTableNum;
    UINT32 CaTableSize          = (UINT32)(sizeof(SVC_CALIB_CA_TABLE_s));
    UINT32 TotalCaTableSize     = CaTableSize * MaxTableNum;
    UINT32 CalcMemAlgoSize;
    SIZE_t CalcMemAlgoOriSize   = 0;
    SIZE_t CalcMemAlgoSepSize   = 0;
    SIZE_t CalcMemCtnSize       = 0;
    UINT32 CalcMemRemapVector   = (UINT32)(sizeof(AMBA_IK_GRID_POINT_s)) * SVC_CALIB_CA_MAX_TBL_LEN;

    if (CAL_OK != AmbaCal_CaGetBufSize(&CalcMemAlgoOriSize)) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to query ca module memory - get ca algo working memory fail!" PRN_CAL_CA_NG
    } else if (0U != AmbaCal_CaSeparatedRBGetBufSize(&CalcMemAlgoSepSize)) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to query ca module memory - query ca algo separate working memory size fail!" PRN_CAL_CA_NG
    } else if (0U != AmbaCT_GetBufSize(AMBA_CT_TYPE_CA, &CalcMemCtnSize)) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to query ca module memory - query ca ctuner working memory size fail!" PRN_CAL_CA_NG
    } else {
        DramShadowSize  = DramShadowHeaderSize;
        DramShadowSize += DramShadowDataSize;

        WorkingMemSize  = TotalCaTableSize;

        CalcMemAlgoSize = GetMaxValU32((UINT32)CalcMemAlgoOriSize, (UINT32)CalcMemAlgoSepSize);
        CalcMemSize    += CalcMemAlgoSize;
        CalcMemSize    += (UINT32)CalcMemCtnSize;
        CalcMemSize    += SVC_CALIB_CA_REMAP_BUF_SIZE;
        CalcMemSize    += (CalcMemRemapVector << 1U);

        PRN_CAL_CA_LOG " " PRN_CAL_CA_DBG
        PRN_CAL_CA_LOG "====== Calib CA Memory Query ======" PRN_CAL_CA_DBG
        PRN_CAL_CA_LOG "  MaxTableNum    : %d"
            PRN_CAL_CA_ARG_UINT32   MaxTableNum PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_DBG
        PRN_CAL_CA_LOG "  DramShadowSize : 0x%X"
            PRN_CAL_CA_ARG_UINT32 DramShadowSize PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_DBG
        PRN_CAL_CA_LOG "    DramShadowHeaderSize : 0x%X"
            PRN_CAL_CA_ARG_UINT32 DramShadowHeaderSize PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_DBG
        PRN_CAL_CA_LOG "    DramShadowDataSize   : 0x%X ( 0x%X x %d )"
            PRN_CAL_CA_ARG_UINT32 DramShadowDataSize  PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_ARG_UINT32 DramShadowTableSize PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_ARG_UINT32 MaxTableNum         PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_DBG
        PRN_CAL_CA_LOG "  WorkingMemSize : 0x%X"
            PRN_CAL_CA_ARG_UINT32 WorkingMemSize PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_DBG
        PRN_CAL_CA_LOG "    TotalCaTableSize : 0x%X ( 0x%X x %d )"
            PRN_CAL_CA_ARG_UINT32 TotalCaTableSize PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_ARG_UINT32 CaTableSize      PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_ARG_UINT32 MaxTableNum      PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_DBG
        PRN_CAL_CA_LOG "  CalcMemSize : 0x%X"
            PRN_CAL_CA_ARG_UINT32 CalcMemSize PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_DBG
        PRN_CAL_CA_LOG "    CalcMemAlgoSize    : 0x%X = max(0x%X, 0x%X)"
            PRN_CAL_CA_ARG_UINT32 CalcMemAlgoSize    PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_ARG_UINT32 CalcMemAlgoOriSize PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_ARG_UINT32 CalcMemAlgoSepSize PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_DBG
        PRN_CAL_CA_LOG "    CalcMemCtnSize     : 0x%X"
            PRN_CAL_CA_ARG_UINT32 CalcMemCtnSize PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_DBG
        PRN_CAL_CA_LOG "    CalcMemRemapSize   : 0x%X"
            PRN_CAL_CA_ARG_UINT32 SVC_CALIB_CA_REMAP_BUF_SIZE PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_DBG
        PRN_CAL_CA_LOG "    CalcMemRemapVectorR: 0x%X"
            PRN_CAL_CA_ARG_UINT32 CalcMemRemapVector PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_DBG
        PRN_CAL_CA_LOG "    CalcMemRemapVectorB: 0x%X"
            PRN_CAL_CA_ARG_UINT32 CalcMemRemapVector PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_DBG

        if (pShadowSize != NULL) {
            *pShadowSize  = DramShadowSize;
        }
        PRN_CAL_CA_LOG "Query the calib ca dram shadow size 0x%X ( table num %d )"
            PRN_CAL_CA_ARG_UINT32 DramShadowSize PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_ARG_UINT32 MaxTableNum    PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_DBG

        if (pWorkMemSize != NULL) {
            *pWorkMemSize = WorkingMemSize;
        }
        PRN_CAL_CA_LOG "Query the calib ca working memory size 0x%X ( table num %d )"
            PRN_CAL_CA_ARG_UINT32 WorkingMemSize PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_ARG_UINT32 MaxTableNum    PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_DBG

        if (pCalcMemSize != NULL) {
            *pCalcMemSize = CalcMemSize;
        }
        PRN_CAL_CA_LOG "Query the calib ca algo. memory size 0x%X ( table num %d )"
            PRN_CAL_CA_ARG_UINT32 CalcMemSize PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_ARG_UINT32 MaxTableNum PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_DBG
    }

    return RetVal;
}


static UINT32 SvcCalib_CaMemLock(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    UINT32 CalcMemAlgoSize;
    SIZE_t CalcMemAlgoOriSize   = 0;
    SIZE_t CalcMemAlgoSepSize   = 0;
    SIZE_t CalcMemCtnSize = 0U;
    UINT32 CalcMemRemapSize = SVC_CALIB_CA_REMAP_BUF_SIZE;
    UINT32 CalcMemRemapVectorSize = (UINT32)(sizeof(AMBA_IK_GRID_POINT_s)) * SVC_CALIB_CA_MAX_TBL_LEN;
    UINT8 *pCalcMem = NULL;
    UINT32 CalcMemSize = 0U;

    /* Configure the ca algo memory */
    if (CAL_OK != AmbaCal_CaGetBufSize(&CalcMemAlgoOriSize)) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to cfg ca module memory - get ca algo working memory fail!" PRN_CAL_CA_NG
    } else if (0U != AmbaCal_CaSeparatedRBGetBufSize(&CalcMemAlgoSepSize)) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to cfg ca module memory - query ca algo separate working memory size fail!" PRN_CAL_CA_NG
    } else if (0U != AmbaCT_GetBufSize(AMBA_CT_TYPE_CA, &CalcMemCtnSize)) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to cfg ca table memory - query ca ctuner working memory size fail!" PRN_CAL_CA_NG
    } else {

        CalcMemAlgoSize = GetMaxValU32((UINT32)CalcMemAlgoOriSize, (UINT32)CalcMemAlgoSepSize);

        CalcMemSize = CalcMemAlgoSize + (UINT32)CalcMemCtnSize + CalcMemRemapSize + (CalcMemRemapVectorSize * 2U);

        if (0U != SvcCalib_ItemCalcMemGet(SVC_CALIB_CA_ID, 1U, &pCalcMem, &CalcMemSize)) {
            RetVal = SVC_NG;
            PRN_CAL_CA_LOG "Fail to cfg ca table memory - query ca ctuner working memory size fail!" PRN_CAL_CA_NG
        } else {
            if (CalcMemSize < (CalcMemAlgoSize
                              + (UINT32)CalcMemCtnSize
                              + CalcMemRemapSize
                              + (CalcMemRemapVectorSize * 2U))) {
                RetVal = SVC_NG;
                PRN_CAL_CA_LOG "Fail to cfg ca table memory - calib calculation memory is not enough to service it!" PRN_CAL_CA_NG
            } else if (pCalcMem == NULL) {
                RetVal = SVC_NG;
                PRN_CAL_CA_LOG "Fail to cfg ca table memory - invalid calib calculation memory!" PRN_CAL_CA_NG
            } else {
                AmbaSvcWrap_MisraMemset(&(SVC_CalibCaCtrl.MemCtrl[SVC_CALIB_CA_MEM_CALC]), 0, sizeof(SVC_CALIB_CA_MEM_CTRL_s));
                SVC_CalibCaCtrl.MemCtrl[SVC_CALIB_CA_MEM_CALC].BufSize = CalcMemAlgoSize;
                SVC_CalibCaCtrl.MemCtrl[SVC_CALIB_CA_MEM_CALC].pBuf    = pCalcMem;
                SVC_CalibCaCtrl.MemCtrl[SVC_CALIB_CA_MEM_CALC].Enable  = 1U;
                pCalcMem = &(pCalcMem[CalcMemAlgoSize]);

                AmbaSvcWrap_MisraMemset(&(SVC_CalibCaCtrl.MemCtrl[SVC_CALIB_CA_MEM_CALC_CTUNER]), 0, sizeof(SVC_CALIB_CA_MEM_CTRL_s));
                SVC_CalibCaCtrl.MemCtrl[SVC_CALIB_CA_MEM_CALC_CTUNER].BufSize = (UINT32)CalcMemCtnSize;
                SVC_CalibCaCtrl.MemCtrl[SVC_CALIB_CA_MEM_CALC_CTUNER].pBuf    = pCalcMem;
                SVC_CalibCaCtrl.MemCtrl[SVC_CALIB_CA_MEM_CALC_CTUNER].Enable  = 1U;
                pCalcMem = &(pCalcMem[(UINT32)CalcMemCtnSize]);

                AmbaSvcWrap_MisraMemset(&(SVC_CalibCaCtrl.MemCtrl[SVC_CALIB_CA_MEM_CALC_REMAP]), 0, sizeof(SVC_CALIB_CA_MEM_CTRL_s));
                SVC_CalibCaCtrl.MemCtrl[SVC_CALIB_CA_MEM_CALC_REMAP].BufSize = CalcMemRemapSize;
                SVC_CalibCaCtrl.MemCtrl[SVC_CALIB_CA_MEM_CALC_REMAP].pBuf    = pCalcMem;
                SVC_CalibCaCtrl.MemCtrl[SVC_CALIB_CA_MEM_CALC_REMAP].Enable  = 1U;
                pCalcMem = &(pCalcMem[CalcMemRemapSize]);

                AmbaSvcWrap_MisraMemset(&(SVC_CalibCaCtrl.MemCtrl[SVC_CALIB_CA_MEM_CALC_REMAP_R]), 0, sizeof(SVC_CALIB_CA_MEM_CTRL_s));
                SVC_CalibCaCtrl.MemCtrl[SVC_CALIB_CA_MEM_CALC_REMAP_R].BufSize = CalcMemRemapVectorSize;
                SVC_CalibCaCtrl.MemCtrl[SVC_CALIB_CA_MEM_CALC_REMAP_R].pBuf    = pCalcMem;
                SVC_CalibCaCtrl.MemCtrl[SVC_CALIB_CA_MEM_CALC_REMAP_R].Enable  = 1U;
                pCalcMem = &(pCalcMem[CalcMemRemapVectorSize]);

                AmbaSvcWrap_MisraMemset(&(SVC_CalibCaCtrl.MemCtrl[SVC_CALIB_CA_MEM_CALC_REMAP_B]), 0, sizeof(SVC_CALIB_CA_MEM_CTRL_s));
                SVC_CalibCaCtrl.MemCtrl[SVC_CALIB_CA_MEM_CALC_REMAP_B].BufSize = CalcMemRemapVectorSize;
                SVC_CalibCaCtrl.MemCtrl[SVC_CALIB_CA_MEM_CALC_REMAP_B].pBuf    = pCalcMem;
                SVC_CalibCaCtrl.MemCtrl[SVC_CALIB_CA_MEM_CALC_REMAP_B].Enable  = 1U;
            }

            if (RetVal != SVC_OK) {
                // Free calib calculation memory
                PRetVal = SvcCalib_ItemCalcMemFree(SVC_CALIB_CA_ID); PRN_CAL_CA_ERR_HDLR
            }
        }
    }

    return RetVal;
}


static void SvcCalib_CaMemUnLock(void)
{
    // Reset the memory control setting
    AmbaSvcWrap_MisraMemset(SVC_CalibCaCtrl.MemCtrl, 0, sizeof(SVC_CalibCaCtrl.MemCtrl));

    // Free the calculation memory
    if (SVC_OK != SvcCalib_ItemCalcMemFree(SVC_CALIB_CA_ID)){
        PRN_CAL_CA_LOG "Fail to un-lock ca calc memory - init calculation memory control fail!" PRN_CAL_CA_NG
    } else {
        PRN_CAL_CA_LOG "Successful to un-lock ca calc memory!" PRN_CAL_CA_OK
    }
}


static UINT32 SvcCalib_CaVinSensorIDCheck(UINT32 VinID, UINT32 SensorID)
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


static UINT32 SvcCalib_CaCreate(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibCaCtrlFlag & SVC_CALIB_CA_FLG_INIT) > 0U) {
        RetVal = SVC_OK;
        PRN_CAL_CA_LOG "Calibration ca module has been created!" PRN_CAL_CA_DBG
    } else {
        // Reset the ca control module
        AmbaSvcWrap_MisraMemset(&SVC_CalibCaCtrl, 0, sizeof(SVC_CalibCaCtrl));

        // Configure the name
        SvcWrap_strcpy(SVC_CalibCaCtrl.Name, sizeof(SVC_CalibCaCtrl.Name), SVC_CALIB_CA_NAME);

        // Create the mutex
        RetVal = AmbaKAL_MutexCreate(&(SVC_CalibCaCtrl.Mutex), SVC_CalibCaCtrl.Name);
        if (RetVal != SVC_OK) {
            PRN_CAL_CA_LOG "Fail to create ca - create mutex fail!" PRN_CAL_CA_NG
        } else {
            PRetVal = (SVC_CalibCaCtrlFlag & (SVC_CALIB_CA_FLG_SHELL_INIT |
                                              SVC_CALIB_CA_FLG_CMD_INIT   |
                                              SVC_LOG_CAL_DEF_FLG         |
                                              SVC_LOG_CAL_DBG));
            SVC_CalibCaCtrlFlag = PRetVal;
            SVC_CalibCaCtrlFlag |= SVC_CALIB_CA_FLG_INIT;
            PRN_CAL_CA_LOG "Successful to create ca!" PRN_CAL_CA_OK
        }
    }

    return RetVal;
}


static UINT32 SvcCalib_CaShadowInit(const SVC_CALIB_OBJ_s *pCalObj)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibCaCtrlFlag & SVC_CALIB_CA_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to initial ca dram shodow - create ca first!" PRN_CAL_CA_NG
    } else if (pCalObj == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to initial ca dram shodow - invalid calib object!" PRN_CAL_CA_NG
    } else if (pCalObj->Enable == 0U) {
        RetVal = 99;
        PRN_CAL_CA_LOG "Disable ca dram shadow!!" PRN_CAL_CA_API
    } else {
        UINT32 DramShadowSize = 0, WorkingMemSize = 0;
        const SVC_CALIB_CA_NAND_HEADER_s *pHeader;

        AmbaMisra_TypeCast(&(pHeader), &(pCalObj->pShadowBuf));

        RetVal = SvcCalib_CaMemQuery(pCalObj->NumOfTable, &DramShadowSize, &WorkingMemSize, NULL);
        if (RetVal != SVC_OK) {
            PRN_CAL_CA_LOG "Fail to initial ca dram shadow - query memory fail!" PRN_CAL_CA_NG
        } else if ((pCalObj->pShadowBuf == NULL) || (pHeader == NULL)) {
            RetVal = SVC_NG;
            PRN_CAL_CA_LOG "Fail to initial ca dram shadow - dram shadow buffer should not null!" PRN_CAL_CA_NG
        } else if (pCalObj->ShadowBufSize < DramShadowSize) {
            RetVal = SVC_NG;
            PRN_CAL_CA_LOG "Fail to initial ca dram shadow - prepare dram shadow size is too small!" PRN_CAL_CA_NG
        } else if (pCalObj->pWorkingBuf == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_CA_LOG "Fail to initial ca dram shadow - working buffer should not null!" PRN_CAL_CA_NG
        } else if (pCalObj->WorkingBufSize < WorkingMemSize) {
            RetVal = SVC_NG;
            PRN_CAL_CA_LOG "Fail to initial ca dram shadow - working buffer size is too small!" PRN_CAL_CA_NG
        } else if (pHeader->NumOfTable > pCalObj->NumOfTable) {
            RetVal = SVC_NG;
            PRN_CAL_CA_LOG "Fail to initial ca dram shadow - calib nand table count(%d) > max ca dram shadow table count(%d)"
                PRN_CAL_CA_ARG_UINT32 pHeader->NumOfTable PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_ARG_UINT32 pCalObj->NumOfTable PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_NG
        } else if (KAL_ERR_NONE != SvcCalib_CaMutexTake(&(SVC_CalibCaCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
            RetVal = SVC_NG;
            PRN_CAL_CA_LOG "Fail to initial ca dram shadow - take mutex fail!" PRN_CAL_CA_NG
        } else {
            UINT8 *pNandHeaderBuf    = pCalObj->pShadowBuf;
            UINT32 NandHeaderSize    = (UINT32)(sizeof(SVC_CALIB_CA_NAND_HEADER_s));
            UINT8 *pNandTableBuf     = &(pNandHeaderBuf[NandHeaderSize]);
            UINT32 TotalCaTableSize  = (UINT32)(sizeof(SVC_CALIB_CA_TABLE_s)) * pCalObj->NumOfTable;

            PRN_CAL_CA_LOG "====== Shadow initialized Start ======" PRN_CAL_CA_DBG
            PRN_CAL_CA_LOG "  DramShadow Addr  : %p 0x%X"
                PRN_CAL_CA_ARG_CPOINT    pCalObj->pShadowBuf PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_ARG_UINT32 pCalObj->ShadowBufSize PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_DBG
            PRN_CAL_CA_LOG "    Nand Header Addr : %p 0x%X"
                PRN_CAL_CA_ARG_CPOINT  pNandHeaderBuf PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_ARG_UINT32 NandHeaderSize  PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_DBG
            PRN_CAL_CA_LOG "    Nand Table Addr  : %p"
                PRN_CAL_CA_ARG_CPOINT       pNandTableBuf PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_DBG
            PRN_CAL_CA_LOG "  Working Mem Addr : %p 0x%X"
                PRN_CAL_CA_ARG_CPOINT    pCalObj->pWorkingBuf PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_ARG_UINT32 pCalObj->WorkingBufSize PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_DBG
            PRN_CAL_CA_LOG "    Ca Table Addr   : %p 0x%08X"
                PRN_CAL_CA_ARG_CPOINT pCalObj->pWorkingBuf PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_ARG_UINT32 TotalCaTableSize     PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_DBG

            // Configure nand header
            AmbaMisra_TypeCast(&(SVC_CalibCaCtrl.pNandHeader), &(pNandHeaderBuf));
            // Configure nand table
            AmbaMisra_TypeCast(&(SVC_CalibCaCtrl.pNandTable),  &(pNandTableBuf)); AmbaMisra_TouchUnused(pNandTableBuf);
            // Configure working ca table
            AmbaMisra_TypeCast(&(SVC_CalibCaCtrl.pCaTable),  &(pCalObj->pWorkingBuf));
            if (SVC_CalibCaCtrl.pNandHeader == NULL) {
                RetVal = SVC_NG;
                PRN_CAL_CA_LOG "Fail to initial ca dram shadow - nand header should not null!" PRN_CAL_CA_NG
            } else if (SVC_CalibCaCtrl.pNandTable == NULL) {
                RetVal = SVC_NG;
                PRN_CAL_CA_LOG "Fail to initial ca dram shadow - nand table should not null!" PRN_CAL_CA_NG
            } else if (SVC_CalibCaCtrl.pCaTable == NULL) {
                RetVal = SVC_NG;
                PRN_CAL_CA_LOG "Fail to initial ca dram shadow - ca table should not null!" PRN_CAL_CA_NG
            } else {
                SVC_CalibCaCtrl.NumOfCaTable = pCalObj->NumOfTable;
                AmbaSvcWrap_MisraMemset(SVC_CalibCaCtrl.pCaTable, 0, TotalCaTableSize);

                if (SVC_CalibCaCtrl.pNandHeader->DebugMsgOn > 0U) {
                    SVC_CalibCaCtrl.EnableDebugMsg |= SVC_CalibCaCtrl.pNandHeader->DebugMsgOn;
                }

                if (SVC_CalibCaCtrl.EnableDebugMsg > 0U) {
                    UINT32 Idx;

                    PRN_CAL_CA_LOG " " PRN_CAL_CA_API
                    PRN_CAL_CA_LOG "---- Ca Nand Info %d ----"
                        PRN_CAL_CA_ARG_UINT32 SVC_CalibCaCtrl.pNandHeader->Reserved[0] PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_API
                    PRN_CAL_CA_LOG "  Enable         : %d"
                        PRN_CAL_CA_ARG_UINT32     SVC_CalibCaCtrl.pNandHeader->Enable PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_API
                    PRN_CAL_CA_LOG "  NumOfTable     : %d"
                        PRN_CAL_CA_ARG_UINT32     SVC_CalibCaCtrl.pNandHeader->NumOfTable PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_API
                    PRN_CAL_CA_LOG "  DebugMsgOn     : %d"
                        PRN_CAL_CA_ARG_UINT32     SVC_CalibCaCtrl.pNandHeader->DebugMsgOn PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_API
                    PRN_CAL_CA_LOG "  TblSelectButs  : 0x%X"
                        PRN_CAL_CA_ARG_UINT32   SVC_CalibCaCtrl.pNandHeader->TableSelectBits PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_API

                    {
                        const SVC_CALIB_CA_NAND_TABLE_s *pNandTable;

                        for (Idx = 0U; Idx < pCalObj->NumOfTable; Idx ++) {
                            pNandTable = &(SVC_CalibCaCtrl.pNandTable[Idx]);

                            PRN_CAL_CA_LOG "  ---- Nand Table[%d] %p -----"
                                PRN_CAL_CA_ARG_UINT32 Idx        PRN_CAL_CA_ARG_POST
                                PRN_CAL_CA_ARG_CPOINT pNandTable PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "    VinSelectBits       : 0x%X"
                                PRN_CAL_CA_ARG_UINT32   pNandTable->Header.VinSelectBits PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "    SensorSelectBits    : 0x%X"
                                PRN_CAL_CA_ARG_UINT32   pNandTable->Header.SensorSelectBits PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "    Version             : 0x%X"
                                PRN_CAL_CA_ARG_UINT32   pNandTable->Header.Version PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "    HorGridNum          : %d"
                                PRN_CAL_CA_ARG_UINT32     pNandTable->Header.HorGridNum PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "    VerGridNum          : %d"
                                PRN_CAL_CA_ARG_UINT32     pNandTable->Header.VerGridNum PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "    TileWidthExp        : %d"
                                PRN_CAL_CA_ARG_UINT32     pNandTable->Header.TileWidthExp PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "    TileHeightExp       : %d"
                                PRN_CAL_CA_ARG_UINT32     pNandTable->Header.TileHeightExp PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "    TwoStageDone        : %d"
                                PRN_CAL_CA_ARG_UINT32     pNandTable->Header.TwoStageDone PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "    VinSensorGeo.StartX : %d"
                                PRN_CAL_CA_ARG_UINT32     pNandTable->Header.VinSensorGeo.StartX PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "    VinSensorGeo.StartY : %d"
                                PRN_CAL_CA_ARG_UINT32     pNandTable->Header.VinSensorGeo.StartY PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "    VinSensorGeo.Width  : %d"
                                PRN_CAL_CA_ARG_UINT32     pNandTable->Header.VinSensorGeo.Width PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "    VinSensorGeo.Height : %d"
                                PRN_CAL_CA_ARG_UINT32     pNandTable->Header.VinSensorGeo.Height PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "    CaRVector           : %p"
                                PRN_CAL_CA_ARG_CPOINT pNandTable->CaRVector PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "    CaBVector           : %p"
                                PRN_CAL_CA_ARG_CPOINT pNandTable->CaBVector PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                        }
                    }

                    PRN_CAL_CA_LOG " " PRN_CAL_CA_API
                    PRN_CAL_CA_LOG "---- Ca Table Info ----" PRN_CAL_CA_API
                    PRN_CAL_CA_LOG "  NumOfTable     : %d"
                        PRN_CAL_CA_ARG_UINT32 SVC_CalibCaCtrl.NumOfCaTable PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_API

                    {
                        const SVC_CALIB_CA_TABLE_s *pCaTable;

                        for (Idx = 0U; Idx < SVC_CalibCaCtrl.NumOfCaTable; Idx ++) {
                            pCaTable = &(SVC_CalibCaCtrl.pCaTable[Idx]);

                            PRN_CAL_CA_LOG "  ---- Ca Table[%d] %p -----"
                                PRN_CAL_CA_ARG_UINT32 Idx      PRN_CAL_CA_ARG_POST
                                PRN_CAL_CA_ARG_CPOINT pCaTable PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "    VinSelectBits       : 0x%X"
                                PRN_CAL_CA_ARG_UINT32   SVC_CalibCaCtrl.pCaTable[Idx].VinSelectBits PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "    SensorSelectBits    : 0x%X"
                                PRN_CAL_CA_ARG_UINT32   SVC_CalibCaCtrl.pCaTable[Idx].SensorSelectBits PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "    Red Table Addr      : %p"
                                PRN_CAL_CA_ARG_CPOINT SVC_CalibCaCtrl.pCaTable[Idx].CaRVector PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "    Blue Table Addr     : %p"
                                PRN_CAL_CA_ARG_CPOINT SVC_CalibCaCtrl.pCaTable[Idx].CaBVector PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                        }
                    }
                }

                SVC_CalibCaCtrlFlag |= SVC_CALIB_CA_FLG_SHADOW_INIT;
                PRN_CAL_CA_LOG "Successful to initial ca dram shadow!" PRN_CAL_CA_OK
            }

            SvcCalib_CaMutexGive(&(SVC_CalibCaCtrl.Mutex));
        }
    }

    return RetVal;
}


static UINT32 SvcCalib_CaTableUpdate(const SVC_CALIB_CHANNEL_s *pCalibChan, const AMBA_IK_MODE_CFG_s *pImgMode)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibCaCtrlFlag & SVC_CALIB_CA_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to update ca table - initial ca module first!" PRN_CAL_CA_NG
    } else if ((SVC_CalibCaCtrlFlag & SVC_CALIB_CA_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to update ca table - initial ca dram shadow first!" PRN_CAL_CA_NG
    } else if (pCalibChan == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to update ca table - invalid calib channel!" PRN_CAL_CA_NG
    } else if (pImgMode == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to update ca table - invalid image mode!" PRN_CAL_CA_NG
    } else if (SVC_CalibCaCtrl.pNandHeader == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to update ca table - invalid nand table header!" PRN_CAL_CA_NG
    } else if ((SVC_CalibCaCtrl.pNandHeader->Enable == 0U) ||
               (SVC_CalibCaCtrl.pNandHeader->NumOfTable == 0U)) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "There is not ca table in shadow buffer!" PRN_CAL_CA_DBG
    } else if (KAL_ERR_NONE != SvcCalib_CaMutexTake(&(SVC_CalibCaCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to update ca table - take mutex fail!" PRN_CAL_CA_NG
    } else {
        const SVC_CALIB_CA_TABLE_s  *pCaTable = SvcCalib_CaTableGet(pCalibChan);

        if (pCaTable == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_CA_LOG "Fail to update ca table - get ca table fail" PRN_CAL_CA_NG
        } else {
            if (SVC_CalibCaCtrl.EnableDebugMsg > 0U) {

                PRN_CAL_CA_LOG " " PRN_CAL_CA_API
                PRN_CAL_CA_LOG "====== Calibration ca info ======" PRN_CAL_CA_API
                PRN_CAL_CA_LOG "  VinSelectBits                     : 0x%X"
                    PRN_CAL_CA_ARG_UINT32 pCaTable->VinSelectBits PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_API
                PRN_CAL_CA_LOG "  SensorSelectBits                  : 0x%X"
                    PRN_CAL_CA_ARG_UINT32 pCaTable->SensorSelectBits PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_API
                PRN_CAL_CA_LOG "  Version                           : 0x%X"
                    PRN_CAL_CA_ARG_UINT32 pCaTable->CaInfo.Version PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_API
                PRN_CAL_CA_LOG "  HorGridNum                        : %d"
                    PRN_CAL_CA_ARG_UINT32   pCaTable->CaInfo.HorGridNum PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_API
                PRN_CAL_CA_LOG "  VerGridNum                        : %d"
                    PRN_CAL_CA_ARG_UINT32   pCaTable->CaInfo.VerGridNum PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_API
                PRN_CAL_CA_LOG "  TileWidthExp                      : %d"
                    PRN_CAL_CA_ARG_UINT32   pCaTable->CaInfo.TileWidthExp PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_API
                PRN_CAL_CA_LOG "  TileHeightExp                     : %d"
                    PRN_CAL_CA_ARG_UINT32   pCaTable->CaInfo.TileHeightExp PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_API
                PRN_CAL_CA_LOG "  VinSensorGeo.StartX               : %d"
                    PRN_CAL_CA_ARG_UINT32   pCaTable->CaInfo.VinSensorGeo.StartX PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_API
                PRN_CAL_CA_LOG "  VinSensorGeo.StartY               : %d"
                    PRN_CAL_CA_ARG_UINT32   pCaTable->CaInfo.VinSensorGeo.StartY PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_API
                PRN_CAL_CA_LOG "  VinSensorGeo.Width                : %d"
                    PRN_CAL_CA_ARG_UINT32   pCaTable->CaInfo.VinSensorGeo.Width PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_API
                PRN_CAL_CA_LOG "  VinSensorGeo.Height               : %d"
                    PRN_CAL_CA_ARG_UINT32   pCaTable->CaInfo.VinSensorGeo.Height PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_API
                PRN_CAL_CA_LOG "  VinSensorGeo.HSubSample.FactorDen : %d"
                    PRN_CAL_CA_ARG_UINT32   pCaTable->CaInfo.VinSensorGeo.HSubSample.FactorDen PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_API
                PRN_CAL_CA_LOG "  VinSensorGeo.HSubSample.FactorNum : %d"
                    PRN_CAL_CA_ARG_UINT32   pCaTable->CaInfo.VinSensorGeo.HSubSample.FactorNum PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_API
                PRN_CAL_CA_LOG "  VinSensorGeo.VSubSample.FactorDen : %d"
                    PRN_CAL_CA_ARG_UINT32   pCaTable->CaInfo.VinSensorGeo.VSubSample.FactorDen PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_API
                PRN_CAL_CA_LOG "  VinSensorGeo.VSubSample.FactorNum : %d"
                    PRN_CAL_CA_ARG_UINT32   pCaTable->CaInfo.VinSensorGeo.VSubSample.FactorNum PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_API
                PRN_CAL_CA_LOG "  Enb2StageCompensation             : %d"
                    PRN_CAL_CA_ARG_UINT32   pCaTable->CaInfo.Enb2StageCompensation PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_API
                PRN_CAL_CA_LOG "  pCawarpRed                        : %p"
                    PRN_CAL_CA_ARG_CPOINT pCaTable->CaInfo.pCawarpRed PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_API
                PRN_CAL_CA_LOG "  pCawarpBlue                       : %p"
                    PRN_CAL_CA_ARG_CPOINT pCaTable->CaInfo.pCawarpBlue PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_API
            }

            if (0U != AmbaIK_SetCawarpInfo(pImgMode, &(pCaTable->CaInfo))) {
                RetVal = SVC_NG;
                PRN_CAL_CA_LOG "Fail to update ca table - set ca info fail!" PRN_CAL_CA_NG
            } else {
                RetVal = AmbaIK_SetCawarpEnb(pImgMode, 1U);
                if (RetVal != SVC_OK) {
                    PRN_CAL_CA_LOG "Fail to update ca table - enable warp fail!" PRN_CAL_CA_NG
                }
            }

            if (RetVal == SVC_OK) {
                PRN_CAL_CA_LOG "Successful to update the ca table VinID(%d) SensorID(%d) ImgContextID(%d)"
                    PRN_CAL_CA_ARG_UINT32 pCalibChan->VinID    PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_ARG_UINT32 pCalibChan->SensorID PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_ARG_UINT32 pImgMode->ContextId  PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_OK
            }
        }

        SvcCalib_CaMutexGive(&(SVC_CalibCaCtrl.Mutex));
    }

    return RetVal;
}


static UINT32 SvcCalib_CaTableCfg(UINT32 NandTableID, UINT32 TableID)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibCaCtrlFlag & SVC_CALIB_CA_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG " Fail to configure ca table - Need to initial ca control module first!" PRN_CAL_CA_NG
    } else if ((SVC_CalibCaCtrlFlag & SVC_CALIB_CA_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG " Fail to configure ca table - Need to initial ca dram shadow first!" PRN_CAL_CA_NG
    } else if (SVC_CalibCaCtrl.pNandTable == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG " Fail to configure ca table - invalid nand table!" PRN_CAL_CA_NG
    } else if (SVC_CalibCaCtrl.pCaTable == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG " Fail to configure ca table - invalid ca table!" PRN_CAL_CA_NG
    } else if (TableID >= SVC_CalibCaCtrl.NumOfCaTable) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG " Fail to configure ca table - output ca table id is out-of max table size!" PRN_CAL_CA_NG
    } else if (NandTableID >= SVC_CalibCaCtrl.NumOfCaTable) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG " Fail to configure ca table - nand ca table id is out-of max table size!" PRN_CAL_CA_NG
    } else {
        SVC_CALIB_CA_TABLE_s *pCaTable = &(SVC_CalibCaCtrl.pCaTable[TableID]);

        /* Reset the ca table context */
        AmbaSvcWrap_MisraMemset(pCaTable, 0, sizeof(SVC_CALIB_CA_TABLE_s));
        {
            const SVC_CALIB_CA_NAND_TABLE_s *pNandTable = &(SVC_CalibCaCtrl.pNandTable[NandTableID]);

            if ((pNandTable->Header.HorGridNum * pNandTable->Header.VerGridNum) > (SVC_CALIB_CA_MAX_TBL_LEN)) {
                RetVal = SVC_NG;
                PRN_CAL_CA_LOG " Warp table grid is out-of range! %dx%d"
                    PRN_CAL_CA_ARG_UINT32 pNandTable->Header.HorGridNum PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_ARG_UINT32 pNandTable->Header.VerGridNum PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_NG
            } else {
                pCaTable->CaInfo.Version                           = pNandTable->Header.Version;
                pCaTable->CaInfo.HorGridNum                        = pNandTable->Header.HorGridNum;
                pCaTable->CaInfo.VerGridNum                        = pNandTable->Header.VerGridNum;
                pCaTable->CaInfo.TileWidthExp                      = pNandTable->Header.TileWidthExp;
                pCaTable->CaInfo.TileHeightExp                     = pNandTable->Header.TileHeightExp;
                pCaTable->CaInfo.VinSensorGeo.StartX               = pNandTable->Header.VinSensorGeo.StartX;
                pCaTable->CaInfo.VinSensorGeo.StartY               = pNandTable->Header.VinSensorGeo.StartY;
                pCaTable->CaInfo.VinSensorGeo.Width                = pNandTable->Header.VinSensorGeo.Width;
                pCaTable->CaInfo.VinSensorGeo.Height               = pNandTable->Header.VinSensorGeo.Height;
                pCaTable->CaInfo.VinSensorGeo.HSubSample.FactorDen = pNandTable->Header.VinSensorGeo.HSubSample.FactorDen;
                pCaTable->CaInfo.VinSensorGeo.HSubSample.FactorNum = pNandTable->Header.VinSensorGeo.HSubSample.FactorNum;
                pCaTable->CaInfo.VinSensorGeo.VSubSample.FactorDen = pNandTable->Header.VinSensorGeo.VSubSample.FactorDen;
                pCaTable->CaInfo.VinSensorGeo.VSubSample.FactorNum = pNandTable->Header.VinSensorGeo.VSubSample.FactorNum;
                pCaTable->CaInfo.pCawarpRed                        = pCaTable->CaRVector;
                pCaTable->CaInfo.pCawarpBlue                       = pCaTable->CaBVector;
                if (pNandTable->Header.TwoStageDone == 0U) {
                    pCaTable->CaInfo.Enb2StageCompensation         = SVC_CALIB_CA_IK_2_STAGE;
                } else {
                    pCaTable->CaInfo.Enb2StageCompensation         = 0U;
                }
                pCaTable->VinSelectBits                            = pNandTable->Header.VinSelectBits;
                pCaTable->SensorSelectBits                         = pNandTable->Header.SensorSelectBits;

                AmbaSvcWrap_MisraMemcpy(pCaTable->CaRVector, pNandTable->CaRVector,
                       (sizeof(AMBA_IK_GRID_POINT_s) * pNandTable->Header.HorGridNum * pNandTable->Header.VerGridNum));

                AmbaSvcWrap_MisraMemcpy(pCaTable->CaBVector, pNandTable->CaBVector,
                       (sizeof(AMBA_IK_GRID_POINT_s) * pNandTable->Header.HorGridNum * pNandTable->Header.VerGridNum));
            }
        }
    }

    return RetVal;
}


static UINT32 SvcCalib_CaTableSearch(UINT32 VinID, UINT32 SensorID, UINT32 *pTblIdx)
{
    UINT32 RetVal = SVC_OK;

    if (SVC_OK != SvcCalib_CaVinSensorIDCheck(VinID, SensorID)) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to search ca table - invalid calib channel -> VinID(%d), SensorID(%d)!"
            PRN_CAL_CA_ARG_UINT32 VinID    PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_ARG_UINT32 SensorID PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_NG
    } else {
        UINT32 Idx, TableIdx = 0xFFFFFFFFU;

        for (Idx = 0U; Idx < SVC_CalibCaCtrl.NumOfCaTable; Idx ++) {
            if (((SVC_CalibCaCtrl.pCaTable[Idx].VinSelectBits & SvcCalib_BitGet(VinID)) > 0U) &&
                ((SVC_CalibCaCtrl.pCaTable[Idx].SensorSelectBits & SensorID) > 0U)) {
                TableIdx = Idx;
                break;
            }
        }

        if (pTblIdx != NULL) {
            *pTblIdx = TableIdx;
        }
    }

    return RetVal;
}


static UINT32 SvcCalib_CaNandTableSearch(UINT32 VinID, UINT32 SensorID, UINT32 *pTblIdx)
{
    UINT32 RetVal = SVC_OK;

    if (SVC_OK != SvcCalib_CaVinSensorIDCheck(VinID, SensorID)) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to search ca nand table - invalid calib channel -> VinID(%d), SensorID(%d)!"
            PRN_CAL_CA_ARG_UINT32 VinID    PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_ARG_UINT32 SensorID PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_NG
    } else {
        UINT32 Idx;

        if (pTblIdx != NULL) {
            *pTblIdx = 0xFFFFFFFFU;
        }

        for (Idx = 0U; Idx < SVC_CalibCaCtrl.NumOfCaTable; Idx ++) {
            if (((SVC_CalibCaCtrl.pNandTable[Idx].Header.VinSelectBits & SvcCalib_BitGet(VinID)) > 0U) &&
                ((SVC_CalibCaCtrl.pNandTable[Idx].Header.SensorSelectBits & SensorID) > 0U) &&
                ((SVC_CalibCaCtrl.pNandHeader->TableSelectBits & SvcCalib_BitGet(Idx)) > 0U)) {
                if (pTblIdx != NULL) {
                    *pTblIdx = Idx;
                }
                break;
            }
        }
    }

    return RetVal;
}


static SVC_CALIB_CA_TABLE_s * SvcCalib_CaTableGet(const SVC_CALIB_CHANNEL_s *pCalibChan)
{
    UINT32 RetVal = SVC_OK;
    SVC_CALIB_CA_TABLE_s *pTable = NULL;

    if ((SVC_CalibCaCtrlFlag & SVC_CALIB_CA_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to get ca table - Need to initial ca control module first!" PRN_CAL_CA_NG
    } else if ((SVC_CalibCaCtrlFlag & SVC_CALIB_CA_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to get ca table - Need to initial ca dram shadow first!" PRN_CAL_CA_NG
    } else if (SVC_CalibCaCtrl.pNandHeader == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to get ca table - invalid nand header!" PRN_CAL_CA_NG
    } else if (SVC_CalibCaCtrl.pNandTable == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to get ca table - invalid nand table!" PRN_CAL_CA_NG
    } else if (SVC_CalibCaCtrl.pCaTable == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to get ca table - invalid ca table!" PRN_CAL_CA_NG
    } else if (pCalibChan == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to get ca table - invalid calib channel!" PRN_CAL_CA_NG
    } else {
        UINT32 Idx;
        UINT32 TableIdx     = 0xFFFFFFFFU;
        UINT32 NandTableIdx = 0xFFFFFFFFU;

        RetVal  = SvcCalib_CaTableSearch(pCalibChan->VinID, pCalibChan->SensorID, &TableIdx);
        RetVal |= SvcCalib_CaNandTableSearch(pCalibChan->VinID, pCalibChan->SensorID, &NandTableIdx);
        if (RetVal == SVC_OK) {

            /* If not get ca table, search the nand table by calib channel */
            if (TableIdx == 0xFFFFFFFFU) {

                if (NandTableIdx < SVC_CalibCaCtrl.NumOfCaTable) {
                    /* Search free ca table */
                    for (Idx = 0; Idx < SVC_CalibCaCtrl.NumOfCaTable; Idx ++) {
                        if ((SVC_CalibCaCtrl.pCaTable[Idx].VinSelectBits == 0U) &&
                            (SVC_CalibCaCtrl.pCaTable[Idx].SensorSelectBits == 0U)) {
                            TableIdx = Idx;
                            PRN_CAL_CA_LOG "Successful to get ca free table(%d)."
                                PRN_CAL_CA_ARG_UINT32 TableIdx PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_DBG
                            break;
                        }
                    }

                    /* There is not free ca table to service it */
                    if (TableIdx == 0xFFFFFFFFU) {
                        RetVal = SVC_NG;
                        PRN_CAL_CA_LOG "Fail to get ca table - there is not free table to support VinID(%d), SensorID(0x%X)"
                            PRN_CAL_CA_ARG_UINT32 pCalibChan->VinID    PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_ARG_UINT32 pCalibChan->SensorID PRN_CAL_CA_ARG_POST
                        PRN_CAL_CA_DBG
                    }
                } else {
                    RetVal = SVC_NG;
                    PRN_CAL_CA_LOG "Fail to get ca table - current ca cannot support VinID(%d), SensorID(0x%X)"
                        PRN_CAL_CA_ARG_UINT32 pCalibChan->VinID    PRN_CAL_CA_ARG_POST
                        PRN_CAL_CA_ARG_UINT32 pCalibChan->SensorID PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_DBG
                }
            }

            /* generate ca table */
            if ((TableIdx < SVC_CalibCaCtrl.NumOfCaTable) && (NandTableIdx < SVC_CalibCaCtrl.NumOfCaTable)) {
                RetVal = SvcCalib_CaTableCfg(NandTableIdx, TableIdx);
                if (RetVal != SVC_OK) {
                    PRN_CAL_CA_LOG "Fail to get ca table - re-configure ca table fail!" PRN_CAL_CA_DBG
                    pTable = NULL;
                } else {
                    PRN_CAL_CA_LOG "Successful to get ca table - configure done!" PRN_CAL_CA_DBG
                    pTable = &(SVC_CalibCaCtrl.pCaTable[TableIdx]);
                }
            } else {
                if (TableIdx < SVC_CalibCaCtrl.NumOfCaTable) {
                    pTable = &(SVC_CalibCaCtrl.pCaTable[TableIdx]);
                    PRN_CAL_CA_LOG "Successful to get ca table - existed table!" PRN_CAL_CA_OK
                }
            }
        }
    }

    AmbaMisra_TouchUnused(&RetVal);

    return pTable;
}


static UINT32 SvcCalib_CaTableGenTbl(const SVC_CALIB_CHANNEL_s *pCalibChan, const char *pScriptPath)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibCaCtrlFlag & SVC_CALIB_CA_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to calc ca table - Need to initial ca control module first!" PRN_CAL_CA_NG
    } else if ((SVC_CalibCaCtrlFlag & SVC_CALIB_CA_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to calc ca table - Need to initial ca dram shadow first!" PRN_CAL_CA_NG
    } else if (SVC_CalibCaCtrl.pNandHeader == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to calc ca table - invalid nand header!" PRN_CAL_CA_NG
    } else if (SVC_CalibCaCtrl.pNandTable == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to calc ca table - invalid nand table!" PRN_CAL_CA_NG
    } else if (pCalibChan == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to calc ca table - invalid calib channel!" PRN_CAL_CA_NG
    } else if (SVC_OK != SvcCalib_CaVinSensorIDCheck(pCalibChan->VinID, pCalibChan->SensorID)) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to calc ca table - invalid calib channel -> VinID(%d), SensorID(%d)!"
            PRN_CAL_CA_ARG_UINT32 pCalibChan->VinID    PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_ARG_UINT32 pCalibChan->SensorID PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_NG
    } else if (SVC_OK != SvcCalib_CaMemLock()) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to calc ca table - lock ca memory fail!" PRN_CAL_CA_NG
    } else if (KAL_ERR_NONE != SvcCalib_CaMutexTake(&(SVC_CalibCaCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to calc wb table - take mutex fail!" PRN_CAL_CA_NG
    } else {
        AMBA_CT_INITIAL_CONFIG_s CtnCfg;

        AmbaSvcWrap_MisraMemset(&CtnCfg, 0, sizeof(CtnCfg));

        CtnCfg.pTunerWorkingBuf    = SVC_CalibCaCtrl.MemCtrl[SVC_CALIB_CA_MEM_CALC_CTUNER].pBuf;
        CtnCfg.TunerWorkingBufSize = SVC_CalibCaCtrl.MemCtrl[SVC_CALIB_CA_MEM_CALC_CTUNER].BufSize;

        CtnCfg.Ca.pCalibWorkingBuf    = SVC_CalibCaCtrl.MemCtrl[SVC_CALIB_CA_MEM_CALC].pBuf;
        CtnCfg.Ca.CalibWorkingBufSize = SVC_CalibCaCtrl.MemCtrl[SVC_CALIB_CA_MEM_CALC].BufSize;

        PRetVal = AmbaCT_Init(AMBA_CT_TYPE_CA, &CtnCfg);
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_CAL_CA_LOG "Fail to calc ca by ctuner - initial ctuner fail! 0x%08X"
                PRN_CAL_CA_ARG_UINT32 PRetVal PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_NG
        } else {
            PRetVal = AmbaCT_Load(pScriptPath);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_CAL_CA_LOG "Fail to calc ca by ctuner - load ctuner script fail! 0x%08X"
                    PRN_CAL_CA_ARG_UINT32 PRetVal PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_NG
            } else {
                PRetVal = AmbaCT_Execute();
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    PRN_CAL_CA_LOG "Fail to calc ca by ctuner - execute ctuner fail! 0x%08X"
                        PRN_CAL_CA_ARG_UINT32 PRetVal PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_NG
                } else {
                    AMBA_CT_CA_CALIB_DATA_s CaData;
                    const AMBA_CAL_CA_DATA_s *pData;
                    const AMBA_CAL_CA_SEPARATE_DATA_s *pSepData;

                    AmbaSvcWrap_MisraMemset(&CaData, 0, sizeof(CaData));

                    PRetVal = AmbaCT_CaGetCalibData(&CaData); PRN_CAL_CA_ERR_HDLR

                    pData    = CaData.pCalibData;
                    pSepData = CaData.pSeparateCalibData;

                    if ((pData == NULL) && (pSepData == NULL)) {
                        RetVal = SVC_NG;
                        PRN_CAL_CA_LOG "Fail to calc ca table - invalid ctuner result!" PRN_CAL_CA_NG
                    } else if (SVC_CalibCaCtrl.CalcCaCtrl.TableIdx >= 32U) {
                        RetVal = SVC_NG;
                        PRN_CAL_CA_LOG "Fail to calc ca table - table id(%d) is out-of range"
                            PRN_CAL_CA_ARG_UINT32 SVC_CalibCaCtrl.CalcCaCtrl.TableIdx PRN_CAL_CA_ARG_POST
                        PRN_CAL_CA_NG
                    } else {
                        SVC_CALIB_CA_NAND_HEADER_s *pNandHeader =  SVC_CalibCaCtrl.pNandHeader;
                        SVC_CALIB_CA_NAND_TABLE_s  *pNandTable = &(SVC_CalibCaCtrl.pNandTable[SVC_CalibCaCtrl.CalcCaCtrl.TableIdx]);

                        AmbaSvcWrap_MisraMemset(pNandTable, 0, sizeof(SVC_CALIB_CA_NAND_TABLE_s));

                        if (pData != NULL) {
                            if ((pData->HorGridNum * pData->VerGridNum) > SVC_CALIB_CA_MAX_TBL_LEN) {
                                PRetVal = SVC_CALIB_CA_MAX_TBL_LEN;
                                PRN_CAL_CA_LOG "Fail to calc ca table - the grid number is out-of range! %dx%d > %d"
                                    PRN_CAL_CA_ARG_UINT32 pData->HorGridNum PRN_CAL_CA_ARG_POST
                                    PRN_CAL_CA_ARG_UINT32 pData->VerGridNum PRN_CAL_CA_ARG_POST
                                    PRN_CAL_CA_ARG_UINT32 PRetVal           PRN_CAL_CA_ARG_POST
                                PRN_CAL_CA_NG
                                RetVal = SVC_NG;
                            } else {
                                AMBA_CAL_GRID_POINT_s *pCaRedTbl;
                                AMBA_CAL_GRID_POINT_s *pCaBlueTbl;
#ifndef CONFIG_SOC_CV2FS
                                pCaRedTbl  = pNandTable->CaRVector;
                                pCaBlueTbl = pNandTable->CaBVector;
#else
                                AmbaMisra_TypeCast(&pCaRedTbl, &(SVC_CalibCaCtrl.MemCtrl[SVC_CALIB_CA_MEM_CALC_REMAP_R].pBuf));
                                AmbaMisra_TypeCast(&pCaBlueTbl, &(SVC_CalibCaCtrl.MemCtrl[SVC_CALIB_CA_MEM_CALC_REMAP_B].pBuf));
#endif
                                pNandTable->Header.Version                           = SVC_CALIB_CA_VERSION;
                                pNandTable->Header.HorGridNum                        = pData->HorGridNum;
                                pNandTable->Header.VerGridNum                        = pData->VerGridNum;
                                pNandTable->Header.TileWidthExp                      = pData->TileWidthExp;
                                pNandTable->Header.TileHeightExp                     = pData->TileHeightExp;
                                pNandTable->Header.VinSensorGeo.StartX               = pData->CalibSensorGeo.StartX;
                                pNandTable->Header.VinSensorGeo.StartY               = pData->CalibSensorGeo.StartY;
                                pNandTable->Header.VinSensorGeo.Width                = pData->CalibSensorGeo.Width;
                                pNandTable->Header.VinSensorGeo.Height               = pData->CalibSensorGeo.Height;
                                pNandTable->Header.VinSensorGeo.HSubSample.FactorDen = pData->CalibSensorGeo.HSubSample.FactorDen;
                                pNandTable->Header.VinSensorGeo.HSubSample.FactorNum = pData->CalibSensorGeo.HSubSample.FactorNum;
                                pNandTable->Header.VinSensorGeo.VSubSample.FactorDen = pData->CalibSensorGeo.VSubSample.FactorDen;
                                pNandTable->Header.VinSensorGeo.VSubSample.FactorNum = pData->CalibSensorGeo.VSubSample.FactorNum;
                                pNandTable->Header.VinSelectBits                     = SVC_CalibCaCtrl.CalibChan.VinSelectBits;
                                pNandTable->Header.SensorSelectBits                  = SVC_CalibCaCtrl.CalibChan.SensorSelectBits;
                                if (CAL_OK != AmbaCal_CaSeparateRBTbl(pData, pCaRedTbl, pCaBlueTbl)) {
                                    PRN_CAL_CA_LOG  "Fail to calc ca table - separate ca table fail! ErrCode: %d ErrMsg: %s"
                                        PRN_CAL_CA_ARG_UINT32 RetVal                     PRN_CAL_CA_ARG_POST
                                        PRN_CAL_CA_ARG_CSTR   AmbaCal_ErrNoToMsg(RetVal) PRN_CAL_CA_ARG_POST
                                    PRN_CAL_CA_NG
                                    RetVal = SVC_NG;
                                } else {
#ifndef CONFIG_SOC_CV2FS
                                    PRetVal = SvcCalib_CaTableRemap(pNandTable, NULL, NULL); PRN_CAL_CA_ERR_HDLR
#else
                                    RetVal = SvcCalib_CaTableRemap(pNandTable, pCaRedTbl, pCaBlueTbl);
#endif
                                }
                            }
                        } else {
                            if ((pSepData->HorGridNum * pSepData->VerGridNum) > SVC_CALIB_CA_MAX_TBL_LEN) {
                                PRetVal = SVC_CALIB_CA_MAX_TBL_LEN;
                                PRN_CAL_CA_LOG "Fail to calc ca table - the separate grid number is out-of range! %dx%d > %d"
                                    PRN_CAL_CA_ARG_UINT32 pSepData->HorGridNum PRN_CAL_CA_ARG_POST
                                    PRN_CAL_CA_ARG_UINT32 pSepData->VerGridNum PRN_CAL_CA_ARG_POST
                                    PRN_CAL_CA_ARG_UINT32 PRetVal              PRN_CAL_CA_ARG_POST
                                PRN_CAL_CA_NG
                                RetVal = SVC_NG;
                            } else if (pSepData->pRCaTbl == NULL) {
                                RetVal = SVC_NG;
                                PRN_CAL_CA_LOG "Fail to calc ca table - separate red vector should not null!" PRN_CAL_CA_NG
                            } else if (pSepData->pBCaTbl == NULL) {
                                RetVal = SVC_NG;
                                PRN_CAL_CA_LOG "Fail to calc ca table - separate blue vector should not null!" PRN_CAL_CA_NG
                            } else {
                                pNandTable->Header.Version                           = SVC_CALIB_CA_VERSION;
                                pNandTable->Header.HorGridNum                        = pSepData->HorGridNum;
                                pNandTable->Header.VerGridNum                        = pSepData->VerGridNum;
                                pNandTable->Header.TileWidthExp                      = pSepData->TileWidthExp;
                                pNandTable->Header.TileHeightExp                     = pSepData->TileHeightExp;
                                pNandTable->Header.VinSensorGeo.StartX               = pSepData->CalibSensorGeo.StartX;
                                pNandTable->Header.VinSensorGeo.StartY               = pSepData->CalibSensorGeo.StartY;
                                pNandTable->Header.VinSensorGeo.Width                = pSepData->CalibSensorGeo.Width;
                                pNandTable->Header.VinSensorGeo.Height               = pSepData->CalibSensorGeo.Height;
                                pNandTable->Header.VinSensorGeo.HSubSample.FactorDen = pSepData->CalibSensorGeo.HSubSample.FactorDen;
                                pNandTable->Header.VinSensorGeo.HSubSample.FactorNum = pSepData->CalibSensorGeo.HSubSample.FactorNum;
                                pNandTable->Header.VinSensorGeo.VSubSample.FactorDen = pSepData->CalibSensorGeo.VSubSample.FactorDen;
                                pNandTable->Header.VinSensorGeo.VSubSample.FactorNum = pSepData->CalibSensorGeo.VSubSample.FactorNum;
                                pNandTable->Header.VinSelectBits                     = SVC_CalibCaCtrl.CalibChan.VinSelectBits;
                                pNandTable->Header.SensorSelectBits                  = SVC_CalibCaCtrl.CalibChan.SensorSelectBits;
#ifndef CONFIG_SOC_CV2FS
                                {
                                    UINT32 TblLen = pSepData->HorGridNum * pSepData->VerGridNum;

                                    AmbaSvcWrap_MisraMemcpy(pNandTable->CaRVector, pSepData->pRCaTbl, sizeof(AMBA_IK_GRID_POINT_s)*TblLen);
                                    AmbaSvcWrap_MisraMemcpy(pNandTable->CaBVector, pSepData->pBCaTbl, sizeof(AMBA_IK_GRID_POINT_s)*TblLen);
                                }
#else
                                RetVal = SvcCalib_CaTableRemap(pNandTable, pSepData->pRCaTbl, pSepData->pBCaTbl);
#endif
                            }
                        }

                        if (RetVal == SVC_OK) {
                            pNandHeader->Enable = 1U;
                            pNandHeader->DebugMsgOn = SVC_CalibCaCtrl.EnableDebugMsg;
                            if ((pNandHeader->TableSelectBits & SvcCalib_BitGet(SVC_CalibCaCtrl.CalcCaCtrl.TableIdx)) == 0U) {
                                pNandHeader->TableSelectBits |= SvcCalib_BitGet(SVC_CalibCaCtrl.CalcCaCtrl.TableIdx);
                                pNandHeader->NumOfTable ++;
                            }

                            PRN_CAL_CA_LOG " " PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "====== Calibration ca info ======" PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "  Version                             : 0x%X"
                                PRN_CAL_CA_ARG_UINT32 pNandTable->Header.Version PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "  HorGridNum                          : %d"
                                PRN_CAL_CA_ARG_UINT32   pNandTable->Header.HorGridNum PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "  VerGridNum                          : %d"
                                PRN_CAL_CA_ARG_UINT32   pNandTable->Header.VerGridNum PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "  TileWidthExp                        : %d"
                                PRN_CAL_CA_ARG_UINT32   pNandTable->Header.TileWidthExp PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "  TileHeightExp                       : %d"
                                PRN_CAL_CA_ARG_UINT32   pNandTable->Header.TileHeightExp PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "  CalibSensorGeo.StartX               : %d"
                                PRN_CAL_CA_ARG_UINT32   pNandTable->Header.VinSensorGeo.StartX PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "  CalibSensorGeo.StartY               : %d"
                                PRN_CAL_CA_ARG_UINT32   pNandTable->Header.VinSensorGeo.StartY PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "  CalibSensorGeo.Width                : %d"
                                PRN_CAL_CA_ARG_UINT32   pNandTable->Header.VinSensorGeo.Width PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "  CalibSensorGeo.Height               : %d"
                                PRN_CAL_CA_ARG_UINT32   pNandTable->Header.VinSensorGeo.Height PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "  CalibSensorGeo.HSubSample.FactorDen : %d"
                                PRN_CAL_CA_ARG_UINT32   pNandTable->Header.VinSensorGeo.HSubSample.FactorDen PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "  CalibSensorGeo.HSubSample.FactorNum : %d"
                                PRN_CAL_CA_ARG_UINT32   pNandTable->Header.VinSensorGeo.HSubSample.FactorNum PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "  CalibSensorGeo.VSubSample.FactorDen : %d"
                                PRN_CAL_CA_ARG_UINT32   pNandTable->Header.VinSensorGeo.VSubSample.FactorDen PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "  CalibSensorGeo.VSubSample.FactorNum : %d"
                                PRN_CAL_CA_ARG_UINT32   pNandTable->Header.VinSensorGeo.VSubSample.FactorNum PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "  TwoStageDone                        : %d"
                                PRN_CAL_CA_ARG_UINT32   pNandTable->Header.TwoStageDone PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "  VinSelectBits                       : 0x%x"
                                PRN_CAL_CA_ARG_UINT32 pNandTable->Header.VinSelectBits PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "  SensorSelectBits                    : 0x%x"
                                PRN_CAL_CA_ARG_UINT32 pNandTable->Header.SensorSelectBits PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "  CaRVector                           : %p"
                                PRN_CAL_CA_ARG_CPOINT  pNandTable->CaRVector PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API
                            PRN_CAL_CA_LOG "  CaBVector                           : %p"
                                PRN_CAL_CA_ARG_CPOINT  pNandTable->CaBVector PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_API

                            PRN_CAL_CA_LOG "Successful to calc ca table" PRN_CAL_CA_OK

                            /* Update to NAND */
                            RetVal = SvcCalib_DataSave(SVC_CALIB_CA_ID);
                            if (RetVal != OK) {
                                PRN_CAL_CA_LOG "Fail to save ca to nand!" PRN_CAL_CA_NG
                            }
                        }

                    }
                }
            }
        }

        SvcCalib_CaMutexGive(&(SVC_CalibCaCtrl.Mutex));

        SvcCalib_CaMemUnLock();
    }

    return RetVal;
}

static UINT32 SvcCalib_CaTableRemap(SVC_CALIB_CA_NAND_TABLE_s *pNandTbl, AMBA_IK_GRID_POINT_s *pCaRedTbl, AMBA_IK_GRID_POINT_s *pCaBlueTbl)
{
    UINT32 RetVal = SVC_NG;

#ifdef CONFIG_SOC_CV2FS
    if ((pNandTbl != NULL) && (pCaRedTbl != NULL) && (pCaBlueTbl != NULL)) {
        UINT32 PRetVal;
        AMBA_IK_CAWARP_INFO_s InputInfo, ResultInfo;
        AMBA_IK_IN_CAWARP_PRE_PROC_s TwoStageProcIn;
        AMBA_IK_OUT_CAWARP_PRE_PROC_s TwoStageProcOut;

        AmbaSvcWrap_MisraMemset(&InputInfo, 0, sizeof(InputInfo));
        InputInfo.Version                           = pNandTbl->Header.Version;
        InputInfo.HorGridNum                        = pNandTbl->Header.HorGridNum;
        InputInfo.VerGridNum                        = pNandTbl->Header.VerGridNum;
        InputInfo.TileWidthExp                      = pNandTbl->Header.TileWidthExp;
        InputInfo.TileHeightExp                     = pNandTbl->Header.TileHeightExp;
        InputInfo.VinSensorGeo.StartX               = pNandTbl->Header.VinSensorGeo.StartX;
        InputInfo.VinSensorGeo.StartY               = pNandTbl->Header.VinSensorGeo.StartY;
        InputInfo.VinSensorGeo.Width                = pNandTbl->Header.VinSensorGeo.Width;
        InputInfo.VinSensorGeo.Height               = pNandTbl->Header.VinSensorGeo.Height;
        InputInfo.VinSensorGeo.HSubSample.FactorDen = pNandTbl->Header.VinSensorGeo.HSubSample.FactorDen;
        InputInfo.VinSensorGeo.HSubSample.FactorNum = pNandTbl->Header.VinSensorGeo.HSubSample.FactorNum;
        InputInfo.VinSensorGeo.VSubSample.FactorDen = pNandTbl->Header.VinSensorGeo.VSubSample.FactorDen;
        InputInfo.VinSensorGeo.VSubSample.FactorNum = pNandTbl->Header.VinSensorGeo.VSubSample.FactorNum;
        InputInfo.pCawarpRed                        = pCaRedTbl;
        InputInfo.pCawarpBlue                       = pCaBlueTbl;
        AmbaSvcWrap_MisraMemset(&TwoStageProcIn, 0, sizeof(TwoStageProcIn));
        TwoStageProcIn.pInpuIinfo                   = &InputInfo;
        TwoStageProcIn.pWorkingBuffer               = SVC_CalibCaCtrl.MemCtrl[SVC_CALIB_CA_MEM_CALC_REMAP].pBuf;
        TwoStageProcIn.WorkingBufferSize            = SVC_CalibCaCtrl.MemCtrl[SVC_CALIB_CA_MEM_CALC_REMAP].BufSize;

        AmbaSvcWrap_MisraMemset(&ResultInfo, 0, sizeof(ResultInfo));
        ResultInfo.pCawarpRed  = pNandTbl->CaRVector;
        ResultInfo.pCawarpBlue = pNandTbl->CaBVector;
        AmbaSvcWrap_MisraMemset(&TwoStageProcOut, 0, sizeof(TwoStageProcOut));
        TwoStageProcOut.pResultInfo                 = &ResultInfo;

        PRetVal = AmbaIK_PreProcCAWarpTbl(&TwoStageProcIn, &TwoStageProcOut);
        if (PRetVal != 0U) {
            PRN_CAL_CA_LOG  "Fail to calc ca table - remap fail! ErrCode: %d"
                PRN_CAL_CA_ARG_UINT32 PRetVal PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_NG
            RetVal = SVC_NG;
        } else {
            pNandTbl->Header.TwoStageDone = 1U;
            RetVal = SVC_OK;
        }
    }
#else
    AmbaMisra_TouchUnused(pNandTbl);
    AmbaMisra_TouchUnused(pCaRedTbl);
    AmbaMisra_TouchUnused(pCaBlueTbl);
    if (pNandTbl->Header.HorGridNum > 0U) {
        RetVal = SVC_OK;
    }
#endif

    return RetVal;
}

static UINT32 SvcCalib_CaTableDataSet(UINT32 TblID, SVC_CALIB_TBL_INFO_s *pTblInfo)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibCaCtrlFlag & SVC_CALIB_CA_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to set ca table - Need to initial ca control module first!" PRN_CAL_CA_NG
    } else if ((SVC_CalibCaCtrlFlag & SVC_CALIB_CA_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to set ca table - Need to initial ca dram shadow first!" PRN_CAL_CA_NG
    } else if (SVC_CalibCaCtrl.pNandHeader == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to set ca table - invalid nand header!" PRN_CAL_CA_NG
    } else if (SVC_CalibCaCtrl.pNandTable == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to set ca table - invalid nand table!" PRN_CAL_CA_NG
    } else if (TblID >= SVC_CalibCaCtrl.NumOfCaTable) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to set ca table - the TableID(%d) is out-of range(%d)!"
            PRN_CAL_CA_ARG_UINT32 TblID                        PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_ARG_UINT32 SVC_CalibCaCtrl.NumOfCaTable PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_NG
    } else if (pTblInfo == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to set ca table - invalid table info!" PRN_CAL_CA_NG
    } else if (pTblInfo->pBuf == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to set ca table - invalid table!" PRN_CAL_CA_NG
    } else if (pTblInfo->BufSize < (UINT32)sizeof(SVC_CALIB_CA_TBL_DATA_s)) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to set ca table - input table size(0x%X) does not match support table size(0x%X)!"
            PRN_CAL_CA_ARG_UINT32 pTblInfo->BufSize                       PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_ARG_UINT32 (UINT32)sizeof(SVC_CALIB_CA_TBL_DATA_s) PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_NG
    } else if (KAL_ERR_NONE != SvcCalib_CaMutexTake(&(SVC_CalibCaCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to set ca table - take mutex fail!" PRN_CAL_CA_NG
    } else {
        SVC_CALIB_CA_TBL_DATA_s    *pInput      = NULL;
        SVC_CALIB_CA_NAND_TABLE_s  *pNandTbl    = &(SVC_CalibCaCtrl.pNandTable[TblID]);
        SVC_CALIB_CA_NAND_HEADER_s *pNandHeader = SVC_CalibCaCtrl.pNandHeader;

        AmbaMisra_TypeCast(&(pInput), &(pTblInfo->pBuf));

        if (pInput->pCaRed == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_CA_LOG "Fail to set ca table - input ca red vector should not null!" PRN_CAL_CA_NG
        } else if (pInput->pCaBlue == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_CA_LOG "Fail to set ca table - input ca blue vector should not null!" PRN_CAL_CA_NG
        } else if (pInput->CaRedLength != pInput->CaBlueLength) {
            RetVal = SVC_NG;
            PRN_CAL_CA_LOG "Fail to set ca table - input ca red vector length(%d) should same with blue vector length(%d)!"
                PRN_CAL_CA_ARG_UINT32 pInput->CaRedLength  PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_ARG_UINT32 pInput->CaBlueLength PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_NG
        } else if (pInput->CaRedLength > SVC_CALIB_CA_MAX_TBL_LEN) {
            UINT32 TempU32 = SVC_CALIB_CA_MAX_TBL_LEN;
            RetVal = SVC_NG;
            PRN_CAL_CA_LOG "Fail to set ca table - input ca vector(%d) is out-of range(%d)!"
                PRN_CAL_CA_ARG_UINT32 pInput->CaRedLength PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_ARG_UINT32 TempU32             PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_NG
        } else {

            AmbaSvcWrap_MisraMemset(pNandTbl, 0, sizeof(SVC_CALIB_CA_NAND_TABLE_s));

            pNandTbl->Header.Version          = SVC_CALIB_CA_VERSION;
            pNandTbl->Header.HorGridNum       = pInput->HorGridNum;
            pNandTbl->Header.VerGridNum       = pInput->VerGridNum;
            pNandTbl->Header.TileWidthExp     = pInput->TileWidthExp;
            pNandTbl->Header.TileHeightExp    = pInput->TileHeightExp;
            pNandTbl->Header.VinSelectBits    = pTblInfo->CalChan.VinSelectBits;
            pNandTbl->Header.SensorSelectBits = pTblInfo->CalChan.SensorSelectBits;
            AmbaSvcWrap_MisraMemcpy(&(pNandTbl->Header.VinSensorGeo), &(pInput->VinSensorGeo), sizeof(SVC_CALIB_VIN_SENSOR_GEO_s));
            AmbaSvcWrap_MisraMemcpy(pNandTbl->CaRVector, pInput->pCaRed, sizeof(AMBA_IK_GRID_POINT_s)*(pInput->CaRedLength));
            AmbaSvcWrap_MisraMemcpy(pNandTbl->CaBVector, pInput->pCaBlue, sizeof(AMBA_IK_GRID_POINT_s)*(pInput->CaBlueLength));

            // Update nand header
            if (pNandHeader->Enable == 0U) {
                pNandHeader->Enable = 1U;
            }
            if (pNandHeader->DebugMsgOn == 0U) {
                pNandHeader->DebugMsgOn = SVC_CalibCaCtrl.EnableDebugMsg;
            }
            if (pNandHeader->DebugMsgOn == 0U) {
                pNandHeader->DebugMsgOn = pTblInfo->DbgMsgOn;
            }
            if (SVC_CalibCaCtrl.EnableDebugMsg == 0U) {
                SVC_CalibCaCtrl.EnableDebugMsg = pTblInfo->DbgMsgOn;
            }
            if ((pNandHeader->TableSelectBits & SvcCalib_BitGet(TblID)) == 0U) {
                pNandHeader->TableSelectBits |= SvcCalib_BitGet(TblID);
                pNandHeader->NumOfTable += 1U;
            }

            /* Update to NAND */
            RetVal = SvcCalib_DataSave(SVC_CALIB_CA_ID);
            if (RetVal != OK) {
                PRN_CAL_CA_LOG "Fail to save ca to nand!" PRN_CAL_CA_NG
            }

            AmbaMisra_TouchUnused(pInput);
            AmbaMisra_TouchUnused(pTblInfo);
        }

        SvcCalib_CaMutexGive(&(SVC_CalibCaCtrl.Mutex));
    }

    return RetVal;
}


static UINT32 SvcCalib_CaTableDataGet(UINT32 TblID, SVC_CALIB_TBL_INFO_s *pTblInfo)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibCaCtrlFlag & SVC_CALIB_CA_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to get ca table - Need to initial ca control module first!" PRN_CAL_CA_NG
    } else if ((SVC_CalibCaCtrlFlag & SVC_CALIB_CA_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to get ca table - Need to initial ca dram shadow first!" PRN_CAL_CA_NG
    } else if (SVC_CalibCaCtrl.pNandTable == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to get ca table - invalid nand table!" PRN_CAL_CA_NG
    } else if (SVC_CalibCaCtrl.pNandHeader->NumOfTable == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "There is not ca table!" PRN_CAL_CA_DBG
    } else if (pTblInfo == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to get ca table - invalid table info!" PRN_CAL_CA_NG
    } else if (pTblInfo->pBuf == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to get ca table - invalid table!" PRN_CAL_CA_NG
    } else if (pTblInfo->BufSize < (UINT32)sizeof(SVC_CALIB_CA_TBL_DATA_s)) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to get ca table - output table size(0x%X) is too small to service ca table size(0x%X)!"
            PRN_CAL_CA_ARG_UINT32 pTblInfo->BufSize                       PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_ARG_UINT32 (UINT32)sizeof(SVC_CALIB_CA_TBL_DATA_s) PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_NG
    } else if (KAL_ERR_NONE != SvcCalib_CaMutexTake(&(SVC_CalibCaCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to get ca table - take mutex fail!" PRN_CAL_CA_NG
    } else {
        UINT32 CurTblID = 0xFFFFFFFFU;
        SVC_CALIB_CA_TBL_DATA_s         *pOutput = NULL;
        const SVC_CALIB_CA_NAND_TABLE_s *pNandTbl;

        AmbaMisra_TypeCast(&(pOutput), &(pTblInfo->pBuf));
        if (TblID == 255U) {
            if (0U != SvcCalib_CaNandTableSearch(pTblInfo->CalChan.VinID, pTblInfo->CalChan.SensorID, &CurTblID)) {
                CurTblID = 0xFFFFFFFFU;
            }
        } else {
            CurTblID = TblID;
        }

        if (CurTblID >= SVC_CalibCaCtrl.NumOfCaTable) {
            RetVal = SVC_NG;

            PRetVal = (SVC_CalibCaCtrl.NumOfCaTable == 0U)?0U:(SVC_CalibCaCtrl.NumOfCaTable - 1U);
            PRN_CAL_CA_LOG "Fail to get ca table - the TableID(0x%X) is out-of range(0 ~ %d)"
                PRN_CAL_CA_ARG_UINT32 CurTblID PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_ARG_UINT32 PRetVal  PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_NG
        } else {
            pNandTbl = &(SVC_CalibCaCtrl.pNandTable[CurTblID]);

            if (pOutput->pCaRed == NULL) {
                RetVal = SVC_NG;
                PRN_CAL_CA_LOG "Fail to get ca table - invalid output ca red table address!" PRN_CAL_CA_NG
            } else if (pOutput->pCaBlue == NULL) {
                RetVal = SVC_NG;
                PRN_CAL_CA_LOG "Fail to get ca table - invalid output ca blue table address!" PRN_CAL_CA_NG
            } else if (pOutput->CaRedLength < SVC_CALIB_CA_MAX_TBL_LEN) {
                PRetVal = SVC_CALIB_CA_MAX_TBL_LEN;
                RetVal = SVC_NG;
                PRN_CAL_CA_LOG "Fail to get ca table - output ca red table length(%d) is too small to service cur ca table(%d)!"
                    PRN_CAL_CA_ARG_UINT32 pOutput->CaRedLength PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_ARG_UINT32 PRetVal              PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_NG
            } else if (pOutput->CaBlueLength < SVC_CALIB_CA_MAX_TBL_LEN) {
                PRetVal = SVC_CALIB_CA_MAX_TBL_LEN;
                RetVal = SVC_NG;
                PRN_CAL_CA_LOG "Fail to get ca table - output ca blue table length(%d) is too small to service cur ca table(%d)!"
                    PRN_CAL_CA_ARG_UINT32 pOutput->CaBlueLength PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_ARG_UINT32 PRetVal               PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_NG
            } else {
                UINT32 CurTblLen       = pNandTbl->Header.HorGridNum * pNandTbl->Header.VerGridNum;
                pOutput->HorGridNum    = pNandTbl->Header.HorGridNum;
                pOutput->VerGridNum    = pNandTbl->Header.VerGridNum;
                pOutput->TileWidthExp  = pNandTbl->Header.TileWidthExp;
                pOutput->TileHeightExp = pNandTbl->Header.TileHeightExp;
                AmbaSvcWrap_MisraMemcpy(&(pOutput->VinSensorGeo), &(pNandTbl->Header.VinSensorGeo), sizeof(SVC_CALIB_VIN_SENSOR_GEO_s));
                AmbaSvcWrap_MisraMemset(pOutput->pCaRed, 0, sizeof(AMBA_IK_GRID_POINT_s)*pOutput->CaRedLength);
                AmbaSvcWrap_MisraMemcpy(pOutput->pCaRed, pNandTbl->CaRVector, sizeof(AMBA_IK_GRID_POINT_s)*CurTblLen);
                AmbaSvcWrap_MisraMemset(pOutput->pCaBlue, 0, sizeof(AMBA_IK_GRID_POINT_s)*pOutput->CaBlueLength);
                AmbaSvcWrap_MisraMemcpy(pOutput->pCaBlue, pNandTbl->CaBVector, sizeof(AMBA_IK_GRID_POINT_s)*CurTblLen);

                pTblInfo->CalChan.VinSelectBits    = pNandTbl->Header.VinSelectBits;
                pTblInfo->CalChan.SensorSelectBits = pNandTbl->Header.SensorSelectBits;

                PRN_CAL_CA_LOG "Success to get ca table from TableID(%d), VinSelectBits(0x%X) SensorSelectBits(0x%X)"
                    PRN_CAL_CA_ARG_UINT32 CurTblID                           PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_ARG_UINT32 pTblInfo->CalChan.VinSelectBits    PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_ARG_UINT32 pTblInfo->CalChan.SensorSelectBits PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_DBG
            }
        }

        SvcCalib_CaMutexGive(&(SVC_CalibCaCtrl.Mutex));

        AmbaMisra_TouchUnused(pTblInfo);
    }

    return RetVal;
}

static void SvcCalib_CaShellStrToU32(const char *pString, UINT32 *pValue)
{
    UINT32 ErrCode = AmbaUtility_StringToUInt32(pString, pValue);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }
}

static UINT32 SvcCalib_CaShellCfgVinSen(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibCaCtrlFlag & SVC_CALIB_CA_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to cfg ca vin/sensor id - initial ca module first!" PRN_CAL_CA_NG
    } else if (ArgCount < 7U) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to cfg ca vin/sensor id - Argc should >= 8" PRN_CAL_CA_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to cfg ca vin/sensor id - Argv should not null!" PRN_CAL_CA_NG
    } else {
        UINT32 VinID            = 0U;
        UINT32 SensorID         = 0U;
        UINT32 VinSelectBits    = 0U;
        UINT32 SensorSelectBits = 0U;

        SvcCalib_CaShellStrToU32(pArgVector[3U], &VinID           );
        SvcCalib_CaShellStrToU32(pArgVector[4U], &SensorID        );
        SvcCalib_CaShellStrToU32(pArgVector[5U], &VinSelectBits   );
        SvcCalib_CaShellStrToU32(pArgVector[6U], &SensorSelectBits);

        if (0U != SvcCalib_CaVinSensorIDCheck(VinID, SensorID)) {
            RetVal = SVC_NG;
            PRN_CAL_CA_LOG "Fail to cfg ca vin/sensor id - invalid VinID(%d)/SensorID(0x%X)"
                PRN_CAL_CA_ARG_UINT32 VinID    PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_ARG_UINT32 SensorID PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_NG
        } else {

            /* Initial the ca calculate parameters */
            AmbaSvcWrap_MisraMemset(&(SVC_CalibCaCtrl.CalcCaCtrl), 0, sizeof(SVC_CALIB_CA_CALCULATE_CTRL_s));
            SVC_CalibCaCtrlFlag &= ~( SVC_CALIB_CA_FLG_CALC_INIT | SVC_CALIB_CA_FLG_CALC_VIN_SEN);

            SVC_CalibCaCtrl.CalcCaCtrl.SaveBinOn                    = 1U;
            SVC_CalibCaCtrl.CalcCaCtrl.TableIdx                     = 0xFFFFFFFFU;
            SVC_CalibCaCtrlFlag |= SVC_CALIB_CA_FLG_CALC_INIT;

            PRN_CAL_CA_LOG " " PRN_CAL_CA_API
            PRN_CAL_CA_LOG "------ Calibration CA initial setting ------" PRN_CAL_CA_API
            PRN_CAL_CA_LOG "  SaveBinOn                    : %d"
                PRN_CAL_CA_ARG_UINT32   SVC_CalibCaCtrl.CalcCaCtrl.SaveBinOn PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_API
            PRN_CAL_CA_LOG "  TableIdx                     : 0x%X"
                PRN_CAL_CA_ARG_UINT32 SVC_CalibCaCtrl.CalcCaCtrl.TableIdx PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_API

            /* Configure the calib channel */
            AmbaSvcWrap_MisraMemset(&(SVC_CalibCaCtrl.CalibChan), 0, sizeof(SVC_CALIB_CHANNEL_s));
            SVC_CalibCaCtrl.CalibChan.VinSelectBits    = VinSelectBits   ;
            SVC_CalibCaCtrl.CalibChan.SensorSelectBits = SensorSelectBits;
            SVC_CalibCaCtrl.CalibChan.VinID            = VinID           ;
            SVC_CalibCaCtrl.CalibChan.SensorID         = SensorID        ;
            SVC_CalibCaCtrl.CalibChan.ExtendData       = 0xFFFFFFFFU     ;
            SVC_CalibCaCtrlFlag |= SVC_CALIB_CA_FLG_CALC_VIN_SEN;

            PRN_CAL_CA_LOG " " PRN_CAL_CA_API
            PRN_CAL_CA_LOG "------ Calibration CA Channel ------" PRN_CAL_CA_API
            PRN_CAL_CA_LOG "  VinID            : 0x%X"
                PRN_CAL_CA_ARG_UINT32 SVC_CalibCaCtrl.CalibChan.VinID PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_API
            PRN_CAL_CA_LOG "  SensorID         : 0x%X"
                PRN_CAL_CA_ARG_UINT32 SVC_CalibCaCtrl.CalibChan.SensorID PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_API
            PRN_CAL_CA_LOG "  VinSelectBits    : 0x%X"
                PRN_CAL_CA_ARG_UINT32 SVC_CalibCaCtrl.CalibChan.VinSelectBits PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_API
            PRN_CAL_CA_LOG "  SensorSelectBits : 0x%X"
                PRN_CAL_CA_ARG_UINT32 SVC_CalibCaCtrl.CalibChan.SensorSelectBits PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_API
        }
    }

    return RetVal;
}


static void SvcCalib_CaShellCfgVinSenU(void)
{
    PRN_CAL_CA_LOG "  %scfg_calib_chan%s        : reset all calibration setting as default"
        PRN_CAL_CA_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_CA_ARG_POST
    PRN_CAL_CA_API
    PRN_CAL_CA_LOG "  ------------------------------------------------------" PRN_CAL_CA_API
    PRN_CAL_CA_LOG "    [VinID]             : vin identify number." PRN_CAL_CA_API
    PRN_CAL_CA_LOG "    [SensorID]          : sensor identify number. 1/2/4/8" PRN_CAL_CA_API
    PRN_CAL_CA_LOG "    [VinSelectBits]     : bit0 : Vin0, bit1 : Vin1, ... " PRN_CAL_CA_API
    PRN_CAL_CA_LOG "    [SensorSelectBits]  : bit0 : sensor0, bit1 : sensor1, ..." PRN_CAL_CA_API
    PRN_CAL_CA_LOG " " PRN_CAL_CA_API
}


static UINT32 SvcCalib_CaShellCfgTblID(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibCaCtrlFlag & SVC_CALIB_CA_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to cfg ca table idx - initial ca module first!" PRN_CAL_CA_NG
    } else if (ArgCount < 3U) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to cfg ca table idx - Argc should >= 3" PRN_CAL_CA_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to cfg ca table idx - Argv should not null!" PRN_CAL_CA_NG
    } else {
        UINT32 TableIdx = 0U;

        SvcCalib_CaShellStrToU32(pArgVector[3U],  &TableIdx );

        SVC_CalibCaCtrl.CalcCaCtrl.TableIdx = TableIdx;

        PRN_CAL_CA_LOG " " PRN_CAL_CA_API
        PRN_CAL_CA_LOG "------ Calibration CA Table Index %d ------"
            PRN_CAL_CA_ARG_UINT32 SVC_CalibCaCtrl.CalcCaCtrl.TableIdx PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_API

    }

    return RetVal;
}


static void SvcCalib_CaShellCfgTblIDU(void)
{
    PRN_CAL_CA_LOG "  %scfg_table_id%s          :"
        PRN_CAL_CA_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_CA_ARG_POST
    PRN_CAL_CA_API
    PRN_CAL_CA_LOG "  ------------------------------------------------------" PRN_CAL_CA_API
    PRN_CAL_CA_LOG "    [Nand Table ID]     : the nand table id." PRN_CAL_CA_API
    PRN_CAL_CA_LOG "                        : the max value depend on each app" PRN_CAL_CA_API
    PRN_CAL_CA_LOG " " PRN_CAL_CA_API
}


static UINT32 SvcCalib_CaShellCfgDbgMsg(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibCaCtrlFlag & SVC_CALIB_CA_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to switch ca debug msg - initial ca module first!" PRN_CAL_CA_NG
    } else if (ArgCount < 3U) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to switch ca debug msg - Argc should >= 3" PRN_CAL_CA_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to switch ca debug msg - Argv should not null!" PRN_CAL_CA_NG
    } else {
        UINT32 DbgMsgOn = 0U;

        SvcCalib_CaShellStrToU32(pArgVector[3U],  &DbgMsgOn );

        if ((DbgMsgOn & 0x1U) > 0U) {
            SVC_CalibCaCtrl.EnableDebugMsg = DbgMsgOn;
        }

        if ((DbgMsgOn & 0x2U) > 0U) {
            SVC_CalibCaCtrlFlag |= SVC_LOG_CAL_DBG;
        }

        PRN_CAL_CA_LOG " " PRN_CAL_CA_API
        PRN_CAL_CA_LOG "------ Calibration CA Debug Msg On/Off %d, lvl %d ------"
            PRN_CAL_CA_ARG_UINT32 SVC_CalibCaCtrl.EnableDebugMsg PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_ARG_UINT32 DbgMsgOn                       PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_API

    }

    return RetVal;
}


static void SvcCalib_CaShellCfgDbgMsgU(void)
{
    PRN_CAL_CA_LOG "  %scfg_dbg_msg%s           :"
        PRN_CAL_CA_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_CA_ARG_POST
    PRN_CAL_CA_API
    PRN_CAL_CA_LOG "  ------------------------------------------------------" PRN_CAL_CA_API
    PRN_CAL_CA_LOG "    [Enable]            : 0: disable debug message" PRN_CAL_CA_API
    PRN_CAL_CA_LOG "                        : 1: enable debug message" PRN_CAL_CA_API
    PRN_CAL_CA_LOG "                        : 2: enable debug message level 2" PRN_CAL_CA_API
    PRN_CAL_CA_LOG " " PRN_CAL_CA_API
}


static UINT32 SvcCalib_CaShellGenTbl(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibCaCtrlFlag & SVC_CALIB_CA_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to proc ca 'gen_table' - initial ca module first!" PRN_CAL_CA_NG
    } else if (ArgCount < 3U) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to proc ca 'gen_table' - Argc should >= 3" PRN_CAL_CA_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to proc ca 'gen_table' - Argv should not null!" PRN_CAL_CA_NG
    } else {
        RetVal = SvcCalib_CaTableGenTbl(&(SVC_CalibCaCtrl.CalibChan), pArgVector[3U]);
    }

    return RetVal;
}


static void SvcCalib_CaShellGenTblU(void)
{
    PRN_CAL_CA_LOG "  %sgen_table%s             : start calculate CA table and save to NVM"
        PRN_CAL_CA_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_CA_ARG_POST
    PRN_CAL_CA_API
    PRN_CAL_CA_LOG "  ------------------------------------------------------" PRN_CAL_CA_API
    PRN_CAL_CA_LOG "    [Script]            : CA ctuner script path." PRN_CAL_CA_API
    PRN_CAL_CA_LOG " " PRN_CAL_CA_API
}


static UINT32 SvcCalib_CaShellEnable(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibCaCtrlFlag & SVC_CALIB_CA_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to enable/disable ca - initial ca module first!" PRN_CAL_CA_NG
    } else if (ArgCount <= 4U) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to enable/disable ca - Argc should > 4" PRN_CAL_CA_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to enable/disable ca - Argv should not null!" PRN_CAL_CA_NG
    } else {
        UINT32 IkContextID = 0xFFFFFFFFU, Enable = 0U;

        SvcCalib_CaShellStrToU32(pArgVector[3U],  &IkContextID );
        SvcCalib_CaShellStrToU32(pArgVector[4U],  &Enable      );
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
            PRetVal = AmbaIK_GetCawarpEnb(&ImgMode, &CurIkState);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_CAL_CA_LOG "Fail to enable/disable ca - get ca enable/disable state fail" PRN_CAL_CA_NG
            } else {
                if (CurIkState != Enable) {
                    PRetVal = AmbaIK_SetCawarpEnb(&ImgMode, Enable);
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;

                        PRN_CAL_CA_LOG "Fail to enable/disable ca - %s ca fail!"
                            PRN_CAL_CA_ARG_CSTR   ( (Enable > 0U) ? ( "enable" ) : ( "disable" ) ) PRN_CAL_CA_ARG_POST
                        PRN_CAL_CA_NG
                    } else {
                        PRN_CAL_CA_LOG "%s ca done!"
                            PRN_CAL_CA_ARG_CSTR   ( (Enable > 0U) ? ( "enable" ) : ( "disable" ) ) PRN_CAL_CA_ARG_POST
                        PRN_CAL_CA_API
                    }
                } else {
                    PRN_CAL_CA_LOG "current IK ca state: %s!"
                        PRN_CAL_CA_ARG_CSTR   ( (CurIkState > 0U) ? ( "enable" ) : ( "disable" ) ) PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_API
                }
            }
        } else {
            RetVal = SVC_NG;
            PRN_CAL_CA_LOG "Fail to enable/disable ca - invalid ik context id(%d)"
                PRN_CAL_CA_ARG_UINT32 IkContextID PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_NG
        }
    }

    return RetVal;
}


static void SvcCalib_CaShellEnableU(void)
{
    PRN_CAL_CA_LOG "  %senable%s                : enable/disable ca effect"
        PRN_CAL_CA_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_CA_ARG_POST
    PRN_CAL_CA_API
    PRN_CAL_CA_LOG "  ------------------------------------------------------" PRN_CAL_CA_API
    PRN_CAL_CA_LOG "    [IK Context ID]     : image kernel context id." PRN_CAL_CA_API
    PRN_CAL_CA_LOG "    [Enable]            : 0 => disable" PRN_CAL_CA_API
    PRN_CAL_CA_LOG "                        : 1 => enable" PRN_CAL_CA_API
    PRN_CAL_CA_LOG " " PRN_CAL_CA_API
}


static UINT32 SvcCalib_CaShellUpd(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibCaCtrlFlag & SVC_CALIB_CA_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to proc ca->upd cmd - initial ca module first!" PRN_CAL_CA_NG
    } else if (ArgCount <= 5U) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to proc ca->upd cmd - Argc should > 5" PRN_CAL_CA_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to proc ca->upd cmd - Argv should not null!" PRN_CAL_CA_NG
    } else {
        UINT32 VinID       = 0xFFFFFFFFU;
        UINT32 SensorID    = 0xFFFFFFFFU;
        UINT32 IkContextID = 0xFFFFFFFFU;

        SvcCalib_CaShellStrToU32(pArgVector[3U],  &VinID       );
        SvcCalib_CaShellStrToU32(pArgVector[4U],  &SensorID    );
        SvcCalib_CaShellStrToU32(pArgVector[5U],  &IkContextID );

        if (SVC_OK != SvcCalib_CaVinSensorIDCheck(VinID, SensorID)) {
            RetVal = SVC_NG;
            PRN_CAL_CA_LOG "Fail to proc ca->upd cmd - invalid VinID(%d), SensorID(%d)!"
                PRN_CAL_CA_ARG_UINT32 VinID    PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_ARG_UINT32 SensorID PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_NG
        } else if (IkContextID == 0xFFFFFFFFU) {
            RetVal = SVC_NG;
            PRN_CAL_CA_LOG "Fail to proc ca->upd cmd - invalid IKContextID(%d)!"
                PRN_CAL_CA_ARG_UINT32 IkContextID PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_NG
        } else {
            UINT32 CurIkEnable = 0U;
            AMBA_IK_MODE_CFG_s ImgMode;

            AmbaSvcWrap_MisraMemset(&ImgMode, 0, sizeof(ImgMode));
            ImgMode.ContextId = IkContextID;
            PRetVal = AmbaIK_GetCawarpEnb(&ImgMode, &CurIkEnable);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_CAL_CA_LOG "Fail to proc ca->upd cmd - get ca enable/disable state fail! ErrCode: 0x%x"
                    PRN_CAL_CA_ARG_UINT32 PRetVal PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_NG
            } else {

                // If the ca has been enable, disable it before update ca table
                if (CurIkEnable > 0U) {
                    PRetVal = AmbaIK_SetCawarpEnb(&ImgMode, 0U);
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;
                        PRN_CAL_CA_LOG "Fail to proc ca->upd cmd - disable ca failure! ErrCode: 0x%x"
                            PRN_CAL_CA_ARG_UINT32 PRetVal PRN_CAL_CA_ARG_POST
                        PRN_CAL_CA_NG
                    }
                }

                if (RetVal == SVC_OK) {
                    UINT32 WorkTblIdx = 0xFFFFFFFFU;
                    SVC_CALIB_CHANNEL_s CalibChan;

                    // If working buffer table exist, reset it first
                    PRetVal = SvcCalib_CaTableSearch(VinID, SensorID, &WorkTblIdx); PRN_CAL_CA_ERR_HDLR
                    if (WorkTblIdx < SVC_CalibCaCtrl.NumOfCaTable) {
                        AmbaSvcWrap_MisraMemset(&(SVC_CalibCaCtrl.pCaTable[WorkTblIdx]), 0, sizeof(SVC_CALIB_CA_TABLE_s));
                    }

                    // Update the calib table from nand
                    AmbaSvcWrap_MisraMemset(&CalibChan, 0, sizeof(CalibChan));
                    CalibChan.VinID    = VinID;
                    CalibChan.SensorID = SensorID;
                    RetVal = SvcCalib_CaTableUpdate(&CalibChan, &ImgMode);
                    if (RetVal != 0U) {
                        PRN_CAL_CA_LOG "Fail to proc ca->upd cmd - update calib table fail! VinID(%d), SensorID(%d), IkContextID(%d)"
                            PRN_CAL_CA_ARG_UINT32 VinID       PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_ARG_UINT32 SensorID    PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_ARG_UINT32 IkContextID PRN_CAL_CA_ARG_POST
                        PRN_CAL_CA_NG
                    }
                }
            }
        }
    }

    return RetVal;
}


static void SvcCalib_CaShellUpdU(void)
{
    PRN_CAL_CA_LOG "  %supd%s                   : update CA table"
        PRN_CAL_CA_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_CA_ARG_POST
    PRN_CAL_CA_API
    PRN_CAL_CA_LOG "  ------------------------------------------------------" PRN_CAL_CA_API
    PRN_CAL_CA_LOG "    [VinID]             : vin identify number." PRN_CAL_CA_API
    PRN_CAL_CA_LOG "    [SensorID]          : sensor identify number. 1/2/4/8" PRN_CAL_CA_API
    PRN_CAL_CA_LOG "    [IK Context ID]     : image kernel context id." PRN_CAL_CA_API
    PRN_CAL_CA_LOG " " PRN_CAL_CA_API
}


static UINT32 SvcCalib_CaShellSet(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibCaCtrlFlag & SVC_CALIB_CA_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to proc ca->upd cmd - initial ca module first!" PRN_CAL_CA_NG
    } else if ((SVC_CalibCaCtrlFlag & SVC_CALIB_CA_FLG_SHADOW_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to proc ca->set cmd - initial ca shadow first!" PRN_CAL_CA_NG
    } else if (SVC_CalibCaCtrl.pNandTable == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to proc ca->set cmd - invalid ca nand table!" PRN_CAL_CA_NG
    } else if (ArgCount <= 5U) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to proc ca->set cmd - Argc should > 5" PRN_CAL_CA_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to proc ca->set cmd - Argv should not null!" PRN_CAL_CA_NG
    } else {
        UINT32 TblID = 0xFFU, ArgIdx = 3U;

        SvcCalib_CaShellStrToU32(pArgVector[ArgIdx],  &TblID); ArgIdx ++;
        if (TblID >= SVC_CalibCaCtrl.NumOfCaTable) {
            RetVal = SVC_NG;
            PRN_CAL_CA_LOG "Fail to proc ca->set cmd - insert table id(%d) is out-of range(%d)!"
                PRN_CAL_CA_ARG_UINT32 TblID                       PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_ARG_UINT32 SVC_CalibCaCtrl.NumOfCaTable PRN_CAL_CA_ARG_POST
            PRN_CAL_CA_NG
        } else {
            SVC_CALIB_CA_NAND_TABLE_s CurCaTbl;
            void  *pCaRedBuf = CurCaTbl.CaRVector;
            void  *pCaBlueBuf = CurCaTbl.CaBVector;
            UINT32 CaBufSize;
            const char *pRedFilePath = NULL, *pBlueFilePath = NULL;
            SVC_CALIB_CA_NAND_TABLE_s *pNandTable = &(SVC_CalibCaCtrl.pNandTable[TblID]);
            AMBA_FS_FILE_INFO_s FileInfo;
            AMBA_FS_FILE *pFile = NULL;
            UINT32 NumSuccess = 0U;
            UINT32 DbgMsgOn = 1U;

            AmbaSvcWrap_MisraMemset(&CurCaTbl, 0, sizeof(SVC_CALIB_CA_NAND_TABLE_s));
            CurCaTbl.Header.Version      = SVC_CALIB_CA_VERSION;
            CurCaTbl.Header.TwoStageDone = 0U;

            SvcCalib_CaShellStrToU32(pArgVector[ArgIdx], &(CurCaTbl.Header.VinSelectBits   )); ArgIdx++;
            SvcCalib_CaShellStrToU32(pArgVector[ArgIdx], &(CurCaTbl.Header.SensorSelectBits)); ArgIdx++;

            SvcCalib_CaShellStrToU32(pArgVector[ArgIdx], &(CurCaTbl.Header.VinSensorGeo.StartX              )); ArgIdx++;
            SvcCalib_CaShellStrToU32(pArgVector[ArgIdx], &(CurCaTbl.Header.VinSensorGeo.StartY              )); ArgIdx++;
            SvcCalib_CaShellStrToU32(pArgVector[ArgIdx], &(CurCaTbl.Header.VinSensorGeo.Width               )); ArgIdx++;
            SvcCalib_CaShellStrToU32(pArgVector[ArgIdx], &(CurCaTbl.Header.VinSensorGeo.Height              )); ArgIdx++;
            SvcCalib_CaShellStrToU32(pArgVector[ArgIdx], &(CurCaTbl.Header.VinSensorGeo.HSubSample.FactorDen)); ArgIdx++;
            SvcCalib_CaShellStrToU32(pArgVector[ArgIdx], &(CurCaTbl.Header.VinSensorGeo.HSubSample.FactorNum)); ArgIdx++;
            SvcCalib_CaShellStrToU32(pArgVector[ArgIdx], &(CurCaTbl.Header.VinSensorGeo.VSubSample.FactorDen)); ArgIdx++;
            SvcCalib_CaShellStrToU32(pArgVector[ArgIdx], &(CurCaTbl.Header.VinSensorGeo.VSubSample.FactorNum)); ArgIdx++;

            SvcCalib_CaShellStrToU32(pArgVector[ArgIdx], &(CurCaTbl.Header.HorGridNum   )); ArgIdx++;
            SvcCalib_CaShellStrToU32(pArgVector[ArgIdx], &(CurCaTbl.Header.VerGridNum   )); ArgIdx++;
            SvcCalib_CaShellStrToU32(pArgVector[ArgIdx], &(CurCaTbl.Header.TileWidthExp )); ArgIdx++;
            SvcCalib_CaShellStrToU32(pArgVector[ArgIdx], &(CurCaTbl.Header.TileHeightExp)); ArgIdx++;

            pRedFilePath = pArgVector[ArgIdx]; ArgIdx++;
            pBlueFilePath = pArgVector[ArgIdx]; ArgIdx++;
            SvcCalib_CaShellStrToU32(pArgVector[ArgIdx], &DbgMsgOn);

            CaBufSize = ((UINT32)sizeof(AMBA_IK_GRID_POINT_s)) * ( CurCaTbl.Header.HorGridNum * CurCaTbl.Header.VerGridNum );

            AmbaSvcWrap_MisraMemset(&FileInfo, 0, sizeof(FileInfo));
            PRetVal = AmbaFS_GetFileInfo(pRedFilePath, &FileInfo);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_CAL_CA_LOG "Fail to proc ca->set cmd - get red file info fail! ErrCode(0x%08X) '%s'"
                    PRN_CAL_CA_ARG_UINT32 PRetVal      PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_ARG_CSTR   pRedFilePath PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_NG
            } else if ((UINT32)(FileInfo.Size) != CaBufSize) {
                RetVal = SVC_NG;
                PRN_CAL_CA_LOG "Fail to proc ca->set cmd - file size(0x%08X) does not match Hor(%d) x Ver(%d) x Grid(%d)!"
                    PRN_CAL_CA_ARG_UINT32 FileInfo.Size PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_ARG_UINT32 CurCaTbl.Header.HorGridNum  PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_ARG_UINT32 CurCaTbl.Header.VerGridNum  PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_ARG_UINT32 sizeof(AMBA_IK_GRID_POINT_s) PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_NG
            } else {
                PRetVal = AmbaFS_FileOpen(pRedFilePath, "rb", &pFile);
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    PRN_CAL_CA_LOG "Fail to proc ca->set cmd - open red file fail! ErrCode(0x%08X)"
                        PRN_CAL_CA_ARG_UINT32 PRetVal PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_NG
                } else if (pFile == NULL) {
                    RetVal = SVC_NG;
                    PRN_CAL_CA_LOG "Fail to proc ca->set cmd - invalid red file pointer" PRN_CAL_CA_NG
                } else {
                    PRetVal = AmbaFS_FileRead(pCaRedBuf, 1, CaBufSize, pFile, &NumSuccess);
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;
                        PRN_CAL_CA_LOG "Fail to proc ca->set cmd - read data fail! ErrCode(0x%08X)"
                            PRN_CAL_CA_ARG_UINT32 PRetVal PRN_CAL_CA_ARG_POST
                        PRN_CAL_CA_NG
                    } else if (NumSuccess != CaBufSize) {
                        RetVal = SVC_NG;
                        PRN_CAL_CA_LOG "Fail to proc ca->set cmd - read size(0x%X) does not same with request size(0x%X) fail!"
                            PRN_CAL_CA_ARG_UINT32 NumSuccess PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_ARG_UINT32 CaBufSize PRN_CAL_CA_ARG_POST
                        PRN_CAL_CA_NG
                    } else {
                        // misra-c
                    }

                    PRetVal = AmbaFS_FileClose(pFile); PRN_CAL_CA_ERR_HDLR
                    pFile = NULL;
                }
            }

            AmbaSvcWrap_MisraMemset(&FileInfo, 0, sizeof(FileInfo));
            PRetVal = AmbaFS_GetFileInfo(pBlueFilePath, &FileInfo);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_CAL_CA_LOG "Fail to proc ca->set cmd - get blue file info fail! ErrCode(0x%08X) '%s'"
                    PRN_CAL_CA_ARG_UINT32 PRetVal      PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_ARG_CSTR   pBlueFilePath PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_NG
            } else if ((UINT32)(FileInfo.Size) != CaBufSize) {
                RetVal = SVC_NG;
                PRN_CAL_CA_LOG "Fail to proc ca->set cmd - file size(0x%08X) does not match Hor(%d) x Ver(%d) x Grid(%d)!"
                    PRN_CAL_CA_ARG_UINT32 FileInfo.Size PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_ARG_UINT32 CurCaTbl.Header.HorGridNum  PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_ARG_UINT32 CurCaTbl.Header.VerGridNum  PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_ARG_UINT32 sizeof(AMBA_IK_GRID_POINT_s) PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_NG
            } else {
                PRetVal = AmbaFS_FileOpen(pBlueFilePath, "rb", &pFile);
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    PRN_CAL_CA_LOG "Fail to proc ca->set cmd - open blue file fail! ErrCode(0x%08X)"
                        PRN_CAL_CA_ARG_UINT32 PRetVal PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_NG
                } else if (pFile == NULL) {
                    RetVal = SVC_NG;
                    PRN_CAL_CA_LOG "Fail to proc ca->set cmd - invalid blue file pointer" PRN_CAL_CA_NG
                } else {
                    NumSuccess = 0U;
                    PRetVal = AmbaFS_FileRead(pCaBlueBuf, 1, CaBufSize, pFile, &NumSuccess);
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;
                        PRN_CAL_CA_LOG "Fail to proc ca->set cmd - read data fail! ErrCode(0x%08X)"
                            PRN_CAL_CA_ARG_UINT32 PRetVal PRN_CAL_CA_ARG_POST
                        PRN_CAL_CA_NG
                    } else if (NumSuccess != CaBufSize) {
                        RetVal = SVC_NG;
                        PRN_CAL_CA_LOG "Fail to proc ca->set cmd - read size(0x%X) does not same with request size(0x%X) fail!"
                            PRN_CAL_CA_ARG_UINT32 NumSuccess PRN_CAL_CA_ARG_POST
                            PRN_CAL_CA_ARG_UINT32 CaBufSize PRN_CAL_CA_ARG_POST
                        PRN_CAL_CA_NG
                    } else {
                        // misra-c
                    }

                    PRetVal = AmbaFS_FileClose(pFile); PRN_CAL_CA_ERR_HDLR
                }
            }

            if (RetVal == 0U) {
                if (KAL_ERR_NONE != SvcCalib_CaMutexTake(&(SVC_CalibCaCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
                    RetVal = SVC_NG;
                    PRN_CAL_CA_LOG "Fail to proc ca->set cmd - take mutex fail!" PRN_CAL_CA_NG
                } else {

                    AmbaSvcWrap_MisraMemcpy(pNandTable, &CurCaTbl, sizeof(SVC_CALIB_CA_NAND_TABLE_s));

                    SVC_CalibCaCtrl.pNandHeader->Enable = 1U;
                    SVC_CalibCaCtrl.pNandHeader->DebugMsgOn = DbgMsgOn;
                    if ((SVC_CalibCaCtrl.pNandHeader->TableSelectBits & SvcCalib_BitGet(TblID)) == 0U) {
                        SVC_CalibCaCtrl.pNandHeader->TableSelectBits |= SvcCalib_BitGet(TblID);
                        SVC_CalibCaCtrl.pNandHeader->NumOfTable ++;
                    }

                    PRN_CAL_CA_LOG " " PRN_CAL_CA_API
                    PRN_CAL_CA_LOG "====== Calibration ca info ======" PRN_CAL_CA_API
                    PRN_CAL_CA_LOG "  Version                             : 0x%X"
                        PRN_CAL_CA_ARG_UINT32 pNandTable->Header.Version PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_API
                    PRN_CAL_CA_LOG "  HorGridNum                          : %d"
                        PRN_CAL_CA_ARG_UINT32   pNandTable->Header.HorGridNum PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_API
                    PRN_CAL_CA_LOG "  VerGridNum                          : %d"
                        PRN_CAL_CA_ARG_UINT32   pNandTable->Header.VerGridNum PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_API
                    PRN_CAL_CA_LOG "  TileWidthExp                        : %d"
                        PRN_CAL_CA_ARG_UINT32   pNandTable->Header.TileWidthExp PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_API
                    PRN_CAL_CA_LOG "  TileHeightExp                       : %d"
                        PRN_CAL_CA_ARG_UINT32   pNandTable->Header.TileHeightExp PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_API
                    PRN_CAL_CA_LOG "  CalibSensorGeo.StartX               : %d"
                        PRN_CAL_CA_ARG_UINT32   pNandTable->Header.VinSensorGeo.StartX PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_API
                    PRN_CAL_CA_LOG "  CalibSensorGeo.StartY               : %d"
                        PRN_CAL_CA_ARG_UINT32   pNandTable->Header.VinSensorGeo.StartY PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_API
                    PRN_CAL_CA_LOG "  CalibSensorGeo.Width                : %d"
                        PRN_CAL_CA_ARG_UINT32   pNandTable->Header.VinSensorGeo.Width PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_API
                    PRN_CAL_CA_LOG "  CalibSensorGeo.Height               : %d"
                        PRN_CAL_CA_ARG_UINT32   pNandTable->Header.VinSensorGeo.Height PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_API
                    PRN_CAL_CA_LOG "  CalibSensorGeo.HSubSample.FactorDen : %d"
                        PRN_CAL_CA_ARG_UINT32   pNandTable->Header.VinSensorGeo.HSubSample.FactorDen PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_API
                    PRN_CAL_CA_LOG "  CalibSensorGeo.HSubSample.FactorNum : %d"
                        PRN_CAL_CA_ARG_UINT32   pNandTable->Header.VinSensorGeo.HSubSample.FactorNum PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_API
                    PRN_CAL_CA_LOG "  CalibSensorGeo.VSubSample.FactorDen : %d"
                        PRN_CAL_CA_ARG_UINT32   pNandTable->Header.VinSensorGeo.VSubSample.FactorDen PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_API
                    PRN_CAL_CA_LOG "  CalibSensorGeo.VSubSample.FactorNum : %d"
                        PRN_CAL_CA_ARG_UINT32   pNandTable->Header.VinSensorGeo.VSubSample.FactorNum PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_API
                    PRN_CAL_CA_LOG "  TwoStageDone                        : %d"
                        PRN_CAL_CA_ARG_UINT32   pNandTable->Header.TwoStageDone PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_API
                    PRN_CAL_CA_LOG "  VinSelectBits                       : 0x%x"
                        PRN_CAL_CA_ARG_UINT32 pNandTable->Header.VinSelectBits PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_API
                    PRN_CAL_CA_LOG "  SensorSelectBits                    : 0x%x"
                        PRN_CAL_CA_ARG_UINT32 pNandTable->Header.SensorSelectBits PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_API
                    PRN_CAL_CA_LOG "  CaRVector                           : %p"
                        PRN_CAL_CA_ARG_CPOINT  pNandTable->CaRVector PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_API
                    PRN_CAL_CA_LOG "  CaBVector                           : %p"
                        PRN_CAL_CA_ARG_CPOINT  pNandTable->CaBVector PRN_CAL_CA_ARG_POST
                    PRN_CAL_CA_API

                    /* Update to NAND */
                    RetVal = SvcCalib_DataSave(SVC_CALIB_CA_ID);
                    if (RetVal != OK) {
                        PRN_CAL_CA_LOG "Fail to save ca to nand!" PRN_CAL_CA_NG
                    }

                    SvcCalib_CaMutexGive(&(SVC_CalibCaCtrl.Mutex));
                }
            }
        }
    }

    return RetVal;
}

static void SvcCalib_CaShellSetU(void)
{
    PRN_CAL_CA_LOG "  %sset%s                   : set ca table manually"
        PRN_CAL_CA_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_CA_ARG_POST
    PRN_CAL_CA_API
    PRN_CAL_CA_LOG "  ------------------------------------------------------" PRN_CAL_CA_API
    PRN_CAL_CA_LOG "    [TblID]                 : inser table index" PRN_CAL_CA_API
    PRN_CAL_CA_LOG "    [VinSelectBits]         : the table supported vin select bits"        PRN_CAL_CA_API
    PRN_CAL_CA_LOG "    [SensorSelectBits]      : the table supported sensor select bits"     PRN_CAL_CA_API
    PRN_CAL_CA_LOG "    [Calib StartX]          : calibration geometric StartX              " PRN_CAL_CA_API
    PRN_CAL_CA_LOG "    [Calib StartY]          : calibration geometric StartY              " PRN_CAL_CA_API
    PRN_CAL_CA_LOG "    [Calib Width]           : calibration geometric Width               " PRN_CAL_CA_API
    PRN_CAL_CA_LOG "    [Calib Height]          : calibration geometric Height              " PRN_CAL_CA_API
    PRN_CAL_CA_LOG "    [Calib HbSampleDen]     : calibration geometric HSubSample.FactorDen" PRN_CAL_CA_API
    PRN_CAL_CA_LOG "    [Calib HbSampleNum]     : calibration geometric HSubSample.FactorNum" PRN_CAL_CA_API
    PRN_CAL_CA_LOG "    [Calib VbSampleDen]     : calibration geometric VSubSample.FactorDen" PRN_CAL_CA_API
    PRN_CAL_CA_LOG "    [Calib VbSampleNum]     : calibration geometric VSubSample.FactorNum" PRN_CAL_CA_API
    PRN_CAL_CA_LOG "    [Ca HorGridNum]         : ca horizontal grid number" PRN_CAL_CA_API
    PRN_CAL_CA_LOG "    [Ca VerGridNum]         : ca vertical grid number"   PRN_CAL_CA_API
    PRN_CAL_CA_LOG "    [Ca TileWidthExp]       : ca tile width exponent"    PRN_CAL_CA_API
    PRN_CAL_CA_LOG "    [Ca TileHeightExp]      : ca tile height exponent"   PRN_CAL_CA_API
    PRN_CAL_CA_LOG "    [Ca Red File Path]      : ca red file path"          PRN_CAL_CA_API
    PRN_CAL_CA_LOG "    [Ca Blue File Path]     : ca blue file path"         PRN_CAL_CA_API
    PRN_CAL_CA_LOG "    [DebugMsgOn]            : configure debug msg"       PRN_CAL_CA_API

    PRN_CAL_CA_LOG " " PRN_CAL_CA_API

}


static void SvcCalib_CaShellEntryInit(void)
{
    if ((SVC_CalibCaCtrlFlag & SVC_CALIB_CA_FLG_SHELL_INIT) == 0U) {
        UINT32 ShellNum = 0U;

        AmbaSvcWrap_MisraMemset(SVC_CalibCaShellFunc, 0, sizeof(SVC_CalibCaShellFunc));

        SVC_CalibCaShellFunc[ShellNum] = (SVC_CALIB_CA_SHELL_FUNC_s) { 1U, "cfg_calib_chan",   SvcCalib_CaShellCfgVinSen,    SvcCalib_CaShellCfgVinSenU    }; ShellNum ++;
        SVC_CalibCaShellFunc[ShellNum] = (SVC_CALIB_CA_SHELL_FUNC_s) { 1U, "cfg_table_id",     SvcCalib_CaShellCfgTblID,     SvcCalib_CaShellCfgTblIDU     }; ShellNum ++;
        SVC_CalibCaShellFunc[ShellNum] = (SVC_CALIB_CA_SHELL_FUNC_s) { 1U, "cfg_dbg_msg",      SvcCalib_CaShellCfgDbgMsg,    SvcCalib_CaShellCfgDbgMsgU    }; ShellNum ++;
        SVC_CalibCaShellFunc[ShellNum] = (SVC_CALIB_CA_SHELL_FUNC_s) { 1U, "gen_table",        SvcCalib_CaShellGenTbl,       SvcCalib_CaShellGenTblU       }; ShellNum ++;
        SVC_CalibCaShellFunc[ShellNum] = (SVC_CALIB_CA_SHELL_FUNC_s) { 1U, "enable",           SvcCalib_CaShellEnable,       SvcCalib_CaShellEnableU       }; ShellNum ++;
        SVC_CalibCaShellFunc[ShellNum] = (SVC_CALIB_CA_SHELL_FUNC_s) { 1U, "upd",              SvcCalib_CaShellUpd,          SvcCalib_CaShellUpdU          }; ShellNum ++;
        SVC_CalibCaShellFunc[ShellNum] = (SVC_CALIB_CA_SHELL_FUNC_s) { 1U, "set",              SvcCalib_CaShellSet,          SvcCalib_CaShellSetU          };

        SVC_CalibCaCtrlFlag |= SVC_CALIB_CA_FLG_SHELL_INIT;
    }
}


UINT32 SvcCalib_CaShellFunc(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_NG, PRetVal;

    SvcCalib_CaShellEntryInit();

    if (ArgCount > 0U) {
        UINT32 ShellIdx;
        UINT32 ShellCount = (UINT32)(sizeof(SVC_CalibCaShellFunc)) / (UINT32)(sizeof(SVC_CalibCaShellFunc[0]));

        for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
            if ((SVC_CalibCaShellFunc[ShellIdx].pFunc != NULL) && (SVC_CalibCaShellFunc[ShellIdx].Enable > 0U)) {
                if (0 == SvcWrap_strcmp(pArgVector[2U], SVC_CalibCaShellFunc[ShellIdx].ShellCmdName)) {
                    PRetVal = (SVC_CalibCaShellFunc[ShellIdx].pFunc)(ArgCount, pArgVector); PRN_CAL_CA_ERR_HDLR

                    RetVal = SVC_OK;
                    break;
                }
            }
        }

        if (RetVal != SVC_OK) {
            SvcCalib_CaShellUsage();
        }
    }

    return RetVal;
}


static void SvcCalib_CaShellUsage(void)
{
    UINT32 ShellIdx;
    UINT32 ShellCount = (UINT32)(sizeof(SVC_CalibCaShellFunc)) / (UINT32)(sizeof(SVC_CalibCaShellFunc[0]));

    PRN_CAL_CA_LOG " " PRN_CAL_CA_API
    PRN_CAL_CA_LOG "====== CA Command Usage ======" PRN_CAL_CA_API

    for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
        if ((SVC_CalibCaShellFunc[ShellIdx].pFunc != NULL) && (SVC_CalibCaShellFunc[ShellIdx].Enable > 0U)) {
            if (SVC_CalibCaShellFunc[ShellIdx].pUsageFunc == NULL) {
                PRN_CAL_CA_LOG "  %s"
                    PRN_CAL_CA_ARG_CSTR   SVC_CalibCaShellFunc[ShellIdx].ShellCmdName PRN_CAL_CA_ARG_POST
                PRN_CAL_CA_API
            } else {
                (SVC_CalibCaShellFunc[ShellIdx].pUsageFunc)();
            }
        }
    }
}


static void SvcCalib_CaCmdHdlrInit(void)
{
    if ((SVC_CalibCaCtrlFlag & SVC_CALIB_CA_FLG_CMD_INIT) == 0U) {

        AmbaSvcWrap_MisraMemset(SVC_CalibCaCmdHdlr, 0, sizeof(SVC_CalibCaCmdHdlr));

        SVC_CalibCaCmdHdlr[SVC_CALIB_CMD_MEM_QUERY      ] = (SVC_CALIB_CA_CMD_HDLR_s) {1, SVC_CALIB_CMD_MEM_QUERY,       SvcCalib_CaCmdMemQuery   };
        SVC_CalibCaCmdHdlr[SVC_CALIB_CMD_ITEM_CREATE    ] = (SVC_CALIB_CA_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_CREATE,     SvcCalib_CaCmdCreate     };
        SVC_CalibCaCmdHdlr[SVC_CALIB_CMD_ITEM_DATA_INIT ] = (SVC_CALIB_CA_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_DATA_INIT,  SvcCalib_CaCmdDataInit   };
        SVC_CalibCaCmdHdlr[SVC_CALIB_CMD_ITEM_TBL_UPDATE] = (SVC_CALIB_CA_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_TBL_UPDATE, SvcCalib_CaCmdTblUpdate  };
        SVC_CalibCaCmdHdlr[SVC_CALIB_CMD_ITEM_TBL_SET   ] = (SVC_CALIB_CA_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_TBL_SET,    SvcCalib_CaCmdTblDataSet };
        SVC_CalibCaCmdHdlr[SVC_CALIB_CMD_ITEM_TBL_GET   ] = (SVC_CALIB_CA_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_TBL_GET,    SvcCalib_CaCmdTblDataGet };

        SVC_CalibCaCtrlFlag |= SVC_CALIB_CA_FLG_CMD_INIT;
    }
}

static UINT32 SvcCalib_CaCmdMemQuery(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    if (pParam1 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to query ca memory - input ca table number should not zero" PRN_CAL_CA_NG
    } else if (pParam2 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to query ca memory - output ca shadow buffer size should not null!" PRN_CAL_CA_NG
    } else if (pParam3 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to query ca memory - output ca working buffer size should not null!" PRN_CAL_CA_NG
    } else if (pParam4 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to query ca memory - ca calculation buffer size should not null!" PRN_CAL_CA_NG
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

        RetVal = SvcCalib_CaMemQuery(NomOfTable, pShadowBufSize, pWorkingBufSize, pCalcMemSize);
        if (RetVal != SVC_OK) {
            PRN_CAL_CA_LOG "Fail to query ca memory - query memory fail!" PRN_CAL_CA_NG
        }

        AmbaMisra_TouchUnused(pParam1);
        AmbaMisra_TouchUnused(pParam2);
        AmbaMisra_TouchUnused(pParam3);
        AmbaMisra_TouchUnused(pParam4);
        AmbaMisra_TouchUnused(pU32Val);
    }

    return RetVal;
}


static UINT32 SvcCalib_CaCmdCreate(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SvcCalib_CaCreate();
    if (RetVal != SVC_OK) {
        PRN_CAL_CA_LOG "Fail to create bpc module fail!" PRN_CAL_CA_NG
    } else {
        AmbaMisra_TouchUnused(pParam1);
        AmbaMisra_TouchUnused(pParam2);
        AmbaMisra_TouchUnused(pParam3);
        AmbaMisra_TouchUnused(pParam4);
    }

    return RetVal;
}


static UINT32 SvcCalib_CaCmdDataInit(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    if (pParam1 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to init ca - input calib object should not null!" PRN_CAL_CA_NG
    } else {
        const SVC_CALIB_OBJ_s *pCalObj;

        AmbaMisra_TypeCast(&pCalObj, &pParam1);
        if (pCalObj == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_CA_LOG "Fail to init ca - invalid calib obj!" PRN_CAL_CA_NG
        } else {
            RetVal = SvcCalib_CaShadowInit(pCalObj);
            if (RetVal != SVC_OK) {
                PRN_CAL_CA_LOG "Fail to init ca - shadow initial fail!" PRN_CAL_CA_NG
            } else {
                SVC_CALIB_CALC_CB_f CbFunc = NULL;
                AmbaMisra_TypeCast(&(CbFunc), &(pParam2));
                SVC_CalibCaCtrl.CalcCbFunc = CbFunc;
            }
        }

        AmbaMisra_TouchUnused(pParam1);
        AmbaMisra_TouchUnused(pParam2);
        AmbaMisra_TouchUnused(pParam3);
        AmbaMisra_TouchUnused(pParam4);
    }

    return RetVal;
}


static UINT32 SvcCalib_CaCmdTblUpdate(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    if (pParam1 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to update ca table - input calib channel should not null!" PRN_CAL_CA_NG
    } else if (pParam2 == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to update ca table - input IK mode cfg should not null!" PRN_CAL_CA_NG
    } else {
        const SVC_CALIB_CHANNEL_s     *pChan;
        const AMBA_IK_MODE_CFG_s      *pImgMode;

        AmbaMisra_TypeCast(&(pChan), &pParam1);
        AmbaMisra_TypeCast(&(pImgMode), &pParam2);

        RetVal = SvcCalib_CaTableUpdate(pChan, pImgMode);
    }

    AmbaMisra_TouchUnused(pParam1);
    AmbaMisra_TouchUnused(pParam2);
    AmbaMisra_TouchUnused(pParam3);
    AmbaMisra_TouchUnused(pParam4);

    return RetVal;
}


static UINT32 SvcCalib_CaCmdTblDataSet(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;
    UINT32 *pU32Val = NULL;
    SVC_CALIB_TBL_INFO_s *pTblInfo;

    AmbaMisra_TypeCast(&(pU32Val), &(pParam1));
    AmbaMisra_TypeCast(&(pTblInfo), &(pParam2));

    if (pU32Val == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to set table data - input table id should not null!" PRN_CAL_CA_NG
    } else {
        RetVal = SvcCalib_CaTableDataSet(*pU32Val, pTblInfo);
    }

    AmbaMisra_TouchUnused(pParam1);
    AmbaMisra_TouchUnused(pParam2);
    AmbaMisra_TouchUnused(pParam3);
    AmbaMisra_TouchUnused(pParam4);
    AmbaMisra_TouchUnused(pU32Val);

    return RetVal;
}


static UINT32 SvcCalib_CaCmdTblDataGet(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;
    UINT32 *pU32Val = NULL;
    SVC_CALIB_TBL_INFO_s *pTblInfo;

    AmbaMisra_TypeCast(&(pU32Val), &(pParam1));
    AmbaMisra_TypeCast(&(pTblInfo), &(pParam2));

    if (pU32Val == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to get table data - input table id should not null!" PRN_CAL_CA_NG
    } else {
        RetVal = SvcCalib_CaTableDataGet(*pU32Val, pTblInfo);
    }

    AmbaMisra_TouchUnused(pParam1);
    AmbaMisra_TouchUnused(pParam2);
    AmbaMisra_TouchUnused(pParam3);
    AmbaMisra_TouchUnused(pParam4);
    AmbaMisra_TouchUnused(pU32Val);

    return RetVal;
}

/**
 * calib ca command entry
 *
 * @param [in] CmdID  the bpc command id
 * @param [in] Param1 no.01 parameter
 * @param [in] Param2 no.02 parameter
 * @param [in] Param3 no.03 parameter
 * @param [in] Param4 no.04 parameter
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcCalib_CaCmdFunc(UINT32 CmdID, void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    SvcCalib_CaCmdHdlrInit();

    if (CmdID >= SVC_CALIB_CMD_NUM) {
        RetVal = SVC_NG;
        PRN_CAL_CA_LOG "Fail to handler cmd - invalid command id(%d)"
            PRN_CAL_CA_ARG_UINT32 CmdID PRN_CAL_CA_ARG_POST
        PRN_CAL_CA_NG
    } else {
        const UINT32 HdlrCount = (UINT32)(sizeof(SVC_CalibCaCmdHdlr)) / ((UINT32)sizeof(SVC_CalibCaCmdHdlr[0]));
        UINT32 HdlrIdx;

        for (HdlrIdx = 0U; HdlrIdx < HdlrCount; HdlrIdx ++) {
            if ((SVC_CalibCaCmdHdlr[HdlrIdx].Enable > 0U) &&
                (SVC_CalibCaCmdHdlr[HdlrIdx].CmdID == CmdID) &&
                (SVC_CalibCaCmdHdlr[HdlrIdx].pHandler != NULL)) {
                RetVal = (SVC_CalibCaCmdHdlr[HdlrIdx].pHandler)(pParam1, pParam2, pParam3, pParam4);
            }
        }
    }

    return RetVal;
}

