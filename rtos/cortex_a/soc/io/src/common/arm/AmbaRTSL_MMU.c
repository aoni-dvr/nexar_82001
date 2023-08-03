/**
 *  @file AmbaRTSL_MMU.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details Memory Management Unit Control APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"

#include "AmbaMMU.h"
#include "AmbaRTSL_MMU.h"
#include "AmbaRTSL_Cache.h"
#include "AmbaRTSL_CPU.h"
#include "AmbaWrap.h"

void AmbaRTSL_MmuSetupPrimaryFwprog(void);

/* MMU MMIO address mapping table */
static volatile UINT32 AmbaMmuInitCtrlFlag0 GNU_ALIGNED_CACHESAFE = 0xDEADBEAFU;
static volatile UINT32 AmbaMmuInitCtrlFlag1 GNU_ALIGNED_CACHESAFE = 0xDEADBEAFU;
static volatile UINT32 AmbaMmuInitCtrlFlag2 GNU_ALIGNED_CACHESAFE = 0xDEADBEAFU;
static volatile UINT32 AmbaMmuInitCtrlFlag3 GNU_ALIGNED_CACHESAFE = 0xDEADBEAFU;

/**
 *  MMU_Enable - Enable MMU
 */
#pragma GCC optimize ("O0")
static void MMU_Enable(void)
{
    UINT64 CpuExtCtrlReg;
    UINT32 SysCtrlReg;

#if !defined(CONFIG_CPU_CORTEX_A76)
#if !defined(CONFIG_XEN_SUPPORT) || defined(AMBA_FWPROG)
    /*
     * Set the SMPEN bit before enabling the data cache.
     * If you do not, then the cache is not coherent with other cores and data corruption could occur.
     */
    CpuExtCtrlReg = AmbaASM_ReadCpuExtCtrl();
    AmbaASM_WriteCpuExtCtrl((CpuExtCtrlReg | 0x40U));       /* Always set SMPEN */
#else
    /* XEN does not allow the touching of CPUEXTCTL */
#endif
#endif

    AmbaMisra_TouchUnused(&CpuExtCtrlReg);    /* for misraC checking, not necessary */

    /* Enable the MMU and data caching */
    SysCtrlReg = AmbaRTSL_CpuReadSysCtrl();
    SysCtrlReg |= ((UINT32)1U << 5U) | ((UINT32)1U);
    AmbaRTSL_CpuWriteSysCtrl(SysCtrlReg);

    /* Invalidate TLB if access flag enable of system control register is changed */
#if defined(CONFIG_ARM64)
    AmbaRTSL_MmuTlbInvalidateAll();
#else
    AmbaRTSL_MmuTlbInvalidateAllA32();
#endif

    /* Enable data caching */
    SysCtrlReg = AmbaRTSL_CpuReadSysCtrl();
    SysCtrlReg |= ((UINT32)1U << 12U) | ((UINT32)1U << 5U) | ((UINT32)1U << 2U) | ((UINT32)1U);
    AmbaRTSL_CpuWriteSysCtrl(SysCtrlReg);

    AmbaRTSL_CacheFlushDataAll();
    AMBA_DSB();
    AMBA_ISB();
}
#pragma GCC push_options

/**
 *  AmbaRTSL_MmuSetupPrimaryFwprog - Bring-up the primary processor and force to non-smp mode for FW programmer.
 */
void AmbaRTSL_MmuSetupPrimaryFwprog(void)
{
    UINT32 SysCtrlReg;

    /* Invalidate TLB and data cache */
    SysCtrlReg = AmbaRTSL_CpuReadSysCtrl();

    AmbaCache_InstInvAll();
    if (0x0U != (SysCtrlReg & 0x4U)) {      /* If data caching is enabled */
        AmbaRTSL_CacheFlushDataAll();       /* Push BSS and stack data to DRAM */
    } else {
        AmbaRTSL_CacheInvalDataAll();
    }
#if defined(CONFIG_ARM64)
    AmbaRTSL_MmuTlbInvalidateAll();
#else
    AmbaRTSL_MmuTlbInvalidateAllA32();
#endif

    AMBA_DSB();
    AMBA_ISB();

#if defined(CONFIG_ARM64)
    AmbaRTSL_MmuTlbConfig();
#else
    AmbaRTSL_MmuTlbConfigA32();
#endif

    AmbaMmuInitCtrlFlag0 = 0x4E4AU; // "NA"
    AmbaMmuInitCtrlFlag1 = 0x4E4AU; // "NA"
    AmbaMmuInitCtrlFlag2 = 0x4E4AU; // "NA"
    AmbaMmuInitCtrlFlag3 = 0x4E4AU; // "NA"

    MMU_Enable();
}

#if (defined(CONFIG_THREADX) && defined(CONFIG_ARM64))
void AmbaMMU_SetupPrimary(UINT32 NumSmpCores)
{
    UINT32 SysCtrlReg;
    volatile UINT32 *pAmbaMmuInitCtrlFlag;
    ULONG Addr;
    UINT32 RetVal = OK;
    /* Invalidate TLB and data cache */
    SysCtrlReg = AmbaRTSL_CpuReadSysCtrl();

    AmbaCache_InstInvAll();
    if (0x0U != (SysCtrlReg & 0x4U)) {      /* If data caching is enabled */
        AmbaRTSL_CacheFlushDataAll();       /* Push BSS and stack data to DRAM */
    } else {
        AmbaRTSL_CacheInvalDataAll();
    }
    AmbaRTSL_MmuTlbInvalidateAll();

    AMBA_DSB();
    AMBA_ISB();

    AmbaRTSL_MmuTlbConfig();

#ifndef AMBA_KAL_NO_SMP
    AmbaMmuInitCtrlFlag0 = 0x4F4BU; // "OK"
    AmbaMmuInitCtrlFlag1 = 0x4E4AU; // "NA"
    AmbaMmuInitCtrlFlag2 = 0x4E4AU; // "NA"
    AmbaMmuInitCtrlFlag3 = 0x4E4AU; // "NA"

    if (0x0U != (SysCtrlReg & 0x4U)) {      /* If data caching is enabled */
        pAmbaMmuInitCtrlFlag = &AmbaMmuInitCtrlFlag0;
        RetVal = AmbaWrap_memcpy(&Addr, &pAmbaMmuInitCtrlFlag, sizeof(Addr));
        if (RetVal == OK) {
            RetVal |= AmbaRTSL_CacheFlushData(Addr, CACHE_LINE_SIZE);
        }
        pAmbaMmuInitCtrlFlag = &AmbaMmuInitCtrlFlag1;
        RetVal = AmbaWrap_memcpy(&Addr, &pAmbaMmuInitCtrlFlag, sizeof(Addr));
        if (RetVal == OK) {
            RetVal |= AmbaRTSL_CacheFlushData(Addr, CACHE_LINE_SIZE);
        }
        pAmbaMmuInitCtrlFlag = &AmbaMmuInitCtrlFlag2;
        RetVal = AmbaWrap_memcpy(&Addr, &pAmbaMmuInitCtrlFlag, sizeof(Addr));
        if (RetVal == OK) {
            RetVal |= AmbaRTSL_CacheFlushData(Addr, CACHE_LINE_SIZE);
        }
        pAmbaMmuInitCtrlFlag = &AmbaMmuInitCtrlFlag3;
        RetVal = AmbaWrap_memcpy(&Addr, &pAmbaMmuInitCtrlFlag, sizeof(Addr));
        if (RetVal == OK) {
            RetVal |= AmbaRTSL_CacheFlushData(Addr, CACHE_LINE_SIZE);
        }
    } else {
        // for misraC checking, do nothing
    }

    if (RetVal == OK) {
        pAmbaMmuInitCtrlFlag = &AmbaMmuInitCtrlFlag1;
        RetVal |= AmbaWrap_memcpy(&Addr, &pAmbaMmuInitCtrlFlag, sizeof(Addr));
        while ((AmbaMmuInitCtrlFlag1 == 0x4E4AU) && (NumSmpCores >= 2U)) {
            if (0x0U != (SysCtrlReg & 0x4U)) {      /* If data caching is enabled */
                RetVal |= AmbaRTSL_CacheInvalData(Addr, CACHE_LINE_SIZE);
            } else {
                // for misraC checking, do nothing
            }
            continue;
        }
        pAmbaMmuInitCtrlFlag = &AmbaMmuInitCtrlFlag2;
        RetVal |= AmbaWrap_memcpy(&Addr, &pAmbaMmuInitCtrlFlag, sizeof(Addr));
        while ((AmbaMmuInitCtrlFlag2 == 0x4E4AU) && (NumSmpCores >= 3U)) {
            if (0x0U != (SysCtrlReg & 0x4U)) {      /* If data caching is enabled */
                RetVal |= AmbaRTSL_CacheInvalData(Addr, CACHE_LINE_SIZE);
            } else {
                // for misraC checking, do nothing
            }
            continue;
        }
        pAmbaMmuInitCtrlFlag = &AmbaMmuInitCtrlFlag3;
        RetVal |= AmbaWrap_memcpy(&Addr, &pAmbaMmuInitCtrlFlag, sizeof(Addr));
        while ((AmbaMmuInitCtrlFlag3 == 0x4E4AU) && (NumSmpCores >= 4U)) {
            if (0x0U != (SysCtrlReg & 0x4U)) {      /* If data caching is enabled */
                RetVal |= AmbaRTSL_CacheInvalData(Addr, CACHE_LINE_SIZE);
            } else {
                // for misraC checking, do nothing
            }
            continue;
        }
    }
#endif

    if (RetVal == OK ) {
        MMU_Enable();
    }
}

/**
 *  AmbaMMU_SetupPrimary - Bring-up the primary processor
 *  @param[in] NumSmpCores Number of SMP cores
 */
void AmbaMMU_SetupPrimaryPreOSInit(void)
{
    UINT32 SysCtrlReg;

    /* Invalidate TLB and data cache */
    SysCtrlReg = AmbaRTSL_CpuReadSysCtrl();

    AmbaCache_InstInvAll();
    if (0x0U != (SysCtrlReg & 0x4U)) {      /* If data caching is enabled */
        AmbaRTSL_CacheFlushDataAll();       /* Push BSS and stack data to DRAM */
    } else {
        AmbaRTSL_CacheInvalDataAll();
    }
    AmbaRTSL_MmuTlbInvalidateAll();

    AMBA_DSB();
    AMBA_ISB();

    AmbaRTSL_MmuTlbConfigPreOSInit();

    MMU_Enable();
}

#if defined(CONFIG_XEN_SUPPORT) && !defined(AMBA_FWPROG)
void AmbaMMU_SetupXenInit(void)
{
    UINT32 SysCtrlReg;

    /* Invalidate TLB and data cache */
    SysCtrlReg = AmbaRTSL_CpuReadSysCtrl();

    AmbaCache_InstInvAll();
    if (0x0U != (SysCtrlReg & 0x4U)) {      /* If data caching is enabled */
        AmbaRTSL_CacheFlushDataAll();       /* Push BSS and stack data to DRAM */
    } else {
        AmbaRTSL_CacheInvalDataAll();
    }
#if defined(CONFIG_ARM64)
    AmbaRTSL_MmuTlbInvalidateAll();
#else
    AmbaRTSL_MmuTlbInvalidateAllA32();
#endif

    AMBA_DSB();
    AMBA_ISB();

    AmbaRTSL_MmuTlbConfigXenInit();

    MMU_Enable();
}
#endif // defined(CONFIG_XEN_SUPPORT) && !defined(AMBA_FWPROG)

#else /* undefined CONFIG_TX64_TMP */
/**
 *  AmbaMMU_SetupPrimary - Bring-up the primary processor
 *  @param[in] NumSmpCores Number of SMP cores
 */
void AmbaMMU_SetupPrimary(UINT32 NumSmpCores)
{
    UINT32 SysCtrlReg;

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    AmbaMMU_Disable();
    AmbaRTSL_MmuTlbInvalidateAll();
#endif

    /* Invalidate TLB and data cache */
    SysCtrlReg = AmbaRTSL_CpuReadSysCtrl();

    AmbaCache_InstInvAll();
    if (0x0U != (SysCtrlReg & 0x4U)) {      /* If data caching is enabled */
        AmbaRTSL_CacheFlushDataAll();       /* Push BSS and stack data to DRAM */
    } else {
        AmbaRTSL_CacheInvalDataAll();
    }
#if defined(CONFIG_ARM64)
    AmbaRTSL_MmuTlbInvalidateAll();
#else
    AmbaRTSL_MmuTlbInvalidateAllA32();
#endif

    AMBA_DSB();
    AMBA_ISB();

#if defined(CONFIG_ARM64)
    AmbaRTSL_MmuTlbConfig();
#else
    AmbaRTSL_MmuTlbConfigA32();
#endif

#ifndef AMBA_KAL_NO_SMP
    AmbaMmuInitCtrlFlag0 = 0x4F4BU; // "OK"
    AmbaMmuInitCtrlFlag1 = 0x4E4AU; // "NA"
    AmbaMmuInitCtrlFlag2 = 0x4E4AU; // "NA"
    AmbaMmuInitCtrlFlag3 = 0x4E4AU; // "NA"

    /* sync point for SMP */
    if (NumSmpCores == 2U) {
        while (AmbaMmuInitCtrlFlag1 == 0x4E4AU) {
            continue;
        }
    } else if (NumSmpCores == 3U) {
        while (AmbaMmuInitCtrlFlag1 == 0x4E4AU) {
            continue;
        }
        while (AmbaMmuInitCtrlFlag2 == 0x4E4AU) {
            continue;
        }
    } else if (NumSmpCores >= 4U) {
        while (AmbaMmuInitCtrlFlag1 == 0x4E4AU) {
            continue;
        }
        while (AmbaMmuInitCtrlFlag2 == 0x4E4AU) {
            continue;
        }
        while (AmbaMmuInitCtrlFlag3 == 0x4E4AU) {
            continue;
        }
    } else {
        // for misraC checking, do nothing
    }
#endif

    MMU_Enable();
}

#endif  /* CONFIG_TX64_TMP */
/**
 *  AmbaMMU_SetupNonPrimaryPreSCU - Bring-up non-primary processor
 */
void AmbaMMU_SetupNonPrimaryPreSCU(void)
{
    UINT32 OldSysCtrlReg, NewSysCtrlReg;
    UINT32 CpuID = AmbaRTSL_CpuGetCoreID();

    OldSysCtrlReg = AmbaRTSL_CpuReadSysCtrl();

    if (0x0U != (OldSysCtrlReg & 0x4U)) {
        NewSysCtrlReg = OldSysCtrlReg & ~0x4U;
        AmbaRTSL_CacheFlushDataAll();               /* Push initialized data and stack data to DRAM */
        AmbaRTSL_CpuWriteSysCtrl(NewSysCtrlReg);    /* Disable data caching */
    }

    /* Wait for the translation table ready */
    switch (CpuID) {
    case 0U:
        while (AmbaMmuInitCtrlFlag0 == 0xDEADBEAFU) {
            continue;
        }
        break;
    case 1U:
        while (AmbaMmuInitCtrlFlag1 == 0xDEADBEAFU) {
            continue;
        }
        break;
    case 2U:
        while (AmbaMmuInitCtrlFlag2 == 0xDEADBEAFU) {
            continue;
        }
        break;
    case 3U:
        while (AmbaMmuInitCtrlFlag3 == 0xDEADBEAFU) {
            continue;
        }
        break;
    default:
        /* for misraC checking, do nothing */
        break;
    }

    /* Invalidate TLB and data cache */
    AmbaCache_InstInvAll();
    AmbaRTSL_CacheInvalDataAll();
#if defined(CONFIG_ARM64)
    AmbaRTSL_MmuTlbInvalidateAll();
#else
    AmbaRTSL_MmuTlbInvalidateAllA32();
#endif

    AMBA_DSB();
    AMBA_ISB();

#if defined(CONFIG_ARM64)
    if (AmbaMmuUserConfig.VectorAddr == 0x0U) {
        /* 0x00000000U: Low vector address */
        (void)AmbaRTSL_MmuCpuSetup(0U);
    } else {
        /* 0xFFFF0000U: High vector address */
        (void)AmbaRTSL_MmuCpuSetup(1U);
    }
#else
    if (AmbaMmuUserConfig.VectorAddr == 0x0U) {
        /* 0x00000000U: Low vector address */
        (void)AmbaRTSL_MmuCpuSetupA32(0U);
    } else {
        /* 0xFFFF0000U: High vector address */
        (void)AmbaRTSL_MmuCpuSetupA32(1U);
    }
#endif
    switch (CpuID) {
    case 0U:
        AmbaMmuInitCtrlFlag0 = 0x4F4BU;
        break;
    case 1U:
        AmbaMmuInitCtrlFlag1 = 0x4F4BU;
        break;
    case 2U:
        AmbaMmuInitCtrlFlag2 = 0x4F4BU;
        break;
    case 3U:
        AmbaMmuInitCtrlFlag3 = 0x4F4BU;
        break;
    default:
        /* for misraC checking, do nothing */
        break;
    }
}

/**
 *  AmbaMMU_SetupNonPrimaryPostSCU - Bring-up non-primary processor
 */
void AmbaMMU_SetupNonPrimaryPostSCU(void)
{
    MMU_Enable();
}

/**
 *  AmbaMMU_Disable - Disable MMU
 */
#pragma GCC optimize ("O0")
void AmbaMMU_Enable(void)
{
    UINT32 SysCtrlReg;

    /* Enable the MMU */
    SysCtrlReg = AmbaRTSL_CpuReadSysCtrl();
    SysCtrlReg |= ((UINT32)1U << 5U) | ((UINT32)1U);
    AmbaRTSL_CpuWriteSysCtrl(SysCtrlReg);

    /* Invalidate TLB if access flag enable of system control register is changed */
#if defined(CONFIG_ARM64)
    AmbaRTSL_MmuTlbInvalidateAll();
#else
    AmbaRTSL_MmuTlbInvalidateAllA32();
#endif

    /* Enable data caching */
    SysCtrlReg = AmbaRTSL_CpuReadSysCtrl();
    SysCtrlReg |= ((UINT32)1U << 12U) | ((UINT32)1U << 5U) | ((UINT32)1U << 2U) | ((UINT32)1U);
    AmbaRTSL_CpuWriteSysCtrl(SysCtrlReg);

    AmbaRTSL_CacheFlushDataAll();
#if defined(CONFIG_ARM64)
    AmbaRTSL_MmuTlbInvalidateAll();
#else
    AmbaRTSL_MmuTlbInvalidateAllA32();
#endif
    AMBA_DSB();
    AMBA_ISB();
}

void AmbaMMU_Disable(void)
{
    UINT32 SysCtrlReg;

    AmbaRTSL_CacheFlushDataAll();
#if defined(CONFIG_ARM64)
    AmbaRTSL_MmuTlbInvalidateAll();
#else
    AmbaRTSL_MmuTlbInvalidateAllA32();
#endif

    AMBA_DSB();
    AMBA_ISB();

    /* Disable the MMU */
    SysCtrlReg = AmbaRTSL_CpuReadSysCtrl();
    SysCtrlReg &= (~(((UINT32)1U << 12U) | ((UINT32)1U << 2U) | ((UINT32)1U)));

    AmbaRTSL_CpuWriteSysCtrl(SysCtrlReg);
    AmbaRTSL_CacheFlushDataAll();
#if defined(CONFIG_ARM64)
    AmbaRTSL_MmuTlbInvalidateAll();
#else
    AmbaRTSL_MmuTlbInvalidateAllA32();
#endif
    AMBA_DSB();
    AMBA_ISB();

}

void AmbaMMU_Disable_Training(void)
{
    UINT32 SysCtrlReg;

    AmbaRTSL_CacheFlushDataAll();
#if defined(CONFIG_ARM64)
    AmbaRTSL_MmuTlbInvalidateAll();
#else
    AmbaRTSL_MmuTlbInvalidateAllA32();
#endif

    AMBA_DSB();
    AMBA_ISB();

    /* Disable the Dcache and MMU */
    SysCtrlReg = AmbaRTSL_CpuReadSysCtrl();
    SysCtrlReg &= (~(((UINT32)1U << 2U) | ((UINT32)1U)));

    AmbaRTSL_CpuWriteSysCtrl(SysCtrlReg);
    AmbaRTSL_CacheFlushDataAll();
#if defined(CONFIG_ARM64)
    AmbaRTSL_MmuTlbInvalidateAll();
#else
    AmbaRTSL_MmuTlbInvalidateAllA32();
#endif
    AMBA_DSB();
    AMBA_ISB();

}

#pragma GCC push_options

