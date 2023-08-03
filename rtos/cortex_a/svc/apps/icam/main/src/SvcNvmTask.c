/**
 *  @file SvcNvmTask.c
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
 *  @details svc nvm task
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaKAL.h"
#include "AmbaNVM_Partition.h"
#include "AmbaPrint.h"
#include "AmbaWrap.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"

#ifdef CONFIG_ICAM_IMGPROC_USED
#include "AmbaIQParamHandlerSample.h"
#endif

#include "SvcWrap.h"
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcNvm.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcTask.h"
#include "SvcSysStat.h"

#include "SvcBufMap.h"
#include "SvcTaskList.h"
#include "SvcAppStat.h"
#include "SvcNvmTask.h"

#define SVC_NVM_TSK_FLAG_INIT          (0x1U)
#define SVC_NVM_TSK_FLAG_SHELL_INIT    (0x1000U)
#define SVC_NVM_TSK_FLAG_OK_MSG        (0x10000U)
#define SVC_NVM_TSK_FLAG_NG_MSG        (0x20000U)
#define SVC_NVM_TSK_FLAG_API_MSG       (0x40000U)
#define SVC_NVM_TSK_FLAG_DBG_MSG       (0x80000U)

#define SVC_NVM_TSK_STACK_SIZE     (0x4000U)
#define SVC_NVM_TSK_MAX_LIST_DEPTH  (256U)
#define SVC_NVM_TSK_MAX_WORK_SIZE  (0x100000U)
#define SVC_NVM_TSK_CRC_FILE_LEN   (64U)
#define SVC_NVM_TSK_CRC_INFO_MAGIC (0x20191218U)
#define SVC_NVM_TSK_MUTEX_TIMEOUT  (3000U)
#define SVC_NVM_TSK_DELAY_TIME     (10)
// #define SVC_NVM_TSK_ADD_TIMEOUT    (100 * SVC_NVM_TSK_DELAY_TIME)

typedef struct {
    UINT32 NvmID;
    UINT32 RegionID;
    char   FileName[SVC_NVM_TSK_CRC_FILE_LEN];
    UINT32 InfoType;
#define SVC_NVM_TSK_TYPE_NORMAL_INFO     (0U)
#define SVC_NVM_TSK_TYPE_PARTIAL_INFO    (1U)
    UINT32 FileCRC32;
    UINT32 HwCRC32;
    UINT32 SwCRC32;
    UINT32 MagicCode;
} SVC_NVM_TSK_CRC_INFO_s;

typedef struct {
    char                    Name[32];
#define SVC_NVM_TSK_CTRL_START    (0x1U)
#define SVC_NVM_TSK_CTRL_DATA_RDY (0x2U)
    AMBA_KAL_EVENT_FLAG_t   CtrlFlag;
    AMBA_KAL_MUTEX_t        Mutex;
    UINT8                   TaskStack[SVC_NVM_TSK_STACK_SIZE];
    SVC_TASK_CTRL_s         TaskCtrl;
    SVC_SYS_STAT_ID         SysStateCtrlID;
    UINT32                  CheckIdx;
    UINT32                  ListIdx;
    UINT32                  NumOfList;
    SVC_NVM_TSK_CRC_INFO_s  CrcCheckList[SVC_NVM_TSK_MAX_LIST_DEPTH];
    UINT8                   *WorkBuf;
} SVC_NVM_TSK_CTRL_s;

static void SvcNvmTask_ReadRomPostProc(UINT32 NvmID, UINT32 RegionID, UINT32 FileIdx, UINT32 ReadSize, UINT32 CalcCRC32, const char *pFileName);
static void SvcNvmTask_SysStateBootDone(UINT32 StatIdx, void *pInfo);
static void SvcNvmTask_CrcCheck(SVC_NVM_TSK_CRC_INFO_s *pCrcInfo);
static void* SvcNvmTask_TaskEntry(void* EntryArg);

typedef UINT32 (*SVC_NVM_TSK_SHELL_FUNC_f)(UINT32 ArgCount, char * const *pArgVector);
typedef void (*SVC_NVM_TSK_USAGE_f)(UINT32 CtrlFlag);
typedef struct {
    char                     CmdName[32];
    SVC_NVM_TSK_SHELL_FUNC_f pFunc;
    SVC_NVM_TSK_USAGE_f      pUsage;
} SVC_NVM_TSK_SHELL_FUNC_s;

static UINT32 SvcNvmTask_ShellDbgMsg(UINT32 ArgCount, char * const *pArgVector);
static void   SvcNvmTask_ShellDbgMsgU(UINT32 CtrlFlag);
static UINT32 SvcNvmTask_ShellInfo(UINT32 ArgCount, char * const *pArgVector);
static void   SvcNvmTask_ShellInfoU(UINT32 CtrlFlag);
static void   SvcNvmTask_ShellEntryInit(void);
static void   SvcNvmTask_ShellUsage(UINT32 CtrlFlag);
       void   SvcNvmTask_ShellEntry(UINT32 ArgCount, char * const *pArgVector);

#define SVC_NVM_TSK_SHELL_CMD_NUM  (2U)
static SVC_NVM_TSK_SHELL_FUNC_s SvcNvmTaskShellFunc[SVC_NVM_TSK_SHELL_CMD_NUM] GNU_SECTION_NOZEROINIT;
static SVC_NVM_TSK_CTRL_s       SvcNvmTaskCtrl GNU_SECTION_NOZEROINIT;
static UINT32                   SvcNvmTaskFlag = (SVC_NVM_TSK_FLAG_OK_MSG | SVC_NVM_TSK_FLAG_NG_MSG | SVC_NVM_TSK_FLAG_API_MSG); //| SVC_NVM_TSK_FLAG_DBG_MSG);
static SVC_WRAP_PRINT_s SvcNvmTaskPrint GNU_SECTION_NOZEROINIT;

#define SVC_LOG_NVM_TSK "NVM_TSK"
#define SVC_LOG_NVM_TSK_HL_TITLE_0 ("\033""[38;2;255;125;38m")
#define SVC_LOG_NVM_TSK_HL_TITLE_1 ("\033""[38;2;255;255;128m")
#define SVC_LOG_NVM_TSK_HL_TITLE_2 ("\033""[38;2;255;255;128m""\033""[4m")
#define SVC_LOG_NVM_TSK_HL_NUM     ("\033""[38;2;255;0;128m")
#define SVC_LOG_NVM_TSK_HL_STR     ("\033""[38;2;156;248;40m")
#define SVC_LOG_NVM_TSK_HL_DEF     ("\033""[39m")
#define SVC_LOG_NVM_TSK_HL_END     ("\033""[0m")

#define PRN_NVM_TSK_LOG   { AmbaSvcWrap_MisraMemset(&(SvcNvmTaskPrint), 0, sizeof(SvcNvmTaskPrint)); SvcNvmTaskPrint.Argc --; SvcNvmTaskPrint.pStrFmt =
#define PRN_NVM_TSK_ARG_UINT32 ; { UINT32      ArgVal =
#define PRN_NVM_TSK_ARG_CSTR   ; { const char *ArgVal =
#define PRN_NVM_TSK_ARG_POST   ; SvcNvmTaskPrint.Argc ++; AmbaMisra_TypeCast(&(SvcNvmTaskPrint.Argv[SvcNvmTaskPrint.Argc].Uint64), &(ArgVal)); }
#define PRN_NVM_TSK_OK         ; SvcNvmTaskPrint.Argc ++; SvcNvmTask_PrintLog(SVC_NVM_TSK_FLAG_OK_MSG , &(SvcNvmTaskPrint)); }
#define PRN_NVM_TSK_NG         ; SvcNvmTaskPrint.Argc ++; SvcNvmTask_PrintLog(SVC_NVM_TSK_FLAG_NG_MSG , &(SvcNvmTaskPrint)); }
#define PRN_NVM_TSK_API        ; SvcNvmTaskPrint.Argc ++; SvcNvmTask_PrintLog(SVC_NVM_TSK_FLAG_API_MSG, &(SvcNvmTaskPrint)); }
#define PRN_NVM_TSK_DBG        ; SvcNvmTaskPrint.Argc ++; SvcNvmTask_PrintLog(SVC_NVM_TSK_FLAG_DBG_MSG, &(SvcNvmTaskPrint)); }

static void SvcNvmTask_PrintLog(UINT32 LogLevel, SVC_WRAP_PRINT_s *pPrint)
{
    if (pPrint != NULL) {
        if ((SvcNvmTaskFlag & LogLevel) > 0U) {

            switch (LogLevel) {
            case SVC_NVM_TSK_FLAG_OK_MSG :
                pPrint->pProc = SvcLog_OK;
                break;
            case SVC_NVM_TSK_FLAG_NG_MSG :
                pPrint->pProc = SvcLog_NG;
                break;
            default :
                pPrint->pProc = SvcLog_DBG;
                break;
            }

            SvcWrap_Print(SVC_LOG_NVM_TSK, pPrint);
        }
    }
}

static void SvcNvmTask_ReadRomPostProc(UINT32 NvmID, UINT32 RegionID, UINT32 FileIdx, UINT32 ReadSize, UINT32 CalcCRC32, const char *pFileName)
{
    SVC_NVM_TSK_CTRL_s *pCurCtrl = (&SvcNvmTaskCtrl);
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SvcNvmTaskFlag & SVC_NVM_TSK_FLAG_INIT) == 0U) {
        PRN_NVM_TSK_LOG "Fail to proc %s rom file - init NVM task first!"
            PRN_NVM_TSK_ARG_CSTR   pFileName PRN_NVM_TSK_ARG_POST
        PRN_NVM_TSK_NG
    } else {
        AMBA_NVM_ROM_FILE_INFO_s RomFileInfo;

        AmbaSvcWrap_MisraMemset(&RomFileInfo, 0, sizeof(RomFileInfo));
        PRetVal = AmbaNVM_GetRomFileInfo(NvmID, RegionID, FileIdx, &RomFileInfo);
        if (PRetVal != 0U) {
            PRN_NVM_TSK_LOG "Fail to proc %s rom file - get rom file info fail! ErrCode(0x%08X) NvmID(%d) RegionID(%d) FileIdx(%d)"
                PRN_NVM_TSK_ARG_CSTR   pFileName PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_ARG_UINT32 PRetVal   PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_ARG_UINT32 NvmID     PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_ARG_UINT32 RegionID  PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_ARG_UINT32 FileIdx   PRN_NVM_TSK_ARG_POST
            PRN_NVM_TSK_NG
        } else if (ReadSize > RomFileInfo.ByteSize) {
            PRN_NVM_TSK_LOG "Fail to proc %s rom file - read size(0x%08X) > file size(0x%08X)!"
                PRN_NVM_TSK_ARG_CSTR   pFileName            PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_ARG_UINT32 ReadSize             PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_ARG_UINT32 RomFileInfo.ByteSize PRN_NVM_TSK_ARG_POST
            PRN_NVM_TSK_NG
        } else {
            SVC_NVM_TSK_CRC_INFO_s  CurInfo;
            INT32                   WaitTimeOut = 0;
            UINT32                  CurIdx;
            SVC_NVM_TSK_CRC_INFO_s *pCrcInfo;

            AmbaMisra_TouchUnused(&WaitTimeOut);

#ifdef SVC_NVM_TSK_ADD_TIMEOUT
            WaitTimeOut = SVC_NVM_TSK_ADD_TIMEOUT;
#endif

            // Configure CRC info
            AmbaSvcWrap_MisraMemset(&CurInfo, 0, sizeof(CurInfo));
            CurInfo.NvmID     = NvmID;
            CurInfo.RegionID  = RegionID;
            CurInfo.HwCRC32   = CalcCRC32;
            CurInfo.FileCRC32 = RomFileInfo.FileCRC32;
            if (ReadSize < RomFileInfo.ByteSize) {
                CurInfo.InfoType = SVC_NVM_TSK_TYPE_PARTIAL_INFO;
            } else {
                CurInfo.InfoType = SVC_NVM_TSK_TYPE_NORMAL_INFO;
            }
            AmbaUtility_StringCopy(CurInfo.FileName, sizeof(CurInfo.FileName), pFileName);

            // Add the CRC info to check list
            do {

                RetVal = AmbaKAL_MutexTake(&(pCurCtrl->Mutex), SVC_NVM_TSK_MUTEX_TIMEOUT);
                if (RetVal == 0U) {

                    CurIdx = pCurCtrl->ListIdx + 1U;

                    if (CurIdx >= SVC_NVM_TSK_MAX_LIST_DEPTH) {
                        CurIdx = 0U;
                    }

                    // If check list is full, ...
                    if (pCurCtrl->NumOfList >= SVC_NVM_TSK_MAX_LIST_DEPTH) {
                        RetVal = SVC_NG;
                        PRN_NVM_TSK_LOG "Fail to proc %s rom file - local crc check list is full! %03d/%03d/%03d%s"
                            PRN_NVM_TSK_ARG_CSTR   pFileName                                    PRN_NVM_TSK_ARG_POST
                            PRN_NVM_TSK_ARG_UINT32 pCurCtrl->ListIdx                            PRN_NVM_TSK_ARG_POST
                            PRN_NVM_TSK_ARG_UINT32 pCurCtrl->CheckIdx                           PRN_NVM_TSK_ARG_POST
                            PRN_NVM_TSK_ARG_UINT32 pCurCtrl->NumOfList                          PRN_NVM_TSK_ARG_POST
                            PRN_NVM_TSK_ARG_CSTR   ((WaitTimeOut > 0)?", Retry after 10 ms":"") PRN_NVM_TSK_ARG_POST
                        PRN_NVM_TSK_DBG
                    } else {

                        pCrcInfo = &(pCurCtrl->CrcCheckList[CurIdx]);

                        // If magic code is exist
                        if (pCrcInfo->MagicCode == SVC_NVM_TSK_CRC_INFO_MAGIC) {
                            RetVal = SVC_NG;
                            PRN_NVM_TSK_LOG "Fail to proc %s rom file - index(%d) crc info should be empty! 0x%08X%s"
                                PRN_NVM_TSK_ARG_CSTR   pFileName                                    PRN_NVM_TSK_ARG_POST
                                PRN_NVM_TSK_ARG_UINT32 CurIdx                                       PRN_NVM_TSK_ARG_POST
                                PRN_NVM_TSK_ARG_UINT32 pCrcInfo->MagicCode                          PRN_NVM_TSK_ARG_POST
                                PRN_NVM_TSK_ARG_CSTR   ((WaitTimeOut > 0)?", Retry after 10 ms":"") PRN_NVM_TSK_ARG_POST
                            PRN_NVM_TSK_DBG
                        } else {

                            // Add current crc info to list
                            AmbaSvcWrap_MisraMemcpy(pCrcInfo, &CurInfo, sizeof(SVC_NVM_TSK_CRC_INFO_s));
                            pCrcInfo->MagicCode = SVC_NVM_TSK_CRC_INFO_MAGIC;

                            pCurCtrl->ListIdx = CurIdx;
                            pCurCtrl->NumOfList += 1U;

                            PRN_NVM_TSK_LOG "Add to check list[%s%03d%s] : %s%d%s / %s0x%08X%s / %s0x%08X%s / %s%s%s"
                                PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_NUM PRN_NVM_TSK_ARG_POST
                                PRN_NVM_TSK_ARG_UINT32 pCurCtrl->ListIdx      PRN_NVM_TSK_ARG_POST
                                PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_END PRN_NVM_TSK_ARG_POST
                                PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_NUM PRN_NVM_TSK_ARG_POST
                                PRN_NVM_TSK_ARG_UINT32 pCrcInfo->InfoType     PRN_NVM_TSK_ARG_POST
                                PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_END PRN_NVM_TSK_ARG_POST
                                PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_NUM PRN_NVM_TSK_ARG_POST
                                PRN_NVM_TSK_ARG_UINT32 pCrcInfo->FileCRC32    PRN_NVM_TSK_ARG_POST
                                PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_END PRN_NVM_TSK_ARG_POST
                                PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_NUM PRN_NVM_TSK_ARG_POST
                                PRN_NVM_TSK_ARG_UINT32 pCrcInfo->HwCRC32      PRN_NVM_TSK_ARG_POST
                                PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_END PRN_NVM_TSK_ARG_POST
                                PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_STR PRN_NVM_TSK_ARG_POST
                                PRN_NVM_TSK_ARG_CSTR   pCrcInfo->FileName     PRN_NVM_TSK_ARG_POST
                                PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_END PRN_NVM_TSK_ARG_POST
                            PRN_NVM_TSK_DBG

                            (void) AmbaKAL_EventFlagSet(&(pCurCtrl->CtrlFlag), SVC_NVM_TSK_CTRL_DATA_RDY);

                            RetVal = SVC_OK;
                        }
                    }

                    (void) AmbaKAL_MutexGive(&(pCurCtrl->Mutex));
                }

                // If waiting mechanism is on and add-process fail
                if ((RetVal != 0U) && (WaitTimeOut > 0)) {
                    (void) AmbaKAL_TaskSleep((UINT32)SVC_NVM_TSK_DELAY_TIME);
                    WaitTimeOut -= SVC_NVM_TSK_DELAY_TIME;
                }

            } while ((RetVal != 0U) && (WaitTimeOut > 0));

            if (RetVal != 0U) {
                PRN_NVM_TSK_LOG "Fail to proc %s rom file - add list fail!"
                    PRN_NVM_TSK_ARG_CSTR   pFileName PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_NG
            }
        }
    }
}

static void SvcNvmTask_SysStateBootDone(UINT32 StatIdx, void *pInfo)
{
    SVC_APP_STAT_DSP_BOOT_s *pBootStatus = NULL;

    if (StatIdx == SVC_APP_STAT_DSP_BOOT) {
        AmbaMisra_TypeCast(&pBootStatus, &pInfo);
        AmbaMisra_TouchUnused(pBootStatus);
        AmbaMisra_TouchUnused(pInfo);

        if (pBootStatus->Status == SVC_APP_STAT_DSP_BOOT_DONE) {
            (void) AmbaKAL_EventFlagSet(&(SvcNvmTaskCtrl.CtrlFlag), SVC_NVM_TSK_CTRL_START);
        }
    }
}

static void SvcNvmTask_CrcCheck(SVC_NVM_TSK_CRC_INFO_s *pCrcInfo)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    const SVC_NVM_TSK_CTRL_s *pCurCtrl = &(SvcNvmTaskCtrl);

    if (pCrcInfo == NULL) {
        PRN_NVM_TSK_LOG "Fail to check crc - invalid crc info!" PRN_NVM_TSK_NG
    } else {
        UINT32 FileSize = 0U;

        // Get the rom file size
        PRetVal = AmbaNVM_GetRomFileSize(pCrcInfo->NvmID, pCrcInfo->RegionID, pCrcInfo->FileName, &FileSize);
        if (PRetVal != 0U) {
            RetVal = 1U;
            PRN_NVM_TSK_LOG "Fail to check crc - get rom file size fail!" PRN_NVM_TSK_NG
        } else if (FileSize == 0U) {
            RetVal = 2U;
            PRN_NVM_TSK_LOG "Fail to check crc - rom file size should not zero!" PRN_NVM_TSK_NG
        } else {
            UINT32 BufSize = SVC_NVM_TSK_MAX_WORK_SIZE;
            UINT8 *pBuf    = pCurCtrl->WorkBuf;

            // Calculate the CRC by Software
            if (FileSize <= BufSize) {

                AmbaSvcWrap_MisraMemset(pBuf, 0, BufSize);

                PRetVal = AmbaNVM_ReadRomFile(pCrcInfo->NvmID, pCrcInfo->RegionID,
                                              pCrcInfo->FileName, 0U, FileSize, pBuf, 1000U);
                if (PRetVal != 0U) {
                    RetVal = 1U;
                    PRN_NVM_TSK_LOG "Fail to check crc - read the data from rom fail! ErrCode(0x%08X), %s"
                    PRN_NVM_TSK_ARG_UINT32 PRetVal            PRN_NVM_TSK_ARG_POST
                    PRN_NVM_TSK_ARG_CSTR   pCrcInfo->FileName PRN_NVM_TSK_ARG_POST
                    PRN_NVM_TSK_NG
                } else {
                    pCrcInfo->SwCRC32 = AmbaUtility_Crc32Sw(pBuf, FileSize);

                    if (pCrcInfo->InfoType == SVC_NVM_TSK_TYPE_PARTIAL_INFO) {
                        pCrcInfo->HwCRC32 = AmbaUtility_Crc32(pBuf, FileSize);
                        PRN_NVM_TSK_LOG "Generate Hardware CRC off-line - %s"
                            PRN_NVM_TSK_ARG_CSTR   pCrcInfo->FileName PRN_NVM_TSK_ARG_POST
                        PRN_NVM_TSK_DBG
                    }
                }
            } else {
                UINT32 RemainSize = FileSize;
                UINT32 ReadSize   = BufSize;
                UINT32 OriCrc     = AMBA_CRC32_INIT_VALUE;
                UINT32 OriHwCrc   = AMBA_CRC32_INIT_VALUE;

                while (RemainSize > 0U) {

                    AmbaSvcWrap_MisraMemset(pBuf, 0, BufSize);

                    if (RemainSize > BufSize) {
                        ReadSize = BufSize;
                    } else {
                        ReadSize = RemainSize;
                    }

                    PRetVal = AmbaNVM_ReadRomFile(pCrcInfo->NvmID, pCrcInfo->RegionID,
                                                  pCrcInfo->FileName, (FileSize - RemainSize), ReadSize, pBuf, 1000U);
                    if (PRetVal != 0U) {
                        RetVal = 1U;
                        PRN_NVM_TSK_LOG "Fail to check crc - read the data from rom fail! ErrCode(0x%08X), %s / 0x%08X, 0x%X, 0x%08X"
                        PRN_NVM_TSK_ARG_UINT32 PRetVal                 PRN_NVM_TSK_ARG_POST
                        PRN_NVM_TSK_ARG_CSTR   pCrcInfo->FileName      PRN_NVM_TSK_ARG_POST
                        PRN_NVM_TSK_ARG_UINT32 (FileSize - RemainSize) PRN_NVM_TSK_ARG_POST
                        PRN_NVM_TSK_ARG_UINT32 ReadSize                PRN_NVM_TSK_ARG_POST
                        PRN_NVM_TSK_NG
                    } else {
                        OriCrc = AmbaUtility_Crc32AddSw(pBuf, ReadSize, OriCrc);

                        if (pCrcInfo->InfoType == SVC_NVM_TSK_TYPE_PARTIAL_INFO) {
                            OriHwCrc = AmbaUtility_Crc32Add(pBuf, ReadSize, OriHwCrc);
                        }

                        RemainSize -= ReadSize;
                    }
                }

                pCrcInfo->SwCRC32 = AmbaUtility_Crc32FinalizeSw(OriCrc);

                if (pCrcInfo->InfoType == SVC_NVM_TSK_TYPE_PARTIAL_INFO) {
                    pCrcInfo->HwCRC32 = AmbaUtility_Crc32Finalize(OriHwCrc);
                    PRN_NVM_TSK_LOG "Generate Hardware CRC off-line, %s"
                        PRN_NVM_TSK_ARG_CSTR   pCrcInfo->FileName PRN_NVM_TSK_ARG_POST
                    PRN_NVM_TSK_DBG
                }
            }

            // Check the FileCRC / Software CRC / HardWare CRC
            if (pCrcInfo->SwCRC32 == 0U) {
                RetVal = 2U;
                PRN_NVM_TSK_LOG "The %s Software CRC should not zero"
                    PRN_NVM_TSK_ARG_CSTR   pCrcInfo->FileName PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_NG
            } else if (pCrcInfo->HwCRC32 == 0U) {
                RetVal = 2U;
                PRN_NVM_TSK_LOG "The %s Hardware CRC should not zero"
                    PRN_NVM_TSK_ARG_CSTR   pCrcInfo->FileName PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_NG
            } else if (pCrcInfo->FileCRC32 == 0U) {
                RetVal = 2U;
                PRN_NVM_TSK_LOG "The %s File CRC should not zero"
                    PRN_NVM_TSK_ARG_CSTR   pCrcInfo->FileName PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_NG
            } else {
                if (( pCrcInfo->SwCRC32 != pCrcInfo->HwCRC32 ) ||
                    ( pCrcInfo->SwCRC32 != pCrcInfo->FileCRC32 )) {
                    RetVal = 3U;
                    PRN_NVM_TSK_LOG "The %s CRC does not match between Software(0x%08X), Hardware(0x%08X) and File(0x%08X)"
                        PRN_NVM_TSK_ARG_CSTR   pCrcInfo->FileName  PRN_NVM_TSK_ARG_POST
                        PRN_NVM_TSK_ARG_UINT32 pCrcInfo->SwCRC32   PRN_NVM_TSK_ARG_POST
                        PRN_NVM_TSK_ARG_UINT32 pCrcInfo->HwCRC32   PRN_NVM_TSK_ARG_POST
                        PRN_NVM_TSK_ARG_UINT32 pCrcInfo->FileCRC32 PRN_NVM_TSK_ARG_POST
                    PRN_NVM_TSK_NG
                } else {
                    RetVal = 0U;
                    PRN_NVM_TSK_LOG "The %s CRC check pass"
                        PRN_NVM_TSK_ARG_CSTR   pCrcInfo->FileName PRN_NVM_TSK_ARG_POST
                    PRN_NVM_TSK_DBG
                }
            }
        }
    }

    AmbaMisra_TouchUnused(&RetVal);
}

static void* SvcNvmTask_TaskEntry(void* EntryArg)
{
    UINT32                  ActualFlag = 0U, CurIdx = 0U;
    ULONG                   TskArgVal = 0U;
    SVC_NVM_TSK_CTRL_s      *pCurCtrl = &(SvcNvmTaskCtrl);
    SVC_NVM_TSK_CRC_INFO_s  *pCurInfo;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaSvcWrap_MisraMemcpy(&TskArgVal, EntryArg, sizeof(ULONG));

    while (TskArgVal != 0xCafeU) {

        pCurInfo = NULL;

        (void) AmbaKAL_EventFlagGet(&(pCurCtrl->CtrlFlag),
                                    (SVC_NVM_TSK_CTRL_START | SVC_NVM_TSK_CTRL_DATA_RDY),
                                    AMBA_KAL_FLAGS_ALL,
                                    AMBA_KAL_FLAGS_CLEAR_NONE,
                                    &ActualFlag,
                                    AMBA_KAL_WAIT_FOREVER);

        // Get the crc info
        if (0U == AmbaKAL_MutexTake(&(pCurCtrl->Mutex), AMBA_KAL_WAIT_FOREVER)) {

            CurIdx = pCurCtrl->CheckIdx + 1U;

            if (CurIdx >= SVC_NVM_TSK_MAX_LIST_DEPTH) {
                CurIdx = 0U;
            }

            if (pCurCtrl->NumOfList > 0U) {
                if (pCurCtrl->CrcCheckList[CurIdx].MagicCode == SVC_NVM_TSK_CRC_INFO_MAGIC) {
                    pCurInfo = &(pCurCtrl->CrcCheckList[CurIdx]);
                }
            }

            (void) AmbaKAL_MutexGive(&(pCurCtrl->Mutex));
        }

        // If the crc info is valid
        if (pCurInfo != NULL) {

            SvcNvmTask_CrcCheck(pCurInfo);

            PRN_NVM_TSK_LOG "Checking list[%s%03d%s] : %s%d%s / %s0x%08X%s / %s0x%08X%s / %s%s%s"
                PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_NUM PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_ARG_UINT32 CurIdx                 PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_END PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_NUM PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_ARG_UINT32 pCurInfo->InfoType     PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_END PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_NUM PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_ARG_UINT32 pCurInfo->FileCRC32    PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_END PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_NUM PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_ARG_UINT32 pCurInfo->HwCRC32      PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_END PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_STR PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_ARG_CSTR   pCurInfo->FileName     PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_END PRN_NVM_TSK_ARG_POST
            PRN_NVM_TSK_API

            // Reset Magic Code field
            pCurInfo->MagicCode = 0U;

            // Update the crc check index
            if (0U == AmbaKAL_MutexTake(&(pCurCtrl->Mutex), AMBA_KAL_WAIT_FOREVER)) {
                pCurCtrl->CheckIdx = CurIdx;
                pCurCtrl->NumOfList -= 1U;

                if (pCurCtrl->CheckIdx == pCurCtrl->ListIdx) {
                    (void) AmbaKAL_EventFlagClear(&(pCurCtrl->CtrlFlag), SVC_NVM_TSK_CTRL_DATA_RDY);
                }

                (void) AmbaKAL_MutexGive(&(pCurCtrl->Mutex));
            }
        }

        AmbaMisra_TouchUnused(&TskArgVal);
    }

    return NULL;
}

static UINT32 SvcNvmTask_ShellInfo(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;
    SVC_NVM_TSK_CTRL_s *pCurCtrl = &(SvcNvmTaskCtrl);

    if (ArgCount < 2U) {
        RetVal = SVC_NG;
        PRN_NVM_TSK_LOG "Fail to print check list - Argc should >= 2" PRN_NVM_TSK_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_NVM_TSK_LOG "Fail to print check list - Argv should not null!" PRN_NVM_TSK_NG
    } else if ((SvcNvmTaskFlag & SVC_NVM_TSK_FLAG_INIT) == 0U) {
        PRN_NVM_TSK_LOG "Fail to print check list - init NVM task module first!" PRN_NVM_TSK_NG
    } else {
        UINT32 MutexTimeOut = 10000U;

        if (0U != AmbaKAL_MutexTake(&(pCurCtrl->Mutex), MutexTimeOut)) {
            PRN_NVM_TSK_LOG "Fail to print check list - take mutex timeout! 0x%08X"
                PRN_NVM_TSK_ARG_UINT32 MutexTimeOut PRN_NVM_TSK_ARG_POST
            PRN_NVM_TSK_NG
        } else {
            UINT32 Idx, IsMagicCode;
            const SVC_NVM_TSK_CRC_INFO_s *pInfo;
            UINT32 R[2], G[2], B[2];
            char HLStr_0[64];
            char HLStr_F[64];
            char HLStr_H[64];
            char HLStr_S[64];

            SVC_WRAP_PRINT_s PrintCtrl;
            SVC_WRAP_SNPRINT_s StrFmtCtrl;
            const char *pArgVal;

            PRN_NVM_TSK_LOG " " PRN_NVM_TSK_API
            PRN_NVM_TSK_LOG "  Idx | NID | RID | Type |  File CRC  |  Hard CRC  |  Soft CRC  | C | FileName" PRN_NVM_TSK_API

            for (Idx = 0U; Idx < SVC_NVM_TSK_MAX_LIST_DEPTH; Idx ++) {

                pInfo = &(pCurCtrl->CrcCheckList[Idx]);

                // Prepare HL

                R[0] = 0U; G[0] = 0U; B[0] = 0U; R[1] = 0U; G[1] = 0U; B[1] = 0U;

                AmbaSvcWrap_MisraMemset(HLStr_0, 0, sizeof(HLStr_0));
                if (Idx == pCurCtrl->CheckIdx) {
                    R[0] = 255U; G[0] = 0U; B[0] = 128U;
                }
                if (Idx == pCurCtrl->ListIdx) {
                    R[1] = 255U; G[1] = 255U; B[1] = 128U;
                }
                if ((Idx != pCurCtrl->CheckIdx) && (Idx != pCurCtrl->ListIdx)) {
                    SvcWrap_strcpy(HLStr_0, sizeof(HLStr_0), SVC_LOG_NVM_TSK_HL_END);
                } else {
                    AmbaSvcWrap_MisraMemset(&StrFmtCtrl, 0, sizeof(StrFmtCtrl));
                    StrFmtCtrl.pStrFmt = "\033""[38;2;%d;%d;%dm""\033""[48;2;%d;%d;%dm";
                    AmbaMisra_TypeCast(&(StrFmtCtrl.Argv[StrFmtCtrl.Argc].Uint64), &(R[0])); StrFmtCtrl.Argc ++;
                    AmbaMisra_TypeCast(&(StrFmtCtrl.Argv[StrFmtCtrl.Argc].Uint64), &(G[0])); StrFmtCtrl.Argc ++;
                    AmbaMisra_TypeCast(&(StrFmtCtrl.Argv[StrFmtCtrl.Argc].Uint64), &(B[0])); StrFmtCtrl.Argc ++;
                    AmbaMisra_TypeCast(&(StrFmtCtrl.Argv[StrFmtCtrl.Argc].Uint64), &(R[1])); StrFmtCtrl.Argc ++;
                    AmbaMisra_TypeCast(&(StrFmtCtrl.Argv[StrFmtCtrl.Argc].Uint64), &(G[1])); StrFmtCtrl.Argc ++;
                    AmbaMisra_TypeCast(&(StrFmtCtrl.Argv[StrFmtCtrl.Argc].Uint64), &(B[1])); StrFmtCtrl.Argc ++;
                    (void) SvcWrap_snprintf(HLStr_0, (UINT32)sizeof(HLStr_0), &StrFmtCtrl);
                }

                // Prepare File CRC HL
                AmbaSvcWrap_MisraMemset(HLStr_F, 0, sizeof(HLStr_F));
                if ((pInfo->FileCRC32 != pInfo->HwCRC32) && (pInfo->FileCRC32 != pInfo->SwCRC32)) {

                    AmbaSvcWrap_MisraMemset(&StrFmtCtrl, 0, sizeof(StrFmtCtrl));
                    StrFmtCtrl.pStrFmt = "\033""[38;2;237;28;36m""\033""[48;2;%d;%d;%dm";
                    AmbaMisra_TypeCast(&(StrFmtCtrl.Argv[StrFmtCtrl.Argc].Uint64), &(R[1])); StrFmtCtrl.Argc ++;
                    AmbaMisra_TypeCast(&(StrFmtCtrl.Argv[StrFmtCtrl.Argc].Uint64), &(G[1])); StrFmtCtrl.Argc ++;
                    AmbaMisra_TypeCast(&(StrFmtCtrl.Argv[StrFmtCtrl.Argc].Uint64), &(B[1])); StrFmtCtrl.Argc ++;
                    (void) SvcWrap_snprintf(HLStr_F, (UINT32)sizeof(HLStr_F), &StrFmtCtrl);
                } else {
                    SvcWrap_strcpy(HLStr_F, sizeof(HLStr_F), HLStr_0);
                }

                // Prepare Hard CRC HL
                AmbaSvcWrap_MisraMemset(HLStr_H, 0, sizeof(HLStr_H));
                if ((pInfo->HwCRC32 != pInfo->FileCRC32) && (pInfo->HwCRC32 != pInfo->SwCRC32)) {

                    AmbaSvcWrap_MisraMemset(&StrFmtCtrl, 0, sizeof(StrFmtCtrl));
                    StrFmtCtrl.pStrFmt = "\033""[38;2;34;177;76m""\033""[48;2;%d;%d;%dm";
                    AmbaMisra_TypeCast(&(StrFmtCtrl.Argv[StrFmtCtrl.Argc].Uint64), &(R[1])); StrFmtCtrl.Argc ++;
                    AmbaMisra_TypeCast(&(StrFmtCtrl.Argv[StrFmtCtrl.Argc].Uint64), &(G[1])); StrFmtCtrl.Argc ++;
                    AmbaMisra_TypeCast(&(StrFmtCtrl.Argv[StrFmtCtrl.Argc].Uint64), &(B[1])); StrFmtCtrl.Argc ++;
                    (void) SvcWrap_snprintf(HLStr_H, (UINT32)sizeof(HLStr_H), &StrFmtCtrl);
                } else {
                    SvcWrap_strcpy(HLStr_H, sizeof(HLStr_H), HLStr_0);
                }

                // Prepare Soft CRC HL
                AmbaSvcWrap_MisraMemset(HLStr_S, 0, sizeof(HLStr_S));
                if ((pInfo->SwCRC32 != pInfo->FileCRC32) && (pInfo->SwCRC32 != pInfo->HwCRC32)) {

                    AmbaSvcWrap_MisraMemset(&StrFmtCtrl, 0, sizeof(StrFmtCtrl));
                    StrFmtCtrl.pStrFmt = "\033""[38;2;0;162;232m""\033""[48;2;%d;%d;%dm";
                    AmbaMisra_TypeCast(&(StrFmtCtrl.Argv[StrFmtCtrl.Argc].Uint64), &(R[1])); StrFmtCtrl.Argc ++;
                    AmbaMisra_TypeCast(&(StrFmtCtrl.Argv[StrFmtCtrl.Argc].Uint64), &(G[1])); StrFmtCtrl.Argc ++;
                    AmbaMisra_TypeCast(&(StrFmtCtrl.Argv[StrFmtCtrl.Argc].Uint64), &(B[1])); StrFmtCtrl.Argc ++;
                    (void) SvcWrap_snprintf(HLStr_S, (UINT32)sizeof(HLStr_S), &StrFmtCtrl);
                } else {
                    SvcWrap_strcpy(HLStr_S, sizeof(HLStr_S), HLStr_0);
                }

                if (pInfo->MagicCode == 0U) {
                    IsMagicCode = 0U;
                } else {
                    IsMagicCode = 1U;
                }

                AmbaSvcWrap_MisraMemset(&(PrintCtrl), 0, sizeof(PrintCtrl));
                PrintCtrl.pStrFmt = "%s  %03d |  %02d |  %02d | %s | %s0x%08X%s | %s0x%08X%s | %s0x%08X%s | %d | %s%s";
                pArgVal = HLStr_0;
                AmbaMisra_TypeCast(&(PrintCtrl.Argv[PrintCtrl.Argc].Uint64), &(pArgVal         )); PrintCtrl.Argc ++;
                AmbaMisra_TypeCast(&(PrintCtrl.Argv[PrintCtrl.Argc].Uint64), &(Idx             )); PrintCtrl.Argc ++;
                AmbaMisra_TypeCast(&(PrintCtrl.Argv[PrintCtrl.Argc].Uint64), &(pInfo->NvmID    )); PrintCtrl.Argc ++;
                AmbaMisra_TypeCast(&(PrintCtrl.Argv[PrintCtrl.Argc].Uint64), &(pInfo->RegionID )); PrintCtrl.Argc ++;
                pArgVal = ((pInfo->InfoType == SVC_NVM_TSK_TYPE_NORMAL_INFO)?"Full":"Part");
                AmbaMisra_TypeCast(&(PrintCtrl.Argv[PrintCtrl.Argc].Uint64), &(pArgVal         )); PrintCtrl.Argc ++;
                pArgVal = HLStr_F;
                AmbaMisra_TypeCast(&(PrintCtrl.Argv[PrintCtrl.Argc].Uint64), &(pArgVal         )); PrintCtrl.Argc ++;
                AmbaMisra_TypeCast(&(PrintCtrl.Argv[PrintCtrl.Argc].Uint64), &(pInfo->FileCRC32)); PrintCtrl.Argc ++;
                pArgVal = HLStr_0;
                AmbaMisra_TypeCast(&(PrintCtrl.Argv[PrintCtrl.Argc].Uint64), &(pArgVal         )); PrintCtrl.Argc ++;
                pArgVal = HLStr_H;
                AmbaMisra_TypeCast(&(PrintCtrl.Argv[PrintCtrl.Argc].Uint64), &(pArgVal         )); PrintCtrl.Argc ++;
                AmbaMisra_TypeCast(&(PrintCtrl.Argv[PrintCtrl.Argc].Uint64), &(pInfo->HwCRC32  )); PrintCtrl.Argc ++;
                pArgVal = HLStr_0;
                AmbaMisra_TypeCast(&(PrintCtrl.Argv[PrintCtrl.Argc].Uint64), &(pArgVal         )); PrintCtrl.Argc ++;
                pArgVal = HLStr_S;
                AmbaMisra_TypeCast(&(PrintCtrl.Argv[PrintCtrl.Argc].Uint64), &(pArgVal         )); PrintCtrl.Argc ++;
                AmbaMisra_TypeCast(&(PrintCtrl.Argv[PrintCtrl.Argc].Uint64), &(pInfo->SwCRC32  )); PrintCtrl.Argc ++;
                pArgVal = HLStr_0;
                AmbaMisra_TypeCast(&(PrintCtrl.Argv[PrintCtrl.Argc].Uint64), &(pArgVal         )); PrintCtrl.Argc ++;
                AmbaMisra_TypeCast(&(PrintCtrl.Argv[PrintCtrl.Argc].Uint64), &(IsMagicCode     )); PrintCtrl.Argc ++;
                pArgVal = pInfo->FileName;
                AmbaMisra_TypeCast(&(PrintCtrl.Argv[PrintCtrl.Argc].Uint64), &(pArgVal         )); PrintCtrl.Argc ++;
                pArgVal = SVC_LOG_NVM_TSK_HL_END;
                AmbaMisra_TypeCast(&(PrintCtrl.Argv[PrintCtrl.Argc].Uint64), &(pArgVal         )); PrintCtrl.Argc ++;
                SvcNvmTask_PrintLog(SVC_NVM_TSK_FLAG_API_MSG, &(PrintCtrl));
            }

            (void) AmbaKAL_MutexGive(&(pCurCtrl->Mutex));
        }
    }

    return RetVal;
}

static void SvcNvmTask_ShellInfoU(UINT32 CtrlFlag)
{
    PRN_NVM_TSK_LOG "    %sinfo%s              : print check list info%s"
        PRN_NVM_TSK_ARG_CSTR   ((CtrlFlag > 0U)?SVC_LOG_NVM_TSK_HL_TITLE_2:SVC_LOG_NVM_TSK_HL_TITLE_1) PRN_NVM_TSK_ARG_POST
        PRN_NVM_TSK_ARG_CSTR   ((CtrlFlag > 0U)?SVC_LOG_NVM_TSK_HL_DEF:SVC_LOG_NVM_TSK_HL_END)         PRN_NVM_TSK_ARG_POST
        PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_END                                                  PRN_NVM_TSK_ARG_POST
    PRN_NVM_TSK_API
    if (CtrlFlag > 0U) {
        PRN_NVM_TSK_LOG " " PRN_NVM_TSK_API
    }
}

static UINT32 SvcNvmTask_ShellDbgMsg(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount < 3U) {
        RetVal = SVC_NG;
        PRN_NVM_TSK_LOG "Fail to switch debug msg - Argc should >= 3" PRN_NVM_TSK_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_NVM_TSK_LOG "Fail to switch debug msg - Argv should not null!" PRN_NVM_TSK_NG
    } else {
        UINT32 DbgMsgOn = 0U;

        PRetVal = SvcWrap_strtoul(pArgVector[3U],  &DbgMsgOn ); AmbaMisra_TouchUnused(&PRetVal);

        if (DbgMsgOn == 0U) {
            SvcNvmTaskFlag &= ~SVC_NVM_TSK_FLAG_DBG_MSG;

            PRN_NVM_TSK_LOG "Success to disable debug message" PRN_NVM_TSK_NG
        } else {
            SvcNvmTaskFlag |= SVC_NVM_TSK_FLAG_DBG_MSG;

            PRN_NVM_TSK_LOG "Success to enable debug message" PRN_NVM_TSK_NG
        }
    }

    return RetVal;
}

static void SvcNvmTask_ShellDbgMsgU(UINT32 CtrlFlag)
{
    PRN_NVM_TSK_LOG "    %sdebug%s             : NVM task debug message%s"
        PRN_NVM_TSK_ARG_CSTR   ((CtrlFlag > 0U)?SVC_LOG_NVM_TSK_HL_TITLE_2:SVC_LOG_NVM_TSK_HL_TITLE_1) PRN_NVM_TSK_ARG_POST
        PRN_NVM_TSK_ARG_CSTR   ((CtrlFlag > 0U)?SVC_LOG_NVM_TSK_HL_DEF:SVC_LOG_NVM_TSK_HL_END)         PRN_NVM_TSK_ARG_POST
        PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_END                                                  PRN_NVM_TSK_ARG_POST
    PRN_NVM_TSK_API
    if (CtrlFlag > 0U) {
        PRN_NVM_TSK_LOG "      Enable          : 0 : disable debug message" PRN_NVM_TSK_API
        PRN_NVM_TSK_LOG "                      : 1 : enable debug message" PRN_NVM_TSK_API
        PRN_NVM_TSK_LOG " " PRN_NVM_TSK_API
    }
}

static void SvcNvmTask_ShellEntryInit(void)
{
    if ((SvcNvmTaskFlag & SVC_NVM_TSK_FLAG_SHELL_INIT) == 0U) {
        UINT32 Cnt = 0U;

        AmbaSvcWrap_MisraMemset(SvcNvmTaskShellFunc, 0, sizeof(SvcNvmTaskShellFunc));
        SvcNvmTaskShellFunc[Cnt] = (SVC_NVM_TSK_SHELL_FUNC_s) { "debug", SvcNvmTask_ShellDbgMsg, SvcNvmTask_ShellDbgMsgU }; Cnt++;
        SvcNvmTaskShellFunc[Cnt] = (SVC_NVM_TSK_SHELL_FUNC_s) { "info",  SvcNvmTask_ShellInfo,   SvcNvmTask_ShellInfoU   };

        SvcNvmTaskFlag |= SVC_NVM_TSK_FLAG_SHELL_INIT;
    }
}

static void SvcNvmTask_ShellUsage(UINT32 CtrlFlag)
{
    UINT32 ShellIdx, ShellCnt = ((UINT32)(sizeof(SvcNvmTaskShellFunc))) / ((UINT32)(sizeof(SvcNvmTaskShellFunc[0])));

    PRN_NVM_TSK_LOG " " PRN_NVM_TSK_API
    PRN_NVM_TSK_LOG "====== %sSVC NVM Task Command Usage%s ======"
        PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_TITLE_0 PRN_NVM_TSK_ARG_POST
        PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_END     PRN_NVM_TSK_ARG_POST
    PRN_NVM_TSK_API

    for (ShellIdx = 0U; ShellIdx < ShellCnt; ShellIdx ++) {
        if (SvcNvmTaskShellFunc[ShellIdx].pUsage == NULL) {
            PRN_NVM_TSK_LOG "  %s"
                PRN_NVM_TSK_ARG_CSTR   SvcNvmTaskShellFunc[ShellIdx].CmdName PRN_NVM_TSK_ARG_POST
            PRN_NVM_TSK_API
        } else {
            (SvcNvmTaskShellFunc[ShellIdx].pUsage)(CtrlFlag);
        }
    }

    PRN_NVM_TSK_LOG "Type 'more' option to print more command description" PRN_NVM_TSK_API
}

/**
 *  shell command entry
 *  @param[in] ArgCount Input arguments counter
 *  @param[in] pArgVector Input arguments data
 */
void SvcNvmTask_ShellEntry(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_NG;
    UINT32 CtrlFlag = 0U;

    SvcNvmTask_ShellEntryInit();

    if(ArgCount == 0xCafeU) {
        UINT32 SIdx;

        PRN_NVM_TSK_LOG " " PRN_NVM_TSK_API
        PRN_NVM_TSK_LOG "==== %sShell Arguments Setting%s (%s%d%s) ===="
            PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_TITLE_1 PRN_NVM_TSK_ARG_POST
            PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_END     PRN_NVM_TSK_ARG_POST
            PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_NUM     PRN_NVM_TSK_ARG_POST
            PRN_NVM_TSK_ARG_UINT32 ArgCount                   PRN_NVM_TSK_ARG_POST
            PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_END     PRN_NVM_TSK_ARG_POST
        PRN_NVM_TSK_API

        for (SIdx = 0U; SIdx < ArgCount; SIdx ++) {
            PRN_NVM_TSK_LOG "  pArgVector[%s%d%s] : %s%s%s"
                PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_NUM PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_ARG_UINT32 SIdx                   PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_END PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_STR PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_ARG_CSTR   pArgVector[SIdx]       PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_ARG_CSTR   SVC_LOG_NVM_TSK_HL_END PRN_NVM_TSK_ARG_POST
            PRN_NVM_TSK_API
        }
    }

    if (2U <= ArgCount) {
        UINT32 CmdCnt = (UINT32)sizeof(SvcNvmTaskShellFunc) / (UINT32)sizeof(SvcNvmTaskShellFunc[0]);
        UINT32 CmdIdx;

        if (0 == SvcWrap_strcmp(pArgVector[1U], "more")) {
            CtrlFlag = 1U;
        } else {

            for (CmdIdx = 0; CmdIdx < CmdCnt; CmdIdx ++) {
                if (0 == SvcWrap_strcmp(pArgVector[1U], SvcNvmTaskShellFunc[CmdIdx].CmdName)) {
                    if (SvcNvmTaskShellFunc[CmdIdx].pFunc != NULL) {
                        if (0U != (SvcNvmTaskShellFunc[CmdIdx].pFunc)(ArgCount, pArgVector)) {
                            if (SvcNvmTaskShellFunc[CmdIdx].pUsage != NULL) {
                                (SvcNvmTaskShellFunc[CmdIdx].pUsage)(1U);
                            }
                        }
                        RetVal = SVC_OK;
                    }
                }
            }
        }
    }

    if (RetVal != SVC_OK) {
        SvcNvmTask_ShellUsage(CtrlFlag);
    }
}

/**
 *  Query Nvm task memory
 *  @return query result
 */
UINT32 SvcNvmTask_MemQry(void)
{
    return (SVC_NVM_TSK_MAX_WORK_SIZE);
}

#ifdef CONFIG_ICAM_IMGPROC_USED
static UINT32 ReadRomFileCrc(UINT32 NvmID, UINT32 RegionID, const char *pFileName, UINT32 StartPos, UINT32 Size, UINT8 *pDataBuf, UINT32 TimeOut)
{
    UINT32 UserPartID = AMBA_NUM_USER_PARTITION;
    UINT32 Rval = SVC_OK;
    AmbaMisra_TouchUnused(&NvmID);

    if (AMBA_NVM_ROM_REGION_SYS_DATA == RegionID) {
        UserPartID = AMBA_USER_PARTITION_SYS_DATA;
    } else if (AMBA_NVM_ROM_REGION_DSP_uCODE == RegionID) {
        UserPartID = AMBA_USER_PARTITION_DSP_uCODE;
    } else {
        PRN_NVM_TSK_LOG "SvcNvm Task Error: invalid Region ID !" PRN_NVM_TSK_API
        Rval = SVC_NG;
    }
    
    if (SVC_OK == Rval) {
        Rval = SvcNvm_ReadRomFileCrc(UserPartID, pFileName, StartPos, Size, pDataBuf, TimeOut);
    }
    
    return Rval;
}
#endif

/**
 *  Create SVC NVM CRC checking task
 *  @return error code
 */
UINT32 SvcNvmTask_Create(void)
{
    UINT8  *pBase;
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SvcNvmTaskFlag & SVC_NVM_TSK_FLAG_INIT) > 0U) {
        PRN_NVM_TSK_LOG "SvcNvm Task has been created!" PRN_NVM_TSK_API
    } else {
        ULONG  BufBase = 0U;
        UINT32 BufSize = 0U;

        PRetVal = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_NVM_CRC_CHECK, &BufBase, &BufSize);
        if (PRetVal != SVC_OK) {
            RetVal = SVC_NG;
            PRN_NVM_TSK_LOG "Fail to create NVM task - request memory fail! ErrCode(0x%X)"
                PRN_NVM_TSK_ARG_UINT32 PRetVal PRN_NVM_TSK_ARG_POST
            PRN_NVM_TSK_NG
        } else if ((BufBase == 0U) || (BufSize == 0U)) {
            RetVal = SVC_NG;
            PRN_NVM_TSK_LOG "Fail to create NVM task - invalid buffer(0x%08X / 0x%X)"
                PRN_NVM_TSK_ARG_UINT32 (UINT32)BufBase PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_ARG_UINT32 BufSize         PRN_NVM_TSK_ARG_POST
            PRN_NVM_TSK_NG
        } else if (BufSize < SVC_NVM_TSK_MAX_WORK_SIZE) {
            RetVal = SVC_NG;
            PRN_NVM_TSK_LOG "Fail to create NVM task - buffer size(0x%X) smaller than requested size(0x%X)"
                PRN_NVM_TSK_ARG_UINT32 BufSize PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_ARG_UINT32 SVC_NVM_TSK_MAX_WORK_SIZE PRN_NVM_TSK_ARG_POST
            PRN_NVM_TSK_NG
        } else {
            // Configure the SvcNvmTask control
            AmbaMisra_TypeCast(&(pBase), &(BufBase));

            // reset the SvcNvmTask
            AmbaSvcWrap_MisraMemset(&SvcNvmTaskCtrl, 0, sizeof(SvcNvmTaskCtrl));
            SvcNvmTaskCtrl.WorkBuf = pBase;

            AmbaUtility_StringCopy(SvcNvmTaskCtrl.Name, sizeof(SvcNvmTaskCtrl.Name), "SvcNvmTask");
            SvcNvmTaskCtrl.CheckIdx  = 0xFFFFFFFFU;
            SvcNvmTaskCtrl.ListIdx   = 0xFFFFFFFFU;
            SvcNvmTaskCtrl.NumOfList = 0U;

            // Create event flag
            PRetVal = AmbaKAL_EventFlagCreate(&(SvcNvmTaskCtrl.CtrlFlag), SvcNvmTaskCtrl.Name);
            if (PRetVal != 0U) {
                RetVal = SVC_NG;
                PRN_NVM_TSK_LOG "Fail to create NVM task - create event flag fail! ErrCode(0x%X)"
                    PRN_NVM_TSK_ARG_UINT32 PRetVal PRN_NVM_TSK_ARG_POST
                PRN_NVM_TSK_NG
            } else {
                // Reset event flag setting
                (void) AmbaKAL_EventFlagClear(&(SvcNvmTaskCtrl.CtrlFlag), 0xFFFFFFFFU);
            }

            // Create the mutex
            if (RetVal == SVC_OK) {
                PRetVal = AmbaKAL_MutexCreate(&(SvcNvmTaskCtrl.Mutex), SvcNvmTaskCtrl.Name);
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    PRN_NVM_TSK_LOG "Fail to create NVM task - create mutex fail! ErrCode(0x%X)"
                        PRN_NVM_TSK_ARG_UINT32 PRetVal PRN_NVM_TSK_ARG_POST
                    PRN_NVM_TSK_NG
                }
            }

            // Register callback
            if (RetVal == SVC_OK) {
#ifdef CONFIG_ICAM_IMGPROC_USED
                // Register SvcNVM to AmbaIQParamHandler
                (void) Amba_IQParam_RegisterReadRomFile(ReadRomFileCrc);
#endif

                // Register SvcNVM callback
                (void) SvcNvm_RegisterReadRomPost(SvcNvmTask_ReadRomPostProc);
            }

            // Register SysState
            if (RetVal == SVC_OK) {
                PRetVal = SvcSysStat_Register(SVC_APP_STAT_DSP_BOOT, SvcNvmTask_SysStateBootDone, &(SvcNvmTaskCtrl.SysStateCtrlID));
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    PRN_NVM_TSK_LOG "Fail to create NVM task - register sys state fail! ErrCode(0x%X)"
                        PRN_NVM_TSK_ARG_UINT32 RetVal PRN_NVM_TSK_ARG_POST
                    PRN_NVM_TSK_NG
                }
            }

            // Create the task
            if (RetVal == SVC_OK) {
                SvcNvmTaskCtrl.TaskCtrl.CpuBits    = SVC_NVM_CRC_CHK_TASK_CPU_BITS;
                SvcNvmTaskCtrl.TaskCtrl.Priority   = (UINT32)SVC_NVM_CRC_CHK_TASK_PRI;
                SvcNvmTaskCtrl.TaskCtrl.pStackBase = SvcNvmTaskCtrl.TaskStack;
                SvcNvmTaskCtrl.TaskCtrl.StackSize  = (UINT32)sizeof(SvcNvmTaskCtrl.TaskStack);
                SvcNvmTaskCtrl.TaskCtrl.EntryArg   = 0U;
                SvcNvmTaskCtrl.TaskCtrl.EntryFunc  = SvcNvmTask_TaskEntry;

                PRetVal = SvcTask_Create(SvcNvmTaskCtrl.Name, &(SvcNvmTaskCtrl.TaskCtrl));
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    PRN_NVM_TSK_LOG "Fail to create NVM task - create task fail! ErrCode(0x%X)"
                        PRN_NVM_TSK_ARG_UINT32 RetVal PRN_NVM_TSK_ARG_POST
                    PRN_NVM_TSK_NG
                }
            }
        }

        if (RetVal == SVC_OK) {
            SvcNvmTaskFlag |= SVC_NVM_TSK_FLAG_INIT;
            PRN_NVM_TSK_LOG "Success to create NVM task" PRN_NVM_TSK_OK
        } else {
            // Stop the callback process
            (void) AmbaKAL_EventFlagClear(&(SvcNvmTaskCtrl.CtrlFlag), SVC_NVM_TSK_CTRL_START);

            // Unregister SvcNVM callback
            (void) SvcNvm_RegisterReadRomPost(NULL);

#ifdef CONFIG_ICAM_IMGPROC_USED
            // Unregister SvcNVM to AmbaIQParamHandler
            (void) Amba_IQParam_RegisterReadRomFile(NULL);
#endif

            // Unregister SysState
            (void) SvcSysStat_Unregister(SVC_APP_STAT_DSP_BOOT, SvcNvmTaskCtrl.SysStateCtrlID);

            // Delete task
            (void) SvcTask_Destroy(&(SvcNvmTaskCtrl.TaskCtrl));

            // Delete event flag
            (void) AmbaKAL_EventFlagDelete(&(SvcNvmTaskCtrl.CtrlFlag));

            // Delete mutex
            (void) AmbaKAL_MutexDelete(&(SvcNvmTaskCtrl.Mutex));
        }
    }

    return RetVal;
}

/**
 *  Delete SVC NVM CRC checking task
 *  @return error code
 */
UINT32 SvcNvmTask_Delete(void)
{
    UINT32 RetVal = SVC_OK;

    if ((SvcNvmTaskFlag & SVC_NVM_TSK_FLAG_INIT) == 0U) {
        PRN_NVM_TSK_LOG "SvcNvm Task has been delete!" PRN_NVM_TSK_API
    } else {
        // Stop the callback process
        (void) AmbaKAL_EventFlagClear(&(SvcNvmTaskCtrl.CtrlFlag), SVC_NVM_TSK_CTRL_START);

        // Unregister SvcNVM callback
        (void) SvcNvm_RegisterReadRomPost(NULL);

#ifdef CONFIG_ICAM_IMGPROC_USED
        // Unregister SvcNVM to AmbaIQParamHandler
        (void) Amba_IQParam_RegisterReadRomFile(NULL);
#endif

        // Unregister SysState
        (void) SvcSysStat_Unregister(SVC_APP_STAT_DSP_BOOT, SvcNvmTaskCtrl.SysStateCtrlID);

        // Delete task
        (void) SvcTask_Destroy(&(SvcNvmTaskCtrl.TaskCtrl));

        // Delete event flag
        (void) AmbaKAL_EventFlagDelete(&(SvcNvmTaskCtrl.CtrlFlag));

        // Delete mutex
        (void) AmbaKAL_MutexDelete(&(SvcNvmTaskCtrl.Mutex));

        // Reset control memory
        AmbaSvcWrap_MisraMemset(&SvcNvmTaskCtrl, 0, sizeof(SVC_NVM_TSK_CTRL_s));

        SvcNvmTaskFlag &= ~SVC_NVM_TSK_FLAG_INIT;

        PRN_NVM_TSK_LOG "Success to delete NVM task" PRN_NVM_TSK_OK
    }

    return RetVal;
}
