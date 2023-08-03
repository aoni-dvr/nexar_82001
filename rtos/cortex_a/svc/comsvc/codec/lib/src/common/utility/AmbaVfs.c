/**
*  @file AmbaVfs.c
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
*  @details Virtual layer of file system
*
*/

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaMisraFix.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "AmbaCFS.h"
#include "AmbaSD.h"
#include "AmbaSvcWrap.h"
#include "AmbaVfs.h"
#include "AmbaCodecCom.h"

#define VFS_BANK_SIZE              (CONFIG_AMBA_VFS_BANK_SIZE)
#define VFS_MAX_ASYNC_NUM          (CONFIG_AMBA_VFS_ASYNC_MAX_NUM)
#define VFS_BANK_NUM               (CONFIG_AMBA_VFS_BANK_PER_FILE)
#define VFS_EVENT_MAX_NUM          (1024U)

#define VFS_PROCESS_LIMIT_TIME     (1000U) /* ms */

#define VFS_BANK_OP_PLUS           (0U)
#define VFS_BANK_OP_MINUS          (1U)
#define VFS_BANK_OP_SET            (2U)
#define VFS_BANK_OP_GET            (3U)
#define VFS_BANK_OP_NUM            (4U)

#define VFS_ASYNC_FLG_NULL_WRITE   (1U)
#define VFS_ASYNC_FLG_CLOSE_DONE   (2U)

typedef struct {
    UINT32                   IsUsed;
    ULONG                    BankAddr;
    AMBA_CFS_FILE_s          *pFile;
    UINT32                   RSize;
    AMBA_KAL_EVENT_FLAG_t    Flag;
} AMBA_VFS_ASYNC_s;

typedef struct {
    AMBA_CFS_INIT_CFG_s   Init;
    AMBA_VFS_ASYNC_s      Async[VFS_MAX_ASYNC_NUM];
    AMBA_CFS_EVENT_INFO_s EventPool[VFS_EVENT_MAX_NUM];
    AMBA_KAL_TASK_t       Task;
    AMBA_KAL_MUTEX_t      Mutex;
    AMBA_KAL_MUTEX_t      AsyncMutex;
} AMBA_VFS_INFO_s;



static AMBA_VFS_INFO_s Info   GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
static VfsCb           VfsCbFunc[AMBA_VFS_CB_TYPE_MAX] = {NULL};




static void VfsLogNg(const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    UINT32 Err;
    char   StrBuf[128U];

    Err = AmbaWrap_memset(StrBuf, 0, sizeof(StrBuf));
    if (Err != CODEC_OK) {
        AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaWrap_memset failed %u", Err, 0U, 0U, 0U, 0U);
    }
    AmbaUtility_StringAppend(StrBuf, 128U, "\033""[0;31m");
    AmbaUtility_StringAppend(StrBuf, 128U, "[VFS]: ");
    AmbaUtility_StringAppend(StrBuf, 128U, pFormat);
    AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, StrBuf, Arg1, Arg2, 0U, 0U, 0U);
}

static UINT32 CheckSdStatus(char Drive)
{
    UINT32                 Rval = CODEC_OK;
    AMBA_SD_CARD_STATUS_s  CardStatus;
#ifndef CONFIG_LINUX
    if ((Drive == 'c') || (Drive == 'C')) {
        if (AmbaSD_GetCardStatus(AMBA_SD_CHANNEL0, &CardStatus) != OK) {
            Rval = CODEC_ERROR_GENERAL_ERROR;
        }
    } else if ((Drive == 'd') || (Drive == 'D')) {
        if (AmbaSD_GetCardStatus(AMBA_SD_CHANNEL1, &CardStatus) != OK) {
            Rval = CODEC_ERROR_GENERAL_ERROR;
        }
    } else {
        /* not sd drive, do nothing */
    }
#else
    AmbaMisra_TouchUnused(&Drive);
    AmbaMisra_TouchUnused(&CardStatus);
#endif

    return Rval;
}

static UINT32 BufferSizeUpdate(UINT32 AsyncId, UINT32 Size, UINT32 Operation)
{
    UINT32 Rval = CODEC_OK, Err, Rsize = 0U;

    AmbaMisra_TouchUnused(&Rval);

    if (Rval == CODEC_OK) {
        if (AsyncId >= (UINT32)VFS_MAX_ASYNC_NUM) {
            VfsLogNg("Invalid AsyncId", AsyncId, 0U);
            Rval = CODEC_ERROR_GENERAL_ERROR;
        }

        if (Operation >= VFS_BANK_OP_NUM) {
            VfsLogNg("Invalid Operation", Operation, 0U);
            Rval = CODEC_ERROR_GENERAL_ERROR;
        }
    }

    if (Rval == CODEC_OK) {
        /* pass misra rule */
        AmbaMisra_TouchUnused(&AsyncId);

        if (AsyncId < (UINT32)VFS_MAX_ASYNC_NUM) {
            Err = AmbaKAL_MutexTake(&(Info.AsyncMutex), 1500U);
            if (Err != OK) {
                VfsLogNg("AmbaKAL_MutexTake AsyncMutex failed %u", Err, 0U);
                Rval = CODEC_ERROR_GENERAL_ERROR;
            }

            switch (Operation) {
                case VFS_BANK_OP_PLUS:
                    Info.Async[AsyncId].RSize += Size;
                    break;
                case VFS_BANK_OP_MINUS:
                    Info.Async[AsyncId].RSize -= Size;
                    break;
                case VFS_BANK_OP_SET:
                    Info.Async[AsyncId].RSize  = Size;
                    break;
                default:
                    /* nothing */
                    break;
            }

            Rsize = Info.Async[AsyncId].RSize;

            Err = AmbaKAL_MutexGive(&(Info.AsyncMutex));
            if (Err != OK) {
                VfsLogNg("AmbaKAL_MutexGive AsyncMutex failed %u", Err, 0U);
                Rval = CODEC_ERROR_GENERAL_ERROR;
            }
        }
    }

    AmbaMisra_TouchUnused(&Rval);

    return Rsize;
}

static void* CfsTaskEntry(void* EntryArg)
{
    UINT32                       Err, LoopU = 1U, i, j, ActualFlags;
    UINT32                       ActEventNum = 0U, AsyncId, RSize, TStart = 0U, TEnd = 0U;
    const AMBA_CFS_EVENT_INFO_s  *pEvent;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TouchUnused(&LoopU);

    while (LoopU == 1U) {
        Err = AmbaCFS_GetEvent(VFS_EVENT_MAX_NUM, KAL_WAIT_FOREVER, Info.EventPool, &ActEventNum);
        if (Err != OK) {
            VfsLogNg("AmbaCFS_GetEvent failed %u", Err, 0U);
            LoopU = 0U;
        } else {
            /* Process Event one by one */
            for (i = 0U; i < ActEventNum; i++) {
                pEvent      = &(Info.EventPool[i]);
                ActualFlags = 0U;
                AsyncId     = VFS_MAX_ASYNC_NUM;

                for (j = 0U; j < (UINT32)VFS_MAX_ASYNC_NUM; j++) {
                    if (pEvent->File == Info.Async[j].pFile) {
                        AsyncId = j;
                    }
                }

                if (AsyncId < (UINT32)VFS_MAX_ASYNC_NUM) {
                    switch (pEvent->Event) {
                        case AMBA_CFS_EVENT_ASYNC_WRITE:
                            Err = AmbaKAL_EventFlagGet(&(Info.Async[AsyncId].Flag), VFS_ASYNC_FLG_NULL_WRITE, AMBA_KAL_FLAGS_ALL,
                                                        KAL_FLAGS_CLEAR_NONE, &ActualFlags, AMBA_KAL_NO_WAIT);
                            AmbaMisra_TouchUnused(&Err);

                            if ((ActualFlags & VFS_ASYNC_FLG_NULL_WRITE) == 0U) {
                                RSize = BufferSizeUpdate(AsyncId, 0U, VFS_BANK_OP_GET);
                                Err = AmbaKAL_GetSysTickCount(&TStart);
                                if (Err != CODEC_OK) {
                                    VfsLogNg("AmbaKAL_GetSysTickCount failed", Err, 0U);
                                }
                                Err = AmbaCFS_ProcessEvent(pEvent->File, pEvent->Event);
                                if (Err != CODEC_OK) {
                                    VfsLogNg("AmbaCFS_ProcessEvent AMBA_CFS_EVENT_ASYNC_WRITE failed", Err, 0U);
                                }
                                Err = AmbaKAL_GetSysTickCount(&TEnd);
                                if (Err != OK) {
                                    VfsLogNg("AmbaKAL_GetSysTickCount failed %u", Err, 0U);
                                } else {
                                    if (RSize < (UINT32)VFS_BANK_SIZE) {
                                        RSize = BufferSizeUpdate(AsyncId, RSize, VFS_BANK_OP_MINUS);
                                        if (RSize == 0U) {
                                            /**/
                                        }
                                    } else {
                                        RSize = BufferSizeUpdate(AsyncId, (UINT32)VFS_BANK_SIZE, VFS_BANK_OP_MINUS);
                                        if (RSize == 0U) {
                                            /**/
                                        }
                                    }
                                }
                                if ((TEnd - TStart) > VFS_PROCESS_LIMIT_TIME) {
                                    AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaCFS_ProcessEvent too long %u", (TEnd - TStart), 0U, 0U, 0U, 0U);
                                }
                            } else {
                                Err = AmbaCFS_DiscardEvent(pEvent->File, pEvent->Event);
                                if (Err != OK) {
                                    VfsLogNg("AmbaCFS_DiscardEvent failed %u", Err, 0U);
                                }
                                AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "Discard one cfs event", 0U, 0U, 0U, 0U, 0U);
                            }
                            break;
                        case AMBA_CFS_EVENT_ASYNC_READ:
                            Err = AmbaCFS_ProcessEvent(pEvent->File, pEvent->Event);
                            if (Err != OK) {
                                VfsLogNg("AmbaCFS_ProcessEvent AMBA_CFS_EVENT_ASYNC_READ failed %u", Err, 0U);
                            }
                            break;
                        case AMBA_CFS_EVENT_ASYNC_CLOSE:
                            Err = AmbaCFS_ProcessEvent(pEvent->File, pEvent->Event);
                            if (Err != OK) {
                                VfsLogNg("AmbaCFS_ProcessEvent AMBA_CFS_EVENT_ASYNC_CLOSE failed %u", Err, 0U);
                            }

                            Err = AmbaKAL_EventFlagSet(&(Info.Async[AsyncId].Flag), VFS_ASYNC_FLG_CLOSE_DONE);
                            if (Err != OK) {
                                VfsLogNg("AmbaKAL_EventFlagSet failed %u", Err, 0U);
                            }
                            break;
                        default:
                            /* nothing */
                            break;
                    }
                }
            }
        }
    }

    return NULL;
}

/**
* Evaluate the memory size for AmbaVFS
* @param [out]  pSize the memory size needed
* @return ErrorCode
*/
UINT32 AmbaVFS_EvalMemSize(UINT32 *pSize)
{
    UINT32    Err, Rval = CODEC_OK, BufferSize = 0U;

    AmbaMisra_TouchUnused(&Rval);

    if (Rval == CODEC_OK) {
        Err = AmbaCFS_GetInitDefaultCfg(&(Info.Init));
        if (Err != OK) {
            VfsLogNg("AmbaCFS_GetInitDefaultCfg failed %u", Err, 0U);
            Rval = CODEC_ERROR_GENERAL_ERROR;
        } else {
            Info.Init.BankSize        = VFS_BANK_SIZE;
            Info.Init.CacheEnable     = 1U;
            Info.Init.CacheMaxFileNum = VFS_MAX_ASYNC_NUM;
            /* CmdAmount =  (BankPerStream * StreamAmount) + StreamAmount */
            Info.Init.CmdAmount       = ((UINT32)VFS_BANK_NUM * (UINT32)VFS_MAX_ASYNC_NUM) + (UINT32)VFS_MAX_ASYNC_NUM;
        }
    }

    if (Rval == CODEC_OK) {
        Err = AmbaCFS_GetInitBufferSize(&(Info.Init), &BufferSize);
        if (Err != OK) {
            VfsLogNg("AmbaCFS_GetInitBufferSize failed %u", Err, 0U);
            Rval = CODEC_ERROR_GENERAL_ERROR;
        } else {
            Info.Init.BufferSize = BufferSize;
        }
    }

    if (Rval == CODEC_OK) {
        BufferSize += ((UINT32)VFS_BANK_SIZE * (UINT32)VFS_BANK_NUM * (UINT32)VFS_MAX_ASYNC_NUM);
        BufferSize  = GetAlignedValU32(BufferSize, (UINT32)AMBA_CACHE_LINE_SIZE);
    }

    *pSize = BufferSize;

    return Rval;
}

/**
* Initialize AmbaVFS and create the task to handle CFS event
* @param [in]  pInit init config
* @return ErrorCode
*/
UINT32 AmbaVFS_Init(const AMBA_VFS_INIT_s *pInit)
{
    UINT32         Err, Rval = CODEC_OK, MinSize = 0U;
    static UINT32  IsInit = 0U;

    AmbaMisra_TouchUnused(&Rval);

    if (IsInit != 0U) {
        AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaVFS is already init", 0U, 0U, 0U, 0U, 0U);
    } else {
        if (Rval == CODEC_OK) {
            Err = AmbaWrap_memset(&Info, 0, sizeof(Info));
            if (Err != CODEC_OK) {
                VfsLogNg("AmbaWrap_memset failed %u", Err, 0U);
                Rval = CODEC_ERROR_GENERAL_ERROR;
            }
        }

        if (Rval == CODEC_OK) {
            Err = AmbaVFS_EvalMemSize(&MinSize);
            if (Err != CODEC_OK) {
                VfsLogNg("AmbaVFS_EvalMemSize failed %u", Err, 0U);
                Rval = CODEC_ERROR_GENERAL_ERROR;
            }
        }

        /* input check */
        if (Rval == CODEC_OK) {
            if (pInit->BufAddr == 0U) {
                VfsLogNg("Invalid BufAddr", 0U, 0U);
                Rval = CODEC_ERROR_ARG;
            }
            if (pInit->BufSize < MinSize) {
                VfsLogNg("BufSize %u should >= MinSize %u", pInit->BufSize, MinSize);
                Rval = CODEC_ERROR_ARG;
            }
        }

        /* init bank buffers */
        if (Rval == CODEC_OK) {
            ULONG i, Addr;

            AmbaMisra_TypeCast(&(Info.Init.Buffer), &(pInit->BufAddr));
            Addr = pInit->BufAddr + Info.Init.BufferSize;

            for (i = 0U; i < (UINT32)VFS_MAX_ASYNC_NUM; i++) {
                if (Addr >= (pInit->BufAddr + pInit->BufSize)) {
                    VfsLogNg("Addr %u is over the threshold", (UINT32)Addr, 0U);
                    Rval = CODEC_ERROR_ARG;
                    break;
                }

                Info.Async[i].BankAddr = Addr;
                Addr += ((UINT32)VFS_BANK_SIZE * (UINT32)VFS_BANK_NUM);
            }
        }

        /* create flag */
        if (Rval == PLAYER_OK) {
            UINT32      i;
            static char Name[] = "VfsAsync_Flag";

            for (i = 0U; i < (UINT32)VFS_MAX_ASYNC_NUM; i++) {
                Err = AmbaKAL_EventFlagCreate(&(Info.Async[i].Flag), Name);
                if (Err != KAL_ERR_NONE) {
                    VfsLogNg("AmbaKAL_EventFlagCreate failed (%u)", Err, 0U);
                    Rval = PLAYER_ERROR_GENERAL_ERROR;
                }
            }
        }

        if (Rval == CODEC_OK) {
            static char  Name[] = "Vfs_Mutex";

            Err = AmbaKAL_MutexCreate(&(Info.Mutex), Name);
            if (Err != OK) {
                VfsLogNg("AmbaKAL_MutexCreate failed %u", Err, 0U);
                Rval = CODEC_ERROR_GENERAL_ERROR;
            }
        }

        if (Rval == CODEC_OK) {
            static char  Name[] = "Vfs_AsyncMutex";

            Err = AmbaKAL_MutexCreate(&(Info.AsyncMutex), Name);
            if (Err != OK) {
                VfsLogNg("AmbaKAL_MutexCreate failed %u", Err, 0U);
                Rval = CODEC_ERROR_GENERAL_ERROR;
            }
        }

        /* init cfs */
        if (Rval == CODEC_OK) {
            Info.Init.CheckCached     = pInit->pCbCacheChk;
            Info.Init.CacheClean      = pInit->pCbCacheCln;
            Info.Init.CacheInvalidate = pInit->pCbCacheInv;
            Info.Init.VirtToPhys      = pInit->pCbVirToPhy;

            Err = AmbaCFS_Init(&(Info.Init));
            if (Err != OK) {
                VfsLogNg("AmbaCFS_Init failed %u", Err, 0U);
                Rval = CODEC_ERROR_GENERAL_ERROR;
            }
        }

        /* create cfs task */
        if (Rval == CODEC_OK) {
#define VFS_STACK_SIZE (0x10000U)
            static char  Name[] = "Vfs_Task";
            static UINT8 StackBuf[VFS_STACK_SIZE] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

            Err = AmbaKAL_TaskCreate(&(Info.Task), Name, pInit->TaskPriority, CfsTaskEntry,
                                     NULL, StackBuf, VFS_STACK_SIZE, 0U);
            if (Err != KAL_ERR_NONE) {
                VfsLogNg("AmbaKAL_TaskCreate failed %u", Err, 0U);
                Rval = CODEC_ERROR_GENERAL_ERROR;
            }

            if (Rval == CODEC_OK) {
                Err = AmbaKAL_TaskSetSmpAffinity(&(Info.Task), pInit->TaskCpuBits);
                if (Err != KAL_ERR_NONE) {
                    VfsLogNg("AmbaKAL_TaskSetSmpAffinity failed %u", Err, 0U);
                    Rval = CODEC_ERROR_GENERAL_ERROR;
                }
            }

            if (Rval == CODEC_OK) {
                Err = AmbaKAL_TaskResume(&(Info.Task));
                if (Err != KAL_ERR_NONE) {
                    VfsLogNg("AmbaKAL_TaskResume failed %u", Err, 0U);
                    Rval = CODEC_ERROR_GENERAL_ERROR;
                }
            }
        }

        if (Rval == CODEC_OK) {
            IsInit = 1U;
        }
    }

    return Rval;
}


/**
* register call back function
* @param [in]  Type
* @param [in]  CbFunc pointer to call back function
*/
void AmbaVFS_RegCbFunc(UINT32 Type, VfsCb CbFunc)
{
    if ((Type < AMBA_VFS_CB_TYPE_MAX) && (CbFunc != NULL)) {
        VfsCbFunc[Type] = CbFunc;
    }
}

/**
* file open
* @param [in]  pFileName file name
* @param [in]  pMode mode
* @param [in]  EnableAsync 0: disable, 1: enable (use bank buffer)
* @param [in]  pVfsFile pointer to svc vfs file
* @return ErrorCode
*/
UINT32 AmbaVFS_Open(const char *pFileName, const char *pMode, UINT32 EnableAsync, AMBA_VFS_FILE_s *pVfsFile)
{
    UINT32                 Err, Rval = CODEC_OK, i, AsyncId = VFS_MAX_ASYNC_NUM, MtxTake = 0U, RSize;
    AMBA_CFS_FILE_PARAM_s  FileParam;

    AmbaMisra_TouchUnused(&Rval);

    if (Rval == CODEC_OK) {
        if (pFileName == NULL) {
            VfsLogNg("pFileName is NULL", 0U, 0U);
            Rval = CODEC_ERROR_GENERAL_ERROR;
        }
        if (pMode == NULL) {
            VfsLogNg("pMode is NULL", 0U, 0U);
            Rval = CODEC_ERROR_GENERAL_ERROR;
        }
        if (pVfsFile == NULL) {
            VfsLogNg("pVfsFile is NULL", 0U, 0U);
            Rval = CODEC_ERROR_GENERAL_ERROR;
        }
    }

    if (Rval == CODEC_OK) {
        Err = AmbaKAL_MutexTake(&(Info.Mutex), 10000U);
        if (Err != OK) {
            VfsLogNg("AmbaVFS_Open MutexTake failed %u", Err, 0U);
            Rval = CODEC_ERROR_TIMEOUT;
        } else {
            MtxTake = 1U;
        }
    }

    if (Rval == CODEC_OK) {
        Err = AmbaWrap_memset(pVfsFile, 0, sizeof(AMBA_VFS_FILE_s));
        if (Err != OK) {
            VfsLogNg("AmbaWrap_memset failed %u", Err, 0U);
        }
        Err = AmbaWrap_memset(&FileParam, 0, sizeof(FileParam));
        if (Err != OK) {
            VfsLogNg("AmbaWrap_memset failed %u", Err, 0U);
        }
        Err = AmbaCFS_GetFileParam(&FileParam);
        if (Err != OK) {
            VfsLogNg("AmbaCFS_GetFileParam failed %u", Err, 0U);
            Rval = CODEC_ERROR_GENERAL_ERROR;
        }
    }

    if (Rval == CODEC_OK) {
        AmbaUtility_StringCopy(FileParam.Filename, AMBA_CFS_MAX_FILENAME_LENGTH, pFileName);
        pVfsFile->Drive = pFileName[0];

        if ((0 == AmbaUtility_StringCompare("r", pMode, AmbaUtility_StringLength(pMode)))) {
            FileParam.Mode = AMBA_CFS_FILE_MODE_READ_ONLY;
        } else if ((0 == AmbaUtility_StringCompare("r+", pMode, AmbaUtility_StringLength(pMode)))) {
            FileParam.Mode = AMBA_CFS_FILE_MODE_READ_WRITE;
        } else if ((0 == AmbaUtility_StringCompare("w", pMode, AmbaUtility_StringLength(pMode)))) {
            FileParam.Mode = AMBA_CFS_FILE_MODE_WRITE_ONLY;
        } else if ((0 == AmbaUtility_StringCompare("w+", pMode, AmbaUtility_StringLength(pMode)))) {
            FileParam.Mode = AMBA_CFS_FILE_MODE_WRITE_READ;
        } else if ((0 == AmbaUtility_StringCompare("a", pMode, AmbaUtility_StringLength(pMode)))) {
            FileParam.Mode = AMBA_CFS_FILE_MODE_APPEND_ONLY;
        } else if ((0 == AmbaUtility_StringCompare("a+", pMode, AmbaUtility_StringLength(pMode)))) {
            FileParam.Mode = AMBA_CFS_FILE_MODE_APPEND_READ;
        } else {
            VfsLogNg("invalid mode", 0U, 0U);
            AmbaPrint_ModulePrintStr5(AMBA_CODEC_PRINT_MODULE_ID, "%s", pMode, NULL, NULL, NULL, NULL);
            Rval = CODEC_ERROR_GENERAL_ERROR;
        }

        if (EnableAsync == 0U) {
            FileParam.AsyncEnable  = 0U;
        } else {
            for (i = 0U; i < (UINT32)VFS_MAX_ASYNC_NUM; i++) {
                if (Info.Async[i].IsUsed == 0U) {
                    AsyncId = i;
                    break;
                }
            }

            if (AsyncId < (UINT32)VFS_MAX_ASYNC_NUM) {
                FileParam.AsyncEnable = 1U;
            #if defined(CONFIG_AMBA_VFS_GDMA_USED)
                FileParam.DmaEnable = 1U;
            #else
                FileParam.DmaEnable = 0U;
            #endif
                FileParam.BankBufferSize  = (UINT32)VFS_BANK_SIZE * (UINT32)VFS_BANK_NUM;
                AmbaMisra_TypeCast(&(FileParam.BankBuffer), &(Info.Async[AsyncId].BankAddr));
            } else {
                VfsLogNg("no available async buffer now", 0U, 0U);
                Rval = CODEC_ERROR_GENERAL_ERROR;
            }
        }
    }

    AmbaMisra_TouchUnused(&pVfsFile);

    if ((Rval == CODEC_OK) && (pVfsFile != NULL)) {
        Err = CheckSdStatus(pVfsFile->Drive);
        if (Err != OK) {
            VfsLogNg("CheckSdStatus failed %u", Err, 0U);
            Rval = CODEC_ERROR_GENERAL_ERROR;
        }
    }

    if ((Rval == CODEC_OK) && (pVfsFile != NULL)) {
        Err = AmbaCFS_FileOpen(&FileParam, &(pVfsFile->pFile));
        if (Err != OK) {
            VfsLogNg("AmbaCFS_FileOpen failed %u", Err, 0U);
            Rval = CODEC_ERROR_GENERAL_ERROR;
        }
    }

    if ((Rval == CODEC_OK) && (pVfsFile != NULL) && (AsyncId < (UINT32)VFS_MAX_ASYNC_NUM)) {
        pVfsFile->AsyncId           = AsyncId;
        pVfsFile->AsyncEnabled      = EnableAsync;

        Info.Async[AsyncId].IsUsed        = 1U;
        Info.Async[AsyncId].pFile         = pVfsFile->pFile;

        RSize = BufferSizeUpdate(AsyncId, 0U, VFS_BANK_OP_SET);
        if (RSize != 0U) {
            VfsLogNg("BufferSizeUpdate VFS_BANK_OP_SET failed %u", RSize, 0U);
            Rval = CODEC_ERROR_GENERAL_ERROR;
        }

        if (Rval == PLAYER_OK) {
            Err = AmbaKAL_EventFlagClear(&(Info.Async[AsyncId].Flag), 0xFFFFFFFFU);
            if (Err != KAL_ERR_NONE) {
                VfsLogNg("AmbaKAL_EventFlagClear failed (%u)", Err, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }
        }
    }

    if (MtxTake == 1U) {
        Err = AmbaKAL_MutexGive(&(Info.Mutex));
        if (Err != OK) {
            VfsLogNg("AmbaVFS_Open MutexGive failed %u", Err, 0U);
            Rval = CODEC_ERROR_GENERAL_ERROR;
        }
    }

    return Rval;
}

/**
* file close
* @param [in]  pVfsFile pointer to svc vfs file
* @return ErrorCode
*/
UINT32 AmbaVFS_Close(const AMBA_VFS_FILE_s *pVfsFile)
{
    UINT32 Rval = CODEC_OK, Err, MtxTake = 0U;

    AmbaMisra_TouchUnused(&Rval);

    if (Rval == CODEC_OK) {
        Err = AmbaKAL_MutexTake(&(Info.Mutex), 10000U);
        if (Err != OK) {
            VfsLogNg("AmbaVFS_Close MutexTake failed %u", Err, 0U);
            Rval = CODEC_ERROR_TIMEOUT;
        } else {
            MtxTake = 1U;
        }
    }

    if (Rval == CODEC_OK) {
        Err = CheckSdStatus(pVfsFile->Drive);
        if (Err != OK) {
            VfsLogNg("CheckSdStatus failed %u", Err, 0U);
            Rval = CODEC_ERROR_GENERAL_ERROR;
        }
    }

    if (Rval == CODEC_OK) {
        Err = AmbaCFS_FileClose(pVfsFile->pFile);
        if (Err != OK) {
            VfsLogNg("AmbaCFS_FileClose failed %u", Err, 0U);
            Rval = CODEC_ERROR_GENERAL_ERROR;
        }
    }

    if (Rval == CODEC_OK) {
        if (pVfsFile->AsyncEnabled > 0U) {
            UINT32 ActualFlags = 0U;

            /* wait close done */
            Err = AmbaKAL_EventFlagGet(&(Info.Async[pVfsFile->AsyncId].Flag), VFS_ASYNC_FLG_CLOSE_DONE, AMBA_KAL_FLAGS_ALL,
                                        KAL_FLAGS_CLEAR_AUTO, &ActualFlags, 10000U);
            if (Err != OK) {
                VfsLogNg("AmbaKAL_EventFlagGet VFS_ASYNC_FLG_CLOSE_DONE failed %u", Err, 0U);
                Rval = CODEC_ERROR_GENERAL_ERROR;
            }

            Info.Async[pVfsFile->AsyncId].IsUsed = 0U;
            Info.Async[pVfsFile->AsyncId].pFile  = NULL;
        }
    }

    if (MtxTake == 1U) {
        Err = AmbaKAL_MutexGive(&(Info.Mutex));
        if (Err != OK) {
            VfsLogNg("AmbaVFS_Close MutexGive failed %u", Err, 0U);
            Rval = CODEC_ERROR_GENERAL_ERROR;
        }
    }

    return Rval;
}

/**
* file read
* @param [in]  pBuf pointer to the buffer
* @param [in]  Size read size
* @param [in]  Count read count
* @param [in]  pVfsFile pointer to svc vfs file
* @param [in]  pNumSuccess success number count
* @return ErrorCode
*/
UINT32 AmbaVFS_Read(void *pBuf, UINT32 Size, UINT32 Count, const AMBA_VFS_FILE_s *pVfsFile, UINT32 *pNumSuccess)
{
    UINT32 Rval = CODEC_OK;

    if (CheckSdStatus(pVfsFile->Drive) == CODEC_OK) {
        if (AMBA_FS_ERR_NONE != AmbaCFS_FileRead(pBuf, Size, Count, pVfsFile->pFile, pNumSuccess)) {
            VfsLogNg("AmbaCFS_FileRead failed", 0U, 0U);
            Rval = CODEC_ERROR_GENERAL_ERROR;
        }
    }
    return Rval;
}

/**
* file write
* @param [in]  pBuf pointer to the buffer
* @param [in]  Size write size
* @param [in]  Count write count
* @param [in]  pVfsFile pointer to svc vfs file
* @param [in]  pNumSuccess success number count
* @return ErrorCode
*/
UINT32 AmbaVFS_Write(void *pBuf, UINT32 Size, UINT32 Count, const AMBA_VFS_FILE_s *pVfsFile, UINT32 *pNumSuccess)
{
    UINT32  Rval = CODEC_OK, Err, RSize, WaitCount = 0U, ActualFlags;
    UINT64  WrSize;

    WrSize = (UINT64)Size * (UINT64)(Count);

    if (CheckSdStatus(pVfsFile->Drive) == CODEC_OK) {
        if ((pVfsFile->AsyncEnabled > 0U) && (pVfsFile->AsyncId < (UINT32)VFS_MAX_ASYNC_NUM)) {
            RSize = BufferSizeUpdate(pVfsFile->AsyncId, 0U, VFS_BANK_OP_GET);
            if ((UINT32)WrSize < ((UINT32)VFS_BANK_SIZE * (UINT32)VFS_MAX_ASYNC_NUM)) {
                while (((RSize + (UINT32)WrSize) > ((UINT32)VFS_BANK_SIZE * (UINT32)VFS_MAX_ASYNC_NUM)) && (RSize >= (UINT32)VFS_BANK_SIZE)) {
                    ActualFlags = 0U;

                    Err = AmbaKAL_EventFlagGet(&(Info.Async[pVfsFile->AsyncId].Flag), VFS_ASYNC_FLG_NULL_WRITE, AMBA_KAL_FLAGS_ALL,
                                                KAL_FLAGS_CLEAR_NONE, &ActualFlags, AMBA_KAL_NO_WAIT);
                    AmbaMisra_TouchUnused(&Err);

                    if ((ActualFlags & VFS_ASYNC_FLG_NULL_WRITE) == 0U) {
                        Err = AmbaKAL_TaskSleep(1U);
                        if (Err != OK) {
                            VfsLogNg("AmbaKAL_TaskSleep failed %u", Err, 0U);
                        }
                        RSize = BufferSizeUpdate(pVfsFile->AsyncId, 0U, VFS_BANK_OP_GET);
                        WaitCount++;
                    } else {
                        AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaVFS_Write force break", 0U, 0U, 0U, 0U, 0U);
                        Rval = CODEC_ERROR_GENERAL_ERROR;
                        break;
                    }

                    if (WaitCount > 1000UL) {
                        AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaVFS_Write wait for available bank over 1000ms", 0U, 0U, 0U, 0U, 0U);
                        WaitCount = 0U;
                    }
                }
            } else {
                while ((RSize + (UINT32)VFS_BANK_SIZE) > ((UINT32)VFS_BANK_SIZE * (UINT32)VFS_MAX_ASYNC_NUM)) {
                    ActualFlags = 0U;

                    Err = AmbaKAL_EventFlagGet(&(Info.Async[pVfsFile->AsyncId].Flag), VFS_ASYNC_FLG_NULL_WRITE, AMBA_KAL_FLAGS_ALL,
                                                KAL_FLAGS_CLEAR_NONE, &ActualFlags, AMBA_KAL_NO_WAIT);
                    AmbaMisra_TouchUnused(&Err);

                    if ((ActualFlags & VFS_ASYNC_FLG_NULL_WRITE) == 0U) {
                        Err = AmbaKAL_TaskSleep(1U);
                        if (Err != OK) {
                            VfsLogNg("AmbaKAL_TaskSleep failed %u", Err, 0U);
                        }
                        RSize = BufferSizeUpdate(pVfsFile->AsyncId, 0U, VFS_BANK_OP_GET);
                        WaitCount++;
                    } else {
                        AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaVFS_Write force break", 0U, 0U, 0U, 0U, 0U);
                        Rval = CODEC_ERROR_GENERAL_ERROR;
                        break;
                    }

                    if (WaitCount > 1000UL) {
                        AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaVFS_Write wait for available bank over 1000ms", 0U, 0U, 0U, 0U, 0U);
                        WaitCount = 0U;
                    }
                }
            }
        }

        if (Rval == CODEC_OK) {
            RSize = BufferSizeUpdate(pVfsFile->AsyncId, (UINT32)WrSize, VFS_BANK_OP_PLUS);
            if (RSize == 0U) {
                /**/
            }
            Err = AmbaCFS_FileWrite(pBuf, Size, Count, pVfsFile->pFile, pNumSuccess);
            if (AMBA_FS_ERR_NONE != Err) {
                VfsLogNg("AmbaCFS_FileWrite failed %u", Err, 0U);
                Rval = CODEC_ERROR_GENERAL_ERROR;
            } else {
                WrSize = (UINT64)Size * (UINT64)(*pNumSuccess);
            }
        }

        if (Rval == CODEC_OK) {
            if (VfsCbFunc[AMBA_VFS_CB_TYPE_WRITE] != NULL) {
                VfsCbFunc[AMBA_VFS_CB_TYPE_WRITE](pVfsFile, &WrSize);
            }
        }
    }

    return Rval;
}

/**
* file tell
* @param [in]  pVfsFile pointer to svc vfs file
* @param [out]  pFilePos current file position
* @return ErrorCode
*/
UINT32 AmbaVFS_Tell(const AMBA_VFS_FILE_s *pVfsFile, UINT64* pFilePos)
{
    UINT32 Rval = CODEC_OK;

    if (CheckSdStatus(pVfsFile->Drive) == CODEC_OK) {
        if (AMBA_FS_ERR_NONE != AmbaCFS_FileTell(pVfsFile->pFile, pFilePos)) {
            VfsLogNg("AmbaCFS_FileTell failed", 0U, 0U);
            Rval = CODEC_ERROR_GENERAL_ERROR;
        }
    }
    return Rval;
}

/**
* file seek
* @param [in]  pVfsFile pointer to svc vfs file
* @param [in]  Offset offset
* @param [in]  Origin origin
* @return ErrorCode
*/
UINT32 AmbaVFS_Seek(const AMBA_VFS_FILE_s *pVfsFile, INT64 Offset, INT32 Origin)
{
    UINT32 Rval = CODEC_OK;

    if (CheckSdStatus(pVfsFile->Drive) == CODEC_OK) {
        if (AMBA_FS_ERR_NONE != AmbaCFS_FileSeek(pVfsFile->pFile, Offset, Origin)) {
            VfsLogNg("AmbaCFS_FileSeek failed", 0U, 0U);
            Rval = CODEC_ERROR_GENERAL_ERROR;
        }
    }
    return Rval;
}

/**
* set position
* @param [in]  pVfsFile pointer to svc vfs file
* @param [in]  pFilePos pointer to file position
* @return ErrorCode
*/
UINT32 AmbaVFS_SetPos(const AMBA_VFS_FILE_s *pVfsFile, const UINT64 *pFilePos)
{
    UINT32 Rval = CODEC_OK;
    INT64  Offset = (INT64)*pFilePos;

    if (CheckSdStatus(pVfsFile->Drive) == CODEC_OK) {
        if (AMBA_FS_ERR_NONE != AmbaCFS_FileSeek(pVfsFile->pFile, Offset, AMBA_CFS_SEEK_START)) {
            VfsLogNg("AmbaCFS_FileSeek failed", 0U, 0U);
            Rval = CODEC_ERROR_GENERAL_ERROR;
        }
    }
    return Rval;
}

/**
* get position
* @param [in]  pVfsFile pointer to svc vfs file
* @param [in]  pFilePos pointer to file position
* @return ErrorCode
*/
UINT32 AmbaVFS_GetPos(const AMBA_VFS_FILE_s *pVfsFile, UINT64 *pFilePos)
{
    UINT32 Rval = CODEC_OK;

    if (CheckSdStatus(pVfsFile->Drive) == CODEC_OK) {
        if (AMBA_FS_ERR_NONE != AmbaCFS_FileTell(pVfsFile->pFile, pFilePos)) {
            VfsLogNg("AmbaCFS_FileTell failed", 0U, 0U);
            Rval = CODEC_ERROR_GENERAL_ERROR;
        }
    }
    return Rval;
}

/**
* file eof
* @param [in]  pVfsFile pointer to svc vfs file
* @param [out]  IsEof 1U: end of file, 0U: not end of file, >1U: error
* @return ErrorCode
*/
UINT32 AmbaVFS_Eof(const AMBA_VFS_FILE_s* pVfsFile, UINT32* IsEof)
{
    UINT32 Rval = CODEC_OK;

    if (CheckSdStatus(pVfsFile->Drive) == CODEC_OK) {
        *IsEof = AmbaCFS_FileEof(pVfsFile->pFile);
        if (*IsEof > 1U) {
            VfsLogNg("AmbaCFS_FileEof failed %u", *IsEof, 0U);
            Rval = CODEC_ERROR_GENERAL_ERROR;
        }
    }
    return Rval;
}

/**
* file sync
* @param [in]  pVfsFile pointer to svc vfs file
* @return ErrorCode
*/
UINT32 AmbaVFS_Sync(const AMBA_VFS_FILE_s* pVfsFile)
{
    UINT32 Rval = CODEC_OK;

    if (CheckSdStatus(pVfsFile->Drive) == CODEC_OK) {
        if (AMBA_FS_ERR_NONE != AmbaCFS_FileSync(pVfsFile->pFile)) {
            VfsLogNg("AmbaCFS_FileSync failed", 0U, 0U);
            Rval = CODEC_ERROR_GENERAL_ERROR;
        }
    }
    return Rval;
}

UINT32 AmbaVFS_NullWrite(const AMBA_VFS_FILE_s* pVfsFile)
{
    UINT32 Rval = CODEC_OK, Err;

    if ((pVfsFile->AsyncEnabled > 0U) && (pVfsFile->AsyncId < (UINT32)VFS_MAX_ASYNC_NUM)) {
        Err = AmbaKAL_EventFlagSet(&(Info.Async[pVfsFile->AsyncId].Flag), VFS_ASYNC_FLG_NULL_WRITE);
        if (Err != OK) {
            VfsLogNg("AmbaKAL_EventFlagSet failed %u", Err, 0U);
            Rval = CODEC_ERROR_GENERAL_ERROR;
        }
    }
    return Rval;
}
