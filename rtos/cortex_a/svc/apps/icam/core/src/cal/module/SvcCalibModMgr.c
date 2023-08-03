/**
*  @file SvcCalibModMgr.c
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
*  @details C file for SVC Module Storage Manager
*
*/

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaShell.h"
#include "AmbaVIN.h"
#include "AmbaEEPROM.h"

#include "AmbaCameraModule.h"

#include "SvcErrCode.h"
#include "SvcWrap.h"
#include "SvcLog.h"
#include "SvcCmd.h"
#include "SvcCalibMgr.h"
#include "SvcCalibModMgr.h"

typedef struct {
#define SVC_MODULE_ITEM_INIT    (0x1U)
    UINT32                State;
    UINT32                ItemVersion;
    UINT32                ModuleVersion;
    UINT32                MinorVersion;
    UINT32                ProjectID;
    UINT8                *pDataBuf;
    UINT32                DataBufSize;
    UINT32                DataBufUsaged;
    SVC_MODULE_CMD_FUNC_f pFunc;
} SVC_MODULE_ITEM_INFO_s;

typedef struct {
    char                    Name[32];
    AMBA_KAL_MUTEX_t        Mutx;
#define SVC_MODULE_CTRL_INIT        (0x1U)
#define SVC_MODULE_CTRL_EEPROM_RDY  (0x2U)
    UINT32                  State;
    AMBA_EEPROM_CHANNEL_s   EEPROMChan;

    UINT8                  *pMemBuf;
    UINT32                  MemSize;
    UINT8                  *pCtrlBuf;
    UINT32                  CtrlBufSize;
    UINT8                  *pItemBuf;
    UINT32                  ItemBufSize;
    UINT8                  *pShadowBuf;
    UINT32                  ShadowBufSize;
    UINT32                  ShadowRemainSize;
    UINT8                  *pWorkBuf;
    UINT32                  WorkBufSize;

    UINT32                  ModuleID;
    UINT32                  ModuleVersion;
    UINT32                  MinorVersion;
    UINT32                  NumOfItem;
    SVC_MODULE_ITEM_INFO_s *pItem;

    UINT32                  NumOfItemCfg;
    SVC_MODULE_ITEM_CFG_s   ItemCfg[SVC_MODULE_ITEM_MAX_NUM];

    void                   *pNextCtrl;
} SVC_MODULE_CTRL_s;

static UINT32 SvcModule_Align(UINT32 Align, UINT32 Size);
static UINT32 SvcModule_MemQuery(const AMBA_EEPROM_CHANNEL_s *pChan, UINT32 *pEEPROMMemSize, UINT32 *pCtrlMemSize, UINT32 *pItemMemSize);
static UINT32 SvcModule_ItemMemQuery(UINT32 NumOfCfg, SVC_MODULE_ITEM_CFG_s *pCfg, UINT32 *pItemWorkMemSize);

static UINT32 SvcModule_LoadData(const AMBA_EEPROM_CHANNEL_s *pChan, UINT32 Offset, UINT8 *pBuf, UINT32 Size);
static UINT32 SvcModule_SaveData(const AMBA_EEPROM_CHANNEL_s *pChan, UINT32 Offset, UINT8 *pBuf, UINT32 Size);

static SVC_MODULE_CTRL_s *pSvcModuleCtrlRoot = NULL;
static UINT32             SvcModuleCtrlFlag = SVC_LOG_CAL_DEF_FLG;

#define SVC_LOG_MODULE "STGMOD"
#define PRN_MOD_MGR_LOG        { SVC_WRAP_PRINT_s CalibModMgrPrint; SvcModule_memset(&(CalibModMgrPrint), 0, sizeof(CalibModMgrPrint)); CalibModMgrPrint.Argc --; CalibModMgrPrint.pStrFmt =
#define PRN_MOD_MGR_ARG_UINT64 ; CalibModMgrPrint.Argc ++; CalibModMgrPrint.Argv[CalibModMgrPrint.Argc].Uint64   =
#define PRN_MOD_MGR_ARG_UINT32 ; CalibModMgrPrint.Argc ++; CalibModMgrPrint.Argv[CalibModMgrPrint.Argc].Uint64   = (UINT64)((
#define PRN_MOD_MGR_ARG_CSTR   ; CalibModMgrPrint.Argc ++; CalibModMgrPrint.Argv[CalibModMgrPrint.Argc].pCStr    = ((
#define PRN_MOD_MGR_ARG_CPOINT ; CalibModMgrPrint.Argc ++; CalibModMgrPrint.Argv[CalibModMgrPrint.Argc].pPointer = ((
#define PRN_MOD_MGR_ARG_POST   ))
#define PRN_MOD_MGR_OK         ; CalibModMgrPrint.Argc ++; SvcModule_PrintLog(SVC_LOG_CAL_OK , &(CalibModMgrPrint)); }
#define PRN_MOD_MGR_NG         ; CalibModMgrPrint.Argc ++; SvcModule_PrintLog(SVC_LOG_CAL_NG , &(CalibModMgrPrint)); }
#define PRN_MOD_MGR_API        ; CalibModMgrPrint.Argc ++; SvcModule_PrintLog(SVC_LOG_CAL_API, &(CalibModMgrPrint)); }
#define PRN_MOD_MGR_DBG        ; CalibModMgrPrint.Argc ++; SvcModule_PrintLog(SVC_LOG_CAL_DBG, &(CalibModMgrPrint)); }

static void SvcModule_PrintLog(UINT32 LogLevel, SVC_WRAP_PRINT_s *pPrint)
{
    if (pPrint != NULL) {
        if ((SvcModuleCtrlFlag & LogLevel) > 0U) {

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

            SvcWrap_Print(SVC_LOG_MODULE, pPrint);
        }
    }
}

static void SvcModule_memset(void *ptr, INT32 v, SIZE_t n)
{
    UINT32 ErrCode = AmbaWrap_memset(ptr, v, n);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }
}

static void SvcModule_memcpy(void *pDst, const void *pSrc, SIZE_t num)
{
    UINT32 ErrCode = AmbaWrap_memcpy(pDst, pSrc, num);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }
}

static UINT32 SvcModule_MutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut)
{
    return AmbaKAL_MutexTake(pMutex, TimeOut);
}

static void SvcModule_MutexGive(AMBA_KAL_MUTEX_t *pMutex)
{
    UINT32 ErrCode = AmbaKAL_MutexGive(pMutex);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }
}

static UINT32 SvcModule_Align(UINT32 Align, UINT32 Size)
{
    UINT32 RetVal;

    if (Align > 1U) {
        if ((0xFFFFFFFFU - Size) < Align) {
            RetVal = Size;
        } else {
            RetVal = ( ( (UINT32)(Size) + (UINT32)(Align - 1U) ) & ( ~( (UINT32)(Align - 1U) ) ) );
        }
    } else {
        RetVal = Size;
    }

    return RetVal;
}

static UINT32 SvcModule_MemAddrDiff(const UINT8 *pVal0, const UINT8 *pVal1)
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

static UINT32 SvcModule_MemQuery(const AMBA_EEPROM_CHANNEL_s *pChan, UINT32 *pEEPROMMemSize, UINT32 *pCtrlMemSize, UINT32 *pItemMemSize)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (pCtrlMemSize != NULL) {
        *pCtrlMemSize = SvcModule_Align(64U, (UINT32) sizeof(SVC_MODULE_CTRL_s));
    }

    if (pItemMemSize != NULL) {
        *pItemMemSize = SvcModule_Align(64U, (UINT32) sizeof(SVC_MODULE_ITEM_INFO_s));
    }

    if (pEEPROMMemSize != NULL) {
        if (pChan == NULL) {
            RetVal = SVC_NG;
            PRN_MOD_MGR_LOG "Fail to query module memory - invalid EEPROM channel!" PRN_MOD_MGR_NG
        } else if (pChan->VinId >= AMBA_NUM_VIN_CHANNEL) {
            RetVal = SVC_NG;
            PRN_MOD_MGR_LOG "Fail to query module memory - invalid VinID(%d)"
                PRN_MOD_MGR_ARG_UINT32 pChan->VinId PRN_MOD_MGR_ARG_POST
            PRN_MOD_MGR_NG
        } else {
            AMBA_EEPROM_DEVICE_INFO_s DevInfo;

            SvcModule_memset(&DevInfo, 0, sizeof(DevInfo));
            PRetVal = AmbaEEPROM_GetDeviceInfo(pChan, &DevInfo);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_MOD_MGR_LOG "Fail to query storage module memory - get eeprom device info fail. VinID(%d), RetVal: 0x%x"
                    PRN_MOD_MGR_ARG_UINT32 pChan->VinId PRN_MOD_MGR_ARG_POST
                    PRN_MOD_MGR_ARG_UINT32 PRetVal      PRN_MOD_MGR_ARG_POST
                PRN_MOD_MGR_NG
            } else {
                if (DevInfo.MaxCapacity == 0U) {
                    RetVal = SVC_NG;
                    PRN_MOD_MGR_LOG "Fail to query storage module memory - get eeprom size should not zero. VinID(%d)"
                        PRN_MOD_MGR_ARG_UINT32 pChan->VinId PRN_MOD_MGR_ARG_POST
                    PRN_MOD_MGR_NG
                }
            }

            if (RetVal == SVC_OK) {
                *pEEPROMMemSize = DevInfo.MaxCapacity;
            }
        }
    }

    return RetVal;
}


static UINT32 SvcModule_ItemMemQuery(UINT32 NumOfCfg, SVC_MODULE_ITEM_CFG_s *pCfg, UINT32 *pItemWorkMemSize)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (pItemWorkMemSize == NULL) {
        RetVal = SVC_NG;
        PRN_MOD_MGR_LOG "Fail to query item working memory - output item working memory should not null!" PRN_MOD_MGR_NG
    } else {
        UINT32 CfgIdx, CurCfgMemSize;

        PRN_MOD_MGR_LOG " " PRN_MOD_MGR_DBG
        PRN_MOD_MGR_LOG "---- Svc Module Item Working Memory ----" PRN_MOD_MGR_DBG
        PRN_MOD_MGR_LOG "  NumOfCfg : %u"
            PRN_MOD_MGR_ARG_UINT32 NumOfCfg PRN_MOD_MGR_ARG_POST
        PRN_MOD_MGR_DBG
        PRN_MOD_MGR_LOG "      pCfg : %p"
            PRN_MOD_MGR_ARG_CPOINT pCfg PRN_MOD_MGR_ARG_POST
        PRN_MOD_MGR_DBG

        *pItemWorkMemSize = 0U;

        if ((NumOfCfg > 0U) && (pCfg != NULL)) {

            PRN_MOD_MGR_LOG "             Idx | Version | Working Memory Size" PRN_MOD_MGR_DBG

            for (CfgIdx = 0U; CfgIdx < NumOfCfg; CfgIdx ++) {
                CurCfgMemSize = 0U;
                if (pCfg[CfgIdx].pFunc != NULL) {
                    PRetVal = (pCfg[CfgIdx].pFunc)(SVC_MODULE_ITEM_CMD_MEM_QRY, &CurCfgMemSize, NULL, NULL, NULL, NULL);
                    if (PRetVal != 0U) {
                        PRN_MOD_MGR_LOG "              %02d |  0x%02x   | failure"
                            PRN_MOD_MGR_ARG_UINT32 CfgIdx                   PRN_MOD_MGR_ARG_POST
                            PRN_MOD_MGR_ARG_UINT32 pCfg[CfgIdx].ItemVersion PRN_MOD_MGR_ARG_POST
                        PRN_MOD_MGR_DBG
                    }
                } else {
                    PRN_MOD_MGR_LOG "              %02d |  0x%02x   | empty"
                        PRN_MOD_MGR_ARG_UINT32 CfgIdx                   PRN_MOD_MGR_ARG_POST
                        PRN_MOD_MGR_ARG_UINT32 pCfg[CfgIdx].ItemVersion PRN_MOD_MGR_ARG_POST
                    PRN_MOD_MGR_DBG
                }

                if (CurCfgMemSize > (*pItemWorkMemSize)) {
                    *pItemWorkMemSize = CurCfgMemSize;
                }
                PRN_MOD_MGR_LOG "              %02d |  0x%02x   | 0x%08x"
                    PRN_MOD_MGR_ARG_UINT32 CfgIdx                   PRN_MOD_MGR_ARG_POST
                    PRN_MOD_MGR_ARG_UINT32 pCfg[CfgIdx].ItemVersion PRN_MOD_MGR_ARG_POST
                    PRN_MOD_MGR_ARG_UINT32 CurCfgMemSize            PRN_MOD_MGR_ARG_POST
                PRN_MOD_MGR_DBG
            }

        }

        PRN_MOD_MGR_LOG "  Work Mem : 0x%08x"
            PRN_MOD_MGR_ARG_UINT32 (*pItemWorkMemSize) PRN_MOD_MGR_ARG_POST
        PRN_MOD_MGR_DBG
    }

    AmbaMisra_TouchUnused(pCfg);

    return RetVal;
}


static UINT32 SvcModule_LoadData(const AMBA_EEPROM_CHANNEL_s *pChan, UINT32 Offset, UINT8 *pBuf, UINT32 Size)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (pChan == NULL) {
        RetVal = SVC_NG;
    } else if (pBuf == NULL) {
        RetVal = SVC_NG;
    } else {
        AMBA_EEPROM_CMD_CFG_s CmdCfg;

        SvcModule_memset(&CmdCfg, 0, sizeof(CmdCfg));
        CmdCfg.Offset = Offset;
        CmdCfg.DataSize = Size;
        AmbaMisra_TypeCast(&(CmdCfg.pDataBuf), &(pBuf));
        PRetVal = AmbaEEPROM_Read(pChan, &CmdCfg);
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
        }
    }

    AmbaMisra_TouchUnused(pBuf);

    return RetVal;
}


static UINT32 SvcModule_SaveData(const AMBA_EEPROM_CHANNEL_s *pChan, UINT32 Offset, UINT8 *pBuf, UINT32 Size)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (pChan == NULL) {
        RetVal = SVC_NG;
    } else if (pBuf == NULL) {
        RetVal = SVC_NG;
    } else {
        AMBA_EEPROM_CMD_CFG_s CmdCfg;

        SvcModule_memset(&CmdCfg, 0, sizeof(CmdCfg));
        CmdCfg.Offset = Offset;
        CmdCfg.DataSize = Size;
        AmbaMisra_TypeCast(&(CmdCfg.pDataBuf), &(pBuf));
        PRetVal = AmbaEEPROM_Write(pChan, &CmdCfg);
        if (PRetVal != 0U) {
            RetVal = SVC_NG;

            SvcModule_InfoAll();
        }

        AmbaMisra_TouchUnused(pBuf);
    }

    return RetVal;
}

/**
 * Query storage module requested memory size
 *
 * @param [in] pChan EEPROM channel
 * @param [in] NumOfCfg number of item config
 * @param [in] pCfg storage module item config
 * @param [out] pMemSize requested memory size
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcModule_MemQry(const AMBA_EEPROM_CHANNEL_s *pChan, UINT32 NumOfCfg, SVC_MODULE_ITEM_CFG_s *pCfg, UINT32 *pMemSize)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (pChan == NULL) {
        RetVal = SVC_NG;
        PRN_MOD_MGR_LOG "Fail to query storage module memory - invalid EEPROM channel!" PRN_MOD_MGR_NG
    } else if (pChan->VinId >= AMBA_NUM_VIN_CHANNEL) {
        RetVal = SVC_NG;
        PRN_MOD_MGR_LOG "Fail to query storage module memory - invalid VinID(%d)"
            PRN_MOD_MGR_ARG_UINT32 pChan->VinId PRN_MOD_MGR_ARG_POST
        PRN_MOD_MGR_NG
    } else if ((NumOfCfg > 0U) && (pCfg == NULL)) {
        RetVal = SVC_NG;
        PRN_MOD_MGR_LOG "Fail to query storage module memory - invalid cfg! NumOfCfg(%d)"
            PRN_MOD_MGR_ARG_UINT32 NumOfCfg PRN_MOD_MGR_ARG_POST
        PRN_MOD_MGR_NG
    } else if (pMemSize == NULL) {
        RetVal = SVC_NG;
        PRN_MOD_MGR_LOG "Fail to query storage module memory - output memory size should not null!" PRN_MOD_MGR_NG
    } else {
        UINT32 ModuleCtrlSize = 0U;
        UINT32 ModuleItemSize = 0U;
        UINT32 ShadowMemSize  = 0U;
        UINT32 WorkMemSize;

        WorkMemSize  = (UINT32) sizeof(Modual_Cal_Param_s);                                // Module header
        WorkMemSize += ( (UINT32) sizeof(Modual_Cal_Header_s) * SVC_MODULE_ITEM_MAX_NUM ); // Item headers
        WorkMemSize += 4U;                                                                 // Header checksum

        RetVal = SvcModule_MemQuery(pChan, &ShadowMemSize, &ModuleCtrlSize, &ModuleItemSize);
        RetVal |= SvcModule_ItemMemQuery(NumOfCfg, pCfg, &PRetVal);
        if (PRetVal > WorkMemSize) {
            WorkMemSize = PRetVal;
        }

        *pMemSize  = ModuleCtrlSize + ShadowMemSize;
        *pMemSize += ( ModuleItemSize * SVC_MODULE_ITEM_MAX_NUM);
        *pMemSize += WorkMemSize;

        PRN_MOD_MGR_LOG " " PRN_MOD_MGR_DBG
        PRN_MOD_MGR_LOG "---- SVC Storage Module Memory Info ----" PRN_MOD_MGR_DBG
        PRN_MOD_MGR_LOG "         EEPROM VinID : %u"
            PRN_MOD_MGR_ARG_UINT32 pChan->VinId PRN_MOD_MGR_ARG_POST
        PRN_MOD_MGR_DBG
        PRN_MOD_MGR_LOG "        EEPROM ChanID : 0x%x"
            PRN_MOD_MGR_ARG_UINT32 pChan->ChanId PRN_MOD_MGR_ARG_POST
        PRN_MOD_MGR_DBG
        PRN_MOD_MGR_LOG "  Module Control Size : 0x%x"
            PRN_MOD_MGR_ARG_UINT32 ModuleCtrlSize PRN_MOD_MGR_ARG_POST
        PRN_MOD_MGR_DBG
        PRN_MOD_MGR_LOG "   Shadow Buffer Size : 0x%x"
            PRN_MOD_MGR_ARG_UINT32 ShadowMemSize PRN_MOD_MGR_ARG_POST
        PRN_MOD_MGR_DBG
        PRN_MOD_MGR_LOG "     Module Item Size : 0x%x x %d"
            PRN_MOD_MGR_ARG_UINT32 ModuleItemSize          PRN_MOD_MGR_ARG_POST
            PRN_MOD_MGR_ARG_UINT32 SVC_MODULE_ITEM_MAX_NUM PRN_MOD_MGR_ARG_POST
        PRN_MOD_MGR_DBG
        PRN_MOD_MGR_LOG "         Working Size : 0x%x"
            PRN_MOD_MGR_ARG_UINT32 WorkMemSize PRN_MOD_MGR_ARG_POST
        PRN_MOD_MGR_DBG
        PRN_MOD_MGR_LOG "                Total : 0x%x"
            PRN_MOD_MGR_ARG_UINT32 *pMemSize PRN_MOD_MGR_ARG_POST
        PRN_MOD_MGR_DBG
    }

    return RetVal;
}

/**
 * Create storage module controller
 *
 * @param [in] pChan EEPROM channel
 * @param [in] MemBase storage module requested memory address
 * @param [in] MemSize storage module requested memory size
 * @param [out] pModuleCtrl storage module ctrl
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcModule_Create(const AMBA_EEPROM_CHANNEL_s *pChan, UINT8 *pMemBuf, UINT32 MemSize, void **pModuleCtrl)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    SVC_MODULE_CTRL_s *pCtrl = NULL;

    AmbaMisra_TypeCast(&(pCtrl), &(pMemBuf));

    if (pChan == NULL) {
        RetVal = SVC_NG;
        PRN_MOD_MGR_LOG "Fail to create storage module - invalid EEPROM channel!" PRN_MOD_MGR_NG
    } else if (pChan->VinId >= AMBA_NUM_VIN_CHANNEL) {
        RetVal = SVC_NG;
        PRN_MOD_MGR_LOG "Fail to create storage module - invalid VinID(%d)"
            PRN_MOD_MGR_ARG_UINT32 pChan->VinId PRN_MOD_MGR_ARG_POST
        PRN_MOD_MGR_NG
    } else if ((pCtrl == NULL) || (MemSize == 0U)) {
        RetVal = SVC_NG;
        PRN_MOD_MGR_LOG "Fail to create storage module - invlid memory setting! %p, 0x%x"
            PRN_MOD_MGR_ARG_CPOINT pCtrl   PRN_MOD_MGR_ARG_POST
            PRN_MOD_MGR_ARG_UINT32 MemSize PRN_MOD_MGR_ARG_POST
        PRN_MOD_MGR_NG
    } else if (pModuleCtrl == NULL) {
        RetVal = SVC_NG;
        PRN_MOD_MGR_LOG "Fail to create storage module - output module control should not null!" PRN_MOD_MGR_NG
    } else {

        SvcModule_memset(pCtrl, 0, MemSize);

        PRetVal = SvcModule_MemQuery(pChan, &(pCtrl->ShadowBufSize), &(pCtrl->CtrlBufSize), &(pCtrl->ItemBufSize));
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_MOD_MGR_LOG "Fail to create storage module - query memory fail. VinId(%d), ChanId"
                PRN_MOD_MGR_ARG_UINT32 pChan->VinId  PRN_MOD_MGR_ARG_POST
                PRN_MOD_MGR_ARG_UINT32 pChan->ChanId PRN_MOD_MGR_ARG_POST
            PRN_MOD_MGR_NG
        } else {
            UINT32 ReqMemSize = pCtrl->ShadowBufSize + pCtrl->CtrlBufSize;

            pCtrl->ItemBufSize *= SVC_MODULE_ITEM_MAX_NUM;

            ReqMemSize += pCtrl->ItemBufSize;

            if (ReqMemSize > MemSize) {
                RetVal = SVC_NG;
                PRN_MOD_MGR_LOG "Fail to create storage module - input memory size does not enough to service it!" PRN_MOD_MGR_NG
            }
        }

        if (RetVal == SVC_OK) {
            SvcModule_memcpy(&(pCtrl->EEPROMChan), pChan, sizeof(AMBA_EEPROM_CHANNEL_s));
            pCtrl->pMemBuf          = pMemBuf;
            pCtrl->MemSize          = MemSize;
            pCtrl->pCtrlBuf         = pCtrl->pMemBuf;
            pCtrl->pItemBuf         = &(pCtrl->pCtrlBuf[pCtrl->CtrlBufSize]);
            pCtrl->pShadowBuf       = &(pCtrl->pItemBuf[pCtrl->ItemBufSize]);
            pCtrl->ShadowRemainSize = pCtrl->ShadowBufSize;
            pCtrl->pWorkBuf         = &(pCtrl->pShadowBuf[pCtrl->ShadowBufSize]);
            pCtrl->WorkBufSize      = pCtrl->MemSize - SvcModule_MemAddrDiff(pCtrl->pWorkBuf, pCtrl->pMemBuf);
            pCtrl->ModuleID         = 0x20190624U;
            pCtrl->ModuleVersion    = SVC_MODULE_VERSION;
            pCtrl->MinorVersion     = SVC_MODULE_MINOR_VERSION;
            pCtrl->NumOfItem        = 0U;
            AmbaMisra_TypeCast(&(pCtrl->pItem), &(pCtrl->pItemBuf));

            SVC_WRAP_SNPRINT "SvcModule_%02d_0x%x"
                SVC_SNPRN_ARG_S pCtrl->Name
                SVC_SNPRN_ARG_UINT32 pCtrl->EEPROMChan.VinId  SVC_SNPRN_ARG_POST
                SVC_SNPRN_ARG_UINT32 pCtrl->EEPROMChan.ChanId SVC_SNPRN_ARG_POST
                SVC_SNPRN_ARG_BSIZE  32
                SVC_SNPRN_ARG_RLEN   &PRetVal
                SVC_SNPRN_ARG_E

            PRetVal = AmbaKAL_MutexCreate(&(pCtrl->Mutx), pCtrl->Name);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_MOD_MGR_LOG "Fail to create storage module - create mutex fail! RetVal: 0x%x"
                    PRN_MOD_MGR_ARG_UINT32 PRetVal PRN_MOD_MGR_ARG_POST
                PRN_MOD_MGR_NG
            } else {
                pCtrl->State |= SVC_MODULE_CTRL_INIT;

                *pModuleCtrl = pCtrl;
                PRN_MOD_MGR_LOG "Success to create storage module" PRN_MOD_MGR_OK

                // Update ctrl link list
                if (pSvcModuleCtrlRoot == NULL) {
                    pSvcModuleCtrlRoot = pCtrl;
                } else {
                    SVC_MODULE_CTRL_s *pCurCtrl = pSvcModuleCtrlRoot;

                    while (pCurCtrl->pNextCtrl != NULL) {
                        AmbaMisra_TypeCast(&(pCurCtrl), &(pCurCtrl->pNextCtrl));
                    }

                    pCurCtrl->pNextCtrl = pCtrl;
                }
            }
        }
    }

    return RetVal;
}

/**
 * Delete storage module controller
 *
 * @param [in] pModuleCtrl storage module ctrl
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcModule_Delete(void *pModuleCtrl)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    SVC_MODULE_CTRL_s *pCtrl = NULL;

    AmbaMisra_TypeCast(&(pCtrl), &(pModuleCtrl));

    if (pCtrl == NULL) {
        RetVal = SVC_NG;
        PRN_MOD_MGR_LOG "Fail to delete storage module - invalid module control!" PRN_MOD_MGR_NG
    } else {
        PRetVal = SvcModule_MutexTake(&(pCtrl->Mutx), AMBA_KAL_WAIT_FOREVER);
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_MOD_MGR_LOG "Fail to delete storage module - take mutex fail! RetVal: 0x%x"
                PRN_MOD_MGR_ARG_UINT32 PRetVal PRN_MOD_MGR_ARG_POST
            PRN_MOD_MGR_NG
        } else {
            SVC_MODULE_CTRL_s *pCurCtrl = pSvcModuleCtrlRoot, *pPreCtrl = NULL;

            // Update the ctrl link list
            while (pCurCtrl != NULL) {
                if (pCurCtrl == pCtrl) {
                    if (pPreCtrl != NULL) {
                        pPreCtrl->pNextCtrl = pCtrl->pNextCtrl;
                    } else {
                        AmbaMisra_TypeCast(&(pSvcModuleCtrlRoot), &(pCtrl->pNextCtrl));
                    }
                    break;
                }
                pPreCtrl = pCurCtrl;
                AmbaMisra_TypeCast(&(pCurCtrl), &(pCurCtrl->pNextCtrl));
            }

            // Delete ctrl module
            SvcModule_MutexGive(&(pCtrl->Mutx));
            PRetVal = AmbaKAL_MutexDelete(&(pCtrl->Mutx));
            if (PRetVal != 0U) {
                PRN_MOD_MGR_LOG "Fail to delete storage module - delete mutex fail! RetVal: 0x%x"
                    PRN_MOD_MGR_ARG_UINT32 PRetVal PRN_MOD_MGR_ARG_POST
                PRN_MOD_MGR_NG
            }

            SvcModule_memset(pCtrl, 0, sizeof(SVC_MODULE_CTRL_s));
        }
    }

    AmbaMisra_TouchUnused(pModuleCtrl);

    return RetVal;
}

/**
 * Config storage module setting
 *
 * @param [in] pModuleCtrl storage module control
 * @param [in] NumOfCfg Configuration number
 * @param [in] pCfg Configuration array
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcModule_Config(void *pModuleCtrl, UINT32 NumOfCfg, const SVC_MODULE_ITEM_CFG_s *pCfg)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    SVC_MODULE_CTRL_s *pCtrl;
    AmbaMisra_TypeCast(&(pCtrl), &(pModuleCtrl));

    if (pCtrl == NULL) {
        RetVal = SVC_NG;
        PRN_MOD_MGR_LOG "Fail to load data from module - invalid control!" PRN_MOD_MGR_NG
    } else if ((pCtrl->State & SVC_MODULE_CTRL_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_MOD_MGR_LOG "Fail to load data from module - initial module first!" PRN_MOD_MGR_NG
    } else {
        if ((NumOfCfg > 0U) && (pCfg != NULL)) {
            PRetVal = SvcModule_MutexTake(&(pCtrl->Mutx), AMBA_KAL_WAIT_FOREVER);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_MOD_MGR_LOG "Fail to load data from module - take mutex fail! RetVal: 0x%x"
                    PRN_MOD_MGR_ARG_UINT32 PRetVal PRN_MOD_MGR_ARG_POST
                PRN_MOD_MGR_NG
            } else {
                UINT32 CfgIdx;

                for (CfgIdx = 0U; CfgIdx < NumOfCfg; CfgIdx ++) {
                    if (pCfg[CfgIdx].pFunc != NULL) {
                        pCtrl->ItemCfg[CfgIdx].ItemVersion = pCfg[CfgIdx].ItemVersion;
                        pCtrl->ItemCfg[CfgIdx].pFunc = pCfg[CfgIdx].pFunc;
                        pCtrl->NumOfItemCfg += 1U;
                    }
                }

                SvcModule_MutexGive(&(pCtrl->Mutx));
            }
        }
    }

    AmbaMisra_TouchUnused(pModuleCtrl);

    return RetVal;
}

/**
 * Load storage module item data from EEPROM
 *
 * @param [in] pModuleCtrl storage module ctrl
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcModule_Load(void *pModuleCtrl)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    SVC_MODULE_CTRL_s *pCtrl;
    AmbaMisra_TypeCast(&(pCtrl), &(pModuleCtrl));

    if (pCtrl == NULL) {
        RetVal = SVC_NG;
        PRN_MOD_MGR_LOG "Fail to load data from module - invalid control!" PRN_MOD_MGR_NG
    } else if ((pCtrl->State & SVC_MODULE_CTRL_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_MOD_MGR_LOG "Fail to load data from module - initial module first!" PRN_MOD_MGR_NG
    } else {

        PRetVal = SvcModule_MutexTake(&(pCtrl->Mutx), AMBA_KAL_WAIT_FOREVER);
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_MOD_MGR_LOG "Fail to load data from module - take mutex fail! RetVal: 0x%x"
                PRN_MOD_MGR_ARG_UINT32 PRetVal PRN_MOD_MGR_ARG_POST
            PRN_MOD_MGR_NG
        } else {
            UINT8 *pBuf = pCtrl->pShadowBuf;
            Modual_Cal_Param_s  *pModuleHeader   = NULL;
            Modual_Cal_Header_s *pItemHeader     = NULL;
            UINT32              *pHeaderChkSum   = NULL;
            UINT32 CurModuleVersion = 0U, CurMinorVersion = 0U, NumOfItem = 0U;
            UINT32 HeaderSize = 0U;

            if ((pCtrl->State & SVC_MODULE_CTRL_EEPROM_RDY) > 0U) {
                PRN_MOD_MGR_LOG "Current eeprom data has been loaded. Reset it before load eeprom data again!" PRN_MOD_MGR_API
                if (pCtrl->pItemBuf != NULL) {
                    SvcModule_memset(pCtrl->pItemBuf, 0, pCtrl->ItemBufSize);
                }
                pCtrl->ModuleVersion = 0U;
                pCtrl->MinorVersion  = 0U;
                pCtrl->NumOfItem     = 0U;
                pCtrl->State &= ~SVC_MODULE_CTRL_EEPROM_RDY;
            }

            // Read module header
            HeaderSize = (UINT32)(sizeof(Modual_Cal_Param_s));
            RetVal = SvcModule_LoadData(&(pCtrl->EEPROMChan), 0U, pBuf, HeaderSize);
            if (RetVal == SVC_OK) {
                AmbaMisra_TypeCast(&(pModuleHeader), &(pCtrl->pShadowBuf));

                if (pModuleHeader != NULL) {
                    PRN_MOD_MGR_LOG "Success to load data from module - load module header done" PRN_MOD_MGR_DBG
                    PRN_MOD_MGR_LOG "---- Svc Storage Module Header - VinID(%d) ChanID(0x%x) ----"
                        PRN_MOD_MGR_ARG_UINT32 pCtrl->EEPROMChan.VinId  PRN_MOD_MGR_ARG_POST
                        PRN_MOD_MGR_ARG_UINT32 pCtrl->EEPROMChan.ChanId PRN_MOD_MGR_ARG_POST
                    PRN_MOD_MGR_DBG
                    PRN_MOD_MGR_LOG "   Version : 0x%llx"
                        PRN_MOD_MGR_ARG_UINT64 pModuleHeader->Version
                    PRN_MOD_MGR_DBG
                    PRN_MOD_MGR_LOG "  ModuleID : 0x%x"
                        PRN_MOD_MGR_ARG_UINT32 pModuleHeader->ModuleID PRN_MOD_MGR_ARG_POST
                    PRN_MOD_MGR_DBG

                    CurModuleVersion = (UINT32)((pModuleHeader->Version & 0xFF00000000000000U) >> 56UL);
                    CurMinorVersion  = (UINT32)(pModuleHeader->Version & 0x000000000000FFFFU);

                    if (CurModuleVersion != SVC_MODULE_VERSION) {
                        RetVal = 99U;
                        PRN_MOD_MGR_LOG "Not supported module version 0x%x!"
                            PRN_MOD_MGR_ARG_UINT32 CurModuleVersion PRN_MOD_MGR_ARG_POST
                        PRN_MOD_MGR_API
                    }
                }
            } else {
                PRN_MOD_MGR_LOG "Fail to load module header fail!" PRN_MOD_MGR_DBG
            }

            if ((RetVal == SVC_OK) && (pModuleHeader != NULL)) {
                UINT32 ItemIdx;
                UINT8 *pCrcBuf;

                NumOfItem = (UINT32)((pModuleHeader->Version & 0x00FF000000000000U) >> 48U);

                if (NumOfItem > 0U) {
                    HeaderSize  = (UINT32) sizeof(Modual_Cal_Param_s);
                    HeaderSize += ( (UINT32) sizeof(Modual_Cal_Header_s) ) * NumOfItem;
                    HeaderSize += 4U;

                    // Read header
                    RetVal = SvcModule_LoadData(&(pCtrl->EEPROMChan), 0U, pBuf, HeaderSize);
                    if (RetVal != SVC_OK) {
                        PRN_MOD_MGR_LOG "Fail to load item headers fail" PRN_MOD_MGR_NG
                    } else {
                        pCtrl->ShadowRemainSize -= HeaderSize;

                        pBuf = &(pCtrl->pShadowBuf[(UINT32) sizeof(Modual_Cal_Param_s)]);
                        AmbaMisra_TypeCast(&(pItemHeader), &pBuf);

                        if (pItemHeader != NULL) {
                            PRN_MOD_MGR_LOG "Success to load data from module - load item header done" PRN_MOD_MGR_DBG
                            PRN_MOD_MGR_LOG "---- Svc Storage Item Header - VinID(%d) ChanID(0x%x) ----"
                                PRN_MOD_MGR_ARG_UINT32 pCtrl->EEPROMChan.VinId  PRN_MOD_MGR_ARG_POST
                                PRN_MOD_MGR_ARG_UINT32 pCtrl->EEPROMChan.ChanId PRN_MOD_MGR_ARG_POST
                            PRN_MOD_MGR_DBG

                            PRN_MOD_MGR_LOG "  Idx |   Version  |    Size    | CheckSum" PRN_MOD_MGR_DBG

                            for (ItemIdx = 0U; ItemIdx < NumOfItem; ItemIdx ++) {

                                PRN_MOD_MGR_LOG "   %02d | 0x%08x | 0x%08x | 0x%08x"
                                    PRN_MOD_MGR_ARG_UINT32 ItemIdx                       PRN_MOD_MGR_ARG_POST
                                    PRN_MOD_MGR_ARG_UINT32 pItemHeader[ItemIdx].Version  PRN_MOD_MGR_ARG_POST
                                    PRN_MOD_MGR_ARG_UINT32 pItemHeader[ItemIdx].Size     PRN_MOD_MGR_ARG_POST
                                    PRN_MOD_MGR_ARG_UINT32 pItemHeader[ItemIdx].CheckSum PRN_MOD_MGR_ARG_POST
                                PRN_MOD_MGR_DBG

                            }

                        }

                        pBuf = &(pCtrl->pShadowBuf[HeaderSize - 4U]);
                        AmbaMisra_TypeCast(&(pHeaderChkSum), &pBuf);

                        if (pHeaderChkSum != NULL) {
                            PRN_MOD_MGR_LOG "Success to load data from module - load header check sum done" PRN_MOD_MGR_DBG
                            PRN_MOD_MGR_LOG "---- Svc Storage Header CheckSum(0x%x) ----"
                                PRN_MOD_MGR_ARG_UINT32 *pHeaderChkSum PRN_MOD_MGR_ARG_POST
                            PRN_MOD_MGR_DBG

                            AmbaMisra_TypeCast((&pCrcBuf), &(pCtrl->pShadowBuf));

                            PRetVal = AmbaUtility_Crc32(pCrcBuf, HeaderSize - 4U);
                            if (*pHeaderChkSum != PRetVal) {
                                UINT32 TempU32 = HeaderSize - 4U;
                                RetVal = SVC_NG;
                                PRN_MOD_MGR_LOG "Fail to load data from module - header CRC check fail! 0x%x != 0x%x, Addr 0x%08x Size 0x%x"
                                    PRN_MOD_MGR_ARG_UINT32 *pHeaderChkSum PRN_MOD_MGR_ARG_POST
                                    PRN_MOD_MGR_ARG_UINT32 PRetVal        PRN_MOD_MGR_ARG_POST
                                    PRN_MOD_MGR_ARG_CPOINT pCrcBuf        PRN_MOD_MGR_ARG_POST
                                    PRN_MOD_MGR_ARG_UINT32 TempU32        PRN_MOD_MGR_ARG_POST
                                PRN_MOD_MGR_NG
                            }

                            AmbaMisra_TouchUnused(pCrcBuf);
                        }
                    }

                    // Read data
                    if ((RetVal == SVC_OK) && (pItemHeader != NULL)) {
                        UINT32 TotalDataSize = 0U;
                        UINT32 CfgIdx;

                        for (ItemIdx = 0U; ItemIdx < NumOfItem; ItemIdx ++) {
                            TotalDataSize += pItemHeader[ItemIdx].Size;
                        }

                        if (pCtrl->ShadowRemainSize < TotalDataSize) {
                            RetVal = SVC_NG;
                            PRN_MOD_MGR_LOG "Fail to load item data - remain buffer is too small to service item data! 0x%x < 0x%x"
                                PRN_MOD_MGR_ARG_UINT32 pCtrl->ShadowRemainSize PRN_MOD_MGR_ARG_POST
                                PRN_MOD_MGR_ARG_UINT32 TotalDataSize           PRN_MOD_MGR_ARG_POST
                            PRN_MOD_MGR_NG
                        } else {

                            pBuf = &(pCtrl->pShadowBuf[HeaderSize]);
                            RetVal = SvcModule_LoadData(&(pCtrl->EEPROMChan), HeaderSize, pBuf, TotalDataSize);
                            if (RetVal != SVC_OK) {
                                PRN_MOD_MGR_LOG "Fail to load item data fail" PRN_MOD_MGR_NG
                            } else {
                                pCtrl->ShadowRemainSize -= TotalDataSize;

                                for (ItemIdx = 0U; ItemIdx < NumOfItem; ItemIdx ++) {

                                    AmbaMisra_TypeCast(&(pCrcBuf), &(pBuf));

                                    PRetVal = AmbaUtility_Crc32(pCrcBuf, pItemHeader[ItemIdx].Size);
                                    if (pItemHeader[ItemIdx].CheckSum != PRetVal) {
                                        RetVal = SVC_NG;
                                        PRN_MOD_MGR_LOG "Fail to load data from module - no.%02d item data CRC check fail! 0x%x != 0x%x, Addr %p, Size 0x%x"
                                            PRN_MOD_MGR_ARG_UINT32 ItemIdx                       PRN_MOD_MGR_ARG_POST
                                            PRN_MOD_MGR_ARG_UINT32 pItemHeader[ItemIdx].CheckSum PRN_MOD_MGR_ARG_POST
                                            PRN_MOD_MGR_ARG_UINT32 PRetVal                       PRN_MOD_MGR_ARG_POST
                                            PRN_MOD_MGR_ARG_CPOINT pCrcBuf                       PRN_MOD_MGR_ARG_POST
                                            PRN_MOD_MGR_ARG_UINT32 pItemHeader[ItemIdx].Size     PRN_MOD_MGR_ARG_POST
                                        PRN_MOD_MGR_NG
                                        break;
                                    } else {
                                        pCtrl->pItem[ItemIdx].ItemVersion   = ((pItemHeader[ItemIdx].Version & 0xFF000000U) >> 24U);
                                        pCtrl->pItem[ItemIdx].ModuleVersion = ((pItemHeader[ItemIdx].Version & 0x00FF0000U) >> 16U);
                                        pCtrl->pItem[ItemIdx].MinorVersion  = ((pItemHeader[ItemIdx].Version & 0x0000FF00U) >> 8U);
                                        pCtrl->pItem[ItemIdx].ProjectID     =  (pItemHeader[ItemIdx].Version & 0x0000000FU);
                                        pCtrl->pItem[ItemIdx].pDataBuf      = pBuf;
                                        pCtrl->pItem[ItemIdx].DataBufSize   = pItemHeader[ItemIdx].Size;
                                        pCtrl->pItem[ItemIdx].DataBufUsaged = pItemHeader[ItemIdx].Size;
                                        pCtrl->pItem[ItemIdx].State         = SVC_MODULE_ITEM_INIT;

                                        for (CfgIdx = 0U; CfgIdx < pCtrl->NumOfItemCfg; CfgIdx ++) {
                                            if (pCtrl->ItemCfg[CfgIdx].ItemVersion == pCtrl->pItem[ItemIdx].ItemVersion) {
                                                pCtrl->pItem[ItemIdx].pFunc = pCtrl->ItemCfg[CfgIdx].pFunc;
                                                break;
                                            }
                                        }

                                    }

                                    pBuf = &(pBuf[pItemHeader[ItemIdx].Size]);
                                }
                            }
                        }
                    }
                }

                if (RetVal == SVC_OK) {
                    pCtrl->NumOfItem     = NumOfItem;
                    pCtrl->ModuleVersion = CurModuleVersion;
                    pCtrl->MinorVersion  = CurMinorVersion;
                }
            }

            if (RetVal != SVC_NG) {
                RetVal = SVC_OK;
                pCtrl->State |= SVC_MODULE_CTRL_EEPROM_RDY;
                PRN_MOD_MGR_LOG "Success to load data from eeprom!" PRN_MOD_MGR_OK
            }

            SvcModule_MutexGive(&(pCtrl->Mutx));

            AmbaMisra_TouchUnused(pModuleHeader);
            AmbaMisra_TouchUnused(pItemHeader);
            AmbaMisra_TouchUnused(pHeaderChkSum);
        }

        if ((SvcModuleCtrlFlag & SVC_LOG_CAL_DBG) > 0U) {
            SvcModule_Info(pCtrl);
        }
    }

    AmbaMisra_TouchUnused(pModuleCtrl);

    return RetVal;
}

/**
 * Save storage module data to EEPROM
 *
 * @param [in] pModuleCtrl storage module controller
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcModule_Save(void *pModuleCtrl)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    SVC_MODULE_CTRL_s *pCtrl;
    AmbaMisra_TypeCast(&(pCtrl), &(pModuleCtrl));

    if (pCtrl == NULL) {
        RetVal = SVC_NG;
        PRN_MOD_MGR_LOG "Fail to save data to module - invalid control!" PRN_MOD_MGR_NG
    } else if ((pCtrl->State & SVC_MODULE_CTRL_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_MOD_MGR_LOG "Fail to save data to module - initial module first!" PRN_MOD_MGR_NG
    } else {

        PRetVal = SvcModule_MutexTake(&(pCtrl->Mutx), AMBA_KAL_WAIT_FOREVER);
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_MOD_MGR_LOG "Fail to save data to module - take mutex fail! RetVal: 0x%x"
                PRN_MOD_MGR_ARG_UINT32 PRetVal PRN_MOD_MGR_ARG_POST
            PRN_MOD_MGR_NG
        } else {
            UINT32 ItemIdx;
            UINT32 EEPROMCapacity;
            UINT32 HeaderSize = 0U;

            // Get the EEPROM capacity
            RetVal = SvcModule_MemQuery(&(pCtrl->EEPROMChan), &EEPROMCapacity, NULL, NULL);
            if (RetVal != 0U) {
                PRN_MOD_MGR_LOG "Fail to save data to module - get eeprom capacity fail" PRN_MOD_MGR_NG
            }

            // Check the total write size
            if (RetVal == SVC_OK) {
                UINT32 TotalDataSize = 0U;

                HeaderSize  = (UINT32) sizeof(Modual_Cal_Param_s);                           // Module header
                HeaderSize += ( (UINT32) sizeof(Modual_Cal_Header_s) * pCtrl->NumOfItem );   // Item headers
                HeaderSize += 4U;                                                               // Header checksum

                for (ItemIdx = 0U; ItemIdx < pCtrl->NumOfItem; ItemIdx ++) {
                    TotalDataSize += pCtrl->pItem[ItemIdx].DataBufUsaged;
                }
                TotalDataSize += HeaderSize;

                if (TotalDataSize > EEPROMCapacity) {
                    RetVal = SVC_NG;
                    PRN_MOD_MGR_LOG "Fail to save data to module - write size(0x%x) is larger than eeprom capacity(0x%x)!"
                        PRN_MOD_MGR_ARG_UINT32 TotalDataSize  PRN_MOD_MGR_ARG_POST
                        PRN_MOD_MGR_ARG_UINT32 EEPROMCapacity PRN_MOD_MGR_ARG_POST
                    PRN_MOD_MGR_NG
                }
            }

            // Configure header
            if (RetVal == SVC_OK) {
                UINT8 *pBuf;
                UINT32 BufSize;
                Modual_Cal_Param_s  *pModuleHeader;
                Modual_Cal_Header_s *pItemHeader;
                UINT32              *pHeaderCheckSum = NULL;
                UINT8               *pCrcBuf;
                UINT32               CrcHeaderSize;
                UINT64 TempU64;

                // Configure module header
                AmbaMisra_TypeCast(&(pModuleHeader), &(pCtrl->pWorkBuf));
                SvcModule_memset(pModuleHeader, 0, pCtrl->WorkBufSize);
                BufSize = (UINT32) sizeof(Modual_Cal_Param_s);

                if (pModuleHeader != NULL) {
                    SvcModule_memset(pModuleHeader, 0, pCtrl->WorkBufSize);
                    pModuleHeader->ModuleID = pCtrl->ModuleID;

                    TempU64 = (UINT64)(pCtrl->ModuleVersion); TempU64 &= 0x00000000000000FFU;
                    TempU64 = TempU64 << 56U;
                    pModuleHeader->Version |= TempU64;

                    TempU64 = (UINT64)(pCtrl->NumOfItem); TempU64 &= 0x00000000000000FFU;
                    TempU64 = TempU64 << 48U;
                    pModuleHeader->Version |= TempU64;

                    TempU64 = (UINT64)(pCtrl->MinorVersion); TempU64 &= 0x000000000000FFFFU;
                    pModuleHeader->Version |= TempU64;

                    PRN_MOD_MGR_LOG " " PRN_MOD_MGR_DBG
                    PRN_MOD_MGR_LOG "---- Configure Module Header %p"
                        PRN_MOD_MGR_ARG_CPOINT pModuleHeader PRN_MOD_MGR_ARG_POST
                    PRN_MOD_MGR_DBG
                    PRN_MOD_MGR_LOG "   Version : 0x%llx"
                        PRN_MOD_MGR_ARG_UINT64 pModuleHeader->Version
                    PRN_MOD_MGR_DBG
                    PRN_MOD_MGR_LOG "  ModuleID : 0x%x"
                        PRN_MOD_MGR_ARG_UINT32 pModuleHeader->ModuleID PRN_MOD_MGR_ARG_POST
                    PRN_MOD_MGR_DBG}

                // Configure item headers
                pBuf = &(pCtrl->pWorkBuf[BufSize]);
                AmbaMisra_TypeCast(&(pItemHeader), &(pBuf));
                BufSize += ((UINT32)sizeof(Modual_Cal_Header_s) * pCtrl->NumOfItem);
                if (pItemHeader != NULL) {
                    UINT32 ItemHeaderNum = 0U;
                    for (ItemIdx = 0U; ItemIdx < SVC_MODULE_ITEM_MAX_NUM; ItemIdx ++) {
                        if ((pCtrl->pItem[ItemIdx].State & SVC_MODULE_ITEM_INIT) > 0U) {
                            pItemHeader[ItemHeaderNum].Version  = ( (pCtrl->pItem[ItemIdx].ItemVersion   & 0x000000FFU ) << 24U );
                            pItemHeader[ItemHeaderNum].Version |= ( (pCtrl->pItem[ItemIdx].ModuleVersion & 0x000000FFU ) << 16U );
                            pItemHeader[ItemHeaderNum].Version |= ( (pCtrl->pItem[ItemIdx].MinorVersion  & 0x000000FFU ) << 8U );
                            pItemHeader[ItemHeaderNum].Version |=   (pCtrl->pItem[ItemIdx].ProjectID     & 0x0000000FU );
                            pItemHeader[ItemHeaderNum].Size     = pCtrl->pItem[ItemIdx].DataBufUsaged;
                            AmbaMisra_TypeCast(&(pCrcBuf), &(pCtrl->pItem[ItemIdx].pDataBuf));
                            pItemHeader[ItemHeaderNum].CheckSum = AmbaUtility_Crc32(pCrcBuf, pItemHeader[ItemHeaderNum].Size);

                            PRN_MOD_MGR_LOG "---- Configure Item Header %p from no.%02d item %p"
                                PRN_MOD_MGR_ARG_CPOINT &(pItemHeader[ItemHeaderNum]) PRN_MOD_MGR_ARG_POST
                                PRN_MOD_MGR_ARG_UINT32 ItemIdx                       PRN_MOD_MGR_ARG_POST
                                PRN_MOD_MGR_ARG_CPOINT &(pCtrl->pItem[ItemIdx])      PRN_MOD_MGR_ARG_POST
                            PRN_MOD_MGR_DBG
                            PRN_MOD_MGR_LOG "  Version : 0x%x"
                                PRN_MOD_MGR_ARG_UINT32 pItemHeader[ItemHeaderNum].Version PRN_MOD_MGR_ARG_POST
                            PRN_MOD_MGR_DBG
                            PRN_MOD_MGR_LOG "     Size : 0x%x"
                                PRN_MOD_MGR_ARG_UINT32 pItemHeader[ItemHeaderNum].Size PRN_MOD_MGR_ARG_POST
                            PRN_MOD_MGR_DBG
                            PRN_MOD_MGR_LOG " CheckSum : 0x%x"
                                PRN_MOD_MGR_ARG_UINT32 pItemHeader[ItemHeaderNum].CheckSum PRN_MOD_MGR_ARG_POST
                            PRN_MOD_MGR_DBG
                            ItemHeaderNum += 1U;
                        }
                    }

                    if (pCtrl->NumOfItem != ItemHeaderNum) {
                        RetVal = SVC_NG;
                        PRN_MOD_MGR_LOG "Fail to save data to module - item header number is wrong %d != %d"
                            PRN_MOD_MGR_ARG_UINT32 ItemHeaderNum    PRN_MOD_MGR_ARG_POST
                            PRN_MOD_MGR_ARG_UINT32 pCtrl->NumOfItem PRN_MOD_MGR_ARG_POST
                        PRN_MOD_MGR_NG
                    }
                }

                // Configure header checksum
                CrcHeaderSize = BufSize;
                pBuf = &(pCtrl->pWorkBuf[BufSize]);
                AmbaMisra_TypeCast(&(pHeaderCheckSum), &(pBuf));
                BufSize += (UINT32) sizeof(UINT32);
                if (pHeaderCheckSum != NULL) {
                    AmbaMisra_TypeCast(&(pCrcBuf), &(pCtrl->pWorkBuf));
                    *pHeaderCheckSum = AmbaUtility_Crc32(pCrcBuf, CrcHeaderSize);
                    PRN_MOD_MGR_LOG "---- Configure Module Header CheckSum 0x%08x"
                        PRN_MOD_MGR_ARG_UINT32 *pHeaderCheckSum PRN_MOD_MGR_ARG_POST
                    PRN_MOD_MGR_DBG
                    AmbaMisra_TouchUnused(pCrcBuf);
                }

                if (HeaderSize != BufSize) {
                    PRN_MOD_MGR_LOG "Fail to save data to module - header size does not match! 0x%x != 0x%x"
                        PRN_MOD_MGR_ARG_UINT32 HeaderSize PRN_MOD_MGR_ARG_POST
                        PRN_MOD_MGR_ARG_UINT32 BufSize    PRN_MOD_MGR_ARG_POST
                    PRN_MOD_MGR_API
                }
                AmbaMisra_TouchUnused(pBuf);
            }

            // Write Item Data
            if (RetVal == SVC_OK) {
                UINT32 ItemDataOffset = HeaderSize;

                for (ItemIdx = 0U; ItemIdx < pCtrl->NumOfItem; ItemIdx ++) {
                    if ((pCtrl->pItem[ItemIdx].State & SVC_MODULE_ITEM_INIT) > 0U) {
                        PRetVal = SvcModule_SaveData(&(pCtrl->EEPROMChan),
                                                     ItemDataOffset,
                                                     pCtrl->pItem[ItemIdx].pDataBuf,
                                                     pCtrl->pItem[ItemIdx].DataBufUsaged);
                        if (PRetVal != 0U) {
                            RetVal |= SVC_NG;
                            PRN_MOD_MGR_LOG "Fail to save data to module - write no.%02d item data fail!"
                                PRN_MOD_MGR_ARG_UINT32 ItemIdx PRN_MOD_MGR_ARG_POST
                            PRN_MOD_MGR_NG
                            break;
                        } else {
                            ItemDataOffset += pCtrl->pItem[ItemIdx].DataBufUsaged;
                        }
                    }
                }

                if (RetVal == SVC_OK) {
                    PRN_MOD_MGR_LOG "Success to save data to module - write item data done!" PRN_MOD_MGR_OK
                }
            }

            // Write Header
            if (RetVal == SVC_OK) {
                // Write Header to eeprom
                RetVal = SvcModule_SaveData(&(pCtrl->EEPROMChan), 0U, pCtrl->pWorkBuf, HeaderSize);
                if (RetVal != SVC_OK) {
                    PRN_MOD_MGR_LOG "Fail to save data to module - write headers fail!" PRN_MOD_MGR_NG
                } else {
                    PRN_MOD_MGR_LOG "Success to save data to module - write header done!" PRN_MOD_MGR_OK
                }
            }

            SvcModule_MutexGive(&(pCtrl->Mutx));
        }
    }

    AmbaMisra_TouchUnused(pModuleCtrl);

    return RetVal;
}

/**
 * Reset storage module
 *
 * @param [in] pModuleCtrl storage module controller
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcModule_Reset(void *pModuleCtrl)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    SVC_MODULE_CTRL_s *pCtrl;
    AmbaMisra_TypeCast(&(pCtrl), &(pModuleCtrl));

    if (pCtrl == NULL) {
        RetVal = SVC_NG;
        PRN_MOD_MGR_LOG "Fail to reset module - invalid control!" PRN_MOD_MGR_NG
    } else if ((pCtrl->State & SVC_MODULE_CTRL_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_MOD_MGR_LOG "Fail to reset module - initial module first!" PRN_MOD_MGR_NG
    } else {
        PRetVal = SvcModule_MutexTake(&(pCtrl->Mutx), AMBA_KAL_WAIT_FOREVER);
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_MOD_MGR_LOG "Fail to reset module - take mutex fail! RetVal: 0x%x"
                PRN_MOD_MGR_ARG_UINT32 PRetVal PRN_MOD_MGR_ARG_POST
            PRN_MOD_MGR_NG
        } else {
            UINT64 TempU64;
            Modual_Cal_Param_s *pModuleHeader;
            AmbaMisra_TypeCast(&(pModuleHeader), &(pCtrl->pWorkBuf));

            if (pModuleHeader != NULL) {
                SvcModule_memset(pModuleHeader, 0, (UINT32) sizeof(Modual_Cal_Param_s) );
                pModuleHeader->ModuleID = pCtrl->ModuleID;

                TempU64 = (UINT64)(pCtrl->ModuleVersion); TempU64 &= 0x00000000000000FFU;
                TempU64 = TempU64 << 56U;
                pModuleHeader->Version |= TempU64;

                TempU64 = (UINT64)(pCtrl->MinorVersion); TempU64 &= 0x000000000000FFFFU;
                pModuleHeader->Version |= TempU64;

                PRN_MOD_MGR_LOG " " PRN_MOD_MGR_DBG
                PRN_MOD_MGR_LOG "---- Configure Module Header %p"
                    PRN_MOD_MGR_ARG_CPOINT pModuleHeader PRN_MOD_MGR_ARG_POST
                PRN_MOD_MGR_DBG
                PRN_MOD_MGR_LOG "   Version : 0x%llx"
                    PRN_MOD_MGR_ARG_UINT64 pModuleHeader->Version
                PRN_MOD_MGR_DBG
                PRN_MOD_MGR_LOG "  ModuleID : 0x%x"
                    PRN_MOD_MGR_ARG_UINT32 pModuleHeader->ModuleID PRN_MOD_MGR_ARG_POST
                PRN_MOD_MGR_DBG

                RetVal = SvcModule_SaveData(&(pCtrl->EEPROMChan), 0U, pCtrl->pWorkBuf, (UINT32) sizeof(Modual_Cal_Param_s) );
                if (RetVal != SVC_OK) {
                    PRN_MOD_MGR_LOG "Fail to reset module - write module headers fail!" PRN_MOD_MGR_NG
                } else {
                    PRN_MOD_MGR_LOG "Success to reset module!" PRN_MOD_MGR_OK
                }
            }

            SvcModule_MutexGive(&(pCtrl->Mutx));
        }
    }

    AmbaMisra_TouchUnused(pModuleCtrl);

    return RetVal;
}

/**
 * Get storage module item data
 *
 * @param [in] pModuleCtrl storage module ctrl
 * @param [in] pDataInfo item data buffer info
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcModule_ItemGet(void *pModuleCtrl, SVC_MODULE_ITEM_DATA_INFO_s *pDataInfo)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    SVC_MODULE_CTRL_s *pCtrl;
    AmbaMisra_TypeCast(&(pCtrl), &(pModuleCtrl));

    if (pCtrl == NULL) {
        RetVal = SVC_NG;
        PRN_MOD_MGR_LOG "Fail to get item data - invalid control!" PRN_MOD_MGR_DBG
    } else if ((pCtrl->State & SVC_MODULE_CTRL_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_MOD_MGR_LOG "Fail to get item data - initial module first!" PRN_MOD_MGR_DBG
    } else if ((pCtrl->State & SVC_MODULE_CTRL_EEPROM_RDY) == 0U) {
        RetVal = SVC_NG;
        PRN_MOD_MGR_LOG "Fail to get item data - loading module first!" PRN_MOD_MGR_DBG
    } else if (pDataInfo == NULL) {
        RetVal = SVC_NG;
        PRN_MOD_MGR_LOG "Fail to get item data - invalid data info!" PRN_MOD_MGR_NG
    } else if (pDataInfo->pData == NULL) {
        RetVal = SVC_NG;
        PRN_MOD_MGR_LOG "Fail to get item data - output data buffer should not null!" PRN_MOD_MGR_NG
    } else {
        PRetVal = SvcModule_MutexTake(&(pCtrl->Mutx), AMBA_KAL_WAIT_FOREVER);
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_MOD_MGR_LOG "Fail to get item data - take mutex fail! RetVal: 0x%x"
                PRN_MOD_MGR_ARG_UINT32 PRetVal PRN_MOD_MGR_ARG_POST
            PRN_MOD_MGR_NG
        } else {
            UINT32 ItemIdx;
            SVC_MODULE_ITEM_INFO_s *pItem = NULL;

            for (ItemIdx = 0U; ItemIdx < pCtrl->NumOfItem; ItemIdx ++) {
                if (((pCtrl->pItem[ItemIdx].State & SVC_MODULE_ITEM_INIT) > 0U) &&
                    (pCtrl->pItem[ItemIdx].ItemVersion == pDataInfo->ItemVersion)) {
                    pItem = &(pCtrl->pItem[ItemIdx]);
                }
            }

            if (pItem == NULL) {
                RetVal = SVC_NG;
                PRN_MOD_MGR_LOG "the item version(0x%x) does not exist!"
                    PRN_MOD_MGR_ARG_UINT32 pDataInfo->ItemVersion PRN_MOD_MGR_ARG_POST
                PRN_MOD_MGR_API
            } else if (pItem->pFunc == NULL) {
                if (pDataInfo->DataSize < pItem->DataBufUsaged) {
                    RetVal = SVC_NG;
                    PRN_MOD_MGR_LOG "Fail to get item data - request data size(0x%x) is lager than output data size(0x%x)"
                        PRN_MOD_MGR_ARG_UINT32 pItem->DataBufUsaged PRN_MOD_MGR_ARG_POST
                        PRN_MOD_MGR_ARG_UINT32 pDataInfo->DataSize  PRN_MOD_MGR_ARG_POST
                    PRN_MOD_MGR_NG
                } else {
                    if (pItem->pDataBuf != NULL) {
                        SvcModule_memset(pDataInfo->pData, 0, pDataInfo->DataSize);
                        SvcModule_memcpy(pDataInfo->pData, pItem->pDataBuf, pItem->DataBufUsaged);
                    }
                }
            } else {
                PRetVal = (pItem->pFunc)(SVC_MODULE_ITEM_CMD_GET, pDataInfo, pItem->pDataBuf, &(pItem->DataBufSize), pCtrl->pWorkBuf, &(pCtrl->WorkBufSize));
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    PRN_MOD_MGR_LOG "Fail to get item data - get item version(0x%x) data fail!"
                        PRN_MOD_MGR_ARG_UINT32 pDataInfo->ItemVersion PRN_MOD_MGR_ARG_POST
                    PRN_MOD_MGR_NG
                }
            }

            SvcModule_MutexGive(&(pCtrl->Mutx));

            AmbaMisra_TouchUnused(pItem);
        }

        AmbaMisra_TouchUnused(pModuleCtrl);
    }

    return RetVal;
}

/**
 * Set storage module item data
 *
 * @param [in] pModuleCtrl storage module ctrl
 * @param [in] pDataInfo item data buffer info
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcModule_ItemSet(void *pModuleCtrl, SVC_MODULE_ITEM_DATA_INFO_s *pDataInfo)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    SVC_MODULE_CTRL_s *pCtrl;
    AmbaMisra_TypeCast(&(pCtrl), &(pModuleCtrl));

    if (pCtrl == NULL) {
        RetVal = SVC_NG;
        PRN_MOD_MGR_LOG "Fail to set item data - invalid control!" PRN_MOD_MGR_NG
    } else if ((pCtrl->State & SVC_MODULE_CTRL_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_MOD_MGR_LOG "Fail to set item data - initial module first!" PRN_MOD_MGR_NG
    } else if ((pCtrl->State & SVC_MODULE_CTRL_EEPROM_RDY) == 0U) {
        RetVal = SVC_NG;
        PRN_MOD_MGR_LOG "Fail to set item data - loading module first!" PRN_MOD_MGR_NG
    } else if (pDataInfo == NULL) {
        RetVal = SVC_NG;
        PRN_MOD_MGR_LOG "Fail to set item data - invalid data info!" PRN_MOD_MGR_NG
    } else if (pDataInfo->pData == NULL) {
        RetVal = SVC_NG;
        PRN_MOD_MGR_LOG "Fail to set item data - output data buffer should not null!" PRN_MOD_MGR_NG
    } else if (pDataInfo->DataSize == 0U) {
        RetVal = SVC_NG;
        PRN_MOD_MGR_LOG "Fail to set item data - output data buffer size should not zero!" PRN_MOD_MGR_NG
    } else {
        PRetVal = SvcModule_MutexTake(&(pCtrl->Mutx), AMBA_KAL_WAIT_FOREVER);
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            PRN_MOD_MGR_LOG "Fail to set item data - take mutex fail! RetVal: 0x%x"
                PRN_MOD_MGR_ARG_UINT32 PRetVal PRN_MOD_MGR_ARG_POST
            PRN_MOD_MGR_NG
        } else {
            UINT32 ItemIdx, CfgIdx, ItemExisted = 0U;
            SVC_MODULE_ITEM_INFO_s *pItem = NULL;

            // Get the item info if exist
            for (ItemIdx = 0U; ItemIdx < SVC_MODULE_ITEM_MAX_NUM; ItemIdx ++) {
                if (((pCtrl->pItem[ItemIdx].State & SVC_MODULE_ITEM_INIT) > 0U) &&
                    (pCtrl->pItem[ItemIdx].ItemVersion == pDataInfo->ItemVersion)) {
                    pItem = &(pCtrl->pItem[ItemIdx]);
                    ItemExisted = 1U;
                    break;
                }
            }

            // Get the item info if non-exist
            if (pItem == NULL) {
                for (ItemIdx = 0U; ItemIdx < SVC_MODULE_ITEM_MAX_NUM; ItemIdx ++) {
                    if ((pCtrl->pItem[ItemIdx].State & SVC_MODULE_ITEM_INIT) == 0U) {
                        pItem = &(pCtrl->pItem[ItemIdx]);
                        break;
                    }
                }

                if (pItem == NULL) {
                    RetVal = SVC_NG;
                    PRN_MOD_MGR_LOG "Fail to set item data - there is not item slot to service it!" PRN_MOD_MGR_NG
                }
            }

            // Get the user function if exist
            if ((RetVal == SVC_OK) && (pItem != NULL)) {
                if (pItem->ItemVersion != pDataInfo->ItemVersion) {
                    SvcModule_memset(pItem, 0, sizeof(SVC_MODULE_ITEM_INFO_s));
                    for (CfgIdx = 0U; CfgIdx < pCtrl->NumOfItemCfg; CfgIdx ++) {
                        if (pDataInfo->ItemVersion == pCtrl->ItemCfg[CfgIdx].ItemVersion) {
                            pItem->pFunc = pCtrl->ItemCfg[CfgIdx].pFunc;
                        }
                    }
                }
            }

            // Set the data
            if ((RetVal == SVC_OK) && (pItem != NULL)) {
                void *pSrcData = NULL, *pDstData = NULL;

                // if user function does not exist
                if (pItem->pFunc == NULL) {
                    pItem->DataBufUsaged = pDataInfo->DataSize;
                    pSrcData = pDataInfo->pData;
                } else {    // if user function exist, call user function to set data to working buffer
                    UINT32 ItemQrySize = 0U;
                    PRetVal = (pItem->pFunc)(SVC_MODULE_ITEM_CMD_MEM_QRY, &(ItemQrySize), NULL, NULL, NULL, NULL);
                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;
                        PRN_MOD_MGR_LOG "Fail to set item data - query item version(0x%x) work memory size fail" PRN_MOD_MGR_NG
                    } else {
                        if (ItemQrySize > pCtrl->WorkBufSize) {
                            RetVal = SVC_NG;
                            PRN_MOD_MGR_LOG "Fail to set item data - current work memory is too small to service it! 0x%x > 0x%x"
                                PRN_MOD_MGR_ARG_UINT32 ItemQrySize        PRN_MOD_MGR_ARG_POST
                                PRN_MOD_MGR_ARG_UINT32 pCtrl->WorkBufSize PRN_MOD_MGR_ARG_POST
                            PRN_MOD_MGR_NG
                        }
                    }

                    if (RetVal == SVC_OK) {

                        PRetVal = (pItem->pFunc)(SVC_MODULE_ITEM_CMD_SET, pDataInfo, pCtrl->pWorkBuf, &(pCtrl->WorkBufSize), &pItem->DataBufUsaged, NULL);
                        if (PRetVal != 0U) {
                            RetVal = SVC_NG;
                            PRN_MOD_MGR_LOG "Fail to set item data - set data fail!" PRN_MOD_MGR_NG
                        } else {
                            AmbaMisra_TypeCast(&(pSrcData), &(pCtrl->pWorkBuf));
                        }
                    }
                }

                if ((RetVal == SVC_OK) && (pSrcData != NULL) && (pItem->DataBufUsaged > 0U)) {

                    // Original item data buffer exist.
                    if (pItem->DataBufSize > 0U) {
                        // if original item data buffer small new data size
                        if (pItem->DataBufUsaged > pItem->DataBufSize) {
                            RetVal = SVC_NG;
                            PRN_MOD_MGR_LOG "Fail to set item data - new data size(0x%x) is out-of original space(0x%x)!"
                                PRN_MOD_MGR_ARG_UINT32 pItem->DataBufUsaged PRN_MOD_MGR_ARG_POST
                                PRN_MOD_MGR_ARG_UINT32 pItem->DataBufSize   PRN_MOD_MGR_ARG_POST
                            PRN_MOD_MGR_NG
                        } else {
                            AmbaMisra_TypeCast(&(pDstData), &(pItem->pDataBuf));
                        }
                    } else {
                        UINT32 NewItemReqBufSize = pItem->DataBufUsaged + (UINT32)(sizeof(Modual_Cal_Header_s));

                        if (pCtrl->NumOfItem == 0U) {
                            pCtrl->ShadowRemainSize -= (UINT32)(sizeof(Modual_Cal_Param_s)) + 4U;
                        }

                        if (pCtrl->ShadowRemainSize < NewItemReqBufSize) {
                            RetVal = SVC_NG;
                            PRN_MOD_MGR_LOG "Fail to set item data - new (header + data) size(0x%x) is out-of remain space(0x%x)!"
                                PRN_MOD_MGR_ARG_UINT32 NewItemReqBufSize       PRN_MOD_MGR_ARG_POST
                                PRN_MOD_MGR_ARG_UINT32 pCtrl->ShadowRemainSize PRN_MOD_MGR_ARG_POST
                            PRN_MOD_MGR_NG
                        } else {
                            pItem->pDataBuf    = &(pCtrl->pShadowBuf[pCtrl->ShadowBufSize - pCtrl->ShadowRemainSize]);
                            pItem->DataBufSize = pItem->DataBufUsaged;
                            pCtrl->ShadowRemainSize -= NewItemReqBufSize;

                            AmbaMisra_TypeCast(&(pDstData), &(pItem->pDataBuf));
                        }
                    }

                    if (pDstData != NULL) {
                        SvcModule_memset(pDstData, 0, pItem->DataBufSize);
                        SvcModule_memcpy(pDstData, pSrcData, pItem->DataBufUsaged);

                        pItem->ItemVersion = pDataInfo->ItemVersion;
                        pItem->ModuleVersion = pCtrl->ModuleVersion;
                        pItem->MinorVersion = pDataInfo->MinorVersion;
                        pItem->ProjectID = pDataInfo->ProjectID;
                        pItem->State = SVC_MODULE_ITEM_INIT;
                        if (ItemExisted == 0U) {
                            pCtrl->NumOfItem += 1U;
                        }
                        PRN_MOD_MGR_LOG "Success to set item data. %p"
                            PRN_MOD_MGR_ARG_CPOINT pItem PRN_MOD_MGR_ARG_POST
                        PRN_MOD_MGR_OK

                        PRN_MOD_MGR_LOG "---- Item Info" PRN_MOD_MGR_DBG
                        PRN_MOD_MGR_LOG "          State : 0x%x"
                            PRN_MOD_MGR_ARG_UINT32 pItem->State PRN_MOD_MGR_ARG_POST
                        PRN_MOD_MGR_DBG
                        PRN_MOD_MGR_LOG "    ItemVersion : 0x%x"
                            PRN_MOD_MGR_ARG_UINT32 pItem->ItemVersion PRN_MOD_MGR_ARG_POST
                        PRN_MOD_MGR_DBG
                        PRN_MOD_MGR_LOG "  ModuleVersion : 0x%x"
                            PRN_MOD_MGR_ARG_UINT32 pItem->ModuleVersion PRN_MOD_MGR_ARG_POST
                        PRN_MOD_MGR_DBG
                        PRN_MOD_MGR_LOG "   MinorVersion : 0x%x"
                            PRN_MOD_MGR_ARG_UINT32 pItem->MinorVersion PRN_MOD_MGR_ARG_POST
                        PRN_MOD_MGR_DBG
                        PRN_MOD_MGR_LOG "      ProjectID : 0x%x"
                            PRN_MOD_MGR_ARG_UINT32 pItem->ProjectID PRN_MOD_MGR_ARG_POST
                        PRN_MOD_MGR_DBG
                        PRN_MOD_MGR_LOG "    DataBufBase : %p"
                            PRN_MOD_MGR_ARG_CPOINT pItem->pDataBuf PRN_MOD_MGR_ARG_POST
                        PRN_MOD_MGR_DBG
                        PRN_MOD_MGR_LOG "    DataBufSize : 0x%08x"
                            PRN_MOD_MGR_ARG_UINT32 pItem->DataBufSize PRN_MOD_MGR_ARG_POST
                        PRN_MOD_MGR_DBG
                        PRN_MOD_MGR_LOG "  DataBufUsaged : 0x%08x"
                            PRN_MOD_MGR_ARG_UINT32 pItem->DataBufUsaged PRN_MOD_MGR_ARG_POST
                        PRN_MOD_MGR_DBG
                    }
                }

                AmbaMisra_TouchUnused(pSrcData);
            }

            SvcModule_MutexGive(&(pCtrl->Mutx));
        }

        AmbaMisra_TouchUnused(pModuleCtrl);
    }

    return RetVal;
}

/**
 * Print storage module controller information
 *
 * @param [in] pModuleCtrl storage module ctrl
 *
 */
void SvcModule_Info(void *pModuleCtrl)
{
    UINT32 PRetVal;
    SVC_MODULE_CTRL_s *pCtrl;
    AmbaMisra_TypeCast(&(pCtrl), &(pModuleCtrl));

    if (pCtrl != NULL) {
        if ((pCtrl->State & SVC_MODULE_CTRL_INIT) > 0U) {
            PRetVal = SvcModule_MutexTake(&(pCtrl->Mutx), AMBA_KAL_NO_WAIT);
            if (PRetVal == 0U) {

                PRN_MOD_MGR_LOG " " PRN_MOD_MGR_API
                PRN_MOD_MGR_LOG "---- Svc Storage Module Info ----" PRN_MOD_MGR_API
                PRN_MOD_MGR_LOG "               Name : %s"
                    PRN_MOD_MGR_ARG_CSTR   pCtrl->Name PRN_MOD_MGR_ARG_POST
                PRN_MOD_MGR_API
                PRN_MOD_MGR_LOG "              State : 0x%x"
                    PRN_MOD_MGR_ARG_UINT32 pCtrl->State PRN_MOD_MGR_ARG_POST
                PRN_MOD_MGR_API
                PRN_MOD_MGR_LOG "   EEPROMChan.VinId : 0x%x"
                    PRN_MOD_MGR_ARG_UINT32 pCtrl->EEPROMChan.VinId PRN_MOD_MGR_ARG_POST
                PRN_MOD_MGR_API
                PRN_MOD_MGR_LOG "  EEPROMChan.ChanId : 0x%x"
                    PRN_MOD_MGR_ARG_UINT32 pCtrl->EEPROMChan.ChanId PRN_MOD_MGR_ARG_POST
                PRN_MOD_MGR_API
                PRN_MOD_MGR_LOG "            MemBase : %p"
                    PRN_MOD_MGR_ARG_CPOINT pCtrl->pMemBuf PRN_MOD_MGR_ARG_POST
                PRN_MOD_MGR_API
                PRN_MOD_MGR_LOG "            MemSize : 0x%x"
                    PRN_MOD_MGR_ARG_UINT32 pCtrl->MemSize PRN_MOD_MGR_ARG_POST
                PRN_MOD_MGR_API
                PRN_MOD_MGR_LOG "        CtrlBufBase : %p"
                    PRN_MOD_MGR_ARG_CPOINT pCtrl->pCtrlBuf PRN_MOD_MGR_ARG_POST
                PRN_MOD_MGR_API
                PRN_MOD_MGR_LOG "        CtrlBufSize : 0x%x"
                    PRN_MOD_MGR_ARG_UINT32 pCtrl->CtrlBufSize PRN_MOD_MGR_ARG_POST
                PRN_MOD_MGR_API
                PRN_MOD_MGR_LOG "        ItemBufBase : %p"
                    PRN_MOD_MGR_ARG_CPOINT pCtrl->pItemBuf PRN_MOD_MGR_ARG_POST
                PRN_MOD_MGR_API
                PRN_MOD_MGR_LOG "        ItemBufSize : 0x%x"
                    PRN_MOD_MGR_ARG_UINT32 pCtrl->ItemBufSize PRN_MOD_MGR_ARG_POST
                PRN_MOD_MGR_API
                PRN_MOD_MGR_LOG "      ShadowBufBase : %p"
                    PRN_MOD_MGR_ARG_CPOINT pCtrl->pShadowBuf PRN_MOD_MGR_ARG_POST
                PRN_MOD_MGR_API
                PRN_MOD_MGR_LOG "      ShadowBufSize : 0x%x"
                    PRN_MOD_MGR_ARG_UINT32 pCtrl->ShadowBufSize PRN_MOD_MGR_ARG_POST
                PRN_MOD_MGR_API
                PRN_MOD_MGR_LOG "   ShadowRemainSize : 0x%x"
                    PRN_MOD_MGR_ARG_UINT32 pCtrl->ShadowRemainSize PRN_MOD_MGR_ARG_POST
                PRN_MOD_MGR_API
                PRN_MOD_MGR_LOG "        WorkBufBase : %p"
                    PRN_MOD_MGR_ARG_CPOINT pCtrl->pWorkBuf PRN_MOD_MGR_ARG_POST
                PRN_MOD_MGR_API
                PRN_MOD_MGR_LOG "        WorkBufSize : 0x%x"
                    PRN_MOD_MGR_ARG_UINT32 pCtrl->WorkBufSize PRN_MOD_MGR_ARG_POST
                PRN_MOD_MGR_API

                {
                    UINT32 ItemIdx;

                    PRN_MOD_MGR_LOG "      ModuleVersion : 0x%x"
                        PRN_MOD_MGR_ARG_UINT32 pCtrl->ModuleVersion PRN_MOD_MGR_ARG_POST
                    PRN_MOD_MGR_API
                    PRN_MOD_MGR_LOG "       MinorVersion : 0x%x"
                        PRN_MOD_MGR_ARG_UINT32 pCtrl->MinorVersion PRN_MOD_MGR_ARG_POST
                    PRN_MOD_MGR_API
                    PRN_MOD_MGR_LOG "          NumOfItem : 0x%x"
                        PRN_MOD_MGR_ARG_UINT32 pCtrl->NumOfItem PRN_MOD_MGR_ARG_POST
                    PRN_MOD_MGR_API
                    PRN_MOD_MGR_LOG "              pItem : Addr | State | Version | Module | Minor | ProID | DataBase   | DataSize" PRN_MOD_MGR_API

                    for (ItemIdx = 0U; ItemIdx < SVC_MODULE_ITEM_MAX_NUM; ItemIdx ++) {

                        PRN_MOD_MGR_LOG "                %p |  0x%02x |   0x%02x  |  0x%02x  |  0x%02x |  0x%02x | %p | 0x%08x/0x%08x"
                            PRN_MOD_MGR_ARG_CPOINT &(pCtrl->pItem[ItemIdx])            PRN_MOD_MGR_ARG_POST
                            PRN_MOD_MGR_ARG_UINT32 pCtrl->pItem[ItemIdx].State         PRN_MOD_MGR_ARG_POST
                            PRN_MOD_MGR_ARG_UINT32 pCtrl->pItem[ItemIdx].ItemVersion   PRN_MOD_MGR_ARG_POST
                            PRN_MOD_MGR_ARG_UINT32 pCtrl->pItem[ItemIdx].ModuleVersion PRN_MOD_MGR_ARG_POST
                            PRN_MOD_MGR_ARG_UINT32 pCtrl->pItem[ItemIdx].MinorVersion  PRN_MOD_MGR_ARG_POST
                            PRN_MOD_MGR_ARG_UINT32 pCtrl->pItem[ItemIdx].ProjectID     PRN_MOD_MGR_ARG_POST
                            PRN_MOD_MGR_ARG_CPOINT pCtrl->pItem[ItemIdx].pDataBuf      PRN_MOD_MGR_ARG_POST
                            PRN_MOD_MGR_ARG_UINT32 pCtrl->pItem[ItemIdx].DataBufUsaged PRN_MOD_MGR_ARG_POST
                            PRN_MOD_MGR_ARG_UINT32 pCtrl->pItem[ItemIdx].DataBufSize   PRN_MOD_MGR_ARG_POST
                        PRN_MOD_MGR_API

                    }

                }

                if (pCtrl->ShadowRemainSize < pCtrl->ShadowBufSize) {
                    Modual_Cal_Param_s  *pModuleHeader   = NULL;
                    Modual_Cal_Header_s *pCalHeader      = NULL;
                    UINT32              *pHeaderCheckSum = NULL;
                    UINT32               CalHeaderNum    = 0U;
                    UINT8               *pTemp           = NULL;

                    PRN_MOD_MGR_LOG "---- Svc Storage Module Header ( Shadow Buffer ) ----" PRN_MOD_MGR_API

                    AmbaMisra_TypeCast(&(pModuleHeader), &(pCtrl->pShadowBuf));
                    AmbaMisra_TouchUnused(pModuleHeader);
                    if (pModuleHeader != NULL) {
                        PRN_MOD_MGR_LOG "------ Module Header ----" PRN_MOD_MGR_API
                        PRN_MOD_MGR_LOG "   Version : 0x%016llx"
                            PRN_MOD_MGR_ARG_UINT64 pModuleHeader->Version
                        PRN_MOD_MGR_API
                        PRN_MOD_MGR_LOG "  ModuleID : 0x%08x"
                            PRN_MOD_MGR_ARG_UINT32 pModuleHeader->ModuleID PRN_MOD_MGR_ARG_POST
                        PRN_MOD_MGR_API

                        CalHeaderNum = (UINT32)((pModuleHeader->Version & 0x00FF000000000000U) >> 48U);
                        pTemp = &(pCtrl->pShadowBuf[(UINT32)(sizeof(Modual_Cal_Param_s))]);
                        AmbaMisra_TypeCast(&(pCalHeader), &(pTemp)); AmbaMisra_TouchUnused(pCalHeader);
                        pTemp = &(pTemp[((UINT32)(sizeof(Modual_Cal_Header_s)) * CalHeaderNum)]);
                        AmbaMisra_TypeCast(&(pHeaderCheckSum), &(pTemp));
                    }

                    if (pCalHeader != NULL) {
                        UINT32 ItemIdx;

                        for (ItemIdx = 0U; ItemIdx < CalHeaderNum; ItemIdx ++) {
                            PRN_MOD_MGR_LOG "------ No.%02d Cal Item Header %p ----"
                                PRN_MOD_MGR_ARG_UINT32 ItemIdx                PRN_MOD_MGR_ARG_POST
                                PRN_MOD_MGR_ARG_CPOINT &(pCalHeader[ItemIdx]) PRN_MOD_MGR_ARG_POST
                            PRN_MOD_MGR_API
                            PRN_MOD_MGR_LOG "   Version : 0x%08x"
                                PRN_MOD_MGR_ARG_UINT32 pCalHeader[ItemIdx].Version PRN_MOD_MGR_ARG_POST
                            PRN_MOD_MGR_API
                            PRN_MOD_MGR_LOG "      Size : 0x%08x"
                                PRN_MOD_MGR_ARG_UINT32 pCalHeader[ItemIdx].Size PRN_MOD_MGR_ARG_POST
                            PRN_MOD_MGR_API
                            PRN_MOD_MGR_LOG "  CheckSum : 0x%08x"
                                PRN_MOD_MGR_ARG_UINT32 pCalHeader[ItemIdx].CheckSum PRN_MOD_MGR_ARG_POST
                            PRN_MOD_MGR_API
                        }
                    }

                    if (pHeaderCheckSum != NULL) {
                        PRN_MOD_MGR_LOG "------ Module Header CheckSum 0x%08x"
                            PRN_MOD_MGR_ARG_UINT32 *pHeaderCheckSum PRN_MOD_MGR_ARG_POST
                        PRN_MOD_MGR_API
                    }
                    AmbaMisra_TouchUnused(pHeaderCheckSum);
                }

                SvcModule_MutexGive(&(pCtrl->Mutx));
            }
        }

        AmbaMisra_TouchUnused(pModuleCtrl);
    }
}

/**
 * Print all hook storage module information
 *
 */
void SvcModule_InfoAll(void)
{
    SVC_MODULE_CTRL_s *pCtrl = pSvcModuleCtrlRoot;

    while (pCtrl != NULL) {
        SvcModule_Info(pCtrl);
        AmbaMisra_TypeCast(&(pCtrl), &(pCtrl->pNextCtrl));
    }
}

/**
 * Enable/Disable storage module debug message
 *
 * @param [in] Enable Enable/Disable debug msg
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
void SvcModule_DebugMsg(UINT32 Enable)
{
    if (Enable > 0U) {
        SvcModuleCtrlFlag |= SVC_LOG_CAL_DBG;
    } else {
        SvcModuleCtrlFlag &= ~SVC_LOG_CAL_DBG;
    }
}


