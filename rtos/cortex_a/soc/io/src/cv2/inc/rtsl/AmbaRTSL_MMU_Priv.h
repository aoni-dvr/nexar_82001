/**
 *  @file AmbaRTSL_MMU_Priv.h
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
 *  @details Common definitions & constants for chip-dependent MMU Control APIs
 *
 */

#ifndef AMBA_RTSL_MMU_PRIV_H
#define AMBA_RTSL_MMU_PRIV_H

/*
 * MMU MMIO address region ID
 */
/* VMSAv8-64 or VMSAv8-32 long-descriptor */
#define AMBA_MMU_MMIO_REGION_NS_AHB_LONG        0x0U    /* Non-Secure AHB */
#define AMBA_MMU_MMIO_REGION_NS_APB_LONG        0x1U    /* Non-Secure AHB */
#define AMBA_MMU_MMIO_REGION_SECURE_AHB_LONG    0x2U    /* Secure AHB */
#define AMBA_MMU_MMIO_REGION_SECURE_APB_LONG    0x3U    /* Secure APB */
#define AMBA_MMU_MMIO_REGION_GPV_N0_LONG        0x4U    /* GPV N0 */
#define AMBA_MMU_MMIO_REGION_AXI_CONFIG_LONG    0x5U    /* AXI Config */
#define AMBA_MMU_MMIO_REGION_GIC_LONG           0x6U    /* GIC */
#define AMBA_MMU_MMIO_REGION_DRAMC_LONG         0x7U    /* DRAM & DDR Controller */
/* VMSAv8-32 short-descriptor */
#define AMBA_MMU_MMIO_REGION_NS_AHB_SHORT       0x0U    /* Non-Secure AHB */
#define AMBA_MMU_MMIO_REGION_NS_APB_SHORT       0x1U    /* Non-Secure APB */
#define AMBA_MMU_MMIO_REGION_SE_AHB_SHORT       0x2U    /* Secure AHB */
#define AMBA_MMU_MMIO_REGION_SE_APB_SHORT       0x3U    /* Secure APB */
#define AMBA_MMU_MMIO_REGION_GPV_N0_SHORT       0x4U    /* GPV N0 */
#define AMBA_MMU_MMIO_REGION_AXI_SHORT          0x5U    /* AXI Config */
#define AMBA_MMU_MMIO_REGION_GIC_SHORT          0x6U    /* GIC */
#define AMBA_MMU_MMIO_REGION_DRAMC_SHORT        0x7U    /* DRAM & DDR Controller */

#define AMBA_MMU_NUM_MMIO_REGION                0x8U

#ifdef CONFIG_ARM32
/**
 *  MMU_GetSecBusAttr - Get memory attribute for secure bus
 *  @return memory attribute
 */
static inline UINT32 MMU_GetSecBusAttr(void)
{
    UINT32 Attr;

    Attr = AMBA_MMU_MAP_ATTR_SHAREABLE(2U);
    Attr |= AMBA_MMU_MAP_ATTR_EXEC_NEVER(3U);
    Attr |= AMBA_MMU_MAP_ATTR_AP((0x3U << 2U) | 0x1U);

    return Attr;
}

/**
 *  MMU_MmioMapInit - Init structure pMmuMmioMap
 */
static inline void MMU_MmioMapInit(AMBA_MMU_MAP_INFO32_s *pMmuMmioMap)
{
    AMBA_MMU_MAP_INFO32_s *pMapTmp;

    /* VMSAv8-32 short-descriptors */
    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_NS_AHB_SHORT];
    pMapTmp->VirtAddr = AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_CORTEX_A53_NONSECURE_AHB_PHYS_BASE_ADDR;
    pMapTmp->MemSize = (AMBA_MMU_MAP_SECTION_SIZE << 6U);
    pMapTmp->MemAttr = MMU_GetSecBusAttr();
    pMapTmp->MemAttr |= AMBA_MMU_MAP_ATTR_SECTION;

    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_NS_APB_SHORT];
    pMapTmp->VirtAddr = AMBA_CORTEX_A53_NONSECURE_APB_VIRT_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_CORTEX_A53_NONSECURE_APB_PHYS_BASE_ADDR;
    pMapTmp->MemSize = (AMBA_MMU_MAP_SECTION_SIZE << 6U);
    pMapTmp->MemAttr = MMU_GetSecBusAttr();
    pMapTmp->MemAttr |= AMBA_MMU_MAP_ATTR_SECTION;

    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_SE_AHB_SHORT];
    pMapTmp->VirtAddr = AMBA_CORTEX_A53_SECURE_AHB_VIRT_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_CORTEX_A53_SECURE_AHB_PHYS_BASE_ADDR;
    pMapTmp->MemSize = (AMBA_MMU_MAP_SECTION_SIZE << 6U);
    pMapTmp->MemAttr = MMU_GetSecBusAttr();
    pMapTmp->MemAttr |= AMBA_MMU_MAP_ATTR_SECTION;

    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_SE_APB_SHORT];
    pMapTmp->VirtAddr = AMBA_CORTEX_A53_SECURE_APB_VIRT_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_CORTEX_A53_SECURE_APB_PHYS_BASE_ADDR;
    pMapTmp->MemSize = (AMBA_MMU_MAP_SECTION_SIZE << 6U);
    pMapTmp->MemAttr = MMU_GetSecBusAttr();
    pMapTmp->MemAttr |= AMBA_MMU_MAP_ATTR_SECTION;

    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_GPV_N0_SHORT];
    pMapTmp->VirtAddr = AMBA_CORTEX_A53_GPV_N0_VIRT_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_CORTEX_A53_GPV_N0_PHYS_BASE_ADDR;
    pMapTmp->MemSize = (AMBA_MMU_MAP_SECTION_SIZE << 1U);
    pMapTmp->MemAttr = MMU_GetSecBusAttr();
    pMapTmp->MemAttr |= AMBA_MMU_MAP_ATTR_SECTION;

    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_AXI_SHORT];
    pMapTmp->VirtAddr = AMBA_CORTEX_A53_AXI_CONFIG_VIRT_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_CORTEX_A53_AXI_CONFIG_PHYS_BASE_ADDR;
    pMapTmp->MemSize = ((UINT32)64U << 10U);
    pMapTmp->MemAttr = MMU_GetSecBusAttr();
    pMapTmp->MemAttr |= AMBA_MMU_MAP_ATTR_SMALL_PAGE;

    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_GIC_SHORT];
    pMapTmp->VirtAddr = AMBA_CORTEX_A53_GIC_VIRT_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_CORTEX_A53_GIC_PHYS_BASE_ADDR;
    pMapTmp->MemSize = (AMBA_MMU_MAP_SECTION_SIZE << 4U);
    pMapTmp->MemAttr = MMU_GetSecBusAttr();
    pMapTmp->MemAttr |= AMBA_MMU_MAP_ATTR_SECTION;

    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_DRAMC_SHORT];
    pMapTmp->VirtAddr = AMBA_CORTEX_A53_DRAM_CONFIG_VIRT_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_CORTEX_A53_DRAM_CONFIG_PHYS_BASE_ADDR;
    pMapTmp->MemSize = ((UINT32)64U << 10U);
    pMapTmp->MemAttr = MMU_GetSecBusAttr();
    pMapTmp->MemAttr |= AMBA_MMU_MAP_ATTR_SMALL_PAGE;

    (void)AmbaRTSL_CacheFlushDataPtr(pMmuMmioMap, AMBA_MMU_NUM_MMIO_REGION * sizeof(AMBA_MMU_MAP_INFO32_s));
}
#endif

#ifdef CONFIG_ARM64
/**
 *  MMU_GetSecBusAttr - Get memory attribute for secure bus
 *  @return memory attribute
 */
static UINT64 MMU_GetSecBusAttr(void)
{
    UINT32 Attr;

    Attr = AMBA_MMU_MAP_ATTR_SHAREABLE(2U);
    Attr |= AMBA_MMU_MAP_ATTR_EXEC_NEVER(3U);
    Attr |= AMBA_MMU_MAP_ATTR_AP((0x3U << 2U) | 0x1U);

    return (UINT64)Attr;
}

/**
 *  MMU_MmioMapInit - Init structure pMmuMmioMap
 */
static inline void MMU_MmioMapInit(AMBA_MMU_MAP_INFO64_s *pMmuMmioMap)
{
    AMBA_MMU_MAP_INFO64_s *pMapTmp;

    /* VMSAv8-64 or VMSAv8-32 long-descriptors */
    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_NS_AHB_LONG];
    pMapTmp->VirtAddr = AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_CORTEX_A53_NONSECURE_AHB_PHYS_BASE_ADDR;
    pMapTmp->MemSize = (UINT64)(AMBA_MMU_MAP_L2_BLOCK_SIZE << 5U);
    pMapTmp->MemAttr = MMU_GetSecBusAttr();
    pMapTmp->MemAttr |= (UINT64)AMBA_MMU_MAP_ATTR_L2_BLOCK;

    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_NS_APB_LONG];
    pMapTmp->VirtAddr = AMBA_CORTEX_A53_NONSECURE_APB_VIRT_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_CORTEX_A53_NONSECURE_APB_PHYS_BASE_ADDR;
    pMapTmp->MemSize = (UINT64)(AMBA_MMU_MAP_L2_BLOCK_SIZE << 5U);
    pMapTmp->MemAttr = MMU_GetSecBusAttr();
    pMapTmp->MemAttr |= (UINT64)AMBA_MMU_MAP_ATTR_L2_BLOCK;

    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_SECURE_AHB_LONG];
    pMapTmp->VirtAddr = AMBA_CORTEX_A53_SECURE_AHB_VIRT_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_CORTEX_A53_SECURE_AHB_PHYS_BASE_ADDR;
    pMapTmp->MemSize = (UINT64)(AMBA_MMU_MAP_L2_BLOCK_SIZE << 5U);
    pMapTmp->MemAttr = MMU_GetSecBusAttr();
    pMapTmp->MemAttr |= (UINT64)AMBA_MMU_MAP_ATTR_L2_BLOCK;

    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_SECURE_APB_LONG];
    pMapTmp->VirtAddr = AMBA_CORTEX_A53_SECURE_APB_VIRT_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_CORTEX_A53_SECURE_APB_PHYS_BASE_ADDR;
    pMapTmp->MemSize = (UINT64)(AMBA_MMU_MAP_L2_BLOCK_SIZE << 5U);
    pMapTmp->MemAttr = MMU_GetSecBusAttr();
    pMapTmp->MemAttr |= (UINT64)AMBA_MMU_MAP_ATTR_L2_BLOCK;

    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_GPV_N0_LONG];
    pMapTmp->VirtAddr = AMBA_CORTEX_A53_GPV_N0_VIRT_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_CORTEX_A53_GPV_N0_PHYS_BASE_ADDR;
    pMapTmp->MemSize = (UINT64)(AMBA_MMU_MAP_L2_BLOCK_SIZE);
    pMapTmp->MemAttr = MMU_GetSecBusAttr();
    pMapTmp->MemAttr |= (UINT64)AMBA_MMU_MAP_ATTR_L2_BLOCK;

    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_AXI_CONFIG_LONG];
    pMapTmp->VirtAddr = AMBA_CORTEX_A53_AXI_CONFIG_VIRT_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_CORTEX_A53_AXI_CONFIG_PHYS_BASE_ADDR;
    pMapTmp->MemSize = ((UINT64)64U << 10U);
    pMapTmp->MemAttr = MMU_GetSecBusAttr();
    pMapTmp->MemAttr |= (UINT64)AMBA_MMU_MAP_ATTR_L3_PAGE;

    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_GIC_LONG];
    pMapTmp->VirtAddr = AMBA_CORTEX_A53_GIC_VIRT_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_CORTEX_A53_GIC_PHYS_BASE_ADDR;
    pMapTmp->MemSize = (UINT64)(AMBA_MMU_MAP_L2_BLOCK_SIZE << 3);
    pMapTmp->MemAttr = MMU_GetSecBusAttr();
    pMapTmp->MemAttr |= (UINT64)AMBA_MMU_MAP_ATTR_L2_BLOCK;

    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_DRAMC_LONG];
    pMapTmp->VirtAddr = AMBA_CORTEX_A53_DRAM_CONFIG_VIRT_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_CORTEX_A53_DRAM_CONFIG_PHYS_BASE_ADDR;
    pMapTmp->MemSize = ((UINT64)64U << 10U);
    pMapTmp->MemAttr = MMU_GetSecBusAttr();
    pMapTmp->MemAttr |= (UINT64)AMBA_MMU_MAP_ATTR_L3_PAGE;

    (void)AmbaRTSL_CacheFlushDataPtr(pMmuMmioMap, AMBA_MMU_NUM_MMIO_REGION * (UINT32)sizeof(AMBA_MMU_MAP_INFO64_s));
}
#endif

#endif  /* AMBA_RTSL_MMU_PRIV_H */
