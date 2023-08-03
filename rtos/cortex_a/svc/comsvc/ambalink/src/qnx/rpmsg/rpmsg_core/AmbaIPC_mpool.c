/**
 * @file AmbaLink.c
 *
 * Copyright (c) 2020 Ambarella International LP
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
 * @details The module of Ambarella Network Support
 *
 */

#include "AmbaIntrinsics.h"
#include "AmbaKAL.h"
#include "AmbaLinkPrivate.h"
#include "AmbaPrint.h"
#include "AmbaNAND.h"
#include "AmbaLink.h"
#include "AmbaLink_core.h"

#ifdef CONFIG_AMBALINK_BOOT_OS
#include "AmbaRTSL_MMU.h"
#include "AmbaCache.h"
#ifdef CONFIG_CPU_CORTEX_A53
#include "AmbaFS.h"
#endif
#include "AmbaSD.h"
#include "AmbaSD_STD.h"
#include "AmbaSD_Ctrl.h"
#include "AmbaNVM_Ctrl.h"
#include "AmbaSYS.h"
#include "AmbaMMU.h"
#include "AmbaCSL_RCT.h"
#include "AmbaUtility.h"
#include "AmbaIPC_List.h"
#include "AmbaIPC_Rpc.h"
#endif
#if defined(CONFIG_AMBALINK_RPMSG_HIBER)
#include <AmbaIPC_LinkCtrl.h>
#include "AmbaIPC_Hiber.h"
#endif

//#define ALLOC_DEBUG_MSG   1
UINT32 AmbaLinkBootType = 0U;
UINT64 LinuxKernelStart = 0x0;
AMBA_KAL_EVENT_FLAG_t AmbaLinkEventFlag;

static AMBA_KAL_MUTEX_t AmbaLinkMemoryPoolMutex;

/** Memory Pool List**/
#define MEM_POOL_NAND_TMP_BUF_NUM   1
#define MEM_POOL_NAND_TMP_BUF_SIZE  0x1000
static UINT8 MemoryPoolNandTmpPtr[MEM_POOL_NAND_TMP_BUF_NUM*MEM_POOL_NAND_TMP_BUF_SIZE];
static UINT8 MemoryPoolNandTmpIndex[MEM_POOL_NAND_TMP_BUF_NUM];

#define MEM_POOL_EMMC_TMP_BUF_NUM   1
#define MEM_POOL_EMMC_TMP_BUF_SIZE  0x1000
static UINT8 MemoryPoolEmmcTmpPtr[MEM_POOL_EMMC_TMP_BUF_NUM*MEM_POOL_EMMC_TMP_BUF_SIZE];
static UINT8 MemoryPoolEmmcTmpIndex[MEM_POOL_EMMC_TMP_BUF_NUM];

#define MEM_POOL_SPI_NOR_TMP_BUF_NUM   1
#define MEM_POOL_SPI_NOR_TMP_BUF_SIZE  0x1000
static UINT8 MemoryPoolSpiNorTmpPtr[MEM_POOL_SPI_NOR_TMP_BUF_NUM*MEM_POOL_SPI_NOR_TMP_BUF_SIZE];
static UINT8 MemoryPoolSpiNorTmpIndex[MEM_POOL_SPI_NOR_TMP_BUF_NUM];

#define MEM_POOL_CRC_BUF_NUM   1
#define MEM_POOL_CRC_BUF_SIZE  0x8000
static UINT8 MemoryPoolCrcPtr[MEM_POOL_CRC_BUF_NUM*MEM_POOL_CRC_BUF_SIZE];
static UINT8 MemoryPoolCrcIndex[MEM_POOL_CRC_BUF_NUM];

#define MEM_POOL_VRING_BUF_NUM   1
#define MEM_POOL_VRING_BUF_SIZE  0x8000
static UINT8 MemoryPoolVringPtr[MEM_POOL_VRING_BUF_NUM*MEM_POOL_VRING_BUF_SIZE];
static UINT8 MemoryPoolVringIndex[MEM_POOL_VRING_BUF_NUM];

#define MEM_POOL_RPC_BUF_NUM   8
#define MEM_POOL_RPC_BUF_SIZE  0x1000
static UINT8 MemoryPoolRpcPtr[MEM_POOL_RPC_BUF_NUM*MEM_POOL_RPC_BUF_SIZE];
static UINT8 MemoryPoolRpcIndex[MEM_POOL_RPC_BUF_NUM];

#define MEM_POOL_COMM_BUF_NUM   1
#define MEM_POOL_COMM_BUF_SIZE  0x1000
static UINT8 MemoryPoolCommPtr[MEM_POOL_COMM_BUF_NUM*MEM_POOL_COMM_BUF_SIZE];
static UINT8 MemoryPoolCommIndex[MEM_POOL_COMM_BUF_NUM];

#define MEM_POOL_FS_DTA_BUF_NUM   4
#define MEM_POOL_FS_DTA_BUF_SIZE  0x800
static UINT8 MemoryPoolFsDtaPtr[MEM_POOL_FS_DTA_BUF_NUM*MEM_POOL_FS_DTA_BUF_SIZE];
static UINT8 MemoryPoolFsDtaIndex[MEM_POOL_FS_DTA_BUF_NUM];

#define MEM_POOL_SVC_BUF_NUM   10
#define MEM_POOL_SVC_BUF_SIZE  0x1000
static UINT8 MemoryPoolSvcPtr[MEM_POOL_SVC_BUF_NUM*MEM_POOL_SVC_BUF_SIZE];
static UINT8 MemoryPoolSvcIndex[MEM_POOL_SVC_BUF_NUM];

#define MEM_POOL_PROC_INFO_BUF_NUM   8
#define MEM_POOL_PROC_INFO_BUF_SIZE  0x100
static UINT8 MemoryPoolProcInfoPtr[MEM_POOL_PROC_INFO_BUF_NUM*MEM_POOL_PROC_INFO_BUF_SIZE];
static UINT8 MemoryPoolProcInfoIndex[MEM_POOL_PROC_INFO_BUF_NUM];

#define MEM_POOL_TPOOL_BUF_NUM   4
#define MEM_POOL_TPOOL_BUF_SIZE  0x100
static UINT8 MemoryPoolTPoolPtr[MEM_POOL_TPOOL_BUF_NUM*MEM_POOL_TPOOL_BUF_SIZE];
static UINT8 MemoryPoolTPoolIndex[MEM_POOL_TPOOL_BUF_NUM];

#define MEM_POOL_TPOOL_THREAD_BUF_NUM   4
#define MEM_POOL_TPOOL_THREAD_BUF_SIZE  0xC00
static UINT8 MemoryPoolTPoolThreadPtr[MEM_POOL_TPOOL_THREAD_BUF_NUM*MEM_POOL_TPOOL_THREAD_BUF_SIZE];
static UINT8 MemoryPoolTPoolThreadIndex[MEM_POOL_TPOOL_THREAD_BUF_NUM];

#define MEM_POOL_TPOOL_QUEUE_BUF_NUM   4
#define MEM_POOL_TPOOL_QUEUE_BUF_SIZE  0x400
static UINT8 MemoryPoolTPoolQueuePtr[MEM_POOL_TPOOL_QUEUE_BUF_NUM*MEM_POOL_TPOOL_QUEUE_BUF_SIZE];
static UINT8 MemoryPoolTPoolQueueIndex[MEM_POOL_TPOOL_QUEUE_BUF_NUM];

#define MEM_POOL_TPOOL_STACK_BUF_NUM   4
#define MEM_POOL_TPOOL_STACK_BUF_SIZE  0x4000
static UINT8 MemoryPoolTPoolStackPtr[MEM_POOL_TPOOL_STACK_BUF_NUM*MEM_POOL_TPOOL_STACK_BUF_SIZE];
static UINT8 MemoryPoolTPoolStackIndex[MEM_POOL_TPOOL_STACK_BUF_NUM];

#define MEM_POOL_CLIENT_BUF_NUM   4
#define MEM_POOL_CLIENT_BUF_SIZE  0x1000
static UINT8 MemoryPoolClientPtr[MEM_POOL_CLIENT_BUF_NUM*MEM_POOL_CLIENT_BUF_SIZE];
static UINT8 MemoryPoolClientIndex[MEM_POOL_CLIENT_BUF_NUM];

#define MEM_POOL_CLIENT_MODE_BUF_NUM   4
#define MEM_POOL_CLIENT_MODE_BUF_SIZE  0x100
static UINT8 MemoryPoolClientModePtr[MEM_POOL_CLIENT_MODE_BUF_NUM*MEM_POOL_CLIENT_MODE_BUF_SIZE];
static UINT8 MemoryPoolClientModeIndex[MEM_POOL_CLIENT_MODE_BUF_NUM];

#define MEM_POOL_TIMEOUT_RECORD_BUF_NUM   4
#define MEM_POOL_TIMEOUT_RECORD_BUF_SIZE  0x100
static UINT8 MemoryPoolTimeoutRecordPtr[MEM_POOL_TIMEOUT_RECORD_BUF_NUM*MEM_POOL_TIMEOUT_RECORD_BUF_SIZE];
static UINT8 MemoryPoolTimeoutRecordIndex[MEM_POOL_TIMEOUT_RECORD_BUF_NUM];

/* 64 rpmsg channels */
#define MEM_POOL_IPC_BUF_NUM   64
#define MEM_POOL_IPC_BUF_SIZE  0x80
static UINT8 MemoryPoolIpcPtr[MEM_POOL_IPC_BUF_NUM*MEM_POOL_IPC_BUF_SIZE];
static UINT8 MemoryPoolIpcIndex[MEM_POOL_IPC_BUF_NUM];

#ifdef CONFIG_OPENAMP
#define MEM_POOL_LIBMETAL_BUF_NUM   32
#define MEM_POOL_LIBMETAL_BUF_SIZE  0x2100
static UINT8 MemoryPoolLibmetalPtr[MEM_POOL_LIBMETAL_BUF_NUM*MEM_POOL_LIBMETAL_BUF_SIZE];
static UINT8 MemoryPoolLibmetalIndex[MEM_POOL_LIBMETAL_BUF_NUM];
#endif // CONFIG_OPENAMP

static AMBA_LINK_MEM_POOL_s AmbaLinkMemPool[MEM_POOL_MAX] = {
    [MEM_POOL_NAND_TMP_BUF] = {
        .Number     = MEM_POOL_NAND_TMP_BUF_NUM,
        .Size       = MEM_POOL_NAND_TMP_BUF_SIZE,
        .pPtr       = &MemoryPoolNandTmpPtr[0],
        .pIndex     = &MemoryPoolNandTmpIndex[0],
    },
    [MEM_POOL_EMMC_TMP_BUF] = {
        .Number     = MEM_POOL_EMMC_TMP_BUF_NUM,
        .Size       = MEM_POOL_EMMC_TMP_BUF_SIZE,
        .pPtr       = &MemoryPoolEmmcTmpPtr[0],
        .pIndex     = &MemoryPoolEmmcTmpIndex[0],
    },
    [MEM_POOL_SPI_NOR_TMP_BUF] = {
        .Number     = MEM_POOL_SPI_NOR_TMP_BUF_NUM,
        .Size       = MEM_POOL_SPI_NOR_TMP_BUF_SIZE,
        .pPtr       = &MemoryPoolSpiNorTmpPtr[0],
        .pIndex     = &MemoryPoolSpiNorTmpIndex[0],
    },
    [MEM_POOL_CRC_BUF] = {
        .Number     = MEM_POOL_CRC_BUF_NUM,
        .Size       = MEM_POOL_CRC_BUF_SIZE,
        .pPtr       = &MemoryPoolCrcPtr[0],
        .pIndex     = &MemoryPoolCrcIndex[0],
    },
    [MEM_POOL_VRING_BUF] = {
        .Number     = MEM_POOL_VRING_BUF_NUM,
        .Size       = MEM_POOL_VRING_BUF_SIZE,
        .pPtr       = &MemoryPoolVringPtr[0],
        .pIndex     = &MemoryPoolVringIndex[0],
    },
    [MEM_POOL_RPC_BUF] = {
        .Number     = MEM_POOL_RPC_BUF_NUM,
        .Size       = MEM_POOL_RPC_BUF_SIZE,
        .pPtr       = &MemoryPoolRpcPtr[0],
        .pIndex     = &MemoryPoolRpcIndex[0],
    },
    [MEM_POOL_COMM_BUF] = {
        .Number     = MEM_POOL_COMM_BUF_NUM,
        .Size       = MEM_POOL_COMM_BUF_SIZE,
        .pPtr       = &MemoryPoolCommPtr[0],
        .pIndex     = &MemoryPoolCommIndex[0],
    },
    [MEM_POOL_FS_DTA_BUF] = {
        .Number     = MEM_POOL_FS_DTA_BUF_NUM,
        .Size       = MEM_POOL_FS_DTA_BUF_SIZE,
        .pPtr       = &MemoryPoolFsDtaPtr[0],
        .pIndex     = &MemoryPoolFsDtaIndex[0],
    },
    [MEM_POOL_SVC_BUF] = {
        .Number     = MEM_POOL_SVC_BUF_NUM,
        .Size       = MEM_POOL_SVC_BUF_SIZE,
        .pPtr       = &MemoryPoolSvcPtr[0],
        .pIndex     = &MemoryPoolSvcIndex[0],
    },
    [MEM_POOL_PROC_INFO_BUF] = {
        .Number     = MEM_POOL_PROC_INFO_BUF_NUM,
        .Size       = MEM_POOL_PROC_INFO_BUF_SIZE,
        .pPtr       = &MemoryPoolProcInfoPtr[0],
        .pIndex     = &MemoryPoolProcInfoIndex[0],
    },
    [MEM_POOL_TPOOL_BUF] = {
        .Number     = MEM_POOL_TPOOL_BUF_NUM,
        .Size       = MEM_POOL_TPOOL_BUF_SIZE,
        .pPtr       = &MemoryPoolTPoolPtr[0],
        .pIndex     = &MemoryPoolTPoolIndex[0],
    },
    [MEM_POOL_TPOOL_THREAD_BUF] = {
        .Number     = MEM_POOL_TPOOL_THREAD_BUF_NUM,
        .Size       = MEM_POOL_TPOOL_THREAD_BUF_SIZE,
        .pPtr       = &MemoryPoolTPoolThreadPtr[0],
        .pIndex     = &MemoryPoolTPoolThreadIndex[0],
    },
    [MEM_POOL_TPOOL_QUEUE_BUF] = {
        .Number     = MEM_POOL_TPOOL_QUEUE_BUF_NUM,
        .Size       = MEM_POOL_TPOOL_QUEUE_BUF_SIZE,
        .pPtr       = &MemoryPoolTPoolQueuePtr[0],
        .pIndex     = &MemoryPoolTPoolQueueIndex[0],

    },
    [MEM_POOL_TPOOL_STACK_BUF] = {
        .Number     = MEM_POOL_TPOOL_STACK_BUF_NUM,
        .Size       = MEM_POOL_TPOOL_STACK_BUF_SIZE,
        .pPtr       = &MemoryPoolTPoolStackPtr[0],
        .pIndex     = &MemoryPoolTPoolStackIndex[0],
    },
    [MEM_POOL_CLIENT_BUF] = {
        .Number     = MEM_POOL_CLIENT_BUF_NUM,
        .Size       = MEM_POOL_CLIENT_BUF_SIZE,
        .pPtr       = &MemoryPoolClientPtr[0],
        .pIndex     = &MemoryPoolClientIndex[0],
    },
    [MEM_POOL_CLIENT_MODE_BUF] = {
        .Number     = MEM_POOL_CLIENT_MODE_BUF_NUM,
        .Size       = MEM_POOL_CLIENT_MODE_BUF_SIZE,
        .pPtr       = &MemoryPoolClientModePtr[0],
        .pIndex     = &MemoryPoolClientModeIndex[0],
    },
    [MEM_POOL_TIMEOUT_RECORD_BUF] = {
        .Number     = MEM_POOL_TIMEOUT_RECORD_BUF_NUM,
        .Size       = MEM_POOL_TIMEOUT_RECORD_BUF_SIZE,
        .pPtr       = &MemoryPoolTimeoutRecordPtr[0],
        .pIndex     = &MemoryPoolTimeoutRecordIndex[0],
    },
    [MEM_POOL_IPC_BUF] = {
        .Number     = MEM_POOL_IPC_BUF_NUM,
        .Size       = MEM_POOL_IPC_BUF_SIZE,
        .pPtr       = &MemoryPoolIpcPtr[0],
        .pIndex     = &MemoryPoolIpcIndex[0],
    },
#ifdef CONFIG_OPENAMP
    [MEM_POOL_LIBMETAL_BUF] = {
        .Number     = MEM_POOL_LIBMETAL_BUF_NUM,
        .Size       = MEM_POOL_LIBMETAL_BUF_SIZE,
        .pPtr       = &MemoryPoolLibmetalPtr[0],
        .pIndex     = &MemoryPoolLibmetalIndex[0],
    },
#endif // CONFIG_OPENAMP
};


/*----------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaLink_Free
 *
 *  @Description:: Free the memory.
 *
 *  @Input      ::
 *      pPtr:   Point to the memory buffer to be freed.
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*----------------------------------------------------------------------------*/
void AmbaLink_Free(UINT32 Num, void *pPtr)
{
    UINT32 i,Find = 0U;
    AMBA_LINK_MEM_POOL_s tmp;

    /* [MISRA Rule 8.13] could be declared as pointing to const */
    (void)pPtr;

#ifdef ALLOC_DEBUG_MSG
    AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Num(%d) pPtr(%x)", Num, (UINT32)pPtr, 0U, 0U, 0U);
    AmbaPrint_Flush();
#endif

    if(Num >= MEM_POOL_MAX) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Num(%d) is bigger than MEM_POOL_MAX(%d)", Num, MEM_POOL_MAX, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }
    else {
        (void)AmbaKAL_MutexTake(&AmbaLinkMemoryPoolMutex, AMBA_KAL_WAIT_FOREVER);
        tmp.Number = AmbaLinkMemPool[Num].Number;
        tmp.Size = AmbaLinkMemPool[Num].Size;
        tmp.pPtr = AmbaLinkMemPool[Num].pPtr;
        tmp.pIndex = AmbaLinkMemPool[Num].pIndex;
        for(i = 0U; i < tmp.Number; i++) {
            if(&(tmp.pPtr[i*tmp.Size]) == pPtr) {
                tmp.pIndex[i] = 0U;
                Find = 1U;
                break;
            }
        }
        (void)AmbaKAL_MutexGive(&AmbaLinkMemoryPoolMutex);

        if(Find == 0U) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s error", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
        }
    }
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaLink_Malloc
 *
 *  @Description:: AmbaLink memory allocation.
 *
 *  @Input      ::
 *      Size:   memory request size
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      void *: pointer to allocated memory buffer
\*----------------------------------------------------------------------------*/
void *AmbaLink_Malloc(UINT32 Num, UINT32 Size)
{
    UINT32 i,Find = 0;
    AMBA_LINK_MEM_POOL_s tmp;
    void *pPtr = NULL;

    if(Num >= MEM_POOL_MAX) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Num(%d) is bigger than MEM_POOL_MAX(%d)", Num, MEM_POOL_MAX, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }
    else {
        if(Size > AmbaLinkMemPool[Num].Size) {
                AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Size(%d) is bigger than AmbaLinkMemPool[%d].Size(%d) ",Size, Num, AmbaLinkMemPool[Num].Size, 0U, 0U);
                AmbaPrint_Flush();
        }

        (void)AmbaKAL_MutexTake(&AmbaLinkMemoryPoolMutex, AMBA_KAL_WAIT_FOREVER);
        tmp.Number = AmbaLinkMemPool[Num].Number;
        tmp.Size = AmbaLinkMemPool[Num].Size;
        tmp.pPtr = AmbaLinkMemPool[Num].pPtr;
        tmp.pIndex = AmbaLinkMemPool[Num].pIndex;

        for(i = 0U; i < tmp.Number; i++) {
            if(tmp.pIndex[i] == 0U) {
                pPtr = &(tmp.pPtr[i*tmp.Size]);
                tmp.pIndex[i] = 1U;
                if (AmbaWrap_memset(pPtr, 0, Size)!= 0U) { }
                Find = 1;
                break;
            }
        }
        (void)AmbaKAL_MutexGive(&AmbaLinkMemoryPoolMutex);

        if(Find == 0U) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Num(%d) Malloc fail", Num, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }

#ifdef ALLOC_DEBUG_MSG
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Num(%d) Size(%d) Max Size(%d) pPtr(%x)", Num, Size, tmp.Size, (UINT32)pPtr, 0U);
        AmbaPrint_Flush();
#endif
    }

    return pPtr;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaLink_MemPool_Init
 *
 *  @Description:: AmbaLink memory init.
 *
 *  @Input      ::
 *      Size:
 *
 *  @Output     ::
 *
 *  @Return     ::
 *      INT32:
\*----------------------------------------------------------------------------*/
//static INT32 AmbaLink_MemPool_Init(void)
INT32 AmbaLink_MemPool_Init(void)
{
    UINT32 i;
    UINT32 Status;
    static char Name[16] = "AmbaLinkPool";
    static UINT8 AmbaLinkMemPoolInit = 0;

    if(AmbaLinkMemPoolInit != 1U) {
        Status = AmbaKAL_MutexCreate(&AmbaLinkMemoryPoolMutex, Name);
        if (Status == 0U) {
            (void)AmbaKAL_MutexTake(&AmbaLinkMemoryPoolMutex, AMBA_KAL_WAIT_FOREVER);
            for(i = 0U; i < MEM_POOL_MAX; i++) {
                if (AmbaWrap_memset(AmbaLinkMemPool[i].pIndex, 0, AmbaLinkMemPool[i].Number)!= 0U) { }
            }
            (void)AmbaKAL_MutexGive(&AmbaLinkMemoryPoolMutex);
            AmbaLinkMemPoolInit = 1U;
        }
    }

    return 0;
}

