/**
 *  @file SvcCalibStitchBlend.c
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
 *  @details svc calibration stitch blend
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaVIN_Def.h"
#include "AmbaDSP_ImageUtility.h"

#include "SvcWrap.h"
#include "SvcCalibMgr.h"

#include "SvcErrCode.h"
#include "SvcLog.h"

#include "SvcCalibStitch.h"

#define SVC_CALIB_ST_BLD_FLG_INIT      (0x1U)
#define SVC_CALIB_ST_BLD_FLG_SHDW_INIT (0x2U)
// #define SVC_CALIB_ST_BLD_FLG_CALC_INIT (0x4U)
// #define SVC_CALIB_ST_BLD_FLG_CALC_CHN  (0x8U)
#define SVC_CALIB_ST_BLD_FLG_DBG_MSG   (0x100U)
#define SVC_CALIB_ST_BLD_FLG_SHELLINIT (0x1000U)
#define SVC_CALIB_ST_BLD_FLG_CMD_INIT  (0x2000U)

#define SVC_CALIB_ST_BLD_NAME          ("SvcCalib_StBlend")
#define SVC_CALIB_ST_BLD_NAND_HDR_SIZE (0x200U)
// #define SVC_CALIB_ST_BLD_VERSION       (0x20180401U)

typedef struct {
    UINT32 Enable;
    UINT32 NumOfTable;
    UINT32 DebugMsgOn;
    UINT32 Reserved[(SVC_CALIB_ST_BLD_NAND_HDR_SIZE / 4U) - 3U];
} SVC_CALIB_ST_BLD_NAND_HEADER_s;

typedef struct {
    UINT32                     Enable;
    UINT32                     Width;
    UINT32                     Height;
    UINT32                     TableLength;
    UINT32                     VinSelectBits;
    UINT32                     SensorSelectBits;
} SVC_CALIB_ST_BLD_TBL_HEADER_s;

typedef struct {
    SVC_CALIB_ST_BLD_TBL_HEADER_s Header;
    UINT8                         Table[SVC_CALIB_STITCH_BLEND_TBL_SZ];
} SVC_CALIB_ST_BLD_NAND_TBL_s;

typedef struct {
    UINT32                        VinSelectBits;
    UINT32                        SensorSelectBits;
} SVC_CALIB_ST_BLD_TBL_s;

// typedef struct {
//     UINT32                          TableIdx;
// } SVC_CALIB_ST_BLD_CALC_CTRL_s;

typedef struct {
    char                            Name[16];
    AMBA_KAL_MUTEX_t                Mutex;
    SVC_CALIB_ST_BLD_NAND_HEADER_s *pNandHeader;
    SVC_CALIB_ST_BLD_NAND_TBL_s    *pNandTable;
    UINT32                          NumOfTable;
    SVC_CALIB_ST_BLD_TBL_s         *pStBlendTable;

    // SVC_CALIB_ST_BLD_CALC_CTRL_s    CalcBlendCtrl;
    // SVC_CALIB_CHANNEL_s             CalibChan;
    UINT32                          EnableDebugMsg;
} SVC_CALIB_ST_BLD_CTRL_s;

static UINT32                       SvcCalib_StBlendMemQuery(UINT32 MaxTableNum, UINT32 *pShadowSize, UINT32 *pWorkMemSize, UINT32 *pCalcMemSize);
static UINT32                       SvcCalib_StBlendVinSensorIDCheck(UINT32 VinID, UINT32 SensorID);
static UINT32                       SvcCalib_StBlendCreate(void);
static UINT32                       SvcCalib_StBlendShadowInit(const SVC_CALIB_OBJ_s *pCalObj);
static UINT32                       SvcCalib_StBlendNandTableSearch(UINT32 VinID, UINT32 SensorID, UINT32 *pTblIdx);
static UINT32                       SvcCalib_StBlendTableDataGet(UINT32 TblID, const SVC_CALIB_TBL_INFO_s *pTblInfo);
static UINT32                       SvcCalib_StBlendTableDataSet(UINT32 TblID, const SVC_CALIB_TBL_INFO_s *pTblInfo);

// Private function definition for calibration command
typedef UINT32 (*SVC_CALIB_ST_BLD_CMD_FUNC_f)(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
typedef struct {
     UINT32                      Enable;
     UINT32                      CmdID;
     SVC_CALIB_ST_BLD_CMD_FUNC_f pHandler;
} SVC_CALIB_ST_BLD_CMD_HDLR_s;

static void   SvcCalib_StBlendCmdHdlrInit(void);
static UINT32 SvcCalib_StBlendCmdMemQuery(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_StBlendCmdInit(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_StBlendCmdTblDataGet(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_StBlendCmdTblDataSet(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
//UINT32 SvcCalib_StitchBlendCmdFunc(UINT32 CmdID, void *pParam1, void *pParam2, void *pParam3, void *pParam4);

// Private function definition for shell command.
typedef UINT32 (*SVC_CALIB_ST_BLD_SHELL_FUNC_f)(UINT32 ArgCount, char * const *pArgVector);
typedef void   (*SVC_CALIB_ST_BLD_SHELL_USAGE_f)(void);

typedef struct {
    UINT32                         Enable;
    char                           ShellCmdName[32];
    SVC_CALIB_ST_BLD_SHELL_FUNC_f  pFunc;
    SVC_CALIB_ST_BLD_SHELL_USAGE_f pUsageFunc;
} SVC_CALIB_ST_BLD_SHELL_FUNC_s;

static UINT32 SvcCalib_StBlendShellCfgDbgMsg(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_StBlendShellCfgDbgMsgU(void);
static void   SvcCalib_StBlendShellEntryInit(void);
static void   SvcCalib_StBlendShellUsage(void);

#define SVC_CALIB_ST_BLD_SHELL_CMD_NUM  (1U)
static SVC_CALIB_ST_BLD_SHELL_FUNC_s CalibStitchBlendShellFunc[SVC_CALIB_ST_BLD_SHELL_CMD_NUM] GNU_SECTION_NOZEROINIT;
static SVC_CALIB_ST_BLD_CMD_HDLR_s SvcCalibStitchBlendCmdHdlr[SVC_CALIB_CMD_NUM] GNU_SECTION_NOZEROINIT;
static SVC_CALIB_ST_BLD_CTRL_s SVC_CalibStitchBlendCtrl GNU_SECTION_NOZEROINIT;

#ifdef SVC_CALIB_DBG_MSG_ON
static UINT32 SVC_CalibStitchBlendCtrlFlag = SVC_LOG_CAL_DEF_FLG | SVC_LOG_CAL_DBG;
#else
static UINT32 SVC_CalibStitchBlendCtrlFlag = SVC_LOG_CAL_DEF_FLG;
#endif

#define SVC_LOG_ST_BLD "CAL_STBLD"

static UINT32 SvcCalib_StBlendMemQuery(UINT32 MaxTableNum, UINT32 *pShadowSize, UINT32 *pWorkMemSize, UINT32 *pCalcMemSize)
{
    UINT32 RetVal = SVC_OK;

    UINT32 DramShadowSize, WorkingMemSize, CalcMemSize = 0;
    UINT32 DramShadowHeaderSize = (UINT32)(sizeof(SVC_CALIB_ST_BLD_NAND_HEADER_s));
    UINT32 DramShadowTableSize  = (UINT32)(sizeof(SVC_CALIB_ST_BLD_NAND_TBL_s));
    UINT32 DramShadowDataSize   = DramShadowTableSize * MaxTableNum;
    UINT32 TableSize            = (UINT32)(sizeof(SVC_CALIB_ST_BLD_TBL_s));
    UINT32 TotalTableSize       = TableSize * MaxTableNum;

    DramShadowSize  = DramShadowHeaderSize;
    DramShadowSize += DramShadowDataSize;
    WorkingMemSize  = TotalTableSize;

    if (pShadowSize != NULL) {
        *pShadowSize  = DramShadowSize;
    }
    SvcLog_DBG(SVC_LOG_ST_BLD, "Query the calib stitch blend dram shadow size 0x%X ( table num %d )", DramShadowSize, MaxTableNum);

    if (pWorkMemSize != NULL) {
        *pWorkMemSize = WorkingMemSize;
    }
    SvcLog_DBG(SVC_LOG_ST_BLD, "Query the calib stitch blend working memory size 0x%X ( table num %d )", WorkingMemSize, MaxTableNum);

    if (pCalcMemSize != NULL) {
        *pCalcMemSize = CalcMemSize;
    }
    SvcLog_DBG(SVC_LOG_ST_BLD, "Query the calib stitch blend algo. memory size 0x%x ( table num %d )", CalcMemSize, MaxTableNum);

    return RetVal;
}

static UINT32 SvcCalib_StBlendVinSensorIDCheck(UINT32 VinID, UINT32 SensorID)
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

static UINT32 SvcCalib_StBlendCreate(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibStitchBlendCtrlFlag & SVC_CALIB_ST_BLD_FLG_INIT) > 0U) {
        RetVal = SVC_OK;
        SvcLog_DBG(SVC_LOG_ST_BLD, "Calibration Stitch Blend module has been created!", 0U, 0U);
    } else {
        // Reset the stitch blend control module
        AmbaSvcWrap_MisraMemset(&SVC_CalibStitchBlendCtrl, 0, sizeof(SVC_CalibStitchBlendCtrl));

        // Configure the name
        SvcWrap_strcpy(SVC_CalibStitchBlendCtrl.Name, sizeof(SVC_CalibStitchBlendCtrl.Name), SVC_CALIB_ST_BLD_NAME);

        // Create the mutex
        RetVal = AmbaKAL_MutexCreate(&(SVC_CalibStitchBlendCtrl.Mutex), SVC_CalibStitchBlendCtrl.Name);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_ST_BLD, "Fail to create stitch blend - create mutex fail!", 0U, 0U);
        } else {
            PRetVal = (SVC_CalibStitchBlendCtrlFlag & (SVC_CALIB_ST_BLD_FLG_DBG_MSG | SVC_CALIB_ST_BLD_FLG_SHELLINIT | SVC_CALIB_ST_BLD_FLG_CMD_INIT));
            SVC_CalibStitchBlendCtrlFlag  = PRetVal;
            SVC_CalibStitchBlendCtrlFlag |= SVC_CALIB_ST_BLD_FLG_INIT;
            SvcLog_OK(SVC_LOG_ST_BLD, "Successful to create blend!", 0U, 0U);
        }
    }

    return RetVal;
}

static UINT32 SvcCalib_StBlendShadowInit(const SVC_CALIB_OBJ_s *pCalObj)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibStitchBlendCtrlFlag & SVC_CALIB_ST_BLD_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_BLD, "Fail to initial stitch blend dram shodow - create stitch blend first!", 0U, 0U);
    } else if (pCalObj == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_BLD, "Fail to initial stitch blend dram shodow - invalid calib object!", 0U, 0U);
    } else if (pCalObj->Enable == 0U) {
        RetVal = 99;
        SvcLog_DBG(SVC_LOG_ST_BLD, "Disable stitch blend dram shadow!!", 0U, 0U);
    } else {
        UINT32 DramShadowSize = 0, WorkingMemSize = 0;
        const SVC_CALIB_ST_BLD_NAND_HEADER_s *pHeader;

        AmbaMisra_TypeCast(&(pHeader), &(pCalObj->pShadowBuf));

        RetVal = SvcCalib_StBlendMemQuery(pCalObj->NumOfTable, &DramShadowSize, &WorkingMemSize, NULL);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_ST_BLD, "Fail to initial stitch blend dram shadow - query memory fail!", 0U, 0U);
        } else if ((pCalObj->pShadowBuf == NULL) || (pHeader == NULL)) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ST_BLD, "Fail to initial stitch blend dram shadow - dram shadow buffer should not null!", 0U, 0U);
        } else if (pCalObj->ShadowBufSize < DramShadowSize) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ST_BLD, "Fail to initial stitch blend dram shadow - prepare dram shadow size is too small!", 0U, 0U);
        } else if (pCalObj->pWorkingBuf == NULL) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ST_BLD, "Fail to initial stitch blend dram shadow - working buffer should not null!", 0U, 0U);
        } else if (pCalObj->WorkingBufSize < WorkingMemSize) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ST_BLD, "Fail to initial stitch blend dram shadow - working buffer size is too small!", 0U, 0U);
        } else if (pHeader->NumOfTable > pCalObj->NumOfTable) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ST_BLD, "Fail to initial stitch blend dram shadow - calib nand table count(%d) > max stitch blend dram shadow table count(%d)",
                    pHeader->NumOfTable, pCalObj->NumOfTable);
        } else {
            if (KAL_ERR_NONE != AmbaKAL_MutexTake(&(SVC_CalibStitchBlendCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ST_BLD, "Fail to initial stitch blend dram shadow - take mutex fail!", 0U, 0U);
            } else {
                UINT8 *pNandHeaderBuf    = pCalObj->pShadowBuf;
                UINT32 NandHeaderSize    = (UINT32)(sizeof(SVC_CALIB_ST_BLD_NAND_HEADER_s));
                UINT8 *pNandTableBuf     = &(pNandHeaderBuf[NandHeaderSize]);
                UINT32 TotalBlendTableSize = sizeof(SVC_CALIB_ST_BLD_TBL_s) * pCalObj->NumOfTable;

                SvcLog_DBG(SVC_LOG_ST_BLD, "====== Shadow initialized Start ======", 0U, 0U);
                SVC_WRAP_PRINT "  DramShadow Addr  : %p 0x%x"
                    SVC_PRN_ARG_S SVC_LOG_ST_BLD
                    SVC_PRN_ARG_CPOINT pCalObj->pShadowBuf    SVC_PRN_ARG_POST
                    SVC_PRN_ARG_UINT32 pCalObj->ShadowBufSize SVC_PRN_ARG_POST
                    SVC_PRN_ARG_E
                SVC_WRAP_PRINT "    Nand Header Addr : %p 0x%x"
                    SVC_PRN_ARG_S SVC_LOG_ST_BLD
                    SVC_PRN_ARG_CPOINT pNandHeaderBuf SVC_PRN_ARG_POST
                    SVC_PRN_ARG_UINT32 NandHeaderSize SVC_PRN_ARG_POST
                    SVC_PRN_ARG_E
                SVC_WRAP_PRINT "    Nand Table Addr  : %p"
                    SVC_PRN_ARG_S SVC_LOG_ST_BLD
                    SVC_PRN_ARG_CPOINT pNandTableBuf SVC_PRN_ARG_POST
                    SVC_PRN_ARG_E
                SVC_WRAP_PRINT "  Working Mem Addr : %p 0x%x"
                    SVC_PRN_ARG_S SVC_LOG_ST_BLD
                    SVC_PRN_ARG_CPOINT pCalObj->pWorkingBuf    SVC_PRN_ARG_POST
                    SVC_PRN_ARG_UINT32 pCalObj->WorkingBufSize SVC_PRN_ARG_POST
                    SVC_PRN_ARG_E
                SVC_WRAP_PRINT "    Stitch BlendTable Addr : %p 0x%08x"
                    SVC_PRN_ARG_S SVC_LOG_ST_BLD
                    SVC_PRN_ARG_CPOINT pCalObj->pWorkingBuf SVC_PRN_ARG_POST
                    SVC_PRN_ARG_UINT32 TotalBlendTableSize  SVC_PRN_ARG_POST
                    SVC_PRN_ARG_E

                // Configure nand header
                AmbaMisra_TypeCast(&(SVC_CalibStitchBlendCtrl.pNandHeader), &(pNandHeaderBuf));
                // Configure nand table
                AmbaMisra_TypeCast(&(SVC_CalibStitchBlendCtrl.pNandTable),  &(pNandTableBuf)); AmbaMisra_TouchUnused(pNandTableBuf);
                // Configure working stitch blend table
                AmbaMisra_TypeCast(&(SVC_CalibStitchBlendCtrl.pStBlendTable),  &(pCalObj->pWorkingBuf));
                if (SVC_CalibStitchBlendCtrl.pNandHeader == NULL) {
                    RetVal = SVC_NG;
                    SvcLog_NG(SVC_LOG_ST_BLD, "Fail to initial stitch blend dram shadow - nand header should not null!", 0U, 0U);
                } else if (SVC_CalibStitchBlendCtrl.pNandTable == NULL) {
                    RetVal = SVC_NG;
                    SvcLog_NG(SVC_LOG_ST_BLD, "Fail to initial stitch blend dram shadow - nand table should not null!", 0U, 0U);
                } else if (SVC_CalibStitchBlendCtrl.pStBlendTable == NULL) {
                    RetVal = SVC_NG;
                    SvcLog_NG(SVC_LOG_ST_BLD, "Fail to initial stitch blend dram shadow - stitch blend table should not null!", 0U, 0U);
                } else {
                    SVC_CalibStitchBlendCtrl.NumOfTable = pCalObj->NumOfTable;
                    AmbaSvcWrap_MisraMemset(SVC_CalibStitchBlendCtrl.pStBlendTable, 0, TotalBlendTableSize);
                    SVC_CalibStitchBlendCtrl.EnableDebugMsg = SVC_CalibStitchBlendCtrl.pNandHeader->DebugMsgOn;
                    if (SVC_CalibStitchBlendCtrl.EnableDebugMsg > 0U) {
                        UINT32 Idx;

                        SvcLog_DBG(SVC_LOG_ST_BLD, "", 0U, 0U);
                        SvcLog_DBG(SVC_LOG_ST_BLD, "---- Stitch BlendNand Info %d ----", SVC_CalibStitchBlendCtrl.pNandHeader->Reserved[0], 0U);
                        SvcLog_DBG(SVC_LOG_ST_BLD, "  Enable         : %d", SVC_CalibStitchBlendCtrl.pNandHeader->Enable, 0U);
                        SvcLog_DBG(SVC_LOG_ST_BLD, "  NumOfTable     : %d", SVC_CalibStitchBlendCtrl.pNandHeader->NumOfTable, 0U);
                        SvcLog_DBG(SVC_LOG_ST_BLD, "  DebugMsgOn     : %d", SVC_CalibStitchBlendCtrl.pNandHeader->DebugMsgOn, 0U);

                        {
                            const SVC_CALIB_ST_BLD_NAND_TBL_s *pNandTable;

                            for (Idx = 0U; Idx < pCalObj->NumOfTable; Idx ++) {
                                pNandTable = &(SVC_CalibStitchBlendCtrl.pNandTable[Idx]);

                                SVC_WRAP_PRINT "  ---- Nand Table[%d] %p -----"
                                    SVC_PRN_ARG_S SVC_LOG_ST_BLD
                                    SVC_PRN_ARG_UINT32 Idx        SVC_PRN_ARG_POST
                                    SVC_PRN_ARG_CPOINT pNandTable SVC_PRN_ARG_POST
                                    SVC_PRN_ARG_E
                                SvcLog_DBG(SVC_LOG_ST_BLD, "    VinSelectBits       : 0x%X",   pNandTable->Header.VinSelectBits       , 0U);
                                SvcLog_DBG(SVC_LOG_ST_BLD, "    SensorSelectBits    : 0x%X",   pNandTable->Header.SensorSelectBits    , 0U);
                                SvcLog_DBG(SVC_LOG_ST_BLD, "    Enable              : %d",     pNandTable->Header.Enable              , 0U);
                                SvcLog_DBG(SVC_LOG_ST_BLD, "    Width               : %d",     pNandTable->Header.Width               , 0U);
                                SvcLog_DBG(SVC_LOG_ST_BLD, "    Height              : %d",     pNandTable->Header.Height              , 0U);
                                SVC_WRAP_PRINT "    Table               : %p ( 0x%x)"
                                    SVC_PRN_ARG_S SVC_LOG_ST_BLD
                                    SVC_PRN_ARG_CPOINT pNandTable->Table                   SVC_PRN_ARG_POST
                                    SVC_PRN_ARG_UINT32 (UINT32)(sizeof(pNandTable->Table)) SVC_PRN_ARG_POST
                                    SVC_PRN_ARG_E
                            }
                        }

                        SvcLog_DBG(SVC_LOG_ST_BLD, "", 0U, 0U);
                        SvcLog_DBG(SVC_LOG_ST_BLD, "---- Stitch BlendTable Info ----", 0U, 0U);
                        SvcLog_DBG(SVC_LOG_ST_BLD, "  NumOfTable     : %d", SVC_CalibStitchBlendCtrl.NumOfTable, 0U);

                        if ((SVC_CalibStitchBlendCtrlFlag & 0xCafeU) == 0xCafeU) {
                            const SVC_CALIB_ST_BLD_TBL_s *pStBlendTable;

                            for (Idx = 0U; Idx < SVC_CalibStitchBlendCtrl.NumOfTable; Idx ++) {
                                pStBlendTable = &(SVC_CalibStitchBlendCtrl.pStBlendTable[Idx]);

                                SVC_WRAP_PRINT "  ---- Stitch BlendTable[%d] %p -----"
                                    SVC_PRN_ARG_S SVC_LOG_ST_BLD
                                    SVC_PRN_ARG_UINT32 Idx           SVC_PRN_ARG_POST
                                    SVC_PRN_ARG_CPOINT pStBlendTable SVC_PRN_ARG_POST
                                    SVC_PRN_ARG_E
                                SvcLog_DBG(SVC_LOG_ST_BLD, "    VinSelectBits       : 0x%X",   SVC_CalibStitchBlendCtrl.pStBlendTable[Idx].VinSelectBits       , 0U);
                                SvcLog_DBG(SVC_LOG_ST_BLD, "    SensorSelectBits    : 0x%X",   SVC_CalibStitchBlendCtrl.pStBlendTable[Idx].SensorSelectBits    , 0U);
                            }
                        }
                    }

                    SVC_CalibStitchBlendCtrlFlag |= SVC_CALIB_ST_BLD_FLG_SHDW_INIT;
                    SvcLog_OK(SVC_LOG_ST_BLD, "Successful to initial stitch blend dram shadow!", 0U, 0U);
                }

                PRetVal = AmbaKAL_MutexGive(&(SVC_CalibStitchBlendCtrl.Mutex)); AmbaMisra_TouchUnused(&PRetVal);
            }
        }
    }

    return RetVal;
}

static UINT32 SvcCalib_StBlendNandTableSearch(UINT32 VinID, UINT32 SensorID, UINT32 *pTblIdx)
{
    UINT32 RetVal = SVC_OK;

    if (SVC_OK != SvcCalib_StBlendVinSensorIDCheck(VinID, SensorID)) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_BLD, "Fail to search stitch blend nand table - invalid calib channel -> VinID(%d), SensorID(%d)!", VinID, SensorID);
    } else {
        UINT32 Idx;

        if (pTblIdx != NULL) {
            *pTblIdx = 0xFFFFFFFFU;
        }

        for (Idx = 0U; Idx < SVC_CalibStitchBlendCtrl.NumOfTable; Idx ++) {

            if (((SVC_CalibStitchBlendCtrl.pNandTable[Idx].Header.VinSelectBits & (0x1UL << VinID)) > 0U) &&
                ((SVC_CalibStitchBlendCtrl.pNandTable[Idx].Header.SensorSelectBits & SensorID) > 0U)) {
                if (pTblIdx != NULL) {
                    *pTblIdx = Idx;
                }
                break;
            }
        }
    }

    return RetVal;
}

static UINT32 SvcCalib_StBlendTableDataGet(UINT32 TblID, const SVC_CALIB_TBL_INFO_s *pTblInfo)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibStitchBlendCtrlFlag & SVC_CALIB_ST_BLD_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_BLD, "Fail to get stitch blend table - Need to initial stitch blend control module first!", 0U, 0U);
    } else if ((SVC_CalibStitchBlendCtrlFlag & SVC_CALIB_ST_BLD_FLG_SHDW_INIT) == 0U) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_BLD, "Fail to get stitch blend table - Need to initial stitch blend dram shadow first!", 0U, 0U);
    } else if (pTblInfo == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_BLD, "Fail to get stitch blend table - output table info should not null!", 0U, 0U);
    } else if (pTblInfo->pBuf == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_BLD, "Fail to get stitch blend table - input table buffer should not null!", 0U, 0U);
    } else if (pTblInfo->BufSize < sizeof(SVC_CALIB_ST_BLEND_TBL_DATA_s)) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_BLD, "Fail to get stitch blend table - input table buffer size is too small! 0x%x/0x%x", pTblInfo->BufSize, sizeof(SVC_CALIB_ST_BLEND_TBL_DATA_s));
    } else if (SVC_OK != SvcCalib_StBlendVinSensorIDCheck(pTblInfo->CalChan.VinID, pTblInfo->CalChan.SensorID)) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_BLD, "Fail to get stitch blend table - invalid calib channel -> VinID(%d), SensorID(%d)!", pTblInfo->CalChan.VinID, pTblInfo->CalChan.SensorID);
    } else {

        if (KAL_ERR_NONE != AmbaKAL_MutexTake(&(SVC_CalibStitchBlendCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ST_BLD, "Fail to get stitch blend table - take mutex fail!", 0U, 0U);
        } else {
            const SVC_CALIB_ST_BLD_NAND_TBL_s *pNandTable = NULL;

            if (TblID >= 0xFFU) {
                RetVal = SvcCalib_StBlendNandTableSearch(pTblInfo->CalChan.VinID, pTblInfo->CalChan.SensorID, &TblID);
            }

            if (TblID >= SVC_CalibStitchBlendCtrl.NumOfTable) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ST_BLD, "Fail to get stitch blend table - request table index(%d) is out-of range(0 ~ %d)!", TblID, SVC_CalibStitchBlendCtrl.NumOfTable - 1U);
            } else {
                SVC_CALIB_ST_BLEND_TBL_DATA_s *pOutput = NULL;

                pNandTable = &(SVC_CalibStitchBlendCtrl.pNandTable[TblID]);

                AmbaMisra_TypeCast(&(pOutput), &(pTblInfo->pBuf));

                if (pOutput == NULL) {
                    RetVal = SVC_NG;
                    SvcLog_NG(SVC_LOG_ST_BLD, "Fail to get stitch blend table - invalid output table data!", 0U, 0U);
                } else if (pOutput->pTbl == NULL) {
                    RetVal = SVC_NG;
                    SvcLog_NG(SVC_LOG_ST_BLD, "Fail to get stitch blend table - invalid output table!", 0U, 0U);
                } else if (pNandTable->Header.TableLength > sizeof(pNandTable->Table)) {
                    RetVal = SVC_NG;
                    SvcLog_NG(SVC_LOG_ST_BLD, "Fail to get stitch blend table - invalid table length(0x%x / 0x%x)", pNandTable->Header.TableLength, sizeof(pNandTable->Table));
                } else if (pOutput->TblLength < pNandTable->Header.TableLength) {
                    RetVal = SVC_NG;
                    SvcLog_NG(SVC_LOG_ST_BLD, "Fail to get stitch blend table - output table size is too small to service it! 0x%x < 0x%x", pOutput->TblLength, pNandTable->Header.TableLength);
                } else {

                    pOutput->Width  = pNandTable->Header.Width;
                    pOutput->Height = pNandTable->Header.Height;
                    AmbaSvcWrap_MisraMemset( pOutput->pTbl, 0,                 ( sizeof(UINT8) * pOutput->TblLength ) );
                    AmbaSvcWrap_MisraMemcpy( pOutput->pTbl, pNandTable->Table, ( sizeof(UINT8) * pNandTable->Header.TableLength ) );
                    pOutput->TblLength = pNandTable->Header.TableLength;

                    if ((SVC_CalibStitchBlendCtrl.EnableDebugMsg > 0U) || (pTblInfo->DbgMsgOn > 0U)) {
                        SvcLog_DBG(SVC_LOG_ST_BLD, "", 0U, 0U);
                        SvcLog_DBG(SVC_LOG_ST_BLD, "---- Get Stitch Blend Table ----", 0U, 0U);
                        SvcLog_DBG(SVC_LOG_ST_BLD, "  Width        : %d"    , pOutput->Width    , 0);
                        SvcLog_DBG(SVC_LOG_ST_BLD, "  Height       : %d"    , pOutput->Height   , 0);
                                    SVC_WRAP_PRINT "  Table        : %p" SVC_PRN_ARG_S SVC_LOG_ST_BLD SVC_PRN_ARG_CPOINT pOutput->pTbl SVC_PRN_ARG_POST SVC_PRN_ARG_E
                        SvcLog_DBG(SVC_LOG_ST_BLD, "  Table Length : 0x%x"  , pOutput->TblLength, 0);
                    }

                    if (TblID >= 0xFFU) {
                        SvcLog_OK(SVC_LOG_ST_BLD, "Successful to get the stitch blend table by VinID(%d) SensorID(%d)", pTblInfo->CalChan.VinID, pTblInfo->CalChan.SensorID );
                    } else {
                        SVC_WRAP_PRINT "Success to get stitch blend table %p by TableID(%d)!"
                            SVC_PRN_ARG_S SVC_LOG_ST_BLD
                            SVC_PRN_ARG_CPOINT &(SVC_CalibStitchBlendCtrl.pNandTable[TblID]) SVC_PRN_ARG_POST
                            SVC_PRN_ARG_UINT32 TblID                                         SVC_PRN_ARG_POST
                            SVC_PRN_ARG_E
                    }
                }
            }

            PRetVal = AmbaKAL_MutexGive(&(SVC_CalibStitchBlendCtrl.Mutex)); AmbaMisra_TouchUnused(&PRetVal);
        }
    }

    return RetVal;
}

static UINT32 SvcCalib_StBlendTableDataSet(UINT32 TblID, const SVC_CALIB_TBL_INFO_s *pTblInfo)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibStitchBlendCtrlFlag & SVC_CALIB_ST_BLD_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_BLD, "Fail to set stitch blend table - Need to initial stitch blend control module first!", 0U, 0U);
    } else if ((SVC_CalibStitchBlendCtrlFlag & SVC_CALIB_ST_BLD_FLG_SHDW_INIT) == 0U) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_BLD, "Fail to set stitch blend table - Need to initial stitch blend dram shadow first!", 0U, 0U);
    } else if (TblID >= SVC_CalibStitchBlendCtrl.NumOfTable) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_BLD, "Fail to set stitch blend table - TableID(%d) out-of range(%d)!", TblID, SVC_CalibStitchBlendCtrl.NumOfTable);
    } else if (pTblInfo == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_BLD, "Fail to set stitch blend table - output table info should not null!", 0U, 0U);
    } else if (pTblInfo->pBuf == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_BLD, "Fail to set stitch blend table - input table buffer should not null!", 0U, 0U);
    } else if (pTblInfo->BufSize < sizeof(SVC_CALIB_ST_BLEND_TBL_DATA_s)) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_BLD, "Fail to set stitch blend table - input table buffer size is too small! 0x%x/0x%x", pTblInfo->BufSize, sizeof(SVC_CALIB_ST_BLEND_TBL_DATA_s));
    } else if ((pTblInfo->CalChan.VinSelectBits == 0U) ||
               (pTblInfo->CalChan.SensorSelectBits == 0U)) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_BLD, "Fail to set stitch blend table - invalid calib channel!", 0U, 0U);
    } else {

        if (KAL_ERR_NONE != AmbaKAL_MutexTake(&(SVC_CalibStitchBlendCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ST_BLD, "Fail to set stitch blend table - take mutex fail!", 0U, 0U);
        } else {
            SVC_CALIB_ST_BLD_NAND_TBL_s *pNandTable = &(SVC_CalibStitchBlendCtrl.pNandTable[TblID]);
            UINT32 IsNewData = 0U;
            const SVC_CALIB_ST_BLEND_TBL_DATA_s *pInput = NULL;

            AmbaMisra_TypeCast(&(pInput), &(pTblInfo->pBuf));

            if (SVC_CalibStitchBlendCtrl.pNandTable[TblID].Header.VinSelectBits == 0U) {
                IsNewData = 1U;
            }

            if (pInput == NULL) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ST_BLD, "Fail to set stitch blend table - invalid input table data!", 0U, 0U);
            } else if (pInput->pTbl == NULL) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ST_BLD, "Fail to set stitch blend table - invalid input table!", 0U, 0U);
            } else if (pInput->TblLength > sizeof(pNandTable->Table)) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ST_BLD, "Fail to set stitch blend table - input table length(%d) is out-of range(%d)!", pInput->TblLength, sizeof(pNandTable->Table));
            } else {
                AmbaSvcWrap_MisraMemset(pNandTable, 0, sizeof(SVC_CALIB_ST_BLD_NAND_TBL_s));
                pNandTable->Header.Enable           = 1U;
                pNandTable->Header.Width            = pInput->Width;
                pNandTable->Header.Height           = pInput->Height;
                pNandTable->Header.VinSelectBits    = pTblInfo->CalChan.VinSelectBits;
                pNandTable->Header.SensorSelectBits = pTblInfo->CalChan.SensorSelectBits;
                pNandTable->Header.TableLength      = pInput->TblLength;
                AmbaSvcWrap_MisraMemcpy(pNandTable->Table, pInput->pTbl, (sizeof(UINT8) * pNandTable->Header.TableLength));

                // Update nand header
                if (SVC_CalibStitchBlendCtrl.pNandHeader->Enable == 0U) {
                    SVC_CalibStitchBlendCtrl.pNandHeader->Enable = 1U;
                }
                if (SVC_CalibStitchBlendCtrl.pNandHeader->DebugMsgOn == 0U) {
                    SVC_CalibStitchBlendCtrl.pNandHeader->DebugMsgOn = SVC_CalibStitchBlendCtrl.EnableDebugMsg;
                }
                if (SVC_CalibStitchBlendCtrl.pNandHeader->DebugMsgOn == 0U) {
                    SVC_CalibStitchBlendCtrl.pNandHeader->DebugMsgOn = pTblInfo->DbgMsgOn;
                }
                if (SVC_CalibStitchBlendCtrl.EnableDebugMsg == 0U) {
                    SVC_CalibStitchBlendCtrl.EnableDebugMsg = pTblInfo->DbgMsgOn;
                }
                if (IsNewData > 0U) {
                    SVC_CalibStitchBlendCtrl.pNandHeader->NumOfTable += 1U;
                }

                if (SVC_CalibStitchBlendCtrl.EnableDebugMsg > 0U) {
                    SvcLog_DBG(SVC_LOG_ST_BLD, " ", 0U, 0U);
                    SVC_WRAP_PRINT "---- Set Stitch Blend Header (%p) ----" SVC_PRN_ARG_S SVC_LOG_ST_BLD SVC_PRN_ARG_CPOINT SVC_CalibStitchBlendCtrl.pNandHeader SVC_PRN_ARG_POST SVC_PRN_ARG_E
                    SvcLog_DBG(SVC_LOG_ST_BLD, "            Enable : %d", SVC_CalibStitchBlendCtrl.pNandHeader->Enable, 0U);
                    SvcLog_DBG(SVC_LOG_ST_BLD, "        NumOfTable : %d", SVC_CalibStitchBlendCtrl.pNandHeader->NumOfTable, 0U);
                    SvcLog_DBG(SVC_LOG_ST_BLD, "        DebugMsgOn : %d", SVC_CalibStitchBlendCtrl.pNandHeader->DebugMsgOn, 0U);
                    SVC_WRAP_PRINT "---- Set Stitch Blend Table (%p) ----" SVC_PRN_ARG_S SVC_LOG_ST_BLD SVC_PRN_ARG_CPOINT pNandTable SVC_PRN_ARG_POST SVC_PRN_ARG_E
                    SvcLog_DBG(SVC_LOG_ST_BLD, "            Enable : %d"     , pNandTable->Header.Enable, 0U);
                    SvcLog_DBG(SVC_LOG_ST_BLD, "     VinSelectBits : 0x%x"   , pNandTable->Header.VinSelectBits, 0U);
                    SvcLog_DBG(SVC_LOG_ST_BLD, "  SensorSelectBits : 0x%x"   , pNandTable->Header.SensorSelectBits, 0U);
                    SvcLog_DBG(SVC_LOG_ST_BLD, "             Width : %d"     , pNandTable->Header.Width, 0U);
                    SvcLog_DBG(SVC_LOG_ST_BLD, "            Height : %d"     , pNandTable->Header.Height, 0U);
                    SvcLog_DBG(SVC_LOG_ST_BLD, "       TableLength : 0x%x"   , pNandTable->Header.TableLength, 0U);
                    SVC_WRAP_PRINT "             Table : %p"
                        SVC_PRN_ARG_S SVC_LOG_ST_BLD
                        SVC_PRN_ARG_CPOINT pNandTable->Table SVC_PRN_ARG_POST
                        SVC_PRN_ARG_E
                }

                /* Update to NAND */
                RetVal = SvcCalib_DataSave(SVC_CALIB_STITCH_BLEND_ID);
                if (RetVal != OK) {
                    SvcLog_NG(SVC_LOG_ST_BLD, "Fail to save stitch blend to nand!", 0U, 0U);
                }
            }

            PRetVal = AmbaKAL_MutexGive(&(SVC_CalibStitchBlendCtrl.Mutex)); AmbaMisra_TouchUnused(&PRetVal);
        }
    }

    return RetVal;
}

static UINT32 SvcCalib_StBlendShellCfgDbgMsg(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibStitchBlendCtrlFlag & SVC_CALIB_ST_BLD_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_BLD, "Fail to switch stitch blend debug msg - initial stitch blend module first!", 0U, 0U);
    }

    if (ArgCount < 3U) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_BLD, "Fail to switch stitch blend debug msg - Argc should >= 3", 0U, 0U);
    }

    if (pArgVector == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_BLD, "Fail to switch stitch blend debug msg - Argv should not null!", 0U, 0U);
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 DbgMsgOn = 0U;

        PRetVal = SvcWrap_strtoul(pArgVector[3U],  &DbgMsgOn ); AmbaMisra_TouchUnused(&PRetVal);

        SVC_CalibStitchBlendCtrl.EnableDebugMsg = DbgMsgOn;

        SvcLog_DBG(SVC_LOG_ST_BLD, "", 0U, 0U);
        SvcLog_DBG(SVC_LOG_ST_BLD, "------ Calibration Stitch Blend Debug Msg On/Off %d ------", SVC_CalibStitchBlendCtrl.EnableDebugMsg, 0U);

    } else {
        AmbaMisra_TouchUnused(&ArgCount);
    }

    return RetVal;
}

static void SvcCalib_StBlendShellCfgDbgMsgU(void)
{
    SVC_WRAP_PRINT "  %scfg_dbg_msg%s           :"
        SVC_PRN_ARG_S SVC_LOG_ST_BLD
        SVC_PRN_ARG_CSTR SVC_LOG_CAL_HL_TITLE_1 SVC_PRN_ARG_POST
        SVC_PRN_ARG_CSTR SVC_LOG_CAL_HL_END     SVC_PRN_ARG_POST
        SVC_PRN_ARG_E
    SvcLog_DBG(SVC_LOG_ST_BLD, "  ------------------------------------------------------", 0U, 0U);
    SvcLog_DBG(SVC_LOG_ST_BLD, "    [Enable]            : 0: disable debug message", 0U, 0U);
    SvcLog_DBG(SVC_LOG_ST_BLD, "                        : 1: enable debug message", 0U, 0U);
    SvcLog_DBG(SVC_LOG_ST_BLD, " ", 0U, 0U);
}

static void SvcCalib_StBlendShellEntryInit(void)
{
    if ((SVC_CalibStitchBlendCtrlFlag & SVC_CALIB_ST_BLD_FLG_SHELLINIT) == 0U) {

        AmbaSvcWrap_MisraMemset(CalibStitchBlendShellFunc, 0, sizeof(CalibStitchBlendShellFunc));

        CalibStitchBlendShellFunc[0U] = (SVC_CALIB_ST_BLD_SHELL_FUNC_s) { 1U, "cfg_dbg_msg",     SvcCalib_StBlendShellCfgDbgMsg,  SvcCalib_StBlendShellCfgDbgMsgU  };

        SVC_CalibStitchBlendCtrlFlag |= SVC_CALIB_ST_BLD_FLG_SHELLINIT;
    }
}

UINT32 SvcCalib_StitchBlendShellFunc(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_NG, PRetVal;

    SvcCalib_StBlendShellEntryInit();

    if (ArgCount > 0U) {
        UINT32 ShellIdx;
        UINT32 ShellCount = (UINT32)(sizeof(CalibStitchBlendShellFunc)) / (UINT32)(sizeof(CalibStitchBlendShellFunc[0]));

        for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
            if ((CalibStitchBlendShellFunc[ShellIdx].pFunc != NULL) && (CalibStitchBlendShellFunc[ShellIdx].Enable > 0U)) {
                if (0 == SvcWrap_strcmp(pArgVector[2U], CalibStitchBlendShellFunc[ShellIdx].ShellCmdName)) {
                    PRetVal = (CalibStitchBlendShellFunc[ShellIdx].pFunc)(ArgCount, pArgVector);
                    AmbaMisra_TouchUnused(&PRetVal);

                    RetVal = SVC_OK;
                    break;
                }
            }
        }

        if (RetVal != SVC_OK) {
            SvcCalib_StBlendShellUsage();
        }
    }

    return RetVal;
}

static void SvcCalib_StBlendShellUsage(void)
{
    UINT32 ShellIdx;
    UINT32 ShellCount = (UINT32)(sizeof(CalibStitchBlendShellFunc)) / (UINT32)(sizeof(CalibStitchBlendShellFunc[0]));

    SvcLog_DBG(SVC_LOG_ST_BLD, "", 0U, 0U);
    SvcLog_DBG(SVC_LOG_ST_BLD, "====== Stitch Blend Command Usage ======", 0U, 0U);

    for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
        if ((CalibStitchBlendShellFunc[ShellIdx].pFunc != NULL) && (CalibStitchBlendShellFunc[ShellIdx].Enable > 0U)) {
            if (CalibStitchBlendShellFunc[ShellIdx].pUsageFunc == NULL) {
                SVC_WRAP_PRINT "  %s" SVC_PRN_ARG_S SVC_LOG_ST_BLD SVC_PRN_ARG_CSTR CalibStitchBlendShellFunc[ShellIdx].ShellCmdName SVC_PRN_ARG_POST SVC_PRN_ARG_E
            } else {
                (CalibStitchBlendShellFunc[ShellIdx].pUsageFunc)();
            }
        }
    }
}

static void SvcCalib_StBlendCmdHdlrInit(void)
{
    if ((SVC_CalibStitchBlendCtrlFlag & SVC_CALIB_ST_BLD_FLG_CMD_INIT) == 0U) {

        AmbaSvcWrap_MisraMemset(SvcCalibStitchBlendCmdHdlr, 0, sizeof(SvcCalibStitchBlendCmdHdlr));

        SvcCalibStitchBlendCmdHdlr[SVC_CALIB_CMD_MEM_QUERY      ] = (SVC_CALIB_ST_BLD_CMD_HDLR_s) {1, SVC_CALIB_CMD_MEM_QUERY,       SvcCalib_StBlendCmdMemQuery   };
        SvcCalibStitchBlendCmdHdlr[SVC_CALIB_CMD_DATA_LOAD      ] = (SVC_CALIB_ST_BLD_CMD_HDLR_s) {1, SVC_CALIB_CMD_DATA_LOAD,       NULL                          };
        SvcCalibStitchBlendCmdHdlr[SVC_CALIB_CMD_ITEM_DATA_INIT ] = (SVC_CALIB_ST_BLD_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_DATA_INIT,  SvcCalib_StBlendCmdInit       };
        SvcCalibStitchBlendCmdHdlr[SVC_CALIB_CMD_ITEM_TBL_CFG   ] = (SVC_CALIB_ST_BLD_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_TBL_CFG,    NULL                          };
        SvcCalibStitchBlendCmdHdlr[SVC_CALIB_CMD_ITEM_TBL_UPDATE] = (SVC_CALIB_ST_BLD_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_TBL_UPDATE, NULL                          };
        SvcCalibStitchBlendCmdHdlr[SVC_CALIB_CMD_ITEM_TBL_GET   ] = (SVC_CALIB_ST_BLD_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_TBL_GET,    SvcCalib_StBlendCmdTblDataGet };
        SvcCalibStitchBlendCmdHdlr[SVC_CALIB_CMD_ITEM_TBL_SET   ] = (SVC_CALIB_ST_BLD_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_TBL_SET,    SvcCalib_StBlendCmdTblDataSet };

        SVC_CalibStitchBlendCtrlFlag |= SVC_CALIB_ST_BLD_FLG_CMD_INIT;
    }
}

static UINT32 SvcCalib_StBlendCmdMemQuery(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    if (pParam1 == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_BLD, "Fail to query stitch blend memory - input stitch blend table number should not zero.", 0U, 0U);
    }

    if (pParam2 == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_BLD, "Fail to query stitch blend memory - output stitch blend shadow buffer size should not null!", 0U, 0U);
    }

    if (pParam3 == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_BLD, "Fail to query stitch blend memory - output stitch blend working buffer size should not null!", 0U, 0U);
    }

    if (pParam4 == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_BLD, "Fail to query stitch blend memory - stitch blend calculation buffer size should not null!", 0U, 0U);
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

        RetVal = SvcCalib_StBlendMemQuery(NomOfTable, pShadowBufSize, pWorkingBufSize, pCalcMemSize);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_ST_BLD, "Fail to query stitch blend memory - query memory fail!", 0U, 0U);
        }

        AmbaMisra_TouchUnused(pParam1);
        AmbaMisra_TouchUnused(pParam2);
        AmbaMisra_TouchUnused(pParam3);
        AmbaMisra_TouchUnused(pParam4);
        AmbaMisra_TouchUnused(pU32Val);
    } else {
        AmbaMisra_TouchUnused(SvcCalibStitchBlendCmdHdlr);
    }

    return RetVal;
}

static UINT32 SvcCalib_StBlendCmdInit(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    if (pParam1 == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_BLD, "Fail to init stitch blend - input calib object should not null!", 0U, 0U);
    }

    if (RetVal == SVC_OK) {
        RetVal = SvcCalib_StBlendCreate();
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_ST_BLD, "Fail to init stitch blend - create module fail!", 0U, 0U);
        }
    }

    if (RetVal == SVC_OK) {
        const SVC_CALIB_OBJ_s *pCalObj;

        AmbaMisra_TypeCast(&pCalObj, &pParam1);
        if (pCalObj == NULL) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ST_BLD, "Fail to init stitch blend - invalid calib obj!", 0U, 0U);
        } else {
            RetVal = SvcCalib_StBlendShadowInit(pCalObj);
            if (RetVal != SVC_OK) {
                SvcLog_NG(SVC_LOG_ST_BLD, "Fail to init stitch blend - shadow initial fail!", 0U, 0U);
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

static UINT32 SvcCalib_StBlendCmdTblDataGet(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;
    UINT32 *pU32Val = NULL;
    const SVC_CALIB_TBL_INFO_s *pTblInfo;

    AmbaMisra_TypeCast(&(pU32Val), &(pParam1));
    AmbaMisra_TypeCast(&(pTblInfo), &(pParam2));

    if (pU32Val == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_BLD, "Fail to get table data - input table id should not null!", 0U, 0U);
    } else {
        RetVal = SvcCalib_StBlendTableDataGet(*pU32Val, pTblInfo);
    }

    AmbaMisra_TouchUnused(pParam1);
    AmbaMisra_TouchUnused(pParam2);
    AmbaMisra_TouchUnused(pParam3);
    AmbaMisra_TouchUnused(pParam4);
    AmbaMisra_TouchUnused(pU32Val);

    return RetVal;

}

static UINT32 SvcCalib_StBlendCmdTblDataSet(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;
    UINT32 *pU32Val = NULL;
    const SVC_CALIB_TBL_INFO_s *pTblInfo;

    AmbaMisra_TypeCast(&(pU32Val), &(pParam1));
    AmbaMisra_TypeCast(&(pTblInfo), &(pParam2));

    if (pU32Val == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_BLD, "Fail to set table data - input table id should not null!", 0U, 0U);
    } else {
        RetVal = SvcCalib_StBlendTableDataSet(*pU32Val, pTblInfo);
    }

    AmbaMisra_TouchUnused(pParam1);
    AmbaMisra_TouchUnused(pParam2);
    AmbaMisra_TouchUnused(pParam3);
    AmbaMisra_TouchUnused(pParam4);
    AmbaMisra_TouchUnused(pU32Val);

    return RetVal;

}

/**
 * calib stitch blend command entry
 *
 * @param [in] CmdID  the stitch blend command id
 * @param [in] Param1 no.01 parameter
 * @param [in] Param2 no.02 parameter
 * @param [in] Param3 no.03 parameter
 * @param [in] Param4 no.04 parameter
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcCalib_StitchBlendCmdFunc(UINT32 CmdID, void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    SvcCalib_StBlendCmdHdlrInit();

    if (CmdID >= SVC_CALIB_CMD_NUM) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ST_BLD, "Fail to handler cmd - invalid command id(%d)", CmdID, 0U);
    } else {
        const UINT32 HdlrCount = (UINT32)(sizeof(SvcCalibStitchBlendCmdHdlr)) / ((UINT32)sizeof(SvcCalibStitchBlendCmdHdlr[0]));
        UINT32 HdlrIdx;

        for (HdlrIdx = 0U; HdlrIdx < HdlrCount; HdlrIdx ++) {
            if ((SvcCalibStitchBlendCmdHdlr[HdlrIdx].Enable > 0U) &&
                (SvcCalibStitchBlendCmdHdlr[HdlrIdx].CmdID == CmdID) &&
                (SvcCalibStitchBlendCmdHdlr[HdlrIdx].pHandler != NULL)) {
                RetVal = (SvcCalibStitchBlendCmdHdlr[HdlrIdx].pHandler)(pParam1, pParam2, pParam3, pParam4);
            }
        }
    }

    return RetVal;
}
