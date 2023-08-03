/**
 *  @file AmbaDRAMC_Def.h
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
 *  @details Definitions & Constants for DRAM controller APIs
 *
 */

#ifndef AMBA_DRAMC_DEF_H
#define AMBA_DRAMC_DEF_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

#ifndef AMBA_DRAMC_PRIV_H
#include "AmbaDRAMC_Priv.h"
#endif

/* DRAM error values */
#define DRAMC_ERR_0000          (DRAM_ERR_BASE)             /* Invalid argument */
#define DRAMC_ERR_0001          (DRAM_ERR_BASE + 1U)        /* Unexpected error */
#define DRAMC_ERR_0002          (DRAM_ERR_BASE + 2U)        /* Address translation error */
#define DRAMC_ERR_0003          (DRAM_ERR_BASE + 3U)        /* Address translation error */
#define DRAMC_ERR_0004          (DRAM_ERR_BASE + 4U)

#define DRAMC_ERR_NONE          OK
#define DRAMC_ERR_ARG           DRAMC_ERR_0000
#define DRAMC_ERR_UNEXPECTED    DRAMC_ERR_0001
#define DRAMC_ERR_AT            DRAMC_ERR_0002
#define DRAMC_ERR_MUTEX         DRAMC_ERR_0003
#define DRAMC_ERR_IO            DRAMC_ERR_0004

/**
 * Definitions for DRAM Address Translation Table (ATT)
 */
#define AMBA_DRAM_ATT_MAP_SIZE          (CONFIG_DDR_SIZE)
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#define AMBA_DRAM_ATT_PAGE_SIZE         (CONFIG_DDR_SIZE >> 14U)
#define AMBA_DRAM_ATT_REG_OFFSET        (16U)
#else
#define AMBA_DRAM_ATT_PAGE_SIZE         (1U << 18U)           /* divided into 256KB pages */
#define AMBA_DRAM_ATT_REG_OFFSET        (14U)
#endif
#define AMBA_DRAM_ATT_LAST_PAGE_ID      (((UINT32)1U << 14U) - 1U)   /* = ((AMBA_DRAM_ATT_MAP_SIZE / AMBA_DRAM_ATT_PAGE_SIZE) - 1UL) */

typedef struct {
    ULONG VirtAddr;                    /* virtaul base address to be translated (must be page size aligned) */
    ULONG PhysAddr;                    /* physical base address (must be page size aligned) */
    ULONG Size;                        /* number of related addresses defined by this translation rule (must be a multiple of page size) */
} AMBA_DRAM_ATT_INFO_s;

typedef struct {
    UINT32 ClientID;
    ULONG VirtAddr;
    ULONG Size;
} AMBA_DRAM_ATT_CLIENT_INFO_s;

typedef struct {
    UINT32  ClientRequestStatis[32];    /* Statistics for each client's number of requests */
    UINT32  ClientBurstStatis[32];      /* Statistics for each client's number of bursts */
    UINT32  ClientMaskWriteStatis[32];  /* Statistics for each client's number of masked write bursts */
} AMBA_DRAMC_STATIS_s;

#endif /* AMBA_DRAMC_DEF_H */
