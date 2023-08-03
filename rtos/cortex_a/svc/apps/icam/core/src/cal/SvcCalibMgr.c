/**
*  @file SvcCalibMgr.c
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
*  @details C file for Calibration Manager control
*
*/

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaMisraFix.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaSYS.h"
#include "AmbaNVM_Partition.h"
#if defined(CONFIG_ENABLE_NAND_BOOT) || defined(CONFIG_ENABLE_SPINAND_BOOT)
#include "AmbaNAND.h"
#include "AmbaNAND_FTL.h"
#endif
#include "AmbaFS.h"
#include "AmbaDSP.h"
#include "AmbaDSP_ImageDefine.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaSvcWrap.h"

#include "SvcWrap.h"
#include "SvcErrCode.h"
#include "SvcNvm.h"
#include "SvcLog.h"
#include "SvcCalibMgr.h"

#define SVC_CALIB_MGR_FLG_MEM_INIT       (0x1U)
#define SVC_CALIB_MGR_FLG_NVM_INIT       (0x2U)
#define SVC_CALIB_MGR_FLG_SHELL_INIT     (0x1000U)
/* Reserve 0x10000/0x20000/0x40000/0x80000 for SvcCalibMgr.h */

#define SVC_CALIB_NAND_SECTOR_SIZE  (512U)
#define SVC_CALIB_SCRIPT_MAX_LENGTH (1024U)
#define SVC_CALIB_SCRIPT_MAX_PARAM  (70U)

typedef struct {
    UINT32 ID;
    UINT32 Offset;
    UINT32 Size;
    UINT32 CheckSum;
} SVC_CALIB_ITEM_HEADER_s;

typedef struct {
#define AMBA_SVC_CALIB_VERSION   (0x20180910U)
    UINT32                       Version;                           // 4            bytes
    UINT32                       NumOfItem;                         // 4            bytes
    SVC_CALIB_ITEM_HEADER_s      ItemInfo[SVC_CALIB_MAX_OBJ_NUM];   // 264, 16 x 16 bytes
    UINT32                       Reserved[61];                      // 508, 244     bytes
    UINT32                       CheckSum;                          // 512          bytes. One NAND sector.
} SVC_CALIB_NAND_HEADER_s;

typedef struct {
#define SVC_CALIB_OBJ_INIT         (0x1U)
#define SVC_CALIB_OBJ_LOAD_DONE    (0x4U)
#define SVC_CALIB_OBJ_SHADOW_INIT  (0x8U)
    UINT32                Status;
    AMBA_KAL_MUTEX_t      Mutex;
    SVC_CALIB_OBJ_s       CalObj;
} SVC_CALIB_OBJ_CTRL_s;

typedef struct {
#define SVC_CALIB_CTRL_CALC_MEM_INIT    (0x1U)
#define SVC_CALIB_CTRL_CALC_MEM_LOCK    (0x2U)
/* bit31 ~ bit2 reserved for locked calib id */
    UINT32                    State;
    UINT8                    *pBuf;
    UINT32                    BufSize;
    SVC_CALIB_CALC_MEM_INFO_s LockMemInfo;
} SVC_CALIB_CALC_MEM_CTRL_s;

typedef struct {
#define SVC_CALIB_CTRL_INIT         (0x1U)
#define SVC_CALIB_CTRL_LOAD_HEADER  (0x2U)
    UINT32                       Status;
    UINT32                       NumOfCalObj;
    SVC_CALIB_OBJ_CTRL_s         CalObjCtrl[SVC_CALIB_MAX_OBJ_NUM];
    SVC_CALIB_CALC_MEM_CTRL_s    CalcMemCtrl;
    SVC_CALIB_CALC_CB_f          CalcCbFunc;
    SVC_CALIB_NAND_HEADER_s      CalHeader;
    AMBA_NVM_FTL_INFO_s          CalNandInfo;
} SVC_CALIB_CTRL_s;

static UINT32                SvcCalibMgr_MutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut);
static void                  SvcCalibMgr_MutexGive(AMBA_KAL_MUTEX_t *pMutex);
static void                  SvcCalibMgr_ErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode);

static UINT32                SvcCalibMgr_ObjCtrlCreate(SVC_CALIB_OBJ_s *pObj, SVC_CALIB_OBJ_CTRL_s *pObjCtrl);
static SVC_CALIB_OBJ_CTRL_s *SvcCalibMgr_ObjCtrlGet(UINT32 CalID);
static void                  SvcCalibMgr_NvmInit(void);
static UINT32                SvcCalibMgr_NvmLoadInfo(AMBA_NVM_FTL_INFO_s *pCalInfo);
static UINT32                SvcCalibMgr_NvmLoadHeader(SVC_CALIB_NAND_HEADER_s *pCalHeader);
static UINT32                SvcCalibMgr_NvmLoadData(const SVC_CALIB_OBJ_s *pCalObj);
static UINT32                SvcCalibMgr_NvmSaveHeader(const SVC_CALIB_NAND_HEADER_s *pCalHeader);
static UINT32                SvcCalibMgr_NvmSaveData(const SVC_CALIB_OBJ_s *pCalObj);
static UINT32                SvcCalibMgr_NvmHeaderConfig(UINT32 CalID, SVC_CALIB_NAND_HEADER_s *pCalHeader, UINT32 DataSize);
static UINT32                SvcCalibMgr_RomLoadData(const SVC_CALIB_OBJ_s *pCalObj);
static UINT32                SvcCalibMgr_FileLoadData(const SVC_CALIB_OBJ_s *pCalObj);
static UINT32                SvcCalibMgr_FileSaveData(const SVC_CALIB_OBJ_s *pCalObj);
static UINT32                SvcCalibMgr_ScriptRun(char *pFilePath);
static UINT32                SvcCalibMgr_ScriptReadLine(AMBA_FS_FILE *pFile, char *pLineBuf, UINT32 LineBufSize, UINT32 *pLength);
static UINT32                SvcCalibMgr_ScriptCmdLineCheck(char *pLineBuf);
static UINT32                SvcCalibMgr_ScriptCmdParser(char *pLineBuf);
static void                  SvcCalibMgr_ScriptMPause(UINT32 DelayTime);
static UINT32                SvcCalibMgr_CalcMemSizeGet(UINT32 *pBufSize);

static void   SvcCalib_Lz77Search(const UINT8 *pSearchBuf, UINT32 SearchSize, const UINT8 *pLookHeadBuf, UINT32 LookHeadSize, UINT32 *pOffset, UINT32 *pLength);
static UINT32 SvcCalib_Lz77Compress(UINT32 MaxSearchSize, const void *pSrc, UINT32 SrcSize, void *pDst, UINT32 DstSize, UINT32 *pCmprSize);
static UINT32 SvcCalib_Lz77Decompress(const void *pSrc, UINT32 SrcSize, void *pDst, UINT32 DstSize, UINT32 *pDeCmprSize);

typedef UINT32 (*SVC_CALIB_MGR_SHELL_FUNC_f)(UINT32 ArgCount, char * const *pArgVector);
typedef void   (*SVC_CALIB_MGR_SHELL_USAGE_f)(void);
typedef struct {
    UINT32                      Enable;
    char                        ShellCmdName[32];
    SVC_CALIB_MGR_SHELL_FUNC_f  pFunc;
    SVC_CALIB_MGR_SHELL_USAGE_f pUsage;
} SVC_CALIB_MGR_SHELL_FUNC_s;

static void   SvcCalibMgr_ShellStrToU32(const char *pString, UINT32 *pValue);
static UINT32 SvcCalibMgr_ShellInfo(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalibMgr_ShellInfoU(void);
static UINT32 SvcCalibMgr_ShellNand(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalibMgr_ShellNandU(void);
static UINT32 SvcCalibMgr_ShellScriptRun(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalibMgr_ShellScriptRunU(void);
static UINT32 SvcCalibMgr_ShellMemHook(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalibMgr_ShellMemHookU(void);
static UINT32 SvcCalibMgr_ShellDbgMsg(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalibMgr_ShellDbgMsgU(void);
static void   SvcCalibMgr_ShellEntryInit(void);
static void   SvcCalibMgr_CmdAppUsage(void);

#define SVC_CALIB_MGR_SHELL_CMD_NUM  (5U)
static SVC_CALIB_MGR_SHELL_FUNC_s CalibMgrShellFunc[SVC_CALIB_MGR_SHELL_CMD_NUM] GNU_SECTION_NOZEROINIT;

static UINT32 SvcCalibMgr_ShellItemObjInit(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalibMgr_ShellItemObjInitU(void);
static UINT32 SvcCalibMgr_ShellItemObjSave(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalibMgr_ShellItemObjSaveU(void);
static UINT32 SvcCalibMgr_ShellItemObjLoad(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalibMgr_ShellItemObjLoadU(void);
static UINT32 SvcCalibMgr_ShellItemObjReset(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCalibMgr_ShellItemObjResetU(void);

#define SVC_CALIB_MGR_SHELL_NAND_NUM  (4U)
static SVC_CALIB_MGR_SHELL_FUNC_s CalibMgrNandShellFunc[SVC_CALIB_MGR_SHELL_NAND_NUM] GNU_SECTION_NOZEROINIT;
static SVC_CALIB_CTRL_s SVC_CalibMgr GNU_SECTION_NOZEROINIT;
static UINT32 SVC_CalibMgrFlag = SVC_LOG_CAL_DEF_FLG;

#define SVC_LOG_CAL "CAL"
#define PRN_CAL_MGR_LOG        { SVC_WRAP_PRINT_s CalibMgrPrint; AmbaSvcWrap_MisraMemset(&(CalibMgrPrint), 0, sizeof(CalibMgrPrint)); CalibMgrPrint.Argc --; CalibMgrPrint.pStrFmt =
#define PRN_CAL_MGR_ARG_UINT64 ; CalibMgrPrint.Argc ++; CalibMgrPrint.Argv[CalibMgrPrint.Argc].Uint64   =
#define PRN_CAL_MGR_ARG_UINT32 ; CalibMgrPrint.Argc ++; CalibMgrPrint.Argv[CalibMgrPrint.Argc].Uint64   = (UINT64)((
#define PRN_CAL_MGR_ARG_CSTR   ; CalibMgrPrint.Argc ++; CalibMgrPrint.Argv[CalibMgrPrint.Argc].pCStr    = ((
#define PRN_CAL_MGR_ARG_CPOINT ; CalibMgrPrint.Argc ++; CalibMgrPrint.Argv[CalibMgrPrint.Argc].pPointer = ((
#define PRN_CAL_MGR_ARG_POST   ))
#define PRN_CAL_MGR_OK         ; CalibMgrPrint.Argc ++; SvcCalib_PrintLog(SVC_LOG_CAL_OK , &(CalibMgrPrint)); }
#define PRN_CAL_MGR_NG         ; CalibMgrPrint.Argc ++; SvcCalib_PrintLog(SVC_LOG_CAL_NG , &(CalibMgrPrint)); }
#define PRN_CAL_MGR_API        ; CalibMgrPrint.Argc ++; SvcCalib_PrintLog(SVC_LOG_CAL_API, &(CalibMgrPrint)); }
#define PRN_CAL_MGR_DBG        ; CalibMgrPrint.Argc ++; SvcCalib_PrintLog(SVC_LOG_CAL_DBG, &(CalibMgrPrint)); }
#define PRN_CAL_MGR_ERR_HDLR   SvcCalibMgr_ErrHdlr(__func__, __LINE__, PRetVal);

static void SvcCalib_PrintLog(UINT32 LogLevel, SVC_WRAP_PRINT_s *pPrint)
{
    if (pPrint != NULL) {
        if ((SVC_CalibMgrFlag & LogLevel) > 0U) {

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

            SvcWrap_Print(SVC_LOG_CAL, pPrint);
        }
    }
}

static UINT32 SvcCalibMgr_MutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut)
{
    return AmbaKAL_MutexTake(pMutex, TimeOut);
}

static void SvcCalibMgr_MutexGive(AMBA_KAL_MUTEX_t *pMutex)
{
    UINT32 ErrCode = AmbaKAL_MutexGive(pMutex);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }
}

static void SvcCalibMgr_ErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode)
{
    if (pCaller != NULL) {
        if (ErrCode != 0U) {
            PRN_CAL_MGR_LOG "Catch ErrCode(0x%08X) @ %s, %d"
                PRN_CAL_MGR_ARG_UINT32 ErrCode  PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_ARG_CSTR   pCaller  PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_ARG_UINT32 CodeLine PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        }
    }
}

static UINT32 SvcCalibMgr_ObjCtrlCreate(SVC_CALIB_OBJ_s *pObj, SVC_CALIB_OBJ_CTRL_s *pObjCtrl)
{
    UINT32 RetVal = SVC_OK;

    if (pObj == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to create calib object control - input calib object should not null!" PRN_CAL_MGR_NG
    } else if (pObj->ID == 0xFFFFFFFFU) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to create calib object control - invalid calib id(%d)"
            PRN_CAL_MGR_ARG_UINT32 pObj->ID PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_NG
    } else if (pObj->ShadowBufSize == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to create calib object control - object dram shadow size should not zero!" PRN_CAL_MGR_NG
    } else if (pObj->pShadowBuf == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to create calib object control - object dram shadow should not null!" PRN_CAL_MGR_NG
    } else if (pObj->CmdFunc == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to create calib object control - calib id(%d) command func should not null!"
            PRN_CAL_MGR_ARG_UINT32 pObj->ID PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_NG
    } else if (pObjCtrl == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to create calib object control - output calib object control should not null!" PRN_CAL_MGR_NG
    } else {

        /* If the output calib object control has been initialized, reset it first. */
        if ((pObjCtrl->Status & SVC_CALIB_OBJ_INIT) > 0U) {
            /* mutex give */
            SvcCalibMgr_MutexGive(&(pObjCtrl->Mutex));

            /* mutex delete */
            if (0U != AmbaKAL_MutexDelete(&(pObjCtrl->Mutex))) {
                PRN_CAL_MGR_LOG "Fail to create calib object control - delete mutex fail" PRN_CAL_MGR_NG
            }
        }

        AmbaSvcWrap_MisraMemset(pObjCtrl, 0, sizeof(SVC_CALIB_OBJ_CTRL_s));
        AmbaSvcWrap_MisraMemcpy(&(pObjCtrl->CalObj), pObj, sizeof(SVC_CALIB_OBJ_s));

        RetVal = AmbaKAL_MutexCreate(&(pObjCtrl->Mutex), pObj->Name);
        if (RetVal != SVC_OK) {
            PRN_CAL_MGR_LOG "Fail to create calib object control - create calib id(%d) mutex fail!"
                PRN_CAL_MGR_ARG_UINT32 pObj->ID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        } else {
            pObjCtrl->Status = SVC_CALIB_OBJ_INIT;
            PRN_CAL_MGR_LOG "Successful to create calib object control - calib id(%d)"
                PRN_CAL_MGR_ARG_UINT32 pObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_DBG
        }
    }

    return RetVal;
}


static SVC_CALIB_OBJ_CTRL_s *SvcCalibMgr_ObjCtrlGet(UINT32 CalID)
{
    SVC_CALIB_OBJ_CTRL_s *pObjCtrl = NULL;

    if ((SVC_CalibMgr.Status & SVC_CALIB_CTRL_INIT) > 0U) {
        UINT32 ObjIdx;

        for (ObjIdx = 0U; ObjIdx < SVC_CalibMgr.NumOfCalObj; ObjIdx ++) {
            if (SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.ID == CalID) {
                pObjCtrl = &(SVC_CalibMgr.CalObjCtrl[ObjIdx]);
            }
        }
    }

    return pObjCtrl;
}

static void SvcCalibMgr_NvmInit(void)
{
    if ((SVC_CalibMgrFlag & SVC_CALIB_MGR_FLG_NVM_INIT) == 0U) {
        SVC_CalibMgrFlag |= SVC_CALIB_MGR_FLG_NVM_INIT;

#if defined(CONFIG_ENABLE_NAND_BOOT) || defined(CONFIG_ENABLE_SPINAND_BOOT)
        {
            UINT32 ErrCode = AmbaNAND_FtlInit(AMBA_USER_PARTITION_CALIBRATION_DATA);
            if (ErrCode != NAND_ERR_NONE) {
                PRN_CAL_MGR_LOG "Initial the calibration nand partition fail! ErrCode(0x%08X)"
                    PRN_CAL_MGR_ARG_UINT32 ErrCode PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_NG
            }
        }
#endif
    }
}

static UINT32 SvcCalibMgr_NvmLoadInfo(AMBA_NVM_FTL_INFO_s *pCalInfo)
{
    UINT32 RetVal = SVC_OK;

    if (pCalInfo == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to get the calib nand info - output info should not null!" PRN_CAL_MGR_NG
    } else {
        AmbaSvcWrap_MisraMemset(pCalInfo, 0, sizeof(AMBA_NVM_FTL_INFO_s));
        if ( NVM_ERR_NONE != SvcNvm_GetFtlInfo(AMBA_USER_PARTITION_CALIBRATION_DATA, pCalInfo) ) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to load calib nand info - get the calib nvm ftl info fail!" PRN_CAL_MGR_NG
        }
    }

    return RetVal;
}


static UINT32 SvcCalibMgr_NvmLoadHeader(SVC_CALIB_NAND_HEADER_s *pCalHeader)
{
    UINT32 RetVal = SVC_OK;

    if (pCalHeader == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to load calib header from NAND - input calib header should not null!" PRN_CAL_MGR_NG
    } else {
        AMBA_NVM_SECTOR_CONFIG_s NvmSectorCfg;
        UINT8  TempBuf[SVC_CALIB_NAND_SECTOR_SIZE];
        UINT32 Idx;

        PRN_CAL_MGR_LOG "Prepare temp buffer to load the nand header. size 0x%X"
            PRN_CAL_MGR_ARG_UINT32 (UINT32)(sizeof(TempBuf)) PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_DBG

        /* Reset the temp buffer */
        AmbaSvcWrap_MisraMemset(TempBuf, 0, sizeof(TempBuf));

        /* Read the calib header */
        AmbaSvcWrap_MisraMemset(&NvmSectorCfg, 0, sizeof(NvmSectorCfg));
        NvmSectorCfg.StartSector = 0;
        NvmSectorCfg.NumSector   = (UINT32)(sizeof(TempBuf)) / SVC_CALIB_NAND_SECTOR_SIZE;
        NvmSectorCfg.pDataBuf    = TempBuf;
        RetVal = SvcNvm_ReadSector(AMBA_USER_PARTITION_CALIBRATION_DATA, &NvmSectorCfg, 2000);
        if (RetVal != NVM_ERR_NONE) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to load calib header from NAND - load the calib header fail!" PRN_CAL_MGR_NG
        } else {
            const UINT8 *pDataBuf = TempBuf;
            const SVC_CALIB_NAND_HEADER_s *pTempHeader;
            AmbaMisra_TypeCast(&(pTempHeader), &(pDataBuf));
            if (pTempHeader != NULL) {
                if (pTempHeader->Version != (UINT32)AMBA_SVC_CALIB_VERSION) {
                    PRN_CAL_MGR_LOG "The header version does not match!" PRN_CAL_MGR_DBG
                } else {
                    UINT32 NandHeaderCrc = AmbaUtility_Crc32(pDataBuf, (UINT32)(sizeof(SVC_CALIB_NAND_HEADER_s) - sizeof(UINT32)));
                    /* CRC check - calib nand header */
                    if (pTempHeader->CheckSum != NandHeaderCrc) {
                        RetVal = SVC_NG;
                        PRN_CAL_MGR_LOG "Fail to load calib header from NAND - the header CRC is incorrect! 0x%X != 0x%X"
                            PRN_CAL_MGR_ARG_UINT32 pTempHeader->CheckSum PRN_CAL_MGR_ARG_POST
                            PRN_CAL_MGR_ARG_UINT32 NandHeaderCrc         PRN_CAL_MGR_ARG_POST
                        PRN_CAL_MGR_NG
                    } else {
                        AmbaSvcWrap_MisraMemset(pCalHeader, 0, sizeof(SVC_CALIB_NAND_HEADER_s));
                        pCalHeader->Version   = pTempHeader->Version;
                        pCalHeader->NumOfItem = pTempHeader->NumOfItem;
                        pCalHeader->CheckSum  = pTempHeader->CheckSum;
                        AmbaSvcWrap_MisraMemcpy(pCalHeader->ItemInfo, pTempHeader->ItemInfo, sizeof(pCalHeader->ItemInfo));

                        PRN_CAL_MGR_LOG "Successful to load header from NAND. %d"
                            PRN_CAL_MGR_ARG_UINT32 pCalHeader->Reserved[0] PRN_CAL_MGR_ARG_POST
                        PRN_CAL_MGR_OK
                    }
                }
            }
        }

        if ((SVC_CalibMgrFlag & SVC_LOG_CAL_DBG) > 0U) {
            UINT32 ObjIdx;
            static char ObjName[16];

            PRN_CAL_MGR_LOG " " PRN_CAL_MGR_API
            PRN_CAL_MGR_LOG "---- Calibration NAND header ----" PRN_CAL_MGR_API
            PRN_CAL_MGR_LOG "  Version    : 0x%08X"
                PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalHeader.Version PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_API
            PRN_CAL_MGR_LOG "  NumOfItem  : %d"
                PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalHeader.NumOfItem PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_API
            PRN_CAL_MGR_LOG "  CheckSum   : 0x%08X"
                PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalHeader.CheckSum PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_API
            PRN_CAL_MGR_LOG "  Item Index : ID Offset     Size       CheckSum   Comment" PRN_CAL_MGR_API
            for (Idx = 0U; Idx < SVC_CALIB_MAX_OBJ_NUM; Idx ++) {

                AmbaSvcWrap_MisraMemset(ObjName, 0, sizeof(ObjName));
                for (ObjIdx = 0U; ObjIdx < SVC_CalibMgr.NumOfCalObj; ObjIdx ++) {
                    if (SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.ID == SVC_CalibMgr.CalHeader.ItemInfo[Idx].ID) {
                        SvcWrap_strcpy(ObjName, sizeof(ObjName), SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.Name);
                    }
                }
                if (0U == SvcWrap_strlen(ObjName)) {
                    ObjName[0] = 'I';
                    ObjName[1] = 'n';
                    ObjName[2] = 'v';
                    ObjName[3] = 'a';
                    ObjName[4] = 'l';
                    ObjName[5] = 'i';
                    ObjName[6] = 'd';
                    ObjName[7] = '\0';
                }

                PRN_CAL_MGR_LOG "  No.%02d Item : %02d 0x%08X 0x%08X 0x%08X %s"
                    PRN_CAL_MGR_ARG_UINT32 Idx                                           PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalHeader.ItemInfo[Idx].ID       PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalHeader.ItemInfo[Idx].Offset   PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalHeader.ItemInfo[Idx].Size     PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalHeader.ItemInfo[Idx].CheckSum PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CSTR   ObjName                                       PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_API
            }
        }
    }

    return RetVal;
}


static UINT32 SvcCalibMgr_NvmLoadData(const SVC_CALIB_OBJ_s *pCalObj)
{
    UINT32 RetVal = SVC_OK;

    if (pCalObj == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to load data from NAND - input calib object should not null!" PRN_CAL_MGR_NG
    } else if (pCalObj->DataSource != SVC_CALIB_DATA_SOURCE_NAND) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to load data from NAND - not support data source(%d)"
            PRN_CAL_MGR_ARG_UINT32 pCalObj->DataSource PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_NG
    } else if (pCalObj->ShadowBufSize == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to load data from NAND - dram shadow size should not zero!" PRN_CAL_MGR_NG
    } else if (pCalObj->pShadowBuf == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to load data from NAND - dram shadow should not null!" PRN_CAL_MGR_NG
    } else if ((pCalObj->ID == SVC_CALIB_EMPTY) || (pCalObj->ID > SVC_CALIB_MAX_OBJ_NUM)) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to load data from NAND - calib id(%d) is out-of range!"
            PRN_CAL_MGR_ARG_UINT32 pCalObj->ID PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_NG
    } else {
        AMBA_NVM_SECTOR_CONFIG_s NvmSectorCfg;
        const SVC_CALIB_ITEM_HEADER_s *pCalItemHeader = &(SVC_CalibMgr.CalHeader.ItemInfo[pCalObj->ID - 1U]);

        if (pCalItemHeader->ID != pCalObj->ID) {
            RetVal = SVC_OK;
            PRN_CAL_MGR_LOG "The calib item id(%d) data is empty! Calib id(%d)"
                PRN_CAL_MGR_ARG_UINT32 pCalItemHeader->ID PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_ARG_UINT32 pCalObj->ID        PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_DBG
        } else {

            /* Get the calib item info by calib id */
            UINT8 *pShadowBuf;
            AmbaMisra_TypeCast(&(pShadowBuf), &(pCalObj->pShadowBuf));

            AmbaSvcWrap_MisraMemset(pShadowBuf, 0, pCalObj->ShadowBufSize);

            AmbaSvcWrap_MisraMemset(&NvmSectorCfg, 0, sizeof(NvmSectorCfg));
            NvmSectorCfg.StartSector = GetAlignedValU32(pCalItemHeader->Offset, SVC_CALIB_NAND_SECTOR_SIZE) / SVC_CALIB_NAND_SECTOR_SIZE;
            NvmSectorCfg.NumSector   = GetAlignedValU32(pCalItemHeader->Size, SVC_CALIB_NAND_SECTOR_SIZE) / SVC_CALIB_NAND_SECTOR_SIZE;
            NvmSectorCfg.pDataBuf    = pShadowBuf;
            RetVal = SvcNvm_ReadSector(AMBA_USER_PARTITION_CALIBRATION_DATA, &NvmSectorCfg, 10000);
            if (RetVal != NVM_ERR_NONE) {
                RetVal = SVC_NG;

                PRN_CAL_MGR_LOG "Fail to load data from NAND - load calib(%d) data fail! Offset: 0x%X, Size(0x%08X)"
                    PRN_CAL_MGR_ARG_UINT32 pCalObj->ID              PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 NvmSectorCfg.StartSector PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 NvmSectorCfg.NumSector   PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_NG
            } else {
                /* CRC check - calib data */
                if (pCalItemHeader->CheckSum != AmbaUtility_Crc32(pShadowBuf, pCalObj->ShadowBufSize)) {
                    RetVal = SVC_NG;
                    PRN_CAL_MGR_LOG "Fail to load data from NAND - the calib data check sum is incorrect!" PRN_CAL_MGR_NG
                }
            }

            if (RetVal == SVC_OK) {
                PRN_CAL_MGR_LOG "Successful to load data from NAND. calib id(%d)"
                    PRN_CAL_MGR_ARG_UINT32 pCalObj->ID PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_OK
            }
        }
    }

    return RetVal;
}


static UINT32 SvcCalibMgr_NvmSaveHeader(const SVC_CALIB_NAND_HEADER_s *pCalHeader)
{
    UINT32 RetVal = SVC_OK;

    if (pCalHeader == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to load calib header from NAND - input calib header should not null!" PRN_CAL_MGR_NG
    } else {
        UINT8 CalHeaderBuf[SVC_CALIB_NAND_SECTOR_SIZE];
        AMBA_NVM_SECTOR_CONFIG_s NvmSectorCfg;

        /* Configure the calib header context */
        AmbaSvcWrap_MisraMemset(CalHeaderBuf, 0, sizeof(CalHeaderBuf));
        AmbaSvcWrap_MisraMemcpy(CalHeaderBuf, pCalHeader, sizeof(SVC_CALIB_NAND_HEADER_s));

        /* Configure NAND sector configuration */
        AmbaSvcWrap_MisraMemset(&NvmSectorCfg, 0, sizeof(NvmSectorCfg));
        NvmSectorCfg.StartSector = 0;
        NvmSectorCfg.NumSector   = GetAlignedValU32((UINT32)(sizeof(CalHeaderBuf)), SVC_CALIB_NAND_SECTOR_SIZE) / SVC_CALIB_NAND_SECTOR_SIZE;
        NvmSectorCfg.pDataBuf    = CalHeaderBuf;
        RetVal = SvcNvm_WriteSector(AMBA_USER_PARTITION_CALIBRATION_DATA, &NvmSectorCfg, 2000);
        if (RetVal != NVM_ERR_NONE) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to save calib header to NAND - write the header fail!" PRN_CAL_MGR_NG
        } else {
            PRN_CAL_MGR_LOG "Successful to save calib header" PRN_CAL_MGR_OK
        }
    }

    return RetVal;
}


static UINT32 SvcCalibMgr_NvmSaveData(const SVC_CALIB_OBJ_s *pCalObj)
{
    UINT32 RetVal = SVC_OK;

    if (pCalObj == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to save data to NAND - input calib object should not null!" PRN_CAL_MGR_NG
    } else if (pCalObj->DataSource != SVC_CALIB_DATA_SOURCE_NAND) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to save data to NAND - not support data source(%d)"
            PRN_CAL_MGR_ARG_UINT32 pCalObj->DataSource PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_NG
    } else if ((pCalObj->ID == SVC_CALIB_EMPTY) || (pCalObj->ID > SVC_CALIB_MAX_OBJ_NUM)) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to save data to NAND - calib id(%d) is out-of range!"
            PRN_CAL_MGR_ARG_UINT32 pCalObj->ID PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_NG
    } else {
        SVC_CALIB_NAND_HEADER_s *pCalHeader = &(SVC_CalibMgr.CalHeader), TmpHeader;
        SVC_CALIB_ITEM_HEADER_s *pItemHeader = &(pCalHeader->ItemInfo[pCalObj->ID - 1U]);
        const AMBA_NVM_FTL_INFO_s *pCalNandInfo = &(SVC_CalibMgr.CalNandInfo);

        /* Configure NAND header */
        AmbaSvcWrap_MisraMemset(&TmpHeader, 0, sizeof(TmpHeader));
        AmbaSvcWrap_MisraMemcpy(&TmpHeader, pCalHeader, sizeof(SVC_CALIB_NAND_HEADER_s));
        RetVal = SvcCalibMgr_NvmHeaderConfig(pCalObj->ID, &TmpHeader, pCalObj->ShadowBufSize);
        if (RetVal != SVC_OK) {
            PRN_CAL_MGR_LOG "Fail to save data to NAND - configure nand header fail!" PRN_CAL_MGR_NG
        } else {
            AMBA_NVM_SECTOR_CONFIG_s NvmSectorCfg;
            UINT8 *pShadowBuf;
            UINT8 *pCalHeaderBuf = NULL;

            AmbaSvcWrap_MisraMemset(&NvmSectorCfg, 0, sizeof(NvmSectorCfg));
            AmbaSvcWrap_MisraMemcpy(pCalHeader, &TmpHeader, sizeof(SVC_CALIB_NAND_HEADER_s));

            AmbaMisra_TypeCast(&(pShadowBuf), &(pCalObj->pShadowBuf));
            AmbaMisra_TypeCast(&(pCalHeaderBuf), &(pCalHeader));

            pItemHeader->CheckSum = (UINT32)AmbaUtility_Crc32(pShadowBuf, pCalObj->ShadowBufSize);
            pCalHeader->CheckSum = (UINT32)(AmbaUtility_Crc32(pCalHeaderBuf, (UINT32)(sizeof(SVC_CALIB_NAND_HEADER_s) - sizeof(UINT32))));

            PRN_CAL_MGR_LOG "Update the calib id(%d) to NAND. Header crc 0x%08X, Data crc 0x%08X, Offset 0x%X, Size 0x%X"
                PRN_CAL_MGR_ARG_UINT32 pCalObj->ID           PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_ARG_UINT32 pCalHeader->CheckSum  PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_ARG_UINT32 pItemHeader->CheckSum PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_ARG_UINT32 pItemHeader->Offset   PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_ARG_UINT32 pItemHeader->Size     PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_API

            NvmSectorCfg.StartSector = GetAlignedValU32(pItemHeader->Offset, SVC_CALIB_NAND_SECTOR_SIZE) / SVC_CALIB_NAND_SECTOR_SIZE;
            NvmSectorCfg.NumSector   = GetAlignedValU32(pItemHeader->Size, SVC_CALIB_NAND_SECTOR_SIZE) / SVC_CALIB_NAND_SECTOR_SIZE;
            NvmSectorCfg.pDataBuf    = pShadowBuf;
            RetVal = SvcNvm_WriteSector(AMBA_USER_PARTITION_CALIBRATION_DATA, &NvmSectorCfg, 2000);

            if (RetVal != NVM_ERR_NONE) {
                RetVal = SVC_NG;

                PRN_CAL_MGR_LOG "Fail to save data to NAND - write to calib NAND fail, Sector offset: 0x%X, Sector size: 0x%X, BufAddr: %p BufSize: 0x%X"
                    PRN_CAL_MGR_ARG_UINT32 NvmSectorCfg.StartSector PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 NvmSectorCfg.NumSector   PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CPOINT pCalObj->pShadowBuf      PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 pCalObj->ShadowBufSize   PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_NG

                PRN_CAL_MGR_LOG " " PRN_CAL_MGR_DBG
                PRN_CAL_MGR_LOG "==== %sCalibration FTL information%s ===="
                    PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_0 PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_DBG
                PRN_CAL_MGR_LOG "  Initialized        : %d"
                    PRN_CAL_MGR_ARG_UINT32 pCalNandInfo->Initialized PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_DBG
                PRN_CAL_MGR_LOG "  WriteProtect       : %d"
                    PRN_CAL_MGR_ARG_UINT32 pCalNandInfo->WriteProtect PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_DBG
                PRN_CAL_MGR_LOG "  TotalSizeInSectors : %d"
                    PRN_CAL_MGR_ARG_UINT32 pCalNandInfo->TotalSizeInSectors PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_DBG
                PRN_CAL_MGR_LOG "  TotalSizeInBytes   : %llu"
                    PRN_CAL_MGR_ARG_UINT64 pCalNandInfo->TotalSizeInBytes
                PRN_CAL_MGR_DBG
                PRN_CAL_MGR_LOG "  PageSizeInBytes    : %d"
                    PRN_CAL_MGR_ARG_UINT32 pCalNandInfo->PageSizeInBytes PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_DBG
                PRN_CAL_MGR_LOG "  BlockSizeInPages   : %d"
                    PRN_CAL_MGR_ARG_UINT32 pCalNandInfo->BlockSizeInPages PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_DBG
                PRN_CAL_MGR_LOG "  EraseSizeInBlocks  : %d"
                    PRN_CAL_MGR_ARG_UINT32 pCalNandInfo->EraseSizeInBlocks PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_DBG
                PRN_CAL_MGR_LOG "  SectorSizeInBytes  : %d"
                    PRN_CAL_MGR_ARG_UINT32 pCalNandInfo->SectorSizeInBytes PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_DBG

            } else {
                PRN_CAL_MGR_LOG "Successful to save data to NAND" PRN_CAL_MGR_OK
                RetVal = SvcCalibMgr_NvmSaveHeader(pCalHeader);
                if (RetVal != SVC_OK) {
                    PRN_CAL_MGR_LOG "Fail to save data to NAND - update the calib header fail!" PRN_CAL_MGR_NG
                }
            }
            AmbaMisra_TouchUnused(pCalHeaderBuf);
        }
    }

    return RetVal;
}


static UINT32 SvcCalibMgr_NvmHeaderConfig(UINT32 CalID, SVC_CALIB_NAND_HEADER_s *pCalHeader, UINT32 DataSize)
{
    UINT32 RetVal = SVC_OK;

    if (CalID >= SVC_CALIB_MAX_OBJ_NUM) {
        RetVal = SVC_NG;
    } else if (pCalHeader == NULL) {
        RetVal = SVC_NG;
    } else if (DataSize == 0U) {
        RetVal = SVC_NG;
    } else {
        SVC_CALIB_ITEM_HEADER_s *pItemHeader = &(pCalHeader->ItemInfo[CalID - 1U]);

        if (pItemHeader->ID != CalID) {
            if ((pCalHeader->NumOfItem > 0U) && (pItemHeader->ID > 0U)) {
                pCalHeader->NumOfItem --;
            }
            pItemHeader->Offset = 0U;
            pItemHeader->Size = 0U;
        } else if (pItemHeader->Size < DataSize) {
            RetVal = SVC_NG;

            PRN_CAL_MGR_LOG "The calib id(%d) is exist. data size(0x%08X) > nand size(0x%08X)"
                PRN_CAL_MGR_ARG_UINT32 CalID             PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_ARG_UINT32 DataSize          PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_ARG_UINT32 pItemHeader->Size PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        } else {
            PRN_CAL_MGR_LOG "Calib id(%d) item info is exist. Replace it! %d"
                PRN_CAL_MGR_ARG_UINT32 CalID           PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_ARG_UINT32 pItemHeader->ID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_API
        }

        if (RetVal == SVC_OK) {
            if (pItemHeader->Size == 0U) {
                UINT32 RequestSize = GetAlignedValU32(DataSize, SVC_CALIB_NAND_SECTOR_SIZE);

                if ((pCalHeader->NumOfItem + 1U) > SVC_CALIB_MAX_OBJ_NUM) {
                    RetVal = SVC_NG;
                    PRN_CAL_MGR_LOG "Fail to save data to NAND - calib item is full" PRN_CAL_MGR_NG
                } else {
                    UINT32 MaxOffset = 0, TempSize = 0;
                    UINT32 Idx;
                    for (Idx = 0U; Idx < SVC_CALIB_MAX_OBJ_NUM; Idx ++) {
                        if ((pCalHeader->ItemInfo[Idx].ID > SVC_CALIB_EMPTY) &&
                            (pCalHeader->ItemInfo[Idx].ID <= SVC_CALIB_MAX_OBJ_NUM)) {
                            if (MaxOffset < pCalHeader->ItemInfo[Idx].Offset) {
                                MaxOffset = pCalHeader->ItemInfo[Idx].Offset;
                                TempSize  = pCalHeader->ItemInfo[Idx].Size;
                            } else {
                                if (MaxOffset == pCalHeader->ItemInfo[Idx].Offset) {
                                    RetVal = SVC_NG;
                                    PRN_CAL_MGR_LOG "Fail to save data to NAND - get same offset of calib item." PRN_CAL_MGR_NG
                                    break;
                                }
                            }
                        }
                    }

                    if (RetVal == SVC_OK) {
                        if (MaxOffset == 0U) {
                            MaxOffset = SVC_CALIB_NAND_SECTOR_SIZE;
                            TempSize  = 0;
                        }

                        pItemHeader->ID = CalID;
                        pItemHeader->Offset = MaxOffset + TempSize;
                        pItemHeader->Size = RequestSize;

                        pCalHeader->NumOfItem ++;
                        pCalHeader->Version = AMBA_SVC_CALIB_VERSION;
                    }
                }
            }
        }
    }

    return RetVal;
}

static UINT32 SvcCalibMgr_RomLoadData(const SVC_CALIB_OBJ_s *pCalObj)
{
    UINT32 RetVal = SVC_OK;

    if (pCalObj == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to load data from ROMFS - input calib object should not null!" PRN_CAL_MGR_NG
    } else if (pCalObj->DataSource != SVC_CALIB_DATA_SOURCE_ROMFS) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to load data from ROMFS - invalid data source (%d)"
            PRN_CAL_MGR_ARG_UINT32 pCalObj->DataSource PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_NG
    } else if (SvcWrap_strlen(pCalObj->DataSourcePath) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to load data from ROMFS - input data source path should not null!" PRN_CAL_MGR_NG
    } else if (pCalObj->pShadowBuf == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to load data from ROMFS - the calib id(%d) dram shadow should not null!"
            PRN_CAL_MGR_ARG_UINT32 pCalObj->ID PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_NG
    } else if (pCalObj->ShadowBufSize == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to load data from ROMFS - the calib id(%d) dram shadow buffer size should not zero!"
            PRN_CAL_MGR_ARG_UINT32 pCalObj->ID PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_NG
    } else {
        UINT32 RomFileSize = 0U;

        RetVal = SvcNvm_GetRomFileSize(AMBA_USER_PARTITION_SYS_DATA, pCalObj->DataSourcePath, &RomFileSize);
        if ((RetVal != NVM_ERR_NONE) || (RomFileSize == 0U)) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to load data from ROMFS - calib id(%d) ROM data size should not zero!"
                PRN_CAL_MGR_ARG_UINT32 pCalObj->ID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        } else {
            if (RomFileSize > pCalObj->ShadowBufSize) {
                RetVal = SVC_NG;

                PRN_CAL_MGR_LOG "Fail to load data from ROMFS - calib id(%d) dram shadow too small, Shadow size %d, ROM file size %d"
                    PRN_CAL_MGR_ARG_UINT32 pCalObj->ID            PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 pCalObj->ShadowBufSize PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 RomFileSize            PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_NG
            }
        }

        if (RetVal == SVC_OK) {
            UINT8 *pShadowBuf;

            AmbaMisra_TypeCast(&(pShadowBuf), &(pCalObj->pShadowBuf));

            RetVal = SvcNvm_ReadRomFileCrc(AMBA_USER_PARTITION_SYS_DATA, pCalObj->DataSourcePath, 0, RomFileSize, pShadowBuf, 20000);
            if (RetVal != NVM_ERR_NONE) {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to load data from ROMFS - calib id(%d) load data fail!"
                    PRN_CAL_MGR_ARG_UINT32 pCalObj->ID PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_NG
            } else {
                PRN_CAL_MGR_LOG "Success to load data from ROMFS - calib id(%d)"
                    PRN_CAL_MGR_ARG_UINT32 pCalObj->ID PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_DBG
            }
        }
    }

    return RetVal;
}


static UINT32 SvcCalibMgr_FileLoadData(const SVC_CALIB_OBJ_s *pCalObj)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (pCalObj == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to load data from FILE - input calib object should not null!" PRN_CAL_MGR_NG
    } else if (pCalObj->DataSource != SVC_CALIB_DATA_SOURCE_FILE) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to load data from FILE - invalid data source (%d)"
            PRN_CAL_MGR_ARG_UINT32 pCalObj->DataSource PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_NG
    } else if (SvcWrap_strlen(pCalObj->DataSourcePath) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to load data from FILE - input data source path should not null!" PRN_CAL_MGR_NG
    } else if (pCalObj->pShadowBuf == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to load data from FILE - the calib id(%d) dram shadow should not null!"
            PRN_CAL_MGR_ARG_UINT32 pCalObj->ID PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_NG
    } else if (pCalObj->ShadowBufSize == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to load data from FILE - the calib id(%d) dram shadow buffer size should not zero!"
            PRN_CAL_MGR_ARG_UINT32 pCalObj->ID PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_NG
    } else {
        char   FilePath[128];
        UINT32 FileSize = 0U;
        AMBA_FS_FILE_INFO_s FileInfo;

        /* Configure the file path */
        AmbaSvcWrap_MisraMemset(FilePath, 0, sizeof(FilePath));
        AmbaUtility_StringAppend(FilePath, (UINT32)sizeof(FilePath), "c:\\");
        AmbaUtility_StringAppend(FilePath, (UINT32)sizeof(FilePath), pCalObj->DataSourcePath);

        /* Get the file status. exist or not */
        AmbaSvcWrap_MisraMemset(&FileInfo, 0, sizeof(FileInfo));
        RetVal = AmbaFS_GetFileInfo(pCalObj->DataSourcePath, &FileInfo);
        if (RetVal != AMBA_FS_ERR_NONE) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to load data from FILE - get the file info fail!" PRN_CAL_MGR_NG
        } else {
            FileSize = (UINT32)(FileInfo.Size & 0x00000000FFFFFFFFU);
            if (FileSize > 0U) {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to load data from FILE - file size should not zero!" PRN_CAL_MGR_NG
            }

            if (pCalObj->ShadowBufSize < FileSize) {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to load data from FILE - calib id(%d) is too small"
                    PRN_CAL_MGR_ARG_UINT32 pCalObj->ID PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_NG
                PRN_CAL_MGR_LOG "  Shadow size %d, File size %d"
                    PRN_CAL_MGR_ARG_UINT32 pCalObj->ShadowBufSize PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 FileSize               PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_NG
            }
        }

        if (RetVal == SVC_OK) {
            AMBA_FS_FILE *pFile = NULL;

            RetVal = AmbaFS_FileOpen(FilePath, "rb", &pFile);
            if (RetVal != AMBA_FS_ERR_NONE) {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to load data from FILE - open file fail!" PRN_CAL_MGR_NG
            } else {
                if (pFile == NULL) {
                    RetVal = SVC_NG;
                    PRN_CAL_MGR_LOG "Fail to load data from FILE - invalid file pointer!" PRN_CAL_MGR_NG
                } else {
                    UINT32 ReadSize = 0U;
                    void *pShadowBuf;
                    AmbaMisra_TypeCast(&(pShadowBuf), &(pCalObj->pShadowBuf));

                    AmbaSvcWrap_MisraMemset(pShadowBuf, 0, pCalObj->ShadowBufSize);
                    RetVal = AmbaFS_FileRead(pShadowBuf, 1, pCalObj->ShadowBufSize, pFile, &ReadSize);
                    if (RetVal != AMBA_FS_ERR_NONE) {
                        RetVal = SVC_NG;
                        PRN_CAL_MGR_LOG "Fail to load data from FILE" PRN_CAL_MGR_NG
                    } else {
                        if (ReadSize != FileSize) {
                            PRN_CAL_MGR_LOG "Warning to load data from FILE - read the calib id(%d) maybe wrong!"
                                PRN_CAL_MGR_ARG_UINT32 pCalObj->ID PRN_CAL_MGR_ARG_POST
                            PRN_CAL_MGR_API
                            PRN_CAL_MGR_LOG "  read size %d is not match request size %d"
                                PRN_CAL_MGR_ARG_UINT32 ReadSize PRN_CAL_MGR_ARG_POST
                                PRN_CAL_MGR_ARG_UINT32 FileSize PRN_CAL_MGR_ARG_POST
                            PRN_CAL_MGR_API
                        }
                    }

                    PRetVal = AmbaFS_FileClose(pFile);
                    if (PRetVal != 0U) {
                        PRN_CAL_MGR_LOG "Fail to load data from FILE - close file fail! ErrCode(0x%08X)"
                            PRN_CAL_MGR_ARG_UINT32 PRetVal PRN_CAL_MGR_ARG_POST
                        PRN_CAL_MGR_NG
                    }
                }
            }
        }
    }

    return RetVal;
}


static UINT32 SvcCalibMgr_FileSaveData(const SVC_CALIB_OBJ_s *pCalObj)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (pCalObj == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to save data from FILE - input calib object should not null!" PRN_CAL_MGR_NG
    } else if ((pCalObj->DataSource != SVC_CALIB_DATA_SOURCE_FILE)
             &&(pCalObj->DataSource != SVC_CALIB_DATA_SOURCE_ROMFS)) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to save data from FILE - invalid data source (%d)"
            PRN_CAL_MGR_ARG_UINT32 pCalObj->DataSource PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_NG
    } else if (SvcWrap_strlen(pCalObj->DataSourcePath) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to save data from FILE - input data source path should not null!" PRN_CAL_MGR_NG
    } else if (pCalObj->pShadowBuf == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to save data from FILE - the calib id(%d) dram shadow should not null!"
            PRN_CAL_MGR_ARG_UINT32 pCalObj->ID PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_NG
    } else if (pCalObj->ShadowBufSize == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to save data from FILE - the calib id(%d) dram shadow buffer size should not zero!"
            PRN_CAL_MGR_ARG_UINT32 pCalObj->ID PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_NG
    } else {
        char   FilePath[128];
        UINT32 FileSize = 0U;
        AMBA_FS_FILE *pFile = NULL;

        /* Configure the file path */
        AmbaSvcWrap_MisraMemset(FilePath, 0, sizeof(FilePath));
        AmbaUtility_StringAppend(FilePath, (UINT32)sizeof(FilePath), "c:\\");
        AmbaUtility_StringAppend(FilePath, (UINT32)sizeof(FilePath), pCalObj->DataSourcePath);

        /* Save the FILE */
        RetVal = AmbaFS_FileOpen(FilePath, "wb", &pFile);
        if (RetVal != AMBA_FS_ERR_NONE) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to save data from FILE - open file fail!" PRN_CAL_MGR_NG
        } else if (pFile == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to save data from FILE - invalid file pointer!" PRN_CAL_MGR_NG
        } else {
            UINT32 WriteSize = 0U;
            void *pShadowBuf;
            AmbaMisra_TypeCast(&(pShadowBuf), &(pCalObj->pShadowBuf));

            AmbaSvcWrap_MisraMemset(pShadowBuf, 0, pCalObj->ShadowBufSize);
            RetVal = AmbaFS_FileWrite(pShadowBuf, 1, pCalObj->ShadowBufSize, pFile, &WriteSize);
            if (RetVal != AMBA_FS_ERR_NONE) {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to save data from FILE" PRN_CAL_MGR_NG
            } else {
                if (WriteSize != FileSize) {
                    PRN_CAL_MGR_LOG "Warning to save data from FILE - read the calib id(%d) maybe wrong!"
                        PRN_CAL_MGR_ARG_UINT32 pCalObj->ID PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_API
                    PRN_CAL_MGR_LOG "  read size %d is not match request size %d"
                        PRN_CAL_MGR_ARG_UINT32 WriteSize PRN_CAL_MGR_ARG_POST
                        PRN_CAL_MGR_ARG_UINT32 FileSize  PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_API
                }
            }

            PRetVal = AmbaFS_FileClose(pFile);
            if (PRetVal != 0U) {
                PRN_CAL_MGR_LOG "Fail to save data from FILE - close file fail! ErrCode(0x%08X)"
                    PRN_CAL_MGR_ARG_UINT32 PRetVal PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_NG
            }
        }
    }

    return RetVal;
}


static UINT32 SvcCalibMgr_ScriptRun(char *pFilePath)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (pFilePath == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to run script - input script path should not null!" PRN_CAL_MGR_NG
        AmbaMisra_TouchUnused(pFilePath);
    } else {
        AMBA_FS_FILE *pFile = NULL;

        if (AMBA_FS_ERR_NONE != AmbaFS_FileOpen(pFilePath, "r", &pFile)) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to run script - open file fail - %s"
                PRN_CAL_MGR_ARG_CSTR   pFilePath PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        } else {
            static char CalScriptLineBuf[SVC_CALIB_SCRIPT_MAX_LENGTH] GNU_SECTION_NOZEROINIT;

            while (AMBA_FS_ERR_NONE == AmbaFS_FileEof(pFile)) {

                AmbaSvcWrap_MisraMemset(CalScriptLineBuf, 0, sizeof(CalScriptLineBuf));
                if (SVC_OK == SvcCalibMgr_ScriptReadLine(pFile, CalScriptLineBuf, (UINT32)sizeof(CalScriptLineBuf), NULL)) {
                    if (SVC_OK == SvcCalibMgr_ScriptCmdLineCheck(CalScriptLineBuf)) {
                        if (SVC_OK != SvcCalibMgr_ScriptCmdParser(CalScriptLineBuf)) {
                            PRN_CAL_MGR_LOG "Fail to run script - parsing cmd fail" PRN_CAL_MGR_NG
                        }
                    }
                }

            }

            PRetVal = AmbaFS_FileClose(pFile);
            if (PRetVal != 0U) {
                PRN_CAL_MGR_LOG "Fail to run script - close file fail! ErrCode(0x%08X)"
                    PRN_CAL_MGR_ARG_UINT32 PRetVal PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_NG
            }

            AmbaMisra_TouchUnused(CalScriptLineBuf);
        }
    }

    return RetVal;
}


static UINT32 SvcCalibMgr_ScriptReadLine(AMBA_FS_FILE *pFile, char *pLineBuf, UINT32 LineBufSize, UINT32 *pLength)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (pFile == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to read line data - file pointer should not null" PRN_CAL_MGR_NG
    } else if (pLineBuf == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to read line data - input line buffer should not null!" PRN_CAL_MGR_NG
    } else if (LineBufSize == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to read line data - line buffer size should not zero!" PRN_CAL_MGR_NG
    } else {
        char CharBuf;
        UINT32 NumOfLen = 0U;

        AmbaSvcWrap_MisraMemset(pLineBuf, 0, LineBufSize);

        do {

            CharBuf = '\0';
            PRetVal = AmbaFS_FileRead(&CharBuf, 1, 1, pFile, &PRetVal);

            if ((CharBuf != '\n') &&
                (CharBuf != '\r') &&
                (RetVal == SVC_OK)) {
                pLineBuf[NumOfLen] = CharBuf;
                NumOfLen += 1U;

                if (NumOfLen >= LineBufSize) {
                    RetVal = SVC_NG;
                    PRN_CAL_MGR_LOG "Fail to read line data - the line buffer is too small to service it" PRN_CAL_MGR_NG
                }
            }

            if (CharBuf == ';') {
                RetVal = 99U;
            }

            if (CharBuf == '\n') {
                break;
            }

        } while (AMBA_FS_ERR_NONE == AmbaFS_FileEof(pFile));

        pLineBuf[NumOfLen] = '\0';

        if (pLength != NULL) {
            *pLength = NumOfLen;
        }
    }

    return RetVal;
}


static UINT32 SvcCalibMgr_ScriptCmdLineCheck(char *pLineBuf)
{
    UINT32 RetVal = SVC_NG;

    if (pLineBuf != NULL) {
        char *pChar = pLineBuf;
        while (*pChar != '\0') {
            if (*pChar == ' ') {
                pChar ++;
            } else {
                if (*pChar != ';') {
                    RetVal = SVC_OK;
                }
                break;
            }
        }

        AmbaMisra_TouchUnused(pChar);
    }

    return RetVal;
}


static UINT32 SvcCalibMgr_ScriptCmdParser(char *pLineBuf)
{
    UINT32 RetVal = SVC_OK;

    if (pLineBuf == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to parse cmd - input line buffer should not null" PRN_CAL_MGR_NG
    } else {
        static char * ArgVector[SVC_CALIB_SCRIPT_MAX_PARAM] GNU_SECTION_NOZEROINIT;
        UINT32        ArgCount = 0U;
        const char Token[2] = " ";
        char *pStrS = pLineBuf;
        char *pStrE = SvcWrap_strstr(pStrS, Token);

        AmbaSvcWrap_MisraMemset(ArgVector, 0, sizeof(ArgVector));

        PRN_CAL_MGR_LOG "%sCmdLine :%s %s%s%s"
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_2 PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_STR     PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_ARG_CSTR   pLineBuf               PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_DBG

        while (pStrE != NULL) {
            *pStrE = '\0';

            if (0U < SvcWrap_strlen(pStrS)) {
                ArgVector[ArgCount] = pStrS;
                ArgCount += 1U;
            }

            pStrS = pStrE;
            pStrS ++;
            pStrE = SvcWrap_strstr(pStrS, Token);
        }

        if (0U < SvcWrap_strlen(pStrS)) {
            ArgVector[ArgCount] = pStrS;
            ArgCount += 1U;
        }

        if ((SVC_CalibMgrFlag & SVC_LOG_CAL_DBG) > 0U) {
            UINT32 Idx;

            PRN_CAL_MGR_LOG "%sArgCount :%s %s%d%s"
                PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_2 PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_NUM     PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_ARG_UINT32 ArgCount               PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_API

            for (Idx = 0U; Idx < ArgCount; Idx ++) {

                PRN_CAL_MGR_LOG "  %sArgVector[%s%s%d%s%s] : %s%s%s%s"
                    PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_2 PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_NUM     PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 Idx                    PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_2 PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_STR     PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CSTR   ArgVector[Idx]         PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_API

            }

            PRN_CAL_MGR_LOG " " PRN_CAL_MGR_API
        }

        if (ArgCount > 0U) {
            UINT32 ExecuteLocalCmd = 0U;

            if (0 == SvcWrap_strcmp(ArgVector[1U], "pause")) {
                if (ArgCount > 1U){
                    UINT32 DelayTime = 0U;
                    SvcCalibMgr_ShellStrToU32(ArgVector[2U], &(DelayTime) );
                    DelayTime *= 1000U;
                    SvcCalibMgr_ScriptMPause(DelayTime);
                    ExecuteLocalCmd = 1U;
                }
            } else if (0 == SvcWrap_strcmp(ArgVector[1U], "mpause")) {
                if (ArgCount > 1U){
                    UINT32 DelayTime = 0U;
                    SvcCalibMgr_ShellStrToU32(ArgVector[2U], &(DelayTime) );
                    SvcCalibMgr_ScriptMPause(DelayTime);
                    ExecuteLocalCmd = 1U;
                }
            } else {
                /* Do nothing */
            }

            if (ExecuteLocalCmd == 0U) {
                SvcCalib_CmdAppEntry(ArgCount, ArgVector);
            }
        }
    }

    return RetVal;
}


static void SvcCalibMgr_ScriptMPause(UINT32 DelayTime)
{
    UINT32 PRetVal;

    if (DelayTime > 0U) {
        PRetVal = AmbaKAL_TaskSleep(DelayTime);
        if (PRetVal != 0U) {
            PRN_CAL_MGR_LOG "Fail to pause script - proc 'delay' fail! ErrCode(0x%08X)"
                PRN_CAL_MGR_ARG_UINT32 PRetVal PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        }
    }
}


static UINT32 SvcCalibMgr_CalcMemSizeGet(UINT32 *pBufSize)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibMgrFlag & SVC_CALIB_MGR_FLG_MEM_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to get calib calculate mem - initial calib control first!" PRN_CAL_MGR_NG
    } else if ((SVC_CalibMgr.Status & SVC_CALIB_CTRL_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to get calib calculate mem - initial calib control first!!!" PRN_CAL_MGR_NG
    } else {
        UINT32 ObjIdx, MemSize;
        UINT32 MaxCalcMemSize = 0U, CalcMemSize;
        SVC_CALIB_OBJ_CTRL_s *pObjCtrl = NULL;

        for (ObjIdx = 0U; ObjIdx < SVC_CalibMgr.NumOfCalObj; ObjIdx ++) {
            pObjCtrl = &(SVC_CalibMgr.CalObjCtrl[ObjIdx]);

            if ((pObjCtrl->Status & SVC_CALIB_OBJ_INIT) > 0U) {
                if (0U == SvcCalibMgr_MutexTake(&(pObjCtrl->Mutex), 10000U)) {

                    if ((pObjCtrl->CalObj.Enable > 0U) &&
                        (pObjCtrl->CalObj.CmdFunc != NULL)) {
                        CalcMemSize = 0U;
                        if (0U != (pObjCtrl->CalObj.CmdFunc)(SVC_CALIB_CMD_MEM_QUERY, &(pObjCtrl->CalObj.NumOfTable), &MemSize, &MemSize, &CalcMemSize)) {
                            RetVal = SVC_NG;
                            PRN_CAL_MGR_LOG "Query CalID(%d) calc memory size failure"
                                PRN_CAL_MGR_ARG_UINT32 pObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
                            PRN_CAL_MGR_NG
                        } else {
                            PRN_CAL_MGR_LOG "Query CalID(%d) calc memory size 0x%08X"
                                PRN_CAL_MGR_ARG_UINT32 pObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
                                PRN_CAL_MGR_ARG_UINT32 CalcMemSize         PRN_CAL_MGR_ARG_POST
                            PRN_CAL_MGR_DBG

                            if (CalcMemSize > MaxCalcMemSize) {
                                PRN_CAL_MGR_LOG "Update max calc memory size 0x%08X -> 0x%08X"
                                    PRN_CAL_MGR_ARG_UINT32 MaxCalcMemSize PRN_CAL_MGR_ARG_POST
                                    PRN_CAL_MGR_ARG_UINT32 CalcMemSize    PRN_CAL_MGR_ARG_POST
                                PRN_CAL_MGR_DBG
                                MaxCalcMemSize = CalcMemSize;
                            }
                        }
                    }

                    SvcCalibMgr_MutexGive(&(pObjCtrl->Mutex));
                }

                if (RetVal != SVC_OK) {
                    break;
                }
            }
        }

        if (RetVal == SVC_OK) {
            *pBufSize = MaxCalcMemSize;

            PRN_CAL_MGR_LOG "Query max calc memory size 0x%08X"
                PRN_CAL_MGR_ARG_UINT32 MaxCalcMemSize PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_DBG
        }
    }

    return RetVal;
}

static void SvcCalibMgr_ShellStrToU32(const char *pString, UINT32 *pValue)
{
    UINT32 ErrCode = AmbaUtility_StringToUInt32(pString, pValue);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }
}

static UINT32 SvcCalibMgr_ShellInfo(UINT32 ArgCount, char * const *pArgVector)
{
    if ((ArgCount > 0U) && (pArgVector != NULL)) {
        UINT32 Idx, ObjIdx;
        static char  ObjName[16];
        const void *pPoint;

        PRN_CAL_MGR_LOG " " PRN_CAL_MGR_API
        PRN_CAL_MGR_LOG "====== %sCalibration Manager Information%s ======"
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_0 PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_API

        PRN_CAL_MGR_LOG "------ %sNAND Info setting%s ------"
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_API
        PRN_CAL_MGR_LOG "  Initialized        : %d"
            PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalNandInfo.Initialized PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_API
        PRN_CAL_MGR_LOG "  WriteProtect       : %d"
            PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalNandInfo.WriteProtect PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_API
        PRN_CAL_MGR_LOG "  TotalSizeInSectors : %d"
            PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalNandInfo.TotalSizeInSectors PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_API
        PRN_CAL_MGR_LOG "  TotalSizeInBytes   : %llu"
            PRN_CAL_MGR_ARG_UINT64 SVC_CalibMgr.CalNandInfo.TotalSizeInBytes
        PRN_CAL_MGR_API
        PRN_CAL_MGR_LOG "  PageSizeInBytes    : %d"
            PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalNandInfo.PageSizeInBytes PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_API
        PRN_CAL_MGR_LOG "  BlockSizeInPages   : %d"
            PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalNandInfo.BlockSizeInPages PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_API
        PRN_CAL_MGR_LOG "  EraseSizeInBlocks  : %d"
            PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalNandInfo.EraseSizeInBlocks PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_API
        PRN_CAL_MGR_LOG "  SectorSizeInBytes  : %d"
            PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalNandInfo.SectorSizeInBytes PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_API

        PRN_CAL_MGR_LOG "------- %sNAND Header%s -------"
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_API
        PRN_CAL_MGR_LOG "  NumOfItem  : %d"
            PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalHeader.NumOfItem PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_API
        PRN_CAL_MGR_LOG "  %sItem Index : ID Offset     Size       CheckSum   Comment%s"
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_2 PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_API
        for (Idx = 0U; Idx < SVC_CALIB_MAX_OBJ_NUM; Idx ++) {

            AmbaSvcWrap_MisraMemset(ObjName, 0, sizeof(ObjName));
            for (ObjIdx = 0U; ObjIdx < SVC_CalibMgr.NumOfCalObj; ObjIdx ++) {
                if (SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.ID == SVC_CalibMgr.CalHeader.ItemInfo[Idx].ID) {
                    SvcWrap_strcpy(ObjName, sizeof(ObjName), SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.Name);
                }
            }
            if (0U == SvcWrap_strlen(ObjName)) {
                ObjName[0] = 'I';
                ObjName[1] = 'n';
                ObjName[2] = 'v';
                ObjName[3] = 'a';
                ObjName[4] = 'l';
                ObjName[5] = 'i';
                ObjName[6] = 'd';
                ObjName[7] = '\0';
            }

            PRN_CAL_MGR_LOG "  No.%02d Item : %02d 0x%08X 0x%08X 0x%08X %s"
                PRN_CAL_MGR_ARG_UINT32 Idx                                           PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalHeader.ItemInfo[Idx].ID       PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalHeader.ItemInfo[Idx].Offset   PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalHeader.ItemInfo[Idx].Size     PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalHeader.ItemInfo[Idx].CheckSum PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_ARG_CSTR   ObjName                                       PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_API
        }

        PRN_CAL_MGR_LOG "------ %sNAND Objects%s ------"
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_API
#ifndef CONFIG_ARM64
        PRN_CAL_MGR_LOG "  %sID NumOfTable ShadowAddr ShadowSize WorkingAddr WorkingSize CalcCbFunc%s"
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_2 PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_API
#else
        PRN_CAL_MGR_LOG "  %sID NumOfTable ShadowAddr         ShadowSize WorkingAddr        WorkingSize CalcCbFunc%s"
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_2 PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_API
#endif
        for (ObjIdx = 0U; ObjIdx < SVC_CalibMgr.NumOfCalObj; ObjIdx ++) {
            if (SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.DataSource == SVC_CALIB_DATA_SOURCE_NAND) {

                AmbaMisra_TypeCast(&(pPoint), &(SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.CmdFunc));

#ifndef CONFIG_ARM64
                PRN_CAL_MGR_LOG "  %02d %10d %p 0x%08X  %p  0x%08X %p"
#else
                PRN_CAL_MGR_LOG "  %02d %10d %p 0x%08X %p 0x%08X  %p"
#endif
                    PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.ID             PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.NumOfTable     PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CPOINT SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.pShadowBuf     PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.ShadowBufSize  PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CPOINT SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.pWorkingBuf    PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.WorkingBufSize PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CPOINT pPoint                                                PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_API

            }
        }

        PRN_CAL_MGR_LOG "------ %sROM Objects%s ------"
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_API

        PRN_CAL_MGR_LOG "  %sID NumOfTable ShadowAddr ShadowSize WorkingAddr WorkingSize CalcCbFunc DataSource DataPath%s"
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_2 PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_API
        for (ObjIdx = 0U; ObjIdx < SVC_CalibMgr.NumOfCalObj; ObjIdx ++) {
            if (SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.DataSource == SVC_CALIB_DATA_SOURCE_ROMFS) {

                AmbaMisra_TypeCast(&(pPoint), &(SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.CmdFunc));

                PRN_CAL_MGR_LOG "  %02d %10d %p 0x%08X  %p  0x%08X %p %10d %s"
                    PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.ID             PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.NumOfTable     PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CPOINT SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.pShadowBuf     PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.ShadowBufSize  PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CPOINT SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.pWorkingBuf    PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.WorkingBufSize PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CPOINT pPoint                                                PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.DataSource     PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CSTR   SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.DataSourcePath PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_API
            }
        }
        PRN_CAL_MGR_LOG "------ %sFILE Objects%s ------"
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_API

        PRN_CAL_MGR_LOG "%s  ID NumOfTable ShadowAddr ShadowSize WorkingAddr WorkingSize CalcCbFunc DataSource DataPath%s"
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_2 PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_API
        for (ObjIdx = 0U; ObjIdx < SVC_CalibMgr.NumOfCalObj; ObjIdx ++) {
            if (SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.DataSource == SVC_CALIB_DATA_SOURCE_FILE) {

                AmbaMisra_TypeCast(&(pPoint), &(SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.CmdFunc));

                PRN_CAL_MGR_LOG "  %02d %10d %p 0x%08X  %p  0x%08X %p %10d %s"
                    PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.ID             PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.NumOfTable     PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CPOINT SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.pShadowBuf     PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.ShadowBufSize  PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CPOINT SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.pWorkingBuf    PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.WorkingBufSize PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CPOINT pPoint                                                PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.DataSource     PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CSTR   SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.DataSourcePath PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_API
            }
        }
        PRN_CAL_MGR_LOG "------ %sUSER Objects%s ------"
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_API

        PRN_CAL_MGR_LOG "%s  ID NumOfTable ShadowAddr ShadowSize WorkingAddr WorkingSize CalcCbFunc DataSource DataPath%s"
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_2 PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_API
        for (ObjIdx = 0U; ObjIdx < SVC_CalibMgr.NumOfCalObj; ObjIdx ++) {
            if (SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.DataSource == SVC_CALIB_DATA_SOURCE_USER) {

                AmbaMisra_TypeCast(&(pPoint), &(SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.CmdFunc));

                PRN_CAL_MGR_LOG "  %02d %10d %p 0x%08X  %p  0x%08X %p %10d %s"
                    PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.ID             PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.NumOfTable     PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CPOINT SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.pShadowBuf     PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.ShadowBufSize  PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CPOINT SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.pWorkingBuf    PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.WorkingBufSize PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CPOINT pPoint                                                PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.DataSource     PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CSTR   SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.DataSourcePath PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_API
            }
        }
    }

    AmbaMisra_TouchUnused(CalibMgrShellFunc);

    return SVC_OK;
}


static void SvcCalibMgr_ShellInfoU(void)
{
    PRN_CAL_MGR_LOG "  %sinfo%s                  : print the calib manager info"
        PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
    PRN_CAL_MGR_API
    PRN_CAL_MGR_LOG " " PRN_CAL_MGR_API
}


static UINT32 SvcCalibMgr_ShellMemHook(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;
    if ((SVC_CalibMgr.Status & SVC_CALIB_CTRL_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to hook calc memory - initial calib control first!" PRN_CAL_MGR_NG
    } else if (ArgCount < 5U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to hook calc memory - Argc should >= 5" PRN_CAL_MGR_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to hook calc memory - Argv should not null!" PRN_CAL_MGR_NG
    } else {
        UINT8 *pMemBuf = NULL;
        UINT32 MemSize = 0U;

#ifdef CONFIG_ARM64
        UINT64 U64Val = 0U;
        if (0U == SvcWrap_strtoull(pArgVector[3U], &U64Val)) {
            AmbaMisra_TypeCast(&(pMemBuf), &(U64Val));
        }
#else
        UINT32 U32Val = 0U;
        SvcCalibMgr_ShellStrToU32(pArgVector[3U], &U32Val);
        AmbaMisra_TypeCast(&(pMemBuf), &(U32Val));
#endif
        SvcCalibMgr_ShellStrToU32(pArgVector[4U], &(MemSize) );
        if (0U != SvcCalib_ItemCalcMemSet(pMemBuf, MemSize)) {
            PRN_CAL_MGR_LOG "Fail to hook calc memory - set calculation memory!" PRN_CAL_MGR_NG
        }
    }

    return RetVal;
}


static void SvcCalibMgr_ShellMemHookU(void)
{
    PRN_CAL_MGR_LOG "  %smem%s                   : hook calib calculation memory"
        PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
    PRN_CAL_MGR_API
    PRN_CAL_MGR_LOG "    MemAddr             : memory address" PRN_CAL_MGR_API
    PRN_CAL_MGR_LOG "    MemSize             : memory size" PRN_CAL_MGR_API
    PRN_CAL_MGR_LOG " " PRN_CAL_MGR_API
}

static UINT32 SvcCalibMgr_ShellDbgMsg(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;
    if (ArgCount <= 3U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to config debug msg - Argc should >= 4" PRN_CAL_MGR_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to config debug msg - Argv should not null!" PRN_CAL_MGR_NG
    } else {
        UINT32 DbgMsgOn = 0U;

        SvcCalibMgr_ShellStrToU32(pArgVector[3U],  &DbgMsgOn);

        if ((DbgMsgOn & 0x2U) > 0U) {
            SVC_CalibMgrFlag |= SVC_LOG_CAL_DBG;
        }

        PRN_CAL_MGR_LOG " " PRN_CAL_MGR_API
        PRN_CAL_MGR_LOG "------ Calibration MGR Debug Msg On/Off, lvl %d ------"
            PRN_CAL_MGR_ARG_UINT32 DbgMsgOn PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_API
    }
    return RetVal;
}

static void SvcCalibMgr_ShellDbgMsgU(void)
{
    PRN_CAL_MGR_LOG "  %sdbg_msg%s               : enable calib mgr debug msg"
        PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
    PRN_CAL_MGR_API
    PRN_CAL_MGR_LOG "     Enable             : 0: disable debug message" PRN_CAL_MGR_API
    PRN_CAL_MGR_LOG "                        : 2: enable debug message level 2" PRN_CAL_MGR_API
    PRN_CAL_MGR_LOG " " PRN_CAL_MGR_API
}

static UINT32 SvcCalibMgr_ShellNand(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_NG;

    if ((ArgCount > 3U) && (pArgVector != NULL)) {
        UINT32 NandShellIdx, NandShellCnt = (UINT32)(sizeof(CalibMgrNandShellFunc)) / (UINT32)(sizeof(CalibMgrNandShellFunc[0U]));

        RetVal = SVC_OK;

        for (NandShellIdx = 0U; NandShellIdx < NandShellCnt; NandShellIdx ++) {
            if ((CalibMgrNandShellFunc[NandShellIdx].pFunc != NULL) && (CalibMgrNandShellFunc[NandShellIdx].Enable > 0U)) {
                if (0 == SvcWrap_strcmp(pArgVector[3U], CalibMgrNandShellFunc[NandShellIdx].ShellCmdName)) {
                    if (0U != (CalibMgrNandShellFunc[NandShellIdx].pFunc)(ArgCount, pArgVector)) {
                        if (CalibMgrNandShellFunc[NandShellIdx].pUsage != NULL) {
                            (CalibMgrNandShellFunc[NandShellIdx].pUsage)();
                        }
                    }
                    break;
                }
            }
        }

        AmbaMisra_TouchUnused(CalibMgrNandShellFunc);
    }

    return RetVal;
}


static void SvcCalibMgr_ShellNandU(void)
{
    UINT32 NandShellIdx, NandShellCnt = (UINT32)(sizeof(CalibMgrNandShellFunc)) / (UINT32)(sizeof(CalibMgrNandShellFunc[0U]));

    PRN_CAL_MGR_LOG "  %sitem%s                  : calibration nand item control"
        PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
    PRN_CAL_MGR_API
    for (NandShellIdx = 0U; NandShellIdx < NandShellCnt; NandShellIdx ++) {
        if ((CalibMgrNandShellFunc[NandShellIdx].pUsage != NULL) && (CalibMgrNandShellFunc[NandShellIdx].Enable > 0U)) {
            (CalibMgrNandShellFunc[NandShellIdx].pUsage)();
        } else {
            PRN_CAL_MGR_LOG "    %s"
                PRN_CAL_MGR_ARG_CSTR   CalibMgrNandShellFunc[NandShellIdx].ShellCmdName PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_API
        }
    }

    PRN_CAL_MGR_LOG " " PRN_CAL_MGR_API
}


static UINT32 SvcCalibMgr_ShellItemObjInit(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;
    if (ArgCount < 4U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to initial calib item - Argc should >= 4" PRN_CAL_MGR_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to initial calib item - invalid arguments" PRN_CAL_MGR_NG
    } else {
        UINT32 CalID = 0U;

        if (ArgCount >= 5U) {
            SvcCalibMgr_ShellStrToU32(pArgVector[4U], &CalID);
        }

        if (CalID == 0U) {
            if (0U == SvcCalib_ItemDataInitAll()) {
                PRN_CAL_MGR_LOG "Initial all calib item done" PRN_CAL_MGR_API
            }
        } else {
            if (0U == SvcCalib_ItemDataInit(CalID)) {
                PRN_CAL_MGR_LOG "Initial calib item done %d"
                    PRN_CAL_MGR_ARG_UINT32 CalID PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_API
            }
        }
    }

    return RetVal;
}


static void SvcCalibMgr_ShellItemObjInitU(void)
{
    PRN_CAL_MGR_LOG "    %sinit%s                : initial all calib item"
        PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_2 PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
    PRN_CAL_MGR_API
    PRN_CAL_MGR_LOG " " PRN_CAL_MGR_API
}


static UINT32 SvcCalibMgr_ShellItemObjSave(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount < 6U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to save calib nand data to file - Argc should >= 6" PRN_CAL_MGR_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to save calib nand data to file - invalid arguments" PRN_CAL_MGR_NG
    } else {
        UINT32 CalID = 0U;

        SvcCalibMgr_ShellStrToU32(pArgVector[4U], &CalID);

        if (CalID == SVC_CALIB_EMPTY) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to save calib nand data to file - invalid calib id(%d)"
                PRN_CAL_MGR_ARG_UINT32 CalID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        } else {
            UINT8 *pBuf = NULL;
            UINT32 CalcMemSize = SVC_CALIB_NAND_SECTOR_SIZE;
            AMBA_NVM_SECTOR_CONFIG_s NvmSectorCfg;

            if (0U != SvcCalib_ItemCalcMemGet(15, 1U, &pBuf, &CalcMemSize)) {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to save calib nand data to file - get calc memory fail!" PRN_CAL_MGR_NG
            } else if (CalcMemSize < SVC_CALIB_NAND_SECTOR_SIZE) {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to save calib nand data to file - memory too small for nand header!" PRN_CAL_MGR_NG
            } else if (pBuf == NULL) {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to save calib nand data to file - invalid buffer!" PRN_CAL_MGR_NG
            } else {
                SVC_CALIB_ITEM_HEADER_s CalItemHeader;

                AmbaSvcWrap_MisraMemset(&CalItemHeader, 0, sizeof(CalItemHeader));

                // Read the calib nand header.
                AmbaSvcWrap_MisraMemset(pBuf, 0, CalcMemSize);
                AmbaSvcWrap_MisraMemset(&NvmSectorCfg, 0, sizeof(NvmSectorCfg));
                NvmSectorCfg.StartSector = 0U;
                NvmSectorCfg.NumSector   = 1U;
                NvmSectorCfg.pDataBuf    = pBuf;
                if (NVM_ERR_NONE != SvcNvm_ReadSector(AMBA_USER_PARTITION_CALIBRATION_DATA, &NvmSectorCfg, 2000)) {
                    RetVal = SVC_NG;
                    PRN_CAL_MGR_LOG "Fail to save calib nand data to file - load the calib header fail!" PRN_CAL_MGR_NG
                } else {
                    SVC_CALIB_NAND_HEADER_s *pNandHeader = NULL;
                    AmbaMisra_TypeCast(&(pNandHeader), &(pBuf));
                    if (pNandHeader == NULL) {
                        RetVal = SVC_NG;
                        PRN_CAL_MGR_LOG "Fail to save calib nand data to file - invalid nand header!" PRN_CAL_MGR_NG
                        AmbaMisra_TouchUnused(pNandHeader);
                    } else if (pNandHeader->Version != (UINT32)AMBA_SVC_CALIB_VERSION) {
                        RetVal = SVC_NG;
                        PRN_CAL_MGR_LOG "The header version does not match!" PRN_CAL_MGR_NG
                    } else if (pNandHeader->ItemInfo[CalID - 1U].ID != CalID) {
                        RetVal = SVC_NG;
                        PRN_CAL_MGR_LOG "Fail to save calib nand data to file - wrong calib id!" PRN_CAL_MGR_NG
                    } else {
                        CalItemHeader.ID     = pNandHeader->ItemInfo[CalID - 1U].ID;
                        CalItemHeader.Offset = pNandHeader->ItemInfo[CalID - 1U].Offset;
                        CalItemHeader.Size   = pNandHeader->ItemInfo[CalID - 1U].Size;

                        PRN_CAL_MGR_LOG "Item Header : ID(%d) Offset(0x%08X) Size(0x%X)"
                            PRN_CAL_MGR_ARG_UINT32 CalItemHeader.ID     PRN_CAL_MGR_ARG_POST
                            PRN_CAL_MGR_ARG_UINT32 CalItemHeader.Offset PRN_CAL_MGR_ARG_POST
                            PRN_CAL_MGR_ARG_UINT32 CalItemHeader.Size   PRN_CAL_MGR_ARG_POST
                        PRN_CAL_MGR_API
                    }
                }

                // Read the calib data
                if (RetVal == SVC_OK) {
                    if (CalcMemSize < GetAlignedValU32(CalItemHeader.Size, SVC_CALIB_NAND_SECTOR_SIZE)) {
                        RetVal = SVC_NG;
                        PRN_CAL_MGR_LOG "Fail to load data from NAND - memory to small to dump calib data! 0x%X 0x%X"
                            PRN_CAL_MGR_ARG_UINT32 CalcMemSize                                                      PRN_CAL_MGR_ARG_POST
                            PRN_CAL_MGR_ARG_UINT32 GetAlignedValU32(CalItemHeader.Size, SVC_CALIB_NAND_SECTOR_SIZE) PRN_CAL_MGR_ARG_POST
                        PRN_CAL_MGR_NG
                    } else {
                        AmbaSvcWrap_MisraMemset(pBuf, 0, CalcMemSize);
                        AmbaSvcWrap_MisraMemset(&NvmSectorCfg, 0, sizeof(NvmSectorCfg));
                        NvmSectorCfg.StartSector = GetAlignedValU32(CalItemHeader.Offset, SVC_CALIB_NAND_SECTOR_SIZE) / SVC_CALIB_NAND_SECTOR_SIZE;
                        NvmSectorCfg.NumSector   = GetAlignedValU32(CalItemHeader.Size, SVC_CALIB_NAND_SECTOR_SIZE)   / SVC_CALIB_NAND_SECTOR_SIZE;
                        NvmSectorCfg.pDataBuf    = pBuf;
                        if (NVM_ERR_NONE != SvcNvm_ReadSector(AMBA_USER_PARTITION_CALIBRATION_DATA, &NvmSectorCfg, 2000)) {
                            RetVal = SVC_NG;
                            PRN_CAL_MGR_LOG "Fail to save calib nand data to file - load the calib data fail!" PRN_CAL_MGR_NG
                        }
                    }
                }

                // Save data to file
                if (RetVal == SVC_OK) {
                    AMBA_FS_FILE *pFile = NULL;

                    if (AMBA_FS_ERR_NONE != AmbaFS_FileOpen(pArgVector[5U], "wb", &pFile)) {
                        RetVal = SVC_NG;
                        PRN_CAL_MGR_LOG "Fail to save calib nand data to file - create file fail - %s"
                            PRN_CAL_MGR_ARG_CSTR   pArgVector[5U] PRN_CAL_MGR_ARG_POST
                        PRN_CAL_MGR_NG
                    } else {
                        UINT32 WriteSize = CalItemHeader.Size, NumSuccess = 0U;

                        PRetVal = AmbaFS_FileWrite(pBuf, 1, WriteSize, pFile, &NumSuccess); AmbaMisra_TouchUnused(&PRetVal);
                        if (WriteSize != NumSuccess) {
                            PRN_CAL_MGR_LOG "Warning to write data - return size(%d) is not equal write size(%d)"
                                PRN_CAL_MGR_ARG_UINT32 NumSuccess PRN_CAL_MGR_ARG_POST
                                PRN_CAL_MGR_ARG_UINT32 WriteSize  PRN_CAL_MGR_ARG_POST
                            PRN_CAL_MGR_DBG
                        }

                        PRN_CAL_MGR_LOG "Save calib(%d) nand data to file - %s"
                            PRN_CAL_MGR_ARG_UINT32 CalID          PRN_CAL_MGR_ARG_POST
                            PRN_CAL_MGR_ARG_CSTR   pArgVector[5U] PRN_CAL_MGR_ARG_POST
                        PRN_CAL_MGR_API

                        PRetVal = AmbaFS_FileClose(pFile);
                        if (PRetVal != 0U) {
                            PRN_CAL_MGR_LOG "Fail to save calib nand data to file - close file fail! ErrCode(0x%08X)"
                                PRN_CAL_MGR_ARG_UINT32 PRetVal PRN_CAL_MGR_ARG_POST
                            PRN_CAL_MGR_NG
                        }
                    }
                }
            }

            if (0U != SvcCalib_ItemCalcMemFree(15)) {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to save calib nand data to file - free calc memory fail!" PRN_CAL_MGR_NG
            }
        }
    }

    return RetVal;
}


static void SvcCalibMgr_ShellItemObjSaveU(void)
{
    PRN_CAL_MGR_LOG "    %ssave%s                : save calib object context from NAND to FILE"
        PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_2 PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
    PRN_CAL_MGR_API
    PRN_CAL_MGR_LOG "      ObjID             : configure calib object id" PRN_CAL_MGR_API
    PRN_CAL_MGR_LOG "      FileName          : configure file path" PRN_CAL_MGR_API
    PRN_CAL_MGR_LOG " " PRN_CAL_MGR_API
}


static UINT32 SvcCalibMgr_ShellItemObjLoad(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount < 6U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to load nand data from file - Argc should >= 6" PRN_CAL_MGR_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to load nand data from file - invalid arguments" PRN_CAL_MGR_NG
    } else {
        UINT32 CalID = 0U;

        SvcCalibMgr_ShellStrToU32(pArgVector[4U], &CalID);
        if (CalID == SVC_CALIB_EMPTY) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to load nand data from file - invalid calib id(%d)"
                PRN_CAL_MGR_ARG_UINT32 CalID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        } else if (CalID >= SVC_CALIB_MAX_OBJ_NUM) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to load nand data from file - calib id is out-of range! %d %d"
                PRN_CAL_MGR_ARG_UINT32 CalID                 PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_ARG_UINT32 SVC_CALIB_MAX_OBJ_NUM PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        } else {
            const SVC_CALIB_OBJ_CTRL_s *pCalObjCtrl = SvcCalibMgr_ObjCtrlGet(CalID);
            if (pCalObjCtrl != NULL) {
                if (pCalObjCtrl->CalObj.DataSource != SVC_CALIB_DATA_SOURCE_NAND) {
                    RetVal = SVC_NG;
                    PRN_CAL_MGR_LOG "Fail to load nand data from file - calib id (%d) source is not NAND"
                        PRN_CAL_MGR_ARG_UINT32 CalID PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_NG
                } else {
                    if ((SVC_CalibMgr.Status & SVC_CALIB_CTRL_LOAD_HEADER) == 0U) {
                        RetVal = SvcCalibMgr_NvmLoadHeader(&(SVC_CalibMgr.CalHeader));
                        if (RetVal != SVC_OK) {
                            PRN_CAL_MGR_LOG "Fail to load calib header" PRN_CAL_MGR_NG
                        }
                    }

                    if (RetVal == SVC_OK) {
                        AMBA_FS_FILE_INFO_s FileInfo;
                        UINT32 FileSize = 0U, FileSizeAlign = 0U;
                        SVC_CALIB_NAND_HEADER_s TmpHeader;
                        SVC_CALIB_ITEM_HEADER_s *pItemInfo;
                        UINT32 CalcMemSize = 0U;
                        UINT8 *pBuf = NULL;

                        /* Get file size */
                        AmbaSvcWrap_MisraMemset(&FileInfo, 0, sizeof(FileInfo));
                        PRetVal = AmbaFS_GetFileInfo(pArgVector[5U], &FileInfo); PRN_CAL_MGR_ERR_HDLR
                        FileSize = (UINT32)(FileInfo.Size);
                        FileSizeAlign = GetAlignedValU32(FileSize, SVC_CALIB_NAND_SECTOR_SIZE);
                        if (FileSize == 0U) {
                            RetVal = SVC_NG;
                            PRN_CAL_MGR_LOG "Fail to load nand data from file - invalid file size" PRN_CAL_MGR_NG
                        }

                        /* Configure nand header */
                        AmbaSvcWrap_MisraMemcpy(&TmpHeader, &(SVC_CalibMgr.CalHeader), sizeof(SVC_CALIB_NAND_HEADER_s));
                        if (SVC_OK != SvcCalibMgr_NvmHeaderConfig(CalID, &TmpHeader, FileSizeAlign)) {
                            RetVal |= SVC_NG;
                            PRN_CAL_MGR_LOG "Fail to load nand data from file - configure nand header fail" PRN_CAL_MGR_NG
                        }

                        /* Get temp buffer */
                        CalcMemSize = FileSizeAlign;
                        PRetVal = SvcCalib_ItemCalcMemGet(15, 1U, &pBuf, &CalcMemSize); AmbaMisra_TouchUnused(&PRetVal);
                        if (pBuf == NULL) {
                            RetVal |= SVC_NG;
                            PRN_CAL_MGR_LOG "Fail to load nand data from file - invalid buffer addr" PRN_CAL_MGR_NG
                        } else if (CalcMemSize < FileSizeAlign) {
                            RetVal |= SVC_NG;
                            PRN_CAL_MGR_LOG "Fail to load nand data from file - temp buffer size(0x%X) smaller requested size(0x%X)"
                                PRN_CAL_MGR_ARG_UINT32 CalcMemSize   PRN_CAL_MGR_ARG_POST
                                PRN_CAL_MGR_ARG_UINT32 FileSizeAlign PRN_CAL_MGR_ARG_POST
                            PRN_CAL_MGR_NG
                        } else {
                            AMBA_FS_FILE *pFile = NULL;
                            AMBA_NVM_SECTOR_CONFIG_s NvmSectorCfg;

                            if (AMBA_FS_ERR_NONE != AmbaFS_FileOpen(pArgVector[5U], "rb", &pFile)) {
                                RetVal = SVC_NG;

                                PRN_CAL_MGR_LOG "Fail to load nand data from file - open file fail - %s"
                                    PRN_CAL_MGR_ARG_CSTR   pArgVector[5U] PRN_CAL_MGR_ARG_POST
                                PRN_CAL_MGR_NG
                            } else {
                                UINT32 ReadSize = FileSize, NumSuccess = 0U;

                                /* Load file data */
                                AmbaSvcWrap_MisraMemset(pBuf, 0, FileSizeAlign);
                                PRetVal = AmbaFS_FileRead(pBuf, 1, ReadSize, pFile, &NumSuccess);
                                if (PRetVal != 0U) {
                                    PRN_CAL_MGR_LOG "Fail to load nand data from file - read file fail! ErrCode(0x%08X)"
                                        PRN_CAL_MGR_ARG_UINT32 PRetVal PRN_CAL_MGR_ARG_POST
                                    PRN_CAL_MGR_NG
                                }

                                PRetVal = AmbaFS_FileClose(pFile);
                                if (PRetVal != 0U) {
                                    PRN_CAL_MGR_LOG "Fail to load nand data from file - close file fail! ErrCode(0x%08X)"
                                        PRN_CAL_MGR_ARG_UINT32 PRetVal PRN_CAL_MGR_ARG_POST
                                    PRN_CAL_MGR_NG
                                }

                                /* Copy data to NAND */
                                pItemInfo = &(TmpHeader.ItemInfo[CalID - 1U]);
                                AmbaSvcWrap_MisraMemset(&NvmSectorCfg, 0, sizeof(NvmSectorCfg));
                                NvmSectorCfg.StartSector = GetAlignedValU32(pItemInfo->Offset, SVC_CALIB_NAND_SECTOR_SIZE) / SVC_CALIB_NAND_SECTOR_SIZE;
                                NvmSectorCfg.NumSector   = GetAlignedValU32(pItemInfo->Size, SVC_CALIB_NAND_SECTOR_SIZE)   / SVC_CALIB_NAND_SECTOR_SIZE;
                                NvmSectorCfg.pDataBuf    = pBuf;
                                if (NVM_ERR_NONE != SvcNvm_WriteSector(AMBA_USER_PARTITION_CALIBRATION_DATA, &NvmSectorCfg, 2000)) {
                                    RetVal = SVC_NG;
                                    PRN_CAL_MGR_LOG "Fail to load nand data from file - write the item data fail!" PRN_CAL_MGR_NG
                                } else {
                                    const SVC_CALIB_NAND_HEADER_s *pHeader = &TmpHeader;

                                    PRN_CAL_MGR_LOG "Successful to load nand data from file - write the item data done!" PRN_CAL_MGR_OK
                                    pItemInfo->CheckSum = AmbaUtility_Crc32(pBuf, pItemInfo->Size);

                                    /* Update NAND header */
                                    AmbaMisra_TypeCast(&(pBuf), &(pHeader));
                                    TmpHeader.CheckSum = AmbaUtility_Crc32(pBuf, (UINT32)(sizeof(SVC_CALIB_NAND_HEADER_s) - sizeof(UINT32)));
                                    AmbaSvcWrap_MisraMemcpy(&(SVC_CalibMgr.CalHeader), &TmpHeader, sizeof(SVC_CALIB_NAND_HEADER_s));

                                    /* Save NAND header */
                                    RetVal = SvcCalibMgr_NvmSaveHeader(&(SVC_CalibMgr.CalHeader));
                                    if (RetVal != SVC_OK) {
                                        PRN_CAL_MGR_LOG "Fail to load nand data from file - write header to nand fail!" PRN_CAL_MGR_NG
                                    }
                                }
                            }
                        }

                        /* Free temp buffer */
                        if (0U != SvcCalib_ItemCalcMemFree(15)) {
                            RetVal = SVC_NG;
                            PRN_CAL_MGR_LOG "Fail to load nand data from file - free calc memory fail!" PRN_CAL_MGR_NG
                        }
                    }
                }
            }
        }
    }

    return RetVal;
}


static void SvcCalibMgr_ShellItemObjLoadU(void)
{
    PRN_CAL_MGR_LOG "    %sload%s                : load calib object context from FILE to NAND"
        PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_2 PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
    PRN_CAL_MGR_API
    PRN_CAL_MGR_LOG "      ObjID             : configure calib object id" PRN_CAL_MGR_API
    PRN_CAL_MGR_LOG "      FileName          : configure file path" PRN_CAL_MGR_API
    PRN_CAL_MGR_LOG " " PRN_CAL_MGR_API
}


static UINT32 SvcCalibMgr_ShellItemObjReset(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount < 5U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to reset nand item - Argc should >= 5" PRN_CAL_MGR_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to reset nand item - invalid arguments" PRN_CAL_MGR_NG
    } else {
        UINT32 CalID = 0U;

        SvcCalibMgr_ShellStrToU32(pArgVector[4U], &CalID);

        if (CalID == SVC_CALIB_EMPTY) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to reset nand item - invalid calib id(%d)"
                PRN_CAL_MGR_ARG_UINT32 CalID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        } else if (CalID >= SVC_CALIB_MAX_OBJ_NUM) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to reset nand item - calib id is out-of range! %d %d"
                PRN_CAL_MGR_ARG_UINT32 CalID                 PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_ARG_UINT32 SVC_CALIB_MAX_OBJ_NUM PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        } else {
            UINT8 *pBuf = NULL;
            UINT32 CalcMemSize = SVC_CALIB_NAND_SECTOR_SIZE;
            AMBA_NVM_SECTOR_CONFIG_s NvmSectorCfg;

            if (0U != SvcCalib_ItemCalcMemGet(15, 1U, &pBuf, &CalcMemSize)) {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to reset nand item - get calc memory fail!" PRN_CAL_MGR_NG
            } else if (pBuf == NULL) {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to reset nand item - invalid buffer addr" PRN_CAL_MGR_NG
            } else if (CalcMemSize == 0U) {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to reset nand item - invalid buffer size" PRN_CAL_MGR_NG
            } else if (CalcMemSize < SVC_CALIB_NAND_SECTOR_SIZE) {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to reset nand item - memory too small for nand header!" PRN_CAL_MGR_NG
            } else {
                AmbaSvcWrap_MisraMemset(pBuf, 0, CalcMemSize);
                AmbaSvcWrap_MisraMemset(&NvmSectorCfg, 0, sizeof(NvmSectorCfg));
                NvmSectorCfg.StartSector = 0;
                NvmSectorCfg.NumSector   = (UINT32)(sizeof(SVC_CALIB_NAND_HEADER_s)) / SVC_CALIB_NAND_SECTOR_SIZE;
                NvmSectorCfg.pDataBuf    = pBuf;
                if (NVM_ERR_NONE != SvcNvm_ReadSector(AMBA_USER_PARTITION_CALIBRATION_DATA, &NvmSectorCfg, 2000)) {
                    RetVal = SVC_NG;
                    PRN_CAL_MGR_LOG "Fail to reset nand item - load the calib header fail!" PRN_CAL_MGR_NG
                } else {
                    SVC_CALIB_NAND_HEADER_s *pNandHeader;

                    AmbaMisra_TypeCast(&(pNandHeader), &(pBuf));
                    if (pNandHeader == NULL) {
                        RetVal = SVC_NG;
                        PRN_CAL_MGR_LOG "Fail to reset nand item - invalid nand header!" PRN_CAL_MGR_NG
                    } else if (pNandHeader->Version != AMBA_SVC_CALIB_VERSION) {
                        RetVal = SVC_NG;
                        PRN_CAL_MGR_LOG "The header version does not match!" PRN_CAL_MGR_NG
                    } else {

                        // Configure the ItemHeader
                        if ((pNandHeader->ItemInfo[CalID - 1U].ID > 0U) && (pNandHeader->NumOfItem > 0U)) {
                            pNandHeader->NumOfItem -= 1U;
                        }
                        pNandHeader->ItemInfo[CalID - 1U].ID       = 0U;
                        pNandHeader->ItemInfo[CalID - 1U].Offset   = 0U;
                        pNandHeader->ItemInfo[CalID - 1U].Size     = 0U;
                        pNandHeader->ItemInfo[CalID - 1U].CheckSum = 0U;

                        // Update Nand Header
                        AmbaMisra_TypeCast(&(pBuf), &(pNandHeader));
                        pNandHeader->CheckSum = AmbaUtility_Crc32(pBuf, (UINT32)(sizeof(SVC_CALIB_NAND_HEADER_s) - sizeof(UINT32)));

                        AmbaSvcWrap_MisraMemset(&NvmSectorCfg, 0, sizeof(NvmSectorCfg));
                        NvmSectorCfg.StartSector = 0U;
                        NvmSectorCfg.NumSector   = 1U;
                        NvmSectorCfg.pDataBuf    = pBuf;
                        if (NVM_ERR_NONE != SvcNvm_WriteSector(AMBA_USER_PARTITION_CALIBRATION_DATA, &NvmSectorCfg, 2000)) {
                            RetVal = SVC_NG;
                            PRN_CAL_MGR_LOG "Fail to reset nand item - write the nand header fail!" PRN_CAL_MGR_NG
                        } else {
                            // Update the calib manager setting
                            SVC_CalibMgr.CalHeader.Version   = pNandHeader->Version;
                            SVC_CalibMgr.CalHeader.NumOfItem = pNandHeader->NumOfItem;
                            SVC_CalibMgr.CalHeader.CheckSum  = pNandHeader->CheckSum;
                            AmbaSvcWrap_MisraMemcpy(SVC_CalibMgr.CalHeader.ItemInfo, pNandHeader->ItemInfo, sizeof(SVC_CalibMgr.CalHeader.ItemInfo));

                            PRN_CAL_MGR_LOG "Successful to reset nand item - calib id(%d)"
                                PRN_CAL_MGR_ARG_UINT32 CalID PRN_CAL_MGR_ARG_POST
                            PRN_CAL_MGR_OK
                        }
                    }
                }
            }

            // Free temp buffer
            if (0U != SvcCalib_ItemCalcMemFree(15)) {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to reset nand item - free calc memory fail!" PRN_CAL_MGR_NG
            }
        }
    }

    return RetVal;
}


static void SvcCalibMgr_ShellItemObjResetU(void)
{
    PRN_CAL_MGR_LOG "    %sreset%s               : reset calib nand item context"
        PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_2 PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
    PRN_CAL_MGR_API
    PRN_CAL_MGR_LOG "      ObjID             : configure calib object id" PRN_CAL_MGR_API
    PRN_CAL_MGR_LOG " " PRN_CAL_MGR_API
}


static UINT32 SvcCalibMgr_ShellScriptRun(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;
    if (ArgCount < 4U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to run script - Argc should >= 4" PRN_CAL_MGR_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to run script - invalid arguments" PRN_CAL_MGR_NG
    } else {
        if (0U != SvcCalibMgr_ScriptRun(pArgVector[3U])) {
            PRN_CAL_MGR_LOG "Fail to run script" PRN_CAL_MGR_NG
        }
    }

    return RetVal;
}


static void SvcCalibMgr_ShellScriptRunU(void)
{
    PRN_CAL_MGR_LOG "  %srun%s                   : run the calibration script"
        PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
    PRN_CAL_MGR_API
    PRN_CAL_MGR_LOG "    Path                : script file path" PRN_CAL_MGR_API
    PRN_CAL_MGR_LOG " " PRN_CAL_MGR_API
}


static void SvcCalibMgr_ShellEntryInit(void)
{
    if ((SVC_CalibMgrFlag & SVC_CALIB_MGR_FLG_SHELL_INIT) == 0U) {

        AmbaSvcWrap_MisraMemset(CalibMgrShellFunc, 0, sizeof(CalibMgrShellFunc));
        CalibMgrShellFunc[0U] = (SVC_CALIB_MGR_SHELL_FUNC_s) { 1U, "info",   SvcCalibMgr_ShellInfo,      SvcCalibMgr_ShellInfoU      };
        CalibMgrShellFunc[1U] = (SVC_CALIB_MGR_SHELL_FUNC_s) { 1U, "item",   SvcCalibMgr_ShellNand,      SvcCalibMgr_ShellNandU      };
        CalibMgrShellFunc[2U] = (SVC_CALIB_MGR_SHELL_FUNC_s) { 0U, "run",    SvcCalibMgr_ShellScriptRun, SvcCalibMgr_ShellScriptRunU };
        CalibMgrShellFunc[3U] = (SVC_CALIB_MGR_SHELL_FUNC_s) { 1U, "mem",    SvcCalibMgr_ShellMemHook,   SvcCalibMgr_ShellMemHookU   };
        CalibMgrShellFunc[4U] = (SVC_CALIB_MGR_SHELL_FUNC_s) { 1U, "dbg_msg",SvcCalibMgr_ShellDbgMsg,    SvcCalibMgr_ShellDbgMsgU    };

        AmbaSvcWrap_MisraMemset(CalibMgrNandShellFunc, 0, sizeof(CalibMgrNandShellFunc));
        CalibMgrNandShellFunc[0U] = (SVC_CALIB_MGR_SHELL_FUNC_s) { 1U, "init",  SvcCalibMgr_ShellItemObjInit,  SvcCalibMgr_ShellItemObjInitU  };
        CalibMgrNandShellFunc[1U] = (SVC_CALIB_MGR_SHELL_FUNC_s) { 1U, "save",  SvcCalibMgr_ShellItemObjSave,  SvcCalibMgr_ShellItemObjSaveU  };
        CalibMgrNandShellFunc[2U] = (SVC_CALIB_MGR_SHELL_FUNC_s) { 1U, "load",  SvcCalibMgr_ShellItemObjLoad,  SvcCalibMgr_ShellItemObjLoadU  };
        CalibMgrNandShellFunc[3U] = (SVC_CALIB_MGR_SHELL_FUNC_s) { 1U, "reset", SvcCalibMgr_ShellItemObjReset, SvcCalibMgr_ShellItemObjResetU };

        SVC_CalibMgrFlag |= SVC_CALIB_MGR_FLG_SHELL_INIT;
    }
}


static void SvcCalibMgr_CmdAppUsage(void)
{
    if ((SVC_CalibMgrFlag & SVC_CALIB_MGR_FLG_MEM_INIT) == 0U) {
        PRN_CAL_MGR_LOG "Initial calib manager memory first!" PRN_CAL_MGR_NG
    } else if ((SVC_CalibMgr.Status & SVC_CALIB_CTRL_INIT) == 0U) {
        PRN_CAL_MGR_LOG "Create calib manager first!" PRN_CAL_MGR_NG
    } else {
        UINT32 ObjIdx;
        const SVC_CALIB_OBJ_CTRL_s *pObjCtrl;

        PRN_CAL_MGR_LOG " " PRN_CAL_MGR_API
        PRN_CAL_MGR_LOG "====== %sCalibration Command Usage%s ======"
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_0 PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_API
        PRN_CAL_MGR_LOG "  %smgr%s"
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_API

        for (ObjIdx = 0U; ObjIdx < SVC_CalibMgr.NumOfCalObj; ObjIdx ++) {

            pObjCtrl = &(SVC_CalibMgr.CalObjCtrl[ObjIdx]);

            if (((pObjCtrl->Status & SVC_CALIB_OBJ_INIT) > 0U) &&
                (pObjCtrl->CalObj.Enable > 0U)) {

                PRN_CAL_MGR_LOG "  %s%s%s"
                    PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CSTR   pObjCtrl->CalObj.Name  PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_API
            }
        }
        PRN_CAL_MGR_LOG "Type 'svc_cal [option]' to check each module detail description" PRN_CAL_MGR_API
    }
}

/**
 * calib shell command entry
 *
 * @param [in] ArgCount Input arguments counter
 * @param [in] pArgVector Input arguments data
 *
 */
void SvcCalib_CmdAppEntry(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    SvcCalibMgr_ShellEntryInit();

    if (ArgCount < 1U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to proc calib mgr shell cmd - invalid arg count!" PRN_CAL_MGR_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to proc calib mgr shell cmd - invalid arg vector!" PRN_CAL_MGR_NG
    } else {
        UINT32 CurArgCnt = ArgCount;

        RetVal = SVC_NG;

        if (0 == SvcWrap_strcmp(pArgVector[CurArgCnt - 1U], "arg_on")) {
            UINT32 SIdx;

            CurArgCnt -= 1U;

            PRN_CAL_MGR_LOG " " PRN_CAL_MGR_API
            PRN_CAL_MGR_LOG "==== %sShell Arguments Setting%s (%s%d%s) ===="
                PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_0 PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_1 PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_ARG_UINT32 CurArgCnt              PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_API

            for (SIdx = 0U; SIdx < CurArgCnt; SIdx ++) {
                PRN_CAL_MGR_LOG "  pArgVector[%s%d%s] : %s%s%s"
                    PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_NUM PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 SIdx               PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_STR PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CSTR   pArgVector[SIdx]   PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_API
            }
        }

        if (0 == SvcWrap_strcmp("mgr", pArgVector[1U])) {
            UINT32 ShellIdx;
            UINT32 ShellCount = (UINT32)(sizeof(CalibMgrShellFunc)) / (UINT32)(sizeof(CalibMgrShellFunc[0]));

            if (CurArgCnt <= 2U) {
                PRN_CAL_MGR_LOG " " PRN_CAL_MGR_API
                PRN_CAL_MGR_LOG "====== %sCalib Manager Command Usage%s ======"
                    PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_TITLE_0 PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CSTR   SVC_LOG_CAL_HL_END     PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_API
            }

            for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
                if ((CalibMgrShellFunc[ShellIdx].pFunc != NULL) && (CalibMgrShellFunc[ShellIdx].Enable > 0U)) {
                    if (CurArgCnt <= 2U) {
                        if (CalibMgrShellFunc[ShellIdx].pUsage == NULL) {
                            PRN_CAL_MGR_LOG "  %s"
                                PRN_CAL_MGR_ARG_CSTR   CalibMgrShellFunc[ShellIdx].ShellCmdName PRN_CAL_MGR_ARG_POST
                            PRN_CAL_MGR_API
                        } else {
                            (CalibMgrShellFunc[ShellIdx].pUsage)();
                        }
                        RetVal = SVC_OK;
                    } else {
                        if (0 == SvcWrap_strcmp(pArgVector[2U], CalibMgrShellFunc[ShellIdx].ShellCmdName)) {
                            if (0U != (CalibMgrShellFunc[ShellIdx].pFunc)(CurArgCnt, pArgVector)) {
                                if (CalibMgrShellFunc[ShellIdx].pUsage != NULL) {
                                    (CalibMgrShellFunc[ShellIdx].pUsage)();
                                }
                            }
                            RetVal = SVC_OK;
                            break;
                        }
                    }
                }
            }
        } else {
            UINT32 ObjIdx;
            const SVC_CALIB_OBJ_CTRL_s *pObjCtrl = NULL;

            for (ObjIdx = 0U; ObjIdx < SVC_CalibMgr.NumOfCalObj; ObjIdx ++) {
                if (((SVC_CalibMgr.CalObjCtrl[ObjIdx].Status & SVC_CALIB_OBJ_INIT) > 0U) &&
                    (SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.Enable > 0U)) {
                    if (0 == SvcWrap_strcmp(pArgVector[1U], SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.Name)) {
                        pObjCtrl = &(SVC_CalibMgr.CalObjCtrl[ObjIdx]);
                        break;
                    }
                }
            }

            if (pObjCtrl != NULL) {
                if (pObjCtrl->CalObj.ShellEntry != NULL) {
                    if (0U != (pObjCtrl->CalObj.ShellEntry)(CurArgCnt, pArgVector)) {
                        PRN_CAL_MGR_LOG "Fail to proc calib mgr shell cmd!" PRN_CAL_MGR_NG
                    }
                }

                RetVal = SVC_OK;
            }
        }
    }

    if (RetVal != SVC_OK) {
        SvcCalibMgr_CmdAppUsage();
    }
}

/**
 * query total calib memory
 *
 * @param [in] pCfg calib configuration
 * @return ErrCode SVC_OK(0)|SVC_NG(1)
 */
UINT32 SvcCalib_MemQry(SVC_CALIB_CFG_s *pCfg)
{
    UINT32 RetVal = SVC_OK;

    if (pCfg == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to query calib memory - input calib configuration should not null!" PRN_CAL_MGR_NG
    } else if (pCfg->pCalObj == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to query calib memory - input calib object should not null!" PRN_CAL_MGR_NG
    } else {
        UINT32 Idx;
        UINT32 CalcMemSize = 0U;

        /* Start query each calib obj dram/working memory */
        for (Idx = 0U; Idx < pCfg->NumOfCalObj; Idx ++) {
            SVC_CALIB_OBJ_s *pObj = &(pCfg->pCalObj[Idx]);

            if (pObj->Enable > 0U) {
                if (pObj->NumOfTable == 0U) {
                    RetVal = SVC_NG;
                    PRN_CAL_MGR_LOG "Fail to query calib memory - CalObj(%d) NumOfTable should not zero"
                        PRN_CAL_MGR_ARG_UINT32 pObj->ID PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_NG
                } else if (pObj->CmdFunc == NULL) {
                    RetVal = SVC_NG;
                    PRN_CAL_MGR_LOG "Fail to query calib memory - CalObj(%d) command func should not null"
                        PRN_CAL_MGR_ARG_UINT32 pObj->ID PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_NG
                } else {
                    RetVal = (pObj->CmdFunc)(SVC_CALIB_CMD_MEM_QUERY, &(pObj->NumOfTable), &(pObj->ShadowBufSize), &(pObj->WorkingBufSize), &(pObj->CalcBufSize));
                    if (RetVal != SVC_OK) {
                        PRN_CAL_MGR_LOG "Fail to query calib memory - query CalObj(%d) memory fail!"
                            PRN_CAL_MGR_ARG_UINT32 pObj->ID PRN_CAL_MGR_ARG_POST
                        PRN_CAL_MGR_NG
                    } else {
                        if (CalcMemSize < pObj->CalcBufSize) {
                            CalcMemSize = pObj->CalcBufSize;
                        }

                        pObj->ShadowBufSize = GetAlignedValU32(pObj->ShadowBufSize, SVC_CALIB_NAND_SECTOR_SIZE);

                        PRN_CAL_MGR_LOG "Successful to query CalObj(%02d) memory 0x%08X / 0x%08X / 0x%08X"
                            PRN_CAL_MGR_ARG_UINT32 pObj->ID             PRN_CAL_MGR_ARG_POST
                            PRN_CAL_MGR_ARG_UINT32 pObj->ShadowBufSize  PRN_CAL_MGR_ARG_POST
                            PRN_CAL_MGR_ARG_UINT32 pObj->WorkingBufSize PRN_CAL_MGR_ARG_POST
                            PRN_CAL_MGR_ARG_UINT32 pObj->CalcBufSize    PRN_CAL_MGR_ARG_POST
                        PRN_CAL_MGR_DBG
                    }
                }

                if (RetVal != SVC_OK) {
                    break;
                }
            }
        }

        pCfg->CalcBufSize = CalcMemSize;
    }

    return RetVal;
}

/**
 * create calibration module
 *
 * @param [in] pCfg calibration configuration
 * @return ErrCode SVC_OK(0)|SVC_NG(1)
 */
UINT32 SvcCalib_Create(const SVC_CALIB_CFG_s *pCfg)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibMgrFlag & SVC_CALIB_MGR_FLG_MEM_INIT) == 0U) {
        AmbaSvcWrap_MisraMemset(&SVC_CalibMgr, 0, sizeof(SVC_CALIB_CTRL_s));
        SVC_CalibMgrFlag |= SVC_CALIB_MGR_FLG_MEM_INIT;
    }

    if (pCfg == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to create the calib manager - input configuration should not null!" PRN_CAL_MGR_NG
    } else if (pCfg->NumOfCalObj == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to create the calib manager - the calib object number should not zero!" PRN_CAL_MGR_NG
    } else if (pCfg->pCalObj == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to create the calib manager - the calib object should not null!" PRN_CAL_MGR_NG
    } else if ((SVC_CalibMgr.Status & SVC_CALIB_CTRL_INIT) > 0U) {
        PRN_CAL_MGR_LOG "The calib manager has been created!" PRN_CAL_MGR_OK
    } else {    /* Initial the calib manager setting */
        UINT32 ObjIdx;

        /* Reset the calib manager setting */
        AmbaSvcWrap_MisraMemset(&SVC_CalibMgr, 0, sizeof(SVC_CalibMgr));

        /* create the calib object control */
        for (ObjIdx = 0U; ObjIdx < pCfg->NumOfCalObj; ObjIdx ++) {
            if (pCfg->pCalObj[ObjIdx].Enable > 0U) {
                RetVal = SvcCalibMgr_ObjCtrlCreate(&(pCfg->pCalObj[ObjIdx]), &(SVC_CalibMgr.CalObjCtrl[SVC_CalibMgr.NumOfCalObj]));
                if (RetVal != SVC_OK) {
                    PRN_CAL_MGR_LOG "Fail to create the calib manager - initial calib id(%d) control fail!"
                        PRN_CAL_MGR_ARG_UINT32 pCfg->pCalObj[ObjIdx].ID PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_NG
                } else {
                    if (SVC_CalibMgr.CalObjCtrl[SVC_CalibMgr.NumOfCalObj].CalObj.CmdFunc == NULL) {
                        RetVal = SVC_NG;
                        PRN_CAL_MGR_LOG "Fail to create the calib manager - calib id(%d) control command func should not null!"
                            PRN_CAL_MGR_ARG_UINT32 pCfg->pCalObj[ObjIdx].ID PRN_CAL_MGR_ARG_POST
                        PRN_CAL_MGR_NG
                    }
                }

                if (RetVal == SVC_OK) {
                    SVC_CalibMgr.NumOfCalObj ++;
                } else {
                    break;
                }
            }
        }

        /* Configure the calculate memory */
        if (RetVal == SVC_OK) {
            SVC_CalibMgr.CalcMemCtrl.pBuf    = pCfg->pCalcBuf;
            SVC_CalibMgr.CalcMemCtrl.BufSize = pCfg->CalcBufSize;

            if ((SVC_CalibMgr.CalcMemCtrl.pBuf != NULL) &&
                (SVC_CalibMgr.CalcMemCtrl.BufSize != 0U)) {
                SVC_CalibMgr.CalcMemCtrl.State = SVC_CALIB_CTRL_CALC_MEM_INIT;
            } else {
                SVC_CalibMgr.CalcMemCtrl.State = 0U;
            }
        }

        SVC_CalibMgr.CalcCbFunc = pCfg->CalcCbFunc;

        /* Update the status */
        if (RetVal == SVC_OK) {
            SVC_CalibMgr.Status = SVC_CALIB_CTRL_INIT;
            PRN_CAL_MGR_LOG "Successful to create the calib manager!" PRN_CAL_MGR_OK
        }

        /* Print the memory setting */
        {
            PRN_CAL_MGR_LOG "==== Calibration Memory Setting ====" PRN_CAL_MGR_API
#ifndef CONFIG_ARM64
            PRN_CAL_MGR_LOG "  ID | Src | Shadow Buffer | Shadow Size | Working Buffer | Working Size | Calc Size  | Name" PRN_CAL_MGR_API
#else
            PRN_CAL_MGR_LOG "  ID | Src |    Shadow Buffer   | Shadow Size |   Working Buffer   | Working Size | Calc Size  | Name" PRN_CAL_MGR_API
#endif

            for (ObjIdx = 0U; ObjIdx < SVC_CalibMgr.NumOfCalObj; ObjIdx ++) {
#ifndef CONFIG_ARM64
                PRN_CAL_MGR_LOG "  %2d |  %02d |   %p  |  0x%08X |   %p   |  0x%08X  | 0x%08X | %s"
#else
                PRN_CAL_MGR_LOG "  %2d |  %02d | %p |  0x%08X | %p |  0x%08X  | 0x%08X | %s"
#endif
                    PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.ID             PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.DataSource     PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CPOINT SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.pShadowBuf     PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.ShadowBufSize  PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CPOINT SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.pWorkingBuf    PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.WorkingBufSize PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.CalcBufSize    PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_CSTR   SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.Name           PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_API
            }

            PRN_CAL_MGR_LOG "  Calculation Buffer | Calculation Size" PRN_CAL_MGR_API
#ifndef CONFIG_ARM64
            PRN_CAL_MGR_LOG "      %p     |   0x%08X"
#else
            PRN_CAL_MGR_LOG "  %p |   0x%08X"
#endif
                PRN_CAL_MGR_ARG_CPOINT SVC_CalibMgr.CalcMemCtrl.pBuf    PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalcMemCtrl.BufSize PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_API
        }

        /* Create the each item */
        {
            SVC_CALIB_OBJ_CTRL_s *pCalObjCtrl;

            for (ObjIdx = 0U; ObjIdx < SVC_CalibMgr.NumOfCalObj; ObjIdx ++) {

                pCalObjCtrl = &(SVC_CalibMgr.CalObjCtrl[ObjIdx]);

                if (pCalObjCtrl->CalObj.CmdFunc != NULL) {
                    RetVal = SvcCalibMgr_MutexTake(&(pCalObjCtrl->Mutex), AMBA_KAL_WAIT_FOREVER);
                    if (RetVal != KAL_ERR_NONE) {
                        RetVal = SVC_NG;
                        PRN_CAL_MGR_LOG "Fail to init calib item - take mutex fail!" PRN_CAL_MGR_NG
                    } else {
                        if (0U != (pCalObjCtrl->CalObj.CmdFunc)(SVC_CALIB_CMD_ITEM_CREATE, NULL, NULL, NULL, NULL)) {
                            PRN_CAL_MGR_LOG "Fail to init calib item - create fail! CalID(%d)"
                                PRN_CAL_MGR_ARG_UINT32 ObjIdx PRN_CAL_MGR_ARG_POST
                            PRN_CAL_MGR_NG
                        }

                        SvcCalibMgr_MutexGive(&(pCalObjCtrl->Mutex));
                    }
                }
            }
        }
    }

    return RetVal;
}

/**
 * load calib data from NVM/ROM/FILE
 *
 * @param [in] CalID calib identification
 * @return ErrCode SVC_OK(0)|SVC_NG(1)
 */
UINT32 SvcCalib_DataLoad(UINT32 CalID)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibMgrFlag & SVC_CALIB_MGR_FLG_MEM_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to load data - initial calib control first!" PRN_CAL_MGR_NG
    } else if ((SVC_CalibMgr.Status & SVC_CALIB_CTRL_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to load data - initial calib control first!!!" PRN_CAL_MGR_NG
    } else {
        SVC_CALIB_OBJ_CTRL_s *pCalObjCtrl = NULL;

        SvcCalibMgr_NvmInit();

        /* Get the calib object control by ID */
        pCalObjCtrl = SvcCalibMgr_ObjCtrlGet(CalID);
        if (pCalObjCtrl == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to load data - invalid calib object control. CalID(%d)"
                PRN_CAL_MGR_ARG_UINT32 CalID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        } else if (pCalObjCtrl->CalObj.CmdFunc == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to load data - calib id(%d) command func should not null!"
                PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        } else if ((pCalObjCtrl->Status & SVC_CALIB_OBJ_INIT) == 0U) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to load data - initial calib(%d) object control first!"
                PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        } else if (pCalObjCtrl->CalObj.pShadowBuf == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to load data - the calib(%d) object dram shadow should not null!"
                PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        } else {
            const SVC_CALIB_OBJ_s *pCalObj = &(pCalObjCtrl->CalObj);
            UINT32 CalObjAddr;
            AmbaMisra_TypeCast(&CalObjAddr, &pCalObj);

            /* Take mutex before loading process */
            RetVal = SvcCalibMgr_MutexTake(&(pCalObjCtrl->Mutex), AMBA_KAL_WAIT_FOREVER);
            if (RetVal != KAL_ERR_NONE) {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to load data - take mutex fail!" PRN_CAL_MGR_NG
            } else {
                void *pShadowBuf;
                AmbaMisra_TypeCast(&pShadowBuf, &(pCalObjCtrl->CalObj.pShadowBuf));

                /* Reset the status */
                pCalObjCtrl->Status &= ~SVC_CALIB_OBJ_LOAD_DONE;

                /* Reset the dram shadow buffer */
                AmbaSvcWrap_MisraMemset(pShadowBuf, 0, pCalObjCtrl->CalObj.ShadowBufSize);

                /* Load the data */
                if (pCalObjCtrl->CalObj.DataSource == SVC_CALIB_DATA_SOURCE_NAND) {
                    if ((SVC_CalibMgr.Status & SVC_CALIB_CTRL_LOAD_HEADER) == 0U) {
                        AmbaSvcWrap_MisraMemset(&(SVC_CalibMgr.CalNandInfo), 0, sizeof(AMBA_NVM_FTL_INFO_s));
                        RetVal = SvcCalibMgr_NvmLoadInfo(&(SVC_CalibMgr.CalNandInfo));
                        if (RetVal != SVC_OK) {
                            PRN_CAL_MGR_LOG "Fail to load calib nand info!" PRN_CAL_MGR_NG
                        } else {
                            AmbaSvcWrap_MisraMemset(&(SVC_CalibMgr.CalHeader), 0, sizeof(SVC_CALIB_NAND_HEADER_s));
                            RetVal = SvcCalibMgr_NvmLoadHeader(&(SVC_CalibMgr.CalHeader));
                            if (RetVal != SVC_OK) {
                                PRN_CAL_MGR_LOG "Fail to load calib header" PRN_CAL_MGR_NG
                            } else {
                                UINT32 ObjIdx, TotalShadowSize = 0U;
                                UINT64 TempU64 = 0ULL;

                                SVC_CalibMgr.Status |= SVC_CALIB_CTRL_LOAD_HEADER;
                                PRN_CAL_MGR_LOG "Successful to load calib header" PRN_CAL_MGR_OK

                                for (ObjIdx = 0U; ObjIdx < SVC_CalibMgr.NumOfCalObj; ObjIdx ++) {
                                    if ((SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.Enable > 0U) &&
                                        (SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.DataSource == SVC_CALIB_DATA_SOURCE_NAND)) {
                                        TotalShadowSize += SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.ShadowBufSize;
                                    }
                                }
                                TotalShadowSize += (UINT32)(sizeof(SVC_CALIB_NAND_HEADER_s));

                                TempU64  = (UINT64)(TotalShadowSize);
                                TempU64 &= 0x00000000FFFFFFFFULL;

                                if (TempU64 > SVC_CalibMgr.CalNandInfo.TotalSizeInBytes) {
                                    RetVal = SVC_NG;
                                    PRN_CAL_MGR_LOG "Fail to load data - total shadow size(%llu) > current calibration nand size(%llu)"
                                        PRN_CAL_MGR_ARG_UINT64 TempU64
                                        PRN_CAL_MGR_ARG_UINT64 SVC_CalibMgr.CalNandInfo.TotalSizeInBytes
                                    PRN_CAL_MGR_NG
                                }
                            }
                        }
                    }
                    if (RetVal == SVC_OK) {
                        RetVal = SvcCalibMgr_NvmLoadData(&(pCalObjCtrl->CalObj));
                    }
                } else if (pCalObjCtrl->CalObj.DataSource == SVC_CALIB_DATA_SOURCE_ROMFS) {
                    RetVal = SvcCalibMgr_RomLoadData(&(pCalObjCtrl->CalObj));
                } else if (pCalObjCtrl->CalObj.DataSource == SVC_CALIB_DATA_SOURCE_FILE) {
                    RetVal = SvcCalibMgr_FileLoadData(&(pCalObjCtrl->CalObj));
                } else { // SVC_CALIB_DATA_SOURCE_USER
                    /* Do nothing */
                }

                if (RetVal == SVC_OK) {
                    pCalObjCtrl->Status |= SVC_CALIB_OBJ_LOAD_DONE;
                    PRN_CAL_MGR_LOG "Successful to load data. calib(%d)"
                        PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_OK
                } else {
                    PRN_CAL_MGR_LOG "Invalid return value. Should not happen!" PRN_CAL_MGR_NG
                }

                SvcCalibMgr_MutexGive(&(pCalObjCtrl->Mutex));
            }
        }
    }

    return RetVal;
}


/**
 * load all calib data from NVM/ROM/FILE
 *
 * @return ErrCode SVC_OK(0)|SVC_NG(1)
 */
UINT32 SvcCalib_DataLoadAll(void)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibMgrFlag & SVC_CALIB_MGR_FLG_MEM_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to load all data - initial calib control first!" PRN_CAL_MGR_NG
    } else if ((SVC_CalibMgr.Status & SVC_CALIB_CTRL_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to load all data - initial calib control first!!!" PRN_CAL_MGR_NG
    } else if (SVC_CalibMgr.NumOfCalObj == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to load all data - number of calib object should not zero" PRN_CAL_MGR_NG
    } else {

        SvcCalibMgr_NvmInit();

        /* Load NAND info */
        AmbaSvcWrap_MisraMemset(&(SVC_CalibMgr.CalNandInfo), 0, sizeof(AMBA_NVM_FTL_INFO_s));
        RetVal = SvcCalibMgr_NvmLoadInfo(&(SVC_CalibMgr.CalNandInfo));
        if (RetVal != SVC_OK) {
            PRN_CAL_MGR_LOG "Fail to load all data - load calib info fail!" PRN_CAL_MGR_NG
        } else {
            if (SVC_CalibMgr.CalNandInfo.Initialized == 0U) {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to load all data - the calibration nand partition is not initialized!" PRN_CAL_MGR_NG
            }
        }

        /* Check the shadow size larger than nand size or not */
        if (RetVal == SVC_OK) {
            UINT32 ObjIdx, TotalShadowSize = 0U;
            UINT64 TempU64 = 0ULL;

            for (ObjIdx = 0U; ObjIdx < SVC_CalibMgr.NumOfCalObj; ObjIdx ++) {
                if ((SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.Enable > 0U) &&
                    (SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.DataSource == SVC_CALIB_DATA_SOURCE_NAND)) {
                    TotalShadowSize += SVC_CalibMgr.CalObjCtrl[ObjIdx].CalObj.ShadowBufSize;
                }
            }
            TotalShadowSize += (UINT32)(sizeof(SVC_CALIB_NAND_HEADER_s));

            TempU64  = (UINT64)(TotalShadowSize);
            TempU64 &= 0x00000000FFFFFFFFULL;

            if (TempU64 > SVC_CalibMgr.CalNandInfo.TotalSizeInBytes) {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to load data - total shadow size(%llu) > current calibration nand size(%llu)"
                    PRN_CAL_MGR_ARG_UINT64 TempU64
                    PRN_CAL_MGR_ARG_UINT64 SVC_CalibMgr.CalNandInfo.TotalSizeInBytes
                PRN_CAL_MGR_NG
            }
        }

        /* Load NAND header */
        if (RetVal == SVC_OK) {
            AmbaSvcWrap_MisraMemset(&(SVC_CalibMgr.CalHeader), 0, sizeof(SVC_CALIB_NAND_HEADER_s));
            RetVal = SvcCalibMgr_NvmLoadHeader(&(SVC_CalibMgr.CalHeader));
            if (RetVal != SVC_OK) {
                PRN_CAL_MGR_LOG "Fail to load all data - load calib nand header fail!" PRN_CAL_MGR_NG
            } else {
                SVC_CalibMgr.Status |= SVC_CALIB_CTRL_LOAD_HEADER;
            }
        }

        /* Load each calib item data */
        if (RetVal == SVC_OK) {
            UINT32 Idx;

            for (Idx = 0; Idx < SVC_CalibMgr.NumOfCalObj; Idx ++) {
                RetVal |= SvcCalib_DataLoad(SVC_CalibMgr.CalObjCtrl[Idx].CalObj.ID);
            }
        }
    }

    return RetVal;
}

/**
 * Save calib data to NVM/FILE
 *
 * @param [in] CalID calib identification
 * @return ErrCode SVC_OK(0)|SVC_NG(1)
 */
UINT32 SvcCalib_DataSave(UINT32 CalID)
{
    UINT32 RetVal = SVC_OK;
    SVC_CALIB_OBJ_CTRL_s *pCalObjCtrl = NULL;

    if ((SVC_CalibMgrFlag & SVC_CALIB_MGR_FLG_MEM_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to save data - initial calib control first!" PRN_CAL_MGR_NG
    } else if ((SVC_CalibMgr.Status & SVC_CALIB_CTRL_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to save data - initial calib control first!!!" PRN_CAL_MGR_NG
    } else {
        /* Get the calib object control by ID */
        pCalObjCtrl = SvcCalibMgr_ObjCtrlGet(CalID);
        if (pCalObjCtrl == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to save data - invalid calib object control. CalID(%d)"
                PRN_CAL_MGR_ARG_UINT32 CalID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        } else if ((pCalObjCtrl->Status & SVC_CALIB_OBJ_INIT) == 0U) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to save data - initial calib(%d) object control first!"
                PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        } else if ((pCalObjCtrl->CalObj.DataSource == SVC_CALIB_DATA_SOURCE_NAND) &&
                   ((SVC_CalibMgr.Status & SVC_CALIB_CTRL_LOAD_HEADER) == 0U)) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to save data - invalid calib NAND partition header. calib id(%d)"
                PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        } else {
            if (pCalObjCtrl->CalObj.DataSource != SVC_CALIB_DATA_SOURCE_USER) {
                RetVal = SvcCalibMgr_MutexTake(&(pCalObjCtrl->Mutex), AMBA_KAL_WAIT_FOREVER);
                if (RetVal != KAL_ERR_NONE) {
                    RetVal = SVC_NG;
                    PRN_CAL_MGR_LOG "Fail to save data - take mutex fail!" PRN_CAL_MGR_NG
                } else {
                    if (pCalObjCtrl->CalObj.DataSource == SVC_CALIB_DATA_SOURCE_NAND) {
                        RetVal = SvcCalibMgr_NvmSaveData(&(pCalObjCtrl->CalObj));
                        if (RetVal != SVC_OK) {
                            PRN_CAL_MGR_LOG "Fail to save data - save calib id(%d) to NAND fail!"
                                PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
                            PRN_CAL_MGR_NG
                        }

                        if (SvcWrap_strlen(pCalObjCtrl->CalObj.DataSourcePath) != 0U) {
                            if (OK == SvcCalibMgr_FileSaveData(&(pCalObjCtrl->CalObj))) {
                                PRN_CAL_MGR_LOG "Successful to save data to file" PRN_CAL_MGR_API
                            }
                        }
                    } else if ((pCalObjCtrl->CalObj.DataSource == SVC_CALIB_DATA_SOURCE_ROMFS) ||
                               (pCalObjCtrl->CalObj.DataSource == SVC_CALIB_DATA_SOURCE_FILE)) {
                        RetVal = SvcCalibMgr_FileSaveData(&(pCalObjCtrl->CalObj));
                        if (RetVal != SVC_OK) {
                            PRN_CAL_MGR_LOG "Fail to save data - save calib id(%d) to file fail!"
                                PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
                            PRN_CAL_MGR_NG
                        }
                    } else {
                        RetVal = SVC_NG;
                        PRN_CAL_MGR_LOG "Fail to save data - invalid data source type(%d)"
                            PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.DataSource PRN_CAL_MGR_ARG_POST
                        PRN_CAL_MGR_NG
                    }

                    if (RetVal == SVC_OK) {
                        PRN_CAL_MGR_LOG "Successful to save data. calib(%d) "
                            PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
                        PRN_CAL_MGR_OK
                    }

                    SvcCalibMgr_MutexGive(&(pCalObjCtrl->Mutex));
                }
            }
        }
    }

    return RetVal;
}

/**
 * initial calib item
 *
 * @param [in] CalID calibration identification
 * @return ErrCode SVC_OK(0)|SVC_NG(1)
 */
UINT32 SvcCalib_ItemDataInit(UINT32 CalID)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibMgrFlag & SVC_CALIB_MGR_FLG_MEM_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to init calib item - initial calib control first!" PRN_CAL_MGR_NG
    } else if ((SVC_CalibMgr.Status & SVC_CALIB_CTRL_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to init calib item - initial calib control first!!!" PRN_CAL_MGR_NG
    } else {
        SVC_CALIB_OBJ_CTRL_s *pCalObjCtrl = SvcCalibMgr_ObjCtrlGet(CalID);

        if (pCalObjCtrl == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to init dram shadow - invalid calib object control. CalID(%d)"
                PRN_CAL_MGR_ARG_UINT32 CalID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        } else if (pCalObjCtrl->CalObj.CmdFunc == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to init calib item - calib id(%d) command func should not null!"
                PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        } else if ((pCalObjCtrl->Status & SVC_CALIB_OBJ_INIT) == 0U) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to init calib item - initial calib(%d) object control first!"
                PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        } else if ((pCalObjCtrl->Status & SVC_CALIB_OBJ_SHADOW_INIT) > 0U) {
            RetVal = 99U;
            PRN_CAL_MGR_LOG "Calib id(%d) dram shadow has been initialized!"
                PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_DBG
        } else {
            SVC_CALIB_OBJ_s *pCalObj = &(pCalObjCtrl->CalObj);
            SVC_CALIB_CALC_CB_f CalcCb = SVC_CalibMgr.CalcCbFunc;
            void *pArg2 = NULL;

            if ((pCalObjCtrl->Status & SVC_CALIB_OBJ_LOAD_DONE) == 0U) {
                PRN_CAL_MGR_LOG "Calib id(%d) data is empty!"
                    PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_DBG
            }

            AmbaMisra_TypeCast(&pArg2, &CalcCb);

            RetVal = SvcCalibMgr_MutexTake(&(pCalObjCtrl->Mutex), AMBA_KAL_WAIT_FOREVER);
            if (RetVal != KAL_ERR_NONE) {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to init calib item - take mutex fail!" PRN_CAL_MGR_NG
            } else {
                /* Reset the status */
                pCalObjCtrl->Status &= ~SVC_CALIB_OBJ_SHADOW_INIT;

                RetVal = (pCalObjCtrl->CalObj.CmdFunc)(SVC_CALIB_CMD_ITEM_DATA_INIT, pCalObj, pArg2, NULL, NULL);
                if (RetVal != SVC_OK) {
                    PRN_CAL_MGR_LOG "Fail to init calib item - initial calib(%d) fail!"
                        PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_NG
                } else {
                    pCalObjCtrl->Status |= SVC_CALIB_OBJ_SHADOW_INIT;
                    PRN_CAL_MGR_LOG "Successful to init calib(%d) item"
                        PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_OK
                }

                SvcCalibMgr_MutexGive(&(pCalObjCtrl->Mutex));
            }
        }
    }

    if (RetVal == 99U) {
        RetVal = SVC_OK;
    }

    return RetVal;
}

/**
 * initial calib item
 *
 * @return ErrCode SVC_OK(0)|SVC_NG(1)
 */
UINT32 SvcCalib_ItemDataInitAll(void)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibMgrFlag & SVC_CALIB_MGR_FLG_MEM_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to initial all dram shadow - initial calib control first!" PRN_CAL_MGR_NG
    } else if ((SVC_CalibMgr.Status & SVC_CALIB_CTRL_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to initial all dram shadow - initial calib control first!!!" PRN_CAL_MGR_NG
    } else if (SVC_CalibMgr.NumOfCalObj == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to initial all dram shadow - number of calib object should not zero" PRN_CAL_MGR_NG
    } else {
        UINT32 Idx;

        for (Idx = 0U; Idx < SVC_CalibMgr.NumOfCalObj; Idx ++) {
            RetVal |= SvcCalib_ItemDataInit(SVC_CalibMgr.CalObjCtrl[Idx].CalObj.ID);
        }
    }

    return RetVal;
}

/**
 * config calib item
 *
 * @param [in] CalID calibration identification
 * @param [in] CmdID config command
 * #param [in] pCfg config info
 * @return ErrCode SVC_OK(0)|SVC_NG(1)
 */
UINT32 SvcCalib_ItemTableCfg(UINT32 CalID, UINT32 CmdID, const void *pCfg)
{
    UINT32 RetVal = SVC_OK;
    SVC_CALIB_OBJ_CTRL_s *pCalObjCtrl = NULL;

    if ((SVC_CalibMgrFlag & SVC_CALIB_MGR_FLG_MEM_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to cfg table - initial calib control first!" PRN_CAL_MGR_NG
    } else if ((SVC_CalibMgr.Status & SVC_CALIB_CTRL_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to cfg table - initial calib control first!!!" PRN_CAL_MGR_NG
    } else {
        pCalObjCtrl = SvcCalibMgr_ObjCtrlGet(CalID);
        if (pCalObjCtrl == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to cfg table - invalid calib object control. CalID(%d)"
                PRN_CAL_MGR_ARG_UINT32 CalID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        } else if (pCalObjCtrl->CalObj.CmdFunc == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to cfg table - calib id(%d) command func should not null!"
                PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        } else if ((pCalObjCtrl->Status & SVC_CALIB_OBJ_INIT) == 0U) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to cfg table - initial calib(%d) object control first!"
                PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        } else if ((pCalObjCtrl->Status & SVC_CALIB_OBJ_LOAD_DONE) == 0U) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to cfg table - loading calib id(%d) data first!"
                PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_DBG
        } else if ((pCalObjCtrl->Status & SVC_CALIB_OBJ_SHADOW_INIT) == 0U) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to cfg table - initial calib id(%d) dram shadow first!"
                PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_DBG
        } else {
            RetVal = SvcCalibMgr_MutexTake(&(pCalObjCtrl->Mutex), AMBA_KAL_WAIT_FOREVER);
            if (RetVal != KAL_ERR_NONE) {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to cfg table - take mutex fail!" PRN_CAL_MGR_NG
            } else {
                void *pArg2 = NULL;

                AmbaMisra_TypeCast(&(pArg2), &(pCfg));

                RetVal = (pCalObjCtrl->CalObj.CmdFunc)(SVC_CALIB_CMD_ITEM_TBL_CFG, &CmdID, pArg2, NULL, NULL);
                if (RetVal != SVC_OK) {
                    PRN_CAL_MGR_LOG "Fail to cfg table - initial calib(%d) fail!"
                        PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_DBG
                } else {
                    PRN_CAL_MGR_LOG "Successful to cfg table. calib(%d)"
                        PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_DBG
                }

                SvcCalibMgr_MutexGive(&(pCalObjCtrl->Mutex));
            }
        }
    }

    return RetVal;
}

/**
 * update calib item table
 *
 * @param [in] CalID calibration identification
 * @param [in] pCalChan calib channel
 * #param [in] pImgMode image kernel configuration
 * @return ErrCode SVC_OK(0)|SVC_NG(1)
 */
UINT32 SvcCalib_ItemTableUpdate(UINT32 CalID, const SVC_CALIB_CHANNEL_s *pCalChan, const AMBA_IK_MODE_CFG_s *pImgMode)
{
    UINT32 RetVal = SVC_OK;
    SVC_CALIB_OBJ_CTRL_s *pCalObjCtrl = NULL;

    if ((SVC_CalibMgrFlag & SVC_CALIB_MGR_FLG_MEM_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to update table - initial calib control first!" PRN_CAL_MGR_NG
    } else if ((SVC_CalibMgr.Status & SVC_CALIB_CTRL_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to update table - initial calib control first!!!" PRN_CAL_MGR_NG
    } else if (pCalChan == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to update table - input calib channel should not null!" PRN_CAL_MGR_NG
    } else if (pImgMode == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to update table - input image mode should not null!" PRN_CAL_MGR_NG
    } else if (pImgMode->ContextId >= AMBA_DSP_MAX_VIEWZONE_NUM) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to update table - ik context id(%d) is out-of max viewzone!"
            PRN_CAL_MGR_ARG_UINT32 pImgMode->ContextId PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_NG
    } else {

        /* Get the calib object control by ID */
        pCalObjCtrl = SvcCalibMgr_ObjCtrlGet(CalID);
        if (pCalObjCtrl == NULL) {
            RetVal = SVC_NG;
            if ((SVC_CalibMgrFlag & SVC_LOG_CAL_DBG) > 0U) {
                PRN_CAL_MGR_LOG "Fail to update table - invalid calib object control. CalID(%d)"
                    PRN_CAL_MGR_ARG_UINT32 CalID PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_NG
            }
        } else if (pCalObjCtrl->CalObj.CmdFunc == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to update table - calib id(%d) command func should not null!"
                PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        } else if ((pCalObjCtrl->Status & SVC_CALIB_OBJ_INIT) == 0U) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to update table - initial calib(%d) object control first!"
                PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        } else if ((pCalObjCtrl->Status & SVC_CALIB_OBJ_LOAD_DONE) == 0U) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to update table - loading calib id(%d) data first!"
                PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_DBG
        } else if ((pCalObjCtrl->Status & SVC_CALIB_OBJ_SHADOW_INIT) == 0U) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to update table - initial calib id(%d) dram shadow first!"
                PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_DBG
        } else {
            RetVal = SvcCalibMgr_MutexTake(&(pCalObjCtrl->Mutex), AMBA_KAL_WAIT_FOREVER);
            if (RetVal != KAL_ERR_NONE) {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to update table - take mutex fail!" PRN_CAL_MGR_NG
            } else {
                void *pArg1 = NULL, *pArg2 = NULL;

                AmbaMisra_TypeCast(&(pArg1), &(pCalChan));
                AmbaMisra_TypeCast(&(pArg2), &(pImgMode));

                if (SVC_OK != (pCalObjCtrl->CalObj.CmdFunc)(SVC_CALIB_CMD_ITEM_TBL_UPDATE, pArg1, pArg2, NULL, NULL)) {
                    PRN_CAL_MGR_LOG "Fail to update calib(%d) table. VinID(%d), SensorID(%d)"
                        PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
                        PRN_CAL_MGR_ARG_UINT32 pCalChan->VinID        PRN_CAL_MGR_ARG_POST
                        PRN_CAL_MGR_ARG_UINT32 pCalChan->SensorID     PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_DBG
                } else {
                    PRN_CAL_MGR_LOG "Successful to update table. calib(%d)"
                        PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_OK
                }

                SvcCalibMgr_MutexGive(&(pCalObjCtrl->Mutex));
            }
        }
    }

    return RetVal;
}

/**
 * get calib item table
 *
 * @param [in] CalID calibration identification
 * @param [in] TblId item table identification
 * #param [in] pTblInfo table info
 * @return ErrCode SVC_OK(0)|SVC_NG(1)
 */
UINT32 SvcCalib_ItemTableGet(UINT32 CalID, UINT32 TblId, SVC_CALIB_TBL_INFO_s *pTblInfo)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    SVC_CALIB_OBJ_CTRL_s *pCalObjCtrl = NULL;

    if ((SVC_CalibMgrFlag & SVC_CALIB_MGR_FLG_MEM_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to get calib table - initial calib control first!" PRN_CAL_MGR_NG
    } else if ((SVC_CalibMgr.Status & SVC_CALIB_CTRL_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to get calib table - initial calib control first!!!" PRN_CAL_MGR_NG
    } else if ((SVC_CalibMgr.Status & SVC_CALIB_CTRL_LOAD_HEADER) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to get calib table - load calib data first!!!" PRN_CAL_MGR_NG
    } else if (pTblInfo == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to get calib table - output table info should not null" PRN_CAL_MGR_NG
    } else if (pTblInfo->pBuf == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to get calib table - output table info buffer should not null" PRN_CAL_MGR_NG
    } else {
        /* Get the calib object control by ID */
        pCalObjCtrl = SvcCalibMgr_ObjCtrlGet(CalID);
        if (pCalObjCtrl == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to get calib table - invalid calib object control. CalID(%d)"
                PRN_CAL_MGR_ARG_UINT32 CalID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        } else if (pCalObjCtrl->CalObj.CmdFunc == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to get calib table - calib id(%d) command func should not null!"
                PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        } else if ((pCalObjCtrl->Status & SVC_CALIB_OBJ_INIT) == 0U) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to get calib table - initial calib(%d) object control first!"
                PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        } else if ((pCalObjCtrl->Status & SVC_CALIB_OBJ_LOAD_DONE) == 0U) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to get calib table - loading calib id(%d) data first!"
                PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_DBG
        } else if ((pCalObjCtrl->Status & SVC_CALIB_OBJ_SHADOW_INIT) == 0U) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to get calib table - initial calib id(%d) dram shadow first!"
                PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_DBG
        } else {
            PRetVal = SvcCalibMgr_MutexTake(&(pCalObjCtrl->Mutex), AMBA_KAL_WAIT_FOREVER);
            if (PRetVal != KAL_ERR_NONE) {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to get calib table - take mutex fail!" PRN_CAL_MGR_NG
            } else {

                RetVal = (pCalObjCtrl->CalObj.CmdFunc)(SVC_CALIB_CMD_ITEM_TBL_GET, &TblId, pTblInfo, NULL, NULL);
                if (RetVal == SVC_OK) {
                    PRN_CAL_MGR_LOG "Successful to get calib table. calib(%d)"
                        PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_OK
                }

                SvcCalibMgr_MutexGive(&(pCalObjCtrl->Mutex));
            }
        }
    }

    return RetVal;
}

/**
 * set calib item table
 *
 * @param [in] CalID calibration identification
 * @param [in] TblId item table identification
 * #param [in] pTblInfo table info
 * @return ErrCode SVC_OK(0)|SVC_NG(1)
 */
UINT32 SvcCalib_ItemTableSet(UINT32 CalID, UINT32 TblId, SVC_CALIB_TBL_INFO_s *pTblInfo)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    SVC_CALIB_OBJ_CTRL_s *pCalObjCtrl = NULL;

    if ((SVC_CalibMgrFlag & SVC_CALIB_MGR_FLG_MEM_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to set calib table - initial calib control first!" PRN_CAL_MGR_NG
    } else if ((SVC_CalibMgr.Status & SVC_CALIB_CTRL_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to set calib table - initial calib control first!!!" PRN_CAL_MGR_NG
    } else if ((SVC_CalibMgr.Status & SVC_CALIB_CTRL_LOAD_HEADER) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to set calib table - load calib data first!!!" PRN_CAL_MGR_NG
    } else {
        /* Set the calib object control by ID */
        pCalObjCtrl = SvcCalibMgr_ObjCtrlGet(CalID);
        if (pCalObjCtrl == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to set calib table - invalid calib object control. CalID(%d)"
                PRN_CAL_MGR_ARG_UINT32 CalID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        } else if (pCalObjCtrl->CalObj.CmdFunc == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to set calib table - calib id(%d) command func should not null!"
                PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        } else if ((pCalObjCtrl->Status & SVC_CALIB_OBJ_INIT) == 0U) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to set calib table - initial calib(%d) object control first!"
                PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        } else if ((pCalObjCtrl->Status & SVC_CALIB_OBJ_LOAD_DONE) == 0U) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to set calib table - loading calib id(%d) data first!"
                PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_DBG
        } else if ((pCalObjCtrl->Status & SVC_CALIB_OBJ_SHADOW_INIT) == 0U) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to set calib table - initial calib id(%d) dram shadow first!"
                PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_DBG
        } else {
            PRetVal = SvcCalibMgr_MutexTake(&(pCalObjCtrl->Mutex), AMBA_KAL_WAIT_FOREVER);
            if (PRetVal != KAL_ERR_NONE) {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to set calib table - take mutex fail!" PRN_CAL_MGR_NG
            } else {

                RetVal = (pCalObjCtrl->CalObj.CmdFunc)(SVC_CALIB_CMD_ITEM_TBL_SET, &TblId, pTblInfo, NULL, NULL);
                if (RetVal == SVC_OK) {
                    PRN_CAL_MGR_LOG "Successful to set calib table. calib(%d)"
                        PRN_CAL_MGR_ARG_UINT32 pCalObjCtrl->CalObj.ID PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_OK
                }

                SvcCalibMgr_MutexGive(&(pCalObjCtrl->Mutex));
            }
        }
    }

    return RetVal;
}

/**
 * set the calculation memory
 *
 * @param [in] BufAddr buffer base
 * @param [in] BufSize buffer size
 * @return ErrCode SVC_OK(0)|SVC_NG(1)
 */
UINT32 SvcCalib_ItemCalcMemSet(UINT8 *pBuf, UINT32 BufSize)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibMgrFlag & SVC_CALIB_MGR_FLG_MEM_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to get calib calculate mem - initial calib control first!" PRN_CAL_MGR_NG
    } else if ((SVC_CalibMgr.Status & SVC_CALIB_CTRL_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to get calib calculate mem - initial calib control first!!!" PRN_CAL_MGR_NG
    } else {
        if ((pBuf != NULL) && (BufSize > 0U)) {
            SVC_CalibMgr.CalcMemCtrl.pBuf    = pBuf;
            SVC_CalibMgr.CalcMemCtrl.BufSize = BufSize;
            SVC_CalibMgr.CalcMemCtrl.State  |= SVC_CALIB_CTRL_CALC_MEM_INIT;

            PRN_CAL_MGR_LOG "Calculation Buffer | Calculation Size" PRN_CAL_MGR_API
            PRN_CAL_MGR_LOG "    %p     |   0x%08X"
                PRN_CAL_MGR_ARG_CPOINT SVC_CalibMgr.CalcMemCtrl.pBuf    PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalcMemCtrl.BufSize PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_API
        }
    }

    return RetVal;
}

/**
 * get the calculation memory
 * If some calib item get the calculation buffer, the state will change to locked.
 *
 * @param [in] CalID calib identification
 * @param [out] pBuf calculation buffer base
 * @param [out] pBufSize calculation buffer size
 * @return ErrCode SVC_OK(0)|SVC_NG(1)
 */
UINT32 SvcCalib_ItemCalcMemGet(UINT32 CalID, UINT32 ReqNum, UINT8 **pBuf, UINT32 *pBufSize)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibMgrFlag & SVC_CALIB_MGR_FLG_MEM_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to get calib calculate mem - initial calib control first!" PRN_CAL_MGR_NG
    } else if ((SVC_CalibMgr.Status & SVC_CALIB_CTRL_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to get calib calculate mem - initial calib control first!!!" PRN_CAL_MGR_NG
    } else if (CalID >= SVC_CALIB_MAX_OBJ_NUM) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to get calib calculate mem - invalid calibration id(%d)"
            PRN_CAL_MGR_ARG_UINT32 CalID PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_NG
    } else if (pBuf == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to get calib calculate mem - output buf address should not null!" PRN_CAL_MGR_NG
    } else if (pBufSize == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to get calib calculate mem - output buf size should not null!" PRN_CAL_MGR_NG
    } else if ((SVC_CalibMgr.CalcMemCtrl.State & 0xFFFFFFFCU) > 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to get calib calculate mem - the memory has been locked! 0x%08X"
            PRN_CAL_MGR_ARG_UINT32 SVC_CalibMgr.CalcMemCtrl.State PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_NG
    } else {
        UINT32 ReqIdx;
        UINT32 CurReqNum = ReqNum;

        if ((SVC_CalibMgr.CalcMemCtrl.State & SVC_CALIB_CTRL_CALC_MEM_INIT) > 0U) {
            UINT32 CurReqMemSize = 0U;

            for (ReqIdx = 0U; ReqIdx < CurReqNum; ReqIdx ++) {
                CurReqMemSize += GetAlignedValU32(pBufSize[ReqIdx], 64U);
            }

            if (CurReqMemSize == 0U) {
                if (0U != SvcCalibMgr_CalcMemSizeGet(&CurReqMemSize)) {
                    CurReqMemSize = 0U;
                }
            }

            if (CurReqMemSize <= SVC_CalibMgr.CalcMemCtrl.BufSize) {
                pBuf[0]     = SVC_CalibMgr.CalcMemCtrl.pBuf;
                pBufSize[0] = SVC_CalibMgr.CalcMemCtrl.BufSize;
            } else {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to get calib calculate mem - configuration calc memory is not enough to service 0x%X!"
                    PRN_CAL_MGR_ARG_UINT32 CurReqMemSize PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_NG
            }
        } else {
            if ((SVC_CalibMgr.CalcMemCtrl.State & SVC_CALIB_CTRL_CALC_MEM_LOCK) > 0U) {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to get calib calculate mem - buffer has been locked!" PRN_CAL_MGR_NG
            } else {
                SVC_CALIB_CALC_MEM_INFO_s *pCalcMemInfo = &(SVC_CalibMgr.CalcMemCtrl.LockMemInfo);

                AmbaSvcWrap_MisraMemset(pCalcMemInfo, 0, sizeof(SVC_CALIB_CALC_MEM_INFO_s));
                pCalcMemInfo->ReqMemNum = CurReqNum;
                if (pCalcMemInfo->ReqMemNum > SVC_CALIB_CALC_MEM_MAX_NUM) {
                    pCalcMemInfo->ReqMemNum = SVC_CALIB_CALC_MEM_MAX_NUM;
                }

                if (SVC_CalibMgr.CalcCbFunc != NULL) {
                    if (SVC_OK == (SVC_CalibMgr.CalcCbFunc)(CalID, SVC_CALIB_CALC_MEM_LOCK, pCalcMemInfo)) {
                        SVC_CalibMgr.CalcMemCtrl.State |= SVC_CALIB_CTRL_CALC_MEM_LOCK;

                        for (ReqIdx = 0U; ReqIdx < CurReqNum; ReqIdx ++) {
                            pBuf[ReqIdx]     = pCalcMemInfo->pReqMem[ReqIdx];
                            pBufSize[ReqIdx] = pCalcMemInfo->ReqMemSize[ReqIdx];
                        }
                    }
                }
            }
        }

        if (RetVal == SVC_OK) {
            /* Lock memory by the calib id */
            SVC_CalibMgr.CalcMemCtrl.State |= SvcCalib_BitGet( CalID << 1U );
        }
    }

    return RetVal;
}

/**
 * free the calculation memory state
 *
 * @param [in] CalID calib identification
 * @return ErrCode SVC_OK(0)|SVC_NG(1)
 */
UINT32 SvcCalib_ItemCalcMemFree(UINT32 CalID)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibMgrFlag & SVC_CALIB_MGR_FLG_MEM_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to free calib calculate mem - initial calib control first!" PRN_CAL_MGR_NG
    } else if ((SVC_CalibMgr.Status & SVC_CALIB_CTRL_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to free calib calculate mem - initial calib control first!!!" PRN_CAL_MGR_NG
    } else if (CalID >= SVC_CALIB_MAX_OBJ_NUM) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to free calib calculate mem - invalid CalibID(%d)"
            PRN_CAL_MGR_ARG_UINT32 CalID PRN_CAL_MGR_ARG_POST
        PRN_CAL_MGR_NG
    } else {

        if ((SVC_CalibMgr.CalcMemCtrl.State & SVC_CALIB_CTRL_CALC_MEM_LOCK) > 0U) {

            if (SVC_CalibMgr.CalcCbFunc != NULL) {
                if (0U != (SVC_CalibMgr.CalcCbFunc)(CalID, SVC_CALIB_CALC_MEM_UNLOCK, &(SVC_CalibMgr.CalcMemCtrl.LockMemInfo))) {
                    RetVal = SVC_NG;

                    PRN_CAL_MGR_LOG "Fail to free calib calculate mem - unlock memory fail! CalibID(%d)"
                        PRN_CAL_MGR_ARG_UINT32 CalID PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_NG
                }
            }

            SVC_CalibMgr.CalcMemCtrl.State &= ~SVC_CALIB_CTRL_CALC_MEM_LOCK;
        }

        /* Un-lock memory by the calib id */
        SVC_CalibMgr.CalcMemCtrl.State &= ~SvcCalib_BitGet( CalID << 1U );
    }

    return RetVal;
}

/**
 * get calib object
 *
 * @param [in] CalID calib identification
 * @param [out] pCalObj calib object
 * @return ErrCode SVC_OK(0)|SVC_NG(1)
 */
UINT32 SvcCalib_ObjGet(UINT32 CalID, SVC_CALIB_OBJ_s *pCalObj)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_CalibMgrFlag & SVC_CALIB_MGR_FLG_MEM_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to get calib object - initial calib control first!" PRN_CAL_MGR_NG
    } else if ((SVC_CalibMgr.Status & SVC_CALIB_CTRL_INIT) == 0U) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to get calib object - initial calib control first!!!" PRN_CAL_MGR_NG
    } else if (pCalObj == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to get calib object - output object should not null!" PRN_CAL_MGR_NG
    } else {
        const SVC_CALIB_OBJ_CTRL_s *pCalObjCtrl = SvcCalibMgr_ObjCtrlGet(CalID);
        if (pCalObjCtrl == NULL) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to get calib object - get calib object control fail!" PRN_CAL_MGR_NG
        } else if (pCalObjCtrl->CalObj.ID != CalID) {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to get calib object - invalid calib object control fail!" PRN_CAL_MGR_NG
        } else {
            AmbaSvcWrap_MisraMemset(pCalObj, 0, sizeof(SVC_CALIB_OBJ_s));
            AmbaSvcWrap_MisraMemcpy(pCalObj, &(pCalObjCtrl->CalObj), sizeof(SVC_CALIB_OBJ_s));
        }
    }

    return RetVal;
}

static void SvcCalib_Lz77Search(const UINT8 *pSearchBuf, UINT32 SearchSize,
                                const UINT8 *pLookHeadBuf, UINT32 LookHeadSize,
                                UINT32 *pOffset, UINT32 *pLength)
{
    if ((pSearchBuf != NULL) &&
        (pLookHeadBuf != NULL) &&
        (pOffset != NULL) &&
        (pLength != NULL)) {
        UINT32 MemCompLeng = (SearchSize >= LookHeadSize)?LookHeadSize:SearchSize;
        INT8   GetResult = -1;
        UINT32 SearchIdx = SearchSize, CompIdx;

        *pOffset = 0U;
        *pLength = 0U;

        if (MemCompLeng > 0U) {

            while (MemCompLeng > 0U) {

                for (SearchIdx = 0U; (SearchIdx + MemCompLeng) <= SearchSize; SearchIdx ++) {

                    GetResult = 1;

                    for (CompIdx = 0U; CompIdx < MemCompLeng; CompIdx ++) {
                        if (pSearchBuf[SearchIdx + CompIdx] != pLookHeadBuf[CompIdx]) {
                            GetResult = 0;
                            break;
                        }
                    }

                    if (GetResult > 0) {
                        break;
                    }
                }

                if (GetResult > 0) {
                    break;
                } else {
                    MemCompLeng -= 1U;
                }
            }
        }

        if (GetResult > 0) {
            *pLength = MemCompLeng;
            *pOffset = SearchSize - SearchIdx;
        }
    }
}


static UINT32 SvcCalib_Lz77Compress(UINT32 MaxSearchSize, const void *pSrc, UINT32 SrcSize, void *pDst, UINT32 DstSize, UINT32 *pCmprSize)
{
    UINT32 RetVal = SVC_OK;
    UINT8 *pSearchBuf   = NULL;
    UINT8 *pLookHeadBuf = NULL;
    UINT8 *pDstBuf      = NULL;

    AmbaMisra_TypeCast(&pLookHeadBuf, &pSrc);
    AmbaMisra_TypeCast(&pDstBuf, &pDst);
    pSearchBuf = pLookHeadBuf;

    if (pLookHeadBuf == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to proc lz77 compress - invalid source data!" PRN_CAL_MGR_NG
    } else if (pDstBuf == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to proc lz77 compress - invalid destination buffer!" PRN_CAL_MGR_NG
        AmbaMisra_TouchUnused(pDst);
    } else if (pCmprSize == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to proc lz77 compress - invalid output compress size!" PRN_CAL_MGR_NG
    } else {
        UINT32 CurSearchNum = 0U;
        UINT32 CurLookHeadNum = 0U;
        UINT32 CurProcSize = 0U;
        UINT32 Offset, Length;
        UINT32 CmprSize = 0U, UpdSize;

        while (CurProcSize < SrcSize) {

            Offset = 0U;
            Length = 0U;

            if (CurSearchNum > 0U) {

                CurLookHeadNum = SrcSize - CurProcSize;
                if (CurLookHeadNum > MaxSearchSize) {
                    CurLookHeadNum = MaxSearchSize;
                }

                SvcCalib_Lz77Search(pSearchBuf, CurSearchNum,
                                    pLookHeadBuf, CurLookHeadNum,
                                    &Offset, &Length);
            }

            if (Offset == 0U) {

                pDstBuf[CmprSize] = (UINT8)Offset;       CmprSize ++;
                pDstBuf[CmprSize] = (UINT8)Length;       CmprSize ++;
                pDstBuf[CmprSize] = pLookHeadBuf[0];     CmprSize ++;

                UpdSize = 1U;
            } else {

                pDstBuf[CmprSize] = (UINT8)Offset; CmprSize ++;
                pDstBuf[CmprSize] = (UINT8)Length; CmprSize ++;

                UpdSize = Length;
            }

            if (CurSearchNum < MaxSearchSize) {
                CurSearchNum += UpdSize;
                if (CurSearchNum > MaxSearchSize) {
                    pSearchBuf = &(pSearchBuf[CurSearchNum - MaxSearchSize]);
                    CurSearchNum = MaxSearchSize;
                }
            } else {
                pSearchBuf = &(pSearchBuf[UpdSize]);
            }

            pLookHeadBuf = &(pLookHeadBuf[UpdSize]);

            CurProcSize += UpdSize;

            if ((CmprSize + 1U) > DstSize) {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to proc lz77 compress - destination is too small to service it! 0x%X + 1U > 0x%X"
                    PRN_CAL_MGR_ARG_UINT32 CmprSize PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 DstSize  PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_NG
                break;
            }
        }

        if (RetVal == SVC_OK) {
            *pCmprSize = CmprSize;
        }
    }

    return RetVal;
}


static UINT32 SvcCalib_Lz77Decompress(const void *pSrc, UINT32 SrcSize, void *pDst, UINT32 DstSize, UINT32 *pDeCmprSize)
{
    UINT32 RetVal = SVC_OK;
    UINT8 *pSrcBuf = NULL;
    UINT8 *pDstBuf = NULL;

    AmbaMisra_TypeCast(&(pSrcBuf), &pSrc);
    AmbaMisra_TypeCast(&(pDstBuf), &pDst);

    if (pSrcBuf == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to proc lz77 decompress - invalid source data!" PRN_CAL_MGR_NG
        AmbaMisra_TouchUnused(pSrcBuf);
    } else if (pDstBuf == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to proc lz77 decompress - invalid destination buffer!" PRN_CAL_MGR_NG
        AmbaMisra_TouchUnused(pDst);
    } else if (pDeCmprSize == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to proc lz77 decompress - invalid output compress size!" PRN_CAL_MGR_NG
    } else {
        UINT32 CurProcSize = 0U;
        UINT32 DeCmprSize = 0U, UpdSize;

        while (CurProcSize < SrcSize) {
            if (pSrcBuf[CurProcSize] == 0U) {

                UpdSize = 1U;

                if ((DeCmprSize + UpdSize) <= DstSize) {
                    pDstBuf[DeCmprSize] = pSrcBuf[CurProcSize + 2U];
                    DeCmprSize  += UpdSize;
                    CurProcSize += 3U;
                } else {
                    RetVal = SVC_NG;
                    PRN_CAL_MGR_LOG "Fail to proc lz77 decompress - destination is too small to service it! 0x%X + 0x%X > 0x%X"
                        PRN_CAL_MGR_ARG_UINT32 DeCmprSize PRN_CAL_MGR_ARG_POST
                        PRN_CAL_MGR_ARG_UINT32 UpdSize    PRN_CAL_MGR_ARG_POST
                        PRN_CAL_MGR_ARG_UINT32 DstSize    PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_NG
                }
            } else {
                if ((pSrcBuf[CurProcSize] > DeCmprSize) || (pSrcBuf[CurProcSize + 1U] > DeCmprSize)) {
                    RetVal = SVC_NG;
                } else {

                    UpdSize = pSrcBuf[CurProcSize + 1U];

                    if ((DeCmprSize + UpdSize) <= DstSize) {
                        AmbaSvcWrap_MisraMemcpy(&(pDstBuf[DeCmprSize]),
                                               &(pDstBuf[DeCmprSize - pSrcBuf[CurProcSize]]),
                                               pSrcBuf[CurProcSize + 1U]);
                        DeCmprSize  += UpdSize;
                        CurProcSize += 2U;
                    } else {
                        RetVal = SVC_NG;
                        PRN_CAL_MGR_LOG "Fail to proc lz77 decompress - destination is too small to service it! 0x%X + 0x%X > 0x%X"
                            PRN_CAL_MGR_ARG_UINT32 DeCmprSize PRN_CAL_MGR_ARG_POST
                            PRN_CAL_MGR_ARG_UINT32 UpdSize    PRN_CAL_MGR_ARG_POST
                            PRN_CAL_MGR_ARG_UINT32 DstSize    PRN_CAL_MGR_ARG_POST
                        PRN_CAL_MGR_NG
                    }
                }
            }

            if (RetVal != SVC_OK) {
                break;
            }
        }

        if (RetVal == SVC_OK) {
            *pDeCmprSize = DeCmprSize;
        }
    }

    return RetVal;
}

/**
 * calib data compress
 *
 * @param [in] pCtrl compress info
 * @return ErrCode SVC_OK(0)|SVC_NG(1)
 */
UINT32 SvcCalib_DataCompress(SVC_CALIB_COMPRESS_s *pCtrl)
{
    UINT32 RetVal = SVC_OK;

    if (pCtrl == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to proc compression - invalid compression ctrl!" PRN_CAL_MGR_NG
        AmbaMisra_TouchUnused(pCtrl);
    } else {
        if (pCtrl->CmprType == SVC_CALIB_CMPR_LZ77) {
            RetVal = SvcCalib_Lz77Compress(pCtrl->Lz77.SearchWin, pCtrl->pSource, pCtrl->SourceSize, pCtrl->pDestination, pCtrl->DestinationSize, pCtrl->pCmprSize);
        } else if (pCtrl->CmprType == SVC_CALIB_CMPR_NONE) {
            if (pCtrl->DestinationSize < pCtrl->SourceSize) {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to proc compression type(%d) - destination size(0x%X) < source size(0x%X)!"
                    PRN_CAL_MGR_ARG_UINT32 pCtrl->CmprType        PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 pCtrl->DestinationSize PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 pCtrl->SourceSize      PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_NG
            } else if (pCtrl->pSource == NULL) {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to proc compression type(%d) - invalid source data!"
                    PRN_CAL_MGR_ARG_UINT32 pCtrl->CmprType PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_NG
            } else if (pCtrl->pDestination == NULL) {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to proc compression type(%d) - invalid destination data!"
                    PRN_CAL_MGR_ARG_UINT32 pCtrl->CmprType PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_NG
            } else if (pCtrl->pCmprSize == NULL) {
                PRN_CAL_MGR_LOG "Fail to proc compression type(%d) - invalid output compression size!"
                    PRN_CAL_MGR_ARG_UINT32 pCtrl->CmprType PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_NG
                RetVal = SVC_NG;
            } else {
                AmbaSvcWrap_MisraMemcpy(pCtrl->pDestination, pCtrl->pSource, pCtrl->SourceSize);
                *(pCtrl->pCmprSize) = pCtrl->SourceSize;
            }
        } else {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to proc compression type(%d) - invalid type!"
                PRN_CAL_MGR_ARG_UINT32 pCtrl->CmprType PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        }

        if (RetVal == SVC_OK) {
            PRN_CAL_MGR_LOG "Success to proc compression type(%d)"
                PRN_CAL_MGR_ARG_UINT32 pCtrl->CmprType PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_OK
        }
    }

    return RetVal;

}

/**
 * calib data de-compress
 *
 * @param [in] pCtrl de-compress info
 * @return ErrCode SVC_OK(0)|SVC_NG(1)
 */
UINT32 SvcCalib_DataDecompress(SVC_CALIB_DECOMPRESS_s *pCtrl)
{
    UINT32 RetVal = SVC_OK;

    if (pCtrl == NULL) {
        RetVal = SVC_NG;
        PRN_CAL_MGR_LOG "Fail to proc decompression - invalid decompression ctrl!" PRN_CAL_MGR_NG
        AmbaMisra_TouchUnused(pCtrl);
    } else {
        if (pCtrl->CmprType == SVC_CALIB_CMPR_LZ77) {
            RetVal = SvcCalib_Lz77Decompress(pCtrl->pSource, pCtrl->SourceSize, pCtrl->pDestination, pCtrl->DestinationSize, pCtrl->pDeCmprSize);
        } else if (pCtrl->CmprType == SVC_CALIB_CMPR_NONE) {
            if (pCtrl->DestinationSize < pCtrl->SourceSize) {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to proc decompression type(%d) - destination size(0x%X) < source size(0x%X)!"
                    PRN_CAL_MGR_ARG_UINT32 pCtrl->CmprType        PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 pCtrl->DestinationSize PRN_CAL_MGR_ARG_POST
                    PRN_CAL_MGR_ARG_UINT32 pCtrl->SourceSize      PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_NG
            } else if (pCtrl->pSource == NULL) {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to proc decompression type(%d) - invalid source data!"
                    PRN_CAL_MGR_ARG_UINT32 pCtrl->CmprType PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_NG
            } else if (pCtrl->pDestination == NULL) {
                RetVal = SVC_NG;
                PRN_CAL_MGR_LOG "Fail to proc decompression type(%d) - invalid destination data!"
                    PRN_CAL_MGR_ARG_UINT32 pCtrl->CmprType PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_NG
            } else if (pCtrl->pDeCmprSize == NULL) {
                PRN_CAL_MGR_LOG "Fail to proc decompression type(%d) - invalid output decompression size!"
                    PRN_CAL_MGR_ARG_UINT32 pCtrl->CmprType PRN_CAL_MGR_ARG_POST
                PRN_CAL_MGR_NG
                RetVal = SVC_NG;
            } else {
                AmbaSvcWrap_MisraMemcpy(pCtrl->pDestination, pCtrl->pSource, pCtrl->SourceSize);
                *(pCtrl->pDeCmprSize) = pCtrl->SourceSize;
            }
        } else {
            RetVal = SVC_NG;
            PRN_CAL_MGR_LOG "Fail to proc decompression type(%d) - invalid type!"
                PRN_CAL_MGR_ARG_UINT32 pCtrl->CmprType PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_NG
        }

        if (RetVal == SVC_OK) {
            PRN_CAL_MGR_LOG "Success to proc decompression type(%d)"
                PRN_CAL_MGR_ARG_UINT32 pCtrl->CmprType PRN_CAL_MGR_ARG_POST
            PRN_CAL_MGR_OK
        }
    }

    return RetVal;
}
