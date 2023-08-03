/**
 *  @file AmbaRTSL_MMU_VMSAv8-32.h
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
 *  @details Common definitions & constants for AArch32 Virtual Memory System Architecture
 *
 */

#ifndef AMBA_RTSL_MMU_VMSA_V8_32_H
#define AMBA_RTSL_MMU_VMSA_V8_32_H

/*
 *  Virtual Memory System Architecture version 8 (VMSAv8-32) level 1 short-descriptor formats
 */
#define AMBA_TLB_L1_SHORT_DESC_INVALID          0x0U    /* Invalid entry */
#define AMBA_TLB_L1_SHORT_DESC_PAGE_TABLE       0x1U    /* Page table */
#define AMBA_TLB_L1_SHORT_DESC_SECTION          0x2U    /* Section or Supersection */
#define AMBA_TLB_L1_SHORT_DESC_SECTION_PXN      0x3U    /* Section or Supersection, if the implementation supports the Privileged eXecute-Never attribute */

#define AMBA_TLB_L1_PAGE_TABLE_ADDR_OFFSET      10UL
#define AMBA_TLB_L1_SUPERSECTION_ADDR_OFFSET    24UL
#define AMBA_TLB_L1_SECTION_ADDR_OFFSET         20UL

static inline UINT32 AMBA_TLB_L1_PAGE_TABLE_BASE(UINT32 x)
{
    return ((x) >> AMBA_TLB_L1_PAGE_TABLE_ADDR_OFFSET);
}
static inline UINT32 AMBA_TLB_L1_SUPERSECTION_BASE(UINT32 x)
{
    return ((x) >> AMBA_TLB_L1_SUPERSECTION_ADDR_OFFSET);
}
static inline UINT32 AMBA_TLB_L1_SECTION_BASE(UINT32 x)
{
    return ((x) >> AMBA_TLB_L1_SECTION_ADDR_OFFSET);
}
#define AMBA_TLB_L1_PAGE_TABLE_BASE_MASK        0xfffffc00U
#define AMBA_TLB_L1_SECTION_BASE_MASK           0xfff00000U
#define AMBA_TLB_L1_SUPERSECTION_BASE_MASK      0x000000ffff000000ULL

typedef struct {
    UINT32  DescType:               2;      /* [1:0]:   0b00 = Invalid entry */
    UINT32  Reserved:               30;     /* [31:2]:  IGNORED */
} AMBA_TLB_S_DESC_L1_INVLD_s;

typedef struct {
    UINT32  DescType:               2;      /* [1:0]:   0b01 = Page table descriptor */
    UINT32  PrivExeNever:           1;      /* [2]:     1 = Privileged eXecute-Never */
    UINT32  NonSecure:              1;      /* [3]:     Non-secure bit. */
    UINT32  Reserved0:              1;      /* [4]:     SBZ */
    UINT32  Domain:                 4;      /* [8:5]:   Domain field. */
    UINT32  Reserved1:              1;      /* [9]:     IMP */
    UINT32  BaseAddr:               22;     /* [31:10]: Page table base address, bits [31:10]. */
} AMBA_TLB_S_DESC_L1_PG_TBL_s;

typedef struct {
    UINT32  DescType:               2;      /* [1:0]:   0b1x = Section or Supersection descriptor. Bit[0]: 0 = Executable, 1 = Privileged Execute-Never. */
    UINT32  Buffered:               1;      /* [2]:     Memory region attribute bits. Bufferable bit */
    UINT32  Cached:                 1;      /* [3]:     Memory region attribute bits. Cacheable bit. */
    UINT32  ExeNever:               1;      /* [4]:     The execute-never bit. Determines whether the region is executable. */
    UINT32  Domain:                 4;      /* [8:5]:   Domain field. */
    UINT32  Reserved:               1;      /* [9]:     IMP */
    UINT32  AP:                     2;      /* [11:10]: Access Permissions bits[1:0]. */
    UINT32  TEX:                    3;      /* [14:12]: Memory region attribute bits. */
    UINT32  AP2:                    1;      /* [15]:    Access Permissions bit[2]. */
    UINT32  Shared:                 1;      /* [16]:    Determines whether the translation is for Shareable memory. */
    UINT32  NotGlobal:              1;      /* [17]:    The not global bit. Determines how the translation is marked in the TLB */
    UINT32  SectType:               1;      /* [18]:    Must be 0 for Section */
    UINT32  NonSecure:              1;      /* [19]:    Non-secure bit. */
    UINT32  BaseAddr:               12;     /* [31:20]: Section base address, PA[31:20]. */
} AMBA_TLB_S_DESC_L1_SEC_s;

typedef struct {
    UINT32  DescType:               2;      /* [1:0]:   0b1x = Section or Supersection descriptor. Bit[0]: 0 = Executable, 1 = Privileged Execute-Never. */
    UINT32  Buffered:               1;      /* [2]:     Memory region attribute bits. Bufferable bit */
    UINT32  Cached:                 1;      /* [3]:     Memory region attribute bits. Cacheable bit. */
    UINT32  ExeNever:               1;      /* [4]:     The execute-never bit. Determines whether the region is executable. */
    UINT32  Domain:                 4;      /* [8:5]:   Domain field. */
    UINT32  Reserved:               1;      /* [9]:     IMP */
    UINT32  AP:                     2;      /* [11:10]: Access Permissions bits[1:0]. */
    UINT32  TEX:                    3;      /* [14:12]: Memory region attribute bits. */
    UINT32  AP2:                    1;      /* [15]:    Access Permissions bit[2]. */
    UINT32  Shared:                 1;      /* [16]:    Determines whether the translation is for Shareable memory. */
    UINT32  NotGlobal:              1;      /* [17]:    The not global bit. Determines how the translation is marked in the TLB */
    UINT32  SectType:               1;      /* [18]:    Must be 1 for SuperSection */
    UINT32  NonSecure:              1;      /* [19]:    Non-secure bit. */
    UINT32  BaseAddr:               12;     /* [31:20]: Section base address, PA[31:20]. */
} AMBA_TLB_S_DESC_L1_SUPER_SEC_s;

typedef struct {
    UINT32  Data;
} AMBA_TLB_S_DESC_L1_s;

/*
 *  Virtual Memory System Architecture version 8 (VMSAv8) level 2 short-descriptor formats
 */
#define AMBA_TLB_L2_SHORT_DESC_INVALID          0x0U    /* Invalid entry */
#define AMBA_TLB_L2_SHORT_DESC_LARGE_PAGE       0x1U    /* Large page */
#define AMBA_TLB_L2_SHORT_DESC_SMALL_PAGE       0x2U    /* Small page */

#define AMBA_TLB_L2_SMALL_PAGE_ADDR_OFFSET      12UL
#define AMBA_TLB_L2_LARGE_PAGE_ADDR_OFFSET      16UL

static inline UINT32 AMBA_TLB_L2_SMALL_PAGE_BASE(UINT32 x)
{
    return ((x) >> AMBA_TLB_L2_SMALL_PAGE_ADDR_OFFSET);
}
static inline UINT32 AMBA_TLB_L2_LARGE_PAGE_BASE(UINT32 x)
{
    return ((x) >> AMBA_TLB_L2_LARGE_PAGE_ADDR_OFFSET);
}
#define AMBA_TLB_L2_SMALL_PAGE_BASE_MASK        0xfffff000U
#define AMBA_TLB_L2_LARGE_PAGE_BASE_MASK        0xffff0000U

typedef struct {
    UINT32  DescType:               2;      /* [1:0]:   0b00 = Invalid entry */
    UINT32  Reserved:               30;     /* [31:2]:  IGNORE */
} AMBA_TLB_S_DESC_L2_INVLD_s;

typedef struct {
    UINT32  DescType:               2;      /* [1:0]:   0b01 = Large page descriptor. */
    UINT32  Buffered:               1;      /* [2]:     Memory region attribute bits. Bufferable bit */
    UINT32  Cached:                 1;      /* [3]:     Memory region attribute bits. Cacheable bit. */
    UINT32  AP:                     2;      /* [5:4]:   Access Permissions bits[1:0]. */
    UINT32  Reserved0:              3;      /* [8:6]:   SBZ. */
    UINT32  AP2:                    1;      /* [9]:     Access Permissions bit[2]. */
    UINT32  Shared:                 1;      /* [10]:    Determines whether the translation is for Shareable memory. */
    UINT32  NotGlobal:              1;      /* [11]:    The not global bit. Used in the TLB matching process. */
    UINT32  TEX:                    3;      /* [14:12]: Memory region attribute bits. */
    UINT32  ExeNever:               1;      /* [15]:    The execute-never bit. Determines whether the region is executable. */
    UINT32  BaseAddr:               16;     /* [31:16]: Small page base address, PA[31:12]. */
} AMBA_TLB_S_DESC_L2_LARGE_PG_s;

typedef struct {
    UINT32  DescType:               2;      /* [1:0]:   0b1x = Small page descriptor. Bit[0]: 0 = Executable, 1 = Execute-Never. */
    UINT32  Buffered:               1;      /* [2]:     Memory region attribute bits. Bufferable bit */
    UINT32  Cached:                 1;      /* [3]:     Memory region attribute bits. Cacheable bit. */
    UINT32  AP:                     2;      /* [5:4]:   Access Permissions bits[1:0]. */
    UINT32  TEX:                    3;      /* [8:6]:   Memory region attribute bits. */
    UINT32  AP2:                    1;      /* [9]:     Access Permissions bit[2]. */
    UINT32  Shared:                 1;      /* [10]:    Determines whether the translation is for Shareable memory. */
    UINT32  NotGlobal:              1;      /* [11]:    The not global bit. Used in the TLB matching process. */
    UINT32  BaseAddr:               20;     /* [31:12]: Small page base address, PA[31:12]. */
} AMBA_TLB_S_DESC_L2_SMALL_PG_s;

typedef struct {
    UINT32  Data;
} AMBA_TLB_S_DESC_L2_s;

/*
 *  Virtual Memory System Architecture version 8 (VMSAv8-32) level 1 long-descriptor formats
 */
#define AMBA_TLB_L1_LONG_DESC_INVALID0          0x0U    /* Invalid entry */
#define AMBA_TLB_L1_LONG_DESC_BLOCK             0x1U    /* Block */
#define AMBA_TLB_L1_LONG_DESC_INVALID1          0x2U    /* Invalid entry */
#define AMBA_TLB_L1_LONG_DESC_TABLE             0x3U    /* Table */

#define AMBA_TLB_L1_BLOCK_ADDR_OFFSET           30ULL
#define AMBA_TLB_L1_TABLE_ADDR_OFFSET           12ULL
#define AMBA_TLB_L1_BLOCK_BASE_MASK             0x000000ffc0000000ULL
#define AMBA_TLB_L1_TABLE_BASE_MASK             0x000000fffffff000ULL

typedef struct {
    UINT32  DescType:               2;      /* [1:0]:   0bx0 = Invalid entry */
    UINT32  Reserved0:              30;     /* [31:2]:  IGNORED */
    UINT32  Reserved1:              32;     /* [63:32]: IGNORED */
} AMBA_TLB_L_DESC_L1_INVLD_s;

typedef struct {
    UINT32  DescType:               2;      /* [1:0]:   0b01 = Block descriptor */
    UINT32  AttrIndex:              3;      /* [4:2]:   Memory attributes index field[2:0] */
    UINT32  NonSecure:              1;      /* [5]:     Non-secure bit. */
    UINT32  AP:                     2;      /* [7:6]:   Access Permissions bits[2:1]. */
    UINT32  Shareability:           2;      /* [9:8]:   Determines whether the translation is for Shareable memory. */
    UINT32  AccessFlag:             1;      /* [10]:    */
    UINT32  NotGlobal:              1;      /* [11]:    The not global bit. Used in the TLB matching process. */
    UINT32  Reserved0:              18;     /* [29:12]: SBZ */
    UINT32  OutputAddrL:            2;      /* [31:30]: Output address (lower bits) */
    UINT32  OutputAddrH:            8;      /* [39:32]: Output address (higher bits) */
    UINT32  Reserved1:              12;     /* [51:40]: SBZ */
    UINT32  Contiguous:             1;      /* [52]:    Indicateds that 16 adjacent translation table entries point to contiguout memory regions */
    UINT32  PrivExeNever:           1;      /* [53]:    (Stage 1 only) PXN bit */
    UINT32  ExeNever:               1;      /* [54]:    XN bit */
    UINT32  Reserved2:              9;      /* [63:55]: IGNORED */
} AMBA_TLB_L_DESC_L1_BLK_s;

typedef struct {
    UINT32  DescType:               2;      /* [1:0]:   0b11 = Table descriptor */
    UINT32  Reserved0:              10;     /* [11:2]:  IGNORED */
    UINT32  L2TableAddrL:           20;     /* [31:12]: Level 2 table address (lower bits) */
    UINT32  L2TableAddrH:           8;      /* [39:32]: Level 2 table address (higher bits) */
    UINT32  Reserved1:              12;     /* [51:40]: SBZ */
    UINT32  Reserved2:              7;      /* [58:52]: IGNORED */
    UINT32  PrivExeNeverTable:      1;      /* [59]:    (Stage 1 only) PXN table */
    UINT32  ExeNeverTable:          1;      /* [60]:    (Stage 1 only) XN table */
    UINT32  ApTable:                2;      /* [62:61]: (Stage 1 only) AP table */
    UINT32  NonSecureTable:         1;      /* [63]:    (Stage 1 only) Non-secure table */
} AMBA_TLB_L_DESC_L1_TBL_s;

typedef struct {
    UINT64  Data;
} AMBA_TLB_L_DESC_L1_s;

/*
 *  Virtual Memory System Architecture version 8 (VMSAv8-32) level 2 long-descriptor formats
 */
#define AMBA_TLB_L2_LONG_DESC_INVALID0          0x0U    /* Invalid entry */
#define AMBA_TLB_L2_LONG_DESC_BLOCK             0x1U    /* Block */
#define AMBA_TLB_L2_LONG_DESC_INVALID1          0x2U    /* Invalid entry */
#define AMBA_TLB_L2_LONG_DESC_TABLE             0x3U    /* Table */

#define AMBA_TLB_L2_BLOCK_ADDR_OFFSET           21UL
#define AMBA_TLB_L2_TABLE_ADDR_OFFSET           12UL
#define AMBA_TLB_L2_BLOCK_BASE_MASK             0x000000ffffe00000ULL
#define AMBA_TLB_L2_TABLE_BASE_MASK             0x000000fffffff000ULL

typedef struct {
    UINT32  DescType:               2;      /* [1:0]:   0bx0 = Invalid entry */
    UINT32  Reserved0:              30;     /* [31:2]:  IGNORED */
    UINT32  Reserved1:              32;     /* [63:32]: IGNORED */
} AMBA_TLB_L_DESC_L2_INVLD_s;

typedef struct {
    UINT32  DescType:               2;      /* [1:0]:   0b01 = Block descriptor */
    UINT32  AttrIndex:              3;      /* [4:2]:   Memory attributes index field[2:0] */
    UINT32  NonSecure:              1;      /* [5]:     Non-secure bit. */
    UINT32  AP:                     2;      /* [7:6]:   Access Permissions bits[2:1]. */
    UINT32  Shareability:           2;      /* [9:8]:   Determines whether the translation is for Shareable memory. */
    UINT32  AccessFlag:             1;      /* [10]:    */
    UINT32  NotGlobal:              1;      /* [11]:    The not global bit. Used in the TLB matching process. */
    UINT32  Reserved0:              9;      /* [20:12]: SBZ */
    UINT32  OutputAddrL:            11;     /* [31:21]: Output address (lower bits) */
    UINT32  OutputAddrH:            8;      /* [39:32]: Output address (higher bits) */
    UINT32  Reserved1:              12;     /* [51:40]: SBZ */
    UINT32  Contiguous:             1;      /* [52]:    Indicateds that 16 adjacent translation table entries point to contiguout memory regions */
    UINT32  PrivExeNever:           1;      /* [53]:    (Stage 1 only) PXN bit */
    UINT32  ExeNever:               1;      /* [54]:    XN bit */
    UINT32  Reserved2:              9;      /* [63:55]: IGNORED */
} AMBA_TLB_L_DESC_L2_BLK_s;

typedef struct {
    UINT32  DescType:               2;      /* [1:0]:   0b11 = Table descriptor */
    UINT32  Reserved0:              10;     /* [11:2]:  IGNORED */
    UINT32  L3TableAddrL:           20;     /* [31:12]: Level 3 table address (lower bits) */
    UINT32  L3TableAddrH:           8;      /* [39:32]: Level 3 table address (higher bits) */
    UINT32  Reserved1:              12;     /* [51:40]: SBZ */
    UINT32  Reserved2:              7;      /* [58:52]: IGNORED */
    UINT32  PrivExeNeverTable:      1;      /* [59]:    (Stage 1 only) PXN table */
    UINT32  ExeNeverTable:          1;      /* [60]:    (Stage 1 only) XN table */
    UINT32  ApTable:                2;      /* [62:61]: (Stage 1 only) AP table */
    UINT32  NonSecureTable:         1;      /* [63]:    (Stage 1 only) Non-secure table */
} AMBA_TLB_L_DESC_L2_TBL_s;

typedef struct {
    UINT64  Data;
} AMBA_TLB_L_DESC_L2_s;

/*
 *  Virtual Memory System Architecture version 8 (VMSAv8-32) level 3 long-descriptor formats
 */
#define AMBA_TLB_L3_LONG_DESC_INVALID0          0x0U    /* Invalid entry */
#define AMBA_TLB_L3_LONG_DESC_RESERVED          0x1U    /* Reserved, Invalid entry */
#define AMBA_TLB_L3_LONG_DESC_INVALID1          0x2U    /* Invalid entry */
#define AMBA_TLB_L3_LONG_DESC_PAGE              0x3U    /* Page */

#define AMBA_TLB_L3_PAGE_ADDR_OFFSET            12UL
#define AMBA_TLB_L3_PAGE_BASE_MASK              0x000000fffffff000ULL

typedef struct {
    UINT32  DescType:               2;      /* [1:0]:   0bx0 = Invalid entry */
    UINT32  Reserved0:              30;     /* [31:2]:  IGNORED */
    UINT32  Reserved1:              32;     /* [63:32]: IGNORED */
} AMBA_TLB_L_DESC_L3_INVLD_s;

typedef struct {
    UINT32  DescType:               2;      /* [1:0]:   0b01 = Reserved, Invalid entry */
    UINT32  Reserved0:              30;     /* [31:2]:  SBZ */
    UINT32  Reserved1:              32;     /* [63:32]: SBZ */
} AMBA_TLB_L_DESC_L3_RSVD_s;

typedef struct {
    UINT32  DescType:               2;      /* [1:0]:   0b11 = Page descriptor */
    UINT32  AttrIndex:              3;      /* [4:2]:   Memory attributes index field[2:0] */
    UINT32  NonSecure:              1;      /* [5]:     Non-secure bit. */
    UINT32  AP:                     2;      /* [7:6]:   Access Permissions bits[2:1]. */
    UINT32  Shareability:           2;      /* [9:8]:   Determines whether the translation is for Shareable memory. */
    UINT32  AccessFlag:             1;      /* [10]:    */
    UINT32  NotGlobal:              1;      /* [11]:    The not global bit. Used in the TLB matching process. */
    UINT32  OutputAddrL:            20;     /* [31:12]: Output address (lower bits) */
    UINT32  OutputAddrH:            8;      /* [39:32]: Output address (higher bits) */
    UINT32  Reserved:               12;     /* [51:40]: SBZ */
    UINT32  Contiguous:             1;      /* [52]:    Indicateds that 16 adjacent translation table entries point to contiguout memory regions */
    UINT32  PrivExeNever:           1;      /* [53]:    (Stage 1 only) PXN bit */
    UINT32  ExeNever:               1;      /* [54]:    XN bit */
    UINT32  Reserved2:              9;      /* [63:55]: IGNORED */
} AMBA_TLB_L_DESC_L3_PG_s;

typedef struct {
    UINT64  Data;
} AMBA_TLB_L_DESC_L3_s;

/* Definitions for 32-bit translation system. */
static inline UINT32 AMBA_TLB_GET_L1_SECT_ID(UINT32 x)
{
    return (((x) >> 20U) & 0xfffU);    /* [31:20] 0 ~ 4095. Section Number. */
}
static inline UINT32 AMBA_TLB_GET_L2_LARGE_PAGE_ID(UINT32 x)
{
    return (((x) >> 16U) & 0xfU);    /* [19:16] 0 ~ 15 */
}
static inline UINT32 AMBA_TLB_GET_L2_SMALL_PAGE_ID(UINT32 x)
{
    return (((x) >> 12U) & 0xffU);    /* [19:12] 0 ~ 255 */
}

static inline UINT32 AMBA_TLB_GET_L1_BLOCK_ID(UINT32 x)
{
    return (((x) >> 30U) & 0x3U);    /* [31:30] 0 ~ 3 */
}
static inline UINT32 AMBA_TLB_GET_L2_BLOCK_ID(UINT32 x)
{
    return (((x) >> 21U) & 0x1ffU);    /* [29:21] 0 ~ 511 */
}
static inline UINT32 AMBA_TLB_GET_L3_PAGE_ID(UINT32 x)
{
    return (((x) >> 12U) & 0x1ffU);    /* [20:12] 0 ~ 511 */
}

/* MMU TLB Descriptors */
#define AMBA_NUM_TLB_L1_SHORT_DESC      4096U    /* Number of L1 descriptors mapped to 4GB DRAM size */
#define AMBA_NUM_TLB_L2_SHORT_DESC      256U     /* Number of L2 descriptors mapped to one L1 page table entry */
#define AMBA_NUM_TLB_L1_LONG_DESC       4U       /* Number of L1 descriptors mapped to 4GB DRAM size */
#define AMBA_NUM_TLB_L2_LONG_DESC       512U     /* Number of L2 descriptors mapped to one L1 page table entry */
#define AMBA_NUM_TLB_L3_LONG_DESC       512U     /* Number of L3 descriptors mapped to one L2 page table entry */

typedef struct {
    /* Aligned on a 16 KB boundary */
    AMBA_TLB_S_DESC_L1_s L1Desc[AMBA_NUM_TLB_L1_SHORT_DESC];                                /* Occupy 16KB of L1 table (maps 1MB) */
    /* Aligned on a 1 KB boundary */
    AMBA_TLB_S_DESC_L2_s L2Desc[AMBA_NUM_TLB_L1_SHORT_DESC][AMBA_NUM_TLB_L2_SHORT_DESC];    /* Occupy 4MB of L2 table (maps 4KB/64KB of each 1MB) */
} AMBA_TLB_S_DESC_s;

typedef struct {
    /* Aligned on a 16 KB boundary */
    AMBA_TLB_L_DESC_L1_s L1Desc[AMBA_NUM_TLB_L1_LONG_DESC];                                                         /* Occupy 32B of L1 table (maps 1GB) */
    UINT64 Reserved[512U - AMBA_NUM_TLB_L1_LONG_DESC];                                                              /* For alignment */
    /* Aligned on a 4 KB boundary */
    AMBA_TLB_L_DESC_L2_s L2Desc[AMBA_NUM_TLB_L1_LONG_DESC][AMBA_NUM_TLB_L2_LONG_DESC];                              /* Occupy 16KB of L2 table (maps 2MB of each 1GB) */
    /* Aligned on a 4 KB boundary */
    AMBA_TLB_L_DESC_L3_s L3Desc[AMBA_NUM_TLB_L1_LONG_DESC][AMBA_NUM_TLB_L2_LONG_DESC][AMBA_NUM_TLB_L3_LONG_DESC];   /* Occupy 8MB of L3 table (maps 4KB of each 2MB) */
} AMBA_TLB_L_DESC_s;

#endif  /* AMBA_RTSL_MMU_VMSA_V8_32_H */
