/**
 *  @file AmbaRTSL_MMU_VMSAv8-64.h
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
 *  @details Common definitions & constants for AArch64 Virtual Memory System Architecture
 *
 */

#ifndef AMBA_RTSL_MMU_VMSA_V8_64_H
#define AMBA_RTSL_MMU_VMSA_V8_64_H

/*
 * Translation granule:
 * 4KB granule:
 *      A level 0 descriptor does not support block translation.
 *      A block entry:
 *      •   In a level 1 table describes the mapping of the associated 1GB input address range.
 *      •   In a level 2 table describes the mapping of the associated 2MB input address range.
 *      Each entry in a level 3 table describes the mapping of the associated 4KB input address range.
 * 16KB granule:
 *      Level 0 and level 1 descriptors do not support block translation.
 *      A block entry in a level 2 table describes the mapping of the associated 32MB input address range.
 *      Each entry in a level 3 table describes the mapping of the associated 16KB input address range.
 * 64KB granule:
 *      Level 0 lookup is not supported.
 *      A level 1 descriptor does not support block translation.
 *      A block entry in a level 2 table describes the mapping of the associated 512MB input address range.
 *      Each entry in a level 3 table describes the mapping of the associated 64KB input address range.
 */

/*
 *  Virtual Memory System Architecture version 8 (VMSAv8-64) level 0 descriptor formats
 */
#define AMBA_TLB_L0_DESC_INVALID0   0x0U        /* Invalid entry */
#define AMBA_TLB_L0_DESC_INVALID1   0x1U        /* Invalid entry */
#define AMBA_TLB_L0_DESC_INVALID2   0x2U        /* Invalid entry */
#define AMBA_TLB_L0_DESC_TABLE      0x3U        /* Table */

#define AMBA_TLB_L0_4KB_TABLE_ADDR_OFFSET   12ULL
#define AMBA_TLB_L0_4KB_TABLE_BASE_MASK     0x000000fffffff000ULL

typedef struct {
    UINT32  DescType:               2;          /* [1:0]:   0bx0 = Invalid entry */
    UINT32  Reserved0:              30;         /* [31:2]:  IGNORED */
    UINT32  Reserved1:              32;         /* [63:32]: IGNORED */
} AMBA_TLB_VMSAV8_L0_INVLD_s;

typedef struct {
    UINT32  DescType:               2;          /* [1:0]:   0b11 = Table descriptor */
    UINT32  Reserved0:              10;         /* [11:2]:  IGNORED */
    UINT32  L1TableAddrL:           20;         /* [31:12]: Level 1 table address (lower bits) */
    UINT32  L1TableAddrH:           8;          /* [39:32]: Level 1 table address (higher bits) */
    UINT32  Reserved1:              12;         /* [51:40]: SBZ */
    UINT32  Reserved2:              7;          /* [58:52]: IGNORED */
    UINT32  PrivExeNeverTable:      1;          /* [59]:    (Stage 1 only) PXN table */
    UINT32  ExeNeverTable:          1;          /* [60]:    (Stage 1 only) XN table */
    UINT32  ApTable:                2;          /* [62:61]: (Stage 1 only) AP table */
    UINT32  NonSecureTable:         1;          /* [63]:    (Stage 1 only) Non-secure table */
} AMBA_TLB_VMSAV8_L0_TBL_s;

typedef struct {
    UINT64  Data;
} AMBA_TLB_VMSAV8_L0_s;

/*
 *  Virtual Memory System Architecture version 8 (VMSAv8-64) level 1 descriptor formats
 */
#define AMBA_TLB_L1_DESC_INVALID0   0x0U        /* Invalid entry */
#define AMBA_TLB_L1_DESC_BLOCK      0x1U        /* Block */
#define AMBA_TLB_L1_DESC_INVALID1   0x2U        /* Invalid entry */
#define AMBA_TLB_L1_DESC_TABLE      0x3U        /* Table */

#define AMBA_TLB_L1_4KB_BLOCK_ADDR_OFFSET   30ULL
#define AMBA_TLB_L1_4KB_TABLE_ADDR_OFFSET   12ULL
#define AMBA_TLB_L1_64KB_TABLE_ADDR_OFFSET  16ULL
#define AMBA_TLB_L1_4KB_BLOCK_BASE_MASK     0x000000ffc0000000ULL
#define AMBA_TLB_L1_4KB_TABLE_BASE_MASK     0x000000fffffff000ULL
#define AMBA_TLB_L1_64KB_TABLE_BASE_MASK    0x000000ffffff0000ULL

typedef struct {
    UINT32  DescType:               2;          /* [1:0]:   0bx0 = Invalid entry */
    UINT32  Reserved0:              30;         /* [31:2]:  IGNORED */
    UINT32  Reserved1:              32;         /* [63:32]: IGNORED */
} AMBA_TLB_VMSAV8_L1_INVLD_s;

typedef struct {
    UINT32  DescType:               2;          /* [1:0]:   0b01 = Block descriptor */
    UINT32  AttrIndex:              3;          /* [4:2]:   Memory attributes index field[2:0] */
    UINT32  NonSecure:              1;          /* [5]:     Non-secure bit. */
    UINT32  AP:                     2;          /* [7:6]:   Access Permissions bits[2:1]. */
    UINT32  Shareability:           2;          /* [9:8]:   Determines whether the translation is for Shareable memory. */
    UINT32  AccessFlag:             1;          /* [10]:    Determines whether the translation is for Shareable memory. */
    UINT32  NotGlobal:              1;          /* [11]:    The not global bit. Used in the TLB matching process. */
    UINT32  Reserved0:              18;         /* [29:12]: SBZ */
    UINT32  OutputAddrL:            2;          /* [31:30]: Output address (lower bits) */
    UINT32  OutputAddrH:            8;          /* [39:32]: Output address (higher bits) */
    UINT32  Reserved1:              12;         /* [51:40]: SBZ */
    UINT32  Contiguous:             1;          /* [52]:    Indicateds that 16 adjacent translation table entries point to contiguout memory regions */
    UINT32  PrivExeNever:           1;          /* [53]:    (Stage 1 only) PXN bit */
    UINT32  ExeNever:               1;          /* [54]:    UXN or XN bit */
    UINT32  Reserved2:              9;          /* [63:55]: IGNORED */
} AMBA_TLB_VMSAV8_L1_BLK_s;

typedef struct {
    UINT32  DescType:               2;          /* [1:0]:   0b11 = Table descriptor */
    UINT32  Reserved0:              10;         /* [11:2]:  IGNORED */
    UINT32  L2TableAddrL:           20;         /* [31:12]: Level 2 table address (lower bits) */
    UINT32  L2TableAddrH:           8;          /* [39:32]: Level 2 table address (higher bits) */
    UINT32  Reserved1:              12;         /* [51:40]: SBZ */
    UINT32  Reserved2:              7;          /* [58:52]: IGNORED */
    UINT32  PrivExeNeverTable:      1;          /* [59]:    (Stage 1 only) PXN table */
    UINT32  ExeNeverTable:          1;          /* [60]:    (Stage 1 only) XN table */
    UINT32  ApTable:                2;          /* [62:61]: (Stage 1 only) AP table */
    UINT32  NonSecureTable:         1;          /* [63]:    (Stage 1 only) Non-secure table */
} AMBA_TLB_VMSAV8_L1_TBL_s;

typedef struct {
    UINT64  Data;
} AMBA_TLB_VMSAV8_L1_s;

/*
 *  Virtual Memory System Architecture version 8 (VMSAv8-64) level 2 descriptor formats
 */
#define AMBA_TLB_L2_DESC_INVALID0   0x0U        /* Invalid entry */
#define AMBA_TLB_L2_DESC_BLOCK      0x1U        /* Block */
#define AMBA_TLB_L2_DESC_INVALID1   0x2U        /* Invalid entry */
#define AMBA_TLB_L2_DESC_TABLE      0x3U        /* Table */

#define AMBA_TLB_L2_4KB_BLOCK_ADDR_OFFSET   21ULL
#define AMBA_TLB_L2_4KB_TABLE_ADDR_OFFSET   12ULL
#define AMBA_TLB_L2_64KB_BLOCK_ADDR_OFFSET  29ULL
#define AMBA_TLB_L2_64KB_TABLE_ADDR_OFFSET  16ULL
#define AMBA_TLB_L2_4KB_BLOCK_BASE_MASK     0x000000ffffe00000ULL
#define AMBA_TLB_L2_4KB_TABLE_BASE_MASK     0x000000fffffff000ULL
#define AMBA_TLB_L2_64KB_BLOCK_BASE_MASK    0x000000ffe0000000ULL
#define AMBA_TLB_L2_64KB_TABLE_BASE_MASK    0x000000ffffff0000ULL

typedef struct {
    UINT32  DescType:               2;          /* [1:0]:   0bx0 = Invalid entry */
    UINT32  Reserved0:              30;         /* [31:2]:  IGNORED */
    UINT32  Reserved1:              32;         /* [63:32]: IGNORED */
} AMBA_TLB_VMSAV8_L2_INVLD_s;

typedef struct {
    UINT32  DescType:               2;          /* [1:0]:   0b01 = Block descriptor */
    UINT32  AttrIndex:              3;          /* [4:2]:   Memory attributes index field[2:0] */
    UINT32  NonSecure:              1;          /* [5]:     Non-secure bit. */
    UINT32  AP:                     2;          /* [7:6]:   Access Permissions bits[2:1]. */
    UINT32  Shareability:           2;          /* [9:8]:   Determines whether the translation is for Shareable memory. */
    UINT32  AccessFlag:             1;          /* [10]:    Determines whether the translation is for Shareable memory. */
    UINT32  NotGlobal:              1;          /* [11]:    The not global bit. Used in the TLB matching process. */
    UINT32  Reserved0:              9;          /* [20:12]: SBZ */
    UINT32  OutputAddrL:            11;         /* [31:21]: Output address (lower bits) */
    UINT32  OutputAddrH:            8;          /* [39:32]: Output address (higher bits) */
    UINT32  Reserved1:              12;         /* [51:40]: SBZ */
    UINT32  Contiguous:             1;          /* [52]:    Indicateds that 16 adjacent translation table entries point to contiguout memory regions */
    UINT32  PrivExeNever:           1;          /* [53]:    (Stage 1 only) PXN bit */
    UINT32  ExeNever:               1;          /* [54]:    UXN or XN bit */
    UINT32  Reserved2:              9;          /* [63:55]: IGNORED */
} AMBA_TLB_VMSAV8_L2_BLK_s;

typedef struct {
    UINT32  DescType:               2;          /* [1:0]:   0b11 = Table descriptor */
    UINT32  Reserved0:              10;         /* [11:2]:  IGNORED */
    UINT32  L3TableAddrL:           20;         /* [31:12]: Level 3 table address (lower bits) */
    UINT32  L3TableAddrH:           8;          /* [39:32]: Level 3 table address (higher bits) */
    UINT32  Reserved1:              12;         /* [51:40]: SBZ */
    UINT32  Reserved2:              7;          /* [58:52]: IGNORED */
    UINT32  PrivExeNeverTable:      1;          /* [59]:    (Stage 1 only) PXN table */
    UINT32  ExeNeverTable:          1;          /* [60]:    (Stage 1 only) XN table */
    UINT32  ApTable:                2;          /* [62:61]: (Stage 1 only) AP table */
    UINT32  NonSecureTable:         1;          /* [63]:    (Stage 1 only) Non-secure table */
} AMBA_TLB_VMSAV8_L2_TBL_s;

typedef struct {
    UINT64  Data;
} AMBA_TLB_VMSAV8_L2_s;

/*
 *  Virtual Memory System Architecture version 8 (VMSAv8-64) level 3 descriptor formats
 */
#define AMBA_TLB_L3_DESC_INVALID0   0x0U        /* Invalid entry */
#define AMBA_TLB_L3_DESC_RESERVED   0x1U        /* Reserved, Invalid entry */
#define AMBA_TLB_L3_DESC_INVALID1   0x2U        /* Invalid entry */
#define AMBA_TLB_L3_DESC_PAGE       0x3U        /* Page */

#define AMBA_TLB_L3_4KB_PAGE_ADDR_OFFSET    12ULL
#define AMBA_TLB_L3_64KB_PAGE_ADDR_OFFSET   16ULL
#define AMBA_TLB_L3_4KB_PAGE_BASE_MASK      0x000000fffffff000ULL
#define AMBA_TLB_L3_64KB_PAGE_BASE_MASK     0x000000ffffff0000ULL

typedef struct {
    UINT32  DescType:               2;          /* [1:0]:   0bx0 = Invalid entry */
    UINT32  Reserved0:              30;         /* [31:2]:  IGNORED */
    UINT32  Reserved1:              32;         /* [63:32]: IGNORED */
} AMBA_TLB_VMSAV8_L3_INVLD_s;

typedef struct {
    UINT32  DescType:               2;          /* [1:0]:   0b01 = Reserved, Invalid entry */
    UINT32  Reserved0:              30;         /* [31:2]:  SBZ */
    UINT32  Reserved1:              32;         /* [63:32]: SBZ */
} AMBA_TLB_VMSAV8_L3_RSVD_s;

typedef struct {
    UINT32  DescType:               2;          /* [1:0]:   0b11 = Page descriptor */
    UINT32  AttrIndex:              3;          /* [4:2]:   Memory attributes index field[2:0] */
    UINT32  NonSecure:              1;          /* [5]:     Non-secure bit. */
    UINT32  AP:                     2;          /* [7:6]:   Access Permissions bits[2:1]. */
    UINT32  Shareability:           2;          /* [9:8]:   Determines whether the translation is for Shareable memory. */
    UINT32  AccessFlag:             1;          /* [10]:    Determines whether the translation is for Shareable memory. */
    UINT32  NotGlobal:              1;          /* [11]:    The not global bit. Used in the TLB matching process. */
    UINT32  OutputAddrL:            20;         /* [31:12]: Output address (lower bits) */
    UINT32  OutputAddrH:            8;          /* [39:32]: Output address (higher bits) */
    UINT32  Reserved:               12;         /* [51:40]: SBZ */
    UINT32  Contiguous:             1;          /* [52]:    Indicateds that 16 adjacent translation table entries point to contiguout memory regions */
    UINT32  PrivExeNever:           1;          /* [53]:    (Stage 1 only) PXN bit */
    UINT32  ExeNever:               1;          /* [54]:    UXN or XN bit */
    UINT32  Reserved2:              9;          /* [63:55]: IGNORED */
} AMBA_TLB_VMSAV8_L3_PG_s;

typedef struct {
    UINT64  Data;
} AMBA_TLB_VMSAV8_L3_s;

/* Definitions for 64-bit translation system. */
static inline UINT64 AMBA_TLB_GET_L0_4KB_DESC_ID(UINT64 x)
{
    return (((x) >> 39U) & 0x1ffU);    /* IA[47:39] */
}
static inline UINT64 AMBA_TLB_GET_L1_4KB_DESC_ID(UINT64 x)
{
    return (((x) >> 30U) & 0x1ffU);    /* IA[38:30] */
}
static inline UINT64 AMBA_TLB_GET_L2_4KB_DESC_ID(UINT64 x)
{
    return (((x) >> 21U) & 0x1ffU);    /* IA[29:21] */
}
static inline UINT64 AMBA_TLB_GET_L3_4KB_DESC_ID(UINT64 x)
{
    return (((x) >> 12U) & 0x1ffU);    /* IA[20:12] */
}

static inline UINT64 AMBA_TLB_GET_L1_64KB_DESC_ID(UINT64 x)
{
    return (((x) >> 42U) & 0x3fU);    /* IA[47:42] */
}
static inline UINT64 AMBA_TLB_GET_L2_64KB_DESC_ID(UINT64 x)
{
    return (((x) >> 29U) & 0x1fffU);    /* IA[41:29] */
}
static inline UINT64 AMBA_TLB_GET_L3_64KB_DESC_ID(UINT64 x)
{
    return (((x) >> 16U) & 0x1fffU);    /* IA[28:16] */
}

/* MMU TLB Descriptors */
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#define AMBA_NUM_TLB_L1_4KB_DESC_PARTIAL    (4U*16U*3U)  /* Number of L1 descriptors(IA[35:30]) */
#define AMBA_NUM_TLB_L1_4KB_DESC_PARTIAL0   (4U*16U)
#define AMBA_NUM_TLB_L1_4KB_DESC_PARTIAL1   4U
#define AMBA_NUM_TLB_L1_4KB_DESC_PARTIAL2   4U
#define AMBA_NUM_TLB_L2_4KB_DESC            512U         /* Number of L2 descriptors mapped to one L1 page table entry */
#define AMBA_NUM_TLB_L3_4KB_DESC_ALL        (512U*16U)   /* Number of L3 descriptors mapped to one L2 page table entry */
#else
#define AMBA_NUM_TLB_L1_4KB_DESC_PARTIAL    4U           /* Number of L1 descriptors mapped to 4GB DRAM size (IA[31:30]) */
#define AMBA_NUM_TLB_L2_4KB_DESC            512U         /* Number of L2 descriptors mapped to one L1 page table entry */
#endif
#define AMBA_NUM_TLB_L3_4KB_DESC            512U         /* Number of L3 descriptors mapped to one L2 page table entry */

#define AMBA_NUM_TLB_L2_64KB_DESC_PARTIAL   8U           /* Number of L2 descriptors mapped to one L1 page table entry (IA[31:29]) */
#define AMBA_NUM_TLB_L3_64KB_DESC           8192U        /* Number of L3 descriptors mapped to one L2 page table entry */

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
typedef struct {
    /* Aligned on a 4 KB boundary */
    AMBA_TLB_VMSAV8_L1_s   L1Desc[AMBA_NUM_TLB_L1_4KB_DESC_PARTIAL];                                /* Occupy 32B of L1 table (maps 1GB) */
    UINT64 Reserved0[512U - AMBA_NUM_TLB_L1_4KB_DESC_PARTIAL];                                      /* For alignment */
    /* Aligned on a 4 KB boundary */
    AMBA_TLB_VMSAV8_L2_s   L2Desc_00[AMBA_NUM_TLB_L1_4KB_DESC_PARTIAL0][AMBA_NUM_TLB_L2_4KB_DESC];  /* Occupy 16KB of L2 table (maps 2MB of each 1GB) */
    AMBA_TLB_VMSAV8_L2_s   L2Desc_10[AMBA_NUM_TLB_L1_4KB_DESC_PARTIAL1][AMBA_NUM_TLB_L2_4KB_DESC];  /* Occupy 16KB of L2 table (maps 2MB of each 1GB) */
    AMBA_TLB_VMSAV8_L2_s   L2Desc_20[AMBA_NUM_TLB_L1_4KB_DESC_PARTIAL2][AMBA_NUM_TLB_L2_4KB_DESC];  /* Occupy 16KB of L2 table (maps 2MB of each 1GB) */
    /* Aligned on a 4 KB boundary */
    AMBA_TLB_VMSAV8_L3_s   L3Desc[AMBA_NUM_TLB_L3_4KB_DESC_ALL];                                    /* Occupy 8MB of L3 table (maps 4KB of each 2MB) */
} AMBA_TLB_GRANULE_4KB_s;
#else
typedef struct {
    /* Aligned on a 4 KB boundary */
    AMBA_TLB_VMSAV8_L1_s   L1Desc[AMBA_NUM_TLB_L1_4KB_DESC_PARTIAL];    /* Occupy 32B of L1 table (maps 1GB) */
    UINT64 Reserved0[512U - AMBA_NUM_TLB_L1_4KB_DESC_PARTIAL];          /* For alignment */
    /* Aligned on a 4 KB boundary */
    AMBA_TLB_VMSAV8_L2_s   L2Desc[AMBA_NUM_TLB_L1_4KB_DESC_PARTIAL][AMBA_NUM_TLB_L2_4KB_DESC]; /* Occupy 16KB of L2 table (maps 2MB of each 1GB) */
    /* Aligned on a 4 KB boundary */
    AMBA_TLB_VMSAV8_L3_s   L3Desc[AMBA_NUM_TLB_L1_4KB_DESC_PARTIAL][AMBA_NUM_TLB_L2_4KB_DESC][AMBA_NUM_TLB_L3_4KB_DESC];   /* Occupy 8MB of L3 table (maps 4KB of each 2MB) */
} AMBA_TLB_GRANULE_4KB_s;
#endif
typedef struct {
    /* CA53 only supports up to 40-bit (1TB) physical address. We limit the virtual address range to the same size. */
    /* Aligned on a 64 KB boundary */
    AMBA_TLB_VMSAV8_L1_s   L1Desc;
    UINT64 Reserved0[8192U - 1U];                                                                                         /* For alignment */
    /* Aligned on a 64 KB boundary */
    AMBA_TLB_VMSAV8_L2_s   L2Desc[AMBA_NUM_TLB_L2_64KB_DESC_PARTIAL];                                      /* Occupy 64B of L2 table (maps 512MB of each 4GB) */
    UINT64 Reserved1[8192U - AMBA_NUM_TLB_L2_64KB_DESC_PARTIAL];                                                 /* For alignment */
    /* Aligned on a 64 KB boundary */
    AMBA_TLB_VMSAV8_L3_s   L3Desc[AMBA_NUM_TLB_L2_64KB_DESC_PARTIAL][AMBA_NUM_TLB_L3_64KB_DESC];   /* Occupy 512KB of L3 table (maps 64KB of each 512MB) */
} AMBA_TLB_GRANULE_64KB_s;

#endif  /* AMBA_RTSL_MMU_VMSA_V8_64_H */
