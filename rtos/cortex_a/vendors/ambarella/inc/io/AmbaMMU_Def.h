/**
 *  @file AmbaMMU_Def.h
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
 *  @details Definitions & Constants for Memory Management Unit Control APIs
 *
 */

#ifndef AMBA_MMU_DEF_H
#define AMBA_MMU_DEF_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif
#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

/*
 * MMU api return value
 */
#define MMU_ERR_0000            (MMU_ERR_BASE)              /* Invalid argument */
#define MMU_ERR_0001            (MMU_ERR_BASE + 0x1U)       /* No TLB info */
#define MMU_ERR_0002            (MMU_ERR_BASE + 0x2U)       /* MMU is disabled */
#define MMU_ERR_00FF            (MMU_ERR_BASE + 0XFFU)      /* Unexpected error */

#define MMU_ERR_NONE            OK
#define MMU_ERR_ARG             MMU_ERR_0000
#define MMU_ERR_NOTLB           MMU_ERR_0001
#define MMU_ERR_NOMMU           MMU_ERR_0002
#define MMU_ERR_UNEXPECTED      MMU_ERR_00FF

/*
 * Memory Region Alignment
 */

#ifdef CONFIG_ARM32
/* VMSAv8-32 short-descriptor */
#define AMBA_MMU_MAP_SUPERSECTION_SIZE  ((UINT32)16U  << 20U)
#define AMBA_MMU_MAP_SECTION_SIZE       ((UINT32)1U   << 20U)
#define AMBA_MMU_MAP_LARGE_PAGE_SIZE    ((UINT32)64U  << 10U)
#define AMBA_MMU_MAP_SMALL_PAGE_SIZE    ((UINT32)4U   << 10U)

/* VMSAv8-32 long-descriptor */
#define AMBA_MMU_MAP_L1_BLOCK_SIZE      ((UINT32)1U   << 30U)
#define AMBA_MMU_MAP_L2_BLOCK_SIZE      ((UINT32)2U   << 20U)
#define AMBA_MMU_MAP_L3_PAGE_SIZE       ((UINT32)4U   << 10U)
#endif

#ifdef CONFIG_ARM64
/* VMSAv8-64 */
#if defined(CONFIG_TRANSLATION_GRANULE_4KB)
#define AMBA_MMU_MAP_L1_BLOCK_SIZE      ((UINT64)1UL   << 30U)
#define AMBA_MMU_MAP_L2_BLOCK_SIZE      ((UINT64)2UL   << 20U)
#define AMBA_MMU_MAP_L3_PAGE_SIZE       ((UINT64)4UL   << 10U)
#elif defined(CONFIG_TRANSLATION_GRANULE_16KB)
#define AMBA_MMU_MAP_L2_BLOCK_SIZE      ((UINT64)32UL  << 20U)
#define AMBA_MMU_MAP_L3_PAGE_SIZE       ((UINT64)16UL  << 10U)
#elif defined(CONFIG_TRANSLATION_GRANULE_64KB)
#define AMBA_MMU_MAP_L2_BLOCK_SIZE      ((UINT64)512UL << 20U)
#define AMBA_MMU_MAP_L3_PAGE_SIZE       ((UINT64)64UL  << 10U)
#endif

#endif

/*
 * MMU map attribute definitions
 */
static inline UINT32 AMBA_MMU_MAP_ATTR_DESC_TYPE(UINT32 x)
{
    return ((x) & 0x7U);    /* Descriptor Type */
}
static inline UINT32 AMBA_MMU_MAP_ATTR_NON_SECURE(UINT32 x)
{
    return (((x) & 0x1U) << 3U);    /* Non-Secure attribute */
}
static inline UINT32 AMBA_MMU_MAP_ATTR_AP(UINT32 x)
{
    return (((x) & 0xfU) << 4U);    /* Access Permissions */
}
static inline UINT32 AMBA_MMU_MAP_ATTR_EXEC_NEVER(UINT32 x)
{
    return (((x) & 0x3U) << 8U);    /* Execute-Never attribute */
}
static inline UINT32 AMBA_MMU_MAP_ATTR_SHAREABLE(UINT32 x)
{
    return (((x) & 0x3U) << 10U);    /* Shareability attribute. 0 = Non-shareable, 2 = Outer Shareable, 3 = Inner Shareable */
}
static inline UINT32 AMBA_MMU_MAP_ATTR_DOMAIN(UINT32 x)
{
    return (((x) & 0xfU) << 12U);
}
static inline UINT32 AMBA_MMU_MAP_ATTR_MEM_ATTR_INCACHE(void)
{
    return (UINT32) 0U;    /* Memory region attributes. Inner cache transient hint. */
}
static inline UINT32 AMBA_MMU_MAP_ATTR_MEM_ATTR(UINT32 x)
{
    return (((x) & 0xffU) << 16U);    /* Memory region attributes. Reference MAIR register for the meaning. */
}
static inline UINT32 AMBA_MMU_MAP_ATTR_NOT_GLOBAL(UINT32 x)
{
    return (((x) & 0x1U) << 24U);
}

/* Outer and Inner Non-shareable */
#define AMBA_MMU_MAP_ATTR_NON_SHAREABLE             \
    (AMBA_MMU_MAP_ATTR_SHAREABLE(0U))

/* Outer shareable */
#define AMBA_MMU_MAP_ATTR_OUTER_SHAREABLE           \
    (AMBA_MMU_MAP_ATTR_SHAREABLE(2U))

/* Inner shareable */
#define AMBA_MMU_MAP_ATTR_INNER_SHAREABLE           \
    (AMBA_MMU_MAP_ATTR_SHAREABLE(3U))

/* Inner cache transient hint */
#define AMBA_MMU_MAP_ATTR_CACHE_HINT_TRANSIENT      \
    (AMBA_MMU_MAP_ATTR_MEM_ATTR_INCACHE())

/* Inner cache non-transient hint */
#define AMBA_MMU_MAP_ATTR_CACHE_HINT_NON_TRANSIENT  \
    (AMBA_MMU_MAP_ATTR_MEM_ATTR(0x08U))

/* Inner cache read allocation hint */
#define AMBA_MMU_MAP_ATTR_CACHE_HINT_READ_ALLOCATE  \
    (AMBA_MMU_MAP_ATTR_MEM_ATTR(0x02U))

/* Inner cache write allocation hint */
#define AMBA_MMU_MAP_ATTR_CACHE_HINT_WRITE_ALLOCATE \
    (AMBA_MMU_MAP_ATTR_MEM_ATTR(0x01U))

/* Device-nGnRnE in ARMv8 or Strongly-ordered in ARMv7, Shareable */
#define AMBA_MMU_MAP_ATTR_STRONGLY_ORDERED          \
    (AMBA_MMU_MAP_ATTR_OUTER_SHAREABLE           |  \
     AMBA_MMU_MAP_ATTR_MEM_ATTR_INCACHE())

/* Device-nGnRE in ARMv8 or Device in ARMv7, Shareable */
#define AMBA_MMU_MAP_ATTR_DEVICE                    \
    (AMBA_MMU_MAP_ATTR_OUTER_SHAREABLE           |  \
     AMBA_MMU_MAP_ATTR_MEM_ATTR(0x04U))

/* Outer and Inner Non-cacheable */
#define AMBA_MMU_MAP_ATTR_CACHE_NONE                \
    (AMBA_MMU_MAP_ATTR_OUTER_SHAREABLE           |  \
     AMBA_MMU_MAP_ATTR_MEM_ATTR(0x44U))

/* Outer and Inner Write-Back */
#define AMBA_MMU_MAP_ATTR_CACHE_WRITE_BACK          \
    (AMBA_MMU_MAP_ATTR_MEM_ATTR(0x77U))

/*
 * Recommanded MMU map attributes
 */
#define AMBA_MMU_MAP_SECURE_BUS_ATTR                \
    (AMBA_MMU_MAP_ATTR_STRONGLY_ORDERED         |   \
     AMBA_MMU_MAP_ATTR_EXEC_NEVER(3U)           |   \
     AMBA_MMU_MAP_ATTR_AP((0x3U << 2U) | 0x1U))

#define AMBA_MMU_MAP_NON_SECURE_BUS_ATTR            \
    (AMBA_MMU_MAP_ATTR_STRONGLY_ORDERED         |   \
     AMBA_MMU_MAP_ATTR_EXEC_NEVER(3U)           |   \
     AMBA_MMU_MAP_ATTR_NON_SECURE(1U)           |   \
     AMBA_MMU_MAP_ATTR_AP((0x3U << 2U) | 0x1U))

#define AMBA_MMU_MAP_NON_CACHE_MEM_ATTR             \
    (AMBA_MMU_MAP_ATTR_CACHE_NONE               |   \
     AMBA_MMU_MAP_ATTR_NON_SECURE(1U)           |   \
     AMBA_MMU_MAP_ATTR_AP((0x3U << 2U) | 0x0U))

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#define AMBA_MMU_MAP_CACHE_MEM_READ_ONLY_ATTR       \
    (AMBA_MMU_MAP_ATTR_CACHE_WRITE_BACK         |   \
     AMBA_MMU_MAP_ATTR_INNER_SHAREABLE          |   \
     AMBA_MMU_MAP_ATTR_NON_SECURE(1U)           |   \
     AMBA_MMU_MAP_ATTR_AP((0x1U << 2U) | 0x0U))

#define AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR      \
    (AMBA_MMU_MAP_ATTR_CACHE_WRITE_BACK         |   \
     AMBA_MMU_MAP_ATTR_INNER_SHAREABLE          |   \
     AMBA_MMU_MAP_ATTR_NON_SECURE(1U)           |   \
     AMBA_MMU_MAP_ATTR_AP((0x3U << 2U) | 0x0U))

#define AMBA_MMU_MAP_CACHE_NX_MEM_READ_ONLY_ATTR    \
    (AMBA_MMU_MAP_ATTR_CACHE_WRITE_BACK         |   \
     AMBA_MMU_MAP_ATTR_INNER_SHAREABLE          |   \
     AMBA_MMU_MAP_ATTR_NON_SECURE(1U)           |   \
     AMBA_MMU_MAP_ATTR_AP((0x1U << 2U) | 0x3U))

#define AMBA_MMU_MAP_CACHE_MEM_RW_NX_ATTR           \
    (AMBA_MMU_MAP_ATTR_CACHE_WRITE_BACK         |   \
     AMBA_MMU_MAP_ATTR_INNER_SHAREABLE          |   \
     AMBA_MMU_MAP_ATTR_NON_SECURE(1U)           |   \
     AMBA_MMU_MAP_ATTR_AP((0x3U << 2U) | 0x1U))
#else //#if defined (CONFIG_SOC_CV5)
#define AMBA_MMU_MAP_CACHE_MEM_READ_ONLY_ATTR       \
    (AMBA_MMU_MAP_ATTR_CACHE_WRITE_BACK         |   \
     AMBA_MMU_MAP_ATTR_NON_SECURE(1U)           |   \
     AMBA_MMU_MAP_ATTR_AP((0x1U << 2U) | 0x0U))

#define AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR      \
    (AMBA_MMU_MAP_ATTR_CACHE_WRITE_BACK         |   \
     AMBA_MMU_MAP_ATTR_NON_SECURE(1U)           |   \
     AMBA_MMU_MAP_ATTR_AP((0x3U << 2U) | 0x0U))

#define AMBA_MMU_MAP_CACHE_NX_MEM_READ_ONLY_ATTR    \
    (AMBA_MMU_MAP_ATTR_CACHE_WRITE_BACK         |   \
     AMBA_MMU_MAP_ATTR_NON_SECURE(1U)           |   \
     AMBA_MMU_MAP_ATTR_AP((0x1U << 2U) | 0x3U))

#define AMBA_MMU_MAP_CACHE_MEM_RW_NX_ATTR           \
    (AMBA_MMU_MAP_ATTR_CACHE_WRITE_BACK         |   \
     AMBA_MMU_MAP_ATTR_NON_SECURE(1U)           |   \
     AMBA_MMU_MAP_ATTR_AP((0x3U << 2U) | 0x1U))
#endif //#if defined (CONFIG_SOC_CV5)

#define AMBA_MMU_MAP_NCACHE_NX_MEM_ATTR          \
    (AMBA_MMU_MAP_ATTR_CACHE_NONE               |   \
     AMBA_MMU_MAP_ATTR_NON_SECURE(1U)           |   \
     AMBA_MMU_MAP_ATTR_AP((0x3U << 2U) | 0x1U))


/* AMBA_MMU_MAP_ATTR_DESC_TYPE */
/* Definition for long-descriptor used by VMSAv8-32 and VMSAv8-64 */
#define AMBA_MMU_MAP_ATTR_L1_BLOCK              0x0U        /* 1GB (4KB granule) */
#define AMBA_MMU_MAP_ATTR_L2_BLOCK              0x1U        /* 2MB (4KB granule) or 32MB (16KB granule) or 512MB (64KB granule) */
#define AMBA_MMU_MAP_ATTR_L3_PAGE               0x2U        /* 4KB (4KB granule) or 16KB (16KB granule) or 64KB (64KB granule) */
/* Definition for short-descriptor used by VMSAv8-32 */
#define AMBA_MMU_MAP_ATTR_SUPERSECTION          0x3U        /* consist of 16MB blocks of memory */
#define AMBA_MMU_MAP_ATTR_SECTION               0x4U        /* consist of 1MB blocks of memory */
#define AMBA_MMU_MAP_ATTR_LARGE_PAGE            0x5U        /* consist of 64KB blocks of memory */
#define AMBA_MMU_MAP_ATTR_SMALL_PAGE            0x6U        /* consist of 4KB blocks of memory */

typedef UINT32 (*AMBA_MMU_PRE_CONFIG_f)(void);

#ifdef CONFIG_ARM32
/* VMSAv8-32  */
typedef struct {
    UINT32  DescType:           3;  /* [2:0]:   Descriptor type. */
    UINT32  NonSecure:          1;  /* [3]:     Non-secure bit. For memory accesses from Secure state. */
    UINT32  S1AP:               2;  /* [5:4]:   AP[2:1]. Stage 1 Access Permissions. */
    UINT32  S2AP:               2;  /* [7:6]:   S2AP. Stage 2 Access Permissions. */
    UINT32  UXN:                1;  /* [8]:     UXN or XN. Unprivileged execute-never bit. */
    UINT32  PXN:                1;  /* [9]:     PXN. Privileged execute-never bit. */
    UINT32  Shareability:       2;  /* [11:10]: Determines whether the translation is for Shareable memory. */
    UINT32  Domain:             4;  /* [15:12]: Domain field. Only supported by VMSAv8-32 short-descriptor. */
    UINT32  MemAttr:            8;  /* [23:16]: Memory region attribute bits. */
    UINT32  NotGlobal:          1;  /* [24]:    The not global bit. Determines how the translation is marked in the TLB. */
    UINT32  Reserved:           7;  /* [31:25]: Reserved. */
} AMBA_MMU_MAP_ATTR32_s;

typedef struct {
    UINT32  VirtAddr;               /* the first virtual address of memory region */
    UINT32  PhysAddr;               /* the first physical address of memory region */
    UINT32  MemSize;                /* should be multiple of page size */
    UINT32  MemAttr;                /* attribute and type of memory region */
} AMBA_MMU_MAP_INFO32_s;

typedef struct {
    AMBA_MMU_PRE_CONFIG_f UserPreConfigEntry;   /* user-defined function used to refine memory map settings */
    UINT32  NumMemMap;                          /* number of memory maps */
    AMBA_MMU_MAP_INFO32_s *pMemMap;             /* pointer to the first memory map */
    UINT32  VectorAddr;                         /* Vector address */
} AMBA_MMU_USER_CONFIG32_s;

#endif

#ifdef CONFIG_ARM64
/* VMSAv8-64 */
typedef struct {
    UINT32  DescType:           3;  /* [2:0]:   Descriptor type. */
    UINT32  NonSecure:          1;  /* [3]:     Non-secure bit. For memory accesses from Secure state. */
    UINT32  S1AP:               2;  /* [5:4]:   AP[2:1]. Stage 1 Access Permissions. */
    UINT32  S2AP:               2;  /* [7:6]:   S2AP. Stage 2 Access Permissions. */
    UINT32  UXN:                1;  /* [8]:     UXN or XN. Unprivileged execute-never bit. */
    UINT32  PXN:                1;  /* [9]:     PXN. Privileged execute-never bit. */
    UINT32  Shareability:       2;  /* [11:10]: Determines whether the translation is for Shareable memory. */
    UINT32  Domain:             4;  /* [15:12]: Domain field. Only supported by VMSAv8-32 short-descriptor. */
    UINT32  MemAttr:            8;  /* [23:16]: Memory region attribute bits. */
    UINT32  NotGlobal:          1;  /* [24]:    The not global bit. Determines how the translation is marked in the TLB. */
    UINT32  Reserved0:          7;  /* [31:25]: Reserved. */
    UINT32  Reserved1:          32; /* [63:32]: Reserved. */
} AMBA_MMU_MAP_ATTR64_s;

typedef struct {
    UINT64  VirtAddr;               /* the first virtual address of memory region */
    UINT64  PhysAddr;               /* the first physical address of memory region */
    UINT64  MemSize;                /* should be multiple of page size */
    UINT64  MemAttr;                /* attribute and type of memory region */
} AMBA_MMU_MAP_INFO64_s;

typedef struct {
    AMBA_MMU_PRE_CONFIG_f UserPreConfigEntry;   /* user-defined function used to refine memory map settings */
    UINT64  NumMemMap;                          /* number of memory maps */
    AMBA_MMU_MAP_INFO64_s *pMemMap;             /* pointer to the first memory map */
    UINT64  VectorAddr;                         /* Vector address */
} AMBA_MMU_USER_CONFIG64_s;

#endif

#endif  /* AMBA_MMU_DEF_H */
