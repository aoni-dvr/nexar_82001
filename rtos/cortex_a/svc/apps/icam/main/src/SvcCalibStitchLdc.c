/**
 *  @file SvcCalibStitchLdc.c
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
 *  @details svc calibration stitch warp
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaVIN_Def.h"
#include "AmbaDSP_ImageUtility.h"

#include "AmbaCalib_ErrNo.h"
#include "AmbaCalib_LDCIF.h"
#include "AmbaCT_TextHdlr.h"
#include "AmbaCT_LdcTunerIF.h"

#include "SvcWrap.h"
#include "SvcCalibMgr.h"

#include "SvcErrCode.h"
#include "SvcLog.h"

#include "SvcCalibStitch.h"

#define SVC_CALIB_ST_LDC_FLG_INIT      (0x1U)
#define SVC_CALIB_ST_LDC_FLG_SHDW_INIT (0x2U)
// #define SVC_CALIB_ST_LDC_FLG_CALC_INIT (0x4U)
// #define SVC_CALIB_ST_LDC_FLG_CALC_CHN  (0x8U)
#define SVC_CALIB_ST_LDC_FLG_DBG_MSG   (0x100U)
#define SVC_CALIB_ST_LDC_FLG_SHELLINIT (0x1000U)
#define SVC_CALIB_ST_LDC_FLG_CMD_INIT  (0x2000U)

#define SVC_CALIB_ST_LDC_NAME          ("SvcCalib_StLdc")
#define SVC_CALIB_ST_LDC_NAND_HDR_SIZE (0x200U)
#define SVC_CALIB_ST_LDC_VERSION       (0x20180401U)
#define SVC_CALIB_ST_LDC_CALIB_2_STAGE (0x0U)
#define SVC_CALIB_ST_LDC_IK_2_STAGE    (0x1U)
#define SVC_CALIB_ST_LDC_MAX_CURVE_LEN ( MAX_LENS_DISTO_TBL_LEN )
#define SVC_CALIB_ST_LDC_MAX_TBL_LEN   ( MAX_WARP_TBL_LEN )

#define SVC_CALIB_ST_LDC_MEM_CALC_REMAP (0U)
#define SVC_CALIB_ST_LDC_MEM_NUM        (1U)

#define SVC_CALIB_ST_LDC_REMAP_BUF_SIZE     ( 393984U )

typedef struct {
    UINT32 Enable;
    UINT32 NumOfTable;
    UINT32 DebugMsgOn;
    UINT32 Reserved[(SVC_CALIB_ST_LDC_NAND_HDR_SIZE / 4U) - 3U];
} SVC_CALIB_ST_LDC_NAND_HEADER_s;

typedef struct {
    UINT32                     Enable;
    // UINT32                     Channel;
    UINT32                     Version;
    UINT32                     HorGridNum;
    UINT32                     VerGridNum;
    UINT32                     TileWidthExp;
    UINT32                     TileHeightExp;
    SVC_CALIB_VIN_SENSOR_GEO_s VinSensorGeo;
    UINT32                     Warp2StageFlag;
    UINT32                     VinSelectBits;
    UINT32                     SensorSelectBits;
    UINT32                     TwoStageDone;
} SVC_CALIB_ST_LDC_TBL_HEADER_s;

typedef struct {
    SVC_CALIB_ST_LDC_TBL_HEADER_s Header;
    AMBA_IK_GRID_POINT_s          StLdcVector[SVC_CALIB_ST_LDC_MAX_TBL_LEN];
} SVC_CALIB_ST_LDC_NAND_TBL_s;

typedef struct {
    UINT32                        VinSelectBits;
    UINT32                        SensorSelectBits;
    AMBA_IK_WARP_INFO_s           WarpInfo;
    AMBA_IK_GRID_POINT_s          StLdcVector[SVC_CALIB_ST_LDC_MAX_TBL_LEN];
} SVC_CALIB_ST_LDC_TBL_s;

typedef struct {
    UINT32                        Enable;
    UINT8                        *pBuf;
    UINT32                        BufSize;
} SVC_CALIB_ST_LDC_MEM_CTRL_s;

typedef struct {
    char                            Name[16];
    AMBA_KAL_MUTEX_t                Mutex;
    SVC_CALIB_ST_LDC_NAND_HEADER_s *pNandHeader;
    SVC_CALIB_ST_LDC_NAND_TBL_s    *pNandTable;
    UINT32                          NumOfTable;
    SVC_CALIB_ST_LDC_TBL_s         *pStLdcTable;

    SVC_CALIB_ST_LDC_MEM_CTRL_s     MemCtrl[SVC_CALIB_ST_LDC_MEM_NUM];

    UINT32                          EnableDebugMsg;
} SVC_CALIB_ST_LDC_CTRL_s;

static UINT32                       SvcCalib_StLdcMemQuery(UINT32 MaxTableNum, UINT32 *pShadowSize, UINT32 *pWorkMemSize, UINT32 *pCalcMemSize);
static UINT32                       SvcCalib_StLdcVinSensorIDCheck(UINT32 VinID, UINT32 SensorID);
static UINT32                       SvcCalib_StLdcCreate(void);
static UINT32                       SvcCalib_StLdcShadowInit(const SVC_CALIB_OBJ_s *pCalObj);
static UINT32                       SvcCalib_StLdcTableUpdate(const SVC_CALIB_CHANNEL_s *pCalibChan, const AMBA_IK_MODE_CFG_s *pImgMode);
static UINT32                       SvcCalib_StLdcTableCfg(UINT32 NandTableID, UINT32 TableID);
static UINT32                       SvcCalib_StLdcTableSearch(UINT32 VinID, UINT32 SensorID, UINT32 *pTblIdx);
static UINT32                       SvcCalib_StLdcNandTableSearch(UINT32 VinID, UINT32 SensorID, UINT32 *pTblIdx);
static SVC_CALIB_ST_LDC_TBL_s*      SvcCalib_StLdcTableGet(const SVC_CALIB_CHANNEL_s *pCalibChan);
static UINT32                       SvcCalib_StLdcTableDataSet(UINT32 TblID, SVC_CALIB_TBL_INFO_s *pTblInfo);
static UINT32                       SvcCalib_StLdcMemLock(void);
static void                         SvcCalib_StLdcMemUnLock(void);

// Private function definition for calibration command
typedef UINT32 (*SVC_CALIB_ST_LDC_CMD_FUNC_f)(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
typedef struct {
     UINT32                      Enable;
     UINT32                      CmdID;
     SVC_CALIB_ST_LDC_CMD_FUNC_f pHandler;
} SVC_CALIB_ST_LDC_CMD_HDLR_s;

static void   SvcCalib_StLdcCmdHdlrInit(void);
static UINT32 SvcCalib_StLdcCmdMemQuery(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_StLdcCmdInit(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_StLdcCmdTblUpdate(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_StLdcCmdTblDataSet(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
//UINT32 SvcCalib_StitchLdcCmdFunc(UINT32 CmdID, void *pParam1, void *pParam2, void *pParam3, void *pParam4);

// Private function definition for shell command.
typedef UINT32 (*SVC_CALIB_ST_LDC_SHELL_FUNC_f)(UINT32 ArgCount, char * const *pArgVector);
typedef void   (*SVC_CALIB_ST_LDC_SHELL_USAGE_f)(void);

typedef struct {
    UINT32                         Enable;
    char                           ShellCmdName[32];
    SVC_CALIB_ST_LDC_SHELL_FUNC_f  pFunc;
    SVC_CALIB_ST_LDC_SHELL_USAGE_f pUsageFunc;
} SVC_CALIB_ST_LDC_SHELL_FUNC_s;

static UINT32 SvcCalib_StLdcShellCfgDbgMsg(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_StLdcShellCfgDbgMsgU(void);
static UINT32 SvcCalib_StLdcShellEnable(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_StLdcShellEnableU(void);
static void   SvcCalib_StLdcShellEntryInit(void);
static void   SvcCalib_StLdcShellUsage(void);

#define SVC_CALIB_ST_LDC_SHELL_CMD_NUM  (2U)
static SVC_CALIB_ST_LDC_SHELL_FUNC_s CalibStitchLdcShellFunc[SVC_CALIB_ST_LDC_SHELL_CMD_NUM] GNU_SECTION_NOZEROINIT;
static SVC_CALIB_ST_LDC_CMD_HDLR_s SvcCalibStitchLdcCmdHdlr[SVC_CALIB_CMD_NUM] GNU_SECTION_NOZEROINIT;
static SVC_CALIB_ST_LDC_CTRL_s SVC_CalibStitchLdcCtrl GNU_SECTION_NOZEROINIT;
#ifdef SVC_CALIB_DBG_MSG_ON
static UINT32 SVC_CalibStitchLdcCtrlFlag = SVC_LOG_CAL_DEF_FLG | SVC_LOG_CAL_DBG;
#else
static UINT32 SVC_CalibStitchLdcCtrlFlag = SVC_LOG_CAL_DEF_FLG;
#endif

#define SVC_LOG_ST_LDC "CAL_STLDC"

static UINT32 SvcCalib_StLdcMemQuery(UINT32 MaxTableNum, UINT32 *pShadowSize, UINT32 *pWorkMemSize, UINT32 *pCalcMemSize)
{
    UINT32 RetVal = SVC_OK;

    UINT32 DramShadowSize = 0, WorkingMemSize = 0, CalcMemSize = 0;
    UINT32 DramShadowHeaderSize = (UINT32)(sizeof(SVC_CALIB_ST_LDC_NAND_HEADER_s));
    UINT32 DramShadowTableSize  = (UINT32)(sizeof(SVC_CALIB_ST_LDC_NAND_TBL_s));
    UINT32 DramShadowDataSize   = DramShadowTableSize * MaxTableNum;
    UINT32 TableSize            = (UINT32)(sizeof(SVC_CALIB_ST_LDC_TBL_s));
    UINT32 TotalTableSize       = TableSize * MaxTableNum;
    SIZE_t CalcMemCtnSize       = (UINT32)(sizeof(DOUBLE)) * SVC_CALIB_ST_LDC_MAX_CURVE_LEN * 2U;
    SIZE_t CalcMemAlgoSize      = 0;
    SIZE_t CalcMemRemapSize     = SVC_CALIB_ST_LDC_REMAP_BUF_SIZE;

    if (CAL_OK != AmbaCal_LdcGetBufSize(&CalcMemAlgoSize)) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to query stitch ldc module memory - get stitch ldc algo working memory fail!", 0U, 0U);
    } else if (0U != AmbaCT_GetBufSize(AMBA_CT_TYPE_LDC, &CalcMemCtnSize)) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to query stitch ldc module memory - get stitch ldc ctuner working memory fail!", 0U, 0U);
    } else {
        DramShadowSize  = DramShadowHeaderSize;
        DramShadowSize += DramShadowDataSize;

        WorkingMemSize  = TotalTableSize;

        CalcMemSize     = CalcMemCtnSize;
        CalcMemSize    += CalcMemAlgoSize;
        CalcMemSize    += CalcMemRemapSize;

        SvcLog_DBG(SVC_LOG_ST_LDC, "", 0U, 0U);
        SvcLog_DBG(SVC_LOG_ST_LDC, "====== Memory Query Info ======", 0U, 0U);
        SvcLog_DBG(SVC_LOG_ST_LDC, "  MaxTableNum    : %d",   MaxTableNum, 0U);
        SvcLog_DBG(SVC_LOG_ST_LDC, "  DramShadowSize : 0x%x", DramShadowSize, 0U);
        SvcLog_DBG(SVC_LOG_ST_LDC, "    DramShadowHeaderSize : 0x%x", DramShadowHeaderSize, 0U);
        SVC_WRAP_PRINT "    DramShadowDataSize   : %#x ( %#x x %d )"
            SVC_PRN_ARG_S SVC_LOG_ST_LDC
            SVC_PRN_ARG_UINT32 DramShadowDataSize  SVC_PRN_ARG_POST
            SVC_PRN_ARG_UINT32 DramShadowTableSize SVC_PRN_ARG_POST
            SVC_PRN_ARG_UINT32 MaxTableNum         SVC_PRN_ARG_POST
            SVC_PRN_ARG_E
        SvcLog_DBG(SVC_LOG_ST_LDC, "  WorkingMemSize : 0x%X", WorkingMemSize, 0U);
        SVC_WRAP_PRINT "    TotalLdcTableSize : %#x ( %#x x %d )"
            SVC_PRN_ARG_S SVC_LOG_ST_LDC
            SVC_PRN_ARG_UINT32 TotalTableSize SVC_PRN_ARG_POST
            SVC_PRN_ARG_UINT32 TableSize      SVC_PRN_ARG_POST
            SVC_PRN_ARG_UINT32 MaxTableNum    SVC_PRN_ARG_POST
            SVC_PRN_ARG_E
        SvcLog_DBG(SVC_LOG_ST_LDC, "  CalcMemSize : 0x%x", CalcMemSize, 0U);
        SvcLog_DBG(SVC_LOG_ST_LDC, "    CalcMemCtnSize     : 0x%x", CalcMemCtnSize, 0U);
        SvcLog_DBG(SVC_LOG_ST_LDC, "    CalcMemAlgoSize    : 0x%x", CalcMemAlgoSize, 0U);
        SvcLog_DBG(SVC_LOG_ST_LDC, "    CalcMemRemapSize   : 0x%x", CalcMemRemapSize, 0U);

        if (pShadowSize != NULL) {
            *pShadowSize  = DramShadowSize;
        }
        SvcLog_DBG(SVC_LOG_ST_LDC, "Query the calib stitch ldc dram shadow size 0x%X ( table num %d )", DramShadowSize, MaxTableNum);

        if (pWorkMemSize != NULL) {
            *pWorkMemSize = WorkingMemSize;
        }
        SvcLog_DBG(SVC_LOG_ST_LDC, "Query the calib stitch ldc working memory size 0x%X ( table num %d )", WorkingMemSize, MaxTableNum);

        if (pCalcMemSize != NULL) {
            *pCalcMemSize = CalcMemSize;
        }
        SvcLog_DBG(SVC_LOG_ST_LDC, "Query the calib stitch ldc algo. memory size 0x%x ( table num %d )", CalcMemSize, MaxTableNum);
    }

    return RetVal;
}

static UINT32 SvcCalib_StLdcVinSensorIDCheck(UINT32 VinID, UINT32 SensorID)
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

static UINT32 SvcCalib_StLdcCreate(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibStitchLdcCtrlFlag & SVC_CALIB_ST_LDC_FLG_INIT) > 0U) {
        RetVal = SVC_OK;
        SvcLog_DBG(SVC_LOG_ST_LDC, "Calibration Stitch LDC module has been created!", 0U, 0U);
    } else {
        // Reset the stitch ldc control module
        AmbaSvcWrap_MisraMemset(&SVC_CalibStitchLdcCtrl, 0, sizeof(SVC_CalibStitchLdcCtrl));

        // Configure the name
        SvcWrap_strcpy(SVC_CalibStitchLdcCtrl.Name, sizeof(SVC_CalibStitchLdcCtrl.Name), SVC_CALIB_ST_LDC_NAME);

        // Create the mutex
        RetVal = AmbaKAL_MutexCreate(&(SVC_CalibStitchLdcCtrl.Mutex), SVC_CalibStitchLdcCtrl.Name);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_ST_LDC, "Fail to create stitch ldc - create mutex fail!", 0U, 0U);
        } else {
            PRetVal = (SVC_CalibStitchLdcCtrlFlag & (SVC_CALIB_ST_LDC_FLG_DBG_MSG | SVC_CALIB_ST_LDC_FLG_SHELLINIT | SVC_CALIB_ST_LDC_FLG_CMD_INIT));
            SVC_CalibStitchLdcCtrlFlag  = PRetVal;
            SVC_CalibStitchLdcCtrlFlag |= SVC_CALIB_ST_LDC_FLG_INIT;
            SvcLog_OK(SVC_LOG_ST_LDC, "Successful to create ldc!", 0U, 0U);
        }
    }

    return RetVal;
}

static UINT32 SvcCalib_StLdcShadowInit(const SVC_CALIB_OBJ_s *pCalObj)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibStitchLdcCtrlFlag & SVC_CALIB_ST_LDC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to initial stitch ldc dram shodow - create stitch ldc first!", 0U, 0U);
    } else if (pCalObj == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to initial stitch ldc dram shodow - invalid calib object!", 0U, 0U);
    } else if (pCalObj->Enable == 0U) {
        RetVal = 99;
        SvcLog_DBG(SVC_LOG_ST_LDC, "Disable stitch ldc dram shadow!!", 0U, 0U);
    } else {
        UINT32 DramShadowSize = 0, WorkingMemSize = 0;
        const SVC_CALIB_ST_LDC_NAND_HEADER_s *pHeader;

        AmbaMisra_TypeCast(&(pHeader), &(pCalObj->pShadowBuf));

        RetVal = SvcCalib_StLdcMemQuery(pCalObj->NumOfTable, &DramShadowSize, &WorkingMemSize, NULL);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_ST_LDC, "Fail to initial stitch ldc dram shadow - query memory fail!", 0U, 0U);
        } else if ((pCalObj->pShadowBuf == NULL) || (pHeader == NULL)) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ST_LDC, "Fail to initial stitch ldc dram shadow - dram shadow buffer should not null!", 0U, 0U);
        } else if (pCalObj->ShadowBufSize < DramShadowSize) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ST_LDC, "Fail to initial stitch ldc dram shadow - prepare dram shadow size is too small!", 0U, 0U);
        } else if (pCalObj->pWorkingBuf == NULL) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ST_LDC, "Fail to initial stitch ldc dram shadow - working buffer should not null!", 0U, 0U);
        } else if (pCalObj->WorkingBufSize < WorkingMemSize) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ST_LDC, "Fail to initial stitch ldc dram shadow - working buffer size is too small!", 0U, 0U);
        } else if (pHeader->NumOfTable > pCalObj->NumOfTable) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ST_LDC, "Fail to initial stitch ldc dram shadow - calib nand table count(%d) > max stitch ldc dram shadow table count(%d)",
                    pHeader->NumOfTable, pCalObj->NumOfTable);
        } else {
            if (KAL_ERR_NONE != AmbaKAL_MutexTake(&(SVC_CalibStitchLdcCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ST_LDC, "Fail to initial stitch ldc dram shadow - take mutex fail!", 0U, 0U);
            } else {
                UINT8 *pNandHeaderBuf    = pCalObj->pShadowBuf;
                UINT32 NandHeaderSize    = (UINT32)(sizeof(SVC_CALIB_ST_LDC_NAND_HEADER_s));
                UINT8 *pNandTableBuf     = &(pNandHeaderBuf[NandHeaderSize]);
                UINT32 TotalLdcTableSize = sizeof(SVC_CALIB_ST_LDC_TBL_s) * pCalObj->NumOfTable;

                SvcLog_DBG(SVC_LOG_ST_LDC, "====== Shadow initialized Start ======", 0U, 0U);
                SVC_WRAP_PRINT "  DramShadow Addr  : %p 0x%x"
                    SVC_PRN_ARG_S SVC_LOG_ST_LDC
                    SVC_PRN_ARG_CPOINT pCalObj->pShadowBuf    SVC_PRN_ARG_POST
                    SVC_PRN_ARG_UINT32 pCalObj->ShadowBufSize SVC_PRN_ARG_POST
                    SVC_PRN_ARG_E
                SVC_WRAP_PRINT "    Nand Header Addr : %p 0x%x"
                    SVC_PRN_ARG_S SVC_LOG_ST_LDC
                    SVC_PRN_ARG_CPOINT pNandHeaderBuf SVC_PRN_ARG_POST
                    SVC_PRN_ARG_UINT32 NandHeaderSize SVC_PRN_ARG_POST
                    SVC_PRN_ARG_E
                SVC_WRAP_PRINT "    Nand Table Addr  : %p"
                    SVC_PRN_ARG_S SVC_LOG_ST_LDC
                    SVC_PRN_ARG_CPOINT pNandTableBuf SVC_PRN_ARG_POST
                    SVC_PRN_ARG_E
                SVC_WRAP_PRINT "  Working Mem Addr : %p 0x%x"
                    SVC_PRN_ARG_S SVC_LOG_ST_LDC
                    SVC_PRN_ARG_CPOINT pCalObj->pWorkingBuf    SVC_PRN_ARG_POST
                    SVC_PRN_ARG_UINT32 pCalObj->WorkingBufSize SVC_PRN_ARG_POST
                    SVC_PRN_ARG_E
                SVC_WRAP_PRINT "    Stitch LdcTable Addr : %p 0x%08x"
                    SVC_PRN_ARG_S SVC_LOG_ST_LDC
                    SVC_PRN_ARG_CPOINT pCalObj->pWorkingBuf SVC_PRN_ARG_POST
                    SVC_PRN_ARG_UINT32 TotalLdcTableSize    SVC_PRN_ARG_POST
                    SVC_PRN_ARG_E

                // Configure nand header
                AmbaMisra_TypeCast(&(SVC_CalibStitchLdcCtrl.pNandHeader), &(pNandHeaderBuf));
                // Configure nand table
                AmbaMisra_TypeCast(&(SVC_CalibStitchLdcCtrl.pNandTable),  &(pNandTableBuf)); AmbaMisra_TouchUnused(pNandTableBuf);
                // Configure working stitch ldc table
                AmbaMisra_TypeCast(&(SVC_CalibStitchLdcCtrl.pStLdcTable),  &(pCalObj->pWorkingBuf));
                if (SVC_CalibStitchLdcCtrl.pNandHeader == NULL) {
                    RetVal = SVC_NG;
                    SvcLog_NG(SVC_LOG_ST_LDC, "Fail to initial stitch ldc dram shadow - nand header should not null!", 0U, 0U);
                } else if (SVC_CalibStitchLdcCtrl.pNandTable == NULL) {
                    RetVal = SVC_NG;
                    SvcLog_NG(SVC_LOG_ST_LDC, "Fail to initial stitch ldc dram shadow - nand table should not null!", 0U, 0U);
                } else if (SVC_CalibStitchLdcCtrl.pStLdcTable == NULL) {
                    RetVal = SVC_NG;
                    SvcLog_NG(SVC_LOG_ST_LDC, "Fail to initial stitch ldc dram shadow - stitch ldc table should not null!", 0U, 0U);
                } else {
                    SVC_CalibStitchLdcCtrl.NumOfTable = pCalObj->NumOfTable;
                    AmbaSvcWrap_MisraMemset(SVC_CalibStitchLdcCtrl.pStLdcTable, 0, TotalLdcTableSize);
                    SVC_CalibStitchLdcCtrl.EnableDebugMsg = SVC_CalibStitchLdcCtrl.pNandHeader->DebugMsgOn;
                    if (SVC_CalibStitchLdcCtrl.EnableDebugMsg > 0U) {
                        UINT32 Idx;

                        SvcLog_DBG(SVC_LOG_ST_LDC, "", 0U, 0U);
                        SvcLog_DBG(SVC_LOG_ST_LDC, "---- Stitch LdcNand Info %d ----", SVC_CalibStitchLdcCtrl.pNandHeader->Reserved[0], 0U);
                        SvcLog_DBG(SVC_LOG_ST_LDC, "  Enable         : %d", SVC_CalibStitchLdcCtrl.pNandHeader->Enable, 0U);
                        SvcLog_DBG(SVC_LOG_ST_LDC, "  NumOfTable     : %d", SVC_CalibStitchLdcCtrl.pNandHeader->NumOfTable, 0U);
                        SvcLog_DBG(SVC_LOG_ST_LDC, "  DebugMsgOn     : %d", SVC_CalibStitchLdcCtrl.pNandHeader->DebugMsgOn, 0U);

                        {
                            const SVC_CALIB_ST_LDC_NAND_TBL_s *pNandTable;

                            for (Idx = 0U; Idx < pCalObj->NumOfTable; Idx ++) {
                                pNandTable = &(SVC_CalibStitchLdcCtrl.pNandTable[Idx]);

                                SVC_WRAP_PRINT "  ---- Nand Table[%d] %p -----"
                                    SVC_PRN_ARG_S SVC_LOG_ST_LDC
                                    SVC_PRN_ARG_UINT32 Idx        SVC_PRN_ARG_POST
                                    SVC_PRN_ARG_CPOINT pNandTable SVC_PRN_ARG_POST
                                    SVC_PRN_ARG_E
                                SvcLog_DBG(SVC_LOG_ST_LDC, "    VinSelectBits       : 0x%X",   pNandTable->Header.VinSelectBits       , 0U);
                                SvcLog_DBG(SVC_LOG_ST_LDC, "    SensorSelectBits    : 0x%X",   pNandTable->Header.SensorSelectBits    , 0U);
                                SvcLog_DBG(SVC_LOG_ST_LDC, "    Enable              : %d",     pNandTable->Header.Enable              , 0U);
                                SvcLog_DBG(SVC_LOG_ST_LDC, "    Version             : 0x%X",   pNandTable->Header.Version             , 0U);
                                SvcLog_DBG(SVC_LOG_ST_LDC, "    HorGridNum          : %d",     pNandTable->Header.HorGridNum          , 0U);
                                SvcLog_DBG(SVC_LOG_ST_LDC, "    VerGridNum          : %d",     pNandTable->Header.VerGridNum          , 0U);
                                SvcLog_DBG(SVC_LOG_ST_LDC, "    TileWidthExp        : %d",     pNandTable->Header.TileWidthExp        , 0U);
                                SvcLog_DBG(SVC_LOG_ST_LDC, "    TileHeightExp       : %d",     pNandTable->Header.TileHeightExp       , 0U);
                                SvcLog_DBG(SVC_LOG_ST_LDC, "    Warp2StageFlag      : %d",     pNandTable->Header.Warp2StageFlag      , 0U);
                                SvcLog_DBG(SVC_LOG_ST_LDC, "    VinSensorGeo.StartX : %d",     pNandTable->Header.VinSensorGeo.StartX , 0U);
                                SvcLog_DBG(SVC_LOG_ST_LDC, "    VinSensorGeo.StartY : %d",     pNandTable->Header.VinSensorGeo.StartY , 0U);
                                SvcLog_DBG(SVC_LOG_ST_LDC, "    VinSensorGeo.Width  : %d",     pNandTable->Header.VinSensorGeo.Width  , 0U);
                                SvcLog_DBG(SVC_LOG_ST_LDC, "    VinSensorGeo.Height : %d",     pNandTable->Header.VinSensorGeo.Height , 0U);
                                SVC_WRAP_PRINT "    LdcVector           : %p"
                                    SVC_PRN_ARG_S SVC_LOG_ST_LDC
                                    SVC_PRN_ARG_CPOINT pNandTable->StLdcVector SVC_PRN_ARG_POST
                                    SVC_PRN_ARG_E
                            }
                        }

                        SvcLog_DBG(SVC_LOG_ST_LDC, "", 0U, 0U);
                        SvcLog_DBG(SVC_LOG_ST_LDC, "---- Stitch LdcTable Info ----", 0U, 0U);
                        SvcLog_DBG(SVC_LOG_ST_LDC, "  NumOfTable     : %d", SVC_CalibStitchLdcCtrl.NumOfTable, 0U);

                        {
                            const SVC_CALIB_ST_LDC_TBL_s *pStLdcTable;

                            for (Idx = 0U; Idx < SVC_CalibStitchLdcCtrl.NumOfTable; Idx ++) {
                                pStLdcTable = &(SVC_CalibStitchLdcCtrl.pStLdcTable[Idx]);

                                SVC_WRAP_PRINT "  ---- Stitch LdcTable[%d] %p -----"
                                    SVC_PRN_ARG_S SVC_LOG_ST_LDC
                                    SVC_PRN_ARG_UINT32 Idx         SVC_PRN_ARG_POST
                                    SVC_PRN_ARG_CPOINT pStLdcTable SVC_PRN_ARG_POST
                                    SVC_PRN_ARG_E
                                SvcLog_DBG(SVC_LOG_ST_LDC, "    VinSelectBits       : 0x%X",   SVC_CalibStitchLdcCtrl.pStLdcTable[Idx].VinSelectBits       , 0U);
                                SvcLog_DBG(SVC_LOG_ST_LDC, "    SensorSelectBits    : 0x%X",   SVC_CalibStitchLdcCtrl.pStLdcTable[Idx].SensorSelectBits    , 0U);
                                SVC_WRAP_PRINT "    Table Addr          : %p"
                                    SVC_PRN_ARG_S SVC_LOG_ST_LDC
                                    SVC_PRN_ARG_CPOINT SVC_CalibStitchLdcCtrl.pStLdcTable[Idx].StLdcVector SVC_PRN_ARG_POST
                                    SVC_PRN_ARG_E
                            }
                        }
                    }

                    SVC_CalibStitchLdcCtrlFlag |= SVC_CALIB_ST_LDC_FLG_SHDW_INIT;
                    SvcLog_OK(SVC_LOG_ST_LDC, "Successful to initial stitch ldc dram shadow!", 0U, 0U);
                }

                PRetVal = AmbaKAL_MutexGive(&(SVC_CalibStitchLdcCtrl.Mutex)); AmbaMisra_TouchUnused(&PRetVal);
            }
        }
    }

    return RetVal;
}

static UINT32 SvcCalib_StLdcTableUpdate(const SVC_CALIB_CHANNEL_s *pCalibChan, const AMBA_IK_MODE_CFG_s *pImgMode)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibStitchLdcCtrlFlag & SVC_CALIB_ST_LDC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to update stitch ldc table - initial stitch ldc module first!", 0U, 0U);
    } else {
        if ((SVC_CalibStitchLdcCtrlFlag & SVC_CALIB_ST_LDC_FLG_SHDW_INIT) == 0U) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ST_LDC, "Fail to update stitch ldc table - initial stitch ldc dram shadow first!", 0U, 0U);
        }
    }

    if (pCalibChan == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to update stitch ldc table - invalid calib channel!", 0U, 0U);
    }

    if (pImgMode == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to update stitch ldc table - invalid image mode!", 0U, 0U);
    }

    if (SVC_CalibStitchLdcCtrl.pNandHeader != NULL) {
        if ((SVC_CalibStitchLdcCtrl.pNandHeader->Enable == 0U) ||
            (SVC_CalibStitchLdcCtrl.pNandHeader->NumOfTable == 0U)) {
            RetVal = SVC_NG;
        }
    }

    if ((RetVal == SVC_OK) &&
        (pCalibChan != NULL) &&
        (pImgMode != NULL)) {
        if (KAL_ERR_NONE != AmbaKAL_MutexTake(&(SVC_CalibStitchLdcCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ST_LDC, "Fail to update stitch ldc table - take mutex fail!", 0U, 0U);
        } else {
            const SVC_CALIB_ST_LDC_TBL_s  *pStLdcTable = SvcCalib_StLdcTableGet(pCalibChan);

            if (pStLdcTable == NULL) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ST_LDC, "Fail to update stitch ldc table - get stitch ldc table fail", 0U, 0U);
            } else {
                if (SVC_CalibStitchLdcCtrl.EnableDebugMsg > 0U) {

                    SvcLog_DBG(SVC_LOG_ST_LDC, "", 0U, 0U);
                    SvcLog_DBG(SVC_LOG_ST_LDC, "====== Calibration stitch ldc info ======", 0U, 0U);
                    SvcLog_DBG(SVC_LOG_ST_LDC, "  VinSelectBits                     : 0x%X", pStLdcTable->VinSelectBits                              , 0U);
                    SvcLog_DBG(SVC_LOG_ST_LDC, "  SensorSelectBits                  : 0x%X", pStLdcTable->SensorSelectBits                           , 0U);
                    SvcLog_DBG(SVC_LOG_ST_LDC, "  Version                           : 0x%X", pStLdcTable->WarpInfo.Version                           , 0U);
                    SvcLog_DBG(SVC_LOG_ST_LDC, "  HorGridNum                        : %d",   pStLdcTable->WarpInfo.HorGridNum                        , 0U);
                    SvcLog_DBG(SVC_LOG_ST_LDC, "  VerGridNum                        : %d",   pStLdcTable->WarpInfo.VerGridNum                        , 0U);
                    SvcLog_DBG(SVC_LOG_ST_LDC, "  TileWidthExp                      : %d",   pStLdcTable->WarpInfo.TileWidthExp                      , 0U);
                    SvcLog_DBG(SVC_LOG_ST_LDC, "  TileHeightExp                     : %d",   pStLdcTable->WarpInfo.TileHeightExp                     , 0U);
                    SvcLog_DBG(SVC_LOG_ST_LDC, "  VinSensorGeo.StartX               : %d",   pStLdcTable->WarpInfo.VinSensorGeo.StartX               , 0U);
                    SvcLog_DBG(SVC_LOG_ST_LDC, "  VinSensorGeo.StartY               : %d",   pStLdcTable->WarpInfo.VinSensorGeo.StartY               , 0U);
                    SvcLog_DBG(SVC_LOG_ST_LDC, "  VinSensorGeo.Width                : %d",   pStLdcTable->WarpInfo.VinSensorGeo.Width                , 0U);
                    SvcLog_DBG(SVC_LOG_ST_LDC, "  VinSensorGeo.Height               : %d",   pStLdcTable->WarpInfo.VinSensorGeo.Height               , 0U);
                    SvcLog_DBG(SVC_LOG_ST_LDC, "  VinSensorGeo.HSubSample.FactorDen : %d",   pStLdcTable->WarpInfo.VinSensorGeo.HSubSample.FactorDen , 0U);
                    SvcLog_DBG(SVC_LOG_ST_LDC, "  VinSensorGeo.HSubSample.FactorNum : %d",   pStLdcTable->WarpInfo.VinSensorGeo.HSubSample.FactorNum , 0U);
                    SvcLog_DBG(SVC_LOG_ST_LDC, "  VinSensorGeo.VSubSample.FactorDen : %d",   pStLdcTable->WarpInfo.VinSensorGeo.VSubSample.FactorDen , 0U);
                    SvcLog_DBG(SVC_LOG_ST_LDC, "  VinSensorGeo.VSubSample.FactorNum : %d",   pStLdcTable->WarpInfo.VinSensorGeo.VSubSample.FactorNum , 0U);
                    SvcLog_DBG(SVC_LOG_ST_LDC, "  Enb2StageCompensation             : %d",   pStLdcTable->WarpInfo.Enb_2StageCompensation            , 0U);
                    SVC_WRAP_PRINT "  pWarp                             : %p"
                        SVC_PRN_ARG_S SVC_LOG_ST_LDC
                        SVC_PRN_ARG_CPOINT pStLdcTable->WarpInfo.pWarp SVC_PRN_ARG_POST
                        SVC_PRN_ARG_E

                    if (SVC_CalibStitchLdcCtrl.EnableDebugMsg == 2U) {
                        // SvcCalib_StLdcTableDump(pStLdcTable);
                    }
                }

                if (0U != AmbaIK_SetWarpInfo(pImgMode, &(pStLdcTable->WarpInfo))) {
                    RetVal = SVC_NG;
                    SvcLog_NG(SVC_LOG_ST_LDC, "Fail to update stitch ldc table - set stitch ldc info fail!", 0U, 0U);
                } else {
                    RetVal = AmbaIK_SetWarpEnb(pImgMode, 1U);
                    if (RetVal != SVC_OK) {
                        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to update stitch ldc table - enable warp fail!", 0U, 0U);
                    }
                }

                if (RetVal == SVC_OK) {

                    SVC_WRAP_PRINT "Successful to update the stitch ldc table VinID(%d) SensorID(%d) ImgContextID(%d)"
                        SVC_PRN_ARG_S SVC_LOG_ST_LDC
                        SVC_PRN_ARG_UINT32 pCalibChan->VinID    SVC_PRN_ARG_POST
                        SVC_PRN_ARG_UINT32 pCalibChan->SensorID SVC_PRN_ARG_POST
                        SVC_PRN_ARG_UINT32 pImgMode->ContextId  SVC_PRN_ARG_POST
                        SVC_PRN_ARG_E

                }
            }

            PRetVal = AmbaKAL_MutexGive(&(SVC_CalibStitchLdcCtrl.Mutex)); AmbaMisra_TouchUnused(&PRetVal);
        }
    }

    return RetVal;
}

static UINT32 SvcCalib_StLdcTableCfg(UINT32 NandTableID, UINT32 TableID)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibStitchLdcCtrlFlag & SVC_CALIB_ST_LDC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, " Fail to configure stitch ldc table - Need to initial stitch ldc control module first!", 0U, 0U);
    } else if ((SVC_CalibStitchLdcCtrlFlag & SVC_CALIB_ST_LDC_FLG_SHDW_INIT) == 0U) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, " Fail to configure stitch ldc table - Need to initial stitch ldc dram shadow first!", 0U, 0U);
    } else if (SVC_CalibStitchLdcCtrl.pNandTable == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, " Fail to configure stitch ldc table - invalid nand table!", 0U, 0U);
    } else if (SVC_CalibStitchLdcCtrl.pStLdcTable == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, " Fail to configure stitch ldc table - invalid stitch ldc table!", 0U, 0U);
    } else if (TableID >= SVC_CalibStitchLdcCtrl.NumOfTable) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, " Fail to configure stitch ldc table - output stitch ldc table id is out-of max table size!", 0U, 0U);
    } else if (NandTableID >= SVC_CalibStitchLdcCtrl.NumOfTable) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, " Fail to configure stitch ldc table - nand stitch ldc table id is out-of max table size!", 0U, 0U);
    } else {
        SVC_CALIB_ST_LDC_TBL_s *pStLdcTable = &(SVC_CalibStitchLdcCtrl.pStLdcTable[TableID]);

        /* Reset the stitch ldc table context */
        AmbaSvcWrap_MisraMemset(pStLdcTable, 0, sizeof(SVC_CALIB_ST_LDC_TBL_s));

        /* Config stitch ldc table */
        {
            const SVC_CALIB_ST_LDC_NAND_TBL_s *pNandTable = &(SVC_CalibStitchLdcCtrl.pNandTable[NandTableID]);

            if ((pNandTable->Header.HorGridNum * pNandTable->Header.VerGridNum) > (SVC_CALIB_ST_LDC_MAX_TBL_LEN)) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ST_LDC, " Warp table grid is out-of range! %dx%d", pNandTable->Header.HorGridNum, pNandTable->Header.VerGridNum);
            } else {
                pStLdcTable->WarpInfo.Version                           = pNandTable->Header.Version;
                pStLdcTable->WarpInfo.HorGridNum                        = pNandTable->Header.HorGridNum;
                pStLdcTable->WarpInfo.VerGridNum                        = pNandTable->Header.VerGridNum;
                pStLdcTable->WarpInfo.TileWidthExp                      = pNandTable->Header.TileWidthExp;
                pStLdcTable->WarpInfo.TileHeightExp                     = pNandTable->Header.TileHeightExp;
                pStLdcTable->WarpInfo.VinSensorGeo.StartX               = pNandTable->Header.VinSensorGeo.StartX;
                pStLdcTable->WarpInfo.VinSensorGeo.StartY               = pNandTable->Header.VinSensorGeo.StartY;
                pStLdcTable->WarpInfo.VinSensorGeo.Width                = pNandTable->Header.VinSensorGeo.Width;
                pStLdcTable->WarpInfo.VinSensorGeo.Height               = pNandTable->Header.VinSensorGeo.Height;
                pStLdcTable->WarpInfo.VinSensorGeo.HSubSample.FactorDen = pNandTable->Header.VinSensorGeo.HSubSample.FactorDen;
                pStLdcTable->WarpInfo.VinSensorGeo.HSubSample.FactorNum = pNandTable->Header.VinSensorGeo.HSubSample.FactorNum;
                pStLdcTable->WarpInfo.VinSensorGeo.VSubSample.FactorDen = pNandTable->Header.VinSensorGeo.VSubSample.FactorDen;
                pStLdcTable->WarpInfo.VinSensorGeo.VSubSample.FactorNum = pNandTable->Header.VinSensorGeo.VSubSample.FactorNum;
                pStLdcTable->WarpInfo.pWarp = pStLdcTable->StLdcVector;

                if (pNandTable->Header.TwoStageDone == 0U) {
                    pStLdcTable->WarpInfo.Enb_2StageCompensation = SVC_CALIB_ST_LDC_IK_2_STAGE;
                } else {
                    pStLdcTable->WarpInfo.Enb_2StageCompensation = SVC_CALIB_ST_LDC_CALIB_2_STAGE;
                }

                pStLdcTable->VinSelectBits = pNandTable->Header.VinSelectBits;
                pStLdcTable->SensorSelectBits = pNandTable->Header.SensorSelectBits;

                PRetVal = AmbaWrap_memcpy(pStLdcTable->StLdcVector, pNandTable->StLdcVector,
                       ((UINT32)(sizeof(AMBA_IK_GRID_POINT_s)) * pNandTable->Header.HorGridNum * pNandTable->Header.VerGridNum));
                AmbaMisra_TouchUnused(&PRetVal);
            }
        }
    }

    return RetVal;
}

static UINT32 SvcCalib_StLdcTableSearch(UINT32 VinID, UINT32 SensorID, UINT32 *pTblIdx)
{
    UINT32 RetVal = SVC_OK;

    if (SVC_OK != SvcCalib_StLdcVinSensorIDCheck(VinID, SensorID)) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to search stitch ldc table - invalid calib channel -> VinID(%d), SensorID(%d)!", VinID, SensorID);
    } else {
        UINT32 Idx, TableIdx = 0xFFFFFFFFU;

        for (Idx = 0U; Idx < SVC_CalibStitchLdcCtrl.NumOfTable; Idx ++) {
            if (((SVC_CalibStitchLdcCtrl.pStLdcTable[Idx].VinSelectBits & (0x1UL << VinID)) > 0U) &&
                ((SVC_CalibStitchLdcCtrl.pStLdcTable[Idx].SensorSelectBits & SensorID) > 0U)) {
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

static UINT32 SvcCalib_StLdcNandTableSearch(UINT32 VinID, UINT32 SensorID, UINT32 *pTblIdx)
{
    UINT32 RetVal = SVC_OK;

    if (SVC_OK != SvcCalib_StLdcVinSensorIDCheck(VinID, SensorID)) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to search stitch ldc nand table - invalid calib channel -> VinID(%d), SensorID(%d)!", VinID, SensorID);
    } else {
        UINT32 Idx;

        if (pTblIdx != NULL) {
            *pTblIdx = 0xFFFFFFFFU;
        }

        for (Idx = 0U; Idx < SVC_CalibStitchLdcCtrl.NumOfTable; Idx ++) {
            if (((SVC_CalibStitchLdcCtrl.pNandTable[Idx].Header.VinSelectBits & (0x1UL << VinID)) > 0U) &&
                ((SVC_CalibStitchLdcCtrl.pNandTable[Idx].Header.SensorSelectBits & SensorID) > 0U)) {
                if (pTblIdx != NULL) {
                    *pTblIdx = Idx;
                }
                break;
            }
        }
    }

    return RetVal;
}

static SVC_CALIB_ST_LDC_TBL_s * SvcCalib_StLdcTableGet(const SVC_CALIB_CHANNEL_s *pCalibChan)
{
    UINT32 RetVal = SVC_OK;
    SVC_CALIB_ST_LDC_TBL_s *pTable = NULL;

    if ((SVC_CalibStitchLdcCtrlFlag & SVC_CALIB_ST_LDC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to get stitch ldc table - Need to initial stitch ldc control module first!", 0U, 0U);
    } else if ((SVC_CalibStitchLdcCtrlFlag & SVC_CALIB_ST_LDC_FLG_SHDW_INIT) == 0U) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to get stitch ldc table - Need to initial stitch ldc dram shadow first!", 0U, 0U);
    } else if (SVC_CalibStitchLdcCtrl.pStLdcTable == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to get stitch ldc table - invalid stitch ldc table!", 0U, 0U);
    } else if (pCalibChan == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to get stitch ldc table - invalid calib channel!", 0U, 0U);
    } else if (SVC_OK != SvcCalib_StLdcVinSensorIDCheck(pCalibChan->VinID, pCalibChan->SensorID)) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to get stitch ldc table - invalid calib channel -> VinID(%d), SensorID(0x%x)!", pCalibChan->VinID, pCalibChan->SensorID);
    } else if (pCalibChan->VinSelectBits > 0xFU) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to get stitch ldc table - invalid calib channel -> VinSelectBits(%d)!", pCalibChan->VinSelectBits, 0U);
    } else if (pCalibChan->SensorSelectBits > 0xFU) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to get stitch ldc table - invalid calib channel -> SensorSelectBits(%d)!", pCalibChan->SensorSelectBits, 0U);
    } else {
        UINT32 Idx;
        UINT32 TableIdx     = 0xFFFFFFFFU;
        UINT32 NandTableIdx = 0xFFFFFFFFU;

        RetVal  = SvcCalib_StLdcTableSearch(pCalibChan->VinID, pCalibChan->SensorID, &TableIdx);
        RetVal |= SvcCalib_StLdcNandTableSearch(pCalibChan->VinID, pCalibChan->SensorID, &NandTableIdx);
        if (RetVal == SVC_OK) {
            /* If not get stitch ldc table, search the nand table by calib channel */
            if (TableIdx == 0xFFFFFFFFU) {
                if (NandTableIdx < SVC_CalibStitchLdcCtrl.NumOfTable) {
                    /* Search free stitch ldc table */
                    for (Idx = 0; Idx < SVC_CalibStitchLdcCtrl.NumOfTable; Idx ++) {
                        if ((SVC_CalibStitchLdcCtrl.pStLdcTable[Idx].VinSelectBits == 0U) &&
                            (SVC_CalibStitchLdcCtrl.pStLdcTable[Idx].SensorSelectBits == 0U)) {
                            TableIdx = Idx;
                            SvcLog_DBG(SVC_LOG_ST_LDC, "Successful to get stitch ldc free table(%d).", TableIdx, 0U);
                            break;
                        }
                    }

                    /* There is not free stitch ldc table to service it */
                    if (TableIdx == 0xFFFFFFFFU) {
                        RetVal = SVC_NG;
                        SvcLog_DBG(SVC_LOG_ST_LDC, "Fail to get stitch ldc table - there is not free table to support VinID(%d), SensorID(0x%X)", pCalibChan->VinID, pCalibChan->SensorID);
                    }
                } else {
                    RetVal = SVC_NG;
                    SvcLog_DBG(SVC_LOG_ST_LDC, "Fail to get stitch ldc table - current stitch ldc cannot support VinID(%d), SensorID(0x%X)", pCalibChan->VinID, pCalibChan->SensorID);
                }
            }

            /* generate stitch ldc table */
            if ((TableIdx < SVC_CalibStitchLdcCtrl.NumOfTable) && (NandTableIdx < SVC_CalibStitchLdcCtrl.NumOfTable)) {
                RetVal = SvcCalib_StLdcTableCfg(NandTableIdx, TableIdx);
                if (RetVal != SVC_OK) {
                    SvcLog_DBG(SVC_LOG_ST_LDC, "Fail to get stitch ldc table - re-configure stitch ldc table fail!", 0U, 0U);
                    pTable = NULL;
                } else {
                    SvcLog_DBG(SVC_LOG_ST_LDC, "Successful to get stitch ldc table - configure done!", 0U, 0U);
                    pTable = &(SVC_CalibStitchLdcCtrl.pStLdcTable[TableIdx]);
                }
            } else {
                if (TableIdx < SVC_CalibStitchLdcCtrl.NumOfTable) {
                    pTable = &(SVC_CalibStitchLdcCtrl.pStLdcTable[TableIdx]);
                    SvcLog_OK(SVC_LOG_ST_LDC, "Successful to get stitch ldc table - existed table!", 0U, 0U);
                }
            }
        }
    }

    AmbaMisra_TouchUnused(&RetVal);

    return pTable;
}

static UINT32 SvcCalib_StLdcMemLock(void)
{
    UINT32 RetVal = SVC_OK;
    UINT32 CalcMemRemapSize = SVC_CALIB_ST_LDC_REMAP_BUF_SIZE;
    UINT32 CalcMemSize = 0U;
    UINT8 *pCalcMem;

    if (0U != SvcCalib_ItemCalcMemGet(SVC_CALIB_STITCH_LDC_ID, 1U, &pCalcMem, &CalcMemSize)) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to lock stitch ldc memory - calc mem get fail!", 0U, 0U);
    } else {
        AmbaSvcWrap_MisraMemset(&(SVC_CalibStitchLdcCtrl.MemCtrl[SVC_CALIB_ST_LDC_MEM_CALC_REMAP]), 0, sizeof(SVC_CALIB_ST_LDC_MEM_CTRL_s));
        SVC_CalibStitchLdcCtrl.MemCtrl[SVC_CALIB_ST_LDC_MEM_CALC_REMAP].BufSize = CalcMemRemapSize;
        SVC_CalibStitchLdcCtrl.MemCtrl[SVC_CALIB_ST_LDC_MEM_CALC_REMAP].pBuf    = pCalcMem;
        SVC_CalibStitchLdcCtrl.MemCtrl[SVC_CALIB_ST_LDC_MEM_CALC_REMAP].Enable  = 1U;
    }

    return RetVal;
}

static void SvcCalib_StLdcMemUnLock(void)
{
    // Reset the memory control setting
    AmbaSvcWrap_MisraMemset(SVC_CalibStitchLdcCtrl.MemCtrl, 0, sizeof(SVC_CalibStitchLdcCtrl.MemCtrl));

    // Free the calculation memory
    if (0U != SvcCalib_ItemCalcMemFree(SVC_CALIB_STITCH_LDC_ID)) {
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to un-lock stitch ldc memory - init calculation memory control fail!", 0U, 0U);
    } else {
        SvcLog_OK(SVC_LOG_ST_LDC, "Successful to un-lock stitch ldc calc memory!", 0U, 0U);
    }
}

static UINT32 SvcCalib_StLdcTableDataSet(UINT32 TblID, SVC_CALIB_TBL_INFO_s *pTblInfo)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibStitchLdcCtrlFlag & SVC_CALIB_ST_LDC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to set stitch ldc table - Need to initial stitch ldc control module first!", 0U, 0U);
    } else if ((SVC_CalibStitchLdcCtrlFlag & SVC_CALIB_ST_LDC_FLG_SHDW_INIT) == 0U) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to set stitch ldc table - Need to initial stitch ldc dram shadow first!", 0U, 0U);
    } else if (TblID >= SVC_CalibStitchLdcCtrl.NumOfTable) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Failure to set stitch ldc table - TableID(%d) out-of range(%d)!", TblID, SVC_CalibStitchLdcCtrl.NumOfTable);
    } else if (pTblInfo == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Failure to set stitch ldc table - output table info should not null!", 0U, 0U);
    } else if (pTblInfo->pBuf == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Failure to set stitch ldc table - input table buffer should not null!", 0U, 0U);
    } else if (pTblInfo->BufSize < sizeof(SVC_CALIB_ST_LDC_TBL_DATA_s)) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Failure to set stitch ldc table - input table buffer size is too small! 0x%x/0x%x", pTblInfo->BufSize, sizeof(SVC_CALIB_ST_LDC_TBL_DATA_s));
    } else if ((pTblInfo->CalChan.VinSelectBits == 0U) ||
               (pTblInfo->CalChan.SensorSelectBits == 0U)) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Failure to set stitch ldc table - invalid calib channel!", 0U, 0U);
    } else if (0U != SvcCalib_StLdcMemLock()) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Failure to set stitch ldc table - lock stitch ldc memory fail!", 0U, 0U);
    } else {

        if (KAL_ERR_NONE != AmbaKAL_MutexTake(&(SVC_CalibStitchLdcCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ST_LDC, "Failure to set stitch ldc table - take mutex fail!", 0U, 0U);
        } else {
            UINT32 IsNewData = 0U;
            SVC_CALIB_ST_LDC_TBL_DATA_s *pInput = NULL;
            SVC_CALIB_ST_LDC_NAND_TBL_s *pNandTbl = &(SVC_CalibStitchLdcCtrl.pNandTable[TblID]);

            AmbaMisra_TypeCast(&(pInput), &(pTblInfo->pBuf));

            if (pInput->StLdcVectorLength > SVC_CALIB_ST_LDC_MAX_TBL_LEN) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ST_LDC, "Failure to set stitch ldc table - input stitch ldc vector is out-of range!", 0U, 0U);
            } else {

                if (pNandTbl->Header.VinSelectBits == 0U) {
                    IsNewData = 1U;
                }

                AmbaSvcWrap_MisraMemset(pNandTbl, 0, sizeof(SVC_CALIB_ST_LDC_NAND_TBL_s));

#ifndef CONFIG_SOC_CV2FS
                pNandTbl->Header.Enable        = 1U;
                pNandTbl->Header.Version       = SVC_CALIB_ST_LDC_VERSION;
                pNandTbl->Header.HorGridNum    = pInput->HorGridNum;
                pNandTbl->Header.VerGridNum    = pInput->VerGridNum;
                pNandTbl->Header.TileWidthExp  = pInput->TileWidthExp;
                pNandTbl->Header.TileHeightExp = pInput->TileHeightExp;
                pNandTbl->Header.TwoStageDone  = 0U;
                AmbaSvcWrap_MisraMemcpy(&(pNandTbl->Header.VinSensorGeo), &(pInput->VinSensorGeo), sizeof(SVC_CALIB_VIN_SENSOR_GEO_s));
                AmbaSvcWrap_MisraMemcpy(pNandTbl->StLdcVector, pInput->pStLdcVector, sizeof(AMBA_IK_GRID_POINT_s)*(pInput->StLdcVectorLength));
#else
                {
                    AMBA_IK_WARP_INFO_s InputInfo, ResultInfo;
                    AMBA_IK_IN_WARP_PRE_PROC_s TwoStageProcIn;
                    AMBA_IK_OUT_WARP_PRE_PROC_s TwoStageProcOut;

                    AmbaSvcWrap_MisraMemset(&InputInfo, 0, sizeof(InputInfo));
                    InputInfo.Version       = SVC_CALIB_ST_LDC_VERSION;
                    InputInfo.HorGridNum    = pInput->HorGridNum;
                    InputInfo.VerGridNum    = pInput->VerGridNum;
                    InputInfo.TileWidthExp  = pInput->TileWidthExp;
                    InputInfo.TileHeightExp = pInput->TileHeightExp;
                    InputInfo.pWarp         = pInput->pStLdcVector;
                    AmbaSvcWrap_MisraMemcpy(&(InputInfo.VinSensorGeo), &(pInput->VinSensorGeo), sizeof(SVC_CALIB_VIN_SENSOR_GEO_s));

                    TwoStageProcIn.pInputInfo        = &InputInfo;
                    TwoStageProcIn.pWorkingBuffer    = SVC_CalibStitchLdcCtrl.MemCtrl[SVC_CALIB_ST_LDC_MEM_CALC_REMAP].pBuf;
                    TwoStageProcIn.WorkingBufferSize = SVC_CalibStitchLdcCtrl.MemCtrl[SVC_CALIB_ST_LDC_MEM_CALC_REMAP].BufSize;

                    AmbaSvcWrap_MisraMemset(&ResultInfo, 0, sizeof(ResultInfo));
                    ResultInfo.pWarp = pNandTbl->StLdcVector;
                    AmbaSvcWrap_MisraMemset(&TwoStageProcOut, 0, sizeof(TwoStageProcOut));
                    TwoStageProcOut.pResultInfo = &ResultInfo;

                    PRetVal = AmbaIK_PreProcWarpTbl(&TwoStageProcIn, &TwoStageProcOut);
                    if (PRetVal != 0U) {
                        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to calc stitch ldc table - two stage remap fail! ErrCode(0x%08x)", PRetVal, 0U);
                        RetVal = SVC_NG;
                    } else {
                        SvcLog_DBG(SVC_LOG_ST_LDC, "Two stage re-map done!", 0U, 0U);

                        pNandTbl->Header.Version                           = ResultInfo.Version;
                        pNandTbl->Header.HorGridNum                        = ResultInfo.HorGridNum;
                        pNandTbl->Header.VerGridNum                        = ResultInfo.VerGridNum;
                        pNandTbl->Header.TileWidthExp                      = ResultInfo.TileWidthExp;
                        pNandTbl->Header.TileHeightExp                     = ResultInfo.TileHeightExp;
                        pNandTbl->Header.TwoStageDone                      = 1U;
                        AmbaSvcWrap_MisraMemcpy(&(pNandTbl->Header.VinSensorGeo), &(ResultInfo.VinSensorGeo), sizeof(SVC_CALIB_VIN_SENSOR_GEO_s));
                    }
                }
#endif

                pNandTbl->Header.VinSelectBits    = pTblInfo->CalChan.VinSelectBits;
                pNandTbl->Header.SensorSelectBits = pTblInfo->CalChan.SensorSelectBits;

                // Update nand header
                if (SVC_CalibStitchLdcCtrl.pNandHeader->Enable == 0U) {
                    SVC_CalibStitchLdcCtrl.pNandHeader->Enable = 1U;
                }
                if (SVC_CalibStitchLdcCtrl.pNandHeader->DebugMsgOn == 0U) {
                    SVC_CalibStitchLdcCtrl.pNandHeader->DebugMsgOn = SVC_CalibStitchLdcCtrl.EnableDebugMsg;
                }
                if (SVC_CalibStitchLdcCtrl.pNandHeader->DebugMsgOn == 0U) {
                    SVC_CalibStitchLdcCtrl.pNandHeader->DebugMsgOn = pTblInfo->DbgMsgOn;
                }
                if (SVC_CalibStitchLdcCtrl.EnableDebugMsg == 0U) {
                    SVC_CalibStitchLdcCtrl.EnableDebugMsg = pTblInfo->DbgMsgOn;
                }
                if (IsNewData > 0U) {
                    SVC_CalibStitchLdcCtrl.pNandHeader->NumOfTable += 1U;
                }

                /* Update to NAND */
                RetVal = SvcCalib_DataSave(SVC_CALIB_STITCH_LDC_ID);
                if (RetVal != OK) {
                    SvcLog_NG(SVC_LOG_ST_LDC, "Failure to save stitch ldc to nand!", 0U, 0U);
                }

                AmbaMisra_TouchUnused(pInput);
                AmbaMisra_TouchUnused(pTblInfo);
            }

            PRetVal = AmbaKAL_MutexGive(&(SVC_CalibStitchLdcCtrl.Mutex)); AmbaMisra_TouchUnused(&PRetVal);

            SvcCalib_StLdcMemUnLock();
        }
    }

    return RetVal;
}

static UINT32 SvcCalib_StLdcShellCfgDbgMsg(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibStitchLdcCtrlFlag & SVC_CALIB_ST_LDC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to switch stitch ldc debug msg - initial stitch ldc module first!", 0U, 0U);
    }

    if (ArgCount < 3U) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to switch stitch ldc debug msg - Argc should >= 3", 0U, 0U);
    }

    if (pArgVector == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to switch stitch ldc debug msg - Argv should not null!", 0U, 0U);
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 DbgMsgOn = 0U;

        PRetVal = SvcWrap_strtoul(pArgVector[3U],  &DbgMsgOn ); AmbaMisra_TouchUnused(&PRetVal);

        SVC_CalibStitchLdcCtrl.EnableDebugMsg = DbgMsgOn;

        SvcLog_DBG(SVC_LOG_ST_LDC, "", 0U, 0U);
        SvcLog_DBG(SVC_LOG_ST_LDC, "------ Calibration Stitch LDC Debug Msg On/Off %d ------", SVC_CalibStitchLdcCtrl.EnableDebugMsg, 0U);

    } else {
        AmbaMisra_TouchUnused(&ArgCount);
    }

    return RetVal;
}

static void SvcCalib_StLdcShellCfgDbgMsgU(void)
{
    SVC_WRAP_PRINT "  %scfg_dbg_msg%s           :"
        SVC_PRN_ARG_S SVC_LOG_ST_LDC
        SVC_PRN_ARG_CSTR SVC_LOG_CAL_HL_TITLE_1 SVC_PRN_ARG_POST
        SVC_PRN_ARG_CSTR SVC_LOG_CAL_HL_END     SVC_PRN_ARG_POST
        SVC_PRN_ARG_E
    SvcLog_DBG(SVC_LOG_ST_LDC, "  ------------------------------------------------------", 0U, 0U);
    SvcLog_DBG(SVC_LOG_ST_LDC, "    [Enable]            : 0: disable debug message", 0U, 0U);
    SvcLog_DBG(SVC_LOG_ST_LDC, "                        : 1: enable debug message", 0U, 0U);
    SvcLog_DBG(SVC_LOG_ST_LDC, " ", 0U, 0U);
}

static UINT32 SvcCalib_StLdcShellEnable(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibStitchLdcCtrlFlag & SVC_CALIB_ST_LDC_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to enable/disable stitch ldc - initial stitch ldc module first!", 0U, 0U);
    }

    if (ArgCount <= 4U) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to enable/disable stitch ldc - Argc should > 4", 0U, 0U);
    }

    if (pArgVector == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to enable/disable stitch ldc - Argv should not null!", 0U, 0U);
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 IkContextID = 0xFFFFFFFFU, Enable = 0U;

        PRetVal = SvcWrap_strtoul(pArgVector[3U],  &IkContextID ); AmbaMisra_TouchUnused(&PRetVal);
        PRetVal = SvcWrap_strtoul(pArgVector[4U],  &Enable      ); AmbaMisra_TouchUnused(&PRetVal);
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
                SvcLog_NG(SVC_LOG_ST_LDC, "Fail to enable/disable stitch ldc - get stitch ldc enable/disable state fail", 0U, 0U);
            } else {
                if (CurIkState != Enable) {
                    PRetVal = AmbaIK_SetWarpEnb(&ImgMode, Enable);
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;
                        SVC_WRAP_PRINT "Fail to enable/disable stitch ldc - %s stitch ldc fail!"
                            SVC_PRN_ARG_S SVC_LOG_ST_LDC
                            SVC_PRN_ARG_CSTR ( (Enable > 0U) ? ( "enable" ) : ( "disable" ) ) SVC_PRN_ARG_POST
                            SVC_PRN_ARG_E
                    } else {
                        SVC_WRAP_PRINT "%s stitch ldc done!"
                            SVC_PRN_ARG_S SVC_LOG_ST_LDC
                            SVC_PRN_ARG_CSTR ( (Enable > 0U) ? ( "enable" ) : ( "disable" ) ) SVC_PRN_ARG_POST
                            SVC_PRN_ARG_E
                    }
                } else {
                    SVC_WRAP_PRINT "current IK Stitch LDC state: %s!"
                        SVC_PRN_ARG_S SVC_LOG_ST_LDC
                        SVC_PRN_ARG_CSTR ( (CurIkState > 0U) ? ( "enable" ) : ( "disable" ) ) SVC_PRN_ARG_POST
                        SVC_PRN_ARG_E
                }
            }
        } else {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ST_LDC, "Fail to enable/disable stitch ldc - invalid ik context id(%d)", IkContextID, 0U);
        }
    } else {
        AmbaMisra_TouchUnused(&ArgCount);
    }

    return RetVal;
}

static void SvcCalib_StLdcShellEnableU(void)
{
    SVC_WRAP_PRINT "  %senable%s                : enable/disable Stitch LDC effect"
        SVC_PRN_ARG_S SVC_LOG_ST_LDC
        SVC_PRN_ARG_CSTR SVC_LOG_CAL_HL_TITLE_1 SVC_PRN_ARG_POST
        SVC_PRN_ARG_CSTR SVC_LOG_CAL_HL_END     SVC_PRN_ARG_POST
        SVC_PRN_ARG_E
    SvcLog_DBG(SVC_LOG_ST_LDC, "  ------------------------------------------------------", 0U, 0U);
    SvcLog_DBG(SVC_LOG_ST_LDC, "    [IK Context ID]     : image kernel context id.", 0U, 0U);
    SvcLog_DBG(SVC_LOG_ST_LDC, "    [Enable]            : 0 => disable", 0U, 0U);
    SvcLog_DBG(SVC_LOG_ST_LDC, "                        : 1 => enable", 0U, 0U);
    SvcLog_DBG(SVC_LOG_ST_LDC, " ", 0U, 0U);
}

static void SvcCalib_StLdcShellEntryInit(void)
{
    if ((SVC_CalibStitchLdcCtrlFlag & SVC_CALIB_ST_LDC_FLG_SHELLINIT) == 0U) {

        AmbaSvcWrap_MisraMemset(CalibStitchLdcShellFunc, 0, sizeof(CalibStitchLdcShellFunc));

        CalibStitchLdcShellFunc[0U] = (SVC_CALIB_ST_LDC_SHELL_FUNC_s) { 1U, "cfg_dbg_msg",     SvcCalib_StLdcShellCfgDbgMsg,  SvcCalib_StLdcShellCfgDbgMsgU  };
        CalibStitchLdcShellFunc[1U] = (SVC_CALIB_ST_LDC_SHELL_FUNC_s) { 1U, "enable",          SvcCalib_StLdcShellEnable,     SvcCalib_StLdcShellEnableU     };

        SVC_CalibStitchLdcCtrlFlag |= SVC_CALIB_ST_LDC_FLG_SHELLINIT;
    }
}

UINT32 SvcCalib_StitchLdcShellFunc(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_NG, PRetVal;

    SvcCalib_StLdcShellEntryInit();

    if (ArgCount > 0U) {
        UINT32 ShellIdx;
        UINT32 ShellCount = (UINT32)(sizeof(CalibStitchLdcShellFunc)) / (UINT32)(sizeof(CalibStitchLdcShellFunc[0]));

        for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
            if ((CalibStitchLdcShellFunc[ShellIdx].pFunc != NULL) && (CalibStitchLdcShellFunc[ShellIdx].Enable > 0U)) {
                if (0 == SvcWrap_strcmp(pArgVector[2U], CalibStitchLdcShellFunc[ShellIdx].ShellCmdName)) {
                    PRetVal = (CalibStitchLdcShellFunc[ShellIdx].pFunc)(ArgCount, pArgVector);
                    AmbaMisra_TouchUnused(&PRetVal);

                    RetVal = SVC_OK;
                    break;
                }
            }
        }

        if (RetVal != SVC_OK) {
            SvcCalib_StLdcShellUsage();
        }
    }

    return RetVal;
}

static void SvcCalib_StLdcShellUsage(void)
{
    UINT32 ShellIdx;
    UINT32 ShellCount = (UINT32)(sizeof(CalibStitchLdcShellFunc)) / (UINT32)(sizeof(CalibStitchLdcShellFunc[0]));

    SvcLog_DBG(SVC_LOG_ST_LDC, "", 0U, 0U);
    SvcLog_DBG(SVC_LOG_ST_LDC, "====== Stitch LDC Command Usage ======", 0U, 0U);

    for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
        if ((CalibStitchLdcShellFunc[ShellIdx].pFunc != NULL) && (CalibStitchLdcShellFunc[ShellIdx].Enable > 0U)) {
            if (CalibStitchLdcShellFunc[ShellIdx].pUsageFunc == NULL) {
                SVC_WRAP_PRINT "  %s"
                    SVC_PRN_ARG_S SVC_LOG_ST_LDC
                    SVC_PRN_ARG_CSTR CalibStitchLdcShellFunc[ShellIdx].ShellCmdName SVC_PRN_ARG_POST
                    SVC_PRN_ARG_E
            } else {
                (CalibStitchLdcShellFunc[ShellIdx].pUsageFunc)();
            }
        }
    }
}

static void SvcCalib_StLdcCmdHdlrInit(void)
{
    if ((SVC_CalibStitchLdcCtrlFlag & SVC_CALIB_ST_LDC_FLG_CMD_INIT) == 0U) {

        AmbaSvcWrap_MisraMemset(SvcCalibStitchLdcCmdHdlr, 0, sizeof(SvcCalibStitchLdcCmdHdlr));

        SvcCalibStitchLdcCmdHdlr[SVC_CALIB_CMD_MEM_QUERY      ] = (SVC_CALIB_ST_LDC_CMD_HDLR_s) {1, SVC_CALIB_CMD_MEM_QUERY,       SvcCalib_StLdcCmdMemQuery   };
        SvcCalibStitchLdcCmdHdlr[SVC_CALIB_CMD_DATA_LOAD      ] = (SVC_CALIB_ST_LDC_CMD_HDLR_s) {1, SVC_CALIB_CMD_DATA_LOAD,       NULL                        };
        SvcCalibStitchLdcCmdHdlr[SVC_CALIB_CMD_ITEM_DATA_INIT ] = (SVC_CALIB_ST_LDC_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_DATA_INIT,  SvcCalib_StLdcCmdInit       };
        SvcCalibStitchLdcCmdHdlr[SVC_CALIB_CMD_ITEM_TBL_CFG   ] = (SVC_CALIB_ST_LDC_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_TBL_CFG,    NULL                        };
        SvcCalibStitchLdcCmdHdlr[SVC_CALIB_CMD_ITEM_TBL_UPDATE] = (SVC_CALIB_ST_LDC_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_TBL_UPDATE, SvcCalib_StLdcCmdTblUpdate  };
        SvcCalibStitchLdcCmdHdlr[SVC_CALIB_CMD_ITEM_TBL_GET   ] = (SVC_CALIB_ST_LDC_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_TBL_GET,    NULL                        };
        SvcCalibStitchLdcCmdHdlr[SVC_CALIB_CMD_ITEM_TBL_SET   ] = (SVC_CALIB_ST_LDC_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_TBL_SET,    SvcCalib_StLdcCmdTblDataSet };

        SVC_CalibStitchLdcCtrlFlag |= SVC_CALIB_ST_LDC_FLG_CMD_INIT;
    }
}

static UINT32 SvcCalib_StLdcCmdMemQuery(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    if (pParam1 == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to query stitch ldc memory - input stitch ldc table number should not zero.", 0U, 0U);
    }

    if (pParam2 == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to query stitch ldc memory - output stitch ldc shadow buffer size should not null!", 0U, 0U);
    }

    if (pParam3 == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to query stitch ldc memory - output stitch ldc working buffer size should not null!", 0U, 0U);
    }

    if (pParam4 == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to query stitch ldc memory - stitch ldc calculation buffer size should not null!", 0U, 0U);
    }

    if (RetVal == SVC_OK) {
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

        RetVal = SvcCalib_StLdcMemQuery(NomOfTable, pShadowBufSize, pWorkingBufSize, pCalcMemSize);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_ST_LDC, "Fail to query stitch ldc memory - query memory fail!", 0U, 0U);
        }

        AmbaMisra_TouchUnused(pParam1);
        AmbaMisra_TouchUnused(pParam2);
        AmbaMisra_TouchUnused(pParam3);
        AmbaMisra_TouchUnused(pParam4);
        AmbaMisra_TouchUnused(pU32Val);
    } else {
        AmbaMisra_TouchUnused(SvcCalibStitchLdcCmdHdlr);
    }

    return RetVal;
}

static UINT32 SvcCalib_StLdcCmdInit(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    if (pParam1 == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to init stitch ldc - input calib object should not null!", 0U, 0U);
    }

    if (RetVal == SVC_OK) {
        RetVal = SvcCalib_StLdcCreate();
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_ST_LDC, "Fail to init stitch ldc - create module fail!", 0U, 0U);
        }
    }

    if (RetVal == SVC_OK) {
        const SVC_CALIB_OBJ_s *pCalObj;

        AmbaMisra_TypeCast(&pCalObj, &pParam1);
        if (pCalObj == NULL) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ST_LDC, "Fail to init stitch ldc - invalid calib obj!", 0U, 0U);
        } else {
            RetVal = SvcCalib_StLdcShadowInit(pCalObj);
            if (RetVal != SVC_OK) {
                SvcLog_NG(SVC_LOG_ST_LDC, "Fail to init stitch ldc - shadow initial fail!", 0U, 0U);
            } else {
                SVC_CALIB_CALC_CB_f CbFunc = NULL;
                AmbaMisra_TypeCast(&(CbFunc), &(pParam2));
                // SVC_CalibStitchBlendCtrl.CalcCbFunc = CbFunc;
            }
        }

        AmbaMisra_TouchUnused(pParam1);
        AmbaMisra_TouchUnused(pParam2);
        AmbaMisra_TouchUnused(pParam3);
        AmbaMisra_TouchUnused(pParam4);
    }

    return RetVal;
}

static UINT32 SvcCalib_StLdcCmdTblUpdate(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    if (pParam1 == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to update stitch ldc table - input calib channel should not null!", 0U, 0U);
    }

    if (pParam2 == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to update stitch ldc table - input IK mode cfg should not null!", 0U, 0U);
    }

    if (RetVal == SVC_OK) {
        const SVC_CALIB_CHANNEL_s     *pChan;
        const AMBA_IK_MODE_CFG_s      *pImgMode;

        AmbaMisra_TypeCast(&(pChan), &pParam1);
        AmbaMisra_TypeCast(&(pImgMode), &pParam2);

        RetVal = SvcCalib_StLdcTableUpdate(pChan, pImgMode);

        AmbaMisra_TouchUnused(pParam1);
        AmbaMisra_TouchUnused(pParam2);
        AmbaMisra_TouchUnused(pParam3);
        AmbaMisra_TouchUnused(pParam4);
    }

    return RetVal;
}

static UINT32 SvcCalib_StLdcCmdTblDataSet(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;
    UINT32 *pU32Val = NULL;
    SVC_CALIB_TBL_INFO_s *pTblInfo;

    AmbaMisra_TypeCast(&(pU32Val), &(pParam1));
    AmbaMisra_TypeCast(&(pTblInfo), &(pParam2));

    if (pU32Val == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to set table data - input table id should not null!", 0U, 0U);
    } else {
        RetVal = SvcCalib_StLdcTableDataSet(*pU32Val, pTblInfo);
    }

    AmbaMisra_TouchUnused(pParam1);
    AmbaMisra_TouchUnused(pParam2);
    AmbaMisra_TouchUnused(pParam3);
    AmbaMisra_TouchUnused(pParam4);
    AmbaMisra_TouchUnused(pU32Val);

    return RetVal;


}

/**
 * calib stitch ldc command entry
 *
 * @param [in] CmdID  the stitch ldc command id
 * @param [in] Param1 no.01 parameter
 * @param [in] Param2 no.02 parameter
 * @param [in] Param3 no.03 parameter
 * @param [in] Param4 no.04 parameter
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcCalib_StitchLdcCmdFunc(UINT32 CmdID, void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    SvcCalib_StLdcCmdHdlrInit();

    if (CmdID >= SVC_CALIB_CMD_NUM) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_LDC, "Fail to handler cmd - invalid command id(%d)", CmdID, 0U);
    } else {
        const UINT32 HdlrCount = (UINT32)(sizeof(SvcCalibStitchLdcCmdHdlr)) / ((UINT32)sizeof(SvcCalibStitchLdcCmdHdlr[0]));
        UINT32 HdlrIdx;

        for (HdlrIdx = 0U; HdlrIdx < HdlrCount; HdlrIdx ++) {
            if ((SvcCalibStitchLdcCmdHdlr[HdlrIdx].Enable > 0U) &&
                (SvcCalibStitchLdcCmdHdlr[HdlrIdx].CmdID == CmdID) &&
                (SvcCalibStitchLdcCmdHdlr[HdlrIdx].pHandler != NULL)) {
                RetVal = (SvcCalibStitchLdcCmdHdlr[HdlrIdx].pHandler)(pParam1, pParam2, pParam3, pParam4);
            }
        }
    }

    return RetVal;
}
