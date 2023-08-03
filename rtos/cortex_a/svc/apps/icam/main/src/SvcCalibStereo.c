/**
 *  @file SvcCalibStereo.c
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
 *  @details svc calibration stereo
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaVIN_Def.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaFS.h"
#include "AmbaDef.h"
#include "AmbaDSP_VideoEnc.h"
#include "AmbaSensor.h"
#include "AmbaYuv.h"
#include "AmbaFPD.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaPrint.h"


#include "SvcWrap.h"
#include "SvcCalibMgr.h"

#include "SvcErrCode.h"
#include "SvcLog.h"

#include "SvcCalibStereo.h"
#include "SvcWarp.h"
#include "SvcVinSrc.h"
#include "SvcVoutSrc.h"
#include "SvcResCfg.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcBufMap.h"


#define SVC_CALIB_STEREO_FLG_INIT      (0x1U)
#define SVC_CALIB_STEREO_FLG_SHDW_INIT (0x2U)
#define SVC_CALIB_STEREO_FLG_DBG_MSG   (0x100U)
#define SVC_CALIB_STEREO_FLG_SHELLINIT (0x1000U)
#define SVC_CALIB_STEREO_FLG_CMD_INIT  (0x2000U)

#define SVC_CALIB_STEREO_NAME          ("Calib_Stereo")
#define SVC_CALIB_STEREO_NAND_HDR_SIZE (0x200U)

typedef struct {
    UINT32 Enable;
    UINT32 NumOfTable;
    UINT32 DebugMsgOn;
    UINT32 Reserved[(SVC_CALIB_STEREO_NAND_HDR_SIZE / 4U) - 3U];
} SVC_CALIB_STEREO_NAND_HEADER_s;

typedef struct {
    UINT32 Enable;
} SVC_CALIB_STEREO_TBL_HEADER_s;

typedef struct {
    char                            SerialNumber[16U];
    AMBA_STU_STEREO_CAM_WARP_INFO_s CamWarpInfo;
} SVC_CALIB_STEREO_TBL_s;

typedef struct {
    SVC_CALIB_STEREO_TBL_HEADER_s Header;
    SVC_CALIB_STEREO_TBL_s        Tbl;
} SVC_CALIB_STEREO_NAND_TBL_s;

typedef struct {
    char                                 Name[16];
    AMBA_KAL_MUTEX_t                     Mutex;
    SVC_CALIB_STEREO_NAND_HEADER_s      *pNandHeader;
    SVC_CALIB_STEREO_NAND_TBL_s         *pNandTable;
    UINT32                               NumOfTable;
    SVC_CALIB_STEREO_INFO_s             *pStereoInfo;

    UINT32                               EnableDebugMsg;
    ULONG                                *BufBaseStereo;
    UINT32                               BufSizeStereo;
} SVC_CALIB_STEREO_CTRL_s;

#define SVC_LOG_CAL_STEREO "CAL_STEREO"
static void LOG_CAL_STEREO_OK(const char *pStr, UINT32 Arg1, UINT32 Arg2);
static void LOG_CAL_STEREO_NG(const char *pStr, UINT32 Arg1, UINT32 Arg2);
static void LOG_CAL_STEREO_API(const char *pStr, UINT32 Arg1, UINT32 Arg2);
static void LOG_CAL_STEREO_DBG(const char *pStr, UINT32 Arg1, UINT32 Arg2);

#define PRN_CAL_STEREO_OK       SVC_WRAP_PRINT
// #define PRN_CAL_STEREO_NG       SVC_WRAP_PRINT
#define PRN_CAL_STEREO_API      SVC_WRAP_PRINT
//#define PRN_CAL_STEREO_DBG      SVC_WRAP_PRINT
//#define PRN_CAL_STEREO_DBG_E    ; SvcWrapPrint.Argc ++; SvcCalib_StereoPrnDbg(SVC_CALIB_STEREO_FLG_DBG_MSG, pPrnModuleStr, &SvcWrapPrint); }

static UINT32                       SvcCalib_StereoMemQuery(UINT32 MaxTableNum, UINT32 *pShadowSize, UINT32 *pWorkMemSize, UINT32 *pCalcMemSize);
static UINT32                       SvcCalib_StereoCreate(void);
static UINT32                       SvcCalib_StereoShadowInit(const SVC_CALIB_OBJ_s *pCalObj);
static UINT32                       SvcCalib_StereoTableDataGet(UINT32 TblID,const SVC_CALIB_TBL_INFO_s *pTblInfo);
static UINT32                       SvcCalib_StereoTableDataSet(UINT32 TblID, const SVC_CALIB_TBL_INFO_s *pTblInfo);

// Private function definition for calibration command
typedef UINT32 (*SVC_CALIB_STEREO_CMD_FUNC_f)(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
typedef struct {
     UINT32                           Enable;
     UINT32                           CmdID;
     SVC_CALIB_STEREO_CMD_FUNC_f pHandler;
} SVC_CALIB_STEREO_CMD_HDLR_s;

// Private function definition for calibration command
static void   SvcCalib_StereoCmdHdlrInit(void);
static UINT32 SvcCalib_StereoCmdMemQuery(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_StereoCmdInit(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
//static UINT32 SvcCalib_StereoCmdShellFunc(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_StereoGetTblIDInSerialNum(const char *pSerialNum, UINT32 *pTblID);
static UINT32 SvcCalib_StereoWarpTableUpdate(SVC_CALIB_CHANNEL_s *pCalibChan, AMBA_IK_MODE_CFG_s *pImgMode);
static UINT32 SvcCalib_StereoCmdTblDataGet(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_StereoCmdTblDataSet(void *pParam1, void *pParam2, void *pParam3, void *pParam4);

// Private function definition for shell command.
typedef UINT32 (*SVC_CALIB_STEREO_SHELL_FUNC_f)(UINT32 ArgCount, char * const *pArgVector);
typedef void   (*SVC_CALIB_STEREO_SHELL_USAGE_f)(void);

typedef struct {
    UINT32                              Enable;
    char                                ShellCmdName[32];
    SVC_CALIB_STEREO_SHELL_FUNC_f  pFunc;
    SVC_CALIB_STEREO_SHELL_USAGE_f pUsageFunc;
} SVC_CALIB_STEREO_SHELL_FUNC_s;

static UINT32 SvcCalib_StereoShellCfgDbgMsg(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_StereoShellCfgDbgMsgU(void);
static UINT32 SvcCalib_StereoShellLoadFromSd(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_StereoShellLoadFromSdU(void);
static UINT32 SvcCalib_StereoShellGetTbl(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_StereoShellGetTblU(void);
static UINT32 SvcCalib_StereoShellSetWarp(UINT32 ArgCount, char * const *pArgVector);
static void SvcCalib_StereoShellSetWarpU(void);
static void   SvcCalib_StereoShellEntryInit(void);
static UINT32 SvcCalib_StereoCmdTblUpdate(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
//static UINT32 SvcCalib_StereoShellFunc(UINT32 ArgCount, char * const *pArgVectorj);
static void   SvcCalib_StereoShellUsage(void);

#define SVC_CALIB_STEREO_SHELL_CMD_NUM  (4U)
static SVC_CALIB_STEREO_SHELL_FUNC_s CalibStereoShellFunc[SVC_CALIB_STEREO_SHELL_CMD_NUM] GNU_SECTION_NOZEROINIT;
static SVC_CALIB_STEREO_CMD_HDLR_s SvcCalibStereoCmdHdlr[SVC_CALIB_CMD_NUM] GNU_SECTION_NOZEROINIT;
static SVC_CALIB_STEREO_CTRL_s SVC_CalibStereoCtrl GNU_SECTION_NOZEROINIT;
static UINT32 SVC_CalibStereoCtrlFlag = 0;

static void LOG_CAL_STEREO_OK(const char *pStr, UINT32 Arg1, UINT32 Arg2)
{
    SvcLog_OK(SVC_LOG_CAL_STEREO, pStr, Arg1, Arg2);
}
static void LOG_CAL_STEREO_NG(const char *pStr, UINT32 Arg1, UINT32 Arg2)
{
    SvcLog_NG(SVC_LOG_CAL_STEREO, pStr, Arg1, Arg2);
}
static void LOG_CAL_STEREO_API(const char *pStr, UINT32 Arg1, UINT32 Arg2)
{
    SvcLog_DBG(SVC_LOG_CAL_STEREO, pStr, Arg1, Arg2);
}
static void LOG_CAL_STEREO_DBG(const char *pStr, UINT32 Arg1, UINT32 Arg2)
{
    if ( ( SVC_CalibStereoCtrlFlag & SVC_CALIB_STEREO_FLG_DBG_MSG ) > 0U ) {
        SvcLog_DBG(SVC_LOG_CAL_STEREO, pStr, Arg1, Arg2);
    }
}
//static void SvcCalib_StereoPrnDbg(UINT32 Level, const char *pModule, SVC_WRAP_PRINT_s *pPrint)
//{
//    if ( ( SVC_CalibStereoCtrlFlag & Level ) > 0U ) {
//        SvcWrap_Print(pModule, pPrint);
//    }
//}

static UINT32 SvcCalib_StereoBufferInit(void)
{
    UINT32 RetVal;
    UINT32 TotalSize;
    ULONG BufferBase;

    RetVal = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_STEREO_CALIB, &BufferBase, &TotalSize);
    if (SVC_OK != RetVal) {
        LOG_CAL_STEREO_NG("SvcBuffer_Request(SMEM_PF0_ID_STEREO_CALIB) failed", 0U, 0U);

        RetVal = SVC_NG;
    } else {
        AmbaMisra_TypeCast(&(SVC_CalibStereoCtrl.BufBaseStereo), &BufferBase);
        SVC_CalibStereoCtrl.BufSizeStereo = TotalSize;

        RetVal = SVC_OK;
    }
    return RetVal;
}

static UINT32 SvcCalib_StereoMemQuery(UINT32 MaxTableNum, UINT32 *pShadowSize, UINT32 *pWorkMemSize, UINT32 *pCalcMemSize)
{
    UINT32 RetVal = SVC_OK;

    UINT32 DramShadowSize = 0, WorkingMemSize = 0, CalcMemSize = 0;
    UINT32 DramShadowHeaderSize = (UINT32)(sizeof(SVC_CALIB_STEREO_NAND_HEADER_s));
    UINT32 DramShadowTableSize  = (UINT32)(sizeof(SVC_CALIB_STEREO_NAND_TBL_s));
    UINT32 DramShadowDataSize   = DramShadowTableSize * MaxTableNum;
    UINT32 InfoSize             = (UINT32)(sizeof(SVC_CALIB_STEREO_INFO_s));
    UINT32 TotalInfoSize        = InfoSize * MaxTableNum;

    DramShadowSize  = DramShadowHeaderSize;
    DramShadowSize += DramShadowDataSize;
    WorkingMemSize  = TotalInfoSize;

    if (pShadowSize != NULL) {
        *pShadowSize  = DramShadowSize;
    }
    LOG_CAL_STEREO_DBG("Query the calib stereo dram shadow size 0x%X ( table num %d )", DramShadowSize, MaxTableNum);

    if (pWorkMemSize != NULL) {
        *pWorkMemSize = WorkingMemSize;
    }
    LOG_CAL_STEREO_DBG("Query the calib stereo working memory size 0x%X ( table num %d )", WorkingMemSize, MaxTableNum);

    if (pCalcMemSize != NULL) {
        *pCalcMemSize = CalcMemSize;
    }
    LOG_CAL_STEREO_DBG("Query the calib stereo algo. memory size 0x%x ( table num %d )", CalcMemSize, MaxTableNum);

    return RetVal;
}

static UINT32 SvcCalib_StereoCreate(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibStereoCtrlFlag & SVC_CALIB_STEREO_FLG_INIT) > 0U) {
        RetVal = SVC_OK;
        LOG_CAL_STEREO_DBG("Calibration stereo module has been created!", 0U, 0U);
    } else {
        // Reset the stereo control module
        AmbaSvcWrap_MisraMemset(&SVC_CalibStereoCtrl, 0, sizeof(SVC_CalibStereoCtrl));

        // Configure the name
        SvcWrap_strcpy(SVC_CalibStereoCtrl.Name, sizeof(SVC_CalibStereoCtrl.Name), SVC_CALIB_STEREO_NAME);

        // Create the mutex
        RetVal = AmbaKAL_MutexCreate(&(SVC_CalibStereoCtrl.Mutex), SVC_CalibStereoCtrl.Name);
        if (RetVal != SVC_OK) {
            LOG_CAL_STEREO_NG("Fail to create stereo - create mutex fail!", 0U, 0U);
        } else {
            PRetVal = (SVC_CalibStereoCtrlFlag & (SVC_CALIB_STEREO_FLG_DBG_MSG | SVC_CALIB_STEREO_FLG_SHELLINIT | SVC_CALIB_STEREO_FLG_CMD_INIT));
            SVC_CalibStereoCtrlFlag  = PRetVal;
            SVC_CalibStereoCtrlFlag |= SVC_CALIB_STEREO_FLG_INIT;
            LOG_CAL_STEREO_OK("Successful to create stereo table!", 0U, 0U);
        }
    }

    return RetVal;
}

static UINT32 SvcCalib_StereoShadowInit(const SVC_CALIB_OBJ_s *pCalObj)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibStereoCtrlFlag & SVC_CALIB_STEREO_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to initial stereo dram shodow - create stereo first!", 0U, 0U);
    } else if (pCalObj == NULL) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to initial stereo dram shodow - invalid calib object!", 0U, 0U);
    } else if (pCalObj->Enable == 0U) {
        RetVal = 99;
        LOG_CAL_STEREO_API("Disable stereo dram shadow!!", 0U, 0U);
    } else {
        UINT32 DramShadowSize = 0, WorkingMemSize = 0;
        const SVC_CALIB_STEREO_NAND_HEADER_s *pHeader;

        AmbaMisra_TypeCast(&(pHeader), &(pCalObj->pShadowBuf));

        RetVal = SvcCalib_StereoBufferInit();
        if (RetVal != SVC_OK) {
            LOG_CAL_STEREO_NG("Fail to initial Stereo shared Buffer", 0U, 0U);
        } else {
            LOG_CAL_STEREO_OK("Sucess to initial Stereo shared Buffer", 0U, 0U);
        }

        RetVal = SvcCalib_StereoMemQuery(pCalObj->NumOfTable, &DramShadowSize, &WorkingMemSize, NULL);
        if (RetVal != SVC_OK) {
            LOG_CAL_STEREO_NG("Fail to initial stereo dram shadow - query memory fail!", 0U, 0U);
        } else if ((pCalObj->pShadowBuf == NULL) || (pHeader == NULL)) {
            RetVal = SVC_NG;
            LOG_CAL_STEREO_NG("Fail to initial stereo dram shadow - dram shadow buffer should not null!", 0U, 0U);
        } else if (pCalObj->ShadowBufSize < DramShadowSize) {
            RetVal = SVC_NG;
            LOG_CAL_STEREO_NG("Fail to initial stereo dram shadow - prepare dram shadow size is too small!", 0U, 0U);
        } else if (pCalObj->pWorkingBuf == NULL) {
                RetVal = SVC_NG;
                LOG_CAL_STEREO_NG("Fail to initial stereo dram shadow - working buffer should not null!", 0U, 0U);
        } else if (pCalObj->WorkingBufSize < WorkingMemSize) {
            RetVal = SVC_NG;
            LOG_CAL_STEREO_NG("Fail to initial stereo dram shadow - working buffer size is too small!", 0U, 0U);
        } else if (pHeader->NumOfTable > pCalObj->NumOfTable) {
            RetVal = SVC_NG;
            LOG_CAL_STEREO_NG("Fail to initial stereo dram shadow - calib nand table count(%d) > max stereo dram shadow table count(%d)",
                    pHeader->NumOfTable, pCalObj->NumOfTable);
        } else {
            if (KAL_ERR_NONE != AmbaKAL_MutexTake(&(SVC_CalibStereoCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
                RetVal = SVC_NG;
                LOG_CAL_STEREO_NG("Fail to initial stereo dram shadow - take mutex fail!", 0U, 0U);
            } else {
                UINT8 *pNandHeaderBuf    = pCalObj->pShadowBuf;
                UINT32 NandHeaderSize    = (UINT32)(sizeof(SVC_CALIB_STEREO_NAND_HEADER_s));
                UINT8 *pNandTableBuf     = &(pNandHeaderBuf[NandHeaderSize]);
                UINT32 TotalStereoTableSize = sizeof(SVC_CALIB_STEREO_TBL_s) * pCalObj->NumOfTable;

                SvcLog_DBG(SVC_LOG_CAL_STEREO, "====== Shadow initialized Start ======", 0U, 0U);
                PRN_CAL_STEREO_API "  DramShadow Addr  : %p 0x%x"
                    SVC_PRN_ARG_S SVC_LOG_CAL_STEREO
                    SVC_PRN_ARG_CPOINT pCalObj->pShadowBuf    SVC_PRN_ARG_POST
                    SVC_PRN_ARG_UINT32 pCalObj->ShadowBufSize SVC_PRN_ARG_POST
                    SVC_PRN_ARG_E
                PRN_CAL_STEREO_API "    Nand Header Addr : %p 0x%x"
                    SVC_PRN_ARG_S SVC_LOG_CAL_STEREO
                    SVC_PRN_ARG_CPOINT pNandHeaderBuf SVC_PRN_ARG_POST
                    SVC_PRN_ARG_UINT32 NandHeaderSize SVC_PRN_ARG_POST
                    SVC_PRN_ARG_E
                PRN_CAL_STEREO_API "    Nand Table Addr  : %p"
                    SVC_PRN_ARG_S SVC_LOG_CAL_STEREO
                    SVC_PRN_ARG_CPOINT pNandTableBuf SVC_PRN_ARG_POST
                    SVC_PRN_ARG_E
                PRN_CAL_STEREO_API "  Working Mem Addr : %p 0x%x"
                    SVC_PRN_ARG_S SVC_LOG_CAL_STEREO
                    SVC_PRN_ARG_CPOINT pCalObj->pWorkingBuf    SVC_PRN_ARG_POST
                    SVC_PRN_ARG_UINT32 pCalObj->WorkingBufSize SVC_PRN_ARG_POST
                    SVC_PRN_ARG_E
                PRN_CAL_STEREO_API "  Stereo Info Addr Addr : %p 0x%x"
                    SVC_PRN_ARG_S SVC_LOG_CAL_STEREO
                    SVC_PRN_ARG_CPOINT pCalObj->pWorkingBuf    SVC_PRN_ARG_POST
                    SVC_PRN_ARG_UINT32 TotalStereoTableSize SVC_PRN_ARG_POST
                    SVC_PRN_ARG_E
                // Configure nand header
                AmbaMisra_TypeCast(&(SVC_CalibStereoCtrl.pNandHeader), &(pNandHeaderBuf));
                // Configure nand table
                AmbaMisra_TypeCast(&(SVC_CalibStereoCtrl.pNandTable),  &(pNandTableBuf)); AmbaMisra_TouchUnused(pNandTableBuf);
                // Configure working stereo table
                AmbaMisra_TypeCast(&(SVC_CalibStereoCtrl.pStereoInfo),  &(pCalObj->pWorkingBuf));
                if (SVC_CalibStereoCtrl.pNandHeader == NULL) {
                    RetVal = SVC_NG;
                    LOG_CAL_STEREO_NG("Fail to initial stereo dram shadow - nand header should not null!", 0U, 0U);
                } else if (SVC_CalibStereoCtrl.pNandTable == NULL) {
                    RetVal = SVC_NG;
                    LOG_CAL_STEREO_NG("Fail to initial stereo dram shadow - nand table should not null!", 0U, 0U);
                } else if (SVC_CalibStereoCtrl.pStereoInfo == NULL) {
                    RetVal = SVC_NG;
                    LOG_CAL_STEREO_NG("Fail to initial stereo dram shadow - stereo table should not null!", 0U, 0U);
                } else {
                    SVC_CalibStereoCtrl.NumOfTable = pCalObj->NumOfTable;
                    AmbaSvcWrap_MisraMemset(SVC_CalibStereoCtrl.pStereoInfo, 0, TotalStereoTableSize);
                    SVC_CalibStereoCtrl.EnableDebugMsg = SVC_CalibStereoCtrl.pNandHeader->DebugMsgOn;
                    if (SVC_CalibStereoCtrl.EnableDebugMsg > 0U) {
                        UINT32 Idx;

                        LOG_CAL_STEREO_API("", 0U, 0U);
                        LOG_CAL_STEREO_API("---- Stereo Info ----", 0U, 0U);
                        LOG_CAL_STEREO_API("  Enable         : %d", SVC_CalibStereoCtrl.pNandHeader->Enable, 0U);
                        LOG_CAL_STEREO_API("  NumOfTable     : %d", SVC_CalibStereoCtrl.pNandHeader->NumOfTable, 0U);
                        LOG_CAL_STEREO_API("  DebugMsgOn     : %d", SVC_CalibStereoCtrl.pNandHeader->DebugMsgOn, 0U);

                        {
                            const SVC_CALIB_STEREO_NAND_TBL_s *pNandTable;

                            for (Idx = 0U; Idx < pCalObj->NumOfTable; Idx ++) {
                                pNandTable = &(SVC_CalibStereoCtrl.pNandTable[Idx]);

                                PRN_CAL_STEREO_API "  ---- Nand Table[%d] %p -----"
                                    SVC_PRN_ARG_S      SVC_LOG_CAL_STEREO
                                    SVC_PRN_ARG_UINT32 Idx  SVC_PRN_ARG_POST
                                    SVC_PRN_ARG_CPOINT  pNandTable  SVC_PRN_ARG_POST
                                    SVC_PRN_ARG_E

                                LOG_CAL_STEREO_API("  -- Header -- ", 0U, 0U);
                                LOG_CAL_STEREO_API("    Enable              : 0x%x",   pNandTable->Header.Enable, 0U);
                                LOG_CAL_STEREO_API("  -- Tbl -- ", 0U, 0U);
                                LOG_CAL_STEREO_API("    Version             : %d.%d.0.0", pNandTable->Tbl.CamWarpInfo.Method.Version[0], pNandTable->Tbl.CamWarpInfo.Method.Version[1]);
                            }
                        }

                        LOG_CAL_STEREO_API("", 0U, 0U);
                        LOG_CAL_STEREO_API("---- Stereo Data Info ----", 0U, 0U);
                        LOG_CAL_STEREO_API("  NumOfTable     : %d", SVC_CalibStereoCtrl.NumOfTable, 0U);

                        if ((SVC_CalibStereoCtrlFlag & 0xCafeU) == 0xCafeU) {
                            const SVC_CALIB_STEREO_INFO_s *pStereoInfo;

                            for (Idx = 0U; Idx < SVC_CalibStereoCtrl.NumOfTable; Idx ++) {
                                pStereoInfo = &(SVC_CalibStereoCtrl.pStereoInfo[Idx]);

                                PRN_CAL_STEREO_API "  ---- Stereo Info[%d] %p -----"
                                    SVC_PRN_ARG_S      SVC_LOG_CAL_STEREO
                                    SVC_PRN_ARG_UINT32 Idx SVC_PRN_ARG_POST
                                    SVC_PRN_ARG_CPOINT  pStereoInfo SVC_PRN_ARG_POST
                                    SVC_PRN_ARG_E

                                LOG_CAL_STEREO_API("    Version             : %d.%d.0.0", SVC_CalibStereoCtrl.pStereoInfo[Idx].CamWarpInfo.Method.Version[0], SVC_CalibStereoCtrl.pStereoInfo[Idx].CamWarpInfo.Method.Version[1]);
                            }
                        }
                    }

                    SVC_CalibStereoCtrlFlag |= SVC_CALIB_STEREO_FLG_SHDW_INIT;
                    LOG_CAL_STEREO_OK("Successful to initial stereo dram shadow!", 0U, 0U);
                }

                PRetVal = AmbaKAL_MutexGive(&(SVC_CalibStereoCtrl.Mutex)); AmbaMisra_TouchUnused(&PRetVal);
            }
        }
    }

    return RetVal;
}
static UINT32 SvcCalib_StereoGetTblIDInSerialNum(const char *pSerialNum, UINT32 *pTblID)
{
    UINT32 RetVal = SVC_NG;
    UINT32 i;
    SVC_CALIB_TBL_INFO_s TblInfo = {0};
    const SVC_CALIB_STEREO_INFO_s    *pOutput = NULL;

    TblInfo.BufSize = sizeof(SVC_CALIB_STEREO_INFO_s);
    TblInfo.DbgMsgOn = 1U;


    for (i = 0U; i < SVC_CalibStereoCtrl.NumOfTable; i++){
        AmbaSvcWrap_MisraMemset(SVC_CalibStereoCtrl.BufBaseStereo, 0, sizeof(SVC_CalibStereoCtrl.BufSizeStereo));
        AmbaMisra_TypeCast(&(TblInfo.pBuf), &(SVC_CalibStereoCtrl.BufBaseStereo));

        RetVal = SvcCalib_ItemTableGet(SVC_CALIB_STEREO_ID, i, &TblInfo);

        AmbaMisra_TypeCast(&(pOutput), &(TblInfo.pBuf));
        if (RetVal == SVC_OK){
            if (SvcWrap_strcmp(pOutput->SerialNumber, pSerialNum) == 0){
                *pTblID = i;
                LOG_CAL_STEREO_OK("Find Table in SerialNum", 0U, 0U);
                LOG_CAL_STEREO_OK(pOutput->SerialNumber, 0U, 0U);
                LOG_CAL_STEREO_OK("Table ID = %d", i, 0U);
                RetVal = SVC_OK;
                break;
            } else {
              RetVal = SVC_NG;
            }
        }
    }

    if (RetVal == SVC_NG){
        LOG_CAL_STEREO_NG("Can't Find Table in SerialNum", 0U, 0U);
    }

    return RetVal;
}
UINT32 SvcCalib_StereoGetTblIDInVinIDSensorID(UINT32 VinID, UINT32 SensorID, UINT32 *pTblID)
{
    #define MAX_SERIAL_NUM_ENTRIES  3U
    typedef struct {
        UINT32      Hash;
        const char  *pSerialNumber;
    } SVC_CALIB_SERIAL_NUMBER_LIST_s;

    UINT32 RetVal = SVC_NG, i;
    UINT32 HashValue;
    static SVC_CALIB_SERIAL_NUMBER_LIST_s SNList[MAX_SERIAL_NUM_ENTRIES] = {
        [0] = {
            .Hash = 0x00000010, .pSerialNumber = "0000"
        },
        [1] = {
            .Hash = 0x01000010, .pSerialNumber = "0004"
        },
        [2] = {
            .Hash = 0x02000010, .pSerialNumber = "0004"
        }
    };
    const char *pSerialNumber = NULL;

    HashValue = ((VinID << 24U) | SensorID);

    for (i = 0; i < MAX_SERIAL_NUM_ENTRIES; i++) {
        if (HashValue == SNList[i].Hash) {
            pSerialNumber = SNList[i].pSerialNumber;
            RetVal = SVC_OK;
            break;
        }
    }

    if (RetVal == SVC_OK){
        RetVal = SvcCalib_StereoGetTblIDInSerialNum(pSerialNumber, pTblID);
    } else {
        AmbaMisra_TouchUnused(&pTblID);
    }

    return RetVal;
}

static UINT32 SvcCalib_StereoWarpTableUpdate(SVC_CALIB_CHANNEL_s *pCalibChan, AMBA_IK_MODE_CFG_s *pImgMode)
{

    UINT32 RetVal = SVC_OK;
    UINT32 Enable = 1U;

    if ((SVC_CalibStereoCtrlFlag & SVC_CALIB_STEREO_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to update stereo warp  table - initial stereo warp module first!", 0U, 0U);
    } else if (pCalibChan == NULL) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to update stereo warp  table - pCalibChan is null", 0U, 0U);
    } else if (pImgMode == NULL) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to update stereo warp  table - pImgMode is null!", 0U, 0U);
    } else {
        SVC_CALIB_TBL_INFO_s TblInfo = {0};
        const SVC_CALIB_STEREO_INFO_s    *pOutput = NULL;
        UINT32 VinID, SensorID, TblID;
        TblInfo.BufSize = sizeof(SVC_CALIB_STEREO_INFO_s);
        TblInfo.DbgMsgOn = 1U;

        (void)pCalibChan;
        (void)pImgMode;

        RetVal = SvcResCfg_GetVinIDOfFovIdx(pImgMode->ContextId, &VinID);
        if (RetVal != SVC_OK) {
            LOG_CAL_STEREO_NG("SvcResCfg_GetVinIDOfFovIdx failed", 0U, 0U);

        }

        RetVal = SvcResCfg_GetSensorIDInVinID(VinID, &SensorID);
        if (RetVal != SVC_OK) {
            LOG_CAL_STEREO_NG("SvcResCfg_GetSensorIDInVinID failed", 0U, 0U);

        }

        RetVal = SvcCalib_StereoGetTblIDInVinIDSensorID(VinID, SensorID, &TblID);
        if (RetVal != SVC_OK) {
            LOG_CAL_STEREO_NG("SvcCalib_StereoGetTblIDInVinIDSensorID failed", 0U, 0U);
        } else {
            AmbaSvcWrap_MisraMemset(SVC_CalibStereoCtrl.BufBaseStereo, 0, sizeof(SVC_CalibStereoCtrl.BufSizeStereo));
            AmbaMisra_TypeCast(&(TblInfo.pBuf), &(SVC_CalibStereoCtrl.BufBaseStereo));
            RetVal |= SvcCalib_ItemTableGet(SVC_CALIB_STEREO_ID, TblID, &TblInfo);

            AmbaMisra_TypeCast(&(pOutput), &(TblInfo.pBuf));


            if (RetVal == SVC_OK) {
                LOG_CAL_STEREO_OK("SvcCalib_ItemTableGet successfully", 0U, 0U);

                RetVal = Svc_ApplyStereoWarpV2(pImgMode->ContextId, &pOutput->CamWarpInfo, Enable);

            } else {
                LOG_CAL_STEREO_NG("SvcCalib_ItemTableGet failed", 0U, 0U);
            }
        }
    }

    return RetVal;

}

static UINT32 SvcCalib_StereoTableDataGet(UINT32 TblID, const SVC_CALIB_TBL_INFO_s *pTblInfo)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibStereoCtrlFlag & SVC_CALIB_STEREO_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to get stereo table - Need to initial stereo control module first!", 0U, 0U);
    } else if ((SVC_CalibStereoCtrlFlag & SVC_CALIB_STEREO_FLG_SHDW_INIT) == 0U) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to get stereo table - Need to initial stereo dram shadow first!", 0U, 0U);
    } else if (pTblInfo == NULL) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to get stereo table - output table info should not null!", 0U, 0U);
    } else {
        if (KAL_ERR_NONE != AmbaKAL_MutexTake(&(SVC_CalibStereoCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
            RetVal = SVC_NG;
            LOG_CAL_STEREO_NG("Fail to get stereo table - take mutex fail!", 0U, 0U);
        } else {
            const SVC_CALIB_STEREO_NAND_TBL_s *pNandTable = NULL;

            if (TblID >= SVC_CalibStereoCtrl.NumOfTable) {
                RetVal = SVC_NG;
                LOG_CAL_STEREO_NG("Fail to get stereo table - request table index(%d) is out-of range(0 ~ %d)!", TblID, SVC_CalibStereoCtrl.NumOfTable - 1U);
            } else {
                //SVC_CALIB_STEREO_INFO_s    *pOutput = &(SVC_CalibStereoCtrl.pStereoInfo[TblID]);
                SVC_CALIB_STEREO_INFO_s    *pOutput = NULL;
                const AMBA_IK_GRID_POINT_s *pWarpTable = NULL;
                ULONG                     WarpTableAddr = 0U;

                pNandTable = &(SVC_CalibStereoCtrl.pNandTable[TblID]);

                {
                    //AmbaMisra_TypeCast32(&(pTblInfo->pBuf), &(pOutput));
                    AmbaMisra_TypeCast(&(pOutput), &(pTblInfo->pBuf));

                    pWarpTable = pOutput->CamWarpInfo.Method.V2100.WarpTable;
                    AmbaMisra_TypeCast(&WarpTableAddr, &pWarpTable);

                    RetVal = AmbaWrap_memcpy(pOutput->SerialNumber, pNandTable->Tbl.SerialNumber, sizeof(pNandTable->Tbl.SerialNumber));
                    RetVal = AmbaWrap_memcpy(&pOutput->CamWarpInfo, &pNandTable->Tbl.CamWarpInfo, sizeof(pNandTable->Tbl.CamWarpInfo));

                    if ((SVC_CalibStereoCtrl.EnableDebugMsg > 0U) || (pTblInfo->DbgMsgOn > 0U)) {
                        LOG_CAL_STEREO_API("", 0U, 0U);
                        LOG_CAL_STEREO_API("  Cmd == %d", pOutput->Cmd, 0U);
                        LOG_CAL_STEREO_API("---- Get Stereo Data ----", 0U, 0U);
                        PRN_CAL_STEREO_API "  Serial Number %s  "
                            SVC_PRN_ARG_S SVC_LOG_CAL_STEREO
                            SVC_PRN_ARG_CSTR pOutput->SerialNumber SVC_PRN_ARG_POST
                            SVC_PRN_ARG_E
                        PRN_CAL_STEREO_API "  Version : %d.%d.%d.%d  "
                            SVC_PRN_ARG_S SVC_LOG_CAL_STEREO
                            SVC_PRN_ARG_UINT32 pOutput->CamWarpInfo.Method.Version[3] SVC_PRN_ARG_POST
                            SVC_PRN_ARG_UINT32 pOutput->CamWarpInfo.Method.Version[2] SVC_PRN_ARG_POST
                            SVC_PRN_ARG_UINT32 pOutput->CamWarpInfo.Method.Version[1] SVC_PRN_ARG_POST
                            SVC_PRN_ARG_UINT32 pOutput->CamWarpInfo.Method.Version[0] SVC_PRN_ARG_POST
                            SVC_PRN_ARG_E

                        /* Version 2100 */
                        if ((pOutput->CamWarpInfo.Method.Version[0] == 2U) && (pOutput->CamWarpInfo.Method.Version[1] == 1U) && (pOutput->CamWarpInfo.Method.Version[2] == 0U) && (pOutput->CamWarpInfo.Method.Version[3] == 0U)) {
                            LOG_CAL_STEREO_API("---- AMBA_STU_WARP_INFO_HEADER_BASE_t ----", 0U, 0U);
                            PRN_CAL_STEREO_API "  HeaderOffset = %d  " SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_UINT32 pOutput->CamWarpInfo.Method.V2100.HBase.HeaderOffset SVC_PRN_ARG_POST SVC_PRN_ARG_E
                            PRN_CAL_STEREO_API "  TableOffset  = %d  " SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_UINT32 pOutput->CamWarpInfo.Method.V2100.HBase.TableOffset SVC_PRN_ARG_POST SVC_PRN_ARG_E
                            PRN_CAL_STEREO_API "  ExifOffset   = %d  " SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_UINT32 pOutput->CamWarpInfo.Method.V2100.HBase.ExifOffset  SVC_PRN_ARG_POST SVC_PRN_ARG_E

                            LOG_CAL_STEREO_API("---- AMBA_STU_WARP_INFO_HEADER_2100_t ----", 0U, 0U);
                            PRN_CAL_STEREO_API "  HorizontalTilesNum = %d  " SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_UINT16 pOutput->CamWarpInfo.Method.V2100.Header.HorizontalTilesNum  SVC_PRN_ARG_POST SVC_PRN_ARG_E
                            PRN_CAL_STEREO_API "  VerticalTilesNum   = %d  " SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_UINT16 pOutput->CamWarpInfo.Method.V2100.Header.VerticalTilesNum    SVC_PRN_ARG_POST SVC_PRN_ARG_E
                            PRN_CAL_STEREO_API "  TileWidthQ16       = %d  " SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_UINT32 pOutput->CamWarpInfo.Method.V2100.Header.TileWidthQ16        SVC_PRN_ARG_POST SVC_PRN_ARG_E
                            PRN_CAL_STEREO_API "  TileHeightQ16      = %d  " SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_UINT32 pOutput->CamWarpInfo.Method.V2100.Header.TileHeightQ16       SVC_PRN_ARG_POST SVC_PRN_ARG_E
                            PRN_CAL_STEREO_API "  WarpMode           = %d  " SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_UINT32 pOutput->CamWarpInfo.Method.V2100.Header.WarpMode.Enable2Pass SVC_PRN_ARG_POST SVC_PRN_ARG_E
                            PRN_CAL_STEREO_API "  Id                 = %d  " SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_UINT32 pOutput->CamWarpInfo.Method.V2100.Header.Id                  SVC_PRN_ARG_POST SVC_PRN_ARG_E
                            PRN_CAL_STEREO_API "  Hash               = %d  " SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_UINT32 pOutput->CamWarpInfo.Method.V2100.Header.Hash                SVC_PRN_ARG_POST SVC_PRN_ARG_E
                            PRN_CAL_STEREO_API "  TableOriginX       = %d  " SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_UINT32 pOutput->CamWarpInfo.Method.V2100.Header.TableOriginX        SVC_PRN_ARG_POST SVC_PRN_ARG_E
                            PRN_CAL_STEREO_API "  TableOriginY       = %d  " SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_UINT32 pOutput->CamWarpInfo.Method.V2100.Header.TableOriginY        SVC_PRN_ARG_POST SVC_PRN_ARG_E
                            PRN_CAL_STEREO_API "  VsyncDelay         = %d  " SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_UINT16 pOutput->CamWarpInfo.Method.V2100.Header.VsyncDelay          SVC_PRN_ARG_POST SVC_PRN_ARG_E
                            PRN_CAL_STEREO_API "  Reserved           = %d  " SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_UINT16 pOutput->CamWarpInfo.Method.V2100.Header.Reserved            SVC_PRN_ARG_POST SVC_PRN_ARG_E
                            PRN_CAL_STEREO_API "  InputWidth         = %d  " SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_UINT32 pOutput->CamWarpInfo.Method.V2100.Header.InputWidth          SVC_PRN_ARG_POST SVC_PRN_ARG_E
                            PRN_CAL_STEREO_API "  InputHeight        = %d  " SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_UINT32 pOutput->CamWarpInfo.Method.V2100.Header.InputHeight         SVC_PRN_ARG_POST SVC_PRN_ARG_E

                            LOG_CAL_STEREO_API("---- AMBA_IK_GRID_POINT_s ----", 0U, 0U);
                            PRN_CAL_STEREO_API "WarpTable Addr       = 0x%x  " SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_UINT32 (UINT32)WarpTableAddr SVC_PRN_ARG_POST SVC_PRN_ARG_E
                        } else {
                            SvcLog_NG(SVC_LOG_CAL_STEREO, "Unkonw version", 0U, 0U);
                        }

                    }

                    PRN_CAL_STEREO_OK "Success to get stereo table 0x%08x by TableID(%d)!"
                        SVC_PRN_ARG_S SVC_LOG_CAL_STEREO
                        SVC_PRN_ARG_CPOINT &(SVC_CalibStereoCtrl.pNandTable[TblID]) SVC_PRN_ARG_POST
                        SVC_PRN_ARG_UINT32 TblID SVC_PRN_ARG_POST
                        SVC_PRN_ARG_E
                }
            }

            PRetVal = AmbaKAL_MutexGive(&(SVC_CalibStereoCtrl.Mutex)); AmbaMisra_TouchUnused(&PRetVal);
        }
    }

    return RetVal;
}

static UINT32 SvcCalib_StereoTableDataSet(UINT32 TblID, const SVC_CALIB_TBL_INFO_s *pTblInfo)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibStereoCtrlFlag & SVC_CALIB_STEREO_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to set stereo table - Need to initial stereo control module first!", 0U, 0U);
    } else if ((SVC_CalibStereoCtrlFlag & SVC_CALIB_STEREO_FLG_SHDW_INIT) == 0U) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to set stereo table - Need to initial stereo dram shadow first!", 0U, 0U);
    } else if (TblID >= SVC_CalibStereoCtrl.NumOfTable) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to set stereo table - TableID(%d) out-of range(%d)!", TblID, SVC_CalibStereoCtrl.NumOfTable);
    } else if (pTblInfo == NULL) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to set stereo table - output table info should not null!", 0U, 0U);
    } else if (pTblInfo->pBuf == NULL) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to set stereo table - input table buffer should not null!", 0U, 0U);
    } else if (pTblInfo->BufSize < sizeof(SVC_CALIB_STEREO_INFO_s)) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to set stereo table - input table buffer size is too small! 0x%x/0x%x", pTblInfo->BufSize, sizeof(SVC_CALIB_STEREO_INFO_s));
    } else {

        if (KAL_ERR_NONE != AmbaKAL_MutexTake(&(SVC_CalibStereoCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
            RetVal = SVC_NG;
            LOG_CAL_STEREO_NG("Fail to set stereo table - take mutex fail!", 0U, 0U);
        } else {
            SVC_CALIB_STEREO_NAND_TBL_s *pNandTable = &(SVC_CalibStereoCtrl.pNandTable[TblID]);
            UINT32 IsNewData = 0U;
            const SVC_CALIB_STEREO_INFO_s *pInput = NULL;

            AmbaMisra_TypeCast(&(pInput), &(pTblInfo->pBuf));

            if (SVC_CalibStereoCtrl.pNandTable[TblID].Header.Enable == 0U) {
                IsNewData = 1U;
            }

            if (pInput == NULL) {
                RetVal = SVC_NG;
                LOG_CAL_STEREO_NG("Fail to set stereo table - invalid input table data!", 0U, 0U);
            } else {
                AmbaSvcWrap_MisraMemset(pNandTable, 0, sizeof(SVC_CALIB_STEREO_NAND_TBL_s));
                pNandTable->Header.Enable = 1U;
                RetVal = AmbaWrap_memcpy(pNandTable->Tbl.SerialNumber, pInput->SerialNumber, sizeof(pNandTable->Tbl.SerialNumber));
                RetVal = AmbaWrap_memcpy(&pNandTable->Tbl.CamWarpInfo, &pInput->CamWarpInfo, sizeof(pNandTable->Tbl.CamWarpInfo));

                // Update nand header
                if (SVC_CalibStereoCtrl.pNandHeader->Enable == 0U) {
                    SVC_CalibStereoCtrl.pNandHeader->Enable = 1U;
                }
                if (SVC_CalibStereoCtrl.pNandHeader->DebugMsgOn == 0U) {
                    SVC_CalibStereoCtrl.pNandHeader->DebugMsgOn = SVC_CalibStereoCtrl.EnableDebugMsg;
                }
                if (SVC_CalibStereoCtrl.pNandHeader->DebugMsgOn == 0U) {
                    SVC_CalibStereoCtrl.pNandHeader->DebugMsgOn = pTblInfo->DbgMsgOn;
                }
                if (SVC_CalibStereoCtrl.EnableDebugMsg == 0U) {
                    SVC_CalibStereoCtrl.EnableDebugMsg = pTblInfo->DbgMsgOn;
                }
                if (IsNewData > 0U) {
                    SVC_CalibStereoCtrl.pNandHeader->NumOfTable += 1U;
                }

                if (SVC_CalibStereoCtrl.EnableDebugMsg > 0U) {
                    LOG_CAL_STEREO_API("", 0U, 0U);
                    PRN_CAL_STEREO_API "---- Set stereo Header (0x%08x) ----" SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_CPOINT SVC_CalibStereoCtrl.pNandHeader SVC_PRN_ARG_POST SVC_PRN_ARG_E
                    PRN_CAL_STEREO_API "            Enable : %d" SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_UINT32 SVC_CalibStereoCtrl.pNandHeader->Enable SVC_PRN_ARG_POST SVC_PRN_ARG_E
                    PRN_CAL_STEREO_API "        NumOfTable : %d" SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_UINT32 SVC_CalibStereoCtrl.pNandHeader->NumOfTable SVC_PRN_ARG_POST SVC_PRN_ARG_E
                    PRN_CAL_STEREO_API "        DebugMsgOn : %d" SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_UINT32 SVC_CalibStereoCtrl.pNandHeader->DebugMsgOn SVC_PRN_ARG_POST SVC_PRN_ARG_E

                    LOG_CAL_STEREO_API("", 0U, 0U);
                    LOG_CAL_STEREO_API("---- Set Stereo Data ----", 0U, 0U);
                    LOG_CAL_STEREO_API("        Version    : %d.%d.0.0", pNandTable->Tbl.CamWarpInfo.Method.Version[0], pNandTable->Tbl.CamWarpInfo.Method.Version[1]);
                }

                /* Update to NAND */
                /* FIXME */
                if (TblID == 1U){
                    RetVal = SvcCalib_DataSave(SVC_CALIB_STEREO_ID);
                }
                if (RetVal != OK) {
                    LOG_CAL_STEREO_NG("Fail to save stereo to nand!", 0U, 0U);
                }
            }

            PRetVal = AmbaKAL_MutexGive(&(SVC_CalibStereoCtrl.Mutex)); AmbaMisra_TouchUnused(&PRetVal);
        }
    }

    return RetVal;
}

static UINT32 SvcCalib_StereoShellCfgDbgMsg(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibStereoCtrlFlag & SVC_CALIB_STEREO_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to switch stitch blend debug msg - initial stitch blend module first!", 0U, 0U);
    }

    if (ArgCount < 3U) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to switch stitch blend debug msg - Argc should >= 3", 0U, 0U);
    }

    if (pArgVector == NULL) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to switch stitch blend debug msg - Argv should not null!", 0U, 0U);
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 DbgMsgOn = 0U;

        PRetVal = SvcWrap_strtoul(pArgVector[3U],  &DbgMsgOn ); AmbaMisra_TouchUnused(&PRetVal);

        SVC_CalibStereoCtrl.EnableDebugMsg = DbgMsgOn;

        LOG_CAL_STEREO_API("", 0U, 0U);
        LOG_CAL_STEREO_API("------ Calibration Stereo Debug Msg On/Off %d ------", SVC_CalibStereoCtrl.EnableDebugMsg, 0U);

    } else {
        AmbaMisra_TouchUnused(&ArgCount);
    }

    return RetVal;
}


static void SvcCalib_StereoShellCfgDbgMsgU(void)
{

    PRN_CAL_STEREO_API "  cfg_dbg_msg           :" SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_E
    PRN_CAL_STEREO_API "  ------------------------------------------------------" SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_E
    PRN_CAL_STEREO_API "    [Enable]            : 0: disable debug message" SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_E
    PRN_CAL_STEREO_API "                        : 1: enable debug message" SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_E
    PRN_CAL_STEREO_API ""SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_E
}


static UINT32 SvcCalib_StereoShellLoadFromSd(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibStereoCtrlFlag & SVC_CALIB_STEREO_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to switch stitch blend debug msg - initial stitch blend module first!", 0U, 0U);
    }

    if (ArgCount < 6U) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to switch stitch blend debug msg - Argc should >= 6", 0U, 0U);
    }

    if (pArgVector == NULL) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to switch stitch blend debug msg - Argv should not null!", 0U, 0U);
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        static UINT8 FileInputBuf[0x20000U];
        static SVC_CALIB_STEREO_INFO_s Tbl;
        static SVC_CALIB_TBL_INFO_s TblInfo = {0};

        const char *pSerialNumber = pArgVector[3U];
        const char *pFileName     = pArgVector[4U];
        UINT32 TblIdx       = 0U;
        AMBA_FS_FILE *pFile = NULL;

        AmbaSvcWrap_MisraMemset(&Tbl, 0, sizeof(SVC_CALIB_STEREO_INFO_s));

        TblInfo.pBuf    = &Tbl;
        TblInfo.BufSize = sizeof(SVC_CALIB_STEREO_INFO_s);

        RetVal = SvcWrap_strtoul(pArgVector[5U], &TblIdx);

        LOG_CAL_STEREO_DBG("-- Serial Number --", 0U, 0U);
        LOG_CAL_STEREO_DBG(pSerialNumber, 0U, 0U);

        RetVal = AmbaWrap_memcpy(Tbl.SerialNumber, pSerialNumber, sizeof(char) * SvcWrap_strlen(pSerialNumber));

        LOG_CAL_STEREO_DBG("-- FileName --", 0U, 0U);
        LOG_CAL_STEREO_DBG(pFileName, 0U, 0U);

        LOG_CAL_STEREO_DBG("-- TblIdx --", 0U, 0U);
        LOG_CAL_STEREO_DBG("Idx = %d", TblIdx, 0U);

        if (SVC_OK == AmbaFS_FileOpen(pFileName, "r", &pFile)) {
            AMBA_FS_FILE_INFO_s FileInfo = {0};

            LOG_CAL_STEREO_DBG("File Open", 0U, 0U);

            if (SVC_OK == AmbaFS_GetFileInfo(pFileName, &FileInfo)) {
                if (sizeof(FileInputBuf) > FileInfo.Size) {
                    UINT32 RdIdx = 0U;

                    LOG_CAL_STEREO_DBG("FileInfo.Size = %u", (UINT32) FileInfo.Size, 0U);

                    while (0U == AmbaFS_FileEof(pFile)) {   /* (0) not end of file, (1) end of file, (-1) not file open */
                        UINT32 ReadCount = 0U;

                        if(SVC_OK == AmbaFS_FileRead(&FileInputBuf[RdIdx], 1U, 512U, pFile, &ReadCount)) {
                            // LOG_CAL_STEREO_DBG("FileRead RdIdx = %d ~ %d", RdIdx, RdIdx + ReadCount);
                            RdIdx += ReadCount;
                        }
                    }

                    RetVal = AmbaWrap_memcpy(&Tbl.CamWarpInfo, FileInputBuf, sizeof(AMBA_STU_STEREO_CAM_WARP_INFO_s));

                    if (SVC_OK == SvcCalib_ItemTableSet(SVC_CALIB_STEREO_ID, TblIdx, &TblInfo)) {
                        LOG_CAL_STEREO_OK("SvcCalib_ItemTableSet successfully", 0U, 0U);
                    } else {
                        LOG_CAL_STEREO_NG("SvcCalib_ItemTableSet failed", 0U, 0U);
                    }
                } else {
                    LOG_CAL_STEREO_NG("FileInputBuf too small", 0U, 0U);
                }
            }
        }

        if (SVC_OK == AmbaFS_FileClose(pFile)) {
            LOG_CAL_STEREO_DBG("File Close", 0U, 0U);
        }

    } else {
        AmbaMisra_TouchUnused(&ArgCount);
    }

    return RetVal;
}

static void SvcCalib_StereoShellLoadFromSdU(void)
{
    PRN_CAL_STEREO_API "  load_from_sd          :" SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_E
    PRN_CAL_STEREO_API "  ------------------------------------------------------" SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_E
    PRN_CAL_STEREO_API "    [SerialNumber]      : Camera Module Serial Number 1 ~ 16 character. ex., aabbccdd1234" SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_E
    PRN_CAL_STEREO_API "    [FileName]          : Calibration data. ex., C:\\warp_table_R.bin "                    SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_E
    PRN_CAL_STEREO_API "    [TblIdx]            : Calibration Table Index. Should be < %d"                         SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_UINT32 SVC_CALIB_STEREO_TBL_NUM SVC_PRN_ARG_POST SVC_PRN_ARG_E
    PRN_CAL_STEREO_API ""SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_E
}

static UINT32 SvcCalib_StereoShellGetTbl(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibStereoCtrlFlag & SVC_CALIB_STEREO_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to switch stitch blend debug msg - initial stitch blend module first!", 0U, 0U);
    }

    if (ArgCount < 3U) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to switch stitch blend debug msg - Argc should >= 3", 0U, 0U);
    }

    if (pArgVector == NULL) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to switch stitch blend debug msg - Argv should not null!", 0U, 0U);
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 TblIdx = 0U;
        SVC_CALIB_TBL_INFO_s TblInfo = {0};
        TblInfo.BufSize = sizeof(SVC_CALIB_STEREO_INFO_s);

        PRetVal = SvcWrap_strtoul(pArgVector[3U],  &TblIdx ); AmbaMisra_TouchUnused(&PRetVal);

        LOG_CAL_STEREO_DBG("-- TblIdx --", 0U, 0U);
        LOG_CAL_STEREO_DBG("Idx = %d", TblIdx, 0U);

        TblInfo.DbgMsgOn = 1U;

        AmbaSvcWrap_MisraMemset(SVC_CalibStereoCtrl.BufBaseStereo, 0, sizeof(SVC_CalibStereoCtrl.BufSizeStereo));
        AmbaMisra_TypeCast(&(TblInfo.pBuf), &(SVC_CalibStereoCtrl.BufBaseStereo));

        if (SVC_OK == SvcCalib_ItemTableGet(SVC_CALIB_STEREO_ID, TblIdx, &TblInfo)) {
            LOG_CAL_STEREO_OK("SvcCalib_ItemTableGet successfully", 0U, 0U);
        } else {
            LOG_CAL_STEREO_NG("SvcCalib_ItemTableGet failed", 0U, 0U);
        }

    } else {
        AmbaMisra_TouchUnused(&ArgCount);
    }

    return RetVal;
}

static void SvcCalib_StereoShellGetTblU(void)
{
    PRN_CAL_STEREO_API "  get_tbl               :" SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_E
    PRN_CAL_STEREO_API "  ------------------------------------------------------" SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_E
    PRN_CAL_STEREO_API "    [TblIdx]            : Get Calib Table. TblIdx should be < %d" SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_UINT32 SVC_CALIB_STEREO_TBL_NUM SVC_PRN_ARG_POST SVC_PRN_ARG_E
    PRN_CAL_STEREO_API ""SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_E
}

static UINT32 SvcCalib_StereoShellSetWarp(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal = SVC_OK;

    if ((SVC_CalibStereoCtrlFlag & SVC_CALIB_STEREO_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to switch stitch blend debug msg - initial stitch blend module first!", 0U, 0U);
    }

    if (ArgCount < 3U) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to switch stitch blend debug msg - Argc should >= 3", 0U, 0U);
    }

    if (pArgVector == NULL) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to switch stitch blend debug msg - Argv should not null!", 0U, 0U);
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        static UINT32 FOVIdx = 0U;
        UINT32 VinID, SensorID, TblID;
        static UINT32 Enable = 0U;
        SVC_CALIB_TBL_INFO_s TblInfo = {0};
        const SVC_CALIB_STEREO_INFO_s *pOutput = NULL;

        TblInfo.BufSize = sizeof(SVC_CALIB_STEREO_INFO_s);
        TblInfo.DbgMsgOn = 1U;

        PRetVal |= SvcWrap_strtoul(pArgVector[3U],  &FOVIdx ); AmbaMisra_TouchUnused(&PRetVal);
        PRetVal |= SvcWrap_strtoul(pArgVector[4U],  &Enable ); AmbaMisra_TouchUnused(&PRetVal);

        LOG_CAL_STEREO_DBG("-- FOVIdx --", 0U, 0U);
        LOG_CAL_STEREO_DBG("Idx = %d, Enable = %d", FOVIdx, Enable);

        RetVal = SvcResCfg_GetVinIDOfFovIdx(FOVIdx, &VinID);
        if (RetVal != SVC_OK) {
            LOG_CAL_STEREO_NG("SvcResCfg_GetVinIDOfFovIdx failed", 0U, 0U);
        }

        RetVal = SvcResCfg_GetSensorIDInVinID(VinID, &SensorID);
        if (RetVal != SVC_OK) {
            LOG_CAL_STEREO_NG("SvcResCfg_GetSensorIDInVinID failed", 0U, 0U);
        }

        RetVal = SvcCalib_StereoGetTblIDInVinIDSensorID(VinID, SensorID, &TblID);
        if (RetVal != SVC_OK) {
            LOG_CAL_STEREO_NG("SvcCalib_StereoGetTblIDInVinIDSensorID failed", 0U, 0U);
        } else {
            AmbaSvcWrap_MisraMemset(SVC_CalibStereoCtrl.BufBaseStereo, 0, sizeof(SVC_CalibStereoCtrl.BufSizeStereo));
            AmbaMisra_TypeCast(&(TblInfo.pBuf), &(SVC_CalibStereoCtrl.BufBaseStereo));

            RetVal |= SvcCalib_ItemTableGet(SVC_CALIB_STEREO_ID, TblID, &TblInfo);

            AmbaMisra_TypeCast(&(pOutput), &(TblInfo.pBuf));

            if (RetVal == SVC_OK) {
                LOG_CAL_STEREO_OK("SvcCalib_ItemTableGet successfully", 0U, 0U);

                RetVal = Svc_ApplyStereoWarpV2(FOVIdx, &pOutput->CamWarpInfo, Enable);
            } else {
                LOG_CAL_STEREO_NG("SvcCalib_ItemTableGet failed", 0U, 0U);
            }
        }
    } else {
        AmbaMisra_TouchUnused(&ArgCount);
    }

    return RetVal;
}

static void SvcCalib_StereoShellSetWarpU(void)
{
    PRN_CAL_STEREO_API "  set_warp               :" SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_E
    PRN_CAL_STEREO_API "  ------------------------------------------------------" SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_E
    PRN_CAL_STEREO_API "    [FOV ID]            : Get Calib Table. TblIdx should be < %d" SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_UINT32 SVC_CALIB_STEREO_TBL_NUM SVC_PRN_ARG_POST SVC_PRN_ARG_E
    PRN_CAL_STEREO_API "    [Enable]    "SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_E
    PRN_CAL_STEREO_API ""SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_E
}

static void SvcCalib_StereoShellEntryInit(void)
{
    if ((SVC_CalibStereoCtrlFlag & SVC_CALIB_STEREO_FLG_SHELLINIT) == 0U) {
        AmbaSvcWrap_MisraMemset(CalibStereoShellFunc, 0, sizeof(CalibStereoShellFunc));

        CalibStereoShellFunc[0U] = (SVC_CALIB_STEREO_SHELL_FUNC_s) { 1U, "cfg_dbg_msg",     SvcCalib_StereoShellCfgDbgMsg,  SvcCalib_StereoShellCfgDbgMsgU  };
        CalibStereoShellFunc[1U] = (SVC_CALIB_STEREO_SHELL_FUNC_s) { 1U, "load_from_sd",    SvcCalib_StereoShellLoadFromSd, SvcCalib_StereoShellLoadFromSdU };
        CalibStereoShellFunc[2U] = (SVC_CALIB_STEREO_SHELL_FUNC_s) { 1U, "get_tbl",         SvcCalib_StereoShellGetTbl,     SvcCalib_StereoShellGetTblU     };
        CalibStereoShellFunc[3U] = (SVC_CALIB_STEREO_SHELL_FUNC_s) { 1U, "set_warp",         SvcCalib_StereoShellSetWarp,     SvcCalib_StereoShellSetWarpU     };

        SVC_CalibStereoCtrlFlag |= SVC_CALIB_STEREO_FLG_SHELLINIT;
    }
}

UINT32 SvcCalib_StereoShellFunc(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_NG, PRetVal;

    SvcCalib_StereoShellEntryInit();

    if (ArgCount > 0U) {
        UINT32 ShellIdx;
        UINT32 ShellCount = (UINT32)(sizeof(CalibStereoShellFunc)) / (UINT32)(sizeof(CalibStereoShellFunc[0]));

        for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
            if ((CalibStereoShellFunc[ShellIdx].pFunc != NULL) && (CalibStereoShellFunc[ShellIdx].Enable > 0U)) {
                if (0 == SvcWrap_strcmp(pArgVector[2U], CalibStereoShellFunc[ShellIdx].ShellCmdName)) {
                    PRetVal = (CalibStereoShellFunc[ShellIdx].pFunc)(ArgCount, pArgVector);
                    AmbaMisra_TouchUnused(&PRetVal);

                    RetVal = SVC_OK;
                    break;
                }
            }
        }

        if (RetVal != SVC_OK) {
            SvcCalib_StereoShellUsage();
        }
    }

    return RetVal;
}

static void SvcCalib_StereoShellUsage(void)
{
    UINT32 ShellIdx;
    UINT32 ShellCount = (UINT32)(sizeof(CalibStereoShellFunc)) / (UINT32)(sizeof(CalibStereoShellFunc[0]));

    LOG_CAL_STEREO_API("", 0U, 0U);
    LOG_CAL_STEREO_API("====== Stitch Blend Command Usage ======", 0U, 0U);

    for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
        if ((CalibStereoShellFunc[ShellIdx].pFunc != NULL) && (CalibStereoShellFunc[ShellIdx].Enable > 0U)) {
            if (CalibStereoShellFunc[ShellIdx].pUsageFunc == NULL) {
                SVC_WRAP_PRINT "  %s" SVC_PRN_ARG_S SVC_LOG_CAL_STEREO SVC_PRN_ARG_CSTR CalibStereoShellFunc[ShellIdx].ShellCmdName SVC_PRN_ARG_POST SVC_PRN_ARG_E
            } else {
                (CalibStereoShellFunc[ShellIdx].pUsageFunc)();
            }
        }
    }
}

static void SvcCalib_StereoCmdHdlrInit(void)
{
    if ((SVC_CalibStereoCtrlFlag & SVC_CALIB_STEREO_FLG_CMD_INIT) == 0U) {
        AmbaSvcWrap_MisraMemset(SvcCalibStereoCmdHdlr, 0, sizeof(SvcCalibStereoCmdHdlr));
        SvcCalibStereoCmdHdlr[SVC_CALIB_CMD_MEM_QUERY      ] = (SVC_CALIB_STEREO_CMD_HDLR_s) {1, SVC_CALIB_CMD_MEM_QUERY,       SvcCalib_StereoCmdMemQuery   };
        SvcCalibStereoCmdHdlr[SVC_CALIB_CMD_DATA_LOAD      ] = (SVC_CALIB_STEREO_CMD_HDLR_s) {1, SVC_CALIB_CMD_DATA_LOAD,       NULL                          };
        SvcCalibStereoCmdHdlr[SVC_CALIB_CMD_ITEM_DATA_INIT ] = (SVC_CALIB_STEREO_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_DATA_INIT,  SvcCalib_StereoCmdInit       };
        SvcCalibStereoCmdHdlr[SVC_CALIB_CMD_ITEM_TBL_CFG   ] = (SVC_CALIB_STEREO_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_TBL_CFG,    NULL                          };
        SvcCalibStereoCmdHdlr[SVC_CALIB_CMD_ITEM_TBL_UPDATE] = (SVC_CALIB_STEREO_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_TBL_UPDATE, SvcCalib_StereoCmdTblUpdate                          };
        SvcCalibStereoCmdHdlr[SVC_CALIB_CMD_ITEM_TBL_GET   ] = (SVC_CALIB_STEREO_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_TBL_GET,    SvcCalib_StereoCmdTblDataGet };
        SvcCalibStereoCmdHdlr[SVC_CALIB_CMD_ITEM_TBL_SET   ] = (SVC_CALIB_STEREO_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_TBL_SET,    SvcCalib_StereoCmdTblDataSet };

        SVC_CalibStereoCtrlFlag |= SVC_CALIB_STEREO_FLG_CMD_INIT;
    }
}

static UINT32 SvcCalib_StereoCmdMemQuery(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    if (pParam1 == NULL) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to query stereo memory - input stereo table number should not zero.", 0U, 0U);
    }

    if (pParam2 == NULL) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to query stereo memory - output stereo shadow buffer size should not null!", 0U, 0U);
    }

    if (pParam3 == NULL) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to query stereo memory - output stereo working buffer size should not null!", 0U, 0U);
    }

    if (pParam4 == NULL) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to query stereo memory - stereo calculation buffer size should not null!", 0U, 0U);
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

        RetVal = SvcCalib_StereoMemQuery(NomOfTable, pShadowBufSize, pWorkingBufSize, pCalcMemSize);
        if (RetVal != SVC_OK) {
            LOG_CAL_STEREO_NG("Fail to query stereo memory - query memory fail!", 0U, 0U);
        }

        AmbaMisra_TouchUnused(pParam1);
        AmbaMisra_TouchUnused(pParam2);
        AmbaMisra_TouchUnused(pParam3);
        AmbaMisra_TouchUnused(pParam4);
        AmbaMisra_TouchUnused(pU32Val);
    } else {
        AmbaMisra_TouchUnused(SvcCalibStereoCmdHdlr);
    }

    return RetVal;
}

static UINT32 SvcCalib_StereoCmdInit(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    if (pParam1 == NULL) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to init stereo - input calib object should not null!", 0U, 0U);
    }

    if (RetVal == SVC_OK) {
        RetVal = SvcCalib_StereoCreate();
        if (RetVal != SVC_OK) {
            LOG_CAL_STEREO_NG("Fail to init stereo - create module fail!", 0U, 0U);
        }
    }

    if (RetVal == SVC_OK) {
        const SVC_CALIB_OBJ_s *pCalObj;

        AmbaMisra_TypeCast(&pCalObj, &pParam1);
        if (pCalObj == NULL) {
            RetVal = SVC_NG;
            LOG_CAL_STEREO_NG("Fail to init stereo - invalid calib obj!", 0U, 0U);
        } else {
            RetVal = SvcCalib_StereoShadowInit(pCalObj);
            if (RetVal != SVC_OK) {
                LOG_CAL_STEREO_NG("Fail to init stereo - shadow initial fail!", 0U, 0U);
            } else {
                SVC_CALIB_CALC_CB_f CbFunc = NULL;
                AmbaMisra_TypeCast(&(CbFunc), &(pParam2));
                // SVC_CalibStereoCtrl.CalcCbFunc = CbFunc;
            }
        }
    } else {
        AmbaMisra_TouchUnused(pParam1);
        AmbaMisra_TouchUnused(pParam2);
        AmbaMisra_TouchUnused(pParam3);
        AmbaMisra_TouchUnused(pParam4);
    }

    return RetVal;
}

static UINT32 SvcCalib_StereoCmdTblDataGet(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;
    UINT32 *pU32Val = NULL;
    const SVC_CALIB_TBL_INFO_s *pTblInfo;

    AmbaMisra_TypeCast(&(pU32Val), &(pParam1));
    AmbaMisra_TypeCast(&(pTblInfo), &(pParam2));

    if (pU32Val == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_CAL_STEREO, "Fail to get table data - input table id should not null!", 0U, 0U);
    } else {
        RetVal = SvcCalib_StereoTableDataGet(*pU32Val, pTblInfo);
    }

    AmbaMisra_TouchUnused(pParam1);
    AmbaMisra_TouchUnused(pParam2);
    AmbaMisra_TouchUnused(pParam3);
    AmbaMisra_TouchUnused(pParam4);
    AmbaMisra_TouchUnused(pU32Val);

    return RetVal;

}
static UINT32 SvcCalib_StereoCmdTblDataSet(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;
    UINT32 *pU32Val = NULL;
    const SVC_CALIB_TBL_INFO_s *pTblInfo;

    AmbaMisra_TypeCast(&(pU32Val), &(pParam1));
    AmbaMisra_TypeCast(&(pTblInfo), &(pParam2));

    if (pU32Val == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_CAL_STEREO, "Fail to set table data - input table id should not null!", 0U, 0U);
    } else {
        RetVal = SvcCalib_StereoTableDataSet(*pU32Val, pTblInfo);
    }

    AmbaMisra_TouchUnused(pParam1);
    AmbaMisra_TouchUnused(pParam2);
    AmbaMisra_TouchUnused(pParam3);
    AmbaMisra_TouchUnused(pParam4);
    AmbaMisra_TouchUnused(pU32Val);

    return RetVal;
}

static UINT32 SvcCalib_StereoCmdTblUpdate(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;
    if (pParam1 == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_CAL_STEREO, "Fail to update stereo wrap table - input calib channel should not null!", 0U, 0U);
    }

    if (pParam2 == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_CAL_STEREO, "Fail to update stereo wrap table - input IK mode cfg should not null!", 0U, 0U);
    }

    if (RetVal == SVC_OK) {
        SVC_CALIB_CHANNEL_s     *pChan;
        AMBA_IK_MODE_CFG_s      *pImgMode;

        (void)pParam1;
        (void)pParam2;
        (void)pParam3;
        (void)pParam4;

        AmbaMisra_TypeCast(&(pChan), &pParam1);
        AmbaMisra_TypeCast(&(pImgMode), &pParam2);

        RetVal = SvcCalib_StereoWarpTableUpdate(pChan, pImgMode);
    } else {
        AmbaMisra_TouchUnused(&pParam1);
        AmbaMisra_TouchUnused(&pParam2);
        AmbaMisra_TouchUnused(&pParam3);
        AmbaMisra_TouchUnused(&pParam4);
    }

    return RetVal;
}

/**
 * calib stereo command entry
 *
 * @param [in] CmdID  the stereo command id
 * @param [in] Param1 no.01 parameter
 * @param [in] Param2 no.02 parameter
 * @param [in] Param3 no.03 parameter
 * @param [in] Param4 no.04 parameter
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcCalib_StereoCmdFunc(UINT32 CmdID, void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    SvcCalib_StereoCmdHdlrInit();

    if (CmdID >= SVC_CALIB_CMD_NUM) {
        RetVal = SVC_NG;
        LOG_CAL_STEREO_NG("Fail to handler cmd - invalid command id(%d)", CmdID, 0U);
    } else {
        const UINT32 HdlrCount = (UINT32)(sizeof(SvcCalibStereoCmdHdlr)) / ((UINT32)sizeof(SvcCalibStereoCmdHdlr[0]));
        UINT32 HdlrIdx;

        for (HdlrIdx = 0U; HdlrIdx < HdlrCount; HdlrIdx ++) {
            if ((SvcCalibStereoCmdHdlr[HdlrIdx].Enable > 0U) &&
                (SvcCalibStereoCmdHdlr[HdlrIdx].CmdID == CmdID) &&
                (SvcCalibStereoCmdHdlr[HdlrIdx].pHandler != NULL)) {
                RetVal = (SvcCalibStereoCmdHdlr[HdlrIdx].pHandler)(pParam1, pParam2, pParam3, pParam4);
            }
        }
    }

    return RetVal;
}
