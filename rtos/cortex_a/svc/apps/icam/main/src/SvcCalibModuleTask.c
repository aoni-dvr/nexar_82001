/**
 *  @file SvcCalibModuleTask.c
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
 *  @details svc calibration task
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"
#include "AmbaShell.h"
#include "AmbaEEPROM.h"

#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_Liveview_Def.h"

#include "AmbaCameraModule.h"

#include "SvcErrCode.h"
#include "SvcMem.h"
#include "SvcBufMap.h"
#include "SvcBuffer.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcCmd.h"
#include "SvcVinSrc.h"
#include "SvcResCfg.h"
#include "SvcCalibMgr.h"
#include "SvcCalibModMgr.h"
#include "SvcCalibCfg.h"
#include "SvcCalibModuleCfg.h"
#include "SvcCalibTask.h"
#include "SvcCalibModuleTask.h"

#define SVC_CALIB_TSK_MODULE_CTRL_INIT      (0x1U)
#define SVC_CALIB_TSK_MODULE_SHELL_INIT     (0x2U)
#define SVC_CALIB_TSK_MODULE_UPD_OFF        (0x4U)

typedef struct {
    UINT32 Usaged;
    UINT8 *pBuf;
    UINT32 BufSize;
} SVC_CALIB_TSK_MODULE_MAX_NUM_s;

typedef struct {
    UINT32                         ItemCfgNum;
    SVC_MODULE_ITEM_CFG_s         *pItemCfg;
    SVC_CALIB_TSK_MODULE_MAX_NUM_s MemSlot[SVC_CALIB_TSK_MODULE_MAX_NUM + 1U];
    SVC_CALIB_TSK_MODULE_MAX_NUM_s MemUpd;
    UINT32                         EnaFovFlag;
    void                          *pModuleCtrl[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT32                         SerDesSelectBits[AMBA_DSP_MAX_VIN_NUM];
} SVC_CALIB_TSK_MODULE_s;

static void   SvcCalibTask_Modulememset(void *ptr, INT32 v, SIZE_t n);
static void   SvcCalibTask_Modulememcpy(void *pDst, const void *pSrc, SIZE_t num);
static void   SvcCalibTask_ModuleErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode);

static UINT32 SvcCalibTask_ModuleCalChanGet(UINT32 FovID, SVC_CALIB_CHANNEL_s *pChan);
static UINT32 SvcCalibTask_ModuleMemCfg(UINT32 FovID, const AMBA_EEPROM_CHANNEL_s *pChan, UINT32 NumOfCfg, SVC_MODULE_ITEM_CFG_s *pCfg, UINT8 **pMemBuf, UINT32 *pMemSize);
static UINT32 SvcCalibTask_ModuleTblIdxGet(UINT32 CalibID, SVC_CALIB_CHANNEL_s *pChan, UINT32 *pTblIdx);
static UINT32 SvcCalibTask_ModuleVigSet(void *pModuleCtrl, SVC_CALIB_CHANNEL_s *pChan, UINT32 TableIdx);
static UINT32 SvcCalibTask_ModuleVigUpd(void *pModuleCtrl, SVC_CALIB_CHANNEL_s *pChan);
static UINT32 SvcCalibTask_ModuleBpcSet(void *pModuleCtrl, SVC_CALIB_CHANNEL_s *pChan);
static UINT32 SvcCalibTask_ModuleBpcUpd(void *pModuleCtrl, SVC_CALIB_CHANNEL_s *pChan);
static UINT32 SvcCalibTask_ModuleBpcInfo(void *pModuleCtrl);
static UINT32 SvcCalibTask_ModuleCamInfoSet(void *pModuleCtrl, FULLVIN_INTRINSIC_INFORMATION_s *pCameraInfo);
static UINT32 SvcCalibTask_ModuleCamInfoGet(void *pModuleCtrl, FULLVIN_INTRINSIC_INFORMATION_s *pCameraInfo);
static UINT32 SvcCalibTask_ModuleWbSet(void *pModuleCtrl, SVC_CALIB_CHANNEL_s *pChan, UINT32 TableIdx);
static UINT32 SvcCalibTask_ModuleWbUpd(void *pModuleCtrl, SVC_CALIB_CHANNEL_s *pChan);
static UINT32 SvcCalibTask_ModuleWbGet(void *pModuleCtrl, WHITE_BLANCE_s *pWbData);
static void   SvcCalibTask_ModuleSerDesQryPause(UINT32 VinID);
static void   SvcCalibTask_ModuleSerDesQryResume(UINT32 VinID);

static SVC_CALIB_TSK_MODULE_s CalibModuleCtrl GNU_SECTION_NOZEROINIT;
static UINT32                 CalibModuleCtrlFlg = 0U;

typedef UINT32 (*SVC_CALIB_MODULE_SHELL_FUNC_f)(UINT32 ArgCount, char * const *pArgVector);
typedef void   (*SVC_CALIB_MODULE_SHELL_USAGE_f)(UINT32 CtrlFlag);

typedef struct {
    UINT32                         Enable;
    char                           CmdName[32];
    SVC_CALIB_MODULE_SHELL_FUNC_f  pFunc;
    SVC_CALIB_MODULE_SHELL_USAGE_f pUsageFunc;
} SVC_CALIB_MODULE_SHELL_FUNC_s;

static void   SvcCalibTask_ModShellStrToU32(const char *pString, UINT32 *pValue);
static UINT32 SvcCalibTask_ModShellInit(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalibTask_ModShellInitU(UINT32 CtrlFlag);
static UINT32 SvcCalibTask_ModShellCreate(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalibTask_ModShellCreateU(UINT32 CtrlFlag);
static UINT32 SvcCalibTask_ModShellDelete(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalibTask_ModShellDeleteU(UINT32 CtrlFlag);
static UINT32 SvcCalibTask_ModShellSave(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalibTask_ModShellSaveU(UINT32 CtrlFlag);
static UINT32 SvcCalibTask_ModShellInfo(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalibTask_ModShellInfoU(UINT32 CtrlFlag);
static UINT32 SvcCalibTask_ModShellReset(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalibTask_ModShellResetU(UINT32 CtrlFlag);
static UINT32 SvcCalibTask_ModShellDbgMsg(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalibTask_ModShellDbgMsgU(UINT32 CtrlFlag);
static void   SvcCalibTask_ModShellUsage(UINT32 CtrlFlag);
static void   SvcCalibTask_ModShellEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
static void   SvcCalibTask_ModShellEntryInit(void);
static void   SvcCalibTask_ModShellInstall(void);

static UINT32 SvcCalibTask_ModShellVig(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalibTask_ModShellVigU(UINT32 CtrlFlag);
static UINT32 SvcCalibTask_ModShellVigSet(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalibTask_ModShellVigSetU(UINT32 CtrlFlag);
static UINT32 SvcCalibTask_ModShellVigUpd(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalibTask_ModShellVigUpdU(UINT32 CtrlFlag);
#define SVC_CALIB_MODULE_SHELL_VIG_NUM (2U)
static SVC_CALIB_MODULE_SHELL_FUNC_s CalibModuleShellVig[SVC_CALIB_MODULE_SHELL_VIG_NUM] GNU_SECTION_NOZEROINIT;

static UINT32 SvcCalibTask_ModShellBpc(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalibTask_ModShellBpcU(UINT32 CtrlFlag);
static UINT32 SvcCalibTask_ModShellBpcSet(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalibTask_ModShellBpcSetU(UINT32 CtrlFlag);
static UINT32 SvcCalibTask_ModShellBpcUpd(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalibTask_ModShellBpcUpdU(UINT32 CtrlFlag);
static UINT32 SvcCalibTask_ModShellBpcInfo(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalibTask_ModShellBpcInfoU(UINT32 CtrlFlag);
#define SVC_CALIB_MODULE_SHELL_BPC_NUM (3U)
static SVC_CALIB_MODULE_SHELL_FUNC_s CalibModuleShellBpc[SVC_CALIB_MODULE_SHELL_BPC_NUM] GNU_SECTION_NOZEROINIT;

static UINT32 SvcCalibTask_ModShellCam(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalibTask_ModShellCamU(UINT32 CtrlFlag);
static UINT32 SvcCalibTask_ModShellCamSet(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalibTask_ModShellCamSetU(UINT32 CtrlFlag);
static UINT32 SvcCalibTask_ModShellCamGet(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalibTask_ModShellCamGetU(UINT32 CtrlFlag);
#define SVC_CALIB_MODULE_SHELL_CAM_NUM (2U)
static SVC_CALIB_MODULE_SHELL_FUNC_s CalibModuleShellCamInfo[SVC_CALIB_MODULE_SHELL_CAM_NUM] GNU_SECTION_NOZEROINIT;

static UINT32 SvcCalibTask_ModShellWb(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalibTask_ModShellWbU(UINT32 CtrlFlag);
static UINT32 SvcCalibTask_ModShellWbSet(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalibTask_ModShellWbSetU(UINT32 CtrlFlag);
static UINT32 SvcCalibTask_ModShellWbGet(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalibTask_ModShellWbGetU(UINT32 CtrlFlag);
static UINT32 SvcCalibTask_ModShellWbUpd(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalibTask_ModShellWbUpdU(UINT32 CtrlFlag);
#define SVC_CALIB_MODULE_SHELL_WB_NUM (3U)
static SVC_CALIB_MODULE_SHELL_FUNC_s CalibModuleShellWb[SVC_CALIB_MODULE_SHELL_WB_NUM] GNU_SECTION_NOZEROINIT;

#define SVC_CALIB_MODULE_SHELL_CMD_NUM  (13U)
static SVC_CALIB_MODULE_SHELL_FUNC_s CalibModuleShellFunc[SVC_CALIB_MODULE_SHELL_CMD_NUM] GNU_SECTION_NOZEROINIT;

#define SVC_LOG_MOD_TSK "CALMOD_TSK"
#define PRN_CAL_MTSK_LOG   { SVC_WRAP_PRINT_s CalibModulePrint; SvcCalibTask_Modulememset(&(CalibModulePrint), 0, sizeof(CalibModulePrint)); CalibModulePrint.Argc --; CalibModulePrint.pStrFmt =
#define PRN_CAL_MTSK_ARG_DOUBLE ; CalibModulePrint.Argc ++; CalibModulePrint.Argv[CalibModulePrint.Argc].Doubld   =
#define PRN_CAL_MTSK_ARG_UINT32 ; CalibModulePrint.Argc ++; CalibModulePrint.Argv[CalibModulePrint.Argc].Uint64   = (UINT64)((
#define PRN_CAL_MTSK_ARG_CSTR   ; CalibModulePrint.Argc ++; CalibModulePrint.Argv[CalibModulePrint.Argc].pCStr    = ((
#define PRN_CAL_MTSK_ARG_CPOINT ; CalibModulePrint.Argc ++; CalibModulePrint.Argv[CalibModulePrint.Argc].pPointer = ((
#define PRN_CAL_MTSK_ARG_POST   ))
#define PRN_CAL_MTSK_OK         ; CalibModulePrint.Argc ++; SvcCalibTask_ModulePrintLog(SVC_LOG_CAL_OK , &(CalibModulePrint)); }
#define PRN_CAL_MTSK_NG         ; CalibModulePrint.Argc ++; SvcCalibTask_ModulePrintLog(SVC_LOG_CAL_NG , &(CalibModulePrint)); }
#define PRN_CAL_MTSK_API        ; CalibModulePrint.Argc ++; SvcCalibTask_ModulePrintLog(SVC_LOG_CAL_API, &(CalibModulePrint)); }
#define PRN_CAL_MTSK_ERR_HDLR   SvcCalibTask_ModuleErrHdlr(__func__, __LINE__, PRetVal);

static void SvcCalibTask_ModulePrintLog(UINT32 LogLevel, SVC_WRAP_PRINT_s *pPrint)
{
    if (pPrint != NULL) {
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

        SvcWrap_Print(SVC_LOG_MOD_TSK, pPrint);
    }
}

static void SvcCalibTask_Modulememset(void *ptr, INT32 v, SIZE_t n)
{
    UINT32 ErrCode = AmbaWrap_memset(ptr, v, n);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }
}

static void SvcCalibTask_Modulememcpy(void *pDst, const void *pSrc, SIZE_t num)
{
    UINT32 ErrCode = AmbaWrap_memcpy(pDst, pSrc, num);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }
}

static void SvcCalibTask_ModuleErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode)
{
    if (pCaller != NULL) {
        if (ErrCode != 0U) {
            PRN_CAL_MTSK_LOG "Catch ErrCode(0x%08x) @ %s, %d"
                PRN_CAL_MTSK_ARG_UINT32 ErrCode  PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_ARG_CSTR   pCaller  PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_ARG_UINT32 CodeLine PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_NG
        }
    }}

static UINT32 SvcCalibTask_ModuleCalChanGet(UINT32 FovID, SVC_CALIB_CHANNEL_s *pChan)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Fail to get FovID(%d) calib channel"
            PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_NG
    } else if (pChan == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Fail to get FovID(%d) calib channel - invalid calib channel"
            PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_NG
    } else {
        UINT32 VinID     = 0xFU;
        UINT32 SerDesIdx = 0xFU;

        PRetVal  = SvcResCfg_GetVinIDOfFovIdx(FovID, &VinID);
        PRetVal |= SvcResCfg_GetSerdesIdxOfFovIdx(FovID, &SerDesIdx);
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_CAL_MTSK_LOG "Fail to get FovID(%d) calib channel"
                PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_NG
        } else {
            if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
                RetVal = SVC_NG;
                PRN_CAL_MTSK_LOG "Fail to get FovID(%d) calib channel - invalid VinID(%d)"
                    PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
                    PRN_CAL_MTSK_ARG_UINT32 VinID PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_NG
            } else if (SerDesIdx >= 4U) {
                RetVal = SVC_NG;
                PRN_CAL_MTSK_LOG "Fail to get FovID(%d) calib channel - invalid SerDesIdx(%d)"
                    PRN_CAL_MTSK_ARG_UINT32 FovID     PRN_CAL_MTSK_ARG_POST
                    PRN_CAL_MTSK_ARG_UINT32 SerDesIdx PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_NG
            } else {
                SvcCalibTask_Modulememset(pChan, 0, sizeof(SVC_CALIB_CHANNEL_s));
                pChan->VinID = VinID;
                pChan->VinSelectBits = (0x1UL << pChan->VinID);
                pChan->SensorID = (0x1UL << SerDesIdx);
                pChan->SensorSelectBits = pChan->SensorID;
                pChan->ExtendData = FovID;
            }
        }
    }

    return RetVal;
}


static UINT32 SvcCalibTask_ModuleMemCfg(UINT32 FovID, const AMBA_EEPROM_CHANNEL_s *pChan, UINT32 NumOfCfg, SVC_MODULE_ITEM_CFG_s *pCfg, UINT8 **pMemBuf, UINT32 *pMemSize)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Fail to cfg FovID(%d) calib module mem - invalid fov id"
            PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_NG
    } else if (pChan == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Fail to cfg FovID(%d) calib module mem - invalid eeprom channel" PRN_CAL_MTSK_NG
    } else {
        UINT32 QueryMemSize = 0U;

        // Query storage module memory size
        PRetVal = SvcModule_MemQry(pChan, NumOfCfg, pCfg, &QueryMemSize);
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_CAL_MTSK_LOG "Fail to cfg FovID(%d) calib module mem - query module memory size fail!"
                PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_NG
        } else {
            UINT32 MinMemDiff = 0xFFFFFFFFU;
            UINT32 Idx, MemSlot = SVC_CALIB_TSK_MODULE_MAX_NUM;

            // search exist memory slot
            for (Idx = 0U; Idx < SVC_CALIB_TSK_MODULE_MAX_NUM; Idx ++) {
                if ((CalibModuleCtrl.MemSlot[Idx].Usaged == 0xCAFEU) &&
                    (CalibModuleCtrl.MemSlot[Idx].pBuf != NULL) &&
                    (CalibModuleCtrl.MemSlot[Idx].BufSize >= QueryMemSize)) {
                    if (MinMemDiff > (CalibModuleCtrl.MemSlot[Idx].BufSize - QueryMemSize)) {
                        MinMemDiff = (CalibModuleCtrl.MemSlot[Idx].BufSize - QueryMemSize);
                        MemSlot = Idx;
                    }
                }
            }

            // create new memory slot
            if (MemSlot == SVC_CALIB_TSK_MODULE_MAX_NUM) {
                if (CalibModuleCtrl.MemSlot[SVC_CALIB_TSK_MODULE_MAX_NUM].BufSize < QueryMemSize) {
                    RetVal = SVC_NG;
                    PRN_CAL_MTSK_LOG "Fail to cfg FovID(%d) calib module - there is not enough memory to service it!"
                        PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
                    PRN_CAL_MTSK_NG
                } else {
                    for (Idx = 0U; Idx < SVC_CALIB_TSK_MODULE_MAX_NUM; Idx ++) {
                        if (CalibModuleCtrl.MemSlot[Idx].Usaged == 0xCAFEU) {
                            MemSlot = Idx;
                            break;
                        }
                    }

                    if (MemSlot == SVC_CALIB_TSK_MODULE_MAX_NUM) {
                        RetVal = SVC_NG;
                        PRN_CAL_MTSK_LOG "Fail to cfg FovID(%d) calib module - there is not free memory slot to service it!"
                            PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
                        PRN_CAL_MTSK_NG
                    } else {
                        CalibModuleCtrl.MemSlot[SVC_CALIB_TSK_MODULE_MAX_NUM].BufSize -= QueryMemSize;
                        CalibModuleCtrl.MemSlot[MemSlot].pBuf = \
                                &(CalibModuleCtrl.MemSlot[SVC_CALIB_TSK_MODULE_MAX_NUM].pBuf[CalibModuleCtrl.MemSlot[SVC_CALIB_TSK_MODULE_MAX_NUM].BufSize]);
                        CalibModuleCtrl.MemSlot[MemSlot].BufSize = QueryMemSize;
                        CalibModuleCtrl.MemSlot[MemSlot].Usaged  = 0xCAFEU;
                    }
                }
            }

            // Configure memory slot
            if ((RetVal == SVC_OK) && (MemSlot != SVC_CALIB_TSK_MODULE_MAX_NUM)) {
                CalibModuleCtrl.MemSlot[MemSlot].Usaged = FovID;
                if (pMemBuf != NULL) {
                    *pMemBuf = CalibModuleCtrl.MemSlot[MemSlot].pBuf;
                }
                if (pMemSize != NULL) {
                    *pMemSize = CalibModuleCtrl.MemSlot[MemSlot].BufSize;
                }
            }

        }
    }

    return RetVal;
}


static UINT32 SvcCalibTask_ModuleTblIdxGet(UINT32 CalibID, SVC_CALIB_CHANNEL_s *pChan, UINT32 *pTblIdx)
{
    UINT32 RetVal = SVC_OK;

    if (pChan == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Fail to get calib table index - invalid calib channel!" PRN_CAL_MTSK_NG
    } else if (pChan->VinID >= AMBA_DSP_MAX_VIN_NUM) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Fail to get calib table index - invalid calib VinID!" PRN_CAL_MTSK_NG
    } else if (pTblIdx == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Fail to get calib table index - output table index should not null!" PRN_CAL_MTSK_NG
    } else {
        const SVC_MODULE_CFG_s *pModuleCfg = SvcModuleCfg_Get();
        if (pModuleCfg == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_MTSK_LOG "Fail to get calib table index - invalid storage module config!" PRN_CAL_MTSK_NG
        } else if (pModuleCfg->pItemUpd == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_MTSK_LOG "Fail to get calib table index - invalid storage module update config!" PRN_CAL_MTSK_NG
        } else {
            UINT32 ItmIdx;
            SVC_MODULE_ITEM_UPD_s *pCurItemUpd = NULL;

            for (ItmIdx = 0U; ItmIdx < pModuleCfg->ItemUpdNum; ItmIdx ++) {
                if ((pModuleCfg->pItemUpd[ItmIdx].CalibID == CalibID) &&
                    (pModuleCfg->pItemUpd[ItmIdx].Enable > 0U)) {
                    pCurItemUpd = &(pModuleCfg->pItemUpd[ItmIdx]);
                    break;
                }
            }

            if (pCurItemUpd == NULL) {
                PRN_CAL_MTSK_LOG "Current system does not support CalibID(%d) for storage module update mechanism"
                    PRN_CAL_MTSK_ARG_UINT32 CalibID PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
            } else {
                for (ItmIdx = 0U; ItmIdx < SVC_CALIB_TSK_MODULE_MAX_NUM; ItmIdx ++) {
                    if ((pCurItemUpd->UpdRule[ItmIdx].Enable > 0U) &&
                        ((pCurItemUpd->UpdRule[ItmIdx].VinSelectBits & (0x1UL << pChan->VinID)) > 0U) &&
                        ((pCurItemUpd->UpdRule[ItmIdx].SerDesSelectBits & pChan->SensorID) > 0U)) {
                        *pTblIdx = pCurItemUpd->UpdRule[ItmIdx].TableIdx;
                        break;
                    }
                }
            }

            AmbaMisra_TouchUnused(pCurItemUpd);
        }

        AmbaMisra_TouchUnused(pChan);
    }

    return RetVal;
}


static UINT32 SvcCalibTask_ModuleVigUpd(void *pModuleCtrl, SVC_CALIB_CHANNEL_s *pChan)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    UINT32 TableIdx = 0xCafeU;

    PRetVal = SvcCalibTask_ModuleTblIdxGet(SVC_CALIB_VIGNETTE_ID, pChan, &TableIdx);
    if (PRetVal != SVC_OK) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Fail to update vig data from module - get calib table index fail!" PRN_CAL_MTSK_NG
    }

    if ((pModuleCtrl != NULL) && (pChan != NULL) && (TableIdx < 0xCafeU)) {
        AMBA_CAL_1D_VIG_CALIB_DATA_V1_s *pVigData = NULL;
        UINT32 ReqMemSize = (UINT32)sizeof(AMBA_CAL_1D_VIG_CALIB_DATA_V1_s);

        if (CalibModuleCtrl.MemUpd.BufSize < ReqMemSize) {
            RetVal = SVC_NG;
            PRN_CAL_MTSK_LOG "Fail to update vig data from module - update memory to small to service it! 0x%x < 0x%x"
                PRN_CAL_MTSK_ARG_UINT32 CalibModuleCtrl.MemUpd.BufSize PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_ARG_UINT32 ReqMemSize                     PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_NG
        }

        AmbaMisra_TypeCast(&(pVigData), &(CalibModuleCtrl.MemUpd.pBuf));

        if ((RetVal == SVC_OK) && (pVigData != NULL)) {
            SVC_MODULE_ITEM_DATA_INFO_s ModuleItemData;

            SvcCalibTask_Modulememset(pVigData, 0, sizeof(AMBA_CAL_1D_VIG_CALIB_DATA_V1_s));
            SvcCalibTask_Modulememset(&ModuleItemData, 0, sizeof(ModuleItemData));
            ModuleItemData.ItemVersion  = SVC_MODULE_ITEM_VIG;
            ModuleItemData.MinorVersion = SVC_MODULE_MINOR_VERSION;
            ModuleItemData.ProjectID    = 0U;
            ModuleItemData.pData        = pVigData;
            ModuleItemData.DataSize     = (UINT32) sizeof(AMBA_CAL_1D_VIG_CALIB_DATA_V1_s);

            RetVal = SvcModule_ItemGet(pModuleCtrl, &ModuleItemData);
            if (RetVal == SVC_OK) {
                SVC_CALIB_TBL_INFO_s TblInfo;
                SvcCalibTask_Modulememset(&TblInfo, 0, sizeof(TblInfo));
                SvcCalibTask_Modulememcpy(&(TblInfo.CalChan), pChan, sizeof(SVC_CALIB_CHANNEL_s));
                TblInfo.DbgMsgOn = 1U;
                TblInfo.pBuf     = ModuleItemData.pData;
                TblInfo.BufSize  = ModuleItemData.DataSize;
                RetVal = SvcCalib_ItemTableSet(SVC_CALIB_VIGNETTE_ID, TableIdx, &TblInfo);
            }

            if (RetVal == SVC_OK) {
                PRN_CAL_MTSK_LOG "Success to vig data from VinID(%d) SerDesBits(0x%x) storage module to calib table index(%d)."
                    PRN_CAL_MTSK_ARG_UINT32 pChan->VinID    PRN_CAL_MTSK_ARG_POST
                    PRN_CAL_MTSK_ARG_UINT32 pChan->SensorID PRN_CAL_MTSK_ARG_POST
                    PRN_CAL_MTSK_ARG_UINT32 TableIdx        PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
            }
        }

        AmbaMisra_TouchUnused(pChan);
    }

    return RetVal;
}


static UINT32 SvcCalibTask_ModuleVigSet(void *pModuleCtrl, SVC_CALIB_CHANNEL_s *pChan, UINT32 TableIdx)
{
    UINT32 RetVal = SVC_OK;

    if ((pModuleCtrl != NULL) && (pChan != NULL)) {
        const SVC_CALIB_CFG_s* pCalCfg = SvcCalibCfg_Get();
        AMBA_CAL_1D_VIG_CALIB_DATA_V1_s *pVigData;
        UINT32 ReqMemSize = (UINT32)sizeof(AMBA_CAL_1D_VIG_CALIB_DATA_V1_s);

        if (CalibModuleCtrl.MemUpd.BufSize < ReqMemSize) {
            RetVal = SVC_NG;
            PRN_CAL_MTSK_LOG "Fail to set vig data to module - update memory to small to service it! 0x%x < 0x%x"
                PRN_CAL_MTSK_ARG_UINT32 CalibModuleCtrl.MemUpd.BufSize PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_ARG_UINT32 ReqMemSize                     PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_NG
        }

        AmbaMisra_TypeCast(&(pVigData), &(CalibModuleCtrl.MemUpd.pBuf));

        if ((RetVal == SVC_OK) && (pCalCfg != NULL) && (pVigData != NULL)) {
            UINT32 CalIdx, TableNum = 0U;
            SVC_CALIB_TBL_INFO_s TblInfo;
            SVC_MODULE_ITEM_DATA_INFO_s ModuleItemData;

            for (CalIdx = 0U; CalIdx < pCalCfg->NumOfCalObj; CalIdx ++) {
                if ((pCalCfg->pCalObj[CalIdx].ID == SVC_CALIB_VIGNETTE_ID) &&
                    (pCalCfg->pCalObj[CalIdx].Enable > 0U)) {
                    TableNum = pCalCfg->pCalObj[CalIdx].NumOfTable;
                    break;
                }
            }

            if (TableNum > 0U) {
                if (TableIdx < TableNum) {
                    // Get vig data from calib vig module
                    SvcCalibTask_Modulememset(pVigData, 0, sizeof(AMBA_CAL_1D_VIG_CALIB_DATA_V1_s));
                    SvcCalibTask_Modulememset(&TblInfo, 0, sizeof(TblInfo));
                    TblInfo.pBuf = pVigData;
                    TblInfo.BufSize = (UINT32) sizeof(AMBA_CAL_1D_VIG_CALIB_DATA_V1_s);
                    RetVal = SvcCalib_ItemTableGet(SVC_CALIB_VIGNETTE_ID, TableIdx, &TblInfo);
                    if (RetVal != SVC_OK) {
                        PRN_CAL_MTSK_LOG "Fail to set vig date to storage module - get vig data from calib fail" PRN_CAL_MTSK_NG
                    }

                    // Set vig data to storage module
                    if (RetVal == SVC_OK) {

                        SvcCalibTask_Modulememset(&ModuleItemData, 0, sizeof(ModuleItemData));
                        ModuleItemData.ItemVersion  = SVC_MODULE_ITEM_VIG;
                        ModuleItemData.MinorVersion = SVC_MODULE_MINOR_VERSION;
                        ModuleItemData.ProjectID    = 0U;
                        ModuleItemData.pData        = pVigData;
                        ModuleItemData.DataSize     = (UINT32) sizeof(AMBA_CAL_1D_VIG_CALIB_DATA_V1_s);

                        RetVal = SvcModule_ItemSet(pModuleCtrl, &ModuleItemData);
                        if (RetVal != SVC_OK) {
                            PRN_CAL_MTSK_LOG "Fail to set vig date to storage module - set vig data to module fail" PRN_CAL_MTSK_NG
                        }
                    }

                    if (RetVal == SVC_OK) {
                        PRN_CAL_MTSK_LOG "Success to set vig date to storage module" PRN_CAL_MTSK_OK
                    }
                }
            }
        }

        AmbaMisra_TouchUnused(pChan);
    }

    return RetVal;
}


static UINT32 SvcCalibTask_ModuleBpcSet(void *pModuleCtrl, SVC_CALIB_CHANNEL_s *pChan)
{
    UINT32 RetVal = SVC_OK;

    if (pModuleCtrl != NULL) {
        const SVC_CALIB_CFG_s* pCalCfg = SvcCalibCfg_Get();
        UINT32 ReqMemSize = SVC_MODULE_ITEM_BPC_SIZE;

        if (CalibModuleCtrl.MemUpd.BufSize < ReqMemSize) {
            RetVal = SVC_NG;
            PRN_CAL_MTSK_LOG "Fail to set bpc data to module - update memory to small to service it! 0x%x < 0x%x"
                PRN_CAL_MTSK_ARG_UINT32 CalibModuleCtrl.MemUpd.BufSize PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_ARG_UINT32 ReqMemSize                     PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_NG
        }

        if ((RetVal == SVC_OK) && (pChan != NULL) && (pCalCfg != NULL)) {
            UINT32 CalIdx, TableNum = 0U;

            for (CalIdx = 0U; CalIdx < pCalCfg->NumOfCalObj; CalIdx ++) {
                if ((pCalCfg->pCalObj[CalIdx].ID == SVC_CALIB_BPC_ID) &&
                    (pCalCfg->pCalObj[CalIdx].Enable > 0U)) {
                    TableNum = pCalCfg->pCalObj[CalIdx].NumOfTable;
                    break;
                }
            }

            if (TableNum > 0U) {
                SVC_CALIB_BPC_TBL_DATA_s CalibSbpTbl;
                SVC_CALIB_TBL_INFO_s     CalibTblInfo;

                // Get vig data from calib vig module

                // Configure calib bpc table
                SvcCalibTask_Modulememset(&CalibSbpTbl, 0, sizeof(SVC_CALIB_BPC_TBL_DATA_s));
                CalibSbpTbl.pSbpTbl    = CalibModuleCtrl.MemUpd.pBuf;
                CalibSbpTbl.SbpTblSize = SVC_MODULE_ITEM_BPC_SIZE;

                // Configure calib table info
                SvcCalibTask_Modulememset(&CalibTblInfo, 0, sizeof(SVC_CALIB_TBL_INFO_s));
                CalibTblInfo.CalChan.VinID            = pChan->VinID;
                CalibTblInfo.CalChan.VinSelectBits    = pChan->VinSelectBits;
                CalibTblInfo.CalChan.SensorID         = pChan->SensorID;
                CalibTblInfo.CalChan.SensorSelectBits = pChan->SensorSelectBits;
                CalibTblInfo.pBuf    = &CalibSbpTbl;
                CalibTblInfo.BufSize = (UINT32) sizeof(SVC_CALIB_BPC_TBL_DATA_s);

                PRN_CAL_MTSK_LOG "---- Calib Table Info (%p)----"
                    PRN_CAL_MTSK_ARG_CPOINT &CalibTblInfo PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "             CalChan.VinID : 0x%x"
                    PRN_CAL_MTSK_ARG_UINT32 CalibTblInfo.CalChan.VinID PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "     CalChan.VinSelectBits : 0x%x"
                    PRN_CAL_MTSK_ARG_UINT32 CalibTblInfo.CalChan.VinSelectBits PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "          CalChan.SensorID : 0x%x"
                    PRN_CAL_MTSK_ARG_UINT32 CalibTblInfo.CalChan.SensorID PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "  CalChan.SensorSelectBits : 0x%x"
                    PRN_CAL_MTSK_ARG_UINT32 CalibTblInfo.CalChan.SensorSelectBits PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "                      pBuf : %p"
                    PRN_CAL_MTSK_ARG_CPOINT CalibTblInfo.pBuf PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "                   BufSize : 0x%08x"
                    PRN_CAL_MTSK_ARG_UINT32 CalibTblInfo.BufSize PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API// Get calib bpc table
                RetVal = SvcCalib_ItemTableGet(SVC_CALIB_BPC_ID, 255U, &CalibTblInfo);
                if (RetVal != SVC_OK) {
                    PRN_CAL_MTSK_LOG "Fail to set bpc date to storage module - get bpc data from calib fail" PRN_CAL_MTSK_NG
                } else {
                    PRN_CAL_MTSK_LOG "---- Calib Bpc Table Info (%p)----"
                        PRN_CAL_MTSK_ARG_CPOINT &CalibTblInfo PRN_CAL_MTSK_ARG_POST
                    PRN_CAL_MTSK_API
                    PRN_CAL_MTSK_LOG "          Version : 0x%x"
                        PRN_CAL_MTSK_ARG_UINT32 CalibSbpTbl.Version PRN_CAL_MTSK_ARG_POST
                    PRN_CAL_MTSK_API
                    PRN_CAL_MTSK_LOG "  CalibGeo.StartX : 0x%x"
                        PRN_CAL_MTSK_ARG_UINT32 CalibSbpTbl.CalibGeo.StartX PRN_CAL_MTSK_ARG_POST
                    PRN_CAL_MTSK_API
                    PRN_CAL_MTSK_LOG "  CalibGeo.StartY : 0x%x"
                        PRN_CAL_MTSK_ARG_UINT32 CalibSbpTbl.CalibGeo.StartY PRN_CAL_MTSK_ARG_POST
                    PRN_CAL_MTSK_API
                    PRN_CAL_MTSK_LOG "   CalibGeo.Width : 0x%x"
                        PRN_CAL_MTSK_ARG_UINT32 CalibSbpTbl.CalibGeo.Width PRN_CAL_MTSK_ARG_POST
                    PRN_CAL_MTSK_API
                    PRN_CAL_MTSK_LOG "  CalibGeo.Height : 0x%x"
                        PRN_CAL_MTSK_ARG_UINT32 CalibSbpTbl.CalibGeo.Height PRN_CAL_MTSK_ARG_POST
                    PRN_CAL_MTSK_API
                    PRN_CAL_MTSK_LOG "       SbpTblBase : %p"
                        PRN_CAL_MTSK_ARG_CPOINT CalibSbpTbl.pSbpTbl PRN_CAL_MTSK_ARG_POST
                    PRN_CAL_MTSK_API
                    PRN_CAL_MTSK_LOG "       SbpTblSize : 0x%x"
                        PRN_CAL_MTSK_ARG_UINT32 CalibSbpTbl.SbpTblSize PRN_CAL_MTSK_ARG_POST
                    PRN_CAL_MTSK_API
                    PRN_CAL_MTSK_LOG "         SbpCount : %d"
                        PRN_CAL_MTSK_ARG_UINT32 CalibSbpTbl.SbpCount PRN_CAL_MTSK_ARG_POST
                    PRN_CAL_MTSK_API
                    PRN_CAL_MTSK_LOG "       SensorMode : 0x%x"
                        PRN_CAL_MTSK_ARG_UINT32 CalibSbpTbl.SensorMode PRN_CAL_MTSK_ARG_POST
                    PRN_CAL_MTSK_API
                }

                // Set vig data to storage module
                if (RetVal == SVC_OK) {
                    SVC_MODULE_ITEM_DATA_INFO_s ModuleItemData;
                    SVC_MODULE_BPC_TBL_DATA_s ModuleBpcTbl;

                    SvcCalibTask_Modulememset(&ModuleBpcTbl, 0, sizeof(ModuleBpcTbl));
                    ModuleBpcTbl.CalibGeo.StartX               = CalibSbpTbl.CalibGeo.StartX               ;
                    ModuleBpcTbl.CalibGeo.StartY               = CalibSbpTbl.CalibGeo.StartY               ;
                    ModuleBpcTbl.CalibGeo.Width                = CalibSbpTbl.CalibGeo.Width                ;
                    ModuleBpcTbl.CalibGeo.Height               = CalibSbpTbl.CalibGeo.Height               ;
                    ModuleBpcTbl.CalibGeo.HSubSample.FactorDen = CalibSbpTbl.CalibGeo.HSubSample.FactorDen ;
                    ModuleBpcTbl.CalibGeo.HSubSample.FactorNum = CalibSbpTbl.CalibGeo.HSubSample.FactorNum ;
                    ModuleBpcTbl.CalibGeo.VSubSample.FactorDen = CalibSbpTbl.CalibGeo.VSubSample.FactorDen ;
                    ModuleBpcTbl.CalibGeo.VSubSample.FactorNum = CalibSbpTbl.CalibGeo.VSubSample.FactorNum ;
                    ModuleBpcTbl.pSbpTbl                       = CalibSbpTbl.pSbpTbl;
                    ModuleBpcTbl.SbpTblSize                    = CalibSbpTbl.SbpTblSize;
                    ModuleBpcTbl.EnableOB                      = 0U;

                    SvcCalibTask_Modulememset(&ModuleItemData, 0, sizeof(ModuleItemData));
                    ModuleItemData.ItemVersion  = SVC_MODULE_ITEM_BPC;
                    ModuleItemData.MinorVersion = SVC_MODULE_MINOR_VERSION;
                    ModuleItemData.ProjectID    = 0U;
                    ModuleItemData.pData        = &ModuleBpcTbl;
                    ModuleItemData.DataSize     = (UINT32) sizeof(SVC_MODULE_BPC_TBL_DATA_s);

                    RetVal = SvcModule_ItemSet(pModuleCtrl, &ModuleItemData);
                    if (RetVal != SVC_OK) {
                        PRN_CAL_MTSK_LOG "Fail to set bpc date to storage module - set bpc data to module fail" PRN_CAL_MTSK_NG
                    }
                }

                if (RetVal == SVC_OK) {
                    PRN_CAL_MTSK_LOG "Success to set bpc date to storage module" PRN_CAL_MTSK_OK
                }
            }
        }

        AmbaMisra_TouchUnused(pChan);
    }

    return RetVal;
}


static UINT32 SvcCalibTask_ModuleBpcUpd(void *pModuleCtrl, SVC_CALIB_CHANNEL_s *pChan)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    UINT32 TableIdx = 0xCafeU;

    PRetVal = SvcCalibTask_ModuleTblIdxGet(SVC_CALIB_BPC_ID, pChan, &TableIdx);
    if (PRetVal != SVC_OK) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Fail to update bpc data from storage module - get calib table index fail!" PRN_CAL_MTSK_NG
    }

    if ((pModuleCtrl != NULL) && (pChan != NULL) && (TableIdx < 0xCafeU)) {
        UINT32 ReqMemSize = SVC_MODULE_ITEM_BPC_SIZE;

        if (CalibModuleCtrl.MemUpd.BufSize < ReqMemSize) {
            RetVal = SVC_NG;
            PRN_CAL_MTSK_LOG "Fail to update bpc data from storage module - update memory to small to service it! 0x%x < 0x%x"
                PRN_CAL_MTSK_ARG_UINT32 CalibModuleCtrl.MemUpd.BufSize PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_ARG_UINT32 ReqMemSize                     PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_NG
        }

        if (RetVal == SVC_OK) {
            SVC_MODULE_BPC_TBL_DATA_s   ModuleBpcTbl;
            SVC_MODULE_ITEM_DATA_INFO_s ModuleItemData;

            SvcCalibTask_Modulememset(&ModuleBpcTbl, 0, sizeof(ModuleBpcTbl));
            ModuleBpcTbl.pSbpTbl    = CalibModuleCtrl.MemUpd.pBuf;
            ModuleBpcTbl.SbpTblSize = SVC_MODULE_ITEM_BPC_SIZE;

            SvcCalibTask_Modulememset(&ModuleItemData, 0, sizeof(ModuleItemData));
            ModuleItemData.ItemVersion = SVC_MODULE_ITEM_BPC;
            ModuleItemData.pData       = &ModuleBpcTbl;
            ModuleItemData.DataSize    = (UINT32) sizeof(ModuleBpcTbl);

            RetVal = SvcModule_ItemGet(pModuleCtrl, &ModuleItemData);

            if (RetVal == SVC_OK) {
                SVC_CALIB_BPC_TBL_DATA_s CalibSbpTbl;
                SVC_CALIB_TBL_INFO_s     CalibTblInfo;

                SvcCalibTask_Modulememset(&CalibSbpTbl, 0, sizeof(CalibSbpTbl));
                CalibSbpTbl.CalibGeo.StartX               = ModuleBpcTbl.CalibGeo.StartX              ;
                CalibSbpTbl.CalibGeo.StartY               = ModuleBpcTbl.CalibGeo.StartY              ;
                CalibSbpTbl.CalibGeo.Width                = ModuleBpcTbl.CalibGeo.Width               ;
                CalibSbpTbl.CalibGeo.Height               = ModuleBpcTbl.CalibGeo.Height              ;
                CalibSbpTbl.CalibGeo.HSubSample.FactorDen = ModuleBpcTbl.CalibGeo.HSubSample.FactorDen;
                CalibSbpTbl.CalibGeo.HSubSample.FactorNum = ModuleBpcTbl.CalibGeo.HSubSample.FactorNum;
                CalibSbpTbl.CalibGeo.VSubSample.FactorDen = ModuleBpcTbl.CalibGeo.VSubSample.FactorDen;
                CalibSbpTbl.CalibGeo.VSubSample.FactorNum = ModuleBpcTbl.CalibGeo.VSubSample.FactorNum;
                CalibSbpTbl.pSbpTbl                       = ModuleBpcTbl.pSbpTbl                      ;
                CalibSbpTbl.SbpTblSize                    = ModuleBpcTbl.SbpTblSize                   ;

                SvcCalibTask_Modulememset(&CalibTblInfo, 0, sizeof(CalibTblInfo));
                CalibTblInfo.CalChan.VinID            = pChan->VinID           ;
                CalibTblInfo.CalChan.VinSelectBits    = pChan->VinSelectBits   ;
                CalibTblInfo.CalChan.SensorID         = pChan->SensorID        ;
                CalibTblInfo.CalChan.SensorSelectBits = pChan->SensorSelectBits;
                CalibTblInfo.CalChan.ExtendData       = pChan->ExtendData      ;
                CalibTblInfo.DbgMsgOn                 = 1U;
                CalibTblInfo.pBuf                     = &CalibSbpTbl;
                CalibTblInfo.BufSize                  = (UINT32) sizeof(CalibSbpTbl);

                RetVal = SvcCalib_ItemTableSet(SVC_CALIB_BPC_ID, TableIdx, &CalibTblInfo);
            } else {
                PRN_CAL_MTSK_LOG "There is not exist bpc table in eeprom" PRN_CAL_MTSK_API
            }

            if (RetVal == SVC_OK) {
                PRN_CAL_MTSK_LOG "Success to bpc data from VinID(%d) SerDesBits(0x%x) storage module to calib table index(%d)."
                    PRN_CAL_MTSK_ARG_UINT32 pChan->VinID    PRN_CAL_MTSK_ARG_POST
                    PRN_CAL_MTSK_ARG_UINT32 pChan->SensorID PRN_CAL_MTSK_ARG_POST
                    PRN_CAL_MTSK_ARG_UINT32 TableIdx        PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
            }
        }

        AmbaMisra_TouchUnused(pChan);
    }

    return RetVal;

}


static UINT32 SvcCalibTask_ModuleBpcInfo(void *pModuleCtrl)
{
    UINT32 RetVal = SVC_OK;

    if (pModuleCtrl != NULL) {
        UINT32 ReqMemSize = SVC_MODULE_ITEM_BPC_SIZE;

        if (CalibModuleCtrl.MemUpd.BufSize < ReqMemSize) {
            RetVal = SVC_NG;
            PRN_CAL_MTSK_LOG "Fail to update bpc data from storage module - update memory to small to service it! 0x%x < 0x%x"
                PRN_CAL_MTSK_ARG_UINT32 CalibModuleCtrl.MemUpd.BufSize PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_ARG_UINT32 ReqMemSize                     PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_NG
        }

        if (RetVal == SVC_OK) {
            SVC_MODULE_BPC_TBL_DATA_s   ModuleBpcTbl;
            SVC_MODULE_ITEM_DATA_INFO_s ModuleItemData;

            SvcCalibTask_Modulememset(&ModuleBpcTbl, 0, sizeof(ModuleBpcTbl));
            ModuleBpcTbl.pSbpTbl    = CalibModuleCtrl.MemUpd.pBuf;
            ModuleBpcTbl.SbpTblSize = SVC_MODULE_ITEM_BPC_SIZE;

            SvcCalibTask_Modulememset(&ModuleItemData, 0, sizeof(ModuleItemData));
            ModuleItemData.ItemVersion = SVC_MODULE_ITEM_BPC;
            ModuleItemData.pData       = &ModuleBpcTbl;
            ModuleItemData.DataSize    = (UINT32) sizeof(ModuleBpcTbl);

            RetVal = SvcModule_ItemGet(pModuleCtrl, &ModuleItemData);
            if (RetVal == SVC_OK) {
                PRN_CAL_MTSK_LOG " " PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "---- Storage Module BPC Table ----" PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "               StartX : %u"
                    PRN_CAL_MTSK_ARG_UINT32 ModuleBpcTbl.CalibGeo.StartX PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "               StartY : %u"
                    PRN_CAL_MTSK_ARG_UINT32 ModuleBpcTbl.CalibGeo.StartY PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "               Width  : %u"
                    PRN_CAL_MTSK_ARG_UINT32 ModuleBpcTbl.CalibGeo.Width PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "               Height : %u"
                    PRN_CAL_MTSK_ARG_UINT32 ModuleBpcTbl.CalibGeo.Height PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "  HSubSampleFactorDen : %u"
                    PRN_CAL_MTSK_ARG_UINT32 ModuleBpcTbl.CalibGeo.HSubSample.FactorDen PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "  HSubSampleFactorNum : %u"
                    PRN_CAL_MTSK_ARG_UINT32 ModuleBpcTbl.CalibGeo.HSubSample.FactorNum PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "  VSubSampleFactorDen : %u"
                    PRN_CAL_MTSK_ARG_UINT32 ModuleBpcTbl.CalibGeo.VSubSample.FactorDen PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "  VSubSampleFactorNum : %u"
                    PRN_CAL_MTSK_ARG_UINT32 ModuleBpcTbl.CalibGeo.VSubSample.FactorNum PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "           SbpTblBase : %p"
                    PRN_CAL_MTSK_ARG_CPOINT ModuleBpcTbl.pSbpTbl PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "           SbpTblSize : 0x%x"
                    PRN_CAL_MTSK_ARG_UINT32 ModuleBpcTbl.SbpTblSize PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "             EnableOB : %u"
                    PRN_CAL_MTSK_ARG_UINT32 ModuleBpcTbl.EnableOB PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
            }
        }
    }

    return RetVal;

}


static UINT32 SvcCalibTask_ModuleCamInfoSet(void *pModuleCtrl, FULLVIN_INTRINSIC_INFORMATION_s *pCameraInfo)
{
    UINT32 RetVal = SVC_OK;

    if ((pModuleCtrl == NULL) || (pCameraInfo == NULL)) {
        RetVal = SVC_NG;
    } else {
        SVC_MODULE_ITEM_DATA_INFO_s ModuleItemData;

        SvcCalibTask_Modulememset(&ModuleItemData, 0, sizeof(ModuleItemData));
        ModuleItemData.ItemVersion  = SVC_MODULE_ITEM_CAM_INTRINSIC;
        ModuleItemData.pData        = pCameraInfo;
        ModuleItemData.DataSize     = (UINT32) sizeof(FULLVIN_INTRINSIC_INFORMATION_s);

        RetVal = SvcModule_ItemSet(pModuleCtrl, &ModuleItemData);
    }

    return RetVal;
}


static UINT32 SvcCalibTask_ModuleCamInfoGet(void *pModuleCtrl, FULLVIN_INTRINSIC_INFORMATION_s *pCameraInfo)
{
    UINT32 RetVal = SVC_OK;

    if ((pModuleCtrl == NULL) || (pCameraInfo == NULL)) {
        RetVal = SVC_NG;
    } else {
        SVC_MODULE_ITEM_DATA_INFO_s ModuleItemData;

        SvcCalibTask_Modulememset(&ModuleItemData, 0, sizeof(ModuleItemData));
        ModuleItemData.ItemVersion  = SVC_MODULE_ITEM_CAM_INTRINSIC;
        ModuleItemData.pData        = pCameraInfo;
        ModuleItemData.DataSize     = (UINT32) sizeof(FULLVIN_INTRINSIC_INFORMATION_s);

        RetVal = SvcModule_ItemGet(pModuleCtrl, &ModuleItemData);
    }

    return RetVal;
}


static UINT32 SvcCalibTask_ModuleWbSet(void *pModuleCtrl, SVC_CALIB_CHANNEL_s *pChan, UINT32 TableIdx)
{
    UINT32 RetVal = SVC_OK;

    if ((pModuleCtrl != NULL) && (pChan != NULL)) {
        const SVC_CALIB_CFG_s* pCalCfg = SvcCalibCfg_Get();
        if (pCalCfg != NULL) {
            UINT32 CalIdx, TableNum = 0U;

            for (CalIdx = 0U; CalIdx < pCalCfg->NumOfCalObj; CalIdx ++) {
                if ((pCalCfg->pCalObj[CalIdx].ID == SVC_CALIB_WB_ID) &&
                    (pCalCfg->pCalObj[CalIdx].Enable > 0U)) {
                    TableNum = pCalCfg->pCalObj[CalIdx].NumOfTable;
                    break;
                }
            }

            if (TableNum > 0U) {
                if (TableIdx < TableNum) {
                    SVC_CALIB_TBL_INFO_s    TblInfo;
                    SVC_CALIB_WB_TBL_DATA_s CalibWbTbl;

                    // Get vig data from calib vig module
                    SvcCalibTask_Modulememset(&CalibWbTbl, 0, sizeof(SVC_CALIB_WB_TBL_DATA_s));
                    SvcCalibTask_Modulememset(&TblInfo, 0, sizeof(TblInfo));
                    TblInfo.pBuf = &CalibWbTbl;
                    TblInfo.BufSize = (UINT32) sizeof(SVC_CALIB_WB_TBL_DATA_s);
                    RetVal = SvcCalib_ItemTableGet(SVC_CALIB_WB_ID, TableIdx, &TblInfo);
                    if (RetVal != SVC_OK) {
                        PRN_CAL_MTSK_LOG "Fail to set wb date to storage module - get wb data from calib fail" PRN_CAL_MTSK_NG
                    }

                    // Set vig data to storage module
                    if (RetVal == SVC_OK) {
                        WHITE_BLANCE_s WbData;
                        SVC_MODULE_ITEM_DATA_INFO_s ModuleItemData;

                        SvcCalibTask_Modulememset(&WbData, 0, sizeof(WbData));
                        WbData.OrgWB[0].GainR = CalibWbTbl.OriGain[0].GainR;
                        WbData.OrgWB[0].GainG = CalibWbTbl.OriGain[0].GainG;
                        WbData.OrgWB[0].GainB = CalibWbTbl.OriGain[0].GainB;
                        WbData.OrgWB[1].GainR = CalibWbTbl.OriGain[1].GainR;
                        WbData.OrgWB[1].GainG = CalibWbTbl.OriGain[1].GainG;
                        WbData.OrgWB[1].GainB = CalibWbTbl.OriGain[1].GainB;
                        WbData.RefWB[0].GainR = CalibWbTbl.RefGain[0].GainR;
                        WbData.RefWB[0].GainG = CalibWbTbl.RefGain[0].GainG;
                        WbData.RefWB[0].GainB = CalibWbTbl.RefGain[0].GainB;
                        WbData.RefWB[1].GainR = CalibWbTbl.RefGain[1].GainR;
                        WbData.RefWB[1].GainG = CalibWbTbl.RefGain[1].GainG;
                        WbData.RefWB[1].GainB = CalibWbTbl.RefGain[1].GainB;

                        SvcCalibTask_Modulememset(&ModuleItemData, 0, sizeof(ModuleItemData));
                        ModuleItemData.ItemVersion  = SVC_MODULE_ITEM_WHITE_BALANCE;
                        ModuleItemData.MinorVersion = SVC_MODULE_MINOR_VERSION;
                        ModuleItemData.ProjectID    = 0U;
                        ModuleItemData.pData        = &WbData;
                        ModuleItemData.DataSize     = (UINT32) sizeof(WHITE_BLANCE_s);

                        RetVal = SvcModule_ItemSet(pModuleCtrl, &ModuleItemData);
                        if (RetVal != SVC_OK) {
                            PRN_CAL_MTSK_LOG "Fail to set wb date to storage module - set wb data to module fail" PRN_CAL_MTSK_NG
                        }
                    }

                    if (RetVal == SVC_OK) {
                        PRN_CAL_MTSK_LOG "Success to set wb date to storage module" PRN_CAL_MTSK_OK
                    }
                }
            }
        }

        AmbaMisra_TouchUnused(pChan);
    }

    return RetVal;
}


static UINT32 SvcCalibTask_ModuleWbUpd(void *pModuleCtrl, SVC_CALIB_CHANNEL_s *pChan)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    UINT32 TableIdx = 0xCafeU;

    PRetVal = SvcCalibTask_ModuleTblIdxGet(SVC_CALIB_WB_ID, pChan, &TableIdx);
    if (PRetVal != SVC_OK) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Fail to update wb data from module - get calib table index fail!" PRN_CAL_MTSK_NG
    }

    if ((pModuleCtrl != NULL) && (pChan != NULL) && (TableIdx < 0xCafeU)) {
        WHITE_BLANCE_s WbData;
        SVC_MODULE_ITEM_DATA_INFO_s ModuleItemData;

        SvcCalibTask_Modulememset(&WbData,         0, sizeof(WHITE_BLANCE_s));
        SvcCalibTask_Modulememset(&ModuleItemData, 0, sizeof(ModuleItemData));
        ModuleItemData.ItemVersion  = SVC_MODULE_ITEM_WHITE_BALANCE;
        ModuleItemData.MinorVersion = SVC_MODULE_MINOR_VERSION;
        ModuleItemData.ProjectID    = 0U;
        ModuleItemData.pData        = &WbData;
        ModuleItemData.DataSize     = (UINT32) sizeof(WHITE_BLANCE_s);
        RetVal = SvcModule_ItemGet(pModuleCtrl, &ModuleItemData);
        if (RetVal == SVC_OK) {
            SVC_CALIB_TBL_INFO_s    TblInfo;
            SVC_CALIB_WB_TBL_DATA_s CalibWbTbl;

            SvcCalibTask_Modulememset(&CalibWbTbl, 0, sizeof(CalibWbTbl));
            CalibWbTbl.OriGain[0].GainR = WbData.OrgWB[0].GainR;
            CalibWbTbl.OriGain[0].GainG = WbData.OrgWB[0].GainG;
            CalibWbTbl.OriGain[0].GainB = WbData.OrgWB[0].GainB;
            CalibWbTbl.OriGain[1].GainR = WbData.OrgWB[1].GainR;
            CalibWbTbl.OriGain[1].GainG = WbData.OrgWB[1].GainG;
            CalibWbTbl.OriGain[1].GainB = WbData.OrgWB[1].GainB;
            CalibWbTbl.RefGain[0].GainR = WbData.RefWB[0].GainR;
            CalibWbTbl.RefGain[0].GainG = WbData.RefWB[0].GainG;
            CalibWbTbl.RefGain[0].GainB = WbData.RefWB[0].GainB;
            CalibWbTbl.RefGain[1].GainR = WbData.RefWB[1].GainR;
            CalibWbTbl.RefGain[1].GainG = WbData.RefWB[1].GainG;
            CalibWbTbl.RefGain[1].GainB = WbData.RefWB[1].GainB;

            SvcCalibTask_Modulememset(&TblInfo, 0, sizeof(TblInfo));
            SvcCalibTask_Modulememcpy(&(TblInfo.CalChan), pChan, sizeof(SVC_CALIB_CHANNEL_s));
            TblInfo.DbgMsgOn = 1U;
            TblInfo.pBuf = &CalibWbTbl;
            TblInfo.BufSize = (UINT32)(sizeof(CalibWbTbl));
            RetVal = SvcCalib_ItemTableSet(SVC_CALIB_WB_ID, TableIdx, &TblInfo);
        }

        if (RetVal == SVC_OK) {
            PRN_CAL_MTSK_LOG "Success to wb data from VinID(%d) SerDesBits(0x%x) storage module to calib table index(%d)."
                PRN_CAL_MTSK_ARG_UINT32 pChan->VinID    PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_ARG_UINT32 pChan->SensorID PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_ARG_UINT32 TableIdx        PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_API
        }

        AmbaMisra_TouchUnused(pChan);
    }

    return RetVal;
}


static UINT32 SvcCalibTask_ModuleWbGet(void *pModuleCtrl, WHITE_BLANCE_s *pWbData)
{
    UINT32 RetVal = SVC_OK;

    if ((pModuleCtrl == NULL) || (pWbData == NULL)) {
        RetVal = SVC_NG;
    } else {
        SVC_MODULE_ITEM_DATA_INFO_s ModuleItemData;

        SvcCalibTask_Modulememset(&ModuleItemData, 0, sizeof(ModuleItemData));
        ModuleItemData.ItemVersion  = SVC_MODULE_ITEM_WHITE_BALANCE;
        ModuleItemData.pData        = pWbData;
        ModuleItemData.DataSize     = (UINT32) sizeof(WHITE_BLANCE_s);

        RetVal = SvcModule_ItemGet(pModuleCtrl, &ModuleItemData);
    }

    return RetVal;
}

static void SvcCalibTask_ModuleSerDesQryPause(UINT32 VinID)
{
    if (VinID < AMBA_DSP_MAX_VIN_NUM) {
        AmbaMisra_TouchUnused(&VinID);
    }
}


static void SvcCalibTask_ModuleSerDesQryResume(UINT32 VinID)
{
    if (VinID < AMBA_DSP_MAX_VIN_NUM) {
        AmbaMisra_TouchUnused(&VinID);
    }
}

static void SvcCalibTask_ModShellStrToU32(const char *pString, UINT32 *pValue)
{
    UINT32 ErrCode = AmbaUtility_StringToUInt32(pString, pValue);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }}

static UINT32 SvcCalibTask_ModShellInit(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount < 5U) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Failure to proc 'init' cmd - invalid arg count!" PRN_CAL_MTSK_NG
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 UserMemBase = 0U;
        UINT32 UserMemSize = 0U;

        SvcCalibTask_ModShellStrToU32(pArgVector[2U], &UserMemBase);
        SvcCalibTask_ModShellStrToU32(pArgVector[3U], &UserMemSize);

        PRetVal = SvcCalibTask_ModuleInit(); PRN_CAL_MTSK_ERR_HDLR
    }

    return RetVal;
}


static void SvcCalibTask_ModShellInitU(UINT32 CtrlFlag)
{
    PRN_CAL_MTSK_LOG "  %sinit%s            : initial storage module module%s"
        PRN_CAL_MTSK_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_CAL_HL_TITLE_1_U:SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_CAL_HL_DEF_FC:SVC_LOG_CAL_HL_END        PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_END                                              PRN_CAL_MTSK_ARG_POST
    PRN_CAL_MTSK_API
    if (CtrlFlag > 0U) {
        PRN_CAL_MTSK_LOG "      UserMemBase : Configure user memory base" PRN_CAL_MTSK_API
        PRN_CAL_MTSK_LOG "      UserMemSize : Configure user memory size" PRN_CAL_MTSK_API
        PRN_CAL_MTSK_LOG " " PRN_CAL_MTSK_API
    }
}

static UINT32 SvcCalibTask_ModShellCreate(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount < 3U) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Failure to proc 'create' cmd - invalid arg count!" PRN_CAL_MTSK_NG
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 FovID = 0xCAFEU;

        SvcCalibTask_ModShellStrToU32(pArgVector[2U], &FovID);

        if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
            PRN_CAL_MTSK_LOG "Invalid FovID(%d)"
                PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_NG
        } else {
            PRetVal = SvcCalibTask_ModuleCreate(FovID); PRN_CAL_MTSK_ERR_HDLR
        }
    }

    return RetVal;
}


static void SvcCalibTask_ModShellCreateU(UINT32 CtrlFlag)
{
    PRN_CAL_MTSK_LOG "  %screate%s          : create storage module module%s"
        PRN_CAL_MTSK_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_CAL_HL_TITLE_1_U:SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_CAL_HL_DEF_FC:SVC_LOG_CAL_HL_END        PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_END                                              PRN_CAL_MTSK_ARG_POST
    PRN_CAL_MTSK_API
    if (CtrlFlag > 0U) {
        PRN_CAL_MTSK_LOG "            FovID : Configure fov id" PRN_CAL_MTSK_API
        PRN_CAL_MTSK_LOG " " PRN_CAL_MTSK_API
    }
}


static UINT32 SvcCalibTask_ModShellDelete(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount < 3U) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Failure to proc 'delete' cmd - invalid arg count!" PRN_CAL_MTSK_NG
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 FovID = 0xCAFEU;

        SvcCalibTask_ModShellStrToU32(pArgVector[2U], &FovID);

        if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
            PRN_CAL_MTSK_LOG "Invalid FovID(%d)"
                PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_NG
        } else {
            PRetVal = SvcCalibTask_ModuleDelete(FovID); PRN_CAL_MTSK_ERR_HDLR
        }
    }

    return RetVal;
}


static void SvcCalibTask_ModShellDeleteU(UINT32 CtrlFlag)
{
    PRN_CAL_MTSK_LOG "  %sdelete%s          : delete storage module module%s"
        PRN_CAL_MTSK_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_CAL_HL_TITLE_1_U:SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_CAL_HL_DEF_FC:SVC_LOG_CAL_HL_END        PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_END                                              PRN_CAL_MTSK_ARG_POST
    PRN_CAL_MTSK_API
    if (CtrlFlag > 0U) {
        PRN_CAL_MTSK_LOG "            FovID : Configure fov id" PRN_CAL_MTSK_API
        PRN_CAL_MTSK_LOG " " PRN_CAL_MTSK_API
    }
}


static UINT32 SvcCalibTask_ModShellSave(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount < 3U) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Failure to proc 'save' cmd - invalid arg count!" PRN_CAL_MTSK_NG
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 FovID = 0xCAFEU;

        SvcCalibTask_ModShellStrToU32(pArgVector[2U], &FovID);

        if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
            PRN_CAL_MTSK_LOG "Invalid FovID(%d)"
                PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_NG
        } else {
            SVC_CALIB_CHANNEL_s CalibChan;
            if(SVC_OK == SvcCalibTask_ModuleCalChanGet(FovID, &CalibChan)) {
                SvcCalibTask_ModuleSerDesQryPause(CalibChan.VinID);
                PRetVal = SvcModule_Save(CalibModuleCtrl.pModuleCtrl[FovID]); PRN_CAL_MTSK_ERR_HDLR
                SvcCalibTask_ModuleSerDesQryResume(CalibChan.VinID);
            }
        }
    }

    return RetVal;
}


static void SvcCalibTask_ModShellSaveU(UINT32 CtrlFlag)
{
    PRN_CAL_MTSK_LOG "  %ssave%s            : write data to storage module module%s"
        PRN_CAL_MTSK_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_CAL_HL_TITLE_1_U:SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_CAL_HL_DEF_FC:SVC_LOG_CAL_HL_END        PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_END                                              PRN_CAL_MTSK_ARG_POST
    PRN_CAL_MTSK_API
    if (CtrlFlag > 0U) {
        PRN_CAL_MTSK_LOG "            FovID : Configure fov id" PRN_CAL_MTSK_API
        PRN_CAL_MTSK_LOG " " PRN_CAL_MTSK_API
    }
}


static UINT32 SvcCalibTask_ModShellInfo(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount < 3U) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Failure to proc 'info' cmd - invalid arg count!" PRN_CAL_MTSK_NG
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 FovID = 0xCAFEU;

        SvcCalibTask_ModShellStrToU32(pArgVector[2U], &FovID);

        if (FovID == 0xCAFEU) {
            SvcModule_InfoAll();
        } else {
            if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
                PRN_CAL_MTSK_LOG "Invalid FovID(%d)"
                    PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_NG
            } else {
                SvcModule_Info(CalibModuleCtrl.pModuleCtrl[FovID]);
            }
        }
    }

    return RetVal;
}


static void SvcCalibTask_ModShellInfoU(UINT32 CtrlFlag)
{
    PRN_CAL_MTSK_LOG "  %sinfo%s            : print svc storage module control info%s"
        PRN_CAL_MTSK_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_CAL_HL_TITLE_1_U:SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_CAL_HL_DEF_FC:SVC_LOG_CAL_HL_END        PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_END                                              PRN_CAL_MTSK_ARG_POST
    PRN_CAL_MTSK_API
    if (CtrlFlag > 0U) {
        PRN_CAL_MTSK_LOG "            FovID : Configure fov id" PRN_CAL_MTSK_API
        PRN_CAL_MTSK_LOG " " PRN_CAL_MTSK_API
    }
}


static UINT32 SvcCalibTask_ModShellReset(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount < 3U) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Failure to proc 'reset' cmd - invalid arg count!" PRN_CAL_MTSK_NG
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 FovID = 0xCAFEU;

        SvcCalibTask_ModShellStrToU32(pArgVector[2U], &FovID);

        if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
            PRN_CAL_MTSK_LOG "Invalid FovID(%d)"
                PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_NG
        } else {
            PRetVal = SvcModule_Reset(CalibModuleCtrl.pModuleCtrl[FovID]); PRN_CAL_MTSK_ERR_HDLR
        }
    }

    return RetVal;
}


static void SvcCalibTask_ModShellResetU(UINT32 CtrlFlag)
{
    PRN_CAL_MTSK_LOG "  %sreset%s           : reset svc storage module%s"
        PRN_CAL_MTSK_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_CAL_HL_TITLE_1_U:SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_CAL_HL_DEF_FC:SVC_LOG_CAL_HL_END        PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_END                                              PRN_CAL_MTSK_ARG_POST
    PRN_CAL_MTSK_API
    if (CtrlFlag > 0U) {
        PRN_CAL_MTSK_LOG "            FovID : Configure fov id" PRN_CAL_MTSK_API
        PRN_CAL_MTSK_LOG " " PRN_CAL_MTSK_API
    }
}


static UINT32 SvcCalibTask_ModShellDbgMsg(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount < 3U) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Failure to proc 'debug' cmd - invalid arg count!" PRN_CAL_MTSK_NG
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 Enable = 0xCAFEU;

        SvcCalibTask_ModShellStrToU32(pArgVector[2U], &Enable);

        SvcModule_DebugMsg(Enable);
    }

    return RetVal;
}


static void SvcCalibTask_ModShellDbgMsgU(UINT32 CtrlFlag)
{
    PRN_CAL_MTSK_LOG "  %sdebug%s           : enable/disable storage module debug message%s"
        PRN_CAL_MTSK_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_CAL_HL_TITLE_1_U:SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_CAL_HL_DEF_FC:SVC_LOG_CAL_HL_END        PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_END                                              PRN_CAL_MTSK_ARG_POST
    PRN_CAL_MTSK_API
    if (CtrlFlag > 0U) {
        PRN_CAL_MTSK_LOG "           Enable : 0 -> disable. 1 -> enable" PRN_CAL_MTSK_API
        PRN_CAL_MTSK_LOG " " PRN_CAL_MTSK_API
    }
}


static UINT32 SvcCalibTask_ModShellVig(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount <= 2U) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Failure to proc svc storage module vig shell cmd - invalid arg count!" PRN_CAL_MTSK_NG
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 ShellIdx;
        UINT32 ShellCount = (UINT32)(sizeof(CalibModuleShellVig)) / (UINT32)(sizeof(CalibModuleShellVig[0]));

        RetVal = SVC_NG;

        for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
            if ((CalibModuleShellVig[ShellIdx].pFunc != NULL) && (CalibModuleShellVig[ShellIdx].Enable > 0U)) {
                if (0 == SvcWrap_strcmp(pArgVector[2U], CalibModuleShellVig[ShellIdx].CmdName)) {
                    if (SVC_OK != (CalibModuleShellVig[ShellIdx].pFunc)(ArgCount, pArgVector)) {
                        if (CalibModuleShellWb[ShellIdx].pUsageFunc != NULL) {
                            (CalibModuleShellWb[ShellIdx].pUsageFunc)(1U);
                        }
                    }
                    RetVal = SVC_OK;
                    break;
                }
            }
        }

        if (RetVal != SVC_OK) {
            PRN_CAL_MTSK_LOG "Failure to proc svc storage module vig shell cmd - invalid cmd!" PRN_CAL_MTSK_NG
        }
    }

    if (RetVal != SVC_OK) {
        SvcCalibTask_ModShellVigU(1U);
    }

    return SVC_OK;
}


static void SvcCalibTask_ModShellVigU(UINT32 CtrlFlag)
{
    UINT32 ShellIdx;
    UINT32 ShellCount = (UINT32)(sizeof(CalibModuleShellVig)) / (UINT32)(sizeof(CalibModuleShellVig[0]));

    PRN_CAL_MTSK_LOG "  %svig%s             : vignette storage module"
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1_U PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_DEF_FC    PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_END       PRN_CAL_MTSK_ARG_POST
    PRN_CAL_MTSK_API

    for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
        if ((CalibModuleShellVig[ShellIdx].pFunc != NULL) && (CalibModuleShellVig[ShellIdx].Enable > 0U)) {
            if (CalibModuleShellVig[ShellIdx].pUsageFunc == NULL) {
                PRN_CAL_MTSK_LOG "    %s"
                    PRN_CAL_MTSK_ARG_CSTR   CalibModuleShellVig[ShellIdx].CmdName PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
            } else {
                (CalibModuleShellVig[ShellIdx].pUsageFunc)(CtrlFlag);
            }
        }
    }

    PRN_CAL_MTSK_LOG " " PRN_CAL_MTSK_API
}


static UINT32 SvcCalibTask_ModShellVigSet(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount < 5U) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Failure to proc 'vig->set' cmd - invalid arg count!" PRN_CAL_MTSK_NG
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 FovID     = 0xCAFEU;
        UINT32 TblIdx    = 0xCAFEU;

        SvcCalibTask_ModShellStrToU32(pArgVector[3U], &FovID);
        SvcCalibTask_ModShellStrToU32(pArgVector[4U], &TblIdx);

        if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
            RetVal = SVC_NG;
            PRN_CAL_MTSK_LOG "Invalid FovID(%d)"
                PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_NG
        } else {
            SVC_CALIB_CHANNEL_s CalibChan;

            if(SVC_OK == SvcCalibTask_ModuleCalChanGet(FovID, &CalibChan)) {

                PRN_CAL_MTSK_LOG "CalibChan.SensorID: 0x%x, SensorSelectBits: 0x%x"
                    PRN_CAL_MTSK_ARG_UINT32 CalibChan.SensorID         PRN_CAL_MTSK_ARG_POST
                    PRN_CAL_MTSK_ARG_UINT32 CalibChan.SensorSelectBits PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API

                PRetVal = SvcCalibTask_ModuleVigSet(CalibModuleCtrl.pModuleCtrl[FovID], &CalibChan, TblIdx); PRN_CAL_MTSK_ERR_HDLR
            }
        }
    }

    return RetVal;
}


static void SvcCalibTask_ModShellVigSetU(UINT32 CtrlFlag)
{
    PRN_CAL_MTSK_LOG "    %sset%s           : set vig data to storage module from calib vig module%s"
        PRN_CAL_MTSK_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_CAL_HL_TITLE_2_U:SVC_LOG_CAL_HL_TITLE_2 PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_CAL_HL_DEF_FC:SVC_LOG_CAL_HL_END        PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_END                                              PRN_CAL_MTSK_ARG_POST
    PRN_CAL_MTSK_API
    if (CtrlFlag > 0U) {
        PRN_CAL_MTSK_LOG "            FovID : Configure fov id" PRN_CAL_MTSK_API
        PRN_CAL_MTSK_LOG "           TblIdx : Configure calib vig table index" PRN_CAL_MTSK_API
        PRN_CAL_MTSK_LOG " " PRN_CAL_MTSK_API
    }
}


static UINT32 SvcCalibTask_ModShellVigUpd(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount < 4U) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Failure to proc 'vig->upd' cmd - invalid arg count!" PRN_CAL_MTSK_NG
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 FovID = 0xCAFEU;

        SvcCalibTask_ModShellStrToU32(pArgVector[5U], &FovID);

        if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
            RetVal = SVC_NG;
            PRN_CAL_MTSK_LOG "Invalid FovID(%d)"
                PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_NG
        } else {
            SVC_CALIB_CHANNEL_s CalibChan;
            if(SVC_OK == SvcCalibTask_ModuleCalChanGet(FovID, &CalibChan)) {
                PRetVal = SvcCalibTask_ModuleVigUpd(CalibModuleCtrl.pModuleCtrl[FovID], &CalibChan); PRN_CAL_MTSK_ERR_HDLR
            }
        }
    }

    return RetVal;
}


static void SvcCalibTask_ModShellVigUpdU(UINT32 CtrlFlag)
{
    PRN_CAL_MTSK_LOG "    %supd%s           : update vig data calib vig module from storage module%s"
        PRN_CAL_MTSK_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_CAL_HL_TITLE_2_U:SVC_LOG_CAL_HL_TITLE_2 PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_CAL_HL_DEF_FC:SVC_LOG_CAL_HL_END        PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_END                                              PRN_CAL_MTSK_ARG_POST
    PRN_CAL_MTSK_API
    if (CtrlFlag > 0U) {
        PRN_CAL_MTSK_LOG "            FovID : Configure fov id" PRN_CAL_MTSK_API
        PRN_CAL_MTSK_LOG " " PRN_CAL_MTSK_API
    }
}


static UINT32 SvcCalibTask_ModShellBpc(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount <= 2U) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Failure to proc svc storage module bpc shell cmd - invalid arg count!" PRN_CAL_MTSK_NG
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 ShellIdx;
        UINT32 ShellCount = (UINT32)(sizeof(CalibModuleShellBpc)) / (UINT32)(sizeof(CalibModuleShellBpc[0]));

        RetVal = SVC_NG;

        for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
            if ((CalibModuleShellBpc[ShellIdx].pFunc != NULL) && (CalibModuleShellBpc[ShellIdx].Enable > 0U)) {
                if (0 == SvcWrap_strcmp(pArgVector[2U], CalibModuleShellBpc[ShellIdx].CmdName)) {
                    if (SVC_OK != (CalibModuleShellBpc[ShellIdx].pFunc)(ArgCount, pArgVector)) {
                        if (CalibModuleShellWb[ShellIdx].pUsageFunc != NULL) {
                            (CalibModuleShellWb[ShellIdx].pUsageFunc)(1U);
                        }
                    }
                    RetVal = SVC_OK;
                    break;
                }
            }
        }

        if (RetVal != SVC_OK) {
            PRN_CAL_MTSK_LOG "Failure to proc svc storage module bpc shell cmd - invalid cmd!" PRN_CAL_MTSK_NG
        }
    }

    if (RetVal != SVC_OK) {
        SvcCalibTask_ModShellBpcU(1U);
    }

    return SVC_OK;
}


static void SvcCalibTask_ModShellBpcU(UINT32 CtrlFlag)
{
    UINT32 ShellIdx;
    UINT32 ShellCount = (UINT32)(sizeof(CalibModuleShellBpc)) / (UINT32)(sizeof(CalibModuleShellBpc[0]));

    PRN_CAL_MTSK_LOG "  %sbpc%s             : bad pixel storage module"
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1_U PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_DEF_FC    PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_END       PRN_CAL_MTSK_ARG_POST
    PRN_CAL_MTSK_API

    for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
        if ((CalibModuleShellBpc[ShellIdx].pFunc != NULL) && (CalibModuleShellBpc[ShellIdx].Enable > 0U)) {
            if (CalibModuleShellBpc[ShellIdx].pUsageFunc == NULL) {
                PRN_CAL_MTSK_LOG "    %s"
                    PRN_CAL_MTSK_ARG_CSTR   CalibModuleShellBpc[ShellIdx].CmdName PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
            } else {
                (CalibModuleShellBpc[ShellIdx].pUsageFunc)(CtrlFlag);
            }
        }
    }

    PRN_CAL_MTSK_LOG " " PRN_CAL_MTSK_API
}


static UINT32 SvcCalibTask_ModShellBpcSet(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount < 4U) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Failure to proc 'bpc->set' cmd - invalid arg count!" PRN_CAL_MTSK_NG
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 FovID     = 0xCAFEU;
        UINT32 TblIdx    = 0xCAFEU;

        SvcCalibTask_ModShellStrToU32(pArgVector[3U], &FovID);
        SvcCalibTask_ModShellStrToU32(pArgVector[4U], &TblIdx);

        if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
            RetVal = SVC_NG;
            PRN_CAL_MTSK_LOG "Invalid FovID(%d)"
                PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_NG
        } else {
            SVC_CALIB_CHANNEL_s CalibChan;

            if(SVC_OK == SvcCalibTask_ModuleCalChanGet(FovID, &CalibChan)) {

                PRN_CAL_MTSK_LOG "CalibChan.SensorID: 0x%x, SensorSelectBits: 0x%x"
                    PRN_CAL_MTSK_ARG_UINT32 CalibChan.SensorID         PRN_CAL_MTSK_ARG_POST
                    PRN_CAL_MTSK_ARG_UINT32 CalibChan.SensorSelectBits PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API

                PRetVal = SvcCalibTask_ModuleBpcSet(CalibModuleCtrl.pModuleCtrl[FovID], &CalibChan); PRN_CAL_MTSK_ERR_HDLR
            }
        }
    }

    return RetVal;
}


static void SvcCalibTask_ModShellBpcSetU(UINT32 CtrlFlag)
{
    PRN_CAL_MTSK_LOG "    %sset%s           : set bpc data to storage module from calib bpc module%s"
        PRN_CAL_MTSK_ARG_CSTR   ((CtrlFlag > 0U)?SVC_LOG_CAL_HL_TITLE_2_U:SVC_LOG_CAL_HL_TITLE_2) PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   ((CtrlFlag > 0U)?SVC_LOG_CAL_HL_DEF_FC:SVC_LOG_CAL_HL_END)        PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_END                                                PRN_CAL_MTSK_ARG_POST
    PRN_CAL_MTSK_API

    if (CtrlFlag > 0U) {
        PRN_CAL_MTSK_LOG "            FovID : Configure fov id" PRN_CAL_MTSK_API
        PRN_CAL_MTSK_LOG "           TblIdx : Configure calib bpc table index" PRN_CAL_MTSK_API
        PRN_CAL_MTSK_LOG " " PRN_CAL_MTSK_API
    }
}


static UINT32 SvcCalibTask_ModShellBpcUpd(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount < 4U) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Failure to proc 'bpc->upd' cmd - invalid arg count!" PRN_CAL_MTSK_NG
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 FovID     = 0xCAFEU;

        SvcCalibTask_ModShellStrToU32(pArgVector[4U], &FovID);

        if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
            RetVal = SVC_NG;
            PRN_CAL_MTSK_LOG "Invalid FovID(%d)"
                PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_NG
        } else {
            SVC_CALIB_CHANNEL_s CalibChan;

            if(SVC_OK == SvcCalibTask_ModuleCalChanGet(FovID, &CalibChan)) {

                PRetVal = SvcCalibTask_ModuleBpcUpd(CalibModuleCtrl.pModuleCtrl[FovID], &CalibChan); PRN_CAL_MTSK_ERR_HDLR
            }
        }
    }

    return RetVal;
}


static void SvcCalibTask_ModShellBpcUpdU(UINT32 CtrlFlag)
{
    PRN_CAL_MTSK_LOG "    %supd%s           : update bpc data calib bpc module from storage module%s"
        PRN_CAL_MTSK_ARG_CSTR   ((CtrlFlag > 0U)?SVC_LOG_CAL_HL_TITLE_2_U:SVC_LOG_CAL_HL_TITLE_2) PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   ((CtrlFlag > 0U)?SVC_LOG_CAL_HL_DEF_FC:SVC_LOG_CAL_HL_END)        PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_END                                                PRN_CAL_MTSK_ARG_POST
    PRN_CAL_MTSK_API
    if (CtrlFlag > 0U) {
        PRN_CAL_MTSK_LOG "            FovID : Configure fov id" PRN_CAL_MTSK_API
        PRN_CAL_MTSK_LOG " " PRN_CAL_MTSK_API
    }
}


static UINT32 SvcCalibTask_ModShellBpcInfo(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount < 4U) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Failure to proc 'bpc->info' cmd - invalid arg count!" PRN_CAL_MTSK_NG
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 FovID     = 0xCAFEU;

        SvcCalibTask_ModShellStrToU32(pArgVector[3U], &FovID);

        if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
            RetVal = SVC_NG;
            PRN_CAL_MTSK_LOG "Invalid FovID(%d)"
                PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_NG
        } else {
            PRetVal = SvcCalibTask_ModuleBpcInfo(CalibModuleCtrl.pModuleCtrl[FovID]); PRN_CAL_MTSK_ERR_HDLR
        }
    }

    return RetVal;
}


static void SvcCalibTask_ModShellBpcInfoU(UINT32 CtrlFlag)
{
    PRN_CAL_MTSK_LOG "    %sinfo%s          : print the storage module bpc data header%s"
        PRN_CAL_MTSK_ARG_CSTR   ((CtrlFlag > 0U)?SVC_LOG_CAL_HL_TITLE_2_U:SVC_LOG_CAL_HL_TITLE_2) PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   ((CtrlFlag > 0U)?SVC_LOG_CAL_HL_DEF_FC:SVC_LOG_CAL_HL_END)        PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_END                                                PRN_CAL_MTSK_ARG_POST
    PRN_CAL_MTSK_API

    if (CtrlFlag > 0U) {
        PRN_CAL_MTSK_LOG "            FovID : Configure fov id" PRN_CAL_MTSK_API
        PRN_CAL_MTSK_LOG " " PRN_CAL_MTSK_API
    }
}


static UINT32 SvcCalibTask_ModShellCam(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount <= 2U) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Failure to proc svc storage module camera info shell cmd - invalid arg count!" PRN_CAL_MTSK_NG
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 ShellIdx;
        UINT32 ShellCount = (UINT32)(sizeof(CalibModuleShellCamInfo)) / (UINT32)(sizeof(CalibModuleShellCamInfo[0]));

        RetVal = SVC_NG;

        for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
            if ((CalibModuleShellCamInfo[ShellIdx].pFunc != NULL) && (CalibModuleShellCamInfo[ShellIdx].Enable > 0U)) {
                if (0 == SvcWrap_strcmp(pArgVector[2U], CalibModuleShellCamInfo[ShellIdx].CmdName)) {
                    if (SVC_OK != (CalibModuleShellCamInfo[ShellIdx].pFunc)(ArgCount, pArgVector)) {
                        if (CalibModuleShellWb[ShellIdx].pUsageFunc != NULL) {
                            (CalibModuleShellWb[ShellIdx].pUsageFunc)(1U);
                        }
                    }
                    RetVal = SVC_OK;
                    break;
                }
            }
        }

        if (RetVal != SVC_OK) {
            PRN_CAL_MTSK_LOG "Failure to proc svc storage module caminfo shell cmd - invalid cmd!" PRN_CAL_MTSK_NG
        }
    }

    if (RetVal != SVC_OK) {
        SvcCalibTask_ModShellCamU(1U);
    }

    return SVC_OK;
}


static void SvcCalibTask_ModShellCamU(UINT32 CtrlFlag)
{
    UINT32 ShellIdx;
    UINT32 ShellCount = (UINT32)(sizeof(CalibModuleShellCamInfo)) / (UINT32)(sizeof(CalibModuleShellCamInfo[0]));

    PRN_CAL_MTSK_LOG "  %scaminfo%s         : camera info storage module%s"
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1_U PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_DEF_FC    PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_END       PRN_CAL_MTSK_ARG_POST
    PRN_CAL_MTSK_API

    for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
        if ((CalibModuleShellCamInfo[ShellIdx].pFunc != NULL) && (CalibModuleShellCamInfo[ShellIdx].Enable > 0U)) {
            if (CalibModuleShellCamInfo[ShellIdx].pUsageFunc == NULL) {
                PRN_CAL_MTSK_LOG "    %s"
                    PRN_CAL_MTSK_ARG_CSTR   CalibModuleShellCamInfo[ShellIdx].CmdName PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
            } else {
                (CalibModuleShellCamInfo[ShellIdx].pUsageFunc)(CtrlFlag);
            }
        }
    }

    PRN_CAL_MTSK_LOG " " PRN_CAL_MTSK_API
}


static UINT32 SvcCalibTask_ModShellCamSet(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount < 10U) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Failure to proc 'caminfo->set' cmd - invalid arg count!" PRN_CAL_MTSK_NG
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 FovID     = 0xCAFEU;
        UINT32 CenterX   = 0U;
        UINT32 CenterY   = 0U;
        DOUBLE ScaleX;
        DOUBLE ScaleY;
        DOUBLE Skew;
        DOUBLE FocuseLen;

        SvcCalibTask_ModShellStrToU32(pArgVector[3U], &FovID);
        SvcCalibTask_ModShellStrToU32(pArgVector[4U], &CenterX);
        SvcCalibTask_ModShellStrToU32(pArgVector[5U], &CenterY);
        ScaleX    = SvcWrap_strtod(pArgVector[6U]);
        ScaleY    = SvcWrap_strtod(pArgVector[7U]);
        Skew      = SvcWrap_strtod(pArgVector[8U]);
        FocuseLen = SvcWrap_strtod(pArgVector[9U]);

        if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
            RetVal = SVC_NG;
            PRN_CAL_MTSK_LOG "Invalid FovID(%d)"
                PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_NG
        } else {
            FULLVIN_INTRINSIC_INFORMATION_s CameraInfo;

            SvcCalibTask_Modulememset(&CameraInfo, 0, sizeof(CameraInfo));
            CameraInfo.Center.X     = CenterX;
            CameraInfo.Center.Y     = CenterY;
            CameraInfo.ScaleFactorX = ScaleX;
            CameraInfo.ScaleFactorY = ScaleY;
            CameraInfo.Skew         = Skew;
            CameraInfo.FocusLength  = FocuseLen;

            PRetVal = SvcCalibTask_ModuleCamInfoSet(CalibModuleCtrl.pModuleCtrl[FovID], &CameraInfo);
            if (PRetVal == 0U) {
                PRN_CAL_MTSK_LOG "Success to set FovID(%d) camerainfo data"
                    PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
            }
        }
    }

    return RetVal;
}


static void SvcCalibTask_ModShellCamSetU(UINT32 CtrlFlag)
{
    PRN_CAL_MTSK_LOG "    %sset%s           : set camera info to storage module from shell cmd%s"
        PRN_CAL_MTSK_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_CAL_HL_TITLE_2_U:SVC_LOG_CAL_HL_TITLE_2 PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_CAL_HL_DEF_FC:SVC_LOG_CAL_HL_END        PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_END                                              PRN_CAL_MTSK_ARG_POST
    PRN_CAL_MTSK_API
    if (CtrlFlag > 0U) {
        PRN_CAL_MTSK_LOG "            FovID : Configure fov id" PRN_CAL_MTSK_API
        PRN_CAL_MTSK_LOG "          CenterX : Configure camera center X" PRN_CAL_MTSK_API
        PRN_CAL_MTSK_LOG "          CenterY : Configure camera center Y" PRN_CAL_MTSK_API
        PRN_CAL_MTSK_LOG "           ScaleX : Configure Scale Factor X (double)" PRN_CAL_MTSK_API
        PRN_CAL_MTSK_LOG "           ScaleY : Configure Scale Factor Y (double)" PRN_CAL_MTSK_API
        PRN_CAL_MTSK_LOG "             Skew : Configure Skew (double)" PRN_CAL_MTSK_API
        PRN_CAL_MTSK_LOG "        FocuseLen : Configure Focus Length (double)" PRN_CAL_MTSK_API
        PRN_CAL_MTSK_LOG " " PRN_CAL_MTSK_API
    }
}


static UINT32 SvcCalibTask_ModShellCamGet(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount < 4U) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Failure to proc 'caminfo->get' cmd - invalid arg count!" PRN_CAL_MTSK_NG
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 FovID = 0xCAFEU;

        SvcCalibTask_ModShellStrToU32(pArgVector[3U], &FovID);

        if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
            RetVal = SVC_NG;
            PRN_CAL_MTSK_LOG "Invalid FovID(%d)"
                PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_NG
        } else {
            FULLVIN_INTRINSIC_INFORMATION_s CameraInfo;

            SvcCalibTask_Modulememset(&CameraInfo, 0, sizeof(CameraInfo));

            PRetVal = SvcCalibTask_ModuleCamInfoGet(CalibModuleCtrl.pModuleCtrl[FovID], &CameraInfo);

            if (PRetVal == 0U) {
                PRN_CAL_MTSK_LOG " " PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "---- FovID(%d) Calib Module CameraInfo ----"
                    PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "  CameraInfo.Center.X     : %d"
                    PRN_CAL_MTSK_ARG_UINT32 CameraInfo.Center.X PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "  CameraInfo.Center.Y     : %d"
                    PRN_CAL_MTSK_ARG_UINT32 CameraInfo.Center.Y PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "  CameraInfo.ScaleFactorX : %8.5f"
                    PRN_CAL_MTSK_ARG_DOUBLE CameraInfo.ScaleFactorX
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "  CameraInfo.ScaleFactorY : %8.5f"
                    PRN_CAL_MTSK_ARG_DOUBLE CameraInfo.ScaleFactorY
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "  CameraInfo.Skew         : %8.5f"
                    PRN_CAL_MTSK_ARG_DOUBLE CameraInfo.Skew
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "  CameraInfo.FocusLength  : %8.5f"
                    PRN_CAL_MTSK_ARG_DOUBLE CameraInfo.FocusLength
                PRN_CAL_MTSK_API
            }
        }
    }

    return RetVal;
}


static void SvcCalibTask_ModShellCamGetU(UINT32 CtrlFlag)
{
    PRN_CAL_MTSK_LOG "    %sget%s           : get camera info from storage module%s"
        PRN_CAL_MTSK_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_CAL_HL_TITLE_2_U:SVC_LOG_CAL_HL_TITLE_2 PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_CAL_HL_DEF_FC:SVC_LOG_CAL_HL_END        PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_END                                              PRN_CAL_MTSK_ARG_POST
    PRN_CAL_MTSK_API
    if (CtrlFlag > 0U) {
        PRN_CAL_MTSK_LOG "            FovID : Configure fov id" PRN_CAL_MTSK_API
        PRN_CAL_MTSK_LOG " " PRN_CAL_MTSK_API
    }
}


static UINT32 SvcCalibTask_ModShellWb(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount <= 2U) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Failure to proc svc storage module wb shell cmd - invalid arg count!" PRN_CAL_MTSK_NG
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 ShellIdx;
        UINT32 ShellCount = (UINT32)(sizeof(CalibModuleShellWb)) / (UINT32)(sizeof(CalibModuleShellWb[0]));

        RetVal = SVC_NG;

        for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
            if ((CalibModuleShellWb[ShellIdx].pFunc != NULL) && (CalibModuleShellWb[ShellIdx].Enable > 0U)) {
                if (0 == SvcWrap_strcmp(pArgVector[2U], CalibModuleShellWb[ShellIdx].CmdName)) {
                    if (SVC_OK != (CalibModuleShellWb[ShellIdx].pFunc)(ArgCount, pArgVector)) {
                        if (CalibModuleShellWb[ShellIdx].pUsageFunc != NULL) {
                            (CalibModuleShellWb[ShellIdx].pUsageFunc)(1U);
                        }
                    }

                    RetVal = SVC_OK;
                    break;
                }
            }
        }

        if (RetVal != SVC_OK) {
            PRN_CAL_MTSK_LOG "Failure to proc svc storage module wb shell cmd - invalid cmd!" PRN_CAL_MTSK_NG
        }
    }

    if (RetVal != SVC_OK) {
        SvcCalibTask_ModShellWbU(1U);
    }

    return SVC_OK;
}


static void SvcCalibTask_ModShellWbU(UINT32 CtrlFlag)
{
    UINT32 ShellIdx;
    UINT32 ShellCount = (UINT32)(sizeof(CalibModuleShellWb)) / (UINT32)(sizeof(CalibModuleShellWb[0]));

    PRN_CAL_MTSK_LOG "  %swb%s              : white balance storage module"
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1_U PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_DEF_FC    PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_END       PRN_CAL_MTSK_ARG_POST
    PRN_CAL_MTSK_API

    for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
        if ((CalibModuleShellWb[ShellIdx].pFunc != NULL) && (CalibModuleShellWb[ShellIdx].Enable > 0U)) {
            if (CalibModuleShellWb[ShellIdx].pUsageFunc == NULL) {
                PRN_CAL_MTSK_LOG "    %s"
                    PRN_CAL_MTSK_ARG_CSTR   CalibModuleShellWb[ShellIdx].CmdName PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
            } else {
                (CalibModuleShellWb[ShellIdx].pUsageFunc)(CtrlFlag);
            }
        }
    }

    PRN_CAL_MTSK_LOG " " PRN_CAL_MTSK_API
}


static UINT32 SvcCalibTask_ModShellWbSet(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount < 5U) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Failure to proc 'wb->set' cmd - invalid arg count!" PRN_CAL_MTSK_NG
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 FovID  = 0xCAFEU;
        UINT32 TblIdx = 0xCAFEU;

        SvcCalibTask_ModShellStrToU32(pArgVector[3U], &FovID);
        SvcCalibTask_ModShellStrToU32(pArgVector[4U], &TblIdx);

        if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
            RetVal = SVC_NG;
            PRN_CAL_MTSK_LOG "Invalid FovID(%d)"
                PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_NG
        } else {
            SVC_CALIB_CHANNEL_s CalibChan;

            if(SVC_OK == SvcCalibTask_ModuleCalChanGet(FovID, &CalibChan)) {

                PRetVal = SvcCalibTask_ModuleWbSet(CalibModuleCtrl.pModuleCtrl[FovID], &CalibChan, TblIdx); PRN_CAL_MTSK_ERR_HDLR
            }
        }
    }

    return RetVal;
}


static void SvcCalibTask_ModShellWbSetU(UINT32 CtrlFlag)
{
    PRN_CAL_MTSK_LOG "    %sset%s           : set wb data to storage module from calib wb module%s"
        PRN_CAL_MTSK_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_CAL_HL_TITLE_2_U:SVC_LOG_CAL_HL_TITLE_2 PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_CAL_HL_DEF_FC:SVC_LOG_CAL_HL_END        PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_END                                              PRN_CAL_MTSK_ARG_POST
    PRN_CAL_MTSK_API
    if (CtrlFlag > 0U) {
        PRN_CAL_MTSK_LOG "            FovID : Configure fov id" PRN_CAL_MTSK_API
        PRN_CAL_MTSK_LOG "           TblIdx : Configure calib wb table index" PRN_CAL_MTSK_API
        PRN_CAL_MTSK_LOG " " PRN_CAL_MTSK_API
    }
}


static UINT32 SvcCalibTask_ModShellWbGet(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount < 4U) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Failure to proc 'wb->get' cmd - invalid arg count!" PRN_CAL_MTSK_NG
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 FovID     = 0xCAFEU;

        SvcCalibTask_ModShellStrToU32(pArgVector[3U], &FovID);

        if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
            RetVal = SVC_NG;
            PRN_CAL_MTSK_LOG "Invalid FovID(%d)"
                PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_NG
        } else {
            WHITE_BLANCE_s WbData;

            SvcCalibTask_Modulememset(&WbData, 0, sizeof(WbData));

            PRetVal = SvcCalibTask_ModuleWbGet(CalibModuleCtrl.pModuleCtrl[FovID], &WbData);

            if (PRetVal == 0U) {
                PRN_CAL_MTSK_LOG " " PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "---- FovID(%d) Calib Module White Balance ----"
                    PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "   Low Color Temperature  Original GainR : %d"
                    PRN_CAL_MTSK_ARG_UINT32 WbData.OrgWB[0].GainR PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "   Low Color Temperature  Original GainG : %d"
                    PRN_CAL_MTSK_ARG_UINT32 WbData.OrgWB[0].GainG PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "   Low Color Temperature  Original GainB : %d"
                    PRN_CAL_MTSK_ARG_UINT32 WbData.OrgWB[0].GainB PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "   Low Color Temperature Reference GainR : %d"
                    PRN_CAL_MTSK_ARG_UINT32 WbData.RefWB[0].GainR PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "   Low Color Temperature Reference GainG : %d"
                    PRN_CAL_MTSK_ARG_UINT32 WbData.RefWB[0].GainG PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "   Low Color Temperature Reference GainB : %d"
                    PRN_CAL_MTSK_ARG_UINT32 WbData.RefWB[0].GainB PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "  High Color Temperature  Original GainR : %d"
                    PRN_CAL_MTSK_ARG_UINT32 WbData.OrgWB[1].GainR PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "  High Color Temperature  Original GainG : %d"
                    PRN_CAL_MTSK_ARG_UINT32 WbData.OrgWB[1].GainG PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "  High Color Temperature  Original GainB : %d"
                    PRN_CAL_MTSK_ARG_UINT32 WbData.OrgWB[1].GainB PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "  High Color Temperature Reference GainR : %d"
                    PRN_CAL_MTSK_ARG_UINT32 WbData.RefWB[1].GainR PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "  High Color Temperature Reference GainG : %d"
                    PRN_CAL_MTSK_ARG_UINT32 WbData.RefWB[1].GainG PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
                PRN_CAL_MTSK_LOG "  High Color Temperature Reference GainB : %d"
                    PRN_CAL_MTSK_ARG_UINT32 WbData.RefWB[1].GainB PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
            }
        }
    }

    return RetVal;
}


static void SvcCalibTask_ModShellWbGetU(UINT32 CtrlFlag)
{
    PRN_CAL_MTSK_LOG "    %sget%s           : get wb from storage module%s"
        PRN_CAL_MTSK_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_CAL_HL_TITLE_2_U:SVC_LOG_CAL_HL_TITLE_2 PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_CAL_HL_DEF_FC:SVC_LOG_CAL_HL_END        PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_END                                              PRN_CAL_MTSK_ARG_POST
    PRN_CAL_MTSK_API
    if (CtrlFlag > 0U) {
        PRN_CAL_MTSK_LOG "            FovID : Configure fov id" PRN_CAL_MTSK_API
        PRN_CAL_MTSK_LOG " " PRN_CAL_MTSK_API
    }
}


static UINT32 SvcCalibTask_ModShellWbUpd(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount < 4U) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Failure to proc 'wb->upd' cmd - invalid arg count!" PRN_CAL_MTSK_NG
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 FovID     = 0xCAFEU;

        SvcCalibTask_ModShellStrToU32(pArgVector[3U], &FovID);

        if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
            RetVal = SVC_NG;
            PRN_CAL_MTSK_LOG "Invalid FovID(%d)"
                PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_NG
        } else {
            SVC_CALIB_CHANNEL_s CalibChan;

            if(SVC_OK == SvcCalibTask_ModuleCalChanGet(FovID, &CalibChan)) {

                PRetVal = SvcCalibTask_ModuleWbUpd(CalibModuleCtrl.pModuleCtrl[FovID], &CalibChan); PRN_CAL_MTSK_ERR_HDLR

            }
        }
    }

    return RetVal;
}


static void SvcCalibTask_ModShellWbUpdU(UINT32 CtrlFlag)
{
    PRN_CAL_MTSK_LOG "    %supd%s           : update wb data to calib wb module from storage module%s"
        PRN_CAL_MTSK_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_CAL_HL_TITLE_2_U:SVC_LOG_CAL_HL_TITLE_2 PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   (CtrlFlag > 0U)?SVC_LOG_CAL_HL_DEF_FC:SVC_LOG_CAL_HL_END        PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_END                                              PRN_CAL_MTSK_ARG_POST
    PRN_CAL_MTSK_API
    if (CtrlFlag > 0U) {
        PRN_CAL_MTSK_LOG "            FovID : Configure fov id" PRN_CAL_MTSK_API
        PRN_CAL_MTSK_LOG " " PRN_CAL_MTSK_API
    }
}


static void SvcCalibTask_ModShellEntryInit(void)
{
    static UINT32 SvcCalibModuleShellInit = 0U;
    if (SvcCalibModuleShellInit == 0U) {
        UINT32 NumEntrys;

        NumEntrys = 0U;
        SvcCalibTask_Modulememset(CalibModuleShellFunc, 0, sizeof(CalibModuleShellFunc));
        CalibModuleShellFunc[NumEntrys] = (SVC_CALIB_MODULE_SHELL_FUNC_s) { 1U, "init",    SvcCalibTask_ModShellInit,   SvcCalibTask_ModShellInitU   }; NumEntrys ++;
        CalibModuleShellFunc[NumEntrys] = (SVC_CALIB_MODULE_SHELL_FUNC_s) { 1U, "create",  SvcCalibTask_ModShellCreate, SvcCalibTask_ModShellCreateU }; NumEntrys ++;
        CalibModuleShellFunc[NumEntrys] = (SVC_CALIB_MODULE_SHELL_FUNC_s) { 1U, "delete",  SvcCalibTask_ModShellDelete, SvcCalibTask_ModShellDeleteU }; NumEntrys ++;
        CalibModuleShellFunc[NumEntrys] = (SVC_CALIB_MODULE_SHELL_FUNC_s) { 1U, "save",    SvcCalibTask_ModShellSave,   SvcCalibTask_ModShellSaveU   }; NumEntrys ++;
        CalibModuleShellFunc[NumEntrys] = (SVC_CALIB_MODULE_SHELL_FUNC_s) { 1U, "info",    SvcCalibTask_ModShellInfo,   SvcCalibTask_ModShellInfoU   }; NumEntrys ++;
        CalibModuleShellFunc[NumEntrys] = (SVC_CALIB_MODULE_SHELL_FUNC_s) { 1U, "reset",   SvcCalibTask_ModShellReset,  SvcCalibTask_ModShellResetU  }; NumEntrys ++;
        CalibModuleShellFunc[NumEntrys] = (SVC_CALIB_MODULE_SHELL_FUNC_s) { 1U, "debug",   SvcCalibTask_ModShellDbgMsg, SvcCalibTask_ModShellDbgMsgU }; NumEntrys ++;
        CalibModuleShellFunc[NumEntrys] = (SVC_CALIB_MODULE_SHELL_FUNC_s) { 1U, "caminfo", SvcCalibTask_ModShellCam,    SvcCalibTask_ModShellCamU    }; NumEntrys ++;
        CalibModuleShellFunc[NumEntrys] = (SVC_CALIB_MODULE_SHELL_FUNC_s) { 1U, "vig",     SvcCalibTask_ModShellVig,    SvcCalibTask_ModShellVigU    }; NumEntrys ++;
        CalibModuleShellFunc[NumEntrys] = (SVC_CALIB_MODULE_SHELL_FUNC_s) { 1U, "bpc",     SvcCalibTask_ModShellBpc,    SvcCalibTask_ModShellBpcU    }; NumEntrys ++;
        CalibModuleShellFunc[NumEntrys] = (SVC_CALIB_MODULE_SHELL_FUNC_s) { 1U, "wb",      SvcCalibTask_ModShellWb,     SvcCalibTask_ModShellWbU     }; NumEntrys ++;
        AmbaMisra_TouchUnused(&NumEntrys);

        NumEntrys = 0U;
        SvcCalibTask_Modulememset(CalibModuleShellVig, 0, sizeof(CalibModuleShellVig));
        CalibModuleShellVig[NumEntrys] = (SVC_CALIB_MODULE_SHELL_FUNC_s) { 1U, "set", SvcCalibTask_ModShellVigSet, SvcCalibTask_ModShellVigSetU }; NumEntrys ++;
        CalibModuleShellVig[NumEntrys] = (SVC_CALIB_MODULE_SHELL_FUNC_s) { 1U, "upd", SvcCalibTask_ModShellVigUpd, SvcCalibTask_ModShellVigUpdU }; NumEntrys ++;
        AmbaMisra_TouchUnused(&NumEntrys);

        NumEntrys = 0U;
        SvcCalibTask_Modulememset(CalibModuleShellBpc, 0, sizeof(CalibModuleShellBpc));
        CalibModuleShellBpc[NumEntrys] = (SVC_CALIB_MODULE_SHELL_FUNC_s) { 1U, "set",  SvcCalibTask_ModShellBpcSet,  SvcCalibTask_ModShellBpcSetU  }; NumEntrys ++;
        CalibModuleShellBpc[NumEntrys] = (SVC_CALIB_MODULE_SHELL_FUNC_s) { 1U, "upd",  SvcCalibTask_ModShellBpcUpd,  SvcCalibTask_ModShellBpcUpdU  }; NumEntrys ++;
        CalibModuleShellBpc[NumEntrys] = (SVC_CALIB_MODULE_SHELL_FUNC_s) { 1U, "info", SvcCalibTask_ModShellBpcInfo, SvcCalibTask_ModShellBpcInfoU }; NumEntrys ++;
        AmbaMisra_TouchUnused(&NumEntrys);

        NumEntrys = 0U;
        SvcCalibTask_Modulememset(CalibModuleShellCamInfo, 0, sizeof(CalibModuleShellCamInfo));
        CalibModuleShellCamInfo[NumEntrys] = (SVC_CALIB_MODULE_SHELL_FUNC_s) { 1U, "set", SvcCalibTask_ModShellCamSet, SvcCalibTask_ModShellCamSetU }; NumEntrys ++;
        CalibModuleShellCamInfo[NumEntrys] = (SVC_CALIB_MODULE_SHELL_FUNC_s) { 1U, "get", SvcCalibTask_ModShellCamGet, SvcCalibTask_ModShellCamGetU }; NumEntrys ++;
        AmbaMisra_TouchUnused(&NumEntrys);

        NumEntrys = 0U;
        SvcCalibTask_Modulememset(CalibModuleShellWb, 0, sizeof(CalibModuleShellWb));
        CalibModuleShellWb[NumEntrys] = (SVC_CALIB_MODULE_SHELL_FUNC_s) { 1U, "set", SvcCalibTask_ModShellWbSet, SvcCalibTask_ModShellWbSetU }; NumEntrys ++;
        CalibModuleShellWb[NumEntrys] = (SVC_CALIB_MODULE_SHELL_FUNC_s) { 1U, "get", SvcCalibTask_ModShellWbGet, SvcCalibTask_ModShellWbGetU }; NumEntrys ++;
        CalibModuleShellWb[NumEntrys] = (SVC_CALIB_MODULE_SHELL_FUNC_s) { 1U, "upd", SvcCalibTask_ModShellWbUpd, SvcCalibTask_ModShellWbUpdU }; NumEntrys ++;
        AmbaMisra_TouchUnused(&NumEntrys);

        SvcCalibModuleShellInit = 1U;
    }
}


static void SvcCalibTask_ModShellUsage(UINT32 CtrlFlag)
{
    UINT32 ShellIdx;
    UINT32 ShellCount = (UINT32)(sizeof(CalibModuleShellFunc)) / (UINT32)(sizeof(CalibModuleShellFunc[0]));

    PRN_CAL_MTSK_LOG " " PRN_CAL_MTSK_API
    PRN_CAL_MTSK_LOG "Please type follow to get more information" PRN_CAL_MTSK_API
    PRN_CAL_MTSK_LOG "  > svc_module more" PRN_CAL_MTSK_API
    PRN_CAL_MTSK_LOG "====== %sSVC Calib Module Command Usage%s ======"
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_0 PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MTSK_ARG_POST
    PRN_CAL_MTSK_API
    for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
        if ((CalibModuleShellFunc[ShellIdx].pFunc != NULL) && (CalibModuleShellFunc[ShellIdx].Enable > 0U)) {
            if (CalibModuleShellFunc[ShellIdx].pUsageFunc == NULL) {
                PRN_CAL_MTSK_LOG "  %s"
                    PRN_CAL_MTSK_ARG_CSTR   CalibModuleShellFunc[ShellIdx].CmdName PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_API
            } else {
                (CalibModuleShellFunc[ShellIdx].pUsageFunc)(CtrlFlag);
            }
        }
    }

    PRN_CAL_MTSK_LOG " " PRN_CAL_MTSK_API
}


static void SvcCalibTask_ModShellEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal = SVC_NG;
    UINT32 CtrlFlag = 0U;

    SvcCalibTask_ModShellEntryInit();

    if(ArgCount == 0xCafeU) {
        UINT32 SIdx;

        PRN_CAL_MTSK_LOG " " PRN_CAL_MTSK_API
        PRN_CAL_MTSK_LOG "==== %sShell Arguments Setting%s (%s%d%s) ===="
            PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_NUM     PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_ARG_UINT32 ArgCount               PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_API

        for (SIdx = 0U; SIdx < ArgCount; SIdx ++) {
            PRN_CAL_MTSK_LOG "  pArgVector[%s%d%s] : %s%s%s"
                PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_NUM PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_ARG_UINT32 SIdx               PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_END PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_STR PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_ARG_CSTR   pArgVector[SIdx]   PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_ARG_CSTR   SVC_LOG_CAL_HL_END PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_API
        }
    }

    if (ArgCount < 1U) {
        PRN_CAL_MTSK_LOG "Failure to proc svc storage module shell cmd - invalid arg count!" PRN_CAL_MTSK_NG
    } else if (pArgVector == NULL) {
        PRN_CAL_MTSK_LOG "Failure to proc svc storage module shell cmd - invalid arg vector!" PRN_CAL_MTSK_NG
    } else {
        UINT32 ShellIdx;
        UINT32 ShellCount = (UINT32)(sizeof(CalibModuleShellFunc)) / (UINT32)(sizeof(CalibModuleShellFunc[0]));

        if (0 == SvcWrap_strcmp(pArgVector[1U], "more")) {
            CtrlFlag = 1U;
        } else {

            for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
                if ((CalibModuleShellFunc[ShellIdx].pFunc != NULL) && (CalibModuleShellFunc[ShellIdx].Enable > 0U)) {
                    if (0 == SvcWrap_strcmp(pArgVector[1U], CalibModuleShellFunc[ShellIdx].CmdName)) {
                        if (SVC_OK != (CalibModuleShellFunc[ShellIdx].pFunc)(ArgCount, pArgVector)) {
                            if (CalibModuleShellFunc[ShellIdx].pUsageFunc != NULL) {
                                (CalibModuleShellFunc[ShellIdx].pUsageFunc)(1U);
                            }
                        }
                        RetVal = SVC_OK;
                        break;
                    }
                }
            }
        }
    }

    if (RetVal != SVC_OK) {
        SvcCalibTask_ModShellUsage(CtrlFlag);
        AmbaMisra_TouchUnused(&PrintFunc);
    }
}


static void SvcCalibTask_ModShellInstall(void)
{
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
    UINT32 Idx, SvcModuleOn = 0U;

    for (Idx = 0U; Idx < pCfg->VinNum; Idx ++) {
        if (pCfg->VinCfg[Idx].pEEPROMDriver != NULL) {
            SvcModuleOn = 1U;
            break;
        }
    }

    if (SvcModuleOn > 0U) {
        AMBA_SHELL_COMMAND_s SvcCalModCmdApp;

        SvcCalModCmdApp.pName    = "svc_module";
        SvcCalModCmdApp.MainFunc = SvcCalibTask_ModShellEntry;
        SvcCalModCmdApp.pNext    = NULL;

        if (SHELL_ERR_SUCCESS != SvcCmd_CommandRegister(&SvcCalModCmdApp)) {
            PRN_CAL_MTSK_LOG "Failure to install svc module task command!" PRN_CAL_MTSK_NG
        }
    }
}

/**
 * Query storage module requested memory
 *
 * @param [out] pMemSize Requested memory size
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcCalibTask_ModuleMemQry(UINT32 *pMemSize)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    const SVC_MODULE_CFG_s *pModuleCfg = SvcModuleCfg_Get();

    if (pMemSize == NULL) {
        RetVal = SVC_NG;
    } else if (pResCfg == NULL) {
        RetVal = SVC_NG;
    } else if (pModuleCfg == NULL) {
        RetVal = SVC_NG;
    } else {
        UINT32 VinIdx, UpdIdx;
        UINT32 VinIDs[AMBA_DSP_MAX_VIN_NUM], VinNum = 0U;
        UINT32 FovIDs[AMBA_DSP_MAX_VIEWZONE_NUM], NumFovs = 0U;
        UINT32 NeededSize = 0U, ReqMemSize, MaxUpdMemSize = 0U;
        AMBA_EEPROM_CHANNEL_s Chan;
        AMBA_EEPROM_OBJ_s *pObj = NULL;

        SvcCalibTask_Modulememset(VinIDs, 0, sizeof(VinIDs));
        PRetVal = SvcResCfg_GetVinIDs(VinIDs, &VinNum); PRN_CAL_MTSK_ERR_HDLR

        /* Query calib module control memory */
        for (VinIdx = 0U; VinIdx < VinNum; VinIdx ++) {
            if (pResCfg->VinCfg[VinIDs[VinIdx]].pEEPROMDriver != NULL) {

                ReqMemSize = 0U;
                AmbaMisra_TypeCast(&(pObj), &(pResCfg->VinCfg[VinIDs[VinIdx]].pEEPROMDriver));

                SvcCalibTask_Modulememset(&Chan, 0, sizeof(AMBA_EEPROM_CHANNEL_s));
                Chan.VinId = VinIDs[VinIdx];
                PRetVal = SvcResCfg_GetSensorIDInVinID(VinIDs[VinIdx], &(Chan.ChanId)); PRN_CAL_MTSK_ERR_HDLR

                if (0U == AmbaEEPROM_Hook(&Chan, pObj)) {
                    PRetVal = SvcModule_MemQry(&Chan, pModuleCfg->ItemCfgNum, pModuleCfg->pItemCfg, &ReqMemSize); PRN_CAL_MTSK_ERR_HDLR
                }

                if (ReqMemSize > 0U) {

                    NumFovs = 0U;

                    PRetVal = SvcResCfg_GetFovIdxsInVinID(VinIDs[VinIdx], FovIDs, &NumFovs); PRN_CAL_MTSK_ERR_HDLR

                    NeededSize += ( ReqMemSize * NumFovs );
                }
            }
        }

        /* Query max calculation memory size */
        if (pObj != NULL) {
            for (UpdIdx = 0U; UpdIdx < pModuleCfg->ItemUpdNum; UpdIdx ++) {
                if (pModuleCfg->pItemUpd[UpdIdx].Enable > 0U) {
                    if (MaxUpdMemSize < pModuleCfg->pItemUpd[UpdIdx].ReqMemSize) {
                        MaxUpdMemSize = pModuleCfg->pItemUpd[UpdIdx].ReqMemSize;
                    }
                }
            }

            NeededSize += MaxUpdMemSize;
        }

        *pMemSize = NeededSize;
    }

    return RetVal;
}

/**
 * Initial storage module
 *
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcCalibTask_ModuleInit(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((CalibModuleCtrlFlg & SVC_CALIB_TSK_MODULE_CTRL_INIT) == 0U) {
        const SVC_MODULE_CFG_s *pModuleCfg = SvcModuleCfg_Get();
        const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
        UINT8 *pBuf = NULL;
        UINT32 Idx, BufSize = 0U;
        ULONG BufBase = 0U;

        SvcCalibTask_Modulememset(&CalibModuleCtrl, 0, sizeof(CalibModuleCtrl));

        PRetVal = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_STOR_MODULE, &BufBase, &BufSize);

        AmbaMisra_TypeCast(&(pBuf), &(BufBase));

        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_CAL_MTSK_LOG "Fail to initial calib module - get buffer base/size fail! RetVal: 0x%x"
                PRN_CAL_MTSK_ARG_UINT32 PRetVal PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_NG
        } else if ((pBuf == NULL) || (BufSize == 0U)) {
            RetVal = SVC_NG;
            PRN_CAL_MTSK_LOG "Fail to initial calib module - invalid buffer base(%p) or size(0x%08x)"
                PRN_CAL_MTSK_ARG_CPOINT pBuf    PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_ARG_UINT32 BufSize PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_NG
        } else if (pModuleCfg == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_MTSK_LOG "Fail to initial calib module - calib module cfg should not null!" PRN_CAL_MTSK_NG
        } else if (pResCfg == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_MTSK_LOG "Fail to initial calib module - invalid resolution config!" PRN_CAL_MTSK_NG
        } else {
            UINT32 FovIdx[AMBA_DSP_MAX_VIEWZONE_NUM], NumFov = 0U;
            UINT32 VinID, SerDesIdx;
            UINT32 UpdIdx, MaxUpdMemSize = 0U;

            SvcCalibTask_Modulememset(CalibModuleCtrl.SerDesSelectBits, 0, sizeof(CalibModuleCtrl.SerDesSelectBits));
            SvcCalibTask_Modulememset(FovIdx, 0, sizeof(FovIdx));
            PRetVal = SvcResCfg_GetFovIdxs(FovIdx, &NumFov); PRN_CAL_MTSK_ERR_HDLR

            for (Idx = 0U; Idx < NumFov; Idx ++) {
                VinID = 0xFU;
                SerDesIdx = 0xFFU;
                if (0U != SvcResCfg_GetVinIDOfFovIdx(FovIdx[Idx], &VinID)) {
                    PRN_CAL_MTSK_LOG "Fail to initial calib module - get FovID(%d) VinID fail"
                        PRN_CAL_MTSK_ARG_UINT32 FovIdx[Idx] PRN_CAL_MTSK_ARG_POST
                    PRN_CAL_MTSK_NG
                } else if (VinID >= AMBA_DSP_MAX_VIN_NUM) {
                    PRN_CAL_MTSK_LOG "Fail to initial calib module - invalid VinID(%d) fail"
                        PRN_CAL_MTSK_ARG_UINT32 VinID PRN_CAL_MTSK_ARG_POST
                    PRN_CAL_MTSK_NG
                } else if (0U != SvcResCfg_GetSerdesIdxOfFovIdx(FovIdx[Idx], &SerDesIdx)) {
                    PRN_CAL_MTSK_LOG "Fail to initial calib module - get FovID(%d) SerDesIdx fail"
                        PRN_CAL_MTSK_ARG_UINT32 FovIdx[Idx] PRN_CAL_MTSK_ARG_POST
                    PRN_CAL_MTSK_NG
                } else if (SerDesIdx >= 4U) {
                    PRN_CAL_MTSK_LOG "Fail to initial calib module - FovID(%d) SerDesIdx(%d) is out-of range"
                        PRN_CAL_MTSK_ARG_UINT32 FovIdx[Idx] PRN_CAL_MTSK_ARG_POST
                        PRN_CAL_MTSK_ARG_UINT32 SerDesIdx   PRN_CAL_MTSK_ARG_POST
                    PRN_CAL_MTSK_NG
                } else {
                    if (pResCfg->VinCfg[VinID].SerdesType == SVC_RES_SERDES_TYPE_NONE) {
                        CalibModuleCtrl.SerDesSelectBits[VinID] = 0x1U;
                    } else {
                        CalibModuleCtrl.SerDesSelectBits[VinID] |= (0x1UL << SerDesIdx);
                    }
                }
            }

            for (UpdIdx = 0U; UpdIdx < pModuleCfg->ItemUpdNum; UpdIdx ++) {
                if (pModuleCfg->pItemUpd[UpdIdx].Enable > 0U) {
                    if (MaxUpdMemSize < pModuleCfg->pItemUpd[UpdIdx].ReqMemSize) {
                        MaxUpdMemSize = pModuleCfg->pItemUpd[UpdIdx].ReqMemSize;
                    }
                }
            }

            if (MaxUpdMemSize == 0U) {
                CalibModuleCtrl.MemUpd.Usaged  = 0xCAFEU;
                CalibModuleCtrl.MemUpd.pBuf    = NULL;
                CalibModuleCtrl.MemUpd.BufSize = 0U;
            } else {
                BufSize -= MaxUpdMemSize;
                CalibModuleCtrl.MemUpd.pBuf    = &(pBuf[BufSize]);
                CalibModuleCtrl.MemUpd.BufSize = MaxUpdMemSize;
            }

            for (Idx = 0U; Idx < (SVC_CALIB_TSK_MODULE_MAX_NUM + 1U); Idx ++) {
                CalibModuleCtrl.MemSlot[Idx].Usaged = 0xCAFEU;
            }
            CalibModuleCtrl.MemSlot[SVC_CALIB_TSK_MODULE_MAX_NUM].pBuf    = pBuf;
            CalibModuleCtrl.MemSlot[SVC_CALIB_TSK_MODULE_MAX_NUM].BufSize = BufSize;
            CalibModuleCtrl.pItemCfg   = pModuleCfg->pItemCfg;
            CalibModuleCtrl.ItemCfgNum = pModuleCfg->ItemCfgNum;
            PRN_CAL_MTSK_LOG "Success to initial calib module" PRN_CAL_MTSK_OK

            CalibModuleCtrlFlg |= SVC_CALIB_TSK_MODULE_CTRL_INIT;
        }
    }

    if ((CalibModuleCtrlFlg & SVC_CALIB_TSK_MODULE_SHELL_INIT) == 0U) {
        SvcCalibTask_ModShellInstall();
        CalibModuleCtrlFlg |= SVC_CALIB_TSK_MODULE_SHELL_INIT;
    }

    return RetVal;
}

/**
 * Reset all storage module and release it
 *
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcCalibTask_ModuleReset(void)
{
    UINT32 RetVal = SVC_OK;
    UINT32 FovIdx;

    for (FovIdx = 0U; FovIdx < AMBA_DSP_MAX_VIEWZONE_NUM; FovIdx ++) {
        RetVal |= SvcCalibTask_ModuleDelete(FovIdx);
    }

    SvcCalibTask_Modulememset(&CalibModuleCtrl, 0, sizeof(CalibModuleCtrl));

    CalibModuleCtrlFlg = 0U;

    return RetVal;
}

/**
 * Create storage module task
 *
 * @param [in] FovID fov identification
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcCalibTask_ModuleCreate(UINT32 FovID)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Fail to create calib module - invalid FovID(%d)"
            PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_NG
    } else if ((CalibModuleCtrl.EnaFovFlag & (0x1UL << FovID)) != 0U) {
        PRN_CAL_MTSK_LOG "The FovID(%d) calib module has been created 0x%x"
            PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_API
    } else {
        UINT32 FovSrc = 0xFFU;
        UINT32 VinID = 0xFFFFFFFFU, SensorID = 0xFFFFFFFFU, SensorIdx;

        PRetVal =  SvcResCfg_GetFovSrc(FovID, &FovSrc);
        PRetVal |= SvcResCfg_GetSensorIdxOfFovIdx(FovID, &VinID, &SensorIdx);
        PRetVal |= SvcResCfg_GetSensorIDOfSensorIdx(VinID, SensorIdx, &SensorID);

        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_CAL_MTSK_LOG "Fail to create FovID(%d) calib module - get FOV/SensorID fail"
                PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_NG
        } else if ((FovSrc != SVC_VIN_SRC_SENSOR) && (FovSrc != SVC_VIN_SRC_YUV)) {
            RetVal = SVC_NG;
            PRN_CAL_MTSK_LOG "Fail to create FovID(%d) calib module - not supported fov source(%d)"
                PRN_CAL_MTSK_ARG_UINT32 FovID  PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_ARG_UINT32 FovSrc PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_NG
        } else {
            AMBA_EEPROM_CHANNEL_s ModuleChan;
            UINT8                *pModuleMem = NULL;
            UINT32                ModuleMemSize = 0U;
            void                 *pModuleCtrl = NULL;

            SvcCalibTask_Modulememset(&ModuleChan, 0, sizeof(ModuleChan));
            ModuleChan.VinId  = VinID;
            ModuleChan.ChanId = SensorID;

            RetVal = SvcCalibTask_ModuleMemCfg(FovID, &ModuleChan, CalibModuleCtrl.ItemCfgNum, CalibModuleCtrl.pItemCfg, &pModuleMem, &ModuleMemSize);
            if (RetVal != SVC_OK) {
                PRN_CAL_MTSK_LOG "Fail to create FovID(%d) calib module - cfg memory fail!"
                    PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
                PRN_CAL_MTSK_NG
            } else {

                PRetVal = SvcModule_Create(&ModuleChan, pModuleMem, ModuleMemSize, &(pModuleCtrl));
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    PRN_CAL_MTSK_LOG "Fail to create FovID(%d) calib module - create svc module fail!"
                        PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
                    PRN_CAL_MTSK_NG
                } else {
                    if (pModuleCtrl == NULL) {
                        RetVal = SVC_NG;
                        PRN_CAL_MTSK_LOG "Fail to create FovID(%d) calib module - invalid module control point!"
                            PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
                        PRN_CAL_MTSK_NG
                    }
                }

                if (RetVal == SVC_OK) {
                    PRetVal = SvcModule_Config(pModuleCtrl, CalibModuleCtrl.ItemCfgNum, CalibModuleCtrl.pItemCfg);
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;
                        PRN_CAL_MTSK_LOG "Fail to create FovID(%d) calib module - config module control fail!"
                            PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
                        PRN_CAL_MTSK_NG
                    }
                }

                if (RetVal == SVC_OK) {
                    CalibModuleCtrl.pModuleCtrl[FovID] = pModuleCtrl;
                    CalibModuleCtrl.EnaFovFlag |= (0x1UL << FovID);
                    PRN_CAL_MTSK_LOG "Success to create FovID(%d) calib module"
                        PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
                    PRN_CAL_MTSK_OK

                    SvcCalibTask_ModuleSerDesQryPause(ModuleChan.VinId);
                    PRetVal = SvcModule_Load(CalibModuleCtrl.pModuleCtrl[FovID]);
                    SvcCalibTask_ModuleSerDesQryResume(ModuleChan.VinId);
                    if (PRetVal != 0U) {
#ifdef SVC_CALIB_DBG_MSG_ON
                        PRN_CAL_MTSK_LOG "Fail to load FovID(%d) calib module data!"
                            PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
                        PRN_CAL_MTSK_NG
#endif
                    }

                }
            }
        }
    }

    return RetVal;
}

/**
 * Delete storage module task
 *
 * @param [in] FovID fov identification
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcCalibTask_ModuleDelete(UINT32 FovID)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Fail to delete FovID(%d) calib module!"
            PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_NG
    } else {
        UINT32 SlotIdx;

        if ((CalibModuleCtrl.pModuleCtrl[FovID] != NULL) &&
            ((CalibModuleCtrl.EnaFovFlag & (0x1UL << FovID)) > 0U)) {

            PRetVal = SvcModule_Delete(CalibModuleCtrl.pModuleCtrl[FovID]); PRN_CAL_MTSK_ERR_HDLR
            CalibModuleCtrl.pModuleCtrl[FovID] = NULL;

            for (SlotIdx = 0U; SlotIdx < SVC_CALIB_TSK_MODULE_MAX_NUM; SlotIdx ++ ) {
                if (CalibModuleCtrl.MemSlot[SlotIdx].Usaged == FovID) {
                    CalibModuleCtrl.MemSlot[SlotIdx].Usaged = 0xCAFEU;
                }
            }

            CalibModuleCtrl.EnaFovFlag &= ~(0x1UL << FovID);

            PRN_CAL_MTSK_LOG "Success to delete FovID(%d) calib module"
                PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
            PRN_CAL_MTSK_OK
        }
    }

    return RetVal;
}

/**
 * Update the storage module data to calib
 *
 * @param [in] FovID Fov identification
 * @param [in] pChan Calib channel
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcCalibTask_ModuleUpdate(UINT32 FovID, SVC_CALIB_CHANNEL_s *pChan)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    const SVC_MODULE_CFG_s* pModuleCfg = SvcModuleCfg_Get();
    const SVC_CALIB_CFG_s* pCalCfg = SvcCalibCfg_Get();

    if (FovID >= AMBA_DSP_MAX_VIEWZONE_NUM) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Fail to update FovID(%d) calib module"
            PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_NG
    } else if (pChan == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Fail to update FovID(%d) calib module - invalid calib chan"
            PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_NG
    } else if (pCalCfg == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Fail to update FovID(%d) calib module - invalid calib cfg"
            PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_NG
    } else if (pModuleCfg == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MTSK_LOG "Fail to update FovID(%d) calib module - invalid calib module cfg"
            PRN_CAL_MTSK_ARG_UINT32 FovID PRN_CAL_MTSK_ARG_POST
        PRN_CAL_MTSK_NG
    } else if ((CalibModuleCtrlFlg & SVC_CALIB_TSK_MODULE_UPD_OFF) > 0U) {
        PRN_CAL_MTSK_LOG "Calib module update has been disable" PRN_CAL_MTSK_API
    } else {
        void *pModuleCtrl = CalibModuleCtrl.pModuleCtrl[FovID];

        if ((pModuleCtrl != NULL) && (pCalCfg->pCalObj != NULL) && (pModuleCfg->pItemUpd != NULL)) {
            UINT32 CalIdx, ItemUpdIdx;

            for (ItemUpdIdx = 0U; ItemUpdIdx < pModuleCfg->ItemUpdNum; ItemUpdIdx ++) {

                if (pModuleCfg->pItemUpd[ItemUpdIdx].Enable > 0U) {

                    for (CalIdx = 0U; CalIdx < pCalCfg->NumOfCalObj; CalIdx ++) {

                        if ((pCalCfg->pCalObj[CalIdx].ID == pModuleCfg->pItemUpd[ItemUpdIdx].CalibID)
                            && (pCalCfg->pCalObj[CalIdx].Enable > 0U)) {

                            switch (pModuleCfg->pItemUpd[ItemUpdIdx].ItemVersion) {
                            case SVC_MODULE_ITEM_BPC :
                                PRetVal = SvcCalibTask_ModuleBpcUpd(pModuleCtrl, pChan); PRN_CAL_MTSK_ERR_HDLR
                                break;

                            default :
                                // Do nothing
                                break;
                            }

                        }
                    }
                }
            }
        }
    }

    return RetVal;
}

/**
 * On/Off calib module auto-update mechanism
 *
 * @param [in] On On/Off
 *
 */
void SvcCalibTask_ModuleUpdSwitch(UINT32 On)
{
    if (On == 0U) {
        CalibModuleCtrlFlg |= SVC_CALIB_TSK_MODULE_UPD_OFF;
        PRN_CAL_MTSK_LOG "Calib module auto-update disable" PRN_CAL_MTSK_API
    } else {
        CalibModuleCtrlFlg &= ~SVC_CALIB_TSK_MODULE_UPD_OFF;
        PRN_CAL_MTSK_LOG "Calib module auto-update enable" PRN_CAL_MTSK_API
    }
}
