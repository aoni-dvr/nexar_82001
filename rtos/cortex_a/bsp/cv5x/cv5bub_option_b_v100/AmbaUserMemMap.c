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
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#include "AmbaFDT.h"
#endif

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
#define AMBA_MMU_RAM_REGION_DSP_CACHE_BUF   (3U)    /* DSP Cacheable Buffers */
#define AMBA_MMU_RAM_REGION_DSP_BUF         (4U)    /* DSP Non-cacheable Buffers */
#define AMBA_MMU_RAM_REGION_RW_DATA         (5U)    /* Read/Write Data */
#define AMBA_MMU_RAM_REGION_DSP_UCODE       (6U)    /* DSP uCode */
#define AMBA_MMU_RAM_REGION_CACHED_HEAP     (7U)    /* Cacheable Memory Heap */
#define AMBA_MMU_RAM_REGION_NONCACHED_HEAP  (8U)    /* Non-cacheable Memory Heap */
#define AMBA_MMU_RAM_REGION_HIGH_VECT       (9U)    /* High Vector Region */
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
static AMBA_MMU_MAP_INFO64_s MMU_RamMapInfo[AMBA_MMU_NUM_RAM_REGION];

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
/**
 *  Data structure of ShareMem.
 */
typedef struct {
    ULONG       CVSharedMemAddr;        /**< CV Shared memory address */
    ULONG       CVSharedMemSize;        /**< CV Shared memory size */

    ULONG       CVRtosMemAddr;          /**< CV Rtos Shared memory address */
    ULONG       CVRtosMemSize;          /**< CV Rtos Shared memory size */

    ULONG       CVSysMemAddr;           /**< CV System memory address */
    ULONG       CVSysMemSize;           /**< CV System memory size */

    ULONG       CVSchdrMemAddr;         /**< CV Schdr Shared memory address */
    ULONG       CVSchdrMemSize;         /**< CV Schdr Shared memory size */

    ULONG       DspProtMemAddr;         /**< DSP protocol memory physical address */
    ULONG       DspProtMemSize;         /**< DSP protocol memory size */

    ULONG       DspProtCacheMemAddr;    /**< DSP protocol cached memory physical address */
    ULONG       DspProtCacheMemSize;    /**< DSP protocol cached memory size */

    ULONG       DspBinMemAddr;          /**< DSP Binary memory  physical address */
    ULONG       DspBinMemSize;          /**< DSP Binary memory size */

    ULONG       DspDataMemAddr;         /**< DSP Data memory physical address */
    ULONG       DspDataMemSize;         /**< DSP Data memory size */
} MMU_MAP_SHM_CTRL_s;
static MMU_MAP_SHM_CTRL_s MMUShmCtrl = {0};

static void MMU_RamMapCVShmCfgPart1(const void *fdt)
{
    INT32 offset, len;
    const char *chr1;
    const UINT32 *ptr1;
    const UINT64 *ptr64;
    const struct fdt_property *prop = NULL;

    offset = AmbaFDT_PathOffset(fdt, "scheduler");
    if (offset >= 0) {
        prop = AmbaFDT_GetProperty(fdt, offset, "cv_att_pa", &len);
        if ((prop != NULL) && (len == 4)) {
            chr1 = (const char *)&prop->data[0];
            AmbaMisra_TypeCast(&ptr1, &chr1);
            MMUShmCtrl.CVSharedMemAddr = (ULONG)AmbaFDT_Fdt32ToCpu(ptr1[0]);
            MMUShmCtrl.CVSchdrMemAddr = (ULONG)AmbaFDT_Fdt32ToCpu(ptr1[0]);
        } else if ((prop != NULL) && (len == 8)) {
            chr1 = (const char *)&prop->data[0];
            AmbaMisra_TypeCast(&ptr64, &chr1);
            MMUShmCtrl.CVSharedMemAddr = (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[0]);
            MMUShmCtrl.CVSchdrMemAddr = (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[0]);
        } else {
            MMUShmCtrl.CVSharedMemAddr = (ULONG)0UL;
            MMUShmCtrl.CVSchdrMemAddr = (ULONG)0UL;
        }

        prop = AmbaFDT_GetProperty(fdt, offset, "cv_att_size", &len);
        if ((prop != NULL) && (len == 4)) {
            chr1 = (const char *)&prop->data[0];
            AmbaMisra_TypeCast(&ptr1, &chr1);
            MMUShmCtrl.CVSharedMemSize= (ULONG)AmbaFDT_Fdt32ToCpu(ptr1[0]);
        } else if ((prop != NULL) && (len == 8)) {
            chr1 = (const char *)&prop->data[0];
            AmbaMisra_TypeCast(&ptr64, &chr1);
            MMUShmCtrl.CVSharedMemSize= (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[0]);
        } else {
            MMUShmCtrl.CVSharedMemSize = (ULONG)0UL;
        }

        prop = AmbaFDT_GetProperty(fdt, offset, "cv_schdr_size", &len);
        if ((prop != NULL) && (len == 4)) {
            chr1 = (const char *)&prop->data[0];
            AmbaMisra_TypeCast(&ptr1, &chr1);
            MMUShmCtrl.CVSchdrMemSize= (ULONG)AmbaFDT_Fdt32ToCpu(ptr1[0]);
        } else if ((prop != NULL) && (len == 8)) {
            chr1 = (const char *)&prop->data[0];
            AmbaMisra_TypeCast(&ptr64, &chr1);
            MMUShmCtrl.CVSchdrMemSize= (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[0]);
        } else {
            MMUShmCtrl.CVSchdrMemSize=(ULONG)0UL;
        }
    }

}

static void MMU_RamMapCVShmCfgPart2(const void *fdt)
{
    INT32 offset, len;
    const char *chr1;
    const UINT32 *ptr1;
    const UINT64 *ptr64;
    const struct fdt_property *prop = NULL;

    offset = AmbaFDT_PathOffset(fdt, "flexidag_sys");
    if (offset >= 0) {
        prop = AmbaFDT_GetProperty(fdt, offset, "reg", &len);
        if ((prop != NULL) && (len == 8)) {
            chr1 = (const char *)&prop->data[0];
            AmbaMisra_TypeCast(&ptr1, &chr1);
            MMUShmCtrl.CVSysMemAddr = (ULONG)AmbaFDT_Fdt32ToCpu(ptr1[0]);
            MMUShmCtrl.CVSysMemSize = (ULONG)AmbaFDT_Fdt32ToCpu(ptr1[1]);
        } else if ((prop != NULL) && (len == 16)) {
            chr1 = (const char *)&prop->data[0];
            AmbaMisra_TypeCast(&ptr64, &chr1);
            MMUShmCtrl.CVSysMemAddr = (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[0]);
            MMUShmCtrl.CVSysMemSize = (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[1]);
        } else {
            MMUShmCtrl.CVSysMemAddr = (ULONG)0UL;
            MMUShmCtrl.CVSysMemSize = (ULONG)0UL;
        }
    }

    offset = AmbaFDT_PathOffset(fdt, "flexidag_rtos");
    if (offset >= 0) {
        prop = AmbaFDT_GetProperty(fdt, offset, "reg", &len);
        if ((prop != NULL) && (len == 8)) {
            chr1 = (const char *)&prop->data[0];
            AmbaMisra_TypeCast(&ptr1, &chr1);
            MMUShmCtrl.CVRtosMemAddr = (ULONG)AmbaFDT_Fdt32ToCpu(ptr1[0]);
            MMUShmCtrl.CVRtosMemSize = (ULONG)AmbaFDT_Fdt32ToCpu(ptr1[1]);
        } else if ((prop != NULL) && (len == 16)) {
            chr1 = (const char *)&prop->data[0];
            AmbaMisra_TypeCast(&ptr64, &chr1);
            MMUShmCtrl.CVRtosMemAddr = (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[0]);
            MMUShmCtrl.CVRtosMemSize = (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[1]);
        } else {
            MMUShmCtrl.CVRtosMemAddr = (ULONG)0UL;
            MMUShmCtrl.CVRtosMemSize = (ULONG)0UL;
        }
    }
}
static void MMU_RamMapCVShmCfg(const void *fdt)
{
    MMU_RamMapCVShmCfgPart1(fdt);
    MMU_RamMapCVShmCfgPart2(fdt);
}

static void MMU_RamMapDSPShmCfgPart1(const void *fdt)
{
    INT32 offset, len;
    const char *chr1;
    const UINT32 *ptr1;
    const UINT64 *ptr64;
    const struct fdt_property *prop = NULL;

    offset = AmbaFDT_PathOffset(fdt, "/reserved-memory/dsp_prot_buf");
    if (offset >= 0) {
        prop = AmbaFDT_GetProperty(fdt, offset, "reg", &len);
        if ((prop != NULL) && (len == 8)) {
            chr1 = (const char *)&prop->data[0];
            AmbaMisra_TypeCast(&ptr1, &chr1);
            MMUShmCtrl.DspProtMemAddr = (ULONG)AmbaFDT_Fdt32ToCpu(ptr1[0]);
            MMUShmCtrl.DspProtMemSize = (ULONG)AmbaFDT_Fdt32ToCpu(ptr1[1]);
        } else if ((prop != NULL) && (len == 16)) {
            chr1 = (const char *)&prop->data[0];
            AmbaMisra_TypeCast(&ptr64, &chr1);
            MMUShmCtrl.DspProtMemAddr = (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[0]);
            MMUShmCtrl.DspProtMemSize = (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[1]);
        } else {
            MMUShmCtrl.DspProtMemAddr = (ULONG)0UL;
            MMUShmCtrl.DspProtMemSize = (ULONG)0UL;
        }
    }

    offset = AmbaFDT_PathOffset(fdt, "/reserved-memory/dsp_prot_cache_buf");
    if (offset >= 0) {
        prop = AmbaFDT_GetProperty(fdt, offset, "reg", &len);
        if ((prop != NULL) && (len == 8)) {
            chr1 = (const char *)&prop->data[0];
            AmbaMisra_TypeCast(&ptr1, &chr1);
            MMUShmCtrl.DspProtCacheMemAddr = (ULONG)AmbaFDT_Fdt32ToCpu(ptr1[0]);
            MMUShmCtrl.DspProtCacheMemSize = (ULONG)AmbaFDT_Fdt32ToCpu(ptr1[1]);
        } else if ((prop != NULL) && (len == 16)) {
            chr1 = (const char *)&prop->data[0];
            AmbaMisra_TypeCast(&ptr64, &chr1);
            MMUShmCtrl.DspProtCacheMemAddr = (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[0]);
            MMUShmCtrl.DspProtCacheMemSize = (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[1]);
        } else {
            MMUShmCtrl.DspProtCacheMemAddr = (ULONG)0UL;
            MMUShmCtrl.DspProtCacheMemSize = (ULONG)0UL;
        }
    }
}

static void MMU_RamMapDSPShmCfgPart2(const void *fdt)
{
    INT32 offset, len;
    const char *chr1;
    const UINT32 *ptr1;
    const UINT64 *ptr64;
    const struct fdt_property *prop = NULL;

    offset = AmbaFDT_PathOffset(fdt, "/reserved-memory/dsp_bin_buf");
    if (offset >= 0) {
        prop = AmbaFDT_GetProperty(fdt, offset, "reg", &len);
        if ((prop != NULL) && (len == 8)) {
            chr1 = (const char *)&prop->data[0];
            AmbaMisra_TypeCast(&ptr1, &chr1);
            MMUShmCtrl.DspBinMemAddr = (ULONG)AmbaFDT_Fdt32ToCpu(ptr1[0]);
            MMUShmCtrl.DspBinMemSize = (ULONG)AmbaFDT_Fdt32ToCpu(ptr1[1]);
        } else if ((prop != NULL) && (len == 16)) {
            chr1 = (const char *)&prop->data[0];
            AmbaMisra_TypeCast(&ptr64, &chr1);
            MMUShmCtrl.DspBinMemAddr = (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[0]);
            MMUShmCtrl.DspBinMemSize = (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[1]);
        } else {
            MMUShmCtrl.DspBinMemAddr = (ULONG)0UL;
            MMUShmCtrl.DspBinMemSize = (ULONG)0UL;
        }
    }

    offset = AmbaFDT_PathOffset(fdt, "/reserved-memory/dsp_data_buf");
    if (offset >= 0) {
        prop = AmbaFDT_GetProperty(fdt, offset, "reg", &len);
        if ((prop != NULL) && (len == 8)) {
            chr1 = (const char *)&prop->data[0];
            AmbaMisra_TypeCast(&ptr1, &chr1);
            MMUShmCtrl.DspDataMemAddr = (ULONG)AmbaFDT_Fdt32ToCpu(ptr1[0]);
            MMUShmCtrl.DspDataMemSize = (ULONG)AmbaFDT_Fdt32ToCpu(ptr1[1]);
        } else if ((prop != NULL) && (len == 16)) {
            chr1 = (const char *)&prop->data[0];
            AmbaMisra_TypeCast(&ptr64, &chr1);
            MMUShmCtrl.DspDataMemAddr = (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[0]);
            MMUShmCtrl.DspDataMemSize = (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[1]);
        } else {
            MMUShmCtrl.DspDataMemAddr = (ULONG)0UL;
            MMUShmCtrl.DspDataMemSize = (ULONG)0UL;
        }
    }
}

static void MMU_RamMapDSPShmCfg(const void *fdt)
{
    MMU_RamMapDSPShmCfgPart1(fdt);
    MMU_RamMapDSPShmCfgPart2(fdt);
}


static void MMU_RamMapShmCfg(void)
{
    const void *fdt;
    INT32 ret;
    ULONG dtb_addr;

    dtb_addr = (ULONG)CONFIG_DTB_LOADADDR;
    AmbaMisra_TypeCast(&fdt, &dtb_addr);

    ret = AmbaFDT_CheckHeader(fdt);
    if (ret == 0) {
        MMU_RamMapCVShmCfg(fdt);
        MMU_RamMapDSPShmCfg(fdt);
    }
}
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
static ULONG MMU_RamMapTranslate(UINT64 Addr)
{
#ifdef CONFIG_ENABLE_VIRTUAL_ADDRESS
    ULONG HighAddr, LowAddr;

    HighAddr = (Addr & 0xF0000000UL) - (AMBA_CORTEX_A76_DRAM_VIRT_BASE_ADDR & 0xF0000000UL);
    LowAddr  = (Addr & 0x0FFFFFFFUL) - (AMBA_CORTEX_A76_DRAM_VIRT_BASE_ADDR & 0x0FFFFFFFUL);

    return (HighAddr | LowAddr);
#else
    return Addr;
#endif
}

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
static ULONG MMU_RamMapTranslateP2V(ULONG Addr)
{
    // Basically, we shall get VirtAddr from lds symbol.
    // here just make phyiscal as virtual
    return Addr;

}
#endif

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
static inline UINT64 MMU_RamMapAlign(UINT64 Addr, UINT64 Align)
{
    return (((Addr & (Align - 1UL)) > 0UL) ? ((Addr & ~(Align - 1UL)) + Align) : Addr);
}

/*
 * When DDR size is bigger than 4GB(32Gb), the end symbol reference may not be able to fit 32-bit offset.
 * So we declare a 64-bit storage here to keep address and referent to the storage symbol instead.
 * This is the way to fix linking error: "relocation truncated to fit: R_AARCH64_ADR_PREL_PG_HI21 against symbol `__cache_heap_end'"
 */
extern UINT64 __cache_heap_end, __non_cache_heap_end;
UINT64 *p_cache_heap_end = &__cache_heap_end;
UINT64 *p_non_cache_heap_end = &__non_cache_heap_end;

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
    extern UINT64 __ddr_resv_start;
    extern UINT64 __ddr_region0_ro_start, __ddr_region0_ro_end;
    extern UINT64 __ddr_region0_rw_start, __ddr_region0_rw_end;
    extern UINT64 __sbrk_start, __bss_end;
#if !defined(CONFIG_DEVICE_TREE_SUPPORT)
    extern UINT64 __dsp_cache_buf_start, __dsp_cache_buf_end;
    extern UINT64 __dsp_buf_start, __dsp_buf_end;
    extern UINT64 __ucode_start, __ucode_end;
#endif
    extern UINT64 __system_exception_table_start;

    extern UINT64 __non_cache_heap_start;
    extern UINT64 __cache_heap_start;
#ifdef CONFIG_ENABLE_AMBALINK
    extern UINT64 __linux_start, __linux_end;
#endif
#if !defined(CONFIG_DEVICE_TREE_SUPPORT)
#if (CONFIG_CV_MEM_SIZE != 0)
    extern UINT64 __cv_start, __cv_sys_start, __cv_sys_end;
#endif
#if (CONFIG_APP_MEM_SIZE != 0)
    extern UINT64 __app_start, __app_end;
#endif
#if (CONFIG_CV_MEM_RTOS_SIZE != 0)
    extern UINT64 __cv_rtos_user_start, __cv_rtos_user_end;
#endif
#endif

#ifdef CONFIG_ENABLE_DSP_DATA_BUF
#if !defined(CONFIG_DEVICE_TREE_SUPPORT)
    extern UINT64 __dsp_data_start, __dsp_data_end;
#endif
#endif

#ifdef __aarch64__
    AMBA_MMU_MAP_INFO64_s *pMapInfo;
#else
    AMBA_MMU_MAP_INFO32_s *pMapInfo;
#endif
    const UINT64 *pTmp;
    UINT64 Tmp = 0U;
#if defined(CONFIG_ENABLE_AMBALINK) && !defined(CONFIG_FWPROG_ATF_ENABLE)
    AMBA_MMU_MAP_ATTR64_s *pMemAttrBits;
#endif

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    MMU_RamMapShmCfg();
#endif

    /* Cacheable Memory: Reserved */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_RESERVED];
    pTmp = &__ddr_resv_start;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = AMBA_DRAM_RESERVED_VIRT_ADDR;
    pMapInfo->MemSize   = AMBA_DRAM_RESERVED_SIZE;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_KB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;

    /* Cacheable Memory: ROM */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_ROM];
    pTmp = &__ddr_region0_ro_start;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = Tmp;
    pTmp = &__ddr_region0_ro_end;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->MemSize   = MMU_RamMapAlign((Tmp), MMU_MAP_SIZE_MB) - pMapInfo->VirtAddr;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;

    /* Cacheable Memory: INIT_DATA */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_INIT_DATA];
    pTmp = &__ddr_region0_rw_start;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = Tmp;
    pTmp = &__ddr_region0_rw_end;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->MemSize   = MMU_RamMapAlign((Tmp), MMU_MAP_SIZE_MB) - pMapInfo->VirtAddr;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;

    /* Cheable Memory: DSP Buffers */
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    if (MMUShmCtrl.DspProtCacheMemSize > 0U) {
        pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_DSP_CACHE_BUF];
        pMapInfo->PhysAddr  = MMUShmCtrl.DspProtCacheMemAddr;
        pMapInfo->VirtAddr  = MMU_RamMapTranslateP2V(MMUShmCtrl.DspProtCacheMemAddr);
        pMapInfo->MemSize   = MMUShmCtrl.DspProtCacheMemSize;
        pMapInfo->MemAttr   = MMU_MAP_TYPE_KB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;
    }

#else
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_DSP_CACHE_BUF];
    pTmp = &__dsp_cache_buf_start;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = Tmp;
    pTmp = &__dsp_cache_buf_end;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->MemSize   = MMU_RamMapAlign((Tmp), MMU_MAP_SIZE_MB) - pMapInfo->VirtAddr;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;
#endif

    /* Non-cheable Memory: DSP Buffers */
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    if (MMUShmCtrl.DspProtMemSize > 0U) {
        pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_DSP_BUF];
        pMapInfo->PhysAddr  = MMUShmCtrl.DspProtMemAddr;
        pMapInfo->VirtAddr  = MMU_RamMapTranslateP2V(MMUShmCtrl.DspProtMemAddr);
        pMapInfo->MemSize   = MMUShmCtrl.DspProtMemSize;
        pMapInfo->MemAttr   = MMU_MAP_TYPE_KB  | AMBA_MMU_MAP_NON_CACHE_MEM_ATTR;
    }
#else
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_DSP_BUF];
    pTmp = &__dsp_buf_start;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = Tmp;
    pTmp = &__dsp_buf_end;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->MemSize   = MMU_RamMapAlign((Tmp), MMU_MAP_SIZE_MB) - pMapInfo->VirtAddr;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_NON_CACHE_MEM_ATTR;
#endif

    /* Cacheable Memory: Read/Write */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_RW_DATA];
    pTmp = &__sbrk_start;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = Tmp;
    pTmp = &__bss_end;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->MemSize   = MMU_RamMapAlign((Tmp), MMU_MAP_SIZE_MB) - pMapInfo->VirtAddr;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;

    /* Cacheable memory: DSP uCodes */
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    if (MMUShmCtrl.DspBinMemSize > 0U) {
        pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_DSP_UCODE];
        pMapInfo->PhysAddr  = MMUShmCtrl.DspBinMemAddr;
        pMapInfo->VirtAddr  = MMU_RamMapTranslateP2V(MMUShmCtrl.DspBinMemAddr);
        pMapInfo->MemSize   = MMUShmCtrl.DspBinMemSize;
        pMapInfo->MemAttr   = MMU_MAP_TYPE_KB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;
    }
#else
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_DSP_UCODE];
    pTmp = &__ucode_start;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = Tmp;
    pTmp = &__ucode_end;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->MemSize   = MMU_RamMapAlign((Tmp), MMU_MAP_SIZE_MB) - pMapInfo->VirtAddr;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;
#endif

    /* Cacheable memory: heap  */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_CACHED_HEAP];
    pTmp = &__cache_heap_start;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = Tmp;
    pTmp = p_cache_heap_end;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->MemSize   = MMU_RamMapAlign((Tmp), MMU_MAP_SIZE_MB) - pMapInfo->VirtAddr;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;

    /* Non-cacheable memory: heap  */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_NONCACHED_HEAP];
    pTmp = &__non_cache_heap_start;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = Tmp;
    pTmp = p_non_cache_heap_end;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->MemSize   = MMU_RamMapAlign((Tmp), MMU_MAP_SIZE_MB) - pMapInfo->VirtAddr;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_NON_CACHE_MEM_ATTR;

    /* Remap the high vector to VECT$$Base */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_HIGH_VECT];
    pTmp = &__system_exception_table_start;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = AMBA_DRAM_EXCEPTION_VIRT_ADDR;
    pMapInfo->MemSize   = AMBA_DRAM_EXCEPTION_SIZE;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_KB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;


#if !defined(CONFIG_DEVICE_TREE_SUPPORT)
#if (CONFIG_APP_MEM_SIZE != 0)
    /* cacheable memory: APP */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_APP];
    pTmp                = &__app_start;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = Tmp;
    pTmp = &__app_end;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->MemSize   = MMU_RamMapAlign((Tmp), MMU_MAP_SIZE_MB) - pMapInfo->VirtAddr;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR | AMBA_MMU_MAP_ATTR_INNER_SHAREABLE;
#if !defined(CONFIG_FWPROG_ATF_ENABLE)
    pMemAttrBits =(AMBA_MMU_MAP_ATTR64_s *) &pMapInfo->MemAttr;
    pMemAttrBits->NonSecure = 0U;
#endif
#endif
#endif

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    if (MMUShmCtrl.DspDataMemSize > 0U) {
        pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_DSP_DATA];
        pMapInfo->PhysAddr  = MMUShmCtrl.DspDataMemAddr;
        pMapInfo->VirtAddr  = MMU_RamMapTranslateP2V(MMUShmCtrl.DspDataMemAddr);
        pMapInfo->MemSize   = MMUShmCtrl.DspDataMemSize;
        pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;
    }
#else
#ifdef CONFIG_ENABLE_DSP_DATA_BUF
    {
        UINT64 StartAddr, EndAddr;

        pTmp = &__dsp_data_start;
        AmbaMisra_TypeCast64(&StartAddr, &pTmp);
        pTmp = &__dsp_data_end;
        AmbaMisra_TypeCast64(&EndAddr, &pTmp);
        if (EndAddr > StartAddr) {
            pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_DSP_DATA];
            pMapInfo->PhysAddr  = MMU_RamMapTranslate(StartAddr);
            pMapInfo->VirtAddr  = StartAddr;
            pMapInfo->MemSize   = MMU_RamMapAlign((EndAddr), MMU_MAP_SIZE_MB) - StartAddr;
            pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;
        }
    }
#endif
#endif

    /*-----------------------------------------------------------------------------------------------*\
     * Below code sections handle MMU map for CV memroy with or without AmbaLink.
     *
     * ----------------------------------------------------------------------------------------------
     * CV memory memory layout with AmbaLink
     * ----------------------------------------------------------------------------------------------
     *
     * (MMU map region)                                                (linker script symbol)
     *
     *                          +-----------------------------------+   __linux_start
     *                          |                                   |
     *  Map AmbaLink region     |   CONFIG_AMBALINK_MEM_SIZE -      |
     *                          |   CONFIG_CV_MEM_SIZE              |
     *                          |                                   |
     *                          +-----------------------------------+   __cv_rtos_user_start
     *                          |                                   |
     *                          |   CONFIG_CV_MEM_RTOS_SIZE         |
     *  Map CV_RTOS region      |                                   |
     *                          |                                   |
     *                          +-----------------------------------+   __cv_sys_start/__cv_rtos_user_end
     *                          |                                   |
     *                          |   CONFIG_CV_MEM_SIZE -            |
     *  Map CV_SYS region       |   CONFIG_CV_MEM_RTOS_SIZE         |
     *                          |                                   |
     *                          |                                   |
     *                          +-----------------------------------+   __linux_end/__cv_sys_end
     *-----------------------------------------------------------------------------------------------*
     * CV memory memory layout without AmbaLink
     * ----------------------------------------------------------------------------------------------
     *
     * (MMU map region)                                                (linker script symbol)
     *
     *                          +-----------------------------------+   __cv_rtos_user_start
     *                          |                                   |
     *                          |   CONFIG_CV_MEM_RTOS_SIZE         |
     *  Map CV_RTOS region      |                                   |
     *                          |                                   |
     *                          +-----------------------------------+   __cv_sys_start/__cv_rtos_user_end
     *                          |                                   |
     *                          |   CONFIG_CV_MEM_SIZE -            |
     *  Map CV_SYS region       |   CONFIG_CV_MEM_RTOS_SIZE         |
     *                          |                                   |
     *                          |                                   |
     *                          +-----------------------------------+   __cv_sys_end
    \*-----------------------------------------------------------------------------------------------*/


#ifdef CONFIG_ENABLE_AMBALINK
    /* Map AmbaLink region */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_LINUX];
    pTmp = &__linux_start;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = Tmp;
#if (CONFIG_CV_MEM_SIZE != 0)
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    if((MMU_RamMapTranslateP2V(MMUShmCtrl.CVSharedMemAddr)) > (ULONG)(pMapInfo->VirtAddr)) {
        Tmp = (UINT64)MMU_RamMapTranslateP2V(MMUShmCtrl.CVSharedMemAddr);
        AmbaMisra_TypeCast64(&pTmp, &Tmp);
    } else {
        pTmp = &__linux_end;
    }
#else
    if((&__cv_start) > (&__linux_start)) {
        pTmp = &__cv_start;
    } else {
        pTmp = &__linux_end;
    }
#endif
#else
    pTmp = &__linux_end;
#endif
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->MemSize   = MMU_RamMapAlign((Tmp), MMU_MAP_SIZE_MB) - pMapInfo->VirtAddr;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR | AMBA_MMU_MAP_ATTR_INNER_SHAREABLE;
#if !defined(CONFIG_FWPROG_ATF_ENABLE)
    pMemAttrBits =(AMBA_MMU_MAP_ATTR64_s *) &pMapInfo->MemAttr;
    pMemAttrBits->NonSecure = 0U;
#endif
#endif

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#if (CONFIG_CV_MEM_RTOS_SIZE != 0)
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_CV_RTOS];
    pMapInfo->PhysAddr  = MMUShmCtrl.CVRtosMemAddr;
    pMapInfo->VirtAddr  = MMU_RamMapTranslateP2V(MMUShmCtrl.CVRtosMemAddr);
    pMapInfo->MemSize   = MMUShmCtrl.CVRtosMemSize;
#ifdef CONFIG_ENABLE_AMBALINK
    /* make sure that cache is coherent in multi-os case */
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR | AMBA_MMU_MAP_ATTR_INNER_SHAREABLE;
#if !defined(CONFIG_FWPROG_ATF_ENABLE)
    pMemAttrBits =(AMBA_MMU_MAP_ATTR64_s *) &pMapInfo->MemAttr;
    pMemAttrBits->NonSecure = 0U;
#endif
#else
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;
#endif
#endif

#if (CONFIG_CV_MEM_SIZE != 0)
    /* CV  */
    /* set visorc init-data area as cacheable */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_CV_INIT];
    pMapInfo->PhysAddr  = MMUShmCtrl.CVSysMemAddr;
    pMapInfo->VirtAddr  = MMU_RamMapTranslateP2V(MMUShmCtrl.CVSysMemAddr);
    pMapInfo->MemSize   = 0x200000;
#ifdef CONFIG_ENABLE_AMBALINK
    /* make sure that cache is coherent in multi-os case */
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR | AMBA_MMU_MAP_ATTR_INNER_SHAREABLE;
#if !defined(CONFIG_FWPROG_ATF_ENABLE)
    pMemAttrBits =(AMBA_MMU_MAP_ATTR64_s *) &pMapInfo->MemAttr;
    pMemAttrBits->NonSecure = 0U;
#endif
#else
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;
#endif

    /* set main data area as cacheable */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_CV_MAIN];
    pMapInfo->PhysAddr  = (MMUShmCtrl.CVSysMemAddr + 0x200000U);
    pMapInfo->VirtAddr  = MMU_RamMapTranslateP2V(MMUShmCtrl.CVSysMemAddr + 0x200000U);
    pMapInfo->MemSize   = MMUShmCtrl.CVSysMemSize - 0x200000U;
#ifdef CONFIG_ENABLE_AMBALINK
    /* make sure that cache is coherent in multi-os case */
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR | AMBA_MMU_MAP_ATTR_INNER_SHAREABLE;
#if !defined(CONFIG_FWPROG_ATF_ENABLE)
    pMemAttrBits =(AMBA_MMU_MAP_ATTR64_s *) &pMapInfo->MemAttr;
    pMemAttrBits->NonSecure = 0U;
#endif
#else
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;
#endif
#endif
#else
#if (CONFIG_CV_MEM_RTOS_SIZE != 0)
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_CV_RTOS];
    pTmp = &__cv_rtos_user_start;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = Tmp;
    pTmp = &__cv_rtos_user_end;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->MemSize   = MMU_RamMapAlign((Tmp), MMU_MAP_SIZE_MB) - pMapInfo->VirtAddr;
#ifdef CONFIG_ENABLE_AMBALINK
    /* make sure that cache is coherent in multi-os case */
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR | AMBA_MMU_MAP_ATTR_INNER_SHAREABLE;
#if !defined(CONFIG_FWPROG_ATF_ENABLE)
    pMemAttrBits =(AMBA_MMU_MAP_ATTR64_s *) &pMapInfo->MemAttr;
    pMemAttrBits->NonSecure = 0U;
#endif
#else
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;
#endif
#endif

#if (CONFIG_CV_MEM_SIZE != 0)
    (void)__cv_start;
    /* CV  */
    /* set visorc init-data area as cacheable */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_CV_INIT];
    pTmp                = &__cv_sys_start;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = Tmp;
    pMapInfo->MemSize   = 0x200000;
#ifdef CONFIG_ENABLE_AMBALINK
    /* make sure that cache is coherent in multi-os case */
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR | AMBA_MMU_MAP_ATTR_INNER_SHAREABLE;
#if !defined(CONFIG_FWPROG_ATF_ENABLE)
    pMemAttrBits =(AMBA_MMU_MAP_ATTR64_s *) &pMapInfo->MemAttr;
    pMemAttrBits->NonSecure = 0U;
#endif
#else
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;
#endif

    /* set main data area as cacheable */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_CV_MAIN];
    Tmp                 += 0x200000U;
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = Tmp;
    pTmp = &__cv_sys_end;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->MemSize   = MMU_RamMapAlign((Tmp), MMU_MAP_SIZE_MB) - pMapInfo->VirtAddr;
#ifdef CONFIG_ENABLE_AMBALINK
    /* make sure that cache is coherent in multi-os case */
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR | AMBA_MMU_MAP_ATTR_INNER_SHAREABLE;
#if !defined(CONFIG_FWPROG_ATF_ENABLE)
    pMemAttrBits =(AMBA_MMU_MAP_ATTR64_s *) &pMapInfo->MemAttr;
    pMemAttrBits->NonSecure = 0U;
#endif
#else
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;
#endif
#endif
#endif

#if defined(CONFIG_PIO_NONCHED_SIZE) && defined(CONFIG_PIO_BASE)
    /*non cached PIO*/
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_PIO_NONCHED];
    pMapInfo->PhysAddr  = MMU_RamMapTranslate((UINT64)CONFIG_PIO_BASE);
    pMapInfo->VirtAddr  = (UINT64)CONFIG_PIO_BASE;
    pMapInfo->MemSize   = (UINT64)CONFIG_PIO_NONCHED_SIZE;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;

    /*cached PIO*/
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_PIO_CACHED];
    pMapInfo->PhysAddr  = MMU_RamMapTranslate((UINT64)CONFIG_PIO_BASE + (UINT64)CONFIG_PIO_NONCHED_SIZE);
    pMapInfo->VirtAddr  = (UINT64)CONFIG_PIO_BASE + (UINT64)CONFIG_PIO_NONCHED_SIZE;
    pMapInfo->MemSize   = (UINT64)CONFIG_PIO_SIZE - (UINT64)CONFIG_PIO_NONCHED_SIZE;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;
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

#ifdef __aarch64__
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MMU_RamMapInitPreOSInit
 *
 *  @Description:: MMU Memory Map initializations for DRAM before init OS
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MMU_RamMapInitPreOSInit(void)
{
    extern UINT64 __ddr_resv_start;
    extern UINT64 __ddr_region0_ro_start, __ddr_region0_ro_end;
    extern UINT64 __ddr_region0_rw_start, __ddr_region0_rw_end;
    extern UINT64 __sbrk_start, __bss_end;
    extern UINT64 __system_exception_table_start;

    extern UINT64 __non_cache_heap_start;
    extern UINT64 __cache_heap_start;

    AMBA_MMU_MAP_INFO64_s *pMapInfo;

    const UINT64 *pTmp;
    UINT64 Tmp = 0U;

    /* Cacheable Memory: Reserved */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_RESERVED];
    pTmp = &__ddr_resv_start;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = AMBA_DRAM_RESERVED_VIRT_ADDR;
    pMapInfo->MemSize   = AMBA_DRAM_RESERVED_SIZE;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_KB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;

    /* Cacheable Memory: ROM */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_ROM];
    pTmp = &__ddr_region0_ro_start;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = Tmp;
    pTmp = &__ddr_region0_ro_end;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->MemSize   = MMU_RamMapAlign((Tmp), MMU_MAP_SIZE_MB) - pMapInfo->VirtAddr;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_ONLY_ATTR;

    /* Cacheable Memory: INIT_DATA */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_INIT_DATA];
    pTmp = &__ddr_region0_rw_start;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = Tmp;
    pTmp = &__ddr_region0_rw_end;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->MemSize   = MMU_RamMapAlign((Tmp), MMU_MAP_SIZE_MB) - pMapInfo->VirtAddr;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;

    /* Cacheable Memory: Read/Write */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_RW_DATA];
    pTmp = &__sbrk_start;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = Tmp;
    pTmp = &__bss_end;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->MemSize   = MMU_RamMapAlign((Tmp), MMU_MAP_SIZE_MB) - pMapInfo->VirtAddr;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;

    /* Cacheable memory: heap  */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_CACHED_HEAP];
    pTmp = &__cache_heap_start;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = Tmp;
    pTmp = p_cache_heap_end;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->MemSize   = MMU_RamMapAlign((Tmp), MMU_MAP_SIZE_MB) - pMapInfo->VirtAddr;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;

    /* Non-cacheable memory: heap  */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_NONCACHED_HEAP];
    pTmp = &__non_cache_heap_start;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = Tmp;
    pTmp = p_non_cache_heap_end;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->MemSize   = MMU_RamMapAlign((Tmp), MMU_MAP_SIZE_MB) - pMapInfo->VirtAddr;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_MB  | AMBA_MMU_MAP_NON_CACHE_MEM_ATTR;

    /* Remap the high vector to VECT$$Base */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_HIGH_VECT];
    pTmp = &__system_exception_table_start;
    AmbaMisra_TypeCast64(&Tmp, &pTmp);
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(Tmp);
    pMapInfo->VirtAddr  = AMBA_DRAM_EXCEPTION_VIRT_ADDR;
    pMapInfo->MemSize   = AMBA_DRAM_EXCEPTION_SIZE;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_KB  | AMBA_MMU_MAP_CACHE_MEM_READ_ONLY_ATTR;

#ifdef CONFIG_DTB_LOADADDR
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_XTB_SHAR_MEM];
    pMapInfo->PhysAddr  = MMU_RamMapTranslate(CONFIG_DTB_LOADADDR);
    pMapInfo->VirtAddr  = CONFIG_DTB_LOADADDR;
    pMapInfo->MemSize   = CONFIG_DTB_SIZE;
    pMapInfo->MemAttr   = MMU_MAP_TYPE_KB | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;
#endif

    return OK;
}
#endif

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
UINT32 AmbaMMU_CheckCached(UINT64 StartAddr, UINT32 Size)
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

extern AMBA_MMU_USER_CONFIG64_s AmbaMmuUserConfig;
AMBA_MMU_USER_CONFIG64_s AmbaMmuUserConfig = {
    .UserPreConfigEntry = MMU_RamMapInit,               /* pointer to the init function of Memory Map */
    .NumMemMap          = AMBA_MMU_NUM_RAM_REGION,      /* number of Memory Map regions */
    .pMemMap            = MMU_RamMapInfo,               /* pointer to the base address of Memory Map Info */
    .VectorAddr         = AMBA_DRAM_EXCEPTION_VIRT_ADDR /* Exception vector address */
};

AMBA_MMU_USER_CONFIG64_s AmbaMmuUserConfigPreOSInit = {
    .UserPreConfigEntry = MMU_RamMapInitPreOSInit,               /* pointer to the init function of Memory Map */
    .NumMemMap          = AMBA_MMU_NUM_RAM_REGION,      /* number of Memory Map regions */
    .pMemMap            = MMU_RamMapInfo,               /* pointer to the base address of Memory Map Info */
    .VectorAddr         = AMBA_DRAM_EXCEPTION_VIRT_ADDR /* Exception vector address */
};

