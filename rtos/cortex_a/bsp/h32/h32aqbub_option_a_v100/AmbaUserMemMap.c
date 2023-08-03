/**
 *  @file AmbaUserMemMap.c
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
 *  @details Mapping the physical address of RAM to logical address with caching/uncaching attribute
 *
 */

#include "AmbaTypes.h"
#include "AmbaMemMap.h"
#include "AmbaMisraFix.h"

#include "AmbaMMU_Def.h"
#include "bsp.h"

#ifdef CONFIG_MMU_USE_SHORT_DESCRIPTOR
#define MMU_MAP_TYPE_MB                 AMBA_MMU_MAP_ATTR_SECTION
#define MMU_MAP_SIZE_MB                 AMBA_MMU_MAP_SECTION_SIZE
#define MMU_MAP_TYPE_KB                 AMBA_MMU_MAP_ATTR_SMALL_PAGE
#define MMU_MAP_SIZE_KB                 AMBA_MMU_MAP_SMALL_PAGE_SIZE
#else /* !defined(CONFIG_MMU_USE_SHORT_DESCRIPTOR) */
#define MMU_MAP_TYPE_MB                 AMBA_MMU_MAP_ATTR_L2_BLOCK
#define MMU_MAP_SIZE_MB                 AMBA_MMU_MAP_L2_BLOCK_SIZE
#define MMU_MAP_TYPE_KB                 AMBA_MMU_MAP_ATTR_L3_PAGE
#define MMU_MAP_SIZE_KB                 AMBA_MMU_MAP_L3_PAGE_SIZE
#endif

#if !defined(CONFIG_CV_MEM_SIZE)
#define CONFIG_CV_MEM_SIZE                        0
#endif

#if !defined(CONFIG_CV_MEM_RTOS_SIZE)
#define CONFIG_CV_MEM_RTOS_SIZE                 0
#endif

#if !defined(CONFIG_APP_MEM_SIZE)
#define CONFIG_APP_MEM_SIZE                        0
#endif

#if !defined(CONFIG_DSP_WORK_SIZE)
#define CONFIG_DSP_WORK_SIZE                    0
#endif

/*-----------------------------------------------------------------------------------------------*\
 * MMU RAM address region ID
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_MMU_RAM_REGION_RESERVED        (0U)    /* Reserved */
#define AMBA_MMU_RAM_REGION_ROM             (1U)    /* ROM */
#define AMBA_MMU_RAM_REGION_INIT_DATA       (2U)    /* Read/Write Data with non-zero initial value */
#define AMBA_MMU_RAM_REGION_DSP_BUF         (3U)    /* DSP Buffers */
#define AMBA_MMU_RAM_REGION_RW_DATA         (4U)    /* Read/Write Data */
#define AMBA_MMU_RAM_REGION_DSP_UCODE       (5U)    /* DSP uCode */
#define AMBA_MMU_RAM_REGION_CACHED_HEAP     (6U)    /* Cacheable Memory Heap */
#define AMBA_MMU_RAM_REGION_NONCACHED_HEAP  (7U)    /* Non-cacheable Memory Heap */
#define AMBA_MMU_RAM_REGION_HIGH_VECT       (8U)    /* High Vector Region */
#ifdef CONFIG_ENABLE_AMBALINK
#define AMBA_MMU_RAM_REGION_LINUX           (AMBA_MMU_RAM_REGION_HIGH_VECT + 1U)  /* Linux */
#else
#define AMBA_MMU_RAM_REGION_LINUX           (AMBA_MMU_RAM_REGION_HIGH_VECT)  /* Linux */
#endif

#if (CONFIG_CV_MEM_SIZE != 0)
#define AMBA_MMU_RAM_REGION_CV_INIT         (AMBA_MMU_RAM_REGION_LINUX + 1U)        /* CV visorc init data */
#define AMBA_MMU_RAM_REGION_CV_MAIN         (AMBA_MMU_RAM_REGION_CV_INIT + 1U)       /* CV visorc main data */
#else
#define AMBA_MMU_RAM_REGION_CV_INIT         (AMBA_MMU_RAM_REGION_LINUX)                /* CV visorc init data */
#define AMBA_MMU_RAM_REGION_CV_MAIN         (AMBA_MMU_RAM_REGION_CV_INIT)           /* CV visorc main data */
#endif

#if (CONFIG_CV_MEM_RTOS_SIZE != 0)
#define AMBA_MMU_RAM_REGION_CV_RTOS         (AMBA_MMU_RAM_REGION_CV_MAIN + 1U)   /* CV rtos Memory*/
#else
#define AMBA_MMU_RAM_REGION_CV_RTOS         (AMBA_MMU_RAM_REGION_CV_MAIN)   /* CV rtos Memory*/
#endif

#if (CONFIG_APP_MEM_SIZE != 0)
#define AMBA_MMU_RAM_REGION_APP             (AMBA_MMU_RAM_REGION_CV_RTOS + 1U)   /* App Memory*/
#else
#define AMBA_MMU_RAM_REGION_APP             (AMBA_MMU_RAM_REGION_CV_RTOS)   /* App Memory*/
#endif

/* DSP work, includes DspWork and DspDbgLog */
#if (CONFIG_DSP_WORK_SIZE != 0)
#define AMBA_MMU_RAM_REGION_DSP_WORK        (AMBA_MMU_RAM_REGION_APP + 1U)
#else
#define AMBA_MMU_RAM_REGION_DSP_WORK        (AMBA_MMU_RAM_REGION_APP)
#endif

/* DSP Data CFG */
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#define AMBA_MMU_RAM_REGION_DSP_DATA        (AMBA_MMU_RAM_REGION_DSP_WORK + 1U)
#else
#ifdef CONFIG_ENABLE_DSP_DATA_BUF
#define AMBA_MMU_RAM_REGION_DSP_DATA        (AMBA_MMU_RAM_REGION_DSP_WORK + 1U)
#else
#define AMBA_MMU_RAM_REGION_DSP_DATA        (AMBA_MMU_RAM_REGION_DSP_WORK)
#endif
#endif


#ifdef CONFIG_PIO_BASE
#define AMBA_MMU_RAM_REGION_PIO_NONCHED     (AMBA_MMU_RAM_REGION_DSP_DATA + 1U)
#define AMBA_MMU_RAM_REGION_PIO_CACHED      (AMBA_MMU_RAM_REGION_PIO_NONCHED + 1U)
#else
#define AMBA_MMU_RAM_REGION_PIO_CACHED      (AMBA_MMU_RAM_REGION_DSP_DATA)
#endif

#ifdef CONFIG_DTB_LOADADDR
#define AMBA_MMU_RAM_REGION_XTB_SHAR_MEM    (AMBA_MMU_RAM_REGION_PIO_CACHED + 1U)
#else
#define AMBA_MMU_RAM_REGION_XTB_SHAR_MEM    (AMBA_MMU_RAM_REGION_PIO_CACHED)
#endif

#define AMBA_MMU_NUM_RAM_REGION             (AMBA_MMU_RAM_REGION_XTB_SHAR_MEM + 1U)   /* Number of DRAM memory map regions */
/*-----------------------------------------------------------------------------------------------*\
 * MMU RAM map table
\*-----------------------------------------------------------------------------------------------*/
#ifdef __aarch64__
static AMBA_MMU_MAP_INFO64_s MMU_RamMapInfo[AMBA_MMU_NUM_RAM_REGION];
#else
static AMBA_MMU_MAP_INFO32_s MMU_RamMapInfo[AMBA_MMU_NUM_RAM_REGION];
#endif
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MMU_RamMapTranslate
 *
 *  @Description:: MMU Memory Map Translation for Virtual Address RTOS.
 *                 Add some more operations to avoid the IAR optimization.
 *                 IAR optimization will cause linking error.
 *
 *  @Input      ::
 *      Addr:  address to be translated
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MMU_RamMapTranslate(UINT32 Addr)
{
#ifdef CONFIG_ENABLE_VIRTUAL_ADDRESS
    UINT32 HighAddr, LowAddr;

    HighAddr = (Addr & 0xF0000000U) - (AMBA_CORTEX_A53_DRAM_VIRT_BASE_ADDR & 0xF0000000U);
    LowAddr  = (Addr & 0x0FFFFFFFU) - (AMBA_CORTEX_A53_DRAM_VIRT_BASE_ADDR & 0x0FFFFFFFU);

    return (HighAddr | LowAddr);
#else
    return Addr;
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MMU_RamMapAlign
 *
 *  @Description:: Align the end of the reion.
 *
 *  @Input      ::
 *      Addr:  address to be aligned.
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline UINT32 MMU_RamMapAlign(UINT32 Addr, UINT32 Align)
{
    return (((Addr & (Align - 1U)) > 0U) ? ((Addr & ~(Align - 1U)) + Align) : Addr);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MMU_RamMapInit
 *
 *  @Description:: MMU Memory Map initializations for DRAM
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MMU_RamMapInit(void)
{
    extern UINT32 __ddr_resv_start;
    extern UINT32 __ddr_region0_ro_start, __ddr_region0_ro_end;
    extern UINT32 __ddr_region0_rw_start, __ddr_region0_rw_end;
    extern UINT32 __sbrk_start, __bss_end;
    extern UINT32 __dsp_buf_start, __dsp_buf_end;
    extern UINT32 __system_exception_table_start;

    extern UINT32 __ucode_start, __ucode_end;
    extern UINT32 __non_cache_heap_start, __non_cache_heap_end;
    extern UINT32 __cache_heap_start, __cache_heap_end;
#ifdef CONFIG_ENABLE_AMBALINK
    extern UINT32 __linux_start, __linux_end;
#endif
#if (CONFIG_CV_MEM_SIZE != 0)
    extern UINT32 __cv_start, __cv_end;
#endif
#if (CONFIG_APP_MEM_SIZE != 0)
    extern UINT32 __app_start, __app_end;
#endif
#if (CONFIG_DSP_WORK_SIZE != 0)
    extern UINT32 __dsp_work_start, __dsp_work_end;
#endif

#ifdef CONFIG_ENABLE_DSP_DATA_BUF
    extern UINT32 __dsp_data_start, __dsp_data_end;
#endif

#ifdef __aarch64__
    AMBA_MMU_MAP_INFO64_s *pMapInfo;
#else
    AMBA_MMU_MAP_INFO32_s *pMapInfo;
#endif
    const UINT32 *pTmp;
    UINT32 Tmp = 0U;
#ifdef CONFIG_ENABLE_AMBALINK
    AMBA_MMU_MAP_ATTR32_s *pMemAttrBits;
#endif

    /* Cacheable Memory: Reserved */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_RESERVED];
    pTmp = &__ddr_resv_start;
    AmbaMisra_TypeCast32(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = AMBA_DRAM_RESERVED_VIRT_ADDR;
    pMapInfo->MemSize   = AMBA_DRAM_RESERVED_SIZE;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_KB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;

    /* Cacheable Memory: ROM */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_ROM];
    pTmp = &__ddr_region0_ro_start;
    AmbaMisra_TypeCast32(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = Tmp;
    pTmp = &__ddr_region0_ro_end;
    AmbaMisra_TypeCast32(&Tmp, &pTmp);
    pMapInfo->MemSize   = MMU_RamMapAlign((Tmp), MMU_MAP_SIZE_KB) - pMapInfo->VirtAddr;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_KB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;

    /* Cacheable Memory: INIT_DATA */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_INIT_DATA];
    pTmp = &__ddr_region0_rw_start;
    AmbaMisra_TypeCast32(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = Tmp;
    pTmp = &__ddr_region0_rw_end;
    AmbaMisra_TypeCast32(&Tmp, &pTmp);
    pMapInfo->MemSize   = MMU_RamMapAlign((Tmp), MMU_MAP_SIZE_KB) - pMapInfo->VirtAddr;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_KB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;

    /* Non-cheable Memory: DSP Buffers */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_DSP_BUF];
    pTmp = &__dsp_buf_start;
    AmbaMisra_TypeCast32(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = Tmp;
    pTmp = &__dsp_buf_end;
    AmbaMisra_TypeCast32(&Tmp, &pTmp);
    pMapInfo->MemSize   = MMU_RamMapAlign((Tmp), MMU_MAP_SIZE_KB) - pMapInfo->VirtAddr;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_KB  | AMBA_MMU_MAP_NON_CACHE_MEM_ATTR;

    /* Cacheable Memory: Read/Write */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_RW_DATA];
    pTmp = &__sbrk_start;
    AmbaMisra_TypeCast32(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = Tmp;
    pTmp = &__bss_end;
    AmbaMisra_TypeCast32(&Tmp, &pTmp);
    pMapInfo->MemSize   = MMU_RamMapAlign((Tmp), MMU_MAP_SIZE_KB) - pMapInfo->VirtAddr;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_KB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;

    /* Cacheable memory: DSP uCodes */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_DSP_UCODE];
    pTmp = &__ucode_start;
    AmbaMisra_TypeCast32(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = Tmp;
    pTmp = &__ucode_end;
    AmbaMisra_TypeCast32(&Tmp, &pTmp);
    pMapInfo->MemSize   = MMU_RamMapAlign((Tmp), MMU_MAP_SIZE_MB) - pMapInfo->VirtAddr;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;

    /* Cacheable memory: heap  */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_CACHED_HEAP];
    pTmp = &__cache_heap_start;
    AmbaMisra_TypeCast32(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = Tmp;
    pTmp = &__cache_heap_end;
    AmbaMisra_TypeCast32(&Tmp, &pTmp);
    pMapInfo->MemSize   = MMU_RamMapAlign((Tmp), MMU_MAP_SIZE_MB) - pMapInfo->VirtAddr;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;

    /* Non-cacheable memory: heap  */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_NONCACHED_HEAP];
    pTmp = &__non_cache_heap_start;
    AmbaMisra_TypeCast32(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = Tmp;
    pTmp = &__non_cache_heap_end;
    AmbaMisra_TypeCast32(&Tmp, &pTmp);
    pMapInfo->MemSize   = MMU_RamMapAlign((Tmp), MMU_MAP_SIZE_MB) - pMapInfo->VirtAddr;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_NON_CACHE_MEM_ATTR;

    /* Remap the high vector to VECT$$Base */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_HIGH_VECT];
    pTmp = &__system_exception_table_start;
    AmbaMisra_TypeCast32(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = AMBA_DRAM_EXCEPTION_VIRT_ADDR;
    pMapInfo->MemSize   = AMBA_DRAM_EXCEPTION_SIZE;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_KB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;

#if (CONFIG_DSP_WORK_SIZE != 0)
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_DSP_WORK];
    pTmp = &__dsp_work_start;
    AmbaMisra_TypeCast32(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = Tmp;
    pTmp = &__dsp_work_end;
    AmbaMisra_TypeCast32(&Tmp, &pTmp);
    pMapInfo->MemSize   = MMU_RamMapAlign((Tmp), MMU_MAP_SIZE_MB) - pMapInfo->VirtAddr;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;
#endif

#ifdef CONFIG_ENABLE_DSP_DATA_BUF
    {
        UINT32 StartAddr, EndAddr;

        pTmp = &__dsp_data_start;
        AmbaMisra_TypeCast32(&StartAddr, &pTmp);
        pTmp = &__dsp_data_end;
        AmbaMisra_TypeCast32(&EndAddr, &pTmp);
        if (EndAddr > StartAddr) {
            pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_DSP_DATA];
            pMapInfo->PhysAddr  = MMU_RamMapTranslate(StartAddr);
            pMapInfo->VirtAddr  = StartAddr;
            pMapInfo->MemSize   = MMU_RamMapAlign((EndAddr), MMU_MAP_SIZE_MB) - StartAddr;
            pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;
        }
    }
#endif

#ifdef CONFIG_ENABLE_AMBALINK
    /* cacheable memory: Linux area */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_LINUX];
    pTmp = &__linux_start;
    AmbaMisra_TypeCast32(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = Tmp;
    pTmp = &__linux_end;
    AmbaMisra_TypeCast32(&Tmp, &pTmp);
    pMapInfo->MemSize   = MMU_RamMapAlign((Tmp), MMU_MAP_SIZE_MB) - pMapInfo->VirtAddr;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR | AMBA_MMU_MAP_ATTR_INNER_SHAREABLE;

    /* non-secure inner-shareable memory */
    pMemAttrBits =(AMBA_MMU_MAP_ATTR32_s *) &pMapInfo->MemAttr;
    pMemAttrBits->NonSecure = 1U;
#endif

#if (CONFIG_CV_MEM_SIZE != 0)
    /* CV  */
    /* set visorc init-data area as cacheable */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_CV_INIT];
    pTmp                = &__cv_start;
    AmbaMisra_TypeCast32(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = Tmp;
    pMapInfo->MemSize   = 0x200000;
#ifdef CONFIG_ENABLE_AMBALINK
    /* make sure that cache is coherent in multi-os case */
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR | AMBA_MMU_MAP_ATTR_INNER_SHAREABLE;

    /* non-secure inner-shareable memory */
    pMemAttrBits =(AMBA_MMU_MAP_ATTR32_s *) &pMapInfo->MemAttr;
    pMemAttrBits->NonSecure = 1U;
#else
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;
#endif

    /* set main data area as cacheable */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_CV_MAIN];
    Tmp                 += 0x200000U;
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = Tmp;
    pTmp = &__cv_end;
    AmbaMisra_TypeCast32(&Tmp, &pTmp);
    pMapInfo->MemSize   = MMU_RamMapAlign((Tmp), MMU_MAP_SIZE_MB) - pMapInfo->VirtAddr;
#ifdef CONFIG_ENABLE_AMBALINK
    /* make sure that cache is coherent in multi-os case */
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR | AMBA_MMU_MAP_ATTR_INNER_SHAREABLE;

    /* non-secure inner-shareable memory */
    pMemAttrBits =(AMBA_MMU_MAP_ATTR32_s *) &pMapInfo->MemAttr;
    pMemAttrBits->NonSecure = 1U;
#else
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;
#endif
#endif

#if (CONFIG_APP_MEM_SIZE != 0)
    /* Non-cacheable memory: APP */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_APP];
    pTmp                = &__app_start;
    AmbaMisra_TypeCast32(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = Tmp;
    pTmp = &__app_end;
    AmbaMisra_TypeCast32(&Tmp, &pTmp);
    pMapInfo->MemSize   = MMU_RamMapAlign((Tmp), MMU_MAP_SIZE_MB) - pMapInfo->VirtAddr;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_NON_CACHE_MEM_ATTR;
#endif

#if defined(CONFIG_PIO_NONCHED_SIZE) && defined(CONFIG_PIO_BASE)
    /*non cached PIO*/
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_PIO_NONCHED];
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(CONFIG_PIO_BASE);
    pMapInfo->VirtAddr  = CONFIG_PIO_BASE;
    pMapInfo->MemSize   = CONFIG_PIO_NONCHED_SIZE;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_KB | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;

    /*cached PIO*/
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_PIO_CACHED];
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(CONFIG_PIO_BASE + CONFIG_PIO_NONCHED_SIZE);
    pMapInfo->VirtAddr  = CONFIG_PIO_BASE + CONFIG_PIO_NONCHED_SIZE;
    pMapInfo->MemSize   = CONFIG_PIO_SIZE - CONFIG_PIO_NONCHED_SIZE;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_KB | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;
#endif

#ifdef CONFIG_DTB_LOADADDR
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_XTB_SHAR_MEM];
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(CONFIG_DTB_LOADADDR);
    pMapInfo->VirtAddr  = CONFIG_DTB_LOADADDR;
    pMapInfo->MemSize   = CONFIG_DTB_SIZE;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_KB | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;
#endif

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaMMU_CheckCached
 *
 *  @Description:: Check if a DRAM area is Cached or not
 *
 *  @Input      ::
 *      startAddr: start address
 *      size:      size
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 :  1 - Cached; 0 - Non-Cached
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaMMU_CheckCached(UINT32 StartAddr, UINT32 Size)
{
#define NUM_CACHED_MMU_REGION_CHECK (3U)
    UINT32 Rval = 0U;
#ifdef __aarch64__
    static const AMBA_MMU_MAP_INFO64_s *const pMapInfo[NUM_CACHED_MMU_REGION_CHECK] = {&MMU_RamMapInfo[AMBA_MMU_RAM_REGION_INIT_DATA],
                                                                                       &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_RW_DATA],
                                                                                       &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_CACHED_HEAP]
                                                                                      };
#else
    static const AMBA_MMU_MAP_INFO32_s *const pMapInfo[NUM_CACHED_MMU_REGION_CHECK] = {&MMU_RamMapInfo[AMBA_MMU_RAM_REGION_INIT_DATA],
                                                                                       &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_RW_DATA],
                                                                                       &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_CACHED_HEAP]
                                                                                      };
#endif

    UINT8 i;
    for (i = 0U; i < NUM_CACHED_MMU_REGION_CHECK; i++) {
        if ((StartAddr >= pMapInfo[i]->VirtAddr) && ((StartAddr + Size) <= (pMapInfo[i]->VirtAddr + pMapInfo[i]->MemSize))) {
            Rval = 1U;
            break;
        }
    }

    return Rval;
}

#ifdef __aarch64__
extern AMBA_MMU_USER_CONFIG64_s AmbaMmuUserConfig;
AMBA_MMU_USER_CONFIG64_s AmbaMmuUserConfig = {
    .UserPreConfigEntry = MMU_RamMapInit,               /* pointer to the init function of Memory Map */
    .NumMemMap          = AMBA_MMU_NUM_RAM_REGION,      /* number of Memory Map regions */
    .pMemMap            = MMU_RamMapInfo,               /* pointer to the base address of Memory Map Info */
    .VectorAddr         = AMBA_DRAM_EXCEPTION_VIRT_ADDR /* Exception vector address */
};
#else
extern AMBA_MMU_USER_CONFIG32_s AmbaMmuUserConfig;
AMBA_MMU_USER_CONFIG32_s AmbaMmuUserConfig = {
    .UserPreConfigEntry = MMU_RamMapInit,               /* pointer to the init function of Memory Map */
    .NumMemMap          = AMBA_MMU_NUM_RAM_REGION,      /* number of Memory Map regions */
    .pMemMap            = MMU_RamMapInfo,               /* pointer to the base address of Memory Map Info */
    .VectorAddr         = AMBA_DRAM_EXCEPTION_VIRT_ADDR /* Exception vector address */
};
#endif

