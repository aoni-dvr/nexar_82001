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
#include "AmbaWrap.h"
#include "AmbaSvcWrap.h"

#ifdef CONFIG_AMBALINK_BOOT_OS
#include "AmbaRTSL_MMU.h"
#include "AmbaCache.h"
#include "AmbaFS.h"
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
#include "AmbaRTSL_GIC.h"
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
#define MEM_POOL_SVC_BUF_SIZE  0x2000
static UINT8 MemoryPoolSvcPtr[MEM_POOL_SVC_BUF_NUM*MEM_POOL_SVC_BUF_SIZE];
static UINT8 MemoryPoolSvcIndex[MEM_POOL_SVC_BUF_NUM];

#define MEM_POOL_PROC_INFO_BUF_NUM   8
#define MEM_POOL_PROC_INFO_BUF_SIZE  0x100
static UINT8 MemoryPoolProcInfoPtr[MEM_POOL_PROC_INFO_BUF_NUM*MEM_POOL_PROC_INFO_BUF_SIZE];
static UINT8 MemoryPoolProcInfoIndex[MEM_POOL_PROC_INFO_BUF_NUM];

#define MEM_POOL_TPOOL_BUF_NUM   4
#define MEM_POOL_TPOOL_BUF_SIZE  0x200
static UINT8 MemoryPoolTPoolPtr[MEM_POOL_TPOOL_BUF_NUM*MEM_POOL_TPOOL_BUF_SIZE];
static UINT8 MemoryPoolTPoolIndex[MEM_POOL_TPOOL_BUF_NUM];

#define MEM_POOL_TPOOL_THREAD_BUF_NUM   4
#define MEM_POOL_TPOOL_THREAD_BUF_SIZE  0x1200
static UINT8 MemoryPoolTPoolThreadPtr[MEM_POOL_TPOOL_THREAD_BUF_NUM*MEM_POOL_TPOOL_THREAD_BUF_SIZE];
static UINT8 MemoryPoolTPoolThreadIndex[MEM_POOL_TPOOL_THREAD_BUF_NUM];

#define MEM_POOL_TPOOL_QUEUE_BUF_NUM   4
#define MEM_POOL_TPOOL_QUEUE_BUF_SIZE  0x800
static UINT8 MemoryPoolTPoolQueuePtr[MEM_POOL_TPOOL_QUEUE_BUF_NUM*MEM_POOL_TPOOL_QUEUE_BUF_SIZE];
static UINT8 MemoryPoolTPoolQueueIndex[MEM_POOL_TPOOL_QUEUE_BUF_NUM];

#define MEM_POOL_TPOOL_STACK_BUF_NUM   4
#define MEM_POOL_TPOOL_STACK_BUF_SIZE  0x4000
static UINT8 MemoryPoolTPoolStackPtr[MEM_POOL_TPOOL_STACK_BUF_NUM*MEM_POOL_TPOOL_STACK_BUF_SIZE];
static UINT8 MemoryPoolTPoolStackIndex[MEM_POOL_TPOOL_STACK_BUF_NUM];

#define MEM_POOL_CLIENT_BUF_NUM   8
#define MEM_POOL_CLIENT_BUF_SIZE  0x1000
static UINT8 MemoryPoolClientPtr[MEM_POOL_CLIENT_BUF_NUM*MEM_POOL_CLIENT_BUF_SIZE];
static UINT8 MemoryPoolClientIndex[MEM_POOL_CLIENT_BUF_NUM];

#define MEM_POOL_CLIENT_MODE_BUF_NUM   8
#define MEM_POOL_CLIENT_MODE_BUF_SIZE  0x100
static UINT8 MemoryPoolClientModePtr[MEM_POOL_CLIENT_MODE_BUF_NUM*MEM_POOL_CLIENT_MODE_BUF_SIZE];
static UINT8 MemoryPoolClientModeIndex[MEM_POOL_CLIENT_MODE_BUF_NUM];

#define MEM_POOL_TIMEOUT_RECORD_BUF_NUM   8
#define MEM_POOL_TIMEOUT_RECORD_BUF_SIZE  0x100
static UINT8 MemoryPoolTimeoutRecordPtr[MEM_POOL_TIMEOUT_RECORD_BUF_NUM*MEM_POOL_TIMEOUT_RECORD_BUF_SIZE];
static UINT8 MemoryPoolTimeoutRecordIndex[MEM_POOL_TIMEOUT_RECORD_BUF_NUM];

/* 64 rpmsg channels */
#define MEM_POOL_IPC_BUF_NUM   64
#define MEM_POOL_IPC_BUF_SIZE  0x100
static UINT8 MemoryPoolIpcPtr[MEM_POOL_IPC_BUF_NUM*MEM_POOL_IPC_BUF_SIZE];
static UINT8 MemoryPoolIpcIndex[MEM_POOL_IPC_BUF_NUM];

#ifdef CONFIG_OPENAMP
#define MEM_POOL_LIBMETAL_BUF_NUM   32
#ifdef CONFIG_AMBALINK_RPMSG_ROLE_MAST
/* cf. virtqueue_allocate() in lib/include/openamp/virtqueue.h of open-amp */
#define MEM_POOL_LIBMETAL_BUF_SIZE  0x4100
#else
#define MEM_POOL_LIBMETAL_BUF_SIZE  0x2100
#endif
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

#ifdef CONFIG_AMBALINK_BOOT_OS
extern UINT32 secure_monitor_call(UINT32 Param0, UINT32 Param1, UINT32 Param2, UINT32 Param3);

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaLink_IsWarmBoot
 *
 *  @Description:: Check if it is Warm Boot
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      INT32 : 1 - Yes, it is Warm-Boot; 0 - No, it is not Warm-Boot
\*-----------------------------------------------------------------------------------------------*/
static INT32 IsWarmBoot = 0;
static INT32 AmbaLink_IsWarmBoot(void)
{
    extern UINT32 AmbaRTSL_CpuReadSysCtrl(void);
    UINT32 SysCtrlReg = AmbaRTSL_CpuReadSysCtrl();
    UINT32 *pWorkRom, *pWorkRam;
    INT32 v;

    pWorkRam = (SysCtrlReg & 0x1) ? (UINT32 *)AMBA_RAM_MAGIC_CODE_VIRT_ADDR : (UINT32 *)AMBA_RAM_MAGIC_CODE_PHYS_ADDR;
    pWorkRom = (SysCtrlReg & 0x1) ? (UINT32 *)AMBA_ROM_MAGIC_CODE_VIRT_ADDR : (UINT32 *)AMBA_ROM_MAGIC_CODE_PHYS_ADDR;

    if (IsWarmBoot != -1 )
        return IsWarmBoot;

    if (*pWorkRam == 0xffffffffU) {
        IsWarmBoot = 0;   /* no, it is not Warm Boot up */
        return 0;
    }
    AmbaWrap_memcmp((void *) pWorkRam,
               (void *) pWorkRom,
               AMBA_MAGIC_CODE_SIZE, &v);
    /* check the Magic Code */
    if (v) {
        IsWarmBoot = 0;
        return 0;   /* no, it is not Warm Boot up */
    }

    IsWarmBoot = 1; /* yes, it is Warm Boot up */
    return 1;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaLink_BootLinuxCore
 *
 *  @Description:: Boot Linux core
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *
\*-----------------------------------------------------------------------------------------------*/
void AmbaLink_BootLinuxCore(void)
{
    ULONG EntryPoint = 0x0U;
    ULONG core = CONFIG_BOOT_CORE_LINUX;

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    /*cv5x core1 mpid 0x100, not 0x1*/
    core += 0xff;
#endif
    EntryPoint = (ULONG)AmbaLink_EntryPoint;

    /* Release Core-3, the primary core of Linux. */
    if (!AmbaLink_IsWarmBoot()) {
#if defined(CONFIG_FWPROG_ATF_ENABLE)
#if defined(CONFIG_ARM64)
        (void)AmbaSmcCall(PSCI_CPU_ON_AARCH64, core, EntryPoint, 0x0U, 0x0U);
#else
        (void)AmbaSmcCallA32(PSCI_CPU_ON_AARCH32, core, EntryPoint, 0x0U, 0x0U);
#endif
#else
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        volatile UINT32 *pAxiConfigReg;

        /* 0x20f2000068 maps to RVBARADDR[39:8] core1 jump addr */
        pAxiConfigReg = (volatile UINT32 *)(AMBA_CORTEX_A76_AXI_CONFIG_PHYS_BASE_ADDR + 0x68U);
        *pAxiConfigReg = (volatile UINT32)(EntryPoint >> 0x8U);

        /* 0x20f2000028 b[17]=0 core1 boot */
        pAxiConfigReg = (volatile UINT32 *)(AMBA_CORTEX_A76_AXI_CONFIG_PHYS_BASE_ADDR + 0x28U);
        *pAxiConfigReg &= ~(0x1U<<17U);
#else
#error TODO
#endif
#endif
    }
}

INT32 AmbaLink_ErasePartition(INT32 PartFlag, UINT32 PartID, UINT32 TimeOut)
{
    UINT32 Rval;
    UINT32 NvmID;

    Rval = AmbaSvcWrap_GetNVMID(&NvmID, 1U, PartID);
    if (Rval == 0U) {
        Rval = AmbaNVM_ErasePartition(NvmID, PartFlag, PartID, TimeOut);
        if (Rval != 0) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaLink_ErasePartition: AmbaNVM_ErasePartition failed! Rval %d, PartFlag %d, PartID %d", Rval, PartFlag, PartID, 0U, 0U);
            AmbaPrint_Flush();
        }
    } else {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaLink_ErasePartition: AmbaSvcWrap_GetNVMID failed! Rval %d, PartID %d\n", Rval, PartID, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaLink_LoadPartition
 *
 *  @Description:: Load Partition
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      INT32 :
\*-----------------------------------------------------------------------------------------------*/
INT32 AmbaLink_LoadPartition(INT32 PartFlag, UINT32 PartID, UINT8 *pDataBuf, UINT32 TimeOut)
{
    INT32 Rval;
    UINT32 NvmID;

    Rval = AmbaSvcWrap_GetNVMID(&NvmID, 1U, PartID);
    if (Rval == 0U) {
        Rval = AmbaNVM_ReadPartition(NvmID, PartFlag, PartID, pDataBuf, TimeOut);
        if (Rval != 0) {
            AmbaPrint_PrintUInt5("AmbaLink_LoadPartition: AmbaNVM_ReadPartition failed! Rval %d, PartFlag %d, PartID %d", Rval, PartFlag, PartID, 0U, 0U);
            AmbaPrint_Flush();
        }
    } else {
        AmbaPrint_PrintUInt5("AmbaLink_LoadPartition: AmbaSvcWrap_GetNVMID failed! Rval %d, PartID %d\n", Rval, PartID, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaLink_ReadPartitionInfo
 *
 *  @Description:: Read PartitionInfo
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      INT32 :
\*-----------------------------------------------------------------------------------------------*/
INT32 AmbaLink_ReadPartitionInfo(INT32 PartFlag, UINT32 PartID, AMBA_PARTITION_ENTRY_s *pPartInfo)
{
    INT32 Rval;
    UINT32 NvmID;

    Rval = AmbaSvcWrap_GetNVMID(&NvmID, 1U, AMBA_USER_PARTITION_PTB);
    if (Rval == 0U) {
        Rval = AmbaNVM_ReadPartitionInfo(NvmID, PartFlag, PartID, pPartInfo);
        if (Rval != 0) {
            AmbaPrint_PrintUInt5("AmbaLink_ReadPartitionInfo: AmbaNVM_ReadPartitionInfo failed! Rval %d, PartFlag %d, PartID %d", Rval, PartFlag, PartID, 0U, 0U);
            AmbaPrint_Flush();
        }
    } else {
        AmbaPrint_PrintUInt5("AmbaLink_ReadPartitionInfo: AmbaSvcWrap_GetNVMID failed! Rval %d, PartID %d\n", Rval, PartID, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    return Rval;
}

static UINT32 Kernel_PartId = AMBA_USER_PARTITION_LINUX_KERNEL;
void AmbaLink_SetKernelPartId(UINT32 PartID)
{
    Kernel_PartId = PartID;
}
/*----------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaLink_LoadKernel
 *
 *  @Description::
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*----------------------------------------------------------------------------*/
static INT32 AmbaLink_LoadKernel(void)
{
    INT32 Rval;
    UINT32 LoadCodeStart, LoadCodeEnd;
    AMBA_PARTITION_ENTRY_s LnxPartInfo;

    AmbaLink_ReadPartitionInfo(1, Kernel_PartId, &LnxPartInfo);

#ifdef CONFIG_AMBALINK_RPMSG_HIBER
    if (AmbaLink_IsWarmBoot()) {
        return AmbaIPC_HiberReturn(AMBA_LINK_SUSPEND_TO_RAM);
    } else {
        AmbaKAL_GetSysTickCount(&LoadCodeStart);
        Rval = AmbaIPC_HiberResume(0x0);
        AmbaKAL_GetSysTickCount(&LoadCodeEnd);
    }

    if (Rval == 0) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Hibernation image loaded (%d ms)!!", LoadCodeEnd - LoadCodeStart, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }
    else
#endif // CONFIG_AMBALINK_RPMSG_HIBER
    {
        AmbaLink_DtbInit();
        AmbaKAL_GetSysTickCount(&LoadCodeStart);

        if (0 > AmbaLink_LoadPartition(1, Kernel_PartId, (UINT8 *)(ULONG)AmbaLinkCtrl.AmbarellaZRealAddr, 8000)) {
            AMBA_FS_FILE *Fp;
            char *pLinuxPath = "c:\\Linux\\Image";
            AMBA_FS_FILE_INFO_s Stat;
            UINT32 SizeToRead;

            if (AmbaFS_GetFileInfo(pLinuxPath, &Stat) == 0) {
                AmbaFS_FileOpen(pLinuxPath, "r",&Fp);

                SizeToRead = (UINT32)Stat.Size;
                while (SizeToRead > 0) {
                    AmbaFS_FileRead((void *)(ULONG) AmbaLinkCtrl.AmbarellaZRealAddr, 1, (UINT64) SizeToRead, Fp, &Rval);
                    SizeToRead -= Rval;
                    if (Rval <= 0)
                        break;
                }
                Rval = AmbaFS_FileClose(Fp);
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaLink_Boot: Load Linux Kernel from SD card: %lld bytes\n", Stat.Size, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();

            } else {
                return -1;
            }
        }
        AmbaKAL_GetSysTickCount(&LoadCodeEnd);

        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Load Linux kernel time = %d", LoadCodeEnd - LoadCodeStart, 0U, 0U, 0U, 0U);
        //AmbaPrint_Flush();
    }

    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaLink_LinuxBoot
 *
 *  @Description::
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*----------------------------------------------------------------------------*/
static INT32 AmbaLink_LinuxBoot(INT32 CpuID, UINT32 KernelStart)
{
    /* ambalink v1: Linux need to be booted at core-3 */
    if (CpuID == CONFIG_BOOT_CORE_LINUX) {
        /* jump to the start point of Linux Kernel
         * uint32_t fnid,
         * uint64_t kernel_start,
         * uint64_t dtb_address,
         * uint64_t cpu_mode,
         * Returns: PSCI error codes.
         */
#if defined(CONFIG_FWPROG_ATF_ENABLE)
        return secure_monitor_call(AMBA_SIP_BOOT_LINUX,
                                   KernelStart,
                                   AmbaLinkCtrl.AmbarellaParamsPhys,
                                   NS_SWITCH_AARCH64);
#else
        void AmbaLink_BootLnxAsm(UINT64 dtb, UINT64 p1, UINT64 p2, UINT64 p3, void *lnx);
        AmbaLink_BootLnxAsm(AmbaLinkCtrl.AmbarellaParamsPhys, 0U, 0U, 0U, (void *)(ULONG)KernelStart);
#endif
    }

    return -1;
}
#endif // CONFIG_AMBALINK_BOOT_OS

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
    AmbaMisra_TouchUnused(pPtr);

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
static INT32 AmbaLink_MemPool_Init(void)
{
    UINT32 i;
    UINT32 Status;
    static char Name[16] = "AmbaLinkPool";
    static UINT8 AmbaLinkMemPoolInitFlag = 0;

    if(AmbaLinkMemPoolInitFlag != 1U) {
        Status = AmbaKAL_MutexCreate(&AmbaLinkMemoryPoolMutex, Name);
        if (Status == 0U) {
            (void)AmbaKAL_MutexTake(&AmbaLinkMemoryPoolMutex, AMBA_KAL_WAIT_FOREVER);
            for(i = 0U; i < MEM_POOL_MAX; i++) {
                if (AmbaWrap_memset(AmbaLinkMemPool[i].pIndex, 0, AmbaLinkMemPool[i].Number)!= 0U) { }
            }
            (void)AmbaKAL_MutexGive(&AmbaLinkMemoryPoolMutex);
            AmbaLinkMemPoolInitFlag = 1U;
        }
    }

    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaLink_RpmsgInitDone
 *
 *  @Description::
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*----------------------------------------------------------------------------*/
void AmbaLink_RpmsgInitDone(void)
{
    if (AmbaKAL_EventFlagSet(&AmbaLinkEventFlag, AMBALINK_BOOT_RPMSG_READY) != OK) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "AmbaKAL_EventFlagSet failed!(0x2)", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
    }
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaLink_BootType
 *
 *  @Description:: Notify the caller Linux is warmboot, coldboot or hibernation boot.
 *
 *  @Input      ::
 *      TimeOutMs:   event flag time out in mini-seconds.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      INT32 : WarmBoot(0) / ColdBoot(1) / Hibernation (2)
\*----------------------------------------------------------------------------*/
UINT32 AmbaLink_BootType(UINT32 TimeOutMs)
{
    UINT32 Status;
    UINT32 ActFlg;

    Status = AmbaKAL_EventFlagGet(&AmbaLinkEventFlag, AMBALINK_BOOT_IPC_READY,
                                   1U, 0U, &ActFlg, TimeOutMs);
    if (Status != 0U) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaKAL_EventFlagGet failed!(%d)", Status, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    return AmbaLinkBootType;
}

#ifdef CONFIG_AMBALINK_BOOT_OS
/*----------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaLink_Root
 *
 *  @Description:: main entry of Ambarella Network Support (before RTOS is running)
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*----------------------------------------------------------------------------*/
void AmbaLink_Root(INT32 CpuID)
{
    /* prevent wake up by either IRQ or FIQ */
    AMBA_disable_interrupt();

    /* In SMP RTOS, ThreadX uses sev and wfe to do synchronization. */
    /* Use another way to do linux booting control. */
    do {
        AMBA_SEVL();
        AMBA_WFE();
        AMBA_WFE();
    } while (IpcReadReg(AHB_SCRATCHPAD_BASE + AHB_SP_SYNC) != 0xdeadbeefU);

    IpcWriteReg(AHB_SCRATCHPAD_BASE + AHB_SP_SYNC, 0x0);

    if (AmbaLinkBootType == AMBALINK_COLD_BOOT) {
        AmbaLink_LinuxBoot(CpuID, AmbaLinkCtrl.AmbarellaZRealAddr);
    } else if ((AmbaLinkBootType == AMBALINK_HIBER_BOOT) && LinuxKernelStart != 0) {
        AmbaLink_LinuxBoot(CpuID, LinuxKernelStart);
    } else {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Linux boot failed (%d, 0x%012llx)!", CpuID, LinuxKernelStart, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaLink_OtherCoreEntry
 *
 *  @Description:: Boot Other CoreEntry
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      INT32 :
\*-----------------------------------------------------------------------------------------------*/
void AmbaLink_OtherCoreEntry(void)
{
#if defined(CONFIG_CPU_CORTEX_A76)
    AmbaLink_Root(CONFIG_BOOT_CORE_LINUX);
    /*TODO: A76 cache coherence*/
#else
    extern UINT32 AmbaRTSL_CpuGetCoreID(void);
    UINT32 CpuID = AmbaRTSL_CpuGetCoreID();
    UINT64 CpuExtCtrlReg = 0;

    CpuExtCtrlReg = AmbaASM_ReadCpuExtCtrl();
    CpuExtCtrlReg |= 0x40;  /* Set SMPEN */
    AmbaASM_WriteCpuExtCtrl(CpuExtCtrlReg);
    AmbaLink_Root(CpuID);   /* jump to the entry of AmbaLink */
#endif

    /* Shall not happen!! */
    __asm__ __volatile__ ("b ." : : : "memory");
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaLink_Boot
 *
 *  @Description::
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*----------------------------------------------------------------------------*/
static INT32 AmbaLink_Boot(UINT32 TimeOut)
{
    INT32 Rval = 0;
    UINT32 ActFlg;

#ifdef AMBA_SYS_BOOT_FROM_SPI_NOR
#ifndef CONFIG_MUTI_BOOT_DEVICE
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || \
    defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32)
#define DMA1_IRQ AMBA_INT_SPI_ID116_DMA1
#elif defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
#define DMA1_IRQ AMBA_INT_SPI_ID118_DMA_ENGINE1
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#define DMA1_IRQ AMBA_INT_SPI_ID118_DMA1
#else
#error unknown chip
#endif
    UINT32 BootMode;

    AmbaSYS_GetBootMode(&BootMode);
    if (BootMode == AMBA_SYS_BOOT_FROM_SPI_NOR) {
        /* reset cmd/addr/data lane to 1bit */
        extern UINT32 AmbaSpiNOR_SoftReset(void);
        AmbaSpiNOR_SoftReset();
        /* let linux handle dma1 */
        AmbaRTSL_GicSetCpuTarget(DMA1_IRQ, 0x1U << CONFIG_BOOT_CORE_LINUX);
        /* for unknown reason linux 5.4 set dma1 irq target back to 0x9, ignore dma1 irq in rtos */
        AmbaRTSL_GicSetISR(DMA1_IRQ, NULL);
    }
#endif
#endif
    /* Boot linux Core */
    AmbaLink_BootLinuxCore();

    if ((Rval = AmbaKAL_EventFlagGet(&AmbaLinkEventFlag,
                                      AMBALINK_LINUX_LOADED,
                                      1U, 1U,
                                      &ActFlg,
                                      TimeOut)) != OK) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaKAL_EventFlagGet failed (%d)!", Rval, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: Load Linux is failed?", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        Rval = -1;
        goto Done;
    }

    if (!AmbaLink_IsWarmBoot()) {
        IpcWriteReg(AHB_SCRATCHPAD_BASE + AHB_SP_SYNC, 0xdeadbeefU);
    }

    AMBA_SEV();
    AMBA_WFE();

Done:
    return Rval;
}
#endif // CONFIG_AMBALINK_BOOT_OS

/*----------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaLink_Init
 *
 *  @Description:: Entry of AmbaLink.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*----------------------------------------------------------------------------*/
void AmbaLink_Init(void)
{
    static char Name[16] = "AmbaLinkFlag";
    extern void AmbaLink_LinkCtrlCfg(void);

    /* init AmbaLink related setting */
    AmbaLink_LinkCtrlCfg();

    /* master init global locks fist */
#if defined(CONFIG_AMBALINK_RPMSG_ROLE_MAST)
    (void)AmbaIPC_MutexInit();
    (void)AmbaIPC_SpinInit();
#endif

    /* init memory pool */
    (void)AmbaLink_MemPool_Init();

    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaLink: SpinLock 0x%08x, 0x%08x", AmbaLinkCtrl.AmbaIpcSpinLockBufAddr, AmbaLinkCtrl.AmbaIpcSpinLockBufSize, 0U, 0U, 0U);
    //AmbaPrint_Flush();
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaLink: Mutex    0x%08x", AmbaLinkCtrl.AmbaIpcMutexBufAddr, 0U, 0U, 0U, 0U);
    //AmbaPrint_Flush();

    if (AmbaLinkEventFlag.tx_event_flags_group_id == 0U) {
        if (AmbaKAL_EventFlagCreate(&AmbaLinkEventFlag, Name) != OK) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: can't not create AmbaLink event flag", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
        }
    }

#if defined(CONFIG_AMBALINK_BOOT_OS) && defined(CONFIG_AMBALINK_RPMSG_HIBER)
    AmbaIPC_HiberInitNVMCtrl();
    AmbaIPC_HiberBootSetup();
#endif

#if !defined(CONFIG_AMBALINK_RPMSG_ROLE_MAST)
    /* slave init global locks */
    (void)AmbaIPC_MutexInit();
    (void)AmbaIPC_SpinInit();
#endif

    AmbaLink_Enable = 1U;
}

/* Simulate Pure Linux without AmbaLink_Init()
 *   AmbaLink_BootLinuxCore ->
 *   AmbaLink_EntryPoint ->
 *   AmbaLink_OtherCoreEntry ->
 *   AmbaLink_Root ->
 *   AmbaLink_LinuxBoot
 */
INT32 AmbaLink_BootV2(UINT32 TimeOut);
INT32 AmbaLink_BootV2(UINT32 TimeOut)
{
    extern void AmbaLink_LinkCtrlCfg(void);

    AmbaLink_LinkCtrlCfg();
#ifdef CONFIG_AMBALINK_BOOT_OS
    AmbaLink_DtbInit();
    AmbaLink_LoadPartition(1, Kernel_PartId, (UINT8 *)(ULONG)AmbaLinkCtrl.AmbarellaZRealAddr, TimeOut);
    AmbaLink_BootLinuxCore();

    IpcWriteReg(AHB_SCRATCHPAD_BASE + AHB_SP_SYNC, 0xdeadbeefU);
    AMBA_SEV();
    AMBA_WFE();
#else
    (void)TimeOut;
#endif // CONFIG_AMBALINK_BOOT_OS

    return 0;
}

#ifdef CONFIG_AMBALINK_BOOT_OS
void AmbaLink_OS(void)
{
    INT32 Rval;

    if (AmbaLink_LoadKernel() < 0) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: AmbaLink_LoadKernel failed!\n", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        return;
    }
    if ((Rval = AmbaKAL_EventFlagSet(&AmbaLinkEventFlag,
                                      AMBALINK_LINUX_LOADED)) != OK) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaKAL_EventFlagSet failed (%d)!", Rval, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
        return;
    }

    AmbaLink_Boot(10000);
}
#endif // CONFIG_AMBALINK_BOOT_OS

/**
 * RPMSG initialization
 *
 * @param[in]  devID   RPMSG_DEV_AMBA, RPMSG_DEV_OAMP
 * @return  0 as OK, others as failed.
 */
UINT32 AmbaLink_RpmsgInit(UINT32 devID)
{
    UINT32 err = 0U;
    UINT32 ActFlg;

    switch (devID) {
    case RPMSG_DEV_AMBA:
        /* TODO: block until rpmsg host online */
        if (AmbaLinkBootType != AMBALINK_COLD_BOOT) {
            AmbaIPC_RpmsgRestoreEnable(1);
        }

        AmbaIPC_RpmsgInit();
        break;

#ifdef CONFIG_OPENAMP
    case RPMSG_DEV_OAMP:
        /* TODO: master role. */
//#ifdef CONFIG_AMBALINK_RPMSG_ROLE_MAST
//        {
//            extern void AmbaOamp_init_master(void);
//
//            AmbaOamp_init_master();
//        }
//#else
        {
            extern void AmbaOamp_init_slave(void);

            AmbaOamp_init_slave();
        }
//#endif
        break;
#endif

    default:
        err = IPC_ERR_EINVAL;
        break;
    }

    if (AmbaLinkBootType != AMBALINK_COLD_BOOT) {
        if (AmbaKAL_EventFlagGet(&AmbaLinkEventFlag,
                                  AMBALINK_BOOT_RPMSG_READY,
                                  1U, 1U,
                                  &ActFlg,
                                  AMBA_KAL_WAIT_FOREVER) != OK) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "AmbaKAL_EventFlagGet failed!(0x2)", NULL, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
        }
    }

    return err;
}
