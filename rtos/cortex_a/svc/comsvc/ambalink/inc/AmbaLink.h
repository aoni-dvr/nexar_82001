/**
 *  @file AmbaLink.h
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
 *  @details Definitions & Constants for Ambarella CPU Link and Network Supports.
 *
 */

#ifndef AMBA_LINK_H
#define AMBA_LINK_H

#include "AmbaMisraFix.h"
#include "AmbaIPC.h"

/**
 * @defgroup AmbaLink General definition of Ambalink
 * @brief AmbaLink module implementation
 *
 * Implementation the AmbaLink module.
 * @{
 */

typedef INT32             (*INT_FUNCTION)(void);
typedef void            (*VOID_UINT32_IN_FUNCTION)(UINT32 m);

/**
 * @brief This function is used to initialize AmbaLink.
 *
 */
void AmbaLink_Init(void);
void AmbaLink_V2Init(void);

/**
 * @brief This function is used to load and boot other OS (e.g Linux)
 */
void AmbaLink_OS(void);

/**
 * @brief Notify the caller Linux is warmboot, coldboot or hibernation boot.
 *  The API is blocked until BootType can be guarateened.
 *
 * @param [in] TimeOutMs The value of time out in millisecond.
 *
 * @return 0 - OK, others - NG
 */
UINT32 AmbaLink_BootType(UINT32 TimeOutMs);


/**
 * @brief Suspend Linux to disk or dram.
 *
 * @param [in] SuspendMode The mode to suspend. See AMBA_LINK_SUSPEND_MODE_e.
 *
 * @return 0 - OK, -1 - NG
 */
INT32 AmbaIPC_LinkCtrlSuspendLinux(UINT32 SuspendMode);

/**
 * @brief This function is blocked until suspend flow is complete.
 *
 * @param [in] TimeOutMs The value of time out in millisecond.
 *
 * @return 0 - OK, others - NG
 */
INT32 AmbaIPC_LinkCtrlWaitSuspend(UINT32 TimeOutMs);

/**
 * @brief Resume Linux from suspend state
 *
 * @param [in] SuspendMode To resume from SuspendMode. See AMBA_LINK_SUSPEND_MODE_e.
 *
 * @return 0 - OK, -1 - NG
 */
INT32 AmbaIPC_LinkCtrlResumeLinux(UINT32 SuspendMode);

/**
 * @brief This function is blocked until resume flow is complete.
 *
 * @param [in] TimeOutMs The value of time out in millisecond.
 *
 * @return 0 - OK, others - NG
 */
INT32 AmbaIPC_LinkCtrlWaitResumeLinux(UINT32 TimeOutMs);


/**
 * @brief This function is called after the initialization of hibernation service of ThreadX.
 *        For example, AmbaHiber test command can be initialized in this function.
 *
 * @return 0 - OK, others - NG
 */
extern INT_FUNCTION AmbaHiber_InitCallBack;

/**
 * @brief  This function is called after the suspend is done for all suspend mode.
 * While this function is called, linux suspend is done.
 *
 * @param [in] SuspendMode The mode to suspend.
 *
 * @return 0 - OK, others - NG
 *
 */
extern VOID_UINT32_IN_FUNCTION AmbaIPC_LinkCtrlSuspendCallBack;

/**
 *  Define the hardware resource owner.
 */
#define OWNER_RTOS   0U
#define OWNER_LINUX  1U

/**
 *  AmbaIPC_LinkCtrlSuspendLinux related data structure.
 */
typedef enum {
    AMBA_LINK_SUSPEND_TO_DISK = 0,
    AMBA_LINK_SUSPEND_TO_RAM
} AMBA_LINK_SUSPEND_MODE_e;

/**
 *  Define the boot type of AmbaLink.
 */
#define AMBALINK_COLD_BOOT              0x0U
#define AMBALINK_WARM_BOOT              0x1U
#define AMBALINK_HIBER_BOOT             0x2U

#define AMBALINK_LOAD_LINUX             0x1U
#define AMBALINK_LINUX_LOADED           0x2U
#define AMBALINK_BOOT_RPMSG_READY       0x4U
#define AMBALINK_BOOT_IPC_READY         0x8U
#define AMBALINK_SUSPEND_START          0x10U
#define AMBALINK_SUSPEND_DONE           0x20U
#define AMBALINK_RPMSG_RESTORE_DONE     0x40U

/**
 *  Define maximum of types.
 */
#define AMBALINK_UINT32_MAX   0xFFFFFFFFU
#define AMBALINK_UINT16_MAX   0xFFFF
#define AMBALINK_UINT8_MAX    0xFF
#define AMBALINK_INT32_MAX    0x7FFFFFFF
#define AMBALINK_INT16_MAX    0x7FFF
#define AMBALINK_INT8_MAX     0x7F

/**
 *  Data structure of AmbaLink control.
 */
typedef struct {
    UINT32      AmbaLinkMachineID;          /**< Machine ID */
    UINT32      AmbaLinkMachineRev;         /**< Machine revision */
    UINT32      AmbaLinkSharedMemAddr;      /**< Shared memory address */
    UINT32      AmbaLinkMemSize;            /**< Memory size for Linux */

    UINT32      AmbarellaPPMSize;           /**< Reserved private memory from DRAM start */
    UINT32      AmbarellaZRealAddr;         /**< Linux kernel start address */
    UINT32      AmbarellaTextOfs;           /**< Linux kernel text offset */
    UINT32      AmbarellaParamsPhys;        /**< ATAG address */
    UINT32      AmbarellaParamsSize;        /**< ATAG size */

    UINT32      VringHostAndClntBufAddr;    /**< Start address of shared vq buff for Host and Client */
    UINT32      VringClntToHostAddr;        /**< Vring buf address for Client -> Host */
    UINT32      VringHostToClntAddr;        /**< Vring buf address for Host -> Client */
    UINT32      RpmsgNumBuf;                /**< Number of RPMSG supported */
    UINT32      RpmsgBufSize;               /**< Per RPMSG buf size */
    UINT32      RpcPktSize;                 /**< per RPC packet size */

    UINT32      AmbaIpcSpinLockBufAddr;     /**< AmbaIPC spinlock shared memory address */
    UINT32      AmbaIpcSpinLockBufSize;     /**< AmbaIPC spinlock memory size */
    UINT32      AmbaIpcMutexBufAddr;        /**< AmbaIPC mutex shared memory address */
    UINT32      AmbaIpcMutexBufSize;        /**< AmbaIPC mutex memory size */

    UINT32      AmbaRpcProfileAddr;         /**< Rpc profiling shared memoy address */
    UINT32      AmbaRpmsgProfileAddr;       /**< Rpmsg profiling shared memory address */

    UINT32      RpmsgSuspBackupAddr;        /**< Rpmsg suspend backup area addr */
    UINT32      RpmsgSuspBackupSize;        /**< Rpmsg suspend backup area size */

    UINT32      AmbaLinkAossAddr;           /**< AOSS info address for hibernation */
    UINT32      AmbaHiberImgCheck;          /**< Hibernation image check */

    const char *pKernelCmdLine;             /**< Point to Linux kernel command line */

    UINT8       AmbaLinkRunTarget;          /**< Determine the CPU target to run Linux. */

    UINT32      AmbaLinkKernelSpaceAddr;    /**< Determine the kernel space start address. */

    void       *pPrivate;                   /**< Reserved pointer for private data */
    UINT32      part_num;                   /**< Partition number, e.g. AMBA_NUM_USER_PARTITION */

#ifdef CONFIG_AMBALINK_RPMSG_G2
    UINT32      G2_BufAddr;                 /**< Ring buffers */
    UINT32      G2_M2SAddr;                 /**< Descriptors for Master to Slave */
    UINT32      G2_S2MAddr;                 /**< Descriptors for Slave to Master */
    UINT32      G2_RpmsgNumBuf;
    UINT32      G2_RpmsgBufSize;
#endif // CONFIG_AMBALINK_RPMSG_G2

} AMBA_LINK_CTRL_s;

extern AMBA_LINK_CTRL_s AmbaLinkCtrl;

/**
 *  Data structure of CVShareMem.
 */
typedef struct {
    ULONG       CVSharedMemAddr;        /**< CV Shared memory address */
    ULONG       CVSharedMemSize;        /**< CV Shared memory size */

    ULONG       CVRtosMemAddr;          /**< CV Rtos Shared memory address */
    ULONG       CVRtosMemSize;          /**< CV Rtos Shared memory size */

    ULONG       CVSysMemAddr;           /**< CV System memory address */
    ULONG       CVSysMemSize;          /**< CV System memory size */

    ULONG       CVSchdrMemAddr;         /**< CV Schdr Shared memory address */
    ULONG       CVSchdrMemSize;         /**< CV Schdr Shared memory size */

    ULONG       AppSharedMemAddr;       /**< App Shared memory address */
    ULONG       AppSharedMemSize;       /**< App Shared memory size */

    ULONG       CVSharedMemClientAddr;  /**< CV Shared memory address */
} AMBA_LINK_CVSHM_CTRL_s;

/**
 *  Data structure of AmbaLink tasks.
 */
#define MAX_TASK_NAME               64

typedef struct {
    char        TaskName[MAX_TASK_NAME];
    UINT32      Priority;
    UINT32      StackSize;
    UINT32      CoreSel;
} AMBA_LINK_TASK_CTRL_s;

typedef enum {
    /* AmbaLink task: It is a AmbaLink init task used to initialize dual OSes modules for non-Boss system. */
    LINK_TASK_INIT = 0,

    /* Hiber task: It is a task used to handle hibernation or suspend flow, such as save hibernation image, suspend linux. */
    LINK_TASK_HIBER,

    /* SVC task: It is a RPC server which is responsible for processing the rpc client requests. */
    LINK_TASK_SVC,

    /* RVQ task: It is a RPMSG task which is used to execute received RPMSG callback function. */
    /* SVQ task: It is a RPMSG task which is used to execute sending RPMSG callback function. */
    LINK_TASK_RVQ,
    LINK_TASK_SVQ,

    /* VFS OPS task: It is a vfs task used to call rtos file system APIs. */
    /* VFS SG task:  It is a vfs task used to handle scatter/gather buffer and trigger real file system read/write. */
    LINK_TASK_VFS_OPS,
    LINK_TASK_VFS_SG,

    /* FsNotify task: It is used to read fs-notify events for fs-notify rpc. */
    LINK_TASK_FS_NOTIFY,

    /**
     *  Fuse task:  The functionality of fuse task is the same to VFS task,
     *  but the file system is implemented in user space in linux.
     */
    LINK_TASK_FUSE,

    /* Safety task: for safety measure */
    LINK_TASK_SAFETY,

    LINK_TASK_MAX
} AMBA_LINK_TASK_e;

extern AMBA_LINK_TASK_CTRL_s AmbaLinkTaskCtrl[LINK_TASK_MAX];

/**
 * @brief This function is used to send a message through a RPMsg channel.
 * If there is no buffer available, it will immediately return as failed.
 *
 * @param [in] Channel a RPMsg channel
 * @param [in] pData a message buffer.
 * @param [in] Length Message length.
 *
 * @return 0 - OK, -1 - NG
 */
INT32 AmbaIPC_TrySend(AMBA_IPC_HANDLE Channel, void const *pData, INT32 Length);

/**
 * @brief This function is used to unregister a RPMsg channel.
 *
 * @param [in] Channel a RPMsg channel
 *
 * @return 0 - OK, -1 - NG
 */
INT32 AmbaIPC_UnregisterChannel(AMBA_IPC_HANDLE Channel);

/**
 * @brief       RPMSG initialization
 *
 * @param [in]  devID   RPMSG_DEV_AMBA, RPMSG_DEV_OAMP
 * @return      0 as OK, others as failed.
 */
UINT32 AmbaLink_RpmsgInit(UINT32 devID);

#ifdef  CONFIG_AMBALINK_RPMSG_ASIL
/**
 * @brief       Configure and initialize RPMSG timer
 *
 * @param [in]  counter   timeout counter. Count down per time-out period.
 */
void rpmsg_timer_config(UINT32 counter);

/**
 * @brief       Expire-function for RPMSG timer
 *
 * @param [in]  arg   NOT used.
 * @note
 *  User need to create timer with this function as expire-function.
 *  Need to calling rpmsg_timer_config before create timer.
 */
void rpmsg_timer_chk(UINT32 arg);

#endif  // CONFIG_AMBALINK_RPMSG_ASIL

/** @} */ // end of group AmbaLink
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
void AmbaLink_SetKernelPartId(UINT32 PartID);
#endif

#endif  /* AMBA_LINK_H */

