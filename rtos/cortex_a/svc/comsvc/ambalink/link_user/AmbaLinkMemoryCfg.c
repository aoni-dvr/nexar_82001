/**
 * @file AmbaLinkMemoryCfg.c
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
 * @details ambalink user config
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaLinkPrivate.h"
#include "AmbaLink.h"
#include "AmbaCache.h"
#include "AmbaNVM_Partition.h"
#include "AmbaPrint.h"
#include "AmbaWrap.h"

/*-----------------------------------------------------------------------------------------------*\
 * example memory layout.
       CONFIG_AMBALINK_RPMSG_SIZE=2048
       CONFIG_AMBALINK_RPMSG_NUM=2048
       CONFIG_AMBALINK_MEM_SIZE=0x08000000
       CONFIG_AMBALINK_SHARED_MEM_SIZE=0x00600000
               0            +-----------------------------------+   0x0000_0000
                            | Reserved memory                   |
                            +-----------------------------------+   0x0000_4000
                            | ATF (ARM Trust Firmware)          |
                            +-----------------------------------+   0x0002_0000
                            | ThreadX                           |
            1920            +-----------------------------------+   0x7800_0000  AmbaLinkSharedMemAddr = 0x78000000  VringHostAndClntBufAddr = 0x78000000
                            | svq_buf (2MB)                     |                2048*1024=send vq
                            +-----------------------------------+
                            | rvq_buf (2MB)                     |                2048*1024=recv vq
            1924            +-----------------------------------+   0x7840_0000  VringClntToHostAddr   = 0x78400000
                            | svq_vring (40KB)                  |
            1924+40K        +-----------------------------------+   0x7840_A000  VringHostToClntAddr   = 0x7840A000
                            | rvq_vring (40KB)                  |
            1924+80K        +-----------------------------------+   0x7841_4000  RpmsgSuspBackupAddr   = 0x78414000
                            | rpmsg suspend backup area (128KB) |                RpmsgSuspBackupSize   = 0x00020000
            1924+208K       +-----------------------------------+   0x7843_4000  AmbaRpcProfileAddr    = 0x78434000
                            | rpc profiling statistics (4k)     |
            1924+212K       +-----------------------------------+   0x7843_5000  AmbaRpmsgProfileAddr  = 0x78435000
                            | rpmsg profiling statistics (68KB) |                0x11000=64K
            1924+280K       +-----------------------------------+   0x7844_6000  AmbaIpcSpinLockBufAddr= 0x78446000
                            | spin_lock (4KB)                   |                AmbaIpcSpinLockBufSize= 0x00001000
            1924+284K       +-----------------------------------+   0x7844_7000  AmbaIpcMutexBufAddr   = 0x78447000
                            | global_mutex (4KB)                |                AmbaIpcMutexBufSize   = 0x00001000
            1924+288K       +-----------------------------------+   0x7844_8000  AmbaLinkAossAddr      = 0x78448000
                            | AOSS (64 KB)                      |                0x10000=64K
            1924+352K       +-----------------------------------+   0x7845_8000
                            | BOSS (reserved)                   |                reserved
            1926            +-----------------------------------+   0x7860_0000  AmbarellaPPMSize      = 0x78600000  ATAG AmbarellaParamsPhys   = 0x78600000
                            | Linux DTB & MMU (512 KB)          |                AmbarellaTextOfs      = 0x00080000  ATAG AmbarellaParamsSize = 0x10000 64K
                            +-----------------------------------+   0x7868_0000  AmbarellaZRealAddr    = 0x78680000
                            | Linux Kernel                      |
                            |   Size: 128 MB - 512 KB           |
            2048            +-----------------------------------+   0x8000_0000
\*-----------------------------------------------------------------------------------------------*/

UINT32 volatile AmbaLink_Enable = 0U;
#define AMBAIPC_SPIN_LOCK_SIZE          0x1000
#define AMBAIPC_MUTEX_SIZE              0x1000

#if defined(CONFIG_AMBALINK_RPMSG_G1)
#define AMBARELLA_PPM_SIZE              (CONFIG_AMBALINK_SHARED_MEM_ADDR + CONFIG_AMBALINK_SHARED_MEM_SIZE)

#if defined(CONFIG_AMBALINK_BOOT_OS_LEGACY_5_7)
#define AMBARELLA_TEXTOFS               0x00080000      /* Kernel offset in Aarch64. */
#else /* After Linux-v5.8 */
#define AMBARELLA_TEXTOFS               0x00
    /* TODO: Kernel offset in Aarch32. */
#endif

#define AMBARELLA_ZRELADDR              (AMBARELLA_PPM_SIZE + AMBARELLA_TEXTOFS)

#if defined(CONFIG_AMBALINK_BOOT_OS_LEGACY_5_7)
/* TEXT_OFFSET is 0x80000 for arm64 */
#define AMBARELLA_PARAMS_PHYS              AMBARELLA_PPM_SIZE
#define AMBARELLA_PARAMS_SIZE              0x10000
#else
/* AMBARELLA_PARAMS_PHYS: has to be 0x200000 boundary */
/* cf. MAX_FDT_SIZE of arch/arm64/include/asm/boot.h */
#define AMBARELLA_PARAMS_SIZE              0x10000
/* cf. MIN_FDT_ALIGN, MIN_KIMG_ALIGN of arch/arm64/include/asm/boot.h */
#define AMBARELLA_PARAMS_PHYS              (AMBARELLA_PPM_SIZE - AMBARELLA_PARAMS_SIZE)
#endif

/*-----------------------------------------------------------------------------------------------*\
 * Rpmsg defined and calcualted.
\*-----------------------------------------------------------------------------------------------*/
#define RPMSG_TOTAL_BUF_SPACE           (CONFIG_AMBALINK_RPMSG_NUM * CONFIG_AMBALINK_RPMSG_SIZE)

#define VRING_HOST_AND_CLNT_BUF         (CONFIG_AMBALINK_SHARED_MEM_ADDR)

#define VRING_SIZE                      ((((CONFIG_AMBALINK_RPMSG_NUM / 2) * 19 + (0x1000 - 1)) & ~(0x1000 - 1)) + \
                                        (((CONFIG_AMBALINK_RPMSG_NUM / 2) * 17 + (0x1000 - 1)) & ~(0x1000 - 1)))

#define VRING_CLNT_TO_HOST              (VRING_HOST_AND_CLNT_BUF + RPMSG_TOTAL_BUF_SPACE)
#define VRING_HOST_TO_CLNT              (VRING_CLNT_TO_HOST + VRING_SIZE)

/*-----------------------------------------------------------------------------------------------*\
 * Rpmsg suspend backup area.
\*-----------------------------------------------------------------------------------------------*/
#define RPMSG_SUSPEND_BACKUP_ADDR       (VRING_HOST_TO_CLNT + VRING_SIZE)
#define RPMSG_SUSPEND_BACKUP_SIZE       0x20000

/*-----------------------------------------------------------------------------------------------*\
 * Rpc and rpmsg profiling statistics area.
\*-----------------------------------------------------------------------------------------------*/
#define RPC_PROFILE_ADDR                (RPMSG_SUSPEND_BACKUP_ADDR + RPMSG_SUSPEND_BACKUP_SIZE)
#define RPC_PROFILE_SIZE                0x1000

#define RPMSG_PROFILE_ADDR              (RPC_PROFILE_ADDR + RPC_PROFILE_SIZE)

#define MAX_RPC_RPMSG_PROFILE_SIZE      0x11000
#define RPC_RPMSG_PROFILE_SIZE          (RPC_PROFILE_SIZE + ((17 * CONFIG_AMBALINK_RPMSG_NUM + (0x1000 - 1)) & ~(0x1000 -1)))

#define RPC_HEADER_SIZE                 128
#define RPMSG_HEADER_SIZE               16
#define RPC_PAYLOAD_SIZE                (CONFIG_AMBALINK_RPMSG_SIZE - RPC_HEADER_SIZE - RPMSG_HEADER_SIZE)

/*-----------------------------------------------------------------------------------------------*\
 * Spin-lock defined and calcualted.
\*-----------------------------------------------------------------------------------------------*/
#define AMBAIPC_SPIN_LOCK_ADDR          (RPMSG_PROFILE_ADDR + MAX_RPC_RPMSG_PROFILE_SIZE)

/*-----------------------------------------------------------------------------------------------*\
 * Global-mutex defined and calcualted.
\*-----------------------------------------------------------------------------------------------*/
#define AMBAIPC_MUTEX_ADDR              (AMBAIPC_SPIN_LOCK_ADDR + AMBAIPC_SPIN_LOCK_SIZE)

/*-----------------------------------------------------------------------------------------------*\
 * AOSS info address (for hibernation) defined and calcualted.
\*-----------------------------------------------------------------------------------------------*/
#define AMBALINK_AOSS_ADDR              (AMBAIPC_MUTEX_ADDR + AMBAIPC_MUTEX_SIZE)
#define AMBALINK_AOSS_SIZE              0x10000

#if !defined(CONFIG_AMBALINK_BOOT_OS_LEGACY_5_7)
#if AMBARELLA_PARAMS_PHYS < (AMBALINK_AOSS_ADDR + AMBALINK_AOSS_SIZE)
#error Not enough size of DTB
#endif
#endif # Check DTB

#endif // CONFIG_AMBALINK_RPMSG_G1

/*-----------------------------------------------------------------------------------------------*\
 * The parameters for the tasks related to AmbaLink.
\*-----------------------------------------------------------------------------------------------*/
AMBA_LINK_CTRL_s AmbaLinkCtrl;
AMBA_LINK_TASK_CTRL_s AmbaLinkTaskCtrl[LINK_TASK_MAX] = {
    [LINK_TASK_INIT] = {
        .TaskName   = "LINK_TASK",
        .Priority   = 115,
        .StackSize  = 0x2000,
        .CoreSel    = 0x1,
    },
    [LINK_TASK_HIBER] = {
        .TaskName   = "LINK_HIBER",
        .Priority   = 116,
        .StackSize  = 0x1000,
        .CoreSel    = 0x1,
    },
    [LINK_TASK_SVC] = {
        .TaskName   = "LINK_RPC_SVC_THREAD",
        .Priority   = 130,
        .StackSize  = 0x1000,
        .CoreSel    = 0x1,
    },
    [LINK_TASK_RVQ] = {
        .TaskName   = "LINK_RPMSG_RVQ",
#if defined(CONFIG_AMBALINK_RPMSG_ASIL)
        .Priority   = 7,
#else
        .Priority   = 90,
#endif
        .StackSize  = 0x1000,
        .CoreSel    = 0x1,
    },
    [LINK_TASK_SVQ] = {
        .TaskName   = "LINK_RPMSG_SVQ",
        .Priority   = 89,
        .StackSize  = 0x1000,
        .CoreSel    = 0x1,
    },
    [LINK_TASK_VFS_OPS] = {
        .TaskName   = "LINK_VFS_OPS",
        .Priority   = 132,
        .StackSize  = 0x8000,
        .CoreSel    = 0x1,
    },
    [LINK_TASK_VFS_SG] = {
        .TaskName   = "LINK_VFS_SG",
        .Priority   = 133,
        .StackSize  = 0x1000,
        .CoreSel    = 0x1,
    },
    [LINK_TASK_FS_NOTIFY] = {
        .TaskName   = "LINK_FS_NOTIFY",
        .Priority   = 135,
        .StackSize  = 0x1000,
        .CoreSel    = 0x1,
    },
    [LINK_TASK_FUSE] = {
        .TaskName   = "LINK_FUSE",
        .Priority   = 134,
        .StackSize  = 0x4000,
        .CoreSel    = 0x1,
    },
    [LINK_TASK_SAFETY] = {
        .TaskName   = "LINK_SAFETY",
        .Priority   = 82,
        .StackSize  = 0x4000,
        .CoreSel    = 0x1,
    },
};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaLink_ShowConfig
 *
 *  @Description:: Show Config
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      INT32 :
\*-----------------------------------------------------------------------------------------------*/
INT32 AmbaLink_ShowConfig(void);
INT32 AmbaLink_ShowConfig(void)
{
    const AMBA_LINK_TASK_CTRL_s *pTask;

#ifdef CONFIG_AMBALINK_RPMSG_G2
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "G2_RpmsgNumBuf        = 0x%08x\r\n", AmbaLinkCtrl.G2_RpmsgNumBuf, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "G2_RpmsgBufSize       = 0x%08x\r\n", AmbaLinkCtrl.G2_RpmsgBufSize, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "G2_BufAddr            = 0x%08x\r\n", AmbaLinkCtrl.G2_BufAddr, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "G2_S2MAddr            = 0x%08x\r\n", AmbaLinkCtrl.G2_S2MAddr, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "G2_M2SAddr            = 0x%08x\r\n", AmbaLinkCtrl.G2_M2SAddr, 0U, 0U, 0U, 0U);
#endif
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaLinkMachineRev    = 0x%08x\r\n", AmbaLinkCtrl.AmbaLinkMachineRev, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaLinkSharedMemAddr = 0x%08x\r\n", AmbaLinkCtrl.AmbaLinkSharedMemAddr, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaLinkMemSize       = 0x%08x\r\n", AmbaLinkCtrl.AmbaLinkMemSize, 0U, 0U, 0U, 0U);

    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbarellaPPMSize      = 0x%08x\r\n", AmbaLinkCtrl.AmbarellaPPMSize, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbarellaZRealAddr    = 0x%08x\r\n", AmbaLinkCtrl.AmbarellaZRealAddr, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbarellaTextOfs      = 0x%08x\r\n", AmbaLinkCtrl.AmbarellaTextOfs, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbarellaParamsPhys   = 0x%08x\r\n", AmbaLinkCtrl.AmbarellaParamsPhys, 0U, 0U, 0U, 0U);

    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "VringHostAndClntBufAddr = 0x%08x\r\n", AmbaLinkCtrl.VringHostAndClntBufAddr, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "VringClntToHostAddr   = 0x%08x\r\n", AmbaLinkCtrl.VringClntToHostAddr, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "VringHostToClntAddr   = 0x%08x\r\n", AmbaLinkCtrl.VringHostToClntAddr, 0U, 0U, 0U, 0U);

    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "RpmsgNumBuf           = 0x%08x\r\n", AmbaLinkCtrl.RpmsgNumBuf, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "RpmsgBufSize          = 0x%08x\r\n", AmbaLinkCtrl.RpmsgBufSize, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "RpcPktSize            = 0x%08x\r\n", AmbaLinkCtrl.RpcPktSize, 0U, 0U, 0U, 0U);

    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "IpcSpinLockBufAddr    = 0x%08x\r\n", AmbaLinkCtrl.AmbaIpcSpinLockBufAddr, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "IpcSpinLockBufSize    = 0x%08x\r\n", AmbaLinkCtrl.AmbaIpcSpinLockBufSize, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "IpcMutexBufAddr       = 0x%08x\r\n", AmbaLinkCtrl.AmbaIpcMutexBufAddr, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "IpcMutexBufSize       = 0x%08x\r\n", AmbaLinkCtrl.AmbaIpcMutexBufSize, 0U, 0U, 0U, 0U);

    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaRpcProfileAddr    = 0x%08x\r\n", AmbaLinkCtrl.AmbaRpcProfileAddr, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaRpmsgProfileAddr  = 0x%08x\r\n", AmbaLinkCtrl.AmbaRpmsgProfileAddr, 0U, 0U, 0U, 0U);

    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "RpmsgSuspBackupAddr   = 0x%08x\r\n", AmbaLinkCtrl.RpmsgSuspBackupAddr, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "RpmsgSuspBackupSize   = 0x%08x\r\n", AmbaLinkCtrl.RpmsgSuspBackupSize, 0U, 0U, 0U, 0U);

    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaLinkAossAddr      = 0x%08x\r\n", AmbaLinkCtrl.AmbaLinkAossAddr, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaHiberImgCheck     = 0x%08x\r\n", AmbaLinkCtrl.AmbaHiberImgCheck, 0U, 0U, 0U, 0U);

    pTask = &AmbaLinkTaskCtrl[LINK_TASK_SVC];

    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "SvcPriority           = 0x%08x\r\n", pTask->Priority, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "SvcStackSize          = 0x%08x\r\n", pTask->StackSize, 0U, 0U, 0U, 0U);

    pTask = &AmbaLinkTaskCtrl[LINK_TASK_RVQ];
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "RvqPriority           = 0x%08x\r\n", pTask->Priority, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "RvqStackSize          = 0x%08x\r\n", pTask->StackSize, 0U, 0U, 0U, 0U);

    pTask = &AmbaLinkTaskCtrl[LINK_TASK_SVQ];
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "SvqPriority           = 0x%08x\r\n", pTask->Priority, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "SvqStackSize          = 0x%08x\r\n", pTask->StackSize, 0U, 0U, 0U, 0U);

    pTask = &AmbaLinkTaskCtrl[LINK_TASK_VFS_OPS];
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "VfsOpsPriority        = 0x%08x\r\n", pTask->Priority, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "VfsOpsStackSize       = 0x%08x\r\n", pTask->StackSize, 0U, 0U, 0U, 0U);

    pTask = &AmbaLinkTaskCtrl[LINK_TASK_VFS_SG];
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "VfsSgPriority         = 0x%08x\r\n", pTask->Priority, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "VfsSgStackSize        = 0x%08x\r\n", pTask->StackSize, 0U, 0U, 0U, 0U);

    AmbaPrint_Flush();

    return 0;
}

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
static int boot_from_part_a = 1;
void AmbaLink_SetBootFromA(int boot_a)
{
    boot_from_part_a = boot_a;
}

int AmbaLink_GetBootFromA(void)
{
    return boot_from_part_a;
}

static int linux_console_enable = 1;
void AmbaLink_SetLinuxConsoleEnable(int enable)
{
    linux_console_enable = enable;
}

int AmbaLink_GetLinuxConsoleEnable(void)
{
    return linux_console_enable;
}

static int erase_data = 0;
void AmbaLink_SetEraseData(int flag)
{
    erase_data = flag;
}
#endif

void AmbaLink_LinkCtrlCfg(void);
void AmbaLink_LinkCtrlCfg(void)
{
    UINT32 addr;
    AMBA_LINK_CTRL_s *pAmbaLinkCtrl = &AmbaLinkCtrl;
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
static char KernelCmdLineA[] = "root=/dev/mtdblock5 "CONFIG_AMBALINK_CMDLINE;
static char KernelCmdLineB[] = "root=/dev/mtdblock10 "CONFIG_AMBALINK_CMDLINE;
static char KernelCmdLineA_EraseData[] = "root=/dev/mtdblock5 erase_data=1 "CONFIG_AMBALINK_CMDLINE;
static char KernelCmdLineB_EraseData[] = "root=/dev/mtdblock10 erase_data=1 "CONFIG_AMBALINK_CMDLINE;
#if defined(CONFIG_BSP_CV25_NEXAR_D080)
static char KernelCmdLineA_Console[] = "root=/dev/mtdblock5 console=ttyS4 "CONFIG_AMBALINK_CMDLINE;
static char KernelCmdLineA_Console_EraseData[] = "root=/dev/mtdblock5 console=ttyS4 erase_data=1 "CONFIG_AMBALINK_CMDLINE;
static char KernelCmdLineB_Console[] = "root=/dev/mtdblock10 console=ttyS4 "CONFIG_AMBALINK_CMDLINE;
static char KernelCmdLineB_Console_EraseData[] = "root=/dev/mtdblock10 console=ttyS4 erase_data=1 "CONFIG_AMBALINK_CMDLINE;
#elif defined(CONFIG_BSP_H32_NEXAR_D081)
static char KernelCmdLineA_Console[] = "root=/dev/mtdblock5 console=ttyS2 "CONFIG_AMBALINK_CMDLINE;
static char KernelCmdLineA_Console_EraseData[] = "root=/dev/mtdblock5 console=ttyS2 erase_data=1 "CONFIG_AMBALINK_CMDLINE;
static char KernelCmdLineB_Console[] = "root=/dev/mtdblock10 console=ttyS2 "CONFIG_AMBALINK_CMDLINE;
static char KernelCmdLineB_Console_EraseData[] = "root=/dev/mtdblock10 console=ttyS2 erase_data=1 "CONFIG_AMBALINK_CMDLINE;
#else
static char KernelCmdLineA_Console[] = "root=/dev/mtdblock5 console=ttyS1 "CONFIG_AMBALINK_CMDLINE;
static char KernelCmdLineA_Console_EraseData[] = "root=/dev/mtdblock5 console=ttyS1 erase_data=1 "CONFIG_AMBALINK_CMDLINE;
static char KernelCmdLineB_Console[] = "root=/dev/mtdblock10 console=ttyS1 "CONFIG_AMBALINK_CMDLINE;
static char KernelCmdLineB_Console_EraseData[] = "root=/dev/mtdblock10 console=ttyS1 erase_data=1 "CONFIG_AMBALINK_CMDLINE;
#endif
#else
#ifdef CONFIG_AMBALINK_CMDLINE
static char KernelCmdLine[] = CONFIG_AMBALINK_CMDLINE;
#else
static char KernelCmdLine[] = "";
#endif
#endif

#if defined(CONFIG_BLD_SEQ_LINUX) || defined(CONFIG_BLD_SEQ_ATF_LINUX)
    pAmbaLinkCtrl->AmbaLinkMachineRev         = 2U;
#else
    pAmbaLinkCtrl->AmbaLinkMachineRev         = 1U;
#endif

#if defined(CONFIG_AMBALINK_RPMSG_G1)
#if defined(CONFIG_AMBALINK_MEM_SIZE)
    pAmbaLinkCtrl->AmbaLinkMemSize            = (UINT32)(CONFIG_AMBALINK_MEM_SIZE - CONFIG_AMBALINK_SHARED_MEM_SIZE);
#endif
#if defined(CONFIG_FWPROG_LNX_LOADADDR)
    pAmbaLinkCtrl->AmbarellaZRealAddr         = (UINT32)CONFIG_FWPROG_LNX_LOADADDR;
#endif
    pAmbaLinkCtrl->AmbaLinkSharedMemAddr      = (UINT32)CONFIG_AMBALINK_SHARED_MEM_ADDR;
    pAmbaLinkCtrl->AmbarellaPPMSize           = (UINT32)AMBARELLA_PPM_SIZE;
    pAmbaLinkCtrl->AmbarellaTextOfs           = (UINT32)AMBARELLA_TEXTOFS;
    pAmbaLinkCtrl->AmbarellaParamsPhys        = (UINT32)AMBARELLA_PARAMS_PHYS;
    pAmbaLinkCtrl->AmbarellaParamsSize        = (UINT32)AMBARELLA_PARAMS_SIZE;

    pAmbaLinkCtrl->AmbaLinkAossAddr           = (UINT32)AMBALINK_AOSS_ADDR;
    pAmbaLinkCtrl->AmbaHiberImgCheck          = 0;

    pAmbaLinkCtrl->VringHostAndClntBufAddr    = VRING_HOST_AND_CLNT_BUF;
    pAmbaLinkCtrl->VringClntToHostAddr        = VRING_CLNT_TO_HOST;
    pAmbaLinkCtrl->VringHostToClntAddr        = VRING_HOST_TO_CLNT;
    pAmbaLinkCtrl->RpmsgNumBuf                = CONFIG_AMBALINK_RPMSG_NUM;
    pAmbaLinkCtrl->RpmsgBufSize               = CONFIG_AMBALINK_RPMSG_SIZE;
    pAmbaLinkCtrl->RpmsgSuspBackupAddr        = RPMSG_SUSPEND_BACKUP_ADDR;
    pAmbaLinkCtrl->RpmsgSuspBackupSize        = RPMSG_SUSPEND_BACKUP_SIZE;
    pAmbaLinkCtrl->RpcPktSize                 = (UINT32)RPC_PAYLOAD_SIZE;
#endif

#ifdef CONFIG_AMBALINK_RPMSG_G2
    pAmbaLinkCtrl->G2_RpmsgNumBuf             = (UINT32)CONFIG_AMBALINK_G2_RPMSG_NUM;
    pAmbaLinkCtrl->G2_RpmsgBufSize            = (UINT32)CONFIG_AMBALINK_G2_RPMSG_SIZE;
    pAmbaLinkCtrl->G2_BufAddr                 = (UINT32)CONFIG_AMBALINK_G2_SHARED_MEM_ADDR;
    pAmbaLinkCtrl->G2_S2MAddr                 = (UINT32)CONFIG_AMBALINK_G2_SHARED_MEM_ADDR + ((UINT32)CONFIG_AMBALINK_G2_RPMSG_NUM * (UINT32)CONFIG_AMBALINK_G2_RPMSG_SIZE);
    pAmbaLinkCtrl->G2_M2SAddr                 = pAmbaLinkCtrl->G2_S2MAddr + (pAmbaLinkCtrl->G2_RpmsgNumBuf*20U);
    /* 4k align round up */
    pAmbaLinkCtrl->G2_M2SAddr                += 0x1000U - 1U;
    pAmbaLinkCtrl->G2_M2SAddr                &= ~(0x1000U - 1U);
#endif // CONFIG_AMBALINK_RPMSG_G2

#if defined(CONFIG_AMBALINK_LOCK_ADDR)
    /* user specified lock address */
    pAmbaLinkCtrl->AmbaIpcSpinLockBufAddr     = CONFIG_AMBALINK_LOCK_ADDR;
#elif !defined(CONFIG_AMBALINK_BOOT_OS) && !defined(CONFIG_AMBALINK_RPMSG_G1)
    /* G2 pure threadx */
    pAmbaLinkCtrl->AmbaIpcSpinLockBufAddr     = pAmbaLinkCtrl->G2_M2SAddr + (pAmbaLinkCtrl->G2_RpmsgNumBuf*20U);
    /* 4k align round up */
    pAmbaLinkCtrl->AmbaIpcSpinLockBufAddr    += 0x1000U - 1U;
    pAmbaLinkCtrl->AmbaIpcSpinLockBufAddr    &= ~(0x1000U - 1U);
#else
    /* G1 linux */
    pAmbaLinkCtrl->AmbaIpcSpinLockBufAddr     = (UINT32)AMBAIPC_SPIN_LOCK_ADDR;
    pAmbaLinkCtrl->AmbaRpcProfileAddr         = RPC_PROFILE_ADDR;
    pAmbaLinkCtrl->AmbaRpmsgProfileAddr       = RPMSG_PROFILE_ADDR;
#endif

    pAmbaLinkCtrl->AmbaIpcSpinLockBufSize     = (UINT32)AMBAIPC_SPIN_LOCK_SIZE;
    pAmbaLinkCtrl->AmbaIpcMutexBufAddr        = pAmbaLinkCtrl->AmbaIpcSpinLockBufAddr + pAmbaLinkCtrl->AmbaIpcSpinLockBufSize;
    pAmbaLinkCtrl->AmbaIpcMutexBufSize        = (UINT32)AMBAIPC_MUTEX_SIZE;

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    if (boot_from_part_a) {
        if (erase_data) {
            AmbaLinkCtrl.pKernelCmdLine             = linux_console_enable ? KernelCmdLineA_Console_EraseData: KernelCmdLineA_EraseData;
        } else {
            AmbaLinkCtrl.pKernelCmdLine             = linux_console_enable ? KernelCmdLineA_Console : KernelCmdLineA;
        }
    } else {
        if (erase_data) {
            AmbaLinkCtrl.pKernelCmdLine             = linux_console_enable ? KernelCmdLineB_Console_EraseData: KernelCmdLineB_EraseData;
        } else {
            AmbaLinkCtrl.pKernelCmdLine             = linux_console_enable ? KernelCmdLineB_Console : KernelCmdLineB;
        }
    }
#else
    pAmbaLinkCtrl->pKernelCmdLine             = KernelCmdLine;
#endif

    /* run on first RTOS core */
#if (CONFIG_BOOT_CORE_SRTOS > 0)
    pAmbaLinkCtrl->AmbaLinkRunTarget          = (UINT8)(0x01U << (UINT8)CONFIG_BOOT_CORE_SRTOS);
#else
    pAmbaLinkCtrl->AmbaLinkRunTarget          = 0x01U;
#endif
    pAmbaLinkCtrl->part_num                   = (UINT32)AMBA_NUM_USER_PARTITION;

#if defined(CONFIG_AMBALINK_RPMSG_G1) && defined(CONFIG_AMBALINK_BOOT_OS)
    {
        extern void AmbaLink_CvCfg(void);
        AmbaLink_CvCfg();
    }
#endif // CONFIG_AMBALINK_RPMSG_G1

#ifdef AMBA_MISRA_FIX_H
    if (AmbaWrap_memcpy(&addr, &pAmbaLinkCtrl, sizeof(addr))!= 0U) { }
#else
    addr = (UINT32)pAmbaLinkCtrl;
#endif

    (void)AmbaCache_DataClean(addr , sizeof(AmbaLinkCtrl));
}

