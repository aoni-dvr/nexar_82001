/**
 *  @file AmbaBLD_MmuMap.c
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
 *  @details Mapping the physical address of RAM to logical address with caching/uncaching attribute
 *
 */

#include "AmbaTypes.h"
#include "AmbaCortexA53.h"

#include "AmbaMMU.h"
#include "AmbaRTSL_Cache.h"

static UINT32 BLD_MemMapInit(void);

/* MMU DRAM memory region ID */
#define AMBA_MMU_RAM_REGION_FLAT            0x0U    /* Cacheable Memory Region. Mapped addresses are same as physical addresses.  */
#define AMBA_MMU_RAM_REGION_FIO_FIFO        0x1U    /* FIFO region used by FIO */
#define AMBA_MMU_RAM_REGION_RSVD            0x2U    /* Memory reserved region */
#define AMBA_MMU_NUM_RAM_REGION             0x3U    /* Number of mapped DRAM memory regions */

/* MMU memory map table. */
#ifdef CONFIG_ARM64
static AMBA_MMU_MAP_INFO64_s MMU_RamMapInfo[AMBA_MMU_NUM_RAM_REGION];

AMBA_MMU_USER_CONFIG64_s AmbaMmuUserConfig = {
    .UserPreConfigEntry = BLD_MemMapInit,
    .NumMemMap          = AMBA_MMU_NUM_RAM_REGION,
    .pMemMap            = MMU_RamMapInfo,
    .VectorAddr         = 0x0ULL
};

/**
 *  MMU_GetMemRwAttr - Get memory attribute for Read-write memory
 *  @return memory attribute
 */
static UINT64 MMU_GetMemRwAttr(void)
{
    UINT32 Attr;

    Attr = AMBA_MMU_MAP_ATTR_CACHE_WRITE_BACK;
    Attr |= AMBA_MMU_MAP_ATTR_NON_SECURE(1U);
#ifdef CONFIG_ATF_HAVE_BL2
    Attr |= AMBA_MMU_MAP_ATTR_AP((0x3U << 2U) | 0x0U);
#else
    Attr |= AMBA_MMU_MAP_ATTR_AP((0x3U << 2U) | 0x1U);
#endif

    return (UINT64)Attr;
}

/**
 *  MMU_GetFifoAttr - Get memory attribute for FIFO
 *  @return memory attribute
 */
static UINT64 MMU_GetFifoAttr(void)
{
    UINT32 Attr;

    Attr = AMBA_MMU_MAP_ATTR_STRONGLY_ORDERED;
#ifdef CONFIG_ATF_HAVE_BL2
    Attr |= AMBA_MMU_MAP_ATTR_AP((0x3U << 2U) | 0x0U);
#else
    Attr |= AMBA_MMU_MAP_ATTR_AP((0x3U << 2U) | 0x1U);
#endif

    return (UINT64)Attr;
}

/**
 *  BLD_MemMapInit - Init structure MMU_RamMapInfo
 */
static UINT32 BLD_MemMapInit(void)
{
    static AMBA_MMU_MAP_INFO64_s *pMapTmp;

    /* VMSAv8-32 short-descriptors */
    pMapTmp = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_FLAT];
    pMapTmp->VirtAddr = (AMBA_CORTEX_A53_DRAM_PHYS_BASE_ADDR);
    pMapTmp->PhysAddr = (AMBA_CORTEX_A53_DRAM_PHYS_BASE_ADDR);
#ifdef CONFIG_DDR_SIZE
    // We limit DDR SIZE to 3G here to avoid Translation table overlap the IO address between 0xE0000000~0xFFFFFFFF
    pMapTmp->MemSize = (CONFIG_DDR_SIZE > 0xC0000000UL) ? 0xC0000000UL : CONFIG_DDR_SIZE;
#else
    pMapTmp->MemSize = (1ULL << 30ULL);
#endif
    pMapTmp->MemAttr = AMBA_MMU_MAP_ATTR_L1_BLOCK;
    pMapTmp->MemAttr |= MMU_GetMemRwAttr();

    pMapTmp = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_FIO_FIFO];
    pMapTmp->VirtAddr = (0xe8020000ULL);
    pMapTmp->PhysAddr = (0xe8020000ULL);
    pMapTmp->MemSize = (0x1000ULL);
    pMapTmp->MemAttr = AMBA_MMU_MAP_ATTR_L3_PAGE;
    pMapTmp->MemAttr |= MMU_GetFifoAttr();

    pMapTmp = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_RSVD];
    pMapTmp->VirtAddr = (AMBA_DRAM_RESERVED_VIRT_ADDR);
    pMapTmp->PhysAddr = (0x00000000ULL);
    pMapTmp->MemSize = (AMBA_DRAM_RESERVED_SIZE);
    pMapTmp->MemAttr = AMBA_MMU_MAP_ATTR_L3_PAGE;
    pMapTmp->MemAttr |= MMU_GetMemRwAttr();

    (void)AmbaRTSL_CacheFlushDataPtr(MMU_RamMapInfo, AMBA_MMU_NUM_RAM_REGION * sizeof(AMBA_MMU_MAP_INFO64_s));

    return OK;
}
#endif

