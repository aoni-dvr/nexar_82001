/**
 *  @file AmbaFlashFwProg_MmuMemMap.c
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
 *  @details MMU definitions for NAND flash firmware program utilities
 */

#include "AmbaTypes.h"
#include "AmbaMemMap.h"
#include "AmbaMisraFix.h"
#ifdef CONFIG_CPU_CORTEX_A76
#include "AmbaCortexA76.h"
#else
#include "AmbaCortexA53.h"
#endif
#include "AmbaMMU_Def.h"
#include "AmbaRTSL_Cache.h"

/*-----------------------------------------------------------------------------------------------*\
 * MMU DRAM memory region ID
\*-----------------------------------------------------------------------------------------------*/
typedef enum {
    /* VMSAv8-64 or VMSAv8-32 long-descriptor */
    AMBA_MMU_RAM_REGION_CACHEABLE_FLAT_LONG,    /* Cacheable Memory Region. Mapped addresses are same as physical addresses.  */
#ifdef CONFIG_CPU_CORTEX_A53
    AMBA_MMU_RAM_REGION_CACHEABLE_ALT_LONG,     /* Cacheable Memory Region. Mapped addresses are same as/different to physical addresses. */
    /* VMSAv8-32 short-descriptor */
    AMBA_MMU_RAM_REGION_CACHEABLE_FLAT_SHORT,   /* Cacheable Memory Region. Mapped addresses are same as physical addresses.  */
    AMBA_MMU_RAM_REGION_CACHEABLE_ALT_SHORT,    /* Cacheable Memory Region. Mapped addresses are same as/different to physical addresses. */
#endif

    AMBA_MMU_RAM_REGION_RSVD,                   /* Memory reserved region */

    AMBA_MMU_NUM_RAM_REGION                     /* Number of mapped DRAM memory regions */
} AMBA_MMU_RAM_REGION_e;

/*-----------------------------------------------------------------------------------------------*\
 * MMU memory map table.
\*-----------------------------------------------------------------------------------------------*/
static AMBA_MMU_MAP_INFO64_s MMU_RamMapInfo[AMBA_MMU_NUM_RAM_REGION];

static UINT32 FwProg_MemMapInit(void);

AMBA_MMU_USER_CONFIG64_s AmbaMmuUserConfig = {
    .UserPreConfigEntry = FwProg_MemMapInit,               /* pointer to the init function of Memory Map */
    .NumMemMap          = AMBA_MMU_NUM_RAM_REGION,      /* number of Memory Map regions */
    .pMemMap            = MMU_RamMapInfo,               /* pointer to the base address of Memory Map Info */
    .VectorAddr         = 0 /* Exception vector address */
};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FwProg_MemMapInit
 *
 *  @Description:: Init structure MMU_RamMapInfo
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 FwProg_MemMapInit(void)
{
#ifdef __aarch64__
    static AMBA_MMU_MAP_INFO64_s *pMapTmp;
#else
    static AMBA_MMU_MAP_INFO32_s *pMapTmp;
#endif

    pMapTmp = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_CACHEABLE_FLAT_LONG];
#ifdef CONFIG_CPU_CORTEX_A76
    pMapTmp->VirtAddr = (AMBA_CORTEX_A76_DRAM_VIRT_BASE_ADDR);
    pMapTmp->PhysAddr = (AMBA_CORTEX_A76_DRAM_PHYS_BASE_ADDR);
    pMapTmp->MemSize = (1U << 30U);
    pMapTmp->MemAttr = (AMBA_MMU_MAP_ATTR_L2_BLOCK | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR);
#else // A53
    pMapTmp->VirtAddr = (AMBA_CORTEX_A53_DRAM_PHYS_BASE_ADDR);
    pMapTmp->PhysAddr = (AMBA_CORTEX_A53_DRAM_PHYS_BASE_ADDR);
    pMapTmp->MemSize = (1U << 30U);
    pMapTmp->MemAttr = (AMBA_MMU_MAP_ATTR_L1_BLOCK | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR);

    pMapTmp = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_CACHEABLE_ALT_LONG];
    pMapTmp->VirtAddr = (AMBA_CORTEX_A53_DRAM_VIRT_BASE_ADDR);
    pMapTmp->PhysAddr = (AMBA_CORTEX_A53_DRAM_PHYS_BASE_ADDR);
    pMapTmp->MemSize = (1U << 30U);
    pMapTmp->MemAttr = (AMBA_MMU_MAP_ATTR_L1_BLOCK | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR);

    pMapTmp = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_CACHEABLE_FLAT_SHORT];
    pMapTmp->VirtAddr = (AMBA_CORTEX_A53_DRAM_PHYS_BASE_ADDR);
    pMapTmp->PhysAddr = (AMBA_CORTEX_A53_DRAM_PHYS_BASE_ADDR);
    pMapTmp->MemSize = (1U << 30U);
    pMapTmp->MemAttr = (AMBA_MMU_MAP_ATTR_SECTION | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR);

    pMapTmp = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_CACHEABLE_ALT_SHORT];
    pMapTmp->VirtAddr = (AMBA_CORTEX_A53_DRAM_VIRT_BASE_ADDR);
    pMapTmp->PhysAddr = (AMBA_CORTEX_A53_DRAM_PHYS_BASE_ADDR);
    pMapTmp->MemSize = (1U << 30U);
    pMapTmp->MemAttr = (AMBA_MMU_MAP_ATTR_SECTION | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR);
#endif // CONFIG_CPU_CORTEX_A76 or CONFIG_CPU_CORTEX_A53

    pMapTmp = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_RSVD];
    pMapTmp->VirtAddr = (AMBA_DRAM_RESERVED_VIRT_ADDR);
    pMapTmp->PhysAddr = (0x00000000U);
    pMapTmp->MemSize = (AMBA_DRAM_RESERVED_SIZE);
    pMapTmp->MemAttr = (AMBA_MMU_MAP_ATTR_L3_PAGE  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR);

#ifdef __aarch64__
    (void)AmbaRTSL_CacheFlushDataPtr(MMU_RamMapInfo, sizeof(AMBA_MMU_MAP_INFO64_s));
#else
    (void)AmbaRTSL_CacheFlushDataPtr(MMU_RamMapInfo, AMBA_MMU_NUM_RAM_REGION * sizeof(AMBA_MMU_MAP_INFO32_s));
#endif

    return OK;
}

