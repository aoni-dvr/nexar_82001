/**
 *  @file SvcCalibAdas.c
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
 *  @details svc calibration ADAS
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"
#include "AmbaKAL.h"
#include "AmbaDSP_ImageUtility.h"

#include "AmbaCalib_EmirrorDef.h"
#include "AmbaCalib_AVMIF.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcCalibMgr.h"
#include "SvcCalibAdas.h"
#include "ArmLog.h"

#define SVC_CALIB_ADAS_FLG_INIT           (0x1U)
#define SVC_CALIB_ADAS_FLG_SHADOW_INIT    (0x2U)
#define SVC_CALIB_ADAS_FLG_DBG_MSG        (0x100U)
#define SVC_CALIB_ADAS_FLG_SHELL_INIT     (0x1000U)
#define SVC_CALIB_ADAS_FLG_CMD_INIT       (0x2000U)

#define SVC_CALIB_ADAS_NAME               ("SvcCalib_ADAS")
#define SVC_CALIB_ADAS_NAND_HEADER_SIZE   (0x200U)

typedef struct {
    UINT32 Enable;
    UINT32 NumOfTable;
    UINT32 DebugMsgOn;
    UINT32 Reserved[(SVC_CALIB_ADAS_NAND_HEADER_SIZE / 4U) - 3U];
} SVC_CALIB_ADAS_NAND_HEADER_s;

typedef struct {
    char                           Name[16];
    AMBA_KAL_MUTEX_t               Mutex;
    SVC_CALIB_ADAS_NAND_HEADER_s  *pNandHeader;
    SVC_CALIB_ADAS_NAND_TABLE_s   *pNandTable;
    UINT32                         MaxTableNum;
    UINT32                         EnableDebugMsg;
} SVC_CALIB_ADAS_CTRL_s;

static UINT32                       SvcCalib_AdasMemQuery(UINT32 MaxTableNum, UINT32 *pShadowSize, UINT32 *pWorkMemSize, UINT32 *pCalcMemSize);
static UINT32                       SvcCalib_AdasCreate(void);
static UINT32                       SvcCalib_AdasShadowInit(const SVC_CALIB_OBJ_s *pCalObj);
static SVC_CALIB_ADAS_NAND_TABLE_s *SvcCalib_AdasTableGet(UINT32 Type);
static SVC_CALIB_ADAS_NAND_TABLE_s *SvcCalib_AdasFreeTableGet(void);

// Private function definition for calibration command
typedef UINT32 (*SVC_CALIB_ADAS_CMD_FUNC_f)(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
typedef struct {
     UINT32                    Enable;
     UINT32                    CmdID;
     SVC_CALIB_ADAS_CMD_FUNC_f pHandler;
} SVC_CALIB_ADAS_CMD_HDLR_s;

static void   SvcCalib_AdasCmdHdlrInit(void);
static UINT32 SvcCalib_AdasCmdMemQuery(void *pParam1, void *pParam2, void *pParam3, void *pParam4);
static UINT32 SvcCalib_AdasCmdInit(void *pParam1, void *pParam2, void *pParam3, void *pParam4);

// Private function definition for shell command.
typedef UINT32 (*SVC_CALIB_ADAS_SHELL_FUNC_f)(UINT32 ArgCount, char * const *pArgVector);
typedef void   (*SVC_CALIB_ADAS_SHELL_USAGE_f)(void);

typedef struct {
    UINT32                       Enable;
    char                         ShellCmdName[32];
    SVC_CALIB_ADAS_SHELL_FUNC_f  pFunc;
    SVC_CALIB_ADAS_SHELL_USAGE_f pUsageFunc;
} SVC_CALIB_ADAS_SHELL_FUNC_s;

static UINT32 SvcCalib_AdasShellCfgDbgMsg(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalib_AdasShellCfgDbgMsgU(void);
static void   SvcCalib_AdasShellEntryInit(void);
static void   SvcCalib_AdasShellUsage(void);

#define SVC_CALIB_ADAS_SHELL_CMD_NUM  (1U)
static SVC_CALIB_ADAS_SHELL_FUNC_s CalibAdasShellFunc[SVC_CALIB_ADAS_SHELL_CMD_NUM] GNU_SECTION_NOZEROINIT;
static SVC_CALIB_ADAS_CMD_HDLR_s SvcCalibAdasCmdHdlr[SVC_CALIB_CMD_NUM] GNU_SECTION_NOZEROINIT;
static SVC_CALIB_ADAS_CTRL_s SVC_CalibAdasCtrl GNU_SECTION_NOZEROINIT;
#ifdef SVC_CALIB_DBG_MSG_ON
static UINT32 SVC_CalibAdasCtrlFlag = SVC_LOG_CAL_DEF_FLG | SVC_LOG_CAL_DBG;
#else
static UINT32 SVC_CalibAdasCtrlFlag = SVC_LOG_CAL_DEF_FLG;
#endif

#define SVC_LOG_ADAS "CAL_ADAS"

/**
 *  Query calib adas memory
 *  @param[in] MaxTableNum Max supported table number
 *  @param[out] pShadowSize The pointer of requested dram shadow memory size
 *  @param[out] pWorkMemSize The pointer of requested working memory size
 *  @param[out] pCalcMemSize The pointer of requested calculation memory size
 *  @return error code
 */

static UINT32 SvcCalib_AdasMemQuery(UINT32 MaxTableNum, UINT32 *pShadowSize, UINT32 *pWorkMemSize, UINT32 *pCalcMemSize)
{
    UINT32 RetVal = SVC_OK;

    UINT32 DramShadowSize;
    UINT32 DramShadowHeaderSize = (UINT32)(sizeof(SVC_CALIB_ADAS_NAND_HEADER_s));
    UINT32 DramShadowTableSize  = (UINT32)(sizeof(SVC_CALIB_ADAS_NAND_TABLE_s));
    UINT32 DramShadowDataSize   = DramShadowTableSize * MaxTableNum;

    DramShadowSize  = DramShadowHeaderSize;
    DramShadowSize += DramShadowDataSize;

    SvcLog_DBG(SVC_LOG_ADAS, "", 0U, 0U);
    SvcLog_DBG(SVC_LOG_ADAS, "====== Memory Query Info ======", 0U, 0U);
    SvcLog_DBG(SVC_LOG_ADAS, "  MaxTableNum    : %d",   MaxTableNum, 0U);
    SvcLog_DBG(SVC_LOG_ADAS, "  DramShadowSize : 0x%x", DramShadowSize, 0U);
    SvcLog_DBG(SVC_LOG_ADAS, "    DramShadowHeaderSize : 0x%x", DramShadowHeaderSize, 0U);
    SVC_WRAP_PRINT "    DramShadowDataSize   : %#x ( %#x x %d )"
        SVC_PRN_ARG_S SVC_LOG_ADAS
        SVC_PRN_ARG_UINT32 DramShadowDataSize  SVC_PRN_ARG_POST
        SVC_PRN_ARG_UINT32 DramShadowTableSize SVC_PRN_ARG_POST
        SVC_PRN_ARG_UINT32 MaxTableNum         SVC_PRN_ARG_POST
        SVC_PRN_ARG_E

    if (pShadowSize != NULL) {
        *pShadowSize  = DramShadowSize;
    }
    SvcLog_DBG(SVC_LOG_ADAS, "Query the calib adas dram shadow size 0x%x ( table num %d )", DramShadowSize, MaxTableNum);

    if (pWorkMemSize != NULL) {
        *pWorkMemSize = 0U;
    }
    SvcLog_DBG(SVC_LOG_ADAS, "Query the calib adas working memory size 0x%x ( table num %d )", 0, MaxTableNum);

    if (pCalcMemSize != NULL) {
        *pCalcMemSize = 0U;
    }
    SvcLog_DBG(SVC_LOG_ADAS, "Query the calib adas algo. memory size 0x%x ( table num %d )", 0, MaxTableNum);

    return RetVal;
}

/**
 *  Create calib adas module
 *  @return error code
 */
static UINT32 SvcCalib_AdasCreate(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibAdasCtrlFlag & SVC_CALIB_ADAS_FLG_INIT) > 0U) {
        RetVal = SVC_OK;
        SvcLog_DBG(SVC_LOG_ADAS, "Calibration ADAS module has been created!", 0U, 0U);
    } else {
        // Reset the adas control module
        AmbaSvcWrap_MisraMemset(&SVC_CalibAdasCtrl, 0, sizeof(SVC_CalibAdasCtrl));

        // Configure the name
        SvcWrap_strcpy(SVC_CalibAdasCtrl.Name, sizeof(SVC_CalibAdasCtrl.Name), SVC_CALIB_ADAS_NAME);

        // Create the mutex
        RetVal = AmbaKAL_MutexCreate(&(SVC_CalibAdasCtrl.Mutex), SVC_CalibAdasCtrl.Name);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_ADAS, "Failure to create adas - create mutex fail!", 0U, 0U);
        } else {
            PRetVal = (SVC_CalibAdasCtrlFlag & (SVC_CALIB_ADAS_FLG_DBG_MSG | SVC_CALIB_ADAS_FLG_SHELL_INIT | SVC_CALIB_ADAS_FLG_CMD_INIT));
            SVC_CalibAdasCtrlFlag = PRetVal;
            SVC_CalibAdasCtrlFlag |= SVC_CALIB_ADAS_FLG_INIT;
            SvcLog_OK(SVC_LOG_ADAS, "Successful to create calib adas!", 0U, 0U);
        }
    }

    return RetVal;
}

/**
 *  Initial calib adas dram shadow
 *  @param[in] pCalObj The pointer of input calib adas object
 *  @return error code
 */
static UINT32 SvcCalib_AdasShadowInit(const SVC_CALIB_OBJ_s *pCalObj)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibAdasCtrlFlag & SVC_CALIB_ADAS_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ADAS, "Failure to initial adas dram shodow - create adas first!", 0U, 0U);
    } else {
        if (pCalObj == NULL) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ADAS, "Failure to initial adas dram shodow - invalid calib object!", 0U, 0U);
        } else {
            if (pCalObj->Enable == 0U) {
                RetVal = 99;
                SvcLog_DBG(SVC_LOG_ADAS, "Disable adas dram shadow!!", 0U, 0U);
            }
        }
    }

    if (RetVal == SVC_OK) {
        UINT32 DramShadowSize = 0, WorkingMemSize = 0;

        RetVal = SvcCalib_AdasMemQuery(pCalObj->NumOfTable, &DramShadowSize, &WorkingMemSize, NULL);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_ADAS, "Failure to initial adas dram shadow - query memory fail!", 0U, 0U);
        } else {
            if (pCalObj->pShadowBuf == NULL) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ADAS, "Failure to initial adas dram shadow - dram shadow buffer should not null!", 0U, 0U);
            } else if (pCalObj->ShadowBufSize < DramShadowSize) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ADAS, "Failure to initial adas dram shadow - prepare dram shadow size is too small!", 0U, 0U);
            } else {
                const SVC_CALIB_ADAS_NAND_HEADER_s *pHeader;

                AmbaMisra_TypeCast(&(pHeader), &(pCalObj->pShadowBuf));
                if (pHeader != NULL) {
                    if (pHeader->NumOfTable > pCalObj->NumOfTable) {
                        RetVal = SVC_NG;
                        SvcLog_NG(SVC_LOG_ADAS, "Failure to initial adas dram shadow - calib nand table count(%d) > max adas dram shadow table count(%d)",
                                pHeader->NumOfTable, pCalObj->NumOfTable);
                    }
                }
            }

            if (pCalObj->pWorkingBuf == NULL) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ADAS, "Failure to initial adas dram shadow - working buffer should not null!", 0U, 0U);
            } else {
                if (pCalObj->WorkingBufSize < WorkingMemSize) {
                    RetVal = SVC_NG;
                    SvcLog_NG(SVC_LOG_ADAS, "Failure to initial adas dram shadow - working buffer size is too small!", 0U, 0U);
                }
            }
        }
    }

    if (RetVal == SVC_OK) {
        if (KAL_ERR_NONE != AmbaKAL_MutexTake(&(SVC_CalibAdasCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ADAS, "Failure to initial adas dram shadow - take mutex fail!", 0U, 0U);
        } else {
            UINT8 *pNandHeaderBuf    = pCalObj->pShadowBuf;
            UINT32 NandHeaderSize    = (UINT32)(sizeof(SVC_CALIB_ADAS_NAND_HEADER_s));
            UINT8 *pNandTableBuf     = &(pNandHeaderBuf[NandHeaderSize]);

            RetVal = SVC_OK;

            SvcLog_DBG(SVC_LOG_ADAS, "====== Shadow initialized Start ======", 0U, 0U);
            SVC_WRAP_PRINT "  DramShadow Addr  : %p 0x%x"
                SVC_PRN_ARG_S SVC_LOG_ADAS
                SVC_PRN_ARG_CPOINT pCalObj->pShadowBuf    SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 pCalObj->ShadowBufSize SVC_PRN_ARG_POST
                SVC_PRN_ARG_E
            SVC_WRAP_PRINT "    Nand Header Addr : %p 0x%x"
                SVC_PRN_ARG_S SVC_LOG_ADAS
                SVC_PRN_ARG_CPOINT pNandHeaderBuf SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 NandHeaderSize SVC_PRN_ARG_POST
                SVC_PRN_ARG_E
            SVC_WRAP_PRINT "    Nand Table Addr  : %p"
                SVC_PRN_ARG_S SVC_LOG_ADAS
                SVC_PRN_ARG_CPOINT pNandTableBuf SVC_PRN_ARG_POST
                SVC_PRN_ARG_E

            // Configure nand header and table
            AmbaMisra_TypeCast(&(SVC_CalibAdasCtrl.pNandHeader), &(pNandHeaderBuf));
            if (SVC_CalibAdasCtrl.pNandHeader == NULL) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ADAS, "Failure to initial adas dram shadow - nand header should not null!", 0U, 0U);
            } else {
                SVC_CalibAdasCtrl.EnableDebugMsg = SVC_CalibAdasCtrl.pNandHeader->DebugMsgOn;

                AmbaMisra_TypeCast(&(SVC_CalibAdasCtrl.pNandTable),  &(pNandTableBuf)); AmbaMisra_TouchUnused(pNandTableBuf);
                if (SVC_CalibAdasCtrl.pNandTable == NULL) {
                    RetVal = SVC_NG;
                    SvcLog_NG(SVC_LOG_ADAS, "Failure to initial adas dram shadow - nand table should not null!", 0U, 0U);
                }
            }

            SVC_CalibAdasCtrl.MaxTableNum = pCalObj->NumOfTable;

            if (SVC_CalibAdasCtrl.pNandHeader != NULL) {
                if (SVC_CalibAdasCtrl.EnableDebugMsg > 0U) {

                    SvcLog_DBG(SVC_LOG_ADAS, "", 0U, 0U);
                    SvcLog_DBG(SVC_LOG_ADAS, "Max Table Number : %d", SVC_CalibAdasCtrl.MaxTableNum , 0U);
                    SvcLog_DBG(SVC_LOG_ADAS, "---- adas Nand Info ----", SVC_CalibAdasCtrl.pNandHeader->Reserved[0], 0U);
                    SvcLog_DBG(SVC_LOG_ADAS, "  Enable         : %d", SVC_CalibAdasCtrl.pNandHeader->Enable, 0U);
                    SvcLog_DBG(SVC_LOG_ADAS, "  NumOfTable     : %d", SVC_CalibAdasCtrl.pNandHeader->NumOfTable, 0U);
                    SvcLog_DBG(SVC_LOG_ADAS, "  DebugMsgOn     : %d", SVC_CalibAdasCtrl.pNandHeader->DebugMsgOn, 0U);

                    if (SVC_CalibAdasCtrl.pNandTable != NULL) {
                        UINT32 Idx;
                        const SVC_CALIB_ADAS_NAND_TABLE_s *pNandTable;

                        for (Idx = 0U; Idx < pCalObj->NumOfTable; Idx ++) {
                            pNandTable = &(SVC_CalibAdasCtrl.pNandTable[Idx]);

                            SVC_WRAP_PRINT "  ---- Nand Table[%d] %p -----"
                                SVC_PRN_ARG_S SVC_LOG_ADAS
                                SVC_PRN_ARG_UINT32 Idx        SVC_PRN_ARG_POST
                                SVC_PRN_ARG_CPOINT pNandTable SVC_PRN_ARG_POST
                                SVC_PRN_ARG_E
                            SvcLog_DBG(SVC_LOG_ADAS, "    Enable              : %d",   pNandTable->Header.Enable             , 0U);
                            SvcLog_DBG(SVC_LOG_ADAS, "    Type                : %d",   pNandTable->Header.Type               , 0U);
                            SvcLog_DBG(SVC_LOG_ADAS, "    Version             : 0x%X", pNandTable->Header.Version            , 0U);
                        }
                    }
                }
            }

            SVC_CalibAdasCtrlFlag |= SVC_CALIB_ADAS_FLG_SHADOW_INIT;
            SvcLog_OK(SVC_LOG_ADAS, "Successful to initial adas dram shadow!", 0U, 0U);

            PRetVal = AmbaKAL_MutexGive(&(SVC_CalibAdasCtrl.Mutex));
            if (PRetVal != SVC_OK) {
                SvcLog_NG(SVC_LOG_ADAS, "AmbaKAL_MutexGive error [1]", 0U, 0U);
            }
        }
    }

    return RetVal;
}

/**
 *  Get the calib adas table
 *  @param[in] Type  Calibration adas type
 *  @return table pointer or error code
 */
static SVC_CALIB_ADAS_NAND_TABLE_s *SvcCalib_AdasTableGet(UINT32 Type)
{
    UINT32 RetVal = SVC_OK;
    SVC_CALIB_ADAS_NAND_TABLE_s *pNandTable = NULL;

    if ((SVC_CalibAdasCtrlFlag & SVC_CALIB_ADAS_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ADAS, "Failure to get adas table - initial adas module first!", 0U, 0U);
    } else {
        if ((SVC_CalibAdasCtrlFlag & SVC_CALIB_ADAS_FLG_SHADOW_INIT) == 0U) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ADAS, "Failure to get adas table - initial adas dram shadow first!", 0U, 0U);
        } else {
            if (SVC_CalibAdasCtrl.pNandHeader == NULL) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ADAS, "Failure to get adas table - initial adas dram shadow first.", 0U, 0U);
            }
            if (SVC_CalibAdasCtrl.pNandHeader == NULL) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ADAS, "Failure to get adas table - initial adas dram shadow first!!", 0U, 0U);
            }
        }
    }

    if ((Type >= SVC_CALIB_ADAS_TYPE_NUM) || (Type == 0U)) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ADAS, "Failure to update adas table - invalid calib channel!", 0U, 0U);
    }

    if (SVC_CalibAdasCtrl.pNandHeader != NULL) {
        if ((SVC_CalibAdasCtrl.pNandHeader->Enable == 0U) ||
            (SVC_CalibAdasCtrl.pNandHeader->NumOfTable == 0U)) {
            RetVal = SVC_NG;
        }
    }

    if (RetVal == SVC_OK) {
        UINT32 NandTblIdx;

        for (NandTblIdx = 0U; NandTblIdx < SVC_CalibAdasCtrl.pNandHeader->NumOfTable; NandTblIdx ++) {
            if ((SVC_CalibAdasCtrl.pNandTable[NandTblIdx].Header.Enable > 0U) &&
                (SVC_CalibAdasCtrl.pNandTable[NandTblIdx].Header.Type == Type)) {

                pNandTable = &(SVC_CalibAdasCtrl.pNandTable[NandTblIdx]);

                break;
            }
        }
    }

    return pNandTable;
}

/**
 *  Get the free calib adas table
 *  @return table pointer or error code
 */
static SVC_CALIB_ADAS_NAND_TABLE_s *SvcCalib_AdasFreeTableGet(void)
{
    UINT32 RetVal = SVC_OK;
    SVC_CALIB_ADAS_NAND_TABLE_s *pNandTable = NULL;

    if ((SVC_CalibAdasCtrlFlag & SVC_CALIB_ADAS_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ADAS, "Failure to get adas free table - initial adas module first!", 0U, 0U);
    } else {
        if ((SVC_CalibAdasCtrlFlag & SVC_CALIB_ADAS_FLG_SHADOW_INIT) == 0U) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ADAS, "Failure to get adas free table - initial adas dram shadow first!", 0U, 0U);
        } else {
            if (SVC_CalibAdasCtrl.pNandHeader == NULL) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ADAS, "Failure to get adas free table - initial adas dram shadow first.", 0U, 0U);
            }
            if (SVC_CalibAdasCtrl.pNandHeader == NULL) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ADAS, "Failure to get adas free table - initial adas dram shadow first!!", 0U, 0U);
            }
        }
    }

    if (RetVal == SVC_OK) {
        UINT32 NandTblIdx;

        for (NandTblIdx = 0U; NandTblIdx < SVC_CalibAdasCtrl.MaxTableNum; NandTblIdx ++) {
            if (SVC_CalibAdasCtrl.pNandTable[NandTblIdx].Header.Type == 0U) {
                pNandTable = &(SVC_CalibAdasCtrl.pNandTable[NandTblIdx]);
                break;
            }
        }
    }

    return pNandTable;
}

/**
 *  Initial calib adas shell entry
 */
static void SvcCalib_AdasShellEntryInit(void)
{
    if ((SVC_CalibAdasCtrlFlag & SVC_CALIB_ADAS_FLG_SHELL_INIT) == 0U) {

        AmbaSvcWrap_MisraMemset(CalibAdasShellFunc, 0, sizeof(CalibAdasShellFunc));

        CalibAdasShellFunc[0U] = (SVC_CALIB_ADAS_SHELL_FUNC_s) { 1U, "cfg_dbg_msg",     SvcCalib_AdasShellCfgDbgMsg,  SvcCalib_AdasShellCfgDbgMsgU   };

        SVC_CalibAdasCtrlFlag |= SVC_CALIB_ADAS_FLG_SHELL_INIT;
    }
}

/**
 *  Adas Calib shell command entry
 *  @param[in] ArgCount Input arguments counter
 *  @param[in] pArgVector Input arguments data
 *  @param[in] pExtraData The pointer of extra data.
 *  @param[in] pCalObj The pointer of calib object.
 *  @return error code
 */
UINT32 SvcCalib_AdasShellFunc(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_NG, PRetVal;

    SvcCalib_AdasShellEntryInit();

    if (ArgCount > 0U) {
        UINT32 ShellIdx;
        UINT32 ShellCount = (UINT32)(sizeof(CalibAdasShellFunc)) / (UINT32)(sizeof(CalibAdasShellFunc[0]));

        for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
            if ((CalibAdasShellFunc[ShellIdx].pFunc != NULL) && (CalibAdasShellFunc[ShellIdx].Enable > 0U)) {
                if (0 == SvcWrap_strcmp(pArgVector[2U], CalibAdasShellFunc[ShellIdx].ShellCmdName)) {
                    PRetVal = (CalibAdasShellFunc[ShellIdx].pFunc)(ArgCount, pArgVector);
                    AmbaMisra_TouchUnused(&PRetVal);

                    RetVal = SVC_OK;
                    break;
                }
            }
        }

        if (RetVal != SVC_OK) {
            SvcCalib_AdasShellUsage();
        }
    }

    return RetVal;
}

/**
 *  Adas Calib shell command usage
 */
static void SvcCalib_AdasShellUsage(void)
{
    UINT32 ShellIdx;
    UINT32 ShellCount = (UINT32)(sizeof(CalibAdasShellFunc)) / (UINT32)(sizeof(CalibAdasShellFunc[0]));

    SvcLog_DBG(SVC_LOG_ADAS, "", 0U, 0U);
    SvcLog_DBG(SVC_LOG_ADAS, "====== Calib ADAS Command Usage ======", 0U, 0U);

    for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
        if ((CalibAdasShellFunc[ShellIdx].pFunc != NULL) && (CalibAdasShellFunc[ShellIdx].Enable > 0U)) {
            if (CalibAdasShellFunc[ShellIdx].pUsageFunc == NULL) {
                SVC_WRAP_PRINT "  %s"
                    SVC_PRN_ARG_S SVC_LOG_ADAS
                    SVC_PRN_ARG_CSTR CalibAdasShellFunc[ShellIdx].ShellCmdName SVC_PRN_ARG_POST
                    SVC_PRN_ARG_E
            } else {
                (CalibAdasShellFunc[ShellIdx].pUsageFunc)();
            }
        }
    }
}

/**
 *  Shell command - enable/disable debug message
 *  @param[in] ArgCount Input arguments counter
 *  @param[in] pArgVector Input arguments data
 *  @param[in] pExtraData The pointer of extra data.
 *  @param[in] pCalObj The pointer of calib object.
 *  @return error code
 */
static UINT32 SvcCalib_AdasShellCfgDbgMsg(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibAdasCtrlFlag & SVC_CALIB_ADAS_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ADAS, "Failure to switch adas debug msg - initial adas module first!", 0U, 0U);
    }

    if (ArgCount <= 3U) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ADAS, "Failure to switch adas debug msg - Argc should >= 3", 0U, 0U);
    }

    if (pArgVector == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ADAS, "Failure to switch adas debug msg - Argv should not null!", 0U, 0U);
    }

    if (RetVal == SVC_OK) {
        UINT32 DbgMsgOn = 0U;

        PRetVal = SvcWrap_strtoul(pArgVector[3U],  &DbgMsgOn ); AmbaMisra_TouchUnused(&PRetVal);

        if (SVC_OK == SvcCalib_AdasDbgMsg(DbgMsgOn)) {
            SvcLog_DBG(SVC_LOG_ADAS, "", 0U, 0U);
            SvcLog_DBG(SVC_LOG_ADAS, "------ Calibration ADAS Debug Msg On/Off %d ------", SVC_CalibAdasCtrl.EnableDebugMsg, 0U);
        }

    } else {
        AmbaMisra_TouchUnused(&ArgCount);
    }

    return RetVal;
}

/**
 *  Shell command usage - enable/disable debug message
 */
static void SvcCalib_AdasShellCfgDbgMsgU(void)
{
    SVC_WRAP_PRINT "  %scfg_dbg_msg%s           :"
        SVC_PRN_ARG_S SVC_LOG_ADAS
        SVC_PRN_ARG_CSTR SVC_LOG_CAL_HL_TITLE_1 SVC_PRN_ARG_POST
        SVC_PRN_ARG_CSTR SVC_LOG_CAL_HL_END     SVC_PRN_ARG_POST
        SVC_PRN_ARG_E
    SvcLog_DBG(SVC_LOG_ADAS, "  ------------------------------------------------------", 0U, 0U);
    SvcLog_DBG(SVC_LOG_ADAS, "    [Enable]            : 0: disable debug message", 0U, 0U);
    SvcLog_DBG(SVC_LOG_ADAS, "                        : 1: enable debug message", 0U, 0U);
    SvcLog_DBG(SVC_LOG_ADAS, " ", 0U, 0U);
}

/**
 *  Initial calib Adas command handler
 */
static void SvcCalib_AdasCmdHdlrInit(void)
{
    if ((SVC_CalibAdasCtrlFlag & SVC_CALIB_ADAS_FLG_CMD_INIT) == 0U) {

        AmbaSvcWrap_MisraMemset(SvcCalibAdasCmdHdlr, 0, sizeof(SvcCalibAdasCmdHdlr));

        SvcCalibAdasCmdHdlr[0] = (SVC_CALIB_ADAS_CMD_HDLR_s) {0, 0U,                            NULL                      };
        SvcCalibAdasCmdHdlr[1] = (SVC_CALIB_ADAS_CMD_HDLR_s) {1, SVC_CALIB_CMD_MEM_QUERY,       SvcCalib_AdasCmdMemQuery  };
        SvcCalibAdasCmdHdlr[2] = (SVC_CALIB_ADAS_CMD_HDLR_s) {1, SVC_CALIB_CMD_DATA_LOAD,       NULL                      };
        SvcCalibAdasCmdHdlr[3] = (SVC_CALIB_ADAS_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_DATA_INIT,  SvcCalib_AdasCmdInit      };
        SvcCalibAdasCmdHdlr[4] = (SVC_CALIB_ADAS_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_TBL_CFG,    NULL                      };
        SvcCalibAdasCmdHdlr[5] = (SVC_CALIB_ADAS_CMD_HDLR_s) {1, SVC_CALIB_CMD_ITEM_TBL_UPDATE, NULL                      };

        SVC_CalibAdasCtrlFlag |= SVC_CALIB_ADAS_FLG_CMD_INIT;

    }
}

/**
 *  Calib adas command - memory query
 *  @param[in] Param1 1st parameter
 *  @param[in] Param2 2nd parameter
 *  @param[in] Param3 3rd parameter
 *  @param[in] Param4 4th parameter
 *  @return error code
 */
static UINT32 SvcCalib_AdasCmdMemQuery(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    if (pParam1 == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ADAS, "Failure to query adas memory - input adas table number should not zero. %d", 0U, 0U);
    }

    if (pParam2 == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ADAS, "Failure to query adas memory - output adas shadow buffer size should not null!", 0U, 0U);
    }

    if (pParam3 == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ADAS, "Failure to query adas memory - output adas working buffer size should not null!", 0U, 0U);
    }

    if (pParam4 == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ADAS, "Failure to query adas memory - output adas alg. buffer size should not null!", 0U, 0U);
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

        RetVal = SvcCalib_AdasMemQuery(NomOfTable, pShadowBufSize, pWorkingBufSize, pCalcMemSize);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_ADAS, "Fail to query adas memory - query memory fail!", 0U, 0U);
        }

        AmbaMisra_TouchUnused(pParam1);
        AmbaMisra_TouchUnused(pParam2);
        AmbaMisra_TouchUnused(pParam3);
        AmbaMisra_TouchUnused(pParam4);
        AmbaMisra_TouchUnused(pU32Val);
    } else {
        AmbaMisra_TouchUnused(SvcCalibAdasCmdHdlr);
    }

    return RetVal;
}

/**
 *  Calib adas command - init command
 *  @param[in] Param1 1st parameter
 *  @param[in] Param2 2nd parameter
 *  @param[in] Param3 3rd parameter
 *  @param[in] Param4 4th parameter
 *  @return error code
 */
static UINT32 SvcCalib_AdasCmdInit(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    if (pParam1 == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ADAS, "Failure to init adas - input calib object should not null!", 0U, 0U);
    }

    if (RetVal == SVC_OK) {
        RetVal = SvcCalib_AdasCreate();
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_ADAS, "Failure to init adas - create module fail!", 0U, 0U);
        }
    }

    if (RetVal == SVC_OK) {
        const SVC_CALIB_OBJ_s *pCalObj;

        AmbaMisra_TypeCast(&pCalObj, &pParam1);
        if (pCalObj == NULL) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ADAS, "Fail to init adas - invalid calib obj!", 0U, 0U);
        } else {
            RetVal = SvcCalib_AdasShadowInit(pCalObj);
            if (RetVal != SVC_OK) {
                SvcLog_NG(SVC_LOG_ADAS, "Fail to init adas - shadow initial fail!", 0U, 0U);
            }
        }

        AmbaMisra_TouchUnused(pParam1);
        AmbaMisra_TouchUnused(pParam2);
        AmbaMisra_TouchUnused(pParam3);
        AmbaMisra_TouchUnused(pParam4);
    }

    return RetVal;
}

/**
 *  Calib adas command entry
 *  @param[in] Param1 1st parameter
 *  @param[in] Param2 2nd parameter
 *  @param[in] Param3 3rd parameter
 *  @param[in] Param4 4th parameter
 *  @return error code
 */
UINT32 SvcCalib_AdasCmdFunc(UINT32 CmdID, void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;

    SvcCalib_AdasCmdHdlrInit();

    if (CmdID >= SVC_CALIB_CMD_NUM) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ADAS, "Failure to handler cmd - invalid command id(%d)", CmdID, 0U);
    } else {
        const UINT32 HdlrCount = (UINT32)(sizeof(SvcCalibAdasCmdHdlr)) / ((UINT32)sizeof(SvcCalibAdasCmdHdlr[0]));
        UINT32 HdlrIdx;

        for (HdlrIdx = 0U; HdlrIdx < HdlrCount; HdlrIdx ++) {
            if ((SvcCalibAdasCmdHdlr[HdlrIdx].Enable > 0U) &&
                (SvcCalibAdasCmdHdlr[HdlrIdx].CmdID == CmdID) &&
                (SvcCalibAdasCmdHdlr[HdlrIdx].pHandler != NULL)) {
                RetVal = (SvcCalibAdasCmdHdlr[HdlrIdx].pHandler)(pParam1, pParam2, pParam3, pParam4);
            }
        }
    }

    return RetVal;
}

/**
 *  Get calib version
 *  @param[in] Type Calibration adas type
 *  @return calib version
 */
UINT32 SvcCalib_AdasCfgGetCalVer(UINT32 Type)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibAdasCtrlFlag & SVC_CALIB_ADAS_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ADAS, "Failure to get adas cfg - initial adas module first!", 0U, 0U);
    } else {
        if ((SVC_CalibAdasCtrlFlag & SVC_CALIB_ADAS_FLG_SHADOW_INIT) == 0U) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ADAS, "Failure to get adas cfg - initial adas dram shadow first!", 0U, 0U);
        } else {
            if (SVC_CalibAdasCtrl.pNandHeader == NULL) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ADAS, "Failure to get adas cfg - initial adas dram shadow first.", 0U, 0U);
            }
            if (SVC_CalibAdasCtrl.pNandTable == NULL) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ADAS, "Failure to get adas cfg - initial adas dram shadow first!!", 0U, 0U);
            }
        }
    }

    if ((Type >= SVC_CALIB_ADAS_TYPE_NUM) || (Type == 0U)) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ADAS, "Failure to get adas cfg - invalid type(%d)", Type, 0U);
    }

    if ((RetVal == SVC_OK)) {
        const SVC_CALIB_ADAS_NAND_TABLE_s *pTbl = NULL;

        if (KAL_ERR_NONE != AmbaKAL_MutexTake(&(SVC_CalibAdasCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ADAS, "Failure to get adas cfg - take mutex fail!", 0U, 0U);
        } else {
            UINT32 PRetVal;
            pTbl = SvcCalib_AdasTableGet(Type);

            if (pTbl == NULL) {
                RetVal = SVC_NG;
            } else {
                RetVal = pTbl->Header.Version;
            }
            PRetVal = AmbaKAL_MutexGive(&(SVC_CalibAdasCtrl.Mutex));
            if (PRetVal != SVC_OK) {
                SvcLog_NG(SVC_LOG_ADAS, "AmbaKAL_MutexGive error [2]", 0U, 0U);
            }
        }
    }


    return RetVal;
}


/**
 *  Get calib adas data
 *  @param[in] Type Calibration adas type
 *  @param[out] pCfg The pointer of output configuration
 *  @return error code
 */
UINT32 SvcCalib_AdasCfgGetV1(UINT32 Type, void *pCfg)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibAdasCtrlFlag & SVC_CALIB_ADAS_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ADAS, "Failure to get adas cfg - initial adas module first!", 0U, 0U);
    } else {
        if ((SVC_CalibAdasCtrlFlag & SVC_CALIB_ADAS_FLG_SHADOW_INIT) == 0U) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ADAS, "Failure to get adas cfg - initial adas dram shadow first!", 0U, 0U);
        } else {
            if (SVC_CalibAdasCtrl.pNandHeader == NULL) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ADAS, "Failure to get adas cfg - initial adas dram shadow first.", 0U, 0U);
            }
            if (SVC_CalibAdasCtrl.pNandTable == NULL) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ADAS, "Failure to get adas cfg - initial adas dram shadow first!!", 0U, 0U);
            }
        }
    }

    if ((Type >= SVC_CALIB_ADAS_TYPE_NUM) || (Type == 0U)) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ADAS, "Failure to get adas cfg - invalid type(%d)", Type, 0U);
    }

    if (pCfg == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ADAS, "Failure to get adas cfg - input cfg should not null!", 0U, 0U);
    } else {
        AmbaMisra_TouchUnused(pCfg);
    }

    if (RetVal == SVC_OK) {
        SVC_CALIB_ADAS_NAND_TABLE_s *pTbl = NULL;

        if (KAL_ERR_NONE != AmbaKAL_MutexTake(&(SVC_CalibAdasCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ADAS, "Failure to get adas cfg - take mutex fail!", 0U, 0U);
        } else {
            pTbl = SvcCalib_AdasTableGet(Type);

            if (pTbl == NULL) {
                RetVal = SVC_NG;
            } else {
                ArmLog_OK(SVC_LOG_ADAS, "Type = %d, Version = 0x%X", pTbl->Header.Type, pTbl->Header.Version);
                AmbaPrint_Flush();
                if (Type < SVC_CALIB_ADAS_TYPE_AVM_F) {//EMIRROR CALIB
                    SVC_CALIB_ADAS_INFO_GET_s *pAdasInfo = NULL;

                    AmbaMisra_TypeCast(&pAdasInfo, &pCfg);

                    if (pTbl->Header.Version == SVC_CALIB_ADAS_VERSION_V1) {

                        pAdasInfo->CalibVer = 1U; /* Version 1 */
                        PRetVal = AmbaWrap_memcpy(&(pAdasInfo->AdasCfgV1.Type),     &(pTbl->AdasCfgV1.Type),     sizeof(AMBA_CAL_EM_VIEW_TYPE_e)); AmbaMisra_TouchUnused(&PRetVal);
                        PRetVal = AmbaWrap_memcpy(&(pAdasInfo->AdasCfgV1.CamPos),   &(pTbl->AdasCfgV1.CamPos),   sizeof(AMBA_CAL_POINT_DB_3D_s)); AmbaMisra_TouchUnused(&PRetVal);
                        PRetVal = AmbaWrap_memcpy(&(pAdasInfo->AdasCfgV1.Roi),      &(pTbl->AdasCfgV1.Roi),      sizeof(AMBA_CAL_ROI_s)); AmbaMisra_TouchUnused(&PRetVal);
                        PRetVal = AmbaWrap_memcpy(&(pAdasInfo->AdasCfgV1.VoutArea), &(pTbl->AdasCfgV1.VoutArea), sizeof(AMBA_CAL_ROI_s)); AmbaMisra_TouchUnused(&PRetVal);
                        if (pAdasInfo->AdasCfgV1.TransCfg.pCurvedSurface != NULL) {
                            PRetVal = AmbaWrap_memcpy(pAdasInfo->AdasCfgV1.TransCfg.pCurvedSurface, &(pTbl->AdasExtData01), sizeof(AMBA_CAL_EM_CURVED_SURFACE_CFG_s)); AmbaMisra_TouchUnused(&PRetVal);
                        }
                        if (pAdasInfo->AdasCfgV1.TransCfg.pCalibDataRaw2World != NULL) {
                            PRetVal = AmbaWrap_memcpy(pAdasInfo->AdasCfgV1.TransCfg.pCalibDataRaw2World, &(pTbl->AdasExtData02), sizeof(AMBA_CAL_EM_CAM_CALIB_DATA_s)); AmbaMisra_TouchUnused(&PRetVal);
                        }
                        PRetVal = AmbaWrap_memcpy(&pAdasInfo->AdasInfoDataV1, &(pTbl->AdasInfoDataV1), sizeof(AMBA_CAL_EM_CALIB_INFO_DATA_V1_s)); AmbaMisra_TouchUnused(&PRetVal);
                        {
                            SVC_WRAP_PRINT "CameraRotationMatrix[0~4] %5f, %5f, %5f, %5f, %5f"
                            SVC_PRN_ARG_S __func__
                            SVC_PRN_ARG_PROC SvcLog_OK
                            SVC_PRN_ARG_DOUBLE  pAdasInfo->AdasInfoDataV1.CameraRotationMatrix[0]
                            SVC_PRN_ARG_DOUBLE  pAdasInfo->AdasInfoDataV1.CameraRotationMatrix[1]
                            SVC_PRN_ARG_DOUBLE  pAdasInfo->AdasInfoDataV1.CameraRotationMatrix[2]
                            SVC_PRN_ARG_DOUBLE  pAdasInfo->AdasInfoDataV1.CameraRotationMatrix[3]
                            SVC_PRN_ARG_DOUBLE  pAdasInfo->AdasInfoDataV1.CameraRotationMatrix[4]
                            SVC_PRN_ARG_E
                            SVC_WRAP_PRINT "CameraRotationMatrix[5~8] %5f, %5f, %5f, %5f"
                            SVC_PRN_ARG_S __func__
                            SVC_PRN_ARG_PROC SvcLog_OK
                            SVC_PRN_ARG_DOUBLE  pAdasInfo->AdasInfoDataV1.CameraRotationMatrix[5]
                            SVC_PRN_ARG_DOUBLE  pAdasInfo->AdasInfoDataV1.CameraRotationMatrix[6]
                            SVC_PRN_ARG_DOUBLE  pAdasInfo->AdasInfoDataV1.CameraRotationMatrix[7]
                            SVC_PRN_ARG_DOUBLE  pAdasInfo->AdasInfoDataV1.CameraRotationMatrix[8]
                            SVC_PRN_ARG_E
                            SVC_WRAP_PRINT "CameraTranslationMatrix[0~3] %5f, %5f, %5f"
                            SVC_PRN_ARG_S __func__
                            SVC_PRN_ARG_PROC SvcLog_OK
                            SVC_PRN_ARG_DOUBLE  pAdasInfo->AdasInfoDataV1.CameraTranslationMatrix[0]
                            SVC_PRN_ARG_DOUBLE  pAdasInfo->AdasInfoDataV1.CameraTranslationMatrix[1]
                            SVC_PRN_ARG_DOUBLE  pAdasInfo->AdasInfoDataV1.CameraTranslationMatrix[2]
                            SVC_PRN_ARG_E
                            SVC_WRAP_PRINT "CameraEulerAngle Pitch: %5f, Roll: %5f, Yaw: %5f"
                            SVC_PRN_ARG_S __func__
                            SVC_PRN_ARG_PROC SvcLog_OK
                            SVC_PRN_ARG_DOUBLE  pAdasInfo->AdasInfoDataV1.CameraEulerAngle.Pitch
                            SVC_PRN_ARG_DOUBLE  pAdasInfo->AdasInfoDataV1.CameraEulerAngle.Roll
                            SVC_PRN_ARG_DOUBLE  pAdasInfo->AdasInfoDataV1.CameraEulerAngle.Yaw
                            SVC_PRN_ARG_E
                            SVC_WRAP_PRINT "CalculatedCameraPos X: %5f, Y: %5f, Z: %5f"
                            SVC_PRN_ARG_S __func__
                            SVC_PRN_ARG_PROC SvcLog_OK
                            SVC_PRN_ARG_DOUBLE  pAdasInfo->AdasInfoDataV1.CalculatedCameraPos.X
                            SVC_PRN_ARG_DOUBLE  pAdasInfo->AdasInfoDataV1.CalculatedCameraPos.Y
                            SVC_PRN_ARG_DOUBLE  pAdasInfo->AdasInfoDataV1.CalculatedCameraPos.Z
                            SVC_PRN_ARG_E
                        }

                    } else {
                        pAdasInfo->CalibVer = 0U; /* Version 0 */
                        PRetVal = AmbaWrap_memcpy(&(pAdasInfo->AdasCfg.Type),     &(pTbl->AdasCfg.Type),     sizeof(AMBA_CAL_EM_VIEW_TYPE_e)); AmbaMisra_TouchUnused(&PRetVal);
                        PRetVal = AmbaWrap_memcpy(&(pAdasInfo->AdasCfg.CamPos),   &(pTbl->AdasCfg.CamPos),   sizeof(AMBA_CAL_POINT_DB_3D_s)); AmbaMisra_TouchUnused(&PRetVal);
                        PRetVal = AmbaWrap_memcpy(&(pAdasInfo->AdasCfg.Roi),      &(pTbl->AdasCfg.Roi),      sizeof(AMBA_CAL_ROI_s)); AmbaMisra_TouchUnused(&PRetVal);
                        PRetVal = AmbaWrap_memcpy(&(pAdasInfo->AdasCfg.VoutArea), &(pTbl->AdasCfg.VoutArea), sizeof(AMBA_CAL_ROI_s)); AmbaMisra_TouchUnused(&PRetVal);

                        if (pAdasInfo->AdasCfg.TransCfg.pCurvedSurface != NULL) {
                            PRetVal = AmbaWrap_memcpy(pAdasInfo->AdasCfg.TransCfg.pCurvedSurface, &(pTbl->AdasExtData01), sizeof(AMBA_CAL_EM_CURVED_SURFACE_CFG_s)); AmbaMisra_TouchUnused(&PRetVal);
                        }

                        if (pAdasInfo->AdasCfg.TransCfg.pCalibDataRaw2World != NULL) {
                            PRetVal = AmbaWrap_memcpy(pAdasInfo->AdasCfg.TransCfg.pCalibDataRaw2World, &(pTbl->AdasExtData02), sizeof(AMBA_CAL_EM_CAM_CALIB_DATA_s)); AmbaMisra_TouchUnused(&PRetVal);
                        }
                    }

                    AmbaMisra_TouchUnused(pTbl);
                }
            }

            PRetVal = AmbaKAL_MutexGive(&(SVC_CalibAdasCtrl.Mutex));
            if (PRetVal != SVC_OK) {
                SvcLog_NG(SVC_LOG_ADAS, "AmbaKAL_MutexGive error [3]", 0U, 0U);
            }
        }
    }

    return RetVal;
}

/**
 *  Get calib adas data
 *  @param[in] Type Calibration adas type
 *  @param[out] pCfg The pointer of output configuration
 *  @return error code
 */
UINT32 SvcCalib_AdasCfgGet(UINT32 Type, void *pCfg)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibAdasCtrlFlag & SVC_CALIB_ADAS_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ADAS, "Failure to get adas cfg - initial adas module first!", 0U, 0U);
    } else {
        if ((SVC_CalibAdasCtrlFlag & SVC_CALIB_ADAS_FLG_SHADOW_INIT) == 0U) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ADAS, "Failure to get adas cfg - initial adas dram shadow first!", 0U, 0U);
        } else {
            if (SVC_CalibAdasCtrl.pNandHeader == NULL) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ADAS, "Failure to get adas cfg - initial adas dram shadow first.", 0U, 0U);
            }
            if (SVC_CalibAdasCtrl.pNandTable == NULL) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ADAS, "Failure to get adas cfg - initial adas dram shadow first!!", 0U, 0U);
            }
        }
    }

    if ((Type >= SVC_CALIB_ADAS_TYPE_NUM) || (Type == 0U)) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ADAS, "Failure to get adas cfg - invalid type(%d)", Type, 0U);
    }

    if (pCfg == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ADAS, "Failure to get adas cfg - input cfg should not null!", 0U, 0U);
    } else {
        AmbaMisra_TouchUnused(pCfg);
    }

    if (RetVal == SVC_OK) {
        SVC_CALIB_ADAS_NAND_TABLE_s *pTbl = NULL;

        if (KAL_ERR_NONE != AmbaKAL_MutexTake(&(SVC_CalibAdasCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ADAS, "Failure to get adas cfg - take mutex fail!", 0U, 0U);
        } else {
            pTbl = SvcCalib_AdasTableGet(Type);

            if (pTbl == NULL) {
                RetVal = SVC_NG;
            } else {
                SvcLog_OK(SVC_LOG_ADAS, "Type = %d, Version = 0x%X", pTbl->Header.Type, pTbl->Header.Version);
                if (Type < SVC_CALIB_ADAS_TYPE_AVM_F) {//EMIRROR CALIB
                    AMBA_CAL_EM_CALC_COORD_CFG_s *pAdasCfg = NULL;

                    AmbaMisra_TypeCast(&pAdasCfg, &pCfg);

                    PRetVal = AmbaWrap_memcpy(&(pAdasCfg->Type),     &(pTbl->AdasCfg.Type),     sizeof(AMBA_CAL_EM_VIEW_TYPE_e)); AmbaMisra_TouchUnused(&PRetVal);
                    PRetVal = AmbaWrap_memcpy(&(pAdasCfg->CamPos),   &(pTbl->AdasCfg.CamPos),   sizeof(AMBA_CAL_POINT_DB_3D_s)); AmbaMisra_TouchUnused(&PRetVal);
                    PRetVal = AmbaWrap_memcpy(&(pAdasCfg->Roi),      &(pTbl->AdasCfg.Roi),      sizeof(AMBA_CAL_ROI_s)); AmbaMisra_TouchUnused(&PRetVal);
                    PRetVal = AmbaWrap_memcpy(&(pAdasCfg->VoutArea), &(pTbl->AdasCfg.VoutArea), sizeof(AMBA_CAL_ROI_s)); AmbaMisra_TouchUnused(&PRetVal);

                    if (pAdasCfg->TransCfg.pCurvedSurface != NULL) {
                        PRetVal = AmbaWrap_memcpy(pAdasCfg->TransCfg.pCurvedSurface, &(pTbl->AdasExtData01), sizeof(AMBA_CAL_EM_CURVED_SURFACE_CFG_s)); AmbaMisra_TouchUnused(&PRetVal);
                    }

                    if (pAdasCfg->TransCfg.pCalibDataRaw2World != NULL) {
                        PRetVal = AmbaWrap_memcpy(pAdasCfg->TransCfg.pCalibDataRaw2World, &(pTbl->AdasExtData02), sizeof(AMBA_CAL_EM_CAM_CALIB_DATA_s)); AmbaMisra_TouchUnused(&PRetVal);
                    }

                    AmbaMisra_TouchUnused(pTbl);
                } else {//AVM CALIB
                    SVC_CALIB_ADAS_NAND_TABLE_s *OutTbl = NULL;
                    AmbaMisra_TypeCast(&OutTbl, &pCfg);
                    PRetVal = AmbaWrap_memcpy(&(OutTbl->AdasAvmCalibCfg),     &(pTbl->AdasAvmCalibCfg),     sizeof(AMBA_CAL_AVM_CALIB_DATA_s)); AmbaMisra_TouchUnused(&PRetVal);
                    PRetVal = AmbaWrap_memcpy(&(OutTbl->AdasAvmPaCfg.Type),     &(pTbl->AdasAvmPaCfg.Type),     sizeof(AMBA_CAL_AVM_VIEW_TYPE_e)); AmbaMisra_TouchUnused(&PRetVal);
                    PRetVal = AmbaWrap_memcpy(&(OutTbl->AdasAvmPaCfg.RawArea),     &(pTbl->AdasAvmPaCfg.RawArea),     sizeof(AMBA_CAL_ROI_s)); AmbaMisra_TouchUnused(&PRetVal);
                    PRetVal = AmbaWrap_memcpy(&(OutTbl->AdasAvmPaCfg.CamDirection),     &(pTbl->AdasAvmPaCfg.CamDirection),     sizeof(AMBA_CAL_POINT_DB_3D_s)); AmbaMisra_TouchUnused(&PRetVal);
                    PRetVal = AmbaWrap_memcpy(&(OutTbl->AdasAvmPaCfg.HMatrix[0]),     &(pTbl->AdasAvmPaCfg.HMatrix[0]),     sizeof(DOUBLE)*9U); AmbaMisra_TouchUnused(&PRetVal);
                    PRetVal = AmbaWrap_memcpy(&(OutTbl->AdasAvmPaCfg.PMatrix[0]),     &(pTbl->AdasAvmPaCfg.PMatrix[0]),     sizeof(DOUBLE)*9U); AmbaMisra_TouchUnused(&PRetVal);
                    if (OutTbl->AdasAvmPaCfg.Cfg.pLDC != NULL) {
                       PRetVal = AmbaWrap_memcpy(OutTbl->AdasAvmPaCfg.Cfg.pLDC,     &(pTbl->AdasExtData05),     sizeof(AMBA_CAL_AVM_MV_LDC_CFG_s)); AmbaMisra_TouchUnused(&PRetVal);
                    }
                    if (OutTbl->AdasAvmPaCfg.Cfg.pPerspective != NULL) {
                       PRetVal = AmbaWrap_memcpy(OutTbl->AdasAvmPaCfg.Cfg.pPerspective,     &(pTbl->AdasExtData03),     sizeof(AMBA_CAL_AVM_MV_PERSPET_V1_CFG_s)); AmbaMisra_TouchUnused(&PRetVal);
                    }
                    if (OutTbl->AdasAvmPaCfg.Cfg.pEqualDistance != NULL) {
                       PRetVal = AmbaWrap_memcpy(OutTbl->AdasAvmPaCfg.Cfg.pEqualDistance,     &(pTbl->AdasExtData04),     sizeof(AMBA_CAL_AVM_MV_EQ_DISTANCE_s)); AmbaMisra_TouchUnused(&PRetVal);
                    }
                    if (OutTbl->AdasAvmPaCfg.Cfg.pFloor != NULL) {
                       PRetVal = AmbaWrap_memcpy(OutTbl->AdasAvmPaCfg.Cfg.pFloor,     &(pTbl->AdasExtData06),     sizeof(AMBA_CAL_AVM_MV_FLOOR_CFG_s)); AmbaMisra_TouchUnused(&PRetVal);
                    }
                    if (OutTbl->AdasAvmPaCfg.Cam.Lens.LensDistoSpec.pRealExpect!= NULL) {
                        OutTbl->AdasAvmPaCfg.Cam.Lens.LensDistoSpec.pRealExpect->Length = pTbl->AdasExtData07.Length;
                        if (OutTbl->AdasAvmPaCfg.Cam.Lens.LensDistoSpec.pRealExpect->pExpectTbl!= NULL) {
                            PRetVal = AmbaWrap_memcpy(OutTbl->AdasAvmPaCfg.Cam.Lens.LensDistoSpec.pRealExpect->pExpectTbl, &(pTbl->AdasExtData08[0]), sizeof(DOUBLE)*MAX_LENS_DISTO_TBL_LEN); AmbaMisra_TouchUnused(&PRetVal);
                        }
                        if (OutTbl->AdasAvmPaCfg.Cam.Lens.LensDistoSpec.pRealExpect->pRealTbl!= NULL) {
                            PRetVal = AmbaWrap_memcpy(OutTbl->AdasAvmPaCfg.Cam.Lens.LensDistoSpec.pRealExpect->pRealTbl, &(pTbl->AdasExtData09[0]), sizeof(DOUBLE)*MAX_LENS_DISTO_TBL_LEN); AmbaMisra_TouchUnused(&PRetVal);
                        }
                    }

                    PRetVal = AmbaWrap_memcpy(&(OutTbl->AdasAvmPaCfg.Cam.OpticalCenter),     &(pTbl->AdasAvmPaCfg.Cam.OpticalCenter),     sizeof(AMBA_CAL_POINT_DB_2D_s)); AmbaMisra_TouchUnused(&PRetVal);
                    PRetVal = AmbaWrap_memcpy(&(OutTbl->AdasAvmPaCfg.Cam.Sensor),     &(pTbl->AdasAvmPaCfg.Cam.Sensor),     sizeof(AMBA_CAL_SENSOR_s)); AmbaMisra_TouchUnused(&PRetVal);
                    PRetVal = AmbaWrap_memcpy(&(OutTbl->AdasAvmPaCfg.Cam.Pos),     &(pTbl->AdasAvmPaCfg.Cam.Pos),     sizeof(AMBA_CAL_POINT_DB_3D_s)); AmbaMisra_TouchUnused(&PRetVal);
                    PRetVal = AmbaWrap_memcpy(&(OutTbl->AdasAvmPaCfg.Cam.Rotation),     &(pTbl->AdasAvmPaCfg.Cam.Rotation),     sizeof(AMBA_CAL_ROTATION_e)); AmbaMisra_TouchUnused(&PRetVal);
                    PRetVal = AmbaWrap_memcpy(&(OutTbl->AdasAvm2DCfg),     &(pTbl->AdasAvm2DCfg),     sizeof(AMBA_CAL_AVM_2D_CFG_V2_s)); AmbaMisra_TouchUnused(&PRetVal);


                }
            }

            PRetVal = AmbaKAL_MutexGive(&(SVC_CalibAdasCtrl.Mutex));
            if (PRetVal != SVC_OK) {
                SvcLog_NG(SVC_LOG_ADAS, "AmbaKAL_MutexGive error [4]", 0U, 0U);
            }
        }
    }

    return RetVal;
}

/**
 *  Set calib adas data
 *  @param[in] Type Calibration adas type
 *  @param[in] pCfg The pointer of input configuration
 *  @return error code
 */
UINT32 SvcCalib_AdasCfgSet(UINT32 Type, void *pCfg)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibAdasCtrlFlag & SVC_CALIB_ADAS_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ADAS, "Failure to set adas cfg - initial adas module first!", 0U, 0U);
    } else {
        if ((SVC_CalibAdasCtrlFlag & SVC_CALIB_ADAS_FLG_SHADOW_INIT) == 0U) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ADAS, "Failure to set adas cfg - initial adas dram shadow first!", 0U, 0U);
        } else {
            if (SVC_CalibAdasCtrl.pNandHeader == NULL) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ADAS, "Failure to set adas cfg - initial adas dram shadow first.", 0U, 0U);
            }
            if (SVC_CalibAdasCtrl.pNandTable == NULL) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ADAS, "Failure to set adas cfg - initial adas dram shadow first!!", 0U, 0U);
            }
        }
    }

    if ((Type >= SVC_CALIB_ADAS_TYPE_NUM) || (Type == 0U)) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ADAS, "Failure to set adas cfg - invalid type(%d)", Type, 0U);
    }

    if (pCfg == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ADAS, "Failure to set adas cfg - input cfg should not null!", 0U, 0U);
    } else {
        AmbaMisra_TouchUnused(pCfg);
    }

    if (RetVal == SVC_OK) {
        SVC_CALIB_ADAS_NAND_TABLE_s *pTbl = NULL;

        if (KAL_ERR_NONE != AmbaKAL_MutexTake(&(SVC_CalibAdasCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ADAS, "Failure to set adas cfg - take mutex fail!", 0U, 0U);
        } else {
            pTbl = SvcCalib_AdasTableGet(Type);

            if (pTbl == NULL) {
                pTbl = SvcCalib_AdasFreeTableGet();
                if (pTbl != NULL) {
                    SVC_CalibAdasCtrl.pNandHeader->NumOfTable += 1U;
                }
            }

            if (pTbl != NULL) {
                if (Type < SVC_CALIB_ADAS_TYPE_AVM_F) {//EMIRROR CALIB
                    const AMBA_CAL_EM_CALC_COORD_CFG_s *pAdasCfg = NULL;

                    AmbaMisra_TypeCast(&(pAdasCfg), &(pCfg));
                    PRetVal = AmbaWrap_memcpy(&(pTbl->AdasCfg), pAdasCfg, sizeof(AMBA_CAL_EM_CALC_COORD_CFG_s)); AmbaMisra_TouchUnused(&PRetVal);
                    if (pAdasCfg->TransCfg.pCurvedSurface != NULL) {
                        PRetVal = AmbaWrap_memcpy(&(pTbl->AdasExtData01), pAdasCfg->TransCfg.pCurvedSurface, sizeof(AMBA_CAL_EM_CURVED_SURFACE_CFG_s)); AmbaMisra_TouchUnused(&PRetVal);
                    }
                    if (pAdasCfg->TransCfg.pCalibDataRaw2World != NULL) {
                        PRetVal = AmbaWrap_memcpy(&(pTbl->AdasExtData02), pAdasCfg->TransCfg.pCalibDataRaw2World, sizeof(AMBA_CAL_EM_CAM_CALIB_DATA_s)); AmbaMisra_TouchUnused(&PRetVal);
                    }

                    pTbl->Header.Enable  = 1U;
                    pTbl->Header.Type    = Type;
                    pTbl->Header.Version = SVC_CALIB_ADAS_VERSION;

                    SVC_CalibAdasCtrl.pNandHeader->DebugMsgOn = SVC_CalibAdasCtrl.EnableDebugMsg;
                    SVC_CalibAdasCtrl.pNandHeader->Enable = 1U;

                    PRetVal = SvcCalib_DataSave(SVC_CALIB_ADAS_ID);
                    if (PRetVal != SVC_OK) {
                        RetVal = SVC_NG;
                        SvcLog_NG(SVC_LOG_ADAS, "Failure to set adas cfg - write to nand fail!", 0U, 0U);
                    }
                } else {//AVM CALIB
                    const SVC_CALIB_ADAS_NAND_TABLE_s *pAdasCfg = NULL;

                    AmbaMisra_TypeCast(&(pAdasCfg), &(pCfg));
                    PRetVal = AmbaWrap_memcpy(&(pTbl->AdasAvmCalibCfg), &pAdasCfg->AdasAvmCalibCfg, sizeof(AMBA_CAL_AVM_CALIB_DATA_s)); AmbaMisra_TouchUnused(&PRetVal);
                    PRetVal = AmbaWrap_memcpy(&(pTbl->AdasAvmPaCfg), &pAdasCfg->AdasAvmPaCfg, sizeof(AMBA_CAL_AVM_PARK_AST_CFG_V1_s)); AmbaMisra_TouchUnused(&PRetVal);
                    if (pAdasCfg->AdasAvmPaCfg.Cfg.pPerspective!= NULL) {
                        PRetVal = AmbaWrap_memcpy(&(pTbl->AdasExtData03), pAdasCfg->AdasAvmPaCfg.Cfg.pPerspective, sizeof(AMBA_CAL_AVM_MV_PERSPET_V1_CFG_s)); AmbaMisra_TouchUnused(&PRetVal);
                    }
                    if (pAdasCfg->AdasAvmPaCfg.Cfg.pEqualDistance!= NULL) {
                        PRetVal = AmbaWrap_memcpy(&(pTbl->AdasExtData04), pAdasCfg->AdasAvmPaCfg.Cfg.pEqualDistance, sizeof(AMBA_CAL_AVM_MV_EQ_DISTANCE_s)); AmbaMisra_TouchUnused(&PRetVal);
                    }
                    if (pAdasCfg->AdasAvmPaCfg.Cfg.pLDC!= NULL) {
                        PRetVal = AmbaWrap_memcpy(&(pTbl->AdasExtData05), pAdasCfg->AdasAvmPaCfg.Cfg.pLDC, sizeof(AMBA_CAL_AVM_MV_LDC_CFG_s)); AmbaMisra_TouchUnused(&PRetVal);
                    }
                    if (pAdasCfg->AdasAvmPaCfg.Cfg.pFloor!= NULL) {
                        PRetVal = AmbaWrap_memcpy(&(pTbl->AdasExtData06), pAdasCfg->AdasAvmPaCfg.Cfg.pFloor, sizeof(AMBA_CAL_AVM_MV_FLOOR_CFG_s)); AmbaMisra_TouchUnused(&PRetVal);
                    }
                    if (pAdasCfg->AdasAvmPaCfg.Cam.Lens.LensDistoSpec.pRealExpect!= NULL) {
                        PRetVal = AmbaWrap_memcpy(&(pTbl->AdasExtData07), pAdasCfg->AdasAvmPaCfg.Cam.Lens.LensDistoSpec.pRealExpect, sizeof(AMBA_CAL_LENS_DST_REAL_EXPECT_s)); AmbaMisra_TouchUnused(&PRetVal);
                        if (pAdasCfg->AdasAvmPaCfg.Cam.Lens.LensDistoSpec.pRealExpect->pExpectTbl!= NULL) {
                            PRetVal = AmbaWrap_memcpy(&(pTbl->AdasExtData08[0]), pAdasCfg->AdasAvmPaCfg.Cam.Lens.LensDistoSpec.pRealExpect->pExpectTbl, sizeof(DOUBLE)*(pAdasCfg->AdasAvmPaCfg.Cam.Lens.LensDistoSpec.pRealExpect->Length)); AmbaMisra_TouchUnused(&PRetVal);
                        }
                        if (pAdasCfg->AdasAvmPaCfg.Cam.Lens.LensDistoSpec.pRealExpect->pRealTbl!= NULL) {
                            PRetVal = AmbaWrap_memcpy(&(pTbl->AdasExtData09[0]), pAdasCfg->AdasAvmPaCfg.Cam.Lens.LensDistoSpec.pRealExpect->pRealTbl, sizeof(DOUBLE)*(pAdasCfg->AdasAvmPaCfg.Cam.Lens.LensDistoSpec.pRealExpect->Length)); AmbaMisra_TouchUnused(&PRetVal);
                        }
                    }
                    PRetVal = AmbaWrap_memcpy(&(pTbl->AdasAvm2DCfg), &pAdasCfg->AdasAvm2DCfg, sizeof(AMBA_CAL_AVM_2D_CFG_V2_s)); AmbaMisra_TouchUnused(&PRetVal);

                    pTbl->Header.Enable  = 1U;
                    pTbl->Header.Type    = Type;
                    pTbl->Header.Version = SVC_CALIB_ADAS_VERSION;

                    SVC_CalibAdasCtrl.pNandHeader->DebugMsgOn = SVC_CalibAdasCtrl.EnableDebugMsg;
                    SVC_CalibAdasCtrl.pNandHeader->Enable = 1U;

                    PRetVal = SvcCalib_DataSave(SVC_CALIB_ADAS_ID);
                    if (PRetVal != SVC_OK) {
                        RetVal = SVC_NG;
                        SvcLog_NG(SVC_LOG_ADAS, "Failure to set adas cfg - write to nand fail!", 0U, 0U);
                    }
                }
            } else {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ADAS, "Failure to set adas cfg - not free space!", 0U, 0U);
            }

            PRetVal = AmbaKAL_MutexGive(&(SVC_CalibAdasCtrl.Mutex));
            if (PRetVal != SVC_OK) {
                SvcLog_NG(SVC_LOG_ADAS, "AmbaKAL_MutexGive error [5]", 0U, 0U);
            }
        }
    }

    return RetVal;
}

/**
 *  Set calib adas data (V1)
 *  @param[in] Type Calibration adas type
 *  @param[in] pCfg The pointer of input configuration
 *  @return error code
 */
UINT32 SvcCalib_AdasCfgSetV1(UINT32 Type, void *pCfg, void *pCfg2)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibAdasCtrlFlag & SVC_CALIB_ADAS_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ADAS, "Failure to set adas cfg - initial adas module first!", 0U, 0U);
    } else {
        if ((SVC_CalibAdasCtrlFlag & SVC_CALIB_ADAS_FLG_SHADOW_INIT) == 0U) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ADAS, "Failure to set adas cfg - initial adas dram shadow first!", 0U, 0U);
        } else {
            if (SVC_CalibAdasCtrl.pNandHeader == NULL) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ADAS, "Failure to set adas cfg - initial adas dram shadow first.", 0U, 0U);
            }
            if (SVC_CalibAdasCtrl.pNandTable == NULL) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ADAS, "Failure to set adas cfg - initial adas dram shadow first!!", 0U, 0U);
            }
        }
    }

    if ((Type >= SVC_CALIB_ADAS_TYPE_NUM) || (Type == 0U)) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ADAS, "Failure to set adas cfg - invalid type(%d)", Type, 0U);
    }

    if ((pCfg == NULL) || (pCfg2 == NULL)) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ADAS, "Failure to set adas cfg - input cfg should not null!", 0U, 0U);
    } else {
        AmbaMisra_TouchUnused(pCfg);
        AmbaMisra_TouchUnused(pCfg2);
    }

    if (RetVal == SVC_OK) {
        SVC_CALIB_ADAS_NAND_TABLE_s *pTbl = NULL;

        if (KAL_ERR_NONE != AmbaKAL_MutexTake(&(SVC_CalibAdasCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ADAS, "Failure to set adas cfg - take mutex fail!", 0U, 0U);
        } else {
            pTbl = SvcCalib_AdasTableGet(Type);

            if (pTbl == NULL) {
                pTbl = SvcCalib_AdasFreeTableGet();
                if (pTbl != NULL) {
                    SVC_CalibAdasCtrl.pNandHeader->NumOfTable += 1U;
                }
            }

            if (pTbl != NULL) {
                if (Type < SVC_CALIB_ADAS_TYPE_AVM_F) {//EMIRROR CALIB
                    const AMBA_CAL_EM_CALC_COORD_CFG_V1_s  *pAdasCfg = NULL;
                    const AMBA_CAL_EM_CALIB_INFO_DATA_V1_s *pAdasInfoDataV1 = NULL;

                    AmbaMisra_TypeCast(&(pAdasCfg), &(pCfg));
                    AmbaMisra_TypeCast(&(pAdasInfoDataV1), &(pCfg2));
                    PRetVal = AmbaWrap_memcpy(&(pTbl->AdasCfgV1), pAdasCfg, sizeof(AMBA_CAL_EM_CALC_COORD_CFG_V1_s)); AmbaMisra_TouchUnused(&PRetVal);
                    PRetVal = AmbaWrap_memcpy(&(pTbl->AdasInfoDataV1), pAdasInfoDataV1, sizeof(AMBA_CAL_EM_CALIB_INFO_DATA_V1_s)); AmbaMisra_TouchUnused(&PRetVal);
                    if (pAdasCfg->TransCfg.pCurvedSurface != NULL) {
                        PRetVal = AmbaWrap_memcpy(&(pTbl->AdasExtData01), pAdasCfg->TransCfg.pCurvedSurface, sizeof(AMBA_CAL_EM_CURVED_SURFACE_CFG_s)); AmbaMisra_TouchUnused(&PRetVal);
                    }
                    if (pAdasCfg->TransCfg.pCalibDataRaw2World != NULL) {
                        PRetVal = AmbaWrap_memcpy(&(pTbl->AdasExtData02), pAdasCfg->TransCfg.pCalibDataRaw2World, sizeof(AMBA_CAL_EM_CAM_CALIB_DATA_s)); AmbaMisra_TouchUnused(&PRetVal);
                    }

                    pTbl->Header.Enable  = 1U;
                    pTbl->Header.Type    = Type;
                    pTbl->Header.Version = SVC_CALIB_ADAS_VERSION_V1;

                    SVC_CalibAdasCtrl.pNandHeader->DebugMsgOn = SVC_CalibAdasCtrl.EnableDebugMsg;
                    SVC_CalibAdasCtrl.pNandHeader->Enable = 1U;

                    PRetVal = SvcCalib_DataSave(SVC_CALIB_ADAS_ID);
                    if (PRetVal != SVC_OK) {
                        RetVal = SVC_NG;
                        SvcLog_NG(SVC_LOG_ADAS, "Failure to set adas cfg - write to nand fail!", 0U, 0U);
                    }
                }
            } else {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ADAS, "Failure to set adas cfg - not free space!", 0U, 0U);
            }

            PRetVal = AmbaKAL_MutexGive(&(SVC_CalibAdasCtrl.Mutex));
            if (PRetVal != SVC_OK) {
                SvcLog_NG(SVC_LOG_ADAS, "AmbaKAL_MutexGive error [6]", 0U, 0U);
            }
        }
    }

    return RetVal;
}

/**
 *  Enable/disable calib debug message
 *  @param[in] Enable Enable or Disable calib dbg message
 *  @return error code
 */
UINT32 SvcCalib_AdasDbgMsg(UINT32 Enable)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_CalibAdasCtrlFlag & SVC_CALIB_ADAS_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_ADAS, "Failure to cfg adas debug msg - initial adas module first!", 0U, 0U);
    } else {
        if ((SVC_CalibAdasCtrlFlag & SVC_CALIB_ADAS_FLG_SHADOW_INIT) == 0U) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ADAS, "Failure to cfg adas debug msg - initial adas dram shadow first!", 0U, 0U);
        } else {
            if (SVC_CalibAdasCtrl.pNandHeader == NULL) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ADAS, "Failure to cfg adas debug msg - initial adas dram shadow first.", 0U, 0U);
            }
            if (SVC_CalibAdasCtrl.pNandHeader == NULL) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ADAS, "Failure to cfg adas debug msg - initial adas dram shadow first!!", 0U, 0U);
            }
        }
    }

    if (RetVal == SVC_OK) {
        if (KAL_ERR_NONE != AmbaKAL_MutexTake(&(SVC_CalibAdasCtrl.Mutex), AMBA_KAL_WAIT_FOREVER)) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_ADAS, "Failure to cfg adas debug msg - take mutex fail!", 0U, 0U);
        } else {

            SVC_CalibAdasCtrl.EnableDebugMsg = Enable;
            SVC_CalibAdasCtrl.pNandHeader->DebugMsgOn = SVC_CalibAdasCtrl.EnableDebugMsg;

            PRetVal = SvcCalib_DataSave(SVC_CALIB_ADAS_ID);
            if (PRetVal != SVC_OK) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_ADAS, "Failure to cfg adas debug msg - write to nand fail!", 0U, 0U);
            }

            PRetVal = AmbaKAL_MutexGive(&(SVC_CalibAdasCtrl.Mutex));
            if (PRetVal != SVC_OK) {
                SvcLog_NG(SVC_LOG_ADAS, "AmbaKAL_MutexGive error [7]", 0U, 0U);
            }
        }
    }

    return RetVal;
}
