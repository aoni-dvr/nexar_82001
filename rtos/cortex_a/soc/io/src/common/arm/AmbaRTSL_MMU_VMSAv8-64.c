/**
 *  @file AmbaRTSL_MMU_VMSAv8-64.c
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

#include "AmbaDef.h"
#include "AmbaIntrinsics.h"
#include "AmbaMisraFix.h"
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#include "AmbaCortexA76.h"
#else
#include "AmbaCortexA53.h"
#endif
#include "AmbaDebugPort.h"

#include "AmbaMMU.h"
#include "AmbaRTSL_Cache.h"
#include "AmbaRTSL_CPU.h"
#include "AmbaRTSL_MMU.h"
#include "AmbaRTSL_MMU_Priv.h"
#include "AmbaRTSL_MMU_VMSAv8-64.h"

/* The page table needs to align on a 4 KB boundary (4KB GRANULE) or 64 KB boundary (64KB GRANULE) */
#if defined(CONFIG_XEN_SUPPORT) && !defined(AMBA_FWPROG)
/* Page size alignment at .lds */
static AMBA_TLB_GRANULE_4KB_s  AmbaMmuPageTable __attribute__((section(".data.mmu_ptbl")));
#else
/* We always use 64 KB boundary here to support both granule size */
static AMBA_TLB_GRANULE_4KB_s AmbaMmuPageTable __attribute__((aligned(0x10000))) GNU_SECTION_NOZEROINIT;
#endif

static AMBA_TLB_GRANULE_4KB_s *pAmbaTlb4KB = NULL;
static AMBA_TLB_GRANULE_64KB_s *pAmbaTlb64KB = NULL;
#if defined (CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
static UINT32 MmuTlbL3Index = 0U;
#endif
static AMBA_MMU_MAP_INFO64_s AmbaMmuMmioMap[AMBA_MMU_NUM_MMIO_REGION];

/**
 *  MMU_ZeroMemory - Zero memory. The AArch64 memset might use cache operation however data caching is not yet enabled here.
 *  @param[in] pBuf Pointer to the block of memory to fill zeros
 *  @param[in] Size Number of bytes to be set to zero
 */
static void MMU_ZeroMemory(const void *pBuf, UINT64 Size)
{
    UINT64 *pDWORD;
    UINT64 SizeTmp = Size;

    AmbaMisra_TypeCast64(&pDWORD, &pBuf);

    while (SizeTmp > 0U) {
        *pDWORD = 0U;
        SizeTmp -= 8U;
        pDWORD++;
    }
}

static UINT32 MMU_CheckAttr(const UINT64 MemAttr)
{
    UINT32 RetVal = 0;

    if (MemAttr == 0x00U) {         // Device-nGnRnE
        RetVal = 0U;
    } else if (MemAttr == 0x04U) {  // Device-nGnRE
        RetVal = 1U;
    } else if (MemAttr == 0x77U) {  // Normal Memory, Write-Back Cacheable
        /* the memory is cached in the L1 Data cache and the L2 cache iff it is marked as Inner and Outer Write-Back Cacheable. */
        RetVal = 3U;
    } else {                                    // Normal Memory, Non-Cacheable
        /* Cortex-A53 doesn't support Write-Through Cacheable */
        RetVal = 2U;
    }

    return RetVal;
}

/**
 *  MMU_TlbReset - Initialize translation table
 */
static void MMU_TlbReset(void)
{
    AMBA_TLB_GRANULE_4KB_s *pPageTblBase = &AmbaMmuPageTable;
    UINT32 Size;

    AmbaMisra_TypeCast64(&pAmbaTlb4KB, &pPageTblBase);
    AmbaMisra_TypeCast64(&pAmbaTlb64KB, &pPageTblBase);

    (void)AmbaRTSL_CacheCleanDataPtr(&pAmbaTlb4KB, (UINT32)sizeof(AMBA_TLB_GRANULE_4KB_s *));
    (void)AmbaRTSL_CacheCleanDataPtr(&pAmbaTlb64KB, (UINT32)sizeof(AMBA_TLB_GRANULE_64KB_s *));

#if !defined(CONFIG_XEN_SUPPORT) || defined(AMBA_FWPROG)
    /* There is no need to invalidate L2 table. Invalidate L0&L1 table only to shorten boot flow.*/
    Size = AMBA_NUM_TLB_L1_4KB_DESC_PARTIAL;
    while (Size > 0U) {
        Size --;
        pPageTblBase->L1Desc[Size].Data = 0U;
    }
    (void)AmbaRTSL_CacheCleanDataPtr(pPageTblBase->L1Desc, AMBA_NUM_TLB_L1_4KB_DESC_PARTIAL * (UINT32)sizeof(AMBA_TLB_VMSAV8_L1_s));
#else
    (void)Size;
#endif // CONFIG_XEN_SUPPORT
}

static void MMU_GetL3IdxStart(UINT64 L2DescIdx, UINT64 L2DescRef, UINT64 VirtAddr, UINT64 *pL3Idx)
{
    if (L2DescIdx == L2DescRef) {
        *pL3Idx = AMBA_TLB_GET_L3_4KB_DESC_ID(VirtAddr);
    } else {
        *pL3Idx = 0U;
    }
}

static void MMU_GetL3IdxEnd(UINT64 L2DescIdx, UINT64 L2DescRef, UINT64 VirtAddr, UINT64 *pL3Idx)
{
    if (L2DescIdx == L2DescRef) {
        *pL3Idx = AMBA_TLB_GET_L3_4KB_DESC_ID(VirtAddr);
    } else {
        *pL3Idx = AMBA_NUM_TLB_L3_4KB_DESC - 1U;;
    }
}

/**
 *  MMU_TlbSetupL1Block_4KB - Setup long-descriptor L1 block descriptor.
 *  @param[in] pPageTable Pointer to translation table
 *  @param[in] pMapInfo Pointer to user-supplied MMU map entry.
 *  @return error code
 */
static UINT32 MMU_TlbSetupL1Block_4KB(const AMBA_MMU_MAP_INFO64_s *pMapInfo)
{
    const AMBA_TLB_VMSAV8_L1_s *pL1Desc;

    AMBA_TLB_VMSAV8_L1_BLK_s L1Block = {0U};
    AMBA_TLB_VMSAV8_L1_BLK_s *pL1Block, TmpL1Block;
    UINT64 L1DescStart, L1DescEnd;
    UINT64 L1BlockBase;
    UINT64 L1DescID;
    UINT32 RetVal = MMU_ERR_NONE;
    AMBA_MMU_MAP_ATTR64_s MemAttrBits;

    AmbaMisra_TypeCast64(&MemAttrBits, &pMapInfo->MemAttr);

    L1DescStart = AMBA_TLB_GET_L1_4KB_DESC_ID(pMapInfo->VirtAddr);
    L1DescEnd   = AMBA_TLB_GET_L1_4KB_DESC_ID(pMapInfo->VirtAddr + pMapInfo->MemSize - 1U);
    L1BlockBase = pMapInfo->PhysAddr & AMBA_TLB_L1_4KB_BLOCK_BASE_MASK;

    if ((L1DescStart >= AMBA_NUM_TLB_L1_4KB_DESC_PARTIAL) || (L1DescEnd >= AMBA_NUM_TLB_L1_4KB_DESC_PARTIAL)) {
        RetVal = MMU_ERR_ARG;
    } else {
        L1Block.DescType = AMBA_TLB_L1_DESC_BLOCK;

        L1Block.AttrIndex    = (UINT8)MMU_CheckAttr(MemAttrBits.MemAttr);
        L1Block.NonSecure    = MemAttrBits.NonSecure;
        L1Block.AP           = MemAttrBits.S1AP;
        L1Block.Shareability = MemAttrBits.Shareability;
        L1Block.AccessFlag   = 1U;
        L1Block.NotGlobal    = MemAttrBits.NotGlobal;
        L1Block.Contiguous   = 0U;
        L1Block.PrivExeNever = MemAttrBits.PXN;
        L1Block.ExeNever     = MemAttrBits.UXN;

        /* pL1Block = &pAmbaTlb4KB->L1Desc[L1DescStart]; */
        pL1Desc = &pAmbaTlb4KB->L1Desc[L1DescStart];
        AmbaMisra_TypeCast64(&pL1Block, &pL1Desc);
        for (L1DescID = L1DescStart; L1DescID <= L1DescEnd; L1DescID++) {
            /* pL1Desc->OutputAddr = L1BlockBase >> AMBA_TLB_L1_4KB_BLOCK_ADDR_OFFSET; */
            AmbaMisra_TypeCast64(&TmpL1Block, &L1BlockBase);
            L1Block.OutputAddrL = TmpL1Block.OutputAddrL;
            L1Block.OutputAddrH = TmpL1Block.OutputAddrH;
            L1BlockBase += (1ULL << AMBA_TLB_L1_4KB_BLOCK_ADDR_OFFSET);

            AmbaMisra_TypeCast64(pL1Block, &L1Block);
            pL1Block++;
        }
        (void)AmbaRTSL_CacheFlushDataPtr(&pAmbaTlb4KB->L1Desc[L1DescStart], (ULONG)((L1DescEnd - L1DescStart) + 1U) * (UINT32)sizeof(AMBA_TLB_VMSAV8_L1_s));
    }

    return RetVal;
}

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
static inline AMBA_TLB_VMSAV8_L2_s* AMBA_TLB_GET_L2_4KB_ENTRY(ULONG x, UINT32 *pRetVal)
{
    AMBA_TLB_VMSAV8_L2_s *pL2EntPtr = NULL;
    UINT32 Index = 0U;

    if (x < AMBA_NUM_TLB_L1_4KB_DESC_PARTIAL0) {
        /* case : 0x0~0x10_0000_0000 */
        pL2EntPtr = pAmbaTlb4KB->L2Desc_00[(UINT32)x];
    } else if (x < ((4U * 16U * 1U) + AMBA_NUM_TLB_L1_4KB_DESC_PARTIAL1)) {
        /* case : 0x10_0000_0000 ~ 0x11_0000_0000 */
        Index = ((UINT32)x - (4U * 16U * 1U));
        pL2EntPtr = pAmbaTlb4KB->L2Desc_10[Index];
    } else if ((x >= (4U * 16U * 2U) ) && ( x < ((4U * 16U * 2U) + AMBA_NUM_TLB_L1_4KB_DESC_PARTIAL2))) {
        /* case : 0x20_0000_0000 ~ 0x21_0000_0000 */
        Index = ((UINT32)x - (4U * 16U * 2U));
        pL2EntPtr = pAmbaTlb4KB->L2Desc_20[Index];
    } else {
        *pRetVal = MMU_ERR_UNEXPECTED;
    }

    return pL2EntPtr;
}
#endif

/**
 *  MMU_TlbSetupL2Block_4KB - Setup long-descriptor L2 block descriptor.
 *  @param[in] pPageTable Pointer to translation table
 *  @param[in] pMapInfo Pointer to user-supplied MMU map entry.
 *  @return error code
 */
static UINT32 MMU_TlbSetupL2Block_4KB(const AMBA_MMU_MAP_INFO64_s *pMapInfo)
{
    AMBA_TLB_VMSAV8_L1_TBL_s *pL1Table, TmpL1Table;

    AMBA_TLB_VMSAV8_L2_BLK_s L2Block = {0};
    AMBA_TLB_VMSAV8_L2_BLK_s TmpL2Block;

    const AMBA_TLB_VMSAV8_L1_s *pL1Desc;
    AMBA_TLB_VMSAV8_L2_s *pL2Desc;
    UINT64 L1DescStart, L1DescEnd;
    UINT64 L2DescStart, L2DescEnd;
    UINT64 L2BlockBase;
    UINT64 L1DescID, L2DescID;
    ULONG VirAddr, PhyAddr;
    UINT32 RetVal = MMU_ERR_NONE;
    AMBA_MMU_MAP_ATTR64_s MemAttrBits;

    AmbaMisra_TypeCast64(&MemAttrBits, &pMapInfo->MemAttr);

    L1DescStart = AMBA_TLB_GET_L1_4KB_DESC_ID(pMapInfo->VirtAddr);
    L1DescEnd   = AMBA_TLB_GET_L1_4KB_DESC_ID(pMapInfo->VirtAddr + pMapInfo->MemSize - 1U);
    L2BlockBase = pMapInfo->PhysAddr & AMBA_TLB_L2_4KB_BLOCK_BASE_MASK;

    if ((L1DescStart >= AMBA_NUM_TLB_L1_4KB_DESC_PARTIAL) || (L1DescEnd >= AMBA_NUM_TLB_L1_4KB_DESC_PARTIAL)) {
        RetVal = MMU_ERR_ARG;
    } else {
        L2Block.DescType = AMBA_TLB_L2_DESC_BLOCK;

        L2Block.AttrIndex = (UINT8)MMU_CheckAttr(MemAttrBits.MemAttr);
        L2Block.NonSecure    = MemAttrBits.NonSecure;
        L2Block.AP           = MemAttrBits.S1AP;
        L2Block.Shareability = MemAttrBits.Shareability;
        L2Block.AccessFlag   = 1U;
        L2Block.NotGlobal    = MemAttrBits.NotGlobal;
        L2Block.Contiguous   = 0U;
        L2Block.PrivExeNever = MemAttrBits.PXN;
        L2Block.ExeNever     = MemAttrBits.UXN;

        for (L1DescID = L1DescStart; (L1DescID <= L1DescEnd); L1DescID++) {
            if (MMU_ERR_NONE == RetVal) {
                pL1Desc = &pAmbaTlb4KB->L1Desc[L1DescID];
                AmbaMisra_TypeCast64(&pL1Table, &pL1Desc);

                if (pL1Table->DescType == AMBA_TLB_L1_DESC_BLOCK) {
                    /* This region is already mapped by L1 Block descriptor. */
                    break;
                } else if (pL1Table->DescType == AMBA_TLB_L1_DESC_TABLE) {
                    /* Use the table base address described in L1 Table descriptor. */
                    PhyAddr = (pL1Desc->Data & AMBA_TLB_L1_4KB_TABLE_BASE_MASK);
                    (void)AmbaRTSL_MmuPhys64ToVirt64(PhyAddr, &VirAddr);
                    AmbaMisra_TypeCast64(&pL2Desc, &VirAddr);
                    (void)AmbaRTSL_CacheFlushDataPtr(&pL2Desc, (UINT32)sizeof(AMBA_TLB_VMSAV8_L2_s *));
                } else {
                    /* Setup a new L1 Table descriptor. */
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
                    pL2Desc = AMBA_TLB_GET_L2_4KB_ENTRY(L1DescID, &RetVal);
#else
                    pL2Desc = pAmbaTlb4KB->L2Desc[L1DescID];
#endif

                    MMU_ZeroMemory(pL2Desc, (UINT64)AMBA_NUM_TLB_L2_4KB_DESC * sizeof(AMBA_TLB_VMSAV8_L2_s));
                    (void)AmbaRTSL_CacheCleanDataPtr(pL2Desc, AMBA_NUM_TLB_L2_4KB_DESC * (UINT32)sizeof(AMBA_TLB_VMSAV8_L2_s));

                    AmbaMisra_TypeCast64(&VirAddr, &pL2Desc);
                    (void)AmbaRTSL_MmuVirt64ToPhys64(VirAddr, &PhyAddr);
                    /* pL1Table->L2TableAddr = PhyAddr >> AMBA_TLB_L1_4KB_TABLE_ADDR_OFFSET; */
                    AmbaMisra_TypeCast64(&TmpL1Table, &PhyAddr);
                    pL1Table->L2TableAddrL = TmpL1Table.L2TableAddrL;
                    pL1Table->L2TableAddrH = TmpL1Table.L2TableAddrH;
                    pL1Table->DescType = AMBA_TLB_L1_DESC_TABLE;
                    (void)AmbaRTSL_CacheFlushDataPtr(pL1Desc, (UINT32)sizeof(AMBA_TLB_VMSAV8_L1_s));
                }

                if (L1DescID == L1DescStart) {
                    L2DescStart = AMBA_TLB_GET_L2_4KB_DESC_ID(pMapInfo->VirtAddr);
                } else {
                    L2DescStart = 0U;
                }

                if (L1DescID == L1DescEnd) {
                    L2DescEnd = AMBA_TLB_GET_L2_4KB_DESC_ID(pMapInfo->VirtAddr + pMapInfo->MemSize - 1U);
                } else {
                    L2DescEnd = AMBA_NUM_TLB_L2_4KB_DESC - 1U;
                }

                for (L2DescID = L2DescStart; (L2DescID <= L2DescEnd); L2DescID++) {
                    /* pL2Desc[L2DescID].OutputAddr = L2BlockBase >> AMBA_TLB_L2_4KB_BLOCK_ADDR_OFFSET; */
                    AmbaMisra_TypeCast64(&TmpL2Block, &L2BlockBase);
                    L2Block.OutputAddrL = TmpL2Block.OutputAddrL;
                    L2Block.OutputAddrH = TmpL2Block.OutputAddrH;
                    L2BlockBase += (1ULL << AMBA_TLB_L2_4KB_BLOCK_ADDR_OFFSET);

                    if (pL2Desc != NULL) {
                        AmbaMisra_TypeCast64(&pL2Desc[L2DescID], &L2Block);
                    } else {
                        break;
                    }
                }
                if (pL2Desc != NULL) {
                    (void)AmbaRTSL_CacheFlushDataPtr(&pL2Desc[L2DescStart], (ULONG)((L2DescEnd - L2DescStart) + 1U) * (UINT32)sizeof(AMBA_TLB_VMSAV8_L2_s));
                } else {
                    RetVal = MMU_ERR_UNEXPECTED;
                }
            }
        }
    }

    return RetVal;
}

/**
 *  MMU_TlbSetupL3Page_4KB - Setup long-descriptor L3 page descriptor.
 *  @param[in] pPageTable Pointer to translation table
 *  @param[in] pMapInfo Pointer to user-supplied MMU map entry.
 *  @return error code
 */
static UINT32 MMU_TlbSetupL3Page_4KB(const AMBA_MMU_MAP_INFO64_s *pMapInfo)
{
    AMBA_TLB_VMSAV8_L1_TBL_s *pL1Table, TmpL1Table;
    AMBA_TLB_VMSAV8_L2_TBL_s *pL2Table, TmpL2Table;

    AMBA_TLB_VMSAV8_L3_PG_s L3Page = {0};
    AMBA_TLB_VMSAV8_L3_PG_s TmpL3Page;

    const AMBA_TLB_VMSAV8_L1_s *pL1Desc;
    const AMBA_TLB_VMSAV8_L2_s *pL2Desc;
    AMBA_TLB_VMSAV8_L3_s *pL3Desc = NULL;
    UINT64 L1DescStart, L1DescEnd;
    UINT64 L2DescStart, L2DescEnd;
    UINT64 L3DescStart, L3DescEnd;
    UINT64 L3PageBase;
    UINT64 L1DescID, L2DescID, L3DescID;
    ULONG VirAddr, PhyAddr;
    UINT32 RetVal = MMU_ERR_NONE;
    AMBA_MMU_MAP_ATTR64_s MemAttrBits;

    AmbaMisra_TypeCast64(&MemAttrBits, &pMapInfo->MemAttr);

    L1DescStart = AMBA_TLB_GET_L1_4KB_DESC_ID(pMapInfo->VirtAddr);
    L1DescEnd   = AMBA_TLB_GET_L1_4KB_DESC_ID(pMapInfo->VirtAddr + pMapInfo->MemSize - 1U);
    L3PageBase  = pMapInfo->PhysAddr & AMBA_TLB_L3_4KB_PAGE_BASE_MASK;

    if ((L1DescStart >= AMBA_NUM_TLB_L1_4KB_DESC_PARTIAL) || (L1DescEnd >= AMBA_NUM_TLB_L1_4KB_DESC_PARTIAL)) {
        RetVal = MMU_ERR_ARG;
    } else {
        L3Page.DescType = AMBA_TLB_L3_DESC_PAGE;

        L3Page.AttrIndex = (UINT8)MMU_CheckAttr(MemAttrBits.MemAttr);
        L3Page.NonSecure     = MemAttrBits.NonSecure;
        L3Page.AP            = MemAttrBits.S1AP;
        L3Page.Shareability  = MemAttrBits.Shareability;
        L3Page.AccessFlag    = 1U;
        L3Page.NotGlobal     = MemAttrBits.NotGlobal;
        L3Page.Contiguous    = 0U;
        L3Page.PrivExeNever  = MemAttrBits.PXN;
        L3Page.ExeNever      = MemAttrBits.UXN;

        for (L1DescID = L1DescStart; L1DescID <= L1DescEnd; L1DescID++) {
            pL1Desc = &pAmbaTlb4KB->L1Desc[L1DescID];
            AmbaMisra_TypeCast64(&pL1Table, &pL1Desc);

            if (pL1Table->DescType == AMBA_TLB_L1_DESC_BLOCK) {
                /* This region is already mapped by L1 Block descriptor. */
                break;
            } else if (pL1Table->DescType == AMBA_TLB_L1_DESC_TABLE) {
                /* Use the table base address described in L1 Table descriptor. */
                PhyAddr = (pL1Desc->Data & AMBA_TLB_L1_4KB_TABLE_BASE_MASK);
                (void)AmbaRTSL_MmuPhys64ToVirt64(PhyAddr, &VirAddr);
                AmbaMisra_TypeCast64(&pL2Desc, &VirAddr);
                (void)AmbaRTSL_CacheFlushDataPtr(pL2Desc, (ULONG)sizeof(AMBA_TLB_VMSAV8_L2_s));
            } else {
                /* Setup a new L1 Table descriptor. */
#if defined (CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
                pL2Desc = AMBA_TLB_GET_L2_4KB_ENTRY(L1DescID, &RetVal);
#else
                pL2Desc = pAmbaTlb4KB->L2Desc[L1DescID];
#endif

                MMU_ZeroMemory(pL2Desc, (UINT64)AMBA_NUM_TLB_L2_4KB_DESC * sizeof(AMBA_TLB_VMSAV8_L2_s));
                (void)AmbaRTSL_CacheCleanDataPtr(pL2Desc, AMBA_NUM_TLB_L2_4KB_DESC * (UINT32)sizeof(AMBA_TLB_VMSAV8_L2_s));

                AmbaMisra_TypeCast64(&VirAddr, &pL2Desc);
                (void)AmbaRTSL_MmuVirt64ToPhys64(VirAddr, &PhyAddr);
                /* pL1Table->L2TableAddr = PhyAddr >> AMBA_TLB_L1_4KB_TABLE_ADDR_OFFSET; */
                AmbaMisra_TypeCast64(&TmpL1Table, &PhyAddr);
                pL1Table->L2TableAddrL = TmpL1Table.L2TableAddrL;
                pL1Table->L2TableAddrH = TmpL1Table.L2TableAddrH;
                pL1Table->DescType = AMBA_TLB_L1_DESC_TABLE;
                (void)AmbaRTSL_CacheFlushDataPtr(pL1Desc, (UINT32)sizeof(AMBA_TLB_VMSAV8_L1_s));
            }

            L2DescStart = 0U;
            L2DescEnd = AMBA_NUM_TLB_L2_4KB_DESC - 1U;
            if (L1DescID == L1DescStart) {
                L2DescStart = AMBA_TLB_GET_L2_4KB_DESC_ID(pMapInfo->VirtAddr);
            }

            if (L1DescID == L1DescEnd) {
                L2DescEnd = AMBA_TLB_GET_L2_4KB_DESC_ID(pMapInfo->VirtAddr + pMapInfo->MemSize - 1U);
            }

            for (L2DescID = L2DescStart; (L2DescID <= L2DescEnd); L2DescID++) {
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
                pL2Desc = AMBA_TLB_GET_L2_4KB_ENTRY(L1DescID, &RetVal);
                if (pL2Desc != NULL) {
                    pL2Desc = &pL2Desc[L2DescID];
                }
#else
                pL2Desc = &pAmbaTlb4KB->L2Desc[L1DescID][L2DescID];
#endif
                if (RetVal == MMU_ERR_NONE) {
                    AmbaMisra_TypeCast64(&pL2Table, &pL2Desc);

                    if (pL2Table->DescType == AMBA_TLB_L2_DESC_BLOCK) {
                        /* This region is already mapped by L2 Block descriptor. */
                        break;
                    } else if (pL2Table->DescType == AMBA_TLB_L2_DESC_TABLE) {
                        /* Use the table base address described in L2 Table descriptor. */
                        PhyAddr = (pL2Desc->Data & AMBA_TLB_L2_4KB_TABLE_BASE_MASK);
                        (void)AmbaRTSL_MmuPhys64ToVirt64(PhyAddr, &VirAddr);
                        AmbaMisra_TypeCast64(&pL3Desc, &VirAddr);
                        (void)AmbaRTSL_CacheFlushDataPtr(&pL3Desc, (UINT32)sizeof(AMBA_TLB_VMSAV8_L3_s *));
                    } else {
                        /* Setup a new L2 Table descriptor. */
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
                        if (MmuTlbL3Index <= (AMBA_NUM_TLB_L3_4KB_DESC_ALL - AMBA_NUM_TLB_L3_4KB_DESC)) {
                            pL3Desc = &pAmbaTlb4KB->L3Desc[MmuTlbL3Index];
                            MmuTlbL3Index += AMBA_NUM_TLB_L3_4KB_DESC;
                            (void)AmbaRTSL_CacheCleanDataPtr(&MmuTlbL3Index, (UINT32)sizeof(UINT32));
                        } else {
                            RetVal = MMU_ERR_UNEXPECTED;
                            AmbaAssert();
                        }
#else
                        pL3Desc = pAmbaTlb4KB->L3Desc[L1DescID][L2DescID];
#endif
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
                        if (RetVal == MMU_ERR_NONE) {
#endif
                            MMU_ZeroMemory(pL3Desc, (UINT64)AMBA_NUM_TLB_L3_4KB_DESC * sizeof(AMBA_TLB_VMSAV8_L3_s));
                            if (pL3Desc != NULL) {
                                (void)AmbaRTSL_CacheCleanDataPtr(pL3Desc, AMBA_NUM_TLB_L3_4KB_DESC * (UINT32)sizeof(AMBA_TLB_VMSAV8_L3_s));
                            }
                            AmbaMisra_TypeCast64(&VirAddr, &pL3Desc);
                            (void)AmbaRTSL_MmuVirt64ToPhys64(VirAddr, &PhyAddr);
                            /* pL2Table->L3TableAddr = PhyAddr >> AMBA_TLB_L2_4KB_TABLE_ADDR_OFFSET; */
                            AmbaMisra_TypeCast64(&TmpL2Table, &PhyAddr);
                            pL2Table->L3TableAddrL = TmpL2Table.L3TableAddrL;
                            pL2Table->L3TableAddrH = TmpL2Table.L3TableAddrH;
                            pL2Table->DescType = AMBA_TLB_L2_DESC_TABLE;
                            (void)AmbaRTSL_CacheFlushDataPtr(pL2Table, (UINT32)sizeof(AMBA_TLB_VMSAV8_L2_TBL_s));
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
                        }
#endif
                    }
                }

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
                if (RetVal == MMU_ERR_NONE) {
#endif
                    MMU_GetL3IdxStart(L2DescID, L2DescStart, pMapInfo->VirtAddr, &L3DescStart);
                    MMU_GetL3IdxEnd(L2DescID, L2DescEnd, (pMapInfo->VirtAddr + pMapInfo->MemSize - 1U), &L3DescEnd);

                    for (L3DescID = L3DescStart; L3DescID <= L3DescEnd; L3DescID++) {
                        /* pL3Desc[L3DescID].OutputAddr = L3PageBase >> AMBA_TLB_L3_4KB_PAGE_ADDR_OFFSET; */
                        AmbaMisra_TypeCast64(&TmpL3Page, &L3PageBase);
                        L3Page.OutputAddrL = TmpL3Page.OutputAddrL;
                        L3Page.OutputAddrH = TmpL3Page.OutputAddrH;
                        L3PageBase += (1ULL << AMBA_TLB_L3_4KB_PAGE_ADDR_OFFSET);

                        AmbaMisra_TypeCast64(&pL3Desc[L3DescID], &L3Page);
                    }
                    if (pL3Desc != NULL) {
                        (void)AmbaRTSL_CacheFlushDataPtr(&pL3Desc[L3DescStart], (ULONG)((L3DescEnd - L3DescStart) + 1U) * (UINT32)sizeof(AMBA_TLB_VMSAV8_L3_s));
                    }
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
                }
#endif
            }
        }
    }

    return RetVal;
}

/**
 *  MMU_TlbSetupL2Block_64KB - Setup long-descriptor L2 block descriptor.
 *  @param[in] pPageTable Pointer to translation table
 *  @param[in] pMapInfo Pointer to user-supplied MMU map entry.
 *  @return error code
 */
static UINT32 MMU_TlbSetupL2Block_64KB(const AMBA_MMU_MAP_INFO64_s *pMapInfo)
{
    AMBA_TLB_VMSAV8_L1_TBL_s *pL1Table, TmpL1Table;

    AMBA_TLB_VMSAV8_L2_BLK_s L2Block = {0};
    AMBA_TLB_VMSAV8_L2_BLK_s TmpL2Block;

    const AMBA_TLB_VMSAV8_L1_s *pL1Desc;
    AMBA_TLB_VMSAV8_L2_s *pL2Desc;
    UINT64 L1DescStart, L1DescEnd;
    UINT64 L2DescStart, L2DescEnd;
    UINT64 L2BlockBase;
    UINT64 L2DescID;
    ULONG VirAddr, PhyAddr;
    UINT32 RetVal = MMU_ERR_NONE;
    AMBA_MMU_MAP_ATTR64_s MemAttrBits;

    AmbaMisra_TypeCast64(&MemAttrBits, &pMapInfo->MemAttr);

    L1DescStart = AMBA_TLB_GET_L1_64KB_DESC_ID(pMapInfo->VirtAddr);
    L1DescEnd   = AMBA_TLB_GET_L1_64KB_DESC_ID(pMapInfo->VirtAddr + pMapInfo->MemSize - 1U);
    L2BlockBase = pMapInfo->PhysAddr & AMBA_TLB_L2_64KB_BLOCK_BASE_MASK;
    L2DescStart = AMBA_TLB_GET_L2_64KB_DESC_ID(pMapInfo->VirtAddr);
    L2DescEnd = AMBA_TLB_GET_L2_64KB_DESC_ID(pMapInfo->VirtAddr + pMapInfo->MemSize - 1U);

    if ((L1DescStart > 0U) || (L1DescEnd > 0U) ||
        (L2DescStart >= AMBA_NUM_TLB_L2_64KB_DESC_PARTIAL) || (L2DescEnd >= AMBA_NUM_TLB_L2_64KB_DESC_PARTIAL)) {
        RetVal = MMU_ERR_ARG;
    } else {
        L2Block.DescType = AMBA_TLB_L2_DESC_BLOCK;

        L2Block.AttrIndex = (UINT8)MMU_CheckAttr(MemAttrBits.MemAttr);
        L2Block.NonSecure    = MemAttrBits.NonSecure;
        L2Block.AP           = MemAttrBits.S1AP;
        L2Block.Shareability = MemAttrBits.Shareability;
        L2Block.AccessFlag   = 1U;
        L2Block.NotGlobal    = MemAttrBits.NotGlobal;
        L2Block.Contiguous   = 0U;
        L2Block.PrivExeNever = MemAttrBits.PXN;
        L2Block.ExeNever     = MemAttrBits.UXN;

        pL1Desc = &pAmbaTlb64KB->L1Desc;
        AmbaMisra_TypeCast64(&pL1Table, &pL1Desc);

        if (pL1Table->DescType == AMBA_TLB_L1_DESC_TABLE) {
            /* Use the table base address described in L1 Table descriptor. */
            PhyAddr = (pL1Desc->Data & AMBA_TLB_L1_64KB_TABLE_BASE_MASK);
            (void)AmbaRTSL_MmuPhys64ToVirt64(PhyAddr, &VirAddr);
            AmbaMisra_TypeCast64(&pL2Desc, &VirAddr);
            (void)AmbaRTSL_CacheFlushDataPtr(pL2Desc, (UINT32)sizeof(*pL2Desc));
        } else {
            /* Setup a new L1 Table descriptor. */
            pL2Desc = pAmbaTlb64KB->L2Desc;
            MMU_ZeroMemory(pL2Desc, (UINT64)AMBA_NUM_TLB_L2_64KB_DESC_PARTIAL * sizeof(AMBA_TLB_VMSAV8_L2_s));
            (void)AmbaRTSL_CacheCleanDataPtr(pL2Desc, AMBA_NUM_TLB_L2_64KB_DESC_PARTIAL * (UINT32)sizeof(AMBA_TLB_VMSAV8_L2_s));

            AmbaMisra_TypeCast64(&VirAddr, &pL2Desc);
            (void)AmbaRTSL_MmuVirt64ToPhys64(VirAddr, &PhyAddr);
            /* pL1Table->L2TableAddr = PhyAddr >> AMBA_TLB_L1_64KB_TABLE_ADDR_OFFSET; */
            AmbaMisra_TypeCast64(&TmpL1Table, &PhyAddr);
            pL1Table->L2TableAddrL = TmpL1Table.L2TableAddrL;
            pL1Table->L2TableAddrH = TmpL1Table.L2TableAddrH;
            pL1Table->DescType = AMBA_TLB_L1_DESC_TABLE;
            (void)AmbaRTSL_CacheFlushDataPtr(pL1Table, (UINT32)sizeof(AMBA_TLB_VMSAV8_L1_TBL_s));
        }

        for (L2DescID = L2DescStart; L2DescID <= L2DescEnd; L2DescID++) {
            /* pL2Desc[L2DescID].OutputAddr = L2BlockBase >> AMBA_TLB_L2_64KB_BLOCK_ADDR_OFFSET; */
            AmbaMisra_TypeCast64(&TmpL2Block, &L2BlockBase);
            L2Block.OutputAddrL = TmpL2Block.OutputAddrL;
            L2Block.OutputAddrH = TmpL2Block.OutputAddrH;
            L2BlockBase += (1ULL << AMBA_TLB_L2_64KB_BLOCK_ADDR_OFFSET);

            AmbaMisra_TypeCast64(&pL2Desc[L2DescID], &L2Block);
        }
        (void)AmbaRTSL_CacheFlushDataPtr(&pL2Desc[L2DescStart], (ULONG)((L2DescEnd - L2DescStart) + 1U) * (UINT32)sizeof(AMBA_TLB_VMSAV8_L2_s));
    }

    return RetVal;
}

/**
 *  MMU_TlbSetupL3Page_64KB - Setup long-descriptor L3 page descriptor.
 *  @param[in] pPageTable Pointer to translation table
 *  @param[in] pMapInfo Pointer to user-supplied MMU map entry.
 *  @return error code
 */
static UINT32 MMU_TlbSetupL3Page_64KB(const AMBA_MMU_MAP_INFO64_s *pMapInfo)
{
    AMBA_TLB_VMSAV8_L1_TBL_s *pL1Table, TmpL1Table;
    AMBA_TLB_VMSAV8_L2_TBL_s *pL2Table, TmpL2Table;

    AMBA_TLB_VMSAV8_L3_PG_s L3Page = {0};
    AMBA_TLB_VMSAV8_L3_PG_s TmpL3Page;

    const AMBA_TLB_VMSAV8_L1_s *pL1Desc;
    const AMBA_TLB_VMSAV8_L2_s *pL2Desc;
    AMBA_TLB_VMSAV8_L3_s *pL3Desc;
    UINT64 L1DescStart, L1DescEnd;
    UINT64 L2DescStart, L2DescEnd;
    UINT64 L3DescStart, L3DescEnd;
    UINT64 L3PageBase;
    UINT64 L2DescID, L3DescID;
    ULONG VirAddr, PhyAddr;
    UINT32 RetVal = MMU_ERR_NONE;
    AMBA_MMU_MAP_ATTR64_s MemAttrBits;

    AmbaMisra_TypeCast64(&MemAttrBits, &pMapInfo->MemAttr);

    L1DescStart = AMBA_TLB_GET_L1_64KB_DESC_ID(pMapInfo->VirtAddr);
    L1DescEnd   = AMBA_TLB_GET_L1_64KB_DESC_ID(pMapInfo->VirtAddr + pMapInfo->MemSize - 1U);
    L2DescStart = AMBA_TLB_GET_L2_64KB_DESC_ID(pMapInfo->VirtAddr);
    L2DescEnd = AMBA_TLB_GET_L2_64KB_DESC_ID(pMapInfo->VirtAddr + pMapInfo->MemSize - 1U);
    L3PageBase  = pMapInfo->PhysAddr & AMBA_TLB_L3_64KB_PAGE_BASE_MASK;

    if ((L1DescStart > 0U) || (L1DescEnd > 0U) ||
        (L2DescStart >= AMBA_NUM_TLB_L2_64KB_DESC_PARTIAL) || (L2DescEnd >= AMBA_NUM_TLB_L2_64KB_DESC_PARTIAL)) {
        RetVal = MMU_ERR_ARG;
    } else {
        L3Page.DescType = AMBA_TLB_L3_DESC_PAGE;

        L3Page.AttrIndex = (UINT8)MMU_CheckAttr(MemAttrBits.MemAttr);
        L3Page.NonSecure     = MemAttrBits.NonSecure;
        L3Page.AP            = MemAttrBits.S1AP;
        L3Page.Shareability  = MemAttrBits.Shareability;
        L3Page.AccessFlag    = 1U;
        L3Page.NotGlobal     = MemAttrBits.NotGlobal;
        L3Page.Contiguous    = 0U;
        L3Page.PrivExeNever  = MemAttrBits.PXN;
        L3Page.ExeNever      = MemAttrBits.UXN;

        pL1Desc = &pAmbaTlb64KB->L1Desc;
        AmbaMisra_TypeCast64(&pL1Table, &pL1Desc);

        if (pL1Table->DescType == AMBA_TLB_L1_DESC_TABLE) {
            /* Use the table base address described in L1 Table descriptor. */
            PhyAddr = (pL1Desc->Data & AMBA_TLB_L1_64KB_TABLE_BASE_MASK);
            (void)AmbaRTSL_MmuPhys64ToVirt64(PhyAddr, &VirAddr);
            AmbaMisra_TypeCast64(&pL2Desc, &VirAddr);
            (void)AmbaRTSL_CacheFlushDataPtr(pL2Desc, (UINT32)sizeof(AMBA_TLB_VMSAV8_L2_s));
        } else {
            /* Setup a new L1 Table descriptor. */
            pL2Desc = pAmbaTlb64KB->L2Desc;
            MMU_ZeroMemory(pL2Desc, (UINT64)AMBA_NUM_TLB_L2_64KB_DESC_PARTIAL * sizeof(AMBA_TLB_VMSAV8_L2_s));
            (void)AmbaRTSL_CacheCleanDataPtr(pL2Desc, AMBA_NUM_TLB_L2_64KB_DESC_PARTIAL * (UINT32)sizeof(AMBA_TLB_VMSAV8_L2_s));

            AmbaMisra_TypeCast64(&VirAddr, &pL2Desc);
            (void)AmbaRTSL_MmuVirt64ToPhys64(VirAddr, &PhyAddr);
            /* pL1Table->L2TableAddr = PhyAddr >> AMBA_TLB_L1_64KB_TABLE_ADDR_OFFSET; */
            AmbaMisra_TypeCast64(&TmpL1Table, &PhyAddr);
            pL1Table->L2TableAddrL = TmpL1Table.L2TableAddrL;
            pL1Table->L2TableAddrH = TmpL1Table.L2TableAddrH;
            pL1Table->DescType = AMBA_TLB_L1_DESC_TABLE;
            (void)AmbaRTSL_CacheFlushDataPtr(pL1Table, (UINT32)sizeof(AMBA_TLB_VMSAV8_L1_TBL_s));
        }

        for (L2DescID = L2DescStart; L2DescID <= L2DescEnd; L2DescID++) {
            pL2Desc = &pAmbaTlb64KB->L2Desc[L2DescID];
            AmbaMisra_TypeCast64(&pL2Table, &pL2Desc);

            if (pL2Table->DescType == AMBA_TLB_L2_DESC_BLOCK) {
                /* This region is already mapped by L2 Block descriptor. */
                break;
            } else if (pL2Table->DescType == AMBA_TLB_L2_DESC_TABLE) {
                /* Use the table base address described in L2 Table descriptor. */
                PhyAddr = (pL2Desc->Data & AMBA_TLB_L2_64KB_TABLE_BASE_MASK);
                (void)AmbaRTSL_MmuPhys64ToVirt64(PhyAddr, &VirAddr);
                AmbaMisra_TypeCast64(&pL3Desc, &VirAddr);
                (void)AmbaRTSL_CacheFlushDataPtr(&pL3Desc, (UINT32)sizeof(AMBA_TLB_VMSAV8_L3_s *));
            } else {
                /* Setup a new L2 Table descriptor. */
                pL3Desc = pAmbaTlb64KB->L3Desc[L2DescID];
                MMU_ZeroMemory(pL3Desc, (UINT64)AMBA_NUM_TLB_L3_64KB_DESC * sizeof(AMBA_TLB_VMSAV8_L3_s));
                (void)AmbaRTSL_CacheCleanDataPtr(pL3Desc, AMBA_NUM_TLB_L3_64KB_DESC * (UINT32)sizeof(AMBA_TLB_VMSAV8_L3_s));

                AmbaMisra_TypeCast64(&VirAddr, &pL3Desc);
                (void)AmbaRTSL_MmuVirt64ToPhys64(VirAddr, &PhyAddr);
                /* pL2Table->L3TableAddr = PhyAddr >> AMBA_TLB_L2_64KB_TABLE_ADDR_OFFSET; */
                AmbaMisra_TypeCast64(&TmpL2Table, &PhyAddr);
                pL2Table->L3TableAddrL = TmpL2Table.L3TableAddrL;
                pL2Table->L3TableAddrH = TmpL2Table.L3TableAddrH;
                pL2Table->DescType = AMBA_TLB_L2_DESC_TABLE;
                (void)AmbaRTSL_CacheFlushDataPtr(pL2Table, (UINT32)sizeof(AMBA_TLB_VMSAV8_L2_TBL_s));
            }

            if (L2DescID == L2DescStart) {
                L3DescStart = AMBA_TLB_GET_L3_64KB_DESC_ID(pMapInfo->VirtAddr);
            } else {
                L3DescStart = 0U;
            }

            if (L2DescID == L2DescEnd) {
                L3DescEnd = AMBA_TLB_GET_L3_64KB_DESC_ID(pMapInfo->VirtAddr + pMapInfo->MemSize - 1U);
            } else {
                L3DescEnd = AMBA_NUM_TLB_L3_64KB_DESC - 1U;
            }

            for (L3DescID = L3DescStart; L3DescID <= L3DescEnd; L3DescID++) {
                /* pL3Desc[L3DescID].OutputAddr = L3PageBase >> AMBA_TLB_L3_4KB_PAGE_ADDR_OFFSET; */
                AmbaMisra_TypeCast64(&TmpL3Page, &L3PageBase);
                L3Page.OutputAddrL = TmpL3Page.OutputAddrL;
                L3Page.OutputAddrH = TmpL3Page.OutputAddrH;
                L3PageBase += (1ULL << AMBA_TLB_L3_64KB_PAGE_ADDR_OFFSET);

                AmbaMisra_TypeCast64(&pL3Desc[L3DescID], &L3Page);
            }
            (void)AmbaRTSL_CacheFlushDataPtr(&pL3Desc[L3DescStart], (ULONG)((L3DescEnd - L3DescStart) + 1U) * (UINT32)sizeof(AMBA_TLB_VMSAV8_L3_s));
        }
    }

    return RetVal;
}

/**
 *  MMU_TlbSetup - Setup MMU page table according to user-supplied MMU map entry.
 *  @param[in] pMapInfo Pointer to user-supplied MMU map entry.
 *  @return error code
 */
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
static UINT32 MMU_TlbSetup(const AMBA_MMU_MAP_INFO64_s *pMapInfo)
{
    UINT64 TCR;
    UINT32 RetVal = MMU_ERR_NONE;
    register UINT32 CurrentEL;
    AMBA_MMU_MAP_ATTR64_s MemAttrBits;

#ifndef AMBA_KAL_NO_SMP
    AmbaMisra_TypeCast64(&MemAttrBits, &pMapInfo->MemAttr);
#endif

    CurrentEL = AmbaASM_ReadCurrentEL();
    CurrentEL = (CurrentEL >> 2U) & 0x3U;

    TCR = AmbaASM_ReadTlbBaseCtrl(CurrentEL);

    /* Check TG0 for granule size */
    switch ((TCR >> 14ULL) & 3ULL) {
    case 0U:    // 4KB
        if (MemAttrBits.DescType == AMBA_MMU_MAP_ATTR_L1_BLOCK) {
            RetVal = MMU_TlbSetupL1Block_4KB(pMapInfo);
        } else if (MemAttrBits.DescType == AMBA_MMU_MAP_ATTR_L2_BLOCK) {
            RetVal = MMU_TlbSetupL2Block_4KB(pMapInfo);
        } else if (MemAttrBits.DescType == AMBA_MMU_MAP_ATTR_L3_PAGE) {
            RetVal = MMU_TlbSetupL3Page_4KB(pMapInfo);
        } else {
            RetVal = MMU_ERR_ARG;
        }
        break;

    case 1U:    // 64KB
        if (MemAttrBits.DescType == AMBA_MMU_MAP_ATTR_L1_BLOCK) {
            RetVal = MMU_ERR_ARG;   /* 64KB granule does not support L1 block translation */
        } else if (MemAttrBits.DescType == AMBA_MMU_MAP_ATTR_L2_BLOCK) {
            RetVal = MMU_TlbSetupL2Block_64KB(pMapInfo);
        } else if (MemAttrBits.DescType == AMBA_MMU_MAP_ATTR_L3_PAGE) {
            RetVal = MMU_TlbSetupL3Page_64KB(pMapInfo);
        } else {
            RetVal = MMU_ERR_ARG;
        }
        break;

    default:
        /* 16KB */
        break;
    }

    return RetVal;
}
#else
static UINT32 MMU_TlbSetup(AMBA_MMU_MAP_INFO64_s *pMapInfo)
{
    UINT64 TCR;
    UINT32 RetVal = MMU_ERR_NONE;
    register UINT32 CurrentEL;
    AMBA_MMU_MAP_ATTR64_s MemAttrBits;

#ifndef AMBA_KAL_NO_SMP
    AmbaMisra_TypeCast64(&MemAttrBits, &pMapInfo->MemAttr);
    MemAttrBits.Shareability = 2U;  /* Make cacheable region as outer shareable. (Device and non-cacheable regions are already outer shareable.) */
    AmbaMisra_TypeCast64(&pMapInfo->MemAttr, &MemAttrBits);
#endif

    CurrentEL = AmbaASM_ReadCurrentEL();
    CurrentEL = (CurrentEL >> 2U) & 0x3U;

    TCR = AmbaASM_ReadTlbBaseCtrl(CurrentEL);

    /* Check TG0 for granule size */
    switch ((TCR >> 14ULL) & 3ULL) {
    case 0U:    // 4KB
        if (MemAttrBits.DescType == AMBA_MMU_MAP_ATTR_L1_BLOCK) {
            RetVal = MMU_TlbSetupL1Block_4KB(pMapInfo);
        } else if (MemAttrBits.DescType == AMBA_MMU_MAP_ATTR_L2_BLOCK) {
            RetVal = MMU_TlbSetupL2Block_4KB(pMapInfo);
        } else if (MemAttrBits.DescType == AMBA_MMU_MAP_ATTR_L3_PAGE) {
            RetVal = MMU_TlbSetupL3Page_4KB(pMapInfo);
        } else {
            RetVal = MMU_ERR_ARG;
        }
        break;

    case 1U:    // 64KB
        if (MemAttrBits.DescType == AMBA_MMU_MAP_ATTR_L1_BLOCK) {
            RetVal = MMU_ERR_ARG;   /* 64KB granule does not support L1 block translation */
        } else if (MemAttrBits.DescType == AMBA_MMU_MAP_ATTR_L2_BLOCK) {
            RetVal = MMU_TlbSetupL2Block_64KB(pMapInfo);
        } else if (MemAttrBits.DescType == AMBA_MMU_MAP_ATTR_L3_PAGE) {
            RetVal = MMU_TlbSetupL3Page_64KB(pMapInfo);
        } else {
            RetVal = MMU_ERR_ARG;
        }
        break;

    default:
        /* 16KB */
        break;
    }

    return RetVal;
}
#endif

/**
 *  AmbaRTSL_MmuVirt64ToPhys64 - Convert virtual address to physical address.
 *  @param[in] VirtAddr virtual address
 *  @param[out] pPhysAddr pointer that store tranfered address
 *  @return error code
 */
UINT32 AmbaRTSL_MmuVirt64ToPhys64(ULONG VirtAddr, ULONG *pPhysAddr)
{
    const AMBA_MMU_MAP_INFO64_s *pMapInfo = AmbaMmuMmioMap;
    UINT32 NumRegion = AMBA_MMU_NUM_MMIO_REGION;
    UINT32 i, RetVal = MMU_ERR_NOTLB;
    UINT32 SysCtrlReg = AmbaRTSL_CpuReadSysCtrl();

    if (pPhysAddr == NULL) {
        RetVal = MMU_ERR_ARG;
    } else {
        /* search in ioMap first. */
        for (i = 0U; i < NumRegion; i++) {
            if ((VirtAddr >= pMapInfo->VirtAddr) &&
                (VirtAddr < (pMapInfo->VirtAddr + pMapInfo->MemSize))) {
                *pPhysAddr = (VirtAddr - pMapInfo->VirtAddr) + pMapInfo->PhysAddr;
                RetVal = MMU_ERR_NONE;
                break;
            }
            pMapInfo++;
        }

        if (RetVal == MMU_ERR_NOTLB) {
            /* search failed in ioMap and then search in user map. */
            pMapInfo = AmbaMmuUserConfig.pMemMap;
            NumRegion = (UINT32)AmbaMmuUserConfig.NumMemMap;

            if (pMapInfo != NULL) {
                for (i = 0U; i < NumRegion; i++) {
                    if ((VirtAddr >= pMapInfo->VirtAddr) &&
                        (VirtAddr < (pMapInfo->VirtAddr + pMapInfo->MemSize))) {
                        *pPhysAddr = (VirtAddr - pMapInfo->VirtAddr) + pMapInfo->PhysAddr;
                        RetVal = MMU_ERR_NONE;
                        break;
                    }
                    pMapInfo++;
                }
            }
        }
    }

    /* Although the TLB traversal is done, we still inform caller about the MMU is disabled. */
    if ((RetVal == MMU_ERR_NONE) && ((SysCtrlReg & 0x1U) == 0U)) {
        RetVal = MMU_ERR_NOMMU;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_MmuPhys64ToVirt64 - Convert physical address to virtual address.
 *  @param[in] PhysAddr physical address
 *  @param[out] pVirtAddr pointer that store tranfered address
 *  @return error code
 */
UINT32 AmbaRTSL_MmuPhys64ToVirt64(ULONG PhysAddr, ULONG *pVirtAddr)
{
    const AMBA_MMU_MAP_INFO64_s *pMapInfo = AmbaMmuMmioMap;
    UINT32 NumRegion = AMBA_MMU_NUM_MMIO_REGION;
    UINT32 i, RetVal = MMU_ERR_NOTLB;
    UINT32 SysCtrlReg = AmbaRTSL_CpuReadSysCtrl();

    if (pVirtAddr == NULL) {
        RetVal = MMU_ERR_ARG;
    } else {
        /* search in ioMap first. */
        for (i = 0U; i < NumRegion; i++) {
            if ((PhysAddr >= pMapInfo->PhysAddr) &&
                (PhysAddr < (pMapInfo->PhysAddr + pMapInfo->MemSize))) {
                *pVirtAddr = (PhysAddr - pMapInfo->PhysAddr) + pMapInfo->VirtAddr;
                RetVal = MMU_ERR_NONE;
                break;
            }
            pMapInfo++;
        }

        if (RetVal == MMU_ERR_NOTLB) {
            /* search failed in ioMap and then search in user map. */
            pMapInfo = AmbaMmuUserConfig.pMemMap;
            NumRegion = (UINT32)AmbaMmuUserConfig.NumMemMap;

            if (pMapInfo != NULL) {
                for (i = 0U; i < NumRegion; i++) {
                    if ((PhysAddr >= pMapInfo->PhysAddr) &&
                        (PhysAddr < (pMapInfo->PhysAddr + pMapInfo->MemSize))) {
                        *pVirtAddr = (PhysAddr - pMapInfo->PhysAddr) + pMapInfo->VirtAddr;
                        RetVal = MMU_ERR_NONE;
                        break;
                    }
                    pMapInfo++;
                }
            }
        }
    }

    /* Although the TLB traversal is done, we still inform caller about the MMU is disabled. */
    if ((RetVal == MMU_ERR_NONE) && ((SysCtrlReg & 0x1U) == 0U)) {
        RetVal = MMU_ERR_NOMMU;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_MmuQueryMapInfo64 - The function performs the translation of virtual memory addresses to physical addresses.
 *  @param[in] VirtAddr virtual address
 *  @param[out] pMemMap pointer to the mapping and attributes of a memory region
 *  @return error code
 */
UINT32 AmbaRTSL_MmuQueryMapInfo64(UINT64 VirtAddr, AMBA_MMU_MAP_INFO64_s * pMemMap)
{
    const AMBA_MMU_MAP_INFO64_s *pMapInfo = AmbaMmuMmioMap;
    UINT32 NumRegion = AMBA_MMU_NUM_MMIO_REGION;
    UINT32 i, RetVal = MMU_ERR_NOTLB;
    UINT32 SysCtrlReg = AmbaRTSL_CpuReadSysCtrl();

    if (pMemMap == NULL) {
        RetVal = MMU_ERR_ARG;
    } else {
        /* search in ioMap first. */
        for (i = 0U; i < NumRegion; i++) {
            if ((VirtAddr >= pMapInfo->VirtAddr) &&
                (VirtAddr < (pMapInfo->VirtAddr + pMapInfo->MemSize))) {
                pMemMap->VirtAddr = pMapInfo->VirtAddr;
                pMemMap->PhysAddr = pMapInfo->PhysAddr;
                pMemMap->MemSize = pMapInfo->MemSize;
                pMemMap->MemAttr = pMapInfo->MemAttr;
                RetVal = MMU_ERR_NONE;
                break;
            }
            pMapInfo++;
        }

        if (RetVal == MMU_ERR_NOTLB) {
            /* search failed in ioMap and then search in user map. */
            pMapInfo = AmbaMmuUserConfig.pMemMap;
            NumRegion = (UINT32)AmbaMmuUserConfig.NumMemMap;

            if (pMapInfo != NULL) {
                for (i = 0U; i < NumRegion; i++) {
                    if ((VirtAddr >= pMapInfo->VirtAddr) &&
                        (VirtAddr < (pMapInfo->VirtAddr + pMapInfo->MemSize))) {
                        pMemMap->VirtAddr = pMapInfo->VirtAddr;
                        pMemMap->PhysAddr = pMapInfo->PhysAddr;
                        pMemMap->MemSize = pMapInfo->MemSize;
                        pMemMap->MemAttr = pMapInfo->MemAttr;
                        RetVal = MMU_ERR_NONE;
                        break;
                    }
                    pMapInfo++;
                }
            }
        }
    }

    /* Although the TLB traversal is done, we still inform caller about the MMU is disabled. */
    if ((RetVal == MMU_ERR_NONE) && ((SysCtrlReg & 0x1U) == 0U)) {
        RetVal = MMU_ERR_NOMMU;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_MmuCpuSetup - Initialise the processor for turning the MMU on.
 *  @param[in] HighVectFlag A flag to indicate high vector or not
 *  @return error code
 */
UINT32 AmbaRTSL_MmuCpuSetup(UINT8 HighVectFlag)
{
    UINT32 MAIR;
    UINT32 TCR32;
    UINT64 TCR64;
    UINT64 TTBR;
    UINT64 VBAR;
    const AMBA_TLB_GRANULE_4KB_s *pPageTblBase = &AmbaMmuPageTable;
    register UINT32 CurrentEL;

    AmbaMisra_TypeCast64(&TTBR, &pPageTblBase);
#if 0 //defined(CONFIG_XEN_SUPPORT) && !defined(AMBA_FWPROG)
    {
        extern UINT64 *vectors;
        VBAR =  (UINT64)&vectors;

        (void)HighVectFlag;
    }
#else
    if (HighVectFlag != 0U) {
        VBAR = AMBA_DRAM_EXCEPTION_VIRT_ADDR;
    } else {
        VBAR = 0x0ULL;
    }
#endif

    MAIR = (0x00U);            /* Device-nGnRnE (ARMv7's Strongly-ordered) */
    MAIR |= ((UINT32)0x04U << 0x08U); /* Device-nGnRE (ARMv7's Device) */
    MAIR |= ((UINT32)0x44U << 0x10U); /* Normal Memory, Outer & Inner Non-Cacheable */
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    MAIR |= ((UINT32)0xffU << 0x18U); /* Normal Memory, Outer & Inner Write-back non-transient */
#else
    MAIR |= ((UINT32)0x77U << 0x18U); /* Normal Memory, Outer & Inner Write-back non-transient */
#endif

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    TCR32 = (0x20000U);         /* Intermediate Physical Address Size is 32 bits, 4GB */
    TCR32 |= (0x3000U);         /* Outer Shareable */
    TCR32 |= (0x500U);          /* Outer & Inner Write-Back Write-Allocate Cacheable */
    TCR32 |= (0x19U);           /* Make TTBR0 points to level 1 (4KB granule) and level 2 (64KB granule) */

#else
    TCR32 = (0x0U);            /* Intermediate Physical Address Size is 32 bits, 4GB */
#if defined(CONFIG_TRANSLATION_GRANULE_64KB)
    TCR32 |= ((UINT32)0x2U << 14U);   /* Translation granule size is 64KB */
#endif
#ifndef AMBA_KAL_NO_SMP
    TCR32 |= ((UINT32)0x2U << 12U);   /* Outer Shareable */
#endif
    TCR32 |= ((UINT32)0x5U << 8U);    /* Outer & Inner Write-Back Write-Allocate Cacheable */
    TCR32 |= (32U);            /* Make TTBR0 points to level 1 (4KB granule) and level 2 (64KB granule) */
#endif

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    TCR64 = (0x200000000ULL);   /* Intermediate Physical Address Size is 40 bits, 1TB */
#if defined(CONFIG_TRANSLATION_GRANULE_64KB)
    TCR64 |= (0x3ULL << 30U);   /* Translation granule size is 64KB */
    TCR64 |= (0x2ULL << 14U);   /* Translation granule size is 64KB */
#else /* CA76 doesn't support 16KB granule. Use 4KB granule by default. */
    TCR64 |= (0x2ULL << 30U);   /* Translation granule size is 4KB */
#endif
    /* Shareability attribute for memory associated with translation table walks using TTBR1_EL1 */
    TCR64 |= (0x3ULL << 28U);   /* Outer Shareable */
    /* Shareability attribute for memory associated with translation table walks using TTBR0_EL1. */
    TCR64 |= (0x3ULL << 12U);
    TCR64 |= (0x5ULL << 24U);   /* Outer & Inner Write-Back Write-Allocate Cacheable */
    TCR64 |= (0x5ULL << 8U);    /* Outer & Inner Write-Back Write-Allocate Cacheable */
    TCR64 |= (0x19ULL);         /* Make TTBR0 points to level 1 (4KB granule) and level 2 (64KB granule) */
#else
    TCR64 = (0x0ULL);           /* Intermediate Physical Address Size is 32 bits, 4GB */
#if defined(CONFIG_TRANSLATION_GRANULE_64KB)
    TCR64 |= (0x3ULL << 30U);   /* Translation granule size is 64KB */
    TCR64 |= (0x2ULL << 14U);   /* Translation granule size is 64KB */
#else /* CA53 doesn't support 16KB granule. Use 4KB granule by default. */
    TCR64 |= (0x2ULL << 30U);   /* Translation granule size is 4KB */
#endif
#ifndef AMBA_KAL_NO_SMP
    TCR64 |= (0x2ULL << 28U);   /* Outer Shareable */
    TCR64 |= (0x2ULL << 12U);   /* Outer Shareable */
#endif
    TCR64 |= (0x5ULL << 24U);   /* Outer & Inner Write-Back Write-Allocate Cacheable */
    TCR64 |= (0x5ULL << 8U);    /* Outer & Inner Write-Back Write-Allocate Cacheable */
    TCR64 |= (32ULL);           /* Make TTBR0 points to level 1 (4KB granule) and level 2 (64KB granule) */
#endif

    CurrentEL = AmbaASM_ReadCurrentEL();
    CurrentEL = (CurrentEL >> 2U) & 0x3U;

    AmbaASM_WriteVectorBaseAddr(CurrentEL, VBAR);

    /* Set/prepare TCR and TTBR */
    AmbaASM_WriteTlbBaseCtrl(CurrentEL, TCR64, TCR32);
    AmbaASM_WriteTlbBase0(CurrentEL, TTBR);
    /* Memory region attributes */
    AmbaASM_WriteMemAttrIndir(CurrentEL, (UINT64)MAIR);

    return MMU_ERR_NONE;
}

/**
 *  AmbaRTSL_MmuTlbInvalidateAll - Invalidate all entries in the TLB.
 */
void AmbaRTSL_MmuTlbInvalidateAll(void)
{
    register UINT32 CurrentEL;

    CurrentEL = AmbaASM_ReadCurrentEL();
    CurrentEL = (CurrentEL >> 2U) & 0x3U;

    AmbaTLB_InvTlbAll(CurrentEL);
}

/**
 *  AmbaRTSL_MmuTlbConfig - Initialize translation table by descriptors
 */
void AmbaRTSL_MmuTlbConfig(void)
{
#if !defined(CONFIG_XEN_SUPPORT) || defined(AMBA_FWPROG)
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    const AMBA_MMU_MAP_INFO64_s *pMapInfo;
#else
    AMBA_MMU_MAP_INFO64_s *pMapInfo;
#endif
    UINT32 i, NumRegion;
    UINT32 SysCtrlReg;

    if (AmbaMmuUserConfig.UserPreConfigEntry != NULL) {
        (void)AmbaMmuUserConfig.UserPreConfigEntry();   /* invoke the init function of RAM map */
    }

    MMU_MmioMapInit(&AmbaMmuMmioMap[0]);

    if (AmbaMmuUserConfig.VectorAddr == 0x0U) {
        /* 0x00000000U: Low vector address */
        (void)AmbaRTSL_MmuCpuSetup(0U);
    } else {
        /* 0xFFFF0000U: High vector address */
        (void)AmbaRTSL_MmuCpuSetup(1U);
    }

    /* Invalidate TLB and data cache */
    SysCtrlReg = AmbaRTSL_CpuReadSysCtrl();

    AmbaCache_InstInvAll();
    if (0x0U == (SysCtrlReg & 0x1U)) {
        MMU_TlbReset();
    }

    /* Setup RAM address map */
    pMapInfo = AmbaMmuUserConfig.pMemMap;               /* pointer to the base address of Memory Map Info */
    NumRegion = (UINT32)AmbaMmuUserConfig.NumMemMap;
    if (pMapInfo != NULL) {
        for (i = 0U; i < NumRegion; i++) {
            (void)MMU_TlbSetup(pMapInfo);
            pMapInfo++;
        }
    }

    /* Setup MMIO address map */
    pMapInfo = AmbaMmuMmioMap;
    NumRegion = AMBA_MMU_NUM_MMIO_REGION;
    for (i = 0U; i < NumRegion; i++) {
        (void)MMU_TlbSetup(pMapInfo);
        pMapInfo++;
    }
#endif

    AMBA_DMB();
}

/**
 *  AmbaRTSL_MmuTlbConfigPreOSInit - Initialize translation table by descriptors
 */
void AmbaRTSL_MmuTlbConfigPreOSInit(void)
{
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    const AMBA_MMU_MAP_INFO64_s *pMapInfo;
#else
    AMBA_MMU_MAP_INFO64_s *pMapInfo;
#endif
    UINT32 i, NumRegion;

    if (AmbaMmuUserConfigPreOSInit.UserPreConfigEntry != NULL) {
        (void)AmbaMmuUserConfigPreOSInit.UserPreConfigEntry();   /* invoke the init function of RAM map */
    }

    MMU_MmioMapInit(&AmbaMmuMmioMap[0]);

    if (AmbaMmuUserConfigPreOSInit.VectorAddr == 0x0U) {
        /* 0x00000000U: Low vector address */
        (void)AmbaRTSL_MmuCpuSetup(0U);
    } else {
        /* 0xFFFF0000U: High vector address */
        (void)AmbaRTSL_MmuCpuSetup(1U);
    }

    MMU_TlbReset();

    /* Setup RAM address map */
    pMapInfo = AmbaMmuUserConfigPreOSInit.pMemMap;               /* pointer to the base address of Memory Map Info */
    NumRegion = (UINT32)AmbaMmuUserConfigPreOSInit.NumMemMap;
    if (pMapInfo != NULL) {
        for (i = 0U; i < NumRegion; i++) {
            (void)MMU_TlbSetup(pMapInfo);
            pMapInfo++;
        }
    }

    /* Setup MMIO address map */
    pMapInfo = AmbaMmuMmioMap;
    NumRegion = AMBA_MMU_NUM_MMIO_REGION;
    for (i = 0U; i < NumRegion; i++) {
        (void)MMU_TlbSetup(pMapInfo);
        pMapInfo++;
    }

    AMBA_DMB();
}

#if defined(CONFIG_XEN_SUPPORT) && !defined(AMBA_FWPROG)
void AmbaRTSL_MmuTlbConfigXenInit(void)
{
    AMBA_MMU_MAP_INFO64_s *pMapInfo;
    UINT32 i, NumRegion;

    if (AmbaMmuUserConfigPreOSInit.UserPreConfigEntry != NULL) {
        (void)AmbaMmuUserConfigPreOSInit.UserPreConfigEntry();   /* invoke the init function of RAM map */
    }

    if (AmbaMmuUserConfigPreOSInit.VectorAddr == 0x0U) {
        /* 0x00000000U: Low vector address */
        (void)AmbaRTSL_MmuCpuSetup(0U);
    } else {
        /* 0xFFFF0000U: High vector address */
        (void)AmbaRTSL_MmuCpuSetup(1U);
    }

    MMU_TlbReset();

    /* Setup RAM address map */
    pMapInfo = AmbaMmuUserConfigPreOSInit.pMemMap;               /* pointer to the base address of Memory Map Info */
    NumRegion = (UINT32)AmbaMmuUserConfigPreOSInit.NumMemMap;
    if (pMapInfo != NULL) {
        for (i = 0U; i < NumRegion; i++) {
            (void)MMU_TlbSetup(pMapInfo);
            pMapInfo++;
        }
    }

    AMBA_DMB();
}
#endif // defined(CONFIG_XEN_SUPPORT) && !defined(AMBA_FWPROG)

