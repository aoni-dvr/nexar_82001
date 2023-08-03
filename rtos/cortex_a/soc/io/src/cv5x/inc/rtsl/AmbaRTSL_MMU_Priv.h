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
/* VMSAv8-64 */
#define AMBA_MMU_MMIO_REGION_AHB_LONG           0x0U    /* AHB */
#define AMBA_MMU_MMIO_REGION_APB_LONG           0x1U    /* APB */
#define AMBA_MMU_MMIO_REGION_GPV_A_LONG         0x2U    /* GPV A */
#define AMBA_MMU_MMIO_REGION_GPV_B_LONG         0x3U    /* GPV B */
#define AMBA_MMU_MMIO_REGION_AXI_CONFIG_LONG    0x4U    /* AXI Config */
#define AMBA_MMU_MMIO_REGION_GIC_LONG           0x5U    /* GIC */
#define AMBA_MMU_MMIO_REGION_DRAMC_LONG         0x6U    /* DRAM & DDR Controller */
#define AMBA_MMU_MMIO_REGION_DEBUG_PORT         0x7U    /* DEBUG PORT */
#define AMBA_MMU_MMIO_REGION_USB32_CTRL         0x8U    /* USB32 Controller */
#define AMBA_MMU_MMIO_REGION_USB32_PHY          0x9U    /* USB32 PHY */
#define AMBA_MMU_MMIO_REGION_PCIE_CTRL          0xAU    /* PCIE Controller */
#define AMBA_MMU_MMIO_REGION_PCIE_PHY           0xBU    /* PCIE PHY */
#define AMBA_MMU_MMIO_REGION_PCIE_CFG           0xCU    /* PCIE Config */
#define AMBA_MMU_MMIO_REGION_CPHY               0xDU    /* CPHY Config */
#define AMBA_MMU_MMIO_REGION_SMEM               0xEU    /* SMEM Config */
#define AMBA_MMU_NUM_MMIO_REGION                0xFU

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
 *  MMU_GetSmemAttr - Get memory attribute for FIFO
 *  @return memory attribute
 */
static UINT64 MMU_GetSmemAttr(void)
{
    UINT32 Attr;

    Attr = AMBA_MMU_MAP_ATTR_CACHE_WRITE_BACK;
    //Attr |= AMBA_MMU_MAP_ATTR_NON_SECURE(1U);
#ifdef CONFIG_ATF_HAVE_BL2
    Attr |= AMBA_MMU_MAP_ATTR_AP((0x3U << 2U) | 0x0U);
#else
    Attr |= AMBA_MMU_MAP_ATTR_AP((0x3U << 2U) | 0x1U);
#endif

    return (UINT64)Attr;
}

/**
 *  MMU_MmioMapInit - Init structure pMmuMmioMap
 */
static inline void MMU_MmioMapInit(AMBA_MMU_MAP_INFO64_s *pMmuMmioMap)
{
    AMBA_MMU_MAP_INFO64_s *pMapTmp;

    /* VMSAv8-64 or VMSAv8-32 long-descriptors */
    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_AHB_LONG];
    pMapTmp->VirtAddr = AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_CORTEX_A76_AHB_PHYS_BASE_ADDR;
    pMapTmp->MemSize = (UINT64)(AMBA_MMU_MAP_L2_BLOCK_SIZE << 5U);
    pMapTmp->MemAttr = MMU_GetSecBusAttr();
    pMapTmp->MemAttr |= (UINT64)AMBA_MMU_MAP_ATTR_L2_BLOCK;

    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_APB_LONG];
    pMapTmp->VirtAddr = AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_CORTEX_A76_APB_PHYS_BASE_ADDR;
    pMapTmp->MemSize = (UINT64)(AMBA_MMU_MAP_L2_BLOCK_SIZE << 5U);
    pMapTmp->MemAttr = MMU_GetSecBusAttr();
    pMapTmp->MemAttr |= (UINT64)AMBA_MMU_MAP_ATTR_L2_BLOCK;

    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_GPV_A_LONG];
    pMapTmp->VirtAddr = AMBA_CORTEX_A76_GPV_A_VIRT_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_CORTEX_A76_GPV_A_PHYS_BASE_ADDR;
    pMapTmp->MemSize = (UINT64)(AMBA_MMU_MAP_L2_BLOCK_SIZE);
    pMapTmp->MemAttr = MMU_GetSecBusAttr();
    pMapTmp->MemAttr |= (UINT64)AMBA_MMU_MAP_ATTR_L2_BLOCK;

    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_GPV_B_LONG];
    pMapTmp->VirtAddr = AMBA_CORTEX_A76_GPV_B_VIRT_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_CORTEX_A76_GPV_B_PHYS_BASE_ADDR;
    pMapTmp->MemSize = (UINT64)(AMBA_MMU_MAP_L2_BLOCK_SIZE);
    pMapTmp->MemAttr = MMU_GetSecBusAttr();
    pMapTmp->MemAttr |= (UINT64)AMBA_MMU_MAP_ATTR_L2_BLOCK;

    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_AXI_CONFIG_LONG];
    pMapTmp->VirtAddr = AMBA_CORTEX_A76_AXI_CONFIG_VIRT_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_CORTEX_A76_AXI_CONFIG_PHYS_BASE_ADDR;
    pMapTmp->MemSize = 0x200000UL;
    pMapTmp->MemAttr = MMU_GetSecBusAttr();
    pMapTmp->MemAttr |= (UINT64)AMBA_MMU_MAP_ATTR_L2_BLOCK;

    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_GIC_LONG];
    pMapTmp->VirtAddr = AMBA_CORTEX_A76_GIC_VIRT_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_CORTEX_A76_GIC_PHYS_BASE_ADDR;
    pMapTmp->MemSize = (UINT64)(AMBA_MMU_MAP_L2_BLOCK_SIZE << 3);
    pMapTmp->MemAttr = MMU_GetSecBusAttr();
    pMapTmp->MemAttr |= (UINT64)AMBA_MMU_MAP_ATTR_L2_BLOCK;

    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_DRAMC_LONG];
    pMapTmp->VirtAddr = AMBA_CORTEX_A76_DRAM_CONFIG_VIRT_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_CORTEX_A76_DRAM_CONFIG_PHYS_BASE_ADDR;
    pMapTmp->MemSize = 0x200000UL;
    pMapTmp->MemAttr = MMU_GetSecBusAttr();
    pMapTmp->MemAttr |= (UINT64)AMBA_MMU_MAP_ATTR_L2_BLOCK;

    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_DEBUG_PORT];
    pMapTmp->VirtAddr = AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR;
    pMapTmp->MemSize = 0x1000000U;
    pMapTmp->MemAttr = MMU_GetSecBusAttr();
    pMapTmp->MemAttr |= (UINT64)AMBA_MMU_MAP_ATTR_L2_BLOCK;

    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_SMEM];
    pMapTmp->VirtAddr = AMBA_DBG_PORT_FMEM0_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_DBG_PORT_FMEM0_BASE_ADDR;
    pMapTmp->MemSize = 0x1000000U;
    pMapTmp->MemAttr = MMU_GetSmemAttr();
    pMapTmp->MemAttr |= (UINT64)AMBA_MMU_MAP_ATTR_L2_BLOCK;

    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_USB32_CTRL];
    pMapTmp->VirtAddr = AMBA_CORTEX_A76_USB32_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_CORTEX_A76_USB32_BASE_ADDR;
    pMapTmp->MemSize = 0x10000000U; // 256MB
    pMapTmp->MemAttr = MMU_GetSecBusAttr();
    pMapTmp->MemAttr |= (UINT64)AMBA_MMU_MAP_ATTR_L2_BLOCK;

    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_USB32_PHY];
    pMapTmp->VirtAddr = AMBA_CORTEX_A76_USB32_PHY_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_CORTEX_A76_USB32_PHY_BASE_ADDR;
    pMapTmp->MemSize = 0x10000000U; // 256MB
    pMapTmp->MemAttr = MMU_GetSecBusAttr();
    pMapTmp->MemAttr |= (UINT64)AMBA_MMU_MAP_ATTR_L2_BLOCK;

    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_PCIE_CTRL];
    pMapTmp->VirtAddr = AMBA_CORTEX_A76_PCIE_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_CORTEX_A76_PCIE_BASE_ADDR;
    pMapTmp->MemSize = 0x10000000U; // 256MB
    pMapTmp->MemAttr = MMU_GetSecBusAttr();
    pMapTmp->MemAttr |= (UINT64)AMBA_MMU_MAP_ATTR_L2_BLOCK;

    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_PCIE_PHY];
    pMapTmp->VirtAddr = AMBA_CORTEX_A76_PCIE_PHY_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_CORTEX_A76_PCIE_PHY_BASE_ADDR;
    pMapTmp->MemSize = 0x10000000U; // 256MB
    pMapTmp->MemAttr = MMU_GetSecBusAttr();
    pMapTmp->MemAttr |= (UINT64)AMBA_MMU_MAP_ATTR_L2_BLOCK;

    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_PCIE_CFG];
    pMapTmp->VirtAddr = AMBA_CORTEX_A76_PCIE_CONFIG_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_CORTEX_A76_PCIE_CONFIG_BASE_ADDR;
    pMapTmp->MemSize = 0x10000000U; // 256MB
    pMapTmp->MemAttr = MMU_GetSecBusAttr();
    pMapTmp->MemAttr |= (UINT64)AMBA_MMU_MAP_ATTR_L2_BLOCK;

    pMapTmp = &pMmuMmioMap[AMBA_MMU_MMIO_REGION_CPHY];
    pMapTmp->VirtAddr = AMBA_CORTEX_A76_CPHY_BASE_ADDR;
    pMapTmp->PhysAddr = AMBA_CORTEX_A76_CPHY_BASE_ADDR;
    pMapTmp->MemSize = 0x10000000U; // 256MB
    pMapTmp->MemAttr = MMU_GetSecBusAttr();
    pMapTmp->MemAttr |= (UINT64)AMBA_MMU_MAP_ATTR_L2_BLOCK;

    (void)AmbaRTSL_CacheFlushDataPtr(pMmuMmioMap, AMBA_MMU_NUM_MMIO_REGION * sizeof(AMBA_MMU_MAP_INFO64_s));
}
#endif


#endif  /* AMBA_RTSL_MMU_PRIV_H */
