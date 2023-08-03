/**
 *  @file AmbaSYS_Def.h
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Constants and Definitions for System APIs
 *
 */

#ifndef AMBA_SYS_DEF_H
#define AMBA_SYS_DEF_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

#ifndef AMBA_SYS_PRIV_H
#include "AmbaSYS_Priv.h"
#endif

#define SYS_ERR_0000                    (SYS_ERR_BASE)          /* Invalid argument */
#define SYS_ERR_0001                    (SYS_ERR_BASE + 0x1U)   /* Not yet implemented */
#define SYS_ERR_0002                    (SYS_ERR_BASE + 0x2U)   /* Unable to do concurrency protection */
#define SYS_ERR_0003                    (SYS_ERR_BASE + 0x3U)   /* Not supported feature */
#define SYS_ERR_0004                    (SYS_ERR_BASE + 0x4U)   /* Invalid clk value */
#define SYS_ERR_00FF                    (SYS_ERR_BASE + 0xFFU)  /* Unexpected error */

/* SYS error values */
#define SYS_ERR_NONE                    OK
#define SYS_ERR_ARG                     SYS_ERR_0000
#define SYS_ERR_IMPL                    SYS_ERR_0001
#define SYS_ERR_MUTEX                   SYS_ERR_0002
#define SYS_ERR_SPRT                    SYS_ERR_0003
#define SYS_ERR_INVALIDCLK              SYS_ERR_0004
#define SYS_ERR_UNEXPECTED              SYS_ERR_00FF

/**
 * Defined in an external file: i.e. AmbaUserSysCtrl.c
 */
#define AARCH32_EXCP_UNDEFINED_INSTRUCTION      0U
#define AARCH32_EXCP_PREFETCH_ABORT             1U
#define AARCH32_EXCP_DATA_ABORT                 2U
#define AARCH32_NUM_EXCP                        3U

#define AARCH32_REG_CPSR                        1U
#define AARCH32_REG_PC                          0U
#define AARCH32_REG_LR                          8U
#define AARCH32_REG_SP                          7U
#define AARCH32_REG_IP                          6U
#define AARCH32_REG_FP                          5U
#define AARCH32_REG_R10                         4U
#define AARCH32_REG_R9                          3U
#define AARCH32_REG_R8                          2U
#define AARCH32_REG_R7                          16U
#define AARCH32_REG_R6                          15U
#define AARCH32_REG_R5                          14U
#define AARCH32_REG_R4                          13U
#define AARCH32_REG_R3                          12U
#define AARCH32_REG_R2                          11U
#define AARCH32_REG_R1                          10U
#define AARCH32_REG_R0                          9U

#define AARCH64_EXCP_SYNC_ERR_EL0               0U
#define AARCH64_EXCP_SYSTEM_ERROR_EL0           1U
#define AARCH64_EXCP_SYNC_ERR_ELX               2U
#define AARCH64_EXCP_SYSTEM_ERROR_ELX           3U
#define AARCH64_NUM_EXCP                        4U

#define AARCH64_REG_CPSR                        1U
#define AARCH64_REG_PC                          0U
#define AARCH64_REG_SP                          3U
#define AARCH64_REG_X00                         2U
#define AARCH64_REG_X01                         4U
#define AARCH64_REG_X02                         5U
#define AARCH64_REG_X03                         6U
#define AARCH64_REG_X04                         7U
#define AARCH64_REG_X05                         8U
#define AARCH64_REG_X06                         9U
#define AARCH64_REG_X07                         10U
#define AARCH64_REG_X08                         11U
#define AARCH64_REG_X09                         12U
#define AARCH64_REG_X10                         13U
#define AARCH64_REG_X11                         14U
#define AARCH64_REG_X12                         15U
#define AARCH64_REG_X13                         16U
#define AARCH64_REG_X14                         17U
#define AARCH64_REG_X15                         18U
#define AARCH64_REG_X16                         19U
#define AARCH64_REG_X17                         20U
#define AARCH64_REG_X18                         21U
#define AARCH64_REG_X19                         22U
#define AARCH64_REG_X20                         23U
#define AARCH64_REG_X21                         24U
#define AARCH64_REG_X22                         25U
#define AARCH64_REG_X23                         26U
#define AARCH64_REG_X24                         27U
#define AARCH64_REG_X25                         28U
#define AARCH64_REG_X26                         29U
#define AARCH64_REG_X27                         30U
#define AARCH64_REG_X28                         31U
#define AARCH64_REG_X29                         32U
#define AARCH64_REG_LR                          33U

/**
 * Module ID for system logger.
 */
#define SYS_LOG_KAL_CPU_LOAD            (KAL_ERR_BASE)
#define SYS_LOG_KAL_TASK                (KAL_ERR_BASE + 0x1U)
#define SYS_LOG_KAL_MUTEX               (KAL_ERR_BASE + 0x2U)
#define SYS_LOG_KAL_SEMAPHORE           (KAL_ERR_BASE + 0x3U)
#define SYS_LOG_KAL_EVENTFLAGS          (KAL_ERR_BASE + 0x4U)
#define SYS_LOG_KAL_MSG_QUEUE           (KAL_ERR_BASE + 0x5U)

#define SYS_LOG_DRAM                    (DRAM_ERR_BASE)
#define SYS_LOG_INT                     (INT_ERR_BASE)
#define SYS_LOG_VOUT                    (VOUT_ERR_BASE)

#define SYS_LOG_VIN                     (VIN_ERR_BASE)
#define SYS_LOG_PIP                     (VIN_ERR_BASE + 0x1U)
#define SYS_LOG_PIP2                    (VIN_ERR_BASE + 0x2U)
#define SYS_LOG_PIP3                    (VIN_ERR_BASE + 0x3U)
#define SYS_LOG_PIP4                    (VIN_ERR_BASE + 0x4U)
#define SYS_LOG_PIP5                    (VIN_ERR_BASE + 0x5U)
#define SYS_LOG_PIP6                    (VIN_ERR_BASE + 0x6U)
#define SYS_LOG_PIP7                    (VIN_ERR_BASE + 0x7U)
#define SYS_LOG_PIP8                    (VIN_ERR_BASE + 0x8U)
#define SYS_LOG_PIP9                    (VIN_ERR_BASE + 0x9U)
#define SYS_LOG_PIP10                   (VIN_ERR_BASE + 0xAU)
#define SYS_LOG_PIP11                   (VIN_ERR_BASE + 0xBU)
#define SYS_LOG_PIP12                   (VIN_ERR_BASE + 0xCU)
#define SYS_LOG_PIP13                   (VIN_ERR_BASE + 0xDU)


/**
 * Function ID for system logger.
 *
 * 0-255 are reserved for general functions
 * 256-65535 are reserved for module-specific functions
 */
#define SYS_LOG_FUNC_NORMAL             0U
#define SYS_LOG_FUNC_RESET              1U
#define SYS_LOG_FUNC_INIT               2U
#define SYS_LOG_FUNC_RAW                3U
#define SYS_LOG_FUNC_CUSTOM             256U

typedef void (*AMBA_SYS_LOG_f)(const char *String);

typedef void (*AMBA_SYS_USER_ENTRY_f)(void);
typedef void (*AMBA_SYS_USER_EXCEPTION_f)(UINT32 ExceptionID, ULONG * pSavedRegs);

typedef struct {
    AMBA_SYS_USER_ENTRY_f UserEntryBefOS;
    AMBA_SYS_USER_ENTRY_f UserEntryAftOS;
    AMBA_SYS_USER_ENTRY_f UserIdleCallback;
    AMBA_SYS_USER_EXCEPTION_f UserExceptionHandler;
} AMBA_SYS_USER_CALLBACKS_s;

#endif  /* AMBA_SYS_DEF_H */
