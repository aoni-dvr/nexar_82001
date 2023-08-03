/**
 * @file AmbaLinkPrivate.h
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
 * @details Definitions for Internal functions for AmbaLink implementation.
 *
 */

#ifndef AMBA_LINK_PRIVATE_H
#define AMBA_LINK_PRIVATE_H
#ifdef CONFIG_CPU_CORTEX_A53
#include "AmbaCortexA53.h"
#elif defined(CONFIG_CPU_CORTEX_R52)
#include "AmbaCortexR52.h"
#elif defined(CONFIG_CPU_CORTEX_A76)
#include "AmbaCortexA76.h"
#endif
#include "AmbaShell.h"

#include "AmbaIPC_Rpc_Def.h"

#include <AmbaATF.h>

//#define IPCDEBUG
//#define IPC_DEBUG_TIMEOUT

void IpcDebug(const char *fmt, ...);
void IpcCacheClean(const void *pAddr, UINT32 Size);
void IpcCacheInvalidate(const void *pAddr, UINT32 Size);
void IpcCacheFlush(const void *pAddr, UINT32 Size);
void *IpcVirtToPhys(void *pAddr);
void *IpcPhysToVirt(void *pAddr);
UINT32 IpcReadReg(const ULONG Addr);
void IpcWriteReg(const ULONG Addr, const UINT32 Value);
void IpcSPWriteBit(const ULONG Addr, const UINT32 bit);

#define ANSI_RED      "\033""[1;31m"
#define ANSI_GREEN    "\033""[1;32m"
#define ANSI_YELLOW   "\033""[1;33m"
#define ANSI_BLUE     "\033""[1;34m"
#define ANSI_MAGENTA  "\033""[1;35m"
#define ANSI_CYAN     "\033""[1;36m"
#define ANSI_WHITE    "\033""[1;37m"
#define ANSI_RESET    "\033""[0m"

#define VQ_ALLOC_RPC_RVQ            0U
#define VQ_ALLOC_RPC_SVQ            1U
#define VQ_ALLOC_MAX                2U

extern volatile UINT32 AmbaLink_Enable;
extern UINT32 AmbaLinkBootType;
extern UINT64 LinuxKernelStart;
extern AMBA_KAL_EVENT_FLAG_t AmbaLinkEventFlag;

/*---------------------------------------------------------------------------*\
 * Software IRQ used in AmbaLink system.
\*---------------------------------------------------------------------------*/
#if defined(AMBA_CORTEX_A53_AXI_CONFIG_PHYS_BASE_ADDR)
#define AXI_CONFIG_PHYS_BASE_ADDR AMBA_CORTEX_A53_AXI_CONFIG_PHYS_BASE_ADDR
#elif defined(AMBA_CORTEX_A53_AXI_CONFIG_BASE_ADDR)
#define AXI_CONFIG_PHYS_BASE_ADDR AMBA_CORTEX_A53_AXI_CONFIG_BASE_ADDR
#elif defined(AMBA_CORTEX_A76_AXI_CONFIG_PHYS_BASE_ADDR)
#define AXI_CONFIG_PHYS_BASE_ADDR AMBA_CORTEX_A76_AXI_CONFIG_PHYS_BASE_ADDR
#elif defined(AMBA_CORTEX_A76_AXI_CONFIG_BASE_ADDR)
#define AXI_CONFIG_PHYS_BASE_ADDR AMBA_CORTEX_A76_AXI_CONFIG_BASE_ADDR
#else
#error AXI not support
#endif

#if defined(AMBA_CORTEX_A53_SCRATCHPAD_NS_BASE_ADDR)
#define AHB_SCRATCHPAD_BASE             AMBA_CORTEX_A53_SCRATCHPAD_NS_BASE_ADDR
#elif defined(AMBA_CORTEX_A53_SCRATCHPAD_S_BASE_ADDR)
#define AHB_SCRATCHPAD_BASE             AMBA_CORTEX_A53_SCRATCHPAD_S_BASE_ADDR
#elif defined(AMBA_CORTEX_A53_AHB_SCRATCHPAD_BASE_ADDR)
#define AHB_SCRATCHPAD_BASE             AMBA_CORTEX_A53_AHB_SCRATCHPAD_BASE_ADDR
#elif defined(AMBA_CORTEX_A53_SCRATCHPAD_BASE_ADDR)
#define AHB_SCRATCHPAD_BASE             AMBA_CORTEX_A53_SCRATCHPAD_BASE_ADDR
#elif defined(AMBA_CA53_SCRATCHPAD_NS_BASE_ADDR)
#define AHB_SCRATCHPAD_BASE             AMBA_CA53_SCRATCHPAD_NS_BASE_ADDR
#elif defined(AMBA_CORTEX_A76_SCRATCHPAD_NS_BASE_ADDR)
#define AHB_SCRATCHPAD_BASE             AMBA_CORTEX_A76_SCRATCHPAD_NS_BASE_ADDR
#else
#error CONFIG_SOC_XXX
#endif

#if defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32)
#define AHB_SP_SWI_SET_OFFSET           0x64U
#define AHB_SP_SWI_CLEAR_OFFSET         0x68U
#define AHB_SP_SYNC                     0x78U
#define AXI_SOFT_IRQ0                   AMBA_INT_SPI_ID123_AXI_SOFT_IRQ0
#elif defined(CONFIG_SOC_CV2)
#define AHB_SP_SWI_SET_OFFSET           0x10U
#define AHB_SP_SWI_CLEAR_OFFSET         0x14U
#define AHB_SP_SYNC                     0x24U
#define AXI_SOFT_IRQ0                   AMBA_INT_SPI_ID123_AXI_SOFT_IRQ0
#elif defined(CONFIG_SOC_H22)
#define AHB_SP_SWI_SET_OFFSET           0x10U
#define AHB_SP_SWI_CLEAR_OFFSET         0x14U
#define AHB_SP_SYNC                     0x24U
#define AXI_SOFT_IRQ0                   AMBA_INT_SPI_ID39_SOFT_IRQ0
#elif defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#define AHB_SP_SWI_SET_OFFSET           0x64U
#define AHB_SP_SWI_CLEAR_OFFSET         0x68U
#define AHB_SP_SYNC                     0x78U
#define AXI_SOFT_IRQ0                   AMBA_INT_SPI_ID122_AXI_SOFT_IRQ0
#else
#error SCRATCHPAD not support
#endif

/*
 * C0: A53-RTOS, C1: Linux
 * For 3rd-OS on A53, C2: 3rd-OS
 * For R52/A53, they are mapped to C2/C1
 */
#define VRING_IRQ_C0_TO_C1_KICK         (AXI_SOFT_IRQ0)
#define VRING_IRQ_C0_TO_C1_ACK          (AXI_SOFT_IRQ0 + 1U)
#define VRING_IRQ_C1_TO_C0_KICK         (AXI_SOFT_IRQ0 + 2U)
#define VRING_IRQ_C1_TO_C0_ACK          (AXI_SOFT_IRQ0 + 3U)
//#define AMBALINK_AMP_SUSPEND_KICK       AXI_SOFT_IRQ(6)
#define VRING_IRQ_C1_TO_C2_KICK         (AXI_SOFT_IRQ0 + 7U)
#define VRING_IRQ_C1_TO_C2_ACK          (AXI_SOFT_IRQ0 + 8U)
#define VRING_IRQ_C2_TO_C1_KICK         (AXI_SOFT_IRQ0 + 9U)
#define VRING_IRQ_C2_TO_C1_ACK          (AXI_SOFT_IRQ0 + 10U)

#define AIPC_IRQ_C0_TO_C1_KICK          0U
#define AIPC_IRQ_C0_TO_C1_ACK           1U
#define AIPC_IRQ_C1_TO_C0_KICK          2U
#define AIPC_IRQ_C1_TO_C0_ACK           3U
#define AIPC_MUTEX_IRQ_RTOS             4U
#define AIPC_MUTEX_IRQ_LINUX            5U
#define AIPC_MUTEX_IRQ_R52              6U
#define AIPC_IRQ_C1_TO_C2_KICK          7U
#define AIPC_IRQ_C1_TO_C2_ACK           8U
#define AIPC_IRQ_C2_TO_C1_KICK          9U
#define AIPC_IRQ_C2_TO_C1_ACK           10U

#define PROFILE_TIMER    AMBA_TIMER6

void AmbaIPC_RpmsgInit(void);
void AmbaIPC_RpmsgSuspend(void);
void AmbaIPC_RpmsgRestoreData(void);
UINT32 AmbaIPC_RpmsgRestoreStatus(void);
void AmbaIPC_RpmsgRestoreEnable(UINT32 Enable);

/**
 * The procedure calls to binder.
 *
 */
typedef enum {
    AMBA_IPC_BINDER_BIND = 0,
    AMBA_IPC_BINDER_REGISTER,
    AMBA_IPC_BINDER_UNREGISTER,
    AMBA_IPC_BINDER_LIST,
    AMBA_IPC_BINDER_REBIND
} AMBA_IPC_BINDER_e;

typedef struct {
    INT32  prog;              /**< program number   */
    INT32  vers;              /**< version number   */
    INT32  proc;              /**< procedure number */
} AMBA_IPC_MSG_CALL_s;

typedef struct {
    AMBA_IPC_REPLY_STATUS_e  status;            /**< reply status  */
} AMBA_IPC_MSG_REPLY_s;

typedef struct {
    INT32  type;              /**< body type: call/reply */
    union {
        AMBA_IPC_MSG_CALL_s    call;    /**< call */
        AMBA_IPC_MSG_REPLY_s   reply;   /**< reply */
    } u;  /**< call or reply */
    /*ISO C forbids zero-size array*/
    /*UINT64 parameters[0];*/
} AMBA_IPC_MSG_s;

typedef struct {
    UINT8   client_addr;      /**< client address */
    UINT8   server_addr;      /**< server address */
    UINT32  xid;              /**< transaction ID */
    UINT32  client_port;      /**< client port    */
    UINT32  client_ctrl_port; /**< client control port */
    UINT32  server_port;      /**< server port    */
    UINT32  mode;             /**< communication mode */
    UINT64    private;          /**< private */
#ifdef RPC_DEBUG
    /** RPC profiling in ThreadX side */
    UINT32    tx_rpc_send_start;  /**< tx_rpc_send_start */
    UINT32    tx_rpc_send_end;    /**< tx_rpc_send_end */
    UINT32    tx_rpc_recv_start;  /**< tx_rpc_recv_start */
    UINT32    tx_rpc_recv_end;    /**< tx_rpc_recv_end */
    /** RPC profiling in Linux */
    UINT32    lk_to_lu_start;     /**< lk_to_lu_start */
    UINT32    lk_to_lu_end;       /**< lk_to_lu_end */
    UINT32    lu_to_lk_start;     /**< lu_to_lk_start */
    UINT32    lu_to_lk_end;       /**< lu_to_lk_end */
#else
    UINT32 dummy[8];
#endif

} AMBA_IPC_XPRT_s;

/* RPC_HDR should update if there is any updates to struct pkt */
typedef struct {
    AMBA_IPC_XPRT_s xprt;   /**< xprt */
    AMBA_IPC_MSG_s  msg;    /**< message */
} AMBA_IPC_PKT_s;

#define AMBA_IPC_HDRLEN     ((sizeof(AMBA_IPC_PKT_s)+3)&~3)

#define RPC_PARAM_SIZE      8

#define AMBA_IPC_BINDING_PORT               111
#define AMBA_IPC_CLNT_CONTROL_PORT          112
#define AMBA_IPC_CLIENT_NR_MAX              8

/**
 * The procedure calls to client controller.
 *
 */
typedef enum {
    AMBA_IPC_CLNT_REBIND = 0
} AMBA_IPC_CLNT_CTRL_e;

/*---------------------------------------------------------------------------*\
 * FDT related.
\*---------------------------------------------------------------------------*/
typedef struct {
    /** Base of kernel */
    UINT64  KernelBase;
    /** Size of kernel */
    UINT64  KernelSize;
    /** Shared memory base */
    UINT64  ShmBase;
    /** Shared memory szie */
    UINT64  ShmSize;
    /** Base of Private Physical Memory 2 (shared memory) */
    UINT64  Ppm2Base;
    /** Size of Private Physical Memory 2 (shared memory) */
    UINT64  Ppm2Size;
} LinuxMemoryInfo;

/**
 *  Data structure of AmbaLink Mem Pool.
 */
#define MEM_POOL_NAND_TMP_BUF         0U
#define MEM_POOL_EMMC_TMP_BUF         1U
#define MEM_POOL_SPI_NOR_TMP_BUF      2U
#define MEM_POOL_CRC_BUF              3U
#define MEM_POOL_VRING_BUF            4U
#define MEM_POOL_RPC_BUF              5U
#define MEM_POOL_COMM_BUF             6U
#define MEM_POOL_FS_DTA_BUF           7U
#define MEM_POOL_SVC_BUF              8U
#define MEM_POOL_PROC_INFO_BUF        9U
#define MEM_POOL_TPOOL_BUF           10U
#define MEM_POOL_TPOOL_THREAD_BUF    11U
#define MEM_POOL_TPOOL_QUEUE_BUF     12U
#define MEM_POOL_TPOOL_STACK_BUF     13U
#define MEM_POOL_CLIENT_BUF          14U
#define MEM_POOL_CLIENT_MODE_BUF     15U
#define MEM_POOL_TIMEOUT_RECORD_BUF  16U
#define MEM_POOL_IPC_BUF             17U

#ifdef CONFIG_OPENAMP
#define MEM_POOL_LIBMETAL_BUF        18U
#define MEM_POOL_MAX                 19U
#else
#define MEM_POOL_MAX                 18U
#endif // CONFIG_OPENAMP

typedef struct {
    UINT8       *pPtr;      /**< Pool base */
    UINT8       *pIndex;    /**< Pool index */
    UINT32      Number;     /**< Pool number */
    UINT32      Size;       /**< Pool size */
} AMBA_LINK_MEM_POOL_s;

void AmbaIPC_dump(void);
INT32 fdtput_spinor(void *fdt);
INT32 fdtput_nand(void *fdt);
INT32 fdtput_emmc(void *fdt);
INT32 fdtput_cmdline(void * fdt, const char * cmdline);
INT32 fdtput_memory(void * fdt, LinuxMemoryInfo *info);
INT32 fdtput_cvshm(void *fdt);
INT32 fdtput_shared_memory(const char *name, void *fdt, UINT64 base, UINT64 size, const char *method);
void AmbaLink_DtbInit(void);
void AmbaLink_Ascii2Unicode(const char *pAsciiStr, WCHAR *pUnicodeStr);
void AmbaLink_Unicode2Ascii(const WCHAR *pUnicodeStr, char *pAsciiStr);
void *AmbaLink_Malloc(UINT32 Num, UINT32 Size);
void AmbaLink_Free(UINT32 Num, void *pPtr);
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
void AmbaLink_SetBootFromA(int boot_a);
int AmbaLink_GetBootFromA(void);
void AmbaLink_SetLinuxConsoleEnable(int enable);
int AmbaLink_GetLinuxConsoleEnable(void);
void AmbaLink_SetEraseData(int reset);
#endif

#endif  /* AMBA_LINK_PRIVATE_H */
